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
   filename   : vecteur.cxx
   Date       : 10/01/1999
   Purpose    : Handling of features as binary vectors.
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"


Vecteur::Vecteur(short taille) {
    TailleVect=taille;
    vecteur= new TVecteur[taille];
    memset(vecteur,0,sizeof(TVecteur)*TailleVect);
}


Vecteur::Vecteur(Vecteur* v) {
    TailleVect=v->TailleVect;
    vecteur= new TVecteur[TailleVect];
    memcpy(vecteur,v->vecteur,sizeof(TVecteur)*TailleVect);
}

Vecteur:: ~Vecteur() {	   
     delete[] vecteur;     
}


void Vecteur::recopie(Vecteur* v) {
    memcpy(vecteur,v->vecteur,sizeof(TVecteur)*TailleVect);
}

void Vecteur::un(GlobalParseur* pars,int i) {
    unsigned int n;
    
    if (i >= pars->iBorneVect) 
        pars->erreur(pars->CHAINES[88]);

    n= i >> gBitVectDivise; //division par racine(gBitVect)
//le reste se calcule par la formule: i- (i/gBitVect)*gBitVect
    vecteur[n]|= Un << (i - (n << gBitVectDivise)); 
}

void Vecteur::zero(GlobalParseur* pars,int i) {
    int n;
	
    if (i >= pars->iBorneVect) 
        pars->erreur(pars->CHAINES[89]);

    n= i >> gBitVectDivise; //division par racine(gBitVect)
//le reste se calcule par la formule: i- (i/gBitVect)*gBitVect
    vecteur[n]&= ~(Un << (i - (n << gBitVectDivise))); 
}

//    n=i/gBitVect;
//    p=i%gBitVect;
//    vecteur[n]= vecteur[n] & ~(Un << p);

 
char Vecteur::egal(Vecteur* v) {
    TVecteur* val=vecteur;
    TVecteur* vval=v->vecteur;
    
    for (register int i=0;i<TailleVect;i++) {
        if (*val != *vval)
            return NON;
        val++;
        vval++;
    }   
    return OUI;
}

char Vecteur::Zero() {
    register int i;
    for (i=0;i<TailleVect;i++) 
        if (vecteur[i]) return 0;
   return 1;
}


void Vecteur::et(Vecteur* v) {
    register int i;
    TVecteur* val=vecteur;
    TVecteur* vval=v->vecteur;
    for (i=0;i<TailleVect;i++) {
        *val&=*vval;
        val++;vval++;
    }
}


char Vecteur::contient(Vecteur* v) {
    register int i;
    for (i=0;i<TailleVect;i++)
        if (v->vecteur[i] != (vecteur[i] & v->vecteur[i])) 
            return NON;
    return OUI;
}

char Vecteur::NeContientPas(Vecteur* v)
  {register int i;
   for (i=0;i<TailleVect;i++) 
       if (vecteur[i] != (vecteur[i] & ~v->vecteur[i])) return NON;
    return OUI;}
 

char Vecteur::contientNegation(Vecteur* neg)
 {register int i;
   for (i=0;i<TailleVect;i++) 
      if ((vecteur[i] & neg->vecteur[i]) != 0) return OUI;
    return NON;
 }

void Vecteur::Max(void) 
  {register int i;
   for (i=0;i<TailleVect;i++) vecteur[i]=(TVecteur)~0L;};


void Vecteur::xou(Vecteur* v) {
    register int i;
    for (i=0;i<TailleVect;i++) 
        vecteur[i]^=v->vecteur[i];
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
void Vecteur::afficheTrace(GlobalParseur* pars) {
	int i,j,debut;

	trace << "[";debut=OUI;
	for (i=0;i<=pars->iNombreDeValeurs;i++) {
		if (Test(i)) {
			j=i*2;
			if (debut) debut=NON;else trace << ",";
			trace << pars->TableDesTraits[j] << ":" << pars->TableDesTraits[j+1];
		}
	}
	trace << "]";
}
   
/*-------------------------------------------------------------------------*/
 void Vecteur::affiche(GlobalParseur* pars,short neg)
  {int i,j,debut;
   char* mot=NULL;
   
   trace << "[";debut=OUI;
   for (i=0;i<=pars->iNombreDeValeurs;i++) 
      if (Test(i)) 
	 {j=i*2;
	  if (neg==3 && (!mot || strcmp(mot,pars->TableDesTraits[j])))
	     {if (debut) debut=NON;else trace << ",";
	      trace << pars->TableDesTraits[j] << ":?";
	      mot=pars->TableDesTraits[j];}
	  if (neg ==1) 
	     {if (debut) debut=NON;else trace << ",";
	      trace << pars->TableDesTraits[j] << ":" << pars->NEGATION << pars->TableDesTraits[j+1];
		 }
	 }
   trace << "]";
  }
   
/*-------------------------------------------------------------------------*/
void Vecteur::affichev(GlobalParseur* pars,Vecteur* v,Vecteur* neg) {
    int debut=OUI;
    
    for (register int i=TailleVect-1;i>=0;i--) {
        STVecteur val=(STVecteur)vecteur[i];
        STVecteur libre=0; 
        if (v!=NULL)
            libre= (STVecteur)v->vecteur[i];
        STVecteur nega=0; 
        if (neg!=NULL)
            nega= (STVecteur)neg->vecteur[i];
        if (val!=0) {
            //On itere a l'envers
            for (register int j=gBitVect-1;j>=0;j--) {
                //Plus de bit a examiner on s'arrete
                if (val==0)
                    break;
                //le nombre est negatif, le jieme bit etait a un
                if (val < 0) {
                    //nous calculons sa position absolue
                    if (debut) debut=NON;else trace << ",";
                    int pos= (i*gBitVect + j)*2;  
                    if (libre<0) trace << "!";
                    trace << pars->TableDesTraits[pos] << ":";
                    if (nega<0) trace << pars->NEGATION;
                    trace << pars->TableDesTraits[pos+1];
                }
                //nous decalons tous les bits d'un cran vers la gauche, pour les
                //amener les uns derriere les autres en position de bit de signe.
                val<<=1;
                libre<<=1;
                nega<<=1;
            }
        }
    }
}


/*-------------------------------------------------------------------------*/
//Explication sur l'analyse d'une sequence de bit...
//Nous utilisons dans le reste du code, le fait que le bit de signe est
//le dernier bit a gauche de n'importe quel nombre. Nous traduisons d'abord
//le nombre dont nous voulons extraire les bits en entier signe, puis nous effectuons
//un shift vers la gauche de tous les bits de ce nombre signe'. Nous bouclons sur le
//nombre de bits dans un vecteur. Si au bout de x iterations, le nombre obtenu est negatif
//cela signifie que le x bit etait a UN. L'inconvenient de cette methode est que les 
//traits sont construits a l'envers...
void Vecteur::Chaine(GlobalParseur* pars,char* chaine) {
    int debut=1;
   
    chaine[0]=0;
    for (register int i=TailleVect-1;i>=0;i--) {
        STVecteur val=(STVecteur)vecteur[i];
        if (val!=0) {
            //On itere a l'envers
            for (register int j=gBitVect-1;j>=0;j--) {
                //Plus de bit a examiner on s'arrete
                if (val==0)
                    break;
                //le nombre est negatif, le jieme bit etait a un
                if (val < 0) {
                    //nous calculons sa position absolue
                    int pos= (i*gBitVect + j)*2;       
                    if (debut) debut=0;else strcat(chaine,",");
                    strcat(chaine,pars->TableDesTraits[pos]);
                    strcat(chaine,":");
                    strcat(chaine,pars->TableDesTraits[pos+1]);
                }
                //nous decalons tous les bits d'un cran vers la gauche, pour les
                //amener les uns derriere les autres en position de bit de signe.
                val<<=1;
            }
        }
    }
}

char Vecteur::Valeur(GlobalParseur* pars,char* chaine,int i) {
    int debut=1;
    int nb=0;
    
    chaine[0]=0;
    STVecteur val=(STVecteur)vecteur[i];
    if (val!=0) {
        //On itere a l'envers
        for (register int j=gBitVect-1;j>=0;j--) {
            //Plus de bit a examiner on s'arrete
            if (val==0)
                return NON;
            //le nombre est negatif, le jieme bit etait a un
            if (val < 0) {
                //nous calculons sa position absolue
                int pos= (i*gBitVect + j)*2;       
                strcat(chaine,pars->TableDesTraits[pos+1]);
                return OUI;
            }
            //nous decalons tous les bits d'un cran vers la gauche, pour les
            //amener les uns derriere les autres en position de bit de signe.
            val<<=1;
        }
    }
    return NON;
}

char Vecteur::Attribut(GlobalParseur* pars,char* chaine) {
    int debut=1;
    int nb=0;
    
    chaine[0]=0;
    for (register int i=TailleVect-1;i>=0;i--) {
        STVecteur val=(STVecteur)vecteur[i];
        if (val!=0) {
            //On itere a l'envers
            for (register int j=gBitVect-1;j>=0;j--) {
                //Plus de bit a examiner on s'arrete
                if (val==0)
                    return NON;
                //le nombre est negatif, le jieme bit etait a un
                if (val < 0) {
                    //nous calculons sa position absolue
                    int pos= (i*gBitVect + j)*2;       
                    strcat(chaine,pars->TableDesTraits[pos]);
                    return OUI;
                }
                //nous decalons tous les bits d'un cran vers la gauche, pour les
                //amener les uns derriere les autres en position de bit de signe.
                val<<=1;
            }
        }
    }
    return NON;
}

/*-------------------------------------------------------------------------*/
void Vecteur::ChaineAttributs(GlobalParseur*pars,char* chaine) {
    int debut=1;
    chaine[0]=0;

    for (register int i=TailleVect-1;i>=0;i--) {
        STVecteur val=(STVecteur)vecteur[i];
        if (val!=0) {
            for (register int j=gBitVect-1;j>=0;j--) {
                if (val==0)
                    break;
                if (val < 0) {
                    int pos= (i*gBitVect + j)*2;       
                    
                    if (!strstr(chaine,pars->TableDesTraits[pos])) {
                        if (debut) debut=0;else strcat(chaine,",");		  
                        strcat(chaine,pars->TableDesTraits[pos]);
                    }
                    
                }
                val<<=1;
            }
        }        
    }
}


/*-------------------------------------------------------------------------*/
void Vecteur::TraitsEnChaine(GlobalParseur* pars,
                             char* resultat,
                             Vecteur* Li,
                             Vecteur* Pres,
                             Vecteur* In,
                             Vecteur* Ne) {
    register int i,j;
    char* motIn=NULL;
    int debut=OUI;

    char nega[3];
    nega[0]=':';
    nega[1]=pars->NEGATION;
    nega[2]=0;
    int dernierpos=-1;

    strcpy(resultat, "[");


    Vecteur v(this);
    v.et(pars->FILTRE);
    if (Ne!=NULL)
        v.et(Ne);
    
    Vecteur n(TailleVect);
    if (Ne!=NULL) {
        n.recopie(Ne);   
        n.et(pars->FILTRE);
    }

    Vecteur p(TailleVect);
    if (Pres) {
        p.recopie(Pres);
        p.et(pars->FILTRE);
    }

    Vecteur l(TailleVect);
    if (Li) {
        l.recopie(Li);
        l.et(pars->FILTRE);
        l.et(&v);
    }

    Vecteur b(TailleVect);
    if (In) {
        b.recopie(In);
        b.et(pars->FILTRE);
        b.et(&v);
    }
                
    for (i=TailleVect-1;i>=0;i--) {

        STVecteur vVal=(STVecteur)v.vecteur[i];
        STVecteur lVal=(STVecteur)l.vecteur[i];
        STVecteur pVal=(STVecteur)p.vecteur[i];
        STVecteur bVal=(STVecteur)b.vecteur[i];
        STVecteur nVal=(STVecteur)n.vecteur[i];
        
        if (vVal || lVal || pVal || bVal || nVal) {
            for (j=gBitVect-1;j>=0;j--) {

                if (!vVal && !lVal && !pVal && !bVal && !nVal)
                    break;

                int pos=-1;
                
                if (vVal < 0) {       
                    if (pos==-1) pos=(i*gBitVect + j)*2;
                    if (debut==OUI) debut=NON;else strcat(resultat, ",");
                    if (lVal<0)
                        strcat(resultat, "!");
                    strcat(resultat, pars->TableDesTraits[pos]); 
                    strcat(resultat,":");
                    strcat(resultat, pars->TableDesTraits[pos+1]);
                }
                else
                    if (lVal<0) {
                        if (pos==-1) pos=(i*gBitVect + j)*2;  
                        if (dernierpos==-1 ||
                            strcmp(pars->TableDesTraits[dernierpos],
                                   pars->TableDesTraits[pos])) {
                            if (debut==OUI) debut=NON;else strcat(resultat, ",");
                            strcat(resultat, "!");
                            strcat(resultat, pars->TableDesTraits[pos]); 
                            strcat(resultat,":!");
                            dernierpos=pos;
                        }
                    }
            
                if (pVal < 0) {
                    if (pos==-1) pos=(i*gBitVect + j)*2;  
                    if (dernierpos==-1 ||
                        strcmp(pars->TableDesTraits[dernierpos],
                               pars->TableDesTraits[pos])) {     
                        if (debut==OUI) debut=NON;else strcat(resultat, ",");
                        strcat(resultat, pars->TableDesTraits[pos]); 
                        strcat(resultat,":?");
                        dernierpos=pos;
                    }
                }
 
                if (bVal < 0) {
                    if (pos==-1) pos=(i*gBitVect + j)*2;  
                    if (dernierpos==-1 ||
                        strcmp(pars->TableDesTraits[dernierpos],
                           pars->TableDesTraits[j])) {
                        if (debut==OUI) debut=NON; else strcat(resultat, ",");
                        strcat(resultat, pars->TableDesTraits[pos]); 
                        strcat(resultat,":!");
                        dernierpos=pos;
                    }
                }
            
                if (nVal < 0) {
                    pos=(i*gBitVect + j)*2;
                    if (debut==OUI) debut=NON; else strcat(resultat, ",");
                    strcat(resultat, pars->TableDesTraits[pos]); 
                    strcat(resultat,nega);
                    strcat(resultat, pars->TableDesTraits[pos+1]);
                }
            
                vVal<<=1;
                lVal<<=1;
                pVal<<=1;
                bVal<<=1;
                nVal<<=1;
            }
        }
    }
    strcat(resultat, "]");
}

void Vecteur::TraitsEnMap(GlobalParseur* pars,
	map<string,string>& m,
	Vecteur* Li,
	Vecteur* Pres,
	Vecteur* In,
	Vecteur* Ne) {
	register int i, j;
	char* motIn = NULL;

	char nega[3];
	nega[0] = ':';
	nega[1] = pars->NEGATION;
	nega[2] = 0;
	int dernierpos = -1;


	Vecteur v(this);
	v.et(pars->FILTRE);
	if (Ne != NULL)
		v.et(Ne);

	Vecteur n(TailleVect);
	if (Ne != NULL) {
		n.recopie(Ne);
		n.et(pars->FILTRE);
	}

	Vecteur p(TailleVect);
	if (Pres) {
		p.recopie(Pres);
		p.et(pars->FILTRE);
	}

	Vecteur l(TailleVect);
	if (Li) {
		l.recopie(Li);
		l.et(pars->FILTRE);
		l.et(&v);
	}

	Vecteur b(TailleVect);
	if (In) {
		b.recopie(In);
		b.et(pars->FILTRE);
		b.et(&v);
	}

	string str;
	for (i = TailleVect - 1; i >= 0; i--) {

		STVecteur vVal = (STVecteur)v.vecteur[i];
		STVecteur lVal = (STVecteur)l.vecteur[i];
		STVecteur pVal = (STVecteur)p.vecteur[i];
		STVecteur bVal = (STVecteur)b.vecteur[i];
		STVecteur nVal = (STVecteur)n.vecteur[i];

		if (vVal || lVal || pVal || bVal || nVal) {
			for (j = gBitVect - 1; j >= 0; j--) {

				if (!vVal && !lVal && !pVal && !bVal && !nVal)
					break;

				int pos = -1;

				if (vVal < 0) {
					if (pos == -1) pos = (i*gBitVect + j) * 2;					
					str = "";
					if (lVal < 0)
						str = "!";
					str += pars->TableDesTraits[pos];
					m[str] = pars->TableDesTraits[pos + 1];
				}
				else {
					if (lVal < 0) {
						if (pos == -1) pos = (i*gBitVect + j) * 2;
						if (dernierpos == -1 || strcmp(pars->TableDesTraits[dernierpos], pars->TableDesTraits[pos])) {
							str = "!";
							str += pars->TableDesTraits[pos];
							m[str] = "!";
							dernierpos = pos;
						}
					}
				}
				if (pVal < 0) {
					if (pos == -1) pos = (i*gBitVect + j) * 2;
					if (dernierpos == -1 || strcmp(pars->TableDesTraits[dernierpos], pars->TableDesTraits[pos])) {
						str = pars->TableDesTraits[pos];
						m[str] = "?";
						dernierpos = pos;
					}
				}

				if (bVal < 0) {
					if (pos == -1) pos = (i*gBitVect + j) * 2;
					if (dernierpos == -1 || strcmp(pars->TableDesTraits[dernierpos], pars->TableDesTraits[j])) {
						str=pars->TableDesTraits[pos];
						m[str] = "!";
						dernierpos = pos;
					}
				}

				if (nVal < 0) {
					pos = (i*gBitVect + j) * 2;
					str = pars->TableDesTraits[pos];
					str += nega;
					m[str] = pars->TableDesTraits[pos + 1];
				}

				vVal <<= 1;
				lVal <<= 1;
				pVal <<= 1;
				bVal <<= 1;
				nVal <<= 1;
			}
		}
	}	
}

/*-------------------------------------------------------------------------*/
void Vecteur::TraitsEnChaineString(GlobalParseur* pars,
                                   string& resultat,
                                   Vecteur* Li,
                                   Vecteur* Pres,
                                   Vecteur* In,
                                   Vecteur* Ne) {
    register int i,j;
    char* motIn=NULL;
    int debut=OUI;

    char nega[3];
    nega[0]=':';
    nega[1]=pars->NEGATION;
    nega[2]=0;
    int dernierpos=-1;

    resultat="[";

    Vecteur v(this);
    v.et(pars->FILTRE);
    if (Ne!=NULL)
        v.et(Ne);
    
    Vecteur n(TailleVect);
    if (Ne!=NULL) {
        n.recopie(Ne);   
        n.et(pars->FILTRE);
    }

    Vecteur p(TailleVect);
    if (Pres) {
        p.recopie(Pres);
        p.et(pars->FILTRE);
    }

    Vecteur l(TailleVect);
    if (Li) {
        l.recopie(Li);
        l.et(pars->FILTRE);
        l.et(&v);
    }

    Vecteur b(TailleVect);
    if (In) {
        b.recopie(In);
        b.et(pars->FILTRE);
        b.et(&v);
    }
                
    for (i=TailleVect-1;i>=0;i--) {

        STVecteur vVal=(STVecteur)v.vecteur[i];
        STVecteur lVal=(STVecteur)l.vecteur[i];
        STVecteur pVal=(STVecteur)p.vecteur[i];
        STVecteur bVal=(STVecteur)b.vecteur[i];
        STVecteur nVal=(STVecteur)n.vecteur[i];
        
        if (vVal || lVal || pVal || bVal || nVal) {
            for (j=gBitVect-1;j>=0;j--) {

                if (!vVal && !lVal && !pVal && !bVal && !nVal)
                    break;

                int pos=-1;
                
                if (vVal < 0) {       
                    if (pos==-1) pos=(i*gBitVect + j)*2;
                    if (debut==OUI) 
                        debut=NON;
                    else 
                        resultat+= ",";
                    if (lVal<0)
                        resultat+= "!";
                    resultat+= pars->TableDesTraits[pos]; 
                    resultat+=":";
                    resultat+= pars->TableDesTraits[pos+1];
                }
                else
                    if (lVal<0) {
                        if (pos==-1) pos=(i*gBitVect + j)*2;  
                        if (dernierpos==-1 ||
                            strcmp(pars->TableDesTraits[dernierpos],
                                   pars->TableDesTraits[pos])) {
                            if (debut==OUI) debut=NON;else resultat+= ",";
                            resultat+= "!";
                            resultat+= pars->TableDesTraits[pos]; 
                            resultat+=":!";
                            dernierpos=pos;
                        }
                    }
            
                if (pVal < 0) {
                    if (pos==-1) pos=(i*gBitVect + j)*2;  
                    if (dernierpos==-1 ||
                        strcmp(pars->TableDesTraits[dernierpos],
                               pars->TableDesTraits[pos])) {     
                        if (debut==OUI) debut=NON;else resultat+= ",";
                        resultat+= pars->TableDesTraits[pos]; 
                        resultat+=":?";
                        dernierpos=pos;
                    }
                }
 
                if (bVal < 0) {
                    if (pos==-1) pos=(i*gBitVect + j)*2;  
                    if (dernierpos==-1 ||
                        strcmp(pars->TableDesTraits[dernierpos],
                           pars->TableDesTraits[j])) {
                        if (debut==OUI) debut=NON; else resultat+= ",";
                        resultat+= pars->TableDesTraits[pos]; 
                        resultat+=":!";
                        dernierpos=pos;
                    }
                }
            
                if (nVal < 0) {
                    pos=(i*gBitVect + j)*2;
                    if (debut==OUI) debut=NON; else resultat+= ",";
                    resultat+= pars->TableDesTraits[pos]; 
                    resultat+=nega;
                    resultat+= pars->TableDesTraits[pos+1];
                }
            
                vVal<<=1;
                lVal<<=1;
                pVal<<=1;
                bVal<<=1;
                nVal<<=1;
            }
        }
    }
    resultat+= "]";
}

   
/*-------------------------------------------------------------------------*/
void Vecteur::Affiche(GlobalParseur* pars,Vecteur* v,Vecteur* neg) {
    int i,j,debut;
   
    debut=1;
    for (i=0;i<=pars->iNombreDeValeurs;i++) 
        if (Test(i)) {
            if (debut) debut=0;else *(pars->Sortie) << ",";
            if (v && v->Test(i)) *(pars->Sortie) << "!";
            j=i*2;
            *(pars->Sortie) << pars->TableDesTraits[j] << ":";
            if (neg && neg->Test(i)) *(pars->Sortie) << pars->NEGATION;
            *(pars->Sortie) << pars->TableDesTraits[j+1];
        }
}

	
#ifdef XIPSTLAPI
void Vecteur::AjouteTraitsSTL(GlobalParseur* pars,
                              VXipFeature& xip,
                              Vecteur* filtre,
							  Vecteur* libre) {
    
    if (filtre==NULL)
        return;
    
    for (register int i=TailleVect-1;i>=0;i--) {
        STVecteur val=(STVecteur)vecteur[i];
        STVecteur fil = (STVecteur)filtre->vecteur[i];
		STVecteur vallibre=0;
		//if (libre!=NULL)
		//	vallibre=(STVecteur)libre->vecteur[i];
        if (val!=0) {
            for (register int j=gBitVect-1;j>=0;j--) {
                if (val==0)
                    break;
                if (val < 0 && fil <0) {       
                    int pos= (i*gBitVect + j)*2;
					string att;
					if (vallibre<0) {
						att="!";
						att+=ConversionChaine(pars,pars->TableDesTraits[pos]);
					}
					else
						att=ConversionChaine(pars,pars->TableDesTraits[pos]);
					string val=ConversionChaine(pars,pars->TableDesTraits[pos+1]);
                    XipFeature* feature=new XipFeature(att,val);
                    xip.push_back(feature);
                }
                val<<=1;
                fil<<=1;
				vallibre<<=1;
            }
        }
    }
}
#endif

void Vecteur::AjouteTraits(GlobalParseur* pars,
                           VECTA<XipFeature*>& xip,
                           Vecteur* filtre,
						   Vecteur* libre) {
    
    static char tlibre[256];
    if (filtre==NULL)
        return;

    for (register int i=TailleVect-1;i>=0;i--) {
        STVecteur val=(STVecteur)vecteur[i];
        STVecteur fil = (STVecteur)filtre->vecteur[i];
		STVecteur vallibre=0;
		//if (libre!=NULL)
		//	vallibre== (STVecteur)libre->vecteur[i];
        if (val!=0) {
            for (register int j=gBitVect-1;j>=0;j--) {
                if (val==0)
                    break;
                if (val < 0 && fil <0) {       
                    int pos= (i*gBitVect + j)*2;
					if (vallibre<0) {
						sprintf(tlibre,"!%s",pars->TableDesTraits[pos]);
						XipFeature* feature=new XipFeature(
							tlibre,
							pars->TableDesTraits[pos+1]);
						xip.ajoute(feature);
					}
					else {
						XipFeature* feature=new XipFeature(
							pars->TableDesTraits[pos],
							pars->TableDesTraits[pos+1]);
						xip.ajoute(feature);                    
					}
                }
                val<<=1;
                fil<<=1;
				vallibre<<=1;
            }
        }
    }
}

void Vecteur::ConcateneAttributs(GlobalParseur* pars,
                                 char* chaine,
                                 Vecteur* filtre,
                                 char* separateur) {

    chaine[0]=0;   
    if (filtre==NULL)
        return;

	bool premier = NON;
	if (separateur[0] == '|')
		premier = OUI;
    for (register int i=TailleVect-1;i>=0;i--) {
        STVecteur val=(STVecteur)vecteur[i];
        STVecteur fil = (STVecteur)filtre->vecteur[i];
        if (val!=0) {
            for (register int j=gBitVect-1;j>=0;j--) {
                if (val==0)
                    break;
                if (val < 0 && fil <0) {       
                    int pos= (i*gBitVect + j)*2;
					if (premier)
						premier = NON;
					else
						strcat(chaine, separateur);
                    strcat(chaine,pars->TableDesTraits[pos]);
                }
                val<<=1;
                fil<<=1;
            }
        }        
    }
}

void Vecteur::ConcateneValeurs(GlobalParseur* pars,
                               char* chaine,
                               Vecteur* filtre,
                               char* separateur,
                               char posSep) {
    chaine[0]=0;
    
    if (filtre==NULL)
        return;


    for (register int i=TailleVect-1;i>=0;i--) {
        STVecteur val=(STVecteur)vecteur[i];
        STVecteur fil = (STVecteur)filtre->vecteur[i];
        if (val!=0) {
            for (register int j=gBitVect-1;j>=0;j--) {
                if (val==0)
                    break;
                if (val < 0 && fil <0) {       
                    int pos= (i*gBitVect + j)*2;
                    if (posSep==-1)
                        posSep=0;
                    else
                        if (!posSep)
                            strcat(chaine,separateur);		  
                    strcat(chaine,pars->TableDesTraits[pos]);
                    strcat(chaine,":");
                    strcat(chaine,pars->TableDesTraits[pos+1]);
                    if (posSep)
                        strcat(chaine,separateur);		  
                }
                val<<=1;
                fil<<=1;
            }
        }        
    }
}

void Vecteur::ConcateneValeursString(GlobalParseur* pars,
                               string& chaine,
                               Vecteur* filtre,
                               char* separateur,
                               char posSep) {
    chaine="[";
    
    if (filtre==NULL)
        return;


    for (register int i=TailleVect-1;i>=0;i--) {
        STVecteur val=(STVecteur)vecteur[i];
        STVecteur fil = (STVecteur)filtre->vecteur[i];
        if (val!=0) {
            for (register int j=gBitVect-1;j>=0;j--) {
                if (val==0)
                    break;
                if (val < 0 && fil <0) {       
                    int pos= (i*gBitVect + j)*2;
                    if (posSep==-1)
                        posSep=NON;
                    else
                        if (!posSep)
                            chaine+=separateur;		  
                    chaine+=pars->TableDesTraits[pos];
					chaine+=":";
                    chaine+=pars->TableDesTraits[pos+1];
                }
                val<<=1;
                fil<<=1;
            }
        }        
    }
	chaine+="]";
}

void Vecteur::AfficheValeursXML(GlobalParseur* pars,ostream& os,Vecteur* filtre) {
    
    if (filtre==NULL)
        return;

    for (register int i=TailleVect-1;i>=0;i--) {
        STVecteur val=(STVecteur)vecteur[i];
        STVecteur fil = (STVecteur)filtre->vecteur[i];
        if (val!=0) {
            for (register int j=gBitVect-1;j>=0;j--) {
                if (val==0)
                    break;
                if (val < 0 && fil <0) {
                    int pos= (i*gBitVect + j)*2;
                    os<<"<FEATURE attribute=\""<<pars->TableDesTraits[pos]<<"\" ";
                    os<<"value=\""<<pars->TableDesTraits[pos+1]<<"\"/>"<<Endl;
                }
                val<<=1;
                fil<<=1;
            }
        }        
    } 
}


void Vecteur::Liste(VECTA<int>& liste) {    

    for (register int i=TailleVect-1;i>=0;i--) {   
        STVecteur val=(STVecteur)vecteur[i];
        if (val!=0) {
            for (register int j=gBitVect-1;j>=0;j--) {
                if (val==0)
                    break;

                if (val < 0) {
                    int pos= i*gBitVect + j;
                    liste.ajoute(pos);
                }

                val<<=1;
            }
        }
    }
}


