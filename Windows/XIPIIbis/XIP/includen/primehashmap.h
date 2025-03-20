#ifndef _primehmap
#define _primehmap

#include <locale>
using namespace std;

#define primelong unsigned long long
static locale loc;                 // the "C" locale

inline unsigned char getprimekey(unsigned char i) {
	static const int primekeys[]={2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251}; //up to 255
	return primekeys[i%54];
}

template <class Y,class Z> class primehash_value;

template <class Y,class Z>  class primehash_cell {
public:
	
	primehash_cell()  {}
	
	~primehash_cell<Y,Z>() {}
	
	virtual bool isValue() {
		return false;
	}
	
	virtual bool Index(Y i) {
		return false;
	}

	virtual void clear() {}
	
	virtual Z& get(Y idx,primelong kidx,primehash_cell<Y,Z>*)=0;
	virtual bool find(Y,primelong)=0;
	virtual primehash_value<Y,Z>* search(Y i,primelong ki)=0;

	virtual void reset(primehash_cell<Y,Z>*) {}

	unsigned char Key(primelong kidx,unsigned char mx) {
		return ((primelong)kidx)%mx;
	}

	primelong keyvalue(long long idx) {
		return idx;
	}

	primelong keyvalue(long idx) {
		return idx;
	}

    primelong keyvalue(int idx) {
		return idx;
	}

    primelong keyvalue(short idx) {
		return idx;
	}

    primelong keyvalue(char idx) {
		return idx;
	}

	primelong keyvalue(float idx) {
		if (idx>-1 && idx<1)
			return(1/idx);
		return idx;
	}

    primelong keyvalue(double idx) {
		if (idx>-1 && idx<1)
			return(1/idx);
		return idx;
	}

	primelong keyvalue(string s) {		
		return use_facet<collate<char> >(loc).hash(s.data(),s.data()+s.length());
	}

	primelong keyvalue(char* chr) {
		if (chr==NULL)
			return 0;
		string s(chr);
		return use_facet<collate<char> >(loc).hash(s.data(),s.data()+s.length());
	}

	virtual void Toclean(primehash_cell<Y,Z>* v) {}
    virtual char erase(Y idx,primelong kidx,primehash_cell<Y,Z>*)=0;
	virtual void occupation(int& t,int& c,int& n,int& m) {}
	virtual void size(int& t) {}
	virtual void set(primehash_cell<Y,Z>* t) {}
	virtual primehash_cell<Y,Z>* _previous() {
		return NULL;
	}
	virtual primelong Index() {
		return 0;
	}
};

template<class Y,class Z> class primehash_value : public primehash_cell<Y,Z> {
public:
	
    Y indexe;
	Z value;
    
	primehash_value<Y,Z>* next;
	primehash_value<Y,Z>* previous;
	
	primehash_value(Y idx) : indexe(idx) {
		next=NULL;
		previous=NULL;
	}
	
	primehash_cell<Y,Z>* _previous() {
		return previous;
	}

	bool Index(Y i) {
		if (i==indexe)
			return true;
		return false;
	}

	primelong Index() {
        return primehash_cell<Y,Z>::keyvalue(indexe);
    }
		
	void clear() {}
	
	bool find(Y i,primelong ki) {
		if (i==indexe)
			return true;
		return false;
	}
	
	primehash_value<Y,Z>* search(Y i,primelong ki) {
		if (i==indexe)
			return this;
		return NULL;
	}

	void Value(Z x) {
		value=x;
	}
	
	Z& get(Y idx,primelong kidx,primehash_cell<Y,Z>*) {
		return value;
	}
	
	bool isValue() {
		return true;
	}
	
	Z& Value() {
		return value;
	}

	void occupation(int& t,int& c,int& n,int& m) {
		n++;
	}	

	void size(int& t) {
		t++;
	}

	char erase(Y idx,primelong kidx,primehash_cell<Y,Z>* root) {
		if (indexe==idx) {
			root->reset(this);
			if (previous!=NULL)
				previous->next=next;
			if (next!=NULL)
				next->previous=previous;
			root->Toclean(this);
			return 1;
		}
		return 0;
	}

};

template <class Y,class Z> class primehash_root : public primehash_value<Y,Z> {
public:
	primehash_value<Y,Z>* end;
	primehash_cell<Y,Z>* clean;
	


	primehash_root(Y idx) : primehash_value<Y,Z>(idx) {
		end=NULL;
		clean=NULL;
	}

	void Toclean(primehash_cell<Y,Z>* v) {
		clean=v;
	}

	void reset(primehash_cell<Y,Z>* hcell) {
		if (hcell==end) {
			end=(primehash_value<Y,Z>*)hcell->_previous();
			if (end==this)
				end=NULL;
		}
	}
};

template <class Y,class Z> class primehash_table : public primehash_cell<Y,Z> {
public:
	primehash_cell<Y,Z>** table;
	unsigned char i_max_cell; //On en garde deux, de toute facon, il y aura du padding, alors autant en profiter...
	unsigned char max_cell;
	unsigned char _size;
	
	primehash_table(int m): i_max_cell(m) {
		_size=0;
		max_cell=getprimekey(i_max_cell);
		table=new primehash_cell<Y,Z>*[max_cell];
		memset(table,0,sizeof(primehash_table*)*max_cell);
	}
		
	~primehash_table<Y,Z>() {		
		clear();
	}
	
	//nettoyage
	void clear() {
		for (int i=0;i<max_cell;i++) {
			if (table[i]!=NULL) {
				table[i]->clear();
				table[i]=NULL;
			}
		}
		delete[] table;
		table=NULL;
		max_cell=0;
	}
	
	void set(primehash_cell<Y,Z>* t) {
		table[primehash_cell<Y,Z>::Key(t->Index(),2)]=t;
		_size++;
	}

	char erase(Y idx,primelong kidx,primehash_cell<Y,Z>* root) {
		unsigned char key=primehash_cell<Y,Z>::Key(kidx,max_cell);
		if (table[key]==NULL)
			return 0;
		
		char op=table[key]->erase(idx,kidx,root);
		if (op==1) {
			root->Toclean(NULL);
			delete table[key];
			table[key]=NULL;
			_size--;
			if (_size==0)
				return 2;
			return 3;
		}

		if (op==2) {
			_size--;
			delete table[key];
			table[key]=NULL;
			if (!_size)
				return 2;
			return 3;
		}		
		return op;
	}

	void size(int& t) {
		for (int i=0;i<max_cell;i++) {
			if (table[i]!=NULL)
				table[i]->size(t);
		}
	}

	//calcul de l'occupation d'une table
	void occupation(int& t,int& c,int& n,int& m) {
		if (m<i_max_cell)
			m=i_max_cell;
		t++;
		c+=max_cell;
		for (int i=0;i<max_cell;i++) {
			if (table[i]!=NULL)
				table[i]->occupation(t,c,n,m);
		}
	}
		
	//Verification de la presence d'une valeur sur clef (idx)
	char Keyvalue(Y idx,primelong kidx,unsigned char& key) {
		//we compute our key
		key=primehash_cell<Y,Z>::Key(kidx,max_cell);
		//If the element does not exist, we return 0
		if (table[key]==NULL)
			return 0;
		//if the element exists, we return the key
		if (table[key]->Index(idx))
			return 1;
		//On retourne -1, il y a une valeur a cet endroit mais dont la clef est differente
		return -1;
	}

	
	//recherche d'une valeur sur clef...
	bool find(Y i,primelong kidx) {
		unsigned char key=primehash_cell<Y,Z>::Key(kidx,max_cell);
		if (table[key]!=NULL)
			return table[key]->find(i,kidx);
		return false;
	}

	primehash_value<Y,Z>* search(Y i,primelong ki) {
		unsigned char key=primehash_cell<Y,Z>::Key(ki,max_cell);
		if (table[key]!=NULL)
			return table[key]->search(i,ki);
		return NULL;
	}

	//Allocation de l'emplacemnet d'un element. Et rangement dans la liste des valeurs...
	Z& get(Y idx,primelong kidx,primehash_cell<Y,Z>* r) {		
		//first we check if the index corresponds to a value...
		//This function set the value of "key"...
		unsigned char key;
		char nb=Keyvalue(idx,kidx,key);
		if (nb==1) //The value is there...
			return table[key]->get(idx,kidx,r);
				
		//if it is a null place, we create the necessary place to store it
		if (!nb) {		
			_size++;
			primehash_value<Y,Z>* hcellv=new primehash_value<Y,Z>(idx);
			if (r!=NULL) {
				primehash_root<Y,Z>* root=(primehash_root<Y,Z>*)r;
				if (root->end==NULL) {
					root->end=hcellv;
					root->next=hcellv;
					hcellv->previous=root;
				}
				else {
					hcellv->previous=root->end;
					root->end->next=hcellv;
					root->end=hcellv;
				}
			}

			table[key]=hcellv;
			return hcellv->Value();
		}
		
		//A more complex case, the index has not been found so far in the current table
		//we need to create it...
		primehash_cell<Y,Z>* hcell=table[key];
		if (hcell->isValue()) {
			//There is already a value stored at this very place...
			//We need then to create a new sub-table to store it...
			primehash_table<Y,Z>* htable=new primehash_table<Y,Z>(i_max_cell+1);
			int m=getprimekey(i_max_cell+1);
			table[key]=htable;
			key=hcell->Key(hcell->Index(),m);
			htable->table[key]=hcell;
			htable->_size=1;
			return htable->get(idx,kidx,r);
		}
		else
			return hcell->get(idx,kidx,r);
	}		
};

template <class Y,class Z> class prime_hash;

//iterator... 
template<class Y,class Z> class prime_iterator {
public:
	primehash_value<Y,Z>* cells;
	prime_iterator() {}
	
	void raz() {
		cells=NULL;
	}
	
	//on initialise notre iterateur avec un element de type prime_hash
	void operator =(prime_hash<Y,Z>& t) {
		cells=NULL;
		if (t.racine!=NULL)
			cells=t.racine->next;
	}
		
	bool operator !=(prime_hash<Y,Z>& t) {
		if (cells==NULL)
			return false;
		return true;
	}

	bool end() {
		if (cells==NULL)
			return true;
		return false;
	}
	
	Y key() {
		return cells->indexe;
	}
	
	Z& value() {
		return cells->Value();
	}

	void value(Z x) {
		cells->Value(x);
	}

	void next() {
		if (cells==NULL)
			return;
		cells=cells->next;
	}
	
	void operator++() {
		next();
	}
	
	void operator++(int i) {
		next();
	}
	
};


template <class Y,class Z> class prime_hash {
public:
	primehash_cell<Y,Z>* root;
	primehash_root<Y,Z>* racine;
	char step;
	
	prime_hash() {
		racine=NULL;
		root=NULL;
		step=0;
	}
	
	~prime_hash() {
		if (root!=NULL && root!=racine)
			root->clear();
		if (racine!=NULL)
			delete racine;
	}
	
	prime_hash<Y,Z>& begin() {
		return *this;
	}

	prime_hash<Y,Z>& end() {
		return *this;
	}

	//pour trouver une valeur...
	bool find(Y i) {
		if (root==NULL)
			return false;
		return root->find(i,root->keyvalue(i));
	}
	
	primehash_value<Y,Z>* search(Y i) {
		if (root==NULL)
			return NULL;
		return root->search(i,root->keyvalue(i));
	}

	//nettoyage
	void clear() {
		if (root!=NULL && root!=racine)
			root->clear();
		if (racine!=NULL)
			delete racine;
		root=NULL;
		racine=NULL;
		step=0;
	}
	
	bool empty() {
		if (racine==NULL || racine->end==NULL)
			return true;
		return false;
	}
	
	//pour calculer l'occupation memoire: 
	// t est le nombre de cellules disponibles a travers toutes les tables
	// n le nombre d'elements effectivement enregistres.
	void occupation(int& t,int& c,int& n,int& m) {
		t=0;n=0;c=0;m=0;
		if (root!=NULL)
			root->occupation(t,c,n,m);
	}

	bool erase(Y idx) {
		if (racine==NULL || racine->end==NULL)
			return false;
		racine->clean=NULL;
		char ret=root->erase(idx,root->keyvalue(idx),racine);
		if (racine->clean!=NULL)
			delete racine->clean;
		if (racine->next==NULL) {
			if (ret==2)
				delete root;
			root=NULL;
			delete racine;
			racine=NULL;
			step=0;
		}
		if (ret)
			return true;
		return false;
	}

	size_t size() {
		int t=0;
		primehash_value<Y,Z>* r=racine->next;
		while (r!=NULL) {
			t++;
			r=r->next;
		}
		return t;
	}

	//Pour acceder a un element. Attention si l'element n'existe pas, alloue l'emplacement necessaire...
	Z& Get(Y idx) {
		switch(step) {
			case 0:
				step=1;
				racine=new primehash_root<Y,Z>(idx);
				racine->next=new primehash_value<Y,Z>(idx);
				racine->end=racine->next;
				racine->next->previous=racine;
				root=racine->next;
				return racine->next->Value();
			case 1:
				if (racine->next->Index(idx))
					return racine->next->Value();				
				root=new primehash_table<Y,Z>(0);
				root->set(racine->next);
				step=2;
				return root->get(idx,root->keyvalue(idx),racine);
			default:
				return root->get(idx,root->keyvalue(idx),racine);
		}
	}
	
	//acces via primehash[]
	Z& operator [](Y i) {
		return Get(i);
	} 

	//affectation
	void operator =(prime_hash<Y,Z>& t) {
		clear();

		prime_iterator<Y,Z> it;
		for (it=t;!it.end();it++)
			Get(it.key())=it.value();
	}
};


#endif
