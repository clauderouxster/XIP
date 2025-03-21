//Lingveta grammar


#ifndef x_node_h
#define x_node_h

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
		static char tabaccent[]={'é','à','è','ç','ê','î','ô','â','û','ù','Ç','À','É','È','Ë','Ê','Ï','Î','Û','Ü','ä','ë','ï','ö','ü','_','#',0};
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
					if (c=='\\') {
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
			if (c=='\'') {
				token+=c;
				c=get();
				while (c!='\'' && !eof()) {
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




#endif
