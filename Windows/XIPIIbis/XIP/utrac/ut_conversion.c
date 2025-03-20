/***************************************************************************
 *            ut_conversion.c
 *
 *  Wed May 26 11:57:43 2004
 *  Copyright  2004 Alliance MCA
 *  Author Antoine Calando (antoine@alliancemca.net)
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

/*! \file
 *	\brief Conversion functions from one charset to another.
 *
 *	\todo EC les fonction inline be fonctionne qu'avec gcc !! il faudrait mettre
 *        une macro UT_INLINE dans un header.	
 *        AC "inline" est pourtant posix, non?
 *  \todo ASSERT sur tous les parametres.
 *
 */
 
#include <stdlib.h>
#include <stdio.h>
#include "utrac.h"

//#undef UT_DEBUG
//#define UT_DEBUG 3
#include "debug.h"

//! \brief Test if c is an extended character.
static inline bool is_ext (char c) {
	return (u_char) c >= 0x80;	
}

/**************************************************************************/
/*!
 * \brief Return the size in byte of an Unicode character in UTF-8.
 *
 * - UTF-8 on 1 byte:  0000 0000  0xxx xxxx => 0xxx xxxx
 * - UTF-8 on 2 bytes: 0000 0yyy  yyxx xxxx => 110y yyyy  10xx xxxx
 * - UTF-8 on 3 bytes: zzzz yyyy  yyxx xxxx => 1110 zzzz  10yy yyyy  10xx xxxx
 * - UTF-8 on 4 bytes: 00uu uuzz  zzzz yyyy  yyxx xxxx => 1111 0uuu 10zz zzzz  10yy yyyy  10xx xxxx
 *
 * \param unicode The scalar value of the Unicode character.
 *
 * \return Size in byte.
 *
 * \bug EC Il n'y a pas de gestion de l'ordre des octets dans le long, ceci ne fonctionne donc
 *      pas sous mac (à moins que cela soit fait en amont ?).
 *      AC On teste ici la valeur scalaire, c'est à dire un long et rien d'autre, donc pas de prb d'endian.
 *
 * \bug EC Que ce passe-t-il si unicode>0x10FFFF ? La fonction renvoi 0... si la fonction
 *      appelant ne le test pas, on obtiendra une boucle sans fin. Il vaut mieux un programme
 *      qui plante qu'un programme qui freeze, la valeur 0 n'est donc pas indiquée.
 *      AC Pas de freeze possible! Par contre les carctères illégaux peuvent être remplacé
 *      par un carctère au choix de l'utilisateur, il faudrait donc renvoyer la taille de
 *		ce caractère d'erreur, afin qu'un éventuel malloc ne soit pas trop court. Mais il
 *      faudrait peut être plutôt gérer ça dans les autres fonctions pour ne pas avoir à
 *      rajouter une structure UtText en argument.
 *
 * \note EC Ce code étant extraimement simple et concis, il serait surement interressant de
 *       le mettre en inline. De plus si ce code est beaucoup utilisé, on peut faire
 *       quelques optimisations (test < == > sur 0xFFFF si le premier test échoue permettrait
 *       1 à 2 tests à la place de 1 à 4 tests par exemple).
 *       AC Pour l'inline, oui, mais la fonction n'est pas appelé si souvent que ça.
 *       Pour l'organisation des tests, les cas les plus fréquents (pour du latin) sont
 *       le cas 1, puis le 2, puis le 3 etc... donc tests bien ordonnés.
 */

int ut_size_unicode (ulong unicode) {
	if ( !(unicode & ~0x7F)) {
		//UTF-8 on 1 byte: 0000 0000  0xxx xxxx => 0xxx xxxx
		return 1;
	} else if ( !(unicode & ~0x7FF)) {
		//UTF-8 on 2 bytes: 0000 0yyy  yyxx xxxx => 110y yyyy  10xx xxxx
		return 2;
	} else if ( !(unicode & ~0xFFFF)) {
		//UTF-8 on 3 bytes: zzzz yyyy  yyxx xxxx => 1110 zzzz  10yy yyyy  10xx xxxx
		return 3;
	} else if (unicode <= 0x10FFFF ) {
		//UTF-8 on 4 bytes: 00uu uuzz  zzzz yyyy  yyxx xxxx => 1111 0uuu 10zz zzzz  10yy yyyy  10xx xxxx
		return 4;
	} else {
		return 0;
	}
}

/*!
 * \brief Convert an UTF-8 character to Unicode scalar value.
 *
 * \param src_p address of the pointer on the beginning of the character. This pointer
 *        is incremented to the beginning of the following character after conversion.
 *
 * \return Unicode scalar value of the converted character.
 *
 * \bug EC Dans le cas d'un caractère invalide, si c'est le premier il est comptabilisé, mais
 *      si ce n'est pas le premier, il ne l'est pas. C'est vraiment ce que l'on veut ?
 *      AC ??? 	Si il est compatibilisé! (voir "while (size--) {...}")

 */

ulong ut_utf8c_to_unicode (char ** src_p) {
	
	ulong unicode;
	int size;

	if (! (**src_p&0x80)) { //==0xxx xxxx (d=done,x=don't care)
		unicode = **src_p; (*src_p)++;
		return unicode;
	} else if (! (**src_p&0x40)) { //==d0xx xxxx error!
		(*src_p)++;
		return UT_UNICODE_NONCHAR;
	} else if (! (**src_p&0x20)) { //==dd0x xxxx
		size = 1;
		unicode = **src_p & 0x1F;
	} else if (! (**src_p&0x10)) { //==ddd0 xxxx
		size = 2;
		unicode = **src_p & 0x0F;
	} else if (! (**src_p&0x08)) { //==dddd 0xxx
		size = 3;
		unicode = **src_p & 0x07;
	} else {
		#if UT_DEBUG > 1
		printf("<%X:%x:%x:", **src_p & 0xFF, (**src_p|0x20), (**src_p|0x10));
		ut_print_binary (**src_p & 0xFF);
		putchar('>');
		#endif
		(*src_p)++;
		return UT_UNICODE_NONCHAR;
	}
	(*src_p)++;
	
	while (size--) {
		if ((**src_p&0xC0) != 0x80) return UT_UNICODE_NONCHAR; //!=10xx xxxx
		unicode<<=6;
		unicode |= **src_p & 0x3F;
		(*src_p)++;
	}
	return unicode;
}

/*!
 * \brief Convert an Unicode scalar value to UTF-8 character.
 * 
 * \param dst_p address of the pointer on the buffer where the character is going to
 *        be written. This pointer is incremented to the end of the character + 1
 *        after conversion.
 * \param unicode Unicode scalar value of the character to convert.
 *
 * \bug EC il y a des cas d'erreur, donc il faut un retour, si l'on tombe dans le cas d'erreur
 *      vu qu'il n'y a plus d'incrémentation de dst_p on risque une boucle sans fin.
 *      AC Pas de boucle sans fin, et erreur peu grave.
 *
 * \note EC une fonction inline serait peut être la bien venue.
 *       AC vrai, il faudrait mettre la fonction dans un include...
 */

void ut_unicode_to_utf8c (ulong unicode, char ** dst_p) {
	
	if ( !(unicode & ~0x7F)) {
		//UTF-8 on 1 byte: 0000 0000  0xxx xxxx => 0xxx xxxx
		*(*dst_p)++ = (char) unicode;
	} else if ( !(unicode & ~0x7FF)) {
		//UTF-8 on 2 bytes: 0000 0yyy  yyxx xxxx => 110y yyyy  10xx xxxx
		*(*dst_p)++ = ((char) (unicode>>6) & 0x1F) | 0xC0; //=> 110y yyyy
		*(*dst_p)++ = ((char) unicode & 0x3F) | 0x80; //=> 10xx xxxx
	} else if ( !(unicode & ~0xFFFF)) {
		//UTF-8 on 3 bytes: zzzz yyyy  yyxx xxxx => 1110 zzzz  10yy yyyy  10xx xxxx
		*(*dst_p)++ = ((char) (unicode>>12) & 0x0F) | 0xE0; //=> 1110 zzzz
		*(*dst_p)++ = ((char) (unicode>>6) & 0x3F) | 0x80; //=> 10yy yyyy
		*(*dst_p)++ = ((char) unicode & 0x3F) | 0x80; //=> 10xx xxxx
	} else {
		ERROR ("*** UTF8 CHAR ON 4 BYTES!!!***");
	}
}

/*!
 * \brief Return size in byte of a character after conversion.
 *
 * \param text UtText structure containing the source charset and the destination charset.
 * \param src_p Address of the pointer on the beginning of the character encoded with the
 *        source charset . This pointer will be incremented to the beginning of the
 *        following character.
 *
 * \return The size of the character when it will be encoded with the destination charset.
 *
 * \warning Il n'y a pas de return final !
 * \bug AC voir ut_size_unicode() quand retour = 0
 */

int ut_size_char (char **src_p, UtCharsetIndex src_charset, UtCharsetIndex dst_charset) {

	ASSERT (*src_p)
	ASSERT (src_charset != UT_UNSET)
	if (dst_charset == UT_UNSET) dst_charset = ut_session->charset_default;
			
	ulong unicode;
	UtCharset * src_cs = &(ut_session->charset [src_charset]);
	UtCharset * dst_cs = &(ut_session->charset [dst_charset]);
	
	if (src_cs->type == UT_CST_ASCII || dst_cs->type == UT_CST_ASCII) {
		if (src_cs->type == UT_CST_UTF_8) ut_utf8c_to_unicode(src_p);
		else (*src_p)++;
		if (ut_session->nomapping_char<0x80) return 1;
		else return 0;
	} else if (src_cs->type == UT_CST_ASCII_EXTENSION) {
		unicode = src_cs->unicode [(u_char) **src_p];
		(*src_p)++;
	} else if (src_cs->type == UT_CST_UTF_8) { 
		unicode = ut_utf8c_to_unicode (src_p); 
	} else { 
		ERROR ("charset type not managed : %d", src_cs->type)
	}

	if (unicode==UT_UNICODE_NONCHAR) unicode = ut_session->nomapping_char;
	
	if (dst_cs->type == UT_CST_UTF_8) {
		return ut_size_unicode (unicode);
	} else if (dst_cs->type == UT_CST_ASCII_EXTENSION) {
		return 1; //often 1 and seldom 0... so let's answer quickly
		/*if (unicode<0x80) return 1;
		else {
			int i; for (i=0x80; i<0x100; i++) 	if (unicode==dst_cs->unicode[i]) break;
			if(i!=0x100) return 1
				&& ut_session->nomapping_char >= 0x100) return 0;
			else return 1;
		}*/
	} else { ERROR ("charset type not managed : %d", src_cs->type) }
	
}

/*!
 * \brief Convert a character.
 *
 * \param text UtText structure containing the source charset and the destination charset.
 * \param src_p Address of the pointer on the beginning of the character encoded with the
 *        source charset . This pointer will be incremented to the beginning of the
 *        following character.
 * \param dst_p address of the pointer on the buffer where the converted character will be
 *        written. This pointer is incremented to the end of the character + 1 after conversion.
 *
 * \todo EC Il y a des cas d'erreur (ERROR), on doit donc pourvoir retourner l'erreur.
 *       AC Les erreurs sont gérées : elles sont indiquées par un 'ut_session->nomapping_char' dans le
 *       texte.
 */

void ut_conv_char (char ** src_p, char ** dst_p, UtCharsetIndex src_charset, UtCharsetIndex dst_charset) {
	ASSERT (*src_p) 	
	ASSERT (*dst_p) 	
	ASSERT (src_charset != UT_UNSET)
	if (dst_charset == UT_UNSET) dst_charset = ut_session->charset_default;

	ulong unicode;
	UtCharset * src_cs = &(ut_session->charset [src_charset]);
	UtCharset * dst_cs = &(ut_session->charset [dst_charset]);
	
	if (src_cs->type == UT_CST_ASCII || dst_cs->type == UT_CST_ASCII) {
		if (src_cs->type == UT_CST_UTF_8) ut_utf8c_to_unicode(src_p);
		else (*src_p)++;
		if (ut_session->nomapping_char<0x80) *(*dst_p)++ = (char) ut_session->nomapping_char;
		return;
	} else if (src_cs->type == UT_CST_ASCII_EXTENSION) {
		unicode = src_cs->unicode [(u_char) **src_p];
		(*src_p)++;
	} else if (src_cs->type == UT_CST_UTF_8) {
		unicode = ut_utf8c_to_unicode (src_p);
	} else {ERROR ("charset type not managed : %d", src_cs->type) }

	if (unicode!=UT_UNICODE_NONCHAR) {
		if (dst_cs->type == UT_CST_UTF_8) {
			if (unicode==UT_UNICODE_NONCHAR) unicode = ut_session->nomapping_char;
			ut_unicode_to_utf8c (unicode, dst_p);
		} else if (dst_cs->type == UT_CST_ASCII_EXTENSION) {
			if (unicode<0x80) {
				*(*dst_p)++ = (char) unicode;
			} else {
				int i; for (i=0x80; i<0x100; i++) 	if (unicode==dst_cs->unicode[i]) break;
				if(i<0x100) {
					*(*dst_p)++ = (char) i;
				} else {
					if (ut_session->nomapping_char < 0x100) *(*dst_p)++ = (char) ut_session->nomapping_char;
				}
			}
		} else {
			ERROR ("charset type not managed : %d", src_cs->type)	
		}
	} else {
		if (ut_session->nomapping_char < 0x80) *(*dst_p)++ = (char) ut_session->nomapping_char;
		else ERROR ("nomapping char must be < 0x80") ;
	}

}


void ut_insert_eol (char ** dst_p, UtEolType dst_eol) {

	switch (dst_eol) {
	  case UT_EOL_CRLF:
	DBG3_S ("+CR");
		*(*dst_p)++ = 0xD; 
	  case UT_EOL_LF:
	DBG3_S ("+LF");
		*(*dst_p)++ = 0xA; break;
	  case UT_EOL_LFCR:
	DBG3_S ("+LF");
		*(*dst_p)++ = 0xA; 
	  case UT_EOL_CR:
	DBG3_S ("+CR");
		*(*dst_p)++ = 0xD; break;
	  case UT_EOL_BSN:
	DBG3_S ("+BSN");
		*(*dst_p)++ = '\\'; *(*dst_p)++ = 'n'; break;
	  case UT_EOL_NUL:
	DBG3_S ("+NUL");
		*(*dst_p)++ = 0; break;
	  default:
		ERROR ("EOL not accepted for conversion : %d", dst_eol)
	  }
}

/*!
 * \brief Count the number of extended character in a text.
 */

uint ut_count_ext_char (UtText * text) {
	uint count = 0, i;	
	for (i=0x80; i<0x100; i++)
		count += text->distribution[i];
	return count;
}



/*!
 * \brief Return the difference between the size of a text and its size after conversion.
 *
 * \param text UtText structure containing the text, the source and the destination charsets
 *
 * \return The size difference. If value is negative, the text will be smaller, if positive,
 *         the text will be bigger.
 *
 * \todo EC Cette fonction ne retourne pas de code d'erreur alors qu'il y a des ERROR() et que
 *       de mauvais paramètres doivent pouvoir la faire pantée. Il faut donc mettre en 
 *       parametre un pointeur sur la variable à fixer (ou l'intégrer dans UtText) et mettre
 *       le type de retour à UtCode.
 *       AC Effectivement, bien qu'il s'agisse alors d'erreurs dues à une mauvaise utilisation
 *		 de l'API.
 * \bug AC voir ut_size_unicode() quand retour = 0
 */

int ut_size_difference (UtText * src_text, UtText * dst_text) {
	
	ASSERT (src_text->charset != UT_UNSET)	
	ASSERT (dst_text->charset != UT_UNSET)
	ASSERT (src_text->eol != UT_EOL_UNSET)	
	ASSERT (dst_text->eol != UT_EOL_UNSET)
	ASSERT (src_text->eol_alt != UT_EOL_UNSET)	
	ASSERT (dst_text->eol_alt != UT_EOL_UNSET)
	
	long size;
	
	DBG3("*********** size diff********")

	UtCharset * src_cs = &(ut_session->charset [src_text->charset]);
	UtCharset * dst_cs = &(ut_session->charset [dst_text->charset]);
	
	if (src_cs->type == UT_CST_ASCII ) {
		if (dst_cs->type == UT_CST_ASCII) {
			if (ut_session->nomapping_char && ut_session->nomapping_char <0x80) size = 0;
			else size = - ut_count_ext_char (src_text);
		} else if (dst_cs->type == UT_CST_ASCII_EXTENSION) {
			if (ut_session->nomapping_char <0x100) size = 0;
			else size = - ut_count_ext_char (src_text);
		} else if (dst_cs->type == UT_CST_UTF_8) {
			if (ut_session->nomapping_char != UT_UNICODE_NONCHAR) 
				size = (ut_size_unicode (ut_session->nomapping_char)-1) * ut_count_ext_char (src_text);	
			else size = - ut_count_ext_char (src_text);	
		} else {
			ERROR ("charset type not managed : %d", dst_cs->type)
		}

	} else if (src_cs->type == UT_CST_ASCII_EXTENSION) {
		if (dst_cs->type == UT_CST_ASCII) {
			if (ut_session->nomapping_char <0x80) size = 0;
			else size = - ut_count_ext_char (src_text);

		} else if (dst_cs->type == UT_CST_ASCII_EXTENSION) {
			int count = 0;
			if (ut_session->nomapping_char>=0x100) {
				int i; for (i=0x80; i<0x100; i++) {
					if (src_text->distribution[i]) {
						ulong unicode = src_cs->unicode[i];
						int j; for (j=0x80; j<0x100; j++) if (unicode==dst_cs->unicode[j]) break;
						if (i==0x100) count -= src_text->distribution[i];
					}
				}
			}
			size = count;
				
		} else if (dst_cs->type == UT_CST_UTF_8) {
			int count = 0;
			int i; for (i=0x80; i<0x100; i++) {
				if (src_text->distribution[i]) {
					ulong unicode = src_cs->unicode[i];
					if (unicode != UT_UNICODE_NONCHAR) 
						count += (ut_size_unicode (unicode) - 1)*src_text->distribution[i];
					else if (ut_session->nomapping_char!=UT_UNICODE_NONCHAR) 
						count += (ut_size_unicode (ut_session->nomapping_char) - 1)*src_text->distribution[i];
					else count -= src_text->distribution[i];
				}
			}
			size = count;
		} else {
			ERROR ("charset type not managed : %d", dst_cs->type)
		}
	} else if (src_cs->type == UT_CST_UTF_8 ) {
		if (dst_cs->type == UT_CST_ASCII) {
			if (ut_session->nomapping_char <0x80) size = 0;
			else size = - ut_count_ext_char (src_text);

		} else if (dst_cs->type == UT_CST_ASCII_EXTENSION) {
			size = 0;

		} else if (dst_cs->type == UT_CST_UTF_8) {
			if (ut_session->nomapping_char == UT_UNICODE_NONCHAR) size = 0;
			else size = - (ut_size_unicode (ut_session->nomapping_char) - 1) * ut_count_ext_char (src_text);

		} else {
			ERROR ("charset type not managed : %d", dst_cs->type)
		}
	} else {
		ERROR ("charset type not managed : %d", dst_cs->type)
	}
	
	DBG3( "** size diff chars : % ld", size);
	
	switch (src_text->eol) {
	  case UT_EOL_NONE:
		break;		
	  case UT_EOL_CRLF:
	  case UT_EOL_LFCR:
		switch (dst_text->eol) { 
		  case UT_EOL_CRLF:
		  case UT_EOL_LFCR:
		  case UT_EOL_BSN:
		  //case UT_EOL_NOCHANGE:	  
			  break;
		  case UT_EOL_CR:
		  case UT_EOL_LF:
			size -= src_text->nb_lines; break;
		  case UT_EOL_NONE:
			size -= 2*src_text->nb_lines; break;
		  default: //+UT_EOL_NON_STD:
		  	ERROR ("dst EOL type unsupported")
		} break;
		
	  case UT_EOL_NUL:
	  case UT_EOL_CR:
	  case UT_EOL_LF:
	  case UT_EOL_MIX:		//1 or 2 bytes, we consider 1 for secureness
		switch (dst_text->eol) { 
		  case UT_EOL_CR:
		  case UT_EOL_LF:
		  case UT_EOL_NUL:	  
		  //case UT_EOL_NOCHANGE:	  
			break;
		  case UT_EOL_CRLF:
		  case UT_EOL_LFCR:
		  case UT_EOL_BSN:
			size += src_text->nb_lines; break;
		  case UT_EOL_NONE:
			size -= src_text->nb_lines; break;
		  default: //+UT_EOL_NON_STD:
		  	ERROR ("dst EOL type unsupported")
		} break;
	  default:
	  	ERROR ("src EOL type unsupported")
	}

	DBG3( "** size diff chars+eol : % ld", size);
	
	switch (src_text->eol_alt) {
	  case UT_EOL_NONE:
		break;		
	  case UT_EOL_NUL:
	  case UT_EOL_CR:
	  case UT_EOL_LF:
	  case UT_EOL_MIX:		//1 or 2 bytes, we consider 1 for secureness
		switch (dst_text->eol_alt) { 
		  case UT_EOL_CR:
		  case UT_EOL_LF:
		  case UT_EOL_NUL:	  
		  //case UT_EOL_NOCHANGE:	  
			break;
		  case UT_EOL_CRLF:
		  case UT_EOL_LFCR:
		  case UT_EOL_BSN:
			size += src_text->nb_lines_alt; break;
		  case UT_EOL_NONE:
			size -= src_text->nb_lines_alt; break;
		  default: //+UT_EOL_NON_STD:
		  	ERROR ("dst EOL type unsupported")
		} break;
	  default:
	  	ERROR ("src EOL type unsupported")
	}

	DBG3( "** size diff chars+eol+alt : % ld", size);
	
	return size;	
}


/*!
 * \brief Convert extended characters and EOL.
 * 
 * The conversion consists to :
 * - remove skip characters,
 * - change null characters to EOL tpye specified in UtText::dst_eol,
 * - change extended characters encoded with UtText::src_charset to UtText::dst_charset encoding.
 *
 * \param text UtText to convert. Updates UtText::data and UtText::size.
 *
 * \return UT_OK on success, error code otherwise.
*/

UtCode ut_conversion_pass (UtText * src_text, UtText * dst_text) {
	
	ASSERT (src_text)
	ASSERT (dst_text)

	ASSERT (dst_text->data == NULL)
	
	//TODO? create dst_text?
	if (dst_text->eol==UT_EOL_UNSET) dst_text->eol = src_text->eol;
	if (dst_text->eol_alt==UT_EOL_UNSET) dst_text->eol_alt = src_text->eol_alt;
	free (dst_text->data);
	dst_text->data = NULL;
	
	long newsize = ut_size_difference (src_text, dst_text);
	
	DBG3 ("size diff : %ld   ext char : %d", newsize, ut_count_ext_char (src_text) )
	newsize += src_text->size;
	DBG3 ("old size: %lu  new size: %lu", src_text->size, newsize)

	//Allocate new buffer for dst
	char *dst_beg = (char*) malloc (newsize+1);	//+1 for UT_EOE_CHAR
	if (!dst_beg) return UT_MALLOC_ERROR;

	char *src = src_text->data;
	char *src_end = src_text->data + src_text->size;
	char *dst = dst_beg;
	int cumul=1;
	
	for (;;) {
		DBG3_S ("<%d>", *src);
		if (!is_ext (*src)) {
			if (*src) {
				if (*src==src_text->skip_char) {
					src++;
				} else if (*src==UT_EOL_ALT_CHAR) {
					ut_insert_eol (&dst, dst_text->eol_alt);
					src++;
				} else {
					*dst++ = *src++;
				}
			} else { //UT_EOL_CHAR
				if (src - src_text->data >= UT_PROCESS_STEP*cumul && ut_session->progress_function) {
					if (!ut_update_progress (src_text, src - src_text->data, false)) break;
					cumul++;
				}
				if (src >= src_end) {
					ASSERT (src==src_end)
					*dst = 0;
					break; //last line?
				}
				ut_insert_eol (&dst, dst_text->eol);
				src++;
				DBG3_S ("!")
			}
		} else { //ext_char
			ut_conv_char (&src, &dst, src_text->charset, dst_text->charset);
		}
	} //for (;;)

	if (src < src_end) {
		//CLEAN HERE!
		DBG3 ( "interrupted! : src:%d   srcend: %d dst:%d", src - src_text->data, src_end - src_text->data, dst - dst_beg)
		free (dst_beg);
		return UT_INTERRUPTED_BY_USER;
	}


	ASSERT ( dst - dst_beg <= newsize )
	DBG3 ( "precalculated size: %ld   actual size: %d", newsize, dst - dst_beg)
	
	//free (src_text->data);
	dst_text->data = dst_beg;
	dst_text->size = dst - dst_beg;
	
	DBG2 ("Conversion done!")
	return UT_OK;
}
