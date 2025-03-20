// BuildingRule.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#include "BuildingRule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BuildingRule dialog


BuildingRule::BuildingRule(CWnd* pParent /*=NULL*/)
	: CDialog(BuildingRule::IDD, pParent)
{
	//{{AFX_DATA_INIT(BuildingRule)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void BuildingRule::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BuildingRule)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BuildingRule, CDialog)
	//{{AFX_MSG_MAP(BuildingRule)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BuildingRule message handlers
