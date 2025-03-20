// webservice.cpp : Defines the entry point for the console application.
//

#include <sstream>
#include <vector>
#include <map>
#include "httpget.h"
#include "httppost.h"

using std::string;
using std::vector;
using std::map;
using std::endl;
using std::cerr;
using std::cout;

#include "restservice.h"

#include "conversion.h"

//-------------------------------------------------------------------------------------------------
int jpg_post_handler(struct soap *soap);
int image_post_handler(struct soap *soap);
int text_post_handler(struct soap *soap);
int generic_POST_handler(struct soap *soap);
int POST_handler(struct soap*, const char*, const char*, int, const char*, const char*, size_t);

int generic_PUT_handler(struct soap *soap);
int generic_DELETE_handler(struct soap *soap);
//-------------------------------------------------------------------------------------------------
static struct http_post_handlers handlers[] =
{ { "image/jpg", jpg_post_handler },
{ "image/*", image_post_handler },
{ "image/*;*", image_post_handler },
{ "text/*", text_post_handler },
{ "text/*;*", text_post_handler },
{ "PUT", generic_PUT_handler },
{ "DELETE", generic_DELETE_handler },
{ NULL }
};

//-------------------------------------------------------------------------------------------------
static map<string, int> stringtypes;

static void initialize() {
	bool init = false;
	if (!init) {
		init = true;
		stringtypes["OK"] = SOAP_OK;
		stringtypes["STOP"] = SOAP_STOP;
		stringtypes["FORM"] = SOAP_FORM;
		stringtypes["HTML"] = SOAP_HTML;
		stringtypes["FILE"] = SOAP_FILE;
		stringtypes["POST"] = SOAP_POST;
		stringtypes["POST_FILE"] = SOAP_POST_FILE;
		stringtypes["GET"] = SOAP_GET;
		stringtypes["PUT"] = SOAP_PUT;
		stringtypes["DEL"] = SOAP_DEL;
		stringtypes["CONNECT"] = SOAP_CONNECT;
	}
}

static void send_error(struct soap* soap, char* buffer, char* entete) {
	strcpy_s(buffer, 2048,entete);
	soap_sprint_fault(soap, buffer + strlen(entete), 2048-strlen(buffer)-1);
}

Callrest::Callrest(void* caller) {
	kifrest = caller;
	soapptr = NULL;
	server = true;
	port = -1;
	initialize();
}

Callrest::~Callrest() {
	if (soapptr) {
		struct soap* soap = (struct soap*)soapptr;
		soap_end(soap);
		soap_done(soap);
	}
}

static void getfrom(struct soap* soap, map<string, string>& dico) {
	dico["body"] = "";
	dico["http_version"] = "";	/* HTTP version used "1.0" or "1.1" */
	dico["encodingStyle"] = "";	/* default = "" which means that SOAP encoding is used */
	dico["lang"] = "";		/* xml:lang attribute value of SOAP-ENV:Text */
	if (soap->http_version != NULL)
		dico["http_version"] = soap->http_version;	/* HTTP version used "1.0" or "1.1" */
	if (soap->encodingStyle != NULL)
	dico["encodingStyle"] = soap->encodingStyle;	/* default = "" which means that SOAP encoding is used */
	if (soap->lang != NULL)
		dico["lang"] = soap->lang;		/* xml:lang attribute value of SOAP-ENV:Text */
	if (soap->tag[0] != 0) dico["tag"] = soap->tag; else dico["tag"] = "";
	if (soap->id[0] != 0) dico["id"] = soap->id; else dico["id"] = "";
	if (soap->href[0] != 0) dico["href"] = soap->href; else dico["href"] = "";
	if (soap->type[0] != 0) dico["type"] = soap->type; else dico["type"] = "";
	if (soap->endpoint[0] != 0) dico["endpoint"] = soap->endpoint; else dico["endpoint"] = "";
	if (soap->path[0] != 0) dico["path"] = soap->path; else dico["path"] = "";
	if (soap->host[0] != 0) dico["host"] = soap->host; else dico["host"] = "";
	if (soap->session_host[0] != 0) dico["session_host"] = soap->session_host; else dico["session_host"] = "";
	char buff[100];
	_itoa_s(soap->port, buff, 10);
	dico["port"] = buff;
	_itoa_s(soap->session_port, buff, 10);
	dico["session_port"] = buff;
}

static int myget(struct soap* soap) {
	map<string, string> dico;
	getfrom(soap, dico);
	Callrest* cr = (Callrest*)soap->user;
	return cr->callget(dico);
}

bool Callrest::launch(std::string hst, int prt) {
	if (soapptr != NULL) {
		strcpy_s(buff, 2048, "RST(119): Error close the REST socket first");
		return false;
	}

	struct soap* soap = soap_new();
	soapptr = soap;

	soap_init(soap);
	/* chunking conserves memory and is generally faster: */
	soap_set_omode(soap, SOAP_IO_CHUNK);
	//soap_register_plugin_arg(soap, http_post, handlers);
	soap->fget = myget;

	soap->user = this;	
	memset(soap->tag, 0, SOAP_TAGLEN);
	memset(soap->id, 0, SOAP_TAGLEN);
	memset(soap->href, 0, SOAP_TAGLEN);
	memset(soap->type, 0, SOAP_TAGLEN);
	memset(soap->endpoint, 0, SOAP_TAGLEN);
	memset(soap->path, 0, SOAP_TAGLEN);
	memset(soap->host, 0, SOAP_TAGLEN);
	memset(soap->session_host, 0, SOAP_TAGLEN);

	SOCKET sserver = soap_bind(soap, hst.c_str(), prt, 100);
	if (!sserver) {
		send_error(soap, buff, "RST(131): LAUNCH=");
		return false;
	}
	port = prt;
	host = hst;
	return true;
}

bool Callrest::connect(std::string hst, int prt) {
	if (soapptr != NULL) {
		strcpy_s(buff, 2048, "RST(119): Error close the REST socket first");
		return false;
	}
	server = false;
	struct soap* soap = soap_new();
	soapptr = soap;
	soap_init(soap);
	memset(soap->tag, 0, SOAP_TAGLEN);
	memset(soap->id, 0, SOAP_TAGLEN);
	memset(soap->href, 0, SOAP_TAGLEN);
	memset(soap->type, 0, SOAP_TAGLEN);
	memset(soap->endpoint, 0, SOAP_TAGLEN);
	memset(soap->path, 0, SOAP_TAGLEN);
	memset(soap->host, 0, SOAP_TAGLEN);
	memset(soap->session_host, 0, SOAP_TAGLEN);

	/* chunking conserves memory and is generally faster: */
	soap_set_omode(soap, SOAP_IO_CHUNK);
	soap->fget = myget;
	soap->user = this;

	port = prt;
	host = hst;
	string endpoint = hostport();
	if (soap_connect(soap, endpoint.c_str(), NULL)) {
		send_error(soap, buff, "RST(132): CONNECT=");
		return false;
	}
	return true;
}

string Callrest::hostport() {
	std::ostringstream os;
	os << host << ":" << port;
	return os.str();
}

SOCKET Callrest::wait() {
	if (soapptr == NULL) {
		strcpy_s(buff, 2048, "RST(110): Error REST socket not opened");
		return false;
	}
	struct soap* soap = (struct soap*)soapptr;
	SOCKET sclient = soap_accept(soap);
	if (!sclient) {
		send_error(soap, buff, "RST(133): WAIT=");
		return false;
	}
	return sclient;
}

bool Callrest::get(map<string, string>& dico,int sclient) {
	if (soapptr == NULL) {
		strcpy_s(buff, 2048, "RST(110): Error REST socket not opened");
		return false;
	}

	struct soap* soap = (struct soap*)soapptr;
	SOCKET sx;
	if (server) {
		sx = soap->socket;
		soap->socket = sclient;
	}

	char* body = NULL;
	size_t len = 0;

	if (soap_begin_recv(soap)) {
		if (soap->error != SOAP_STOP) {
			if (server)
				soap->socket = sx;
			send_error(soap, buff, "RST(101):");
			return false;
		}
		soap->error = SOAP_OK;
	}

	body = soap_get_http_body(soap, &len);

	getfrom(soap, dico);
	if (body != NULL)
		dico["body"] = body;

	if (soap_end_recv(soap)) {
		if (server)
			soap->socket = sx;
		send_error(soap, buff, "RST(102):");
		return false;
	}
	if (server)
		soap->socket = sx;
	return true;
}

bool Callrest::put(string& tokens, string stype, bool empty,int sclient) {
	if (soapptr == NULL) {
		strcpy_s(buff, 2048, "RST(110): Error REST socket not opened");
		return false;
	}

	int type = 0;
	if (stringtypes.find(stype) == stringtypes.end()) {
		if (s_is_digit(stype))
			type = atoi(stype.c_str());
		else
			strcpy_s(buff, 2048, "RST(111): Unknown type. Accepted are: HTML, FILE, FORM, POST, DELETE, PUT or STOP");
		return false;
	}
	else
		type = stringtypes[stype];

	struct soap* soap = (struct soap*)soapptr;
	SOCKET sx;
	if (server) {
		sx = soap->socket;
		soap->socket = sclient;
	}

	if (empty || tokens.size()==0) {
		soap_send_empty_response(soap, type);
		if (server)
			soap->socket = sx;
		return true;
	}

	if (soap_response(soap, type)) {
		send_error(soap, buff, "RST(103):");
		if (server)
			soap->socket = sx;
		return false;
	}
	if (soap_send(soap, tokens.c_str())) {
		send_error(soap, buff, "RST(104):");
		if (server)
			soap->socket = sx;
		return false;
	}
	if (soap_end_send(soap)) {
		send_error(soap, buff, "RST(105):");
		if (server)
			soap->socket = sx;
		return false;
	}
	if (server)
		soap->socket = sx;
	return true;
}

bool Callrest::close() {
	if (soapptr == NULL) {
		strcpy_s(buff, 2048, "RST(110): Error REST socket not opened");
		return false;
	}
	struct soap* soap = (struct soap*)soapptr;
	if (soap_closesock(soap)) {
		send_error(soap, buff, "RST(107):");
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------
/* the jpg handler just responds with HTTP OK */
static int jpg_post_handler(struct soap *soap)
{
	map<string, string> dico;
	getfrom(soap, dico);

	char *buf;
	size_t len;
	soap_http_body(soap, &buf, &len);
	Callrest* cr = (Callrest*)soap->user;
	return cr->callpost("jpg", buf,dico);
}

/* the image handler responds with HTTP OK and a text/html body */
static int image_post_handler(struct soap *soap)
{
	map<string, string> dico;
	getfrom(soap, dico);

	char *buf;
	size_t len;
	soap_http_body(soap, &buf, &len);
	Callrest* cr = (Callrest*)soap->user;
	return cr->callpost("image", buf,dico);
}


/* the text handler copies the message back */
static int text_post_handler(struct soap *soap)
{
	map<string, string> dico;
	getfrom(soap, dico);

	char *buf;
	size_t len;
	soap_http_body(soap, &buf, &len);
	/* use current soap->http_content from HTTP header as return HTTP type */
	Callrest* cr = (Callrest*)soap->user;
	return cr->callpost("text", buf, dico);
}

/* the generic POST handler */
static int generic_POST_handler(struct soap* soap) {
	map<string, string> dico;
	getfrom(soap, dico);

	char *buf;
	size_t len;
	soap_http_body(soap, &buf, &len);
	Callrest* cr = (Callrest*)soap->user;
	return cr->callpost("POST", buf, dico);
}

static int POST_handler(struct soap* soap, const char* key, const char* val, int nb, const char* key2, const char* val2, size_t sz) {
	map<string, string> dico;
	getfrom(soap, dico);

	char *buf;
	size_t len;
	soap_http_body(soap, &buf, &len);
	Callrest* cr = (Callrest*)soap->user;
	return cr->callpost("POST", buf, dico);
}

/* the generic PUT handler */
static int generic_PUT_handler(struct soap *soap)
{
	map<string, string> dico;
	getfrom(soap, dico);

	char *buf;
	size_t len;
	soap_http_body(soap, &buf, &len);
	Callrest* cr = (Callrest*)soap->user;
	return cr->callpost("PUT", buf, dico);
}

/* the generic DELETE handler */
static int generic_DELETE_handler(struct soap *soap)
{
	map<string, string> dico;
	getfrom(soap, dico);

	Callrest* cr = (Callrest*)soap->user;
	return cr->callpost("DELETE", "", dico);
}

