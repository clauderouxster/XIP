/* $Id: machine.c,v 1.7 2009/03/03 19:23:39 grondeau Exp $ */
/* Copyright (c) 1990-93 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

#include "fst/machine.h"
#include "fst/fsm_io.h"

/******************************************************
 **
 **  MACHINE.C
 **
 **  Yves Monier, RXRC, February 1996
 **
 **  Portable functions to read/write binary data on
 **  different platforms.
 **
 ******************************************************/

fst_byte next_byte(FILE *stream, ALONG *byte_countp)
{
  int c;

  if ((c = fsm_fgetc(stream)) == EOF)
    {
      fsm_fprintf(stderr, "Unexpected end-of-file\n");
      exit(1);
    }

  (*byte_countp)++;

  return(c);
} /* next_byte */


unsigned int next_two_bytes(FILE *stream, ALONG *byte_countp)
{
  unsigned int b = 0;
  
#if !defined(_M_IX86) && !defined(FSTWIN64) && !defined(ARM)	
  fread (&b, (size_t) 2, (size_t) 1, stream);
  
  b = b >> 8*(sizeof(b) - 2);

#else

  int c1, c2;

  if ((c1 = fsm_fgetc(stream)) == EOF)
    {
      fsm_fprintf(stderr, "Unexpected end-of-file\n");
      exit(1);
    }
  if ((c2 = fsm_fgetc(stream)) == EOF)
    {
      fsm_fprintf(stderr, "Unexpected end-of-file\n");
      exit(1);
    }
  b = c1 * 256 + c2;

#endif

  *byte_countp += 2;
  
  return b;
}

ALONG next_three_bytes(FILE *stream, ALONG *byte_countp)
{
  ALONG b = 0;
  
#if  !defined(_M_IX86) && !defined(FSTWIN64)	&& !defined(ARM)	
	
  fread (&b, (size_t) 3, (size_t) 1, stream);
  
  b = b >> 8*(sizeof(b) - 3);

#else

  int c1, c2, c3;

  if ((c1 = fsm_fgetc(stream)) == EOF)
    {
      fsm_fprintf(stderr, "Unexpected end-of-file\n");
      exit(1);
    }
  if ((c2 = fsm_fgetc(stream)) == EOF)
    {
      fsm_fprintf(stderr, "Unexpected end-of-file\n");
      exit(1);
    }
  if ((c3 = fsm_fgetc(stream)) == EOF)
    {
      fsm_fprintf(stderr, "Unexpected end-of-file\n");
      exit(1);
    }
  b = c1 * 65536 + c2 * 256 + c3;

#endif

  *byte_countp += 3;
  
  return b;
}

ALONG next_four_bytes(FILE *stream, ALONG *byte_countp)
{
  ALONG b = 0;
  
#if  !defined(_M_IX86) && !defined(FSTWIN64)	&& !defined(ARM)	

  fread (&b, (size_t) 4, (size_t) 1, stream);
  
  b = b >> 8*(sizeof(b) - 4);

#else

  int c1, c2, c3, c4;

  if ((c1 = fsm_fgetc(stream)) == EOF)
    {
      fsm_fprintf(stderr, "Unexpected end-of-file\n");
      exit(1);
    }
  if ((c2 = fsm_fgetc(stream)) == EOF)
    {
      fsm_fprintf(stderr, "Unexpected end-of-file\n");
      exit(1);
    }
  if ((c3 = fsm_fgetc(stream)) == EOF)
    {
      fsm_fprintf(stderr, "Unexpected end-of-file\n");
      exit(1);
    }
  if ((c4 = fsm_fgetc(stream)) == EOF)
    {
      fsm_fprintf(stderr, "Unexpected end-of-file\n");
      exit(1);
    }
  b = c1 * 16777216 + c2 * 65536 + c3 * 256 + c4;

#endif

  *byte_countp += 4;
  
  return b;
} /* next_four_bytes */


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/*************************************************************
 *                                                           *
 * functions for writing binary files *
 *                                                           *
 *************************************************************/

void write_byte(fst_byte b, FILE *stream, ALONG *byte_countp)
{
  if (fputc(b, stream) == EOF)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }

  (*byte_countp)++;
}


void write_two_bytes(unsigned int b, FILE *stream, ALONG *byte_countp)
{
#if  !defined(_M_IX86) && !defined(FSTWIN64) && !defined(ARM)	

  b <<= 8*(sizeof(b) - 2);

  if (fwrite(&b, (size_t) 1, (size_t) 2, stream) < 2)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }

#else

	int c1, c2;

	c1 = b / 256;
	c2 = b % 256;

  	if (fputc(c1, stream) == EOF)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }
  	if (fputc(c2, stream) == EOF)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }
#endif

  *byte_countp += 2;
}

void write_three_bytes(ALONG b,  FILE *stream, ALONG *byte_countp)
{
#if  !defined(_M_IX86) && !defined(FSTWIN64)	&& !defined(ARM)	
	
  b <<= 8*(sizeof(b) - 3);
  if (fwrite (&b, (size_t) 1, (size_t) 3, stream) < 3)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }

#else

	int c1, c2, c3;

	c1 = b / 65536;
	c2 = (b - c1 * 65536) / 256;
	c3 = (b - c1 * 65536) % 256;

  	if (fputc(c1, stream) == EOF)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }
  	if (fputc(c2, stream) == EOF)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }
  	if (fputc(c3, stream) == EOF)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }
#endif

  *byte_countp += 3;
}

void write_four_bytes(ALONG b, FILE *stream, ALONG *byte_countp)
{
#if  !defined(_M_IX86) && !defined(FSTWIN64)	&& !defined(ARM)	

  b <<= 8*(sizeof(b) - 4);
  if (fwrite (&b, (size_t) 1, (size_t) 4, stream) < 4)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }

#else

	int c1, c2, c3, c4;

	c1 = b / 16777216;
	c2 = (b - c1 * 16777216) / 65536;
	c3 = (b - c1 * 16777216 - c2 * 65536) / 256;
	c4 = (b - c1 * 16777216 - c2 * 65536) % 256;

  	if (fputc(c1, stream) == EOF)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }
  	if (fputc(c2, stream) == EOF)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }
  	if (fputc(c3, stream) == EOF)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }
  	if (fputc(c4, stream) == EOF)
    {
      fsm_fprintf(stderr, "Write error. No space on device? Aborting.\n");
      exit(1);
    }
#endif

  *byte_countp += 4;
}

#endif /* SLIM_FST */
