// limit.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#include "limit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Climit dialog


Climit::Climit(CWnd* pParent /*=NULL*/)
	: CDialog(Climit::IDD, pParent)
{
	//{{AFX_DATA_INIT(Climit)
	m_limit = 0;
	//}}AFX_DATA_INIT
}


void Climit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Climit)
	DDX_Text(pDX, IDC_EDIT1, m_limit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Climit, CDialog)
	//{{AFX_MSG_MAP(Climit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Climit message handlers
