/* $Id: readscript.c,v 1.12 2000/02/09 15:42:24 gaal Exp $ */

/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library,
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.60
   filename   : readscript.c
   Date       : 10/01/1999
   Purpose    : SCRIPT HANDLING FUNCTIONS FOR  LOOK-UP OF WORDS IN DICTIONARIES encapsulation
   Programmer : Claude ROUX
   Reviewer   :
   */


/*************************************************************
 *
 *  FILE  "readscript.c"
 *
 *      SCRIPT HANDLING FUNCTIONS FOR
 *      LOOK-UP OF WORDS IN DICTIONARIES
 *
 *  Andre Kempe, RANK XEROX, Grenoble, France
 *  30/01/96 - ...
 *
 *************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "fst/types.h"
#include "fst/struct.h"
#include "fst/modify.h"
#include "fst/bin-in.h"
#include "fst/look.h"
#include "fst/label.h"
#include "fst/twol-net.h"
#include "fst/pars-tbl.h"
#include "fst/compact.h"

#include "lktypes.h"
#include "ems.h"
#include "lookup.h"
#include "networks.h"
#include "readscript.h"
#include "fst-types.h"
#include "interface.h"
#include "comm-def.h"
#include "interface.lex.h"
#include "re.lex.h"
char buffer_erreur[1000];
void message_erreur(void* parseur, char* message);
void NormalisationNomFichier(char* nomFichier, char* tampon);
void AjouteFichierDansDivers(void* parseur, char* name);
void GetGrmPath(void* parseur, char* path);
/*===========================================================
  HANDLE (+HASH) SYMBOLS
  ===========================================================*/
tempBuffer* alloc_tempBuffer() {
	tempBuffer* a;
	int i;
	a = (tempBuffer*)malloc(sizeof(tempBuffer));
	a->size = 50;
	a->buffer = (char**)malloc(sizeof(char*)*a->size);
	for (i = 0; i < a->size; i++)
		a->buffer[i] = (char*)malloc(1024);
	a->max = 0;
	return a;
}

tempIntBuffer* alloc_tempIntBuffer() {
	tempIntBuffer* a;
	a = (tempIntBuffer*)malloc(sizeof(tempIntBuffer));
	a->size = 50;
	a->buffer = (int*)malloc(sizeof(int)*a->size);
	a->max = 0;
	a->i = 0;
	return a;
}

void add_tempBuffer(tempBuffer* tb, char* s) {
	char** buffer;
	int i;
	int size = tb->size;
	if (tb->max >= tb->size) {
		size += 50;
		buffer = (char**)malloc(sizeof(char*)*size);
		for (i = 0; i < size; i++) {
			buffer[i] = (char*)malloc(1024);
			if (i < tb->size) {
				strcpy(buffer[i], tb->buffer[i]);
				free(tb->buffer[i]);
			}
		}
		free(tb->buffer);
		tb->buffer = buffer;
		tb->size = size;
	}
	strcpy(tb->buffer[tb->max], s);
	tb->max++;
}

void add_tempIntBuffer(tempIntBuffer* tb, int s) {
	if (tb->i >= tb->size) {
		tb->size += 50;
		tb->buffer = (int*)realloc(tb->buffer, tb->size*sizeof(int));
	}
	tb->buffer[tb->i] = s;
	tb->i++;
	tb->max++;
}

void dealloc_tempBuffer(tempBuffer* tb) {
	int i;
	for (i = 0; i < tb->size; i++)
		free(tb->buffer[i]);
	free(tb->buffer);
	free(tb);
}

void dealloc_tempIntBuffer(tempIntBuffer* tb) {
	free(tb->buffer);
	free(tb);
}


long calculate_symbol_hash_index(unsigned char *key)
{
	unsigned char *pt;  long index = 1, index2 = 0;

	for (pt = key; *pt; pt++)
	{
		index *= *pt; index2 += *pt;
	}

	index += 17 * index2;

	return (labs(index % SYMBOL.hash_tab_len));
}

void hash_insert_symbols()
{
	/* Hash inserts all symbols. */

	HASHptr *hash, cell;  char **symbol;  long i;

	for (i = 0; i < SYMBOL.num_symbols; i++)
	{
		symbol = SYMBOL.symbol + i;
		cell = SYMBOL.hash_list + i;
		hash = SYMBOL.hash_tab + calculate_symbol_hash_index((unsigned char *)*symbol);

		cell->item = symbol;
		cell->next = *hash;
		*hash = cell;
	}
}

short hash_find_symbol(char *key)
{
	HASHptr cell;  short idx;

	for (cell = SYMBOL.hash_tab[calculate_symbol_hash_index((unsigned char *)key)];
		cell;
		cell = cell->next)
	if (!strcmp(*(cell->item), key))
		break;           /* item found */

	if (cell)
		idx = cell - SYMBOL.hash_list;
	else
		idx = NO_INDEX;

	return (idx);
}

/*-----------------------------------------------------------*/
void make_symbol_table(void* parseur)
{
	char *pt;  int i;

	alloc_symbol_table(TEMP_DATA.num_nets, TEMP_DATA.len_symbol_byffer);

	//rewind (TEMP_FILES.symbols);

	for (i = 0, pt = SYMBOL.buffer;
		i< SYMBOL.num_symbols;
		i++, pt += strlen(pt) + 1)
	{
		//lookup_read_line (line, L_LINE, TEMP_FILES.symbols);
		strcpy(pt, TEMP_FILES.symbols->buffer[i]);
		SYMBOL.symbol[i] = pt;
	}

	if (pt > SYMBOL.buffer + SYMBOL.bf_len)
		message_erreur(parseur, "symbol buffer overflow (programming error)");

	hash_insert_symbols();
}

/*===========================================================
  HANDLE TEMPORARY FILES
  ===========================================================*/

void open_temp_files(void* parseur)
{
	//TEMP_FILES.symbols = tmpfile ();
	//TEMP_FILES.netfile_names = tmpfile ();
	//TEMP_FILES.strategies = tmpfile ();

	TEMP_FILES.symbols = alloc_tempBuffer();
	TEMP_FILES.netfile_names = alloc_tempBuffer();
	TEMP_FILES.strategies = alloc_tempIntBuffer();


	if ((!TEMP_FILES.symbols) ||
		(!TEMP_FILES.netfile_names) ||
		(!TEMP_FILES.strategies))
		message_erreur(parseur, "temporary files could not be opened");
}

void close_temp_files()
{
	//fclose (TEMP_FILES.symbols);
	//fclose (TEMP_FILES.netfile_names);
	//fclose (TEMP_FILES.strategies);

	dealloc_tempBuffer(TEMP_FILES.symbols);
	dealloc_tempBuffer(TEMP_FILES.netfile_names);
	dealloc_tempIntBuffer(TEMP_FILES.strategies);
}

/*-----------------------------------------------------------*/
void symbols_to_temp_files(FILE *script, void* parseur)
{
	char line[L_LINE + 2], buffer[L_LINE + 2], *token[MAX_TOKENS], name[L_LINE + 2],
		*symbol, *filename;
	int sym_idx, num;  long size_symbols = 0;

	for (sym_idx = 0;; sym_idx++)
	{
		if (!lookup_read_line(line, L_LINE, script))
			message_erreur(parseur, "unexpected end of file name section in script");

		strcpy(buffer, line);
		if (!(num = tokenize_line(buffer, token, SCRIPT_SEPARATORS)))
			break;   /* empty line == end of file name section in script */

		/* get symbol and file name */

		if (num == 1)   /* no symbol given; only file name */
		{
			filename = token[0];

			switch (sym_idx)
			{
			case LEXICON_IDX:
				symbol = LEXICON_SYMBOL;  break;
			case NORMALIZER_IDX:
				symbol = NORMALIZER_SYMBOL;  break;
			case GUESSER_IDX:
				symbol = GUESSER_SYMBOL;  break;
			default:
				sprintf(buffer_erreur, "you must assign a symbol to the file '%s'\nin line %d of the script", filename, sym_idx + 1);
				message_erreur(parseur, buffer_erreur);
			}
		}
		else if (num == 2)   /* symbol and file name given */
		{
			symbol = token[0];
			//We must solve the filename here...
			NormalisationNomFichier(name, token[1]);
			AjouteFichierDansDivers(parseur, name);
			filename = name;
		}
		else {
			sprintf(buffer_erreur, "too many tokens in line %d of the script\n%s", sym_idx + 1, line);
			message_erreur(parseur, buffer_erreur);
		}

		/* put symbol and file name to temporary files */

		size_symbols += strlen(symbol) + 1;
		//fprintf (TEMP_FILES.symbols, "%s\n", symbol);
		//fprintf (TEMP_FILES.netfile_names, "%s\n", filename);
		add_tempBuffer(TEMP_FILES.symbols, symbol);
		add_tempBuffer(TEMP_FILES.netfile_names, filename);
	}

	TEMP_DATA.num_nets = sym_idx;
	TEMP_DATA.len_symbol_byffer = size_symbols;
}

void strategies_to_temp_files(FILE *script, void* parseur)
{
	char line[L_LINE + 2], buffer[L_LINE + 2], *token[MAX_TOKENS];
	int idx = 0, sym, i, num, maxlen = 0;

	while (TRUE)
	{
		if (!lookup_read_line(line, L_LINE, script))
		{
			if (!idx)    /* no strategy section */
				message_erreur(parseur, "no strategies given in lookup script");
			else
				break;   /* end of strategy section */
		}

		strcpy(buffer, line);
		if (!(num = tokenize_line(buffer, token, SCRIPT_SEPARATORS)))
			continue;    /* ignore empty lines */

		/* convert symbols to IDs and write strategy to temp file */

		maxlen = max_of(maxlen, num);  /* max lenght of a strategy */

		for (i = 0; i < num; i++)
		{
			if ((sym = hash_find_symbol(token[i])) == NO_INDEX) {
				sprintf(buffer_erreur, "undefined symbol '%s' in strategy '%s'", token[i], line);
				message_erreur(parseur, buffer_erreur);
			}

			//fwrite (&sym, sizeof (int), 1, TEMP_FILES.strategies);		  
			add_tempIntBuffer(TEMP_FILES.strategies, sym);
		}

		sym = NO_INDEX;
		//fwrite (&sym, sizeof (int), 1, TEMP_FILES.strategies);
		add_tempIntBuffer(TEMP_FILES.strategies, sym);
		idx++;
	}

	TEMP_DATA.num_strategies = idx;
	TEMP_DATA.max_len_strategy = maxlen;
}

/*===========================================================
  FIND LOOKUP SCRIPT
  ===========================================================*/

int get_id_from_language(char *language)
{
	int id;

	if (!strcmp(language, LANG_LOOK_ENGLISH))
		id = lg_ENGLISH;
	else if (!strcmp(language, LANG_LOOK_FRENCH))
		id = lg_FRENCH;
	else if (!strcmp(language, LANG_LOOK_GERMAN))
		id = lg_GERMAN;
	else if (!strcmp(language, LANG_LOOK_DUTCH))
		id = lg_DUTCH;
	else if (!strcmp(language, LANG_LOOK_SPANISH))
		id = lg_SPANISH;
	else if (!strcmp(language, LANG_LOOK_PORTUGUESE))
		id = lg_PORTUGUESE;
	else if (!strcmp(language, LANG_LOOK_ITALIAN))
		id = lg_ITALIAN;
	else
		error_message("unknown language '%s' in command line", language);

	return (id);
}

char *get_script_name_from_environment(int id)
{
	char *name, *envvar;

	switch (id)
	{
	case lg_BASE:
		envvar = ENV_BASE;  break;
	case lg_ENGLISH:
		envvar = ENV_ENGLISH;  break;
	case lg_FRENCH:
		envvar = ENV_FRENCH;  break;
	case lg_GERMAN:
		envvar = ENV_GERMAN;  break;
	case lg_DUTCH:
		envvar = ENV_DUTCH;  break;
	case lg_SPANISH:
		envvar = ENV_SPANISH;  break;
	case lg_PORTUGUESE:
		envvar = ENV_PORTUGUESE;  break;
	case lg_ITALIAN:
		envvar = ENV_ITALIAN;  break;
	default:
		error_message("unknown language ID %d\n\
					  (programming error in 'get_script_name_from_environment')", id);
	}

	name = getenv(envvar);
	return (name);
}

char *get_script_name_from_default(int id)
{
	char *name;

	switch (id)
	{
	case lg_BASE:
		name = LSCR_BASE;  break;
	case lg_ENGLISH:
		name = LSCR_ENGLISH;  break;
	case lg_FRENCH:
		name = LSCR_FRENCH;  break;
	case lg_GERMAN:
		name = LSCR_GERMAN;  break;
	case lg_DUTCH:
		name = LSCR_DUTCH;  break;
	case lg_SPANISH:
		name = LSCR_SPANISH;  break;
	case lg_PORTUGUESE:
		name = LSCR_PORTUGUESE;  break;
	case lg_ITALIAN:
		name = LSCR_ITALIAN;  break;
	default:
		error_message("unknown language ID %d\n\
					  (programming error in 'get_script_name_from_default')", id);
	}

	return (name);
}

char *get_script_name_from_id(int id)
{
	char *name;

	if (!(name = get_script_name_from_environment(id)))
		name = get_script_name_from_default(id);

	return (name);
}

void find_default_script_name(char *language, char *script_name)
{
	char *base_path, *lang_script;
	int base, lang;

	base = lg_BASE;
	lang = get_id_from_language(language);

	base_path = get_script_name_from_id(base);
	lang_script = get_script_name_from_id(lang);

	strcpy(script_name, base_path);
	strcat(script_name, lang_script);
}

void find_script_name(char *option, char *argument, char *name)
{
	if (!strcmp(option, OPT_LANGUAGE))
		find_default_script_name(argument, name);
	else if (!strcmp(option, OPT_SCRIPT))
		strcpy(name, argument);
	else
		error_message("unknown option '%s' in command line", option);
}

/*===========================================================
  READ LOOKUP SCRIPT
  ===========================================================*/

void read_script_get_networks(char *option, char *argument, void* parseur)
{
	FILE *script;  char script_name[L_LINE + 2];

	find_script_name(option, argument, script_name);

	if (!(script = fopen(script_name, "r"))) {
		sprintf(buffer_erreur, "lookup script '%s'\ncould not be found or opened", script_name);
		message_erreur(parseur, buffer_erreur);
	}
	else
	if (LOOKUP_FLAGS.show_comments)
		fprintf(stderr, "Reading script from \"%s\"\n", script_name);

	open_temp_files(parseur);

	symbols_to_temp_files(script, parseur);
	make_symbol_table(parseur);

	strategies_to_temp_files(script, parseur);
	fclose(script);
	free_symbol_table();

	get_networks(parseur);
	get_strategies();

	close_temp_files();
}


/*===========================================================
  NO LOOKUP SCRIPT
  ===========================================================*/

NVptr no_script_get_networks(char *argument)
{

	/* Reads in the networks from the file and makes up a default
	   strategy. We assume that the first net is the lexicon and
	   the other nets, if any, are normalizing tranducers to be
	   applied in the given order. This is the same strategy
	   as in the old infl application.
	   */

	FILE *stream = fopen(argument, "rb");
	NVptr nv = NULL;
	int i;

	if (stream)
	{
		if (std_file_header_p(stream))
		{
			LEXNETS = networks_from_file(stream);

			if (!LOOKUP_FLAGS.vectorize_nets) /** shared arc doesn't work together with vectorization tg **/
			for (i = 0; i < NV_len(LEXNETS); i++)
				NV_net(LEXNETS, i) = SHARE_make_shared_arclists(NV_net(LEXNETS, i), DONT_COPY);
		}
		else
		{
			fclose(stream);
			error_message("'%s' is not the right type of file\n",
				argument);
		}
	}
	else
		error_message("Lexicon file '%s' could not be found or opened\n",
		argument);

	if (NV_len(LEXNETS) > 0)
	{
		STRATEGIES = make_nvv(NV_len(LEXNETS));

		/* The main lexicon. */

		nv = make_nv(1);
		NV_net(nv, 0) = NV_net(LEXNETS, 0);
		NVV_nv(STRATEGIES, 0) = nv;

		for (i = 1; i < NV_len(LEXNETS); i++)
		{
			/* Normalizing transducers */

			nv = make_nv(2);
			NV_net(nv, 0) = NV_net(LEXNETS, i);
			NV_net(nv, 1) = NV_net(LEXNETS, 0);
			NVV_nv(STRATEGIES, i) = nv;
		}
	}
	else /* This should never happen. */
		error_message("Lexicon file '%s' is empty!\n",
		argument);
	return nv;
}

//---COMPILING SCRIPTS


void compiler_un_script_fst(char* lescript) {
	FST_CNTXTptr cntxt = (FST_CNTXTptr)get_default_context();
	ifile file;

	start_flex(NULL);
	start_regex_flex(NULL);

	print_prompt = 0;
	IY_VERBOSE = 1;
	IY_INTERACTIVE = 0;
	file.name = lescript;
	file.type = FILE_IN;
	iy_source(&file, cntxt);
	file.name = NULL;
}

