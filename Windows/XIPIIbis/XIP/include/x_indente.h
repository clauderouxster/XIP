#ifndef x_indente
#define x_indente

#include "x_node.h"

class x_fltk_reading : public x_readstring {
public:

	x_fltk_reading(string& s) : x_readstring(s) {}

	virtual void loadtoken(bool keepspace=false) {
		unsigned char s,c;
		Initable();
		string ch;
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

		bool decimal=false;
		bool digit=false;
		bool exposant=false;
		long line=0;
		char lastc=0;
		s=get();
		locpos=tell();
		while (!eof()) {
			if (s=='/' && token=="") {
				s=get();
				if (s=='/') {
					//this is a comment
					token="//";
					s=get();
					token+=s;
					while (s!=10 && s!=13 && !eof()) {
						s=get();
						token+=s;
					}

					stack.push_back(token);
					stackln.push_back(line);
					stacktype.push_back(9);
					token="";
					line++;
					lastc=s;
					s=get();
					locpos=tell();					
					continue;
				}
				seek(locpos);
				s='/';	
			}

			c=s;
			if (token=="") {
				if (c=='@') {
					s=get();
					if (s=='"') {
						lastc=0;
						token=s;
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
						token="";
						s=get();
						locpos=tell();				
						continue;
					}
					seek(locpos);
				}
				else {
					if (c=='"') {
						lastc=0;
						token=c;
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
						token="";
						s=get();
						locpos=tell();				
						continue;
					}
					if (c=='\'') {
						token=c;
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
						token="";
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
						break;
					case '.':
						if (!decimal) {
							token+=c;
							decimal=true;
							normaltoken=false;
						}
						break;
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
						if (token=="") {
							digit=true;
							token=c;
						}
						else
							token+=c;
					}
				}

				if (normaltoken) {
					if (token!="") {
						stack.push_back(token);
						if (digit)
							stacktype.push_back(3);
						else
							stacktype.push_back(4);
						stackln.push_back(line);
						token="";
						digit=false;
						exposant=false;
						decimal=false;						
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
			}
			s=get();
			locpos=tell();
		}

		if (token!="") {
			stack.push_back(token);
			if (digit)
				stacktype.push_back(3);
			else
				stacktype.push_back(4);
			stackln.push_back(line);
		}
	}
};


class x_xip_reading : public x_fltk_reading {
public:

	x_xip_reading(string& s) : x_fltk_reading(s) {}

	void loadtoken(bool keepspace=false) {
		unsigned char s,c;
		Initable();		
		string ch;
		string token;
		stack.clear();
		stackln.clear();
		stacktype.clear();
		int lg=strlen((char*)table);
		table[lg-1]=0;

		long locpos=Length();

		if (locpos!=-1) {
			stack.reserve(locpos/2);
			stackln.reserve(locpos/2);
			stacktype.reserve(locpos/2);
		}

		bool decimal=false;
		bool digit=false;
		long line=0;
		char lastc=0;
		s=get();
		locpos=tell();
		while (!eof()) {
			if (s=='/' && token=="") {
				s=get();
				if (s=='/') {
					//this is a comment
					token="//";
					s=get();
					token+=s;
					while (s!=10 && s!=13 && !eof()) {
						s=get();
						token+=s;
					}

					stack.push_back(token);
					stackln.push_back(line);
					stacktype.push_back(9);
					token="";
					line++;
					lastc=s;
					s=get();
					locpos=tell();					
					continue;
				}

				token="/";				
				while (!eof() && s!='\\') {
					if (s==10)
						line++;
					token+=s;
					s=get();
				}
				token+="\\";
				stack.push_back(token);
				stackln.push_back(line);
				stacktype.push_back(9);
				token="";
				lastc=s;
				s=get();
				locpos=tell();					
				continue;
			}

			c=s;
			if (c=='"' && token=="") {
				lastc=0;
				token=c;
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
				token="";
				s=get();
				locpos=tell();				
				continue;
			}
			
			if (strchr((char*)table,c)) {
				token+=c;
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
						break;
					case '.':
						if (!decimal) {
							token+=c;
							decimal=true;
							normaltoken=false;
						}
						break;
					}
				}
				else {
					if (c>=48 && c<=57) {
						normaltoken=false;
						if (token=="") {
							digit=true;
							token=c;
						}
						else
							token+=c;
					}
				}

				if (normaltoken) {
					if (token!="") {
						stack.push_back(token);
						if (digit)
							stacktype.push_back(3);
						else {
							if ((token[0]=='$' || token[0]=='#') && token.size()>1 && token[1]>=48 && token[1]<=57)
								stacktype.push_back(6);
							else
								stacktype.push_back(4);
						}
						stackln.push_back(line);
						token="";
						digit=false;
						decimal=false;						
					}
					if (c=='#' || c=='$')
						token=c;
					else {
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
				}
			}
			s=get();
			locpos=tell();
		}

		if (token!="") {
			stack.push_back(token);
			if (digit)
				stacktype.push_back(3);
			else
				stacktype.push_back(4);
			stackln.push_back(line);
		}
	}
};



#endif
