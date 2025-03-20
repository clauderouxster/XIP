#ifndef bnf_compileC_h
#define bnf_compileC_h
#include "x_node.h"



class bnf_compileC {
public:

map<long,long> x_recordcr;
map<string,int> compteurs;
map<string,int> compteursessai;
char gFail;
int baseline;
x_reading* fx;

bnf_compileC(int l=0) {
    baseline=l;
    fx=NULL;
}

void x_decompte(string s) {
    if (compteurs.find(s)==compteurs.end())
        compteurs[s]=0;
    compteurs[s]+=1;
}

void x_decompte_essai(string s) {
    if (compteursessai.find(s)==compteursessai.end())
        compteursessai[s]=0;
    compteursessai[s]+=1;
}

void x_addlinenumber(int i) {
	for (int k=0;k<i;k++)
		x_recordcr[k]=k;
}

long x_linenumber(long off) {
	map<long,long>::iterator lower;
	lower=x_recordcr.lower_bound(off);
	if (lower!=x_recordcr.begin())
		lower--;
	return lower->second;
}


void setfail( string lab,long pos,char test,int errornumber) {
    if (test) {
		if (fx->lastposerror==0) {
			gFail=1;
			fx->lastposerror=pos;
			fx->labelerror=lab;
			fx->errornumber=errornumber;
		}
	}
}


void x_pop_node(x_node** tree,int addsubtree) {
	if (*tree!=NULL) {
		if ((*tree)->init==0) {
			delete *tree;
			*tree=NULL;
		}
		else
		 (*tree)->pop(addsubtree);
	}
}

void x_init_node(string& lreturn,string& lret,x_node** tree,string name,long d,long e) {
    if (lret.size()>0) {
        if (*tree==NULL)
            *tree=new x_node;
        (*tree)->start=d;
        (*tree)->end=e;
        (*tree)->token=name;
        (*tree)->init=1;
        (*tree)->value=lret;
        lreturn+=lret;
    }
}

void x_init_tree(x_node** tree,x_node* subtree,int& addsubtree) {
    if (subtree==NULL)
        return;
    if (*tree==NULL)
       *tree=new x_node;
    if ((*tree)->append(subtree)==0)
        delete subtree;
    else
        addsubtree++;
}

char x_test_string(string& lret,int sz,string sub) {
  long pos=fx->tell();
  string lst=fx->read(sz);
  if (lst.size()<sz) {
      fx->seek(pos);
      return(0);
  }
  if (lst==sub) {
      lret+=lst;
      return(1);
  }
  fx->seek(pos);
  return(0);
  }

void addrecord(unsigned char x,long pos) {
  if (x=='\n')
      x_recordcr[pos]=1;
}

char x_test_char(string& lret,unsigned char sub) {
  long pos=fx->tell();
  string lst=fx->read(1);
  if (lst.size()!=1) {
      fx->seek(pos);
      return(0);
  }
  unsigned char x=lst[0];
  addrecord(x,pos);
  if (x==sub) {
      lret+=lst;
      return(1);
  }
  fx->seek(pos);
  return(0);
  }


char x_test_in(string& lret,char* sub) {
  long pos=fx->tell();
  string lst=fx->read(1);
  if (lst.size()!=1) {
      fx->seek(pos);
      return(0);
  }
  char x=lst[0];
  addrecord(x,pos);
  if (strchr(sub,x)!=NULL) {
      lret+=lst;
      return(1);
  }
  fx->seek(pos);
  return(0);
  }


char x_test_ord(string& lret,unsigned char b,unsigned char g) {
  long pos=fx->tell();
  string lst=fx->read(1);
  if (lst=="") {
     fx->seek(pos);
     return(0);
  }
  unsigned char x=lst[0];
  addrecord(x,pos);
  
  if (x>=b && x<=g) {
     lret+=lst;
     return(1);
  }
  fx->seek(pos);
  return(0);
  }

char x_test_any(string& lret) {
  long pos=fx->tell();
  string lst=fx->read(1);
  if (lst=="") {
     fx->seek(pos);
     return(0);
  }
  unsigned char x=lst[0];
  addrecord(x,pos);  
  lret+=lst;
  return(1);
}


void x_displaytree(x_node* tree,int depth);
void extractmax(x_node* tree,long& themax);
void extracttext(long themax);
bool x_complong(const long premier,const long  second);
x_node* x_parsing(x_reading*,x_parsing_mode mode);
x_node* x_parse_string(string x,x_parsing_mode mode);
x_node* x_parse_file(string x,x_parsing_mode mode);


};
#endif
