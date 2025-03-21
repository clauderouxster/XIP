/*******************************************************************
|***
|***         File: trainguesser.c
|***       Author: Bruno Maximilian Schulze
|***      Purpose: language guesser training tool
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


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "string2id.h"

#define MAXLINELEN 2000000      /* max. length of input */
#define TRIGS       200000      /* max. number of different trigrams */
#define VERSION "V1.200 (Fri Jan 24 17:27:32 MET 1997)"


typedef enum outp { c, awk, simple } output_t;


dict_t          *freqdict;
unsigned long   *freqhash;
unsigned long    MINFREQ   = 10;
unsigned long    MAXTRIGS  = TRIGS;
unsigned long    trignum   = 0;
unsigned long    N         = 0;
output_t         output    = c;
char             lang[5];
char            *WordFile  = NULL;
int              logprobs  = 0;
int              WordProbs = 0;


void 
fatal_error(const char *msg) {

//    extern int errno;
    
 //   if (errno)
  //      perror(msg);
   // else {
        fputs(msg, stderr);
        fputs("\n", stderr);
    //}
    exit(2);
}


void
syntax(void) {
    
    fprintf(stderr, "trainguesser (c) 1996-97 Xerox Corporation\n");
    fprintf(stderr, "created by Bruno Maximilian Schulze and Gregory Grefenstette\n");
    fprintf(stderr, "usage: guesser [options] language\n");
    fprintf(stderr, "   -b <num>\t  set threshold for trigram frequencies (default: %ld)\n", MINFREQ);
    fprintf(stderr, "   -n <num>\t  set new number of max. trigrams (default: %ld)\n", MAXTRIGS);
    fprintf(stderr, "   -a      \t  generate awk output (default: c output)\n");
    fprintf(stderr, "   -s      \t  generate output in table format\n");
    fprintf(stderr, "   -l      \t  print logarithmic probabilities\n");
    fprintf(stderr, "   -w <file>\t generate additional code for shortword probabilities (C output only)\n");
    fprintf(stderr, "   -v      \t  shows version\n");
    fprintf(stderr, "   -h      \t  prints this text\n");
    fprintf(stderr, "Input is read from stdin; output is written to stdout\n");
    exit(1);
}


void check_args(int ac, char *av[]) {
    
    extern char   *optarg;
    extern int     optind;
    int            c;
    
    while((c = getopt(ac, av, "hb:n:alsw:v")) != EOF)
        switch(c) {
        case 'b':
            MINFREQ = atol(optarg);
            break;
        case 'n':
            MAXTRIGS = atol(optarg);
            break;
        case 'a':
            output = awk;
            break;
        case 's':
            output = simple;
            break;
        case 'l':
            logprobs = 1;
            break;
        case 'w':
            WordFile  = (char *)strdup(optarg);
            WordProbs = 1;
            break;
        case 'v':
            fprintf(stderr, "trainguesser %s\n", VERSION);
            exit(0);
        case 'h':
        case '?':
            syntax();
        }
    if (optind == ac) syntax();
    
    strcpy(lang, av[ac - 1]);
}


void
storeprob(const unsigned char *word) {

    unsigned long  id;
    int            len = strlen((char *)word);
    register int   i, j;
    unsigned char *trig, *token;
    unsigned char  savec;
    
    /* chop off newline character at the end of a word. */
    if (word[len - 1] == '\n')
        len--;
    
    /* augment word by adding boundary markers ('_') */
    token = (unsigned char *)malloc(len + 3);
    assert(token);
    *token = '_';
    for (i = 0; i < len; i++)
        token[i+1] = word[i];
    token[len+1] = '_';
    token[len+2] = '\0';
    
    for(j = 0; j < len; j++) {
        trig = token + j;         /* set start position. */
        savec = *(token + j + 3); /* save char following the current trigram. */
        *(trig + 3) = '\0';       /* mark end of current trigram. */
        id = string2id(trig, freqdict, insert_alloc);
        if (id >= MAXTRIGS) fatal_error("max number of trigrams exceeded");
        freqhash[id]++;
        *(trig + 3) = savec;      /* restore the original string. */
    }
    free(token);
}


void
make_letter_trigrams(const unsigned char *line) {

    unsigned char *word;
    const char    *wordsep = " ";
    
    word = (unsigned char *)strtok((char *)line, wordsep);
    if (word) {
        
        do {
            storeprob(word);
            word = (unsigned char *)strtok(NULL, wordsep);
        } while (word);
    }
}


void
init(void) {

    freqdict = make_dict(2 * MAXTRIGS, 1);
    freqhash = (unsigned long *)calloc(MAXTRIGS, sizeof(unsigned long));
    assert(freqhash);
}


void
get_trig_num(void) {

    unsigned long  i;
    
    for (i = 0; i < freqdict->filled; i++) {
        N += freqhash[i];
        if (freqhash[i] > MINFREQ)
            trignum++;
    }
}


void
simple_output(void) {

    unsigned long i;
    double        p;
    
    for (i = 0; i < freqdict->filled; i++)
        if (freqhash[i] > MINFREQ) {
            p = (double)freqhash[i]/(double)N;
            printf("%3s\t%5ld\t%.9f\n", id2string(i, freqdict), freqhash[i], (logprobs)? log(p) : p);
        }
}


void
awk_output(void) {
    
    unsigned long i;
    double        p;
    
    puts("BEGIN {");
    
    for (i = 0; i < freqdict->filled; i++)
        if (freqhash[i] > MINFREQ) {
            p = (double)freqhash[i]/(double)N;
            printf("%s[\"%s\"] =  %.9f;\n", lang, id2string(i, freqdict), 
                   (logprobs)? log(p) : p);
        }
    puts("}");
}


/* write header consisting of copyright message, include directives and *
 * definitions.                                                         */
void
write_header(void) {

    printf("/* (c) 1996-97 Xerox Corp. */\n\n");
    printf("#include <stdlib.h>\n");
    printf("#include <stdio.h>\n\n");
    printf("#include \"languages.h\"\n");
    printf("#include \"string2id.h\"\n\n");
    printf("#define %s_MAX_TRIGs %ld\n\n", lang, trignum);
}


/* write language-specific hashing table.                             *
 * NOTE: the order in which the entries will be written is arbitrary. */
void
write_trighashtbl(void) {

    register unsigned long i, j;
    double                 p;

    printf("static const langhash_t %s_hash[%s_MAX_TRIGs] = {\n", lang, lang);
    
    /* initialization of the hashing table.                            *
     * first step is to find the first entry and write it -- without a *
     * (trailing) comma.                                               */
    for (j = 0; j < freqdict->filled; j++) {
        if (freqhash[j] > MINFREQ) {
            p = (double)freqhash[j]/(double)N;
            printf("  { \"%s\", %.9f }", id2string(j, freqdict), (logprobs)? log(p) : p);
            break;
        }
    }
    
    /* now write the remaining entries, separated by commas */
    for (i = j + 1; i < freqdict->filled; i++)
        if (freqhash[i] > MINFREQ) {
            p = (double)freqhash[i]/(double)N;
            printf(", { \"%s\", %.9f }", id2string(i, freqdict), (logprobs)? log(p) : p);
        }    
    printf("\n};\n\n");
}


/* declare and initialize the data structure for the short word *
 * probabilities.                                               */
int
write_shortwordhashtbl(void) {

    register unsigned long n = 0;
    double         swprob;
    FILE          *wfp;
    char           shrtwrd[6];
    
    if (!(wfp = fopen(WordFile, "r"))) {
        fprintf(stderr, "can't open short word file %s for language %s\n", WordFile, lang);
        exit(1);
    }
    if (fscanf(wfp, "%lf %s", &swprob, shrtwrd) != EOF) {
        printf("static const wordhash_t %s_wordhash[] = {\n", lang);
        n++;
        printf("  { \"%s\", %f }", shrtwrd, swprob);
        while(fscanf(wfp, "%lf %s", &swprob, shrtwrd) != EOF) {
            printf("  , { \"%s\", %f }", shrtwrd, swprob);
            n++;
        }
    
        printf("\n};\n\n");
    }
    fclose(wfp);
    return(n);
}


/* write initialization code. */
void
write_initcode(const int maxshortword) {

    printf("int\n");
    printf("init_%s(int lgno) {\n\n", lang);
    printf("    register int  i;\n");
    printf("    unsigned long cid;\n\n");
    printf("    if (%s_MAX_TRIGs) {\n", lang);
    printf("        languages[lgno].dict = make_dict(2 * %s_MAX_TRIGs, 0);\n", lang);
    printf("        languages[lgno].hash = (double *)malloc(sizeof(double) * %s_MAX_TRIGs);\n", lang);
    printf("        if (!languages[lgno].hash) return(1);\n");
    printf("        for (i = 0; i < %s_MAX_TRIGs; i++) {\n", lang);
    printf("           cid = string2id((char *)%s_hash[i].trig, languages[lgno].dict, insert_copy);\n", lang);
    printf("           if (cid == ~0) return(1);\n");
    printf("           languages[lgno].hash[cid] = %s_hash[i].prob;\n", lang);
    printf("        }\n");
    if (WordProbs) {
        printf("        languages[lgno].wdict = make_dict(2 * %d, 0);\n", maxshortword);
        printf("        languages[lgno].whash = (double *)malloc(sizeof(double) * %d);\n", maxshortword);
        printf("        for ( i = 0; i < %d; i++) {\n", maxshortword - 1);
        printf("           cid = string2id((char *)%s_wordhash[i].word, languages[lgno].wdict, insert_copy);\n", lang);
        printf("           if (cid == ~0) return(1);\n");
        printf("           languages[lgno].whash[cid] = %s_wordhash[i].prob;\n", lang);
        printf("        }\n");
    } else {
        printf("        languages[lgno].wdict = (dict_t *)NULL;\n");
        printf("        languages[lgno].whash = (double *)NULL;\n");
    }        
    printf("    } else languages[lgno].used = 0;\n\n");
    printf("    return(0);\n");
    printf("}\n");
}


void
c_output(void) {

    int maxshortword = 0;

    write_header();
    write_trighashtbl();
    if (WordProbs) maxshortword = write_shortwordhashtbl();
    write_initcode(maxshortword);
}


int
main (int argc, char *argv[]) {
    
    unsigned char  str[MAXLINELEN];
    
    check_args(argc, argv);
    init();
    
    while (fgets((char *)str, MAXLINELEN, stdin)) {
        make_letter_trigrams(str);
    }
    
    get_trig_num();
    
    if (output == c)
        c_output();
    else if (output == awk)
        awk_output();
    else
        simple_output();
    
    fprintf(stderr, "# diff. trigrams: %ld\n", freqdict->filled);
    exit(0);
}

