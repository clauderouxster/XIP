/*******************************************************************
|***
|***         File: guesser_lib.c
|***       Author: Bruno Maximilian Schulze
|***      Purpose: language guesser API (implementation module)
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

#include<locale.h>
#include<ctype.h>
#include<stdlib.h>
#include<stdio.h>
#include<limits.h>
#include<math.h>
#include<string.h>
#include<assert.h>

#include "string2id.h"
#include "languages.h"
#include "guess_lib.h"


/*+++++++++++++++++++++++++++++++++*
 + SOME GENERAL DEFINITIONS        +
 *+++++++++++++++++++++++++++++++++*/

/* 1. constants */

#define MINPROB_DEF -20.0        /* log(1*10^-20)        */
#define True  1
#define False 0

/* the following constant is usually defined in floats.h. since this header  *
 * file does not exist under SunOS 4.1.3 and since we use nothing else from  *
 * there, we decided to define it ourselves.                                 *
 * this might cause problems when porting the code to machines with a        *
 * different representation of floating point numbers is an issue.           */
#define DBL_MAX 1.7976931348623157E+308

/* 2. macros */

/* strcmp(s1, s2) yields 0 iff s1 == s2 */
#define STREQ(a,b) (((a) == (b)) || \
                    ((a) && (b) && (strcmp((a), (b)) == 0)))


/*++++++++++++++++++++++++++++++++++++++*
 + DECLARATION OF GLOBAL VARIABLES      +
 *++++++++++++++++++++++++++++++++++++++*/

int GuessWords       = 0;
int MaxShowLangs     = 0;

double minprob = MINPROB_DEF;


/* to determine the best (and worst) language, the following data *
 * structure is used.                                             */
typedef struct {
    double  bestval;
    int     bestlg;
    double  worstval;
} langcmp_t;


/* the following array will be filled and sorted (by probability scores) *
 * in order to show the ranking of the languages.                        */
typedef struct {
    double   prob;
    int      lang;
} probarr_t;

static probarr_t *probarr_srt = NULL;

static double *textprob          = NULL;    /* probs of the whole text over all lgs.    */
static double *shortwordprob     = NULL;    /* short-word probs (whole text, all lgs.)  */
static double *trigshortwordprob = NULL;    /* combination of trig and short word probs */

static int MaxWords         = 0;        /* maximum number of words after *
                                         * which the guesser should stop */
static int StopGuess        = False;    /* boolean flag.                 */


/* the guesser is provided with a list of characters that have been ignored *
 * by the training tool. therefore they should also be ignored during the   *
 * guessing process proper. for the character membership test we use a      *
 * boolean array indexed by the integer value of the characters.            */
#define CHARS2DELETE "1234567890`~!@#$%^&*()_+-=[]\{}|;:\",./<>?\t\\"
static const char stopcharset[] = CHARS2DELETE;
static int        stopchars[256];
static const int  numstopchars = sizeof(stopcharset) - 1;

/*++++++++++++++++++++++++++++*
 + Utility Functions          +
 *++++++++++++++++++++++++++++*/

static void 
fatal_error(const char *msg) {

    //extern int errno;
    
    fputs("guesser_lib: ", stderr);
    //if (errno)
     //   perror(msg);
    //else {
        fputs(msg, stderr);
        fputs("\n", stderr);
    //}
    exit(2);
}


static int
probcmp(const void *aa, const void *bb) {

    /* to make gcc happy, we use shadow variables ... */
    const probarr_t *a = (probarr_t *)aa;
    const probarr_t *b = (probarr_t *)bb;

    if (a->prob > b->prob)
        return(-1);
    if (a->prob < b->prob)
        return(1);
    return(0);
}


/* determine the best language in a given probability vector. */
static int
bestlang(const double *probvec) {

    register int    i;
    int             lang  = 0;
    double          best  = probvec[0];
    double          worst = probvec[0];
    
    for (i = 1; i < _num_langs; i++) {
        if (probvec[i] > best && languages[i].used) {
            best = probvec[i];
            lang = i;
        }
        if (probvec[i] < worst && languages[i].used)
            worst = probvec[i];
    }
    return( (best == worst)? _num_langs : lang );
}


/* calculate linear combination of two probability vectors. */
static void
combine_probs(double       **res,
              const double  *vec1,
              const double  *vec2,
              const double  weight1,
              const double  weight2) {

    register int i;

    for (i = 0; i < _num_langs; i++)
        (*res)[i] = weight1 * vec1[i] + weight2 * vec2[i];
}



/* 'clean' word from undesired characters.  *
 * function returns length of clean string. */
static int
clean_word(const unsigned char *word, const int offset, unsigned char *token) {

    int            len = strlen(word);
    register int   i, ci = offset;

    for (i = 0; i < len; i++)
        if (!stopchars[(int)word[i]]) {
            token[ci] = tolower(word[i]);
            ci++;
        }
    token[ci] = '\0';
    return(ci);
} 


static void
init_stop_chars(void) {
    
    register int  i;
    
    memset(stopchars, 0, 256);
    
    for (i = 0; i < numstopchars; i++)
        stopchars[(int)stopcharset[i]] = True;
}


/*+++++++++++++++++++++++++++++++
 +     UNDOCUMENTED FUNCTIONS   +
 +++++++++++++++++++++++++++++++*/


/* function similar to set_language(), but works over all available languages */
void
set_all_languages(const int mode) {

    register int i;

    for (i = 0; i < _num_langs; i++)
        languages[i].used = mode;
}


/* the following function can be used to print a comma-separated list *
 * of all available languages. the output goes to stdout.             */
void
print_avail_langs(const guesser_lang_type type) {
  
    unsigned int  i;
    char          c;
    char         *str;

    /* print a comma-separated list of available languages. *
     * BUG: if the last printed language is not the last    *
     *      language in the list of available languages, it *
     *      will printed with a comma anyway.               */
    for (i = 0; i < _num_langs; i++) {
        if (languages[i].used) {
            if (type == guesser_iso639) 
                str = languages[i].iso639;
            else if (type == guesser_iso639_2) 
                str = languages[i].iso639_2;
            else 
                str = languages[i].full;
            c = languages[i].iso639[strlen(languages[i].iso639) - 1];
            if (c != 'U' && c != 'E') 
                printf("%s%c", str, (i < _num_langs - 1)? ',' : '\n');
        }
    }
    printf("\n");
}


/*++++++++++++++++++++++
 +    MAIN FUNCTIONS   +
 ++++++++++++++++++++++*/

/* disable or enable a given language. */
int
set_language(const char *lang, const int mode) {

    register int i;
    int          res = 0;

    for (i = 0; i < _num_langs; i++)
        if (STREQ(languages[i].full, lang) || 
            STREQ(languages[i].iso639, lang) || STREQ(languages[i].iso639_2, lang)) {
            languages[i].used = mode;
            res = 1;
        }
    return(res);
}


void
reset_guesser(void) {

    register int  i;
    
    for (i = 0; i < _num_langs; i++) {
        shortwordprob[i] = 
            textprob[i] = 
            trigshortwordprob[i] =
            (languages[i].used)? 0.0 : -(DBL_MAX);
    } 
    GuessWords = 0;
}


int
init_guesser(void) {

    register int   i;

    /* we set the locale to ISO Latin I in order to be able to map all characters to    *
     * their lowercase variants. we do this for _all_ languages regardless of the       *
     * actual character set (i.e. even for UNICODE-encoded languages, such as Chinese). */
    setlocale(LC_CTYPE, "iso_8859_1");

    if (!textprob) {
        textprob = (double *)malloc(sizeof(double) * _num_langs);
        if (!textprob) fatal_error("can't allocate probability vector\n");
    }

    if (!shortwordprob) {
        shortwordprob = (double *)malloc(sizeof(double) * _num_langs);
        if (!shortwordprob) fatal_error("can't allocate short word probability vector\n");
    }

    if (!trigshortwordprob) {
        trigshortwordprob = (double *)malloc(sizeof(double) * _num_langs);
        if (!trigshortwordprob) fatal_error("can't allocate trig/shortword probability vector\n");
    }

    if (!probarr_srt) {
        probarr_srt = (probarr_t *)malloc(sizeof(probarr_t) * _num_langs);
        if (!probarr_srt) fatal_error("can't allocate sorted probability vector\n");
    }
    reset_guesser();
    init_stop_chars();

    /* initialization of the language-specific hashing tables. */
    for (i = 0; i < _num_langs; i++) {
        if (languages[i].used && languages[i].init( i )) fatal_error("error in initialization");
    }
    return(_num_langs);
}


/* Print the n first elements (language short form and logarithmatic        *
 * probability) of the internal probability table sorted by descending      *
 * probability scores.                                                      *
 * If the flag 'perc' is set to 1, percental probabilities will be printed. */
void
show_sorted_probs(int n, const int perc, const guesser_id_type type) {

    register       int i;
    double         probsum = 0.0;
    static double *probs   = NULL;
    double         best;
    int            (*cmpfunc)(const void *, const void *) = probcmp;

    for (i = 0; i < _num_langs; i++) {
        probarr_srt[i].lang = i;
        if (languages[i].used)
            switch (type) {
            case shortword: 
                probarr_srt[i].prob = shortwordprob[i];
                break;
            case trig: 
                probarr_srt[i].prob = textprob[i];
                break;
            case trig_shortword:
                probarr_srt[i].prob = trigshortwordprob[i];
                break;
            }
        else probarr_srt[i].prob = -(DBL_MAX);
    }
    qsort((probarr_t *)probarr_srt, _num_langs, sizeof(probarr_t), cmpfunc);
    best    = probarr_srt[0].prob;
        
    if (n <= 0) n = _num_langs;

    if (!probs)
        if ((probs = (double *)malloc(sizeof(double) * _num_langs)) == NULL)
            fatal_error("can't allocate percental probability vector");
    
    if (perc) {
        for (i = 0; i < _num_langs; i++) {
            if (languages[probarr_srt[i].lang].used) {
                probs[i] = exp(probarr_srt[i].prob - best);
                probsum += probs[i];
            }
        }
        for (i = 0; i < n; i++)
            if (languages[probarr_srt[i].lang].used)
                printf("%-4s\t%3.8f\n", languages[probarr_srt[i].lang].iso639_2, probs[i]/probsum * 100.0);
    } else for (i = 0; i < n; i++)
        if (languages[probarr_srt[i].lang].used)
            printf("%-4s\t%5.8f\n", languages[probarr_srt[i].lang].iso639_2, probarr_srt[i].prob - best);
}


void
stop_guesser(const int word_num) {

    StopGuess  = True;
    MaxWords   = word_num;
    GuessWords = 0;
}


static void
store_trig_prob(unsigned char *word) {
  
    unsigned long  cid;
    register int   j, maxidx, lang;
    unsigned char *trig, *token;
    unsigned char  savec;
    double         p;
    int            len = strlen(word), toklen;

    token = (unsigned char *)malloc(len + 3);
    *token            = '_';
    toklen            = clean_word(word, 1, token);
    token[toklen]     = '_';
    token[toklen + 1] = '\0';
        
    /* largest index is given by the actual number of characters *
     * (i.e. stop chars removed) minus ngram size (here 3) plus  *
     * the number of boundary markers (i.e. 2).                  */
    maxidx = toklen - 3 + 2;
    
    /* compute for each possible letter trigram a probability */
    for (j = 0; j < maxidx; j++) {
        trig = token + j;         /* set start position. */
        savec = *(token + j + 3); /* save char following the current trigram. */
        *(trig + 3) = '\0';       /* mark end of current trigram. */
        
        for (lang = 0; lang < _num_langs; lang++)
            if (languages[lang].used) {
                if (toklen > 1) {
                    cid = string2id(trig, languages[lang].dict, no_insert);
                    p = (cid != ~0 && cid != -1)? languages[lang].hash[cid] : minprob;
                    textprob[lang] += p;
                }
                else textprob[lang] += minprob;
            }
        *(trig + 3) = savec;      /* restore the original string. */
    }
    free(token);
}


static void
store_shortword_prob(const unsigned char *word) {
    
    unsigned long  cid;
    register int   lang;
    unsigned char *token = (unsigned char *)malloc(strlen(word) + 1);

    (void)clean_word(word, 0, token);
    for (lang = 0; lang < _num_langs; lang++)
        if (languages[lang].used)
            /* if there isn't a short word dictionary for the current language, *
             * assign the minimal probability.                                  */
            if (!languages[lang].wdict)
                shortwordprob[lang] += minprob;
            else {
                cid = string2id(token, languages[lang].wdict, no_insert);
                shortwordprob[lang] += (cid != ~0 && cid != -1)? languages[lang].whash[cid] : minprob;
            }
    free(token);
}


void
scan_line(const unsigned char *line, const guesser_id_type type) {

    unsigned char *word;
    unsigned char *input   = (unsigned char *)strdup(line);
    int            linelen = strlen(line);
    const char    *wordsep = " ";
    
    /* chop off newline character at the end of a word. */
    if (input[linelen - 1] == '\n') input[linelen - 1] = '\0';

    word = (unsigned char *)strtok((char *)input, wordsep);
    if (word)
        do {
            ++GuessWords;
            /* compute probabilities of current word and *
             * for the text up to that word.             */
            if (type != shortword)
                store_trig_prob(word);
            if (type == shortword || type == trig_shortword)
                store_shortword_prob(word);
            if (StopGuess && GuessWords >= MaxWords)
                break;
            word = (unsigned char *)strtok(NULL, wordsep);
        } while (word);
    free(input);
}


void
do_guess(char            *res,
         const guesser_lang_type  lgtype,
         const guesser_id_type    type) {
    
    int            lang;

    if (type == trig) 
        lang  = bestlang(textprob);
    else if (type == shortword)
        lang = bestlang(shortwordprob);
    else if (type == trig_shortword) {
        combine_probs(&trigshortwordprob, textprob, shortwordprob, 1.0, 1.0);
        lang = bestlang(trigshortwordprob);
    } else lang = _num_langs; /* should never be reached */
    
    /* copy the return string */
    if (lgtype == guesser_iso639)
        strcpy(res, languages[lang].iso639);
    else if (lgtype == guesser_iso639_2)
        strcpy(res, languages[lang].iso639_2);
    else strcpy(res, languages[lang].full);
}


void
guess_language(const unsigned char  *line,
               char                 *res,
               const guesser_lang_type       lgtype,
               const guesser_id_type         type) {
    
    if (line) {
        reset_guesser();
        scan_line(line, type);
        do_guess(res, lgtype, type);
    } else
    /* copy the return string */
    if (lgtype == guesser_iso639)
        strcpy(res, languages[_num_langs].iso639);
    else if (lgtype == guesser_iso639_2)
        strcpy(res, languages[_num_langs].iso639_2);
    else strcpy(res, languages[_num_langs].full);
}
