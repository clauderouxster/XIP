// kifmethods.cpp : implementation file
//

#include "stdafx.h"
#include "KIFF.h"
#include "kif.h"
#include "kifmethods.h"
#include "afxdialogex.h"


// kifmethods dialog

IMPLEMENT_DYNAMIC(kifmethods, CDialogEx)

kifmethods::kifmethods(CWnd* pParent /*=NULL*/)
	: CDialogEx(kifmethods::IDD, pParent)
	, atype(_T(""))
	, letexte(_T(""))
	, lemessage(_T(""))
{
}

kifmethods::~kifmethods()
{
}

void kifmethods::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, atype);
	DDX_Control(pDX, IDC_COMBO1, laliste);
	DDX_CBString(pDX, IDC_COMBO1, letexte);
}


BEGIN_MESSAGE_MAP(kifmethods, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &kifmethods::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// kifmethods message handlers


void kifmethods::OnCbnSelchangeCombo1()
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

BOOL kifmethods::OnInitDialog() {
	BOOL v = CDialogEx::OnInitDialog();
	m_ToolTip.Create(this);
	m_ToolTip.AddTool(&laliste, lemessage);
	SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	for (int i = 0; i < listes.size(); i++)
		laliste.AddString(listes[i].c_str());
	laliste.SetCurSel(0);
	return v;
}

BOOL kifmethods::PreTranslateMessage(MSG* pMsg) {
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
