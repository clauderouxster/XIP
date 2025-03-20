// xipwinView.cpp : implementation of the CXipwinView class
//

#include "stdafx.h"
#include "xipwin.h"

#define pasofficielxip_variable

#include "pasofficiel.h"
#include <strstream>
#include "xipwinDoc.h"
#include "xipwinView.h"
#include "optionsXip.h"
#include "xipvecta.h"
#include "limit.h"
#include "Filetree.h"
#include "inforule.h"
#include "Featuretree.h"
#include "helpscripting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int iAnalysePhrase;
extern char ntmFileName[1000];
extern char hmmFileName[1000];

extern CString pathNameIndent;
extern int CompteAnalyse;
extern VECTA<char*> parameterfiles;
extern int parseurCourant;
extern char erreurChgt;
extern CString grammarFileName[10];

BOOL g_categories = FALSE;
BOOL g_full = FALSE;
BOOL g_lemme = FALSE;
BOOL g_offset = FALSE;
BOOL g_reduit = TRUE;
BOOL g_sentence = FALSE;
BOOL g_surface = TRUE;
BOOL g_utf8 = FALSE;
BOOL g_wordnumb = FALSE;
BOOL g_obname=TRUE;
BOOL g_obnode=FALSE;
BOOL g_obcreation=FALSE;
BOOL g_maths=FALSE;
BOOL g_rulenumber=FALSE;
BOOL g_inpututf8=FALSE;
BOOL g_detectlanguage=FALSE;
BOOL g_debugmode=FALSE;
BOOL g_generation=FALSE;
BOOL g_display_feature_dependency=FALSE;
char g_dependency=1;
extern int nb_language;

/////////////////////////////////////////////////////////////////////////////
// CXipwinView

IMPLEMENT_DYNCREATE(CXipwinView, CEditView)

BEGIN_MESSAGE_MAP(CXipwinView, CEditView)
	//{{AFX_MSG_MAP(CXipwinView)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_EDIT_OPTIONS, OnEditOptions)
	ON_UPDATE_COMMAND_UI(ID_EDIT_OPTIONS, OnUpdateEditOptions)
	ON_COMMAND(ID_setlimit, Onsetlimit)
	ON_COMMAND(IDC_seefiles, Onseefiles)
	ON_UPDATE_COMMAND_UI(IDC_seefiles, OnUpdateseefiles)
	ON_COMMAND(IDC_info, Oninfo)
	ON_UPDATE_COMMAND_UI(IDC_info, OnUpdateinfo)
	ON_COMMAND(IDC_featuredisplay, Onfeaturedisplay)
	ON_UPDATE_COMMAND_UI(IDC_featuredisplay, OnUpdatefeaturedisplay)
	ON_COMMAND(ID_loadanothergrammar, Onloadanothergrammar)
	ON_UPDATE_COMMAND_UI(ID_loadanothergrammar, OnUpdateloadanothergrammar)
	ON_COMMAND(ID_VIEW_GRAMMAR1, OnViewGrammar1)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRAMMAR1, OnUpdateViewGrammar1)
	ON_COMMAND(ID_VIEW_GRAMMAR2, OnViewGrammar2)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRAMMAR2, OnUpdateViewGrammar2)
	ON_COMMAND(ID_VIEW_GRAMMAR3, OnViewGrammar3)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRAMMAR3, OnUpdateViewGrammar3)
	ON_COMMAND(ID_VIEW_GRAMMAR4, OnViewGrammar4)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRAMMAR4, OnUpdateViewGrammar4)
	ON_COMMAND(id_help_scripting, Onhelpscripting)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXipwinView construction/destruction

CXipwinView::CXipwinView()
{
	// TODO: add construction code here    
}

CXipwinView::~CXipwinView()
{
}


BOOL CXipwinView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

    m_dwDefaultStyle = dwStyleDefault;
    m_dwDefaultStyle &= ~ES_AUTOHSCROLL;
    m_dwDefaultStyle &= ~WS_HSCROLL;
    return CCtrlView::PreCreateWindow(cs);

	//return CEditView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CXipwinView drawing

void CXipwinView::OnDraw(CDC* pDC)
{
	CXipwinDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CXipwinView printing

BOOL CXipwinView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CXipwinView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CXipwinView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CXipwinView diagnostics

#ifdef _DEBUG
void CXipwinView::AssertValid() const
{
	CEditView::AssertValid();
}

void CXipwinView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CXipwinDoc* CXipwinView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CXipwinDoc)));
	return (CXipwinDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXipwinView message handlers


void CXipwinView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
    CXipwinDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    SetWindowText(pDoc->texte);
    
    CEditView::OnUpdate(pSender,lHint,pHint);		
}

void CXipwinView::OnFileClose() 
{
	// TODO: Add your command handler code here
	
}

XIPLONG ReturnDisplayMode() {
    XIPLONG opt=0;
    if (g_categories)
        opt|=DISPLAY_CATEGORY;
    if (g_full)
        opt|=DISPLAY_FULL;
    if (g_lemme)
        opt|=DISPLAY_LEMMA;
    if (g_offset)
        opt|=DISPLAY_OFFSET;
    if (g_reduit)
        opt|=DISPLAY_REDUCED;
    if (g_sentence)
        opt|=DISPLAY_SENTENCE;
    if (g_surface)
        opt|=DISPLAY_SURFACE;
    if (g_wordnumb)
        opt|=DISPLAY_WORDNUM;
    if (g_obname)
        opt|=DISPLAY_DEPENDENCY_BY_NAME;
    if (g_obnode)
        opt|=DISPLAY_DEPENDENCY_BY_NODE;
    if (g_obcreation)
        opt|=DISPLAY_DEPENDENCY_BY_CREATION;
    if (g_utf8)
        opt|=DISPLAY_CONVERSION_UTF8;    
    if (g_maths)
        opt|=DISPLAY_MATHEMATICS;
    if (g_rulenumber)
        opt|=DISPLAY_DEPENDENCY_NUMBER;
    if (g_inpututf8)
        opt|=UTF8_INPUT;
    if (g_detectlanguage)
        opt|=LANGUAGE_GUESSER;
    if (g_debugmode)
        opt|=DISPLAY_EXECUTION_ERROR;
	if (g_generation)
		opt|=DISPLAY_GENERATION;
	if (g_display_feature_dependency)
		opt|=DISPLAY_DEPENDENCY_FEATURE_VALUE;
    opt|=DISPLAY_CHUNK_TREE;
    return opt;
}

void SetDisplayMode(XIPLONG opt) {    
    
    if ((opt & DISPLAY_CATEGORY) == DISPLAY_CATEGORY)
        g_categories=TRUE;
    else
        g_categories=FALSE;
    
    if ((opt & DISPLAY_FULL) == DISPLAY_FULL)
        g_full=TRUE;
    else
        g_full=FALSE;
    
    if ((opt & DISPLAY_LEMMA) == DISPLAY_LEMMA)
        g_lemme=TRUE;
    else
        g_lemme=FALSE;
    
    if ((opt & DISPLAY_OFFSET) == DISPLAY_OFFSET)
        g_offset=TRUE;
    else
        g_offset=FALSE;
    
    if ((opt & DISPLAY_REDUCED) == DISPLAY_REDUCED)
        g_reduit=TRUE;
    else
        g_reduit=FALSE;
    
    if ((opt & DISPLAY_SENTENCE) == DISPLAY_SENTENCE)
        g_sentence=TRUE;
    else
        g_sentence=FALSE;
    
    if ((opt & DISPLAY_SURFACE) == DISPLAY_SURFACE)
        g_surface=TRUE;
    else
        g_surface=FALSE;
    
    if ((opt & DISPLAY_WORDNUM) == DISPLAY_WORDNUM)
        g_wordnumb=TRUE;
    else
        g_wordnumb=FALSE;
    
    if ((opt & DISPLAY_DEPENDENCY_BY_NAME) == DISPLAY_DEPENDENCY_BY_NAME)
        g_obname=TRUE;
    else
        g_obname=FALSE;
    
    if ((opt & DISPLAY_DEPENDENCY_BY_NODE) == DISPLAY_DEPENDENCY_BY_NODE)
        g_obnode=TRUE;
    else
        g_obnode=FALSE;
    
    if ((opt & DISPLAY_DEPENDENCY_BY_CREATION) == DISPLAY_DEPENDENCY_BY_CREATION)
        g_obcreation=TRUE;
    else
        g_obcreation=FALSE;
    
    if ((opt & DISPLAY_CONVERSION_UTF8) == DISPLAY_CONVERSION_UTF8)
        g_utf8=TRUE;
    else
        g_utf8=FALSE;
    
    if ((opt & DISPLAY_MATHEMATICS) == DISPLAY_MATHEMATICS)
        g_maths=TRUE;
    else
        g_maths=FALSE;
    
    if ((opt & DISPLAY_DEPENDENCY_NUMBER) == DISPLAY_DEPENDENCY_NUMBER)
        g_rulenumber=TRUE;
    else
        g_rulenumber=FALSE;
    
    if ((opt & UTF8_INPUT) == UTF8_INPUT)
        g_inpututf8=TRUE;
    else
        g_inpututf8=FALSE;

    if ((opt & LANGUAGE_GUESSER) == LANGUAGE_GUESSER)
        g_detectlanguage=TRUE;
    else
        g_detectlanguage=FALSE;

	if ((opt & DISPLAY_EXECUTION_ERROR) == DISPLAY_EXECUTION_ERROR)
        g_debugmode=TRUE;
    else
        g_debugmode=FALSE;

	if ((opt&DISPLAY_GENERATION)==DISPLAY_GENERATION)
		g_generation=TRUE;
	else
		g_generation=FALSE;

	if ((opt&DISPLAY_DEPENDENCY_FEATURE_VALUE)==DISPLAY_DEPENDENCY_FEATURE_VALUE)
		g_display_feature_dependency=TRUE;
	else
		g_display_feature_dependency=FALSE;
    
	XipSetDependencyExtraction(parseurCourant,g_dependency);
    XipSetDisplayMode(parseurCourant, opt);
}


void UpdateDisplayMode() {
    XIPLONG opt=0;
    if (g_categories)
        opt|=DISPLAY_CATEGORY;
    if (g_full)
        opt|=DISPLAY_FULL;
    if (g_lemme)
        opt|=DISPLAY_LEMMA;
    if (g_offset)
        opt|=DISPLAY_OFFSET;
    if (g_reduit)
        opt|=DISPLAY_REDUCED;
    if (g_sentence)
        opt|=DISPLAY_SENTENCE;
    if (g_surface)
        opt|=DISPLAY_SURFACE;
    if (g_wordnumb)
        opt|=DISPLAY_WORDNUM;
    if (g_obname)
        opt|=DISPLAY_DEPENDENCY_BY_NAME;
    if (g_obnode)
        opt|=DISPLAY_DEPENDENCY_BY_NODE;
    if (g_obcreation)
        opt|=DISPLAY_DEPENDENCY_BY_CREATION;
    if (g_utf8)
        opt|=DISPLAY_CONVERSION_UTF8;    
    if (g_maths)
        opt|=DISPLAY_MATHEMATICS;
    if (g_rulenumber)
        opt|=DISPLAY_DEPENDENCY_NUMBER;
    if (g_inpututf8)
        opt|=UTF8_INPUT;
    if (g_detectlanguage)
        opt|=LANGUAGE_GUESSER;
    if (g_debugmode)
        opt|=DISPLAY_EXECUTION_ERROR;
	if (g_display_feature_dependency)
		 opt|=DISPLAY_DEPENDENCY_FEATURE_VALUE;
    
    opt|=DISPLAY_CHUNK_TREE;
    XipSetDependencyExtraction(parseurCourant,g_dependency);
    XipSetDisplayMode(parseurCourant, opt);
}

void CXipwinView::OnEditOptions() 
{
	// TODO: Add your command handler code here
    CoptionsXip options;
	options.m_categories = g_categories;
	options.m_full = g_full;
	options.m_lemme = g_lemme;
	options.m_offset = g_offset;
	options.m_reduit = g_reduit;
	options.m_sentence = g_sentence;
	options.m_surface = g_surface;
    options.m_wordnumb = g_wordnumb;
    options.m_obname=g_obname;
    options.m_obnode=g_obnode;
    options.m_obcreation=g_obcreation;
    options.m_maths=g_maths;
    options.m_rulenumber=g_rulenumber;
    options.m_inpututf8=g_inpututf8;
    options.m_detectlanguage=g_detectlanguage;
	options.m_debugmode=g_debugmode;
	options.display_feature_value=g_display_feature_dependency;

    if (g_dependency==1)
        options.m_dependency=TRUE;
    else
        options.m_dependency=FALSE;
    options.m_utf8=g_utf8;
    
    
    if (options.DoModal()==IDOK) {        
        g_categories = options.m_categories;
        g_full = options.m_full;
        g_lemme = options.m_lemme;
        g_offset = options.m_offset;
        g_reduit = options.m_reduit;
        g_sentence = options.m_sentence;
        g_surface = options.m_surface;
        g_utf8 = options.m_utf8;
        g_wordnumb = options.m_wordnumb;
        g_obname=options.m_obname;
        g_obnode=options.m_obnode;
        g_obcreation=options.m_obcreation;
        g_maths=options.m_maths;
        g_rulenumber=options.m_rulenumber;
        g_inpututf8=options.m_inpututf8;
        g_detectlanguage=options.m_detectlanguage;
		g_debugmode=options.m_debugmode;
        g_display_feature_dependency=options.display_feature_value;

        if (options.m_dependency==TRUE)
            g_dependency=1;
        else
            g_dependency=0;
        UpdateDisplayMode();
    }
}

void CXipwinView::OnUpdateEditOptions(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
    pCmdUI->Enable(erreurChgt==0);
}



void CXipwinView::Onsetlimit() 
{
	// TODO: Add your command handler code here
    Climit diag;
    if (diag.DoModal()==IDOK) {
        int val=diag.m_limit;
        XipSetRuleLimit(parseurCourant,val);
    }
}

void CXipwinView::Onseefiles() 
{
	// TODO: Add your command handler code here
    CFiletree file;
    file.DoModal();

}

void CXipwinView::OnUpdateseefiles(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);		
}

void CXipwinView::Oninfo() 
{
	// TODO: Add your command handler code here
    Cinforule info;	
    info.start_rule=0;
    info.m_selection=0;
    info.removecomment=TRUE;
    info.DoModal();

}

void CXipwinView::OnUpdateinfo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);	
}

void CXipwinView::Onfeaturedisplay() 
{
	// TODO: Add your command handler code here
    arbretrait* arbre=XipGetFeatureTree(parseurCourant);
	CFeaturetree cf;    
    cf.DoModal();
	
}

void CXipwinView::OnUpdatefeaturedisplay(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(erreurChgt==0);	
}

void CXipwinView::Onloadanothergrammar() 
{
    if (nb_language>=4) {
        AfxMessageBox("Maximum grammars reached",MB_OK);
        return;
    }
	// TODO: Add your command handler code here
    //we load another grammar, which is kept in memory with no handler
    CFileDialog scriptgrm(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Grammar (*.grm)|*.grm|");
    char grammar[1000];
    
    if (scriptgrm.DoModal()==IDCANCEL)
        return;

    //We now test if this grammar has already been loaded
    for (int i=0;i<nb_language;i++) {
        if (grammarFileName[i]==scriptgrm.GetPathName()) {
            AfxMessageBox("This grammar has already been loaded in memory");
            return;
        }
    }

    try {            
        strcpy(grammar,(char*)(const char*)scriptgrm.GetPathName());        
        BeginWaitCursor();
        int p=XipGrmFile(grammar);
        grammarFileName[p]=grammar;
        nb_language++;
        EndWaitCursor();
    }
    catch (XIPError* message) {
        char errorch[1000];
        sprintf(errorch,"%s\r\n file: %s line:%d",message->message,message->filename,message->line);
        AfxMessageBox(errorch,MB_OK);
        delete message;
    }
}

void CXipwinView::OnUpdateloadanothergrammar(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    pCmdUI->Enable(erreurChgt==0);	
	
}

void CXipwinView::OnViewGrammar1() 
{
	// TODO: Add your command handler code here
    parseurCourant=0;	
    UpdateDisplayMode();
}

void CXipwinView::OnUpdateViewGrammar1(CCmdUI* pCmdUI) 
{
    // TODO: Add your command update UI handler code here
    if (XipExistGrammar(0)==1) {
        pCmdUI->SetText(grammarFileName[0]);
        pCmdUI->Enable(TRUE);		    
        if (parseurCourant==0)
            pCmdUI->SetCheck(TRUE);        
        else
            pCmdUI->SetCheck(FALSE);
    }
    else
        pCmdUI->Enable(FALSE);		
    
    
}


void CXipwinView::OnViewGrammar2() 
{
	// TODO: Add your command handler code here
	parseurCourant=1;
    UpdateDisplayMode();
}

void CXipwinView::OnUpdateViewGrammar2(CCmdUI* pCmdUI) 
{
    // TODO: Add your command update UI handler code here
    if (XipExistGrammar(1)==1) {
        pCmdUI->SetText(grammarFileName[1]);
        pCmdUI->Enable(TRUE);		    
        if (parseurCourant==1)
            pCmdUI->SetCheck(TRUE);        
        else
            pCmdUI->SetCheck(FALSE);
    }
    else
        pCmdUI->Enable(FALSE);		
    
}

void CXipwinView::OnViewGrammar3() 
{
	// TODO: Add your command handler code here
	parseurCourant=2;
    UpdateDisplayMode();
}

void CXipwinView::OnUpdateViewGrammar3(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    if (XipExistGrammar(2)==1) {
        pCmdUI->SetText(grammarFileName[2]);
        pCmdUI->Enable(TRUE);	
        if (parseurCourant==2)
            pCmdUI->SetCheck(TRUE);        
        else
            pCmdUI->SetCheck(FALSE);        
    }
    else
        pCmdUI->Enable(FALSE);		
    
}

void CXipwinView::OnViewGrammar4() 
{
	// TODO: Add your command handler code here
    parseurCourant=3;
    UpdateDisplayMode();

}

void CXipwinView::OnUpdateViewGrammar4(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	// TODO: Add your command update UI handler code here
    if (XipExistGrammar(3)==1) {
        pCmdUI->SetText(grammarFileName[3]);
        pCmdUI->Enable(TRUE);	
        if (parseurCourant==3)
            pCmdUI->SetCheck(TRUE);        
        else
            pCmdUI->SetCheck(FALSE);        
    }
    else
        pCmdUI->Enable(FALSE);			
}

void CXipwinView::Onhelpscripting() 
{
    helpscripting h;
	// TODO: Add your command handler code here
    h.DoModal();
}
