#ifndef _primehmap
#define _primehmap

#include <locale>

using std::locale;

#define primelong unsigned long long
static locale loc;                 // the "C" locale

inline unsigned short* primekeys() {
	static unsigned short pkeys[]={41,37,31,29,23};
	return pkeys;
}

//#define HASHFIBONACCI
//#ifdef HASHFIBONACCI
//static const int sizetable=13;
//
//
//inline unsigned short getprimekey(unsigned char i) {
//    //up to 255
//	//Fibonacci
//	//static unsigned short pkeys[sizetable];
//	//static bool init=false;
//	//if (!init) {
//	//	init=true;
//	//	pkeys[sizetable-1]=2;
//	//	pkeys[sizetable-2]=3;
//	//	for (int i=sizetable-3;i>=0;i--)
//	//		pkeys[i]=pkeys[i+1]+pkeys[i+2];
//	//}
//	static unsigned short* pkeys[]={7,13,5,17,3,19,2,23,11,29};
//	return pkeys[i];
//}
//#else
//inline unsigned short getprimekey(unsigned char i) {
//    //up to 255
//	//static const int pkeys[]={2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251};
//	//static const int pkeys[]={41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251};
//	static unsigned short* pkeys[]={8191,17,5,3,2};
//	return pkeys[i];
//}
//#endif

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
	
	virtual bool find(Y,primelong)=0;
	virtual primehash_value<Y,Z>* search(Y i,primelong ki)=0;
    
	virtual void reset(primehash_cell<Y,Z>*) {}

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
		if (idx && (idx>-1 && idx<1))
			return(1/idx);
		return idx;
	}

    primelong keyvalue(double idx) {
		if (idx && (idx>-1 && idx<1))
			return(1/idx);
		return idx;
	}

	primelong keyvalue(string s) {		
		return std::use_facet<std::collate<char> >(loc).hash(s.data(),s.data()+s.length());
	}

	primelong keyvalue(char* chr) {
		if (chr==NULL)
			return 0;
		string s(chr);
		return std::use_facet<std::collate<char> >(loc).hash(s.data(),s.data()+s.length());
	}

	virtual void Toclean(primehash_cell<Y,Z>* v) {}
    virtual char erase(Y idx,primelong kidx,primehash_cell<Y,Z>*)=0;
	virtual void occupation(int& t,int& c,int& n,int& m) {}
	virtual void size(int& t) {}
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

	primehash_value() {
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
		
	void clear() {
		delete this;
	}
	
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
	primehash_value<Y,Z>* clean;
	unsigned short* pkeys;
	bool cleaning;
	int mx;

	primehash_root(unsigned short* p) : primehash_value<Y,Z>() {
		pkeys=p;
		cleaning=false;
		if (pkeys==NULL)
			pkeys=primekeys();
		else
			cleaning=true;
		mx=0;
		while (pkeys[mx]!=0)
			mx++;
		end=NULL;
		clean=NULL;
	}

	~primehash_root() {
		if (cleaning)
			delete[] pkeys;
	}

	void setseeds(unsigned short* p) {
		if (cleaning)
			delete[] pkeys;
		pkeys=p;
		mx=0;
		while (pkeys[mx]!=0)
			mx++;
		cleaning=true;
	}

	void Toclean(primehash_cell<Y,Z>* v) {
		clean=(primehash_value<Y,Z>*)v;
	}

	void reset(primehash_cell<Y,Z>* hcell) {
		if (hcell==end) {
			end=(primehash_value<Y,Z>*)hcell->_previous();
			if (end==this)
				end=NULL;
		}
	}

	unsigned short getprimekey(unsigned char i) {
		if (i>=mx) {
			if (!pkeys[mx])
				pkeys[mx]=pkeys[mx-1];
			pkeys[mx]++;
			return pkeys[mx];
		}

		return pkeys[i];
	}

};

template <class Y,class Z> class primehash_table : public primehash_cell<Y,Z> {
public:
	unsigned short max_cell;
	short _size;
	primehash_cell<Y,Z>** table;
	unsigned char i_max_cell; //On en garde deux, de toute facon, il y aura du padding, alors autant en profiter...
	
	primehash_table(int m,primehash_root<Y,Z>* r): i_max_cell(m) {
		_size=0;
		max_cell=r->getprimekey(i_max_cell);
		table=new primehash_cell<Y,Z>*[max_cell];
		memset(table,0,sizeof(primehash_table*)*max_cell);
	}
		
	~primehash_table<Y,Z>() {		
		clear();
	}
	
	//nettoyage
	void clear() {
		for (int i=0;i<max_cell;i++) {
            if (!_size)
                break;
			if (table[i]!=NULL) {
				table[i]->clear();
				table[i]=NULL;
                _size--;
			}
		}
		delete[] table;
	}
	
	char erase(Y idx,primelong kidx,primehash_cell<Y,Z>* root) {
		unsigned short key=kidx%max_cell;
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
		
	//recherche d'une valeur sur clef...
	bool find(Y i,primelong ki) {
		primehash_cell<Y,Z>* cell=table[ki%max_cell];
        if (cell==NULL)
			return false;
		return cell->find(i,ki);
	}

	primehash_value<Y,Z>* search(Y i,primelong ki) {
		primehash_cell<Y,Z>* cell=table[ki%max_cell];
        if (cell==NULL)
			return NULL;
		return cell->search(i,ki);
	}

	//Allocation de l'emplacemnet d'un element. Et rangement dans la liste des valeurs...
	void put(Y idx,Z& v,primelong kidx,primehash_root<Y,Z>* r) {
		//first we check if the index corresponds to a value...
		unsigned short key=kidx%max_cell;
        primehash_cell<Y,Z>* hcell=table[key];
        
        if (hcell==NULL) {
            //if it is a null place, we create the necessary place to store it
			_size++;
			primehash_value<Y,Z>* hcellv=new primehash_value<Y,Z>(idx);
			hcellv->value=v;
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
            return;
		}
		
		//A more complex case, the index has not been found so far in the current table
		//we need to create it...
		if (hcell->isValue()) {
            if (((primehash_value<Y,Z>*)hcell)->indexe==idx) {//The value is there...
                ((primehash_value<Y,Z>*)hcell)->value=v;
                return;
            }
            
            //There is already a value stored at this very place...
			//We need then to create a new sub-table to store it...
			primehash_table<Y,Z>* htable=new primehash_table<Y,Z>(i_max_cell+1,r);
			table[key]=htable;
			key=hcell->Index()%htable->max_cell;
			htable->table[key]=hcell;
			htable->_size=1;
			htable->put(idx,v,kidx,r);
            return;
		}
        
        ((primehash_table<Y,Z>*)hcell)->put(idx,v,kidx,r);
	}		

    
	//Allocation de l'emplacemnet d'un element. Et rangement dans la liste des valeurs...
	primehash_value<Y,Z>* get(Y idx,primelong kidx,primehash_root<Y,Z>* r) {		
		//first we check if the index corresponds to a value...
		unsigned short key=kidx%max_cell;
        primehash_cell<Y,Z>* hcell=table[key];

        if (hcell==NULL) {
            //if it is a null place, we create the necessary place to store it
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
			return hcellv;
		}
		
		//A more complex case, the index has not been found so far in the current table
		//we need to create it...
		if (hcell->isValue()) {
            if (((primehash_value<Y,Z>*)hcell)->indexe==idx) //The value is there...
                return (primehash_value<Y,Z>*)hcell;
            
            //There is already a value stored at this very place...
			//We need then to create a new sub-table to store it...
			primehash_table<Y,Z>* htable=new primehash_table<Y,Z>(i_max_cell+1,r);
			table[key]=htable;
			key=hcell->Index()%htable->max_cell;
			htable->table[key]=hcell;
			htable->_size=1;
			return htable->get(idx,kidx,r);
		}
        
        return ((primehash_table<Y,Z>*)hcell)->get(idx,kidx,r);
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
	primehash_table<Y,Z>* root;
	primehash_root<Y,Z>* racine;
	primehash_value<Y,Z>* last;

	prime_hash(unsigned short*p=NULL) {
		racine=new primehash_root<Y,Z>(p);
		root=new primehash_table<Y,Z>(0,racine);
		last=NULL;
	}
	
	~prime_hash() {
        delete root;
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
		if (last!=NULL && last->indexe==i)
			return true;
		return root->find(i,root->keyvalue(i));
	}
	
	bool setseeds(unsigned short* p) {
		if (racine->end!=NULL)
			return false;

		racine->setseeds(p);
		delete root;
		root=new primehash_table<Y,Z>(0,racine);
		return true;
	}

	primehash_value<Y,Z>* search(Y i) {  
		if (last!=NULL && last->indexe==i)
			return last;
		last=root->search(i,root->keyvalue(i));
		return last;
	}

	//nettoyage
	void clear() {
		for (int i=0;i<root->max_cell;i++) {
            if (!root->_size)
                break;
			if (root->table[i]!=NULL) {
				root->table[i]->clear();
				root->table[i]=NULL;
                root->_size--;
			}
		}
        root->_size=0;
		racine->end=NULL;
		racine->next=NULL;	
		last=NULL;
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
		if (racine->end==NULL)
			return false;
		last=NULL;
		racine->clean=NULL;
		char ret=root->erase(idx,root->keyvalue(idx),racine);
		if (racine->clean!=NULL)
			delete racine->clean;
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
		if (last!=NULL && last->indexe==idx)
			return last->value;		
        last=root->get(idx,root->keyvalue(idx),racine);
		return last->value;
	}
	
	//acces via primehash[]
	Z& operator [](Y idx) {
		if (last!=NULL && last->indexe==idx)
			return last->value;		
        last=root->get(idx,root->keyvalue(idx),racine);
		return last->value;
	}

	void put(Y idx,Z v) {
		root->put(idx,v,root->keyvalue(idx),racine);
	}

	//affectation
	void operator =(prime_hash<Y,Z>& t) {
		clear();
        primehash_value<Y,Z>* cell=t.racine->next;
        while (cell!=NULL) {
            root->put(cell->indexe,cell->value,root->keyvalue(cell->indexe),racine);
            cell=cell->next;
        }
	}
};


#endif
