// ParametreFile.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#include "ParametreFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParametreFile dialog


CParametreFile::CParametreFile(CWnd* pParent /*=NULL*/)
	: CDialog(CParametreFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParametreFile)
	m_rule = _T("");
	//}}AFX_DATA_INIT
    create=TRUE;
    filename="";
}


void CParametreFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParametreFile)
	DDX_Control(pDX, IDC_listfilename, m_listfilewindow);
	DDX_Text(pDX, IDC_rule, m_rule);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParametreFile, CDialog)
	//{{AFX_MSG_MAP(CParametreFile)
	ON_BN_CLICKED(IDC_load, Onload)
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParametreFile message handlers

void CParametreFile::Onload() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CFileDialog param(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Parameter (*.xip)|*.xip|");
    
    if (param.DoModal()==IDCANCEL)
        return;    
    int i=m_listfilewindow.AddString(param.GetPathName());    
    m_listfilewindow.SetCurSel(i);
    create=FALSE;
    UpdateData(FALSE);
    OnOK();

}



BOOL CParametreFile::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    for (int i=0;i<filenames.size();i++)
        m_listfilewindow.AddString(filenames[i]);

    m_listfilewindow.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CParametreFile::OnOK() 
{
	// TODO: Add extra validation here
    int i=m_listfilewindow.GetCurSel();
    m_listfilewindow.GetLBText(i,filename);
	CDialog::OnOK();
}
