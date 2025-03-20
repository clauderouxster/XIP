/***************************************************************************
 *            utrac_cmd.c
 *
 *  Tue Oct  5 11:30:06 2004
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
 * \file utrac_cmd.c
 * \author Antoine Calando (antoine@alliancemca.net)
 * \brief "utrac" shell command.
 * \todo doc dixygen...
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <endian.h>
#include <math.h>

#include "utrac.h"

#undef UT_DEBUG
#define UT_DEBUG 3
#include "debug.h"

#define ESC_RED "\e[31m"
#define ESC_NONE "\e[0m"
const char * NONE_STR = "none";

/****************************************************************************************************/

int custom_pb_fct (float rate) {
	
	if (!rate) printf("Rate = 0\n");
		
	const int barsize = 100;
	
	putchar(0xD);
	float i; for (i=0; i < barsize*rate; i++) {
		putchar('*');
		//printf ("<%f>", i);
	}
	for (; i<barsize; i++) putchar ('-');
	putchar(0xD);
	
	//printf ("%2.0f%%\n", (double) 100*rate);
	if (rate==1.0) printf ("\n fin!\n");
	
	return true;
}

/****************************************************************************************************/


UtCode parse_charset_eol (char * charset_eol, int *charset_ip, UtEolType * eol_type) {
	
	char * separator = charset_eol;

	for (;;) {
		if (!*separator) {
			separator = NULL;
			break;
		} else if (*separator != '/') { 
			separator++;
		} else {
			*separator = 0;
			break;
		}
	}
	
	*charset_ip = ut_find_charset (charset_eol);
	
	if (*charset_ip != UT_UNSET && !separator) {
		*eol_type = UT_EOL_UNSET;
		return UT_OK;
	}
	

	if (separator) {
		*separator = '/';
		*eol_type = ut_find_eol (separator+1);
	} else {
		*eol_type = ut_find_eol (charset_eol);
	}

	if (*eol_type == UT_EOL_UNSET || (separator && *charset_ip == UT_UNSET))
		return UT_NOT_FOUND_ERROR;
	
	return UT_OK;
}
/****************************************************************************************************/

void test_rcode (UtCode rcode, char * argv[]) {
	if (rcode!=UT_OK) {
		fprintf (stderr, "%s: %s (error %d)\n", argv[0], ut_error_message(rcode), rcode);
		if (ut_session->error_string) fprintf (stderr, "%s: %s\n", argv[0], ut_session->error_string);
		exit(rcode);
	}
}

/****************************************************************************************************/
void print_all_ratings (UtText *text) {
		ASSERT (text->evaluation)
			
		ulong prev_checksum = 0;
		long prev_rating = LONG_MIN; 
		long rating;
		
		bool *done = malloc (sizeof(bool)*ut_session->nb_charsets);
		int i; for (i=0; i<ut_session->nb_charsets; i++) done[i] = false;
		printf ("With locale   Brut   Checksum      Name(s)");
		for (;;) {
			long rating_max = LONG_MIN; 
			short index = -1;
			
			for (i=0; i<ut_session->nb_charsets; i++) {
				if (done[i] || ut_session->charset[i].type != UT_CST_ASCII_EXTENSION) continue;
				//printf ("<%d>", i);
				rating = text->evaluation[i].rating * ut_get_charset_coef (i);

				//DBG3 ("%li = %li * %f", rating, src_text.evaluation[i].rating, 
				//	UT_LANGUAGE_COEF[ ut_session->charset[i].language [ut_session->language_default]])

				if (rating == prev_rating ) {
					rating_max = rating;
					index = i;
					//printf ("*");
					if (text->evaluation[i].checksum == prev_checksum) break;
				} else if (rating > rating_max) {
					rating_max = rating;
					index = i;
					//printf ("+");
				}
			}
			if (index == -1) break;
			if ( i == ut_session->nb_charsets) {
				printf ("\n%8ld %8ld    (%8lx)    %s", 
						(long) (text->evaluation [index].rating * ut_get_charset_coef (index)),
						text->evaluation [index].rating, text->evaluation [index].checksum,
						ut_session->charset [index].name );
			} else {
				printf (", %s", ut_session->charset [index].name);
			}
			prev_checksum = text->evaluation [index].checksum;
			prev_rating = rating_max;
			done[index] = true;
			
		} //for (;;)
		free(done);
		putchar('\n');
}


/****************************************************************************************************/

void print_parameters () {
	printf ("Language: ");
	if ( ut_session->language_default == UT_UNSET) printf ("None\n");
	else printf ("%s\n", ut_session->language.name[ut_session->language_default]);
	printf ("System: ");
	if (ut_session->system_default == UT_UNSET) printf ("None\n");
	else printf ("%s\n", ut_session->system.name[ut_session->system_default]);
	printf ("Output charset: %s\n", ut_session->charset[ut_session->charset_default].name);
	printf ("Output EOL: %s\n", UT_EOL_NAME[ut_session->eol_default]);
	printf ("Error character: ");
	if (ut_session->nomapping_char < 0x80) {
		printf("'%c'\n", (char) ut_session->nomapping_char);
	} else {
		printf ("(not yet coded, sorry)\n");
	//if (ut_session->charset[ut_session->charset_default].type == UT_CST_UTF_8 && )
	//ut_unicode_to_utf8c (
	}
}


/****************************************************************************************************/

void print_list () {
	int j;
	printf("Charsets:\n");
	for (j=0; j<ut_session->nb_charsets; j++) {
		printf ("  %s", ut_session->charset[j].name);
		if (ut_session->charset[j].alias)
			printf (", %s", ut_session->charset[j].alias);
		putchar('\n');
	}
	printf ("EOL: ");
	for (j=0; j<= UT_EOL_NONE; j++) {
		printf ("%s", UT_EOL_NAME[j]);
		if (j<UT_EOL_NONE) printf(", ");
		else putchar('\n');
	}
	printf ("Languages:\n");
	for (j=0; j < ut_session->language.n; j++) {
		printf ("  %c%c", ut_session->language.code[j*2+0], ut_session->language.code[j*2+1] );
		printf (" (%s)\n", ut_session->language.name[j]);
		//if (j+1<ut_session->language.n) printf(", ");
		//putchar('\n');
	}
	printf ("Systems:\n");
	for (j=0; j < ut_session->system.n; j++) {
		printf ("  %c%c", ut_session->system.code[j*2+0], ut_session->system.code[j*2+1] );
		printf (" (%s)\n", ut_session->system.name[j]);
		//if (j+1<ut_session->system.n) printf(", ");
		//else putchar('\n');
	}
	
	
	
}

/****************************************************************************************************/
void print_distribution (UtText *text) {
	
	uint i, j, max=0;
	char scan;
	const int COLS_N = 8;
	UtCharset * cs = &(ut_session->charset[ut_session->charset_default]);

	for (i=0; i<256; i++)
		if (text->distribution[i] > max) max = text->distribution[i];
	max = ((uint) floor (log10 ((double) max)) )+1;

	int nb_lines = 256/COLS_N;
	if (256%COLS_N) nb_lines++;
	for (i=0; i<nb_lines; i++) {
		for (j=i; j<256; j+=nb_lines) {
			
			if ( j < ' ' || j==0x7F
				|| ( 0x80 <= j && (cs->type==UT_CST_ASCII || cs->type==UT_CST_UTF_8) )
				|| ( cs->char_type && cs->char_type[j].categorie == UT_CTG_CONTROL) )
				scan = '.';
			else
				scan = j;
				
			if (text->distribution[j]) 	printf("<%2x+%c> %*lu ", j, scan, max,text->distribution[j]);
			else					 	printf("<%2x-%c> %.*s ", j, scan, max, "          ");
		}
		printf("\n");
	}
	
}

/****************************************************************************************************/

void print_ext_chars (UtText *text, bool use_color_b) {
	UtExtCharLine *scan_exl = text->ext_char;
	char * scan_char;
	while (scan_exl) {
		printf ("%3lu [%lu]: ", scan_exl->line_i, scan_exl->nb_ext_chars);
		scan_char = scan_exl->line_p;
		while (*scan_char) {
			if ((u_char)*scan_char<0x80) putchar(*scan_char++);
			else {
				char dst_char[5]; //max utf-8 length is 4
				char *dst_char_p = dst_char;
				ut_conv_char (&scan_char, &dst_char_p, text->charset, ut_session->charset_default);
				*dst_char_p = 0;
				if (use_color_b) printf (ESC_RED);
				printf ("%s", dst_char);
				if (use_color_b) printf (ESC_NONE);
				
			//if (*scan_char>0x9F) printf ("\e[31m%c\e[0m", (int) *scan_char);
			//else printf ("\e[31m%c\e[0m", (int) *scan_char - 0x80 +'A');
			}
			//scan_char++;
		}
		putchar ('\n');
		scan_exl = scan_exl->next;	
	}
}

/****************************************************************************************************/
void all_ext_chars (UtText *text, bool use_color_b) {

	int i, j;
	
	printf("              |");
	for	(j=0x80; j<0x100; j++) {
		if (!text->distribution[j]) continue;
		printf (" %2X |", j);
	}

	char in[1], out[7], *in_p, *out_p;
	
	for	(i=0; i<ut_session->nb_charsets; i++) {
		if (ut_session->charset[i].type != UT_CST_ASCII_EXTENSION) continue;
		printf("\n%13.13s |", ut_session->charset[i].name);
		
		for	(j=0x80; j<0x100; j++) {
			if (!text->distribution[j]) continue;
			in[0] = j; in_p = in;
			out_p = out;
			ut_conv_char (&in_p, &out_p, i, ut_session->charset_default);
			*out_p = 0;
			if (use_color_b) printf (ESC_RED);
			printf ("  %s ", out);
			if (use_color_b) printf (ESC_NONE);
			putchar ('|');
		}
	}
	
	putchar('\n');
	
}

/****************************************************************************************************/
int callback (UtText * text, float progress) {
	
	ut_print_binary (text->current_pass);
	printf (" : progress : %f\n", progress);
	
	return true;
}


int main (int argc, char * argv[]) {
	
	int i;
	
	for (i=1; i<argc; i++) {
		
		//if (argv[i][0] =='-') {
		if (!strcmp (argv[i], "-h") ||	!strcmp (argv[i], "--help")) {
			fprintf (stderr, 
			"usage: %s [OPTION] [FILE]\n"
			"With no FILE, read standard input; with no OPTION, recognize and write converted text to standard output.\n"
			"LC_ALL, LC_TYPE, LANG are read to determine prefered language and output charset.\n"
			"\n"
			" -i  --file-info          Print file information\n"
			" -p  --print-charset      Print recognized charset\n"
			" -P  --print-all-charset  Print ranked list of charsets\n"
			"\n"
			" -f  --from               Force input charset (disable recognition)\n"
			" -t  --to                 Select output charset\n"
			" -L  --language           Select language\n"
			" -S  --system             Select system\n"
			"\n"
			" -x  --ext-chars          Print lines with extended characters\n"
			" -c  --colors             (with -x) Use colors\n"
			" -z  --distribution       Print distribution\n"
			" -a  --all-ext-chars      Print all ext chars in each charset\n"
			"\n"
			" -b  --bar                Display a progress bar\n"
			" -l  --list               List charsets/eol/lanuages/systems\n"
			" -d  --default-info       Print default/chosen parameters\n"
			" -v  --version            Print version\n"
			" -h  --help               Print this help\n"
			"\n"
			"For more information, try: man utrac\n",
			argv[0]
			);
			exit(0);
		} else if (!strcmp (argv[i], "-v") ||	!strcmp (argv[i], "--version")) {
			fprintf (stderr, "Utrac Universal Text Recognizer And Converter (version " UT_VERSION ")\n"
					"Written by Antoine Calando - Alliance MCA (antoine@alliancemca.net)\n");
			exit(0);
		}
	}

	UtCode rcode = ut_init ();
	test_rcode (rcode,argv);

	const char * filename = NULL;
	
	int src_charset = UT_UNSET, dst_charset = UT_UNSET;
	UtEolType src_eol = UT_EOL_UNSET, dst_eol = UT_EOL_UNSET;
	bool print_ext_char_b = false;
	bool use_color_b = false;
	bool print_charset_name_b = false;
	bool print_all_ratings_b = false;
	bool print_parameters_b = false;
	bool print_list_b = false;
	bool print_file_info_b = false;
	bool print_distribution_b = false;
	bool progress_bar_b = false;
	bool convert_b = false;
	bool all_ext_chars_b = false;
	
	for (i=1; i<argc; i++) {
		if (!strcmp (argv[i], "-f") ||	!strcmp (argv[i], "--from")) {
			
			if (++i==argc || parse_charset_eol (argv[i], &src_charset, &src_eol)!=UT_OK) {
				fprintf (stderr, "%s : error invalid charset or EOL %s\nTry `%s --help' for more information\n", argv[0], argv[i], argv[0]);
				ut_finish (); exit(-1);
			}
			if (src_eol!=UT_EOL_UNSET) {
				fprintf (stderr, "%s : warning input EOL type is ignored\n", argv[0]);
			}
		} else if (!strcmp (argv[i], "-t") ||	!strcmp (argv[i], "--to")) {
			if (++i==argc || parse_charset_eol (argv[i], &dst_charset, &dst_eol)!=UT_OK) {
				fprintf (stderr, "%s : error invalid charset %s\nTry `%s --help' for more information\n", argv[0], argv[i], argv[0]);
				ut_finish (); exit(-1);
			}
		} else if (!strcmp (argv[i], "-x") ||	!strcmp (argv[i], "--ext-chars")) {
			print_ext_char_b = true;
		} else if (!strcmp (argv[i], "-c") ||	!strcmp (argv[i], "--colors")) {
			use_color_b = true;
		} else if (!strcmp (argv[i], "-i") || !strcmp (argv[i], "--file-info")) {
			print_file_info_b = true;
		} else if (!strcmp (argv[i], "-p") || !strcmp (argv[i], "--print-charset")) {
			print_charset_name_b = true;
		} else if (!strcmp (argv[i], "-P") || !strcmp (argv[i], "--print-all-charsets")) {
			print_all_ratings_b = true;
		} else if (!strcmp (argv[i], "-L") || !strcmp (argv[i], "--language")) {
			int language_id;
			if (++i==argc || 
				( ( language_id = ut_find_lang_sys (argv[i], &ut_session->language)) == UT_UNSET
					&& !ut_str_fuzzy_cmp (argv[i], NONE_STR, 0)	) ) {
				fprintf (stderr, "%s : error invalid language %s\nTry `%s --help' for more information\n", argv[0], argv[i], argv[0]);
				ut_finish (); exit(-1);
			}
			ut_session->language_default = language_id;	
		} else if (!strcmp (argv[i], "-S") || !strcmp (argv[i], "--system")) {
			int system_id = UT_UNSET;
			if (++i==argc || 
				( ( system_id = ut_find_lang_sys (argv[i], &ut_session->system)) == UT_UNSET
					&& !ut_str_fuzzy_cmp (argv[i], NONE_STR, 0)	) ) {
				fprintf (stderr, "%s: error invalid system %s\nTry `%s --help' for more information\n", argv[0], argv[i], argv[0]);
				ut_finish (); exit(-1);
			}
			ut_session->system_default = system_id;	
		} else if (!strcmp (argv[i], "-d") || !strcmp (argv[i], "--default-info")) {
			print_parameters_b = true;
		} else if (!strcmp (argv[i], "-l") ||	!strcmp (argv[i], "--list")) {
			print_list_b = true;
		} else if (!strcmp (argv[i], "-z") ||	!strcmp (argv[i], "--distribution")) {
			print_distribution_b = true;
		} else if (!strcmp (argv[i], "-a") ||	!strcmp (argv[i], "--all-ext-chars")) {
			all_ext_chars_b = true;
		} else if (!strcmp (argv[i], "-b") ||	!strcmp (argv[i], "--bar")) {
			progress_bar_b = true;
		} else if (!filename) {
			filename = argv[i];
		} else {
			fprintf (stderr, "%s : error invalid option '%s'\nTry `%s --help' for more information\n", argv[0], argv[i], argv[0]);
			ut_finish (); exit(-1);
		}
	}
	
	if (print_parameters_b) {
		print_parameters ();
	}
	if (print_list_b) {
		print_list ();
	}
		
	if (print_parameters_b || print_list_b) {	
		ut_finish ();
		exit (0);
	}

	UtText src_text, dst_text;
	ut_init_text (&src_text);
	ut_init_text (&dst_text);
	
	src_text.flags |= UT_F_IDENTIFY_EOL | UT_F_TRANSFORM_EOL;
	
	ASSERT (src_text.flags == (UT_F_IDENTIFY_EOL | UT_F_TRANSFORM_EOL 
							| UT_F_REMOVE_ILLEGAL_CHAR | UT_F_IDENTIFY_CHARSET) );
	
	if (dst_charset!=UT_UNSET) ut_session->charset_default = dst_charset;
	if (dst_eol!=UT_EOL_UNSET) ut_session->eol_default = dst_eol;

	if (src_charset!=UT_UNSET) {
		src_text.charset = src_charset;
		src_text.flags &= ~UT_F_IDENTIFY_CHARSET;
	}
	
	if (src_eol!=UT_EOL_UNSET) {
		src_text.eol = src_eol;
		src_text.flags &= ~UT_F_IDENTIFY_EOL;		
	}
		
	//if (use_color_b) print_ext_char_b = true;
	if (print_ext_char_b) src_text.flags |= UT_F_REFERENCE_EXT_CHAR;	//else src_text.flags &= ~UT_F_REFERENCE_EXT_CHAR;
		
	if (progress_bar_b) ut_session->progress_function = &callback;
	
	src_text.pass_flags = UT_PF_LOAD | UT_PF_RECOGNIZE;
	if (!print_distribution_b && !print_ext_char_b && !print_all_ratings_b && !print_charset_name_b && !all_ext_chars_b
		&& !print_file_info_b) {
		convert_b = true;
		src_text.pass_flags |= UT_PF_CONVERT;
	}
	
	ut_init_progress (&src_text);	

	rcode = ut_load (&src_text, filename);
	test_rcode (rcode,argv);

	rcode = ut_recognize (&src_text);
	test_rcode (rcode,argv);

	if (print_distribution_b) {
		print_distribution (&src_text);
	}
	
	if (print_ext_char_b) {
		print_ext_chars (&src_text, use_color_b);
	} 
	
	if (print_charset_name_b) {
		printf ("%s\n", ut_session->charset [src_text.charset].name);
	}

	if (print_file_info_b) {
		printf ("Filename: %s\n", filename?filename:"<stdin>");
		//if (src_text.evaluation) printf ("CONNARD");
		printf ("Charset (%s): %s\n", src_text.evaluation?"unsure":"sure", ut_session->charset [src_text.charset].name);
		printf ("EOL: %s (%lu lines)\n", UT_EOL_NAME [src_text.eol], src_text.nb_lines);
		if (src_text.eol_alt != UT_EOL_NONE)
			printf ("EOL alt: %s (%lu alt lines)\n", UT_EOL_NAME [src_text.eol_alt], src_text.nb_lines_alt);
		printf ("Size: %lu\n", src_text.size);
	}
	
	if (print_all_ratings_b) {
		if (src_text.evaluation) {
			print_all_ratings (&src_text);
		} else {
			printf ("%s\n", ut_session->charset [src_text.charset].name);
		}
	}
	
	if (all_ext_chars_b) {
		all_ext_chars (&src_text, use_color_b);	
	}

	if (convert_b) {
		rcode = ut_convert (&src_text, &dst_text);
		test_rcode (rcode,argv);

		printf ("%s", dst_text.data);
	}

	ut_free_text (&src_text);
	ut_free_text (&dst_text);
	
	ut_finish ();
	exit (0);

} //end main
	
	
	/*
	
	UtCode rcode = ut_init ();
	
	if (rcode!=UT_OK) {
		printf ("Error! ");
		if (ut_session->error_string) printf (ut_session->error_string);
		else printf (" num : %d", rcode);
		putchar('\n');
		return rcode;	
	}
	
	
	if (argc==2) {
	
		UtText * text = ut_init_text ();
		
		if (strcmp(argv[1],"-")) {
			DBG3 ("Loading : <%s>", argv[1])
			src_text.filename = argv[1];
		} else {
			DBG3 ("stdin!")
			src_text.filename = NULL;
		}
		ut_session->progress_function = custom_pb_fct;
			
		rcode = ut_process_text (text, true);
		if (rcode!=UT_OK) {
			printf ("Error! ");
			if (ut_session->error_string) printf (ut_session->error_string);
			else printf (" num : %d", rcode);
			putchar('\n');
			return rcode;
		}
		
		ut_debug_text (text);
		//ut_debug_text_rating (text);

		//src_text.dst_charset = 4;
		//src_text.dst_eol = UT_EOL_CRLF;
		//ut_session->nomapping_char = UT_UNICODE_NONCHAR; //0x7F; //
		//ut_conversion_pass (text);
		
		printf ("=============== conversion de %s vers %s ================\n",
			ut_session->charset [src_text.src_charset].name, ut_session->charset [src_text.dst_charset].name);
		//printf (src_text.data);
		
		ut_free_text(src_text);
	} //if argc==1
	
	ut_finish ();
	
	return UT_OK;
	*/
