
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ntm.h"
#include "ntmstdinputobject.h"
#include "fat-str.h"
#include "utf8_util.h"
#include "utf8_fatstr_util.h"

extern void ntmReset(NtmContext* ct);
extern void errorHandler(NtmContext *ct, const char *message, ...);
//extern id_type inputIdVect[NTM_ID_SIZE+1];

/* UNI_SUR_HIGH_START is already defined in ConvertUTF.c */
#define UNI_SUR_HIGH_START(UTF32)0xD800


/*=====================*/
/* ntmClearInputObject */
/*=====================*/
void ntmClearInputObject(NtmContext* ct) {
    /** Clears any previously allocated input object
     * This is an API function
     * @param ct : NtmContext containing the input object
     */
    if (ct->levels.closeInputObject) {
        ct->levels.closeInputObject(ct->levels.pInputObject);
        ct->levels.closeInputObject = NULL;
        ct->levels.pInputObject = NULL;
    }
}
/*-------------------------------------*/
/* char * based string input functions */
/*-------------------------------------*/

/*==========================*/
/* ntmCStringSetInputObject */
/*==========================*/
/**
 * Fills a structure containing a pointer to the string to read,
 * its length and the pointer to the next char to read.
 * Internal function. Not to be called by an user app.
 * @param pInputObject : pointer to the InputObject to fill
 * @param pString : the char * containing the input data
 * @return 1 if problem (currently null pString pointer), 0 if OK.
 */
int ntmCStringSetInputObject( void *pInputObject, unsigned char *pString) {
    if (NULL == pString) return 1; /* No string supplied */
    ((ntmCStringInputObject*)pInputObject)->pString = pString;
    ((ntmCStringInputObject*)pInputObject)->length = strlen((char *) ((ntmCStringInputObject*)pInputObject)->pString);
    ((ntmCStringInputObject*)pInputObject)->nextReadPos = 0;
    return 0;
}

/*============================*/
/* ntmCStringClearInputObject */
/*============================*/
/**
 * Frees a CStringSetInputObject
 * Calls free() on the input object.
 * Internal function. Not to be called by an user app.
 * @param pInputObject : pointer to the input object
 * @return Always 0 (means OK)
 */
int ntmCStringClearInputObject( void *pInputObject){
    if(pInputObject) {
        free(pInputObject);
    }
    return 0;
}


/*===========================*/
/* ntmCStringReadInputObject */
/*===========================*/
/** Reads the next char from a string containing 8-bit strings (not Unicode)
 * Internal function. Not to be called by an user app.
 * @param pInputObject : must have been setup with ntmCStringSetInputObject
 * @param p : a pointer to the integer that will contain the character just read
 * @return Returns EOF or 0 if a character has been read.
 */
int ntmCStringReadInputObject( void *pInputObject, int *p) {
    if (((ntmCStringInputObject*)pInputObject)->nextReadPos == ((ntmCStringInputObject*)pInputObject)->length) {
        return EOF;
    }
    *p = ((ntmCStringInputObject*)pInputObject)->pString[((ntmCStringInputObject*)pInputObject)->nextReadPos++];
    
    return 0;
}
/*======================*/
/* ntmCcharCodePointLen */
/*======================*/
/* Returns the length of a non-UTF-8 char */
int ntmCcharCodePointLen(int ic){ return 1;}

/*=======================*/
/* ntmSetInputTextString */
/*=======================*/
/**
 * Sets a NTM context to read a string containing 8bit chars (not Unicode)
 * This is an API function.
 * @param ct : pointer to the context to fill
 * @param pString : the C string to read
 */
void ntmSetInputTextString(NtmContext* ct, unsigned char* pString) {
    
    /* Create a ntmCStringInputObject */
    void *pInputObject;
    
    /* Clear any previously allocated input object */
    ntmClearInputObject(ct);
    
    pInputObject = malloc(sizeof (ntmCStringInputObject));
    if (! pInputObject) {
        errorHandler(ct, "Cannot allocate ntmCStringInputObject");
        return;
    }
    /* Initialize it */
    if (ntmCStringSetInputObject( pInputObject, pString)) {
        errorHandler(ct, "Input string is NULL!");
        free(pInputObject);
        return;
    }
    
    ntmReset(ct);
    ct->levels.pInputObject = pInputObject;
    
    /* Set the functions that open, close and read from this string */
    ct->levels.closeInputObject = ntmCStringClearInputObject;
    ct->levels.readInputObject = ntmCStringReadInputObject;
    ct->levels.getInputCodePointLen = ntmCcharCodePointLen;
    /* cfsm init function */
    init_string_functions(CHAR_ENC_ISO_8859_1);
    
}

/*-----------------------------------*/
/* char * based File input functions */
/*-----------------------------------*/

/*==========================*/
/* ntmCFileClearInputObject */
/*==========================*/
/**
 * Frees a CFileSetInputObject
 * Does nothing in this case since the input object is simply a FILE *,
 * there is nothing to free. Needed by the API.
 * Internal function. Not to be called by an user app.
 * @param pInputObject : pointer to the input object
 * @return Always 0 (means OK)
 */
int ntmCFileClearInputObject( void *pInputObject){
    return 0;
}

/*=========================*/
/* ntmCFileReadInputObject */
/*=========================*/
/** Reads the next char from a file containing 8-bit strings (not Unicode)
 * Internal function. Not to be called by an user app.
 * @param pInputObject : in this case, it is only a FILE *
 * @param p : a pointer to the integer that will contain the character just read
 * @return Returns EOF or 0 if a character has been read.
 */
int ntmCFileReadInputObject( void *pInputObject, int *p) {
    if( EOF == (*p = getc((FILE *)pInputObject)))	{
        return EOF;
    }
    else {
        return 0;
    }
}



/*=======================*/
/* ntmSetInputTextStream */
/*=======================*/
/**
 * Sets a NTM context to read a stream containing 8bit chars (not Unicode)
 * This is an API function.
 * @param ct : pointer to the context to fill
 * @param stream : the FILE * of the file to read.
 */
void ntmSetInputTextStream(NtmContext* ct, FILE* stream) {
    
    if (NULL == stream) {
        errorHandler(ct, "Input stream is NULL!");
        return;
    }
    ntmReset(ct);
    /* Clear any previously allocated input object */
    ntmClearInputObject(ct);
    
    ct->levels.pInputObject = (void *) stream;
    
    /* Set the functions that open, close and read from this string */
    ct->levels.closeInputObject = ntmCFileClearInputObject;
    ct->levels.readInputObject = ntmCFileReadInputObject;
    ct->levels.getInputCodePointLen = ntmCcharCodePointLen;
    
    init_string_functions(CHAR_ENC_ISO_8859_1);
}


/*----------------------------------*/
/* UTF-8 based File input functions */
/*----------------------------------*/




/* --------------------------------------------------------------------- */

/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 */
extern const char trailingBytesForUTF8[256];


/*===========================*/
/* ntmUTF8FileSetInputObject */
/*===========================*/
/**
 * Fills a structure containing a pointer to the string to read,
 * its length and the pointer to the next char to read.
 * Internal function. Not to be called by an user app.
 * @param pInputObject : pointer to the InputObject to fill
 * @param pString : the char * containing the input data
 * @return 1 if problem (currently null pString pointer), 0 if OK.
 */

int ntmUTF8FileSetInputObject( void *pInputObject, FILE * stream) {
    if (NULL == stream) return 1; /* No stream supplied */
    ((ntmUTF8FileInputObject*)pInputObject)->pFile = stream;
    ((ntmUTF8FileInputObject*)pInputObject)->utf8_start = ((ntmUTF8FileInputObject*)pInputObject)->utf8_buf;
    ((ntmUTF8FileInputObject*)pInputObject)->utf8_end =   ((ntmUTF8FileInputObject*)pInputObject)->utf8_buf;
    ((ntmUTF8FileInputObject*)pInputObject)->utf16_start = ((ntmUTF8FileInputObject*)pInputObject)->utf16_buf;
    ((ntmUTF8FileInputObject*)pInputObject)->utf16_end =   ((ntmUTF8FileInputObject*)pInputObject)->utf16_buf;
    memset((void *)(((ntmUTF8FileInputObject*)pInputObject)->utf8_buf), 0, NTM_UTF8_BUFFER_SIZE * sizeof(UTF8));
    memset((void *)(((ntmUTF8FileInputObject*)pInputObject)->utf16_buf), 0, NTM_UTF8_BUFFER_SIZE * sizeof(UTF16));
    return 0;
}

/*===========================*/
/* ntmUTF8FileSetInputObject */
/*===========================*/
/**
 * Fills a structure containing a pointer to the string to read,
 * its length and the pointer to the next char to read.
 * Internal function. Not to be called by an user app.
 * @param pInputObject : pointer to the InputObject to fill
 * @param pString : the char * containing the input data
 * @return 1 if problem (currently null pString pointer), 0 if OK.
 */

int ntmUTF8StringSetInputObject( void *pInputObject, unsigned char* pString) {
    int length;
    if (NULL == pString) return 1; /* No stream supplied */
    ((ntmUTF8StringInputObject*)pInputObject)->s = pString;
    length=strlen( (char*) pString);
    ((ntmUTF8StringInputObject*)pInputObject)->used=0;
    
    ((ntmUTF8StringInputObject*)pInputObject)->utf8_start = ((ntmUTF8StringInputObject*)pInputObject)->utf8_buf;
    ((ntmUTF8StringInputObject*)pInputObject)->utf8_end =   ((ntmUTF8StringInputObject*)pInputObject)->utf8_buf;
    ((ntmUTF8StringInputObject*)pInputObject)->utf16_start = ((ntmUTF8StringInputObject*)pInputObject)->utf16_buf;
    ((ntmUTF8StringInputObject*)pInputObject)->utf16_end =   ((ntmUTF8StringInputObject*)pInputObject)->utf16_buf;
    ((ntmUTF8StringInputObject*)pInputObject)->utf8_buf = (UTF8*)malloc(length*sizeof(UTF8));
    ((ntmUTF8StringInputObject*)pInputObject)->utf16_buf = (UTF16*)malloc(length*sizeof(UTF16));
    return 0;
}

/*=============================*/
/* ntmUTF8FileClearInputObject */
/*=============================*/
/**
 * Frees a UTF8FileInputObject
 * Calls free() on the input object.
 * Internal function. Not to be called by an user app.
 * @param pInputObject : pointer to the input object
 * @return Always 0 (means OK)
 */
int ntmUTF8FileClearInputObject( void *pInputObject){
    free(pInputObject);
    pInputObject = 0;
    return 0;
}

/*=============================*/
/* ntmUTF8StringClearInputObject */
/*=============================*/
/**
 * Frees a UTF8StringInputObject
 * Calls free() on the input object.
 * Internal function. Not to be called by an user app.
 * @param pInputObject : pointer to the input object
 * @return Always 0 (means OK)
 */
int ntmUTF8StringClearInputObject( void *pInputObject){
    ntmUTF8StringInputObject* pI=(ntmUTF8StringInputObject*)pInputObject;
    free(pI->utf16_buf);
    free(pI->utf8_buf);
    free(pInputObject);
    pInputObject = 0;
    return 0;
}

/*=============================*/
/* ntmUTF-8FileReadInputObject */
/*=============================*/
/** Reads the next char from a file containing UTF-8 Unicode strings
 * Internal function. Not to be called by an user app.
 * @param pInputObject : in this case, it is only a FILE *
 * @param p : a pointer to the integer that will contain the character just read, coded in UTF-16 (only for the first 16 bits)
 * @return Returns EOF or 0 if a character has been read.
 U-00000000 - U-0000007F: 0xxxxxxx
 U-00000080 - U-000007FF: 110xxxxx 10xxxxxx
 U-00000800 - U-0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
 U-00010000 - U-001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 U-00200000 - U-03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 U-04000000 - U-7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

 To decode multi-bytes encodings : shift left 6 bits the previous result, add the current byte.
 When done, remove the leading bytes that carry no meaning, ie :

 2 bytes : subtract 110.....10......
 3 bytes : subtract 1110....10....10......
 4 bytes : subtract 11110...10....10....10......
 etc.. inserting 10.... in the middle for each additional byte.
 The dots really mean zero, but I write it so for clarity.

 */

int ntmUTF8FileReadInputObject( void *pInputObject, int *p) {
    int i;
    UTF8 * source;
    UTF16 * target;
    ConversionResult conv_result;
    
    /* helps the debugging, let the compiler optimize it out */
    ntmUTF8FileInputObject*  pI = (ntmUTF8FileInputObject*)pInputObject;
    
    /* Do we have anything in the utf-16 buffer ? */
    
    if (pI->utf16_start < pI->utf16_end) {
        /* Yes, just return it */
        *p = *(pI->utf16_start ++);
        return 0;
    }
    
    /* We need to read more from the file */
    
    /* First move the incomplete UTF-8 chars to the beginning of the buffer */
    i = 0 ;
    while (pI->utf8_start < pI->utf8_end) {
        (pI->utf8_buf[i++] =
        *(pI->utf8_start ++));
    }
    pI->utf8_start = pI->utf8_buf;
    pI->utf8_end   = pI->utf8_buf + i;
    pI->utf16_start = pI->utf16_buf;
    pI->utf16_end   = pI->utf16_buf;
    
    /* read what we can */
    if (! feof(pI->pFile)) {
        size_t current_length = pI->utf8_end - pI->utf8_start;
        
        current_length += fread(pI->utf8_buf + current_length,
        1,
        NTM_UTF8_BUFFER_SIZE - current_length,
        pI->pFile );
        pI->utf8_end   = pI->utf8_buf + current_length;
    }
    
    /* Is what we have long enough ? */
    
    if (pI->utf8_start == pI->utf8_end) {
        /* Nothing left, report EOF */
        return EOF;
    }
    
    /* Decode what we have in the buffer */
    source = pI->utf8_start;
    target = pI->utf16_start;
    conv_result = conversionOK;
    while ( source < pI->utf8_end) {
        conv_result = ConvertUTF8toUTF16_1(
        (const UTF8**)&source,
        pI->utf8_end,
        pI->utf16_end,
        lenientConversion);
        if ( conversionOK == conv_result ) {
            pI->utf16_end++;
        }
        else {
            if ( sourceIllegal == conv_result ) {
                fprintf(stderr, "\n%s\n\n", "The input data contains an illegal UTF-8 character");
                return -2;
            }
            else break; /* source or target exhausted, return what we have */
        }
        
    }
    pI->utf8_start = source;
    if (pI->utf16_start < pI->utf16_end) {
        *p = *(pI->utf16_start ++);
        return 0;
    }
    else {
        return EOF;
    }
} /* ntmUTF8FileReadInputObject */

/*=============================*/
/* ntmUTF-8StringReadInputObject */
/*=============================*/
/** Reads the next char from a file containing UTF-8 Unicode strings
 * Internal function. Not to be called by an user app.
 * @param pInputObject : in this case, it is only a FILE *
 * @param p : a pointer to the integer that will contain the character just read, coded in UTF-16 (only for the first 16 bits)
 * @return Returns EOF or 0 if a character has been read.
 U-00000000 - U-0000007F: 0xxxxxxx
 U-00000080 - U-000007FF: 110xxxxx 10xxxxxx
 U-00000800 - U-0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
 U-00010000 - U-001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 U-00200000 - U-03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 U-04000000 - U-7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

 To decode multi-bytes encodings : shift left 6 bits the previous result, add the current byte.
 When done, remove the leading bytes that carry no meaning, ie :

 2 bytes : subtract 110.....10......
 3 bytes : subtract 1110....10....10......
 4 bytes : subtract 11110...10....10....10......
 etc.. inserting 10.... in the middle for each additional byte.
 The dots really mean zero, but I write it so for clarity.

 */

int ntmUTF8StringReadInputObject( void *pInputObject, int *p) {
    UTF8 * source;
    UTF16 * target;
    ConversionResult conv_result;
    int length;
    
    
    /* helps the debugging, let the compiler optimize it out */
    ntmUTF8StringInputObject*  pI = (ntmUTF8StringInputObject*)pInputObject;
    
    
    /* Do we have anything in the utf-16 buffer ? */
    
    if (pI->utf16_start < pI->utf16_end) {
        /* Yes, just return it */
        *p = *(pI->utf16_start ++);
        return 0;
    }
    
    if (pI->used==1)
        return EOF;
    
    length=strlen((char *)pI->s);
    pI->utf8_start = pI->utf8_buf;
    pI->utf8_end   = pI->utf8_buf + length;
    pI->utf16_start = pI->utf16_buf;
    pI->utf16_end   = pI->utf16_buf;
    
    
    
    memcpy(pI->utf8_buf, pI->s, length);
    
    pI->utf8_buf=pI->utf8_buf;
    pI->utf8_start=pI->utf8_buf;
    pI->utf8_end   = pI->utf8_buf + length;
    
    
    /* Is what we have long enough ? */
    
    if (pI->utf8_start == pI->utf8_end) {
        /* Nothing left, report EOF */
        return EOF;
    }
    
    pI->used=1;
    
    /* Decode what we have in the buffer */
    source = pI->utf8_start;
    target = pI->utf16_start;
    conv_result = conversionOK;
    while ( source < pI->utf8_end) {
        conv_result = ConvertUTF8toUTF16_1(
        (const UTF8**)&source,
        pI->utf8_end,
        pI->utf16_end,
        lenientConversion);
        if ( conversionOK == conv_result ) {
            pI->utf16_end++;
        }
        else {
            if ( sourceIllegal == conv_result ) {
                fprintf(stderr, "\n%s\n\n", "The input data contains an illegal UTF-8 character");
                /* errorHandler(); */
                return -2;
            }
            else break; /* source or target exhausted, return what we have */
        }
        
    }
    pI->utf8_start = source;
    if (pI->utf16_start < pI->utf16_end) {
        *p = *(pI->utf16_start ++);
        return 0;
    }
    else {
        return EOF;
    }
} /* ntmUTF8StringReadInputObject */


/*=====================*/
/* ntmUTF8CodePointLen */
/*=====================*/

/* Returns the UTF-8 length of a Unicode code point */
int ntmUTF8CodePointLen(int ic){
    if (ic < 0x80){
        return 1;
    }
    else {
        if (ic < 0x800) {
            return 2;
        }
        /* We probably will never go farther, but having a routine that
         works with all values is a Good Idea (TM) */
        else {
            if (ic < 0x10000) {
                return 3;
            }
            else {
                if (ic < 0x200000) {
                    return 4;
                }
                else {
                    if (ic < 0x400000) {
                        return 5;
                    }
                    else {
                        return 6;
                    }
                }
            }
        }
    }
}



/*========================*/
/* ntmSetInputUTF8Stream */
/*========================*/
/**
 * Sets a NTM context to read a stream containing UTF-8 Unicode chars
 * This is an API function.
 * @param ct : pointer to the context to fill
 * @param stream : the FILE * of the file to read.
 */
void ntmSetInputUTF8Stream(NtmContext* ct, FILE* stream) {
    
    /* Create a ntmUTF8FileInputObject */
    void *pInputObject;
    
    /* Clear any previously allocated input object */
    ntmClearInputObject(ct);
    
    pInputObject = malloc(sizeof (ntmUTF8FileInputObject));
    if (! pInputObject) {
        errorHandler(ct, "Cannot allocate ntmUTF8FileInputObject");
        return;
    }
    /* Initialize it */
    if (ntmUTF8FileSetInputObject( pInputObject, stream)) {
        errorHandler(ct, "Input stream is NULL!");
        free(pInputObject);
        return;
    }
    
    ntmReset(ct);
    ct->levels.pInputObject = pInputObject;
    
    /* Set the functions that open, close and read from this string */
    ct->levels.closeInputObject = ntmUTF8FileClearInputObject;   /* dealloc the object */
    ct->levels.readInputObject = ntmUTF8FileReadInputObject; /* Only the way to read the file is different */
    ct->levels.getInputCodePointLen = ntmUTF8CodePointLen;
    
    init_string_functions(CHAR_ENC_UTF_8);
}


/*========================*/
/* ntmSetInputUTF8String */
/*========================*/
/**
 * Sets a NTM context to read a stream containing UTF-8 Unicode chars
 * This is an API function.
 * @param ct : pointer to the context to fill
 * @param stream : the FILE * of the file to read.
 */
void ntmSetInputUTF8String(NtmContext* ct, unsigned char* pString) {
    
    /* Create a ntmCStringInputObject */
    void *pInputObject;
    
    /* Clear any previously allocated input object */
    ntmClearInputObject(ct);
    
    pInputObject = malloc(sizeof (ntmUTF8StringInputObject));
    if (! pInputObject) {
        errorHandler(ct, "Cannot allocate ntmCStringInputObject");
        return;
    }
    /* Initialize it */
    if (ntmUTF8StringSetInputObject( pInputObject, pString)) {
        errorHandler(ct, "Input string is NULL!");
        free(pInputObject);
        return;
    }
    
    ntmReset(ct);
    ct->levels.pInputObject = pInputObject;
    
    /* Set the functions that open, close and read from this string */
    ct->levels.closeInputObject = ntmUTF8StringClearInputObject;   /* dealloc the object */
    ct->levels.readInputObject = ntmUTF8StringReadInputObject; /* Only the way to read the file is different */
    ct->levels.getInputCodePointLen = ntmUTF8CodePointLen;
    
    init_string_functions(CHAR_ENC_UTF_8);
}


/*==============================*/
/* The standard readId function */
/*==============================*/
/**
 * readId gets the Id of the next character (potentially Unicode) from an InputObject.
 * It updates inputIdVect if necessary.
 * @param pLevel : the ntmLevel from the ntmContext
 * (that was setup by ntmSetInputTextStream, ntmSetInputTextString or other)
 * @param pId : the C-FSM Id corresponding to the input character
 * @return EOF if end of input, 0 if OK, conversionError if an error occurred.
 */
int readId(Levels *pLevel, id_type *pId) {
    int ic;
    char error;
    FAT_CHAR fatStr[2];
    if ((error=pLevel->readInputObject(pLevel->pInputObject, &ic))) {
        *pId = ID_EOT;
        if (error==-2)
            return -2;
        return EOF;
    }
    return getInternalCharId(pLevel, ic, pId);
    
    /* if ( 0 == pLevel->inputIdVect[ic]){
        ConvertUTF16_1toFatStr(ic, fatStr); /* Not specific to UTF-8, works also in non-UTF8 mode 
        if(intern_atomic_label(pId, fatStr)) {
            /* There was an error in the conversion 
            *pId = ID_EOT;
            return EOF;
        }
        pLevel->inputIdVect[ic] = *pId;
        pLevel->inputIdLen[*pId] = pLevel->getInputCodePointLen(ic);
    }
    else {
        *pId = pLevel->inputIdVect[ic];
    }
    return 0; */
}


inline int getInternalCharId(Levels *pLevel, int ic, id_type *pId) {
    FAT_CHAR fatStr[2];
    if ( 0 == pLevel->inputIdVect[ic]){
        ConvertUTF16_1toFatStr(ic, fatStr); /* Not specific to UTF-8, works also in non-UTF8 mode */
        if(intern_atomic_label(pId, fatStr)) {
            /* There was an error in the conversion */
            *pId = ID_EOT;
            return EOF;
        }
        pLevel->inputIdVect[ic] = *pId;
        /* pLevel->inputIdLen[*pId] = pLevel->getInputCodePointLen(ic); */
        pLevel->inputIdLen[*pId] = ntmUTF8CodePointLen(ic);
    }
    else {
        *pId = pLevel->inputIdVect[ic];
    }
    return 0;
}



/* --------------------------------------------------------------------- */

