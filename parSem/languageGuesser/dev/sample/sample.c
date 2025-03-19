/*******************************************************************
|***
|***         File: sample-guess.c
|***       Author: Bruno Maximilian Schulze
|***      Purpose: simple language guesser
|***      Created:
|***     Modified:
|***    Copyright: Rank Xerox Research Centre
|***
\*******************************************************************/
/* Copyright (c) 1996 by the Xerox Corporation.  All rights reserved.
   Copyright protection claimed includes all forms and matters of
   copyrightable material and information now allowed by statutory or
   judicial law or hereinafter granted, including without limitation,
   material generated from the software programs which are displayed 
   on the screen such as icons, screen display looks, etc.
   */


#include<stdio.h>       /* necessary for gets() and puts() */
#include "guess_lib.h"  /* for RXRC Language Guesser API   */


/* begin of main program */
int main () {

    unsigned char  str[1024];           /* text of a single line. */
    char           res[20];            /* guess results string.  */
    int            max_lang;           /* number of avail. langs.*/
    guesser_id_type    GuessType = trig_shortword;
    guesser_lang_type  LangType  = guesser_full;

    max_lang = init_guesser();         /* API initialization. */


    puts("This programme is waiting for a sentence and it returns the language of this sentence\n\n");
    while (gets((char *)str)) {        /* while input from stdin ... */

        guess_language(str, res,  LangType, GuessType);
                                       /* guess the language of the input *
                                        * line. use combined trigram and  *
                                        * short word probabilities.       *
                                        * the result will be stored in    *
                                        * variable 'res' as the full      *
                                        * English name of the language.   */

        puts(res);                     /* print the language name. */
    }
    exit(0);
}

