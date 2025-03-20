/***************************************************************************
 *            ut_charset.c
 *
 *  Fri Apr 23 15:24:30 2004
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
 
 
/*!\file
 * \brief Functions which parse the charset.dat file.
 *
 * \author Antoine Calando (antoine@alliancemca.net)
 *
 * \todo EC les fonction inline be fonctionne qu'avec gcc !! il faudrait mettre
 *       une macro UT_INLINE dans un header.
 *       AC ???
 */

#define _UT_CHARSET_C_

#include <stdlib.h>
#include <stdio.h>
#include <endian.h>
#include <byteswap.h>
#define __USE_GNU	//for strndup
#include <string.h>

#include "utrac.h"
#include "ut_charset.h"

//#undef UT_DEBUG
//#define UT_DEBUG 3
#include "debug.h"

// ***************************************************************************************
// const char * charmaps_filename = "/home/antoine/dev/libimport/charmaps_categ.txt";

//! \brief 	Keywords used in file charset.dat. 
const static char * charmap_keyword = "Charmap:";
const static char * alias_keyword = "Alias:";
const static char * common_name_keyword = "CommonName:";
const static char * comment_keyword = "Comment:";
const static char * language_keyword = "Language:";
const static char * system_keyword = "System:";
const static char * language_def_keyword = "DefineLanguage:";
const static char * system_def_keyword = "DefineSystem:";

//! \brief Alphabet names that can be recognized in file charset.dat
const static char * SCRIPT_NAME[] = { "LATIN", "CYRILLIC", "ARABIC", "GREEK", "HEBREW", "THAI", NULL};

// ***************************************************************************************
/*!
 * \brief test si le caractère ASCII (sur un octet) est un espace ou une tabulation
 * \note EC pourquoi ne pas utiliser isblank à la place de is_blank ? (a cause du char ?)
 * \note AC parce que isblank est spécifique à la glibc! (réponse trouvé à posteriori :)
 */
static inline bool is_blank (char c) { return (c==' ' || c=='\t'); }

/*!
 * \brief test si le caractère ASCII (sur un octet) est une fin de ligne
 * \note EC On a réservé des octets de fin lors du malloc pour charger le fichier texte, ces
 *       octets devaient contenir la dernière fin de ligne si elle n'est pas présente et
 *       le caractère 0 final (une ligne devrait toujours ce finir par la combinaison de fin
 *       de ligne. De plus, un \r n'est pas forcemment une fin de ligne, cela peut être
 *       un saut de ligne dans un champ... Dans quels cas est utilsée cette fonction ? Ne
 *       faudrait-il pas plusieurs fonctions ?
 * \note AC Ok pour la première remarque (un '\n' a été rajouté à la fin du fichier dans
 *       ut_load_file() et le test sur 0 a été retiré ici). Sinon le '\r' est forcement
 *       une fin de ligne car il s'agit ici du fichier contenant les infos charsets
 *       qui peut au pire être au format CR/LF/CRLF (selon système) mais rien d'autre.
 * 
 */
static inline bool is_eol (char c)   { return (c=='\n' || c=='\r' /*|| c=='\0'*/); }

/*!
 * \brief test si le caractère ASCII (sur un octet) est une fin de ligne ou un début de commentaire
 * \note EC pourquoi ne pas utiliser is_eol ?
 * \note AC pour le test sur '#' (un appel, même inline, embrouillerait le source) 
 */
static inline bool is_eol_c (char c) { return (c=='#' || c=='\n' || c=='\r' /*|| c=='\0'*/); }

/*!
 * \brief Link used to store temporarily UtCharset structures in a linked list
 * \note EC Une définition de structure doit se trouver dans un header
 * \note AC Elle n'est nécessaire qu'ici, ça permet d'alléger les .h
 */
typedef struct UtCharmapLink {
	UtCharset * charset;
	struct UtCharmapLink * next;
} UtCharmapLink;

/**************************************************************************/
/*!
 * \brief Expend size of an UtLangSys dynamic array
 *
 * \bug EC Il n'y a pas de test du retour de realloc !
 *       La fonction pouvant planter (realloc), elle doit renvoyer un code d'erreur.
 *      AC corrigé.
 */
static UtCode expend_lang_sys (UtLangSys *lang_sys) {

	lang_sys->n_max += UT_LANG_SYS_ALLOC_STEP;	
	lang_sys->name = (char**)  realloc (lang_sys->name, lang_sys->n_max*sizeof(char*));
	//lang_sys->code = (ushort*) realloc (lang_sys->code, lang_sys->n_max*sizeof(ushort));
	lang_sys->code = (char*) realloc (lang_sys->code, lang_sys->n_max*2);
	//lang_sys->code[0] = 0; lang_sys->code[1] = 0;
	
	if (!lang_sys->name	|| !lang_sys->code) return UT_MALLOC_ERROR;
	//if (!lang_sys->name	) return UT_MALLOC_ERROR;
	else return UT_OK;
		
	DBG3 ("Lang/sys dynamic array (at %p) expended to %d elements", lang_sys, lang_sys->n_max)
}

/**************************************************************************/
/*!
 * \brief Copy a string from file buffer.
 *
 * \bug  EC Dans le cas ou la premiere ligne d'un buffer lu en mémoire ne contienne pas de données
 *       pertinantes (ligne vide, ligne d'espace, etc.), la fin de ligne serait touvée, puis
 *       lors de la boucle de recherche du dernier caractère espace ou tabulation, on remonterait
 *       à *(buffer-1) ce qui entrainerait une segmentation fault (lors de la commande ou lors
 *       du free).
 *       AC La fonction n'est appelé que sur une ligne débutant par une commande, donc de la mémoire
 *       lisible.
 * \bug  EC le retour de strndup() n'est pas testé ! 
 *       AC pfff...  pour un dizaine d'octet max... c'est de la diptèrophilie...
 */
static UtCode parse_string_line (char** scan_in, char ** dst) {

	char *scan = *scan_in;

	while (is_blank(*scan)) scan++; //trim space before language name

	char * name_beg = scan;
	while (!is_eol_c(*scan)) scan++; //find eol or comment
	do scan--; while (is_blank(*scan));	//go back until first nonblank char
	
	if (scan-name_beg<0) return UT_STRING_MISSING_ERROR;
	
	*dst = strndup (name_beg, scan-name_beg+1);

	*scan_in = scan;
	return UT_OK;
}

/**************************************************************************/
/*!
 * \brief Parse parameter of a "DefineLanguage" or "DefineSystem" line.
 */
static UtCode parse_lang_sys_def_line (char** scan_in, UtLangSys * lang_sys) {

	char *scan = *scan_in;
	
	if (ut_session->nb_charsets) return UT_LANG_SYS_DEF_AFTER_CHARSET_ERROR;
	if (lang_sys->n == lang_sys->n_max) {
		UT_TRY( expend_lang_sys (lang_sys) )
	}

	//printf (scan);
	while (is_blank(*scan)) 
		scan++; //trim space before language id
	
	if (is_eol_c(*scan)) return 	UT_LANG_SYS_CODE_MISSING_ERROR;
		
	if (is_blank(*(scan+1)) || is_eol_c(*(scan+1))) return UT_PARTIAL_LANG_SYS_CODE_ERROR;
	
	//lang_sys->code [lang_sys->n] = *(((ushort*)(scan)))++;
	//#if BYTE_ORDER == LITTLE_ENDIAN
	//bswap_16(lang_sys->code [lang_sys->n]);
	//#endif
	lang_sys->code [lang_sys->n*2+0] = *scan++;
	lang_sys->code [lang_sys->n*2+1] = *scan++;
	
	//check if language exists
	int i; for (i=0; i<lang_sys->n; i++)
		if (lang_sys->code [i*2+0] == lang_sys->code [lang_sys->n*2+0] && 
			lang_sys->code [i*2+1] == lang_sys->code [lang_sys->n*2+1]) 
			return UT_LANG_SYS_ALREADY_DEFINED_ERROR;

	UtCode rcode = parse_string_line (&scan, &lang_sys->name[lang_sys->n]);
	if (rcode!=UT_OK) return rcode;
	
	lang_sys->n++;
	
	DBG("Lang/sys (%p) added : %s (%c%c) at pos %d", 
			lang_sys, lang_sys->name [lang_sys->n],
			lang_sys->code [lang_sys->n*2+0],
			lang_sys->code [lang_sys->n*2+1], lang_sys->n-1)
	
	*scan_in = scan;
	return UT_OK;
}


/**************************************************************************/
/*!
 * \brief Parse parameter of a "Charmap"line.
 */
static UtCode parse_charmap_line (char** scan_in, UtCharmapLink ** current_link) {
	
	char* scan = *scan_in;
	UtCharmapLink * old_link = *current_link;
	
	UtCharset * new_charset = (UtCharset*) malloc (sizeof(UtCharset));
	if (!new_charset) return UT_MALLOC_ERROR;
	new_charset->name = NULL;
	new_charset->alias = NULL;
	new_charset->common_name = NULL;
	new_charset->comment = NULL;
	new_charset->type = UT_CST_UNSET;
	new_charset->language = (u_char*) malloc (ut_session->language.n*(sizeof(u_char)));
	new_charset->system = (u_char*) malloc (ut_session->system.n*(sizeof(u_char)));
	new_charset->unicode = NULL;
	new_charset->char_type = NULL;
	
	int i; 
	for (i=0; i<ut_session->language.n; i++) new_charset->language[i] = 0;
	for (i=0; i<ut_session->system.n; i++) new_charset->system[i] = 0;

	UtCode rcode = parse_string_line (&scan, &new_charset->name);
	if (rcode!=UT_OK) return rcode;

	i = 0; while (UT_CHARSET_NAME[i]) {
		if (strcmp (UT_CHARSET_NAME[i], new_charset->name)==0) break;
		i++;
	}
	new_charset->type = (UtCharsetType) i;
	
	UtCharmapLink * new_link;
	if (old_link->charset ) {
		new_link = (UtCharmapLink*) calloc (1, sizeof(UtCharmapLink));
		old_link->next = new_link; 
	} else {
		new_link = old_link;
	}
	new_link->charset = new_charset;
	new_link->next = NULL;
	ut_session->nb_charsets++;

	DBG3 (" - Charset %s added! - ", new_charset->name)
	*current_link = new_link;
	*scan_in = scan;
	return UT_OK;
}

/**************************************************************************/
/*!
 * \brief Parse parameter of a "Language" or "System" line.
 */
static UtCode parse_lang_sys_line (char** scan_in, UtLangSys * lang_sys, char * lang_sys_coef) {
	char *scan = *scan_in;
	
	u_char language_id, coef_id;

	for(;;) {
		while (is_blank(*scan)) scan++;
		if (is_eol_c(*scan)) break;
		//ushort lang_sys_code = *(ushort*)scan;
		#if BYTE_ORDER == LITTLE_ENDIAN
		bswap_16 (*(ushort*)scan);
		#endif
		
		for (language_id=0; language_id<lang_sys->n; language_id++) {
			//if ( *(ushort*)scan == lang_sys->code[language_id]) break;
			if (    *scan == lang_sys->code[language_id*2+0] &&
				*(scan+1) == lang_sys->code[language_id*2+1]) break;
		}
		
		if (language_id==lang_sys->n) return UT_LANG_SYS_UNDEFINED_ERROR;
			
		scan+=2;
		if (*scan==':') {
			char * beg = ++scan;
			coef_id = strtoul (beg, &scan, 0);
			if (beg==scan) return UT_LANG_SYS_COEF_MISSING_ERROR;
			if (!is_blank(*scan) && !is_eol_c(*scan)) return UT_LANG_SYS_INCORRECT_COEF_ERROR;
			if (coef_id>UT_COEF_MAX) return UT_LANG_SYS_COEF_TOO_BIG_ERROR;
		} else coef_id = 1;

		lang_sys_coef[language_id] = coef_id;
	} // for(;;)

	*scan_in = scan-1;
	return UT_OK;
}

/**************************************************************************/
/*!
 * \brief Parse charmap entry.
 */
static UtCode parse_charmap_entry (char** scan_in, UtCharset * charset) {
	
	if (charset->type!=UT_CST_ASCII && charset->type!=UT_CST_ASCII_EXTENSION)
		return UT_CHARMAP_ENTRY_ILLEGAL_ERROR;
	
	char* scan = *scan_in;
	char * hex_beg = scan;
	
	ulong character = strtoul (hex_beg, &scan, 16);
	if (hex_beg==scan) return UT_INCORRECT_CHARMAP_ENTRY_ERROR;    //useless?

	if (character >= 0x80 && charset->type!=UT_CST_ASCII_EXTENSION)
		return UT_CHARMAP_ENTRY_ILLEGAL_ERROR;

	hex_beg = scan;
	ulong unicode = strtoul (hex_beg, &scan, 16);
	if (hex_beg==scan) unicode = UT_UNICODE_NONCHAR;   //some unicode entries are empty!
	if (character>0xFF)		return UT_CHAR_TOO_BIG_ERROR;
	if (unicode > 0xFFFF) return UT_UNICODE_CHAR_TOO_BIG_ERROR;
		
	if (!charset->unicode && !charset->char_type) {
		charset->unicode = (ushort*) malloc (sizeof( ushort[0x100]));
		charset->char_type = (UtCharType*) malloc (sizeof( UtCharType[0x100]));
		if (!charset->unicode || !charset->char_type) 
			return UT_MALLOC_ERROR;
		int i; for (i=0; i<0x100; i++) {
			charset->unicode[i] = UT_UNICODE_NONCHAR;
			charset->char_type[i].categorie = UT_CTG_UNSET;
			charset->char_type[i].script = 0;
		}
	}
	
	charset->unicode[(u_char)character] = (ushort) unicode;
	
	while (is_blank(*scan)) scan++;
	
	if ('A'<=*scan && *scan <= 'Z') {
		
		if (character==0||character==0x9||character==0xA||character==0xD||character==0x20)
			charset->char_type[(u_char) character].categorie = UT_CTG_DELIMITER;
		else 
		  #if BYTE_ORDER==BIG_ENDIAN
		  switch (* (ushort*) scan ) {
		  #else
		  switch (bswap_16(* (ushort*) scan )) { //}
		  #endif
			case 'Lu': charset->char_type[(u_char) character].categorie = UT_CTG_UPPERCASE; break;
			case 'Ll': charset->char_type[(u_char) character].categorie = UT_CTG_LOWERCASE; break;
			case 'Lt': 
			case 'Lm': 
			case 'Lo': charset->char_type[(u_char) character].categorie = UT_CTG_OTHER_LETTER; break;

			case 'Mn': charset->char_type[(u_char) character].categorie = UT_CTG_MARK; break;
			case 'Mc': 
			case 'Me': charset->char_type[(u_char) character].categorie = UT_CTG_OTHER; break;

			case 'Nd': 
			case 'Nl': 
			case 'No': charset->char_type[(u_char) character].categorie = UT_CTG_NUMBER; break;

			case 'Pc': 
			case 'Pd': 
			case 'Po': charset->char_type[(u_char) character].categorie = UT_CTG_PONCTUATION; break;
			case 'Ps': charset->char_type[(u_char) character].categorie = UT_CTG_PONCT_INIT_OTHER ; break;
			case 'Pe': charset->char_type[(u_char) character].categorie = UT_CTG_PONCT_FINAL_OTHER ; break;
			case 'Pi':
				switch (unicode) {
					case 0x00AB: charset->char_type[(u_char) character].categorie = UT_CTG_PONCT_INIT_0 ; break;
					case 0x2018: charset->char_type[(u_char) character].categorie = UT_CTG_PONCT_INIT_1 ; break;
					case 0x201C: charset->char_type[(u_char) character].categorie = UT_CTG_PONCT_INIT_2 ; break;
					case 0x2039: charset->char_type[(u_char) character].categorie = UT_CTG_PONCT_INIT_3 ; break;
					default: charset->char_type[(u_char) character].categorie = UT_CTG_PONCT_INIT_OTHER ; break;
				} break;
		
			case 'Pf': 
				switch (unicode) {
					case 0x00BB: charset->char_type[(u_char) character].categorie = UT_CTG_PONCT_FINAL_0 ; break;
					case 0x2019: charset->char_type[(u_char) character].categorie = UT_CTG_PONCT_FINAL_1 ; break;
					case 0x201D: charset->char_type[(u_char) character].categorie = UT_CTG_PONCT_FINAL_2 ; break;
					case 0x203A: charset->char_type[(u_char) character].categorie = UT_CTG_PONCT_FINAL_3 ; break;
					default: charset->char_type[(u_char) character].categorie = UT_CTG_PONCT_FINAL_OTHER ; break;
				} break;

			case 'Sc': charset->char_type[(u_char) character].categorie = UT_CTG_CURRENCY; break;
			case 'Sm': 
			case 'Sk': 
			case 'So': charset->char_type[(u_char) character].categorie = UT_CTG_SYMBOL; break;

			case 'Zs': charset->char_type[(u_char) character].categorie = UT_CTG_DELIMITER; break;
			case 'Zl': 
			case 'Zp': charset->char_type[(u_char) character].categorie = UT_CTG_OTHER; break;

			case 'Cc': charset->char_type[(u_char) character].categorie = UT_CTG_CONTROL; break;
			case 'Cf': 
			case 'Cs': 
			case 'Co': 
			case 'Cn': charset->char_type[(u_char) character].categorie = UT_CTG_OTHER; break;
			default: return UT_UNDEFINED_CATEGORY_ERROR;
		}
		scan +=2;
		while (is_blank(*scan)) scan++;
	}
	
	//look for an script type in the comment (latin, arabic, hebrew...)
	if (*scan == '#') {							//is there a comment?
		const char ** script = SCRIPT_NAME;
		int index_script = 0;
		char * first_eol, *first_script;
		first_eol = strchr (scan, '\n');		//find the eol and replace it by \0
		if (first_eol) *first_eol=0;			//in order to use strstr
		while (*script) {
			index_script++;
			first_script = strstr (scan, *script);		//locate substring
			if (first_script && first_script < first_eol) {
				charset->char_type [(u_char) character].script = (char) index_script;//found
				if (first_eol) scan = first_eol; //speed up the parsing
				break;
			} 
			script++;
		}
		if (first_eol) *first_eol='\n';			//replace the 0 by the initial eol
	}
	//while (*scan!='\n') scan++;
	*scan_in = scan-1;
	return UT_OK;
}

//! \brief Compare a null-ended string with a non-null-ended string.
static bool streq (const char * src, char **cmp) {
	char *cmp_scan = *cmp;
	while (*src) {
		if (*src!=*cmp_scan || !*cmp_scan) return false;
		src++; cmp_scan++;
	}
	*cmp = cmp_scan;
	return true;
}


UtCode ut_print_charsets () {
	
	int i; for (i=0; i < ut_session->nb_charsets; i++) {
		printf ("%2d: %20s %2d [", i, ut_session->charset[i].name, ut_session->charset[i].type);
		int j; for (j=0; j<ut_session->language.n_max;j++) printf("%d ",(int)ut_session->charset[i].language[j]);
		printf("] [");	
		for (j=0; j<ut_session->system.n_max;j++) printf("%d ",(int)ut_session->charset[i].system[j]);
		printf("]\n");
	}
	
	
}


/*****************************************************************************/
/*!
 * \brief Loads and parses file charset.dat.
 *
 * This function loads and parses file charset.dat containing all informations about
 * charset in a UtCharset array in UtSession::charset.
 *
 * \return UT_CODE on success, error code otherwise
 *
 * \todo documentation of the charset.dat file
 */
UtCode ut_load_charsets () {
	
	DBG3 ("Loading charsets...")

	int i;
	char * file_buffer;
	int rcode;
	const char * filename;
	{
		#ifdef UT_CHARMAPS_FILENAME
		filename = UT_CHARMAPS_FILENAME;
		rcode = ut_load_charset_file (filename, &file_buffer);
	}
	if (rcode!=UT_OK) {
		#endif
		filename = UT_CHARMAPS_FILENAME2;
		rcode = ut_load_charset_file (filename, &file_buffer);
	}

	if (rcode!=UT_OK) return rcode;
	
	char * scan = file_buffer;
	int line = 1;
	
	//each new charmap is added to a linked list
	UtCharmapLink * charmap_list = (UtCharmapLink*) calloc (1, sizeof(UtCharmapLink));
	UtCharmapLink * current_link = charmap_list;
	
	//parse file 
	while (*scan) {
		if (*scan=='\r') {
			if (*(scan+1)=='\n') scan++;
			line++;
		} else if (*scan=='\n') {
			line++;
		} else if (!is_blank(*scan)) {
			if (*scan=='#') {
				while (!is_eol(*++scan));
				scan--;
			} else if (*scan=='0' && *(scan+1)=='x') {
				rcode = parse_charmap_entry(&scan, current_link->charset);

			} else if ( streq (charmap_keyword, &scan) ) {
				rcode = parse_charmap_line(&scan, &current_link);
			} else if ( streq (alias_keyword, &scan) ) {
				rcode = parse_string_line(&scan, &current_link->charset->alias);
			} else if ( streq (common_name_keyword, &scan) ) {
				rcode = parse_string_line(&scan, &current_link->charset->common_name);
			} else if ( streq (comment_keyword, &scan) ) {
				rcode = parse_string_line(&scan, &current_link->charset->comment);

			} else if ( streq (language_keyword, &scan) ) {
				rcode = parse_lang_sys_line(&scan, &ut_session->language, current_link->charset->language);
			} else if ( streq (system_keyword, &scan) ) {
				rcode = parse_lang_sys_line(&scan, &ut_session->system, current_link->charset->system);

			} else if ( streq (language_def_keyword, &scan) ) {
				rcode = parse_lang_sys_def_line(&scan, &ut_session->language);
			} else if ( streq (system_def_keyword, &scan) ) {
				rcode = parse_lang_sys_def_line(&scan, &ut_session->system);
			} else {
				//error
				//rcode = utSYNTAX_ERROR;
				if (!ut_session->error_string) ut_session->error_string = (char*) malloc (UT_ERROR_STRING_SIZE);
				snprintf (ut_session->error_string, UT_ERROR_STRING_SIZE,
						"syntax error in %s at line %d:\n%s", filename, line, scan);
				return UT_SYNTAX_ERROR;
			}
			if (rcode!=UT_OK) {
				if (!ut_session->error_string) ut_session->error_string = (char*) malloc (UT_ERROR_STRING_SIZE);
				snprintf (ut_session->error_string, UT_ERROR_STRING_SIZE,
						"error %d in %s at line %d", rcode, filename, line);
				//malloc'ed blocs (file_buffer & links) not free'ed
				return UT_CHARSET_FILE_ERROR;
			}
		} //else
		scan++;
	} //while
	
	//put pointers from charmap linked list in an array
	ut_session->charset = (UtCharset*) calloc (ut_session->nb_charsets, sizeof (UtCharset));
	i=0;
	current_link = charmap_list;
	while (current_link) {
		ut_session->charset[i].name			= current_link->charset->name;
		ut_session->charset[i].alias		= current_link->charset->alias;
		ut_session->charset[i].common_name	= current_link->charset->common_name;
		ut_session->charset[i].comment 		= current_link->charset->comment;
		ut_session->charset[i].type 		= current_link->charset->type;
		ut_session->charset[i].unicode		= current_link->charset->unicode;
		ut_session->charset[i].char_type	= current_link->charset->char_type;
		ut_session->charset[i].language		= current_link->charset->language;
		ut_session->charset[i].system		= current_link->charset->system;
		charmap_list = current_link->next;
		free(current_link->charset);
		free(current_link);
		current_link = charmap_list;
		i++;
	}
	free (file_buffer);
	
	DBG2 ("Charset file %s processed!", filename)
	//ut_print_charsets () ;
	return UT_OK;
	
}
