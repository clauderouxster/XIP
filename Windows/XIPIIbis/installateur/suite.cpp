// suite.cpp : implementation file
//

#include "stdafx.h"
#include "installateur.h"
#include "suite.h"
#include "proprietes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Csuite dialog
void replace(CString& var,char i,char n,char remp);

Csuite::Csuite(CWnd* pParent /*=NULL*/)
	: CDialog(Csuite::IDD, pParent)
{
	//{{AFX_DATA_INIT(Csuite)
	m_param1 = _T("");
	m_param2 = _T("");
	m_param3 = _T("");
	m_param4 = _T("");
	m_param5 = _T("");
	m_param6 = _T("");
	m_param7 = _T("");
	m_param8 = _T("");
	//}}AFX_DATA_INIT
}


void Csuite::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Csuite)
	DDX_Text(pDX, IDC_parameter1, m_param1);
	DDX_Text(pDX, IDC_parameter2, m_param2);
	DDX_Text(pDX, IDC_parameter3, m_param3);
	DDX_Text(pDX, IDC_parameter4, m_param4);
	DDX_Text(pDX, IDC_parameter5, m_param5);
	DDX_Text(pDX, IDC_parameter6, m_param6);
	DDX_Text(pDX, IDC_parameter7, m_param7);
	DDX_Text(pDX, IDC_parameter8, m_param8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Csuite, CDialog)
	//{{AFX_MSG_MAP(Csuite)
	ON_BN_CLICKED(IDC_explore_parameter1, Onexploreparameter1)
	ON_BN_CLICKED(IDC_explore_parameter2, Onexploreparameter2)
	ON_BN_CLICKED(IDC_explore_parameter3, Onexploreparameter3)
	ON_BN_CLICKED(IDC_explore_parameter4, Onexploreparameter4)
	ON_BN_CLICKED(IDC_explore_parameter5, Onexploreparameter5)
	ON_BN_CLICKED(IDC_explore_parameter6, Onexploreparameter6)
	ON_BN_CLICKED(IDC_explore_parameter7, Onexploreparameter7)
	ON_BN_CLICKED(IDC_explore_parameter8, Onexploreparameter8)
	ON_BN_CLICKED(IDC_paramtowindows, Onparamtowindows)
	ON_BN_CLICKED(IDC_paramtounix, Onparamtounix)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Csuite message handlers
void Csuite::deParam(CString* params,Cproprietes* p) {
    prop=p;
	m_param1 = params[0];
	m_param2 = params[1];
	m_param3 = params[2];
	m_param4 = params[3];
	m_param5 = params[4];
	m_param6 = params[5];
	m_param7 = params[6];
}


void Csuite::versParam(CString* params) {
	params[0] = m_param1;
    params[1] = m_param2;
	params[2] = m_param3;
	params[3] = m_param4;
	params[4] = m_param5;
	params[5] = m_param6;
	params[6] = m_param7;	
}

void Csuite::Onexploreparameter1() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CString path;
    prop->reconstitueChemin(m_param1,path);

    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"XIP (*.xip)|*.xip|");	
    if (fichier.DoModal()==IDOK) {
        m_param1=fichier.GetPathName();   
        prop->rechercheChemin(m_param1,fichier.GetFileName());
        UpdateData(FALSE);
    }
	
}

void Csuite::Onexploreparameter2() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);    
    CString path;
    prop->reconstitueChemin(m_param2,path);

    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"XIP (*.xip)|*.xip|");	
    if (fichier.DoModal()==IDOK) {
        m_param2=fichier.GetPathName();   
        prop->rechercheChemin(m_param2,fichier.GetFileName());
        UpdateData(FALSE);
    }
	
}

void Csuite::Onexploreparameter3() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CString path;
    prop->reconstitueChemin(m_param3,path);

    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"XIP (*.xip)|*.xip|");	
    if (fichier.DoModal()==IDOK) {
        m_param3=fichier.GetPathName();   
        prop->rechercheChemin(m_param3,fichier.GetFileName());
        UpdateData(FALSE);
    }
	
}

void Csuite::Onexploreparameter4() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CString path;
    prop->reconstitueChemin(m_param4,path);

    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"XIP (*.xip)|*.xip|");	
    if (fichier.DoModal()==IDOK) {
        m_param4=fichier.GetPathName();   
        prop->rechercheChemin(m_param4,fichier.GetFileName());
        UpdateData(FALSE);
    }
	
}

void Csuite::Onexploreparameter5() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CString path;
    prop->reconstitueChemin(m_param5,path);

    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"XIP (*.xip)|*.xip|");	
    if (fichier.DoModal()==IDOK) {
        m_param5=fichier.GetPathName();   
        prop->rechercheChemin(m_param5,fichier.GetFileName());
        UpdateData(FALSE);
    }
	
}

void Csuite::Onexploreparameter6() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CString path;
    prop->reconstitueChemin(m_param6,path);

    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"XIP (*.xip)|*.xip|");	
    if (fichier.DoModal()==IDOK) {
        m_param6=fichier.GetPathName();   
        prop->rechercheChemin(m_param6,fichier.GetFileName());
        UpdateData(FALSE);
    }
	
}

void Csuite::Onexploreparameter7() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CString path;
    prop->reconstitueChemin(m_param7,path);

    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"XIP (*.xip)|*.xip|");	
    if (fichier.DoModal()==IDOK) {
        m_param7=fichier.GetPathName();   
        prop->rechercheChemin(m_param7,fichier.GetFileName());
        UpdateData(FALSE);
    }
	
}

void Csuite::Onexploreparameter8() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CString path;
    prop->reconstitueChemin(m_param8,path);

    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"XIP (*.xip)|*.xip|");	
    if (fichier.DoModal()==IDOK) {
        m_param8=fichier.GetPathName();   
        prop->rechercheChemin(m_param8,fichier.GetFileName());
        UpdateData(FALSE);
    }
	
}


CString returnName(CString& v) {
    CString name=v;
    int j=name.ReverseFind('\\');
    if (j!=-1)
        name=name.Mid(j+1);
    return name;
}

void Csuite::Onparamtowindows() 
{
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
    UpdateData(TRUE);
	replace(m_param1,'/','\\',1);
	replace(m_param2,'/','\\',1);
	replace(m_param3,'/','\\',1);
	replace(m_param4,'/','\\',1); 
	replace(m_param5,'/','\\',1);
	replace(m_param6,'/','\\',1);
	replace(m_param7,'/','\\',1);
	replace(m_param8,'/','\\',1);
    prop->rechercheChemin(m_param1,returnName(m_param1));
    prop->rechercheChemin(m_param2,returnName(m_param2));
    prop->rechercheChemin(m_param3,returnName(m_param3));
    prop->rechercheChemin(m_param4,returnName(m_param4));
    prop->rechercheChemin(m_param5,returnName(m_param5));
    prop->rechercheChemin(m_param6,returnName(m_param6));
    prop->rechercheChemin(m_param7,returnName(m_param7));
    prop->rechercheChemin(m_param8,returnName(m_param8));
   UpdateData(FALSE);

}

void Csuite::Onparamtounix() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
	replace(m_param1,'\\','/',1);
	replace(m_param2,'\\','/',1);
	replace(m_param3,'\\','/',1);
	replace(m_param4,'\\','/',1); 
	replace(m_param5,'\\','/',1);
	replace(m_param6,'\\','/',1);
	replace(m_param7,'\\','/',1);
	replace(m_param8,'\\','/',1);
    prop->rechercheChemin(m_param1,returnName(m_param1));
    prop->rechercheChemin(m_param2,returnName(m_param2));
    prop->rechercheChemin(m_param3,returnName(m_param3));
    prop->rechercheChemin(m_param4,returnName(m_param4));
    prop->rechercheChemin(m_param5,returnName(m_param5));
    prop->rechercheChemin(m_param6,returnName(m_param6));
    prop->rechercheChemin(m_param7,returnName(m_param7));
    prop->rechercheChemin(m_param8,returnName(m_param8));
    UpdateData(FALSE);	
	
}
