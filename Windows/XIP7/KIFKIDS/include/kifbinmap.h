/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the XIP library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : XIP
Version    : 10.00
filename   : kifbinmap.h
Date       : 09/09/2010
Purpose    : Implementation of a map sans collision
Programmer : Claude ROUX
Reviewer   :
*/


#ifndef i_kifbinmap
#define i_kifbinmap

const int kifbinbits = 6;
const int kifbinsize = 1 << kifbinbits;
#define ushort unsigned short
#define binuint64 unsigned long long
const binuint64 binONE = 1;

const binuint64 binval64[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768,
binONE << 16, binONE << 17, binONE << 18, binONE << 19, binONE << 20, binONE << 21, binONE << 22, binONE << 23, binONE << 24,
binONE << 25, binONE << 26, binONE << 27, binONE << 28,
binONE << 29, binONE << 30, binONE << 31, binONE << 32, binONE << 33, binONE << 34, binONE << 35, binONE << 36, binONE << 37,
binONE << 38, binONE << 39, binONE << 40, binONE << 41,
binONE << 42, binONE << 43, binONE << 44, binONE << 45, binONE << 46, binONE << 47, binONE << 48, binONE << 49, binONE << 50,
binONE << 51, binONE << 52, binONE << 53, binONE << 54,
binONE << 55, binONE << 56, binONE << 57, binONE << 58, binONE << 59, binONE << 60, binONE << 61, binONE << 62, binONE << 63 };

template <class Z> class bin_hash;

template<class S, class Z> class bin_iter : public std::iterator<std::forward_iterator_tag, Z> {
public:
	Z** table;
	Z second;
	S first;
	int i, j;

private:
	binuint64* indexes;
	binuint64 filter;
	int tsize;

public:
	bin_iter<S, Z>() {
		table = NULL;
	}

	bin_iter<S, Z>(Z** n, binuint64* idx, int sz) {
		first = 0;
		tsize = sz;
		table = n;
		indexes = idx;
		i = 0;
		j = 0;
		filter = 0;
		if (n != NULL)
			next();
	}

	bin_iter<S, Z>(Z** n, binuint64* idx, int sz, int ix, int jx) {
		indexes = idx;
		table = n;
		tsize = sz;
		i = ix;
		j = jx;
		first = ((i << kifbinbits) + j);
		second = table[i][j];
		filter = indexes[i] >> (j + 1);
	}


	bin_iter<S, Z>& operator=(const bin_iter<S, Z>& raw) {
		filter = raw.filter;
		table = raw.table;
		indexes = raw.indexes;
		tsize = raw.tsize;
		i = raw.i;
		j = raw.j;
		first = raw.first;
		second = raw.second;
		return (*this);
	}


	bin_iter& operator++() {
		next();
	}

	bin_iter<S, Z>* operator->()  {
		return this;
	}

	inline void next() {
		while (i < tsize) {
			if (!j)
				filter = indexes[i];

			if (filter) {
				if (!(filter & 1)) {
					while (!(filter & 15)) {
						filter >>= 4;
						j = j + 4;
					}
					while (!(filter & 1)) {
						filter >>= 1;
						j++;
					}
				}
				first = ((i << kifbinbits) + j);
				second = table[i][j];
				filter >>= 1;
				if (!filter) {
					j = 0;
					i++;
				}
				else
					j++;
				return;
			}
			j = 0;
			i++;
		}

		first = 0;
		table = NULL;
		tsize = -1;
	}

	bin_iter<S, Z>& operator++(int) {
		next();
		return *this;
	}

	friend bool operator!=(bin_iter<S, Z> a, bin_iter<S, Z> b) {
		if (a.table == b.table)
			return false;
		return true;
	}

	friend bool operator==(bin_iter<S, Z> a, bin_iter<S, Z> b) {
		if (a.table == b.table)
			return true;
		return false;
	}
};

template <class Z> class bin_hash {
public:
	int tsize;
	Z** table;
	binuint64* indexes;

	typedef bin_iter<short, Z> iterator;

	iterator begin(){ return iterator(table, indexes, tsize); }
	iterator end() {
		return iterator();
	}

	bin_hash()  {
		tsize = 4;
		table = new Z*[tsize];
		indexes = new binuint64[tsize];

		table[0] = new Z[kifbinsize];
		table[1] = new Z[kifbinsize];
		table[2] = NULL;
		table[3] = NULL;
		indexes[0] = 0;
		indexes[1] = 0;
		indexes[2] = 0;
		indexes[3] = 0;
		memset(table[0], NULL, sizeof(Z) << kifbinbits);
		memset(table[1], NULL, sizeof(Z) << kifbinbits);
	}

	bin_hash(bin_hash<Z>& t) {
		static int sz = sizeof(Z) << kifbinbits;
		tsize = t.tsize;
		table = new Z*[tsize];
		indexes = new binuint64[tsize];

		for (int i = 0; i < t.tsize; i++) {
			if (t.table[i] != NULL) {
				table[i] = new Z[kifbinsize];
				indexes[i] = t.indexes[i];
				memcpy(table[i], t.table[i], sz);
			}
			else {
				table[i] = NULL;
				indexes[i] = 0;
			}
		}
	}

	~bin_hash() {
		for (int i = 0; i < tsize; i++) {
			if (table[i] != NULL)
				delete[] table[i];
		}
		delete[] table;
		delete[] indexes;
	}

	//pour trouver une valeur...
	iterator find(ushort r) {
		int i = r >> kifbinbits;
		if (i >= tsize || !indexes[i])
			return iterator();

		r = r - (i << kifbinbits);
		if (!(indexes[i] & binval64[r]))
			return iterator();

		return iterator(table, indexes, tsize, i, r);
	}

	bool get(ushort p, int& i, int& r) {
		i = p >> kifbinbits;
		if (i >= tsize || !indexes[i])
			return false;
		r = p - (i << kifbinbits);
		if (!(indexes[i] & binval64[r]))
			return false;

		return true;
	}

	bool check(ushort r) {
		int i = r >> kifbinbits;
		if (i >= tsize || !indexes[i])
			return false;
		r = r - (i << kifbinbits);
		if (!(indexes[i] & binval64[r]))
			return false;

		return true;
	}

	Z search(ushort r) {
		int i = r >> kifbinbits;
		if (i >= tsize || !indexes[i])
			return NULL;

		r = r - (i << kifbinbits);
		return table[i][r];
	}

	//nettoyage
	void clear() {
		for (int i = 0; i < tsize; i++) {
			if (table[i] != NULL) {
				delete[] table[i];
				table[i] = NULL;
				indexes[i] = 0;
			}
		}
	}

	bool empty() {
		for (int i = 0; i < tsize; i++) {
			if (indexes[i])
				return false;
		}
		return true;
	}

	void erase(ushort r) {
		int i = r >> kifbinbits;
		if (table[i] == NULL)
			return;

		r = r - (i << kifbinbits);
		indexes[i] &= ~binval64[r];
		table[i][r] = NULL;
	}

	size_t size() {
		int nb = 0;
		binuint64 filter;

		for (int i = 0; i < tsize; i++) {
			if (table[i] != NULL) {
				filter = indexes[i];
				while (filter) {
					if (!(filter & 1)) {
						while (!(filter & 65535))
							filter >>= 16;
						while (!(filter & 255))
							filter >>= 8;
						while (!(filter & 15))
							filter >>= 4;
						while (!(filter & 1))
							filter >>= 1;
					}
					nb++;
					filter >>= 1;
				}
			}
		}

		return nb;
	}

	void resize(int sz) {
		Z** ntable = new Z*[sz];
		binuint64* nindexes = new binuint64[sz];

		int i;
		for (i = 0; i < tsize; i++) {
			ntable[i] = table[i];
			nindexes[i] = indexes[i];
		}

		tsize = sz;
		for (; i < tsize; i++)  {
			ntable[i] = NULL;
			nindexes[i] = 0;
		}
		delete[] table;
		delete[] indexes;
		table = ntable;
		indexes = nindexes;
	}

	Z& operator [](ushort r) {
		int i = r >> kifbinbits;
		r = r - (i << kifbinbits);
		if (i >= tsize)
			resize(i + 2);
		if (table[i] == NULL) {
			table[i] = new Z[kifbinsize];
			memset(table[i], NULL, sizeof(Z) << kifbinbits);
		}
		indexes[i] |= binval64[r];
		//indexes[i] |= (1 << r);
		return table[i][r];
	}

	void put(ushort r, Z v) {
		int i = r >> kifbinbits;
		r = r - (i << kifbinbits);
		if (i >= tsize)
			resize(i + 2);
		if (table[i] == NULL) {
			table[i] = new Z[kifbinsize];
			memset(table[i], NULL, sizeof(Z) << kifbinbits);
		}
		indexes[i] |= binval64[r];
		//indexes[i] |= (1 << r);
		table[i][r] = v;
	}

	//affectation
	void operator =(bin_hash<Z>& t) {
		static int sz = sizeof(Z) << kifbinbits;
		int i;

		for (i = 0; i < tsize; i++) {
			if (table[i] != NULL) {
				delete[] table[i];
				indexes[i] = 0;
				table[i] = NULL;
			}
		}

		if (tsize < t.tsize) {
			tsize = t.tsize;
			delete[] table;
			delete[] indexes;
			table = new Z*[tsize];
			indexes = new binuint64[tsize];
		}

		for (i = 0; i < t.tsize; i++) {
			if (t.table[i] != NULL) {
				table[i] = new Z[kifbinsize];
				indexes[i] = t.indexes[i];
				memcpy(table[i], t.table[i], sz);
			}
			else {
				table[i] = NULL;
				indexes[i] = 0;
			}
		}
	}
};

template <class L, class Z> class hash_bin {
public:
	int tsize;
	Z** table;
	binuint64* indexes;
	bool initmem;

	typedef bin_iter<L, Z> iterator;

	iterator begin(){ return iterator(table, indexes, tsize); }
	iterator end() {
		return iterator();
	}

	hash_bin(bool m = true)  {
		initmem = m;
		tsize = 4;
		table = new Z*[tsize];
		indexes = new binuint64[tsize];

		table[0] = new Z[kifbinsize];
		table[1] = new Z[kifbinsize];
		table[2] = NULL;
		table[3] = NULL;
		indexes[0] = 0;
		indexes[1] = 0;
		indexes[2] = 0;
		indexes[3] = 0;
		if (initmem) {
			memset(table[0], NULL, sizeof(Z) << kifbinbits);
			memset(table[1], NULL, sizeof(Z) << kifbinbits);
		}
	}

	hash_bin(int tz, bool m = true)  {
		initmem = m;
		tsize = tz;
		if (tsize < 2)
			tsize = 2;

		table = new Z*[tsize];
		indexes = new binuint64[tsize];

		for (int i = 0; i < tz; i++) {
			table[i] = NULL;
			indexes[i] = 0;
		}

		table[0] = new Z[kifbinsize];
		table[1] = new Z[kifbinsize];
		if (initmem) {
			memset(table[0], NULL, sizeof(Z) << kifbinbits);
			memset(table[1], NULL, sizeof(Z) << kifbinbits);
		}
	}

	hash_bin(hash_bin<L, Z>& t) {
		static int sz = sizeof(Z) << kifbinbits;
		initmem = t.initmem;

		tsize = t.tsize;
		table = new Z*[tsize];
		indexes = new binuint64[tsize];

		for (int i = 0; i < t.tsize; i++) {
			if (t.table[i] != NULL) {
				table[i] = new Z[kifbinsize];
				indexes[i] = t.indexes[i];
				for (int j = 0; j < kifbinsize; j++)
					table[i][j] = t.table[i][j];
			}
			else {
				table[i] = NULL;
				indexes[i] = 0;
			}
		}
	}

	~hash_bin() {
		for (int i = 0; i < tsize; i++) {
			if (table[i] != NULL)
				delete[] table[i];
		}
		delete[] table;
		delete[] indexes;
	}

	int countnull() {
		int nb = 0;
		for (int j = 0; j < tsize; j++) {
			if (table[j] == NULL)
				nb++;
		}
		return nb;
	}

	//pour trouver une valeur...
	iterator find(L r) {
		int i = r >> kifbinbits;
		if (i >= tsize || !indexes[i])
			return iterator();

		r = r - (i << kifbinbits);
		if (!(indexes[i] & binval64[r]))
			return iterator();

		return iterator(table, indexes, tsize, i, r);
	}

	bool get(L p, int& i, int& r) {
		i = p >> kifbinbits;
		if (i >= tsize || !indexes[i])
			return false;
		r = p - (i << kifbinbits);
		if (!(indexes[i] & binval64[r]))
			return false;

		return true;
	}

	bool check(L r) {
		int i = r >> kifbinbits;
		if (i >= tsize || !indexes[i])
			return false;
		r = r - (i << kifbinbits);
		if (!(indexes[i] & binval64[r]))
			return false;

		return true;
	}

	Z search(L r) {
		int i = r >> kifbinbits;
		if (i >= tsize || !indexes[i])
			return NULL;

		r = r - (i << kifbinbits);
		return table[i][r];
	}

	//nettoyage
	void clear() {
		for (int i = 0; i < tsize; i++) {
			if (table[i] != NULL) {
				delete[] table[i];
				table[i] = NULL;
				indexes[i] = 0;
			}
		}
	}

	bool empty() {
		for (int i = 0; i < tsize; i++) {
			if (indexes[i])
				return false;
		}
		return true;
	}

	void erase(L r) {
		int i = r >> kifbinbits;
		if (table[i] == NULL)
			return;

		r = r - (i << kifbinbits);
		indexes[i] &= ~binval64[r];
	}

	void erase(iterator& it) {
		if (table[it.i] == NULL)
			return;

		indexes[it.i] &= ~binval64[it.j];
	}

	size_t size() {
		int nb = 0;
		binuint64 filter;

		for (int i = 0; i < tsize; i++) {
			if (table[i] != NULL) {
				filter = indexes[i];
				while (filter) {
					if (!(filter & 1)) {
						while (!(filter & 65535))
							filter >>= 16;
						while (!(filter & 255))
							filter >>= 8;
						while (!(filter & 15))
							filter >>= 4;
						while (!(filter & 1))
							filter >>= 1;
					}
					nb++;
					filter >>= 1;
				}
			}
		}

		return nb;
	}

	void resize(int sz) {
		Z** ntable = new Z*[sz];
		binuint64* nindexes = new binuint64[sz];

		int i;
		for (i = 0; i < tsize; i++) {
			ntable[i] = table[i];
			nindexes[i] = indexes[i];
		}

		tsize = sz;
		for (; i < tsize; i++)  {
			ntable[i] = NULL;
			nindexes[i] = 0;
		}
		delete[] table;
		delete[] indexes;
		table = ntable;
		indexes = nindexes;
	}

	Z& operator [](L r) {
		int i = r >> kifbinbits;
		r = r - (i << kifbinbits);
		if (i >= tsize)
			resize(i + 2);
		if (table[i] == NULL) {
			table[i] = new Z[kifbinsize];
			if (initmem)
				memset(table[i], NULL, sizeof(Z) << kifbinbits);
		}
		indexes[i] |= binval64[r];
		//indexes[i] |= (1 << r);
		return table[i][r];
	}

	//affectation
	void operator =(hash_bin<L, Z>& t) {
		static int sz = sizeof(Z) << kifbinbits;
		int i;

		for (i = 0; i < tsize; i++) {
			if (table[i] != NULL) {
				delete[] table[i];
				indexes[i] = 0;
				table[i] = NULL;
			}
		}

		if (tsize < t.tsize) {
			tsize = t.tsize;
			delete[] table;
			delete[] indexes;
			table = new Z*[tsize];
			indexes = new binuint64[tsize];
		}

		for (i = 0; i < t.tsize; i++) {
			if (t.table[i] != NULL) {
				table[i] = new Z[kifbinsize];
				indexes[i] = t.indexes[i];
				for (int j = 0; j < kifbinsize; j++)
					table[i][j] = t.table[i][j];
			}
			else {
				table[i] = NULL;
				indexes[i] = 0;
			}
		}
	}
};

template <class Z> class hash_short {
private:
	short hashshortsz;
	short hashshortkey; // 65536/hashshortsz == 2^^hashshortkey

public:
	hash_bin<unsigned short, Z>** table;

	hash_short(short h, short k)  {
		hashshortsz = h;
		hashshortkey = k;
		table = new hash_bin<unsigned short, Z>*[hashshortsz];
		for (int i = 0; i < hashshortsz; i++)
			table[i] = NULL;
	}

	~hash_short() {
		for (int i = 0; i < hashshortsz; i++) {
			if (table[i] != NULL)
				delete table[i];
		}
		delete[] table;
	}

	bool check(ushort r) {
		int i = r >> hashshortkey;
		if (table[i] == NULL)
			return false;
		//65536/hashshortsz = hashshortsz192 = 2^^13
		r = r - (i << hashshortkey);
		return table[i]->check(r);
	}

	Z& operator [](ushort r) {
		int i = r >> hashshortkey;
		if (table[i] == NULL)
			table[i] = new hash_bin<ushort, Z>(2);

		r = r - (i << hashshortkey);
		return (*table[i])[r];
	}

	int countnull() {
		int nb = 0;
		for (int i = 0; i < hashshortsz; i++) {
			if (table[i] == NULL)
				nb++;
			else {
				for (int j = 0; j < table[i]->tsize; j++) {
					if (table[i]->table[j] == NULL)
						nb++;
				}
			}
		}
		return nb;
	}
};

#endif
