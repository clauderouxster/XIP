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
filename   : kifsqlite.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language: SQLite implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "kif.h"
#include "kifpredicate.h"
#include "kifversion.h"
#include "sqlite3.h"
#include "conversion.h"
#include "kifcontainer.h"
#include "kifmaps.h"

short KifNewId(string n);
string KifIdString(short n);
string& Trim(string&);
#define STR(x) (char*)x.c_str()

#ifdef WIN32
#define Exporting __declspec(dllexport)
#define sprintf_ss(a,b,c,d,e) sprintf_s(a,b,c,d,e)
#else
#define sprintf_ss(a,b,c,d,e) sprintf(a,c,d,e)
#endif

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType sqlite_type;

//We create a map between our methods, which have been declared in our class below with their names.
class KifSQLite;
//This typedef defines a type "SQLiteMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (KifSQLite::*SQLiteMethod)(KifElement*, KifDomain*, KifCallFunction*, int);

static int callback(void *kifsql, int argc, char **argv, char **azColName);
static int predicatecallback(void *kifsql, int argc, char **argv, char **azColName);
static int countpredicatecallback(void *kifsql, int argc, char **argv, char **azColName);

string Quotting(string& si) {
	string s = "'";
	for (int i = 0; i < si.size(); i++) {
		if (si[i] == '\'')
			s += "'";
		s += si[i];
	}
	s += "'";
	s = conversion_latin_to_utf8(s);
	return s;
}

class KifIteratorSQLite : public KifIteration {
public:
	KifSQLite* value;
	sqlite3_stmt *stmt;
	int returncode;
	vector<string> columnnames;
	int columnCount;
	KifMap* currentrow;
	int nb;
	int garbage_i;

	KifIteratorSQLite(KifCode* klc, KifElement* kp) : KifIteration(klc, kp) {
		currentrow = NULL;
		columnCount = 0;
		nb = -1;
		garbage_i = -1;
	}

	//This method stores as a map a row of results
	void Storevalue();

	//The next methods are the one that should be derived to add iterator capacities to KifSQLite

	KifElement* IteratorKey() {
		return kifcode->Provideinteger(nb);
	}

	KifElement* IteratorValue();
	void Next();
	KifElement* End();
	KifElement* Begin();
};


class KifSQLite : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, SQLiteMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	string command;
	string dbname;
	sqlite3 *db;
	KifVector* vresults;
	string sqlcommand;
	string bindcommand;
	sqlite3_stmt* stmt;
	vector<int> insertstructure;

	KifSQLite(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, sqlite_type) {
		dbname = "";
		db = NULL;
		sqlcommand = "";
		vresults = new KifVector(NULL, NULL);
		vresults->kifcode = kifcode;
		vresults->Setreference();
		stmt = NULL;
	}

	~KifSQLite() {
		if (db != NULL)
			sqlite3_close(db);
		vresults->Clear();
		delete vresults;
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return sqlite_type;
	}

	//How to set the inner value
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		return true;
	}

	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//iterator it=mydb;
	KifElement* Newiterator(bool rev) {
		KifIteratorSQLite* iter = new KifIteratorSQLite(kifcode, NULL);
		iter->reverse = rev;
		//we set the value with the right one
		iter->value = this;
		return iter;
	}


	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifSQLite* kperso = new KifSQLite(kifcode, kp);
		return kperso;
	}


	//Declaration
	//All our methods must have been declared in our kifsqlitemethod variable (which might take any name you want)
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

	KifElement* MethodOpen(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db != NULL)
			return kifcode->Returnerror("SQLite(800): A database has already been opened with this object");

		//the first parameter is the dbname
		KifElement* kelement = callfunc->Evaluatethread(0, domain, idthread);
		dbname = kelement->String();
		int rc = sqlite3_open(STR(dbname), &db);
		if (rc) {
			string message = "SQLite(801): Error opening database='";
			message += dbname;
			message += "' --> ";
			message += sqlite3_errmsg(db);
			return kifcode->Returnerror(message);
		}
		return kifTRUE;
	}

	KifElement* MethodClose(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(802): Cannot close this database");
		sqlite3_close(db);
		db = NULL;
		return kifTRUE;
	}

	KifElement* MethodRun(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(803): Cannot use this database");
		KifElement* kcommand = callfunc->Evaluatethread(0, domain, idthread);
		sqlcommand = kcommand->String();

		//If we are dealing with an iterator, then we simply return our local object, in order
		//to build an iterator. sqlcommand will keep the sql command to pass to the iterator.
		if (contextualpattern != NULL && contextualpattern->type == kifIterator)
			return this;

		char* errmsg;
		vresults->Clear();
		int rc = sqlite3_exec(db, STR(sqlcommand), callback, this, &errmsg);
		if (rc != SQLITE_OK) {
			sqlcommand += "SQLite(811): Execute error=";
			sqlcommand += errmsg;
			return kifcode->Returnerror(sqlcommand);
		}
		return vresults;
	}

	KifElement* MethodExecute(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(803): Cannot use this database");
		KifElement* kcommand = callfunc->Evaluatethread(0, domain, idthread);
		sqlcommand = kcommand->String();

		char* errmsg;
		int rc = sqlite3_exec(db, STR(sqlcommand), NULL, 0, &errmsg);
		if (rc != SQLITE_OK) {
			sqlcommand += "SQLite(811): Execute error=";
			sqlcommand += errmsg;
			return kifcode->Returnerror(sqlcommand);
		}
		return kifTRUE;
	}

	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(803): Cannot use this database");
		//A typical call would be:
		// mydb.create("table1","name TEXT","age INTEGER",12);
		command = "create table ";
		//The first parameter is the table name
		KifElement* table = callfunc->Evaluatethread(0, domain, idthread);
		command += table->String();
		command += " (";
		//The next parameters are the rest of the table description
		for (int i = 1; i < callfunc->Size(); i++) {
			table = callfunc->Evaluatethread(i, domain, idthread);
			if (i != 1)
				command += ",";
			command += table->String();
		}
		command += ");";
		char *zErrMsg = NULL;
		int rc = sqlite3_exec(db, STR(command), NULL, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			command += "SQLite(805): Create error=";
			command += zErrMsg;
			sqlite3_free(zErrMsg);
			return kifcode->Returnerror(command);
		}
		if (zErrMsg != NULL)
			sqlite3_free(zErrMsg);
		return kifTRUE;
	}

	KifElement* MethodPredicate(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(810): This database has not been opened yet...");
		string predicatename = callfunc->Evaluatethread(0, domain, idthread)->String();
		int predicatearity = callfunc->Evaluatethread(1, domain, idthread)->Integer();
		stringstream sqlc;
		sqlc << "CREATE TABLE ";
		sqlc << predicatename << "_" << predicatearity << "(";
		//The next parameters are the rest of the table description
		int i;
		for (i = 0; i < predicatearity; i++) {
			if (i)
				sqlc << ",";
			sqlc << "P" << i << " TEXT";
		}
		if (predicatearity != 0) {
			sqlc << ",PRIMARY KEY(";
			for (i = 0; i<predicatearity; i++) {
				if (i)
					sqlc << ",";
				sqlc << "P" << i;
			}
			sqlc << ")";
		}
		sqlc << ");";

		if (predicatearity>1)
			sqlc << " CREATE INDEX i" << predicatename << "_" << predicatearity << " ON " << predicatename << "_" << predicatearity << "(P0);";

		char *zErrMsg = NULL;
		if (sqlite3_exec(db, STR(sqlc.str()), NULL, 0, &zErrMsg) != SQLITE_OK) {
			string message = "SQLite(830): Error while creating the persistence table: ";
			message += zErrMsg;
			return kifcode->Returnerror(message);
		}
		return kifTRUE;
	}

	KifElement* MethodStorepredicate(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(810): This database has not been opened yet...");

		KifPredicateDatabase* current = (KifPredicateDatabase*)contextualpattern;
		stringstream sqlc;
		KifElement* e;
		sqlc << "insert into " << kifGlobals->KifIdString(current->name) << "_" << current->parameters.size() << " values (";
		string val;
		for (int i = 0; i < current->parameters.size(); i++) {
			if (current->parameters[i]->unifiedDeclaration(domain) == false)
				return kifFALSE;
			if (i)
				sqlc << ",";
			e = current->parameters[i]->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
			val = e->String();
			val = s_replacestrings(val, "\"", "\\\"");
			if (e->aString())
				sqlc << "\"@" << val << "\"";
			else
				sqlc << "\"" << val << "\"";
		}
		sqlc << ");";

		char *zErrMsg = NULL;
		int rc = sqlite3_exec(db, STR(sqlc.str()), NULL, NULL, &zErrMsg);
		if (rc != SQLITE_OK) {
			string message = "SQLite(837): Error while storing a predicate: ";
			message += zErrMsg;
			return kifcode->Returnerror(message);
		}

		return kifTRUE;
	}

	KifElement* MethodRemovepredicate(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(810): This database has not been opened yet...");

		KifPredicateDatabase* current = (KifPredicateDatabase*)contextualpattern;
		stringstream sqlc;
		KifElement* e;
		bool first = true;
		string val;
		sqlc << "delete from " << kifGlobals->KifIdString(current->name) << "_" << current->parameters.size();
		for (int i = 0; i < current->parameters.size(); i++) {
			if (current->parameters[i]->unifiedDeclaration(domain) == false)
				continue;

			if (!first)
				sqlc << " AND P" << i << "=";
			else
				sqlc << " where (P" << i << "=";
			first = false;
			e = current->parameters[i]->Execute(kifNULL, domain);
			val = e->String();
			val = s_replacestrings(val, "\"", "\\\"");
			if (e->aString())
				sqlc << "\"@" << val << "\"";
			else
				sqlc << "\"" << val << "\"";
		}

		if (!first)
			sqlc << ");";
		else
			sqlc << ";";

		char *zErrMsg = NULL;
		int rc = sqlite3_exec(db, STR(sqlc.str()), NULL, NULL, &zErrMsg);
		if (rc != SQLITE_OK) {
			string message = "SQLite(839): Error while deleting a predicate: ";
			message += zErrMsg;
			return kifcode->Returnerror(message);
		}

		return kifTRUE;
	}
	KifElement* MethodCountpredicate(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(810): This database has not been opened yet...");

		KifInstructionEvaluate* current = (KifInstructionEvaluate*)contextualpattern;
		//First we need to check whether any of the parameters of current are unified...
		stringstream sqlc;
		KifElement* e;
		bool first = true;
		string val;
		sqlc << "select count(P0) from " << kifGlobals->KifIdString(current->headrule->name) << "_" << current->headrule->parameters.size();
		for (int i = 0; i < current->headrule->parameters.size(); i++) {
			if (current->headrule->parameters[i]->unifiedDeclaration(domain)) {
				e = current->headrule->parameters[i]->Execute(kifNULL, domain);
				if (e == kifUNIVERSAL)
					continue;

				if (!first)
					sqlc << " AND ";
				else
					sqlc << " where (";
				sqlc << "P" << i << "=";
				first = false;
				val = e->String();
				val = s_replacestrings(val, "\"", "\\\"");
				if (e->aString())
					sqlc << "\"@" << val << "\"";
				else
					sqlc << "\"" << val << "\"";
			}
		}
		if (!first)
			sqlc << ");";
		else
			sqlc << ";";

		char *zErrMsg = NULL;
		int rc = sqlite3_exec(db, STR(sqlc.str()), countpredicatecallback, current, &zErrMsg);
		if (rc != SQLITE_OK) {
			string message = "SQLite(837): Error while testing a predicate: ";
			message += zErrMsg;
			return kifcode->Returnerror(message);
		}
		return kifTRUE;
	}

	KifElement* MethodReadpredicate(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(810): This database has not been opened yet...");

		KifInstructionEvaluate* current = (KifInstructionEvaluate*)contextualpattern;
		//First we need to check whether any of the parameters of current are unified...
		stringstream sqlc;
		KifElement* e;
		bool first = true;
		string val;
		sqlc << "select * from " << kifGlobals->KifIdString(current->headrule->name) << "_" << current->headrule->parameters.size();
		for (int i = 0; i < current->headrule->parameters.size(); i++) {
			if (current->headrule->parameters[i]->unifiedDeclaration(domain)) {
				e = current->headrule->parameters[i]->Execute(kifNULL, domain);
				if (e == kifUNIVERSAL)
					continue;

				if (!first)
					sqlc << " AND ";
				else
					sqlc << " where (";
				sqlc << "P" << i << "=";
				first = false;
				val = e->String();
				val = s_replacestrings(val, "\"", "\\\"");
				if (e->aString())
					sqlc << "\"@" << val << "\"";
				else
					sqlc << "\"" << val << "\"";
			}
		}
		if (!first)
			sqlc << ");";
		else
			sqlc << ";";

		char *zErrMsg = NULL;
		int rc = sqlite3_exec(db, STR(sqlc.str()), predicatecallback, current, &zErrMsg);
		if (rc != SQLITE_OK) {
			string message = "SQLite(838): Error while reading a predicate: ";
			message += zErrMsg;
			return kifcode->Returnerror(message);
		}
		return kifTRUE;
	}

	KifElement* MethodInsertThroughBind(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(803): Cannot use this database");
		if (callfunc->Size() < 2)
			return kifcode->Returnerror("SQLite(808): Missing parameters in 'INSERTBIND'");

		char *zErrMsg = NULL;
		int pos, rc;
		if (bindcommand == "ok") {
			KifElement* table = callfunc->Evaluatethread(0, domain, idthread);
			sqlcommand = "PRAGMA table_info(";
			sqlcommand += table->String();
			sqlcommand += ")";
			vresults->Clear();
			rc = sqlite3_exec(db, STR(sqlcommand), callback, this, &zErrMsg);
			if (rc != SQLITE_OK)
				return kifcode->Returnerror("SQLite(897): Unknown table");

			insertstructure.clear();
			bindcommand = "INSERT INTO ";
			bindcommand += table->String();
			bindcommand += " VALUES (";
			for (pos = 0; pos < vresults->values.size(); pos++) {
				command = ((KifMapStringString*)vresults->values[pos])->values["type"];
				if (command == "TEXT" || command == "BLOB" || command == "VARCHAR")
					insertstructure.push_back(SQLITE_TEXT);
				else
				if (command == "INTEGER")
					insertstructure.push_back(SQLITE_INTEGER);
				else
				if (command == "REAL" || command == "FLOAT")
					insertstructure.push_back(SQLITE_FLOAT);
				else
					insertstructure.push_back(SQLITE_TEXT);
				if (pos != 0)
					bindcommand += ",";
				bindcommand += "?";
			}

			bindcommand += ")";
			rc = sqlite3_prepare_v2(db, STR(bindcommand), bindcommand.size(), &stmt, NULL);
			if (rc != SQLITE_OK)
				return kifcode->Returnerror("SQLite(899): Wrong value list");

		}

		if (stmt == NULL)
			return kifcode->Returnerror("SQLite(893): You need to do a BEGIN first");

		KifElement* kelement;
		for (pos = 1; pos <= insertstructure.size(); pos++) {
			kelement = callfunc->Evaluatethread(pos, domain, idthread);
			switch (insertstructure[pos - 1]) {
			case SQLITE_TEXT:
				command = kelement->String();
				rc = sqlite3_bind_text(stmt, pos, STR(command), command.size(), SQLITE_STATIC);
				break;
			case SQLITE_INTEGER:
				rc = sqlite3_bind_int(stmt, pos, kelement->Integer());
				break;
			case SQLITE_FLOAT:
				rc = sqlite3_bind_double(stmt, pos, kelement->Float());
			}

			if (rc != SQLITE_OK) {
				command = "SQLite(895): Wrong value for:";
				command += insertstructure[pos];
				return kifcode->Returnerror(command);
			}
		}

		bool failure = false;
		if (sqlite3_step(stmt) != SQLITE_DONE)
			failure = true;
		sqlite3_reset(stmt);

		if (failure)
			return kifcode->Returnerror("SQLite(812): Transaction failed!");

		return kifTRUE;
	}

	KifElement* MethodBegin(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(803): Cannot use this database");
		string mode;
		if (callfunc->Size() != 0) {
			mode = callfunc->Evaluatethread(0, domain, idthread)->String();
			Trim(mode);
		}
		//A typical call would be:
		// mydb.create("table1","name TEXT","age INTEGER",12);
		command = "BEGIN";
		if (mode != "") {
			command += " ";
			command += mode;
		}
		command += " TRANSACTION;";
		bindcommand = "ok";
		stmt = NULL;
		//command+=" TRANSACTION;";
		char *zErrMsg = NULL;
		int rc = sqlite3_exec(db, STR(command), NULL, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			command += "SQLite(805): 'BEGIN' error=";
			command += zErrMsg;
			sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
			sqlite3_free(zErrMsg);
			return kifcode->Returnerror(command);
		}
		if (zErrMsg != NULL)
			sqlite3_free(zErrMsg);

		return kifTRUE;
	}


	KifElement* MethodCommit(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(803): Cannot use this database");
		//A typical call would be:
		// mydb.create("table1","name TEXT","age INTEGER",12);
		command = "COMMIT;";
		char *zErrMsg = NULL;
		int rc = sqlite3_exec(db, STR(command), NULL, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			command += "SQLite(807): 'COMMIT' error=";
			command += zErrMsg;
			sqlite3_free(zErrMsg);
			return kifcode->Returnerror(command);
		}
		bindcommand = "";
		if (stmt != NULL)
			sqlite3_finalize(stmt);
		stmt = NULL;
		if (zErrMsg != NULL)
			sqlite3_free(zErrMsg);

		return kifTRUE;
	}

	KifElement* MethodInsert(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(803): Cannot use this database");
		if (callfunc->Size() == 0 || (callfunc->Size() % 2) == 0)
			return kifcode->Returnerror("SQLite(808): Missing parameters in 'INSERT'");

		string lacommande = "insert into ";
		//A typical call would be:
		// mydb.insert("table1","name","toto","age",12);
		//The first parameter is the table name
		KifElement* table = callfunc->Evaluatethread(0, domain, idthread);
		lacommande += table->String();
		lacommande += " (";
		//One parameter our of two is column name
		for (int i = 1; i < callfunc->Size(); i += 2) {
			table = callfunc->Evaluatethread(i, domain, idthread);
			if (i != 1)
				lacommande += ",";
			lacommande += table->String();
		}
		lacommande += ") values (";
		string s;
		//One parameter out of two is a value associated to a column name
		for (int i = 2; i < callfunc->Size(); i += 2) {
			table = callfunc->Evaluatethread(i, domain, idthread);
			if (i != 2)
				lacommande += ",";
			s = table->String();
			s = Quotting(s);
			lacommande += s;
		}
		lacommande += ");";
		char *zErrMsg = NULL;

		int rc = sqlite3_exec(db, STR(lacommande), NULL, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			lacommande += "SQLite(809): Create error=";
			lacommande += zErrMsg;
			sqlite3_free(zErrMsg);
			return kifcode->Returnerror(lacommande);
		}
		if (zErrMsg != NULL)
			sqlite3_free(zErrMsg);
		return kifTRUE;
	}

	KifElement* MethodMemory(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (db == NULL)
			return kifcode->Returnerror("SQLite(803): Cannot use this database");
		long l = sqlite3_memory_used();
		return kifcode->Provideinteger(l);
	}

	//ExecuteMethod must be implemented in order to execute our new KiF methods. 
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
		if (db != NULL)
			sqlite3_close(db);
	}

	void Clear() {
		//Your reset code here
		if (db != NULL)
			sqlite3_close(db);
	}

	//How to interprete your class according to the basic following types:
	string String() {
		return dbname;
	}

	long Integer() {
		return sqlite3_memory_used();
	}

	double Float() { return 0; }
	bool Boolean() {
		if (db == NULL)
			return false;
		return true;
	}

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> KifSQLite::kifexportedmethods;
map<short, SQLiteMethod>  KifSQLite::linkedmethods;
map<string, string> KifSQLite::infomethods;
hmap<short, bool> KifSQLite::methodassigned;

static int callback(void *kifsql, int argc, char **argv, char **azColName){
	int i;
	//We use our first parameter as the place where to store our stuff
	KifSQLite* ksql = (KifSQLite*)kifsql;
	KifMapStringString* kmap = new KifMapStringString(ksql->kifcode, NULL);
	for (i = 0; i < argc; i++) {
		if (argv[i] == NULL)
			kmap->values[azColName[i]] = "";
		else
			kmap->values[azColName[i]] = argv[i];
	}
	ksql->vresults->Push(kmap);
	return 0;
}

static int predicatecallback(void *kifsql, int argc, char **argv, char **azColName) {
	int i;
	//We use our first parameter as the place where to store our stuff
	KifInstructionEvaluate* current = (KifInstructionEvaluate*)kifsql;
	KifPredicateDatabase* kpd = (KifPredicateDatabase*)current->head;
	KifSQLite* ksql = (KifSQLite*)kpd->database;
	KifPredicate* kpred = new KifPredicate(current->kifcode, NULL, kpd->name);
	KifElement* e;
	string s;
	for (i = 0; i < argc; i++) {
		if (argv[i] == NULL)
			kpred->parameters.push_back(kifNULL);
		else {
			s = argv[i];
			if (argv[i][0] == '@') {
				s = s.substr(1, s.size() - 1);
				kpred->parameters.push_back(current->kifcode->Providestring(s));
			}
			else {
				e = current->kifcode->EvaluatePredicateParameter((KifDomain*)current->value, s);
				kpred->parameters.push_back(e);
			}
		}
	}

	kpred->Setreference();
	current->results.push_back(kpred);
	return 0;
}

static int countpredicatecallback(void *kifsql, int argc, char **argv, char **azColName) {
	int i;
	//We use our first parameter as the place where to store our stuff
	KifInstructionEvaluate* current = (KifInstructionEvaluate*)kifsql;
	KifPredicateDatabase* kpd = (KifPredicateDatabase*)current->head;
	KifSQLite* ksql = (KifSQLite*)kpd->database;

	KifString* kstr = current->kifcode->Providestringraw("");
	for (i = 0; i < argc; i++) {
		if (argv[i] != NULL)
			kstr->value = argv[i];
	}

	kstr->Putpopped(1);
	current->value = kstr;
	return 0;
}


void KifIteratorSQLite::Storevalue() {
	//We can then relieve the previous element	
	returncode = sqlite3_step(stmt);
	if (currentrow != NULL) {
		currentrow->Resetreference();
		currentrow = NULL;
	}

	if (returncode != SQLITE_ROW)
		return;

	currentrow = new KifMap(kifcode, NULL);
	currentrow->Setreference();
	KifInteger* kint;
	KifFloat* kfloat;
	KifString* ktext;
	string txt;

	for (int i = 0; i < columnCount; i++) {

		switch (sqlite3_column_type(stmt, i)) {
		case SQLITE_INTEGER:
			kint = kifcode->Provideinteger(sqlite3_column_int(stmt, i));
			currentrow->Push(columnnames[i], kint);
			break;
		case SQLITE_BLOB:
		case SQLITE_TEXT:
			txt = (char*)sqlite3_column_text(stmt, i);
			ktext = kifcode->Providestring(txt);
			currentrow->Push(columnnames[i], ktext);
			break;
		case SQLITE_FLOAT:
			kfloat = kifcode->Providefloat(sqlite3_column_double(stmt, i));
			currentrow->Push(columnnames[i], kfloat);
			break;
		default:
			currentrow->Push(columnnames[i], kifNULL);
		}
	}
	nb++;
}

KifElement* KifIteratorSQLite::IteratorValue() {
	if (currentrow == NULL)
		return kifNULL;
	return currentrow;
}

void KifIteratorSQLite::Next() {
	Storevalue();
}

KifElement* KifIteratorSQLite::End() {
	if (returncode != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		if (currentrow != NULL)
			currentrow->Resetreference();
		return kifTRUE;
	}
	return kifFALSE;
}

KifElement* KifIteratorSQLite::Begin() {
	nb = -1;
	int rc = sqlite3_prepare_v2(value->db, STR(value->sqlcommand), -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		string message = "Wrong statement: ";
		message += value->sqlcommand;
		return kifcode->Returnerror(message);
	}
	//we get the number of columns matching the sql command
	columnCount = sqlite3_column_count(stmt);

	//we then store their names
	char* name;
	char supername[100];
	for (int i = 0; i < columnCount; i++) {
		name = (char*)sqlite3_column_name(stmt, i);
		if (name == NULL) {
			sprintf_s(supername, 100, "column%d", i);
			columnnames.push_back(supername);
		}
		else
			columnnames.push_back(name);
	}

	Storevalue();
	return kifTRUE;
}

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifsqliteElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	KifSQLite* local = new KifSQLite(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name, SQLiteMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	KifSQLite::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	KifSQLite::linkedmethods[idname] = func;
	KifSQLite::methodassigned[idname] = true;
	KifSQLite::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifsqlite(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}
#ifndef KIF_RAW
		cerr << "SQLite is a trademark of Hipp, Wyrick & Company, Inc." << endl;
#endif
		kifcode->KifInitLib();
		///The new type will be: sqlite. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifsqliteElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: KifSQLite)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifsqliteElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("sqlite", CreatekifsqliteElement);
		if (res == kifVOID)
			return false;
		sqlite_type = res;

		return true;
	}


	Exporting bool kifsqlite_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifsqlite(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name with a specific method from
		//our new class.
		MethodInitialization("open", &KifSQLite::MethodOpen, P_ONE, "open(string pathname): open a database");
		MethodInitialization("predicate", &KifSQLite::MethodPredicate, P_TWO, "predicate(string name,int arity): declare a predicate of arity 'arity' in the database");
		MethodInitialization("%readpredicate", &KifSQLite::MethodReadpredicate, P_NONE, "readpredicate(): read a predicate from the database");
		MethodInitialization("%countpredicate", &KifSQLite::MethodCountpredicate, P_NONE, "countpredicate(): test if a predicate is in the database");
		MethodInitialization("%storepredicate", &KifSQLite::MethodStorepredicate, P_NONE, "storepredicate(): store a predicate in the database");
		MethodInitialization("%removepredicate", &KifSQLite::MethodRemovepredicate, P_NONE, "removepredicate(): remove a predicate from the database");

		MethodInitialization("close", &KifSQLite::MethodClose, P_NONE, "close(): close a database");
		MethodInitialization("create", &KifSQLite::MethodCreate, 65534, "create(x1,x2,x3): create a table in a database, with the arguments x1,x2,x3.\rEx. mydb.create('table1','nom TEXT PRIMARY KEY','age INTEGER');");
		MethodInitialization("insert", &KifSQLite::MethodInsert, 65534, "insert(string table,string column,int value,...): insert a line in a table.\rEx. mydb.insert('table1','nom',nm,'age',i);");
		MethodInitialization("insertbind", &KifSQLite::MethodInsertThroughBind, 65534, "insertbind(string table,int v1,...): insert a line in a table through a begin/commit mode.\rEx. mydb.insert('table1',nm,i);");
		MethodInitialization("run", &KifSQLite::MethodRun, P_ONE, "run(string sqlcommand): execute a sql command. If the input variable is a vector, then all possible values will be stored in it. It the input variable is an iterator, then it is possible to iterate on the results of the sql command. Each result is a map, where each key is a column name.");
		MethodInitialization("execute", &KifSQLite::MethodExecute, P_ONE, "execute(string sqlcommand): execute a raw sql command.");
		MethodInitialization("begin", &KifSQLite::MethodBegin, P_NONE | P_ONE, "begin(string mode): to enter the commit mode: default mode is DEFERRED, other modes are: IMMEDIATE and EXCLUSIVE");
		MethodInitialization("commit", &KifSQLite::MethodCommit, P_NONE, "commit(): the SQL command are then processed. It should finish a series of commands initiated with a begin");
		MethodInitialization("memory", &KifSQLite::MethodMemory, P_NONE, "memory(): return how much memory SQLite uses");

		kifGlobals->kifAllFunctions[sqlite_type] = KifSQLite::methodassigned;
		kifGlobals->kifAllInformation[sqlite_type] = KifSQLite::infomethods;

		return true;
	}
	Exporting bool kifsqlite_Terminate(KifCode* kifcode) {
		KifSQLite::kifexportedmethods.clear();
		KifSQLite::linkedmethods.clear();
		KifSQLite::methodassigned.clear();
		KifSQLite::infomethods.clear();
		return true;
	}
}

