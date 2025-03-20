#include <afxinet.h>
#include <afxwin.h>
#include <process.h>
#include <stdlib.h>


#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libxml/xmlmemory.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
using namespace std;

#define TailleChaine 512
#define NON 0
#define AUTOMATEPILE 2
#define XIPLIBXML
#include "xipvecta.h"
#include "xipstlres.h"
#include "resinter.h"
#include "automate.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

extern TCHAR szHello[50000];

extern "C" {
    void xmlParserInputShrink(xmlParserInputPtr in);
    xmlXPathObjectPtr xmlXPathNewNodeSet(xmlNodePtr val);
    void xmlXPathNodeSetAddUnique(xmlNodeSetPtr cur, xmlNodePtr val);
    int isolat1ToUTF8(unsigned char* out, int *outlen,const unsigned char* in, int *inlen);
    void xmlNodeDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc, 
        xmlNodePtr cur,int level, int format, const char *encoding);
    int xmlNodeDump(xmlBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur, int level,int format);
    int xmlXPathRegisterNs(xmlXPathContextPtr ctxt, const xmlChar *prefix,const xmlChar *ns_uri);
}

static UINT FINPOS=-1;


void CallAfterEachSentence(int ipar,XipResult* res,void* data) {
    SortedWords* sw=(SortedWords*)data;
    XipUnit* xu=res->sentences[0];
    for (int i=0;i<xu->leaves.size();i++) {
        XipLeaf* leaf=(XipLeaf*)xu->leaves[i]->daughters[0];
        if (leaf->lemma==sw->verb || leaf->lemma==sw->word) {
            //On affiche alors la phrase
            *sw->fichier<<"<SENT>"<<endl;
            for (int j=0;j<xu->leaves.size();j++) {
                leaf=(XipLeaf*)xu->leaves[j]->daughters[0];
                *sw->fichier<<leaf->surface<<" ";
            }
            *sw->fichier<<endl<<"</SENT>"<<endl;
            break;
        }
    }
    XipCleanCurrentXipResult(ipar);    
}


void RechercheWeb(SortedWords* sw,ostringstream& osi,ofstream& resultat) {    
    
    CInternetSession session("WebSearch");
    CStdioFile* pFile = NULL;
    char szBuff[5001];    
    ostringstream os;
    int i=0;
    
    
    resultat<<"<WORD verb=\""<<sw->verb<<"\" syn=\""<<sw->word<<"\">"<<endl;
    try{            
        sprintf(szBuff,
            "http://query.nytimes.com/search/query?srcht=a&srchot=a&query=%s+AND+%s&field=body&mon0=03&day0=24&year0=2004&sort=closest&sources=all",
            sw->verb,
            sw->word);
        
        pFile = session.OpenURL(szBuff);
        
        UINT nRead=1;
        while (nRead>0) {
            nRead = pFile->Read(szBuff, 5000);
            szBuff[nRead]=0;
            os<<szBuff;
        }        
        delete pFile;
        //On recherche alors la chaine qui nous interesse:
        
        string& pt=os.str();                     
        UINT ptdeb=0,ptfin=0;
        
        while (ptdeb!=-1) {
            ptdeb=pt.find("<!-- RESULT_ROW_BEGIN -->",ptdeb);
            ptfin=ptdeb+1;
            if (ptdeb==FINPOS)
                break;
            
            unsigned char* debutchaine= (unsigned char*)pt.c_str()+ptdeb;
            string categorie="NONE";
            //On applique notre automate pour detecter les categories
            static automatepile acateg((unsigned char*)"?*<strong>%1?+%2 |",NULL);
            if (acateg.recherchepartielle(debutchaine)!=NULL) {
                acateg.extraction(debutchaine);
                categorie=(char*)acateg.chaines[2];
                ptfin=acateg.maxpos;
            }
            
            string adresse="NONE";
            //On recherche ensuite l'adresse du document
            static automatepile aadr((unsigned char*)"?*a href=\"%1?+%2\"",NULL);
            if (aadr.recherchepartielle(debutchaine+ptfin)!=NULL) {
                aadr.extraction(debutchaine+ptfin);
                adresse=(char*)aadr.chaines[2];
                ptfin=aadr.maxpos;
            }
            
            resultat<<"<GROUP>"<<endl;
            resultat<<"<CATEGORY>"<<categorie<<"</CATEGORY>"<<endl;
            
            //Il s'agit d'un code pour avoir l'article sur une seule page en entier
            adresse+="?pagewanted=print&position=";
            //On dispose alors de l'adresse dans le NY-times  
            pFile = session.OpenURL(adresse.c_str());
            //On lit le fichier en memoire
            
            
            ostringstream article;            
            nRead=1;
            while (nRead>0) {
                nRead = pFile->Read(szBuff, 5000);
                szBuff[nRead]=0;
                article<<szBuff;
            }        
            UINT debutarticle=article.str().find("<NYT_TEXT>",0);
            if (debutarticle!=FINPOS) {
                debutarticle=article.str().find("<p>",debutarticle);
                debutarticle+=3;
                UINT finarticle=article.str().find("</NYT_TEXT>",debutarticle);
                if (finarticle!=FINPOS) {
                    string texte=article.str().substr(debutarticle,finarticle-debutarticle);
                    
                    //On appelle alors XIP
                    XipResult* xipres=XipCreateCurrentXipResult(0);
                    XipParseString(texte,0, xipres,0);            
                    XipDeleteCurrentXipResult(0);
                }
            }
            resultat<<"</GROUP>"<<endl;
            delete pFile;            
            ptdeb=ptfin;
        }        
    }
    catch (...) 
    {
        os<<"Erreur on WEB searching"<<endl;
    }
    
    resultat<<"</WORD>"<<endl;
    session.Close();                
    
    
}

extern int dessin;

char RemplaceEspace(char* word) {
    char ch[1000];
    if (strchr(word,' ')) {
        for (int i=0;i<strlen(word);i++) {
            if (word[i]==32)
                word[i]='+';
        }
        sprintf(ch,"%%22%s%%22",word);
        strcpy(word,ch);
        return 1;
    }
    return 0;
}

void ReadingWordFile(void* data) {
    
    int max=1000;
    
    echange* e= (echange*)data;
    ostringstream& osi= *e->os; 
    //First we initate the loading of the grammar
    string grmfilename="C:\\XIP\\Grammaire2.1\\testvitesse\\english.grm";
    strcpy(szHello,"\r\nGrammaire chargement\r\n");    
    RedrawWindow(e->hwnd,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN|RDW_ERASENOW|RDW_UPDATENOW);
    //UpdateWindow(e->hwnd);    
    int ipar=XipGrmFile(grmfilename);
    strcat(szHello,"\r\nGrammaire chargement fini\r\n");    
    RedrawWindow(e->hwnd,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN|RDW_ERASENOW|RDW_UPDATENOW);
    
    //We have our word structure to start with
    SortedWords sw;
    ofstream resultat("C:\\XIP\\XIPIIbis\\resinternet\\results\\newyorktimes.txt");
    sw.fichier=&resultat;
    resultat<<"<NY>"<<endl;
    XipSetCallBack(ipar,CallAfterEachSentence,(void*)&sw);
    
    //We load our RDF file
    strcat(szHello,"\r\nDebut chargement RDF\r\n");    
    RedrawWindow(e->hwnd,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN|RDW_ERASENOW|RDW_UPDATENOW);
    
    
    xmlDocPtr doc=xmlParseFile("C:\\XIP\\Grammaire2.1\\wordnetrdf\\words.xml");
    strcat(szHello,"RDF chargement fini\r\n");    
    RedrawWindow(e->hwnd,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN|RDW_ERASENOW|RDW_UPDATENOW);
    
    xmlXPathContextPtr xpath=xmlXPathNewContext(doc);
    xmlNodePtr nn=doc->children;
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
    strcat(szHello,"Lancement du XPath\r\n");    
    RedrawWindow(e->hwnd,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN|RDW_ERASENOW|RDW_UPDATENOW);

    const unsigned char chemin[]="/rdf:RDF/b:Verb";
    xmlXPathObjectPtr listeNode=xmlXPathEval(chemin,xpath);
    strcat(szHello,"XPath effectue\r\n");    
    RedrawWindow(e->hwnd,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN|RDW_ERASENOW|RDW_UPDATENOW);

    xmlNodePtr noeud;
    szHello[0]=0;
    if (listeNode!=NULL && listeNode->nodesetval!=NULL && listeNode->nodesetval->nodeNr>0) {
        for (int truc=0;truc<listeNode->nodesetval->nodeNr;truc++) {
            if (max<0)
                break;
            noeud=listeNode->nodesetval->nodeTab[truc];
            //On extrait les mots
            if (noeud->properties!=NULL) {
                xmlAttrPtr propriete=noeud->properties;                
                while (propriete!=NULL) {                                 
                    if (!strcmp((char*)propriete->name,"wordForm")) {
                        strcpy(sw.verb,(char*)propriete->children->content);
                        //then we deal with the other possible synonyms  
                        if (noeud->children !=NULL) {
                            xmlNodePtr nx=noeud->children->next;
                            while (nx!=NULL) {
                                while (nx!=NULL && strcmp((char*)nx->name,"wordForm"))
                                    nx=nx->next;
                                if (nx!=NULL) {
                                    strcpy(sw.word,(char*)nx->children->content);
                                    dessin=0;
                                    sprintf(szHello,"%s:%s",sw.verb,sw.word);
                                    RedrawWindow(e->hwnd,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN|RDW_ERASENOW|RDW_UPDATENOW);                                    
                                    RedrawWindow(e->hwnd,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN|RDW_ERASENOW|RDW_UPDATENOW);
                                    while(dessin==0);

                                    if (!RemplaceEspace(sw.word)) {
                                        max--;
                                        //RechercheWeb(&sw,*e->os,resultat);
                                    }
                                    nx=nx->next;
                                }
                            }
                        }
                        break;
                    }
                    propriete=propriete->next;
                }
            }
            
        }
    }
    
    resultat<<"</NY>"<<endl;
    XipFreeGrammar(0);
    _endthread();
}