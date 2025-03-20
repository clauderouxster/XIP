//Kif Formalism



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
	long line;

	x_node(long l) {
		init=0;
		start=0;
		end=0;
		line=l;
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
    string name;
    int errornumber;

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
		if (size!=-1 && pos>=size)
			return 1;
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




