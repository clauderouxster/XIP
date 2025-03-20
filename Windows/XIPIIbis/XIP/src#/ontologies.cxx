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
   Module     : ontologie.cxx
   Date       :
   Purpose    : ontology handling
   Programmer : Claude ROUX
   Reviewer   : 

*/

#include "parsbin.h"

/*----------------------------------------------------------------------------------------*/
#define Rang(val) val >> gBitVectDivise;
#define BitPos(val,pos) Un << (val - (pos << gBitVectDivise)); 
void strcatchr(char* tmp,char c);
/*----------------------------------------------------------------------------------------*/

granule::granule(int val) {
    rang = Rang(val);
    mot =  BitPos(val,rang);
}

granule::granule(int rg,TVecteur val) {
    rang = rg;
    mot =  val;
}


granule::granule(granule* g) {
    if (g==NULL) {
        mot=0;
        rang=0;
    }
    else {
        mot=g->mot;
        rang=g->rang;
    }    
}

granule::granule(granule& g) {
    mot=g.mot;
    rang=g.rang;
}

//---------------------------------------------------------------------------------------

//Vérification de la présence d'un concept dans un autre

char ontologie::present(ontologie* v) {
    
    int pos=0;
    int i=0;
    
    while (i<v->element.dernier) {
        
        
        if (pos >= element.dernier)
            //la liste des elements courant est epuisee, pas celle de v
            //c'est un cas d'erreur
            return NON;
        
        //3 cas
        //a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
        //les granules sont ordonnes...
        if (v->element[i]->rang < element[pos]->rang)
            return NON;
        
        //b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
        if (v->element[i]->rang > element[pos]->rang)
            pos++;
        else {
            //c) c'est le meme element, on verifie si les bits sont compatibles:
            if ((element[pos]->mot & v->element[i]->mot) != v->element[i]->mot)
                return NON;
            //on passe aux elements suivants
            pos++;
            i++;
        }
    }
    return OUI;
}

char ontologie::present(int index) {
	int pos=0;
	short rang = Rang(index);
	TVecteur mot =  BitPos(index,rang);

	while (pos<element.dernier && element[pos]->rang<rang) pos++;
	if (pos==element.dernier || element[pos]->rang>rang)
		return NON;

	if ((element[pos]->mot & mot) == mot)            
		return OUI;
	return NON;
}

char ontologie::present(ontologie& v) {

	int pos=0;
	int i=0;

	while (i<v.element.dernier) {


		if (pos >= element.dernier)
			//la liste des elements courant est epuisee, pas celle de v
			//c'est un cas d'erreur
			return NON;

		//3 cas
		//a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les granules sont ordonnes...
		if (v.element[i]->rang < element[pos]->rang)
			return NON;

		//b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
		if (v.element[i]->rang > element[pos]->rang)
			pos++;
		else {
			//c) c'est le meme element, on verifie si les bits sont compatibles:
			if ((element[pos]->mot & v.element[i]->mot) != v.element[i]->mot)
				return NON;
			//on passe aux elements suivants
			pos++;
			i++;
		}
	}
	return OUI;
}
/*----------------------------------------------------------------------------------------*/
void ontologie::etnon(ontologie& v) {
    int pos=0;
    int i=0;
    granule* g;

    while (i<v.element.dernier) {

      if (pos >= element.dernier)
	//la liste des elements courant est epuisee, pas celle de v
	//il ne peut plus y avoir d'autres elements communs
	break;

      //3 cas
      //a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
      //les granules sont ordonnes...
      if (v.element[i]->rang < element[pos]->rang)
	//on passe a l'element suivant
	i++;
      else
	//b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
	if (v.element[i]->rang > element[pos]->rang)
	  pos++;
	else {
	  //c) c'est le meme element, on conserve les bits...
	  //on se prepare une nouvelle granule
	  element[pos]->mot &=~(v.element[i]->mot);
	  
	  if (element[pos]->mot==0) {
	    g=element.retireElement(pos);
	    delete g;
	  }
	  else
	    //on passe aux elements suivants
	    pos++;
	  i++;
	}
    }
}
/*----------------------------------------------------------------------------------------*/
void ontologie::etnon(ontologie* v) {
    int pos=0;
    int i=0;
    granule* g;

    while (i<v->element.dernier) {

      if (pos >= element.dernier)
	//la liste des elements courant est epuisee, pas celle de v
	//il ne peut plus y avoir d'autres elements communs
	break;

      //3 cas
      //a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
      //les granules sont ordonnes...
      if (v->element[i]->rang < element[pos]->rang)
	//on passe a l'element suivant
	i++;
      else
	//b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
	if (v->element[i]->rang > element[pos]->rang)
	  pos++;
	else {
	  //c) c'est le meme element, on conserve les bits...
	  //on se prepare une nouvelle granule
	  element[pos]->mot &=~v->element[i]->mot;
	  
	  if (element[pos]->mot==0) {
	    g=element.retireElement(pos);
	    delete g;
	  }
	  else
	    //on passe aux elements suivants
	    pos++;
	  i++;
	}
    }
}

/*----------------------------------------------------------------------------------------*/
void ontologie::ou(granule* g) {
    int pos=0;

    while (pos<element.dernier) {

        //3 cas
        //a) l'element a ajoute est place avant, on l'insere simplement
	if (element[pos]->rang > g->rang) {
            //on ne fait que recopier le pointeur...
            element.insere(pos,new granule(g));
            return;
        }
        else
	  //b) l'element est situe apres, il faut donc avancer d'un cran...
	  if (element[pos]->rang < g->rang)
	    pos++;
	  else {
	    //c) c'est le meme element, on fusionne les bits
	    element[pos]->mot |= g->mot;
	    //Et on detruit le pointeur
	    delete g;
	    return;
	  }
    }
    //sinon on le rajoute en queue
    element.ajoute(g);
}

void ontologie::ou(granule& g) {
    int pos=0;

    while (pos<element.dernier) {

        //3 cas
        //a) l'element a ajoute est place avant, on l'insere simplement
	if (element[pos]->rang > g.rang) {
            //on ne fait que recopier le pointeur...
            element.insere(pos,new granule(g));
            return;
        }
        else
	  //b) l'element est situe apres, il faut donc avancer d'un cran...
	  if (element[pos]->rang < g.rang)
	    pos++;
	  else {
	    //c) c'est le meme element, on fusionne les bits
              element[pos]->mot |= g.mot;
              //Et on detruit le pointeur
              return;
	  }
    }

    //sinon on le rajoute en queue
    element.ajoute(new granule(g));
}

void ontologie::ou(int index) {
    int pos=0;
    short rang = Rang(index);
    TVecteur mot =  BitPos(index,rang);
    
    while (pos<element.dernier) {
        
        //3 cas
        //a) l'element a ajoute est place avant, on l'insere simplement
        if (element[pos]->rang > rang) {
            //on ne fait que recopier le pointeur...
            element.insere(pos,new granule(rang,mot));
            return;
        }
    
        //b) l'element est situe apres, il faut donc avancer d'un cran...
        if (element[pos]->rang < rang)
            pos++;
        else {
            //c) c'est le meme element, on fusionne les bits
            element[pos]->mot |= mot;
            //Et on detruit le pointeur
            return;
        }
    }
    
    //sinon on le rajoute en queue
    element.ajoute(new granule(rang,mot));
}


void ontologie::ou(ontologie& ajout) {
    int pos=0;
    int i=0;

    while (i<ajout.element.dernier) {

        if (pos >= element.dernier) {
            //dans le cas ou l'on a epuise la liste des elements courants
            //on rajoute simplement le reste a la suite...
            for (;i<ajout.element.dernier;i++)
                element.ajoute(new granule(ajout.element[i]));
            break;
        }

        //3 cas
        //a) l'element a ajoute est place avant, on l'insere simplement
	if (element[pos]->rang > ajout.element[i]->rang) {
            //on ne fait que recopier le pointeur...
            element.insere(pos,new granule(ajout.element[i]));                   
            //et on passe a l'element suivant
            i++;
        }
        else 
	  //b) l'element est situe apres, il faut donc avancer d'un cran...
	  if (element[pos]->rang < ajout.element[i]->rang)
	    pos++;
	  else {
	    //c) c'est le meme element, on fusionne les bits
	    element[pos]->mot |= ajout.element[i]->mot;
	    //on passe aux elements suivants
	    pos++;
	    i++;
	  }
    }
}

/*----------------------------------------------------------------------------------------*/
void ontologie::ou(ontologie* ajout) {
    int pos=0;
    int i=0;

    while (i<ajout->element.dernier) {

        if (pos >= element.dernier) {
            //dans le cas ou l'on a epuise la liste des elements courants
            //on rajoute simplement le reste a la suite...
            for (;i<ajout->element.dernier;i++)
                element.ajoute(new granule(ajout->element[i]));
            break;
        }

        //3 cas
        //a) l'element a ajoute est place avant, on l'insere simplement
	if (element[pos]->rang > ajout->element[i]->rang) {
            //on ne fait que recopier le pointeur...
            element.insere(pos,new granule(ajout->element[i]));                   
            //et on passe a l'element suivant
            i++;
        }
        else
            //b) l'element est situe apres, il faut donc avancer d'un cran...
            if (element[pos]->rang < ajout->element[i]->rang)
                pos++;
            else {
	      //c) c'est le meme element, on fusionne les bits
	      element[pos]->mot |= ajout->element[i]->mot;
	      //on passe aux elements suivants
	      pos++;
	      i++;
	    }
    }
}


/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
void ontologie::et(ontologie* v1,ontologie* v2) {
    
    int pos=0;
    int i=0;

    while (i<v2->element.dernier) {

      if (pos >= v1->element.dernier)
	//la liste des elements courant est epuisee, pas celle de v
	//il ne peut plus y avoir d'autres elements communs
	break;

      //3 cas
      //a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
      //les granules sont ordonnes...
      if (v2->element[i]->rang < v1->element[pos]->rang)
	//on passe a l'element suivant
	i++;
      else
	//b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
	if (v2->element[i]->rang > v1->element[pos]->rang)
	  pos++;
	else {
	  //c) c'est le meme element, on conserve les bits...
	  //on se prepare une nouvelle granule
	  granule* g= new granule();
	  g->mot= v1->element[pos]->mot & v2->element[i]->mot;
	  g->rang=v1->element[pos]->rang;
	  
	  if (g->mot==0)
	    delete g;
	  else
	    element.ajoute(g);
	  //on passe aux elements suivants
	  pos++;
	  i++;
	}
    }
}
/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
void ontologie::et(ontologie* v) {
    
    int pos=0;
    int i=0;
    granule* g;
    
    while (i<v->element.dernier) {
        
        if (pos >= element.dernier)
            //la liste des elements courant est epuisee, pas celle de v
            //il ne peut plus y avoir d'autres elements communs
            break;
        
        //3 cas
        //a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
        //les granules sont ordonnes...
        if (v->element[i]->rang < element[pos]->rang)
            //on passe a l'element suivant
            i++;
        else
            //b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
            if (v->element[i]->rang > element[pos]->rang) {
                //cet element en pos n'existe pas dans v 
                g=element.retireElement(pos);
                delete g;
            }
            else {
                //c) c'est le meme element, on conserve les bits...
                //on se prepare une nouvelle granule
                element[pos]->mot &= v->element[i]->mot;
                
                if (element[pos]->mot==0) {
                    g=element.retireElement(pos);
                    delete g;
                }
                else
                    //on passe aux elements suivants
                    pos++;
                i++;
            }
    }
    for (i=element.dernier-1;i>=pos;i--) {
        g=element.retire();
        delete g;
    }      
}
/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
int ontologie::et(short r, TVecteur m,int start) {
    
    int pos=start;
    granule* g;
    


    while(pos<element.dernier) {
        //si cet element n'est pas dans la liste on repart        
        if (r>element[pos]->rang) {
            g=element.retireElement(pos);
            delete g;                    
            continue;
        }

        if (r==element[pos]->rang) {
            element[pos]->mot &= m;
            
            if (element[pos]->mot==0) {
                g=element.retireElement(pos);
                delete g;
                return start;
            }        
            return start+1;
        }
        return start;
                
    }
    return start;    
}

/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
void ontologie::et(ontologie& v) {
    
    int pos=0;
    int i=0;
    granule* g;
    int premier=-1;

    while (i<v.element.dernier) {
        
        if (pos >= element.dernier)
            //la liste des elements courant est epuisee, pas celle de v
            //il ne peut plus y avoir d'autres elements communs
            break;
        
        //3 cas
        //a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
        //les granules sont ordonnes...
        if (v.element[i]->rang < element[pos]->rang)
            //on passe a l'element suivant
            i++;            
        else
            //b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
            if (v.element[i]->rang > element[pos]->rang) {
                //cet element en pos n'existe pas dans v 
                g=element.retireElement(pos);
                delete g;
            }
            else {
                //c) c'est le meme element, on conserve les bits...
                //on se prepare une nouvelle granule
                element[pos]->mot &= v.element[i]->mot;
                
                if (element[pos]->mot==0) {
                    g=element.retireElement(pos);
                    delete g;
                }
                else
                    //on passe aux elements suivants
                    pos++;
                i++;
            }
    }
    //On retire les elements en queue
    for (i=element.dernier-1;i>=pos;i--) {
        g=element.retire();
        delete g;
    }      
}
/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
void ontologie::et(ontologie& v1,ontologie& v2) {
    
    int pos=0;
    int i=0;
    
    while (i<v2.element.dernier) {
        
        if (pos >= v1.element.dernier)
            //la liste des elements courant est epuisee, pas celle de v
            //il ne peut plus y avoir d'autres elements communs
            break;
        
        //3 cas
        //a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
        //les granules sont ordonnes...
        if (v2.element[i]->rang < v1.element[pos]->rang)
            //on passe a l'element suivant
            i++;
        else
            //b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
            if (v2.element[i]->rang > v1.element[pos]->rang)
                pos++;
            else {
                //c) c'est le meme element, on conserve les bits...
                //on se prepare une nouvelle granule
                granule* g= new granule();
                g->mot= v1.element[pos]->mot & v2.element[i]->mot;
                g->rang=v1.element[pos]->rang;
                
                if (g->mot==0)
                    delete g;
                else
                    element.ajoute(g);
                //on passe aux elements suivants
                pos++;
                i++;
            }
    }
}

/*----------------------------------------------------------------------------------------*/ 

ontologie::ontologie() {
}

ontologie::ontologie(granule* g) {    
    element.ajoute(g);    
}


//Duplication d'une ontologie...
ontologie::ontologie(ontologie* v) {
    if (v!=NULL) {        
        for (int i=0;i<v->element.dernier;i++)
            element.ajoute(new granule(v->element[i]));
    }
}

//Duplication d'une ontologie...
ontologie::ontologie(ontologie& v) {
    for (int i=0;i<v.element.dernier;i++)
        element.ajoute(new granule(v.element[i]));
}

ontologie::~ontologie() {
    element.nettoie();
}


void ontologie::table(VECTA<int>& liste) {

    //We build in reverse order
    for (int i=element.dernier-1;i>=0;i--) {
        STVecteur word=(STVecteur)element[i]->mot;
        int base = element[i]->rang*gBitVect;
        for (int k=gBitVect-1;k>=0;k--) {
            //No more bits to seek, we stop
            if (word==0)
                break;
            //the sign bit is the left most bit
            if (word < 0)
                liste.ajoute(base+k);
            //we then go to the next element...shifting bits by 1 to the left
            word <<= 1;
        }
    }
}

int ontologie::compte() {
    int counter=0;
    //We build in reverse order
    for (int i=element.dernier-1;i>=0;i--) {
        STVecteur word=(STVecteur)element[i]->mot;
        int base = element[i]->rang*gBitVect;
        for (int k=gBitVect-1;k>=0;k--) {
            //No more bits to seek, we stop
            if (word==0)
                break;
            //the sign bit is the left most bit
            if (word < 0)
                counter++;
            //we then go to the next element...shifting bits by 1 to the left
            word <<= 1;
        }
    }
    return counter;
}

//We retrait the bit b
void ontologie::retrait(int b) {
    int rank= Rang(b);
    TVecteur word=  BitPos(b,rank);

    //We look for the first element of rang=rank
    for (int i=0;i<element.dernier;i++) {
        if (element[i]->rang==rank) {
            //We XOR the bit
            element[i]->mot^=word;
            if (element[i]->mot==0) {
                //the element does not contain any bit anymore
                //we retrait it
                granule* g=element.retireElement(i);
                delete g;
            }
            return;
        }
        
        //The elements are ordered...
        if (rank > element[i]->rang)
            return;
    }    
}

char ontologie::egal(ontologie& v) {
    if (element.dernier!=v.element.dernier)
        return NON;
    for (int i=0;i<element.dernier;i++) {
        if (element[i]->rang!=v.element[i]->rang)
            return NON;
        if (element[i]->mot!=v.element[i]->mot)
            return NON;
    }
    return OUI;
}

char ontologie::egal(ontologie* v) {
    if (element.dernier!=v->element.dernier)
        return NON;
    for (int i=0;i<element.dernier;i++) {
        if (element[i]->rang!=v->element[i]->rang)
            return NON;
        if (element[i]->mot!=v->element[i]->mot)
            return NON;
    }
    return OUI;
}


hierarchie::hierarchie(unsigned char* s,int i) {
    semme=DupliqueChaine(s);
    index=i;
    onto.element.ajoute(new granule(i));
    tete=NON;
}


hierarchie::~hierarchie() {
    libere(semme);
}


/*Une ontologie est de la forme:

  racine { mot1{mot2,mot,..,mot3}, mot2{m1,...,m2} }
*/

void GlobalParseur::LitOntologies(istream& fichier,Hierarchie& unehierarchie,hierarchie* racine,char& retour) {
    char tampon[100];    
    char k=0;
    hierarchie* hie;
    etat* e;    

    while (k!='}' && !fichier.eof()) {
        //on lit d'abord la racine
        k=lit(fichier,tampon);
        hie=NULL;
        
        //tout d'abord, on verifie qu'on ne la pas deja rencontre
        if (tampon[0]!=0) {
            ontologie tracedescendant;
            e=unehierarchie.ontologies.recherche(tampon);
            if (e==NULL) {
                //il est nouveau. on le cree                
                hie=new hierarchie((uchar*)tampon,unehierarchie.indexontologie++);   
                unehierarchie.affecte(hie->index,hie);
                if (retour==-1)
                    hie->tete=OUI;

                if (racine!=NULL) //on fusionne les bits de la racine
                    hie->onto.ou(racine->onto);
                //Puis on construit les descendants
                hie->descendants.ou(hie->index);
                unehierarchie.ontologies.parsebrute((uchar*)tampon,hie);
            }
            else {//Il existe deja, par consequent, il derive de plusieurs
                //ancetre, on lui rajoute un nouveau bloc d'ancetre
                hie=(hierarchie*)e->valeur;                                
                hie->onto.ou(racine->onto);
                //on garde une trace des descendants de cet element
                tracedescendant.ou(hie->descendants);
            }

            if (k=='{')
                LitOntologies(fichier,unehierarchie,hie,k);                    
            
            //on garde une trace des fils pour la racine
            if (racine!=NULL && hie!=NULL) {
                racine->fils.ou(hie->index);
                racine->descendants.ou(hie->descendants);
                if (e!=NULL) {
                    //et on retire ceux qui ne proviennent pas de cette analyse
                    racine->descendants.etnon(tracedescendant);
                    //on rajoute quand meme l'element courant (deja present dans l'automate)
                    racine->descendants.ou(hie->index);
                }
            }
        }
        
        if (k!='}' && k!=',')
            erreur(CHAINES[131]);
    }
    retour=lit(fichier,tampon);
    if (fichier.eof())
        retour='}';
}


void constructionXML(VECTA<hierarchie*>& unehierarchie,
                     int tete,
                     int index,
                     VECTA<int>& tables,
                     string& bufint,
                     ostream& os) {

    hierarchie* racine=unehierarchie[tete];

	char trouve=NON;
    for (int j=0;j<tables.dernier;j++) {
        if (tables[j]<0) {
            tables.affecte(j,-1);
            continue;
        }

        hierarchie* courant=unehierarchie[tables[j]];
        //s'il s'agit d'un fils
        if (racine->fils.present(courant->index)) {
            string buf=bufint;
            buf+="@";
            buf+=(char*)courant->semme;
            trouve=OUI;
            int val=tables[j];
            tables.affecte(j,-1);
            constructionXML(unehierarchie,val,j+1,tables,buf,os);
        }
    }
    
    if (trouve==NON)
		os<<"<FEATURE attribute=\""<<(char*)bufint.c_str()<<"\" value=\"+\">"<<endl;    
}


void ontologie::AfficheValeursXML(VECTA<hierarchie*>& unehierarchie,Traits* tr,ostream& os) {

    static actionOntologie actions[]={ONTOLOGIE_COMPARAISON,ONTOLOGIE_COMPARAISON_STRICTE,ONTOLOGIE_AFFECTATION};    
    if (tr==NULL || tr->onto==NULL)
        return;
        
    actionOntologie action;
    int i;
    VECTA<int> unetable;
    VECTA<int> tetes;
    VECTA<int> inter;
    VECTA<ontologie*>& tronto=*tr->onto;
    
    for (int j=0;j<3;j++) {
        action=actions[j];
        if (tronto[action]==NULL)
            continue;

        if (present(tronto[action])) {
            unetable.raz();
            //On recupere tous les elements
            table(unetable);
            //On cherche la tete de hierarchie
            for (i=0;i<unetable.dernier;i++)
                if (unehierarchie[unetable[i]]->tete==OUI) {
                    tetes.ajoute(unetable[i]);                    
                    unetable.affecte(i,-1);
                }


            for (i=0;i<tetes.dernier;i++) {
                hierarchie* racine=unehierarchie[tetes[i]];
                string buf="@";
                buf+=(char*)racine->semme;                
                if (tetes.dernier>1) {
                    inter.affecte(unetable);
                    constructionXML(unehierarchie,tetes[i],0,inter,buf,os);
                }
                else
                    constructionXML(unehierarchie,tetes[i],0,unetable,buf,os);                
            }
        }
    } 
}

#ifdef XIPLIBXML
void constructionNoeudXML(GlobalParseur* parseur,
						  VECTA<hierarchie*>& unehierarchie,
                          int tete,
                          int index,
                          VECTA<int>& tables,
                          string& bufint,
                          xmlNodePtr node) {

    hierarchie* racine=unehierarchie[tete];
    char trouve=NON;
    for (int j=0;j<tables.dernier;j++) {
        if (tables[j]<0) {
            tables.affecte(j,-1);
            continue;
        }

        hierarchie* courant=unehierarchie[tables[j]];
        //s'il s'agit d'un fils
        if (racine->fils.present(courant->index)) {
            string buf=bufint;
            buf+="@";
            buf+=(char*)courant->semme;
            trouve=OUI;
            int val=tables[j];
            tables.affecte(j,-1);
            constructionNoeudXML(parseur,unehierarchie,val,j+1,tables,buf,node);
        }
    }
    
    if (trouve==NON) {
        xmlNodePtr feature=parseur->CreationNoeudXML(node,"FEATURE",NULL);
        parseur->AjouteAttributValeur(feature,"attribute",(char*)bufint.c_str());
        parseur->AjouteAttributValeur(feature,"value","+");        
    }
}

void ontologie::ConstruitXML(GlobalParseur* parseur,VECTA<hierarchie*>& unehierarchie,xmlNodePtr node,Traits* tr) {

    static actionOntologie actions[]={ONTOLOGIE_COMPARAISON,ONTOLOGIE_COMPARAISON_STRICTE,ONTOLOGIE_AFFECTATION};    
    if (tr->onto==NULL)
        return;
        
    actionOntologie action;
    int i;
    VECTA<int> unetable;
    VECTA<int> tetes;
    VECTA<int> inter;
    VECTA<ontologie*>& tronto=*tr->onto;
    
    for (int j=0;j<3;j++) {
        action=actions[j];
        if (tronto[action]==NULL)
            continue;

        if (present(tronto[action])) {
            unetable.raz();
            //On recupere tous les elements
            table(unetable);
            //On cherche la tete de hierarchie
            for (i=0;i<unetable.dernier;i++)
                if (unehierarchie[unetable[i]]->tete==OUI) {
                    tetes.ajoute(unetable[i]);
                    unetable.affecte(i,-1);
                }

            for (i=0;i<tetes.dernier;i++) {
                hierarchie* racine=unehierarchie[tetes[i]];
                string buf="@";
                buf+=(char*)racine->semme;                
                if (tetes.dernier>1) {
                    inter.affecte(unetable);
                    constructionNoeudXML(parseur,unehierarchie,tetes[i],0,inter,buf,node);
                }
                else
                    constructionNoeudXML(parseur,unehierarchie,tetes[i],0,unetable,buf,node);                
            }
        }
    } 
}

#endif


void constructionConceptrecursif(VECTA<hierarchie*>& unehierarchie,
                                 int tete,
                                 int index,
                                 VECTA<int>& tables,
                                 string& bufint,
                                 string& buffer,
								 string sep) {

    hierarchie* racine=unehierarchie[tete];
    char trouve=NON;
    for (int j=0;j<tables.dernier;j++) {
        if (tables[j]<0) {
            tables.affecte(j,-1);
            continue;
        }

        hierarchie* courant=unehierarchie[tables[j]];
        //s'il s'agit d'un fils
        if (racine->fils.present(courant->index)) {
            string buf=bufint;
            buf+="@";
            buf+=(char*)courant->semme;
            trouve=OUI;
            int val=tables[j];
            tables.affecte(j,-1);
            constructionConceptrecursif(unehierarchie,val,j+1,tables,buf,buffer,sep);
        }
    }
    if (trouve==NON) {
        buffer+=sep;
        buffer+=bufint;        
    }
}


void constructionConceptrecursif(VECTA<hierarchie*>& unehierarchie,
                                 int tete,
                                 int index,
                                 VECTA<int>& tables,
                                 string& bufint,
								 vector<string>& reponses) {

    hierarchie* racine=unehierarchie[tete];
	char trouve=NON;
    for (int j=0;j<tables.dernier;j++) {
        if (tables[j]<0) {
            tables.affecte(j,-1);
            continue;
        }

        hierarchie* courant=unehierarchie[tables[j]];
        //s'il s'agit d'un fils
        if (racine->fils.present(courant->index)) {
            string buf=bufint;
            buf+="@";
            buf+=(char*)courant->semme;
            trouve=OUI;
            int val=tables[j];
            tables.affecte(j,-1);
            constructionConceptrecursif(unehierarchie,val,j+1,tables,buf,reponses);
        }
    }
    if (trouve==NON) {
		if (bufint.size()!=0)
			reponses.push_back(bufint);
    }
}


void ontologie::ConcateneValeurs(GlobalParseur* parseur,VECTA<hierarchie*>& unehierarchie,Traits* tr,char* buffer,char* sep) {
    static actionOntologie actions[]={ONTOLOGIE_COMPARAISON,ONTOLOGIE_COMPARAISON_STRICTE,ONTOLOGIE_AFFECTATION};    
    if (tr==NULL || tr->onto==NULL)
        return;
        
    actionOntologie action;
    int i;
    VECTA<int> unetable;
    VECTA<int> tetes;
    VECTA<int> inter;
    VECTA<ontologie*>& tronto=*tr->onto;
    
    for (int j=0;j<3;j++) {
        action=actions[j];
        if (tronto[action]==NULL)
            continue;

        if (present(tronto[action])) {
            unetable.raz();
            //On recupere tous les elements
            table(unetable);
            //On cherche la tete de hierarchie
            for (i=0;i<unetable.dernier;i++)
                if (unehierarchie[unetable[i]]->tete==OUI) {
                    tetes.ajoute(unetable[i]);
                    unetable.affecte(i,-1);
                }


            for (i=0;i<tetes.dernier;i++) {
                hierarchie* racine=unehierarchie[tetes[i]];
				string buf="@";
                buf+=(char*)racine->semme;
				string lebuffer;
                if (tetes.dernier>1) {
                    inter.affecte(unetable);
                    constructionConceptrecursif(unehierarchie,tetes[i],0,inter,buf,lebuffer,sep);
                }
                else
                    constructionConceptrecursif(unehierarchie,tetes[i],0,unetable,buf,lebuffer,sep);
				strcpy(buffer,(char*)lebuffer.c_str());
            }
        }
    }    
}

void ontologie::ConcateneValeurs(GlobalParseur* parseur,VECTA<hierarchie*>& unehierarchie,Traits* tr,string& buffer,char* sep) {
    static actionOntologie actions[]={ONTOLOGIE_COMPARAISON,ONTOLOGIE_COMPARAISON_STRICTE,ONTOLOGIE_AFFECTATION};    
    if (tr==NULL || tr->onto==NULL)
        return;
        
    actionOntologie action;
    int i;
    VECTA<int> unetable;
    VECTA<int> tetes;
    VECTA<int> inter;
    VECTA<ontologie*>& tronto=*tr->onto;
    
    for (int j=0;j<3;j++) {
        action=actions[j];
        if (tronto[action]==NULL)
            continue;

        if (present(tronto[action])) {
            unetable.raz();
            //On recupere tous les elements
            table(unetable);
            //On cherche la tete de hierarchie
            for (i=0;i<unetable.dernier;i++)
                if (unehierarchie[unetable[i]]->tete==OUI) {
                    tetes.ajoute(unetable[i]);
                    unetable.affecte(i,-1);
                }


            
            for (i=0;i<tetes.dernier;i++) {
                hierarchie* racine=unehierarchie[tetes[i]];
                string buf="@";
                buf+=(char*)racine->semme;
                if (tetes.dernier>1) {
                    inter.affecte(unetable);
                    constructionConceptrecursif(unehierarchie,tetes[i],0,inter,buf,buffer,sep);
                }
                else
                    constructionConceptrecursif(unehierarchie,tetes[i],0,unetable,buf,buffer,sep);
            }
        }
    }    
}

void ontologie::ConcateneValeurs(GlobalParseur* parseur,VECTA<hierarchie*>& unehierarchie,Traits* tr,vector<string>& reponses) {
    static actionOntologie actions[]={ONTOLOGIE_COMPARAISON,ONTOLOGIE_COMPARAISON_STRICTE,ONTOLOGIE_AFFECTATION};    
    if (tr==NULL || tr->onto==NULL)
        return;
        
    actionOntologie action;
    int i;
    VECTA<int> unetable;
    VECTA<int> tetes;
    VECTA<int> inter;
    VECTA<ontologie*>& tronto=*tr->onto;
    
    for (int j=0;j<3;j++) {
        action=actions[j];
        if (tronto[action]==NULL)
            continue;

        if (present(tronto[action])) {
            unetable.raz();
            //On recupere tous les elements
            table(unetable);
            //On cherche la tete de hierarchie
            for (i=0;i<unetable.dernier;i++)
                if (unehierarchie[unetable[i]]->tete==OUI) {
                    tetes.ajoute(unetable[i]);
                    unetable.affecte(i,-1);
                }


            for (i=0;i<tetes.dernier;i++) {
                hierarchie* racine=unehierarchie[tetes[i]];
				string buf="@";
                buf+=(char*)racine->semme;
                if (tetes.dernier>1) {
                    inter.affecte(unetable);
                    constructionConceptrecursif(unehierarchie,tetes[i],0,inter,buf,reponses);
                }
                else
                    constructionConceptrecursif(unehierarchie,tetes[i],0,unetable,buf,reponses);
            }
        }
    }    
}

//Une ligne d'ontologie est sous la forme: @rac@c1@c2
void GlobalParseur::ParseOntologie(istream& fichier,Traits* resultat,char& k,char libre) {    
    char tampon[100];
    string err;    
    hierarchie* rhie;
    hierarchie* hie;
    ontologie* valeur=new ontologie;
    actionOntologie action=ONTOLOGIE_COMPARAISON;

    k=lit(fichier,tampon);

    //le premier element sert de racine pour tout le traitement
    etat* e=hierarchies->ontologies.recherche(tampon);
    if (e==NULL || e->valeur==NULL) {
		err=CHAINES[293];
		err+=":";
		err+=tampon;
        erreur(STR(err));
    }

    //On utilise l'index du concept pour initialiser la valeur
    rhie=(hierarchie*)e->valeur;
    //On affecte l'integralite du chemin ontologique que dans le seul cas d'une affectation
    valeur->ou(rhie->index);

    //On n'utilise que l'index du concept
    while (k=='@') {
        k=lit(fichier,tampon);
        e=hierarchies->ontologies.recherche(tampon);
        if (e==NULL || e->valeur==NULL) {
			err=CHAINES[293];
			err+=":";
			err+=tampon;
			erreur(STR(err));
        }

        hie=(hierarchie*)e->valeur;

        //On verifie que c'est un fils valide pour la racine courante
        if (!rhie->fils.present(hie->index))
            erreur(CHAINES[294]);        

        //Tout d'abord on effectue une intersection entre les ancetres du nouvel element
        //et des elements deja trouve
        valeur->et(hie->onto);
        //Puis on rajoute notre element qui a ete certainement elimine par l'operation precedente
        valeur->ou(hie->index);
        rhie=hie;
    }
      
    //Dans le cas d'un test, deux possibilites: le "+" ou le "~"
    //Nous enregistrons cette valeur dans l'index
	if (k==':') {
		k=lit(fichier,tampon);
		switch(k) {
			case '!':
				if (libre==1)
					action=ONTOLOGIE_LIBRE;
				else
					action=ONTOLOGIE_BLOQUEE;
				valeur->ou(rhie->onto);
				k=lit(fichier,tampon);
				break;
			case '~':
				action=ONTOLOGIE_NEGATION;
				k=lit(fichier,tampon);
				break;
			case ':':
				action=ONTOLOGIE_COMPARAISON_STRICTE;
				k=lit(fichier,tampon);
				if (k=='~') {
					action=ONTOLOGIE_NEGATION_STRICTE;
					k=lit(fichier,tampon);
				}
		}
	}
    else {//sinon, il s'agit de la creation d'une ontologie
        if (k=='=') {            
            action=ONTOLOGIE_AFFECTATION;
            k=lit(fichier,tampon);
            if (k=='~') { //il s'agit de retirer une valeur qui a ete instanciee
                action=ONTOLOGIE_DESTRUCTION;
                k=lit(fichier,tampon);
            }
            else //Dans ce cas il faut aussi rajouter tout le chemin ontologique
                valeur->ou(rhie->onto);
        }
    }
    if (resultat->onto==NULL)
        resultat->onto=new VECTA<ontologie*>;

    if (resultat->onto->cell(action)==NULL)
        resultat->onto->affecte(action,valeur);
    else {
        resultat->onto->cell(action)->ou(valeur);
        delete valeur;
    }
}


//Cette methode traduit une structure ontologique en l'intersection de tous ses descendants
void transformedescendants(GlobalParseur* parseur,Hierarchie& unehierarchie, ontologie* ont) {

    if (ont==NULL)
        return;

    VECTA<int> concepts;
    int min=-1,index=0,i,compte;

    ont->table(concepts);
    if (concepts.dernier==0)        
        return;   

    //cette table est construite a l'envers. On va chercher l'element qui a le moins de bits
    //dans ses descendants. Soit l'element le plus profond dans la hierarchie
    for (i=0;i<concepts.dernier;i++) {
		compte=unehierarchie.lahierarchie[concepts[i]]->descendants.compte();        
        if (min==-1 || compte<min) {
            min=compte;
            index=i;
        }
    }

    //On construit notre amorce avec l'element le plus profond dans la table
	ontologie base(unehierarchie.lahierarchie[concepts[index]]->descendants);        
    //Puis on ne garde que les elements qui font vraiment partie du chemin
    for (i=0;i<concepts.dernier;i++) {
        //Puis on intersecte avec tous les elements de la table
        if (i!=index)        
			base.et(unehierarchie.lahierarchie[concepts[i]]->descendants);
    }

    //On remplace alors les elements de ont par ceux de la base
    ont->element.nettoie();
    ont->element.affecte(base.element);
    base.raz();
}

//Une ligne d'ontologie est sous la forme: @rac@c1@c2
void GlobalParseur::ParseOntologie(char* chaine,Traits* resultat) {    
    string err;    
    char* boucle;    
    hierarchie* rhie;
    hierarchie* hie;
    ontologie* valeur=new ontologie;    

    char* pt=strchr(chaine+1,'@');
    if (pt!=NULL)
        *pt=0;

    //le premier element sert de racine pour tout le traitement
    etat* e=hierarchies->ontologies.recherche(chaine+1);
    if (e==NULL || e->valeur==NULL) {
		err=CHAINES[293];
		err+=":";
		err+=chaine+1;
        erreur(STR(err));
    }
    if (pt!=NULL)
        *pt='@';
    //On utilise l'index du concept pour initialiser la valeur
    rhie=(hierarchie*)e->valeur;
    //On affecte l'integralite du chemin ontologique que dans le seul cas d'une affectation
    valeur->ou(rhie->index);
    boucle=pt;
    //On n'utilise que l'index du concept
    while (boucle!=NULL) {        
        pt=strchr(boucle+1,'@');
        if (pt!=NULL)
            *pt=0;
        e=hierarchies->ontologies.recherche(boucle+1);
        if (e==NULL || e->valeur==NULL) {
			err=CHAINES[293];
			err+=":";
			err+=boucle+1;
			erreur(STR(err));
        }

        hie=(hierarchie*)e->valeur;

        //On verifie que c'est un fils valide pour la racine courante
        if (!rhie->fils.present(hie->index))
            erreur(CHAINES[294]);        

        //Tout d'abord on effectue une intersection entre les ancetres du nouvel element
        //et des elements deja trouve
        valeur->et(hie->onto);
        //Puis on rajoute notre element qui a ete certainement elimine par l'operation precedente
        valeur->ou(hie->index);
        rhie=hie;
        if (pt!=NULL)
            *pt='@';
        boucle=pt;
    }
      
    if (resultat->onto==NULL)
        resultat->onto=new VECTA<ontologie*>;

    if (resultat->onto->cell(ONTOLOGIE_COMPARAISON)==NULL)
        resultat->onto->affecte(ONTOLOGIE_COMPARAISON,valeur);
    else {
        resultat->onto->cell(ONTOLOGIE_COMPARAISON)->ou(valeur);
        delete valeur;
    }
}

//Une ligne d'ontologie est sous la forme: @rac@c1@c2
ontologie* GlobalParseur::OntologieTrait(char* chaine) {    
    string err;    
    char* boucle;    
    hierarchie* rhie;
    hierarchie* hie;
    

    char* pt=strchr(chaine+1,'@');
    if (pt!=NULL)
        *pt=0;

    //le premier element sert de racine pour tout le traitement
    etat* e=hierarchies->ontologies.recherche(chaine+1);
    if (e==NULL || e->valeur==NULL)
        return NULL;
    if (pt!=NULL)
        *pt='@';
    //On utilise l'index du concept pour initialiser la valeur
    rhie=(hierarchie*)e->valeur;
    //On affecte l'integralite du chemin ontologique que dans le seul cas d'une affectation
	ontologie* valeur=new ontologie;    
    valeur->ou(rhie->index);
    boucle=pt;
    //On n'utilise que l'index du concept
    while (boucle!=NULL) {        
        pt=strchr(boucle+1,'@');
        if (pt!=NULL)
            *pt=0;
        e=hierarchies->ontologies.recherche(boucle+1);
        if (e==NULL || e->valeur==NULL) {
			err=CHAINES[293];
			err+=":";
			err+=boucle+1;
			erreur(STR(err));
        }

        hie=(hierarchie*)e->valeur;

        //On verifie que c'est un fils valide pour la racine courante
        if (!rhie->fils.present(hie->index))
            erreur(CHAINES[294]);        

        //Tout d'abord on effectue une intersection entre les ancetres du nouvel element
        //et des elements deja trouve
        valeur->et(hie->onto);
        //Puis on rajoute notre element qui a ete certainement elimine par l'operation precedente
        valeur->ou(hie->index);
        rhie=hie;
        if (pt!=NULL)
            *pt='@';
        boucle=pt;
    }
	return valeur;      
}

