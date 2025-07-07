/* $Id: utf8_fatstr_util.h,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 2003 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  UTF8_FATSTR_UTIL.H
 **
 **  JEAN-MARC COURSIMAULT, XRCE 2003
 **
 **  Utf-8 utilities for fat-strings
 **
 ******************************************************/


#ifndef UTF8_FATSTR_UTIL_H
#define UTF8_FATSTR_UTIL_H

#include "fst/ConvertUTF.h"
#include "fst/fat-str.h"

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Release the global variable TEMPORARY_UTF8_FATSTR.
   (To be called by function release_fsm_memory()). */
void FstBaseExport free_UTF8fat_string_memory();
void FstBaseExport ConvertUTF16_1toFatStr (unsigned int ic, FAT_STR fatStr);

ConversionResult FstBaseExport ConvertUTF8toFAT_CHAR (UTF8** sourceStart, UTF8* sourceEnd, 
					FAT_CHAR * pFatChar, ConversionFlags flags);

ConversionResult FstBaseExport ConvertUTF8to16bit (UTF8** sourceStart,UTF8* sourceEnd, 
                                     UTF16 * pUtf16, ConversionFlags flags);
ConversionResult FstBaseExport ConvertFatChartoUTF8 (FAT_STR source, UTF8 * target, UTF8* targetEnd, ConversionFlags flags);

ConversionResult FstBaseExport ConvertFatStrtoUTF8 (FAT_STR source, UTF8 * target, UTF8* targetEnd, ConversionFlags flags);


FAT_STR FstBaseExport utf8_thin_to_temp_fat (char *t);
FAT_STR FstBaseExport utf8_thin_to_fat (const char *t);

void utf8_fat_char_out (FAT_STR fc);
void utf8_fat_char_out_print_esc (FAT_STR fc);

void utf8_fat_string_to_buf (FAT_STR fs, STRING_BUFFERptr buf);
void utf8_fat_string_to_buf_esc (FAT_STR fs, STRING_BUFFERptr buf);
void utf8_fat_char_to_buf (FAT_STR fc, STRING_BUFFERptr buf);
int utf8_is_character_printable(FAT_STR fc);

size_t FstBaseExport utf8_required_length_from_fat_str(FAT_STR fs);

int FstBaseExport utf8_fprint_fat_string (FILE *outfile, FAT_STR fs);
int FstBaseExport utf8_fprint_fat_string_esc (FILE *outfile, FAT_STR fs, char *esc_codes);
int FstBaseExport utf8_fprint_fat_string_with_esc (FILE *outfile, FAT_STR fs);
int FstBaseExport utf8_fprint_fat_char_esc (FILE *outfile, FAT_STR fc , char *esc_codes);

char *utf8_fat_to_thin_esc (FAT_STR fs, char *s, char *esc_codes,int hexa_or_unicode_always);

extern const char trailingBytesForUTF8[256];
extern Boolean isLegalUTF8(const UTF8 *source, int length);
/*
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */
extern const UTF32 offsetsFromUTF8[6];

#define UNI_SUR_HIGH_START	(UTF32)0xD800
#define UNI_SUR_HIGH_END	(UTF32)0xDBFF
#define UNI_SUR_LOW_START	(UTF32)0xDC00
#define UNI_SUR_LOW_END		(UTF32)0xDFFF

#ifdef __cplusplus
	} /* extern "C" */
#endif

#endif
