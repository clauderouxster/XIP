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
filename   : varlibxml.cxx
Date       : 13/02/2003
Purpose    : Manipulation of XML files as XIP objects
Programmer : Claude ROUX
Reviewer   : 
*/

/*
UTF-8
UTF-16
UTF-16
EBCDIC
ISO-10646-UCS-4
ISO-10646-UCS-4
ISO-10646-UCS-4
ISO-10646-UCS-4
ISO-10646-UCS-2
ISO-8859-1
ISO-8859-2
ISO-8859-3
ISO-8859-4
ISO-8859-5
ISO-8859-6
ISO-8859-7
ISO-8859-8
ISO-8859-9
ISO-2022-JP
Shift-JIS
EUC-JP
*/



#define AfficherTrace(c) if (parseur->TRACE) trace<<c<<endl;
char* operatorstrings[] = {" and ", " or ", " not "};

const char* aliasreserves[] = { "_xtd","_currentxmlnode","_current","@"};

#include "parsbin.h"
#include "varlibxml.h"
#include "conversion.h"

extern ostream* flot_erreur;

string& Trim(string& chaine);
//--------------------------------------------------------------------------------------
void strcatchr(char* tmp,char c) {
    int l=strlen(tmp);
    tmp[l]=c;
    tmp[l+1]=0;
}

void strcatchr(GlobalParseur* parseur,int max,char* tmp,char c) {
    int l=strlen(tmp);
    if (l>=max)
        parseur->erreur(parseur->CHAINES[336]);
    tmp[l]=c;
    tmp[l+1]=0;   
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
/**
 * asciiToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of ASCII chars
 * @inlen:  the length of @in
 *
 * Take a block of ASCII chars in and try to convert it to an UTF-8
 * block of chars out.
 * Returns 0 if success, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     as the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of ocetes consumed.
 */


unsigned char* GlobalParseur::conversionUTF8VersAscii(unsigned char* contenu) {
	chainetraduction=conversionUTF8VersLatin(contenu);
    return (unsigned char*)chainetraduction.c_str();        
}



uchar* GlobalParseur::conversionAsciiVersUTF8(uchar* contenu) {
	chainetraduction=conversionLatinVersUTF8(contenu);
    return (unsigned char*)chainetraduction.c_str();        
}


#ifdef XIPLIBXML
#include "varlibxml.h"
#include <wchar.h>
#include <libxml/HTMLparser.h>


extern "C" {
void xmlParserInputShrink(xmlParserInputPtr in);
xmlXPathObjectPtr xmlXPathNewNodeSet(xmlNodePtr val);
void xmlXPathNodeSetAddUnique(xmlNodeSetPtr cur, xmlNodePtr val);
void xmlNodeDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc, 
                       xmlNodePtr cur,int level, int format, const char *encoding);
int xmlNodeDump(xmlBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur, int level,int format);
int xmlXPathRegisterNs(xmlXPathContextPtr ctxt, const xmlChar *prefix,const xmlChar *ns_uri);
xmlNodePtr xmlNewNodeEatName(xmlNsPtr ns,xmlChar *name);
xmlNsPtr xmlNewNs(xmlNodePtr node, const xmlChar *href, const xmlChar *prefix);

}

string xmlEncoding(string& tag,string& txt) {
	string buffer="<";
	buffer+=tag;
	buffer+="/>";
	xmlDocPtr doc=xmlSAXParseMemory(NULL,STR(buffer),buffer.size(),0);
    doc->encoding=xmlCharStrdup("UTF-8");
    txt = conversionLatinVersUTF8(USTR(txt));    
	xmlChar* valeur=USTR(txt);
    //On stocke tout dans le champ contenu associe au noeud
    xmlAddChild(doc->children,xmlNewDocText(doc,valeur));
	xmlChar* buff=NULL;
	int longueur;
	xmlDocDumpMemoryEnc(doc, &buff,&longueur,"UTF-8");
	buffer=(char*)buff;
	xmlFree(buff);
	xmlFreeDoc(doc);
	int pos=buffer.find(tag);
	buffer=buffer.substr(pos-1,buffer.size()-pos);
	return buffer;
}

xmlChar* xmlCodage(GlobalParseur* parseur,xmlDocPtr doc,uchar* buff) {
    if (parseur->TestOptions(AFFICHAGE_UTF8))
        return xmlCharStrdup((const char*)parseur->conversionAsciiVersUTF8(buff));    
    return xmlCharStrdup((const char*)buff);
}



inline xmlNodePtr creationNouveauNoeudXML(xmlDocPtr doc,xmlNsPtr ns,xmlChar* name,xmlChar* contenu) {

    xmlNodePtr n=xmlNewNodeEatName(ns, name);
    if (contenu!=NULL) {
        xmlNodePtr nouveau=xmlNewDocText(doc, contenu);
        xmlAddChild(n, nouveau);
    }

    return n;
}


xmlNodePtr GlobalParseur::creeNouveauNoeudXML(xmlDocPtr doc,xmlNsPtr ns,xmlChar* name,xmlChar* contenu) {
	xmlNodePtr n=creationNouveauNoeudXML(doc,ns,name,contenu);
	NodeReference(n);
	return n;
}

//--------------------------------------------------------------------------------------
uchar* minuscule(unsigned char* c);
TVecteur TrouveValeur(Vecteur* v, Vecteur* vf,int& rang);
void Trim(char* chaine);
char TestChaineVide(unsigned char* chaine);
//--------------------------------------------------------------------------------------
const char* INSTRUCTIONCHAINE[] = {
    "none","assign","assignattribute","save",
    "compare","settext","attribute","test","create","substring","createattribute","rename",
    "feature","analyze","merge","skip","clean","markup",
	"setnamespace","tokenize","createadd","getdata","setparent","set","tokenizeattribute","initialindexhits","indexhits","%"};

const char* VARIABLESXML[] = {"none","current","next","previous","parent","daughter","%"};

//--------------------------------------------------------------------------------------
unsigned char* metXML::Chaine() {
    return (uchar*)INSTRUCTIONCHAINE[methode];
}
//--------------------------------------------------------------------------------------
arborescenceXML::~arborescenceXML() {
	for (map<string, xmlXPathObjectPtr>::iterator iter=lesindex.begin();iter!=lesindex.end();iter++) {
		xmlXPathObjectPtr liste= (xmlXPathObjectPtr)(*iter).second;
        if (liste!=NULL)
            xmlXPathFreeObject(liste);
	}

	if (pointentree!=NULL)
		libere(pointentree);
	if (xpath!=NULL)
		xmlXPathFreeContext(xpath);
    if (document!=NULL)
        xmlFreeDoc(document);        
    if (nomfichier!=NULL)
        libere(nomfichier);
	if (xpathcourant!=NULL)
		xmlXPathFreeContext(xpathcourant);

}

xmlXPathObjectPtr arborescenceXML::recherche(string clef) {
	return lesindex[clef];
}

void arborescenceXML::concatene(string clef,xmlNodePtr courant,long value) {
	xmlXPathObjectPtr listeNode=recherche(clef);
	if (listeNode==NULL) {
		listeNode=xmlXPathNewNodeSet(courant);            
		if (value!=-1)
			listeNode->user=(void*)value;
		lesindex[clef]=listeNode;		
	}
	else
		xmlXPathNodeSetAddUnique(listeNode->nodesetval,courant);
}


void arborescenceXML::raz() {
	for (int i=0;i<100;i++)
		lesclefs[i]="";
}

//--------------------------------------------------------------------------------------
chaXML::chaXML(unsigned char* c,char maj) {        
    chaine=DupliqueChaine(c);
    if (maj==1)
        Majuscule(chaine);
    else
        if (maj==2)
            minuscule(chaine);
}

//--------------------------------------------------------------------------------------
variXML::variXML(char* v) {
    variable=XMLVARINONE;
    for (int i=1;VARIABLESXML[i][0]!='%';i++)
        if (!strcmp(v,VARIABLESXML[i])) {
            variable=(VARIABLEXML)i;
            break;
        }
}

unsigned char* chaXML::Chaine() {
	return chaine;
}

//--------------------------------------------------------------------------------------
metXML::metXML(char* c) {
    methode=XMLUNKNOWN;

    if (!strcmp(c,"setattribute")) {
        methode=XMLCREATIONATTRIBUT;
        return;
    }

    if (!strcmp(c,"getattribute")) {
        methode=XMLASSIGNATIONCHAMP;
        return;
    }
    
    if (!strcmp(c,"gettext")) {
        methode=XMLASSIGNATION;
        return;
    }

    for (int i=1;INSTRUCTIONCHAINE[i][0]!='%';i++)
        if (!strcmp(c,INSTRUCTIONCHAINE[i])) {
            methode=(INSTRUCTION)i;
            break;
        }        
}

//--------------------------------------------------------------------------------------
double mathXML::TraitementFormule(GlobalParseur* parseur,VECTA<Noeud*>& vectNoeuds,string& buff,VECTA<ResultatFonction*>& focus) {
    if (formule==NULL)
        return 0;

    double r;
    char erreur=NON;
    if (formule->TypeString())
        r=formule->TraitementFormuleString(parseur,vectNoeuds,erreur,buff,&focus);
    else {
        r=formule->TraitementFormule(parseur,vectNoeuds,erreur,&focus);
		char res[10];
        sprintf(res,"%g",r);
		buff=res;
    }
    return r;
}
//--------------------------------------------------------------------------------------
dtdXML::dtdXML(char* n, XMLEVENEMENT ev) {
    vide=NULL;
    clef=new chaXML((uchar*)n,2);
    nom=(char*)clef->Chaine();
    evenement=ev;
    action=XMLDEPENDENCY;
    tr=NULL;
    marquage=NULL;
}

dtdXML::dtdXML(clefXML* n,XMLEVENEMENT ev) {
    vide=NULL;
    clef=n;
    //On extrait le nom de la regle
    //S'il y a un crochet le nom est juste avant
    int de=n->regles.dernier-2;
    if (n->regles[de]->nature==']') {
        while (n->regles[de]->nature!='[') de--;
        de--;
    }
    nom=(char*)n->regles[de]->Chaine();
    evenement=ev;
    action=XMLDEPENDENCY;
    tr=NULL;
    marquage=NULL;
}

dtdXML::~dtdXML() {
    //delete nom;
    if (marquage!=NULL)
        libere(marquage);
    if (clef!=NULL)
        delete clef;
}
//--------------------------------------------------------------------------------------
XPathIndexation::XPathIndexation(clefXML* cxml) {
    clef=cxml->Clef();
    listeXpath.ajoute(cxml);
}

XPathIndexation::~XPathIndexation() {
	for (int u=0;u<listeCleMinimale.dernier;u++)
		libere(listeCleMinimale[u]);
}

//--------------------------------------------------------------------------------------
arborescenceXML::arborescenceXML(xmlDocPtr doc,const unsigned char* pe,int rac,char* nf) {
    XmlNodeCourant=NULL;
    XmlCourant=NULL;
    XmlReferenceNode=NULL;        
	xpathcourant=NULL;

    document=doc;        
    pointentree=Majuscule(DupliqueChaine((uchar*)pe));
    principal=rac;
    xpath=NULL;        
    nomfichier=NULL;
    index=-1;
    ajout=NON;
    reference=NON;
    if (nf!=NULL)
        nomfichier=DupliqueChaine(nf);
}
//--------------------------------------------------------------------------------------
void arborescenceXML::trouveIndex(VECTA<XPathIndexation*>& xpathindex) {
    for (int ki=0;ki<xpathindex.dernier;ki++) {
        if (!compchmin(pointentree,xpathindex[ki]->clef)) {
            index=ki;
            return;
        }
    }
}

//--------------------------------------------------------------------------------------
void nettoieXmlPrivate(xmlNodePtr node) {
    
    xmlAttrPtr propriete;
    while (node!=NULL) {
        if (node->properties!=NULL) {
            propriete=node->properties;
            while (propriete!=NULL) {
                if (propriete->_private!=NULL) {
                    xmlFree(propriete->_private);
                    propriete->_private=NULL;
                }
                propriete=propriete->next;
            }            
        }
        
        if (node->_private!=NULL) {
            xmlFree(node->_private);
            node->_private=NULL;
        }

        if (node->children!=NULL)
            nettoieXmlPrivate(node->children);
        node=node->next;
    }
}

void arborescenceXML::retrouveXmlPrivate(xmlNodePtr ancetre,xmlNodePtr node,uchar* laclef,VECTA<uchar*>& attributs,int maxelements) {    
    xmlAttrPtr propriete;
    xpathvaluePtr xv;    
    if (node!=NULL) {
        if (node->_private!=NULL) {
            xv=(xpathvaluePtr)node->_private;
            attributs.affecte(xv->index-1,xv->str);
        }

        if (node->properties!=NULL) {
            propriete=node->properties;
            while (propriete!=NULL) {
                if (propriete->_private!=NULL) {
                    xv=(xpathvaluePtr)propriete->_private;
                    attributs.affecte(xv->index-1,xv->str);
                }
                propriete=propriete->next;
            }            
        }        
        retrouveXmlPrivate(ancetre,node->parent,laclef,attributs,maxelements);
    }
    else {
        string chaine;
        char ch[4];
        char* pt=(char*)laclef;
        char* debut=(char*)laclef;
        for (int i=0;i<attributs.dernier;i++) {
            if (attributs[i]==NULL)
                return;
            sprintf(ch,"_%d",i+1);
            pt=strstr((char*)pt,ch);
            if (pt==NULL)
                return;
            *pt=0;
            chaine+=debut;
            chaine+=(char*)attributs[i];
            *pt='_';pt+=2;
            debut=pt;
        }
        if (pt!=NULL)
            chaine+=pt;
        
		concatene(chaine,ancetre,maxelements);
    }    
}


//--------------------------------------------------------------------------------------
void arborescenceXML::indexer(VECTA<XPathIndexation*>& xpathindex,char conversion) {
    //le document est une liste de xmlNodePtr
    //Nous allons le parcourir en fonction de nos indexes.
    //On recherche d'abord le bon index.
    XPathIndexation* xpathind=NULL;
    int i;

    //Si pas d'instruction d'indexation, on revient
    if (index==-1)
        return;

    //On met a jour le namespace
    xmlNodePtr nn=document->children;
    xmlNsPtr cur=NULL;
    while (nn->type!=XML_ELEMENT_NODE)
        nn=nn->next;
    if (nn!=NULL)
        cur=nn->nsDef;
    
    //pour le contexte xpath a partir du namespace du document
    while (cur!=NULL) {
        xmlXPathRegisterNs(xpath, cur->prefix,cur->href);
        cur=cur->next;
    }

    xpathind=xpathindex[index];

    xmlXPathObjectPtr listeNode;
    uchar* laclef;
    VECTA<uchar*> attributs;    
    
    for (i=0;i<xpathind->listeXpath.dernier;i++) { 
        //On ne peut utiliser des XPath dont la partie variable
        //est precede d'un autre signe que l'egalite (ici considere comme
        //une affectation)
        if (xpathind->listeXpath[i]->reindexation==SAUTE)
            continue;
        laclef=xpathind->listeCleMinimale[i];
        if (laclef!=NULL) {                  
            *flot_erreur<<" Index:"<<i<<" = "<<(char*)laclef<<" --> ";
            listeNode=xmlXPathEval(laclef,xpath);
			int nbelements=0;
			if (listeNode!=NULL && listeNode->nodesetval!=NULL)
				nbelements=listeNode->nodesetval->nodeNr;
            //On jette un coup d'oeil sur la structure construite en memoire
            if (nbelements>0) {
                if (xpathind->listeXpath[i]->indexation==0)
                    //on cree directement l'entree et on conserve le listeNode
                    lesindex[(char*)laclef]=listeNode;
                else {										
                    for (int j=0;j<nbelements;j++) {
						attributs.raz();
                        retrouveXmlPrivate(listeNode->nodesetval->nodeTab[j],
                        listeNode->nodesetval->nodeTab[j],laclef,
                        attributs,nbelements);
                    }					
                    xmlXPathFreeObject(listeNode);
                }
            }
			*flot_erreur<<nbelements<<endl;
            nettoieXmlPrivate(document->children);
        }
    }
}


//--------------------------------------------------------------------------------------
/*Cette methode permet de rajouter une clexml dans xpathindex
Cette variable sera utilisee par la suite pour guider l'indexation
d'un fichier XML base de donnees.

  clefXML->reindexation peut avoir trois differentes valeurs:
    a) SAUTE dans ce cas cela signifie que la partie XIP de l'expression n'est pas precedee d'une affectation
     Par exemple: @lemma!="!#1[lemme]!". Ces expressions ne peuvent donc etre utilisees pour faire 
     de l'indexation et doivent etre systematiquement recalculee a la volee
    b) NON. Il s'agit d'expression XPath que l'on peut utiliser pour faire de l'indexation mais qui ne sont
    liees a aucune regle de construction de noeuds a la volee. Lorsqu'un noeud XML a ete rajoute, on doit
    les recalculer. Elles peuvent etre indexees a l'initialisation d'un fichier XML, 
    mais pas en cours d'analyse.
    c) Il s'agit d'expression qui utilisent le meme XPath qu'une regle de reconstruction. Il n'est pas besoin
    de les reindexer.

*/
void GlobalParseur::ajouteXpathIndex(clefXML* cxml) {

    //cette indexation n'a pas de sens pour le fichier courant en cours d'analyse
    if (!compchmin(cxml->Clef(),(uchar*)"_xtd") ||
        !compchmin(cxml->Clef(),(uchar*)"_current"))        
        return;

    string lesclefs[100];

    if (!compchmin(cxml->Clef(),(uchar*)"_currentxmlnode")) {
        cxml->reindexation=SAUTE;
        return;
    }

    uchar* clefminimale=NULL;
    int nb=cxml->ConstruitClefMinimale(this,lesclefs);    
    
    if (nb==0)
        return;    


    for (int iclef=0;iclef<nb;iclef++) {
        char ajout=OUI;
        clefminimale=USTR(lesclefs[iclef]);
        for (int i=0;i<xpathindex.dernier;i++) {
            if (!compchmin(cxml->Clef(),xpathindex[i]->clef)) {                        
                cxml->index=xpathindex[i]->listeCleMinimale.dernier;
                INSTRUCTION type=cxml->regles.fin()->Methode();
                //Dans cette boucle on verifie la compatibilite selon le type du XPath
                //Si ce XPath est un XMLCREATION, on le compare a tous les XPath qui n'en sont pas
                for (int j=0;j<xpathindex[i]->listeXpath.dernier;j++)  {
                    clefXML* courante=xpathindex[i]->listeXpath[j];
                    if (courante->reindexation!=NON)
                        continue;               
                    //S'il s'agit d'un XMLCREATION, alors on ne prend en compte aucun
                    //XMLCREATION, sinon uniquement ceux-la...
                    if (type==XMLCREATION) {
                        if (courante->regles.fin()->Methode()==XMLCREATION)
                            continue;
                    }
                    else {
                        if (courante->regles.fin()->Methode()!=XMLCREATION)
                            continue;
                    }
                    
                    if (cxml->VerifieCompatibilite(courante)) {
                        if (type==XMLCREATION)
                            courante->reindexation=OUI;
                        else { //dans ce cas on a trouve une expression, on s'arrete
                            cxml->reindexation=OUI;
                            break;
                        }
                    }
                    
                }
                char trouve=NON;
                for (int k=0;k<xpathindex[i]->listeCleMinimale.dernier;k++) {
                    if (!compchmin(xpathindex[i]->listeCleMinimale[k],clefminimale)) {
                        trouve=OUI;
                        break;
                    }
                }
                if (trouve==NON)
                    xpathindex[i]->listeCleMinimale.ajoute(DupliqueChaine(clefminimale));
                xpathindex[i]->listeXpath.ajoute(cxml);            
                ajout=NON;
                break;
            }
        }
        
        if (ajout==OUI) {
            XPathIndexation* xpathind=new XPathIndexation(cxml);
            xpathind->listeCleMinimale.ajoute(DupliqueChaine(clefminimale));    
            cxml->index=0;
            xpathindex.ajoute(xpathind);    
        }
    }
}

Noeud* mathXML::RenvoieNoeud(VECTA<Noeud*>& vectNoeuds) {
    if (formule->element->type==VALNOEUD) {
        TestFonctionNoeud* test=(TestFonctionNoeud*)((TestFormuleElementNoeud*)formule->element)->tf;
        return vectNoeuds[test->arggauche];
    }
    return NULL;
}

char TestSigne(clefXML* cle) {

    //On peut donc renvoyer quatre valeurs:
    // 0--> erreur
    // 1 --> chaine=
    // 2 --> /

    int pos=cle->regles.dernier-1;
    if (!pos || cle->regles[pos]->Type()!=XMLELEMENT )        
        return 0;    

    //Directement a gauche
    
    VECTA<eleXML*>& reg=cle->regles;

    if (reg[pos]->nature=='/')
        return 2;    

    if (reg[pos]->nature=='"') {
        pos--;
        if (!pos)
            return 0;
        if (reg[pos]->nature=='/')
            return 3;
        return 0;
    }
    
    if(reg[pos]->nature=='=') {
        if (reg[pos-1]->Type()!=XMLCHAINE)
            return 0;
        return 1;
    }

    return 0;        
}

//--------------------------------------------------------------------------------------
//Nous allons utiliser un codage xpath, sachant que les informations de XIP
//doivent necessairement etre enclose entre deux !.
//par defaut litXMLGuide = NON
clefXML*  GlobalParseur::ParseXPathExpression(istream& fichier,
                                     char& k,
                                     char* tampon,
                                     char negation,
                                     VECTA<char>& vari,
                                     char litXMLGuide) {


    eleXML* ele;
    chaXML* chxml;
    mathXML* mathxml;
    clefXML* clexml;
    string chaine;
    char ik;
    int pos;
    TestFormule* formule;
    string accumulationerreur;
    accumulationerreur=tampon;
	accumulationerreur+=k;

    char fermeparenthese=NON;
    

    if (litXMLGuide==NON) {
        if (k!='@')
            erreur(CHAINES[296]);
        //Le debut de l'expression commence necessairement par une @, suivi d'une "( ou d'une fleche"
        k=lit(fichier,tampon);
        if (k=='>' && tampon[strlen(tampon)-1]=='-') {
            tampon[strlen(tampon)-1]=0;
            k='(';
        }

        if (k!='(' && k!='.') 
            erreur(CHAINES[191]);

        if (k=='(')
            fermeparenthese=OUI;
        //On garde la trace de cet alias dans notre liste d'alias
        //Est un alias reserve?
        pos=0;
        while (aliasreserves[pos][0]!='@' && compchmin((uchar*)aliasreserves[pos],(uchar*)tampon)) pos++;
        if (aliasreserves[pos][0]=='@') {
            for (pos=0;pos<ListeAlias.dernier;pos++) {
                if (!strcmp(ListeAlias[pos],tampon)) {
                    pos=-1;
                    break;
                }
            }
            if (pos!=-1)
                ListeAlias.ajoute(DupliqueChaine(tampon));
        }
    }
    else
        strcpy(tampon,"xtdguide");

    if (tampon[0]==0)
        erreur(CHAINES[296]);

    clexml=new clefXML((uchar*)tampon);   
    char lindexation=NON;

    k=0;
    char etat=0;
    char fin=NON;  
    uchar K;
	char testegal=NON;
    char guillemet=NON;
    K=LectCrypte(fichier);
    char fleche_trouvee=NON;
	int comptecrochet=0;
    char traiter;
    while (!fichier.eof() && fin==NON) {
                     
        traiter=NON;
        switch(etat) {
        case 0: 
            switch(K) {
            case ')':
                fin=OUI;
                traiter=OUI;
                break;
            case '"':
                etat=6;
                traiter=OUI;
                break;                
            case '\\': //on saute un caractere
                K=LectCrypte(fichier);
                etat=5;
                traiter=OUI;
                break;
            case '#': //lecture d'une variable
                etat=7;
                break;
            case '[':
				comptecrochet++;
				etat=1;
				traiter=OUI;
				break;
            case ']': 
				comptecrochet--;
            case '>':
            case '<':            
            case '=':
            case '&':
            case '|':
            case '~':                        
            case '/':
                etat=1;
                traiter=OUI;
                break;
            case '-':
                etat=2;
                traiter=OUI;
                break;
            case '!':
                etat=4;
                traiter=OUI;
                break;
            default:
                if (K<=32) {
                    LectureBruteBlancs(fichier,K,NON);
                    etat=0;
                }
                else
                    etat=5;
            }
            if (chaine.size() && traiter==OUI) {
                ik=0;
                if (!compchmin(USTR(chaine),(uchar*)"not"))
                    ik='~';
                if (!compchmin(USTR(chaine),(uchar*)"and"))
                    ik='&';
                if (!compchmin(USTR(chaine),(uchar*)"or"))
                    ik='|';
                if (!ik) {
                    ik=';';
                    formule=NULL;
                    if (litXMLGuide==NON) {
						testegal=NON;
						//Pour gerer les cas ou @xxxx correspond a une instruction XIP
						if (chaine[0]=='@' && K=='=') {
							testegal=OUI;
							chaine+="_";
						}
						MetEnAttenteLecture(ik,STR(chaine));                
                        formule=LireUneFormule(fichier,ik,vari,NON);
						if (testegal==OUI)
							chaine=chaine.substr(0,chaine.size()-1);
                    }
                    if (formule==NULL) {
                        chxml=new chaXML(USTR(chaine),0);            
                        clexml->regles.ajoute(chxml);    
                        enAttente[0]=0;
                    }
                    else {
                        //On verifie s'il y un signe egal a gauche
                        pos=TestSigne(clexml);
                        if (pos==1) {
                            ele=new eleXML;                
                            ele->nature='"';
                            clexml->regles.ajoute(ele);                            
                            //accumulationerreur+=chaine;
                            accumulationerreur+=K;
                        }                        
                        mathxml=new mathXML;
                        mathxml->formule=formule;
                        clexml->regles.ajoute(mathxml);                        
                        if (pos==1) {
                            ele=new eleXML;                
                            ele->nature='"';
                            clexml->regles.ajoute(ele);
                            //accumulationerreur+=chaine;
                            accumulationerreur+=K;
                        }                        
                    }
                }
                else {
                    ele=new eleXML;                
                    ele->nature=ik;
                    clexml->regles.ajoute(ele);
                    accumulationerreur+=chaine;
                    accumulationerreur+=K;
                    etat=0;                    
                }
                chaine="";
            }
            break;                
        case 1: //un caractere de separation
            ele=new eleXML;                
            ele->nature=K;
            clexml->regles.ajoute(ele);
            accumulationerreur+=chaine;
            accumulationerreur+=K;
            K=LectCrypte(fichier);
            etat=0;
            break;
        case 2: //lecture d'une fleche?
            etat=3;
            K=LectCrypte(fichier);
            break;
        case 3: //lecture d'une fleche (final)
            if (K=='>') {
                fin=OUI;
                fleche_trouvee=OUI;
            }
            else {
                ele=new eleXML;                
                ele->nature='-';
                clexml->regles.ajoute(ele);
                accumulationerreur+=chaine;
                accumulationerreur+=K;
                etat=0;                
            }
            break;
        case 4: //instruction XIP
            K=lit(fichier,tampon);
            //negation du signe '=' --> !=
            if (K=='=') {
                ele=new eleXML;                
                ele->nature='!';
                clexml->regles.ajoute(ele);
                ele=new eleXML;                
                ele->nature=K;
                clexml->regles.ajoute(ele);
                accumulationerreur+=chaine;
                accumulationerreur+=K;
                K=LectCrypte(fichier);
                etat=0;
                break;
            }
            //On verifie l'indexation
            //On doit avoir un signe egal a gauche, ou une quote ou un /
            //Il ne faut pas que le signe egal soit precede d'autre chose qu'une chaine
            pos=TestSigne(clexml);
            if (pos==NON)
                lindexation=SAUTE;

            MetEnAttenteLecture(K,tampon);                
            k=K;formule=LireUneFormule(fichier,k,vari,OUI);K=k;
            mathxml=new mathXML;
            mathxml->formule=formule;
          
            //On rajoute des quotes de chaque cote
            ele=new eleXML;                
            ele->nature='"';
            clexml->regles.ajoute(ele);

            clexml->regles.ajoute(mathxml);
            if (K!='!')
                erreur(CHAINES[264]);
            K=LectCrypte(fichier);
            
            ele=new eleXML;                
            ele->nature='"';
            clexml->regles.ajoute(ele);

            etat=0;
            break;
        case 5://caractere normal
            chaine+=K;
            K=LectCrypte(fichier);
            etat=0;
            break;
        case 6: //une quote
            ele=new eleXML;                
            ele->nature=K;
            clexml->regles.ajoute(ele);
            accumulationerreur+=chaine;
            accumulationerreur+=K;
            K=LectCrypte(fichier);
            etat=8;
            guillemet=OUI;
            break;
        case 7:
            chaine+=K;
            K=LectCrypte(fichier);//on lit les chiffres
            while (K>='0' && K<='9') {
                chaine+=K;
                K=LectCrypte(fichier);
            }
            etat=10;
            break;
        case 8: //Sinon, on lit toute la chaine jusqu'a la prochaine quote
            if (K=='"') {      
                pos=TestSigne(clexml);
                //si nous sommes au milieu d'un chemin
                if (pos==3) {//on doit retirer la quote precedente
                    ele=clexml->regles.retire();
                    delete ele;
                }

                //cas particulier dans le cas ou le caractere a gauche est une '/'
                if (chaine.size()) {
                    chxml=new chaXML(USTR(chaine),0);            
                    clexml->regles.ajoute(chxml);    
                }
                
                chaine="";
                //si nous ne sommes pas au milieu d'un chemin
                //nous rajoutons notre quote
                if (pos!=3) {
                    ele=new eleXML;                
                    ele->nature='"';
                    clexml->regles.ajoute(ele);
                }
                accumulationerreur+=chaine;
                accumulationerreur+=K;                

                etat=0;  
                guillemet=NON;
            }
            else {
                if (K=='\\')
                    K=LectCrypte(fichier);                
                chaine+=K;                
                etat=8;
            }
            K=LectCrypte(fichier);
            break;
        case 10: //lecture d'une variable
            if (K=='[') {                
                while (K!=']') {
                    chaine+=K;
                    K=LectCrypte(fichier);
                }
                chaine+=K;
                K=LectCrypte(fichier);
            }
            else {
                if (K=='<') {
                    while (K!='>') {
                        chaine+=K;
                        K=LectCrypte(fichier);
                    }
                    chaine+=K;
                    K=LectCrypte(fichier);
                }
            }
            etat=0;
            break;
      }
      if (fin==OUI)
          break;
     }
    
	 if (comptecrochet!=0)
		 erreur(CHAINES[287]);


    if (fichier.eof()) {
        delete clexml;
        return NULL;
    }

    if ((fermeparenthese==OUI && K!=')') || guillemet==OUI) {
        accumulationerreur+="  -->  ";
        accumulationerreur+=CHAINES[55];
        erreur((char*)accumulationerreur.c_str());
    }

    //Nous debouchons ensuite sur une methode
    //On lit alors l'instruction:
    K=lit(fichier,tampon);
    accumulationerreur+=tampon;
    accumulationerreur+=K;

    //Il nous reste peut-etre la fleche a lire encore
    if (fleche_trouvee==NON) {
        if (K!='>' || tampon[0]!='-') {        
        accumulationerreur+="  -->  ";
        accumulationerreur+=CHAINES[54];
        erreur((char*)accumulationerreur.c_str());
        }
        
        K=lit(fichier,tampon);
        accumulationerreur+=tampon;
        accumulationerreur+=K;
    }
    
    if (K!='(') {        
        accumulationerreur+="  -->  ";
        accumulationerreur+=CHAINES[30];
        erreur((char*)accumulationerreur.c_str());
    }
    
    metXML* met=new metXML(tampon);
    
    if (met->methode==XMLUNKNOWN) {        
        accumulationerreur+="  -->  ";
        accumulationerreur+=CHAINES[248];
        erreur((char*)accumulationerreur.c_str());
    }
    
    //Dans le cas d'une creation d'element il faut evidemment
    //effectuer une demarche differente
    if (met->methode==XMLCREATION) {
        clexml->sauvegarder=OUI;
        met->sauvegarder=OUI;
    }
    
    if (met->methode==XMLCREATIONAVECAJOUT) {
        met->methode=XMLCREATION;
        clexml->sauvegarder=2;
        met->sauvegarder=2;
    }

    clexml->regles.ajoute(met);
    while (K!=')') {
        K=LitTantQue(fichier,(uchar*)tampon,"+!.,)/","\"");
		if (K=='+')
			erreur(CHAINES[393]);
        if (tampon[0]!=0) {//c'est encore une chaine
			if (tampon[strlen(tampon)-1]=='\"')
				ik=K;
			else
				ik=';';
            formule=NULL;
            if (litXMLGuide==NON && LuEntreCotes==NON) {
                MetEnAttenteLecture(ik,tampon);                
                formule=LireUneFormule(fichier,ik,vari,NON);
            }
            if (formule==NULL) {
                chxml=new chaXML((uchar*)tampon,0);            
                met->regles.ajoute(chxml);    
                enAttente[0]=0;
            }
            else {
                mathxml=new mathXML;
                mathxml->formule=formule;
                met->regles.ajoute(mathxml);                        
            }             
        }

        if (K=='/') {
            ele=new eleXML;                
            ele->nature='/';
            met->regles.ajoute(ele);                
            accumulationerreur+=tampon;
            accumulationerreur+=K;
        }
        
        
        //sinon c'est une formule mathematique
        //s'il s'agit d'une expression XIP
        //Il nous faut un parsage particulier
        if (K=='!') {
            k=K;formule=LireUneFormule(fichier,k,vari,OUI);K=k;
            mathxml=new mathXML;
            mathxml->formule=formule;
            met->regles.ajoute(mathxml);
            if (K!='!')
                erreur(CHAINES[264]);
        }
    }
    //On verifie s'il y a un point dans l'expression finale
    K=lit(fichier,tampon);

    if (litXMLGuide==OUI) {
        if (K!='.' && K!=';')
            erreur(CHAINES[305]);
    }
    else
        MetEnAttenteLecture(K,tampon);

    //Dans le cas d'une instruction que ne fait pas reference a _xtd (mot clef reserve pour controler le
    //fichier XML qui est en cours d'analyse), il nous faut conserver une liste des XPath pour lesquels 
    //une indexation de type automate est necessaire.
    //On recherche d'abord dans xpathindex l'element correspondant
    if (clexml->sauvegarder!=NON && lindexation==SAUTE) 
        erreur(CHAINES[290]);
    clexml->reindexation=lindexation;
    ajouteXpathIndex(clexml);
    k=K;
    return clexml;
}
                   
//--------------------------------------------------------------------------------------

clefXML* GlobalParseur::LectureXMLDB(istream& fichier,
                                     char& k,
                                     char* tampon,
                                     char negation,
                                     VECTA<char>& vari) {return NULL;}

//--------------------------------------------------------------------------------------
void arborescenceXML::creeXPathContext() {
    xpath=xmlXPathNewContext(document);    
}

xmlXPathObjectPtr arborescenceXML::EvalueXPath(xmlNodePtr xmlref,const uchar* xp) {
    if (xpath==NULL)
        return NULL;
    xmlXPathObjectPtr res;
    if (reference==OUI || xmlref!=NULL) {
        if (xmlref==NULL)
            return NULL;
        xpath->node=xmlref;
        xpath->doc=xmlref->doc;
    }
    res=xmlXPathEval(xp,xpath);
    return res;
}

//--------------------------------------------------------------------------------------

char indXML::AnalyseVariable(GlobalParseur* parseur,
                             string& valeur,
                             VECTA<Noeud*>& vectNoeuds,
                             VECTA<ResultatFonction*>& vectFonc) {

    return parseur->RecuperationTraitValeur(test,valeur,vectNoeuds,&vectFonc);
}
//--------------------------------------------------------------------------------------
char indXML::RecupereValeur(GlobalParseur* parseur,
                            string& valeur,
                            VECTA<Noeud*>& vectNoeuds,
                            VECTA<ResultatFonction*>& vectFonc) {

    Traits* tr=NULL;
    Noeud* n=NULL;
    if (test->Type()==TESTFONCTIONNOEUD) {
        tr=test->traits[0];
        n= vectNoeuds[test->arggauche];
        if (n==NULL)
            return NON;
    }
    else 
        return NON;

    if (tr!=NULL) {
        //On n'autorise pas la mise a jour du lemme
        if (tr->lemme!=NULL) {
            n->Lemme(valeur);
            return OUI;
        }
        else {
            if (tr->surface!=NULL) {
                n->Surface(valeur);
                return NON;
            }
        }
        
        //On recupere le nom de l'attribut
        char attribut[100];
        attribut[0]=0;
        tr->present->Attribut(parseur,attribut);
        trait* vattribut=parseur->ArbTraits->recherche(attribut);
        int rang=vattribut->indice>> gBitVectDivise;
        //Puis on construit le trait
        tr->present->Valeur(parseur,STR(valeur),rang);        
    }
    
    return OUI;
}

//--------------------------------------------------------------------------------------
//Cette methode est utilisee pour mettre a jour des traits dans un noeud
//a partir d'information recuperee dans un fichier XML
char indXML::MAJVariable(GlobalParseur* parseur,
                         char* valeur,
                         VECTA<Noeud*>& vectNoeuds) {
    Traits* tr=test->traits[0];
    Noeud* n= vectNoeuds[test->arggauche];
    if (n==NULL)
        return NON;
    if (tr!=NULL) {
        //On n'autorise pas la mise a jour du lemme
        if (tr->lemme!=NULL || tr->surface!=NULL)
            return NON;
        
        if (valeur[0]=='@') {
            parseur->ParseOntologie(valeur,tr);
            return OUI;
        }

        //On recupere le nom de l'attribut
        char attribut[100];
        attribut[0]=0;
        tr->present->Attribut(parseur,attribut);
        trait* vattribut=parseur->ArbTraits->recherche(attribut);
        if (vattribut==NULL) {
            AfficherTrace("Unknown attribute:"<<attribut);
            return NON;
        }
        
        //Puis on construit le trait
        trait* vvaleur=parseur->RetrouveValeurTrait(vattribut,valeur,NON);
        if (vvaleur==NULL) {
            AfficherTrace("Unknown value:"<<attribut<<":"<<valeur);
            return NON;
        }
        Traits regtr(parseur);
        regtr.ou(vvaleur);
        return ITR(parseur,&regtr,n->source->traitsbase,ECRIT);            
    }
    
    return NON;
}

//--------------------------------------------------------------------------------------
//positioninsertion;
//fininsertion;
//debuttraitement;
//fintraitement;


void extractionchaine(uchar* donnees,
                      int debut,
                      int fin,
                      string& mot) {
    
    //for (int i=debut;i<fin;i++)        
      //  mot=mot+(char)(donnees[i]); 
    if (donnees==NULL)
        return;
    char c=0;
    if (donnees[fin]) {
        c=donnees[fin];
        donnees[fin]=0;
    }
    mot=(char*)(donnees+debut);
    if (c)
        donnees[fin]=c;
}


void xmlparcours(xmlNodePtr n,VECTA<xmlNodePtr>& g) {
	if (n==NULL || n->type==XML_DOCUMENT_NODE)
		return;

	if (g.cherche(n)!=-1)
		return;

	g.ajoute(n);

	if (n->next!=NULL)
		xmlparcours(n->next,g);
	if (n->children!=NULL)
		xmlparcours(n->children,g);
}

void affichenode(xmlNodePtr x,char p) {
	if (x==NULL || x->type==XML_DOCUMENT_NODE)
		return;

	if (x->type==XML_TEXT_NODE)
		cout<<"'"<<x->content<<"' ";
	else {
		if (x->type==XML_ELEMENT_NODE) {
			cout<<x->name<<"{";
			xmlNodePtr courant=x->children;
			while (courant!=NULL) {
				affichenode(courant,0);
				courant=courant->next;
			}
			cout<<"}";
		}
	}
	if (p==1)
		cout<<endl;
}

char testloop(xmlNodePtr x) {
	static VECTA<xmlNodePtr> verif;
	verif.raz();

	while (x!=NULL) {
		if (verif.cherche(x)!=-1)
			return NON;
		verif.ajoute(x);
		x=x->parent;
	}
	return OUI;
}

char dansparents(xmlNodePtr courant,xmlNodePtr mere) {
	while (courant!=NULL) {
		if (courant==mere)
			return OUI;
		courant=courant->parent;
	}
	return NON;
}

int compteanalyse=0; 
void GlobalParseur::insertiondenoeuds(xmlNodePtr node,Noeud* nds,xmlChar* balise,char* attributs) {
    int i;
    if (nds==NULL)
        return;
	char* pt;


    string mot="";
	xmlNodePtr nouveau;
    //On se positionne alors en fin de noeud (pour gerer les cas de phrases semblables)        
#ifdef _DEBUG
    compteanalyse++;
#endif
	int debutnoeud,finnoeud;    
    //On recherche le noeud ou se trouve notre balise
    int bd=-1;
    int bf=-1;
	xmlChar* nombalise;
	xmlChar* champ=NULL;
    //Il s'agit ici des offsets de la sequence de noeuds couvertes par nds
    //de la position "debutnoeud" a la position "finnoeud"
    debutnoeud=nds->gauche;
    //Nous allons fonctionner sur la base d'une egalite
    finnoeud=nds->droite-1;
	char agauche=NON;

	if (alphrase.dernier==0)
		return;
	if (presencetoken==OUI) {
		//dans le cas d'une tokenisation, on se base sur les numeros de noeuds
		for (i=0;i<alphrase.dernier;i++) {
			if (nds->num_gauche==alphrase[i]->num) {
				bd=i;
				while (i<alphrase.dernier && nds->num_droite!=alphrase[i]->num)
					i++;
				if (i<alphrase.dernier) {
					agauche=OUI;
					bf=i;
				}
				break;        
			}
		}
		if (bd==-1 ||
			bf==-1 ||
			alphrase[bd]->noeud==NULL ||
			alphrase[bd]->noeud->parent==NULL ||
			alphrase[bd]->noeud->parent->parent==NULL ||
			alphrase[bf]->noeud->parent==NULL ||
			alphrase[bf]->noeud->parent->parent==NULL ||
			alphrase[bd]->noeud->parent->parent!=alphrase[bf]->noeud->parent->parent)
			return;
		//dans ce cas il suffit simplement de rajouter un noeud superieur...
		nombalise=xmlEncodeEntitiesReentrant(node->doc, (uchar*)balise);    
		Trim((char*)nombalise);
		//Puis on cree un nouveau noeud superieur
		nouveau=creeNouveauNoeudXML(node->doc, NULL,nombalise,champ);
		//On rajoute alors les attributs en decomposant notre chaine    
		pt=strchr(attributs,'=');
		while (pt!=NULL) {
			*pt=0;
			char* attribut=attributs+1;
			char* valeur;
			pt=strchr(pt+1,'"');
			if (pt!=NULL)
				valeur=pt+1;
			pt=strchr(pt+1,'"');
			*pt=0;
			AjouteAttributValeur(nouveau,attribut,valeur);
			attributs=pt+1;
			pt=strchr(attributs,'=');
		}
		//Nous avons nos deux noeuds
		xmlAddNextSibling(alphrase[bd]->noeud->parent, nouveau);
		for (i=bd;i<=bf;i++) {
			alphrase[i]->profondeur++;
			xmlUnlinkNode(alphrase[i]->noeud->parent);
			xmlAddChild(nouveau,alphrase[i]->noeud->parent);
		}
		return;
	}

	for (i=0;i<alphrase.dernier;i++) {
		if (alphrase[i]->noeud==NULL)
			continue;
		if (debutnoeud>=alphrase[i]->gauche && finnoeud<=alphrase[i]->droit) {
			bd=i;bf=i;            
			break;        
		}
	}
    
    if (i==alphrase.dernier) {
        //s'il s'agit d'une balise a cheval sur plusieurs balises
        //Nous verifions d'abord que l'on peut placer une telle balise
        
        for (i=0;i<alphrase.dernier;i++) {
            //On recherche d'abord la balise d'entree
            if (debutnoeud>=alphrase[i]->gauche && debutnoeud<=alphrase[i]->droit) {
                bd=i;        
            }
            //Mais aussi la balise de finnoeud
            if (finnoeud>=alphrase[i]->gauche && finnoeud<=alphrase[i]->droit) {
                bf=i;        
            }            
            if (bd!=-1 && bf!=-1)
                break;
        }

        if (bd==-1 || bf==-1)
            return;
        
        //Premier cas, la premiere balise est en profondeur mais son debut coincide avec le debut
        //du texte a baliser.
        if (alphrase[bd]->profondeur>alphrase[bf]->profondeur && alphrase[bd]->gauche==debutnoeud)
            agauche=OUI;
        else {
            //La contrainte pour pouvoir batir une balise dans cette situation
            //est que les balises de bd a bf soit de meme profondeur ou de profondeur
            //decroissante. Si la balise pointee par bd est plus profonde que celle pointee
            //par bf, on ne peut rien faire. 
            for (i=bd+1;i<=bf;i++) {
                if (alphrase[bd]->profondeur>alphrase[i]->profondeur)                
                    return;
            }
            //De meme il faut que la balise initiale soit a la meme profondeur 
            //que la balise finale (sauf si la balise finale est entierement integree)
            if (alphrase[bd]->profondeur>alphrase[bf]->profondeur && finnoeud<alphrase[bf]->droit)
                return;
        }

        //Deuxieme cas, la balise finale est en profondeur et sa fin ne coincide pas avec la fin
        //de notre chaine... C'est un cas impossible: yyy<Bf>yyyzz</Bf> (on veut marquer les 'y')
        if (alphrase[bf]->profondeur>alphrase[bd]->profondeur && alphrase[bf]->droit!=finnoeud)
            return;
    }

    if (bd==-1 || bf==-1)
        return;
    
    int finbd,debutbf;    
    //debut indique le debut de la chaine dans la balise de tete
    int debut=debutnoeud-alphrase[bd]->gauche;
    //fin indique la fin de la chaine dans la balise finale
    int fin=finnoeud-alphrase[bf]->gauche+1;
    
    //Si nous sommes dans la meme balise, finbd et debutbf ont les valeurs
    //de fin et debut
    if (bd==bf) {
        finbd=fin;
        debutbf=debut;
    }
    else {
        //en revanche, ici finbd correspond a la fin du texte dans la balise
        //de demarrage
        finbd=xmlStrlen(alphrase[bd]->noeud->content);        
        //debutbf correspond au debut dans la balise finale
        debutbf=0;        
        //finnoeud correspond a la fin dans la balise d'entree en terme de position
        //dans la chaine d'analyse.
        finnoeud=debutnoeud+finbd-debut;
    }

    

    //Premier noeud cree avec la nouvelle balise dont le contenu
    //comprend la partie extraite dans la balise de tete
    nouveau=alphrase[bd]->noeud;
    xmlNodePtr suivant;
    xmlNodePtr mere;
    aligne* ngauche=NULL;
    aligne* ndroit=NULL;

    //On cherche la profondeur maximale a remonter dans notre hierarchie de noeud
    int profmax=alphrase[bd]->profondeur;
    while (nouveau!=NULL && nouveau!=node) {
        nouveau=nouveau->parent;
        profmax--;
    }
    //prof nous sert de profondeur maximale

    int bfmax=bf;
    
    
    //Dans le cas ou il ne s'agit pas d'une balise enchassee
    //On cree un nouveau morceau de noeud texte
    if (agauche==NON) {
        mot="";
        extractionchaine(alphrase[bd]->noeud->content,debut,finbd,mot);        
        if (mot.length()>0)
            champ=(uchar*)mot.c_str();
    }
    
    int ipr;
    //On cree le nom de notre balise
    nombalise=xmlEncodeEntitiesReentrant(node->doc, (uchar*)balise);    
    Trim((char*)nombalise);
    //Puis on cree un nouveau noeud superieur
    nouveau=creeNouveauNoeudXML(node->doc, NULL,nombalise,champ);
    //On rajoute alors les attributs en decomposant notre chaine    
    pt=strchr(attributs,'=');
    while (pt!=NULL) {
        *pt=0;
        char* attribut=attributs+1;
        char* valeur;
        pt=strchr(pt+1,'"');
        if (pt!=NULL)
            valeur=pt+1;
        pt=strchr(pt+1,'"');
        *pt=0;
        AjouteAttributValeur(nouveau,attribut,valeur);
        attributs=pt+1;
        pt=strchr(attributs,'=');
    }
    
	xmlNodePtr reference=nouveau;
    xmlNodePtr frere;
	xmlNodePtr lepere=NULL;

    if (agauche==NON) {        
        //Mise-a-jour de notre structure alphrase
        //il s'agit du nouveau noeud text integre dans notre arbre XML
        //On garde sa trace pour remettre alphrase a jour...    
		mere=alphrase[bd]->noeud;
        ngauche=new aligne(nouveau->children,debutnoeud,finnoeud,-1);
        suivant=xmlAddNextSibling(mere, nouveau);
		lepere=mere->parent;
		profmax=alphrase[bd]->profondeur-1;
        frere=suivant;        
    }
    else {
        //On remonte a partir du noeud courant pour trouver le noeud parent adequat
        mere=alphrase[bd]->noeud;
        for (ipr=alphrase[bd]->profondeur-1;ipr>profmax;ipr--)
            mere=mere->parent;

        //On rajoute nouveau avant le premier noeud
        xmlAddPrevSibling(mere,nouveau);        
        //puis on delie le noeud courant
        xmlUnlinkNode(mere);
        //et on le place sous le nouveau noeud
        xmlAddChild(nouveau,mere);
        suivant=nouveau;
        frere=mere;        
    }

    //desormais, le noeud a gauche s'arrete la ou commence le noeud "text" suivant
    finnoeud=alphrase[bd]->droit;
    alphrase[bd]->droit=debutnoeud-1;
   
    //Puis pour tous les noeuds suivant de profondeur inferieur, on les rajoute comme enfant a suivant
    xmlNodePtr courant=NULL;
    for (i=bd+1;i<=bf;i++) {
        //on remonte de la profondeur sus-mentionnee, avec "profmax" comme borne superieure
        mere=alphrase[i]->noeud;
		char modifieparent=NON;
		for (ipr=alphrase[i]->profondeur-1;ipr>profmax;ipr--) {
			if (lepere==mere) {
				//cas particulier, d'une balise dans une balise...
				//dans ce cas, le probleme vient de ce que l'on risque de construire un circuit.
				//la balise nouvelle vient donc d'etre incorporee comme fils de la balise de base
				//alphrase[i]->noeud...Il nous faut donc, debarrasser suivant de son mauvais parent
				modifieparent=OUI;
			}
            mere=mere->parent;
		}
        //si ce noeud a deja ete ajoute, on passe a la suite
        if (mere==courant || dansparents(suivant,mere) || mere==lepere)
            continue;
        
        courant=mere;
        //On le retire de la hierarchie
		if (modifieparent==OUI)
			xmlUnlinkNode(suivant);
		else
			//On libere la mere de sa position dans la hierarchie
			xmlUnlinkNode(mere);

        //On le rajoute comme enfant sous le nouveau noeud courant
        int tailledernier=0;
        
        //Dans le cas ou il y a un danger de fusion de noeuds textes...
        if (i==bf && suivant->last!=NULL && suivant->last->type==XML_TEXT_NODE)
            tailledernier=xmlStrlen(suivant->last->content);
        xmlNodePtr res=xmlAddChild(suivant,mere);

        //dans le cas ou la mere a ete detruite (son texte a ete sauvagement ajoute dans le noeud
        //precedent, surtout dans le cas du dernier noeud, ce noeud devient le nouveau bf
        if (res!=mere) {
            alphrase[i]->noeud=res;
			fin+=tailledernier;
        }
    }

    //de deux choses l'une
    //soit nous sommes dans la meme balise
    //et dans ce cas le reste du texte est le reste du texte dans la balise
    nouveau=alphrase[bf]->noeud;
    if (bd!=bf) {
        //Si dans la balise finale, seule une partie du texte doit etre extrait,
        //Nous creons un nouveau noeud texte qui contient la partie restante
        if (fin<xmlStrlen(alphrase[bf]->noeud->content)) {
            mot="";
            //Soit donc la chaine allant de 0 jusqu'a fin (on teste sur <=)
            extractionchaine(alphrase[bf]->noeud->content,0,fin,mot);
            if (mot.length()>0) {
                champ=(uchar*)mot.c_str();
                //Puis on ecrase le debut de la chaine avec ce qui suit
                strcpy((char*)alphrase[bf]->noeud->content,(char*)alphrase[bf]->noeud->content+fin);

                //On retient donc la position de cette sous-chaine qui va etre dans un nouveau noeud text
                //Son debut coincide evidement avec le precedent debut
                debutnoeud=alphrase[bf]->gauche;
                //sa fin se calcule en fonction du retrecissement
                finnoeud=nds->droite-1;

                nouveau=xmlNewDocText(node->doc, champ);
                xmlUnlinkNode(alphrase[bf]->noeud);
                xmlAddNextSibling(suivant, alphrase[bf]->noeud);
                //On le rajoute sous suivant
                nouveau=xmlAddChild(suivant, nouveau);
                ndroit=new aligne(nouveau,debutnoeud,finnoeud,-1);
                //le debut du noeud suivant est la fin
                alphrase[bf]->gauche=finnoeud+1;                                
                alphrase.insere(bf,ndroit);
                bfmax++;
                if (ngauche!=NULL) {
                    alphrase.insere(bd+1,ngauche);
                    bfmax++;
                }
            }
        }
    }
    else { 
        //On a donc coupe notre noeud courant (et unique) pour en extraire un morceau.
        //on sauvegarde la fin du texte sous la forme d'un nouveau noeud         
        if (alphrase[bf]->noeud->content!=NULL && alphrase[bf]->noeud->content[fin]!=0) {
            champ=alphrase[bf]->noeud->content+fin;
            nouveau=xmlNewDocText(node->doc, champ);
            xmlAddNextSibling(suivant, nouveau);
            //le debut du noeud suivant est la fin de ce qui vient d'etre insere
            ndroit=new aligne(nouveau,ngauche->droit+1,finnoeud,-1);                            
            alphrase.insere(bd+1,ngauche);                      
            alphrase.insere(bd+2,ndroit);
            bfmax+=2;
        }
        else {   
            alphrase.insere(bd+1,ngauche);        
            bfmax++;
        }
    }


    //Desormais dans notre premier noeud le debut de ce qui a ete retiree
    //devient la borne finale du champ content de ce noeud
    if (alphrase[bd]->noeud->content!=NULL && agauche==NON)
        alphrase[bd]->noeud->content[debut]=0;           


    //mise-a-jour des profondeurs;
    for (i=bd;i<=bfmax;i++) {
        nouveau=alphrase[i]->noeud;
		if (nouveau==NULL)
			continue;
        profmax=0;
        while (nouveau->parent!=NULL) {
            profmax++;
            nouveau=nouveau->parent;
        }
        alphrase[i]->profondeur=profmax;            
    }
}
//--------------------------------------------------------------------------------------
char recupere_donnees_xml(GlobalParseur* parseur,
                          xmlNodePtr node,
                          TestFormule* formule) {
    
    if (formule->element->type!=VDICTIONNAIRE || node==NULL || node->type==XML_DOCUMENT_NODE)
        return NON;

    TestFormuleElementVectorString* vvect=(TestFormuleElementVectorString*)formule->element;
    VariableDictionnaire* vdict=(VariableDictionnaire*)vvect->var;        
    if (vdict==NULL)
        return NON;
    vdict->raz();
    string index="#tag";
    string valeur=(char*)node->name;
    vdict->AffecteValeurStringIndexChaine(new string(valeur),index);
    xmlAttrPtr propriete;
    if (node->properties!=NULL) {
        propriete=node->properties;
        while (propriete!=NULL) {                
            index=(char*)propriete->name;
            valeur=(char*)propriete->children->content;
            vdict->AffecteValeurStringIndexChaine(new string(valeur),index);            
            propriete=propriete->next;
        }           
    }
    return OUI;
}

char initialise_variable(xmlNodePtr node,TestFormule* formule) {
    if (formule->element->type!=VXMLOBJET)
        return NON;

	TestFormuleElementXML* vvxml=(TestFormuleElementXML*)formule->element;
	VariableXML* vxml=(VariableXML*)vvxml->vxml;
	vxml->noeud=node;
	return OUI;
}

//Retourne la valeur d'un attribut

string ValeurAttribut(xmlNodePtr node,string& nom) {
	xmlAttrPtr  propriete=NULL;
	if (node->properties!=NULL) {
		propriete=node->properties;            
		while (propriete!=NULL) {                
			if (nom==(char*)propriete->name) {
				string base=(char*)propriete->children->content;
				return base;
			}
			propriete=propriete->next;
		}
	}            
	return "";
}

char ValeursSaufAttribut(xmlNodePtr node,string& nom,map<string,string>& valeurs) {
	xmlAttrPtr  propriete=NULL;
	char trouve=NON;
	if (node->properties!=NULL) {
		propriete=node->properties;            
		while (propriete!=NULL) {                
			if (nom!=(char*)propriete->name)
				valeurs[(char*)propriete->name]=(char*)propriete->children->content;
			propriete=propriete->next;
			trouve=OUI;
		}
	}            
	return trouve;
}


//--------------------------------------------------------------------------------------
char metXML::ExecuteMethode(GlobalParseur* parseur,
                            arborescenceXML* arboxml,
                            xmlNodePtr pere,
                            xmlNodePtr node,
                            VECTA<uchar*>& lesresultats,
                            VECTA<Noeud*>& vectNoeuds,
                            VECTA<ResultatFonction*>& vectFonc) {
    
	string base;
    string inter;
    string nombalise;
    string lesclefs;
    xmlAttrPtr propriete;
    int i;
    xmlChar* valeur;
    VECTA<char*> chemin;
    double res;
    string encodage;
    string nomfichier;
    Noeud* nds;
    mathXML* math;
    xmlNsPtr cur;
	xmlNodePtr xmlnouveau;

    switch(methode) {
    case XMLNETTOYAGE:
        if (arboxml==parseur->basexmlcourante)
            return OUI;
        //On detruit la structure en memoire correspondant a arboxml
        for (i=0;i<parseur->BaseDonneesXML.dernier;i++) {
            if (parseur->BaseDonneesXML[i]==arboxml) {
                parseur->BaseDonneesXML.retireElement(i);
                delete arboxml;
                break;
            }
        }
        return OUI;
    case XMLSAUVEGARDE:
        //le nom du fichier est dans le premier element de la regle
        //Le deuxieme parametre optionnel permet d'indiquer un codage specifique
        if (regles[0]->Type()==XMLCHAINE)
            nomfichier=(char*)regles[0]->Chaine();
        else
            if (regles[0]->Type()==XMLMATH) {
                regles[0]->TraitementFormule(parseur,vectNoeuds,base,vectFonc);
                nomfichier=base;
            }
            else
                return NON;
        
        encodage=parseur->encodage;
        
        if (regles[1]!=NULL) {
            if (regles[1]->Type()==XMLCHAINE)
                encodage=(char*)regles[1]->Chaine();
            else
                if (regles[1]->Type()==XMLMATH) {
                    regles[1]->TraitementFormule(parseur,vectNoeuds,inter,vectFonc);
                    encodage=inter;
                }
        }
		if (nomfichier=="")
			return NON;
        xmlKeepBlanksDefault(0);    
        xmlSaveFormatFileEnc(STR(nomfichier), node->doc,STR(encodage),1);
        return OUI;
    case XMLCONTENU:
        if (node==NULL || node->type==XML_DOCUMENT_NODE || regles.dernier==0)
            return NON;
        if (node->children !=NULL && node->children->content!=NULL) {
            xmlUnlinkNode(node->children);
            xmlFreeNode(node->children);
        }
    case XMLCREATION:
        //Creation d'un nouveau noeud dont le contenu peut etre present dans le premier parametres        
        if (regles.dernier==0)
            return OUI;   

        base="";
        //si c'est un XMLCREATIONAJOUT, il n'y a que deux parametres
        if (sauvegarder==2) {
            if (!ConstruitUneClef(parseur,NULL,base,vectNoeuds,vectFonc,0,regles.dernier-1))
                return NON;
        }
        else {
            if (!ConstruitUneClef(parseur,NULL,base,vectNoeuds,vectFonc,0,regles.dernier))
                return NON;
        }
        
		//valeur = xmlCodage(parseur,node->doc, USTR(base));
		valeur=xmlCharStrdup((const char*)USTR(base));
        //On stocke tout dans le champ contenu associe au noeud
        xmlnouveau=xmlAddChild(node,xmlNewDocText(node->doc,valeur));        
        arboxml->XmlCourant=node;
        return OUI;
    case XMLINITIALHIT:
        if (regles.dernier==0)
			return NON;
		if (regles[0]->Type()==XMLMATH) {
            math=(mathXML*)regles[0];
            return math->formule->AffecteVariable(parseur,NULL,vectNoeuds,NULL,arboxml->tailleinitiale);       
            arboxml->XmlCourant=node;
            return SORTIEANALYSE;
        }
        return NON;
    case XMLHIT:
        if (regles.dernier==0)
			return NON;
		if (regles[0]->Type()==XMLMATH) {
            math=(mathXML*)regles[0];
            return math->formule->AffecteVariable(parseur,NULL,vectNoeuds,NULL,arboxml->taille);       
            arboxml->XmlCourant=node;
            return SORTIEANALYSE;
        }
        return NON;
    case XMLTEST:
        if (regles.dernier==0) {
            arboxml->XmlCourant=node;
            return SORTIEANALYSE;
        }
        return NON;
    case XMLCOMPARAISONSOUSCHAINE:
    case XMLCOMPARAISON:        
        //Si l'on est arrive jusque la, c'est qu'il s'agit d'une verification
        if (regles.dernier==0)
            return NON;
        //On compare le contenu avec une chaine ou avec un noeud
        if (!ConstruitUneClef(parseur,NULL,lesclefs,vectNoeuds,vectFonc,0,regles.dernier))
            return NON;
        
        if (node!=NULL && node->children!=NULL && node->children->content!=NULL) {
            base=(char*)node->children->content;
            Trim(base);
            if (methode==XMLCOMPARAISONSOUSCHAINE) {
				if (base.find(lesclefs)!=string::npos) {
                    arboxml->XmlCourant=node;
                    return SORTIEANALYSE;   
                }
            }
            else {
                //On compare avec le champ text            
                if (!compchmin(USTR(lesclefs),USTR(base))) {
                    arboxml->XmlCourant=node;
                    return SORTIEANALYSE;   
                }
            }
        }
        return NON;    
    case XMLCHAMP:
        if (regles.dernier==0)
            return NON;
        
        if (node->properties!=NULL) {
            propriete=node->properties;
            if (!ConstruitUneClef(parseur,NULL,lesclefs,vectNoeuds,vectFonc,0,regles.dernier))
                return NON;
            while (propriete!=NULL) {                
                if (!compchmin((uchar*)propriete->name,USTR(lesclefs))) {
                    base=(char*)propriete->children->content;
                    Trim(base);
                    lesresultats.ajoute((uchar*)DupliqueChaine(STR(base)));
                    arboxml->XmlCourant=node;
                    return OUI;
                }
                
                propriete=propriete->next;
            }            
            return NON;
        }
    case XMLRENOMMER:
        //Renommer un noeud XML
        if (regles.dernier!=1 || (regles[0]->Type()!=XMLCHAINE && regles[0]->Type()!=XMLMATH))
            return NON;
        //node est le noeud courant que l'on va renommer
        if (node->name!=NULL) {
            xmlFree((xmlChar*)node->name);
            if (regles[0]->Type()==XMLCHAINE)                
                valeur=xmlCodage(parseur,node->doc, regles[0]->Chaine());    
            else {
                regles[0]->TraitementFormule(parseur,vectNoeuds,base,vectFonc);
                valeur=xmlCodage(parseur,node->doc, USTR(base));    
            }
            node->name=valeur;
            return OUI;
        }
        return NON;    
    case XMLMETNAMESPACE:
    case XMLCREATIONATTRIBUT:
        if (regles.dernier!=2 || (regles[0]->Type()!=XMLCHAINE && regles[0]->Type()!=XMLMATH))
            return NON;
        base="";
        //Il s'agit d'une assignation de traits provenant du champ texte a un noeud
		if (regles[1]->Type()==XMLINDEX) {
            regles[1]->RecupereValeur(parseur,base,vectNoeuds,vectFonc);                   
		}
        else {
            if (regles[1]->Type()==XMLMATH) {                
                regles[1]->TraitementFormule(parseur,vectNoeuds,base,vectFonc);                                
            }
            else
                base=(char*)regles[1]->Chaine();
        }

        if (regles[0]->Type()==XMLCHAINE)                
            valeur=regles[0]->Chaine();    
        else {
            regles[0]->TraitementFormule(parseur,vectNoeuds,inter,vectFonc);
            valeur=USTR(inter);
        }        

        if (methode==XMLMETNAMESPACE) {
            //if it is a namespace allocation
            cur=xmlNewNs(node->doc->children, XSTR(base), (xmlChar*)valeur);
            if (arboxml!=parseur->basexmlcourante && arboxml->xpath!=NULL && cur!=NULL)
                xmlXPathRegisterNs(arboxml->xpath, cur->prefix,cur->href);
        }
        else
            parseur->AjouteAttributValeur(node,(char*)valeur,STR(base));
        arboxml->XmlCourant=node;
        return OUI;
    case XMLASSIGNATIONCHAMP:       
        if (regles.dernier!=2)
            return NON;

        if (regles[1]->Type()==XMLCHAINE)                
            valeur=regles[1]->Chaine();    
        else {
            if (regles[1]->Type()==XMLMATH) {
                regles[1]->TraitementFormule(parseur,vectNoeuds,inter,vectFonc);
                valeur=USTR(inter);
            }
            else
                return NON;
        }
    case XMLASSIGNATION:     
        if (methode==XMLASSIGNATION && regles.dernier!=1)
            return NON;

        if (regles[0]->Type()!=XMLINDEX && regles[0]->Type()!=XMLMATH)
            return NON;
                
        if (methode==XMLASSIGNATIONCHAMP) {


            propriete=NULL;
            if (node->properties!=NULL) {
                propriete=node->properties;            
                while (propriete!=NULL) {                
                    if (!compchmin((uchar*)propriete->name,valeur)) {
                        base=(char*)propriete->children->content;
                        Trim(base);
                        break;
                    }                
                    propriete=propriete->next;
                }            
            }
            if (propriete==NULL)
                return NON;
        }
        else {
            if (node!=NULL && node->children!=NULL && node->children->content!=NULL) {
                base=(char*)node->children->content;
                Trim(base);
            }
        }

        //Il s'agit d'une assignation de traits provenant du champ texte a un noeud
        if (regles[0]->Type()==XMLINDEX)
            return regles[0]->MAJVariable(parseur,STR(base),vectNoeuds);                   
        else {
            math=(mathXML*)regles[0];
            return math->formule->AffecteVariable(parseur,USTR(base),vectNoeuds,NULL);       
        }
      case XMLINSERTION:
          if (regles[0]->Type()!=XMLMATH)
              return NON;

          if (regles[1]->Type()==XMLCHAINE)                
              nombalise=(char*)regles[1]->Chaine();    
          else {
              if (regles[1]->Type()==XMLMATH)
                  regles[1]->TraitementFormule(parseur,vectNoeuds,nombalise,vectFonc);                  
              else
                  return NON;
          }


          nds=regles[0]->RenvoieNoeud(vectNoeuds);
          if (nds==NULL)
              return NON;
          base="";
          for (i=2;i<regles.dernier;i+=2) {
              if (regles[i+1]==NULL)
                  return NON;

              switch (regles[i]->Type()) {
              case XMLCHAINE:
                  base+=" ";
                  base+=(char*)regles[i]->Chaine();
                  base+="=";
                  break;
              case XMLMATH:
				  inter="";
                  base+=" ";
                  res=regles[i]->TraitementFormule(parseur,vectNoeuds,inter,vectFonc);
                  if (inter.size()==0)
                      return NON;
                  base+=inter;
                  base+="=";
                  break;
              default:
                  return NON;
              }

              switch (regles[i+1]->Type()) {
              case XMLCHAINE:
                  base+="\"";
                  base+=(char*)regles[i+1]->Chaine();
                  base+="\"";
                  break;
              case XMLMATH:
                  base+="\"";
				  inter="";
                  res=regles[i+1]->TraitementFormule(parseur,vectNoeuds,inter,vectFonc);
                  if (inter.size()==0)
                      inter="~";
                  base+=inter;
                  base+="\"";
                  break;
              default:
                  return NON;
              }
          }
          parseur->insertiondenoeuds(node,nds,XSTR(nombalise),STR(base));
          break;
      case XMLLITDONNEES:
          if (regles[0]->Type()!=XMLMATH)
              return NON;
          if (recupere_donnees_xml(parseur,node,((mathXML*)regles[0])->formule)==NON)
              return NON;
          break;
      case XMLPOSITIONPARENT:
          node=node->parent;
          arboxml->XmlReferenceNode=node;
          break;
	  case XMLMETDANSVAR://set
          if (regles[0]->Type()!=XMLMATH)
              return NON;
          if (initialise_variable(node,((mathXML*)regles[0])->formule)==NON)
              return NON;
		  break;
    }
    arboxml->XmlCourant=node;
    return OUI;
}



//------------------------------------------------------------------------------
//On verifie et on renvoie le noeud XML en suivant les champs fournis dans regles
char clefXML::AnalyseClefNodeXML(GlobalParseur* parseur,
                                 xmlNodePtr noeud,
                                 int position,
                                 arborescenceXML* arboxml,
                                 VECTA<uchar*>& lesresultats,
                                 VECTA<Noeud*>& vectNoeuds,
                                 VECTA<ResultatFonction*>& vectFonc) {
    
    int i=position;    

    //Execution de la regle proprement dite
    if (regles[i]==NULL || regles[i]->Type() != XMLMETHODE)
        return NON;

    if (regles[i]->Methode()==XMLSAUVEGARDE || regles[i]->Methode()==XMLNETTOYAGE)
        if (arboxml!=parseur->basexmlcourante)
            noeud=arboxml->document->children;
    
    if (noeud==NULL)
        return NON;  
    
    //On conserve une reference pour le cas d'une utilisation via reference (via _currentxmlnode)
    //Si l'on est dans un _currentxmlnode, evidemment cette reference n'est pas remise en cause
    if (arboxml!=parseur->basexmlcourante && arboxml->reference==NON)
        arboxml->XmlReferenceNode=noeud;
    return regles[i]->ExecuteMethode(parseur,arboxml,NULL,noeud,lesresultats,vectNoeuds,vectFonc);
}

//------------------------------------------------------------------------------
//Verification du XPath par rapport au noeud courant
int clefXML::TestNode(GlobalParseur* parseur,
                      VECTA<Noeud*>& vectNoeuds,
                      VECTA<ResultatFonction*>& vectFonc,
                      xmlNodePtr node,
                      xmlXPathContextPtr xpath) {
    

    if (node==NULL || node->type==XML_DOCUMENT_NODE)
        return -1;

    //cas particulier, on tombe tout de suite sur la methode
    if (regles.dernier>2 && regles[2]->Type()==XMLMETHODE && node->name!=NULL &&
        !compchmin((uchar*)node->name,(uchar*)regles[1]->Chaine()))
        return 2;

    char trouve=NON;
    string laclef;
    ConstruitLaClef(parseur,laclef,vectNoeuds,vectFonc);
    _xmlNode* courant=node;
    
    
    if (laclef.size()!=0) {
        //xmlXPathContextPtr xpath=xmlXPathNewContext(node->doc);
        //On met a jour le namespace
        xmlNodePtr nn=node->doc->children;
        xmlNsPtr cur=NULL;
        while (nn!=NULL && nn->type!=XML_ELEMENT_NODE)
            nn=nn->next;
        if (nn!=NULL)
            cur=nn->nsDef;
        
        //pour le contexte xpath a partir du namespace du document
        while (cur!=NULL) {
            xmlXPathRegisterNs(xpath, cur->prefix,cur->href);
            cur=cur->next;
        }

        if (node->nsDef!=NULL) {
            cur=node->nsDef;
            
            //pour le contexte xpath a partir du namespace du document
            while (cur!=NULL) { 
                xmlXPathRegisterNs(xpath, cur->prefix,cur->href);
                cur=cur->next;
            }
        }
        
        xmlXPathObjectPtr listeNode = xmlXPathEval(XSTR(laclef),xpath);
        if (listeNode!=NULL && listeNode->nodesetval!=NULL) {
            for (int i=0;i<listeNode->nodesetval->nodeNr;i++) {
                if (node==listeNode->nodesetval->nodeTab[i]) {
                    trouve=OUI;
                    break;
                }
            }
            
            xmlXPathFreeObject(listeNode);
        }
        
        //xmlXPathFreeContext(xpath);
        if (trouve==OUI)
            return regles.dernier-1;
    }
    return -1;
}

char TestLettres(char* x) {
	int ln=strlen(x);
    for (int i=0;i<ln;i++)
        if (!isalpha(x[i]))
            return NON;
    return OUI;
}

//----------------------------------------------------------------------------------------------------
//Creation d'un nouveau noeud


char clefXML::CreerNoeudXML(GlobalParseur* parseur,                               
                            arborescenceXML* arboxml,
                            xmlNodePtr pere,                            
                            VECTA<Noeud*>& vectNoeuds,
                            VECTA<ResultatFonction*>& vectFonc) {
    

    xmlNodePtr courant=pere;
    xmlNodePtr node;
    xmlNodePtr noderef=NULL;
    xmlNodePtr ancien=NULL;
    
    xmlChar* attribut=NULL;
    xmlChar* valeur=NULL;
    char propriete=NON;
    char premier=NON;
    int dernier;
    int i,borne;
    
    int index_chaine;
    int position_xpath;
    string chaine;        
    xmlChar* buff;
    double res;
    int suivant;    
    string laclef;
    int nbvaleurs=0;

    //Tout d'abord, on recherche le noeud le plus profond correspondant, pour
    //trouver le point d'accroche du nouveau noeud.
    //Il nous faut dans un premier temps trouver la profondeur adequate
    courant=pere;
    xmlXPathObjectPtr listeNode;
    dernier=regles.dernier-2;
    metXML* met=NULL;
    int nb_parents;

    suivant=regles.dernier-1;
    while (suivant>=0 && regles[suivant]->Type()!=XMLMETHODE) suivant--;
    if (suivant>=0)
        met=(metXML*)regles[suivant];


    if (arboxml==parseur->basexmlcourante) {
        //Si nous n'avons pas de fichier XML en cours, on repart
        if (arboxml->XmlNodeCourant==NULL)
            return NON;

        borne=1;        
        //Si le pere est deja le noeud racine, alors le noeud
        //courant est le noeud courant et on n'a plus besoin
        //d'evaluer les chemins.
        if (pere==(xmlNodePtr)pere->doc) {
            courant=arboxml->XmlNodeCourant;
            dernier=borne;
        }
        else {
            ancien=arboxml->xpathcourant->doc->children;
            //On change la racine de l'arbre, si la racine que l'on a detecte correspond
            if (pere!=NULL && pere->name!=NULL)
                arboxml->xpathcourant->doc->children=pere;
        }
    }
    else {
        borne=arboxml->principal;
        if (arboxml->reference==OUI) {
            noderef=arboxml->XmlReferenceNode;
            if (noderef==NULL)
                return NON;
        }
    }
    
    i=dernier;
    uchar* lenouvelindex=NULL;
    int dernieradetruire;

    //Cas particulier, ou on attaque directement la methode
    if (i<borne) {
        dernier=dernier+1;
        //dans ce cas, l'element courant est celui pointe par
        //XmlCourant.
        //En revanche, s'il n'y a pas d'element courant, c'est une erreur
        if (arboxml->XmlCourant==NULL)
            return NON;
        courant=arboxml->XmlCourant;
    }
    else {
        int maxi=1;
        //s'il s'agit d'un index gere par automate
        if (arboxml!=parseur->basexmlcourante) {            
            //On verifie l'existence de la chaine dans nos indexes:            
			arboxml->raz();
            ConstruitClef(parseur,arboxml,arboxml->lesclefs,vectNoeuds,vectFonc,1,regles.dernier-1,nbvaleurs);   
            //Dans le cas d'une construction par reference, on ne fait aucune verification de doublon,
            //on vient simplement se raccrocher au noeud courant...
            if (arboxml->reference==OUI) {
                index_chaine=0;
                dernier=0;
                dernieradetruire=nbvaleurs-1;
            }
            else {
                listeNode=arboxml->recherche(arboxml->lesclefs[nbvaleurs-2]);
                dernieradetruire=nbvaleurs-1;
                index_chaine=nbvaleurs;
                if (listeNode!=NULL) {
                    dernier=(int)(uchar)arboxml->lesclefs[dernieradetruire][0];                
                    //On prend le premier noeud
                    if (listeNode->nodesetval!=NULL && listeNode->nodesetval->nodeNr>0)
                        if (arboxml->reference==NON)
                            courant=listeNode->nodesetval->nodeTab[0];            
                }
                else {
                    //On verifie quand meme si ce nouvel index n'existe pas deja
                    dernieradetruire-=2;
                    //sinon, on construit tous les noeuds manquants
                    dernier=0;
                    lenouvelindex=USTR(arboxml->lesclefs[nbvaleurs-2]);
                    for (index_chaine=0;index_chaine<nbvaleurs;index_chaine+=2) {                    
                        //On recherche pour une partie de la clef les noeuds d'attache potentiels
                        listeNode=arboxml->recherche(arboxml->lesclefs[index_chaine]);                        
                        if (listeNode!=NULL && 
                            listeNode->nodesetval!=NULL && 
                            listeNode->nodesetval->nodeNr>0) {
                            //On prend le premier noeud
                            courant=listeNode->nodesetval->nodeTab[0];
                            dernier=(int)(uchar)arboxml->lesclefs[index_chaine+1][0];
                        }
                        else
                            break;                                     
                    }
                }
            }
        }
        else {     
            //s'il s'agit d'un chemin absolu, on ne veut pas evaluer la racine du document
            if (regles.dernier>3 && regles[1]->nature=='/' && regles[1]->nature!=regles[2]->nature)
                maxi=3;
            
            suivant=ConstruitClef(parseur,arboxml,arboxml->lesclefs,vectNoeuds,vectFonc,1,regles.dernier-1,nbvaleurs);
            for (index_chaine=nbvaleurs-1;index_chaine>=1;index_chaine-=2) {            
                dernier=(int)(uchar)arboxml->lesclefs[index_chaine][0];
                
                if (index_chaine<maxi)
                    break;
                
                //On recherche pour une partie de la clef les noeuds d'attache potentiels
                
                listeNode=xmlXPathEval(XSTR(arboxml->lesclefs[index_chaine-1]),arboxml->xpathcourant);
                if (listeNode!=NULL && listeNode->nodesetval!=NULL && listeNode->nodesetval->nodeNr>0) {
                    //On prend le premier noeud
                    courant=listeNode->nodesetval->nodeTab[0];   
                    break;
                }
                xmlXPathFreeObject(listeNode);               
            }
        }
    }

    index_chaine-=2;
    //s'il s'ajout d'un creation avec ajout
    if (sauvegarder==2) {
        
        //Il s'agit de dupliquer les noeuds avec les memes informations
        //On remonte d'un cran dans les noeuds
        //Il faut recuperer le deuxiemme parametre de CreateAdd
        nb_parents=1;
        int dernier_met=met->regles.dernier-1;
        if (met !=NULL && met->regles[dernier_met]!=NULL && met->regles[dernier_met]->Type()==XMLMATH) {                
            laclef="";
            met->regles[dernier_met]->TraitementFormule(parseur,vectNoeuds,laclef,vectFonc);
            nb_parents=atoi(STR(laclef));
        }

        int calc_position=nbvaleurs-(nb_parents+1)*2;
        if (index_chaine>calc_position) {
            position_xpath=index_chaine-calc_position;
            //On divise par deux
            nb_parents=position_xpath>>1;
            index_chaine-=position_xpath;
            dernier=(int)(uchar)arboxml->lesclefs[index_chaine+1][0]; 
            lenouvelindex=USTR(arboxml->lesclefs[nbvaleurs-2]);
            while (nb_parents!=0) {
                courant=courant->parent;
                nb_parents--;
            }
        }
    }

    //Dans le cas d'une recherche par XPath, ou l'on a modifie la racine de l'arbre XML associe a un document
    //On lui redonne son ancienne valeur.
    if (ancien!=NULL)
        arboxml->xpathcourant->doc->children=ancien;


    //Cas particulier. Le XPath ne correspond a rien de connu
    //Il nous faut eviter de creer un noeud XML qui englobe 
    //le nom de reference dans arboxml
    if (arboxml!=parseur->basexmlcourante && dernier==0)
        dernier=1;

    char guillemet=NON;

    
    if (index_chaine>=0) 
        laclef=arboxml->lesclefs[index_chaine];
    else
        laclef="";

    for (i=dernier;i<regles.dernier;i++) {
        
        //On passe alors a l'execution de la methode
        if (regles[i]->Type()==XMLMETHODE)
            break;
        
        switch (regles[i]->Type()) {
        case XMLMATH:
            chaine="";
            res=regles[i]->TraitementFormule(parseur,vectNoeuds,chaine,vectFonc);   
            laclef+=chaine;
            //if (parseur->TestOptions(AFFICHAGE_UTF8))
            //    strcpy(chaine,(char*)parseur->conversionAsciiVersUTF8((uchar*)chaine));
            //De deux choses l'une, c'est soit la valeur d'un attribut
            if (attribut!=NULL) {
                 valeur=xmlCharStrdup(STR(chaine));                
            }
            else {//soit un noeud XML
                //On cree alors un nouveau noeud XML, que l'on rajoute dans l'arbre
                buff=xmlCodage(parseur,courant->doc, USTR(chaine));
                Trim((char*)buff);
                node=creationNouveauNoeudXML(courant->doc, NULL,buff,NULL);
                xmlAddChild(courant,node);
                courant=node;
                if (arboxml!=parseur->basexmlcourante && arboxml->reference==NON) {
                    //Si ce XPath etait deja dans l'automate, on l'enrichit
                    //sinon on cree un listeNode dans lequel on range notre noeud courant
					arboxml->concatene(laclef,courant);
                }
            }
            break;
        case XMLCHAINE:
            laclef+=(char*)regles[i]->Chaine();
            //s'il s'agit d'un nouveau noeud
            if (!propriete) {
                //On cree alors un nouveau noeud XML, que l'on rajoute dans l'arbre
                buff=xmlCodage(parseur,courant->doc, regles[i]->Chaine());
                Trim((char*)buff);
                node=creationNouveauNoeudXML(courant->doc, NULL,buff,NULL);
                xmlAddChild(courant,node);
                courant=node;
                if (arboxml!=parseur->basexmlcourante && arboxml->reference==NON) {
                    //Si ce XPath etait deja dans l'automate, on l'enrichit
                    //sinon on cree un listeNode dans lequel on range notre noeud courant
					arboxml->concatene(laclef,courant);
                }
            }
            else {
                if (premier==OUI)
                    attribut=xmlCodage(parseur,courant->doc, regles[i]->Chaine());
                else 
                    valeur=xmlCharStrdup((char*)regles[i]->Chaine());                
                premier=NON;
            }
            break;
        case XMLELEMENT:
            //Il s'agit d'une analyse des proprietes
            guillemet=NON;
            switch(regles[i]->nature) {
            case '[':        
                laclef+=regles[i]->nature;
                propriete=OUI;
                premier=OUI;
                attribut=NULL;
                valeur=NULL;
                break;
            case ']':
                laclef+=regles[i]->nature;
                propriete=NON;
                if (arboxml!=parseur->basexmlcourante && arboxml->reference==NON) {
                    //Si ce XPath etait deja dans l'automate, on l'enrichit
                    //sinon on cree un listeNode dans lequel on range notre noeud courant
					arboxml->concatene(laclef,courant);
                }
  
                parseur->AjouteAttributValeur(courant,(char*)attribut,(char*)valeur);
  
                if (attribut!=NULL)
                    xmlFree(attribut);
                if (valeur!=NULL)
                    xmlFree(valeur);
                attribut=NULL;
                valeur=NULL;
                break;
            case '&':
                //On rajoute alors les proprietes du nom
                laclef+=operatorstrings[0];
                parseur->AjouteAttributValeur(courant,(char*)attribut,(char*)valeur);
                if (attribut!=NULL)
                    xmlFree(attribut);
                if (valeur!=NULL)
                    xmlFree(valeur);
                propriete=OUI;
                premier=OUI;
                attribut=NULL;
                valeur=NULL;            
                break;
            default:
                laclef+=regles[i]->nature;
            }
        }
    }
    
    //Execution de la regle proprement dite
    if (regles[i]==NULL || regles[i]->Type() != XMLMETHODE)
        return NON;
    
    //Si courant est nouveau noeud qui vient d'etre cree, on met a jour l'index
    if (lenouvelindex!=NULL && arboxml!=parseur->basexmlcourante && arboxml->reference==NON) {        
        //Si ce XPath etait deja dans l'automate, on l'enrichit
        //sinon on cree un listeNode dans lequel on range notre noeud courant
		arboxml->concatene((char*)lenouvelindex,courant);
        arboxml->ajout=OUI;
    }

    if (arboxml!=parseur->basexmlcourante && arboxml->reference==NON)
        arboxml->XmlReferenceNode=courant;

    VECTA<uchar*> lesresultats;
    return regles[i]->ExecuteMethode(parseur,arboxml,pere,courant,lesresultats,vectNoeuds,vectFonc);
}


//----------------------------------------------------------------------------------------------------
//Cette methode execute la methode associee a une clef
char clefXML::ExecuteClef(GlobalParseur* parseur,
                          arborescenceXML* arboxml,
                          VECTA<uchar*>& lesresultats,
                          VECTA<Noeud*>& vectNoeuds,
                          VECTA<ResultatFonction*>& vectFonc,
                          xmlresultat* xmlres) {
    
    string lesclefs;
    xmlNodePtr noeud;

	arboxml->tailleinitiale=0;
	arboxml->taille=0;
    //Cas particulier, nous pointons directement sur une methode
    //On doit alors utiliser le noeud courant
    if (regles[1]->Type()==XMLMETHODE) {
        if (arboxml==parseur->basexmlcourante || arboxml->reference==NON)
            xmlres->ajoute(1,arboxml,arboxml->XmlCourant);
        else {
            if (arboxml->XmlReferenceNode==NULL)
                return NON;
            xmlres->ajoute(1,arboxml,arboxml->XmlReferenceNode);
        }
        return OUI;    
    //return AnalyseClefNodeXML(parseur,arboxml->XmlCourant,1,arboxml,lesresultats,vectNoeuds,vectFonc);
    }

    //Tout d'abord, il faut analyser la clef elle-meme
    //suivant pointe sur le premier element d'execution

    int suivant=ConstruitUneClef(parseur,arboxml,lesclefs,vectNoeuds,vectFonc,1,regles.dernier);
    char calc=NON;    
    xmlNodePtr noderef=NULL;
    if (arboxml!=parseur->basexmlcourante && arboxml->reference==OUI)
        noderef=arboxml->XmlReferenceNode;
    xmlXPathObjectPtr listeNode = NULL;
    char libere=NON;
    if (arboxml!=parseur->basexmlcourante) {
        listeNode=arboxml->recherche(lesclefs);
        if (listeNode==NULL) {
        /*Plusieurs situations:
        a) Il s'agit d'un XPath de type SAUTE, on doit l'evaluer a la volee 
        (aucune indexation n'est possible)
        b) Il s'agit d'un XPath de type NON et un noeud a ete rajoute, on doit l'evaluer a la volee
        (il n'y a la non plus aucune nouvelle indexation)
        c) dans tous les autres cas, c'est une erreur
            */
            if (reindexation==SAUTE || (reindexation==NON && arboxml->ajout==OUI)) {
                listeNode=arboxml->EvalueXPath(noderef,USTR(lesclefs));
                libere=OUI;
            }
        }
    }
    else {//sinon, c'est la cas ou aucune indexation n'est necessaire
        listeNode=arboxml->EvalueXPath(noderef,USTR(lesclefs));        
        libere=OUI;
    }
    
    if (listeNode!=NULL && listeNode->nodesetval!=NULL && listeNode->nodesetval->nodeNr>0) {
		arboxml->tailleinitiale=(long)listeNode->user;
		arboxml->taille=listeNode->nodesetval->nodeNr;
        for (int truc=0;truc<listeNode->nodesetval->nodeNr;truc++) {
            noeud=listeNode->nodesetval->nodeTab[truc];            
            if (noeud!=NULL) {
				if (xmlres->xmlnodes.cherche(noeud)==-1)
					xmlres->ajoute(suivant,arboxml,noeud);
                calc=OUI;
            }            
        }
    }
    if (libere==OUI)
        xmlXPathFreeObject(listeNode);
    
    return calc;
}    
//----------------------------------------------------------------------------------------------------
//Cette methode reconstruit une clef a partir de regles en s'arretant a profondeur

//----------------------------------------------------------------------------------------------------
//Cette methode reconstruit une clef a partir de regles en s'arretant a profondeur
int clefXML::ConstruitClef(GlobalParseur* parseur,
                           arborescenceXML* arboxml,
                           string* lesclefs,
                           VECTA<Noeud*>& vectNoeuds,
                           VECTA<ResultatFonction*>& vectFonc,
                           int premierElement,
                           int dernierElement,
                           int& nbelements) {
    
    string laclef;
    string inter;
    char propriete=NON;
    char premier=NON;
    int i;
    VECTA<uchar*> resultats;
    VECTA<uchar*> intermediaires;
    double res;    
    char guillemet=NON;
    int crochet=0;
    int pos=0;
    nbelements=0;
	char ch[]={0,0};

    for (i=premierElement;i<dernierElement;i++) {        
        if (regles[i]->Type()==XMLMETHODE)
            break;

        switch(regles[i]->Type()) {
        case XMLMATH:
			inter="";
            res=regles[i]->TraitementFormule(parseur,vectNoeuds,inter,vectFonc);      
            if (parseur->TestOptions(AFFICHAGE_UTF8))                
                inter=(char*)parseur->conversionAsciiVersUTF8(USTR(inter));
            laclef+=inter;
            break;
        case XMLCHAINE:
            laclef+=(char*)regles[i]->Chaine();
            break;
        case XMLELEMENT:
            guillemet=NON;
            //cas particulier: & | ~
            switch(regles[i]->nature) {
            case '&':
                laclef+=operatorstrings[0];
                break;
            case '|':
                laclef+=operatorstrings[1];
                break;
            case '~':
                laclef+=operatorstrings[2];
                break;
            case '[':
                crochet++;
                propriete=OUI;
                laclef+=regles[i]->nature;
                break;
            case '=':
                laclef+=regles[i]->nature;
                break;
            case '\"':
                guillemet=OUI;
                laclef+=regles[i]->nature;
                break;
            case ']':
                propriete=NON;
                crochet--;
                laclef+=regles[i]->nature;
                break;
            case '/':
                //Dans le cas de chemin decroissant
                //On garde chaque etape intermediaire
                if (laclef.size()!=0) {
                    lesclefs[nbelements++]=laclef;     
					ch[0]=(uchar)i;
                    lesclefs[nbelements++]=ch;
                }
            default:
                laclef+=regles[i]->nature;
            }            
            break;
        case XMLINDEX:  
            inter="";
            if (!regles[i]->AnalyseVariable(parseur,inter,vectNoeuds,vectFonc))
                return regles.dernier;
            //if (parseur->TestOptions(AFFICHAGE_UTF8))                
            //    inter=(char*)parseur->conversionAsciiVersUTF8((uchar*)inter));           
            laclef+=inter;
        }
    }
    
    if (laclef.size()!=0) {
        lesclefs[nbelements++]=laclef;
		ch[0]=(uchar)i;
        lesclefs[nbelements++]=ch;
    }

    return i;
}

int clefXML::ConstruitUneClef(GlobalParseur* parseur,
                           arborescenceXML* arboxml,
                           string& laclef,
                           VECTA<Noeud*>& vectNoeuds,
                           VECTA<ResultatFonction*>& vectFonc,
                           int premierElement,
                           int dernierElement) {
    
    string  inter;
    laclef="";
    char propriete=NON;
    char premier=NON;
    int i;
    VECTA<uchar*> resultats;
    VECTA<uchar*> intermediaires;
    double res;    
    char guillemet=NON;
    int crochet=0;
    int pos=0;
        
    for (i=premierElement;i<dernierElement;i++) {        
        if (regles[i]->Type()==XMLMETHODE)
            break;

        switch(regles[i]->Type()) {
        case XMLMATH:
			inter="";
            res=regles[i]->TraitementFormule(parseur,vectNoeuds,inter,vectFonc);      
            if (parseur->TestOptions(AFFICHAGE_UTF8))                
                inter=(char*)parseur->conversionAsciiVersUTF8(USTR(inter));                       
            laclef+=inter;
            break;
        case XMLCHAINE:
            laclef+=(char*)regles[i]->Chaine();
            break;
        case XMLELEMENT:
            guillemet=NON;
            //cas particulier: & | ~
            switch(regles[i]->nature) {
            case '&':
                laclef+=operatorstrings[0];
                break;
            case '|':
                laclef+=operatorstrings[1];
                break;
            case '~':
                laclef+=operatorstrings[2];
                break;
            case '[':
                crochet++;
                propriete=OUI;
                laclef+=regles[i]->nature;
                break;
            case '=':
                laclef+=regles[i]->nature;
                break;
            case '\"':
                guillemet=OUI;
                laclef+=regles[i]->nature;
                break;
            case ']':
                propriete=NON;
                crochet--;
                laclef+=regles[i]->nature;
                break;
            default:
                laclef+=regles[i]->nature;
            }            
            break;
        case XMLINDEX:  
            inter="";
            if (!regles[i]->AnalyseVariable(parseur,inter,vectNoeuds,vectFonc))
                return regles.dernier;
            laclef+=inter;
        }
    }
    
    return i;
}

//On construit une chaine dans laquelle on expurge les attributs de type
//XIP
int clefXML::ConstruitClefMinimale(GlobalParseur* parseur,string* lesclefs) {
       
    string laclef;
    char ch[10];
    int i;
    char guillemet=NON;
    int dernierElement=regles.size()-1;  
    int num=1;
    int crochet=0;
    int nb=0;

    for (i=1;i<dernierElement;i++) {        
        if (regles[i]->Type()==XMLMETHODE)
            break;
        
        switch(regles[i]->Type()) {
        case XMLCHAINE:
            laclef+=(char*)regles[i]->Chaine();
            break;
        case XMLELEMENT:
            guillemet=NON;
            //cas particulier: & | ~
            switch(regles[i]->nature) {
            case '&':
                laclef+=operatorstrings[0];
                break;
            case '|':
                laclef+=operatorstrings[1];
                break;
            case '~':
                laclef+=operatorstrings[2];
                break;
            case '[':
                crochet++;
                laclef+=regles[i]->nature;
                break;
            case '=':
                laclef+=regles[i]->nature;
                break;
            case '/':
                if (laclef.size()!=0)
                    lesclefs[nb++]=laclef;
            case '\"':                
                laclef+=regles[i]->nature;
                break;
            case ']':
                crochet--;
                laclef+=regles[i]->nature;
                break;
            default:
                laclef+=regles[i]->nature;
            }            
            break;
            case XMLMATH:
            case XMLINDEX:                
                sprintf(ch,"_%d",num++);
                laclef+=ch;
                break;
        }
    }
    if (laclef.size()!=0 && nb) {
        lesclefs[nb++]=laclef;
       indexation=num-1;
    }
    
    return nb;
}

//On construit une chaine dans laquelle on expurge les attributs de type
//XIP
void clefXML::ConstruitLaClef(GlobalParseur* parseur,
                              string& laclef,
                              VECTA<Noeud*>& vectNoeuds,
                              VECTA<ResultatFonction*>& vectFonc) {
           
    string  inter;

    laclef="";
    int i;
    char guillemet=NON;
    int dernierElement=regles.size()-1;  
    int num=1;
    int crochet=0;
    
    for (i=1;i<dernierElement;i++) {        
        if (regles[i]->Type()==XMLMETHODE)
            break;
        
        switch(regles[i]->Type()) {
        case XMLMATH:
			inter="";
            regles[i]->TraitementFormule(parseur,vectNoeuds,inter,vectFonc);      
            //if (parseur->TestOptions(AFFICHAGE_UTF8))                
            //    inter=(char*)parseur->conversionAsciiVersUTF8((uchar*)inter));                       
            laclef+=inter;
            break;
        case XMLCHAINE:
            laclef+=(char*)regles[i]->Chaine();
            break;
        case XMLELEMENT:
            guillemet=NON;
            //cas particulier: & | ~
            switch(regles[i]->nature) {
            case '&':
                laclef+=operatorstrings[0];
                break;
            case '|':
                laclef+=operatorstrings[1];
                break;
            case '~':
                laclef+=operatorstrings[2];
                break;
            case '[':
                crochet++;
                laclef+=regles[i]->nature;
                break;
            case '=':
                laclef+=regles[i]->nature;
                break;
            case '\"':
                laclef+=regles[i]->nature;
                break;
            case ']':
                crochet--;
                laclef+=regles[i]->nature;
                break;
            default:
                laclef+=regles[i]->nature;
            }            
            break;
        }
    }
}

//On traduit notre expression minimale en un automate
char clefXML::VerifieCompatibilite(clefXML* avec) {
       
    int i,j=1;
    if (regles.size() != avec->regles.size())
        return NON;

    int dernierElement=regles.size()-1;  
    
    for (i=1;i<dernierElement;i++) {        
        
        //if (j>=avec->regles.size())
            //return NON;

        if (regles[i]->Type()==XMLMETHODE) {
            if (avec->regles[j]->Type()==XMLMETHODE)
                return OUI;
            else
                return NON;
        }
        
        if (avec->regles[j]->Type()==XMLMETHODE)
            return NON;

        //Dans le cas d'une variable, on la saute
        if (regles[i]->Type()==XMLMATH ||
            regles[i]->Type()==XMLINDEX) {
            if (avec->regles[j]->Type()!=XMLCHAINE &&
                avec->regles[j]->Type()!=regles[i]->Type())
                return NON;
            j++;
            continue;
        }
         
        //La comparaison n'est pas necessaire
        if (avec->regles[j]->Type()==XMLMATH ||
            avec->regles[j]->Type()==XMLINDEX) {
            if (regles[i]->Type()!=XMLCHAINE &&
                avec->regles[j]->Type()!=regles[i]->Type())
                return NON;
            j++;
            continue;
        }
        

        if (regles[i]->Type() != avec->regles[j]->Type())
            return NON;

        switch(regles[i]->Type()) {
        case XMLCHAINE:            
            if (compchmin(regles[i]->Chaine(),avec->regles[j]->Chaine()))
                return NON;
            break;
        case XMLELEMENT:
            if (regles[i]->nature != avec->regles[j]->nature)
                return NON;
        }
        j++;
    }
    return OUI;
}

//--------------------------------------------------------------------------------------
char TestTypeRegle(clefXML* entree,int i) {
    if (entree->regles[i]->Type() == XMLMETHODE)
        return NON;
    if (strstr((char*)entree->regles[i]->Chaine(),"::"))
        return NON;
    return OUI;
}
//--------------------------------------------------------------------------------------

char GlobalParseur::ExecuteXML(clefXML* entree,
                               VECTA<Noeud*>& vectNoeuds,
                               VECTA<ResultatFonction*>& vectFonc,
                               xmlresultat* xmlres) {
    //On isole la premiere chaine...
    VECTA<uchar*> laClef;
    char trouve=NON;
    int i;    
    
    
    //Cas particulier, si entree->Clef() vaut xtd (_current is a synonym)
    if (!compchmin(entree->Clef(),(uchar*)"_xtd") ||
        !compchmin(entree->Clef(),(uchar*)"_current")) {        
        //Dans ce cas, il s'agit de verifier si un noeud appartient au chemin courant
        //Il s'agit forcement d'un test, on remonte dans le temps
        xmlNodePtr courant=NULL;
        //On remonte jusqu'au premier champ
        if (entree->regles.fin()->Methode()==XMLCREATION) {
            i=1;
            while (i<entree->regles.dernier && entree->regles[i]->Type()==XMLELEMENT) i++;
            if (TestTypeRegle(entree,i)) {
                courant=basexmlcourante->XmlNodeCourant;
                while (courant!=NULL && courant->name!=NULL) {
                    if (!compchmin((uchar*)courant->name,(uchar*)entree->regles[i]->Chaine()))
                        break;
                    courant=courant->parent;
                }
            }
            
            if (courant==NULL)
                courant=basexmlcourante->XmlNodeCourant;
            entree->CreerNoeudXML(this,basexmlcourante,courant,vectNoeuds,vectFonc);
            return OUI;
        }
        else {
            if (entree->regles.dernier==2 && entree->regles[1]->Type()==XMLMETHODE)
                i=1;
            else 
                i= entree->TestNode(this,vectNoeuds,vectFonc,basexmlcourante->XmlNodeCourant,basexmlcourante->xpathcourant);
            
            if (i==-1)
                return NON;
            
            xmlres->ajoute(i,basexmlcourante,basexmlcourante->XmlNodeCourant);
            return OUI;
            //return entree->AnalyseClefNodeXML(this,XmlNodeCourant,i,NULL,laClef,vectNoeuds,vectFonc);        
        }
    }        

#ifdef _DEBUG
    uchar* lenom=entree->Clef();
#endif
    //recherche dans la base de donnee d'un fichier partageant le meme debut que le XPath
    //Ce debut correspond a l'alias du fichier: le @mydb
    for (i=0;i<BaseDonneesXML.dernier;i++) {
        if (!compchmin(entree->Clef(),BaseDonneesXML[i]->pointentree)) {
            trouve=OUI;
            //Dans tous les cas de type non _currentxmlnode, on remet le pointeur XmlReference a NULL
            //de cette facon, si l'execution conduit a une erreur, nous sommes protegees en cas de
            //tentative d'utilise de cette reference par un _currentxmlnode
            if (BaseDonneesXML[i]->reference==NON)
                BaseDonneesXML[i]->XmlReferenceNode=NULL;
            if (entree->sauvegarder==NON) {
                //s'il ne s'agit pas d'une creation de noeud
                //On construit la clef de recherche
                return entree->ExecuteClef(this,BaseDonneesXML[i],laClef,vectNoeuds,vectFonc,xmlres);                    
            }
            else {//sinon on cree un noeud
                if (BaseDonneesXML[i]->reference==NON)
                    return entree->CreerNoeudXML(this,BaseDonneesXML[i],BaseDonneesXML[i]->document->children,vectNoeuds,vectFonc);
                else
                    return entree->CreerNoeudXML(this,BaseDonneesXML[i],BaseDonneesXML[i]->XmlReferenceNode,vectNoeuds,vectFonc);
            }
        }
    }
    
    arborescenceXML* axml;
    if (trouve==NON && !compchmin(entree->Clef(),(uchar*)"_currentxmlnode")) {
        i=1;
        while (i<entree->regles.dernier && entree->regles[i]->Type()==XMLELEMENT) i++;
        axml = new arborescenceXML(NULL,entree->Clef(),i+1);
        axml->reference=OUI;        
        BaseDonneesXML.ajoute(axml);
        axml->creeXPathContext();
        if (entree->sauvegarder==NON)
            return entree->ExecuteClef(this,axml,laClef,vectNoeuds,vectFonc,xmlres);
        else            
            return entree->CreerNoeudXML(this,axml,BaseDonneesXML[i]->XmlReferenceNode,vectNoeuds,vectFonc);        
    }

    //Dans le cas d'une nouvelle creation de Noeud
    if (trouve==NON && entree->sauvegarder!=NON) {
        string buffer;
        string clefminimale;
        string clefautomate;
        
        //On cree d'abord un noeud factice englobant. Il s'agit de l'element suivant
        //dans la regle. Puisque la clef est en fait un alias defini par l'utilisateur
        //qui ne doit pas apparaitre dans le fichier XML.
        //On verifie evidemment que cet element existe
        i=1;
        while (i<entree->regles.dernier && 
            entree->regles[i]->Type()==XMLELEMENT) i++;
        //Creation d'un nouveau document a la volee
        if (entree->regles[i]!=NULL) {
            xmlKeepBlanksDefault(0);

            
            entree->ConstruitUneClef(this,NULL,buffer,vectNoeuds,vectFonc,i-1,entree->regles.dernier);
            clefminimale=buffer;
            if (clefminimale.size()==0)
                return NON;
            if (clefminimale[0]!='/') {
                laClef.nettoie();
                return NON;
            }
			//On veut donc isoler la racine...
			//deux cas de figures, on rencontre soit un '/', soit un '['
            char* pt= STR(clefminimale)+1;
			while (*pt!=0) {
				if (*pt=='/')
					break;
				if (*pt=='[') {
					while (*pt!=']') pt++;
				}
				pt++;
			}
            if (pt!=NULL)
                *pt=0;
            clefautomate=clefminimale;
            //On retire alors les '['
            pt = strchr(STR(clefminimale),'[');
            if (pt!=NULL) {
                *pt=' ';
            //Puis le ']'
                pt = strchr(STR(clefminimale),']');
                if (pt!=NULL)
                    *pt=0;
                
                //Puis le @
                pt = strchr(STR(clefminimale),'@');
                while (pt!=NULL) {
                    *pt=' ';
                    pt = strchr((char*)pt+1,'@');
                }
                //Enfin, les and
                pt = strstr(STR(clefminimale)," and ");
                while (pt!=NULL) {
                    pt[1]=' ';
                    pt[2]=' ';
                    pt[3]=' ';
                    pt = strstr(STR(clefminimale)," and ");
                }                
            }

			buffer="<";
			buffer+=STR(clefminimale)+1;
			buffer+="/>";
            //On cree la racine de notre document
            xmlDocPtr doc=xmlSAXParseMemory(NULL,STR(buffer),buffer.size(),0);
            if (doc==NULL)
                return NON;

            doc->encoding=xmlCharStrdup(encodage);
            axml = new arborescenceXML(doc,entree->Clef(),i+1);
            BaseDonneesXML.ajoute(axml);
            axml->creeXPathContext();
            axml->trouveIndex(xpathindex);
            
            pt=strchr(STR(clefautomate),'[');
            if (pt!=NULL) {                
                xmlXPathObjectPtr listeNode=xmlXPathNewNodeSet(doc->children);
				axml->lesindex[clefautomate]=listeNode;
                *pt=0;
            }

            xmlXPathObjectPtr listeNode=xmlXPathNewNodeSet(doc->children);
			axml->lesindex[clefautomate]=listeNode;
            
            laClef.nettoie();            
            axml->ajout=OUI;

            return entree->CreerNoeudXML(this,axml,axml->document->children,vectNoeuds,vectFonc);                        
        }
    }
    return NON;
}

void GlobalParseur::NodeReference(xmlNodePtr node) {
    
	if (basexmlreference==NULL) {
		basexmlreference = new arborescenceXML(NULL,(const uchar*)"_currentxmlnode",2);
		basexmlreference->reference=OUI;        
		BaseDonneesXML.ajoute(basexmlreference);
		basexmlreference->creeXPathContext();
	}
	basexmlreference->XmlReferenceNode=node;
	basexmlreference->XmlCourant=node;
	basexmlreference->XmlNodeCourant=node;
}

//--------------------------------------------------------------------------------------

//Premiere methode, on charge le fichier en memoire
//On construit alors une liste en memoire de fichier charge...
void GlobalParseur::ChargerFichierXML(char* nomfichier,char* alias) {
    *flot_erreur<<"Indexing file:"<<nomfichier<<endl;             
    xmlDocPtr doc= xmlSAXParseFile(NULL, nomfichier, 1);
    
    LIGNES=0;
    NOM=nomfichier;

    if (doc == NULL)  {
        sprintf(Chaine_traits,"Unknown filename:%s\n",nomfichier);
        erreur(Chaine_traits);
    }
    
    xmlNodePtr pointentree=doc->children;
    if (pointentree!=NULL) {
        //Nous supposons une arborescence
        arborescenceXML* axml = new arborescenceXML(doc,(uchar*)alias,0,nomfichier);
        //On construit alors l'indexation du document sur la base
        //des champs associes.        
        VECTA<char*> chemin;        
        string  buff=alias;
		buff+=".";
        chemin.ajoute(Majuscule(STR(buff)));
        axml->creeXPathContext();        
        axml->trouveIndex(xpathindex);
        axml->indexer(xpathindex,TestOptions(AFFICHAGE_UTF8));
        BaseDonneesXML.ajoute(axml);
        //De plus nous connaissons grace a la grammaire la liste des instructions qui font
        //reference a ce document. Nous allons construire un automate qui reconnaitra ces
        //expressions tout en indexant le fichier
        //l'alias nous sert de clef pour retrouver ces expressions
    }
}

//-xmldb 0 toto  C:\XIP\patron\Reuters\epo.xml 
//--------------------------------------------------------------------------------------
dtdXML* GlobalParseur::evalueTypeNoeud(xmlNodePtr node) {
    if (CompareAvecDTD==SANSDESCRIPTION)
        return NULL;
    
    string valeur;
    int i;
    if (node->name==NULL || node->name[0]==0)
        return NULL;
    if (CompareAvecDTD==LIREAVECDTD) {
        valeur=(char*)node->name;
        minuscule(USTR(valeur));
        
        for (i=0;i<InformationDtd.dernier;i++) {
            if (valeur==(char*)InformationDtd[i]->nom)
                return InformationDtd[i];
        }
        return NULL;
    }
    
    //Sinon, il s'agit d'une structure lue dans un fichier de parametre
    if (CompareAvecDTD==LIREAVECPARAM) {
        static VECTA<Noeud*> vectNoeuds;
        static VECTA<ResultatFonction*> vectFonc;
        string laclef;
        for (int j=0;j<InformationDtd.dernier;j++) {
            if (compchmin((uchar*)node->name,(uchar*)InformationDtd[j]->nom))
                continue;
            clefXML* entree=(clefXML*)InformationDtd[j]->clef;

            //Dans le cas, ou il n'y a que le nom de la balise.
            if (entree->regles.dernier==3) {
                if (!compchmin((uchar*)node->name,(uchar*)entree->regles[1]->Chaine()))
                    return InformationDtd[j];
                continue;
            }
            
            //On change la racine de l'arbre, si la racine que l'on a detecte correspond

            char trouve=NON;                        
            xmlNodePtr courant=node;
            entree->ConstruitLaClef(this,laclef,vectNoeuds,vectFonc);
            if (laclef.size()!=0) {
                if (laclef[0]=='/' && laclef[1]=='/') {     
					valeur="self::";
					valeur+=STR(laclef)+2;
					laclef=valeur;
                    //On compte alors les '/', pour remonter dans les parents
                    i=valeur.size();
                    for (int ll=0;ll<i;ll++) {
                        if (valeur[ll]=='/')
                            courant=courant->parent;
                    }
                    basexmlcourante->xpathcourant->node=courant;
                }
                
                
                basexmlcourante->xpathcourant->doc=node->doc;
                
                xmlXPathObjectPtr listeNode = xmlXPathEval(XSTR(laclef),basexmlcourante->xpathcourant);
                if (listeNode!=NULL && listeNode->nodesetval!=NULL) {
                    for (int i=0;i<listeNode->nodesetval->nodeNr;i++) {
                        if (node==listeNode->nodesetval->nodeTab[i]) {
                            trouve=OUI;
                            break;
                        }
                    }                    
                    xmlXPathFreeObject(listeNode);
                }
                if (trouve==OUI)
                    return InformationDtd[j];
            }
        }                
    }
    
    
    return NULL;
}

//------------------------------------------------------------------------------
//Analyse d'un document en fonction de la DTD
//------------------------------------------------------------------------------
inline void GlobalParseur::accumulation(string& accumulateur,char* c) {
    if (c==NULL || !*c)
        return;
    accumulateur= accumulateur+c;
}






void RecupereTextXmlNode(string& accumulateur,xmlNodePtr node) {    
    xmlBufferPtr buffer = xmlBufferCreate();
    xmlChar *cont;
    

    int taille=accumulateur.size();
	//Dans ce cas on va chercher le premier noeud text juste sous le noeud courant...
	if (node->type==XML_ELEMENT_NODE)
		node=node->children;

    switch (node->type) {
    case XML_CDATA_SECTION_NODE:
    case XML_TEXT_NODE:
        if (node->content != NULL) { 
            //if (conversionutf8==OUI)
              //  conversionUTF8VersAscii(buffer,node->content);
            //else
            xmlBufferCat(buffer, node->content);            
        }
        break;
    case XML_ENTITY_REF_NODE:
        /* recursive substitution of entity references */
        cont = xmlNodeGetContent(node);
        
        if (cont) {
            xmlBufferCat(buffer,
                (const xmlChar *) cont);
            xmlFree(cont);
        }
        break;
    default:
        break;
    }        
	if (buffer->content!=NULL && *buffer->content!=NULL)    
		accumulateur+=(char*)buffer->content;
    xmlBufferFree(buffer);
}

//--------------------------------------------------------------------------------------
//On peut accumuler dans deux cas: TRAIT ou ACCUMULATION
inline char TestAccumulation(XMLEVENEMENT ev) {
    if ( (ev & XMLACCUMULE)==XMLACCUMULE ||
         (ev & XMLTRAIT)==XMLTRAIT )
        return OUI;
    return NON;
}

inline char TestAnalysePure(XMLEVENEMENT ev) {
    if ((ev & XMLANALYSE)==XMLANALYSE) {
        if ((ev & XMLACCUMULE)==XMLACCUMULE)
            return NON;
        return OUI;
    }
    return NON;
}

inline char TestAnalyse(XMLEVENEMENT ev) {
    if ((ev & XMLANALYSE)==XMLANALYSE)
        return OUI;
    return NON;
}

inline char TestDejaToken(XMLEVENEMENT ev) {
    if ((ev & XMLDEJATOKEN)==XMLDEJATOKEN)
        return OUI;
    return NON;
}

inline char TestAttribut(XMLEVENEMENT ev) {
    if ((ev & XMLGETATTRIBUTE)==XMLGETATTRIBUTE)
        return OUI;
    return NON;
}

inline char TestTraits(XMLEVENEMENT ev) {
    if ((ev & XMLTRAIT)==XMLTRAIT)
        return OUI;
    return NON;
}
//--------------------------------------------------------------------------------------
inline char TestAccumule(XMLEVENEMENT ev) {
    if ((ev & XMLACCUMULE)==XMLACCUMULE)
        return OUI;
    return NON;
}

inline char TestAnalysePure(VECTA<XMLEVENEMENT>& pile) {
    for (int i=pile.dernier-1;i>=0;i--) {
        if (!TestAnalysePure(pile[i]))
            return NON;
    }
    return OUI;
}

void GlobalParseur::RecupereContenuXmlNode(string& accumulateur,xmlNodePtr node,int profondeur) {    
    xmlBufferPtr buffer = xmlBufferCreate();
    xmlChar *cont;
    

    int taille=accumulateur.size();

    switch (node->type) {
    case XML_CDATA_SECTION_NODE:
    case XML_TEXT_NODE:
        if (node->content != NULL) { 
            //if (conversionutf8==OUI)
              //  conversionUTF8VersAscii(buffer,node->content);
            //else
            xmlBufferCat(buffer, node->content);            
        }
        break;
    case XML_ENTITY_REF_NODE:
        /* recursive substitution of entity references */
        cont = xmlNodeGetContent(node);
        
        if (cont) {
            xmlBufferCat(buffer,
                (const xmlChar *) cont);
            xmlFree(cont);
        }
        break;
    default:
        break;
    }        
    
	alphrase.ajoute(new aligne(node,taille,taille+buffer->use-1,profondeur));
    accumulation(accumulateur,(char*)buffer->content);
    xmlBufferFree(buffer);
}
//--------------------------------------------------------------------------------------
//Principal methode pour appliquer un XML guide sur un texte
void GlobalParseur::AnalyseRecursiveXML(xmlNodePtr pere,
                                        string& accumulateur,
                                        dtdXML* dtdparent,
                                        char renumerote,
                                        char arbre,
                                        XMLEVENEMENT courant,
                                        int profondeur) {
    //Selon le type du noeud nous allons soit analyser le texte, soit effectuer une accumulation
    
    
    xmlNodePtr node=pere;    
    xmlNodePtr  xmlres=NULL;
    XMLEVENEMENT ev=defautEvenement;        
    XMLEVENEMENT nouveau;
    string nouveauAccu="";
	char traitelafin=NON;
    dtdXML* dtd;
    ostream* os=&cout;
    if (OS!=NULL)
        os=OS;
    
	noeudxmltokencourrant=NULL;
    while (node !=NULL) {
        
        basexmlcourante->XmlNodeCourant=node;
        if (node->type==XML_ELEMENT_NODE && node->_private==NULL) {
            node=node->next;
            continue;
        }


        char analyser=NON;
        string* accu=&accumulateur;
        char* cha=NULL;
        ev=defautEvenement;  
        XMLACTION act=defautAction;
        dtd=NULL;
        char* chainepardefaut=NULL;

		//Cas particulier, nous sommes face a un noeud dont on doit recuperer
		//l'attribut pour tokenizationattribute
		dtd=evalueTypeNoeud(node);

		//Soit il s'agit d'un noeud de type TEXT, soit d'un noeud vide
		if (node->type!=XML_ELEMENT_NODE || (node->children==NULL && dtd==NULL) ) {            			
            //On recupere le texte associe            
            if (dtdparent!=NULL) {
                if (dtdparent->evenement!=XMLSAUTE) {
                    RecupereContenuXmlNode(accumulateur,node,profondeur);
                    if (TestAccumulation(dtdparent->evenement)) {
                        node=node->next;
                        continue;
                    }
                    if (TestAnalyse(dtdparent->evenement)) {
                        ev=dtdparent->evenement;
                        act=dtdparent->action;
                        chainepardefaut=dtdparent->vide;
                        analyser=OUI;
                        //On repositionne le noeud courant pour le reste de l'analyse
                        basexmlcourante->XmlNodeCourant=node->parent;
                    }
                }
            }
            else {
                if (defautEvenement!=XMLSAUTE) {
                    RecupereContenuXmlNode(accumulateur,node,profondeur);
                    if (TestAnalyse(defautEvenement)) {
                        analyser=OUI;
                        //On repositionne le noeud courant pour le reste de l'analyse
                        basexmlcourante->XmlNodeCourant=node->parent;
                    }
                }
            }
        }
        else {
			char Accum=NON;
            //On construit un chemin de noeud ascendant jusqu'a la racine
            if (TestOptions(AFFICHAGE_BALISE) && strcmp((char*)node->name,"text")) {
                *os<<"<"<<node->name;
                //On affiche aussi les attributs
                if (node->properties!=NULL) {
                    xmlAttrPtr propriete=node->properties;                
                    while (propriete!=NULL) {                                 
                        *os<<" "<<propriete->name<<"="<<"\""<<propriete->children->content<<"\"";
                        propriete=propriete->next;
                    }
                }
                *os<<">"<<Endl;
            }
            
            
            
            if (dtd!=NULL) {
                ev=dtd->evenement;
                act=dtd->action;
                chainepardefaut=dtd->vide;
				if (TestDejaToken(ev))
					traitelafin=OUI;
            }
            
            nouveau=courant;
            if (TestAccumulation(ev))
                nouveau=XMLACCUMULE;
            
            xptpos* xp;
			
            //Dans le cas d'une analyse, on prepare le noeud a recevoir le resultat
            //de l'execution
            
            if (TestAnalyse(ev) && (TestAccumule(ev) || !TestAccumulation(courant))) {
                
                xp=(xptpos*)node->_private;
                
                //Pour le moment l'offset est mal calcule, on prefere une base toujours a zero
                //if (xp!=NULL)
                //  baseOffset=xp->pos;
                //else
                baseOffset=0;
				baseOffsetCar=0;
				Accum=OUI;
                
                if (!TestAnalysePure(ev) || chainepardefaut!=NULL)
                    analyser=OUI;
                
                //On analyse alors les noeuds inferieurs
                if (XML==OUTPUTLIBXML && gAfficherResultats==OUI) {            
                    xmlres=CreationNoeudXML(node,"XIPRESULT",NULL);
                    if (mathematique==OUI)
                        AjouteAttributValeur(xmlres,"math","1");
                    else
                        AjouteAttributValeur(xmlres,"math","0");
                    
                    xp->xmlres=xmlres;
                    listeDesNodesCourants.ajoute(node);
                }
                
                XmlLunit=NULL;
                accu=&nouveauAccu;
            }
            
            
            
            //Dans le cas d'une structure de traits, il faut
            //introduire un mot clef special qui declenchera
            //l'affectation de traits
            if (TestTraits(ev)) {
				Accum=OUI;
                sprintf(Chaine_traits," zzbo%s ",dtd->marquage);
                //si le dernier caractere est un espace, alors il n'est pas necessaire d'en rajouter un
                accumulation(*accu,(char*)Chaine_traits);            
            }            
            
            //On analyse les noeuds suivants inferieurs
            //S'il s'agit d'un evenement traits, l'analyse recursive a deja ete faite
            if (node->children!=NULL && act!=XMLSKIPALL ) {  
				if (Accum==NON)
					alphrase.ajoute(new aligne(node,accumulateur.size(),accumulateur.size(),profondeur));
                //cas particulier, il s'agit d'une accumulation et l'element n'est pas
                //pris en compte par la grammaire. On accumule simplement son contenu
				DeduitFonction(-1,DEDUCTIONPREBALISE,NON);
                if (dtd==NULL && dtdparent!=NULL && TestAccumulation(courant))
                    AnalyseRecursiveXML(node->children,*accu,dtdparent,renumerote,arbre,nouveau,profondeur+1);
                else
                    AnalyseRecursiveXML(node->children,*accu,dtd,renumerote,arbre,nouveau,profondeur+1);
                basexmlcourante->XmlNodeCourant=node;
				DeduitFonction(-1,DEDUCTIONPOSTBALISE,NON);
            }
			else {
				//Dans le cas de tokenizeattribute, nous ne devons pas analyser de champ text
				if (TestAttribut(ev)) {
					//dans ce cas, le champ correspond a notre attribut
					*accu+=ValeurAttribut(node,dtd->attribut);
					analyser=OUI;
				}
			}
            
            if (TestTraits(ev)) {
                sprintf(Chaine_traits," zzbf%s ",dtd->marquage);
                //si le dernier caractere est un espace, alors il n'est pas necessaire d'en rajouter un
                accumulation(*accu,(char*)Chaine_traits);
                //On decale alors les indices
            }
        }
        //Puis on se lance dans l'analyse proprement dite
        //Plusieurs cas lorsqu'il s'agit d'une analyse
        //a) Le texte a ete accumule
        //b) Nous ne sommes pas dans une phrase d'accumulation

		//Nous gardons le noeud suivant en reserve au cas ou des regles modifieraient la forme du
		//fichier courant...
		xmlNodePtr nsuivant=node->next;
        if (analyser==OUI) {
            cha=(char*)accu->c_str();  
            //le deuxieme parametre d'une regle de DTD correspond a la chaine a
            //utiliser si notre chaine est vide (ne contient que des caracteres <=32)
            //Nous avons deux cas possibles
            //a) Mode merge
            //b) Mode analyze mais alors dtd=NULL (nous devons etre en recursion)
            if (chainepardefaut!=NULL && TestChaineVide((uchar*)cha)) {
                accumulation(*accu,chainepardefaut);
                cha=(char*)accu->c_str();
            }
                        

            int cald=CalculFonctionDefini;
            int desd=DESAMBIGUISATIONDefini;
            int appd=appliqueGrammaireDefini;
            switch(act) {
            case XMLCHUNK:
                CalculFonction=NON;
                CalculFonctionDefini=NON;
                break;
            case XMLTOKEN:
                DESAMBIGUISATION=OUI;
                CalculFonction=NON;                
                DESAMBIGUISATIONDefini=OUI;
                CalculFonctionDefini=NON;                
                break;
            case XMLDEPENDENCY:
                CalculFonction=OUI;
                CalculFonctionDefini=OUI;
            }
			if (node !=NULL && node->type==XML_TEXT_NODE)
				basexmlcourante->XmlNodeCourant=node->parent;			
			else
				basexmlcourante->XmlNodeCourant=node;			
			//On execute la partie INITIALE, avant toute chose
            
            XmlResult=xmlres;
            //Dans le cas d'une lecture token par token
            if (TestDejaToken(ev)) {
				if (alphrase.dernier>0) {
					presencetoken=OUI;
					Trim((char*)cha);
					renumerotation=renumerote;
					int posdernier=phrase.dernier;
					long bs=alphrase.fin()->droit;
					//Nous sommes dans un mode de tokenization...
					alphrase.fin()->gauche+=baseOffsetToken;
					alphrase.fin()->droit+=baseOffsetToken;
					baseOffset=baseOffsetToken;
					alphrase.fin()->num=num_token++;
					if (TestAttribut(ev))
						noeudxmltokencourrant=node;
					else
						noeudxmltokencourrant=node->parent;
					AddAToken((unsigned char*)cha);

					baseOffsetToken+=bs;

					//On a elimine des noeuds, apres application d'une phrase
					//On reduit alors alphrase d'autant
					if (fenetre!=-1) {
						int nm=phrase.vecteur[fenetre]->num_mot;
						for (posdernier=0;posdernier<alphrase.dernier && alphrase[posdernier]->num<nm;posdernier++)
							delete alphrase.retireElement(0);
					}
				}
                //On vide evidemment cette chaine, pas d'accumulation
                *accu="";
            }
            else {//sinon, il s'agit de la chaine a analyser
				presencetoken=NON;
				gBaliseSansFin->raz();				
				if (interceptexml!=NULL) {
					string intercha(cha);
					vector<KifElement*> params;	
					params.push_back(kifloader->Providestring(intercha));
					params[0]->Setreference();
					KifElement* resultat=kifloader->Execute(interceptexml,params);
					params[0]->Resetreference();
					intercha=resultat->String();
					resultat->Release();
					StringAssociation(USTR(intercha));                        
					SentenceAnalysis(renumerote,arbre);
				}
				else {
					StringAssociation((uchar*)cha);                        
					SentenceAnalysis(renumerote,arbre);
				}
				alphrase.nettoie();
			}

			if (AppelExterneXml!=NULL)
				(*AppelExterneXml)(iParseurCourant,xipresult,donneesExternesXml);
        
            //Puis on reinitialise avec les valeurs initiales
            CalculFonctionDefini=cald;
            DESAMBIGUISATIONDefini=desd;
            appliqueGrammaireDefini=appd;
            CalculFonction=cald;
            DESAMBIGUISATION=desd;
            appliqueGrammaire=appd;
            *accu="";   
        }

        if (TestOptions(AFFICHAGE_BALISE)  && strcmp((char*)node->name,"text"))
            *os<<"</"<<node->name<<">"<<Endl;
		
        //Puis les noeuds freres
        node=nsuivant;		
    }

	//Dans le cas d'un traitement par token, des que l'on remonte dans l'arborescence
	//il nous faut terminer le travail en cours, pour que des phrases ne court pas d'un
	//noeud XML a un autre...
	if (traitelafin==OUI) {
		//On consomme tous les mots en memoire
		ParseFinal();
		//on nettoie la representation
		alphrase.nettoie();		
	}
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int DetermineEvenement(GlobalParseur* parseur,INSTRUCTION ev) {
    
    switch (ev) {
    case XMLFSKIP:
        return XMLSAUTE;
    case XMLFANALYZE:
        return XMLANALYSE;
    case XMLFFEATURE:
        return XMLTRAIT;
    case XMLTOKENIZE:
        return XMLDEJATOKEN|XMLANALYSE;
    case XMLTOKENIZEATTRIBUTE:
        return XMLDEJATOKEN|XMLANALYSE|XMLGETATTRIBUTE;
    case XMLFMERGE:
        return XMLACCUMULE|XMLANALYSE;
    }
    parseur->erreur(parseur->CHAINES[267]);
    return XMLSAUTE;
}

XMLACTION GlobalParseur::DetermineAction(XMLEVENEMENT ev,char* action) {
    
    if (ev==XMLSAUTE) {
        if (!strcmp(action,"text"))
            return XMLSKIPTEXT;
        
        if (!strcmp(action,"all"))
            return XMLSKIPALL;
    }
    else {
        if (!strcmp(action,"token"))
            return XMLTOKEN;
        if (!strcmp(action,"chunk"))
            return XMLCHUNK;
        if (!strcmp(action,"dependency"))
            return XMLDEPENDENCY;
    }
    string err=CHAINES[265];
	err+=action;
    erreur(STR(err));
    return XMLDEPENDENCY;
}


//------------------------------------------------------------------------------
//lecture du fichier de description
char GlobalParseur::LitXMLGuide(istream& fichier) {
    static char tampon[1000];
	string tmp;
    
    char k=0;
    VECTA<char> vari;
    static VECTA<Noeud*> vectNoeuds;
    static VECTA<ResultatFonction*> vectFonc;    

    CompareAvecDTD=LIREAVECPARAM;
    eleXML* e=NULL;
    clefXML* lecture_xml_guide;
    dtdXML* dtd;
    metXML* met;
    tampon[0]=0;
    lecture_xml_guide=ParseXPathExpression(fichier,k,tampon,0,vari,OUI);
    while (lecture_xml_guide!=NULL) {        
        e=lecture_xml_guide->regles[1];        
        //dans le cas d'une analyse par defaut
        if (!strcmp((char*)e->Chaine(),"#default")) {
            //On jette un coup d'oeil sur sa fonction
            if (lecture_xml_guide->regles[2]!=NULL && lecture_xml_guide->regles[2]->Type() == XMLMETHODE) {
                met=(metXML*)lecture_xml_guide->regles[2];
                defautEvenement=(XMLEVENEMENT)DetermineEvenement(this,met->methode);
                if (met->regles[0]==NULL) 
                    erreur(CHAINES[269]);
                defautAction=DetermineAction(defautEvenement,(char*)met->regles[0]->Chaine());
            }
            else
                erreur(CHAINES[266]);
            delete lecture_xml_guide;
        }
        else {
            //on recherche l'evenement et l'action            
            met=(metXML*)lecture_xml_guide->regles.fin();
            XMLEVENEMENT ev=(XMLEVENEMENT)DetermineEvenement(this,met->methode);
            dtd=new dtdXML(lecture_xml_guide,ev);
            *flot_erreur<<"Name="<<dtd->nom<<endl;
            InformationDtd.ajoute(dtd);
            if (ev==XMLTRAIT) {
                if (met->regles.dernier<2)
                    erreur(CHAINES[268]);
                Traits* premier=new Traits(this);
                Traits* courant=new Traits(this);
                XipFeatureTraits(premier,(char*)met->regles[0]->Chaine(),"+");
                XipFeatureTraits(courant,(char*)met->regles[1]->Chaine(),"+");
                //Il faut d'abord construire la chaine de marquage                
                lecture_xml_guide->ConstruitLaClef(this,tmp,vectNoeuds,vectFonc);
				strcpy(tampon,STR(tmp));
                //On retire tous les caracteres <"A" && >"z"
				int ln=strlen(tampon);
                for (int i=0;i<ln;i++) {                    
                    if ( (tampon[i]>='A' && tampon[i]<='Z') ||
                        (tampon[i]>='a' && tampon[i]<='z') ||
                        (tampon[i]>='1' && tampon[i]<='9'))
                        continue;
                    tampon[i]='x';
                }

                dtd->marquage=DupliqueChaine(tampon);
                minuscule((uchar*)dtd->marquage);

                //On transforme alors le nom de la balise en une balise entrante
                sprintf(tampon,"zzbo%s",dtd->marquage);                    
                balisesinsertion.parsebrute((uchar*)tampon,NULL);
                

                auto_vocabulaire* auv=new auto_vocabulaire(this,tampon,NULL,NULL,NULL,courant,premier);
                auv->ecraser=BALISEDEBUT;
                                
                etat* a=LesBalises.parse((uchar*)tampon,auv);
                if (a!=NULL) {
                    auto_vocabulaire* aauv=(auto_vocabulaire*)a->valeur;
                    if (aauv!=auv)
                        aauv->ajoute(auv);
                }
                
                //Puis la balise fermante
                sprintf(tampon,"zzbf%s",dtd->marquage);
                balisesinsertion.parsebrute((uchar*)tampon,NULL);                

                courant=new Traits(this);                
                XipFeatureTraits(courant,(char*)met->regles[1]->Chaine(),"+");
                
                auv=new auto_vocabulaire(this,tampon,NULL,NULL,NULL,courant,new Traits(courant));
                auv->ecraser=BALISEFIN;
                a=LesBalises.parse((uchar*)tampon,auv);
                if (a!=NULL) {
                    auto_vocabulaire* aauv=(auto_vocabulaire*)a->valeur;
                    if (aauv!=auv)
                        aauv->ajoute(auv);
                }
                
                if (!strchr(PremierCaractereBalise,'z')) {
                    char ppi=strlen(PremierCaractereBalise);
                    PremierCaractereBalise[ppi]='z';
                    PremierCaractereBalise[ppi+1]=0;
                }                                  
                if (met->regles.dernier==3) {                    
                    dtd->action=DetermineAction(dtd->evenement,(char*)met->regles[2]->Chaine());                    
                    dtd->evenement=(XMLEVENEMENT)(dtd->evenement|XMLANALYSE);
                }
            }
            else {
                if (met->regles[0]==NULL) 
                    erreur(CHAINES[269]);
                dtd->action=DetermineAction(dtd->evenement,(char*)met->regles[0]->Chaine());
				//Dans ce cas, la liste des attributs est plus longue:
				//Premier champ obligatoire: le nom de l'attribut qui fournit
				//le token
				if (met->methode==XMLTOKENIZEATTRIBUTE) {
					if (met->regles[1]==NULL)
						erreur("Missing argument in tokenizeattribute(action,attribute_name,empty");
					dtd->attribut=(char*)met->regles[1]->Chaine();
					if (met->regles[2]!=NULL)
						dtd->vide=(char*)met->regles[2]->Chaine();
				}
				else {
					if (met->regles[1]!=NULL)
						dtd->vide=(char*)met->regles[1]->Chaine();
				}
            }
        }
        lecture_xml_guide=ParseXPathExpression(fichier,k,tampon,0,vari,OUI);
    }
    return NON;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
                                        

//Il s'agit d'une analyse de la DTD pour reperer les informations pertinentes:
//les actions associees a une balise
//les traits associees a une balise
void GlobalParseur::ExtraitMoelleBalise(xmlElementPtr elem) {
    
    //Il s'agit donc de xipaction dont la valeur indique l'action
    //sauter, accumuler ou analyser
    CompareAvecDTD=LIREAVECDTD;
    xmlAttributePtr prop;
    char action=0;
    dtdXML* dtd;
    XMLEVENEMENT ev;
    string defaut;
    string valeur;
    while (elem!=NULL) {
        if (elem->type!=XML_ELEMENT_DECL) {
            elem=(xmlElementPtr)elem->next;
            continue;
        }
        prop=elem->attributes;
        dtd=NULL;
        //Il peut y avoir traits et type d'analyse en mŠme temps        
        while (prop!=NULL) {
            if (prop->type!=XML_ATTRIBUTE_DECL) {
                prop=(xmlAttributePtr)prop->next;
                continue;
            }
            
            if (strcmp((char*)elem->name,(char*)prop->elem)){
                prop=(xmlAttributePtr)prop->next;
                continue;
            }

            if (!strcmp((char*)prop->name,"xipaction")) {
                action=1;
                ev=XMLANALYSE;                    
                break;
            }            
            if (!strcmp((char*)prop->name,"xipfeature") ) {
                action=2;
                ev=XMLTRAIT;
                break;
            }
            prop=(xmlAttributePtr)prop->next;
        }
        if (prop==NULL || strcmp((char*)elem->name,(char*)prop->elem)) {
            elem=(xmlElementPtr)elem->next;
            continue;
        }
        //Liste des attributs;
        xmlEnumerationPtr arbre=prop->tree;
        //xipaction
        defaut=(char*)prop->defaultValue;
        Majuscule(STR(defaut));

        XMLACTION act=XMLDEPENDENCY;
        Traits* premier;
        Traits* courant;

        
        switch(action) {
        case 1:                        
            if (defaut=="ANALYZE")
                ev=XMLANALYSE;
            else {
                if (defaut=="SKIP")
                    ev=XMLSAUTE;            
                else {
                    if (defaut=="MERGE")
                        ev=(XMLEVENEMENT)((int)XMLACCUMULE|(int)XMLANALYSE);
                }
            }
            
            if (arbre!=NULL) {
                valeur=(char*)arbre->name;
                minuscule(USTR(valeur));
                if (valeur=="token")
                    act=XMLTOKEN;
                else
                    if (valeur=="chunk")
                        act=XMLCHUNK;
            }
            
            
            dtd=new dtdXML((char*)elem->name,ev);
            InformationDtd.ajoute(dtd);
            dtd->action=act;
            break;
        case 2:            
            if (defaut=="TOKEN") {
                act=XMLTOKEN;            
                ev=XMLANALYSE;
            }
            else {
                if (defaut=="CHUNK") {
                    act=XMLCHUNK;
                    ev=XMLANALYSE;
                }
                else {
                    if (defaut=="DEPENDENCY") {
                        act=XMLDEPENDENCY;
                        ev=XMLANALYSE;
                    }
                }
            }
            
            ev=(XMLEVENEMENT)((int)ev|(int)XMLTRAIT);            
            dtd=new dtdXML((char*)elem->name,ev);
            InformationDtd.ajoute(dtd);
            dtd->action=act;
            
            //On calcule les traits
            //le defaut correspond au nom d'attribut et chaque valeur a un element dans le "tree"
            
            if (arbre==NULL || arbre->next==NULL) {
                defaut=CHAINES[252];
				defaut+=":";
				defaut+=(char*)elem->name;
                erreur(STR(defaut));
                
            }                
            
            premier=new Traits(this);
            courant=new Traits(this);
            XipFeatureTraits(premier,(char*)arbre->name,"+");
            XipFeatureTraits(courant,(char*)arbre->next->name,"+");
            
            //On transforme alors le nom de la balise en une balise entrante
            dtd->marquage=DupliqueChaine(dtd->nom);
            minuscule((uchar*)dtd->marquage);

			valeur="zzbo";
			valeur+=dtd->marquage;
            balisesinsertion.parsebrute(USTR(valeur),NULL);            
            

            auto_vocabulaire* auv=new auto_vocabulaire(this,STR(valeur),NULL,NULL,NULL,courant,premier);           
            auv->ecraser=BALISEDEBUT;

            etat* a=LesBalises.parse(USTR(valeur),auv);
            if (a!=NULL) {
                auto_vocabulaire* aauv=(auto_vocabulaire*)a->valeur;
                if (aauv!=auv)
                    aauv->ajoute(auv);
            }
            
            
            //Puis la balise fermante
			valeur="zzbf";
			valeur+=dtd->marquage;
            balisesinsertion.parsebrute(USTR(valeur),NULL);

            courant=new Traits(this);
            //XipFeatureTraits(courant,defaut,(char*)arbre->next->name);
            XipFeatureTraits(courant,(char*)arbre->next->name,"+");
            
            auv=new auto_vocabulaire(this,STR(valeur),NULL,NULL,NULL,courant,new Traits(courant));
            auv->ecraser=BALISEFIN;
            a=LesBalises.parse(USTR(valeur),auv);
            if (a!=NULL) {
                auto_vocabulaire* aauv=(auto_vocabulaire*)a->valeur;
                if (aauv!=auv)
                    aauv->ajoute(auv);
            }
            
            if (!strchr(PremierCaractereBalise,'z')) {
                char ppi=strlen(PremierCaractereBalise);
                PremierCaractereBalise[ppi]='z';
                PremierCaractereBalise[ppi+1]=0;
            }                                  
        }
        elem=(xmlElementPtr)elem->next;
    }
}

//--------------------------------------------------------------------------------------
void GlobalParseur::AnalyseXDTD(char* xdtd) {
    LIGNES=0;
    NOM=xdtd;

    //Si la dtd existe, on la detruit et on la remplace
    if (xipdtd!=NULL)
        xmlFreeDtd(xipdtd);
    xipdtd=NULL;

    if (xdtd==NULL || xdtd[0]==0)
        return;

    xmlChar* normalized=xmlNormalizeWindowsPath((const xmlChar *)xdtd);
    xipdtd=xmlParseDTD(NULL,normalized);
    if (xipdtd==NULL) 
        erreur(CHAINES[250]);
    if (xipdtd->children==NULL)
        erreur(CHAINES[250]);
    ExtraitMoelleBalise((xmlElementPtr)xipdtd->children->next);
    xmlFree(normalized);

    for (int i=0;i<InformationDtd.dernier;i++) {
        //pour toutes les balises non trait
        if (InformationDtd[i]->evenement!=XMLTRAIT)
            InformationDtd[i]->tr=CreationTrait("xmltag",(char*)InformationDtd[i]->nom); 
    }
}

//--------------------------------------------------------------------------------------
//Les methodes ci-dessous sont utilises pour analyser un document par morceau
extern "C" {
void debutElementXML(void *ctx, const xmlChar *fullname, const xmlChar **atts) {

    char premier=NON;
    xmlParserCtxtPtr x = (xmlParserCtxtPtr)ctx;    
    //_private contient un pointeur vers GlobalParseur    
    //Le premier noeud n'a pas encore ete cree
    if (x->node==NULL)
        premier=OUI;

    GlobalParseur* parseur=(GlobalParseur*)(x->_private);
    startElement(ctx, fullname, atts);    
    xmlNodePtr n=x->node;
    long pos=x->input->consumed+(x->input->cur-x->input->base);
    
    xptpos* xp=new xptpos(pos,n);    
    if (!premier)
        parseur->liste_offsets.ajoute(xp);    

    if (parseur->profondeur_analyse==0)
        return;
    
    //c'est le sommet de l'arbre, on cree un contexte XPATH
//    if (arboxml->xpathcourant==NULL)
//        arboxml->xpathcourant=xmlXPathNewContext(x->myDoc);
    
    //Nous calculons la profondeur d'analyse
    int i=0;
    //C'est le noeud courant    
    while (n!=NULL) {
        i++;
        n=n->parent;
    }
    
    //si l'on detecte que l'on a atteint la bonne profondeur de balise
    //On garde la trace de cette balise
    if (i==parseur->profondeur_analyse+1)
        parseur->balise_analyse=(uchar*)x->node->name;
    else {
        if (parseur->TestOptions(AFFICHAGE_FICHIER_XML_ECRAN)) {
            if (i<=parseur->profondeur_analyse && (parseur->OS!=NULL || parseur->texte_xml_courant==NULL)) {
                ostream* os=&cout;
                xmlChar* buf=NULL;
                n=x->node;
                int longueur=0;
                xmlNodePtr nfils=n->children;                        
                xmlDocPtr doc=n->doc;
                xmlNodePtr ncourant=doc->children;
                n->children=NULL;
                doc->children=n;
                xmlDocDumpMemoryEnc(doc, &buf,&longueur, (const char*)doc->encoding);
                char* pt=strstr((char*)buf,"/>");
                *pt='>';pt[1]='\n';pt[2]=0;
                n->children=nfils;
                doc->children=ncourant;
                
                if (parseur->OS!=NULL)
                    os=parseur->OS;
                
                //ce n'est pas la tete du document, on retire le <?...?> en tete
                if (i>2) {
                    pt=strstr((char*)buf,"<?");
                    pt=strstr(pt,"?>");
                    pt=strchr(pt,'<');
                    strcpy((char*)buf,pt);
                }
                
                parseur->amorce_affichage+=(char*)buf;
                xmlFree(buf);
            }
        }
    }
}

//--------------------------------------------------------------------------------------


void debutElementNs(void *ctx,
                    const xmlChar *localname,
                    const xmlChar *prefix,
                    const xmlChar *URI,
                    int nb_namespaces,
                    const xmlChar **namespaces,
                    int nb_attributes,
                    int nb_defaulted,
                    const xmlChar **attributes) {

    char premier=NON;
    xmlParserCtxtPtr x = (xmlParserCtxtPtr)ctx;    
    //_private contient un pointeur vers GlobalParseur    
    //Le premier noeud n'a pas encore ete cree
    if (x->node==NULL)
        premier=OUI;

    xmlSAX2StartElementNs(ctx,localname,prefix,URI,nb_namespaces,namespaces,
        nb_attributes,nb_defaulted,attributes);

    GlobalParseur* parseur=(GlobalParseur*)(x->_private);       
    xmlNodePtr n=x->node;
    long pos=x->input->consumed+(x->input->cur-x->input->base);
    
    xptpos* xp=new xptpos(pos,n);    
    if (!premier)
        parseur->liste_offsets.ajoute(xp);    

    if (parseur->profondeur_analyse==0)
        return;
    
    //c'est le sommet de l'arbre, on cree un contexte XPATH
    //if (arboxml->xpathcourant==NULL)
    //    arboxml->xpathcourant=xmlXPathNewContext(x->myDoc);
    
    //Nous calculons la profondeur d'analyse
    int i=0;
    //C'est le noeud courant    
    while (n!=NULL) {
        i++;
        n=n->parent;
    }
    
    //si l'on detecte que l'on a atteint la bonne profondeur de balise
    //On garde la trace de cette balise
    if (i==parseur->profondeur_analyse+1)
        parseur->balise_analyse=(uchar*)x->node->name;
    else {
        if (parseur->TestOptions(AFFICHAGE_FICHIER_XML_ECRAN)) {
            if (i<=parseur->profondeur_analyse && (parseur->OS!=NULL || parseur->texte_xml_courant==NULL)) {
                ostream* os=&cout;
                xmlChar* buf=NULL;
                n=x->node;
                int longueur=0;
                xmlNodePtr nfils=n->children;                        
                xmlDocPtr doc=n->doc;
                xmlNodePtr ncourant=doc->children;
                n->children=NULL;
                doc->children=n;
                xmlDocDumpMemoryEnc(doc, &buf,&longueur, (const char*)doc->encoding);
                char* pt=strstr((char*)buf,"/>");
                *pt='>';pt[1]='\n';pt[2]=0;
                n->children=nfils;
                doc->children=ncourant;
                
                if (parseur->OS!=NULL)
                    os=parseur->OS;
                
                //ce n'est pas la tete du document, on retire le <?...?> en tete
                if (i>2) {
                    pt=strstr((char*)buf,"<?");
                    pt=strstr(pt,"?>");
                    pt=strchr(pt,'<');
                    strcpy((char*)buf,pt);
                }
                
                parseur->amorce_affichage+=(char*)buf;
                xmlFree(buf);
            }
        }
    }
}


void finElementNs(void *ctx,
                  const xmlChar * name,
                  const xmlChar * prefix,
                  const xmlChar * URI) {
    
    
    xmlParserCtxtPtr x = (xmlParserCtxtPtr)ctx;
    xmlSAX2EndElementNs(ctx,name,prefix,URI);

    xmlNodePtr ncourant=x->node;
    
    GlobalParseur* parseur=(GlobalParseur*)(x->_private);
    if (parseur->profondeur_analyse==0 || parseur->balise_analyse==NULL)
        return;

    //Nous calculons la profondeur d'analyse
    int i=0;
    xmlNodePtr n=ncourant;
    //C'est le noeud courant    
    while (n!=NULL) {
        i++;
        n=n->parent;
    }
    
    //Nous sommes sur la balise finale, nous analysons le bout de document
    //et nous le nettoyons
    if (i==parseur->profondeur_analyse &&
        !strcmp((char*)name,(char*)parseur->balise_analyse)) {
        parseur->ParseFichierXML(x->myDoc);
        
        parseur->liste_offsets.nettoie();

        //Nettoyage du noeud courant, pour faire de la place pour les autres
        //lectures...
        if (x->node!=NULL) {
            xmlNodePtr fils=x->node->children;
            
            
            xmlNodePtr suivant=fils->next;
            while (fils!=NULL) {
                suivant=fils->next;
                xmlUnlinkNode(fils);
                xmlFreeNode(fils);
                //xmlFreeNodeList(fils);
                fils=suivant;
            }
            
            xmlClearNodeInfoSeq(&x->node_seq);
            parseur->balise_analyse=(uchar*)"";
        }
    }    
    
    if (parseur->TestOptions(AFFICHAGE_FICHIER_XML_ECRAN)) {
        if (ncourant!=NULL && i<=parseur->profondeur_analyse && (parseur->OS!=NULL || parseur->texte_xml_courant==NULL)) {
            
            ostream* os=&cout;
            
            if (parseur->OS!=NULL)
                os=parseur->OS;
            *os<<"</"<<ncourant->name<<">"<<endl;
        }
    }
    
}



}
//--------------------------------------------------------------------------------------
void GlobalParseur::AnalyseChaineXML(char* texte,int profondeur,char renumerote,char tree) {

    LIGNES=0;
    NOM="stringxml";

    xmlDocPtr doc;
    basexmlcourante->xpathcourant=NULL;        

    num_analyse_xml=0;
    balise_analyse=NULL;
    profondeur_analyse=profondeur;
    texte_xml_courant=NULL;
    baseOffset=0;
	baseOffsetCar=0;


    xmlSAXHandler* sax = (xmlSAXHandler *) xmlMalloc(sizeof(xmlSAXHandler));
    sax->initialized=0;
    //initxmlDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);       
    xmlSAX2InitDefaultSAXHandler(sax, xmlGetWarningsDefaultValue); 
    sax->endElementNs = finElementNs;
    sax->startElementNs = debutElementNs;        
      
    //On conserve un pointeur sur l'objet GlobalParseur
    //Puis on analyse le document    
    doc = xmlSAXParseMemoryWithData(sax,texte,strlen(texte),1,this);
	basexmlcourante->document=doc;
     
    if (doc == NULL)  {
        sprintf(Chaine_traits,"Unknown filename:%s\n",texte);
        erreur(Chaine_traits);
    }

    basexmlcourante->xpathcourant=xmlXPathNewContext(doc);
    //si profondeur==0, on charge le document entierement en memoire
    if (profondeur==0)
        ParseFichierXML(doc);
    
    if (doc->children!=NULL) {
        xptpos* xp=(xptpos*)doc->children->_private;
        if (xp!=NULL)
            delete xp;
    }

    liste_offsets.nettoie();


    AfficheVariablesGlobales();
    DeductionFinale=NON;
    xmlFreeDoc(doc);
    xmlFree(sax);
    if (basexmlcourante->xpathcourant!=NULL) 
        xmlXPathFreeContext(basexmlcourante->xpathcourant);
	basexmlcourante->document=NULL;
    basexmlcourante->xpathcourant=NULL;
    basexmlcourante->XmlNodeCourant=NULL;
    basexmlcourante->XmlReferenceNode=NULL;
    
}

//--------------------------------------------------------------------------------------
//Traitement d'un fichier de type XML
void GlobalParseur::AnalyseFichierXML(char* texte, int profondeur,char renumerote,char arbre) {

    LIGNES=0;
    NOM=texte;
    
	TVecteur parambase=ParametresAffichage;
	ChoixUTF8(AFFICHAGE_UTF8);

    xmlDocPtr doc;
    basexmlcourante->xpathcourant=NULL;        
    num_analyse_xml=0;
    balise_analyse=NULL;
    profondeur_analyse=profondeur;
    renseigne(texte);

    if (TestOptions(AFFICHAGE_FICHIER_XML_ECRAN))
        texte_xml_courant=NULL;
    else
        texte_xml_courant=texte;

    baseOffset=0;
	baseOffsetCar=0;


    xmlSAXHandler* sax = (xmlSAXHandler *) xmlMalloc(sizeof(xmlSAXHandler));
    sax->initialized=0;
    //initxmlDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);       
    xmlSAX2InitDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);       

    sax->endElementNs = finElementNs;
    sax->startElementNs = debutElementNs;        
    //On conserve un pointeur sur l'objet GlobalParseur
    //Puis on analyse le document
    doc = xmlSAXParseFileWithData(sax,texte,1,this);
	basexmlcourante->document=doc;

    //htmlDocPtr dochtml=htmlSAXParseFile(texte, NULL, sax,NULL);
    
    if (doc == NULL)  {
        sprintf(Chaine_traits,"Unknown filename:%s\n",texte);
        erreur(Chaine_traits);
    }

    basexmlcourante->xpathcourant=xmlXPathNewContext(doc);

    //si profondeur==0, on charge le document entierement en memoire
    if (profondeur==0)
        ParseFichierXML(doc);

    if (doc->children!=NULL) {
        xptpos* xp=(xptpos*)doc->children->_private;
        if (xp!=NULL)
            delete xp;
    }

    liste_offsets.nettoie();

    xmlNodePtr ancienunit=XmlLunit;
    XmlLunit=doc->children;

    AfficheVariablesGlobales();
    XmlLunit=ancienunit;
    DeductionFinale=NON;
    xmlFreeDoc(doc);
    if (basexmlcourante->xpathcourant!=NULL)
        xmlXPathFreeContext(basexmlcourante->xpathcourant);
    basexmlcourante->xpathcourant=NULL;
    basexmlcourante->XmlNodeCourant=NULL;
    basexmlcourante->XmlReferenceNode=NULL;
	basexmlcourante->document=NULL;

	ParametresAffichage=parambase;
	ParametresAffichageDefini=ParametresAffichage;

}

//--------------------------------------------------------------------------------------
void GlobalParseur::ParseFichierXML(xmlDocPtr doc) {
    ostream* os=&cout;

	TVecteur parambase=ParametresAffichage;
	ChoixUTF8(AFFICHAGE_UTF8);

	baseOffsetToken=0;
    if (OS!=NULL)
        os=OS;

    string accumulateur="";
    if (XML==OUI) {
        if (mathematique==OUI)
            *os<<"<XIPRESULT math=\"1\">"<<Endl;
        else
            *os<<"<XIPRESULT math=\"0\">"<<Endl;
    }


    AnalyseRecursiveXML(doc->children,accumulateur,NULL,NON,OUI,defautEvenement,1);
	//Il est possible qu'il reste encore des choses a analyser...
	CloseAnalysis();
    //Dans le cas d'un OUTPUTLIBXML
    //Il nous faut reconstruire l'arbre
    if (XML==OUTPUTLIBXML) {
        
        for (int i=0;i<listeDesNodesCourants.dernier;i++) {
            xptpos* xp=(xptpos*)listeDesNodesCourants[i]->_private;
            xmlNodePtr lresult=(xmlNodePtr)xp->xmlres;
            if (lresult!=NULL) {
                xmlAddChild(listeDesNodesCourants[i],lresult);
                xp->xmlres=NULL;
            }            
        }
        listeDesNodesCourants.raz();
        
        if (mathematique!=NON && gAfficherResultats==OUI) {
            xmlNodePtr ancienunit=basexmlcourante->XmlNodeCourant;    
            //Il faut eviter que n'echoue l'integration du noeud XIPVARIABLE
            //au noeud courant qui est aussi le noeud le plus de la hierarchie
            basexmlcourante->XmlNodeCourant=NULL;
            basexmlcourante->XmlReferenceNode=NULL;
            xmlNodePtr unevariable=CreationNoeudXML(doc->children,"XIPVARIABLES",NULL);
            basexmlcourante->XmlNodeCourant=ancienunit;
            basexmlcourante->XmlReferenceNode=NULL;
            XmlLunit=unevariable;
            AfficheVariablesGlobales();            
            XmlLunit=NULL;
        }

        if (OS==NULL && texte_xml_courant!=NULL) {
            //Puis on recupere le nom du fichier et on rajoute ".out"
            char ch[10];
            sprintf(ch,"%d",num_analyse_xml);
            string nomfichier;
            if (repertoireresultat=="")
                nomfichier=texte_xml_courant;
            else {                
                //sinon, on recupere juste le nom du fichier
                char* last_slash= strrchr(texte_xml_courant,SEP);
                nomfichier=repertoireresultat;
                if (repertoireresultat[repertoireresultat.size()-1]!=SEP)
                    nomfichier+=SEP;
                if (last_slash!=NULL)
                    nomfichier+=last_slash+1;
                else
                    nomfichier+=texte_xml_courant;
            }

            nomfichier+=".";            
            nomfichier+=ch;
            nomfichier+=".xml";
            xmlSaveFile((char*)nomfichier.c_str(),doc);
        }
        else {
            //Sinon, on sauvegarde tout dans un buffer..
            xmlChar* buf=NULL;
            int longueur=0;
            xmlNodePtr ncourant=doc->children;
            xmlNodePtr nfils=ncourant;
            
            if (TestOptions(AFFICHAGE_FICHIER_XML_ECRAN)) {
                for (int k=1;k<profondeur_analyse;k++)
                    nfils=nfils->children;
                doc->children=nfils;
                *os<<amorce_affichage;
                amorce_affichage="";
            }
            
            xmlDocDumpMemoryEnc(doc, &buf,&longueur, (const char*)doc->encoding);

            if (TestOptions(AFFICHAGE_FICHIER_XML_ECRAN) && profondeur_analyse>1) {
                doc->children=ncourant;
                char* pt=strstr((char*)buf,"<?");
                pt=strstr(pt,"?>");
                pt=strchr(pt,'<');
                strcpy((char*)buf,pt);
            }

            *os<<buf;
            xmlFree(buf);
        }
    }

    num_analyse_xml++;

	ParametresAffichage=parambase;
	ParametresAffichageDefini=ParametresAffichage;

    if (XML==OUI)
        *os<<"</XIPRESULT>"<<Endl;  
    DeduitFonction(-1,DEDUCTIONFINALE,NON);

}
//--------------------------------------------------------------------------------------
xmlNodePtr GlobalParseur::CreationNoeudXML(xmlNodePtr pere,char* balise,char* texte) {
    xmlNodePtr node;
    xmlChar* buff;
    xmlChar* champ=NULL;

    if (pere==NULL)
        return NULL;

    buff=xmlEncodeEntitiesReentrant(pere->doc, (uchar*)balise);    
    
    if (texte!=NULL)
        champ = xmlCodage(this,pere->doc, (uchar*)texte);                            

    Trim((char*)buff);
    node=creeNouveauNoeudXML(pere->doc, NULL,buff,champ);
    if (champ!=NULL)
        xmlFree(champ);
    if (pere!=basexmlcourante->XmlNodeCourant)
        xmlAddChild(pere,node);
    return node;
}
//--------------------------------------------------------------------------------------
void GlobalParseur::AjouteAttributValeur(xmlNodePtr node,char* attribut,char* valeur) {  
    xmlChar* val;

    if (valeur==NULL || attribut==NULL || node==NULL || node->type==XML_DOCUMENT_NODE)
        return;

    if (valeur[0]=='\'' && valeur[strlen((char*)valeur)]=='\'') {
        strcpy(valeur,valeur+1);
        valeur[strlen(valeur)-1]=0;
    }
    Trim(valeur);
    if (attribut[0]=='@')
        strcpy(attribut,attribut+1);

    //seule la valeur peut necessiter un transcodage
    val=xmlCodage(this,node->doc, (uchar*)valeur);

    xmlSetProp(node, (uchar*)attribut, (uchar*)val);
    if (val!=NULL)
      xmlFree(val);
}


//On regenere un fichier XML a partir d'un fichier texte
void GlobalParseur::GenereFichierXML(char* texte,
                                     char* racinebalise,
                                     char* encode,
                                     char renumerote,
                                     char tree) {
    ostringstream textefichier;
    ifstream fichier(texte,modeOuverture);
    string buffer;

    if (fichier.fail() ) {
		buffer=CHAINES[97];
		buffer+=texte;
        erreur(STR(buffer));
    }

	string encoder=c_Tolower(encode);
    bool latinencode=true;
	if (encoder[0]=='u')
		latinencode=false;

    //On lit le contenu du fichier en memoire
    while (!fichier.eof()) {
		getline(fichier,buffer);
		if (latinencode==false)
			buffer=conversionLatinVersUTF8(buffer);
		else
			buffer=conversionUTF8VersLatin(buffer);
        textefichier<<buffer;
    }
    renseigne(texte);
    AnalyseChaineTexteVersXML(textefichier.str(),racinebalise,encode,renumerote,tree);
}


void GlobalParseur::AnalyseChaineTexteVersXML(string textefichier,
                                              char* racinebalise,
                                              char* encode,
                                              char renumerote,
                                              char tree) {
    char buffer[100]; 
    
	if (!strcmpi(encode,"ascii"))
        encode="UTF-8";

    sprintf(buffer,"<%s/>",racinebalise);            
    //On cree la racine de notre document
    xmlDocPtr doc=xmlSAXParseMemory(NULL,buffer,strlen(buffer),0);
    doc->encoding=xmlCharStrdup(encode);
    xmlChar* valeur = conversionAsciiVersUTF8((uchar*)textefichier.c_str());    
    //On stocke tout dans le champ contenu associe au noeud
    xmlAddChild(doc->children,xmlNewDocText(doc,valeur));
    //Puis on appelle l'analyse
    
    LIGNES=0;
    NOM="stringxml";

    basexmlcourante->xpathcourant=NULL;        
	basexmlcourante->document=doc;
    num_analyse_xml=0;
    balise_analyse=NULL;
    profondeur_analyse=0;
    texte_xml_courant=NULL;
    baseOffset=0;
	baseOffsetCar=0;
    
    //On cree notre structure porteur d'eau
    //Cette structure permet de garder une trace de la creation
    //des noeuds XML integres dans le resultat XML.
    xptpos* xp=new xptpos(0,doc->children);

    //Quelques initialisations obligatoires
    //XML=OUTPUTLIBXML;
    //gAfficherResultats=NON;
    //gAfficherResultatsDefini=NON;
    //ParametresAffichage=AFFICHAGE_OFFSET|AFFICHAGE_FICHIER_XML_ECRAN;
    //ParametresAffichageDefini=AFFICHAGE_OFFSET|AFFICHAGE_FICHIER_XML_ECRAN;

    basexmlcourante->xpathcourant=xmlXPathNewContext(doc);
    

    ParseFichierXML(doc);

    //Que l'on detruit en fin d'analyse
    delete xp;
    liste_offsets.nettoie();

        
    AfficheVariablesGlobales();
    DeductionFinale=NON;
    xmlFreeDoc(doc);
    if (basexmlcourante->xpathcourant!=NULL)
        xmlXPathFreeContext(basexmlcourante->xpathcourant);
    basexmlcourante->xpathcourant=NULL;
    basexmlcourante->XmlNodeCourant=NULL;
    basexmlcourante->XmlReferenceNode=NULL;
	basexmlcourante->document=NULL;

}

#endif











