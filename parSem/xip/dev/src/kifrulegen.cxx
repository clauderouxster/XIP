/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003, Xerox Corporation, XRCE-Grenoble.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIPWIN
   Version    : 3.16
   filename   : kifrulegen.cpp
   Date       : 10/01/2000
   Purpose    : To kifrulegen a rule
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"
#include "kifrulegen.h"
Exportation char XipMessageLanguage(unsigned int ipar);

static char* kif_field_name[] = {"",
                      "sequence:",
                      "idrules:",
                      "tagging:",
                      "extraction:",
                      "dependencyrules:",
                      "semantic:",
                      "marker:",
                      "terms:",
                      "reshuffling:",
                      "reshuffling:",
                      "split:",
                      "doublereduction:",
                      "",
                      "fragment:",
                      "vocabulary:",
                      "lprules:",
                      "functions:",
					  "multiwords:",
					  "multilemmas:",
					  "tokenselection:",
					  "lexicals:",
					  "generation:",
					  "order:",
					  "sequenceoncontext:"};

static char* kif_noms_champ[] = {"",
                      "séquence:",
                      "dirègles:",
                      "désambiguïsation:",
                      "extraction:",
                      "déduction:",
                      "sémantique:",
                      "marqueur:",
                      "termes:",
                      "reconstructeur:",
                      "reconstructeur:",
                      "découpeséquence:",
                      "doubleréduction:",
                      "",
                      "fragmenter:",
                      "vocabulaires:",
                      "plrègles:",
                      "fonctions:",
					  "multimots:",
					  "multilemmes:",
					  "tokenselection:",
					  "lexicales:",
					  "génération:",
					  "ordre:",
					  "séquencesurcontexte:"};


char contiguous(Noeud* n1, Noeud* n2) {
    Noeud* m=n1->pere->fils;
    if (m==NULL)
        return 0;

	if (n1->frere==n2)
		return 1;
	return 0;
}

char firstnode(Noeud* n) {
	if (n->pere!=NULL && n->pere->fils==n)
		return 1;
	return 0;
}

char KifSelectedNode::addFeature(string& rule,char typerule) {
    //we add the features    
    char co='[';
    char cf=']';
    string text;
    
    char add=1;

    for (int f=0;f<features.size();f++) {
        if (add==1)
            text+=co;
        else
            text+=",";
        add=0;
        text+=features[f].c_str();
    }
    
    if (features.size()) {
        text+=cf;
        int i=rule.size()-1;        
        if (rule[i]==']') {
            while (rule[i]!='[') i--;            
            rule.insert(i,text);
        }
        else
            rule+=text;
    }

    return 1;
}

void KifSelectedDependency::addFeature(string& text) {
    //we add the features            
    for (int f=0;f<features.size();f++) {
        if (!f)
            text+="[";
        else
            text+=",";
        text+=features[f].c_str();
    }
    
    if (features.size())
        text+="]";
    
}


char KifRuleGeneration::buildrule(VECTA<KifSelectedNode*>& choices, 
                             int& i, 
                             string& rule_text,
                             int nbelements,
                             Noeud* firstnd,
                             char typerule) {
    char ch[10];
    //if we have enough element for a rule then, we create it
    string val;
    Noeud* lastnd=firstnd;    
    Noeud* nd;
	Noeud* cr;
    
    while (i<=nbelements) {
        //the index is the position of the element hn in the daughters list of the mother node
        //we explore each element after this first one on the mother node
        KifSelectedNode* sn=choices[i];
        nd=sn->node;
        if (sn->any==0)
            val=nd->X->nom;
        else
            val="?";
        
        //int the case of a XIPLEAF, the category should not appear
        //if (nd->Type()==XIPLEAF && typerule!=TAGGINGRULE)
        //    val="";

        //Two cases:
        //a) The element is at the same level as the first node
        //b) The element is a node under the previous the node
        if (nd->pere==firstnd->pere) {
            //We check if it is contiguous to the previous node
            if (nd!=lastnd) {
                if (typerule!=TAGGINGRULE && !contiguous(lastnd,nd)) {
                    //then we add ? in the formula
					if (addquestionmark)
						rule_text+=",?+";
					else {
						cr=lastnd->frere;
						while (cr!=NULL && cr!=nd) {
							rule_text+=",";
							rule_text+=cr->X->nom;
							cr=cr->frere;
						}
					}
				}
                if (i)
                    rule_text+=",";
            }
            rule_text+=val;  
            

            if (sn->genere!=0) {
                sprintf(ch,"#%d",numvar[nd->num_noeud]);
                rule_text+=ch;
            }

            if (sn->addFeature(rule_text,typerule)==0)
                return 0;
            lastnd=nd;
        }
        else {//if it is a sub-node, we add a sub-level            
            Noeud* loopnd=nd->pere;
            while (loopnd!=NULL) {
                if (loopnd==lastnd) 
                    break;
                loopnd=loopnd->pere;
            }
            
            if (loopnd==NULL) {
                //this is not a sub-node;
                //we return
                return 1;
            }
            
            //else we add our sub-level   
            loopnd=choices[i]->node;
            if (loopnd->fils!=NULL && typerule!=TAGGINGRULE) {
                rule_text+="{";
                if (!firstnode(nd)) {
					if (addquestionmark)
						rule_text+="?+,";
					else {
						cr=nd->pere->fils;
						while (cr!=NULL && cr!=nd) {								
							rule_text+=cr->X->nom;
							rule_text+=",";
							cr=cr->frere;
						}
					}
				}
            }
            if (buildrule(choices,i,rule_text,nbelements,nd,typerule)==0)
                return 0;
            if (loopnd->fils!=NULL && typerule!=TAGGINGRULE) {
                rule_text+="}";
            }
            continue;
        }
        i++;
    }
    return 1;
}

void KifRuleGeneration::builddependency(VECTA<KifSelectedNode*>& choices, 
                                    int& i, 
                                    string& rule_text,                                    
                                    Noeud* mothernd,
                                    char& addif) {
    char ch[100];

    //if we have enough element for a rule then, we create it
    string val;        
    char feature_added=0;
    Noeud* lastnd=NULL;
    while (i<choices.dernier) {
        //the index is the position of the element hn in the daughters list of the mother node
        //we explore each element after this first one on the mother node
        KifSelectedNode* sn=choices[i];
        Noeud* nd=sn->node;
        string addcat;
        if (sn->any==0)
            val=nd->X->nom;
        else
            val="ANY";

        addcat=val;
        //Two cases:
        //a) The element is at the same level as the first node
        //b) The element is a node under the previous the node
        
        char create_dependency=1;
        if (numvar[nd->num_noeud]==-1)
            numvar.affecte(nd->num_noeud,num_var++);
        
        if (sn->genere==2)
            create_dependency=0;
        
        sprintf(ch,"(#%d)",numvar[nd->num_noeud]);
        val+=ch;
        ch[0]=0;
        if (numvar[choices[0]->index]==-1)
            numvar.affecte(choices[0]->index,num_var++);

        if (mothernd!=NULL)
            //we are in sub-node, we add a mother clause                
            sprintf(ch," & mother(#%d,#%d)",numvar[mothernd->num_noeud],numvar[nd->num_noeud]);
        else
            sprintf(ch," & #%d<#%d",numvar[choices[0]->index],numvar[nd->num_noeud]);
        
        if (lastnd!=NULL && nd->pere!=lastnd->pere) {
            
            if (nd->pere==lastnd) {
                //this is a sub-node;
                //else we add our sub-level                    
                builddependency(choices,i,rule_text,lastnd,addif);
                continue;
            }
            else {
                if (mothernd!=NULL)
                    return;
            }            
        }  
        
        if (create_dependency==1) {
            if (addif==2) {            
                rule_text+=tab;
                rule_text+=" & ";
            }
            addif=2;        
            rule_text+="D_";
            rule_text+=val;  
            if (ch[0]!=0)
                rule_text+=ch;
            lastnd=nd;
            int j;
            for (j=0;j<categorydependency.size();j++) {
                if (categorydependency[j]==addcat)
                    break;
            }
            if (j==categorydependency.size())
                categorydependency.push_back(addcat);            
        }
        
        //we add the features  
        if (sn->features.size()) {
            if (create_dependency==0)
                rule_text+=tab;
            sprintf(ch," & #%d",numvar[sn->index]);
            rule_text+=ch;
            sn->addFeature(rule_text,DEPENDENCYRULE);
            if (create_dependency==0)
                rule_text+="\r\n";
        }
        
        if (create_dependency==1)
            rule_text+="\r\n";        
        i++;
    }
}


char KifRuleGeneration::kifrulegenRule(VECTA<int>& focus,
                                string& rule_text,
                                VECTA<KifSelectedNode*>& nodeselection,
                                VECTA<KifSelectedDependency*>& dependencyindexes,
                                Noeud* boundary,
                                string category,
                                int dependencysize,
                                char typerule) {
        //We check whether all the nodes share the same mother node
    int rule=0;
    int nbelements=0;
    char ch[100];
    Noeud* nd;
    Noeud* firstnode;
    Noeud* loopnd;

    //If no focus is provided, we extract variables from the dependencies
    //and the tree regular expression.
    int argcounter=dependencysize;

    
    int i,j;
    ResultatFonction* dep;
    
    if (!nodeselection.dernier)
        return 3;

    //We have a focus, we don't need to extract variables
    if (typerule==DEPENDENCYRULE) {
        if (focus.dernier!=0) {
            argcounter=0;
            for (i=0;i<nodeselection.dernier;i++) {
                for (j=0;j<focus.dernier;j++) {
                    if (nodeselection[i]->index==focus[j]) {
                        if (numvar[focus[j]]==-1)
                            numvar.affecte(focus[j],num_var++);
                        nodeselection[i]->genere=1;
                        break;
                    }
                }
            }
        }
        
        for (i=0;i<dependencyindexes.dernier;i++) {
            dep=dependencyindexes[i]->dependency;
            for (j=0;j<dep->arg.size();j++) {
                
                //we use this parameter as a focus
                if (argcounter) {
                    focus.ajoute(dep->arg[j]->num_noeud);
                    argcounter--;
                }
                if (numvar[dep->arg[j]->num_noeud]==-1)
                    numvar.affecte(dep->arg[j]->num_noeud,num_var++);
                for (int k=0;k<nodeselection.dernier;k++) {
                    if (nodeselection[k]->index==dep->arg[j]->num_noeud) {
                        nodeselection[k]->genere=2;
                        break;
                    }
                }
            }
        }
    }
    else
        argcounter=0;

    if (typerule==IDRULE) {
        nodeselection[0]->features.push_back("first:+");
        nodeselection.fin()->features.push_back("last:+");
    }

	static VECTA<int> added;
	added.raz();
	for (i=0;i<nodeselection.dernier;i++)
		added.affecte(nodeselection[i]->node->num_noeud,1);

    if (nodeselection[0]!=NULL) {
        firstnode=nodeselection[0]->node;        
        rule=1;
        //If we need some parameter, we use this one
        if (argcounter) {
            focus.ajoute(firstnode->num_noeud);
            if (numvar[firstnode->num_noeud]==-1)
                numvar.affecte(firstnode->num_noeud,num_var++);
            if (nodeselection[0]->genere==0)
                nodeselection[0]->genere=1;
            argcounter--;
        }

        for (i=1;i<nodeselection.dernier;i++) {
            nd=nodeselection[i]->node;
           
            if (argcounter) {
                focus.ajoute(nd->num_noeud);
                if (numvar[nd->num_noeud]==-1)
                    numvar.affecte(nd->num_noeud,num_var++);
                if (nodeselection[i]->genere==0)
                    nodeselection[i]->genere=1;
                argcounter--;
            }

            if (typerule==IDRULE) {
                //In that case the node should be contiguous
                if (!contiguous(nodeselection[i-1]->node,nd)) {
                    //Special case: nd is XipLeaf whose mother is nodeselection[i-1]
                    //We have two cases:
                    //a) The node is under the previous node and it is a XipLeaf
                    //b) The node is after a XipLeaf and it should be contiguous to the mother of that node
                    char error=1;
                    if (nd->fils==NULL) {
                        //Is the current node under the previous node
                        if (nd->pere==nodeselection[i-1]->node)
                            error=0;
                        else //is the mother of the previous node contiguous to the mother of the current node
                            if (nodeselection[i-1]->node->fils==NULL &&
                                contiguous(nodeselection[i-1]->node->pere,nd->pere))
                                error=0;
                    }
                    else {//is the mother of the previous node contiguous to the current node
                        if (nodeselection[i-1]->node->fils==NULL &&
                            contiguous(nodeselection[i-1]->node->pere,nd))
                            error=0;
                    }
                    if (error==1) {
                        rule_text="Error: the categories should be contiguous in an ID rule";
                        return 1;
                    }
                }
            }
            //we check if the node is compatible with our structure:
            //a) if it is a sister of the firstnode
            //b) if it is a daughter of the previous node
            //c) if there is no un-selected node between it and the first node

            //If the current node is not a sister of the first node
            //it should necessary be a sub-node of the last node that has been scanned.
            if (nd->pere!=firstnode->pere) {
				if (typerule==IDRULE || typerule==SEQUENCERULE) {
					rule_text="Error: no sub-node selection for a chunk rule";
					return 1;
				}
                //Is a daughter of the previous node?
                if (nd->pere!=nodeselection[i-1]->node) {
                    //Is it a brother of a previous node?
                    char found=0; 
                    int nm;
                    for (nm=0;nm<i;nm++) {
                        if (nd->pere==nodeselection[nm]->node) {
                            found=1;
                            break;
                        }
                    }
                    if (found==0) {
                        //we check to see whether there is not a missing node
                        //between this node and the already selected nodes...
                        //loopnd=nd->pere->pere;
                        //while (loopnd!=NULL) {
                        //    for (nm=0;nm<i;nm++) {
                        //        if (loopnd==nodeselection[nm]->node) {
                        //            rule_text="Error: Missing node in the path...";
                        //            return 1;
                        //        }
                        //    }
                        //    loopnd=loopnd->pere;
                        //}
						//In this case, we try to found out a common ancestor
						//with previous nodes
						if (typerule==DEPENDENCYRULE || typerule==LEFTCONTEXT || typerule==RIGHTCONTEXT) {
							//we know that this common anscestor is not there, at the first level
							//we analyze the first node, and we try to find a common ancestor with it
							loopnd=firstnode;
							found=0;
							Noeud* loopcurrent=nd->pere;
							while (loopnd!=NULL) {
								loopcurrent=nd->pere;
								while (loopcurrent!=NULL) {
									if (loopcurrent==loopnd) {
										found=1;
										break;
									}
									loopcurrent=loopcurrent->pere;
								}
								if (found==1)
									break;
								loopnd=loopnd->pere;
							}
							if (found==0)
								break;
							//We must insert those intermediate nodes in our structure
							//the top node is loopnd
							//first we add these nodes before i
							int addi=0;
							loopcurrent=nd->pere;
							KifSelectedNode* sn;
							while (loopcurrent!=loopnd) {
								if (added[loopcurrent->num_noeud]==1)
									break;
								added.affecte(loopcurrent->num_noeud,1);
								if (loopcurrent->pere==NULL)
									break;
								sn=new KifSelectedNode(loopcurrent,loopcurrent->num_noeud);
								nodeselection.insertion(i,sn);								
								loopcurrent=loopcurrent->pere;
								addi++;
							}
							//Then we add the other nodes at the beginning
							loopcurrent=firstnode;
							while (loopcurrent!=loopnd) {								
								loopcurrent=loopcurrent->pere;
								if (added[loopcurrent->num_noeud]==1)
									break;
								added.affecte(loopcurrent->num_noeud,1);
								if (loopcurrent->pere==NULL)
									break;
								sn=new KifSelectedNode(loopcurrent,loopcurrent->num_noeud);
								firstnode=loopcurrent;
								nodeselection.insertion(0,sn);
								addi++;								
							}							
							i+=addi;
							rule+=addi;
						}
						else
							break;
                    }
                }
            }

            nbelements=i;
            rule++;            
            //we keep it            
        }
    }

    //If there is still some arguments to fill in, we use the rest of the rule
    if (argcounter) {
        for (int p=i;p<nodeselection.dernier;p++) {
            nd=nodeselection[p]->node;           
            if (argcounter) {
                focus.ajoute(nd->num_noeud);
                if (numvar[nd->num_noeud]==-1)
                    numvar.affecte(nd->num_noeud,num_var++);
                if (nodeselection[p]->genere==0)
                    nodeselection[p]->genere=1;
                argcounter--;
            }
            else
                break;
        }
    }

    //if we have enough element for a rule then, we create it
    string val;
    tab="";
    if (typerule!=DEPENDENCYRULE) {
        if (rule!=nodeselection.dernier) {
            rule_text="Error: Wrong selection of nodes";
            return 1;
        }
        
        if (typerule==LEFTCONTEXT)
            rule_text="|";

        i=0;
        if (buildrule(nodeselection,i,rule_text,nbelements,firstnode,typerule)==0)
            return 4;

        //In the case of a left context we may need to add a ?+, if the last element
        //of the kifrulegend rule is not contiguous to the first element
        if (typerule==LEFTCONTEXT) {
            if (!contiguous(nodeselection.fin()->node,boundary))
                rule_text+=",?+";
            rule_text+="|";
        }
        else {
            if (typerule==RIGHTCONTEXT) {
                val="|";
                if (!contiguous(boundary,nodeselection[0]->node))
                    val+="?+,";    
                val+=rule_text;
                rule_text=val;                
                rule_text+="|";
            }
        }
   
        return 0;
    }

    if (rule>=1) {
        rule_text="|";
        val=firstnode->X->nom;
        rule_text+=val;
        if (numvar[firstnode->num_noeud]==-1)
            numvar.affecte(firstnode->num_noeud,num_var++);
        sprintf(ch,"#%d",numvar[firstnode->num_noeud]);
        rule_text+=ch;
        nodeselection[0]->addFeature(rule_text,typerule);        
        i=1;
        if (buildrule(nodeselection,i,rule_text,nbelements,firstnode,typerule)==0)
            return 4;
        rule_text+="|\r\n";
        tab+="\t";
    }
    
    char ifadded=0;
    
    for (i=0;i<dependencyindexes.dernier;i++) {
        dep=dependencyindexes[i]->dependency;
        if (ifadded==0) {
            rule_text+=tab;
            rule_text+= "if (";
            ifadded=1;            
        }
        else {
            rule_text+=tab;
            rule_text+=" & ";
        }
        
        rule_text+=dep->Fx->nom;
        dependencyindexes[i]->addFeature(rule_text);

        rule_text+="(";
        for (j=0;j<dep->arg.size();j++) {
            if (!j)
                sprintf(ch,"#%d",numvar[dep->arg[j]->num_noeud]);
            else
                sprintf(ch,",#%d",numvar[dep->arg[j]->num_noeud]);
            rule_text+=ch;                
        }
        rule_text+=")\r\n";
    }

    //Pour les categories suivantes, on passe par des dependances
    if (nbelements+1<nodeselection.dernier) {
        if (ifadded==0) {
            rule_text+=tab;
            rule_text+= "if (";
            ifadded=1;        
        }
        else 
            ifadded=2;

        
        for (i=nbelements+1;i<nodeselection.dernier;i++) 
            builddependency(nodeselection,i,rule_text,NULL,ifadded);
    }

    if (ifadded) {
        rule_text+=tab;
        rule_text+=")\r\n";        
    }

    if (dependencysize==0) {
        rule_text+=tab;
        rule_text+=category;        
    }
    else {
        //we use as many parameter as needed for our task
        for (i=0;i<focus.size();i++) {
            if (i)
                rule_text+=",";
            else
                rule_text+=tab;   
            rule_text+=category;        
            int split=dependencysize;
            if (split) {
                rule_text+="(";
                while (split && i<focus.size()) {
                    if (split<dependencysize)
                        rule_text+=",";
                    sprintf(ch,"#%d",numvar[focus[i++]]);
                    rule_text+=ch;        
                    split--;
                }
                i--;
                rule_text+=")";
            }
            if (split!=0) {
                rule_text="Error: Missing argument...";
                return 2;
            }
        }
    }
    rule_text+=".\r\n";
    return 0;
}



//Generation of the rules corresponding to the categorydependency vector
void KifRuleGeneration::kifrulegenRuleCategory(int ipar,string filename,vector<string>& categories) {
    //if there is no dependency of that sort we return
    if (categories.size()==0)
        return;

    char** fieldkeywords;
    if (XipMessageLanguage(ipar)==1)
        fieldkeywords=kif_field_name;
    else
        fieldkeywords=kif_noms_champ;

    //first we create the file
    ofstream file(STR(filename));
    //first we create the function section
    file<<fieldkeywords[17]<<endl<<endl;
    int i;
    for (i=0;i<categories.size();i++) {
        if (i)
            file<<",";
        file<<"D_"<<categories[i].c_str();
    }
    file<<"."<<endl<<endl<<endl;
    //we then create the dependency section
    file<<fieldkeywords[5]<<endl<<endl;
    for (i=0;i<categories.size();i++) {
        //the category should not be preceded by D_
        if (categories[i]!="ANY")
            file<<"|"<<categories[i]<<"#1| "<<"D_"<<categories[i]<<"(#1)."<<endl;
        else
            file<<"|?#1| "<<"D_ANY(#1)."<<endl;
    }    
}
