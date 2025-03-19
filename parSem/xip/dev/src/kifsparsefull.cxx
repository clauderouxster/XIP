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
filename   : kifsparsefull.cxx
Date       : 20/06/2016
Purpose    : This specific file contains the full implementation of the sparse matrix...
Programmer : Claude ROUX
Reviewer   :
*/

//Use KIF_DLL only in Windows, if you link to the KiF DLL
//#define KIF_DLL
#include "kif.h"
#include "kifversion.h"
#include "kifvecte.h"
#include "kifcontainer.h"
#include "kifmaps.h"
#include "fractalhmap.h"
#include "kiftreemaps.h"
#include "kifpmaps.h"

#define NMAX 100
#define TINY 1.5e-16
#define REAL double
#define ZERO (double)0.0
#define ONE (double)1.0
#define TWO (double)2.0

#ifdef WIN32
#include <math.h>
#endif

#ifdef UNIX
#include <math.h>
#include <float.h>
#define ABS(x) fabs((double)x)
#define TRUE true
#define FALSE false
#else
#define ABS(x) abs((double)x)
#endif
#define SQR(X) ((X) * (X))                     /* square of X         */
#define MAXIT 50                      /*  Maximal number of           */
#define BASIS     2

#define SWAP(typ, a, b) { typ temp; temp = a; a = b; b = temp; }

#ifdef FLT_EPSILON               /* ANSI C compiler ? ................*/
#define MACH_EPS  (REAL)FLT_EPSILON
/*.IX{MACH\unt EPS}*/
#else                            /* not an ANSI C compiler ? .........*/
#ifdef APPLE
#include <float.h>
#define MACH_EPS (REAL)FLT_EPSILON
#define TRUE true
#define FALSE false
#else
#ifdef WIN32
#include <limits>
#define MACH_EPS numeric_limits<double>::epsilon()
#else
#define MACH_EPS  mach_eps()     /* machine constant .................*/
#endif
#endif
#endif

#if defined(LDOUBLE) &&                     /* Borland C++ 3.0 or    */\
	(defined(BC3) || defined(MC6) ||        /* Microsoft C 6.0 or    */\
	defined(EMX09B))                       /* emx 0.9b (GCC272) with */
/* maximal precision?     */
#define FABS(x)    fabsl((x))               /* use the long double    */
/*.IX{FABS}*/
#define SQRT(x)    sqrtl((x))               /* versions of the basic  */
/*.IX{SQRT}*/
#define POW(x, y)  powl((x), (y))           /* floating point         */
/*.IX{POW}*/
#define SIN(x)     sinl((x))                /* functions              */
/*.IX{SIN}*/
#define COS(x)     cosl((x))
/*.IX{COS}*/
#define EXP(x)     expl((x))
/*.IX{EXP}*/
#define LOG(x)     logl((x))
/*.IX{LOG}*/
#define ATAN(x)    atanl((x))
/*.IX{ATAN}*/
#define ACOS(x)    acosl((x))
/*.IX{ACOS}*/
#define COSH(x)    coshl((x))
/*.IX{COSH}*/

#else                                       /* less precision or not a*/
/* BC3 and not a MC6 ?    */
#define FABS(x)    (REAL)fabs((double)(x))  /* declare names of basic */
#ifdef LONG_DOUBLE_USED                     /* floating point         */
#define SQRT(x)    sqrtlong((x))            /* functions that can be  */
/*.IX{SQRT}*/
#else                                       /* used in each of the    */
#define SQRT(x)    (REAL)sqrt((double)(x))  /* three precisions       */
#endif
#define POW(x, y)  (REAL)pow((double)(x), \
	/*.IX{POW}*/                              \
	(double)(y))
#define SIN(x)     (REAL)sin((double)(x))
/*.IX{SIN}*/
#define COS(x)     (REAL)cos((double)(x))
/*.IX{COS}*/
#define EXP(x)     (REAL)exp((double)(x))
/*.IX{EXP}*/
#define LOG(x)     (REAL)log((double)(x))
/*.IX{LOG}*/
#define ATAN(x)    (REAL)atan((double)(x))
/*.IX{ATAN}*/
#define ACOS(x)    (REAL)acos((double)(x))
/*.IX{ACOS}*/
#define COSH(x)    (REAL)cosh((double)(x))
/*.IX{COSH}*/
#endif

REAL comabs             /* Complex absolute value ....................*/
(
REAL  ar,          /* Real part .......................*/
REAL  ai           /* Imaginary part ..................*/
)
/*====================================================================*
*                                                                    *
*  Complex absolute value of   a                                     *
*                                                                    *
*====================================================================*
*                                                                    *
*   Input parameters:                                                *
*   ================                                                 *
*      ar,ai    REAL   ar, ai;                                       *
*               Real, imaginary parts of  a                          *
*                                                                    *
*   Return value :                                                   *
*   =============                                                    *
*      Absolute value of a                                           *
*                                                                    *
*   Macros used :    SQRT, ABS, SWAP                                 *
*   =============                                                    *
*                                                                    *
*====================================================================*/
{
	if (ar == ZERO && ai == ZERO) return (ZERO);

	ar = ABS(ar);
	ai = ABS(ai);

	if (ai > ar)                                  /* Switch  ai and ar */
		SWAP(REAL, ai, ar)

		return ((ai == ZERO) ? (ar) : (ar * SQRT(ONE + ai / ar * ai / ar)));
}


int comdiv              /* Complex division ..........................*/
(
REAL   ar,            /* Real part of numerator ..........*/
REAL   ai,            /* Imaginary part of numerator .....*/
REAL   br,            /* Real part of denominator ........*/
REAL   bi,            /* Imaginary part of denominator ...*/
REAL&  cr,            /* Real part of quotient ...........*/
REAL& ci             /* Imaginary part of quotient ......*/
)
/*====================================================================*
*                                                                    *
*  Complex division  c = a / b                                       *
*                                                                    *
*====================================================================*
*                                                                    *
*   Input parameters:                                                *
*   ================                                                 *
*      ar,ai    REAL   ar, ai;                                       *
*               Real, imaginary parts of numerator                   *
*      br,bi    REAL   br, bi;                                       *
*               Real, imaginary parts of denominator                 *
*                                                                    *
*   Output parameters:                                               *
*   ==================                                               *
*      cr,ci    REAL   *cr, *ci;                                     *
*               Real , imaginary parts of the quotient               *
*                                                                    *
*   Return value :                                                   *
*   =============                                                    *
*      = 0      ok                                                   *
*      = 1      division by 0                                        *
*                                                                    *
*   Macro used :     ABS                                             *
*   ============                                                     *
*                                                                    *
*====================================================================
*/
{
	REAL tmp;

	if (br == ZERO && bi == ZERO) return (1);

	if (ABS(br) > ABS(bi))
	{
		tmp = bi / br;
		br = tmp * bi + br;
		cr = (ar + tmp * ai) / br;
		ci = (ai - tmp * ar) / br;
	}
	else
	{
		tmp = br / bi;
		bi = tmp * br + bi;
		cr = (tmp * ar + ai) / bi;
		ci = (tmp * ai - ar) / bi;
	}

	return (0);
}

//Replace kifsparse by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType kifsparse_type;
static KifType kifsparseline_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifsparse;
class Kifsparseline;
//This typedef defines a type "kifsparseMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifsparse::*kifsparseMethod)(KifElement*, KifDomain*, KifCallFunction*, int);
typedef KifElement* (Kifsparseline::*kifsparselineMethod)(KifElement*, KifDomain*, KifCallFunction*, int);


template <class Z> class kif_hash_minmax : public prime_hash<long, Z> {
public:
	long maxidx;
	long minidx;
	char step;

	kif_hash_minmax<Z>() : prime_hash<long, Z>() {
		step = 0;
	}

	Z& GET(long idx) {
		if (!step) {
			maxidx = idx;
			minidx = idx;
		}
		else {
			if (maxidx<idx) maxidx = idx;
			if (minidx>idx) minidx = idx;
		}

		return prime_hash<long, Z>::Get(idx);
	}

	Z& operator [](long i) {
		return GET(i);
	}

};

class KifIteratorkifsparseline : public KifIteration {
public:
	Kifsparseline* value;
	prime_iterator<long, double > line;

	KifIteratorkifsparseline(KifCode* klc, KifElement* kp) : KifIteration(klc, kp) {
		value = NULL;
	}


	//The next methods are the ones that should be derived to add iterator capacities to Kifsparse
	long IteratorValueInteger();
	double IteratorValueFloat();
	string IteratorKeyString();
	KifElement* IteratorKey();
	KifElement* IteratorValue();
	void Next();
	KifElement* End();
	KifElement* Begin();
};

class KifIteratorkifsparse : public KifIteration {
public:
	Kifsparse* value;
	Kifsparseline* line;
	prime_iterator<long, prime_hash<long, double> > aline;

	KifIteratorkifsparse(KifCode* klc, KifElement* kp) : KifIteration(klc, kp) {
		value = NULL;
		line = NULL;
	}


	//The next methods are the ones that should be derived to add iterator capacities to Kifsparse
	KifElement* IteratorKey();
	KifElement* IteratorValue();
	void Next();
	KifElement* End();
	KifElement* Begin();
};

//-------------------------------------------------------------
#define KIFMATRIX kif_hash_minmax<prime_hash<long,double > >

double valdouble(KIFMATRIX& m, int i, int j) {
	if (m.find(i) && m[i].find(j))
		return m[i][j];
	return 0;
}

//-------------------------------------------------------------
class Kifsparseline : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, kifsparselineMethod> linkedmethods;
	static hmap<short, bool> methodassigned;	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	prime_hash<long, double >* mvector;
	Kifsparse* base;
	bool isrow;
	int idx;
	bool pastouche;
	unsigned int sizevector;
	//---------------------------------------------------------------------------------------------------------------------
	Kifsparseline(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, kifsparseline_type), base(NULL), mvector(NULL), isrow(false), idx(-1), pastouche(false), sizevector(0) {}


	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return kifsparseline_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifsparseline* kperso = new Kifsparseline(KifCurrent(), NULL);
		//Your initialisations here
		return kperso;
	}

	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//This description is facultative
	//iterator it=mydb;
	KifElement* Newiterator(bool rev) {
		KifIteratorkifsparseline* iter = new KifIteratorkifsparseline(kifcode, NULL);
		iter->reverse = rev;
		iter->value = this;
		//Your initialisation here for your iterator object
		return iter;
	}
	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	Exported bool initialconstructor() {
		return false;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	Exported KifElement* constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
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

	void Resetreference(int inc = 1) {
		if (pastouche)
			return;
		KifElement::Resetreference(inc);
	}

	unsigned int Idx(KifElement* k);
	void IncrementNB();
	void DecrementNB();

	void populate(unsigned int l, double value, bool filterzero = true);

	Exported bool Setvalue(KifElement* ke, KifElement* theidx, KifElement* dom) {
		ThreadLock _lock(type);
		if (ke == this)
			return true;
		if (theidx == NULL) {
			if (ke->type != kifsparseline_type)
				return true;
			mvector = ((Kifsparseline*)ke)->mvector;
			base = ((Kifsparseline*)ke)->base;
			isrow = ((Kifsparseline*)ke)->isrow;
			idx = ((Kifsparseline*)ke)->idx;
			return true;
		}

		KifElement* keyleft;
		if (theidx->type != kifIndex || ((KifIndex*)theidx)->interval == true) {
			kifcode->Returnerror("SPM(100): Wrong index");
			return true;
		}

		KifIndex* kind = (KifIndex*)theidx;
		keyleft = kind->key->Execute(kifNULL, (KifDomain*)dom);
		populate(Idx(keyleft), ke->Float());
		return true;
	}

	KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* idx) {
		ThreadLock _lock(type);

		if (idx == NULL)
			return this;

		if (idx->type != kifIndex || ((KifIndex*)idx)->interval == true)
			return kifcode->Returnerror("SPM(100): Wrong index");

		KifIndex* kind = (KifIndex*)idx;
		int l = Idx(kind->key->Execute(kifNULL, (KifDomain*)dom));

		if (mvector->find(l))
			return kifcode->Providefloat((*mvector)[l]);
		return kifZERO;
	}

	KifElement* MethodMin(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		double minval;
		bool first = true;
		prime_iterator<long, double> itl;
		//for each column for that row
		for (itl = *mvector; !itl.end(); itl++) {
			if (first) {
				minval = itl.value();
				first = false;
			}
			else
				minval = min(minval, itl.value());
		}

		return kifcode->Provideinteger(minval);
	}

	KifElement* MethodMax(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		double maxval;
		bool first = true;
		prime_iterator<long, double> itl;
		//for each column for that row
		for (itl = *mvector; !itl.end(); itl++) {
			if (first) {
				maxval = itl.value();
				first = false;
			}
			else
				maxval = max(maxval, itl.value());
		}

		return kifcode->Provideinteger(maxval);
	}

	KifElement* MethodIndex(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (mvector == NULL)
			return kifZERO;
		return kifcode->Provideinteger(idx);
	}

	KifElement* MethodSum(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (mvector == NULL)
			return kifNULL;
		double v = 0;
		prime_iterator<long, double> itl;
		//for each column for that row
		for (itl = *mvector; !itl.end(); itl++)
			v += itl.value();
		return kifcode->Providefloat(v);
	}

	KifElement* MethodProduct(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (mvector == NULL)
			return kifNULL;
		double v;
		bool first = true;
		prime_iterator<long, double> itl;
		//for each column for that row
		for (itl = *mvector; !itl.end(); itl++) {
			if (first)
				v = itl.value();
			else
				v *= itl.value();
		}
		return kifcode->Providefloat(v);
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		KiFArityError(name)
			return (this->*linkedmethods[name])(contextualpattern, domain, callfunc, idthread);
	}


	void Clear() {
		if (mvector == NULL)
			return;
		//Your reset code here
		prime_iterator<long, double> itl;

		//for each column for that row
		for (itl = *mvector; !itl.end(); itl++)
			DecrementNB();
		mvector->clear();
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		Clear();
	}


	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
	long Size() {
		if (mvector == NULL)
			return 0;
		long nbx = 0;
		prime_iterator<long, double> itl;
		//for each column for that row
		for (itl = *mvector; !itl.end(); itl++)
			nbx++;
		return nbx;
	}

	//How to interprete your class according to the basic following types:
	string String() {
		if (mvector == NULL)
			return "";
		bool first = true;
		prime_iterator<long, double> itl;
		//for each column for that row
		stringstream buff;
		for (itl = *mvector; !itl.end(); itl++) {
			if (first)
				first = false;
			else
				buff << ",";
			buff << itl.value();
		}
		return buff.str();
	}

	Exported KifElement* Vector(KifElement* dom, KifVector* v = NULL) {
		KifElement* kvect = v;
		if (kvect == NULL)
			kvect = Selectvectorfloat(kifNULL, dom);
		else
			kvect->Clear();

		if (mvector == NULL)
			return kvect;

		prime_iterator<long, double> it;
		for (it = *mvector; !it.end(); it++)
			kvect->storevalue(it.value());
		return kvect;
	}


	Exported KifElement* Map(KifElement* dom, KifMap* m = NULL) {
		KifElement* kmap = m;
		if (kmap == NULL)
			kmap = Selectmapfloat(kifNULL, dom);
		else
			kmap->Clear();

		if (mvector == NULL)
			return kmap;

		long c;
		prime_iterator<long, double> it;
		for (it = *mvector; !it.end(); it++) {
			c = it.key();
			kmap->storekey(c, it.value());
		}
		return kmap;
	}

	long Integer() { return idx; }
	double Float() { return 0; }
	bool Boolean() { return true; }

	void storevalue(KifElement* res, int r, int c, double v);


	//You can overcome the following function behaviour
	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);

	//union of two sets
	KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);

	//intersection of two sets
	KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);

	KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);

	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);

	//Beware that a might be the current element
	//plus: a+b
	KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);

	//minus: a-b
	KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);

	//Division: a/b
	KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);

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
		if (a->type != kifsparseline_type)
			return kifFALSE;

		Kifsparseline* ma = (Kifsparseline*)a;

		if (ma->mvector == NULL) {
			if (mvector == NULL)
				return kifTRUE;
			return kifFALSE;
		}

		if (ma->isrow != isrow)
			return kifFALSE;

		prime_iterator<long, double> itl;
		unsigned int l;
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			if (ma->mvector->find(l)) {
				if (itl.value() != ma->mvector->Get(l))
					return kifFALSE;
			}
			else
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
map<short, KifCallMethod> Kifsparseline::kifexportedmethods;
map<short, kifsparselineMethod>  Kifsparseline::linkedmethods;
map<string, string> Kifsparseline::infomethods;
hmap<short, bool> Kifsparseline::methodassigned;

//-------------------------------------------------------------

class Kifsparse : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, kifsparseMethod> linkedmethods;
	static hmap<short, bool> methodassigned;	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	KIFMATRIX matrixvalues;
	kif_hash_minmax<prime_hash<long, double > > matrixbyrow;
	kif_hash_minmax<prime_hash<long, double> > matrixbycolumn;
	hmap<string, unsigned int> sirow;
	hmap<string, unsigned int> sicol;
	hmap<unsigned int, string> isrow;
	hmap<unsigned int, string> iscol;
	double minval;
	double maxval;
	bool firstinsertion;
	int nb;
	unsigned int rowsize;
	unsigned int columnsize;

	//---------------------------------------------------------------------------------------------------------------------
	Kifsparse(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, kifsparse_type), nb(0),
		firstinsertion(true), minval(0), maxval(0), rowsize(0), columnsize(0){}

	void cleaning() {
		prime_iterator<long, prime_hash<long, double> > itr;
		itr = matrixbyrow;
		for (itr = matrixbyrow; !itr.end(); itr++)
			itr.value().clear();
		for (itr = matrixbycolumn; !itr.end(); itr++)
			itr.value().clear();
		for (itr = matrixvalues; !itr.end(); itr++)
			itr.value().clear();

		matrixbyrow.clear();
		matrixvalues.clear();
		matrixbycolumn.clear();
	}

	void Clear() {
		//Your reset code here
		firstinsertion = true;
		cleaning();
		isrow.clear();
		iscol.clear();
		sirow.clear();
		sicol.clear();
		rowsize = 0;
		columnsize = 0;
		nb = 0;
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
		cleaning();
		isrow.clear();
		iscol.clear();
		sirow.clear();
		sicol.clear();
		rowsize = 0;
		columnsize = 0;
		nb = 0;
	}

	~Kifsparse() {
		cleaning();
	}


	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return kifsparse_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifsparse* kperso = new Kifsparse(KifCurrent(), NULL);
		//Your initialisations here
		return kperso;
	}

	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//This description is facultative
	//iterator it=mydb;
	KifElement* Newiterator(bool rev) {
		KifIteratorkifsparse* iter = new KifIteratorkifsparse(kifcode, NULL);
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
	double Val(int i, int j) {
		if (!matrixbyrow.find(i) || !matrixbyrow[i].find(j))
			return 0;
		return matrixbyrow[i][j];
	}


	void populate(unsigned int r, unsigned int c, double value, bool filterzero = true);
	void populate(unsigned int r, unsigned int c, KifElement* value);

	unsigned int IdxRow(string s) {
		if (sirow.find(s) == sirow.end()) {
			int i = sirow.size();
			sirow[s] = i;
			isrow[i] = s;
			return i;
		}
		return sirow[s];
	}

	unsigned int IdxCol(string s) {
		if (sicol.find(s) == sicol.end()) {
			int i = sicol.size();
			sicol[s] = i;
			iscol[i] = s;
			return i;
		}
		return sicol[s];
	}

	unsigned int IdxRow(KifElement* k) {
		if (k->type == kifString) {
			string& s = ((KifString*)k)->value;
			if (sirow.find(s) == sirow.end()) {
				int i = sirow.size();
				sirow[s] = i;
				isrow[i] = s;
				return i;
			}
			return sirow[s];
		}
		else
			return k->Integer();
	}

	unsigned int IdxCol(KifElement* k) {
		if (k->type == kifString) {
			string& s = ((KifString*)k)->value;
			if (sicol.find(s) == sicol.end()) {
				int i = sicol.size();
				sicol[s] = i;
				iscol[i] = s;
				return i;
			}
			return sicol[s];
		}
		else
			return k->Integer();
	}

	void populate(KifElement* krow, KifElement* kcol, KifElement* value) {
		if (value->Float() == 0)
			return;

		if (firstinsertion) {
			maxval = value->Float();
			minval = maxval;
			firstinsertion = false;
		}
		else {
			maxval = max(value->Float(), maxval);
			minval = min(value->Float(), minval);
		}

		unsigned int r = IdxRow(krow);
		unsigned int c = IdxCol(kcol);

		if (!matrixbyrow.find(r) || !matrixbyrow[r].find(c))
			nb++;
		rowsize = max(rowsize, r);
		columnsize = max(columnsize, c);
		matrixbyrow[r][c] = value->Float();
		matrixbycolumn[c][r] = value->Float();
	}

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
				matrixbyrow.clear();
				KifElement::Resetreference();
			}
		}
	}


	void populate(Kifsparse* ke) {
		prime_iterator<long, prime_hash<long, double> > itr;
		prime_iterator<long, double> itl;

		unsigned int r, c;
		for (itr = ke->matrixbyrow; !itr.end(); itr++) {
			//for each column for that row
			r = itr.key();
			for (itl = itr.value(); !itl.end(); itl++) {
				c = itl.key();
				populate(r, c, itl.value());
			}
		}

		rowsize = ke->columnsize;
		columnsize = ke->rowsize;
	}

	void populate(bool clear = true) {
		prime_iterator<long, prime_hash<long, double> > itr;
		prime_iterator<long, double> itl;

		unsigned int r;
		for (itr = matrixvalues; !itr.end(); itr++) {
			//for each column for that row
			r = itr.key();
			for (itl = itr.value(); !itl.end(); itl++)
				populate(r, itl.key(), itl.value());
		}
		if (clear)
			matrixvalues.clear();
	}


	Kifsparse* duplicate() {
		Kifsparse* res = new Kifsparse(kifcode, NULL);

		prime_iterator<long, prime_hash<long, double> > itr;
		prime_iterator<long, double> itl;

		unsigned int r;
		for (itr = matrixbyrow; !itr.end(); itr++) {
			//for each column for that row
			r = itr.key();
			for (itl = itr.value(); !itl.end(); itl++)
				res->populate(r, itl.key(), itl.value());
		}
		return res;
	}

	void initializematrixvalues() {
		matrixvalues.clear();
		prime_iterator<long, prime_hash<long, double> > itr;
		prime_iterator<long, double> itl;

		unsigned int r;
		for (itr = matrixbyrow; !itr.end(); itr++) {
			//for each column for that row
			r = itr.key();
			for (itl = itr.value(); !itl.end(); itl++)
				matrixvalues[r][itl.key()] = itl.value();
		}
	}

	//we only create a local matrix with values...
	Kifsparse* duplicatevalues() {
		Kifsparse* res = new Kifsparse(NULL, NULL);
		prime_iterator<long, prime_hash<long, double> > itr;
		prime_iterator<long, double> itl;

		unsigned int r;
		for (itr = matrixbyrow; !itr.end(); itr++) {
			//for each column for that row
			r = itr.key();
			for (itl = itr.value(); !itl.end(); itl++)
				res->matrixvalues[r][itl.key()] = itl.value();
		}
		return res;
	}


	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
		ThreadLock _lock(type);
		if (ke == this)
			return true;
		if (idx == NULL) {
			Clear();
			if (ke->type != kifsparse_type)
				return true;
			populate((Kifsparse*)ke);
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
			keyleft = kind->key->Execute(kifNULL, (KifDomain*)dom);
			keyright = kind->right->Execute(kifNULL, (KifDomain*)dom);
			populate(keyleft, keyright, ke);
			return true;
		}
		unsigned int r, c;
		KifIteration* itr;
		//it should be a map as value or a sparseline
		if (ke->aVectorContainer()) {
			unsigned int itx = 0;
			//then it means that only the column key is present
			if (kind->right != NULL && kind->right != kifNULL) {
				c = kind->right->Execute(kifNULL, (KifDomain*)dom)->Integer();
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
				r = kind->key->Execute(kifNULL, (KifDomain*)dom)->Integer();
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
				c = kind->right->Execute(kifNULL, (KifDomain*)dom)->Integer();
				itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
					r = IdxRow(itr->IteratorKeyString());
					populate(r, c, itr->IteratorValueFloat());
				}
				itr->Release();
				return true;
			}

			//We have the line, not the column
			if (kind->key != NULL && kind->key != kifNULL) {
				r = kind->key->Execute(kifNULL, (KifDomain*)dom)->Integer();
				itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
					c = IdxRow(itr->IteratorKeyString());
					populate(r, c, itr->IteratorValueFloat());
				}
				itr->Release();
				return true;
			}
		}
		else {
			if (ke->type = kifsparseline_type) {
				Kifsparseline* kline = (Kifsparseline*)ke;
				prime_iterator<long, double> itl;
				if (kline->mvector == NULL)
					return true;

				//then it means that only the column key is present
				if (kind->right != NULL && kind->right != kifNULL) {
					c = kind->right->Execute(kifNULL, (KifDomain*)dom)->Integer();
					for (itl = *kline->mvector; !itl.end(); itl++) {
						r = itl.key();
						populate(r, c, itl.value());
					}
					return true;
				}

				//We have the line, not the column
				if (kind->key != NULL && kind->key != kifNULL) {
					r = kind->key->Execute(kifNULL, (KifDomain*)dom)->Integer();
					for (itl = *kline->mvector; !itl.end(); itl++) {
						c = itl.key();
						populate(r, c, itl.value());
					}
					return true;
				}
			}
		}
		kifcode->Returnerror("MAT(131): Wrong index or Wrong value (recipient should be a map or a mvector)");
		return true;
	}

	KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* idx) {
		ThreadLock _lock(type);

		if (idx == NULL)
			return this;

		if (idx->type != kifIndex)
			return kifcode->Returnerror("SPM(100): Wrong index");

		//Two possibilities:
		//both indexes
		bool interval = ((KifIndex*)idx)->interval;
		KifIndex* kind = (KifIndex*)idx;
		int r, c;
		if (interval == true && kind->key != kifNULL && kind->right != kifNULL) {
			r = IdxRow(kind->key->Execute(kifNULL, (KifDomain*)dom));
			c = IdxCol(kind->right->Execute(kifNULL, (KifDomain*)dom));
			if (matrixbyrow.find(r) && matrixbyrow[r].find(c))
				return kifcode->Providefloat(matrixbyrow[r][c]);
			return kifZERO;
		}

		Kifsparseline* kline;
		if (contextualpattern->type == kifsparseline_type)
			kline = (Kifsparseline*)contextualpattern;
		else
			kline = new Kifsparseline(kifcode, NULL);

		kline->base = this;
		//then it means that only the column key is present
		if (kind->right != NULL && kind->right != kifNULL) {
			c = IdxCol(kind->right->Execute(kifNULL, (KifDomain*)dom));
			if (matrixbycolumn.find(c)) {
				kline->mvector = &matrixbycolumn[c];
				kline->isrow = false;
				kline->idx = c;
			}
			return kline;
		}
		if (kind->key != NULL && kind->key != kifNULL) {
			//We have the line, not the column
			r = kind->key->Execute(kifNULL, (KifDomain*)dom)->Integer();
			if (matrixbyrow.find(r)) {
				kline->mvector = &matrixbyrow[r];
				kline->isrow = true;
				kline->idx = r;
			}
			return kline;
		}
		return this;
	}

	KifElement* MethodDuplicate(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		return duplicate();
	}

	KifElement* MethodOccupation(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		KifVector* kvect = new KifVector(kifcode, NULL);
		KifVectorInteger* kint;
		int t, n, c, m;
		prime_iterator<long, prime_hash<long, double> > itr;

		if (callfunc->Evaluatethread(0, domain, idthread)->Boolean()) {
			matrixbyrow.occupation(t, c, n, m);
			kint = new KifVectorInteger(kifcode, NULL);
			kint->values.push_back(t);
			kint->values.push_back(c);
			kint->values.push_back(n);
			kint->values.push_back(m);
			kint->Setreference();
			kvect->values.push_back(kint);

			for (itr = matrixbyrow; !itr.end(); itr++) {
				itr.value().occupation(t, c, n, m);
				kint = new KifVectorInteger(kifcode, NULL);
				kint->values.push_back(t);
				kint->values.push_back(c);
				kint->values.push_back(n);
				kint->values.push_back(m);
				kint->Setreference();
				kvect->values.push_back(kint);
			}
		}
		else {
			matrixbycolumn.occupation(t, c, n, m);
			kint = new KifVectorInteger(kifcode, NULL);
			kint->values.push_back(t);
			kint->values.push_back(c);
			kint->values.push_back(n);
			kint->values.push_back(m);
			kint->Setreference();
			kvect->values.push_back(kint);

			for (itr = matrixbycolumn; !itr.end(); itr++) {
				itr.value().occupation(t, c, n, m);
				kint = new KifVectorInteger(kifcode, NULL);
				kint->values.push_back(t);
				kint->values.push_back(c);
				kint->values.push_back(n);
				kint->values.push_back(m);
				kint->Setreference();
				kvect->values.push_back(kint);
			}
		}

		return kvect;
	}

	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodBoundaries(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
		kvect->storevalue(matrixbyrow.minidx);
		kvect->storevalue(matrixbyrow.maxidx);
		kvect->storevalue(matrixbycolumn.minidx);
		kvect->storevalue(matrixbycolumn.maxidx);
		return kvect;
	}

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
		return kifTRUE;
	}


	KifElement* MethodMin(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		return kifcode->Provideinteger(minval);
	}

	KifElement* MethodMax(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		return kifcode->Provideinteger(maxval);
	}

	KifElement* MethodRow(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		int r = callfunc->Evaluatethread(0, domain, idthread)->Integer();
		if (contextualpattern->aVectorContainer()) {
			KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
			if (matrixbyrow.find(r)) {
				prime_iterator<long, double> it;
				for (it = matrixbyrow[r]; !it.end(); it++)
					kvect->storevalue(it.value());
			}
			return kvect;
		}

		if (contextualpattern->type >= kifMap && contextualpattern->type <= kifPrimeMapFloatFloat) {
			KifElement* kmap = Selectmapfloat(callfunc, contextualpattern);
			long c;
			if (matrixbyrow.find(r)) {
				prime_iterator<long, double> it;
				for (it = matrixbyrow[r]; !it.end(); it++) {
					c = it.key();
					kmap->storekey(c, it.value());
				}
			}
			return kmap;
		}

		if (contextualpattern->type == kifsparseline_type) {
			Kifsparseline* kline = (Kifsparseline*)contextualpattern;
			kline->base = this;
			if (matrixbyrow.find(r)) {
				kline->mvector = &matrixbyrow[r];
				kline->isrow = true;
				kline->idx = r;
			}
			return kline;
		}
		return kifcode->Returnerror("MAT(123): Wrong recipient variable type");
	}

	KifElement* MethodColumn(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		int c = callfunc->Evaluatethread(0, domain, idthread)->Integer();

		if (contextualpattern->aVectorContainer()) {
			KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
			if (matrixbycolumn.find(c)) {
				prime_iterator<long, double> it;
				for (it = matrixbycolumn[c]; !it.end(); it++)
					kvect->storevalue(it.value());
			}
			return kvect;
		}

		if (contextualpattern->type >= kifMap && contextualpattern->type <= kifPrimeMapFloatFloat) {
			KifElement* kmap = Selectmapfloat(callfunc, contextualpattern);
			long r;
			if (matrixbycolumn.find(c)) {
				prime_iterator<long, double> it;
				for (it = matrixbycolumn[c]; !it.end(); it++) {
					r = it.key();
					kmap->storekey(r, it.value());
				}
			}
			return kmap;
		}


		if (contextualpattern->type == kifsparseline_type) {
			Kifsparseline* kline = (Kifsparseline*)contextualpattern;
			kline->base = this;
			if (matrixbycolumn.find(c)) {
				kline->mvector = &matrixbycolumn[c];
				kline->isrow = false;
				kline->idx = c;
			}
			return kline;
		}
		return kifcode->Returnerror("MAT(123): Wrong recipient variable type");
	}

	KifElement* MethodRowKey(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (callfunc->Size() == 1) {
			int r = callfunc->Evaluatethread(0, domain, idthread)->Integer();
			if (isrow.find(r) != isrow.end())
				return kifcode->Providestring(isrow[r]);
			return kifNULL;
		}
		//We return a vector of indexes
		KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
		prime_iterator<long, prime_hash<long, double> > itr;
		for (itr = matrixbyrow; !itr.end(); itr++) {
			//for each column for that row
			kvect->storevalue(itr.key());
		}
		return kvect;
	}

	KifElement* MethodColKey(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (callfunc->Size() == 1) {
			int c = callfunc->Evaluatethread(0, domain, idthread)->Integer();
			if (iscol.find(c) != iscol.end())
				return kifcode->Providestring(iscol[c]);
			return kifNULL;
		}
		//We return a vector of indexes
		KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
		prime_iterator<long, prime_hash<long, double> > itr;
		for (itr = matrixbycolumn; !itr.end(); itr++) {
			kvect->storevalue(itr.key());
			//for each column for that row
		}
		return kvect;
	}

	KifElement* MethodTransposed(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		Kifsparse* res = new Kifsparse(kifcode, NULL);

		prime_iterator<long, prime_hash<long, double> > itr;
		prime_iterator<long, double> itl;
		unsigned int r, c;
		for (itr = matrixbyrow; !itr.end(); itr++) {
			//for each column for that row
			r = itr.key();
			for (itl = itr.value(); !itl.end(); itl++) {
				c = itl.key();
				res->populate(c, r, itl.value());
			}
		}

		res->rowsize = columnsize;
		res->columnsize = rowsize;

		return res;
	}

	double val(int i, int j) {
		if (!matrixvalues.find(i) || !matrixvalues[i].find(j))
			return 0;
		return matrixvalues[i][j];
	}
	//-------------------------------------------------------------------------------------------------------

	/**************************************************************
	* Given an N x N matrix A, this routine replaces it by the LU *
	* decomposition of a rowwise permutation of itself. A and N   *
	* are input. INDX is an output vector which records the row   *
	* permutation effected by the partial pivoting; D is output   *
	* as -1 or 1, depending on whether the number of row inter-   *
	* changes was even or odd, respectively. This routine is used *
	* in combination with LUBKSB to solve linear equations or to  *
	* invert a matrix. Return code is 1, if matrix is singular.   *
	*                            C++ version by J-P Moreau, Paris *
	**************************************************************
	*/

	int LUDCMP(int n, VECTE<int>& INDX, int& d) {

		d = 1;
		double AMAX, DUM, SUM;
		int  I, IMAX, J, K;
		vector<double> VV;

		for (I = 0; I < n; I++)  {
			AMAX = 0.0;
			for (J = 0; J<n; J++)  {
				SUM = val(I, J);
				if (ABS(SUM) > AMAX)
					AMAX = ABS(SUM);
			}

			if (AMAX < TINY)
				return 1;
			VV.push_back(1.0 / AMAX);
		} // i loop 

		for (J = 0; J < n; J++)  {

			for (I = 0; I < J; I++)  {
				SUM = val(I, J);
				for (K = 0; K < I; K++)
					SUM = SUM - val(I, K)*val(K, J);
				matrixvalues[I][J] = SUM;
			} // i loop 
			AMAX = 0.0;

			for (I = J; I < n; I++)  {
				SUM = val(I, J);
				for (K = 0; K < J; K++)
					SUM = SUM - val(I, K)*val(K, J);
				matrixvalues[I][J] = SUM;
				DUM = VV[I] * ABS(SUM);
				if (DUM >= AMAX) {
					IMAX = I;
					AMAX = DUM;
				}
			} // i loop

			if (J != IMAX)  {
				for (K = 0; K < n; K++)  {
					DUM = val(IMAX, K);
					matrixvalues[IMAX][K] = val(J, K);
					matrixvalues[J][K] = DUM;
				} // k loop 
				d = -d;
				VV[IMAX] = VV[J];
			}

			INDX[J] = IMAX;

			if (ABS(val(J, J)) < TINY)
				matrixvalues[J][J] = TINY;

			if (J != n - 1)  {
				DUM = 1.0 / val(J, J);
				for (I = J + 1; I < n; I++)
					matrixvalues[I][J] *= DUM;
			}
		} // j loop 

		return 0;

	} // subroutine LUDCMP 


	/*****************************************************************
	* Solves the set of N linear equations A . X = B.  Here A is     *
	* input, not as the matrix A but rather as its LU decomposition, *
	* determined by the routine LUDCMP. INDX is input as the permuta-*
	* tion vector returned by LUDCMP. B is input as the right-hand   *
	* side vector B, and returns with the solution vector X. A, N and*
	* INDX are not modified by this routine and can be used for suc- *
	* cessive calls with different right-hand sides. This routine is *
	* also efficient for plain matrix inversion.                     *
	****************************************************************
	*/
	void LUBKSB(int n, VECTE<int>& INDX, VECTE<double>& B)  {
		double SUM;
		int  I, II, J, LL;

		II = -1;

		for (I = 0; I < n; I++)  {
			LL = INDX[I];
			SUM = B[LL];
			B[LL] = B[I];
			if (II != -1)
			for (J = II; J < I; J++)
				SUM = SUM - val(I, J)*B[J];
			else
			if (SUM != 0.0)
				II = I;
			B[I] = SUM;
		} // i loop

		for (I = n - 1; I >= 0; I--)  {
			SUM = B[I];
			if (I < n - 1)  {
				for (J = I + 1; J < n; J++)
					SUM = SUM - val(I, J)*B[J];
			}
			B[I] = SUM / val(I, I);
		} // i loop 

	} // LUBKSB    
	//---------------------------------------------------------------------------------------------------------

	int IAMAX(VECTE<double>& A, int N) {
		double T; int I;
		T = 0.0;
		for (I = 1; I <= N; I++)
		if (fabs(A[I]) > T) {
			T = fabs(A[I]);
			return I;
		}
		return 0;
	}

	void SCALE(int N, double T, VECTE<double>& A) {
		int I;
		for (I = 1; I <= N; I++) A[I] = T*A[I];
	}

	void DAXPY(int N, double A, VECTE<double>& X, VECTE<double>& Y) {
		int I;
		for (I = 1; I <= N; I++)  Y[I] = Y[I] + A*X[I];
	}


	void NSBFAC(int N, int ML, int MU, VECTE<int>& IPVT, int& IND) {
		/*------------------------------------------------------------------
		!     LU factorization of a band matrix (non symmetric) with partial
		!     pivoting.
		!     INPUTS:
		!     B  : banded matrix. The correspondance between full matrix
		!     A(i,j) and band matrix B(k,l) is given by following sequence:
		!     m=ml+mu+1;
		!     for (j=1; j<=n; j++) {
		!       i1=Max(1,j-mu);
		!       i2=Min(n,j+ml);
		!       for (i=i1; i<=i2; i++) {
		!         k=i-j+m;
		!         B[k][j] = A[i][j];
		!       }
		!     }
		!     N   : size of B
		!     ML  : number of lower diagonals
		!     MU  : number of upper diagonals
		!     OUTPUTS:
		!     B   : banded matrix storing the LU elements, the ML first lines
		!           of which are used for pivoting.
		!     IPVT: integer vector of size N storing the pivoting indices.
		!     IND : flag = 0,  B is non singular
		!                = k,  B may be singular
		!------------------------------------------------------------------
		*/
		//  Labels:  e10, e20;
		double T;
		int I0, I, J, J0, J1, JU, JZ, K, KP1, L, LM, M, MM, NM1;
		VECTE<double> TMP;
		VECTE<double> TMP1;
		M = ML + MU + 1;
		IND = 0;

		J0 = MU + 2;
		J1 = min(N, M) - 1;
		if (J1 >= J0)
		for (JZ = J0; JZ <= J1; JZ++) {
			I0 = M + 1 - JZ;
			for (I = I0; I <= ML; I++)  matrixvalues[I - 1][JZ - 1] = 0.0;
		}
		JZ = J1;
		JU = 0;

		NM1 = N - 1;
		if (NM1 >= 0)
		for (K = 1; K <= NM1; K++) {
			KP1 = K + 1;
			JZ = JZ + 1;
			if (JZ <= N && ML >= 1)
			for (I = 1; I <= ML; I++)  matrixvalues[I - 1][JZ - 1] = 0.0;

			LM = min(ML, N - K);
			for (I = 1; I <= LM + 1; I++) TMP[I] = matrixvalues[I + M - 2][K - 1];
			L = IAMAX(TMP, LM + 1) + M - 1;
			IPVT[K] = L + K - M;
			if (val(L - 1, K - 1) == 0.0) goto e10;
			if (L != M) {
				T = val(L - 1, K - 1);
				matrixvalues[L - 1][K - 1] = val(M - 1, K - 1);
				matrixvalues[M - 1][K - 1] = T;
			}
			T = -1.0 / matrixvalues[M - 1][K - 1];

			for (I = 1; I <= LM; I++) TMP[I] = val(I + M - 1, K - 1);
			SCALE(LM, T, TMP);
			for (I = 1; I <= LM; I++) matrixvalues[I + M - 1][K - 1] = TMP[I];

			JU = min(max(JU, MU + IPVT[K]), N);
			MM = M;
			if (JU >= KP1)
			for (J = KP1; J <= JU; J++) {
				L--; MM--;
				T = matrixvalues[L - 1][J - 1];
				if (L != MM) {
					matrixvalues[L - 1][J - 1] = val(MM - 1, J - 1);
					matrixvalues[MM - 1][J - 1] = T;
				}
				for (I = 1; I <= LM; I++) {
					TMP[I] = val(I + M - 1, K - 1);
					TMP1[I] = val(I + MM - 1, J - 1);
				}
				DAXPY(LM, T, TMP, TMP1);
				for (I = 1; I <= LM; I++)
					matrixvalues[I + MM - 1][J - 1] = TMP1[I];
			}
			goto e20;
		e10:      IND = K;
		e20:;
		}
		IPVT[N] = N;
		if (val(M - 1, N - 1) == 0.0)  IND = N;
	}


	void NSBSLV(int N, int ML, int MU, VECTE<int>& IPVT, VECTE<double>& B, VECTE<double>& X) {
		/*-------------------------------------------------------------------
		!     Solve banded linear system Ax := b
		!     INPUTS:
		!     A   : banded matrix as output of LU factorization by NSBFAC
		!           (see storing mode in NSBFAC subroutine).
		!     LDA : 1st dimension of A in calling program (lda >= 2ml+mu+1)
		!     N   : order of A                             ---------------
		!     ML  : number of lower diagonals
		!     MU  : number of upper diagonals
		!     IPVT: integer vector of size N storing the pivoting indices
		!           as output of NSBFAC.
		!     B   : second member vector
		!     OUTPUT:
		!     X   : solution vector
		!-------------------------------------------------------------------
		*/
		double T;
		int I, K, L, LA, LB, LM, M, NM1;
		VECTE<double> TMP;
		VECTE<double> TMP1;

		for (I = 1; I <= N; I++)  X[I] = B[I];
		M = ML + MU + 1;
		NM1 = N - 1;
		//    solve L*y = b
		if (ML != 0 && NM1 >= 1)
		for (K = 1; K <= NM1; K++) {
			LM = min(ML, N - K);
			L = IPVT[K];
			T = X[L];
			if (L != K) {
				X[L] = X[K];
				X[K] = T;
			}
			for (I = 1; I <= LM; I++) {
				TMP[I] = val(I + M - 1, K - 1);
				TMP1[I] = X[I + K];
			}
			DAXPY(LM, T, TMP, TMP1);
			for (I = 1; I <= LM; I++) X[I + K] = TMP1[I];
		}
		//    solve U*y = x
		for (K = N; K > 0; K--) {
			X[K] = X[K] / val(M - 1, K - 1);
			LM = min(K, M) - 1;
			LA = M - LM;
			LB = K - LM;
			T = -X[K];
			for (I = 1; I <= LM; I++) {
				TMP[I] = val(I + LA - 2, K - 1);
				TMP1[I] = X[I + LB - 1];
			}
			DAXPY(LM, T, TMP, TMP1);
			for (I = 1; I <= LM; I++) X[I + LB - 1] = TMP1[I];
		}
	}


	//---------------------------------------------------------------------------------------------------------
	int balance       /* balance a matrix .........................*/
		(int       n,      /* size of matrix ..............*/
		VECTE<double>&      scal, /* Scaling data ................*/
		int&     low,    /* first relevant row index ....*/
		int&     high,   /* last relevant row index .....*/
		int       basis  /* base of computer numbers ....*/
		) {
		/*====================================================================*
		*                                                                    *
		*  balance balances the matrix mat so that the rows with zero entries*
		*  off the diagonal are isolated and the remaining columns and rows  *
		*  are resized to have one norm close to 1.                          *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Input parameters:                                                *
		*   ================                                                 *
		*      n        int n;  ( n > 0 )                                    *
		*               Dimension of mat                                     *
		*      mat      double   *mat[n];                                      *
		*               n x n input matrix                                   *
		*      basis    int basis;                                           *
		*               Base of number representaion in the given computer   *
		*               (see BASIS)                                          *
		*                                                                    *
		*   Output parameters:                                               *
		*   ==================                                               *
		*      mat      double   *mat[n];                                      *
		*               scaled matrix                                        *
		*      low      int *low;                                            *
		*      high     int *high;                                           *
		*               the rows 0 to low-1 and those from high to n-1       *
		*               contain isolated eigenvalues (only nonzero entry on  *
		*               the diagonal)                                        *
		*      scal     double   scal[];                                       *
		*               the vector scal contains the isolated eigenvalues in *
		*               the positions 0 to low-1 and high to n-1, its other  *
		*               components contain the scaling factors for           *
		*               transforming mat.                                    *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Macros:     SWAP, ABS                                            *
		*   =======                                                          *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Constants used:     TRUE, FALSE                                  *
		*   ==============                                                   *
		*                                                                    *
		*====================================================================
		*/

		register int i, j;
		int      iter, k, m;
		double     b2, r, c, f, g, s;

		b2 = (double)(basis * basis);
		m = 0;
		k = n - 1;

		do
		{
			iter = FALSE;
			for (j = k; j >= 0; j--)
			{
				for (r = 0.0, i = 0; i <= k; i++)
				if (i != j)  r += ABS(val(j, i));

				if (r == 0.0)
				{
					scal[k] = (double)j;
					if (j != k)
					{
						for (i = 0; i <= k; i++) SWAP(double, matrixvalues[i][j], matrixvalues[i][k])
						for (i = m; i < n; i++)  SWAP(double, matrixvalues[j][i], matrixvalues[k][i])
					}
					k--;
					iter = TRUE;
				}
			}   /* end of j */
		}   /* end of do  */
		while (iter);

		do
		{
			iter = FALSE;
			for (j = m; j <= k; j++)
			{
				for (c = 0.0, i = m; i <= k; i++)
				if (i != j) c += ABS(val(i, j));
				if (c == 0.0)
				{
					scal[m] = (double)j;
					if (j != m)
					{
						for (i = 0; i <= k; i++) SWAP(double, matrixvalues[i][j], matrixvalues[i][m])
						for (i = m; i < n; i++)  SWAP(double, matrixvalues[j][i], matrixvalues[m][i])
					}
					m++;
					iter = TRUE;
				}
			}   /* end of j */
		}   /* end of do  */
		while (iter);

		low = m;
		high = k;
		for (i = m; i <= k; i++) scal[i] = 1.0;

		do
		{
			iter = FALSE;
			for (i = m; i <= k; i++)
			{
				for (c = r = 0.0, j = m; j <= k; j++)
				if (j != i)
				{
					c += ABS(val(j, i));
					r += ABS(val(i, j));
				}
				g = r / basis;
				f = 1.0;
				s = c + r;

				while (c < g)
				{
					f *= basis;
					c *= b2;
				}

				g = r * basis;
				while (c >= g)
				{
					f /= basis;
					c /= b2;
				}

				if ((c + r) / f < (double)0.95 * s)
				{
					g = 1.0 / f;
					scal[i] = scal[i] * f;
					iter = TRUE;
					for (j = m; j < n; j++) matrixvalues[i][j] *= g;
					for (j = 0; j <= k; j++) matrixvalues[j][i] *= f;
				}
			}
		} while (iter);

		return (0);
	}

	int balback       /* reverse balancing ........................*/
		(int     n,        /* Dimension of matrix .........*/
		int     low,      /* first nonzero row ...........*/
		int     high,     /* last nonzero row ............*/
		VECTE<double>& scal,   /* Scaling data ................*/
		KIFMATRIX& eivec   /* Eigenvectors ................*/
		) {
		/*====================================================================*
		*                                                                    *
		*  balback reverses the balancing of balance for the eigenvactors.   *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Input parameters:                                                *
		*   ================                                                 *
		*      n        int n;  ( n > 0 )                                    *
		*               Dimension of mat                                     *
		*      low      int low;                                             *
		*      high     int high;   see balance                              *
		*      eivec    REAL   *eivec[n];                                    *
		*               Matrix of eigenvectors, as computed in  qr2          *
		*      scal     REAL   scal[];                                       *
		*               Scaling data from  balance                           *
		*                                                                    *
		*   Output parameter:                                                *
		*   ================                                                 *
		*      eivec    REAL   *eivec[n];                                    *
		*               Non-normalized eigenvectors of the original matrix   *
		*                                                                    *
		*   Macros :    SWAP()                                               *
		*   ========                                                         *
		*                                                                    *
		*====================================================================
		*/

		register int i, j, k;
		double s;

		for (i = low; i <= high; i++)  {
			s = scal[i];
			for (j = 0; j < n; j++) eivec[i][j] *= s;
		}

		for (i = low - 1; i >= 0; i--)  {
			k = (int)scal[i];
			if (k != i)
			for (j = 0; j < n; j++) SWAP(double, eivec[i][j], eivec[k][j])
		}

		for (i = high + 1; i < n; i++)  {
			k = (int)scal[i];
			if (k != i)
			for (j = 0; j < n; j++) SWAP(double, eivec[i][j], eivec[k][j])
		}
		return (0);
	}

	int elmhes       /* reduce matrix to upper Hessenberg form ....*/
		(int       n,       /* Dimension of matrix .........*/
		int       low,     /* first nonzero row ...........*/
		int       high,    /* last nonzero row ............*/
		VECTE<int>&       perm   /* Permutation vector ..........*/
		)
		/*====================================================================*
		*                                                                    *
		*  elmhes transforms the matrix mat to upper Hessenberg form.        *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Input parameters:                                                *
		*   ================                                                 *
		*      n        int n;  ( n > 0 )                                    *
		*               Dimension of mat                                     *
		*      low      int low;                                             *
		*      high     int high; see  balance                               *
		*      mat      REAL   *mat[n];                                      *
		*               n x n matrix                                         *
		*                                                                    *
		*   Output parameter:                                                *
		*   =================                                                *
		*      mat      REAL   *mat[n];                                      *
		*               upper Hessenberg matrix; additional information on   *
		*               the transformation is stored in the lower triangle   *
		*      perm     int perm[];                                          *
		*               Permutation vector for elmtrans                      *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Macros:   SWAP, ABS                                              *
		*   =======                                                          *
		*                                                                    *
		*====================================================================
		*/
	{
		register int i, j, m;
		REAL   x, y;

		for (m = low + 1; m < high; m++)
		{
			i = m;
			x = ZERO;
			for (j = m; j <= high; j++)
			if (ABS(val(j, m - 1)) > ABS(x))
			{
				x = val(j, m - 1);
				i = j;
			}

			perm[m] = i;
			if (i != m)
			{
				for (j = m - 1; j < n; j++) SWAP(REAL, matrixvalues[i][j], matrixvalues[m][j])
				for (j = 0; j <= high; j++) SWAP(REAL, matrixvalues[j][i], matrixvalues[j][m])
			}

			if (x != ZERO)
			{
				for (i = m + 1; i <= high; i++)
				{
					y = val(i, m - 1);
					if (y != ZERO)
					{
						y = matrixvalues[i][m - 1] = y / x;
						for (j = m; j < n; j++) matrixvalues[i][j] -= y * matrixvalues[m][j];
						for (j = 0; j <= high; j++) matrixvalues[j][m] += y * matrixvalues[j][i];
					}
				} /* end i */
			}
		} /* end m */

		return (0);
	}

	int elmtrans       /* copy to Hessenberg form .................*/
		(int     n,       /* Dimension of matrix .........*/
		int     low,     /* first nonzero row ...........*/
		int     high,    /* last nonzero row ............*/
		VECTE<int>&    perm,  /* row permutations ............*/
		KIFMATRIX&  h      /* Hessenberg matrix ...........*/
		)
		/*====================================================================*
		*                                                                    *
		*  elmtrans copies the Hessenberg matrix stored in mat to h.         *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Input parameters:                                                *
		*   ================                                                 *
		*      n        int n;  ( n > 0 )                                    *
		*               Dimension of  mat and eivec                          *
		*      low      int low;                                             *
		*      high     int high; see  balance                               *
		*      mat      REAL   *mat[n];                                      *
		*               n x n input matrix                                   *
		*      perm     int *perm;                                           *
		*               Permutation data from  elmhes                        *
		*                                                                    *
		*   Output parameter:                                                *
		*   ================                                                 *
		*      h        REAL   *h[n];                                        *
		*               Hessenberg matrix                                    *
		*                                                                    *
		*====================================================================
		*/
	{
		register int k, i, j;

		for (i = 0; i < n; i++)
		{
			for (k = 0; k < n; k++) h[i][k] = ZERO;
			h[i][i] = ONE;
		}

		for (i = high - 1; i > low; i--)
		{
			j = perm[i];
			for (k = i + 1; k <= high; k++) h[k][i] = val(k, i - 1);
			if (i != j)
			{
				for (k = i; k <= high; k++)
				{
					h[i][k] = h[j][k];
					h[j][k] = ZERO;
				}
				h[j][i] = ONE;
			}
		}

		return (0);
	}

	int orthes     /* reduce orthogonally to upper Hessenberg form */
		(
		int  n,                  /* Dimension of matrix     */
		int  low,                /* [low,low]..[high,high]: */
		int  high,               /* submatrix to be reduced */
		VECTE<REAL>& d                 /* reduction information   */
		)  {                       /* error code              */

		/***********************************************************************
		* This function reduces matrix mat to upper Hessenberg form by         *
		* Householder transformations. All details of the transformations are  *
		* stored in the remaining triangle of the Hessenberg matrix and in     *
		* vector d.                                                            *
		*                                                                      *
		* Input parameters:                                                    *
		* =================                                                    *
		* n        dimension of mat                                            *
		* low  \   rows 0 to low-1 and high+1 to n-1 contain isolated          *
		* high  >  eigenvalues, i. e. eigenvalues corresponding to             *
		*      /   eigenvectors that are multiples of unit vectors             *
		* mat      [0..n-1,0..n-1] matrix to be reduced                        *
		*                                                                      *
		* Output parameters:                                                   *
		* ==================                                                   *
		* mat      the desired Hessenberg matrix together with the first part  *
		*          of the reduction information below the subdiagonal          *
		* d        [0..n-1] vector with the remaining reduction information    *
		*                                                                      *
		* Return value:                                                        *
		* =============                                                        *
		* Error code. This can only be the value 0 here.                       *
		*                                                                      *
		* global names used:                                                   *
		* ==================                                                   *
		* REAL, MACH_EPS, ZERO, SQRT                                           *
		*                                                                      *
		* -------------------------------------------------------------------- *
		* Literature: Numerical Mathematics 12 (1968), pages 359 and 360       *
		**********************************************************************
		*/


		int  i, j, m;    /* loop variables                                  */
		REAL s,          /* Euclidian norm sigma of the subdiagonal column  */
			/* vector v of mat, that shall be reflected into a */
			/* multiple of the unit vector e1 = (1,0,...,0)    */
			/* (v = (v1,..,v(high-m+1))                        */
			x = ZERO,   /* first element of v in the beginning, then       */
			/* summation variable in the actual Householder    */
			/* transformation                                  */
			y,          /* sigma^2 in the beginning, then ||u||^2, with    */
			/* u := v +- sigma * e1                            */
			eps;        /* tolerance for checking if the transformation is */
		/* valid                                           */

		eps = (REAL)128.0 * MACH_EPS;

		for (m = low + 1; m < high; m++)
		{
			for (y = ZERO, i = high; i >= m; i--)
				x = val(i, m - 1);
			d[i] = x;
			y = y + x * x;
			if (y <= eps)
				s = ZERO;
			else
			{
				s = (x >= ZERO) ? -SQRT(y) : SQRT(y);
				y -= x * s;
				d[m] = x - s;

				for (j = m; j < n; j++)               /* multiply mat from the  */
				{                                     /* left by  (E-(u*uT)/y)  */
					for (x = ZERO, i = high; i >= m; i--)
						x += d[i] * val(i, j);
					for (x /= y, i = m; i <= high; i++)
						matrixvalues[i][j] -= x * d[i];
				}

				for (i = 0; i <= high; i++)           /* multiply mat from the  */
				{                                     /* right by  (E-(u*uT)/y) */
					for (x = ZERO, j = high; j >= m; j--)
						x += d[j] * val(i, j);
					for (x /= y, j = m; j <= high; j++)
						matrixvalues[i][j] -= x * d[j];
				}
			}

			matrixvalues[m][m - 1] = s;
		}

		return 0;

	}    /* --------------------------- orthes -------------------------- */

	int orttrans       /* compute orthogonal transformation matrix */
		(
		int  n,      /* Dimension of matrix               */
		int  low,    /* [low,low]..[high,high]: submatrix */
		int  high,   /* affected by the reduction         */
		VECTE<REAL>& d,    /* remaining reduction information   */
		KIFMATRIX& v    /* transformation matrix             */
		)   {         /* error code                        */

		/***********************************************************************
		* compute the matrix v of accumulated transformations from the         *
		* information left by the Householder reduction of matrix mat to upper *
		* Hessenberg form below the Hessenberg matrix in mat and in the        *
		* vector d. The contents of the latter are destroyed.                  *
		*                                                                      *
		* Input parameters:                                                    *
		* =================                                                    *
		* n        dimension of mat                                            *
		* low  \   rows 0 to low-1 and high+1 to n-1 contain isolated          *
		* high  >  eigenvalues, i. e. eigenvalues corresponding to             *
		*      /   eigenvectors that are multiples of unit vectors             *
		* mat      [0..n-1,0..n-1] matrix produced by `orthes' giving the      *
		*          upper Hessenberg matrix and part of the information on the  *
		*          orthogonal reduction                                        *
		* d        [0..n-1] vector with the remaining information on the       *
		*          orthogonal reduction to upper Hessenberg form               *
		*                                                                      *
		* Output parameters:                                                   *
		* ==================                                                   *
		* d        input vector destroyed by this function                     *
		* v        [0..n-1,0..n-1] matrix defining the similarity reduction    *
		*          to upper Hessenberg form                                    *
		*                                                                      *
		* Return value:                                                        *
		* =============                                                        *
		* Error code. This can only be the value 0 here.                       *
		*                                                                      *
		* global names used:                                                   *
		* =================                                                    *
		* REAL, ZERO, ONE                                                      *
		*                                                                      *
		* -------------------------------------------------------------------- *
		* Literature: Numerical Mathematics 16 (1970), page 191                *
		**********************************************************************
		*/


		int  i, j, m;                        /* loop variables              */
		REAL x,                              /* summation variable in the   */
			/* Householder transformation  */
			y;                              /* sigma  respectively         */
		/* sigma * (v1 +- sigma)       */

		for (i = 0; i < n; i++)              /* form the unit matrix in v   */
		{
			for (j = 0; j < n; j++)
				v[i][j] = ZERO;
			v[i][i] = ONE;
		}

		for (m = high - 1; m > low; m--)     /* apply the transformations   */
		{                                    /* that reduced mat to upper   */
			y = val(m, m - 1);                 /* Hessenberg form also to the */
			/* unit matrix in v. This      */
			if (y != ZERO)                     /* produces the desired        */
			{                                  /* transformation matrix in v. */
				y *= d[m];
				for (i = m + 1; i <= high; i++)
					d[i] = val(i, m - 1);
				for (j = m; j <= high; j++)
				{
					for (x = ZERO, i = m; i <= high; i++)
						x += d[i] * v[i][j];
					for (x /= y, i = m; i <= high; i++)
						v[i][j] += x * d[i];
				}
			}
		}

		return 0;

	}    /* -------------------------- orttrans ------------------------- */


	/* ------------------------------------------------------------------ */
	int hqrvec       /* compute eigenvectors ......................*/
		(int     n,           /* Dimension of matrix .......*/
		int     low,         /* first nonzero row .........*/
		int     high,        /* last nonzero row ..........*/
		VECTE<REAL>&    wr,        /* Real parts of evalues .....*/
		VECTE<REAL>&    wi,        /* Imaginary parts of evalues */
		KIFMATRIX& eivec      /* Eigenvectors ..............*/
		)
		/*====================================================================*
		*                                                                    *
		*  hqrvec computes the eigenvectors for the eigenvalues found in hqr2*
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Input parameters:                                                *
		*   ================                                                 *
		*      n        int n;  ( n > 0 )                                    *
		*               Dimension of  mat and eivec, number of eigenvalues.  *
		*      low      int low;                                             *
		*      high     int high; see  balance                               *
		*      h        REAL   *h[n];                                        *
		*               upper Hessenberg matrix                              *
		*      wr       REAL   wr[n];                                        *
		*               Real parts of the n eigenvalues.                     *
		*      wi       REAL   wi[n];                                        *
		*               Imaginary parts of the n eigenvalues.                *
		*                                                                    *
		*   Output parameter:                                                *
		*   ================                                                 *
		*      eivec    REAL   *eivec[n];                                    *
		*               Matrix, whose columns are the eigenvectors           *
		*                                                                    *
		*   Return value :                                                   *
		*   =============                                                    *
		*      =  0     all ok                                               *
		*      =  1     h is the zero matrix.                                *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   function in use  :                                               *
		*   ==================                                               *
		*                                                                    *
		*      int   comdiv(): complex division                              *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Constants used  :    MACH_EPS                                    *
		*   =================                                                *
		*                                                                    *
		*   Macros :   SQR, ABS                                              *
		*   ========                                                         *
		*                                                                    *
		*====================================================================
		*/
	{
		int   i, j, k;
		int   l, m, en, na;
		REAL  p, q, r = ZERO, s = ZERO, t, w, x, y, z = ZERO,
			ra, sa, vr, vi, norm;

		for (norm = ZERO, i = 0; i < n; i++)        /* find norm of h       */
		{
			for (j = i; j < n; j++) norm += ABS(val(i, j));
		}

		if (norm == ZERO) return (1);               /* zero matrix          */

		for (en = n - 1; en >= 0; en--)             /* transform back       */
		{
			p = wr[en];
			q = wi[en];
			na = en - 1;
			if (q == ZERO)
			{
				m = en;
				matrixvalues[en][en] = ONE;
				for (i = na; i >= 0; i--)
				{
					w = val(i, i) - p;
					r = val(i, en);
					for (j = m; j <= na; j++) r += val(i, j) * val(j, en);
					if (wi[i] < ZERO)
					{
						z = w;
						s = r;
					}
					else
					{
						m = i;
						if (wi[i] == ZERO)
							matrixvalues[i][en] = -r / ((w != ZERO) ? (w) : (MACH_EPS * norm));
						else
						{  /* Solve the linear system:            */
							/* | w   x |  | h[i][en]   |   | -r |  */
							/* |       |  |            | = |    |  */
							/* | y   z |  | h[i+1][en] |   | -s |  */

							x = val(i, i + 1);
							y = val(i + 1, i);
							q = SQR(wr[i] - p) + SQR(wi[i]);
							matrixvalues[i][en] = t = (x * s - z * r) / q;
							matrixvalues[i + 1][en] = ((ABS(x) > ABS(z)) ?
								(-r - w * t) / x : (-s - y * t) / z);
						}
					}  /* wi[i] >= 0  */
				}  /*  end i     */
			}  /* end q = 0  */

			else if (q < ZERO)
			{
				m = na;
				if (ABS(val(en, na)) > ABS(val(na, en)))
				{
					matrixvalues[na][na] = -(val(en, en) - p) / val(en, na);
					matrixvalues[na][en] = -q / val(en, na);
				}
				else
					comdiv(-val(na, en), ZERO, val(na, na) - p, q, matrixvalues[na][na], matrixvalues[na][en]);

				matrixvalues[en][na] = ONE;
				matrixvalues[en][en] = ZERO;
				for (i = na - 1; i >= 0; i--)
				{
					w = val(i, i) - p;
					ra = val(i, en);
					sa = ZERO;
					for (j = m; j <= na; j++)
					{
						ra += val(i, j) * val(j, na);
						sa += val(i, j) * val(j, en);
					}

					if (wi[i] < ZERO)
					{
						z = w;
						r = ra;
						s = sa;
					}
					else
					{
						m = i;
						if (wi[i] == ZERO)
							comdiv(-ra, -sa, w, q, matrixvalues[i][na], matrixvalues[i][en]);
						else
						{

							/* solve complex linear system:                              */
							/* | w+i*q     x | | h[i][na] + i*h[i][en]  |   | -ra+i*sa | */
							/* |             | |                        | = |          | */
							/* |   y    z+i*q| | h[i+1][na]+i*h[i+1][en]|   | -r+i*s   | */

							x = val(i, i + 1);
							y = val(i + 1, i);
							vr = SQR(wr[i] - p) + SQR(wi[i]) - SQR(q);
							vi = TWO * q * (wr[i] - p);
							if (vr == ZERO && vi == ZERO)
								vr = MACH_EPS * norm *
								(ABS(w) + ABS(q) + ABS(x) + ABS(y) + ABS(z));

							comdiv(x * r - z * ra + q * sa, x * s - z * sa - q * ra,
								vr, vi, matrixvalues[i][na], matrixvalues[i][en]);
							if (ABS(x) > ABS(z) + ABS(q))
							{
								matrixvalues[i + 1][na] = (-ra - w * val(i, na) + q * val(i, en)) / x;
								matrixvalues[i + 1][en] = (-sa - w * val(i, en) - q * val(i, na)) / x;
							}
							else
								comdiv(-r - y * val(i, na), -s - y * val(i, en), z, q,
								matrixvalues[i + 1][na], matrixvalues[i + 1][en]);

						}   /* end wi[i] > 0  */
					}   /* end wi[i] >= 0  */
				}   /* end i            */
			}    /*  if q < 0        */
		}    /* end  en           */

		for (i = 0; i < n; i++)         /* Eigenvectors for the evalues for */
		if (i < low || i > high)      /* rows < low  and rows  > high     */
		for (k = i + 1; k < n; k++) eivec[i][k] = val(i, k);

		for (j = n - 1; j >= low; j--)
		{
			m = (j <= high) ? j : high;
			if (wi[j] < ZERO)
			{
				for (l = j - 1, i = low; i <= high; i++)
				{
					for (y = z = ZERO, k = low; k <= m; k++)
					{
						y += eivec[i][k] * val(k, l);
						z += eivec[i][k] * val(k, j);
					}

					eivec[i][l] = y;
					eivec[i][j] = z;
				}
			}
			else
			if (wi[j] == ZERO)
			{
				for (i = low; i <= high; i++)
				{
					for (z = ZERO, k = low; k <= m; k++)
						z += eivec[i][k] * val(k, j);
					eivec[i][j] = z;
				}
			}

		}  /*  end j  */

		return (0);
	}

	/* ------------------------------------------------------------------ */
	int hqr2         /* compute eigenvalues .......................*/
		(int     vec,         /* switch for computing evectors*/
		int     n,           /* Dimension of matrix .........*/
		int     low,         /* first nonzero row ...........*/
		int     high,        /* last nonzero row ............*/
		VECTE<REAL>&    wr,        /* Real parts of eigenvalues ...*/
		VECTE<REAL>&    wi,        /* Imaginary parts of evalues ..*/
		KIFMATRIX& eivec,     /* Matrix of eigenvectors ......*/
		VECTE<int>&     cnt        /* Iteration counter ...........*/
		) {
		/*====================================================================*
		*                                                                    *
		*  hqr2 computes the eigenvalues and (if vec != 0) the eigenvectors  *
		*  of an  n * n upper Hessenberg matrix.                             *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Control parameter:                                               *
		*   ==================                                               *
		*      vec      int vec;                                             *
		*       = 0     compute eigenvalues only                             *
		*       = 1     compute all eigenvalues and eigenvectors             *
		*                                                                    *
		*   Input parameters:                                                *
		*   ================                                                 *
		*      n        int n;  ( n > 0 )                                    *
		*               Dimension of  mat and eivec,                         *
		*               length of the real parts vector  wr and of the       *
		*               imaginary parts vector  wi of the eigenvalues.       *
		*      low      int low;                                             *
		*      high     int high; see  balance                               *
		*      h        REAL   *h[n];                                        *
		*               upper  Hessenberg matrix                             *
		*                                                                    *
		*   Output parameters:                                               *
		*   ==================                                               *
		*      eivec    REAL   *eivec[n];     ( bei vec = 1 )                *
		*               Matrix, which for vec = 1 contains the eigenvectors  *
		*               as follows  :                                        *
		*               For real eigebvalues the corresponding column        *
		*               contains the corresponding eigenvactor, while for    *
		*               complex eigenvalues the corresponding column contains*
		*               the real part of the eigenvactor with its imaginary  *
		*               part is stored in the subsequent column of eivec.    *
		*               The eigenvactor for the complex conjugate eigenvactor*
		*               is given by the complex conjugate eigenvactor.       *
		*      wr       REAL   wr[n];                                        *
		*               Real part of the n eigenvalues.                      *
		*      wi       REAL   wi[n];                                        *
		*               Imaginary parts of the eigenvalues                   *
		*      cnt      int cnt[n];                                          *
		*               vector of iterations used for each eigenvalue.       *
		*               For a complex conjugate eigenvalue pair the second   *
		*               entry is negative.                                   *
		*                                                                    *
		*   Return value :                                                   *
		*   =============                                                    *
		*      =   0    all ok                                               *
		*      = 4xx    Iteration maximum exceeded when computing evalue xx  *
		*      =  99    zero  matrix                                         *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   functions in use  :                                              *
		*   ===================                                              *
		*                                                                    *
		*      int hqrvec(): reverse transform for eigenvectors              *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Constants used  :   MACH_EPS, MAXIT                              *
		*   =================                                                *
		*                                                                    *
		*   Macros  :   SWAP, ABS, SQRT                                      *
		*   =========                                                        *
		*                                                                    *
		*====================================================================
		*/

		int  i, j;
		int  na, en, iter, k, l, m;
		REAL p = ZERO, q = ZERO, r = ZERO, s, t, w, x, y, z;

		for (i = 0; i < n; i++)
		if (i < low || i > high)
		{
			wr[i] = val(i, i);
			wi[i] = ZERO;
			cnt[i] = 0;
		}

		en = high;
		t = ZERO;

		while (en >= low)
		{
			iter = 0;
			na = en - 1;

			for (;;)
			{
				for (l = en; l > low; l--)             /* search for small      */
				if (ABS(val(l, l - 1)) <=               /* subdiagonal element   */
					MACH_EPS * (ABS(val(l - 1, l - 1)) + ABS(val(l, l))))  break;

				x = val(en, en);
				if (l == en)                            /* found one evalue     */
				{
					wr[en] = matrixvalues[en][en] = x + t;
					wi[en] = ZERO;
					cnt[en] = iter;
					en--;
					break;
				}

				y = val(na, na);
				w = val(en, na) * val(na, en);

				if (l == na)                            /* found two evalues    */
				{
					p = (y - x) * 0.5;
					q = p * p + w;
					z = SQRT(ABS(q));
					x = matrixvalues[en][en] = x + t;
					matrixvalues[na][na] = y + t;
					cnt[en] = -iter;
					cnt[na] = iter;
					if (q >= ZERO)
					{                                     /* real eigenvalues     */
						z = (p < ZERO) ? (p - z) : (p + z);
						wr[na] = x + z;
						s = x - w / z;
						wr[en] = s;
						wi[na] = ZERO;
						wi[en] = ZERO;
						x = val(en, na);
						r = SQRT(x * x + z * z);

						if (vec)
						{
							p = x / r;
							q = z / r;
							for (j = na; j < n; j++)
							{
								z = val(na, j);
								matrixvalues[na][j] = q * z + p * val(en, j);
								matrixvalues[en][j] = q * val(en, j) - p * z;
							}

							for (i = 0; i <= en; i++)
							{
								z = val(i, na);
								matrixvalues[i][na] = q * z + p * val(i, en);
								matrixvalues[i][en] = q * val(i, en) - p * z;
							}

							for (i = low; i <= high; i++)
							{
								z = eivec[i][na];
								eivec[i][na] = q * z + p * eivec[i][en];
								eivec[i][en] = q * eivec[i][en] - p * z;
							}
						}  /* end if (vec) */
					}  /* end if (q >= ZERO) */
					else                                  /* pair of complex      */
					{                                     /* conjugate evalues    */
						wr[en] = x + p;
						wr[na] = x + p;
						wi[na] = z;
						wi[en] = -z;
					}

					en -= 2;
					break;
				}  /* end if (l == na) */

				if (iter >= MAXIT)
				{
					cnt[en] = MAXIT + 1;
					return (en);                         /* MAXIT Iterations     */
				}

				if ((iter != 0) && (iter % 10 == 0))
				{
					t += x;
					for (i = low; i <= en; i++) matrixvalues[i][i] -= x;
					s = ABS(val(en, na)) + ABS(val(na, en - 2));
					x = y = (REAL)0.75 * s;
					w = -(REAL)0.4375 * s * s;
				}

				iter++;

				for (m = en - 2; m >= l; m--)
				{
					z = val(m, m);
					r = x - z;
					s = y - z;
					p = (r * s - w) / val(m + 1, m) + val(m, m + 1);
					q = val(m + 1, m + 1) - z - r - s;
					r = val(m + 2, m + 1);
					s = ABS(p) + ABS(q) + ABS(r);
					p /= s;
					q /= s;
					r /= s;
					if (m == l) break;
					if (ABS(val(m, m - 1)) * (ABS(q) + ABS(r)) <=
						MACH_EPS * ABS(p)
						* (ABS(val(m - 1, m - 1)) + ABS(z) + ABS(val(m + 1, m + 1))))
						break;
				}

				for (i = m + 2; i <= en; i++) matrixvalues[i][i - 2] = ZERO;
				for (i = m + 3; i <= en; i++) matrixvalues[i][i - 3] = ZERO;

				for (k = m; k <= na; k++)
				{
					if (k != m)             /* double  QR step, for rows l to en  */
					{                       /* and columns m to en                */
						p = val(k, k - 1);
						q = val(k + 1, k - 1);
						r = (k != na) ? val(k + 2, k - 1) : ZERO;
						x = ABS(p) + ABS(q) + ABS(r);
						if (x == ZERO) continue;                  /*  next k        */
						p /= x;
						q /= x;
						r /= x;
					}
					s = SQRT(p * p + q * q + r * r);
					if (p < ZERO) s = -s;

					if (k != m) matrixvalues[k][k - 1] = -s * x;
					else if (l != m)
						matrixvalues[k][k - 1] = -val(k, k - 1);
					p += s;
					x = p / s;
					y = q / s;
					z = r / s;
					q /= p;
					r /= p;

					for (j = k; j < n; j++)               /* modify rows          */
					{
						p = val(k, j) + q * val(k + 1, j);
						if (k != na)
						{
							p += r * val(k + 2, j);
							matrixvalues[k + 2][j] -= p * z;
						}
						matrixvalues[k + 1][j] -= p * y;
						matrixvalues[k][j] -= p * x;
					}

					j = (k + 3 < en) ? (k + 3) : en;
					for (i = 0; i <= j; i++)              /* modify columns       */
					{
						p = x * val(i, k) + y * val(i, k + 1);
						if (k != na)
						{
							p += z * val(i, k + 2);
							matrixvalues[i][k + 2] -= p * r;
						}
						matrixvalues[i][k + 1] -= p * q;
						matrixvalues[i][k] -= p;
					}

					if (vec)      /* if eigenvectors are needed ..................*/
					{
						for (i = low; i <= high; i++)
						{
							p = x * eivec[i][k] + y * eivec[i][k + 1];
							if (k != na)
							{
								p += z * eivec[i][k + 2];
								eivec[i][k + 2] -= p * r;
							}
							eivec[i][k + 1] -= p * q;
							eivec[i][k] -= p;
						}
					}
				}    /* end k          */

			}    /* end for ( ; ;) */

		}    /* while (en >= low)                      All evalues found    */

		if (vec)                                /* transform evectors back  */
		if (hqrvec(n, low, high, wr, wi, eivec)) return (99);
		return (0);
	}

	int norm_1       /* normalize eigenvectors to have one norm 1 .*/
		(int     n,       /* Dimension of matrix ...........*/
		KIFMATRIX& v,     /* Matrix with eigenvektors ......*/
		VECTE<REAL>&    wi     /* Imaginary parts of evalues ....*/
		)
		/*====================================================================*
		*                                                                    *
		*  norm_1 normalizes the one norm of the column vectors in v.        *
		*  (special attention to complex vectors in v  is given)             *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Input parameters:                                                *
		*   ================                                                 *
		*      n        int n; ( n > 0 )                                     *
		*               Dimension of matrix v                                *
		*      v        REAL   *v[];                                         *
		*               Matrix of eigenvectors                               *
		*      wi       REAL   wi[];                                         *
		*               Imaginary parts of the eigenvalues                   *
		*                                                                    *
		*   Output parameter:                                                *
		*   ================                                                 *
		*      v        REAL   *v[];                                         *
		*               Matrix with normalized eigenvectors                  *
		*                                                                    *
		*   Return value :                                                   *
		*   =============                                                    *
		*      = 0      all ok                                               *
		*      = 1      n < 1                                                *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   functions used  :                                                *
		*   =================                                                *
		*      REAL   comabs():  complex absolute value                      *
		*      int    comdiv():  complex division                            *
		*                                                                    *
		*   Macros :   ABS                                                   *
		*   ========                                                         *
		*                                                                    *
		*====================================================================
		*/
	{
		int  i, j;
		REAL maxi, tr, ti;

		if (n < 1) return (1);

		for (j = 0; j < n; j++)
		{
			if (wi[j] == ZERO)
			{
				maxi = v[0][j];
				for (i = 1; i < n; i++)
				if (ABS(v[i][j]) > ABS(maxi))  maxi = v[i][j];

				if (maxi != ZERO)
				{
					maxi = ONE / maxi;
					for (i = 0; i < n; i++) v[i][j] *= maxi;
				}
			}
			else
			{
				tr = v[0][j];
				ti = v[0][j + 1];
				for (i = 1; i < n; i++)
				if (comabs(v[i][j], v[i][j + 1]) > comabs(tr, ti))
				{
					tr = v[i][j];
					ti = v[i][j + 1];
				}

				if (tr != ZERO || ti != ZERO)
				for (i = 0; i < n; i++)
					comdiv(v[i][j], v[i][j + 1], tr, ti, v[i][j], v[i][j + 1]);

				j++;                                          /* raise j by two */
			}
		}
		return (0);
	}

	int eigen               /* Compute all evalues/evectors of a matrix ..*/
		(
		int     vec,           /* switch for computing evectors ...*/
		int     ortho,         /* orthogonal Hessenberg reduction? */
		int     ev_norm,       /* normalize Eigenvectors? .........*/
		int     n,             /* size of matrix ..................*/
		KIFMATRIX& eivec,         /* Eigenvectors ....................*/
		VECTE<REAL>& valre,         /* real parts of eigenvalues .......*/
		VECTE<REAL>& valim,         /* imaginary parts of eigenvalues ..*/
		VECTE<int>& cnt            /* Iteration counter ...............*/
		)
		/*====================================================================*
		*                                                                    *
		*  The function  eigen  determines all eigenvalues and (if desired)  *
		*  all eigenvectors of a real square  n * n  matrix via the QR method*
		*  in the version of  Martin, Parlett, Peters, Reinsch and Wilkinson.*
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Literature:                                                      *
		*   ===========                                                      *
		*      1) Peters, Wilkinson: Eigenvectors of real and complex        *
		*         matrices by LR and QR triangularisations,                  *
		*         Num. Math. 16, p.184-204, (1970); [PETE70]; contribution   *
		*         II/15, p. 372 - 395 in [WILK71].                           *
		*      2) Martin, Wilkinson: Similarity reductions of a general      *
		*         matrix to Hessenberg form, Num. Math. 12, p. 349-368,(1968)*
		*         [MART 68]; contribution II,13, p. 339 - 358 in [WILK71].   *
		*      3) Parlett, Reinsch: Balancing a matrix for calculations of   *
		*         eigenvalues and eigenvectors, Num. Math. 13, p. 293-304,   *
		*         (1969); [PARL69]; contribution II/11, p.315 - 326 in       *
		*         [WILK71].                                                  *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Control parameters:                                              *
		*   ===================                                              *
		*      vec      int vec;                                             *
		*               call for eigen :                                     *
		*       = 0     compute eigenvalues only                             *
		*       = 1     compute all eigenvalues and eigenvectors             *
		*      ortho    flag that shows if transformation of mat to          *
		*               Hessenberg form shall be done orthogonally by        *
		*               `orthes' (flag set) or elementarily by `elmhes'      *
		*               (flag cleared). The Householder matrices used in     *
		*               orthogonal transformation have the advantage of      *
		*               preserving the symmetry of input matrices.           *
		*      ev_norm  flag that shows if Eigenvectors shall be             *
		*               normalized (flag set) or not (flag cleared)          *
		*                                                                    *
		*   Input parameters:                                                *
		*   ================                                                 *
		*      n        int n;  ( n > 0 )                                    *
		*               size of matrix, number of eigenvalues                *
		*      mat      REAL   *mat[n];                                      *
		*               matrix                                               *
		*                                                                    *
		*   Output parameters:                                               *
		*   ==================                                               *
		*      eivec    REAL   *eivec[n];     ( bei vec = 1 )                *
		*               matrix, if  vec = 1  this holds the eigenvectors     *
		*               thus :                                               *
		*               If the jth eigenvalue of the matrix is real then the *
		*               jth column is the corresponding real eigenvector;    *
		*               if the jth eigenvalue is complex then the jth column *
		*               of eivec contains the real part of the eigenvector   *
		*               while its imaginary part is in column j+1.           *
		*               (the j+1st eigenvector is the complex conjugate      *
		*               vector.)                                             *
		*      valre    REAL   valre[n];                                     *
		*               Real parts of the eigenvalues.                       *
		*      valim    REAL   valim[n];                                     *
		*               Imaginary parts of the eigenvalues                   *
		*      cnt      int cnt[n];                                          *
		*               vector containing the number of iterations for each  *
		*               eigenvalue. (for a complex conjugate pair the second *
		*               entry is negative.)                                  *
		*                                                                    *
		*   Return value :                                                   *
		*   =============                                                    *
		*      =   0    all ok                                               *
		*      =   1    n < 1 or other invalid input parameter               *
		*      =   2    insufficient memory                                  *
		*      = 10x    error x from balance()                               *
		*      = 20x    error x from elmh()                                  *
		*      = 30x    error x from elmtrans()   (for vec = 1 only)         *
		*      = 4xx    error xx from hqr2()                                 *
		*      = 50x    error x from balback()    (for vec = 1 only)         *
		*      = 60x    error x from norm_1()     (for vec = 1 only)         *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Functions in use   :                                             *
		*   ===================                                              *
		*                                                                    *
		*   static int balance (): Balancing of an  n x n  matrix            *
		*   static int elmh ():    Transformation to upper Hessenberg form   *
		*   static int elmtrans(): intialize eigenvectors                    *
		*   static int hqr2 ():    compute eigenvalues/eigenvectors          *
		*   static int balback (): Reverse balancing to obtain eigenvectors  *
		*   static int norm_1 ():  Normalize eigenvectors                    *
		*                                                                    *
		*   void *vmalloc():       allocate vector or matrix                 *
		*   void vmfree():         free list of vectors and matrices         *
		*                                                                    *
		*====================================================================*
		*                                                                    *
		*   Constants used   :     NULL, BASIS                               *
		*   ===================                                              *
		*                                                                    *
		*====================================================================*/
	{
		int      low, high, rc;
		VECTE<REAL> scale;
		VECTE<REAL> d;

		if (n < 1) return (1);                       /*  n >= 1 ............*/

		cnt.raz();

		if (n == 1)                                  /*  n = 1 .............*/
		{
			eivec[0][0] = ONE;
			valre[0] = val(0, 0);
			valim[0] = ZERO;
			return (0);
		}



		/* balance mat for nearly */
		rc = balance(n, scale,              /* equal row and column   */
			low, high, BASIS);      /* one norms              */
		if (rc)
		{
			return (100 + rc);
		}

		if (ortho)
			rc = orthes(n, low, high, d);
		else
			rc = elmhes(n, low, high, cnt);   /*  reduce mat to upper   */
		if (rc)                                   /*  Hessenberg form       */
		{
			return (200 + rc);
		}

		if (vec)                                  /*  initialize eivec      */
		{
			if (ortho)
				rc = orttrans(n, low, high, d, eivec);
			else
				rc = elmtrans(n, low, high, cnt, eivec);
			if (rc)
			{
				return (300 + rc);
			}
		}

		/*  execute Francis QR    */
		rc = hqr2(vec, n, low, high, valre, valim, eivec, cnt);     /*  algorithm to obtain   */
		if (rc)                                   /*  eigenvalues           */
		{
			return (400 + rc);
		}

		if (vec)
		{
			rc = balback(n, low, high,             /*  reverse balancing if  */
				scale, eivec);        /*  eigenvaectors are to  */
			if (rc)                                 /*  be determined         */
			{
				return (500 + rc);
			}
			if (ev_norm)
				rc = norm_1(n, eivec, valim);        /* normalize eigenvectors */
			if (rc)
			{
				return (600 + rc);
			}
		}

		return (0);
	}

	bool inversion(Kifsparse& Y, bool keep) {
		initializematrixvalues();

		//else Local decomposition
		int n = 1 + max(matrixbyrow.maxidx, matrixbycolumn.maxidx);
		VECTE<int> INDX;
		int id;
		//call LU decomposition, on the matrixbyrow values
		int rc = LUDCMP(n, INDX, id);
		if (rc) {
			matrixvalues.clear();
			return false;
		}


		VECTE<double> temp;
		int i;
		//We create an identity matrix, which will contain the final result... 
		for (i = 0; i < n; i++) {
			Y.matrixvalues[i][i] = 1;
			if (keep)
				Y.matrixbycolumn[i][i] = 0;
		}

		for (int j = 0; j < n; j++) {
			for (i = 0; i < n; i++)
				temp[i] = Y.val(i, j);
			LUBKSB(n, INDX, temp);
			for (i = 0; i < n; i++) {
				Y.matrixvalues[i][j] = temp[i];
				if (keep)
					Y.matrixbycolumn[i][j] = 0;
			}
		}

		matrixvalues.clear();
		return true;
	}


	//--------------------------------------------------------------------------------------------------------

	KifElement* MethodEigen(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {


		//else Local decomposition
		KifElement* keig = callfunc->Evaluatethread(0, domain, idthread);

		if (keig->type != kifsparse_type)
			return kifcode->Returnerror("MAT(105): wrong parameter, missing a sparse matrix as result");


		KifElement* kvalreal = callfunc->Evaluatethread(1, domain, idthread);
		if (!kvalreal->aVectorContainer())
			return kifcode->Returnerror("MAT(106): wrong parameter, missing a vector for real values");

		KifElement* kvalimaginary = callfunc->Evaluatethread(2, domain, idthread);
		if (!kvalimaginary->aVectorContainer())
			return kifcode->Returnerror("MAT(107): wrong parameter, missing a vector for imaginary values");

		int     vec = callfunc->Evaluatethread(3, domain, idthread)->Integer();
		int     ortho = callfunc->Evaluatethread(4, domain, idthread)->Integer();
		int     ev_norm = callfunc->Evaluatethread(5, domain, idthread)->Integer();

		int n = 1 + max(matrixbyrow.maxidx, matrixbycolumn.maxidx);
		initializematrixvalues();

		Kifsparse* eig = (Kifsparse*)keig;
		eig->Clear();

		VECTE<REAL> valre;         /* real parts of eigenvalues .......*/
		VECTE<REAL> valim;         /* imaginary parts of eigenvalues ..*/
		VECTE<int> cnt;            /* Iteration counter ...............*/

		int err = eigen(vec, ortho, ev_norm, n, eig->matrixvalues, valre, valim, cnt);
		eig->populate();
		int i;
		kvalreal->Clear();
		kvalimaginary->Clear();
		for (i = 0; i < valre.dernier; i++) {
			kvalreal->storevalue(valre[i]);
			kvalimaginary->storevalue(valim[i]);
		}

		return kifcode->Provideinteger(err);
	}

	KifElement* MethodLUDecomposition(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		bool tosolve = callfunc->Evaluatethread(1, domain, idthread)->Boolean();
		KifElement* ke = callfunc->Evaluatethread(0, domain, idthread);
		if (ke->type != kifVector)
			return kifcode->Returnerror("MAT(204): Expecting a vector as parameter");

		initializematrixvalues();
		//else Local decomposition
		int n = 1 + max(matrixbyrow.maxidx, matrixbycolumn.maxidx);
		VECTE<int> INDX;
		int id;
		//call LU decomposition, on the matrixbyrow values
		int rc = LUDCMP(n, INDX, id);
		if (rc) {
			matrixvalues.clear();
			return kifcode->Returnerror("MAT(103): No LU decomposition for this matrix");
		}

		KifVector* kvect = (KifVector*)ke;
		kvect->Clear();
		for (int i = 0; i < INDX.size(); i++)
			kvect->Push(kifcode->Provideinteger(INDX[i]));
		populate(true - tosolve);
		return kifTRUE;
	}

	KifElement* MethodLUSolve(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		if (matrixvalues.empty())
			return kifcode->Returnerror("MAT(205): You should have called 'LUdecomp' with second parameter as 'true'");

		KifElement* kindx = callfunc->Evaluatethread(0, domain, idthread);
		if (kindx->type != kifVector)
			return kifcode->Returnerror("MAT(204): Expecting a vector as parameter");

		KifElement* ke = callfunc->Evaluatethread(1, domain, idthread);
		if (ke->type != kifVector)
			return kifcode->Returnerror("MAT(204): Expecting a vector as parameter");

		//else Local decomposition
		int n = 1 + max(matrixbyrow.maxidx, matrixbycolumn.maxidx);
		KifVector* kvindx = (KifVector*)kindx;
		VECTE<int> INDX;
		for (int i = 0; i < kvindx->Size(); i++)
			INDX.push_back(kvindx->values[i]->Integer());

		KifVector* kvect = (KifVector*)ke;
		VECTE<double> temp;
		for (int i = 0; i < kvect->Size(); i++)
			temp.push_back(kvect->values[i]->Float());
		LUBKSB(n, INDX, temp);

		kvect->Clear();
		for (int i = 0; i < temp.dernier; i++)
			kvect->Push(kifcode->Providefloat(temp[i]));
		return ke;
	}

	KifElement* MethodLUBandMatrixDecomposition(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		bool local = callfunc->Evaluatethread(0, domain, idthread)->Boolean();
		KifElement* ke = callfunc->Evaluatethread(1, domain, idthread);

		if (ke->type != kifVector)
			return kifcode->Returnerror("MAT(204): Expecting a vector as parameter");

		initializematrixvalues();
		//else Local decomposition
		int n = 1 + max(matrixbyrow.maxidx, matrixbycolumn.maxidx);
		int ML = 1;
		int MU = 1;
		int IND;
		VECTE<int> IPVT;
		//call LU decomposition, on the matrixbyrow values
		NSBFAC(n, ML, MU, IPVT, IND);
		populate();
		for (int i = 1; i <= n; i++)
			((KifVector*)ke)->Push(kifcode->Providefloat(IPVT[i]));
		return kifcode->Provideinteger(IND);
	}

	KifElement* MethodLUBandMatrixSolve(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		bool local = callfunc->Evaluatethread(0, domain, idthread)->Boolean();
		KifElement* ke = callfunc->Evaluatethread(1, domain, idthread);

		if (ke->type != kifVector)
			return kifcode->Returnerror("MAT(204): Expecting a vector as parameter");

		initializematrixvalues();
		//else Local decomposition
		int n = 1 + max(matrixbyrow.maxidx, matrixbycolumn.maxidx);
		int ML = 1;
		int MU = 1;
		int IND;
		VECTE<int> IPVT;
		VECTE<REAL> B;
		VECTE<REAL> X;
		//call LU decomposition, on the matrixbyrow values
		NSBFAC(n, ML, MU, IPVT, IND);
		int i;
		for (i = 1; i <= n; i++)
			B[i] = 1.0;

		NSBSLV(n, ML, MU, IPVT, B, X);
		matrixvalues.clear();
		for (i = 1; i <= n; i++)
			((KifVector*)ke)->Push(kifcode->Providefloat(X[i]));
		return kifTRUE;
	}

	KifElement* MethodInversion(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		Kifsparse* Y = new Kifsparse(kifcode, NULL);
		if (!inversion(*Y, false)) {
			Y->Resetreference();
			matrixvalues.clear();
			return kifcode->Returnerror("MAT(104): Singular matrix, no solution");
		}
		matrixvalues.clear();
		Y->populate();
		return Y;
	}


	KifElement* MethodDeterminant(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		int i, id, n, rc;
		double det;

		n = 1 + max(matrixbyrow.maxidx, matrixbycolumn.maxidx);

		initializematrixvalues();
		VECTE<int> INDX;

		//call LU decomposition
		rc = LUDCMP(n, INDX, id);

		//calculate determinant and display
		det = id;
		if (rc == 1) {
			matrixvalues.clear();
			return kifcode->Returnerror("MAT(102): No determinant for this matrix");
		}

		for (i = 0; i < n; i++)  {
			det *= val(i, i);
			if (det == 0) {
				matrixvalues.clear();
				return kifZERO;
			}
		}

		matrixvalues.clear();
		return kifcode->Providefloat(det);
	}
	//---------------------------------------------------------------------------------------------------------------------

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
		return nb;
	}

	//How to interprete your class according to the basic following types:
	string String() {
		stringstream buff;

		for (int r = 0; r < rowsize; r++) {
			for (int c = 0; c < columnsize; c++) {
				if (c)
					buff << ",";
				if (matrixbyrow.find(r) && matrixbyrow[r].find(c))
					buff << matrixbyrow[r][c];
				else
					buff << "0";
			}
			buff << endl;
		}

		return buff.str();
	}

	long Integer() { return nb; }
	double Float() { return 0; }
	bool Boolean() { return true; }

	//You can overcome the following function behaviour
	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		return kifNULL;
	}
	//union of two sets
	KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifsparse_type || b->type != kifsparse_type) {
			Clear();
			return this;
		}

		Kifsparse* ma = (Kifsparse*)a;
		Kifsparse* mb = (Kifsparse*)b;
		Kifsparse* res;
		if (a != this) {
			res = this;
			res->Clear();
		}
		else
			res = new Kifsparse(kifcode, NULL);

		prime_iterator<long, prime_hash<long, double> > itr;
		prime_iterator<long, double> itl;




		unsigned int r, c;
		double v;
		for (itr = ma->matrixbyrow; !itr.end(); itr++) {
			//for each column for that row
			r = itr.key();
			for (itl = itr.value(); !itl.end(); itl++) {
				c = itl.key();
				if (mb->matrixbyrow.find(r) && mb->matrixbyrow[r].find(c)) {
					v = (long)itl.value() | (long)mb->matrixbyrow[r][c];
					res->populate(r, c, v);
				}
			}
		}
		return res;
	}

	//intersection of two sets
	KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifsparse_type || b->type != kifsparse_type) {
			Clear();
			return this;
		}

		Kifsparse* ma = (Kifsparse*)a;
		Kifsparse* mb = (Kifsparse*)b;
		Kifsparse* res;
		if (a != this) {
			res = this;
			res->Clear();
		}
		else
			res = new Kifsparse(kifcode, NULL);

		prime_iterator<long, prime_hash<long, double> > itr;
		prime_iterator<long, double> itl;

		unsigned int r, c;
		double v;
		for (itr = ma->matrixbyrow; !itr.end(); itr++) {
			//for each column for that row
			r = itr.key();
			for (itl = itr.value(); !itl.end(); itl++) {
				c = itl.key();
				if (mb->matrixbyrow.find(r) && mb->matrixbyrow[r].find(c)) {
					v = (long)itl.value()&(long)mb->matrixbyrow[r][c];
					res->populate(r, c, v);
				}
			}
		}
		return res;
	}

	KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifsparse_type || b->type != kifsparse_type) {
			Clear();
			return this;
		}

		Kifsparse* ma = (Kifsparse*)a;
		Kifsparse* mb = (Kifsparse*)b;
		Kifsparse* res;
		if (a != this) {
			res = this;
			res->Clear();
		}
		else
			res = new Kifsparse(kifcode, NULL);

		prime_iterator<long, prime_hash<long, double> > itr;
		prime_iterator<long, double> itl;

		unsigned int r, c;
		double v;
		for (itr = ma->matrixbyrow; !itr.end(); itr++) {
			//for each column for that row
			r = itr.key();
			for (itl = itr.value(); !itl.end(); itl++) {
				c = itl.key();
				if (mb->matrixbyrow.find(r) && mb->matrixbyrow[r].find(c)) {
					v = (long)itl.value() ^ (long)mb->matrixbyrow[r][c];
					res->populate(r, c, v);
				}
			}
		}
		return res;
	}

	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);

	//Beware that a might be the current element
	//plus: a+b
	KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

		if (a->type != kifsparse_type || b->type != kifsparse_type) {
			Clear();
			return this;
		}

		Kifsparse* ma = (Kifsparse*)a;
		Kifsparse* mb = (Kifsparse*)b;
		Kifsparse* res;
		if (a != this) {
			res = this;
			res->Clear();
		}
		else
			res = new Kifsparse(kifcode, NULL);

		prime_iterator<long, prime_hash<long, double> > itr;
		prime_iterator<long, double> itl;

		unsigned int r, c;
		double v;
		for (itr = ma->matrixbyrow; !itr.end(); itr++) {
			//for each column for that row
			r = itr.key();
			for (itl = itr.value(); !itl.end(); itl++) {
				c = itl.key();
				v = itl.value() + mb->Val(r, c);
				res->populate(r, c, v);
			}
		}
		return res;
	}

	//minus: a-b
	KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifsparse_type || b->type != kifsparse_type) {
			Clear();
			return this;
		}

		Kifsparse* ma = (Kifsparse*)a;
		Kifsparse* mb = (Kifsparse*)b;
		Kifsparse* res;
		if (a != this) {
			res = this;
			res->Clear();
		}
		else
			res = new Kifsparse(kifcode, NULL);

		prime_iterator<long, prime_hash<long, double> > itr;
		prime_iterator<long, double> itl;

		unsigned int r, c;
		double v;
		for (itr = ma->matrixbyrow; !itr.end(); itr++) {
			//for each column for that row
			r = itr.key();
			for (itl = itr.value(); !itl.end(); itl++) {
				c = itl.key();
				v = itl.value() - mb->Val(r, c);
				res->populate(r, c, v);
			}
		}
		return res;
	}

	//Division: a/b
	KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type != kifsparse_type || b->type != kifsparse_type) {
			Clear();
			return this;
		}

		Kifsparse* ma = (Kifsparse*)a;
		Kifsparse* mb = (Kifsparse*)b;
		Kifsparse* res;
		if (a != this) {
			res = this;
			res->Clear();
		}
		else
			res = new Kifsparse(kifcode, NULL);


		double v;
		unsigned int r, c;
		prime_iterator<long, prime_hash<long, double> > itva;
		prime_iterator<long, double> itl;

		if (b->type != kifsparse_type) {
			v = b->Float();
			if (v == 0)
				return kifcode->Returnerror("MAT(203): Cannot divide by 0");

			for (itva = ma->matrixbyrow; !itva.end(); itva++) {
				//for each column for that row
				r = itva.key();
				for (itl = itva.value(); !itl.end(); itl++) {
					c = itl.key();
					res->populate(r, c, itl.value() / v);
				}
			}
			return res;
		}


		Kifsparse inverted(NULL, NULL);

		if (!mb->inversion(inverted, true))
			return kifcode->Returnerror("MAT(202): Cannot divide with this matrix");

		if (ma->columnsize != inverted.rowsize || ma->rowsize != inverted.columnsize)
			return kifcode->Returnerror("MAT(209): Matrix size mismatch");

		prime_iterator<long, prime_hash<long, double> > itvb;

		for (itva = ma->matrixbyrow; !itva.end(); itva++) {
			r = itva.key();
			//for each column for that row
			for (itvb = inverted.matrixbycolumn; !itvb.end(); itvb++) {
				c = itvb.key();
				v = 0;
				for (itl = itva.value(); !itl.end(); itl++) {
					//then for each line from the other matrix	
					v += itl.value()*inverted.val(itl.key(), c);
				}
				res->populate(r, c, v);
			}
		}
		inverted.matrixvalues.clear();
		inverted.matrixbycolumn.clear();
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
		if (a->type != kifsparse_type)
			return kifFALSE;

		Kifsparse* ma = (Kifsparse*)a;

		prime_iterator<long, prime_hash<long, double> > itr;
		prime_iterator<long, double> itl;

		unsigned int r, c;
		for (itr = matrixbyrow; !itr.end(); itr++) {
			//for each column for that row
			r = itr.key();
			for (itl = itr.value(); !itl.end(); itl++) {
				c = itl.key();
				if (!ma->matrixbyrow.find(r) || !ma->matrixbyrow[r].find(c))
					return kifFALSE;
				if (itl.value() != ma->matrixbyrow[r][c])
					return kifFALSE;
			}
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
map<short, KifCallMethod> Kifsparse::kifexportedmethods;
map<short, kifsparseMethod>  Kifsparse::linkedmethods;
map<string, string> Kifsparse::infomethods;
hmap<short, bool> Kifsparse::methodassigned;

Exported KifElement* Kifsparse::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != kifsparse_type) {
		Clear();
		return this;
	}
	Kifsparse* ma = (Kifsparse*)a;
	Kifsparse* res = new Kifsparse(kifcode, NULL);

	double v;
	unsigned int r, c;
	prime_iterator<long, prime_hash<long, double> > itva;
	prime_iterator<long, double> itl;


	if (b->type != kifsparse_type) {
		v = b->Float();

		for (itva = ma->matrixbyrow; !itva.end(); itva++) {
			//for each column for that row
			r = itva.key();
			for (itl = itva.value(); !itl.end(); itl++) {
				c = itl.key();
				res->populate(r, c, v*itl.value());
			}
		}
		return res;
	}

	Kifsparse* mb = (Kifsparse*)b;
	if (ma->columnsize != mb->rowsize)
		return kifcode->Returnerror("MAT(209): Matrix size mismatch");

	int R = 0;
	int C = 0;
	prime_iterator<long, prime_hash<long, double> > itvb;
	for (itva = ma->matrixbyrow; !itva.end(); itva++) {
		r = itva.key();
		//for each column for that row
		itvb = mb->matrixbycolumn;
		for (itvb = mb->matrixbycolumn; !itvb.end(); itvb++) {
			c = itvb.key();
			v = 0;
			for (itl = itva.value(); !itl.end(); itl++) {
				//then for each line from the other matrix	
				v += itl.value()*mb->Val(itl.key(), c);
			}
			res->populate(r, c, v);
			R = max(R, r);
			C = max(C, c);
		}
	}
	res->rowsize = R + 1;
	res->columnsize = C + 1;
	return res;
}

void Kifsparse::populate(unsigned int r, unsigned int c, double value, bool filterzero) {
	if (value == 0 && filterzero)
		return;

	if (firstinsertion) {
		maxval = value;
		minval = value;
		firstinsertion = false;
	}
	else {
		maxval = max(value, maxval);
		minval = min(value, minval);
	}

	if (!matrixbyrow.find(r) || !matrixbyrow[r].find(c))
		nb++;

	rowsize = max(rowsize, r);
	columnsize = max(columnsize, c);
	matrixbyrow[r][c] = value;
	matrixbycolumn[c][r] = value;
}

void Kifsparse::populate(unsigned int r, unsigned int c, KifElement* value) {
	if (value->Float() == 0)
		return;

	if (firstinsertion) {
		maxval = value->Float();
		minval = maxval;
		firstinsertion = false;
	}
	else {
		maxval = max(value->Float(), maxval);
		minval = min(value->Float(), minval);
	}

	if (!matrixbyrow.find(r) || !matrixbyrow[r].find(c))
		nb++;

	rowsize = max(rowsize, r);
	columnsize = max(columnsize, c);
	matrixbyrow[r][c] = value->Float();
	matrixbycolumn[c][r] = value->Float();
}

//---------------------------------------------------------------------------------------------------------------------
void Kifsparseline::populate(unsigned int l, double value, bool filterzero) {
	if ((value == 0 && filterzero) || mvector == NULL)
		return;
	if (isrow)
		base->populate(idx, l, value, filterzero);
	else
		base->populate(l, idx, value, filterzero);
	sizevector = max(sizevector, l);
}

unsigned int Kifsparseline::Idx(KifElement* k) {
	if (base == NULL)
		return 0;
	if (isrow)
		return base->IdxRow(k);
	return base->IdxCol(k);
}

void Kifsparseline::IncrementNB() {
	base->nb++;
}

void Kifsparseline::DecrementNB() {
	base->nb--;
}

KifElement* Kifsparseline::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	if (mvector == NULL || b != this)
		return kifFALSE;

	prime_iterator<long, double> itl;
	//for each column for that row
	double va = a->Float();
	for (itl = *mvector; !itl.end(); itl++) {
		if (va == itl.value()) {
			if (idx == true)
				return kifcode->Provideinteger(itl.key());
			return kifTRUE;
		}
	}
	return kifFALSE;
}


void Kifsparseline::storevalue(KifElement* res, int r, int c, double v) {
	if (res != this) {
		char ch[100];
		if (res->aVector())
			res->storevalue(v);
		else {
			if (res->aMapContainer()) {
				sprintf_s(ch, 100, "%d:%d", r, c);
				res->storekey(ch, v);
			}
			else {
				if (res->type == kifsparseline_type) {
					if (isrow)
						((Kifsparseline*)res)->populate(c, v);
					else
						((Kifsparseline*)res)->populate(r, v);
				}
				else {
					if (isrow)
						((Kifsparse*)res)->populate(r, c, v);
					else
						((Kifsparse*)res)->populate(c, r, v);
				}
			}
		}
	}
	else {
		if (isrow)
			populate(c, v);
		else
			populate(r, v);
	}
}

//union of two sets
KifElement* Kifsparseline::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != kifsparseline_type) {
		Clear();
		return this;
	}

	Kifsparseline* ma = (Kifsparseline*)a;

	if (ma->mvector == NULL) {
		Clear();
		return this;
	}

	KifElement* res;
	if (a != context) {
		if (context->aVector())
			res = Selectvectorfloat(this, context);
		else {
			if (context->aMapContainer())
				res = Selectmapfloat(this, context);
			else
			if (context->type == kifsparseline_type)
				res = context;
			else
				res = new Kifsparse(kifcode, NULL);
		}
	}
	else
		res = a;

	prime_iterator<long, double> itl;

	unsigned int l;
	double v;
	//for each column for that row
	if (b->type == kifsparseline_type) {
		Kifsparseline* mb = (Kifsparseline*)b;
		if (mb->mvector == NULL)
			return res;
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			if (mb->mvector->find(l)) {
				v = (long)itl.value() | (long)mb->mvector->Get(l);
				storevalue(res, 0, l, v);
			}
		}
	}
	else {
		long vv = b->Integer();
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			v = (long)itl.value() | vv;
			storevalue(res, 0, l, v);
		}
	}
	return res;
}

//intersection of two sets
KifElement* Kifsparseline::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != kifsparseline_type) {
		Clear();
		return this;
	}

	Kifsparseline* ma = (Kifsparseline*)a;

	if (ma->mvector == NULL) {
		Clear();
		return this;
	}

	KifElement* res;
	if (a != context) {
		switch (context->type) {
		case kifVector:
			res = Selectvector(this, context);
			break;
		case kifVectorFloat:
			res = Selectvectorfloat(this, context);
			break;
		case kifMap:
			res = Selectmap(this, context);
			break;
		case kifMapStringFloat:
			res = new KifMapStringFloat(kifcode, NULL);
			break;
		default:
			if (context->type == kifsparseline_type)
				res = context;
			else
				res = new Kifsparse(kifcode, NULL);
		}
	}
	else
		res = a;

	prime_iterator<long, double> itl;

	unsigned int l;
	double v;
	//for each column for that row
	if (b->type == kifsparseline_type) {
		Kifsparseline* mb = (Kifsparseline*)b;
		if (mb->mvector == NULL)
			return res;
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			if (mb->mvector->find(l)) {
				v = (long)itl.value()&(long)mb->mvector->Get(l);
				storevalue(res, 0, l, v);
			}
		}
	}
	else {
		long vv = b->Integer();
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			v = (long)itl.value()&vv;
			storevalue(res, 0, l, v);
		}
	}
	return res;
}

KifElement* Kifsparseline::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != kifsparseline_type) {
		Clear();
		return this;
	}

	Kifsparseline* ma = (Kifsparseline*)a;

	if (ma->mvector == NULL) {
		Clear();
		return this;
	}

	KifElement* res;
	if (a != context) {
		switch (context->type) {
		case kifVector:
			res = Selectvector(this, context);
			break;
		case kifVectorFloat:
			res = Selectvectorfloat(this, context);
			break;
		case kifMap:
			res = Selectmap(this, context);
			break;
		case kifMapStringFloat:
			res = new KifMapStringFloat(kifcode, NULL);
			break;
		default:
			if (context->type == kifsparseline_type)
				res = context;
			else
				res = new Kifsparse(kifcode, NULL);
		}
	}
	else
		res = a;

	prime_iterator<long, double> itl;


	unsigned int l;
	double v;
	//for each column for that row
	if (b->type == kifsparseline_type) {
		Kifsparseline* mb = (Kifsparseline*)b;
		if (mb->mvector == NULL)
			return res;
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			if (mb->mvector->find(l)) {
				v = (long)itl.value() ^ (long)mb->mvector->Get(l);
				storevalue(res, 0, l, v);
			}
		}
	}
	else {
		long vv = b->Float();
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			v = (long)itl.value() ^ vv;
			storevalue(res, 0, l, v);
		}
	}
	return res;
}

Exported KifElement* Kifsparseline::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != kifsparseline_type) {
		Clear();
		return this;
	}

	Kifsparseline* ma = (Kifsparseline*)a;

	if (ma->mvector == NULL) {
		Clear();
		return this;
	}

	KifElement* res;
	if (a != context) {
		switch (context->type) {
		case kifVector:
			res = Selectvector(this, context);
			break;
		case kifVectorFloat:
			res = Selectvectorfloat(this, context);
			break;
		case kifMap:
			res = Selectmap(this, context);
			break;
		case kifMapStringFloat:
			res = new KifMapStringFloat(kifcode, NULL);
			break;
		default:
			if (context->type == kifsparseline_type)
				res = context;
			else
				res = new Kifsparse(kifcode, NULL);
		}
	}
	else
		res = a;

	prime_iterator<long, double> itl;


	unsigned int l;
	double v;
	//for each column for that row
	if (b->type == kifsparseline_type) {
		Kifsparseline* mb = (Kifsparseline*)b;
		if (mb->mvector == NULL)
			return res;
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			if (mb->mvector->find(l)) {
				v = itl.value()*mb->mvector->Get(l);
				storevalue(res, 0, l, v);
			}
		}
	}
	else {
		double vv = b->Float();
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			v = itl.value()*vv;
			storevalue(res, 0, l, v);
		}
	}
	return res;
}


//Beware that a might be the current element
//plus: a+b
KifElement* Kifsparseline::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != kifsparseline_type) {
		Clear();
		return this;
	}

	Kifsparseline* ma = (Kifsparseline*)a;

	if (ma->mvector == NULL) {
		Clear();
		return this;
	}

	KifElement* res;
	if (a != context) {
		switch (context->type) {
		case kifVector:
			res = Selectvector(this, context);
			break;
		case kifVectorFloat:
			res = Selectvectorfloat(this, context);
			break;
		case kifMap:
			res = Selectmap(this, context);
			break;
		case kifMapStringFloat:
			res = new KifMapStringFloat(kifcode, NULL);
			break;
		default:
			if (context->type == kifsparseline_type)
				res = context;
			else
				res = new Kifsparse(kifcode, NULL);
		}
	}
	else
		res = a;

	prime_iterator<long, double> itl;


	unsigned int l;
	double v;
	//for each column for that row
	if (b->type == kifsparseline_type) {
		Kifsparseline* mb = (Kifsparseline*)b;
		if (mb->mvector == NULL)
			return res;
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			if (mb->mvector->find(l)) {
				v = itl.value() + mb->mvector->Get(l);
				storevalue(res, 0, l, v);
			}
		}
	}
	else {
		double vv = b->Float();
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			v = itl.value() + vv;
			storevalue(res, 0, l, v);
		}
	}
	return res;
}


//minus: a-b
KifElement* Kifsparseline::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != kifsparseline_type) {
		Clear();
		return this;
	}

	Kifsparseline* ma = (Kifsparseline*)a;

	if (ma->mvector == NULL) {
		Clear();
		return this;
	}
	KifElement* res;
	if (a != context) {
		switch (context->type) {
		case kifVector:
			res = Selectvector(this, context);
			break;
		case kifVectorFloat:
			res = Selectvectorfloat(this, context);
			break;
		case kifMap:
			res = Selectmap(this, context);
			break;
		case kifMapStringFloat:
			res = new KifMapStringFloat(kifcode, NULL);
			break;
		default:
			if (context->type == kifsparseline_type)
				res = context;
			else
				res = new Kifsparse(kifcode, NULL);
		}
	}
	else
		res = a;

	prime_iterator<long, double> itl;


	unsigned int l;
	double v;
	//for each column for that row
	if (b->type == kifsparseline_type) {
		Kifsparseline* mb = (Kifsparseline*)b;
		if (mb->mvector == NULL)
			return res;
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			if (mb->mvector->find(l)) {
				v = itl.value() - mb->mvector->Get(l);
				storevalue(res, 0, l, v);
			}
		}
	}
	else {
		double vv = b->Float();
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			v = itl.value() - vv;
			storevalue(res, 0, l, v);
		}
	}
	return res;
}


//Division: a/b
KifElement* Kifsparseline::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != kifsparseline_type) {
		Clear();
		return this;
	}

	Kifsparseline* ma = (Kifsparseline*)a;

	if (ma->mvector == NULL) {
		Clear();
		return this;
	}

	KifElement* res = NULL;

	if (a != context) {
		switch (context->type) {
		case kifVector:
			res = Selectvector(this, context);
			break;
		case kifVectorFloat:
			res = Selectvectorfloat(this, context);
			break;
		case kifMap:
			res = Selectmap(this, context);
			break;
		case kifMapStringFloat:
			res = new KifMapStringFloat(kifcode, NULL);
			break;
		default:
			if (context->type == kifsparseline_type)
				res = context;
			else
				res = new Kifsparse(kifcode, NULL);
		}
	}
	else
		res = a;

	prime_iterator<long, double> itl;


	unsigned int l;
	double v;
	//for each column for that row
	if (b->type == kifsparseline_type) {
		Kifsparseline* mb = (Kifsparseline*)b;
		if (mb->mvector == NULL)
			return res;
		long vv;
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			if (mb->mvector->find(l)) {
				vv = mb->mvector->Get(l);
				if (vv == 0)
					return kifcode->Returnerror("MAT(203): Cannot divide by 0");
				v = itl.value() / vv;
				storevalue(res, 0, l, v);
			}
		}
	}
	else {
		double vv = b->Float();
		if (vv == 0)
			return kifcode->Returnerror("MAT(203): Cannot divide by 0");
		for (itl = *mvector; !itl.end(); itl++) {
			l = itl.key();
			v = itl.value() / vv;
			storevalue(res, 0, l, v);
		}
	}
	return res;
}


//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific iterator implementation...
KifElement* KifIteratorkifsparse::IteratorKey() {
	return kifcode->Provideinteger(aline.key());
}


KifElement* KifIteratorkifsparse::IteratorValue() {
	line->mvector = &aline.value();
	line->isrow = 1 - reverse;
	line->idx = aline.key();
	return line;
}

void KifIteratorkifsparse::Next() {
	//reverse denotes the direction of iteration (from beginning to end or reverse)	
	aline++;
}

KifElement* KifIteratorkifsparse::End() {
	if (aline.end()) {
		if (line != NULL) {
			line->pastouche = false;
			line->Resetreference();
			line = NULL;
		}
		return kifTRUE;
	}
	return kifFALSE;
}

KifElement* KifIteratorkifsparse::Begin() {
	//Your initialisation
	if (reverse)
		aline = value->matrixbycolumn;
	else
		aline = value->matrixbyrow;
	if (!aline.end()) {
		line = new Kifsparseline(kifcode, NULL);
		line->Setreference();
		line->base = value;
		line->pastouche = true;
		return kifTRUE;
	}
	return kifFALSE;
}
//-----------------------------------------------------------------------------------------------------------------------
KifElement* KifIteratorkifsparseline::IteratorKey() {
	if (value->isrow) {
		if (value->base->isrow.find(line.key()) != value->base->isrow.end())
			return kifcode->Providestring(value->base->isrow[line.key()]);
	}
	else
	if (value->base->iscol.find(line.key()) != value->base->iscol.end())
		return kifcode->Providestring(value->base->iscol[line.key()]);

	return kifcode->Provideinteger(line.key());
}

string KifIteratorkifsparseline::IteratorKeyString() {

	if (value->isrow) {
		if (value->base->isrow.find(line.key()) != value->base->isrow.end())
			return value->base->isrow[line.key()];
	}
	else
	if (value->base->iscol.find(line.key()) != value->base->iscol.end())
		return value->base->iscol[line.key()];

	char ch[20];
	sprintf_s(ch, 20, "%d", line.key());
	return ch;
}


long KifIteratorkifsparseline::IteratorValueInteger() {
	return line.value();
}

double KifIteratorkifsparseline::IteratorValueFloat() {
	return line.value();
}

KifElement* KifIteratorkifsparseline::IteratorValue() {
	return kifcode->Providefloat(line.value());
}

void KifIteratorkifsparseline::Next() {
	//reverse denotes the direction of iteration (from beginning to end or reverse)	
	line++;
}

KifElement* KifIteratorkifsparseline::End() {
	if (line.end())
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifIteratorkifsparseline::Begin() {
	//Your initialisation
	if (value == NULL || value->mvector == NULL) {
		line.raz();
		return kifFALSE;
	}

	line = *value->mvector;
	if (!line.end())
		return kifTRUE;
	return kifFALSE;
}
//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifsparseElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifsparse* local = new Kifsparse(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitializationSparse(KifCode* kifcode, string name, kifsparseMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifsparse::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifsparse::linkedmethods[idname] = func;
	Kifsparse::methodassigned[idname] = true;
	Kifsparse::infomethods[name] = infos;
}

static KifElement* CreatekifsparselineElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifsparseline* local = new Kifsparseline(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitializationSparseLine(KifCode* kifcode, string name, kifsparselineMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifsparseline::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifsparseline::linkedmethods[idname] = func;
	Kifsparseline::methodassigned[idname] = true;
	Kifsparseline::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exported bool Init_kifsparse(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}
		kifcode->KifInitLib();

		///The new type will be: kifsparse. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifsparseElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifsparse)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifsparseElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("matrix", CreatekifsparseElement);
		if (res == kifVOID)
			return false;
		kifsparse_type = res;


		res = KifAddNewType("mvector", CreatekifsparselineElement);
		if (res == kifVOID)
			return false;
		kifsparseline_type = res;
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

		MethodInitializationSparse(kifcode, "boundaries", &Kifsparse::MethodBoundaries, P_NONE, "boundaries(): return the matrix boundaries");
		MethodInitializationSparse(kifcode, "dimension", &Kifsparse::MethodMatrixSize, P_NONE | P_TWO, "dimension(): return the matrix size.\ndimension(int rowsize,int columnsize): set the matrix size.");
		MethodInitializationSparse(kifcode, "occupation", &Kifsparse::MethodOccupation, P_ONE, "occupation(bool row): return the matrix memory occupation as a vector");
		MethodInitializationSparse(kifcode, "row", &Kifsparse::MethodRow, P_ONE, "row(x): return the value on row x as a map or as a mvector");
		MethodInitializationSparse(kifcode, "column", &Kifsparse::MethodColumn, P_ONE, "column(x): return the value on column x as a map or as a mvector");
		MethodInitializationSparse(kifcode, "rowkey", &Kifsparse::MethodRowKey, P_NONE | P_ONE, "rowkey(int r): return the key matching r or the vector of all row keys");
		MethodInitializationSparse(kifcode, "colkey", &Kifsparse::MethodColKey, P_NONE | P_ONE, "colkey(int c): return the key matching c or the vector of all column keys");
		MethodInitializationSparse(kifcode, "transposed", &Kifsparse::MethodTransposed, P_NONE, "transposed(): return the transposed matrix");
		MethodInitializationSparse(kifcode, "determinant", &Kifsparse::MethodDeterminant, P_NONE, "determinant(): return the matrix determinant");
		MethodInitializationSparse(kifcode, "duplicate", &Kifsparse::MethodDuplicate, P_NONE, "duplicate(): return a duplication of the matrix");
		MethodInitializationSparse(kifcode, "LUdecomp", &Kifsparse::MethodLUDecomposition, P_TWO, "LUdecomp(vector permutations,bool tosolve): Replaces the current matrix with the LU decomposition of a rowwise permutation of itself. tosolve should be used if the next instruction is LUsolve");
		MethodInitializationSparse(kifcode, "LUsolve", &Kifsparse::MethodLUSolve, P_TWO, "LUsolve(vector permutations,vector values): solve linear equations based on values content. Return the result in values...");
		MethodInitializationSparse(kifcode, "invert", &Kifsparse::MethodInversion, P_NONE, "invert(): Return the inverted matrix.");
		MethodInitializationSparse(kifcode, "LUbanddecomp", &Kifsparse::MethodLUBandMatrixDecomposition, P_TWO, "LUbanddecomp(bool local,vector res): return a LU decomposition of a band matrix");
		MethodInitializationSparse(kifcode, "LUbandsolver", &Kifsparse::MethodLUBandMatrixSolve, P_TWO, "LUbandsolver(bool local,vector res): return a LU factorization of a band matrix");
		MethodInitializationSparse(kifcode, "min", &Kifsparse::MethodMin, P_NONE, "min(): return the min value of the matrix");
		MethodInitializationSparse(kifcode, "max", &Kifsparse::MethodMax, P_NONE, "max(): return the max value of the matrix");
		MethodInitializationSparse(kifcode, "eigen", &Kifsparse::MethodEigen, P_SIX,
			"eigen(sparse eigenmatrix,vector valreal,vector valimaginary,int vec,int ortho,int ev_norm):\
				vec = 0     compute eigenvalues only\
					vec = 1     compute all eigenvalues and eigenvectors\
						ortho    flag that shows if transformation of mat to\
							Hessenberg form shall be done orthogonally by `orthes' (flag set) or elementarily by `elmhes' (flag cleared). The Householder matrices used in\
								orthogonal transformation have the advantage of preserving the symmetry of input matrices.\
									ev_norm  flag that shows if Eigenvectors shall be normalized (flag set) or not (flag cleared)");
		MethodInitializationSparseLine(kifcode, "sum", &Kifsparseline::MethodSum, P_NONE, "sum(): return the sum of all elements");
		MethodInitializationSparseLine(kifcode, "product", &Kifsparseline::MethodProduct, P_NONE, "product(): return the product of all elements");
		MethodInitializationSparseLine(kifcode, "index", &Kifsparseline::MethodIndex, P_NONE, "index(): return the column or the row current index");
		MethodInitializationSparseLine(kifcode, "min", &Kifsparseline::MethodMin, P_NONE, "min(): return the min value of the sparse vector");
		MethodInitializationSparseLine(kifcode, "max", &Kifsparseline::MethodMax, P_NONE, "max(): return the max value of the sparse vector");

		kifGlobals->kifAllFunctions[kifsparseline_type] = Kifsparseline::methodassigned;
		kifGlobals->kifAllInformation[kifsparseline_type] = Kifsparseline::infomethods;
		kifGlobals->kifAllFunctions[kifsparse_type] = Kifsparse::methodassigned;
		kifGlobals->kifAllInformation[kifsparse_type] = Kifsparse::infomethods;

		return true;
	}

	Exported bool kifsparse_Terminate(KifCode* kifcode) {
		Kifsparseline::kifexportedmethods.clear();
		Kifsparseline::linkedmethods.clear();
		Kifsparseline::methodassigned.clear();
		Kifsparseline::infomethods.clear();
		Kifsparse::kifexportedmethods.clear();
		Kifsparse::linkedmethods.clear();
		Kifsparse::methodassigned.clear();
		Kifsparse::infomethods.clear();
		return true;
	}
}




