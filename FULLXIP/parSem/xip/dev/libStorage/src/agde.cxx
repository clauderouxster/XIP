#include "general.h"
#include "agde.h"

#ifdef _DEBUG
#define statique
#else
#define statique static
#endif

extern int nb_grammars;
TansSinput sinput;

void storeIndexSentence(int numdoc,int indexphrase,string&);

int XipParseXipVECTA(VECTA<XipNode*>& xipnode,XipResult* xipres,int ipar);

granulet* agde::CreeConcept() {
	granulet* n=new granulet;
	n->rang= Rank(gNouveau_Vecteur);
	n->mot= BitPos(gNouveau_Vecteur,n->rang);
	//On garde une liste de tous les granulets a detruire
	gNouveau_Vecteur++;
	return n;
}

agde::~agde() {
	gTextes.nettoie();
}


// transforms a list of strings into a unique string whose separators 
// corresponds to "separator"
string vectInString(vector<string>& v, string separator) {
	string s;
	string tempo;
	string sep = separator;
	int length = v.size();
	for (int i = 0; i < length; i++) {
		if(i == length-1)
			sep = "";
		tempo = v[i] + sep;
		s = s + tempo;
	}
	return s;
}


void ajusteNode(vector<XipNode*>& vNode,int gPhrase,int dPhrase) {
	for(int i = 0; i < vNode.size(); i++) {
		vNode[i]->left = gPhrase;
		vNode[i]->right = dPhrase;
		if(vNode[i]->daughters.size() > 0) {
			ajusteNode(vNode[i]->daughters,gPhrase,dPhrase);
		}
	}
}

void ajuste(XipResult& analyse,int gPhrase,int dPhrase) {
	for(int i = 0; i < analyse.sentences.size(); i++) {
		analyse.sentences[i]->root->left = gPhrase;
		analyse.sentences[i]->root->right = dPhrase;
		ajusteNode(analyse.sentences[i]->root->daughters,gPhrase,dPhrase);
		for(int j = 0; j < analyse.sentences[i]->dependencies.size(); j++) {
			ajusteNode(analyse.sentences[i]->dependencies[j]->parameters,gPhrase,dPhrase);
		}
	}
}


// void agde::saveConcepts(char* filename) {
	//     ofstream file(filename);
	
	//     file<<"<concepts>"<<endl;
	//     //we save the list of id for the concepts...
	//     saveTableFile(file);
	
	//     for (int i=0;i<gListeDesConcepts.size();i++)
	//         gListeDesConcepts[i]->save(file);
	//     file<<"</concepts>"<<endl;
// }

// void agde::saveDocuments(char* filename,int idoc) {
	//     ofstream file(filename);
	
	//     file<<"<binary>"<<endl;
	//     gTextes[idoc]->save(file);
	//     file<<"</binary>"<<endl;
// }


//We reload the document structure
levelText* agde::reloadDocument(string filename) {    
	ifstream file(filename.c_str(),ios::in);
	if (file.fail() )
		return NULL;
	char buffer[1000];
	levelText* level=NULL;
	file.getline(buffer,1000);
	while (strcmp(buffer,"</binary>")) {
		level=new levelText(-1);
		level->reload(file,buffer,1000);
		gTextes.affecte(level->number,level);
		file.getline(buffer,1000);
	}
	return level;
}

//We reload one document
levelText* agde::reloadDocument(int ref) {
	if (gTextes[ref]==NULL)
		return NULL;
	if (gTextes[ref]->loaded==YES)
		return gTextes[ref];
	string buff;
	buff=gTextes[ref]->identity+ ".agde";
	ifstream file(buff.c_str(),ios::in);
	if (file.fail() )
		return NULL;
	char buffer[1000];
	file.getline(buffer,1000);
	levelText* level=gTextes[ref];
	while (strcmp(buffer,"</binary>")) {
		level->reload(file,buffer,1000);
		file.getline(buffer,1000);
	}
	return level;
}

void agde::saveTableFile(ofstream& file) {
	file<<"<tableofdoc>"<<endl;
	for (int i=0;i<gTextes.size();i++) {
		if (gTextes[i]!=NULL) 
			file<<"<equ id=\""<<gTextes[i]->identity<<"\" num=\""<<gTextes[i]->number<<"\">"<<endl;
	}
	file<<"</tableofdoc>"<<endl;
}


void agde::reloadTableFile(ifstream& file) {
	char buffer[1000];
	char* end;
	char id[1000];
	int num=-1;
	gTextes.ajuste(1000);
	gTextes.raz();
	file.getline(buffer,1000);
	if (strcmp(buffer,"<tableofdoc>")) {
		cerr<<"Reading error..."<<endl;
		exit(-1);
	}
	file.getline(buffer,1000);
	while (strcmp(buffer,"</tableofdoc>")) {
		if (!sinput.Extract(buffer,"<equ id=\"%s\" num=\"%d\"",&end,id,&num)) {
			// 	  cerr<<"Reading error for an equ id in: "<<filename<<" = "<<buffer<<endl;
			cerr<<"Reading error ..."<<endl;
			exit(-1);
		}
		levelText* level=new levelText(-1);
		level->number=num;
		level->identity=id;
		gTextes.affecte(num,level);
		file.getline(buffer,1000);
	}
}



void agde::reloadConcepts(string filename) {
	ifstream file(filename.c_str(),ios::in);
	if (file.fail() )
		return;
	int ref;
	char buffer[1000];
	unsigned char conc[1000];
	char* end;
	file.getline(buffer,1000);
	if (strcmp(buffer,"<concepts>")) {
		cerr<<"Reading error: "<<filename<<endl;
		exit(-1);
	}
	reloadTableFile(file);
	file.getline(buffer,1000);
	while (!file.eof() && strcmp(buffer,"</concepts>")) {
		if (!sinput.Extract(buffer,"<key v=\"%s\" ref=\"%d\">",&end,conc,&ref)) {
			cerr<<"Reading error for a key in: "<<filename<<" = "<<buffer<<endl;
			exit(-1);
		}
		vecteur doc;         
		file.getline(buffer,1000);
		if (!strcmp(buffer,"<documents>")) {
			doc.reload(file);
			file.getline(buffer,1000);
			if (strcmp(buffer,"</documents>")) {
				cerr<<"Reading error for a keyword </documents> in: "<<filename<<endl;
				exit(-1);
			}
		}
		vecteur* concept=new vecteur;
		concept->element.ajoute(new granulet(ref));
		entities* c=addConcept((char*)conc,concept);
		if (c!=NULL) {
			c->reference=ref;
			
			for (int idoc=0;idoc<doc.element.size();idoc++)
				c->documents.element.ajoute(new granulet(doc.element[idoc]));
		}
		file.getline(buffer,1000);
		if (strcmp(buffer,"</key>")) {
			cerr<<"Reading error: "<<filename<<", wrong tag: "<<buffer<<endl;
			exit(-1);
		}
		file.getline(buffer,1000);
	}
	file.close();
}
//*********************************************************************************
//This method helps to generate or find a concept on keyword
entities* agde::concept(string mot,string pos, char lower) {
	entities* c=NULL;
	if (!mot.size())
		return NULL;
	unsigned char* lw= (unsigned char*)mot.c_str();
	if (lower == YES) {
		while (*lw) {
			if (*lw < 48)
				*lw='_';
			else
				*lw = tolower(*lw);
			*lw++;
		}
	} else {
		while (*lw) {
			if (*lw < 48)
				*lw='_';
			*lw++;
		}
	}
	int cat=typePOS(pos);
	etat* e=autoconcepts.recherche((unsigned char*)mot.c_str());
	if (e!=NULL)
		c=(entities*)e->valeur;
	entities* h;
	if (c!=NULL) {
		h=c;
		while (h!=NULL) {
			if (h->pos==cat)
				break;
			h=h->homonyme;
		}
		if (h!=NULL) {
			//we keep a track of the number of time a concept has been used...
			h->occurence++;
			if (!h->nouveauConcept)
				h->nouveauConcept=2;            
			return h;
		}
	}
	//We check whether this word is stored in the database
	int occ=0;
	int id=ResearchAndAdd(mot,cat,occ);    
	if (id!=-1) {
		//it already has a reference, no need to create a new one in the table...
		c=new entities(*this,mot,cat,id,occ); 
		//ReloadDocConcept(c);
	} else {
		c=new entities(*this,mot,cat);
		newConceptAdded=YES;
	}
	e=autoconcepts.recherche((unsigned char*)c->cle.c_str());
	if (e!=NULL) {
		h=(entities*)e->valeur;            
		c->homonyme=h->homonyme;
		h->homonyme=c;
	} else {
		autoconcepts.parse((unsigned char*)c->cle.c_str(),(void*)c);
	}
	return c;
}

char agde::concept(int id) {
	//We check whether this word is stored in the database
	int occ=0;
	int cat=0;
	string mot;
	if (indexedConceptList[id] != NULL)
		return YES;
	if (!ResearchAndAdd(id,mot,cat,occ))
		return NO;
	entities* c=new entities(*this,mot,cat,id,occ); 
	//ReloadDocConcept(c);
	etat* e=autoconcepts.recherche((unsigned char*)c->cle.c_str());
	if (e!=NULL) {
		entities* h=(entities*)e->valeur;            
		c->homonyme=h->homonyme;
		h->homonyme=c;
	} else {
		autoconcepts.parse((unsigned char*)c->cle.c_str(),(void*)c);
	}
	newConceptAdded=YES;
	return YES;
}

entities* agde::aConcept(int id) {
	//We check whether this word is stored in the database
	int occ=0;
	int cat=0;
	string mot;
	if (indexedConceptList[id] != NULL)
		return indexedConceptList[id];
	if (!ResearchAndAdd(id,mot,cat,occ))
		return NULL;
	entities* c=new entities(*this,mot,cat,id,occ); 
	//ReloadDocConcept(c);
	etat* e=autoconcepts.recherche((unsigned char*)c->cle.c_str());
	if (e!=NULL) {
		entities* h=(entities*)e->valeur;            
		c->homonyme=h->homonyme;
		h->homonyme=c;
	} else {
		autoconcepts.parse((unsigned char*)c->cle.c_str(),(void*)c);
	}
	newConceptAdded=YES;
	return c;
}

//This method helps to find a concept on keyword
entities* agde::conceptQuestion(string mot,string pos, char lower) {
	entities* c=NULL;
	if (!mot.size()) {
		return NULL;
	}
	if (mot=="\\r" || mot=="li"  || mot=="ol"  || mot=="br"  || mot=="ul"  || mot.size()==1) {
		return NULL;
	}
	// Setting upper-case letters to lower-case if requested
	unsigned char* lw= (unsigned char*)mot.c_str();
	if (lower == YES) {
		while (*lw) {
			if (*lw < 48)
				*lw='_';
			else
				*lw = tolower(*lw);
			*lw++;
		}
	}
	else {
		while (*lw) {
			if (*lw < 48)
				*lw='_';
			*lw++;
		}
	}
	int cat=typePOS(pos);
	etat* e=autoconcepts.recherche((unsigned char*)mot.c_str());
	if (e!=NULL) {
		c=(entities*)e->valeur;
	}
	entities* h;
	if (c!=NULL) {
		h=c;
		while (h!=NULL) {
			if (h->pos==cat)
				break;
			h=h->homonyme;
		}
		if (h!=NULL) {
			//we keep a track of the number of time a concept has been used...
			h->occurence++;
			if (!h->nouveauConcept) {
				h->nouveauConcept=2;
			}
			return h;
		}
	}
	//We check whether this word is stored in the database
	int occ=0;
	int id=ResearchAndAdd(mot,cat,occ);
	if (id==-1) {
		return NULL;
	}
	c=new entities(*this,mot,cat,id,occ); //it already has a reference, no need to create a new one in the table...
	//ReloadDocConcept(c);
	e=autoconcepts.recherche((unsigned char*)c->cle.c_str());
	if (e!=NULL) {
		h=(entities*)e->valeur;            
		c->homonyme=h->homonyme;
		h->homonyme=c;
	} else {
		autoconcepts.parse((unsigned char*)c->cle.c_str(),(void*)c);
	}
	newConceptAdded=YES;
	return c;
}

void agde::addToConceptList(entities* a) {
	a->reference=listeDesConcepts.size();
	listeDesConcepts.ajoute(a);
	etat* e=autoconcepts.recherche((unsigned char*)a->cle.c_str());
	if (e!=NULL) {
		entities* h=(entities*)e->valeur;            
		a->homonyme=h->homonyme;
		h->homonyme=a;
	} else {
		autoconcepts.parse((unsigned char*)a->cle.c_str(),a);
	}
	gNouveau_Vecteur++;
}

//This method helps to generate or find a concept on keyword
entities* agde::addConcept(string mot,vecteur* v) {
	entities* c=NULL;
	newConceptAdded=YES;
	if (mot=="\\r" || mot=="li"  ||  mot=="ol"  || mot=="br"  || mot=="ul"  || mot.size()==1) {
		return NULL;
	}
	c=new entities(*this,mot,0,v);   
	addToConceptList(c);    
	return c;
}

//we traverse the concept list to remove this document from that list
void agde::removeDoc(int doc) {
	for (int i=0;i<listeDesConcepts.size();i++)
		listeDesConcepts[i]->documents.remove(doc);
}

void agde::exploreNodes(XipNode* root, XipNode* mother, XipNode* node, levelDependency* dep, int numberSent, char stockeTraits) {
	entities* current;
	string trait;
	int ifeat;
	if (node->Type()==XIPLEAF) {
		XipNode* loop=node;
		while (loop!=NULL) {           
			if (loop->category=="PUNCT")
				return;
			loop=loop->mother;
		}
		XipLeaf* leaf=(XipLeaf*)node;
		if (stockeTraits==1) {
			for (ifeat=0;ifeat<leaf->features.size();ifeat++) {
				trait=leaf->features[ifeat]->attribute+":"+leaf->features[ifeat]->value;
				current=concept(trait,"FEAT");
				if (current!=NULL && numberSent!=-1) {
					current->addSent(numberSent);
					dep->append(current->concept);
				}
			}
		}
		current=concept(leaf->lemma,leaf->category,YES);
		if (current!=NULL) {
			//For each concept we keep a track of the document it is connected with
			if (numberSent!=-1) {
				current->addSent(numberSent);
				//we also keep a track of the offset for that sentence
				current->addoffset(numberSent,leaf->left-root->index,leaf->right-root->index);
			}
			dep->append(current->concept);
			//The features of the nodes should also be taken into account
		}        
	} else {
		//else we explore the subnodes
		if (stockeTraits==1) {
			for (ifeat=0;ifeat<node->features.size();ifeat++) {                
				trait=node->features[ifeat]->attribute+":"+node->features[ifeat]->value;
				current=concept(trait,"FEAT");
				if (current!=NULL) {
					if (numberSent!=-1)
						current->addSent(numberSent);
					dep->append(current->concept);
				}
			}
		}        
		for (int i=0;i<node->daughters.size();i++) {
			if (node->category=="PUNCT" || (node->mother!=NULL && node->mother->category=="PUNCT")) {
				continue;
			}
			exploreNodes(root,node,node->daughters[i],dep,numberSent,stockeTraits);
		}
	}
}

void agde::exploreNodesQuestion(XipNode* mother, XipNode* node, levelDependency* dep, char onlyfeatures) {
	entities* current;
	string trait;
	int ifeat;
	if (node->Type()==XIPLEAF) {
		if (onlyfeatures==0) {
			XipLeaf* leaf=(XipLeaf*)node;
			current=concept(leaf->lemma,leaf->category,YES);
			if (current!=NULL)
				dep->append(current->concept);
		}
		//else we explore the subnodes
		for (ifeat=0;ifeat<node->features.size();ifeat++) {                
			trait=node->features[ifeat]->attribute+":"+node->features[ifeat]->value;
			current=concept(trait,"FEAT");
			if (current!=NULL) {
				dep->append(current->concept);
			}
		}        
	} else {
		//else we explore the subnodes
		for (ifeat=0;ifeat<node->features.size();ifeat++) {                
			trait=node->features[ifeat]->attribute+":"+node->features[ifeat]->value;
			current=concept(trait,"FEAT");
			if (current!=NULL) {
				dep->append(current->concept);
			}
		}
		char only=onlyfeatures;
		if (node->category=="PREP" || node->category=="PRON") {
			only=1;
		}
		for (int i=0;i<node->daughters.size();i++) {
			exploreNodesQuestion(node,node->daughters[i],dep,only);
		}
	}
}

void agde::exploreNodesQuestionWithNoFeatures(XipNode* mother, XipNode* node, levelDependency* dep) {
	entities* current;
	if (node->Type()==XIPLEAF) {                
		XipLeaf* leaf=(XipLeaf*)node;
		current=concept(leaf->lemma,leaf->category,YES);
		if (current!=NULL) {
			dep->append(current->concept);  
		}
	}
	else {       
		if (node->category=="NOUN" || node->category=="VERB" || node->category=="NUM" || 
			node->category=="ADJ" || node->category=="ADJ2" || node->category=="ADV") {            
			for (int i=0;i<node->daughters.size();i++) {
				exploreNodesQuestionWithNoFeatures(node,node->daughters[i],dep);
			}
		}
	}
}

char buffer_read[10000];

void agde::lookForCoref(XipNode* node, XipResult* xipcoref,VECTA<XipNode*>& NameLists,levelDependency* dep_vect,int numberDoc) {
	if (xipcoref==NULL) {
		return;
	}
	if (node->category!="PRON" && node->category!="DET") {
		return;
	}
	if (xipcoref->sentences.size()==0) {
		return;
	}
	for (int i=0;i<xipcoref->sentences[0]->dependencies.size();i++) {
		XipDependency* dep=xipcoref->sentences[0]->dependencies[i];
		if (dep->parameters[0]->left==node->left &&
		dep->parameters[0]->right==node->right) {
			if (dep->parameters[1]->index!=-1) {
				exploreNodes(NULL,NULL,NameLists[(int)(dep->parameters[1]->index)],dep_vect,numberDoc);
			} else {
				exploreNodes(NULL,NULL,dep->parameters[1],dep_vect,numberDoc);
			}
		}
	}
}

void PutLeaves(VECTA<XipNode*>& feuilles,XipNode* root) {
	feuilles.ajoute(root);
	for (int i=0;i<root->daughters.size();i++) {
		PutLeaves(feuilles,root->daughters[i]);
	}
}

//Main function to extract dependencies and information from XIP
levelText* agde::extractInformation(XipResult& xip, XipResult* xipcoref, string idtext, int ipar, int calcul_semantique,                                    
			levelText* text, int first_element, int last_element) {
	//We explore each sentence
	//We need a Text object
	int numberSent = -1;
	int numberDoc = -1;
	long baseoffset;
	vector<XipDependency*> semantique;
	if (first_element==-1) {
		first_element=0;
		last_element=xip.sentences.size();
	}
	int newSentence;
	string pPhrase;
	levelParagraph* para=NULL;
	int countPara = 0;
	levelSentence* sent;
	char newpara=0;    
	int ifeat,iw,isent;
	char origine;
	if (text==NULL) {
		text=new levelText(numberDoc,idtext);
		numberDoc=LookForDocument(idtext);
		text->number=numberDoc;
		text->loaded=YES;
		para=new levelParagraph;  
		text->paragraphes.ajoute(para);
		text->firstsentence=NewIDSentence();
		numberSent=text->firstsentence;
	}
	else {
		numberDoc=text->number;
		para=text->paragraphes.fin();
		sent=para->sentences.fin();
		numberSent=NewIDSentence();
	}
	VECTA<XipNode*> NameLists;
	static VECTA<XipNode*> feuilles;
	feuilles.raz();
	//On stocke les mots de la phrase...
	for (isent=first_element;isent<last_element;isent++) {
		PutLeaves(feuilles,xip.sentences[isent]->root);
	}
	int itp;
	for (isent=first_element; isent<last_element;isent++) {
		XipNode* root=xip.sentences[isent]->root;
		//for each unit of dependencies and trees
		//first we need a sentence object
		if (calcul_semantique) {
			if (newpara==1) {
				para=new levelParagraph;  
				text->paragraphes.ajoute(para);
				newpara=0;
			}
			//cout<<"Traitement de la "<<isent+1<<"eme phrase sur "<<xip.sentences.size()<<endl;
			newSentence = 1;
			sent=new levelSentence;
			//Il nous faut un numero unique
			sent->num=numberSent;
			string text_phrase="";            
			long right=-1;
			if (store_sentence==1) {
				//Construction de la chaine de caractere correspondant a la phrase...
				for (itp=0;itp<xip.sentences[isent]->leaves.size();itp++) {
					//Initialization
					if (right==-1) {
						right=((XipLeaf*)xip.sentences[isent]->leaves[itp]->daughters[0])->left;
					}
					if (right<0) {
						break;
					}
					string txt=((XipLeaf*)xip.sentences[isent]->leaves[itp]->daughters[0])->surface;
					long left=((XipLeaf*)xip.sentences[isent]->leaves[itp]->daughters[0])->left;
					if (left!=right)
						text_phrase+=" ";
					text_phrase+=txt;
					right=((XipLeaf*)xip.sentences[isent]->leaves[itp]->daughters[0])->right;
				}
				storeIndexSentence(numberDoc,numberSent,text_phrase);         
			}
			para->sentences.ajoute(sent);
			if (root!=NULL) {
				long trucg=root->left;
				long trucd=root->right;
				baseoffset=root->index;
				sent->left=root->left+baseoffset;
				sent->right = root->right+baseoffset;
				if (para->sentences.size()==1) {
					para->left=sent->left;
					if (text->paragraphes.size()==1)
						text->left=sent->left;
				}
				para->right=sent->right;
				text->right=sent->right;
			}
		} else  {           
			cout<<"Enrichissement de: "<<para->sentences.size()<<"eme phrase, "<<countPara<<"eme paragraphe, document:"<<numberDoc<<endl;
		}
		cout<<"Traitement de la phrase:"<<isent<<"("<<xip.sentences[isent]->dependencies.size()<<")"<<endl;
		VECTA<levelDependency*> depends;
		VECTA<levelDependency*> newdepends;
		vecteur ancien;
		int idep;
		XipDependency* dep;
		levelDependency* dep_vect;
		int idd;
		char detruit;
		entities* current;
		int cherchesemantique=0;
		for (idep=0;idep<xip.sentences[isent]->dependencies.size();idep++) {
			dep= xip.sentences[isent]->dependencies[idep];
			//////////////////////////////////////////////////
			//////////TEST THE DEPENDENCY HERE////////////////
			//////////////////////////////////////////////////
			origine='*';
			//The name of dependency is a concept
			dep_vect=new levelDependency;
			current=concept(dep->name,"DEP");
			if (current) {
				current->addSent(numberSent);
				dep_vect->append(current->concept);
			}
			//The features of the dependencies should also be taken into account
			for (ifeat=0;ifeat<dep->features.size();ifeat++) {
				string trait;
				//In the case of a stack element, we only store the value
				//as if it was a word...
				if (dep->features[ifeat]->attribute=="$STACK") {
					trait=dep->features[ifeat]->value;
					current=concept(trait,"NAV");
				} else {
					trait=dep->features[ifeat]->attribute+":"+dep->features[ifeat]->value;
					current=concept(trait,"FEAT");
				}
				if (current) {
					current->addSent(numberSent);
					dep_vect->append(current->concept);
				}
			}
			dep_vect->type=origine;
			//each word is also a concept
			for (iw=0;iw<dep->parameters.size();iw++) {
				XipNode* node=dep->parameters[iw];    
				if (iw==0) {
					dep_vect->left=node->left-baseoffset;
					dep_vect->right=node->right-baseoffset;
				}
				
				exploreNodes(root,NULL,node,dep_vect,numberSent);
			}
			//dep_vect contains all the information we are interested in for a dependency
			//we merge the new vector at the sentence level
			//We check the duplicate case
			detruit=0;
			for (idd=0;idd<depends.dernier;idd++) {
				if (depends[idd]!=NULL) {
					if (depends[idd]->key.equal(dep_vect->key)) {
						delete dep_vect;
						detruit=1;
						break;
					}
				}
			}
			if (detruit==1) {
				continue;
			}
			depends.affecte(idep,dep_vect);
			sent->dependencies.ajoute(dep_vect);	
			sent->append(dep_vect->key);
			//we also merge at the paragraph level
			para->append(dep_vect->key);
			//we finally merge at the text level
			text->append(dep_vect->key);            
		}
		if (calcul_semantique) {
			levelDependency dep_word;
			//For each word in the sentence...        
			exploreNodes(root,NULL,root,&dep_word,numberSent);    
			sent->append(dep_word.key);
			//we also merge at the paragraph level
			para->append(dep_word.key);
			//we finally merge at the text level
			text->append(dep_word.key);            
		}
		numberSent++;
	}
	return text;
}

//--------------------------------------------------------------------------------------
void NodeString(XipNode* node,string& focustring) {
	if (node->Type()==XIPLEAF)
		focustring=focustring+" "+((XipLeaf*)node)->lemma;
	else {
		for (int i=0;i<node->daughters.size();i++)
			NodeString(node->daughters[i],focustring);
	}
}

// This method translates a query that has been analyzed with xip
// into a levelSentence object
int agde::extractInfoRequest(XipResult& xip, int numQuery, levelSentence& query, vecteur& focus, string& focustring, char wordonly) { 
	int numberDoc = -1;
	string trait;
	levelDependency* dep_vect;
	// We process each sentence as a text
	//for each unit of dependencies and trees
	//first we need a sentence object
	if (xip.sentences[numQuery]->root!=NULL) {
		query.left=xip.sentences[numQuery]->root->left;
		query.right = xip.sentences[numQuery]->root->right;
	}
	dep_vect=new levelDependency;
	for (int inode=0;inode<xip.sentences[numQuery]->leaves.size();inode++) {           
		exploreNodesQuestionWithNoFeatures(NULL,xip.sentences[numQuery]->leaves[inode],dep_vect);
	}
	query.append(dep_vect->key);
	delete dep_vect;
	//In case of word only we stop here
	if (wordonly==1)  {          
		return 0;
	}
	char cherchesemantique=0;    
	// Study of each dependency in the query
	for (int idep=0;idep<xip.sentences[numQuery]->dependencies.size();idep++) {
		XipDependency* dep= xip.sentences[numQuery]->dependencies[idep];
		if(dep->name == "SEMANTIQUE" || dep->name == "ADDITION" || dep->name == "SYNONYME") {
			cherchesemantique=1;
			continue;
		}
		dep_vect=new levelDependency;
		//The name of dependency is a concept            
		entities* current=conceptQuestion(dep->name,"DEP");
		if (current) {
			dep_vect->append(current->concept);
			//We gather all the dependency names as a vecteur
			dependencyName.merge(current->concept);
		}
		//each word is also a concept
		for (int iw=0;iw<dep->parameters.size();iw++) {
			XipNode* node=dep->parameters[iw];    
			if (iw==0) {
				dep_vect->left=node->left;
				dep_vect->right=node->right;
			}
			exploreNodesQuestion(NULL,node,dep_vect);           
		}	
		if (dep->name=="FOCUS") {
			focus.merge(dep_vect->key);
			NodeString(dep->parameters[0],focustring);
		}	
		//The features of the dependencies should also be taken into account
		//We have deactivated the reading of features from dependencies...	
		for (int ifeat=0;ifeat<dep->features.size();ifeat++) {
			trait=dep->features[ifeat]->attribute+":"+dep->features[ifeat]->value;
			current=conceptQuestion(trait,"FEAT");
			if (current) {
				dep_vect->append(current->concept);
			}
		}	
		//dep_vect contains all the information we are interested in for a dependency
		//we merge the new vector at the sentence level
		query.dependencies.ajoute(dep_vect);        
		query.append(dep_vect->key);	
	}
	return cherchesemantique;    
}

char agde::ExtractDependencies(string buffer,int ipar, VECTA<levelDependency*>& results) {
	XipResult* xip=XipResultCreate();
	XipParseString(buffer,ipar,xip);
	//We have one sentence only
	XipUnit* xu=xip->sentences[0];
	if (xu==NULL) {
		XipResultDelete(xip);
		return 0;
	}
	for (int idep=0;idep<xu->dependencies.size();idep++) {
		XipDependency* dep= xu->dependencies[idep];
		vecteur* vect=new vecteur;
		levelDependency* dep_vect=new levelDependency;
		entities* current=conceptQuestion(dep->name,"DEP");
		if (current) {
			dep_vect->append(current->concept);
		}
		//each word is also a concept
		for (int iw=0;iw<dep->parameters.size();iw++) {
			XipNode* node=dep->parameters[iw];    
			exploreNodesQuestion(NULL,node,dep_vect);
		}
		//The features of the dependencies should also be taken into account
		for (int ifeat=0;ifeat<dep->features.size();ifeat++) {                        
			string trait=dep->features[ifeat]->attribute+":"+dep->features[ifeat]->value;
			current=conceptQuestion(trait,"FEAT");
			if (current) {
				dep_vect->append(current->concept);  
			}
		}
		results.ajoute(dep_vect);
	}
	XipResultDelete(xip);
	return 1;
}

void agde::printEntities(vecteur& v,ostream& os) {
	vector<int> indexes;
	v.table(indexes);
	char debut=1;
	for (int i=0;i<indexes.size();i++) {
		//We only display nouns, adjectives and verbs..
		if (concept(indexes[i])==YES) {
			if (indexedConceptList[indexes[i]]->pos>=1 && indexedConceptList[indexes[i]]->pos<=3) {
				if (!debut) {
					os<<",";
				}
				os<<indexedConceptList[indexes[i]]->cle;
				switch(indexedConceptList[indexes[i]]->pos) {
					case 1: 
						os<<":N";
						break;
					case 2: 
						os<<":V";
						break;
					case 3: 
						os<<":A";
						break;
					default:
						os<<"*";
				}
				debut=0;
			}
		}
	}
	os<<".";
}

// //This method constructs the table of documents containing
// //entities included in a vecteur
// void agde::returnCommonDocuments(vecteur& v, VECTA<int>& interdocs) {
	//     VECTA<int> entites;
	//     v.table(entites);
	//     vecteur inter;
	
	//     for (int ie=0;ie<entites.size();ie++)  {
		//         entities* e=gListeDesConcepts[entites[ie]];
		//         if (ie==0)
		//             inter.merge(e->documents);
		//         else 
		//             inter.common(e->documents);
	//     }
	//     //We build the list of all documents indexes
	//     inter.table(interdocs);
// }


