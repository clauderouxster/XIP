#ifndef createau_h


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


	arcTRANS(uchar U, int L,int c) {
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

	string nom;
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
			strcpy(ss,(char*)cl);
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
	void ajoutemot(uchar* surface, uchar* lemme, uchar* traits);
#ifdef NTMCONNECTION
	int parcoursdesarcsfst(vector<arcnum*>& premiers,map<string,arcnum*>& lesarcs, 
						STATE* s,char premier,int,char*);
#endif
};

#endif
