#include "general.h"
#include "agde.h"

//Variables globales


extern TansSinput sinput;

static char RefTable[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWabcdefghij";

//Word to String
char* WtoS(VECTEUR mot,char* chaine) {
	//Nous utilisons une base 42 (tous les car. compris entre "0" et "Z")
	int pos=99;
	chaine[100]=0;
	VECTEUR div=0;
	VECTEUR reste=0;
	while (mot!=0) {
		div=mot/42;
		reste=mot%42;
		chaine[pos--]=RefTable[reste];
		mot=div;
	}

	strcpy(chaine,chaine+pos+1);
	return chaine;
}

//String to Word
VECTEUR StoW(char* chaine) {
	//Nous utilisons une base 42 (tous les car. compris entre "0" et "Z")
	VECTEUR mot=0;
	for (int i=0;chaine[i]!=0;i++) {
		int val= strchr(RefTable,chaine[i])-RefTable;
		mot=mot*42 + val;
	}
	return mot;
}


/*----------------------------------------------------------------------------------------*/

granulet::granulet(int val) {
	rang = Rank(val);
	mot =  BitPos(val,rang);
}

granulet::granulet(int rg,VECTEUR val) {
	rang = rg;
	mot =  val;
}


granulet::granulet(granulet* g) {
	if (g==NULL) {
		mot=0;
		rang=0;
	}
	else {
		mot=g->mot;
		rang=g->rang;
	}
}

granulet::granulet(granulet& g) {
	mot=g.mot;
	rang=g.rang;
}

void granulet::save(ostream& os) {
	char chaine[110];
	char autre[20];
	sprintf(autre,"%d",rang);
	os<<"<token word=\"";
	os<<WtoS(mot,chaine);
	os<<"\" pos=\"";
	os<<autre<<"\">"<<endl;
}

void granulet::reload(char* chaine) {
	char ident[1000];
	char* end;

	if (!sinput.Extract(chaine,"token word=\"%s\" pos=\"%d\"",&end,ident,&rang)) {
		cerr<<"Error reading file"<<endl;
		exit(-1);
	}
	mot=StoW(ident);
}

//Vérification de la présence d'un concept dans un autre

int vecteur::present(vecteur* v) {

	int pos=0;
	int i=0;

	while (i<v->element.dernier) {


		if (pos >= element.dernier)
			//la liste des elements courant est epuisee, pas celle de v
			//c'est un cas d'erreur
			return NO;

		//3 cas
		//a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les granulets sont ordonnes...
		if (v->element[i]->rang < element[pos]->rang)
			return NO;

		//b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
		if (v->element[i]->rang > element[pos]->rang)
			pos++;
		else {
			//c) c'est le meme element, on verifie si les bits sont compatibles:
			if ((element[pos]->mot & v->element[i]->mot) != v->element[i]->mot)
				return NO;
			//on passe aux elements suivants
			pos++;
			i++;
		}
	}
	return YES;
}

int vecteur::present(vecteur& v) {

	int pos=0;
	int i=0;

	while (i<v.element.dernier) {


		if (pos >= element.dernier)
			//la liste des elements courant est epuisee, pas celle de v
			//c'est un cas d'erreur
			return NO;

		//3 cas
		//a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les granulets sont ordonnes...
		if (v.element[i]->rang < element[pos]->rang)
			return NO;

		//b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
		if (v.element[i]->rang > element[pos]->rang)
			pos++;
		else {
			//c) c'est le meme element, on verifie si les bits sont compatibles:
			if ((element[pos]->mot & v.element[i]->mot) != v.element[i]->mot)
				return NO;
			//on passe aux elements suivants
			pos++;
			i++;
		}
	}
	return YES;
}
/*----------------------------------------------------------------------------------------*/
void vecteur::filter(vecteur& v) {
	int pos=0;
	int i=0;
	granulet* g;

	while (i<v.element.dernier) {

		if (pos >= element.dernier)
			//la liste des elements courant est epuisee, pas celle de v
			//il ne peut plus y avoir d'autres elements communs
			break;

		//3 cas
		//a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les granulets sont ordonnes...
		if (v.element[i]->rang < element[pos]->rang)
			//on passe a l'element suivant
			i++;
		else
			//b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
			if (v.element[i]->rang > element[pos]->rang)
				pos++;
			else {
				//c) c'est le meme element, on conserve les bits...
				//on se prepare une nouvelle granulet
				element[pos]->mot &=~(v.element[i]->mot);

				if (element[pos]->mot==0) {
					g=element.retireElement(pos);
					delete g;
				}
				else
					//on passe aux elements suivants
					pos++;
				i++;
			}
	}
}
/*----------------------------------------------------------------------------------------*/
void vecteur::filter(vecteur* v) {
	int pos=0;
	int i=0;
	granulet* g;

	while (i<v->element.dernier) {

		if (pos >= element.dernier)
			//la liste des elements courant est epuisee, pas celle de v
			//il ne peut plus y avoir d'autres elements communs
			break;

		//3 cas
		//a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les granulets sont ordonnes...
		if (v->element[i]->rang < element[pos]->rang)
			//on passe a l'element suivant
			i++;
		else
			//b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
			if (v->element[i]->rang > element[pos]->rang)
				pos++;
			else {
				//c) c'est le meme element, on conserve les bits...
				//on se prepare une nouvelle granulet
				element[pos]->mot &=~v->element[i]->mot;

				if (element[pos]->mot==0) {
					g=element.retireElement(pos);
					delete g;
				}
				else
					//on passe aux elements suivants
					pos++;
				i++;
			}
	}
}

/*----------------------------------------------------------------------------------------*/
void vecteur::merge(granulet* g) {
	int pos=0;

	while (pos<element.dernier) {

		//3 cas
		//a) l'element a ajoute est place avant, on l'insere simplement
		if (element[pos]->rang > g->rang) {
			//on ne fait que recopier le pointeur...
			element.insere(pos,new granulet(g));
			return;
		}
		else
			//b) l'element est situe apres, il faut donc avancer d'un cran...
			if (element[pos]->rang < g->rang)
				pos++;
			else {
				//c) c'est le meme element, on fusionne les bits
				element[pos]->mot |= g->mot;
				//Et on detruit le pointeur
				delete g;
				return;
			}
	}
	//sinon on le rajoute en queue
	element.ajoute(g);
}

void vecteur::merge(granulet& g) {
	int pos=0;

	while (pos<element.dernier) {

		//3 cas
		//a) l'element a ajoute est place avant, on l'insere simplement
		if (element[pos]->rang > g.rang) {
			//on ne fait que recopier le pointeur...
			element.insere(pos,new granulet(g));
			return;
		}
		else
			//b) l'element est situe apres, il faut donc avancer d'un cran...
			if (element[pos]->rang < g.rang)
				pos++;
			else {
				//c) c'est le meme element, on fusionne les bits
				element[pos]->mot |= g.mot;
				//Et on detruit le pointeur
				return;
			}
	}

	//sinon on le rajoute en queue
	element.ajoute(new granulet(g));
}

void vecteur::merge(vecteur& ajout) {
	int pos=0;
	int i=0;

	while (i<ajout.element.dernier) {

		if (pos >= element.dernier) {
			//dans le cas ou l'on a epuise la liste des elements courants
			//on rajoute simplement le reste a la suite...
			for (;i<ajout.element.dernier;i++)
				element.ajoute(new granulet(ajout.element[i]));
			break;
		}

		//3 cas
		//a) l'element a ajoute est place avant, on l'insere simplement
		if (element[pos]->rang > ajout.element[i]->rang) {
			//on ne fait que recopier le pointeur...
			element.insere(pos,new granulet(ajout.element[i]));
			//et on passe a l'element suivant
			i++;
		}
		else
			//b) l'element est situe apres, il faut donc avancer d'un cran...
			if (element[pos]->rang < ajout.element[i]->rang)
				pos++;
			else {
				//c) c'est le meme element, on fusionne les bits
				element[pos]->mot |= ajout.element[i]->mot;
				//on passe aux elements suivants
				pos++;
				i++;
			}
	}
}

/*----------------------------------------------------------------------------------------*/
void vecteur::merge(vecteur* ajout) {
	int pos=0;
	int i=0;

	while (i<ajout->element.dernier) {

		if (pos >= element.dernier) {
			//dans le cas ou l'on a epuise la liste des elements courants
			//on rajoute simplement le reste a la suite...
			for (;i<ajout->element.dernier;i++)
				element.ajoute(new granulet(ajout->element[i]));
			break;
		}

		//3 cas
		//a) l'element a ajoute est place avant, on l'insere simplement
		if (element[pos]->rang > ajout->element[i]->rang) {
			//on ne fait que recopier le pointeur...
			element.insere(pos,new granulet(ajout->element[i]));
			//et on passe a l'element suivant
			i++;
		}
		else
			//b) l'element est situe apres, il faut donc avancer d'un cran...
			if (element[pos]->rang < ajout->element[i]->rang)
				pos++;
			else {
				//c) c'est le meme element, on fusionne les bits
				element[pos]->mot |= ajout->element[i]->mot;
				//on passe aux elements suivants
				pos++;
				i++;
			}
	}
}


/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
void vecteur::common(vecteur* v1,vecteur* v2) {

	int pos=0;
	int i=0;

	while (i<v2->element.dernier) {

		if (pos >= v1->element.dernier)
			//la liste des elements courant est epuisee, pas celle de v
			//il ne peut plus y avoir d'autres elements communs
			break;

		//3 cas
		//a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les granulets sont ordonnes...
		if (v2->element[i]->rang < v1->element[pos]->rang)
			//on passe a l'element suivant
			i++;
		else
			//b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
			if (v2->element[i]->rang > v1->element[pos]->rang)
				pos++;
			else {
				//c) c'est le meme element, on conserve les bits...
				//on se prepare une nouvelle granulet
				granulet* g= new granulet();
				g->mot= v1->element[pos]->mot & v2->element[i]->mot;
				g->rang=v1->element[pos]->rang;

				if (g->mot==0)
					delete g;
				else
					element.ajoute(g);
				//on passe aux elements suivants
				pos++;
				i++;
			}
	}
}
/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
void vecteur::common(vecteur* v) {

	int pos=0;
	int i=0;
	granulet* g;

	while (i<v->element.dernier) {

		if (pos >= element.dernier)
			//la liste des elements courant est epuisee, pas celle de v
			//il ne peut plus y avoir d'autres elements communs
			break;

		//3 cas
		//a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les granulets sont ordonnes...
		if (v->element[i]->rang < element[pos]->rang)
			//on passe a l'element suivant
			i++;
		else
			//b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
			if (v->element[i]->rang > element[pos]->rang) {
				//cet element en pos n'existe pas dans v
				g=element.retireElement(pos);
				delete g;
			}
			else {
				//c) c'est le meme element, on conserve les bits...
				//on se prepare une nouvelle granulet
				element[pos]->mot &= v->element[i]->mot;

				if (element[pos]->mot==0) {
					g=element.retireElement(pos);
					delete g;
				}
				else
					//on passe aux elements suivants
					pos++;
				i++;
			}
	}
	for (i=element.dernier-1;i>=pos;i--) {
		g=element.retire();
		delete g;
	}
}
/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
int vecteur::common(short r, VECTEUR m,int start) {

	int pos=start;
	granulet* g;



	while(pos<element.dernier) {
		//si cet element n'est pas dans la liste on repart
		if (r>element[pos]->rang) {
			g=element.retireElement(pos);
			delete g;
			continue;
		}

		if (r==element[pos]->rang) {
			element[pos]->mot &= m;

			if (element[pos]->mot==0) {
				g=element.retireElement(pos);
				delete g;
				return start;
			}
			return start+1;
		}
		return start;

	}
	return start;
}

/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
void vecteur::common(vecteur& v) {

	int pos=0;
	int i=0;
	granulet* g;
	int premier=-1;

	while (i<v.element.dernier) {

		if (pos >= element.dernier)
			//la liste des elements courant est epuisee, pas celle de v
			//il ne peut plus y avoir d'autres elements communs
			break;

		//3 cas
		//a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les granulets sont ordonnes...
		if (v.element[i]->rang < element[pos]->rang)
			//on passe a l'element suivant
			i++;
		else
			//b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
			if (v.element[i]->rang > element[pos]->rang) {
				//cet element en pos n'existe pas dans v
				g=element.retireElement(pos);
				delete g;
			}
			else {
				//c) c'est le meme element, on conserve les bits...
				//on se prepare une nouvelle granulet
				element[pos]->mot &= v.element[i]->mot;

				if (element[pos]->mot==0) {
					g=element.retireElement(pos);
					delete g;
				}
				else
					//on passe aux elements suivants
					pos++;
				i++;
			}
	}
	//On retire les elements en queue
	for (i=element.dernier-1;i>=pos;i--) {
		g=element.retire();
		delete g;
	}
}
/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
void vecteur::common(vecteur& v1,vecteur& v2) {

	int pos=0;
	int i=0;

	while (i<v2.element.dernier) {

		if (pos >= v1.element.dernier)
			//la liste des elements courant est epuisee, pas celle de v
			//il ne peut plus y avoir d'autres elements communs
			break;

		//3 cas
		//a) l'element a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les granulets sont ordonnes...
		if (v2.element[i]->rang < v1.element[pos]->rang)
			//on passe a l'element suivant
			i++;
		else
			//b) l'element a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
			if (v2.element[i]->rang > v1.element[pos]->rang)
				pos++;
			else {
				//c) c'est le meme element, on conserve les bits...
				//on se prepare une nouvelle granulet
				granulet* g= new granulet();
				g->mot= v1.element[pos]->mot & v2.element[i]->mot;
				g->rang=v1.element[pos]->rang;

				if (g->mot==0)
					delete g;
				else
					element.ajoute(g);
				//on passe aux elements suivants
				pos++;
				i++;
			}
	}
}

/*----------------------------------------------------------------------------------------*/
//Duplication d'un vecteur...
vecteur::vecteur(vecteur* v) {
	if (v!=NULL)
		for (int i=0;i<v->element.dernier;i++)
			element.ajoute(new granulet(v->element[i]));
}

//Duplication d'un vecteur...
vecteur::vecteur(vecteur& v) {
	for (int i=0;i<v.element.dernier;i++)
		element.ajoute(new granulet(v.element[i]));
}

vecteur::vecteur(agde* Agde,int& reference) {

	reference=Agde->gNouveau_Vecteur;
	element.ajoute(Agde->CreeConcept());
}


void vecteur::save(ostream& os) {
	os<<"<vector>"<<endl;
	for (int i=0;i<element.dernier;i++)
		element[i]->save(os);
	os<<"</vector>"<<endl;
}

void vecteur::reload(ifstream& file) {
	char chaine[1000];
	chaine[0]=0;

	file.getline(chaine,1000);
	if (strcmp(chaine,"<vector>")) {
		cerr<<"Error reading file"<<endl;
		exit(-1);
	}
	file.getline(chaine,1000);

	while (!file.eof() && strcmp(chaine,"</vector>")) {
		granulet* g = new granulet;
		g->reload(chaine);
		element.ajoute(g);
		file.getline(chaine,1000);
	}
}

//This method builds a list of integer out of a vecteur element
//each integer corresponds to one concept encoded on the elements
void vecteur::table(vector<int>& liste) {

	//We build in reverse order
	for (int i=element.dernier-1;i>=0;i--) {
		SVECTEUR word=(SVECTEUR)element[i]->mot;
		int base = element[i]->rang*TAILLEVECTEUR;
		for (int k=TAILLEVECTEUR-1;k>=0;k--) {
			//No more bits to seek, we stop
			if (word==0)
				break;
			//the sign bit is the left most bit
			if (word < 0)
				liste.push_back(base+k);
			//we then go to the next element...shifting bits by 1 to the left
			word <<= 1;
		}
	}
}

void vecteur::table(VECTA<int>& liste) {

	//We build in reverse order
	for (int i=element.dernier-1;i>=0;i--) {
		SVECTEUR word=(SVECTEUR)element[i]->mot;
		int base = element[i]->rang*TAILLEVECTEUR;
		for (int k=TAILLEVECTEUR-1;k>=0;k--) {
			//No more bits to seek, we stop
			if (word==0)
				break;
			//the sign bit is the left most bit
			if (word < 0)
				liste.ajoute(base+k);
			//we then go to the next element...shifting bits by 1 to the left
			word <<= 1;
		}
	}
}

int vecteur::count() {
	int counter=0;
	//We build in reverse order
	for (int i=element.dernier-1;i>=0;i--) {
		SVECTEUR word=(SVECTEUR)element[i]->mot;
		int base = element[i]->rang*TAILLEVECTEUR;
		for (int k=TAILLEVECTEUR-1;k>=0;k--) {
			//No more bits to seek, we stop
			if (word==0)
				break;
			//the sign bit is the left most bit
			if (word < 0)
				counter++;
			//we then go to the next element...shifting bits by 1 to the left
			word <<= 1;
		}
	}
	return counter;
}

//We remove the bit b
void vecteur::remove(int b) {
	int rank= Rank(b);
	VECTEUR word=  BitPos(b,rank);

	//We look for the first element of rang=rank
	for (int i=0;i<element.dernier;i++) {
		if (element[i]->rang==rank) {
			//We XOR the bit
			element[i]->mot^=word;
			if (element[i]->mot==0) {
				//the element does not contain any bit anymore
				//we remove it
				granulet* g=element.retireElement(i);
				delete g;
			}
			return;
		}

		//The elements are ordered...
		if (rank > element[i]->rang)
			return;
	}
}

//This methods constructs a vector that is a filter for
//the vecteur method 'filter' from string of char
void vecteur::makeFiltre(string rel,string POS, agde& Agde) {
	entities* fil= Agde.concept(rel,POS);
	merge(fil->concept);
}

void vecteur::makeFiltre(string rel,string POS, agde* Agde) {
	entities* fil= Agde->concept(rel,POS);
	merge(fil->concept);
}

// // transforms a list of strings into a unique string whose separators
// // corresponds to "separator"
// string vectInString(vector<string>& v, string separator) {
//   string s;
//   string tempo;
//   string sep = separator;
//   int length = v.size();
//   for (int i = 0; i < length; i++) {
//     if(i == length-1)
//       sep = "";
//     tempo = v[i] + sep;
//     s = s + tempo;
//   }
//   return s;
// }


char vecteur::equal(vecteur& v) {
	if (element.dernier!=v.element.dernier)
		return 0;
	for (int i=0;i<element.dernier;i++) {
		if (element[i]->rang!=v.element[i]->rang)
			return 0;
		if (element[i]->mot!=v.element[i]->mot)
			return 0;
	}
	return 1;
}

char vecteur::equal(vecteur* v) {
	if (element.dernier!=v->element.dernier)
		return 0;
	for (int i=0;i<element.dernier;i++) {
		if (element[i]->rang!=v->element[i]->rang)
			return 0;
		if (element[i]->mot!=v->element[i]->mot)
			return 0;
	}
	return 1;
}

