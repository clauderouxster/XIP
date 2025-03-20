#ifdef XIPYTHON
#ifndef WIN32
#define Py_BUILD_CORE
#endif
#ifdef APPLE
#ifdef DYNAMICLIB
#include <crt_externs.h>
char **environ = *_NSGetEnviron();
#endif
#endif
#endif


#ifdef XIPPYTHON

//On se definit un bout de code python pour la bonne bouche
string insere_classes_xip(char glob) {
    string func="";
    func+="class XipNode:\n";
    func+=" def getnode(self,i):\n";
    func+="  if i==-1:\n";
    func+="   return 0\n";
	func+="  liste=node(self.grm,i)\n";
    func+="  self.index=i\n";
    return func;    
}


//Une fonction de base de python pour pouvoir jouer avec les noeuds
extern "C" {
	//Pour recuperer par exemple la version du moteur
    static PyObject* xips_version(PyObject* self, PyObject* args) {
        char version[1000];      
        
         sprintf(version,"XIP: Xerox Incremental Parser %s (2000-2007) ",VERSION_XIP);
        return PyString_FromString(version);
    }

	//La je recois un index de noeud XIP, un simple entier et je
	//retourne une liste qui comprend toutes les infos...
    static PyObject* xips_xipnode(PyObject* self, PyObject* args) {
        int node;          
		int parsnum;
        
        if (!PyArg_ParseTuple(args,"ii",&parsnum,&node))  {
				return Py_BuildValue("[issssiiiiiiillll]",0,
					"NONE","NONE","NONE","[NONE:NONE]",-1,-1,-1,-1,-1,0,0,-1,-1,-1,-1);
        }
        
        GlobalParseur* parseur=gVecteurParseurs[parsnum];        
        if (parseur==NULL) {
            return Py_BuildValue("[issssiiiiiiillll]",0,
                "NONE","NONE","NONE","[NONE:NONE]",-1,-1,-1,-1,-1,0,0,-1,-1,-1,-1);
        }
		parseur_courant=gVecteurParseurs[parsnum];
		parseur->actif=OUI;
        
        Noeud* n=NULL;
        
        
        if (n==NULL) {
        }        

        string rpos;
        string rsurface;
        string rlemme;
        string rtraits;
        int rpere=-1;
        int rfils=-1;
        int rdernier=-1;
        int rsuivant=-1;
        int rprecedent=-1;
		PyObject* listeinter;
		PyObject* laliste=NULL;
		int choix=n->source->choisie;

		n->source->choisie=i;
		n->ConstruitUnNoeud(parseur, 
			rpos,
			rsurface,
			rlemme,
			rtraits,
			rpere,
			rfils,
			rdernier,
			rsuivant,
			rprecedent);

		listeinter=Py_BuildValue("[issssiiiiiiillll]",1,
			(char*)rpos.c_str(),(char*)rsurface.c_str(),
			(char*)rlemme.c_str(),(char*)rtraits.c_str(),
			rpere,rfils,rdernier,rsuivant,rprecedent,n->num_gauche,n->num_droite,
			n->gauche,n->droite,n->gauchecar,n->droitecar);
		PyList_Insert(laliste,i,listeinter);
		Py_DECREF(listeinter);
		return laliste;
	}


    static PyObject* xips_xipnodes(PyObject* self, PyObject* args) {
        char* node;      
        int i,parsnum;
        
        PyObject* liste=PyList_New(0);
		if (!PyArg_ParseTuple(args,"is",&parsnum,&node))
				return liste;

		GlobalParseur* parseur=gVecteurParseurs[parsnum];
        if (parseur==NULL)
            return liste;
		parseur_courant=parseur;
		parseur->actif=OUI;
        
        Categorie* c=parseur->RetrouveCat(node);
        if (c==NULL)
            return liste;       

        for (i=0; i< parseur->IndexParCat[c->X.val].dernier;i++) {
            PyObject* pcourant=PyInt_FromLong(parseur->IndexParCat[c->X.val][i]->N.num_noeud);
            PyList_Insert(liste,i,pcourant);
			Py_DECREF(pcourant);
        }
        
        return liste;
    }
    
    static PyObject* xips_xiplexicalnodes(PyObject* self, PyObject* args) {
        int i,parsnum=index_parseur_courant;
        
        PyObject* liste=PyList_New(0);        
		if (!PyArg_ParseTuple(args,"i",&parsnum)) 
            return liste;


		GlobalParseur* parseur=gVecteurParseurs[parsnum];
        if (parseur==NULL)
            return liste;

		parseur_courant=parseur;
		parseur->actif=OUI;
        
        for (i=0; i< parseur->NoeudsLexicaux.dernier;i++) {
            PyObject* pcourant=PyInt_FromLong(parseur->NoeudsLexicaux[i]->num_noeud);
            PyList_Insert(liste,i,pcourant);
			Py_DECREF(pcourant);
        }
        
        return liste;
    }

    static PyObject* xips_xipfictivenodes(PyObject* self, PyObject* args) {
        int i,parsnum;
        
        PyObject* liste=PyList_New(0);        
		if (!PyArg_ParseTuple(args,"i",&parsnum)) 
            return liste;

		GlobalParseur* parseur=gVecteurParseurs[parsnum];
        if (parseur==NULL)
            return liste;

		parseur_courant=parseur;
		parseur->actif=OUI;
        
        Noeud* n=NULL;
        
        for (i=0; i< parseur->NoeudsDependance.dernier;i++) {
            PyObject* pcourant=PyInt_FromLong(parseur->NoeudsDependance[i]->N.num_noeud);
            PyList_Insert(liste,i,pcourant);
			Py_DECREF(pcourant);
        }
        
        return liste;
    }

    static PyObject* xips_xipinternalstring(PyObject* self, PyObject* args) {
                
        int parsnum;
        
		if (!PyArg_ParseTuple(args,"i",&parsnum)) 
            return PyString_FromString("");

		GlobalParseur* parseur=gVecteurParseurs[parsnum];
        if (parseur==NULL || parseur->donneesATraiter==NULL)
            return PyString_FromString("");

		parseur_courant=parseur;
		parseur->actif=OUI;
        
        
        return PyString_FromString((char*)parseur->donneesATraiter);
    }
    
    static PyObject* xips_dependency(PyObject* self, PyObject* args) {
        int dep;      
        int i,parsnum;
        
		if (!PyArg_ParseTuple(args,"ii",&parsnum,&dep))
			return Py_BuildValue("[ssi]","NONE","[NONE:NONE]",-1);		

        GlobalParseur* parseur=gVecteurParseurs[parsnum];
        if (parseur==NULL) {
            return Py_BuildValue("[ssi]",
                "NONE","[NONE:NONE]",-1);
        }
		parseur_courant=parseur;
		parseur->actif=OUI;

        if (parseur->INTERFONCTIONS[0]==NULL) {
            return Py_BuildValue("[ssi]",
                "NONE","[NONE:NONE]",-1);
        }

        ResultatFonction* rf=(*parseur->INTERFONCTIONS[0])[dep];
        if (rf==NULL) {
            return Py_BuildValue("[ssi]",
                "NONE","[NONE:NONE]",-1);
        }

        
		string traits;
		if (rf->traits!=NULL)
			rf->traits->TraitsEnChaineString(parseur,traits,parseur->FILTREFONCTION);
		for (i=0;i<rf->stack.dernier;i++) {
			if (i)
				traits+=",";
			else {
				traits=traits.substr(0,traits.size()-1);
				if (traits!="[")
					traits+=",";
			}
			traits+="stack:'";
			traits+=rf->stack[i];
			traits+="'";
		}
		if (rf->stack.dernier)
			traits+="]";


        PyObject* pcourant;
        PyObject* liste=PyList_New(0);
        int params=0;

        pcourant=PyString_FromString(rf->Fx->nom);
        PyList_Insert(liste,params++,pcourant);
		Py_DECREF(pcourant);

        pcourant=PyString_FromString(traits.c_str());
        PyList_Insert(liste,params++,pcourant);
		Py_DECREF(pcourant);

        for (i=0; i< rf->arg.dernier;i++) {
            pcourant=PyInt_FromLong(rf->arg[i]->num_noeud);
            PyList_Insert(liste,params++,pcourant);
			Py_DECREF(pcourant);
        }
        
        return liste;
    }

	static PyObject* xips_setfloatvariable(PyObject* self, PyObject* args) {
        float valeur;
		char* variable;
        int grm;

		
        if (!PyArg_ParseTuple(args,"isf",&grm,&variable,&valeur)) 
            return PyInt_FromLong(-1);
        
		string var=variable;
		int r=XipSetVariable(grm,var,valeur);
        return PyInt_FromLong(r);
    }

	static PyObject* xips_setintvariable(PyObject* self, PyObject* args) {
        int valeur;
		char* variable;
        int grm;

        if (!PyArg_ParseTuple(args,"isi",&grm,&variable,&valeur)) 
            return PyInt_FromLong(-1);
        
		string var=variable;
		int r=XipSetVariable(grm,var,(float)valeur);
        return PyInt_FromLong(r);
    }

	static PyObject* xips_setstringvariable(PyObject* self, PyObject* args) {
        char* valeur;
		char* variable;
        int grm;

        if (!PyArg_ParseTuple(args,"iss",&grm,&variable,&valeur)) 
            return PyInt_FromLong(-1);
        
		string var=variable;
		string value=valeur;
		int r=XipSetStringVariable(grm,var,value);
        return PyInt_FromLong(r);
    }


    static PyObject* xips_setdisplay(PyObject* self, PyObject* args) {
        TVecteur mode;
        int grm;

        if (!PyArg_ParseTuple(args,"iL",&grm,&mode)) 
            return PyInt_FromLong(-1);
        
        XipSetDisplayMode(grm,mode,80);

        return PyInt_FromLong(0);
    }

	static PyObject* xips_adddisplay(PyObject* self, PyObject* args) {
        TVecteur mode;
        int grm;

        if (!PyArg_ParseTuple(args,"iL",&grm,&mode)) 
            return PyInt_FromLong(-1);
        
		XipAddFlagDisplayMode(grm,mode,80);

        return PyInt_FromLong(0);
    }

	static PyObject* xips_testdisplay(PyObject* self, PyObject* args) {
        TVecteur mode;
        int grm;

        if (!PyArg_ParseTuple(args,"iL",&grm,&mode)) 
            return PyInt_FromLong(-1);
        
		if (XipTestFlagDisplayMode(grm,mode)==1)
			return PyInt_FromLong(1);

        return PyInt_FromLong(0);
    }
	
	static PyObject* xips_currentgrammarindex(PyObject* self, PyObject* args) {
        if (parseur_courant==NULL)
            return PyInt_FromLong(-1);

        return PyInt_FromLong(parseur_courant->iParseurCourant);
    }


	static PyObject* xips_removedisplay(PyObject* self, PyObject* args) {
        TVecteur mode;
        int grm;

        if (!PyArg_ParseTuple(args,"iL",&grm,&mode)) 
            return PyInt_FromLong(-1);
        
		XipRemoveFlagDisplayMode(grm,mode);

        return PyInt_FromLong(0);
    }


    static PyObject* xips_dependencies(PyObject* self, PyObject* args) {
        char* dep;      
        int i,parsnum;
        PyObject* liste=PyList_New(0);

		if (!PyArg_ParseTuple(args,"is",&parsnum,&dep))
				return liste;
        
		GlobalParseur* parseur=gVecteurParseurs[parsnum];
        if (parseur==NULL)
            return liste;

		parseur_courant=parseur;
		parseur->actif=OUI;

        //On recupere donc nos dependances;
        Categorie* cdep=parseur->RetrouveFonc(dep);
        if (cdep==NULL)
            return liste;

        PyObject* pcourant;
        

        int idep=cdep->X.val;
		ResultatFonction* rf;
        if (parseur->INTERFONCTIONS[idep]!=NULL) {
            for (i=0;i<parseur->INTERFONCTIONS[idep]->dernier;i++) {
				rf=(*parseur->INTERFONCTIONS[idep])[i];
                if (rf!=NULL && rf->Fx!=NULL) {
                    pcourant=PyInt_FromLong(rf->index);
                    PyList_Insert(liste,i,pcourant);
					Py_DECREF(pcourant);
                }
            }
        }

        return liste;
    }

    static PyObject* xips_dependencyonfirstnode(PyObject* self, PyObject* args) {
        int node,parsnum;
        PyObject* liste=PyList_New(0);

		if (!PyArg_ParseTuple(args,"ii",&parsnum,&node)) 
			return liste;
        
		GlobalParseur* parseur=gVecteurParseurs[parsnum];
        if (parseur==NULL)
            return liste;

		parseur_courant=parseur;
		parseur->actif=OUI;

        if (parseur->NoeudsTemporaires[node]==NULL)
            return liste;

        PyObject* pcourant;
        
        if (parseur->PREMIERFONCTIONS[node]!=NULL) {
            for (int i=0;i<parseur->PREMIERFONCTIONS[node]->dernier;i++) {
                if ((*parseur->PREMIERFONCTIONS[node])[i]!=NULL) {
                    pcourant=PyInt_FromLong((*parseur->PREMIERFONCTIONS[node])[i]->index);
                    PyList_Insert(liste,i,pcourant);
					Py_DECREF(pcourant);
                }
            }
        }

        return liste;
    }

    static PyObject* xips_dependencyonsecondnode(PyObject* self, PyObject* args) {
        int node,parsnum;
        PyObject* liste=PyList_New(0);

		if (!PyArg_ParseTuple(args,"ii",&parsnum,&node)) 
			return liste;
        
		GlobalParseur* parseur=gVecteurParseurs[parsnum];
        if (parseur==NULL)
            return liste;

		parseur_courant=parseur;
		parseur->actif=OUI;

        if (parseur->NoeudsTemporaires[node]==NULL)
            return liste;

        PyObject* pcourant;
        
		if (parseur->SECONDFONCTIONS[node]!=NULL) {
			for (int i=0;i<parseur->SECONDFONCTIONS[node]->dernier;i++) {
				if ((*parseur->SECONDFONCTIONS[node])[i]!=NULL) {
					pcourant=PyInt_FromLong((*parseur->SECONDFONCTIONS[node])[i]->index);
					PyList_Insert(liste,i,pcourant);
					Py_DECREF(pcourant);
				}
			}
		}
        return liste;
    }

    
    static PyMethodDef NoeudMethode[] = {
        
        {"node",xips_xipnode,METH_VARARGS,"Return a xip node."},
        {"nodeset",xips_xipnodes,METH_VARARGS,"Return a set of xip nodes."},
        {"dependency",xips_dependency,METH_VARARGS,"Return a dependency."},
        {"dependencyset",xips_dependencies,METH_VARARGS,"Return a set of dependency."},
        {"parsestring",xips_parsestring,METH_VARARGS,"parse a string with grammar g: xips.parsestring(g,str,data). (data is any python object)"},
        {"parsefile",xips_parsefile,METH_VARARGS,"parse a file with grammar g: xips.parsefile(g,filename,data). (data is any python object)"},
        {"parsexmlstring",xips_parsexmlstring,METH_VARARGS,"parse an XML string with grammar g at depth: xips.parsexmlstring(g,str,depth,data). (data is any python object)"},
        {"parsexmlfile",xips_parsexmlfile,METH_VARARGS,"parse an XML file with grammar g at depth: xips.parsexmlfile(g,filename,depth,data). (data is any python object)"},
        {"loadgrm",xips_loadgrm,METH_VARARGS,"Load a grammar file: xips.loadgrm(filename)."},
        {"reloadgrm",xips_reloadgrm,METH_VARARGS,"Reload the grammar file with the index grm: xips.reloadgrm(grm)."},
        {"setdisplay",xips_setdisplay,METH_VARARGS,"Set the mode display xips.setdisplay(grm,mode)."},
        {"adddisplay",xips_adddisplay,METH_VARARGS,"Add a mode display display xips.adddisplay(grm,mode)."},
        {"removedisplay",xips_removedisplay,METH_VARARGS,"Remove a mode display xips.removedisplay(grm,mode)."},
        {"testdisplay",xips_testdisplay,METH_VARARGS,"Test a mode display xips.testdisplay(grm,mode), return 1 or 0"},
        {"whoami",xips_version,METH_VARARGS,"return the XIP engine version."},
        {"dependencyonfirstnode",xips_dependencyonfirstnode,METH_VARARGS,"return the list of dependencies whose first parameter is the node n: xips.dependencyonfirstnode(n)."},
        {"dependencyonsecondnode",xips_dependencyonsecondnode,METH_VARARGS,"return the list of dependencies whose second parameter is the node n: xips.dependencyonsecondnode(n)."},
        {"fictivenodes",xips_xipfictivenodes,METH_VARARGS,"return the list of fictive nodes that have been created so far."},
        {"setexchange",xips_setexechange,METH_VARARGS,"Set the exchange variable: xips.setexchange(v,o). v is a XIP python variable: return -1 if wrong parameters. -2 if unknown XIP variable. -3 if wrong XIP variable type. 0 if OK."},
        {"internalstring",xips_xipinternalstring,METH_VARARGS,"return the internal string buffer. Applies only when parsing with a string or from an XML file."},
        {"lexicals",xips_xiplexicalnodes,METH_VARARGS,"return the list of the lexical node indexes (no parameter)."},        
        {"setfloat",xips_setfloatvariable,METH_VARARGS,"setfloat(par,v,f) Sets the variable v (a string) with the value f (a float) for the grammar grm."},        
        {"setint",xips_setintvariable,METH_VARARGS,"setint(par,v,i) Sets the variable v (a string) with the value i (an int) for the grammar grm."},        
        {"setstring",xips_setstringvariable,METH_VARARGS,"setstring(par,v,s) Sets the variable v (a string) with the value i (a string) for the grammar grm."},        
		{"setdebug",xips_setdebug,METH_VARARGS,"setdebug(module_number,module_line) Sets a break point in the module module_num at module_line."},        
		{"currentgrammarindex",xips_currentgrammarindex,METH_VARARGS,"grammarindex() return the current grammar index."},        
		{"currentrule",xips_current_rule,METH_VARARGS,"currentrule() return the current rule index."},        
		{"currentruletext",xips_current_rule_text,METH_VARARGS,"currentruletext(displayheader) return the current rule text, displayheader is 0,1 or 2."},
        {NULL,NULL,0,NULL}
    };
    


    //Exportation void initxips() {
    //    PyObject* module=Py_InitModule("xips",NoeudMethode);        
    //    PyRun_SimpleString("import xips\n");
    //    PyRun_SimpleString(insere_classes_xip(OUI).c_str());
    //    PyObject* pDict = PyModule_GetDict(module);        
    //    PyRun_String(insere_classes_xip(SAUTE).c_str(),Py_file_input,pDict,pDict);                
    //}

    Exportation void initxips() {
        PyObject* module=Py_InitModule("xips",NoeudMethode);        
        PyObject* pDict = PyModule_GetDict(module);      
        PyRun_String(insere_classes_xip(OUI).c_str(),Py_file_input,pDict,pDict);                
    }
}

//Initialisation unique
void GlobalParseur::initialisation_python() {
    if (Py_IsInitialized())
		return;
    Py_Initialize();
    initxips();    
}
#else
void initxips() {}
#endif
