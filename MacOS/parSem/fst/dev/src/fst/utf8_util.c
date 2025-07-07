/* $Id: utf8_util.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 2003 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  UTF8_UTIL.C
 **
 **  JEAN-MARC COURSIMAULT, XRCE 2003
 **
 **  Utf-8 conversion to/from 16bits utilities.
 **
 ******************************************************/


#include "fst/utf8_util.h"

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif
#include "fst/ConvertUTF.h"


/*======================*/
/* ConvertUTF8toUTF16-1 */
/*======================*/
/** 
* Convert a single UTF-8 char (maybe coded of several bytes) from a buffer to UTF-16.
* Only UTF-8 chars that can be coded in UTF-16 on 16 bits are accepted.
* All others are replaced by UNI_REPLACEMENT_CHAR
* The result is a unsigned short.
* @param sourceStart : pointer to the address of the beginning of the string to translate. 
*                      That will be updated by this function to the next UTF-8 char.
* @param sourceEnd: the address of the char after the last in the input string
* @param pUTF16 : the address of the short to fill
* @param flags : either strictConversion or lenientConversion. See ConvertUTF.h for details.
* @return : ConversionResult . See ConvertUTF.h for details.
*/

ConversionResult ConvertUTF8toUTF16_1 (
		const UTF8** sourceStart, const UTF8* sourceEnd, 
		UTF16 * pUtf16, ConversionFlags flags) {

	ConversionResult result = conversionOK;
	const UTF8* source = *sourceStart;

		UTF32 ch = 0;
		unsigned short extraBytesToRead = trailingBytesForUTF8[*source];

		if (source + extraBytesToRead >= sourceEnd) {
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
			*pUtf16 = (unsigned short)ch;
		}
	*sourceStart = source;
	return result;
}

/*=====================================*/
/* ConvertUTF8toUTF16-1_length_unknown */
/*=====================================*/
/** 
* Convert a single UTF-8 char (maybe coded of several bytes) from a buffer to UTF-16.
* Only UTF-8 chars that can be coded in UTF-16 on 16 bits are accepted.
* All others are replaced by UNI_REPLACEMENT_CHAR
* We don't know a priori what the length of the string is. It's zero-terminated, though.
* The result is a unsigned short.
* @param sourceStart : pointer to the address of the beginning of the string to translate. 
*                      That will be updated by this function to the next UTF-8 char.
* @param pUTF16 : the address of the short to fill
* @param flags : either strictConversion or lenientConversion. See ConvertUTF.h for details.
* @return : ConversionResult . See ConvertUTF.h for details.
*/

ConversionResult ConvertUTF8toUTF16_1_length_unknown (
		const UTF8** sourceStart, 
		UTF16 * pUtf16, ConversionFlags flags) {

	ConversionResult result = conversionOK;
	const UTF8* source = *sourceStart;

		UTF32 ch = 0;
		unsigned short extraBytesToRead = trailingBytesForUTF8[*source];


		/* Do this check whether lenient or strict */
		if (! isLegalUTF8(source, extraBytesToRead+1)) {
			*pUtf16 = UNI_REPLACEMENT_CHAR;
			*sourceStart = ++source;
			return sourceIllegal;
		}


		switch (extraBytesToRead) {
			case 3:	
				if(0 == *source) {
					*pUtf16 = UNI_REPLACEMENT_CHAR;
					*sourceStart = source++;
					return  sourceExhausted;
				}
				else
				{
					ch += *source++; ch <<= 6;
				}
			case 2:
				if(0 == *source) {
					*pUtf16 = UNI_REPLACEMENT_CHAR;
					*sourceStart = source++;
					return  sourceExhausted;
				}
				else
				{
					ch += *source++; 
					ch <<= 6;
				}

			case 1:
					if(0 == *source) {
					*pUtf16 = UNI_REPLACEMENT_CHAR;
					*sourceStart = source++;
					return  sourceExhausted;
				}
				else
				{
					ch += *source++; 
					ch <<= 6;
				}

			case 0:
				if(0 == *source) {
					*pUtf16 = UNI_REPLACEMENT_CHAR;
					*sourceStart = source++;
					return  sourceExhausted;
				}
				else
				{
					ch += *source++;
				}
		}

		ch -= offsetsFromUTF8[extraBytesToRead];

		if (ch > UNI_MAX_BMP) 
		{
			/* Just replace characters above this limit */
			ch = UNI_REPLACEMENT_CHAR;
		}

		if ((flags == strictConversion) && (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END)) {
			source -= (extraBytesToRead+1); /* return to the illegal value itself */
			source ++;						/* plus one character otherwise we will loop */
			*pUtf16 = UNI_REPLACEMENT_CHAR;
			result = sourceIllegal;
		} else {
			*pUtf16 = (unsigned short)ch;
		}
	*sourceStart = source;
	return result;
}

