// grm.cpp : implementation file
//

#include "stdafx.h"
#include "installateur.h"
#include "grm.h"
#include "xipvecta.h"
#include "encryptage.h"
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
	m_xipkeyfile = _T("");
	//}}AFX_DATA_INIT
}


void Cgrm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cgrm)
	DDX_Control(pDX, IDC_explore_path_name, m_explore_path_name);
	DDX_Text(pDX, IDC_pathname, m_pathname);
	DDX_Text(pDX, IDC_xipkey, m_xipkeyfile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Cgrm, CDialog)
	//{{AFX_MSG_MAP(Cgrm)
	ON_BN_CLICKED(IDC_explore_path_name, Onexplorepathname)
	ON_BN_CLICKED(IDC_explore_xip_key, Onexplorexipkey)
	ON_BN_CLICKED(IDKEY, OnKey)
	ON_BN_CLICKED(IDKEYCRYPTED, OnKeyCrypted)
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

void Cgrm::Onexplorexipkey() 
{
    CFileDialog fichier(TRUE,"xip","language",OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"XIP (*.xip)|*.xip|");	
    if (fichier.DoModal()==IDOK) {        
        m_xipkeyfile=fichier.GetPathName();
        UpdateData(FALSE);
    }	
	
}

int ExtraitClef(char* nomfichier,int& debut,int& fin,VECTA<char*>& CalculClef,char*);
char InsereClef(char* nomfichier, CString clef,int debut,int fin,VECTA<char*>& CalculClef,char*);

void Cgrm::Compute() 
{   
    VECTA<char*> CalculClef;
    char utilise[1000];
    utilise[0]=0;
    int debut,fin;
    if (m_xipkeyfile=="")
        return;

    if (ExtraitClef((char*)LPCTSTR(m_xipkeyfile),debut,fin,CalculClef,utilise)==1) {
        Cencryptage encrypte;
        encrypte.initialise(CalculClef,utilise);
        if (encrypte.DoModal()==IDOK && encrypte.m_clef!="") {            
            if (InsereClef((char*)LPCTSTR(m_xipkeyfile),encrypte.m_clef,debut,fin,CalculClef,utilise)==1) {
                CString message="File:"+m_xipkeyfile+" has been modified successfully";
                AfxMessageBox(message);                
            }
        }
    }

    for (int imo=0;imo<CalculClef.dernier;imo++)
        delete[] CalculClef[imo];
    CalculClef.raz();
}

extern char cryptage;
extern int bouclage;
void Cgrm::OnKeyCrypted() {
	cryptage=1;
	bouclage=0;
	Compute();
}

void Cgrm::OnKey() {
	cryptage=0;
	bouclage=0;
	Compute();
}

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
            if (prop.m_variable4!="" && prop.m_pathvar4!="")
                fichier<<"path:"<<endl<<LPCTSTR(prop.m_variable4)<<"="<<LPCTSTR(prop.m_pathvar4)<<endl;
            if (prop.m_variable5!="" && prop.m_pathvar5!="")
                fichier<<"path:"<<endl<<LPCTSTR(prop.m_variable5)<<"="<<LPCTSTR(prop.m_pathvar5)<<endl;
            if (prop.m_variable6!="" && prop.m_pathvar6!="")
                fichier<<"path:"<<endl<<LPCTSTR(prop.m_variable6)<<"="<<LPCTSTR(prop.m_pathvar6)<<endl;
            if (prop.m_variable7!="" && prop.m_pathvar7!="")
                fichier<<"path:"<<endl<<LPCTSTR(prop.m_variable7)<<"="<<LPCTSTR(prop.m_pathvar7)<<endl;
            if (prop.m_variable8!="" && prop.m_pathvar8!="")
                fichier<<"path:"<<endl<<LPCTSTR(prop.m_variable8)<<"="<<LPCTSTR(prop.m_pathvar8)<<endl;
            if (prop.m_variable9!="" && prop.m_pathvar9!="")
                fichier<<"path:"<<endl<<LPCTSTR(prop.m_variable9)<<"="<<LPCTSTR(prop.m_pathvar9)<<endl;
            if (prop.m_variable10!="" && prop.m_pathvar10!="")
                fichier<<"path:"<<endl<<LPCTSTR(prop.m_variable10)<<"="<<LPCTSTR(prop.m_pathvar10)<<endl;
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
