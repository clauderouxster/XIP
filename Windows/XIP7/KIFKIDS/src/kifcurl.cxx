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
#include <curl/curl.h>
#include "kifcontainer.h"

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif

#define STR(x) (char*)x.c_str()

static map<string, CURLoption> curloptions;

static void Init() {
	static bool init = false;
	if (!init) {
		curloptions["CURLOPT_ACCEPTTIMEOUT_MS"] = CURLOPT_ACCEPTTIMEOUT_MS;
		curloptions["CURLOPT_ACCEPT_ENCODING"] = CURLOPT_ACCEPT_ENCODING;
		curloptions["CURLOPT_ADDRESS_SCOPE"] = CURLOPT_ADDRESS_SCOPE;
		curloptions["CURLOPT_APPEND"] = CURLOPT_APPEND;
		curloptions["CURLOPT_AUTOREFERER"] = CURLOPT_AUTOREFERER;
		curloptions["CURLOPT_BUFFERSIZE"] = CURLOPT_BUFFERSIZE;
		curloptions["CURLOPT_CAINFO"] = CURLOPT_CAINFO;
		curloptions["CURLOPT_CAPATH"] = CURLOPT_CAPATH;
		curloptions["CURLOPT_CERTINFO"] = CURLOPT_CERTINFO;
		curloptions["CURLOPT_CHUNK_BGN_FUNCTION"] = CURLOPT_CHUNK_BGN_FUNCTION;
		curloptions["CURLOPT_CHUNK_DATA"] = CURLOPT_CHUNK_DATA;
		curloptions["CURLOPT_CHUNK_END_FUNCTION"] = CURLOPT_CHUNK_END_FUNCTION;
		curloptions["CURLOPT_CLOSESOCKETDATA"] = CURLOPT_CLOSESOCKETDATA;
		curloptions["CURLOPT_CLOSESOCKETFUNCTION"] = CURLOPT_CLOSESOCKETFUNCTION;
		curloptions["CURLOPT_CONNECTTIMEOUT"] = CURLOPT_CONNECTTIMEOUT;
		curloptions["CURLOPT_CONNECTTIMEOUT_MS"] = CURLOPT_CONNECTTIMEOUT_MS;
		curloptions["CURLOPT_CONNECT_ONLY"] = CURLOPT_CONNECT_ONLY;
		curloptions["CURLOPT_CONV_FROM_NETWORK_FUNCTION"] = CURLOPT_CONV_FROM_NETWORK_FUNCTION;
		curloptions["CURLOPT_CONV_FROM_UTF8_FUNCTION"] = CURLOPT_CONV_FROM_UTF8_FUNCTION;
		curloptions["CURLOPT_CONV_TO_NETWORK_FUNCTION"] = CURLOPT_CONV_TO_NETWORK_FUNCTION;
		curloptions["CURLOPT_COOKIE"] = CURLOPT_COOKIE;
		curloptions["CURLOPT_COOKIEFILE"] = CURLOPT_COOKIEFILE;
		curloptions["CURLOPT_COOKIEJAR"] = CURLOPT_COOKIEJAR;
		curloptions["CURLOPT_COOKIELIST"] = CURLOPT_COOKIELIST;
		curloptions["CURLOPT_COOKIESESSION"] = CURLOPT_COOKIESESSION;
		curloptions["CURLOPT_COPYPOSTFIELDS"] = CURLOPT_COPYPOSTFIELDS;
		curloptions["CURLOPT_CRLF"] = CURLOPT_CRLF;
		curloptions["CURLOPT_CRLFILE"] = CURLOPT_CRLFILE;
		curloptions["CURLOPT_CUSTOMREQUEST"] = CURLOPT_CUSTOMREQUEST;
		curloptions["CURLOPT_DEBUGDATA"] = CURLOPT_DEBUGDATA;
		curloptions["CURLOPT_DEBUGFUNCTION"] = CURLOPT_DEBUGFUNCTION;
		curloptions["CURLOPT_DIRLISTONLY"] = CURLOPT_DIRLISTONLY;
		curloptions["CURLOPT_DNS_CACHE_TIMEOUT"] = CURLOPT_DNS_CACHE_TIMEOUT;
		curloptions["CURLOPT_DNS_SERVERS"] = CURLOPT_DNS_SERVERS;
		curloptions["CURLOPT_DNS_USE_GLOBAL_CACHE"] = CURLOPT_DNS_USE_GLOBAL_CACHE;
		curloptions["CURLOPT_EGDSOCKET"] = CURLOPT_EGDSOCKET;
		curloptions["CURLOPT_ERRORBUFFER"] = CURLOPT_ERRORBUFFER;
		curloptions["CURLOPT_FAILONERROR"] = CURLOPT_FAILONERROR;
		curloptions["CURLOPT_FILETIME"] = CURLOPT_FILETIME;
		curloptions["CURLOPT_FNMATCH_DATA"] = CURLOPT_FNMATCH_DATA;
		curloptions["CURLOPT_FNMATCH_FUNCTION"] = CURLOPT_FNMATCH_FUNCTION;
		curloptions["CURLOPT_FOLLOWLOCATION"] = CURLOPT_FOLLOWLOCATION;
		curloptions["CURLOPT_FORBID_REUSE"] = CURLOPT_FORBID_REUSE;
		curloptions["CURLOPT_FRESH_CONNECT"] = CURLOPT_FRESH_CONNECT;
		curloptions["CURLOPT_FTPPORT"] = CURLOPT_FTPPORT;
		curloptions["CURLOPT_FTPSSLAUTH"] = CURLOPT_FTPSSLAUTH;
		curloptions["CURLOPT_FTP_ACCOUNT"] = CURLOPT_FTP_ACCOUNT;
		curloptions["CURLOPT_FTP_ALTERNATIVE_TO_USER"] = CURLOPT_FTP_ALTERNATIVE_TO_USER;
		curloptions["CURLOPT_FTP_CREATE_MISSING_DIRS"] = CURLOPT_FTP_CREATE_MISSING_DIRS;
		curloptions["CURLOPT_FTP_FILEMETHOD"] = CURLOPT_FTP_FILEMETHOD;
		curloptions["CURLOPT_FTP_RESPONSE_TIMEOUT"] = CURLOPT_FTP_RESPONSE_TIMEOUT;
		curloptions["CURLOPT_FTP_SKIP_PASV_IP"] = CURLOPT_FTP_SKIP_PASV_IP;
		curloptions["CURLOPT_FTP_SSL_CCC"] = CURLOPT_FTP_SSL_CCC;
		curloptions["CURLOPT_FTP_USE_EPRT"] = CURLOPT_FTP_USE_EPRT;
		curloptions["CURLOPT_FTP_USE_EPSV"] = CURLOPT_FTP_USE_EPSV;
		curloptions["CURLOPT_FTP_USE_PRET"] = CURLOPT_FTP_USE_PRET;
		curloptions["CURLOPT_GSSAPI_DELEGATION"] = CURLOPT_GSSAPI_DELEGATION;
		curloptions["CURLOPT_HEADER"] = CURLOPT_HEADER;
		curloptions["CURLOPT_HEADERDATA"] = CURLOPT_HEADERDATA;
		curloptions["CURLOPT_HEADERFUNCTION"] = CURLOPT_HEADERFUNCTION;
		curloptions["CURLOPT_HTTP200ALIASES"] = CURLOPT_HTTP200ALIASES;
		curloptions["CURLOPT_HTTPAUTH"] = CURLOPT_HTTPAUTH;
		curloptions["CURLOPT_HTTPGET"] = CURLOPT_HTTPGET;
		curloptions["CURLOPT_HTTPHEADER"] = CURLOPT_HTTPHEADER;
		curloptions["CURLOPT_HTTPPOST"] = CURLOPT_HTTPPOST;
		curloptions["CURLOPT_HTTPPROXYTUNNEL"] = CURLOPT_HTTPPROXYTUNNEL;
		curloptions["CURLOPT_HTTP_CONTENT_DECODING"] = CURLOPT_HTTP_CONTENT_DECODING;
		curloptions["CURLOPT_HTTP_TRANSFER_DECODING"] = CURLOPT_HTTP_TRANSFER_DECODING;
		curloptions["CURLOPT_HTTP_VERSION"] = CURLOPT_HTTP_VERSION;
		curloptions["CURLOPT_IGNORE_CONTENT_LENGTH"] = CURLOPT_IGNORE_CONTENT_LENGTH;
		curloptions["CURLOPT_INFILESIZE"] = CURLOPT_INFILESIZE;
		curloptions["CURLOPT_INFILESIZE_LARGE"] = CURLOPT_INFILESIZE_LARGE;
		curloptions["CURLOPT_INTERLEAVEDATA"] = CURLOPT_INTERLEAVEDATA;
		curloptions["CURLOPT_INTERLEAVEFUNCTION"] = CURLOPT_INTERLEAVEFUNCTION;
		curloptions["CURLOPT_IOCTLDATA"] = CURLOPT_IOCTLDATA;
		curloptions["CURLOPT_IOCTLFUNCTION"] = CURLOPT_IOCTLFUNCTION;
		curloptions["CURLOPT_IPRESOLVE"] = CURLOPT_IPRESOLVE;
		curloptions["CURLOPT_ISSUERCERT"] = CURLOPT_ISSUERCERT;
		curloptions["CURLOPT_KEYPASSWD"] = CURLOPT_KEYPASSWD;
		curloptions["CURLOPT_KRBLEVEL"] = CURLOPT_KRBLEVEL;
		curloptions["CURLOPT_LOCALPORT"] = CURLOPT_LOCALPORT;
		curloptions["CURLOPT_LOCALPORTRANGE"] = CURLOPT_LOCALPORTRANGE;
		curloptions["CURLOPT_LOW_SPEED_LIMIT"] = CURLOPT_LOW_SPEED_LIMIT;
		curloptions["CURLOPT_LOW_SPEED_TIME"] = CURLOPT_LOW_SPEED_TIME;
		curloptions["CURLOPT_MAIL_FROM"] = CURLOPT_MAIL_FROM;
		curloptions["CURLOPT_MAIL_RCPT"] = CURLOPT_MAIL_RCPT;
		curloptions["CURLOPT_MAXCONNECTS"] = CURLOPT_MAXCONNECTS;
		curloptions["CURLOPT_MAXFILESIZE"] = CURLOPT_MAXFILESIZE;
		curloptions["CURLOPT_MAXFILESIZE_LARGE"] = CURLOPT_MAXFILESIZE_LARGE;
		curloptions["CURLOPT_MAXREDIRS"] = CURLOPT_MAXREDIRS;
		curloptions["CURLOPT_MAX_RECV_SPEED_LARGE"] = CURLOPT_MAX_RECV_SPEED_LARGE;
		curloptions["CURLOPT_MAX_SEND_SPEED_LARGE"] = CURLOPT_MAX_SEND_SPEED_LARGE;
		curloptions["CURLOPT_NETRC"] = CURLOPT_NETRC;
		curloptions["CURLOPT_NETRC_FILE"] = CURLOPT_NETRC_FILE;
		curloptions["CURLOPT_NEW_DIRECTORY_PERMS"] = CURLOPT_NEW_DIRECTORY_PERMS;
		curloptions["CURLOPT_NEW_FILE_PERMS"] = CURLOPT_NEW_FILE_PERMS;
		curloptions["CURLOPT_NOBODY"] = CURLOPT_NOBODY;
		curloptions["CURLOPT_NOPROGRESS"] = CURLOPT_NOPROGRESS;
		curloptions["CURLOPT_NOPROXY"] = CURLOPT_NOPROXY;
		curloptions["CURLOPT_NOSIGNAL"] = CURLOPT_NOSIGNAL;
		curloptions["CURLOPT_OPENSOCKETDATA"] = CURLOPT_OPENSOCKETDATA;
		curloptions["CURLOPT_OPENSOCKETFUNCTION"] = CURLOPT_OPENSOCKETFUNCTION;
		curloptions["CURLOPT_PASSWORD"] = CURLOPT_PASSWORD;
		curloptions["CURLOPT_PORT"] = CURLOPT_PORT;
		curloptions["CURLOPT_POST"] = CURLOPT_POST;
		curloptions["CURLOPT_POSTFIELDS"] = CURLOPT_POSTFIELDS;
		curloptions["CURLOPT_POSTFIELDSIZE"] = CURLOPT_POSTFIELDSIZE;
		curloptions["CURLOPT_POSTFIELDSIZE_LARGE"] = CURLOPT_POSTFIELDSIZE_LARGE;
		curloptions["CURLOPT_POSTQUOTE"] = CURLOPT_POSTQUOTE;
		curloptions["CURLOPT_POSTREDIR"] = CURLOPT_POSTREDIR;
		curloptions["CURLOPT_PREQUOTE"] = CURLOPT_PREQUOTE;
		curloptions["CURLOPT_PRIVATE"] = CURLOPT_PRIVATE;
		curloptions["CURLOPT_PROGRESSDATA"] = CURLOPT_PROGRESSDATA;
		curloptions["CURLOPT_PROGRESSFUNCTION"] = CURLOPT_PROGRESSFUNCTION;
		curloptions["CURLOPT_PROTOCOLS"] = CURLOPT_PROTOCOLS;
		curloptions["CURLOPT_PROXY"] = CURLOPT_PROXY;
		curloptions["CURLOPT_PROXYAUTH"] = CURLOPT_PROXYAUTH;
		curloptions["CURLOPT_PROXYPASSWORD"] = CURLOPT_PROXYPASSWORD;
		curloptions["CURLOPT_PROXYPORT"] = CURLOPT_PROXYPORT;
		curloptions["CURLOPT_PROXYTYPE"] = CURLOPT_PROXYTYPE;
		curloptions["CURLOPT_PROXYUSERNAME"] = CURLOPT_PROXYUSERNAME;
		curloptions["CURLOPT_PROXYUSERPWD"] = CURLOPT_PROXYUSERPWD;
		curloptions["CURLOPT_PROXY_TRANSFER_MODE"] = CURLOPT_PROXY_TRANSFER_MODE;
		curloptions["CURLOPT_PUT"] = CURLOPT_PUT;
		curloptions["CURLOPT_QUOTE"] = CURLOPT_QUOTE;
		curloptions["CURLOPT_RANDOM_FILE"] = CURLOPT_RANDOM_FILE;
		curloptions["CURLOPT_RANGE"] = CURLOPT_RANGE;
		curloptions["CURLOPT_READDATA"] = CURLOPT_READDATA;
		curloptions["CURLOPT_READFUNCTION"] = CURLOPT_READFUNCTION;
		curloptions["CURLOPT_REDIR_PROTOCOLS"] = CURLOPT_REDIR_PROTOCOLS;
		curloptions["CURLOPT_REFERER"] = CURLOPT_REFERER;
		curloptions["CURLOPT_RESOLVE"] = CURLOPT_RESOLVE;
		curloptions["CURLOPT_RESUME_FROM"] = CURLOPT_RESUME_FROM;
		curloptions["CURLOPT_RESUME_FROM_LARGE"] = CURLOPT_RESUME_FROM_LARGE;
		curloptions["CURLOPT_RTSP_CLIENT_CSEQ"] = CURLOPT_RTSP_CLIENT_CSEQ;
		curloptions["CURLOPT_RTSP_REQUEST"] = CURLOPT_RTSP_REQUEST;
		curloptions["CURLOPT_RTSP_SERVER_CSEQ"] = CURLOPT_RTSP_SERVER_CSEQ;
		curloptions["CURLOPT_RTSP_SESSION_ID"] = CURLOPT_RTSP_SESSION_ID;
		curloptions["CURLOPT_RTSP_STREAM_URI"] = CURLOPT_RTSP_STREAM_URI;
		curloptions["CURLOPT_RTSP_TRANSPORT"] = CURLOPT_RTSP_TRANSPORT;
		curloptions["CURLOPT_SEEKDATA"] = CURLOPT_SEEKDATA;
		curloptions["CURLOPT_SEEKFUNCTION"] = CURLOPT_SEEKFUNCTION;
		curloptions["CURLOPT_SHARE"] = CURLOPT_SHARE;
		curloptions["CURLOPT_SOCKOPTDATA"] = CURLOPT_SOCKOPTDATA;
		curloptions["CURLOPT_SOCKOPTFUNCTION"] = CURLOPT_SOCKOPTFUNCTION;
		curloptions["CURLOPT_SOCKS5_GSSAPI_NEC"] = CURLOPT_SOCKS5_GSSAPI_NEC;
		curloptions["CURLOPT_SOCKS5_GSSAPI_SERVICE"] = CURLOPT_SOCKS5_GSSAPI_SERVICE;
		curloptions["CURLOPT_SSH_AUTH_TYPES"] = CURLOPT_SSH_AUTH_TYPES;
		curloptions["CURLOPT_SSH_HOST_PUBLIC_KEY_MD5"] = CURLOPT_SSH_HOST_PUBLIC_KEY_MD5;
		curloptions["CURLOPT_SSH_KEYDATA"] = CURLOPT_SSH_KEYDATA;
		curloptions["CURLOPT_SSH_KEYFUNCTION"] = CURLOPT_SSH_KEYFUNCTION;
		curloptions["CURLOPT_SSH_KNOWNHOSTS"] = CURLOPT_SSH_KNOWNHOSTS;
		curloptions["CURLOPT_SSH_PRIVATE_KEYFILE"] = CURLOPT_SSH_PRIVATE_KEYFILE;
		curloptions["CURLOPT_SSH_PUBLIC_KEYFILE"] = CURLOPT_SSH_PUBLIC_KEYFILE;
		curloptions["CURLOPT_SSLCERT"] = CURLOPT_SSLCERT;
		curloptions["CURLOPT_SSLCERTTYPE"] = CURLOPT_SSLCERTTYPE;
		curloptions["CURLOPT_SSLENGINE"] = CURLOPT_SSLENGINE;
		curloptions["CURLOPT_SSLENGINE_DEFAULT"] = CURLOPT_SSLENGINE_DEFAULT;
		curloptions["CURLOPT_SSLKEY"] = CURLOPT_SSLKEY;
		curloptions["CURLOPT_SSLKEYTYPE"] = CURLOPT_SSLKEYTYPE;
		curloptions["CURLOPT_SSLVERSION"] = CURLOPT_SSLVERSION;
		curloptions["CURLOPT_SSL_CIPHER_LIST"] = CURLOPT_SSL_CIPHER_LIST;
		curloptions["CURLOPT_SSL_CTX_DATA"] = CURLOPT_SSL_CTX_DATA;
		curloptions["CURLOPT_SSL_CTX_FUNCTION"] = CURLOPT_SSL_CTX_FUNCTION;
		curloptions["CURLOPT_SSL_SESSIONID_CACHE"] = CURLOPT_SSL_SESSIONID_CACHE;
		curloptions["CURLOPT_SSL_VERIFYHOST"] = CURLOPT_SSL_VERIFYHOST;
		curloptions["CURLOPT_SSL_VERIFYPEER"] = CURLOPT_SSL_VERIFYPEER;
		curloptions["CURLOPT_STDERR"] = CURLOPT_STDERR;
		curloptions["CURLOPT_TELNETOPTIONS"] = CURLOPT_TELNETOPTIONS;
		curloptions["CURLOPT_TFTP_BLKSIZE"] = CURLOPT_TFTP_BLKSIZE;
		curloptions["CURLOPT_TIMECONDITION"] = CURLOPT_TIMECONDITION;
		curloptions["CURLOPT_TIMEOUT"] = CURLOPT_TIMEOUT;
		curloptions["CURLOPT_TIMEOUT_MS"] = CURLOPT_TIMEOUT_MS;
		curloptions["CURLOPT_TIMEVALUE"] = CURLOPT_TIMEVALUE;
		curloptions["CURLOPT_TLSAUTH_PASSWORD"] = CURLOPT_TLSAUTH_PASSWORD;
		curloptions["CURLOPT_TLSAUTH_TYPE"] = CURLOPT_TLSAUTH_TYPE;
		curloptions["CURLOPT_TLSAUTH_USERNAME"] = CURLOPT_TLSAUTH_USERNAME;
		curloptions["CURLOPT_TRANSFERTEXT"] = CURLOPT_TRANSFERTEXT;
		curloptions["CURLOPT_TRANSFER_ENCODING"] = CURLOPT_TRANSFER_ENCODING;
		curloptions["CURLOPT_UNRESTRICTED_AUTH"] = CURLOPT_UNRESTRICTED_AUTH;
		curloptions["CURLOPT_UPLOAD"] = CURLOPT_UPLOAD;
		curloptions["CURLOPT_URL"] = CURLOPT_URL;
		curloptions["CURLOPT_USERAGENT"] = CURLOPT_USERAGENT;
		curloptions["CURLOPT_USERNAME"] = CURLOPT_USERNAME;
		curloptions["CURLOPT_USERPWD"] = CURLOPT_USERPWD;
		curloptions["CURLOPT_USE_SSL"] = CURLOPT_USE_SSL;
		curloptions["CURLOPT_VERBOSE"] = CURLOPT_VERBOSE;
		curloptions["CURLOPT_WILDCARDMATCH"] = CURLOPT_WILDCARDMATCH;
		curloptions["CURLOPT_WRITEDATA"] = CURLOPT_WRITEDATA;
		curloptions["CURLOPT_WRITEFUNCTION"] = CURLOPT_WRITEFUNCTION;
		curloptions["CURLOPT_SSL_OPTIONS"] = CURLOPT_SSL_OPTIONS;
		curloptions["CURLOPT_INTERFACE"] = CURLOPT_INTERFACE;
		curloptions["CURLOPT_MAIL_AUTH"] = CURLOPT_MAIL_AUTH;
		curloptions["CURLOPT_TCP_KEEPALIVE"] = CURLOPT_TCP_KEEPALIVE;
		curloptions["CURLOPT_TCP_KEEPIDLE"] = CURLOPT_TCP_KEEPIDLE;
		curloptions["CURLOPT_TCP_KEEPINTVL"] = CURLOPT_TCP_KEEPINTVL;
		curloptions["CURLOPT_TCP_NODELAY"] = CURLOPT_TCP_NODELAY;

#ifndef MAVERICK
		curloptions["CURLOPT_UNIX_SOCKET_PATH"] = CURLOPT_UNIX_SOCKET_PATH;
		curloptions["CURLOPT_XFERINFODATA"] = CURLOPT_XFERINFODATA;
		curloptions["CURLOPT_XFERINFOFUNCTION"] = CURLOPT_XFERINFOFUNCTION;
		curloptions["CURLOPT_XOAUTH2_BEARER"] = CURLOPT_XOAUTH2_BEARER;
		curloptions["CURLOPT_SSL_ENABLE_ALPN"] = CURLOPT_SSL_ENABLE_ALPN;
		curloptions["CURLOPT_SSL_ENABLE_NPN"] = CURLOPT_SSL_ENABLE_NPN;
		curloptions["CURLOPT_SSL_FALSESTART"] = CURLOPT_SSL_FALSESTART;
		curloptions["CURLOPT_SASL_IR"] = CURLOPT_SASL_IR;
		curloptions["CURLOPT_SERVICE_NAME"] = CURLOPT_SERVICE_NAME;
		curloptions["CURLOPT_PROXYHEADER"] = CURLOPT_PROXYHEADER;
		curloptions["CURLOPT_PATH_AS_IS"] = CURLOPT_PATH_AS_IS;
		curloptions["CURLOPT_PINNEDPUBLICKEY"] = CURLOPT_PINNEDPUBLICKEY;
		curloptions["CURLOPT_PIPEWAIT"] = CURLOPT_PIPEWAIT;
		curloptions["CURLOPT_LOGIN_OPTIONS"] = CURLOPT_LOGIN_OPTIONS;
		curloptions["CURLOPT_HEADEROPT"] = CURLOPT_HEADEROPT;
		curloptions["CURLOPT_DNS_INTERFACE"] = CURLOPT_DNS_INTERFACE;
		curloptions["CURLOPT_DNS_LOCAL_IP4"] = CURLOPT_DNS_LOCAL_IP4;
		curloptions["CURLOPT_DNS_LOCAL_IP6"] = CURLOPT_DNS_LOCAL_IP6;
		curloptions["CURLOPT_EXPECT_100_TIMEOUT_MS"] = CURLOPT_EXPECT_100_TIMEOUT_MS;
		curloptions["CURLOPT_PROXY_SERVICE_NAME"] = CURLOPT_PROXY_SERVICE_NAME;
		curloptions["CURLOPT_SSL_VERIFYSTATUS"] = CURLOPT_SSL_VERIFYSTATUS;
#endif
		init = true;
	}
}

//Replace curl by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType curl_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifcurl;
//This typedef defines a type "curlMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifcurl::*curlMethod)(KifElement*, KifDomain*, KifCallFunction*, int idthread);


size_t call_writing(char *ptr, size_t size, size_t nmemb, Kifcurl* userdata);

class Kifcurl : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, curlMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	KifElement* function;
	KifElement* object;
	KifDomain* dom;
	KifElement* context;
	string urlstr;
	CURL *curl;

	char* urlbuffer;
	int urlsize;

	//---------------------------------------------------------------------------------------------------------------------
	Kifcurl(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, curl_type) {
		curl = NULL;
		if (base->type != curl_type)
			//Do not forget your variable initialisation
			function = base->Robfunction(this);
		else
			curl = curl_easy_init();
		object = kifNULL;
		dom = NULL;
		context = NULL;
		urlsize = 2048;
		urlbuffer = (char*)malloc(urlsize);
	}

	~Kifcurl() {
		free(urlbuffer);
		if (curl != NULL)
			curl_easy_cleanup(curl);
	}

	//this function is called to test the kind of function we expect from the library with the KIFCURLSTRINGS[0] operator...
	bool testfunction(KifElement* func) {
		if (func->type == kifFunction) {
			KifFunction* kfunction = (KifFunction*)func;
			if (kfunction->arguments.size() != 2)
				return false;
			return true;
		}
		return false;
	}


	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		return true;
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return curl_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifcurl* kperso = new Kifcurl(KifCurrent(), this);
		kperso->function = function;
		//Your initialisations here
		return kperso;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	bool initialconstructor() {
		return true;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	KifElement* constructor(KifDomain* domloc, KifElement* params, int idthread, KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize() == 0)
			return this;

		//Your first parameter is at position 0 etc...		
		object = params->Instruction(0); //would match i as in the example above: mynewtype toto(i,j);
		if (object->type == kifInstance)
			object = object->Thevariable(domloc, idthread, NULL);
		else
			object = object->Exec(kifNULL, domloc, kifNULL, idthread, callfunc);
		object->Setreference();
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

	KifElement* errormsg(CURLcode res) {
		const char* errmsg = curl_easy_strerror(res);
		char ch[1024];
		sprintf(ch, "URL(%d): %s", res, errmsg);
		return kifcode->Returnerror(ch);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodProxy(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		string sproxy = callfunc->Evaluatethread(0, domain, idthread)->String();
		strcpy_s(urlbuffer, urlsize, STR(sproxy));
		CURLcode res = curl_easy_setopt(curl, CURLOPT_PROXY, urlbuffer);
		if (res == 0)
			return kifTRUE;
		return errormsg(res);
	}

	KifElement* MethodPWD(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		string user = callfunc->Evaluatethread(0, domain, idthread)->String();
		string pwd = callfunc->Evaluatethread(1, domain, idthread)->String();
		user += ":";
		user += pwd;

		strcpy_s(urlbuffer, urlsize, STR(user));
		CURLcode res = curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, urlbuffer);
		if (res == 0)
			return kifTRUE;
		return errormsg(res);
	}

	KifElement* MethodOptions(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		string option = callfunc->Evaluatethread(0, domain, idthread)->String();
		if (curloptions.find(option) == curloptions.end())
			return kifcode->Returnerror(KIFCURLSTRINGS[1]);

		CURLcode res;
		CURLoption noption = curloptions[option];
		KifElement*  kdata = callfunc->Evaluatethread(1, domain, idthread);
		if (kdata->type == kifString) {
			string data = kdata->String();
			if (data.size() >= urlsize) {
				free(urlbuffer);
				urlsize = data.size()*1.5;
				urlbuffer = (char*)malloc(urlsize);
			}

			strcpy_s(urlbuffer, urlsize, STR(data));
			res = curl_easy_setopt(curl, noption, urlbuffer);
		}
		else {
			long data = kdata->Integer();
			res = curl_easy_setopt(curl, noption, data);
		}

		if (res == 0)
			return kifTRUE;
		return errormsg(res);
	}

	KifElement* MethodURL(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		urlstr = callfunc->Evaluatethread(0, domain, idthread)->String();
		if (urlstr != "") {
			if (urlstr.size() >= urlsize) {
				free(urlbuffer);
				urlsize = urlstr.size()*1.5;
				urlbuffer = (char*)malloc(urlsize);
			}
			strcpy_s(urlbuffer, urlsize, STR(urlstr));
			curl_easy_setopt(curl, CURLOPT_URL, urlbuffer);
		}

		FILE* tmp = NULL;
		if (callfunc->Size() == 1) {
			if (function != NULL) {
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_writing);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
				dom = domain;
				context = contextualpattern;
			}
		}
		else {
			string filename = callfunc->Evaluatethread(1, domain, idthread)->String();
			tmp = fopen(STR(filename), KIFCURLSTRINGS[2]);
			if (tmp == NULL)
				return kifcode->Returnerror(KIFCURLSTRINGS[3]);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, tmp);
		}

		CURLcode res = curl_easy_perform(curl);
		if (tmp != NULL)
			fclose(tmp);
		if (res == 0)
			return kifTRUE;
		return errormsg(res);
	}

	KifElement* MethodExecute(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		FILE* tmp = NULL;
		if (callfunc->Size() == 0) {
			if (function != NULL) {
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_writing);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
				dom = domain;
				context = contextualpattern;
			}
		}
		else {
			string filename = callfunc->Evaluatethread(0, domain, idthread)->String();
			tmp = fopen(STR(filename), KIFCURLSTRINGS[2]);
			if (tmp == NULL)
				return kifcode->Returnerror(KIFCURLSTRINGS[3]);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, tmp);
		}

		CURLcode res = curl_easy_perform(curl);
		if (tmp != NULL)
			fclose(tmp);
		if (res == 0)
			return kifTRUE;
		return errormsg(res);
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFCURLSTRINGS[4]
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

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFCURLSTRINGS[5] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return urlstr; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifcurl::kifexportedmethods;
map<short, curlMethod>  Kifcurl::linkedmethods;
map<string, string> Kifcurl::infomethods;
hmap<short, bool> Kifcurl::methodassigned;

size_t call_writing(char *ptr, size_t size, size_t nmemb, Kifcurl* userdata) {
	int max = size*nmemb;
	string s;
	for (int i = 0; i < max; i++)
		s += ptr[i];
	KifFunction* func = userdata->function->Functioncall(userdata->dom);
	KifCallFunctionGeneral kfunc(NULL, NULL, func->Name(), func);
	kfunc.kifcode = func->kifcode;
	KifString* kstr = userdata->kifcode->Providestring(s);
	kstr->Setreference();
	userdata->object->Setreference();
	kfunc.parameters.push_back(kstr);
	kfunc.parameters.push_back(userdata->object);
	threadblocinit;
	kfunc.Exec(userdata->context, userdata->dom, kifNULL, idthread, callfunc);
	kstr->Resetreference();
	userdata->object->Resetreference();
	return max;
}

//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifcurlElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifcurl* local = new Kifcurl(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(KifCode* kifcode, string name, curlMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifcurl::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifcurl::linkedmethods[idname] = func;
	Kifcurl::methodassigned[idname] = true;
	Kifcurl::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifcurl(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << KIFCURLSTRINGS[6] << endl;
			return false;
		}

		kifcode->KifInitLib();

		if (kifGlobals->TestExternalType(KIFCURLSTRINGS[7]) == true)
			return false;

		///The new type will be: curl. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifcurlElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifcurl)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifcurlElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType(KIFCURLSTRINGS[7], CreatekifcurlElement);
		if (res == kifVOID)
			return false;
		curl_type = res;

		return true;
	}

	Exporting bool kifcurl_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifcurl(kifcode, version) == false)
			return false;

		curl_global_init(CURL_GLOBAL_DEFAULT);
		Init();
		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization(kifcode, KIFCURLSTRINGS[8], &Kifcurl::MethodProxy, P_ONE, KIFCURLSTRINGS[9]);
		MethodInitialization(kifcode, KIFCURLSTRINGS[10], &Kifcurl::MethodPWD, P_TWO, KIFCURLSTRINGS[11]);
		MethodInitialization(kifcode, KIFCURLSTRINGS[12], &Kifcurl::MethodURL, P_ONE | P_TWO, KIFCURLSTRINGS[13]);
		MethodInitialization(kifcode, KIFCURLSTRINGS[14], &Kifcurl::MethodExecute, P_NONE | P_ONE, KIFCURLSTRINGS[15]);
		MethodInitialization(kifcode, KIFCURLSTRINGS[16], &Kifcurl::MethodOptions, P_TWO, KIFCURLSTRINGS[17]);

		kifGlobals->kifAllFunctions[curl_type] = Kifcurl::methodassigned;
		kifGlobals->kifAllInformation[curl_type] = Kifcurl::infomethods;

		return true;
	}

	Exporting bool kifcurl_Terminate(KifCode* kifcode) {
		curl_global_cleanup();
		Kifcurl::kifexportedmethods.clear();
		Kifcurl::linkedmethods.clear();
		Kifcurl::methodassigned.clear();
		Kifcurl::infomethods.clear();
		return true;
	}
}


