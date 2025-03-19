#include "stdafx.h"
#include "ncoord.h"


#include <map>
#include "conversion.h"

extern BOOL g_offset;

NCoord::NCoord(CDC* pDC,XipNode* N,long* X,long* Y,long y,XipNode* pere,int pos) {
    long vx;
    NCoord* n;
   	CSize lchaine;
    long kTailleLettre;
    long kMotEspacement=10;

    if (N->Type()==XIPLEAF) {
        XipLeaf* leaf=(XipLeaf*)N;
		string nm = conversion_utf8_to_latin(leaf->surface);
		nom = (char*)nm.c_str();
    }
    else
        nom=(char*)N->category.c_str();

	if (g_offset==TRUE) {
		char ch[100];
		sprintf(ch,"<%d,%d>",N->left,N->right);
		nom+=ch;
	}

    lchaine=pDC->GetTextExtent(nom);
    longueur=lchaine.cx;
    kTailleLettre=lchaine.cy+2;
    yNom=y;xNom=*X;yBasCentre=y+kTailleLettre;yHautCentre=y;
    if (yBasCentre > *Y) *Y=yBasCentre;  
    fils=NULL;suivant=NULL;vx=0;

    if (N->Type()==XIPNODE) {
        if (N->daughters.size()) {
            fils=new NCoord(pDC,N->daughters[0],X,Y,y+kTailleLettre*2,N,0);
            n=fils;
            while(n->suivant) 
                n=n->suivant;
            vx=n->xNom+n->longueur;
            xNom=fils->xNom+(vx-fils->xNom)/2-longueur/2;
        }
    }

    xCentreNom=xNom+longueur/2;
    if (!vx || xNom+longueur>=vx)  {
        vx=xNom+longueur*2;
        if (vx > *X) 
            *X = vx;
    }

    pos++;
    if (pere!=NULL && pos < pere->daughters.size() && pere->daughters[pos]->left>=0)
        suivant=new NCoord(pDC,pere->daughters[pos],X,Y,y,pere,pos);
}

