/* $Id: fat-str.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

/**************************************************************************
**  
**                             	fat-str.h
**                  Todd Yampol, Xerox PARC: January, 1989
**                  Lauri Karttunen, RXRC: October, 1993
**
**   Declarations & Function prototypes for fat (16 bit) characters & strings.
**   
**************************************************************************/

#ifndef C_FSM_FAT_STR

#define C_FSM_FAT_STR

#include "fst/fst_windefs.h"
#include <stdio.h>
#include "fst/str-buf.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Replacing Todd's old definition as suggested by Chris Manning.

typedef struct FCHAR {
	unsigned int char_num:8;
	unsigned int char_set:8;
} FAT_CHAR, *FAT_STR;

#define CHARSET(X) (X)->char_set
#define CHARNUM(X) (X)->char_num

*/

typedef unsigned char FAT_CHAR[2];
typedef FAT_CHAR *FAT_STR;
/*
#define CHARSET(X) (*(X))[1]
#define CHARNUM(X) (*(X))[0]
*/
#define CHARSET(X) (*(X))[0]
#define CHARNUM(X) (*(X))[1]

#define fat_charcpy(X, Y)  (X)[0] = (Y)[0] ; (X)[1] = (Y)[1];

/* Release the global variable TEMPORARY_FATSTR.
   (To be called by function release_fsm_memory()). */
void FstBaseExport free_fat_string_memory();
int FstBaseExport fat_charcmp(FAT_CHAR s1, FAT_CHAR s2);
int FstBaseExport fat_strcmp(void *s1, void *s2);
int FstBaseExport fat_strcmp_lab(register FAT_STR s1, register FAT_STR s2);
FAT_STR FstBaseExport fat_strcpy(FAT_STR s1, FAT_STR s2);
FAT_STR FstBaseExport copy_fat_string(FAT_STR fs);
FAT_STR fat_strcat(FAT_STR s1, FAT_STR s2);
int FstBaseExport fat_strlen(register FAT_STR s);
FAT_STR FstBaseExport thin_to_fat (const char *t);
FAT_STR FstBaseExport thin_to_temp_fat(register char *t);
char FstBaseExport *utf8_fat_to_thin(FAT_STR fs, char *s);
char FstBaseExport *fat_to_thin (register FAT_STR fs, char *s);
char *fat_to_thin_esc (register FAT_STR fs, char *s, char *esc_codes, int hexa_or_unicode_always);

int fstring_to_charcode(FAT_STR fs);
FAT_STR fchar_to_fstring (FAT_CHAR *fc);
FAT_STR charcode_to_fstring (int i);
FAT_STR integer_to_fstring(int i);

int fprint_fat_char (FILE *outfile, FAT_STR fc);
int FstBaseExport fprint_fat_string (FILE *outfile, FAT_STR fs);

#define print_fat_char(X) fprint_fat_char(stdout, (X))
#define print_fat_string(X) FST_fprint_fat_string(stdout, (X))

/* Escaped versions (for any char in esc_code, it prints  become %char) */

int FstBaseExport fprint_fat_char_esc (FILE *outfile, FAT_STR fc , char *esc_codes);
int FstBaseExport fprint_fat_string_esc (FILE *outfile, FAT_STR fs, char *esc_codes);

#define print_fat_char_esc(X) fprint_fat_esc(stdout, (X))
#define print_fat_string_esc(X) FST_fprint_fat_string_esc(stdout, (X))

/* Better versions of the two functions above. Maybe fprint_fat_char_esc should be modified. */

int fprint_fat_char_with_esc (FILE *outfile, FAT_STR fc);
int FstBaseExport fprint_fat_string_with_esc (FILE *outfile, FAT_STR fs);

/* Hack to perform real iso_latin_1 printable character checking.
   Languages like French, German, Swedish and others need this! TG */
/* #define	is_iso_latin_1_upperhalf(c) ((c) > 160) */
#define	is_iso_latin_1_upperhalf(c) ( ( (c ) >>7 ) && ( (c) >> 5))
#define	isgraph_iso_latin_1(c)       (isgraph(c) || is_iso_latin_1_upperhalf(c))

int is_character_printable(unsigned char c);

void hex_to_buf(long i, STRING_BUFFERptr buf);
void fat_char_to_buf(FAT_STR fc, STRING_BUFFERptr buf);
void fat_char_to_buf_esc (FAT_STR fc , char *esc_codes, STRING_BUFFERptr buf);
void fat_string_to_buf (FAT_STR fs, STRING_BUFFERptr buf);
void fat_string_to_buf_esc (FAT_STR fs, char *esc, STRING_BUFFERptr buf);

#ifdef __cplusplus
}
#endif

#endif
