// ntm.cpp : implementation file
//

#include "stdafx.h"
#include "installateur.h"
#include "proprietes.h"
#include "ntm.h"
#include "var.h"

void replace(CString& var,char i,char n,char remp);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Cntm dialog


Cntm::Cntm(CWnd* pParent /*=NULL*/)
	: CDialog(Cntm::IDD, pParent)
{
	//{{AFX_DATA_INIT(Cntm)
	m_ntmfilename = _T("");
	//}}AFX_DATA_INIT
    initial=TRUE;
}


void Cntm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cntm)
	DDX_Control(pDX, IDC_lexique, m_lexique);
	DDX_Control(pDX, IDC_chemins, m_chemins);
	DDX_Text(pDX, IDC_ntm, m_ntmfilename);
	//}}AFX_DATA_MAP
    if (initial==TRUE) {        
        for (int i=0;i<lexiques.size();i++) {
            m_lexique.AddString(lexiques[i].c_str());
            m_chemins.AddString(chemins[i].c_str());
        }
    }
    initial=FALSE;
}


BEGIN_MESSAGE_MAP(Cntm, CDialog)
	//{{AFX_MSG_MAP(Cntm)
	ON_BN_CLICKED(IDC_explore, Onexplore)
	ON_BN_CLICKED(IDC_delete, Ondelete)
	ON_BN_CLICKED(IDC_add, Onadd)
	ON_LBN_SELCHANGE(IDC_chemins, OnSelchangechemins)
	ON_LBN_SELCHANGE(IDC_lexique, OnSelchangelexique)
	ON_BN_CLICKED(IDC_adaptpath, Onadaptpath)
	ON_BN_CLICKED(IDC_ntmsaveas, Onntmsaveas)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cntm message handlers

#define modeOuverture ios::in|ios::binary
char Cntm::chargement() {
    m_ntmfilename=fichierntm;
    ifstream fichier(fichierntm,modeOuverture);
   
    if (fichier.fail())
        return 0;

    int i=0;
    unsigned char c;
    c=fichier.get();
    while (!fichier.eof()) {        
        if (c!=13)
            buffer[i++]=c;
        c=fichier.get();
    }
    buffer[i]=0;
    i=0;
    lecture=FALSE;
    if (!memcmp(buffer,"SPACES:",7)) {        
        while (buffer[i]!='\n') i++;
        i++;
        while (buffer[i]!='\n') i++;
        while (buffer[i]<=32) i++;
    }
    else 
        return 0;

    if (!memcmp(buffer+i,"SEPARATORS:",10)) {
        while (buffer[i]!='\n') i++;
        i++;
        while (buffer[i]!='\n') i++;
        while (buffer[i]<=32) i++;
    }

    lecture=TRUE;
    buffer[i-1]=0;
    debutfinbuffer=strstr((char*)buffer+i,"\n\n");
    debutfinbuffer[1]=0;
    unsigned char k;
    int l=0;
    //On extrait nos chaines
    while (buffer[i]!=0) {
        int j=i;
        while (buffer[j]>32) j++;
        k=buffer[j];
        buffer[j]=0;
        lexiques.push_back((char*)buffer+i);
        buffer[j]=k;
        i=j+1;      
        while (buffer[i]<=32) i++;
        j=i;
        while (buffer[j]>32) j++;
        k=buffer[j];
        buffer[j]=0;
        if (buffer[i]!='$') {
            CString chm=buffer+i;
            chm.Replace("/","\\");
            int psd=chm.ReverseFind('\\');
            CString nm=chm.Right(chm.GetLength()-psd-1);
            prop->rechercheChemin(chm,nm);
            chm.Replace("\\","/");
            chemins.push_back(LPCTSTR(chm));
        }
        else
            chemins.push_back((char*)buffer+i);        
        buffer[j]=k;
        i=j+1;
        while (buffer[i] && buffer[i]<=32) i++;
    }
    debutfinbuffer[1]='\n';    
    debutfinbuffer++;
    return 1;
}

        
void Cntm::maj() {
    lexiques.clear();
    chemins.clear();
    for (int j=0;j<m_lexique.GetCount();j++) {
        CString var;
        CString chem;
        m_lexique.GetText(j,var);
        m_chemins.GetText(j,chem);
        lexiques.push_back(LPCTSTR(var));
        chemins.push_back(LPCTSTR(chem));
    }
    
}

void Cntm::Onexplore() 
{
    UpdateData(TRUE);
    int i=m_lexique.GetCurSel();
    if (i!=-1) {
        CString mpath;
        m_chemins.GetText(i,mpath);
        CString path;
        prop->reconstitueChemin(mpath,path,"/");
        path.Replace("/","\\");
        CFileDialog lefichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"LEX (*.fst)|*.fst|");	
        if (lefichier.DoModal()==IDOK) {
            mpath=lefichier.GetPathName();
            prop->rechercheChemin(mpath,lefichier.GetFileName());
            mpath.Replace("\\","/");
            m_chemins.DeleteString(i);
            m_chemins.InsertString(i,mpath);
            UpdateData(FALSE);
            maj();
        }
    }
}

void Cntm::Ondelete() 
{
    UpdateData(TRUE);
    int i=m_lexique.GetCurSel();
    if (i!=-1) {
        m_chemins.DeleteString(i);
        m_lexique.DeleteString(i);
        UpdateData(FALSE);
        maj();
    }
}

void Cntm::Onadd() 
{
    UpdateData(TRUE);
    Cvar var;
    if (var.DoModal()==IDOK) {        
        int isel=m_lexique.AddString(var.m_var);
        m_chemins.AddString("Select a Path or this script will not be saved");
        m_lexique.SetCurSel(isel);
        UpdateData(FALSE);
        maj();
    }
}

void Cntm::OnOK() 
{
    if (lecture==FALSE) {
        CDialog::OnCancel();
        return;
    }
    
    // TODO: Add extra validation here
    for (int i=0;i<chemins.size();i++) {
        if (chemins[i]=="Select a Path or this script will not be saved") {
            AfxMessageBox("There is a variable whose pathname has not been initialized",MB_OK);
            return;
        }
    }
    
	CDialog::OnOK();
}

void Cntm::OnSelchangechemins() 
{
	// TODO: Add your control notification handler code here
    int i=m_chemins.GetCurSel();
    m_lexique.SetCurSel(i);
    
	
}

void Cntm::OnSelchangelexique() 
{
	// TODO: Add your control notification handler code here
    int i=m_lexique.GetCurSel();
    m_chemins.SetCurSel(i);

}

void Cntm::Onadaptpath() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    vector<CString> vect;	
    int i,j;
    //We take the first element as example
    CString example_path;
    CString v;  
    CString name;
    m_chemins.GetText(0,example_path);
    j=example_path.ReverseFind('/');    
    if (j!=-1) {        
        name=example_path.Mid(j+1);
        prop->rechercheChemin(example_path,name);
        example_path.Replace("\\","/");        
        vect.push_back(example_path);        
        i=example_path.ReverseFind('/');
        example_path=example_path.Left(i+1);
        for (i=1;i<m_chemins.GetCount();i++) {
            m_chemins.GetText(i,v);
            j=v.ReverseFind('/');
            if (j!=-1)
                name=v.Mid(j+1);
            v=example_path;
            v+=name;            
            vect.push_back(v);
        }
    }
    m_chemins.ResetContent();    
    chemins.clear();
    for (i=0;i<vect.size();i++) {
        m_chemins.AddString(vect[i]);
        chemins.push_back(LPCTSTR(vect[i]));
    }
    UpdateData(FALSE);

}


void Cntm::Onntmsaveas() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CString path;
    prop->reconstitueChemin(m_ntmfilename,path);
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"NTM (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {
        m_ntmfilename=fichier.GetPathName();   
        prop->rechercheChemin(m_ntmfilename,fichier.GetFileName());
        UpdateData(FALSE);
    }
    
}
