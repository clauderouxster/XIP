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
filename   : kiftemplate.cxx
Date       : 09/09/2010
Purpose    : A KiF library template
Programmer : Claude ROUX
Reviewer   :
*/

//Use KIF_DLL only in Windows, if you link to the KiF DLL
//#define KIF_DLL
#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"
#include "kifmaps.h"

#ifdef WIN32
#include <math.h>
#endif

#define NMAX 100
#define TINY 1.5e-16
#define REAL double
#define ZERO (double)0.0
#define ONE (double)1.0
#define TWO (double)2.0

#ifdef UNIX
#include <math.h>
#include <float.h>
#define ABS(x) fabs((double)x)
#define TRUE true
#define FALSE false
#else
#define ABS(x) abs((double)x)
#endif

static KifType kifmatrixfloat_type;
static KifType kifmatrixinteger_type;


//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifmatrixfloat;
//This typedef defines a type "kifmatrixfloatMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifmatrixfloat::*kifmatrixfloatMethod)(KifElement*, KifDomain*, KifCallFunction*, int);

class Kifmatrixinteger;
//This typedef defines a type "kifmatrixintegerMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifmatrixinteger::*kifmatrixintegerMethod)(KifElement*, KifDomain*, KifCallFunction*, int);
//---------------------------------------------------------------------------
class KifIteratorkifmatrixfloat : public KifIteration {
public:
	Kifmatrixfloat* value;
	unsigned short r;
	unsigned short c;
	unsigned short mr, mc;

	KifIteratorkifmatrixfloat(KifCode* klc, KifElement* kp) : KifIteration(klc, kp) {
		value = NULL;
	}

	//The next methods are the ones that should be derived to add iterator capacities to Kifsparse
	KifElement* IteratorKey();

	KifElement* IteratorValue();
	long IteratorValueInteger();
	double IteratorValueFloat();

	void Next();
	KifElement* End();
	KifElement* Begin();
};
//---------------------------------------------------------------------------
class KifIteratorkifmatrixinteger : public KifIteration {
public:
	Kifmatrixinteger* value;
	unsigned short r;
	unsigned short c;
	unsigned short mr, mc;

	KifIteratorkifmatrixinteger(KifCode* klc, KifElement* kp) : KifIteration(klc, kp) {
		value = NULL;
	}

	//The next methods are the ones that should be derived to add iterator capacities to Kifsparse
	KifElement* IteratorKey();

	KifElement* IteratorValue();
	long IteratorValueInteger();
	double IteratorValueFloat();

	void Next();
	KifElement* End();
	KifElement* Begin();
};

//---------------------------------------------------------------------------
class KIFLINEINTEGER {
public:
	long* line;
	int size;

	KIFLINEINTEGER() {
		size = 0;
		line = NULL;
	}

	~KIFLINEINTEGER() {
		if (line != NULL)
			delete[] line;
	}

	void clear(int nb = 0) {
		if (line != NULL) {
			delete[] line;
			size = 0;
			line = NULL;
		}
	}

	void release() {
		line = NULL;
		size = 0;
	}

	void update(int r) {
		if (r >= size) {
			long* ln = new long[r + 1];
			memset(ln, 0, (r + 1)*sizeof(long));
			if (line != NULL) {
				memcpy(ln, line, size*sizeof(long));
				delete[] line;
			}
			size = r + 1;
			line = ln;
		}
	}

	inline long& operator [](int r) {
		if (r >= size) {
			long* ln = new long[r + 1];
			memset(ln, 0, (r + 1)*sizeof(long));
			if (line != NULL) {
				memcpy(ln, line, size*sizeof(long));
				delete[] line;
			}
			size = r + 1;
			line = ln;
		}
		return line[r];
	}

	void init(int r) {
		if (line == NULL) {
			size = r;
			line = new long[r];
			memset(line, 0, size*sizeof(long));
		}
	}

	bool check(int r) {
		if (r < 0 || r >= size || line == NULL)
			return false;
		return true;
	}
};

class KIFLINEFLOAT {
public:
	double* line;
	int size;

	KIFLINEFLOAT() {
		size = 0;
		line = NULL;
	}

	~KIFLINEFLOAT() {
		if (line != NULL)
			delete[] line;
	}

	void clear(int nb = 0) {
		if (line != NULL) {
			delete[] line;
			size = 0;
			line = NULL;
		}
	}

	void release() {
		line = NULL;
		size = 0;
	}

	void update(int r) {
		if (r >= size) {
			double* ln = new double[r + 1];
			memset(ln, 0, (r + 1)*sizeof(double));
			if (line != NULL) {
				memcpy(ln, line, size*sizeof(double));
				delete[] line;
			}
			size = r + 1;
			line = ln;
		}
	}

	inline double& operator [](int r) {
		if (r >= size) {
			double* ln = new double[r+1];
			memset(ln, 0, (r + 1)*sizeof(double));
			if (line != NULL) {
				memcpy(ln, line, size*sizeof(double));
				delete[] line;
			}
			size = r + 1;
			line = ln;
		}
		return line[r];
	}

	void init(int r) {
		if (line == NULL) {
			size = r;
			line = new double[r];
			memset(line, 0, size*sizeof(double));
		}
	}

	bool check(int r) {
		if (r < 0 || r >= size || line == NULL)
			return false;
		return true;
	}
};

template<class Z> class KIFMATRIX {
public:
	Z* matrix;
	int size;

	KIFMATRIX() {
		matrix = NULL;
		size = 0;
	}

	KIFMATRIX(int r) {
		matrix = NULL;
		size = 0;
		init(r);
	}

	void clear(int nb = 0) {
		if (matrix != NULL) {
			delete[] matrix;
			matrix = NULL;
			size = 0;
		}
	}

	~KIFMATRIX() {
		clear();
	}

	void update(int r, int c) {
		if (r >= size) {
			Z* mat = new Z[r + 1];
			memset(mat, NULL, (r + 1)*sizeof(Z));
			if (matrix != NULL) {
				for (int i = 0; i < size; i++) {
					mat[i].line = matrix[i].line;
					mat[i].size = matrix[i].size;
					matrix[i].release();
				}
				delete[] matrix;
			}
			matrix = mat;
			size = r + 1;
		}

		for (int i = 0; i<size; i++)
			matrix[i].update(c);
	}

	void init(int r) {
		if (matrix == NULL && r > 0) {
			size = r;
			matrix = new Z[size];
		}
	}

	inline Z& operator[](int r) {
		if (r >= size) {
			Z* mat = new Z[r + 1];
			memset(mat, NULL, (r + 1)*sizeof(Z));
			if (matrix != NULL) {
				for (int i = 0; i < size; i++) {
					mat[i].line = matrix[i].line;
					mat[i].size= matrix[i].size;
					matrix[i].release();					
				}
				delete[] matrix;
			}
			matrix = mat;
			size = r + 1;
		}
		return matrix[r];
	}

	bool check(int r) {
		if (r < 0 || r >= size || matrix == NULL || matrix[r].line == NULL)
			return false;
		return true;
	}
};

//--------------------------------------------------------------------

class Kifmatrixinteger : public KifBasic {

public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static map<string, string> infomethods;

	static map<short, KifCallMethod> kifexportedmethods;
	static map<short, kifmatrixintegerMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	KIFMATRIX<KIFLINEINTEGER> values;

	unsigned short rowsize;
	unsigned short columnsize;

	//---------------------------------------------------------------------------------------------------------------------
	Kifmatrixinteger(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, kifmatrixinteger_type) {
		rowsize = 0;
		columnsize = 0;
	}


	void cleaning() {
		values.clear();
		rowsize = 0;
		columnsize = 0;
	}

	void Clear() {
		//Your reset code here
		cleaning();
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
		cleaning();
	}

	~Kifmatrixinteger() {
		cleaning();
	}


	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return kifmatrixinteger_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifmatrixinteger* kperso = new Kifmatrixinteger(KifCurrent(), NULL);
		//Your initialisations here
		populate(kperso);
		return kperso;
	}

	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//This description is facultative
	//iterator it=mydb;
	KifElement* Newiterator(bool rev) {
		KifIteratorkifmatrixinteger* iter = new KifIteratorkifmatrixinteger(kifcode, NULL);
		iter->reverse = rev;
		iter->value = this;
		//Your initialisation here for your iterator object
		return iter;
	}
	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	Exported bool initialconstructor() {
		return true;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	Exported KifElement* constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize() == 0)
			return this;


		//Your first parameter is at position 0 etc...
		rowsize = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc)->Integer(); //would match i as in the example above: mynewtype toto(i,j);
		columnsize = 1;
		if (params->InstructionSize() == 2)
			columnsize = params->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc)->Integer(); //would match j as in the example above: mynewtype toto(i,j);	

		values.init(rowsize);
		return this;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestringraw(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------

	void Setreference(int inc) {
		reference += inc;
		Putpopped(0);
	}

	void Resetreference(int inc = 1) {

		ThreadLock _lock(type);
		reference -= inc;
		if (reference <= 0) {
			if (popped == 1)
				popped = 0;
			else {
				cleaning();
				KifElement::Resetreference();
			}
		}
	}

	void init(int rs, int cs) {
		rowsize = rs;
		columnsize = cs;
		values.init(rs);
	}

	void populate(unsigned short r, unsigned short c, long val) {
		values[r][c] = val;
	}

	void populate(KifElement* krow, KifElement* kcol, KifElement* value) {
		unsigned short r = krow->Integer();
		unsigned short c = kcol->Integer();
		long val = value->Integer();

		values[r][c] = val;
	}


	void populate(Kifmatrixinteger* ke) {
		ke->cleaning();
		ke->init(rowsize, columnsize);
		for (unsigned short r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (unsigned short c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						ke->values[r][c] = values[r][c];
				}
			}
		}
	}

	long val(unsigned short r, unsigned short c) {
		if (values.check(r) && values[r].check(c))
			return values[r][c];
		return 0;
	}

	int LUDCMP(int n, VECTE<int>& INDX, int& d, Kifmatrixinteger& m) {

		d = 1;
		double AMAX, DUM, SUM;
		int  I, IMAX, J, K;
		vector<double> VV;

		for (I = 0; I < n; I++)  {
			AMAX = 0.0;
			for (J = 0; J<n; J++)  {
				SUM = m.val(I, J);
				if (ABS(SUM) > AMAX)
					AMAX = ABS(SUM);
			}

			if (AMAX < TINY)
				return 1;
			VV.push_back(1.0 / AMAX);
		} // i loop 

		for (J = 0; J < n; J++)  {

			for (I = 0; I < J; I++)  {
				SUM = m.val(I, J);
				for (K = 0; K < I; K++)
					SUM = SUM - m.val(I, K)*m.val(K, J);
				m.values[I][J] = SUM;
			} // i loop 
			AMAX = 0.0;

			for (I = J; I < n; I++)  {
				SUM = m.val(I, J);
				for (K = 0; K < J; K++)
					SUM = SUM - m.val(I, K)*m.val(K, J);
				m.values[I][J] = SUM;
				DUM = VV[I] * ABS(SUM);
				if (DUM >= AMAX) {
					IMAX = I;
					AMAX = DUM;
				}
			} // i loop

			if (J != IMAX)  {
				for (K = 0; K < n; K++)  {
					DUM = m.val(IMAX, K);
					m.values[IMAX][K] = m.val(J, K);
					m.values[J][K] = DUM;
				} // k loop 
				d = -d;
				VV[IMAX] = VV[J];
			}

			INDX[J] = IMAX;

			if (ABS(m.val(J, J)) < TINY)
				m.values[J][J] = TINY;

			if (J != n - 1)  {
				DUM = 1.0 / m.val(J, J);
				for (I = J + 1; I < n; I++)
					m.values[I][J] *= DUM;
			}
		} // j loop 

		return 0;

	} // subroutine LUDCMP 

	void LUBKSB(int n, VECTE<int>& INDX, VECTE<double>& B, Kifmatrixinteger& m)  {
		double SUM;
		int  I, II, J, LL;

		II = -1;

		for (I = 0; I < n; I++)  {
			LL = INDX[I];
			SUM = B[LL];
			B[LL] = B[I];
			if (II != -1)
			for (J = II; J < I; J++)
				SUM = SUM - m.val(I, J)*B[J];
			else
			if (SUM != 0.0)
				II = I;
			B[I] = SUM;
		} // i loop

		for (I = n - 1; I >= 0; I--)  {
			SUM = B[I];
			if (I < n - 1)  {
				for (J = I + 1; J < n; J++)
					SUM = SUM - m.val(I, J)*B[J];
			}
			B[I] = SUM / m.val(I, I);
		} // i loop 

	} // LUBKSB    


	bool inversion(Kifmatrixinteger& Y) {
		//else Local decomposition

		Y.init(rowsize, columnsize);

		Kifmatrixinteger m(NULL, NULL);
		populate(&m);

		VECTE<int> INDX(rowsize);
		int id;
		//call LU decomposition, on the matrixbyrow values
		int rc = LUDCMP(rowsize, INDX, id, m);
		if (rc == 1)
			return false;


		VECTE<double> temp(rowsize);
		int i;
		//We create an identity matrix, which will contain the final result... 
		for (i = 0; i < rowsize; i++)
			Y.values[i][i] = 1;

		for (int j = 0; j < rowsize; j++) {
			for (i = 0; i < rowsize; i++)
				temp[i] = Y.val(i, j);
			LUBKSB(rowsize, INDX, temp, m);
			for (i = 0; i < rowsize; i++) {
				Y.values[i][j] = temp[i];
			}
		}

		return true;
	}

	KifElement* MethodInversion(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		if (rowsize != columnsize)
			return kifNULL;

		Kifmatrixinteger* Y = new Kifmatrixinteger(kifcode, NULL);
		if (!inversion(*Y)) {
			Y->Release();
			return kifNULL;
		}
		return Y;
	}

	int determinant(unsigned short rsize, unsigned short csize, KIFMATRIX<KIFLINEINTEGER>& m) {
		if (rsize == 2 && csize == 2) {
			//then in that case
			if (m.check(0) && m.check(1))
				return (m[0][0] * m[1][1] - m[1][0] * m[0][1]);

			return 0;
		}


		int i;
		for (i = 0; i < rsize; i++) {
			if (m.check(i) == false)
				return 0;
		}

		i = 0;
		int det = 0;
		for (int j = 0; j < rsize; j++) {
			if (!m[i].check(j) || m[i][j] == 0)
				continue;

			KIFMATRIX<KIFLINEINTEGER> sub(rsize - 1);

			int pc = 0;
			int pr = 0;
			for (int r = 0; r < rsize; r++) {
				if (r == i)
					continue;
				pc = 0;
				for (int c = 0; c < csize; c++) {
					if (c == j)
						continue;
					sub[pr][pc] = m[r][c];
					pc++;
				}
				pr++;
			}
			int sg = (int)pow(-1, (i + j + 2));
			det += m[i][j] * sg*determinant(rsize - 1, csize - 1, sub);
		}
		return det;
	}


	KifElement* MethodDeterminant(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (columnsize < 2 || columnsize != rowsize)
			return 0;

		int det = determinant(rowsize, columnsize, values);
		return kifcode->Provideinteger(det);
	}


	KifElement* MethodSum(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		long v = 0;
		for (unsigned short r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (unsigned short c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						v += values[r][c];
				}
			}
		}
		return kifcode->Provideinteger(v);
	}

	KifElement* MethodProduct(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		long v = 1;
		for (unsigned short r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (unsigned short c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						v *= values[r][c];
				}
			}
		}
		return kifcode->Provideinteger(v);
	}


	Kifmatrixinteger* duplicate() {
		Kifmatrixinteger* ke = new Kifmatrixinteger(kifcode, NULL);

		for (unsigned short r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (unsigned short c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						ke->values[r][c] = values[r][c];
				}
			}
		}

		ke->rowsize = rowsize;
		ke->columnsize = columnsize;
		return ke;
	}

	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
		threadblocinit;

		ThreadLock _lock(type);
		if (ke == this)
			return true;
		if (idx == NULL) {
			Clear();
			if (ke->type != kifmatrixinteger_type)
				return true;
			((Kifmatrixinteger*)ke)->populate(this);
			return true;
		}

		KifElement* keyleft;
		KifElement* keyright;
		if (idx->type != kifIndex) {
			kifcode->Returnerror("SPM(100): Wrong index");
			return true;
		}

		//Two possibilities:
		//both indexes
		bool interval = ((KifIndex*)idx)->interval;
		KifIndex* kind = (KifIndex*)idx;
		if (interval == true && kind->key != kifNULL && kind->right != kifNULL) {
			keyleft = kind->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			keyright = kind->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			populate(keyleft, keyright, ke);
			keyleft->Release();
			keyright->Release();
			return true;
		}
		unsigned short r, c;
		KifIteration* itr;
		//it should be a map as value or a matriceline
		if (ke->aVectorContainer()) {
			unsigned short itx = 0;
			//then it means that only the column key is present
			if (kind->right != NULL && kind->right != kifNULL) {
				keyleft = kind->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
				c = keyleft->Integer();
				keyleft->Release();

				itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
					populate(itx, c, itr->IteratorValueInteger());
					itx++;
				}
				itr->Release();
				return true;
			}

			//We have the line, not the column
			if (kind->key != NULL && kind->key != kifNULL) {
				keyleft = kind->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
				r = keyleft->Integer();
				keyleft->Release();
				itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
					populate(r, itx, itr->IteratorValueInteger());
					itx++;
				}
				itr->Release();
				return true;
			}
		}
		else
		if (ke->aMapContainer()) {
			//then it means that only the column key is present
			if (kind->right != NULL && kind->right != kifNULL) {
				keyleft = kind->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
				c = keyleft->Integer();
				keyleft->Release();
				itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
					r = itr->IteratorKeyInteger();
					populate(r, c, itr->IteratorValueInteger());
				}
				itr->Release();
				return true;
			}

			//We have the line, not the column
			if (kind->key != NULL && kind->key != kifNULL) {
				keyleft = kind->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
				r = keyleft->Integer();
				keyleft->Release();
				itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
					c = itr->IteratorKeyInteger();
					populate(r, c, itr->IteratorValueInteger());
				}
				itr->Release();
				return true;
			}
		}
		kifcode->Returnerror("MAT(131): Wrong index or Wrong value (recipient should be a map or a mvector)");
		return true;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
		ThreadLock _lock(type);

		if (idx == NULL)
			return this;

		if (idx->type != kifIndex)
			return kifcode->Returnerror("SPM(100): Wrong index");

		//Two possibilities:
		//both indexes
		bool interval = ((KifIndex*)idx)->interval;
		KifIndex* kind = (KifIndex*)idx;
		unsigned short r, c;
		if (interval == true && kind->key != kifNULL && kind->right != kifNULL) {
			r = kind->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc)->Integer();
			c = kind->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc)->Integer();
			if (values.check(r) && values[r].check(c))
				return kifcode->Provideinteger(values[r][c]);
			return kifZERO;
		}

		//then it means that only the column key is present
		if (kind->right != NULL && kind->right != kifNULL) {
			c = kind->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc)->Integer();
			if (c < 0 || c >= columnsize)
				return kifNULL;

			KifElement* kvect = Selectvectorfloat(kind, contextualpattern);

			for (unsigned short r = 0; r < rowsize; r++) {
				if (values.check(r) && values[r].check(c))
					kvect->storevalue(values[r][c]);
				else
					kvect->storevalue((long)0);
			}
			return kvect;
		}

		if (kind->key != NULL && kind->key != kifNULL) {
			//We have the line, not the column
			r = kind->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc)->Integer();
			if (r < 0 || r >= rowsize)
				return kifNULL;

			KifElement* kvect = Selectvectorfloat(kind, contextualpattern);
			if (values.check(r) == false)
				return kvect;

			KIFLINEINTEGER& line = values[r];
			for (unsigned short i = 0; i < columnsize; i++) {
				if (line.check(i))
					kvect->storevalue(line[i]);
				else
					kvect->storevalue((long)0);
			}
			return kvect;
		}
		return this;
	}

	KifElement* MethodDuplicate(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		return duplicate();
	}

	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.

	KifElement* MethodMatrixSize(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (callfunc->Size() == 0) {
			KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
			kvect->storevalue((long)rowsize);
			kvect->storevalue((long)columnsize);
			return kvect;
		}

		//we use the values to set the new matrix size...
		int szr = callfunc->Evaluatethread(0, domain, idthread)->Integer();
		int szc = callfunc->Evaluatethread(1, domain, idthread)->Integer();
		rowsize = szr;
		columnsize = szc;
		values.update(szr, szc);
		return kifTRUE;
	}


	KifElement* MethodTransposed(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		Kifmatrixinteger* res = new Kifmatrixinteger(kifcode, NULL);

		for (unsigned short r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (unsigned short c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						res->populate(c, r, values[r][c]);
				}
			}
		}

		res->rowsize = columnsize;
		res->columnsize = rowsize;

		return res;
	}

	//-------------------------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------------------------


	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		KiFArityError(name)

			return (this->*linkedmethods[name])(contextualpattern, domain, callfunc, idthread);
	}



	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
	long Size() {
		return (rowsize*columnsize);
	}

	//How to interprete your class according to the basic following types:
	string String() {
		stringstream buff;

		for (int r = 0; r < rowsize; r++) {
			for (int c = 0; c < columnsize; c++) {
				if (c)
					buff << ",";
				if (values.check(r) && values[r].check(c))
					buff << values[r][c];
				else
					buff << "0";
			}
			buff << endl;
		}

		return buff.str();
	}

	long Integer() { return Size(); }
	double Float() { return Size(); }
	bool Boolean() { return true; }

	//You can overcome the following function behaviour
	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		return kifNULL;
	}
	//union of two sets
	KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifmatrixinteger_type || b->type != kifmatrixinteger_type) {
			Clear();
			return this;
		}

		Kifmatrixinteger* ma = (Kifmatrixinteger*)a;
		Kifmatrixinteger* mb = (Kifmatrixinteger*)b;
		Kifmatrixinteger* res = new Kifmatrixinteger(kifcode, NULL);

		long v;
		for (unsigned short r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (unsigned short c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = (long)ma->values[r][c] | (long)mb->values[r][c];
						res->populate(r, c, (long)v);
					}
				}
			}
		}

		return res;
	}

	//intersection of two sets
	KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifmatrixinteger_type || b->type != kifmatrixinteger_type) {
			Clear();
			return this;
		}

		Kifmatrixinteger* ma = (Kifmatrixinteger*)a;
		Kifmatrixinteger* mb = (Kifmatrixinteger*)b;
		Kifmatrixinteger* res = new Kifmatrixinteger(kifcode, NULL);

		long v;
		for (unsigned short r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (unsigned short c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = (long)ma->values[r][c] & (long)mb->values[r][c];
						res->populate(r, c, (long)v);
					}
				}
			}
		}
		return res;
	}

	KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifmatrixinteger_type || b->type != kifmatrixinteger_type) {
			Clear();
			return this;
		}

		Kifmatrixinteger* ma = (Kifmatrixinteger*)a;
		Kifmatrixinteger* mb = (Kifmatrixinteger*)b;
		Kifmatrixinteger* res = new Kifmatrixinteger(kifcode, NULL);

		long v;
		for (unsigned short r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (unsigned short c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = (long)ma->values[r][c] ^ (long)mb->values[r][c];
						res->populate(r, c, (long)v);
					}
				}
			}
		}

		return res;
	}

	//Beware that a might be the current element
	//plus: a+b
	KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

		if (a->type != kifmatrixinteger_type || b->type != kifmatrixinteger_type) {
			Clear();
			return this;
		}

		Kifmatrixinteger* ma = (Kifmatrixinteger*)a;
		Kifmatrixinteger* mb = (Kifmatrixinteger*)b;
		Kifmatrixinteger* res = new Kifmatrixinteger(kifcode, NULL);
		long v;
		if (b->type != kifmatrixinteger_type) {
			long vb = b->Integer();
			for (unsigned short r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (unsigned short c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] + vb;
							res->populate(r, c, v);
						}
					}
				}
			}
			return res;
		}

		for (unsigned short r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (unsigned short c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = ma->values[r][c] + mb->values[r][c];
						res->populate(r, c, v);
					}
				}
			}
		}

		return res;
	}

	//minus: a-b
	KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifmatrixinteger_type || b->type != kifmatrixinteger_type) {
			Clear();
			return this;
		}

		Kifmatrixinteger* ma = (Kifmatrixinteger*)a;
		Kifmatrixinteger* mb = (Kifmatrixinteger*)b;
		Kifmatrixinteger* res = new Kifmatrixinteger(kifcode, NULL);

		long v;
		if (b->type != kifmatrixinteger_type) {
			long vb = b->Integer();
			for (unsigned short r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (unsigned short c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] - vb;
							res->populate(r, c, v);
						}
					}
				}
			}
			return res;
		}


		for (unsigned short r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (unsigned short c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = ma->values[r][c] - mb->values[r][c];
						res->populate(r, c, v);
					}
				}
			}
		}
		return res;
	}

	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifmatrixinteger_type) {
			Clear();
			return this;
		}
		Kifmatrixinteger* ma = (Kifmatrixinteger*)a;
		Kifmatrixinteger* res = new Kifmatrixinteger(kifcode, NULL);

		long v;
		unsigned short r, c, k;

		if (b->type != kifmatrixinteger_type) {
			long vb = b->Integer();
			for (r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] * vb;
							res->populate(r, c, v);
						}
					}
				}
			}
			return res;
		}

		Kifmatrixinteger* mb = (Kifmatrixinteger*)b;
		if (ma->columnsize != mb->rowsize)
			return kifcode->Returnerror("MAT(209): Matrix size mismatch");

		res->init(ma->rowsize, mb->columnsize);
		for (r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r)) {
				for (c = 0; c < mb->columnsize; c++) {
					v = 0;
					for (k = 0; k < ma->columnsize; k++) {
						if (ma->values[r].check(k) && mb->values.check(k) && mb->values[k].check(c))
							v += ma->values[r][k] * mb->values[k][c];
					}
					res->populate(r, c, v);
				}
			}
		}

		return res;
	}


	//Division: a/b
	KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifmatrixinteger_type || b->type != kifmatrixinteger_type) {
			Clear();
			return this;
		}

		Kifmatrixinteger* ma = (Kifmatrixinteger*)a;
		Kifmatrixinteger* res = new Kifmatrixinteger(kifcode, NULL);

		long v;
		unsigned short r;
		unsigned short c;

		if (b->type != kifmatrixinteger_type) {
			long vb = b->Integer();
			if (vb == 0)
				return kifcode->Returnerror("MAT(203): Cannot divide by 0");

			for (r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] / vb;
							res->populate(r, c, v);
						}
					}
				}
			}
			return res;
		}

		Kifmatrixinteger* mb = (Kifmatrixinteger*)b;

		Kifmatrixinteger inverted(NULL, NULL);

		if (!mb->inversion(inverted))
			return kifcode->Returnerror("MAT(202): Cannot divide with this matrix");

		if (ma->columnsize != inverted.rowsize)
			return kifcode->Returnerror("MAT(202): Cannot divide with this matrix");

		unsigned short k;
		for (r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r)) {
				for (c = 0; c < inverted.columnsize; c++) {
					v = 0;
					for (k = 0; k < ma->columnsize; k++) {
						if (ma->values[r].check(k) && inverted.values.check(k) && inverted.values[k].check(c))
							v += ma->values[r][k] * inverted.values[k][c];
					}
					res->populate(r, c, v);
				}
			}
		}

		res->rowsize = ma->rowsize;
		res->columnsize = inverted.columnsize;
		return res;

	}

	//Comparison functions that might also be modified
	//inferior: this<a
	KifElement* less(KifElement* a) {
		return kifFALSE;
	}
	//superior: this>a
	KifElement* more(KifElement* a) {
		return kifFALSE;
	}

	//equality: this==a
	KifElement* same(KifElement* a) {
		if (a->type != kifmatrixinteger_type)
			return kifFALSE;

		Kifmatrixinteger* ma = (Kifmatrixinteger*)a;
		if (ma->rowsize != rowsize && ma->columnsize != columnsize)
			return kifFALSE;

		for (unsigned short r = 0; r < rowsize; r++) {
			if (ma->values.check(r)) {
				if (!values.check(r))
					return kifFALSE;
				for (unsigned short c = 0; c < columnsize; c++) {
					if (ma->values[r].check(c)) {
						if (!values[r].check(c))
							return kifFALSE;

						if (ma->values[r][c] != values[r][c])
							return kifFALSE;
					}
					else {
						if (values[r].check(c))
							return kifFALSE;
					}
				}
			}
			else
			if (values.check(r))
				return kifFALSE;
		}
		return kifTRUE;
	}

	//different: this!=a
	KifElement* different(KifElement* a) {
		if (same(a) == kifFALSE)
			return kifTRUE;
		return kifFALSE;
	}

	//inferior or equal: this<=a
	KifElement* lessequal(KifElement* a) {
		return kifFALSE;
	}

	//superior or equal: this>=a
	KifElement* moreequal(KifElement* a) {
		return kifFALSE;
	}

};

//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifmatrixinteger::kifexportedmethods;
map<short, kifmatrixintegerMethod>  Kifmatrixinteger::linkedmethods;
map<string, string> Kifmatrixinteger::infomethods;
hmap<short, bool> Kifmatrixinteger::methodassigned;

//---------------------------------------------------------------------------------------------------------------------

//The next methods are the ones that should be derived to add iterator capacities to Kifsparse
KifElement* KifIteratorkifmatrixinteger::IteratorKey() {
	char buff[100];
	sprintf_s(buff, 100, "%d:%d", r, c);
	return kifcode->Providestringraw(buff);
}

KifElement* KifIteratorkifmatrixinteger::IteratorValue() {
	long v = 0;
	if (value->values.check(r) && value->values[r].check(c))
		v = value->values[r][c];

	return kifcode->Provideinteger(v);
}

long KifIteratorkifmatrixinteger::IteratorValueInteger() {
	long v = 0;
	if (value->values.check(r) && value->values[r].check(c))
		v = value->values[r][c];
	return v;
}

double KifIteratorkifmatrixinteger::IteratorValueFloat() {
	double v = 0;
	if (value->values.check(r) && value->values[r].check(c))
		v = (double)value->values[r][c];
	return v;
}

void KifIteratorkifmatrixinteger::Next() {
	r++;
	if (r == mr) {
		mc++;
		r = 0;
	}
}

KifElement* KifIteratorkifmatrixinteger::End() {
	if (r == mr && c == mc)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifIteratorkifmatrixinteger::Begin() {
	r = 0;
	c = 0;
	mr = value->rowsize;
	mc = value->columnsize;

	if (!mr && !mc)
		return kifFALSE;

	return kifTRUE;
}
//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifmatrixintegerElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifmatrixinteger* local = new Kifmatrixinteger(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitializationMatrixInteger(KifCode* kifcode, string name, kifmatrixintegerMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifmatrixinteger::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifmatrixinteger::linkedmethods[idname] = func;
	Kifmatrixinteger::methodassigned[idname] = true;
	Kifmatrixinteger::infomethods[name] = infos;
}

//--------------------------------------------------------------------

class Kifmatrixfloat : public KifBasic {

public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static map<string, string> infomethods;

	static map<short, KifCallMethod> kifexportedmethods;
	static map<short, kifmatrixfloatMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	KIFMATRIX<KIFLINEFLOAT> values;

	unsigned short rowsize;
	unsigned short columnsize;

	//---------------------------------------------------------------------------------------------------------------------
	Kifmatrixfloat(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, kifmatrixfloat_type) {
		rowsize = 0;
		columnsize = 0;
	}


	void cleaning() {
		values.clear();
		rowsize = 0;
		columnsize = 0;
	}

	void Clear() {
		//Your reset code here
		cleaning();
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
		cleaning();
	}

	~Kifmatrixfloat() {
		cleaning();
	}


	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return kifmatrixfloat_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifmatrixfloat* kperso = new Kifmatrixfloat(KifCurrent(), NULL);
		//Your initialisations here
		populate(kperso);
		return kperso;
	}

	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//This description is facultative
	//iterator it=mydb;
	KifElement* Newiterator(bool rev) {
		KifIteratorkifmatrixfloat* iter = new KifIteratorkifmatrixfloat(kifcode, NULL);
		iter->reverse = rev;
		iter->value = this;
		//Your initialisation here for your iterator object
		return iter;
	}
	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	Exported bool initialconstructor() {
		return true;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	Exported KifElement* constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize() == 0)
			return this;


		//Your first parameter is at position 0 etc...
		rowsize = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc)->Integer(); //would match i as in the example above: mynewtype toto(i,j);
		columnsize = 1;
		if (params->InstructionSize() == 2)
			columnsize = params->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc)->Integer(); //would match j as in the example above: mynewtype toto(i,j);	

		values.init(rowsize);
		return this;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestringraw(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------

	void Setreference(int inc) {
		reference += inc;
		Putpopped(0);
	}

	void Resetreference(int inc = 1) {

		ThreadLock _lock(type);
		reference -= inc;
		if (reference <= 0) {
			if (popped == 1)
				popped = 0;
			else {
				cleaning();
				KifElement::Resetreference();
			}
		}
	}

	void init(int rs, int cs) {
		rowsize = rs;
		columnsize = cs;
		values.init(rs);
	}

	void populate(unsigned short r,unsigned short c,double val) {
		values[r][c] = val;
	}

	void populate(KifElement* krow, KifElement* kcol, KifElement* value) {
		unsigned short r = krow->Integer();
		unsigned short c = kcol->Integer();
		double val = value->Float();

		values[r][c] = val;
	}


	void populate(Kifmatrixfloat* ke) {
		ke->cleaning();
		ke->init(rowsize, columnsize);
		for (unsigned short r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (unsigned short c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						ke->values[r][c] = values[r][c];
				}
			}
		}
	}

	double val(unsigned short r, unsigned short c) {
		if (values.check(r) && values[r].check(c))
			return values[r][c];
		return 0;
	}

	int LUDCMP(int n, VECTE<int>& INDX, int& d, Kifmatrixfloat& m) {

		d = 1;
		double AMAX, DUM, SUM;
		int  I, IMAX, J, K;
		vector<double> VV;

		for (I = 0; I < n; I++)  {
			AMAX = 0.0;
			for (J = 0; J<n; J++)  {
				SUM = m.val(I, J);
				if (ABS(SUM) > AMAX)
					AMAX = ABS(SUM);
			}

			if (AMAX < TINY)
				return 1;
			VV.push_back(1.0 / AMAX);
		} // i loop 

		for (J = 0; J < n; J++)  {

			for (I = 0; I < J; I++)  {
				SUM = m.val(I, J);
				for (K = 0; K < I; K++)
					SUM = SUM - m.val(I, K)*m.val(K, J);
				m.values[I][J] = SUM;
			} // i loop 
			AMAX = 0.0;

			for (I = J; I < n; I++)  {
				SUM = m.val(I, J);
				for (K = 0; K < J; K++)
					SUM = SUM - m.val(I, K)*m.val(K, J);
				m.values[I][J] = SUM;
				DUM = VV[I] * ABS(SUM);
				if (DUM >= AMAX) {
					IMAX = I;
					AMAX = DUM;
				}
			} // i loop

			if (J != IMAX)  {
				for (K = 0; K < n; K++)  {
					DUM = m.val(IMAX, K);
					m.values[IMAX][K] = m.val(J, K);
					m.values[J][K] = DUM;
				} // k loop 
				d = -d;
				VV[IMAX] = VV[J];
			}

			INDX[J] = IMAX;

			if (ABS(m.val(J, J)) < TINY)
				m.values[J][J] = TINY;

			if (J != n - 1)  {
				DUM = 1.0 / m.val(J, J);
				for (I = J + 1; I < n; I++)
					m.values[I][J] *= DUM;
			}
		} // j loop 

		return 0;

	} // subroutine LUDCMP 

	void LUBKSB(int n, VECTE<int>& INDX, VECTE<double>& B, Kifmatrixfloat& m)  {
		double SUM;
		int  I, II, J, LL;

		II = -1;

		for (I = 0; I < n; I++)  {
			LL = INDX[I];
			SUM = B[LL];
			B[LL] = B[I];
			if (II != -1)
			for (J = II; J < I; J++)
				SUM = SUM - m.val(I, J)*B[J];
			else
			if (SUM != 0.0)
				II = I;
			B[I] = SUM;
		} // i loop

		for (I = n - 1; I >= 0; I--)  {
			SUM = B[I];
			if (I < n - 1)  {
				for (J = I + 1; J < n; J++)
					SUM = SUM - m.val(I, J)*B[J];
			}
			B[I] = SUM / m.val(I, I);
		} // i loop 

	} // LUBKSB    


	bool inversion(Kifmatrixfloat& Y) {
		//else Local decomposition

		Y.init(rowsize, columnsize);

		Kifmatrixfloat m(NULL, NULL);
		populate(&m);

		VECTE<int> INDX(rowsize);
		int id;
		//call LU decomposition, on the matrixbyrow values
		int rc = LUDCMP(rowsize, INDX, id, m);
		if (rc == 1)
			return false;


		VECTE<double> temp(rowsize);
		int i;
		//We create an identity matrix, which will contain the final result... 
		for (i = 0; i < rowsize; i++)
			Y.values[i][i] = 1;		

		for (int j = 0; j < rowsize; j++) {
			for (i = 0; i < rowsize; i++)
				temp[i] = Y.val(i, j);
			LUBKSB(rowsize, INDX, temp, m);
			for (i = 0; i < rowsize; i++) {
				Y.values[i][j] = temp[i];
			}
		}

		return true;
	}

	KifElement* MethodInversion(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		if (rowsize != columnsize)
			return kifNULL;

		Kifmatrixfloat* Y = new Kifmatrixfloat(kifcode, NULL);		
		if (!inversion(*Y)) {
			Y->Release();
			return kifNULL;
		}
		return Y;
	}
	
	int determinant(unsigned short rsize, unsigned short csize, KIFMATRIX<KIFLINEFLOAT>& m) {
		if (rsize == 2 && csize == 2) {
			//then in that case
			if (m.check(0) && m.check(1))
					return (m[0][0] * m[1][1] - m[1][0] * m[0][1]);

			return 0;
		}
				

		int i;
		for (i = 0; i < rsize; i++) {
			if (m.check(i) == false)
				return 0;
		}

		i = 0;
		int det = 0;
		for (int j = 0; j < rsize; j++) {
			if (!m[i].check(j) || m[i][j] == 0)
				continue;

			KIFMATRIX<KIFLINEFLOAT> sub(rsize - 1);			

			int pc = 0;
			int pr = 0;
			for (int r = 0; r < rsize; r++) {
				if (r == i)
					continue;
				pc = 0;
				for (int c = 0; c < csize; c++) {
					if (c == j)
						continue;
					sub[pr][pc] = m[r][c];
					pc++;
				}
				pr++;
			}
			int sg = (int)pow(-1, (i + j + 2));
			det += m[i][j] * sg*determinant(rsize - 1, csize - 1, sub);
		}
		return det;
	}


	KifElement* MethodDeterminant(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (columnsize < 2 || columnsize != rowsize)
			return 0;

		int det = determinant(rowsize, columnsize, values);
		return kifcode->Provideinteger(det);
	}


	KifElement* MethodSum(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		double v = 0;
		for (unsigned short r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (unsigned short c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						v += values[r][c];
				}
			}
		}
		return kifcode->Providefloat(v);
	}

	KifElement* MethodProduct(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		double v = 1;
		for (unsigned short r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (unsigned short c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						v *= values[r][c];
				}
			}
		}
		return kifcode->Providefloat(v);
	}


	Kifmatrixfloat* duplicate() {
		Kifmatrixfloat* ke = new Kifmatrixfloat(kifcode, NULL);

		for (unsigned short r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (unsigned short c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						ke->values[r][c] = values[r][c];
				}
			}
		}

		ke->rowsize = rowsize;
		ke->columnsize = columnsize;
		return ke;
	}

	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
		threadblocinit;

		ThreadLock _lock(type);
		if (ke == this)
			return true;
		if (idx == NULL) {
			Clear();
			if (ke->type != kifmatrixfloat_type)
				return true;
			((Kifmatrixfloat*)ke)->populate(this);
			return true;
		}

		KifElement* keyleft;
		KifElement* keyright;
		if (idx->type != kifIndex) {
			kifcode->Returnerror("SPM(100): Wrong index");
			return true;
		}

		//Two possibilities:
		//both indexes
		bool interval = ((KifIndex*)idx)->interval;
		KifIndex* kind = (KifIndex*)idx;
		if (interval == true && kind->key != kifNULL && kind->right != kifNULL) {
			keyleft = kind->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			keyright = kind->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			populate(keyleft, keyright, ke);
			keyleft->Release();
			keyright->Release();
			return true;
		}
		unsigned short r, c;
		KifIteration* itr;
		//it should be a map as value or a matriceline
		if (ke->aVectorContainer()) {
			unsigned short itx = 0;
			//then it means that only the column key is present
			if (kind->right != NULL && kind->right != kifNULL) {
				keyleft = kind->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
				c = keyleft->Integer();
				keyleft->Release();

				itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
					populate(itx, c, itr->IteratorValueFloat());
					itx++;
				}
				itr->Release();
				return true;
			}

			//We have the line, not the column
			if (kind->key != NULL && kind->key != kifNULL) {
				keyleft = kind->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
				r = keyleft->Integer();
				keyleft->Release();
				itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
					populate(r, itx, itr->IteratorValueFloat());
					itx++;
				}
				itr->Release();
				return true;
			}
		}
		else
		if (ke->aMapContainer()) {
			//then it means that only the column key is present
			if (kind->right != NULL && kind->right != kifNULL) {
				keyleft = kind->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
				c = keyleft->Integer();
				keyleft->Release();
				itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
					r = itr->IteratorKeyInteger();
					populate(r, c, itr->IteratorValueFloat());
				}
				itr->Release();
				return true;
			}

			//We have the line, not the column
			if (kind->key != NULL && kind->key != kifNULL) {
				keyleft = kind->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
				r = keyleft->Integer();
				keyleft->Release();
				itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
					c = itr->IteratorKeyInteger();
					populate(r, c, itr->IteratorValueFloat());
				}
				itr->Release();
				return true;
			}
		}
		kifcode->Returnerror("MAT(131): Wrong index or Wrong value (recipient should be a map or a mvector)");
		return true;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
		ThreadLock _lock(type);

		if (idx == NULL)
			return this;

		if (idx->type != kifIndex)
			return kifcode->Returnerror("SPM(100): Wrong index");

		//Two possibilities:
		//both indexes
		bool interval = ((KifIndex*)idx)->interval;
		KifIndex* kind = (KifIndex*)idx;
		unsigned short r, c;
		if (interval == true && kind->key != kifNULL && kind->right != kifNULL) {
			r = kind->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc)->Integer();
			c = kind->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc)->Integer();
			if (values.check(r) && values[r].check(c))
				return kifcode->Providefloat(values[r][c]);
			return kifZERO;
		}
		
		//then it means that only the column key is present
		if (kind->right != NULL && kind->right != kifNULL) {
			c = kind->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc)->Integer();
			if (c < 0 || c >= columnsize)
				return kifNULL;

			KifElement* kvect = Selectvectorfloat(kind, contextualpattern);

			for (unsigned short r = 0; r < rowsize; r++) {
				if (values.check(r) && values[r].check(c))
					kvect->storevalue(values[r][c]);
				else
					kvect->storevalue((double)0);
			}
			return kvect;
		}

		if (kind->key != NULL && kind->key != kifNULL) {
			//We have the line, not the column
			r = kind->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc)->Integer();
			if (r < 0 || r >= rowsize)
				return kifNULL;

			KifElement* kvect = Selectvectorfloat(kind, contextualpattern);
			if (values.check(r) == false)
				return kvect;

			KIFLINEFLOAT& line = values[r];
			for (unsigned short i = 0; i < columnsize; i++) {
				if (line.check(i))
					kvect->storevalue(line[i]);
				else
					kvect->storevalue((double)0);
			}
			return kvect;
		}
		return this;
	}

	KifElement* MethodDuplicate(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		return duplicate();
	}

	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.

	KifElement* MethodMatrixSize(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (callfunc->Size() == 0) {
			KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
			kvect->storevalue((long)rowsize);
			kvect->storevalue((long)columnsize);
			return kvect;
		}

		//we use the values to set the new matrix size...
		int szr = callfunc->Evaluatethread(0, domain, idthread)->Integer();
		int szc = callfunc->Evaluatethread(1, domain, idthread)->Integer();
		rowsize = szr;
		columnsize = szc;
		values.update(szr, szc);
		return kifTRUE;
	}


	KifElement* MethodTransposed(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		Kifmatrixfloat* res = new Kifmatrixfloat(kifcode, NULL);

		for (unsigned short r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (unsigned short c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						res->populate(c, r, values[r][c]);
				}
			}
		}

		res->rowsize = columnsize;
		res->columnsize = rowsize;

		return res;
	}

	//-------------------------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------------------------


	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		KiFArityError(name)

			return (this->*linkedmethods[name])(contextualpattern, domain, callfunc, idthread);
	}



	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
	long Size() {
		return (rowsize*columnsize);
	}

	//How to interprete your class according to the basic following types:
	string String() {
		stringstream buff;

		for (int r = 0; r < rowsize; r++) {
			for (int c = 0; c < columnsize; c++) {
				if (c)
					buff << ",";
				if (values.check(r) && values[r].check(c))
					buff << values[r][c];
				else
					buff << "0";
			}
			buff << endl;
		}

		return buff.str();
	}

	long Integer() { return Size(); }
	double Float() { return Size(); }
	bool Boolean() { return true; }

	//You can overcome the following function behaviour
	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		return kifNULL;
	}
	//union of two sets
	KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifmatrixfloat_type || b->type != kifmatrixfloat_type) {
			Clear();
			return this;
		}

		Kifmatrixfloat* ma = (Kifmatrixfloat*)a;
		Kifmatrixfloat* mb = (Kifmatrixfloat*)b;
		Kifmatrixfloat* res = new Kifmatrixfloat(kifcode, NULL);

		long v;
		for (unsigned short r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (unsigned short c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = (long)ma->values[r][c] | (long)mb->values[r][c];
						res->populate(r, c, (double)v);
					}
				}
			}
		}

		return res;
	}

	//intersection of two sets
	KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifmatrixfloat_type || b->type != kifmatrixfloat_type) {
			Clear();
			return this;
		}

		Kifmatrixfloat* ma = (Kifmatrixfloat*)a;
		Kifmatrixfloat* mb = (Kifmatrixfloat*)b;
		Kifmatrixfloat* res = new Kifmatrixfloat(kifcode, NULL);

		long v;
		for (unsigned short r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (unsigned short c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = (long)ma->values[r][c] & (long)mb->values[r][c];
						res->populate(r, c, (double)v);
					}
				}
			}
		}
		return res;
	}

	KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifmatrixfloat_type || b->type != kifmatrixfloat_type) {
			Clear();
			return this;
		}

		Kifmatrixfloat* ma = (Kifmatrixfloat*)a;
		Kifmatrixfloat* mb = (Kifmatrixfloat*)b;
		Kifmatrixfloat* res = new Kifmatrixfloat(kifcode, NULL);

		long v;
		for (unsigned short r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (unsigned short c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = (long)ma->values[r][c] ^ (long)mb->values[r][c];
						res->populate(r, c, (double)v);
					}
				}
			}
		}

		return res;
	}

	//Beware that a might be the current element
	//plus: a+b
	KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

		if (a->type != kifmatrixfloat_type || b->type != kifmatrixfloat_type) {
			Clear();
			return this;
		}

		Kifmatrixfloat* ma = (Kifmatrixfloat*)a;
		Kifmatrixfloat* mb = (Kifmatrixfloat*)b;
		Kifmatrixfloat* res = new Kifmatrixfloat(kifcode, NULL);
		double v;
		if (b->type != kifmatrixfloat_type) {
			double vb = b->Float();
			for (unsigned short r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (unsigned short c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] + vb;
							res->populate(r, c, v);
						}
					}
				}
			}
			return res;
		}

		for (unsigned short r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (unsigned short c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = ma->values[r][c] + mb->values[r][c];
						res->populate(r, c, v);
					}
				}
			}
		}

		return res;
	}

	//minus: a-b
	KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifmatrixfloat_type || b->type != kifmatrixfloat_type) {
			Clear();
			return this;
		}

		Kifmatrixfloat* ma = (Kifmatrixfloat*)a;
		Kifmatrixfloat* mb = (Kifmatrixfloat*)b;
		Kifmatrixfloat* res = new Kifmatrixfloat(kifcode, NULL);

		double v;
		if (b->type != kifmatrixfloat_type) {
			double vb = b->Float();
			for (unsigned short r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (unsigned short c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] - vb;
							res->populate(r, c, v);
						}
					}
				}
			}
			return res;
		}

		
		for (unsigned short r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (unsigned short c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = ma->values[r][c] - mb->values[r][c];
						res->populate(r, c, v);
					}
				}
			}
		}
		return res;
	}

	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifmatrixfloat_type) {
			Clear();
			return this;
		}
		Kifmatrixfloat* ma = (Kifmatrixfloat*)a;
		Kifmatrixfloat* res = new Kifmatrixfloat(kifcode, NULL);

		double v;
		unsigned short r, c, k;

		if (b->type != kifmatrixfloat_type) {
			double vb = b->Float();
			for (r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] * vb;
							res->populate(r, c, v);
						}
					}
				}
			}
			return res;
		}

		Kifmatrixfloat* mb = (Kifmatrixfloat*)b;
		if (ma->columnsize != mb->rowsize)
			return kifcode->Returnerror("MAT(209): Matrix size mismatch");

		res->init(ma->rowsize, mb->columnsize);
		for (r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r)) {
				for (c = 0; c < mb->columnsize; c++) {
					v = 0;
					for (k = 0; k < ma->columnsize; k++) {
						if (ma->values[r].check(k) && mb->values.check(k) && mb->values[k].check(c))
							v += ma->values[r][k] * mb->values[k][c];
					}
					res->populate(r, c, v);
				}
			}
		}

		return res;
	}


	//Division: a/b
	KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifmatrixfloat_type || b->type != kifmatrixfloat_type) {
			Clear();
			return this;
		}

		Kifmatrixfloat* ma = (Kifmatrixfloat*)a;
		Kifmatrixfloat* res = new Kifmatrixfloat(kifcode, NULL);

		double v;
		unsigned short r;
		unsigned short c;

		if (b->type != kifmatrixfloat_type) {
			double vb = b->Float();
			if (vb==0)
				return kifcode->Returnerror("MAT(203): Cannot divide by 0");

			for (r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] / vb;
							res->populate(r, c, v);
						}
					}
				}
			}
			return res;
		}

		Kifmatrixfloat* mb = (Kifmatrixfloat*)b;

		Kifmatrixfloat inverted(NULL, NULL);

		if (!mb->inversion(inverted))
			return kifcode->Returnerror("MAT(202): Cannot divide with this matrix");

		if (ma->columnsize != inverted.rowsize)
			return kifcode->Returnerror("MAT(202): Cannot divide with this matrix");

		unsigned short k;
		for (r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r)) {
				for (c = 0; c < inverted.columnsize; c++) {
					v = 0;
					for (k = 0; k < ma->columnsize; k++) {
						if (ma->values[r].check(k) && inverted.values.check(k) && inverted.values[k].check(c))
							v += ma->values[r][k] * inverted.values[k][c];
					}
					res->populate(r, c, v);
				}
			}
		}

		res->rowsize = ma->rowsize;
		res->columnsize = inverted.columnsize;
		return res;

	}

	//Comparison functions that might also be modified
	//inferior: this<a
	KifElement* less(KifElement* a) {
		return kifFALSE;
	}
	//superior: this>a
	KifElement* more(KifElement* a) {
		return kifFALSE;
	}

	//equality: this==a
	KifElement* same(KifElement* a) {
		if (a->type != kifmatrixfloat_type)
			return kifFALSE;

		Kifmatrixfloat* ma = (Kifmatrixfloat*)a;
		if (ma->rowsize != rowsize && ma->columnsize != columnsize)
			return kifFALSE;

		for (unsigned short r = 0; r < rowsize; r++) {
			if (ma->values.check(r)) {
				if (!values.check(r))
					return kifFALSE;
				for (unsigned short c = 0; c < columnsize; c++) {
					if (ma->values[r].check(c)) {
						if (!values[r].check(c))
							return kifFALSE;

						if (ma->values[r][c] != values[r][c])
							return kifFALSE;
					}
					else {
						if (values[r].check(c))
							return kifFALSE;
					}
				}
			}
			else
			if (values.check(r))
				return kifFALSE;
		}
		return kifTRUE;
	}

	//different: this!=a
	KifElement* different(KifElement* a) {
		if (same(a) == kifFALSE)
			return kifTRUE;
		return kifFALSE;
	}

	//inferior or equal: this<=a
	KifElement* lessequal(KifElement* a) {
		return kifFALSE;
	}

	//superior or equal: this>=a
	KifElement* moreequal(KifElement* a) {
		return kifFALSE;
	}

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifmatrixfloat::kifexportedmethods;
map<short, kifmatrixfloatMethod>  Kifmatrixfloat::linkedmethods;
map<string, string> Kifmatrixfloat::infomethods;
hmap<short, bool> Kifmatrixfloat::methodassigned;


//---------------------------------------------------------------------------------------------------------------------

//The next methods are the ones that should be derived to add iterator capacities to Kifsparse
KifElement* KifIteratorkifmatrixfloat::IteratorKey() {
	char buff[100];
	sprintf_s(buff, 100, "%d:%d", r, c);
	return kifcode->Providestringraw(buff);
}

KifElement* KifIteratorkifmatrixfloat::IteratorValue() {
	double v = 0;
	if (value->values.check(r) && value->values[r].check(c))
		v = value->values[r][c];

	return kifcode->Providefloat(v);
}

long KifIteratorkifmatrixfloat::IteratorValueInteger() {
	double v = 0;
	if (value->values.check(r) && value->values[r].check(c))
		v = value->values[r][c];
	return (long)v;
}

double KifIteratorkifmatrixfloat::IteratorValueFloat() {
	double v = 0;
	if (value->values.check(r) && value->values[r].check(c))
		v = value->values[r][c];
	return v;
}

void KifIteratorkifmatrixfloat::Next() {
	r++;
	if (r == mr) {
		mc++;
		r = 0;
	}
}

KifElement* KifIteratorkifmatrixfloat::End() {
	if (r == mr && c == mc)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifIteratorkifmatrixfloat::Begin() {
	r = 0;
	c = 0;
	mr = value->rowsize;
	mc = value->columnsize;

	if (!mr && !mc)
		return kifFALSE;

	return kifTRUE;
}

//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifmatrixfloatElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifmatrixfloat* local = new Kifmatrixfloat(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitializationMatriceFloat(KifCode* kifcode, string name, kifmatrixfloatMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifmatrixfloat::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifmatrixfloat::linkedmethods[idname] = func;
	Kifmatrixfloat::methodassigned[idname] = true;
	Kifmatrixfloat::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exported bool Init_kifsparse(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}
		kifcode->KifInitLib();

		///The new type will be: kifmatrixfloat. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifmatrixfloatElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifmatrixfloat)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifmatrixfloatElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("matrix", CreatekifmatrixfloatElement);
		if (res == kifVOID)
			return false;
		kifmatrixfloat_type = res;
		res = KifAddNewType("imatrix", CreatekifmatrixintegerElement);
		if (res == kifVOID)
			return false;
		kifmatrixinteger_type = res;

		return true;
	}

	Exported bool kifsparse_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifsparse(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.

		MethodInitializationMatriceFloat(kifcode, "transpose", &Kifmatrixfloat::MethodTransposed, P_NONE, "transpose(): return the transposed matrix");
		MethodInitializationMatriceFloat(kifcode, "dimension", &Kifmatrixfloat::MethodMatrixSize, P_NONE | P_TWO, "dimension(): return the matrix size.\ndimension(int rowsize,int columnsize): set the matrix size.");
		MethodInitializationMatriceFloat(kifcode, "determinant", &Kifmatrixfloat::MethodDeterminant, P_NONE, "determinant(): return the matrix determinant");
		MethodInitializationMatriceFloat(kifcode, "sum", &Kifmatrixfloat::MethodSum, P_NONE, "sum(): return the sum of all elements");
		MethodInitializationMatriceFloat(kifcode, "product", &Kifmatrixfloat::MethodProduct, P_NONE, "product(): return the product of all elements");
		MethodInitializationMatriceFloat(kifcode, "invert", &Kifmatrixfloat::MethodInversion, P_NONE, "invert(): Return the inverted matrix.");


		kifGlobals->kifAllFunctions[kifmatrixfloat_type] = Kifmatrixfloat::methodassigned;
		kifGlobals->kifAllInformation[kifmatrixfloat_type] = Kifmatrixfloat::infomethods;

		MethodInitializationMatrixInteger(kifcode, "transpose", &Kifmatrixinteger::MethodTransposed, P_NONE, "transpose(): return the transposed matrix");
		MethodInitializationMatrixInteger(kifcode, "dimension", &Kifmatrixinteger::MethodMatrixSize, P_NONE | P_TWO, "dimension(): return the matrix size.\ndimension(int rowsize,int columnsize): set the matrix size.");
		MethodInitializationMatrixInteger(kifcode, "determinant", &Kifmatrixinteger::MethodDeterminant, P_NONE, "determinant(): return the matrix determinant");
		MethodInitializationMatrixInteger(kifcode, "sum", &Kifmatrixinteger::MethodSum, P_NONE, "sum(): return the sum of all elements");
		MethodInitializationMatrixInteger(kifcode, "product", &Kifmatrixinteger::MethodProduct, P_NONE, "product(): return the product of all elements");
		MethodInitializationMatrixInteger(kifcode, "invert", &Kifmatrixinteger::MethodInversion, P_NONE, "invert(): Return the inverted matrix.");


		kifGlobals->kifAllFunctions[kifmatrixinteger_type] = Kifmatrixinteger::methodassigned;
		kifGlobals->kifAllInformation[kifmatrixinteger_type] = Kifmatrixinteger::infomethods;

		return true;
	}

	Exported bool kifsparse_Terminate(KifCode* kifcode) {
		Kifmatrixfloat::kifexportedmethods.clear();
		Kifmatrixfloat::linkedmethods.clear();
		Kifmatrixfloat::methodassigned.clear();
		Kifmatrixfloat::infomethods.clear();

		Kifmatrixinteger::kifexportedmethods.clear();
		Kifmatrixinteger::linkedmethods.clear();
		Kifmatrixinteger::methodassigned.clear();
		Kifmatrixinteger::infomethods.clear();
		return true;
	}
}




