// seerule.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#include "seerule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Cseerule dialog


Cseerule::Cseerule(CWnd* pParent /*=NULL*/)
	: CDialog(Cseerule::IDD, pParent)
{
	//{{AFX_DATA_INIT(Cseerule)
	m_rulenumber = 0;
	m_rulenumberlast = 0;
	//}}AFX_DATA_INIT
}


void Cseerule::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cseerule)
	DDX_Text(pDX, IDC_rulenumber, m_rulenumber);
	DDX_Text(pDX, IDC_limitlast, m_rulenumberlast);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Cseerule, CDialog)
	//{{AFX_MSG_MAP(Cseerule)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cseerule message handlers

