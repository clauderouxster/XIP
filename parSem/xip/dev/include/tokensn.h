#ifndef bnf_tokens_h
#define bnf_tokens_h
#include "x_node.h"


class bnf_tokens {
public:


int currentpos;
int intoken;
int lineerror;
string labelerror;
int errornumber;
char gFail;
int baseline;
x_reading* fx;

vector<string> labelerrors;
vector<int> lineerrors;
vector<int> errornumbers;

bnf_tokens(int l=0) {
    baseline=l;
    fx=NULL;
    intoken=0;
    currentpos=0;
    
}

void initialize(x_reading* xr) {
   labelerrors.clear();
   lineerrors.clear();
   errornumbers.clear();

    baseline=0;
    fx=xr;
    gFail=0;
    intoken=0;
    currentpos=0;
}

void setfail(char test) {
    if (test)
        gFail=1;
}

void initfail(string lab,long pos,int error) {
        if (gFail==1)
            return;
	if (pos>=fx->stackln.size()) {
		lineerror=-1;
		labelerror=lab;
		errornumber=error;
		return;
	}
    lineerror=fx->stackln[pos]+baseline;
    labelerror=lab;
    errornumber=error;
    labelerrors.push_back(labelerror);
    lineerrors.push_back(lineerror);
    errornumbers.push_back(errornumber);    
}

void poperrors() {
	if (!gFail) {
		labelerrors.pop_back();
		lineerrors.pop_back();
		errornumbers.pop_back();
	}
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

inline void x_init_node(string& lreturn,string& lret,x_node** tree,string name,long d,bool conc) {
    if (lret.size()>0 || conc==false) {
        if (*tree==NULL)
            *tree=new x_node;
        (*tree)->start=d;
        (*tree)->end=d;
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
char x_test_pstring(string& lret,int sz,string& sub);

void incrementintoken() {
	intoken++;
	if (intoken==fx->stack[currentpos].size()) {
		currentpos++;
		intoken=0;
	}
}

void incrementpos() {
    currentpos++;
    intoken=0;

}


inline char x_test_char(string& lret,unsigned char sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	unsigned char x=fx->stack[currentpos][intoken];
	if (x==sub) {
		incrementintoken();
		lret+=x;
		return(1);
	}
  return(0);
 }


inline char x_test_in(string& lret,char* sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	char x=fx->stack[currentpos][intoken];
	if (strchr(sub,x)!=NULL) {
		lret+=x;
		incrementintoken();
		return(1);
	}
	return(0);
}


inline char x_test_ord(string& lret,unsigned char b,unsigned char g) {
	if (currentpos>=fx->stack.size())
		return(0);
	unsigned char x=fx->stack[currentpos][intoken];
	if (x>=b && x<=g) {
		incrementintoken();
		lret+=x;
		return(1);
	}
	return(0);
}

inline char x_test_any(string& lret) {
	if (currentpos>=fx->stack.size())
		return(0);
	char x=fx->stack[currentpos][intoken];
	incrementintoken();
	lret+=x;
	return(1);
}


char m_EOF(string& lreturn,x_node** tree) {
	if (currentpos>=fx->stack.size())
		return(1);
	return(0);
}

long x_linenumber(long p) {
	if (p<fx->stackln.size())
		return (fx->stackln[p]+baseline);
	return -1;
}

string x_errormsg(int i);
x_node* x_parsing(x_reading* xr,x_parsing_mode mode,bool display=true);



char m_exposant(string& lreturn,x_node** tree);
char m_exposant_0_1(string& lreturn,x_node** tree);
char m_exposant_0_2(string& lreturn,x_node** tree);
char m_exposant_0_2_3(string& lreturn,x_node** tree);
char m_exposant_0_2_3_4(string& lreturn,x_node** tree);
char m_exposant_0_5(string& lreturn,x_node** tree);
char m_enumber(string& lreturn,x_node** tree);
char m_enumber_0_1(string& lreturn,x_node** tree);
char m_enumber_0_1_2(string& lreturn,x_node** tree);
char m_fnumber(string& lreturn,x_node** tree);
char m_fnumber_0_1(string& lreturn,x_node** tree);
char m_fnumber_0_1_2(string& lreturn,x_node** tree);
char m_char(string& lreturn,x_node** tree);
char m_http(string& lreturn,x_node** tree);
char m_http_0_1(string& lreturn,x_node** tree);
char m_address(string& lreturn,x_node** tree);
char m_address_0_1(string& lreturn,x_node** tree);
char m_address_0_1_2(string& lreturn,x_node** tree);
char m_token(string& lreturn,x_node** tree);
char m_eparse(string& lreturn,x_node** tree);
char m_eparse_0_1(string& lreturn,x_node** tree);
char m_fparse(string& lreturn,x_node** tree);
char m_fparse_0_1(string& lreturn,x_node** tree);
char m_nonspace(string& lreturn,x_node** tree);
};

class bnf_tokens_pop {
public:
	bnf_tokens* pointer;

	bnf_tokens_pop(bnf_tokens* b) {
		pointer=b;
	}

	~bnf_tokens_pop() {
		pointer->poperrors();
	}
};


#endif
