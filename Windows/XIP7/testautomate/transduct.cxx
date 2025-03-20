#include <ostream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;
#define TailleChaine 512
#define NON 0
#define OUI 1

#include "vecta.h"
#include "transduct.h"
#include "automaton.h"
void Trim(char* chaine);

unsigned char spaces[]={9,13,32,160,0};
unsigned char separators[]= {',',';','.',':','!','?','-','_','"',39,'`','(',')','[',']','{','}','^','=','/','\\','|','*','+','%','$','£','#','<','>','~','&',' ','«','»','°',10,0};
unsigned char specials[]={9,13,32,160,',',';','.',':','!','?','-','_','"',39,'`','(',')','[',']','{','}','^','=','/','\\','|','*','+','%','$','£','#','<','>','~','&',' ','«','»','°',10,0};



char* returnstring(arc* a) {
	
	static char s[2];
	switch(a->action) {
    case AUTOMATONANY:
        return "";
    case AUTOMATONDIGIT:      
		return "0123456789";
    case AUTOMATONPUNCT:
        return "<>{}[](),;:.&|!/\\=~#@^?+-*$%\"'_¬£€`";
    case AUTOMATONLETTER:
		return "abcdefghijklmnopqrstuvwxyz";
    case AUTOMATONLETTERUPPER:
        return "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    case AUTOMATONLABEL:
		s[0]=a->label;
		s[1]=0;
		return s;
    }
	return NULL;
}




char arctrans::test(unsigned char c) {
	if (c==u)
		return OUI;
	return NON;
}

char arctransarc::test(unsigned char c) {
	return a->test(c);
}


void construitinitiaux(etat* e,transducteur* t,int ref) {

	for (int i=0;i<e->arcs.dernier;i++) {
		if (e->arcs[i]->marque==100)
			continue;
		e->arcs[i]->marque=100;
		if (e->arcs[i]->action==AUTOMATONEPSILON)
			construitinitiaux(e->arcs[i]->e,t,ref);
		else {
			unsigned char* str=(unsigned char*)returnstring(e->arcs[i]);
			int j;
			if (str==NULL)
				continue;
			if (str[0]==0) {
				for (j=0;j<256;j++)
					t->initiaux[j].ajoute(e->arcs[i]->e->id+ref);
			}
			else{
				int l=strlen((char*)str);
				for (j=0;j<l;j++)
					t->initiaux[str[j]].ajoute(e->arcs[i]->e->id+ref);
			}
		}
	}
}

int transducteur::ajouteterminaison(char* tampon,char test) {
	if (test==OUI) {
		etat* e=aterminaison->recherche(tampon);
		if (e!=NULL)
			return (int)e->valeur;
	}
	char* m=new char[strlen(tampon)];		
	strcpy(m,tampon);
	aterminaison->parsebrute((unsigned char*)tampon,(void*)terminaisons.dernier);
	terminaisons.ajoute(m);
	return (terminaisons.dernier-1);
}

//On cree une duplication de notre automate dans le transducteur
void transducteur::maj(automate* a) {
	int ref=arcs.dernier;
	int i,j;
	arctransarc* ar;
	VECTA<arc*> garde;
	etat* et;
	VECTA<arctrans*> finaux;
	arctrans* arfinal;

	for (i=-1;i<a->etats.dernier;i++) {
		ar=new arctransarc;
		i==-1?et=&a->e:et=a->etats[i];
		garde.raz();	

		//Creation d'un noeud terminal, que l'on connectera en fin d'analyse
		if (et->valeur!=NULL) {
			char* tampon=(char*)et->valeur;
			arfinal=new arctrans;
			arfinal->id=ajouteterminaison(tampon,1);
			finaux.ajoute(ar);
			finaux.ajoute(arfinal);
			//On se reserve une case d'ajout pour plus tard
			garde.ajoute(a->e.arcs[0]);
		}			
		//Dupliquation des arcs
		for (j=0;j<et->arcs.dernier;j++) {
			if (et->arcs[j]->manipulation==AUTOMATONDELETED ||
				et->arcs[j]->manipulation==AUTOMATONMARKEDFORDELETION) 
				continue;
			garde.ajoute(et->arcs[j]);
		}

		if (garde.dernier) {
			ar->next=new int[garde.dernier+1];
			ar->next[0]=garde.dernier;
			for (j=0;j<garde.dernier;j++)
				ar->next[j+1]=garde[j]->e->id+ref;
		}
		arcs.ajoute(ar);
	}

	for (i=0;i<finaux.dernier;i+=2) {
		ar=(arctransarc*)finaux[i];
		arfinal=finaux[i+1];
		//On le met en tete
		ar->next[1]=arcs.dernier;
		arcs.ajoute(arfinal);
	}

	for (i=-1;i<a->etats.dernier;i++) {
		i==-1?et=&a->e:et=a->etats[i];
		if (et->arcs.dernier!=0) {
			for (j=0;j<et->arcs.dernier;j++) {
				arcs[et->arcs[j]->e->id+ref]->add(et->arcs[j]);
			}
		}
	}
	

	construitinitiaux(&a->e,this,ref);
}

transducteur::~transducteur() {
	delete aterminaison;
	terminaisons.nettoie();
	arcs.nettoie();
	rgx.nettoie();
}


transducteur::transducteur(char* n) {
	aterminaison=new automate;
	nom=strdup(n);
	ifstream entree(n,ios::in|ios::binary);
	char tampon[1000];
	tampon[0]=0;
	while (!entree.eof() && strcmp(tampon,"lexicons:")) {
		entree.getline(tampon,1000);
		Trim(tampon);
	}
	while (!entree.eof()) {
		entree.getline(tampon,1000);
		Trim(tampon);
		if (!strcmp(tampon,"vocabulary:"))
			break;
		if (tampon[0]!=0)
			lexiques.push_back(tampon);
	}

	liretrans();
	automate* a;
	int i;
	while (!entree.eof()) {
		entree.getline(tampon,1000);
		Trim(tampon);
		if (tampon[0]==0)
			continue;
		if (tampon[0]!=34) {
			cout<<"Erreur";
			break;
		}
		i=1;
		while (tampon[i]!=34 && tampon[0]) {
			if (tampon[i]=='\\')
				i++;
			i++;
		}
		tampon[i]=0;
		i++;		
		while (tampon[i]!=34) i++;
		i++;
		int debut=i;
		while (tampon[i]!=34 && tampon[0]) i++;
		tampon[i]=0;
		int idt=ajouteterminaison(tampon+debut,1);
		if (chainecomplexe((unsigned char*)tampon+1,0,ARCINITIAL)==OUI) {
			a=new automate;			
			a->parse((unsigned char*)tampon+1,(void*)terminaisons[idt]);
			a->aupluspres();
			maj(a);
			rgx.ajoute(a);
		}
		else {//c'est un mot simple a integrer
			VECTA<int>& ia=initiaux[tampon[1]];		
			char pc=tampon[1];
			arctrans* atret=NULL;
			int dernier=2;
			arctrans* at=NULL;
			for (int j=0;j<ia.dernier;j++) {
				at=arcs[ia[j]];
				if (pc==at->u && pc==at->l) {
					atret=at;
					break;
				}
			}

			if (atret!=NULL) {
				atret=ajoutemot(at,tampon+2,0,idt,dernier);
				dernier+=2;//Nous avons une base 2
			}
			else {
				atret=new arctrans;
				atret->u=tampon[1];
				atret->l=tampon[1];
				initiaux[tampon[1]].ajoute(arcs.dernier);
				arcs.ajoute(atret);
			}


			//dernier pointe sur le dernier caractere					
			while (tampon[dernier]!=0) {
				at=new arctrans;
				at->u=tampon[dernier];
				at->l=tampon[dernier];
				if (atret->next==NULL) {
					atret->next=new int[2];
					atret->next[0]=1;
					atret->next[1]=arcs.dernier;
				}
				else {
					int* nn=atret->next;
					atret->next=new int[nn[0]+2];
					atret->next[nn[0]+1]=0;
					int ar=1;
					for (ar=1;ar<=nn[0];ar++) {
						if (arcs[nn[ar]]->u>=tampon[dernier])
							break;
						atret->next[ar]=nn[ar];
					}
					atret->next[ar]=arcs.dernier;
					for (;ar<=nn[0];ar++)
						atret->next[ar+1]=nn[ar];
					atret->next[0]=nn[0]+1;
					delete nn;
				}
				arcs.ajoute(at);
				atret=at;
				dernier++;
			}
			at=new arctrans;
			at->id=idt;
			atret->next=new int[2];
			atret->next[0]=1;
			atret->next[1]=arcs.dernier;
			arcs.ajoute(at);
		}
	}
}


void transducteur::lookup(char* mts) {
	int i;
	char res[1000];
	arctrans* a;
	results.nettoie();
	VECTA<int>& ia=initiaux[mts[0]];		
	res[0]=mts[0];
	for (i=0;i<ia.dernier;i++) {
		a=arcs[ia[i]];	
		cherche(a,mts,1,strlen(mts),res,1);
	}
	for (i=0;i<results.dernier;i++)
		cout<<results[i]<<endl;
}



void transducteur::parscoursdesarcs(arctrans* a,char* surface,char* res,int p) {
	char retour=0;

	if (a->next!=NULL) {
		int t=a->next[0];
		int* n=a->next+1;
		for (int j=0;j<t;j++) {
			arctrans* an=arcs[n[j]];
			if (an->u==1) {
				strcat(res+p,"+");
				strcat(res+p,terminaisons[an->l]);
				parscoursdesarcs(an,surface,res,p+1);
			}
		}
	}
	else
		cout<<surface<<"\t"<<res<<endl;
}



void affichearc(VECTA<parcoursarc*>& liste,transducteur* t,char* surface,VECTA<int>& ids) {
	char res[1000];
	res[0]=0;
	int p=0;
	for (int i=0;i<liste.dernier;i++) {
		arctrans* a=liste[i]->a;
		if (a->id!=-1) {
			if (ids.cherche(a->id)==-1) {
				ids.ajoute(a->id);
				strcat(res+p,"\t");
				strcat(res,t->terminaisons[a->id]);
				cout<<surface<<"\t"<<res<<endl;
				t->results.ajoute(strdup(res));
			}
			return;
		}

		if (a->l==2) {
			strcat(res+p,"\t");
			t->parscoursdesarcs(a,surface,res,p);
		}
		else {
			if (a->type()==1) {
				if (!i)
					strcpy(res,surface);
			}
			else {
				res[p++]=a->l;
				res[p]=0;
			}
		}
	}
}

void construitchemin(parcoursarc* pa,VECTA<parcoursarc*>& liste) {
	if (pa==NULL)
		return;
	construitchemin(pa->pere,liste);
	liste.ajoute(pa);
}

lecteur* transducteur::fournir(char type,char* n) {
	lecteur* fe;
	if (type==0) //string
		fe=new lecteur(n);
	else
		fe=new lecteurfichier(n);
	return fe;
}



void transducteur::lire(lecteur* lect) {

	int i=0;
	int j;
	unsigned char c;
	VECTA<parcoursarc*> courants;
	VECTA<parcoursarc*> suivants;
	VECTA<parcoursarc*> piles[1000];
	VECTA<parcoursarc*> chaines;
	char res[1000];
	VECTA<int> ids;
	char initial=1;
	int nbseps=0;
	int nbchars=0;
	int compteurbouc=0;

	c=lect->lit();
	char boucle=1;
	while (boucle) {
		res[i]=c;
		res[i+1]=0;
		compteurbouc++;
		cherchearcs(courants,c,i,suivants,chaines,initial);
		if (strchr((char*)specials,c))
			nbseps++;
		if (res[0]==10)
			cout<<"+CR"<<endl;
		//dans le cas ou on ne trouve rien et qu'en plus ce n'est pas un separateur
		//on accumule
		initial=0;
		if (suivants.dernier==0) {
			if (!nbseps) {
				//S'il s'agit d'un caractere seul, il est evidemment impossible que rien de bon n'en sorte
				if (i!=0) {
					c=lect->lit();
					while (!strchr((char*)specials,c)) {
						res[i++]=c;
						c=lect->lit();
					}
					res[i]=0;
					lect->metattente(c);					
				}
				cout<<res<<"\t"<<res<<"\t+?"<<endl;
			}
			else {	
				int trouve=i+1;
				int idx=i-1;
				if (idx==-1 && (strchr((char*)spaces,c) || strchr((char*)separators,c)))
						nbseps=0;

				for (;idx>=0;idx--) {
					char cc=res[idx+1];
					res[idx+1]=0;
					if (strchr((char*)spaces,cc) || strchr((char*)separators,cc))
						nbseps--;

					//On recule etape par etape
					//On nettoie les courants, pour les repositionner sur les etats precedents
					courants.affecte(piles[idx]);
					if (cherchearcs(courants,0,idx,suivants,chaines,0)==1) {	
						ids.raz();

						//On recherche d'abord les ancetres
						for (j=0;j<courants.dernier;j++) {
							for (int k=0;k<courants[j]->pile.dernier;k++) {
								suivants.raz();
								construitchemin(courants[j]->pile[k],suivants);
								if (suivants[0]->premier==1)
									affichearc(suivants,this,res,ids);
							}
						}
						trouve=idx+1;
						res[trouve]=cc;
						break;
					}
					if (!nbseps) {
						cout<<res<<"\t"<<res<<"\t+?"<<endl;
						trouve=idx+1;
						res[trouve]=cc;
						break;
					}
					res[idx+1]=cc;
				}
				//Nous avons donc des caracteres a conserver a partir de idx
				for (idx=i;idx>=trouve;idx--)
					lect->metattente(res[idx]);
			}
			results.nettoie();
			courants.raz();
			suivants.raz();
			chaines.nettoie();
			i=0;
			initial=1;
		}
		else {
			piles[i].affecte(suivants);
			courants.affecte(suivants);
			suivants.raz();
			i++;
		}
		if (lect->end()) {
			if (i==0)
				break;
			c=0;
		}
		else
			c=lect->lit();
		nbchars++;
	}
	delete lect;
}


char transducteur::cherchearcs(VECTA<parcoursarc*>& courants,
							   unsigned char c,
							   int i,
							   VECTA<parcoursarc*>& suivants,
							   VECTA<parcoursarc*>& chaines,
							   char initial) {
	int t,j;
	int* n;
	arctrans* an;
	unsigned char normalize=tolower(c);
	unsigned char loop[2];
	int nb=1;
	parcoursarc* pac;
	int l=0;
	if (normalize!=c) {
		if (normalize>c) {
			loop[1]=c;
			loop[0]=normalize;
		}
		else {
			loop[0]=c;
			loop[1]=normalize;
		}
		nb=2;
	}
	else {
		loop[0]=c;
		loop[1]=0;
	}

	char retour=0;
	if (initial==1) {
		for (l=0;l<nb;l++) {
			VECTA<int>& ia=initiaux[loop[l]];		
			for (t=0;t<ia.dernier;t++) {
				pac=new parcoursarc(arcs[ia[t]],1);
				suivants.ajoute(pac);	
				chaines.ajoute(pac);
				retour=1;
			}
		}
		return(retour);
	}
	
	int pos=0;
	
	//Dans le cas d'un separateur trouve
	if (c==0) {
		for (j=0;j<courants.dernier;j++) {
			pac=courants[j];
			arctrans* a=pac->a;
			if (a->next!=NULL) {
				t=a->next[0];
				n=a->next+1;
				for (int k=0;k<t;k++) {
					an=arcs[n[k]];
					if (an->id!=-1 || an->u==1) {
						char tr=0;
						for (int m=0;m<pac->pile.dernier;m++) {
							if (pac->pile[m]->a==an) {
								tr=1;
								break;
							}
						}
						if (tr==0)
							pac->ajoute(new parcoursarc(an));
						retour=1;
					}
				}	
			}
		}
		return(retour);	
	}

	
	VECTA<int> gardes(10);
	for (j=0;j<courants.dernier;j++) {
		pac=courants[j];
		arctrans* a=pac->a;
		char ajout=0;
		if (a->next!=NULL) {
			t=a->next[0];
			n=a->next+1;
			for (int j=0;j<t;j++) {
				an=arcs[n[j]];
				if (gardes.cherche(n[j])!=-1)
					continue;
				if (an->test(loop[0]) || an->test(loop[1])) {
					//On rajoute ces liens au noeuds deja present	
					parcoursarc* pa=NULL;
					ajout=1;
					gardes.ajoute(n[j]);
					pa=new parcoursarc(an);
					chaines.ajoute(pa);
					pa->idx=n[j];
					pac->ajoute(pa);						
					suivants.ajoute(pa);
					retour=1;
				}
				else
					if (an->type()==0 && an->u>loop[0])
						break;
			}
		}
	}
	return(retour);
}

void transducteur::cherche(arctrans* a,char* mot,int i,int s,char* r,int ir) {
	int t;
	int* n;
	arctrans* an;

	if (i>=s) {
		t=a->next[0];
		n=a->next+1;
		for (int j=0;j<t;j++) {
			an=arcs[n[j]];
			if (an->id!=-1) {
				r[ir]=0;
				strcat(r,terminaisons[an->id]);
				results.ajoute(strdup(r));
				trouve=1;
			}
			else
				break;
		}	
		return;
	}

	if (a->next!=NULL) {
		t=a->next[0];
		n=a->next+1;
		for (int j=0;j<t;j++) {
			an=arcs[n[j]];
			if (an->test(mot[i])) {
				r[ir]=an->l;
				cherche(an,mot,i+1,s,r,ir+1);
			}
			else
				if (an->type()==0 && an->u>mot[i])
					break;
		}
	}
}

arctrans* transducteur::ajoutemot(arctrans* a,char* mot,int i,int id,int& dernier) {
	int t;
	int* n;
	arctrans* an;

	if (mot[i]==0) {
		t=a->next[0];
		n=a->next+1;
		for (int j=0;j<t;j++) {
			an=arcs[n[j]];
			if (an->id!=-1 || an->l==-1) {
				if (an->id==id)
					return NULL;
				return an;
			}
		}
		return a;
	}

	if (a->next!=NULL) {
		t=a->next[0];
		n=a->next+1;
		for (int j=0;j<t;j++) {
			an=arcs[n[j]];
			if (an->type()!=0)
				continue;
			if (mot[i]==an->u && mot[i]==an->l)
				return ajoutemot(an,mot,i+1,id,dernier);
		}
	}
	dernier=i;
	return a;
}

int transducteur::lirecode(ifstream& fichier,unsigned char& c) {
	static char tampon[1000];
	int i=0;
	c=fichier.get();
	int mult=1;
	if (c=='-') {
		mult=-1;
		c=fichier.get();
	}
	while (c>='0' && c<='9') {
		tampon[i++]=c;
		c=fichier.get();
	}
	tampon[i]=0;
	return atoi(tampon)*mult;
}


void transducteur::liretrans() {
	for (size_t n=0;n < lexiques.size();n++) {
		cout<<"Loading:"<<lexiques[n].c_str()<<endl;
		ifstream fichier(lexiques[n].c_str(),ios::in|ios::binary);
		//Tout d'abord on lit la taille des terminaisons
		char tampon[1000];
		fichier.getline(tampon,1000);
		int thesize=atoi(tampon);
		terminaisons.ajuste(thesize);
		while (!fichier.eof()) {
			fichier.getline(tampon,1000);
			Trim(tampon);
			if (tampon[0]==0)
				break;
			ajouteterminaison(tampon,0);
		}

		unsigned char c;	
		arctrans* a;
		int i=0;
		int k;
		VECTA<int> nn;
		c=fichier.get();
		//On lit les etats initiaux
		while(!fichier.eof() && c!='e') {
			//On lit maintenant l'automate
			if (c=='i') {
				i=lirecode(fichier,c);
				if (c=='a') {
					k=lirecode(fichier,c);
					initiaux[i].ajoute(k);
				}
			}
		}
		thesize=lirecode(fichier,c);
		int affichepoint=thesize/50;
		arcs.ajuste(thesize);
		while (!fichier.eof()) {
			if (c!='u' && c!='x') {
				cout<<"Erreur"<<endl;
				exit(-1);
			}
			if (c=='u' || c=='x') {
				//Debut d'un arc
				a=new arctrans;
				//ce qui suit est soit un code, soit un l
				switch(c) {
						case 'u':
							c=fichier.get();
							a->u=c;
							c=fichier.get();
							a->l=c;
							break;
						case 'x':
							c=fichier.get();
							a->u=c;
							a->l=c;
							break;
				}
				c=fichier.get();
				nn.raz();
				while (c=='n')
					nn.ajoute(lirecode(fichier,c));
				if (nn.dernier!=0) {
					a->next=new int[nn.dernier+1];
					a->next[0]=nn.dernier;
					for (i=0;i<nn.dernier;i++)
						a->next[i+1]=nn[i]+arcs.dernier;
				}
			}
			arcs.ajoute(a);
			a=NULL;
			if (arcs.dernier%affichepoint==0)
				cout<<".";
		}
	}
	cout<<endl;
}


