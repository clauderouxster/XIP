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
   filename   : ntmhmm.h
   Date       : 10/09/1999
   Purpose    : Derivation of base class GlobalParseur to use NTM and HMM
   Programmer : Claude ROUX
   Reviewer   : 
*/

/*
*  Copyright (c) 1999,2004 Xerox Corporation,
*  project XIP (Xerox Incremental Parser)
*  Xerox Research Centre Europe
*  All Rights Reserved
*  Copyright protection claimed includes all forms and matters of copyrightable
*  material and information now allowed by statutory or judicial law or
*  hereinafter granted, including without limitation, material generated from
*  the software programs which are displayed on the screen such as icons,
*  screen display looks, etc. All rights reserved. No part of this document
*  may be reproduced, stored in a retrieval system, or transmitted, in any
*  form or by any means, electronic, mechanical, photocopied, recorded, or
*  otherwise, without prior written consent from XEROX corporation.
*
* Programmer : Claude ROUX
*/


/**
 * Derivation of base class GlobalParseur to use NTM and HMM
 */

#ifndef ntmhmm_h
#define ntmhmm_h

const int longueurTableau=5000;

extern "C" {
#include "hmmtypes.h"
#include "memoire.h"
}

#ifdef NTMCONNECTION
extern "C" {
#include "fst/struct.h"
#include "fst/context.h"
#include "fst/pars-tbl.h"

#include "ntm.h"
#include "applyloc.h"
}
#endif

//Classe De traitement des hmm
class GlobalParseurHMM : public GlobalParseur {
 public:
    WORDBFptr wbf; 
    VTREEptr vtree; 
    long words_total;
    char* hmmFileName;    
    char retourchariot;

    VECTA<auto_vocabulaire*> pileDeMots;


//Variables de traitement HMM
    T_DISAMB_FLAGS DISAMB_FLAGS;
    T_HMM HMM;
    TAG_HASH_TYPE_HMM TAG;
    CLASS_HASH_TYPE_HMM CLASS;

    char initialisation_analyse_token;
    char entree_est_chaine;


    GlobalParseurHMM(char* hmm,char* filename,char* path);
    virtual ~GlobalParseurHMM();

    void output_all_prev_words (long idx,VCELLptr vcell);

    void viterbi_next_word(int next_p);

    void print_one_word_one_tag(INPWORDptr word, 
                                short tag);

    void print_one_word (long idx, 
                         VCELLptr vcell);

    int mot_suivant();
	VECTA<char*> listedehmmcat;

    virtual void SentenceAnalysis(char renumerote,char tree);
    virtual void AddAToken(unsigned char* tok) {}
    char ParseUneLigne(char* ligne,ListeVoc** voc,char supplementaire);
    int InitDisambText (char *hmmdata);
    void CloseHMM();


    //Methodes locales du HMM
    void set_disamb_flags (char *argflags);
    void error_message (const char *msg, ...);
    void  work_message (char *msg, ...);


    //Lecture du hmm
    void read_hmm_file (char *filename);
    double HMM_read_double (FILE* file);
    char* HMM_read_line (char *line, FILE* file);
    char* put_tags_to_buffer (FILE *file, int num_tags);
    void store_tags (int num_tags);
    void read_tags (FILE *file, int num_tags);
    char* put_out_tags_to_buffer (FILE *file);
    void store_out_tags (int num_tags);
    void read_out_tags (char *filename, int num_tags);
    short* put_classes_to_buffer (FILE *file, int num_tags, int num_classes);
    void store_classes (int num_classes);
    void read_classes (FILE *file, int num_tags, int num_classes);
    void read_store_Pi (FILE *file);
    void read_store_A (FILE *file);
    void read_store_B (FILE *file);
    long HMM_read_long (FILE* file);

    //Methodes de HASH.c    
    int calculate_tag_hash_index (unsigned char *key);  
    void hash_insert_tags ();
    short hash_find_tag (const char *key);
    int calculate_class_hash_index (short *key);
    void hash_insert_classes ();
    short hash_find_class (short *key);
    void delete_alltag_list (ATGCELLptr atgcell, VTREEptr vtree);
    void delete_vcell_list (VCELLptr vcell, VTREEptr vtree);
    void clear_vit_vector_and_lists (VITPOSptr vec, VTREEptr vtree);
    void exchange_vit_vectors (VTREEptr vtree);
    void set_current_class_data (WORDBFptr wbf, VTREEptr vtree);
    double calculate_path_score (VTREEptr vtree, short prev, short cur);
    void connect_cur_to_prev(VTREEptr vtree, short prev_pos, short cur_pos, double score);
    ATGCELLptr list_all_tags (VTREEptr vtree, VITPOSptr vec);
    void viterbi_start (VTREEptr vtree);
    short evaluate_prev_tags (VTREEptr vtree, short cur);
    void viterbi_step_best_tag (VTREEptr vtree);
    void viterbi_step_all_tags (VTREEptr vtree);
    void viterbi_step (VTREEptr vtree);
    void viterbi_end (VTREEptr vtree);
    void alloc_tags (int num_tags);
    void free_tags ();     
    void alloc_classes (int num_classes);
    void free_classes ();
    void alloc_hmm_basic ();
    void free_hmm_basic ();
    char* get_tag_string (char *full_tag);
    int get_next_word (FILE *file, WORDBFptr wbf);
    void analyse_input_line (char *line, int *num_tokens, int *tag);
    void free_all_mem ();
    void read_hmm_file_header (FILE *file,short show_comments);
    void HMM_write_long (FILE* file, long val);
    void delete_words (WORDBFptr wbf, long num);
    void store_to_word_buffer(WORDBFptr wbf, char *line, int tag, int new_word_p);
    virtual typeParseur Type() {return PARSEURHMM;}
	void ListeCatHMMAjouter(char* entree);
    virtual void StartAnalysis() {}
    virtual void CloreAnalyse() {}
	void ParseFinal();
	void razpile() {
		pileDeMots.raz();
	}
	void libereWord(word_offset*);
};

#ifdef NTMCONNECTION
//--------------------------------------------------------
class GlobalParseurNTM : public GlobalParseurHMM {
 public:

    char* ntmFileName;
	char ChaineEntree[longueurTableau];

    NtmContext ntmContexte;
    //static FST_CNTXTptr cntxt;
    static int compteur;
    char ntmProcessToken(char* s,NtmToken* token);    
    GlobalParseurNTM(char* ntm,char* hmm,char* filename,char* path);
    ~GlobalParseurNTM();
    void NTMLoadScript();

    void InitialisationParametres(char* parametres);

    void SentenceAnalysis(char renumerote,char tree);
    
    void Apply();
    

    void StringAssociation(unsigned char* chaine);
    FILE* FileAssociation(FILE* fichier);

    virtual typeParseur Type() {return PARSEURNTM;}
    void AddAToken(unsigned char* tok);    
    void StartAnalysis();
    void CloseAnalysis();
    void AnalyseTokenFichier();
    void AnalyseTokenChaine();
    char ChargementFichierScript();
};
#endif
#endif
