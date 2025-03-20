#ifndef UTF8_UTIL_H
#define UTF8_UTIL_H

#include <stdio.h>
#include "fst/fat-str.h"

#include "fst/ConvertUTF.h"

#ifdef __cplusplus
extern "C" {
#endif


ConversionResult FstBaseExport ConvertUTF8toUTF16_1 (
		const UTF8** sourceStart, const UTF8* sourceEnd, 
		UTF16 * pUtf16, ConversionFlags flags);

ConversionResult ConvertUTF8toUTF16_1_length_unknown (
		const UTF8** sourceStart, 
		UTF16 * pUtf16, ConversionFlags flags);

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
	extern "C" }
#endif

#endif
