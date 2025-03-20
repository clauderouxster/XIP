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
filename   : graphe.cxx
Date       : 02/05/2007
Purpose    : Implementation of the graphe class to handle conceptual graph representation
Programmer : Claude ROUX
Reviewer   : 
*/


#include "parsbin.h"
#include "graphe.h"


/*
Nous implementons ici les graphes. Un graphe se declare avec une variable de type graph sous la forme d'une
description XML:

	<g name="...">
		<c id="..." i="..."> //i est une instance
			<r id="...">
				<c...>
			</r>
			<r id="..">
				...
		</c>
	</g>

	Un id doit etre prealable declare dans une hierarchie grelation ou gconcept

	les valeurs d'un id ou d'une instance sont:
		#1
		#1[trait]
		variable
		chaine
	L'interpretation et la construction se font a la volee
*/

//On lit jusqu'a la chaine: </g>;
graphe* GlobalParseur::litgraphe(istream& fichier) {
	uchar k;
	const char* final="</g>;";
	int f=0;
	xmlDocPtr doc;
	string gre;

	k=LectCrypte(fichier);
	while (!fichier.eof() && k!='<')
		k=LectCrypte(fichier);

	while (!fichier.eof()) {
		//On ne garde pas les caracteres de separation
		if (k=='\n' || k=='\r' || k=='\t') {
			k=LectCrypte(fichier);
			continue;
		}

		if (k==final[f])
			f++;
		else
			f=0;
		if (k==';' && f) {
			doc=xmlParseMemory((char*)gre.c_str(),gre.size());
			graphe* g=allouegraphe();
			map<string,int> partage;
			g->construit(fichier,doc->children,NULL,NULL,partage,'g');
			return g;
		}
		gre+=k;
		k=LectCrypte(fichier);
	}
	erreur("Bad Graph Format");
	return NULL;
}

char prop(xmlNodePtr node,string p, string& v) {
	xmlAttrPtr propriete=node->properties;
	while (propriete!=NULL) {
		string index=(char*)propriete->name;
		string valeur=(char*)propriete->children->content;
		if (index==p) {
			v=valeur;
			return OUI;
		}
		propriete=propriete->next;
	}
	return NON;
}


//On construit un graphe en lisant le contenu du doc
void graphe::construit(istream& fichier,xmlNodePtr noeud,gconcept* pereconc,
						grelation* pererel,map<string,int>& partage, char type) {
	
	if (noeud==NULL)
		return;

	char nodename=noeud->name[0];
	if (nodename!=type) {
		 if (nodename!='a' && type!='r') {
				affiche();
				parseur->erreur("Wrong graph structure");
		 }
	}
	
	if (nodename=='g') {
		//Il s'agit du nom du graphe, sa racine
		if (prop(noeud,"name",nom)==NON)
			parseur->erreur("Missing name attribute for the graph");
		construit(fichier,noeud->children,NULL,NULL,partage,'c');
		return;
	}

	string id;
	string instance;
	string laction;
	etat* e;
	char k;
	hierarchie* h;
	gconcept* c;
	grelation* r;
	string err;
	VECTA<char> variables_utilisees;
	TestFormule* tf;
	while (noeud!=NULL) {
		if (noeud->type==XML_TEXT_NODE) {
			noeud=noeud->next;
			continue;
		}
		nodename=noeud->name[0];
		if (nodename!=type) {
			if (nodename!='a' && type!='r') {
				affiche();
				parseur->erreur("Wrong graph structure");
			 }
		}
		if (nodename=='c') {			
			//Construction d'un concept
			//On recupere son id et son instance
			laction="";
			c=NULL;
			//Recherche d'un concept partage
			if (prop(noeud,"l",laction)) {
				c=lesconcepts[partage[laction]-1];
				if (c==NULL)
					partage[laction]=lesconcepts.dernier+1;
			}
			//si on n'a pas encore de concept...
			if (c==NULL) {
				if (prop(noeud,"id",id)==NON) {
					affiche();
					parseur->erreur("Missing ID on a CONCEPT node");
				}
				//On verifie ensuite que l'id existe vraiement
				e=parseur->concepts->ontologies.recherche((char*)id.c_str());
				if (e==NULL || e->valeur==NULL) {
					err="Unknown concept:";
					err+=id;
					parseur->erreur(STR(err));
				}
				h=(hierarchie*)e->valeur;
				//Si l'instance est proposee
				tf=NULL;
				if (prop(noeud,"i",instance)) {
					//dans le cas d'une formule
					if (instance[0]=='!') {
						k=' ';
						parseur->MetEnAttenteLecture(';',(char*)instance.c_str()+1);
						tf=parseur->LireUneFormule(fichier,k,variables_utilisees,2);
						instanciee=OUI;
					}
				}
				c=new gconcept(h,tf,instance);
				laction="";
				if (prop(noeud,"action",laction)) {
					if (ancre!=NULL)
						parseur->erreur("You cannot declare more than one ANCHOR node in a graph");
					ancre=c;
					if (laction=="anchor")
						c->action=ANCRAGE;
					else
						if (laction=="full")
							c->action=COPIEPLEINE;
				}

				//Pour l'instance, nous permettons trois cas:
				c->index=lesconcepts.dernier;
				lesconcepts.ajoute(c);
				indexconcepts.ou(h->onto);
			}
			
			if (pererel!=NULL)
				pererel->ajoute(c);
			if (racine==NULL)
				racine=c;

			construit(fichier,noeud->children,c,NULL,partage,'r');
		}
		else {
			if (nodename=='r') {
				if (prop(noeud,"id",id)==NON) {
					affiche();
					parseur->erreur("Missing ID on a RELATION node");
				}
				e=parseur->relations->ontologies.recherche((char*)id.c_str());
				if (e==NULL || e->valeur==NULL) {
					affiche();
					err+="Unknown relation:";
					err+=id.c_str();
					parseur->erreur(STR(err));
				}
				h=(hierarchie*)e->valeur;
				r=new grelation(h);
				r->index=lesrelations.dernier;
				lesrelations.ajoute(r);
				if (pereconc==NULL)
					parseur->erreur("Wrong relation");
				pereconc->ajoute(r);
				indexrelations.ou(h->onto);
				construit(fichier,noeud->children,NULL,r,partage,'c');
			}
			else {
				if (nodename=='a') {
					//On rajoute un graphe
					tf=NULL;
					if (prop(noeud,"ref",instance)) {
						//dans le cas d'une formule
						if (instance[0]!='!')
							parseur->erreur("A graph addendum needs a formula");
						k=' ';
						parseur->MetEnAttenteLecture(';',(char*)instance.c_str()+1);
						tf=parseur->LireUneFormule(fichier,k,variables_utilisees,2);
						instanciee=OUI;
						//On verifie certaine chose
						if (tf->TypeGraphe()==NON)
							parseur->erreur("The formula in the ref property should be a GRAPH variable");
					}
					else
						parseur->erreur("Missing ref in a 'a' node");
					//ce concept n'a pas de hconc...
					c=new gconcept(NULL,tf,instance);
					c->action=AJOUTGRAPHE;
					//Pour l'instance, nous permettons trois cas:
					c->index=lesconcepts.dernier;
					lesconcepts.ajoute(c);
					if (pererel!=NULL)
						pererel->ajoute(c);					
					if (racine==NULL)
						racine=c;
					construit(fichier,noeud->children,c,NULL,partage,'r');
				}
				else {				
					err="Unknown XML markup tag in a graph description";
					err+=nodename;
					parseur->erreur(STR(err));
				}
			}
		}
		noeud=noeud->next;
	}
}

void gconcept::ajoute(grelation* r) {
	int i=0;
	for (i=0;i<relations.dernier;i++) {
		if (r->hrel->index<relations[i]->hrel->index)
			break;
	}
	relations.insere(i,r);
	r->parent=this;
}

void grelation::ajoute(gconcept* c) {
	int i=0;
	for (i=0;i<concepts.dernier;i++) {
		if (c->hconc->index<concepts[i]->hconc->index)
			break;
	}
	concepts.insere(i,c);
	projete=NON;
	c->parents.ajoute(this);
}


gconcept* gconcept::clone(graphe* g,
						  VECTA<Noeud*>& noeuds,
                          char& erreur,
						  VECTA<ResultatFonction*>* focus) {
							  
	gconcept* c=NULL;
	if (parents.dernier>1)
		//c'est un noeud partage...
		//On verifie si c'est la premiere instance
		c=g->lesconcepts[index_clone];			
	
	//sinon, il faut le creer
	if (c==NULL) {
		index_clone=g->lesconcepts.dernier;
		c=new gconcept(hconc,NULL,"");
		if (formule!=NULL)
			formule->TraitementFormuleString(g->parseur,noeuds,erreur,c->instance,focus);
		else
			c->instance=instance;
		for (int i=0;i<relations.dernier;i++) { 
			grelation* r=relations[i]->clone(g,noeuds,erreur,focus);
			c->ajoute(r);
		}

		c->index=g->lesconcepts.dernier;
		g->lesconcepts.ajoute(c);
		g->indexconcepts.ou(hconc->onto);
		if (action==ANCRAGE)
			g->ancre=c;
	}
	return c;
}

grelation* grelation::clone(graphe* g,
						  VECTA<Noeud*>& noeuds,
                          char& erreur,
						  VECTA<ResultatFonction*>* focus) {

	grelation* r=new grelation(hrel);
	gconcept* c;
	for (int i=0;i<concepts.dernier;i++) { 
		if (concepts[i]->action==AJOUTGRAPHE) {
			//Il s'agit de la concatenation d'un graphe externe
			VECTA<graphe*> lesgraphes;
			concepts[i]->formule->TraitementFormuleGraphe(g->parseur,lesgraphes,noeuds,erreur,focus);
			for (int i=0;i<lesgraphes.dernier;i++) {
				//s'il y en a plus d'un ca va etre durail
				//On clone alors les concepts de ces graphes
				c=lesgraphes[i]->racine->clone(g,noeuds,erreur,focus);
				r->ajoute(c);
			}
		}
		else { 
			c=concepts[i]->clone(g,noeuds,erreur,focus);
			r->ajoute(c);
		}
	}

	r->index=g->lesrelations.dernier;
	g->lesrelations.ajoute(r);
	g->indexrelations.ou(hrel->onto);
	return r;
}

void graphe::razindexclone() {
	for (int i=0;i<lesconcepts.dernier;i++)
		lesconcepts[i]->index_clone=-1;
}

graphe* graphe::clone(VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* focus) {
	//dans le cas d'un graphe dont aucune instance ne doit etre calculee
	//On renvoie le graphe lui-meme, avec une reference incrementee
	if (instanciee==NON) {
		ref++;
		return this;
	}
	//Sinon, on clone ce graphe et on calcule ses instances...
	//On prepare le clonage
	razindexclone();
	graphe* g=parseur->allouegraphe();
	g->nom=nom+"_ins";
	if (racine!=NULL)
		g->racine=racine->clone(g,noeuds,erreur,focus);
	return g;
}

gconcept* gconcept::clone(graphe* g) {

	gconcept* c=NULL;
	if (parents.dernier>1)
		//c'est un noeud partage...
		//On verifie si c'est la premiere instance
		c=g->lesconcepts[index_clone];

	
	//sinon, il faut le creer
	if (c==NULL) {
		index_clone=g->lesconcepts.dernier;
		c=new gconcept(hconc,NULL,instance);
		for (int i=0;i<relations.dernier;i++) { 
			grelation* r=relations[i]->clone(g);
			c->ajoute(r);
		}

		c->index=g->lesconcepts.dernier;
		g->lesconcepts.ajoute(c);
		g->indexconcepts.ou(hconc->onto);
		if (action==ANCRAGE)
			g->ancre=c;
	}

	return c;
}

grelation* grelation::clone(graphe* g) {

	grelation* r=new grelation(hrel);
	gconcept* c;
	for (int i=0;i<concepts.dernier;i++) { 
		c=concepts[i]->clone(g);
		r->ajoute(c);
	}

	r->index=g->lesrelations.dernier;
	g->lesrelations.ajoute(r);
	g->indexrelations.ou(hrel->onto);
	return r;
}

char gconcept::affiche(ostream& os,string espace) {
	char retour=NON;
	string s;
	if (hconc!=NULL)
		s=(char*)hconc->semme;
	char chaine[10];
	sprintf(chaine,"%d",index);
	s+="(";
	s+=chaine;
	if (instance!="") {
		s+=":";
		s+=instance;
	}
	s+=")";
	os<<s;
	
	if (relations.dernier==0) {
		os<<Endl;
		return OUI;
	}
	
	string espplus=espace;
	for (size_t i=0;i<s.size();i++)
		espplus+=" ";
	
	for (int i=0;i<relations.dernier;i++) {
		if (i)
			os<<espplus;
		relations[i]->affiche(os,espplus);
		
	}
	
	return retour;
}


#ifdef XIPLIBXML
//creation d'une sortie XML
void grelation::sortieXml(GlobalParseur* parseur,xmlNodePtr parent) {
	xmlNodePtr r=parseur->CreationNoeudXML(parent,"r",NULL);
	//On rajoute notre concept
	parseur->AjouteAttributValeur(r,"id",(char*)hrel->semme);
	for (int i=0;i<concepts.dernier;i++) 
		concepts[i]->sortieXml(parseur,r);
}



void gconcept::sortieXml(GlobalParseur* parseur,xmlNodePtr parent) {
	xmlNodePtr c=parseur->CreationNoeudXML(parent,"c",NULL);
	//On rajoute notre concept
	if (hconc!=NULL)
		parseur->AjouteAttributValeur(c,"id",(char*)hconc->semme);
	if (instance!="")
		parseur->AjouteAttributValeur(c,"i",(char*)instance.c_str());
	for (int i=0;i<relations.dernier;i++) 
		relations[i]->sortieXml(parseur,c);
}

void graphe::sortieXml(xmlNodePtr parent) {
	//On cree d'abord le noeud superieur "g"
	xmlNodePtr g=parseur->CreationNoeudXML(parent,"g",NULL);
	parseur->AjouteAttributValeur(g,"name",(char*)nom.c_str());
	racine->sortieXml(parseur,g);
}

xmlDocPtr creeXMLGraphe() {
	char basedoc[]="<graphes/>";
	xmlDocPtr ref=xmlSAXParseMemory(NULL,basedoc,strlen(basedoc),0);	
	return ref;
}

#endif

XipRelation* grelation::ConstruitResultat() {
	XipRelation* xr=new XipRelation((char*)hrel->semme);
	for (int i=0;i<concepts.dernier;i++) {
		XipConcept* xc=concepts[i]->ConstruitResultat();
		xr->concepts.push_back(xc);
	}
	return xr;
}



XipConcept* gconcept::ConstruitResultat() {
	XipConcept* xc;
	if (hconc!=NULL)
		xc=new XipConcept((char*)hconc->semme,instance);
	else
		xc=new XipConcept("",instance);

	for (int i=0;i<relations.dernier;i++) {
		XipRelation* xr=relations[i]->ConstruitResultat();
		xc->relations.push_back(xr);
	}
	return xc;
}

XipGraph* graphe::ConstruitResultat() {
	//On cree d'abord le noeud superieur "g"
	XipGraph* xg=new XipGraph(nom);
	xg->root=racine->ConstruitResultat();
	return xg;
}

#ifdef XIPPYTHON

//On construit une representation Python du graphe
//La structure python va de la forme suivante:
//dictionaire: name, root
//root est compose de 
PyObject* pythonclone(graphe* g) {
	PyObject* grap=PyDict_New();
	if (g==NULL || g->racine==NULL)
		return grap;

	PyObject* dico;

	//On range le nom du dictionaire
    PyObject* pelement=PyString_FromString(g->nom.c_str());
    PyDict_SetItemString(grap, "name", pelement);
    Py_DECREF(pelement);
	//Ensuite on construit un element pour chaque relation
	int i,j;
	VECTA<PyObject*> rels;
	VECTA<PyObject*> concs;
	PyObject* liste;
	for (i=0;i<g->lesrelations.dernier;i++) {
		dico=PyDict_New();
		pelement=PyString_FromString("r");
		PyDict_SetItemString(dico,"type",pelement);
		Py_DECREF(pelement);
		pelement=PyString_FromString((char*)g->lesrelations[i]->hrel->semme);
		PyDict_SetItemString(dico,"id",pelement);
		Py_DECREF(pelement);
		rels.ajoute(dico);
	}

	for (i=0;i<g->lesconcepts.dernier;i++) {
		dico=PyDict_New();
		pelement=PyString_FromString("c");
		PyDict_SetItemString(dico,"type",pelement);
		Py_DECREF(pelement);
		if (g->lesconcepts[i]->hconc!=NULL)
			pelement=PyString_FromString((char*)g->lesconcepts[i]->hconc->semme);
		else
			pelement=PyString_FromString("#");
		PyDict_SetItemString(dico,"id",pelement);
		Py_DECREF(pelement);
		pelement=PyString_FromString(g->lesconcepts[i]->instance.c_str());
		PyDict_SetItemString(dico,"instance",pelement);
		Py_DECREF(pelement);
		liste=PyList_New(0);
		for (j=0;j<g->lesconcepts[i]->relations.dernier;j++) {
			pelement=rels[g->lesconcepts[i]->relations[j]->index];
			PyList_Insert(liste,j,pelement);
			Py_DECREF(pelement);
		}
		PyDict_SetItemString(dico,"relations",liste);
		Py_DECREF(liste);
		concs.ajoute(dico);
	}

	for (i=0;i<g->lesrelations.dernier;i++) {
		liste=PyList_New(0);
		for (j=0;j<g->lesrelations[i]->concepts.dernier;j++) {
			pelement=concs[g->lesrelations[i]->concepts[j]->index];
			PyList_Insert(liste,j,pelement);
			Py_DECREF(pelement);
		}
		PyDict_SetItemString(rels[i],"concepts",liste);
		Py_DECREF(liste);
	}

	PyDict_SetItemString(grap, "root", concs[g->racine->index]);
	Py_DECREF(concs[g->racine->index]);
	return grap;
}

#endif

char grelation::affiche(ostream& os,string espace){
	char retour=NON;
	os<<"-["<<hrel->semme<<"]";
	int max=strlen((char*)hrel->semme)+3;
	string espplus=espace;
	int i;
	for (i=0;i<max;i++)
		espplus+=" ";
	for (i=0;i<concepts.dernier;i++) {
		if (i)
			os<<espplus;
		os<<"-";
		concepts[i]->affiche(os,espplus);		
	}
	return retour;
}

void graphe::chaine(string& s) {
	ostringstream os;
	os<<nom<<"=";
	string espace="";
	for (size_t i=0;i<nom.size()+1;i++)
		espace+=" ";
	if (racine!=NULL)
		racine->affiche(os,espace);
	os<<Endl;
	s=os.str();
}
void graphe::affiche() {
	ostream* os=&cout;

	if (parseur->OS!=NULL)
		os=parseur->OS;
	*os<<nom<<"=";
	string espace="";
	for (size_t i=0;i<nom.size()+1;i++)
		espace+=" ";
	if (racine!=NULL)
		racine->affiche(*os,espace);
	*os<<Endl;
}


char grelation::compare(grelation* r) {
	if (r==NULL)
		return NON;
	if (r->hrel==hrel) {
		for (int i=0;i<concepts.dernier;i++) {
			if (!concepts[i]->compare(r->concepts[i]))
				return NON;
		}
	}
	return OUI;
}

char gconcept::compare(gconcept* c) {
	if (c==NULL)
		return NON;
	if (c->hconc==hconc && c->instance==instance) {
		for (int i=0;i<relations.dernier;i++) {
			if (!relations[i]->compare(c->relations[i]))
				return NON;
		}
	}
	return OUI;
}

char graphe::compare(graphe* g) {
	if (g==NULL)
		return NON;
	if (racine==NULL && g->racine!=NULL)
		return NON;
	return racine->compare(g->racine);
}

graphe* GlobalParseur::allouegraphe() {
	graphe* g=new graphe(this);
	g->index=premiergraphelibre;
	if (premiergraphelibre==listegraphes.dernier) {
		listegraphes.ajoute(g);
		premiergraphelibre=listegraphes.dernier;
	}
	else {
		listegraphes.affecte(premiergraphelibre,g);
		//on se positionne ensuite sur la premiere case vide
		while (premiergraphelibre<listegraphes.dernier && listegraphes[premiergraphelibre]!=NULL)
			premiergraphelibre++;
	}
	
	return g;
}

void GlobalParseur::retiregraphe(graphe* g) {
	if (g==NULL)
		return;
	g->ref--;
	if (g->ref==0) {
		premiergraphelibre=g->index;
		listegraphes.affecte(g->index,NULL);		
		//si c'est le dernier de la liste, alors on repositionne listegraphe.dernier
		if (premiergraphelibre==listegraphes.dernier-1)
			listegraphes.dernier=premiergraphelibre;
		delete g;
	}
}

char grelation::projection_concept(grelation* r,int i, int j) {
	if (i>=r->concepts.dernier)
		return OUI;

	if (j>= concepts.dernier)
		return NON;
	while (j<concepts.dernier && concepts[j]->projete==OUI)
		j++;
	if (j==concepts.dernier)
		return NON;

	char retour=NON;
	if (concepts[j]->projection(r->concepts[i])) {		
		concepts[j]->projete=OUI;
		retour=projection_concept(r,i+1,0);	
		concepts[j]->projete=NON;
	}
	if (retour==NON)
		return projection_concept(r,i,j+1);
	return OUI;
}

char grelation::projection(grelation* r) {
	if (r==NULL)
		return NON;
	
	//Faut qu'il y ait plus de relations courantes que dans g
	if (r->concepts.dernier>concepts.dernier)
		return NON;

	//On verifie la chose suivante:
	//tout d'abord il faut que les deux concepts soient compatibles:
	//Il faut que le concept c soit plus profond dans la hierarchie ou egal au concept courant
	if (hrel->onto.present(r->hrel->index)) {
		r->projeteur=index;
		//Ensuite on doit verifier les concepts
		char retour=projection_concept(r,0,0);
		if (retour==OUI)
			return retour;
	}
	return NON;
}


char gconcept::projection_relation(gconcept* c,int i, int j) {
	if (i>=c->relations.dernier)
		return OUI;

	if (j>= relations.dernier)
		return NON;
	while (j<relations.dernier && relations[j]->projete==OUI)
		j++;
	if (j==relations.dernier)
		return NON;

	char retour=NON;
	if (relations[j]->projection(c->relations[i])) {
		relations[j]->projete=OUI;
		retour=projection_relation(c,i+1,0);	
		relations[j]->projete=NON;
	}
	if (retour==NON)
		return projection_relation(c,i,j+1);
	return OUI;
}

char gconcept::projection(gconcept* c) {
	if (c==NULL)
		return NON;
	
	//Faut qu'il y ait plus de relations courantes que dans g
	if (c->relations.dernier>relations.dernier)
		return NON;

	if (hconc==NULL || c->hconc==NULL)
		return NON;
	//On verifie la chose suivante:
	//tout d'abord il faut que les deux concepts soient compatibles:
	//Il faut que le concept c soit plus profond dans la hierarchie ou egal au concept courant
	if (hconc->onto.present(c->hconc->index)) {
		//si instance existe, elles doivent etre les memes
		if (c->instance!="" && instance!=c->instance)
			return NON;
		c->projeteur=index;
		//Ensuite on doit verifier les relations
		char retour=projection_relation(c,0,0);
		if (retour==OUI)
			return retour;
	}
	return NON;
}

//Le graphe g doit se projeter sur le graphe courant
char graphe::projection(graphe* g) {
	if (g==NULL)
		return NON;
	if (racine==NULL && g->racine!=NULL)
		return NON;	
	//Les relations et les concepts de g doivent etre present dans le graphe courant
	if (!indexconcepts.present(g->indexconcepts) ||
		!indexrelations.present(g->indexrelations))
		return NON;
	//On cherche ensuite ou projeter
	for (int i=0;i<lesconcepts.dernier;i++) {
		if (lesconcepts[i]->projection(g->racine))
			return OUI;
	}
	return NON;
}

graphe* graphe::duplique(graphe* g) {
	graphe* rep=parseur->allouegraphe();
	rep->nom=g->nom+"_ext";

	gconcept* c;
	grelation* r;
	int i,j;
	//on parcourt notre liste de concepts projetes
	//On initialise la liste de concept de notre graphe d'ou seront issus les duplicats

	VECTA<int> listes;
	VECTA<gconcept*> listesconcepts;
	int ind;
	for (i=0;i<g->lesconcepts.dernier;i++) {
		ind=g->lesconcepts[i]->projeteur;
		//le champ projeteur contient l'index du noeud qui nous interesse
		c=new gconcept(lesconcepts[ind]->hconc,NULL,lesconcepts[ind]->instance);
		//On marque qu'il a ete consomme
		c->index=rep->lesconcepts.dernier;
		rep->lesconcepts.ajoute(c);
		rep->indexconcepts.ou(c->hconc->onto);
		//Cas particulier ou le concept de g est de type FULL...
		if (g->lesconcepts[i]->action==COPIEPLEINE) {
			listes.ajoute(ind);
			listesconcepts.ajoute(c);
		}
	}

	for (i=0;i<g->lesrelations.dernier;i++) {
		ind=g->lesrelations[i]->projeteur;
		//le champ projeteur contient l'index du noeud qui nous interesse
		r=new grelation(lesrelations[ind]->hrel);
		r->index=rep->lesrelations.dernier;
		rep->lesrelations.ajoute(r);
		rep->indexrelations.ou(r->hrel->onto);
		//On range aussi les concepts dans les relations
		for (j=0;j<g->lesrelations[i]->concepts.dernier;j++)
			r->ajoute(rep->lesconcepts[g->lesrelations[i]->concepts[j]->index]);			
	}
	//Enfin on range les relations aux bons endroits
	for (i=0;i<rep->lesconcepts.dernier;i++) {
		for (j=0;j<g->lesconcepts[i]->relations.dernier;j++)
			rep->lesconcepts[i]->ajoute(rep->lesrelations[g->lesconcepts[i]->relations[j]->index]);			
	}
	rep->racine=rep->lesconcepts[g->racine->index];
	//Enfin on traite les duplicats

	if (listes.dernier) {
		//On recopie alors l'integralite a partir d'un concept...
		for (i=0;i<listes.dernier;i++) {
			c=lesconcepts[listes[i]];
			razindexclone();
			for (ind=0;ind<c->relations.dernier;ind++) {
				r=c->relations[ind]->clone(rep);
				listesconcepts[i]->ajoute(r);
			}
		}
	}

	return rep;
}

void graphe::extraire(graphe* g,VECTA<graphe*>& resultats) {
	if (g==NULL)
		return;
	if (racine==NULL && g->racine!=NULL)
		return;	
	//Les relations et les concepts de g doivent etre present dans le graphe courant
	if (!indexconcepts.present(g->indexconcepts) ||
		!indexrelations.present(g->indexrelations))
		return;
	//On cherche ensuite ou projeter
	graphe* rep=NULL;
	int i;
	for (i=0;i<lesconcepts.dernier;i++) {
		if (lesconcepts[i]->projection(g->racine)) {
			//On alloue d'abord un graphe
			rep=duplique(g);
			resultats.ajoute(rep);
		}
	}
}

void grelation::marque(char type) {
	projete=type;
	for (int i=0;i<concepts.dernier;i++)
		concepts[i]->marque(type);
}

void gconcept::marque(char type) {
	projete=type;
	for (int i=0;i<relations.dernier;i++)
		relations[i]->marque(type);
}

void graphe::retiregraphe(gconcept* latete,graphe* reconnaitre,VECTA<grelation*>& gardes,char detruire) {
	int j,k;
	grelation* r;
	//dans ce cas, il faut retirer du graphe courant, les projections...
	//Il faut en revanche, faire attention:
	//a) a ne pas retirer la tete de projection
	//b) a ne par retirer des relations auxquelles sont encore accrochees des concepts
	//dans un premier temps, on recherche les relations pour lesquelles il reste
	//des concepts accrochees

	//On marque tous les concepts qui ont projete
	for (j=0;j<reconnaitre->lesconcepts.dernier;j++) {
		//On garde aussi la trace des concepts qui portent la marque full
		if (reconnaitre->lesconcepts[j]->action==COPIEPLEINE) 
			//On va aussi marquer les noeuds inferieurs
			lesconcepts[reconnaitre->lesconcepts[j]->projeteur]->marque(OUI);
		else
			lesconcepts[reconnaitre->lesconcepts[j]->projeteur]->projete=OUI;
	}

	//Il ne faut pas retirer la tete...
	latete->projete=NON;

	//On ne retire pas non plus les relations qui pointent vers des concepts
	//non retires	
	for (j=0;j<reconnaitre->lesrelations.dernier;j++) {
		//r est une relation qui a projete avec une relation de reconnaitre
		r=lesrelations[reconnaitre->lesrelations[j]->projeteur];
		r->projete=OUI;
		for (k=r->concepts.dernier-1;k>=0;k--) {
			if (r->concepts[k]->projete==NON)
				r->projete=NON;
			else
				r->concepts.retireElement(k);
		}
		//On les rajoute sur la racine de la projection
		if (r->projete==NON)
			gardes.ajoute(r);
	}
	//Le dernier cas est le suivant, lorsque des relations non choisies proviennent
	//de concepts selectionnes
	for (j=0;j<reconnaitre->lesconcepts.dernier;j++) {
		gconcept* c=lesconcepts[reconnaitre->lesconcepts[j]->projeteur];
		if (c->projete==OUI) {
			for (k=c->relations.dernier-1;k>=0;k--) {
				if (c->relations[k]->projete==NON)
					gardes.ajoute(c->relations[k]);
			}
		}
	}

	//Dans le cas d'une destruction de graphe, et que tout est detruit
	//alors la tete peut etre detruite
	if (detruire==OUI && gardes.dernier==0) {
		latete->projete=OUI;
		//On verifie aussi la relation parent
		for (int lp=0;lp<latete->parents.dernier;lp++) {
			//Si c'est le seul element, il faut aussi detruire la relation
			grelation* rp=latete->parents[lp];
			if (rp->concepts.dernier==1) {
				rp->projete=OUI;
				//On la retire de son propre parent
				gconcept* p=rp->parent;
				for (j=0;j<p->relations.dernier;j++) {
					if (p->relations[j]==rp) {
						p->relations.retireElement(j);
						break;
					}
				}
			}
			else {
				//on retire ce concept
				for (j=0;j<rp->concepts.dernier;j++) {
					if (latete==rp->concepts[j]) {
						rp->concepts.retireElement(j);
						break;
					}
				}
			}
		}
	}


	for (j=latete->relations.dernier-1;j>=0;j--) {
		if (latete->relations[j]->projete==OUI)
			latete->relations.retireElement(j);
	}

	//On nettoie tout d'un coup
	for (j=lesconcepts.dernier-1;j>=0;j--) {
		if (lesconcepts[j]->projete==OUI)
			delete lesconcepts.retireElement(j);
	}
	for (j=lesrelations.dernier-1;j>=0;j--) {
		if (lesrelations[j]->projete==OUI)
			delete lesrelations.retireElement(j);
	}

	//Puis on reindexe relations et concepts
	indexconcepts.raz();
	indexrelations.raz();
	for (j=lesconcepts.dernier-1;j>=0;j--) {
		lesconcepts[j]->index=j;
		indexconcepts.ou(lesconcepts[j]->hconc->onto);
	}

	for (j=lesrelations.dernier-1;j>=0;j--) {
		lesrelations[j]->index=j;
		indexrelations.ou(lesrelations[j]->hrel->onto);
	}
}

//remplacement d'une portion de graphe
char graphe::remplacement(graphe* reconnaitre, graphe* remplace) {
	if (reconnaitre==NULL || remplace==NULL || remplace->racine==NULL)
		return NON;
	if (racine==NULL && reconnaitre->racine!=NULL)
		return NON;
	//Les relations et les concepts de reconnaitre doivent etre present dans le graphe courant
	if (!indexconcepts.present(reconnaitre->indexconcepts) ||
		!indexrelations.present(reconnaitre->indexrelations))
		return NON;
	//On cherche ensuite ou projeter
	graphe* rep=NULL;
	int i;
	char boucle=OUI;
	char remp_effectue=NON;
	for (i=0;i<lesconcepts.dernier;i++)
		lesconcepts[i]->projeteur=-1;

	while (boucle) {
		boucle=NON;
		for (i=0;i<lesconcepts.dernier;i++) {
			//cas particulier, on ne veut pas reutiliser une tete deja utilisee, risque de bouclage
			if (lesconcepts[i]->projeteur==-2)
				continue;
			if (lesconcepts[i]->projection(reconnaitre->racine)) {
				gconcept* latete=lesconcepts[i];
				VECTA<grelation*> gardes;
				retiregraphe(latete,reconnaitre,gardes,NON);
				int j,k;
				char ajoute;
				grelation* r;

				//Par defaut l'ancre est la tete...
				ancre=latete;
				//La ensuite on rajoute le nouveau graphe...
				//On ecrase donc la tete avec les infos venant du nouveau graphe
				//Tout d'abord on verifie si la tete est la meme que le noeud de remplacement
				//pour conserver l'instance
				if (latete->hconc==remplace->racine->hconc) {
					//Si une instance est precisee, elle prend pas sur l'ancienne
					if (remplace->racine->instance!="")
						latete->instance=remplace->racine->instance;
				}
				else {//sinon, on ecrase tout
					latete->hconc=remplace->racine->hconc;
					latete->instance=remplace->racine->instance;
				}
				//on recopie alors le nouveau graphe, uniquement ses relations
				for (j=0;j<remplace->racine->relations.dernier;j++) {
					remplace->razindexclone();
					r=remplace->racine->relations[j]->clone(this);
					ajoute=OUI;
					for (k=0;k<latete->relations.dernier;k++) {
						if (latete->relations[k]->hrel==r->hrel) {
							ajoute=NON;
							//On fusionne alors le contenu de r dans latete
							for (int l=0;l<r->concepts.dernier;l++)
								latete->relations[k]->ajoute(r->concepts[l]);
							r->raz();
							r->projete=OUI;
							break;
						}
					}
					if (ajoute==OUI)
						latete->ajoute(r);
				}

				//On redistribue les relations epargnees sur le noeud d'ancrage...
				for (j=0;j<gardes.dernier;j++) {
					char ajoute=OUI;
					for (k=0;k<ancre->relations.dernier;k++) {
						if (ancre->relations[k]==gardes[j]) {
							ajoute=NON;
							break;
						}
					}
					if (ajoute==OUI)
						ancre->ajoute(gardes[j]);
				}
				
				//Pour eviter les bouclages intempestifs, on marque la tete...
				latete->projeteur=-2;
				boucle=OUI;
				remp_effectue=OUI;
				break;
			}
		}
	}
	return remp_effectue;
}

char graphe::detruitgraphe(graphe* patron) {
	if (patron==NULL)
		return NON;
	if (racine==NULL && patron->racine!=NULL)
		return NON;	
	//Les relations et les concepts de g doivent etre present dans le graphe courant
	if (!indexconcepts.present(patron->indexconcepts) ||
		!indexrelations.present(patron->indexrelations))
		return NON;
	//On cherche ensuite ou projeter
	char boucle=OUI;
	char destruction_effectue=NON;
	int i;
	for (i=0;i<lesconcepts.dernier;i++)
		lesconcepts[i]->projeteur=-1;
	while (boucle) {
		boucle=NON;
		for (i=0;i<lesconcepts.dernier;i++) {
			if (lesconcepts[i]->projeteur==-2 || racine==lesconcepts[i])
				continue;
			if (lesconcepts[i]->projection(patron->racine)) {
				gconcept* latete=lesconcepts[i];
				VECTA<grelation*> gardes;
				retiregraphe(latete,patron,gardes,OUI);
				boucle=OUI;
				destruction_effectue=OUI;
				if (gardes.dernier!=0)
					//si la tete a survecu, on la marque
					latete->projeteur=-2;

				break;
			}
		}
	}
	return destruction_effectue;
}


void VariableGraphe::affiche(ostream& os) {
	if (g!=NULL)
		g->affiche();
}        


void grelation::afficheXML(ostream& os) {
	os<<"<r id=\""<<hrel->semme<<"\"";
	if (concepts.dernier!=0) {
		os<<">"<<Endl;
		for (int i=0;i<concepts.dernier;i++) 
			concepts[i]->afficheXML(os);
		os<<"</r>"<<Endl;
	}
	else
		os<<"/>"<<Endl;
}


void gconcept::afficheXML(ostream& os) {
	os<<"<c ";
	if (hconc!=NULL)
		os<<"id=\""<<hconc->semme<<"\"";
	if (instance!="")
		os<<" i=\""<<instance.c_str()<<"\"";
	if (relations.dernier!=0) {
		os<<">"<<Endl;
		for (int i=0;i<relations.dernier;i++) 
			relations[i]->afficheXML(os);
		os<<"</c>"<<Endl;
	}
	else
		os<<"/>"<<Endl;
}



void VariableGraphe::afficheXML(ostream& os) {
	if (g==NULL)
		return;
	os<<"<g name=\""<<g->nom.c_str()<<"\">"<<Endl;
	g->racine->afficheXML(os);
	os<<"</g>"<<Endl;

}

void VariableGraphe::afficheNoeudXML(ostream& os,Noeud* n) {}

