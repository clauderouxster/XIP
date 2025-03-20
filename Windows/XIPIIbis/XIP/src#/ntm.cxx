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
   Module     : ntm.cxx
   Date       :
   Purpose    : NTM encapsulation
   Programmer : Claude ROUX
   Reviewer   : 

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
*/

/**
 * Interface between XIP and NTM (through HMM)
 */

#ifdef NTMCONNECTION

#include "parsbin.h"
#include "ntmhmm.h"
extern "C" {
#include "fst/struct.h"
#include "fst/context.h"
#include "fst/pars-tbl.h"
#include "fst/modify.h"
#include "fst/cfsm.h"
#include "ntm.h"


//Test de la langue

    void ntmReset(NtmContext* ct);
    void resetNtmMemory(NtmMemory * mem);
    void NormalisationNomFichier(char* nomFichier,char* tampon);
}

void MettreEnMinuscule(unsigned char* iterateur);
void Trim(char* chaine);
//static const int longueurTableau=5000;


FST_CNTXTptr cntxt=NULL;

int GlobalParseurNTM::compteur=0;

void initialisation_fst() {
    if (cntxt!=NULL)
        return;
    cfsm_init();
    cntxt = (FST_CNTXTptr)get_default_context();
}


void force_initialisation_fst() {
	if (cntxt!=NULL) {
		NOT_INITIALIZED_P = TRUE;
		cfsm_init();
		cntxt = (FST_CNTXTptr)get_default_context();
	}
}


extern char modeOutC;
/*===========================================================*/

Exportation void XipNtmVersion(char* str) {
    strcpy(str,ntmWhoami());
}

char GlobalParseurNTM::ntmProcessToken(char* s,NtmToken* token) {


    int ind=0, i=0;
    NtmEntry *e=NULL;
    unsigned char* pt;
    ListeVoc* voc=NULL;
    int maxlong=longueurTableau;
    char ajouteFinLigne=NON;

    ostream* os=&cout;

    if (OS!=NULL)
        os=OS;

    //si le mot comporte un accent et que nous sommes dans une
    //situation de conversion utf8, alors il faut que la baseoffset
    //soit modifiee.
    int bgreel;
    int bdreel;
    
    if (TestOptions(ENTREE_UTF8)) {
        bgreel=token->initBytepos;	
        bdreel=bgreel+token->inputByteLength;
    }
    else {
        bgreel=token->initpos;
        bdreel=bgreel+token->inputLength;
	}
	
	borneGaucheCar=token->initpos+baseOffsetCar;
	borneDroiteCar=token->initpos+token->inputLength+baseOffsetCar;            
    
    borneGauche=bgreel+baseOffset;
    borneDroite=bdreel+baseOffset;
	    
	if (gAfficherResultats==OUI &&
        TestOptions(AFFICHAGE_OFFSET) && 
        appliqueGrammaire==NON && xipresult==NULL)
        *os<<"["<<borneGauche<<"-"<<borneDroite<<"]"<<Endl;

    char testFinDePhrase=NON;
    char testBalise=NON;
    
    s[0]=0;
    char retourchariotlocal=NON;
    
    for(i=0, e = token->entrySeq; i < token->entryNb; i++, e++) {

        //ind=ntmSnprintIdSeq(s, maxlg, e->normform);

        ind += snprintInputForm(&ntmContexte.levels, s+ind, maxlong-ind-5, token);

        if (ind==0)
			return NON;

        s[ind]=0;
        pt=(uchar*)s;
        char chgt=NON;

        //cas particulier, on rencontre juste un retour chariot
        if (s[ind-1]==10)
            retourchariotlocal=OUI;
    
        int nbcr=0;
        while ( *pt!=0) {
            if (*pt<=31) {
                if (*pt==10)
                    nbcr++;
                *pt=' ';
                chgt=OUI;
            }
            ++pt;
        }

        //Nous avons rajoute cette ligne pour gerer les problemes liees au codage UTF8
        /*
        if (!i && TestOptions(ENTREE_UTF8) && chainetraductionbrute!=NULL) {
            //On verifie s'il n'y pas de pb de realignement
            //dans chainetraductionbrute nous avons la valeur UNICODE correspondante
            //Nous verifions si aucun des caracteres dans chainertraductionbrute n'est superieur
            //a 128 (ce qui implique un double octets)
            int ll;
            int accent=0;
            //On calcule le nombre de caracteres superieurs a 1 present dans la chaine
            for (ll=bgreel;ll<bdreel;ll++)
                accent+=chainetraductionbrute[ll];            
            baseOffset+=accent;
            borneDroite+=accent;
        }
        */

        if (ind < maxlong) s[ind++] = '\t';
        int pos=ind;
        ind += ntmSnprintIdSeq(s+ind, maxlong-ind-5, e->lemma);
        
        if (chgt==OUI) {
            s[ind]=0;
            pt=(uchar*)s+pos;
            
            while ( *pt!=0) {
                if (*pt<=31)
                    *pt=' ';
                ++pt;
            }
        }

        if (ind < maxlong) s[ind++] = '\t';

        //Dans le cas d'un passage par le HMM, il faut garder la trace des offsets...
        if (wbf!=NULL) {
            static char ch[100];
			if (borneGauche==borneGaucheCar && borneDroite==borneDroiteCar)
				sprintf(ch,"+%d+%d",borneGauche,borneDroite);
			else
				sprintf(ch,"+%d+%d+%d+%d",borneGauche,borneDroite,borneGaucheCar,borneDroiteCar);
            memcpy(s+ind,ch,strlen(ch));
            ind += strlen(ch);
        }

		if (retourchariotlocal==NON && retourchariot) {
			if (retourchariot<=9) {            
				sprintf(s+ind,"+CR%d",retourchariot);
				ind+=4;
			}
			else {
				memcpy(s+ind,"+CRxN",7);
				ind+=5;
			}
		}                

        int prevind=ind;
        ind += ntmSnprintIdSeq(s+ind, maxlong-ind-5, e->features);
        
        //Dans le cas d'un retour chariot detecte, on regarde si l'on a juste un retour chariot (associe a ?+)
        if (retourchariotlocal==OUI) {
            //Oui, dans ce cas on retourne sans rien modifier
			if (s[prevind]=='+' && s[prevind+1]=='?') {
				retourchariot++;
                return NON;
			}
            //sinon, on desactive le retourchariot
            retourchariot=NON;
        }
                

        if (ind < maxlong)
            s[ind]=0;
        else
            s[maxlong-1]=0;


        if (appliqueGrammaire==NON && xipresult==NULL) {
            ajouteFinLigne=OUI;
			num_mot++;
			nombre_mots++;
			if (gAfficherResultats==OUI)
                *os<<s<<Endl;
        }
        else {
            if (wbf==NULL) {
                if (testBalise==NON && TestBalise(s)==OUI)
                    return NON;

                if (testBalise==NON)
                    testFinDePhrase=EndOfSentence(s,NON);
                
                testBalise=OUI;
                char* copieChaine;
                LexiconEntryParsing(s,&copieChaine);
                if (voc==NULL) {
                    voc=ConstruitForme(copieChaine,num_mot);
                    if (voc!=NULL) {
                        voc->borneGauche=borneGauche;
                        voc->borneDroite=borneDroite;
                        voc->borneGaucheCar=borneGaucheCar;
                        voc->borneDroiteCar=borneDroiteCar;
#ifdef XIPLIBXML
						voc->xmlnoeud=noeudxmltokencourrant;
#endif
                        num_mot++;
                        nombre_mots++;
                    }
                }
                else
                    FusionneForme(voc,copieChaine);
            }//wbf!=NULL
            else {                
                int tag,num_tokens;

                if (testBalise==NON) {
                    auto_vocabulaire* trad=RechercheBalise(s);
                    pileDeMots.ajoute(trad);
                    if (trad!=NULL)
                        return NON;
                    testBalise=OUI;
                }
                
                analyse_input_line (s, &num_tokens, &tag);
                store_to_word_buffer (wbf, s, tag, (! i));
            }
        }
        s[0]=0;
        ind=0;
    }
    
    if (wbf==NULL) {
        if (testFinDePhrase==OUI || EndOfSentence(NULL,UTILISE)) {
            Parse(OUI);
            Detruit();
            
            num_phrase++;
            
			if (renumerotation==OUI) {
                num_mot=0;
				num_token=0;
			}
            return OUI;
        }
    }
 
	if (ajouteFinLigne==OUI && gAfficherResultats==OUI) {
        *os<<Endl;
		os->flush();
		if (modeOutC==OUI) {
			printf(((ostringstream*)os)->str().c_str());
			fflush(stdout);
			delete OS;
			OS=new ostringstream;
		}
	}
    return OUI;
}


void GlobalParseurNTM::StringAssociation(unsigned char* chaine) {
	if (TestOptions(TEST_INPUT_UTF8))
		TestUtf8Chaine(chaine);

    donneesATraiter=chaine;
    entree_est_chaine=OUI;
    if (TestOptions(EXECUTE_TOKENIZE))
        return;

	if (TestOptions(ENTREE_EN_MINUSCULE))
		MettreEnMinuscule(donneesATraiter);

    if (TestOptions(ENTREE_UTF8)) {        
		encodage_source=UTF8_ENCODING;
        //ntmSetInputTextString(&ntmContexte,conversionUTF8VersAscii((unsigned char*)chaine));
        //init_string_functions(CHAR_ENC_ISO_8859_1);
        ntmSetPrintByteIndices(&ntmContexte,1);
        ntmSetInputUTF8String(&ntmContexte,chaine);
        init_string_functions(CHAR_ENC_UTF_8);
    }
    else {        
		encodage_source=LATIN_ENCODING;
        ntmSetPrintByteIndices(&ntmContexte,0);
        ntmSetInputTextString(&ntmContexte,chaine);
        init_string_functions(CHAR_ENC_ISO_8859_1);
    }
}

FILE* GlobalParseurNTM::FileAssociation(FILE* fichier) {
    fichierDonnees=fichier;
	if (TestOptions(TEST_INPUT_UTF8))
		TestUtf8Fichier(fichier);

    entree_est_chaine=NON;
    if (TestOptions(EXECUTE_TOKENIZE))
        return fichierDonnees;

    if (TestOptions(ENTREE_UTF8)) {
		encodage_source=UTF8_ENCODING;
        ntmSetPrintByteIndices(&ntmContexte,1);
        ntmSetInputUTF8Stream(&ntmContexte, fichierDonnees);
        init_string_functions(CHAR_ENC_UTF_8);
    }
    else {
		encodage_source=LATIN_ENCODING;
        ntmSetPrintByteIndices(&ntmContexte,0);
        ntmSetInputTextStream(&ntmContexte,fichierDonnees);
        init_string_functions(CHAR_ENC_ISO_8859_1);
    }
	return fichierDonnees;
}

void GlobalParseurNTM::AddAToken(unsigned char* tok) {
    StartAnalysis();

    NtmToken* token;
    retourchariot=NON;
    //char chaine[longueurTableau];

    if (TestOptions(ENTREE_UTF8))
        token=ntmAnalyzeThisTokenUTF8(&ntmContexte,tok);
    else
        token=ntmAnalyzeThisToken(&ntmContexte, tok);

    if (MaxSentence!=-1 && num_phrase>MaxSentence)
        return;

    if (ntmWarning(&ntmContexte)) {
        fprintf(stderr, "\n####%s\n", ntmMessage(&ntmContexte));
        ntmResetWarning(&ntmContexte);
    }

    if (!ntmError(&ntmContexte)) {
        if (!token->ignore) {
            if (ntmProcessToken(ChaineEntree,token)==OUI) {
//				if (phrase.fin()!=NULL) {
//                    baseOffset=phrase.fin()->borneDroite+1;
//					baseOffsetCar=phrase.fin()->borneDroiteCar+1;
//				}
                gBaliseDebut->raz();
                
                if (wbf!=NULL) {
                    exchange_vit_vectors (vtree);   /* prev <--> cur */
                    clear_vit_vector_and_lists (vtree->cur, vtree);
                    
                    set_current_class_data (wbf, vtree);
                    viterbi_next_word (TRUE);
                }    
                retourchariot=NON;
            }
        }
    }    
}


void GlobalParseurNTM::Apply() {

    NtmToken *token=NULL, *t=NULL;
    //char chaine[longueurTableau];
    retourchariot=NON;

    token = ntmReadTokens(&ntmContexte);
    while ( !ntmError(&ntmContexte) && !ntmIsEofToken(token) ) {
        
        if (MaxSentence!=-1 && num_phrase>MaxSentence)
            break;


        if (ntmWarning(&ntmContexte)) {
            fprintf(stderr, "\n####%s\n", ntmMessage(&ntmContexte));
            ntmResetWarning(&ntmContexte);
            /* exit(1); */
        }

        if (!token->ignore) {
            if (ntmProcessToken(ChaineEntree,token)==OUI) {
                gBaliseDebut->raz();

                if (wbf!=NULL) {
                    exchange_vit_vectors (vtree);   /* prev <--> cur */
                    clear_vit_vector_and_lists (vtree->cur, vtree);
                    
                    set_current_class_data (wbf, vtree);
					viterbi_next_word (TRUE);
                }    
                retourchariot=NON;
            }
        }
        
        token = ntmReadTokens(&ntmContexte);
    }

    if (ntmError(&ntmContexte)) {
        fprintf(stderr, "\n%s\n\n", ntmMessage(&ntmContexte));
    }
}

void GlobalParseurNTM::StartAnalysis() {    

    if (initialisation_analyse_token==OUI)
        return;

    wbf=NULL; 
    baseOffset=0;
	baseOffsetCar=0;

    if (hmmFileName!=NULL && hmmFileName[0]!=0) {
        DISAMB_FLAGS.system.cur_line_num = 0;
        DISAMB_FLAGS.system.reading_hmm = FALSE;
        DISAMB_FLAGS.system.reading_input = TRUE;
    
        wbf = alloc_word_buffer (MAX_WORDS, MAX_ALTBF, MAX_LINEBF);
        vtree = alloc_viterbi (TAG.num);
    }
    initialisation_analyse_token=OUI;
}

void GlobalParseurNTM::CloseAnalysis() {
    if (initialisation_analyse_token==NON)
        return;

    if (wbf!=NULL && wbf->num)   /* still words in buffer */
    {
        vtree->cur->flags.sent_end = TRUE;
        viterbi_next_word (FALSE);
    }

    FinAnalyse=OUI;
    while (phrase.dernier!=0) {       
        Parse(OUI);
        Detruit();
        num_phrase++;
    }

    FinAnalyse=NON;
    //HMM lu
    if (wbf!=NULL) {
        words_total = wbf->words_total;

        free_word_buffer (wbf);
        free_viterbi (vtree);
		wbf=NULL;
		vtree=NULL;

        DISAMB_FLAGS.system.cur_line_num = 0;
        DISAMB_FLAGS.system.reading_hmm = FALSE;
        DISAMB_FLAGS.system.reading_input = FALSE;
    }
    initialisation_analyse_token=NON;
}


void GlobalParseurNTM::AnalyseTokenFichier() {
    //Chaque ligne est un token
    static char buff[1000];
    while (!feof(fichierDonnees)){
        fgets(buff,999,fichierDonnees);
        Trim(buff);
        if (buff[0])
            AddAToken((uchar*)buff);
    }
}

void GlobalParseurNTM::AnalyseTokenChaine() {
    //Chaque ligne est un token
    char* debut=(char*)donneesATraiter;
    char* pt=strchr(debut,'\n');
    
    
    while (pt!=NULL) {
        char trouve=NON;
        *pt=0;                
        if (pt[-1]=='\r') {
            pt[-1]=0;   
            trouve=OUI;
        }
        if (debut[0])
            AddAToken((uchar*)debut);
        *pt='\n';
        if (trouve==OUI)
            pt[-1]='\r';
        debut=pt+1;
        pt=strchr(debut,'\n');
    }

    if (debut[0])
        AddAToken((uchar*)debut);
}

void GlobalParseurNTM::SentenceAnalysis(char renumerote,char arbre) {
	num_phrase=0;
    wbf=NULL; 
    baseOffset=0;
	baseOffsetCar=0;
    renumerotation=renumerote;

    if (TestOptions(EXECUTE_TOKENIZE)) {
        if (entree_est_chaine==-1)
            return;
        if (entree_est_chaine==OUI)
            AnalyseTokenChaine();
        else
            AnalyseTokenFichier();    
        entree_est_chaine=-1;
    }

    //Dans un mode d'analyse par token, on ne passe par Apply
    if (initialisation_analyse_token==OUI) {
        CloreAnalyse();
        return;
    }


    if (hmmFileName!=NULL && hmmFileName[0]!=0) {
        DISAMB_FLAGS.system.cur_line_num = 0;
        DISAMB_FLAGS.system.reading_hmm = FALSE;
        DISAMB_FLAGS.system.reading_input = TRUE;
    
        wbf = alloc_word_buffer (MAX_WORDS, MAX_ALTBF, MAX_LINEBF);
        vtree = alloc_viterbi (TAG.num);
    }
    
    Apply();

    if (wbf!=NULL && wbf->num)   /* still words in buffer */
    {
        vtree->cur->flags.sent_end = TRUE;
        viterbi_next_word (FALSE);
    }

    FinAnalyse=OUI;
    while (phrase.dernier!=0) {       
        Parse(OUI);
        Detruit();
        num_phrase++;
    }
    FinAnalyse=NON;
    //HMM lu
    if (wbf!=NULL) {
        words_total = wbf->words_total;

        free_word_buffer (wbf);
        free_viterbi (vtree);
		wbf=NULL;
		vtree=NULL;

        DISAMB_FLAGS.system.cur_line_num = 0;
        DISAMB_FLAGS.system.reading_hmm = FALSE;
        DISAMB_FLAGS.system.reading_input = FALSE;
    }

//  return (words_total);
}


GlobalParseurNTM::~GlobalParseurNTM() {

    GlobalParseurNTM::compteur--;
    ntmResetStrategies(&ntmContexte);
    
    ntmClearInputObject(&ntmContexte);
    //free_default_context((FST_CNTXTptr)get_default_context());
    //seule la toute premiere instance peut detruire le contexte de CFSM
    //if (GlobalParseurNTM::compteur==0) {
    //    release_fsm_memory();     
    //    unset_default_context();
    //}
    //cfsm_free();
}

void GlobalParseurNTM::NTMLoadScript() {

    ChargementFichierScript();
	//ntmLoadScript(&ntmContexte,ntmFileName);
    if (ntmError(&ntmContexte)) {
        NOM="NTM INITIALISATION PROCEDURE";        
        erreur(ntmMessage(&ntmContexte));        
    }         
}

GlobalParseurNTM::GlobalParseurNTM(char* ntm,char* hmm,char* grammar,char* path) : 
GlobalParseurHMM(hmm,grammar,path) {
    
    entree_est_chaine=-1;
    ntmFileName=ntm;    
    
    renumerotation=NON;
    initialisation_fst();
	init_string_functions(CHAR_ENC_ISO_8859_1);
    ntmInit(&ntmContexte);
    ntmCopyright();    
    //On compte le nombre d'instance de GlobalParseurNTM construite (compteur est une
    //variable de classe)
    GlobalParseurNTM::compteur++;
    ntmSetVectorizeNumber(&ntmContexte, 25);
    ntmSetSeparatorConstraint(&ntmContexte, 1);
    initialisation_analyse_token=NON;
}


void GlobalParseurNTM::InitialisationParametres(char* parametres) {
            //sprintf(parametres,"%d:%d:%d:%d:%d:%d|", reduce,num_extract,
            //        tagger,fonction_synt,
            //        tree,renumerote);

    ParametresAffichage=parametres[0];
    AfficheFonction=parametres[1];
    Desambiguisation(parametres[2]);
    CalculFonction=parametres[3];

    ntmSetSeparatorConstraint(&ntmContexte, parametres[8]);
    
    if (parametres[9]==OUI)
        ntmSetUnknownStringMode(&ntmContexte,BYCHAR);
    else
        ntmSetUnknownStringMode(&ntmContexte,BYTOKEN);

    if (nom_fichier[0])
        appliqueGrammaire=parametres[10];
}


//On traite les variables d'environnemnt a ce niveau
char GlobalParseurNTM::ChargementFichierScript() {

    string base;
    string suite;
    string script_reconstitue;
	char resultat[1000];
    char chemin[1000];
    NOM=ntmFileName;
	Nom_scripts.ajoute(DupliqueChaine(ntmFileName));
    ifstream fichier(ntmFileName,modeOuverture);

    if (fichier.fail()) {
        sprintf(resultat,"Unknown NTM script file: %s",ntmFileName);
        erreur(resultat);
    }
   
	//Il y a quatre sections:
	//SPACES, SEPARATORS, definition des alias, utilisation des alias
    vector<string> lexiques;    

    int i=0;
    unsigned char c;
    c=fichier.get();
    while (!fichier.eof()) {        
		if (c!=13) {
            base+=c;
			i++;
		}
        c=fichier.get();
    }
	//base contient l'ensemble du fichier...
	base+="\n\n\n\n\n";

    //We look for the SEPARATOR keyword:
	size_t pos=base.find("SEPARATORS");
	if (pos!=string::npos) {
		pos=base.find("\n\n",pos+1);
		script_reconstitue=base.substr(0,pos+2);
		suite=base.substr(pos+2,base.size()-pos-2);
	}

	//suite contient les chemins
    int j;
    i=0;
    while (i<suite.size()) {
        j=0;
        while (suite[i]!='\n') {
            chemin[j++]=suite[i];
            i++;
        }
        i++;        

        chemin[j]=0;
        //on recherche d'abord le debut de notre chemin
        j=0;        
        while (chemin[j]>32) j++;
        while (chemin[j]<=32) j++;
        Trim(chemin+j);
        if (chemin[j]!=0) {
            //on normalise avec les variables en memoire
            NormalisationNomFichier(resultat,chemin+j);
			Nom_divers.ajoute(DupliqueChaine(resultat));
			chemin[j]=0;
			script_reconstitue+=chemin;
			script_reconstitue+=resultat;
			script_reconstitue+="\n";
        }        
		if (suite[i]=='\n') {
			script_reconstitue+=suite.substr(i,suite.size()-i);
            break;
		}
    }   
	loadStrategyScriptFromString(&ntmContexte,(char*)script_reconstitue.c_str());
    return 1;
}





#endif
