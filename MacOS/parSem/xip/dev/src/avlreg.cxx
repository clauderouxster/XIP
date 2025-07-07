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
   filename   : avlreg.cxx
   Date       : 10/01/1999
   Purpose    : Implementation of ID-rules
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"


 
CREERAMASSE(ChaineCat);

Doublon::Doublon(GlobalParseur* pars,Cat* x,char etoile) {
    X=x;
    if (!etoile)
        nb=1;
    else
        nb=pars->MaxDoublons;
    suivant=0;
}

Doublon::Doublon(Doublon* d) {

    X=d->X;
    nb=d->nb;
    suivant=NULL;
    if (d->suivant!=NULL)
        suivant=new Doublon(d->suivant);
}

char Doublon::insere(GlobalParseur* pars,Cat* x,char etoile) {
    Doublon* s=this;
    
    while (s != NULL) {
        if (x->val==(s->X)->val) {
            if (etoile)
                s->nb=pars->MaxDoublons;
            else
                s->nb+=1;

            if (s->nb > pars->MaxDoublons)
                s->nb=pars->MaxDoublons;
                
            return NON;
        }
        s=s->suivant;
    }
    
    s=new Doublon(pars,x,etoile);
    s->suivant=suivant;
    suivant=s;
    return OUI;
}

Doublon* Doublon::recherche(Cat* x) {
    Doublon* s=this;
    
    while (s != NULL) {
        if (x->val==(s->X)->val)
            return s;
        s=s->suivant;
    }
    
    return NULL;
}
  






//Cette methode verifie si deux regles ont exactement la meme forme...
//On regarde simplement si elles partagent les memes traits...
//On rappelle que les traits (Traits) sont partages pour l'ensemble des
//regles.
char Regle::egalite(Regle* r) {
    ChaineTraits* ch;
    ChaineTraits* rch;

    ch= traits;
    
    if (r==NULL || r->traits==NULL) 
        return NON;
    
    while (ch != NULL) {
        rch=r->traits;
        while (rch!= NULL && ch->traits != rch->traits)
            rch=rch->suivant;
        
        if (rch==NULL)
            return NON;
        ch=ch->suivant;
    }
    
    return OUI;
}    
    

void Regle::insere(Regle* r) {
	courant->suivant=r;
	courant=r;
}
    
    /*Duplication d'une règle*/

Regle::Regle(Regle* r) {
	
    short i;
    
	frere=r->frere;
	r->frere=this;
	couche=r->couche;
	identificateur=r->identificateur;
	numero=r->numero;
	label=r->label;
	
    courant=this;
    cle=r->cle;
    last=r->last;
    first=r->first;
    passageLexical=r->passageLexical;
    gauche=r->gauche;
    droit=r->droit;
    listes=r->listes;
    detfonc=NON;
    IDoublon=r->IDoublon;
    Sx=r->Sx;
    suivant=NULL;
    Nombre=0;

    if (r->traits)
        traits=new ChaineTraits(r->traits,Nombre);
    else
        traits=NULL;

    i=0;
    if (r->TeteTraits) 
        TeteTraits=new ChaineTraits(r->TeteTraits,i);
    else 
        TeteTraits=NULL;
    
    doublon=NULL;
    if (r->doublon != NULL)
        doublon=new Doublon(r->doublon);
}

Regle::Regle(Cat* X) {
	
	frere=NULL;
    cle=X->Val;
    IDoublon=0;
    doublon=NULL;
    passageLexical=NON;
    gauche=NULL;
    droit=NULL;
    listes=NULL;
    detfonc=OUI;
    traits=NULL;
    Nombre=0;
    last=NULL;
    first=NULL;

}

Regle::Regle(Cat* X,Traits* tr) {
	

	frere=NULL;
    passageLexical=NON;
    traits=NULL;
    Nombre=0;
    suivant=NULL;
    IDoublon=0;
    last=NULL;
    first=NULL;

    if (tr)
        TeteTraits=new ChaineTraits(tr,X,NON); 
    else
        TeteTraits=NULL;
    
    cle=0;
    Sx=X;
    numero=0;
    courant=this;
    doublon=NULL;
    gauche=NULL;
    droit=NULL;
    listes=NULL;
    detfonc=OUI;
    
}

char Regle::TestContexte(GlobalParseur* pars,
                         LListe& Base, 
                         size_t ipremier,
                         size_t idernier,
                         VECTA<Noeud*>& vect_noeuds) {

	borneG=-1;
	borneD=-1;

	if (gauche!=NULL) {
   
        if (gauche->TestUnContexte(pars,Base,ipremier,-1,NON,borneG,&vect_noeuds)==NON) {
            if (pars->TRACE) 
                trace<<pars->CHAINES[205]<<endl;
            return NON;
        }
    }

    if (droit!=NULL) {
        if (droit->TestUnContexte(pars,Base,idernier,1,NON,borneD,&vect_noeuds)==NON) {
            if (pars->TRACE) 
                trace<< pars->CHAINES[206]<<endl;
            return NON;
        }
    }    
    return OUI;
}

char Regle::TestExpressionBooleen(GlobalParseur* parseur,VECTA<Noeud*>& vect_noeuds) {
    if (listes!=NULL)
        return listes->Test(parseur,vect_noeuds);
    return OUI;
}

char Regle::Contient(Regle* r) {
    short i;
    Doublon* d;
    Doublon* s;

    if (r->cle==(r->cle & cle) && IDoublon >= r->IDoublon) {
        i=0;
        d=r->doublon;
        while (d!=NULL) {
            s=doublon->recherche(d->X);
            if (s==NULL || s->nb < d->nb)
                return NON;
            d=d->suivant;
        }
        return OUI;
    }
    return NON;
}
     
void Regle::affiche(char meta) {}


void Regle::insere(GlobalParseur* pars,Cat* X) {
    char ajout=OUI;

    char etoile=NON;

    if (cle==(cle|X->Val)) {
        if (doublon==NULL)
            doublon=new Doublon(pars,X,etoile);
        else
            ajout=doublon->insere(pars,X,etoile);            
        
        if (ajout)
            IDoublon++;
    } 
    else
        cle|=X->Val;
}

//Cette fonction n'est utilise que dans InitRegle (parseur.cxx)
void Regle::insere(GlobalParseur* pars,Cat* X,Traits* tr,char etoileKleene,int argu,char testLecture) {
    TVecteur inter;
    char ajout=OUI;
    char etoile;
    
    inter=cle|X->Val;

    if (tr!=NULL) {
        etoile=etoileKleene;

        //ON teste la presence du trait FinSeq (le trait last) sur l'une des categories
        //de facon a pouvoir l'isoler dans la sequence lors  du parsage
        if ((tr->valeur)->contient((pars->FinSeq)->valeur) ||
            tr->present!=NULL &&
            (tr->present)->contient((pars->FinSeq)->valeur) ) {

            if (last != NULL) 
                pars->erreur(pars->CHAINES[215]);

            last=X;
        }
        
        //ON teste la presence du trait DebutSeq (le trait first) sur l'une des categories
        //de facon a pouvoir l'isoler dans la sequence lors  du parsage
        if ((tr->valeur)->contient((pars->DebutSeq)->valeur) ||
            tr->present!=NULL &&
            (tr->present)->contient((pars->DebutSeq)->valeur) ) {

            if (first != NULL) 
                pars->erreur(pars->CHAINES[216]);
            first=X;
        }
    }
    else
        etoile=NON;



    if (inter==cle || etoile){
        if (doublon==NULL)
            doublon=new Doublon(pars,X,etoile);
        else
            ajout=doublon->insere(pars,X,etoile);            

        if (ajout)
            IDoublon++;
        
    }

    cle=inter;
    
    if (tr) {
        if (!traits) 
            traits=new ChaineTraits(tr,X,etoileKleene,argu,testLecture);
        else
            traits->insere(new ChaineTraits(tr,X,etoileKleene,argu,testLecture));
        Nombre++;
    }
    
}

char Regle::retire(Cat* X) {
  /*unsigned long inter;*/

  if (doublon==NULL) 
      cle-=X->Val;
  else {
      Doublon* r=doublon->recherche(X);
      if (r==NULL)
          cle-=X->Val;
      else {
          r->nb--;
          if (!r->nb) {
              if (r==doublon) {
                  doublon=doublon->suivant;
                  r->suivant=NULL;
                  delete r;
              }
              else {
                  Doublon* d=doublon->suivant;
                  Doublon* prec=doublon;
                  while (d!=NULL) {
                      if (d==r) {
                          prec->suivant=r->suivant;
                          r->suivant=NULL;
                          delete r;
                          break;                          
                      }
                      prec=d;
                      d=d->suivant;
                  }
              }
          }
      }
                          
  }

  return (!cle);
}

void Regle::detruit() { 
    if (traits!=NULL) 
        delete traits;

    if (TeteTraits!=NULL) 
        delete TeteTraits;
    if (suivant) suivant->detruit();
    delete this;
}

void Regle::detruitNoeud() { 
    if (traits!=NULL) 
        delete traits;

    if (TeteTraits!=NULL) 
        delete TeteTraits;    
    delete this;
}

Regle::~Regle() {
    if (detfonc) {
        if (gauche!=NULL)
            delete gauche;
        if (droit!=NULL)
            delete droit;
        if (listes!=NULL)
            delete listes;
    }
   if (doublon != NULL)
       delete doublon;
}


//Cette methode permet de verifier si la derniere categorie d'une sequence (une phrase) correspond
//a la categorie qui a ete marque comme <last> ou <first> dans la regle...
char Regle::testDerniereCat(Cat* X) {

//Si aucune categorie n'a ete marquee on retourne OUI...
    if (last==NULL)
        return OUI;
    
    return (last==X);
}

char Regle::testPremiereCat(Cat* X) {

//Si aucune categorie n'a ete marquee on retourne OUI...
    if (first==NULL)
        return OUI;

    return (first==X);
}

void ChercheRegle::insere(GlobalParseur* pars,Cat* X) {
    Doublon* der=NULL;

    if (cle==(cle|X->Val)) {        
        for (int i=0;i<doublon.dernier;i++) {
            if (doublon[i]->X==X) {
                doublon[i]->nb++;
                return;
            }        
            if (der==NULL && doublon[i]->X==NULL)
                der=doublon[i];
        }
        if (der==NULL)
            if (doublon.dernier<doublon.taille) {
                doublon.dernier++;
                der=doublon[doublon.dernier-1];
            }
            else {
                der=new Doublon(pars,NULL,NON);
                doublon.ajoute(der);
            }
        der->X=X;
        der->nb=1;
    }        
    else
        cle|=X->Val;
}

void ChercheRegle::retire(Cat* X) {
    char ajout=NON;
    char etoile=NON;

    for (int i=0;i<doublon.dernier;i++) {
        if (doublon[i]->X==NULL)
            continue;
        if (doublon[i]->X==X) {
            doublon[i]->nb--;
            if (doublon[i]->nb==0) {
                doublon[i]->X=NULL; 
                if (i==doublon.dernier-1)
                    doublon.dernier--;
            }
            return;            
        }
    }
    cle-=X->Val;
}
 

void ChercheRegle::raz() {
    cle=0;   
    doublon.dernier=0;
    for (int i=0;i<doublon.dernier;i++) {
        doublon[i]->X=NULL;
        doublon[i]->nb=0;
    }
}

//------------------------------------------------------------------------


avl_Regle::avl_Regle(Regle* element) {
 autre=NULL;
 noeud=element;
 //gauche=NULL;droit=NULL;haut=0;
}

avl_Regle::~avl_Regle()  {
    noeud->detruitNoeud();
}


//void avl_Regle::insere(avl_Regle** courant,avl_Regle* ajout) {
//    
//    if (*courant==NULL) 
//        *courant=ajout;
//    else
//        if (ajout->noeud->cle == ((*courant)->noeud)->cle) {
//
//            avl_Regle* boucle=*courant; 
//            while (boucle->autre!=NULL)
//                boucle=boucle->autre;
//                        
//            boucle->autre=ajout;
//        }
//        else {
//            if (ajout->noeud->cle < ((*courant)->noeud)->cle)
//                insere(&(*courant)->gauche,ajout);
//            else insere(&(*courant)->droit,ajout);
//            hauteur(courant);
//        }
//}
//
//void avl_Regle::typage(avl_Regle** courant,int *type)
//{int hg,hd;
// avl_Regle* element;
//
// element=*courant;
// if (*type<11 && element !=NULL)
//     {if (element->gauche !=NULL) hg=(element->gauche)->haut;
//       else hg=-1;
//      if (element->droit !=NULL) hd=(element->droit)->haut;
//       else hd=-1;
//      if (hg>hd) {*type=*type*10+1;typage(&element->gauche,type);}
//       else if (hg<hd || (hg==hd && hd!=-1))
//	    {*type=*type*10+2;
//	     typage(&element->droit,type);}}}
//
//void avl_Regle::hauteur(avl_Regle** courant)
//{int hd,hg,type;
// avl_Regle* element;
//
// element=*courant;
// if (element->gauche !=NULL) hg=1+(element->gauche)->haut;
//  else hg=0;
// if (element->droit !=NULL) hd=1+(element->droit)->haut;
//  else hd=0;
// if (hg==hd || hg==hd+1) element->haut=hg;
//  else if (hd==hg+1) element->haut=hd;
//   else {type=0;
//	 if (hg>hd) element->haut=hg;
//	    else element->haut=hd;
//	 typage(courant,&type);
//	 reconstruit(courant,type);}}
//
//
//void avl_Regle::reconstruit(avl_Regle** courant,int type)
//{avl_Regle* element;
// avl_Regle* e;
// avl_Regle* eg;
// avl_Regle* ed;
//
//element=*courant;
//if (type>10)
// {switch(type)
//  {case 11:
//    {e=(element->gauche)->droit;
//     (element->gauche)->droit=element;
//     *courant=element->gauche;
//     ((*courant)->droit)->gauche=e;
//     hauteur(&(*courant)->droit);
//     hauteur(courant);
//     break;}
//  case 12:
//      {eg=((element->gauche)->droit)->gauche;
//       ed=((element->gauche)->droit)->droit;
//       ((element->gauche)->droit)->gauche=element->gauche;
//       ((element->gauche)->droit)->droit=element;
//       *courant=(element->gauche)->droit;
//       ((*courant)->gauche)->droit=eg;
//       ((*courant)->droit)->gauche=ed;
//       hauteur(&(*courant)->gauche);
//       hauteur(&(*courant)->droit);
//       hauteur(courant);
//       break;}
//  case 21:
//      {eg=((element->droit)->gauche)->gauche;
//       ed=((element->droit)->gauche)->droit;
//       ((element->droit)->gauche)->droit=element->droit;
//       ((element->droit)->gauche)->gauche=element;
//       *courant=(element->droit)->gauche;
//       ((*courant)->gauche)->droit=eg;
//       ((*courant)->droit)->gauche=ed;
//       hauteur(&(*courant)->gauche);
//       hauteur(&(*courant)->droit);
//       hauteur(courant);
//       break;}
//  case 22:
//     {e=(element->droit)->gauche;
//     (element->droit)->gauche=element;
//     *courant=element->droit;
//     ((*courant)->gauche)->droit=e;
//     hauteur(&(*courant)->gauche);
//     hauteur(courant);}}}};
//
//avl_Regle* avl_Regle::suivant() {return gauche;}
//
//avl_Regle* avl_Regle::recherche(Regle* mot) {
//
//    avl_Regle* reg=this;
//
//    while (reg!=NULL) {
//        if ( (reg->noeud)->cle==mot->cle) 
//            return reg;
//        
//        if (mot->cle < (reg->noeud)->cle)
//            reg=reg->gauche;
//        else
//            reg=reg->droit;
//    }
//    return NULL;
//}
//
//
//avl_Regle* avl_Regle::recherche(TVecteur mot) {
//    avl_Regle* reg=this;
//    
//    while (reg!=NULL) {
//        if ( (reg->noeud)->cle==mot) 
//            return reg;
//
//        if (mot < (reg->noeud)->cle)
//            reg=reg->gauche;
//        else
//            reg=reg->droit;
//    }
//    return NULL;
//}


/*-------------------------------------------------------------------------*/

avl_Regle* GlobalParseur::recherchesurclef(TVecteur cle,int laCouche) {
	map<TVecteur, avl_Regle*>::iterator test=ArbReg[laCouche].find(cle);
	if (test==ArbReg[laCouche].end())
		return NULL;
	return (*test).second;
}

void GlobalParseur::ajouteregle(int laCouche,avl_Regle* avl) {
	map<TVecteur, avl_Regle*>::iterator test=ArbReg[laCouche].find(avl->noeud->cle);
	if (test==ArbReg[laCouche].end())
		ArbReg[laCouche][avl->noeud->cle]=avl;
	else {
		avl_Regle* boucle=(*test).second; 
		while (boucle->autre!=NULL)
			boucle=boucle->autre;

		boucle->autre=avl;
	}
}

Regle* GlobalParseur::recherche(ChercheRegle& reg,avl_Regle** ar,short laCouche,char& premier) {
    Regle* r;
    avl_Regle* aR;
    
    aR=*ar;
	if (premier) {
        aR=recherchesurclef(reg.cle,laCouche);
		premier=NON;
	}

    while (aR) {
        r=aR->noeud;

        if (r->doublon==NULL && reg.doublon.dernier==0) {
            *ar=aR->autre;
            return r;
        }

        Doublon* dr=r->doublon;
        char trouve;


        while (dr!=NULL) {            
            if (dr->nb!=MaxDoublons) {
                trouve=NON;
                int i=0;
                for (i=0; i<reg.doublon.dernier;i++) {
                    if (reg.doublon[i]->X==NULL)
                        continue;
                    if (dr->X==reg.doublon[i]->X) {
                        trouve=OUI;
                        break;
                    }
                }
                
                if (trouve==NON ||
                    dr->nb!=reg.doublon[i]->nb)
                    break;
            }
            dr=dr->suivant;
        }
        
        if (dr==NULL) {
            *ar=aR->autre;
            return r;
        }
        
        aR=aR->autre;
    }
    *ar=NULL;
    return NULL;
}
/*-------------------------------------------------------------------------*/



ChaineCat::ChaineCat(GlobalParseur* pars,char code,char etoileKleene,Cat* x,Traits* ts,char tl) {
   RAMASSE(ChaineCat);
   X=x;
   Code=code;
   etoile=etoileKleene;
   arg=-1;
   testLecture=tl;
   traits=pars->FactoriseTraits(ts);
}

ChaineCat::~ChaineCat() {
	JETTE(ChaineCat);
}
/*-------------------------------------------------------------------------*/
RegleTraits::RegleTraits(Traits* p,Traits* s,Cat* c) {
    test=p;
    affecte=s;
    X=c;
}

char RegleTraits::Test(GlobalParseur* parseur,Traits* tr,Cat* c,char type) {
    if (X!=NULL && c!=NULL && c!=X)
        return NON;

    if (type==GAUCHE)
        return ITR(parseur,test,tr,NONECRIT,NULL);
    return ITR(parseur,affecte,tr,NONECRIT,NULL);
}
