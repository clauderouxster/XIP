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
   filename   : analyse.cxx
   Date       : 10/01/1999
   Purpose    : Chunking rules
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"
#include "conversion.h"
char* TV(GlobalParseur* parseur,Vecteur* v);
char* TC(GlobalParseur* parseur,Traits* tr);
char majlemmesurface(GlobalParseur* parseur,Traits* RegTraits,Noeud* noeud,char type);

extern "C" {
	Exporting bool xipExecutionEnd();
}
//void VireCategorie(ListeVoc* voc,Liste* L,char majtraits);
char SelectionneVocabulaire(Liste* L);
char AFFICHE_PHRASE_MULTIMOTS=NON;

int compteurdonc=0;
void initphraseindex(LListe& phrase) {
	int i;
	Liste* L;
	if (phrase.dernier) {
		int base=phrase[0]->N.num_droite;
		int increm=0;
		for (i=0;i<phrase.dernier;i++) {
			L=phrase[i];
			L->iphrase=i;
			if (L->voc!=NULL) {
				L->N.num_mot=increm+base;
				L->N.num_gauche=increm+base;
				L->N.num_droite=increm+base;
				increm++;
			}
			else {
				base=L->N.num_droite;
				increm=1;
			}
		}
	}
}


void initphrasevecteur(LListe& phrase) {
	int i;
	Liste* L;
	if (phrase.dernier) {
		TVecteur laClef=0;
		int nbn=1;
		for (i=phrase.dernier-1;i>=0;i--) {
			L=phrase[i];
			laClef|=L->index;
			L->N.clef_suivant=laClef;
			L->N.nombre=nbn++;
			L->iphrase=i;
		}
	}
}
//---------------------------------------------------------------------------------
void Liste::choixCategorieTestNoeud(GlobalParseur* parseur,int i) {
	if (N.sousnoeud==NON)
		choixCategorie(parseur,i);
	else
		choixCategorie(NULL,i);
}
//---------------------------------------------------------------------------------
void Liste::choixCategorie(GlobalParseur* parseur,int i) {
	int e;
	ListeVoc* lv;
	
	if (i==choisie)
		return;

	if (parseur!=NULL && voc->suivants[choisie]!=NULL) {
		int dernier=parseur->phraseCourante->dernier;
		//Il nous faut retirer toutes les categories prealablements ajoutees
		//On recherche la position de la categorie a retrouver
		lv=voc->suivants[choisie];
		e=iphrase;
		while (lv!=NULL) {
			while(e< parseur->phraseCourante->dernier && parseur->phraseCourante->vecteur[e]!=lv->L) e++;
			if (e >= parseur->phraseCourante->dernier)
				break;
			parseur->phraseCourante->retireElement(e);
			lv=lv->suivant;
		}
		if (dernier!=parseur->phraseCourante->dernier)
			initphraseindex(*parseur->phraseCourante);
	}

    N.X=voc->X[i];
    N.surface=(voc->X[i])->nom;
    X=(voc->X[i]);
    choisie=i;
    if (N.fils!=NULL && N.fils->lemme!=NULL) {
        N.fils->lemme=(char*)voc->lemme[i];
        N.fils->surface=(char*)voc->surface;
    }

	//dans le cas ou l'element est de plus ambigue sur le nb de categories
	//phraseCourant pointe sur la structure qui contient les Listes
	//On recherche d'abord la position de l'element courant
	if (parseur!=NULL && voc->suivants[choisie]!=NULL && voc->suivants[choisie]->L->N.sousnoeud==NON) {
		lv=NULL;
		char trouve=NON;
		e=iphrase;
		if (parseur->phraseCourante->vecteur[e]==this) {
			lv=voc->suivants[choisie];
			trouve=OUI;
		}

		//Il etait deja insere..
		if (lv==NULL || lv->L->N.pere!=NULL || lv->L==parseur->phraseCourante->vecteur[e+1])
			return;

		while (lv!=NULL) {
			parseur->phraseCourante->insere(++e,lv->L);
			lv=lv->suivant;
		}
		if (trouve==OUI) {
			initphrasevecteur(*parseur->phraseCourante);
			initphraseindex(*parseur->phraseCourante);
		}
	}
}

void Liste::choixCatTraits(int i) {
	X=voc->X[i];
	N.X=X;
	N.surface=(voc->X[i])->nom;
	choisie=i;
	traitsbase->recopie(voc->tr[i]);
	if (N.fils!=NULL && N.fils->lemme!=NULL) {
		N.fils->X=X;
		N.fils->lemme=(char*)voc->lemme[i];
		N.fils->surface=(char*)voc->surface;
	}
}


inline char Liste::TestEnCouche(GlobalParseur* pars,
                                int laCouche,int& pos_cat,char& multiple) {

    multiple=NON;
    if ( (pars->CategoriesDansCouche[laCouche] & index) == 0)
        return NON;

    if (voc!=NULL && voc->nbX > 1 && index != X->Val)
        multiple=OUI;
    else
        return OUI;
        
    int premier=-1;

    for (int i=pos_cat;i<voc->X.dernier;i++) {
        //On teste si la categorie est presente dans la couche en question
        //CategoriesDansCouche contient l'ensemble des categories qui apparaissent
        //dans la partie droite d'une regle. On filtre ainsi les noeuds
        //qui ont plusieurs interpretations (Verbe, Nom)... On choisit uniquement l'interpretation
        //qui soit presente dans la couche 
        if (pars->CategoriesObligatoiresDansCouche[laCouche] & (voc->X[i])->Val) {            
            choixCategorie(pars,i);
            if (premier==-1)
                pos_cat=i+1;
            return OUI;
        }

        if (premier==OUI && (pars->CategoriesDansCouche[laCouche] & (voc->X[i])->Val)) {
            pos_cat=i+1;
            premier=i;
        }
    }

    if (premier!=-1) {
        choixCategorie(pars,premier);
        pos_cat=premier+1;
        return OUI;
    }

    return NON;
}


/*-----------------------------Class segment---------------------------------------*/
void GlobalParseur::MAJInterVictoire(LListe& Phrase) {

    if (Phrase.dernier==1 && Phrase[0]->X==&Categories[1]->X) {
        //Deja calcule
        LINTERVICTOIRE=Phrase[0];
        INTERVICTOIRE=&LINTERVICTOIRE->N;
        return;
    }

    if (LINTERVICTOIRE==NULL)
        LINTERVICTOIRE = TasDeListes->fournir(this,&Categories[1]->X,NULL);

    INTERVICTOIRE = &LINTERVICTOIRE->N;


    INTERVICTOIRE->surface=Categories[1]->X.nom;
    INTERVICTOIRE->X = &Categories[1]->X;
    LINTERVICTOIRE->traitsactif->ou(Categories[1]->traits);
    LINTERVICTOIRE->traitsbase->ou(Categories[1]->traits);

    if (Phrase.dernier) {
        INTERVICTOIRE->gauche= Phrase[0]->N.gauche;
        INTERVICTOIRE->droite= Phrase[0]->N.droite;
        INTERVICTOIRE->gauchecar= Phrase[0]->N.gauchecar;
        INTERVICTOIRE->droitecar= Phrase[0]->N.droitecar;
        INTERVICTOIRE->num_gauche= Phrase[0]->N.num_gauche;
        INTERVICTOIRE->num_droite= Phrase[0]->N.num_droite;
    }


    Noeud* noeud;
    Noeud* premier=NULL;
    Noeud* dernier=NULL;

    Liste* base=NULL;
    int leDernier=Phrase.dernier-1;
	PTT(LINTERVICTOIRE, Phrase, 0, leDernier);
	LINTERVICTOIRE->SauveTraits();

    for (int ik=0;ik<=leDernier;ik++) {
        base=Phrase[ik];        

        noeud=&base->N;
        //les noeuds superieurs ne sont pas couverts
        base->couvert=UTILISE;
        if (premier==NULL) {
            premier=noeud;
            MetDebut(base->traitsbase);
        }
        else {
            premier->insere(noeud);
            dernier=noeud;
        }

		if (ik==leDernier) {
            MetFin(base->traitsbase);
		}

        base->SauveTraits();
        if (dernier!=NULL) {
            INTERVICTOIRE->droite=dernier->droite;        
            INTERVICTOIRE->droitecar=dernier->droitecar;        
            INTERVICTOIRE->num_droite=dernier->num_droite;        
        }
    }
    INTERVICTOIRE->fils=premier;	
}

void majnouvellecouche(LListe& nc) {
	for (int i=0;i<nc.dernier;i++) {
		nc[i]->N.sousnoeud=NON;
		Noeud* n=nc[i]->N.fils;
		while (n!=NULL) {
			n->sousnoeud=OUI;
			n=n->frere;
		}
	}
}
/*-------------------------------------------------------------------------*/
inline void Libres(LListe& Phrase, VECTA<Vecteur*>& libres) {
	for (int i = 0; i < Phrase.dernier; i++) {
		if (Phrase[i]->traitsactif->libre != NULL)
			libres.ajoute(new Vecteur(Phrase[i]->traitsbase->libre));
		else
			libres.ajoute(NULL);
	}
}

inline void ReplaceLibres(LListe& Phrase, VECTA<Vecteur*>& libres) {
	for (int i = 0; i < Phrase.dernier; i++) {
		if (libres[i] != NULL) {
			if (Phrase[i]->traitsbase->libre == NULL) {
				Phrase[i]->traitsbase->libre = libres[i];
				libres.affecte(i, NULL);
			}
			else
				Phrase[i]->traitsbase->libre->ou(libres[i]);
		}
	}
	libres.nettoie();
}

void GlobalParseur::ActiveCouche(LListe& Phrase,int couche) {

	phraseCourante=&Phrase;
    int maxboucle=0;

    //La couche 0 correspond a l'initialisation de la phrase
    int couche_noeuds=couche+1;
	VECTA<Vecteur*> libres;

    if (ChunkerLicense==OUI) {
        for (int lacouche=couche;lacouche<=MaxProfondeurAnalyse;lacouche++) {
			if (xipExecutionEnd())
				break;

            //On cherche la couche suivante...
            while (lacouche<=MaxProfondeurAnalyse) {
                if (CouchesActives[lacouche]!=VIDE) {

                    if (CouchesActives[lacouche]==REGLES_SIMPLIFICATION) {
                        if (TAGGING==OUI || DESAMBIGUISATION==OUI) 
                            break;
                    }
                    else

                        break;
                }    
                lacouche++;
            }
            if (lacouche>MaxProfondeurAnalyse)
                break;
            
            //si l'on a epuise les couches disponibles pour les phrases
            //on s'arrete
            if (couche_noeuds>=MaxCouchePhrase) {
                if (TRACE)
                    trace<<CHAINES[258]<<endl;
                break;
            }


            //Si un filtre est present sur les traits pour la couche
            if (TableDesFiltresDeCouche[lacouche].dernier) {
                char rescouche=OUI;    

                if (DeductionLicense==OUI) {
                    IndexationArbrePartiel(*phraseCourante,NON);
                    rescouche=(int)DeduitFonction(lacouche,DEDUCTIONFILTRE,NON);
                    AnnulationArbrePartiel(*phraseCourante);
                }

                if (rescouche==NON) {
                    if (TRACE)
                        trace<<"-->"<<CHAINES[58]<<lacouche<<endl;
                    continue;
                }
            }

            LListe& NouvelleCouche=glisteDesPhrases[couche_noeuds];                            
            NouvelleCouche.raz();
            ModificationDansCouche[lacouche]=NON;            
            

            switch (CouchesActives[lacouche]) {
            case REGLES_VOCABULAIRE:
                IndexationArbrePartiel(*phraseCourante,NON);
                AppliqueVocabulaire(lacouche);
                AnnulationArbrePartiel(*phraseCourante);
                break;
            case REGLES_DI:
                AppliqueDI(*phraseCourante,NouvelleCouche,lacouche);
                demarre_regle=0;
                demarre_couche=-1;
                aller_label=NULL;
                if (ModificationDansCouche[lacouche]==OUI) {
                    phraseCourante=&NouvelleCouche;
					majnouvellecouche(NouvelleCouche);
                    couche_noeuds++;
                }
                break;
            case REGLES_SEQUENCE:     
			case REGLES_SEQUENCE_CONTEXTE:     
                AppliqueSequence(*phraseCourante,NouvelleCouche,lacouche,REGLES_SEQUENCE);            
                demarre_regle=0;
                demarre_couche=-1;
                aller_label=NULL;
                if (ModificationDansCouche[lacouche]==OUI) {
                    phraseCourante=&NouvelleCouche;
					majnouvellecouche(NouvelleCouche);
                    couche_noeuds++;
                }
                break;

            case REGLES_TERME:
                AppliqueSequence(*phraseCourante,NouvelleCouche,lacouche,REGLES_TERME);            
                demarre_regle=0;
                demarre_couche=-1;
                aller_label=NULL;
                if (ModificationDansCouche[lacouche]==OUI) {
                    phraseCourante=&NouvelleCouche;
					majnouvellecouche(NouvelleCouche);
                    couche_noeuds++;
                }                
                break;
            case REGLES_MULTIMOTS:
                AppliqueMultiMots(*phraseCourante,NouvelleCouche,MULTISURFACE,lacouche);            
                demarre_regle=0;
                demarre_couche=-1;
                aller_label=NULL;
                if (ModificationDansCouche[lacouche]==OUI) {
                    phraseCourante=&NouvelleCouche;
					majnouvellecouche(NouvelleCouche);
                    couche_noeuds++;
                }                
                break;
            case REGLES_MULTILEMMES:
                AppliqueMultiMots(*phraseCourante,NouvelleCouche,MULTILEMME,lacouche);            
                demarre_regle=0;
                demarre_couche=-1;
                aller_label=NULL;
                if (ModificationDansCouche[lacouche]==OUI) {
                    phraseCourante=&NouvelleCouche;
					majnouvellecouche(NouvelleCouche);
                    couche_noeuds++;
                }                
                break;
            case REGLES_TOKENSELECTION:
				AppliqueTokenSelection(*phraseCourante,NouvelleCouche,lacouche,REGLES_TOKENSELECTION);            
                demarre_regle=0;
                demarre_couche=-1;
                aller_label=NULL;
                if (ModificationDansCouche[lacouche]==OUI) {
                    phraseCourante=&NouvelleCouche;
					majnouvellecouche(NouvelleCouche);
                    couche_noeuds++;
                }                
                break;
            case REGLES_FRAGMENTE:
                AppliqueFragmente(*phraseCourante,NouvelleCouche,lacouche,REGLES_FRAGMENTE);            
                demarre_regle=0;
                demarre_couche=-1;
                aller_label=NULL;
                if (ModificationDansCouche[lacouche]==OUI) {
                    phraseCourante=&NouvelleCouche;
					majnouvellecouche(NouvelleCouche);
                    couche_noeuds++;
                }                
                break;
            case REGLES_SIMPLIFICATION:
                AppliqueSimplification(*phraseCourante,lacouche);  
                demarre_regle=0;
                demarre_couche=-1;
                aller_label=NULL;
                break;
            case REGLES_MARQUAGE:
                MarqueNoeuds(*phraseCourante,lacouche);        
                demarre_regle=0;
                demarre_couche=-1;
                aller_label=NULL;
                break;
            case REGLES_CARTESIEN:
                AppliqueProduitCartesien(*phraseCourante,lacouche);
                demarre_regle=0;
                demarre_couche=-1;
                aller_label=NULL;
                break;
            case REGLES_RECONSTRUCTION:                
                AppliqueReconstruit(*phraseCourante,lacouche);
                demarre_regle=0;
                demarre_couche=-1;
                aller_label=NULL;
                break;
			case REGLES_SEMANTIQUE:
				if (SemantiqueLicense==OUI && CalculFonction!=NON) {
                    IndexationArbrePartiel(*phraseCourante,UTILISE);
					AppliqueSemantiques(lacouche,NON); 
					AnnulationArbrePartiel(*phraseCourante);
				}
				break;
			case REGLES_GENERATION:
                if (DeductionLicense==OUI) {
                    IndexationArbrePartiel(*phraseCourante,UTILISE);
                    DeduitFonction(lacouche,DEDUCTIONGENERATION,NON);
					//Les regles de generation sans couche sont systematiquement appliquees
					//apres
					DeduitFonction(0,DEDUCTIONGENERATION,NON);
                    AnnulationArbrePartiel(*phraseCourante);
                }
				break;
            case REGLES_DEDUCTION:                
                if (DeductionLicense==OUI) {
					Libres(*phraseCourante, libres);
                    IndexationArbrePartiel(*phraseCourante,UTILISE);
                    DeduitFonction(lacouche,DEDUCTIONCOUCHE,NON);
                    AnnulationArbrePartiel(*phraseCourante);
					ReplaceLibres(*phraseCourante, libres);
                }
            }

            //s'il s'agit d'un stop on arrete l'analyse a cet endroit
            if (aller_label==label_stop)                
                break;            

            if (aller_label==label_terminal)
                return;

            //dans le cas d'un GOTO (voir ListeTestArbre::inference), 
            //on peut etre amener a changer de couche.
            if (demarre_couche!=-1)
                lacouche=demarre_couche-1;
            else
                demarre_regle=0;       
            //Dans le cas ou l'on demande un arret de l'analyse, on sort
            if (parse_phrase==NON)
                return;
        }
    }

    if (DESAMBIGUISATION==NON)
        //on sauve le resultat et fin des analyses...
        MAJInterVictoire(*phraseCourante);
}
/*-------------------------------------------------------------------------*/
char GlobalParseur::coherent(int laCouche,Liste* ld, TVecteur Y, TVecteur Cle) {
    int val;
    TVecteur clef;

    if (ld->voc==NULL || (ld->voc)->nbX==1) {
        val=(ld->X)->val;
        clef=Cle|(ld->X)->Val;
        return  ((Y==(TabDroiteUn[laCouche][val] & Y) &&
                  clef==(clef&TabFiltre[laCouche][val])));
    }
    
    
    ListeVoc* voc=ld->voc;
    int posCat=-1;
    int premier=-1;

    //Verification globale
    if ( (CategoriesDansCouche[laCouche] & ld->index) == 0)
        return NON;
    
    for (int i=0;i<voc->nbX;i++) {
        if (CategoriesObligatoiresDansCouche[laCouche] & (voc->X[i])->Val) {
            posCat=i;
            if (premier==-1)
                premier=i;
            break;
        }
        
        //On prend la premiere interpretation qui correspond a une couche
        if (posCat==-1 && (CategoriesDansCouche[laCouche] & (voc->X[i])->Val)) 
            premier=i;
    }
        
    if (posCat!=-1) {
        val=(voc->X[posCat])->val;
        clef=Cle|(voc->X[posCat])->Val;
        if ((Y==(TabDroiteUn[laCouche][val] & Y) &&
             clef==(clef&TabFiltre[laCouche][val]))) {
            ld->choixCategorie(this,posCat);
            return OUI;
        }
    }

    //dans le cas ou la categorie preferee ne peut marcher, on continue l'analyse en partant de
    //la premiere categorie rencontree qui soit compatible.
    if (premier!=-1) 
        for (;premier<voc->nbX;premier++) {
            val=(voc->X[premier])->val;
            clef=Cle|(voc->X[premier])->Val;
            if ((Y==(TabDroiteUn[laCouche][val] & Y) &&
                 clef==(clef&TabFiltre[laCouche][val]))) {
                ld->choixCategorie(this,premier);
                return OUI;
            }
        }
 
    
    return NON;
}
/*-------------------------------------------------------------------------*/
//Il s'agit d'une sortie qui permet le regroupement de tous les lexemes par syntagme.
//Le but etant de fournir une liste de syntagme au lieu d'une liste de mots.
//Aucune tentative de regroupement des syntagmes n'est alors tentee.
//Cette fonction est le point d'entree de l'analyse...
//L'ordre est celui donne par le numero de couche.
int comptedi=0;
void GlobalParseur::AppliqueDI(LListe& Phrase,LListe& NouvelleListe,int laCouche) {

    //on puise dans gListeDesPhrases pour aller chercher une couche ou sauvegarder les phrases.
    //LListe& NouvelleListe= glisteDesPhrases[laCouche];
    //NouvelleListe.dernier=0;

    ChercheRegle regle(this);
	char arret_analyse=NON;
    Regle* R;
    Liste* Attente=NULL;
    Liste* L;
    Liste* ld;
    Liste* Ld;
    Liste* Lf; 
    short dernier,boucle;
    char solution;
    avl_Regle* aR;
    int ik,iListe;
	VECTA<Noeud*> noeuds_arguments(3);

    if (laCouche>Limite_Couche) {
        for (iListe=0;iListe<Phrase.dernier;iListe++)
            NouvelleListe.ajoute(Phrase[iListe]);
		//Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
		//commencer a reappliquer nos regles
		demarre_regle=0;
		demarre_couche=label_stop->couche;
		aller_label=label_stop;
		return;
	}

    //Les profondeurs varient de 1 a 99, MaxProfondeurAnalyse vaut le maximum de profondeur trouve

    if (TRACE)
        trace<<"--> "<<CHAINES[146]<<laCouche<<endl<<endl;

    int i=0;
    //On se positionne sur le dernier element de la liste
    iListe=Phrase.dernier-1;
    int pos_cat=0;
    char multiple=NON;
	initphraseindex(Phrase);
	initphrasevecteur(Phrase);
    //On analyse tous les elements de la liste a partir du dernier
    while (iListe>=0) {
        L=Phrase[iListe];
        L->N.nombre=Phrase.dernier-iListe;

        //On recherche la premiere categorie qui peut apparaitre dans la couche:
        while (iListe>=0 && !L->TestEnCouche(this,laCouche,pos_cat,multiple)) {
            //Ceux que l'on saute sont simplement reintegre dans la liste
            NouvelleListe.ajoute(L);
			comptedi++;
            pos_cat=0;
            L=Phrase[--iListe];
            if (L!=NULL)
                L->N.nombre=Phrase.dernier-iListe;
        }

        //Si le premier element a ete passe, on s'arrete, toute la liste a ete parcourue...
        if (iListe<0)
            break;

        
        //On initialise le noeud avec la bonne categorie en fonction de la couche dans
        //laquelle on est...
        regle.insere(this,L->X);
        int dernierListe=iListe;
		int taillePhrase=Phrase.dernier;

        Ld=L;
        Lf=Phrase[dernierListe];

        //On positionne le curseur dans la liste sur le premier element a droite
        if (iListe >0) {
            ld=Phrase[iListe-1];
            ik=iListe-1;
        }
        else
            ld=NULL; //si l'on peut

        //On isole une sequence coherente de categories a partir de la droite
        while (ld  &&
               coherent(laCouche,
                        ld,
                        (Ld->X)->Val,
                        regle.cle)) {
            regle.insere(this,ld->X);
            Ld=ld;
            ld->debut=NON;
            ld->fin=NON;
            ld->instanciateur=NULL;
            iListe=ik;
            if (ik >0) 
                ld=Phrase[--ik];
            else
                break;
        }

		//dans le cas ou des elements ont ete rajoutes ou retires, on 
		//repositionne correctement l'index du dernier element
		if (Lf!=NULL && Lf!=Phrase[dernierListe])
			dernierListe+=Phrase.dernier-taillePhrase;

        //On retire les elements incoherents en fonction des PL-regles
        while (iListe<dernierListe && !AppPl(laCouche,Phrase,iListe,dernierListe)) {
            regle.retire(Phrase[iListe]->X);
            iListe++;
        }

        solution=NON;
        boucle=OUI;


        Ld=Phrase[iListe];

        
        Ld->debut=NON;
        Ld->fin=NON;

        aR=NULL;
      
        Lf->fin=OUI;
        Lf->debut=NON;

		if (dernierListe>=Phrase.dernier)
			dernierListe=Phrase.dernier-1;
		
        while (boucle) {
            if (TRACE || TRACENOEUD) {
                trace<<endl;
                Espace(0);trace << laCouche << CHAINES[148];
                Phrase.Affiche(trace,iListe,dernierListe);
            }


            solution=0;

            Ld->debut=OUI;
			char premiereanalyse=OUI;
            R=recherche(regle,&aR,laCouche,premiereanalyse);            
            IEspace=i;
	 
            while (R) {
				
				if (!testEspaceRegle(R)) {
					R=recherche(regle,&aR,laCouche,premiereanalyse);
					continue;
				}

				reglecourante=R;
                //dans le cas d'un GOTO, il nous faut virer les regles avant
                //le label
                if (R->activation(this)==NON) {                    
                    //Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
                    //commencer a reappliquer nos regles
                    demarre_regle=0;
                    demarre_couche=label_stop->couche;
                    aller_label=label_stop;
                    break;
                }


                if (R->numero<demarre_regle) {
                    R=recherche(regle,&aR,laCouche,premiereanalyse); 
                    continue;
                }

                if (TRACE) {
                    Espace(i);
                    trace << laCouche << CHAINES[80];
                    VlabelVecteur[R->numero]->aff(trace);
                    trace<<endl;
                }

                //On verifie que la derniere categorie correspond
                //bien a celle qui a ete marquee <last> 
                //dans la regle (si ce trait a ete bien ajoute)
                if (R->testPremiereCat(Ld->X) && R->testDerniereCat(Lf->X)) {
                    //On associe a ce nouveau noeud le numero de la couche qui l'a cree

                    L=TasDeListes->fournir(this,R->Sx,NULL);
                    L->N.numero=R->identificateur;
                    L->N.gauche=Ld->N.gauche;
                    L->N.droite=Lf->N.droite;
                    L->N.gauchecar=Ld->N.gauchecar;
                    L->N.droitecar=Lf->N.droitecar;
                    L->N.num_gauche=Ld->N.num_gauche;
                    L->N.num_droite=Lf->N.num_droite;

                    IEspace=i;
                    noeuds_arguments.raz();
                    noeuds_arguments.affecte(0,&L->N);
                        
                    if (R->TestContexte(this,Phrase,iListe-1,dernierListe+1,noeuds_arguments) &&
                        InitTraits(L,Phrase,iListe,dernierListe,R->traits,
                                   R->TeteTraits,R->Nombre,&noeuds_arguments) &&
                                   R->TestExpressionBooleen(this,noeuds_arguments) &&
								   AppVregle(L->traitsbase,L->X,PostRCT)) {


                        if (R->borneD!=-1)
                            InstancieContexte(Phrase,dernierListe+1,R->borneD);
                        if (R->borneG!=-1)
                            InstancieContexte(Phrase,R->borneG,iListe-1);

                        //La couche a modifiee quelque chose dans la structure initiale
                        ModificationDansCouche[laCouche]=OUI;
						
						//Application finale des Specifications de traits par defaut
						AppVregle(L->traitsbase,L->X,PostSTD);
						L->SauveTraits();

						pos_cat=0;
                        ListeVoc* voc;
                        Cat* XRef;
                        int ivocr;
						
                        //S'il s'agit d'une regle de type: ? -> etc...
                        //Dans ce cas, l'on ne fait qu'initialiser des traits sur les noeuds
                        //sans les placer dans un arbre...
                        if ((R->Sx)->Val==0) {

                            //Puis pour chaque noeud de la liste, on transforme les traits initialises
                            //en traits definitifs, du dernier vers le premier (la liste NouvelleListe
                            //est toujours cree a l'envers)
                            for (ik=dernierListe;ik>=iListe;ik--) {
                                ld=Phrase[ik];
                                if (R->TeteTraits!=NULL)
                                    InstancieTraits(ld->traitsactif,(R->TeteTraits)->traits);

                                if (DESAMBIGUISATION==OUI && ld->voc!=NULL && ld->voc->nbX>1) {
                                    //On ne garde que les entrees avec les memes categories
                                    voc=ld->voc;
                                    voc->garder.raz();
                                    XRef=ld->X;
                                    voc->garder.affecte(ld->choisie,OUI);
                                    for (ivocr=0;ivocr<voc->nbX;ivocr++)
                                        if (voc->X[ivocr]==XRef)
                                            voc->garder.affecte(ivocr,OUI);
                                    VireCategorie(ld->voc,ld,OUI);
                                    ld->index=(ld->voc)->CalculIndex();     
                                }

                                ld->StabiliseTraits(OUI);
                                //et on ajoute cet element dans la liste
                                NouvelleListe.ajoute(ld);
								comptedi++;
                            }
                            
                            //On retire les traits first et last de ces nouvelles structures de traits
                            RetireDebut(Ld->traitsbase);
                            RetireFin(Lf->traitsbase);
                            
                            //(Ld->traitsbase)->etnon(DebutSeq);
                            //(Lf->traitsbase)->etnon(FinSeq);

                            //Enfin, on ne construit pas de pere pour cet ensemble de noeud...
                            solution=OUI;
                            arret_analyse=incrementestats(R);
                            //Il n'est pas necessaire de conserver ce noeud L...
                            TasDeListes->RecupereDernier(this);
                            break;
                        }
                        
                        L->N.fils=&Ld->N;
                        

                        //Puis pour chaque noeud de la liste, on transforme les traits initialises
                        //en traits definitifs
                        for (ik=iListe;ik<=dernierListe;ik++) {                         
                            ld=Phrase[ik];
                            if (ik>iListe)
                                //ld->N devient un frere du precedent Ld...
                                Ld->N.insere(&ld->N);
   
                            if (DESAMBIGUISATION==OUI && ld->voc!=NULL && ld->voc->nbX>1) {
                                //On ne garde que les entrees avec les memes categories
                                voc=ld->voc;
                                voc->garder.raz();
                                XRef=ld->X;
                                voc->garder.affecte(ld->choisie,OUI);
                                for (ivocr=0;ivocr<voc->nbX;ivocr++)
                                    if (voc->X[ivocr]==XRef)
                                        voc->garder.affecte(ivocr,OUI); 
                                VireCategorie(ld->voc,ld,OUI);
                                ld->index=(ld->voc)->CalculIndex();  
                            }
                       
                            ld->StabiliseTraits(OUI);
                        }

         
                        if (TRACENOEUD) {
                            Espace(i);
                            trace << laCouche << " -> ";
                            L->affichet(this,CHAINES[81]);
                        }
                        
                        solution=OUI;
                        arret_analyse=incrementestats(R);
                        NouvelleListe.ajoute(L);
						comptedi++;
                        break;
                    }
                    else
                        if (TRACE) {
                            Espace(i);
                            trace << laCouche;
                            trace << CHAINES[82]<<endl;
                        }

                    //On recupere le L qui a ete donnee par fournir...
                    TasDeListes->RecupereDernier(this);
                }


                if (!solution && aR)
                    R=recherche(regle,&aR,laCouche,premiereanalyse); 
                else 
                    R=NULL;

                if ((TRACE || TRACENOEUD) && R) {
                    Espace(i);
                    trace << laCouche;
                    if (!solution) {
                        trace << CHAINES[149];
                        Phrase.Affiche(trace,iListe,dernierListe);
                    }
                    else
                        trace << CHAINES[83]<<endl;             
                }
            }
            if (solution) 
                break;

            //On retire la catégorie en tête de la chaîne (à gauche)
            if (iListe<dernierListe) {
                regle.retire(Ld->X);
                ld=Ld;
                iListe++;
                Ld=Phrase[iListe];
                dernier=NON;
                aR=NULL;
            }
            else boucle=NON;
        }

        //s'il n'y a pas de solution, c'est le noeud mot lui-meme que l'on ajoute...
        if (!solution) {
            if (multiple==NON) { //si aucune autre interpretation existe pour cette categorie
                NouvelleListe.ajoute(Ld); //alors on passe a l'element suivant...
				comptedi++;
                iListe--; //sinon, on essaie avec l'interpretation suivante de ce mot
            }            //pos_cat contient la position de l'interpretation suivante du mot
        }
        else 
            iListe--; //Quand une solution a ete trouvee on passe tout de suite au noeud suivant
                   
        regle.raz();
    }
    
    if (TRACE)
        trace<<"<--- "<<CHAINES[146]<<laCouche<<endl<<endl;


    //On inverse cette liste (elle a ete construite a l'envers)
    NouvelleListe.inverse();    
}

//------------------------------------------------------------------------------
char testascii(string& s) {
	for (int i=0;i<s.size();i++) {
		if ((uchar)s[i]>127)
			return NON;
	}
	return OUI;
}

//de la gauche vers la droite... 
void GlobalParseur::AppliqueFragmente(LListe& Phrase,LListe& NouvelleListe,int laCouche,typeRegle leType) {
	char arret_analyse=NON;
    Liste* ld;
    char solution=NON;
    int i=0;        
    int il;
    int iListe=0;    
    int ic=0;
    char* leLemme=NULL;
    string resultats[10];
    VECTA<Noeud*> noeuds_arguments(3);
    VECTA<Liste*> listenouveaux;

    if (laCouche>Limite_Couche) {
        for (iListe=0;iListe<Phrase.dernier;iListe++)
            NouvelleListe.ajoute(Phrase[iListe]);
		//Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
		//commencer a reappliquer nos regles
		demarre_regle=0;
		demarre_couche=label_stop->couche;
		aller_label=label_stop;
		return;
	}

    while (iListe<Phrase.dernier) {
        ld= Phrase[iListe];
        solution=NON;
        
        while (ld!=NULL && (ld->index & CategoriesDansCouche[laCouche]) == 0) {  
            NouvelleListe.ajoute(ld);
            ld=Phrase[++iListe];
        }
        
        if (ld==NULL)
            break;

        
        //Pour chaque mot, on regarde s'il y a un automate pour le traiter        
        if (NumeroOrdre()==NON && ld->voc!=NULL) {
            for (i=0;i<Fragmentes[laCouche].dernier;i++) {      
                //On recupere la fonction
                Sequence* fonc=Fragmentes[laCouche][i]->fragmente;
				reglecourante=fonc;

				int borne=1;

                //On a deux types de regle qui peuvent s'appliquer sur
                //la forme de surface ou sur le lemme
                if (Fragmentes[laCouche][i]->surface==NON)
                    borne=(ld->voc)->nbX;

                for (ic=0;ic<borne;ic++) {                
                    //On recherche la regle sur le lemme ou la forme de surface
                    if (Fragmentes[laCouche][i]->surface==0) {                        
                        if (fonc->Fx->Val!=0 && ld->voc->X[ic]!=fonc->Fx)
                            continue;
                        
                        if (!ITR(this,fonc->traits,ld->voc->tr[ic],NONECRIT,&ld->N))
                            continue;
                        leLemme=(ld->voc)->lemme[ic];
                    }
                    else {
                        leLemme=(ld->voc)->surface;
                        if (fonc->Fx->Val!=0 && (ld->index & fonc->Fx->Val)==0)
                            continue;
                        if (!ITR(this,fonc->traits,ld->traitsbase,NONECRIT,&ld->N))
                            continue;
                    }                                                            

                    automatepile& autom=Fragmentes[laCouche][i]->a;
                    if (autom.recherche(leLemme)) {                    
                        
                        //La valeur #0 est toujours le sommet que l'on construit...
                        noeuds_arguments.raz();                        
                        //On associe le noeud courant a la variable 0
                        noeuds_arguments.affecte(0,&ld->N);
                        if (fonc->TestContexteSimple(this,Phrase,iListe-1,iListe+1,noeuds_arguments)) {
                                                    
                            //La couche a modifiee quelque chose dans la structure initiale
                            ModificationDansCouche[laCouche]=OUI;
                            
                            if (fonc->borneD!=-1)
                                InstancieContexte(Phrase,iListe+1,fonc->borneD);
                            if (fonc->borneG!=-1)
                                InstancieContexte(Phrase,fonc->borneG,iListe-1);
                            
                            //Puis pour chaque noeud de la liste, on transforme les traits initialises
                            //en traits definitifs
                            
                            solution=OUI;      
                            arret_analyse=incrementestats(fonc);
                            //On construit une serie de noeuds supplementaires, pour chacun des elements
                            //de la regle
                            autom.extraitvariables((uchar*)leLemme,resultats);
                            EtatArbre* reg=fonc->regle;
                            Liste* ln;
                            int varcour;
                            char premier=OUI;
                            listenouveaux.raz();
							
							//On calcule nos differentes positions en fonction des
							//chaines extraites...
							int pos=ld->N.gauche;
							int poscar=ld->N.gauchecar;
							VECTA<int> positions;
							VECTA<int> positionscar;

							for (int increment=1;increment<=autom.maxvar;increment++) {
								positions.affecte(increment,pos);
								positionscar.affecte(increment,poscar);
								pos+=resultats[increment].size();
								poscar+=size_c(resultats[increment]);
							}

							positions.ajoute(pos);
							positionscar.ajoute(poscar);
								
							while (reg->etatfin==0) {
                                ArcArbre* arc=reg->arcs[0];
								varcour=arc->desamb;

                                if (resultats[varcour].size()==0) {
                                    solution=NON;
                                    break;
                                }
                                
                                //si la chaine est vide, on ne cree pas de noeud
                                if (resultats[varcour]=="") {
                                    reg=arc->etatarbre;
                                    continue;
                                }
                                
                                if (arc->traits!=NULL) {
                                    if (arc->traits->lemme!=NULL)
                                        arc->traits->lemme->affecte(resultats);
                                    
                                    if (arc->traits->surface!=NULL)
                                        arc->traits->surface->affecte(resultats);
                                }
								
								
                                ln=CreeNoeudFragment(STR(resultats[varcour]),arc->traits,arc->X);
								ln->N.fictif=NON;
								ln->N.fils->fictif=NON;

								
								ln->N.gauche=positions[varcour];
								ln->N.gauchecar=positionscar[varcour];

								ln->N.droite=positions[varcour+1];
								ln->N.droitecar=positionscar[varcour+1];

								ln->N.num_gauche=ld->N.num_gauche;
								ln->N.num_droite=ld->N.num_droite;
                                
                                if (arc->argument!=-1)
                                    noeuds_arguments.affecte(arc->argument,&ln->N);

								ln->voc->borneGauche=ln->N.gauche;
								ln->voc->borneDroite=ln->N.droite;
								ln->voc->borneGaucheCar=ln->N.gauchecar;
								ln->voc->borneDroiteCar=ln->N.droitecar;

								ln->N.fils->gauche=ln->N.gauche;
                                ln->N.fils->droite=ln->N.droite;
                                ln->N.fils->gauchecar=ln->N.gauchecar;
                                ln->N.fils->droitecar=ln->N.droitecar;
                                ln->N.fils->num_gauche=ln->N.num_gauche;
                                ln->N.fils->num_droite=ln->N.num_droite;

								listenouveaux.ajoute(ln);
                                reg=arc->etatarbre;	
                            }

                            if (solution==OUI) {                                                            
								if (fonc->listes!=NULL)
                                    solution=fonc->listes->Test(this,noeuds_arguments);
                                
                                if (solution==OUI) {                                                                    
                                    for (il=0;il<listenouveaux.dernier;il++)
                                        NouvelleListe.ajoute(listenouveaux[il]);
                                }
								arret_analyse=incrementestats(fonc);
                            }
                            if (solution==NON) {
                                //Il y a deux appels a chaque fois
                                for (il=0;il<listenouveaux.dernier*2;il++)
                                    TasDeListes->RecupereDernier(this);
                                
                            }
                        }
                    }
                    if (solution==OUI)
                        break;
                }
                if (solution==OUI)
                    break;
            }
        }
        if (solution==NON) 
            NouvelleListe.ajoute(ld);
        
        iListe++;
    }
}

//------------------------------------------------------------------------------
//Application des expressions multi mots sur une phrase
void GlobalParseur::AppliqueMultiMots(LListe& Phrase,LListe& NouvelleListe,char typeregle,int laCouche) {
	char arret_analyse=NON;
    string laphrase="";
    VECTA<int> offsets;
	VECTA<int> debuts;

    if (laCouche>Limite_Couche) {
        for (int iListe=0;iListe<Phrase.dernier;iListe++)
            NouvelleListe.ajoute(Phrase[iListe]);
		//Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
		//commencer a reappliquer nos regles
		demarre_regle=0;
		demarre_couche=label_stop->couche;
		aller_label=label_stop;
		return;
	}
   
    //dans un premier temps, il nous faut construire la phrase a partir des lemmes
    //Lorsque nous sommes face a une categorie majeure, on met son nom directement, les lectures
    //ambigues sont separees par un "_"
    int i;
    long gauche;
    long droite=-1;
    for (i=0;i<Phrase.dernier;i++) {
        if (droite==-1)
            droite=Phrase[i]->N.gauche;
        gauche=Phrase[i]->N.gauche;
        
        if (Phrase[i]->voc==NULL) {
            laphrase+=" ";
			debuts.ajoute(laphrase.size());
            laphrase+=Phrase[i]->X->nom;
			offsets.ajoute(laphrase.size());
        }
        else {
            //On recupere la forme de surface            
			if (typeregle==MULTISURFACE) {
				if (droite!=gauche)
					laphrase+=" ";
				debuts.ajoute(laphrase.size());
				laphrase+=Phrase[i]->voc->surface;        
				offsets.ajoute(laphrase.size());
			}
			else {
				//On rajoute toujours un blanc pour les lemmes
				if (laphrase.size()>0)
					laphrase+=" ";
				debuts.ajoute(laphrase.size());
				laphrase+=Phrase[i]->voc->lemme[0];        
				offsets.ajoute(laphrase.size());
			}
        }
        droite=Phrase[i]->N.droite;
    }
	
	if (AFFICHE_PHRASE_MULTIMOTS) {
		ostream* os=&cout;
		if (OS!=NULL)
			os=OS;
		*os<<"MWR:<"<<laphrase.c_str()<<">"<<endl;
	}

	//Pour eviter les problemes de fin de chaine, on rajoute un element
	//final
	offsets.ajoute(laphrase.size()+1);
    etat* e;
    int pos,j;
    multimots* m;
    Liste* L;
    
    uchar* str=(uchar*)laphrase.c_str();
	for (i=0;i<offsets.dernier-1;i++) {       
		uchar* cstr=str+debuts[i];
		char ajout=NON;
		//Pour les couches simples
		e=NULL;
		if (motsmultiples[typeregle][laCouche]!=NULL) {
			e=motsmultiples[typeregle][laCouche]->recherchepartielle(cstr);
			if (e!=NULL)
				pos=motsmultiples[typeregle][laCouche]->maxpos+debuts[i];
		}

		//Pour les couches complexes en cas d'echec...
		if (e==NULL && motsmultiplescomplexes[typeregle][laCouche]!=NULL) {
			e=motsmultiplescomplexes[typeregle][laCouche]->recherchepartielle(cstr);
			if (e!=NULL)
				pos=motsmultiplescomplexes[typeregle][laCouche]->maxpos+debuts[i];
		}

		if (e!=NULL) {
			//pos pointe sur la fin de la chaine
			//On recherche la fin de notre chaine
			//Il faut que la fin coincide avec le debut d'une autre chaine sans l'inclure
			char trouve=NON;
			for (j=i+1;j<offsets.dernier;j++) {
				if (pos==offsets[j]) {
					trouve=OUI;
					break;
				}
				if (pos<offsets[j])
					break;
			}
			if (trouve==NON) {
				NouvelleListe.ajoute(Phrase[i]);
				continue;
			}
			
			m=(multimots*)e->valeur;
			
			if (!testEspaceRegle(m)) {
				NouvelleListe.ajoute(Phrase[i]);
				continue;
			}

			reglecourante=m;
			if (m->activation(this)==NON) {                    
				//Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
				//commencer a reappliquer nos regles
				demarre_regle=0;
				demarre_couche=label_stop->couche;
				aller_label=label_stop;
				NouvelleListe.ajoute(Phrase[i]);
				continue;
			}
			
			//On repere nos bornes gauches et droites
			//Pour tester nos contextes: i et j
			char test=OUI;
			if (m->gauche!=NULL || m->droit!=NULL || m->listes!=NULL) {
				VECTA<Noeud*> noeuds_arguments;
				int elementGauche=-1;
				int elementDroit=-1;
				
				if (m->gauche!=NULL)
					test=(m->gauche)->TestUnContexte(this,Phrase,i-1,-1,NON,elementGauche,&noeuds_arguments);

				if (test==OUI) {
					if (m->droit != NULL)
						test=(m->droit)->TestUnContexte(this,Phrase,j+1,1,NON,elementDroit,&noeuds_arguments);
				}
				if (test==OUI && m->listes!=NULL)
					test=m->listes->Test(this,noeuds_arguments);
			}


			if (test==OUI) {
				//j contient alors le numero du dernier mot touche par l'automate
				//Nous devons donc construire de i jusqu'a j
				L=TasDeListes->fournir(this,m->X,m->tr);
				ajout=OUI;

				//puis on ajoute nos nouveaux noeuds
				//On applique les Vregles sur ce nouveau noeud pere...
				if (!AppVregle(L->traitsbase,L->X,RCT))
					ajout=NON;
				else {
					AppVregle(L->traitsbase,L->X,STD);
					/* Remontée des traits libres vers la tête...*/
					if (!PTT(L,Phrase,i,j)) 
						ajout=NON;
					else
						if (!AppVregle(L->traitsbase,L->X,PostRCT)) 
							ajout=NON;
						else
							AppVregle(L->traitsbase,L->X,PostSTD);
				}
				if (ajout==OUI) {
					L->N.numero=m->identificateur;

					L->N.gauche= Phrase[i]->N.gauche;
					L->N.droite= Phrase[j]->N.droite;
					L->N.gauchecar= Phrase[i]->N.gauchecar;
					L->N.droitecar= Phrase[j]->N.droitecar;
					L->N.num_gauche= Phrase[i]->N.num_gauche;
					L->N.num_droite= Phrase[j]->N.num_droite;

					Liste* premier=Phrase[i];
					L->N.fils=&premier->N;
					for (int s=i+1;s<=j;s++)
						premier->N.insere(&Phrase[s]->N);
					i=j;
					if (premier!=NULL) {
						MetDebut(premier->traitsbase);
						MetDebut(premier->traitsactif);
						if (premier->N.dernier->source!=NULL) {
							MetFin(premier->N.dernier->source->traitsbase);
							MetFin(premier->N.dernier->source->traitsactif);
						}
					}
					arret_analyse=incrementestats(m);
					ModificationDansCouche[laCouche]=OUI;

					NouvelleListe.ajoute(L);
				}
				else
					TasDeListes->RecupereDernier(this);
			}
		}
		if (ajout==NON)
			NouvelleListe.ajoute(Phrase[i]);
	}
}

//------------------------------------------------------------------------------
//de la gauche vers la droite... Categories en sequence
void GlobalParseur::AppliqueSequence(LListe& Phrase,LListe& NouvelleListe,int laCouche,typeRegle leType) {
	char arret_analyse=NON;
    Liste* curseur;
    int categorie;
    VectSequence* sequence;
    Liste* L;
    Liste* ld;
    char solution=NON;
    int i=0;
    int dernierNoeudIntegre=-1;
    //LListe& NouvelleListe= glisteDesPhrases[laCouche];
    //NouvelleListe.dernier=0;
    VECTA<char> majCategorie;
    VectSequence termes;
    VECTA<void*> a;
	VECTA<Noeud*> noeuds_arguments(3);
    ResultatSequence element(this,NULL,NULL);

    if (TRACE)
        trace<<endl<<"--> "<<CHAINES[174]<<laCouche<<endl<<endl;

    int iListe=0;    
    int ic=0;
    char chercheCat=NON;
    char indexMisAJour=NON;
    char sequenceSuivante=NON;
	element.initialise(this,NULL,NULL);

    if (laCouche>Limite_Couche) {
        for (int iListe=0;iListe<Phrase.dernier;iListe++)
            NouvelleListe.ajoute(Phrase[iListe]);
		//Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
		//commencer a reappliquer nos regles
		demarre_regle=0;
		demarre_couche=label_stop->couche;
		aller_label=label_stop;
		return;
	}

	char testTouteCategorie;
    while (iListe<Phrase.dernier) {

        sequence=NULL;
        int isequence=0;

        //On recherche la premiere categorie qui permet l'application d'une regle...
        while (iListe<Phrase.dernier) {
            ld= Phrase[iListe];

            if (chercheCat==NON) {
                //Cette categorie ne peut commencer une regle de sequence
                while (ld!=NULL && (ld->index & CategoriesDansCouche[laCouche]) == 0) {  
                    NouvelleListe.ajoute(ld);
                    ld=Phrase[++iListe];
                }
            }
            if (ld==NULL)
                break;

            isequence=0;
			testTouteCategorie=-1;
            if (leType==REGLES_SEQUENCE) {
                //Si cette categorie est un mot...
                if (ld->voc!=NULL && (ld->voc)->nbX>1 && ld->index!=(ld->X)->Val) {
                    chercheCat=OUI;
                    
                    for (;ic<(ld->voc)->nbX;ic++) {
                        categorie=((ld->voc)->X[ic])->val;
                        sequence=TableDesSequences[laCouche][categorie];
                        //On verifie que la regle utilisee precedemment ne commencait pas par
                        //un ?. En fait, si des regles de type: Cat = ? etc... sont presentes en
                        //tete des regles, leur reapplication est redondante... En effet, ? peut
                        //correspondre a n'importe quelle categorie, donc il n'est pas interessant
                        //de reessayer ces regles une nouvelle fois...Rappelons qu'elles sont placees
                        //systematiquement pour toutes les categories dans toutes les couches.
                        if (ic>=1 && sequence!=NULL) {
                            isequence=0;
							testTouteCategorie=OUI;
                            while ((*sequence)[isequence]!=NULL) {
                                if ((*sequence)[isequence]->touteCategorie==OUI)
                                    isequence++;
                                else
                                    break;
                            }
                            if ((*sequence)[isequence]==NULL)
                                continue;
                        }
                        
                        if (sequence!=NULL)
                            break;
                    }
                }
                else {
                    categorie=(ld->X)->val;
                    sequence=TableDesSequences[laCouche][categorie];
                    chercheCat=NON;
                    ic=0;
                }
            }
            else {
                char* leLemme=NULL;
                sequence=NULL;
                if (ld->voc!=NULL) {
                    termes.raz(); 
                    for (ic=0;ic<(ld->voc)->nbX;ic++) {                        
                        //sinon il s'agit d'une regle de terme
                        //On recherche la regle sur le lemme         
                        if (Termes[laCouche]!=NULL) {
                            a.raz();
                            leLemme=(ld->voc)->lemme[ic];
                            Termes[laCouche]->recherchetouslabels(leLemme,a);       
                            for (int ia=0;ia<a.dernier;ia++) {
                                vocabulaire_terme* vterm=(vocabulaire_terme*)a[ia];
                                for (int iaa=0;iaa<vterm->terme->dernier;iaa++)
                                    termes.ajoute(vterm->terme->cell(iaa));
                            }
                            if (a.dernier!=0)
                                sequence=&termes;
                        }
                    }
                }
            }
            
            if (sequence!=NULL)
                break;

            ic=0;
            chercheCat=NON;
            NouvelleListe.ajoute(ld);
            iListe++;
        }


        if (sequence==NULL)
            break;

        if (indexMisAJour==NON) {
			initphrasevecteur(Phrase);
            indexMisAJour=OUI;
        }
		
        Sequence* regle=(*sequence)[isequence];
		int modifcurseur=-1;
		int taillecourante;
        while (regle!=NULL) {
			reglecourante=regle;
            solution=NON;

			curseur=Phrase[iListe];
			if (regle->touteCategorie==testTouteCategorie || !testEspaceRegle(regle)) {
				regle=(*sequence)[++isequence];
				continue;
			}

            
            if (regle->activation(this)==NON) {                    
                //Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
                //commencer a reappliquer nos regles
                demarre_regle=0;
                demarre_couche=label_stop->couche;
                aller_label=label_stop;
                break;
            }

            //Dans le cas d'un goto on saute les regles avant le label
            if (regle->numero<demarre_regle) {
                regle=(*sequence)[++isequence];
                continue;
            }
            
			majCategorie.raz();
			noeuds_arguments.raz();
			if (modifcurseur!=-1) {
				taillecourante=Phrase.dernier;
				Phrase.dernier=modifcurseur;
			}
            char resseq=sequence->Match(this,element,Phrase,iListe,laCouche,
                                    isequence,noeuds_arguments,majCategorie);
            
			if (modifcurseur!=-1) {
				//La desambiguisation peut changer la taille, il faut en tenir compte
				Phrase.dernier=taillecourante-(modifcurseur-Phrase.dernier);
				modifcurseur=-1;
			}

			//Si on a modifie la taille de Phrase, c'est pour appliquer la regle
			//un cran avant.

            if (TRACE && resseq==NON) {
                trace << laCouche<<CHAINES[230];
                if (VlabelVecteur[regle->numero]->type==REGLES_SEQUENCE)
                    VlabelVecteur[regle->numero]->aff(trace);
                else
                    VlabelVecteur[regle->numero]->aff(trace);
                trace<<endl;
                trace << CHAINES[231]<<curseur->N.surface
                      <<"["<< curseur->N.gauche<<"-"<<curseur->N.droite<<"]"<<endl<<endl;
            }

            if (resseq==OUI) {
                char insereNoeuds=OUI;
                L=NULL;           

                //L'utilisation de la methode sequence->Match peut modifier la valeur de isequence et donc
                //la regle courante utilisee. On reinitialise "regle" par consequent
                regle=(*sequence)[isequence];

                int iDebut=element.debut;
                int iFin=element.fin;
                Liste* boucle=Phrase[iDebut];
				int ik=iDebut+1;

                if (element.bon && element.fin != -1) {
                    if (TRACE) {
                        trace<<endl;
                        Espace(i);
                        trace << laCouche << CHAINES[232];
                        VlabelVecteur[regle->numero]->aff(trace);trace<<endl;                              
                        trace <<CHAINES[231]<<boucle->N.surface
                              <<"["<< boucle->N.gauche<<"-"<<boucle->N.droite<<"]"<<Endl;
                    }

					insereNoeuds=OUI;			

					//Il s'agit d'une regle de type ? =
					if ((element.X)->Val==0)
						insereNoeuds=NON;

					if (regle->L!=NULL)
						L=regle->L;
					else {
						L=TasDeListes->fournir(this,element.X,NULL);
						L->N.numero=regle->identificateur;

						L->N.gauche= boucle->N.gauche;
						L->N.droite= Phrase[iFin]->N.droite;
						L->N.gauchecar= boucle->N.gauchecar;
						L->N.droitecar= Phrase[iFin]->N.droitecar;
						L->N.num_gauche= boucle->N.num_gauche;
						L->N.num_droite= Phrase[iFin]->N.num_droite;


						boucle->debut=OUI;
						boucle->fin=NON;
						Traits* ins=boucle->instanciateur;
						boucle->N.annule();
						boucle->instanciateur=ins;


						while (ik<=iFin) {
							ld=Phrase[ik];
							ld->debut=NON; //position intermediaire...
							ld->fin=NON;
							ik++;
						}

					}
					
					IEspace=i;					

                    //La valeur #0 est toujours le sommet que l'on construit...
                    noeuds_arguments.affecte(0,&L->N);
                    if (regle->L!=NULL || 
						(InitTraits(L,Phrase,iDebut,iFin,NULL,element.traits,element.Nombre) &&
                         regle->TestExpressionBooleen(this,noeuds_arguments) &&
						 AppVregle(L->traitsbase,L->X,PostRCT))) {


                        //La couche a modifiee quelque chose dans la structure initiale
                        ModificationDansCouche[laCouche]=OUI;

                        for (int ii=0; ii<majCategorie.dernier;ii++) {
                            ld=Phrase[iDebut+ii];
                            ld->couvert=majCategorie[ii];
                        }        

                        if (regle->borneD!=-1)
                            InstancieContexte(Phrase,iFin+1,regle->borneD);
                        if (regle->borneG!=-1)
                            InstancieContexte(Phrase,regle->borneG,iDebut-1);

						AppVregle(L->traitsbase,L->X,PostSTD);
						L->SauveTraits();

                        //Puis pour chaque noeud de la liste, on transforme les traits initialises
                        //en traits definitifs
                        for (ik=iDebut;ik<=iFin;ik++) {
                            ld=Phrase[ik];

                            if (insereNoeuds==NON) {
                                //On instancie les traits dans le cas d'une regle en ? avec les traits
                                //associe a ce ? dans la partie gauche de la regle.
                                Traits* trsequence=regle->traits;
                                if (trsequence!=NULL)
                                    InstancieTraits(ld->traitsactif,trsequence);
                                ld->StabiliseTraits(NON);
                            }
                            else
                                if (ik > iDebut)
                                    boucle->N.insere(&ld->N);
                            
                            if (DESAMBIGUISATION==OUI && ld->voc!=NULL && 
                                (ld->couvert==OUI || (ld->couvert==UTILISE && ld->aSelectionner!=NULL))) {
                                (ld->voc)->garder.raz();
                                if (ld->couvert==OUI)
                                    (ld->voc)->garder.affecte(ld->choisie,OUI);
                                if (SelectionneVocabulaire(ld))
                                    VireCategorie(ld->voc,ld,OUI);                                
                                ld->index=(ld->voc)->CalculIndex();     
                            }

                            ld->StabiliseTraits(OUI);
                        }
                    
                        //S'il s'agit d'une regle de type: ? -> etc...
                        //Dans ce cas, l'on ne fait qu'initialiser des traits sur les noeuds
                        //sans les placer dans un arbre...
                        if (insereNoeuds==NON) {
                                                     
                            //On retire les traits first et last de ces nouvelles structures de traits
                            RetireDebut(Phrase[iDebut]->traitsbase);
                            //ld pointe sur le dernier noeud...
                            RetireFin(ld->traitsbase);
                            //On oblige a l'incrementation de isequence, pour tester l'ensemble des regles
                            //qui commencent par cette categorie...
							arret_analyse=incrementestats(regle);
                        }
                        else {
                            //On va donc remplacer les categories comprises entre boucle et 
                            //element.fin par une nouvelle categorie: L
                            L->N.fils=&boucle->N;

                            dernierNoeudIntegre=iFin;

                    
                            if (TRACENOEUD) {
                                Espace(i);
                                trace << i << "> :";L->affichet(this,CHAINES[81]);
                            }

                            //Si l'increment est vers la droite, on ajoute simplement l'element
                            if (element.increment==-1) {
                                //sinon, c'est plus compliquee, il faut retirer les elements precedemment ajoutees
                                //et qui desormais sont chapautes par L
                                //Les elements vont de iDebut a iFin exclu
                                //retirer
                                for (ik=iDebut;ik<iFin;ik++)
                                    NouvelleListe.retire();
                            }

                            NouvelleListe.ajoute(L);
                            iListe=1+iFin;
                            sequenceSuivante=NON;

                            ic=0; //on remet le position de categories a zero, pour demarrer sur la premiere
                            //categorie du mot suivant.
                            solution=OUI;
                            arret_analyse=incrementestats(regle);
                        
                            //element->Detruit();
                            break; //while (sequence != NULL)
                        }
                    }
					else {
						//Dans le cas d'une regle surdeterminee, et apres
						//echec du where ou des traits... On coupe
						//l'analyse et on retente avec une chaine plus courte
						if (regle->deterministe==SURDETERMINE && iListe<element.fin) {
							//Tout d'abord, on recupere le L courant
							TasDeListes->RecupereDernier(this);
							//Puis on limite la taille de phrase a element.fin
							modifcurseur=element.fin;
							//Puis on retente
							continue;
						}
					}
                }
                
                //On recupere le L qui a ete donnee par fournir...
                if (L!=NULL)
                    TasDeListes->RecupereDernier(this);
                //element->Detruit();
            }
            //On va chercher la fonction suivante... Meme categorie de depart, meme couche...
            regle=(*sequence)[++isequence];
        }
        
        if (solution==NON) {
            if (chercheCat==OUI)
                ic++;
            else {
                NouvelleListe.ajoute(curseur);                
                iListe++;
            }
        }
    }
    
    if (TRACE)
        trace<<"<--- "<<CHAINES[174]<<laCouche<<endl<<endl;
}


/*-------------------------------------------------------------------------*/
static void TraceListeMots(GlobalParseur* pars,LListe& Phrase) {
    int nb;

    for (int ic=0;ic<Phrase.dernier;ic++) {
        if (ic)
            trace<<" ; ";
        nb=0;
        if (Phrase[ic]->voc!=NULL)
            nb=(Phrase[ic]->voc)->nbX;
        if (nb>1 && Phrase[ic]->index) {
            trace<<"[";
            for (int ik=0;ik<nb;ik++) {
                if (ik)
                    trace<<",";
                trace<<((Phrase[ic]->voc)->X[ik])->nom;
                trace<<"<"<<(Phrase[ic]->voc)->lemme[ik]<<">";
            }
            trace<<"]";
        }
        else {
            trace<<(Phrase[ic]->X)->nom;
            trace<<"<"<<Phrase[ic]->N.surface<<">";
        }
    }
        
    trace<<endl<<endl;
}
/*-------------------------------------------------------------------------*/
//Il s'agit non plus d'une unification mais d'une verification stricte sur les bits
char GlobalParseur::TestTraitsReg(Traits* reg, 
                                  Traits* noeud,
                                  char* lemme,
                                  char* surface) {
    register short i;
    TVecteur* nval=(noeud->valeur)->vecteur;

    if (reg->lpresent!=NULL) {
        char erreur=OUI;
        TVecteur* rlp=(reg->lpresent)->vecteur;
        
        for (i=0;i<iTailleVect;i++) {
            if (( *nval & *rlp) != 0) {
                erreur=NON;
                break;
            }
            nval++;
            rlp++;
        }
        
        if (erreur==OUI)
            return NON;
    }

    nval=(noeud->valeur)->vecteur;
    
    TVecteur* nneg= NULL;
    TVecteur* rneg=NULL;
    TVecteur* rpres=NULL;
    
    TVecteur* rval=(reg->valeur)->vecteur;
    TVecteur* rfil=(reg->filtre)->vecteur;
    TVecteur* nfil= (noeud->filtre)->vecteur;

    if (noeud->negation!=NULL)
        nneg=(noeud->negation)->vecteur;
    if (reg->negation!=NULL)
        rneg=(reg->negation)->vecteur;
    if (reg->present!=NULL)
        rpres=(reg->present)->vecteur;

    for (i=0;i<iTailleVect;i++) {
        if (rneg!=NULL) {
            if ( (*nval & *rneg) != 0)
                return NON;         
            rneg++;
        }
        
        
        if (nneg!=NULL) {
            if ((*rval & *nneg) != 0)
                return NON;
            nneg++;
        }
        

        if (rpres!=NULL) {            
            if ((*nfil & *rpres) != *rpres)
                return NON;
            rpres++;
        }
        
        if (*rval!=0) {
            TVecteur r=*rfil & *nfil;
            if ((*nval & r) != (*rval & r))
                return NON;
        }
        
        rval++;
        nval++;
        nfil++;
        rfil++;                
    }
	//Dans le cas d'un test ou ni la surface ni le lemme ne sont fournis
	//il est inutile de tester quoique ce soit de plus, on retourne OUI
    if (reg->lemme!=NULL && lemme!=NULL) {
        if (!reg->lemme->recherche(lemme)) {
            if (reg->maniplemme!=NEG)
                return NON;
        }
        else {
            if (reg->maniplemme==NEG)
                return NON;            
        }
    }
    
    if (reg->surface!=NULL && surface!=NULL) {
        if (!reg->surface->recherche(surface)) {
            if (reg->manipsurface!=NEG) 
                return NON;
        }
        else {
            if (reg->manipsurface==NEG) 
                return NON;            
        }
    }        

    static VECTA<Noeud*> vnoeuds;
    char erreur;
    if (reg->varlemme!=NULL) {
        string bufflemme;
        erreur=NON;
        reg->varlemme->TraitementFormuleString(this,vnoeuds,erreur,bufflemme);
        if (bufflemme.size() && !strcmp((char*)bufflemme.c_str(),lemme)) {
            if (reg->maniplemme==VARNEG)
                return NON;
        }
        else {
            if (reg->maniplemme==VARCOMPARE)
                return NON;
        }
    }
    

    if (reg->varsurface!=NULL) {
        string buffsurface;
        erreur=NON;
        reg->varlemme->TraitementFormuleString(this,vnoeuds,erreur,buffsurface);
        if (buffsurface.size() && !strcmp((char*)buffsurface.c_str(),surface)) {
            if (reg->manipsurface==VARNEG)
                return NON;
        }
        else {
            if (reg->manipsurface==VARCOMPARE)
                return NON;
        }
    }

    return OUI;
}

/*-------------------------------------------------------------------------*/
char GlobalParseur::SelectionneVocabulaire(Liste* L) {

    Traits* reg = L->aSelectionner;
    ListeVoc* voc=L->voc;
    Cat* cat= NULL;
    char choisie=OUI;
    char garderUne=NON;

    if (L->couvert==OUI)
        cat=voc->X[L->choisie];
    else
        choisie=NON;

    for (int i=0;i<voc->nbX;i++) {
        if ( (cat==NULL || voc->X[i]==cat) &&
             (reg==NULL || ITR(this,reg,voc->tr[i],NONECRIT))) {

            garderUne=OUI;
            //dans le cas ou la categorie principale n'a pas ete encore
            //choisie (dans le cas d'un "?" dans une regle associee avec
            //une selection de lemme...
            if (choisie==NON) {
                choisie=OUI;
                L->choixCategorie(this,i);
            }

            voc->garder.affecte(i,OUI);
            if (reg!=NULL)
                InstancieTraits(voc->tr[i],reg);
        }
    }

    return garderUne;
}
/*-------------------------------------------------------------------------*/
char GlobalParseur::VireCategorie(ListeVoc* voc,Liste* L,char majtraits) {
    int vok=0;
    VECTA<int> mar;

    if (voc->nbX==1) {
        if (TRACENOEUD) {
            if (voc->tag[vok]!=NULL)
                trace<<CHAINES[176]<<voc->tag[vok]<<endl;
            else
                trace<<CHAINES[176]<<voc->surface<<endl;
        }

        (L->traitsactif)->ou(AmbiguiteErreur);
        //On renvoie un message pour indiquer que le nettoyage n'a pas eu lieu
        return NON;
    }
    
    for (vok=0;vok<voc->nbX;vok++) {
        if (majtraits==OUI)
            //on vire les analyses provenant du vocabulaire
            (L->traitsactif)->etnon(voc->tr[vok]);
        if (voc->garder[vok]==NON) {
			voc->X.affecte(vok,NULL);
            if (TRACENOEUD) {    
                if (voc->tag[vok]!=NULL)
                    trace<<CHAINES[177]<<voc->tag[vok];
                else
                    trace<<CHAINES[177]<<voc->lemme[vok];
                trace<<endl;
            }

            if (voc->tag[vok]!=NULL) {
                libere(voc->tag[vok]);                
                voc->tag.affecte(vok,NULL);
            }
            if (voc->lemme[vok]!=NULL) {
                libere(voc->lemme[vok]);
                voc->lemme.affecte(vok,NULL);
            }
            if (voc->tr[vok]!=NULL) {
                delete voc->tr[vok];
                voc->tr.affecte(vok,NULL);
            }
            if (voc->suivants[vok]!=NULL)
                voc->suivants.affecte(vok,NULL);
        }
        else 
            mar.ajoute(vok);        
    }

    Vecteur filtreur(iTailleVect);
    char modifTraits=NON;

    if (majtraits==OUI && voc->tr[L->choisie]!=NULL)
        if (FusionneTraits(L->traitsactif,voc->tr[L->choisie],filtreur))
            modifTraits=OUI;

    for (vok=0;vok <mar.dernier;vok++) {
        int kov=mar[vok];
        
        if (majtraits==OUI)    
            if (FusionneTraits(L->traitsactif,voc->tr[kov],filtreur))
                modifTraits=OUI;
        
        if (kov!=vok) {        
            voc->X.affecte(vok,voc->X[kov]);
            voc->tag.affecte(vok,voc->tag[kov]);
            voc->tag.affecte(kov,NULL);
            voc->lemme.affecte(vok,voc->lemme[kov]);
            voc->tr.affecte(vok,voc->tr[kov]);
			voc->suivants.affecte(vok,voc->suivants[kov]);
        }
    }
    
    if (modifTraits==OUI)
        NettoieTraitsIncoherent(L->traitsactif,filtreur);
    
    
    voc->X.dernier=mar.dernier;
    voc->tag.dernier=mar.dernier;
    voc->lemme.dernier=mar.dernier;
    voc->tr.dernier=mar.dernier;
    voc->nbX=mar.dernier;

	L->choisie=-1;
	L->choixCategorie(this,0);

    L->traitsbase->ou(TraitNoeudTerminal);
	if (voc->nbX==1) {
		//16 correspond au trait: noambiguity
		L->traitsactif->filtre->vecteur[0]|=PasAmbigue->valeur->vecteur[0];
		L->traitsactif->valeur->vecteur[0]|=PasAmbigue->valeur->vecteur[0];
		L->traitsbase->filtre->vecteur[0]|=PasAmbigue->valeur->vecteur[0];
		L->traitsbase->valeur->vecteur[0]|=PasAmbigue->valeur->vecteur[0];
	}
    return OUI;
}


/*-------------------------------------------------------------------------*/
void GlobalParseur::ReinitPhrase(Liste* L, ListeVoc* voc,int i,Traits* reg_traits) {

    //On retire d'abord l'ensemble des traits provenant du vocabulaire lui-meme
    //On conserve tous les traits qui ont ete enregistres avant.
    for (int ip=0;ip<voc->nbX;ip++)
        (L->traitsbase)->etnon(voc->tr[ip]);


    InstancieTraits(voc->tr[i],reg_traits);

    (L->traitsactif)->raz();    

    (L->traitsbase)->ou(voc->tr[i]);
    //Instanciation des traits possibles
    InstancieTraits(L->traitsbase,reg_traits);

    L->choixCategorie(this,i);

    if (L->N.fils!=NULL) {
        (L->N.fils)->X=voc->X[i];
    
        (L->N.fils)->surface=(char*)voc->surface;
        (L->N.fils)->lemme=(char*)voc->lemme[i];
        
        (L->N.fils)->source=L;
        L->traitsbase->ou(TraitNoeudTerminal);
    }

    L->X=voc->X[i];
}
/*-------------------------------------------------------------------------*/
char GlobalParseur::TestTraitsDeListe(Liste* L,Traits* tr) {

    return TestTraitsReg(tr,L->traitsactif,NULL,NULL);
}


/*-------------------------------------------------------------------------*/
void GlobalParseur::filtreCategorie(ArcArbre* ar,Liste* L) {			
	char initNoeud=OUI;
	char enGarderUne=NON;
	if (L!=NULL && L->voc!=NULL) {                
		ListeVoc* voc=L->voc;
		voc->garder.raz();
		for (int i=0;i<voc->nbX;i++) {
			if ( ((ar->X)->Val==0 || voc->X[i]==ar->X) &&
				(ar->traits == NULL ||
				TestTraitsReg(ar->traits,voc->tr[i],voc->lemme[i],voc->surface)) &&                      
				(ar->selection == NULL ||
				TestTraitsReg(ar->selection,voc->tr[i],voc->lemme[i],voc->surface))) {
					//L'initialisation ne se fait qu'avec la premiere categorie de la regle
					//kat=0

					if (initNoeud==OUI) {
						//Reinitialisation du mot de la phrase (traits et noeud)
						ReinitPhrase(L,voc,i,ar->traits);
						initNoeud=NON;
					}
					else {
						//dans le cas, ou ce n'est pas la premiere categorie
						//on la marque simplement, mais on enregistre aussi les
						//traits
						InstancieTraits(voc->tr[i],ar->traits);
						InstancieTraits(L->traitsbase,voc->tr[i]);
					} 
					voc->garder.affecte(i,OUI);
					enGarderUne=OUI;
			}           
		}
		if (TRACENOEUD) 
			trace<<CHAINES[178]<<endl;
		//toutes sont virees, impossible, on reconstruit InitTraits
		if (enGarderUne==OUI) {
			(L->traitsactif)->ou(L->traitsbase);
			VireCategorie(voc,L,NON); 
			L->index=voc->CalculIndex();
		}
	}	
}

void GlobalParseur::MajDesAutresCategories(Simplifie* reg,LListe& Phrase) {

    //On recherche la premiere categorie qui satisfait la regle:
    VECTA<ArcArbre*>& v=reg->vectarbre;
    if (v.dernier==0)
        return;

	for (int kat=0;kat<v.dernier;kat++) {
		ArcArbre* ar=v[kat];
		for (int nl=0;nl<ar->numliste.dernier;nl++)
			filtreCategorie(ar,Phrase[ar->numliste[nl]]);
	}
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::EcrasementCategorie(Cat* nouvX, Traits* nouvTR,ListeVoc* voc,Liste* L) {
    
    if (TRACENOEUD)
        trace<<CHAINES[177]<<voc->lemme[0]<<endl;
    
    //On ne conserve que les traits communs
    //qui peuvent provenir d'autres sources que le lexique.
    //et ne doivent pas etre perdue.        
    TVecteur debutFin = TestDebutFin(voc->tr[0]);
    TVecteur liberte=0;
    if ((L->traitsbase)->libre!=NULL)
        liberte=((L->traitsbase)->libre)->vecteur[0];

    //On retire d'abord l'ensemble des traits provenant du vocabulaire lui-meme
    //On conserve tous les traits qui ont ete enregistres avant.
    for (int ip=0;ip<voc->nbX;ip++)
        (L->traitsbase)->etnon(voc->tr[ip]);
    
    voc->X.affecte(0,nouvX);
    //On detruit alors toutes les autres interpretations stockees jusque la...
	int vok;
    for (vok=1;vok<voc->nbX;vok++) {
        voc->X.affecte(vok,NULL);
        if (voc->tag[vok]!=NULL) {
            libere(voc->tag[vok]);                
            voc->tag.affecte(vok,NULL);
        }
        if (voc->lemme[vok]!=NULL) {
            libere(voc->lemme[vok]);
            voc->lemme.affecte(vok,NULL);
        }
        if (voc->tr[vok]!=NULL) {
            delete voc->tr[vok];
            voc->tr.affecte(vok,NULL);
        }
		if (voc->suivants[vok]!=NULL)
			voc->suivants.affecte(vok,NULL);
    }           

    (voc->tr[0])->raz();
    voc->X.dernier=1;
    voc->tag.dernier=1;
    voc->lemme.dernier=1;
    voc->tr.dernier=1;
	if (voc->suivants[0]!=NULL)
		voc->suivants.dernier=1;
	else
		voc->suivants.dernier=0;    

    voc->nbX=1;

    L->choisie=0;
    //ReinitPhrase(L,voc,0,reg->traits_categorie[0]); 
    InstancieTraits(voc->tr[0],nouvTR);
    
    (L->traitsactif)->raz();    
    
    (L->traitsbase)->ou(voc->tr[0]);

    //Instanciation des traits possibles
    InstancieTraits(L->traitsbase,nouvTR);

	if (nouvTR!=NULL)
		majlemmesurface(this,nouvTR,&L->N,ECRIT);

	L->X=voc->X[0];
	L->N.X=L->X;
	L->N.surface=(voc->X[0])->nom;
	
    if (L->N.fils!=NULL) {
		L->N.fils->X=L->X;
		L->N.fils->lemme=(char*)voc->lemme[0];
		L->N.fils->surface=(char*)voc->surface;
        L->N.fils->source=L;
        //On remet ce trait pour eviter de perdre cette precieuse information
        L->traitsbase->ou(TraitNoeudTerminal);
    }

    if (debutFin!=0)
        MetDebutFin(L->traitsbase,debutFin);
    
    if (liberte!=0)
        ((L->traitsbase)->libre)->vecteur[0]|=liberte;
}

/*-------------------------------------------------------------------------*/
char GlobalParseur::MajDesCategories(Simplifie* reg, ListeVoc* voc,Liste* L) {

    //On recherche la premiere categorie qui satisfait la regle:
    char initNoeud=OUI;
    char enGarderUne=NON;
    voc->garder.raz();

    //Dans ce cas on ecrase avec la categorie fournie
    if (reg->ecrase==OUI) {
        EcrasementCategorie(reg->categorie[0],reg->traits_categorie[0],voc,L);
        return OUI;
    }

    for (int kat=0;kat<reg->nbCat;kat++) {
        for (int i=0;i < voc->nbX;i++) {
            if ((reg->categorie[kat]->Val ==0 || 
                 voc->X[i]==reg->categorie[kat]) &&
                (reg->traits_categorie[kat] == NULL ||
                 TestTraitsReg(reg->traits_categorie[kat],voc->tr[i],voc->lemme[i],voc->surface))) {
                //Si la categorie que l'on choisit est la 0, elle a deja ete initialisee
                //L'initialisation ne se fait qu'avec la premiere categorie de la regle
                //kat=0
                
                if (initNoeud==OUI) {
                    //Reinitialisation du mot de la phrase (traits et noeud)
                    ReinitPhrase(L,voc,i,reg->traits_categorie[kat]); 
                    initNoeud=NON;
                }
                else {
                    //dans le cas, ou ce n'est pas la premiere categorie
                    //on la marque simplement, mais on enregistre aussi les
                    //traits
                    InstancieTraits(voc->tr[i],reg->traits_categorie[kat]);
                    InstancieTraits(L->traitsbase,voc->tr[i]);
                } 
                voc->garder.affecte(i,OUI);
                enGarderUne=OUI;
            }
        }
    }    

    if (enGarderUne==OUI)
        (L->traitsactif)->ou(L->traitsbase);
            
    return enGarderUne;
}

char TestLemmeCommun(GlobalParseur* parseur,Traits* reg, VECTA<char*>& lemmes) {
	if (reg->lemme==NULL && reg->varlemme==NULL)
		return OUI;

	for (int l=0;l<lemmes.dernier;l++) {
		char* lemme=lemmes[l];
		if (reg->lemme!=NULL && lemme!=NULL) {
			if (!reg->lemme->recherche(lemme)) {
				if (reg->maniplemme!=NEG)
					return NON;
			}
			else {
				if (reg->maniplemme==NEG)
					return NON;            
			}
		}
		if (reg->varlemme!=NULL) {
			string bufflemme;
			static VECTA<Noeud*> vnoeuds;
			char erreur=NON;
			reg->varlemme->TraitementFormuleString(parseur,vnoeuds,erreur,bufflemme);
			if (bufflemme.size() && !strcmp((char*)bufflemme.c_str(),lemme)) {
				if (reg->maniplemme==VARNEG)
					return NON;
			}
			else {
				if (reg->maniplemme==VARCOMPARE)
					return NON;
			}
		}
	}
	return OUI;
}
/*-------------------------------------------------------------------------*/

char GlobalParseur::AppliqueSimplification(LListe& Phrase,int laCouche) {
	char arret_analyse=NON;
    Liste* L;
    VECTA<Simplifie*>* vreg;
    Simplifie* reg;

    if (laCouche>Limite_Couche) {
		//Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
		//commencer a reappliquer nos regles
		demarre_regle=0;
		demarre_couche=label_stop->couche;
		aller_label=label_stop;
		return OUI;
	}

    if (laCouche==-1)
        vreg=&ReglesSimplifie;
    else {
        vreg=&TableDesSimplifications[laCouche];
        if (TRACE)
            trace<<" --> "<<CHAINES[180]<<laCouche<<endl<<endl;
    }

    
    if (TRACENOEUD) {
        trace<<CHAINES[182]<<endl;
        TraceListeMots(this,Phrase);
    }       
	
	initphraseindex(Phrase);
	initphrasevecteur(Phrase);

    //Chaque element de type Simplifie a un index qui correspond 
    //a l'union de ses categories
    for (int ireg=demarre_regle;ireg<vreg->dernier;ireg++) {
        reg=(*vreg)[ireg];
		reglecourante=reg;
		if (!testEspaceRegle(reg))
			continue;

        if (reg->activation(this)==NON) {
            //Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
            //commencer a reappliquer nos regles
            demarre_regle=0;
            demarre_couche=label_stop->couche;
            aller_label=label_stop;
            break;
        }
        
        //Dans le cas d'un GOTO on saute les regles avant le label
        if (reg->numero<demarre_regle)
            continue;        

		int pos=0;
        L=Phrase[pos];
        while (L!=NULL && NumeroOrdre()==NON) {

        //On recherche la premiere categorie qui correspond a cet index (index dans Phrase)
            while (L!=NULL) {

                if (L->voc!=NULL && ((L->index & reg->clef) == reg->clef) && 
                    (reg->communs==NULL || 
                     (TestTraitsReg(reg->communs,L->traitsactif,NULL,L->N.fils->surface) &&
					  TestLemmeCommun(this,reg->communs,L->voc->lemme))))
                    break;
                
                L=Phrase[++pos];
            }

            if (L!=NULL) {
                ListeVoc* voc=L->voc;

                //On dispose donc d'une ambiguite du meme type que la regle
                //On verifie les traits.
                //Pour chacune des categories de reg
                char applicable=OUI;      
                for (int it=0;it<reg->nbX;it++) {
                    //tous==OUI s'il faut verifier toutes les entrees, NON sinon...
                    char trouve=reg->tous[it];
                    //on recherche une categorie dans voc qui correspond
                    for (int itp=0;itp<voc->nbX;itp++) {
                        if ( (reg->X[it]== XCatVide ||  reg->X[it] == voc->X[itp]) && 
                            (reg->traits[it]==NULL ||
                             TestTraitsReg(reg->traits[it],voc->tr[itp],voc->lemme[itp],voc->surface))) {
                            //dans le cas ou l'on ne cherche qu'une seule entree, on s'arrete
                            if (reg->tous[it]==NON) {
                                trouve=OUI;
                                break;
                            }
                            //sinon, il faut toutes les trouvees
                        }
                        else
                            //dans le cas d'un echec sur une entree, on verifie s'il fallait les trouver toutes...
                            if (reg->tous[it]==OUI) {
                                trouve=NON;
                                break;
                            }
                    }
                    
                    if (trouve==reg->negation[it]) {
                        applicable=NON;
                        break;
                    }
                }           
                
                if (applicable) {
                    
                    //On marque les categories.
                    //Cette marque servira a determiner si une categorie doit etre
                    //garde ou detruite en fin de processus...                    
                    char test=OUI;

                    //RAZ des listes de numliste pour les elements du contexte
                    //marques avec %
                    for (int rva=0;rva<reg->vectarbre.dernier;rva++) 
                        (reg->vectarbre[rva])->numliste.dernier=0;

                    VECTA<Noeud*> noeuds_arguments;
                    //#0 correspond au noeud courrant
                    noeuds_arguments.affecte(0,&L->N);
                    int elementGauche=-1;
                    int elementDroit=-1;

                    if (reg->gauche!=NULL)
                        test=(reg->gauche)->TestUnContexte(this,Phrase,pos-1,-1,4,elementGauche,&noeuds_arguments);

                    if (test==OUI)
                        if (reg->droit != NULL)
                            test=(reg->droit)->TestUnContexte(this,Phrase,pos+1,1,4,elementDroit,&noeuds_arguments);

                    if (test==OUI && reg->listes.liste.dernier>0)
                        test=reg->listes.Test(this,noeuds_arguments);

                    if (test==OUI) {
                        if (TRACENOEUD) {
                            trace<<endl<<endl<<CHAINES[180];
                            VlabelVecteur[reg->numero]->aff(trace);trace<<endl;
                            if (TRACETRAITS) {
                                trace<<L->X->nom;
                                L->traitsbase->TraitsEnChaine(this,Chaine_traits);
                                trace<<Chaine_traits<<endl;
                            }
                        }
                        
                        if (MajDesCategories(reg,voc,L)) {
                            arret_analyse=incrementestats(reg);
                            //On teste maintenant les categories que l'on doit virer...
                            char viree=VireCategorie(voc,L,NON);
                            if (viree==OUI) {
                                if (TRACENOEUD) {
                                    trace<<"Tagging Rule:";
                                    VlabelVecteur[reg->numero]->aff(trace);trace<<endl;
                                }
                                if (laCouche!=-1)
                                    ModificationDansCouche[laCouche]=OUI;
                                if (elementGauche!=-1) 
                                    InstancieContexte(Phrase,elementGauche,pos-1,OUI);
                                if (elementDroit!=-1) 
                                    InstancieContexte(Phrase,pos+1,elementDroit,OUI);
                            }
                            L->index=voc->CalculIndex();     
                                //dans le cas ou certains noeuds ont ete marques comme
                                //devant etre desambiguise dans le contexte
                                //on appelle cette fonction
                            MajDesAutresCategories(reg,Phrase);    
                        }
                        else
                            if (TRACENOEUD) {                                
                                trace<<CHAINES[228];
                                if (voc->tag[0])
                                    trace<<voc->tag[0];
                                else
                                    trace<<voc->surface;
                                trace<<CHAINES[229]<<endl;
                            }
                    } 
                }
                L=Phrase[++pos];
            }
        }    
    }

    if (TRACENOEUD) {
        trace<<CHAINES[186]<<endl;
        TraceListeMots(this,Phrase);
    }       

    if (laCouche!=-1) {
        if (TRACE)
            trace<<" <--- "<<CHAINES[180]<<laCouche<<endl<<endl;
    }
    return OUI;
}
/*-------------------------------------------------------------------------*/


char GlobalParseur::AppliqueProduitCartesien(LListe& Phrase,int laCouche) {
	char arret_analyse=NON;
    Liste* L;
    VECTA<Cartesien*>* vproduit;
    Cartesien* produit;

    if (laCouche>Limite_Couche) {
		//Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
		//commencer a reappliquer nos regles
		demarre_regle=0;
		demarre_couche=label_stop->couche;
		aller_label=label_stop;
		return OUI;
	}

	vproduit=&TableProduitCartesien[laCouche];
	if (TRACE)
		trace<<" --> "<<CHAINES[180]<<laCouche<<endl<<endl;

    
    if (TRACENOEUD) {
        trace<<CHAINES[182]<<endl;
        TraceListeMots(this,Phrase);
    }       

    //Chaque element de type Cartesien a un index qui correspond 
    //a l'union de ses categories

    for (int ireg=demarre_regle;ireg<vproduit->dernier;ireg++) {
        produit=(*vproduit)[ireg];
		reglecourante=produit;
		if (!testEspaceRegle(produit))
			continue;

        if (produit->activation(this)==NON) {                    
            //Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
            //commencer a reappliquer nos regles
            demarre_regle=0;
            demarre_couche=label_stop->couche;
            aller_label=label_stop;
            break;
        }

        //Dans le cas d'un GOTO on saute les regles avant le label
        if (produit->numero<demarre_regle)
            continue;

        int pos=0;

        L=Phrase[pos];
        while (L!=NULL) {
        //On recherche la premiere categorie qui correspond a cet index (index dans Phrase)
            while (L!=NULL) {

                if (L->voc!=NULL && ((L->index & produit->clef) == produit->clef))
                    break;
                
                L=Phrase[++pos];
            }
            
            if (L!=NULL) {  
                VECTA<Noeud*> noeuds_arguments;
                char solution=NON;                
                char fin=-1;
				initIteration();
                produit->regle->regle->ExtraitMots(this,noeuds_arguments,fin,1,solution,Phrase,pos,produit->regle->deterministe,&produit->listes);
				if (solution==OUI)
					arret_analyse=incrementestats(produit);
            }
            L=Phrase[++pos];
        }
    }
    return OUI;
}

//On applique des regles de vocabulaire en couche
void GlobalParseur::AppliqueVocabulaire(int laCouche) {

	if (EspaceLexique[laCouche]!=NULL && EspaceLexique[laCouche]->actif==NON)
		return;

	if (laCouche>Limite_Couche) {                    
		//Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
		//commencer a reappliquer nos regles
		demarre_regle=0;
		demarre_couche=label_stop->couche;
		aller_label=label_stop;
		return;
	}
    Liste* L;
    for (int i=0; i<NoeudsLexicaux.dernier;i++) {
		L=NoeudsLexicaux[i]->source;
		if (L->voc!=NULL) {			
			bool trouve=NON;
			Vecteur filtreur(iTailleVect);
			for (int ilem=0;ilem<L->voc->lemme.dernier;ilem++) {
                vlex.raz();
				strcpy_s(vlex.surface,2048,L->voc->surface);
                strcpy_s(vlex.lemme,2048,L->voc->lemme[ilem]);
				vlex.init(L->voc->X[ilem],L->voc->tr[ilem]);

				VECTA<void*> a;        
				auto_vocabulaire* auv=NULL;
				RechercheVocabulaire(a,laCouche,vlex.lemme,vlex.surface);

				for (int ia=0;ia<a.dernier;ia++) {
					auv=(auto_vocabulaire*)a[ia];
					TestLexique(auv,OUI,i-1);
				}

				if (a.dernier) {
					if (FusionneTraits(L->traitsbase,L->voc->tr[ilem],filtreur))
						trouve=OUI;
				}
			}
			if (trouve)
				NettoieTraitsIncoherent(L->traitsbase,filtreur);
		}
	}
}
    
//------------------------------------------------------------------------------
//Ce type de regle permet de selection une serie de tokens specifiques.
void GlobalParseur::AppliqueTokenSelection(LListe& Phrase,LListe& NouvelleListe,int laCouche,typeRegle leType) {
    if (laCouche>Limite_Couche) {
        for (int iListe=0;iListe<Phrase.dernier;iListe++)
            NouvelleListe.ajoute(Phrase[iListe]);
		//Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
		//commencer a reappliquer nos regles
		demarre_regle=0;
		demarre_couche=label_stop->couche;
		aller_label=label_stop;
		return;
	}

	ResultatSequence element(this,NULL,NULL);
	VECTA<Noeud*> noeuds_arguments(3);
	VECTA<char> majCategorie;
	initphraseindex(Phrase);	
	initphrasevecteur(Phrase);

	for (int i=0;i<Phrase.dernier;i++) {
		if (Phrase[i]->voc->suivants.dernier!=0) {
			for (int c=0;c<Phrase[i]->voc->nbX;c++) {
				for (int r=0;r<ReglesTokenSelection[laCouche].dernier;r++) {
					TokenSelection* ts=ReglesTokenSelection[laCouche][r];
					if (ts->activation(this)==NON) {                    
						//Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
						//commencer a reappliquer nos regles
						demarre_regle=0;
						demarre_couche=label_stop->couche;
						aller_label=label_stop;
						break;
					}

					//Dans le cas d'un goto on saute les regles avant le label
					if (ts->numero<demarre_regle)
						continue;
					
					Phrase[i]->choixCategorie(this,c);
					Liste* L=Phrase[i];
					noeuds_arguments.raz();
					noeuds_arguments.affecte(0,&L->N);
					element.initialise(this,NULL,NULL);
					majCategorie.raz();
					if (ts->regle->MatchTokens(this,Phrase,i,element,noeuds_arguments,majCategorie) && 
						ts->regle->TestExpressionBooleen(this,noeuds_arguments)) {
							L->voc->garder.raz();
							L->voc->garder.affecte(c,OUI);
							VireCategorie(L->voc,L,OUI);							
							break;
					}
				}
			}
		}
	}
}


