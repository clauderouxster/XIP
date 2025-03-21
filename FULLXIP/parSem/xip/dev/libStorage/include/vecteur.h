#ifndef vecteur_h
#define vecteur_h

class agde;

class granulet {
	public:
	VECTEUR mot;
	short rang;
	granulet(int);
	granulet(granulet* g=NULL);
	granulet(granulet& g);
	granulet(int,VECTEUR);
	void affiche(ostream&);	
	void save(ostream&);
	void reload(char*);
};

class vecteur {
	public:
	VECTA<granulet*> element;
	vecteur() {};
	vecteur(vecteur* v);
	vecteur(vecteur& v);
	vecteur(agde*,int& reference);
	~vecteur() {element.nettoie();}
	void raz() {element.raz();}
	void reload(ifstream&);
	void merge(vecteur&);	
	void merge(vecteur*);
	void merge(granulet*);
	void merge(granulet&);
	int count();
	void remove(int b);
	void save(ostream& os);
	void table(vector<int>&);
	void table(VECTA<int>&);
	int present(vecteur*);
	int present(vecteur&);
	void common(vecteur*,vecteur*);
	void common(vecteur&,vecteur&);
	void common(vecteur&);
	void common(vecteur*);
	void filter(vecteur*);
	void filter(vecteur&);
	void makeFiltre(string rel,string POS, agde& Agde);
	void makeFiltre(string rel,string POS, agde* Agde);
	char equal(vecteur& v);
	char equal(vecteur* v);
	int common(short r, VECTEUR m,int start);
};

char* WtoS(VECTEUR mot,char* chaine);
VECTEUR StoW(char* chaine);
#endif
