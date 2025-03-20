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
   filename   : affiche.cxx
   Date       : 10/01/1999
   Purpose    : Display a tree on screen in ascii
   Programmer : Claude ROUX
   Reviewer   : 
*/


#include "parsbin.h"
#include "generation.h"

static unsigned char* ecran=NULL;

class BrancheGraphe {
public:
    Noeud* tete;
    Cat* fonction;
    VECTA<BrancheGraphe*> bn;
    int index;
    char utilisee;
    

    BrancheGraphe(Cat* Sx,Noeud* premier,int ind) {
        tete=premier;
        fonction=Sx;
        index=ind;
        utilisee=NON;
    }

    void ajoute(Cat* Sx,Noeud* dernier,int ind) {
        BrancheGraphe* bg=new BrancheGraphe(Sx,dernier,ind);
        bn.ajoute(bg);
    }

};

/*-------------------------------------------------------------------------
void AfficheBrancheGraphe(VECTA<BrancheGraphe*>& noeuds,BrancheGraphe* bg,int pos) {

    if (pos>noeuds.dernier)
        return;
    

    if (bg!=NULL && bg->utilisee!=OUI) {
        bg->utilisee=OUI;        
        (bg->tete)->AfficheMot(cout);        
            

        for (int i=0;i<noeuds[pos]->bn.dernier;i++) {
            cout<<" -- "<<fonction->nom<<" -- ";            
            AfficheBrancheGraphe(noeuds,noeuds[pos],NON,position);
        }    
        cout<<endl;
    }

    if (rl==OUI)
        AfficheBrancheGraphe(noeuds,pos+1,OUI,deplacement);
}

-------------------------------------------------------------------------*/

extern GlobalParseur* parseur_courant;

NCoord::NCoord(Noeud* N,float* X,float* Y,float y) {
    float vx;
    NCoord* n;
    float kTailleLettre;
    float kMotEspacement=10;

    nom=ConversionChaine(parseur_courant,N->surface);
	longueur=nom.size();
	if (parseur_courant->colonne!=-1 && longueur>parseur_courant->colonne) {
		nom=nom.substr(0,parseur_courant->colonne-10);
		longueur=nom.size();
	}
    


    kTailleLettre=3;
    yNom=y;xNom=*X+2;yBasCentre=y+kTailleLettre;yHautCentre=y;
    if (yBasCentre > *Y) *Y=yBasCentre;  
    fils=NULL;suivant=NULL;vx=0;
    if (N->fils) {
        fils=new NCoord(N->fils,X,Y,y+kTailleLettre);
        n=fils;
        while(n->suivant) n=n->suivant;
        vx=n->xNom+n->longueur;
        float ll=(vx-fils->xNom)/2-longueur/2;
        xNom=fils->xNom+ll;
    }

    xCentreNom=xNom+longueur/2;
    if (!vx || xNom+longueur>=vx) {
        vx=xNom+longueur;
        if (vx >= *X) 
            *X = vx+1;
    }
    
    if (N->frere && N->frere->droite!=-10) suivant=new NCoord(N->frere,X,Y,y);
}

NCoord::NCoord(genereGraphe* g,float* X,float* Y,float y) {
    float vx;
    float kTailleLettre;
    float kMotEspacement=10;

	
	if (g->noeud!=NULL) {
		ostringstream os;
		g->noeud->Affiche(os,3);
		nom=os.str();
		if (nom[0]=='#')
			nom.erase(0,1);
	}
	else
		nom="GEN_TREE";

	longueur=nom.size();
	if (parseur_courant->colonne!=-1 && longueur>parseur_courant->colonne) {
		nom=nom.substr(0,parseur_courant->colonne-10);
		longueur=nom.size();
	}
    


    kTailleLettre=3;
    yNom=y;xNom=*X+2;
	yBasCentre=y+kTailleLettre;
	yHautCentre=y;
    
	if (yBasCentre > *Y) 
		*Y=yBasCentre;  
    
	fils=NULL;suivant=NULL;vx=0;
	NCoord* ns=fils;
	if (g->enfants.dernier!=0) {
		fils=new NCoord(g->enfants[0],X,Y,y+kTailleLettre);
		ns=fils;
		for (int i=1;i<g->enfants.dernier;i++) {
			ns->suivant=new NCoord(g->enfants[i],X,Y,y+kTailleLettre);
			vx=ns->xNom+ns->longueur;
			float ll=(vx-fils->xNom)/2-longueur/2;
			xNom=fils->xNom+ll;
			ns=ns->suivant;
		}
	}

    xCentreNom=xNom+longueur/2;
    if (!vx || xNom+longueur>=vx) {
        vx=xNom+longueur;
        if (vx >= *X) 
            *X = vx+1;
    }
}

void GlobalParseur::MetEcran(int y,int x,char c) {
    long pos=x*Y_SIZE+y;

    if (x >= 0 && x < X_SIZE && y>=0 && y<Y_SIZE)
        ecran[pos]=(unsigned char)c;
}

unsigned char GlobalParseur::LitEcran(int y,int x) {
    long pos=x*Y_SIZE+y;
    if (x >= 0 && x < X_SIZE && y>=0 && y<Y_SIZE)
        return *(ecran+pos);
    else
        return ' ';
}
    
void GlobalParseur::InitEcran() {
    if (ecran==NULL)
        ecran=new unsigned char[sizeof(char)*X_SIZE*Y_SIZE];
    for (int i=0;i<Y_SIZE;i++)
        for (int j=0;j<X_SIZE;j++)
            MetEcran(i,j,' ');
}

void DetruitEcran() {
    if (ecran!=NULL)
        libere(ecran);
    ecran=NULL;
}

void GlobalParseur::AfficheEcran(ostream& os,long maxX,long maxY) {
    os <<endl ;
    for (int j=0; j<maxY;j++) {
        for (int i=0;i<maxX;i++)
            os<<LitEcran(j,i);
        os<<endl;
    }
}

//colonne maximum, correspond a la position la plus a gauche de lecture...
void GlobalParseur::AfficheCoupe(ostream& os, long maxX,long maxY) {
    //On coupe l'affichage a colonne
    int col=0;
    int mlig=0;
	int compteur=0;

    
    while (col < maxX) {
        if (mlig==0)
            mlig=maxX;

		if (compteur>maxX) {
			os<<"Column value too short"<<Endl;
			break;
		}
		compteur++;
        if ( (mlig-col) > colonne) {
            mlig=col+colonne;
            for (int iscan=0;iscan < maxY;iscan++) {
                //On recherche la colonne ou couper
                if (!strchr(" +-|",LitEcran(iscan,mlig))) {
                    mlig--;
                    while (!strchr(" +-|",LitEcran(iscan,mlig)))
                        mlig--;
                    //On repart du debut...
                    iscan=0;
                }
            }
        }
            
        os <<endl ;
        for (int j=0; j<maxY;j++) {
            for (int i=col;i<mlig;i++)
                os<<LitEcran(j,i);
            os<<endl;
        }
        
        col=mlig++;
        mlig=maxX;
    }
}
        


void DessineLigne2(GlobalParseur* pars,float X1, float Y1, float X2, float Y2) {
    float x,gradient,x1,y1,y2,x2;
    char d='!';

    if (Y1 >= pars->Y_SIZE)
        return;

    x1=X1;y1=Y1;x2=X2;y2=Y2;
    
    float pente;
    if (x1!=x2)
        pente= (y1-y2)/(x1-x2);
    else {
        pente=0;
        x=x1;
    }
    
    if (!pente)
        d='|';
    else
        if (pente >-0.7 && pente <-0.2)
            d='/';
        else
            if (pente >= -0.2 && pente < 0)
                d='\'';
            else
                if (pente >= 0 && pente <= 0.2)
                    d='`';
                    else
                        if (pente < 0.7 && pente >0)
                            d='\\';
    
    gradient= y1-pente*x1;

    if (Y2 >= pars->Y_SIZE)
        Y2=(float)pars->Y_SIZE-1;
    
    for (int y=Y1;y<Y2;y++) {       
        if (pente)
            x= ((float)y-gradient)/pente;
        if (x >= 0 && x< pars->X_SIZE) {
            pars->MetEcran((int)y,(int)x,d);
            if ((d=='\'' || d=='`') && x<pars->X_SIZE-5) {
                 pars->MetEcran((int)y,(int)x+1,d); 
                 pars->MetEcran((int)y,(int)x+2,d);
            }
        }
    }
}
    
void DessineLigne1(GlobalParseur* pars,long X1, long Y1, long X2, long Y2) {
    long x,x1,y,x2;
    
    if (Y1>= pars->Y_SIZE)
        return;

    if (X1 >= pars->X_SIZE)
        return;

    if (Y2>= pars->Y_SIZE)
        Y2=pars->Y_SIZE-1;

    
    if (X1 > X2) {
        x1=X2;
        x2=X1;
    }
    else {
        x1=X1;
        x2=X2;
    }

    for (x=x1;x<=x2;x++)
        if (pars->LitEcran(Y1,x)!='+')
            pars->MetEcran(Y1,x,'-');

    for (y=Y1+1;y<=Y2;y++)
        pars->MetEcran(y,X2,'|');

    pars->MetEcran(Y1,X2,'+');

} 

void NCoord::Dessine(GlobalParseur* pars) {
    NCoord* n;

    long x_nom = xNom;
    long y_nom= yNom;

    int len=x_nom+nom.size();
 
    if (y_nom < pars->Y_SIZE && len < pars->X_SIZE) {
        for (int i=x_nom;i<len;i++)
            pars->MetEcran(y_nom,i,nom[i-x_nom]);

        n=fils;
        while (n) {
            DessineLigne1(pars,(long)xCentreNom,(long)(yNom+1),(long)n->xCentreNom,(long)(n->yNom-1));
            n=n->suivant;
        }
    }

    if (suivant) 
        suivant->Dessine(pars);
    if (fils) 
        fils->Dessine(pars);
}

