/*******************************************************************
|***
|***         File: guesser.c
|***       Author: Bruno Maximilian Schulze, Gregory Grefenstette
|***      Purpose: language identification tool
|***      Created:
|***     Modified:
|***    Copyright: Xerox Corporation
|***
\*******************************************************************/
/* Copyright (c) 1996-97 by the Xerox Corporation.  All rights reserved.
   Copyright protection claimed includes all forms and matters of
   copyrightable material and information now allowed by statutory or
   judicial law or hereinafter granted, including without limitation,
   material generated from the software programs which are displayed 
   on the screen such as icons, screen display looks, etc.
   */


#include<stdio.h>
#include<stdlib.h>

#include "guess_lib.h"


#define MAXLINELEN 2000000      /* max. length of input */
#define VERSION "v1.300 (August 1999)"

/* strcmp(s1, s2) yields 0 iff s1 == s2 */
#define STREQ(a,b) (((a) == (b)) || \
                    ((a) && (b) && (strcmp((a), (b)) == 0)))


extern int MaxShowLangs;

int MaxLines       = 0;
int summarizelangs = 0;
int silent         = 0;
int SuppVar        = 1;
int ShowProbTbl    = 0;
int PercentProb    = 0;
int max_lang;

guesser_id_type    GuessType = trig_shortword;
guesser_lang_type  LangType  = iso639_2;


void
syntax(void) {

  fprintf(stderr, "guesslang (c) 1996-97 Xerox Corporation\n");
  fprintf(stderr, "created by Bruno Maximilian Schulze and Gregory Grefenstette\n");
  fprintf(stderr, "usage: guesser [options]\n");
  fprintf(stderr, "   -f <format>\t  prints language in one of the following formats:\n");
  fprintf(stderr, "           \t\t iso639         \t ISO 639 two-letter code\n");
  fprintf(stderr, "           \t\t iso639_2       \t ISO 639-2 three-letter code (default)\n");
  fprintf(stderr, "           \t\t full           \t full language name (in English)\n");
  fprintf(stderr, "   -s      \t  silence options; prints only language code as result\n");
  fprintf(stderr, "   -S      \t  reads text line by line and makes guess on whole input\n");
  fprintf(stderr, "   -p <prob>\t  sets minimum (log.) probability (default: %1.1f)\n", minprob);
  fprintf(stderr, "   -l <num>\t  stops process after <num> lines\n");
  fprintf(stderr, "   -w <num>\t  stops process after <num> words\n");
  fprintf(stderr, "   -U      \t  makes the guesser sensitive to diacritic characters\n");
  fprintf(stderr, "   -x <lang>\t  language will be ignored\n");
  fprintf(stderr, "   -X <lang>\t  only this language should be accepted\n");
  fprintf(stderr, "   -g <type>\t  selects the classification method\n");
  fprintf(stderr, "            \t\t trig          \t trigram lid\n");
  fprintf(stderr, "            \t\t shortword     \t shortword lid\n");
  fprintf(stderr, "            \t\t trig_shortword\t combined trigram and shortword lid (default)\n");
  fprintf(stderr, "   -d <num>\t  shows <num> best languages and probs (0 shows all)\n");
  fprintf(stderr, "   -P      \t  for -d: show percental probabilities\n");
  fprintf(stderr, "   -v      \t  shows version\n");
  fprintf(stderr, "   -h      \t  prints this text\n");
  fprintf(stderr, "Input is read from stdin; result is written to stdout\n");
  exit(1);
}


void
print_version(void) {
  
    extern void  print_avail_langs(int);
    
    fprintf(stderr, "guesslang %s: ", VERSION);
    print_avail_langs(LangType);
}


void
check_args(int argc, char *argv[]) {

  extern char   *optarg;
  extern int     optind;
  extern void    set_all_languages(int);
  int            c;
  int            languages_excl = 0;

  while((c = getopt(argc, argv, "f:hl:p:sSvw:x:X:g:Ud:P")) != EOF)
      switch(c) {
      case 's':
          silent = 1;
          break;
      case 'S':
          summarizelangs = 1;
          break;
      case 'p':
          minprob = -atof(optarg);
          break;
      case 'l':
          MaxLines = atoi(optarg);
          break;
      case 'v':
          print_version();
          exit(0);
      case 'w':
          stop_guesser(atoi(optarg));
          break;
      case 'U':
          SuppVar = 0;
          break;
      case 'f':
          if (STREQ(optarg, "iso639"))
              LangType = iso639;
          else if (STREQ(optarg, "iso639_2"))
              LangType = iso639_2;
          else if (STREQ(optarg, "full"))
              LangType = full;
          else {
              fprintf(stderr, "unrecognized language type: %s\n", optarg);
              syntax();
          }
          break;
      case 'x':
          if (set_language(optarg, 0))
              languages_excl = 1;
          else fprintf(stderr, "invalid language (%s)\n", optarg);
          break;
      case 'X':
          if (!languages_excl)
              set_all_languages(0);
          if (!set_language(optarg, 1))
              fprintf(stderr, "invalid language (%s)\n", optarg);
          languages_excl = 1;
          break;
      case 'g':
          if (STREQ(optarg, "trig"))
              GuessType = trig;
          else if (STREQ(optarg, "shortword"))
              GuessType = shortword;
          else if (STREQ(optarg, "trig_shortword"))
              GuessType = trig_shortword;
          else {
              fprintf(stderr, "unrecognized guess type: %s\n", optarg);
              syntax();
          }
          break;
      case 'd':
          ShowProbTbl  = 1;
          MaxShowLangs = atoi(optarg);
          if (MaxShowLangs == 0)
              MaxShowLangs = max_lang;
          if (MaxShowLangs > max_lang || MaxShowLangs < 0) {
              fprintf(stderr, "-d %d: invalid argument\n", MaxShowLangs); 
              syntax();
          }
          break;
      case 'P':
          PercentProb = 1;
          break;
      case 'h':
      case '?':
          syntax();
      }

  if (argc - optind > 0) {
      fprintf(stderr, "no further arguments required\n");
      syntax();
  }
  if (MaxLines && !summarizelangs) {
      fprintf(stderr, "'-l' option requires '-S' option\n");
      exit(1);
  }

  if (languages_excl)
      reset_guesser();
  
}


void
print_language(const char *lang, const int line) {
    
    unsigned char  last;
    int            len;

    if (!silent) fputs("I GUESS ... ", stdout);
    if (LangType == full) 
        fputs(lang, stdout);
    else {
        len = strlen(lang);
        last = lang[len-1];
        if (SuppVar && (last == 'U' || last == 'E')) 
            len--;
        (void)fwrite(lang, sizeof(char), len, stdout);
    }
    if (!silent)
        if (summarizelangs)
            printf(" (%d/%d lines/words)\n", line, GuessWords);
        else
            printf(" (%d words)\n", GuessWords);
    else putchar('\n');

}


int
main (int argc, char *argv[]) {

    unsigned char  str[MAXLINELEN];
    register int   line = 0;
    char           res[20];
    extern void    show_sorted_probs(int, int, guesser_id_type);

    max_lang = init_guesser();
    check_args(argc, argv);

    while (fgets((char *)str, MAXLINELEN, stdin)) {
        if (MaxLines && line >= MaxLines) 
            break;
        ++line;
        if (summarizelangs)
            scan_line(str, GuessType);
        else {
            guess_language(str, res, LangType, GuessType);
            print_language(res, line);
            if (ShowProbTbl) show_sorted_probs(MaxShowLangs, PercentProb, GuessType);
        }
    }
    
    if (line) {
        if (summarizelangs) {
            do_guess(res, LangType, GuessType);
            print_language(res, line);
        }
        if (ShowProbTbl)
            show_sorted_probs(MaxShowLangs, PercentProb, GuessType);
        exit(0);
    } 
    exit(1);
}
