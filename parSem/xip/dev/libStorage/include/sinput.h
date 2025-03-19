#ifndef sinput_h
#define sinput_h
#define MAXARGS     31
#define TM(x) (d.declencheChaine[x]!=-1)

typedef struct {
	//The list of all integer parameters.
	char description[100];
	int* entier[MAXARGS];
	int mementier[10];
	char declencheEntier[MAXARGS];
	int ien;
	char DecimalCharacter;
	char copy;
	int depthError;
	char erreur;
	//The list of all float parameters.
	float* flottant[MAXARGS];
	int memflottant[10];
	char declencheFlottant[MAXARGS];
	int ifl;
	//The list of all strings with their associated length
	char*  string[MAXARGS];
	char** chaine[MAXARGS];
	int* longueur[MAXARGS];
	int memchaine[10];
	char declencheChaine[MAXARGS]; 
	int ich;
	//The list of specified substrings in the d.description parameter
	char* souschaine[MAXARGS*2];
	int lsouschaine[MAXARGS*2];
	int isc;
	char ImmediateMatching;
} TansSinputData;

typedef struct{
	char iImmediateMatching;
	char iStopString[400];
	char iInString[400];
	char decimal;
	char copy;
} TansSinputInitialData;

class TansSinput {
	TansSinputData d;
	TansSinputInitialData InitialesDonnees;
	char DejaInitialisee;
	char ErrorMessage[255];
	char* StartOfString;
	char StopString[400];
	char InString[400];
	char isNumber(char ch,char DecimalCharacter);
	int TestAlphaString(unsigned char charac);
	char MemoryTest(int posvar);
	char FindString(char* ch,char** start,char** afterstart,char** endOfstring,int debut,int IM);
	public:
	TansSinput() {
		DejaInitialisee=0;
	}
	TansSinput(char immediat,char copy, char decimal,char* instring,char* stopstring);
	void GetError(char* msg,char**);
	char* Extract(char* inputstring, const char* description,char** endOfstring, ...);
	void Init(char immediat,char copy, char decimal,char* instring,char* stopstring);
	
};
#endif


