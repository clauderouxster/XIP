#ifndef general_h
#define general_h

#define YES 1
#define NO 0
#define TailleChaine 512
#define AUTOMATEPILE 2


#include <stdio.h>
#include <stdlib.h>
#include "xipvecta.h"

#include <iostream>
#include <strstream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

//typedef basic_string<char> string;

#include "sinput.h"

const int TAILLEVECTEUR=16;
const int SHIFTING = 4; //16 = 2^4

#ifdef WIN32
#define VECTEUR unsigned short
#define SVECTEUR short
#else
#define VECTEUR unsigned short
#define SVECTEUR short
#endif

//#define VECTEUR unsigned long long
//#define SVECTEUR long long

#define Rank(val) val >> SHIFTING;
#define BitPos(val,pos) UN << (val - (pos << SHIFTING)); 

const VECTEUR UN=1;

extern TansSinput sinput;

//compare deux chaines de caracteres sans tenir compte de la casse
inline char compch(unsigned char* mot1, unsigned char* mot2) {
	unsigned char c1=*mot1++;
	unsigned char c2=*mot2++;
	while (c1==c2 && c1 && c2) {
		c1=*mot1++;
		c2=*mot2++;
	}
	if (c1 < c2) {
		return -1;
	}
	if (c1 > c2) {
		return 1;
	}
	return 0;
}

#endif

