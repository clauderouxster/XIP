// var.cpp : implementation file
//

#include "stdafx.h"
#include "installateur.h"
#include "var.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Cvar dialog


Cvar::Cvar(CWnd* pParent /*=NULL*/)
	: CDialog(Cvar::IDD, pParent)
{
	//{{AFX_DATA_INIT(Cvar)
	m_var = _T("");
	//}}AFX_DATA_INIT
}


void Cvar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cvar)
	DDX_Text(pDX, IDC_variable, m_var);
	//}}AFX_DATA_MAP    
}


BEGIN_MESSAGE_MAP(Cvar, CDialog)
	//{{AFX_MSG_MAP(Cvar)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cvar message handlers
