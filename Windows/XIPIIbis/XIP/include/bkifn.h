#ifndef bnf_bkif_h
#define bnf_bkif_h
#include "x_node.h"



class bnf_bkif {
public:

map<long,long> x_recordcr;
map<string,int> compteurs;
map<string,int> compteursessai;
map<long,string> tokens;
map<long,string> numbers;
map<long,string> doublestring;
map<long,string> simplestring;

char gFail;
int baseline;
x_reading* fx;

bnf_bkif(int l=0) {
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
	if (x_recordcr.size()==0)
		return 0;
	map<long,long>::iterator lower;
	lower=x_recordcr.lower_bound(off);
	if (lower!=x_recordcr.begin()) {
		if (lower==x_recordcr.end()) {
			lower--;
			return lower->second+1;
		}
	}
	return lower->second;
}

void setfail(char test) {
    if (test)
        gFail=1;
}

void initfail(string lab,long pos,int errornumber) {
    fx->lastposerror=pos;
    fx->labelerror=lab;
    fx->errornumber=errornumber;
}

inline void x_pop_node(x_node** tree,int addsubtree) {
	if (*tree!=NULL) {
		if ((*tree)->init==0) {
			delete *tree;
			*tree=NULL;
		}
		else
		 (*tree)->pop(addsubtree);
	}
}

inline void x_init_node(string& lreturn,string& lret,x_node** tree,string name,long d,long e,bool conc) {
    if (lret.size()>0 || conc==false) {
        if (*tree==NULL)
            *tree=new x_node;
        (*tree)->start=d;
        (*tree)->end=e;
        (*tree)->token=name;
        (*tree)->init=1;        
        if (conc) {
            (*tree)->value=lret;
            lreturn+=lret;
        }
    }
}


inline void x_init_tree(x_node** tree,x_node* subtree,int& addsubtree) {
    if (subtree==NULL)
        return;
    if (*tree==NULL)
       *tree=new x_node;
    if ((*tree)->append(subtree)==0)
        delete subtree;
    else
        addsubtree++;
}

char x_test_string(string& lret,int sz,string sub);

void addrecord(unsigned char x,long pos) {
  if (x=='\n')
      x_recordcr[pos]=1;
}

inline char x_test_char(string& lret,unsigned char sub) {
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

char m_space(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	static const char label[]="space";
	static char espaces[]={10,13,9,32,0};
	string lret;
	long pos=fx->tell();
	while (!fx->eof()) {
		lret=fx->read(1);
		if (lret=="" || strchr(espaces,lret[0])==NULL)
			break;
		pos++;
	}
	fx->seek(pos);
	return(1);
}

char m_spaceeof(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	static char espaces[]={10,13,9,32,0};
	string lret;
	while (!fx->eof()) {
		lret=fx->read(1);
		if (fx->eof() || lret=="" || strchr(espaces,lret[0])==NULL)
			break;
	}
	return(1);
}

char m_EOF(string& lreturn,x_node** tree) {
  if (fx->eof())
      return(1);
  long pos=fx->tell();
  fx->read(1);
  if (fx->eof())
      return(1);
      
   fx->seek(pos);
   return(0);
}

inline char x_test_in(string& lret,char* sub) {
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


inline char x_test_ord(string& lret,unsigned char b,unsigned char g) {
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

inline char x_test_any(string& lret) {
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

string x_errormsg(int i);
void x_displaytree(x_node* tree,int depth);
void extractmax(x_node* tree,long& themax);
void extracttext(long themax);
bool x_complong(const long premier,const long  second);
x_node* x_parsing(x_reading*,x_parsing_mode mode);
x_node* x_parse_string(string x,x_parsing_mode mode);
x_node* x_parse_file(string x,x_parsing_mode mode);


char m_rc(string& lreturn,x_node** tree);
char m_rc_0(string& lreturn,x_node** tree);
char m_shell(string& lreturn,x_node** tree);
char m_shell_1(string& lreturn,x_node** tree);
char m_shell_1_2(string& lreturn,x_node** tree);
char m_shell_1_3(string& lreturn,x_node** tree);
char m_commentaires(string& lreturn,x_node** tree);
char m_commentaires_4(string& lreturn,x_node** tree);
char m_commentaires_4_6(string& lreturn,x_node** tree);
char m_char(string& lreturn,x_node** tree);
char m_char_8(string& lreturn,x_node** tree);
char m_digit(string& lreturn,x_node** tree);
char m_ifor(string& lreturn,x_node** tree);
char m_ifor_9(string& lreturn,x_node** tree);
char m_ifor_9_10(string& lreturn,x_node** tree);
char m_ifor_9_12(string& lreturn,x_node** tree);
char m_ifor_9_13(string& lreturn,x_node** tree);
char m_ifi(string& lreturn,x_node** tree);
char m_ifi_15(string& lreturn,x_node** tree);
char m_ifi_15_19(string& lreturn,x_node** tree);
char m_ifi_15_21(string& lreturn,x_node** tree);
char m_ifi_15_21_22(string& lreturn,x_node** tree);
char m_iloop(string& lreturn,x_node** tree);
char m_iloop_26(string& lreturn,x_node** tree);
char m_iswitch(string& lreturn,x_node** tree);
char m_iswitch_31(string& lreturn,x_node** tree);
char m_iswitch_31_36(string& lreturn,x_node** tree);
char m_itry(string& lreturn,x_node** tree);
char m_itry_39(string& lreturn,x_node** tree);
char m_itry_39_45(string& lreturn,x_node** tree);
char m_itry_39_48(string& lreturn,x_node** tree);
char m_itry_39_48_49(string& lreturn,x_node** tree);
char m_instruction(string& lreturn,x_node** tree);
char m_instruction_53(string& lreturn,x_node** tree);
char m_instruction_53_55(string& lreturn,x_node** tree);
char m_instruction_53_55_56(string& lreturn,x_node** tree);
char m_instruction_53_55_57(string& lreturn,x_node** tree);
char m_instruction_53_55_58(string& lreturn,x_node** tree);
char m_instruction_53_55_59(string& lreturn,x_node** tree);
char m_instruction_53_55_60(string& lreturn,x_node** tree);
char m_word(string& lreturn,x_node** tree);
void loadtoken(x_reading* xr);
};
#endif
