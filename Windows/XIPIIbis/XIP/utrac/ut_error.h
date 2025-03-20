/***************************************************************************
 *            ut_error.h
 *
 *  Tue Oct  5 11:27:59 2004
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
 * \file ut_error.h
 * \author  Antoine Calando (antoine@alliancemca.net)
 *
 * \todo tableau de correspondance avec des messages en clair.
 */
 
#ifndef _UT_ERROR_H_
#define _UT_ERROR_H_
 
typedef enum {
	UT_OK=0,
	UT_INTERRUPTED_BY_USER,
	UT_ERROR,
	//UT_SIZE_MISSING,
	UT_NOT_FOUND_ERROR,
	//UT_UTF8_RECOGNIZED,

	UT_MALLOC_ERROR=100,
	UT_OPEN_FILE_ERROR,
	UT_CLOSE_FILE_ERROR,
	UT_FSTAT_FILE_ERROR,
	UT_READ_FILE_ERROR,
	UT_READ_FILE_ERROR2,
	UT_FILE_TOO_BIG_ERROR,

	UT_ALREADY_INITIALISED_ERROR=200,
	UT_SYNTAX_ERROR=210,
	UT_STRING_MISSING_ERROR,
	UT_LANG_SYS_ALREADY_DEFINED_ERROR,
	UT_LANG_SYS_CODE_MISSING_ERROR,
	UT_PARTIAL_LANG_SYS_CODE_ERROR,
	UT_LANG_SYS_NAME_MISSING_ERROR,
	UT_LANG_SYS_UNDEFINED_ERROR = 220,
	UT_LANG_SYS_COEF_MISSING_ERROR,
	UT_LANG_SYS_INCORRECT_COEF_ERROR,
	UT_LANG_SYS_COEF_TOO_BIG_ERROR,
	UT_LANG_SYS_DEF_AFTER_CHARSET_ERROR,
	UT_INCORRECT_CHARMAP_ENTRY_ERROR,
	UT_UNDEFINED_CATEGORY_ERROR = 230,
	UT_CHAR_TOO_BIG_ERROR,
	UT_UNICODE_CHAR_TOO_BIG_ERROR,
	UT_CHARSET_FILE_ERROR,
	UT_CHARMAP_ENTRY_ILLEGAL_ERROR,
	
	UT_BAD_PARAMETER_ERROR = 300,
	UT_BAD_FLAGS_ERROR ,
	UT_DATA_OR_FILNAME_UNSPECIFIED_ERROR,
	//UT_EMPTY_FILE_ERROR,
	UT_EMPTY_DATA_ERROR,
	UT_BINARY_DATA_ERROR = 310,
	UT_CHARSET_NOT_RECOGNIZED_ERROR,
	
	UT_ERROR_MAX = 500
} UtCode;
 
#endif // _UT_ERROR_H_
