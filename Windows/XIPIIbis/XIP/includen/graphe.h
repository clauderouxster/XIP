/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.61
   filename   : graphe.h
   Date       : 02/05/2007
   Purpose    : Definition of the GRAPHE class
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef graphe_h
#define graphe_h
#include "ontologies.h"
//Un graphe va etre defini par son nom, et un index sur ses relations et ses concepts

string& litgraphe(ifstream& fichier);

class grelation;
class XipRelation;
class XipConcept;
class XipGraph;
#ifdef XIPPYTHON
PyObject* pythonclone(graphe* g);
#endif

typedef enum {AUCUNE, AJOUTGRAPHE, ANCRAGE, COPIEPLEINE} actiongraphe;

class gconcept {
public:
	VECTA<grelation*> parents;
	hierarchie* hconc;
	TestFormule* formule;
	string instance;
	VECTA<grelation*> relations;
	char projete;
	int projeteur;
	actiongraphe action;
	int index;
	int index_clone;

	gconcept(hierarchie* i, TestFormule* tf,string s) {
		index_clone=-1;
		action=AUCUNE;
		projeteur=-1;
		index=-1;
		hconc=i;
		instance=s;
		projete=NON;
		formule=tf;
	}
	void ajoute(grelation* r);
	void marque(char);
	gconcept* clone(graphe* g,VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* focus);
	gconcept* clone(graphe* g);
	char affiche(ostream& os,string);
	char compare(gconcept*);
	char projection(gconcept*);
	char projection_relation(gconcept* c,int i, int j);
	XipConcept* ConstruitResultat();
	void raz() {
		relations.raz();
		parents.raz();
	}
#ifdef XIPLIBXML
	void sortieXml(GlobalParseur*,xmlNodePtr);
#endif
	void afficheXML(ostream& os);

};

class grelation {
public:

	gconcept* parent;
	hierarchie* hrel;
	char projete;
	int index;
	int projeteur;
	grelation(hierarchie* i) {
		parent=NULL;
		projeteur=-1;
		index=-1;
		hrel=i;
	}
	VECTA<gconcept*> concepts;
	void ajoute(gconcept* c);
	grelation* clone(graphe* g,VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* focus);
	grelation* clone(graphe* g);
	char affiche(ostream& os,string);
	void marque(char);
	char compare(grelation*);
	char projection(grelation*);
	char projection_concept(grelation* r,int i, int j);
	XipRelation* ConstruitResultat();
	void raz() {
		concepts.raz();
		parent=NULL;
	}
	void afficheXML(ostream& os);
#ifdef XIPLIBXML
	void sortieXml(GlobalParseur*,xmlNodePtr);
#endif

};


class graphe {
public:
	string nom;
	GlobalParseur* parseur;
	int index;
	int ref;
	char instanciee;

	ontologie indexconcepts;
	ontologie indexrelations;
	gconcept* racine;
	gconcept* ancre;
	VECTA<gconcept*> lesconcepts;
	VECTA<grelation*> lesrelations;
	
	graphe(GlobalParseur* p) {
		instanciee=NON;
		index=-1;
		ref=1;
		parseur=p;
		racine=NULL;
		ancre=NULL;
	}
	void affiche();
	~graphe() {
		lesconcepts.nettoie();
		lesrelations.nettoie();
	}
	void construit(istream&,xmlNodePtr noeud,gconcept* pere,grelation* pererel,map<string,int>&,char type);
	graphe* clone(VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* focus);
	void chaine(string&);
	char compare(graphe*);
	char projection(graphe*);
	void extraire(graphe*,VECTA<graphe*>&);
	graphe* duplique(graphe*);
	char remplacement(graphe* reconnaitre, graphe* remplace);
	void retiregraphe(gconcept* latete,graphe* reconnaitre,VECTA<grelation*>& gardes,char detruire);
	char detruitgraphe(graphe* patron);
	XipGraph* ConstruitResultat();
	void razindexclone();
#ifdef XIPLIBXML
	void sortieXml(xmlNodePtr);
#endif
};
#ifdef XIPLIBXML
xmlDocPtr creeXMLGraphe();
#endif
#endif

