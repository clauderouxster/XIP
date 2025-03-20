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
   filename   : result.cxx
   Date       : 10/01/1999
   Purpose    : Creation of a XipResult (both for VECTA and STL version)
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"
extern string choix_ambigue;

char* TC(GlobalParseur* parseur,Traits* tr);


void ajoutevariablenoeud(Variable* vari, XipNode* node,Noeud* n) {
	static char nm[1000];
	static char ch[100];

	if (vari==NULL)
		return;

	if (vari->sorte==VLEMMEINDEX) {
		//Nous avons beaucoup de noms differents
		VariableLemmeIndex* vli=(VariableLemmeIndex*)vari;
		for (int i=0;i<vli->listes.dernier;i++)
			ajoutevariablenoeud(vli->listes[i], node,n);
        return;
	}

	sprintf(nm,"#%s",vari->nom);
	XipFeature* feat=NULL;
	if (vari->sorte==VCHAINE) {
		VariableString* vs=(VariableString*)vari;
		feat=new XipFeature(nm,vs->chaine);		
	}
	else {
		double res;
		if (vari->sorte!=VNOEUD)
			res= vari->Valeur();
		else
			res = vari->Valeurs(n);

		if (vari->type==REEL)
			sprintf(ch,"%f",res);
		else
			sprintf(ch,"%d",(int)res);

		feat=new XipFeature(nm,ch);
	}

	node->features.AJOUT(feat);
}

void ajouteontologie(GlobalParseur* parseur,XipNode* node,VECTA<ontologie*>* onto) {
	int jk;
	XipFeature* feat;
	vector<string> reponses;

	for (int ix=0;ix<onto->dernier;ix++) {

		onto->cell(ix)->ConcateneValeurs(parseur,parseur->hierarchies->lahierarchie,parseur->TraitsFiltreOntologie,reponses);
		for (int i=0;i<reponses.size();i++) {
			string& ontolog=reponses[i];
			for (jk=0;jk<node->features.size();jk++) {
				if (node->features[jk]->attribute[0]=='@' &&
					node->features[jk]->attribute==ontolog) {
						jk=-1;
						break;
				}
			}
			if (jk!=-1) {
				feat=new XipFeature(ontolog,"+");
				node->features.push_back(feat);
			}
		}
	}
}

//Dans le cas d'une structure STL
XipNode* Noeud::ConstruitResultat(GlobalParseur* parseur,XipUnit* unit) {
    char ch[100];
    char nm[100];
    XipFeature* feat;


    string surf="";
    string lem="";
    if (surface!=NULL)
        surf=ConversionChaine(parseur,surface);
    if (lemme!=NULL)
        lem=ConversionChaine(parseur,lemme);

    XipNode* node;
	string ontolog;
    int i;
    char recursion=OUI;

    if (X!=NULL)
        node= new XipNode(ConversionChaine(parseur,X->nom),gauche,droite,gauchecar,droitecar);        
    else
        node= new XipNode(surf,gauche,droite,gauchecar,droitecar); 
	node->idnode=num_noeud;
	node->infos=infos;
    soeur=node;
	if (source!=NULL) {
		((source->traitsbase)->valeur)->AJOUTTRAITS(parseur,node->features,parseur->FILTRE,(source->traitsbase)->libre);
		if (source->traitsbase->onto!=NULL)
			ajouteontologie(parseur,node,source->traitsbase->onto);
	}

    //On ajoute aussi les numeros de regle
    if (numero!=-1 && parseur->TestOptions(AFFICHAGE_NUMERO_DEPENDANCE)) {        
        sprintf_s(ch,100,"%d",numero);
        strcpy_s(nm,100,"#rule");
        feat=new XipFeature(nm,ch);
        node->features.AJOUT(feat);
    }

    if (source->voc!=NULL) {    

        unit->leaves.AJOUT(node);
		//s'il s'agit d'un noeud final, la recursion s'arrete
		if (fils==NULL || fils->fils==NULL)
			recursion=NON;
        //le premier est forcement deja dans XipNode
        for (i=0;i<source->voc->nbX;i++) {
            //On cree un XipNode racine

            lem=ConversionChaine(parseur,source->voc->lemme[i]);  
            string cat=ConversionChaine(parseur,source->voc->X[i]->nom); 
            string surfa = ConversionChaine(parseur,source->voc->surface);
			string initf;
			if (source->voc->tag[i]!=NULL)
				initf=source->voc->tag[i];

            char trouve=NON;
            XipLeaf* nodefils = new XipLeaf(cat,surfa,lem,initf,gauche,droite,gauchecar,droitecar);
            nodefils->mother=node;
            source->voc->tr[i]->valeur->AJOUTTRAITS(parseur,nodefils->features,parseur->FILTRE,source->voc->tr[i]->libre);
			if (source->voc->tr[i]->onto!=NULL)
				ajouteontologie(parseur,node,source->voc->tr[i]->onto);

            for (size_t j=0;j<node->daughters.size();j++) {                
                XipLeaf* fille = (XipLeaf*)node->daughters[j];
                if (COMPARECHAINE(fille->lemma,lem) &&
                    COMPARECHAINE(fille->category,cat) ) {                    
                    if (fille->features.size()!=nodefils->features.size())
                        continue;
                    trouve=OUI;
                    for (size_t ifeat=0;ifeat<fille->features.size();ifeat++) {
                        if (COMPARECHAINE(fille->features[ifeat]->attribute,nodefils->features[ifeat]->attribute)==0 ||
                            COMPARECHAINE(fille->features[ifeat]->value,nodefils->features[ifeat]->value)==0) {
                            trouve=NON;
                            break;
                        }
                    }
                    if (trouve==OUI)
                        break;
                }
            }

            if (trouve==OUI)
                delete nodefils;
            else
                node->daughters.AJOUT(nodefils);
        }
    }
	else  {
        if (parseur->appliqueGrammaire!=NON) {                       
            for (i=0;i<source->variables.dernier;i++)
				ajoutevariablenoeud(source->variables[i], node,this);
        }
	}

        
    if (recursion==OUI) {
        Noeud* lesfils=fils;
    
        while (lesfils != NULL) {
            XipNode* unFils = lesfils->ConstruitResultat(parseur,unit);
            unFils->mother=node;
            node->daughters.AJOUT(unFils);
            lesfils=lesfils->frere;
        }
    }
    return node;
}

XipDependency* ResultatFonction::ConstruitResultat(GlobalParseur* parseur,XipUnit* unit) {
    int ist;
    XipDependency* depend = new XipDependency(ConversionChaine(parseur,Fx->nom));

	depend->free=(void*)this;
	soeur=depend;
	depend->flag=0;

    if (traits!=NULL)
		(traits->valeur)->AJOUTTRAITS(parseur,depend->features,parseur->FILTREFONCTION,traits->libre);
    
    XipFeature* feat;
    char ch[100];
    char nm[100];

    if (parseur->TestOptions(AFFICHAGE_NUMERO_DEPENDANCE)) {
        for (ist=0;ist<numeros.dernier;ist++) {
            strcpy_s(nm,100,"#rule");
            sprintf_s(ch,100,"%d",numeros[ist]);
            feat=new XipFeature(nm,ch);
            depend->features.AJOUT(feat);
        }
    }
        

    for (ist=0;ist<vardep.dernier;ist++) {
        if (vardep[ist]!=NULL) {
            sprintf_s(nm,100,"$%s",vardep[ist]->nom);
            feat=new XipFeature(nm,vardep[ist]->valdep(this));
            depend->features.AJOUT(feat);
        }
    }

    for (ist=0;ist<stack.dernier;ist++) {
        feat=new XipFeature((char*)"$STACK",stack[ist]);
        depend->features.AJOUT(feat);
    }

    for (int i=0;i<arg.dernier;i++) {
        if (arg[i]->soeur!=NULL)
            depend->parameters.AJOUT(arg[i]->soeur);
        else {//On verifie si le noeud superieur du noeud directement inferieur existe
            Noeud* n=arg[i]->fils;
            while (n!=NULL) {
                if (n->pere!=NULL && n->pere->soeur!=NULL) {
                    depend->parameters.AJOUT(n->pere->soeur);
                    break;
                }
                n=n->frere;
            }
            //sinon, le noeud n'est pas tout simplement pas integre...
        }
    }
    
    return depend;
}


void GlobalParseur::ConstruitResultat(XipResult* xip) {
    
    if (INTERVICTOIRE==NULL)
        return;
    int i;

    XipUnit* unitLing=new XipUnit;
	XipDependency* depend;
    unitLing->root = INTERVICTOIRE->ConstruitResultat(this,unitLing);

    if (langue_de_la_grammaire!=NULL) {
		unitLing->language=langue_de_la_grammaire;
		unitLing->language+=choix_ambigue;
	}
    char generation=NON;
    for (i=1;i<INTERFONCTIONS.dernier;i++) {
        if (INTERFONCTIONS[i]!=NULL) { 
			for (int j=0;j<INTERFONCTIONS[i]->dernier;j++) {
				VECTA<ResultatFonction*>& itf=(*INTERFONCTIONS[i]);
                if (itf[j]!=NULL) {
                    if (itf[j]->Fx!=NULL &&
                        (itf[j]->Fx)->cacher!=OUI) {
							if (itf[j]->Fx->type==C_GENERATION)
								generation=OUI;
                        depend=itf[j]->ConstruitResultat(this,unitLing);
                        unitLing->dependencies.AJOUT(depend);
                    }
                }
			}
        }
    }

	if (generation==OUI) {
		//On remplit les subdependencies avec les bonnes valeurs
		for (i=0;i<unitLing->dependencies.size();i++) {
			ResultatFonction* r=(ResultatFonction*)(unitLing->dependencies[i]->free);
			//S'il s'agit d'une generation
			if (r->Fx->type==C_GENERATION) {
				if (r->frere!=NULL) {
					//On range ses fils dans notre subdependencies
					for (int k=0;k<r->frere->enfants.dernier;k++) {
						//chacun des enfants est associe a un XipDependency
						if (r->frere->enfants[k]->noeud!=NULL) {
							depend=r->frere->enfants[k]->noeud->soeur;
							//C'est lui qu'on range sous notre noeud courant
							if (depend!=NULL) {
								unitLing->dependencies[i]->subdependencies.push_back(depend);
								depend->flag=1;
							}
						}
					}
				}
			}
		}
		vector<XipDependency*> ve;
		for (i=unitLing->dependencies.size()-1;i>=0;i--) {
			if (unitLing->dependencies[i]->flag==0)
				ve.push_back(unitLing->dependencies[i]);
			unitLing->dependencies[i]->free=NULL;
		}
		unitLing->dependencies.clear();
		for (i=0;i<ve.size();i++)
			unitLing->dependencies.push_back(ve[i]);
	}

    xip->sentences.AJOUT(unitLing);
}

void stringjson(stringstream& res, string& value) {
	if (value.find("\\") != -1)
		value = s_replacestrings(value, "\\", "\\\\");

	if (value.find("\"") == -1)
		res << "\"" << value << "\"";
	else
		res << "\"" << s_replacestrings(value, "\"", "\\\"") << "\"";
}


void couple(stringstream& res, string clef, string& val) {
	res << "," << "\"" << clef << "\"" << ":";
	stringjson(res, val);

}

void vcouple(stringstream& res, string clef, string val) {
	res << "," << "\"" << clef << "\"" << ":";
	stringjson(res, val);
}

void icouple(stringstream& res, string clef, int v) {
	res << "," << "\"" << clef << "\"" << ":" << v;
}

void json_ajoutenoeud(stringstream& liste, XipNode* node, map<int, string>& noeuds, int i) {
	stringstream noeud;

	noeud << "{" << "\"#pos\":\""<<node->category<<"\"";

	icouple(noeud, "#id", node->idnode);

	int j;
	for (j = 0; j < node->features.size(); j++)
		couple(noeud, node->features[j]->attribute, node->features[j]->value);

	icouple(noeud, "#left", node->left);
	icouple(noeud, "#right", node->right);

	if (node->leftchar != node->left)
		icouple(noeud, "#leftchar", node->leftchar);

	if (node->rightchar != node->right)
		icouple(noeud, "#rightchar", node->rightchar);

	if (node->Type() == XIPLEAF) {
		XipLeaf* feuille = (XipLeaf*)node;
		couple(noeud, "#lemma", feuille->lemma);
		couple(noeud, "#surface", feuille->surface);
		noeud << "}";
		liste << noeud.str();
		return;
	}

	if (node->daughters.size()) {
		noeud << "," << "\"#children\":[";
		for (j = 0; j < node->daughters.size(); j++) {
			if (j)
				noeud << ",";
			json_ajoutenoeud(noeud, node->daughters[j], noeuds, j);
		}
		noeud << "]";
	}

	noeud << "}";
	noeuds[node->idnode] = noeud.str();
	liste << noeud.str();
}

void json_ajoutedependance(stringstream& liste, map<int, string>& noeuds, XipDependency* d) {
	stringstream dep;
	dep << "{" << "\"#name\":\"" << d->name << "\"";

	int j;
	bool stacks = false;
	bool rule = false;
	for (j = 0; j < d->features.size(); j++) {
		if (d->features[j]->attribute == "$STACK") {
			stacks = true;
			continue;
		}
		if (d->features[j]->attribute == "#rule") {
			rule = true;
			continue;
		}
		vcouple(dep, d->features[j]->attribute, d->features[j]->value);
	}

	if (stacks) {
		dep << ",\"$STACK\":[";		
		for (j = 0; j < d->features.size(); j++) {
			if (d->features[j]->attribute == "$STACK") {
				if (!stacks)
					dep << ",";
				stacks = false;
				stringjson(dep,d->features[j]->value);
			}
		}
		dep << "]";
	}

	if (rule) {
		dep << ",\"#rules\":[";
		for (j = 0; j < d->features.size(); j++) {
			if (d->features[j]->attribute == "#rule") {
				if (!rule)
					dep << ",";
				rule = false;
				stringjson(dep, d->features[j]->value);
			}
		}
		dep << "]";
	}

	dep << ",\"#parameters\":[";
	for (j = 0; j < d->parameters.size(); j++) {
		if (j)
			dep << ",";
		dep << noeuds[d->parameters[j]->idnode];		
	}

	dep << "]}";
	liste << dep.str();
}


void json_ajouteunit(stringstream& unit, XipUnit* xunit) {
	stringstream liste;
	liste << "[";
	map<int, string> noeuds;
	json_ajoutenoeud(liste, xunit->root, noeuds, 1);
	for (int j = 0; j < xunit->dependencies.size(); j++) {
		liste << ",";
		json_ajoutedependance(liste, noeuds, xunit->dependencies[j]);
	}
	liste << "]";
	unit << liste.str();
}

void json_resultat(stringstream& resultat, XipResult* result) {
	resultat << "[";
	for (int i = 0; i < result->sentences.size(); i++) {
		if (i)
			resultat << ",";
		json_ajouteunit(resultat, result->sentences[i]);
	}
	resultat << "]";
}

Exportation string XipUnit::JSon() {
	stringstream resultat;
	json_ajouteunit(resultat, this);
	return resultat.str();

}

Exportation string XipResult::JSon() {
	stringstream resultat;
	json_resultat(resultat, this);
	return resultat.str();
}


