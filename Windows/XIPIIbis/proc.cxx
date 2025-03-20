char EtatArbre::AppliqueLineaire(ContexteAnalyse& ct,Noeud* n) {
    ArcArbre* ar;
    EtatArbre* boucle=this;
    int bc;
    int debutbc=0;
    int maxbc=arcs.dernier;
    
   //Pas d'elements lexicaux a ce niveau d'analyse
    if (n!=NULL && n->terminal())
        return NON;   

    char erreur=NON;                
    Noeud* nsuivant;
    EtatArbre* et;

    if (ct.borne!=-1) {
        debutbc=ct.borne;
        maxbc=ct.borne+1;
        ct.borne=-1;
    }


    bc=debutbc;
    EtatArbre* courant=NULL;
    
    while (bc<maxbc) {
        

        if (n==NULL) {                                           
            //dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
            if (boucle->rechercheCheminEpsilon(ct.parseur->XCatEpsilon)==OUI)
                return OUI;            
            return NON;
        }
        
        ar=boucle->arcs[bc];
        et=ar->etatarbre;
        bc++;
        //Les comparaisons ne sont executes qu'une seule fois
        if (et->marque==n->num_noeud)
            continue;
        
        if ((n->clef_suivant & ar->clefsuivante) != ar->clefsuivante)
            continue;                        
                
        erreur=OUI;
        
        if (ar->profondeurNonBornee==OUI)
            return OUI;
        
        nsuivant=n->frere;
        char boucleinterne=NON;
        
        //S'il s'agit d'une boucle sur soi-meme, on peut faire une derecursivation locale
        if (nsuivant!=NULL && ar->type==1 && ar->etatarbre==boucle && maxbc==2 && boucle->fils==NULL) {
            //Dans ce cas, nous verifions la presence d'un etat epsilon a la suite
            //de cet etat arbre qui soit unique
            ArcArbre* artest=boucle->arcs[2-bc];            
            EtatArbre* suivant=boucle;            
            if (artest->type==0 && artest->X==ct.parseur->XCatEpsilon) {
                suivant=artest->etatarbre;
                if (suivant!=NULL && suivant->arcs.dernier==1)
                    artest=suivant->arcs[0];                
            }
            
            
            //Enfin il faut aussi n'avoir qu'un seul etat a la suite...
            if (artest->X!=ct.parseur->XCatEpsilon) {
                //dans ce cas, il s'agit d'un cas parfait de boucle
                //Le test NULL est deja pris en compte par l'epsilon                  
                EtatArbre* precedent=suivant;
                suivant=artest->etatarbre;
                //Test de continuation                                
                while (nsuivant!=NULL) {
                    //Tout d'abord, est-ce l'element que l'on attendait?
                    if (ct.parseur->CompareNoeudSousArbre(artest,nsuivant)) {
                        //Il y a donc une solution, on sort
                        boucleinterne=OUI;
                        //Tout d'abord, on enregistre le contexte courant d'analyse
                        //ces deux elements deviennent le noeud courant
                        et=suivant;
                        ar=artest;
                        courant=boucle;
                        boucle=precedent;
                        //On a deja calcule la comparaison Noeud-arc
                        n=nsuivant;
                        nsuivant=nsuivant->frere;
                        break;
                    }
                    //Sinon, a-t-on le droit de boucler dessus?
                    if (!ct.parseur->CompareNoeudSousArbre(ar,nsuivant)) {
                        //dans ce cas, il n'y a pas de solution
                        boucleinterne=SAUTE;
                        break;
                    }
                    nsuivant=nsuivant->frere;
                }                
            }
            //Dans le cas d'une erreur, on passe a l'arc suivant
            if (boucleinterne==SAUTE)
                continue;
        }

        char testNoeud=NON;
        char ajoute=NON;
        
        
        if (ar->X==ct.parseur->XCatEpsilon) {
            erreur=NON;
            nsuivant=n;            
        }
        else {
            //Le test sur les traits du noeud (en recherche non  bornee) ne
            //peut s'effectuer que sur le noeud le plus haut
            if (ct.parseur->CompareNoeudSousArbre(ar,n)==OUI)            
                erreur=NON;
        }
        
        if (maxbc==1 && boucleinterne==NON) {
            if (erreur==OUI)
                return NON;
            //c'est une simple boucle, nul besoin de recursion
            boucle=et;
            bc=0;
            maxbc=boucle->arcs.dernier;
            n=nsuivant;
        }//sinon, cas plus complexe
        else {    
            if (erreur==NON) {
                if (nsuivant==NULL) {                                           
                    //dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
                    if (et->rechercheCheminEpsilon(ct.parseur->XCatEpsilon)==OUI)
                        return OUI;
                }
                else {
                    //dans le cas d'un saut epsilon 
                    //On ne peut passer par la qu'une seule fois                    
                    boucle->marque=n->num_noeud;                    
                    char chemin=et->AppliqueLineaire(ct,nsuivant);                               
                    boucle->marque=-1;
                    if (chemin==OUI)
                        return OUI;
                }            
            }
        }

        //Dans le cas d'une boucle interne, on reboucle sur le meme arc avec n=nsuivant
        if (boucleinterne==OUI) {
            boucle=courant;
            //On retrouve l'arc precedent 
            bc--;            
        }
    }

    if (boucle->etatfin!=1)
        return NON;
    return OUI;
}
