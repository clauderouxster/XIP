    
    EtatArbre* boucle=this;
    EtatArbre* et;

    Liste* L;
    char auMoinsUnNoeud=NON;

    VECTA<SousArbre*> pile(0);

    int premierN=iListe;
    int dernierN=iListe;
    int icourant;
    char solution;

    if (chemin==-1) {
        chemin=NON;      
        if (r!=NULL)
            r->fin=-1;
    }

    char erreur=NON;
    int bp=pile.dernier;

    while (chemin!=OUI && bc<boucle->arcs.dernier) {

        L=Phrase[iListe];
        Noeud* n=L->N;
        if (L==NULL) {
            if (boucle->rechercheCheminEpsilon(ct.parseur->XCatEpsilon)==OUI) {
                etatfinal=1;
                chemin=OUI;
                break;
            }
            DepileAmb(pile,-1);
            return;
        }

        int sp=pile.dernier;
        ar=boucle->arcs[bc];
        et=ar->etatarbre;
        bc++;

        //Les comparaisons ne sont executes qu'une seule fois
        if (et->marque==n->num_noeud)
            continue;
        
        if ((n->clef_suivant & ar->clefsuivante) != ar->clefsuivante)
            continue;

        nombre_boucle_sequence++;
        erreur=OUI;        
        int isuivant=iListe+increment;
        char boucleinterne=NON;
        //S'il s'agit d'une boucle sur soi-meme, on peut faire une derecursivation locale
        if (isuivant<Phrase.dernier && ar->type==1 && maxbc>=2 && maxbc==bc && boucle->fils==NULL) {
            //Dans ce cas, nous verifions la presence d'un etat epsilon a la suite
            //de cet etat arbre qui soit unique
            ArcArbre* artest=boucle->arcs[0];            
            EtatArbre* suivant=boucle;            
            if (artest->X==ct.parseur->XCatEpsilon) {
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
                sp=pile.nb;
                st=stacks.dernier();                                  
                while (isuivant<Phrase.dernier) {                  
                    Liste* lsuivant=Phrase[isuivant];
                    //Tout d'abord, est-ce l'element que l'on attendait?
                    if (parseur->CompareNoeudSousArbre(artest,lsuivant,maj,couverture)) {                    
                        //Il y a donc une solution, on sort
                        boucleinterne=OUI;
                        //Tout d'abord, on enregistre le contexte courant d'analyse
                        //ces deux elements deviennent le noeud courant
                        et=suivant;
                        ar=artest;
                        icourant=iListe;
                        courant=boucle;
                        boucle=precedent;
                        etatfinal=boucle->etatfin;
                        //On a deja calcule la comparaison Noeud-arc
                        calculComparaison=NON;                        
                        iListe=isuivant+1;                        
                        L=Phrase[iListe];
                        break;
                    }
                    //Sinon, a-t-on le droit de boucler dessus?
                    if (parseur->CompareNoeudSousArbre(ar,lsuivant,maj,couverture))                    
                        //dans ce cas, il n'y a pas de solution
                        boucleinterne=SAUTE;
                        break;
                    }
                    isuivant++;
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
            char couverture=NON;
            
            if (parseur->CompareNoeudSousArbre(ar,L,maj,couverture)) {
                erreur=NON;
                Noeud* n=&L->N;
                /*
                if (ar->fils != NULL) {
                    if (n->fils != NULL && (n->fils)->fils!=NULL)
                        if (((ar->clef & n->clef) == ar->clef) &&
                            PreTestCategorie(parseur,ar->fils,(n->fils)->X) != -1) {
                            erreur=OUI;
                            solution=NON;
                            int st=pile.dernier; 
                            static Piles locstacks;
                            locstacks.raz();
                            (ar->fils)->Marquage(parseur,solution,n->fils,2,locstacks,NULL);
                            if (solution) {
                                if (noeuds_arguments!=NULL) {
                                    for (int ipile=0;ipile<locstacks.dernier();ipile++)
                                        if (locstacks.noeuds[ipile]->marquage!=-1)
                                            noeuds_arguments->affecte(locstacks.noeuds[ipile]->marquage,locstacks.noeuds[ipile]);
                                }                                        
                                erreur=NON;
                            }
                            else {
                                DepileAmb(pile,st);                            
                                L->instanciateur=NULL;
                                return;
                            }
                        }
                }
                */
                if (maj==OUI && majCategorie!=NULL && lePremier!=-1) 
                    majCategorie->affecte(iListe-lePremier,couverture);
            }
            
            if (erreur==OUI && bc==maxbc) {
                L->instanciateur=NULL;
                DepileAmb(pile,-1);
                return;
            }    
            
            if (noeuds_arguments!=NULL && ar->argument!=-1)
                noeuds_arguments->affecte(ar->argument,&L->N);

            //les ar de ce type permettent la desamb totale d'un noeud dans la liste des mots
            if (ar->desamb==OUI && erreur==NON) {
                ar->numliste.ajoute(iListe);
                pile.ajoute(ar);
            }

            auMoinsUnNoeud=OUI;
        }


        //Remarque, dans le cas d'un element optionnel (A* ou (A) ), le systeme
        //integre automatiquement l'element suivant (ar->suivant) dans la regle en premiere position
        //Si l'analyse est longue, cet element suivant devra etre le dernier a etre
        //analysee
        sp=pile.dernier;
        if (maxbc==1 && boucleinterne==NON) {
            if (erreur==OUI)
                return;
            //Dans certain cas, on peut directement sauter un etat
            //lorsque celui-ci est un simple epsilon conduisant a un autre 
            //epsilon            
            while (et->arcs.dernier==1 && et->arcs[0]->X==ct.parseur->XCatEpsilon)
                et=et->arcs[0]->etatarbre;
            //c'est une simple boucle, nul besoin de recursion
            boucle=et;
            bc=0;
            maxbc=boucle->arcs.dernier;
            n=nsuivant;
            etatfinal=boucle->etatfin;
        }//sinon, cas plus complexe
        else {    
            if (erreur==NON) {
                if (Phrase[isuivant]==NULL) {                                           
                    //dans ce cas, il est possible d'avoir des etats epsilons de saut (type!=1)
                    if (et->rechercheCheminEpsilon(ct.parseur->XCatEpsilon)==OUI) {
                        etatfinal=OUI;
                        solution=OUI;
                        if (r!=NULL && r->fin < dernierN)
                            r->fin=dernierN;                    
                    }
                }
                else {
                    if (PreTestEtatCategorie(parseur,et,Phrase[isuivant],calculTest)==OUI) {
                        boucle->marque=n->num_noeud;
                        //Dans certain cas, on peut directement sauter un etat
                        //lorsque celui-ci est un simple epsilon conduisant a un autre 
                        //epsilon
                        while (et->arcs.dernier==1 && et->arcs[0]->X==ct.parseur->XCatEpsilon)
                            et=et->arcs[0]->etatarbre;
                        et->AppliqueLineaire(parseur,solution,
                                 Phrase,
                                 isuivant,
                                 increment,                                 
                                 maj,
                                 r,
                                 noeuds_arguments,
                                 majCategorie,
                                 lePremier);
                        boucle->marque=-1;
                    }
                    
                }           
                if (solution==OUI) {            
                    chemin=OUI;
                    return;
                }                
                //On revient au status precedent
                if (Phrase[isuivant])
                    Phrase[isuivant]->instanciateur=NULL;
                //On retire les numeros de mot a desambiguiser rajoute lors de la recursion
                DepileAmb(pile,sp);                
            }
        }


            
          
            if (solution) {
                chemin=solution;
                return;
            }

            //On revient au status precedent
            if (Phrase[isuivant])
                Phrase[isuivant]->instanciateur=NULL;
            //On retire les numeros de mot a desambiguiser rajoute lors de la recursion
            DepileAmb(pile,sp);
        }

        //Quatre possibilites d'analyse:
        // AR, (AR), AR+, AR*
        // Dans le cas d'un element optionnel, ou d'une boucle
        
        if (ar->derivations.dernier!=0 || erreur==OUI) {
            L->instanciateur=NULL;
            DepileAmb(pile,-1);
            return;
        }
        
        dernierN=iListe;

        iListe+=increment;
        ar=ar->suivant;
    }

    if (erreur==OUI || dernierN==-1) {
        DepileAmb(pile,-1);
        return;
    }

    chemin=OUI;
    if (r!=NULL && auMoinsUnNoeud==OUI && r->fin<dernierN)
        r->fin=dernierN;
}
