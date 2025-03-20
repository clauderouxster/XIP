// printvar.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#include "printvar.h"

#define XIPLIBXML
#include "pasofficiel.h"

extern int parseurCourant;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Cprintvar dialog


Cprintvar::Cprintvar(CWnd* pParent /*=NULL*/)
	: CDialog(Cprintvar::IDD, pParent)
{
	//{{AFX_DATA_INIT(Cprintvar)
	m_name = _T("");
	m_value = _T("");
	//}}AFX_DATA_INIT
}


void Cprintvar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cprintvar)
	DDX_Text(pDX, IDC_variablename, m_name);
	DDX_Text(pDX, IDC_variablevalue, m_value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Cprintvar, CDialog)
	//{{AFX_MSG_MAP(Cprintvar)
	ON_BN_CLICKED(ID_printvalue, Onprintvalue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cprintvar message handlers

void Cprintvar::Onprintvalue() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    string val="Unknown";
    XipGetVariableValue(parseurCourant,LPCTSTR(m_name),val);
    m_value=val.c_str();
    UpdateData(FALSE);
	
}
