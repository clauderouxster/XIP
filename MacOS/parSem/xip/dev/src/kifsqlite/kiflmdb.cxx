/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the KiF library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : KiF
Version    : 1.00
filename   : kiflmdb.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language: lmdb implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "kif.h"
#include "kifpredicate.h"
#include "kifversion.h"
#include "kifcontainer.h"

#define _XOPEN_SOURCE 500

#ifdef _WIN32
#include "win32compability.h"
#endif
#include "lmdb.h"

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif


short KifNewId(string n);
string KifIdString(short n);
string& Trim(string&);
#define STR(x) (char*)x.c_str()

#ifdef WIN32
#define sprintf_ss(a,b,c,d,e) sprintf_s(a,b,c,d,e)
#else
#define sprintf_s(a,b,c,d) sprintf(a,c,d)
#define sprintf_ss(a,b,c,d,e) sprintf(a,c,d,e)
#define strcpy_s(a,b,c) strcpy(a,c)
#endif

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType lmdb_type;

//We create a map between our methods, which have been declared in our class below with their names.
class kiflmdb;
//This typedef defines a type "LMDBMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (kiflmdb::*LMDBMethod)(KifElement*, KifDomain*, KifCallFunction*, int);
//----------------------------------------------------------------------------------------------
static void addvariable(KifCode* kifcode, string name, long value) {
	short idname = KifNewId(name);
	KifVariableDeclaration* var = kifcode->Createstaticvariable(name);
	KifBaseInteger* local = new KifBaseInteger(kifcode, var, value);
}

static map<long, string> kiflmdperrors;
static bool create_variables(KifCode* kifcode) {


	short idname = KifNewId("MDB_FIXEDMAP");
	if (kifcode->gGlobals->kconstants->declarations.find(idname) != kifcode->gGlobals->kconstants->declarations.end())
		return false;

	addvariable(kifcode, "MDB_FIXEDMAP", 0x01);
	addvariable(kifcode, "MDB_NOSUBDIR", 0x4000);
	addvariable(kifcode, "MDB_NOSYNC", 0x10000);
	addvariable(kifcode, "MDB_RDONLY", 0x20000);
	addvariable(kifcode, "MDB_NOMETASYNC", 0x40000);
	addvariable(kifcode, "MDB_WRITEMAP", 0x80000);
	addvariable(kifcode, "MDB_MAPASYNC", 0x100000);
	addvariable(kifcode, "MDB_REVERSEKEY", 0x02);
	addvariable(kifcode, "MDB_DUPSORT", 0x04);
	addvariable(kifcode, "MDB_INTEGERKEY", 0x08);
	addvariable(kifcode, "MDB_DUPFIXED", 0x10);
	addvariable(kifcode, "MDB_INTEGERDUP", 0x20);
	addvariable(kifcode, "MDB_REVERSEDUP", 0x40);
	addvariable(kifcode, "MDB_CREATE", 0x40000);

	addvariable(kifcode, "MDB_NOOVERWRITE", 0x10);
	/** Only for #MDB_DUPSORT<br>
	* For put: don't write if the key and data pair already exist.<br>
	* For mdb_cursor_del: remove all duplicate data items.
	*/
	addvariable(kifcode, "MDB_NODUPDATA", 0x20);
	/** For mdb_cursor_put: overwrite the current key/data pair */
	addvariable(kifcode, "MDB_CURRENT", 0x40);
	/** For put: Just reserve space for data, don't copy it. Return a
	* pointer to the reserved space.
	*/
	addvariable(kifcode, "MDB_RESERVE", 0x10000);
	/** Data is being appended, don't split full pages. */
	addvariable(kifcode, "MDB_APPEND", 0x20000);
	/** Duplicate data is being appended, don't split full pages. */
	addvariable(kifcode, "MDB_APPENDDUP", 0x40000);
	/** Store multiple data items in one call. */
	addvariable(kifcode, "MDB_MULTIPLE", 0x80000);

	addvariable(kifcode, "MDB_FIRST", MDB_FIRST);
	addvariable(kifcode, "MDB_FIRST_DUP", MDB_FIRST_DUP);
	addvariable(kifcode, "MDB_GET_BOTH", MDB_GET_BOTH);
	addvariable(kifcode, "MDB_GET_BOTH_RANGE", MDB_GET_BOTH_RANGE);
	addvariable(kifcode, "MDB_GET_CURRENT", MDB_GET_CURRENT);
	addvariable(kifcode, "MDB_GET_MULTIPLE", MDB_GET_MULTIPLE);
	addvariable(kifcode, "MDB_LAST", MDB_LAST);
	addvariable(kifcode, "MDB_LAST_DUP", MDB_LAST_DUP);
	addvariable(kifcode, "MDB_NEXT", MDB_NEXT);
	addvariable(kifcode, "MDB_NEXT_DUP", MDB_NEXT_DUP);
	addvariable(kifcode, "MDB_NEXT_MULTIPLE", MDB_NEXT_MULTIPLE);
	addvariable(kifcode, "MDB_NEXT_NODUP", MDB_NEXT_NODUP);
	addvariable(kifcode, "MDB_PREV", MDB_PREV);
	addvariable(kifcode, "MDB_PREV_DUP", MDB_PREV_DUP);
	addvariable(kifcode, "MDB_PREV_NODUP", MDB_PREV_NODUP);
	addvariable(kifcode, "MDB_SET", MDB_SET);
	addvariable(kifcode, "MDB_SET_KEY", MDB_SET_KEY);
	addvariable(kifcode, "MDB_SET_RANGE", MDB_SET_RANGE);

	addvariable(kifcode, "MDB_SUCCESS", 0);
	/** key/data pair already exists */
	addvariable(kifcode, "MDB_KEYEXIST", (-30799));
	/** key/data pair not found (EOF) */
	addvariable(kifcode, "MDB_NOTFOUND", (-30798));
	/** Requested page not found - this usually indicates corruption */
	addvariable(kifcode, "MDB_PAGE_NOTFOUND", (-30797));
	/** Located page was wrong type */
	addvariable(kifcode, "MDB_CORRUPTED", (-30796));
	/** Update of meta page failed, probably I/O error */
	addvariable(kifcode, "MDB_PANIC", (-30795));
	/** Environment version mismatch */
	addvariable(kifcode, "MDB_VERSION_MISMATCH", (-30794));
	/** File is not a valid MDB file */
	addvariable(kifcode, "MDB_INVALID", (-30793));
	/** Environment mapsize reached */
	addvariable(kifcode, "MDB_MAP_FULL", (-30792));
	/** Environment maxdbs reached */
	addvariable(kifcode, "MDB_DBS_FULL", (-30791));
	/** Environment maxreaders reached */
	addvariable(kifcode, "MDB_READERS_FULL", (-30790));
	/** Too many TLS keys in use - Windows only */
	addvariable(kifcode, "MDB_TLS_FULL", (-30789));
	/** Txn has too many dirty pages */
	addvariable(kifcode, "MDB_TXN_FULL", (-30788));
	/** Cursor stack too deep - internal error */
	addvariable(kifcode, "MDB_CURSOR_FULL", (-30787));
	/** Page has not enough space - internal error */
	addvariable(kifcode, "MDB_PAGE_FULL", (-30786));
	/** Database contents grew beyond environment mapsize */
	addvariable(kifcode, "MDB_MAP_RESIZED", (-30785));
	/** Database flags changed or would change */
	addvariable(kifcode, "MDB_INCOMPATIBLE", (-30784));
	addvariable(kifcode, "MDB_LAST_ERRCODE", MDB_INCOMPATIBLE);

	kiflmdperrors[0] = "SUCCESS";
	/** key/data pair already exists */
	kiflmdperrors[-30799] = "KEYEXIST";
	/** key/data pair not found (EOF) */
	kiflmdperrors[-30798] = "NOT FOUND";
	/** Requested page not found - this usually indicates corruption */
	kiflmdperrors[-30797] = "PAGE NOT FOUND";
	/** Located page was wrong type */
	kiflmdperrors[-30796] = "CORRUPTED";
	/** Update of meta page failed, probably I/O error */
	kiflmdperrors[-30795] = "PANIC";
	/** Environment version mismatch */
	kiflmdperrors[-30794] = "VERSION MISMATCH";
	/** File is not a valid MDB file */
	kiflmdperrors[-30793] = "INVALID";
	/** Environment mapsize reached */
	kiflmdperrors[-30792] = "MAP FULL";
	/** Environment maxdbs reached */
	kiflmdperrors[-30791] = "DBS FULL";
	/** Environment maxreaders reached */
	kiflmdperrors[-30790] = "READERS FULL";
	/** Too many TLS keys in use - Windows only */
	kiflmdperrors[-30789] = "TLS FULL";
	/** Txn has too many dirty pages */
	kiflmdperrors[-30788] = "TXN FULL";
	/** Cursor stack too deep - internal error */
	kiflmdperrors[-30787] = "CURSOR FULL";
	/** Page has not enough space - internal error */
	kiflmdperrors[-30786] = "PAGE FULL";
	/** Database contents grew beyond environment mapsize */
	kiflmdperrors[-30785] = "MAP RESIZED";
	/** Database flags changed or would change */
	kiflmdperrors[-30784] = "INCOMPATIBLE";
	kiflmdperrors[MDB_INCOMPATIBLE] = "LAST ERRCODE";
	return true;
}

static bool isDirectory(string dirName_in) {
#ifdef WIN32
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;
#else
	struct stat st;
	if (stat(STR(dirName_in), &st) == 0)
	if ((st.st_mode & S_IFMT) == S_IFDIR)
		return true;
	return false;    // this is not a directory!
#endif
}

class KifIteratorLMDB : public KifIteration {
public:
	kiflmdb* value;
	MDB_cursor *cursor;
	int endoftransaction;
	MDB_val key, data;
	MDB_txn *txn;
	bool start;
	MDB_cursor_op cursorflag;

	KifIteratorLMDB(KifCode* klc, KifElement* kp) : KifIteration(klc, kp) {
		start = false;
		endoftransaction = 0;
		cursor = NULL;
		txn = NULL;
		key.mv_data = NULL;
		key.mv_size = 0;
		data.mv_data = NULL;
		data.mv_size = 0;
	}


	//The next methods are the one that should be derived to add iterator capacities to kiflmdb

	KifElement* IteratorKey();
	KifElement* IteratorValue();
	void Next();
	KifElement* End();
	KifElement* Begin();

	~KifIteratorLMDB();
};


class kiflmdb : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, LMDBMethod> linkedmethods;
	static hmap<short, bool> methodassigned;	static map<string, string> infomethods;


	MDB_env *env;
	MDB_dbi dbi;
	map<string, MDB_dbi> databases;
	MDB_val key, data;
	MDB_txn *txn;
	size_t fixedsize;
	bool integerkey;
	MDB_stat mst;
	string directory;
	size_t size;
	MDB_cursor_op cursorflag;
	MDB_cursor *cursor;
	bool selfcommit;

	kiflmdb(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, lmdb_type) {
		fixedsize = 0;
		size = 0;
		integerkey = false;
		env = NULL;
		selfcommit = false;
		txn = NULL;
		cursorflag = MDB_NEXT;
		cursor = NULL;
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return lmdb_type;
	}

	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//iterator it=mydb;
	KifElement* Newiterator(bool rev) {
		KifIteratorLMDB* iter = new KifIteratorLMDB(kifcode, NULL);
		iter->reverse = rev;
		//we set the value with the right one
		iter->value = this;
		iter->cursorflag = cursorflag;
		return iter;
	}


	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		kiflmdb* kperso = new kiflmdb(kifcode, kp);
		return kperso;
	}

	//Declaration
	//All our methods must have been declared in our kiflmdbmethod variable (which might take any name you want)
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: list, type or isa
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
		return kifcode->Providestring(infomethods[s]);
	}


	//----------------------------------------------------------
	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
		if (env == NULL) {
			kifcode->Returnerror("LMD(005): Database has not been opened yet");
			return true;
		}

		if (idx == NULL || idx->isConst())
			return true;


		int rc = 0;
		if (txn == NULL) {
			rc = mdb_txn_begin(env, NULL, 0, &txn);
			if (rc) {
				string mess = "LMD(017): Error";
				if (kiflmdperrors.find(rc) != kiflmdperrors.end())
					mess += ": " + kiflmdperrors[rc];
				else {
					ostringstream convert;
					char* msg = mdb_strerror(rc);
					if (msg != NULL)
						convert << ": " << msg << "(" << rc << ")";
					else
						convert << " (" << rc << ")";
					mess += convert.str();
				}

				return kifcode->Returnerror(mess);
			}
		}

		long v;
		char* vkey = NULL;
		string avalue = ke->String();
		int msz = max(avalue.size() + 1, fixedsize);
		char* vvalue = new char[msz];
		strcpy(vvalue, STR(avalue));
		if (integerkey) {
			key.mv_size = sizeof(long);
			v = idx->Integer();
			key.mv_data = &v;
		}
		else {
			string akey = idx->String();
			msz = max(akey.size() + 1, fixedsize);
			vkey = new char[msz];
			strcpy(vkey, STR(akey));
			key.mv_data = vkey;
			if (fixedsize)
				key.mv_size = fixedsize;
			else
				key.mv_size = akey.size() + 1;
		}

		if (fixedsize)
			data.mv_size = fixedsize;
		else
			data.mv_size = avalue.size() + 1;

		data.mv_data = vvalue;
		rc = mdb_put(txn, dbi, &key, &data, MDB_NODUPDATA);

		delete[] vvalue;
		if (vkey != NULL)
			delete[] vkey;

		if (rc) {
			string mess = "LMD(018): Error in PUT";
			if (kiflmdperrors.find(rc) != kiflmdperrors.end())
				mess += ": " + kiflmdperrors[rc];
			else {
				ostringstream convert;
				char* msg = mdb_strerror(rc);
				if (msg != NULL)
					convert << ": " << msg << "(" << rc << ")";
				else
					convert << " (" << rc << ")";
				mess += convert.str();
			}

			kifcode->Returnerror(mess);
		}

		if (selfcommit) {
			mdb_txn_commit(txn);
			txn = NULL;
		}
		return true;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement*  idx, int idthread, KifCallFunction* callfunc) {
		if (env == NULL)
			return kifcode->Returnerror("LMD(005): Database has not been opened yet");

		if (idx == NULL || idx->isConst())
			return kifNULL;

		KifElement* kkey;
		if (idx->type == kifIndex) {
			KifIndex* kind = (KifIndex*)idx;
			kkey = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		}
		else
			kkey = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

		if (kkey == kifNULL)
			return kifNOELEMENT;

		int rc = 0;
		if (txn == NULL) {
			rc = mdb_txn_begin(env, NULL, 0, &txn);
			if (rc) {
				string mess = "LMD(017): Error";
				if (kiflmdperrors.find(rc) != kiflmdperrors.end())
					mess += ": " + kiflmdperrors[rc];
				else {
					ostringstream convert;
					char* msg = mdb_strerror(rc);
					if (msg != NULL)
						convert << ": " << msg << "(" << rc << ")";
					else
						convert << " (" << rc << ")";
					mess += convert.str();
				}

				return kifcode->Returnerror(mess);
			}
		}

		rc = mdb_cursor_open(txn, dbi, &cursor);
		if (rc) {
			string mess = "LMD(017): Error";
			if (kiflmdperrors.find(rc) != kiflmdperrors.end())
				mess += ": " + kiflmdperrors[rc];
			else {
				ostringstream convert;
				char* msg = mdb_strerror(rc);
				if (msg != NULL)
					convert << ": " << msg << "(" << rc << ")";
				else
					convert << " (" << rc << ")";
				mess += convert.str();
			}

			return kifcode->Returnerror(mess);
		}

		char* vkey = NULL;
		string akey;
		long v;
		if (integerkey) {
			key.mv_size = sizeof(long);
			v = idx->Integer();
			key.mv_data = &v;
		}
		else {
			akey = kkey->String();
			int msz = max(akey.size() + 1, fixedsize);
			vkey = new char[msz];
			strcpy(vkey, STR(akey));
			if (fixedsize)
				key.mv_size = fixedsize;
			else
				key.mv_size = akey.size() + 1;
			key.mv_data = vkey;
		}

		if (fixedsize)
			data.mv_size = fixedsize;
		else
			data.mv_size = 0;
		data.mv_data = NULL;

		rc = mdb_cursor_get(cursor, &key, &data, MDB_SET_KEY);
		KifVectorString* kvect = (KifVectorString*)Selectvectorstring(this, contextualpattern);
		if (rc)
			return kvect;

		kvect->values.push_back((char*)data.mv_data);

		string k;
		long vk;
		while (!mdb_cursor_get(cursor, &key, &data, MDB_NEXT)) {
			if (integerkey) {
				vk = atol((char*)key.mv_data);
				if (v == vk)
					break;
			}
			else {
				k = (char*)key.mv_data;
				if (k != akey)
					break;
			}
			kvect->values.push_back((char*)data.mv_data);
		}

		mdb_cursor_close(cursor);
		mdb_txn_abort(txn);
		cursor = NULL;
		txn = NULL;

		if (vkey != NULL)
			delete[] vkey;
		return kvect;
	}
	//----------------------------------------------------------

	KifElement* MethodOpen(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//Three parameters: first directory, then size, then environment flags
		if (env != NULL)
			return kifcode->Returnerror("LMD(001): Database already opened");

		directory = callfunc->Evaluate(0, domain)->String();
		if (isDirectory(directory) == false) {
			directory = "LMD(010): This is not a directory: " + directory;
			return kifcode->Returnerror(directory);
		}

		size = callfunc->Evaluate(1, domain)->Long();

		long nbdbs = callfunc->Evaluate(2, domain)->Integer();
		long envflags = callfunc->Evaluate(3, domain)->Integer();
		fixedsize = 0;
		if (callfunc->Size() == 5)
			fixedsize = callfunc->Evaluate(4, domain)->Integer();
		else {
			if ((envflags&MDB_DUPFIXED) == MDB_DUPFIXED || (envflags&MDB_FIXEDMAP) == MDB_FIXEDMAP)
				return kifcode->Returnerror("LMD(019): You need to provide as a 5th parameter the fixed size, that will be used in the database.");

			if ((MDB_INTEGERKEY&envflags) == MDB_INTEGERKEY)
				integerkey = true;
		}


		int rc = mdb_env_create(&env);
		if (rc) {
			string mess = "LMD(004): Error in opening database";
			if (kiflmdperrors.find(rc) != kiflmdperrors.end())
				mess += ": " + kiflmdperrors[rc];
			else {
				ostringstream convert;
				char* msg = mdb_strerror(rc);
				if (msg != NULL)
					convert << ": " << msg << "(" << rc << ")";
				else
					convert << " (" << rc << ")";
				mess += convert.str();
			}

			return kifcode->Returnerror(mess);
		}
		rc = mdb_env_set_mapsize(env, size);
		if (rc) {
			string mess = "LMD(004): Error in opening database";
			if (kiflmdperrors.find(rc) != kiflmdperrors.end())
				mess += ": " + kiflmdperrors[rc];
			else {
				ostringstream convert;
				char* msg = mdb_strerror(rc);
				if (msg != NULL)
					convert << ": " << msg << "(" << rc << ")";
				else
					convert << " (" << rc << ")";
				mess += convert.str();
			}

			return kifcode->Returnerror(mess);
		}
		if (nbdbs)
			rc = mdb_env_set_maxdbs(env, nbdbs);
		rc = mdb_env_open(env, STR(directory), envflags, 0664);

		if (rc) {
			string mess = "LMD(004): Error in opening database";
			if (kiflmdperrors.find(rc) != kiflmdperrors.end())
				mess += ": " + kiflmdperrors[rc];
			else {
				ostringstream convert;
				char* msg = mdb_strerror(rc);
				if (msg != NULL)
					convert << ": " << msg << "(" << rc << ")";
				else
					convert << " (" << rc << ")";
				mess += convert.str();
			}

			return kifcode->Returnerror(mess);
		}
		return kifTRUE;
	}

	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (env == NULL)
			return kifcode->Returnerror("LMD(005): Database has not been opened yet");

		string tablename = callfunc->Evaluate(0, domain)->String();
		long openflags = callfunc->Evaluate(1, domain)->Integer();
		int rc = mdb_txn_begin(env, NULL, 0, &txn);
		if (tablename == "")
			rc = mdb_open(txn, NULL, openflags, &dbi);
		else {
			rc = mdb_open(txn, STR(tablename), openflags, &dbi);
			databases[tablename] = dbi;
		}

		return kifTRUE;
	}

	KifElement* MethodSelectdatabase(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (env == NULL)
			return kifcode->Returnerror("LMD(005): Database has not been opened yet");

		string tablename = callfunc->Evaluate(0, domain)->String();
		if (databases.find(tablename) == databases.end())
			return kifcode->Returnerror("LMD(024): Unknown table");

		dbi = databases[tablename];
		return kifTRUE;
	}


	KifElement* MethodGet(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (env == NULL)
			return kifcode->Returnerror("LMD(005): Database has not been opened yet");

		KifElement* kkey = callfunc->Evaluate(0, domain);
		MDB_cursor_op cursorflagcall = (MDB_cursor_op)callfunc->Evaluate(1, domain)->Integer();

		if (txn == NULL)
			mdb_txn_begin(env, NULL, 0, &txn);

		int rc = mdb_cursor_open(txn, dbi, &cursor);
		if (rc) {
			string mess = "LMD(013): Error in GET";
			if (kiflmdperrors.find(rc) != kiflmdperrors.end())
				mess += ": " + kiflmdperrors[rc];
			else {
				ostringstream convert;
				char* msg = mdb_strerror(rc);
				if (msg != NULL)
					convert << ": " << msg << "(" << rc << ")";
				else
					convert << " (" << rc << ")";
				mess += convert.str();
			}

			return kifcode->Returnerror(mess);
		}

		char* vkey = NULL;
		long v;
		if (integerkey) {
			key.mv_size = sizeof(long);
			v = kkey->Integer();
			key.mv_data = &v;
		}
		else {
			string akey = kkey->String();
			int msz = max(akey.size() + 1, fixedsize);
			vkey = new char[msz];
			strcpy(vkey, STR(akey));
			if (fixedsize)
				key.mv_size = fixedsize;
			else
				key.mv_size = akey.size() + 1;
			key.mv_data = vkey;
		}

		if (fixedsize)
			data.mv_size = fixedsize;
		else
			data.mv_size = 0;
		data.mv_data = NULL;

		rc = mdb_cursor_get(cursor, &key, &data, cursorflagcall);
		if (vkey != NULL)
			delete[] vkey;

		if (rc) {
			string mess = "LMD(013): Error in GET";
			if (kiflmdperrors.find(rc) != kiflmdperrors.end())
				mess += ": " + kiflmdperrors[rc];
			else {
				ostringstream convert;
				char* msg = mdb_strerror(rc);
				if (msg != NULL)
					convert << ": " << msg << "(" << rc << ")";
				else
					convert << " (" << rc << ")";
				mess += convert.str();
			}

			return kifcode->Returnerror(mess);
		}

		return kifTRUE;
	}

	KifElement* MethodSetCursorMode(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (env == NULL)
			return kifcode->Returnerror("LMD(005): Database has not been opened yet");

		cursorflag = (MDB_cursor_op)callfunc->Evaluate(0, domain)->Integer();

		return kifTRUE;
	}

	KifElement* MethodSelfCommit(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (env == NULL)
			return kifcode->Returnerror("LMD(005): Database has not been opened yet");
		selfcommit = callfunc->Evaluate(0, domain)->Boolean();
		return kifTRUE;
	}

	KifElement* MethodBegin(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (env == NULL)
			return kifcode->Returnerror("LMD(005): Database has not been opened yet");

		if (txn != NULL)
			return kifTRUE;

		int rc = 0;
		rc = mdb_txn_begin(env, NULL, 0, &txn);
		if (rc) {
			string mess = "LMD(006): Error in BEGIN";
			if (kiflmdperrors.find(rc) != kiflmdperrors.end())
				mess += ": " + kiflmdperrors[rc];
			else {
				ostringstream convert;
				char* msg = mdb_strerror(rc);
				if (msg != NULL)
					convert << ": " << msg << "(" << rc << ")";
				else
					convert << " (" << rc << ")";
				mess += convert.str();
			}

			return kifcode->Returnerror(mess);
		}
		rc = mdb_open(txn, NULL, 0, &dbi);
		if (rc) {
			string mess = "LMD(006): Error in BEGIN";
			if (kiflmdperrors.find(rc) != kiflmdperrors.end())
				mess += ": " + kiflmdperrors[rc];
			else {
				ostringstream convert;
				char* msg = mdb_strerror(rc);
				if (msg != NULL)
					convert << ": " << msg << "(" << rc << ")";
				else
					convert << " (" << rc << ")";
				mess += convert.str();
			}

			return kifcode->Returnerror(mess);
		}

		return kifTRUE;
	}

	KifElement* MethodCommit(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (env == NULL)
			return kifcode->Returnerror("LMD(005): Database has not been opened yet");
		if (txn == NULL)
			return kifcode->Returnerror("LMD(011): Database has not been prepared");

		int rc = 0;
		rc = mdb_txn_commit(txn);
		if (rc) {
			string mess = "LMD(006): Error in COMMIT";
			if (kiflmdperrors.find(rc) != kiflmdperrors.end())
				mess += ": " + kiflmdperrors[rc];
			else {
				ostringstream convert;
				char* msg = mdb_strerror(rc);
				if (msg != NULL)
					convert << ": " << msg << "(" << rc << ")";
				else
					convert << " (" << rc << ")";
				mess += convert.str();
			}
			return kifcode->Returnerror(mess);
		}
		txn = NULL;
		return kifTRUE;
	}

	KifElement* MethodPut(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (env == NULL)
			return kifcode->Returnerror("LMD(005): Database has not been opened yet");
		//Need key/value
		KifElement* kkey = callfunc->Evaluate(0, domain);
		string avalue = callfunc->Evaluate(1, domain)->String();

		long flags = callfunc->Evaluate(2, domain)->Integer();

		char* vkey = NULL;
		long v;
		int msz;
		if (integerkey) {
			key.mv_size = sizeof(long);
			v = kkey->Integer();
			key.mv_data = &v;
		}
		else {
			string akey = kkey->String();
			msz = max(akey.size() + 1, fixedsize);
			vkey = new char[msz];
			strcpy(vkey, STR(akey));
			if (fixedsize)
				key.mv_size = fixedsize;
			else
				key.mv_size = akey.size() + 1;
			key.mv_data = vkey;
		}

		if (fixedsize)
			data.mv_size = fixedsize;
		else
			data.mv_size = avalue.size() + 1;

		msz = max(avalue.size() + 1, fixedsize);
		char* vvalue = new char[msz];
		strcpy(vvalue, STR(avalue));
		data.mv_data = vvalue;
		int rc = mdb_put(txn, dbi, &key, &data, flags);

		delete[] vvalue;
		if (vkey != NULL)
			delete[] vkey;

		if (rc) {
			string mess = "LMD(018): Error in PUT";
			if (kiflmdperrors.find(rc) != kiflmdperrors.end())
				mess += ": " + kiflmdperrors[rc];
			else {
				ostringstream convert;
				char* msg = mdb_strerror(rc);
				if (msg != NULL)
					convert << ": " << msg << "(" << rc << ")";
				else
					convert << " (" << rc << ")";
				mess += convert.str();
			}

			return kifcode->Returnerror(mess);
		}

		return kifTRUE;
	}

	KifElement* MethodDelete(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (env == NULL)
			return kifcode->Returnerror("LMD(005): Database has not been opened yet");
		//Need key/value
		KifElement* kkey = callfunc->Evaluate(0, domain);
		char* vkey = NULL;
		long v;
		if (integerkey) {
			key.mv_size = sizeof(long);
			v = kkey->Integer();
			key.mv_data = &v;
		}
		else {
			string akey = kkey->String();
			int msz = max(akey.size() + 1, fixedsize);
			vkey = new char[msz];
			strcpy(vkey, STR(akey));
			if (fixedsize)
				key.mv_size = fixedsize;
			else
				key.mv_size = akey.size() + 1;
			key.mv_data = vkey;
		}

		int rc = 0;
		if (txn == NULL)
			rc = mdb_txn_begin(env, NULL, 0, &txn);
		rc = mdb_del(txn, dbi, &key, NULL);
		if (rc)
			mdb_txn_abort(txn);
		else
			rc = mdb_txn_commit(txn);

		if (vkey != NULL)
			delete[] vkey;
		return kifcode->Provideinteger(rc);
	}

	KifElement* MethodClose(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (env == NULL)
			return kifcode->Returnerror("LMD(005): Database has not been opened yet");
		mdb_close(env, dbi);

		mdb_txn_abort(txn);
		mdb_env_close(env);
		env = NULL;
		return kifTRUE;
	}

	~kiflmdb() {
		if (env != NULL) {
			mdb_close(env, dbi);

			mdb_txn_abort(txn);
			mdb_env_close(env);
		}
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

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//How to interprete your class according to the basic following types:
	string String() {
		return directory;
	}

	long Integer() {
		return size;
	}

	double Float() { return size; }
	bool Boolean() {
		if (env == NULL)
			return false;
		return true;
	}

};

//We need to declare once again our local definitions.
map<short, KifCallMethod> kiflmdb::kifexportedmethods;
map<short, LMDBMethod>  kiflmdb::linkedmethods;
map<string, string> kiflmdb::infomethods;
hmap<short, bool> kiflmdb::methodassigned;

KifElement* KifIteratorLMDB::IteratorKey() {
	if (key.mv_size == 0)
		return kifcode->Providestringraw("");
	return kifcode->Providestringraw((char*)key.mv_data);
}

KifElement* KifIteratorLMDB::IteratorValue() {
	if (data.mv_size == 0)
		return kifcode->Providestringraw("");
	return kifcode->Providestringraw((char*)data.mv_data);
}

void KifIteratorLMDB::Next() {
	endoftransaction = mdb_cursor_get(cursor, &key, &data, cursorflag);
}

KifElement* KifIteratorLMDB::End() {
	if (endoftransaction != 0) {
		mdb_cursor_close(cursor);
		mdb_txn_abort(txn);
		if (start == false) {
			value->txn = NULL;
			value->cursor = NULL;
		}
		cursor = NULL;
		txn = NULL;
		return kifTRUE;
	}

	if (cursor == NULL && txn == NULL) {
		txn = value->txn;
		cursor = value->cursor;
		if (cursor == NULL) {
			kifcode->Returnerror("LMD(012): Wrong access to iterator");
			return kifRAISEERROR;
		}
		key = value->key;
		data = value->data;
	}

	return kifFALSE;
}

KifElement* KifIteratorLMDB::Begin() {
	start = true;
	int rc = mdb_txn_begin(value->env, NULL, 1, &txn);
	rc = mdb_cursor_open(txn, value->dbi, &cursor);
	if (rc)
		return kifFALSE;
	endoftransaction = mdb_cursor_get(cursor, &key, &data, cursorflag);
	return kifTRUE;
}

KifIteratorLMDB::~KifIteratorLMDB() {
	if (start == true) {
		if (cursor != NULL) {
			mdb_cursor_close(cursor);
			mdb_txn_abort(txn);
		}
	}
}

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekiflmdbElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	kiflmdb* local = new kiflmdb(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name, LMDBMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	kiflmdb::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	kiflmdb::linkedmethods[idname] = func;
	kiflmdb::methodassigned[idname] = true;
	kiflmdb::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kiflmdb(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}
		kifcode->KifInitLib();
		///The new type will be: lmdb. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekiflmdbElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: kiflmdb)
		//Whenever the keyword testage will be found by KiF it will then call CreatekiflmdbElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("lmdb", CreatekiflmdbElement);
		if (!create_variables(kifcode))
			return false;

		if (res == kifVOID)
			return false;
		lmdb_type = res;

		return true;
	}

	Exporting bool kiflmdb_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kiflmdb(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name with a specific method from
		//our new class.
		MethodInitialization("open", &kiflmdb::MethodOpen, P_FOUR | P_FIVE, "open(string pathname,int size,int nbdbs,int flags,int fixedsize): open a database");
		MethodInitialization("create", &kiflmdb::MethodCreate, P_TWO, "create(string table,int flags): create a table");
		MethodInitialization("begin", &kiflmdb::MethodBegin, P_NONE, "begin(): open transaction mode");
		MethodInitialization("commit", &kiflmdb::MethodCommit, P_NONE, "commit(): commit transactions");
		MethodInitialization("selfcommit", &kiflmdb::MethodSelfCommit, P_ONE, "selfcommit(bool): commit transaction is automatically done when using 'var[key]=value'");
		MethodInitialization("put", &kiflmdb::MethodPut, P_THREE, "put(string key,string value,int flags): store a value");
		MethodInitialization("delete", &kiflmdb::MethodDelete, P_ONE, "delete(string key): delete a key");
		MethodInitialization("cursormode", &kiflmdb::MethodSetCursorMode, P_ONE, "cursormode(int cursorflag): set the cursor mode access");
		MethodInitialization("get", &kiflmdb::MethodGet, P_TWO, "get(string key,int cursorflag): Get a value with a key, with the size sz along a cursorflag... Use an iterator to get values, without a call to 'begin'");
		MethodInitialization("close", &kiflmdb::MethodClose, P_NONE, "close(): close a session");
		MethodInitialization("select", &kiflmdb::MethodSelectdatabase, P_ONE, "select(string table): switch to another table");

		kifGlobals->kifAllFunctions[lmdb_type] = kiflmdb::methodassigned;
		kifGlobals->kifAllInformation[lmdb_type] = kiflmdb::infomethods;
		create_variables(kifcode);

		return true;
	}
	Exporting bool kiflmdb_Terminate(KifCode* kifcode) {
		kiflmdb::kifexportedmethods.clear();
		kiflmdb::linkedmethods.clear();
		kiflmdb::methodassigned.clear();
		kiflmdb::infomethods.clear();
		return true;
	}
}





