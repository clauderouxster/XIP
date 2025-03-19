#ifndef entities_h
#define entities_h
/*-------------------------------------------------------------------------*/
class agde;

class offsets {
	public:
	int idsent;
	long left;
	long right;
	
	offsets(int i,long l,long r) {
		idsent=i;
		left=l;
		right=r;
	}
};

class entities {
	public:
	entities* homonyme;
	//liste des documents qui contiennent ce concept...
	vecteur documents;
	vecteur concept;
	string cle;
	int reference;
	char nouveauConcept;
	char nouveauFichier;
	int occurence;
	VECTA<offsets*> the_offsets;
	//pos=1 : noun
	//pos=2 : verb
	//pos=3 : adjective
	//pos=0 : other
	int pos;
	entities();
	entities(agde&,string,int cat,vecteur* v=NULL);
	entities(agde&,string mot,int cat,int id,int occ);
	~entities();
	void affiche(entities*,int);
	void addDoc(int);
	void addSent(int);
	void affiche(ostream&);
	void save(ofstream& file);
	void addoffset(int idsent,long left,long right);
};

#endif
