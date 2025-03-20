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
   filename   : avltrait.cxx
   Date       : 10/01/1999
   Purpose    : Implementation of feature handling
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"

extern ostream* flot_erreur;


char featureroot::type() {
	return 0;
}

char arbretrait::type() {
	return 1;
}

arbretrait::arbretrait(GlobalParseur* parseur) {
	parseur->vlarbredestraits.ajoute(this);
	pere=NULL;
}

CREERAMASSE(trait);
//CREERAMASSE(Traits);
CREERAMASSE(CatTraits);

trait::trait(GlobalParseur* pars,const char* mot,int i,typeTrait ty) : avl((char*)mot) {
	RAMASSE(trait);
	permanent=NON;
    indice=i;
    type=ty;
    valeur=new Vecteur(pars->iTailleVect);
    freretrait=NULL;
}
	
trait::~trait() {
	JETTE(trait);
    //if (autre!=NULL) 
    //    delete autre;
    //if (gauche) 
    //    delete gauche;
    //if (droit) 
    //    delete droit;
    delete valeur;
}

 
void trait::insere(trait** courant,trait* ajout)
 {
  avl_insere((avl**)courant,(avl*)ajout);
 }


trait* trait::recherche(const char* mot)
 {
  return (trait*)avl_recherche((char*)mot);
 }

trait* trait::Autre() {return (trait*)autre;};
trait* trait::Frere() {return (trait*)frere;};

//-----------------------------------------------------------------------
void InsereTrait(trait** arbre,trait* liste,trait* v) {
    static trait* ATfin;
 
    if (liste->frere==NULL) 
        ATfin=liste;
    v->frere=ATfin->frere;
    ATfin->frere=v;
    ATfin=v;
    (*arbre)->insere(arbre,v);
}

/*-------------------------------------------------------------------------*/
char testduplicat(trait* tr) {
	if (tr->type==LISTEDUPLICAT || tr->type==DUPLICAT)
		return OUI;
	return NON;
}
/*-------------------------------------------------------------------------*/
trait* GlobalParseur::CreeTrait(const char* tampon) {
    if (!strcmp(s_utf8_to_latin((char*)tampon),CHAINES[0])) erreur(CHAINES[0]);
    trait* tr;

    if (ArbTraits) {
        //On verifie si le trait existe
        tr=ArbTraits->recherche(tampon);
        //On le retourne
        if (tr!=NULL) {
			if (tr->type==LISTE)
				tr->type=LISTEDUPLICAT;
			else
				tr->type=DUPLICAT;
            return tr;        
        }
    }
 
    tr=new trait(this,tampon,IndiceCreeTrait,ATTRIBUT);

    arbretrait* atr=new arbretrait(this);
    atr->nom=tampon;	
    tr->freretrait=atr;


    if (!ArbTraits) {
        ArbTraits=tr;
        LTraits=tr;
    }
    else
        InsereTrait(&ArbTraits,LTraits,tr);
 
    IndiceCreeTrait++;
    return tr;
}
/*-------------------------------------------------------------------------*/

int GlobalParseur::CreeValeur(trait* avl,const char* tampon,const char* NomPere) {
 
    if (!strcmp(s_utf8_to_latin((char*)tampon),CHAINES[2])) 
        erreur(CHAINES[0]);

    trait* tr=NULL;

    if (avl!=NULL) {
        indiceCreeValeur+=2;
        iNombreDeValeurs++;
		//On verifie si cette valeur n'a pas deja ete affectee
		trait* boucavl=avl;
		while (boucavl!=NULL) {
			if (!compchmin((unsigned char*)tampon,boucavl->cle.c)) {
				static string mess;
				mess=CHAINES[367];
				mess+=": ";
				mess+=NomPere;
				mess+="=";
				mess+=tampon;
				//sprintf(mess,"%s: %s=%s",CHAINES[367],NomPere,tampon);
				erreur(STR(mess));
			}
			boucavl=boucavl->Autre();
		}
        TableDesTraits[indiceCreeValeur]=DupliqueChaine(NomPere);
        TableDesTraits[indiceCreeValeur+1]=DupliqueChaine(tampon);
        TableAttributs.affecte(iNombreDeValeurs,avl);
        //Chaque valeur terminale correspond a un "trait"
        tr=new trait(this,tampon,iNombreDeValeurs,VALEUR);
        tr->valeur->un(this,iNombreDeValeurs);
        tr->autre=avl->autre;
        avl->autre=tr;


        arbretrait* atr=new arbretrait(this);
        atr->nom=tampon;
        tr->freretrait=atr;       
        avl->freretrait->ajoute(tr->freretrait);        


        return iNombreDeValeurs;
    }
    return -1;
}

int GlobalParseur::CreeValeurSans(trait* avl,const char* tampon,const char* NomPere) {
 
    if (!strcmp(s_utf8_to_latin((char*)tampon),CHAINES[2])) 
        erreur(CHAINES[0]);

    trait* tr=NULL;

    if (avl!=NULL) {
        indiceCreeValeur+=2;
        iNombreDeValeurs++;
		//On verifie si cette valeur n'a pas deja ete affectee
		trait* boucavl=avl;
		while (boucavl!=NULL) {
			if (!compchmin((unsigned char*)tampon,boucavl->cle.c)) {
				static string mess;
				mess=CHAINES[367];
				mess+=": ";
				mess+=NomPere;
				mess+="=";
				mess+=tampon;
				//sprintf(mess,"%s: %s=%s",CHAINES[367],NomPere,tampon);
				return boucavl->indice;
			}
			boucavl=boucavl->Autre();
		}
        TableDesTraits[indiceCreeValeur]=DupliqueChaine(NomPere);
        TableDesTraits[indiceCreeValeur+1]=DupliqueChaine(tampon);
        TableAttributs.affecte(iNombreDeValeurs,avl);
        //Chaque valeur terminale correspond a un "trait"
        tr=new trait(this,tampon,iNombreDeValeurs,VALEUR);
        tr->valeur->un(this,iNombreDeValeurs);
        tr->autre=avl->autre;
        avl->autre=tr;


        arbretrait* atr=new arbretrait(this);
        atr->nom=tampon;
        tr->freretrait=atr;       
        avl->freretrait->ajoute(tr->freretrait);        


        return iNombreDeValeurs;
    }
    return -1;
}
/*-------------------------------------------------------------------------*/

trait* GlobalParseur::RecAtt(istream& fichier,char& K,char racine) {
    char k=',',tampon[XIPBUFFERSIZE];
    trait* tr;
    
    
    k=lit(fichier,tampon);
    if (k!=':') 
        erreur(CHAINES[135]);

    //On cree un nouvel attribut
    tr=CreeTrait(tampon);    

    if (racine==OUI) {        
		featureroot* fr=new featureroot(this,NOM,(char*)tr->freretrait->nom.c_str());        
        tr->freretrait=fr;
        larbredestraits->ajoute(tr->freretrait);
    }

    K=RecVal(fichier,tr,tampon);

    return tr;
}
/*-------------------------------------------------------------------------*/
trait* GlobalParseur::CreationTrait(const char* attribut,const char* valeur) {
    trait* tc;

    tc=CreeTrait(attribut);
    int i=CreeValeur(tc,valeur,attribut);
    (tc->valeur)->un(this,i);
	larbredestraits->ajoute(tc->freretrait);
    return tc;
}


/*-------------------------------------------------------------------------*/
trait* GlobalParseur::ConstAttributs(istream& fichier, char& k) {
    trait* tr=RecAtt(fichier,k,OUI);
    if (ERREUR_SURVENUE==-1)
        erreur(CHAINES[68]);
    return tr;
}
/*-------------------------------------------------------------------------*/
//si la chaine dans tampon contient une structure de type: num-num, il s'agit
//d'une definition de domaine...

char GlobalParseur::RecDomaineValeur(trait* avl,
                                     const char* domaine,
                                     const char* NomPere) {

    int posunder=-1;
    int longueur=strlen(domaine);

    if (domaine[0]<'0' || domaine[0] >'9')
        return NON;

    int i=1;
    for (i=0;i<longueur;i++)
        if (domaine[i]<'0' || domaine[i] >'9')
            if (domaine[i]!='-')
                return NON;
            else
                posunder=i;
    
    //Si on n'a pas trouve de "-" on repart...
    if (posunder==-1)
        return NON;

    //Il s'agit d'un domaine
    char* tampon=(char*)domaine;
    int droite=atoi(tampon+posunder+1);   
    tampon[posunder]=0;
    int gauche=atoi(tampon);

    //droite contient la borne droite du domaine
    //tampon contient la borne gauche
    char chaine[10];
    int index;
    for (i=gauche;i<=droite;i++) {
        //pour chaque element du domaine, on cree une entree
        sprintf_s(chaine,10,"%d",i);
        index=CreeValeur(avl,chaine,NomPere);
        (avl->valeur)->un(this,index);
    }
    return OUI;
}


/*-------------------------------------------------------------------------*/
void CalculValeurARetirer(trait* tr, Vecteur& v) {
    
}
/*-------------------------------------------------------------------------*/

char GlobalParseur::RecVal(istream& fichier,trait* tc,const char* nom) {
    char k,kk,tampon[XIPBUFFERSIZE];
    int i;
    trait* tr;
	char permanent=NON;

    k=lit(fichier,tampon);
	if (k=='=') {
		permanent=OUI;
		k=lit(fichier,tampon);
	}
 
    //Dans le cas ou la valeur est unique et fournie sans accolade
    if ((k==']' || k==',') && tampon[0] != '\0') {
		if (permanent==OUI)
			erreur("Permanent features can only be described at the value level");
        i=CreeValeur(tc,tampon,nom);
        if (i!=-1)
            (tc->valeur)->un(this,i);        
        return k;
    }
    else //description sous la forme d'attribut
        if (k=='[') {
			if (permanent==OUI)
				erreur("Permanent features can only be described at the value level");
            kk=',';
            while (kk==',') {

                if (tc!=NULL) {
                    if (testduplicat(tc))
                        erreur(CHAINES[68]);
                    tc->type=LISTE;
                }


                tr=RecAtt(fichier,kk,NON);

                //si c'est une racine
				if (!testduplicat(tr))
					tc->freretrait->ajoute(tr->freretrait);
				else {
					arbretrait* atr=new arbretrait(this);
					atr->nom=tr->freretrait->nom+"+";
					tc->freretrait->ajoute(atr);
				}

                
                if (tr!=NULL && tc!=NULL)
                    (tc->valeur)->ou(tr->valeur);
            }
            if (kk!=']') 
                erreur(CHAINES[116]);
            else 
                return lit(fichier,tampon);
        }
        else //lecture des valeurs terminales
            if (k=='{') {
                char premier=OUI;
				if (permanent==OUI)
					tc->permanent=OUI;
                while (k!='}') {                    
                    k=lit(fichier,tampon);

                    //Cas particulier, un trait est dedouble
                    //Un attribut est present plusieurs fois sous le meme nom dans des structures differentes
                    if (tampon[0]==0 && premier==OUI) {
                        if (!testduplicat(tc))
                            erreur(CHAINES[244]);                            
                        return lit(fichier,tampon);
                    }
                    
                    premier=NON;
                    if (k!=',' && k!='}') 
                        erreur(CHAINES[131]);
                    
                    if (testduplicat(tc))
                        erreur(CHAINES[68]);
                    
                    //S'il ne s'agit pas d'une valeur de type:[0-n]
                    if (RecDomaineValeur(tc,tampon,nom)==NON) {
                        //On cree alors une valeur terminale
                        i=CreeValeur(tc,tampon,nom);
                        //Puis on met a jour le bit correspondant
                        //pour l'attribut
                        if (i!=-1)
                            (tc->valeur)->un(this,i);
                    }                
                }
                return lit(fichier,tampon);
            }
            else 
                erreur(CHAINES[157]);
    
    return NON;
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

Traits::Traits(Traits* tr) {
    //RAMASSE(Traits);
	
    filtre=new Vecteur(tr->filtre);
    valeur=new Vecteur(tr->valeur);
    if (tr->permanent) 
        permanent=new Vecteur(tr->permanent); 
    else 
		permanent=NULL;
    if (tr->negation) 
        negation=new Vecteur(tr->negation); 
    else negation=NULL;

	
    if (tr->bloquage) 
        bloquage=new Vecteur(tr->bloquage); 
    else bloquage=NULL;
    if (tr->libre) 
        libre=new Vecteur(tr->libre); 
    else libre=NULL;

    if (tr->efface) 
        efface=new Vecteur(tr->efface); 
    else efface=NULL;
 
    if (tr->present) 
        present=new Vecteur(tr->present); 
    else present=NULL;

    if (tr->lpresent) 
        lpresent=new Vecteur(tr->lpresent); 
    else lpresent=NULL;

    maniplemme=tr->maniplemme;
    manipsurface=tr->manipsurface;
    varlemme=tr->varlemme;
    varsurface=tr->varsurface;

    if (tr->lemme!=NULL) {
        if (tr->lemme->Type()==NON)
            lemme=new automate;
        else
            lemme=new automatepile;
        lemme->duplique(tr->lemme);
    }
    else
        lemme=NULL;    
   
    if (tr->surface!=NULL) {
        if (tr->surface->Type()==NON)
            surface=new automate;
        else
            surface=new automatepile;
        surface->duplique(tr->surface);
    }
    else
        surface=NULL;  

    onto=NULL;
    if (tr->onto!=NULL) {
        VECTA<ontologie*>& tronto = *tr->onto;
        onto=new VECTA<ontologie*>;
        for (int i=0;i<tronto.dernier;i++) {
            if (tronto[i]!=NULL)
                onto->affecte(i,new ontologie(tronto[i]));
        }
    }

}

Traits::Traits(Traits& tr) {

    //RAMASSE(Traits);
    filtre=new Vecteur(tr.filtre);
    valeur=new Vecteur(tr.valeur);
    if (tr.permanent) 
        permanent=new Vecteur(tr.permanent); 
    else 
		permanent=NULL;

    if (tr.negation) 
        negation=new Vecteur(tr.negation); 
    else negation=NULL;

	
    if (tr.bloquage) 
        bloquage=new Vecteur(tr.bloquage); 
    else bloquage=NULL;
    if (tr.libre) 
        libre=new Vecteur(tr.libre); 
    else libre=NULL;

    if (tr.efface) 
        efface=new Vecteur(tr.efface); 
    else efface=NULL;
 
    if (tr.present) 
        present=new Vecteur(tr.present); 
    else present=NULL;

    if (tr.lpresent) 
        lpresent=new Vecteur(tr.lpresent); 
    else lpresent=NULL;

    maniplemme=tr.maniplemme;
    manipsurface=tr.manipsurface;
    varlemme=tr.varlemme;
    varsurface=tr.varsurface;


    if (tr.lemme!=NULL) {
        if (tr.lemme->Type()==NON)            
            lemme=new automate;
        else
            lemme=new automatepile;
        lemme->duplique(tr.lemme);
    }
    else
        lemme=NULL;    
   
    if (tr.surface!=NULL) {
        if (tr.surface->Type()==NON)
            surface=new automate;
        else
            surface=new automatepile;
        surface->duplique(tr.surface);
    }
    else
        surface=NULL;    

    onto=NULL;
    if (tr.onto!=NULL) {
        VECTA<ontologie*>& tronto = *tr.onto;
        onto=new VECTA<ontologie*>;
        for (int i=0;i<tronto.dernier;i++) {
            if (tronto[i]!=NULL)
                onto->affecte(i,new ontologie(tronto[i]));
        }
    }
}
	

Traits::Traits(Traits* tr,short* calcul) {    
	//RAMASSE(Traits);
    ++*calcul;
    filtre=new Vecteur(tr->filtre);
    valeur=new Vecteur(tr->valeur);
    if (tr->permanent) 
        permanent=new Vecteur(tr->permanent); 
    else 
		permanent=NULL;

    if (tr->negation) negation=new Vecteur(tr->negation); else negation=NULL;
	 
	 
    if (tr->bloquage) bloquage=new Vecteur(tr->bloquage); else bloquage=NULL;
    if (tr->libre) libre=new Vecteur(tr->libre); else libre=NULL;
    if (tr->efface) efface=new Vecteur(tr->efface); else efface=NULL;
    if (tr->present) present=new Vecteur(tr->present); else present=NULL;
    if (tr->lpresent) lpresent=new Vecteur(tr->lpresent); else lpresent=NULL;

    maniplemme=tr->maniplemme;
    manipsurface=tr->manipsurface;
    varlemme=tr->varlemme;
    varsurface=tr->varsurface;


    if (tr->lemme!=NULL) {
        lemme=new automate;
        if (tr->lemme->Type()==NON)
            lemme=new automate;
        else
            lemme=new automatepile;
        lemme->duplique(tr->lemme);
    }
    else
        lemme=NULL;

    if (tr->surface!=NULL) {
        if (tr->surface->Type()==NON)
            surface=new automate;
        else
            surface=new automatepile;
        surface->duplique(tr->surface);
    }
    else
        surface=NULL;    

    onto=NULL;
    if (tr->onto!=NULL) {
        VECTA<ontologie*>& tronto = *tr->onto;
        onto=new VECTA<ontologie*>;
        for (int i=0;i<tronto.dernier;i++) {
            if (tronto[i]!=NULL)
                onto->affecte(i,new ontologie(tronto[i]));
        }
    }
}
	
Traits::Traits(GlobalParseur* p) {
    //RAMASSE(Traits);
    negation=NULL;bloquage=NULL;libre=NULL;
    present=NULL;
    lpresent=NULL;
    efface=NULL;
    valeur=new Vecteur(p->iTailleVect);

    maniplemme=COMPARE;
    manipsurface=COMPARE;
    varlemme=NULL;
    varsurface=NULL;

    
    filtre=new Vecteur(p->iTailleVect);
	permanent=NULL;
    lemme=NULL;
    surface=NULL;
    onto=NULL;
}

Traits::Traits(int taillevect) {
    //RAMASSE(Traits);
    negation=NULL;bloquage=NULL;libre=NULL;
    present=NULL;
    lpresent=NULL;
    efface=NULL;
    valeur=new Vecteur(taillevect);

    maniplemme=COMPARE;
    manipsurface=COMPARE;
    varlemme=NULL;
    varsurface=NULL;

    
    filtre=new Vecteur(taillevect);
	permanent=NULL;
    lemme=NULL;
    surface=NULL;
    onto=NULL;
}

Traits::~Traits() {
    //JETTE(Traits);
    if (onto!=NULL) {
        onto->nettoie();
        delete onto;
    }
    delete filtre;
    delete valeur;
    if (lemme!=NULL)
        delete lemme;
    if (surface!=NULL)
        delete surface;
    if (negation) delete negation;
    if (bloquage) delete bloquage;
    if (libre) delete libre;
    if (efface) delete efface;
    if (present) delete present;
    if (lpresent) delete lpresent;
	if (permanent) delete permanent;
    
}
	
  
void Traits::TraitsEnChaine(GlobalParseur* pars,char* resultat)
{
    valeur->TraitsEnChaine(pars,resultat,libre,present,bloquage,negation);
}

void Traits::TraitsEnMap(GlobalParseur* pars, map<string, string>& m) {
	valeur->TraitsEnMap(pars, m, libre, present, bloquage, negation);
}

void Traits::TraitsEnChaineString(GlobalParseur* pars,string& resultat,Vecteur* filtrage)
{
    if (filtrage==NULL)
		valeur->TraitsEnChaineString(pars,resultat,libre,present,bloquage,negation);
	else
		valeur->ConcateneValeursString(pars,resultat,filtrage,",",-1);

	if (onto!=NULL) {
		char ajout=OUI;
		if (resultat[0]=='[')
			resultat=resultat.substr(0,resultat.length()-1);
		else
			resultat="[";
		for (int i=0;i<onto->dernier;i++) {
			if (resultat.length()>1)
				resultat+=",";
			(*onto)[i]->ConcateneValeurs(pars,pars->hierarchies->lahierarchie,this,resultat,",");
		}
		resultat+="]";
	}
}

 /*-------------------------------------------------------------------------------------*/


     
void Traits::AfficheValeursXML(GlobalParseur* parseur,ostream& os,Vecteur* filtrexml) {    
    valeur->AfficheValeursXML(parseur,os,filtrexml);    
    
    if (onto!=NULL && onto->cell(ONTOLOGIE_COMPARAISON)!=NULL)
		onto->cell(ONTOLOGIE_COMPARAISON)->AfficheValeursXML(parseur->hierarchies->lahierarchie,parseur->TraitsFiltreOntologie,os);
}

char Traits::egal(Traits* tr) {
    if (valeur->egal(tr->valeur)) 
        return OUI;
    return NON;
}
 

char Traits::contient(Traits* tr) {
    return (valeur->contient(tr->valeur) &&
            (!tr->negation || !valeur->contientNegation(tr->negation)) &&
            (!negation || !(tr->valeur)->contientNegation(negation)));
}


void Traits::ou(trait* tr) {
    
    valeur->ou(tr->valeur);
    filtre->ou(tr->valeur);    
	if (tr->permanent==OUI) {
		if (permanent==NULL)
			permanent=new Vecteur(tr->valeur);
		else
			permanent->ou(tr->valeur);
	}
}

void Traits::oup(trait* tr) {
    
    valeur->ou(tr->valeur);
    filtre->ou(tr->valeur);    
	if (permanent==NULL)
		permanent=new Vecteur(tr->valeur);
	else
		permanent->ou(tr->valeur);
}


void Traits::inittrait(trait* attribut,trait* val) {
	valeur->ou(val->valeur);
    filtre->ou(attribut->valeur);    
	if (attribut->permanent==OUI) {
		if (permanent==NULL)
			permanent=new Vecteur(val->valeur);
		else
			permanent->ou(val->valeur);
	}
}

void Traits::etnon(trait* tr) {
    
    valeur->etnon(tr->valeur);
    filtre->etnon(tr->valeur);
    
}


void Traits::razp() {
	if (permanent) {
		delete permanent;
		permanent=NULL;
	}
	raz();
}

void Traits::raz() {
  
    valeur->raz();
    filtre->raz();

	//les traits permanents ne peuvent etre effaces...
	if (permanent) {
		valeur->ou(permanent);
		filtre->ou(permanent);
    }

    if (negation) {
        delete negation;
        negation=NULL;
    }
   
    if (bloquage) {
        delete bloquage;
        bloquage=NULL;
    }
  
    if (libre) {
        delete libre;
        libre=NULL;
    }

    if (efface) {
        delete efface;
        efface=NULL;
    }
     
    if (present) {
        delete present;
        present=NULL;
    }

    if (lpresent) {
        delete lpresent;
        lpresent=NULL;
    }


    if (onto!=NULL) {
        onto->nettoie();
        delete onto;
        onto=NULL;
    }
}

void Traits::etnon(Traits* tr) {
    if (tr==NULL)
        return;

    TVecteur* val= valeur->vecteur;
    TVecteur* trval= tr->valeur->vecteur;
    TVecteur* fil = filtre->vecteur;
    TVecteur* trfil = tr->filtre->vecteur;  

    char neg=NON;
    if (tr->negation)
        if (negation) 
            neg=OUI;
        else
            negation=new Vecteur(tr->negation);

    char bloc=NON;
    if (tr->bloquage) 
        if (bloquage)
            bloc=OUI;
        else
            bloquage=new Vecteur(tr->bloquage);

    char lib=NON;
    if (tr->libre) 
        if (libre) 
            lib=OUI;
        else libre=new Vecteur(tr->libre);
    char eff=NON;
    if (tr->efface) 
        if (efface) 
            eff=OUI;
        else efface=new Vecteur(tr->efface);

    char pres=NON;
    if (tr->present) 
        if (present) 
            pres=OUI;
        else present=new Vecteur(tr->present);

    char lpres=NON;
    if (tr->lpresent) 
        if (lpresent) 
            lpres=OUI;
        else 
            lpresent=new Vecteur(tr->lpresent);    

        
    int i;
    for (i=0; i<valeur->TailleVect;i++) {
        *val&=~*trval;
        *fil&=~*trfil;


        if (neg) 
            negation->vecteur[i]&=~tr->negation->vecteur[i];
        if (bloc)
            bloquage->vecteur[i]&=~tr->bloquage->vecteur[i];
        if (lib)
            libre->vecteur[i]&=~tr->libre->vecteur[i];
        if (eff)
            efface->vecteur[i]&=~tr->efface->vecteur[i];
        if (pres) 
            present->vecteur[i]&=~tr->present->vecteur[i];        
        if (lpres) 
            lpresent->vecteur[i]&=~tr->lpresent->vecteur[i];        

		if (permanent!=NULL) {//les traits qui ne peuvent etre perdu
			*val|=permanent->vecteur[i];
			*fil|=permanent->vecteur[i];
		}
        
        trval++;
        val++;
        trfil++;
        fil++;
    }           

     //Intersection d'elements ontologique
    //Le resultat est stocke dans "this"
    if (tr->onto && onto!=NULL) {
        VECTA<ontologie*>& tronto = *tr->onto;
        VECTA<ontologie*>& loconto=*onto;
        
        for (i=loconto.dernier-1;i>=0;i--) {
            if (loconto[i]==NULL || tronto[i]==NULL)
                continue;
            if (loconto[i]->present(tronto[i]))
                loconto[i]->etnon(tronto[i]);                    
        }
    }
}

void Traits::et(Traits* tr) {
    if (tr==NULL)
        return;

    //Intersection d'elements ontologique
    //Le resultat est stocke dans "this"
    if (tr->onto && onto!=NULL) {
        VECTA<ontologie*>& tronto = *tr->onto;
        VECTA<ontologie*>& loconto=*onto;
        
        for (int i=loconto.dernier-1;i>=0;i--) {    
            if (loconto[i]==NULL)
                continue;                     
            
            //si cet element est nul, on retire le loconto equivalent
            if (tronto[i]==NULL) {
                delete loconto.retireElement(i);
                continue;
            }
            
            //Soit on effectue une intersection
            if (loconto[i]->present(tronto[i]) ||
                tronto[i]->present(loconto[i]))
                loconto[i]->et(tronto[i]);                                                
            else //soit on le retire
                delete loconto.retireElement(i);
        }                    
    }



    TVecteur* val= valeur->vecteur;
    TVecteur* trval= tr->valeur->vecteur;
    TVecteur* fil = filtre->vecteur;
    TVecteur* trfil = tr->filtre->vecteur;  
    char neg=NON;
    if (tr->negation)
        if (negation) 
            neg=OUI;
        else
            negation=new Vecteur(tr->negation);
    char bloc=NON;
    if (tr->bloquage) 
        if (bloquage)
            bloc=OUI;
        else
            bloquage=new Vecteur(tr->bloquage);

    char lib=NON;
    if (tr->libre) 
        if (libre) 
            lib=OUI;
        else libre=new Vecteur(tr->libre);
    char eff=NON;
    if (tr->efface) 
        if (efface) 
            eff=OUI;
        else efface=new Vecteur(tr->efface);

    char pres=NON;
    if (tr->present) 
        if (present) 
            pres=OUI;
        else present=new Vecteur(tr->present);

    char lpres=NON;
    if (tr->lpresent) 
        if (lpresent) 
            lpres=OUI;
        else 
            lpresent=new Vecteur(tr->lpresent);    
    
    for (int i=0; i<valeur->TailleVect;i++) {
        *val&=*trval;
        *fil&=*trfil;
        
        if (neg) 
            negation->vecteur[i]&=tr->negation->vecteur[i];
        if (bloc)
            bloquage->vecteur[i]&=tr->bloquage->vecteur[i];
        if (lib)
            libre->vecteur[i]&=tr->libre->vecteur[i];
        if (eff)
            efface->vecteur[i]&=tr->efface->vecteur[i];
        if (pres) 
            present->vecteur[i]&=tr->present->vecteur[i];        
        if (lpres) 
            lpresent->vecteur[i]&=tr->lpresent->vecteur[i];        

		if (permanent!=NULL) {//les traits qui ne peuvent etre perdu
			*val|=permanent->vecteur[i];
			*fil|=permanent->vecteur[i];
		}
 
        trval++;
        val++;
        trfil++;
        fil++;
    }    
    
}

char Traits::vide() {
	//cas particulier: traits last et first
	//Que l'on doit retirer
	TVecteur v=valeur->vecteur[0];
	valeur->vecteur[0]&=(~3);
	//valeur->vecteur[0]&=(~2);
	char res=valeur->Zero();
	valeur->vecteur[0]=v;
	return res;
}

//Verifie que deux structures de traits sont parfaitement egales...
char Traits::egaux(Traits* tr) {


    if (!valeur->egal(tr->valeur))
        return NON;

//   if (!filtre->egal(tr->filtre))
//       return NON;


   if (tr->negation) 
       if (negation) {
          if (!negation->egal(tr->negation))
              return NON;
       }
       else
           return NON;
   else
       if (negation)
           return NON;

   if (tr->permanent) 
       if (permanent) {
          if (!permanent->egal(tr->permanent))
              return NON;
       }
       else
           return NON;
   else
       if (permanent)
           return NON;

 
   if (tr->bloquage) 
       if (bloquage) {
          if (!bloquage->egal(tr->bloquage))
              return NON;
       }
       else
           return NON;
   else
       if (bloquage)
           return NON;


   if (tr->libre) 
       if (libre) {
           if (!libre->egal(tr->libre))
               return NON;
       }
       else
           return NON;
   else
       if (libre)
           return NON;


   if (tr->efface) 
       if (efface) {
           if (!efface->egal(tr->efface))
               return NON;
       }
       else
           return NON;
   else
       if (efface)
           return NON;

   if (tr->present) 
       if (present) {
           if (!present->egal(tr->present))
               return NON;
       }
       else
           return NON;
   else
       if (present)
           return NON;

   if (tr->lpresent) 
       if (lpresent) {
           if (!lpresent->egal(tr->lpresent))
               return NON;
       }
       else
           return NON;
   else
       if (lpresent)
           return NON;


   if (tr->maniplemme!=maniplemme)
       return NON;
   
   if (tr->manipsurface!=manipsurface)
       return NON;

   if (tr->varlemme!=varlemme)
       return NON;
   
   if (tr->varsurface!=varsurface)
       return NON;

   if (lemme==NULL) {
       if (tr->lemme!=NULL)
           return NON;
   }
   else {
       if (tr->lemme==NULL)
           return NON;
       else
           if (lemme->compare(tr->lemme))
               return NON;
   }
          
           
   if (onto==NULL) {
       if (tr->onto!=NULL)
           return NON;
   }
   else {
       if (tr->onto==NULL)
           return NON;
       if (onto->dernier!=tr->onto->dernier)
           return NON;
       VECTA<ontologie*>& tronto = *tr->onto;
       VECTA<ontologie*>& loconto = *onto;
       
       for (int i=0;i<loconto.dernier;i++) {
           if (loconto[i]==NULL) {
               if (tronto[i]!=NULL)
                   return NON;
               continue;
           }

           if (tronto[i]==NULL)
               return NON;

           if (!loconto[i]->egal(tronto[i]))
               return NON;
       }
   }

   if (surface==NULL) {
       if (tr->surface!=NULL)
           return NON;
   }
   else
       if (tr->surface==NULL)
           return NON;
       else
           if (surface->compare(tr->surface))
               return NON;

   return OUI;
  }

    

// Hash table code added by Aaron 10/2006
// Adaptation of Bob Jenkins' one-at-a-time hash function
// public domain:  http://burtleburtle.net/bob/hash/doobs.html

static TVecteur mix(TVecteur clef, char *data, int length) {
    int i;
    for (i = 0; i < length; i++) {
	clef += data[i];
	clef += (clef << 10);
	clef ^= (clef >> 6);
    }
    return clef;
}

static TVecteur mixVecteur(TVecteur clef, Vecteur *v) {
    return mix(clef, (char *) v->vecteur, v->TailleVect * sizeof(v->vecteur[0]));
}

TVecteur Traits::ClefHachage(int tailleTableau) {
    
    char tampon[1000];
    // Have to use a size of 1000, because that's what the buffer
    // overflow protection in automate::chaine() assumes.

    TVecteur clef = 0;

    clef = mixVecteur(clef, valeur);
    if (negation) clef = mixVecteur(clef, negation);
    if (bloquage) clef = mixVecteur(clef, bloquage);
    if (libre)    clef = mixVecteur(clef, libre);
    if (efface)   clef = mixVecteur(clef, efface);
    if (present)  clef = mixVecteur(clef, present);
    if (lpresent) clef = mixVecteur(clef, lpresent);
    if (permanent) clef = mixVecteur(clef, permanent);
    
    clef = mix(clef, (char *) &maniplemme,   sizeof(maniplemme));
    clef = mix(clef, (char *) &manipsurface, sizeof(manipsurface));
    clef = mix(clef, (char *) &varlemme,     sizeof(varlemme));
    clef = mix(clef, (char *) &varsurface,   sizeof(varsurface));

    if (lemme) {
	lemme->chaine(tampon);
	clef = mix(clef, tampon, strlen(tampon));
    }

    if (surface) {
	surface->chaine(tampon);
	clef = mix(clef, tampon, strlen(tampon));
    }

    if (onto) {
	VECTA<ontologie*>& ontologies = *onto;
	for (int i=0; i<ontologies.dernier; i++) {
	    ontologie *o = ontologies[i];
	    if (o) {
		for (int j=0; j<o->element.dernier; j++) {
		    granule *g = o->element[j];
		    clef = mix(clef, (char *) &g->rang, sizeof(g->rang));
		    clef = mix(clef, (char *) &g->mot, sizeof(g->mot));
		}
	    }
	}
    }

    clef += (clef << 3);
    clef ^= (clef >> 11);
    clef += (clef << 15);
    
    // tailleTableau must be a power of 2 so we can simply do a bit
    // mask rather than taking the modulus.
    clef = clef & (tailleTableau - 1);
    
    return clef;

}

void Traits::recopie(Traits* tr) {
        
    if (!tr->libre && libre) {
        delete libre;
        libre=NULL;
    }
    valeur->recopie(tr->valeur);
    filtre->recopie(tr->filtre);
    if (tr->libre!=NULL) {
        if (libre==NULL)
            libre=new Vecteur(tr->libre);
        else
            libre->recopie(tr->libre);
    }
    if (tr->bloquage!=NULL) {
        if (bloquage==NULL)
            bloquage=new Vecteur(tr->bloquage);
        else
            bloquage->recopie(tr->bloquage);
    }

	if (tr->permanent!=NULL) {
        if (permanent==NULL)
            permanent=new Vecteur(tr->permanent);
        else
            permanent->ou(tr->permanent);
    }
    
    int i;
    if (tr->onto!=NULL) {        
        if (onto==NULL)
            onto=new VECTA<ontologie*>;
        else
            onto->nettoie();        
        for (i=0;i<tr->onto->dernier;i++)
            onto->ajoute(new ontologie(tr->onto->cell(i)));
    }
}

void Traits::recopie(Traits& tr) {
        
    if (!tr.libre && libre) {
        delete libre;
        libre=NULL;
    }
    valeur->recopie(tr.valeur);
    filtre->recopie(tr.filtre);
    
    if (tr.onto!=NULL) {        
        if (onto==NULL)
            onto=new VECTA<ontologie*>;
        else
            onto->nettoie();        
        for (int i=0;i<tr.onto->dernier;i++)
            onto->ajoute(new ontologie(tr.onto->cell(i)));
    }

	if (tr.permanent!=NULL) {
        if (permanent==NULL)
            permanent=new Vecteur(tr.permanent);
        else
            permanent->ou(tr.permanent);
    }

    if (tr.libre!=NULL)
        if (libre==NULL)
            libre=new Vecteur(tr.libre);
        else
            libre->recopie(tr.libre);
    if (tr.bloquage!=NULL)
        if (bloquage==NULL)
            bloquage=new Vecteur(tr.bloquage);
        else
            bloquage->recopie(tr.bloquage);
}
       
void Traits::oup(Traits* tr) {
	ou(tr);

	if (permanent) 
		permanent->ou(tr->valeur);
	else
		permanent=new Vecteur(tr->valeur);
	valeur->ou(permanent);
	filtre->ou(permanent);
    
}

       
void Traits::oup(Traits& tr) {
	ou(tr);
	if (permanent) 
		permanent->ou(tr.valeur);
	else
		permanent=new Vecteur(tr.valeur);
}

void Traits::ou(Traits* tr) {
    if (tr==NULL)
        return;
    static actionOntologie actions[]={ONTOLOGIE_COMPARAISON,ONTOLOGIE_COMPARAISON_STRICTE,ONTOLOGIE_AFFECTATION,ONTOLOGIE_LIBRE,ONTOLOGIE_BLOQUEE};

    //mise a jour de l'ontologie.
    //Pour des raisons d'efficacite, on fait en sorte
    //de ne pas rajouter des morceaux de chemin deja present.
    //Exemple, il ne faut pas rajouter @animal@mammifere, si on a deja
    //@animal@mammifere@chien. En revanche, on doit remplacer @animal@mammifere
    //par @animal@mammifere@chien, si @animal@mammifere etait present
    if (tr->onto) {
        if (onto==NULL)
            onto=new VECTA<ontologie*>;
        VECTA<ontologie*>& ronto = *tr->onto;
        VECTA<ontologie*>& loconto=*onto;
        //seul nous interesse les elements en 
        //ONTOLOGIE_AFFECTATION
        //ONTOLOGIE_COMPARAISON        
        actionOntologie action;
        for (int i=0;i<5;i++) {
            //dans le cas d'une affectation de valeur,
            //il est important aussi d'eviter de generer les 
            //hyponymes ou hyperonymes
            //est-ce que on a @animal@mammifere@chien et est-ce que on veut rajouter
            //@animal@mammifere
            
            //est-ce que on a @animal@mammifere et est-ce que on veut rajouter
            //@animal@mammifere@chien
            action=actions[i];
            if (ronto[action]==NULL)                
                continue;            
            
            if (loconto[ONTOLOGIE_COMPARAISON]!=NULL) {
                if (ronto[action]->present(loconto[ONTOLOGIE_COMPARAISON]))
                    loconto[ONTOLOGIE_COMPARAISON]->ou(ronto[action]);                
            }
            else
                loconto.affecte(ONTOLOGIE_COMPARAISON,new ontologie(ronto[action]));
        }
        if (onto->dernier==0) {
            delete onto;
            onto=NULL;
        }
    }

    
    TVecteur* val= valeur->vecteur;
    TVecteur* trval= tr->valeur->vecteur;
    TVecteur* fil = filtre->vecteur;
    TVecteur* trfil = tr->filtre->vecteur;  

    char neg=NON;
    if (tr->negation) {
        if (negation) 
            neg=OUI;
        else
            negation=new Vecteur(tr->negation);
    }

    char bloc=NON;
    if (tr->bloquage) {
        if (bloquage)
            bloc=OUI;
        else
            bloquage=new Vecteur(tr->bloquage);
    }
    
    char lib=NON;
    if (tr->libre) {
        if (libre) 
            lib=OUI;
        else 
            libre=new Vecteur(tr->libre);
    }
    
    char eff=NON;
    if (tr->efface) {
        if (efface) 
            eff=OUI;
        else 
            efface=new Vecteur(tr->efface);
    }
    
    char pres=NON;
    if (tr->present) {
        if (present) 
            pres=OUI;
        else 
            present=new Vecteur(tr->present);
    }
    
    char lpres=NON;
    if (tr->lpresent) {
        if (lpresent) 
            lpres=OUI;
        else 
            lpresent=new Vecteur(tr->lpresent);
    }
    
	char perm=NON;
    if (tr->permanent) 
        if (permanent) 
            perm=OUI;
        else 
            permanent=new Vecteur(tr->permanent);
    
    for (int i=0; i<valeur->TailleVect;i++) {
        *val|=*trval;
        *fil|=*trfil;
        
        if (neg) 
            negation->vecteur[i]|=tr->negation->vecteur[i];
        if (bloc)
            bloquage->vecteur[i]|=tr->bloquage->vecteur[i];
        if (lib)
            libre->vecteur[i]|=tr->libre->vecteur[i];
        if (eff)
            efface->vecteur[i]|=tr->efface->vecteur[i];
        if (pres) 
            present->vecteur[i]|=tr->present->vecteur[i];        
        if (lpres) 
            lpresent->vecteur[i]|=tr->lpresent->vecteur[i];        
        if (perm) 
            permanent->vecteur[i]|=tr->permanent->vecteur[i];        
        
        trval++;
        val++;
        trfil++;
        fil++;
    }    
    
}


void Traits::xou(Traits* tr) {
    if (tr==NULL)
        return;

    TVecteur* val= valeur->vecteur;
    TVecteur* trval= tr->valeur->vecteur;
    TVecteur* fil = filtre->vecteur;
    TVecteur* trfil = tr->filtre->vecteur;  
    char neg=NON;
    if (tr->negation)
        if (negation) 
            neg=OUI;
        else
            negation=new Vecteur(tr->negation);
    char bloc=NON;
    if (tr->bloquage) 
        if (bloquage)
            bloc=OUI;
        else
            bloquage=new Vecteur(tr->bloquage);

    char lib=NON;
    if (tr->libre) 
        if (libre) 
            lib=OUI;
        else libre=new Vecteur(tr->libre);

    char eff=NON;
    if (tr->efface) 
        if (efface) 
            eff=OUI;
        else efface=new Vecteur(tr->efface);

    char pres=NON;
    if (tr->present) 
        if (present) 
            pres=OUI;
        else present=new Vecteur(tr->present);

    char lpres=NON;
    if (tr->lpresent) 
        if (lpresent) 
            lpres=OUI;
        else 
            lpresent=new Vecteur(tr->lpresent);

    
    for (int i=0; i<valeur->TailleVect;i++) {
        *val^=*trval;
        *fil^=*trfil;
        
        if (neg) 
            negation->vecteur[i]^=tr->negation->vecteur[i];
        if (bloc)
            bloquage->vecteur[i]^=tr->bloquage->vecteur[i];
        if (lib)
            libre->vecteur[i]^=tr->libre->vecteur[i];
        if (eff)
            efface->vecteur[i]^=tr->efface->vecteur[i];
        if (pres) 
            present->vecteur[i]^=tr->present->vecteur[i];        
        if (lpres) 
            lpresent->vecteur[i]^=tr->lpresent->vecteur[i];        

		if (permanent!=NULL) {//les traits qui ne peuvent etre perdu
			*val|=permanent->vecteur[i];
			*fil|=permanent->vecteur[i];
		}
         
        trval++;
        val++;
        trfil++;
        fil++;
    }    
    
}

void Traits::ou(Traits& tr) {

    if (tr.onto) {
        if (onto==NULL)
            onto=new VECTA<ontologie*>;
        VECTA<ontologie*>& ronto = *tr.onto;
        VECTA<ontologie*>& loconto=*onto;

        //seul nous interesse les elements en 
        //ONTOLOGIE_AFFECTATION
        //ONTOLOGIE_COMPARAISON
        static actionOntologie actions[]={ONTOLOGIE_COMPARAISON,ONTOLOGIE_COMPARAISON_STRICTE,ONTOLOGIE_AFFECTATION,ONTOLOGIE_LIBRE,ONTOLOGIE_BLOQUEE};
        actionOntologie action;        
        for (int i=0;i<5;i++) {
            //dans le cas d'une affectation de valeur,
            //il est important aussi d'eviter de generer les 
            //hyponymes ou hyperonymes
            //est-ce que on a @animal@mammifere@chien et est-ce que on veut rajouter
            //@animal@mammifere
            
            //est-ce que on a @animal@mammifere et est-ce que on veut rajouter
            //@animal@mammifere@chien
            action=actions[i];
            if (ronto[action]==NULL)
                continue;
            if (loconto[ONTOLOGIE_COMPARAISON]!=NULL) {
                if (ronto[action]->present(loconto[ONTOLOGIE_COMPARAISON]))
                    loconto[ONTOLOGIE_COMPARAISON]->ou(ronto[action]);                
            }
            else
                loconto.affecte(ONTOLOGIE_COMPARAISON,new ontologie(ronto[action]));
        }

        if (onto->dernier==0) {
            delete onto;
            onto=NULL;
        }
    }

    TVecteur* val= valeur->vecteur;
    TVecteur* trval= tr.valeur->vecteur;
    TVecteur* fil = filtre->vecteur;
    TVecteur* trfil = tr.filtre->vecteur;  
    char neg=NON;
    if (tr.negation)
        if (negation) 
            neg=OUI;
        else
            negation=new Vecteur(tr.negation);
    char bloc=NON;
    if (tr.bloquage) 
        if (bloquage)
            bloc=OUI;
        else
            bloquage=new Vecteur(tr.bloquage);

    char lib=NON;
    if (tr.libre) 
        if (libre) 
            lib=OUI;
        else libre=new Vecteur(tr.libre);
    char eff=NON;
    if (tr.efface) 
        if (efface) 
            eff=OUI;
        else efface=new Vecteur(tr.efface);

    char pres=NON;
    if (tr.present) 
        if (present) 
            pres=OUI;
        else present=new Vecteur(tr.present);

    char lpres=NON;
    if (tr.lpresent) 
        if (lpresent) 
            lpres=OUI;
        else 
            lpresent=new Vecteur(tr.lpresent);

	char perm=NON;
    if (tr.permanent) 
        if (permanent) 
            perm=OUI;
        else 
            permanent=new Vecteur(tr.permanent);
    
    for (int i=0; i<valeur->TailleVect;i++) {
        *val|=*trval;
        *fil|=*trfil;
        
        if (neg) 
            negation->vecteur[i]|=tr.negation->vecteur[i];
        if (bloc)
            bloquage->vecteur[i]|=tr.bloquage->vecteur[i];
        if (lib)
            libre->vecteur[i]|=tr.libre->vecteur[i];
        if (eff)
            efface->vecteur[i]|=tr.efface->vecteur[i];
        if (pres) 
            present->vecteur[i]|=tr.present->vecteur[i];        
        if (lpres) 
            lpresent->vecteur[i]|=tr.lpresent->vecteur[i];        
        if (perm) 
            permanent->vecteur[i]|=tr.permanent->vecteur[i];        
        
        trval++;
        val++;
        trfil++;
        fil++;
    }    
    
}


void Traits::ou(Categorie* categ) {
    if (categ->traits!=NULL)
        ou(categ->traits);
    
}

void Traits::oup(Categorie* categ) {
	if (categ->traits!=NULL)
        oup(categ->traits);
}
 
void Traits::etnon(Categorie* categ) {
    if (categ->traits!=NULL)
        etnon(categ->traits);
    
}


//-------------------------------------------------------------------------- 
//ChaineTraits, cette classe est utilisee pour conserver une liste de 
//pointeur vers la liste des traits que toutes les regles partagent.

ChaineTraits::ChaineTraits(Traits* tr,Cat* x,char etoileKleene,int argu,char tl) {
    traits=tr;
    dejaLu=NON;
    suivant=NULL;
    autre=NULL;
    arg=argu;
    X=x;
    etoile=etoileKleene;
    testLecture=tl;
}

ChaineTraits::ChaineTraits(ChaineTraits* ch,short& nombre) {

    nombre++;
    X=ch->X;
    etoile=ch->etoile;
    traits=ch->traits;
    dejaLu=NON;
    arg=ch->arg;
    testLecture=ch->testLecture;
    if (ch->suivant)
        suivant=new ChaineTraits(ch->suivant,nombre);
    else
        suivant=NULL;
    
    if (ch->autre)
        autre=new ChaineTraits(ch->autre,nombre);
    else
        autre=NULL;

}
      
ChaineTraits::~ChaineTraits() {
    if (autre!=NULL)
        delete autre;

    if (suivant!=NULL)
        delete suivant;
}
  
void ChaineTraits::initialise() {
    dejaLu=NON;

    if (suivant) 
        suivant->initialise();
    if (autre) 
        autre->initialise();
}

void ChaineTraits::insere(ChaineTraits* tr) {
    ChaineTraits* Tr=this;
    ChaineTraits* prec=NULL;

    while (Tr && Tr->X!=tr->X) 
        Tr=Tr->suivant; //On recherche une structure de traits partageant la meme categorie


    if (Tr) { //si on en trouve une, on attache l'element apres la premiere categorie
        //On cherche le lieu ou accroche le nouvel element
        prec=NULL;

        ChaineTraits* b=Tr;
        while (b!=NULL && b->etoile==NON) {
            prec=b;
            b=b->autre;
        }
        //Dans le cas ou prec==NULL, cela signifie que le premier element est de type *
        //on l'inverse avec le suivant si leur type sont differents
        if (prec==NULL) {
            tr->autre=Tr->autre;
            Tr->autre=tr;
            if (tr->etoile!=Tr->etoile) {
                Traits* inter=Tr->traits;
                Tr->traits=tr->traits;
                tr->traits=inter;
                tr->etoile=OUI;
                Tr->etoile=NON;
            }
        }
        else {
            tr->autre=prec->autre;
            prec->autre=tr;
        }
    }
    else {
        tr->suivant=suivant;
        suivant=tr;
    }
}

/*-------------------------------------------------------------------------*/
/* La classe qui contient les abréviations obtenues dans le fichier des définitions*/

abreviation::abreviation(char* n,Traits* tr) {
  Nom=n;
  traits=tr;
  suivant=NULL;
}
   
abreviation::~abreviation() {
   
    if (traits!=NULL)
        delete traits;
    if (Nom!=NULL) 
        libere(Nom);
    if (suivant!=NULL) delete suivant;
}

void abreviation::insere(GlobalParseur* pars,char* n,Traits* tr) {
    abreviation* ab=this;
  
    while (ab && strcmp(ab->Nom,n)) 
        ab=ab->suivant;
   if (ab) {
	   static string mess=pars->CHAINES[164];
       mess+=n;
       pars->erreur(STR(mess));
   }

   ab=new abreviation(n,tr);
   ab->suivant=suivant;
   suivant=ab;
}

Traits* abreviation::recherche(GlobalParseur* pars,char* n) {
    abreviation* ab=this;
   
    while (ab && strcmpi(ab->Nom,n)) ab=ab->suivant;
   
    if (!ab) {
	   static string mess=pars->CHAINES[165];
       mess+=n;
       pars->erreur(STR(mess));
    }

    return ab->traits;
}

void abreviation::nom(Traits* TR,char* chaine) {

    chaine[0]=0;
}

char* abreviation::alias(Traits* TR) {
    abreviation* ab=this;
    
    while (ab != NULL) {
        if (ab->Nom[0]=='_')
            if (TR->contient(ab->traits) || (ab->traits)->contient(TR))
                return ab->Nom;
        ab=ab->suivant;
    }

    return NULL;    
}

/*-------------------------------------------------------------------------------------*/
/*Cette classe est conçue pour enregistrer dans une liste, les structures lues dans les
fichiers textes...*/ 

  ListeDefaut::ListeDefaut(Vecteur* v,Traits* tr,int i,int j, int k) 
    {valeur=v;un=i;deux=j;reponse=k;courant=this;suivant=NULL;non=NULL;
     if (tr) {contexte=new Vecteur(tr->valeur);
	      if (tr->negation) non=new Vecteur(tr->negation);}
	else contexte=NULL;
}
 
void ListeDefaut::insere(Vecteur* v,Traits* tr,int i,int j,int k) {
    ListeDefaut* l=new ListeDefaut(v,tr,i,j,k);
    courant->suivant=l;
    courant=l;
}
    
 
 
char GlobalParseur::RechercheDefaut(Vecteur* v) {
    ListeDefaut* ld=Defauts;
  
    while (ld && !v->egal(ld->valeur)) 
        ld=ld->suivant;
    if (!ld) 
        return NON;
    return OUI;
}

char GlobalParseur::MetDefaut(Vecteur* v,Vecteur* valeur) {
    ListeDefaut* ld=Defauts;
  
    while (ld) {
        if (v->contient(ld->valeur) && (!ld->contexte || valeur->contient(ld->contexte)) 
            && (!ld->non || !valeur->contient(ld->non)) && 
            valeur->Test(ld->un) && valeur->Test(ld->deux)) {
            valeur->etnon(ld->valeur);valeur->un(this,ld->reponse);		  
            return OUI;
        }
        ld=ld->suivant;
    }
    return NON;
}
//-------------------------------------------------------------------------- 

char majlemmesurface(GlobalParseur* parseur,Traits* RegTraits,Noeud* noeud,char type) {
    string chaine;
	char* inter;
	if (RegTraits->lemme != NULL) {            
		chaine="";
		//Remplacement de la chaine
		if (RegTraits->maniplemme==ECRASE && type==ECRIT) {
			//On recupere notre chaine
			RegTraits->lemme->mot(chaine);
			if (noeud->fils->lemme==NULL || 
				noeud->source->voc==NULL || 
				chaine[0]==0)
				return NON;

			//On remplace alors le lemme par la chaine contenue dans la variable
			int posmot=noeud->source->choisie;
			inter=noeud->source->voc->lemme[posmot];
			libere(inter);
			inter=(char*)DupliqueChaine(USTR(chaine));
			noeud->source->voc->lemme.affecte(posmot,inter);
			noeud->fils->lemme=inter;
		}
		else {
			noeud->Lemme(chaine);
			if (!RegTraits->lemme->recherche(STR(chaine))) {            
				if (RegTraits->maniplemme!=NEG)
					return NON;    
			}
			else {
				if (RegTraits->maniplemme==NEG)
					return NON;    
			}
		}
	}
	if (RegTraits->surface != NULL) {
		chaine="";
		if (RegTraits->manipsurface==ECRASE && type==ECRIT) {
			RegTraits->surface->mot(chaine);
			if (noeud->fils->surface==NULL || 
				noeud->source->voc==NULL || 
				chaine[0]==0)
				return NON;

			//On remplace alors la forme de surface par la chaine contenue dans la variable                
			inter=noeud->source->voc->surface;
			libere(inter);
			inter=(char*)DupliqueChaine(STR(chaine));
			noeud->source->voc->surface=inter;
			noeud->fils->surface=inter;
		}
		else {  
			noeud->Surface(chaine);
			if (!RegTraits->surface->recherche(STR(chaine))) {            
				if (RegTraits->manipsurface!=NEG)
					return NON;    
			}
			else {
				if (RegTraits->manipsurface==NEG)
					return NON;    
			}
		}
	}

    VECTA<Noeud*> vnoeuds;
    char erreur;        
    if (RegTraits->varlemme!=NULL) {
        erreur=NON;
        string bufflemme;
        RegTraits->varlemme->TraitementFormuleString(parseur,vnoeuds,erreur,bufflemme);
        if (erreur==OUI)
            return NON;
        
        if (type==ECRIT && RegTraits->maniplemme==VARECRASE) {
            if (noeud->fils->lemme==NULL || 
                noeud->source->voc==NULL || 
                bufflemme.size()==0)
                return NON;
            
            //On remplace alors le lemme par la chaine contenue dans la variable
            int posmot=noeud->source->choisie;
            inter=noeud->source->voc->lemme[posmot];
            libere(inter);
            inter=(char*)DupliqueChaine(bufflemme.c_str());
            noeud->source->voc->lemme.affecte(posmot,inter);
            noeud->fils->lemme=inter;
        }
        else {
            if (bufflemme.size()==0) {
                if (RegTraits->maniplemme!=VARNEG)
                    return NON;
            }
            else {
                if (RegTraits->maniplemme!=VARECRASE) {
                    chaine="";
                    noeud->Lemme(chaine);
                    
                    if (bufflemme==chaine) {
                        if (RegTraits->maniplemme==VARNEG)
                            return NON;
                    }
                    else {
                        if (RegTraits->maniplemme==VARCOMPARE)
                            return NON;
                    }
                }
            }
        }
    }
    
    if (RegTraits->varsurface!=NULL) {
        erreur=NON;
        string buffsurface;
        RegTraits->varsurface->TraitementFormuleString(parseur,vnoeuds,erreur,buffsurface);
        if (erreur==OUI)
            return NON;
        
        if (type==ECRIT && RegTraits->manipsurface==VARECRASE) {
            if (noeud->fils->surface==NULL || 
                noeud->source->voc==NULL || 
                buffsurface.size()==0)
                return NON;
            
            //On remplace alors la forme de surface par la chaine contenue dans la variable                
            inter=noeud->source->voc->surface;
            libere(inter);
            inter=(char*)DupliqueChaine(buffsurface.c_str());
            noeud->source->voc->surface=inter;
            noeud->fils->surface=inter;
        }
        else {
            if (buffsurface.size()==0) {
                if (RegTraits->manipsurface!=VARNEG)
                    return NON;
            }
            else {             
                if (RegTraits->manipsurface!=VARECRASE) {
                    chaine="";
                    noeud->Surface(chaine);
                    
                    if (buffsurface==chaine) {
                        if (RegTraits->manipsurface==VARNEG)
                            return NON;
                    }
                    else {
                        if (RegTraits->manipsurface==VARCOMPARE)
                            return NON;
                    }
                }
            }
        }
    }
    return OUI;
}

char ITR(GlobalParseur* parseur,Traits* RegTraits,Traits* tr,char type,Noeud* noeud) {

    TVecteur* nval=(tr->valeur)->vecteur;
    register short i;
    int iTailleVect=parseur->iTailleVect;
    
    if (RegTraits==NULL)
        return OUI;

	if (tr==NULL)
		return NON;

    if (RegTraits->lpresent!=NULL) {
        char erreur=OUI;
        TVecteur* rlp=(RegTraits->lpresent)->vecteur;
        
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
    
    nval=(tr->valeur)->vecteur;
    
    TVecteur* nneg= NULL;
    if (tr->negation!=NULL)
        nneg=(tr->negation)->vecteur;

    TVecteur* rneg=NULL;
    if (RegTraits->negation!=NULL)
        rneg=(RegTraits->negation)->vecteur;
    
    TVecteur* rval=(RegTraits->valeur)->vecteur;
    TVecteur* rfil=(RegTraits->filtre)->vecteur;
    TVecteur* rpres=NULL;
    if (RegTraits->present!=NULL)
        rpres=(RegTraits->present)->vecteur;
    TVecteur* nfil= (tr->filtre)->vecteur;


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
    
    if (noeud!=NULL)
        if (majlemmesurface(parseur,RegTraits,noeud,type)==NON)
            return NON;
    
    actionOntologie action;
	static actionOntologie actions[]={
            ONTOLOGIE_COMPARAISON,
            ONTOLOGIE_NEGATION,
            ONTOLOGIE_COMPARAISON_STRICTE,
            ONTOLOGIE_NEGATION_STRICTE
    };
    //Test sur l'ontologie
    if (RegTraits->onto) {                            
        VECTA<ontologie*>& ronto = *RegTraits->onto;               
        
        for (i=0;i<4;i++) {
            action=actions[i];
            if (ronto[action]==NULL)
                continue;

            char neg=NON;
            if (action==ONTOLOGIE_NEGATION ||
                action==ONTOLOGIE_NEGATION_STRICTE)
                neg=OUI;
            
            char trouve=NON;
            //si aucun trait d'ontologie locaux, alors a moins d'etre dans une negation
            //il s'agit d'une erreur
            if (tr->onto==NULL)
                return neg;
            else {
                VECTA<ontologie*>& loconto=*tr->onto;                
                char test;                
                switch(action) {
                case ONTOLOGIE_NEGATION:
                case ONTOLOGIE_COMPARAISON:                
                    test=loconto[ONTOLOGIE_COMPARAISON]->present(ronto[action]);
                    break;
                case ONTOLOGIE_NEGATION_STRICTE:
                case ONTOLOGIE_COMPARAISON_STRICTE:                
                    test=loconto[ONTOLOGIE_COMPARAISON]->egal(ronto[action]);
                    break;
                }
                if (test==neg)
                    return NON;
            }
        }
    }

    //vregle==OUI ou vregle==NON, on ecrit les valeurs. Si valeur 2 pas d'ecriture...
    if (type== OUI) {
        if (RegTraits->negation)
            if (tr->negation)
                (tr->negation)->ou(RegTraits->negation);
            else
                tr->negation=new Vecteur(RegTraits->negation);
        parseur->InstancieTraits(tr,RegTraits);
    }

    return OUI;
}

void AffectationTraitsOntologiques(Traits* tr,Traits* RegTraits) {
	static actionOntologie actions[]={ONTOLOGIE_AFFECTATION,ONTOLOGIE_DESTRUCTION,ONTOLOGIE_LIBRE,ONTOLOGIE_BLOQUEE};
    actionOntologie action;

    //mise a jour de l'ontologie
    if (RegTraits->onto) {
        if (tr->onto==NULL)
            tr->onto=new VECTA<ontologie*>;
        VECTA<ontologie*>& ronto = *RegTraits->onto;
        VECTA<ontologie*>& loconto=*tr->onto;

        for (int i=0;i<4;i++) {
            action=actions[i];
            //Seul les objets introduit dans la structure de traits avec un "=" sont recopies
            //1 est une creation de l'ontologie
            //2 est une destruction de l'ontologie
            if (ronto[action]==NULL)
                continue;

			switch(action) {
				case ONTOLOGIE_LIBRE:
					if (loconto[ONTOLOGIE_LIBRE]==NULL)
						loconto.affecte(ONTOLOGIE_LIBRE,new ontologie(ronto[action]));
					else
						loconto[ONTOLOGIE_LIBRE]->ou(ronto[action]);
					break;
				case ONTOLOGIE_BLOQUEE:
					if (loconto[ONTOLOGIE_LIBRE]==NULL)
						continue;
					if (loconto[ONTOLOGIE_LIBRE]->present(ronto[action]))
						loconto[ONTOLOGIE_LIBRE]->etnon(ronto[action]);
					if (loconto[ONTOLOGIE_LIBRE]->element.dernier==0) {
						delete loconto[ONTOLOGIE_LIBRE];
						loconto.affecte(ONTOLOGIE_LIBRE,NULL);
					}
					break;
				case ONTOLOGIE_DESTRUCTION:
					if (loconto[ONTOLOGIE_COMPARAISON]==NULL)
						continue;
					if (loconto[ONTOLOGIE_COMPARAISON]->present(ronto[action]))
						loconto[ONTOLOGIE_COMPARAISON]->etnon(ronto[action]);
					if (loconto[ONTOLOGIE_COMPARAISON]->element.dernier==0) {
						delete loconto[ONTOLOGIE_COMPARAISON];
						loconto.affecte(ONTOLOGIE_COMPARAISON,NULL);
					}
					break;
				case ONTOLOGIE_AFFECTATION:
					if (loconto[ONTOLOGIE_COMPARAISON]==NULL)
						loconto.affecte(ONTOLOGIE_COMPARAISON,new ontologie(ronto[action]));                   
					else {
						//dans le cas d'une affectation de valeur,
						//il est important aussi d'eviter de generer les 
						//hyponymes ou hyperonymes
						//est-ce que on a @animal@mammifere@chien et est-ce que on veut rajouter
						//@animal@mammifere                    
						//est-ce que on a @animal@mammifere et est-ce que on veut rajouter
						//@animal@mammifere@chien
						if (!ronto[action]->present(loconto[ONTOLOGIE_COMPARAISON]))
							loconto[ONTOLOGIE_COMPARAISON]->ou(ronto[action]);
					}
			}
		}
		if (tr->onto->dernier==0) {
			delete tr->onto;
			tr->onto=NULL;
		}
	}
}

void GlobalParseur::InstancieTraits(Traits* tr,Traits* RegTraits) {

    if (RegTraits==NULL || tr==NULL)
        return;

    //Effacement de traits...
    if (RegTraits->efface!=NULL) {
        (tr->valeur)->etnon(RegTraits->efface);
        (tr->filtre)->etnon(RegTraits->efface);
    }

	if (RegTraits->permanent!=NULL) {
		if (tr->permanent!=NULL)
			tr->permanent->ou(RegTraits->permanent);
		else
			tr->permanent=new Vecteur(RegTraits->permanent);
	}

	if (tr->permanent!=NULL) {
		tr->valeur->ou(tr->permanent);
		tr->filtre->ou(tr->permanent);
	}

    (tr->valeur)->ou(RegTraits->valeur);
    (tr->filtre)->ou(RegTraits->filtre);

    //Tout d'abord, les traits qui doivent etre bloques sont retires de tr->libre
    if (RegTraits->bloquage && tr->libre) 
        (tr->libre)->etnon(RegTraits->bloquage);

    //Les traits libres des regles ont le pas sur les traits bloques du noeud...
    //Puis si des traits doivent devenir libre, on les place dans la zone libre
    if (RegTraits->libre) {
        if (tr->libre) 
            (tr->libre)->ou(RegTraits->libre);
        else 
            tr->libre=new Vecteur(RegTraits->libre);   
    }

	AffectationTraitsOntologiques(tr,RegTraits);
}



char GlobalParseur::FusionneTraits(Traits* recoit, Traits* nouveau, Vecteur& filtreur) {

    TVecteur* filrec=(recoit->filtre)->vecteur;
    TVecteur* filnouv = (nouveau->filtre)->vecteur;
    TVecteur* rec=(recoit->valeur)->vecteur;
    TVecteur* nouv=(nouveau->valeur)->vecteur;
    TVecteur fil;
    char modif=NON;

    int i;

    for (i=0;i<iTailleVect;i++) {

        fil = *filrec & *filnouv;
        //Nous determinons si les traits sont compatibles
        if ((*rec & fil) != (*nouv & fil)) {
            //erreur recoit les traits pour lesquels il y a une erreur
            TVecteur erreur= (*rec & fil) ^ (*nouv & fil);
            //On stocke ces traits dans le vecteur filtreur
            filtreur.vecteur[i]|=erreur;
            modif=OUI;
        }
        
        *rec|=*nouv;
        *filrec|=*filnouv;
		//Les traits permanents ne peuvent etre detruit
		if (recoit->permanent!=NULL) {
			*rec|=recoit->permanent->vecteur[i];
			*filrec|=recoit->permanent->vecteur[i];
		}
        filrec++;
        filnouv++;
        rec++;
        nouv++;
    }

    if (nouveau->libre!=NULL) {
        if (recoit->libre==NULL)
            recoit->libre=new Vecteur(iTailleVect);
        recoit->libre->ou(nouveau->libre);
    }

	AffectationTraitsOntologiques(recoit,nouveau);
        
    return modif;
}

void GlobalParseur::NettoieTraitsIncoherent(Traits* tous, Vecteur& filtreur) {
    VECTA<int> liste;

    filtreur.Liste(liste);

    Vecteur retrait(iTailleVect);
    for (int i=0;i<liste.dernier;i++)
        retrait.ou(TableAttributs[liste[i]]->valeur);

    (tous->valeur)->etnon(&retrait);
    (tous->filtre)->etnon(&retrait);
    if (tous->libre!=NULL)
        (tous->libre)->etnon(&retrait);
}

    
//------------------------------------------------------------------------------------
char GlobalParseur::XipFeatureTraits(Traits* tr,char* attribut,char* valeur) {

    //s'il s'agit d'une valeur ontologique
    if (attribut[0]=='@') {
        ParseOntologie(attribut,tr);
        return OUI;
    }
        
    //recherche du nom de l'attribut
    trait* vattribut=ArbTraits->recherche(attribut);

    if (!vattribut) {
        if (AFFICHE_ERREUR==OUI) {
            sprintf_s(Chaine_traits,3000,"Feature %s:%s",CHAINES[173],attribut,valeur);
            if (TRACE)
                trace<<Chaine_traits<<Endl;
            insere_erreur(Chaine_traits);
            *flot_erreur<<Chaine_traits<<Endl;
        }
        
        return NON;
    }
    
    trait* vvaleur=RetrouveValeurTrait(vattribut,valeur,NON);
    if (vvaleur!=NULL) {
		tr->inittrait(vattribut,vvaleur);
        return OUI;
    }
    return NON;
}

char GlobalParseur::TestXipFeatureTraits(Traits* tr,char* attribut,char* valeur) {

    //s'il s'agit d'une valeur ontologique
    if (attribut[0]=='@') {
        ParseOntologie(attribut,tr);
        return OUI;
    }
        
    //recherche du nom de l'attribut
    trait* vattribut=ArbTraits->recherche(attribut);

    if (!vattribut) {
        if (AFFICHE_ERREUR==OUI) {
            sprintf_s(Chaine_traits,3000,"Feature %s:%s",CHAINES[173],attribut,valeur);
            if (TRACE)
                trace<<Chaine_traits<<Endl;
            insere_erreur(Chaine_traits);
            *flot_erreur<<Chaine_traits<<Endl;
        }
        
        return NON;
    }
    
    trait* vvaleur=RetrouveValeurTrait(vattribut,valeur,NON);
	if (vvaleur!=NULL && tr->valeur->contient(vvaleur->valeur))
		return OUI;
    return NON;
}

