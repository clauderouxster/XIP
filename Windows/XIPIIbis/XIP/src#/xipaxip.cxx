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
   filename   : xipaxip.cxx
   Date       : 10/01/1999
   Purpose    : Reuse of a XipResult as starting point for a new parse
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifdef UNIX
#include <unistd.h>
#include <locale.h>
#include <ctype.h>
#include <sys/socket.h>
#else
#include <io.h>
#endif
#include "parsbin.h"
#include "avlfonc.h"

extern ostream* flot_erreur;

void majvariablelemme(VariableLemmeIndex* vlemme,Noeud* n,uchar* chaine,int pos,string,double res);    
char rechercheNomFonction(char* nom);

Stocke_Variable* ExtractVariableName(VECTA<Variable*>& Var,char* nom, char* valeur) {
    char c;

    double vf=atof(valeur);
    char* pt=strstr(nom,"##");
    if (pt!=NULL) {
        c=pt[2];
        pt[2]=0;
    }
    
    for (int i=0;i<Var.size();i++) {
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


//------------------------------------------------------------------------------------
void GlobalParseur::AffecteSousNoeuds(Noeud* n,trait* attribut,trait* valeur,ontologie* v) {
	if (n==NULL || n->source==NULL)
		return;

	Traits* tb=n->source->traitsbase;
	Traits* ta=n->source->traitsactif;

	if (v!=NULL) {
		if (tb->onto==NULL)
			tb->onto=new VECTA<ontologie*>;
		tb->onto->cell(ONTOLOGIE_COMPARAISON)->ou(v);
	
		if (ta->onto==NULL)
			ta->onto=new VECTA<ontologie*>;
		ta->onto->cell(ONTOLOGIE_COMPARAISON)->ou(v);
	}
	else {
		tb->inittrait(attribut,valeur);
		ta->inittrait(attribut,valeur);
	}
	Noeud* f=n->fils;
	while (f!=NULL) {
		AffecteSousNoeuds(f,attribut,valeur,v);
		f=f->frere;
	}
}

char GlobalParseur::XipFeatureSubTraits(Noeud* n,char* attribut,char* valeur) {
	trait* att=NULL;
	trait* val=NULL;
	ontologie* v=NULL;
	if (attribut[0]=='@') {
        v=OntologieTrait(attribut);
		if (v==NULL)
			return NON;
	}
	else {
		att=ArbTraits->recherche(attribut);   
		if (att==NULL)
			return NULL;
		val=RetrouveValeurTrait(att,valeur,NON);
	}

	AffecteSousNoeuds(n,att,val,v);
	if (v!=NULL)
		delete v;
	return OUI;
}

//---------------------------------------------------------------------------------------------------
void GlobalParseur::InitialiseVariable(Liste* L,VECTA<Stocke_Variable*>& StockeVar) {
    
    for (int ivar=0;ivar<StockeVar.size();ivar++) {
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
            if (L!=NULL) {       
                StockeVar[ivar]->var->Affecte(L->position,StockeVar[ivar]->val);
                //On enregiste la variable dans le noeud
                L->variables.affecte(StockeVar[ivar]->var->position,StockeVar[ivar]->var);
            }
        }
    }
    
    StockeVar.nettoie();
}

//---------------------------------------------------------------------------------------------------

Traits* GlobalParseur::BuildXipFeature(VXipFeature& features,VECTA<Stocke_Variable*>& StockeVar) {    
    Stocke_Variable* sv;
    Traits* tr=new Traits(this);
    
    //On traduit les traits...
    for (int ifeat=0;ifeat<features.size();ifeat++) {
        char* attribute=VALEURCHAINE(features[ifeat]->attribute);
        char* value=VALEURCHAINE(features[ifeat]->value);
        if (attribute[0]!='#')
            XipFeatureTraits(tr,attribute,value);
        else {
            sv=ExtractVariableName(Variables,attribute+1,value);
            if (sv!=NULL)
                StockeVar.ajoute(sv);
        }                
    }
    return tr;
}

//---------------------------------------------------------------------------------------------------
void GlobalParseur::BuildXipNodeLeaf(Liste* pere,XipNode* noeud) {
    Categorie* cat=NULL;
    VECTA<Stocke_Variable*> StockeVar;
    
    Liste* L;
    
    
    vocabulaire v;
    v.raz();
    ListeVoc* voc=NULL;    
    Traits concatene(this);
    char modificationVoc=NON;
    
    char typeEntree=OUI;   
    FusionX=0;
    Traits* tr=BuildXipFeature(noeud->features,StockeVar);
    L=TasDeListes->fournir(this,pere->X,tr);
    noeud->index=L->position;
    
    //On recopie traitsbase dans traitscourant
    L->SauveTraits();
    
    
    L->N.gauche=noeud->left;
    L->N.droite=noeud->right;
    char pos[1000];    
    for (int ilem=0;ilem<noeud->daughters.size();ilem++) {
        XipLeaf* feuille=(XipLeaf*)noeud->daughters[ilem];
        strcpy((char*)v.surface,VALEURCHAINE(feuille->surface));
        strcpy((char*)v.lemme,VALEURCHAINE(feuille->lemma));
        tr=BuildXipFeature(feuille->features,StockeVar);
        
        Categorie* cat=ChercheCat(VALEURCHAINE(feuille->category));
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
        v.raz();
    }    
    
    if (voc!=NULL) {
        L->N.surface=(char*)voc->surface;       
        L->N.lemme=(char*)voc->lemme[0];
        L->N.gauche=noeud->left;
        L->N.droite=noeud->right;
        
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
        
        nfils->gauche=noeud->left;
        nfils->droite=noeud->right;
        
        L->N.fils=nfils;
        nfils->pere=&L->N;

    }    
}
//-------------------------------------------------------------------------------------
void GlobalParseur::BuildXipNode(Liste* pere,LListe& Phrase,XipNode* noeud) {

    Categorie* cat=NULL;
    VECTA<Stocke_Variable*> StockeVar;
   
    Liste* L;
    Traits* tr=BuildXipFeature(noeud->features,StockeVar);

    //s'il ne s'agit pas d'un noeud lexical, on recherche la categorie...
    if (noeud->daughters.size()!=0) {
        cat=ChercheCat(VALEURCHAINE(noeud->category));
        if (cat!=NULL)
            tr->ou(cat);
    }
    
    //Le pere du noeud terminal est vide...
    //On cree donc un noeud pere qui pointera sur ce noeud
    //terminal
    L=TasDeListes->fournir(this,&cat->X,tr);
    L->N.gauche=noeud->left;
    L->N.droite=noeud->right;

    noeud->index=L->position;
    
    //On recopie traitsbase dans traitscourant
    L->SauveTraits();
    
    L->N.gauche=noeud->left;
    L->N.droite=noeud->right;
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
    
    for (int i=0;i<noeud->daughters.size();i++) {
        if (noeud->daughters[i]->Type()==XIPLEAF)
            BuildXipNodeLeaf(L,noeud);      
        else
            BuildXipNode(L,Phrase,noeud->daughters[i]);
    }
}

//-------------------------------------------------------------------------------------
void GlobalParseur::BuildXipDependency(XipDependency* depend,
                                       VECTA<Stocke_Variable*>& StockeVar) { 
    
    
    Categorie* fonction=ChercheFonc(VALEURCHAINE(depend->name));
    VECTA<char*> pile;
    ResultatFonction* rf=FournirResFonc(&fonction->X,NULL);
    rf->numeros.ajoute(99999);

    Stocke_Variable* sv;    
    
    //On traduit les traits...
    for (int ifeat=0;ifeat<depend->features.size();ifeat++) {
        char* attribute=VALEURCHAINE(depend->features[ifeat]->attribute);
        char* value=VALEURCHAINE(depend->features[ifeat]->value);
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
    }



    //Lecture des parametres..
    int i;
    for (i=0;i<depend->parameters.size();i++) {
        Noeud* n= &TasDeListes->vecteur[depend->parameters[i]->index]->N;
        rf->arg.affecte(i,n);
        rf->nb_args++;
    }

    for (i=0;i<pile.size();i++)
        rf->stack.ajoute(pile[i]);
    
    FONCTIONSINTERMEDIAIRES.ajoute(rf);    
}


//-------------------------------------------------------------------------------------

void GlobalParseur::AnalyseXipResult(XipResult* xipbase) {
    
    //On relit chaque phrase independemment
    for (int i=0;i<xipbase->sentences.size();i++) {
        VECTA<Stocke_Variable*> StockeVar;
        //Tout d'abord l'arbre
        XipNode* racine= xipbase->sentences[i]->root;
        LListe& Phrase= glisteDesPhrases[0]; //les couches vont de 0 a MaxCouches...
        Phrase.dernier=0;
        int id;

        
        TasDeListes->Raz();       
        BuildXipNode(NULL,Phrase,racine);

        for (id=0;id<xipbase->sentences[i]->dependencies.size();id++)
            BuildXipDependency(xipbase->sentences[i]->dependencies[id],StockeVar);

        for (id=0;i<StockeVar.size();id++) {
            StockeVar[id]->var->Affecte(StockeVar[id]->rf->index,StockeVar[id]->val);
            StockeVar[id]->rf->vardep.affecte(StockeVar[id]->var->position,
                (VariableIndexeeDependance*)StockeVar[id]->var);
        }

        StockeVar.nettoie();
        MajINTERFONCTIONS();

        TVecteur laClef=0;
        int nbn=1;
        for (i=Phrase.dernier-1;i>=0;i--) {
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
        num_phrase++;        
    }
}



//-------------------------------------------------------------------------------------

void GlobalParseur::AnalyseXipNode(XipNode* racine) {
 

    if (racine->daughters.size()==0)
        return;

    INTERVICTOIRE=NULL;    
    gBaliseDebut->raz();
    gBaliseTraits->raz();
    memoiresem.max++;

    int i;
    LListe& Phrase= glisteDesPhrases[0]; //les couches vont de 0 a MaxCouches...
    Phrase.dernier=0;
    
    TasDeListes->Raz();
    LINTERVICTOIRE = TasDeListes->fournir(this,&Categories[1]->X,NULL);

    //Nous devons construire une structure sur les fils...
    for (i=0;i<racine->daughters.size();i++)
        BuildXipNode(NULL,Phrase,racine->daughters[i]);   
    
    TVecteur laClef=0;
    int nbn=1;
    for (i=Phrase.dernier-1;i>=0;i--) {
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
    num_phrase++;
    
}


void GlobalParseur::AnalyseXipLeaves(XipUnit* unite) {
    

    if (unite->leaves.size()==0)
        return;

    INTERVICTOIRE=NULL;    
    gBaliseDebut->raz();
    gBaliseTraits->raz();
    memoiresem.max++;

    LListe& Phrase= glisteDesPhrases[0]; //les couches vont de 0 a MaxCouches...
    Phrase.dernier=0;
    int i;
    TasDeListes->Raz();
    LINTERVICTOIRE = TasDeListes->fournir(this,&Categories[1]->X,NULL);
    
    //Nous devons construire une structure sur les fils...
    for (i=0;i<unite->leaves.size();i++) {
        int dernier=Phrase.dernier;
        BuildXipNode(NULL,Phrase,unite->leaves[i]);
        if (Phrase[dernier]!=NULL)
           Phrase.affecte(dernier,Phrase[dernier]->N.fils->source);
    }

    TVecteur laClef=0;
    int nbn=1;
    for (i=Phrase.dernier-1;i>=0;i--) {
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
    num_phrase++;
    
}

   
void GlobalParseur::AnalyseXipVECTA(VECTA<XipNode*>& unite) {
    

    if (unite.size()==0)
        return;

    INTERVICTOIRE=NULL;
    gBaliseDebut->raz();
    gBaliseTraits->raz();
    memoiresem.max++;   

    LListe& Phrase= glisteDesPhrases[0]; //les couches vont de 0 a MaxCouches...
    Phrase.dernier=0;
    int i;
    TasDeListes->Raz();       
    LINTERVICTOIRE = TasDeListes->fournir(this,&Categories[1]->X,NULL);
    
    //Nous devons construire une structure sur les fils...
    for (i=0;i<unite.size();i++) {
        int dernier=Phrase.dernier;
        BuildXipNode(NULL,Phrase,unite[i]);
        if (Phrase[dernier]!=NULL)
           Phrase.affecte(dernier,Phrase[dernier]->N.fils->source);
    }

    TVecteur laClef=0;
    int nbn=1;
    for (i=Phrase.dernier-1;i>=0;i--) {
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
    num_phrase++;
    
}

   
    





    
    


        

