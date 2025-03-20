#ifndef transduct_h
#define transduct_h

class transducteur;
class automate;
class arc;


class valdico {
public:

	string clef;
	int val;

	valdico(char* c, int v) {
		clef=c;
		val=v;
	}
};


class dicoperso {
public:

	VECTA<valdico*> chaines[100];

	void affecte(char* c,int);

	~dicoperso() {
		for (int i=0;i<100;i++)
			chaines[i].nettoie();
	}

	int hachage(char* c);
	valdico* recherche(char*,int);
	valdico* operator [](char* c) {
		int hache=hachage(c);
		return recherche(c,hache);
	}
};

class arctrans {
public:
	unsigned char u;
	unsigned char l;
	int id;
	int* next;

	virtual char type() {
		return 0;
	}

	arctrans() {
		u=0;
		l=0;
		id=-1;
		next=NULL;
	}
	~arctrans() {
		delete[] next;
	}
	virtual char test(unsigned char);
	virtual void add(arc* a) {}
};

class arctransarc : public arctrans {
public:
	arc* a;
	
	char type() {
		return 1;
	}

	arctransarc() {
		a=NULL;
	}
	char test(unsigned char);
	void add(arc* ar) {
		a=ar;
	}
};

class parcoursarc {
public:
	char marque;
	arctrans* a;
	parcoursarc* pere;
	int idx;
	char premier;
	parcoursarc(arctrans* aa=NULL,char p=0) {
		pere=NULL;
		idx=-1;
		marque=0;
		a=aa;
		premier=p;
	}
	VECTA<parcoursarc*> pile;
	void ajoute(parcoursarc* a) {
		pile.ajoute(a);
		a->pere=this;
	}

	void raz() {
		pile.raz();
	}
};


#define TVecteur unsigned __int64
const TVecteur Un=1;
#define gBitVect 64
#define gBitVectDivise 6 // = 64=2^6
#define NON 0
#define OUI 1


class lecteur {
public:
	char* chaine;
	int index;

	int pointeur;
	char attente[1000];
	int iattente;
	char fin;

	lecteur(char* c) {
		fin=0;
		index=0;
		chaine=c;
		pointeur=0;
		iattente=0;
		attente[iattente]=0;
	}

	void metattente(char c) {
		attente[iattente++]=c;
	}

	virtual char end() {
		return fin;
	}
	virtual char lit() {
		if (iattente==0) {
			if (chaine[pointeur]==0)
				fin=1;
			return chaine[pointeur++];
		}
		else {
			if (iattente==1)
				if (chaine[pointeur]==0)
					fin=1;
			return attente[--iattente];
		}
	}
};

class lecteurfichier: public lecteur {
public:

	ifstream fichier;

	lecteurfichier(char* n): lecteur(n),fichier(n,ios::in|ios::binary) {
		chaine=n;
	}

	char end() {
		return fichier.eof();
	}

	char lit() {
		if (iattente==0)
			return fichier.get();
		else
			return attente[--iattente];
	}
};

class transducteur {
public:
	
	char * nom;
	char trouve;
	VECTA<char*> results;
	VECTA<char*> terminaisons;
	VECTA<arctrans*> arcs;
	VECTA<int> initiaux[256];
	vector<string> lexiques;
	automate* aterminaison;
	VECTA<automate*> rgx;
	
	int ajouteterminaison(char* t,char);
	transducteur(char* n);
	int lirecode(ifstream& fichier,unsigned char& c);
	void lookup(char* mot);
	void cherche(arctrans* a,char* mot,int i,int s,char* r,int ir);
	arctrans* ajoutemot(arctrans* a,char* mot,int i,int id,int&);
	char cherchearcs(VECTA<parcoursarc*>& courants,unsigned char c,int i,
		VECTA<parcoursarc*>& suivants,VECTA<parcoursarc*>& chaines,char);
	void maj(automate*);
	void liretrans();
	void lire(lecteur*);
	void parscoursdesarcs(arctrans* a,char* s,char* res,int);
	lecteur* fournir(char,char*);
	~transducteur();
}; 


#endif
