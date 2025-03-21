
#include "kif.h"
#include "sparse.h"

extern const char* kifErrorStrings[];

/*----------------------------------------------------------------------------------------*/
#define Rank(val) val >> gBitVectorDivide;
#define BitPos(val,pos) One << (val - (pos << gBitVectorDivide)); 
void strcatchr(char* tmp,char c);
/*----------------------------------------------------------------------------------------*/

itemsparse::itemsparse(int val) {
	rank = Rank(val);
	word =  BitPos(val,rank);
}

itemsparse::itemsparse(int rg,TVector val) {
	rank = rg;
	word =  val;
}


itemsparse::itemsparse(itemsparse* g) {
	if (g==NULL) {
		word=0;
		rank=0;
	}
	else {
		word=g->word;
		rank=g->rank;
	}    
}

itemsparse::itemsparse(itemsparse& g) {
	word=g.word;
	rank=g.rank;
}

//---------------------------------------------------------------------------------------

//Vérification de la présence d'un concept dans un autre

bool sparse::in(sparse* v) {

	int pos=0;
	int i=0;

	while (i<v->filters.dernier) {


		if (pos >= filters.dernier)
			//la liste des filterss courant est epuisee, pas celle de v
			//c'est un cas d'erreur
			return false;

		//3 cas
		//a) l'filters a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les itemsparses sont ordonnes...
		if (v->filters[i]->rank < filters[pos]->rank)
			return false;

		//b) l'filters a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
		if (v->filters[i]->rank > filters[pos]->rank)
			pos++;
		else {
			//c) c'est le meme filters, on verifie si les bits sont compatibles:
			if ((filters[pos]->word & v->filters[i]->word) != v->filters[i]->word)
				return false;
			//on passe aux filterss suivants
			pos++;
			i++;
		}
	}
	return true;
}

bool sparse::in(int index) {
	int pos=0;
	short rank = Rank(index);
	TVector word =  BitPos(index,rank);

	while (pos<filters.size() && filters[pos]->rank<rank) pos++;
	if (pos==filters.size() || filters[pos]->rank>rank)
		return false;

	if ((filters[pos]->word & word) == word)            
		return true;
	return false;
}

bool sparse::in(sparse& v) {

	int pos=0;
	int i=0;

	while (i<v.filters.dernier) {


		if (pos >= filters.dernier)
			//la liste des filterss courant est epuisee, pas celle de v
			//c'est un cas d'erreur
			return false;

		//3 cas
		//a) l'filters a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les itemsparses sont ordonnes...
		if (v.filters[i]->rank < filters[pos]->rank)
			return false;

		//b) l'filters a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
		if (v.filters[i]->rank > filters[pos]->rank)
			pos++;
		else {
			//c) c'est le meme filters, on verifie si les bits sont compatibles:
			if ((filters[pos]->word & v.filters[i]->word) != v.filters[i]->word)
				return false;
			//on passe aux filterss suivants
			pos++;
			i++;
		}
	}
	return true;
}

/*----------------------------------------------------------------------------------------*/
void sparse::andnot(sparse& v) {
	int pos=0;
	int i=0;
	itemsparse* g;

	while (i<v.filters.dernier) {

		if (pos >= filters.dernier)
			//la liste des filterss courant est epuisee, pas celle de v
			//il ne peut plus y avoir d'autres filterss communs
			break;

		//3 cas
		//a) l'filters a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les itemsparses sont ordonnes...
		if (v.filters[i]->rank < filters[pos]->rank)
			//on passe a l'filters suivant
			i++;
		else
			//b) l'filters a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
			if (v.filters[i]->rank > filters[pos]->rank)
				pos++;
			else {
				//c) c'est le meme filters, on conserve les bits...
				//on se prepare une nouvelle itemsparse
				filters[pos]->word &=~(v.filters[i]->word);

				if (filters[pos]->word==0) {
					g=filters.retireElement(pos);
					delete g;
				}
				else
					//on passe aux filterss suivants
					pos++;
				i++;
			}
	}
}
/*----------------------------------------------------------------------------------------*/
void sparse::andnot(sparse* v) {
	int pos=0;
	int i=0;
	itemsparse* g;

	while (i<v->filters.dernier) {

		if (pos >= filters.dernier)
			//la liste des filterss courant est epuisee, pas celle de v
			//il ne peut plus y avoir d'autres filterss communs
			break;

		//3 cas
		//a) l'filters a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les itemsparses sont ordonnes...
		if (v->filters[i]->rank < filters[pos]->rank)
			//on passe a l'filters suivant
			i++;
		else
			//b) l'filters a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
			if (v->filters[i]->rank > filters[pos]->rank)
				pos++;
			else {
				//c) c'est le meme filters, on conserve les bits...
				//on se prepare une nouvelle itemsparse
				filters[pos]->word &=~v->filters[i]->word;

				if (filters[pos]->word==0) {
					g=filters.retireElement(pos);
					delete g;
				}
				else
					//on passe aux filterss suivants
					pos++;
				i++;
			}
	}
}

/*----------------------------------------------------------------------------------------*/
void sparse::or(itemsparse* g) {
	int pos=0;

	while (pos<filters.dernier) {

		//3 cas
		//a) l'filters a ajoute est place avant, on l'insere simplement
		if (filters[pos]->rank > g->rank) {
			//on ne fait que recopier le pointeur...
			filters.insere(pos,new itemsparse(g));
			return;
		}
		else
			//b) l'filters est situe apres, il faut donc avancer d'un cran...
			if (filters[pos]->rank < g->rank)
				pos++;
			else {
				//c) c'est le meme filters, on fusionne les bits
				filters[pos]->word |= g->word;
				//Et on detruit le pointeur
				delete g;
				return;
			}
	}
	//sinon on le rajoute en queue
	filters.push_back(g);
}

void sparse::or(itemsparse& g) {
	int pos=0;

	while (pos<filters.dernier) {

		//3 cas
		//a) l'filters a ajoute est place avant, on l'insere simplement
		if (filters[pos]->rank > g.rank) {
			//on ne fait que recopier le pointeur...
			filters.insere(pos,new itemsparse(g));
			return;
		}
		else
			//b) l'filters est situe apres, il faut donc avancer d'un cran...
			if (filters[pos]->rank < g.rank)
				pos++;
			else {
				//c) c'est le meme filters, on fusionne les bits
				filters[pos]->word |= g.word;
				//Et on detruit le pointeur
				return;
			}
	}

	//sinon on le rajoute en queue
	filters.push_back(new itemsparse(g));
}

void sparse::or(int index) {
	int pos=0;
	short rank = Rank(index);
	TVector word =  BitPos(index,rank);

	while (pos<filters.size() && filters[pos]->rank<rank) pos++;
	if (pos==filters.size())
		filters.push_back(new itemsparse(rank,word));
	else
		if (filters[pos]->rank > rank) {
			//on ne fait que recopier le pointeur...
			filters.insere(pos,new itemsparse(rank,word));
			return;
		}
		filters[pos]->word |= word;
}

void sparse::add(int index) {
	int pos=0;
	short rank = Rank(index);
	TVector word =  BitPos(index,rank);

	while (pos<filters.size() && filters[pos]->rank<rank) pos++;
	if (pos==filters.size())
		filters.push_back(new itemsparse(rank,word));
	else
		if (filters[pos]->rank > rank) {
			//on ne fait que recopier le pointeur...
			filters.insere(pos,new itemsparse(rank,word));
			return;
		}
		filters[pos]->word |= word;
}

void sparse::or(sparse& ajout) {
	int pos=0;
	int i=0;

	while (i<ajout.filters.dernier) {

		if (pos >= filters.dernier) {
			//dans le cas ou l'on a epuise la liste des filterss courants
			//on rajoute simplement le reste a la suite...
			for (;i<ajout.filters.dernier;i++)
				filters.push_back(new itemsparse(ajout.filters[i]));
			break;
		}

		//3 cas
		//a) l'filters a ajoute est place avant, on l'insere simplement
		if (filters[pos]->rank > ajout.filters[i]->rank) {
			//on ne fait que recopier le pointeur...
			filters.insere(pos,new itemsparse(ajout.filters[i]));                   
			//et on passe a l'filters suivant
			i++;
		}
		else 
			//b) l'filters est situe apres, il faut donc avancer d'un cran...
			if (filters[pos]->rank < ajout.filters[i]->rank)
				pos++;
			else {
				//c) c'est le meme filters, on fusionne les bits
				filters[pos]->word |= ajout.filters[i]->word;
				//on passe aux filterss suivants
				pos++;
				i++;
			}
	}
}

/*----------------------------------------------------------------------------------------*/
void sparse::or(sparse* ajout) {
	int pos=0;
	int i=0;

	while (i<ajout->filters.dernier) {

		if (pos >= filters.dernier) {
			//dans le cas ou l'on a epuise la liste des filterss courants
			//on rajoute simplement le reste a la suite...
			for (;i<ajout->filters.dernier;i++)
				filters.push_back(new itemsparse(ajout->filters[i]));
			break;
		}

		//3 cas
		//a) l'filters a ajoute est place avant, on l'insere simplement
		if (filters[pos]->rank > ajout->filters[i]->rank) {
			//on ne fait que recopier le pointeur...
			filters.insere(pos,new itemsparse(ajout->filters[i]));                   
			//et on passe a l'filters suivant
			i++;
		}
		else
			//b) l'filters est situe apres, il faut donc avancer d'un cran...
			if (filters[pos]->rank < ajout->filters[i]->rank)
				pos++;
			else {
				//c) c'est le meme filters, on fusionne les bits
				filters[pos]->word |= ajout->filters[i]->word;
				//on passe aux filterss suivants
				pos++;
				i++;
			}
	}
}


void sparse::reset() {
	filters.clear();
}

/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
void sparse::and(sparse* v1,sparse* v2) {

	int pos=0;
	int i=0;	
	filters.clear();

	while (i<v2->filters.dernier) {

		if (pos >= v1->filters.dernier)
			//la liste des filterss courant est epuisee, pas celle de v
			//il ne peut plus y avoir d'autres filterss communs
			break;

		//3 cas
		//a) l'filters a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les itemsparses sont ordonnes...
		if (v2->filters[i]->rank < v1->filters[pos]->rank)
			//on passe a l'filters suivant
			i++;
		else
			//b) l'filters a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
			if (v2->filters[i]->rank > v1->filters[pos]->rank)
				pos++;
			else {
				//c) c'est le meme filters, on conserve les bits...
				//on se prepare une nouvelle itemsparse
				itemsparse* g= new itemsparse();
				g->word= v1->filters[pos]->word & v2->filters[i]->word;
				g->rank=v1->filters[pos]->rank;

				if (g->word==0)
					delete g;
				else
					filters.push_back(g);
				//on passe aux filterss suivants
				pos++;
				i++;
			}
	}
}
/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
void sparse::and(sparse* v) {

	int pos=0;
	int i=0;
	itemsparse* g;

	while (i<v->filters.dernier) {

		if (pos >= filters.dernier)
			//la liste des filterss courant est epuisee, pas celle de v
			//il ne peut plus y avoir d'autres filterss communs
			break;

		//3 cas
		//a) l'filters a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les itemsparses sont ordonnes...
		if (v->filters[i]->rank < filters[pos]->rank)
			//on passe a l'filters suivant
			i++;
		else
			//b) l'filters a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
			if (v->filters[i]->rank > filters[pos]->rank) {
				//cet filters en pos n'existe pas dans v 
				g=filters.retireElement(pos);
				delete g;
			}
			else {
				//c) c'est le meme filters, on conserve les bits...
				//on se prepare une nouvelle itemsparse
				filters[pos]->word &= v->filters[i]->word;

				if (filters[pos]->word==0) {
					g=filters.retireElement(pos);
					delete g;
				}
				else
					//on passe aux filterss suivants
					pos++;
				i++;
			}
	}
	for (i=filters.dernier-1;i>=pos;i--) {
		g=filters.retire();
		delete g;
	}      
}
/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
int sparse::and(short r, TVector m,int start) {

	int pos=start;
	itemsparse* g;



	while(pos<filters.dernier) {
		//si cet filters n'est pas dans la liste on repart        
		if (r>filters[pos]->rank) {
			g=filters.retireElement(pos);
			delete g;                    
			continue;
		}

		if (r==filters[pos]->rank) {
			filters[pos]->word &= m;

			if (filters[pos]->word==0) {
				g=filters.retireElement(pos);
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
void sparse::and(sparse& v) {

	int pos=0;
	int i=0;
	itemsparse* g;
	int premier=-1;

	while (i<v.filters.dernier) {

		if (pos >= filters.dernier)
			//la liste des filterss courant est epuisee, pas celle de v
			//il ne peut plus y avoir d'autres filterss communs
			break;

		//3 cas
		//a) l'filters a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les itemsparses sont ordonnes...
		if (v.filters[i]->rank < filters[pos]->rank)
			//on passe a l'filters suivant
			i++;            
		else
			//b) l'filters a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
			if (v.filters[i]->rank > filters[pos]->rank) {
				//cet filters en pos n'existe pas dans v 
				g=filters.retireElement(pos);
				delete g;
			}
			else {
				//c) c'est le meme filters, on conserve les bits...
				//on se prepare une nouvelle itemsparse
				filters[pos]->word &= v.filters[i]->word;

				if (filters[pos]->word==0) {
					g=filters.retireElement(pos);
					delete g;
				}
				else
					//on passe aux filterss suivants
					pos++;
				i++;
			}
	}
	//On retire les filterss en queue
	for (i=filters.dernier-1;i>=pos;i--) {
		g=filters.retire();
		delete g;
	}      
}
/*----------------------------------------------------------------------------------------*/
//On renvoie les bits communs entre les deux ensembles
void sparse::and(sparse& v1,sparse& v2) {

	int pos=0;
	int i=0;
	filters.clear();

	while (i<v2.filters.dernier) {

		if (pos >= v1.filters.dernier)
			//la liste des filterss courant est epuisee, pas celle de v
			//il ne peut plus y avoir d'autres filterss communs
			break;

		//3 cas
		//a) l'filters a comparer est place avant, il n'est donc pas compris dans courant, puisque
		//les itemsparses sont ordonnes...
		if (v2.filters[i]->rank < v1.filters[pos]->rank)
			//on passe a l'filters suivant
			i++;
		else
			//b) l'filters a comparer est place apres, il faut donc se deplacer d'un cran dans l'ensemble courant
			if (v2.filters[i]->rank > v1.filters[pos]->rank)
				pos++;
			else {
				//c) c'est le meme filters, on conserve les bits...
				//on se prepare une nouvelle itemsparse
				itemsparse* g= new itemsparse();
				g->word= v1.filters[pos]->word & v2.filters[i]->word;
				g->rank=v1.filters[pos]->rank;

				if (g->word==0)
					delete g;
				else
					filters.push_back(g);
				//on passe aux filterss suivants
				pos++;
				i++;
			}
	}
}

/*----------------------------------------------------------------------------------------*/ 

sparse::sparse() {}

sparse::sparse(itemsparse* g) {    
	filters.push_back(g);    
}


//Duplication d'une sparse...
sparse::sparse(sparse* v) {
	if (v!=NULL) {        
		for (int i=0;i<v->filters.dernier;i++)
			filters.push_back(new itemsparse(v->filters[i]));
	}
}

//Duplication d'une sparse...
sparse::sparse(sparse& v) {
	for (int i=0;i<v.filters.dernier;i++)
		filters.push_back(new itemsparse(v.filters[i]));
}

sparse::~sparse() {
	filters.nettoie();
}


void sparse::table(VECTA<int>& liste) {

	liste.raz();
	//We build in reverse order
	for (int i=filters.dernier-1;i>=0;i--) {
		STVector word=(STVector)filters[i]->word;
		int base = filters[i]->rank*gBitVector;
		for (int k=gBitVector-1;k>=0;k--) {
			//No more bits to seek, we stop
			if (word==0)
				break;
			//the sign bit is the left most bit
			if (word < 0)
				liste.push_back(base+k);
			//we then go to the next filters...shifting bits by 1 to the left
			word <<= 1;
		}
	}
}


int sparse::count() {
	int counter=0;
	//We build in reverse order
	for (int i=filters.dernier-1;i>=0;i--) {
		STVector word=(STVector)filters[i]->word;
		int base = filters[i]->rank*gBitVector;
		for (int k=gBitVector-1;k>=0;k--) {
			//No more bits to seek, we stop
			if (word==0)
				break;
			//the sign bit is the left most bit
			if (word < 0)
				counter++;
			//we then go to the next filters...shifting bits by 1 to the left
			word <<= 1;
		}
	}
	return counter;
}

//We retrait the bit b
void sparse::remove(int b) {
	int rank= Rank(b);
	TVector word=  BitPos(b,rank);

	//We look for the first filters of rank=rank
	for (int i=0;i<filters.dernier;i++) {
		if (filters[i]->rank==rank) {
			//We XOR the bit
			filters[i]->word^=word;
			if (filters[i]->word==0) {
				//the filters does not contain any bit anymore
				//we retrait it
				itemsparse* g=filters.retireElement(i);
				delete g;
			}
			return;
		}

		//The filterss are ordered...
		if (rank > filters[i]->rank)
			return;
	}    
}

bool sparse::same(sparse& v) {
	if (filters.dernier!=v.filters.dernier)
		return false;
	for (int i=0;i<filters.dernier;i++) {
		if (filters[i]->rank!=v.filters[i]->rank)
			return false;
		if (filters[i]->word!=v.filters[i]->word)
			return false;
	}
	return true;
}

bool sparse::same(sparse* v) {
	if (filters.dernier!=v->filters.dernier)
		return false;
	for (int i=0;i<filters.dernier;i++) {
		if (filters[i]->rank!=v->filters[i]->rank)
			return false;
		if (filters[i]->word!=v->filters[i]->word)
			return false;
	}
	return true;
}

