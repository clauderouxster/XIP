/***************************************************************************
 *            utrac.h
 *
 *  Tue Oct  5 11:28:44 2004
 *  Copyright  2004  Alliance MCA
 *  Written by : Antoine Calando (antoine@alliancemca.net)
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
/*!
 * \file utrac.h
 * \author Antoine Calando (antoine@alliancemca.net)
 */

#ifndef _UTRAC_H_
#define _UTRAC_H_

#ifndef __cplusplus
typedef unsigned short int bool;
#define true 1
#define false 0
#else
extern "C" {
#endif

//#include "debug.h"
#include <sys/types.h>
#include "ut_error.h"
#include "ut_text.h"
#include "ut_charset.h"

#define UT_VERSION		"0.3.0"

#define UT_EOL_CHAR 	0x0		//!< Character code for end of line.
#define UT_EOL_ALT_CHAR 0xD		//!< Character code for end of line 2 (see UtEolType).
#define UT_SKIP_CHAR	0x1		//!< Character code for character to skip during conversion.
#define UT_EOF_CHAR 	0x0		//!< Character code for end of file.
#define UT_UNICODE_NONCHAR 0xFFFF	//!< Illegal character, also used to indicate "no character".
//#define BUFFER_OFFSET 4

#define UT_UNSET -1	//!< Unset variable (often used for indexes).
//#define UT_NO_CHANGE -2U

#define UT_THRESHOLD_CONTROL_CHAR 0.05	//!< Maximum percentage of illegal control chars accepted in a file.
#define UT_THRESHOLD_UTF8 0.01			//!< Maximum percentage of utf-8 errors in an UTF-8 file.

#define UT_LOAD_STEP 1*1024*1024	//!< Step in bytes between two calls of the "progress bar" function during loading.
#define UT_PROCESS_STEP 1*1024*1024 //!< Step in bytes between two calls of the "progress bar" function during processing.


#define UT_COEF_MAX 5	//!< Maximum number of coefficients for languages and systems.

#ifdef _UT_CHARSET_C_
const float UT_LANG_SYS_COEF [UT_COEF_MAX] = { 1.0, 1.02, 1.04, 1.06, 1.10 };
//const char * UT_CHARMAPS_FILENAME2 = "/home/antoine/dev/utrac/charsets.dat";
//UT_CHARMAPS_FILENAME should be set with "gcc -D ..."
const char * UT_CHARMAPS_FILENAME2 = "charsets.dat";
const char * UT_DEFAULT_ENCODING_UNIX = "ISO-8859-1";
#else
//! Language and system coeficients applied to charset rating, depending on language or system selected.
extern const float UT_LANG_SYS_COEF [];
//! Path 1 to file containing charset informations.
extern const char * UT_CHARMAPS_FILENAME;
//! Path 2 to file containing charset informations.
extern const char * UT_CHARMAPS_FILENAME2;
//! Default encoding on Unix systems.
extern const char * UT_DEFAULT_ENCODING_UNIX;
#endif

#define UT_LANG_SYS_ALLOC_STEP 	8		//!< Initial size for dynamic array UtLangSys.
#define UT_ERROR_STRING_SIZE	128		//!< Size of UtSession::error_string.
#define UT_STDIN_BUFFER_SIZE	65536	//!< Initial size for dynamic buffer in ut_load_text_stdin().


/***************************************************************************/
/*!
 * \brief Structure containing different languages or systems used
 *
 * This structure is a dynamic array containing the list of languages
 * or systems defined in the charset data file
 */
typedef struct UtLangSys {
	char ** name;		//!< Array of names of language or system.
	char * code;		//!< Array of codes on two uppercase characters.
	ushort n;			//!< Number of languages or systems listed in UtLangSys::name and UtLangSys::code.
	ushort n_max;		//!< Number of languages or systems that UtLangSys::name or UtLangSys::code can contains.
} UtLangSys ;


/***************************************************************************/
/*!
 * \brief Structure containing all required information for Utrac.
 *
 * This structure contains all the required information
 * for an utrac session (charsets data, language, system
 * and charset default...). Its unique instance can be accessed with the
 * ut_session pointer which is defined as a global variable.
 * It is created with ut_init() and destroyed with ut_finish().
 * \sa ut_session.
 */

typedef struct UtSession {
	struct UtCharset * charset;		//!< Charset array loaded from charset data file.
	int nb_charsets;					//!< Number of charsets in UtSession::charset.

	UtLangSys language;				//!< List of languages used.
	UtLangSys system;				//!< List of languages used.

	int language_default;			//!< Index of default language (relative to UtSession:: #language).
	int system_default;				//!< Index of default system (relative to UtSession::system).
	UtEolType eol_default;			//!< Default type of end of line.
	UtEolType eol_alt_default;		//!< Default type 2 of end of line.
	UtCharsetIndex charset_default;	//!< Default charset of the system.

	ulong nomapping_char;			//!< Character used if a character conversion error occurs. No character inserted if zero. Set by user.
	
	int (*progress_function)
	(UtText*,float);				//!< User function called during processing to update a progress bar. Can be NULL.
									//!< The function is called regularly (see UT_LOAD_STEP and UT_PROCESS_STEP),
									//!< with the float argument betwwen 0.0 and 1.0, indicating the part of the job done.
									//!< The function is called only once with 0.0 (the first time) and once with 1.0 (the
									//!< last time), so these values can be checked to do initialisation and cleanup. 
									//!< If this function return 0, the processing is interrupted. Set by user.

	char * error_string;			//!< error message (seldom used).
} UtSession;

#ifdef _UTRAC_C_
	UtSession * ut_session = NULL; //!< Point to the UtSession structure instanciated by ut_init().
#else
	extern UtSession  * ut_session;
#endif

#define UT_TRY(func) \
{\
	UtCode rcode = func;\
	if (rcode != UT_OK) return rcode;\
}


// ********** utrac.c ********** 
UtCode ut_init ();
UtCode ut_init_noalloc ();
void ut_finish ();
void ut_finish_nofree ();
UtText * ut_init_text_heap ();
void ut_init_text (UtText * new_text);
void ut_free_text_heap (UtText *text);
void ut_free_text (UtText * text);



UtCode ut_init_progress (UtText *text);
UtCode ut_load (UtText *text, const char * filename);
UtCode ut_recognize (UtText *text);
UtCode ut_convert (UtText *src_text, UtText *dst_text);

//UtCode ut_process_text (UtText * text, bool convert);

// ********** utils.c ********** 
UtCode ut_load_charsets ();
UtCode ut_load_charset_file (const char * filename, char ** buffer);
UtCharsetIndex ut_find_charset (char * charset_name);
UtEolType ut_find_eol (char * eol_name);
int ut_find_lang_sys (char * language_name, UtLangSys * lang_sys);

double ut_get_charset_coef (UtCharsetIndex i);
bool ut_str_fuzzy_cmp (const char *str1, const char *str2, char stop_char);

bool ut_update_progress (struct UtText *, ulong, bool);

ulong ut_crc32 (ushort , ulong);

void ut_print_binary (ulong src);
UtCode ut_debug_text (struct UtText *);
UtCode ut_debug_text_rating (struct UtText *);

// ************** ut_messages.c *************
const char * ut_error_message (UtCode code);

// ************** ut_load.c *************
UtCode ut_load_file_pass (UtText *text, const char * filename);
UtCode ut_load_stdin_pass (UtText *text);

// ********** ut_recognition1.c ********** 
UtCode ut_distrib_utf_pass (struct UtText *);
UtCode ut_eol_pass (struct UtText *);

// ********** ut_recognition2.c ********** 
UtCode ut_xascii_pass  (struct UtText *);

// ********** ut_conversion.c ********** 
int ut_size_char (char **src_p, UtCharsetIndex src_charset, UtCharsetIndex dst_charset);
void ut_conv_char (char ** src_p, char ** dst_p, UtCharsetIndex src_charset, UtCharsetIndex dst_charset);
void ut_insert_eol (char ** dst_p, UtEolType dst_eol);

uint ut_count_ext_char (UtText * text);
int ut_size_difference (UtText * src_text, UtText * dst_text);
	
UtCode ut_conversion_pass (UtText * src_text, UtText * dst_text);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _UTRAC_H
