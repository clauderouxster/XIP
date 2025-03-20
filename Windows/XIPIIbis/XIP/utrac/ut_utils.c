/***************************************************************************
 *            ut_utils.c
 *
 *  Tue Oct  5 11:29:53 2004
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
 * \file ut_utils.c 
 * \brief Various internal functions
 * \author Antoine Calando (antoine@alliancemca.net)
 */

#include <sys/stat.h>
#include <unistd.h>
#include <float.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h> //for SSIZE_MAX

#include <endian.h>
#include <byteswap.h>

#include "utrac.h"
#include "ut_text.h"
#include "ut_charset.h"

//#undef UT_DEBUG
//#define UT_DEBUG 3
#include "debug.h"

/***************************************************************************/
/*!
 * \brief Load a file in a buffer
 * 	 
 * \param filename Path to the file.
 * \param buffer Pointer used to return buffer address. Buffer must be free after used by user.
 *
 * \return UT_OK on succes, error code otherwise.
 *
 * \bug EC il n'y a qu'un appel à read or lorsque read!=size ce n'est pas une erreur si errno
 *      vaut EAGAIN.
 *      AC read() ne peut pas renvoyer EAGAIN si le fichier n'a pas été open() en mode EAGAIN
 *      (norme POSIX)
 */
UtCode ut_load_charset_file (const char * filename, char ** buffer) {
	
	DBG3 ("Loading file %s...", filename)
	
	int fd = open (filename, O_RDONLY);
	if (fd==-1) return UT_OPEN_FILE_ERROR;

	struct stat f_stat;
	if (fstat (fd, &f_stat)) return UT_FSTAT_FILE_ERROR;
	
	//some space is needed to add an EOL and an EOF
	*buffer = (char*) malloc (f_stat.st_size + 2);
	if (!*buffer) return UT_MALLOC_ERROR;
	
	int code=read (fd, *buffer, f_stat.st_size);
	if (code<=0) return UT_READ_FILE_ERROR;
	if (code!=f_stat.st_size) return UT_READ_FILE_ERROR2;

	DBG3 ("File %s (%lu b) loaded!", filename, f_stat.st_size)
	
	*(*buffer+f_stat.st_size) = '\n';
	*(*buffer+f_stat.st_size+1) = UT_EOF_CHAR;

	if (close(fd)) return UT_CLOSE_FILE_ERROR;

	return UT_OK;
}



/***************************************************************************/
/*!
 * \brief Print a number in binary form on stdout (debug).
 *
 * \param src Number to print.
 *
 * \bug pas de gestion big/little endian
 */
void ut_print_binary (ulong src) {

	int i; for (i=0; i<16; i++) {
		if (src&1<<15) putchar('x');
		else putchar ('-');
		src<<=1;
		if (!((i+1)%4)) putchar(' ');
	}
	
}

/***************************************************************************/
/*!
 * \brief Print content of a UtText structure (debug)
 */
UtCode ut_debug_text (UtText * text) {
	
	ASSERT (text);
	
	printf ("=====> Structure UtText :\n");
	//data	
	printf ("- size : %lu - %luk - %lum\n", text->size, text->size/1024, text->size/1024/1024);
	printf ("- lines1 : %lu - %luk\n", text->nb_lines, text->nb_lines/1024);
	printf ("- lines2 : %lu - %luk\n", text->nb_lines_alt, text->nb_lines_alt/1024);
	printf ("- skip char : <%c>\n", text->skip_char);
	printf ("- flags : "); ut_print_binary (text->flags); putchar('\n');		
	//distrib
	//ext_char
	//charmap
	printf ("- eol1 : <%d>\n", text->eol);
	printf ("- eol2 : <%d>\n", text->eol_alt);
	printf ("- charset : <%hu>", text->charset);
	if (text->charset != UT_UNSET) printf (" (%s)", 
		ut_session->charset[text->charset].name);
	putchar('\n');
	//convert eol
	//convert charset
	return UT_OK;
}

/***************************************************************************/
/*!
 * \brief Print content of a UtText::evaluation array (debug)
 */
UtCode ut_debug_text_rating (UtText * text) {
	
	ASSERT (text);
	if (!text->evaluation) return UT_OK;
	
	int i; for (i=0; i<ut_session->nb_charsets; i++) {
		printf ("=> %2i:   chk:%11lx   rtg:%6ld     %s\n", i, text->evaluation[i].checksum, 
			text->evaluation[i].rating, ut_session->charset[i].name);	
	}
		
	return UT_OK;	
}

//@{
/**	brief Utility functions for ut_str_fuzzy_cmp()	These functions test if a character is uppercase, lowercase, letter or number.*/

static inline bool is_maj (char c) { return ('A'<=c && c<='Z'); }
static inline bool is_min (char c) { return ('a'<=c && c<='z'); }
static inline bool is_letter (char c) { c &= ~0x20;	return is_maj(c); }
static inline bool is_num (char c) { return ('0'<=c && c<='9'); }
// @}



/*! \brief get charset index from a string

*/

UtCharsetIndex ut_find_charset (char * charset_name) {
	
	ASSERT (charset_name)
	
	UtCharsetIndex i;
	for (i=0; i<ut_session->nb_charsets; i++) {
		if ( ut_session->charset[i].name &&
			ut_str_fuzzy_cmp (charset_name, ut_session->charset[i].name, 0)) break;
		if ( ut_session->charset[i].alias &&
			ut_str_fuzzy_cmp (charset_name, ut_session->charset[i].alias, 0)) break;	
	}

	if (i==ut_session->nb_charsets) return UT_UNSET;
	else return i;
}

UtEolType ut_find_eol (char * eol_name) {
	
	ASSERT (eol_name)
	
	UtEolType j;
	for (j=	UT_EOL_CR; j<UT_EOL_NONE; j++) 
		if ( UT_EOL_NAME[j] && ut_str_fuzzy_cmp (eol_name, UT_EOL_NAME[j], 0) ) break;

	if (j==UT_EOL_NONE) return UT_EOL_UNSET;
	else return j;
}

int ut_find_lang_sys (char * language_name, UtLangSys * lang_sys) {
	
	int language_id;
	char ln[2];
	
	ln[0] = language_name[0];
	ln[1] = language_name[1];
	if ('a'<= ln[0] && ln[0] <= 'z' ) ln[0] += 'A'-'a';
	if ('a'<= ln[1] && ln[1] <= 'z' ) ln[1] += 'A'-'a';
	
	for (language_id=0; language_id < lang_sys->n; language_id++) {
		if ( ln[0] == lang_sys->code[language_id*2+0] &&
			 ln[1] == lang_sys->code[language_id*2+1]) break;
	}
	
	if (language_id == lang_sys->n) return UT_UNSET;
	
	return language_id;
}



/***************************************************************************/
/*!
 * \brief Approximative comparaison between two strings.
 *
 * The comparaison focuses only on substrings composed of number or letter
 * (case is not significant). For instance "iso8859 1"=="ISO-8859-1",
 * but "Mac Roman"!="MacRoman".
 */
bool ut_str_fuzzy_cmp (const char *str1, const char *str2, char stop_char) {
	
	ASSERT(str1)
	ASSERT(str2)
	//DBG3 (" <%s> =? <%s> ", str1, str2);

	const char SEP = '*';
	const char END = 0;
	char prec1, c1=0;
	char prec2, c2=0;
	
	for (;;) {
		prec1 = c1;
		if (is_letter(*str1)) {
			if (is_maj(prec1) || prec1==SEP) c1 = *str1++ & ~0x20;
			else c1 = SEP;
		} else if (is_num (*str1)) {
			if (is_num (prec1) || prec1==SEP) c1 = *str1++;
			else c1 = SEP;
		} else if (!*str1 || *str1==stop_char) { 
			if (prec1==SEP) c1 = END;
			else c1=SEP;
		} else {
			c1 = SEP;
			while (!is_letter(*str1) && !is_num(*str1) && *str1 && *str1!=stop_char) str1++;
		}
		prec2 = c2;
		if (is_letter(*str2)) {
			if (is_maj(prec2) || prec2==SEP) c2 = *str2++ & ~0x20;
			else c2 = SEP;
		} else if (is_num (*str2)) {
			if (is_num (prec2) || prec2==SEP) c2 = *str2++;
			else c2 = SEP;
		} else if (!*str2 || *str2==stop_char) { 
			if (prec2==SEP) c2 = END;
			else c2=SEP;
		} else {
			c2 = SEP;
			while (!is_letter(*str2) && !is_num(*str2) && *str2 && *str2!=stop_char) str2++;
		}
		if (c1!=c2) {
			//DBG3 ("false");
			return false; }
		if (c1==END) {
			//DBG3 ("true");
			return true;
		}
	}
}




double ut_get_charset_coef (UtCharsetIndex i) {
	
	float coef;
	
	if (ut_session->language_default>=0)
		coef = UT_LANG_SYS_COEF [ut_session->charset[i].language[ut_session->language_default]];
	else
		coef = 1.0;

	if (ut_session->system_default>=0)
		 coef *= UT_LANG_SYS_COEF [ut_session->charset[i].system[ut_session->system_default]];
	
	return coef;
}





/***************************************************************************/
/*!
 * \brief Function which call the user-defined function UtText::progress_function.
 *
 * \param text Related UtText structure.
 * \param processed Size in byte processed, compared to UtText::size.
 * \param start_stop If true, the user-defined function will be call for initialisation or cleanup.
 *
 *	\return This function returns the same return code than the user-defined function, i.e. 0
 *          if the processing must be interrupted, 1 otherwise.
 */

bool ut_update_progress (UtText * text, ulong processed, bool start_stop) {
	
	ASSERT (ut_session->progress_function)
	
	float rate;
	
	if (start_stop) {
		if (!text->progress_done) rate = 0;
		else if (!text->progress_todo) rate = 1.0;
		else {
			rate = 0;
			DBG1 ("ut_update_progress: done!=0 && todo!=0 !?!?")
		}
	} else {
		rate = text->progress_done + (1-text->progress_done)*( (float) processed/text->size)/text->progress_todo;
		if (rate==0.0) rate = FLT_MIN;
		else if (rate==1.0) rate = 1.0 - FLT_MIN;
		if (rate>1.0) {
			DBG1 ("ut_update_progress: rate = %f !!", rate)
		}
	}
	
	return (*(ut_session->progress_function)) (text, rate);	
}

/***************************************************************************/
/*! \brief table CRC32 ? */
ulong ut_crc32_table[256];
/*! \biref MAGIC NUMBER ? */
const ulong UT_CRC32_POLY=0x04c11db7;

/***************************************************************************/
/*!
 * \brief Function which call the user-defined function UtText::progress_function.
 *
 * \param data The data to "checksum"
 * \param crc_in The previous returned checksum, 0 if none
 *
 * \return The resulting checksum.
 *
 * \note Compute the CRC of a data. Code was modified and the following may not be exact :
 *       The following C code (by Rob Warnock <rpw3@sgi.com>) does CRC-32 in
 *       BigEndian/BigEndian byte/bit order.  That is, the data is sent most
 *       significant byte first, and each of the bits within a byte is sent most
 *       significant bit first, as in FDDI. You will need to twiddle with it to do
 *       Ethernet CRC, i.e., BigEndian/LittleEndian byte/bit order.
 *       The CRCs this code generates agree with the vendor-supplied Verilog models
 *       of several of the popular FDDI "MAC" chips.
 */

ulong ut_crc32(ushort data, ulong crc_in) {
	ulong  crc;

	if (!ut_crc32_table[1]) {
		int i, j; ulong c;
		for (i = 0; i < 256; ++i) {
			for (c = i << 24, j = 8; j > 0; --j) c = c & 0x80000000 ? (c << 1) ^ UT_CRC32_POLY : (c << 1);
			ut_crc32_table[i] = c;
		}
	}
	crc_in = ~crc_in;
	crc = (crc_in << 8) ^ ut_crc32_table[((crc_in >> 16) ^ data )>>8];  //crc for 8 MSB of data
	crc = (crc << 8) ^ ut_crc32_table[(crc >> 24) ^ (data&0xFF)];       //crc for 8 LSB of data
	return ~crc;
}
