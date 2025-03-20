// Searchstring.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#include "Searchstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchstring dialog


CSearchstring::CSearchstring(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchstring::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSearchstring)
	m_searchstring = _T("");
	//}}AFX_DATA_INIT
    find=0;
}


void CSearchstring::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchstring)
	DDX_Text(pDX, IDC_searchstring, m_searchstring);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchstring, CDialog)
	//{{AFX_MSG_MAP(CSearchstring)
	ON_BN_CLICKED(IDC_find, Onfind)
	ON_BN_CLICKED(IDC_findnext, Onfindnext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchstring message handlers

void CSearchstring::Onfind() 
{
	// TODO: Add your control notification handler code here
    find=1;
	OnOK();
}

void CSearchstring::Onfindnext() 
{
	// TODO: Add your control notification handler code here
    find=2;
    OnOK();	
}
