/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.60
   filename   : sparse.h
   Date       : 10/09/2004
   Purpose    : Ontology implementation
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef sparse_h
#define sparse_h


#define gBitVector 64
#define gBitVectorDivide 6 // = 64=2^6

#ifdef WIN32
#define TVector unsigned __int64
#define STVector __int64
#else
#define TVector unsigned long long
#define STVector long long
#endif

const TVector One=1;

class itemsparse {
	
public:
    TVector word;
    short rank;
	
    itemsparse(int);
    itemsparse(itemsparse* g=NULL);
    itemsparse(itemsparse& g);
	itemsparse(int,TVector);  
};


class sparse {
	
public:
        
	VECTA<itemsparse*> filters;

	sparse();
    sparse(itemsparse*);
	sparse(sparse* v);
	sparse(sparse& v);
	
	~sparse();
	
	void reset();
	void or(sparse&);	
	void or(sparse*);
    void or(int);
	void or(itemsparse*);
	void or(itemsparse&);
	int count();
	void remove(int b);
    void table(VECTA<int>&);

	bool in(sparse*);
	bool in(sparse&);
    bool in(int index);
	void add(int index);
    int and(short r, TVector m,int start);
	void and(sparse*,sparse*);
	void and(sparse&,sparse&);
	void and(sparse&);
	void and(sparse*);
	void andnot(sparse*);
	void andnot(sparse&);
    bool same(sparse& v);
    bool same(sparse* v);
};


#endif
