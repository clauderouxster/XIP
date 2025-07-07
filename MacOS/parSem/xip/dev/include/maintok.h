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
   filename   : maintok.h
   Date       : 21/01/2002
   Purpose    : Encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef maintok_h
#define maintok_h


class GlobalParseurTokenLookup : public GlobalParseurHMM {
public:
    
    NETptr token_fst;
    char *token_file;
    char *script_file;
    int type_lookup;

	int str_sto;
    NVptr reseaux_lexicaux;
	VECTA<RANGE_RECORD*> up_range;
    NVVptr strategies_lexicales;
    char flags[100];
	lower_sigma_net* alphabet_bas;
	void* controle_strategies;
	long* success_count;
	PARSE_TBL my_parse_table;
	PARSE_TBL parse_table;
	_LkFlags ourflags;
	VECTA<NETptr> nets;
	ALPHABETptr mylookup_table;
	uint16* look_char_vect;
	bool premier;
	RANGEptr range_map;
	long dernieroffsetcar;
	long dernieroffset;
	void Ajusteoffset();
	bool ajusteoffset;
    GlobalParseurTokenLookup(char* fst,char* script,
        char* hmm, 
        char* filename, 
        char* path, 
        char* lookup_flags,
        int type);
    ~GlobalParseurTokenLookup();
    void appel(char* internal,long offset);
    int tokenizestring(char* string,int DONT_VECTORIZE_SLOW,int use_doubled_config_elimination);
    int tokenizefile(FILE* file,int DONT_VECTORIZE_SLOW,int use_doubled_config_elimination);    
    void print();
    char lookup();
    void StringAssociation(unsigned char* chaine);
    FILE* FileAssociation(FILE* fichier);
    void SentenceAnalysis(char renumerote,char arbre);
    void LoadTokenLookup();
    virtual typeParseur Type() {return PARSEURTOKENLOOK;}
    void AddAToken(unsigned char* tok);    
    void StartAnalysis();
    void CloseAnalysis();
    void AnalyseTokenFichier();
    void AnalyseTokenChaine();
    char ChargementFichierScript();
	void litupper(NVptr);
	void uppermap();
	void globalinit();
	void DeclencheAjusteoffset(bool v) {ajusteoffset=true;}
};


void initialisation_fst();
void relacher_memoire_fst();

class TokLOOK {
public:

	_LkFlags ourflags;
	int taille_mot;
	char *script_file;
	int type_lookup;
	int str_sto;

	NVptr reseaux_lexicaux;
	NVVptr strategies_lexicales;
	char utf8tokenizer;
	char utf8lookup;
	int selection_side;
	VECTA<RANGE_RECORD*> up_range;
	VECTA<NETptr> nets;
	lower_sigma_net* alphabet_bas;
	void* controle_strategies;
	long* success_count;
	PARSE_TBL parse_table;
	APPLYptr* strat_apply;
	ALPHABETptr mylookup_table;
	uint16* look_char_vect;
	ALPHABETptr lookup_id_table;
	GlobalParseur* parseur;

	TokLOOK(GlobalParseur*,char* script_file,char* flags,int type,char utf8mode);
	TokLOOK(GlobalParseur*,NETptr,char* flags,int type,char utf8mode);
	~TokLOOK();
	void litupper(NVptr);
	void uppermap();
	void lowermap();
	word_offset wo;

	void appel(uchar* internal,long offset);
	void raz();
	void down(char*);
	void up(char*);
	void look(char* str);
};

#endif
