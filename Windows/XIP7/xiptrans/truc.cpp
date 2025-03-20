/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003, Xerox Corporation, XRCE-Grenoble.
* This file can only be used with the XIP library, 
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : XIP
Version    : 3.16
filename   : lecttest.cxx
Date       : 10/01/1999
Purpose    : Compiling and parsing of a test after a "if"
Programmer : Claude ROUX
Reviewer   : 
*/

#include "parsbin.h"

char rechercheNomFonction(char* nom);
/*-------------------------------------------------------------------------*/
Traits* GlobalParseur::BatitTraits(ifstream& fichier,char finCaractere,VECTA<char*>* pile,char* clearStack) {
    Traits* resultat=new Traits(this); 
    Traits* rr; 
    trait* vattribut; 
    char k=' ',tampon[1000],test,affecter,option,libre;
    
    //par defaut finCaractere==']'
    while (k!=finCaractere) {
        k=lit(fichier,tampon);
        if (k=='(') {
            option=OUI;
            k=lit(fichier,tampon);
        } 
        else
            option=NON;
        
        if (!strcmp(tampon,"$stack")) {
            if (k!='=' && k!=':') {
                if (clearStack==NULL)
                    erreur(CHAINES[135]);
                else
                    *clearStack=STACKNONVIDE;
                continue;
            }
            char operateur=k;
            k=lit(fichier,tampon);
            if (pile!=NULL) {
                char neg=NON;
                if (k=='~') {
                    k=lit(fichier,tampon);
                    if (tampon[0]==0) {
                        if (clearStack!=NULL)
                            *clearStack=OUI;
                        continue;
                    }
                    neg=OUI;
                }
                
                if (tampon[0]=='-' && tampon[1] >= '1' && tampon[1]<='9') {
                    if (clearStack!=NULL)
                        *clearStack=48-tampon[1];
                }
                else {
                    char suite[1000];
                    suite[0]=0;
                    if (neg==OUI) {
                        suite[0]='~';
                        suite[1]=0;
                    }
                    strcat(suite,tampon);   
                    pile->ajoute(DupliqueChaine(suite));
                }
            }
            continue;
        }
        
        if (k=='#') {
            if (!Abreviation) {
                delete resultat;
                erreur(CHAINES[61]);
            }
            
            k=lit(fichier,tampon);
            rr=Abreviation->recherche(this,tampon);
            
            if (rr==NULL) {
                delete resultat;
                erreur(CHAINES[134]);
            }
            resultat->ou(rr);
            if (k!=',' && k!=finCaractere) {
                delete resultat;
                erreur(CHAINES[134]);
            }
        }
        else {
            test=2;
            libre=NON;
            if (k=='!') {
                test=3;
                libre=OUI;
                k=lit(fichier,tampon);
            }
            
            //Cas particulier, nous nous interessons au seul filtre
            //seul cas d'utilisation dans la comparaison entre noeud
            //dans les regles d'extraction: #1[genre]:#2[genre]
            if (k==',' || k==finCaractere) {
                if (LEMME!= NULL && !strcmp(tampon,LEMME)) {
                    if (resultat->lemme==NULL)
                        resultat->lemme=new automate("\\*",0);
                    else {
                        delete resultat;
                        erreur(CHAINES[136]);
                    }
                    continue;
                }
                
                if (SURFACE!= NULL && !strcmp(tampon,SURFACE)) {
                    if (resultat->surface==NULL)
                        resultat->surface=new automate("\\*",0);
                    else {
                        delete resultat;
                        erreur(CHAINES[136]);
                    }
                    continue;
                }
                
                //recherche du nom de l'attribut
                vattribut=ArbTraits->recherche(tampon);
                if (!vattribut) {
                    delete resultat;
                    erreur(Concatene(tampon,CHAINES[124]));
                }
                
                //Si vattribut->type==LISTE, il s'agit d'une liste d'attribut
                if (vattribut->type==LISTE) {
                    if (resultat->lpresent)
                        erreur(CHAINES[45]);
                    if (!resultat->lpresent) 
                        resultat->lpresent=new Vecteur(iTailleVect);
                    (resultat->lpresent)->ou(vattribut->valeur);                
                }
                else {
                    if (!resultat->present) 
                        resultat->present=new Vecteur(iTailleVect);
                    (resultat->present)->ou(vattribut->valeur);                
                }
                continue;
            }
            
            if (k!=':' && k!='=') {
                delete resultat;
                erreur(CHAINES[135]);
            }
            
            //Deux possibilites: attribut:valeur ou attribut=valeur
            affecter=NON;
            if (k=='=') 
                affecter=OUI; 
            
            if (LEMME!= NULL && !strcmp(tampon,LEMME)) {                
                k=litSans(fichier,tampon);  
                if (k==NEGATION) {
                    if (affecter==OUI)
                        erreur(CHAINES[274]);
                    resultat->maniplemme=NEG;
                    k=litSans(fichier,tampon);
                }
                if (resultat->lemme==NULL)
                    resultat->lemme=new automate(tampon,0);
                else {
                    delete resultat;
                    erreur(CHAINES[136]);
                }
                
                if (affecter==OUI)
                    resultat->maniplemme=ECRASE;
                
                continue;
            }
            
            
            if (SURFACE!= NULL && !strcmp(tampon,SURFACE)) {
                k=litSans(fichier,tampon);      
                if (k==NEGATION) {
                    if (affecter==OUI)
                        erreur(CHAINES[274]);
                    resultat->manipsurface=NEG;
                    k=litSans(fichier,tampon);
                }

                if (resultat->surface==NULL)
                    resultat->surface=new automate(tampon,0);
                else {
                    delete resultat;
                    erreur(CHAINES[136]);
                }
                
                if (affecter==OUI)
                    resultat->manipsurface=ECRASE;
                
                continue;
            }
            
            //recherche du nom de l'attribut
            vattribut=ArbTraits->recherche(tampon);
            if (!vattribut) {
                delete resultat;
                erreur(Concatene(tampon,CHAINES[124]));
            }
            
            //lecture de la valeur du trait: les caracteres suivants
            //une negation: valeur:~
            //une virgule ou un !
            k=lit(fichier,tampon);
            if (k!=',' && k!=finCaractere && k!=NEGATION && k!='!' && k!='#') {
                delete resultat;
                erreur(CHAINES[134]);
            }
            
            
            //le caractere suivant est un !
            //  !trait:! liberation de trait
            //  trait:! bloquage du trait
            if (k=='!') {
                if (affecter) {
                    delete resultat;
                    erreur(CHAINES[63]);
                }
                if (libre) {
                    if (!resultat->libre) 
                        resultat->libre=new Vecteur(iTailleVect);
                    (resultat->libre)->ou(vattribut->valeur);
                }
                else {
                    if (!resultat->bloquage) 
                        resultat->bloquage=new Vecteur(iTailleVect);
                    (resultat->bloquage)->ou(vattribut->valeur);
                }
                k=lit(fichier,tampon);
            }
            else {
                //Test sur la presence d'un trait
                if (k=='#') {
                    (resultat->valeur)->ou(vattribut->valeur);
                    k=lit(fichier,tampon);
                    continue;
                }
                
                //Test sur le trait universel: trait:?
                if (k!=NEGATION && (!affecter || tampon[0]=='?')) {
                    if (vattribut->type==LISTE) {
                        if (resultat->lpresent)
                            erreur(CHAINES[45]);
                        if (!resultat->lpresent) 
                            resultat->lpresent=new Vecteur(iTailleVect);
                        (resultat->lpresent)->ou(vattribut->valeur);                
                    }
                    else {
                        if (!resultat->present) 
                            resultat->present=new Vecteur(iTailleVect);
                        (resultat->present)->ou(vattribut->valeur);
                    }
                    if (tampon[0]=='?') 
                        continue;
                }
                
                //Cas d'une negation trouvee
                if (k==NEGATION) {
                    k=lit(fichier,tampon);
                    //Premier cas, si le signe egal etait mentionne, 
                    //c'est un effacement: attribut=NEGATION.
                    //Tous les traits sont effaces...
                    if (affecter) {
                        if (!resultat->efface)
                            resultat->efface=new Vecteur(iTailleVect);
                        test=0;
                    }
                    else {
                        if (!resultat->negation)
                            resultat->negation=new Vecteur(iTailleVect);
                        if (tampon[0]!='\0') 
                            test=1; 
                        else 
                            test=0;
                    }
                }
                
                if (test) {
                    if (affecter && !strcmp(tampon,CHAINES[65])) {
                        if (!RechercheDefaut(vattribut->valeur)) {
                            delete resultat;
                            erreur(CHAINES[66]);
                        }
                        
                        if (!resultat->filtreDefaut)
                            resultat->filtreDefaut=new Vecteur(iTailleVect);
                        (resultat->filtreDefaut)->ou(vattribut->valeur); 
                    }
                    else {                        
                        int indice = RetrouveTrait(vattribut,tampon,OUI);
                        
                        if ((resultat->filtre)->contient(vattribut->valeur)) {
                            delete resultat;
                            erreur(CHAINES[68]);
                        }
                        if (test>=2) {
                            resultat->valeur->un(this,indice);
                            
                            if (libre) {
                                if (!resultat->libre) 
                                    resultat->libre=new Vecteur(iTailleVect);
                                (resultat->libre)->ou(vattribut->valeur);
                            }
                            (resultat->filtre)->ou(vattribut->valeur);
                        }
                        else {
                            resultat->negation->un(this,indice);
                        }
                    }
                }
                else {
                    //dans le cas de la regle: attribut=NEGATION
                    if (affecter)
                        (resultat->efface)->ou(vattribut->valeur);
                    else
                        (resultat->negation)->ou(vattribut->valeur);
                }
            }
        }
    }
    return resultat;
}

/*-------------------------------------------------------------------------*/
SousArbre* GlobalParseur::ConstruitNoeud(ifstream& fichier,
                                         VECTA<SousArbre*>& nettoyage,
                                         char* tampon,
                                         char& k,
                                         typeRegle regle,
                                         char negation,
                                         char& optionnel,
                                         char& modification) {
    
    char neg=negation;    
    char desamb=NON;
    int iter=0;
    int num_arg=-1;
    Traits* trcourant=NULL;
    SousArbre* element=NULL;
    char enProfondeur=NON;
    Categorie* c;
    char suite[1000];
    Traits* selection=NULL;
    Traits* selectiontous=NULL;
    
    optionnel=NON;
    char relire=NON;
    
    
    if (k==NEGATION) {
        if (tampon[0]==0) {
            //Negation
            neg=1-negation;
            k=lit(fichier,tampon);
            modification=OUI;
        }
        else
            relire=OUI;
    }
    
    //Il s'agit d'un caractere de controle
    //on repart
    if (tampon[0]==0 && k!='#')
        return NULL;
    
    if (k=='^') {
        enProfondeur=OUI;
        k=lit(fichier,suite);
        modification=OUI;
    }
    
    int l=strlen(tampon);
    desamb=NON;
    int pos=-1;
    for (int i=0;i<l;i++) {
        if (tampon[i]=='*' || tampon[i]=='+' || tampon[i]=='-') {
            if (pos==-1)
                pos=i;
            optionnel=tampon[i];
        }
        else
            if (tampon[i]=='%') {
                if (pos==-1)
                    pos=i;
                desamb=OUI;
            }
    }    
    
    //     //Lecture du nombre maximal d'iteration d'une structure * ou +
    //     if (k==34 && optionnel!=NON) { //"12"
    //         k=lit(fichier,suite);
    //         modification=OUI;
    //         //Normalement, c'est un chiffre
    //         iter=atoi(suite);
    //         if (k!=34) 
    //             erreur(CHAINES[166]);
    //         k=lit(fichier,suite);
    //     }
    
    if (pos!=-1)
        tampon[pos]=0;
    
    if (k=='#') {
        //nous lisons le numero de l'argument...
        k=lit(fichier,suite);
        modification=OUI;
        //Une variable
        num_arg=atoi(suite);
    }
    
    if (k=='<') {
        k=litBis(fichier,suite);
        modification=OUI;
        //S'il s'agit d'un selectiontous
        if (k=='*') {
            selectiontous=BatitTraits(fichier,'>');
            k=lit(fichier,suite);
            if (k=='<') {
                selection=BatitTraits(fichier,'>');
                k=lit(fichier,suite);
            }
        }                
        else{
            MetEnAttenteLecture(k,suite);
            selection=BatitTraits(fichier,'>');            
            k=lit(fichier,suite);
        }
    }
    
    
    if (k=='[') {
        modification=OUI;
        trcourant=BatitTraits(fichier);            
        k=lit(fichier,suite);
    }
    
    if (!strchr(",.;|(){}=~",k)) {
        sprintf(suite,"%s %c",CHAINES[36],k);
        erreur(suite);
    }
    
    c=CatVide;
    if (tampon[0])
        c=ChercheCat(tampon);        
    
    if (regle==REGLES_RECONSTRUCTION_MODELE && c!=CatVide) {
        //Il s'agit d'une modification d'un noeud
        //dans le cadre d'une regle de reconstruction
        //pour lui donner une nouvelle categorie
        if (trcourant==NULL)
            trcourant=new Traits(c->traits);
        else
            trcourant->ou(c->traits);
    }
    
    trcourant=FactoriseTraits(trcourant);

    //si une queue de regle a deja ete calcule, il n'est pas besoin d'en
    //generer une nouvelle version
    element=new SousArbre(&c->X,trcourant,num_arg);
    nettoyage.ajoute(element);
        
    element->selection=FactoriseTraits(selection);
    element->selectiontous=FactoriseTraits(selectiontous);
    element->negation=neg;
    
    //dans le cas ou il s'agit d'une exploration en profondeur non bornee
    //on conserve le pere du sous-arbre dans le champ suivantbis...
    //if (sous!=NULL && sous->profondeurNonBornee==OUI)
    //  sous->suivantbis=element;
    
    element->profondeurNonBornee=enProfondeur;
    
    element->desamb=desamb;
    //Borne bloque le nombre d'iteration sur un objet de type: Cat* ou Cat+
    element->borne=iter;
    //On reinitialise le tampon, pour d'autres lectures
    tampon[0]=0;
    if (relire==OUI) {
        k=lit(fichier,tampon);
        modification=OUI;
    }
    
    return element;
}

ArcArbre* GlobalParseur::ConstruitArc(ifstream& fichier,
                                         char* tampon,
                                         char& k,
                                         typeRegle regle,
                                         char negation,
                                         char& optionnel,
                                         char& modification,
                                         EtatArbre* lesuivant) {
    
    char neg=negation;    
    char desamb=NON;
    int iter=0;
    int num_arg=-1;
    Traits* trcourant=NULL;
    ArcArbre* element=NULL;
    char enProfondeur=NON;
    Categorie* c;
    char suite[1000];
    Traits* selection=NULL;
    Traits* selectiontous=NULL;
    
    optionnel=NON;
    char relire=NON;
    
    
    if (k==NEGATION) {
        if (tampon[0]==0) {
            //Negation
            neg=1-negation;
            k=lit(fichier,tampon);
            modification=OUI;
        }
        else
            relire=OUI;
    }
    
    //Il s'agit d'un caractere de controle
    //on repart
    if (tampon[0]==0 && k!='#')
        return NULL;
    
    if (k=='^') {
        enProfondeur=OUI;
        k=lit(fichier,suite);
        modification=OUI;
    }
    
    int l=strlen(tampon);
    desamb=NON;
    int pos=-1;
    for (int i=0;i<l;i++) {
        if (tampon[i]=='*' || tampon[i]=='+' || tampon[i]=='-') {
            if (pos==-1)
                pos=i;
            optionnel=tampon[i];
        }
        else
            if (tampon[i]=='%') {
                if (pos==-1)
                    pos=i;
                desamb=OUI;
            }
    }    
    
    //     //Lecture du nombre maximal d'iteration d'une structure * ou +
    //     if (k==34 && optionnel!=NON) { //"12"
    //         k=lit(fichier,suite);
    //         modification=OUI;
    //         //Normalement, c'est un chiffre
    //         iter=atoi(suite);
    //         if (k!=34) 
    //             erreur(CHAINES[166]);
    //         k=lit(fichier,suite);
    //     }
    
    if (pos!=-1)
        tampon[pos]=0;
    
    if (k=='#') {
        //nous lisons le numero de l'argument...
        k=lit(fichier,suite);
        modification=OUI;
        //Une variable
        num_arg=atoi(suite);
    }
    
    if (k=='<') {
        k=litBis(fichier,suite);
        modification=OUI;
        //S'il s'agit d'un selectiontous
        if (k=='*') {
            selectiontous=BatitTraits(fichier,'>');
            k=lit(fichier,suite);
            if (k=='<') {
                selection=BatitTraits(fichier,'>');
                k=lit(fichier,suite);
            }
        }                
        else{
            MetEnAttenteLecture(k,suite);
            selection=BatitTraits(fichier,'>');            
            k=lit(fichier,suite);
        }
    }
    
    
    if (k=='[') {
        modification=OUI;
        trcourant=BatitTraits(fichier);            
        k=lit(fichier,suite);
    }
    
    if (!strchr(",.;|(){}=~",k)) {
        sprintf(suite,"%s %c",CHAINES[36],k);
        erreur(suite);
    }
    
    c=CatVide;
    if (tampon[0])
        c=ChercheCat(tampon);        
    
    if (regle==REGLES_RECONSTRUCTION_MODELE && c!=CatVide) {
        //Il s'agit d'une modification d'un noeud
        //dans le cadre d'une regle de reconstruction
        //pour lui donner une nouvelle categorie
        if (trcourant==NULL)
            trcourant=new Traits(c->traits);
        else
            trcourant->ou(c->traits);
    }
    
    trcourant=FactoriseTraits(trcourant);

    //si une queue de regle a deja ete calcule, il n'est pas besoin d'en
    //generer une nouvelle version
    element=listeEtatArbre.allouearc(&c->X,trcourant,num_arg);
    element->etatarbre=lesuivant;
        
    element->selection=FactoriseTraits(selection);
    element->selectiontous=FactoriseTraits(selectiontous);
    element->negation=neg;
    
    //dans le cas ou il s'agit d'une exploration en profondeur non bornee
    //on conserve le pere du sous-arbre dans le champ suivantbis...
    //if (sous!=NULL && sous->profondeurNonBornee==OUI)
    //  sous->suivantbis=element;
    
    element->profondeurNonBornee=enProfondeur;    
    element->desamb=desamb;
    //On reinitialise le tampon, pour d'autres lectures
    tampon[0]=0;
    if (relire==OUI) {
        k=lit(fichier,tampon);
        modification=OUI;
    }
    
    return element;
}


void TraitementArgument(GlobalParseur* parseur,SousArbre* element,typeRegle regle,SeqFonc* fonc) {
    int num_arg=element->argument;
    int arg; 
    Fonction* fFonc=NULL;
    
    if (num_arg==-1)
        return;
    
    
    if (fonc->type()!=REGLES_SEQUENCE)
        fFonc=(Fonction*)fonc;
    
    //Nous avons un argument lie avec une variable de type #num.
    
    arg=-1;
    
    //Nous recherchons la position de cet argument dans la
    //liste deja construite
    //de la fonction syntaxique. 
    //si l'argument est absent
    for (int i=0;i<fonc->nb_args;i++)
        if (fonc->arg[i]==(size_t)num_arg) {
            arg=i;
            if (fFonc!=NULL)                            
                fFonc->marque.affecte(i,1);
            break;
        }
        
        if (arg==-1) {
            //nous n'avons pas trouve cet argument, dans ce cas,
            //il ne s'agit pas d'une erreur
            //mais simplement d'un argument supplementaire que l'on 
            //enregistre a la fin de la liste
            //des arguments de la fonction... Une marque speciale est 
            //enregistree
            //pour s'en souvenir
            //les valeurs sont pour marque: 
            // 1 argument normal
            // 2 argument supplementaire a ne pas afficher...
            if (regle==VIDE || regle==REGLES_DEDUCTION || regle==REGLES_EXTRACTION) {
                if (regle!=REGLES_DEDUCTION)
                    arg=fonc->nb_args;
                else
                    arg=num_arg;
                if (fFonc!=NULL)
                    fFonc->marque.affecte(arg,2);
                fonc->arg.affecte(arg,num_arg); 
                //on initialise la valeur avec num_arg...
                ++fonc->nb_args; //on incremente le nombre d'argument...
            }
            //dans le cas des regles de simplification, on rajoute quelques contraintes:
            //a) la valeur #1 est par defaut la valeur du noeud que l'on simplifie, elle ne
            //peut donc etre utilise pour marquer un element du contexte.
            //b) la variable associee n'a aucun lien avec des arguments inexistants d'une fonction
            //il s'agit forcement d'un contexte dans le cas d'une regle de simplification
            else {
                arg=num_arg;
                if (regle==REGLES_RECONSTRUCTION || 
                    regle==REGLES_RECONSTRUCTION_MODELE &&
                    fFonc!=NULL)
                    fFonc->marque.affecte(arg,OUI);
                if (regle==REGLES_SIMPLIFICATION && arg==0)
                    parseur->erreur(parseur->CHAINES[104]);
            }                        
        }
        
        element->argument=arg;
}


void TraitementArcArgument(GlobalParseur* parseur,ArcArbre* element,typeRegle regle,SeqFonc* fonc) {
    int num_arg=element->argument;
    int arg; 
    Fonction* fFonc=NULL;
    
    if (num_arg==-1)
        return;
    
    
    if (fonc->type()!=REGLES_SEQUENCE)
        fFonc=(Fonction*)fonc;
    
    //Nous avons un argument lie avec une variable de type #num.
    
    arg=-1;
    
    //Nous recherchons la position de cet argument dans la
    //liste deja construite
    //de la fonction syntaxique. 
    //si l'argument est absent
    for (int i=0;i<fonc->nb_args;i++)
        if (fonc->arg[i]==(size_t)num_arg) {
            arg=i;
            if (fFonc!=NULL)                            
                fFonc->marque.affecte(i,1);
            break;
        }
        
        if (arg==-1) {
            //nous n'avons pas trouve cet argument, dans ce cas,
            //il ne s'agit pas d'une erreur
            //mais simplement d'un argument supplementaire que l'on 
            //enregistre a la fin de la liste
            //des arguments de la fonction... Une marque speciale est 
            //enregistree
            //pour s'en souvenir
            //les valeurs sont pour marque: 
            // 1 argument normal
            // 2 argument supplementaire a ne pas afficher...
            if (regle==VIDE || regle==REGLES_DEDUCTION || regle==REGLES_EXTRACTION) {
                if (regle!=REGLES_DEDUCTION)
                    arg=fonc->nb_args;
                else
                    arg=num_arg;
                if (fFonc!=NULL)
                    fFonc->marque.affecte(arg,2);
                fonc->arg.affecte(arg,num_arg); 
                //on initialise la valeur avec num_arg...
                ++fonc->nb_args; //on incremente le nombre d'argument...
            }
            //dans le cas des regles de simplification, on rajoute quelques contraintes:
            //a) la valeur #1 est par defaut la valeur du noeud que l'on simplifie, elle ne
            //peut donc etre utilise pour marquer un element du contexte.
            //b) la variable associee n'a aucun lien avec des arguments inexistants d'une fonction
            //il s'agit forcement d'un contexte dans le cas d'une regle de simplification
            else {
                arg=num_arg;
                if (regle==REGLES_RECONSTRUCTION || 
                    regle==REGLES_RECONSTRUCTION_MODELE &&
                    fFonc!=NULL)
                    fFonc->marque.affecte(arg,OUI);
                if (regle==REGLES_SIMPLIFICATION && arg==0)
                    parseur->erreur(parseur->CHAINES[104]);
            }                        
        }
        
        element->argument=arg;
}


void SousArbre::ConstruitClef(GlobalParseur* pars,typeRegle regle) {
    SousArbre* f=fils;
    clef=0;    
    
    //Simplification
    while (f!=NULL) {
        if (f->profondeurNonBornee==NON && f->negation==NON && f->X!=NULL && f->X->val!=CATFUSION)
            clef|=(f->X)->Val;
        f=f->suivant;
    }
    f=this;
    int nb=0;
    while (f!=NULL) {
        if (f->saute==NON)
            nombre++;
        f=f->suivant;
    }
    
    f=this;
    while (f!=NULL) {
        f->nombre=nombre;
        if (f->saute==NON)
            nombre--;
        f=f->suivant;
    }
}

void ConstruitExtremite(SousArbre* element,VECTA<SousArbre*>& pile,char premier) {
    SousArbre* courant=element;
    int i=0;
    
    if (premier==OUI || element->freres.dernier==0) {
        if (element->suivant==NULL)
            pile.ajoute(element);
    }
    else
        pile.ajoute(element->freres.fin());
    
    element->marquage=OUI;
    courant=element;
    
    if (element->reboucle>=0) //dans ce cas, le premier element est celui de la boucle, on le saute
        i=1;
    
    for (;i<courant->derivations.dernier;i++) {
        if (courant->derivations[i]->marquage==NON) {
            courant->derivations[i]->marquage=OUI;
            ConstruitExtremite(courant->derivations[i],pile,NON);
            courant->derivations[i]->marquage=NON;
        }
    }
    
    element->marquage=NON;
}

char EgauxSousArbre(GlobalParseur* pars,SousArbre* ar1, SousArbre* ar2) {
    
    if (ar1->derivations.dernier || ar2->derivations.dernier)
        return NON;
    
    if (ar1->argument!=ar2->argument)
        return NON;
    
    if (ar1->X!=ar2->X)
        return NON;
    
    if (ar1->traits!=ar2->traits)
        return NON;
    
    if (ar1->fils==NULL || ar2->fils==NULL) {
        if (ar1->fils!=ar2->fils)
            return NON;
    }
    else
        if (!EgauxSousArbre(pars,ar1->fils,ar2->fils))
            return NON;    
        return OUI;
}


void NettoieNettoyage(VECTA<SousArbre*>& nettoyage, VECTA<SousArbre*>& vn) {
    
    for (int net=0;net<nettoyage.dernier;net++) {
        for (int ivn=0;ivn<vn.dernier;ivn++) {
            if (nettoyage[net]==vn[ivn]) {
                delete nettoyage.retireElement(net);
                vn.affecte(ivn,NULL);
                net--;
                break;
            }
        }
    }    
}

char FusionneNoeud(GlobalParseur* parseur,SousArbre* cle,VECTA<SousArbre*>& vn) {
    if (cle->derivations.dernier<=1 || cle->saute==OUI)
        return NON;
    
    char detectee=OUI;
    Traits* tr=cle->derivations[0]->traits;
    TVecteur v=0;
    SousArbre* ar;
    SousArbre* lefils=NULL;
    int num_arg=cle->derivations[0]->argument;
    
    for (int idv=0;idv<cle->derivations.dernier;idv++) {
        ar=cle->derivations[idv];
        
        if (ar->X==parseur->XCatVide ||
            ar->X==NULL ||
            ar->fils!=NULL ||
            ar->argument!=num_arg ||
            ar->selectiontous!=NULL ||
            ar->selection!=NULL ||
            ar->derivations.dernier)
            continue;
        
        if (tr!=ar->traits)
            continue;
        
        vn.ajoute(ar);
        v|=ar->X->Val;
    }
    
    if (vn.dernier<=1)
        return NON;
    
    //On fusionne toutes les categories en un seul noeud
    //On cree un noeud X particulier
    Cat* X= new Cat;
    //Indication d'un noeud fusion
    X->val=CATFUSION;
    X->index=CATFUSION;
    X->cacher=NON;
    X->Val=v;
    X->nom=DupliqueChaine((char*)"%");
    
    if (cle->derivations.dernier==vn.dernier) {
        cle->derivations.raz();
        ar=cle;
    }
    else { //sinon on nettoie (a l'exception du premier element qui servira de centre de fusion)
        //la liste de derivations de tous ceux que l'on va fusionner
        ar=vn.retireElement(0);
        for (int i=0;i<vn.dernier;i++) {
            for (int j=0;j<cle->derivations.dernier;j++) {
                if (cle->derivations[j]==vn[i]) {
                    cle->derivations.retireElement(j);
                    break;
                }
            }
        }        
    }
    
    ar->X=X;
    ar->argument=num_arg;
    ar->traits=tr;
    
    return OUI;
}


void FusionneFictif(SousArbre* arbre,char deterministe,VECTA<SousArbre*>& nettoyage) {
    VECTA<SousArbre*> pile;
    
    //On cree alors un element fictif qui chapeaute l'element non bornee
    //Traitement du premier element    
    ConstruitExtremite(arbre,pile,OUI);
    for (int ip=0;ip<pile.dernier;ip++)
        pile[ip]->suivant=arbre->freres[0];
    
    for (int i=0;i<arbre->freres.dernier;i++) {
        pile.raz();
        ConstruitExtremite(arbre->freres[i],pile,OUI);
        for (int ip=0;ip<pile.dernier;ip++) {
            pile[ip]->suivant=arbre->freres[i+1];
        }
    }
}

SousArbre* GlobalParseur::ConstruitArbre(ifstream& fichier,
                                         SeqFonc* fonc,
                                         VECTA<SousArbre*>& nettoyage,
                                         char negation,
                                         char fin,
                                         char optio,
                                         int increment,                                         
                                         typeRegle regle) {
    
    char k, tampon[1000];
    char suite[100];
    SousArbre* cle=NULL;
    SousArbre* element=NULL;
    SousArbre* fictif=NULL;
    
    char debut;
    char disjonction=NON;
    char neg=NON;
    
    Sequence* seq=NULL;
    Fonction* fFonc=NULL;
    
    if (fonc->type()==REGLES_SEQUENCE)
        seq=(Sequence*)fonc;
    else
        fFonc=(Fonction*)fonc;
    
    k=0;
    debut=OUI;
    
    k=lit(fichier,tampon);    
    
    
    if (fin=='|' && k=='@') {
        fonc->deterministe=longue;
        k=lit(fichier,tampon);
    }
    
    if (fin=='|' && k=='!') {
        fonc->deterministe=courte;
        k=lit(fichier,tampon);
    }
    
    while (!fichier.eof()) {
        
        int num_arg=-1;
        char optionnel=NON;
        char modification=NON;
        
#ifndef XIP_LIGHT
        if (regle==REGLES_SEQUENCE || regle==REGLES_EXTRACTION || regle==REGLES_RECONSTRUCTION_MODELE) {
            if (!strcmp(tampon,"where") && k=='(') {
                TestArbre* tarbre=NULL;
                static VECTA<char> vari(5);
                //lecture du test... Il s'agit d'un test en debut de definition            
                tarbre=new TestArbre;
                k=' ';
                LectureTest(fichier,k,tampon,NON,vari,&tarbre);
                TVecteur clefArbre;
                if (fonc->listes!=NULL)
                    erreur(CHAINES[10]);
                fonc->listes=new ListeTestArbre;
                tarbre->direct=tarbre->LineariseDirect(this,clefArbre,*fonc->listes);
                delete tarbre;
                modification=OUI;
                if (regle==REGLES_RECONSTRUCTION_MODELE)
                    break;
            }
        }
#endif
        neg=negation;
        if (k=='~')
            neg=1-negation;
        int ifr;
        element=ConstruitNoeud(fichier,nettoyage,tampon,k,regle,negation,optionnel,modification);
        if (element!=NULL) {
            debut=NON;
            TraitementArgument(this,element,regle,fonc);
            if (k=='{') {
                element->fils=ConstruitArbre(fichier,fonc,nettoyage,negation,'}',optionnel,increment,regle);
                k=lit(fichier,tampon);
            }
        }
        else
            if (k=='(') {
                debut=NON;
                element=ConstruitArbre(fichier,fonc,nettoyage,neg,')',NON,increment,regle);
                neg=negation;
                modification=OUI;
                k=lit(fichier,tampon);
                optionnel='(';
                if (tampon[0]=='*' || tampon[0]=='+')
                    optionnel=tampon[0];
                else
                    //si l'element est seul
                    if (element->freres.dernier==0 && element->derivations.dernier==0)
                        optionnel='(';
                    
            }
            
            char nepasrelire=NON;
            //Lecture d'un contexte: un '|'
            if (seq!=NULL && seq->passageLexical!=-1 && k=='|') {
                if (debut==OUI)
                    seq->gauche=LitUneSequence(fichier,seq->Fx,neg,-1);
                else
                    seq->droit=LitUneSequence(fichier,seq->Fx,neg,1);
                k=lit(fichier,tampon); 
                //cas affreusement particulier, ou juste apres un contexte, il n'y a qu'une seule categorie a lire
                //on relance la lecture sans faire de boucle...
                if (debut==OUI && k==fin && element==NULL)
                    element=ConstruitNoeud(fichier,nettoyage,tampon,k,regle,negation,optionnel,modification);
                nepasrelire=OUI;
                modification=OUI;
            }
            
            debut=NON;
            char disjonctionPrecedente=disjonction;
            
            if (k==',' || k==';') {
                if (negation==OUI) {
                    if (k==',')
                        k=';';
                    else
                        k=',';
                }
                
                if (k==';')
                    disjonction=OUI;
                else
                    disjonction=NON;
                if (nepasrelire==NON)
                    k=lit(fichier,tampon);
                modification=OUI;
            }
            else
                if (k==fin) {
                    modification=OUI;
                    fin=OUI;
                }
                
                int rebouclage=NON;
                if (element!=NULL) {
                    if (optionnel=='*' || optionnel=='+' || optionnel=='-') {
                        //On boucle sur soi-meme...
                        if (optionnel=='+')
                            element->plus=OUI;
                        
                        rebouclage=OUI;
                        if (element->freres.dernier==0) {
                            if (element->X!=NULL) {    
                                element->reboucle=element->derivations.dernier;
                                element->derivations.ajoute(element);
                            }
                            else {
                                VECTA<SousArbre*> pile;
                                ConstruitExtremite(element,pile,OUI);
                                //On reboucle donc au sein de la derivation
                                for (int ip=1;ip<pile.dernier;ip++) {
                                    pile[ip]->reboucle=pile[ip]->derivations.dernier;
                                    pile[ip]->derivations.ajoute(element);
                                }
                            }
                        }
                        else {
                            element->freres.fin()->reboucle=element->freres.fin()->derivations.dernier;
                            element->freres.fin()->derivations.ajoute(element);
                        }                
                    }
                    
                    
                    //Les cas de creations d'un element fictif sont:
                    //a) un element optionnel
                    //b) une disjonction nouvelle qui se prepare
                    if ( (disjonction==OUI && disjonctionPrecedente==NON) ||
                        ((optionnel=='(' || optionnel=='*' || optionnel=='-') && 
                        (cle==NULL || cle->X!=NULL || disjonctionPrecedente==NON))) {

                        if (optionnel=='-')
                            element->saute=TOUS;
                        
                        //Cas particulier, l'element sous * est deja un element complexe
                        //on ne cree pas alors un niveau supplementaire
                        if (element->X!=NULL) {
                            //on cree alors un noeud fictif dans lequel on place notre element
                            fictif = new SousArbre(NULL,NULL,-1);
                            fictif->derivations.ajoute(element);
                            nettoyage.ajoute(fictif);

                            //on conserve au plus au niveau l'argument de l'element bouclant
                            fictif->argument=element->argument;
                            element=fictif;
                        }
                        
                        //Dans le cas d'un rebouclage interne au sein de la derivation
                        //on le retient au sein de cet element fictif
                        if (rebouclage==OUI)
                            element->reboucle=-2;                    
                        
                        if (optionnel=='*' || optionnel=='(')
                            element->saute=OUI;
                        else
                            if (optionnel=='-')
                                element->saute=TOUS;
                    }
                    
                    if (disjonctionPrecedente==OUI) {
                        if (optionnel=='*' || optionnel=='(' || optionnel=='-')
                            cle->saute=OUI;
                        if (increment==1) {
                            if (cle==NULL)
                                cle=element;
                            else
                                if (cle->freres.dernier==0)
                                    cle->derivations.ajoute(element);
                                else
                                    cle->freres.fin()->derivations.ajoute(element);
                        }
                        else
                            cle->derivations.ajoute(element);
                    }
                    else {
                        if (cle==NULL)
                            cle=element;
                        else {
                            if (increment==1)
                                cle->freres.ajoute(element);
                            else {
                                //On ajoute les elements a l'envers
                                element->freres.ajoute(cle);
                                for (ifr=0;ifr<cle->freres.dernier;ifr++)
                                    element->freres.ajoute(cle->freres[ifr]);
                                cle->freres.raz();
                                cle=element;
                            }
                        }
                    }
                }
                
                if (modification==NON) {
                    strcpy(suite,CHAINES[69]);
                    strcat(suite,":");
                    strcat(suite,tampon);
                    tampon[0]=k;
                    tampon[1]=0;
                    strcat(suite,tampon);
                    erreur(suite);
                }
                
                if (fin==OUI)
                    break;
    }
    
    if (cle != NULL) {
        FusionneFictif(cle,fonc->deterministe,nettoyage);
        element=cle;
        static VECTA<SousArbre*> vn(20);
        while (element != NULL) {
            if (FusionneNoeud(this,element,vn))
                NettoieNettoyage(nettoyage,vn);            
            vn.raz();
            element=element->suivant;
        }                
        cle->ConstruitClef(this,regle);
    }
    
    return cle;
}            




EtatArbre* GlobalParseur::ConstruitAutomate(ifstream& fichier,
                                            SeqFonc* fonc,                                            
                                            char negation,
                                            char fin,
                                            char optio,
                                            int increment,                                         
                                            typeRegle regle,
                                            EtatArbre** queue) {
    
    char k, tampon[1000];
    EtatArbre* cle=NULL;
    EtatArbre* debutcle=NULL;
    EtatArbre* lesuivant=NULL;
    ArcArbre* element=NULL;
    ArcArbre* fictif=NULL;    
    EtatArbre* etatfictif=NULL;

    char disjonction=NON;
    char neg=NON;
    char ajoutelement=OUI;
    
    Sequence* seq=NULL;
    Fonction* fFonc=NULL;
    
    if (fonc->type()==REGLES_SEQUENCE)
        seq=(Sequence*)fonc;
    else
        fFonc=(Fonction*)fonc;
    
    k=0;
    
    
    k=lit(fichier,tampon);    
    
    
    if (fin=='|' && k=='@') {
        fonc->deterministe=longue;
        k=lit(fichier,tampon);
    }
    
    if (fin=='|' && k=='!') {
        fonc->deterministe=courte;
        k=lit(fichier,tampon);
    }
    char utilise=-1;
    lesuivant=listeEtatArbre.alloue();
    EtatArbre* finqueue=NULL;                
    EtatArbre* deb=NULL;
    int iarc;
    *queue=lesuivant;

    cle=listeEtatArbre.alloue();
    debutcle=cle;

    while (!fichier.eof()) {
        
        int num_arg=-1;
        char optionnel=NON;
        char modification=NON;
        ajoutelement=OUI;
        neg=negation;
        deb=NULL;
        if (k=='~')
            neg=1-negation;   
        element=ConstruitArc(fichier,tampon,k,regle,negation,optionnel,modification,lesuivant);
        if (element!=NULL) {
            utilise=OUI;
            TraitementArcArgument(this,element,regle,fonc);
            if (k=='{') {
                finqueue=NULL;
                etatfictif=lesuivant;
                if (lesuivant->fils!=NULL) {
                fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);
                fictif->etatarbre=lesuivant;
                etatfictif=listeEtatArbre.alloue();
                etatfictif->ajoutearc(fictif,XCatEpsilon,fonc->deterministe);
                element->etatarbre=etatfictif;
                //On rajoute un niveau dans l'arbre pour l'etat inferieur
                etatfictif->fils=ConstruitAutomate(fichier,
                    fonc,negation,'}',optionnel,increment,regle,&finqueue);   
                etatfictif->ConstruitClef(XCatEpsilon);
                k=lit(fichier,tampon);
            }
        }
        else {
            if (k=='(') {        
                deb=ConstruitAutomate(fichier,fonc,neg,')',NON,increment,regle,&finqueue);
                
                VECTA<ArcArbre*> lesderniers;
                k=lit(fichier,tampon);                                
                EtatArbre* courant=cle;

                //Si on lit des parentheses imbriquees, on rajoute un niveau
                if (fin==')') {
                    fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);
                    fictif->etatarbre=deb;
                    cle->ajoutearc(fictif,XCatEpsilon,fonc->deterministe);
                    //Puis on cree un epsilon vers l'etat suivant
                    fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);
                    fictif->etatarbre=lesuivant;
                    finqueue->ajoutearc(fictif,XCatEpsilon,fonc->deterministe);                    
                    courant=deb;
                }
                else {
                    //Tout d'abord finqueue devient l'etat final pour tous les elements de cle qui branchait
                    //vers le suivant
                    cle->arcvers(lesderniers,lesuivant);                
                    for (iarc=0;iarc<lesderniers.dernier;iarc++)                    
                        lesderniers[iarc]->etatarbre=finqueue;

                    //on rajoute les arcs directement dans cle                 
                    for (iarc=0;iarc<deb->arcs.dernier;iarc++)
                        cle->ajoutearc(deb->arcs[iarc],XCatEpsilon,fonc->deterministe);
                    
                    deb->arcs.raz();
                    listeEtatArbre.detruit(deb);
                    listeEtatArbre.detruit(lesuivant);
                    lesuivant=finqueue;
                    finqueue->etatfin=0;
                    *queue=lesuivant;                  
                }
                
                switch (tampon[0]) {
                case '*':
                    fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);    
                    fictif->etatarbre=finqueue;
                    courant->ajoutearc(fictif,XCatEpsilon,fonc->deterministe);
                case '+':
                    //On boucle alors vers le debut
                    fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);    
                    fictif->etatarbre=courant;
                    fictif->type=1;
                    finqueue->ajoutearc(fictif,XCatEpsilon,fonc->deterministe);
                    break;
                default:
                    fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);    
                    fictif->etatarbre=finqueue;
                    courant->ajoutearc(fictif,XCatEpsilon,fonc->deterministe);                    
                }                

                //Puis on lit l'etat final avec l'etat suivant
                finqueue=lesuivant;
                optionnel=0;
                ajoutelement=NON;
                utilise=OUI;
            }
        }
                                
        char disjonctionPrecedente=disjonction;
        if (k!=',' && k!=';' && k!=fin)
            erreur(CHAINES[280]);

        if (k==',' || k==';') {
            if (negation==OUI) {
                if (k==',')
                    k=';';
                else
                    k=',';
            }
            
            if (k==';')
                disjonction=OUI;
            else
                disjonction=NON;
        }
        
        if (ajoutelement==OUI) {
            if (optionnel) {                           
                //Tout d'abord on tente l'element suivant
                element->etatarbre->ajoutearc(element,XCatEpsilon,fonc->deterministe);
                element->type=1;                
                if (optionnel=='*') {
                    //on cree un element de saut qui part de l'etat initial et va a l'etat final
                    fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);
                    fictif->etatarbre=lesuivant;
                    cle->ajoutearc(fictif,XCatEpsilon,fonc->deterministe);                    
                }            
            }            
            cle->ajoutearc(element,XCatEpsilon,fonc->deterministe);    
        } 
            
        if (disjonction==NON) {
            //dans le cas d'une boucle
            
            //dans le cas d'une disjonction, on reste sur l'element courant                                
            //On fusionne alors le noeud courant
            /*
            if (cle!=NULL) {                
            if (FusionneNoeud(this,cle,vn)) {
            NettoieNettoyage(nettoyage,vn);            
            vn.raz();
            }
            }
            */
            
            cle=lesuivant;    
            if (k!=fin) {
                lesuivant=listeEtatArbre.alloue();                
                utilise=NON;
            }
        }
                  
        if (k==fin)
            break;
        *queue=lesuivant;
        k=lit(fichier,tampon);
    }
    
    if (utilise==NON)
        listeEtatArbre.detruit(lesuivant);


    (*queue)->etatfin=OUI;

    
    return debutcle;
}
