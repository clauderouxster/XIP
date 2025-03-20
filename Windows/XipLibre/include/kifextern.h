#ifndef kifextern_h
#define kifextern_h

void kifinitialisation();
bool openkifsession();
bool closekifsession();
int compilekifsession(const char* code);
char* theerrormsg();
char* displaykifsession();
char* listecodes();
bool loadkifsession(const char* filename);
char* lastkifdisplay();
char* lastloadedfile();
bool savekifsession(const char* filename);
bool loadkiffile(const char* filename);
void kiffilename(const char* filename);
const char* LesLignes(char* lecode);
void kifremove(int i);
long kifgotoline(char* lecode,int nb,long* mx);
void termination();
void arretprg();
bool windowmode();
void lesuivant(bool v);
void entredans(bool v);
void modebug(bool v);
void jusquefin();
const char* lindentation(char* basecode,int blancs);
int indentationVirtuel(char* cr,char* acc);
long laligne();
int ajoutebreak(const char* c,long l);
bool isbreakpoint(const char* c,long l);
void sortiefonction();
void nettoyerpointarrets();
bool kifencours();
void kifcleansession();
bool sessionactive();
char* kifligneenligne(char* lecode,long* mx,char p);
int NbLignes(const char* lecode);
bool runkifsession();
bool compileandrun(const char* codeinit);
long kiflignecourantedebug();
char* ligneparligne(char* lecode,char p);
void kifpile(int n);
bool closefinalkif();
const char* kifDisplayLocalVariable(const char* ch);
const char* kifForWhileRegular();
int KifCurrentThreadId();
const char* kifgetlineandfile(const char* msg,long* line,const char* cline,const char* dans);
bool isdebug();
void kifBlock();
void kifRelease();
void kifBlockWindow();
void kifReleaseWindow();
void kifthreadlock();
void kifthreadunlock();
void kifsticktothread(int);
const char* getkeywords();
const char* gettypes();

#ifdef WIN32
void SelectionVariables(string& s);
void GetVariables(string& s);
void ReinitVariables(string& s);
void kifsplitter(string& thestr,string thesplitter,vector<string>& v);
void kifsetarguments(string args);
int indentationvirtuel(string& basecode);
const char* indentation(char* lecode,string);
int kifaddselection(string& s);
void listebreakpoint(const char* c,vector<long>& v);
void cleancodeindente();
#endif

#endif

