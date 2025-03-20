#ifndef pasofficielxip_h
#define pasofficielxip_h

#include <vector>
using namespace std;
#include <string>
#include "xipstlres.h"
#include "xipvecta.h"


#ifdef pasofficielxip_variable
const char* typeVariableString[]={"UNKNOWN", "REAL", "INTEGER", "STRING", "STRUCTURE","PYTHONOBJECT","XMLOBJECT"};
#else
extern const char* typeVariableString[];
#endif

#define DISPLAY_INFORMATION 1
#define DISPLAY_COMMENTS 2

class arbretrait {
public:

    string nom;
    VECTA<arbretrait*> fils;
    arbretrait* pere;
    virtual char type();

};

class featureroot : public arbretrait {
public:
    string filename;
    char type();
};

typedef enum {VIDE,
              REGLES_SEQUENCE,
              REGLES_DI,
              REGLES_SIMPLIFICATION,
              REGLES_EXTRACTION,
              REGLES_DEDUCTION, 
              REGLES_SEMANTIQUE,
              REGLES_MARQUAGE,
              REGLES_TERME,
              REGLES_RECONSTRUCTION,
              REGLES_RECONSTRUCTION_MODELE,
              REGLES_DECOUPESEQUENCE,
              REGLES_CARTESIEN,
              REGLES_BOUCLE,
              REGLES_FRAGMENTE,
              REGLES_VOCABULAIRE,
              REGLES_PL,
              REGLES_MULTIMOTS} typeRegle;


class DecompteRegle {
public:

    typeRegle regle;
    char* nom_fichier;
    unsigned long position;
    unsigned int ligne;
    int couche;
    int couche_relative;
    void* user;
};


class Label {
public:

    char* etiquette;
    typeRegle type;
    int index;
	int numero;
    int couche;
    DecompteRegle* dr;    
};

typedef enum {VT_NONE, VT_REAL, VT_INTEGER, VT_STRING, VT_STRUCTURE} typeVariable;


Exportation VECTA<Label*>* XipReturnInfoRule(int ipar);
Exportation char XipIsGrammarCrypted(unsigned int ipar);
Exportation void XipSetRuleLimit(int ipar,int num_rule);
Exportation char XipSetLabelRuleLimit(int ipar,string num_rule);
Exportation void XipGetListCategories(int,vector<string>&);
Exportation void XipGetListDependencies(int,vector<string>&);
Exportation void XipGetListVariables(int,vector<string>&,vector<typeVariable>&);
Exportation char XipMessageLanguage(unsigned int ipar);
Exportation char XipGetVariableValue(int ipar,string variable,string& val);
Exportation arbretrait* XipGetFeatureTree(int ipar);
Exportation void XipGrammarPathnames(int ipar,
                                    string& grammar,
                                    vector<string>& features,
                                    long& debutf,
                                    vector<string>& lexicons,
                                    long& debutl,
                                    vector<string>& rules,
                                    long& debutr,
                                    vector<string>& parameters);
Exportation void XipReturnRules(int ipar,vector<int>& rulenumbers);
Exportation void XipDisplayActivatedRules(int ipar,ostringstream* os,int index, char display);
Exportation void XipGrammarInformation(int ipar,
                                   vector<string>& license,
                                   string& clef,
                                   string& version,
                                   string& language,
                                   string& grammarlanguage,
                                   string& thelocale,
                                   string& charset,
                                   char& hmm,
                                   char& reentrance,
                                   int& max_words,                                   
                                   int& max_size_word,
                                   char& lexicon_encoding,
                                   int& featurenumber,
                                   string& tracepathname,
                                   string& indentpathname);

Exportation int XipParseStringAndOS(string texte,
                                    int ipar, 
                                    XipResult* xip,
                                    ostringstream* os,
                                    char xml,
                                    char applique_grammaire);
Exportation void ListOfOperations(vector<string>& lists);
unsigned char* XipGetCurrentInternalStringBuffer(unsigned int ipar);
Exportation void XipGetUnknownVariables(VECTA<char*>& unknowns);
Exportation void XipSetErrorStream(ostringstream* os);
Exportation XIPError* XipGetLastError();
#endif
