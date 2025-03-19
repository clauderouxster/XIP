/* $Id: utf8_fatstr_util.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 2003 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  UTF8_FATSTR_UTIL.C
 **
 **  JEAN-MARC COURSIMAULT, XRCE 2003
 **
 **  Utf-8 utilities for fat-strings
 **
 ******************************************************/

#include "fst/utf8_util.h"
#include "fst/struct.h"
#include "fst/pretty-p.h"
#include "fst/fsm_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fst/error.h"

#include "fst/ConvertUTF.h"
#include "fst/utf8_fatstr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
#include "assert.h"
#endif

/* Temporary fat_string buffer to be used for conversion by function
   thin_to_temp_fat(): its size is adjusted as needed.
   (!! must be released by calling free_fat_string_buffer() in function
   release_fsm_memory()). */
static FAT_STR TEMPORARY_UTF8_FATSTR= NULL;
static size_t TEMPORARY_UTF8_FATSTR_len= 0;


/* 
In the following, we call UTF16-1 the UTF-16 encoding of the
first 63488 Unicode chars. That means that we cannot use/represent
the Unicode characters requiring surrogates.
That's because the FSM way to represent a character is in a FAT_CHAR
(16 bits).   */


/*
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... six byte sequence.)
 */
static const UTF8 firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

static const int halfShift	= 10; /* used for shifting by 10 bits */
static const UTF32 halfBase	= 0x0010000UL;




/*=======================*/
/* ConvertUTF8toFAT_CHAR */
/*=======================*/
/** 
* Convert a single UTF-8 char (maybe coded of several bytes) to a single FAT_CHAR
* Only UTF-8 chars that can be coded in UTF-16 on 16 bits are accepted.
* All others are replaced by UNI_REPLACEMENT_CHAR
* The result is a single FAT_CHAR.
* @param sourceStart : pointer to the address of the string to translate
* @param sourceEnd: the address of the char after the last in the input string,,
*					or zero for no checking.
* @param pFatChar : the address of the fat char to fill
* @param flags : either strictConversion or lenientConversion. See ConvertUTF.h for details.
* @return : ConversionResult . See ConvertUTF.h for details.
*/


ConversionResult ConvertUTF8toFAT_CHAR (
		UTF8** sourceStart, UTF8* sourceEnd, 
		FAT_CHAR * pFatChar, ConversionFlags flags) {

	ConversionResult result = conversionOK;
	UTF8* source = *sourceStart;

		UTF32 ch = 0;
		unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
		/* In some cases we don't know where the string ends */
		if (sourceEnd && (source + extraBytesToRead > sourceEnd)) {
			return  sourceExhausted;
		}
		/* Do this check whether lenient or strict */
		if (! isLegalUTF8(source, extraBytesToRead+1)) {
			return sourceIllegal;
		}

		/*
		 * The cases all fall through. See "Note A" below.
		 */
		switch (extraBytesToRead) {
			case 3:	ch += *source++; ch <<= 6;
			case 2:	ch += *source++; ch <<= 6;
			case 1:	ch += *source++; ch <<= 6;
			case 0:	ch += *source++;
		}
		ch -= offsetsFromUTF8[extraBytesToRead];

		if (ch > UNI_MAX_BMP) 
		{
			/* Just replace characters above this limit */
			ch = UNI_REPLACEMENT_CHAR;
		}

		if ((flags == strictConversion) && (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END)) {
			source -= (extraBytesToRead+1); /* return to the illegal value itself */
			result = sourceIllegal;
		} else {
			CHARSET(pFatChar) = (char)(ch / 256);
			CHARNUM(pFatChar) = (char)(ch & 255U);
		}
	*sourceStart = source;
	return result;
}

/*=========================*/
/* ConvertUTF8strtoFAT_STR */
/*=========================*/
/** 
* Convert a whole string containing UTF-8 to a FAT_STR
* Only UTF-8 chars that can be coded in UTF-16 on 16 bits are accepted.
* All others are replaced by UNI_REPLACEMENT_CHAR
* @param sourceStart : pointer to the address of the string to translate
* @param sourceEnd: the address of the char after the last in the input string
* @param targetStart : pointer to the FAT_STR to fill
* @param targetEnd : the address of the FAT_CHAR after the last to fill
* @param flags : either strictConversion or lenientConversion. See ConvertUTF.h for details.
* @return : ConversionResult . See ConvertUTF.h for details.
*/

ConversionResult ConvertUTF8strtoFAT_STR (
		UTF8** sourceStart, UTF8* sourceEnd, 
		FAT_CHAR ** targetStart, ConversionFlags flags) {

	ConversionResult result = conversionOK;
	ConversionResult final_result = conversionOK;
	UTF8* source = *sourceStart;
	FAT_CHAR* target = *targetStart;
//	while (source < sourceEnd && (conversionOK != result)) {
	while (source < sourceEnd) {
		result = ConvertUTF8toFAT_CHAR ( &source, sourceEnd, target, flags);
		if (result != conversionOK)
		  { 
		    fsm_fprintf(stderr, "'%c' (0x%X) in '%s' is an illegal\
 UTF-8 character.\n", *source, *source, *sourceStart);
		    CHARSET(target) = 0xFD;
		    CHARNUM(target) = 0xFF;
		    source = source+1;
		    final_result = result;
		  }
		target++;
	}
	CHARSET(target) = 0;
	CHARNUM(target) = 0;
	*sourceStart = source;
	*targetStart = target++;
	return final_result;
}

/* Convert a single fat char to UTF-8 */
ConversionResult ConvertFatChartoUTF8 (FAT_STR source, UTF8 * target, UTF8* targetEnd, ConversionFlags flags){
	ConversionResult result = conversionOK;
	UTF32 ch;
	unsigned short bytesToWrite = 0;
	const UTF32 byteMask = 0xBF;
	const UTF32 byteMark = 0x80; 
	
	ch = CHARSET(source) * 256 + CHARNUM(source);

	/* If we have a surrogate pair, convert to UTF32 first. */
	if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
		UTF32 ch2 = ch = CHARSET(source) * 256 + CHARNUM(source);
		if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
			ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
				+ (ch2 - UNI_SUR_LOW_START) + halfBase;
			++source;
		} else if (flags == strictConversion) { /* it's an unpaired high surrogate */
			--source; /* return to the illegal value itself */
			return sourceIllegal;
		}
	} else if ((flags == strictConversion) && (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END)) {
		--source; /* return to the illegal value itself */
		return sourceIllegal;
	}
	/* Figure out how many bytes the result will require */
	if (ch < (UTF32)0x80) {			bytesToWrite = 1;
	} else if (ch < (UTF32)0x800) {		bytesToWrite = 2;
	} else if (ch < (UTF32)0x10000) {	bytesToWrite = 3;
	} else if (ch < (UTF32)0x200000) {	bytesToWrite = 4;
	} else {				bytesToWrite = 2;
						ch = UNI_REPLACEMENT_CHAR;
	}

	target += bytesToWrite;
	if (target > targetEnd) {
		target -= bytesToWrite;
		return targetExhausted;
	}
	switch (bytesToWrite) {	/* note: everything falls through. */
		case 4:	*--target = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;
		case 3:	*--target = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;
		case 2:	*--target = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;
		case 1:	*--target = (unsigned char)( ch | firstByteMark[bytesToWrite]);
	}
	target += bytesToWrite;
	*target = 0;

	return result;

}

/* Should we make this function call ConvertFatChartoUTF8 repeatedly ? */
/* It would be slower, but the code would be in a single place         */
ConversionResult ConvertFatStrtoUTF8 (FAT_STR source, UTF8 * target, UTF8* targetEnd, ConversionFlags flags){

	ConversionResult result = conversionOK;
	while (CHARSET(source) || CHARNUM(source)) {
		UTF32 ch;
		unsigned short bytesToWrite = 0;
		const UTF32 byteMask = 0xBF;
		const UTF32 byteMark = 0x80; 
		
		ch = CHARSET(source) * 256 + CHARNUM(source);

		/* If we have a surrogate pair, convert to UTF32 first. */
		if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
			UTF32 ch2 = ch = CHARSET(source) * 256 + CHARNUM(source);
			if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
				ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
					+ (ch2 - UNI_SUR_LOW_START) + halfBase;
				++source;
			} else if (flags == strictConversion) { /* it's an unpaired high surrogate */
				--source; /* return to the illegal value itself */
				result = sourceIllegal;
				break;
			}
		} else if ((flags == strictConversion) && (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END)) {
			--source; /* return to the illegal value itself */
			result = sourceIllegal;
			break;
		}
		/* Figure out how many bytes the result will require */
		if (ch < (UTF32)0x80) {			bytesToWrite = 1;
		} else if (ch < (UTF32)0x800) {		bytesToWrite = 2;
		} else if (ch < (UTF32)0x10000) {	bytesToWrite = 3;
		} else if (ch < (UTF32)0x200000) {	bytesToWrite = 4;
		} else {				bytesToWrite = 2;
							ch = UNI_REPLACEMENT_CHAR;
		}

		target += bytesToWrite;
		if (target > targetEnd) {
			target -= bytesToWrite; result = targetExhausted; break;
		}
		switch (bytesToWrite) {	/* note: everything falls through. */
			case 4:	*--target = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;
			case 3:	*--target = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;
			case 2:	*--target = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;
			case 1:	*--target = (unsigned char)( ch | firstByteMark[bytesToWrite]);
		}
		target += bytesToWrite;
		source++;
	}
	*target = 0;
	return result;

}

/*=============*/
/* utf8_length */
/*=============*/
/** Returns the number of Unicode characters in a UTF-8 string
* @param utf8str : the input string
* @return : the number of Unicode chars in the input string. Incomplete characters, if any, are not counted.
*/
size_t utf8_length(char * utf8str)
{
	size_t len = 0;
	unsigned short extraBytesToRead;
	char * start = utf8str;
	while (*start){
		extraBytesToRead = trailingBytesForUTF8[(unsigned char)*start];
		len ++;
		start += extraBytesToRead +1;
	}

	return len; 
}


/*===================================*/
/* utf8_required_length_from_fat_str */
/*===================================*/
/** Return the number of bytes needed to code a FAT_STR to utf8 
* (including the zero byte at the end)
* @param fs : the fat string in input
* @return   : the number of bytes needed including the terminating zero
*/
size_t utf8_required_length_from_fat_str(FAT_STR fs){

  FAT_STR f = fs;
  size_t requiredLength;
  UTF32 ch;

  for( f= fs, requiredLength = 0; ((CHARSET(f) != 0) || (CHARNUM(f) != 0)); f++ )
    {
      ch = CHARSET(f) * 256 + CHARNUM(f);	

      /* Figure out how many bytes the result will require */
      if (ch < (UTF32)0x80)
	{
	  requiredLength += 1;
	} else if (ch < (UTF32)0x800) {
	  requiredLength += 2;
	} else if (ch < (UTF32)0x10000) {
	  requiredLength += 3;
	} else if (ch < (UTF32)0x200000) {
	  requiredLength += 4;
	} else {
	  requiredLength += 2;
	}
    }

  requiredLength++; /* zero_byte */

  return requiredLength ;
} /* utf8_required_length_from_fat_str */


/*=======================*/  
/* utf8_thin_to_temp_fat */
/*=======================*/  
/** Fills the static buffer TEMPORARY_UTF8_FATSTR with the Unicode chars
    corresponding to the UTF-8 input string.
* @param : the char * input string containing UTF-8 chars
* @return : the static TEMPORARY_UTF8_FATSTR
*/

FAT_STR utf8_thin_to_temp_fat (char *t)
{
  /* Copies string T into the static fat string TEMPORARY_UTF8_FATSTR whose
     length is adjusted as needed. Use carefully because the previous value
     is rewritten by the next call. */
  FAT_STR f;
  
  size_t actual_length = utf8_length(t);

  if (TEMPORARY_UTF8_FATSTR == NULL || actual_length >= TEMPORARY_UTF8_FATSTR_len)
    {
      TEMPORARY_UTF8_FATSTR_len = actual_length + 1;
      if (TEMPORARY_UTF8_FATSTR == NULL)
	TEMPORARY_UTF8_FATSTR = (FAT_STR) malloc((size_t) TEMPORARY_UTF8_FATSTR_len * sizeof(FAT_CHAR));
      else
	TEMPORARY_UTF8_FATSTR = (FAT_STR) realloc ((void *) TEMPORARY_UTF8_FATSTR,
				(size_t) TEMPORARY_UTF8_FATSTR_len * sizeof(FAT_CHAR));
      if (TEMPORARY_UTF8_FATSTR == NULL)
	not_enough_memory("UTF8_THIN_TO_TEMP_FAT");
    }      

  f = TEMPORARY_UTF8_FATSTR;

  ConvertUTF8strtoFAT_STR ((UTF8 **)&t, (UTF8 *)(t + strlen(t)), 
			   &f, strictConversion);

  CHARNUM(f) = '\0';
  CHARSET(f) = '\0';
  
  return (TEMPORARY_UTF8_FATSTR);
}

/* Release the global variable TEMPORARY_UTF8_FATSTR.
   (To be called by function release_fsm_memory()). */
void free_UTF8fat_string_memory()
{
    if (TEMPORARY_UTF8_FATSTR != NULL)
    {
        free(TEMPORARY_UTF8_FATSTR);
        TEMPORARY_UTF8_FATSTR= NULL;
    }
}


FAT_STR utf8_thin_to_fat (const char *t)
{
  FAT_STR f, fs;

  fs = (FAT_STR) malloc((size_t) (strlen(t) + 1) * (size_t) sizeof(FAT_CHAR));
  
  if (fs == NULL)
    not_enough_memory("UTF8_THIN_TO_FAT");
  
  f = fs;

  ConvertUTF8strtoFAT_STR ((UTF8 **)&t, (UTF8 *) (t + strlen(t)), 
			   &f, strictConversion);

  CHARNUM(f) = '\0';
  CHARSET(f) = '\0';
  
  return (fs);
}



int utf8_is_character_printable(FAT_STR fc)
/* The un-printable characters are those under 0x20, plus 0x7f (delete),  except : 
	0x08 : backspace 
	0x09 : tab
	0x0a : line feed
	0x0b : vertical tab
	0x0c : form feed
	0x0d : cr (carriage return)
	These six characters produce visible output, even if it is a kind of "space"
	The characters not cited here are considered printable 
	and do not need to be escaped to produce visible output
*/

{
	unsigned int c = CHARNUM(fc) + 256 * CHARSET(fc);
	return !( c < 0x07 || (c > 0x0d && c < 0x20 ) || c == 0x7f );
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void utf8_fat_char_to_buf (FAT_STR fc, STRING_BUFFERptr buf)
{
	char buffer[7];
	int i;

  if ((CHARSET(fc) == 0) && (127 >= CHARNUM(fc)))	// Under 128, no need do decode
    {
      append_char_to_buffer((char) CHARNUM(fc), buf);
    }
  else
    {
	  ConvertFatChartoUTF8 (fc, (UTF8 *) buffer, (UTF8*) buffer + 7, lenientConversion);
	  for (i=0; ( i < 7 ) && buffer[i] ; i++) 
	  {
		  append_char_to_buffer(buffer[i], buf);
	  }

    }
}


void utf8_fat_string_to_buf (FAT_STR fs, STRING_BUFFERptr buf)
{ 
  for (; !(CHARSET(fs) == 0 && CHARNUM(fs) == 0);)
    utf8_fat_char_to_buf(fs++, buf);
}

#endif

char * utf8_fat_to_thin (FAT_STR fs, char *s)
{
  /* Copies fat string FS to thin string S.  If S is NULL, a
     new string is allocated, otherwise characters are copied
     to S.  It is up to the caller to verify that S has
     enough space.
     */
  
  char *t, *str;
  size_t required_length;
  required_length = utf8_required_length_from_fat_str(fs);

  if (s == NULL)
    t = (char *) malloc(required_length *  sizeof(char));
  else
    t = s;
  
  if (t == NULL)
    not_enough_memory("FAT_TO_THIN");
  
  str = t;

	  ConvertFatStrtoUTF8 (fs, (UTF8 *) t, (UTF8*) (t + required_length), lenientConversion);

  
  return (t);
}



static int utf8_fprint_fat_char_new(FILE *outfile, FAT_STR fc)
{
  /* Prints \xYY  for unprintable characters */

	char buffer[7];
	int i=0;

	if (utf8_is_character_printable(fc)){
		ConvertFatChartoUTF8 (fc, (UTF8 *) buffer, (UTF8*) buffer + 7, lenientConversion);
		/*
		for (i=0; ( i < 7 ) && buffer[i] ; i++) {
			fsm_fprintf (outfile, "%c", buffer[i]);
		}
		It is probably safer to send the UTF-8 encode string as a whole
		instead of character by character. -- LK 2003-02-07
		*/
		fsm_fprintf(outfile, "%s", (char *) buffer);
		return i;
	}
	else 
	{
		fsm_fprintf(outfile, "\\x");
		fprintf(outfile,"\\x%x",CHARNUM(fc));
		return 1;
    }

}



int utf8_fprint_fat_char_esc (FILE *outfile, FAT_STR fc , char *esc_codes)
{
  char buffer[7];
  int len = 0;
  if (utf8_is_character_printable(fc)){
    /* We have a printable character */
    ConvertFatChartoUTF8 (fc, (UTF8 *) buffer, (UTF8*) buffer + 7, lenientConversion);
    for (len=0; ( len < 7 ) && buffer[len] ; len++) {
      fsm_fprintf (outfile, "%c", buffer[len]);
    }
    return len;
  }
  else 
    {
      /* We have an unprintable character */
      if (esc_codes != NULL) 
        {
          /* We have a list of chars to escape */
          if (((CHARNUM(fc)) == '%') ||
              (strchr(esc_codes, (CHARNUM(fc))) != NULL))
            {
              /* The character we have belongs to the list of characters
                 to escape */
              /* If we have a representation for it, print the representation, 
                 otherwise print it inside quotes */
              switch (CHARNUM(fc))
                {
                case '\v':
                  fsm_fprintf(outfile, "\"\\v\"");
                  len += 4;
                  break;
                case '\b':
                  fsm_fprintf(outfile, "\"\\b\"");
                  len += 4;
                  break;
                case '\r':
                  fsm_fprintf(outfile, "\"\\r\"");
                  len += 4;
                  break;
                case '\t':
                  fsm_fprintf(outfile, "\"\\t\"");
                  len += 4;
                  break;
                case '\n':
                  fsm_fprintf(outfile, "\"\\n\"");
                  len += 4;
                  break;
                case ' ':
                  fsm_fprintf(outfile, "\" \"");
                  len += 3;
                  break;
                case '"':
                  fsm_fprintf(outfile, "%%\"");
                  len += 2;
                  break;
                default:
                  len += 3;
                  fsm_fprintf (outfile, "\"%c\"", CHARNUM(fc));
                }
            }
          else 
            {
              /* The character we have does not belong to the list of characters to escape */
              /* Print it normally */
              len += utf8_fprint_fat_char_new(outfile, fc);
            }
        }
      else
        {
          /* We have an unprintable char, but no list of chars to escape */
          /* Print it normally */
          len += utf8_fprint_fat_char_new(outfile, fc);
        }
    }
  return(len);
}

int utf8_fprint_fat_char (FILE *outfile, FAT_STR fc)

{
  /* Used to print a bracketed decimal value for non-printing
     characters. We are discontinuing this non-feature. LK. 93-01-10\
     Cannot use isprint() for this test because it only works
     for seven-bit ASCII characters.
     */
  if (CHARSET(fc) == '\0')
    {
      /*      if (CHARNUM(fc) >= ' ')
	      fsm_fprintf (outfile, "%c", CHARNUM(fc));
	      else
	      fsm_fprintf (outfile, "[%d]", (int) CHARNUM(fc)); 
	      */
      fsm_fprintf (outfile, "%c", CHARNUM(fc));
      return(1);
    }
  else
    {
      fsm_fprintf (outfile, "<%d,%d>", CHARSET(fc), CHARNUM(fc));
      return(5);
    }
}

/* Prints escaped chars (including the backslash and the quote) */
int utf8_fprint_fat_char_with_esc(FILE *outfile, FAT_STR fc)
{
  int len;
  if ((CHARSET(fc) == '\0') && (CHARNUM(fc) < 128))
      switch (CHARNUM(fc))
	{
	case '\v':
	  fsm_fprintf(outfile, "\\v");
	  len = 2;
	  break;
	case '\b':
	  fsm_fprintf(outfile, "\\b");
	  len = 2;
	  break;
	case '\r':
	  fsm_fprintf(outfile, "\\r");
	  len = 2;
	  break;
	case '\t':
	  fsm_fprintf(outfile, "\\t");
	  len = 2;
	  break;
	case '\n':
	  fsm_fprintf(outfile, "\\n");
	  len = 2;
	  break;
	case '\\':
	  fsm_fprintf(outfile, "\\\\");
	  len = 2;
	  break;
	case '"':
	  fsm_fprintf(outfile, "%%\"");
	  len = 2;
	  break;
	default:
	  len = 2;
	  fsm_fprintf (outfile, "%c", CHARNUM(fc));
	}
    else
      len = utf8_fprint_fat_char_new(outfile, fc);

  return(len);
}


int utf8_fprint_fat_string (FILE *outfile, FAT_STR fs)
{
  int len = 0;
 
  while (!(CHARSET(fs) == '\0' && CHARNUM(fs) == '\0'))
/* JMC : replaced this seemingly obsolete function with the latest one */
/*     len += utf8_fprint_fat_char(outfile, fs++); */
/*	len +=	utf8_fprint_fat_char_with_esc(outfile, fs++); */
    len += utf8_fprint_fat_char_new(outfile, fs++);
  /* LK : fixed once more. No "escapes" intended by the calling functions. */
  return(len);

}


int utf8_fprint_fat_string_esc (FILE *outfile, FAT_STR fs, char *esc_codes)
{
  int len = 0;
  
  while (!(CHARSET(fs) == '\0' && CHARNUM(fs) == '\0'))
    len += utf8_fprint_fat_char_esc(outfile, fs++, esc_codes);

  return(len);
}


/* If the input is a single character, escape it, otherwise call utf8_fprint_fat_char_with_esc ? */
/* Note that the list of escaped chars is not the same on both cases. Weird. Bug ? */

int utf8_fprint_fat_string_with_esc (FILE *outfile, FAT_STR fs)
{
  int len = 0;

  if (CHARNUM(fs+1) == '\0' && CHARSET(fs+1) == '\0' && CHARSET(fs) == '\0')
    {
      switch (CHARNUM(fs)) {
      case '0':
	fsm_fprintf(outfile, "%s", "%0");
	return(2);
      case '?':
	fsm_fprintf(outfile, "%s", "%?");
	return(2);
      case '%':
	fsm_fprintf(outfile, "%s", "%%");
	return(2);
      default:
	break;
      }
    }
  
  while (!(CHARSET(fs) == '\0' && CHARNUM(fs) == '\0'))
    len += utf8_fprint_fat_char_with_esc(outfile, fs++);

  return(len);
}

char *utf8_fat_to_thin_esc (FAT_STR fs, char *s, char *esc_codes,
		       int hexa_or_unicode_always)
{
  /* Original comment : Copies fat string FS to thin string S.  If S is NULL, a
     new string is allocated, otherwise characters are copied
     to S.  It is up to the caller to verify that S has
     enough space.
     This is aimed for printing: so visible characters are printed as
     they should appear but not visible ones - like 0x82 or unicode
     ones (like 1234) - will be printed as \x82 and \u1234.
     Must be improved when there are standard methods for Unicode rendering.
  */
  /* utf-8 implementation : esc_codes was not used (only checked for NULL value).
     Now we just escape ? , 0, \, \v, \b, \r, \t, \n, and space.
     The rest is printed as UTF-8.
  */
  
  char *t, *str;
  char buffer[7];
  int i;

  if (s == NULL)
    /* Warning : doesn't guarantee that there will always be enough space */
    t = (char *) malloc((size_t) (fat_strlen(fs) + 1) *  sizeof(char));
  else
    t = s;

  if (t == NULL)
    not_enough_memory("FAT_TO_THIN");

  str = t;
  while (CHARNUM(fs) != '\0' || CHARSET(fs) != '\0')
    {
      if (CHARSET(fs) == 0)
        {
          switch (CHARNUM(fs))
            {
            case '?':
              *str++ = '\"';
              *str++ = '?'; *str++ = '\"'; fs++;
              break;
            case '0':
              *str++ = '\"';
              *str++ = '0'; *str++ = '\"'; fs++;
              break;
            case ' ':
              *str++ = '\"'; *str++ = ' '; *str++ = '\"'; fs++;
              break;
            case '\v':
              *str++ = '\"';
              *str++ = '\\'; *str++ = 'v'; *str++ = '\"'; fs++;
              break;
            case '\b':
              *str++ = '\"';
              *str++ = '\\'; *str++ = 'b'; *str++ = '\"'; fs++;
              break;
            case '\r':
              *str++ = '\"';
              *str++ = '\\'; *str++ = 'r'; *str++ = '\"'; fs++;
              break;
            case '\t':
              *str++ = '\"';
              *str++ = '\\'; *str++ = 't'; *str++ = '\"'; fs++;
              break;
            case '\n':
              *str++ = '\"';
              *str++ = '\\'; *str++ = 'n'; *str++ = '\"'; fs++;
              break;
            case '"':
              *str++ = '%'; *str++ = '"';
              break;
            default:

              if (utf8_is_character_printable(fs)){
                ConvertFatChartoUTF8 (fs, (UTF8 *) buffer, (UTF8*) buffer + 7, lenientConversion);
                for (i=0; ( i < 7 ) && buffer[i] ; i++) 
                  {
                    *str++ = buffer[i];
                  }
              }
              else
                {
                  sprintf(str,"\\x%.2x",CHARNUM(fs));
                  str += 4;
                }

            }					
        }
      else
        {
          if (utf8_is_character_printable(fs)){
            ConvertFatChartoUTF8 (fs, (UTF8 *) buffer, (UTF8*) buffer + 7, lenientConversion);
            for (i=0; ( i < 7 ) && buffer[i] ; i++) 
              {
                *str++ = buffer[i];
              }
          }
          else
            {
              *str++ = '\\';
              *str++ = 'x';
              sprintf(str,"%x",CHARNUM(fs));
            }
        }
      fs++;
    }
  *str = '\0';
  return (t);
}


/*========================*/
/* ConvertUTF16_1toFatStr */
/*========================*/
/** 
* Takes a single Unicode char coded in UTF-16 (1 short) and fills a FAT_STR, 
* taking care of endianness. No check is performed.
* Only the UTF-16 codes below UNI_MAX_BMP should be given to this function.
* @param int * ic : input 
* @param FAT_STR * fatStr. Must be at least 2 FAT_CHAR long (4 bytes)
*/

void ConvertUTF16_1toFatStr (unsigned int ic, FAT_STR fatStr){

  CHARSET(fatStr) = ic / 256;
  CHARNUM(fatStr) = ic & 255U;

  /* terminate the string */
  CHARSET(fatStr + 1) = 0;
  CHARNUM(fatStr + 1) = 0;
}
