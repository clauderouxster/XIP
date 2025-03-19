/* $Id: fat-str.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/**************************************************************************
 **  
 **                                     fat-str.c
 **                  Todd Yampol, Xerox PARC: January, 1989
 **  
 **     Implementation for fat (16 bit) characters & strings.
 **   
 **************************************************************************/

#ifdef __GNUC__
static char rcsid[] __attribute__ ((unused)) = "$Id: fat-str.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $";
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "fst/fat-str.h"
#include "fst/error.h"
#include "fst/fsm_io.h"

/* Temporary fat_string buffer to be used for conversion by function
   thin_to_temp_fat(): its size is adjusted as needed.
   (!! must be released by calling free_fat_string_buffer() in function
   release_fsm_memory()). */
static FAT_STR TEMPORARY_FATSTR = NULL;
static size_t TEMPORARY_FATSTR_len = 0;

int fat_charcmp(FAT_CHAR s1, FAT_CHAR s2)
{
/*****
 *
 * Returns 0 is s1 is identical to s2.
 * Returns 1 if s1 is greater.
 * Returns -1 if s2 is greater.
 *
 *****/
  if (CHARSET(&s1) == CHARSET(&s2))
    {
      if (CHARNUM(&s1) == CHARNUM(&s2))
        return(0);
      else if (CHARNUM(&s1) > CHARNUM(&s2))
        return(1);
      else
        return(-1);
    }
  else if (CHARSET(&s1) > CHARSET(&s2))
    return(1);
  else
   return(-1);
}

int fat_strcmp(void *s1, void *s2)
/*****
 *
 * Returns 0 is s1 is identical to s2.
 * Returns 1 if s1 is greater.
 * Returns -1 if s2 is greater.
 *
 *****/
{
  FAT_STR fs1 = (FAT_STR) s1, fs2 = (FAT_STR) s2;
  
  if (fs1 == fs2)
    return(0);
  
  while (!(CHARNUM(fs1) == '\0' && CHARSET(fs1) == '\0'))
    {
      if (!(CHARNUM(fs1) == CHARNUM(fs2) && CHARSET(fs1) == CHARSET(fs2)))
        {
          if (CHARNUM(fs2) == '\0' && CHARSET(fs2) == '\0')
            return(1);        /* s2 is a proper initial substring of s1 */
          else if (CHARSET(fs1) == CHARSET(fs2))
            {
              if (CHARNUM(fs1) > CHARNUM(fs2))
                return(1);  /* s1 is greater. */
              else
                return(-1); /* s2 is greater. */
            }
          else if (CHARSET(fs1) > CHARSET(fs2))
            return(1);        /* s1 is greater. */
          else
            return(-1);       /* s2 is greater. */
        }      

      fs1++;
      fs2++;
    }
  if (CHARNUM(fs2) == '\0' && CHARSET(fs2) == '\0')
    return(0);			/* strings are identical */
  else
    return(-1);			/* s1 is a proper initial substring of s2 */
}


/* version of fat_str comparison for labels, where alphabetic strings
 * are always lower than non-alphabetic strings
 */
int fat_strcmp_lab(FAT_STR s1, FAT_STR s2)

/* alphabetic always lower than non-alphabetic */
/*****
 *
 * Returns 0 is s1 is identical to s2.
 * Returns a positive value if s1 is greater.
 * Returns a negative value if s2 is greater.
 *
 *****/
{
  /* while not at the end of s1 */
  while (!(CHARNUM(s1) == '\0' && CHARSET(s1) == '\0'))
    {
      /* if the two fat chars are not equal */
      if (!(CHARNUM(s1) == CHARNUM(s2) && CHARSET(s1) == CHARSET(s2)))
        {
	  if (CHARNUM(s2) == '\0' && CHARSET(s2) == '\0')
	    return(1);		/* s2 is a proper initial substring of s1 */
	  else			/* a genuine mismatch */
            {
	      if (CHARSET(s1) == '\0' && CHARSET(s2) == '\0')
                {
		  /* common ascii */
		  if (isalpha(CHARNUM(s1)))
                    {
		      if (isalpha(CHARNUM(s2)))
			return CHARNUM(s1) - CHARNUM(s2);
		      else
			return -1; /* alphabetic before non-alph */
                    }
		  else if (isalpha(CHARNUM(s2)))
		    return 1;	/* alphabetic before non-alph */
		  else		/* both are non-alph */
		    return CHARNUM(s1) - CHARNUM(s2);
                }
	      else		/* different character sets */
                return(fat_charcmp(*s1, *s2));
            }
        }
      /* else the fat chars are equal so far */
      s1++;
      s2++;
    }
  /* reach here when s1 is at the end */
  /* if s2 is also at the end */
  if (CHARNUM(s2) == '\0' && CHARSET(s2) == '\0')
    return(0);			/* strings are identical */
  else				/* s1 is done, but s2 isn't */
    return(-1);			/* s1 is a proper initial substring of s2 */
}


FAT_STR fat_strcpy(FAT_STR fs1, FAT_STR fs2)
{
  FAT_STR fs = fs1;
  
  while (CHARNUM(fs2) || CHARSET(fs2))
    {
      CHARNUM(fs) = CHARNUM(fs2);
      CHARSET(fs++) = CHARSET(fs2++);
    }
  
  CHARSET(fs) = CHARNUM(fs) = '\0';
  
  return (fs1);
}


int fat_strlen(FAT_STR fs)
{
  int i;
  
  for (i = 0; CHARNUM(fs) || CHARSET(fs); fs++, i++);
  
  return (i);
}

FAT_STR copy_fat_string(FAT_STR fs)
{
  /* The previous version called fat_strcpy() directly with the malloc
     statement as the first argument. This works fine on other systems
     but not in Symantec C for the Power Mac when optimization is
     turned on. */

  FAT_STR fs_copy = (FAT_STR) malloc((size_t) (fat_strlen(fs) + 1) *
				     (size_t) sizeof(FAT_CHAR)) ;

  return(fat_strcpy(fs_copy, fs));
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

FAT_STR fat_strcat(FAT_STR fs1, FAT_STR fs2)
{
  fat_strcpy(fs1 + fat_strlen(fs1), fs2);
  
  return (fs1);
}

#endif /* SLIM_FST */

FAT_STR thin_to_fat (const char *t)
{
  FAT_STR f, fs;
  
  fs = (FAT_STR) malloc((size_t) (strlen(t) + 1) * (size_t) sizeof(FAT_CHAR));
  
  if (fs == NULL)
    not_enough_memory("THIN_TO_FAT");
  
  f = fs;
  while (*t != '\0')
    {
      CHARNUM(f) = *t++;
      CHARSET(f++) = '\0';
    }
  
  CHARSET(f) = '\0';
  CHARNUM(f) = '\0';
  
  return (fs);
}

/* Copies string T into the static fat string TEMPORARY_FATSTR
   whose length is adjusted as needed. Use carefully because the
   previous value is rewritten by the next call. */
FAT_STR thin_to_temp_fat (char *t)
{

  FAT_STR f;
  unsigned int len= strlen(t);
  
  if ((TEMPORARY_FATSTR == NULL)
      || (len >= TEMPORARY_FATSTR_len))
  {
      unsigned int i; /* YM, 2/23/96 unsigned added */
      
      TEMPORARY_FATSTR_len = len + 1;
     
      if (TEMPORARY_FATSTR == NULL)
          TEMPORARY_FATSTR= (FAT_STR) malloc((size_t) TEMPORARY_FATSTR_len * sizeof(FAT_CHAR));
      else
          TEMPORARY_FATSTR= (FAT_STR) realloc((void *)TEMPORARY_FATSTR,
				(size_t) TEMPORARY_FATSTR_len * sizeof(FAT_CHAR));
     
      if (TEMPORARY_FATSTR == NULL)
          not_enough_memory("THIN_TO_TEMP_FAT");
      
      f = TEMPORARY_FATSTR;
      for (i = 0; i < TEMPORARY_FATSTR_len; i++)
          CHARSET(f++) = '\0';
  }
  
  f = TEMPORARY_FATSTR;
  
  while (*t != '\0')
      CHARNUM(f++) = *t++;
  
  CHARNUM(f) = '\0';
  CHARSET(f) = '\0';
  
  return (TEMPORARY_FATSTR);
}

/* Release the global variable buffer TEMPORARY_FATSTR.
   (!! must be called by function release_fsm_memory()). */
void free_fat_string_memory()
{
    if (TEMPORARY_FATSTR != NULL)
    {
        free(TEMPORARY_FATSTR);
        TEMPORARY_FATSTR= NULL;
    }
}


char *fat_to_thin (FAT_STR fs, char *s)
{
  /* Copies fat string FS to thin string S.  If S is NULL, a
     new string is allocated, otherwise characters are copied
     to S.  It is up to the caller to verify that S has
     enough space.
     */
  
  char *t, *str;
  
  if (s == NULL)
    t = (char *) malloc((size_t) (fat_strlen(fs) + 1) *  sizeof(char));
  else
    t = s;
  
  if (t == NULL)
    not_enough_memory("FAT_TO_THIN");
  
  str = t;
  while (CHARNUM(fs) != '\0' || CHARSET(fs) != '\0')
    *str++ = CHARNUM(fs++);
  
  *str = '\0';
  
  return (t);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */


void hex_char_to_buf(unsigned char b, char * str)
{ 
  unsigned char n, j = b;
  short k;
  for (k = 0; k < 8; k = k + 4)
    {
      n = ((unsigned char) (j << k)) >> 4;   /* cast needed!  09-06-26 LK */
      switch(n)
	{
	case 15:  *str++ = ('F');
	  break;
	case 14:  *str++ = ('E');
	  break;
	case 13:  *str++ = ('D');
	  break;
	case 12:  *str++ = ('C');
	  break;
	case 11:  *str++ = ('B');
	  break;
	case 10:  *str++ = ('A');
	  break;
	case 9:  *str++ = ('9');
	  break;
	case 8:  *str++ = ('8');
	  break;
	case 7:  *str++ = ('7');
	  break;
	case 6:  *str++ = ('6');
	  break;
	case 5:  *str++ = ('5');
	  break;
	case 4:  *str++ = ('4');
	  break;
	case 3:  *str++ = ('3');
	  break;
	case 2:  *str++ = ('2');
	  break;
	case 1:  *str++ = ('1');
	  break;
	case 0:
	   *str++ = ('0');
	  break;
	}
    }
}

char *fat_to_thin_esc (FAT_STR fs, char *s, char *esc_codes,
		       int hexa_or_unicode_always)
{
  /* Copies fat string FS to thin string S.  If S is NULL, a
     new string is allocated, otherwise characters are copied
     to S.  It is up to the caller to verify that S has
     enough space.
     This is aimed for printing: so visible characters are printed as
     they should appear but not visible ones - like 0x82 or unicode
     ones (like 1234) - will be printed as \x82 and \u1234.
     Must be improved when there are standard methods for Unicode rendering.
     */
  
  char *t, *str;
  
  if (s == NULL)
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
	  if (! hexa_or_unicode_always)
	    {
	      if (esc_codes != NULL)
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
		    case ' ':
		      *str++ = '\"'; *str++ = ' '; *str++ = '\"'; fs++;
		      break;
                    case '"':
                      *str++ = '%'; *str++ = '"'; fs++;
                      break;
		    default:
		      if (is_character_printable(CHARNUM(fs)))
			*str++ = CHARNUM(fs++);
		      else
			{
			  *str++ = '\\';
			  *str++ = 'x';
			  hex_char_to_buf(CHARNUM(fs++), str++); str++;
			}
		    }
		}
	      else
		{
		  *str++ = '\"';
		  *str++ = 'x';
		  hex_char_to_buf(CHARNUM(fs++), str++); str++;
		}
	    }
	}
      else
	{
	  *str++ = '\\';
	  *str++ = 'u';
	  hex_char_to_buf(CHARSET(fs), str++); str++;
	  hex_char_to_buf(CHARNUM(fs++), str++); str++;
	}
    }

  *str = '\0';
  
  return (t);
}

FAT_STR charcode_to_fstring(int i)
{
  /* Store the low 8 bits as CHARNUM, next 8 bits as CHARSET, ignore the rest.
   */
  FAT_STR str;
  
  str = (FAT_STR) calloc((size_t) 2, (size_t) sizeof(FAT_CHAR));
  if (str == NULL)
    not_enough_memory("CHARCODE_TO_FSTRING");
  
  CHARSET(str) = (i >> 8) & 255;
  CHARNUM(str) = i & 255;
  return(str);
}

#endif /* SLIM_FST */

FAT_STR integer_to_fstring(int i)
{
  /* Convert the integer to a fat string   */
  FAT_STR str;
  int len = (i == 0 ? 2 : (int)(log10(i) + 2));
  int j,k;
  unsigned char c;
  
  str = (FAT_STR) calloc((size_t) len, (size_t) sizeof(FAT_CHAR));
  if (str == NULL)
    not_enough_memory("INTEGER_TO_FSTRING");
  
  j = 0;
  
  do {
    str[j][1] = (i % 10 + '0');
    j++;
  } while ((i /= 10) > 0);
  
  str[j][1] = '\0';

  for (k = 0, j--; k < j; k++, j--)
    {
      c = str[k][1];
      str[k][1] = str[j][1];
      str[j][1] = c;
    }


  return(str);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int fstring_to_charcode(FAT_STR fs)
{
  return(CHARSET(fs) << 8 | CHARNUM(fs));
}

#endif /* SLIM_FST */

FAT_STR fchar_to_fstring(FAT_CHAR *fc)
{
  FAT_STR str;
  
  str = (FAT_STR) calloc((size_t) 2, (size_t) sizeof(FAT_CHAR));
  if (str == NULL)
    not_enough_memory("NETWORK_FROM_FILE");
  
  CHARSET(str) = CHARSET(fc);
  CHARNUM(str) = CHARNUM(fc);
  return(str);
}



int fprint_fat_char (FILE *outfile, FAT_STR fc)
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


static void fprint_char_as_hex(FILE *stream, unsigned char b)
{
  unsigned char n, j = b;
  short k;

  for (k = 0; k < 8; k = k + 4)
    {
      n = ((unsigned char) (j << k)) >> 4;
      switch(n)
	{
	case 15: fsm_putc('F', stream);
	  break;
	case 14: fsm_putc('E', stream);
	  break;
	case 13: fsm_putc('D', stream);
	  break;
	case 12: fsm_putc('C', stream);
	  break;
	case 11: fsm_putc('B', stream);
	  break;
	case 10: fsm_putc('A', stream);
	  break;
	case 9: fsm_putc('9', stream);
	  break;
	case 8: fsm_putc('8', stream);
	  break;
	case 7: fsm_putc('7', stream);
	  break;
	case 6: fsm_putc('6', stream);
	  break;
	case 5: fsm_putc('5', stream);
	  break;
	case 4: fsm_putc('4', stream);
	  break;
	case 3: fsm_putc('3', stream);
	  break;
	case 2: fsm_putc('2', stream);
	  break;
	case 1: fsm_putc('1', stream);
	  break;
	case 0:
	  fsm_putc('0', stream);
	  break;
	default:
	  handle_error("Illegal hex value", "FPRINTF_CHAR_AS_HEX", n);
	}
    }
}

/*#ifdef __GNUC__
inline
#endif */
int is_character_printable(unsigned char c)
     /* This should work for all platforms, fonts and terminal types.
	C control characters : \b \t \n \v \f \r (0x08-0x0d) are considered as
	"printables" since they effect the output. So does ' ' (the space).
	Hack: Now only works for iso-latin-1 on an xterm. 99-1-19 TG 
	*/
{
  return( ( 0x7 < c && c < 0x0d ) ||
	  (c == ' ') ||
	  (isgraph_iso_latin_1(c) &&  (c < 0x80 || c > 0xa0)));
}



static int fprint_fat_char_new(FILE *outfile, FAT_STR fc)
{
  /* Prints \uXXXX for characters that are not in Latin 1
     Prints \xYY  for characters that are in the 00FF range but not printable as
     Latin-1 (like Microsoft comma - 130 (decimal)=\202 (octal). TG
     */

  if (CHARSET(fc) == '\0')
    {
      if (is_character_printable(CHARNUM(fc)))
	fsm_fprintf (outfile, "%c", CHARNUM(fc));
      else
	{
	  fsm_fprintf(outfile, "\\x");
	  fprint_char_as_hex(outfile, CHARNUM(fc));
	}
      return(1);
    }
  else
    {
      fsm_fprintf(outfile, "\\u");
      fprint_char_as_hex(outfile, CHARSET(fc));
      fprint_char_as_hex(outfile, CHARNUM(fc));
      return(6);
    }
}


/* Escaped versions: for any char in esc_code, it prints  become %char,
   except that tab and newline are printed as "\t" and "\n",
   respectively, space as " ";  carriage return as "\r",
	   vertical tabulate as "\v", backspace as "\b", form feed as"\f".
   */

int fprint_fat_char_esc (FILE *outfile, FAT_STR fc , char *esc_codes)
{
  int len = 0;

  if ((esc_codes != NULL) && (CHARSET(fc) == '\0'))
    {
      if (((CHARNUM(fc)) == '%') ||
	  (strchr(esc_codes, (CHARNUM(fc))) != NULL))
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
      else
	len += fprint_fat_char_new(outfile, fc);
    }
  else
    len += fprint_fat_char_new(outfile, fc);
  return(len);
} /* fprint_fat_char_esc */


int fprint_fat_string (FILE *outfile, FAT_STR fs)
{
  int len = 0;
  
  while (!(CHARSET(fs) == '\0' && CHARNUM(fs) == '\0'))
    len += fprint_fat_char(outfile, fs++);
  
  return(len);
}

int fprint_fat_string_esc (FILE *outfile, FAT_STR fs, char *esc_codes)
{
  int len = 0;
  
  while (!(CHARSET(fs) == '\0' && CHARNUM(fs) == '\0'))
    len += fprint_fat_char_esc(outfile, fs++, esc_codes);

  return(len);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int fprint_fat_char_with_esc(FILE *outfile, FAT_STR fc)
{
  int len;

  if (CHARSET(fc) == '\0')
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
      len = fprint_fat_char_new(outfile, fc);

  return(len);
}

int fprint_fat_string_with_esc (FILE *outfile, FAT_STR fs)
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
    len += fprint_fat_char_with_esc(outfile, fs++);

  return(len);
}


void hex_to_buf(long i, STRING_BUFFERptr buf)
{
  unsigned long j = (unsigned long) i;
  int print_zero_p = 0;
  short k, n;
  for (k = 0; k < 32; k = k + 4)
    {
      n = (short)((j << k) >> 28); /* YM, 2/23/96, (short) cast added */
      switch(n)
	{
	case 15: append_char_to_buffer('F', buf);
	  break;
	case 14: append_char_to_buffer('E', buf);
	  break;
	case 13: append_char_to_buffer('D', buf);
	  break;
	case 12: append_char_to_buffer('C', buf);
	  break;
	case 11: append_char_to_buffer('B', buf);
	  break;
	case 10: append_char_to_buffer('A', buf);
	  break;
	case 9: append_char_to_buffer('9', buf);
	  break;
	case 8: append_char_to_buffer('8', buf);
	  break;
	case 7: append_char_to_buffer('7', buf);
	  break;
	case 6: append_char_to_buffer('6', buf);
	  break;
	case 5: append_char_to_buffer('5', buf);
	  break;
	case 4: append_char_to_buffer('4', buf);
	  break;
	case 3: append_char_to_buffer('3', buf);
	  break;
	case 2: append_char_to_buffer('2', buf);
	  break;
	case 1: append_char_to_buffer('1', buf);
	  break;
	case 0: if (print_zero_p || k == 24)
	  append_char_to_buffer('0', buf);
	}
      if (n != 0)
	print_zero_p = 1;
    }
}


void fat_char_to_buf (FAT_STR fc, STRING_BUFFERptr buf)
{
  if (CHARSET(fc) == 0)
    {
      append_char_to_buffer((char) CHARNUM(fc), buf);
    }
  else
    {
      append_char_to_buffer('\\', buf); 
      append_char_to_buffer('u', buf);
      hex_to_buf(CHARSET(fc), buf);
      hex_to_buf(CHARNUM(fc), buf);
    }
}

static void fat_char_to_buf_print_esc (FAT_STR fc, STRING_BUFFERptr buf)
     /* Aimed for printing out visible characters */
{
  if (CHARSET(fc) == 0)
    {
       if (is_character_printable(CHARNUM(fc)))
	 append_char_to_buffer((char) CHARNUM(fc), buf);
       else
	 {
	   append_char_to_buffer('\\', buf);
	   append_char_to_buffer('x', buf);
	   hex_to_buf(CHARNUM(fc), buf);
	 }
    }
  else
    {
      append_char_to_buffer('\\', buf); 
      append_char_to_buffer('u', buf);
      hex_to_buf(CHARSET(fc), buf);
      hex_to_buf(CHARNUM(fc), buf);
    }
}

void fat_char_to_buf_esc (FAT_STR fc , char *esc_codes, STRING_BUFFERptr buf)
{
  if ((esc_codes != NULL) && (CHARSET(fc) == '\0'))
    {
      if (((CHARNUM(fc)) == '%') ||
	  (strchr(esc_codes, (CHARNUM(fc))) != NULL))
	switch (CHARNUM(fc))
	  {
	  case '\t':
	    append_string_to_buffer("\"\\t\"", buf);
	    break;
	  case '\n':
	    append_string_to_buffer("\"\\n\"", buf);
	    break;
	  case ' ':
	    append_string_to_buffer("\" \"", buf);
	    break;
	  case '\r':
	    append_string_to_buffer("\"\\r\"", buf);
	    break;
	  case '\v':
	    append_string_to_buffer("\"\\v\"", buf);
	    break;
	  case '\b':
	    append_string_to_buffer("\"\\b\"", buf);
	    break;
	  case '\f':
	    append_string_to_buffer("\"\\f\"", buf);
	    break;
          case '"':
            append_string_to_buffer("%\"", buf);
            break;
	  default:
	    append_char_to_buffer('"', buf);
	    fat_char_to_buf(fc, buf);
	    append_char_to_buffer('"', buf);
	  }
      else
	fat_char_to_buf_print_esc(fc, buf);
    }
  else
    fat_char_to_buf_print_esc(fc, buf);
}

void fat_string_to_buf (FAT_STR fs, STRING_BUFFERptr buf)
{ 
  for (; !(CHARSET(fs) == 0 && CHARNUM(fs) == 0);)
    fat_char_to_buf(fs++, buf);
}

void fat_string_to_buf_esc (FAT_STR fs, char *esc, STRING_BUFFERptr buf)
{ 
  for (; !(CHARSET(fs) == 0 && CHARNUM(fs) == 0);)
    fat_char_to_buf_esc(fs++, esc, buf);
}

#endif /* SLIM_FST */

