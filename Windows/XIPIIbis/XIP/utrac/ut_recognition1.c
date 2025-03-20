/***************************************************************************
 *            ut_recognition1.c
 *
 *  Tue Oct  5 11:29:40 2004
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
 * \file ut_recognition1.c
 * \brief Distrib/utf8 pass and EOL pass
 * \author Antoine Calando (antoine@alliancemca.net)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utrac.h"

#undef UT_DEBUG
#define UT_DEBUG 1
#include "debug.h"

/***************************************************************************/
/*!
 * \brief Return false if unicode scalar value is invalid
 */
bool inline ut_unicode_invalid (ulong unicode) {
	return ((   0x0000FDD0 <= unicode && unicode <= 0x0000FDEF  )
		 || (   0x0010FFFE <= unicode                			)
		 || ( ( 0xFFF0FFFE  & unicode ) == 0x0000FFFE			));
}

/***************************************************************************/
/*!
 * \brief Scan the text to calculate frequency distribution and UTF-8 correctness.
 *
 * This function calculate the frequency distribution, i.e. for i between 0 and 255,
 * text->distribution [i] is equal to the number of bytes "i" in the text. This
 * distribution is used to determinate if the file is binary or ASCII. The text is also
 * simultaneously scanned to check for UTF-8 errors.
 *
 * \return UT_OK on success, UT_BINARY_DATA_ERROR if file is binary, error code otherwise.
 */
UtCode ut_distrib_utf_pass (UtText * text) {
	
	char * scan = text->data;
	char * scan_end;
	
	ASSERT(text);
	ASSERT(text->data);
	
	//bug! (see assert l85)
	if (text->size) scan_end = scan + text->size;
	else scan_end = NULL;
	
	ulong unicode = 0;
	ushort multibyte = 0;
	ulong error_utf8 = 0;
	int cumul = 1;
	if (!text->distribution) text->distribution = (ulong*) malloc (sizeof(ulong)*256);
	int i; for (i=0; i<0x100; i++) text->distribution[i] = 0;

	scan--;	//incrementation at the beginning of the loop is faster
	for (;;) {
		scan++;
		//EC: double test de !*scan !! AC ok
		switch (*scan) {
		  case 0:
			if (scan>=scan_end) {
				ASSERT (!scan_end || scan==scan_end)
				goto out_for;
			} else if (!scan_end) goto out_for;
		  case 0xA:
		  case 0xD:
			if (scan - text->data >= UT_PROCESS_STEP*cumul && ut_session->progress_function) {
				if (!ut_update_progress (text, scan - text->data, false)) goto out_for;
				cumul++;
			}
		}
		
		text->distribution [(u_char) *scan]++;
		if (multibyte) {
			if ((*scan & 0xC0) == 0x80) {	//==10xx xxxx
				unicode <<= 6;
				unicode |= *scan & 0x3F;
				if(!--multibyte) { //last multybyte byte? then test if noncharacter (66 cases)
 					if (ut_unicode_invalid (unicode)) error_utf8++;
				}
			} else {
				multibyte = 0;
				error_utf8++;
			}
		} else if (*scan & 0x80) { //1xxx xxx
			if ((*scan & 0xE0) == 0xC0) { //110x xxxx
				multibyte = 1;
				unicode = *scan & 0x1F;
			} else if ((*scan & 0xF0) == 0xE0) { //1110 xxxx
				multibyte = 2;
				unicode = *scan & 0x0F;
			} else if ((*scan & 0xF8) == 0xF0) { //1111 0xxx
				multibyte = 3;
				unicode = *scan & 0x07;
			} else { //error
				error_utf8++;
			}
		}
	} //for (;;)
	out_for:
	//interrupted?
	//EC: ou il y a déja un 0 dans le texte ! AC test déjà fait
	if (scan<scan_end) {
		return UT_INTERRUPTED_BY_USER;
	}
	
	if (multibyte) error_utf8++;
	
	DBG2 ("Distribution and UTF-8 pass done! (%lu B)", text->size)
	
	if (!text->size) text->size = scan - text->data; //terminating 0 not counted
	if (!text->size) return UT_EMPTY_DATA_ERROR;
	
	ulong nb_ctrl_chars = 0;
	// count the number of control chars
	for (i=0; i<0x20; i++) {
		if (i==0x9 || i==0xA || i==0xD) continue;
		nb_ctrl_chars += text->distribution[i];
	}
	nb_ctrl_chars += text->distribution[0x7F];
	
	//test if text is actually binary data
	if (text->size * UT_THRESHOLD_CONTROL_CHAR < nb_ctrl_chars) {
		//to do: detect if UTF16!?!?
		DBG3 ("Binary file detected! (%lu cc)", nb_ctrl_chars)
		return UT_BINARY_DATA_ERROR;
	}
	
	//count the number of extended char
	ulong nb_ext_chars = 0;
	for (i=0x80; i<0x100; i++) {
		nb_ext_chars += text->distribution[i];
	}
	DBG3 ("UTF-8 error : %lu, ext char number : %lu", error_utf8, nb_ext_chars)

	if (text->flags & UT_F_IDENTIFY_CHARSET) {
		if (!nb_ext_chars) {
			//text is ASCII
			for (i=0; i<ut_session->nb_charsets; i++) 
				if (ut_session->charset[i].type == UT_CST_ASCII) break;
			ASSERT_MSG (i!=ut_session->nb_charsets, "ASCII not defined")
			text->charset = i;
			DBG3 ("ASCII Encoding detected!")
		} else if (nb_ext_chars * UT_THRESHOLD_UTF8 > error_utf8) {
			//text is UTF-8
		
			for (i=0; i<ut_session->nb_charsets; i++) 
				if (ut_session->charset[i].type == UT_CST_UTF_8) break;
			ASSERT_MSG (i!=ut_session->nb_charsets, "UTF-8 not defined")
			text->charset = i;
			DBG3 ("UTF-8 Encoding detected!")
		} else {
			text->charset = UT_UNSET;
		}
	}

	return UT_OK;
}


/***************************************************************************/
/*!
 * \brief Change all UT_EOL_CHAR to UT_EOL_ALT_CHAR, from beg to end-1.
 *
 * \note EC pourquoi revenir en arrière ?
 *       AC Si on s'est trompé de type d'eol (un LF a été scanné avant un CRLF par ex)
 */
void ut_change_EOL1toEOL2 (char * beg, char * end) {
	ASSERT (beg<end)
	ASSERT (*end==UT_EOL_CHAR)
	char * scan = beg;
	for(;;) {
		if (*scan==UT_EOL_CHAR) {
			if (scan==end) return;
			*scan=UT_EOL_ALT_CHAR;
		}
		scan++;
	}		
}

/***************************************************************************/
/*
 * \brief Change all UT_EOL_ALT_CHAR to UT_EOL_CHAR, from beg to end-1.
 *
 * \note pour faire de vraie optimisation, on utilise strchr() à la place de
 *       for(;;) {... scan++ }, strchr() est une macro assembleur.
 */
/*
void ut_change_lff2eoe (char * beg, char * end) {
	ASSERT (beg<end)
	ASSERT (*end==UT_EOL_ALT_CHAR)
	char * scan = beg;
	for(;;) {
		if (*scan==UT_EOL_ALT_CHAR) {
			if (scan==end) return;
			*scan=UT_EOL_CHAR;
		}
		scan++;
	}		
}
*/
// \brief exemple de fonction de remplacement pour ut_change_lff_eoe()
/*
void ut_change_lff_eoe_maybe (char * beg, char * end) 
	{
	char * scan; //les variables locales en début de bloc, sinon c'est du C++

	ASSERT (beg!=NULL) //important à tester en debug
	ASSERT (end!=NULL) //important à tester en debug
	ASSERT (beg<end)  
	ASSERT (*end==UT_EOL_ALT_CHAR) //c'est sur que cela doit être en ASSERT ?

	*end = UT_EOL_CHAR; //c'est bien le 0 final ? non ?

	
	 donc ici pas d'appel de fonction ! c'est une directive __asm {}

	 il vaut mieux cependant utiliser memchr, c'est plus sûr (puisque
	 l'on spécifie la taille du buffer), et plus rapide car il utilis
	 REPNE SCASB

   movb AL,octet à rechercher
	 movl EDX,adresse du buffer
	 movl ECX,taille du buffer -1
	 rpne scasb
	 je ...
	 EDX contient l'adresse de l'octet trouvé
	 
	for(scan=beg;
			(scan=strchr(scan,UT_EOL_ALT_CHAR));
			*scan=UT_EOL_CHAR)
			;
	
	Si il peut y avoir des 0 dans le texte avant l'appel de cette fonction, il faut faire
	une double boucle pour avancer d'un octet si scan!=end alors que strchr renvoi NULL
	}
	*/


/***************************************************************************/
/*!
 * \brief Scan the text to detect EOL type and replace EOL by UT_EOL_CHAR or UT_EOL_ALT_CHAR.
 *	
 * EOL are recognized and replaced by UT_EOL_CHAR (null char), and eventually UT_EOL_ALT_CHAR 
 * if EOL type is UT_EOL_CRLF_CR or UT_EOL_CRLF_LF (see UtEolType).
 * ut_session->progress_function() is called only if ( text->flags & UT_F_TRANSFORM_EOL )
 *
 * \return UT_OK on success, error code otherwise.
 */

UtCode ut_eol_pass (UtText * text) {

	char * scan = text->data;
	char * scan_end = text->data+text->size;
	ASSERT ( *scan_end == 0 )
	//ASSERT ( text->flags & UT_F_TRANSFORM_EOL )
	text->nb_lines = 0;
	text->nb_lines_alt = 0;
	ulong cumul=1;
	
	//while (scan < scan_end) {
	
	UtEolType eol1 = UT_EOL_NONE;
	UtEolType eol2 = UT_EOL_NONE;
	

	for (;;) {
		DBG3_S ("<%d>", *scan);
		
		if ((u_char)*scan<0x20) { 			//======== control code =============
			if (!*scan) {						//--------null char
				if (scan>=scan_end) {	
					ASSERT (scan==scan_end)
					break;
				} else if (scan - text->data >= UT_PROCESS_STEP*cumul && ut_session->progress_function) {
					if (!ut_update_progress (text, scan - text->data, false)) break;
					cumul++;
				}				
			}
			if (*scan == 0xA) { 				//-------- LF (+CR?)    -------------
				DBG3_S ("*");
				if (scan - text->data >= UT_PROCESS_STEP*cumul && ut_session->progress_function) {
					ut_update_progress (text, scan - text->data, false);
					cumul++;
				}

				if (*(scan+1) == 0xD) { //LFCR
					switch (eol1) {
					  case UT_EOL_LFCR:
					  case UT_EOL_MIX:
						if (*(scan+2) == 0xA) goto LF_only;
						break;
					  case UT_EOL_CRLF:
						if (*(scan+2) == 0xA) goto LF_only;
						eol1 = UT_EOL_MIX;
						if (eol2 != UT_EOL_NONE) {
							ERROR ("EOL2 todo...")
						}
						break;
					  case UT_EOL_CR:
					  case UT_EOL_LF:
						if (*(scan+2) == 0xA) goto LF_only;
						ASSERT (eol2 == UT_EOL_NONE)
						eol2 = eol1;
						text->nb_lines_alt = text->nb_lines;
					    text->nb_lines = 0;
						*scan = UT_EOL_CHAR;
						ut_change_EOL1toEOL2 (text->data, scan);
					  case UT_EOL_NONE:
						eol1 = UT_EOL_LFCR;
						break;
					  default:
					  	ERROR ("Forbiden case!?!")
					}
					*scan++ = UT_EOL_CHAR;
					*scan++ = text->skip_char;
					text->nb_lines++;
				} else {     //LF only
					LF_only:
					switch (eol1) {
					  case UT_EOL_NONE:
						eol1 = UT_EOL_LF;
					  case UT_EOL_LF:
					  case UT_EOL_MIX:
						*scan++ = UT_EOL_CHAR;
						text->nb_lines++;
						break;
					  case UT_EOL_CR:
						eol1 = UT_EOL_MIX;
						*scan++ = UT_EOL_CHAR;
						text->nb_lines++;
						break;
					  case UT_EOL_CRLF:
					  case UT_EOL_LFCR:
						switch (eol2) {
						  case UT_EOL_NONE:	
							eol2 = UT_EOL_LF;
							break;
						  case UT_EOL_CR:
							eol2 = UT_EOL_MIX;
						  case UT_EOL_LF:
						  case UT_EOL_MIX:
							break;  
						  default:
						  	ERROR ("Forbiden case!?!")
						}
						*scan++ = UT_EOL_ALT_CHAR;
						text->nb_lines_alt++;
						break;
					  default:
						  	ERROR ("Forbiden case!?!")
					} //switch
				} // else LF
			} else if (*scan == 0xD) { 		//--------- CR (LF?)      ------------
				DBG3_S ("*");
				if (scan - text->data >= UT_PROCESS_STEP*cumul && ut_session->progress_function) {
					ut_update_progress (text, scan - text->data, false);
					cumul++;
				}

				if (*(scan+1) == 0xA) { //CRLF
					switch (eol1) {
					  case UT_EOL_CRLF:
					  case UT_EOL_MIX:
						break;
					  case UT_EOL_LFCR:
						eol1 = UT_EOL_MIX;
					  	if (eol2 != UT_EOL_NONE) {
							ERROR ("EOL2 todo...")
						}
						break;
					  case UT_EOL_CR:
					  case UT_EOL_LF:
						ASSERT (eol2 == UT_EOL_NONE)
						eol2 = eol1;
						text->nb_lines_alt = text->nb_lines;
					    text->nb_lines = 0;
						*scan = UT_EOL_CHAR;
						ut_change_EOL1toEOL2 (text->data, scan);
					  case UT_EOL_NONE:
						eol1 = UT_EOL_CRLF;
					    break;
					  default:
					  	ERROR ("Forbiden case!?!")
					}
					*scan++ = UT_EOL_CHAR;
					*scan++ = text->skip_char;
					text->nb_lines++;
				} else {     //CR only
					switch (eol1) {
					  case UT_EOL_NONE:
						eol1 = UT_EOL_CR;
					  case UT_EOL_CR:
					  case UT_EOL_MIX:
						*scan++ = UT_EOL_CHAR;
						text->nb_lines++;
						break;
					  case UT_EOL_LF:
						eol1 = UT_EOL_MIX;
						*scan++ = UT_EOL_CHAR;
						text->nb_lines++;
						break;
					  case UT_EOL_CRLF:
					  case UT_EOL_LFCR:
						switch (eol2) {
						  case UT_EOL_CR:
						  case UT_EOL_MIX:
							break;  
						  case UT_EOL_NONE:	
							eol2 = UT_EOL_CR;
							break;
						  case UT_EOL_LF:
							eol2 = UT_EOL_MIX;
							break;
						  default:
						  	ERROR ("Forbiden case!?!")
						}
						*scan++ = UT_EOL_ALT_CHAR;
						text->nb_lines_alt++;
						break;
					  default:
						  	ERROR ("Forbiden case!?!")
					} //switch
				} // else CR
			} else if (*scan == 0x9 ) { 		//------------- tab ----------
				scan++;
			} else if (text->flags & UT_F_REMOVE_ILLEGAL_CHAR) {
				*scan++ = text->skip_char;
			} //else
			
		} else {					//======== non control code =============
			if (*scan == 0x7F && (text->flags & UT_F_REMOVE_ILLEGAL_CHAR) ) {  //control char del
				*scan++ = text->skip_char;
			} else {
				scan++;
			} //else
		} //else
	} //while

	//interrupted?
	if (scan<scan_end) {
		return UT_INTERRUPTED_BY_USER;
	}
	
	if (text->flags & UT_F_ADD_FINAL_EOL) {
		//add EOE if missinG
		if (   (*(scan-2) != UT_EOL_CHAR || *(scan-1) != text->skip_char)
			&&  *(scan-1) != UT_EOL_CHAR ) {
			if (text->flags & UT_F_TRANSFORM_EOL) {
				*scan = UT_EOL_CHAR;
				text->size++;
			} /* text->flags & UT_F_TRANSFORM_EOL should be true
			else { switch (text->eol) {
			  case UT_EOL_CR:
				*scan = 0xD;
				text->size++;
			    break;
			  case UT_EOL_LF:
				*scan = 0xA;
				text->size++;
			  case UT_EOL_LF:
				*scan++ = 0xD;
				*scan   = 0xA;
				text->size+=2;
			} } //else switch
			*/
			text->nb_lines++;
		} // if *scan
	} //if text->flags
	
	if (text->eol == UT_EOL_UNSET) {
		text->eol = eol1;
		text->eol_alt = eol2;
	} else {
		text->nb_lines = UT_UNSET;
		text->nb_lines_alt = UT_UNSET;
	}
	
	//verify EOF
	ASSERT (*scan == UT_EOF_CHAR)

	DBG2 ("End Of Line pass done! (%lu B)", text->size)

	return UT_OK;
}

// ************* Check for UTF16 - big endian & little endian *********
/*
{
	ulong error_utf16 = 0 ; //, error_utf16be = 0, error_utf16le = 0;
	ushort * scanw;
	ushort * scanw_end;
	
	if ( ifd->data_size%2) {
		error_utf16 = -1U;
	} else {
		scanw = (ushort *) ifd->data;
		scanw_end = scanw+ifd->data_size/2;
		for (;;) {
			if (!*scanw && scanw==scanw_end) break;
			if (0xD800 <=*scanw && *scanw < 0xDC00) { //surrogate?
				unicode = (*scanw & 0x3FF) + 0x400;
				scanw++;
				if (!(0xDC00 <= *scanw && *scanw < 0xE000 )) {
					error_utf16++;
					if (scanw==scanw_end) break;
				}
				unicode <<= 10;
				unicode |= *scanw & 0x3FF;
			} else {
				unicode = *scanw;	
			}
			if (   ( 0xFDD0 <= unicode && unicode <= 0xFDEF )
				|| ( (unicode & 0xFFF0FFFE) == 0x0000FFFE)
				|| ( unicode >= 0x0010FFFE)
				|| ( 0xD800 <=unicode && unicode < 0xE000) ) {
				error_utf16++;
			}
			scanw++;
		} //for (;;)
		printf ("UTF16 : %lu errors\n", error_utf16);
	} //else
	

	//ulong error_utf32be = 0, error_utf32le = 0;
} */
