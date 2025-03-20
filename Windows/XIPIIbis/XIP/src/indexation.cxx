/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.60
   filename   : indexation.cxx
   Date       : 10/01/1999
   Purpose    : Indexing of seamntic disambiguation rules
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"
extern ostream* flot_erreur;

void NormaliseNomFichier(char* nomFichier,char* tampon,char* repertoire);
void lance_erreur(char* message);
extern GlobalParseur* parseur_courant;
int MAXVIRTUELVALEURINITIALE=5000;

MemoireVirtuelleSemantique::MemoireVirtuelleSemantique(int val) :  
    memoire(MAXVIRTUELVALEURINITIALE),
    nouveauxElementsDeRegles(20),
    nouveauxTests(MAXVIRTUELVALEURINITIALE) {
    max=0;indicateur=0;
}

	void MemoireVirtuelleSemantique::ajuste(int val) {
		memoire.ajuste(val);
		nouveauxTests.ajuste(val);
	}

char lettrebrute(unsigned char c) {

    if (c<=32 || strchr("<>{}[](),;:.&|!/\\=~#@^\"",c))
        return NON;

    return c;
}


unsigned char indexeur::lire(ifstream& fichier) {
    unsigned char x=fichier.get();
    if (x=='\n')
        lignes++;
    return x;
}

char indexeur::recherche_champ(ifstream& fichier) {
	 char tampon[500];
	 char traits[1000];
	 uchar c;
     char sem1[] = "semantic";
     char sem2[] = "sémantique";

	 while (!fichier.eof()) {
		LitCategorie(fichier,tampon,traits,c);
		if (c==':' && (!strcmp(tampon,sem1) || !strcmp(tampon,sem2)))
			return OUI;
	 }
	 return NON;
}
//
//
//
//    unsigned char cc=0,c=0;
//
//    
//    while (!fichier.eof()) {
//        cc=lettre(lire(fichier));
//        while (!fichier.eof() && cc!='s') {
//            c=lire(fichier);
//            cc=lettre(c);
//        }
//
//        int i=0;
//        if (cc=='s') {
//            char test1=OUI;
//            char test2=OUI;
//            while (!fichier.eof() && c!=':' ) {
//                if (test1==OUI && cc!=sem1[i])
//                    test1=NON;
//                if (test2==OUI &&  cc!=sem2[i])
//                    test2=NON;
//                if (test1==OUI || test2==OUI) {
//                    i++;
//                    c=lire(fichier);
//                    cc=lettre(c);
//                }
//                else 
//                    break;
//            }
//            if (c==':' && ((test1==OUI && i==8) || (test2==OUI && i==10)))
//                return OUI;
//        }
//    }
//    
//    return NON;
//}

unsigned char indexeur::LitCommentaires(ifstream& fichier,unsigned char cc,unsigned char* r) {
    unsigned char c=cc;

    if (c=='/') {
        c=lire(fichier);
        
        //C'est la lecture d'une ligne
        if (c=='/') {
            while (!fichier.eof() && c!='\n') c=lire(fichier);
        }
        else
            while (!fichier.eof() && c!='\\') c=lire(fichier);

        c=lire(fichier);
        while (!fichier.eof() && lespace(c)) c=lire(fichier);
        *r=c;
        return OUI;
    }

    *r=c;
    return NON;
}


void indexeur::LitCategorie(ifstream& fichier,char* categorie,char* traits,uchar& c) {
    
    unsigned char k;

    c=lire(fichier);
    
    while (!fichier.eof() && lespace(c)) c=lire(fichier);

    unsigned char cc=c;
    while (LitCommentaires(fichier,c,&cc)) {
        c=cc;
    }
    int i=0;
    categorie[0]=0;
    //Sequence d'echappement
    if (cc=='\\') {
        k=lire(fichier);    
        categorie[i++]=k;
        cc=lire(fichier);
    }

    k=lettre(cc);

    while (!fichier.eof() && k) {
        categorie[i] = k;
        c=lire(fichier);
        //Sequence d'echappement
        if (c=='\\') {
            k=lire(fichier);
            categorie[++i]=k;
            c=lire(fichier);
        }

        k=lettre(c);
        i++;
    }
    categorie[i]='\0';
    //Si des traits suivent
    i=1;
    strcpy_s(traits,1000,"[]");
    if (c=='[') {
        c=lire(fichier);
        while (c!=']') {
            traits[i++]=c;
            c=lire(fichier);
        }
        traits[i++]=']';
        traits[i]=0;
    }            
}
//-------------------------------------------------------------------------------------
void indexeur::LitUneCategorie(GlobalParseur* parseur,
                               ifstream& fichier,
                               char* categorie,
                               Traits** traits,uchar& c) {
    
    unsigned char k;

    c=lire(fichier);
    
    while (!fichier.eof() && lespace(c)) c=lire(fichier);

    unsigned char cc=c;
    while (LitCommentaires(fichier,c,&cc)) {
        c=cc;
    }
    int i=0;
    categorie[0]=0;
    //Sequence d'echappement
    if (cc=='\\') {
        k=lire(fichier);    
        categorie[i++]=k;
        cc=lire(fichier);
    }

    k=lettre(cc);

    while (!fichier.eof() && k) {
        categorie[i] = k;
        c=lire(fichier);
        //Sequence d'echappement
        if (c=='\\') {
            k=lire(fichier);
            categorie[++i]=k;
            c=lire(fichier);
        }

        k=lettre(c);
        i++;
    }
    categorie[i]='\0';
    //Si des traits suivent
    i=1;    
    if (c=='[') {
	parseur->LIGNES=lignes;
        *traits=parseur->BatitTraits(fichier,']');
	lignes=parseur->LIGNES;
    }
}

/****************************************************************************************************/
char indexeur::LireEntreCotes(ifstream& fichier,char* tampon,uchar& k,int& i) {
    if (k!='\"')
        return NON;

    k=lire(fichier);
    while (!fichier.eof() && k!='"' && i<100) {
        tampon[i++]=k;
        k=lire(fichier);
        if (k=='\\') {
            k=lire(fichier);
            tampon[++i]=k;
            k=lire(fichier);
        }
    }

    if (i>100 || fichier.eof()) {
        char erreur[100];
        sprintf_s(erreur,100,"Ligne:%d \nThe \" caracter should always be preceded by the escape caracter: \\",lignes);
        *flot_erreur<<erreur<<endl;
        lance_erreur(erreur);
    }

    k=lire(fichier);
    return OUI;
}
//--------------------------------------------------------------------------------------------------
//Lecture d'un index (jusqu'a un :)
int indexeur::LitIndex(GlobalParseur* parseur,
                       ifstream& fichier,                       
                       char* tampon,
                       char* categorie,
                       char* traits,
                       Traits** tr,
                       long& premier,
                       long& ligne,
                       char construitindex) {
    unsigned char c,k;

    c=lire(fichier);
    
    while (!fichier.eof() && lespace(c)) c=lire(fichier);

    while (LitCommentaires(fichier,c,&k)) {
        c=k;
    }

    //On enregistre le premier caractere
    premier=(long)fichier.tellg()-1;
    ligne=lignes;
    tampon[0]=0;
    int i=0;
    //Sequence d'echappement
    if (c=='\\') {
        k=lire(fichier);    
        tampon[i++]=k;
        c=lire(fichier);
    }

    LireEntreCotes(fichier,tampon,c,i);
    k=lettrebrute(c);

    while (!fichier.eof() && c !=':') {
        tampon[i] = k;
        c=lire(fichier);
        //Sequence d'echappement
        if (c=='\\') {
            k=lire(fichier);
            tampon[++i]=k;
            c=lire(fichier);
        }

        k=lettrebrute(c);
        i++;
    }
    tampon[i]='\0';
    c=lire(fichier);
    if (construitindex==OUI)
        LitCategorie(fichier,categorie,traits,c);
    else {
        *tr=NULL;
        LitUneCategorie(parseur,fichier,categorie,tr,c);
    }

    //Nous avons notre index
    //Ensuite, il nous faut lire la regle jusqu'au premier point final...  
    while (!fichier.eof() && c!='.') {
        if (!LitCommentaires(fichier,c,&c))
            c=lire(fichier);

        //s'il s'agit d'une chaine entre double quotes.
        if (c=='"') {
            c=lire(fichier);
            //On lit tous les caracteres entre
            while (c!='"') {
                if (c=='\\')
                    c=lire(fichier);
                c=lire(fichier);
            }
            c=lire(fichier);
        }
        
        if (c=='\\') {
            c=lire(fichier);
            c=lire(fichier);
        }        
    }

    if (tampon[0]==0 || fichier.eof())
        return -1;
    return OUI;
}

void indexeur::extrait_index(VECTA<char*>& fichiers) {
    char tampon[1000];
    long debut,ligne;
    int i;
    long compteur=0;
    char categorie[100];
    char traits[1000];

    findex<<"FILES:"<<endl;
    for (i=0;i<fichiers.dernier;i++)
        findex<<i<<"#"<<fichiers[i]<<endl;

    findex<<endl<<"INDEXES:"<<endl;
 
    for (i=0;i<fichiers.dernier;i++) {
        ifstream sem(fichiers[i],modeOuverture);
        raz();
        *flot_erreur<<endl<<"Indexing file: "<<fichiers[i]<<endl;
        if (recherche_champ(sem)==OUI) {
            while (LitIndex(NULL,sem,tampon,categorie,traits,NULL,debut,ligne,OUI)==OUI) {
                entreeindex ind(tampon,categorie,traits,debut,ligne,i); 
                findex<<ind.lemme<<" # "
                    <<ind.categorie<<" # "
                    <<ind.curseur<<" # "
                    <<ind.ligne<<" # "
                    <<ind.num_fichier<<endl;
                findex<<ind.traits<<endl;
                if ((compteur%1000) == 0) {
                    if ((compteur%10000)==0)
                        *flot_erreur<<compteur/1000;
                    *flot_erreur<<".";
                }
                compteur++;
            }            
        }
    }
    *flot_erreur<<"Done."<<endl<<endl;
}


void GlobalParseur::AjouteSem(auto_vocabulaire_semantique* vs) {
    if (Semantiques[laCoucheSemantique][couchesemantique]==NULL)
        Semantiques[laCoucheSemantique][couchesemantique]=new automate;
    etat* esem=Semantiques[laCoucheSemantique][couchesemantique]->parse((uchar*)vs->lemme,vs);
    if (esem!=NULL) {
        auto_vocabulaire_semantique* vvs=(auto_vocabulaire_semantique*)esem->valeur;
        if (vvs!=(void*)vs)
            vvs->ajoute(vs);
    }
    else 
        RenvoieErreurAutomate(Semantiques[laCoucheSemantique][couchesemantique]->erreur);    
}

void indexeur::indexer(GlobalParseur* parseur,char* nom_fichier) {
    char tampon[1000];
    long debut,ligne;
    long compteur=0;
    char categorie[100];
    Traits* traits=NULL;
    ifstream sem(nom_fichier,modeOuverture);
    int numfichier=parseur->fichiersSemantiques.dernier;
 
        
    if (recherche_champ(sem)==OUI) {                

        while (LitIndex(parseur,sem,tampon,categorie,NULL,&traits,debut,ligne,NON)==OUI) {
            Categorie* cat=parseur->ChercheCat(categorie);
            traits=parseur->FactoriseTraits(traits);
            auto_vocabulaire_semantique* vs=new auto_vocabulaire_semantique(tampon,
                cat->Categ(),debut,ligne,numfichier);
            vs->traits_mot=traits;            
            parseur->AjouteSem(vs);
            if ((compteur%1000) == 0) {
                if ((compteur%10000)==0)
                    *flot_erreur<<compteur/1000;
                *flot_erreur<<".";
            }
            compteur++;            
        }            
        sem.close();
        ifstream* f=new ifstream(nom_fichier,modeOuverture);
        parseur->fichiersSemantiquesNoms.ajoute(DupliqueChaine(nom_fichier));
        parseur->fichiersSemantiques.ajoute(f);        
    }
	else
		parseur_courant->erreur(parseur_courant->CHAINES[224]);    
}

void GlobalParseur::ChargeIndex(istream& fichier) {

    char tampon[1000];
    char categorie[100];
    char* findeligne;
    char mot[1000];
    char nom[1000];
	char path[1000];

	//On recupere le repertoire du fichier d'index
    char* last_slash= strrchr(NOM,SEP);
    
    if (last_slash==NULL)
        strcpy_s(path,1000,repertoire);
    else {
		char c=*++last_slash;
        *last_slash=0;
        strcpy_s(path,1000,NOM);
		*last_slash=c;
    }

    //Chaque ligne correspond a un nom de fichier...
    LitLigne(fichier,tampon);
    int num;
    //On enregistre le nombre de fichiers deja lu sous la forme d'index
    int baseFichier=fichiersSemantiques.dernier;
    while (xipsinput.Extract(tampon,"%d#%s",&findeligne,&num,mot)) {
        
        categorie[0]=0;
        NormaliseNomFichier(nom,mot,path);
        strcat_s(nom,1000,mot);
        ifstream* f= new ifstream(nom,modeOuverture);

        if (f->fail() ) {
            sprintf_s(tampon,1000,"%s %s (index)",CHAINES[97],nom);
            erreur(tampon);
        }

        //On ouvre tous ces fichiers a l'avance
        fichiersSemantiquesNoms.ajoute(DupliqueChaine(nom));
        fichiersSemantiques.ajoute(f);
        
        LitLigne(fichier,tampon);
    }
    
    if (strcmp(tampon,"INDEXES:"))
        erreur(CHAINES[224]);
    

    long curseur;
    long ligne;
    char unlong[100];
    long compteur=0;    

    *flot_erreur<<"Reading index file..."<<endl;
    while (!fichier.eof()) {
        LitLigne(fichier,tampon);
        if (tampon[0]) {
            if (!xipsinput.Extract(tampon,"%s # %s # %s # %d # %d",
                                   &findeligne,mot,categorie,unlong,&ligne,&num))
                erreur(CHAINES[226]);
        }
        else
            break;
        curseur=atol(unlong);
        //L'indexation des fichiers peut dependre du nombre de fichiers d'index deja lu
        num+=baseFichier;        
        Categorie* cat=ChercheCat(categorie);
        //Lecture des traits;
        char k=lit(fichier,tampon);
        if (k!='[')
            erreur(CHAINES[226]);
        k=lit(fichier,tampon);
        Traits* tr=NULL;
        if (k!=']' || tampon[0]!=0) {
            MetEnAttenteLecture(k,tampon);    
            tr=FactoriseTraits(BatitTraits(fichier,']'));
        }
        auto_vocabulaire_semantique* vs=new auto_vocabulaire_semantique(mot,cat->Categ(),curseur,ligne,num);
        vs->traits_mot=tr;
        if ((compteur%1000) == 0) {
            if ((compteur%10000)==0)
                *flot_erreur<<compteur/1000;
            *flot_erreur<<".";
        }

        compteur++;        

        if (Semantiques[laCoucheSemantique][couchesemantique]==NULL)
            Semantiques[laCoucheSemantique][couchesemantique]=new automate;
        etat* esem=Semantiques[laCoucheSemantique][couchesemantique]->parse((uchar*)mot,vs);
        if (esem!=NULL) {
            auto_vocabulaire_semantique* vvs=(auto_vocabulaire_semantique*)esem->valeur;
            if (vvs!=(void*)vs)
                vvs->ajoute(vs);
        }
        else 
            RenvoieErreurAutomate(Semantiques[laCoucheSemantique][couchesemantique]->erreur);                
    }
    *flot_erreur<<endl;
}
        
void GlobalParseur::ChargeSem(auto_vocabulaire_semantique* vs,char positionner) {

    if (vs->charge==OUI) {
        vs->compteur=memoiresem.max;
        return;
    }


    char lecryptage=cryptage;
    cryptage=NON;
	char k, tampon[XIPBUFFERSIZE];
	char mot[XIPBUFFERSIZE];
	char categ[XIPBUFFERSIZE];
    Categorie* c;
    Traits* mot_tr;


    //On utilise le fichier correspondant
    ifstream& fichier=*fichiersSemantiques[vs->num_fichier];
    //et on se position a l'endroit adequat
    if (positionner==OUI) {
        fichier.seekg(vs->curseur);
        LIGNES=vs->ligne;
        NOM=fichiersSemantiquesNoms[vs->num_fichier];
    }

    char ancienneTrace = TRACEREGLE;
    if (CalculFonction==2) {
        if (Trace==NULL) {
            Trace = new ofstream(Nom_Trace);
            *Trace<<CHAINES[0]<<":"<<Endl;
        }
        TRACEREGLE=2;
    }
    
    k=LitEgal(fichier,(unsigned char*)mot);
    mot_tr=NULL;
    if (strcmp(mot,vs->lemme)) {
        *flot_erreur<<"Index:"<<vs->num_fichier<<" is invalid (you should rebuild it)"<<endl;
        cryptage=lecryptage;
        return;    
    }

    if (k==':' && mot[0]==0) {
        cryptage=lecryptage;
        return;
    }

    if (k=='-') {
        k=lit(fichier,tampon);  
        if (k!='>') {
            cryptage=lecryptage;
            return;
        }
    }

    c=CatVide;
    //nous sommes dans le cadre des deux regles du dessus
    //on lit d'abord une categorie
    k=lit(fichier,categ);
    
    c=ChercheCat(categ);
    if (c->Categ() != vs->X)  {
        cryptage=lecryptage;
        return;
    }
    
    if (k=='[')
        mot_tr = BatitTraitsPlus(fichier,c,&k,tampon);

    if (k!='=') {
        if (mot_tr!=NULL)
            delete mot_tr;
        cryptage=lecryptage;
        return;
    }

    //*flot_erreur<<"Loading:"<<mot<<endl;

    k=lit(fichier,tampon);
    //si une regle de dependence est attachee
    VectTestFonctionSemantique* vtf=new VectTestFonctionSemantique;
    vtf->numero=numero_regle_semantique;
    Dependance* depend=NULL;
    static VECTA<char> vari(5);
    vari.raz();
    //La variable 0 est une variable par defaut qui correspond 
    //au mot que l'on cherche a desambiguiser
    vari.affecte(0,OUI);
    memoiresem.indicateur=OUI;
    if (!strcmp(tampon,"if")) {
		
        TestArbre* tarbre=new TestArbre;
        k=' ';
        LectureTest(fichier,k,tampon,NON,vari,&tarbre);

        tarbre->direct=tarbre->LineariseDirect(this,vtf->clef,vtf->listes);
		TestArbre* boucle=TestArbre::r_dernier[iParseurCourant];
		while (boucle!=tarbre) {
			TestArbre* prec=boucle->_precedent;
			delete boucle;
			boucle=prec;
		}
		delete tarbre;
    }
    

    char boucle=OUI;
    while (boucle) {
        depend=ConstruitDependance(fichier,vari,k,tampon,OUI);            
        vtf->dependances.ajoute(depend);
        k=lit(fichier,tampon);
        if (k==',')
            k=lit(fichier,tampon);
        else 
            boucle=NON;
    }
    
    vs->semantique=vtf;
    vs->traits_mot=FactoriseTraits(mot_tr);
    vs->X = &c->X;
    vs->charge=OUI;
    vs->compteur=memoiresem.max;
    
    //Pour le moment on vire toutes les phrases precedentes
    if (memoiresem.memoire.dernier>=MAXVIRTUEL) {        
        int imem;        
        for (imem=0;imem<memoiresem.memoire.dernier;imem++) {
            auto_vocabulaire_semantique* av=memoiresem.memoire[imem];        
            if (av->compteur<=memoiresem.max-1) {
                memoiresem.memoire.retireElement(imem);                                    
                av->semantique->Destruction(this);
                if (av->semantique!=NULL)
                    delete av->semantique;
                av->semantique=NULL;
                av->charge=NON;
                av->compteur=-1;
                av->traits_mot=NULL;
                imem--;
            }
        }
    }

    memoiresem.memoire.ajoute(vs);
    vs->semantique->aDetruire.ajoute(memoiresem.nouveauxElementsDeRegles);
    vs->semantique->aDetruireFonc.ajoute(memoiresem.nouveauxTests);
    memoiresem.nouveauxElementsDeRegles.raz();
    memoiresem.nouveauxTests.raz();
    memoiresem.indicateur=NON;
    cryptage=lecryptage;

    if (CalculFonction==2) {
        *Trace<<Endl<<Endl;
        TRACEREGLE=ancienneTrace;
    }

    if (k!='.') 
        erreur(CHAINES[132]);
}

        
void GlobalParseur::ChargementSemantique(istream& fichier,char* nom) {


	char k, tampon[XIPBUFFERSIZE];
	char mot[XIPBUFFERSIZE];
	char categ[XIPBUFFERSIZE];
    Categorie* c;
    Traits* mot_tr;

    ostream* os=&cout;
    if (OS!=NULL)
        os=OS;

    

    k=0;
    while (k!=':') {
        k=LitEgal(fichier,(unsigned char*)mot);
        mot_tr=NULL;
        
        if (k==':' && mot[0]==0)
            return;
        
        if (k=='-') {
            k=lit(fichier,tampon);  
            if (k!='>') {
                erreur("MISSING -> in a semantic file");               
            }
        }
        *os<<mot<<endl;
        c=CatVide;
        //nous sommes dans le cadre des deux regles du dessus
        //on lit d'abord une categorie
        k=lit(fichier,categ);
        
        c=ChercheCat(categ);
        
        if (k=='[')
            mot_tr = BatitTraitsPlus(fichier,c,&k,tampon);
        
        if (k!='=') {
            if (mot_tr!=NULL)
                delete mot_tr;
            erreur("MISSING = in a semantic file");
        }
        
        //*flot_erreur<<"Loading:"<<mot<<endl;
        
        k=lit(fichier,tampon);
        //si une regle de dependence est attachee
        VectTestFonctionSemantique* vtf=new VectTestFonctionSemantique;
        vtf->numero=numero_regle_semantique;
        Dependance* depend=NULL;
        static VECTA<char> vari(5);
        vari.raz();
        //La variable 0 est une variable par defaut qui correspond 
        //au mot que l'on cherche a desambiguiser
        vari.affecte(0,OUI);
        if (!strcmp(tampon,"if")) {
            
            TestArbre* tarbre=new TestArbre;
            k=' ';
            LectureTest(fichier,k,tampon,NON,vari,&tarbre);
            
            tarbre->direct=tarbre->LineariseDirect(this,vtf->clef,vtf->listes);
            delete tarbre;
        }
        
        
        char boucle=OUI;
        while (boucle) {
            depend=ConstruitDependance(fichier,vari,k,tampon,OUI);            
            vtf->dependances.ajoute(depend);
            k=lit(fichier,tampon);
            if (k==',')
                k=lit(fichier,tampon);
            else 
                boucle=NON;
        }
                
        if (k!='.') 
            erreur(CHAINES[132]);
    }
}



