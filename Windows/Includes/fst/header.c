/* $Id: header.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1990-93 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "fst/header.h"
#include "fst/fsm_io.h"
#include "fst/machine.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
#include "fst/pretty-p.h"
#include "fst/config.h"
#endif /* SLIM_FST */


#ifdef MAC_OS
#include "fst/site-dep.h"
#endif
#include "fst/utf8_fatstr_util.h"

/******************************************************
 **
 **  HEADER.C
 **
 **  Lauri Karttunen, RXRC December 1993
 **
 ******************************************************/

extern time_t OLDEST_WRITE_DATE;    /* declared in STRUCT.C */

static STANDARD_HEADER LAST_HEADER =
	{0,0,0,0,0,0,0,0,0,0,0,0, 0, NULL, 0, NULL, 0, 0, 0, NULL};
static STANDARD_HEADER NEXT_HEADER =
	{0,0,0,0,0,0,0,0,0,0,0,0, 0, NULL, 0, NULL, 0, 0, 0, NULL};

static char *gmt_date(struct tm *cur_time_struct)
{
  static char date[256];
  
  if (date == NULL)
    not_enough_memory("GMT_DATE");
  
  strftime(date, 255, "%B %d, %Y %H:%M:%S GMT", cur_time_struct);
  
  return (date);
}


struct tm *get_gmt_time_st()
{
    time_t cur_time;
    int isdst = 0;
    struct tm *time_s;

    cur_time = time((time_t *)0);
/* need this workaround for solaris 2.3 because setting tm_isdst = -1 does not acts as specified in mktime() */
    time_s = localtime(&cur_time); /* get isdst */
    isdst = time_s->tm_isdst;
#ifdef MAC_OS
   /* Sigh. Don't know a way to get the GMT time on the Mac. It should be
         possible to get it from the Map control panel but don't know how
            to get access to it.
            */
    time_s->tm_hour = time_s->tm_hour + LOCAL_TO_GMT_OFFSET ;
    if (isdst > 0)
      time_s->tm_hour++;
#else
    time_s = gmtime(&cur_time);
/*    gmt_time_s->tm_isdst = -1;	 don't make any conversion */
    time_s->tm_isdst = isdst;	/* don't make any conversion */
#endif

    return(time_s);
}

time_t get_gmt_time()
{
    time_t gmt_time;
    struct tm *gmt_time_s;

    gmt_time_s = get_gmt_time_st();
    gmt_time = mktime(gmt_time_s);
    return(gmt_time);
}

/**************************************************************
 *                                                            *
 * support functions for handling application privilege level *
 *                                                            *
 *************************************************************/

static int get_appl_privilege_level()
{
#if defined(WORLD)
    /* Public version */
    return FILE_TYPE_LICENSE_PUBLIC;
#elif defined(LICENSED)
    /* Licensed version */
    return FILE_TYPE_LICENSE_LICENSED;
#elif defined(RESERVED)
    /* Licensed version */
    return FILE_TYPE_LICENSE_RESERVED;
#else
    /* Xerox private version */
    return FILE_TYPE_LICENSE_PRIVATE;
#endif
}

int valid_license_type(int code)
{
  switch(code)
    {
    case FILE_TYPE_LICENSE_PRIVATE:
    case FILE_TYPE_LICENSE_RESERVED:
    case FILE_TYPE_LICENSE_LICENSED:
    case FILE_TYPE_LICENSE_PUBLIC:
    case FILE_TYPE_LICENSE_NONE:
      return(TRUE);
      break;
    default:
      return(FALSE);
      break;
    }
}

char *decode_license_type(int code)
{
  /* Convert license code into a string. Userd in iy-help.c. */

  switch(code)
    {
    case FILE_TYPE_LICENSE_NONE:
      return("NONE");
      break;
    case FILE_TYPE_LICENSE_PUBLIC:
      return("PUBLIC");
      break;
    case FILE_TYPE_LICENSE_LICENSED:
      return("LICENSED");
      break;
    case FILE_TYPE_LICENSE_RESERVED:
      return("RESERVED");
      break;
    case FILE_TYPE_LICENSE_PRIVATE:
      return("XEROX");
      break;
    default:
      return("UNKNOWN");
      break;
    }
}

int encode_license_type(char *str)
{
  /* Convert string to a numberical license code. */

  if (strcmp(str, "PUBLIC") == 0)
    return FILE_TYPE_LICENSE_PUBLIC;
  else if (strcmp(str, "LICENSED") == 0)
    return FILE_TYPE_LICENSE_LICENSED;
  else if (strcmp(str, "XEROX") == 0)
    return FILE_TYPE_LICENSE_PRIVATE;
  else
    return(-1);
}


static int encrypt_license_type(int license_type, time_t num)
{
  int i = num & 255;

  if (i > 128)
    return(i - license_type);
  else
    return(i + license_type);
}

static int decrypt_license_type(int license_type, int num)
{
  int i = num & 255;

  if (i > 128)
    return(i - license_type);
  else
    return(license_type - i);
}


/* Check that the application level has been initialized and does not
   violate the global application privilege */
static int user_license_type = -1;
static int max_user_license_type =  -1;

static void check_user_license_type()
{
    /* Init appl_privilege_level if necessary */
    if (user_license_type == -1)
      {
        user_license_type = get_appl_privilege_level();
	max_user_license_type = user_license_type;
      }

    /* Check that this is not a violation of the global application level */
    if (user_license_type > get_appl_privilege_level())
    {
        handle_warning("Cannot set license type: not enough privilege",
		       "CHECK_LICENSE", user_license_type);
        user_license_type = get_appl_privilege_level();
    }
    /* Check it is valid */
    if (user_license_type > max_user_license_type)
    {
        handle_warning("Invalid license type", "CHECK_LICENSE",
		       user_license_type);
        user_license_type = max_user_license_type;
    }
}

int get_user_license_type()
{
    check_user_license_type();
    return user_license_type;
}

#if !defined(WORLD)
/* Set new application level to generate FST file in a format supported by less privilege FST applications */
void set_user_license_type(int level)
{
    user_license_type = level;
    check_user_license_type();
}
#endif


/*************************************************
 *                                               *
 * support functions for storing start locations *
 *                                               *
 *************************************************/
 
static void ensure_loc_space(STANDARD_HEADERptr header, int net_count)
{
  unsigned long *array = HEADER_start_locations(header);
  int size = HEADER_max_net_count(header);

  if (size == 0)
    array = (unsigned long *)
      malloc((size_t) (net_count * sizeof(unsigned long)));
  else if (size < net_count)
    array = (unsigned long *)
      realloc(array, (size_t) (net_count * sizeof(unsigned long)));
  
  if (!(array))
    not_enough_memory("ensure_loc_space");

  HEADER_start_locations(header) = array;
  HEADER_max_net_count(header) = net_count;
}


/*********************************************
 *                                           *
 * support functions for storing string data *
 *                                           *
 *********************************************/
 
static void adjust_string_buffer(char **buffer, int *size, int new_size)
{
  if (*size == 0)
    {
      if (256 < new_size)
        *size = new_size - 1;
      else
        *size = 255;

      *buffer = (char *) malloc((size_t) (*size + 1) * sizeof(char));
    }
  else
    {
      *buffer = (char *) realloc(*buffer, (size_t) new_size * sizeof(char));
      *size = new_size - 1;
    }
    
  if (!*buffer)
    not_enough_memory("adjust_string_buffer");
}


static void copy_string_data(char **str1, int *max1, char *str2)
{
  if (str2)
    {
      int len2 = strlen(str2);
      
      if (*max1 <= len2)
        adjust_string_buffer(str1, max1, len2+1);
        
      strcpy(*str1, str2);
    }
  else if (*str1)
    **str1 = '\0';
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
/******************************************
 *                                        *
 * support functions for copyright notice *
 *                                        *
 ******************************************/


static char *standard_copyright_notice(struct tm *cur_time_struct, NETptr net)
{
  OBJECTptr copyright_prop;
  static char *copyright = NULL;
  static int len, max_len = 0;
  
  if (COPYRIGHT_ATTR == NULL)
    COPYRIGHT_ATTR = thin_to_fat("COPYRIGHT-NOTICE");
  
  copyright_prop = get_value(net, COPYRIGHT_ATTR);
  
  if (copyright_prop != NULL && OBJ_type(copyright_prop) == String)
    {
      len = fat_strlen(OBJ_str(copyright_prop));
      
      if (max_len <= len)
         adjust_string_buffer(&copyright, &max_len, len + 1);
         
      FST_fat_to_thin(OBJ_str(copyright_prop), copyright);
    }
  else
    {
      char *owner = fsm_copyright(NULL);
	      
      len = strlen(owner) + 22;
      
      /* 22 chars for "Copyright (c) nnnn by " + 1 for termination. */
      
      if (max_len <= len)
        adjust_string_buffer(&copyright, &max_len, len + 1);
      
      strftime (copyright, 200, "Copyright (c) %Y by " , cur_time_struct);
      strncat(copyright, owner, len);
      return (copyright);
    }
  return(copyright);
}
#endif /* SLIM_FST */

STANDARD_HEADERptr make_header(void)
{
  STANDARD_HEADERptr h;
  h = (STANDARD_HEADERptr) calloc((size_t)1, (size_t) sizeof(STANDARD_HEADER));
  
  if (h == NULL)
    not_enough_memory("MAKE_HEADER");
  return(h);
} /* make_header */ 


STANDARD_HEADERptr copy_header(STANDARD_HEADERptr new, STANDARD_HEADERptr old)
{
  int i;

   if (!new)
     new = make_header();
   
   HEADER_file_length(new) = HEADER_file_length(old);
   HEADER_file_type(new) = HEADER_file_type(old);
   HEADER_file_version(new) = HEADER_file_version(old);
   HEADER_copyright_loc(new) = HEADER_copyright_loc(old);
   HEADER_creation_date(new) = HEADER_creation_date(old);
   HEADER_header_length(new) = HEADER_header_length(old);
   HEADER_net_count(new) = HEADER_net_count(old);
   HEADER_license_type(new) = HEADER_license_type(old);
   HEADER_reserved1(new) = HEADER_reserved1(old);
   HEADER_reserved2(new) = HEADER_reserved2(old);
   HEADER_reserved3(new) = HEADER_reserved3(old);
   HEADER_output_p(new) = HEADER_output_p(old);

   copy_string_data(&(*new).ascii_date, &(*new).date_max,
  				    HEADER_ascii_date(old));
   copy_string_data(&HEADER_copyright_notice(new), &HEADER_copyright_max(new),
  					HEADER_copyright_notice(old));
   copy_string_data(&HEADER_file_name(new), &HEADER_file_max(new),
  					HEADER_file_name(old));

   switch(HEADER_file_type(old))
     {
     case OLD_STANDARD_FILE_TYPE:
     case OLD_STANDARD_COMPACTED_FILE_TYPE:
       /* Old file headers do not have net locations. */
       break;
     default:
       ensure_loc_space(new, HEADER_net_count(old));
       
       for (i = 0; i < (int)HEADER_net_count(old); i++)
	 HEADER_net_location(new, i) = HEADER_net_location(old, i);
       break;
     }
  return(new);
}

void set_header_file_name(STANDARD_HEADERptr header, char *name)
{
   if (!header)
     header = &LAST_HEADER;
     
   copy_string_data(&HEADER_file_name(header), &HEADER_file_max(header), name);
}

/* To be called by free_global_variables(). */
void strip_global_headers()
{
    /* Do not try to free the 2 static headers LAST_HEADER
       and NEXT_HEADER, but only the memory pointed by them. */

    /* Strip NEXT_HEADER:  */
    if (HEADER_ascii_date(&NEXT_HEADER) != NULL)
    {       
      free((void *) HEADER_ascii_date(&NEXT_HEADER));
      HEADER_ascii_date(&NEXT_HEADER) = NULL;
    }
    
    if (HEADER_copyright_notice(&NEXT_HEADER) != NULL)
    {
        free((void *) HEADER_copyright_notice(&NEXT_HEADER));
        HEADER_copyright_notice(&NEXT_HEADER)= NULL;
    }
  
    if (HEADER_file_name(&NEXT_HEADER) != NULL)
    {
      free((void *) HEADER_file_name(&NEXT_HEADER));
      HEADER_file_name(&NEXT_HEADER)= NULL;
    }

    if (HEADER_start_locations(&NEXT_HEADER) != NULL)
    {
      free((void *) HEADER_start_locations(&NEXT_HEADER));
      HEADER_start_locations(&NEXT_HEADER)= NULL;
    }

    
    /* Strip LAST_HEADER:  */
    if (HEADER_ascii_date(&LAST_HEADER) != NULL)
    {
        free((void *) HEADER_ascii_date(&LAST_HEADER));
        HEADER_ascii_date(&LAST_HEADER)= NULL;
    }
    if (HEADER_copyright_notice(&LAST_HEADER) != NULL)
    {
        free((void *) HEADER_copyright_notice(&LAST_HEADER));
        HEADER_copyright_notice(&LAST_HEADER)= NULL;
    }
    if (HEADER_file_name(&LAST_HEADER) != NULL)
    {
      free((void *) HEADER_file_name(&LAST_HEADER));
      HEADER_file_name(&LAST_HEADER)= NULL;
    }

    if (HEADER_start_locations(&LAST_HEADER) != NULL)
    {
      free((void *) HEADER_start_locations(&LAST_HEADER));
      HEADER_start_locations(&LAST_HEADER)= NULL;
    }

} /* strip_global_headers */


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
void free_header(STANDARD_HEADERptr header)
{
  if (header == NULL)
    return;
  
  if (header == &NEXT_HEADER || header == &LAST_HEADER)
    {
      handle_warning("Do not try to free a static header.", "FREE_HEADER", 0);
      return;
    }
  
  if (HEADER_ascii_date(header) != NULL)
    free((void *) HEADER_ascii_date(header));
  if (HEADER_copyright_notice(header) != NULL)
    free((void *) HEADER_copyright_notice(header));
  if (HEADER_file_name(header)  != NULL)
    free((void *) HEADER_file_name(header));

  if (HEADER_start_locations(header) != NULL)
    free((void *) HEADER_start_locations(header));

  free((void *) header);
}

#endif /* SLIM_FST */

static int compare_creation_date_p(time_t current_date, time_t file_date)
{
  /* Returns -1 if the file date is greater than the current date.
     In that case, something is definitely wrong. In non-WORLD mode,
     the oldest possible file dates back to March 1, 1990; in WORLD
     mode, files written before June 1, 2000 are not available. The
     setting of OLDEST_WRITE_DATE captures this constraint. If it is met,
     the return value is 1; otherwise 0. */

  if (file_date <= current_date)
    return(file_date >= OLDEST_WRITE_DATE);
  else
    return(-1);
}


void report_dates (time_t other_date, time_t file_date, int order)
{
  /* If ORDER is 0, FILE_DATE falls in the expected range.
     Negative order means that FILE_DATE is too old, positive
     means that it is beyond the current date.
     */
  
  struct tm time = *localtime(&file_date);
  
  fsm_fprintf(stderr, "             File date is: %s\n", gmt_date(&time));
  
  if (order < 0)
    {
      time = *localtime(&other_date);
      fsm_fprintf(stderr, "Earliest possible date is: %s\n", gmt_date(&time));
    }
  else
    {
      time = *localtime(&other_date);
      fsm_fprintf(stderr, "          Current date is: %s\n", gmt_date(&time));
    }
}


static int header_check_license (int license_type)
{
  int license_type_error = ERR_OPERATION_NOT_ALLOWED;
  int max_license_number = get_appl_privilege_level();
  /* Order of license privilege are from 0 (max) to 3 (min) */

  if (!valid_license_type(license_type))
    license_type_error = ERR_INVALID_LICENSE_TYPE;
  else if (license_type <= max_license_number)
    license_type_error = NO_ERROR;
  else
    {
      fsm_fprintf(stderr, "*** The application is not permitted to load \
this file.\n    You need a licensed version of the program.\n");
	
    }
  return license_type_error;
}

int get_file_type (char* file_name, int* file_type)
{
  /* Opens the stream and reads the first 6 bytes from this stream
     to get the file type code:
     -> 1 stands for Kaplan compressed network
     -> 19 stands for older standard file type
     -> 8 stands for really old standard file type
     -> 101 stands for old Karttunen compacted network (?)
     -> 20 stands for Karttunen compacted network (?)
     -> 21 stands for new standard file type;
     -> 22 stands for new Karttunen compacted network.

     Types 21 and 22 are identical to 19 and type 20, respectively,
     except for the encoding of "fat strings", sequences of
     two byte characters. Strings are used to encode the sigma
     alphabet of the network and the attributes of network
     properties. In previous versions of the c-fsm software,
     the strings were written into file in clear text and
     could be seen by examining the file in a text editor
     such as Emacs. In files of type 21 and 22, the fat
     characters making up a string are stored as numbers,
     offset by the last 6 bytes of the file position. To
     see the difference compare write_decrypted_fat_str()
     with the old write_fat_str() in bin-out.c
  */
  int error_num = NO_ERROR;
  FILE* stream= NULL;
  int header_length= 0;
  unsigned long byte_countp= 0;
  *file_type= -1;

  stream = fopen(file_name, "rb");
  if (stream == NULL)
    {
      fsm_fprintf(stderr, "Cannot open '%s'\n", file_name);
      error_num = ERR_FILE_OPENING;
    }
  else
    {
      header_length= next_four_bytes(stream, &byte_countp);
      *file_type = next_byte(stream, &byte_countp);
      fclose(stream);
      switch (*file_type)
        {
	case STANDARD_FILE_TYPE:
	case OLD_STANDARD_FILE_TYPE:
	case OLD_STANDARD_COMPACTED_FILE_TYPE:
	case STANDARD_COMPACTED_FILE_TYPE:
	case KAPLAN_COMPACTED_FILE_TYPE:
	case NEW_STANDARD_FILE_TYPE:
	case NEW_COMPACTED_FILE_TYPE:
	  error_num = NO_ERROR;
	  break;
	default:
	  fsm_fprintf(stderr, "Unknown file type %d\n", *file_type);
	  error_num = ERR_WRONG_FILE_TYPE;
	  break;
        }
    } /* stream != NULL */
  return error_num;
}

STANDARD_HEADERptr read_header (FILE *stream, unsigned long *byte_countp,
                                STANDARD_HEADERptr header, int check_only_p)
{
  /* Reads the first FILE_STAMP_SIZE (currently 20) bytes from stream.
     and stores the values of FILE_LENGTH, TYPE, VERSION, COPYRIGHT_LOC,
     HEADER_LENGTH, and the reserved bytes (currently zero) in HEADER.
     
     If the values are not valid, an error occurs unless CHECK_ONLY_P
     flag is TRUE, in which case the function simply returns NULL.
     If no anomaly is detected, the ASCII file date and copyright notice
     following the fixed length reader are also read and a new file pointer
     containing all the information is returned.
  */
  
  /*  time_t file_date, current_date, raw_file_date; */
  unsigned long file_date, current_date, raw_file_date;
  unsigned long offset = CL_TIME_OFFSET;
  int i, date_order = 0, copyright_len = 0, date_len;
  int license_type;

  *byte_countp = 0;
  
  if (!header)
    header = make_header();
  
  HEADER_file_length(header) = next_four_bytes(stream, byte_countp);
  
  if (HEADER_file_length(header) <= 0)
    {
      if (check_only_p == TRUE)
	return(NULL);
      else
	handle_error("Bad file length", "GET_HEADER",
		     HEADER_file_length(header));
    }
  
  HEADER_file_type(header)     = next_byte(stream, byte_countp);
  HEADER_file_version(header)  = next_byte(stream, byte_countp);
  HEADER_copyright_loc(header) = next_byte(stream, byte_countp);
  
  raw_file_date = next_four_bytes(stream, byte_countp);
  file_date = raw_file_date - offset;
  current_date = get_gmt_time();
  
  date_order = compare_creation_date_p(current_date, file_date);
  
  if (date_order <= 0)
    {
      if (check_only_p == TRUE)
        {
	  /*

          handle_warning("Bad file date", "GET_HEADER", date_order);

	  Eliminating this warning. It causes problems in applications
	  such as lexc that are expecting *not* to find a valid
	  binary file header.  2002-05-31. LK
	  */
          return(NULL);
        }
      else
        {
          if (date_order == 0)
            report_dates(OLDEST_WRITE_DATE, file_date, date_order);
          else
            report_dates(current_date, file_date, date_order);
          handle_error("Bad file date", "GET_HEADER", date_order);
        }
    }
  
  HEADER_creation_date(header) = file_date;
  HEADER_header_length(header) = next_two_bytes(stream, byte_countp);
  
  if (HEADER_header_length(header) > HEADER_file_length(header) ||
      HEADER_copyright_loc(header) > HEADER_header_length(header))
    {
      if (check_only_p == TRUE)
	return(NULL);
      else
	handle_error("Bad header length", "GET_HEADER",
		     HEADER_header_length(header));
    }
  
  HEADER_net_count(header) = next_three_bytes(stream, byte_countp);
  
  license_type = next_byte(stream, byte_countp);

  /* If file type is something other than 21 or 22  and the license type is 0
     we have a public legacy file of networks with no license type
     or some special XLE file (type == 50) which for now is treated
     as public. */

  if (license_type == 0 && 
      (!(HEADER_file_type(header) == 21 || HEADER_file_type(header) == 22)))
    HEADER_license_type(header) = FILE_TYPE_LICENSE_PUBLIC;
  else
    HEADER_license_type(header) =
      decrypt_license_type(license_type, file_date);
			 
  if (header_check_license(HEADER_license_type(header)) != NO_ERROR)
    {
      if (check_only_p)
	return(NULL);
      else
	handle_error("Bad license type", "GET_HEADER",
		     HEADER_license_type(header));
    }
  /* read the unused header space */
  HEADER_reserved1(header) = next_byte(stream, byte_countp);
  HEADER_reserved2(header) = next_byte(stream, byte_countp);
  HEADER_reserved3(header) = next_byte(stream, byte_countp);
  
  if (HEADER_reserved1(header) != 0 ||
      HEADER_reserved2(header) != 0 ||
      HEADER_reserved3(header) != 0)
    {
      if (check_only_p == TRUE)
	return(NULL);
      else
	handle_error("Reserved header bytes not zeros",
		     "GET_HEADER",
                     (HEADER_reserved1(header) << 16) | (HEADER_reserved2(header) << 8) |
                     HEADER_reserved3(header));
    }
  
  if (HEADER_copyright_loc(header) > 0)
    {
      if (HEADER_copyright_loc(header) <= *byte_countp)
	{
	  if (check_only_p == TRUE)
	    return(NULL);
	  else
	    handle_error("Bad copyright location", "GET_HEADER",
			 HEADER_copyright_loc(header));
	}
      else
	copyright_len = HEADER_header_length(header) -
	  HEADER_copyright_loc(header);
    }
  
  date_len = HEADER_header_length(header) - *byte_countp - copyright_len;
  
  if (!check_only_p)
    {
      if (HEADER_date_max(header) <= date_len)
        adjust_string_buffer(&HEADER_ascii_date(header),
			     &HEADER_date_max(header),
			     date_len + 1);
        					 
      if (HEADER_copyright_max(header) <= copyright_len)
        adjust_string_buffer(&HEADER_copyright_notice(header),
			     &HEADER_copyright_max(header),
			     copyright_len + 1);
  
      fread(HEADER_ascii_date(header), (size_t) date_len, (size_t) 1, stream);
      HEADER_ascii_date(header)[date_len] = '\0';
  
      fread(HEADER_copyright_notice(header), (size_t) copyright_len,
	    (size_t) 1, stream);
      HEADER_copyright_notice(header)[copyright_len] = '\0';
  
      *byte_countp += date_len + copyright_len;
  
      if (HEADER_header_length(header) != *byte_countp)
	handle_error("header length mismatch", "READ_HEADER",
		     *byte_countp - HEADER_header_length(header));
       
      HEADER_output_p(header) = FALSE;

      switch (HEADER_file_type(header))
	{
	case OLD_STANDARD_FILE_TYPE:
	case OLD_STANDARD_COMPACTED_FILE_TYPE:
	  /* Old file headers do not contain start locations. */
	  break;
	default:
	  ensure_loc_space(header, HEADER_net_count(header));
	  for (i = 0; i < (int)HEADER_net_count(header); i++)
	    HEADER_net_location(header, i) = 
	      next_four_bytes(stream, byte_countp);
	  break;
	}

      copy_header(&LAST_HEADER, header);
    }
  else if (fseek(stream, (long int) (date_len + copyright_len), SEEK_CUR))
    return(NULL);
  
  return (header);
} /* read_header */

STANDARD_HEADERptr get_header (FILE *stream, unsigned long *byte_countp)
{
  if (read_header(stream, byte_countp, &NEXT_HEADER, FALSE))
    return(&LAST_HEADER);
  else
    return(FALSE);
} /* get_header */


	
STANDARD_HEADERptr check_header (FILE *stream, unsigned long *byte_countp)
{
   /* Tries to verify that the file begins with a valid header.
      Stops as soon as that seems to be so, without reading the
      record creation date and the copyright notice. Does not
      reset the file pointer to the beginning of the file.
      Returns the partially filled pointer on success, NULL
      on failure.
      */
      
  if (read_header(stream, byte_countp, &NEXT_HEADER, TRUE))
    return(&NEXT_HEADER);
  else
    return(NULL);
}	

	
int std_file_header_p(FILE *stream)
{
   /* Tries to verify that the file begins with a valid header.
      Stops as soon as that seems to be so, without reading the
      record creation date and the copyright notice. Resets
      the file pointer to the beginning of the file. Returns
      TRUE if the file appears to begin with a valid file pointer,
      otherwise returns FALSE.
      */
      
  unsigned long byte_count;
  STANDARD_HEADERptr header = check_header(stream, &byte_count);
  
  fseek(stream, 0, 0);
  
  if (header == NULL)
    return(FALSE);
  else
    return(TRUE);

}/* std_file_header */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

STANDARD_HEADERptr write_header(FILE *stream, int type, int version,
                                char *copyright_notice, int net_count,
                                NETptr net, unsigned long *byte_countp,
				int old_format_p)
{
  /* Writes the header bytes and returns a record. */
  
  int len1, len2, copyright_loc, header_length, license_type;
  char *creation_date;
  /* time_t cur_time, file_date; */
  unsigned long cur_time, file_date, offset;
  struct tm *cur_time_struct;
  STANDARD_HEADERptr header = &LAST_HEADER;
  int i = 0;
  
  offset = CL_TIME_OFFSET;
  *byte_countp = 0;
  
  /* If type is not specified, we check the value of old_format_p
     and the net to determine the file type.
     Unless old_format_p is non-zero, type is NEW_STANDARD_FILE_TYPE (21)
     when the net is of standard type. If the net is compacted, record
     type as NEW_COMPACTED_FILE_TYPE (22). If the we are writing in
     the old format, the types are COMPACTED_FILE_TYPE (20) and
     STANDARD_FILE_TYPE (19), respectively.

     */
  
  if (type == 0)
    {
      if (old_format_p)
	{
	  if (net && NET_compacted(net))
	    type = STANDARD_COMPACTED_FILE_TYPE;
	  else
	    type = STANDARD_FILE_TYPE;	
	}
      else
	{
	  if (net && NET_compacted(net))
	    type = NEW_COMPACTED_FILE_TYPE;
	  else
	    type = NEW_STANDARD_FILE_TYPE;	
	}
    }

  /*
  if (type == 0)
    {                           Replaced by the code above.
      if (NET_compacted(net))
        type = STANDARD_COMPACTED_FILE_TYPE;
      else
        type = STANDARD_FILE_TYPE;
    }
  */
  cur_time_struct = get_gmt_time_st();
  cur_time = mktime(cur_time_struct);

  file_date = cur_time + offset;
  
  creation_date = gmt_date(cur_time_struct);
  
  len1 = strlen(creation_date);
  len1 += len1 % 2;
  
  copyright_loc = FILE_STAMP_LENGTH + len1;
  
  if (copyright_loc > 255)
    handle_error("Bad copyright location", "WRITE_HEADER", copyright_loc);
  
  if (copyright_notice == NULL)
    copyright_notice = standard_copyright_notice(cur_time_struct, net);
  
  len2 = strlen(copyright_notice);
  len2 += len2 % 2;
  
  header_length = copyright_loc + len2;

  /* Record the file length: file length will go here later */
  write_four_bytes((unsigned long) 0, stream, byte_countp);

  /* Record the file type:   */
  write_byte((fst_byte) type, stream, byte_countp);
  
  /* Record the file version: */
  write_byte((fst_byte) version, stream, byte_countp);

  /* Record the location of copyright notice */
  write_byte((fst_byte) copyright_loc, stream, byte_countp);

  /* Record the timestamp: */
  write_four_bytes(file_date, stream, byte_countp);
  
  /* Record the header length: */
  write_two_bytes((int) header_length, stream, byte_countp);

  /* Record the location of the net count: the real net count
     will go here later */
/*   HEADER_net_count(header) = *byte_countp; */
  write_three_bytes((unsigned long) net_count, stream, byte_countp);

  /* Record the license_type unless we are writing in the old format. */

  if (old_format_p)
    write_byte((unsigned long) 0, stream, byte_countp);
  else
    {
      license_type = get_user_license_type();
      write_byte((unsigned char)encrypt_license_type(license_type, cur_time),
		 stream, byte_countp);
    }
  /* Record the unused header space: */
  write_byte((unsigned long) 0, stream, byte_countp);
  write_byte((unsigned long) 0, stream, byte_countp);
  write_byte((unsigned long) 0, stream, byte_countp);

  /* Record the creation date: */
  fwrite (creation_date, (size_t) sizeof(char),
          (size_t) len1, stream);
  *byte_countp += len1;

  /* Record the copyright notice: */
  fwrite (copyright_notice, (size_t) sizeof(char), len2, stream);
  *byte_countp += len2;
  
  /* Provisory length. Reset after recording the start locations. */
  HEADER_header_length(header) = *byte_countp;

  /* Record the start locations of networks: the real values 
     will go here later */
  for (i = 0; i < net_count; i++)
    write_four_bytes((unsigned long) 0, stream, byte_countp);

  /* Make sure there is enough space to record the start locations. */
  ensure_loc_space(header, net_count);

  HEADER_file_type(header) = type;
  HEADER_file_version(header) = version;
  HEADER_copyright_loc(header) = copyright_loc;
  HEADER_creation_date(header) = cur_time;

  HEADER_net_count(header) = net_count;
  HEADER_license_type(header) = license_type;
  
  copy_string_data(&HEADER_ascii_date(header), &HEADER_date_max(header),
  				creation_date);			 
  
  copy_string_data(&HEADER_copyright_notice(header),
		   &HEADER_copyright_max(header), copyright_notice);	 					 
  HEADER_output_p(header) = TRUE;
  return(header);
}

STANDARD_HEADERptr get_last_header(void)
{
  return(&LAST_HEADER);
}

/***********************************
 *
 *  Printing file headers
 *
 ************************************/

#ifndef WORLD
static void print_start_locations(STANDARD_HEADERptr header,
				  int rm, FILE *stream)
{
  unsigned long *array = HEADER_start_locations(header);
  int net_count = HEADER_net_count(header);
  char *name = "Start locations:  ";
  int len = strlen(name);
  int i;

  string_out(name);

  int_out(array[0]);

  for (i = 1; i< net_count; i++)
    {
      char_out(',');
      buffer_out_s(rm, len, stream);
      int_out(array[i]);
    }
}
#endif /* !WORLD */


void print_std_file_header_s(STANDARD_HEADERptr header, int rm,
                             int indent, FILE *stream)
{
  if (header == NULL)
    fsm_fprintf(stderr, "No header.  No files have been read or written.\n");
  else
    {
      buffer_out_s(rm,indent,stream);
      string_out("File name:        ");
      string_out(HEADER_file_name(header));
      fresh_line_s(rm, indent,stream);
      string_out("Write date:       ");
      string_out(HEADER_ascii_date(header));
      fresh_line_s(rm, indent,stream);
      string_out("Copyright notice: ");
      string_out(HEADER_copyright_notice(header));
      fresh_line_s(rm, indent,stream);
      string_out("Net count:        ");
      int_out(HEADER_net_count(header));
      if (HEADER_net_count(header) == 1)
        string_out(" net");
      else
        string_out(" nets");
      fresh_line_s(rm, indent,stream);
      string_out("File type:        ");
      switch(HEADER_file_type(header))
	{
	case KAPLAN_COMPACTED_FILE_TYPE:
	  string_out("Compressed Index");
	  break;
	case OLD_STANDARD_FILE_TYPE:
	  string_out("Very Old Standard");
	  break;
	case OLD_STANDARD_COMPACTED_FILE_TYPE:
	  string_out("Very Old Compacted");
	  break;
	case STANDARD_FILE_TYPE:
	  string_out("Old Standard");
	  break;
	case STANDARD_COMPACTED_FILE_TYPE:
	  string_out("Old Compacted");
	  break;
	case NEW_STANDARD_FILE_TYPE:
	  string_out("Standard");
	  break;
	case NEW_COMPACTED_FILE_TYPE:
	  string_out("Compacted");
	  break;
	default:
	  int_out(HEADER_file_type(header));
	}
      fresh_line_s(rm, indent,stream);      
      string_out("File version:     ");
      int_out(HEADER_file_version(header));
      fresh_line_s(rm, indent,stream);
      string_out("File length:      ");
      int_out(HEADER_file_length(header));
      string_out(" bytes");
      buffer_out_s(rm, indent, stream);
      fresh_line_s(rm, indent, stream);
      /* Not printing start locations for WORLD. */
      /* Not printing license type for WORLD. */
#ifndef WORLD
      switch(HEADER_file_type(header))
	{
	case OLD_STANDARD_FILE_TYPE:
	case OLD_STANDARD_COMPACTED_FILE_TYPE:
	  /* Old file headers do not have start locations */
	  break;
	default:
	  print_start_locations(header, rm, stream);
	  fresh_line_s(rm, indent, stream);
	  break;
	}
      string_out("License type:     ");
      string_out(decode_license_type(HEADER_license_type(header)));
      buffer_out_s(rm, indent, stream);
      fresh_line_s(rm, indent, stream);
#endif /* WORLD */
    }
}


void print_file_info (FILE *stream)
{
  STANDARD_HEADERptr header = get_last_header();
  
  if (HEADER_net_count(header) == 0)
    fsm_fprintf(stream, "No files have been read or saved yet.\n");
  else
    {
      switch (HEADER_output_p(header))
        {
        case 0:
          fsm_fprintf(stream, "**** Last input file ****\n");
          break;
        case 1:
          fsm_fprintf(stream, "**** Last output file ****\n");
          break;
        }
      print_std_file_header_s(header, 78, 0, stream);
    }
}

#endif /* SLIM_FST */
