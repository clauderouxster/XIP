// corpusStorage.cpp : Defines the entry point for the console application.
#include "general.h"
#include "agde.h"
#include "myconnect.h"
#include "xipvecta.h"

extern char server_machine[100];
extern char server_root[100];
extern char server_password[100];

#ifdef XIPDLL_EXPORT
__declspec(dllexport) int agde_analysis(char* grmFile, char* machine, char* database, char* user, char* pw , char* filename, char store_sentence, char cleanbase);
#endif

void CleanBase();
int number_bloc=0;

void Phrase(int ipar,XipResult* res,void* data) {
	agde* Agde=(agde*)data;
	static levelText* leTexte=NULL;
	char add_text=NO;
	if (leTexte==NULL) {
		add_text=YES;
	}
	string nom= Agde->current;
	//int valroot=res->sentences[0]->root->index;
	//int valleft=res->sentences[0]->root->left;
	//int valright=res->sentences[0]->root->right;
	//res->sentences[0]->root->index=valleft;
	for (int i=0;i<res->sentences.size();i++) {
		res->sentences[i]->root->index=0;
	}
	leTexte = Agde->extractInformation(*res,NULL,nom,0,1,leTexte,0,1);
	if (leTexte!=NULL && add_text==YES) {
		Agde->gTextes.ajoute(leTexte);
	}
	if (number_bloc && (number_bloc%500) == 0) {
		if (Agde->newConceptAdded==YES)
			Agde->SaveDocConcept();
		for (int i=0;i<Agde->gTextes.dernier;i++) {
			//TP- the standard store method has to be modified to store dependencies informations
			Agde->gTextes[i]->store(Agde);
		}
		Agde->storeconceptoffset();
		Agde->listeDesConcepts.raz();
		Agde->autoconcepts.nettoie();
		Agde->gTextes.nettoie();
		leTexte=NULL;
	}
	number_bloc++;
	Agde->tailleresult++;
	XipCleanCurrentXipResult(ipar);
}


int agde_analysis(char * grmFile, char* machine, char* database, char* user, char* mdp, char* filename,
 			char store_sentence, char cleanbase) {
	int i;
	cout <<"grmFile:" <<grmFile <<endl;
	cout <<" machine:" <<machine <<" database:" <<database <<" user:" <<user <<" password:" <<mdp <<endl;
	cout <<"filename:" <<filename <<" " <<(int)store_sentence <<" " <<(int)cleanbase <<endl;
	int grammar_index = XipGrmFile(grmFile, 0);
	strcpy(server_machine,machine);
	strcpy(server_root,user);
	strcpy(server_password,mdp);
	CreateConnection(database);
	levelText* leTexte=NULL;
	if (cleanbase==1) {
		CleanBase();
	}
	agde* Agde=new agde;
	Agde->current=filename;
	Agde->store_sentence=store_sentence;
	string thefilename=filename;
	
	XipResult* xipsplit=XipCreateCurrentXipResult(grammar_index);
	try{
		XipSetCallBack(grammar_index,Phrase,(void*)Agde);
		XipSetDisplayMode(grammar_index, DISPLAY_OFFSET|DISPLAY_LEMMA|DISPLAY_CHUNK_TREE);
		//We parse the file
		XipParseFile(thefilename,grammar_index,xipsplit,1);
		//On s'occupe du relicat
		if (Agde->newConceptAdded==YES) {
			Agde->SaveDocConcept();
		}
		for (i=0;i<Agde->gTextes.dernier;i++) {
			//TP- the standard store method has to be modified to store dependencies informations
			Agde->gTextes[i]->store(Agde);
		}
		Agde->storeconceptoffset();
		Agde->listeDesConcepts.raz();
		Agde->autoconcepts.nettoie();
		Agde->gTextes.nettoie();
	}
	catch (XIPError* message) {
		char errorch[1000];
		sprintf(errorch,"%s\r\n file: %s line:%d",message->message,message->filename,message->line);
		throw errorch;
		return -1;
	}
	leTexte = Agde->extractInformation(*xipsplit,NULL,filename,0,1,NULL);
	if (leTexte!=NULL) {
		Agde->SaveDocConcept();
		//TP- the standard store method has to be modified to store dependencies informations
		leTexte->store(Agde);
		delete leTexte;
	}
	delete Agde;
	Disconnection();
	return 0;
}
