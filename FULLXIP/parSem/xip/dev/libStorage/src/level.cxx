#include "general.h"
#include "agde.h"


#include "level.h"


/*#include "general.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "level.h"
*/
//*********************************************************************************
levelText::levelText(int numberDoc,string id) : level() {
	number=numberDoc;
	loaded=NO;
	identity=id;
	firstsentence=0;
}

void levelText::save(ofstream& file) {
	file<<"<document id=\""<<identity<<"\" num=\""<<number<<"\" left=\""<<left<<"\" right=\""<<right<<"\">"<<endl;
	key.save(file);

	for (int i=0;i<paragraphes.dernier;i++)
		paragraphes[i]->save(file);
	file<<"</document>"<<endl;
}

void levelText::reload(ifstream& file, char* buffer,int max) {
	char ident[1000];
	char* end;

	file.getline(buffer,max);
	if (!sinput.Extract(buffer,"id=\"%s\" num=\"%d\" left=\"%d\" right=\"%d\"", &end,ident,&number,&left,&right)) {
		cerr<<"Error reading file (text): "<<buffer<<endl;
		exit(-1);
	}

	loaded=YES;
	identity=strdup(ident);
	key.reload(file);
	file.getline(buffer,max);
	while ( strcmp(buffer,"</document>")) {
		levelParagraph* para=new levelParagraph;
		para->reload(file,buffer,max);
		paragraphes.ajoute(para);
		file.getline(buffer,max);
	}
}

void levelParagraph::save(ofstream& file) {
	file<<"<paragraph left=\""<<left<<"\" right=\""<<right<<"\">"<<endl;
	key.save(file);
	for (int i=0; i< sentences.dernier;i++)
		sentences[i]->save(file);
	file<<"</paragraph>"<<endl;
}

void levelParagraph::reload(ifstream& file, char* buffer,int max) {
	char* end;

	if (!sinput.Extract(buffer,"paragraph left=\"%d\" right=\"%d\"", &end,&left,&right)) {
		cerr<<"Error reading file (paragraph): "<<buffer<<endl;
		exit(-1);
	}
	key.reload(file);
	file.getline(buffer,max);
	while (strcmp(buffer,"</paragraph>")) {
		levelSentence* sent=new levelSentence;
		sent->reload(file,buffer,max);
		sentences.ajoute(sent);
		file.getline(buffer,max);
	}
}


void levelSentence::save(ofstream& file) {
	int i;
	file<<"<sentence left=\""<<left<<"\" right=\""<<right<<"\">"<<endl;
	key.save(file);
	for (i=0;i<subgraphes.dernier;i++)
		subgraphes[i]->save(file,YES);
	for (i=0;i<dependencies.dernier;i++)
		dependencies[i]->save(file,NO);
	file<<"</sentence>"<<endl;
}


void levelSentence::reload(ifstream& file, char* buffer,int max) {
	char* end;

	if (!sinput.Extract(buffer,"sentence left=\"%d\" right=\"%d\"", &end,&left,&right)) {
		cerr<<"Error reading file (sentence): "<<buffer<<endl;
		exit(-1);
	}
	key.reload(file);
	file.getline(buffer,max);

	while (strcmp(buffer,"</sentence>")) {
		levelDependency* dep=new levelDependency;
		if (dep->reload(file,buffer,max)=='Y')
			subgraphes.ajoute(dep);
		else
			dependencies.ajoute(dep);
		file.getline(buffer,max);
	}
}

void levelDependency::save(ofstream& file,char sub) {
	if (sub==YES)
		file<<"<dependency left=\""<<left<<"\" right=\""<<right<<"\" sub=\"Y\">"<<endl;
	else
		file<<"<dependency left=\""<<left<<"\" right=\""<<right<<"\" sub=\"N\">"<<endl;
	key.save(file);
	file<<"</dependency>"<<endl;
}

char levelDependency::reload(ifstream& file, char* buffer,int max) {
	char* end;
	char type[10];

	if (!sinput.Extract(buffer,"dependency left=\"%d\" right=\"%d\" sub=\"%S\"", &end,&left,&right,type)) {
		cerr<<"Error reading file (dependency): "<<buffer<<endl;
		exit(-1);
	}

	key.reload(file);
	file.getline(buffer,max);
	if (strcmp(buffer,"</dependency>")) {
		cerr<<"Error reading file"<<endl;
		exit(-1);
	}
	return type[0];
}

levelText::~levelText() {
	paragraphes.nettoie();
}



char levelText::enrichSublevel(vecteur& filter,vecteur& adding) {

	char solution=NO;

	if (!key.present(filter))
		return solution;

	for (int i=0;i<paragraphes.dernier;i++) {
		if (paragraphes[i]->enrichSublevel(filter,adding)) {
			paragraphes[i]->append(adding);
			solution=YES;
		}
	}

	if (solution==YES)
		append(adding);

	return solution;
}

char levelParagraph::enrichSublevel(vecteur& filter,vecteur& adding) {

	char solution = NO;

	if (!key.present(filter))
		return solution;

	for (int i=0;i<sentences.dernier;i++) {
		if (sentences[i]->enrichSublevel(filter,adding)) {
			sentences[i]->append(adding);
			solution=YES;
		}
	}

	return solution;
}

char levelSentence::enrichSublevel(vecteur& filter,vecteur& adding) {

	char solution=NO;
	int i;

	if (!key.present(filter)) {
		return solution;
	}

	VECTA<levelDependency*> news;


	//If possible, we enrich the subgraph level
	for (i=0;i<subgraphes.dernier;i++) {
		if (subgraphes[i]->enrichSublevel(filter,adding)) {
			// 	  cout<<"Enrichissement subgraph"<<endl;
			subgraphes[i]->append(adding);
			levelDependency* ls=new levelDependency;
			ls->append(subgraphes[i]->key);
			ls->key.filter(filter);
			ls->append(adding);
			news.ajoute(ls);
			solution=YES;
		}
	}

	for (i=0;i<news.dernier;i++)
		subgraphes.ajoute(news[i]);

	news.raz();
	//If possible, we enrich the dependency level
	for (i=0;i<dependencies.dernier;i++) {
		if (dependencies[i]->enrichSublevel(filter,adding)) {
			// 	  cout<<"Enrichissement dependency"<<endl;
			dependencies[i]->append(adding);
			levelDependency* ld=new levelDependency;
			ld->append(dependencies[i]->key);
			ld->key.filter(filter);
			ld->append(adding);
			news.ajoute(ld);
			solution=YES;
		}
	}

	for (i=0;i<news.dernier;i++)
		dependencies.ajoute(news[i]);

	//As we search the answer sentence, the enrichment level
	//has to be compulsory only at the sentence level
	solution = YES;
	return solution;
}


char levelDependency::enrichSublevel(vecteur& filter,vecteur& adding) {

	return key.present(filter);
}



char levelText::testSublevel(vecteur& value,VECTA<levelResult*>& offsets) {
	char result=0;

	for (int i=0;i<paragraphes.dernier;i++) {
		if (paragraphes[i] !=NULL)
			if (paragraphes[i]->testSublevel(value,offsets))
				result=1;
	}

	return result;
}

char levelParagraph::testSublevel(vecteur& value,VECTA<levelResult*>& offsets) {
	char result=0;

	if (!key.present(value))
		return NO;

	for (int i=0;i<sentences.dernier;i++) {
		if (sentences[i]!=NULL)
			if (sentences[i]->testSublevel(value,offsets))
				result=1;
	}

	if (result==0) {
		cerr<<"PARAGRAPHE LEVEL..."<<key.element.dernier<<endl;
		levelResult* res=new levelResult;
		res->init(PARAGRAPHLEVEL,&key,left,right);
		offsets.ajoute(res);
	}

	return YES;
}

char levelSentence::testSublevel(vecteur& value,VECTA<levelResult*>& offsets) {

	int i;
	char result=0;

	if (!key.present(value))
		return NO;

	for (i=0;i<subgraphes.dernier;i++) {
		if (subgraphes[i]!=NULL)
			if (subgraphes[i]->testSublevel(value,offsets))
				result=1;
	}

	for (i=0;i<dependencies.dernier;i++) {
		if (dependencies[i]!=NULL)
			if (dependencies[i]->testSublevel(value,offsets))
				result=1;
	}

	if (result==0) {
		cerr<<"SENTENCE LEVEL: "<<key.element.dernier<<endl;
		levelResult* res=new levelResult;
		res->init(SENTENCELEVEL,&key,left,right);
		offsets.ajoute(res);
	}
	return YES;
}


char levelDependency::testSublevel(vecteur& value,VECTA<levelResult*>& offsets) {
	return key.present(value);
}




void levelText::enrich(vecteur& adding) {
	append(adding);

	for (int i=0;i<paragraphes.dernier;i++)
		paragraphes[i]->enrich(adding);
}

void levelParagraph::enrich(vecteur& adding) {
	append(adding);

	for (int i=0;i<sentences.dernier;i++)
		sentences[i]->enrich(adding);
}

void levelSentence::enrich(vecteur& adding) {
	int i;
	append(adding);

	for (i=0;i<subgraphes.dernier;i++)
		subgraphes[i]->enrich(adding);

	for (i=0;i<dependencies.dernier;i++)
		dependencies[i]->enrich(adding);
}

void levelDependency::enrich(vecteur& adding) {
	append(adding);
}

