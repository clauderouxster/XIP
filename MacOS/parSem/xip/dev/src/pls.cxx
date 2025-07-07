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
   filename   : pls.cxx
   Date       : 10/01/1999
   Purpose    : LP rule compiler
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"
#include "pls.h"

/*-----------------Remise-a-zero des contextes de compilations----------------------*/

void GlobalParseur::RAZContextePL() {
    for (int i=0;i< NbCats;i++) {
        TabDroite[i]=0;
        TabGauche[i]=0;
        TabMembre[i]=0;
        TabDerniers[i]=0;
        TabLexique[i]=NON;
        TabContexte[i]=0;
    }
}


/*-------------------------Existe-t-il une Pl-règle entre tg et td?-----------------------*/
char GlobalParseur::ExistePL(int laCouche,Traits* vg,Traits* vd,Cat* cg,Cat* cd) {
    VECTA<RegleTraits*>& pl=PListes[laCouche];
    int i;
 
    for (i=0;i<pl.dernier;i++) {
        if (pl[i]->Test(this,vg,cg,GAUCHE) && pl[i]->Test(this,vd,cd,DROIT)) 
            return OUI;        
    }

    //Les pl regles de la couche 0 ont une portee universelle
    //Si l'on a pas trouve de regles dans la couche donnee
    //on verifie avec les pl-regles universelles...
    if (laCouche) {
        VECTA<RegleTraits*>& pl0=PListes[0];
        for (i=0;i<pl0.dernier;i++) {
            if (pl0[i]->Test(this,vg,cg,GAUCHE) && pl0[i]->Test(this,vd,cd,DROIT)) 
                return OUI;        
        }
    }

    return NON;
}


/*-------------------------Existe-t-il une Pl-règle avec valeur à gauche?------------*/
//--->   valeur < pl
RegleTraits* GlobalParseur::TrouvePLDroite(VECTA<RegleTraits*>& PL,int& i,Traits* valeur,Cat* c) {    

    for (i;i<PL.dernier;i++) {
        if (PL[i]->Test(this,valeur,c,GAUCHE)) {
            i++;
            return PL[i-1];
        }        
    }
    
    i=PL.dernier;
    return NULL;
}
//----------------------------------------------------------------------------------
char Ramene(VECTA<Traits*>& listeTraits,Traits* Z, Traits* Tr, Traits* tr) {

    for (int i=0;i<listeTraits.dernier;i++) {
        if (Tr==listeTraits[i] || tr==listeTraits[i])
            continue;

        if (listeTraits[i]->contient(Z))
            return OUI;
    }
    return NON;
}

//---------------------------------------------------------------------------------------------------------
//On teste la transitivite des PL-regles... X < Z et Z!=Y et Z<Y
char GlobalParseur::TestTransitivite(int laCouche,
                                     Traits* X,
                                     Traits* Y,
                                     VECTA<Traits*>& listeTraits,
                                     Traits* tr,
                                     Traits* Tr,
                                     Cat* cx,
                                     Cat* cy) {
    RegleTraits* Z;
    VECTA<RegleTraits*>& PL=PListes[laCouche];
    int i=0;

    for (i=0;i<PL.dernier;i++) {
        Z=TrouvePLDroite(PL,i,X,cx);
        if (Z && !Y->contient(Z->affecte) && 
            Ramene(listeTraits,Z->affecte,Tr,tr) && 
            ExistePL(laCouche,Z->affecte,Y,Z->X,cy)) 
            return NON;    
    }

    if (laCouche) {
        VECTA<RegleTraits*>& PL0=PListes[0];
        
        for (i=0;i<PL0.dernier;i++) {
            Z=TrouvePLDroite(PL0,i,X,cx);
            if (Z && !Y->contient(Z->affecte) && 
                Ramene(listeTraits,Z->affecte,Tr,tr) && 
                ExistePL(laCouche,Z->affecte,Y,Z->X,cy)) 
                return NON;    
        }
    }

    return OUI;
}

/*----------------------------------------------------------------------------------------------*/
void GlobalParseur::ConstruitTeteDroite(int laCouche,
                                        VECTA<Traits*>& listeTraits,
                                        VECTA<Cat*>& listeCat) {
    short x,y;
    Traits* trY;
    Traits* trX;
    Cat* cx;
    Cat* cy;
    char droite; 
    TVecteur verifTab;

    if (listeTraits.dernier<=1)
        return;
    
    if (listeTraits.dernier==2) {
        //Element X
        trX=listeTraits[0];
        x=listeCat[0]->val;
        cx=listeCat[0];

        //Element Y
        trY=listeTraits[1];
        y=listeCat[1]->val;
        cy=listeCat[1];
     
        //On verifie d'abord si y peut etre a droite de x
        //1) est-ce que y a deja ete range parmi les elements a droite de x ?
        verifTab= TabDroiteUn[laCouche][x] & listeCat[1]->Val;

        //S'il n'existe aucune PL regle qui empeche y<x 
        //et que y n'est pas deja a droite de x
        if (!verifTab && !ExistePL(laCouche,trY,trX,cy,cx)) {
            //On range Y dans la table a droite de X
            TabDroiteUn[laCouche][x]|=listeCat[1]->Val;
            TabDroite[x]|=listeCat[1]->Val;
            TabGauche[y]|=listeCat[0]->Val;
        }
     
        //On fait la meme chose pour Y par rapport a X
        //Est-ce que X a deja ete range a droite de Y
        verifTab=TabDroiteUn[laCouche][y] & listeCat[0]->Val;
        if (!verifTab && !ExistePL(laCouche,trX,trY,cx,cy)) {
            //On range X a droite de Y
            TabDroiteUn[laCouche][y]|=listeCat[0]->Val;
            TabDroite[y]|=listeCat[0]->Val;
            TabGauche[x]|=listeCat[1]->Val;
        }
     return;
    }
     

    for (int i=0;i<listeTraits.dernier;i++) {
        trY=listeTraits[i];
        x=listeCat[i]->val;
        cx=listeCat[i];
   
        for (int j=0;j<listeTraits.dernier;j++) {
            trX=listeTraits[j];
            verifTab=TabDroiteUn[laCouche][x] & listeCat[j]->Val;

            if (!verifTab && trX!=trY) {
                y=listeCat[j]->val;
                cy=listeCat[j];
                droite=OUI;
                if (ExistePL(laCouche,trX,trY,cx,cy)) 
                    droite=NON;
                else 
                    droite=TestTransitivite(laCouche,trY,trX,listeTraits,trX,trY,cy,cx);

                if (droite) {
                    TabDroiteUn[laCouche][x]|=listeCat[j]->Val;
                    TabDroite[x]|=listeCat[j]->Val;
                    TabGauche[y]|=listeCat[i]->Val;
                    if (TabRegle[laCouche][y]) 
                        TabDroiteUn[laCouche][x]|=TabRegle[laCouche][y];
                }
            }
        }
    }
}
    
/*----------------------------------------------------------------------------------------------*/
void GlobalParseur::ConstruitDerniers(int laCouche,Cat* tete, TVecteur  cle) {
    int i,j,k;
    int v[NbCats];
    TVecteur  x,c[NbCats];
    short test;

    k=0;
    for (i=0;i<=MaxIndiceCat;i++) {
        x=Un << i%gBitVect;
        if (x & cle) {
            c[k]=x;
            v[k]=i;
            k++;
        }
    }

    for (i=0;i < k;i++) {
        test=1;
        for (j=0;j < k;j++)
            if (i !=j)
                if ((c[i] & TabGauche[v[j]]) && !(c[i] & TabDroite[v[j]])) {
                    test=0;break;
                }
    } 
     
    for (i=0;i < k;i++) {
        test=1;
        for (j=0;j < k;j++)
            if (i !=j)
                if ((c[i] & TabGauche[v[j]]) && !(c[i] & TabDroiteUn[laCouche][v[j]])) {
                    test=0;
                    break;
                }
        if (test)
            TabDerniers[tete->val]|=c[i];
    }
}
	 

/*-------------------------On construit le vecteur ATTENTE-----------------------*/
void GlobalParseur::ConsATTENTE(int laCouche) {
    TVecteur  v,V;
    short i,j,nb,NB;
 
    for (i=0;i<=MaxIndiceCat;i++) {
        V=Un << i%gBitVect;nb=0;NB=0;
        for (j=0;j<=MaxIndiceCat;j++) {
            v= Un << j%gBitVect;
            if (TabRegle[laCouche][j]) { //si j est une categorie non lexicale
                if (i != j && (TabGauche[i] & v)) { //et si j appartient a une categorie a gauche d'une autre categorie
                    ATTENTE|=V; //alors il peut y avoir attente...
                    TabContexte[i]|=TabFinals[j];
                    TabContexte[i]|=v;
                }
            }
        }
    }
}

/*
	 if (V & TabRegle[laCouche][j]) {if (!TabLexique[i]) nb++; else NB++;}
	 if (NB > 1) RETIRE|=V;
	 if (nb > 1) {ATTENTE|=V;TabContexte[i]|=~0;break;}}}}}  */   

/*-------------------On enrichit les differents vecteurs avec les categories marquees par une etoile ---*/
void GlobalParseur::FinalisePourEtoile(int laCouche) {
    Categorie* c;

    for (int i=0;i<Categories.dernier;i++) {
        c=Categories[i];
        if (c->etoile) {
            ATTENTE|=c->X.Val;
            TabDroiteUn[laCouche][c->X.val]|=c->X.Val;
            TabDroite[c->X.val]|=c->X.Val;
            TabGauche[c->X.val]|=c->X.Val; 
            TabContexte[c->X.val]|=TabGauche[c->X.val];
            TabContexte[c->X.val]|=TabFinals[c->X.val];
        }
    }
}
/*-------------------------On construit le vecteur TabMembre-----------------------*/
void GlobalParseur::ConsMembre(int laCouche) {
    TVecteur  V;
    short i,j;
 
    for (i=0;i<=MaxIndiceCat;i++) {
        if (TabRegle[laCouche][i]) {
            V=Un << i%gBitVect;
            for (j=0;j<=MaxIndiceCat;j++) {
                if (i!=j && (V & TabRegle[laCouche][j])) 
                    TabMembre[i] |= Un << j%gBitVect;
            }
        }
    }
}
       
/*-------------------------On construit le vecteur TabFinals-----------------------*/
void GlobalParseur::ConsFinals(int laCouche) {
    short i,j,k;
 
    for (i=0;i<=MaxIndiceCat;i++) TabFinals[i]=TabDerniers[i];
    for (i=0;i<=MaxIndiceCat;i++)
        if (TabDerniers[i])
            for (j=0;j<=MaxIndiceCat;j++) {
                if (TabMembre[i] & Un << j%gBitVect) {
                    TabFinals[j] |= TabDerniers[i];
                    if (TabMembre[j])
                        for (k=0;k<=MaxIndiceCat;k++)
                            if ((TabMembre[j] & Un << k%gBitVect) && TabDerniers[k])
				TabFinals[k]|=TabFinals[j];
                }
            }
}
				
			   

/*-------------------------On applatit une liste de traits-----------------------*/
void GlobalParseur::Applatit(ChaineTraits* Tr,
                             VECTA<Traits*>& listeTraits,
                             VECTA<Cat*>& listeCat) {
  
    while (Tr) {
        listeTraits.ajoute(Tr->traits);
        listeCat.ajoute(Tr->X);
        Tr=Tr->suivant;
    }
}
/*-------------------------On construit les tableaux de tete et de droite-----------------------*/
void GlobalParseur::CalculTeteDroite() {

    for (int i=0;i<=MaxProfondeurAnalyse;i++) {
        Regle* r=ListeDesRegles[i];

        //On remet a jour les differents tableaux
        RAZContextePL();

        //ConsMembre(i);
 
        while (r) {
            VECTA<Traits*> listeTraits;
            VECTA<Cat*> listeCat;

            Doublon* doublons=r->doublon;
            while (doublons!=NULL) {
                TabDroiteUn[i][(doublons->X)->val]|=(doublons->X)->Val;
                doublons=doublons->suivant;
            }
            listeTraits.raz();
            listeCat.raz();
            Applatit(r->traits,listeTraits,listeCat);
            ConstruitTeteDroite(i,listeTraits,listeCat);
            r=r->suivant;
        }

        
        r=ListeDesRegles[i]; 
        while (r) { 
            ConstruitDerniers(i,r->Sx,r->cle); 
            r=r->suivant; 
        } 
        
        ConsFinals(i); 
        ConsATTENTE(i); 
        FinalisePourEtoile(i);
        
    }
}
