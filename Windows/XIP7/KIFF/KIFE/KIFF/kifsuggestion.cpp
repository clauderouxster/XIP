// kifsuggestion.cpp : implementation file
//

#include "stdafx.h"
#include "KIFF.h"
#include "kif.h"
#include "kifsuggestion.h"
#include "afxdialogex.h"


// kifsuggestion dialog

IMPLEMENT_DYNAMIC(kifsuggestion, CDialogEx)

kifsuggestion::kifsuggestion(CWnd* pParent /*=NULL*/)
: CDialogEx(kifsuggestion::IDD, pParent)
{

}

kifsuggestion::~kifsuggestion()
{
}

void kifsuggestion::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_liste_suggestion, laliste);
}


BEGIN_MESSAGE_MAP(kifsuggestion, CDialogEx)
	ON_CBN_SELCHANGE(IDC_liste_suggestion, &kifsuggestion::OnCbnSelchangelistesuggestion)
	ON_CBN_EDITCHANGE(IDC_liste_suggestion, &kifsuggestion::OnCbnEditchangelistesuggestion)
END_MESSAGE_MAP()


// kifsuggestion message handlers

BOOL kifsuggestion::OnInitDialog() {
	BOOL v = CDialogEx::OnInitDialog();
	for (int i = 0; i < listes.size(); i++)
		laliste.AddString(listes[i].c_str());
	return v;
}

void kifsuggestion::OnCbnSelchangelistesuggestion()
{
	// TODO: Add your control notification handler code here
	int i = laliste.GetCurSel();
	if (i >= 0 && i <= listes.size())
		resultat = listes[i];
	CDialogEx::OnOK();
}


void kifsuggestion::OnCbnEditchangelistesuggestion()
{
	// TODO: Add your control notification handler code here
	cerr << "";
}
