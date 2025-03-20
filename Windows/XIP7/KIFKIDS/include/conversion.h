#ifndef conversion_h
#define conversion_h

#ifndef Exporting
#if defined(WIN32)
#define Exporting __declspec(dllexport)
#else
#define Exporting
#endif
#endif

#define BULONG unsigned long long
#define BLONG long long

const short token_comma = 1;
const short token_separator = 2;
const short token_period = 4;
const short token_keepwithdigit = 8;
const short token_keeprc = 16;

using std::wstring;

union bulongchar {
	BULONG val;
	unsigned char ustr[8];
	char str[8];
	bulongchar() {
		memset(str, 0, 8);
	}
	void clear() {
		memset(str, 0, 8);
	}
};

//--------------------- Main Initialization, to be launched before using any of the following methods...
Exporting void inittableutf8();

//--------------------- Code Indention
void IndentCode(string& codestr, string& codeindente, int blancs, const char* kifelse = "else", const char* kifelif = "elif", const char* kifif = "if");
void IndentationCode(string& codeindente, vector<string>& code, vector <int>& blancs, int mxbase, bool construit, const char* kifelse = "else", const char* kifelif = "elif", const char* kifif = "if");
int VirtualIndentation(string& codestr, const char* kifelse, const char* kifelif, const char* kifif);

//--------------------- Carriage Return normalisation
void cr_normalise(string& code);

//--------------------- Trimming strings
Exporting string& Trim(string& chaine);
Exporting string& Trimleft(string& chaine);
Exporting string& Trimright(string& chaine);

Exporting wstring& Trim(wstring& chaine);
Exporting wstring& Trimleft(wstring& chaine);
Exporting wstring& Trimright(wstring& chaine);

//--------------------- Digits conversion

BLONG conversionintegerhexa(char* number);
double conversionfloat(char* s);
double convertfloat(char* s);
double convertfloat(wstring value);
long convertinteger(wstring value);

//--------------------- Conversions
Exporting bool valid_latin_table(int table);

void s_EvaluateMetaCharacters(string& s);

Exporting bool s_is_utf8(unsigned char* contenu, int longueur);

Exporting char s_is_pure_utf8(unsigned char* contenu, int longueur);
Exporting char c_detect_utf8(unsigned char* utf);
Exporting char c_test_utf8(unsigned char* utf);

Exporting string c_unicode_to_utf8(wchar_t code);

Exporting string s_unicode_to_utf8(BULONG code);
Exporting string s_utf8_to_dos(unsigned char* contenu);
Exporting string s_dos_to_utf8(unsigned char* contenu);
Exporting string s_utf8_to_dos(const char* contenu);
Exporting string s_dos_to_utf8(const char* contenu);

Exporting void s_unicode_to_utf8(string& s, wchar_t* str);
Exporting void s_unicode_to_utf8(string& s, wstring& str);
Exporting void s_unicode_to_utf8(BULONG code, char* utf);
Exporting void s_utf8_to_unicode(wstring& s, char* str);
Exporting void s_doubleutf8_to_unicode(wstring& s, wchar_t* str);
Exporting void c_latin_to_utf8(unsigned char code, unsigned char* utf);

Exporting BULONG c_latin_table_to_unicode(int table, BULONG c);
Exporting BULONG c_unicode_to_latin(BULONG u);

//--------------------- Conversion with the recipient string cleared
Exporting void sc_unicode_to_utf8(string& s, wstring& str);
Exporting void sc_utf8_to_unicode(wstring& s, char* str);

//--------------------- Character conversion
Exporting void c_HTMLcode(string& s, BULONG code);
Exporting void c_XMLcode(string& s, BULONG code);
Exporting bool c_utf8_to_latin(unsigned char* utf, unsigned char& code);
Exporting int c_utf8_to_unicode(unsigned char* utf, BULONG& code);
Exporting int c_unicode_to_utf8(BULONG code, unsigned char* utf);
Exporting int c_lookforUTF8(unsigned char* contenu, int longueur);

//--------------------- String conversion
Exporting unsigned char conversion_utf8_to_latin(short);
Exporting string conversion_utf8_to_latin(string contenu);
Exporting string conversion_latin_to_utf8(string contenu);
Exporting string conversion_latin_to_utf8(unsigned char* contenu);
Exporting string conversion_utf8_to_latin(unsigned char* contenu);

Exporting string conversion_latin_table_to_utf8(int, unsigned char* contenu);
Exporting wstring conversion_latin_table_to_unicode(int table, unsigned char* contenu);

Exporting int conversion_utf8_to_fat(unsigned char* contenu, vector<string>& l);

Exporting char* s_utf8_to_latin(char* str);
Exporting void s_latin_to_utf8(string& res, unsigned char* contenu);

//--------------------- UNICODE Manipulation
Exporting bool b_alpha(BULONG c);

Exporting bool c_is_space(BULONG c);
Exporting bool c_is_punctuation(BULONG c);
Exporting bool c_is_digit(BULONG code);
Exporting bool c_is_lower(BULONG code);
Exporting bool c_is_upper(BULONG code);

Exporting char c_is_alpha(BULONG c);

Exporting BULONG c_to_lower(BULONG c);
Exporting BULONG c_to_upper(BULONG c);

//--------------------- Character manipulation
Exporting void c_to_lower(string&, unsigned char* s, int lg);
Exporting void c_to_upper(string&, unsigned char* s, int lg);

Exporting char c_is_alpha(unsigned char* m, int& i);

Exporting string c_latin_utf8(int c);

//--------------------- String manipulation
Exporting bool s_is_upper(string s);
Exporting bool s_is_alpha(string s);
Exporting bool s_is_lower(string s);
Exporting bool s_is_digit(string& str);
Exporting bool s_is_punctuation(string& str);
Exporting bool s_is_space(string& str);


Exporting bool s_is_punctuation(wstring& str);
Exporting bool s_is_alpha(wstring& s);
Exporting bool s_is_upper(wstring& s);
Exporting bool s_is_lower(wstring& s);
Exporting bool s_is_vowel(wstring& s);
Exporting bool s_is_consonant(wstring& s);
Exporting bool s_is_digit(wstring& str);

Exporting bool s_is_consonant(unsigned char* s);
Exporting bool s_is_vowel(unsigned char* s);

Exporting string s_to_lower(string s);
Exporting string s_to_upper(string s);
Exporting string s_deaccentuate(unsigned char* s);

Exporting wstring s_to_upper(wstring& s);
Exporting wstring s_to_lower(wstring& s);
Exporting wstring s_deaccentuate(wstring& s);

Exporting char s_is_number(unsigned char* str, char decimal, long& l, double& f);
Exporting void s_binary(unsigned char* s, int mx, int lg, BULONG* codes);

//--------------------- Finding or replacing
Exporting char* strrstr(char* str, char* sub, long sz, long szsub);
Exporting wchar_t* wcsrstr(wchar_t* str, wchar_t* sub, long sz, long szsub);
Exporting void s_findall(string& s, string& substr, vector<long>& v);
Exporting void s_findall(wstring& s, wstring& substr, vector<long>& v);
Exporting void s_findall(char* s, char* substr, vector<long>& v);

Exporting long s_findbyte(string& s, string& substr, long i);
Exporting long s_rfindbyte(string& s, string& substr, long i);
Exporting int s_find(unsigned char* s, string fd, int pos);
Exporting int s_find(string& s, string fd, int pos);
Exporting int s_rfind(unsigned char* s, string fd, int pos);
Exporting int s_rfind(string& s, string fd, int pos);
Exporting int s_find(wstring& s, wstring substr, int i);
Exporting int s_rfind(wstring& s, wstring substr, int pos);

//--------------------- Extracting substring
Exporting string s_left(string& s, int nb);
Exporting string s_right(string& s, int nb);
Exporting string s_middle(string& s, int g, int nb);

Exporting string s_replacestring(string& s, string& r, string& rep);
Exporting string s_replacestrings(string& s, string r, string rep);

Exporting string s_insert_sep(string s, string sep);
Exporting string s_insert_sep_byte(string& s, string sep);
Exporting string s_char_reverse(string& s, int& length);
Exporting string s_revert(string& s);

Exporting wstring s_left(wstring& s, int nb);
Exporting wstring s_right(wstring& s, int nb);
Exporting wstring s_middle(wstring& s, int l, int nb);
Exporting wstring s_replacestring(wstring& str, wstring reg, wstring rep);

Exporting wstring s_revert(wstring& s);


Exporting int size_c(string& s);
Exporting int size_c(const char* s);

Exporting void s_split(string& s, string& splitter, vector<string>& strs, bool keepblanks = false);
Exporting void s_split(wstring& s, wstring& splitter, vector<wstring>& strs, bool keepblanks = false);
Exporting void v_split(string& thestr, string thesplitter, vector<string>& v);


//--------------------- Character extraction from potential UTF8 string-----------------
Exporting short c_char_get_wide(unsigned char* m, int& i);

Exporting unsigned char c_utf8_latin(string s);

Exporting int c_code_get(unsigned char* m, int& i, BULONG& code);
Exporting int c_chartobyteposition(unsigned char* contenu, int charpos);
Exporting int c_bytetocharposition(unsigned char* contenu, int charpos);
Exporting int c_chartobytepositionidx(unsigned char* contenu, int charpos, int& nb, int& i);
Exporting int c_bytetocharpositionidx(unsigned char* contenu, int charpos, int& nb, int& i);
Exporting int c_char_next(unsigned char* m, int& i);

Exporting string c_char_index(string& s, size_t i);
Exporting string c_char_index_remove(string& s, size_t i);

Exporting bool c_char_index_assign(string& s, string c, int i);
Exporting bool c_char_index_insert(string& s, string c, size_t i);

Exporting BULONG c_char_index_code(string& s, size_t i);

Exporting void c_get_utf8_char(short t, string&);
Exporting void c_char_index_code_all(string& s, vector<int>& vect);
Exporting void c_char_index_code_all_long(string& s, vector<long>& vect);

Exporting string c_to_utf8(unsigned char* m);
Exporting string c_char_get(unsigned char* m, int& i);
Exporting string c_char_get_next(unsigned char* m, int& i);

//--------------------- Tokenization
Exporting bool s_threedigits(wstring& s, int i);


void vw_tokenize(vector<wstring>& vect, wstring& thestr, short flag);
void s_tokenize(vector<wstring>& vect, wstring& thestr, map<wstring, bool>&);
void v_tokenize(vector<string>& vect, string& thestr, short flag);
void s_tokenize(vector<string>& vect, string& thestr, map<string, bool>&);

//--------------------- agnostring implementation

class agnostring : public string {
public:
	int bytepos;
	int charpos;

	agnostring() : string() {}
	agnostring(agnostring& s) : string(s.c_str()) { bytepos = 0; charpos = 0; }

	agnostring(const char* s) : string(s) { bytepos = 0; charpos = 0; }
	agnostring(const unsigned char* s) : string((const char*)s) { bytepos = 0; charpos = 0; }

	agnostring(char* s) : string(s) { bytepos = 0; charpos = 0; }
	agnostring(unsigned char* s) : string((char*)s) { bytepos = 0; charpos = 0; }

	agnostring(string s) : string(s) { bytepos = 0; charpos = 0; }
	agnostring(wstring s) { s_unicode_to_utf8(*this, s);  bytepos = 0; charpos = 0; }

	agnostring(BULONG b) : string(s_unicode_to_utf8(b)) { bytepos = 0; charpos = 0; }

	BULONG tonumber() {
		return conversionintegerhexa((char*)c_str());
	}

	string tolower() {
		return s_to_lower(*this);
	}

	string toupper() {
		return s_to_upper(*this);
	}

	bool isupper() {
		return s_is_upper(*this);
	}

	bool isalpha() {
		return s_is_alpha(*this);
	}

	bool islower() {
		return s_is_lower(*this);
	}

	bool isconsonant() {
		return s_is_consonant((unsigned char*)c_str());
	}

	bool isvowel() {
		return s_is_vowel((unsigned char*)c_str());
	}

	bool ispunctuation() {
		return s_is_punctuation(*this);
	}

	bool isspace() {
		return s_is_space(*this);
	}

	string deaccentuate() {
		return s_deaccentuate((unsigned char*)c_str());
	}

	bool isdigit() {
		return s_is_digit(*this);
	}

	char isnumber(char decimal, long& l, double& f) {
		return s_is_number((unsigned char*)c_str(), decimal, l, f);
	}

	bool isutf8() {
		return s_is_utf8((unsigned char*)c_str(), size());
	}

	char ispureutf8() {
		return s_is_pure_utf8((unsigned char*)c_str(), size());
	}

	string utf8tolatin() {
		return conversion_utf8_to_latin((unsigned char*)c_str());
	}

	wstring utf8tounicode() {
		wstring ws;
		s_utf8_to_unicode(ws, (char*)c_str());
		return ws;
	}

	wstring latintounicode() {
		wstring ws;
		s_utf8_to_unicode(ws, (char*)c_str());
		return ws;
	}

	string latintoutf8() {
		return conversion_latin_to_utf8((unsigned char*)c_str());
	}

	string latintoutf8(int table) {
		return conversion_latin_table_to_utf8(table, (unsigned char*)c_str());
	}

	wstring latintounicode(int table) {
		return conversion_latin_table_to_unicode(table, (unsigned char*)c_str());
	}

	string dostoutf8() {
		return s_dos_to_utf8((unsigned char*)c_str());
	}

	string utf8todos() {
		return s_utf8_to_dos((unsigned char*)c_str());
	}

	void begin() {
		bytepos = 0;
		charpos = 0;
	}

	bool end() {
		if (bytepos >= size())
			return true;
		return false;
	}

	int getbytepos() {
		return bytepos;
	}

	void setbytepos(int i) {
		bytepos = i;
		charpos = c_bytetocharposition((unsigned char*)c_str(), bytepos);
	}

	int getcharpos() {
		return charpos;
	}

	void setcharpos(int i) {
		charpos = i;
		bytepos = c_chartobyteposition((unsigned char*)c_str(), charpos);
	}

	void setpos(int b, int c) {
		bytepos = b;
		charpos = c;
	}

	string next() {
		charpos++;
		return c_char_get_next((unsigned char*)c_str(), bytepos);
	}

	BULONG nextcode() {
		charpos++;
		BULONG v;
		bytepos += 1 + c_utf8_to_unicode((unsigned char*)c_str() + bytepos, v);
		return v;
	}

	void following() {
		charpos++;
		bytepos += 1 + c_test_utf8((unsigned char*)c_str() + bytepos);
	}

	string next(int& i) {
		return c_char_get_next((unsigned char*)c_str(), i);
	}

	BULONG nextcode(int& i) {
		BULONG v;
		i += 1 + c_utf8_to_unicode((unsigned char*)c_str() + i, v);
		return v;
	}

	void following(int& i) {
		i += 1 + c_test_utf8((unsigned char*)c_str() + i);
	}

	char getbyte(int i) {
		string& s = *this;
		return s[i];
	}

	string operator [](int i) {
		return c_char_index(*this, i);
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

	string replace(string r, string rep) {
		return s_replacestring(*this, r, rep);
	}

	bool insert(string c, size_t i) {
		return c_char_index_insert(*this, c, i);
	}

	int find(string fd, int pos) {
		return s_find((unsigned char*)c_str(), fd, pos);
	}

	int rfind(string fd, int pos) {
		return s_rfind((unsigned char*)c_str(), fd, pos);
	}

	void findall(string fd, vector<long>& v) {
		s_findall(*this, fd, v);
	}

	string extract(int g, int nb) {
		return s_middle(*this, g, nb);
	}

	string left(int nb) {
		return s_left(*this, nb);
	}

	string right(int nb) {
		return s_right(*this, nb);
	}

	int chartobyteposition(int pos) {
		return c_chartobyteposition((unsigned char*)c_str(), pos);
	}

	int bytetocharposition(int pos) {
		return c_bytetocharposition((unsigned char*)c_str(), pos);
	}

	void tokenize(vector<string>& vect, bool comma, bool separator, bool keepwithdigit) {
		short flags = token_period;
		if (comma)
			flags |= token_comma;
		if (separator)
			flags |= token_separator;
		if (keepwithdigit)
			flags |= token_keepwithdigit;
		v_tokenize(vect, *this, flags);
	}

	void tokenize(vector<string>& vect, short flag) {
		v_tokenize(vect, *this, flag);
	}

	void stokenize(vector<string>& vect, map<string, bool>& keeps) {
		s_tokenize(vect, *this, keeps);
	}

	void tokenize(vector<wstring>& vect, bool comma, bool separator, bool keepwithdigit) {
		short flags = token_period;
		if (comma)
			flags |= token_comma;
		if (separator)
			flags |= token_separator;
		if (keepwithdigit)
			flags |= token_keepwithdigit;

		wstring ws;
		s_utf8_to_unicode(ws, (char*)c_str());
		vw_tokenize(vect, ws, flags);
	}


	void tokenize(vector<wstring>& vect, short flag) {
		wstring ws;
		s_utf8_to_unicode(ws, (char*)c_str());
		vw_tokenize(vect, ws, flag);
	}

	void stokenize(vector<wstring>& vect, map<wstring, bool>& keeps) {
		wstring ws;
		s_utf8_to_unicode(ws, (char*)c_str());
		s_tokenize(vect, ws, keeps);
	}

	void split(string thesplitter, vector<string>& v) {
		v_split(*this, thesplitter, v);
	}

	void unicodes(vector<int>& vect) {
		c_char_index_code_all(*this, vect);
	}

	void unicodes(vector<long>& vect) {
		c_char_index_code_all_long(*this, vect);
	}

	size_t sizec() {
		return size_c(c_str());
	}

	string revert() {
		return s_revert(*this);
	}

	Exporting string insertseparator(string sep) {
		return s_insert_sep(*this, sep);
	}

	BULONG code(size_t i) {
		return c_char_index_code(*this, i);
	}

	int count(string x) {
		int cp = 0;
		int ps = 0;
		while ((ps = find(x, ps)) != string::npos) {
			cp++;
			ps++;
		}
		return cp;
	}

	int count(BULONG b) {
		int cp = 0;
		begin();
		while (!end()) {
			if (b == nextcode())
				cp++;
		}
		return cp;
	}
};

#endif

