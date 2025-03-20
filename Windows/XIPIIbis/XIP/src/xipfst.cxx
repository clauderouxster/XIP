/* $Id: tok-aux.c,v 1.47 2004/02/03 01:18:43 lk Exp $ */

/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.63
   filename   : transduct.cxx
   Date       : 
   Purpose    : TRANSDUCTION XIPTRANS IMPLEMENTATION
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"
#include "ntmhmm.h"
#include "conversion.h"
#include "xipfst.h"

extern int kif_declencheur;
void MettreEnMinuscule(unsigned char* iterateur);
void Trim(char* chaine);

extern "C" {
	void NormalisationNomFichier(char* nomFichier,char* tampon);
}


//------------------------------------------------------------------------------
//									LOOK UP
//------------------------------------------------------------------------------

char GlobalParseurXipFST::lookup() {

	if (!lectures.dernier)
		return NON;
    if (MaxSentence!=-1 && num_phrase>MaxSentence)
        return OUI;

	//Nous disposons donc de notre structure de lecture...
    borneGauche=lectures[0]->gauche;
    borneDroite=lectures[0]->droite;
    borneGaucheCar=lectures[0]->gauchecar;
    borneDroiteCar=lectures[0]->droitecar;

    ostream* os=&cout;
    ListeVoc* voc=NULL;

    if (OS!=NULL)
        os=OS;

	string ch;
	char chmin[10];
    char testFinDePhrase=NON;
    char testBalise=NON;
    
    char retourchariotlocal=NON;    
    
    char ajouteFinLigne=NON;
    //chaque lecture correspond a un mot
    if (gAfficherResultats==OUI &&
        TestOptions(AFFICHAGE_OFFSET) && 
        appliqueGrammaire==NON && xipresult==NULL)
        *os<<"["<<borneGauche<<"-"<<borneDroite<<"]"<<Endl;


    for (int i=0;i<lectures.dernier;i++) {
        char* s= lectures[i]->mot;     

        //cas particulier, on rencontre juste un retour chariot
        if (s[0]==10 && s[2]==10)
            retourchariotlocal=OUI;

		//On suppose que nos lexiques internes sont en ISO-Latin
		//En UTF8, le resultat est aussi en UTF8, donc on a besoin d'une conversion
		//pour pouvoir interroger nos lexiques internes.

        uchar* pt=(uchar*)s;
        char chgt=NON;

        int nbcr=0;
        while ( *pt!=0) {            
            if (*pt<=31 && *pt!=9) {
                if (*pt==10)
                    nbcr++;
                *pt=' ';
                chgt=OUI;
            }
            ++pt;
        }
                
        char* ptt=NULL;
		if (retourchariotlocal==NON && retourchariot) {
			if (retourchariot<=9)
				sprintf(chmin,"+CR%d",retourchariot);
			else
				strcpy(chmin,"+CRxN");

			ch=chmin;
			ptt=strrchr(s,9);
            if (ptt!=NULL) {
				ch+=ptt+1;
				ptt[1]=0;
				lectures[i]->longueur=ptt-s+1;
				s=lectures[i]->concatene(STR(ch));
            }
			else
				s=lectures[i]->concatene(STR(ch));
		}            

        //Dans le cas d'un retour chariot detecte, on regarde si l'on a juste un retour chariot (associe a +?)
        if (retourchariotlocal==OUI) {
			ptt=strrchr(s,9);
			if (ptt!=NULL && ptt[1]=='+' && ptt[2]=='?') {
				retourchariot++;
                return NON;
			}
            //sinon, on desactive le retourchariot
            retourchariot=NON;
        }

		//Dans le cas d'un passage par le HMM, il faut garder la trace des offsets...
        if (wbf!=NULL) {            
			if (borneGauche==borneGaucheCar && borneDroite==borneDroiteCar)
				sprintf(chmin,"+%d+%d",borneGauche,borneDroite);
			else
				sprintf(chmin,"+%d+%d+%d+%d",borneGauche,borneDroite,borneGaucheCar,borneDroiteCar);
	        //On cherche les traits:
			ch=chmin;
            ptt=strrchr(s,9);
            if (ptt!=NULL) {
				ch+=ptt+1;
				ptt[1]=0;
				lectures[i]->longueur=ptt-s+1;
				s=lectures[i]->concatene(STR(ch));
            }
        }
        
                
        if (appliqueGrammaire==NON && xipresult==NULL) {
            ajouteFinLigne=OUI;
			num_mot++;
			nombre_mots++;
            if (gAfficherResultats==OUI)
                *os<<ConversionChaine(this,s)<<Endl;
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
    }
    
    if (wbf==NULL) {
        if (testFinDePhrase==OUI || EndOfSentence(NULL,UTILISE)) {
            Parse(OUI);
            Detruit();
            
            num_phrase++;
            
			if (renumerotation==OUI) {
				num_token=0;
                num_mot=0;
			}
            return OUI;
        }
    }
 
    if (ajouteFinLigne==OUI && gAfficherResultats==OUI)
        *os<<Endl;

    return OUI;
}

//------------------------------------------------------------------------------
//									APPLY
//------------------------------------------------------------------------------

void GlobalParseurXipFST::Apply() {

}




void GlobalParseurXipFST::SentenceAnalysis(char renumerote,char arbre) {
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

		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = FALSE;
	}
}


void GlobalParseurXipFST::StringAssociation(unsigned char* chaine) {
	if (TestOptions(TEST_INPUT_UTF8))
		TestUtf8Chaine(chaine);

    donneesATraiter=chaine;
    entree_est_chaine=OUI;
    if (TestOptions(EXECUTE_TOKENIZE))
        return;

	if (TestOptions(ENTREE_EN_MINUSCULE))
		MettreEnMinuscule(donneesATraiter);

}

FILE* GlobalParseurXipFST::FileAssociation(FILE* fichier) {
	fichierDonnees=fichier;
	if (TestOptions(TEST_INPUT_UTF8))
		TestUtf8Fichier(fichier);
    
    entree_est_chaine=NON;
    if (TestOptions(EXECUTE_TOKENIZE))
        return fichierDonnees;

	return fichierDonnees;
}

void GlobalParseurXipFST::AddAToken(unsigned char* tok) {

	StartAnalysis();
}

void GlobalParseurXipFST::StartAnalysis() {
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

void GlobalParseurXipFST::CloseAnalysis() {
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

		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = FALSE;
	}
	initialisation_analyse_token=NON;
}

void GlobalParseurXipFST::AnalyseTokenFichier() {
	//Chaque ligne est un token
	static char buff[1000];
	while (!feof(fichierDonnees)){
		fgets(buff,999,fichierDonnees);
		Trim(buff);
		if (buff[0])
			AddAToken((uchar*)buff);
	}
}

void GlobalParseurXipFST::AnalyseTokenChaine() {

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


GlobalParseurXipFST::GlobalParseurXipFST(char* ftrans,char* hmm,char* grammar,char* path) : GlobalParseurHMM(hmm,grammar,path) {
	if (kifloader==NULL)
		kifloader = new KifCodeParseur(NULL, kif_declencheur, (void*)this, OS);
	KifAutomaton* ktrans = new KifAutomaton(kifloader, NULL);
	ktrans->Setreference();
	string fname(ftrans);
	string fcar("+");
	ktrans->load(fname, fcar);
}


GlobalParseurXipFST::~GlobalParseurXipFST() {
	ktrans->Resetreference();
}

