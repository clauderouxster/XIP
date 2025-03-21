/* $Id: tokenplus.cxx,v 1.137 2017/10/11 08:29:13 roux Exp $ */

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
   filename   : tokenplus.cxx
   Date       :
   Purpose    : tokenization+lookup  encapsulation
   Programmer : Claude ROUX
   Reviewer   :
   */

#ifdef NTMCONNECTION

extern "C" {
#include "fst/context.h"
#include "fst/cfsm.h"
#include "applyloc.h"
}

#include "fst/modify.h"
#include "fst/bin-in.h"
#include "fst/pars-tbl.h"
#include "lktypes.h"
#include "ems.h"
#include "lookup.h"

#include "networks.h"

#include "tok-aux.h"

#include <iostream>

using namespace std;
#include "xipvecta.h"

#include "parsbin.h"
#include "ntmhmm.h"
#include "maintok.h"
#include "versiontok.h"
#include "conversion.h"

void initialisation_fst();

#define LANGUAGE_PREFIX_LENGTH 3

#if defined _WINDOWS
#define strcasecmp _stricmp
#endif
void Trim(char* chaine);
char* strdup_vit(char* ch);
char* strdup_vit(const char* ch);
extern "C" {
#include "utf8_fatstr_util.h"
	void read_script_get_networks(char *option, char *argument, void*);
	int lookup_word(char* input, void* parseur);
	extern NVptr  LEXNETS;            /* transducers: lexicon, guesser, etc. */
	extern NVVptr STRATEGIES;        /* contains a NV for every strategy */
	extern int LOOKUP_ALGORITHM;
	void* get_strat_control();
	void set_strat_control(void* sc,int);
	int get_str_no();
	void set_success_count(long* sc);
	long* get_success_count();

	void reclaim_seq_memory();
	void free_btree_mem(void);
	void free_strats_control();
	void free_in_out_buffers();
	void free_success_count();

	Exporting bool xipExecutionEnd();

}

//------------------------------------------
void reseting_initialisation_fst() {
	if (NET_HEAP != NULL) {
		free_in_out_buffers();
		reclaim_seq_memory();
		free_btree_mem();
		unset_default_context();
		release_fsm_memory();
	}
}
//------------------------------------------

#define action_change	2
#define action_delete	4
#define action_insert	8
#define action_switch	16
#define action_last		32
#define action_prefix	64
#define action_prefix_detected	128

extern "C" {

	void AjouteFichierDansDivers(GlobalParseur* parseur, char* name) {
		parseur->Nom_divers.ajoute(DupliqueChaine(name));
	}

	void GetGrmPath(GlobalParseur* parseur, char* path) {
		int pos = parseur->GrmPathname.rfind(SEP);
		if (pos == -1) {
			path[0] = '.';
			path[1] = SEP;
			path[2] = 0;
		}
		else {
			strcpy(path, (char*)parseur->GrmPathname.c_str());
			path[pos + 1] = 0;
		}
	}

	char CheckEditDistance(GlobalParseur* parseur) {
		if (parseur->idsequence.size())
			return 1;
		return 0;
	}

	void RemoveId(GlobalParseur* parseur) {
		parseur->idsequence.pop_back();
	}

	char AddId(GlobalParseur* parseur, short id) {
		parseur->idsequence.push_back(id);
		return 1;
	}

	short* CleanEditDistance(GlobalParseur* parseur) {
		if (!parseur->editdistanceflags)
			return NULL;

		short* ret = new short[parseur->idsequence.size() + 3];
		int i;
		ret[0] = parseur->editdistancescore;
		ret[1] = parseur->idsequence.size();
		for (i = 0; i < parseur->idsequence.size(); i++)
			ret[i + 2] = parseur->idsequence[i];
		ret[i + 2] = 0;
		parseur->idsequence.clear();
		if ((parseur->editdistanceflags&action_last) == action_last)
			parseur->editdistancescore = 0;

		return ret;
	}

	char RemoveLastEditDistanceAction(GlobalParseur* parseur) {
		char ret = 0;
		if ((parseur->editdistanceflags&action_last) == action_last) {
			ret = 1;
			parseur->editdistanceflags &= ~action_last;
		}
		return ret;
	}

	void AddLastEditDistanceAction(GlobalParseur* parseur, char ret, short* pile) {
		if (pile == NULL)
			return;

		if (ret)
			parseur->editdistanceflags |= action_last;
		parseur->idsequence.clear();
		parseur->editdistancescore = pile[0];
		short sz = pile[1];
		for (int i = 0; i < sz; i++)
			parseur->idsequence.push_back(pile[i + 2]);
		delete[] pile;
	}


	char EditDistanceAction(GlobalParseur* parseur, char action) {
		if ((parseur->editdistanceflags&action_last) == action_last)
			return 0;

		if (parseur->editdistancescore >= parseur->editdistancemax)
			return 0;

		if ((parseur->editdistanceflags&action) == action) {
			parseur->editdistancescore++;
			return 1;
		}

		return 0;
	}

	void AddEditDistanceAction(GlobalParseur* parseur, char action) {
		parseur->editdistanceflags |= action;
	}

	char RemoveEditDistanceAction(GlobalParseur* parseur, char action) {
		if ((parseur->editdistanceflags&action) != action)
			return 0;
		parseur->editdistanceflags &= ~action;
		return 1;
	}

	char CheckEditDistanceAction(GlobalParseur* parseur, char action) {
		if ((parseur->editdistanceflags&action_last) == action_last)
			return 0;

		if ((parseur->editdistanceflags&action) == action)
			return 1;
		return 0;
	}

	void EditDistancePop(GlobalParseur* parseur) {
		parseur->editdistancescore--;
	}


	int get_label_utf8(id_type id, char* buffer, int escape_p) {
		FAT_STR name;

		switch (id) {

		case 0:			/* EPSILON */
			//fputc('0', stream);
			strcat(buffer, "0");
			return(1);
			break;
		case 1:			/* OTHER */
			//fputc('?', stream);
			strcat(buffer, "?");
			return(1);
			break;
		default:
			LABELptr label = id_to_label(id);
			switch (LABEL_arity(label)) {
			case 1:
				if (id == BOUNDARY_SYMBOL) {
					strcpy(buffer, ".#.");
					return(3);
				}
				else {
					name = LABEL_atom(label);
					if (escape_p && fat_strlen(name) == 1)
						/* Space, zero, newline, question mark, percent sign, tab, carriage return,
						vertical tabulate, backspace, form feed */
						ConvertFatStrtoUTF8(name, (UTF8*)buffer, (UTF8*)buffer + 10, lenientConversion);
					else
						ConvertFatStrtoUTF8(name, (UTF8*)buffer, (UTF8*)buffer + 10, lenientConversion);
					return 0;
				}
				break;
			default:
				strcpy(buffer, "<?>");
				break;
			}
		}
		return 1;
	}


	void message_erreur(GlobalParseur* parseur, char* message) {
		parseur->erreur(message);
	}
	void free_out_cell(OUT_CELLptr out);

	void insert_token_char(char c, GlobalParseur* parseur, char final) {
		if (c == 10)
			return;

		word_offset* w = parseur->lectures.fin();
		w->ajoute(c, 0, 0);
	}

	int symbolsize(id_type id, GlobalParseur* parseur) {
		static char bufferinterne[2048];
		if (parseur->TestOptions(ENTREE_UTF8)) {
			bufferinterne[0] = 0;
			if (get_label_utf8(id, bufferinterne, FALSE) == 0)
				return strlen(bufferinterne);
		}
		return 1;
	}

	void insert_token_id(id_type id, GlobalParseur* parseur) {
		static char bufferinterne[2048];
		bufferinterne[0] = 0;
		word_offset* w = parseur->lectures.fin();
		if (parseur->encodage_source == UTF8_ENCODING)
			get_label_utf8(id, bufferinterne, FALSE);
		else
			copy_label_name(id, UPPER, bufferinterne, 1990, TRUE, FALSE);
		//Cas tres particulier, ou la forme du lemme est aussi par accident un symbole multicaractere
		//La modification faite ici est tres locale et devra etre eliminee
		char ajout = NON;
		if (w->mot[w->longueur - 1] == 9 && w->mot[w->longueur - 2] == 9) {
			w->longueur--;
			ajout = OUI;
		}
		w->concatene(bufferinterne);
		if (ajout == OUI)
			w->ajoute(9, 0, 0);
	}

	void AfficheContenu(GlobalParseur* parseur, unsigned short* ins, char* buff) {
		char bi[1000];
		buff[0] = 0;

		short* inseq = (short*)ins;
		int nb = 0;
		while (*inseq) {
			if (*inseq != -1) {
				if (parseur->encodage_source == UTF8_ENCODING)
					get_label_utf8(*inseq, bi, FALSE);
				else
					copy_label_name(*inseq, UPPER, bi, 1990, TRUE, FALSE);
				strcat(buff, bi);
			}
			else
				strcat(buff, "/");
			if (nb >= 200)
				break;
			nb++;
			inseq++;
		}
		buff[nb] = 0;

	}
	//Insertion de la forme de surface que l'on insere en fonction de l'encodage
	//interne...
	void insert_token(char* buffer, GlobalParseur* parseur) {
		word_offset* w = parseur->fournirWordOffset();
		if (parseur->encodage_courant == parseur->encodage_source)
			w->concatene(buffer);
		else
		if (parseur->encodage_courant == UTF8_ENCODING)
			w->concatene((char*)parseur->conversionUTF8VersAscii((uchar*)buffer));
		else
			w->concatene((char*)parseur->conversionAsciiVersUTF8((uchar*)buffer));
	}

	void add_token(char* buffer, GlobalParseur* parseur) {
		word_offset* w = parseur->lectures.fin();
		w->concatene(buffer);
	}

	void insert_all_id_sequence(GlobalParseur* parseur, char prefix) {
		if (!prefix && parseur->editdistancescore == 0)
			return;

		insert_token_char(9, parseur, 0);
		for (int i = 0; i < parseur->idsequence.size(); i++)
			insert_token_id(parseur->idsequence[i], parseur);
		insert_token_char(9, parseur, 0);
		char buff[100];
		sprintf_s(buff,100, "+score_%d", parseur->editdistancescore);
		add_token(buff, parseur);
	}

	void produce_tokens_det(OUT_CELLptr cell, GlobalParseur* parseur) {

		/* Produce incremental output for the client, perhaps as a callback to
		the client. Ultimately this will depend on the API to the client--we
		might simply copy our local fsm structures into his (acyclic, if
		linear-bounded) graph structures. Here we make up a standard C fsm
		(could be constant) so that our existing fsm operations can be used
		to produce a readable transcript. TEXTPOS is used just to inform the
		client of our progress in terms that he also understands. If text
		was provided to us as a file, presumably the client also can see the
		current file position when we invoke his callback.
		*/

		static char internalstring[1000];
		if (cell) {
			produce_tokens_det(cell->link, parseur);
			if (cell->outid != EPSILON) {
				if (parseur->TestOptions(ENTREE_UTF8)) {
					internalstring[0] = 0;
					get_label_utf8(cell->outid, internalstring, FALSE);
				}
				else
					copy_label_name(cell->outid, UPPER, internalstring, 100, TRUE, FALSE);
				parseur->appel(internalstring, cell->offset);
				//print_label(cell->outid, stream, FALSE);
			}
			free_out_cell(cell);/* 27.03.96 This cell can be freed anyway? */
		}
	}




	enum is_first_of_cell_chain produce_tokens_nobranching_det(OUT_CELLptr cell,
		OUT_CELLptr succ,
		OUT_CELLptr rs_branch,
		void *parseur) {
		/* Produce tokens from the beginning (left-hand-side end) of the chain
		(let's call it HEAD) until the actual tail cell. We start from the tail
		and descend until the head is found.
		*/

		enum is_first_of_cell_chain  ch_head = head_not_yet_reached;

		if (cell) { /* (right-hand-side) successor MAY exist */
			/* Descend recursively on the chain until the very first cell
	on the left side (head) is reached */
			/* If there was a branch on the right_hand side then the
			remaining left_hand side part mustn't be deleted simply since
			the branch may prove valid and then those left_hand_side
			cells must be written out later.  But if there is a segment
			of chain from the head with no branching then this segment
			can be produced (printed - the characters) and freed (the
			cells)
			In the examples below numbers show reference counts, that
			is, the number of branches from this cell node.

			1 <- 1 <- 9 <- 1 <- 0 The leftmost segment of this chain
			(before 9) can  be written and freed;
			the rightmost segment must be freed only;
			9 itself must remain.
			3 <- 1 <- 1 <- 1 <- 0 The rightmost segment of this chain
			(after 3) must be freed WITHOUT being written.
			1 <- 1 <- 1 <- 1 <- 0	The whole chain can be written and freed
			including the rightmost (0) cell.
			*/
			if (cell->refcount > 1)
				/* this is a branch. It is told to recursive call(s) for ancestor cell(s). */
				rs_branch = cell;
			if ((cell->link) == NULL) {/* then it is the head:  useless to descend more ! */
				/* This cell is the head of the chain - that is, the first
character to be produced eventually - is reached now */
				if (cell->refcount < 2) {
					/* else this cell has other references as well; so, even
					if the head of the chain is being reached now, this
					cell must be neither freed nor produced */
					if (cell->outid != EPSILON)
						produce_tokens_det(cell, (GlobalParseurTokenLookup*)parseur);
					free_out_cell(cell);
					if (succ)
						succ->link = NULL;
					ch_head = head_reached;
				}
				else {
					/* Abandon all actions, that is, this chain
					should be left as it is immediately, because if we
					can't produce and free the head (ie first) cell (its
					outid character) we can't produce any further one
					either. Tell it to previous recursive calls of his
					routine.  */
					ch_head = head_reached_but_branching;
				}
				return (ch_head); /* ascend immediately */
			}

			ch_head = produce_tokens_nobranching_det(cell->link, cell, rs_branch, parseur);

			if (ch_head == head_reached_but_branching) {
				/* If neither this cell is branching nor its righthand-side offsprings
				are but there were branches in lefthand-side ancestors then simply
				free this cell without producing output: we are on a FAILURE path
				in this config but we would like to produce the most possible */
				if (rs_branch == NULL) {
					free_out_cell(cell);
					if (succ)
						succ->link = NULL;
				}
				return (ch_head);
			}

			if (ch_head == head_reached) {

				/*  This cell is on the right path because its left-hand
				predecessors have no branching (since the head of the
				chain has been reached and nobody reported a
				branching condition on the left side) so it can be
				simply written out and freed.  Nobody will miss it
				but its successor must know it doesn't exist any
				more.
				*/

				if (cell->refcount < 2) {
					if (cell->outid != EPSILON)
						produce_tokens_det(cell, (GlobalParseurTokenLookup*)parseur);
					free_out_cell(cell);
					if (succ)
						succ->link = NULL;
				}
				else {/* this cell halts the producing process because it branches. */
					ch_head = head_reached_but_branching;
					return (ch_head);
				}
			}
			else
				return (ch_head);
		}
		return (ch_head);
	}


	void free_output_cells_and_print_if_sure(OUT_CELLptr cell, void* parseur) {

		/* Supposes a linear chain backwards from cell;
		If a previous state is referred only by this, it can be deleted.
		Repeat it recursively!
		*/

		OUT_CELLptr prev, rs_branch;
		/* first the latest branch is cut - we know it is dead -
		until the most recent branching.
		*/
		while (cell && cell->refcount < 1) {

			if ((prev = cell->link)) {

				prev->refcount--;
			}
			free_out_cell(cell);
			cell = prev;
		}

		if (cell &&
			(cell->outid == tok_bound) &&
			cell->refcount == 1) {

			/* then this chain backward can be printed out because it MUST be valid -
			provided ALL previous elements has refcount 1.
			Then memory can be reclaimed */

			/* Now output is produced and cells are reclaimed. */
			if (cell->link) {
				/* Tokens produced below will be freed, too */
				rs_branch = NULL;
				if (cell->refcount) /* then it is BRANCHING */
					rs_branch = cell;
				if (head_reached == (produce_tokens_nobranching_det(cell->link, cell,
					rs_branch, parseur)))
					cell->link = NULL;  /* to be sure nobody passes it again */
			}
		}
	}

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void word_offset::print() {
	cout << mot << ":" << gauche << "-" << droite << endl;
}

GlobalParseurTokenLookup::~GlobalParseurTokenLookup() {
	lectures.nettoie();

	LOWER_SIGMAS_LEXNETS = alphabet_bas;
	LEXNETS = reseaux_lexicaux;
	STRATEGIES = strategies_lexicales;

	set_strat_control(controle_strategies,str_sto);
	free_strats_control();
	set_strat_control(NULL,0);

	set_success_count(success_count);
	free_success_count();
	set_success_count(NULL);

	free(token_file);
	free(script_file);


	if (LOWER_SIGMAS_LEXNETS != NULL && LEXNETS != NULL)
		free_lower_sigmas_all_networks();

	CHK_free_all_check_nets(reseaux_lexicaux);

	free_nv_only(reseaux_lexicaux);

	//if (range_map != NULL)
	//	free_range_map(range_map, RANGE_VECTOR_len(range_map));

	//if (my_parse_table != NULL)
	//	free_parse_table(my_parse_table);

	LOWER_SIGMAS_LEXNETS = NULL;
	STRATEGIES = NULL;
	LEXNETS = NULL;
}



void GlobalParseurTokenLookup::StringAssociation(unsigned char* chaine) {
	if (TestOptions(TEST_INPUT_UTF8))
		TestUtf8Chaine(chaine);

	donneesATraiter = chaine;
	entree_est_chaine = OUI;
	if (TestOptions(ENTREE_UTF8)) {
		encodage_courant = UTF8_ENCODING;
		init_string_functions(CHAR_ENC_UTF_8);
	}
	else {
		encodage_courant = encodage_fst;
		init_string_functions(CHAR_ENC_ISO_8859_1);
	}
}

FILE* GlobalParseurTokenLookup::FileAssociation(FILE* fichier) {
	fichierDonnees = fichier;
	if (TestOptions(TEST_INPUT_UTF8))
		TestUtf8Fichier(fichier);

	entree_est_chaine = NON;
	if (TestOptions(ENTREE_UTF8)) {
		encodage_courant = UTF8_ENCODING;
		init_string_functions(CHAR_ENC_UTF_8);
	}
	else {
		encodage_courant = encodage_fst;
		init_string_functions(CHAR_ENC_ISO_8859_1);
	}
	return fichierDonnees;
}

void GlobalParseurTokenLookup::SentenceAnalysis(char renumerote, char arbre) {
	num_phrase = 0;
	wbf = NULL;
	baseOffset = 0;
	baseOffsetCar = 0;
	renumerotation = renumerote;
	retourchariot = NON;

	if (taille_mot != wo.taille)
		wo.realloue(taille_mot);

	if (TestOptions(EXECUTE_TOKENIZE)) {
		if (entree_est_chaine == -1)
			return;
		if (entree_est_chaine == OUI)
			AnalyseTokenChaine();
		else
			AnalyseTokenFichier();
		entree_est_chaine = -1;
	}

	//Dans un mode d'analyse par token, on ne passe par Apply
	if (initialisation_analyse_token == OUI) {
		CloreAnalyse();
		return;
	}

	if (hmmFileName != NULL && hmmFileName[0] != 0) {
		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = TRUE;

		wbf = alloc_word_buffer(MAX_WORDS, MAX_ALTBF, MAX_LINEBF);
		vtree = alloc_viterbi(TAG.num);
	}

	if (entree_est_chaine)
		tokenizestring((char*)donneesATraiter, 1, TRUE);
	else
		tokenizefile(fichierDonnees, 1, TRUE);	

	if (wbf != NULL && wbf->num)   {/* still words in buffer */
		vtree->cur->flags.sent_end = TRUE;
		viterbi_next_word(FALSE);
	}

	FinAnalyse = OUI;
	ajusteoffset = true;
	while (phrase.dernier != 0) {
		Parse(OUI);
		Detruit();
		num_phrase++;
		ajusteoffset = false;
		if (xipExecutionEnd())
			break;
	}
	FinAnalyse = NON;
	//HMM lu
	if (wbf != NULL) {
		words_total = wbf->words_total;

		free_word_buffer(wbf);
		free_viterbi(vtree);

		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = FALSE;
	}

	//  return (words_total);
}


GlobalParseurTokenLookup::GlobalParseurTokenLookup(char* tokenfile,
	char* script,
	char* hmm,
	char* filename,
	char* path,
	char* lookup_flags,
	int type) : GlobalParseurHMM(hmm, filename, path) {

	renumerotation = NON;
	premier = true;
	ajusteoffset = true;
	parse_table = NULL;
	initialisation_analyse_token = NON;
	initialisation_fst();

	token_file = strdup_vit(tokenfile);
	script_file = strdup_vit(script);
	Nom_divers.ajoute(DupliqueChaine(token_file));
	Nom_scripts.ajoute(DupliqueChaine(script_file));
	type_lookup = type;
	LOOKUP_ALGORITHM = type;
	nondet_limit = NONDET_LIMIT_DEFAULT;
	flags[0] = 0;
	if (lookup_flags != NULL)
		strcpy_s(flags,100,lookup_flags);

}

void GlobalParseurTokenLookup::LoadTokenLookup() {
	char* vecteur[] = { NULL, NULL };

	LK_OUT_CHAIN_DEPTH = LK_OUT_CHAIN_DEPTH_DEFAULT;

	if (flags[0] != 0) {
		vecteur[0] = "-flags";
		vecteur[1] = flags;
		set_lookup_flags(2, vecteur);
	}
	else
		set_lookup_flags(0, vecteur);

	//Dans tous les cas de figure, on se place en mode UTF8 pour preparer les tables internes
	init_string_functions(CHAR_ENC_UTF_8);
	READING_SCRIPT = TRUE;
	CUR_LINE_NUM = 0;
	LOOKUP_PARSE_TABLE = NULL;

	LEXNETS = NULL;
	STRATEGIES = NULL;

	read_script_get_networks("-f", script_file, this); /* read lexicon file */


	READING_SCRIPT = FALSE;

	reseaux_lexicaux = LEXNETS;
	strategies_lexicales = STRATEGIES;

	if ((LOOKUP_FLAGS.apply_with_check || LOOKUP_FLAGS.evaluate_check)
		&& (!LOOKUP_FLAGS.checknet_from_file))
		CHK_make_all_check_nets(LEXNETS);

	if (LOOKUP_FLAGS.vectorize_nets)
		vectorize_all_networks();


	/* Initialization step:
	make range maps for all nets whatever the algorithm maybe. */
	make_range_map_for_every_net(LOWER, type_lookup); /* *** it may be optimized .. but beware later usage */
	litupper(reseaux_lexicaux);
	select_result_function();
	select_match_tables(LOWER);

	make_id_vector_or_parse_table(LOWER);
	alloc_in_out_buffers();
	if (type_lookup == 1)
		allocate_alt_alg_memory(FALSE, NULL, NULL);

	make_lower_sigmas_all_networks();
	READING_INPUT = TRUE;
	CUR_LINE_NUM = 0;

	LOOKUP_INPUT_FILE = stdin;
	LOOKUP_OUTPUT_FILE = stdout;

	LOOKUP_SECONDS = time(NULL);

	if (!(token_fst = read_first_net(token_file)))
		/* Cannot read the transducer. Close open files. */
		erreur("Error while loading token file");
	
	init_strats_contr(); /* */
	str_sto = get_str_no();
	controle_strategies = get_strat_control();
	set_strat_control(NULL, 0);

	alloc_success_count();
	success_count = get_success_count();
	set_success_count(NULL);

	alphabet_bas = LOWER_SIGMAS_LEXNETS;
	LOWER_SIGMAS_LEXNETS = NULL;
	LEXNETS = NULL;
	STRATEGIES = NULL;
	READING_SCRIPT = FALSE;
	READING_INPUT = FALSE;
	my_parse_table = LOOKUP_PARSE_TABLE;
	ourflags = LOOKUP_FLAGS;

	nets.ajoute(token_fst);
	up_range.ajoute(NET_uprange_map(token_fst));
	mylookup_table = LOOKUP_CHAR_TABLE;
	look_char_vect = LOOKUP_CHAR_VECT;
	range_map = NULL;
}

void GlobalParseurTokenLookup::globalinit() {
	raz();
	dernieroffsetcar = 0;
	dernieroffset = 0;
	LOOKUP_FLAGS = ourflags;
	select_result_function();
	uppermap();
	LOOKUP_PARSE_TABLE = my_parse_table;
	LOWER_SIGMAS_LEXNETS = alphabet_bas;
	LOOKUP_CHAR_TABLE = mylookup_table;
	set_strat_control(controle_strategies, str_sto);
	set_success_count(success_count);
	LOOKUP_CHAR_VECT = look_char_vect;
	LEXNETS = reseaux_lexicaux;
	STRATEGIES = strategies_lexicales;
	LOOKUP_ALGORITHM = type_lookup;
	premier = true;
}

int GlobalParseurTokenLookup::tokenizestring(char* str,
	int DONT_VECTORIZE_SLOW,
	int use_doubled_config_elimination) {

	globalinit();
	return deterministic_tokenize(token_fst,
		NULL,
		str,
		(void*)this,
		&range_map,
		&my_parse_table,
		DONT_VECTORIZE_SLOW,
		use_doubled_config_elimination);
}

word_offset* localoffset;
int GlobalParseurTokenLookup::tokenizefile(FILE* in,
	int DONT_VECTORIZE_SLOW,
	int use_doubled_config_elimination) {

	globalinit();
	localoffset = &wo;
	return deterministic_tokenize(token_fst,
		in,
		NULL,
		(void*)this,
		&range_map,
		&my_parse_table,
		DONT_VECTORIZE_SLOW,
		use_doubled_config_elimination);
}

void GlobalParseurTokenLookup::print() {
	ostream* os = &cout;
	if (OS != NULL)
		os = OS;

	for (int i = 0; i < lectures.dernier; i++) {
		*os << lectures[i]->mot;
		if (lectures[i]->mot[lectures[i]->longueur - 1] != '\n')
			*os << endl;
	}
	*os << endl;
}

void GlobalParseurTokenLookup::appel(char* bufferinterne, long offset) {
	if (bufferinterne[0] == 10) {
		if (lookup() == OUI) {
			gBaliseDebut->raz();

			if (wbf != NULL) {
				exchange_vit_vectors(vtree);   /* prev <--> cur */
				clear_vit_vector_and_lists(vtree->cur, vtree);

				set_current_class_data(wbf, vtree);
				viterbi_next_word(TRUE);
			}
			retourchariot = NON;

		}
		raz();
	}
	else
		wo.ajoute(bufferinterne, offset, baseOffsetCar);
}

void GlobalParseurTokenLookup::litupper(NVptr strats) {
	for (int i = 0; i < NV_len(strats); i++) {
		NETptr net = NV_net(strats, i);
		nets.ajoute(net);
		up_range.ajoute(NET_uprange_map(net));
	}
}


void GlobalParseurTokenLookup::uppermap() {
	for (int i = 0; i < nets.dernier; i++)
		NET_uprange_map(nets[i]) = up_range[i];
}

void GlobalParseurTokenLookup::Ajusteoffset() {
	short df;
	ListeVoc* final;
	if (ajusteoffset == false)
		return;
	for (int offset = 0; offset < phrase.dernier; offset++) {
		final = phrase[offset];
		//on calcule les espaces intermediaires entre les mots pour reajuster les offsets en caracteres
		df = final->borneGauche - dernieroffset;
		final->borneGaucheCar = dernieroffsetcar;
		final->borneDroiteCar = dernieroffsetcar + size_c(final->surfacebrute);
		if (df != 0) {
			final->borneGaucheCar += df;
			final->borneDroiteCar += df;
			baseOffsetCar += df;
		}
		dernieroffset = final->borneDroite;
		dernieroffsetcar = final->borneDroiteCar;
	}
}

char GlobalParseurTokenLookup::lookup() {

	if (wo.gauche == -1 && wo.droite == -1 && wo.mot[0] == 0)
		return NON;
	if (MaxSentence != -1 && num_phrase > MaxSentence)
		return OUI;

	if (premier == true) {
		select_match_tables(LOWER);
		premier = false;
	}

	bool loop = true;
	int compteloop = 0;
	int originegauche = wo.gauche;
	int originegauchechar = wo.gauchecar;
	string lestraits;
	vector<string>* gras = NULL;
	int igras = 0;
	while (loop) {
		if (wo.longueur > taille_mot || gras != NULL) {
			if (compteloop == 0) {
				gras = new vector<string>;
				conversion_utf8_to_fat((uchar*)wo.mot, *gras);
			}
			string s;
			int nbcars = 0;
			for (; igras < gras->size() && s.size() < taille_mot; igras++, nbcars++)
				s += (*gras)[igras];
			if (s == "")
				break;
			else {
				if (compteloop != 0 && wbf != NULL) {
					exchange_vit_vectors(vtree);   /* prev <--> cur */
					clear_vit_vector_and_lists(vtree->cur, vtree);

					set_current_class_data(wbf, vtree);
					viterbi_next_word(TRUE);
				}
			}
			strcpy(wo.mot, STR(s));
			wo.gauche = originegauche;
			originegauche = wo.gauche + s.size();
			wo.droite = originegauche;
			wo.gauchecar = wo.gauche;
			wo.droitecar = wo.droite;
			compteloop++;
		}
		else
			loop = false;

		char* s = wo.mot;
		lookup_word(wo.mot, (void*)this);
		//Nous disposons donc de notre structure de lecture...
		borneGauche = wo.gauche;
		borneDroite = wo.droite;
		borneGaucheCar = borneGauche;
		borneDroiteCar = borneDroite;

		ostream* os = &cout;
		ListeVoc* voc = NULL;

		if (OS != NULL)
			os = OS;

		static char ch[100];
		char testFinDePhrase = NON;
		char testBalise = NON;

		char retourchariotlocal = NON;

		char ajouteFinLigne = NON;
		//chaque lecture correspond a un mot
		if (gAfficherResultats == OUI &&
			TestOptions(AFFICHAGE_OFFSET) &&
			appliqueGrammaire == NON && xipresult == NULL)
			*os << "[" << borneGauche << "-" << borneDroite << "]" << Endl;

		//en cas d'erreur
		if (lectures.dernier == 0) {
			//Nous creons une entree a la volee. basee sur la categorie donnee dans control.xip
			word_offset* w = fournirWordOffset();
			sprintf_s(ch, 100, "%s\t%s\t+%s+hmm_%s", wo.mot, wo.mot, TAG_NOUVEAU, TAG_NOUVEAU);
			w->concatene(ch);
		}


		for (int i = 0; i < lectures.dernier; i++) {
			s = lectures[i]->mot;

			//On suppose que nos lexiques internes sont en ISO-Latin
			//En UTF8, le resultat est aussi en UTF8, donc on a besoin d'une conversion
			//pour pouvoir interroger nos lexiques internes.

			uchar* pt = (uchar*)s;
			char chgt = NON;

			int nbcr = 0;
			while (*pt != 0) {
				if (*pt <= 31 && *pt != 9) {
					if (*pt == 10)
						nbcr++;
					*pt = ' ';
					chgt = OUI;
				}
				++pt;
			}

			char* ptt = NULL;
			if (retourchariotlocal == NON && retourchariot) {
				if (retourchariot <= 9)
					sprintf_s(ch,100, "+CR%d", retourchariot);
				else
					strcpy_s(ch,100, "+CRxN");

				ptt = strrchr(s, 9);
				if (ptt != NULL) {
					strcat(ch, ptt + 1);
					ptt[1] = 0;
					lectures[i]->longueur = ptt - s + 1;
					s = lectures[i]->concatene(ch);
				}
				else
					s = lectures[i]->concatene(ch);
			}

			//Dans le cas d'un retour chariot detecte, on regarde si l'on a juste un retour chariot (associe a ?+)
			if (retourchariotlocal == OUI) {
				ptt = strrchr(s, 9);
				if (ptt != NULL && ptt[1] == '+' && ptt[2] == '?') {
					retourchariot++;
					return NON;
				}
				//sinon, on desactive le retourchariot
				retourchariot = NON;
			}

			//Dans le cas d'un passage par le HMM, il faut garder la trace des offsets...
			if (wbf != NULL) {
				if (borneGauche == borneGaucheCar && borneDroite == borneDroiteCar)
					sprintf_s(ch,100, "+%d+%d", borneGauche, borneDroite);
				else
					sprintf_s(ch, 100, "+%d+%d+%d+%d", borneGauche, borneDroite, borneGaucheCar, borneDroiteCar);
				//On cherche les traits:
				ptt = strrchr(s, 9);
				if (ptt != NULL) {
					strcat(ch, ptt + 1);
					ptt[1] = 0;
					lectures[i]->longueur = ptt - s + 1;
					s = lectures[i]->concatene(ch);
				}
			}


			if (appliqueGrammaire == NON && xipresult == NULL) {
				ajouteFinLigne = OUI;
				num_mot++;
				nombre_mots++;
				if (gAfficherResultats == OUI)
					*os << s << Endl;
			}
			else {
				if (wbf == NULL) {
					if (testBalise == NON && TestBalise(s) == OUI)
						return NON;

					if (testBalise == NON)
						testFinDePhrase = EndOfSentence(s, NON);

					testBalise = OUI;
					char* copieChaine;
					LexiconEntryParsing(s, &copieChaine);
					if (voc == NULL) {
						voc = ConstruitForme(copieChaine, num_mot);
						if (voc != NULL) {
							voc->borneGauche = borneGauche;
							voc->borneDroite = borneDroite;
							voc->borneGaucheCar = borneGaucheCar;
							voc->borneDroiteCar = borneDroiteCar;
#ifdef XIPLIBXML
							voc->xmlnoeud = noeudxmltokencourrant;
#endif

							num_mot++;
							nombre_mots++;
						}
					}
					else
						FusionneForme(voc, copieChaine);
				}//wbf!=NULL
				else {
					int tag, num_tokens;

					if (testBalise == NON) {
						auto_vocabulaire* trad = RechercheBalise(s);
						pileDeMots.ajoute(trad);
						if (trad != NULL)
							return NON;
						testBalise = OUI;
					}

					analyse_input_line(s, &num_tokens, &tag);
					store_to_word_buffer(wbf, s, tag, (!i));
				}
			}
		}

		if (loop)
			nettoieLectures();

		if (wbf == NULL) {
			if (testFinDePhrase == OUI || EndOfSentence(NULL, UTILISE)) {
				ajusteoffset = true;
				Parse(OUI);
				Detruit();

				num_phrase++;

				if (renumerotation == OUI) {
					num_mot = 0;
					num_token = 0;
				}
				if (gras != NULL)
					delete gras;
				return OUI;
			}
		}

		if (ajouteFinLigne == OUI && gAfficherResultats == OUI)
			*os << Endl;
	}
	if (gras != NULL)
		delete gras;
	//LEXNETS=NULL;
	//STRATEGIES=NULL;
	return OUI;
}




void GlobalParseurTokenLookup::StartAnalysis() {

	if (initialisation_analyse_token == OUI)
		return;

	wbf = NULL;
	baseOffset = 0;
	baseOffsetCar = 0;

	if (hmmFileName != NULL && hmmFileName[0] != 0) {
		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = TRUE;

		wbf = alloc_word_buffer(MAX_WORDS, MAX_ALTBF, MAX_LINEBF);
		vtree = alloc_viterbi(TAG.num);
	}
	initialisation_analyse_token = OUI;
}

void GlobalParseurTokenLookup::CloseAnalysis() {
	if (initialisation_analyse_token == NON)
		return;

	if (wbf != NULL && wbf->num)   /* still words in buffer */
	{
		vtree->cur->flags.sent_end = TRUE;
		viterbi_next_word(FALSE);
	}

	FinAnalyse = OUI;
	ajusteoffset = true;
	while (phrase.dernier != 0) {
		Parse(OUI);
		Detruit();
		num_phrase++;
		ajusteoffset = false;
	}
	FinAnalyse = NON;
	//HMM lu
	if (wbf != NULL) {
		words_total = wbf->words_total;

		free_word_buffer(wbf);
		free_viterbi(vtree);

		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = FALSE;
	}
	initialisation_analyse_token = NON;
}


void GlobalParseurTokenLookup::AnalyseTokenFichier() {
	//Chaque ligne est un token
	static char buff[1000];
	while (!feof(fichierDonnees)){
		fgets(buff, 999, fichierDonnees);
		Trim(buff);
		if (buff[0])
			AddAToken((uchar*)buff);
	}
}

void GlobalParseurTokenLookup::AnalyseTokenChaine() {
	//Chaque ligne est un token
	static char buff[1000];
	char* debut = (char*)donneesATraiter;
	char* pt = strchr(debut, '\n');


	while (pt != NULL) {
		char trouve = NON;
		*pt = 0;
		if (pt[-1] == '\r') {
			pt[-1] = 0;
			trouve = OUI;
		}
		if (debut[0])
			AddAToken((uchar*)debut);
		*pt = '\n';
		if (trouve == OUI)
			pt[-1] = '\r';
		debut = pt + 1;
		pt = strchr(debut, '\n');
	}

	if (debut[0])
		AddAToken((uchar*)debut);
}


void GlobalParseurTokenLookup::AddAToken(uchar* debut) {
	StartAnalysis();
	wo.raz();
	wo.concatene((char*)debut);
	lookup();
}



TokLOOK::~TokLOOK() {
	free(script_file);
	LOWER_SIGMAS_LEXNETS = alphabet_bas;
	LEXNETS = reseaux_lexicaux;
	set_strat_control(controle_strategies, str_sto);
	LEXNETS = reseaux_lexicaux;
	STRATEGIES = strategies_lexicales;
	free_strats_control();
	set_strat_control(NULL, 0);

	set_success_count(success_count);
	free_success_count();
	set_success_count(NULL);

	if (reseaux_lexicaux && NV_len(reseaux_lexicaux) == 1) {
		if (LOWER_SIGMAS_LEXNETS != NULL && LEXNETS != NULL)
			free_lower_sigmas_all_networks();
	}
	else
		CHK_free_all_check_nets(reseaux_lexicaux);
	if ((!reseaux_lexicaux) || (!NV_len(reseaux_lexicaux)))
		return;

	free_nv_only(NVV_nv(strategies_lexicales, 0));
	free_nvv_only(strategies_lexicales);
	free_nv_only(reseaux_lexicaux);

	LOWER_SIGMAS_LEXNETS = NULL;
	STRATEGIES = NULL;
	LEXNETS = NULL;
}

void TokLOOK::litupper(NVptr strats) {
	for (int i = 0; i < NV_len(strats); i++) {
		NETptr net = NV_net(strats, i);
		nets.ajoute(net);
		up_range.ajoute(NET_uprange_map(net));
	}
}

void TokLOOK::uppermap() {
	selection_side = LOWER;
	for (int i = 0; i < nets.dernier; i++)
		NET_uprange_map(nets[i]) = up_range[i];
}

void TokLOOK::lowermap() {
	selection_side = UPPER;
	for (int i = 0; i < nets.dernier; i++)
		NET_uprange_map(nets[i]) = up_range[i];
}

TokLOOK::TokLOOK(GlobalParseur* p, char* script, char* flags, int type, char utf8mode) {
	initialisation_fst();
	taille_mot = 200;
	wo.realloue(taille_mot);
	LOOKUP_PARSE_TABLE = NULL;

	parseur = p;

	script_file = strdup_vit(script);
	type_lookup = type;
	nondet_limit = 1000;
	char* vecteur[] = { NULL, NULL };

	LOOKUP_ALGORITHM = type;
	LK_OUT_CHAIN_DEPTH = LK_OUT_CHAIN_DEPTH_DEFAULT;


	if (flags[0] != 0) {
		vecteur[0] = "-flags";
		vecteur[1] = flags;
		set_lookup_flags(2, vecteur);
	}
	else
		set_lookup_flags(0, vecteur);

	if (utf8mode == 1) {
		init_string_functions(CHAR_ENC_UTF_8);
		utf8lookup = 1;
	}
	else
		utf8lookup = 0;

	NET_FILES.num = 0;
	NET_FILES.file = NULL;
	TEMP_FILES.symbols = NULL;
	TEMP_FILES.netfile_names = NULL;
	TEMP_FILES.strategies = NULL;
	TEMP_DATA.num_nets = 0;
	TEMP_DATA.len_symbol_byffer = 0;
	TEMP_DATA.num_strategies = 0;
	TEMP_DATA.max_len_strategy = 0;

	READING_SCRIPT = TRUE;
	CUR_LINE_NUM = 0;
	read_script_get_networks("-f", script_file, parseur); /* read lexicon file */

	READING_SCRIPT = FALSE;


	if ((LOOKUP_FLAGS.apply_with_check || LOOKUP_FLAGS.evaluate_check)
		&& (!LOOKUP_FLAGS.checknet_from_file))
		CHK_make_all_check_nets(LEXNETS);

	vectorize_all_networks();

	reseaux_lexicaux = LEXNETS;
	strategies_lexicales = STRATEGIES;

	/* Initialization step:
	make range maps for all nets whatever the algorithm maybe. */
	make_range_map_for_every_net(LOWER, type); /* *** it may be optimized .. but beware later usage */
	litupper(reseaux_lexicaux);

	select_result_function();
	selection_side = BOTH_SIDES;
	select_match_tables(selection_side);

	make_id_vector_or_parse_table(selection_side);
	
	alloc_in_out_buffers();
	if (type_lookup == 1)
		allocate_alt_alg_memory(FALSE, NULL, NULL);

	make_lower_sigmas_all_networks();
	READING_INPUT = TRUE;
	CUR_LINE_NUM = 0;

	LOOKUP_INPUT_FILE = stdin;
	LOOKUP_OUTPUT_FILE = stdout;

	LOOKUP_SECONDS = time(NULL);

	init_strats_contr(); /* */
	str_sto = get_str_no();
	controle_strategies = get_strat_control();
	set_strat_control(NULL, 0);

	alloc_success_count();
	success_count = get_success_count();
	set_success_count(NULL);

	alphabet_bas = LOWER_SIGMAS_LEXNETS;
	LOWER_SIGMAS_LEXNETS = NULL;
	LEXNETS = NULL;
	STRATEGIES = NULL;
	READING_SCRIPT = FALSE;
	READING_INPUT = FALSE;
	parse_table = LOOKUP_PARSE_TABLE;
	strat_apply = STRATEGIES_APPLY_CONTEXT;
	mylookup_table = LOOKUP_CHAR_TABLE;
	look_char_vect = LOOKUP_CHAR_VECT;
	ourflags = LOOKUP_FLAGS;
}

TokLOOK* charge_un_fst(GlobalParseur* p, FILE* stream, string sfilename, bool utf8) {
	if (!(std_file_header_p(stream))) {
		string message = "FST(002): File cannot be loaded: ";
		message += sfilename;
		fclose(stream);
		p->erreur(STR(message));
	}
	initialisation_fst();
	NVptr nets = standard_nets_from_stream(stream);
	set_header_file_name(NULL, STR(sfilename));


	TokLOOK* tok = new TokLOOK(p, nets->nets[0], "", 0, utf8);
	return tok;
}

TokLOOK::TokLOOK(GlobalParseur* p, NETptr net, char* flags, int type, char utf8mode) {
	initialisation_fst();
	taille_mot = 200;
	wo.realloue(taille_mot);
	LOOKUP_PARSE_TABLE = NULL;
	parseur = p;

	type_lookup = type;
	nondet_limit = 1000;
	char* vecteur[] = { NULL, NULL };
	LOOKUP_ALGORITHM = type;
	LK_OUT_CHAIN_DEPTH = LK_OUT_CHAIN_DEPTH_DEFAULT;

	if (flags[0] != 0) {
		vecteur[0] = "-flags";
		vecteur[1] = flags;
		set_lookup_flags(2, vecteur);
	}
	else
		set_lookup_flags(0, vecteur);


	if (utf8mode == 1) {
		utf8lookup = utf8mode;
		init_string_functions(CHAR_ENC_UTF_8);
	}
	else
		utf8lookup = 0;

	READING_SCRIPT = FALSE;

	reseaux_lexicaux = make_nv(1);
	NV_net(reseaux_lexicaux, 0) = net;
	strategies_lexicales = make_nvv(1);
	NVV_nv(strategies_lexicales, 0) = make_nv(1);
	NV_net(NVV_nv(strategies_lexicales, 0), 0) = NV_net(reseaux_lexicaux, 0);
	LEXNETS = reseaux_lexicaux;
	STRATEGIES = strategies_lexicales;

	//Si on charge un seul automate, la vectorisation n'est pas une bonne idee... 
	//En particulier, cela rend les operations sur les automates incontrolables...
	//vectorize_all_networks ();     

	/* Initialization step:
	make range maps for all nets whatever the algorithm maybe. */
	make_range_map_for_every_net(LOWER, type); /* *** it may be optimized .. but beware later usage */
	litupper(reseaux_lexicaux);

	script_file = strdup_vit("NULL");

	select_result_function();
	selection_side = BOTH_SIDES;
	select_match_tables(selection_side);

	make_id_vector_or_parse_table(selection_side);	
	alloc_in_out_buffers();
	if (type_lookup == 1)
		allocate_alt_alg_memory(FALSE, NULL, NULL);

	make_lower_sigmas_all_networks();
	READING_INPUT = TRUE;
	CUR_LINE_NUM = 0;

	LOOKUP_INPUT_FILE = stdin;
	LOOKUP_OUTPUT_FILE = stdout;

	LOOKUP_SECONDS = time(NULL);

	init_strats_contr();
	str_sto = get_str_no();
	controle_strategies = get_strat_control();
	set_strat_control(NULL, 0);

	alloc_success_count();
	success_count = get_success_count();
	set_success_count(NULL);


	alphabet_bas = LOWER_SIGMAS_LEXNETS;
	LOWER_SIGMAS_LEXNETS = NULL;
	LEXNETS = NULL;
	STRATEGIES = NULL;
	READING_SCRIPT = FALSE;
	READING_INPUT = FALSE;
	parse_table = LOOKUP_PARSE_TABLE;
	strat_apply = STRATEGIES_APPLY_CONTEXT;
	mylookup_table = LOOKUP_CHAR_TABLE;
	look_char_vect = LOOKUP_CHAR_VECT;
	ourflags = LOOKUP_FLAGS;
}

void TokLOOK::up(char* str) {
	wo.raz();
	uppermap();
	int ty = type_lookup;
	type_lookup = 0;
	look(str);
	type_lookup = ty;
}

void TokLOOK::down(char* str) {
	wo.raz();
	lowermap();
	int ty = type_lookup;
	type_lookup = 4;
	look(str);
	type_lookup = ty;
}

void TokLOOK::look(char* str) {
	//le mot est dans wo.mot
	if (LEXNETS != reseaux_lexicaux) {
		LOOKUP_FLAGS = ourflags;
		select_result_function();
		STRATEGIES_APPLY_CONTEXT = strat_apply;
		LOOKUP_CHAR_TABLE = mylookup_table;
		LOOKUP_PARSE_TABLE = parse_table;
		LOWER_SIGMAS_LEXNETS = alphabet_bas;
		LOOKUP_CHAR_VECT = look_char_vect;
		LEXNETS = reseaux_lexicaux;
		STRATEGIES = strategies_lexicales;
		LOOKUP_ALGORITHM = type_lookup;
		select_match_tables(selection_side);
		set_strat_control(controle_strategies, str_sto);
		set_success_count(success_count);
	}

	if (utf8lookup == 1)
		init_string_functions(CHAR_ENC_UTF_8);
	else
		init_string_functions(CHAR_ENC_ISO_8859_1);
	lookup_word(str, (void*)parseur);
	if (utf8tokenizer == 1)
		init_string_functions(CHAR_ENC_UTF_8);
	else
		init_string_functions(CHAR_ENC_ISO_8859_1);
	LEXNETS = NULL;
	STRATEGIES = NULL;
}

#endif


