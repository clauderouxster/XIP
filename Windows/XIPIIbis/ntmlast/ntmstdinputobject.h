#ifndef NTMSTDINPUTOBJECT
#define NTMSTDINPUTOBJECT

#include "ConvertUTF.h"

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct ntmCStringInputObject {
	unsigned char *pString;
	size_t length;
    size_t nextReadPos;
} ntmCStringInputObject;




typedef struct ntmCFileInputObject {
	FILE *pFile;
} ntmCFileInputObject;

#define NTM_UTF8_BUFFER_SIZE 7

typedef struct ntmUTF8StringInputObject {
  unsigned char *s;                                  /* The string we read from*/
  UTF8 * utf8_start;				/* Next not-decoded character in the UTF-8 buffer	*/
  UTF8 * utf8_end;	
  UTF16 * utf16_start;				/* Next UTF-16 char in the UTF-16 buffer to return	*/
  UTF16 * utf16_end;					
  UTF8 utf8_buf[NTM_UTF8_BUFFER_SIZE];		/* Contains the characters read from the string	*/
  UTF16 utf16_buf[NTM_UTF8_BUFFER_SIZE];	/* Contains the UTF-16 chars decoded			*/
} ntmUTF8StringInputObject;


/* Buffers are empty when start == end */
typedef struct ntmUTF8FileInputObject {
  FILE *pFile;                                  /* The file we read from*/
  UTF8 * utf8_start;				/* Next not-decoded character in the UTF-8 buffer	*/
  UTF8 * utf8_end;	
  UTF16 * utf16_start;				/* Next UTF-16 char in the UTF-16 buffer to return	*/
  UTF16 * utf16_end;					
  UTF8 utf8_buf[NTM_UTF8_BUFFER_SIZE];		/* Contains the characters read from the file	*/
  UTF16 utf16_buf[NTM_UTF8_BUFFER_SIZE];	/* Contains the UTF-16 chars decoded			*/
} ntmUTF8FileInputObject;

int readId(Levels *pLevel, id_type *pId);


#ifdef __cplusplus
}
#endif

#endif
