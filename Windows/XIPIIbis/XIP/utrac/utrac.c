/***************************************************************************
 *            utrac.c
 *
 *  Tue Oct  5 11:29:59 2004
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
 * \file utrac.c
 * \author Antoine Calando (antoine@alliancemca.net)
 * \brief Public API for using Utrac.
 */
 
#define _UTRAC_C_

#include <stdlib.h>
#include <stdio.h>
#include "utrac.h"

#undef UT_DEBUG
#define UT_DEBUG 3
#include "debug.h"


/***************************************************************************/
/*!
 * \brief Initialize the Utrac library.
 *
 * This function must be called before any other Utrac function. It allocates an UtSession struture
 * that is accessible by the ut_session pointer, initalizes it, loads charsets data, and sets
 * default	language, charset and end of line type. The memory used is about 630kb for 47 charsets
 * loaded.
 *	
 * \note On Unix systems, LANG, LC_ALL and LC_TYPE are parsed to find default language and
 * charsets (ISO-8859-1 is used else), and	eol type is set to LF.
 *
 * \return UT_OK on success, an error code on failure.
 */
UtCode ut_init () {
	
	if (ut_session) return UT_ALREADY_INITIALISED_ERROR;
		
	ut_session = (UtSession*) malloc (sizeof(UtSession));
	if (!ut_session) return UT_MALLOC_ERROR;
		
	return ut_init_noalloc();
}

/*!
 * \brief Initialize the Utrac library, without allocating memory for UtSession. Used internally
 */
UtCode ut_init_noalloc () {
	//ut_session->flags = UT_F_UNSET; //flags_in
	ut_session->charset = NULL;
	ut_session->nb_charsets = 0;
	ut_session->language.name = NULL;
	ut_session->language.code = NULL;
	ut_session->language.n = 0;
	ut_session->language.n_max = 0;
	ut_session->system.name = NULL;
	ut_session->system.code = NULL;
	ut_session->system.n = 0;
	ut_session->system.n_max = 0;
	//ut_session->charset_default = UT_UNSET;
	ut_session->eol_default = UT_EOL_UNSET;
	ut_session->eol_alt_default = UT_EOL_UNSET;
	
	ut_session->nomapping_char = '_';
	ut_session->progress_function = NULL;
	ut_session->error_string = NULL;
	//load charsets data
	UT_TRY (ut_load_charsets ())

	//find default language, charset, eol type on the system
	#ifdef linux
	//should we use nl_langinfo()? (discovered later...) ->yes!
	int i;
	ut_session->language_default = 0; //language_default_in
	ut_session->system_default = 3; 		//3 (to check in file charsets.dat)
	ut_session->eol_default = UT_EOL_LF;
	ut_session->eol_alt_default = UT_EOL_LF;
	ut_session->charset_default = ut_find_charset("ISO-8859-1");
	
	char * def_enc = getenv ("LC_CTYPE");
	if (!def_enc) def_enc = getenv ("LC_ALL");
	if (!def_enc) def_enc = getenv ("LANG");
	if (def_enc) {
		if (def_enc[2]=='_' || def_enc[2]=='.' || def_enc[2]==0) {
			for (i=0; i<ut_session->language.n; i++) {
				if (def_enc[0]-'a'+'A'== ut_session->language.code[i*2+0]
					&& def_enc[1]-'a'+'A'== ut_session->language.code[i*2+1] ) {
					ut_session->language_default = i;
					break;
				}
			} //for
		}
		if (def_enc[2]=='.') def_enc +=3;
		if (def_enc[2]=='_' && def_enc[5]=='.') def_enc +=6;
		for (i=0; i<ut_session->nb_charsets; i++) 
			if (ut_str_fuzzy_cmp (def_enc, ut_session->charset[i].name,'@')) break;
		if (i!=ut_session->nb_charsets) ut_session->charset_default = i;
	}
	
	if (ut_session->charset_default == UT_UNSET) {
		for (i=0; i<ut_session->nb_charsets; i++) 
			if (ut_str_fuzzy_cmp (UT_DEFAULT_ENCODING_UNIX, ut_session->charset[i].name,0)) break;
		if (i==ut_session->nb_charsets) {
			DBG1 ("*** No default charset ***")
		}
		else ut_session->charset_default = i;
	}
	#else
	ERROR ("pas unix!")
	#endif

	#if UT_DEBUG == 2
	if (ut_session->language_default != UT_UNSET)
		DBG2 ("lang: %s" , ut_session->language.name[ut_session->language_default])
	if (ut_session->charset_default != UT_UNSET)
		DBG2 ("charset: %s", ut_session->charset[ut_session->charset_default].name)
	if (ut_session->eol_default != UT_EOL_UNSET)
		DBG2 ("eol: %s", UT_EOL_NAME [ut_session->eol_default])
	#endif
	
	return UT_OK;
}

/*!
 * \brief Free ressources allocated during initialization of Utrac.
 *
 * This function frees the structure allocated in ut_session
 * by ut_init(). It must be the last Utrac function called.
 *
 * \return UT_OK on success, an error code on failure.
 */

void ut_finish () {
	
	ut_finish_nofree ();
	free(ut_session);
	ut_session = NULL;
	
	return;
}

/*!
 * \brief Free ressources allocated during initialization of Utrac, without freeing UtSession. Used internally.
 */
void ut_finish_nofree () {
	
	if (!ut_session) return;
	
	int i; for(i=0; i<ut_session->nb_charsets; i++) {
		free(ut_session->charset[i].name);
		free(ut_session->charset[i].alias);
		free(ut_session->charset[i].common_name);
		free(ut_session->charset[i].comment);
		free(ut_session->charset[i].unicode);
		free(ut_session->charset[i].char_type);
		free(ut_session->charset[i].language);
		free(ut_session->charset[i].system);
	}
	free (ut_session->charset);
	
	for (i=0; i<ut_session->language.n; i++) 
		free (ut_session->language.name[i]);
	free (ut_session->language.name);
	free (ut_session->language.code);

	for (i=0; i<ut_session->system.n; i++) 
		free (ut_session->system.name[i]);
	free (ut_session->system.name);
	free (ut_session->system.code);
	
	free (ut_session->error_string);
	return;
};




/***************************************************************************/
/*!
 * \brief Allocates and initalizes an UtText structure.
 *
 * \return A pointer to the allocated structure, or NULL if the allocation failed.
 */

UtText * ut_init_text_heap () {
	ASSERT (ut_session)
	UtText* new_text = (UtText*) malloc (sizeof(UtText));
	if (!new_text) return NULL;
	
	ut_init_text (new_text);
	
	return new_text;
}

/*!
 * \brief Initalizes an UtText structure.
 * \param new_text A pointer on the structure to initialize
 */

void ut_init_text (UtText * new_text) {
		
	new_text->data = NULL;
	new_text->size = 0;

	new_text->eol = UT_EOL_UNSET;
	new_text->eol_alt = UT_EOL_UNSET;
	new_text->charset = UT_UNSET;

	new_text->nb_lines = UT_UNSET;
	new_text->nb_lines_alt = UT_UNSET;
	new_text->distribution = NULL;
	//int i; for (i=0; i<0x100; i++) new_text->distribution [i] = 0;
	new_text->ext_char = NULL;
	new_text->evaluation = NULL;

	new_text->flags = UT_F_DEFAULT;
	new_text->pass_flags = UT_PF_UNSET;
	new_text->skip_char = UT_SKIP_CHAR;
	
	new_text->progress_done = 0.0;
	new_text->progress_todo = 0;
	new_text->current_pass = UT_PF_UNSET;

	new_text->user = NULL;
}

/*!
 * \brief Free an UtText structure.
 * \param text pointer to the structure to free.
 */

void ut_free_text_heap (UtText *text) {
	
	ut_free_text (text);
	free(text);	
	
}

/*!
 * \brief Free the contents of an UtText structure, without freeing the structure itself.
 * \param text pointer to the structure to free.
 */

void ut_free_text (UtText *text) {
	//free(text->filename);
	//filename is not freed because it is set by user. 
	free(text->data); text->data = NULL;
	free(text->distribution); text->distribution = NULL;
	while (text->ext_char) {
		UtExtCharLine * tmp = text->ext_char;
		text->ext_char = text->ext_char->next;
		free (tmp);
	} text->ext_char = NULL;

	free(text->evaluation); text->evaluation = NULL;
	//text->user should be free by the user.
}


/*!
 * \brief Initialize an UtText structure before using the 'progress bar' callback feature
 *
 * Can be used internaly or by the user. The UtText must have member UtText::pass_flag set, or
 * at least UtText::flags (if UtText::pass_flags is unset, it will be set for just a recognition
 * pass and subpasses will be selected upon the value of UtText::flags).
*/

UtCode ut_init_progress (UtText *text) {
	
	ASSERT (text);
	
	text->progress_done = 0.0;
	text->progress_todo = 0;
	if (text->pass_flags == UT_PF_UNSET) text->pass_flags = UT_PF_RECOGNIZE;

	if (text->pass_flags & UT_PF_LOAD ) text->progress_todo++;
		
	if (text->pass_flags & UT_PF_RECOGNIZE ) {
		if ((text->flags & UT_F_IDENTIFY_CHARSET) || (text->pass_flags & UT_PF_CONVERT ) )
			text->pass_flags |= UT_PF_DISTRIB_PASS;
		else text->pass_flags &= ~UT_PF_DISTRIB_PASS;
		if (text->flags & (UT_F_TRANSFORM_EOL | UT_F_REMOVE_ILLEGAL_CHAR | UT_F_ADD_FINAL_EOL | UT_F_IDENTIFY_EOL ) )
			text->pass_flags |= UT_PF_EOL_PASS;
		else text->pass_flags &= ~UT_PF_EOL_PASS;

		if (text->flags & (UT_F_IDENTIFY_CHARSET | UT_F_REFERENCE_EXT_CHAR ) )
			text->pass_flags |= UT_PF_XASCII_PASS;
		else text->pass_flags &= ~UT_PF_XASCII_PASS;
		
		if (text->pass_flags & UT_PF_DISTRIB_PASS) text->progress_todo++;
		if (text->pass_flags & UT_PF_EOL_PASS) text->progress_todo++;
		if (text->pass_flags & UT_PF_XASCII_PASS) text->progress_todo++;
	} else {
		text->pass_flags &= ~(UT_PF_DISTRIB_PASS | UT_PF_EOL_PASS | UT_PF_XASCII_PASS);
	}
		
	if (text->pass_flags & UT_PF_CONVERT ) text->progress_todo++;
	
	return UT_OK;
}

/*! \brief Load a file in an UtText structure
 *
 * If filename is null, it will read stdin. text->data and text->size will be set.
 * If ut_session->progress_function is set, it will be called during loading and members of
 * text dealing with this feature will be updated.
 */

UtCode ut_load (UtText *text, const char * filename) {

	ASSERT (text);

	if (text->pass_flags==UT_PF_UNSET) {
		text->pass_flags |= UT_PF_LOAD | UT_PF_RECOGNIZE;
		ut_init_progress(text);
	}
	
	if (ut_session->progress_function && text->progress_done == 0.0) ut_update_progress (text, 0, true);

	text->current_pass = UT_PF_LOAD;

	if (filename) {
		UT_TRY ( ut_load_file_pass (text, filename) )
	} else {
		UT_TRY ( ut_load_stdin_pass (text) )
	}

	text->current_pass = UT_PF_NONE;
	
	if (ut_session->progress_function) {
		text->progress_done+= (1-text->progress_done)/text->progress_todo;
		text->progress_todo--;
	}
	
	//if (ut_session->progress_function && text->progress_done == 0.0) ut_update_progress (text, 0, true);
	if (ut_session->progress_function && text->progress_todo == 0) ut_update_progress (text, 0, true);
		
	return UT_OK;	
}


/*! \brief Recognize charset and EOL of a text.
 *
 * text->data must be set. If text->size is null, recognition will stop at the first
 * null character. text->flags must also be set to select processes to do (see UtTextFlags).
 * 
 * If ut_session->progress_function is set, it will be called during loading and members of
 * text dealing with this feature will be updated.
 *
 * If UT_F_FORCE_BINARY is set, texts with caracters between 0 and 0x19 (space is 0x20, and TAB, CR, 
 * LF are excluded of this range) won't produce error.
  *
 *
 * If UT_F_IDENTIFY_EOL is set, text->eol, text->eol_alt, text->nb_lines, text->nb_lines_alt will be
 * updated. If convertion of EOL is planned, UT_F_TRANSFORM_EOL must be set.
 *
 * If UT_F_IDENTIFY_CHARSET is set, text->charset will be updated. text->evaluation also if charset
 * is 8bits and ASCII -erivated.

 * text->distribution will always be set, text->ext_char also (but this is a bug!)
 *
 * If ut_session->progress_function is set, it will be called during loading and members of
 * text dealing with this feature will be updated. 
*/

UtCode ut_recognize (UtText *text) {
	
	if (!text || !text->data) return UT_BAD_PARAMETER_ERROR;

	if (text->pass_flags==UT_PF_UNSET) ut_init_progress(text);

	if (ut_session->progress_function && text->progress_done == 0.0) ut_update_progress (text, 0, true);
	
	//FIRST PASS
	if (text->pass_flags & UT_PF_DISTRIB_PASS) {
		text->current_pass = UT_PF_DISTRIB_PASS | UT_PF_RECOGNIZE;
		int rcode = ut_distrib_utf_pass (text);
		text->current_pass = UT_PF_NONE;

		if (rcode == UT_BINARY_DATA_ERROR) {
			if ( !(text->flags & UT_F_FORCE_BINARY)) return rcode;
		} else if ( rcode != UT_OK) return rcode;
		
		if (text->charset != UT_UNSET && text->pass_flags & UT_PF_XASCII_PASS) {
			text->pass_flags &= ~UT_PF_XASCII_PASS | UT_PF_RECOGNIZE;
			text->progress_todo--;			
		}
	
		if (ut_session->progress_function) {
			text->progress_done+= (1-text->progress_done)/text->progress_todo;
			text->progress_todo--;
		}
	}

	// set text->skip_char
	if (text->flags & UT_F_REMOVE_ILLEGAL_CHAR ) {
		text->skip_char = UT_SKIP_CHAR;
	} else {
		//if control code accepted in file, try to find one not used
		int i; for (i=1; i<0x20; i++) {
			if (i==UT_EOL_ALT_CHAR || i== 0x9|| i==0xA || i==0xD) continue;	//UT_EOL_CHAR and UT_EOF_CHAR = 0
			if (!text->distribution[i]) break;
		}
		if (i!=0x20) text->skip_char = i; 
		else text->skip_char = UT_SKIP_CHAR; //all control code used, nevermind, we use UT_SKIP_CHAR
	}

	//ASSERT (text->flags & UT_F_TRANSFORM_EOL)
	
	//SECOND PASS
	if (text->pass_flags & UT_PF_EOL_PASS) {
		text->current_pass = UT_PF_EOL_PASS | UT_PF_RECOGNIZE;
		UT_TRY ( ut_eol_pass (text) )
		text->current_pass = UT_PF_NONE;
		if (ut_session->progress_function) {
			text->progress_done+= (1-text->progress_done)/text->progress_todo;
			text->progress_todo--;
		}
	}

	//THIRD PASS
	if ( text->pass_flags & UT_PF_XASCII_PASS ) {
		text->current_pass = UT_PF_XASCII_PASS | UT_PF_RECOGNIZE;
		UT_TRY ( ut_xascii_pass (text) )
		text->current_pass = UT_PF_NONE;
		if (ut_session->progress_function) {
			text->progress_done+= (1-text->progress_done)/text->progress_todo;
			text->progress_todo--;
		}
	}

	if (ut_session->progress_function && text->progress_todo == 0) ut_update_progress (text, 0, true);
	
	return UT_OK;	
}


/*!
 * \brief Convert a text.
 *
 * \param src_text source text, with input eol and charset set.
 * \param dst_text destination text, with output eol and charset set. If it is null, src_text will be replaced
 * by the destination text, and output eol and charset will be selectionned from  ut_session.
 *
 * If ut_session->progress_function is set, it will be called during loading and members of
 * text dealing with this feature will be updated. 
 */

UtCode ut_convert (UtText *src_text, UtText *dst_text) {
	
	if (!src_text || !src_text->data) return UT_BAD_PARAMETER_ERROR;

	ASSERT (src_text->eol != UT_EOL_UNSET)
	ASSERT (src_text->charset != UT_UNSET)
	ASSERT (src_text->distribution)

	bool same_text = false;
	if (!dst_text) {
		same_text = true;
		dst_text = ut_init_text_heap ();
		if (!dst_text) return UT_MALLOC_ERROR;
	}
	
	ASSERT (dst_text)
	
	if (src_text->pass_flags==UT_PF_UNSET) {
		src_text->pass_flags |= UT_PF_CONVERT;
		ut_init_progress(src_text);
	}

	
	if (ut_session->progress_function && src_text->progress_done == 0.0) ut_update_progress (src_text, 0, true);
	
	if (dst_text->eol	  == UT_EOL_UNSET)  dst_text->eol 		= ut_session->eol_default;
	if (dst_text->eol_alt == UT_EOL_UNSET)  dst_text->eol_alt 	= ut_session->eol_alt_default;
	if (dst_text->charset == UT_UNSET) 		dst_text->charset	= ut_session->charset_default;

	src_text->current_pass = UT_PF_CONVERT;
	UT_TRY	( ut_conversion_pass (src_text, dst_text) )
	src_text->current_pass = UT_PF_NONE;

	if (ut_session->progress_function) {
		src_text->progress_done+= (1-src_text->progress_done)/src_text->progress_todo;
		src_text->progress_todo--;
	}
	
	if (ut_session->progress_function && src_text->progress_todo == 0) ut_update_progress (src_text, 0, true);
		
	if (same_text) {
		free (src_text->data);
		src_text->data = dst_text->data;
		dst_text->data = NULL;
		src_text->size = dst_text->size ;
		src_text->eol = dst_text->eol ;
		src_text->eol_alt = dst_text->eol_alt ;
		src_text->charset = dst_text->charset ;
		free (src_text->distribution);
		src_text->distribution = NULL;
		while (src_text->ext_char) {
			UtExtCharLine * tmp = src_text->ext_char;
			src_text->ext_char = src_text->ext_char->next;
			free (tmp);
		} src_text->ext_char = NULL;
		free(src_text->evaluation); 
		src_text->evaluation = NULL;
		ut_free_text_heap (dst_text);
	}

	return UT_OK;	
}




/***************************************************************************/
/* OLD DOC!!!!
 * \brief Recognize charset and EOL type of a text, and eventually convert it.
 * 
 * This function take an UtText structure as a parameter and do severeal tasks :
 * -# it loads the file (or read the standard input),
 * -# it calculate the frequency distribution of each byte in the file
 *    (UtText::distribution), checks if the file is binary data or text,
 *    checks if it is ASCII or UTF-8,
 * -# it recognize the EOL type, and replace each EOL by null character to make
 *    further processing of the file easier (this feature can be disbled).
 * -# if the charset has not been determined earlier as ASCII or UTF-8, it tries
 *    to detect which known charset fit the best to the text.
 * -# it eventually convert the text, replacing EOL and extended character by
 *    those corresponding to the selection of the user and/or the result of the recogntion.
 *
 * \param text Text to recognize and eventually convert. Some members must be set
 *        before calling this function, but some other are optionnal. Members that
 *        select the input text are:
 * 	      - UtText::data: Pointer to the text to process (which must be null terminated).
 *          If NULL, UtText::filename is used.
 *        - UtText::filename: Path to the file containing the text to process, which will
 *          be loaded if . If NULL, standard input is read.
 * 	      - UtText::size: If UtText::data is set, this member can also be set to indicate
 *          the size of the text, if null, the first null character will determine the
 *          end of the text.
 * 
 * Members that modifies the recognition or the conversion are:
 * 	- UtText::flags: Flags to customize the processing and the modification of the text.
 *    Set intially to UT_F_DEFAULT.
 * 	- UtText::src_eol and UtText::src_charset: EOL type and charset of the text used as
 *    source for the conversion. If unset, the values taken are those recognized automatically.
 * 	- UtText::dst_eol and UtText::dst_charset: EOL type and charset of the text resulting of
 *    the conversion. If unset, the values taken are those by default found by ut_init().
 * 	- UtText::nomapping_char: Character inserted during the conversion each time an error occurs.
 * 
 * Misc member:
 * 	- UtText::progress_function: Custom function provided by the user to refresh a progress bar.
 *
 * \param convert If true, conversion is effectued after recognition.
 *	
 * \return UT_OK on success, error code on failure (see UtCode).
 */
