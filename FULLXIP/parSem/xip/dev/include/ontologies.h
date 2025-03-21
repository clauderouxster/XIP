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
   filename   : ontologie.h
   Date       : 10/09/2004
   Purpose    : Ontology implementation
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef ontologie_h
#define ontologie_h


class granule {
	
public:
    TVecteur mot;
    short rang;
	
    granule(int);
    granule(granule* g=NULL);
    granule(granule& g);
	granule(int,TVecteur);  
};


class ontologie {
	
public:
        
	VECTA<granule*> element;	    

	ontologie();
    ontologie(granule*);
	ontologie(ontologie* v);
	ontologie(ontologie& v);
	
	~ontologie();
	
	void raz() {element.raz();}	
	void ou(ontologie&);	
	void ou(ontologie*);
    void ou(int);
	void ou(granule*);
	void ou(granule&);
	int compte();
	void retrait(int b);
    void table(VECTA<int>&);

	char present(ontologie*);
	char present(ontologie&);
    char present(int index);
    int et(short r, TVecteur m,int start);
	void et(ontologie*,ontologie*);
	void et(ontologie&,ontologie&);
	void et(ontologie&);
	void et(ontologie*);
	void etnon(ontologie*);
	void etnon(ontologie&);
    char egal(ontologie& v);
    char egal(ontologie* v);
    void ConcateneValeurs(GlobalParseur* parseur,VECTA<hierarchie*>& unehierarchie,Traits*,char*,char*);
	void ConcateneValeurs(GlobalParseur* parseur,VECTA<hierarchie*>& unehierarchie,Traits* tr,string& buffer,char*);
	void ConcateneValeurs(GlobalParseur* parseur,VECTA<hierarchie*>& unehierarchie,Traits* tr,vector<string>& reponses);
    void AfficheValeursXML(VECTA<hierarchie*>& unehierarchie,Traits*,ostream& os);
#ifdef XIPLIBXML
    void ConstruitXML(GlobalParseur*,VECTA<hierarchie*>& unehierarchie,xmlNodePtr node,Traits* test);
#endif
};


class hierarchie {
public:

    unsigned char* semme;
    ontologie onto;
    ontologie fils;
    ontologie descendants;
    int index;
    char tete;

    hierarchie(unsigned char* s,int i);
    ~hierarchie();

};

#endif
