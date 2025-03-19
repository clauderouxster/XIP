/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 4.00
   filename   : xiplibxml.cxx
   Date       : 10/01/1999
   Purpose    : Specific implementation of GlobalParseurXML based on libxml
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifdef XIPLIBXML

#include "parsbin.h"


void majvariablelemme(VariableLemmeIndex* vlemme,Noeud* n,uchar* chaine,int pos,string,double res);    
char rechercheNomFonction(char* nom);
void Trim(char* chaine);
Stocke_Variable* ExtractVariableName(VECTA<Variable*>& Var,char* nom, char* valeur);
Stocke_Variable* RechercheVariable(VECTA<Variable*>& Var,char* nom, char* valeur);
xmlChar* xmlCodage(GlobalParseur* parseur,xmlDocPtr doc,uchar* buff);
//---------------------------------------------------------------------------------------------------

Traits* GlobalParseurXML::BuildFeature(xmlNodePtr features,
                                       VECTA<Stocke_Variable*>& StockeVar,
                                       xmlNodePtr* suivant ) {
    Stocke_Variable* sv;
    Traits* tr=new Traits(this);    
    
    //On traduit les traits...
    while (features!=NULL && !strcmp((char*)features->name,"FEATURE")) {
        xmlAttrPtr attributs=features->properties;
        char* attribute=(char*)attributs->children->content;
        char* value=(char*)attributs->next->children->content;
        if (attribute[0]!='#')
            XipFeatureTraits(tr,attribute,value);
        else {
            sv=ExtractVariableName(Variables,attribute+1,value);
            if (sv!=NULL)
                StockeVar.ajoute(sv);
        }                
        features=features->next->next;
    }

    *suivant=features;

    return tr;
}

void GlobalParseurXML::TraduitChaine(char* chaine) {
    xmlChar* champ = xmlCodage(this,refdocument, (uchar*)chaine);
    strcpy(chaine,(char*)champ);
    xmlFree(champ);
}

//---------------------------------------------------------------------------------------------------
void GlobalParseurXML::BuildNodeLeaf(Liste* pere,xmlNodePtr noeud,VECTA<Liste*>& TableNoeuds) {
    Categorie* cat=NULL;
    VECTA<Stocke_Variable*> StockeVar;
    
    xmlNodePtr token;
    Liste* L;
    
    
    vocabulaire v;
    v.raz();
    ListeVoc* voc=NULL;    
    Traits concatene(this);
    char modificationVoc=NON;
    
    char typeEntree=OUI;   
    FusionX=0;

    xmlAttrPtr attributs=noeud->properties;
    int num=atoi((char*)attributs->children->content);
    Traits* tr=BuildFeature(noeud->children->next,StockeVar,&token);
    long left= atol((char*)attributs->next->next->children->content);
    long right= atol((char*)attributs->next->next->next->children->content);


    L=TasDeListes->fournir(this,pere->X,tr);
    TableNoeuds.affecte(num,L);
    
    //On recopie traitsbase dans traitscourant
    L->SauveTraits();    
    
    L->N.gauche=left;
    L->N.droite=right;
    char pos[1000];    
    char surf[1000];
    //token correspond a un noeud TOKEN
    //sur lequel on trouve des readings en masse    
    //Le contenu du champ text associe au token correspond a la forme de surface
    strcpy_s(surf,1000,(char*)token->children->content);
    Trim(surf);
    TraduitChaine(surf);
    //Premiere lecture
    xmlNodePtr reading=token->children->next;
    while (reading!=NULL) {        
        xmlNodePtr suivant=NULL;
        strcpy_s((char*)v.surface,2048,surf);
        xmlAttrPtr attreading=reading->properties;        
        //Le premier champ correspond a lemma
        strcpy_s((char*)v.lemme,2048,(char*)attreading->children->content);
        TraduitChaine(v.lemme);
        tr=BuildFeature(reading->children->next,StockeVar,&suivant);
        
        //L'attribut suivant correspond a la categorie
        Categorie* cat=ChercheCat((char*)attreading->next->children->content);
        if (cat!=NULL) {
            tr->ou(cat);
            v.init(&cat->X,tr);
        }
        else
            v.init(pere->X,tr);
        
        auto_vocabulaire* traduction=NULL;
        //On regarde aussi, si par hasard le mot n'est pas present aussi dans les lexiques internes        
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
            sprintf_s(pos,1000,"%s\t%s",v.surface,v.lemme);
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
                sprintf_s(pos,1000,"%s\t%s",v.surface,v.lemme);
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
        v.raz();
        reading=reading->next->next;
    }
    
    if (voc!=NULL) {
        L->N.surface=(char*)voc->surface;       
        L->N.lemme=(char*)voc->lemme[0];
        L->N.gauche=left;
        L->N.droite=right;
        
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
        voc->L=L;
        L->index=voc->CalculIndex();
        L->SauveTraits();
        pere->N.fils=&L->N;        
        L->N.pere=&pere->N;

        Liste* Lfils=TasDeListes->fournir(this,voc->X[0],NULL);

        Noeud* nfils=&Lfils->N;

        nfils->surface= (char*)voc->surface;
        nfils->lemme=(char*)voc->lemme[0];
        
        nfils->num_mot=voc->num_mot;
        nfils->clef=(voc->X[0])->Val;
        
        nfils->gauche=left;
        nfils->droite=right;
        
        L->N.fils=nfils;
        nfils->pere=&L->N;

    }    
}
//-------------------------------------------------------------------------------------

void GlobalParseurXML::BuildNode(Liste* pere,
                                 LListe& Phrase,
                                 xmlNodePtr noeud,
                                 VECTA<Liste*>& TableNoeuds) {

    Categorie* cat=NULL;
    VECTA<Stocke_Variable*> StockeVar;
   
    Liste* L;
    xmlAttrPtr attributs=noeud->properties;
    xmlNodePtr suivant;
    Traits* tr=BuildFeature(noeud->children->next,StockeVar,&suivant);
    int num=atoi((char*)attributs->children->content);
    long left= atol((char*)attributs->next->next->children->content);
    long right= atol((char*)attributs->next->next->next->children->content);

    //s'il ne s'agit pas d'un noeud lexical, on recherche la categorie...
    if (!strcmp((char*)noeud->name,"NODE")) {
        //Le tag est deuxieme champ du noeud
        xmlAttrPtr tag=attributs->next;
        cat=ChercheCat((char*)tag->children->content);
        if (cat!=NULL)
            tr->ou(cat);
        
    }

    //Le pere du noeud terminal est vide...
    //On cree donc un noeud pere qui pointera sur ce noeud
    //terminal
    L=TasDeListes->fournir(this,&cat->X,tr);
    L->N.gauche=left;
    L->N.droite=right;

    
    TableNoeuds.affecte(num,L);
    
    //On recopie traitsbase dans traitscourant
    L->SauveTraits();
    
    if (pere==NULL)
        Phrase.ajoute(L);
    else {
        if (pere->N.fils==NULL) {
            pere->N.fils=&L->N;
            L->N.pere=&pere->N;
        }
        else {
            pere->N.fils->insere(&L->N);                
            L->N.pere=&pere->N;
        }
    }
    
    InitialiseVariable(L,StockeVar);

    while (suivant!=NULL) {
        if (!strcmp((char*)suivant->name,"NODE")) 
            BuildNode(L,Phrase,suivant,TableNoeuds);
        else
            BuildNodeLeaf(L,noeud,TableNoeuds);      
        suivant=suivant->next->next;
    }
}

//-------------------------------------------------------------------------------------

void GlobalParseurXML::BuildDependency(xmlNodePtr dependencies,
                                       VECTA<Stocke_Variable*>& StockeVar,
                                       VECTA<Liste*>& TableNoeuds) { 
    
    
    xmlAttrPtr attributs=dependencies->properties;
    xmlNodePtr parameters;
    Categorie* fonction=ChercheFonc((char*)attributs->children->content);
    VECTA<char*> pile;
    ResultatFonction* rf=FournirResFonc(&fonction->X,NULL);
    rf->numeros.ajoute(99999);
    
    Stocke_Variable* sv;       
    
    xmlNodePtr features=dependencies->children->next;
    //On traduit les traits...
    while (features!=NULL && !strcmp((char*)features->name,"FEATURE")) {
        xmlAttrPtr attributs=features->properties;
        char* attribute=(char*)attributs->children->content;
        char* value=(char*)attributs->next->children->content;
        if (!strcmp(attribute,"$STACK"))
            pile.ajoute(DupliqueChaine(value));
        else
            if (attribute[0]=='$') {           
                sv=ExtractVariableName(Variables,attribute+1,value);
                if (sv!=NULL)
                    StockeVar.ajoute(sv);
            }
            else
                XipFeatureTraits(rf->traits,attribute,value);            
        features=features->next->next;
    }
    
    parameters=features;

    //Lecture des parametres..
    int i=0;
    while (parameters!=NULL && !strcmp((char*)parameters->name,"PARAMETER")) {
        attributs=parameters->properties;
        int num=atoi((char*)attributs->next->children->content);
        Noeud* n= &TableNoeuds[num]->N;
        rf->arg.affecte(i,n);
        rf->nb_args++;
        parameters=parameters->next->next;
        i++;
    }

    for (i=0;i<pile.size();i++)
        rf->stack.ajoute(pile[i]);
    
    FONCTIONSINTERMEDIAIRES.ajoute(rf);    
}


//-------------------------------------------------------------------------------------
void GlobalParseurXML::AnalyseXipXMLFilename(char* filename) {
    
    /*
     * build an XML tree from a file;
     */
    xmlDocPtr doc= xmlParseFile(filename);           
    if (doc == NULL)  {
        sprintf_s(Chaine_traits,3000,"Unknown filename:%s\n",filename);
        erreur(Chaine_traits);
    }
    renseigne(filename);
    AnalyseDOCXML(doc);
    xmlFreeDoc(doc);
}

void GlobalParseurXML::AnalyseXipXMLString(unsigned char* chaine) {

    /*
     * build an XML tree from a string;
     */
    xmlDocPtr doc= xmlSAXParseMemory(NULL,(const char*)chaine,strlen((char*)chaine),0);
    AnalyseDOCXML(doc);
    xmlFreeDoc(doc);
}

void GlobalParseurXML::AnalyseDOCXML(xmlDocPtr doc) {
    xmlAttrPtr attributs;
    VECTA<Stocke_Variable*> StockeVar;
    
    //On verifie s'il s'agit bien d'un fichier XML result
    xmlNodePtr noeud=doc->children;

    if (strcmp((char*)noeud->name,"XIPRESULT")) {        
        if (doc->URL!=NULL && doc->URL[0])
            sprintf_s(Chaine_traits,3000,"Unknown XML file type:%s: %s\n",doc->URL,noeud->name);
        else
            sprintf_s(Chaine_traits,3000,"Unknown XML type: %s\n",noeud->name);
        xmlFreeDoc(doc);
        erreur(Chaine_traits);
    }

    attributs = noeud->properties;
    //S'il y a des variables a lire
    if (!strcmp((char*)attributs->children->content,"1")) {
        xmlNodePtr math=noeud->next->next;
        while (math!=NULL && strcmp((char*)math->name,"VARIABLES"))
            math=math->next->next;

        if (!strcmp((char*)math->name,"VARIABLES")) {
            xmlNodePtr features=math->children->next;            
            while (features!=NULL && !strcmp((char*)features->name,"FEATURE")) {
                xmlAttrPtr attributs=features->properties;
                char* attribute=(char*)attributs->children->content;
                char* value=(char*)attributs->next->children->content;

                Stocke_Variable* sv=RechercheVariable(Variables,attribute+1,value);
                if (sv!=NULL)
                    StockeVar.ajoute(sv);                
                features=features->next->next;
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
        }
    }
    
    //On se position sur le premier noeud LUNIT
    noeud=noeud->children->next;
    VECTA<Liste*> TableNoeuds;
    xmlNodePtr dependences;
    //On relit chaque phrase independemment
    while (noeud!=NULL && !strcmp((char*)noeud->name,"LUNIT")) {
        
        //Tout d'abord l'arbre
        xmlNodePtr racine=noeud->children->next;
        TableNoeuds.raz();        
        
        LListe& Phrase= glisteDesPhrases[0]; //les couches vont de 0 a MaxCouches...
        Phrase.dernier=0;
        int id;

        
        TasDeListes->Raz();       
        BuildNode(NULL,Phrase,racine,TableNoeuds);
        dependences=racine->next->next;
        while (dependences != NULL && !strcmp((char*)dependences->name,"DEPENDENCY")) {
            BuildDependency(dependences,StockeVar,TableNoeuds);
            dependences=dependences->next->next;
        }

        for (id=0;id<StockeVar.size();id++) {
            StockeVar[id]->var->Affecte(StockeVar[id]->rf->index,StockeVar[id]->val);
            StockeVar[id]->rf->vardep.affecte(StockeVar[id]->var->position,
                (VariableIndexeeDependance*)StockeVar[id]->var);
        }

        StockeVar.nettoie();
        MajINTERFONCTIONS();

        TVecteur laClef=0;
        int nbn=1;
        for (int i=Phrase.dernier-1;i>=0;i--) {
            Liste* L=Phrase[i];
            laClef|=L->index;
            L->N.clef_suivant=laClef;
            L->N.nombre=nbn++;
        }
        

        //Application d'un jeu de regles special pour simplifier 
        //les categories multiples pour un mot donne
        if (TAGGING==OUI || DESAMBIGUISATION==OUI)
            AppliqueSimplification(Phrase,-1);

        if (Phrase.size() > 0)
            ActiveCouche(Phrase,0);
        ExecuteExtraction();
        
        NettoiePhrase();
        NettoieVecteursTravail();
        noeud=noeud->next->next;
        num_phrase++;            
    }    
}

//-------------------------------------------------------------------------------------

 
#endif
   
    





    
    


        



