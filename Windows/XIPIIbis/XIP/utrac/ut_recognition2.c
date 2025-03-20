/***************************************************************************
 *            ut_recognition2.c
 *
 *  Tue Oct  5 11:29:47 2004
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
 * \file ut_recognition2.c
 * \author Antoine Calando (antoine@alliancemca.net)
 * \brief Extended ASCII charset pass.
 */

#include <stdlib.h>
#include <stdio.h>
#include "ut_text.h"
#include "ut_charset.h"
#include "utrac.h"

//#undef UT_DEBUG
//#define UT_DEBUG 3
#include "debug.h"

/***************************************************************************/
//! \brief Move the scan_pre pointer to the previous character and return it. 
char inline ut_get_pre_char (char **scan_pre, UtText * text) {
	do { 
		if (*scan_pre == text->data) return 0;
		--(*scan_pre);	
	} while (**scan_pre == text->skip_char);
	return **scan_pre;
}

/***************************************************************************/
//! \brief Move the scan_post pointer to the next character and return it. 
char inline ut_get_post_char (char **scan_post, UtText * text, char *scan_end) {
	do { 
		if (*scan_post == scan_end) return 0; 
		++(*scan_post);
	} while (**scan_post == text->skip_char);
	return **scan_post;
}


/***************************************************************************/
/*!
 * \brief Rate each charset relatively yo the text and register lines with extended characters.
 *
 * - Rate single byte extended ascii charsets: the function scan the whole text. Each time an
 *   extended character is found, and for each charset, it is encoded in this charset, compared to
 *   the previous and following character(s), and depending on the result, some points are added to
 *   charset rating. For instance, "café" (Latin1) will get more points than "cafÈ" (MacRoman).
 *   The checksum of all the extended characters in each charset is also calculated, to determine
 *   which charsets will have the same result (see UtCharsetEval).
 * - Register lines with extended chars: each time an extended character is found, and if that
 *   character was not already found, the line is registered in a linked list (see UtExtCharLine).
 *   After the whole text is scanned, the line linked list is filtered and sorted to keep only
 *   the most revelant lines.
 *
 * \todo check if charmap exists!
 *
 * \return UT_OK on success, error code otherwise.
 */ 

UtCode ut_xascii_pass (UtText * text) {
	

	int i,j;
	char * scan = text->data;
	char * scan_end = text->data + text->size;
	
	char * line_beg = scan;
	ulong line_i = 0;
	ulong nb_ext_chars = 0;	//number of ext char in current line
	bool ext_char[0x80]; for (i=0x0; i<0x80; i++) ext_char[i] = false;	//bit for each of the 128 ext char in current line
	bool ext_char_diff = false;		//ext char not previously found in current line?
	
	UtExtCharLine * scan_exl, * pre_exl, * new_exl;
	ulong  ponct_init[UT_CTG_PONCT_IF_N]; for (i=0; i<UT_CTG_PONCT_IF_N; i++) ponct_init[i] = 0;
	
	
	if (text->charset == UT_UNSET) {
		if (!text->evaluation) 
			text->evaluation = (UtCharsetEval*) malloc ( sizeof (UtCharsetEval) * ut_session->nb_charsets);
		
		for (i=0; i<ut_session->nb_charsets; i++) {
			text->evaluation [i].rating = 0;
			text->evaluation [i].checksum = 0;
		}
	}

	int cumul = 1;
	scan--;
	for (;;) {
		scan++;
		if (!*scan) { //eol!!!
			if (scan - text->data >= UT_PROCESS_STEP*cumul && ut_session->progress_function) {
				if (!ut_update_progress (text, scan - text->data, false)) break;
				cumul++;
			}
			if (scan >= scan_end) {
				ASSERT (scan==scan_end)
				break; //last line?
			}
			if (text->flags & UT_F_REFERENCE_EXT_CHAR ) {
				if (ext_char_diff) { //extended char in this line?
					//create new struct
					new_exl = (UtExtCharLine*) malloc (sizeof(UtExtCharLine));
					new_exl->line_p = line_beg;
					new_exl->line_i = line_i;
					new_exl->nb_ext_chars = nb_ext_chars;
					
					//the link is inserted in the list which is sorted by
					//line with biggest number of extended char first
					if (!text->ext_char			//insert struct at first pos?
						|| text->ext_char->nb_ext_chars <= nb_ext_chars ) {
						new_exl->next = text->ext_char;
						text->ext_char = new_exl;
					} else {
						pre_exl = scan_exl = text->ext_char;
						while (scan_exl && scan_exl->nb_ext_chars > nb_ext_chars) {
							pre_exl = scan_exl;
							scan_exl = scan_exl->next;
						}
						pre_exl->next = new_exl;
						new_exl->next = scan_exl;
					}
					ext_char_diff = false;
				} //if
				nb_ext_chars = 0;
				line_beg = scan+1;
				line_i++;
			}
			
		} else if ((u_char)*scan>0x7F) { //char extended found
			if (text->flags & UT_F_REFERENCE_EXT_CHAR ) {
				nb_ext_chars++;
				if (!ext_char[(u_char)*scan-0x80]) { //already found?
					ext_char[(u_char)*scan-0x80] = true;
					ext_char_diff = true;
				}
			}

			if (text->charset == UT_UNSET) {	
			
				UtCharsetEval * cs_eval = &(text->evaluation[0]);
		
				//rate each charset for this extended char
				for (i=0; i<ut_session->nb_charsets; i++, cs_eval++) {
					UtCharset * cs = &(ut_session->charset[i]);
					if (cs->type != UT_CST_ASCII_EXTENSION) continue;
	
					char tmp;
					UtCateg pre1_ctg, pre2_ctg, scan_ctg, post1_ctg, post2_ctg, post3_ctg;
					UtScript pre1_scr, scan_scr, post1_scr;
					char * scan_pre = scan, * scan_post = scan;
					
					//get category and alphabet type of chars at pos scan-1, scan and scan+1
					scan_ctg  = (cs->char_type[(u_char) *scan].categorie);
					scan_scr  = (cs->char_type[(u_char) *scan].script);
					tmp = ut_get_pre_char  (&scan_pre, text);
					pre1_ctg  = (cs->char_type[(u_char) tmp].categorie);
					pre1_scr  = (cs->char_type[(u_char) tmp].script);
					tmp = ut_get_post_char (&scan_post, text, scan_end);
					post1_ctg = (cs->char_type[(u_char) tmp].categorie);
					post1_scr  = (cs->char_type[(u_char) tmp].script);
					
					//compare to previous and following char(s)
					switch (scan_ctg) {
					  case UT_CTG_UPPERCASE:
						if     ( pre1_ctg==UT_CTG_DELIMITER && 
								(post1_ctg==UT_CTG_LOWERCASE || post1_ctg==UT_CTG_UPPERCASE)) 		cs_eval->rating++;
						else
							if ( pre1_ctg==UT_CTG_UPPERCASE) 								cs_eval->rating++;
						else {
							post2_ctg = (cs->char_type [(u_char) ut_get_post_char (&scan_post, text, scan_end)].categorie);
							if (post1_ctg==UT_CTG_UPPERCASE && post2_ctg!=UT_CTG_LOWERCASE) 		cs_eval->rating++;
							else {
								pre2_ctg  = (cs->char_type [(u_char) ut_get_pre_char  (&scan_pre, text)].categorie);
								if ( pre1_ctg==UT_CTG_DELIMITER && post1_ctg==UT_CTG_DELIMITER &&
								  (( pre2_ctg==UT_CTG_UPPERCASE && post2_ctg==UT_CTG_UPPERCASE) ||
									(pre2_ctg==UT_CTG_NUMBER && post2_ctg==UT_CTG_NUMBER))) 			cs_eval->rating++;
							} 
						} break;
						
					  case UT_CTG_LOWERCASE:
						if     ( pre1_ctg==UT_CTG_LOWERCASE) 								cs_eval->rating++;
						else 
							if (post1_ctg==UT_CTG_LOWERCASE) 								cs_eval->rating++;
						else
							if ( pre1_ctg==UT_CTG_UPPERCASE && post1_ctg!=UT_CTG_UPPERCASE) 		cs_eval->rating++;
						else {
							pre2_ctg  = (cs->char_type [(u_char) ut_get_pre_char  (&scan_pre , text)].categorie);
							post2_ctg = (cs->char_type [(u_char) ut_get_post_char (&scan_post, text, scan_end)].categorie);
							post3_ctg = (cs->char_type [(u_char) ut_get_post_char (&scan_post, text, scan_end)].categorie);
							if ( pre1_ctg==UT_CTG_DELIMITER && post1_ctg==UT_CTG_DELIMITER &&
							  (( pre2_ctg==UT_CTG_LOWERCASE && (post2_ctg==UT_CTG_LOWERCASE || (post2_ctg==UT_CTG_UPPERCASE && post3_ctg==UT_CTG_LOWERCASE)) 
							  ) || (pre2_ctg==UT_CTG_NUMBER && post2_ctg==UT_CTG_NUMBER))) 		cs_eval->rating++;
						} break;
					  case UT_CTG_OTHER_LETTER:
							if (pre1_ctg==UT_CTG_OTHER_LETTER) 							cs_eval->rating++;
							if (post1_ctg==UT_CTG_OTHER_LETTER) 							cs_eval->rating++;
						break;
	
					  case UT_CTG_MARK:
							if (pre1_ctg>=UT_CTG_UPPERCASE && pre1_ctg<=UT_CTG_OTHER_LETTER) 		cs_eval->rating++;
							if (post1_ctg>=UT_CTG_UPPERCASE && post1_ctg<=UT_CTG_OTHER_LETTER) 	cs_eval->rating++;
						break;
	
					  case UT_CTG_CONTROL:
					  case UT_CTG_UNSET:
						cs_eval->rating-=2;
						break;
	
					  case UT_CTG_CURRENCY:
							if (pre1_ctg==UT_CTG_NUMBER || post1_ctg==UT_CTG_NUMBER) cs_eval->rating++;
							else if (pre1_ctg==UT_CTG_DELIMITER) {
								pre2_ctg  = (cs->char_type [(u_char) ut_get_pre_char  (&scan_pre , text)].categorie);
								if (pre2_ctg==UT_CTG_NUMBER ) cs_eval->rating++;
							}
						break;
	
					  case UT_CTG_SYMBOL:
						switch (cs->unicode[(u_char)*scan]) {
						  case 0x00B0: /* ° */
							pre2_ctg  = (cs->char_type [(u_char) ut_get_pre_char  (&scan_pre, text)].categorie);  
							if (pre2_ctg>UT_CTG_OTHER_LETTER && (*(scan-1)=='N' || *(scan-1)=='n') 
								&& post1_ctg>UT_CTG_OTHER_LETTER) cs_eval->rating+=3;
						} break;
					  case UT_CTG_DELIMITER:
						if (pre1_ctg==post1_ctg || *scan==*(scan-1) || *scan==*(scan+1)) cs_eval->rating++;
						break;
					  case UT_CTG_NUMBER:
					  case UT_CTG_PONCTUATION:
					  case UT_CTG_OTHER:  break;
					  default: 
						for (j=0; j<UT_CTG_PONCT_IF_N; j++) {
							if (scan_ctg==UT_CTG_PONCT_INIT_0+j) ponct_init[j]++;
							else if (scan_ctg==UT_CTG_PONCT_FINAL_0+j && ponct_init[j]) {
								ponct_init[j]--;
								cs_eval->rating+=2;
							}
						} //for
					} //switch
					
					//rate according to the script
					if (scan_scr==1) {
						if (scan_scr== pre1_scr)
							cs_eval->rating++;
						if (scan_scr == post1_scr)
							cs_eval->rating++;
					} else if (scan_scr>1) {
						if (scan_scr== pre1_scr)
							cs_eval->rating+=2;
						if (scan_scr == post1_scr)
							cs_eval->rating+=2;
					}

				} //for nb_charsets

			} //if (text->charset == UT_UNSET)
		
		} //if (*scan>0x7F)

	} //for (;;)
	
	
	//interrupted?
	if (scan<scan_end) {
		return UT_INTERRUPTED_BY_USER;
	}
	
	if (text->flags & UT_F_REFERENCE_EXT_CHAR ) {
		//filter the extended line linked list
		for (i=0x0; i<0x80; i++) ext_char[i] = false;
		pre_exl = scan_exl = text->ext_char;
		
		while (scan_exl) {  //scan each struct
			ext_char_diff = false;
			scan = scan_exl->line_p;
			while (*scan) { //scan each char
				if ((u_char)*scan>0x7F) { //char extended found
					if (!ext_char[(u_char)*scan-0x80]) { //already found?
						ext_char[(u_char)*scan-0x80] = true;
						ext_char_diff = true;
					}
				}
				scan++;
			}//while
			
			if (!ext_char_diff) { //remove the struct ext_char_line?
				pre_exl->next = scan_exl->next; //(first struct is never removed, so this code is ok)
				free (scan_exl);
				scan_exl = pre_exl->next;
			} else {
				pre_exl = scan_exl;
				scan_exl = scan_exl->next;
			}
		} //while

		//sort the extended line linked list with an insertion sort
		UtExtCharLine * src_exl, *pre_src_exl;
		UtExtCharLine * dst_exl, *pre_dst_exl;
		
		src_exl = pre_src_exl = text->ext_char;
		while (src_exl) {
			
			pre_dst_exl = dst_exl = text->ext_char;
			new_exl = src_exl->next;
			
			while (src_exl!=dst_exl) {
				if (src_exl->line_i < dst_exl->line_i) {
					//insert src before dst postion
					pre_src_exl->next = src_exl->next;
					src_exl->next = dst_exl;
	
					if (dst_exl == text->ext_char)  text->ext_char = src_exl; //fisrt pos?
					else pre_dst_exl->next = src_exl;	//second pos or after
					src_exl = pre_src_exl;
					break;
				} //if
				pre_dst_exl = dst_exl;
				dst_exl = dst_exl->next;
			} //while
			pre_src_exl = src_exl;
			src_exl = new_exl;
		} //while
	}
	
	if (text->charset == UT_UNSET) {	
		//calculate checksum for each charset
		for (i=0; i<ut_session->nb_charsets; i++) {
			if (ut_session->charset[i].type != UT_CST_ASCII_EXTENSION) continue;
			for (j=0x80; j<0x100; j++) {
				if ( text->distribution[j]) text->evaluation[i].checksum 
						= ut_crc32 (ut_session->charset[i].unicode[(u_char)j], text->evaluation[i].checksum);
			}
		}

		//choose the best charmap depending on the results of the estimation
		//and on the selected language
		double max_value = -1; //long could also be used
		short max_index = -1;
		double tmp;
		
		for (i=0; i<ut_session->nb_charsets; i++) {
			tmp = text->evaluation[i].rating;
			tmp *= ut_get_charset_coef (i);

			if (tmp > max_value) {
				max_value = tmp;
				max_index = i;
			}
		}
		text->charset = max_index;
	
		if (max_index<0) {
			DBG1 ("*** NO CHARSET SELECTED !!! ***")
			//return UT_CHARSET_NOT_RECOGNIZED_ERROR;
		} else {
			DBG2 ("%s selected", ut_session->charset[max_index].name)
		}
	}
	DBG2 ("Extended Ascii charset pass done! (%lu B)", text->size)
		
	return UT_OK;
}
