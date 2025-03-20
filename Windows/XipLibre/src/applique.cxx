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
   filename   : applique.cxx
   Date       : 10/01/1999 
   Purpose    : Deduction+sequence+marking rules
   Programmer : Claude ROUX
   Reviewer   : 
*/


#if defined(WIN32) & !defined(_WINDLL)
#include <windows.h>
#endif

extern int teste_differentes_modifications[100];


#include "parsbin.h"
extern ostream* flot_erreur;
void affecteontologielibre(Traits* sommet,Traits* traits,ontologie*);
inline char testn(Noeud* n, ArcArbre* ar) {
    if (ar->negation || (n->source->voc!=NULL && n->source->voc->nbX>1))
        return OUI;
    return (n->X==ar->X);
}
char* TC(GlobalParseur* parseur,Traits* tr);

const char CALCULNEGATION=100;
STVecteur dureeaccumulee=0;
float moyenneapplique;
int appelinf=0;


#if defined(WIN32) & !defined(_WINDLL)
static LARGE_INTEGER tempsdemarre;

void demarreTemps() {
    QueryPerformanceCounter(&tempsdemarre);    
}

void finTemps() {
    appelinf=appelinf+1;
    LARGE_INTEGER fintemps;
    TVecteur duree;
    QueryPerformanceCounter(&fintemps);
    duree = fintemps.QuadPart-tempsdemarre.QuadPart;
    dureeaccumulee+=duree;
    moyenneapplique=dureeaccumulee/appelinf;
}
#endif


//Quelques variables pour les statistiques
float resolution_tentative=0;
float tentative_boucle_dependance=0;
float resolution_reussite=0;
float resolution_quantite=0;
float nombre_boucle_sequence=0;
float nombre_boucle_marquage=0;
float nb_echec_inference=0;
int nombre_boucle_inference=0;
int nombre_derecursivation=0;
int test[9];
Exportation char nouvel_algo=OUI;

int ContexteAnalyse::verifie(VECTA<Noeud*>& vars) {
    int i;
    for (i=0;i<variables.dernier;i++) {
        VECTA<Noeud*>& varis=*variables[i];
        if (varis.dernier!=vars.dernier)
            break;
        char deja=OUI;
        for (int j=0;j<vars.dernier;j++) {
            if (varis[j]!=vars[j]) {
                deja=NON;
                break;   
            }
        }
        if (deja==OUI)
            return i;
    }
    VECTA<Noeud*>* n=variables.cell(variables.dernier);
    if (n==NULL) {
        n=new VECTA<Noeud*>;
        variables.ajoute(n);
    }
    else {
        n->raz();
        variables.dernier++;
    }

    for (i=0;i<vars.dernier;i++)
        n->ajoute(vars[i]);    
    
    return -1;
}

void ListerCategories(STVecteur v,VECTA<int>& vv) {
    
    for (int i=NbCats-1;i>=0;i--) {
        if (v==0)
            return;
        if (v<0) 
            vv.ajoute(i);
        v<<=1;
    }            
}

//On instancie sur les noeuds les valeurs qui ont ete initialise...
void StabiliseTraitsVocabulaire(VECTA<ListeVoc*>& pile,VECTA<Traits*>& treg) {
    for (int i=0;i<pile.dernier;i++) {
        ListeVoc* voc=pile[i];
        if (treg[i]!=NULL) {
            for (int iv=0;iv<voc->nbX;iv++)
                voc->tr[iv]->ou(treg[i]);
        }
    }
}


void PileArcNoeud::ajout(int arg,Noeud* n) {
	varias.affecte(arg,n);
	nbvariables++;
}

void PileArcNoeud::retrait(int arg) {
	varias.affecte(arg,NULL);
	nbvariables--;
}

void PileArcNoeud::raz() {
	vecttr.raz();
	affectation.raz();
	chgts.raz();
	creees.raz();
}


void Piles::ajoute(Traits* tr,Noeud* n,int p,ArcArbre* a) {
	traits.ajoute(tr);
	noeuds.ajoute(n);
	profondeurs.ajoute(p);
	arcs.ajoute(a);
	if (a!=NULL && a->desamb==OUI)
		desamb=OUI;
}


//Nous utilisons ici la possibilite de supprimer des lectures
void SupprimeLecture(GlobalParseur* parseur,Piles& pile) {
	if (pile.desamb==NON)
		return;

	for (int i=0;i<pile.arcs.dernier;i++) {
		if (pile.arcs[i]!=NULL && pile.arcs[i]->desamb==1)
			parseur->filtreCategorie(pile.arcs[i],pile.noeuds[i]->source);
	}
}

//On instancie sur les noeuds les valeurs qui ont ete initialise...
void StabiliseTraitsNoeud(VECTA<Noeud*>& pile,char traitelibre) {
    for (int i=0;i<pile.dernier;i++) {
        Liste* l=pile[i]->source;
        
		if (l!=NULL && l->instanciateur!=NULL) {
			//On remet les traits libres
			if (l->traitsbase->libre!=NULL) {
				if (l->traitsactif->libre==NULL)
					l->traitsactif->libre=new Vecteur(l->traitsbase->libre);
				else
					l->traitsactif->libre->recopie(l->traitsbase->libre);
			}
			else {
				if (l->traitsactif->libre!=NULL) 
					l->traitsactif->libre->raz();
			}
            l->StabiliseTraits(traitelibre);
		}
    }
}


//On instancie sur les noeuds les valeurs qui ont ete initialise...
void InstancieTraitsNoeud(GlobalParseur* parseur,
						VECTA<Traits*>& pile,
						VECTA<Noeud*>& pileNoeuds,
						VECTA<ArcArbre*>& pileArcs,
						char dependance) {
    Liste* l;
    Noeud* n;
	static Vecteur vbloc(1);
	static int tailleinitiale=1;


	if (tailleinitiale<parseur->iTailleVect) {
		vbloc.TailleVect=parseur->iTailleVect;
		delete[] vbloc.vecteur;
		vbloc.vecteur= new TVecteur[parseur->iTailleVect];
		memset(vbloc.vecteur,-1,sizeof(TVecteur)*parseur->iTailleVect);
		tailleinitiale=parseur->iTailleVect;
	}
	ArcArbre* ar;
    for (int i=0;i<pile.dernier;i++) {
        n=pileNoeuds[i];
		ar=pileArcs[i];
        l=n->source;

        l->instanciateur=pile[i];

        if (parseur->TRACE || parseur->TRACEDEPENDANCE) {
            if (pile[i]!=NULL) {
                trace<<(l->X)->nom<<"<"<<l->N.gauche<<"-"<<l->N.droite<<"> = ";
                (l->traitsactif)->TraitsEnChaine(parseur,parseur->Chaine_traits);
                trace<<parseur->Chaine_traits;
                trace<<parseur->CHAINES[8];
                pile[i]->TraitsEnChaine(parseur,parseur->Chaine_traits);
                trace<<parseur->Chaine_traits;
            }
        }
        
        if (l!=NULL) {
            //l=n->source
            //l'instanciateur est alors ecrit
            l->SauveTraits();//duplication du champ traits            
            if (l->instanciateur != NULL) {
				//cas ou il n'y a pas de traits libres sur l->instanciateur
				//on ne tient pas a faire monter ces traits par la suite sur une dependance
				Vecteur* bloc=l->instanciateur->bloquage;
				if (dependance==OUI && l->instanciateur->libre==NULL) {
					//tous les traits deviennent des lors bloques
					l->instanciateur->bloquage=&vbloc;
				}

                parseur->InstancieTraits(l->traitsactif,l->instanciateur); 
				//S'il s'agit d'une regle de marquage
				//On instancie aussi les traits de base
				if (dependance==NON)
					parseur->InstancieTraits(l->traitsbase,l->instanciateur); 

				l->instanciateur->bloquage=bloc;	
                if (parseur->TRACE || parseur->TRACEDEPENDANCE) {   
                    (l->traitsactif)->TraitsEnChaine(parseur,parseur->Chaine_traits);
                    trace<<parseur->CHAINES[7]<<parseur->Chaine_traits<<Endl;
                }
            }
        }        
		//Dans le cas d'un marquage qui attaque aussi les lectures d'un mot
		if (ar!=NULL && ar->selection!=NULL || ar->selectiontous!=NULL) {
			for (int lect=0;lect<l->voc->nbX;lect++) {
				if (ar->selectiontous!=NULL)
					parseur->InstancieTraits(l->voc->tr[lect],ar->selectiontous);
				if (ar->selection!=NULL && (ar->X->Val==0 || ar->X==l->voc->X[lect]) && ITR(parseur,ar->selection,l->voc->tr[lect],NONECRIT))
					parseur->InstancieTraits(l->voc->tr[lect],ar->selection);
			}
		}
	}
}

//-------------------------------------------------------------------------------------
inline void DepileAmb(VECTA<ArcArbre*>& pile,int sp) {

    //On retire les numeros de mot a desambiguiser rajoute lors de la recursion
    for (int ip=pile.dernier-1;ip>sp;ip--) {
        pile[ip]->numliste.retire();
        pile.retire();
    }
}

//--------------------------------------------------------------------------------
inline void Depile(GlobalParseur* parseur,ExtractionFonction& pile,int sp) {
    ResultatFonction* r;
    for (int bp=pile.dernier;bp>sp;bp--) {
        r=pile.retire();
        parseur->RendreResFonc(r);
    }
}

inline void Depile(VECTA<int>* pile,int sp) {
	if (pile==NULL)
		return;
    pile->dernier=sp;
}


inline void Depile(VECTA<Traits*>& pile,int sp) {
    pile.dernier=sp;
}

inline void Depile(VECTA<Noeud*>& pile,int sp) {
    pile.dernier=sp;
}

inline void Depile(VECTA<ListeVoc*>& pile,int sp) {
    pile.dernier=sp;
}
//-----------------------------------------------------------------------------
inline void NettoyageVariable(VECTA<VECTA<Noeud*>*>* variables,VECTA<int>* pileArg,int bp) {
    if (variables!=NULL) {
        for (int i=pileArg->dernier-1;i>=bp;i--)
            variables->vecteur[pileArg->vecteur[i]]->retire();
        pileArg->dernier=bp;
    }
}

//--------------------------------------------------------------------------------
//Cette fonction ne garde dans une pile que les solutions...
inline void ReorganisePile(GlobalParseur* parseur,ExtractionFonction& pile,int bp) {
    
    if (pile.dernier==0 || pile.dernier==bp+1)
        return;

    ResultatFonction* r=pile.fin();
    VECTA<ResultatFonction*> garde(3);

    int i;
    //Cette variable va servir de reference pour la taille des elements que l'on conserve dans la pile
    int nbvar=pile.fin()->nb_noeuds;

    for (i=pile.dernier-1;i>bp;i--) {
        r=pile.retire();

        if (r!=NULL && r->nb_noeuds==nbvar)
            garde.ajoute(r);
        else
            parseur->RendreResFonc(r); 
    }
    
    for (i=garde.dernier-1;i>=0;i--)
        pile.ajoute(garde[i]);  
}

//-------------------------------------------------------------------------------
//Cette methode est utilise pour verifier a l'avance si un noeud est compatible avec un element de la regle
//Comme la comparaison est une operation couteuse, on transmet un booleen pour indiquer a la recursion 
//suivante que la comparaison a deja ete effectuee et qu'elle a abouti a un succes, si elle a fonctionne
//evidemment...

inline char PreTestEtatCategorie(ContexteAnalyse& ct,EtatArbre* et,Noeud* n,char& calcul) {
    calcul=OUI;
    int i;

	ct.bornemin=-1;

    //S'il y a un plus d'un arc, on ne peut rien tester
    if (et->arcs.dernier==0 || et->type==1 || et->etatfin!=0)        
        return OUI;   
    
    for (i=0;i<et->arcs.dernier;i++) {
        if (ct.parseur->CompareNoeudSousArbre(et->arcs[i],n)==OUI) {
            ct.bornemin=i;
            ct.bornemax=et->arcs.dernier;
            calcul=NON;
            return OUI;
        }
    }
    
    return NON;
}

int GlobalParseur::PreTestCategorie(EtatArbre* et,Noeud* n) {
    //S'il y a un plus d'un arc, on ne peut rien tester
    if (et->arcs.dernier==0 || et->type==1 || et->etatfin!=0)        
        return -1;   
    
    for (int i=0;i<et->arcs.dernier;i++) {
		if (CompareNoeudSousArbre(et->arcs[i],n)==OUI) {
            return i;
		}
    }
    
    return -2;
}


int PreTestEtatCategorie(GlobalParseur* parseur,EtatArbre* et,Noeud* n,char& calcul) {
    calcul=OUI;
    int i;
    
    //S'il y a un plus d'un arc, on ne peut rien tester
    if (et->arcs.dernier==0 || et->type==1 || et->etatfin!=0)        
        return OUI;   
    
    char retour=-1;
    for (i=0;i<et->arcs.dernier;i++) {
        if (parseur->CompareNoeudSousArbre(et->arcs[i],n)==OUI) {
            calcul=NON;
            return OUI;
        }
    }
    
    return -1;
}

inline char PreTestArcCategorie(GlobalParseur* parseur,ArcArbre* ar,Noeud* n,char& calcul) {
    calcul=OUI;
    
    //C'est un simple deplacement
    if (ar->X==parseur->XCatEpsilon)
        return OUI;
    
    if (!ar->profondeurNonBornee) {
        if (parseur->CompareNoeudSousArbre(ar,n)==NON) 
            return NON;
        else
            calcul=NON;
    }
    
    return OUI;
}


//--------------------------------------------------------------------------------

char RetireExtremite(PileNoeuds* courant,int & sp) {
    if (courant==NULL)
        return NON;

    for (int i=courant->noeuds.dernier-1;i>=0;i--) {
        if (sp==0)
            break;

        PileNoeuds* pn=courant->pile[i];

        if (pn!=NULL) {       
            char detruire=RetireExtremite(pn,sp);
            if (detruire==OUI) {
                //On le marque pour une destruction ulterieure
                pn->nb=-2;
                //On detruit tous ceux qui sont semblables
                for (int j=i;j>=0 && (courant->pile[j] && courant->pile[j]->nb==-2);j--)
                    courant->pile.retire();
                delete pn;
                if (sp>0) {
                    courant->noeuds.retire();
                    sp--;
                }
            }
        }
        else {
            courant->noeuds.retire();
            sp--;
        }
    }
    
    if (courant->noeuds.dernier==0)
        return OUI;
    return NON;
}

//Remise a zero des variables affectes pendant une recursion
void NettoiePileVariables(PileNoeuds& pile,int sp) {
    int val=pile.nb-sp;

    if (val==0)
        return;

    RetireExtremite(&pile,val);
    pile.nb=sp;
}

void ConstruitListeVariables(PileNoeuds* pile,VECTA<Noeud*>* var, VECTA<VECTA<Noeud*>*>& variables) {
    VECTA<Noeud*> nouveau;

    for (int i=pile->noeuds.dernier-1;i>=0;i--) {
        
        if (var!=NULL)
            nouveau.affecte(*var);
        
        nouveau.affecte(pile->argument,pile->noeuds[i]);
        
        if (pile->pile[i]!=NULL)
            ConstruitListeVariables(pile->pile[i],&nouveau,variables);
        else {//c'est une extremite
            VECTA<Noeud*>* vn=new VECTA<Noeud*>;
            vn->affecte(nouveau);
            variables.ajoute(vn);
        }
    }
}

//--------------------------------------------------------------------------------
//Cette fonction ne garde dans une pile que les solutions...
inline void ReorganisePile(VECTA<PileNoeuds*>& pile,int bp) {
    
    if (pile.dernier==0 || pile.dernier==bp+1)
        return;

    PileNoeuds* r=pile.fin();
    VECTA<PileNoeuds*> garde(3);
    int i;
    //Cette variable va servir de reference pour la taille des elements que l'on conserve dans la pile
    int nbvar=pile.fin()->nb;
    
    for (i=pile.dernier-1;i>bp;i--) {
        r=pile.retire();
        
        if (r!=NULL && r->nb==nbvar)
            garde.ajoute(r);
        else
            delete r;
    }
    for (i=garde.dernier-1;i>=0;i--)
        pile.ajoute(garde[i]);  
}
//-----------------------------------------------------------------------
void AnnuleArg(VECTA<Noeud*>& pile,int arg) {
    if (arg!=-1)
        pile.affecte(arg,NULL);
}
//-----------------------------------------------------------------------
void EtatArbre::ExtraitMots(GlobalParseur* parseur,
                            VECTA<Noeud*>& pile,
                            char& fin,
                            int increment,
                            char& chemin,
                            LListe& Phrase,
                            int iListe,
                            char deterministe,
                            ListeTestArbre* listes) {



	if (parseur->testIteration()==NON)
		return;
	parseur->incIteration();


    ArcArbre* ar;
    EtatArbre* et;
    char solution;
    char debut =NON;
    Noeud* n;
    Liste* L;
    int bc=0;
    int maxbc=arcs.dernier;
    EtatArbre* boucle=this;
    char etatfinal=boucle->etatfin;

    if (fin==OUI)
        return;

    if (fin==-1) {
        fin=NON;
        debut=OUI;
    }

    char erreur=NON;

    if (Phrase[iListe]->N.nombre<nombre)
        return;

    
    while (bc<maxbc && fin==NON) {
        L=Phrase[iListe];
        ar=boucle->arcs[bc];
        et=ar->etatarbre;
        bc++;
        
        if (L==NULL) {
            if (boucle->rechercheCheminEpsilon(parseur->XCatEpsilon)==OUI) {
                etatfinal=1;
                chemin=OUI;
                break;
            }
            AnnuleArg(pile,ar->argument);
            return;
        }

        //Cas particulier, ou le et->etatfinal!=0 et arcs.dernier>0
        //Il s'agit d'un cas ou l'on doit arreter l'analyse toute de suite
        if (boucle->etatfin==1) {
            etatfinal=1;            
            int derarc=boucle->arcs.dernier;
            boucle->arcs.dernier=0;
            boucle->ExtraitMots(parseur,pile,fin,increment,chemin,Phrase,iListe,deterministe,listes);
            boucle->arcs.dernier=derarc;
            chemin=OUI;
        }
        
        int sp=pile.dernier;
        
        //Les comparaisons ne sont executes qu'une seule fois
        if (et->etat==L->N.etat)
            continue;
        
        if ((L->N.clef_suivant & ar->clefsuivante) != ar->clefsuivante)
            continue;

        erreur=OUI;        
        int isuivant=iListe+increment;
        
        char testNoeud=NON;
        char ajoute=NON;
        
        n=&L->N;        
        if (ar->X==parseur->XCatEpsilon) {
            erreur=NON;
            isuivant=iListe;            
        }
        else {
            if (parseur->CompareNoeudSousArbre(ar,n)) {
                erreur=NON;                                    
                if (ar->argument!=-1 && increment==1 && L->voc!=NULL)
                    pile.affecte(ar->argument,n);
            }            
        }    
                       
        sp=pile.dernier;
        if (maxbc==1) {
            if (erreur==OUI)
                return;
            //Dans certain cas, on peut directement sauter un etat
            //lorsque celui-ci est un simple epsilon conduisant a un autre 
            //epsilon            
            while (et->arcs.dernier==1 && et->arcs[0]->X==parseur->XCatEpsilon)
                et=et->arcs[0]->etatarbre;
            //c'est une simple boucle, nul besoin de recursion
            boucle=et;
            bc=0;
            maxbc=boucle->arcs.dernier;            
            iListe=isuivant;            
            etatfinal=boucle->etatfin;            
        }//sinon, cas plus complexe
        else {    
            solution=NON;
            if (erreur==NON) {
                if (Phrase[isuivant]==NULL) {                                           
                    //dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
                    if (et->rechercheCheminEpsilon(parseur->XCatEpsilon)==OUI) {
                        etatfinal=OUI;
                        solution=OUI;
                    }
                }
                else {
                    char calculTest;
                    if (PreTestEtatCategorie(parseur,et,&Phrase[isuivant]->N,calculTest)==OUI) {
                        
                        //Dans certain cas, on peut directement sauter un etat
                        //lorsque celui-ci est un simple epsilon conduisant a un autre 
                        //epsilon
                        while (et->arcs.dernier==1 && et->arcs[0]->X==parseur->XCatEpsilon)
                            et=et->arcs[0]->etatarbre;                          
                        if (et->rechercheCheminEpsilon(parseur->XCatEpsilon)==OUI) {
                            etatfinal=OUI;
                            solution=OUI;
                        }
                        else {
                            if (ar->X!=parseur->XCatEpsilon)
                                Phrase[iListe]->N.etat=boucle->marque;                                
                            et->ExtraitMots(parseur,pile,fin,increment,
                                chemin,Phrase,isuivant,deterministe,listes);
                            Phrase[iListe]->N.etat=-1;
                        }
                    }
                    
                }
                if (solution==OUI)
                    chemin=OUI;
                else
                    AnnuleArg(pile,ar->argument);
            }
        }
    }
       
    if (erreur==OUI && etatfinal==0)
        return;

    if (etatfinal==2 && Phrase[iListe]!=NULL) 
        return;   

    if (chemin==NON && etatfinal==0)
        return;

    Noeud* n2=pile[2];
    Noeud* n1=pile[1];
    if (n1==NULL || n2==NULL)
        return;
    
    ListeVoc* v1=n1->source->voc;        
    ListeVoc* v2=n2->source->voc;        
    
    if (v1->nbX==1 && v2->nbX==1)
        return;
    
    
    Liste* l1=n1->source;
    Liste* l2=n2->source;
    v1->garder.raz();
    v2->garder.raz();
    l1->SauveTraits();
    l2->SauveTraits();
    
    char garde=NON;
    for (int i1=0;i1<v1->nbX;i1++) {
        l1->choixCatTraits(i1);
        
        for (int i2=0;i2<v2->nbX;i2++) {
            l2->choixCatTraits(i2);
            if (listes->TestFormule(parseur,pile,0)) {
                garde=OUI;
                v1->garder.affecte(i1,OUI);
                v2->garder.affecte(i2,OUI);
            }
        }
    }
    
    if (garde==NON) {
        l1->RetrouveTraits();
        l2->RetrouveTraits();
        return;
    }
    
    parseur->VireCategorie(v1,l1,OUI);
    parseur->VireCategorie(v2,l2,OUI);
    l1->RetrouveTraits();
    l2->RetrouveTraits();
    
    chemin=OUI;
    if (deterministe!=NON)
        fin=OUI;
}

//-----------------------------------------------------------------------
char ArcArbre::TestMarquage(GlobalParseur* parseur,
                            Noeud* n,
                            char profondeur,
                            Piles& stacks,
                            Fonction* fonc,
                            VECTA<VECTA<Noeud*>*>* variables,
                            VECTA<int>* pileArg) {
    

    char correct=NON;
    int sp=stacks.dernier();
    int vsp=0;
    if (variables!=NULL)
        vsp=pileArg->dernier;

    if (parseur->CompareNoeudSousArbre(this,n)) {
        //clef contient le codage binaire de toutes les 
        //categories situees sous un meme noeud.
        //cette valeur est systematiquement stockee sur le noeud
        //pere aussi bien pour l'arbre syntaxique
        //Noeud que pour le sous-arbre... En une seule operation, 
        //on peut savoir si les categories situees
        //sous le noeud N contiennent l'ensemble des categories
        //non optionnelles de la regle que l'on applique...
        

        stacks.ajoute(traits,n,profondeur,this);
        n->marquage=-1;
        if (argument!=-1) {
            n->marquage=argument;
            if (variables!=NULL) {
                if ( (*variables)[argument]==NULL)
                    variables->affecte(argument,new VECTA<Noeud*>);
                (*variables)[argument]->ajoute(n);
                pileArg->ajoute(argument);
            }
        }
        
        correct=OUI;
        
        char calcul;
        if (etatarbre->fils != NULL) {
            correct=NON;
            if (n->fils != NULL && (n->fils)->fils!=NULL) {
                if (((etatarbre->clef & n->clef) == etatarbre->clef) && 
                    PreTestEtatCategorie(parseur,etatarbre->fils,n->fils,calcul)==OUI) {
                    char solution=NON;
                    
                    (etatarbre->fils)->Marquage(parseur,solution,n->fils,
                        profondeur+1,stacks,fonc,variables,pileArg);
                    
                    if (solution==OUI) 
                        correct=OUI;
                }
            }
        }                
    }

    if (correct==NON) {
        stacks.depile(sp);
        NettoyageVariable(variables,pileArg,vsp);
    }
    return correct;
}

void EtatArbre::Marquage(GlobalParseur* parseur,
                         char& chemin,
                         Noeud* n,
                         char profondeur,
                         Piles& stacks,
                         Fonction* fonc,
                         VECTA<VECTA<Noeud*>*>* variables,
                         VECTA<int>* pileArg) {
	if (parseur->testIteration()==NON)
		return;
	parseur->incIteration();

    EtatArbre* boucle=this;
    
    char solution;
    Noeud* premierN=n;
    Noeud* dernierN=n;
    int sp=stacks.dernier();
    int vsp=0;    
    char erreur=NON;
    char debut=NON;
    char calcul;

    if (variables!=NULL)
        vsp=pileArg->dernier;

    if (chemin==-1) {
        chemin=NON;
        debut=OUI;
    }

    if (n->nombre<nombre)
        return;

    int bc=0;
    int maxbc=boucle->arcs.dernier;
    Noeud* nsuivant;
    EtatArbre* et;    
    ArcArbre* ar;
    Noeud* ncourant=NULL;
    char etatfinal=boucle->etatfin;    
    while (bc<maxbc) {        
        
        if (n==NULL) {                                           
            //dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
            if (boucle->rechercheCheminEpsilon(parseur->XCatEpsilon)==OUI) {
                etatfinal=1;
                chemin=OUI;
                break;
            }
            stacks.depile(sp);
            NettoyageVariable(variables,pileArg,vsp);
            return;
        }

        //Cas particulier, ou le et->etatfinal!=0 et arcs.dernier>0
        //Il s'agit d'un cas ou l'on doit arrete l'analyse toute de suite
        if (boucle->etatfin==1) {
            etatfinal=1;
            chemin=OUI;
            break;
        }
        
        ar=boucle->arcs[bc];
        et=ar->etatarbre;
        bc++;

        //Les comparaisons ne sont executes qu'une seule fois
        if (et->etat==n->etat)
            continue;                
        
        if ((n->clef_suivant & ar->clefsuivante) != ar->clefsuivante)
            continue;

        //nombre_boucle_marquage++;                        

        erreur=OUI;        
        nsuivant=n->frere;
        char testNoeud=NON;
        char ajoute=NON;
        sp=stacks.dernier();
        if (variables!=NULL)
            vsp=pileArg->dernier;

        
        if (ar->X==parseur->XCatEpsilon) {
            erreur=NON;
            nsuivant=n;            
        }
        else 
			if (ar->TestMarquage(parseur,n,profondeur,stacks,fonc,variables,pileArg)) {
				erreur=NON;
				dernierN=n;
			}                        

        solution=NON;                          
        if (maxbc==1) {
            if (erreur==OUI)
                return;
            //Dans certain cas, on peut directement sauter un etat
            //lorsque celui-ci est un simple epsilon conduisant a un autre 
            //epsilon            
            while (et->arcs.dernier==1 && et->arcs[0]->X==parseur->XCatEpsilon)
                et=et->arcs[0]->etatarbre;
            //c'est une simple boucle, nul besoin de recursion
            boucle=et;
            bc=0;
            maxbc=boucle->arcs.dernier;
            n=nsuivant;
            etatfinal=boucle->etatfin;
        }//sinon, cas plus complexe
        else {    
            if (erreur==NON) {
                if (nsuivant==NULL) {                                           
                    //dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
                    if (et->rechercheCheminEpsilon(parseur->XCatEpsilon)==OUI) {
                        etatfinal=OUI;
                        solution=OUI;
                    }
                }
                else {
                    //dans le cas d'un saut epsilon 
                    //On ne peut passer par la qu'une seule fois                                        
                    if (PreTestEtatCategorie(parseur,et,nsuivant,calcul)==OUI) {
                        if (ar->X!=parseur->XCatEpsilon)
                            n->etat=boucle->etat;                            
                        //Dans certain cas, on peut directement sauter un etat
                        //lorsque celui-ci est un simple epsilon conduisant a un autre 
                        //epsilon
                        while (et->arcs.dernier==1 && et->arcs[0]->X==parseur->XCatEpsilon)
                            et=et->arcs[0]->etatarbre;
                        et->Marquage(parseur,solution,nsuivant,profondeur,stacks,fonc,variables,pileArg);
                        n->etat=-1;
                    }                                        
                }   
                if (solution==OUI) {            
                    chemin=OUI;
                    break;
                }
                else {
                    stacks.depile(sp);
                    NettoyageVariable(variables,pileArg,vsp);
                }
            }
        }
	}

    if (erreur==OUI && etatfinal==0)
        return;

    if (etatfinal==2 && n!=NULL) 
        return;   

    if (chemin==NON && etatfinal==0)
        return;

    if (profondeur==1) {
        //Initialisation dans le cas d'une regle de marquage
        Traits temporaire(parseur);         

            
        if (premierN!=NULL) {
            char untest=NON;
            VECTA<Noeud*> varias;
            if (fonc!=NULL && fonc->listes!=NULL) {
                //on extrait un jeu de variable
                for (int i=0;i<stacks.dernier();i++)
                    if (stacks.noeuds[i]->marquage!=-1)
                        varias.affecte(stacks.noeuds[i]->marquage,stacks.noeuds[i]);
                untest=fonc->listes->Test(parseur,varias);
            }
			else
				untest=OUI;
			if (untest)
				untest=parseur->PTT(&temporaire,premierN,dernierN);
            if (!untest) {
                stacks.depile(sp);
                NettoyageVariable(variables,pileArg,vsp);
                return;
            }
        }
        InstancieTraitsNoeud(parseur,stacks.traits,stacks.noeuds,stacks.arcs,NON);
		StabiliseTraitsNoeud(stacks.noeuds,OUI);
		SupprimeLecture(parseur,stacks);
    }
        
    chemin=OUI;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

char EtatArbre::AppliqueLineaire(GlobalParseur* parseur,
                                 char& chemin,
                                 LListe& Phrase,
                                 int iListe,
                                 int increment,                                 
                                 char maj,
                                 ResultatSequence& r,
								 int lePremier,
                                 VECTA<Noeud*>* noeuds_arguments,
								 VECTA<int>* noeuds_reperes,
                                 VECTA<char>* majCategorie,
								 Sequence* regle) {
    

	if (parseur->testIteration()==NON)
		return NON;
	parseur->incIteration();
    EtatArbre* boucle=this;
    EtatArbre* et;
    ArcArbre* ar;

    Liste* L;    

    VECTA<ArcArbre*> pile(0);

    char solution;

	char vide=NON;
    if (chemin==-1) {
        chemin=NON;      
        r.fin=-1;
		vide=OUI;
    }

    char erreur=NON;
	char presencecategorie=NON;
    int bp=pile.dernier;
    char etatfinal=boucle->etatfin;
    int bc=0;
    int maxbc=boucle->arcs.dernier;
	L=Phrase[iListe];
	int isuivant;
	int spi=-1;

    while (chemin!=OUI && bc<maxbc) {
		if (L==NULL) {
			if (boucle->rechercheCheminEpsilon(parseur->XCatEpsilon)==OUI) {
				etatfinal=1;
				chemin=OUI;
				//dans le cas ou rien n'a ete consomme, il n'y a pas de dernier element...
				break;
			}
			DepileAmb(pile,-1);
			return NON;
		}
		vide=NON;
		//Cas particulier, ou le et->etatfinal!=0 et arcs.dernier>0
        //Il s'agit d'un cas ou l'on doit arreter l'analyse toute de suite
        if (boucle->etatfin==1) {
            etatfinal=1;
            chemin=OUI;
            break;
        }
        int sp=pile.dernier;
		if (noeuds_reperes!=NULL)
			spi=noeuds_reperes->dernier;
        ar=boucle->arcs[bc];
        et=ar->etatarbre;
        bc++;

        //Les comparaisons ne sont executes qu'une seule fois
        if (et->etat==L->N.etat)
            continue;
        
        if (increment==1 && lePremier!=-1) {
            if ((L->N.clef_suivant & ar->clefsuivante) != ar->clefsuivante)
                continue;
        }

		

        //nombre_boucle_sequence++;
        erreur=OUI;        

		if (ar->X==parseur->XCatEpsilon) {
			erreur=NON;
			isuivant=iListe; 
		}
		else {
			char couverture=NON;

			if (parseur->CompareNoeudSousArbre(ar,L,maj,couverture)) {
				erreur=NON;                                    
				isuivant=iListe+increment;
				if (maj==OUI && majCategorie!=NULL && lePremier!=-1) 
					majCategorie->affecte(iListe-lePremier,couverture);
			
				if (noeuds_arguments!=NULL && ar->argument!=-1) {
					noeuds_arguments->affecte(ar->argument,&L->N);
				}

				if (noeuds_reperes!=NULL)
					noeuds_reperes->ajoute(iListe);

				//les ar de ce type permettent la desamb totale d'un noeud dans la liste des mots
				if (ar->desamb==OUI) {
					ar->numliste.ajoute(iListe);
					pile.ajoute(ar);
				}            
			}
		}
        
        //Remarque, dans le cas d'un element optionnel (A* ou (A) ), le systeme
        //integre automatiquement l'element suivant (ar->suivant) dans la regle en premiere position
        //Si l'analyse est longue, cet element suivant devra etre le dernier a etre
        //analysee
        if (maxbc==1) {
            if (erreur==OUI)
                return NON;
            //Dans certain cas, on peut directement sauter un etat
            //lorsque celui-ci est un simple epsilon conduisant a un autre 
            //epsilon            
            while (et->arcs.dernier==1 && et->arcs[0]->X==parseur->XCatEpsilon)
                et=et->arcs[0]->etatarbre;
            //c'est une simple boucle, nul besoin de recursion
            boucle=et;
            bc=0;
            maxbc=boucle->arcs.dernier;            
            iListe=isuivant;            
            etatfinal=boucle->etatfin;  
			L=Phrase[iListe];
			continue;
        }
		
		//sinon, cas plus complexe
		solution=NON;
		if (erreur==NON) {
			if (Phrase[isuivant]==NULL) {                                           
				//dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
				if (et->rechercheCheminEpsilon(parseur->XCatEpsilon)==OUI) {
					etatfinal=OUI;
					solution=OUI;
					break;
				}
			}
			else {
				char calculTest;
				if (PreTestEtatCategorie(parseur,et,&Phrase[isuivant]->N,calculTest)==OUI) {

					//Dans certain cas, on peut directement sauter un etat
					//lorsque celui-ci est un simple epsilon conduisant a un autre 
					//epsilon
					while (et->arcs.dernier==1 && et->arcs[0]->X==parseur->XCatEpsilon)
						et=et->arcs[0]->etatarbre;                                                  

					if (ar->X!=parseur->XCatEpsilon)
						Phrase[iListe]->N.etat=boucle->etat;                            
					et->AppliqueLineaire(parseur,solution,
						Phrase,
						isuivant,
						increment,                                 
						maj,
						r,
						lePremier,
						noeuds_arguments,
						noeuds_reperes,
						majCategorie,
						regle);
					Phrase[iListe]->N.etat=-1;                        
				}                                        
			}           
			if (solution==OUI) {            
				chemin=OUI;
				return OUI;
			}                
			//On revient au status precedent
			if (Phrase[isuivant])
				Phrase[isuivant]->instanciateur=NULL;
			//On retire les numeros de mot a desambiguiser rajoute lors de la recursion
			DepileAmb(pile,sp);                
		}

		Depile(noeuds_reperes,spi);
    }


	if (erreur==OUI && etatfinal==0) {
        DepileAmb(pile,-1);
        return NON;
    }

	if (etatfinal==2 && Phrase[iListe]!=NULL) 
        return NON;   

    if (chemin==NON && etatfinal==0)
        return NON;
	if (noeuds_reperes!=NULL && noeuds_reperes->dernier) {
		regle->L=NULL;                    	
		if (regle->increment==1) {
			if (!regle->TestContexteDroit(parseur,Phrase,noeuds_reperes->fin()+1,*noeuds_arguments))
				return NON;
		}
		else {
			if (!regle->TestContexteGauche(parseur,Phrase,noeuds_reperes->fin()-1,*noeuds_arguments))
				return NON;
		}
		
		r.fin=noeuds_reperes->fin();

		if (regle->label->type==REGLES_SEQUENCE_CONTEXTE) {

			int iDebut=r.debut;
			int iFin=r.fin;
			Liste* boucle=Phrase[iDebut];

			L=parseur->TasDeListes->fournir(parseur,regle->Fx,NULL);
			L->N.numero=regle->identificateur;
			L->N.gauche= boucle->N.gauche;
			L->N.droite= Phrase[iFin]->N.droite;
			L->N.gauchecar= boucle->N.gauchecar;
			L->N.droitecar= Phrase[iFin]->N.droitecar;
			L->N.num_gauche= boucle->N.num_gauche;
			L->N.num_droite= Phrase[iFin]->N.num_droite;


			int ik=iDebut+1;
			boucle->debut=OUI;
			boucle->fin=NON;
			Traits* ins=boucle->instanciateur;
			boucle->N.annule();
			boucle->instanciateur=ins;

			Liste* ld;

			while (ik<=iFin) {
				ld=Phrase[ik];
				ld->debut=NON; //position intermediaire...
				ld->fin=NON;
				ik++;
			}

			Phrase[iFin]->fin=OUI;

			//La valeur #0 est toujours le sommet que l'on construit...
			noeuds_arguments->affecte(0,&L->N);
			if (!parseur->InitTraits(L,Phrase,iDebut,iFin,NULL,r.traits,r.Nombre) ||
				!regle->TestExpressionBooleen(parseur,*noeuds_arguments) ||
				!parseur->AppVregle(L->traitsbase,L->X,PostRCT)) {
					noeuds_arguments->affecte(0,NULL);
					parseur->TasDeListes->RecupereDernier(parseur);
					return NON;
			}
			else
				regle->L=L;
		}		
	}

    chemin=OUI;
	return OUI;    
}
//-------------------------------------------------------------------------

//----------------------------------------------------------------------


/*
La description qui suit vaut pour l'ensemble des procedures ci-dessous
Nous avons cinq possibilites de codage pour les elements suivants:

ar ar+ ar* (ar) (ar1;ar2;ar3)

1) ar: ar->X!=NULL && ar->saute==NON && ar->derivations.dernier==0
2) ar+: ar->X!=NULL && ar->saute==NON && ar->derivations.dernier!=0
3) ar*: ar->X==NULL && ar->saute==OUI && ar->derivations.dernier!=0
4) (ar): ar->X!=NULL && ar->saute==OUI && ar->derivations.dernier!=0
5) (ar1;ar2;ar3): ar->X==NULL && ar->saute==NON && ar->derivations.dernier!=0

Attention, pour un element de type ar+, sa premiere interpretation impose la presence d'un element, mais pour les
interpretations suivantes, il est transforme en ar* (pour cela on transforme son ar->saute==NON en ar->saute==OUI)
*/

inline void ArcArbre::Empile(Noeud* n,char profondeur,PileNoeuds& pile) {
    //dans le cas ou un argument doit etre ajoute
    //on cree un nouvel element (sur base)
    //et on l'ajoute (uniquement si l'on est pas dans la negation: increment=1
    PileNoeuds* variables;    
    int finpile=pile.nb;
    pile.nb++;
    if (pile.noeuds.dernier==0) {
        pile.argument=argument;
        pile.noeuds.ajoute(n);
        pile.profondeur=profondeur;
    }
    else {
        //sinon on parcourt la totalite des noeuds...
        variables=&pile;
        PileNoeuds* prec=NULL;
        while (variables!=NULL) {                            
            if (variables->argument!=argument) {
                prec=variables;
                variables=variables->pile[variables->noeuds.dernier-1];
            }
            else
                break;
        }                
        if (variables==NULL) {
            //On ajoute donc une nouvelle pile en face des derniers noeuds introduits
            int ip;
            for (ip=0;ip<prec->noeuds.dernier;ip++)
                if (prec->pile[ip]==NULL)
                    break;
                //Nous ne creons qu'une seule instance de pile...
                //de facon a conserver une repartition homogene des futures variables
                PileNoeuds* pn=new PileNoeuds(argument,n,profondeur);
                for (;ip<prec->noeuds.dernier;ip++)
                    prec->pile.ajoute(pn);
        }
        else
            variables->noeuds.ajoute(n);
    }
}


inline char ArcArbre::TestNoeud(GlobalParseur* parseur,Noeud* n,char calculComparaison) {
        
    char testNoeud=NON;            

    if (calculComparaison==NON)
        testNoeud=OUI;
    else
        testNoeud=parseur->CompareNoeudSousArbre(this,n);
    
    if (testNoeud==OUI) {        
        
        //clef contient le codage binaire de toutes les 
        //categories situees sous un meme noeud.
        //cette valeur est systematiquement stockee sur le noeud
        //pere aussi bien pour l'arbre syntaxique
        //Noeud que pour le sous-arbre... En une seule operation, 
        //on peut savoir si les categories situees
        //sous le noeud N contiennent l'ensemble des categories
        //non optionnelles de la regle que l'on applique...                
        if (etatarbre->fils != NULL) {
            char correct=NON;
            char calculTest;
            if (n->fils != NULL && (n->fils)->fils!=NULL) {
                if (((etatarbre->clef & n->clef) == etatarbre->clef) && 
                    PreTestEtatCategorie(parseur,etatarbre->fils,n->fils,calculTest)==OUI)
                    correct=OUI;
            }
            if (correct==NON)
                return NON;
        }
        

        return OUI;
    }
    
    return NON;
}

//--------------------------------------------------------------------------------
//Cette methode est appele dans le cadre d'une regle de deduction basee sur un automate d'arbre.
// |automate| where (test) dependance. La difference fondamentale avec Applique reside dans la possibilite
// d'une interpretation non deterministe du test. En revanche, dans Applique, le test reussit si au moins une interpretation
// pour l'expression booleene est vraie.
float resolution_boucle=0;
int boucledinference=0;
void EtatArbre::AppliqueInference(ContexteAnalyse& ct,
                                  char& chemin,                                  
                                  char& fin,
                                  Noeud* n,
                                  char profondeur,
                                  char enProfondeur,
                                  Piles& stacks,
                                  PileNoeuds& pile,
                                  char calculComparaison,
								  char& resultatfinal) {
    

	if (ct.parseur->testIteration()==NON)
		return;
	ct.parseur->incIteration();

	boucledinference++;

	ArcArbre* ar;
    EtatArbre* boucle=this;
    char solution;
    int st=stacks.dernier();
    int sp=pile.nb;
    char calculTest;        
    int bc;
    int sploc,stloc;
    //Pas d'elements lexicaux a ce niveau d'analyse
    if (n!=NULL && n->terminal())
        return;

    int maxbc=arcs.dernier;
    int debutbc=0;


    if (ct.bornemin!=-1) {
        debutbc=ct.bornemin;
        maxbc=ct.bornemax;
        ct.bornemin=-1;
    }


    
    if (fin==OUI)
        return;
    
    if (fin==-1)
        fin=NON;        
    
    if (n->nombre<nombre)
        return;

    char erreur=NON;                
    Noeud* nsuivant;
    EtatArbre* et;
    bc=debutbc;
    Noeud* ncourant=NULL;
    char etatfinal=boucle->etatfin;
    nombre_boucle_inference++;
    //Protection contre les boucles trop longue
    if (ct.parseur->compte_boucle_inference>=1000) {
        if (ct.parseur->compte_boucle_inference==1000 && ct.parseur->AFFICHE_ERREUR==OUI)
            *flot_erreur<<"Rule:"<<ct.fonc->numero<<" too complex. Analysis stopped..."<<endl;
        return;
    }

    while (bc<maxbc && fin==NON) {        
        
        if (n==NULL) {                                           
            //dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
            if (boucle->rechercheCheminEpsilon(ct.parseur->XCatEpsilon)==OUI) {
                etatfinal=1;
                chemin=OUI;
            }
            break;
        }

        if (boucle->etatfin==1) {
            etatfinal=1;
            chemin=OUI;
            int derarc=boucle->arcs.dernier;
            boucle->arcs.dernier=0;
            calculTest=OUI;
			solution=NON;
            boucle->AppliqueInference(ct,solution,fin,n,profondeur,enProfondeur,stacks,pile,calculTest,resultatfinal);
            boucle->arcs.dernier=derarc;
        }

        ar=boucle->arcs[bc];
        et=ar->etatarbre;
        bc++;        

        if (n->nombre<boucle->nombre)
            continue;

        //Les comparaisons ne sont executes qu'une seule fois
        if (et->etat==n->etat)
            continue;

        if ((n->clef_suivant & ar->clefsuivante) != ar->clefsuivante)
            continue;

        resolution_boucle++;                        
        ct.parseur->compte_boucle_inference++;
        
        //On garde les positions dans les piles d'execution
        sp=pile.nb;
        st=stacks.dernier();
        
        erreur=OUI;
        if (ar->profondeurNonBornee==OUI) {            
            stacks.ajoute(ar->traits,n,profondeur);
            
            if (n->fils != NULL && (n->fils)->fils!=NULL) {
                Noeud* fiston=n->fils;
                while (fiston!=NULL) {
                    //On explore les sous-noeuds;
                    solution=NON;
                    sploc=pile.nb;
                    stloc=stacks.dernier();
                    AppliqueInference(ct,
                        solution,
                        fin,
                        fiston,
                        profondeur+1,
                        OUI, //enProfondeur
                        stacks,pile,OUI,resultatfinal);
                    
                    if (solution==OUI) {
                        erreur=NON;
                        chemin=enProfondeur;
                    }
                    else {
                        stacks.depile(stloc);
                        NettoiePileVariables(pile,sploc);
                        fin=NON;
                    }
                    fiston=fiston->frere;
                }
            }
        }
        
        nsuivant=n->frere;                
          
        if (ar->X==ct.parseur->XCatEpsilon) {
            erreur=NON;
            nsuivant=n;            
        }
        else {
            //Le test sur les traits du noeud (en recherche non  bornee) ne
            //peut s'effectuer que sur le noeud le plus haut
            if (ar->X->Val==0 || testn(n,ar)) {                
                if (ar->TestNoeud(ct.parseur,n,calculComparaison)) {
                    erreur=NON;      
                    stacks.ajoute(ar->traits,n,profondeur);
					if (ar->argument!=-1)
                        ar->Empile(n,profondeur,pile);
                    
                    if (et->fils!=NULL) {
                        erreur=OUI;
                        solution=NON;
                        et->fils->AppliqueInference(ct,solution,fin,n->fils,
                            profondeur+1,
                            enProfondeur,
                            stacks,pile,OUI,resultatfinal);
						if (solution==OUI)
                            erreur=NON;
                        else {
                            stacks.depile(st);
                            NettoiePileVariables(pile,sp);
                            fin=NON;
                        }
                    }                
                }
            }        
        }
        calculComparaison=OUI; //evidement cela ne concerne que le premier element        

        solution=NON;                  
        //S'il s'agit d'un etat final avec arcs: cas particulier
        if (maxbc==1) {
            if (erreur==OUI) {
                if (boucle->etatfin) {
                    etatfinal=boucle->etatfin;                    
                    break;
                }
                return;
            }            

            //Dans certain cas, on peut directement sauter un etat
            //lorsque celui-ci est un simple epsilon conduisant a un autre 
            //epsilon                        
            while (et->arcs.dernier==1 && et->arcs[0]->X==ct.parseur->XCatEpsilon)
                et=et->arcs[0]->etatarbre;            
            //c'est une simple boucle, nul besoin de recursion            
            boucle=et;
            bc=0;
            maxbc=boucle->arcs.dernier;
            n=nsuivant;
            etatfinal=boucle->etatfin;
        }//sinon, cas plus complexe
        else {    
            if (erreur==NON) {
                if (nsuivant==NULL) {                                           
                    //dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
                    if (et->rechercheCheminEpsilon(ct.parseur->XCatEpsilon)==OUI) {
                        etatfinal=OUI;
                        solution=OUI;
                        //De plus, il n'est plus besoin de faire de boucle supplementaire...
                        break;
                    }
                }
                else {
                    //dans le cas d'un saut epsilon 
                    //On ne peut passer par la qu'une seule fois     
                    if (PreTestEtatCategorie(ct,et,nsuivant,calculTest)==OUI) {
                        if (ar->X!=ct.parseur->XCatEpsilon)
                            n->etat=boucle->etat;                            
                        //Dans certain cas, on peut directement sauter un etat
                        //lorsque celui-ci est un simple epsilon conduisant a un autre 
                        //epsilon
                        while (et->arcs.dernier==1 && et->arcs[0]->X==ct.parseur->XCatEpsilon)
                            et=et->arcs[0]->etatarbre;
                        
                        et->AppliqueInference(ct,solution,fin,
                            nsuivant,
                            profondeur,enProfondeur,
                            stacks,pile,calculTest,resultatfinal);                        
                        n->etat=-1;
                    }
                                        
                }            
                if (solution==OUI) {
                    chemin=OUI;
                    if (ct.deterministe!=NON)
                        break;
                }
                else {
                    stacks.depile(st);
                    NettoiePileVariables(pile,sp);
                }
            }
        }

		//Dans le cas d'une exploration en profondeur, on ne s'interesse qu'au seul noeud
        //courant...
        if (ar->profondeurNonBornee==OUI && enProfondeur==OUI)
            break;
    }    

    if (etatfinal==2 && n!=NULL) 
        return;   


	if (chemin==NON && etatfinal==0)
        return;

    //Si certains traits sont devenus libres, on les fait percoler sur un noeud temporaire
    //en cas d'echec... fin de la mission...
    //Si increment==-1, on est dans la negation, il ne faut rien generer alors
    if (profondeur==1 && ct.increment==1) {

		//Il faut evidemment qu'on soit sur un etat final
		if (etatfinal==0) {
			if (ct.deterministe==CALCULNEGATION)
				chemin=OUI;
			else
				chemin=NON;
			return;
		}

        if (ct.deterministe==CALCULNEGATION) {
            chemin=OUI;
            return;
        }

        int nbvar=0;
        char resultat=NON;        

        if (pile.nb==0)
            //Si la pile est vide, il faut necessairement qu'aucune variable ne soit attendue
            //sinon, il s'agit d'un cas d'erreur
            if ((ct.fonc->regle)->marque.dernier==0)
                resultat=OUI;

		Noeud* le_dernier=NULL;
		int i;

		for (i=stacks.profondeurs.dernier-1;i>=0;i--) {
			if (stacks.profondeurs[i]==1) {
				le_dernier=stacks.noeuds[i];
				break;
			}
		}

        InstancieTraitsNoeud(ct.parseur,stacks.traits,stacks.noeuds,stacks.arcs,OUI);

        char manqueVariables;
        VECTA<Traits*> vecttr(3);
        VECTA<Traits*> affectation(3);
        VECTA<ResultatFonction*> chgts;
		VECTA<ResultatFonction*> creees;
        
        ContexteInference ci;

		Traits temporaire(ct.parseur);
		Vecteur bloc(ct.parseur->iTailleVect);

        VECTA<VECTA<Noeud*>*> lesVariables;
        ConstruitListeVariables(&pile,NULL,lesVariables);

        //Quelque soit le resultat du calcul sur les inferences, on s'arrete dans le cas
        //d'une analyse deterministe...
        if (ct.deterministe!=NON)
            fin=OUI;           
     
        for (int isp=lesVariables.dernier-1;isp>=0;isp--) {
            if (lesVariables[isp]==NULL)
                return;
            VECTA<Noeud*>& varias = *lesVariables[isp];
		
            char resverif=ct.verifie(varias);
            int iposres=ct.resultats.dernier;
            if (resverif!=-1) {
                resultat=ct.resultats[resverif];
                if (resultat==OUI)
                    continue;
            }
            else                
                ct.resultats.ajoute(0);            

            manqueVariables=NON;
            //verification que toutes les variables sont initialisees
            for (i=0;i<(ct.fonc->regle)->marque.dernier;i++) {
                int vargf=(ct.fonc->regle)->marque[i];
                if (vargf!=NON && varias[i]==NULL) {
                    manqueVariables=OUI;
                    break;
                }
            }

            if (manqueVariables==OUI)
                continue;


            //Recolte des traits libres transmis par les noeuds aux nouvelles dependances
            char bloquer=NON;
            temporaire.razp();
            bloc.raz();
            Traits* transmettre=NULL;
			//On place dans bloc tous les traits libres provenant des noeuds a l'avance			
			//Puis on retire les traits libres qui nous interessent
			//n=stacks.noeuds[0];
			//while (n!=NULL) {
			//	if (n->source!=NULL && n->source->traitsactif->libre) {
			//		bloc.ou(n->source->traitsactif->libre);
			//		bloquer=OUI;
			//		if (n->source->instanciateur!=NULL && n->source->instanciateur->libre)
			//			bloc.etnon(n->source->instanciateur->libre);
			//	}
			//	if (n==ndernier)
			//		break;
			//	n=n->frere;
			//}
			//for (i=0;i<varias.dernier;i++) {
			//	n=varias[i];
			//	if (n!=NULL && n->source!=NULL && n->source->traitsactif->libre) {
			//		bloc.ou(n->source->traitsactif->libre);
			//		bloquer=OUI;
			//		if (n->source->instanciateur!=NULL && n->source->instanciateur->libre)
			//			bloc.etnon(n->source->instanciateur->libre);
			//	}
			//}

            //Seuls les traits des noeuds superieurs sont transmis

			ct.parseur->PTT(&temporaire,stacks.noeuds[0],le_dernier);

            
			//De meme les traits des variables sont aussi transmis...
			for (i=0;i<varias.dernier;i++) {
				n=varias[i];
				if (n!=NULL && n->source->instanciateur!=NULL) {
					if (n->source->instanciateur->libre!=NULL)
						ct.parseur->PTT(&temporaire,n,n);
					else
						affecteontologielibre(&temporaire,n->source->traitsactif,n->source->instanciateur->Onto(ONTOLOGIE_LIBRE));
					if (n->source->instanciateur->bloquage!=NULL) {
						bloc.ou(n->source->instanciateur->bloquage);
						bloquer=OUI;
					}					
				}
			}

            //Si l'on a des traits libres, on les transmet
            if (temporaire.Libre() || temporaire.Onto(ONTOLOGIE_COMPARAISON)!=NULL) {
                if (bloquer==OUI)
                    temporaire.valeur->etnon(&bloc);
                transmettre=&temporaire;
            }

            int idep;
            chgts.raz();
            char reslocal=NON;
			creees.raz();

            //dans le cas ou l'expression ne comprend pas de disjonction
            if (ct.fonc->listes.liste.dernier>0) {
                if (TestClef(ct.parseur,ct.fonc->clef)) {
                    //Deux types de verification
                    if (ct.fonc->typeTest==INSTRUCTION_DETERMINISTE) {
                        //Un simple test introduit par un where
                        reslocal = ct.fonc->listes.Test(ct.parseur,varias);
                        if (reslocal==OUI){
							for (idep=0;idep<ct.fonc->dependances.dernier;idep++) {
								if (ct.parseur->MajDependance(varias,chgts,creees,ct.fonc,ct.fonc->dependances[idep],NULL,idep,transmettre)==OUI) {
									ct.parseur->DestructionNodes(creees);
									break;
								}
							}
                        }
                    }
                    else {
                        if (transmettre!=NULL)
                            vecttr.ajoute(transmettre);                                   
                        if (ct.fonc->typeTest==INSTRUCTION_DEPENDANCE) {
                            ci.initialise(ct.parseur,ct.fonc,varias,NULL);                        
                            //Le "if", on explore toutes les possibilites
                            ct.fonc->listes.calculInference(ci,vecttr,affectation,0,reslocal);
							if (reslocal==NON)
								nb_echec_inference++;
                        }
                        else {//il s'agit d'un iftest
                            //on commence donc a cette regle en particulier
                            //Mais on evite de lancer l'analyse de nouveau
                            ct.fonc->regle_contexte=NON;
                            ct.fonc->premiertest=UTILISECONTEXTE;
                            ct.parseur->DeduitFonction(ct.couche,ct.type_deduction,OUI,ct.fonc->position,&varias,NULL,ct.fonc->finboucle);
                            ct.fonc->premiertest=NON;
                            ct.fonc->regle_contexte=OUI;
                        }
                    }
                }
            }
            else {
                if (ct.fonc->typeTest==INSTRUCTION_DEPENDANCE) {
					for (idep=0;idep<ct.fonc->dependances.dernier;idep++) {
						if (ct.parseur->MajDependance(varias,chgts,creees,ct.fonc,ct.fonc->dependances[idep],NULL,idep,transmettre)==OUI) {
							ct.parseur->DestructionNodes(creees);
							break;
						}
					}
                }
                else {
                    //il s'agit d'un iftest
                    //on commence donc a cette regle en particulier
                    //Mais on evite de lancer l'analyse de nouveau
                    ct.parseur->DeduitFonction(ct.couche,ct.type_deduction,OUI,ct.fonc->position+1,&varias,NULL,ct.fonc->finboucle);
                }
                reslocal=OUI;
            }
            
            if (ct.parseur->aller_label!=NULL)
                fin=OUI;

            ct.resultats.affecte(iposres,reslocal);
            if (reslocal==OUI)
                resultat=OUI;

            vecttr.raz();
            if (reslocal==OUI && ct.deterministe)
                break;
        }
        lesVariables.nettoie();
		if ( resultat==OUI) {
            StabiliseTraitsNoeud(stacks.noeuds,NON);        
			resultatfinal=OUI;
		}
        else
            return;
    }

    chemin=OUI;
}


//-------------------------------------------------------------------------------
//----- Autre version d'applique inference
char coherencepile(ContexteAnalyse& ct,PileArcNoeud& pile) {
	for (int i=0;i<pile.dejautilises.dernier;i++) {
		char trouve=OUI;
		for (int ik=0;ik<pile.dejautilises[i]->dernier;ik++) {
			if (pile.varias[ik]!=(*pile.dejautilises[i])[ik]) {
				trouve=NON;
				break;
			}
		}
		if (trouve==OUI)
			return NON;
	}
	return OUI;
}


char GlobalParseur::CreationDependance(ContexteAnalyse& ct,PileArcNoeud& pile) {

	char fin=NON;

	if (ct.deterministe==CALCULNEGATION)
		fin=OUI;

	int nbvar=0;
	char resultat=NON;        

	Noeud* le_dernier=NULL;
	int i;

	//le noeud le plus profond est notre noeud
	for (i=pile.stacks.profondeurs.dernier-1;i>=0;i--) {
		if (pile.stacks.profondeurs[i]==1) {
			le_dernier=pile.stacks.noeuds[i];
			break;
		}
	}

	//verification que toutes les variables sont initialisees
	for (i=0;i<(ct.fonc->regle)->marque.dernier;i++) {
		if ((ct.fonc->regle)->marque[i]!=NON && pile.varias[i]==NULL)
			return fin;
	}

	for (i=0;i<pile.dejautilises.dernier;i++) {
		char trouve=OUI;
		for (int ik=0;ik<pile.dejautilises[i]->dernier;ik++) {
			if (pile.varias[ik]!=(*pile.dejautilises[i])[ik]) {
				trouve=NON;
				break;
			}
		}
		if (trouve==OUI)
			return fin;
	}

	VECTA<Noeud*>* vnds=new VECTA<Noeud*>;
	vnds->affecte(pile.varias);
	pile.dejautilises.ajoute(vnds);

	fin=OUI;
	pile.raz();

	Traits temporaire(this);
	Vecteur bloc(iTailleVect);
	
	//Quelque soit le resultat du calcul sur les inferences, on s'arrete dans le cas
	//d'une analyse deterministe...
	if (ct.deterministe!=NON)
		fin=SORTIEANALYSE;           

	InstancieTraitsNoeud(this,pile.stacks.traits,pile.stacks.noeuds,pile.stacks.arcs,OUI);

	//Recolte des traits libres transmis par les noeuds aux nouvelles dependances
	char bloquer=NON;
	Traits* transmettre=NULL;
	//Seuls les traits des noeuds superieurs sont transmis
	PTT(&temporaire,pile.stacks.noeuds[0],le_dernier);
	Noeud* n;
	//De meme les traits des variables sont aussi transmis...
	for (i=0;i<pile.varias.dernier;i++) {
		n=pile.varias[i];
		if (n!=NULL && n->source->instanciateur!=NULL) {
			if (n->source->instanciateur->libre!=NULL)
				PTT(&temporaire,n,n);
			else
				affecteontologielibre(&temporaire,n->source->traitsactif,n->source->instanciateur->Onto(ONTOLOGIE_LIBRE));
			if (n->source->instanciateur->bloquage!=NULL) {
				bloc.ou(n->source->instanciateur->bloquage);
				bloquer=OUI;
			}					
		}
	}

	//Si l'on a des traits libres, on les transmet
	if (temporaire.Libre() || temporaire.Onto(ONTOLOGIE_COMPARAISON)!=NULL) {
		if (bloquer==OUI)
			temporaire.valeur->etnon(&bloc);
		transmettre=&temporaire;
	}


	int idep;
	char reslocal=NON;

	//dans le cas ou l'expression ne comprend pas de disjonction
	if (ct.fonc->listes.liste.dernier>0) {
		if (TestClef(this,ct.fonc->clef)) {
			//Deux types de verification
			if (ct.fonc->typeTest==INSTRUCTION_DETERMINISTE) {
				//Un simple test introduit par un where
				reslocal = ct.fonc->listes.Test(this,pile.varias);
				if (reslocal==OUI){
					for (idep=0;idep<ct.fonc->dependances.dernier;idep++) {
						if (MajDependance(pile.varias,pile.chgts,pile.creees,ct.fonc,ct.fonc->dependances[idep],NULL,idep,transmettre)==OUI) {
							DestructionNodes(pile.creees);
							break;
						}
					}
				}
			}
			else {
				if (transmettre!=NULL)
					pile.vecttr.ajoute(transmettre);                                   
				if (ct.fonc->typeTest==INSTRUCTION_DEPENDANCE) {
					pile.ci.initialise(this,ct.fonc,pile.varias,NULL);                        
					//Le "if", on explore toutes les possibilites
					ct.fonc->listes.calculInference(pile.ci,pile.vecttr,pile.affectation,0,reslocal);
					if (reslocal==NON)
						nb_echec_inference++;
				}
				else {//il s'agit d'un iftest
					//on commence donc a cette regle en particulier
					//Mais on evite de lancer l'analyse de nouveau
					ct.fonc->regle_contexte=NON;
					ct.fonc->premiertest=UTILISECONTEXTE;
					DeduitFonction(ct.couche,ct.type_deduction,OUI,ct.fonc->position,&pile.varias,NULL,ct.fonc->finboucle);
					ct.fonc->premiertest=NON;
					ct.fonc->regle_contexte=OUI;
				}
			}
		}
	}
	else {
		if (ct.fonc->typeTest==INSTRUCTION_DEPENDANCE) {
			for (idep=0;idep<ct.fonc->dependances.dernier;idep++) {
				if (MajDependance(pile.varias,pile.chgts,pile.creees,ct.fonc,ct.fonc->dependances[idep],NULL,idep,transmettre)==OUI) {
					DestructionNodes(pile.creees);
					break;
				}
			}
		}
		else {
			//il s'agit d'un iftest
			//on commence donc a cette regle en particulier
			//Mais on evite de lancer l'analyse de nouveau
			DeduitFonction(ct.couche,ct.type_deduction,OUI,ct.fonc->position+1,&pile.varias,NULL,ct.fonc->finboucle);
		}
		reslocal=OUI;
	}

	if (aller_label!=NULL)
		reslocal=OUI;

	if ( reslocal==OUI) {
		StabiliseTraitsNoeud(pile.stacks.noeuds,NON);        
		return fin;
	}
	return NON;
}


char EtatArbre::MotifInference(ContexteAnalyse& ct,
							   int iar,
							   PileArcNoeud& pile,
							   Noeud* n,
							   int profondeur,
							   char calculComparaison) {
    



	//Si nous sommes au bout de nos arcs, nous remontons d'un cran pour analyser la suite
	//de l'expression
	if (ct.parseur->testIteration()==NON)
		return NON;
	ct.parseur->incIteration();
	boucledinference++;
	if (n!=NULL && n->nombre<nombre)
		return NON;



	char solution=NON;
	int maxbc=arcs.dernier;

	//Toute premiere boucle...
	if (calculComparaison==-1) {
		calculComparaison=0;
		maxbc=ct.bornemax;
	}

	char sol=NON;
	EtatArbre* et;
	ArcArbre* ar=arcs[iar];
	Noeud* nn;
	char calculTest;

	int iarsuivant;

	while (arcs.dernier==0 || iar<maxbc) {
		if (ar==NULL || etatfin!=NON) {
			if (pile.e.dernier==0) {
				if (pile.nbvariables==ct.fonc->regle->nbvariables && 
					(etatfin==1 || (etatfin==2 && n==NULL)))
					sol=ct.parseur->CreationDependance(ct,pile);				
			}
			else {
				EtatArbre* ett=pile.e.fin();
				nn=pile.n.fin();
				int pp=pile.p.fin();
				iarsuivant=pile.iar.fin();
				pile.retire();
				sol=ett->MotifInference(ct,iarsuivant,pile,nn,pp,OUI);
				pile.ajoute(ett,nn,pp,iarsuivant);
			}
			if (solution==NON)
				solution=sol;
			if (ar==NULL)
				return solution;
		}

		if (n==NULL) {                                           
			//dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
			//On force le ar a NULL
			if (rechercheCheminEpsilon(ct.parseur->XCatEpsilon,iar)==OUI) {
				ar=NULL;
				iar=maxbc-1;
				continue;
			}
			break;
		}

		et=ar->etatarbre;


		if (et->etat==n->etat || (n->clef_suivant & ar->clefsuivante) != ar->clefsuivante) {
			iar++;
			ar=arcs[iar];
			continue;
		}


		if (ar->X==ct.parseur->XCatEpsilon) {
			sol=et->MotifInference(ct,0,pile,n,profondeur,OUI);
			if (solution==NON)
				solution=sol;
		}
		else {
			//Le test sur les traits du noeud (en recherche non  bornee) ne
			//peut s'effectuer que sur le noeud le plus haut
			if (ar->X->Val==0 || testn(n,ar)) {                
				if (ar->TestNoeud(ct.parseur,n,calculComparaison)) {

					//On teste sur le noeud suivant...
					//Avant toute chose...
					nn=n->frere;
					if (nn==NULL || nn->nombre>=et->nombre) {
						iarsuivant=ct.parseur->PreTestCategorie(et,nn);
						if (iarsuivant!=-2) {							
							if (iarsuivant==-1) {
								calculTest=OUI;
								iarsuivant=0;
							}
							else
								calculTest=NON;


							//On affecte notre variable avec le noeud courant
							if (ar->argument!=-1) {
								if (pile.varias[ar->argument]!=NULL)
									*flot_erreur<<"*WARNING* Rule:"<<ct.fonc->identificateur<<" --> Duplicate variable #"<<ar->argument<<" in the tree pattern."<<endl;
								pile.ajout(ar->argument,n);
								n->etat=etat;
							}
							int st=pile.stacks.dernier();
							pile.stacks.ajoute(ar->traits,n,profondeur,ar);


							//s'il faut explorer les noeuds inferieurs, on va garder dans la pile
							//la position du prochain noeud a analyser
							if (et->fils!=NULL) {
								nn=n->fils;
								if (nn!=NULL && nn->nombre>=et->fils->nombre) {
									int iars=iarsuivant;
									iarsuivant=ct.parseur->PreTestCategorie(et->fils,nn);
									if (iarsuivant!=-2) {																				
										if (iarsuivant==-1) {
											calculTest=OUI;
											iarsuivant=0;
										}
										else
											calculTest=NON;

										//On garde donc dans la pile, l'element suivant
										pile.ajoute(et,n->frere,profondeur,iars);
										//On lance avec le premier element des arcs possibles dessous
										sol=et->fils->MotifInference(ct,iarsuivant,pile,nn,profondeur+1,calculTest);
										pile.retire();
									}
								}
							}
							else
								sol=et->MotifInference(ct,iarsuivant,pile,nn,profondeur,calculTest);

							pile.stacks.depile(st);
							if (ar->argument!=-1) {
								pile.retrait(ar->argument);
								n->etat=-1;
							}

							if (solution==NON)
								solution=sol;

						}
					}
				}
			}
			//Sinon, il faut aussi explorer avec les arcs suivants, sauf si on est en mode deterministe
			if (solution==SORTIEANALYSE)
				return solution;
		}
		//Seul le premier calcul a ete effectue, dans les autres cas, il faut
		//retraiter les analyses...
		calculComparaison=OUI;
		iar++;
		ar=arcs[iar];
	}
	return solution;
}


void EtatArbre::VerifieLineaire(ContexteAnalyse& ct,
                                  char& chemin,  
								  char& fin,
                                  Noeud* n,
                                  Noeud* ndernier,
                                  char calculComparaison) {
    
	if (ct.parseur->testIteration()==NON)
		return;
	ct.parseur->incIteration();
									  
    ArcArbre* ar;
    EtatArbre* boucle=this;
    char solution;
    char calculTest;        
    int bc;
    //Pas d'elements lexicaux a ce niveau d'analyse
    if (n!=NULL && n->terminal())
        return;

    int maxbc=arcs.dernier;
    int debutbc=0;

    if (ct.bornemin!=-1) {
        debutbc=ct.bornemin;
        maxbc=ct.bornemax;
        ct.bornemin=-1;
    }
    
    if (fin==OUI)
        return;
    
    if (fin==-1)
        fin=NON;        
    
    if (n->nombre<nombre)
        return;

    ndernier=n;           

    char erreur=NON;                
    Noeud* nsuivant;
    EtatArbre* et;
    bc=debutbc;
    Noeud* ncourant=NULL;
    char etatfinal=boucle->etatfin;

    while (bc<maxbc && fin==NON) {        
        
        if (n==NULL) {                                           
            //dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
            if (boucle->rechercheCheminEpsilon(ct.parseur->XCatEpsilon)==OUI) {
                etatfinal=1;
                chemin=OUI;
            }
            break;
        }

        if (boucle->etatfin==1) {
            etatfinal=1;
            chemin=OUI;
            int derarc=boucle->arcs.dernier;
            boucle->arcs.dernier=0;
            calculTest=OUI;
			solution=NON;
            boucle->VerifieLineaire(ct,solution,fin,n,ndernier,calculTest);
            boucle->arcs.dernier=derarc;
        }

        ar=boucle->arcs[bc];
        et=ar->etatarbre;
        bc++;        

        if (n->nombre<boucle->nombre)
            continue;

        //Les comparaisons ne sont executes qu'une seule fois
        if (et->etat==n->etat)
            continue;

        if ((n->clef_suivant & ar->clefsuivante) != ar->clefsuivante)
            continue;
                
        nsuivant=n->frere;                
          
        if (ar->X==ct.parseur->XCatEpsilon) {
            erreur=NON;
            nsuivant=n;            
        }
        else {
            //Le test sur les traits du noeud (en recherche non  bornee) ne
            //peut s'effectuer que sur le noeud le plus haut
            if (ar->X->Val==0 || testn(n,ar)) {                
                if (ar->TestNoeud(ct.parseur,n,calculComparaison)) {
                    erreur=NON;      
				}
			}
		}
        
		calculComparaison=OUI; //evidement cela ne concerne que le premier element        
        solution=NON;                  
        //S'il s'agit d'un etat final avec arcs: cas particulier
        if (maxbc==1) {
            if (erreur==OUI) {
                if (boucle->etatfin) {
                    etatfinal=boucle->etatfin;                    
                    break;
                }
                return;
            }            

            //Dans certain cas, on peut directement sauter un etat
            //lorsque celui-ci est un simple epsilon conduisant a un autre 
            //epsilon                        
            while (et->arcs.dernier==1 && et->arcs[0]->X==ct.parseur->XCatEpsilon)
                et=et->arcs[0]->etatarbre;            
            //c'est une simple boucle, nul besoin de recursion            
            boucle=et;
            bc=0;
            maxbc=boucle->arcs.dernier;
            n=nsuivant;
            etatfinal=boucle->etatfin;
        }//sinon, cas plus complexe
        else {    
            if (erreur==NON) {
                if (nsuivant==NULL) {                                           
                    //dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
                    if (et->rechercheCheminEpsilon(ct.parseur->XCatEpsilon)==OUI) {
                        etatfinal=OUI;
                        solution=OUI;
                        //De plus, il n'est plus besoin de faire de boucle supplementaire...
                        break;
                    }
                }
                else {
                    //dans le cas d'un saut epsilon 
                    //On ne peut passer par la qu'une seule fois     
                    if (PreTestEtatCategorie(ct,et,nsuivant,calculTest)==OUI) {
                        if (ar->X!=ct.parseur->XCatEpsilon)
                            n->etat=boucle->etat;                            
                        //Dans certain cas, on peut directement sauter un etat
                        //lorsque celui-ci est un simple epsilon conduisant a un autre 
                        //epsilon
                        while (et->arcs.dernier==1 && et->arcs[0]->X==ct.parseur->XCatEpsilon)
                            et=et->arcs[0]->etatarbre;
                        
                        et->VerifieLineaire(ct,solution,fin,nsuivant,ndernier,calculTest);                        
                        n->etat=-1;
                    }
                                        
                }            
                if (solution==OUI) {
                    chemin=OUI;
                    if (ct.deterministe!=NON)
                        break;
                }
            }
        }
    }    

    if (etatfinal==2 && n!=NULL) 
        return;   


	if (chemin==NON && etatfinal==0)
        return;

    //Si certains traits sont devenus libres, on les fait percoler sur un noeud temporaire
    //en cas d'echec... fin de la mission...
    //Si increment==-1, on est dans la negation, il ne faut rien generer alors
    if (ct.increment==1) {
		//Il faut evidemment qu'on soit sur un etat final
		if (etatfinal==0) {
			if (ct.deterministe==CALCULNEGATION)
				chemin=OUI;
			else
				chemin=NON;
			return;
		}

        if (ct.deterministe==CALCULNEGATION) {
            chemin=OUI;
            return;
        }
    }

    chemin=OUI;
}

void EtatArbre::Decoupe(GlobalParseur* parseur,
						Sequence* seq,
                        char& chemin,
                        VECTA<ListeVoc*>& Phrase,
                        int iListe,
                        int increment,
                        VECTA<ListeVoc*>& pile,
                        VECTA<Traits*>& piletraits,
                        int& coupe,
                        char stabilise,
						VECTA<ListeVoc*>* variables) {

	if (parseur->testIteration()==NON)
		return;
	parseur->incIteration();

    EtatArbre* boucle=this;
    EtatArbre* et;
    ArcArbre* ar;    

    ListeVoc* L;

    char solution;

    if (chemin==-1)
        chemin=NON;      

    int bc=0;
    char erreur=NON;
    int bp=pile.dernier;
    char etatfinal=boucle->etatfin;
    int maxbc=boucle->arcs.dernier;
	while (chemin!=OUI && bc<maxbc) {
		int sp=pile.dernier;
		L=Phrase[iListe];
		if (L==NULL) {
			if (boucle->rechercheCheminEpsilon(parseur->XCatEpsilon)==OUI) {
				etatfinal=1;
				chemin=OUI;
				break;
			}
			Depile(pile,sp);
			Depile(piletraits,sp);
			return;
		}

		if (boucle->etatfin==1) {
			etatfinal=1;
			chemin=OUI;
			break;
		}

		ar=boucle->arcs[bc];
		et=ar->etatarbre;
		bc++;

		//Les comparaisons ne sont executes qu'une seule fois
		if (et->etat==L->etat)
			continue;

		erreur=OUI;        
		int isuivant=iListe+increment;

		char testNoeud=NON;
		char ajoute=NON;        

		if (ar->X==parseur->XCatEpsilon) {
			erreur=NON;
			isuivant=iListe;            
		}
		else {           
			if (parseur->CompareEntree(L,ar->X,ar->traits,ar->selectiontous,ar->selection)) {
				erreur=NON;                                    
				if (ar->argument!=-1) {
					if (ar->argument==0) {
						coupe=iListe;
						if (seq!=NULL)
							seq->deja=coupe;
					}
					if (variables!=NULL) {
						variables->affecte(ar->argument,L);
						L->idx=iListe;
					}
				}
			}
		}

		//Remarque, dans le cas d'un element optionnel (A* ou (A) ), le systeme
		//integre automatiquement l'element suivant (ar->suivant) dans la regle en premiere position
		//Si l'analyse est longue, cet element suivant devra etre le dernier a etre
		//analysee

		solution=NON;
		if (maxbc==1) {
			if (erreur==OUI)
				return;
			//Dans certain cas, on peut directement sauter un etat
			//lorsque celui-ci est un simple epsilon conduisant a un autre 
			//epsilon            
			while (et->arcs.dernier==1 && et->arcs[0]->X==parseur->XCatEpsilon)
				et=et->arcs[0]->etatarbre;
			//c'est une simple boucle, nul besoin de recursion
			boucle=et;
			bc=0;
			maxbc=boucle->arcs.dernier;            
			iListe=isuivant;            
			etatfinal=boucle->etatfin;      
			if (etatfinal)
				solution=OUI;
		}//sinon, cas plus complexe
		else {    			
			if (erreur==NON) {
				if (Phrase[isuivant]==NULL) {                                           
					//dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
					if (et->rechercheCheminEpsilon(parseur->XCatEpsilon)==OUI) {
						etatfinal=OUI;
						solution=OUI;
					}
				}
				else {
					//Dans certain cas, on peut directement sauter un etat
					//lorsque celui-ci est un simple epsilon conduisant a un autre 
					//epsilon
					while (et->arcs.dernier==1 && et->arcs[0]->X==parseur->XCatEpsilon)
						et=et->arcs[0]->etatarbre;                          
					if (et->rechercheCheminEpsilon(parseur->XCatEpsilon)==OUI) {
						etatfinal=OUI;
						solution=OUI;
					}
					else {
						if (ar->X!=parseur->XCatEpsilon)                            
							Phrase[iListe]->etat=boucle->etat;                            
						et->Decoupe(parseur,seq,solution,Phrase,
							isuivant,increment,pile,piletraits,
							coupe,stabilise,variables);
						Phrase[iListe]->etat=-1;
					}                    
				}       
			}
		}
		if (solution==OUI) {            
			if (seq!=NULL && variables!=NULL) {
				if (seq->deja==coupe) {
					if (seq->listes!=NULL) {							
						VECTA<Noeud*> vect_noeuds;
						int i;
						for (i=0;i<variables->dernier;i++)
							vect_noeuds.affecte(i,parseur->ProduitListe((*variables)[i]));
						Noeud* premier=NULL;
						Noeud* ncourant;
						//On construit notre liste de noeuds attaches les uns aux autres
						for (i=0;i<=coupe;i++) {
							ncourant=parseur->ProduitListe(parseur->phrase[i]);
							if (premier==NULL)
								premier=ncourant;
							else
								premier->insere(ncourant);
						}
						if (seq->TestExpressionBooleen(parseur,vect_noeuds)) {						
							seq->deja=-1;
							chemin=OUI;
							return;
						}
					}
					else {
						chemin=OUI;
						return;
					}
					solution=NON;
				}
				else {
					if (seq->deja==-1) {
						chemin=OUI;
						return;
					}
					solution=NON;
				}
			}
			else {
				chemin=OUI;
				return;
			}
		}
		Depile(pile,sp);
		Depile(piletraits,sp);
	}

    if (erreur==OUI && etatfinal==0)
        return;

    if (etatfinal==2 && Phrase[iListe]!=NULL) 
        return;   

    if (chemin==NON)
        return;

    if (stabilise==OUI)
        StabiliseTraitsVocabulaire(pile,piletraits);

    chemin=OUI;
}


//-------------------------------------------------------------------------------------------------

void GlobalParseur::AppliqueSemantiques(int laCouche,char typecouche) {

    if (Semantiques[laCouche][typecouche]==NULL)
        return;
	
	static VECTA<Traits*> vecttr;
	static VECTA<Traits*> affectation; 

	VECTA<Noeud*> variables(5);
    VECTA<char*> lesLemmes;
	VECTA<void*> a;
    int ilem;

    for (int i=0; i<NoeudsLexicaux.dernier;i++) {
        //Pour chacun des lemmes...
        lesLemmes.raz();
        if (NoeudsLexicaux[i]->source->voc!=NULL) {
            for (ilem=0;ilem<NoeudsLexicaux[i]->source->voc->lemme.dernier;ilem++) {
                char* lem=NoeudsLexicaux[i]->source->voc->lemme[ilem];
                if (lem!=NULL) {
                    char trouve=NON;
                    for (int li=0;li<lesLemmes.dernier;li++) {
                        if (!strcmp(lesLemmes[li],lem)) {
                            trouve=OUI;
                            break;
                        }
                    }
                    if (trouve==NON)
                        lesLemmes.ajoute(lem);
                }
            }
        }

        for (int ilem=0;ilem<lesLemmes.dernier;ilem++) {
            a.raz();
            //dans le cas ou l'expression ne comprend pas de disjonction
            Semantiques[laCouche][typecouche]->recherchetouslabels(lesLemmes[ilem],a);       
            
            for (int ia=0;ia<a.dernier;ia++) {
                auto_vocabulaire_semantique* voc_sem= (auto_vocabulaire_semantique*)a[ia];
                
                char resultat=NON;
                Traits* traitsDuMot=NoeudsLexicaux[i]->lesTraits();
                Cat* categ=NULL;
                if (NoeudsLexicaux[i]->source!=NULL)
                    categ= (NoeudsLexicaux[i]->source)->X;
                char positionner=OUI;
                while (voc_sem!=NULL) {
                    
                    //On recherche une entree qui convient dans la liste des regles
                    while (voc_sem!=NULL) {      
                        if (voc_sem->X==categ || voc_sem->X==XCatVide) {
                            if (voc_sem->traits_mot==NULL || 
                                ITR(this,voc_sem->traits_mot,traitsDuMot,NONECRIT))
                                break;
                        }
                        voc_sem=voc_sem->Autre();
                    }
                    
                    if (voc_sem==NULL)
                        break;
                    
                    if (voc_sem->charge==NON)
                        ChargeSem(voc_sem,positionner);
                    else
                        voc_sem->compteur=memoiresem.max;
                    
                    if (voc_sem->charge==NON) {
                        voc_sem=voc_sem->Autre();
                        continue;
                    }
                    
                    VectTestFonctionSemantique* fonc=voc_sem->semantique;					
				    variables.affecte(0,NoeudsLexicaux[i]);
                    
                    if (fonc->listes.liste.dernier>0) {
                        if (TestClef(this,fonc->clef)) {
                            ContexteInference ci;
                            vecttr.raz();
                            affectation.raz();
                            ci.initialise(this,fonc,variables,NULL);
                            fonc->listes.calculInference(ci,vecttr,affectation,0,resultat);
                        }
                    }
                    else {
                        static VECTA<ResultatFonction*> chgts;
						chgts.raz();
                        for (int idep=0;idep<fonc->dependances.dernier;idep++)
                            MajDependance(variables,chgts,chgts,fonc,fonc->dependances[idep],NULL,idep,NULL);
                        resultat=OUI;
                    }
                    
                    if (resultat==OUI) {
                        MajINTERFONCTIONS();
                        if (voc_sem->traits_mot!=NULL)
                            InstancieTraits(traitsDuMot,voc_sem->traits_mot);
                    }
                    
                    voc_sem=voc_sem->Autre();
                }        
            }
        }
    }
}



inline char comparecat(Noeud* n, Cat* Y) {
	if (Y->val==0)
		return OUI;
	if (n->X==Y)
		return OUI;
	return NON;
}
//-------------------------------------------------------------------------------------------
char TestTraitsPresent(VECTA<Noeud*>& noeuds,int taille,int index, Vecteur* present) {
	Vecteur ramasse(taille);
	for (;index<noeuds.dernier;index++) 
		ramasse.ou(noeuds[index]->source->traitsactif->valeur);
	TVecteur* p=present->vecteur;
	TVecteur* r=ramasse.vecteur;

	for (index=0;index<taille;index++) {
		if ((*r&*p)!=*p)
			return NON;
		r++;
		p++;
	}
	return OUI;
}
int bouclageinterne=0;
//-------------------------------------------------------------------------------------------
void EtatArbre::ResolutionFictif(GlobalParseur* parseur,VectTFInstruction* vtf,
                                 char& resultat,
                                 typeDeduction type_deduction,
                                 int lacouche,
                                 VECTA<Noeud*>& noeudreference) {

	if (parseur->testIteration()==NON)
		return;
	parseur->incIteration();
   
	//if (!TestClef(parseur,vtf->clef))
    //    return;

	//Intersection entre le noeud TOP et la regle elle-meme
	//if ((parseur->ClefArbre & vtf->regle->clef) != vtf->regle->clef)
	//	return;
	if (vtf->listes.liste.dernier>0) {
		if (vtf->listes.TestDependances(parseur,0)==NON)
			return;
	}

	
    
    ContexteAnalyse ct; 
    int categorie=0;

    for (int iarc=0;iarc<arcs.dernier;iarc++) {
        ArcArbre* larc=arcs[iarc];        
            
        //Dans le cas d'un epsilon, il faut aller chercher la categorie suivante
        if (larc->X==parseur->XCatEpsilon) {
            if (larc->etatarbre->arcs.dernier>1 ||
                larc->etatarbre->arcs[0]->X==parseur->XCatEpsilon) {
                larc->etatarbre->ResolutionFictif(parseur,vtf,resultat,type_deduction,lacouche,noeudreference);
                continue;
            }
            else 
                larc=larc->etatarbre->arcs[0];
        }

        if (larc->profondeurNonBornee==OUI)
            categorie=0;
        else
            categorie=larc->X->val;

		LListe& listenoeuds=parseur->IndexParCat[categorie];
        for (int i=0; i< listenoeuds.dernier;i++) {  
			tentative_boucle_dependance+=0.01;
			bouclageinterne++;
            Noeud* n=&listenoeuds[i]->N;            
            char calculTest;
			
			if (vtf->regle->aplat!=0) {
				if ( (n->clefsurindex & vtf->regle->aplat)!=vtf->regle->aplat)
					continue;
			}

            //On verifie si les noeuds suivants sont compatibles avec la regle
            if ((n->clef_suivant  & vtf->regle->clef) != vtf->regle->clef) {
                //test[0]++;
                continue;
            }
            
            //On regarde ensuite s'il y assez de noeuds pour que la regle puisse s'appliquer
            if (n->nombre<vtf->regle->nombre) {
                //test[1]++;
                continue;
            }
            
            //Test sur la compatibilite entre le pere du noeud courant et la regle qui doit s'appliquer
            Noeud* nn;
            char calcul;                        
            
            //Tous les tests qui suivent sont destines a tester un cran en avant l'analyse.
            //Des statistiques sur le comportement du code, montrent que les regles echouent
            //en moyenne sur le deuxieme element de la regle, qu'il soit le suivant ou le fils
            //du premier element de la regle.
            
            //On teste sur le suivant, si le premier element n'est pas optionnel
            calculTest=1;
            ct.initialise(parseur,1,vtf->deterministe,vtf,type_deduction,lacouche);
            
            
            if (!parseur->CompareNoeudSousArbre(larc,n)) {
                //test[8]++;
                continue; 
            }
            
            EtatArbre* suivant=larc->etatarbre;
            nn=n->frere;
            if (nn==NULL) {
                if (!suivant->rechercheCheminEpsilon(parseur->XCatEpsilon)){
                    //test[3]++;
                    continue;
                }
            }
            else {
                if (PreTestEtatCategorie(ct,suivant,nn,calcul)==NON) {
                    //test[4]++;
                    continue;
                }
            }
            calculTest=0;
            
            //On teste sur le fils
            if (larc->etatarbre->fils!=NULL) {
                nn=n->fils;
                if (nn==NULL || nn->fils==NULL) {
                    //test[5]++;
                    continue;
                }
                
                if ((n->clef & larc->etatarbre->clef) != larc->etatarbre->clef) {
                    //test[6]++;
                    continue;
                }
                
                if (PreTestEtatCategorie(ct,larc->etatarbre->fils,nn,calcul)==NON) {
                    //test[7]++;
                    continue; 
                }
            }

			char fin=-1;
            resolution_tentative++;
            

            char chemin=NON;
			char resultatfinal=NON;
			fin=-1;    
            parseur->compte_boucle_inference=0;
            resolution_boucle=0;                 
            ct.bornemin=iarc;  
            ct.bornemax=iarc+1;  
            chemin=NON;            
			parseur->initIteration();

			//if (nouvel_algo==NON) {
			//	PileNoeuds pile;
			//	Piles stacks;
			//	stacks.raz();
			//	//Nous nous servons de nos noeuds initiaux pour initialiser les variables
			//	stacks.noeuds.affecte(noeudreference);
			//	AppliqueInference(ct,chemin,fin,n,1,NON,stacks,pile,calculTest,resultatfinal);               
	  //          pile.nettoie();
			//}
			//else {
				//PileArcNoeud mapile;
			parseur->pilebase.initialise();
			parseur->pilebase.stacks.noeuds.affecte(noeudreference);
			calculTest=-1;
			resultatfinal=MotifInference(ct,iarc,parseur->pilebase,n,1,calculTest);
			//}

            if (resultatfinal) {
                resolution_reussite++;
                resultat=OUI;
            }
            else 
                resolution_quantite++;
            
            parseur->MajINTERFONCTIONS();

            //des que l'on a un resultat positif dans le cas d'un contexte negation
            //c'est une erreur
            if (ct.deterministe==CALCULNEGATION && resultat==OUI) {
                resultat=NON;
                return;
            }
        }
    }

    //Si l'on a aucun resultat positif pendant l'analyse
    //c'est une bonne reponse dans le cas de la negation
    if (ct.deterministe==CALCULNEGATION && resultat==NON) {
        resultat=OUI;
        return;
    }
}

