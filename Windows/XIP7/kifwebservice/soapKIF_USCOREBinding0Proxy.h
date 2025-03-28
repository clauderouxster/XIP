/* soapKIF_USCOREBinding0Proxy.h
   Generated by gSOAP 2.8.22 from kifwebservice.h

Copyright(C) 2000-2015, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#ifndef soapKIF_USCOREBinding0Proxy_H
#define soapKIF_USCOREBinding0Proxy_H
#include "soapH.h"

class SOAP_CMAC KIF_USCOREBinding0Proxy
{ public:
	struct soap *soap;
	bool soap_own;
	/// Endpoint URL of service 'KIF_USCOREBinding0Proxy' (change as needed)
	const char *soap_endpoint;
	/// Variables globally declared in kifwebservice.h (non-static)
	/// Constructor
	KIF_USCOREBinding0Proxy();
	/// Constructor to use/share an engine state
	KIF_USCOREBinding0Proxy(struct soap*);
	/// Constructor with endpoint URL
	KIF_USCOREBinding0Proxy(const char *url);
	/// Constructor with engine input+output mode control
	KIF_USCOREBinding0Proxy(soap_mode iomode);
	/// Constructor with URL and input+output mode control
	KIF_USCOREBinding0Proxy(const char *url, soap_mode iomode);
	/// Constructor with engine input and output mode control
	KIF_USCOREBinding0Proxy(soap_mode imode, soap_mode omode);
	/// Destructor frees deserialized data
	virtual	~KIF_USCOREBinding0Proxy();
	/// Initializer used by constructors
	virtual	void KIF_USCOREBinding0Proxy_init(soap_mode imode, soap_mode omode);
	/// Delete all deserialized data (with soap_destroy and soap_end)
	virtual	void destroy();
	/// Delete all deserialized data and reset to default
	virtual	void reset();
	/// Disables and removes SOAP Header from message
	virtual	void soap_noheader();
	/// Get SOAP Header structure (NULL when absent)
	virtual	const SOAP_ENV__Header *soap_header();
	/// Get SOAP Fault structure (NULL when absent)
	virtual	const SOAP_ENV__Fault *soap_fault();
	/// Get SOAP Fault string (NULL when absent)
	virtual	const char *soap_fault_string();
	/// Get SOAP Fault detail as string (NULL when absent)
	virtual	const char *soap_fault_detail();
	/// Close connection (normally automatic, except for send_X ops)
	virtual	int soap_close_socket();
	/// Force close connection (can kill a thread blocked on IO)
	virtual	int soap_force_close_socket();
	/// Print fault
	virtual	void soap_print_fault(FILE*);
#ifndef WITH_LEAN
	/// Print fault to stream
#ifndef WITH_COMPAT
	virtual	void soap_stream_fault(std::ostream&);
#endif

	/// Put fault into buffer
	virtual	char *soap_sprint_fault(char *buf, size_t len);
#endif

	/// Web service operation 'callKIF0' (returns error code or SOAP_OK)
	virtual	int callKIF0(std::string name, std::string &returned) { return this->callKIF0(NULL, NULL, name, returned); }
	virtual	int callKIF0(const char *endpoint, const char *soap_action, std::string name, std::string &returned);

	/// Web service operation 'callKIF1' (returns error code or SOAP_OK)
	virtual	int callKIF1(std::string name, std::string param1, std::string &returned) { return this->callKIF1(NULL, NULL, name, param1, returned); }
	virtual	int callKIF1(const char *endpoint, const char *soap_action, std::string name, std::string param1, std::string &returned);

	/// Web service operation 'callKIF2' (returns error code or SOAP_OK)
	virtual	int callKIF2(std::string name, std::string param1, std::string param2, std::string &returned) { return this->callKIF2(NULL, NULL, name, param1, param2, returned); }
	virtual	int callKIF2(const char *endpoint, const char *soap_action, std::string name, std::string param1, std::string param2, std::string &returned);

	/// Web service operation 'callKIF3' (returns error code or SOAP_OK)
	virtual	int callKIF3(std::string name, std::string param1, std::string param2, std::string param3, std::string &returned) { return this->callKIF3(NULL, NULL, name, param1, param2, param3, returned); }
	virtual	int callKIF3(const char *endpoint, const char *soap_action, std::string name, std::string param1, std::string param2, std::string param3, std::string &returned);

	/// Web service operation 'callKIF4' (returns error code or SOAP_OK)
	virtual	int callKIF4(std::string name, std::string param1, std::string param2, std::string param3, std::string param4, std::string &returned) { return this->callKIF4(NULL, NULL, name, param1, param2, param3, param4, returned); }
	virtual	int callKIF4(const char *endpoint, const char *soap_action, std::string name, std::string param1, std::string param2, std::string param3, std::string param4, std::string &returned);

	/// Web service operation 'callKIF5' (returns error code or SOAP_OK)
	virtual	int callKIF5(std::string name, std::string param1, std::string param2, std::string param3, std::string param4, std::string param5, std::string &returned) { return this->callKIF5(NULL, NULL, name, param1, param2, param3, param4, param5, returned); }
	virtual	int callKIF5(const char *endpoint, const char *soap_action, std::string name, std::string param1, std::string param2, std::string param3, std::string param4, std::string param5, std::string &returned);

	/// Web service operation 'MethodsResponse' (returns error code or SOAP_OK)
	virtual	int MethodsResponse(std::string &returned) { return this->MethodsResponse(NULL, NULL, returned); }
	virtual	int MethodsResponse(const char *endpoint, const char *soap_action, std::string &returned);
};
#endif
