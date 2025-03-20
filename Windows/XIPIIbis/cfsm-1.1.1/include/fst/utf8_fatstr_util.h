#ifndef UTF8_FATSTR_UTIL_H
#define UTF8_FATSTR_UTIL_H

#include "ConvertUTF.h"
#include "fat-str.h"

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

void FsmFstExport ConvertUTF16_1toFatStr (unsigned int ic, FAT_STR fatStr);

ConversionResult ConvertUTF8toFAT_CHAR (UTF8** sourceStart, UTF8* sourceEnd, 
					FAT_CHAR * pFatChar, ConversionFlags flags);

ConversionResult ConvertUTF8to16bit (UTF8** sourceStart,UTF8* sourceEnd, 
                                     UTF16 * pUtf16, ConversionFlags flags);
ConversionResult FsmFstExport ConvertFatChartoUTF8 (FAT_STR source, UTF8 * target, UTF8* targetEnd, ConversionFlags flags);

ConversionResult FsmFstExport ConvertFatStrtoUTF8 (FAT_STR source, UTF8 * target, UTF8* targetEnd, ConversionFlags flags);


FAT_STR FsmFstExport utf8_thin_to_temp_fat (char *t);
FAT_STR utf8_thin_to_fat (const char *t);

void utf8_fat_char_out (FAT_STR fc);
void utf8_fat_char_out_print_esc (FAT_STR fc);

void utf8_fat_string_to_buf (FAT_STR fs, STRING_BUFFERptr buf);
void utf8_fat_string_to_buf_esc (FAT_STR fs, STRING_BUFFERptr buf);
void utf8_fat_char_to_buf (FAT_STR fc, STRING_BUFFERptr buf);
int utf8_is_character_printable(FAT_STR fc);
char * utf8_fat_to_thin (FAT_STR fs, char *s);

int FsmFstExport utf8_fprint_fat_string (FILE *outfile, FAT_STR fs);
int utf8_fprint_fat_string_esc (FILE *outfile, FAT_STR fs, char *esc_codes);
int utf8_fprint_fat_string_with_esc (FILE *outfile, FAT_STR fs);
int utf8_fprint_fat_char_esc (FILE *outfile, FAT_STR fc , char *esc_codes);

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
}
#endif

#endif
