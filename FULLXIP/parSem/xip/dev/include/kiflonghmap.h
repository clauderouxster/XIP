#ifndef _kiflonghmap
#define _kiflonghmap

#define NB_TABLE_CELLS 16

#ifndef BULONG
#ifdef WIN32
#define BULONG unsigned _int64
#else
#define BULONG unsigned long long
#endif
#endif

template <class Z>  class hash_bulong_cell {
public:
	BULONG indexe;

	hash_bulong_cell(BULONG idx) : indexe(idx) {}

	~hash_bulong_cell<Z>() {}


	virtual short Size() {
		return 0;
	}

	virtual bool isValue()=0;
	virtual void clear() {}
	virtual Z& Get(BULONG i)=0;
	virtual bool find(BULONG)=0;
	virtual void erase(BULONG i) {}
	virtual hash_bulong_cell<Z>* Table(short i) {
		return NULL;
	}

	virtual hash_bulong_cell<Z>* Next(int& i) {
		return this;
	}

	virtual int Max() {
		return 0;
	}

};

template<class Z> class hash_bulong_cell_value : public hash_bulong_cell<Z> {
public:

	Z value;
	hash_bulong_cell_value(BULONG idx) : hash_bulong_cell<Z>(idx) {}
	~hash_bulong_cell_value<Z>() {}

	void clear() {}

	bool find(BULONG i) {
		if (i==hash_bulong_cell<Z>::indexe)
			return true;
		return false;
	}

	Z& Get(BULONG i) {
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

template <class Z> class hash_bulong_cell_table : public hash_bulong_cell<Z> {
public:

	hash_bulong_cell<Z>** table;
	hash_bulong_cell<Z>* next;
	unsigned short key;	
	short max_cell;
	short nbelements;
	short moved;
	char type;

	hash_bulong_cell_table(int m,BULONG idx,char t=1): max_cell(m),hash_bulong_cell<Z>(idx) {
		type=t;
		moved=-1;
		table=new hash_bulong_cell<Z>*[max_cell+1];
		next=NULL;
		nbelements=0;
		memset(table,0,sizeof(hash_bulong_cell_table*)*(max_cell+1));
	}

	bool isValue() {
		return false;
	}

	short Size() {
		return nbelements;
	}

	hash_bulong_cell<Z>* Table(short i) {
		return table[i];
	}

	hash_bulong_cell<Z>* Next(int& i) {
		while (i<max_cell && (table[i]==NULL || !table[i]->isValue()))
			i++;
		if (i==max_cell) {
			if (next==NULL)
				return NULL;
			i=0;
			return next->Next(i);
		}
		return this;
	}

	int Max() {
		return max_cell;
	}

	~hash_bulong_cell_table<Z>() {		
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

	void erase(BULONG idx) {
		key=idx%max_cell;
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

	short inValue(BULONG idx) {
		key=idx%max_cell;
		if (table[key]==NULL)
			return -2;
		if (table[key]->indexe==idx)
			return key;
		return -1;
	}

	bool find(BULONG i) {
		switch(inValue(i)) {
		case -1:
			return table[key]->find(i);
		case -2:
			return false;
		default:
			return true;
		}
	}


	Z& Get(BULONG idx) {

		short nb=inValue(idx);
		if (nb>=0)
			return table[(unsigned short)nb]->Get(idx);

		hash_bulong_cell<Z>* hcell;
		if (nb==-2) {			
			hcell=new hash_bulong_cell_value<Z>(idx);
			table[key]=hcell;
			moved=key;
			nbelements++;
			return ((hash_bulong_cell_value<Z>*)hcell)->Value();
		}

		//A more complex case, the index has not been found so far in the current table
		//we need to create it...
		hcell=table[key];
		if (hcell->isValue()) {			
			int m;			

			if (nbelements==1 && moved>=0) {
				max_cell-=2;
				int kx=table[moved]->indexe%max_cell;
				if (kx!=moved) {
					table[kx]=table[moved];
					table[moved]=NULL;
				}
				moved=-10;				
				return Get(idx);
			}

			m=max_cell+type;
			hash_bulong_cell_table<Z>* htable=new hash_bulong_cell_table<Z>(m,hcell->indexe,type+1);
			htable->next=next;
			next=htable;
			table[key]=htable;
			key=hcell->indexe%m;
			htable->table[key]=hcell;			
			htable->nbelements=1;
			htable->moved=key;
			return (*htable)[idx];
		}
		else
			return hcell->Get(idx);
	}

	Z& operator [](BULONG i) {
		return Get(i);
	} 

};


template <class Z> class kif_hash_bulong {
public:
	hash_bulong_cell<Z>* root;
	char step;

	kif_hash_bulong() {
		root=NULL;
		step=0;
	}

	void operator =(kif_hash_bulong<Z>& t);

	bool end() {
		return false;
	}

	kif_hash_bulong<Z>& begin() {
		return *this;
	}

	bool find(BULONG i) {
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

	~kif_hash_bulong() {
		if (root!=NULL)
			root->clear();
	}

	bool empty() {
		if (!step)
			return true;
		return false;
	}

	void erase(BULONG i) {
		if (root!=NULL)
			root->erase(i);
	}

	Z& Get(BULONG i) {
		hash_bulong_cell_value<Z>* hcell;
		unsigned short key;
		hash_bulong_cell_table<Z>* htable;
		switch(step) {
		case 0:
			step=1;
			hcell=new hash_bulong_cell_value<Z>(i);
			root=hcell;
			return hcell->Value();
		case 1:
			hcell=(hash_bulong_cell_value<Z>*)root;
			if (hcell->indexe==i)
				return hcell->Value();

			htable=new hash_bulong_cell_table<Z>(NB_TABLE_CELLS,root->indexe);
			key=root->indexe%NB_TABLE_CELLS;
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

	Z& operator [](BULONG i) {
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

	hash_bulong_cell<Z>* cells;
	int iter;
	hash_bulong_cell_value<Z>* current;
	kif_hash_iteration() {}

	void raz() {
		iter=-1;
		current=NULL;
		cells=NULL;
	}

	bool operator !=(bool x) {
		if (cells==NULL && current==NULL)
			return false;
		return true;

	}

	void operator =(kif_hash_bulong<Z>& t) {
		iter=0;
		current=NULL;
		cells=t.root;
		if (cells!=NULL)
			next();
	}

	bool end() {
		if (cells==NULL && current==NULL)
			return true;
		return false;
	}

	BULONG first() {
		return current->indexe;
	}

	Z& second() {
		return current->Value();
	}

	void next() {
		current=NULL;
		if (cells==NULL)
			return;

		if (cells->isValue()) {
			current=(hash_bulong_cell_value<Z>*)cells;
			cells=NULL;
			return;
		}

		cells=cells->Next(iter);
		if (cells!=NULL)
			current=(hash_bulong_cell_value<Z>*)cells->Table(iter++);
	}

	void operator++() {
		next();
	}

	void operator++(int i) {
		next();
	}
};


#endif
