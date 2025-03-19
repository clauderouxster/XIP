#include "general.h"

#include "agde.h"

/*************************************************************************************/
int typePOS(string pos) {

	if (pos=="NAV")
		return 1;
	if (pos=="NOUN")
		return 1;
	if (pos=="VERB")
		return 1;
	if (pos=="ADJ")
		return 1;
	if (pos=="DEP")
		return 4;
	if (pos=="FEAT")
		return 5;
	if (pos=="MARKUP")
		return 6;
	if (pos=="ATTRIBUTE")
		return 7;
	if (pos=="PEID")
		return 8;

	return 1;
}

const char* typePOSCat(int pos) {

	switch(pos) {
	case 0:
		return "Other";
	case 1:
		return "NAV";
	case 4:
		return "DEP";
	case 5:
		return "FEAT";
	case 6:
		return "MARKUP";
	case 7:
		return "ATTRIBUTE";
	case 8:
		return "PEID";
	}
	return "";
}

/*************************************************************************************/


/*************************************************************************************/
entities::entities() {
	homonyme=NULL;
	nouveauConcept=0;
	nouveauFichier=0;
	occurence=0;
	cle="";
	reference=0;
	pos=-1;
}

entities::entities(agde& Agde,string mot,int cat,vecteur* c) {
	homonyme=NULL;
	reference=0;
	occurence=1;
	pos=cat;

	cle=mot;
	nouveauConcept=1;
	nouveauFichier=0;
	if (c==NULL) {
		reference=Agde.gNouveau_Vecteur;
		concept.element.ajoute(Agde.CreeConcept());
		Agde.listeDesConcepts.ajoute(this);
		Agde.indexedConceptList.affecte(reference,this);
	}
	else
		concept.merge(c);
}

entities::entities(agde& Agde,string mot,int cat,int id,int occ) {
	homonyme=NULL;
	nouveauConcept=0;
	nouveauFichier=0;
	occurence=occ;
	cle=mot;
	reference=id;
	pos=cat;
	concept.element.ajoute(new granulet(id));
	Agde.listeDesConcepts.ajoute(this);
	Agde.indexedConceptList.affecte(reference,this);
}

entities::~entities() {
	the_offsets.nettoie();
	if (homonyme)
		delete homonyme;
}

void entities::addoffset(int idsent,long left,long right) {
	char trouve=0;
	int i=the_offsets.dernier-1;
	while (i>=0 && the_offsets[i]->idsent==idsent) {
		if (the_offsets[i]->left==left && the_offsets[i]->right==right)
			return;
		i--;
	}

	the_offsets.ajoute(new offsets(idsent,left,right));
}


//we append the document number
void entities::addDoc(int doc) {
	//The doc number starts at 0
	//the 0 are not recorded in a vecteur
	// -1 means that no document should be appended to the list of documents
	if(doc == -1)
		return;
	granulet* g= new granulet(doc);
	documents.merge(g);
	nouveauFichier=1;
}

//we append the document number
void entities::addSent(int idsent) {
	//The doc number starts at 0
	//the 0 are not recorded in a vecteur
	// -1 means that no document should be appended to the list of documents
	if(idsent== -1)
		return;
	granulet* g= new granulet(idsent);
	documents.merge(g);
	nouveauFichier=1;
}


void entities::save(ofstream& file) {
	file<<"<key v=\""<<cle<<"\" ref=\""<<reference<<"\">"<<endl;

	file<<"<documents>"<<endl;
	documents.save(file);
	file<<"</documents>"<<endl;
	file<<"</key>"<<endl;
}


/*************************************************************************************/

