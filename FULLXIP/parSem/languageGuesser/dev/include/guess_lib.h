/*******************************************************************
|***
|***         File: guesser_lib.h
|***       Author: Bruno Maximilian Schulze
|***      Purpose: language guesser API: function prototypes
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

/*

  DISCLAIMER:

  The code in guesser_lib.c is not (yet?) foolproof. There are occasions
  where it might fail nongracefully, producing bus errors, segmentation
  violations or non-termination of function calls.

 */

#ifndef _GUESSER_H_
#define _GUESSER_H_

#ifdef XIPDLL_EXPORT
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif


#include "globals.h"


typedef enum guess_t { trig, shortword, trig_shortword } guesser_id_type;
typedef enum lang_t  { guesser_iso639, guesser_iso639_2, guesser_full } guesser_lang_type;

EXPORT void print_avail_langs(const guesser_lang_type type);
EXPORT void set_all_languages(const int mode);

/* Initializes the guesser. Must be called beforehand any calls to  *
 * guess_language() or scan_line(), otherwise the results are       *
 * unpredictable. It is not necessary to call this function more    *
 * than once (cf. reset_guesser()).                                 *
 * This function returns the number of all available languages.     */
EXPORT int init_guesser(void);


/* Resets the internal tables of the guesser. After a call to this function *
 * the probabilities calculated so far (by calling scan_line()) will        *
 * be lost.                                                                 *
 * This function can be used if you want to apply the guesser more than     *
 * once, but the guesses should be independent from the previous calls.     *
 * By using of reset_guesser() a time consuming hard reset using the        *
 * function init_guesser() can be avoided.                                  */
EXPORT void reset_guesser(void);


/* set_language() can be used  to include or exclude a language for guessing. *
 * It takes as argument a short form of a language and a boolean flag 'mode'. *
 * When mode is equal to 0 the language will be excluded.                     *
 * The functions returns '1' in case of success and '0' otherwise.            */
EXPORT int set_language(const char *lang, const int mode);


/* Limits the number of words that are to be considered for guessing. */
EXPORT void stop_guesser(const int num_words);


/* Determines the language of the given string. Usually, this is a line    *
 * of text or at least a bunch of words.				   *
 * guess_language() performs a soft reset (reset_guesser()), so the        *
 * results are not influenced by previous calls to scan_line() or          *
 * guess_language().                                                       *
 * Inspect the variable 'GuessWords' to find out how many words have       *
 * been actually considered for guessing.                                  *
 *									   *
 * The result will be copied to the string 'res'. Depending on 'lgtype'    *
 * this will be a two-letter code (iso639), a three-letter code (iso639-2) *
 * or the full name of the language.                                       *
 * With the paramater 'type' the type of the guess method can be specified *
 *									   *
 * NOTE: The string 'res' will not be allocated by this function.          *
 *       In addition, we do not check for overflow of 'res'.               */

EXPORT void guess_language(const unsigned char *line,
               char *res,
               const guesser_lang_type lgtype, 
               const guesser_id_type type);


/* This function can be used if you want get a guess for more than one      *
 * line of text. Call this function for each line of your input text,       *
 * and finally call do_guess() to get the overall estimation.               *
 * With the paramater 'type' the type of the guess method will be specified */
EXPORT  void scan_line(const unsigned char *line, const guesser_id_type type);


/* Perform a guess on the text read so far by using the function           *
 * scan_line().                                                            *
 * The function has no effect on the internal status of the library.       *
 * Inspect the variable 'GuessWords' to find out how many words have been  *
 * actually considered for the guess.                                      *
 * Cf. guess_language() for details about the parameters and return value. */
EXPORT void do_guess(char *res,
         const guesser_lang_type lgtype,
         const guesser_id_type type);


#endif
