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
   filename   : xmlxip.cxx
   Date       : 10/01/1999
   Purpose    : Basic implementation of XML input parsing
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"

void majvariablelemme(VariableLemmeIndex* vlemme,Noeud* n,uchar* chaine,int pos,string,double res);    
extern ostream* flot_erreur;

char TestChaineVide(unsigned char* chaine) {
    size_t l=strlen((char*)chaine);
    for (size_t i=0;i<l;i++) {
        if (chaine[i]>32)
            return NON;
    }

    return OUI;
}


//Le fichier contient alors des entrees de la forme:
/*
 <LUNIT>

 <NODE tag=v, start=v, end=v> //can contain embedded nodes
     <features>    //optional (global features)
     </features>
     </mainreading lemma=v,pos=v>
     <reading lemma=v, pos=v>
        <features> //local features
          attr=v.
          attr=v.
        </features>
     </reading> //could be other readings.
 </NODE>
 </LUNIT>
*/
char rechercheNomFonction(char* nom);


Stocke_Variable* RechercheVariable(VECTA<Variable*>& Var,char* nom, char* valeur) {
    char c;

    double vf=atof(valeur);
    char* pt=strstr(nom,"##");
    if (pt!=NULL) {
        c=pt[2];
        pt[2]=0;
    }
    
    for (int i=0;i<Var.dernier;i++) {
        if (!strcmp(nom,Var[i]->nom)) {
            if (pt!=NULL)
                pt[2]=c;
            Stocke_Variable* sv=new Stocke_Variable(Var[i],vf,nom);
            return sv;
        }
    }

    if (pt!=NULL)
        pt[2]=c;

    return NULL;
}

char NettoieUneChaine(char* chaine,char* resul,const char* trouve,const char* remplace) {

    char* debut;
    char* courant=chaine;
    char trouver=NON;

    resul[0]=0;
    while ((debut=strstr(courant,trouve))) {
        *debut=0;
        strcat(resul,courant);
        strcat(resul,remplace);
        courant=debut+strlen(trouve);
        trouver=OUI;
    }        
    
    strcat(resul,courant);
    return trouver;
}


void GlobalParseur::NettoieLaChaine(char* chaine,char* resul) {
    char buffer[1000]; 
    
    strcpy(resul,chaine);
    for (int i=0;i<caractereXML.dernier;i++) {
        if (NettoieUneChaine(chaine,buffer,codageXML[i],caractereXML[i]))
            strcpy(resul,buffer);
    }
}

#ifdef XIPLIBXML
GlobalParseurXML::GlobalParseurXML(char* filename,char* path) : GlobalParseur(filename,path) {
    detruireDonnees=NON;
}
 
GlobalParseurXML::~GlobalParseurXML() {}

void GlobalParseurXML::LireBaliseVariable(char* balise,const char* vari) {
    unsigned char c=0;
    unsigned int i=0;

    balise[0]=0;
	int ln=strlen(vari);
    while (!donnees->eof() && i<ln) {
        c=donnees->get();
        c=tolower(c);
        if (c!=vari[i]) {
            i=0;
            continue;
        }

        balise[i++]=c;
    }
        
    if (donnees->eof())
        return;

    char agit=OUI;
    while (!donnees->eof() && (agit==NON || c!='>')) {
        c=donnees->get();
        if (c=='\"')
            agit=1-agit;
        if (agit==OUI)
            c=tolower(c);
        balise[i++]=c;
    }

    balise[i]=0;
}

void GlobalParseurXML::LireUneBalise(char* balise,char init=0) {
    unsigned char c=init;
    int i=0;

    balise[0]=0;

    while (!donnees->eof() && c!='<')
        c=donnees->get();

    if (donnees->eof())
        return;

    balise[i++]=c;

    char agit=OUI;
    while (!donnees->eof() && (agit==NON || c!='>')) {
        c=donnees->get();
        if (c=='\"')
            agit=1-agit;
        if (agit==OUI)
            c=tolower(c);
        balise[i++]=c;
    }

    balise[i]=0;
}
 
unsigned char GlobalParseurXML::LirePCDATA(char* pcdata) {
    unsigned char c=0;
    int i=0;

    pcdata[0]=0;
    c=donnees->get();
    while (c==10 || c==13)
        c=donnees->get();        

    while (!donnees->eof() && c!='<') {
        if (c!=10 && c!=13)
            pcdata[i++]=c;
        c=donnees->get();
    }
    
    pcdata[i]=0; //on retire le dernier retour chariot
    return c;
}

char GlobalParseurXML::LireTraits(Traits* tr,char* attribut,char* valeur) {

    //s'il s'agit d'une valeur ontologique
    if (attribut[0]=='@') {
        ParseOntologie(attribut,tr);
        return OUI;
    }

    //recherche du nom de l'attribut
    trait* vattribut=ArbTraits->recherche(attribut);

    if (!vattribut) {
        if (AFFICHE_ERREUR==OUI) {
            sprintf(Chaine_traits,"Feature %s:%s",CHAINES[173],attribut,valeur);
            if (TRACE)
                trace<<Chaine_traits<<Endl;
            insere_erreur(Chaine_traits);
            *flot_erreur<<Chaine_traits<<Endl;
        }
        
        return NON;
    }
    
    trait* vvaleur=RetrouveValeurTrait(vattribut,valeur,NON);
    if (vvaleur!=NULL) {
		tr->inittrait(vattribut,vvaleur);
        return OUI;
    }
    return NON;
}
//---------------------------------------------------------------------------------------------------------
void GlobalParseurXML::LireReadings(char* balise,
                                    Liste* lpere,
                                    char* lemme,
                                    char* surface,
                                    VECTA<Stocke_Variable*>& StockeVar,
                                    char& finbalise,
                                    char& finunite) {
    vocabulaire v;
    char* findechaine;
    ListeVoc* voc=NULL;
    char premier=OUI;
    Traits concatene(this);
    char modificationVoc=NON;
    char inter[1000];
    char pos[1000];
    char attribute[1000];
    char value[1000];
    Stocke_Variable* sv;

    char typeEntree=OUI;   
    FusionX=0;
    while (xipsinput.Extract(balise,"<reading lemma=\"%s\" pos=\"%s\"",&findechaine,inter,pos)) {
        
        Categorie* catVoc=NULL;
        Traits* trVoc=NULL;
        
        LireUneBalise(balise);  
        
        //Lorsque typeEntree=2, cela signifie que dans le lexique utilisateur
        //cette entree doit etre completement ecrasee. On ne se preoccupe plus
        //alors de ses autres lectures.
        if (typeEntree!=2) {
            NettoieLaChaine(inter,lemme);
            if (finbalise==NON && EndOfSentence(lemme,NON))
                finunite=OUI;
            
            catVoc=ChercheCat(pos);
            trVoc=new Traits(this);
            trVoc->ou(catVoc);
        }
        
        while (xipsinput.Extract(balise,"<feature attribute=\"%s\" value=\"%s\"",&findechaine,attribute,value)) {
            if (finbalise==NON && EndOfSentence(balise,NON))
                finunite=OUI;
            //Si typeEntree==2, on saute tous les traits sans les integrer
            if (typeEntree!=2) {
                if (attribute[0]!='#')
                    LireTraits(trVoc,attribute,value);
                else {
                    sv=RechercheVariable(Variables,attribute+1,value);
                    if (sv!=NULL)
                        StockeVar.ajoute(sv);
                }
            }
            LireUneBalise(balise);
        }
        
        //On saute l'ensemble des entrees qui suivent
        if (!strcmp(balise,"</reading>") && typeEntree==2) {
            LireUneBalise(balise);
            continue;
        }
        
        strcpy((char*)v.surface,surface);
        strcpy((char*)v.lemme,lemme);
        v.init(&catVoc->X,trVoc);
        
        auto_vocabulaire* traduction=NULL;
        VECTA<void*> a;        
        ExtraitVocabulaire(a,0,v);
        for (int ia=0;ia<a.dernier;ia++) {            
            auto_vocabulaire* trad=(auto_vocabulaire*)a[ia];        
            
            while (trad != NULL) {
                //si on n'a pas besoin de tester les traits, 
                //on sort en cas de meme categorie            
                if (trad->X==&CatVide->X ||
                    trad->X==v.X) {
                    if (trad->traits_mot==NULL || 
                        ITR(this,trad->traits_mot,v.initiaux,NONECRIT))
                        break;
                }
                
                //Plusieurs mots peuvent partager le meme lemme...
                trad=trad->Autre();
            }
            
            
            if (trad != NULL) {      
                modificationVoc=OUI;
                if (trad->ecraser!=NOUVEL && trad->ecraser!=UNEFOISNOUVEAU) {
                    //si ce n'est pas les memes categories, 
                    //on nettoie les traits en question
                    if (trad->ecraser==ECRASER || trad->ecraser==ECRASERTOUT) {
                        v.initiaux->raz();
                        //autre categorie
                        v.X=trad->XNouveau;
                        //Dans le cas ou on est dans une situation de remplacement total
                        //le type de retour passe a 2...
                        if (trad->ecraser==ECRASERTOUT)
                            typeEntree=2;
                    }
                    
                    //on ajoute les nouveaux traits...
                    if (trad->tr != NULL) {
                        InstancieTraits(v.initiaux,trad->tr);                        
                        if ((trad->tr)->lemme!=NULL && (trad->tr)->maniplemme==ECRASE)
                            (trad->tr)->lemme->mot(v.lemme,2048);
                        if ((trad->tr)->surface!=NULL && (trad->tr)->manipsurface==ECRASE)
                            (trad->tr)->surface->mot(v.surface,2048);
                    }
                }
                else
                    traduction=trad;
            }
        }
        
        concatene.ou(v.initiaux);
        
        int e=0;
        if (voc==NULL) {
            voc=fournirListeVoc(v);
            voc->num_mot=num_mot++;
            nombre_mots++;
            phrase.ajoute(voc);
        }
        else {
            voc->tr.ajoute(v.initiaux);
            voc->lemme.ajoute(DupliqueChaine(v.lemme));
            voc->X.ajoute(v.X);
            e=voc->nbX;
            voc->nbX++;
        }
        
        FusionX|=v.X->Val;
        if (TestOptions(AFFICHAGE_ENTREE) || DESAMBIGUISATION==OUI) {
            sprintf(pos,"%s\t%s",v.surface,v.lemme);
            voc->tag.affecte(e,DupliqueChaine(pos));
        }
        else
            voc->tag.affecte(e,NULL);
        
        if (traduction!=NULL && traduction->ecraser==NOUVEL) {
            v.X=traduction->XNouveau;
            //on ajoute les nouveaux traits...
            if (traduction->tr != NULL) {
                v.initiaux=new Traits(traduction->tr);
                if ((traduction->tr)->lemme!=NULL && (traduction->tr)->maniplemme==ECRASE)
                    (traduction->tr)->lemme->mot(v.lemme,2048);
                if ((traduction->tr)->surface!=NULL && 
                    (traduction->tr)->manipsurface==ECRASE)
                    (traduction->tr)->surface->mot(v.surface,2048);
            }
            else {
                Categorie* cc= Categories[v.X->index];
                v.initiaux=new Traits(this);
                (v.initiaux)->ou(cc);
            }
            
            FusionX|=v.X->Val;            
            if (TestOptions(AFFICHAGE_ENTREE) || DESAMBIGUISATION==OUI) {
                sprintf(pos,"%s\t%s",v.surface,v.lemme);
                voc->tag.ajoute(DupliqueChaine(pos));
            }
            else
                voc->tag.ajoute(NULL);
            
            concatene.ou(v.initiaux);
            voc->tr.ajoute(v.initiaux);
            voc->lemme.ajoute(DupliqueChaine(v.lemme));
            voc->X.ajoute(v.X);
            voc->nbX++;
        }
        
        if (!strcmp(balise,"</reading>"))
            LireUneBalise(balise);
    }
    
    v.raz();
    
    if (voc!=NULL) {
        //Le pere du noeud terminal est vide...
        //On cree donc un noeud pere qui pointera sur ce noeud
        //terminal
        Liste* L=TasDeListes->fournir(this,lpere->X,NULL);
        
        //On recopie traitsbase dans traitscourant
        L->SauveTraits();
        
        L->N.gauche=lpere->N.gauche;
        L->N.droite=lpere->N.droite;
        
        //Dans le cas ou l'on a des 
        if (ListeCoupeSequence.dernier)
            voc->global=new Traits(concatene);
        
        L->N.surface=(char*)voc->surface;       
        L->N.lemme=(char*)voc->lemme[0];
        L->N.num_mot=voc->num_mot;
        L->N.clef=(voc->X[0])->Val;
                
        //Dans le cas ou une modification de vocabulaire est survenue, due a une lecture dans le lexique utilisateur
        if (modificationVoc==OUI) {
            //On remplace par les traits issus de la concatenation des entrees du vocabulaire
            (L->traitsbase)->recopie(concatene);
            //on ecase la categorie de base
            L->X=voc->X[0];
        }
        
        L->voc=voc;
        L->index=voc->CalculIndex();
        
        lpere->N.fils=&L->N;
        L->N.pere=&lpere->N;
    }
}

//---------------------------------------------------------------------------------------------------------
Liste* GlobalParseurXML::LireNoeud(char* balise,Liste* lpere,VECTA<Noeud*>& LesNoeuds,char& finbalise) {

    char surface[1000];
    char lemme[1000];
    char inter[1000];
    char pos[1000];
    int start,end;
    Traits* tr;
    Noeud* noeud;
    char terminal=NON;
    Categorie* cat=NULL;
    
    Liste* L;
    int num=-1;
    char finunite=NON;
    char carsuivant=0;
    VECTA<Stocke_Variable*> StockeVar;
    Stocke_Variable* sv;
    char* findechaine;

    if (xipsinput.Extract(balise,"<node num=\"%d\" tag=\"%s\" start=\"%d\" end=\"%d\">",
                       &findechaine,&num,surface,&start,&end)) {
        cat=ChercheCat(surface);
        if (cat==NULL) {
            *flot_erreur<<"Wrong entry:"<<surface<<endl;
            return NULL;
        }
    }
    else
        if (!xipsinput.Extract(balise,"<token pos=\"%s\" start=\"%d\" end=\"%d\">",
                               &findechaine,pos,&start,&end)) {
            *flot_erreur<<"Wrong entry:"<<balise<<endl;
            return NULL;
        }
        else {
            //Il faut lire le reste de la chaine, jusqu'a la balise suivante...
            carsuivant=LirePCDATA(inter);
            NettoieLaChaine(inter,surface);
            if (finbalise==NON && EndOfSentence(balise,NON))
                finunite=OUI;

            cat=ChercheCat(pos);       
            if (cat==NULL) {
                *flot_erreur<<"Wrong tag:"<<pos<<endl;
                return NULL;
            }
        }

    tr=new Traits(this);
    if (cat!=NULL)
        tr->ou(cat);

    LireUneBalise(balise,carsuivant); 

    char attribute[1000];
    char value[1000];
    while (xipsinput.Extract(balise,"<feature attribute=\"%s\" value=\"%s\"",&findechaine,attribute,value)) {
        if (finbalise==NON && EndOfSentence(balise,NON))
            finunite=OUI;
        if (attribute[0]!='#')
            LireTraits(tr,attribute,value);
        else {
            sv=RechercheVariable(Variables,attribute+1,value);
            if (sv!=NULL)
                StockeVar.ajoute(sv);
        }
            
        LireUneBalise(balise);
    }

    L=TasDeListes->fournir(this,&cat->X,tr);   
    //Le dernier noeud cree est range dans le pointeur noeud...
    noeud=&L->N;
    noeud->gauche=start;
    noeud->droite=end;
    if (lpere!=NULL) {
        if (lpere->N.fils==NULL)
            lpere->N.fils=noeud;
        else
            (lpere->N.fils)->insere(noeud);
        noeud->pere=&lpere->N;
    }
    //noeud pointe sur le dernier noeud cree
    noeud->num_noeud=num;
    //Puis on stocke dans ce tableau ces noeuds nouvellement crees.
    LesNoeuds.affecte(num,noeud);

    if (!memcmp(balise,"<reading lemma=",15))
        LireReadings(balise,L,lemme,surface,StockeVar,finbalise,finunite);

    for (int ivar=0;ivar<StockeVar.dernier;ivar++) {
        //On enregiste la valeur dans la variable
        //Premier cas celui d'une variable globale (attachee a aucun noeud)
        VariableLemmeIndex* varchaine;
        char* pt;
        char* lenom;
        int posnom;
        switch (StockeVar[ivar]->var->sorte) {
        case VGLOBALE:
        case VGLOBALEREINIT:
            StockeVar[ivar]->var->AffecteValeur(StockeVar[ivar]->val);
            break;
        case VLEMMEINDEX:
            posnom=0;
            varchaine=(VariableLemmeIndex*)StockeVar[ivar]->var;
            varchaine->utilisee=OUI;
            lenom=StockeVar[ivar]->nom;
            pt=strstr(lenom,"##");
            if (pt!=NULL) {
                pt=strchr(pt+3,'#');
                posnom=pt-lenom+1;
            }
			majvariablelemme(varchaine,NULL,(uchar*)lenom,posnom,"",StockeVar[ivar]->val);				
            break;
        default:
            StockeVar[ivar]->var->Affecte(L->position,StockeVar[ivar]->val);
            //On enregiste la variable dans le noeud
            L->variables.affecte(StockeVar[ivar]->var->position,StockeVar[ivar]->var);
        }
    }
    
    StockeVar.nettoie();
    
    if (tr!=NULL)
        delete tr;
        
    //dans le cas d'une lecture d'une liste de tokens (sans lunit)
    //si la chaine d'arret a ete trouvee, on s'arrete et on
    //place la valeur 2 dans finbalise...
    if (finunite==OUI) {
        finbalise=2;
        return L;
    }
        
    while (!memcmp(balise,"<node",5) || !memcmp(balise,"<token",6)) {
        LireNoeud(balise,L,LesNoeuds,finbalise);   
        LireUneBalise(balise);
    }
    
    if (!strcmp(balise,"</node>") || !strcmp(balise,"</token>"))
        return L;
    
    *flot_erreur<<"No </NODE> tag"<<Endl;
    return NULL;
}
   
//---------------------------------------------------------------------------------------------------------

void GlobalParseurXML::LireDependances(char* balise,VECTA<Noeud*>& LesNoeuds,VECTA<Stocke_Variable*>& StockeVar) {
    char name[1000];
    char* findechaine;

    if (!xipsinput.Extract(balise,"<dependency name=\"%s\">",&findechaine,name)) {
        *flot_erreur<<"Wrong entry:"<<balise<<endl;
        return;
    }

    Categorie* fonction=ChercheFonc(name);
    LireUneBalise(balise);

    char attribute[1000];
    char value[1000];
    VECTA<char*> pile;
    ResultatFonction* rf=FournirResFonc(&fonction->X,NULL);
    rf->numeros.ajoute(99999);
    

    while (xipsinput.Extract(balise,"<feature attribute=\"%s\" value=\"%s\"",&findechaine,attribute,value)) {
        if (!strcmp(attribute,"$STACK"))
            pile.ajoute(DupliqueChaine(value));
        else {
            if (attribute[0]=='$') {
                Stocke_Variable* sv = RechercheVariable(Variables,attribute+1,value);
                if (sv!=NULL) {
                    sv->rf=rf;
                    StockeVar.ajoute(sv);
                }
            }
            else
                LireTraits(rf->traits,attribute,value);
        }
        LireUneBalise(balise);
    }

    //Lecture des parametres..
    int num,position;

    while (xipsinput.Extract(balise,"<parameter ind=\"%d\" num=\"%d\"",&findechaine,&position,&num)) {
        if (LesNoeuds[num]==NULL) {
            *flot_erreur<<"NULL Node:"<<num<<Endl;
            RendreResFonc(rf);
            break;
        }
        else {
            rf->arg.affecte(position,LesNoeuds[num]);
            rf->nb_args++;
        }
        LireUneBalise(balise);
    }

    for (int i=0;i<pile.dernier;i++)
        rf->stack.ajoute(pile[i]);
    
    FONCTIONSINTERMEDIAIRES.ajoute(rf);
    if (strcmp(balise,"</dependency>"))
        *flot_erreur<<"Wrong TAG name:"<<balise<<Endl;        
    
}

void GlobalParseurXML::SentenceAnalysis(char renumerote,char arbre) {
    char balise[1000];
    Liste* L;
    VECTA<Noeud*> LesNoeuds(10);
    VECTA<Stocke_Variable*> StockeVar;
    baseOffset=0;

	num_phrase=0;
    balise[0]=0;

    LireBaliseVariable(balise,"<xipresult");

    if (!strcmp(balise,"<xipresult math=\"1\">")) {
        //Tout d'abord on recherche la presence de variables:
        LireBaliseVariable(balise,"<variables");

        if (!strcmp(balise,"<variables>")) {
            char attribute[1000];
            char value[1000];
            char* findechaine;
            LireUneBalise(balise);
            while (xipsinput.Extract(balise,"<feature attribute=\"%s\" value=\"%s\"",&findechaine,attribute,value)) {
                Stocke_Variable* sv=RechercheVariable(Variables,attribute+1,value);
                if (sv!=NULL)
                    StockeVar.ajoute(sv);                
                LireUneBalise(balise);
            }
 
            for (int ivar=0;ivar<StockeVar.dernier;ivar++) {
                //On enregiste la valeur dans la variable
                //Premier cas celui d'une variable globale (attachee a aucun noeud)
                VariableGlobale* varglob;
                VariableLemmeIndex* varchaine;
                char* pt;
                char* lenom;
                int posnom;
                switch (StockeVar[ivar]->var->sorte) {
                case VGLOBALE:
                case VGLOBALEREINIT:
                    varglob=(VariableGlobale*)StockeVar[ivar]->var;
                    varglob->AffecteValeur(StockeVar[ivar]->val);
                    break;
                case VLEMMEINDEX:
                    posnom=0;
                    varchaine=(VariableLemmeIndex*)StockeVar[ivar]->var;
                    varchaine->utilisee=OUI;
                    lenom=StockeVar[ivar]->nom;
                    pt=strstr(lenom,"##");
                    if (pt!=NULL) {
                        pt=strchr(pt+3,'#');
                        posnom=pt-lenom+1;
                    }
					majvariablelemme(varchaine,NULL,(uchar*)lenom,posnom,"",StockeVar[ivar]->val);				
                    break;
                }
            }   
            StockeVar.nettoie();            
            LireUneBalise(balise);
        }

        //On remonte au debut du fichier
        donnees->seekg(0);    

        balise[0]=0;
        LireBaliseVariable(balise,"<xipresult");
    }

    char finbalise=NON;

    while (!donnees->eof() && strcmp(balise,"</xipresult>")) {
        LireUneBalise(balise);

        //Dans le cas ou l'on rencontre une balise <lunit>, cela signifie que la decomposition en
        //phrase est deja effectuee
        if (!strcmp(balise,"<lunit>")) {
            finbalise=OUI;
            LireUneBalise(balise);
        }

        if (!strcmp(balise,"</xipresult>"))
            break;

        LListe& Phrase= glisteDesPhrases[0]; //les couches vont de 0 a MaxCouches...
        Phrase.dernier=0;

        TasDeListes->Raz();
        LesNoeuds.raz();
       
        while (!donnees->eof() && strcmp(balise,"</xipresult>")) {
                    
            if (!strcmp(balise,"</lunit>"))
                break;

            L=NULL;
            if (!memcmp(balise,"<node",5) || !memcmp(balise,"<token",6) )
                L=LireNoeud(balise,NULL,LesNoeuds,finbalise);

            if (L!=NULL)
                Phrase.ajoute(L);

            //On teste les regles
            if (finbalise==NON)
                if (EndOfSentence(NULL,UTILISE))
                    break;

            //Dans le cas ou l'on a rencontre une FIN annoncee, on s'arrete...
            if (finbalise==2) {
                finbalise=NON;
                break;
            }

            LireUneBalise(balise);

            while (!memcmp(balise,"<dependency",11)) {
                LireDependances(balise,LesNoeuds,StockeVar);
                LireUneBalise(balise);
            }                                   
        }

        
        MajINTERFONCTIONS();
        for (int i=0;i<StockeVar.dernier;i++) {
            StockeVar[i]->var->Affecte(StockeVar[i]->rf->index,StockeVar[i]->val);
            StockeVar[i]->rf->vardep.affecte(StockeVar[i]->var->position,(VariableIndexeeDependance*)StockeVar[i]->var);
        }

        StockeVar.nettoie();

        //Application d'un jeu de regles special pour simplifier 
        //les categories multiples pour un mot donne
        if (TAGGING==OUI || DESAMBIGUISATION==OUI)
            AppliqueSimplification(Phrase,-1);
        if (Phrase.dernier > 0)
            ActiveCouche(Phrase,0);
        ExecuteExtraction();
//        AffichageResultat();
        NettoieVecteursTravail();
        num_phrase++;
        NettoiePhrase();
    }
}

#endif   

    





    
    

        

