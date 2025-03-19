#ifndef xipclient_h
#define xipclient_h

#include "xipresult.h"

int XIPParseString(char* string,int port, char* hostname,char* parameters,XipResult&);
int XIPParseFile(char* filename,int port, char* hostname,char* parameters,XipResult&);
void XIPBuildParameter(char* parameters);
void XMLCodingTable(char* filename,char* field);


#endif
