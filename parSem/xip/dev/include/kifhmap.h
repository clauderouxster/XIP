#ifndef _kifhmap
#define _kifhmap

#define NB_TABLE_CELLS 16

template <class Z>  class hash_short_cell {
public:
	short indexe;

	hash_short_cell(short idx) : indexe(idx) {}

	~hash_short_cell<Z>() {}


	virtual short Size() {
		return 0;
	}

	virtual bool isValue()=0;
	virtual void clear() {}
	virtual Z& Get(short i)=0;
	virtual bool find(short)=0;
	virtual void erase(short i) {}
	virtual hash_short_cell<Z>* Table(short i) {
		return NULL;
	}

	virtual hash_short_cell<Z>* Next() {
		return NULL;
	}

	virtual int Max() {
		return 0;
	}

};

template<class Z> class hash_short_cell_value : public hash_short_cell<Z> {
public:

	Z value;
	hash_short_cell_value(int idx) : hash_short_cell<Z>(idx) {}
	~hash_short_cell_value<Z>() {}

	void clear() {}

	bool find(short i) {
		if (i==hash_short_cell<Z>::indexe)
			return true;
		return false;
	}

	Z& Get(short i) {
		return value;
	}

	bool isValue() {
		return true;
	}

	Z& Value() {
		return value;
	}

	short Size() {
		return 1;
	}

};

template <class Z> class hash_short_cell_table : public hash_short_cell<Z> {
public:

	hash_short_cell<Z>** table;
	hash_short_cell<Z>* next;
	unsigned short key;	
	short max_cell;
	short nbelements;
	short moved;
	char type;

	hash_short_cell_table(int m,int idx,char t=1): max_cell(m),hash_short_cell<Z>(idx) {
		type=t;
		moved=-1;
		table=new hash_short_cell<Z>*[max_cell+1];
		next=NULL;
		nbelements=0;
		memset(table,0,sizeof(hash_short_cell_table*)*(max_cell+1));
	}

	bool isValue() {
		return false;
	}

	short Size() {
		return nbelements;
	}

	hash_short_cell<Z>* Table(short i) {
		return table[i];
	}

	hash_short_cell<Z>* Next() {
		return next;
	}

	int Max() {
		return max_cell;
	}

	~hash_short_cell_table<Z>() {		
		clear();
		//delete[] table;
	}


	void clear() {
		for (short i=0;i<max_cell;i++) {
			if (table[i]!=NULL) {
				table[i]->clear();
				delete table[i];
				table[i]=NULL;
			}
		}
		delete[] table;
		table=NULL;
		max_cell=0;
		next=NULL;
	}

	void erase(short idx) {
		key=((unsigned short)idx)%max_cell;
		if (table[key]==NULL)
			return;
		if (table[key]->indexe==idx) {
			if (table[key]->isValue()) {
				delete table[key];
				table[key]=NULL;
				nbelements--;
			}
			else {
				table[key]->erase(idx);
				if (table[key]->Size()==0) {
					delete table[key];
					table[key]=NULL;
					nbelements--;
				}
			}
		}
	}

	short inValue(short idx) {
		key=((unsigned short)idx)%max_cell;
		if (table[key]==NULL)
			return -2;
		if (table[key]->indexe==idx)
			return key;
		return -1;
	}

	bool find(short i) {
		switch(inValue(i)) {
		case -1:
			return table[key]->find(i);
		case -2:
			return false;
		default:
			return true;
		}
	}


	Z& Get(short idx) {

		short nb=inValue(idx);
		if (nb>=0)
			return table[(unsigned short)nb]->Get(idx);

		hash_short_cell<Z>* hcell;
		if (nb==-2) {			
			hcell=new hash_short_cell_value<Z>(idx);
			table[key]=hcell;
			moved=key;
			nbelements++;
			return ((hash_short_cell_value<Z>*)hcell)->Value();
		}

		//A more complex case, the index has not been found so far in the current table
		//we need to create it...
		hcell=table[key];
		if (hcell->isValue()) {			
			int m;			

			if (nbelements==1 && moved>=0) {
				int kx=table[moved]->indexe%(max_cell+1);
				table[kx]=table[moved];
				table[moved]=NULL;
				moved=-10;
				max_cell++;
				return Get(idx);
			}

			m=max_cell+type;
			hash_short_cell_table<Z>* htable=new hash_short_cell_table<Z>(m,hcell->indexe,type+1);
			htable->next=next;
			next=htable;
			table[key]=htable;
			key=((unsigned short)hcell->indexe)%m;
			htable->table[key]=hcell;			
			htable->nbelements=1;
			htable->moved=key;
			return (*htable)[idx];
		}
		else
			return hcell->Get(idx);
	}

	Z& operator [](int i) {
		return Get(i);
	} 

	Z& operator [](short i) {
		return Get(i);
	} 

};


template <class Z> class kif_hash_short {
public:
	hash_short_cell<Z>* root;
	char step;

	kif_hash_short() {
		root=NULL;
		step=0;
	}

	void operator =(kif_hash_short<Z>& t);

	bool end() {
		return false;
	}

	kif_hash_short<Z>& begin() {
		return *this;
	}

	bool find(short i) {
		if (root==NULL)
			return false;
		return root->find(i);
	}

	virtual void clear() {
		if (root!=NULL)
			root->clear();
		root=NULL;
		step=0;
	}

	~kif_hash_short() {
		if (root!=NULL)
			root->clear();
	}

	bool empty() {
		if (!step)
			return true;
		return false;
	}

	void erase(short i) {
		if (root!=NULL)
			root->erase(i);
	}

	Z& Get(short i) {
		hash_short_cell_value<Z>* hcell;
		unsigned short key;
		hash_short_cell_table<Z>* htable;
		switch(step) {
		case 0:
			step=1;
			hcell=new hash_short_cell_value<Z>(i);
			root=hcell;
			return hcell->Value();
		case 1:
			hcell=(hash_short_cell_value<Z>*)root;
			if (hcell->indexe==i)
				return hcell->Value();

			htable=new hash_short_cell_table<Z>(NB_TABLE_CELLS,root->indexe);
			key=((unsigned short)root->indexe)%NB_TABLE_CELLS;
			htable->table[key]=hcell;
			step=2;
			root=htable;
			htable->nbelements=1;
			htable->moved=key;
			return (*htable)[i];
		default:
			return root->Get(i);
		}
	}

	Z& operator [](short i) {
		return Get(i);
	} 

	int size() {
		if (root==NULL)
			return 0;
		return 1;
	}
};


template<class Z> class kif_hash_iteration {
public:

	hash_short_cell<Z>* tables;
	int iter;
	hash_short_cell_value<Z>* current;
	kif_hash_iteration() {}

	void raz() {
		iter=-1;
		current=NULL;
		tables=NULL;
	}

	bool operator !=(bool x) {
		if (tables==NULL && current==NULL)
			return false;
		return true;

	}

	void operator =(kif_hash_short<Z>& t) {
		iter=-1;
		current=NULL;
		tables=t.root;
		if (tables!=NULL)
			next();
	}

	bool end() {
		if (tables==NULL && current==NULL)
			return true;
		return false;
	}

	short first() {
		return current->indexe;
	}

	Z& second() {
		return current->Value();
	}

	void next() {
		current=NULL;
		if (tables==NULL)
			return;

		if (tables->isValue()) {
			current=(hash_short_cell_value<Z>*)tables;
			tables=NULL;
			return;
		}

		hash_short_cell<Z>* localtable=tables;
		iter++;
		while(localtable!=NULL && (iter>=localtable->Max() || localtable->Table(iter)==NULL || !localtable->Table(iter)->isValue())) {
			iter++;
			if (iter>=localtable->Max()) {
				localtable=localtable->Next();
				tables=localtable;
				iter=0;
			}
		}
		if (tables!=NULL)
			current=(hash_short_cell_value<Z>*)localtable->Table(iter);
	}

	void operator++() {
		next();
	}

	void operator++(int i) {
		next();
	}
};


#endif
