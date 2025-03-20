#ifndef conversion_h
#define conversion_h

#ifndef Exported
#ifdef WIN32
#define Exported __declspec(dllexport)
#else
#define Exported
#endif
#endif

#ifdef WIN32
#define BULONG unsigned _int64
#define BLONG _int64
#else
#define BULONG unsigned long long
#define BLONG long long
#endif

#ifndef BULONG
#define BULONG unsigned long long
#define BLONG long long
#endif

const short token_comma=1;
const short token_separator=2;
const short token_period=4;
const short token_keepwithdigit=8;

using std::wstring;

union bulongchar {
	BULONG val;
	unsigned char ustr[8];
	char str[8];
	bulongchar() {
		memset(str,0,8);			
	}
	void clear() {
		memset(str,0,8);			
	}
};

void IndentCode(string& codestr,string& codeindente,int blancs,const char* kifelse="else",const char* kifelif="elif",const char* kifif="if");
void IndentationCode(string& codeindente,vector<string>& code,vector <int>& blancs,int mxbase,bool construit,const char* kifelse="else",const char* kifelif="elif",const char* kifif="if");
void c_normalise(string& code);

Exported string& Trim(string& chaine);
Exported string& Trimleft(string& chaine);
Exported string& Trimright(string& chaine);
void v_split(string& thestr,string thesplitter,vector<string>& v);

Exported BULONG s_XLConvert(char* number);
Exported void inittableutf8();
void s_EvaluateMetaCharacters(string& s);
void c_HTMLcode(string& s,BULONG code);

Exported void s_unicodetoutf8(string& s, wchar_t* str);
Exported void s_unicodetoutf8(string& s, wstring& str);
Exported string s_unicodetoutf8(BULONG code);
Exported void s_unicodetoutf8(BULONG code,char* utf);

Exported void s_utf8tounicode(wstring& s,char* str);
Exported void s_doubleutf8tounicode(wstring& s,wchar_t* str);
Exported int c_unicodetoutf8(BULONG code,unsigned char* utf);
Exported void c_latintoutf8(unsigned char code,unsigned char* utf);
Exported bool c_utf8tolatin(unsigned char* utf,unsigned char& code);
Exported int c_utf8tounicode(unsigned char* utf,BULONG& code);
Exported char c_testutf8(unsigned char* utf);

Exported string conversionUTF8VersLatin(unsigned char* contenu);
Exported unsigned char conversionUTF8ToLatinOne(short);
Exported string conversionLatinVersUTF8(unsigned char* contenu);
Exported string conversionUTF8VersLatin(string contenu);
Exported string conversionLatinVersUTF8(string contenu);

Exported string s_utf8todos(unsigned char* contenu);
Exported string s_dostoutf8(unsigned char* contenu);

Exported string s_utf8todos(const char* contenu);
Exported string s_dostoutf8(const char* contenu);

Exported int conversionUTF8VersGras(unsigned char* contenu,vector<string>& l);
Exported int conversionUTF8VersGras(string contenu,vector<string>& l);
Exported void conversionUTF8VersGrasOne(unsigned char* contenu,string& s);
Exported void s_latintoutf8(string& res,unsigned char* contenu);

Exported  int c_lookforUTF8(unsigned char* contenu,int longueur);

Exported short c_char_get_wide(unsigned char* m,int& i);
Exported void c_get_utf8_char(short t,string&);
Exported unsigned char c_utf8_latin(string s);

Exported string c_latin_utf8(int c);
Exported bool c_Isupper(string s);
Exported bool c_Isalpha(string s);
Exported bool c_Islower(string s);
Exported BULONG c_tolower(BULONG c);
Exported BULONG c_toupper(BULONG c);
Exported char c_is_number(unsigned char* str,char decimal,long& l,double& f);
Exported bool c_is_digit(string& str);
Exported bool c_is_punctuation(string& str);

Exported bool c_Isconsonants(unsigned char* s);
Exported bool c_Isvowels(unsigned char* s);
Exported string s_deaccentuate(unsigned char* s);

Exported void s_binary(unsigned char* s,int mx,int lg,BULONG* codes);

Exported int c_code_get(unsigned char* m,int& i,BULONG& code);
Exported int c_chartobyteposition(unsigned char* contenu,int charpos);
Exported int c_bytetocharposition(unsigned char* contenu,int charpos);
Exported int c_chartobytepositionidx(unsigned char* contenu,int charpos,int& nb,int& i);
Exported int c_bytetocharpositionidx(unsigned char* contenu,int charpos,int& nb,int& i);
Exported int c_find(unsigned char* s, string fd,int pos);
Exported int c_rfind(unsigned char* s, string fd,int pos);
Exported int c_find(string& s, string fd,int pos);
Exported int c_rfind(string& s, string fd,int pos);
Exported string c_Tolower(string s);
Exported string c_Toupper(string s);
Exported  void c_Tolower(string&,unsigned char* s,int lg);
Exported  void c_Toupper(string&,unsigned char* s,int lg);
Exported int c_size(string& s);
Exported int c_size(const char* s);
Exported string c_left(string& s,int nb);
Exported string c_right(string& s,int nb);
Exported string c_middle(string& s,int g,int nb);
Exported string c_char_index(string& s,size_t i);
Exported bool c_char_index_assign(string& s, string c,int i);
Exported string c_char_index_remove(string& s, size_t i);
Exported bool c_char_index_insert(string& s, string c,size_t i);
Exported BULONG c_char_index_code(string& s,size_t i);
Exported void c_char_index_code_all(string& s,vector<int>& vect);
Exported void c_char_index_code_all_long(string& s,vector<long>& vect);
Exported bool c_is_utf8(unsigned char* contenu,int longueur);
Exported char c_is_pure_utf8(unsigned char* contenu,int longueur);
Exported char c_is_alpha(unsigned char* m,int& i);
Exported string c_to_utf8(unsigned char* m);
Exported string c_replacestring(string& s,string& r,string& rep);
Exported string c_creplacestring(string& s,string r,string rep);
Exported string c_char_get(unsigned char* m,int& i);
Exported string c_char_get_next(unsigned char* m,int& i);
Exported int c_char_next(unsigned char* m,int& i);
Exported string c_insert_sep(string s,string sep);
Exported  string c_insert_sep_byte(string& s,string sep);
Exported string c_char_reverse(string& s,int& length);
Exported string s_revert(string& s);
Exported bool c_is_punctuation(BULONG code);
Exported bool c_is_digit(BULONG code);
Exported bool c_threedigits(wstring& s,int i);
Exported wstring c_replacewstring(wstring& str,wstring reg,wstring rep);
void v_tokenize(vector<string>& vect, string& thestr, short flag);
void s_tokenize(vector<string>& vect, string& thestr, map<string,bool>&);


class agnostring : public string {
public:
	int bytepos;
	int charpos;
	agnostring() : string() {}

	agnostring(const char* s) : string(s) {bytepos=0;charpos=0;}
	agnostring(const unsigned char* s) : string((const char*)s) {bytepos=0;charpos=0;}
	agnostring(char* s) : string(s) {bytepos=0;charpos=0;}
	agnostring(unsigned char* s) : string((char*)s) {bytepos=0;charpos=0;}
	agnostring(string s) : string(s) {bytepos=0;charpos=0;}
	agnostring(agnostring& s) : string(s.c_str()) {bytepos=0;charpos=0;}
	agnostring(BULONG b) : string(s_unicodetoutf8(b)) {bytepos=0;charpos=0;}

	BULONG tonumber() {
		return s_XLConvert((char*)c_str());
	}

	string tolower() {
		return c_Tolower(*this);
	}

	string toupper() {
		return c_Toupper(*this);
	}

	bool isupper() { 
		return c_Isupper(*this);
	}

	bool isalpha() {
		return c_Isalpha(*this);
	}

	bool islower() {
		return c_Islower(*this);
	}

	bool isconsonant() { 
		return c_Isconsonants((unsigned char*)c_str());
	}

	bool isvowel() { 
		return c_Isvowels((unsigned char*)c_str());
	}

	bool ispunctuation() {
		return c_is_punctuation(*this);
	}

	string deaccentuate() {
		return s_deaccentuate((unsigned char*)c_str());
	}

	bool isdigit() { 
		return c_is_digit(*this);
	}

	char isnumber(char decimal,long& l,double& f) { 
		return c_is_number((unsigned char*)c_str(),decimal,l,f);
	}

	bool isutf8() { 
		return c_is_utf8((unsigned char*)c_str(),size());
	}

	char ispureutf8() { 
		return c_is_pure_utf8((unsigned char*)c_str(),size());
	}

	string utf8tolatin() {
		return conversionUTF8VersLatin((unsigned char*)c_str());
	}

	string latintoutf8() {
		return conversionLatinVersUTF8((unsigned char*)c_str());
	}

	string dostoutf8() {
		return s_dostoutf8((unsigned char*)c_str());
	}

	string utf8todos() {
		return s_utf8todos((unsigned char*)c_str());
	}

	void begin() {
		bytepos=0;
		charpos=0;
	}

	bool end() {
		if (bytepos>=size())
			return true;
		return false;
	}

	int getbytepos() {
		return bytepos;
	}

	void setbytepos(int i) {
		bytepos=i;
		charpos=c_bytetocharposition((unsigned char*)c_str(),bytepos);
	}

	int getcharpos() {
		return charpos;
	}

	void setcharpos(int i) {
		charpos=i;
		bytepos=c_chartobyteposition((unsigned char*)c_str(),charpos);
	}

	string next() {
		charpos++;
		return c_char_get_next((unsigned char*)c_str(),bytepos);
	}

	BULONG nextcode() {
		charpos++;
		BULONG v;
		bytepos+=1+c_utf8tounicode((unsigned char*)c_str()+bytepos,v);
		return v;
	}

	void following() {
		charpos++;
		bytepos+=1+c_testutf8((unsigned char*)c_str()+bytepos);
	}

	string next(int& i) {
		return c_char_get_next((unsigned char*)c_str(),i);
	}

	BULONG nextcode(int& i) {
		BULONG v;
		i+=1+c_utf8tounicode((unsigned char*)c_str()+i,v);
		return v;
	}

	void following(int& i) {
		i+=1+c_testutf8((unsigned char*)c_str()+i);
	}

	char getbyte(int i) {
		string& s=*this;
		return s[i];
	}

	string operator [](int i) {
		return c_char_index(*this,i);
	}
	
	string trim() {
		return Trim(*this);
	}

	string trimleft() {
		return Trimleft(*this);
	}

	string trimright() {
		return Trimright(*this);
	}

	string replace(string r,string rep) {
		return c_replacestring(*this,r,rep);
	}

	bool insert(string c,size_t i) {
		return c_char_index_insert(*this,c,i);
	}

	int find(string fd,int pos) {
		return c_find((unsigned char*)c_str(),fd,pos);
	}

	int rfind(string fd,int pos) {
		return c_rfind((unsigned char*)c_str(),fd,pos);
	}

	string extract(int g,int nb) {
		return c_middle(*this,g,nb);
	}

	string left(int nb) {
		return c_left(*this,nb);
	}

	string right(int nb) {
		return c_right(*this,nb);
	}

	int chartobyteposition(int pos) {
		return c_chartobyteposition((unsigned char*)c_str(),pos);
	}

	int bytetocharposition(int pos) {
		return c_bytetocharposition((unsigned char*)c_str(),pos);
	}

	void tokenize(vector<string>& vect,bool comma,bool separator,bool keepwithdigit) {
		short flags=token_period;
		if (comma)
			flags|=token_comma;
		if (separator)
			flags|=token_separator;
		if (keepwithdigit)
			flags|=token_keepwithdigit;
		v_tokenize(vect,*this,flags);
	}

	void tokenize(vector<string>& vect,short flag) {
		v_tokenize(vect,*this,flag);
	}

	void stokenize(vector<string>& vect,map<string,bool>& keeps) {
		s_tokenize(vect, *this, keeps);
	}

	void unicodes(vector<int>& vect) {
		c_char_index_code_all(*this,vect);
	}

	void unicodes(vector<long>& vect) {
		c_char_index_code_all_long(*this,vect);
	}

	size_t sizec() {
		return c_size(c_str());
	}
 
	string revert() {
		return s_revert(*this);
	}

	Exported string insertseparator(string sep) {
		return c_insert_sep(*this,sep);
	}

	BULONG code(size_t i) {
		return c_char_index_code(*this,i);
	}	
};

#endif

