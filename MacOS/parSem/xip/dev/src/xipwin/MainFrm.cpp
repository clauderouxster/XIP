// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#define XIPLIBXML
#include "xipstlres.h"
#include <stdlib.h>
#include <stdio.h>

#include "xipwin.h"

#include "MainFrm.h"

#include "DirDialog.h"
#include "xipwinDoc.h"
#include "xipwinView.h"
#include "resultat.h"
#include "resdoc.h"
#include <fstream>
#include "RuleBuilding.h"
#include "seerule.h"
#include "ruledoc.h"
#include "pasofficiel.h"
#include "generate.h"
#include "saisiedoc.h"
#include "Filetree.h"
#include "inforule.h"
#include "Featuretree.h"
#include "helpscripting.h"
#include ".\mainfrm.h"

#include <map>
#include "conversion.h"

BOOL python_debug_mode=FALSE;
extern BOOL output_activated;

extern VECTA<rule_file*> new_documents;
string xip_lemma;
string xip_surface;
int rule_limit=-1;

char notntmonly=1;
int theRuleLimit=-1;
char input_type=XML_INSERT;

extern CXipwinApp theAppXIPWIN;
XipResult* resxip=NULL;
int iAnalysePhrase;
CString grammarFileName[10];
extern CString pathNameIndent;
int CompteAnalyse=0;
VECTA<char*> parameterfiles;
int parseurCourant;
char erreurChgt=1;
char premierChgt=1;
char choixXML=0;
extern CString grammarPathInput;
VECTA<long> updatedlistlabels;
VECTA<long> originallabels;

vector<string> feature_pathnames;
vector<string> lexicon_pathnames;
vector<string> rule_pathnames;
vector<string> param_pathnames;
vector<string> scripts_pathnames;
vector<string> divers_pathnames;

VECTA<Label*>* labels_numero=NULL;
VECTA<Label*>* labels_index=NULL;

string languageFilename;


vector<string> liste_categories;
vector<string> liste_dependances;

extern CMultiDocTemplate* pGrTemplate;
extern CMultiDocTemplate* pDocTemplate;
extern CMultiDocTemplate* pRuleTemplate;
extern CMultiDocTemplate* pEditTemplate;
extern CXipwinDoc* docoutput;

XIPLONG ReturnDisplayMode();
void SetDisplayMode(XIPLONG opt);

extern CXipwinDoc* DocumentSaisie;
void UpdateDisplayMode();

long offset_begins[3];

#ifdef _DEBUG
#define new DEBUG_NEW
#define statique
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#else
#define statique static
#endif


char* insertwindowsnewline(string& texte) {
	texte = conversion_utf8_to_latin(texte);

    static char* buffer=NULL;
	static int taille=0;

	if (taille < texte.size()*2) {
		if (buffer!=NULL)
			delete[] buffer;
		taille=texte.size()*2+1;
		buffer=new char[taille];
	}

    int j=0;    
    for (int i=0;i<texte.size();i++) {
        if (texte[i]=='\r' && texte[i+1]=='\n') {
            buffer[j++]='\r';
            i++;
            buffer[j++]='\n';
        }
        else
            if (texte[i]=='\n' || texte[i]=='\r') {
                buffer[j++]='\r';
                buffer[j++]='\n';
            }
            else
                buffer[j++]=texte[i];
    }
    buffer[j]=0;
    return buffer;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
#ifdef XIPSTLAPI
int ExecuterChaine(char* texte,int ipar, std::ostrstream* os);
#else
int ExecuterChaine(char* texte,int ipar, ostrstream* os);
#endif

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_ANALYSE_PARSE, OnAnalyseParse)
	ON_COMMAND(ID_BUTTON32772, OnButton32772)
	ON_WM_CLOSE()
	ON_COMMAND(ID_BUTTON32774, OnButton32774)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32774, OnUpdateButton32774)
	ON_COMMAND(ID_BUTTON32775, OnButton32775)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32775, OnUpdateButton32775)
	ON_COMMAND(ID_ANALYZIS_RELOADGRAMMAR, OnAnalyzisReloadgrammar)
	ON_UPDATE_COMMAND_UI(ID_ANALYZIS_RELOADGRAMMAR, OnUpdateAnalyzisReloadgrammar)
	ON_COMMAND(ID_BUTTON32779, OnButton32779)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32772, OnUpdateButton32772)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32779, OnUpdateButton32779)
	ON_UPDATE_COMMAND_UI(ID_ANALYSE_PARSE, OnUpdateAnalyseParse)
	ON_COMMAND(ID_BUTTONXML, OnButtonxml)
	ON_UPDATE_COMMAND_UI(ID_BUTTONXML, OnUpdateButtonxml)
	ON_COMMAND(ID_BUTTONTXT, OnButtontxt)
	ON_UPDATE_COMMAND_UI(ID_BUTTONTXT, OnUpdateButtontxt)
	ON_COMMAND(ID_PARSING_PARSETEXTTOXML, OnParsingParsetexttoxml)
	ON_UPDATE_COMMAND_UI(ID_PARSING_PARSETEXTTOXML, OnUpdateParsingParsetexttoxml)
	ON_COMMAND(id_createrule, Oncreaterule)
	ON_UPDATE_COMMAND_UI(id_createrule, OnUpdatecreaterule)
	ON_COMMAND(ID_rule_number, Onrulenumber)
	ON_COMMAND(ID_PARSING_PARSEFILE, OnParsingParsefile)
	ON_COMMAND(IDC_seefiles, Onseefiles)
	ON_UPDATE_COMMAND_UI(IDC_seefiles, OnUpdateseefiles)
	ON_COMMAND(IDC_info, Oninfo)
	ON_UPDATE_COMMAND_UI(IDC_info, OnUpdateinfo)
	ON_COMMAND(IDC_featuredisplay, Onfeaturedisplay)
	ON_UPDATE_COMMAND_UI(IDC_featuredisplay, OnUpdatefeaturedisplay)
	ON_COMMAND(id_help_scripting, Onhelpscripting)
	ON_COMMAND(ID_XML_INPUT, OnXmlInput)
	ON_UPDATE_COMMAND_UI(ID_XML_INPUT, OnUpdateXmlInput)
	ON_COMMAND(ID_NTM_ONLY, OnNtmOnly)
	ON_UPDATE_COMMAND_UI(ID_NTM_ONLY, OnUpdateNtmOnly)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_RULE_DEBUGFROM0, OnRuleDebugfrom0)
	ON_UPDATE_COMMAND_UI(ID_RULE_DEBUGFROM0, OnUpdateRuleDebugfrom0)
	ON_UPDATE_COMMAND_UI(ID_BUTTONPLUS, OnUpdateButtonplus)
	ON_COMMAND(ID_BUTTONPLUS, OnButtonplus)
	ON_COMMAND(ID_BUTTONMOINS, OnButtonmoins)
	ON_UPDATE_COMMAND_UI(ID_BUTTONMOINS, OnUpdateButtonmoins)
	ON_COMMAND(ID_RELOAD_AND_PARSE, OnReloadAndParse)
	ON_UPDATE_COMMAND_UI(ID_RELOAD_AND_PARSE, OnUpdateReloadAndParse)
	ON_COMMAND(ID_DEPENDANCE_ANALYSE, &CMainFrame::OnDependanceAnalyse)
	ON_UPDATE_COMMAND_UI(ID_DEPENDANCE_ANALYSE, &CMainFrame::OnUpdateDependanceAnalyse)
	ON_UPDATE_COMMAND_UI(ID_DEBUGDEP, &CMainFrame::OnUpdateDebugdep)
	ON_COMMAND(ID_DEBUGDEP, &CMainFrame::OnDebugdep)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_LIGNE
};


CMainFrame* laMain=NULL;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
    parseurCourant=-1;
    TreeDisplay=FALSE;

	laMain=this;
    if (resxip==NULL)
        resxip=XipResultCreate();
}

CMainFrame::~CMainFrame() 
{
    if (parseurCourant!=-1)
        XipFreeGrammar(parseurCourant);
    
    XipResultDelete(resxip);
    for (int i=0;i<parameterfiles.dernier;i++)
        free(parameterfiles[i]);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{    
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
    
    
	return 0;
}

void CMainFrame::AfficheLigne(CString& l) {
	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_LIGNE), l);
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

//void testcallback(int handler,XipResult* xip,void* data) {
//	cout<<handler<<endl;
//}


void XipGetUnknownVariables(VECTA<char*>&);
int ChargerGrammaire(CString& grammar,char init) {        
            
    static char errorch[1000];
    try {            
        parseurCourant=XipGrmFile(LPCTSTR(grammar));
        grammarFileName[parseurCourant]=grammar;
		//XipSetCallBack(parseurCourant,testcallback,NULL);
        erreurChgt=0;        
        premierChgt=0;
    }
    catch (XIPError* message) {
		VECTA<char*> unknowns;
		XipGetUnknownVariables(unknowns);
		if (unknowns.dernier!=0) {
			CString message=unknowns[0]+1;
			CDirDialog dlg(NULL,message);
			int rep=dlg.DoModal();
			if (rep==IDOK) {				
				CString chemin=dlg.GetPath();
				if (chemin[chemin.GetLength()-1]=='\\')
					chemin=chemin.Left(chemin.GetLength()-1);
				message=unknowns[0]+1;
				message+="=";
				message+=chemin;
				putenv(LPCTSTR(message));
				return ChargerGrammaire(grammar,init);				
			}
		}
		else {			
			sprintf(errorch,"%s\r\n file: %s line:%d",message->message.c_str(),message->filename.c_str(),message->line);
			AfxMessageBox(errorch,MB_OK);
			delete message;
			erreurChgt=1;
			return -100;
		}
    }

    pathNameIndent = XipIndentFilePathname(parseurCourant);
    
    XipGetLabels(parseurCourant, xip_surface,xip_lemma);
    updatedlistlabels.raz();    
	labels_index=XipReturnInfoRuleIndex(parseurCourant);
	labels_numero=XipReturnInfoRuleNumero(parseurCourant);


    originallabels.raz();
    feature_pathnames.clear();
    lexicon_pathnames.clear();
    rule_pathnames.clear();
    new_documents.nettoie();
    param_pathnames.clear();
	divers_pathnames.clear();
	scripts_pathnames.clear();
    XipGrammarPathnames(parseurCourant,
        languageFilename,        
        feature_pathnames,
        offset_begins[0],
        lexicon_pathnames,
        offset_begins[1],
        rule_pathnames,
        offset_begins[2],
        param_pathnames,
		scripts_pathnames,
		divers_pathnames);

	if (labels_numero!=NULL) {
		for (int i=0;i<labels_numero->dernier;i++) {
			if (labels_numero->vecteur[i]->dr!=NULL) 
				originallabels.ajoute(labels_numero->vecteur[i]->dr->position);
		}

		liste_categories.clear();
		liste_dependances.clear();

		XipGetListCategories(parseurCourant,liste_categories);
		XipGetListDependencies(parseurCourant,liste_dependances);

		UpdateDisplayMode();
	}
    return XipLicense(parseurCourant);
}

void XMLpretty(string input,CString& res) {
    //first we read all the markup tags
    vector<string> exploded;
    string base="";
    int i;
    for (i=0;i<input.size();i++) {
        if (input[i]=='<') {
            string s="";            
            while (input[i]!='>') {
                s+=input[i];
                i++;
            }
            s+=input[i];            
            int j=i+1;
            unsigned char c=input[j];            
            while (j<input.size() && c<32) c=input[++j];
            i=j-1;

            if (base.size()!=0)
                exploded.push_back(base);
            exploded.push_back(s);
            base="";
        }
        else
            if (input[i]!='\r') {
                if (input[i]=='\n')
                    base+='\r';
                base+=input[i];            
            }
    }
    if (base.size()!=0)
        exploded.push_back(base);
    
    int indent=0;
    int taille;
    int j;
    for (i=0; i<exploded.size();i++) {
        if (!i) {
            res+=exploded[i].c_str();
            res+="\r\n";
            continue;
        }
        if (exploded[i][0]=='<') {
            if (exploded[i][1]=='/') {//final 
                indent-=3;
                if (i>0) {
                    if (exploded[i-1][0]=='<') {
                        res+="\r\n";                
                        for (j=0;j<indent;j++)
                            res+=" ";
                    }
                    else {
                        taille=exploded[i-1].size()-1;
                        if (exploded[i-1][taille]=='\n') {
                            for (j=0;j<indent;j++)
                                res+=" ";
                        }
                    }
                }
            }
            else {                
                if (exploded[i-1][0]=='<') {
                    res+="\r\n";                
                    for (j=0;j<indent;j++)
                        res+=" ";
                }
                else {
                    taille=exploded[i-1].size()-1;
                    if (exploded[i-1][taille]=='\n') {
                        for (j=0;j<indent;j++)
                            res+=" ";
                    }
                }
                taille=exploded[i].size()-2;
                if (exploded[i][taille]!='/')
                    indent+=3;
            }
            res+=exploded[i].c_str();
        }
        else
            res+=exploded[i].c_str();
    }
}

void CMainFrame::OnAnalyseParse() 
{
	// TODO: Add your command handler code here    
    //On recupere la vue active qui contiendra les resultats
    //POSITION pos= theApp.GetFirstDocTemplatePosition();
    //CDocTemplate* cdoc = theApp.GetNextDocTemplate(pos ); 


    //Pour s'éviter le message de sauvegarde...
    DocumentSaisie->SetModifiedFlag(FALSE);
        

    CXipwinDoc* doc= (CXipwinDoc*)pDocTemplate->OpenDocumentFile(NULL);
    
    CString chaine("Result Id:");
    char ch[2048];
    chaine+= itoa(CompteAnalyse,ch,10);
    doc->SetTitle(chaine);

    if (parseurCourant==-1)
        return;
    
    CString texte;

    CView* vue= DocumentSaisie->GetViewSaisie();
    DocumentSaisie->OnSaveDocument(grammarPathInput);
      
    vue->GetWindowText(texte);        

    BeginWaitCursor();
    XipSetRuleLimit(parseurCourant,rule_limit);
	XipSetRuleRankNumberLimit(parseurCourant,-1);
	if (texte.GetLength() > 1) {
		texte+="\r\n";
		if (output_activated==FALSE) {
			delete resxip;
			resxip=XipResultCreate();
			XipParseString(LPCTSTR(texte),parseurCourant,resxip,notntmonly);
			doc->texte="NO OUTPUT MODE\r\n";
		}
		else {
			try {			
				if (TreeDisplay==TRUE && choixXML==0) {
					delete resxip;
					resxip=XipResultCreate();
					XipParseString(LPCTSTR(texte),parseurCourant,resxip,notntmonly); 
					for (iAnalysePhrase=0;iAnalysePhrase<resxip->sentences.size();iAnalysePhrase++) {
						CResdoc* gr= (CResdoc*)pGrTemplate->OpenDocumentFile(NULL);
						CString titre("Result Id:");
						titre+= itoa(CompteAnalyse,ch,10);
						titre+=" Sentence=";
						titre+=itoa(iAnalysePhrase,ch,10);
						gr->SetTitle(titre);
					}
				}

				ostringstream os;
				string stexte=LPCTSTR(texte);
				if (choixXML==0) {
					XipParseStringOS(stexte,parseurCourant, &os,0,notntmonly);
					os<<"\r\n\r\n";                
					doc->texte+=insertwindowsnewline(os.str());
				}
				else {
					XipParseStringXMLOS(stexte,parseurCourant, &os,0,input_type,notntmonly);                    
					//On va rajoute quelques retours chariots            
					int espace=0;            
					XMLpretty(os.str(),doc->texte);
				}
			}
			catch(XIPError* err) {
				static char errorch[1000];
				sprintf(errorch,"%s\r\n file: %s line:%d",err->message.c_str(),err->filename.c_str(),err->line);
				AfxMessageBox(errorch,MB_OK);
				delete err;
			}
		}
		CompteAnalyse++;
	}
	EndWaitCursor();

	doc->UpdateAllViews(NULL);
}

void CMainFrame::OnButton32772() 
{
    OnAnalyseParse();	
}

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnButton32774() 
{
    TreeDisplay=TRUE-TreeDisplay;
}

void CMainFrame::OnUpdateButton32774(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(TRUE-TreeDisplay);
        
}

void CMainFrame::OnButton32775() 
{
	// TODO: Add your command handler code here
    TreeDisplay=TRUE-TreeDisplay;
	
}

void CMainFrame::OnUpdateButton32775(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TreeDisplay);
	
}

void SetPythonDebugMode(char test,int module,int ligne) {    
	try {            
		XipSetPythonDebug(parseurCourant,1,(char*)LPCTSTR(grammarFileName[parseurCourant]),test,module,ligne);	

		XIPLONG opt=ReturnDisplayMode();
		opt|=CHECK_INPUT_UTF8;
		XipSetDisplayMode(parseurCourant,opt);

		feature_pathnames.clear();
		lexicon_pathnames.clear();
		rule_pathnames.clear();
		new_documents.nettoie();            
		param_pathnames.clear();
		divers_pathnames.clear();
		scripts_pathnames.clear();
		XipGrammarPathnames(parseurCourant,
			languageFilename,        
			feature_pathnames,
			offset_begins[0],
			lexicon_pathnames,
			offset_begins[1],
			rule_pathnames,
			offset_begins[2],
			param_pathnames,
			scripts_pathnames,
			divers_pathnames);

		originallabels.raz();
		liste_categories.clear();
		liste_dependances.clear();

		erreurChgt=0;
		XipGetListCategories(parseurCourant,liste_categories);
		XipGetListDependencies(parseurCourant,liste_dependances);
		updatedlistlabels.raz();
		labels_index=XipReturnInfoRuleIndex(parseurCourant);
		labels_numero=XipReturnInfoRuleNumero(parseurCourant);
		int i;

		//POSITION firstdoc=pEditTemplate->GetFirstDocPosition();
		//CSaisieDoc* doc;
		//while (firstdoc!=NULL) {
		//    doc=(CSaisieDoc*)pEditTemplate->GetNextDoc(firstdoc);
		//    doc->OnCloseDocument();
		//}
		POSITION firstdoc=pRuleTemplate->GetFirstDocPosition();            
		CRuleDoc* rg;
		while (firstdoc!=NULL) {
			rg=(CRuleDoc*)pEditTemplate->GetNextDoc(firstdoc);
			rg->OnCloseDocument();
		}

		for (i=0;i<labels_numero->dernier;i++)
			if (labels_numero->vecteur[i]->dr!=NULL) 
				originallabels.ajoute(labels_numero->vecteur[i]->dr->position);

	}
	catch (XIPError* message) {
		static char errorch[1000];
		sprintf(errorch,"%s\r\n file: %s line:%d",message->message.c_str(),message->filename.c_str(),message->line);
		AfxMessageBox(errorch,MB_OK);
		delete message;
		erreurChgt=1;
	}
}          

void CMainFrame::OnAnalyzisReloadgrammar() 
{    
    
    if (premierChgt==1) {
        CFileDialog scriptgrm(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Grammar (*.grm)|*.grm|");
        
        if (scriptgrm.DoModal()==IDCANCEL)
            return;
       
        BeginWaitCursor();
        int tempsRestant=ChargerGrammaire(scriptgrm.GetPathName(),0);
        if (tempsRestant!=-1) {
            if (tempsRestant>=0) {
                char chif[400];
                if (tempsRestant==0)
                    strcpy(chif,"License has expired");
                else
                    sprintf(chif,"Warning: license expires in %d day(s)",tempsRestant);                
                AfxMessageBox(chif,MB_OK);
            }
            if (tempsRestant<=0)
                return;
        }          
        EndWaitCursor();        
    }
    else {
        try {            
            BeginWaitCursor();
			XipReloadGrammar(parseurCourant,1,(char*)LPCTSTR(grammarFileName[parseurCourant]));              
        
			XIPLONG opt=ReturnDisplayMode();
			opt|=CHECK_INPUT_UTF8;
			XipSetDisplayMode(parseurCourant,opt);

            feature_pathnames.clear();
            lexicon_pathnames.clear();
            rule_pathnames.clear();
            new_documents.nettoie();            
            param_pathnames.clear();
			divers_pathnames.clear();
			scripts_pathnames.clear();
            XipGrammarPathnames(parseurCourant,
                languageFilename,        
                feature_pathnames,
                offset_begins[0],
                lexicon_pathnames,
                offset_begins[1],
                rule_pathnames,
                offset_begins[2],
                param_pathnames,
				scripts_pathnames,
				divers_pathnames);
            
            originallabels.raz();
            liste_categories.clear();
            liste_dependances.clear();

            erreurChgt=0;
            XipGetListCategories(parseurCourant,liste_categories);
            XipGetListDependencies(parseurCourant,liste_dependances);
            updatedlistlabels.raz();
			labels_index=XipReturnInfoRuleIndex(parseurCourant);
			labels_numero=XipReturnInfoRuleNumero(parseurCourant);
			int i;

            //POSITION firstdoc=pEditTemplate->GetFirstDocPosition();
            //CSaisieDoc* doc;
            //while (firstdoc!=NULL) {
            //    doc=(CSaisieDoc*)pEditTemplate->GetNextDoc(firstdoc);
            //    doc->OnCloseDocument();
            //}
            POSITION firstdoc=pRuleTemplate->GetFirstDocPosition();            
            CRuleDoc* rg;
            while (firstdoc!=NULL) {
                rg=(CRuleDoc*)pEditTemplate->GetNextDoc(firstdoc);
                rg->OnCloseDocument();
            }

            for (i=0;i<labels_numero->dernier;i++)
                if (labels_numero->vecteur[i]->dr!=NULL) 
                    originallabels.ajoute(labels_numero->vecteur[i]->dr->position);

            EndWaitCursor();        
        }
        catch (XIPError* message) {
            static char errorch[1000];
            sprintf(errorch,"%s\r\n file: %s line:%d",message->message.c_str(),message->filename.c_str(),message->line);
            AfxMessageBox(errorch,MB_OK);
            delete message;
            erreurChgt=1;
        }
    }       
    
}

void CMainFrame::OnUpdateAnalyzisReloadgrammar(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    pCmdUI->Enable(TRUE);    
	
}

void CMainFrame::OnButton32779() 
{
    XipIndentFile(parseurCourant,1);
    OnAnalyseParse();	
	XipIndentFile(parseurCourant,0);
    DocumentSaisie->ViewIndentoutput();
}


void lit(ifstream& f,char* buf,int max) {
    int i=0;
    char c;
    while (i<max && !f.eof()) {
        f.get(c);
        if (c==13 || c==10 || c=='\n')
            break;
        buf[i++]=c;
    }
    buf[i]=0;
}       

void Trim(ifstream& f,char* buf,int max) {
    char boucle=1;
    buf[0]=0;
    lit(f,buf,max);
    int l;
    int i;
    while (!f.eof()) {           
        l=strlen(buf);
        i=l-1;
        while (i>=0 && buf[i]<=32)
            buf[i--]=0;
        i=0;
        while (buf[i]<=32 && buf[i]!=0)
            i++;
        if (i)
            strcpy(buf,buf+i);
        if (buf[0]!=0 && buf[0]!='#')
            return;
        lit(f,buf,max);
    }    
    buf[0]=0;
}



void CMainFrame::OnUpdateButton32772(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    pCmdUI->Enable(erreurChgt==0);	
}

void CMainFrame::OnUpdateButton32779(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);	
}

void CMainFrame::OnUpdateAnalyseParse(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    pCmdUI->Enable(erreurChgt==0);		
}

void CMainFrame::OnButtonxml() 
{
	// TODO: Add your command handler code here
	choixXML=0;
    input_type=XML_INSERT;
    UpdateDisplayMode();
}

void CMainFrame::OnUpdateButtonxml(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(choixXML==1);
}

void CMainFrame::OnButtontxt() 
{
	// TODO: Add your command handler code here
	choixXML=1;
    XIPLONG opt=ReturnDisplayMode();
    opt&=~DISPLAY_WORDNUM;
    opt|=DISPLAY_NONE|DISPLAY_OFFSET|UTF8_INPUT;
    SetDisplayMode(opt);
}

void CMainFrame::OnUpdateButtontxt(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(choixXML==0);
}

void CMainFrame::OnParsingParsetexttoxml() 
{
	// TODO: Add your command handler code here    
    //On recupere la vue active qui contiendra les resultats
    //POSITION pos= theApp.GetFirstDocTemplatePosition();
    //CDocTemplate* cdoc = theApp.GetNextDocTemplate(pos ); 


    //Pour s'éviter le message de sauvegarde...
    DocumentSaisie->SetModifiedFlag(FALSE);
        

    CXipwinDoc* doc= (CXipwinDoc*)pDocTemplate->OpenDocumentFile(NULL);
    
    CString chaine("Result Id:");
    char ch[100];
    chaine+= itoa(CompteAnalyse,ch,10);
    doc->SetTitle(chaine);

    if (parseurCourant==-1)
        return;
    
    CString texte;

    CView* vue= DocumentSaisie->GetViewSaisie();
    
      
    vue->GetWindowText(texte);        

    BeginWaitCursor();
    if (texte.GetLength() > 1) {        
        
        ostringstream os;
        string stexte=LPCTSTR(texte);
        XipSetDisplayMode(parseurCourant,DISPLAY_NONE|DISPLAY_OFFSET|CHECK_INPUT_UTF8);
		try {
			XipParseStringTextToXMLOS(stexte,parseurCourant,&os,"root","ascii",input_type);        
		}
		catch(XIPError* err) {
			static char errorch[1000];
			sprintf(errorch,"%s\r\n file: %s line:%d",err->message.c_str(),err->filename.c_str(),err->line);
			AfxMessageBox(errorch,MB_OK);
			delete err;
		}

        //On va rajoute quelques retours chariots
        int espace=0;
        string schn=(char*)os.str().c_str();
        char* chn=(char*)schn.c_str();
        int lalong=os.str().size();
        for (int i=0;i<lalong;i++) {
            if (chn[i]=='\n')
                doc->texte+="\r\n";
            else {
                
                int j;
                    
                if (chn[i]=='<') { 
                    int increment=0;
                    doc->texte+="\r\n";
                    if (i+1<lalong && chn[i+1]=='/')
                        espace-=3;
                    else {
                        int k=i+1;
                        while (k<lalong && chn[k]!='>') k++;                            
                        if (k<lalong && chn[k-1]!='/')
                            increment=3;
                    }
                    for (j=0;j<espace;j++)
                        doc->texte+=" ";
                    espace+=increment;
                }
                doc->texte+=chn[i];
                if (chn[i]=='>') {                                        
                    j=i+1;
                    while (j<lalong && chn[j]<=32) j++;
                    if (chn[j]!='<') {                
                        doc->texte+="\r\n";
                        for (j=0;j<espace;j++)
                            doc->texte+=" ";
                    }
                }

            }
        }

        

        
        CompteAnalyse++;
    }
    EndWaitCursor();

    doc->UpdateAllViews(NULL);
	
}

void CMainFrame::OnUpdateParsingParsetexttoxml(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);		
}

void CMainFrame::Oncreaterule() 
{
	// TODO: Add your command handler code here
    if (parseurCourant==-1)
        return;
    
    if (XipIsGrammarCrypted(parseurCourant))
        return;

    CString texte;

    CView* vue= DocumentSaisie->GetViewSaisie();
    
      
    vue->GetWindowText(texte);        

    BeginWaitCursor();
    if (texte.GetLength() > 1) {
        texte+="\r\n";
        resxip->sentences.clear();
        XIPLONG opt=ReturnDisplayMode();
        XIPLONG optsave=opt;
        opt&=~DISPLAY_WORDNUM;
		opt&=~DISPLAY_GENERATION;
		optsave|=DISPLAY_DEPENDENCY_NUMBER;
        opt|=DISPLAY_NONE|DISPLAY_OFFSET|DISPLAY_DEPENDENCY_NUMBER|DISPLAY_MATHEMATICS;
        SetDisplayMode(opt);
        
        XipSetRuleLimit(parseurCourant,rule_limit);
		XipSetRuleRankNumberLimit(parseurCourant,-1);
        ostringstream os;
		try {
			XipParseStringAndOS(LPCTSTR(texte),parseurCourant,resxip,&os,XML_NONE,1);     
		}
		catch(XIPError* err) {
			static char errorch[1000];
			sprintf(errorch,"%s\r\n file: %s line:%d",err->message.c_str(),err->filename.c_str(),err->line);
			AfxMessageBox(errorch,MB_OK);
			delete err;
		}


        //XipParseString(LPCTSTR(texte),parseurCourant,resxip); 
        
        //We only take the first sentence...
        if (os.str().size()>0) {
            if (docoutput==NULL) {
                docoutput= (CXipwinDoc*)pDocTemplate->OpenDocumentFile(NULL);        
                docoutput->SetTitle("OUTPUT");
            }
            docoutput->texte= insertwindowsnewline(os.str());

            docoutput->UpdateAllViews(NULL);
        }

		theRuleLimit=-1;
        if (resxip->sentences.size()>=1) {
            CRuleDoc* rg= (CRuleDoc*)pRuleTemplate->OpenDocumentFile(NULL);
            rg->xipsent=resxip;
            rg->sentence=texte;
            rg->UpdateAllViews(NULL);
        }
        //We set back the previous options.
        SetDisplayMode(optsave);
    }
            
	EndWaitCursor();
}

void CMainFrame::OnUpdatecreaterule(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);		
}


//void readTextFromFile(DecompteRegle* dr,DecompteRegle* drnext,int idr,char* buffer_read) {
//                    
//    long gauche=originallabels[idr];
//    char* fichier=dr->nom_fichier;
//    int nbchar=1000;
//
//    if (drnext!=NULL && !strcmp(dr->nom_fichier,drnext->nom_fichier))
//        nbchar=originallabels[idr+1]-gauche;
//    
//    if (nbchar>=10000)
//        nbchar=9999;
//    ifstream ansText(fichier,ios::in|ios::binary);
//            
//    ansText.seekg(gauche);
//    ansText.read(buffer_read, nbchar); 
//    int nb=ansText.gcount();
//    buffer_read[nb] = 0;    
//}
//
//void FormatRuleText(CString& ruletext,int idrule,int breakpoint,BOOL remove_comment) {
//    char buffer[10000];
//
//	if (labels_index==NULL)
//		return;
//
//
//	VECTA<Label*>& labels= *labels_index;
//
//	if (labels[idrule]==NULL || labels[idrule]->dr==NULL)
//        return;
//
//	int index=labels[idrule]->numero;
//
//    if (index==breakpoint+1)
//        ruletext+="\r\n------------------------------------ Execution stops here ------------------------------------\r\n";
//    ruletext+="--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n";
//    ruletext+="In file";
//    ruletext+=" : (";
//	sprintf(buffer,"%d:%d",idrule,index);
//    ruletext+=buffer;
//    ruletext+=") :";
//    ruletext+=(*labels_numero)[index]->dr->nom_fichier;
//    ruletext+="\r\n";
//    if ((*labels_numero)[index+1]!=NULL)
//        readTextFromFile((*labels_numero)[index]->dr,(*labels_numero)[index+1]->dr,index,buffer);
//    else
//        readTextFromFile((*labels_numero)[index]->dr,NULL,index,buffer);
//    int lg=strlen(buffer);
//    
//    int i;
//    
//    for (i=0;i<lg;i++) {
//        if (buffer[i]>32)
//            break;
//    }
//    
//    long firstsel=-1;
//    long lastsel=-1;
//    for (;i<lg;i++) {
//        if (buffer[i]=='/') {
//            if (buffer[i+1]=='/') {
//                while (i<=lg && buffer[i]!=10) i++;
//                while (buffer[i]<=32) i++;
//                i--;
//                continue;
//            }
//            else {
//                while (buffer[i]!='\\') i++;
//                i++;
//                while (buffer[i]<=32) i++;                
//                i--;
//                continue;
//            }
//        }
//        
//        if (buffer[i]==10) {            
//            while (buffer[i]==10) i++;
//            i--;
//        }
//        else {
//            if (firstsel==-1)
//                firstsel=i;
//            lastsel=i;
//        }
//    }
//    //If there is a change of rule in the same file
//    DecompteRegle* dr=(*labels_numero)[index]->dr;    
//    DecompteRegle* drnext=NULL;
//    
//    if ((*labels_numero)[index+1]!=NULL)
//        drnext=(*labels_numero)[index+1]->dr;    
//
//    if (drnext!=NULL && dr->regle!=drnext->regle && 
//        !strcmp(dr->nom_fichier,drnext->nom_fichier)) {
//        //we have to remove the field name
//        while (buffer[lastsel]!=':') lastsel--;
//        while (buffer[lastsel]>32) lastsel--;
//    }
//    else
//        lastsel++;
//        
//    for (i=lastsel;i>=firstsel;i--) {
//        if (buffer[i]>=32) {
//            lastsel=i+1;
//            break;
//        }
//    }
//    for (i=firstsel;i<lastsel;i++) {
//        if (buffer[i]==10)
//          ruletext+="\r";
//        ruletext+=buffer[i];
//    }
//    ruletext+="\r\n";
//}


void FormatRuleText(CString& ruletext,int idrule,int breakpoint,BOOL remove_comment) {
	ostringstream os;
	if (labels_index==NULL || labels_numero==NULL || (*labels_index)[idrule]==NULL)
		return;

	//First we extract the rule number
	int numero=(*labels_index)[idrule]->numero;
	if (numero!=-1) {
		VECTA<Label*>& labels= *labels_index;
		DecompteRegle* dr=labels[numero]->dr;    

		XipDisplayActivatedRules(parseurCourant,&os,idrule, 5);
	}
	else
		ruletext+="Reload the grammar\r\nNew Rules Have Been Added\r\n";

    if (idrule==breakpoint+1)
        ruletext+="------------------------------------ Execution stops here ------------------------------------\r\n";
	ruletext+=os.str().c_str();
}


void CMainFrame::Onrulenumber() 
{
    if (labels_numero->dernier==0)
        return;

    Cseerule diag;
    if (diag.DoModal()==IDOK) {
        int val=diag.m_rulenumber;
        int fin=diag.m_rulenumberlast;
        if ((*labels_numero)[val]==NULL || (*labels_numero)[fin]==NULL)
            return;

        DocumentSaisie->SetModifiedFlag(FALSE);
        
        
        CXipwinDoc* doc= (CXipwinDoc*)pDocTemplate->OpenDocumentFile(NULL);
        
        CString chaine("Rule:");
        char ch[100];
        chaine+= itoa(val,ch,10);
        doc->SetTitle(chaine);

        static char buffer[10000];
        
        for (int j=val;j<=fin;j++)
            FormatRuleText(doc->texte,j,-1,TRUE);

        doc->UpdateAllViews(NULL);
    }    
}


void CMainFrame::OnParsingParsefile() 
{
	// TODO: Add your command handler code here
    CFileDialog textfile(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Text (*.txt)|*.txt|");
    if (textfile.DoModal()==IDCANCEL)
        return;
       
    BeginWaitCursor();
    CString output=textfile.GetPathName();
    output+=".out";
    ofstream* file=new ofstream(output);    
    XipParseFileToFile(LPCTSTR(textfile.GetPathName()),parseurCourant,file);
    delete file;
    CXipwinDoc* fenetreoutput= (CXipwinDoc*)pDocTemplate->OpenDocumentFile(output);            

    EndWaitCursor();
}

void CMainFrame::Onseefiles() 
{
	// TODO: Add your command handler code here
    CFiletree file;
    file.DoModal();


}

void CMainFrame::OnUpdateseefiles(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);		
	
}

void CMainFrame::Oninfo() 
{
	// TODO: Add your command handler code here
    Cinforule info;	
    info.start_rule=0;
    info.m_selection=0;
    info.removecomment=TRUE;
    info.DoModal();
	
}

void CMainFrame::OnUpdateinfo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);		
	
}

void CMainFrame::Onfeaturedisplay() 
{
	// TODO: Add your command handler code here
    arbretrait* arbre=XipGetFeatureTree(parseurCourant);
	CFeaturetree cf;    
    cf.DoModal();

}

void CMainFrame::OnUpdatefeaturedisplay(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);		
	
}

void CMainFrame::Onhelpscripting() 
{
	// TODO: Add your command handler code here
		// TODO: Add your command handler code here
    helpscripting h;
    h.DoModal();

}


void CMainFrame::OnXmlInput() 
{
	// TODO: Add your command handler code here
    choixXML=1;
    input_type=XML_NONE;	
    OnAnalyseParse();	
    choixXML=0;
    input_type=XML_INSERT;	

}

void CMainFrame::OnUpdateXmlInput(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    pCmdUI->Enable(erreurChgt==0);	  	
}

void CMainFrame::OnNtmOnly() 
{
	// TODO: Add your command handler code here
    notntmonly=0;	
    OnAnalyseParse();	
    notntmonly=1;	
}



void CMainFrame::OnUpdateNtmOnly(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    pCmdUI->Enable(erreurChgt==0);		
}

void CMainFrame::OnRuleDebugfrom0()
{
	// TODO: Add your command handler code here
    if (parseurCourant==-1)
        return;
    
    if (XipIsGrammarCrypted(parseurCourant))
        return;

    CString texte;

    CView* vue= DocumentSaisie->GetViewSaisie();
    
      
    vue->GetWindowText(texte);        

    BeginWaitCursor();
    if (texte.GetLength() > 1) {
        texte+="\r\n";
        resxip->sentences.clear();
        XIPLONG opt=ReturnDisplayMode();
        XIPLONG optsave=opt;
        opt&=~DISPLAY_WORDNUM;
		opt&=~DISPLAY_GENERATION;
        opt|=DISPLAY_NONE|DISPLAY_OFFSET|DISPLAY_DEPENDENCY_NUMBER|DISPLAY_MATHEMATICS;
        SetDisplayMode(opt);
        
        XipSetRuleLimit(parseurCourant,0);		
        ostringstream os;
		try {
			XipParseStringAndOS(LPCTSTR(texte),parseurCourant,resxip,&os,XML_NONE,1);     
		}
		catch(XIPError* err) {
			static char errorch[1000];
			sprintf(errorch,"%s\r\n file: %s line:%d",err->message.c_str(),err->filename.c_str(),err->line);
			AfxMessageBox(errorch,MB_OK);
			delete err;
		}


        //XipParseString(LPCTSTR(texte),parseurCourant,resxip); 
        
        //We only take the first sentence...
        if (os.str().size()>0) {
            if (docoutput==NULL) {
                docoutput= (CXipwinDoc*)pDocTemplate->OpenDocumentFile(NULL);        
                docoutput->SetTitle("OUTPUT");
            }
            docoutput->texte= insertwindowsnewline(os.str());

            docoutput->UpdateAllViews(NULL);
        }

		theRuleLimit=0;
        if (resxip->sentences.size()>=1) {
            CRuleDoc* rg= (CRuleDoc*)pRuleTemplate->OpenDocumentFile(NULL);
            rg->xipsent=resxip;
            rg->sentence=texte;
            rg->UpdateAllViews(NULL);
        }
        //We set back the previous options.
        SetDisplayMode(optsave);
    }
            
	EndWaitCursor();
}

void CMainFrame::OnUpdateRuleDebugfrom0(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);		
}

void CMainFrame::OnUpdateButtonplus(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);	
}

void CMainFrame::OnButtonplus()
{
	// TODO: Add your command handler code here
	if (rule_limit!=-1 && rule_limit<labels_numero->dernier-3) {
		rule_limit++;
		XipSetRuleLimit(parseurCourant,rule_limit);
		XipSetRuleRankNumberLimit(parseurCourant,-1);
	}
	OnAnalyseParse();	
}


void CMainFrame::OnButtonmoins()
{
	// TODO: Add your command handler code here
	if (rule_limit>0) {
		rule_limit--;
		XipSetRuleLimit(parseurCourant,rule_limit);
		XipSetRuleRankNumberLimit(parseurCourant,-1);
	}
	OnAnalyseParse();	
}

void CMainFrame::OnUpdateButtonmoins(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);	
}

void CMainFrame::OnReloadAndParse()
{
	// TODO: Add your command handler code here
	OnAnalyzisReloadgrammar();
	if (erreurChgt==0)
		OnAnalyseParse();	
}

void CMainFrame::OnUpdateReloadAndParse(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1);
}

void CMainFrame::OnDependanceAnalyse() {
    //Pour s'éviter le message de sauvegarde...
    DocumentSaisie->SetModifiedFlag(FALSE);
        

    CXipwinDoc* doc= (CXipwinDoc*)pDocTemplate->OpenDocumentFile(NULL);
    
    CString chaine("Result Id:");
    char ch[100];
    chaine+= itoa(CompteAnalyse,ch,10);
    doc->SetTitle(chaine);

    if (parseurCourant==-1)
        return;
    
    CString texte;

    CView* vue= DocumentSaisie->GetViewSaisie();
    
      
    vue->GetWindowText(texte);        

    BeginWaitCursor();
    if (texte.GetLength() > 1) {        
        
        ostringstream os;
        string stexte=LPCTSTR(texte);
        XipSetDisplayMode(parseurCourant,DISPLAY_CHUNK_TREE|DISPLAY_GENERATION|DISPLAY_SURFACE|CHECK_INPUT_UTF8);
		try {
			XipGenerateFromString(stexte,parseurCourant,&os);        
			os<<"\r\n\r\n";                
			doc->texte+=insertwindowsnewline(os.str());
		}
		catch(XIPError* err) {
			static char errorch[1000];
			sprintf(errorch,"%s\r\n file: %s line:%d",err->message.c_str(),err->filename.c_str(),err->line);
			AfxMessageBox(errorch,MB_OK);
			delete err;
		}       
        
        CompteAnalyse++;
    }
    EndWaitCursor();

    doc->UpdateAllViews(NULL);	
}


void CMainFrame::OnUpdateDependanceAnalyse(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);	
}

void CMainFrame::OnUpdateDebugdep(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);	
}

void CMainFrame::OnDebugdep()
{
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
    if (parseurCourant==-1)
        return;
    
    if (XipIsGrammarCrypted(parseurCourant))
        return;

    CString texte;

    CView* vue= DocumentSaisie->GetViewSaisie();
    
      
    vue->GetWindowText(texte);        

    BeginWaitCursor();
    if (texte.GetLength() > 1) {
        texte+="\r\n";
		resxip->sentences.clear();
        XIPLONG opt=ReturnDisplayMode();
        XIPLONG optsave=opt;
        opt&=~DISPLAY_WORDNUM;
        opt|=DISPLAY_CHUNK_TREE;
		opt|=DISPLAY_GENERATION;
		optsave|=DISPLAY_GENERATION;
		optsave|=DISPLAY_DEPENDENCY_NUMBER;
		opt|=DISPLAY_SURFACE|DISPLAY_DEPENDENCY_NUMBER;
        SetDisplayMode(opt);
        
        XipSetRuleLimit(parseurCourant,0);		
        ostringstream os;
		try {
			XipGenerateFromStringXipResult(LPCTSTR(texte),resxip,parseurCourant,&os);
		}
		catch(XIPError* err) {
			static char errorch[1000];
			sprintf(errorch,"%s\r\n file: %s line:%d",err->message.c_str(),err->filename.c_str(),err->line);
			AfxMessageBox(errorch,MB_OK);
			delete err;
		}


        //XipParseString(LPCTSTR(texte),parseurCourant,resxip); 
        
        //We only take the first sentence...
        if (os.str().size()>0) {
            if (docoutput==NULL) {
                docoutput= (CXipwinDoc*)pDocTemplate->OpenDocumentFile(NULL);        
                docoutput->SetTitle("OUTPUT");
            }
            docoutput->texte= texte;

            docoutput->UpdateAllViews(NULL);
        }

		theRuleLimit=0;
        if (resxip->sentences.size()>=1) {
            CRuleDoc* rg= (CRuleDoc*)pRuleTemplate->OpenDocumentFile(NULL);
            rg->xipsent=resxip;
            rg->sentence=texte;
            rg->UpdateAllViews(NULL);
        }
        //We set back the previous options.
        SetDisplayMode(optsave);
    }
            
	EndWaitCursor();
}
