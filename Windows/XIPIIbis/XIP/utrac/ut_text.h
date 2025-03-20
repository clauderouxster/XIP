/***************************************************************************
 *            ut_text.h
 *
 *  Tue Oct  5 11:28:11 2004
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
 * \file ut_text.h
 * \author Antoine Calando (antoine@alliancemca.net)
 */

#ifndef _UT_TEXT_H_
#define _UT_TEXT_H_



/***************************************************************************/
/*!
 * \brief Flags that control the recognition of a text.
 * 
 * They are set by the user to tune the way the text will be analysed
 * (during function ut_recognize() ).
 *  Some of them are unimplemented (UT_F_REFERENCE_EXT_CHAR, always true).
 */

typedef enum UtTextFlags {
	UT_F_UNSET 				 = 0,
	UT_F_FORCE_BINARY		 = 1<<0, //!< Force processing of the file even if it is detected as binary data.
	UT_F_IDENTIFY_EOL 		 = 1<<1, 
	UT_F_TRANSFORM_EOL 		 = 1<<2, //!< Replace EOL by null character to simplify the processing.
	UT_F_REMOVE_ILLEGAL_CHAR = 1<<3, //!< Remove control characters (except CR, LF and TAB).
	UT_F_ADD_FINAL_EOL 		 = 1<<4, //!< Add a final EOL to the text if the last line is not empty.
	UT_F_IDENTIFY_CHARSET	 = 1<<5,
	UT_F_REFERENCE_EXT_CHAR	 = 1<<6, //!< Register the lines that contains extended characters (unimplemented, always true).

	UT_F_DEFAULT = UT_F_REMOVE_ILLEGAL_CHAR | UT_F_IDENTIFY_CHARSET
} UtTextFlags;

/***************************************************************************/
/*!
 * \brief Flags that describe each step in the processing of a text.
 * 
 * They are set by the user or by utrac to select which pass will be done,
 * in ordrer to compute the % of the process done for the 'progress bar'
 * callback.
 *
 */

typedef enum UtPassFlags {
	UT_PF_UNSET 		= 0,
	UT_PF_NONE			= 1<<0,
	UT_PF_LOAD			= 1<<1,
	UT_PF_RECOGNIZE		= 1<<2,
	UT_PF_DISTRIB_PASS	= 1<<3,
	UT_PF_EOL_PASS		= 1<<4,
	UT_PF_XASCII_PASS	= 1<<5,
	UT_PF_CONVERT		= 1<<6,

	UT_PF_MAX			= 1<<6
	//UT_PF_ALL			= UT_PF_LOAD | UT_PF_RECOGNIZE | UT_PF_CONVERT
} UtPassFlags;

	



/***************************************************************************/
/*!
 * \brief Contains evaluation of a charset.
 *
 * An array of this structure is instanciated in UtText and holds the result of
 * the evaluation of each charset. The charset which get the best rating will
 * be choosed for the conversion.
 */

typedef struct UtCharsetEval {
	long rating; 	//!< Mark attributed to the charset depending on the text
	ulong checksum;	//!< Checksum of each extended character in the text. Used to find equivalent charsets.
} UtCharsetEval;


/***************************************************************************/
/*!
 * \brief Refers to a line with extended characters.
 *
 * This structure refers to a line with extended characters.
 * The list of lines with extended characters is filtered to exclude lines
 * with same characters and is stocked in a linked list accessible from UtText.
 */

typedef struct UtExtCharLine {
	char * line_p;		//!< Pointer to the beginning line.
	ulong line_i;		//!< Number of the line.
	ulong nb_ext_chars;	//!< Number of extended characters in the line.
	struct UtExtCharLine * next;	//!< Pointer to the next struture. NULL if last.
} UtExtCharLine;


/***************************************************************************/
/*!
 * \brief Types of End-of-line characters.
 *
 * Different types are CRLF (DOS/Windows), LF (Unix), CR (Mac). The types CRLF_CR and
 * CRLF_LF exists in some CSV databases : entries are ended with CRLF, but some fields
 * may contains LF or CR alone to indicate a "carriage return" in the field.
 * CR is the character 0xD, LF is 0xA.
 *
 * \note EC le cas du LFCR n'est pas pris en compte (cela n'existe pas ?)
 *       AC Si! je ne l'ai pas rencontré, mais il faudrait le rajouter...
 *       (en fait il faudrait même modifier pas mal de trucs dans la reconnaissance
 *       de fins de ligne)
 */

typedef enum UtEolType {
	UT_EOL_UNSET=-1, 
	UT_EOL_CR,
	UT_EOL_LF,
	UT_EOL_CRLF,
	UT_EOL_LFCR,
	UT_EOL_MIX,		//!< Detection only
	UT_EOL_BSN,		//!< \n, conversion only
	UT_EOL_NUL,		//!< ASCII NUL character
//	UT_EOL_SPACE,
//	UT_EOL_TAB,
//	UT_EOL_NOCHANGE,	//!< Conversion only
	UT_EOL_NONE		//always the last
} UtEolType;

extern const char * UT_EOL_NAME [];

typedef short UtCharsetIndex;

/***************************************************************************/
/*!
 * \brief Contains all the information about a text and its processing.
 *
 * This structure is created by ut_init_text() and destroyed by ut_free_text(). It is used
 * to pass different arguments to ut_process_text(), and to stock information about the
 * text all along its processing.
 */

typedef struct UtText {
	char * data; 					//!< Pointer to the beginning of the text. It is finished by a null character. Set by user or Utrac.
	ulong size;						//!< Size of the text, without the terminating null character. Set by user or Utrac.

	UtEolType eol;					//!< EOL type recognized by Utrac.
	UtEolType eol_alt;				//!< EOL type recognized by Utrac.
	UtCharsetIndex charset;			//!< Charset recognized by Utrac.

	ulong nb_lines;					//!< Number of lines in the text. Set by Utrac.
	ulong nb_lines_alt;				//!< Number of alt lines in the text. Set by Utrac.
	ulong * distribution;			//!< Frequency distribution of the text. Set by Utrac.
	UtExtCharLine * ext_char;		//!< Linked list of lines containing extended characters. Set by Utrac.
	UtCharsetEval * evaluation;		//!< Array containg evaluation of each charset. Set by Utrac.

	UtTextFlags flags;				//!< Flags that control the processing of the text. Set by user.
	UtPassFlags pass_flags;
	char skip_char;					//!< Character to skip during conversion. A variable is used rather than the constant
									//!< UT_SKIP_CHAR, since the text can already already contains UT_SKIP_CHAR value if
									//!< UT_F_REMOVE_ILLEGAL_CHAR is not set. Set by user.
	float progress_done;			//!< Part of the process already done. Value included between 0.0 and 1.0. Set by Utrac.
	int progress_todo;				//!< Number of passes to do before end of the process. Set by Utrac.
	UtPassFlags current_pass;		//!< Type of the pass in progress (used in the 'progress bar' callback)
	
	void * user;					//!< Structure for user data. Never touched by utrac, except during initalisation.
} UtText;

#endif //_UT_TEXT_H_
