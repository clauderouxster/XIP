// grm.cpp : implementation file
//

#include "stdafx.h"
#include "installateur.h"
#include "grm.h"
#include "xipvecta.h"
#include "proprietes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Cgrm dialog


Cgrm::Cgrm(CWnd* pParent /*=NULL*/)
	: CDialog(Cgrm::IDD, pParent)
{
	//{{AFX_DATA_INIT(Cgrm)
	m_pathname = _T("");
	//}}AFX_DATA_INIT
}


void Cgrm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cgrm)
	DDX_Control(pDX, IDC_explore_path_name, m_explore_path_name);
	DDX_Text(pDX, IDC_pathname, m_pathname);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Cgrm, CDialog)
	//{{AFX_MSG_MAP(Cgrm)
	ON_BN_CLICKED(IDC_explore_path_name, Onexplorepathname)
	ON_BN_CLICKED(IDMODIFY, OnModify)
	ON_BN_CLICKED(IDEXIT, OnExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cgrm message handlers

void Cgrm::Onexplorepathname() 
{
    CFileDialog fichier(TRUE,"grm","language",OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"GRM (*.grm)|*.grm|");	
    if (fichier.DoModal()==IDOK) {        
        m_pathname=fichier.GetPathName();
        UpdateData(FALSE);
    }	
}


int ExtraitClef(char* nomfichier,int& debut,int& fin,VECTA<char*>& CalculClef,char*);
char InsereClef(char* nomfichier, CString clef,int debut,int fin,VECTA<char*>& CalculClef,char*);


void Cgrm::OnModify() 
{
    UpdateData(FALSE);
    if (m_pathname=="")
        return;
    
    Cproprietes prop;
    prop.m_pathname=m_pathname;        
    prop.LoadGrmFiles(m_pathname);
    if (prop.DoModal()==IDOK) {
        //On regenere le fichier            
        if (prop.m_language!="") {
            
            ofstream fichier(prop.m_pathname);
            
            //Tout d'abord les variables
            if (prop.m_variable1!="" && prop.m_pathvar1!="")
                fichier<<"path:"<<endl<<LPCTSTR(prop.m_variable1)<<"="<<LPCTSTR(prop.m_pathvar1)<<endl;
            if (prop.m_variable2!="" && prop.m_pathvar2!="")
                fichier<<"path:"<<endl<<LPCTSTR(prop.m_variable2)<<"="<<LPCTSTR(prop.m_pathvar2)<<endl;
            if (prop.m_variable3!="" && prop.m_pathvar3!="")
                fichier<<"path:"<<endl<<LPCTSTR(prop.m_variable3)<<"="<<LPCTSTR(prop.m_pathvar3)<<endl;
            //Ensuite les conversions
            if (prop.m_conversion==TRUE)
                fichier<<"conversion:"<<endl<<"utf82ascii"<<endl;
            //Ensuite le fichier de langue
            if (prop.m_crypte==TRUE)
                fichier<<"gram:"<<endl;
            else
                fichier<<"grammar:"<<endl;
            fichier<<LPCTSTR(prop.m_language)<<endl;
            //Ensuite ntm
            if (prop.m_ntm!="")
                fichier<<"ntm:"<<endl<<LPCTSTR(prop.m_ntm)<<endl;
            //hmm
            if (prop.m_hmm!="")
                fichier<<"hmm:"<<endl<<LPCTSTR(prop.m_hmm)<<endl;
            //Puis les parametres
            for (int i=0;i<7;i++) {
                if (prop.params[i]!="")
                    fichier<<"parameter:"<<endl<<LPCTSTR(prop.params[i])<<endl;
            }
            if (prop.ntmpath!="" && prop.ntmfichier!=NULL) {
                ofstream ff(prop.ntmpath);
                ff<<prop.ntmfichier->str().c_str();
                delete prop.ntmfichier;
                prop.ntmfichier=NULL;
            }
        }
    }
}


void Cgrm::OnExit() 
{
    exit(-1);	
}

void Cgrm::Onconversion() 
{
	// TODO: Add your control notification handler code here
	
}
