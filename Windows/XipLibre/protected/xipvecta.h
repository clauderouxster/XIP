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
   filename   : xipvecta.h
   Date       : 10/09/1992
   Purpose    : Template to handle vector of pointers.
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef xipvecta_h
#define xipvecta_h

#ifndef WIN32
#include "divers.h"
#endif

template <class Z> class VECTA {
 public:
     
     //Un vecteur de Fonction
     Z* vecteur;
     
     //taille est la taille actuelle de la liste
     int taille;
     
     Z zero;
     
     //dernier element entre... Pour gerer les ajouts en fin de liste...
     int dernier;
     
     VECTA(int t=5,Z z=(Z)0) {
         zero=z;
         vecteur=NULL;
         if (t>0) {
             vecteur=new Z[t];
             for (dernier=0;dernier<t;dernier++)
                 vecteur[dernier]=zero;
         }
         taille=t;
         dernier=0;
     }
     
     ~VECTA() {
         delete[] vecteur;
     }
     
     
     inline void raz() {
         dernier=0;
         if (vecteur==NULL)
             return;
         memset(vecteur,0,taille*sizeof(Z));
         //for (int i=0;i<taille;i++)
         //    vecteur[i]=zero;
     }

	 inline void met(int z) {
         dernier=0;
         if (vecteur==NULL)
             return;
         memset(vecteur,z,taille*sizeof(Z));
         //for (int i=0;i<taille;i++)
         //    vecteur[i]=zero;
     }
     
     
     inline void nettoie() {
         if (vecteur==NULL)
             return;
         for (int i=0;i<taille;i++) {
             if (vecteur[i]!=zero)
                 delete vecteur[i];
             vecteur[i]=zero;
         }
         dernier=0;
     }
     
      inline void nettoietable() {
         if (vecteur==NULL)
             return;
         for (int i=0;i<taille;i++) {
             if (vecteur[i]!=zero)
                 delete[] vecteur[i];
             vecteur[i]=zero;
         }
         dernier=0;
     }

    void nulle(Z v) {
         zero=v;
     }
     
	 void reserve(int t) {
		 delete[] vecteur;
		 taille=t;
		 vecteur=new Z[t];
		 memset(vecteur,0,taille*sizeof(Z));
	 }

     void ajuste(int t) {
         Z* tfs;    
         
         if (t<=taille)
             return;
         //on realloue par bloc de t
         tfs=new Z[t];
         
         //for (i=0;i<dernier;i++)
         //  tfs[i]=vecteur[i];
         memcpy(tfs,vecteur,sizeof(Z)*dernier);
         
         //for (i=dernier;i<t;i++)
         //  tfs[i]=zero;
         
         memset(&tfs[dernier],0,sizeof(Z)*(t-dernier));
         
         delete[] vecteur;
         vecteur=tfs;
         taille=t;
     }
     
     void resize(int t) {
         Z* tfs;    
         
         if (t<=taille)
             return;
         //on realloue par bloc de t
         tfs=new Z[t];
         
         memcpy(tfs,vecteur,sizeof(Z)*dernier);         
         memset(&tfs[dernier],0,sizeof(Z)*(t-dernier));
         
         delete[] vecteur;
         vecteur=tfs;
         taille=t;
     }

     void aupluspres() {
         if (dernier==taille)
             return;
         Z* tfs;    
         
         //on realloue par bloc de t
         tfs=new Z[dernier];
         
         memcpy(tfs,vecteur,sizeof(Z)*dernier);
         
         delete[] vecteur;
         vecteur=tfs;
         taille=dernier;     
     }

     void reduit(int t) {        
         if (t>=taille)
             return;
         nettoie();
         delete[] vecteur;
         //on realloue par bloc de t
         vecteur=new Z[t];
         taille=t;
     }


     inline Z retire() {
         if (dernier==0)
             return zero;
         dernier--;
         Z v=vecteur[dernier];
         vecteur[dernier]=zero;
         return v;
     }
     
     inline void pop_back() {
         if (dernier==0)
             return;
         dernier--;         
         vecteur[dernier]=zero;
     }
     
     
     inline Z retireElement(int i=-1) {        
         if (dernier==0)
             return zero;
         
         int pos=i;
         
         if (i==-1)
             pos=dernier-1;
         
         Z v=vecteur[pos];
         vecteur[pos]=zero;
         
         //On deplace toutes les cases de 1...
         //if (i!=-1)
         //  memcpy(vecteur+i,vecteur+i+1,sizeof(Z)*(dernier-i-1));
         
         if (i!=-1) {
             for (int k=i;k<dernier-1;k++)
                 vecteur[k]=vecteur[k+1];
             if (dernier>0)
                 vecteur[dernier-1]=zero;
         }
         
         dernier--;
         return v;
     }
     
	 inline Z remove(int pos = -1) {
		 Z v;
		 if (pos < 0) {
			 if (dernier == 0)
				 return zero;
			 dernier--;
			 v = vecteur[dernier];
			 vecteur[dernier] = zero;
			 return v;
		 }

		 if (pos >= dernier)
			 return zero;
		 v = vecteur[pos];
		 //On deplace toutes les cases de 1...
		 dernier--;
		 for (; pos < dernier; pos++)
			 vecteur[pos] = vecteur[pos + 1];
		 vecteur[dernier] = zero;
		 return v;
	 }

     inline void erase(size_t pos) {        
         if (pos>=dernier)
             return;

         //On deplace toutes les cases de 1...
		 dernier--;
		 for (;pos<dernier;pos++)
			 vecteur[pos]=vecteur[pos+1];		 
		 vecteur[dernier]=zero;                  
     }

	 inline void insert(int pos,Z val) {
		 if (pos<0)
			 return;

		 if (dernier>=taille)
			 ajuste(taille+10);

		 //Dans ce cas, c'est un simple push
		 if (pos>=dernier) {
			 vecteur[pos]=val;
			 dernier++;
			 return;
		 }

		 //on ajoute alors l'element a sa place
		 //si la case est vide on le place a cet endroit
		 //sinon on effectue un deplacement de tous les elements vers la droite
		 if (vecteur[pos]!=NULL) {                          
			 //sinon, on deplace tous les elements d'une case vers la droite
			 for (int i=dernier-1;i>=pos;i--)
				 vecteur[i+1]=vecteur[i];
			 vecteur[pos]=val;
			 dernier++;
		 }
		 else
			 vecteur[pos]=val;
	 }

     inline Z fin() {
         if (dernier==0)
             return zero;
         return vecteur[dernier-1];
     }
     
     inline char detruit(int i) {   
         if (vecteur==NULL || i<0 || i >= dernier || vecteur[i]==zero)
             return 0;
         
         delete vecteur[i];
         vecteur[i]=zero;
         if (i==dernier-1)
             dernier--;
         return 1;
     }
     
     
     inline int ajoute(Z val,int inc=10) {
         
         if (dernier >= taille)
             ajuste(taille+inc);
         
         //sinon on ajoute l'element en queue...
         vecteur[dernier]=val;
         dernier++;
         return dernier-1;
     }

     inline void ajoute(VECTA<Z>& z) {
         int ta=z.dernier+dernier;
         if (ta > taille)
             ajuste(ta+3);

         for (int i=0;i<z.dernier;i++)
             ajoute(z.vecteur[i]);
     }
     
     
     inline int push_back(Z val,int inc=10) {
         
         if (dernier >= taille)
             ajuste(taille+inc);
         
         //sinon on ajoute l'element en queue...
         vecteur[dernier]=val;
         dernier++;
         return dernier-1;
     }
     
          
	 void operator =(VECTA<Z>& z) {
		 dernier=z.dernier;
		 if (dernier>=taille) {
			 delete[] vecteur;
			 taille=dernier+3;
			 vecteur=new Z[taille];
		 }

		 memset(vecteur,0,taille);
		 memcpy(vecteur,z.vecteur,sizeof(Z)*dernier);		 
	 }

     inline Z operator [](int i) {
         if (vecteur==NULL || i<0 || i >= dernier)
             return zero;
         
         return vecteur[i];
     } 

     inline void affecte(VECTA<Z>& z) {
         dernier=0;         
         ajoute(z);
     }

	 inline void affecte(VECTA<Z>* z) {
		 if (z==NULL)
			 return;
         dernier=0;         
         ajoute(*z);
     }
     
     
     inline Z cell(int pos) {
         if (vecteur==NULL || pos<0 || pos>=taille)
             return zero;
         
         return vecteur[pos];
     }
     
     inline Z cell(unsigned int pos) {
         if (vecteur==NULL || pos<0 || pos>=taille)
             return zero;
         
         return vecteur[pos];
     }
     
     inline Z affecte(int pos,Z val) {
         if (pos<0)
             return zero;
         
         if (pos >=taille)
             ajuste(pos+10);
         
         //on ajoute alors l'element a sa place
         vecteur[pos]=val;
         if (pos >=dernier)
             dernier=pos+1;
         return vecteur[pos];
     }
     
	 inline void annule(int pos) {
         if (pos<0)
             return;
         
         if (pos >=dernier)
			 return;
		 
		 vecteur[pos]=zero;
		 
		 if (pos==dernier-1)
			 dernier=pos;
	 }

     inline Z insere(int pos,Z val,int inc=10) {
         if (pos<0)
             return zero;
         
         if (pos>=taille)
             ajuste(pos+inc);

         //on ajoute alors l'element a sa place
         //si la case est vide on le place a cet endroit
         //sinon on effectue un deplacement de tous les elements vers la droite
         if (vecteur[pos]!=NULL) {
             
             if (dernier >=taille)
                 ajuste(taille+10);
             
             //sinon, on deplace tous les elements d'une case vers la droite
             for (int i=dernier-1;i>=pos;i--)
                 vecteur[i+1]=vecteur[i];
             dernier++;
         }
         
         vecteur[pos]=val;
         if (pos==dernier)
             dernier++;         
         return vecteur[pos];
     }

     //Autre insertion, forcement en pos avec deplacement vers la droite des valeurs
     inline Z insertion(int pos,Z val) {
         if (pos<0)
             return zero;
         
         //si l'element est en queue, on l'ajoute a sa position
         if (pos>=dernier) {
             affecte(pos,val);
             return vecteur[pos];
         }

         //on ajoute alors l'element a sa place         
         //sinon on effectue un deplacement de tous les elements vers la droite

         if (dernier >=taille)
             ajuste(taille+10);
         
         //sinon, on deplace tous les elements d'une case vers la droite
         for (int i=dernier-1;i>=pos;i--)
             vecteur[i+1]=vecteur[i];
         
         vecteur[pos]=val;
         dernier++;

         return vecteur[pos];
     }

     int size() {
         return dernier;
     } 
     
     void clear() {
         nettoie();
     }
     
     void asseche() {
         if (vecteur!=NULL)
             delete[] vecteur;
         vecteur=NULL;
         dernier=0;
         taille=0;
     }

     inline int cherche(Z v) {
         for (int i=0;i<dernier;i++)
             if (vecteur[i]==v)
                 return i;
        return -1;
     }
};


#endif


