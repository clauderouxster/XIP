# -*- coding: cp1252 -*-
import rulen
import sys

debugmode=False
classname=""
donotconcatenate=[]

#we use the following rules for pre-tokenization: <,>,^,= --->  string in double quote, string in simple quote, token, number

def tl(x):
    return (type(x)==type([]))
def td(x):
    return (type(x)==type({}))
        

def applyrule(r,tree):
    lreturn=[]
    s=rulen.readstring(r)
    return rulen.m_regle(s,lreturn,tree,0)


def explore(tree,nb,sub):
    if "%NODES" in tree:
        lst=tree["%NODES"]
        for l in lst:
            treeanalysis(l,nb,sub)

def insertion(struct,sub,operator):
    struct.insert(0,operator)
    coupe=None
    if '$' in sub[0]:
        coupe=sub[0]
        sub=sub[1:]
    if sub[0][0] in ["&","|","()","{}","^"]:
        if sub[0][0]==operator:
            struct.extend(sub[0][1:])
        else:
            struct.extend(sub)
    else:
        struct.extend(sub[0])
    if coupe!=None:
        x=coupe["$"][1:]
        struct.insert(1,{"$":x})
        
        


def treeanalysis(tree,nb,struct):
    #space="".zfill(nb).replace("0"," ")
    #Nous nous occupons que des categories suivantes: CROCHET, UNION, STRING, CHIFFRE, MOT
    if "%NAME" in tree:
        name=tree["%NAME"]
        value=tree["%VALUE"]
        #print space,name,value
        if name in ["string","point","interval","mot","digits","symbole"]:
            element={name:value}
            struct.append(element)
        elif name=="coupe":
            element={"$":value}
            struct.append(element)
        elif name=="negation":
            #Constraint on applying the ANY operator on the next element
            sub=[]
            explore(tree,nb+1,sub)
            struct.append(["~",sub])
        elif name=="kleene":
            sub=[]
            explore(tree,nb+1,sub)
            if sub==[]:
                struct[-1]=[value,struct[-1]]
            else:
                struct[-1]=[value[0],struct[-1],sub]
        elif name=="liste":
            sub=[]
            explore(tree,nb+1,sub)
            struct.append(sub)
        elif name=="contexte":
            sub=[]
            explore(tree,nb+1,sub)
            sub.insert(0,"{}")
            struct.append(sub)
        elif name=="optionel":
            sub=[]
            explore(tree,nb+1,sub)
            sub.insert(0,"()")
            struct.append(sub)
        elif name=="union":
            sub=[]
            explore(tree,nb+1,sub)
            insertion(struct,sub,"|")
        elif name=="exclusif":
            sub=[]
            explore(tree,nb+1,sub)
            insertion(struct,sub,"^")
        elif name=="sequence":
            sub=[]
            explore(tree,nb+1,sub)
            insertion(struct,sub,"&")
        else:
            explore(tree,nb+1,struct)
    else:
        explore(tree,nb+1,struct)
    

stringrule="""
#include <stdio.h>
#include <string.h>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>


using namespace std;
typedef enum {PARTIAL,FULL} x_parsing_mode;

class x_node {
public:

	string token;
	string value;
	char init;
	vector<x_node*> nodes;
	long start;
	long end;

	x_node() {
		init=0;
		start=0;
		end=0;
	}

	~x_node() {
		for (int i=0;i<nodes.size();i++)
			delete nodes[i];
	}

	void set(string& t,string& v) {
		token=t;
		value=v;
		init=1;
	}

	
	char append(x_node* n) {
		if (n!=NULL && (n->init==1 || n->nodes.size()>0)) {
			if (n->init==1)
				nodes.push_back(n);
			else {
				for (int i=0;i<n->nodes.size();i++)
					nodes.push_back(n->nodes[i]);
				n->nodes.clear();
				delete n;
			}
			return 1;
		}
		return 0;
	}

	void pop(int i) {
            while (i>0) {
                x_node* n=nodes[nodes.size()-1];
                nodes.pop_back();
                delete n;
                i--;
            }
	};
    };

class x_couple {
    public:
    long pos;
    int intoken;
    x_node* tree;
    string values;

    x_couple(long p,int itok,x_node* s,string& lret) {
       pos=p;
       intoken=itok;
       tree=s;
       values=lret;
    }
    void set(string& lret) {
      lret=values;
    }
};
     

class x_reading {
public:

        vector<string> stack;
        vector<long> stackln;
        vector<char> stacktype;

	long pos;
	string currentbuffer;
	string name;

	char table[255];

	virtual void Initable() {
		static char tabaccent[]={'�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','_','#',0};
		strcpy(table,tabaccent);
		int i=strlen(table);
		char c;
		for (c=65;c<=90;c++)
			table[i++]=c;
		for (c=97;c<=122;c++)
			table[i++]=c;
		for (c=48;c<=57;c++)
			table[i++]=c;
		table[i]=0;
	}

	virtual void seek(long p) {
		pos=p;
	}

	virtual char eof() {return 0;}

	long tell(void) {
		return pos;
	}

	virtual unsigned char peek() {
		return 0;
	}

        virtual char get() {
            return -1;
        }
        
	virtual string read(int sz) { return "";}
	virtual void close() {}
	virtual char error() {
		if (pos==-1)
			return 1;
		return 0;
	}
	virtual void reset(long p) {}

         virtual bool digitonly(string& thestr) {
            int i=0;
            static char tabs[]={'0','1','2','3','4','5','6','7','8','9','.',0};
            if (thestr[i]=='-')
		i++;
  	    for (;i<thestr.size();i++) 
		if (strchr(tabs,thestr[i])==NULL)
			return false;		
	    return true;
        }

	virtual void loadtoken(bool keepspace=false) {
		char s,c;
		Initable();
		string ch;
		string token;
		stack.clear();
		stackln.clear();
		stacktype.clear();

		long pos;
		long line=0;
		char lastc=0;
		s=get();
		pos=tell();
		while (!eof()) {
			if (s=='/' && token=="") {
				s=get();
				if (s=='/') {
					//this is a comment
					s=get();
					while (s!=10 && s!=13 && !eof())
						s=get();
					token="";
                                        s=get();
                                        pos=tell();					
					continue;
				}
				seek(pos);
				s='/';	
			}

			c=s;
			if (c=='"') {
				token+=c;
				c=get();				
				while (c!='"' && !eof()) {
					if (c=='\\\\') {
						token+=c;
						c=get();
					}						
					token+=c;
					c=get();
				}
				token+='"';
				stack.push_back(token);
				stackln.push_back(line);
				stacktype.push_back(1);
				token="";
				s=get();
				pos=tell();				
				continue;
			}
			if (c=='\\'') {
				token+=c;
				c=get();
				while (c!='\\'' && !eof()) {
					token+=c;
					c=get();
				}
				token+="'";
				stack.push_back(token);				
				stackln.push_back(line);
				stacktype.push_back(2);
				token="";
				s=get();
				pos=tell();				
				continue;
			}
			if (strchr(table,c))
				token+=c;
			else {
				if (token!="") {
					if (digitonly(token)) {
						if (stack.size()>0) {
							ch=stack[stack.size()-1];
							if (ch==".") {
								stack[stack.size()-1]+=token;
								ch=stack[stack.size()-2];
								if (digitonly(ch)) {
									stack[stack.size()-2]+=stack[stack.size()-1];
									stacktype[stack.size()-2]=3;
									stack.pop_back();
									stackln.pop_back();
									stacktype.pop_back();
								}
							}
							else {
								stack.push_back(token);
								stacktype.push_back(3);
								stackln.push_back(line);
							}							
						}
						else {
							stack.push_back(token);
							stacktype.push_back(3);
							stackln.push_back(line);
						}
					}
					else {
						stack.push_back(token);
						stacktype.push_back(4);
						stackln.push_back(line);
					}
					token="";
				}
				if (c>32 || keepspace) {
					ch=c;
					stack.push_back(ch);
					stackln.push_back(line);
					stacktype.push_back(0);
				}
				if (c==10 && lastc==13)
					lastc=0;
				else
					if (c==10 || c==13) {
						lastc=c;
						line++;
					}
			}
			s=get();
			pos=tell();
		}
		if (token!="") {
			if (digitonly(token)) {
				if (stack.size()>0) {
					ch=stack[stack.size()-1];
					if (ch==".") {
						stack[stack.size()-1]+=token;
						ch=stack[stack.size()-2];
						if (digitonly(ch)) {
							stack[stack.size()-2]+=stack[stack.size()-1];
							stacktype[stack.size()-2]=3;
							stack.pop_back();
							stackln.pop_back();
							stacktype.pop_back();
						}
					}
					else {
						stack.push_back(token);
						stacktype.push_back(3);
						stackln.push_back(line);
					}							
				}
				else {
					stack.push_back(token);
					stacktype.push_back(3);
					stackln.push_back(line);
				}
			}
			else {
				stack.push_back(token);
				stacktype.push_back(4);
				stackln.push_back(line);
			}
		}

	}
};    

class x_readstring: public x_reading {
public:
	string parcours;
	long length;


	x_readstring(string s) {
		name="%%STRING"; 
		pos=0;
		parcours=s;
		length=s.size();
	}

	unsigned char peek() {
		if (pos<length)
			return parcours[pos+1];
		else
			return 0;
	}

	char eof() {
		if (pos>length)
			return 1;
		return 0;
	}

        char get() {
            if (pos==length) {
                pos++;
                return -1;
            }
            return parcours[pos++];
        }

	string read(int sz) {
		long borne=sz;
		if (borne>=length)
			borne=length;
		if (eof())
			return "";
		currentbuffer=parcours.substr(pos,borne);
		pos+=borne;
		return currentbuffer;
	}
	
	void reset(long p) {
		pos=p;
	}
};

class x_readfile: public x_reading {
public:
	ifstream* fichier;
	char* buf;
	int taille;
	long size;

	x_readfile(char* n) {
		buf=new char[258];
		taille=256;
		name=n;
		fichier=new ifstream(n,ios::in|ios::binary);
		if (fichier->fail() )
			pos=-1;
		else
			pos=0;
		size=-1;
	}

	x_readfile(ifstream* fic,char* n,long p) {
		buf=new char[258];
		taille=256;
		name=n;
		fichier=fic;
		pos=p;
		size=-1;
	}

	char error() {
		if (fichier==NULL || pos==-1)
			return 1;
		return 0;
	}

	void seek(long p) {
		if (fichier->eof())
			size=pos;
		pos=p;
		fichier->seekg(p);
		fichier->clear();
	}

	unsigned char peek() {
		return (unsigned char)fichier->peek();
	}

	char eof() {
		if (fichier==NULL || (size!=-1 && pos>=size))
			return 1;
		return fichier->eof();
	}

        char get() {
            if (fichier->eof())
                return -1;
            pos++;
            return fichier->get();
        }

	string read(int sz) {
		if (taille<sz) {
			delete buf;
			buf=new char[sz+2];
			taille=sz;
		}
		buf[sz]=0;
		if (fichier->eof())
			return "";
		fichier->read(buf,sz);
		long lz=fichier->gcount();
		buf[lz]=0;    
		pos+=sz;
		currentbuffer= buf;
		return buf;
	}

	void close() {
		fichier->close();
	}        

	void reset(long p) {
                if (fichier==NULL)
                    return;
		pos=p;
		fichier->clear();
		fichier->seekg(pos);
		size=-1;
	}
};


"""

otherfunctions="""
class %%%%% {
public:


int currentpos;
int intoken;
int lineerror;
string labelerror;
int errornumber;
char gFail;
int baseline;
x_reading* fx;

%%%%%(int l=0) {
    baseline=l;
    fx=NULL;
    intoken=0;
    currentpos=0;
}

void initialize(x_reading* xr) {
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
    lineerror=fx->stackln[pos]+baseline;
    labelerror=lab;
    errornumber=error;
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
	char x=fx->stack[currentpos][intoken];
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
	char x=fx->stack[currentpos][intoken];
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
	incrementintoken();
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
x_node* x_parsing(x_reading* xr,x_parsing_mode mode);

"""


xteststringtoken="""
char &&&&::x_test_string(string& lret,int sz,string sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	if (fx->stack[currentpos]==sub) {
		incrementpos();
		lret+=sub;
		return 1;
	}
	return 0;
}
"""



parsestr="""

string *****::x_errormsg(int i) {
	return errorlabels[i];
}


x_node* *****::x_parsing(x_reading* xr,x_parsing_mode mode) {
fx=xr;
	string lret;
	x_node* tree=new x_node;
	gFail=0;
	lineerror=-1;
	labelerror="";
	errornumber=-1;
	currentpos=0;
	intoken=0;
	char res=%%%(lret,&tree);

	if (currentpos!=fx->stack.size() && mode==FULL) {
		if (errornumber!=-1) 
			cerr<<endl<<"Error line:"<<lineerror<<"="<<errorlabels[errornumber]<<endl;
		else
			cerr<<endl<<"Error line:"<<lineerror<<endl;
		if (tree!=NULL)
			delete tree;
		return NULL;
	}

	if (res==1)
		return tree;

	delete tree;
	return NULL;
}

"""


def displayanalysis(tree,nb):
    space="".zfill(nb).replace("0"," ")
    if "%NAME" in tree:
        name=tree["%NAME"]
        value=tree["%VALUE"]
        print space,name,value
    if "%NODES" in tree:
        lst=tree["%NODES"]
        for l in lst:
            displayanalysis(l,nb+1)

def returnbasename(name):
    pssubfunc=name.find("_")
    if pssubfunc==-1:
        basename=name
    else:
        basename=name[:pssubfunc]
    return basename    

def createrule(name,s,nb,trace,basedeclaration):
    global classname
    basename=returnbasename(name)
    
    newname=name
    if nb!=None:
        newname+="_"+str(nb[0])
        nb[0]+=1
    declr="char "+classname+"::m_"+newname+"(string& lreturn,x_node** tree)"
    s.append(declr+" {")
    trace.append(declr+";")
    s.append(" if (gFail==1) return(0);");
    s.append(' static const char label[]="'+basename+'";')
    s.append(" string lret;")
    s.append(" long pos=currentpos;\n\tint itok=intoken;")
    s.append(" x_node* subtree=NULL;")
    s.append(" int addsubtree=0;")
    s.append(" bool exitonfail=false;")    
    return newname

def concatenaterule(elements,nb,values,nbtab):
    rule=[]
    concat=[]
    for r in elements:
        if tl(r[0]):
            subrule=analyserule(r)
            if subrule!=[]:
                rule.append(subrule)
        else:
            if r[0][0:2]=="X_":
                concat.append(r[0][2:])
            else:
                if concat!=[]:
                    if len(concat)==1:
                        nr='x_test_char(lret,'+concat[0]+")"
                        ngarde=nr
                    else:
                        vari='!static char tab'+str(nbtab[0])+"[]="+'{'+",".join(concat)+',0}'
                        rule.append(vari);
                        var="tab"+str(nbtab[0])
                        nbtab[0]+=1;
                        nr='x_test_in(lret,'+var+")"
                        ngarde='x_test_in(lret,'+var+")"
                    values.append(ngarde)
                    rule.append(nr)
                if r[0][0:2]=="x_":
                    values.append(r[0])
                concat=[]
                rule.append(r)
    if concat!=[]:
        if len(concat)==1:
            nr='x_test_char(lret,'+concat[0]+")"
            ngarde=nr
        else:
            vari='!static char tab'+str(nbtab[0])+"[]="+'{'+",".join(concat)+',0}'
            rule.append(vari);
            var="tab"+str(nbtab[0])
            nbtab[0]+=1;
            nr='x_test_in(lret,'+var+")"
            ngarde='x_test_in(lret,'+var+")"
        values.append(ngarde)
        rule.append(nr)
    return rule    


def bodysequence(rule,subdef):
    if tl(rule):
        for r in rule:
            bodysequence(r,subdef)
    else:
        if rule[0]=="#":
            subdef.append(rule[1:])
            return
        subdef.append("//BODYSEQUENCE")
        subdef.append(" subtree=NULL;")
        subdef.append(" if ("+rule+") ")
        subdef.append("    x_init_tree(tree,subtree,addsubtree);")
        subdef.append(" else {")
        subdef.append("    x_pop_node(tree,addsubtree);")
        subdef.append("    currentpos=pos;\n\t\tintoken=itok;")
        subdef.append("    setfail(exitonfail);")
        subdef.append("    return(0);")
        subdef.append(" }")

def bodyoptional(rule,subdef):
    if tl(rule):
        for r in rule:
            bodyoptional(r,subdef)
    else:
        subdef.append("//BODYOPTIONAL")
        if rule[0]=="#":
            subdef.append(rule[1:])
            return
        subdef.append(" subtree=NULL;")
        subdef.append(" if ("+rule+")")
        subdef.append("    x_init_tree(tree,subtree,addsubtree);")
        subdef.append(" else")
        subdef.append("   lret=\"\";")


def bodycontextual(rule,subdef):
    if tl(rule):
        for r in rule:
            bodycontextual(r,subdef)
    else:
        subdef.append("//BODYCONTEXTUAL")
        subdef.append(" long poscontext=currentpos;\n\tint itokcontext=intoken;")
        ru=rule
        adddelete=False
        if ",&subtree)" in ru:
            subdef.append(" x_node* subtreebis=NULL;")
            ru=ru.replace(",&subtree)",",&subtreebis)")
            adddelete=True
        subdef.append(" string lretbis;")
        ru=ru.replace("(lret,","(lretbis,")
        subdef.append(" if ("+ru+") {")
        if adddelete:
            subdef.append("    if (subtreebis!=NULL)")
            subdef.append("        delete subtreebis;")
        subdef.append("    currentpos=poscontext;\n\t\tintoken=itokcontext;")
        subdef.append(" }")        
        subdef.append(" else {")
        subdef.append("    currentpos=pos;\n\t\tintoken=itok;")
        subdef.append("    setfail(exitonfail);")
        subdef.append("    return(0);")
        subdef.append(" }")
        
def bodynegation(rule,subdef):
    if tl(rule):
        for r in rule:
            bodynegation(r,subdef)
    else:
        subdef.append("//BODYNEGATION")
        if rule[0]=="#":
            subdef.append(rule[1:])
            return
        subdef.append(" subtree=new x_node;")
        subdef.append(" if ("+rule+"==0) {")
        subdef.append("   x_test_any(lret);")
        subdef.append("   x_init_tree(tree,subtree,addsubtree);")
        subdef.append(" }")
        subdef.append(" else {")
        subdef.append("   x_pop_node(tree,addsubtree);")
        subdef.append("   currentpos=pos;\n\t\tintoken=itok;")
        subdef.append("   setfail(exitonfail);")
        subdef.append("   return(0);")
        subdef.append(" }")
        

def bodyor_old(rule,s,init,subdef,first):
    if tl(rule):
        for r in rule:
            bodyor_old(r,s,init,subdef,False)
    else:
        if rule[0]=="!":
            init.append(rule[1:])
        elif s[0]=="":
            s[0]+=" if ("+rule            
        else:
            s[0]+=" || "+rule
    if first:
        s[0]+=")"
        subdef.append(" subtree=NULL;")
        subdef.append(s[0])
        subdef.append("  x_init_tree(tree,subtree,addsubtree);")
        subdef.append(" else {")
        subdef.append("   x_pop_node(tree,addsubtree);")
        subdef.append("   currentpos=pos;\n\t\tintoken=itok;")
        subdef.append("   setfail(exitonfail);")
        subdef.append("   return 0;")
        subdef.append(" }")

                    
def bodyorrec(rule,subdef):
    if tl(rule):
        for r in rule:
            bodyorrec(r,subdef)
    else:
        if rule[0]!="!":
            if rule[0]=="#":
                subdef.append(rule[1:])
                return
            subdef.append(" lret=\"\";")
            subdef.append(" subtree=NULL;")
            subdef.append(" currentpos=pos;\n\t\tintoken=itok;")
            subdef.append(" if ("+rule+") {")
            subdef.append("   results.push_back(new x_couple(currentpos,intoken,subtree,lret));")
            subdef.append("    if (max<currentpos) {")
            subdef.append("        max=currentpos;")
            subdef.append("    }")                
            subdef.append(" }")
            subdef.append(" gFail=0;")

        
def bodyevaluate(rule):    
    for r in rule:
        if tl(r):
            rs=bodyevaluate(r)
            if rs==False:
                return rs
        elif r[0]!="!":
            if r[0:2]!='x_':
                return False
    return True
        
def bodyor(rule,subdef,init,firstloop):
    subdef.append("//BODYOR")
    res=bodyevaluate(rule)
    if firstloop or res:
        s=[""]
        bodyor_old(rule,s,init,subdef,True)
    else:
        subdef.append(" long max=0;")
        subdef.append(" vector<x_couple*> results;")
        bodyorrec(rule,subdef)
        subdef.append(" if (results.size()==0) {")
        subdef.append("     currentpos=pos;\n\t\tintoken=itok;")
        subdef.append("     setfail(exitonfail);")
        subdef.append("     return 0;")
        subdef.append("  }")
        subdef.append(" char add=0;")
        subdef.append(" for (int i=0;i<results.size();i++) {")
        subdef.append("    if (add==0 && max==results[i]->pos) {")
        subdef.append("        add=1;")
        subdef.append("        currentpos=results[i]->pos;\n\t\tintoken=results[i]->intoken;")
        subdef.append("        results[i]->set(lret);")
        subdef.append("        x_init_tree(tree,results[i]->tree,addsubtree);")
        subdef.append("     }")
        subdef.append("     else ")
        subdef.append("       delete results[i]->tree;")
        subdef.append("     delete results[i];")
        subdef.append("  }")

def bodywhile(rule,subdef,operator):
    if tl(rule):
        bodywhile(rule[0],subdef,operator)
        return

    if rule[0]=="#":
        subdef.append(rule[1:])
        return
    subdef.append("//BODYWHILE")
    if operator=="+":        
        subdef.append(" subtree=NULL;")
        subdef.append(" if ("+rule+")")
        subdef.append("     x_init_tree(tree,subtree,addsubtree);")
        subdef.append(" else {")
        subdef.append("    x_pop_node(tree,addsubtree);")
        subdef.append("    currentpos=pos;\n\t\tintoken=itok;")
        subdef.append("    setfail(exitonfail);")
        subdef.append("    return(0);")
        subdef.append(" }")   
        
    subdef.append(" while (currentpos<fx->stack.size()) {")
    subdef.append("   subtree=NULL;")
    subdef.append("   if ("+rule+")")
    subdef.append("         x_init_tree(tree,subtree,addsubtree);")
    subdef.append("   else")
    subdef.append("      break;")
    subdef.append(" }")

def bodyconstraint(const,subdef,operator):
    rule=const[0][0]
    constraint=const[1][0]
    subdef.append("//CONSTRAINT")
    if rule[0]=="#":
        subdef.append(rule[1:])
        return
    if operator=="+":
        subdef.append(" subtree=NULL;")
        subdef.append(" if ("+constraint+") {")
        subdef.append("     x_pop_node(tree,addsubtree);")
        subdef.append("     currentpos=pos;\n\t\tintoken=itok;")
        subdef.append("     setfail(exitonfail);")
        subdef.append("     return(0);")
        subdef.append(" }")
        subdef.append(" subtree=NULL;")
        subdef.append(" if ("+rule+")")
        subdef.append("       x_init_tree(tree,subtree,addsubtree);")
        subdef.append(" else {")
        subdef.append("    x_pop_node(tree,addsubtree);")
        subdef.append("    currentpos=pos;\n\t\tintoken=itok;")
        subdef.append("    setfail(exitonfail);")
        subdef.append("    return(0);")
        subdef.append(" }")        
    subdef.append(" int foundsolution=0;")
    subdef.append(" while (currentpos<fx->stack.size()) {")
    subdef.append("   subtree=NULL;")
    subdef.append("   if ("+constraint+") {")
    subdef.append("      foundsolution=1;")
    subdef.append("      x_init_tree(tree,subtree,addsubtree);")
    subdef.append("      break;")
    subdef.append("   }")
    subdef.append("   subtree=NULL;")
    subdef.append("   if ("+rule+")")
    subdef.append("         x_init_tree(tree,subtree,addsubtree);")
    subdef.append("   else")
    subdef.append("      break;")
    subdef.append("   }")                  
    subdef.append(" if (foundsolution==0) {")
    subdef.append("    x_pop_node(tree,addsubtree);")
    subdef.append("    currentpos=pos;\n\t\tintoken=itok;")
    subdef.append("    setfail(exitonfail);")
    subdef.append("    return(0);")
    subdef.append(" }")
                      
def replacerule(elements,rule):
    for r in elements:
        if tl(r):
            rl=[]
            replacerule(r,rl)
            rule.append(rl)
        else:
            if r[0:2]=="X_":
                nr='x_test_char(lret,'+r[2:]+')'
                rule.append(nr)
            else:
                rule.append(r)
        
lasterror=None
def generate(name,ruletree,result,functions,nb,current,initialisations,firstloop,trace,nbtab,dicorules):
    global lasterror
    global donotconcatenate
    if tl(ruletree):
        operator=ruletree[0]
        if operator in ["&","|","()","{}","+","*","~","^"]:
            subdef=[]
            newname=createrule(name,subdef,nb,trace,firstloop)
            new_rgx="m_"+newname+"(lret,&subtree)"
            result.append(new_rgx)
            baselst=[]
            pointrule=None
            aff=False
            if lasterror!=None:
                aff=True
                result.append("# exitonfail=true;")
                result.append("# initfail(label,currentpos,"+lasterror+");")
                lasterror=None
            
            #Concatenation of element
            if operator=="@":
                #The first element is a test on POINT
                [therule]=ruletree[1:]
                pointrule=therule[0]
                nexts=therule[1:]
            else:
                nexts=ruletree[1:]
            for r in nexts:
                newdef=[]
                generate(newname,r,newdef,functions,nb,operator,initialisations,firstloop,trace,nbtab,dicorules)
                baselst.append(newdef)
                
            lst=[]
            if operator=="~":
                replacerule(baselst,lst)
                bodynegation(lst,subdef)
            if operator=="&":
                replacerule(baselst,lst)
                bodysequence(lst,subdef)
            elif operator=="|":
                values=[]
                lst=concatenaterule(baselst,nb,values,nbtab)                    
                ftest=[True]
                bodyor(lst,subdef,initialisations,firstloop)       
            elif operator=="^":
                values=[]
                lst=concatenaterule(baselst,nb,values,nbtab)
                ftest=[True]
                bodyor(lst,subdef,initialisations,True)       
            elif operator=="()":
                replacerule(baselst,lst)
                bodyoptional(lst,subdef)
            elif operator=="{}":
                replacerule(baselst,lst)
                bodycontextual(lst,subdef)
            elif operator in ["+","*"]:
                replacerule(baselst,lst)
                if len(lst)==1:
                    bodywhile(lst,subdef,operator)
                else:
                    bodyconstraint(lst,subdef,operator)
            xname=returnbasename(newname)
            if xname not in donotconcatenate:
                subdef.append(" lreturn+=lret;")
#            subdef.append(" lreturn+=lret;")
            subdef.append(" return(1);")
            subdef.append(" }")            
            keyfunc="".join(subdef[9:])
            lasterror=None
            #We remove the functions that share the same body
            #We use the name of the first one to match
            try:
                myname=dicorules[keyfunc]
                if aff:
                    result[-3]=result[-3].replace("_"+newname,"_"+myname)
                else:
                    result[-1]=result[-1].replace("_"+newname,"_"+myname)
                discover="m_"+newname+"("
                for t in trace:
                    if discover in t:
                        trace.remove(t)
                        break
            except:                
                dicorules[keyfunc]=newname
                functions.append(subdef)
        else:
            for rule in ruletree:
                generate(name,rule,result,functions,nb,current,initialisations,firstloop,trace,nbtab,dicorules)               
    else:
        if td(ruletree):
            if "$" in ruletree:
                lasterror=ruletree["$"]
                return

            if "symbole" in ruletree:
                sym=ruletree["symbole"][1:]
                if sym=="\\":
                    sym+="\\"
                if sym=='"':
                    result.append("X_34")
                elif sym=="'":
                    result.append("X_39")
                else:
                   result.append("X_'"+sym+"'")                
            elif "string" in ruletree:
                lx=len(ruletree["string"][1:])
                sb=ruletree["string"][1:]
                if lx==1:
                    if sb=='"':
                        result.append("X_34")
                    elif sb=="'":
                        result.append("X_39")
                    else:
                        result.append("X_'"+sb+"'")
                else:
                    result.append('x_test_string(lret,'+str(lx)+',"'+sb+'")')
            elif "point" in ruletree:
                result.append("x_test_any(lret)")
            elif "digits" in ruletree:
                result.append("X_"+ruletree["digits"])
            elif "mot" in ruletree:
                result.append("m_"+ruletree["mot"]+"(lret,&subtree)")
            elif "interval" in ruletree:
                s=ruletree["interval"].split("-")
                result.append("x_test_ord(lret,'"+s[0]+"','"+s[1]+"')")               
            if lasterror!=None:
                result.append("# exitonfail=true;")
                result.append("# initfail(label,currentpos,"+lasterror+");")
                lasterror=None
        else:
            print
            print "Erreur",ruletree
            print
                
                              
def simplify(r,bodies):
    if tl(r):
        if r[0] in ["|","()","{}","&","+","*","^"]:
            b=[r[0]]
            for i in r[1:]:
                simplify(i,b)
            bodies.append(b)
        else:
            if tl(r) and len(r)==1:
                bodies.append(r[0])
            else:
                bodies.append(r)
    else:
        bodies.append(r)
            

def cleanrules(rules,simple):
    for r in rules:
        s=[r[0],r[1]]
        simplify(r[2],s)
        simple.append(s)

tokenrule="""
char &&&&::m_%%%%(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char label[]="%%%%";
	if (fx->stacktype[currentpos]==****) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}
"""

tokenrulenumber="""
char &&&&::m_%%%%(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char label[]="%%%%";
	if (fx->stacktype[currentpos]==****) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	if (fx->stack[currentpos]=="-" && fx->stacktype[currentpos+1]==****) {
		string s="-";
		s+=fx->stack[currentpos+1];
		x_init_node(lreturn,s,tree,label,currentpos,true);
		currentpos+=2;
		return(1);
	}
	return(0);
}
"""


            
def parsenotrule(name,classname,token,tokrule):
    sy=tokrule.replace("&&&&",classname)
    sy=sy.replace("****",token)
    sy=sy.replace("%%%%",name)
    return sy
        
def compiling(filename):
    global stringrule
    global classname
    global donotconcatenate
    f=open(filename,"r")
    erreur=[]
    rules=[]
    comments=[]
    kept=[]
    removed=[]
    errormessages=[]
    callpersonal=[]

    #we use the following rules for pre-tokenization: <,>,^,= --->  string in double quote, string in simple quote, token, number
    parsenot=[]
    usetokens=[]
    usenumbers=[]
    usesimplestring=[]
    usedoublestring=[]

    for e in f:
        l=e.strip()
        if l=="":
            continue
        #we use tokens
        if l[0]=='^':
            l=l[1:]
            ps=l.find(":")
            ct=l[:ps].strip()
            usetokens.append(ct)
            parsenot.append(ct)
        if l[0]=='%':
            l=l[1:]
            ps=l.find(":")
            ct=l[:ps].strip()
            donotconcatenate.append(ct)
        #we detect numbers
        if l[0]=='=':
            l=l[1:]
            ps=l.find(":")
            ct=l[:ps].strip()
            usenumbers.append(ct)
            parsenot.append(ct)
        #we detect string in double quotes
        if l[0]=='<':
            l=l[1:]
            ps=l.find(":")
            ct=l[:ps].strip()
            usedoublestring.append(ct)
            parsenot.append(ct)
        #we detect string in simple quotes
        if l[0]=='>':
            l=l[1:]
            ps=l.find(":")
            ct=l[:ps].strip()
            usesimplestring.append(ct)
            parsenot.append(ct)
        if l[0]=='!':
            l=l[1:]
            ps=l.find(":")
            ct=l[:ps].strip()
            removed.append(ct)
            
        if l[0]=='#':
            comments.append(l)
        elif l[0]=="+":
            kpt=l[1:].strip().split(",")
            for k in kpt:
                kept.append(k.strip())
        elif l[0]=="%":
            kpt=l[1:].strip().split(",")
            for k in kpt:
                callpersonal.append(k.strip())
        elif l[0]=="*":
            kpt=l[1:].strip().split("=")
            errormessages.append([kpt[0],kpt[1]])
        else:
            tree={}
            if not applyrule(l,tree):
                erreur.append(l)
            else:
                struct=[{"%":l}]
                treeanalysis(tree,0,struct)
                #displayanalysis(tree,0)
                rules.append(struct)

    for x in erreur:
        print "Error:",x
    nb=[0]
    #First the .h file
    if "/" in filename:
        xf=filename.split("/")
        sep="/"
    else:
        xf=filename.split("\\")
        sep=chr(92)

    nameHfile="x_node.h"

    fileh=sep.join(xf[:-1])+sep+nameHfile
    foutput=open(fileh,"w")
    for c in comments:
        foutput.write("//"+c[1:]+"\n")
    foutput.write("\n\n")
    foutput.write("#ifndef x_node_h\n")
    foutput.write("#define x_node_h\n")
    
    foutput.write(stringrule)
    foutput.write("\n\n#endif\n")
    foutput.close()



    #then the .cxx
    if ".txt" in filename:
        filenewh=filename.replace(".txt","n.h")        
        filenewcxx=filename.replace(".txt","n.cxx")        
    else:
        filenewh=filename+"n.h"
        filenewcxx=filename+"n.cxx"

    ips=filename.rfind("\\")
    if ips==-1:
        ips=0
    pip=filename.rfind(".")
    if pip==-1:
        pip=len(filename)

    classname="bnf_"+filename[ips+1:pip]
    print classname
    foutput=open(filenewh,"w")
    
    
    bodies=[]
    cleanrules(rules,bodies)

    initialisations=[]
    trace=[]
    nbtab=[0]
    dicorules={}

    ###H Section: Class definition###################################
    foutput.write("#ifndef "+classname+"_h\n")
    foutput.write("#define "+classname+"_h\n")
    foutput.write('#include "'+nameHfile+'"\n\n')

    
    for r in bodies:
        #The first element is the rule name
        functions=[]
        s=[]
        name=r[1]["mot"]
        if name in parsenot:
            continue
        createrule(name,s,None,trace,True)
        generate(name,r[2:],s,functions,nb,None,initialisations,True,trace,nbtab,dicorules)
    nb=[0]

    sother=otherfunctions.replace("%%%%%",classname)
    foutput.write(sother+"\n\n")

    for t in trace:
        st=t.replace(classname+"::","")
        foutput.write(st+"\n")

    for u in parsenot:
        sinc="char m_"+u+"(string& lreturn,x_node** tree);"
        foutput.write(sinc+"\n")

    foutput.write("};\n#endif\n")
    foutput.close()
    
    ##### CXX Section############################################
    foutput=open(filenewcxx,"w")

    ips=filenewh.rfind("\\")
    if ips==-1:
        ips=0
    newh=filenewh[ips+1:]
    foutput.write('#include "'+newh+'"\n\n\n')
    foutput.write("static char* errorlabels[]={")
    if len(errormessages)==0:
        foutput.write('""');
        
    cpterr=0
    first=True
    for errmsg in errormessages:
        idx=int(errmsg[0])
        if not first:
            foutput.write(",")
        first=False
        if idx>cpterr:
            while (cpterr<idx):
                foutput.write('"",');
                cpterr+=1;
        foutput.write('"'+errmsg[1].strip()+'"');
        cpterr+=1
    foutput.write("};\n\n")

    for i in initialisations:
        foutput.write(i+";\n")

    initialisations=[]
    trace=[]
    nbtab=[0]
    dicorules={}
    for r in bodies:
        #The first element is the rule name
        functions=[]
        s=[]
        name=r[1]["mot"]
        if name in parsenot:
            continue
        #We first create the base function
        createrule(name,s,None,trace,False)
        #then we create the derived functions
        generate(name,r[2:],s,functions,nb,None,initialisations,False,trace,nbtab,dicorules)        
        xxs=s[-1]
        s=s[:-1]
        xbase=[]
        replacerule([xxs],xbase)
        xs=xbase[0]
        #s.append(' x_decompte_essai("'+name+'");');
        s.append(" if ("+xs+")")
        s.append("       x_init_tree(tree,subtree,addsubtree);")
        s.append(" else {")
        s.append("    x_pop_node(tree,addsubtree);")
        s.append("    currentpos=pos;\n\t\tintoken=itok;")
        s.append("    setfail(exitonfail);")
        s.append("    return(0);")
        s.append(" }")
        if name not in removed:
            if name in donotconcatenate:
                s.append(" x_init_node(lreturn,lret,tree,label,pos,false);")
            else:
                s.append(" x_init_node(lreturn,lret,tree,label,pos,true);")            
        else:
            s.append(" x_pop_node(tree,addsubtree);")
        #s.append(' x_decompte("'+name+'");');
        s.append(" return(1);")
        s.append("}")
        functions.append(s)
        for fc in functions:
            for fct in fc:
                foutput.write(fct+"\n")
            foutput.write("\n\n")

    nvx=parsestr
    if comments!=[]:
        nvx=parsestr.replace("&&&&",comments[0][1:])
        sx=nvx

    if not kept==[]:
        sx=nvx.replace("%%%","m_"+kept[0])

    sy=sx.replace("*****",classname)
    foutput.write(sy)
    sy=xteststringtoken.replace("&&&&",classname)
    foutput.write(sy)
    for u in usetokens:
        sy=parsenotrule(u,classname,"4",tokenrule)
        foutput.write(sy)
    for u in usenumbers:
        sy=parsenotrule(u,classname,"3",tokenrulenumber)
        foutput.write(sy)
    for u in usesimplestring:
        sy=parsenotrule(u,classname,"2",tokenrule)
        foutput.write(sy)
    for u in usedoublestring:
        sy=parsenotrule(u,classname,"1",tokenrule)
        foutput.write(sy)
        
    foutput.close()
    f.close()

try:
    chemin=sys.argv[1]
except:
    chemin=r"W:\users\Roux\xip\parSem\xip\KIF\kif"
compiling(chemin)
