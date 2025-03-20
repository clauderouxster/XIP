// webservice.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "soapH.h"
#include "soapKIF_USCOREBindingService.h"

SOAP_NMAC struct Namespace namespaces[] =
{
	{ "SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope", NULL },
	{ "SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/*/soap-encoding", NULL },
	{ "xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL },
	{ "xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL },
	{ "kifws", "urn:kifservice", NULL, NULL },
	{ NULL, NULL, NULL, NULL }
};


int KIF_USCOREBindingService::callKIF(std::string name, std::string param1, std::string param2, std::string param3, std::string &returned) {
	std::cout << name << ":" << param1 << ":" << param2 << ":" << param3 << std::endl;
	returned = "Termine";
	return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	KIF_USCOREBindingService lance;
	lance.run(8090);
	return 0;
}

