// debogueur.cpp : implementation file
//

#include "stdafx.h"
#include "kif.h"
extern "C" {
#include "kifextern.h"
}
#include "KIFFN.h"
#include "debogueur.h"
#include "afxdialogex.h"
#include "KIFNDoc.h"
#include "KIFNView.h"

extern bool endofprocess;
// debogueur dialog

IMPLEMENT_DYNAMIC(debogueur, CDialogEx)

debogueur::debogueur(CWnd* pParent /*=NULL*/)
	: CDialogEx(debogueur::IDD, pParent)
	, vvariables(_T(""))
	, vpile(_T(""))
	, vlecode(_T(""))
	, vsortie(_T(""))
	, choixvariables(_T(""))
{

}

debogueur::~debogueur()
{
}

void debogueur::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, vvariables);
	DDX_Text(pDX, IDC_EDIT2, vpile);
	DDX_Control(pDX, IDC_LECODE, lecode);
	DDX_Text(pDX, IDC_LECODE, vlecode);
	DDX_Control(pDX, IDSUIVANT, bsuivant);
	DDX_Control(pDX, IDSAUTE, bsaute);
	DDX_Control(pDX, IDSORTIR, bsortir);
	DDX_Control(pDX, IDPOINTARRET, bbreak);
	DDX_Control(pDX, IDARRETPRG, barret);
	DDX_Control(pDX, IDFIN, bfin);
	DDX_Text(pDX, IDC_EDIT5, vsortie);
	DDX_Text(pDX, IDC_EDIT4, choixvariables);
}


BEGIN_MESSAGE_MAP(debogueur, CDialogEx)
	ON_BN_CLICKED(IDSAUTE, &debogueur::OnBnClickedSaute)
	ON_BN_CLICKED(IDSORTIR, &debogueur::OnBnClickedSortir)
	ON_BN_CLICKED(IDFIN, &debogueur::OnBnClickedFin)
	ON_BN_CLICKED(IDSUIVANT, &debogueur::OnBnClickedSuivant)
	ON_BN_CLICKED(IDARRETPRG, &debogueur::OnBnClickedArretprg)
	ON_BN_CLICKED(IDPOINTARRET, &debogueur::OnBnClickedPointarret)
	ON_EN_CHANGE(IDC_EDIT4, &debogueur::OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, &debogueur::OnEnChangeEdit5)
	ON_MESSAGE(MET_A_JOUR, &debogueur::MAJ)
END_MESSAGE_MAP()


// debogueur message handlers


LRESULT debogueur::MAJ(WPARAM wParam, LPARAM lParam) {
	Ligne();
	UpdateData(FALSE);
	return 0;
}

void debogueur::Ligne() {
	int D,F;
	lecode.GetSel(D,F);
	int l=lecode.LineFromChar(D)+1;

	lecode.SetFocus();
	long d=lecode.LineIndex(lignecourante-1);
	long f=d+lecode.LineLength(d);
	lecode.SetSel(d,f);	
	lecode.LineScroll(lignecourante-l, 0);

}


BOOL debogueur::OnInitDialog() {
	BOOL v=CDialogEx::OnInitDialog();
	Ligne();
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

	m_ToolTip.AddTool( &bsuivant, _T("Analyser la ligne suivante"));
	m_ToolTip.AddTool( &barret, _T("Arrêter l'exécution du programme"));
	m_ToolTip.AddTool( &bsaute, _T("Sauter jusqu'au point d'arrêt suivant"));
	m_ToolTip.AddTool( &bfin, _T("Analyser jusqu'à la fin sans interruption"));
	m_ToolTip.AddTool( &bbreak, _T("Rajouter un point d'arrêt"));
	m_ToolTip.AddTool( &bsortir, _T("Sortir de la boucle ou de la fonction courante"));

	return v;
}

BOOL debogueur::PreTranslateMessage(MSG* pMsg) {
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void debogueur::Lapile(string& s) {
	wstring base;
	s_utf8tounicode(base,(char*)s.c_str());
	vpile=(WCHAR*)base.c_str();
}

void debogueur::Variables(string& s) {
	wstring base;
	s_utf8tounicode(base,(char*)s.c_str());
	vvariables=(WCHAR*)base.c_str();
}

void debogueur::OnBnClickedSaute()
{
	// TODO: Add your control notification handler code here
	lesuivant(false);
	ThreadLock _lock(kifTemporary);
	endofprocess=true;
	if (kifencours()==false)
		CDialogEx::OnOK();
}


void debogueur::OnBnClickedSortir()
{
	// TODO: Add your control notification handler code here
	sortiefonction();
	ThreadLock _lock(kifTemporary);
	endofprocess=true;
	if (kifencours()==false)
		CDialogEx::OnOK();
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
	ThreadLock _lock(kifTemporary);
	endofprocess=true;
	if (kifencours()==false)
		CDialogEx::OnOK();
}


extern "C" {
	    bool kifltk_Terminate(KifCode* localkifcode);
}

void debogueur::OnBnClickedArretprg()
{
	// TODO: Add your control notification handler code here
	termination();
	kifltk_Terminate(NULL);
	CDialogEx::OnOK();
}

extern CKIFNView* vuecourante;

void debogueur::OnBnClickedPointarret()
{
	int d,f;
	lecode.GetSel(d,f);
	long l=lecode.LineFromChar(d)+1;
	vuecourante->Pointarret(l);
}

void debogueur::OnEnChangeEdit4()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	string s;
	for (int i=0;i<choixvariables.GetLength();i++) {
		if (choixvariables[i]!='\r')
			s+=choixvariables[i];
	}

	SelectionVariables(s);
	string vrs;
	ReinitVariables(vrs);
	Variables(vrs);
	UpdateData(FALSE);
}

void debogueur::OnEnChangeEdit5()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	UpdateData(TRUE);
	int lg=vsortie.GetLength();
	int lgb=voutput.GetLength();
	if (lg>lgb) {
		CString texte=vsortie.Right(lg-lgb).Trim();
		if (texte.GetLength()>1 && texte[texte.GetLength()-1]==';') {
			string lecode;
			texte=texte.Left(texte.GetLength()-1);
			wchar_t* base=(wchar_t*)LPCWSTR(texte);
			s_unicodetoutf8(lecode,base);
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
