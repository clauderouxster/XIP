// optionsXip.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#include "optionsXip.h"
#include ".\optionsxip.h"
#define XIPLIBXML
#include "pasofficiel.h"
extern int rule_limit;
extern int parseurCourant;
extern CString grammarFileName[10];

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char nouvel_algo;
extern BOOL python_debug_mode;
void SetPythonDebugMode(char test,int module,int ligne);
int python_module=0;
int python_ligne=0;
extern BOOL output_activated;

void XipInsertionDifference(int test,int val);
Exportation void XipSetDisplayMultiWordString(char v);

/////////////////////////////////////////////////////////////////////////////
// CoptionsXip dialog



CoptionsXip::CoptionsXip(CWnd* pParent /*=NULL*/)
	: CDialog(CoptionsXip::IDD, pParent)
	, upto(rule_limit)
	, m_debugmode(false)
	, pythondebugmode(python_debug_mode)
	, modulepython(python_module)
	, lignepython(python_ligne)
	, edittest(-1)
	, edittestvaleur(0)
	, output_enable(FALSE)
	, select_algo(FALSE)
	, display_feature_value(FALSE)
	, MULTIWRDDISPLAY(FALSE)
{
	//{{AFX_DATA_INIT(CoptionsXip)
	m_categories = FALSE;
	m_full = FALSE;
	m_lemme = FALSE;
	m_offset = TRUE;
	m_reduit = FALSE;
	m_sentence = FALSE;
	m_surface = FALSE;
	m_wordnumb = FALSE;
	m_utf8 = FALSE;
	m_dependency = TRUE;
	m_maths = FALSE;
	m_rulenumber = TRUE;
	m_inpututf8 = FALSE;
	m_detectlanguage = FALSE;
	//}}AFX_DATA_INIT
    m_obname= FALSE;
    m_obnode= FALSE;
    m_obcreation= FALSE;
	char x=XipGetTaggerMode(parseurCourant);
	if (x==1)
		tagger_mode=TRUE;
	else
		tagger_mode=FALSE;    
	output_enable=TRUE;
}


void CoptionsXip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CoptionsXip)
	DDX_Check(pDX, IDC_cetegories, m_categories);
	DDX_Check(pDX, IDC_full, m_full);
	DDX_Check(pDX, IDC_lemme, m_lemme);
	DDX_Check(pDX, IDC_offset, m_offset);
	DDX_Check(pDX, IDC_reduit, m_reduit);
	DDX_Check(pDX, IDC_sentence, m_sentence);
	DDX_Check(pDX, IDC_surface, m_surface);
	DDX_Check(pDX, IDC_wordnumb, m_wordnumb);
	DDX_Check(pDX, IDC_utf8, m_utf8);
	DDX_Check(pDX, IDC_dependency, m_dependency);
	DDX_Check(pDX, IDC_maths, m_maths);
	DDX_Check(pDX, IDC_rulenumber, m_rulenumber);
	DDX_Check(pDX, IDC_inpututf8, m_inpututf8);
	DDX_Check(pDX, IDC_detectlanguage, m_detectlanguage);
	DDX_Check(pDX, IDC_debugmode, m_debugmode);
	//}}AFX_DATA_MAP

	if (m_obcreation)
		CheckRadioButton(IDC_byname,IDC_bycreation,IDC_bycreation);
	else
		if (m_obnode)
			CheckRadioButton(IDC_byname,IDC_bycreation,IDC_bynode);
		else
			CheckRadioButton(IDC_byname,IDC_bycreation,IDC_byname);
	DDX_Text(pDX, IDC_EDIT1, upto);
	DDX_Check(pDX, IDC_pythondebug, pythondebugmode);
	DDX_Text(pDX, IDC_modulepython, modulepython);
	DDX_Text(pDX, IDC_pythonline, lignepython);
	DDX_Check(pDX, IDC_tagger_mode, tagger_mode);
	DDX_Text(pDX, IDC_EDITTEST, edittest);
	DDV_MinMaxInt(pDX, edittest, -1, 99);
	DDX_Text(pDX, IDC_EDITTESTVALEUR, edittestvaleur);
	DDX_Check(pDX, IDC_tagger_mode2, output_enable);
	DDX_Check(pDX, IDC_CHECK2, select_algo);
	DDX_Check(pDX, IDC_displayfeaturevaluedep, display_feature_value);
	DDX_Check(pDX, IDC_MULTWRDDISPLAY, MULTIWRDDISPLAY);
}


BEGIN_MESSAGE_MAP(CoptionsXip, CDialog)
	//{{AFX_MSG_MAP(CoptionsXip)
	ON_BN_CLICKED(IDC_bycreation, Onbycreation)
	ON_BN_CLICKED(IDC_byname, Onbyname)
	ON_BN_CLICKED(IDC_bynode, Onbynode)
	ON_BN_CLICKED(IDC_lemme, Onlemme)
	ON_BN_CLICKED(IDC_surface, Onsurface)
	ON_BN_CLICKED(IDC_offset, Onoffset)
	ON_BN_CLICKED(IDC_wordnumb, Onwordnumb)
	ON_BN_CLICKED(IDC_full, Onfull)
	ON_BN_CLICKED(IDC_reduit, Onreduit)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK2, &CoptionsXip::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_displayfeaturevaluedep, &CoptionsXip::OnBnClickeddisplayfeaturevaluedep)
	ON_BN_CLICKED(IDC_MULTWRDDISPLAY, &CoptionsXip::OnBnClickedMultwrddisplay)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CoptionsXip message handlers

void CoptionsXip::Onbycreation() 
{
	// TODO: Add your control notification handler code here
    m_obcreation=TRUE;
    m_obnode=FALSE;
    m_obname=FALSE;    
	CheckRadioButton(IDC_byname,IDC_bycreation,IDC_bycreation);
}

void CoptionsXip::Onbyname() 
{
	// TODO: Add your control notification handler code here
    m_obcreation=FALSE;
    m_obnode=FALSE;
    m_obname=TRUE;    
	CheckRadioButton(IDC_byname,IDC_bycreation,IDC_byname);
}

void CoptionsXip::Onbynode() 
{
	// TODO: Add your control notification handler code here
    m_obcreation=FALSE;
    m_obnode=TRUE;
    m_obname=FALSE;    
    CheckRadioButton(IDC_byname,IDC_bycreation,IDC_bynode);
	
}


void CoptionsXip::Onlemme() 
{
    // TODO: Add your control notification handler code here
    CButton* b= (CButton*)GetDlgItem(IDC_lemme);
    if (b->GetCheck()==TRUE) {
        CButton* bs= (CButton*)GetDlgItem(IDC_surface);
        CButton* r= (CButton*)GetDlgItem(IDC_reduit);
        CButton* bl= (CButton*)GetDlgItem(IDC_full);
        bl->SetCheck(FALSE);
        bs->SetCheck(FALSE);
        r->SetCheck(TRUE);        
    }
    else
        b->SetCheck(TRUE);

}

void CoptionsXip::Onsurface() 
{
    // TODO: Add your control notification handler code here
    CButton* b= (CButton*)GetDlgItem(IDC_surface);
    if (b->GetCheck()==TRUE) {
        CButton* bl= (CButton*)GetDlgItem(IDC_lemme);
        bl->SetCheck(FALSE);
    }
    else
        b->SetCheck(TRUE);

}

void CoptionsXip::Onoffset() 
{
	// TODO: Add your control notification handler code here
	CButton* b= (CButton*)GetDlgItem(IDC_offset);
    if (b->GetCheck()==TRUE) {
        CButton* bl= (CButton*)GetDlgItem(IDC_wordnumb);
        bl->SetCheck(FALSE);
    }
}

void CoptionsXip::Onwordnumb() 
{
	// TODO: Add your control notification handler code here
	CButton* b= (CButton*)GetDlgItem(IDC_wordnumb);
    if (b->GetCheck()==TRUE) {
        CButton* bl= (CButton*)GetDlgItem(IDC_offset);
        bl->SetCheck(FALSE);
    }
	
}

void CoptionsXip::Onfull() 
{
	// TODO: Add your control notification handler code here
    CButton* b= (CButton*)GetDlgItem(IDC_full);
    if (b->GetCheck()==TRUE) {
        CButton* bl= (CButton*)GetDlgItem(IDC_reduit);
        bl->SetCheck(FALSE);
        bl= (CButton*)GetDlgItem(IDC_lemme);
        bl->SetCheck(FALSE);
        bl= (CButton*)GetDlgItem(IDC_surface);
        bl->SetCheck(TRUE);
    }
    else
        b->SetCheck(TRUE);
}

void CoptionsXip::Onreduit() 
{
    CButton* bl;
	// TODO: Add your control notification handler code here
	CButton* b= (CButton*)GetDlgItem(IDC_reduit);
    if (b->GetCheck()==TRUE) {
        bl= (CButton*)GetDlgItem(IDC_full);
        bl->SetCheck(FALSE);
    }
    else
        b->SetCheck(TRUE);    
}



void CoptionsXip::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	rule_limit=upto;
	char test=1;
	if (pythondebugmode==FALSE)
		test=0;
	if (python_debug_mode!=pythondebugmode || modulepython!=python_module || python_ligne!=lignepython) {
		BeginWaitCursor();
		SetPythonDebugMode(test,modulepython,lignepython);
		EndWaitCursor();        
	}

	python_debug_mode=pythondebugmode;
	python_ligne=lignepython;
	python_module=modulepython;
	output_activated=output_enable;
	if (tagger_mode==TRUE)
		XipSetTaggerMode(parseurCourant,1);
	else
		XipSetTaggerMode(parseurCourant,0);
	if (edittest!=-1)
		XipInsertionDifference(edittest,edittestvaleur);
	OnOK();
}



void CoptionsXip::OnBnClickedCheck2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (select_algo==TRUE)
		nouvel_algo=1;
	else
		nouvel_algo=0;
}

void CoptionsXip::OnBnClickeddisplayfeaturevaluedep()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CoptionsXip::OnBnClickedMultwrddisplay()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (MULTIWRDDISPLAY)
		XipSetDisplayMultiWordString(1);
	else
		XipSetDisplayMultiWordString(0);
}
