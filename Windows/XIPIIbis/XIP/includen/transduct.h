#ifndef transduct_h
#define transduct_h

class GlobalParseurTrans;
class GlobalParseurHMM;
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
	int u;
	int l;
	int id;
	int* next;
	char fin;
	int copie;
#ifdef _DEBUG
	int pos;
	int passage;
	void print(GlobalParseurTrans* parseur,int,int max=3);
	void affichearc(GlobalParseurTrans* parseur,int);
#endif

	virtual char type() {
		return 0;
	}

	arctrans() {
#ifdef _DEBUG
		passage=0;
#endif
		u=0;
		l=0;
		id=-1;
		next=NULL;
		fin=0;
		copie=-1;
	}
	~arctrans() {
		if (copie==-1)
			delete[] next;
	}
	virtual char test(unsigned char,bool);
	virtual void add(arc* a) {}
	virtual void set(automate*) {}

	virtual int surface(uchar*) {
		return -1;
	}
	char element(GlobalParseurTrans*,word_offset*,char c,string& tr,vector<string>& terminaisons);
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
	char test(unsigned char,bool);
	void add(arc* ar) {
		a=ar;
	}
};

class arctransarcbase : public arctransarc {
public:
	automate* alemme;
	
	arctransarcbase() {
		a=NULL;
		alemme=NULL;
	}
	void set(automate* e) {
		alemme=e;
	}
	int surface(uchar* m) {
		alemme->recherchemax(m);
		return alemme->maxpos;
	}
};

typedef enum {ARCCOURANT,ARCNORM,ARCGUESSER} TYPEPARCOURS;
typedef enum {A_RIEN, A_LC,A_RC} TYPETESTARC;

class parcoursarc {
public:
	arctrans* a;
	parcoursarc* pere;
	int idx;
	int position;

	char type_epsilon;
	char premier;
	char reutiliser;
	char marque;
	TYPEPARCOURS origine;

	arctrans* lcrc;
	TYPETESTARC lcOUrc;

	parcoursarc(arctrans* aa,char p) {
		lcrc=NULL;
		lcOUrc=A_RIEN;
		origine=ARCCOURANT;
		type_epsilon=0;
		position=-1;
		pere=NULL;
		idx=-1;
		marque=0;
		a=aa;
		premier=p;
		reutiliser=NON;
	}
	VECTA<parcoursarc*> pile;
	void ajoute(parcoursarc* a) {
		pile.ajoute(a);
		a->pere=this;
	}

	void init(arctrans* aa,char p) {
		lcrc=NULL;
		lcOUrc=A_RIEN;
		pere=NULL;
		idx=-1;
		marque=0;
		a=aa;
		premier=p;
		reutiliser=NON;
		type_epsilon=0;
	}

	void raz() {
		lcrc=NULL;
		lcOUrc=A_RIEN;
		pile.raz();
		pere=NULL;
		idx=-1;
		marque=0;
		a=NULL;
		premier=-1;
		reutiliser=NON;
		type_epsilon=0;
	}
};



class lecteur {
public:
	long droit;
	long droitcar;
	bool utf8;

	int longueur;
	int longueurcar;

	unsigned char* chaine;
	int index;
	long gauche;
	long gauchecar;
	GlobalParseurTrans* courant;

	int debut;
	int buffcourant;
	int buffer[1000];
	int coupe;
	int parcours;


	int pointeur;
	int attente[1000];
	int iattente;
	char fin;
	int code;


	void ajoute(int c) {
		if (buffcourant==coupe*2) {
			memcpy(buffer,buffer+coupe,coupe*sizeof(int));
			buffcourant=coupe;
			debut-=coupe;
		}
		buffer[buffcourant++]=c;
	}

	void init(int pos) {
		parcours=pos;
	}

	int prec() {
		if (parcours<0)
			return -1;
		return buffer[parcours--];
	}

	virtual void raccourcir(int c) {
		droit--;
		droitcar--;
		longueur--;
		longueurcar--;
		buffcourant--;
		pointeur--;
	}

	lecteur(unsigned char* c) {
		utf8=false;
		buffcourant=0;
		debut=0;
		coupe=500;

		courant=NULL;
		gauche=0;
		droit=0;
		gauchecar=0;
		droitcar=0;
		fin=0;
		index=0;
		chaine=c;
		pointeur=0;
		iattente=0;
		attente[iattente]=0;
		longueur=0;
		longueurcar=0;
	}

	void metattente(int c) {
		attente[iattente++]=c;
		raccourcir(c);
	}


	virtual void raz() {
		longueur=0;
		longueurcar=0;	
		debut=buffcourant;
	}

	long Gauche() {
		return droit-longueur;
	}

	long Droite() {
		return droit;
	}

	long GaucheCar() {
		return droitcar-longueurcar;
	}

	long DroiteCar() {
		return droitcar;
	}

	virtual char end() {
		return fin;
	}
	virtual int lit() {
		droit++;
		droitcar++;
		longueur++;
		longueurcar++;
		if (iattente==0) {
			if (chaine[pointeur]==0)
				fin=1;
			code=chaine[pointeur++];
			ajoute(code);
		}
		else {
			if (iattente==1)
				if (chaine[pointeur]==0)
					fin=1;
			code=attente[--iattente];
			buffcourant++;
			pointeur++;
		}
		return code;
	}
};


class lecteurutf8 : public lecteur {
public:

	VECTA<uchar> longueurs;

	lecteurutf8(uchar* n) : lecteur(n) {
		utf8=true;
		longueur=0;		
	}

	void raccourcir(int c) {
		longueurcar--;
		droit-=longueurs[c];
		droitcar--;
		longueur-=longueurs[c];
		pointeur-=longueurs[c];
		buffcourant--;
	}

	virtual uchar extrait() {
		droit++;
		longueur++;
		return chaine[pointeur++];
	}

	void raz() {
		longueur=0;
		longueurcar=0;		
		debut=buffcourant;
	}


	int lituncar();

	virtual int lit() {
		if (iattente==0) {
			code=lituncar();
			if (code==0)
				fin=1;
			ajoute(code);
			return code;
		}
		else {
			droitcar++;
			code=attente[--iattente];
			if (code==0)
				fin=1;
			droit+=longueurs[code];
			longueur+=longueurs[code];
			pointeur+=longueurs[code];
			longueurcar++;
			buffcourant++;
			return code;
		}
	}
};

class lecteurfichierutf8: public lecteurutf8 {
public:

	FILE* fichier;

	lecteurfichierutf8(FILE* f,unsigned char* n): lecteurutf8(n) {
		fichier=f;
		utf8=true;
	}

	uchar extrait() {
		droit++;
		longueur++;
		return fgetc(fichier);
	}

	~lecteurfichierutf8() {
		fclose(fichier);
	}

	char end() {
		return feof(fichier);
	}

	int lit() {
		if (iattente==0) {
			code=lituncar();
			ajoute(code);
			return code;
		}
		else {
			code=attente[--iattente];
			droitcar++;
			droit+=longueurs[code];
			longueur+=longueurs[code];
			longueurcar++;
			buffcourant++;
			return code;
		}
	}
};


class lecteurfichier: public lecteur {
public:

	FILE* fichier;

	lecteurfichier(FILE* f,unsigned char* n): lecteur(n) {
		fichier=f;
	}


	~lecteurfichier() {
		fclose(fichier);
	}

	char end() {
		return feof(fichier);
	}

	int lit() {
		droit++;
		droitcar++;
		longueur++;
		longueurcar++;
		if (iattente==0) {
			code=fgetc(fichier);
			ajoute(code);
		}
		else {
			code=attente[--iattente];
			buffcourant++;
		}
		return code;
	}
};



class GlobalParseurTrans: public GlobalParseurHMM {
public:
	
	char * nom;
	char trouve;
	VECTA<int> LC;
	VECTA<int> RC;
	vector<string> terminaisons;
	map<string,int> mterminaison;
	map<string,string> remplacements;
	map<string,string> dynamiques;
	uchar Cremplacements[256];
	VECTA<arctrans*> arcs;
	VECTA<arctrans*> arcsnorm;
	VECTA<arctrans*> arcsguessers;
	VECTA<int> initiaux[256];
	VECTA<int> initiauxnorm[256];
	VECTA<int> initiauxguessers[256];
	vector<string> lexiques;
	vector<string> guessers;
	vector<string> guessersvoc;
	vector<string> fichiersvoc;
	vector<string> normaliseurs;
	vector<string> auvol;
	vector<string> compiler;
	VECTA<automate*> rgx;
	VECTA<automate*> rgxguess;
	lecteur* lecteurcourant;
	VECTA<uchar> alphabet;
	char* spaces[256];
	VECTA<char*> separators;
	char* caractereconversion[256];
	char nettoiespace;
	char nettoiesep;
	char nettoieconv;

	VECTA<parcoursarc*> TasDeparcoursarc;
	parcoursarc* fournirparcoursarc(arctrans*,char p=0);
	void nettoyerparcoursarc();
	
	uchar alpha(int c) {
		return alphabet[c];
	}
#ifdef _DEBUG
	void reinit(VECTA<arctrans*>&);
#endif
	int ajouteterminaison(string t,char);
	void LitUneRg(ifstream& fichier,VECTA<automate*>&,char guesser);
	GlobalParseurTrans(char* ftrans,char* hmm,char* filename,char* path);
	int lirecode(ifstream& fichier,unsigned char& c,char type=NON);
	char lookup();
	char cherchearcs(VECTA<parcoursarc*>& courants,int c,int i,
		VECTA<parcoursarc*>& suivants,VECTA<parcoursarc*>& chaines,char,lecteur&);
	void maj(automate*,automate*,VECTA<arctrans*>& arcscourant,char);
	void liretrans(vector<string>& lexs,VECTA<int>*,VECTA<arctrans*>& lesarcs);
	void lire(lecteur*);
	char testLC(arctrans* a,lecteur& circ);
	char parcoursdesarcs(arctrans* a,word_offset*,string,string&,char choixarc);
	~GlobalParseurTrans();
    virtual typeParseur Type() {return PARSEURTRANS;}
	char affichearc(VECTA<parcoursarc*>& liste,
		lecteur& circ,
		char* surface,word_offset* wo,VECTA<int>& ids,char choixarc=0);
	

    void SentenceAnalysis(char renumerote,char tree);
    
    void Apply();
    

    void StringAssociation(unsigned char* chaine);
    FILE* FileAssociation(FILE* fichier);

    void AddAToken(unsigned char* tok);    
    void StartAnalysis();
    void CloseAnalysis();
    void AnalyseTokenFichier();
    void AnalyseTokenChaine();
	char normalise(char c, uchar* loop);
	char testseparator(int c);
	void lecturespace(char*);
	void lecturesep(char*);
	void lectureconv(ifstream&,char* tampon);
	void lectureremplace(ifstream&,char* tampon);
	void lecturecode(char** sep,char*);
	char consommeepsilon(arctrans* a,
		int index,
		uchar* loop,
		VECTA<parcoursarc*>& suivants,
		VECTA<parcoursarc*>& chaines,
		parcoursarc* pac,
		char);
	void rechargementsarcs();
	void chargementlexiques();
	int remplace(string surface,string&);
	char deviner(unsigned char* mot,
		VECTA<parcoursarc*>& courants,
		VECTA<parcoursarc*>& suivants,
		VECTA<parcoursarc*>& chaines,
		VECTA<parcoursarc*>& finals);
	char normaliser(int* mot,unsigned char* res,int i,int r,arctrans* a);
	char consommeepsilonmin(arctrans* a,uchar c,char choixarc);
	char test(unsigned char* mot,int* lemme,VECTA<arctrans*>& courants,int& dernier);
	arctrans* unarc(int,char);
	int liretraits(int* lemme,char* curseur,int);
	char ajoutemot(string& nom,string& surface, string& lemme, string& traits);
	char testLCRC(arctrans* a);
	arctrans* testNextLCRC(arctrans* a,TYPETESTARC&);
	char* Space(int);
}; 


#endif
