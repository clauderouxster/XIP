// filenameinput.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#include "filenameinput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// filenameinput dialog


filenameinput::filenameinput(CWnd* pParent /*=NULL*/)
	: CDialog(filenameinput::IDD, pParent)
{
	//{{AFX_DATA_INIT(filenameinput)
	m_filename = _T("");
	//}}AFX_DATA_INIT
}


void filenameinput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(filenameinput)
	DDX_Text(pDX, IDC_filename, m_filename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(filenameinput, CDialog)
	//{{AFX_MSG_MAP(filenameinput)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// filenameinput message handlers
