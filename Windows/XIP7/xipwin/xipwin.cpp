// xipwin.cpp : Defines the class behaviors for the application.
//


#include "stdafx.h"

#include "pasofficiel.h"

#include "xipwin.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "xipwinDoc.h"
#include "xipwinView.h"
#include "resdoc.h"
#include "resultat.h"

#include <iostream>

#include "xipvecta.h"
#include "RuleBuilding.h"
#include "ruledoc.h"
#include "saisiedoc.h"
#include "saisieview.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL output_activated;
CMultiDocTemplate* pGrTemplate;
CMultiDocTemplate* pDocTemplate;
CMultiDocTemplate* pRuleTemplate;
CMultiDocTemplate* pEditTemplate;
/////////////////////////////////////////////////////////////////////////////
// CXipwinApp

BEGIN_MESSAGE_MAP(CXipwinApp, CWinApp)
	//{{AFX_MSG_MAP(CXipwinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXipwinApp construction

CXipwinApp::CXipwinApp(LPCTSTR lpszAppName) : CWinApp(lpszAppName)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXipwinApp object

CXipwinApp theAppXIPWIN("XIPWIN1.0");

/////////////////////////////////////////////////////////////////////////////
// CXipwinApp initialization

BOOL CXipwinApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("XIP DEMO"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	
	pDocTemplate = new CMultiDocTemplate(
		IDR_XIPWINTYPE,
		RUNTIME_CLASS(CXipwinDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CXipwinView));
	AddDocTemplate(pDocTemplate);
    

    pGrTemplate = new CMultiDocTemplate(
        IDR_RESTYPE,
        RUNTIME_CLASS(CResdoc),
        RUNTIME_CLASS(CMDIChildWnd), 
        RUNTIME_CLASS(Cresultat));
    AddDocTemplate(pGrTemplate);

    pRuleTemplate = new CMultiDocTemplate(
        IDR_RESTYPE,
        RUNTIME_CLASS(CRuleDoc),
        RUNTIME_CLASS(CMDIChildWnd), 
        RUNTIME_CLASS(RuleBuilding));
    AddDocTemplate(pRuleTemplate);

    pEditTemplate = new CMultiDocTemplate(
        IDR_RESTYPE,
        RUNTIME_CLASS(CSaisieDoc),
        RUNTIME_CLASS(CMDIChildWnd), // custom MDI child frame
        RUNTIME_CLASS(CSaisieView));
    AddDocTemplate(pEditTemplate);


	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}


void Usage()
{
    cerr<<"Usage:" <<endl;
    cerr<<"     -ntm ntmscript"<<endl;
    cerr<<"     -l (parameter file for a given language)"<<endl;
    cerr<<"     -hmm (hmm filename)"<<endl;
}

extern CString grammarFileName[10];
extern int parseurCourant;
CString grammarPathInput;
int ChargerGrammaire(CString&,char);
int nb_language=0;


void CXipwinApp::ParseCommandLine(CCommandLineInfo& rCmdInfo) {
	output_activated=TRUE;
    
    BeginWaitCursor();
    
	if (__argc>1) {        
		char gram[1000];
		for (int ix=0;ix<__argc-1;ix++) {
			strcpy(gram,__targv[ix+1]);        
			grammarFileName[ix]=gram;
			int l=strlen(gram)-1;
			while (gram[l]!='\\') l--;
			gram[l]=0;        
			grammarPathInput=gram;
			grammarPathInput+="\\input.txt";
			SetCurrentDirectory(gram);
			gram[l]='\\';
			int tempsRestant=ChargerGrammaire(grammarFileName[ix],1);		
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
			nb_language++;
		}
	}
    
    EndWaitCursor();

}


void CXipwinApp::Creation() {
    OnFileNew();
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    static string question;
    Whoami(question);
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
    CWnd* message=GetDlgItem(IDC_ntmversion);
    char str[1000];
    XipNtmVersion(str);
    message->SetWindowText(str);
    message=GetDlgItem(IDC_whoami);
    string strbis;
    Whoami(strbis);
#ifdef ARCH64
	strcpy(str," 64bits");
	strbis+=str;
#endif
    message->SetWindowText(strbis.c_str());
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CXipwinApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CXipwinApp message handlers


