//###################################################################################################################################################
//##	KiF BNF grammar ####
//###################################################################################################################################################
//###################################################################################################################################################
//## BNF grammar formalism
//###################################################################################################################################################
//
// To compile the KiF BNF grammar, you need the following files:
//	- kif (the file that contains the grammar description of the KiF formalism)
//	- compilateurC.py (a Python program, the BNF compiler)
//	- rulen.py (the BNF grammar of the our BNF formalism)
//	- optimise.kif (some improvment over the generated code, it is a KiF program)
//
// You first execute the Python program: 'compilateurC.py' with the 'kif' file as input: python compilateurC.py ./kif
// N.B. This Python program also requires 'rulen.py'.
// which generates: kifn.h, kifn.cxx and x_nodes.h
//
// You can also call: 'optimise.kif', without any parameters, it will load the last generated 'kifn.cxx' as input.
// You can skip this step if you do not have access to kif at this stage.
// It provides some additional performance improvment.
// It then generates: kifn_o.cxx, which can replace kifn.cxx in your code.
//
//###################################################################################################################################################
//## Left-hand-side of the rules ###
// The system pre-tokenizes the code beforehand, thanks to loadtoken in x_nodes.h.
// It automatically tokenizes along the following elements:
//	- Double quotes strings (type=1): "..."
//	- Single quotes strings (type=2): '...'
//	- Large strings (type=5):	@".."@
//	- Comments, which are removed from the input:
//		* line comments:	//....
//		* large comments:	/@....@/
//	- Numbers (type=3, with exponential 'e' and decimal point)
//	- Tokens (type=4): a sequence of unicode characters, which are not punctuations or spaces, to the exception of "_". '_protégée' is a valid token for instance.
//	- Punctuations (type=0)
//
// You need to use some specific meta-characters to tell the system to use these pre-defined tokens instead of their rule description.
// You still need to declare these rules, even though their description won't be taken into account.
// The BNF compiler will associate these rules with specific C++ methods, which will be based on the internal code chosen within loadtoken
//
// These meta-characters should be placed as the first character in the rule.
// Their interpretation is the following: <,>,^,=,& ,~
//		< : double quote string		(type in loadtoken is: 1)
//		> : simple quote string		(type in loadtoken is: 2)
//		~ : large strings @".."@	(type in loadtoken is: 5)
//		= : digits or number		(type in loadtoken is: 3)
//		^ : token					(type in loadtoken is: 4)
//		& : punctuation				(type in loadtoken is: 0)
//
// The following meta-characters have a different interpretation, they are used to control how the parsing should take place:
//		+: defines the entry point of the grammar, in other words, which rule to use in order to start parsing the code.
//		!: you use this meta-character to avoid the matching strings to be part of the final analysis tree
//		%: you use this meta-characte to avoid storing substrings within the analysis tree (reducing the storage to only salient strings)
//
//
//###################################################################################################################################################
//## Right-hand-side of the rules ###
// You can use the following operators in the right-hand side of a rule:
//
// Character or Strings Operators:
//	%x		: The '%' operator is used to introduce the character 'x'
//	$xxx	: The '$' operator is used to introduce the string 'xxx''
//	\ddd	: Defines a character through its unicode code.
//	X-x		: The '-' operator defines a range of characters.
//
// Operators:
//	:=		:	This operator associates a rule name with its description.
//	' '		:	The space character defines a conjunction of rule elements.
//	^		:	This operator is an exclusive disjunction, it stops at the first target that matches.
//	|		:	This operator is also a disjunction, but in this case it selects the longest match between each rule element.
//	[..]	:	This operator defines a sub-sequence of tokens. DO NOT PUT ANY SPACE BETWEEN '[' and the following element or between the last element and ']'.
//	(..)	:	This operator defines an optional sequence of tokens. DO NOT PUT ANY SPACE BETWEEN '(' and the following element or between the last element and ')'.
//	{..}	:	This operator defines a context at the end of rule, to constrain the rule execution. The context content is not appended to the current analysis tree.
//
// Kleene Operators:
// Important: These operators are compiled into 'while' loops in C++, not recursive calls...
//	r*		:	The element 'r' should be present 0 or n times. 'r' is a rule element, it can be a sequence of elements: [..]*
//	r+		:	The element 'r' should be present at least once. 'r' is a rule element, it can be a sequence of elements: [..]+
//
// Cut Operator:
//	*n = error message : Defines an error message of numerical index 'n'. These error messages must be defined at the beginning of the file.
//	;d		:	Introduces a 'point of no return' associated with a message (see below), it is the last point beyond which you cannot backtrack.
//				When a "point of no return" is reached, then if there is an error later on in the code, the system will stop and returns the last error message
//				of index 'd'.
//
//
// It is also possible to generate some BNF rules, whose contents can be modified on the fly programmatically.
// Basically, the system generates some C++ code out of the BNF rules, which is based on some variable content instead of characters or strings.
//
//	@x		:	'@' creates a "character" variable in the C++ code, whose initial value will be the character 'x'.
//				A Y_val_d (char) will be then created in the C++ class, 'd' is a digit corresponding to the order in which these variables were created.
//
//	:VARI	:	':' creates a "string" variable in the C++ code, whose initial value is the string VARI.
//				A VS_var_d (string) will then be created in the c++ class, 'd' is a digit corresponding to the order in which these variables were created.
//
// It is then possible to alter the behaviour of these BNF rules in the parser by simply modifying the initial value of these variables.
//
//
//###################################################################################################################################################
//###################################################################################################################################################
//###################################################################################################################################################
//## Error message strings, each message is introduced with '*' followed with its number index.
//## Last error message beyond which you cannot backtrack, it is declared in a rule with ';' followed by the corresponding index number.
//###################################################################################################################################################
//###################################################################################################################################################
//###################################################################################################################################################
//## This is the entry point of our parser: analyse, hence the "+" ######
//###################################################################################################################################################
//## The most basic structures ###
//^ --> we use string tokens
//########## SHELL and LABELS ##############################################
//If you define a #!path, then you do not want to parse it in your kif program
//### Used in debug mode ###
//########## OPERATORS ##############################################
//########## CURRENT DECLARATIONS ##############################################
//## Some feature definitions for functions and variables ###
//########## FUNCTIONS ##############################################
//########## THE PARSER ##############################################
//## Our entry point ####
//########## VARIABLES ##############################################
//########## IN & ON ##############################################
//########## TUPLES, VECTORS and MAPS ##############################################
//########## JSON  ##################################################
//########## PREDICATE ##############################################
//affectexpression is called for an affectation such as "is" or "="
//For predicate vectors...
//For predicate map... The order is important for the unification
//########## LAMBDA && HASKELL ##############################################
//########## EXPRESSION DEFINITIONS ##############################################
//wnexpressions is called at the beginning of an affectation, it has been implemented to take into account "-var"
// We do not want any comparison in the expression (as it might disrupt other parses, for instance we forbid two or more comparisons in a row)
// We also prevent here, the operator in to be used in an expression, to avoid confusion with "in" in Haskell
// If you need a "in" expression, then in that case it should be embedded into (..)
//< --> we use the doublestring map
//> --> we use the simplestring map
//= --> we use numbers
//########## BASIC INSTRUCTIONS ##############################################
//########## PATTERNS FOR CERTAIN SPECIALIZED OBJECTS CALLED FROM WITHIN KIF INSTRUCTIONS ##############################################
//######### Grammar pattern: tk :=  ... ############################


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

#include <sys/stat.h>

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
	bool readstrings;

	unsigned char table[255];

	virtual void Initable() {
        int i=0;
		unsigned char c;
		for (c=65;c<=90;c++)
			table[i++]=c;
		for (c=97;c<=122;c++)
			table[i++]=c;
		for (c=128;c<255;c++)
			table[i++]=c;
                table[i++]='_';
                table[i++]='#';
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
        
    virtual long Length() {
		return -1;
	}

	virtual void loadtoken(bool keepspace=false) {
		unsigned char s,c;
		Initable();
		char ch[]={0,0};
		string token;
		stack.clear();
		stackln.clear();
		stacktype.clear();

		long locpos=Length();

		if (locpos!=-1) {
			stack.reserve(locpos/2);
			stackln.reserve(locpos/2);
			stacktype.reserve(locpos/2);
		}
		char decimal=0;
		bool digit=false;
		bool exposant=false;
		
		long line=0;
		char lastc=0;
		char consumed=1;
		s=get();
		locpos=tell();
		while (!eof()) {
                        consumed=1;
			if (s=='/' && token=="" && readstrings) {
				s=get();
				if (s=='/') {
					//this is a comment
					s=get();
					while (s != 10 && s != 13) {
						if (eof())
							return;
						s = get();
					}
					token.clear();
					line++;
					lastc=s;
					s=get();
					locpos=tell();					
					continue;
				}
				if (s == '@') {
					//this is a long comment up to */
					lastc = 0;
					while (1) {
						if (eof())
							return;
						s = get();
						if (s == '@') {
							s = get();
							if (s == '/')
								break;
						}

						if (s == 10 && lastc == 13)
							lastc = 0;
						else
						if (s == 10 || s == 13) {
							lastc = s;
							line++;
						}							
					}

					token.clear();
					s = get();
					locpos = tell();
					continue;
				}
				
				seek(locpos);
				s='/';	
			}

			c=s;
			if (token=="" && readstrings) {
				if (c=='@') {
					s=get();
					if (s=='"') {
						lastc=0;
						token+=s;
						c=get();				
						while (!eof()) {
							if (c==10 && lastc==13)
								lastc=0;
							else {
								if (c==10 || c==13) {
									lastc=c;
									line++;
								}					
							}
							token+=c;
							bool fd=false;
							if (c=='"')
								fd=true;
							c=get();
							if (fd && c=='@')
								break;
						}
						stack.push_back(token);
						stackln.push_back(line);
						stacktype.push_back(5);
						token.clear();
						s=get();
						locpos=tell();				
						continue;
					}
					seek(locpos);
				}
				else {
				if (c=='"') {
					lastc=0;
					token+=c;
					c=get();				
					while (c!='"' && !eof()) {
						if (c==10 && lastc==13)
							lastc=0;
						else
							if (c==10 || c==13) {
								lastc=c;
								line++;
							}						
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
					token.clear();
					s=get();
					locpos=tell();				
					continue;
				}
				if (c=='\'') {
					token+=c;
					c=get();
					while (c!='\'' && !eof()) {
						if (c==10 && lastc==13)
							lastc=0;
						else
							if (c==10 || c==13) {
								lastc=c;
								line++;
							}						
						token+=c;
						c=get();
					}
					token+="'";
					stack.push_back(token);				
					stackln.push_back(line);
					stacktype.push_back(2);
					token.clear();
					s=get();
					locpos=tell();				
					continue;
				}
			     }
		        }
			if (strchr((char*)table,c)) {
				token+=c;
				if (c=='e' && decimal)
					exposant=true;
			}
			else {
				bool normaltoken=true;
				if (digit) {
					switch (c) {
					case 48:
					case 49:
					case 50:
					case 51:
					case 52:
					case 53:
					case 54:
					case 55:
					case 56:
					case 57:
						token+=c;
						normaltoken=false;
                                                if (decimal)
                                                    decimal=2;
						break;
					case '.':
						if (!decimal) {
							token+=c;
							decimal=1;
							normaltoken=false;
						}
						break;
					case '+':
					case '-':
						if (exposant) {
							token+=c;
							normaltoken=false;
						}
					}
				}
				else {
					if (c>=48 && c<=57) {
						normaltoken=false;
						if (token=="")
							digit=true;
						token+=c;
					}
				}

				if (normaltoken) {
                                    if (token!="") {
                                         if (digit) {
                                              if (decimal==1)
                                                   token=token.substr(0,token.size()-1);
                                              stack.push_back(token);
                                              stacktype.push_back(3);
                                              if (decimal==1) {
                                                   ch[0]='.';
                                                   stack.push_back(ch);
                                                   stackln.push_back(line);
                                                   stacktype.push_back(0);
                                              }
                                         }
                                         else {
                                              stack.push_back(token);
                                              stacktype.push_back(4);
                                         }
                                         stackln.push_back(line);
                                         token.clear();
                                         digit=false;
                                         exposant=false;
                                         decimal=false;
                                    }
					if (c>32 || keepspace) {
						ch[0]=c;
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
			}
			consumed=0;
			s=get();
			locpos=tell();
		}
		
		if (!consumed && s > 32 && s < 128) {
			ch[0] = s;
			stack.push_back(ch);
			stackln.push_back(line);
			stacktype.push_back(0);
		}
		else {
                    if (token!="") {
                            stack.push_back(token);
                            if (digit)
                                    stacktype.push_back(3);
                            else
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
        	readstrings=true;
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

        long Length() {
		return length;
	}

	char eof() {
		if (pos>length)
			return 1;
		return 0;
	}

        char get() {
			if (pos>length)
			    return -1;
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
	bool localfile;

	x_readfile(char* n,long p=0) {
	        readstrings=true;
		localfile=true;
		buf=new char[258];
		taille=256;
		name=n;
		size=-1;
		fichier=new ifstream(n,ios::in|ios::binary);
		if (fichier->fail() )
			pos=-1;
		else {
			Size();
			pos=p;
			if (pos)
				fichier->seekg(pos);
		}
	}

	long Size() {
		FILE* flocal;
		struct stat scible;
		int stcible=-1;
#ifdef WIN32
		fopen_s(&flocal,(char*)(name.c_str()),"rb");
#else
		flocal=fopen((char*)(name.c_str()),"rb");
#endif

		if (flocal!=NULL) {

#if  defined(WIN32) | defined(APPLE) | defined(XIPFSTAT64)
			stcible=fstat(flocal->_file,&scible);
#else
			stcible=fstat(flocal->_fileno,&scible);
#endif
			if (stcible>=0)
				size=scible.st_size;
			fclose(flocal);
		}
		return size;
	}

	long Length() {
		if (size==-1)
			return Size();
		return size;
	}

	x_readfile(ifstream* fic,char* n,long p) {
        	readstrings=true;
		buf=new char[258];
		localfile=false;
		taille=256;
		name=n;
		fichier=fic;
		pos=p;
		size=-1;
	}

	~x_readfile() {
		if (localfile) {
			if (fichier!=NULL)
				delete fichier;
		}
		delete[] buf;
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
