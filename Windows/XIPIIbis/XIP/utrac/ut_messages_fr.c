/***************************************************************************
 *            ut_messages_fr.c
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
 * \file ut_messages_fr.c
 * \author  Antoine Calando (antoine@alliancemca.net)
 */

#include "debug.h"
#include "utrac.h"
#include "ut_error.h"



/*!
 *\brief Names of eol type.
 */
const char * UT_EOL_NAME[] = { "CR", "LF", "CRLF", "LFCR", "Mix","BSN", "Nul", "Aucun" };

typedef struct UtErrorMessage {
	UtCode code;
	const char * message;
} UtErrorMessage;


const char * ut_error_message (UtCode code) {
	
	UtErrorMessage array[] = {
		{ UT_INTERRUPTED_BY_USER, "Traitement interrompu par l'utilisateur" },
		//{ UT_ERROR,	"Erreur indéfinie" },
		//{ UT_SIZE_MISSING, "Taille manquante" },
		{ UT_NOT_FOUND_ERROR, "Element non trouvé" },

		{ UT_MALLOC_ERROR, "Allocation mémoire refusée"},
		{ UT_OPEN_FILE_ERROR, "Impossible d'ouvrir le fichier"},
		{ UT_CLOSE_FILE_ERROR, "Impossible de fermer le fichier"},
		{ UT_FSTAT_FILE_ERROR, "Appel à fstat refusé"},
		{ UT_READ_FILE_ERROR, "Erreur lors de la lecture"},
		{ UT_READ_FILE_ERROR2, "Erreur lors de la lecture"},
		{ UT_FILE_TOO_BIG_ERROR, "Fichier trop gros"},

		{ UT_ALREADY_INITIALISED_ERROR, "Utrac déjà initialisé"},
		{ UT_SYNTAX_ERROR, "Erreur de syntaxe"},
		{ UT_STRING_MISSING_ERROR, "Chaîne de caractère manquante"},
		{ UT_LANG_SYS_ALREADY_DEFINED_ERROR, "Langage/système déjà défini"},
		{ UT_LANG_SYS_CODE_MISSING_ERROR, "Code langage/système manquant"},
		{ UT_PARTIAL_LANG_SYS_CODE_ERROR, "Code langage/système manquant"},
		{ UT_LANG_SYS_NAME_MISSING_ERROR, "Nom langage/système manquant"},
		{ UT_LANG_SYS_UNDEFINED_ERROR, "Langage/système indéfini"},
		{ UT_LANG_SYS_COEF_MISSING_ERROR, "Coefficient langage/système manquant"},
		{ UT_LANG_SYS_INCORRECT_COEF_ERROR, "Coefficient langage/système incorrect"},
		{ UT_LANG_SYS_COEF_TOO_BIG_ERROR, "Coefficient langage/système trop grand"},
		{ UT_LANG_SYS_DEF_AFTER_CHARSET_ERROR, "Langage/système défini après un charset"},
		{ UT_INCORRECT_CHARMAP_ENTRY_ERROR, "Entrée charmap incorrecte"},
		{ UT_UNDEFINED_CATEGORY_ERROR, "Catégorie de caractère indéfinie"},
		{ UT_CHAR_TOO_BIG_ERROR, "Caractère trop grand"},
		{ UT_UNICODE_CHAR_TOO_BIG_ERROR, "Caractère Unicode trop grand"},
		{ UT_CHARSET_FILE_ERROR, "Erreur dans le fichier de charsets"},
		{ UT_CHARMAP_ENTRY_ILLEGAL_ERROR, "Entrée charmap illegalle dans ce charset"},
	
		{ UT_BAD_PARAMETER_ERROR, "Mauvais paramètre"},
		//{ UT_BAD_FLAGS_ERROR, ""},
		//{ UT_DATA_OR_FILNAME_UNSPECIFIED_ERROR, ""},
		//{ UT_EMPTY_FILE_ERROR, "Fichier vide"},
		{ UT_EMPTY_DATA_ERROR, "Données inexistantes"},
		{ UT_BINARY_DATA_ERROR, "Données binaires"},
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
