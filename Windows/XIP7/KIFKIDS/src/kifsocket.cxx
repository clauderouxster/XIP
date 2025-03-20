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
filename   : kifsocket.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language: Soket implementation
Programmer : Claude ROUX
Reviewer   :
*/

//Use KIF_DLL only in Windows, if you link to the KiF DLL
//#ifndef KIF_DLL
//#define KIF_DLL
//#endif

#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"

#ifdef UNIX
#include <unistd.h>
#endif

#ifdef WIN32
#define closesock closesocket
#define readsock(x,y,z) recv(x,y,z,0)
#define writesock(x,y,z) send(x,y,z,0)
WSADATA WSAData;
#else
#include <sys/utsname.h>        /* for uname() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#define SOCKET_ERROR -1
#define closesock close
#define readsock read
#define writesock write
#endif

//static kifmutex general;
//static kifmutex waiting;

//Replace kifsocket by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType kifsocket_type;
static KifType remote_type;

#define STR(x) (char*)x.c_str()
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifkifsocket;
//This typedef defines a type "kifsocketMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifkifsocket::*kifsocketMethod)(KifElement*, KifDomain*, KifCallFunction*, int);


/* class KifIteratorkifsocket: public KifIteration {
public:
Kifkifsocket* value;

KifIteratorkifsocket(KifCode* klc,KifElement* kp) : KifIteration(klc,kp) {
value=NULL;
}


//The next methods are the ones that should be derived to add iterator capacities to Kifkifsocket
KifElement* IteratorKey();
KifElement* IteratorValue();
KifElement* Next();
KifElement* End();
KifElement* Begin();
};

*/
const int MAXHOSTNAME = 1000;
const int MAXSIZEINPUT = 256;
const int POSSIGNATURE = 5;
const char* FORMATSIGNATURE = "%4x";

short KifStringId(string n);

class Kifkifsocket;

string Msgerror() {
	char* s = strerror(errno);
	if (strstr(s, KIFSOCKETSTRINGS[0]) != NULL)
		return "";
	return s;
}

static short XConvert(char* number, int nb) {
	unsigned short v = 0;
	for (int i = 0; i<nb; i++) {
		switch (number[i]) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			v <<= 4;
			v |= number[i] - 48;
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			v <<= 4;
			v |= number[i] - 55;
			break;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			v <<= 4;
			v |= number[i] - 87;
		}
	}
	return v;
}

static bool checkipaddres(string serveraddr) {
	int countpoint = 0;
	for (int i = 0; i<serveraddr.size(); i++) {
		if (serveraddr[i] == '.') {
			countpoint++;
			if (countpoint>3)
				return false;
		}
		else
		if (serveraddr[i]<48 || serveraddr[i]>57)
			return false;
	}
	return true;
}

class Kifremote : public KifBasic {
public:
	static map<short, KifCallMethod> kifexportedmethods;

	Kifkifsocket* kifsocket;
	string objectname;
	vector<string> functions;
	string execution;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	//---------------------------------------------------------------------------------------------------------------------
	Kifremote(KifCode* kifcode, KifElement* base, string e) : KifBasic(kifcode, base, remote_type) {
		//Do not forget your variable initialisation
		kifsocket = NULL;
		execution = e;
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return remote_type;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		if (kval->type != type)
			return true;
		kifsocket = ((Kifremote*)kval)->kifsocket;
		objectname = ((Kifremote*)kval)->objectname;
		execution = ((Kifremote*)kval)->execution;
		for (int i = 0; i < ((Kifremote*)kval)->functions.size(); i++)
			functions.push_back(((Kifremote*)kval)->functions[i]);
		return true;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifremote* kperso = new Kifremote(kifcode, NULL, execution);
		//Your initialisations here
		return kperso;
	}

	//-----------------------------------------------------------------------------------------------------------------------

	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	//In this specific case, we create our function on the fly...
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) == kifexportedmethods.end()) {
			KifCallMethod kcm(n, P_FULL);
			kifexportedmethods[n] = kcm;
		}
		return &kifexportedmethods[n];
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {}
	KifElement* Information(KifElement* kstr) {
		return kifNULL;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.	
	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc);

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFSOCKETSTRINGS[1] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() {
		string s = "[";
		for (int i = 0; i < functions.size(); i++) {
			if (i)
				s += ",";
			s += "'";
			s += functions[i];
			s += "'";
		}
		s += "]";
		return s;
	}

	long Integer() {
		return functions.size();
	}
	double Float() {
		return functions.size();
	}
	bool Boolean() {
		if (functions.size() != 0)
			return true;
		return false;
	}

	KifElement* Vector(KifElement*, KifVector* vv) {
		KifVector* kvect = vv;
		if (kvect == NULL)
			kvect = new KifVector(kifcode, NULL);
		else
			kvect->Clear();
		for (int i = 0; i < functions.size(); i++)
			kvect->Push(kifcode->Providestring(functions[i]));
		return kvect;
	}

	KifElement* same(KifElement* a) {
		if (a->type == remote_type) {
			if (((Kifremote*)a)->objectname == objectname && ((Kifremote*)a)->execution == execution)
				return kifTRUE;
			return kifFALSE;
		}
		if ((void*)a == (void*)kifsocket)
			return kifTRUE;
		return kifFALSE;
	}
};

static bool validstream(int nb) {
	if (nb == SOCKET_ERROR || nb == 0)
		return false;
	return true;
}

static map<int, Kifkifsocket*> thelocalsockets;

class Kifkifsocket : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, kifsocketMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	map<int, bool> socketclients;
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	struct sockaddr_in servAddr;
	int idclient;
	bool blocking;
	struct hostent *hp;
	char servername[MAXHOSTNAME + 10];
	int sock, port;
	bool server;
	//For inserting a time out
	fd_set readfds;
	fd_set writefds;
	struct timeval tv;
	bool timeout;
	int v_timeout;

#ifdef WIN32
	int len;
#else
	socklen_t len;
#endif
	//---------------------------------------------------------------------------------------------------------------------
	Kifkifsocket(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, kifsocket_type) {
		//Do not forget your variable initialisation
		hp = NULL;
		sock = -1;
		idclient = 0;
		blocking = true;
		timeout = false;
		thelocalsockets[idgarbage] = this;
	}

	~Kifkifsocket() {
		Close();
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return kifsocket_type;
	}


	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		return true;
	}

	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifkifsocket* kperso = new Kifkifsocket(kifcode, NULL);
		return kperso;
	}


	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//This description is facultative
	//iterator it=mydb;
	//	KifElement* Newiterator(bool rev) {
	//		KifIteratorkifsocket* iter=new KifIteratorkifsocket(kifcode,NULL);
	//		iter->reverse=rev;
	//		//Your initialisation here for your iterator object
	//		return iter;
	//	}
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
		return kifcode->Providestring(infomethods[s]);
	}


	KifElement* CreateSocket() {

		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1)
			return kifcode->Returnerror(KIFSOCKETSTRINGS[2]);
#ifdef WIN32        
		memset((char *)&servAddr, 0, sizeof(servAddr));
#else
		bzero((char *)&servAddr, sizeof(servAddr));
#endif

		if (checkipaddres(servername) == false) {
			hp = gethostbyname(servername);                  /* get our address info */
			if (hp == NULL)                             /* we don't exist !? */
				return kifcode->Returnerror(KIFSOCKETSTRINGS[3]);
			memcpy((char *)&servAddr.sin_addr, hp->h_addr, hp->h_length);     /* set address */
			servAddr.sin_family = hp->h_addrtype;              /* this is our host address */
			/* this is our port number */
		}
		else {
			servAddr.sin_addr.s_addr = inet_addr(servername);
			servAddr.sin_family = AF_INET;
		}

		servAddr.sin_port = htons(port);

		len = sizeof(servAddr);


		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreateServer(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//KifThreadLock _lock(general);
		//In our example, we have only two parameters

		if (hp != NULL)
			return kifcode->Returnerror(KIFSOCKETSTRINGS[4]);
		//0 is the first parameter and so on...
		KifElement* kport;
		KifElement* knbclients;
		if (callfunc->Size() == 2) {
			kport = callfunc->Evaluatethread(0, domain, idthread);
			gethostname(servername, MAXHOSTNAME);
			knbclients = callfunc->Evaluatethread(1, domain, idthread);
		}
		else {
			KifElement* kserver = callfunc->Evaluatethread(0, domain, idthread);
			string s = kserver->String();
			if (s.size() >= MAXHOSTNAME)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[5]);
			strcpy(servername, (char*)s.c_str());
			kport = callfunc->Evaluatethread(1, domain, idthread);
			knbclients = callfunc->Evaluatethread(2, domain, idthread);
		}

		int nbclients = knbclients->Integer();
		port = kport->Integer();
		KifElement* ret = CreateSocket();

		if (ret != kifTRUE)
			return ret;

#ifndef WIN32
		int yes = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			string errmessage = KIFSOCKETSTRINGS[6];
			errmessage += Msgerror();
			return kifcode->Returnerror(errmessage);
		}
#endif

		servAddr.sin_addr.s_addr = INADDR_ANY;
		if (bind(sock, (struct sockaddr*)&servAddr, len) < 0) {
			string errmessage = KIFSOCKETSTRINGS[7];
			errmessage += Msgerror();
			return kifcode->Returnerror(errmessage);
		}

		if (listen(sock, nbclients) < 0) {
			string errmessage = KIFSOCKETSTRINGS[8];
			errmessage += Msgerror();
			return kifcode->Returnerror(errmessage);
		}
		server = true;
		return kifTRUE;
	}

	KifElement* MethodTimeout(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//KifThreadLock _lock(general);
		//In our example, we have only two parameters

		KifElement* ktimeout = callfunc->Evaluatethread(0, domain, idthread);
		v_timeout = ktimeout->Integer();
		if (v_timeout == -1) {
			timeout = false;
			return kifTRUE;
		}

		//We create our timeout		
		tv.tv_sec = v_timeout;
		tv.tv_usec = 0;
		timeout = true;

		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		return kifTRUE;
	}

	KifElement* MethodCreateClient(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//KifThreadLock _lock(general);
		//In our example, we have only two parameters

		if (hp != NULL)
			return kifcode->Returnerror(KIFSOCKETSTRINGS[4]);
		//0 is the first parameter and so on...
		KifElement* kserver = callfunc->Evaluatethread(0, domain, idthread);
		KifElement* kport = callfunc->Evaluatethread(1, domain, idthread);
		port = kport->Integer();

		strcpy(servername, (char*)kserver->String().c_str());
		KifElement* ret = CreateSocket();
		if (ret != kifTRUE)
			return ret;
		server = false;
		if (connect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
			string errmessage = KIFSOCKETSTRINGS[9];
			errmessage += Msgerror();
			return kifcode->Returnerror(errmessage);
		}
		return kifTRUE;
	}

	KifElement* MethodWait(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//KifThreadLock _lock(waiting);

		struct sockaddr_in cliAddr;
		int socketclient = -1;
		if (server == true) {
			if ((socketclient = accept(sock, (struct sockaddr*)&cliAddr, &len)) < 0) {
				string errmessage = KIFSOCKETSTRINGS[10];
				errmessage += Msgerror();
				return kifcode->Returnerror(errmessage);
			}
			if (Error() == true)
				return Errorptr();
			socketclients[socketclient] = true;
			return kifcode->Provideinteger(socketclient);
		}
		return kifcode->Provideinteger(-1);
	}

	KifElement* MethodBlock(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//KifThreadLock _lock(general);
		blocking = callfunc->Evaluatethread(0, domain, idthread)->Boolean();
#ifdef WIN32
		u_long iMode = 0;
		if (blocking == false)
			iMode = 1;
		ioctlsocket(sock, FIONBIO, &iMode);
#else
		int flags;
		flags = fcntl(sock, F_GETFL, 0);
		if (blocking == false)
			flags |= O_NONBLOCK;
		else
			flags &= ~O_NONBLOCK;
		fcntl(sock, F_SETFL, flags);
#endif
		return kifTRUE;
	}

	void Close() {
		if (hp != NULL && sock != -1) {
			closesock(sock);
			map<int, bool>::iterator it;
			for (it = socketclients.begin(); it != socketclients.end(); it++)
				closesock(it->first);
			socketclients.clear();
		}
		hp = NULL;
		sock = -1;
	}

	KifElement* MethodClose(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//KifThreadLock _lock(general);
		if (server == true) {
			if (hp == NULL || sock == -1)
				return kifFALSE;

			//We clean a client connection
			if (callfunc->Size() == 1) {
				int socketclient = callfunc->Evaluatethread(0, domain, idthread)->Integer();
				if (socketclient == -1)
					return kifcode->Returnerror(KIFSOCKETSTRINGS[11]);
				closesock(socketclient);
				if (socketclients.find(socketclient) != socketclients.end())
					socketclients.erase(socketclient);
				return kifTRUE;
			}
		}

		//otherwise we clean the current socket
		if (hp != NULL && sock != -1)
			closesock(sock);
		hp = NULL;
		return kifTRUE;
	}

	bool TestTimeOutRead(int currentsock) {
		if (timeout == true) {
			tv.tv_sec = v_timeout;
			tv.tv_usec = 0;
			FD_ZERO(&readfds);
			//Then we insert our socket in it
			FD_CLR(currentsock, &readfds);
			FD_SET(currentsock, &readfds);
			//int sl=select(currentsock+1,&readfds,NULL,NULL, &tv);
			int sl = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
			if (sl < 0) {
				string errmessage = KIFSOCKETSTRINGS[12];
				errmessage += Msgerror();
				kifcode->Returnerror(errmessage);
				return false;
			}
			sl = FD_ISSET(currentsock, &readfds);
			if (sl == 0) {
				string errmessage = KIFSOCKETSTRINGS[13];
				//errmessage+=Msgerror();
				kifcode->Returnerror(errmessage);
				return false;
			}
		}
		return true;
	}

	bool TestTimeOutWrite(int currentsock) {
		if (timeout == true) {
			tv.tv_sec = v_timeout;
			tv.tv_usec = 0;
			FD_ZERO(&writefds);
			//Then we insert our socket in it
			FD_CLR(currentsock, &writefds);
			FD_SET(currentsock, &writefds);
			//int sl=select(currentsock+1,NULL,&writefds,NULL,&tv);
			int sl = select(FD_SETSIZE, NULL, &writefds, NULL, &tv);
			if (sl < 0) {
				string errmessage = KIFSOCKETSTRINGS[14];
				errmessage += Msgerror();
				kifcode->Returnerror(errmessage);
				return false;
			}
			sl = FD_ISSET(currentsock, &writefds);
			if (sl == 0) {
				string errmessage = KIFSOCKETSTRINGS[13];
				//errmessage+=Msgerror();
				kifcode->Returnerror(errmessage);
				return false;
			}
		}
		return true;
	}


	KifElement* MethodRead(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		short ssz;
		string res;
		int currentsock;
		int socketclient;

		if (server == true) {
			if (callfunc == NULL || callfunc->Size() == 0)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[15]);

			socketclient = callfunc->Evaluatethread(0, domain, idthread)->Integer();
			if (socketclient == -1)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[11]);
			currentsock = socketclient;
		}
		else
			currentsock = sock;


		if (currentsock == -1)
			return kifcode->Returnerror(KIFSOCKETSTRINGS[16]);

		char inputstr[MAXSIZEINPUT + 1];

		int nbcharread = 0;
		int nbloc;
		while (nbcharread < POSSIGNATURE) {
			if (TestTimeOutRead(currentsock) == false)
				return kifNULL;
			nbloc = readsock(currentsock, inputstr + nbcharread, POSSIGNATURE - nbcharread);
			if (validstream(nbloc) == false) {
				string errmessage = KIFSOCKETSTRINGS[17];
				errmessage += Msgerror();
				return kifcode->Returnerror(errmessage);
			}
			nbcharread += nbloc;
		}

		inputstr[POSSIGNATURE] = 0;
		ssz = XConvert(inputstr + 1, POSSIGNATURE - 1);
		int maxtoread;
		//cout<<KIFSOCKETSTRINGS[18]<<ssz<<":"<<inputstr<<endl;
		while (ssz > 0) {
			inputstr[0] = 0;
			if (TestTimeOutRead(currentsock) == false)
				return kifNULL;
			maxtoread = ssz;
			if (maxtoread > MAXSIZEINPUT)
				maxtoread = MAXSIZEINPUT;
			nbcharread = readsock(currentsock, inputstr, maxtoread);
			if (validstream(nbcharread) == false) {
				string errmessage = KIFSOCKETSTRINGS[17];
				errmessage += Msgerror();
				return kifcode->Returnerror(errmessage);
			}
			inputstr[nbcharread] = 0;
			ssz -= nbcharread;
			res += inputstr;
		}
		return kifcode->Providestring(res);
	}


	KifElement* MethodRun(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		short ssz;
		string res;
		int currentsock;
		int socketclient;

		if (server == false)
			return kifcode->Returnerror(KIFSOCKETSTRINGS[19]);


		socketclient = callfunc->Evaluatethread(0, domain, idthread)->Integer();
		if (socketclient == -1)
			return kifcode->Returnerror(KIFSOCKETSTRINGS[11]);
		currentsock = socketclient;

		string stopstring = callfunc->Evaluatethread(1, domain, idthread)->String();

		if (currentsock == -1)
			return kifcode->Returnerror(KIFSOCKETSTRINGS[16]);

		char inputstr[MAXSIZEINPUT + 1];

		bool erroronexecution = false;
		while (true) {
			if (Error() == true || erroronexecution == true) {
				Seterror(true);
				return Errorptr();
			}

			int nbcharread = 0;
			int nbloc;
			while (nbcharread < POSSIGNATURE) {
				if (TestTimeOutRead(currentsock) == false)
					return kifNULL;
				nbloc = readsock(currentsock, inputstr + nbcharread, POSSIGNATURE - nbcharread);
				if (validstream(nbloc) == false) {
					string errmessage = KIFSOCKETSTRINGS[17];
					errmessage += Msgerror();
					return kifcode->Returnerror(errmessage);
				}
				nbcharread += nbloc;
			}

			char act = inputstr[0];
			inputstr[POSSIGNATURE] = 0;
			ssz = XConvert(inputstr + 1, POSSIGNATURE - 1);
			int maxtoread;
			while (ssz > 0) {
				inputstr[0] = 0;
				if (TestTimeOutRead(currentsock) == false)
					return kifNULL;
				maxtoread = ssz;
				if (maxtoread > MAXSIZEINPUT)
					maxtoread = MAXSIZEINPUT;
				nbcharread = readsock(currentsock, inputstr, maxtoread);
				if (validstream(nbcharread) == false) {
					string errmessage = KIFSOCKETSTRINGS[17];
					errmessage += Msgerror();
					return kifcode->Returnerror(errmessage);
				}
				inputstr[nbcharread] = 0;
				ssz -= nbcharread;
				res += inputstr;
			}

			if (stopstring != "" && res == stopstring)
				break;

			if (act == 'G') {
				//Then the string, which has been read, is the name of an object, which we are looking for
				short n = KifStringId(res);
				KifElement* obj = kifcode->Get(n, domain);
				if (obj == NULL || obj->type != kifVariable || obj->Private() || obj->Returnvalue()->type != kifDomain) {
					string errmessage = KIFSOCKETSTRINGS[20];
					errmessage += res;
					errmessage += KIFSOCKETSTRINGS[21];
					errmessage += Msgerror();

					KifElement* kerr = kifcode->Returnerror(errmessage);
					Seterror(false);
					erroronexecution = true;

					KifCallFunction kfunc(NULL, NULL, n);
					kfunc.kifcode = kifcode;
					//We add our parameters
					//including the client to which date should be sent to
					KifInteger sclient(NULL, NULL, socketclient);
					sclient.kifcode = kifcode;
					kfunc.parameters.push_back(&sclient);
					kfunc.parameters.push_back(kerr);
					//we then return to the client a map, which contains the information
					//of that frame...
					Write("N", contextualpattern, domain, &kfunc, idthread);
					res = "";
					continue;
				}

				//we get the map corresponding to the object
				KifVector* functions = new KifVector(kifcode, NULL);
				KifDomain* dm = (KifDomain*)obj->Returnvalue();
				KifFrame* m = (KifFrame*)dm->Frame();
				bin_hash<KifElement*>::iterator it;
				KifElement* x;
				for (it = m->declarations.begin(); it != m->declarations.end(); it++) {
					x = it->second;
					if (x->type == kifFunction && !x->Private())
						functions->Push(kifcode->Providestringraw(KifIdString(x->Name())));
				}

				KifCallFunction kfunc(NULL, NULL, n);
				kfunc.kifcode = kifcode;
				//We add our parameters
				//including the client to which date should be sent to
				KifInteger sclient(NULL, NULL, socketclient);
				sclient.kifcode = kifcode;
				kfunc.parameters.push_back(&sclient);
				kfunc.parameters.push_back(functions);
				//we then return to the client a map, which contains the information
				//of that frame...
				Write("N", contextualpattern, domain, &kfunc, idthread);
				//we then return to reading information...
				res = "";
				continue;
			}

			//We try to get a function handle
			if (act == 'g') {
				//Then the string, which has been read, is the name of the function, which we are looking for
				short n = KifStringId(res);
				KifElement* obj = kifcode->Get(n, domain);
				if (obj == NULL || obj->type != kifFunction || obj->Private()) {
					string errmessage = KIFSOCKETSTRINGS[22];
					errmessage += res;
					errmessage += KIFSOCKETSTRINGS[21];

					KifElement* kerr = kifcode->Returnerror(errmessage);
					Seterror(false);
					erroronexecution = true;

					KifCallFunction kfunc(NULL, NULL, n);
					kfunc.kifcode = kifcode;
					//We add our parameters
					//including the client to which date should be sent to
					KifInteger sclient(NULL, NULL, socketclient);
					sclient.kifcode = kifcode;
					kfunc.parameters.push_back(&sclient);
					kfunc.parameters.push_back(kerr);
					//we then return to the client a map, which contains the information
					//of that frame...
					Write("N", contextualpattern, domain, &kfunc, idthread);
					res = "";
					continue;
				}

				KifCallFunction kfunc(NULL, NULL, n);
				kfunc.kifcode = kifcode;
				//We add our parameters
				//including the client to which date should be sent to
				KifInteger sclient(NULL, NULL, socketclient);
				sclient.kifcode = kifcode;
				kfunc.parameters.push_back(&sclient);
				kfunc.parameters.push_back(kifcode->Providestring(res));
				//we then return to the client a map, which contains the information
				//of that frame...
				Write("N", contextualpattern, domain, &kfunc, idthread);
				//we then return to reading information...
				res = "";
				continue;
			}
			if (act == 'O') {
				//This string is the object name again
				short n = KifStringId(res);
				//Now we read the function name
				string functionname = MethodRead(contextualpattern, domain, callfunc, idthread)->String();
				//The the number of parameters
				int nb = MethodRead(contextualpattern, domain, callfunc, idthread)->Integer();

				vector<KifElement*> args;
				while (nb > 0) {
					args.push_back(MethodRead(contextualpattern, domain, callfunc, idthread));
					nb--;
				}
				//We need to read everything first...
				KifElement* obj = kifcode->Get(n, domain);
				KifDomain* kframe = (KifDomain*)obj->Returnvalue();
				//We then must execute it...
				n = KifStringId(functionname);
				//we look for it
				KifFunction* func = (KifFunction*)kframe->Declaration(n);
				KifCallFunctionGeneral kfunc(NULL, NULL, n, func);
				kfunc.kifcode = kifcode;
				for (nb = 0; nb < args.size(); nb++)
					kfunc.parameters.push_back(args[nb]);
				obj = kfunc.Exec(contextualpattern, kframe, kifNULL, idthread, callfunc);
				if (Error() == true) {
					obj = Errorptr();
					Seterror(false);
					erroronexecution = true;
				}

				//Now that we got our answer, we return it to the client socket
				kfunc.parameters.clear();
				KifInteger sclient(NULL, NULL, socketclient);
				sclient.kifcode = kifcode;
				kfunc.parameters.push_back(&sclient);
				kfunc.parameters.push_back(obj);
				Write("N", contextualpattern, domain, &kfunc, idthread);
				res = "";
				continue;
			}
			if (act == 'X') {
				//This string is the function name
				short n = KifStringId(res);
				//Now we read the function name again
				string functionname = MethodRead(contextualpattern, domain, callfunc, idthread)->String();
				//Then the number of parameters
				int nb = MethodRead(contextualpattern, domain, callfunc, idthread)->Integer();

				vector<KifElement*> args;
				while (nb > 0) {
					args.push_back(MethodRead(contextualpattern, domain, callfunc, idthread));
					nb--;
				}
				//We need to read everything first...
				KifElement* func = kifcode->Get(n, domain);
				KifCallFunctionGeneral kfunc(NULL, NULL, n, (KifFunction*)func);
				kfunc.kifcode = kifcode;
				for (nb = 0; nb < args.size(); nb++)
					kfunc.parameters.push_back(args[nb]);
				func = kfunc.Exec(contextualpattern, domain, kifNULL, idthread, callfunc);
				if (Error() == true) {
					func = Errorptr();
					Seterror(false);
					erroronexecution = true;
				}
				//Now that we got our answer, we return it to the client socket
				kfunc.parameters.clear();
				KifInteger sclient(NULL, NULL, socketclient);
				sclient.kifcode = kifcode;
				kfunc.parameters.push_back(&sclient);
				kfunc.parameters.push_back(func);
				Write("N", contextualpattern, domain, &kfunc, idthread);
				res = "";
				continue;
			}
		}
		return kifTRUE;
	}

	//act=N: Normal case
	//act=G: Get frame
	//act=X: Execute method

	KifElement* Write(string act, KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		KifElement* kvalue;
		short sz;
		string strc;
		int first = 0;
		char padding[POSSIGNATURE + 1];

		memset(padding, 0, POSSIGNATURE + 1);

		int currentsock = sock;

		if (server == true) {
			first = 1;
			if (callfunc->Size() <= 1)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[23]);

			currentsock = callfunc->Evaluatethread(0, domain, idthread)->Integer();
			if (currentsock == -1)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[16]);
		}

		char* buff;
		bool written = false;
		for (int i = first; i<callfunc->Size(); i++) {
			kvalue = callfunc->Evaluatethread(i, domain, idthread);
			if (kvalue->type == kifError) {
				strc = "&&ERROR&&:";
				strc += kvalue->String();
			}
			else
				strc = kvalue->String();

			sz = strc.size();
			if (sz>0) {
				written = true;
				sprintf_s(padding + 1, POSSIGNATURE, FORMATSIGNATURE, sz);
				padding[0] = act[0];
				//cout<<KIFSOCKETSTRINGS[24]<<sz<<":"<<padding<<endl;

				if (TestTimeOutWrite(currentsock) == false)
					return kifNULL;

				if (writesock(currentsock, padding, POSSIGNATURE)<0) {
					string errmessage = KIFSOCKETSTRINGS[25];
					errmessage += Msgerror();
					return kifcode->Returnerror(errmessage);
				}
				buff = STR(strc);
				while (sz>0) {
					int nbsz = sz;
					if (nbsz > MAXSIZEINPUT)
						nbsz = MAXSIZEINPUT;

					if (TestTimeOutWrite(currentsock) == false)
						return kifNULL;

					if (writesock(currentsock, buff, nbsz) < 0) {
						string errmessage = KIFSOCKETSTRINGS[25];
						errmessage += Msgerror();
						return kifcode->Returnerror(errmessage);
					}
					buff += MAXSIZEINPUT;
					sz -= MAXSIZEINPUT;
				}
			}
		}

		if (written == false) {
			//Empty strings... We still write it...
			sprintf_s(padding + 1, POSSIGNATURE, FORMATSIGNATURE, 0);
			padding[0] = act[0];
			if (TestTimeOutWrite(currentsock) == false)
				return kifNULL;

			if (writesock(currentsock, padding, POSSIGNATURE) < 0) {
				string errmessage = KIFSOCKETSTRINGS[25];
				errmessage += Msgerror();
				return kifcode->Returnerror(errmessage);
			}
		}
		return kifTRUE;
	}

	KifElement* MethodWrite(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		return Write("N", contextualpattern, domain, callfunc, idthread);
	}

	KifElement* MethodGet(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		int nb = -1;
		int currentsock;
		if (server == true) {
			if (callfunc->Size() == 0)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[26]);

			currentsock = callfunc->Evaluatethread(0, domain, idthread)->Integer();
			if (currentsock == -1)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[11]);
		}
		else
			currentsock = sock;

		if (currentsock == -1)
			return kifcode->Returnerror(KIFSOCKETSTRINGS[16]);

		if (TestTimeOutRead(currentsock) == false)
			return kifNULL;

		char rd[] = { 0, 0, 0 };
		nb = readsock(currentsock, rd, 1);

		if (validstream(nb) == false) {						//In the case of non blocking socket, we simply return the empty string
			string errmessage = KIFSOCKETSTRINGS[27];
			errmessage += Msgerror();
			return kifcode->Returnerror(errmessage);
		}
		if (!nb)
			return kifNULL;

		if (contextualpattern->type == kifByte) {
			((KifByte*)contextualpattern)->value = rd[0];
			return contextualpattern;
		}

		return new KifByte(kifcode, NULL, rd[0]);
	}

	KifElement* MethodReadRaw(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		string res;
		int nb = -1;
		int currentsock;
		//KifThreadLock _lock(reading);
		int nbbytes = MAXSIZEINPUT;
		bool decrement = false;
		if (server == true) {
			if (callfunc->Size() == 0)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[26]);

			currentsock = callfunc->Evaluatethread(0, domain, idthread)->Integer();
			if (callfunc->Size() == 2) {
				nbbytes = callfunc->Evaluatethread(1, domain, idthread)->Integer();
				decrement = true;
			}

			if (currentsock == -1)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[11]);
		}
		else {
			if (callfunc->Size() == 2)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[28]);
			currentsock = sock;
			if (callfunc->Size() == 1) {
				nbbytes = callfunc->Evaluatethread(0, domain, idthread)->Integer();
				decrement = true;
			}
		}

		if (currentsock == -1)
			return kifcode->Returnerror(KIFSOCKETSTRINGS[16]);

		if (TestTimeOutRead(currentsock) == false)
			return kifNULL;

		char inputstr[MAXSIZEINPUT + 1];

		if (nbbytes >= MAXSIZEINPUT)
			nb = readsock(currentsock, inputstr, MAXSIZEINPUT);
		else
			nb = readsock(currentsock, inputstr, nbbytes);

		if (validstream(nb) == false) {						//In the case of non blocking socket, we simply return the empty string
			string errmessage = KIFSOCKETSTRINGS[29];
			errmessage += Msgerror();
			return kifcode->Returnerror(errmessage);
		}

		if (nb != 0) {
			inputstr[nb] = 0;
			res += inputstr;
		}
		if (decrement)
			nbbytes -= nb;
		while (nb != 0 && nbbytes > 0) {
			if (TestTimeOutRead(currentsock) == false)
				return kifNULL;
			if (nbbytes >= MAXSIZEINPUT)
				nb = readsock(currentsock, inputstr, MAXSIZEINPUT);
			else
				nb = readsock(currentsock, inputstr, nbbytes);
			if (decrement)
				nbbytes -= nb;
			if (nb != 0) {
				inputstr[nb] = 0;
				res += inputstr;
			}
			if (validstream(nb) == false) {
				string errmessage = KIFSOCKETSTRINGS[29];
				errmessage += Msgerror();
				return kifcode->Returnerror(errmessage);
			}
		}
		return kifcode->Providestring(res);
	}

	KifElement* MethodWriteRaw(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {


		int currentsock = sock;
		int first = 0;
		if (server == true) {
			first = 1;
			if (callfunc->Size() <= 1)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[30]);

			currentsock = callfunc->Evaluatethread(0, domain, idthread)->Integer();
			if (currentsock == -1)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[16]);
		}

		if (TestTimeOutWrite(currentsock) == false)
			return kifNULL;

		KifElement* kvalue;
		string strc;
		char* buff;
		int sz;
		for (int i = first; i<callfunc->Size(); i++) {
			kvalue = callfunc->Evaluatethread(i, domain, idthread);
			strc = kvalue->String();
			buff = STR(strc);
			sz = strc.size();
			while (sz>0) {
				int nbsz = sz;
				if (nbsz > MAXSIZEINPUT)
					nbsz = MAXSIZEINPUT;
				if (writesock(currentsock, buff, nbsz) < 0) {
					string errmessage = KIFSOCKETSTRINGS[25];
					errmessage += Msgerror();
					return kifcode->Returnerror(errmessage);
				}
				buff += MAXSIZEINPUT;
				sz -= MAXSIZEINPUT;
			}
		}
		return kifTRUE;
	}



	KifElement* MethodGethostname(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		gethostname(servername, MAXHOSTNAME);           /* who are we? */
		return kifcode->Providestringraw(servername);
	}

	KifElement* MethodGetpeername(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (server == true) {
			int socketclient = callfunc->Evaluatethread(0, domain, idthread)->Integer();
			if (socketclient == -1)
				return kifcode->Returnerror(KIFSOCKETSTRINGS[11]);
			struct sockaddr cliAddr;
#ifdef WIN32
			int len = sizeof(struct sockaddr);
#else
			socklen_t len = sizeof(struct sockaddr);
#endif
			getpeername(socketclient, &cliAddr, &len);
			struct sockaddr_in* client = (struct sockaddr_in*)&cliAddr;
			char* nm = inet_ntoa(client->sin_addr);
			KifMap* kmap = new KifMap(kifcode, NULL);
			string key1 = KIFSOCKETSTRINGS[31];
			kmap->Push(key1, kifcode->Provideinteger(client->sin_port));
			string key2 = KIFSOCKETSTRINGS[32];
			string nms = nm;
			kmap->Push(key2, kifcode->Providestring(nms));
			return kmap;
		}
		return kifcode->Returnerror(KIFSOCKETSTRINGS[33]);
	}

	KifElement* MethodGetframe(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//This method is used to return an object which has been declated on the server side...
		//we create a remote object

		//we then check that the object exists
		string objectname = callfunc->Evaluatethread(0, domain, idthread)->String();
		//we request an object
		Write("G", contextualpattern, domain, callfunc, idthread);
		//we then read the information, which should be a map...
		KifElement* m = MethodRead(contextualpattern, domain, NULL, idthread);
		string smap = m->String();
		if (smap[0] != '[') {
			if (smap.size() > 13 && !memcmp(STR(smap), "&&ERROR&&:", 10))
				return kifcode->Returnerror(smap.substr(10, smap.size() - 10));
			else
				return kifcode->Returnerror(smap);
		}
		//we then transform it into a map
		m = m->Vector(domain);
		Kifremote* remote = new Kifremote(kifcode, NULL, "O");
		remote->kifsocket = this;
		remote->objectname = objectname;
		for (int i = 0; i < ((KifVector*)m)->values.size(); i++)
			remote->functions.push_back(((KifVector*)m)->values[i]->String());
		return remote;
	}

	KifElement* MethodPort(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		return kifcode->Provideinteger(port);
	}

	KifElement* MethodGetfunction(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//This method is used to return an object which has been declated on the server side...
		//we create a remote object

		//we then check that the object exists
		string objectname = callfunc->Evaluatethread(0, domain, idthread)->String();
		//we request an object
		Write("g", contextualpattern, domain, callfunc, idthread);
		//we then read the information, which should be a string...
		KifElement* m = MethodRead(contextualpattern, domain, NULL, idthread);
		string smap = m->String();
		if (smap.size() > 13 && !memcmp(STR(smap), "&&ERROR&&:", 10))
			return kifcode->Returnerror(smap.substr(10, smap.size() - 10));

		Kifremote* remote = new Kifremote(kifcode, NULL, "X");
		remote->kifsocket = this;
		remote->objectname = objectname;
		remote->functions.push_back(m->String());
		return remote;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFSOCKETSTRINGS[34]
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		KiFArityError(name)
			return (this->*linkedmethods[name])(contextualpattern, domain, callfunc, idthread);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//if (hp!=NULL)
		//	closesock(sock);
		hp = NULL;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
		//if (hp!=NULL)
		//	closesock(sock);
		hp = NULL;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFSOCKETSTRINGS[1] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return servername; }
	long Integer() { return port; }
	double Float() { return port; }
	bool Boolean() { return true; }

	//You can overcome the following function behaviour

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
		if (a == this)
			return kifTRUE;
		return kifFALSE;
	}

	//different: this!=a
	KifElement* different(KifElement* a) {
		if (a != this)
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

//Execution of a method in the case of a remote... The list of potential functions are in the description vector
KifElement* Kifremote::ExecMethod(short n, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
	KiFArityError(n)
		string name = KifIdString(n);
	int i = 0;

	for (i = 0; i < functions.size(); i++) {
		if (functions[i] == name)
			break;
	}

	if (i == functions.size())
		return kifcode->Returnerror(KIFSOCKETSTRINGS[35]);

	//We are now executing a remote function
	//First we send the name of the object, then the name of the function, then the number of parameters, then the parameters
	KifCallFunction kfunc(NULL, this, n);
	kfunc.kifcode = kifcode;
	KifBaseString kname(NULL, NULL, objectname);
	kname.kifcode = kifcode;
	kfunc.parameters.push_back(&kname);
	kifsocket->Write(execution, contextualpattern, domain, &kfunc, idthread);
	kname.value = name;
	kifsocket->Write("N", contextualpattern, domain, &kfunc, idthread);
	//The number of parameters
	kfunc.parameters.pop_back();
	KifInteger knum(NULL, NULL, callfunc->Size());
	knum.kifcode = kifcode;
	kfunc.parameters.push_back(&knum);
	kifsocket->Write("N", contextualpattern, domain, &kfunc, idthread);
	//then the parameters themselves
	kfunc.parameters.clear();
	kfunc.parameters.push_back(&kname);
	for (i = 0; i<callfunc->Size(); i++) {
		kname.value = callfunc->Evaluatethread(i, domain, idthread)->String();
		kifsocket->Write("N", contextualpattern, domain, &kfunc, idthread);
	}

	//We must get the result as well now
	KifElement* ret = kifsocket->MethodRead(contextualpattern, domain, NULL, idthread);
	string res = ret->String();
	if (res.size()>13 && !memcmp(STR(res), "&&ERROR&&:", 10))
		return kifcode->Returnerror(res.substr(10, res.size() - 10));
	return ret;
}

//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifremote::kifexportedmethods;
map<short, KifCallMethod> Kifkifsocket::kifexportedmethods;
map<short, kifsocketMethod>  Kifkifsocket::linkedmethods;
map<string, string> Kifkifsocket::infomethods;
hmap<short, bool> Kifkifsocket::methodassigned;

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific iterator implementation...
//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifkifsocketElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifkifsocket* local = new Kifkifsocket(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifremoteElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifremote* local = new Kifremote(kifcode, variable, "N");
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name, kifsocketMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifkifsocket::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifkifsocket::linkedmethods[idname] = func;
	Kifkifsocket::methodassigned[idname] = true;
	Kifkifsocket::infomethods[name] = infos;
}

#ifdef WIN32
#define Exporting __declspec(dllexport)
static bool wsainit = true;
#endif

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {

	Exporting void kifsocket_WindowsInit() {
#ifdef WIN32
		if (wsainit) {
			WSAStartup(MAKEWORD(2, 0), &WSAData);
			wsainit = false;
		}
#endif
	}

	Exporting bool Init_kifsocket(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << KIFSOCKETSTRINGS[36] << endl;
			return false;
		}


		kifcode->KifInitLib();
		///The new type will be: kifsocket. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifkifsocketElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifkifsocket)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifkifsocketElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType(KIFSOCKETSTRINGS[37], CreatekifkifsocketElement);
		if (res == kifVOID)
			return false;
		kifsocket_type = res;

		res = KifAddNewType(KIFSOCKETSTRINGS[38], CreatekifremoteElement);
		if (res == kifVOID)
			return false;
		remote_type = res;

		return true;
	}

	Exporting bool kifsocket_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifsocket(kifcode, version) == false)
			return false;

		kifsocket_WindowsInit();

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization(KIFSOCKETSTRINGS[39], &Kifkifsocket::MethodCreateServer, P_THREE | P_TWO, KIFSOCKETSTRINGS[40]);
		MethodInitialization(KIFSOCKETSTRINGS[41], &Kifkifsocket::MethodCreateClient, P_TWO, KIFSOCKETSTRINGS[42]);
		MethodInitialization(KIFSOCKETSTRINGS[43], &Kifkifsocket::MethodWait, P_NONE, KIFSOCKETSTRINGS[44]);
		MethodInitialization(KIFSOCKETSTRINGS[45], &Kifkifsocket::MethodRead, P_ONE | P_NONE, KIFSOCKETSTRINGS[46]);
		MethodInitialization(KIFSOCKETSTRINGS[47], &Kifkifsocket::MethodRun, P_TWO, KIFSOCKETSTRINGS[48]);
		MethodInitialization(KIFSOCKETSTRINGS[49], &Kifkifsocket::MethodWrite, P_ATLEASTONE, KIFSOCKETSTRINGS[50]);
		MethodInitialization(KIFSOCKETSTRINGS[51], &Kifkifsocket::MethodReadRaw, P_TWO | P_ONE | P_NONE, KIFSOCKETSTRINGS[52]);
		MethodInitialization(KIFSOCKETSTRINGS[53], &Kifkifsocket::MethodGet, P_ONE | P_NONE, KIFSOCKETSTRINGS[54]);
		MethodInitialization(KIFSOCKETSTRINGS[55], &Kifkifsocket::MethodWriteRaw, P_ATLEASTONE, KIFSOCKETSTRINGS[56]);
		MethodInitialization(KIFSOCKETSTRINGS[57], &Kifkifsocket::MethodClose, P_NONE | P_ONE, KIFSOCKETSTRINGS[58]);
		MethodInitialization(KIFSOCKETSTRINGS[59], &Kifkifsocket::MethodBlock, P_ONE, KIFSOCKETSTRINGS[60]);
		MethodInitialization(KIFSOCKETSTRINGS[61], &Kifkifsocket::MethodTimeout, P_ONE, KIFSOCKETSTRINGS[62]);
		MethodInitialization(KIFSOCKETSTRINGS[63], &Kifkifsocket::MethodGethostname, P_NONE, KIFSOCKETSTRINGS[64]);
		MethodInitialization(KIFSOCKETSTRINGS[31], &Kifkifsocket::MethodPort, P_NONE, KIFSOCKETSTRINGS[65]);
		MethodInitialization(KIFSOCKETSTRINGS[66], &Kifkifsocket::MethodGetpeername, P_ONE, KIFSOCKETSTRINGS[67]);
		MethodInitialization(KIFSOCKETSTRINGS[68], &Kifkifsocket::MethodGetframe, P_ONE, KIFSOCKETSTRINGS[69]);
		MethodInitialization(KIFSOCKETSTRINGS[70], &Kifkifsocket::MethodGetfunction, P_ONE, KIFSOCKETSTRINGS[71]);

		kifGlobals->kifAllFunctions[kifsocket_type] = Kifkifsocket::methodassigned;
		kifGlobals->kifAllInformation[kifsocket_type] = Kifkifsocket::infomethods;

		//If you want to create specific variables in the main frame		
		//KifVariableDeclaration* var=kifcode->Createvariable(KIFSOCKETSTRINGS[72]);
		//then we create its value
		//Kifkifsocket* local=new Kifkifsocket(kifcode,var);
		//Mutex creation
		//KifCreateMutex(general);
		//KifCreateMutex(waiting);
		return true;
	}


	Exporting bool kifsocket_Terminate(KifCode* kifcode) {
		//KifDeleteMutex(general);
		//KifDeleteMutex(waiting);
		Kifremote::kifexportedmethods.clear();
		Kifkifsocket::kifexportedmethods.clear();
		Kifkifsocket::linkedmethods.clear();
		Kifkifsocket::methodassigned.clear();
		Kifkifsocket::infomethods.clear();
		ThreadLock _lock(kifGarbaging);
		ThreadLock _lockg(kifGarbage);

		map<int, Kifkifsocket*>::iterator it;
		for (it = thelocalsockets.begin(); it != thelocalsockets.end(); it++) {
			if (kifGlobals->garbage[it->first] != NULL)
				it->second->Close();
		}
		thelocalsockets.clear();
#ifdef WIN32
		WSACleanup();
		wsainit = true;
#endif
		return true;
	}
}





