*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003, Xerox Corporation, XRCE-Grenoble.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 3.16
   filename   : sousarbre.cxx
   Date       : 12/09/2003
   Purpose    : Deduction+sequence+marking rules (old ways)
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"

//------------------------------------------------------------------------
extern float nombre_boucle_sequence;
extern float nombre_boucle_marquage;
//------------------------------------------------------------------------
void StabiliseTraitsVocabulaire(VECTA<ListeVoc*>& pile,VECTA<Traits*>& treg);
void StabiliseTraitsNoeud(VECTA<Noeud*>& pile);
void InstancieTraitsNoeud(GlobalParseur* parseur,VECTA<Traits*>& pile,VECTA<Noeud*>& pileNoeuds);
void AnnuleArg(VECTA<Noeud*>& pile,int arg);
//------------------------------------------------------------------------
inline char PreTestCategorie(SousArbre* ar,Cat* X) {

    if (ar->X != NULL && ar->X->Val!=0 && !ar->derivations.dernier)
        //si ce noeud ne peut etre compare, on le saute
        if (ar->X != X)
            return NON;

    return OUI;
}

inline char PreTestCategorie(GlobalParseur* parseur,SousArbre* ar,Noeud* n,char& calcul) {
    calcul=OUI;
    if (ar->X!=NULL && !ar->profondeurNonBornee && ar->plus!=SAUTE) 
        if (ar->saute==NON || !ar->derivations.dernier) {
            if (parseur->CompareNoeudSousArbre(ar,n)==NON) 
                return NON;
            else
                calcul=NON;
        }
    return OUI;
}

inline void NettoyageVariables(VECTA<VECTA<Noeud*>*>* variables,VECTA<int>* pileArg,int bp) {
    if (variables!=NULL) {
        for (int i=pileArg->dernier-1;i>=bp;i--)
            variables->vecteur[pileArg->vecteur[i]]->retire();
        pileArg->dernier=bp;
    }
}
//------------------------------------------------------------------------
inline void DepileAmbig(VECTA<SousArbre*>& pile,int sp) {

    //On retire les numeros de mot a desambiguiser rajoute lors de la recursion
    for (int ip=pile.dernier-1;ip>sp;ip--) {
        pile[ip]->numliste.retire();
        pile.retire();
    }
}

//------------------------------------------------------------------------
char PreTestNoeudCat(GlobalParseur* parseur,SousArbre* ar,Liste* L) {
    Noeud* n=NULL;
    if (L!=NULL)
        n=&L->N;

    if (ar->X!=NULL && !ar->profondeurNonBornee && ar->plus!=SAUTE) 
        if (ar->saute==NON || !ar->derivations.dernier) {
            if (parseur->CompareNoeudSousArbre(ar,n)==NON) 
                return NON;
        }
    
    return OUI;
}

inline void Depilage(VECTA<Traits*>& pile,int sp) {
    pile.dernier=sp;
}

inline void Depilage(VECTA<Noeud*>& pile,int sp) {
    pile.dernier=sp;
}

inline void Depilage(VECTA<ListeVoc*>& pile,int sp) {
    pile.dernier=sp;
}

//--------------------------------------------------------------------------------
//Cette methode est appele dans le cadre d'une regle de deduction basee sur un automate d'arbre.
// |automate| where (test) dependance. La difference fondamentale avec Applique reside dans la possibilite
// d'une interpretation non deterministe du test. En revanche, dans Applique, le test reussit si au moins une interpretation
// pour l'expression booleene est vraie.
void SousArbre::ExtraitMots(GlobalParseur* parseur,
                            VECTA<Noeud*>& pile,
                            char& fin,
                            int increment,
                            char& chemin,
                            LListe& Phrase,
                            int iListe,
                            char deterministe,
                            ListeTestArbre* listes) {




#ifndef XIP_LIGHT
    SousArbre* ar=this;
    char solution;
    char debut =NON;
    Noeud* n;
    Liste* L;

    if (fin==OUI)
        return;

    if (fin==-1) {
        fin=NON;
        debut=OUI;
    }

    char erreur=NON;

    while (fin==NON && ar != NULL && erreur==NON) {        
        L=Phrase[iListe];

        if (L==NULL)
            return;

        n=&L->N;
        erreur=OUI;
        char testNoeud=NON;
        char ajoute=NON;

        if (ar->X==NULL)
            erreur=NON;
        else 
            testNoeud=parseur->CompareNoeudSousArbre(ar,n);

        if (testNoeud==OUI) {
            erreur=NON;
            if (ar->argument!=-1 && increment==1 && L->voc!=NULL)
                pile.affecte(ar->argument,n);
        }
            
        char proms=OUI;
        int ider=0; 
        char boucle=OUI;

        //Dans le cas d'une structure A+, si erreur==OUI, cela signifie que cette boucle n'a aucun interet, puisqu'il y
        //a deja eu echec. Par consequent on n'entre pas...
        if (!ar->derivations.dernier || (ar->saute==NON && ar->plus!=SAUTE && erreur==OUI))
            boucle=NON;

        while (boucle) {

            solution=NON;
            int isuivant=iListe;
            
            SousArbre* ars;

            if ((ar->X!=NULL || ar->saute==OUI) &&
                ((deterministe!=longue && proms==OUI) || 
                (deterministe==longue && ider==ar->derivations.dernier))) {

                if (deterministe==longue)
                    boucle=NON;

                ars=ar->suivant;

                if (ar->saute==NON && ar->plus!=SAUTE && ars!=NULL) //dans ce cas, il faut se positionner sur l'element suivant
                    isuivant++;
            }
            else {
                //dans le cas ou l'on a deja analyse tous les cas de
                //derivations, on s'arrete
                if (ider==ar->derivations.dernier)
                    break;

                ars=ar->derivations[ider];
                if (ar->reboucle==ider)
                    isuivant++;
                ider++;
            }

            //Dans le cas d'une analyse courte, on s'arrete des que le dernier element a ete analyse
            //Dans le cas d'une analyse longue, il faut aussi pouvoir analyser l'element suivant
            if (deterministe!=longue && ider==ar->derivations.dernier)
                boucle=NON;
            
            //s'il s'agit d'une boucle avec une erreur detectee
            //on doit relancer l'analyse sur l'element suivant de la regle
            //mais toujours sur le noeud courant
            if (ars==ar && erreur==OUI)
                continue;

            proms=NON;
            
                //si nous sommes au-dela des bornes, on place -1 dans isuivant comme indication
            if (isuivant>=0 && isuivant< Phrase.dernier) {
                if (ars==NULL) {
                    if (erreur==NON)
                        solution=OUI;
                }
                else {
                    char leplus=ar->plus;
                    if (ar->plus==OUI)
                        ar->plus=SAUTE;
                    ars->ExtraitMots(parseur,pile,fin,increment,solution,Phrase,isuivant,deterministe,listes);
                    ar->plus=leplus;
                }
            }

            if (solution==OUI)
                chemin=OUI;
            else
                fin=NON;
                

            if (fin==OUI)
                return;
        }

        if (ar->derivations.dernier!=0 || erreur==OUI) {
            AnnuleArg(pile,ar->argument);
            return;
        }

        debut=NON;

        iListe++;
        ar=ar->suivant;
    }


    if (fin==BLOQUEE)
        fin=NON;

    if (erreur==OUI)
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

    char garder=NON;
    for (int i1=0;i1<v1->nbX;i1++) {
        l1->choixCatTraits(i1);
           
        for (int i2=0;i2<v2->nbX;i2++) {
            if (v2->garder[i2]==OUI)
                continue;
            l2->choixCatTraits(i2);
            if (listes->TestFormule(parseur,pile,0)) {
                garder=OUI;
                v1->garder.affecte(i1,OUI);
                v2->garder.affecte(i2,OUI);
            }
        }
    }

    if (garder==NON)
        return;

    parseur->VireCategorie(v1,l1,OUI);
    parseur->VireCategorie(v2,l2,OUI);
    l1->RetrouveTraits();
    l2->RetrouveTraits();

    chemin=OUI;
    if (deterministe!=NON)
        fin=OUI;

#endif
}
//-----------------------------------------------------------------------------
void SousArbre::Marquage(GlobalParseur* parseur,
                         char& chemin,
                         Noeud* n,
                         char profondeur,
                         Piles& stacks,
                         Fonction* fonc,
                         VECTA<VECTA<Noeud*>*>* variables,
                         VECTA<int>* pileArg) {

    SousArbre* ar=this;
    SousArbre* ars;
    char solution;
    Noeud* premierN=n;
    Noeud* dernierN=n;
    int sp=stacks.dernier();
    int vsp=0;
    int vspLoc=0;
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

    while (ar != NULL && erreur==NON) {

 
        if (n==NULL) {        
            //Si toutes les elements de la regle qui restent a analyser sont optionnels
            //On peut alors considerer que la regle s'est appliquee, sinon c'est evidemment un echec
            ars=ar;
            while (ars!=NULL && ars->saute!=NON)
                ars=ars->suivant;
            if (ars==NULL)
                break;

            stacks.depile(sp);
            NettoyageVariables(variables,pileArg,vsp);
            return;
        }

        nombre_boucle_marquage++;

        dernierN=n;        
        erreur=OUI;

        if (ar->X==NULL)
            erreur=NON;
        else {
            if (parseur->CompareNoeudSousArbre(ar,n)) {

                //clef contient le codage binaire de toutes les 
                //categories situees sous un meme noeud.
                //cette valeur est systematiquement stockee sur le noeud
                //pere aussi bien pour l'arbre syntaxique
                //Noeud que pour le sous-arbre... En une seule operation, 
                //on peut savoir si les categories situees
                //sous le noeud N contiennent l'ensemble des categories
                //non optionnelles de la regle que l'on applique...

                if (ar->fils != NULL) {
                    if (n->fils != NULL && (n->fils)->fils!=NULL)
                        if (((ar->clef & n->clef) == ar->clef) && 
                            PreTestCategorie(parseur,ar->fils,n->fils,calcul)==OUI) {
                            solution=NON;
                            sp=stacks.dernier();
                            if (variables!=NULL)
                                vspLoc=pileArg->dernier;
                            
                            (ar->fils)->Marquage(parseur,solution,n->fils,
                                                 profondeur+1,stacks,fonc,variables,pileArg);

                            if (solution==OUI)
                                erreur=NON;
                            else {
                                stacks.depile(sp);
                                NettoyageVariables(variables,pileArg,vspLoc);
                            }
                        }
                }
                else
                    erreur=NON;
            }
            
            if (erreur==OUI && ar->derivations.dernier==0) {
                stacks.depile(sp);
                NettoyageVariables(variables,pileArg,vsp);
                return;
            }
        }

        char proms=OUI;
        int lsp=stacks.dernier();     
        if (variables!=NULL) {
            vspLoc=pileArg->dernier;
            if (ar->saute==TOUS) {
                if (ar->suivant==NULL)
                    proms=NON;            
                else
                    //On le transforme en ?*
                    ar->saute=OUI;
            }
        }

        int ider=0; 
        char boucle=OUI;
        
        //Dans le cas d'une structure A+, si erreur==OUI, cela signifie que cette boucle n'a aucun interet, puisqu'il y
        //a deja eu echec. Par consequent on n'entre pas...
        if (!ar->derivations.dernier || (ar->saute==NON && ar->plus!=SAUTE && erreur==OUI))
            boucle=NON;

        while (boucle) {
            solution=NON;
            Noeud* nsuivant=n;

            char noeudSuivant=NON;
            if (proms==OUI && (ar->X!=NULL || ar->saute==OUI)) {
                ars=ar->suivant;
                
                if (ar->saute==NON && ar->plus!=SAUTE && ars!=NULL) //dans ce cas, il faut se positionner sur l'element suivant
                    noeudSuivant=OUI;                    
            }
            else {
                ars=ar->derivations[ider];
                if (ar->reboucle==ider)
                    noeudSuivant=OUI;
                ider++;
            }
            
            if (ider==ar->derivations.dernier)
                boucle=NON;
            
            //s'il s'agit d'une boucle avec une erreur detectee
            //on doit relancer l'analyse sur l'element suivant de la regle
            //mais toujours sur le noeud courant
            if (ars==ar && erreur==OUI)
                continue;

            proms=NON;
            solution=NON;

            if (noeudSuivant==OUI) {
                nsuivant=n->frere;
                stacks.ajoute(ar->traits,n);
                if (ar->argument!=-1 && variables!=NULL) {
                    if ( (*variables)[ar->argument]==NULL)
                        variables->affecte(ar->argument,new VECTA<Noeud*>);
                    (*variables)[ar->argument]->ajoute(n);
                    pileArg->ajoute(ar->argument);
                }
            }

            if (nsuivant!=NULL) {
                if (ars==NULL) {
                    if (erreur==NON)
                        solution=OUI;
                }
                else {
                    //On verifie la compatibilite entre la regle le noeud suivant
                    if (PreTestCategorie(parseur,ars,nsuivant,calcul)==OUI) {
                        char leplus=ar->plus;
                        if (ar->plus==OUI)
                            ar->plus=SAUTE;
                        ars->Marquage(parseur,solution,nsuivant,profondeur,stacks,fonc,variables,pileArg);
                        ar->plus=leplus;
                    }
                }
            }
            else //cas particulier, lorsque l'on analyse un ?- dans une regle de reconstruction
                if (ar->saute==TOUS && erreur==NON && variables!=NULL && boucle==NON)
                    solution=OUI;

            //cas particulier
            if (boucle==NON && ars!=NULL && ars->saute==TOUS)
                solution=OUI;

            if (solution==OUI) {
                chemin=OUI; 
                return;
            }
            stacks.depile(lsp);
            NettoyageVariables(variables,pileArg,vspLoc);
        }
        
        if (ar->derivations.dernier!=0 || erreur==OUI) 
            return;
        
        n->marquage=-1;
        if (ar->argument!=-1) {
            n->marquage=ar->argument;
            if (variables!=NULL) {
                if ( (*variables)[ar->argument]==NULL)
                    variables->affecte(ar->argument,new VECTA<Noeud*>);
                (*variables)[ar->argument]->ajoute(n);
                pileArg->ajoute(ar->argument);
            }
        }

        stacks.ajoute(ar->traits,n);
        debut=NON; 
        n=n->frere;
        ar=ar->suivant;
    }

    if (erreur==OUI)
        return;

    if (profondeur==1) {
        //Initialisation dans le cas d'une regle de marquage
        Traits temporaire(parseur);         

        InstancieTraitsNoeud(parseur,stacks.traits,stacks.noeuds);
            
        if (premierN!=NULL) {
            char test=parseur->PTT(&temporaire,premierN,dernierN);
#ifndef XIP_LIGHT
            static VECTA<Noeud*> varias;
            if (test==OUI && fonc!=NULL && fonc->listes!=NULL) {
                //on extrait un jeu de variable
                varias.raz();
                for (int i=0;i<stacks.dernier();i++)
                    if (stacks.noeuds[i]->marquage!=-1)
                        varias.affecte(stacks.noeuds[i]->marquage,stacks.noeuds[i]);
                test=fonc->listes->Test(parseur,varias);
            }
#endif
            
            if (!test) {
                stacks.depile(sp);
                NettoyageVariables(variables,pileArg,vsp);
                return;
            }
        }
        StabiliseTraitsNoeud(stacks.noeuds);
    }
        
    chemin=OUI;
}
//-------------------------------------------------------------------------------------
//Cette regle applique une regle de sequence ainsi qu'un contexte
void SousArbre::AppliqueSequence(GlobalParseur* parseur,
                         char& chemin,
                         LListe& Phrase,
                         int iListe,
                         int increment,
                         int deterministe,
                         char maj,
                         ResultatSequence* r,
                         VECTA<Noeud*>* noeuds_arguments,
                         VECTA<char>* majCategorie,
                         int lePremier) {

    SousArbre* ar=this;
    SousArbre* ars;

    Liste* L;
    char auMoinsUnNoeud=NON;

    VECTA<SousArbre*> pile(0);

    int premierN=iListe;
    int dernierN=iListe;
    char solution;

    if (chemin==-1) {
        chemin=NON;      
        if (r!=NULL)
            r->fin=-1;
    }

    char erreur=NON;
    int bp=pile.dernier;

    while (chemin!=OUI && ar != NULL && erreur==NON) {

        L=Phrase[iListe];

        if (L==NULL) {
            //Si toutes les elements de la regle qui restent a analyser sont optionnels
            //On peut alors considerer que la regle s'est appliquee, sinon c'est evidemment un echec
            ars=ar;
            while (ars!=NULL && ars->saute==OUI)
                ars=ars->suivant;
            if (ars==NULL)
                break;
            DepileAmbig(pile,-1);
            return;
        }
        int sp=pile.dernier;

        nombre_boucle_sequence++;

        erreur=OUI;
        //Un noeud fictif se traduit par ar->X==NULL
        if (ar->X==NULL) 
            erreur=NON;
        else {
            char couverture=NON;
            
            if (parseur->CompareNoeudSousArbre(ar,L,maj,couverture)) {
                erreur=NON;
                Noeud* n=&L->N;
                if (ar->fils != NULL) {
                    if (n->fils != NULL && (n->fils)->fils!=NULL)
                        if (((ar->clef & n->clef) == ar->clef) &&
                            PreTestCategorie(ar->fils,(n->fils)->X) == OUI) {
                            erreur=OUI;
                            solution=NON;
                            int st=pile.dernier; 
                            static Piles locstacks;
                            locstacks.raz();
                            (ar->fils)->Marquage(parseur,solution,n->fils,2,locstacks,NULL);
                            if (solution) {
                                if (noeuds_arguments!=NULL) {
                                    for (int ipile=0;ipile<locstacks.dernier();ipile++)
                                        if (locstacks.noeuds[ipile]->marquage!=-1)
                                            noeuds_arguments->affecte(locstacks.noeuds[ipile]->marquage,locstacks.noeuds[ipile]);
                                }                                        
                                erreur=NON;
                            }
                            else {
                                DepileAmbig(pile,st);                            
                                L->instanciateur=NULL;
                                return;
                            }
                        }
                }
                if (maj==OUI && majCategorie!=NULL && lePremier!=-1) 
                    majCategorie->affecte(iListe-lePremier,couverture);
            }
            
            if (erreur==OUI && ar->derivations.dernier==0) {
                L->instanciateur=NULL;
                DepileAmbig(pile,-1);
                return;
            }    
            
            if (noeuds_arguments!=NULL && ar->argument!=-1)
                noeuds_arguments->affecte(ar->argument,&L->N);

            //les ar de ce type permettent la desamb totale d'un noeud dans la liste des mots
            if (ar->desamb==OUI && erreur==NON) {
                ar->numliste.ajoute(iListe);
                pile.ajoute(ar);
            }

            auMoinsUnNoeud=OUI;
        }


        //Remarque, dans le cas d'un element optionnel (A* ou (A) ), le systeme
        //integre automatiquement l'element suivant (ar->suivant) dans la regle en premiere position
        //Si l'analyse est longue, cet element suivant devra etre le dernier a etre
        //analysee
        char proms=OUI;
        int ider=0; 
        char boucle=OUI;
        sp=pile.dernier;

        //Dans le cas d'une structure A+, si erreur==OUI, cela signifie que cette boucle n'a aucun interet, puisqu'il y
        //a deja eu echec. Par consequent on n'entre pas...
        if (!ar->derivations.dernier || (ar->saute==NON && ar->plus!=SAUTE && erreur==OUI))
            boucle=NON;
        

        while (boucle) {
            solution=NON;
            int isuivant=iListe;
            
            char noeudSuivant=NON;
            
            //ar->reboucle contient l'index de l'element qui reboucle au sein de la derivation.
            //Cela correspond a une structure AR* ou AR+... Lorsque l'on tombe sur un element
            //qui reboucle, il faut que le systeme puisse se reappliquer sur le noeud suivant
            
            //Si l'on est dans un cadre d'analyse au plus court: deterministe==court
            //Alors, s'il s'agit du debut de la boucle, on saute l'application de cet
            //element de regle. Il faut bien sur que cet element puisse s'appliquer et porte
            //une vraie categorie. De plus, il faut que cet element soit optionnel (saute==OUI)

            // Les cas sont les suivants:
            // (AR), AR* --> saute==OUI et AR->X==NULL
            // AR+ --> saute==NON et AR->X != NULL
            //Attention cas particulier: ARS;ARS;ARS --> AR->X==NULL et AR->saute==NON, on ne peut sauter cet element
            if ((ar->X!=NULL || ar->saute==OUI) &&
                ((deterministe!=longue && proms==OUI) || 
                 (deterministe==longue && ider==ar->derivations.dernier))) {

                if (deterministe==longue)
                    boucle=NON;
                ars=ar->suivant;
                //dans le cas ou il s'agit d'un element AR+, il faut evidemment passer a l'element suivant
                if (ar->saute==NON && ar->plus!=SAUTE) {
                    //Nous savons que cet element ne peut etre saute, il devient
                    //par consequent le dernier de la liste
                    dernierN=iListe;
                    if (ars!=NULL)
                        isuivant+=increment;
                }
            }
            else {
                //dans le cas ou l'on a deja analyse tous les cas de
                //derivations, on s'arrete
                if (ider==ar->derivations.dernier)
                    break;
                ars=ar->derivations[ider];
                if (ar->reboucle==ider)
                    isuivant+=increment;
                ider++;
            }
            
            if (deterministe!=longue && ider==ar->derivations.dernier)
                boucle=NON;            
            
            //s'il s'agit d'une boucle avec une erreur detectee
            //on doit relancer l'analyse sur l'element suivant de la regle
            //mais toujours sur le noeud courant
            if (ars==ar && erreur==OUI)
                continue;
            
            proms=NON;
            
            if (isuivant>=0 && isuivant<Phrase.dernier) {
                if (ars==NULL) {
                    if (erreur==NON) {
                        solution=NOEUDSAUTE;
                        //Le noeud courant n'est pas pris en compte
                        //puisque la regle s'est arrete sur le dernier noeud precedent
                        if (r!=NULL && r->fin < dernierN)
                            r->fin=dernierN;
                    }
                }
                else {
                    //Dans le cas ou il s'agit d'un ar+, on le transforme en ar*
                    char leplus=ar->plus;
                    if (ar->plus==OUI)
                        ar->plus=SAUTE;
                    //soit on applique l'element suivant
                    if (PreTestNoeudCat(parseur,ars,Phrase[isuivant])==OUI) {
                        ars->AppliqueSequence(parseur,solution,Phrase,isuivant,increment,deterministe,
                                      maj,r,noeuds_arguments,majCategorie,lePremier);
                        //Si nous avons saute la categorie courante dans la recursion
                        if (solution==NOEUDSAUTE) {
                            //Si nous sommes en train de sauter la categorie courante
                            if (ars->X == NULL) {
                                if (r!=NULL)
                                //Nous reculons d'un cran le domaine de selection
                                    r->fin=dernierN;
                            }
                            else
                                //sinon le type de retour de recursion est le normal
                                solution=OUI;
                        }                                
                    }
                    ar->plus=leplus;
                }
            }
            
            if (solution) {
                chemin=solution;
                return;
            }

            //On revient au status precedent
            if (Phrase[isuivant])
                Phrase[isuivant]->instanciateur=NULL;
            //On retire les numeros de mot a desambiguiser rajoute lors de la recursion
            DepileAmbig(pile,sp);
        }

        //Quatre possibilites d'analyse:
        // AR, (AR), AR+, AR*
        // Dans le cas d'un element optionnel, ou d'une boucle
        
        if (ar->derivations.dernier!=0 || erreur==OUI) {
            L->instanciateur=NULL;
            DepileAmbig(pile,-1);
            return;
        }
        
        dernierN=iListe;

        iListe+=increment;
        ar=ar->suivant;
    }

    if (erreur==OUI || dernierN==-1) {
        DepileAmbig(pile,-1);
        return;
    }

    chemin=OUI;
    if (r!=NULL && auMoinsUnNoeud==OUI && r->fin<dernierN)
        r->fin=dernierN;
}

//-------------------------------------------------------------------------------------

void SousArbre::Applique(GlobalParseur* parseur,
                         char& chemin,
                         VECTA<ListeVoc*>& Phrase,
                         int iListe,
                         int increment,
                         VECTA<ListeVoc*>& pile,
                         VECTA<Traits*>& piletraits,
                         int& coupe,
                         char stabilise) {

    SousArbre* ar=this;
    char solution;

    if (chemin==-1) 
        chemin=NON;
       
    char erreur=NON;
    int sp=pile.dernier;

    while (chemin!=OUI && ar != NULL && erreur==NON) {

        ListeVoc* L=Phrase[iListe];

        if (L==NULL) {
            Depilage(pile,sp);
            Depilage(piletraits,sp);
            return;
        }

        erreur=OUI;

        if (ar->X==NULL)
            erreur=NON;
        else
            if (parseur->CompareEntree(L,ar->X,ar->traits,ar->selectiontous,ar->selection))
                erreur=NON;
            else 
                if (ar->derivations.dernier==0) {
                    Depilage(pile,sp);
                    Depilage(piletraits,sp);
                    return;
                }
        
        if (ar->argument!=-1)
            coupe=iListe;

        int lsp=pile.dernier;
        char proms=OUI;
        int ider=0; 
        char boucle=OUI;

        //Dans le cas d'une structure A+, si erreur==OUI, cela signifie que cette boucle n'a aucun interet, puisqu'il y
        //a deja eu echec. Par consequent on n'entre pas...
        if (!ar->derivations.dernier || (ar->saute==NON && ar->plus!=SAUTE && erreur==OUI))
            boucle=NON;

        while (boucle) {
            solution=NON;
            int isuivant=iListe;
            
            SousArbre* ars;
            char noeudSuivant=NON;

            if (proms==OUI && (ar->X!=NULL || ar->saute==OUI)) {
                ars=ar->suivant;
                //si nous sommes dans un cas de Cat+ (ar->saute==NON), alors on ne passe au noeud suivant que
                //si il y a quelque chose a tester avant... Sinon le test a deja ete fait par ailleurs
                //au-dessus entre le noeud courant et ar...
                if (ar->saute==NON && ar->plus!=SAUTE && ars!=NULL) 
                    noeudSuivant=OUI;
            }
            else {
                ars=ar->derivations[ider];
                if (ar->reboucle==ider)
                    noeudSuivant=OUI;                
                ider++;
            }

            if (ider==ar->derivations.dernier)
                boucle=NON;
            
            //s'il s'agit d'une boucle avec une erreur detectee
            //on doit relancer l'analyse sur l'element suivant de la regle
            //mais toujours sur le noeud courant
            if (ars==ar && erreur==OUI)
                continue;

            proms=NON;
            if (noeudSuivant==OUI) {
                pile.ajoute(L);
                piletraits.ajoute(ar->traits);
                isuivant+=increment;  
                //si nous sommes au-dela des bornes, on place -1 dans isuivant comme indication
                if (isuivant<0 || isuivant>= Phrase.dernier)
                    isuivant=-1;
            }
            
            if (isuivant!=-1) {
                if (ars==NULL) {
                    if (erreur==NON)
                        solution=OUI;
                }
                else {
                    char leplus=ar->plus;
                    if (ar->plus==OUI)
                        ar->plus=SAUTE;
                    ars->Applique(parseur,solution,Phrase,isuivant,increment,pile,piletraits,coupe,stabilise);
                    ar->plus=leplus;
                }
            }
            
            if (solution==OUI) {
                chemin=OUI;
                return;
            }
            
            Depilage(pile,lsp);
            Depilage(piletraits,lsp);
        }
        
        if (ar->derivations.dernier!=0 || erreur==OUI)
            return;

        pile.ajoute(L);
        piletraits.ajoute(ar->traits);

        iListe+=increment;
        ar=ar->suivant;
    }
    
    if (erreur==OUI) {
        Depilage(pile,sp);
        Depilage(piletraits,sp);
        return;
    }

    if (stabilise==OUI)
        StabiliseTraitsVocabulaire(pile,piletraits);
    chemin=OUI;
}

//-------------------------------------------------------------------------------
char GlobalParseur::CompareNoeudSousArbre(SousArbre* opt,Noeud* n) {
    char couv;
    Traits* trtest=opt->traits;
    
    if (opt==NULL || n==NULL || n->source==NULL)
        return opt->negation;
    
    if (!CompareCategories(n->source,opt->X,opt->selectiontous,opt->selection,NON,couv))
        return opt->negation;
    
    char leOUI=1-opt->negation;
    
    if (trtest==NULL)
        return leOUI;
    
    if (n->lesTraits()==NULL)
        return opt->negation;
    
    char* mot;
    if (n->fils!=NULL)
        mot=(n->fils)->surface;
    else
        mot=n->surface;
    
    n->source->instanciateur=NULL;
    
    if (ITR(this,trtest,(n->source)->traitsbase,NONECRIT,n) == opt->negation)
        return NON;
    else
        return OUI;
    
    return leOUI;
}
//-------------------------------------------------------------------------------
char GlobalParseur::CompareNoeudSousArbre(SousArbre* boucle,Liste* L, char maj,char& couverture) {
    
    char* mot;
    Traits* trtest=boucle->traits;
    
    if (!CompareCategories(L,boucle->X,boucle->selectiontous,boucle->selection,maj,couverture))
        return boucle->negation;
    
    //Si le test reussit, alors on renvoie la difference avec la negation
    //exemple: si le test reussit et la negation vaut OUI, le retour sera 1-OUI=NON
    // sinon si le test echoue le retour sera la negation elle-meme
    char leOUI= 1-boucle->negation;
    
    if (L->N.fils!=NULL)
        mot=(L->N.fils)->surface;
    else
        mot=L->N.surface;
    
    if (trtest==NULL)
        return leOUI;
    
    L->instanciateur=NULL; 
    
    if (ITR(this,trtest,L->traitsbase,NONECRIT,&L->N) != boucle->negation) {
        if (boucle->negation==NON)
            L->instanciateur=boucle->traits;
        return OUI;
    }        
    else
        return NON;
    
    return boucle->negation;
}

/*-------------------------------------------------------------------------*/
SousArbre* GlobalParseur::ConstruitNoeud(ifstream& fichier,
                                         VECTA<SousArbre*>& nettoyage,
                                         char* tampon,
                                         char& k,
                                         typeRegle regle,
                                         char negation,
                                         char& optionnel,
                                         char& modification) {
    
    char neg=negation;    
    char desamb=NON;
    int iter=0;
    int num_arg=-1;
    Traits* trcourant=NULL;
    SousArbre* element=NULL;
    char enProfondeur=NON;
    Categorie* c;
    char suite[1000];
    Traits* selection=NULL;
    Traits* selectiontous=NULL;
    
    optionnel=NON;
    char relire=NON;
    
    
    if (k==NEGATION) {
        if (tampon[0]==0) {
            //Negation
            neg=1-negation;
            k=lit(fichier,tampon);
            modification=OUI;
        }
        else
            relire=OUI;
    }
    
    //Il s'agit d'un caractere de controle
    //on repart
    if (tampon[0]==0 && k!='#')
        return NULL;
    
    if (k=='^') {
        enProfondeur=OUI;
        k=lit(fichier,suite);
        modification=OUI;
    }
    
    int l=strlen(tampon);
    desamb=NON;
    int pos=-1;
    for (int i=0;i<l;i++) {
        if (tampon[i]=='*' || tampon[i]=='+' || tampon[i]=='-') {
            if (pos==-1)
                pos=i;
            optionnel=tampon[i];
        }
        else
            if (tampon[i]=='%') {
                if (pos==-1)
                    pos=i;
                desamb=OUI;
            }
    }    
    
    //     //Lecture du nombre maximal d'iteration d'une structure * ou +
    //     if (k==34 && optionnel!=NON) { //"12"
    //         k=lit(fichier,suite);
    //         modification=OUI;
    //         //Normalement, c'est un chiffre
    //         iter=atoi(suite);
    //         if (k!=34) 
    //             erreur(CHAINES[166]);
    //         k=lit(fichier,suite);
    //     }
    
    if (pos!=-1)
        tampon[pos]=0;
    
    if (k=='#') {
        //nous lisons le numero de l'argument...
        k=lit(fichier,suite);
        modification=OUI;
        //Une variable
        num_arg=atoi(suite);
    }
    
    if (k=='<') {
        k=litBis(fichier,suite);
        modification=OUI;
        //S'il s'agit d'un selectiontous
        if (k=='*') {
            selectiontous=BatitTraits(fichier,'>');
            k=lit(fichier,suite);
            if (k=='<') {
                selection=BatitTraits(fichier,'>');
                k=lit(fichier,suite);
            }
        }                
        else{
            MetEnAttenteLecture(k,suite);
            selection=BatitTraits(fichier,'>');            
            k=lit(fichier,suite);
        }
    }
    
    
    if (k=='[') {
        modification=OUI;
        trcourant=BatitTraits(fichier);            
        k=lit(fichier,suite);
    }
    
    if (!strchr(",.;|(){}=~",k)) {
        sprintf(suite,"%s %c",CHAINES[36],k);
        erreur(suite);
    }
    
    c=CatVide;
    if (tampon[0])
        c=ChercheCat(tampon);        
    
    if (regle==REGLES_RECONSTRUCTION_MODELE && c!=CatVide) {
        //Il s'agit d'une modification d'un noeud
        //dans le cadre d'une regle de reconstruction
        //pour lui donner une nouvelle categorie
        if (trcourant==NULL)
            trcourant=new Traits(c->traits);
        else
            trcourant->ou(c->traits);
    }
    
    trcourant=FactoriseTraits(trcourant);

    //si une queue de regle a deja ete calcule, il n'est pas besoin d'en
    //generer une nouvelle version
    element=new SousArbre(&c->X,trcourant,num_arg);
    nettoyage.ajoute(element);
        
    element->selection=FactoriseTraits(selection);
    element->selectiontous=FactoriseTraits(selectiontous);
    element->negation=neg;
    
    //dans le cas ou il s'agit d'une exploration en profondeur non bornee
    //on conserve le pere du sous-arbre dans le champ suivantbis...
    //if (sous!=NULL && sous->profondeurNonBornee==OUI)
    //  sous->suivantbis=element;
    
    element->profondeurNonBornee=enProfondeur;
    
    element->desamb=desamb;
    //Borne bloque le nombre d'iteration sur un objet de type: Cat* ou Cat+
    element->borne=iter;
    //On reinitialise le tampon, pour d'autres lectures
    tampon[0]=0;
    if (relire==OUI) {
        k=lit(fichier,tampon);
        modification=OUI;
    }
    
    return element;
}


void TraitementArgument(GlobalParseur* parseur,SousArbre* element,typeRegle regle,SeqFonc* fonc) {
    int num_arg=element->argument;
    int arg; 
    Fonction* fFonc=NULL;
    
    if (num_arg==-1)
        return;
    
    
    if (fonc->type()!=REGLES_SEQUENCE)
        fFonc=(Fonction*)fonc;
    
    //Nous avons un argument lie avec une variable de type #num.
    
    arg=-1;
    
    //Nous recherchons la position de cet argument dans la
    //liste deja construite
    //de la fonction syntaxique. 
    //si l'argument est absent
    for (int i=0;i<fonc->nb_args;i++)
        if (fonc->arg[i]==(size_t)num_arg) {
            arg=i;
            if (fFonc!=NULL)                            
                fFonc->marque.affecte(i,1);
            break;
        }
        
        if (arg==-1) {
            //nous n'avons pas trouve cet argument, dans ce cas,
            //il ne s'agit pas d'une erreur
            //mais simplement d'un argument supplementaire que l'on 
            //enregistre a la fin de la liste
            //des arguments de la fonction... Une marque speciale est 
            //enregistree
            //pour s'en souvenir
            //les valeurs sont pour marque: 
            // 1 argument normal
            // 2 argument supplementaire a ne pas afficher...
            if (regle==VIDE || regle==REGLES_DEDUCTION || regle==REGLES_EXTRACTION) {
                if (regle!=REGLES_DEDUCTION)
                    arg=fonc->nb_args;
                else
                    arg=num_arg;
                if (fFonc!=NULL)
                    fFonc->marque.affecte(arg,2);
                fonc->arg.affecte(arg,num_arg); 
                //on initialise la valeur avec num_arg...
                ++fonc->nb_args; //on incremente le nombre d'argument...
            }
            //dans le cas des regles de simplification, on rajoute quelques contraintes:
            //a) la valeur #1 est par defaut la valeur du noeud que l'on simplifie, elle ne
            //peut donc etre utilise pour marquer un element du contexte.
            //b) la variable associee n'a aucun lien avec des arguments inexistants d'une fonction
            //il s'agit forcement d'un contexte dans le cas d'une regle de simplification
            else {
                arg=num_arg;
                if (regle==REGLES_RECONSTRUCTION || 
                    regle==REGLES_RECONSTRUCTION_MODELE &&
                    fFonc!=NULL)
                    fFonc->marque.affecte(arg,OUI);
                if (regle==REGLES_SIMPLIFICATION && arg==0)
                    parseur->erreur(parseur->CHAINES[104]);
            }                        
        }
        
        element->argument=arg;
}


void SousArbre::ConstruitClef(GlobalParseur* pars,typeRegle regle) {
    SousArbre* f=fils;
    clef=0;    
    
    //Simplification
    while (f!=NULL) {
        if (f->profondeurNonBornee==NON && f->negation==NON && f->X!=NULL && f->X->val!=CATFUSION)
            clef|=(f->X)->Val;
        f=f->suivant;
    }
    f=this;
    int nb=0;
    while (f!=NULL) {
        if (f->saute==NON)
            nombre++;
        f=f->suivant;
    }
    
    f=this;
    while (f!=NULL) {
        f->nombre=nombre;
        if (f->saute==NON)
            nombre--;
        f=f->suivant;
    }
}

void ConstruitExtremite(SousArbre* element,VECTA<SousArbre*>& pile,char premier) {
    SousArbre* courant=element;
    int i=0;
    
    if (premier==OUI || element->freres.dernier==0) {
        if (element->suivant==NULL)
            pile.ajoute(element);
    }
    else
        pile.ajoute(element->freres.fin());
    
    element->marquage=OUI;
    courant=element;
    
    if (element->reboucle>=0) //dans ce cas, le premier element est celui de la boucle, on le saute
        i=1;
    
    for (;i<courant->derivations.dernier;i++) {
        if (courant->derivations[i]->marquage==NON) {
            courant->derivations[i]->marquage=OUI;
            ConstruitExtremite(courant->derivations[i],pile,NON);
            courant->derivations[i]->marquage=NON;
        }
    }
    
    element->marquage=NON;
}

char EgauxSousArbre(GlobalParseur* pars,SousArbre* ar1, SousArbre* ar2) {
    
    if (ar1->derivations.dernier || ar2->derivations.dernier)
        return NON;
    
    if (ar1->argument!=ar2->argument)
        return NON;
    
    if (ar1->X!=ar2->X)
        return NON;
    
    if (ar1->traits!=ar2->traits)
        return NON;
    
    if (ar1->fils==NULL || ar2->fils==NULL) {
        if (ar1->fils!=ar2->fils)
            return NON;
    }
    else
        if (!EgauxSousArbre(pars,ar1->fils,ar2->fils))
            return NON;    
        return OUI;
}


void NettoieNettoyage(VECTA<SousArbre*>& nettoyage, VECTA<SousArbre*>& vn) {
    
    for (int net=0;net<nettoyage.dernier;net++) {
        for (int ivn=0;ivn<vn.dernier;ivn++) {
            if (nettoyage[net]==vn[ivn]) {
                delete nettoyage.retireElement(net);
                vn.affecte(ivn,NULL);
                net--;
                break;
            }
        }
    }    
}

char FusionneNoeud(GlobalParseur* parseur,SousArbre* cle,VECTA<SousArbre*>& vn) {
    if (cle->derivations.dernier<=1 || cle->saute==OUI)
        return NON;
    
    char detectee=OUI;
    Traits* tr=cle->derivations[0]->traits;
    TVecteur v=0;
    SousArbre* ar;
    SousArbre* lefils=NULL;
    int num_arg=cle->derivations[0]->argument;
    
    for (int idv=0;idv<cle->derivations.dernier;idv++) {
        ar=cle->derivations[idv];
        
        if (ar->X==parseur->XCatVide ||
            ar->X==NULL ||
            ar->fils!=NULL ||
            ar->argument!=num_arg ||
            ar->selectiontous!=NULL ||
            ar->selection!=NULL ||
            ar->derivations.dernier)
            continue;
        
        if (tr!=ar->traits)
            continue;
        
        vn.ajoute(ar);
        v|=ar->X->Val;
    }
    
    if (vn.dernier<=1)
        return NON;
    
    //On fusionne toutes les categories en un seul noeud
    //On cree un noeud X particulier
    Cat* X= new Cat;
    //Indication d'un noeud fusion
    X->val=CATFUSION;
    X->index=CATFUSION;
    X->cacher=NON;
    X->Val=v;
    X->nom=DupliqueChaine((char*)"%");
    
    if (cle->derivations.dernier==vn.dernier) {
        cle->derivations.raz();
        ar=cle;
    }
    else { //sinon on nettoie (a l'exception du premier element qui servira de centre de fusion)
        //la liste de derivations de tous ceux que l'on va fusionner
        ar=vn.retireElement(0);
        for (int i=0;i<vn.dernier;i++) {
            for (int j=0;j<cle->derivations.dernier;j++) {
                if (cle->derivations[j]==vn[i]) {
                    cle->derivations.retireElement(j);
                    break;
                }
            }
        }        
    }
    
    ar->X=X;
    ar->argument=num_arg;
    ar->traits=tr;
    
    return OUI;
}



void FusionneFictif(SousArbre* arbre,char deterministe,VECTA<SousArbre*>& nettoyage) {
    VECTA<SousArbre*> pile;
    
    //On cree alors un element fictif qui chapeaute l'element non bornee
    //Traitement du premier element    
    ConstruitExtremite(arbre,pile,OUI);
    for (int ip=0;ip<pile.dernier;ip++)
        pile[ip]->suivant=arbre->freres[0];
    
    for (int i=0;i<arbre->freres.dernier;i++) {
        pile.raz();
        ConstruitExtremite(arbre->freres[i],pile,OUI);
        for (int ip=0;ip<pile.dernier;ip++) {
            pile[ip]->suivant=arbre->freres[i+1];
        }
    }
}

SousArbre* GlobalParseur::ConstruitArbre(ifstream& fichier,
                                         SeqFonc* fonc,
                                         VECTA<SousArbre*>& nettoyage,
                                         char negation,
                                         char fin,
                                         char optio,
                                         int increment,                                         
                                         typeRegle regle) {
    
    char k, tampon[1000];
    char suite[100];
    SousArbre* cle=NULL;
    SousArbre* element=NULL;
    SousArbre* fictif=NULL;
    
    char debut;
    char disjonction=NON;
    char neg=NON;
    
    Sequence* seq=NULL;
    Fonction* fFonc=NULL;
    
    if (fonc->type()==REGLES_SEQUENCE)
        seq=(Sequence*)fonc;
    else
        fFonc=(Fonction*)fonc;
    
    k=0;
    debut=OUI;
    
    k=lit(fichier,tampon);    
    
    
    if (fin=='|' && k=='@') {
        fonc->deterministe=longue;
        k=lit(fichier,tampon);
    }
    
    if (fin=='|' && k=='!') {
        fonc->deterministe=courte;
        k=lit(fichier,tampon);
    }
    
    while (!fichier.eof()) {
        
        int num_arg=-1;
        char optionnel=NON;
        char modification=NON;
        
#ifndef XIP_LIGHT
        if (regle==REGLES_SEQUENCE || regle==REGLES_EXTRACTION || regle==REGLES_RECONSTRUCTION_MODELE) {
            if (!strcmp(tampon,"where") && k=='(') {
                TestArbre* tarbre=NULL;
                static VECTA<char> vari(5);
                //lecture du test... Il s'agit d'un test en debut de definition            
                tarbre=new TestArbre;
                k=' ';
                LectureTest(fichier,k,tampon,NON,vari,&tarbre);
                TVecteur clefArbre;
                if (fonc->listes!=NULL)
                    erreur(CHAINES[10]);
                fonc->listes=new ListeTestArbre;
                tarbre->direct=tarbre->LineariseDirect(this,clefArbre,*fonc->listes);
                delete tarbre;
                modification=OUI;
                if (regle==REGLES_RECONSTRUCTION_MODELE)
                    break;
            }
        }
#endif
        neg=negation;
        if (k=='~')
            neg=1-negation;
        int ifr;
        element=ConstruitNoeud(fichier,nettoyage,tampon,k,regle,negation,optionnel,modification);
        if (element!=NULL) {
            debut=NON;
            TraitementArgument(this,element,regle,fonc);
            if (k=='{') {
                element->fils=ConstruitArbre(fichier,fonc,nettoyage,negation,'}',optionnel,increment,regle);
                k=lit(fichier,tampon);
            }
        }
        else
            if (k=='(') {
                debut=NON;
                element=ConstruitArbre(fichier,fonc,nettoyage,neg,')',NON,increment,regle);
                neg=negation;
                modification=OUI;
                k=lit(fichier,tampon);
                optionnel='(';
                if (tampon[0]=='*' || tampon[0]=='+')
                    optionnel=tampon[0];
                else
                    //si l'element est seul
                    if (element->freres.dernier==0 && element->derivations.dernier==0)
                        optionnel='(';
                    
            }
            
            char nepasrelire=NON;
            
            debut=NON;
            char disjonctionPrecedente=disjonction;
            
            if (k==',' || k==';') {                
                if (k==';')
                    disjonction=OUI;
                else
                    disjonction=NON;
                if (nepasrelire==NON)
                    k=lit(fichier,tampon);
                modification=OUI;
            }
            else
                if (k==fin) {
                    modification=OUI;
                    fin=OUI;
                }
                
                int rebouclage=NON;
                if (element!=NULL) {
                    if (optionnel=='*' || optionnel=='+' || optionnel=='-') {
                        //On boucle sur soi-meme...
                        if (optionnel=='+')
                            element->plus=OUI;
                        
                        rebouclage=OUI;
                        if (element->freres.dernier==0) {
                            if (element->X!=NULL) {    
                                element->reboucle=element->derivations.dernier;
                                element->derivations.ajoute(element);
                            }
                            else {
                                VECTA<SousArbre*> pile;
                                ConstruitExtremite(element,pile,OUI);
                                //On reboucle donc au sein de la derivation
                                for (int ip=1;ip<pile.dernier;ip++) {
                                    pile[ip]->reboucle=pile[ip]->derivations.dernier;
                                    pile[ip]->derivations.ajoute(element);
                                }
                            }
                        }
                        else {
                            element->freres.fin()->reboucle=element->freres.fin()->derivations.dernier;
                            element->freres.fin()->derivations.ajoute(element);
                        }                
                    }
                    
                    
                    //Les cas de creations d'un element fictif sont:
                    //a) un element optionnel
                    //b) une disjonction nouvelle qui se prepare
                    if ( (disjonction==OUI && disjonctionPrecedente==NON) ||
                        ((optionnel=='(' || optionnel=='*' || optionnel=='-') && 
                        (cle==NULL || cle->X!=NULL || disjonctionPrecedente==NON))) {

                        if (optionnel=='-')
                            element->saute=TOUS;
                        
                        //Cas particulier, l'element sous * est deja un element complexe
                        //on ne cree pas alors un niveau supplementaire
                        if (element->X!=NULL) {
                            //on cree alors un noeud fictif dans lequel on place notre element
                            fictif = new SousArbre(NULL,NULL,-1);
                            fictif->derivations.ajoute(element);
                            nettoyage.ajoute(fictif);

                            //on conserve au plus au niveau l'argument de l'element bouclant
                            fictif->argument=element->argument;
                            element=fictif;
                        }
                        
                        //Dans le cas d'un rebouclage interne au sein de la derivation
                        //on le retient au sein de cet element fictif
                        if (rebouclage==OUI)
                            element->reboucle=-2;                    
                        
                        if (optionnel=='*' || optionnel=='(')
                            element->saute=OUI;
                        else
                            if (optionnel=='-')
                                element->saute=TOUS;
                    }
                    
                    if (disjonctionPrecedente==OUI) {
                        if (optionnel=='*' || optionnel=='(' || optionnel=='-')
                            cle->saute=OUI;
                        if (increment==1) {
                            if (cle==NULL)
                                cle=element;
                            else
                                if (cle->freres.dernier==0)
                                    cle->derivations.ajoute(element);
                                else
                                    cle->freres.fin()->derivations.ajoute(element);
                        }
                        else
                            cle->derivations.ajoute(element);
                    }
                    else {
                        if (cle==NULL)
                            cle=element;
                        else {
                            if (increment==1)
                                cle->freres.ajoute(element);
                            else {
                                //On ajoute les elements a l'envers
                                element->freres.ajoute(cle);
                                for (ifr=0;ifr<cle->freres.dernier;ifr++)
                                    element->freres.ajoute(cle->freres[ifr]);
                                cle->freres.raz();
                                cle=element;
                            }
                        }
                    }
                }
                
                if (modification==NON) {
                    strcpy(suite,CHAINES[69]);
                    strcat(suite,":");
                    strcat(suite,tampon);
                    tampon[0]=k;
                    tampon[1]=0;
                    strcat(suite,tampon);
                    erreur(suite);
                }
                
                if (fin==OUI)
                    break;
    }
    
    if (cle != NULL) {
        FusionneFictif(cle,fonc->deterministe,nettoyage);
        element=cle;
        static VECTA<SousArbre*> vn(20);
        while (element != NULL) {
            if (FusionneNoeud(this,element,vn))
                NettoieNettoyage(nettoyage,vn);            
            vn.raz();
            element=element->suivant;
        }                
        cle->ConstruitClef(this,regle);
    }
    
    return cle;
}



Noeud* SousArbre::RecreeArbre(GlobalParseur* parseur,
                              LListe& Phrase,
                              LListe& lesAjouts,
                              VECTA<int>& phrs,
                              VECTA<int>& ajouts,
                              Noeud* racine,
                              VECTA<VECTA<Noeud*>*>& variables,
                              VECTA<Noeud*>& noeudsfreres,
                              char top) {
    
    SousArbre* ar=this;
    
    int premier=-1;
    Liste* L;
    Noeud* premierNoeud=NULL;
    Noeud* n;
    int pos;
    

    //On enregistre la position du premier noeud qui nous interesse
    if (top==OUI)
        premier=(racine->source)->position;
    
    while (ar!=NULL) {        
        int finboucle=0;
        
        if (variables[ar->argument]!=NULL)
            finboucle=variables[ar->argument]->dernier;
        
        //On cree un nouveau sommet
        if (ar->argument==-1 && ar->fils!=NULL) {
            L=(parseur->TasDeListes)->fournir(parseur,ar->X,ar->traits);            
            Noeud* nouvN=&L->N;
            nouvN->fils= (ar->fils)->RecreeArbre(parseur,Phrase,lesAjouts,phrs,ajouts,NULL,variables,noeudsfreres,NON);
            if (nouvN->fils!=NULL) {
                nouvN->gauche=(nouvN->fils)->gauche;
                nouvN->droite= (nouvN->fils->dernier)->droite;
            }
            
            if (premierNoeud==NULL)
                premierNoeud=nouvN;
            else
                premierNoeud->insere(nouvN);
            
            //s'il s'agit de la creation d'un nouveau sommet au top
            if (top==OUI) {
                //rien n'a ete encore ajoute
                if (phrs.dernier==0)
                    //la position est par rapport au premier noeud repere dans l'analyse
                    pos=racine->source->position;
                else
                    //ou par rapport au dernier noeud conserve
                    pos=phrs.fin();
                //On rajoute ce nouveau sommet
                ajouts.ajoute(pos);
                lesAjouts.ajoute(L);
            }
            //On transfert alors les traits libres
            parseur->PTT(L->traitsbase,nouvN->fils,nouvN->fils->dernier);
            L->SauveTraits();
        }
        
        
        for (int v=0;v<finboucle;v++) {
            n=(*variables[ar->argument])[v];
            
            if (premierNoeud==NULL)
                premierNoeud=n;
            
            if (n==NULL)
                return NULL;
            
            char ajouter=NON;
            //Dans le cas d'un noeud transmis vers le sommet
            if (top==OUI) {
                //On desactive le noeud par rapport a ses ancetres et autres positions
                if (n->frere!=NULL)
                    noeudsfreres.ajoute(n->frere);                
                n->annule();
                //S'il s'agit d'un noeud deja au sommet, sa position est placee dans phrs
                if (Phrase[n->source->position] == n->source)
                    phrs.ajoute(n->source->position);
                else {
                    //sinon sa position est dans ajouts
                    //rien n'a ete encore ajoute
                    if (phrs.dernier==0)
                        //la position est par rapport au premier noeud repere dans l'analyse
                        pos=racine->source->position;
                    else
                        //ou par rapport au dernier noeud conserve
                        pos=phrs.fin();
                    //On rajoute ce nouveau sommet
                    ajouts.ajoute(pos);
                    lesAjouts.ajoute(n->source);
                    ajouter=OUI;
                }
            }
            else {
                if (n->frere!=NULL) 
                    noeudsfreres.ajoute(n->frere);                
                n->annule();
            }
            
            //Quelques modifications sur le noeud en fonction de ar
            if ( (ar->X)->val!=0 && ar->X != n->X) {
                parseur->IndexParCat[(n->X)->val].affecte((n->source)->indexCat,NULL);
                
                //Categories differentes
                //On retire les traits specifiques de l'ancienne categorie
                Categorie* cat= parseur->Categories[(n->X)->index];
                ((n->source)->traitsbase)->etnon(cat->traits);
                ((n->source)->traitsactif)->etnon(cat->traits);
                
                
                if (n->surface==(n->X)->nom)
                    n->surface=(ar->X)->nom;
                n->X=ar->X;
                (n->source)->X=ar->X;
                (n->source)->indexCat=parseur->IndexParCat[(n->X)->val].dernier;
                parseur->IndexParCat[(n->X)->val].ajoute(n->source);
                //Dans le cas d'une feuille, il faut de plus verifier si cette nouvelle categorie
                //correspond a une ancienne categorie
                if ((n->source)->voc!=NULL) {
                    ListeVoc* voc= (n->source)->voc;
                    char trouve=NON;
                    for (int icat=0;icat<voc->nbX;icat++) {
                        if (voc->X[icat]==ar->X) {
                            n->source->choixCategorie(icat);
                            trouve=OUI;
                            break;
                        }
                    }
                    //sinon, il s'agit d'un remplacement definitif
                    if (trouve==NON)
                        parseur->EcrasementCategorie(ar->X,NULL,voc,n->source);
                }
            }
            
            //des traits differents, il s'ajoute aux traits courants...
            if (ar->traits!=NULL) {
                parseur->InstancieTraits( (n->source)->traitsactif,ar->traits);
                ((n->source)->traitsbase)->ou((n->source)->traitsactif);
            }
            
            char insertion=OUI;
            //On cree les fils
            if (ar->fils!=NULL) {
                n->fils=NULL;                
                n->dernier=n;
                n->fils=(ar->fils)->RecreeArbre(parseur,Phrase,lesAjouts,phrs,ajouts,n,variables,noeudsfreres,NON);

                if (n->fils!=NULL) {
                    n->gauche=(n->fils)->gauche;
                    n->droite= ((n->fils)->dernier)->droite;
                }
                else {
                    insertion=NON; //ses fils sont absents, ce noeud doit disparaitre...
                    //dans le cas d'une structure en haut
                    if (top==OUI)
                        //on le retire des differentes listes
                        if (ajouter==OUI) {
                            lesAjouts.retire();
                            ajouts.retire();
                        }
                        else
                            phrs.retire();                    
                }
            }
            //Il devient son frere
            if (premierNoeud!=n && insertion==OUI)
                premierNoeud->insere(n);            
        }
        ar=ar->suivant;                    
    }
    
    return premierNoeud;
}

void SousArbre::RechercheDesamb(VECTA<SousArbre*>& vectarbre) {
    
    if (X!=NULL)
        if (desamb==OUI)
            vectarbre.ajoute(this);
        
        if (liens!=NULL)
            liens->RechercheDesamb(vectarbre);
        
}

char TestPatronReconstruction(SousArbre* cle,SousArbre* pere,char* mes,char* message) {

    while (cle!=NULL) {
        if (pere!=NULL) {
            char nom[100];
            if (pere->X!=NULL)
                strcpy(nom,pere->X->nom);
            else
                strcpy(nom,"?");
            //Uniquement si c'est le dernier element
            if (cle->suivant==NULL && cle->argument!=-1) {
                if (cle->X==NULL || cle->derivations.dernier!=0) {
                    if (cle->traits!=NULL) {
                        sprintf(message,"%s: %s{..?#%d}",mes,nom,cle->argument);
                        return NON;
                    }
                }
                else 
                    if (cle->traits== NULL || !TestLast(cle->traits)) { 
                        sprintf(message,"%s: %s{..%s#%d}",mes,nom,cle->X->nom,cle->argument);
                        return NON;
                    }
            }
        }
        if (cle->fils!=NULL)
            if (!TestPatronReconstruction(cle->fils,cle,mes,message))
                return NON;
        cle=cle->suivant;
    }
    return OUI;
}
