#ifdef UNIX
#include <unistd.h>
#include <locale.h>
#include <ctype.h>
#include <sys/socket.h>
#else
#include <io.h>
#endif
#include "parsbin.h"
#include "defines.h"
#include "avlfonc.h"


char rechercheNomFonction(char* nom);

Stocke_Variable* ExtractVariableName(VECTA<Variable*>& Var,char* nom, char* valeur) {
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

//------------------------------------------------------------------------------------
char GlobalParseur::XipFeatureTraits(Traits* tr,char* attribut,char* valeur) {
        
    //recherche du nom de l'attribut
    trait* vattribut=ArbTraits->recherche(attribut);

    if (!vattribut) {
        if (AFFICHE_ERREUR==OUI) {
            sprintf(Chaine_traits,"Feature %s:%s",CHAINES[173],attribut,valeur);
            if (TRACE)
                trace<<Chaine_traits<<Endl;
            insere_erreur(Chaine_traits);
            cerr<<Chaine_traits<<Endl;
        }
        
        return NON;
    }
    
    int indice=RetrouveTrait(vattribut,valeur,NON);
    if (indice!=-1) {
        (tr->valeur)->un(this,indice);
        (tr->filtre)->ou(vattribut->valeur);
        return OUI;
    }
    return NON;
}

//---------------------------------------------------------------------------------------------------
void GlobalParseur::InitialiseVariable(Liste* L,VECTA<Stocke_Variable*>& StockeVar) {
    
    for (int ivar=0;ivar<StockeVar.dernier;ivar++) {
        //On enregiste la valeur dans la variable
        //Premier cas celui d'une variable globale (attachee a aucun noeud)
        VariableGlobale* varglob;
        VariableChaine* varchaine;
        char* pt;
        char* lenom;
        int posnom;
        switch (StockeVar[ivar]->var->lien) {
        case VGLOBALE:
        case VGLOBALEREINIT:
            StockeVar[ivar]->var->AffecteValeur(StockeVar[ivar]->val);
            break;
        case VCHAINE:
            posnom=0;
            varchaine=(VariableChaine*)StockeVar[ivar]->var;
            varchaine->utilisee=OUI;
            lenom=StockeVar[ivar]->nom;
            pt=strstr(lenom,"##");
            if (pt!=NULL) {
                pt=strchr(pt+3,'#');
                posnom=pt-lenom+1;
            }
            varglob=varchaine->insere((uchar*)lenom,posnom);
            varglob->AffecteValeur(StockeVar[ivar]->val);
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
    if (features.dernier==0)
        return NULL;
    
    Stocke_Variable* sv;
    Traits* tr=new Traits(this);
    
    //On traduit les traits...
    for (int ifeat=0;ifeat<features.dernier;ifeat++) {
        if (features[ifeat]->attribute[0]!='#')
            XipFeatureTraits(tr,features[ifeat]->attribute,features[ifeat]->value);
        else {
            sv=ExtractVariableName(Variables,features[ifeat]->attribute+1,features[ifeat]->value);
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
    Traits* tr=BuildXipFeature(noeud->features,StockeVar);
    
    static vocabulaire v;
    ListeVoc* voc=NULL;
    char premier=OUI;
    Traits concatene(this);
    char modificationVoc=NON;
    
    char typeEntree=OUI;   
    FusionX=0;
    
    L=TasDeListes->fournir(this,&cat->X,tr);
    noeud->index=L->position;
    
    //On recopie traitsbase dans traitscourant
    L->SauveTraits();
    
    L->N.gauche=noeud->left;
    L->N.droite=noeud->right;
    char pos[1000];    
    for (int ilem=0;ilem<noeud->lemma.dernier;ilem++) {
        strcpy((char*)v.surface,noeud->surface);
        strcpy((char*)v.lemme,noeud->lemma[ilem]);
        v.init(pere->X,tr);
        
        avl_vocabulaire* traduction=NULL;
        //On regarde aussi, si par hasard le mot n'est pas present aussi dans les lexiques internes
        if (ArbVoc != NULL) {
            avl_vocabulaire* trad=ArbVoc->recherchecasse(v.lemme);
            
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
                            strcpy(v.lemme,(trad->tr)->lemme);
                        if ((trad->tr)->surface!=NULL && (trad->tr)->manipsurface==ECRASE)
                            strcpy(v.surface,(trad->tr)->surface);
                    }
                }
                else
                    traduction=trad;
            }
        }
        
        concatene.ou(v.initiaux);
        int e=0;
        if (voc==NULL) {
            voc=new ListeVoc(v);
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
        if (AffichageReduit==NON || DESAMBIGUISATION==OUI) {
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
                    strcpy(v.lemme,(traduction->tr)->lemme);
                if ((traduction->tr)->surface!=NULL && 
                    (traduction->tr)->manipsurface==ECRASE)
                    strcpy(v.surface,(traduction->tr)->surface);
            }
            else {
                Categorie* cc= Categories[v.X->index];
                v.initiaux=new Traits(this);
                (v.initiaux)->ou(cc);
            }
            
            FusionX|=v.X->Val;            
            if (AffichageReduit==NON || DESAMBIGUISATION==OUI) {
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
        
        pere->N.insere(&L->N);
    }    
}
//-------------------------------------------------------------------------------------
void GlobalParseur::BuildXipNode(Liste* pere,LListe& Phrase,XipNode* noeud) {

    Categorie* cat=NULL;
    VECTA<Stocke_Variable*> StockeVar;
   
    Liste* L;
    Traits* tr=BuildXipFeature(noeud->features,StockeVar);

    //s'il ne s'agit pas d'un noeud lexical, on recherche la categorie...
    if (noeud->daughters.dernier!=0) {
        cat=ChercheCat(noeud->surface);
        if (cat!=NULL)
            tr->ou(cat);
    }
    
    //Le pere du noeud terminal est vide...
    //On cree donc un noeud pere qui pointera sur ce noeud
    //terminal
    L=TasDeListes->fournir(this,&cat->X,tr);
    noeud->index=L->position;
    
    //On recopie traitsbase dans traitscourant
    L->SauveTraits();
    
    L->N.gauche=noeud->left;
    L->N.droite=noeud->right;
    if (pere==NULL)
        Phrase.ajoute(L);
    else
        pere->N.insere(&L->N);
        
    if (noeud->daughters.dernier==0)
        BuildXipNodeLeaf(L,noeud->daughters[0]);
    
    InitialiseVariable(L,StockeVar);
    
    for (int i=0;i<noeud->daughters.size();i++)
        BuildXipNode(L,Phrase,noeud->daughters[i]);
}

//-------------------------------------------------------------------------------------
void GlobalParseur::BuildXipDependency(XipDependency* depend,
                                       VECTA<Stocke_Variable*>& StockeVar) { 
    
    
    Categorie* fonction=ChercheFonc(depend->name);
    VECTA<char*> pile;
    ResultatFonction* rf=FournirResFonc(&fonction->X,NULL);
    rf->numeros.ajoute(99999);

    Stocke_Variable* sv;    
    
    //On traduit les traits...
    for (int ifeat=0;ifeat<depend->features.dernier;ifeat++) {
        char* attribute=depend->features[ifeat]->attribute;
        if (!strcmp(attribute,"$STACK"))
            pile.ajoute(DupliqueChaine(depend->features[ifeat]->value));
        else
            if (attribute[0]=='$') {           
                sv=ExtractVariableName(Variables,attribute+1,depend->features[ifeat]->value);
                if (sv!=NULL)
                    StockeVar.ajoute(sv);
            }
            else
                XipFeatureTraits(rf->traits,attribute,depend->features[ifeat]->value);
    }



    //Lecture des parametres..
    int i;
    for (i=0;i<depend->parameters.size();i++) {
        Noeud* n= &TasDeListes->vecteur[depend->parameters[i]->index]->N;
        rf->arg.affecte(i,n);
        rf->nb_args++;
    }

    for (i=0;i<pile.dernier;i++)
        rf->stack.ajoute(pile[i]);
    
    FONCTIONSINTERMEDIAIRES.ajoute(rf);    
}


void GlobalParseur::BuildXipResult(XipResult* xipres) {
    
    //On relit chaque phrase independemment
    for (int i=0;i<xipres->sentences.dernier;i++) {
        VECTA<Stocke_Variable*> StockeVar;
        //Tout d'abord l'arbre
        XipNode* racine= xipres->sentences[i]->root;
        LListe& Phrase= glisteDesPhrases[MaxCouches]; //les couches vont de 0 a MaxCouches...
        Phrase.dernier=0;
        int id;

        TasDeListes->Raz();       
        BuildXipNode(NULL,Phrase,racine);
        
        for (id=0;id<xipres->sentences[i]->dependencies.size();id++)
            BuildXipDependency(xipres->sentences[i]->dependencies[id],StockeVar);

        for (id=0;i<StockeVar.dernier;id++) {
            StockeVar[id]->var->Affecte(StockeVar[id]->rf->index,StockeVar[id]->val);
            StockeVar[id]->rf->vardep.affecte(StockeVar[id]->var->position,
                (VariableIndexeeDependance*)StockeVar[id]->var);
        }

        StockeVar.nettoie();

        MajINTERFONCTIONS();
        
#ifndef XIP_LIGHT
        //Application d'un jeu de regles special pour simplifier 
        //les categories multiples pour un mot donne
        if (TAGGING==OUI || DESAMBIGUISATION==OUI)
            AppliqueSimplification(Phrase,-1);
#endif            
        if (Phrase.dernier > 0)
            ActiveCouche(Phrase,0);
        ExecuteExtraction();
        NettoieVecteursTravail();
        num_phrase++;
        NettoiePhrase();
    }
}


   

    





    
    

        

