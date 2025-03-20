// debogueur.cpp : implementation file
//

#include "stdafx.h"
#include "kif.h"
extern "C" {
#include "kifextern.h"
}
#include "KIFF.h"
#include "debogueur.h"

// debogueur dialog

#define WCSTR(x) (wchar_t*)LPCWSTR(x)

IMPLEMENT_DYNAMIC(debogueur, CDialogEx)

	debogueur::debogueur(CWnd* pParent /*=NULL*/)
	: CDialogEx(debogueur::IDD, pParent)
	, vvariables(_T(""))
	, vpile(_T(""))
	, vsortie(_T(""))
	, choixvariable(_T(""))
	, localvariables("")
	, locals(_T(""))
	, threadid(0)
	, stickthread(FALSE)
{
	currentview=NULL;
}

debogueur::~debogueur()
{
}

void debogueur::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, vvariables);
	DDX_Text(pDX, IDC_EDIT2, vpile);
	DDX_Control(pDX, IDSUIVANT, bsuivant);
	DDX_Control(pDX, IDSAUTE, bsaute);
	DDX_Control(pDX, IDSORTIR, bsortir);
	DDX_Control(pDX, IDPOINTARRET, bbreak);
	DDX_Control(pDX, IDARRETPRG, barret);
	DDX_Control(pDX, IDFIN, bfin);
	DDX_Text(pDX, IDC_EDIT5, vsortie);
	DDX_Text(pDX, IDC_EDIT4, choixvariable);
	DDX_Text(pDX, IDC_CURRENT_VARIABLE, locals);
	DDX_Control(pDX, IDC_EDIT4, localvar);
	DDX_Control(pDX, IDENTRE, bentre);
	DDX_Text(pDX, IDC_EDIT3, threadid);
	DDX_Check(pDX, IDC_CHECK1, stickthread);
}


BEGIN_MESSAGE_MAP(debogueur, CDialogEx)
	ON_BN_CLICKED(IDSAUTE, &debogueur::OnBnClickedSaute)
	ON_BN_CLICKED(IDSORTIR, &debogueur::OnBnClickedSortir)
	ON_BN_CLICKED(IDFIN, &debogueur::OnBnClickedFin)
	ON_BN_CLICKED(IDSUIVANT, &debogueur::OnBnClickedSuivant)
	ON_BN_CLICKED(IDARRETPRG, &debogueur::OnBnClickedArretprg)
	ON_BN_CLICKED(IDPOINTARRET, &debogueur::OnBnClickedPointarret)
	ON_EN_CHANGE(IDC_EDIT1, &debogueur::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT4, &debogueur::OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, &debogueur::OnEnChangeEdit5)
	ON_MESSAGE(MET_A_JOUR, &debogueur::MAJ)
	ON_BN_CLICKED(IDENTRE, &debogueur::OnBnClickedEntre)
	ON_BN_CLICKED(IDC_CHECK1, &debogueur::OnBnClickedStickToThread)
	ON_EN_CHANGE(IDC_EDIT3, &debogueur::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_CURRENT_VARIABLE, &debogueur::OnEnChangeCurrentVariable)
END_MESSAGE_MAP()


// debogueur message handlers

LRESULT debogueur::MAJ(WPARAM wParam, LPARAM lParam) {	
	UpdateData(FALSE);
	//::SendMessage((HWND)wParam,MET_A_JOUR,(WPARAM)lignecourante,NULL);
	return 1;
}


BOOL debogueur::OnInitDialog() {
	BOOL v=CDialogEx::OnInitDialog();
	HICON  hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_SUIVANT),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);
	bsuivant.SetIcon(hIcn);
	
	hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_SAUTE),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);	
	bsaute.SetIcon(hIcn);
	
	hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_SORTIR),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);
	bsortir.SetIcon(hIcn);

	
	hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_ENTRE),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);	
	bentre.SetIcon(hIcn);

	hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_BREAK),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);
	bbreak.SetIcon(hIcn);

	hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_GO),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);
	bfin.SetIcon(hIcn);

	hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_TERMINER),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);
	barret.SetIcon(hIcn);

	m_ToolTip.Create(this);

	m_ToolTip.AddTool( &bsuivant, _T("Analyze next line"));
	m_ToolTip.AddTool( &barret, _T("Stop program"));
	m_ToolTip.AddTool( &bsaute, _T("Jump to next breakpoint"));
	m_ToolTip.AddTool( &bentre, _T("Run into a function"));
	m_ToolTip.AddTool( &bfin, _T("Run up to end"));
	m_ToolTip.AddTool( &bbreak, _T("Add or remove a breakpoint on the current line"));
	m_ToolTip.AddTool( &bsortir, _T("Exit current function"));
	m_ToolTip.AddTool( &localvar, _T("Add variable names to filter"));

	return v;
}

BOOL debogueur::PreTranslateMessage(MSG* pMsg) {
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void debogueur::Lapile(string& s) {
	wstring base;
	s_utf8_to_unicode(base,(char*)s.c_str());
	vpile=(WCHAR*)base.c_str();
}

void debogueur::Variables(string& s) {
	wstring base;
	s_utf8_to_unicode(base,(char*)s.c_str());
	vvariables=(WCHAR*)base.c_str();
}

void debogueur::Locals() {
	wstring base;
	s_utf8_to_unicode(base,(char*)localvariables.c_str());
	locals=(WCHAR*)base.c_str();
}

extern LockedThread* loquet;
void debogueur::OnBnClickedSaute()
{
	// TODO: Add your control notification handler code here			
	lesuivant(false);
	if (kifencours()==false)
		CDialogEx::OnOK();
	else
		loquet->Released();
}


void debogueur::OnBnClickedSortir()
{
	// TODO: Add your control notification handler code here		
	sortiefonction();		
	if (kifencours()==false)
		CDialogEx::OnOK();
	else
		loquet->Released();
}


void debogueur::OnBnClickedFin()
{
	// TODO: Add your control notification handler code here
	jusquefin();
	CDialogEx::OnOK();
}


void debogueur::OnBnClickedSuivant()
{
	// TODO: Add your control notification handler code here	
	lesuivant(true);
	if (kifencours()==false)
		CDialogEx::OnOK();
	else
		loquet->Released();
}


extern "C" {
	bool kifltk_Terminate(KifCode* localkifcode);
}

void debogueur::OnBnClickedArretprg()
{
	// TODO: Add your control notification handler code here
	arretprg();
	CDialogEx::OnOK();
}


void debogueur::OnBnClickedPointarret()
{
	currentview->Pointarret(lignecourante);
}


void debogueur::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void debogueur::OnEnChangeEdit4()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	
	CString lastcar;
	lastcar=choixvariable.Right(1);
	if (lastcar[0]>=32)
		return;

	string s;
	string vrs;
	for (int i=0;i<choixvariable.GetLength();i++) {
		if (choixvariable[i]!='\r') {
			vrs="";
			s_unicode_to_utf8(vrs,WCSTR(choixvariable[i]));
			s+=vrs;
		}
	}

	SelectionVariables(s);
	vrs="";
	ReinitVariables(vrs);
	localvariables+="\r\n";
	localvariables+=vrs;
	Locals();
	UpdateData(FALSE);	
}


void debogueur::OnEnChangeEdit5()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here	
	UpdateData(TRUE);
	int lg=vsortie.GetLength();
	int lgb=voutput.GetLength();
	if (lg>lgb) {
		CString texte=vsortie.Right(lg-lgb).Trim();
		if (texte.GetLength()>1 && texte[texte.GetLength()-1]==';') {
			string lecode;
			texte=texte.Left(texte.GetLength()-1);
			wchar_t* base=WCSTR(texte);
			s_unicode_to_utf8(lecode,base);
			int i=compilekifsession(STR(lecode));
			if (i!=-1) {
				if (runkifsession()) {
					char* res=displaykifsession();
					if (res!=NULL && res[0]!=0) {
						vsortie+="\r\n";
						vsortie+=res;
						voutput=vsortie;
						UpdateData(FALSE);
					}
				}
			}
		}
	}
}


void debogueur::OnBnClickedEntre()
{
	// TODO: Add your control notification handler code here
	entredans(true);
	if (kifencours()==false)
		CDialogEx::OnOK();
	else
		loquet->Released();
}


void debogueur::OnBnClickedStickToThread()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (stickthread==FALSE)
		kifsticktothread(-99);
	else
		kifsticktothread(threadid);
}


void debogueur::OnEnChangeEdit3()
{
	UpdateData(TRUE);
	stickthread=TRUE;
	kifsticktothread(threadid);
	UpdateData(FALSE);
}


void debogueur::OnEnChangeCurrentVariable()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
