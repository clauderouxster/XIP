#ifdef NTMCONNECTION
//Necessaire pour le convertisseur FST/TRANS

#include "applyloc.h"
#include "lktypes.h"
    
#include "networks.h"

extern "C" {
NVptr no_script_get_networks (char *argument);
extern HEAPptr FstBaseExport ARC_HEAP;
int get_label_utf8(id_type id, char* buffer,int escape_p);
}
void initialisation_fst();

#ifdef NTMCONNECTION
class arcnum;
#endif

#endif

//-------------------------BASE-------------------------

#include "parsbin.h"
#include <map>
#include <algorithm>
void lance_erreur(char* message);
extern ostream* flot_erreur;

#define cfinmot 1
#define cfinres 2
#define epsilon 4
#define cANY 5


time_t   tempsinitial;
void Trim(char* chaine);

int base255(int nb,uchar* s) {
	char ch[100];
	s[0]=0;
	int p=0;
	while (nb!=0) {
		uchar v=1+nb%255;
		sprintf_s(ch,100,"%c%s",v,s);
		strcpy_s((char*)s,100,ch);
		nb=nb/255;
	}
	return strlen((char*)s);
}


class arcTRANS {
public:
	int l; //lemme
	int pere;
	int id;
	int clef;
	int garde;
	VECTA<int> suivants;

	uchar u; //surface
	char type;
	char appel;
	char fin;
	arcTRANS* sameset;


	arcTRANS(uchar U, int L,int c) {
		sameset=NULL;
		pere=-1;
		u=U;
		l=L;
		type=0;
		appel=0;
		clef=c;
		garde=-1;
		fin=0;
	}

	void ajoute(VECTA<arcTRANS*>& arcs,arcTRANS* a) {
		int i=0;
		for (;i<suivants.dernier;i++) {
			if (a->clef>arcs[suivants[i]]->clef)				
				break;
		}
		suivants.insere(i,a->id,suivants.taille);
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




//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
class elarc {
public:

	VECTA<arcTRANS*> clef;
	int ec;
	int id;
	elarc(int m,int l,int e) : clef(m) {
		ec=e;
		id=l;
	}
	void ajoute(arcTRANS* a) {
		clef.ajoute(a,clef.taille<<2);
	}
};
//----------------------------------------------------------------------------------------------------
class dicoarc {
public:
	int increment;
	map<string, elarc* > clefs;
	int compteur;


	dicoarc(int inc) {
		increment=inc;
		compteur=0;
	}

	void ajoute(string c,arcTRANS* a,int ec) {
		elarc* va=clefs[c];
		if (va==NULL) {
			va=new elarc(increment,a->id,ec);
			clefs[c]=va;			
		}
		va->ajoute(a);
		compteur++;
	}


	~dicoarc() {
		map<string,elarc*>::iterator i;
		for (i=clefs.begin();i!=clefs.end();i++) {
			elarc* va=i->second;
			if (va!=NULL)
				delete va;
		}
	}
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
class BuildAutomaton {
public:

	VECTA<arcTRANS*> arcs;
	VECTA<arcTRANS*> initiaux[256];
	dicoarc* finaux;
	VECTA<char*> Slesclefs;
	short lesclefs[256][256];
	int arcsutiles;
	char longuetable;

	map<string,int> subtraits;
	vector<string> lsubtraits;
	int liretraits(uchar* lemme,char* curseur);
	void compileautomaton(const char* nom);
	void affiche(arcTRANS* a, int indent);
	void affichage();
	int nbtraits;
	int maxsize;
	void sauve(ofstream& fichier,arcTRANS* a,int indent);
	void sauvetout(string&);


	BuildAutomaton(int m) : arcs(m),Slesclefs(1000) {
		longuetable=NON;
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

	int ajoutetraits(char* curseur) {
		int id=subtraits[curseur];
		if (id==0) {
			subtraits[curseur]=nbtraits;
			lsubtraits.push_back(curseur);
			id=nbtraits;
			nbtraits++;
		}
		return id;
	}

	char parcours(arcTRANS* a,string& clef,int);

	int rechercheclef(uchar* cl) {
		int ec=lesclefs[cl[0]][cl[1]];
		if (ec==-1) {
			ec=Slesclefs.dernier;
			lesclefs[cl[0]][cl[1]]=ec;
			char* ss=new char[3];
			strcpy_s(ss,3,(char*)cl);
			Slesclefs.ajoute(ss,Slesclefs.taille);
		}

		return ec;
	}

	int rechercheclef(uchar u,uchar l) {
		return lesclefs[u][l];
	}

	int minimisation(int);
	void sauvegarde(const char* nom);
	char afficher;
#ifdef NTMCONNECTION
	int parcoursdesarcsfst(vector<arcnum*>& premiers,map<string,arcnum*>& lesarcs, 
						STATE* s,char premier,int,char*,int);
#endif
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

char BuildAutomaton::parcours(arcTRANS* a,string& laclef,int app) {
	if (a==NULL)
		return OUI;

	if (a->garde==app) {		
		char base[20];
		sprintf_s(base,20,"_%d",a->id+Slesclefs.dernier+1);
		laclef+=base;
		return OUI;
	}

	a->garde=app;
	laclef+=a->u;
	laclef+=a->l;
	arcTRANS* an;
	for (int i=0;i<a->suivants.dernier;i++) {
		an=arcs[a->suivants[i]];
		laclef+='<';
		if (parcours(an,laclef,app)==NON)
			return NON;
		laclef+='>';
	}
	return OUI;
}

//----------------------------------------------------------------------------------------------------

int BuildAutomaton::liretraits(uchar* lemme,char* curseur) {
	char* pt=strchr(curseur,'+');
	int plemme=strlen((char*)lemme);
	int id;
	//On rajoute un fin de chaine
	lemme[plemme++]=cfinres;

	while (pt!=NULL) {
		*pt=0;
		id=ajoutetraits(curseur);
		lemme[plemme++]=(uchar)id;
		curseur=pt+1;
		pt=strchr(curseur,'+');
	}
	lemme[plemme]=0;
	return plemme;
}

//----------------------------------------------------------------------------------------------------

int compteclefm=0;
//On va rattacher les arcs communs
int BuildAutomaton::minimisation(int app) {
	char boucle=OUI;
	int comptemodif=0;
	int comptedest=0;
	dicoarc* nouveau=new dicoarc(3);	
	int i;
	int max=0;
	int cpvaldiff=0;
	char buffer[100];
	map<int,string> signatures;


	if (afficher==OUI)
		cout<<app<<": Minimisation: ";
	string laclef;
	elarc* va;

	map<string,elarc*>::iterator f=finaux->clefs.begin();
	for (;f!=finaux->clefs.end();f++) {
		va=f->second;
		VECTA<arcTRANS*>& lesarcs=va->clef;
		arcTRANS* courant;
		arcTRANS* pere;
		arcTRANS* fils;
		int idx;
		int debut=0;
		int idcommun=-1;
		int uneclef=va->ec;
		
		arcTRANS* base=lesarcs[debut++];
		//On va donc chercher l'element vers lequel pointe base[clef]
		while (debut<lesarcs.dernier) {
			idx=base->recherche(arcs,uneclef);
			fils=arcs[idx];
			if (fils->suivants.dernier<=1) {
				base->appel=-3;
				idcommun=idx;
				pere=arcs[base->pere];
				if (pere!=NULL) {
					sprintf_s(buffer,100,"%c%c_%d",base->u,base->l,cpvaldiff);
					nouveau->ajoute(buffer,pere,base->clef);
					pere->appel=app+1;
				}
				break;
			}
			arcTRANS* base=lesarcs[debut++];
		}

		dicoarc* multiple=new dicoarc(100);		
		for (i=0;i<lesarcs.dernier;i++) {
			courant=lesarcs[i];	
			
			//Dans le cas d'un arc unique
			idx=courant->rechercheidx(arcs,uneclef);
			fils=arcs[courant->suivants[idx]];

			if (fils->suivants.dernier<=1 && idcommun!=-1) {
				courant->suivants.vecteur[idx]=idcommun;
				courant->appel=-2;
				//On garde alors les nouveaux finaux
				pere=arcs[courant->pere];
				if (pere!=NULL) {
					sprintf_s(buffer,100,"%c%c_%d",courant->u,courant->l,cpvaldiff);
					nouveau->ajoute(buffer,pere,courant->clef);
					pere->appel=app+1;
				}
				comptemodif++;
			}
			else {
				//Il faut calculer les clefs des elements qui partent de courant
				laclef=signatures[fils->id];
				courant->appel=-1;
				if (laclef=="") {
					if (parcours(fils,laclef,fils->id)==OUI)
						signatures[fils->id]=laclef;
					else
						laclef="";
				}
				if (laclef!="") {
					multiple->ajoute(laclef,courant,-1);
					compteclefm++;
				}
			}
		}
		cpvaldiff++;
		map<string,elarc*>::iterator j=multiple->clefs.begin();
		for (;j!=multiple->clefs.end();j++) {
			VECTA<arcTRANS*>& pmult=j->second->clef;
			if (pmult.dernier<=1)
				continue;
			if (max<pmult.dernier)
				max=pmult.dernier;
			base=pmult[0];
			base->appel=-3;
			idcommun=base->recherche(arcs,uneclef);
			pere=arcs[base->pere];
			if (pere!=NULL) {
				sprintf_s(buffer,100,"%c%c_%d",base->u,base->l,cpvaldiff);
				nouveau->ajoute(buffer,pere,base->clef);
				pere->appel=app+1;
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
					sprintf_s(buffer,100,"%c%c_%d",courant->u,courant->l,cpvaldiff);
					nouveau->ajoute(buffer,pere,courant->clef);
					pere->appel=app+1;
				}
				comptemodif++;
				comptedest++;
			}
			cpvaldiff++;
		}
		delete multiple;
	}	
	time_t tempsinter;
	time(&tempsinter);
	double duree = difftime( tempsinter,tempsinitial);

	if (afficher==OUI)
		cout<<"Modified arcs: "<<comptemodif<<"\tSub-automata: "<<comptedest<<"\tDuration: "<<duree<<"s "<<Endl;
	delete finaux;
	finaux=nouveau;
	return comptemodif;
}

//----------------------------------------------------------------------------------------------------

void blanc(int indent) {
	for (int i=0;i<indent;i++)
		cout<<" ";
}

void BuildAutomaton::affiche(arcTRANS* a, int indent) {
	char ch[100];
	char c;

	if (a->type==NON)
		sprintf_s(ch,100,"%c/%c:%d ",a->u,a->l,a->id);
	else {
		if (a->u!=cfinmot)
			c=a->u;
		else
			c='.';
		sprintf_s(ch,100,"%c/%s:%d ",c,lsubtraits[a->l].c_str(),a->id);
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
		cout<<Endl;
}

void BuildAutomaton::affichage() {
	for (int i=0;i<256;i++) {
		for (int j=0;j<initiaux[i].dernier;j++) {
			arcTRANS* a=initiaux[i][j];
			affiche(a,0);
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void BuildAutomaton::sauve(ofstream& fichier,arcTRANS* a,int indent) {
	if (a->appel==-90) {
		uchar bs[100];
		int n=base255(a->id,bs);
		static char c[]={34,35,36,37,38,39};
		fichier<<c[n]<<bs;
	}

	if (a->appel!=-100)
		return;

	a->appel=-90;
	fichier<<indent<<a->u<<a->l;
	for (int l=0;l<a->suivants.dernier;l++)
		sauve(fichier,arcs[a->suivants[l]],indent+1);
}

void BuildAutomaton::sauvetout(string& nom) {
	char ch[100];
	ofstream fichier(nom.c_str(),ios::out|ios::binary);
	sprintf_s(ch,100,"%d\n",lsubtraits.size());
	fichier<<ch;
	for (size_t i=0;i<lsubtraits.size();i++) 
		fichier<<lsubtraits[i].c_str()<<'\n';
	fichier<<'\n';
	fichier<<arcsutiles<<'\n';
	for (int i=0;i<256;i++) {
		for (int j=0;j<initiaux[i].dernier;j++) {
			arcTRANS* a=initiaux[i][j];
			sauve(fichier,a,0);
		}
	}
}

//----------------------------------------------------------------------------------------------------

void lireuneligne(ifstream& fichier,uchar* surface,int max) {
	int i=0;
	uchar c=fichier.get();
	while (!fichier.eof() && (c=='\n' || c=='\r'))
		c=fichier.get();
	while (!fichier.eof() && i<max-1) {
		if (c=='\n' || c=='\r')
			break;
		surface[i++]=c;
		c=fichier.get();
	}
	surface[i]=0;
}


void BuildAutomaton::compileautomaton(const char* nom) {

	ifstream fichier(nom,modeOuverture);
	if (fichier.fail()) {
		string err="Unknown file: ";
		err+=nom;
		*flot_erreur<<err.c_str()<<Endl;
		lance_erreur((char*)err.c_str());
	}

	uchar surface[1000];
	uchar lemme[1000];
	uchar traits[1000];
	
	surface[0]=0;
	lemme[0]=0;
	traits[0]=0;


	uchar clef[]={0,0,0};
	int i,e;
	int maxlg;
	int lglemme;
	int lgsurface;

	//Le code 0 ne peut evidemment etre retenu...
	ajoutetraits("#");
	//Ni le code 1...
	ajoutetraits("&");
	int compteur=1;
	int ec;
	int nblignes=0;
	arcTRANS* arcbouc;

	while (!fichier.eof()) {
		if (afficher==OUI) {
			if (compteur%500==0) {
				cout<<".";
				if (compteur%20000==0) {
					cout<<compteur<<":"<<arcs.dernier<<":"<<Slesclefs.dernier<<Endl;
				}
			}
			compteur++;
		}

		//On lit trois par trois
		lireuneligne(fichier,surface,1000);
		lireuneligne(fichier,lemme,1000);
		lireuneligne(fichier,traits,1000);
		Trim((char*)surface);
		Trim((char*)lemme);
		Trim((char*)traits);
		nblignes+=3;

		if (surface[0]==0 && lemme[0]==0 && traits[0]==0)
			break;
		
		if (surface[0]==0 || lemme[0]==0 || traits[0]==0) {
			string err="Wrong file structure line: ";
			sprintf_s((char*)surface,1000,"%d",nblignes);
			err+=(char*)surface;
			lance_erreur((char*)err.c_str());
		}
		strcat_s((char*)traits,1000-strlen((char*)traits),"+");		
		lgsurface=strlen((char*)surface);
		//On decoupe les traits selon le "+"
		lglemme=liretraits((uchar*)lemme,(char*)traits+1);

		if (lgsurface>=lglemme)
			surface[lgsurface++]=cfinmot;
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
			arcs.ajoute(arcbouc,arcs.taille);
			int nt=0;
			int poss=surface[0];
			for (;nt<initiaux[poss].dernier;nt++) {
				if (arcbouc->l>initiaux[poss][nt]->l)
					break;
			}
			initiaux[poss].insere(nt,arcbouc,initiaux[poss].taille);
		}
		//Ensuite on parcourt les chaines
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
				clef[1]=1;
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

				arcs.ajoute(an,arcs.taille);
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
	}

	if (afficher==OUI)
		cout<<Endl<<Endl<<"Word Forms: "<<compteur<<"\tArcs: "<<arcs.dernier<<"\tKeys: "<<Slesclefs.dernier<<"\tFinal Arcs: "<<finaux->clefs.size()<<Endl<<Endl;
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
		if (afficher==OUI) {
			if (compteur%500==0) {
				cout<<".";
				if (compteur%20000==0) {
					cout<<"VERIF: "<<compteur<<":"<<Endl;
				}
			}
			compteur++;
		}
		//On lit trois par trois
		//fichierverif.getline((char*)surface,1000);
		//fichierverif.getline((char*)lemme,1000);
		//fichierverif.getline((char*)traits,1000);
		lireuneligne(fichierverif,surface,1000);
		lireuneligne(fichierverif,lemme,1000);
		lireuneligne(fichierverif,traits,1000);
		Trim((char*)surface);
		Trim((char*)lemme);
		Trim((char*)traits);
		if (surface[0]==0 || lemme[0]==0 || traits[0]==0)
			break;

		strcat_s((char*)traits,1000-strlen((char*)traits),"+");
		lgsurface=strlen((char*)surface);
		//On decoupe les traits selon le "+"
		lglemme=liretraits((uchar*)lemme,(char*)traits+1);
		if (lgsurface>=lglemme)
			surface[lgsurface++]=cfinmot;

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
			cout<<"Erreur de relecture"<<Endl;
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
				clef[1]=1;
			else
				clef[1]=lemme[i];
			ec=rechercheclef(clef);
			e=arcbouc->recherche(arcs,ec);
			if (e==-1) {
				cout<<"Unbalanced automaton (see Xerox for information): "<<arcbouc->id<<" "<<surface<<" "<<lemme<<":"<<i<<Endl;
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
	if (afficher==OUI)
		cout<<Endl<<"To save: "<<arcsutiles<<Endl;
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

class cpltrans {
public:
	int id;
	int clef[3];
	cpltrans(arcTRANS* a) {
		id=a->id;
		clef[0]=a->u;
		clef[1]=a->l;
		clef[2]=0;
	}
};
//-----------------------------------------------------------------------------------------

bool comptrans(const cpltrans& premier,const cpltrans& second) {
	if (premier.clef[0]<second.clef[0])
		return true;
	if (premier.clef[0]==second.clef[0])
		if (premier.clef[1]<second.clef[1])
			return true;
	return false;
}
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
void BuildAutomaton::sauvegarde(const char* nom) {
	char chx[20];
	char* ch;
	int i;
#ifdef _DEBUG
	int compteurarc=0;
#endif
	ofstream fichier(nom,ios::out|ios::binary);
	sprintf_s(chx,20,"%d\n",lsubtraits.size());
	fichier<<chx;
	for (size_t ii=0;ii<lsubtraits.size();ii++) 
		fichier<<lsubtraits[ii].c_str()<<'\n';
	fichier<<'\n';
	for (i=0;i<256;i++) {
		for (int j=0;j<initiaux[i].dernier;j++) {
			sprintf_s(chx,20,"i%da%d",i,initiaux[i][j]->id);
			fichier<<chx;
		}
	}

	fichier<<"e"<<arcsutiles;
	for (i=0;i<arcs.dernier;i++) {
		arcTRANS* a=arcs[i];
		if (a->appel!=-100)
			continue;
		ch=chx;

#ifdef _DEBUG
	compteurarc++;
#endif

		if (a->sameset!=NULL) {
			ch[0]='S';
			ch=chx+1;
		}
		if (a->type==OUI) {
			if (a->u==cfinmot)
				sprintf_s(ch,20,"t%c",a->l);
			else
				sprintf_s(ch,20,"T%c%c",a->u,a->l);
		}
		else {
			if (a->u==a->l)
				sprintf_s(ch,20,"x%c",a->l);
			else
				if (longuetable==NON)
					sprintf_s(ch,20,"u%c%c",a->u,a->l);
				else {
					if (a->fin==1)
						sprintf_s(ch,20,"V%c%d",a->u,a->l);
					else
						sprintf_s(ch,20,"U%c%d",a->u,a->l);
				}
		}

		vector<cpltrans> liste;
		if (a->sameset!=NULL) {
			//Il s'agit d'un cas particulier, dans la conversion depuis un FST
			//On partage les memes ensembles d'arcs suivants
			cpltrans cpl(arcs[a->sameset->id]);
			liste.push_back(cpl);
		}
		else {
			for (int j=0;j<a->suivants.dernier;j++) {
				cpltrans cpl(arcs[a->suivants[j]]);
				liste.push_back(cpl);
			}
		}

		sort(liste.begin(),liste.end(),comptrans);
		string res=chx;
		uchar bs[100];
		for (size_t l=0;l<liste.size();l++) {
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

		

void compileautomaton(string nom,string sauve,int nbarcs,char affiche) {
	//Verification des dates des fichiers, pour eviter des compilations intempestives et inutiles
	//Si la date du fichier de base est plus recente que celle du fichier a generer, on recompile
	//Sinon on saute...

	if (affiche==NON) {
		int stinit=-1;
		int stcible=-1;
		struct stat scible;
		struct stat sinit;
		FILE* init=fopen((char*)nom.c_str(),"r");
		FILE* cible=fopen((char*)sauve.c_str(),"r");
		if (init!=NULL) {
			if (cible!=NULL) {
#if  defined(WIN32) | defined(APPLE) | defined(XIPFSTAT64)
				stinit=fstat(init->_file,&sinit);
				stcible=fstat(cible->_file,&scible);
#else
				stinit=fstat(init->_fileno,&sinit);
				stcible=fstat(cible->_fileno,&scible);
#endif
				fclose(cible);
			}
			fclose(init);
		}
		if (stinit!=-1 && stcible!=-1 && scible.st_mtime > sinit.st_mtime)
			return;
	}
	BuildAutomaton ba(nbarcs);
	ba.afficher=affiche;
	ba.compileautomaton(nom.c_str());
	ba.sauvegarde(sauve.c_str());
}

//-----------------------Conversion FST/TRANS-------------------------------------------------------------------
#ifdef NTMCONNECTION

class arcnum {
public:
	int id;
	ARC* a;
	arcTRANS* at;
	arcnum(int i,ARC* r) {
		id=i;
		a=r;
		at=NULL;
	}
};

int BuildAutomaton::parcoursdesarcsfst(vector<arcnum*>& premiers,
										map<string,arcnum*>& lesarcs, 
										STATE* s,
										char premier,
										int profondeur,
										char* garde,
										int lecourant) {

	ARC* a=s->arc.set;
	
	s->unused=1;
	
	static char internals[100];
	static uchar carcl[100];
	static char carcu[100];
	arcnum* an;
	int comptearc=0;

	while (a!=NULL) {		
		garde[profondeur]=0;
		if (a->unused==0)
			an=new arcnum(arcs.dernier,a); 
		id_type id = ARC_label(a);
		LABELptr lab=id_to_label(id);
		copy_label_name(a->label, UPPER, (char*)carcl, 99, TRUE, FALSE);
		copy_label_name(a->label, LOWER, carcu, 99, TRUE, FALSE);				
		uchar idu=carcu[0];
		int idl=-1;			
		int ajout=0;

		if (!idu)
			idu=cfinmot;

		if (lab->id==1 && carcu[0]=='?')
			//Le caractere ANY
			idu=cANY;

		//fin de chaine
		if (lab->arity==2) {
			if (carcu[0]=='0') {
				idu=cfinmot;
				if (afficher==2)
					cout<<garde<<endl;
			}
			else
				if (carcl[0]=='0')
					idl=1;
				else {
					if (afficher==1) {
						garde[profondeur]=carcu[0];
						garde[profondeur+1]=0;
						ajout=1;
					}
				}
		}
		else {						
			if (afficher) {
				garde[profondeur]=carcu[0];
				garde[profondeur+1]=0;
				ajout=1;
			}
		}

		if (idl==-1) {
			if (carcl[0]<=32)
				idl=ajoutetraits((char*)"_");	
			else
				idl=ajoutetraits((char*)carcl);	
		}
		sprintf_s(internals,100,"%x",a);
		if (a->unused==0) {
			int ec=rechercheclef(idu,idl);
			an->at=new arcTRANS(idu,idl,ec);
			an->at->appel=-100;
			an->at->id=an->id;
			an->at->pere=lecourant;
				
			arcs.ajoute(an->at);
			a->unused=1;
			
			if (STATE_final(a->destination))
				an->at->fin=1;

			if (arcs.dernier%1000==0) {
				cout<<".";
				if (arcs.dernier%15000==0)
					cout<<arcs.dernier<<endl;
			}

			if (premier==1)
				premiers.push_back(an);
			
			lesarcs[internals]=an;
			if (idu==cfinmot) {
				STATE* san=a->destination;
				if (san->arc.set!=NULL && san->arc.set->next!=NULL)
					an->at->u=epsilon;
			}
		}
		else
			an=lesarcs[internals];

		if (a->destination->unused==0)
			parcoursdesarcsfst(premiers,lesarcs,a->destination,0,profondeur+ajout,garde,an->id);

		comptearc++;
		a=a->next;
	}
	return comptearc;
}

void conversionfsttrans(char* fst,char* res) {
	map<string,arcnum*> lesarcs;
	vector<arcnum*> premiers;

	initialisation_fst();
	NVptr nv=no_script_get_networks (fst);
	//BLOCK* a=ARC_HEAP->first;
	int taille=nv->nets[0]->num_arcs+1;
	BuildAutomaton ba(taille);

	ba.ajoutetraits("#");
	ba.ajoutetraits("&");

	char garde[10000];
	ba.afficher=OUI;
	ba.parcoursdesarcsfst(premiers,lesarcs, nv->nets[0]->start.state,1,0,garde,-1);
	ba.longuetable=OUI;

	cout<<endl<<"Processing"<<endl;
	map<string,arcnum*>::iterator i;
	char internals[100];

	map<string,arcTRANS*> latrace;
	int nbset=0;
	for (i=lesarcs.begin();i!=lesarcs.end();i++) {
		arcnum* an=i->second;
		STATE* st=an->a->destination;
		if (st!=NULL) {
			ARC* a=st->arc.set;
			//static uchar carcl[100];
			//static char carcu[100];
			//copy_label_name(a->label, UPPER, (char*)carcl, 99, TRUE, FALSE);
			//copy_label_name(a->label, LOWER, carcu, 99, TRUE, FALSE);				
			//cout<<carcu<<":"<<carcl<<endl;
			sprintf_s(internals,100,"%x",st->arc.set);
			arcTRANS* ansame=latrace[internals];
			if (ansame!=NULL)
				an->at->sameset=ansame;
			else {
				latrace[internals]=an->at;
				while (a!=NULL) {
					sprintf_s(internals,100,"%x",a);	
					arcnum* r=lesarcs[internals];
					an->at->ajoute(ba.arcs,r->at);
					a=a->next;
				}
			}
		}
	}
	for (size_t j=0;j<premiers.size();j++) {
		int possd=premiers[j]->at->u;
		int possf=premiers[j]->at->u;
		if (premiers[j]->at->u==cANY) {
			possd=33;
			possf=255;
		}
		int nt=0;
		for (int poss=possd;poss<=possf;poss++) {
			for (;nt<ba.initiaux[poss].dernier;nt++) {
				if (premiers[j]->at->l>ba.initiaux[poss][nt]->l)
					break;
			}
			ba.initiaux[poss].insere(nt,premiers[j]->at,ba.initiaux[poss].taille);
		}
	}
	cout<<endl<<"Saving in "<<res<<endl;
	ba.arcsutiles=ba.arcs.dernier;
	ba.sauvegarde(res);
}


#endif
