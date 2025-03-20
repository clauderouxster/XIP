// ListeVariable.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#include "ListeVariable.h"

#define XIPLIBXML
#include "xipstlres.h"
#include "pasofficiel.h"
#include "printvar.h"
extern int parseurCourant;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListeVariable dialog


CListeVariable::CListeVariable(CWnd* pParent /*=NULL*/)
	: CDialog(CListeVariable::IDD, pParent)
{
	//{{AFX_DATA_INIT(CListeVariable)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CListeVariable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListeVariable)
	DDX_Control(pDX, IDC_listevar, m_variables);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CListeVariable, CDialog)
	//{{AFX_MSG_MAP(CListeVariable)
	ON_WM_CANCELMODE()
	ON_LBN_DBLCLK(IDC_listevar, OnDblclklistevar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListeVariable message handlers

BOOL CListeVariable::OnInitDialog() 
{
	CDialog::OnInitDialog();

	
	// TODO: Add extra initialization here
    //First we need to load the variable names and types
    vector<string> vars;
    vector<typeVariable> vartype;
    
    XipGetListVariables(parseurCourant,vars,vartype);
    //We then fill in the lists...
    int i;
    for (i=0;i<vars.size();i++) {
		string affiche=vars[i];
		for (int j=affiche.size();j<40;j++)
			affiche+=" ";
		affiche+="\t\t";
		affiche+=typeVariableString[vartype[i]];
        m_variables.AddString((char*)affiche.c_str());
        //m_types.AddString(typeVariableString[vartype[i]]);
    }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CListeVariable::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CListeVariable::OnDblclklistevar() 
{
	// TODO: Add your control notification handler code here
	int i=m_variables.GetCurSel();
    if (i != LB_ERR) {
        CString var;
        m_variables.GetText(i,var);
        Cprintvar prt;
		int fi=var.Find(" ");
		var=var.Mid(0,fi);
		var=var.Trim();
        prt.m_name=var;
        prt.DoModal();
        OnOK();
    }
}
