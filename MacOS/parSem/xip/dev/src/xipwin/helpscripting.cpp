// helpscripting.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#include "helpscripting.h"
#define XIPLIBXML
#include "xipstlres.h"
#include "xipvecta.h"
#include "pasofficiel.h"

extern int parseurCourant;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// helpscripting dialog


helpscripting::helpscripting(CWnd* pParent /*=NULL*/)
	: CDialog(helpscripting::IDD, pParent)
{
	//{{AFX_DATA_INIT(helpscripting)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void helpscripting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(helpscripting)
	DDX_Control(pDX, IDC_instruction, m_instructions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(helpscripting, CDialog)
	//{{AFX_MSG_MAP(helpscripting)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// helpscripting message handlers

BOOL helpscripting::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	vector<string> messages;
    ListOfOperations(messages);
    
    HTREEITEM racine=m_instructions.InsertItem("Help"); 
    HTREEITEM racinebis;

    unsigned char c1;
  
    for (int i=0;i<messages.size();i++) {
        c1=messages[i][0];
        if (c1>='0' && c1<='9')
            racinebis=m_instructions.InsertItem((char*)messages[i].c_str(),racine); 
        else {
            char* vv=(char*)messages[i].c_str();
            char* pt=strchr(vv,'\t');
            if (pt!=NULL)
                *pt=0;
            HTREEITEM decl=m_instructions.InsertItem(vv,racinebis); 
            pt++;
            if (pt!=NULL) {
                while (*pt=='\t') pt++;
                m_instructions.InsertItem(pt,decl); 
            }
        }
    }

    m_instructions.Expand(racine,TVE_EXPAND);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
