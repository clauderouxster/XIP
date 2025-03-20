/***************************************************************************
 *            ut_messages_en.c
 *
 *  Jan 20 2005
 *  Copyright  2005  Alliance MCA
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
 * \file ut_messages_en.c
 * \author  Antoine Calando (antoine@alliancemca.net)
 */

#include "debug.h"
#include "utrac.h"
#include "ut_error.h"


/*!
 *\brief Names of eol type.
 */
const char * UT_EOL_NAME[] = { "CR", "LF", "CRLF", "LFCR", "Mix","BSN", "Nul", "None" };

typedef struct UtErrorMessage {
	UtCode code;
	const char * message;
} UtErrorMessage;


const char * ut_error_message (UtCode code) {
	
	UtErrorMessage array[] = {
		{ UT_INTERRUPTED_BY_USER, "Processing interrupted by user" },
		//{ UT_ERROR,	"Erreur indéfinie" },
		//{ UT_SIZE_MISSING, "Taille manquante" },
		{ UT_NOT_FOUND_ERROR, "Element not found" },

		{ UT_MALLOC_ERROR, "Memory allocation refused"},
		{ UT_OPEN_FILE_ERROR, "Unable to open file"},
		{ UT_CLOSE_FILE_ERROR, "Unable to close file"},
		{ UT_FSTAT_FILE_ERROR, "Call to fstat refused"},
		{ UT_READ_FILE_ERROR, "Error during reading"},
		{ UT_READ_FILE_ERROR2, "Error during reading"},
		{ UT_FILE_TOO_BIG_ERROR, "File too big"},

		{ UT_ALREADY_INITIALISED_ERROR, "Utrac already initialised"},
		{ UT_SYNTAX_ERROR, "Syntax error"},
		{ UT_STRING_MISSING_ERROR, "Missing string"},
		{ UT_LANG_SYS_ALREADY_DEFINED_ERROR, "Language/system already defined"},
		{ UT_LANG_SYS_CODE_MISSING_ERROR, "Missing language/system code "},
		{ UT_PARTIAL_LANG_SYS_CODE_ERROR, "Incomplete language/system entry"},
		{ UT_LANG_SYS_NAME_MISSING_ERROR, "Missing language/system name"},
		{ UT_LANG_SYS_UNDEFINED_ERROR, "Undefined language/system"},
		{ UT_LANG_SYS_COEF_MISSING_ERROR, "Missing language/system coefficient"},
		{ UT_LANG_SYS_INCORRECT_COEF_ERROR, "Incorrect language/system coefficient"},
		{ UT_LANG_SYS_COEF_TOO_BIG_ERROR, "Language/system coefficient too big"},
		{ UT_LANG_SYS_DEF_AFTER_CHARSET_ERROR, "Language/system defined after charsets"},
		{ UT_INCORRECT_CHARMAP_ENTRY_ERROR, "Incorrect charmap entry"},
		{ UT_UNDEFINED_CATEGORY_ERROR, "Undefined character category"},
		{ UT_CHAR_TOO_BIG_ERROR, "Character too big"},
		{ UT_UNICODE_CHAR_TOO_BIG_ERROR, "Unicode character too big"},
		{ UT_CHARSET_FILE_ERROR, "Error in charsets file"},
		{ UT_CHARMAP_ENTRY_ILLEGAL_ERROR, "Illegal charmap entry in this charset"},
	
		{ UT_BAD_PARAMETER_ERROR, "Bad parameter"},
		//{ UT_BAD_FLAGS_ERROR, ""},
		//{ UT_DATA_OR_FILNAME_UNSPECIFIED_ERROR, ""},
		//{ UT_EMPTY_FILE_ERROR, "Fichier vide"},
		{ UT_EMPTY_DATA_ERROR, "No data"},
		{ UT_BINARY_DATA_ERROR, "Binary data"},
		//{ UT_CHARSET_NOT_RECOGNIZED_ERROR, ""},
		
		{ UT_UNSET, NULL }
	};
	
	int i=0;
	while (array[i].code != UT_UNSET) {
		if (array[i].code == code) return array[i].message;
		i++;
	}
			
	return NULL;
}
