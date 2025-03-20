// kifmethod.cpp : implementation file
//

#include "stdafx.h"
#include "KIFF.h"
#include "kif.h"
#include "kifmethod.h"
#include "afxdialogex.h"


// kifmethod dialog

IMPLEMENT_DYNAMIC(kifmethod, CDialogEx)

kifmethod::kifmethod(CWnd* pParent /*=NULL*/)
	: CDialogEx(kifmethod::IDD, pParent)
	, atype(_T(""))
	, letexte(_T(""))
	, lemessage(_T("empty"))
{
	x = 0; y = 0; 
}

kifmethod::~kifmethod()
{
}

void kifmethod::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBOLISTE, laliste);
	DDX_Text(pDX, IDC_EDIT2, atype);
	DDX_CBString(pDX, IDC_COMBOLISTE, letexte);
}


BEGIN_MESSAGE_MAP(kifmethod, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBOLISTE, &kifmethod::OnCbnSelchangeComboliste)	
END_MESSAGE_MAP()

BOOL kifmethod::PreTranslateMessage(MSG* pMsg) {
	int i = laliste.GetCurSel();
	if (i >= 0 && i <= listes.size()) {
		CString cmsg;
		laliste.GetLBText(i, cmsg);
		wstring s = cmsg;
		if (infos.find(s) != infos.end()) {
			s = infos[s];
			lemessage = s.c_str();
			if (lemessage != dernierechaine) {
				dernierechaine = lemessage;
				m_ToolTip.UpdateTipText(lemessage, &laliste);
			}
		}
		else {
			lemessage = "No description";
			if (lemessage != dernierechaine) {
				dernierechaine = lemessage;
				m_ToolTip.UpdateTipText(lemessage, &laliste);
			}
		}
	}

	m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}


// kifmethod message handlers


void kifmethod::OnCbnSelchangeComboliste()
{
	// TODO: Add your control notification handler code here	
	if (laliste.GetDroppedState()) {
		int i = laliste.GetCurSel();
		if (i >= 0 && i <= listes.size()) {
			CString cmsg;
			laliste.GetLBText(i, cmsg);
			resultat = cmsg;
		}
		CDialogEx::OnOK();
	}
}

BOOL kifmethod::OnInitDialog() {
	BOOL v = CDialogEx::OnInitDialog();
	m_ToolTip.Create(this);
	m_ToolTip.AddTool(&laliste, lemessage);
	SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	for (int i = 0; i < listes.size(); i++)
		laliste.AddString(listes[i].c_str());
	laliste.SetCurSel(0);
	return v;
}


