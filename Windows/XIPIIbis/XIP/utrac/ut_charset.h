/***************************************************************************
 *            ut_charset.h
 *
 *  Tue Oct  5 11:27:31 2004
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
 * \file ut_charset.h
 * \author Antoine Calando (antoine@alliancemca.net)
 */

#ifndef _UT_CHARSET_H_
#define _UT_CHARSET_H_

#include <sys/types.h>

//#include "debug.h"

/*!
 * \brief Categories to classify characters.
 *
 * They are inspirated from ftp://ftp.unicode.org/Public/UNIDATA/UCD.html#General_Category_Values
 *
 * \todo EC explication de tous les enums
 *       AC voir lien!!!
 */
typedef enum UtCateg {
	UT_CTG_UNSET=0 /*A*/,
	UT_CTG_UPPERCASE/*B*/,
	UT_CTG_LOWERCASE/*C*/,
	UT_CTG_OTHER_LETTER/*D*/,
	UT_CTG_NUMBER/*E*/,
	UT_CTG_PONCTUATION,
		UT_CTG_PONCT_INIT_0/*G*/,
		UT_CTG_PONCT_INIT_1,
		UT_CTG_PONCT_INIT_2,
		UT_CTG_PONCT_INIT_3,
		UT_CTG_PONCT_INIT_OTHER,
		UT_CTG_PONCT_FINAL_0/*L*/,
		UT_CTG_PONCT_FINAL_1,
		UT_CTG_PONCT_FINAL_2,
		UT_CTG_PONCT_FINAL_3,
		UT_CTG_PONCT_FINAL_OTHER, 
	UT_CTG_CURRENCY/*Q*/,
	UT_CTG_SYMBOL,
	UT_CTG_CONTROL,
	UT_CTG_DELIMITER/*T*/,
	UT_CTG_MARK,
	UT_CTG_OTHER/*V*/
} UtCateg;


#define UT_CTG_PONCT_IF_N UT_CTG_PONCT_INIT_OTHER-UT_CTG_PONCT_INIT_0+1

/*! Type of alphabet (latin, cyrillic, arabic, greek, hebrew, thai...)
	The index is the same than for array SCRIPT_NAME in ut_charset.c
*/
typedef u_char UtScript;

/*!
 * \brief Description of a character.
 */
typedef struct UtCharType {
	UtCateg categorie; 	//!< Categorie of the character.
	UtScript script;	//!< Alphabet of the character.
} UtCharType;


/*! \brief Charset Name */

#ifdef _UT_CHARSET_C_
const char * UT_CHARSET_NAME[]  = {
	"ASCII",
	"UTF-8",
	"UTF-16BE",
	"UTF-16LE",
	"UTF-32BE",
	"UTF-32LE",
	NULL
};
#else
extern const char * UT_CHARSET_NAME[];
#endif

/*!
 * \biref Charset type.
 *
 * ASCII extension means charsets monobytes whose 128 firsts characters are the same
 * than ASCII's ones.
 */

typedef enum UtCharsetType {
	UT_CST_UNSET = -1,
	UT_CST_ASCII = 0,
	UT_CST_UTF_8,
	UT_CST_UTF_16BE,	//!< unimplemented.
	UT_CST_UTF_16LE,	//!< unimplemented.
	UT_CST_UTF_32BE,	//!< unimplemented.
	UT_CST_UTF_32LE,	//!< unimplemented.
	UT_CST_ASCII_EXTENSION
} UtCharsetType;


/*!
 * \brief Contains informations about a charset.
 *
 * This structure is used to describe a charset.  It is stocked as an array in UtSession::charset.
 * This array is created from file charsets.dat by ut_load_charsets() (called in ut_init()); it is
 * destroyed in ut_finish().
 */
typedef struct UtCharset {
	char * name;			//!< Standard name of the charset.
	char * alias;			//!< Other name (TODO: make an array.)
	char * common_name;		//!< Friendly name for non-geek users.
	char * comment;			//!< Friendly comment for non-geek users.
	UtCharsetType type;		//!< Type of the charset.
	ushort * unicode;		//!< Unicode charmap array (on 16 bits!).
	UtCharType * char_type;	//!< Character description arra
	u_char * language;		//!< Coefficients array of size \link UtSession::language ut_session->language.n \endlink
	u_char * system;			//!< Coefficients array of size \link UtSession::system ut_session->system.n \endlink
} UtCharset;

#endif // _UT_CHARSET_H_
