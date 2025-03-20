// automaton.h : main header file for the automaton DLL
//

#ifndef automaton_h
#define automaton_h

class arc;
class etat;
class automate;



#define AUTOMATONSTART 1
#define AUTOMATONEND   2
#define AUTOMATONNODE 4
#define AUTOMATEPILE 2

typedef enum {
    ARCINITIAL,
    ARCSIMPLE,
    ARCCOMPLEXE,
    ARCBOUCLE,
    ARCCROCHET,
    ARCVIRGULE,
    ARCFINCROPAR,
    ARCPARENTHESE,
	ARCCODE,
	ARCANY} typeautoparse;

typedef enum {AUTOMATONARC,AUTOMATONARCARACTERE,AUTOMATONARCCHAINE} typearc;


typedef enum {AUTOMATONIF,AUTOMATONELSE,AUTOMATONEPSILON,AUTOMATONANY,AUTOMATONDIGIT,AUTOMATONPUNCT,
AUTOMATONLETTER,
AUTOMATONLETTERUPPER,
AUTOMATONLABEL,
AUTOMATONVARIABLE,
AUTOMATONSTAR='*',AUTOMATONPLUS='+'} arcautomate;

typedef enum {
AUTOMATONMARKEDFORDELETION,
AUTOMATONDELETED,
AUTOMATONNONE} arcmanip;

char analyseCaractere(unsigned char c);
arcautomate TestId(char* m);
char chainecomplexe(unsigned char* chaine,int position, typeautoparse state);
char typeautomate(unsigned char* chaine,int position);

typedef enum {
        AERR_NO_ERROR,
        AERR_MISSING_PARENTHESIS,
        AERR_MISSING_BRACKET,
        AERR_WRONG_NEGATION_POSITION,
        AERR_WRONG_USE_OF_COMMA,
        AERR_WRONG_KLEENE,
        AERR_MISMATCHED_PARENTHESIS,
        AERR_MISMATCHED_BRACKET,
        AERR_WRONG_VARIABLE_ORDER,
		AERR_WRONG_CODE} autoerreur;


class etatarc {
public:
    etat* e;
    arc* ar;    

    etatarc(etat* ee, arc* aar) {
        e=ee;
        ar=aar;        
    }
};

class etat {
public:
    char status;
    char marque;
    int nombre;
    void* valeur;
    size_t id;
    VECTA<arc*> arcs;        
    

    char start() {
        if ( (status& AUTOMATONSTART)==AUTOMATONSTART)
            return 1;
        return 0;
    }

    char end() {
        if ( (status & AUTOMATONEND)==AUTOMATONEND)
            return 1;
        return 0;
    }

    char node() {
        if ( (status & AUTOMATONNODE)==AUTOMATONNODE)
            return 1;
        return 0;
    }


    
    etat(automate* a,char s=AUTOMATONSTART);
    etat();

    void marquedestruction(VECTA<arc*>& lesarcs);

    void aupluspres();
    etat* parse(automate*,
        unsigned char* chaine,
        int position, 
        int& finchaine,
        unsigned char fin,
        void* value, etat* 
        debut,arc*, 
        etat*,
        typeautoparse state,
        char negation);    
    
    etat* parsebrute(automate* a,                 
        unsigned char* chaine,
        int position,                                   
        void* value,                 
        typeautoparse state);
	etat* parsebrutelin(automate* a,unsigned char* chaine,void* value);
    arc* trouvearc(unsigned char l,arcautomate,char,char);
    arc* trouvearc(arc* a);
    void ajoutearc(arc* a);
    void compose(etat* a,VECTA<void*>& res);        
    void recherchetous(unsigned char* mot,VECTA<void*>& res,int i,int max);    
    etat* recherche(unsigned char* mot,automate*,int i,int max);
    etat* recherchelin(unsigned char* mot,automate* ref,int i);
    etat* recherchepartielle(unsigned char* mot,automate*,int i);

    void print(int p);
    void chaine(char*);
    void ArcEpsilon(VECTA<etatarc*>& eas);
    int CalculChemin(int max);
    void fusion(VECTA<arc*>&);
    void dedoublonne();
    void recherchetouslin(unsigned char* mot,VECTA<void*>& res,int i);
    char retirebrute(unsigned char* mot,automate* ref,int i);
    
};



class automate {
public:
    size_t dernier;
    VECTA<etat*> etats;
    autoerreur erreur;
    int lieuerreur;
    int maxpos;
    
    
    etat e;

    automate();
    
    automate(unsigned char* m,void* ind);

    ~automate();

	void retirefordeleted(VECTA<arc*>& adetruire);
    virtual void affecte(unsigned char chaines[10][TailleChaine]) {}
    virtual void extraitvariables(unsigned char* ch,unsigned char chaines[10][TailleChaine]) {}
    virtual void extraitvariables(unsigned char* ch,automate*) {}
    virtual void extraction(unsigned char* ch) {}
    virtual void mot(char*);
    void aupluspres();
    void chaine(char*);
    int compare(automate* a);
    void duplique(automate* a);    
    etat* parse(unsigned char* mot,void* valeur,char ordre=1);
    etat* parsebrute(unsigned char* mot,void* valeur);
	etat* parsebrutelin(unsigned char* mot,void* valeur);
    void recherchetouslabels(unsigned char* mot,VECTA<void*>& res);
    etat* recherche(unsigned char* mot);
    void recherchetouslabels(char* mot,VECTA<void*>& res);
    etat* recherchepartielle(unsigned char*);
    etat* recherche(char* mot);
    void compose(automate& a,VECTA<void*>& res);
    void demarque();
    void print();
    void retireEpsilon(VECTA<arc*>&);
    void initErreur(int,autoerreur);
    void marquedestruction();    
    virtual char Type() {return NON;}
    virtual void initvar() {}
    virtual void majvar(arc*,int) {}
    virtual char construitvar(unsigned char lab,int) {return 1;}
    virtual void testordre(char ordre) {}
    void fusion(VECTA<arc*>&);
    void nettoie();
    int rechercheavecextraction(unsigned char* m,string&);
    int rechercheavecextractionparlafin(unsigned char* m,string& res);
    int rechercheavecextractiondansvecteur(unsigned char* m,VECTA<string*>& vect);
    char retirebrute(unsigned char* mot);
    int recherchejusqualafin(unsigned char* m,string& res);
};

class automatepile : public automate {
public:

    int variables[10];   
    int maxvar;

    automatepile() : automate() {raz();};
    
    automatepile(unsigned char* m,void* ind) : automate(m,ind) {raz();};

    unsigned char chaines[10][TailleChaine];
    void affecte(unsigned char chaines[10][TailleChaine]);
    void extraitvariables(unsigned char* ch,unsigned char chaines[10][TailleChaine]);
    void extraitvariables(unsigned char* ch,automate*);
    void extraction(unsigned char* ch);    
    void mot(char*);
    void initvar();
    void majvar(arc*,int);
    char construitvar(unsigned char lab,int);
    void testordre(char ordre);
    void raz();
    char Type() {return AUTOMATEPILE;}
};


class arc {
public:
                
    unsigned char label;
    char negation;
    char marque;

    arcautomate action;
	arcmanip manipulation;
    etat* e;
    

    arc(arcautomate a,char n,unsigned char l=0) {       
		manipulation=AUTOMATONNONE;
        label=l;        
        e=NULL;   
        negation=n;
        action=a;        
        marque=0;
    }    
    
    void marquedestruction(VECTA<arc*>& lesarcs);

    char compare(unsigned char c) {        
        return (c==label);
    }
    char test(unsigned char c);
    void chaine(char*);

    void aupluspres(int id);
    void recherchetous(unsigned char* mot,VECTA<void*>& res,int i,int max);
    etat* recherche(unsigned char* mot,automate*,int i,int max);    
    void print(int p,int id);
    void compose(arc* a,VECTA<void*>& res);
    void ArcEpsilon(VECTA<etatarc*>& eas);
    int CalculChemin(int max);
    void fusion(VECTA<arc*>&);
    void dedoublonne();
    char retirebrute(unsigned char* mot,automate* ref,int i);
};

#endif


