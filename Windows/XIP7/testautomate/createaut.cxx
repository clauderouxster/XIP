#include <ostream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
using namespace std;

#define NON 0
#define OUI 1

#define cfinmot 1
#define cfinres 2


#include "vecta.h"

#define modeOuverture ios::in|ios::binary
#define uchar unsigned char

int base255(int nb,uchar* s) {
	char ch[100];
	s[0]=0;
	int p=0;
	while (nb!=0) {
		uchar v=1+nb%255;
		sprintf(ch,"%c%s",v,s);
		strcpy((char*)s,ch);
		nb=nb/255;
	}
	return strlen(ch);
}


class arcTRANS {
public:
	uchar u; //surface
	uchar l; //lemme
	char type;
	int pere;
	int id;
	int appel;
	int clef;
	VECTA<int> suivants;

	arcTRANS::arcTRANS(uchar U, uchar L,int c) {
		pere=-1;
		u=U;
		l=L;
		type=0;
		appel=0;
		clef=c;
	}

	void ajoute(VECTA<arcTRANS*>& arcs,arcTRANS* a) {
		int i=0;
		for (;i<suivants.dernier;i++) {
			if (a->clef>arcs[suivants[i]]->clef)				
				break;
		}
		suivants.insere(i,a->id);
	}

	void remplace(int id,int nouveau) {
		for (int i=0;i<suivants.dernier;i++) {
			if (suivants[i]==id) {
				suivants.affecte(i,nouveau);
				return;
			}
		}
	}

	int rechercheidx(VECTA<arcTRANS*>& arcs,int c) {
		for (int i=0;i<suivants.dernier;i++) {
			if (arcs[suivants[i]]->clef==c)
				return i;
		}
		return -1;		
	}


	int recherche(VECTA<arcTRANS*>& arcs,int c) {
		for (int i=0;i<suivants.dernier;i++) {
			if (arcs[suivants[i]]->clef==c)
				return suivants[i];
		}
		return -1;		
	}
};

class elarc {
public:

	VECTA<arcTRANS*> clef;
	int ec;
	int lg;
	elarc(int m,int l, int e) : clef(m) {
		ec=e; 
		lg=l;
	}

	void ajoute(arcTRANS* a) {
		clef.ajoute(a);
	}
};

class dicoarc {
public:
	int increment;
	VECTA<elarc*> lesclefs;
	map<string, elarc* > clefs;


	dicoarc(int inc) : lesclefs(100) {
		increment=inc;
	}

	void ajoute(string c,arcTRANS* a,int ec) {
		elarc* va=clefs[c];
		if (va==NULL) {
			int l=c.size();
			va=new elarc(increment,l,ec);
			int i=0;
			for (;i<lesclefs.dernier;i++) {
				if (lesclefs[i]->lg>l)				
					break;
			}
			lesclefs.insere(i,va);
			clefs[c]=va;			
		}
		va->ajoute(a);
	}


	~dicoarc() {
		for (int i=0;i<lesclefs.dernier;i++)
			delete lesclefs[i];
		lesclefs.raz();
	}
};


class BuildAutomaton {
public:

	VECTA<arcTRANS*> arcs;
	VECTA<arcTRANS*> initiaux[256];
	dicoarc* finaux;
	VECTA<char*> Slesclefs;
	short lesclefs[256][256];
	int arcsutiles;

	map<string,uchar> subtraits;
	vector<string> lsubtraits;
	int liretraits(uchar* lemme,char* curseur);
	void compileautomaton(const char* nom);
	void affiche(arcTRANS* a, int indent);
	void affichage();
	uchar nbtraits;
	int maxsize;


	BuildAutomaton(int m) : arcs(m),Slesclefs(1000) {
		finaux=new dicoarc(m>>3);
		maxsize=m;
		nbtraits=0;
		for (int i=0;i<256;i++)
			for (int j=0;j<256;j++)
				lesclefs[i][j]=-1;
	}
	~BuildAutomaton() {
		delete finaux;
		Slesclefs.nettoie();
		arcs.nettoie();
	}

	uchar ajoutetraits(char* curseur) {
		uchar id=subtraits[curseur];
		if (id==0) {
			subtraits[curseur]=nbtraits;
			lsubtraits.push_back(curseur);
			id=nbtraits;
			nbtraits++;
		}
		return id;
	}

	char parcours(arcTRANS* a,string& clef,VECTA<int>&);

	int rechercheclef(uchar* cl) {
		int ec=lesclefs[cl[0]][cl[1]];
		if (ec==-1) {
			ec=Slesclefs.dernier;
			lesclefs[cl[0]][cl[1]]=ec;
			char* ss=new char[3];
			strcpy(ss,(char*)cl);
			Slesclefs.ajoute(ss);
		}

		return ec;
	}

	int rechercheclef(uchar u,uchar l) {
		return lesclefs[u][l];
	}

	int minimisation(int);
	void sauvegarde(const char* nom);
};


char BuildAutomaton::parcours(arcTRANS* a,string& laclef,VECTA<int>& ids) {
	if (a==NULL)
		return OUI;

	char base[100];
	if (ids.cherche(a->id)!=-1) {		
		sprintf(base,"_%d",a->id+Slesclefs.dernier+1);
		laclef+=base;
		return OUI;
	}
	ids.ajoute(a->id);	
	//sprintf(base,"=%d",a->clef);
	//laclef+=base;
	laclef+=a->u;
	laclef+=a->l;
	arcTRANS* an;
	for (int i=0;i<a->suivants.dernier;i++) {
		an=arcs[a->suivants[i]];
		laclef+='<';
		if (parcours(an,laclef,ids)==NON)
			return NON;
		laclef+='>';
	}
	return OUI;
}


int BuildAutomaton::liretraits(uchar* lemme,char* curseur) {
	char* pt=strchr(curseur,'+');
	int plemme=strlen((char*)lemme);
	uchar id;
	//On rajoute un fin de chaine
	lemme[plemme++]=cfinres;

	while (pt!=NULL) {
		*pt=0;
		id=ajoutetraits(curseur);
		lemme[plemme++]=id;
		curseur=pt+1;
		pt=strchr(curseur,'+');
	}
	lemme[plemme]=0;
	return plemme;
}

int compteclefm=0;
//On va rattacher les arcs communs
int BuildAutomaton::minimisation(int app) {
	VECTA<int> ids;
	char boucle=OUI;
	int comptemodif=0;
	int comptedest=0;
	dicoarc* nouveau=new dicoarc(3);	
	int i;
	int max=0;
	int cpvaldiff=0;
	char buffer[100];
	int comptage=0;

	cout<<app<<": Minimisation: ";
	string laclef;

	for (int f=0;f<finaux->lesclefs.dernier;f++) {
		VECTA<arcTRANS*>& lesarcs=finaux->lesclefs[f]->clef;
		arcTRANS* courant;
		arcTRANS* pere;
		arcTRANS* fils;
		int idx;
		int debut=0;
		int idcommun=-1;
		int uneclef=finaux->lesclefs[f]->ec;

		arcTRANS* base=lesarcs[debut++];
		//On va donc chercher l'element vers lequel pointe base[clef]
		while (debut<lesarcs.dernier) {
			idx=base->recherche(arcs,uneclef);
			fils=arcs[idx];
			comptage++;
			if (fils->suivants.dernier<=1) {
				base->appel=-3;
				idcommun=idx;
				pere=arcs[base->pere];
				if (pere!=NULL) {
					if (pere->appel<app+1) {
						sprintf(buffer,"%c%c_%d",base->u,base->l,cpvaldiff);
						nouveau->ajoute(buffer,pere,base->clef);
						pere->appel=app+1;
					}
				}
				break;
			}
			arcTRANS* base=lesarcs[debut++];
		}

		dicoarc* multiple=new dicoarc(3);
		
		for (i=0;i<lesarcs.dernier;i++) {
			courant=lesarcs[i];	
			if (courant->appel!=app)
					continue;
			
			comptage++;
			//Dans le cas d'un arc unique
			idx=courant->rechercheidx(arcs,uneclef);
			fils=arcs[courant->suivants[idx]];

			if (fils->suivants.dernier<=1 && idcommun!=-1) {
				courant->suivants.vecteur[idx]=idcommun;
				courant->appel=-2;
				//On garde alors les nouveaux finaux
				pere=arcs[courant->pere];
				if (pere!=NULL) {
					if (pere->appel<app+1) {
						sprintf(buffer,"%c%c_%d",courant->u,courant->l,cpvaldiff);
						nouveau->ajoute(buffer,pere,courant->clef);
						pere->appel=app+1;
					}
				}
				comptemodif++;
			}
			else {
				//Il faut calculer les clefs des elements qui partent de courant
				ids.raz();				
				laclef="";
				courant->appel=-1;
				if (parcours(fils,laclef,ids)==OUI) {
					multiple->ajoute(laclef,courant,-1);
					compteclefm++;
				}
			}
		}
		cpvaldiff++;
		for (int j=0;j<multiple->lesclefs.dernier;j++) {
			VECTA<arcTRANS*>& pmult=multiple->lesclefs[j]->clef;
			if (pmult.dernier<=1)
				continue;
			if (max<pmult.dernier)
				max=pmult.dernier;
			base=pmult[0];
			base->appel=-3;
			idcommun=base->recherche(arcs,uneclef);
			arcs[idcommun]->appel=-3;
			pere=arcs[base->pere];
			if (pere!=NULL) {
				if (pere->appel<app+1) {
					sprintf(buffer,"%c%c_%d",base->u,base->l,cpvaldiff);
					nouveau->ajoute(buffer,pere,base->clef);
					pere->appel=app+1;
				}
			}
			
			for (i=1;i<pmult.dernier;i++) {
				courant=pmult[i];	
				if (courant->appel!=-1) 
					continue;

				courant->appel=-2;
				idx=courant->rechercheidx(arcs,uneclef);
				courant->suivants.vecteur[idx]=idcommun;

				pere=arcs[courant->pere];
				if (pere!=NULL) {
					if (pere->appel<app+1) {
						sprintf(buffer,"%c%c_%d",courant->u,courant->l,cpvaldiff);
						nouveau->ajoute(buffer,pere,courant->clef);
						pere->appel=app+1;
					}
				}
				comptemodif++;
				comptedest++;
			}
			cpvaldiff++;
		}
		delete multiple;	
	}
	cout<<comptemodif<<" "<<comptedest<<" "<<comptage<<endl;
	delete finaux;
	finaux=nouveau;
	return comptemodif;
}


void blanc(int indent) {
	for (int i=0;i<indent;i++)
		cout<<" ";
}

void BuildAutomaton::affiche(arcTRANS* a, int indent) {
	char ch[100];
	char c;

	if (a->type==NON)
		sprintf(ch,"%c/%c:%d ",a->u,a->l,a->id);
	else {
		if (a->u!=cfinmot)
			c=a->u;
		else
			c='.';
		sprintf(ch,"%c/%s:%d ",c,lsubtraits[a->l].c_str(),a->id);
	}
	cout<<ch;
	if (indent==0)
		indent=strlen(ch);
	for (int l=0;l<a->suivants.dernier;l++) {
		arcTRANS* s=arcs[a->suivants[l]];
		affiche(s,indent+strlen(ch));
		if (l<a->suivants.dernier-1)
			blanc(indent);
	}
	
	if (a->suivants.dernier==0)
		cout<<endl;
}

void BuildAutomaton::affichage() {
	for (int i=0;i<256;i++) {
		for (int j=0;j<initiaux[i].dernier;j++) {
			arcTRANS* a=initiaux[i][j];
			affiche(a,0);
		}
	}
}

void BuildAutomaton::compileautomaton(const char* nom) {

	ifstream fichier(nom,modeOuverture);
	uchar surface[1000];
	uchar lemme[1000];
	int i,e;
	uchar traits[1000];
	uchar clef[]={0,0,0};
	int maxlg;
	int lglemme;
	int lgsurface;

	//Le code 0 ne peut evidemment etre retenu...
	ajoutetraits("#");
	int compteur=1;
	int ec;
	int max=0;
	arcTRANS* arcbouc;

	while (!fichier.eof()) {
		if (compteur%500==0) {
			cout<<".";
			if (compteur%20000==0) {
				cout<<compteur<<":"<<arcs.dernier<<":"<<Slesclefs.dernier<<endl;
			}
		}
		compteur++;
		//On lit trois par trois
		fichier.getline((char*)surface,1000);
		fichier.getline((char*)lemme,1000);
		fichier.getline((char*)traits,1000);
		strcat((char*)traits,"+");
		lgsurface=strlen((char*)surface);
		//On decoupe les traits selon le "+"
		lglemme=liretraits((uchar*)lemme,(char*)traits+1);
		//On recupere le noeud initial

		VECTA<arcTRANS*>& ch=initiaux[surface[0]];
		arcbouc=NULL;
		for (i=0;i<ch.dernier;i++) {
			if (ch[i]->l==lemme[0]) {
				arcbouc=ch[i];
				break;
			}
		}

		ec=rechercheclef(clef);

		if (arcbouc==NULL) {
			//Il faut le creer
			arcbouc=new arcTRANS(surface[0],lemme[0],ec);
			arcbouc->id=arcs.dernier;
			arcs.ajoute(arcbouc);
			int nt=0;
			int poss=surface[0];
			for (;nt<initiaux[poss].dernier;nt++) {
				if (arcbouc->l>initiaux[poss][nt]->l)
					break;
			}
			initiaux[poss].insere(nt,arcbouc);
		}
		//Ensuite on parcours les chaines
		char verif=OUI;
		maxlg=lglemme;
		if (maxlg<lgsurface)
			maxlg=lgsurface;
		char idtype=NON;
		char debut=OUI;
		arcTRANS* dernierarc=NULL;
		for (i=1;i<maxlg;i++) {
			if (i>=lgsurface)
				clef[0]=cfinmot;
			else
				clef[0]=surface[i];
			if (i>=lglemme)
				clef[1]=cfinres;
			else
				clef[1]=lemme[i];
			e=-1;

			dernierarc=arcbouc;
			ec=rechercheclef(clef);

			if (verif==OUI)
				e=arcbouc->recherche(arcs,ec);
			if (e==-1) {
				//cet arc est inconnu, nous devons le creer
				arcTRANS* an=new arcTRANS(clef[0],clef[1],ec);
				an->type=idtype;

				an->id=arcs.dernier;
				an->pere=arcbouc->id;
				arcbouc->ajoute(arcs,an);

				arcs.ajoute(an);
				//Desormais tous les arcs sont nouveaux...
				verif=NON;
				arcbouc=an;
			}
			else
				//On connait l'arc suivant
				arcbouc=arcs[e];


			if (clef[1]==cfinres && debut==OUI) {
				debut=NON;
				idtype=OUI;
			}				
		}		
		if (dernierarc!=NULL)
			finaux->ajoute((char*)clef,dernierarc,ec);
		max++;
	}

	cout<<compteur<<":"<<arcs.dernier<<":"<<Slesclefs.dernier<<" "<<max<<endl;
	i=0;
	fichier.close();
	while (minimisation(i)) {
		i++;
	}
	for (i=0;i<arcs.dernier;i++)
		arcs[i]->appel=0;

	//Verification
	compteur=1;
	char erreur=NON;
	ifstream fichierverif(nom,modeOuverture);
	while (!fichierverif.eof()) {
		if (compteur%500==0) {
			cout<<".";
			if (compteur%20000==0) {
				cout<<"VERIF: "<<compteur<<":"<<endl;
			}
		}
		compteur++;
		//On lit trois par trois
		fichierverif.getline((char*)surface,1000);
		fichierverif.getline((char*)lemme,1000);
		fichierverif.getline((char*)traits,1000);
		strcat((char*)traits,"+");
		lgsurface=strlen((char*)surface);
		//On decoupe les traits selon le "+"
		lglemme=liretraits((uchar*)lemme,(char*)traits+1);
		//On recupere le noeud initial

		VECTA<arcTRANS*>& ch=initiaux[surface[0]];
		arcbouc=NULL;
		for (i=0;i<ch.dernier;i++) {
			if (ch[i]->l==lemme[0]) {
				arcbouc=ch[i];
				break;
			}
		}

		if (arcbouc==NULL) {
			cout<<"Erreur de relecture"<<endl;
			erreur=OUI;
			break;
		}

		arcbouc->appel=-100;
		//Ensuite on parcours les chaines
		maxlg=lglemme;
		if (maxlg<lgsurface)
			maxlg=lgsurface;
		for (i=1;i<maxlg;i++) {
			if (i>=lgsurface)
				clef[0]=cfinmot;
			else
				clef[0]=surface[i];
			if (i>=lglemme)
				clef[1]=cfinres;
			else
				clef[1]=lemme[i];
			ec=rechercheclef(clef);
			e=arcbouc->recherche(arcs,ec);
			if (e==-1) {
				cout<<"Erreur de relecture: "<<arcbouc->id<<" "<<surface<<" "<<lemme<<":"<<i<<endl;
				erreur=OUI;
				break;
			}
			else
				arcbouc=arcs[e];

			arcbouc->appel=-100;
		}
		if (erreur==OUI)
			break;
	}
	arcsutiles=0;
	for (i=0;i<arcs.dernier;i++) {
		if (arcs[i]->appel==-100) {
			arcs[i]->id=arcsutiles;
			arcsutiles++;
		}
	}
	cout<<"To save: "<<arcsutiles<<endl;
}

class cpltrans {
public:
	int id;
	uchar clef[3];
	cpltrans(arcTRANS* a) {
		id=a->id;
		clef[0]=a->u;
		clef[1]=a->l;
		clef[2]=0;
	}
};

bool comptrans(const cpltrans& premier,const cpltrans& second) {
	if (premier.clef[0]<second.clef[0])
		return true;
	if (premier.clef[0]==second.clef[0])
		if (premier.clef[1]<second.clef[1])
			return true;
	return false;
}

void BuildAutomaton::sauvegarde(const char* nom) {
	char ch[20];
	int i;

	ofstream fichier(nom,ios::out|ios::binary);
	sprintf(ch,"%d\n",lsubtraits.size());
	fichier<<ch;
	for (i=0;i<lsubtraits.size();i++) 
		fichier<<lsubtraits[i].c_str()<<'\n';
	fichier<<'\n';
	for (i=0;i<256;i++) {
		for (int j=0;j<initiaux[i].dernier;j++) {
			sprintf(ch,"i%da%d",i,initiaux[i][j]->id);
			fichier<<ch;
		}
	}

	fichier<<"e"<<arcsutiles;
	for (i=0;i<arcs.dernier;i++) {
		arcTRANS* a=arcs[i];
		if (a->appel!=-100)
			continue;
		if (a->type==OUI) {
			if (a->u==cfinmot)
				sprintf(ch,"t%c",a->l);
			else
				sprintf(ch,"T%c%c",a->u,a->l);
		}
		else {
			if (a->u==a->l)
				sprintf(ch,"x%c",a->l);
			else
				sprintf(ch,"u%c%c",a->u,a->l);
		}
		vector<cpltrans> liste;
		for (int j=0;j<a->suivants.dernier;j++) {
			cpltrans cpl(arcs[a->suivants[j]]);
			liste.push_back(cpl);
		}

		sort(liste.begin(),liste.end(),comptrans);
		string res=ch;
		uchar bs[100];
		for (int l=0;l<liste.size();l++) {
			int diff=liste[l].id-a->id;
			uchar xc;
			int lg=base255(abs(diff),bs);	
			if (diff<0)
				xc=76+lg;
			else
				xc=108+lg;
			res+=xc;
			res+=(char*)bs;
		}
		fichier<<res.c_str();
	}
}

		

void compileautomaton(string nom,string sauve,int nbarcs) {
	BuildAutomaton ba(nbarcs);
	ba.compileautomaton(nom.c_str());
	ba.sauvegarde(sauve.c_str());
}
