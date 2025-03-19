#ifdef WIN32
#include <windows.h>
#endif
#include <iomanip>
#include <custom.h>

#include "general.h"
#include "agde.h"
#include "myconnect.h"

extern const int nextsent=20;
static Connection* bd=NULL;
static char peupleur[100000];
char server_machine[100];
char server_root[100];
char server_password[100];

/* Define some general constants */
#ifndef TRUE
#define TRUE            (1)     /* Logical true */
#define FALSE           (0)     /* Logical false */
#endif


void chainelong(SVECTEUR v,char* chiff) {
	int pos=2;
	strcpy(chiff,"0x");
	int val=0;
	for (int i=0;i<64;i++) {
		if (i && !(i%4)) {
			if (val>=10) {
				val+=55;
			} else {
				val+=48;
			}
			chiff[pos++]=val;
			val=0;
		}
		val*=2;
		if (v<0) {
			val++;
		}
		v<<=1;
	}
	//la derniere valeur doit etre prise en compte
	if (val>=10) {
		val+=55;
	} else {
		val+=48;
	}
	chiff[pos++]=val;
	chiff[pos]=0;
}

//Create the connection with the database...
void CreateConnection(char* database) {
	try {
		if (bd==NULL) {
			bd= new Connection(database, server_machine, server_root, server_password);
		}
	}
	//Connection("mots","","root","racine",4444,0,60,true,"/home/roux/DBserver/data/mysql.sock",0);
	catch(BadQuery &er) {
		cerr << "Error 1: " << er.what() << endl;
		throw er.what();
	}

}

void Disconnection() {
	cerr<<"Disconnection from the database..."<<endl;
	delete bd;
}

void LockTableConcepts() {
	Query query=bd->query();
	query.exec("LOCK TABLES concepts WRITE;");
}

void UnLockTableConcepts() {
	Query query=bd->query();
	query.exec("UNLOCK TABLES;");
}

void CleanBase() {
	Query query=bd->query();
	try {
		query.exec("delete from dependances;");
		query.exec("delete from concepts;");
		query.exec("delete from textsourceindexes;");
		query.exec("delete from sentencetoconcepts;");
		query.exec("delete from offsets;");
		query.exec("delete from concepttosentences;");
		query.exec("delete from sentences;");
		query.exec("delete from wordoffset;");
	}
	catch(BadQuery &er) {
		cerr << "Error 2: " << er.what() << endl;
	}

}

//First we count the number of concepts...
void agde::CompteConcepts() {
	Query query=bd->query();
	query << "select count(*) from concepts;";
	Result res = query.store();
	Result::iterator i=res.begin();
	Row row = *i;
	gNouveau_Vecteur=row.at(0);
}

int agde::NewIDSentence() {
	Query query=bd->query();
	query << "select count(*) from sentences;";
	Result res = query.store();
	Result::iterator i=res.begin();
	Row row = *i;
	return row.at(0);
}


//First we count the number of files...
void agde::CompteFichiers() {
	Query query=bd->query();
	query << "select count(*) from textsourceindexes;";
	Result res = query.store();
	Result::iterator i=res.begin();
	Row row = *i;
	nbFichiers=row.at(0);
}

int agde::ReloadDocConcept(entities* e) {
	Query query=bd->query();
	try {
		query << "select rank,value from concepttosentences where idconcept="<<e->reference<<";";
		Result res=query.store();
		//on regenere la chose
		if (!res.empty()) {
			//we return the id of that word
			Result::iterator i;
			for (i=res.begin(); i!=res.end();i++) {
				granulet* g=new granulet;
				Row row = *i;
				g->mot=row.at(1);
				g->rang=row.at(0);
				e->documents.element.ajoute(g);
			}
		}
	}
	catch(BadQuery &er) {
		cerr << "Error 3: " << er.what() << endl;
	}
	return 0;
}

int ReloadConceptVectors(entities* e) {
	Query query=bd->query();
	try {
		query << "select rank,value from concepttosentences where (idconcept="<<e->reference<<" and (";
		for (int ie=0;ie<e->documents.element.size();ie++) {
			if (ie) {
				query<<" or ";
			}
			query<<"rank="<<e->documents.element[ie]->rang;
		}
		query<<"));";
		Result res=query.store();
		//on regenere la chose
		if (!res.empty()) {
			//we return the id of that word
			Result::iterator i;
			for (i=res.begin(); i!=res.end();i++) {
				granulet* g=new granulet;
				Row row = *i;
				g->mot=row.at(1);
				g->rang=row.at(0);
				e->documents.merge(g);
			}
			//On detruit alors ces elements
			ostringstream oss;
			oss<< "delete from concepttosentences where (idconcept="<<e->reference<<" and (";
			for (int ie=0;ie<e->documents.element.size();ie++) {
				if (ie) {
					oss<<" or ";
				}
				oss<<"rank="<<e->documents.element[ie]->rang;
			}
			oss<<"));";
			query.exec(oss.str());
		}
	}
	catch(BadQuery &er) {
		cerr << "Error 4: " << er.what() << endl;
	}
	return 0;
}

void agde::ReloadAllDocConcept() {
	Query query=bd->query();
	query << "select idconcept,rank,value from concepttosentences;";
	Result res=query.store();
	entities* e;
	//on regenere la chose
	if (!res.empty()) {
		//we return the id of that word
		Result::iterator i;
		for (i=res.begin(); i!=res.end();i++) {
			granulet* g=new granulet;
			Row row = *i;
			g->mot=row.at(2);
			g->rang=row.at(1);
			e=indexedConceptList[(int)row.at(0)];
			e->documents.merge(g);
		}
	}
}

void agde::ReloadAllConcepts() {
	cerr<<"Loading Concepts"<<endl;
	Query query=bd->query();
	query << "select idconcept,word,occurence,pos from concepts;";
	Result res = query.store();
	string wd;
	if (!res.empty()) {
		//we return the id of that word
		Result::iterator i;
		Row row = *i;
		for (i=res.begin();i!=res.end();i++) {
			wd=row.at(1);
			int pos=row.at(3);
			int occ=row.at(2);
			int id =row.at(0);
			entities* c=new entities(*this,wd,pos,id,occ);
			etat* e=autoconcepts.recherche((unsigned char*)c->cle.c_str());
			if (e!=NULL) {
				entities* h=(entities*)e->valeur;
				c->homonyme=h->homonyme;
				h->homonyme=c;
			} else {
				autoconcepts.parse((unsigned char*)c->cle.c_str(),(void*)c);
			}
		}
		ReloadAllDocConcept();
	}
	cerr<<"Concepts Loaded"<<endl;
}

//We look for a concept... If we cannot find it we create it
int agde::ResearchAndAdd(string& chaine,int& pos,int& occ) {

	Query query=bd->query();
	query << "select idconcept,occurence from concepts where pos="<<pos<<" and word=\""<<chaine<<"\";";
	Result res = query.store();

	if (!res.empty()) {
		//we return the id of that word
		Result::iterator i=res.begin();
		Row row = *i;
		occ=row.at(1);
		return row.at(0);
	}
	occ=1;
	return -1;
}

//We look for a concept... If we cannot find it we create it
char agde::ResearchAndAdd(int iconc,string& word,int& pos,int& occ) {

	Query query=bd->query();
	query << "select word,occurence,pos from concepts where idconcept=\""<<iconc<<"\";";
	Result res = query.store();
	if (!res.empty()) {
		//we return the id of that word
		Result::iterator i=res.begin();
		Row row = *i;
		pos=row.at(2);
		occ=row.at(1);
		word=row.at(0);
		return 1;
	}
	return 0;
}

//We look for a concept... If we cannot find it we create it
void ResearchCatWord(const char* word,VECTA<int>& pos) {

	Query query=bd->query();
	query << "select pos from concepts where word=\""<<word<<"\";";
	Result res = query.store();
	if (!res.empty()) {
		//we return the id of that word
		Result::iterator i;
		for (i=res.begin();i<res.end();i++) {
			Row row = *i;
			int po=row.at(0);
			pos.ajoute(po);
		}
	}
}

void agde::LoadDocumentName(string& pathname, int idoc) {
	Query query=bd->query();
	query << "select pathname from textsourceindexes where idsource="<<idoc<<";";
	Result res = query.store();
	if (!res.empty()) {
		//we return the id of that word
		Result::iterator i=res.begin();
		Row row = *i;
		pathname= row.at(0);
	}
}

int agde::LookForDocument(string filename) {
	Query query=bd->query();
	char* nom=(char*)filename.c_str();
	int longueur=filename.size();
	char inter[1000];
	int pos=0;
	for (int i=0;i<longueur;i++) {
		if (nom[i]=='\\') {
			inter[pos++]='\\';
		}
		inter[pos++]=nom[i];
	}
	inter[pos]=0;
	query << "select idsource from textsourceindexes where pathname=\""<<inter<<"\";";
	Result res = query.store();
	if (!res.empty()) {
		//we return the id of that word
		Result::iterator i=res.begin();
		Row row = *i;
		return row.at(0);
	}
	//We add this new document to our table and we send the new document id
	sprintf(peupleur,"insert into textsourceindexes values (%d,'%s');",nbFichiers,inter);
	query.exec(peupleur);
	nbFichiers++;
	return (nbFichiers-1);
}

void StoreInXMLfile(int id,string title,string newid,unsigned long left,unsigned long right) {
	Query query=bd->query();
	query.exec("LOCK TABLES xmlfile WRITE;");
	sprintf(peupleur,"insert into xmlfile values (%d,%d,%d,\"%s\");",id,left,right,(char*)title.c_str());
	try {
		query.exec(peupleur);
	} catch(BadQuery &er) {
		cerr << "Error (concepts): " << er.what() << endl;
	}
	query.exec("UNLOCK TABLES;");
}

void agde::SaveDocConcept() {
	char inter[1000];
	int i;
	newConceptAdded=0;
	Query query=bd->query();
	cerr<<"Stockage fichier-concept"<<endl;
	query.exec("LOCK TABLES concepts WRITE;");
	for (i=0;i<listeDesConcepts.dernier;i++) {
		peupleur[0]=0;
		if (listeDesConcepts[i]!=NULL) {
			//s'il s'agit d'une creation d'un concept, on le rajoute dans la table
			if (listeDesConcepts[i]->nouveauConcept==1) {
				listeDesConcepts[i]->nouveauConcept=0;
				sprintf(peupleur,"insert into concepts values (%d,\"%s\",%d,%d);",
				        listeDesConcepts[i]->reference,
				        listeDesConcepts[i]->cle.c_str(),
				        listeDesConcepts[i]->occurence,
				        listeDesConcepts[i]->pos);
			} else {
				//sinon on le met a jour
				if (listeDesConcepts[i]->nouveauConcept==2) {
					sprintf(peupleur,"update concepts set occurence=%d where idconcept=%d",
					        listeDesConcepts[i]->occurence, listeDesConcepts[i]->reference);
				}
			}
			if (peupleur[0]) {
				try {
					query.exec(peupleur);
				} catch(BadQuery &er) {
					cerr << "Error (concepts): " << er.what() << endl;
				}
			}
		}
	}
	query.exec("LOCK TABLES concepttosentences WRITE;");
	for (i=0;i<listeDesConcepts.dernier;i++) {
		if (listeDesConcepts[i]!=NULL && listeDesConcepts[i]->nouveauFichier==1) {
			char debut=1;
			try {
				//On nettoie cette table d'abord pour ce concept
				ReloadConceptVectors(listeDesConcepts[i]);
				strcpy(peupleur,"insert into concepttosentences values ");
				for (int iv=0; iv< listeDesConcepts[i]->documents.element.dernier;iv++) {
					if (debut==1) {
						sprintf(inter,"(%d,%d,%u)",
						        listeDesConcepts[i]->reference,
						        listeDesConcepts[i]->documents.element[iv]->rang,
						        listeDesConcepts[i]->documents.element[iv]->mot);
						debut=0;
					} else {
						sprintf(inter,",(%d,%d,%u)", listeDesConcepts[i]->reference,
						        listeDesConcepts[i]->documents.element[iv]->rang, listeDesConcepts[i]->documents.element[iv]->mot);
					}
					strcat(peupleur,inter);
				}
				strcat(peupleur,";");
				query.exec(peupleur);
			}
			catch(BadQuery &er) {
				cerr << "Error(concepttosentences): " << er.what() << endl;
			}
		}
	}
	query.exec("UNLOCK TABLES;");
	cerr<<"Fin Stockage fichier-concept"<<endl;
}

int PositionInDocument(Iddoc* ph,long& gauche, long& droite,ostream& os) {
	try {
		Query query=bd->query();
		query << "select beginoffset,endoffset from offsets where (idsource="<<ph->idoc
		<<" and idpara ="<<ph->ipar<<" and idsent="<<ph->isent<<");";
		Result res=query.store();
		if (!res.empty()) {
			Result::iterator i=res.begin();
			Row row = *i;
			gauche=row.at(0);
			droite=row.at(1);
			return 1;
		}
	}
	catch(BadQuery &er) {
		os<< "Error 5: " << er.what() << "\r\n";
	}
	return 0;
}

int OffsetDocument(Iddoc* ph,long& gauche, long& droite,ostream& os) {
	try {
		Query query=bd->query();

		query << "select gauche,droit from xmlfile where (idsource="<<ph->idoc<<");";

		Result res=query.store();
		if (!res.empty()) {
			Result::iterator i=res.begin();
			Row row = *i;
			gauche=row.at(0);
			droite=row.at(1);
			return 1;
		}
	}
	catch(BadQuery &er) {
		os<< "Error 6: " << er.what() << "\r\n";
	}
	return 0;
}

//-----------------------------------------------------------------------------------------

int RequeteSurPhrase(int idoc,vecteur& key,int isent,
                     vecteur& v,
                     VECTA<int>& deps,
                     VECTA<int>& numdeps) {
	VECTEUR mot;
	try {
		Query query=bd->query();
		query << "select rank,value,iddep from dependances where "
		<<"idsource="<<idoc <<" and idsent ="<<isent <<" and (";
		for (int idep=0;idep<deps.dernier;idep++) {
			for (int i=0;i<key.element.dernier;i++) {
				granulet* g=key.element[i];
				if (idep || i) {
					query<<" OR ";
				}
				query<<"(iddep ="<<deps[idep] <<" and rank = "<<g->rang<< " and (value & "<<g->mot<<" = "<<g->mot<<"))";
			}
		}
		query<<") order by iddep;";
		Result res=query.store();
		int taille=res.size();
		if (!res.empty()) {
			Result::iterator i;
			int pos=0;
			int num=-1;
			int dep;
			int compte=0;
			//We only keep those that have at least the same number of elements as key
			for (i=res.begin();i<res.end();i++) {
				Row row = *i;
				dep=row.at(2);
				if (num!=dep) {
					compte=1;
					num=dep;
				} else {
					compte=compte+1;
				}
				if (compte==key.element.dernier) {
					numdeps.ajoute(dep);
				}
				pos=row.at(0);
				mot=row.at(1);
				v.merge(new granulet(pos,mot));
			}
			if (numdeps.dernier>0) {
				return 1;
			}
		}
	}
	catch(BadQuery &er) {
		cerr << "Error 7: " << er.what() << endl;
	}
	return 0;
}

void GetStringText(int idoc, vector<string>& sentence,ostream& os) {
	Query query=bd->query();
	query<<"select sentencetext from sentences where (idsource="<<idoc<<");";
	try {
		Result res=query.store();
		if (!res.empty()) {
			Result::iterator it;
			for (it=res.begin(); it!=res.end();it++) {
				Row row = *it;
				string s(row.at(0));
				sentence.push_back(s);
			}
		}
	}
	catch(BadQuery &er) {
		os<< "Error 32: " << er.what() << "\r\n";
	}
}


//----------------------------------------------------------------------------------
void RequeteBaseTexte(vecteur& key,VECTA<Iddoc*>& liste,char type,ostream& os) {
	if (!key.element.dernier)
		return;
	try {
		int i;
		granulet* g;
		Query query=bd->query();
		if (key.element.dernier==1) {
			g=key.element[0];
			query << "select idsource from sentencetoconcepts where idpara=-1 and rank = "<<g->rang
			<< " and (value & "<<g->mot<<" = "<<g->mot<<");";
		} else {
			query<<"select d0.idsource from sentencetoconcepts as d0 inner join sentencetoconcepts as d1 using (idsource)";
			for (i=2;i<key.element.dernier;i++) {
				query<<", sentencetoconcepts as d"<<i;
			}
			granulet* g0=key.element[0];
			granulet* g1=key.element[1];
			query<<" where (";
			for (i=2;i<key.element.dernier;i++) {
				g=key.element[i];
				query<<"d"<<i<<".idsource = d0.idsource and "
				<<"d"<<i<<".idpara=-1 and ";
			}
			query<<"d0.idpara=-1 and "
			<<"d0.rank = "<<g0->rang<<" and "
			<<"d1.rank = "<<g1->rang<<" and ";
			for (i=2;i<key.element.dernier;i++) {
				g=key.element[i];
				query<<"d"<<i<<".rank = "<<g->rang<<" and ";
			}
			query<<"(d0.value & "<<g0->mot<<") = "<<g0->mot<<" and " <<"(d1.value & "<<g1->mot<<") = "<<g1->mot;
			for (i=2;i<key.element.dernier;i++) {
				g=key.element[i];
				query<<" and (d"<<i<<".value & "<<g->mot<<") = "<<g->mot;
			}
			query<<");";
		}
		Result res=query.store();
		if (!res.empty()) {
			Result::iterator i;
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				int idoc=row.at(0);
				char trouve=0;
				for (int il=0;il<liste.dernier;il++) {
					if (liste[il]->idoc==idoc) {
						trouve=1;
						break;
					}
				}
				if (!trouve) {
					liste.ajoute(new Iddoc(type,idoc,-1,-1));
				}
			}
		}
	}
	catch(BadQuery &er) {
		os<< "Error 8: " << er.what() << "\r\n";
	}

}
//----------------------------------------------------------------------------------
void RequeteBaseParagraphe(vecteur& key,VECTA<Iddoc*>& liste,char type,ostream& os) {
	if (!key.element.dernier) {
		return;
	}
	try {
		int i;
		granulet* g;
		Query query=bd->query();
		if (key.element.dernier==1) {
			g=key.element[0];
			query << "select idsource,idpara from sentencetoconcepts where idpara!=-1 and idsent=-1 and rank = "<<g->rang
			<< " and (value & "<<g->mot<<" = "<<g->mot<<");";
		} else {
			int last=key.element.dernier-1;
			granulet* g0=key.element[last];
			granulet* g1=key.element[last-1];
			last=last-2;
			int max=0;
			query<<"select d0.idsource,d0.idpara from sentencetoconcepts"
			<<" as d0 inner join sentencetoconcepts as d1 using (idsource,idpara)";
			for (i=last;i>=max;i--) {
				query<<", sentencetoconcepts as d"<<last-i+2;
			}
			query<<" where (";
			query<<"d0.idpara!=-1 and d0.idsent=-1 and " <<"d0.rank = "<<g0->rang<<" and " <<"d1.rank = "<<g1->rang<<" and ";
			for (i=last;i>=max;i--) {
				g=key.element[i];
				query<<"d"<<last-i+2<<".idsource=d0.idsource and "
				<<"d"<<last-i+2<<".idpara=d0.idpara and ";
			}
			for (i=last;i>=max;i--) {
				g=key.element[i];
				query<<"d"<<last-i+2<<".idpara = d0.idpara and "
				<<"d"<<last-i+2<<".idsent = -1 and ";
			}
			for (i=last;i>=max;i--) {
				g=key.element[i];
				query<<"d"<<last-i+2<<".rank = "<<g->rang<<" and " ;
			}
			query<<"(d0.value & "<<g0->mot<<") = "<<g0->mot<<" and "
			<<"(d1.value & "<<g1->mot<<") = "<<g1->mot;
			for (i=last;i>=max;i--) {
				g=key.element[i];
				query<<" and (d"<<last-i+2<<".value & "<<g->mot<<") = "<<g->mot;
			}
			query<<");";
		}
		Result res=query.store();
		if (!res.empty()) {
			Result::iterator i;
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				int idoc=row.at(0);
				int idpara=row.at(1);
				char trouve=0;
				for (int il=0;il<liste.dernier;il++) {
					if (liste[il]->idoc==idoc &&
					                liste[il]->ipar==idpara) {
						trouve=1;
						break;
					}
				}
				if (!trouve)  {
					liste.ajoute(new Iddoc(type,idoc,idpara,-1));
				}
			}
		}
	}
	catch(BadQuery &er) {
		os<< "Error 9: " << er.what() << "\r\n";
	}
}


void RequestForOneDocument(int docid,VECTA<Iddoc*>& liste,granulet* g,char type,ostream& os) {
	Query query=bd->query();
	query<<"select idsource,idpara,idsent from sentencetoconcepts where (idsource="<<docid
	<<" and idsent!=-1 and "<<"rank = "<<g->rang<<" and (value & "<<g->mot<<") = "<<g->mot<<");";
	Result res=query.store();
	if (!res.empty()) {
		os<<"Taille:"<<res.size()<<Endl;
		Result::iterator i;
		int rangj=0;
		for (i=res.begin(); i!=res.end();i++) {
			Row row = *i;
			int idoc=row.at(0);
			int ipara=row.at(1);
			int idsent=row.at(2);
			char trouve=0;
			for (int il=0;il<liste.dernier;il++) {
				if (liste[il]->idoc==idoc && liste[il]->isent==idsent) {
					trouve=1;
					break;
				}
			}
			if (!trouve) {
				liste.ajoute(new Iddoc(type,idoc,ipara,idsent));
			}
		}
	}
}

//----------------------------------------------------------------------------------
void RequeteBasePhraseWithDocuments(vecteur& key,VECTA<Iddoc*>& liste,vector<int>& documents,char type,ostream& os) {
	int i;
	if (!key.element.dernier)
		return;
	try {
		int last = key.element.dernier-1;
		granulet* g0=key.element[last];
		granulet* g1=key.element[last-1];
		last=last-2;
		if (g1==NULL) {
			for (i=0;i<documents.size();i++)
				RequestForOneDocument(documents[i],liste,g0,type,os);

			return;
		}
		char ch[100];
		string docid="d0.idsource IN (";
		for (i=0;i<documents.size();i++) {
			sprintf(ch,"%d",documents[i]);
			if (i) {
				docid+=",";
			}
			docid+=ch;
		}
		docid+=") and ";
		granulet* g;
		Query query=bd->query();
		query<<"select d0.idsource,d0.idpara,d0.idsent from sentencetoconcepts as d0 inner join sentencetoconcepts as d1 using (idsource,idsent)";
		for (i=last;i>=0;i--) {
			query<<", sentencetoconcepts as d"<<last-i+2;
		}
		query<<" where ("<<docid;
		for (i=last;i>=0;i--) {
			g=key.element[i];
			query<<"d"<<last-i+2<<".idsource = d0.idsource and d"<<last-i+2<<".idsent=d0.idsent and ";
		}
		query<<"d0.idsent!=-1 and "
		<<"d0.rank = "<<g0->rang;
		if (g1!=NULL) {
			query<<" and d1.rank = "<<g1->rang;
		}
		for (i=last;i>=0;i--) {
			g=key.element[i];
			query<<" and d"<<last-i+2<<".rank = "<<g->rang;
		}
		query<<" and (d0.value & "<<g0->mot<<") = "<<g0->mot;
		if(g1!=NULL) {
			query<<" and (d1.value & "<<g1->mot<<") = "<<g1->mot;
		}
		for (i=last;i>=0;i--) {
			g=key.element[i];
			query<<" and (d"<<last-i+2<<".value & "<<g->mot<<") = "<<g->mot;
		}
		query<<");";
		Result res=query.store();
		if (!res.empty()) {
			os<<"Taille:"<<res.size()<<Endl;
			Result::iterator i;
			int rangj=0;
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				int idoc=row.at(0);
				int ipara=row.at(1);
				int idsent=row.at(2);
				char trouve=0;
				for (int il=0;il<liste.dernier;il++) {
					if (liste[il]->idoc==idoc &&
					                liste[il]->isent==idsent) {
						trouve=1;
						break;
					}
				}
				if (!trouve) {
					liste.ajoute(new Iddoc(type,idoc,ipara,idsent));
				}
			}
		}
	}
	catch(BadQuery &er) {
		os<< "Error 10: " << er.what() << "\r\n";
	}
}

int LoadOneDocument(int sentid,VECTA<Iddoc*>& liste,char type,ostream& os, int& docnum) {
	Query query=bd->query();
	query<<"select idsource,idpara,sentencetext from sentences where idsent="<<sentid<<";";
	Result res=query.store();
	int doci=-1;
	if (!res.empty()) {
		Result::iterator i;
		int rangj=0;
		for (i=res.begin(); i!=res.end();i++) {
			Row row = *i;
			int idoc=row.at(0);
			int ipara=row.at(1);
			char trouve=0;
			if (docnum==idoc) {
				return 0;
			}
			doci=idoc;
			for (int il=0;il<liste.dernier;il++) {
				if (liste[il]->idoc==idoc &&
				                liste[il]->isent==sentid) {
					trouve=1;
					break;
				}
			}
			if (!trouve) {
				liste.ajoute(new Iddoc(type,idoc,ipara,sentid));
				liste.fin()->sentence_text=row.at(2);
			}
		}
	}
	docnum=doci;
	return 1;
}

//----------------------------------------------------------------------------------
void RequeteBasePhraseWithSentences(int firstsentence, int lastsentence, VECTA<Iddoc*>& liste,
                                    vector<int>& sentences, char type, ostream& os) {
	try {
		int doc=-1;
		for (int i=firstsentence;i<lastsentence;i++) {
			doc=-1;
			LoadOneDocument(sentences[i],liste,type,os,doc);
		}
	}
	catch(BadQuery &er) {
		os<< "Error 11: " << er.what() << "\r\n";
	}
}

void RequeteBasePhraseWithNoSentences(int firstsentence, int lastsentence, VECTA<Iddoc*>& liste,
                                      vector<int>& sentences, char type, ostream& os) {
	try {
		int doc=-1;
		int nbdoc=0;
		for (int i=firstsentence;i<sentences.size();i++) {
			if (LoadOneDocument(sentences[i],liste,type,os,doc)==1) {
				if (nbdoc>=lastsentence) {
					return;
				}
				nbdoc++;
			}
		}
	}
	catch(BadQuery &er) {
		os<< "Error 12: " << er.what() << "\r\n";
	}
}


//----------------------------------------------------------------------------------
void RequeteBasePhrase(vecteur& key,VECTA<Iddoc*>& liste,char type,ostream& os) {
	if (!key.element.dernier) {
		return;
	}
	try {
		int i;
		granulet* g;
		Query query=bd->query();
		if (key.element.dernier==1) {
			g=key.element[0];
			query << "select idsource,idpara,idsent from sentencetoconcepts where idsent!=-1 and rank = "<<g->rang
			<< " and (value & "<<g->mot<<" = "<<g->mot<<");";
		} else {
			query<<"select d0.idsource,d0.idpara,d0.idsent from sentencetoconcepts as d0 inner join sentencetoconcepts as d1 using (idsource,idsent)";
			int last = key.element.dernier-1;
			granulet* g0=key.element[last];
			granulet* g1=key.element[last-1];
			last=last-2;
			for (i=last;i>=0;i--) {
				query<<", sentencetoconcepts as d"<<last-i+2;
			}
			query<<" where (";
			for (i=last;i>=0;i--) {
				g=key.element[i];
				query<<"d"<<last-i+2<<".idsource = d0.idsource and d"<<last-i+2<<".idsent=d0.idsent and ";
			}
			query<<"d0.idsent!=-1 and " <<"d0.rank = "<<g0->rang<<" and " <<"d1.rank = "<<g1->rang<<" and ";
			for (i=last;i>=0;i--) {
				g=key.element[i];
				query<<"d"<<last-i+2<<".rank = "<<g->rang<<" and ";
			}
			query<<"(d0.value & "<<g0->mot<<") = "<<g0->mot<<" and " <<"(d1.value & "<<g1->mot<<") = "<<g1->mot;
			for (i=last;i>=0;i--) {
				g=key.element[i];
				query<<" and (d"<<last-i+2<<".value & "<<g->mot<<") = "<<g->mot;
			}
			query<<");";
		}
		Result res=query.store();
		if (!res.empty()) {
			Result::iterator i;
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				int idoc=row.at(0);
				int ipara=row.at(1);
				int idsent=row.at(2);
				char trouve=0;
				for (int il=0;il<liste.dernier;il++) {
					if (liste[il]->idoc==idoc &&
					                liste[il]->isent==idsent) {
						trouve=1;
						break;
					}
				}
				if (!trouve)
					liste.ajoute(new Iddoc(type,idoc,ipara,idsent));
			}
		}
	}
	catch(BadQuery &er) {
		os<< "Error 13: " << er.what() << "\r\n";
	}
}

void RequestOnTypeVector(vector<string>& strings,vector<int>& ids,int type,ostream& os) {
	try{
		Query query=bd->query();
		query<<"select word,idconcept from concepts where POS="<<type<<";";
		Result res=query.store();
		if (!res.empty()) {
			Result::iterator i;
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				string s(row.at(0));
				strings.push_back(s);
				ids.push_back(row.at(1));
			}
		}
	}
	catch(BadQuery &er) {
		os<< "Error 14: " << er.what() << "\r\n";
	}
}

void RequestOnType(vector<string>& strings,int type,ostream& os) {
	try{
		Query query=bd->query();
		query<<"select word from concepts where POS="<<type<<";";
		Result res=query.store();
		if (!res.empty()) {
			Result::iterator i;
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				string s(row.at(0));
				strings.push_back(s);
			}
		}
	}
	catch(BadQuery &er) {
		os<< "Error 15: " << er.what() << "\r\n";
	}
}

void VectorRequestOnType(vecteur& filtre,int type,ostream& os) {
	try {
		Query query=bd->query();
		query<<"select idconcept from concepts where POS="<<type<<";";
		Result res=query.store();
		if (!res.empty()) {
			Result::iterator i;
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				int id=row.at(0);
				granulet* g=new granulet(id);
				filtre.merge(g);
			}
		}
	}
	catch(BadQuery &er) {
		os<< "Error 16: " << er.what() << "\r\n";
	}
}

int RequeteBaseDependance(int idoc,int isent,vecteur& key,ostream& os) {
	if (!key.element.dernier) {
		return 0;
	}
	try {
		granulet* g=key.element[0];
		Query query=bd->query();
		query << "select iddep,rank,value from dependances where (idsent="<<isent
		<<" and rank >= "<<g->rang <<" and rank <= "<<key.element.fin()->rang<<");";
		Result res=query.store();
		Result::iterator k=res.begin();
		int j=0;
		while (k!=res.end()) {
			Row row = *k;
			int iddep=row.at(0);
			int idep=row.at(0);
			j=0;
			while (iddep==idep && j<key.element.dernier) {
				int rang=row.at(1);
				VECTEUR mot=row.at(2);
				//Les listes sont ordonnees
				g=key.element[j];
				if (rang==g->rang) {
					if ((mot & g->mot)==g->mot) {
						j++;
					} else {
						break;
					}
				} else {
					if (rang >g->rang) {
						break;
					}
				}
				k++;
				row = *k;
				if (k==res.end()) {
					break;
				}
				idep=row.at(0);
			}
			if (j==key.element.dernier) {
				return 1;
			}
			if (k==res.end()) {
				return 0;
			}
			while (k!=res.end()) {
				row = *k;
				idep=row.at(0);
				if (idep!=iddep) {
					break;
				}
				k++;
			}
		}
	}
	catch(BadQuery &er) {
		os<< "Error 17: " << er.what() << "\r\n";
	}
	return 0;
}
//-------------------------------------------------------------------------------
int RequeteBaseDependanceInitial(int idoc,int isent,vecteur& key,ostream& os) {
	if (!key.element.dernier) {
		return 0;
	}
	try {
		int i;
		granulet* g;
		Query query=bd->query();
		if (key.element.dernier==1) {
			g=key.element[0];
			query << "select count(*) from dependances where idsource="<<idoc
			<<" and idsent="<<isent <<" and rank = "<<g->rang << " and (value & "<<g->mot<<" = "<<g->mot<<");";
		}
		else {
			query<<"select count(*) from dependances as d0 inner join dependances as d1 using (idsource,idsent,iddep)";
			for (i=2;i<key.element.dernier;i++) {
				query<<", dependances as d"<<i;
			}
			granulet* g0=key.element[0];
			granulet* g1=key.element[1];
			query<<" where (d0.idsource = "<<idoc<<" and " <<"d0.idsent = "<<isent<<" and ";
			for (i=2;i<key.element.dernier;i++) {
				g=key.element[i];
				query<<"d"<<i<<".idsource = "<<idoc <<" and d"<<i<<".idsent="<<isent
				<<" and d"<<i<<".iddep=d0.iddep and ";
			}
			for (i=2;i<key.element.dernier;i++) {
				g=key.element[i];
				query<<"d"<<i<<".rank = "<<g->rang<<" and ";
			}
			query<<"d0.rank = "<<g0->rang<<" and " <<"d1.rank = "<<g1->rang;
			for (i=2;i<key.element.dernier;i++) {
				g=key.element[i];
				query<<" and (d"<<i<<".value & "<<g->mot<<") = "<<g->mot;
			}
			query<<" and (d0.value & "<<g0->mot<<") = "<<g0->mot<<" and " <<"(d1.value & "<<g1->mot<<") = "<<g1->mot;
			query<<");";
		}
		Result res=query.store();
		if (!res.empty()) {
			Result::iterator i=res.begin();
			Row row = *i;
			return row.at(0);
		}
	}
	catch(BadQuery &er) {
		os<< "Error 18: " << er.what() << "\r\n";
	}
	return 0;
}

//--------------------------------------------------------------------------------------
int RequeteBaseDependanceDictionaires(int idoc, int isent, vecteur& key, char* dico, ostream& os) {
	if (!key.element.dernier) {
		return 0;
	}
	try {
		int i;
		granulet* g;
		Query query=bd->query();
		if (key.element.dernier==1) {
			string dictionaires=" and (type=\"*\"";
			if (dico[0] == TRUE)
				dictionaires=dictionaires+" or type=\"w\"";
			if (dico[1] == TRUE)
				dictionaires=dictionaires+" or type=\"b\"";
			if (dico[2] == TRUE)
				dictionaires=dictionaires+" or type=\"d\"";
			if (dico[3] == TRUE)
				dictionaires=dictionaires+" or type=\"e\"";
			if (dico[4] == TRUE)
				dictionaires=dictionaires+" or type=\"s\"";
			if (dico[5] == TRUE)
				dictionaires=dictionaires+" or type=\"r\"";
			if (dico[6] == TRUE)
				dictionaires=dictionaires+" or type=\"t\"";
			dictionaires=dictionaires+") ";
			g=key.element[0];
			query << "select count(*) from dependances where idsource="<<idoc <<" and idsent="<<isent
			<<" and rank = "<<g->rang <<dictionaires << " and (value & "<<g->mot<<" = "<<g->mot<<");";
		} else {
			string dico0="(d0.type=\"*\"";
			if (dico[0] == TRUE) {
				dico0=dico0+" or d0.type=\"w\"";
			}
			if (dico[1] == TRUE) {
				dico0=dico0+" or d0.type=\"b\"";
			}
			if (dico[2] == TRUE) {
				dico0=dico0+" or d0.type=\"d\"";
			}
			if (dico[3] == TRUE) {
				dico0=dico0+" or d0.type=\"e\"";
			}
			if (dico[4] == TRUE) {
				dico0=dico0+" or d0.type=\"s\"";
			}
			if (dico[5] == TRUE) {
				dico0=dico0+" or d0.type=\"r\"";
			}
			if (dico[6] == TRUE) {
				dico0=dico0+" or d0.type=\"t\"";
			}
			dico0=dico0+") and ";
			query<<"select count(*) from dependances as d0 inner join dependances as d1 using (idsource,idsent,iddep)";
			for (i=2;i<key.element.dernier;i++) {
				query<<", dependances as d"<<i;
			}
			granulet* g0=key.element[0];
			granulet* g1=key.element[1];
			query<<" where (d0.idsource = "<<idoc<<" and " <<"d0.idsent = "<<isent<<" and "<<dico0;
			for (i=2;i<key.element.dernier;i++) {
				char nbch[10];
				sprintf(nbch,"%d",i);
				string nbi=nbch;
				string dico=" (d"+nbi+".type=\"*\"";
				if (dico[0] == TRUE)
					dico=dico+" or d"+nbi+".type=\"w\"";
				if (dico[1] == TRUE)
					dico=dico+" or d"+nbi+".type=\"b\"";
				if (dico[2] == TRUE)
					dico=dico+" or d"+nbi+".type=\"d\"";
				if (dico[3] == TRUE)
					dico=dico+" or d"+nbi+".type=\"e\"";
				if (dico[4] == TRUE)
					dico=dico+" or d"+nbi+".type=\"s\"";
				if (dico[5] == TRUE)
					dico=dico+" or d"+nbi+".type=\"r\"";
				if (dico[6] == TRUE)
					dico=dico+" or d"+nbi+".type=\"t\"";
				dico=dico+") and ";
				g=key.element[i];
				query<<"d"<<i<<".idsource = "<<idoc <<" and d"<<i<<".idsent="<<isent
				<<" and d"<<i<<".iddep=d0.iddep and "<<dico;
			}
			for (i=2;i<key.element.dernier;i++) {
				g=key.element[i];
				query<<"d"<<i<<".rank = "<<g->rang<<" and ";
			}
			query<<"d0.rank = "<<g0->rang<<" and " <<"d1.rank = "<<g1->rang;
			for (i=2;i<key.element.dernier;i++) {
				g=key.element[i];
				query<<" and (d"<<i<<".value & "<<g->mot<<") = "<<g->mot;
			}
			query<<" and (d0.value & "<<g0->mot<<") = "<<g0->mot<<" and "
			<<"(d1.value & "<<g1->mot<<") = "<<g1->mot;
			query<<");";
		}
		Result res=query.store();
		if (!res.empty()) {
			Result::iterator i=res.begin();
			Row row = *i;
			return row.at(0);
		}
	}
	catch(BadQuery &er) {
		os<< "Error 19: " << er.what() << "\r\n";
	}
	return 0;
}

int LookForDependencies(vecteur& key,VECTA<Iddoc*>& phrases,ostream& os) {
	if (key.element.dernier==0) {
		return -1;
	}
	VECTA<Iddoc*> res;
	for (int i=0;i<phrases.dernier;i++) {
		if (RequeteBaseDependance(phrases[i]->idoc,phrases[i]->isent,key,os)) {
			res.ajoute(phrases.retireElement(i));
			i--;
		}
	}
	phrases.nettoie();
	phrases.affecte(res);
	return 0;
}

int LookForDependenciesDictionaries(vecteur& key,VECTA<Iddoc*>& phrases,char* dictionaries,ostream& os) {
	if (key.element.dernier==0) {
		return -1;
	}
	VECTA<Iddoc*> res;
	for (int i=0;i<phrases.dernier;i++) {
		if (RequeteBaseDependanceDictionaires(phrases[i]->idoc,phrases[i]->isent,key,dictionaries,os)) {
			res.ajoute(phrases.retireElement(i));
			i--;
		}
	}
	phrases.raz();
	phrases.affecte(res);
	return 0;
}

int CountConceptDocuments(int id) {
	Query query=bd->query();
	try {
		query << "select count(*) from concepttosentences where idconcept="<<id<<";";
		Result res=query.store();
		//on regenere la chose
		if (!res.empty()) {
			//we return the id of that word
			Result::iterator i=res.begin();
			Row row = *i;
			int val=row.at(0);
			return val;
		}
	}
	catch(BadQuery &er) {
		cerr << "Error 20: " << er.what() << endl;
	}
	return 0;
}

//This function is used to load the initial set of document associated to the concept
//whose list of documents has been found to be the shortest.
void LoadConceptDocuments(vecteur& documents,int id) {
	Query query=bd->query();
	try {
		query << "select rank,value from concepttosentences where idconcept="<<id<<";";
		Result res=query.store();
		//on regenere la chose
		if (!res.empty()) {
			//we return the id of that word
			Result::iterator i;
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				granulet* g=new granulet;
				g->mot=row.at(1);
				g->rang=row.at(0);
				documents.element.ajoute(g);
			}
		}
	}
	catch(BadQuery &er) {
		cerr << "Error 21: " << er.what() << endl;
	}

}

//This function is used to load a set of documents thanks to the rank values already found
//by a previous search operation. At each step the list is shortened as the number of documents
//obtained from the intersection is smaller.
void LoadFiltredConceptDocuments(vecteur& documents,int id,ostream& os) {
	Query query=bd->query();
	try {
		if (documents.element.size()==0) {
			return;
		}
		query << "select rank,value from concepttosentences where (idconcept="<<id<<" and (";
		for (int k=0;k<documents.element.size();k++) {
			if (k) {
				query<<" or ";
			}
			query<<" rank="<<documents.element[k]->rang;
		}
		query<<"));";
		Result res=query.store();
		//on regenere la chose
		if (!res.empty()) {
			//we return the id of that word
			Result::iterator i;
			int start=0;
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				short rang=row.at(0);
				VECTEUR mot=row.at(1);
				start=documents.common(rang,mot,start);
			}
			//Tous ceux qui sont en queue doivent etre detruit
			int max=documents.element.size();
			for (int j=start;j<max;j++) {
				granulet* g=documents.element.retireElement(start);
				delete g;
			}
		} else {
			documents.element.nettoie();
		}
	}
	catch(BadQuery &er) {
		os<< "Error 22: " << er.what() << endl;
	}
}

int LookForDocument(vecteur& key, vecteur& sentences, VECTA<Iddoc*>& phrases, vector<int>& ids,ostream& os) {
	if (key.element.dernier==0) {
		return -1;
	}
	//First we search on the shortest list
	int i;
	int alreadyfound=-1;
	int mincount=-1;
	if (ids.size()>1) {
		//We compute the number of element for each concept
		for (i=0;i<ids.size();i++) {
			int count=CountConceptDocuments(ids[i]);
			if (mincount==-1) {
				mincount=count;
				alreadyfound=i;
			} else {
				//we then select the shortest document list
				if (count<mincount) {
					mincount=count;
					alreadyfound=i;
				}
			}
		}
		if (alreadyfound==-1) {
			return 0;
		}
	} else {
		alreadyfound=0;
	}
	//we then load the shortest list which we will use as a seed
	//to extract and filter out the other values
	vecteur v;
	LoadConceptDocuments(v,ids[alreadyfound]);
	sentences.merge(v);
	//we then load the other results with the previous concept
	//as filter.
	for (i=0;i<ids.size();i++) {
		if (i!=alreadyfound) {
			LoadFiltredConceptDocuments(sentences,ids[i],os);
		}
	}
	vector<int> res;
	sentences.table(res);
	if (res.size()==0) {
		return 0;
	}
	os<<"Nombre de phrases extraites:"<<res.size()<<Endl;
	for (int j=0;j<res.size();j++) {
		if (j!=0) {
			os<<",";
		}
		os<<res[j];
	}
	os<<"."<<Endl;
	int first=0;
	int last=res.size();
	if (last>nextsent) {
		last=nextsent;
	}
	RequeteBasePhraseWithSentences(first,last,phrases,res,0,os);
	if (phrases.dernier==0) {
		return 0;
	}
	return res.size();
}

int LookForTheDocuments(vecteur& key, vecteur& sentences, VECTA<Iddoc*>& phrases, vector<int>& ids,ostream& os) {
	if (key.element.dernier==0) {
		return -1;
	}
	//First we search on the shortest list
	int i;
	int alreadyfound=-1;
	int mincount=-1;
	if (ids.size()>1) {
		//We compute the number of element for each concept
		for (i=0;i<ids.size();i++) {
			int count=CountConceptDocuments(ids[i]);
			if (mincount==-1) {
				mincount=count;
				alreadyfound=i;
			} else {
				//we then select the shortest document list
				if (count<mincount) {
					mincount=count;
					alreadyfound=i;
				}
			}
		}
		if (alreadyfound==-1) {
			return 0;
		}
	} else {
		alreadyfound=0;
	}
	//we then load the shortest list which we will use as a seed
	//to extract and filter out the other values
	vecteur v;
	LoadConceptDocuments(v,ids[alreadyfound]);
	sentences.merge(v);
	//we then load the other results with the previous concept
	//as filter.
	for (i=0;i<ids.size();i++) {
		if (i!=alreadyfound) {
			LoadFiltredConceptDocuments(sentences,ids[i],os);
		}
	}
	vector<int> res;
	sentences.table(res);
	if (res.size()==0) {
		return 0;
	}
	int first=0;
	int last=res.size();
	if (last>nextsent) {
		last=nextsent;
	}
	RequeteBasePhraseWithNoSentences(first,last,phrases,res,0,os);
	if (phrases.dernier==0) {
		return 0;
	}
	return res.size();
}

int LookForDocumentLevel(vecteur& key,VECTA<Iddoc*>& phrases,int level,char type,ostream& os) {
	if (key.element.dernier==0) {
		return -1;
	}
	switch (level) {
	case 1:
		RequeteBaseTexte(key,phrases,type,os);
		break;
	case 2:
		RequeteBaseParagraphe(key,phrases,type,os);
		break;
	default:
		RequeteBasePhrase(key,phrases,type,os);
	}
	if (phrases.dernier==0) {
		return 0;
	}
	return 1;
}

int GetSentenceVector(vecteur& leResul,int idoc,int isent,ostream& os) {
	try {
		Query query=bd->query();
		query << "select rank,value from sentencetoconcepts where idsource="<<idoc<<" and idsent="<<isent<<";";
		Result res=query.store();
		if (!res.empty()) {
			Result::iterator i;
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				leResul.element.ajoute(new granulet(row.at(0),row.at(1)));
			}
			return 1;
		}
	}
	catch(BadQuery &er) {
		cerr << "Error 23: " << er.what() << endl;
	}
	return 0;
}

void agde::RestoreDependencies(levelText* text,int idoc,int ipar,int isent) {
	try {
		Query query=bd->query();
		levelParagraph* para=text->paragraphes[ipar];
		if (para==NULL) {
			return;
		}
		levelSentence* sent=para->sentences[isent];
		if (sent==NULL) {
			return;
		}
		query << "select * from dependances where idsource="<<idoc <<" and idparagraphe="<<ipar
		<<" and idsent="<<isent<<";";
		Result res=query.store();
		if (!res.empty()) {
			//we return the id of that word
			Result::iterator i;
			int ipar=-1;
			levelDependency* level;
			//on regenere la chose
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				if (row.at(6)=="d") {
					level=sent->dependencies[(int)(row.at(3))];
					if (level==NULL) {
						level=new levelDependency;
						sent->dependencies.affecte(row.at(3),level);
					}
				} else {
					level=sent->subgraphes[(int)(row.at(3))];
					if (level==NULL) {
						level=new levelDependency;
						sent->subgraphes.affecte(row.at(3),level);
					}
				}
				granulet* g=new granulet;
				g->mot=row.at(5);
				g->rang=row.at(4);
				level->key.element.ajoute(g);
			}
		}
	}
	catch(BadQuery &er) {
		cerr << "Error 24: " << er.what() << endl;
	}
}

void agde::RestoreAllDocuments() {
	levelText* text;
	try {
		Query query=bd->query();
		query << "select rank,value,idsource from sentencetoconcepts where idsent=-1 order by idsource,rank;";
		Result res=query.store();
		int idsource;
		int idcourant=-1;
		if (!res.empty()) {
			//we return the id of that word
			Result::iterator i;
			//on regenere la chose
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				int i_rang=row.at(0);
				VECTEUR i_mot=row.at(1);
				idsource=row.at(2);
				if (idsource!=idcourant) {
					text=new levelText(idsource);
					gTextes.affecte(idsource,text);
					LoadDocumentName(text->identity,idsource);
					idcourant=idsource;
				}
				granulet* g=new granulet(i_rang,i_mot);
				text->key.element.ajoute(g);
			}
		}
	}
	catch(BadQuery &er) {
		cerr << "Error 25: " << er.what() << endl;
	}
}

levelText* agde::RestoreDocument(int idoc) {
	levelText* text;
	try {
		Query query=bd->query();
		query << "select rank,value from sentencetoconcepts where idsource="<<idoc<<" and idsent=-1 order by rank;";
		Result res=query.store();
		text=new levelText(idoc);
		LoadDocumentName(text->identity,idoc);
		if (!res.empty()) {
			//we return the id of that word
			Result::iterator i;
			//on regenere la chose
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				int i_rang=row.at(0);
				VECTEUR i_mot=row.at(1);
				granulet* g=new granulet(i_rang,i_mot);
				text->key.element.ajoute(g);
			}
		}
	}
	catch(BadQuery &er) {
		cerr << "Error 26: " << er.what() << endl;
	}
	return text;
}

char agde::RestoreSentences(int idoc) {
	levelText* text=gTextes[idoc];
	if (text==NULL) {
		return 0;
	}
	try {
		Query query=bd->query();
		query << "select rank,value,idsent from sentencetoconcepts where idsource="<<idoc<<" and idsent!=-1 order by idsent,rank;";
		Result res=query.store();
		int id=-1;
		int idphr;
		levelSentence* sent;
		levelParagraph* para;
		if (!res.empty()) {
			//we return the id of that word
			Result::iterator i;
			//on regenere la chose
			for (i=res.begin(); i!=res.end();i++) {
				Row row = *i;
				int i_rang=row.at(0);
				VECTEUR i_mot=row.at(1);
				idphr=row.at(2);
				granulet* g=new granulet(i_rang,i_mot);
				if (id!=idphr) {
					sent=new levelSentence;
					if (id==-1) {
						para=new levelParagraph;
						text->paragraphes.ajoute(para);
					}
					para->sentences.ajoute(sent);
					id=idphr;
				}
				sent->key.element.ajoute(g);
			}
		}
	}
	catch(BadQuery &er) {
		cerr << "Error 27: " << er.what() << endl;
	}
	return 1;
}


//Cleaning of all references of a given document in the database, before restoring it
//to be used when a document has been enriched
void DiscardDocument(int idoc) {
	Query query=bd->query();
	sprintf(peupleur,"delete from dependances where idsource=%d;",idoc);
	query.exec(peupleur);
	sprintf(peupleur,"delete from sentencetoconcepts where idsource=%d;",idoc);
	query.exec(peupleur);
}


void levelDependency::store(agde* ag,Query* query,int idoc,int ipar,int isent,int idep) {
	char inter[1000];
	int iv;
	strcpy(peupleur,"insert into dependances values ");
	char debut=1;
	for (iv=0;iv<key.element.dernier;iv++) {
		if (debut==1) {
			sprintf(inter,"(%d,%d,%d,%d,%u,'%c')",
			        idoc,isent,idep,key.element[iv]->rang,key.element[iv]->mot,type);
			debut=0;
		} else {
			sprintf(inter,",(%d,%d,%d,%d,%u,'%c')", idoc,isent,idep,key.element[iv]->rang,key.element[iv]->mot,type);
		}
		strcat(peupleur,inter);
	}
	try {
		if (iv) {
			strcat(peupleur,";");
			query->exec(peupleur);
		}
	}
	catch(BadQuery &er) {
		cerr << "Error(dependances): " << er.what() << endl;
	}
}

void levelSentence::store(agde* ag,Query* query,int idoc,int ipar) {
	int i;
	//,iv;

	//    query->exec("LOCK TABLES phrases WRITE;");
	//     strcpy(peupleur,"insert into phrases values ");
	//     char debut=1;

	//     for (iv=0;iv<key.element.dernier;iv++) {
	//         char ch[1000];
	//         chainelong((SVECTEUR)key.element[iv]->mot,ch);

	//         if (debut==1) {
	//             sprintf(inter,"(%d,%d,%d,%d,%s,%d,%d)",
	//                     idoc,ipar,isent,key.element[iv]->rang,ch,left,right);
	//             debut=0;
	//         }
	//         else
	//             sprintf(inter,",(%d,%d,%d,%d,%s,%d,%d)",
	//                     idoc,ipar,isent,key.element[iv]->rang,ch,left,right);
	//         strcat(peupleur,inter);
	//     }

	//     if (iv) {
	//         strcat(peupleur,";");
	//         query->exec(peupleur);
	//     }


	for (i=0;i<dependencies.dernier;i++) {
		dependencies[i]->left=left;
		dependencies[i]->right=right;
		dependencies[i]->store(ag,query,idoc,ipar,num,i);
	}
	for (i=0;i<subgraphes.dernier;i++) {
		subgraphes[i]->left=left;
		subgraphes[i]->right=right;
		subgraphes[i]->store(ag,query,idoc,ipar,num,i+dependencies.dernier);
	}
}

void levelParagraph::store(agde* ag,int idoc,int ipar) {
	Query query=bd->query();
	query.exec("LOCK TABLES dependances WRITE;");
	for (int i=0;i<sentences.dernier;i++) {
		sentences[i]->store(ag,&query,idoc,ipar);
	}
	query.exec("UNLOCK TABLES;");
}


void levelSentence::storeIndex(agde* ag,Query* query,int idoc,int ipar) {
	char inter[1000];
	strcpy(peupleur,"insert into sentencetoconcepts values ");
	char debut=0;
	for (int iv=0; iv< key.element.dernier;iv++) {
		inter[0]=',';
		sprintf(inter+debut,"(%d,%d,%u)",
		        num,key.element[iv]->rang,key.element[iv]->mot);
		debut=1;
		strcat(peupleur,inter);
	}
	try {
		if (debut==1) {
			//The final ';' at the end of the string to lauch the command
			strcat(peupleur,";");
			query->exec(peupleur);
		}
	}
	catch(BadQuery &er) {
		cerr << "Error insertion sentence: " << er.what() << endl;
	}
}


void levelParagraph::storeIndex(agde* ag,Query* query,int idoc,int ipar) {
	/*
	char inter[1000];
	strcpy(peupleur,"insert into sentencetoconcepts values ");
	char debut=0;

	for (int iv=0; iv< key.element.dernier;iv++) {
		inter[0]=',';	
		sprintf(inter+debut,"(%d,%d,-1,%d,%u)",
		key.element[iv]->rang,key.element[iv]->mot);
		debut=1;
		strcat(peupleur,inter);
}

	if (debut==1) {
		//The final ';' at the end of the string to lauch the command
		strcat(peupleur,";");
		query->exec(peupleur);
}
	*/
	for (int i=0;i<sentences.dernier;i++) {
		cerr<<"Storing:"<<i<<endl;
		sentences[i]->storeIndex(ag,query,idoc,ipar);
	}
}


void levelParagraph::storeOffsets(agde* ag,Query* query,int idoc,int ipar) {
	char inter[1000];
	try {
		query->exec("LOCK TABLES offsets WRITE;");
		strcpy(peupleur,"insert into offsets values ");
		sprintf(inter,"(%d,%d,-1,%d,%d)",idoc,ipar, left,right);
		strcat(peupleur,inter);
		for (int i=0;i<sentences.dernier;i++) {
			sprintf(inter,",(%d,%d,%d,%d,%d)",
			        idoc,ipar,
			        sentences[i]->num,
			        sentences[i]->left,
			        sentences[i]->right);
			strcat(peupleur,inter);
		}
		//The final ';' at the end of the string to lauch the command
		strcat(peupleur,";");
		query->exec(peupleur);
		query->exec("UNLOCK TABLES;");
	}
	catch(BadQuery &er) {
		cerr << "Error 28: " << er.what() << endl;
	}
}


void levelText::storeIndex(agde* ag) {
	Query query=bd->query();
	try {
		query.exec("LOCK TABLES sentencetoconcepts WRITE;");
		char debut=0;
		/*
		char inter[1000];

		strcpy(peupleur,"insert into sentencetoconcepts values ");
		for (int iv=0; iv< key.element.dernier;iv++) {
			inter[0]=',';
			sprintf(inter+debut,"(%d,-1,-1,%d,%u)",
			number,key.element[iv]->rang,key.element[iv]->mot);
			debut=1;
			strcat(peupleur,inter);
	}

		if (debut==1) {
			//The final ';' at the end of the string to lauch the command
			strcat(peupleur,";");
			query.exec(peupleur);
	}
		*/
		int  i;
		for (i=0;i<paragraphes.dernier;i++) {
			paragraphes[i]->storeIndex(ag,&query,number,i);
		}
		query.exec("UNLOCK TABLES;");
		for (i=0;i<paragraphes.dernier;i++) {
			paragraphes[i]->storeOffsets(ag,&query,number,i);
		}
	}
	catch(BadQuery &er) {
		cerr << "Error 29: " << er.what() << endl;
	}
}


void storeIndexSentence(int numdoc,int indexphrase,string& texte) {
	try {
		Query query=bd->query();
		sprintf(peupleur,"insert into sentences values (%d,%d,0,\"",indexphrase,numdoc);
		int s=strlen(peupleur);
		for (int i=0;i<texte.size();i++) {
			if (texte[i]=='\"') {
				peupleur[s++]='\\';
			} else {
				if (texte[i]=='\\') {
					peupleur[s++]='\\\\';
				}
			}
			peupleur[s++]=texte[i];
		}
		peupleur[s++]='\"';
		peupleur[s++]=')';
		peupleur[s++]=';';
		peupleur[s]=0;
		query.exec(peupleur);
	}
	catch(BadQuery &er) {
		cerr << "Error 30: " << er.what() << endl;
	}
}

void levelText::storeIndexSentence() {
	Query query=bd->query();
	char debut=0;
	char inter[1000];
	int index=firstsentence;
	try {
		query.exec("LOCK TABLES sentences WRITE;");
		strcpy(peupleur,"insert into sentences values ");
		for (int i=0;i<paragraphes.dernier;i++) {
			for (int j=0;j<paragraphes[i]->sentences.dernier;j++) {
				inter[0]=',';
				index=paragraphes[i]->sentences[j]->num;
				sprintf(inter+debut,"(%d,%d)",number,index);
				debut=1;
				strcat(peupleur,inter);
			}
		}
		if (debut==1) {
			//The final ';' at the end of the string to lauch the command
			strcat(peupleur,";");
			query.exec(peupleur);
		}
		query.exec("UNLOCK TABLES;");
	}
	catch(BadQuery &er) {
		cerr << "Error 31: " << er.what() << endl;
	}
}

void levelText::store(agde* ag) {
	storeIndex(ag);
	cout<<"Dependencies storage..."<<endl;
	for (int  i=0;i<paragraphes.dernier;i++) {
		cout<<"Paragraphe:"<<i<<" sur: "<<paragraphes.dernier<<endl;
		paragraphes[i]->store(ag,number,i);
	}
}

//On stocke le fichier en question...
void agde::store(int idoc) {
	cerr<<"Stockage"<<endl;
	DiscardDocument(idoc);
	//We then stock the indexes in the document table
	//TP: i've replace the storeIndex call by the store call
	//gTextes[idoc]->storeIndex(this);
	gTextes[idoc]->store(this);
	cerr<<"Fin Stockage"<<endl;
}

void agde::storeconceptoffset() {
	Query query=bd->query();
	query.exec("LOCK TABLES wordoffset WRITE;");
	char inter[1000];
	char debut=0;
	try {
		for (int i=0;i<listeDesConcepts.dernier;i++) {
			debut=0;
			strcpy(peupleur,"insert into wordoffset values ");
			for (int j=0;j<listeDesConcepts[i]->the_offsets.dernier;j++) {
				sprintf(inter,"(%d,%d,%d,%d),",
				        listeDesConcepts[i]->reference,
				        listeDesConcepts[i]->the_offsets[j]->idsent,
				        listeDesConcepts[i]->the_offsets[j]->left,
				        listeDesConcepts[i]->the_offsets[j]->right);
				debut=1;
				strcat(peupleur,inter);
			}
			if (debut==1) {
				//The final ';' at the end of the string to lauch the command
				peupleur[strlen(peupleur)-1]=';';
				query.exec(peupleur);
			}
		}
		query.exec("UNLOCK TABLES;");
	}
	catch(BadQuery &er) {
		cerr << "Error 41: " << er.what() << endl;
	}
}

