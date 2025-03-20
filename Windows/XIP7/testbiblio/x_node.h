//x_parse_file("C:\\ANL\\Mekalinga\\ANL\\Mekalinga\\simple\\test\\generation.xip",FULL);
//Formalisme de test: Nous allons fournir une grammaire pour analyser des chaines de type: X = X*,X+,Z,(X[i:o,j:x]).
//x_parse_string("GV(#1,#2){Noun$#1(#2),Verb$#1(#3)}|",m_generation,False)
//x_parse_string("$#1[test](#1,#2) &",m_elementif,False)
//Elements de dependance
//On peut avoir les cas suivants:
// dependency: $1 / $#1 / $1(#1,#2) etc...
// tests:
//    EGAUX_ONTOLOGIE         #1[@ontologie] :: #2[@ontologie]
//    COMPATIBLES_ONTOLOGIE   #1[@ontologie] : #2[@ontologie]
//    AFFECTATION_ONTOLOGIE   #1[@ontologie] = {#2 | #3..}
//    COMPATIBLES_LECTURE     #1<tr>:#2<tr>
//    AFFECTATION_LECTURE     #1<tr>={...}
//    EGAUX_LECTURE
//    COMPATIBLES             #1[tr]:#2[tr]
//    EGAUX                   #1[tr]::#2[tr]
//    EGALITE                 #1:#2
//    INFERIEUR               #1<#2
//    SUPERIEUR               #1>#2
//    INF_TRAITS              #1[tr]< ch | #2 | #2[tr]
//    SUP_TRAITS              #1[tr]> ch | #2 | #2[tr]
//    SUP_EGAL_TRAITS         #1[tr]<= ch | #2 | #2[tr]
//    INF_EGAL_TRAITS         #1[tr]>= ch | #2 | #2[tr]
//    AFFECTATION             #1[tr]={...}
//    TEST                    #1[tr]



#include <stdio.h>
#include <string.h>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

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
    x_node* tree;
    vector<string> values;

    x_couple(long p,x_node* s,vector<string>& lret) {
       pos=p;
       tree=s;
       for (int i=0;i<lret.size();i++)
           values.push_back(lret[i]);
    }
    void set(vector<string>& lret) {
      lret.clear();
      for (int i=0;i<values.size();i++)
          lret.push_back(values[i]);
    }
};
     

class x_reading {
  public:


    long pos;
    string currentbuffer;
    long lastposerror;
    string labelerror;

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
    
    virtual string read(int sz) { return "";}
    virtual void close() {}
    char error() {
      if (pos==-1)
          return 1;
      return 0;
    }
};    

class x_readstring: public x_reading {
    public:
      string parcours;
      long length;

    x_readstring(string s) {
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
		if (pos>=length)
			return 1;
		return 0;
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
};

class x_readfile: public x_reading {
    public:
    string name;
    ifstream* fichier;
    char* buf;
    int taille;
        
    
    x_readfile(char* n) {
       buf=new char[258];
       taille=256;
       name=n;
       fichier=new ifstream(n,ios::in|ios::binary);
       if (fichier->fail() )
           pos=-1;
       else
           pos=0;
    }

	void seek(long p) {
        pos=p;
        fichier->seekg(p);
    }

    unsigned char peek() {
       return (unsigned char)fichier->peek();
    }

	char eof() {
		return fichier->eof();
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
        pos+=sz;
        currentbuffer= buf;
        return buf;
    }

    void close() {
        fichier->close();
    }        
};
x_node* x_parsing(x_reading* fx,x_parsing_mode mode);


