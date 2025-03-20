// proprietes.cpp : implementation file
//

#include "stdafx.h"

#include "installateur.h"
#include "proprietes.h"
#include "suite.h"
#include "ntm.h"
#include "xipvecta.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Cproprietes dialog


Cproprietes::Cproprietes(CWnd* pParent /*=NULL*/)
	: CDialog(Cproprietes::IDD, pParent)
{
	//{{AFX_DATA_INIT(Cproprietes)
	m_hmm = _T("");
	m_language = _T("");
	m_ntm = _T("");
	m_pathname = _T("");
	m_variable1 = _T("");
	m_variable2 = _T("");
	m_variable3 = _T("");
	m_conversion = FALSE;
	m_crypte = FALSE;
	m_pathvar1 = _T("");
	m_pathvar2 = _T("");
	m_pathvar3 = _T("");
	//}}AFX_DATA_INIT
    ntmpath="";
    ntmfichier=NULL;
}


void Cproprietes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cproprietes)
	DDX_Text(pDX, IDC_hmm, m_hmm);
	DDX_Text(pDX, IDC_language, m_language);
	DDX_Text(pDX, IDC_ntm, m_ntm);
	DDX_Text(pDX, IDC_pathname, m_pathname);
	DDX_Text(pDX, IDC_variable1, m_variable1);
	DDX_Text(pDX, IDC_variable2, m_variable2);
	DDX_Text(pDX, IDC_variable3, m_variable3);
	DDX_Check(pDX, IDC_CHECK1, m_conversion);
	DDX_Check(pDX, IDC_CHECK2, m_crypte);
	DDX_Text(pDX, IDC_pathvar1, m_pathvar1);
	DDX_Text(pDX, IDC_pathvar2, m_pathvar2);
	DDX_Text(pDX, IDC_pathvar3, m_pathvar3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Cproprietes, CDialog)
	//{{AFX_MSG_MAP(Cproprietes)
	ON_BN_CLICKED(IDC_explore_hmm, Onexplorehmm)
	ON_BN_CLICKED(IDC_explore_ntm, Onexplorentm)
	ON_BN_CLICKED(IDC_PARAMS, OnParams)
	ON_BN_CLICKED(IDC_explore_variable1, Onexplorevariable1)
	ON_BN_CLICKED(IDC_explore_variable2, Onexplorevariable2)
	ON_BN_CLICKED(IDC_explore_variable3, Onexplorevariable3)
	ON_BN_CLICKED(IDC_explore_language, Onexplorelanguage)
	ON_BN_CLICKED(IDC_savegrmfileas, Onsavegrmfileas)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cproprietes message handlers

#define modeOuverture ios::in|ios::binary

Cproprietes::~Cproprietes() {
    if (ntmfichier!=NULL)
        delete ntmfichier;
}


void GrmRead(ifstream& grm,char* buf,int max) {
    int i=0;
    char c=0;
    while (i<max && !grm.eof()) {
        grm.get(c);
        if (c==13 || c==10 || c=='\n')
            break;
        buf[i++]=c;
    }
    buf[i]=0;
}       

void GrmTrim(ifstream& grm,char* buf,int max,char norm) {
    char chemin[1000];
    char boucle=1;
    buf[0]=0;
    GrmRead(grm,chemin,max);
    int l;
    int i;
    while (!grm.eof()) {           
        l=strlen(chemin);
        i=l-1;
        while (i>=0 && chemin[i]<=32)
            chemin[i--]=0;
        i=0;
        while (chemin[i]<=32 && chemin[i]!=0)
            i++;
        if (i)
            strcpy(chemin,chemin+i);
        if (chemin[0]!=0 && chemin[0]!='#')
            break;
        GrmRead(grm,chemin,max);
    }    
    strcpy(buf,chemin);
}

void Cproprietes::LoadGrmFiles(CString grmname) {
    
    char bufread[1000];
    
    ifstream grm(LPCSTR(grmname),modeOuverture);
    if (grm.fail())
        return;    

    int ivar=0;
    int ipar=0;
    int crypte=0;
    for (int j=0;j<10;j++)
        variables[j][0]=0;

    char conversion=0;
    while (!grm.eof()) {
        GrmTrim(grm,bufread,999,0);
        
        if (!*bufread)
            break;

        //First line is grammar
        //Second line is ntm
        //thrid line is hmm
        //following lines are parameter files            
        if (!strcmp(bufread,"conversion:")) {
            GrmTrim(grm,bufread,999,1);            
            if (!strcmp(bufread,"utf82ascii"))
                conversion=1;
            continue;
        }


        if (!strcmp(bufread,"path:")) {
            GrmTrim(grm,variables[ivar++],999,1);
            continue;
        }

        if (!strcmp(bufread,"ntm:")) {
            GrmTrim(grm,bufread,999,1);            
            if (bufread[0]!=0) {
                m_ntm=bufread;            
            }
            continue;
        }
        
        if (!strcmp(bufread,"hmm:")) {
            GrmTrim(grm,bufread,999,1);
            if (bufread[0]!=0) {
                m_hmm=bufread;                            
            }
            continue;
        }

        if (!strcmp(bufread,"grammar:") || 
            !strcmp(bufread,"gram:")) {
            if (!strcmp(bufread,"gram:"))
                crypte=1;

            GrmTrim(grm,bufread,999,1);
            if (bufread[0]!=0) {
                m_language=bufread;
            }
            continue;
        }

        if (!strcmp(bufread,"parameter:")) {
            GrmTrim(grm,bufread,999,1);
            if (bufread[0]!=0)
                params[ipar++]=bufread;
            continue;
        }

    }

    m_conversion=conversion;
    m_crypte=crypte;

    //On recupere les morceaux:
    char fait=0;
    for (int i=0;i<3;i++) {
        char* pt=strchr(variables[i],'=');
        if (pt!=NULL) {
            *pt=0;
            switch(i) {
            case 0:
                m_variable1=variables[0]; 
                m_pathvar1=pt+1;
                SetCurrentDirectory(m_pathvar1);
                fait=1;
                break;
            case 1:
                m_variable2=variables[1]; 
                m_pathvar2=pt+1;
                if (!fait)
                    SetCurrentDirectory(m_pathvar2);
                fait=1;
                break;
            case 2:
                m_variable3=variables[2]; 
                m_pathvar3=pt+1;
                if (!fait)
                    SetCurrentDirectory(m_pathvar3);
                fait=1;
                break;
            }
        }
    }
}

void Cproprietes::maj() {    
    CString nm;
    int psd;

    if (m_ntm!="" && m_ntm[0]!='$') {        
        psd=m_ntm.ReverseFind('\\');
        nm=m_ntm.Right(m_ntm.GetLength()-psd-1);
        rechercheChemin(m_ntm,nm);                
    }

    if (m_language!="" && m_language[0]!='$') {
        psd=m_language.ReverseFind('\\');
        nm=m_language.Right(m_language.GetLength()-psd-1);
        rechercheChemin(m_language,nm);                
    }

    if (m_hmm!="" && m_hmm[0]!='$') {
        psd=m_hmm.ReverseFind('\\');
        nm=m_hmm.Right(m_hmm.GetLength()-psd-1);
        rechercheChemin(m_hmm,nm);                
    }
}


void Cproprietes::reconstitueChemin(CString chemin,CString& reponse,char* sep) {        
    if (chemin[0]!='$') {
        reponse=chemin;
        return;
    }

    int pos=chemin.Find(sep,0);
    CString var=chemin.Left(pos);
    var=var.Right(var.GetLength()-1);
    if (var==m_variable1)
        reponse=m_pathvar1;
    else {
        if (var==m_variable2)
            reponse=m_pathvar2;
        else {
            if (var==m_variable3)
                reponse=m_pathvar3;
        }
    }
    reponse+=chemin.Right(chemin.GetLength()-pos);

}


void Cproprietes::rechercheChemin(CString& chemin,CString nom,char* sep) {        
    CString ch="\\";
    ch+=nom;
    CString chem=chemin;
    int debut=chem.Find(ch);
    chem.Delete(debut,ch.GetLength());    

    if (m_pathvar1!="" && chem.Find(m_pathvar1,0)==0) {
        chemin="$"+m_variable1;
        chemin+=chem.Right(chem.GetLength()-m_pathvar1.GetLength());
        chemin+=sep;       
        chemin+=nom;
    }
    else {
        if (m_pathvar2!="" && chem.Find(m_pathvar2,0)==0) {
            chemin="$"+m_variable2;
            chemin+=chem.Right(chem.GetLength()-m_pathvar2.GetLength());
            chemin+=sep;       
            chemin+=nom;
        }
        else {
            if (m_pathvar3!="" && chem.Find(m_pathvar3,0)==0) {
                chemin="$"+m_variable3;
                chemin+=chem.Right(chem.GetLength()-m_pathvar3.GetLength());
                chemin+=sep;       
                chemin+=nom;
            }
        }
    }
}

void Cproprietes::Onexplorehmm() 
{
    UpdateData(TRUE);
    CString path;
    reconstitueChemin(m_hmm,path);
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"HMM (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {
        m_hmm=fichier.GetPathName();   
        rechercheChemin(m_hmm,fichier.GetFileName());
        UpdateData(FALSE);
    }
}


void Cproprietes::Onexplorentm() 
{
    UpdateData(TRUE);
    CString path;
    reconstitueChemin(m_ntm,path);
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"NTM (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {        
        m_ntm=fichier.GetPathName();
        rechercheChemin(m_ntm,fichier.GetFileName());
        Cntm ntm;
        ntm.fichierntm=fichier.GetPathName();
        ntm.prop=this;
        if (ntm.chargement()==0) {
            AfxMessageBox("Error: Unknown file",MB_OK);
            return;
        }

        
        //On regenere notre fichier ntmscript
        if (ntm.DoModal()==IDOK) {
            if (ntmfichier!=NULL)
                delete ntmfichier;
            ntmfichier=new ostringstream;
            ntmpath=fichier.GetPathName();
            *ntmfichier<<ntm.buffer<<endl;
            int tt=ntm.chemins.size();
            int titi=ntm.lexiques.size();
            for (int i=0;i<ntm.chemins.size();i++) {
                *ntmfichier<<ntm.lexiques[i]<<"\t\t"<<ntm.chemins[i]<<endl;
            }
            *ntmfichier<<ntm.debutfinbuffer<<endl;
            m_ntm=ntm.m_ntmfilename;
            ntmpath=m_ntm;
            int dernier=m_ntm.ReverseFind('\\');
            CString nom=m_ntm.Right(m_ntm.GetLength()-dernier-1);
            rechercheChemin(m_ntm,nom);
        }

        UpdateData(FALSE);
    }
}

void Cproprietes::OnParams() 
{

    UpdateData(TRUE);
    Csuite dparam;
    dparam.deParam(params,this);    
    if (dparam.DoModal()==IDOK) {
        dparam.versParam(params);
        UpdateData(FALSE);
    }

}

void Cproprietes::Onexplorevariable1() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CString path=m_pathvar1+"\\*.*";
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"DIR (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {        
        m_pathvar1=fichier.GetPathName();
        CString ch="\\";
        ch+=fichier.GetFileName();        
        int debut=m_pathvar1.Find(ch);
        m_pathvar1.Delete(debut,ch.GetLength());
        maj();
        //On remet a jour les autres chemins
        UpdateData(FALSE);
    }
	
}

void Cproprietes::Onexplorevariable2() 
{
	UpdateData(TRUE);
    // TODO: Add your control notification handler code here
    CString path=m_pathvar2+"\\*.*";
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"DIR (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {        
        m_pathvar2=fichier.GetPathName();
        CString ch="\\";
        ch+=fichier.GetFileName();        
        int debut=m_pathvar2.Find(ch);
        m_pathvar2.Delete(debut,ch.GetLength());
        maj();
        UpdateData(FALSE);
    }
	
}

void Cproprietes::Onexplorevariable3() 
{
	UpdateData(TRUE);
    // TODO: Add your control notification handler code here
    CString path=m_pathvar3+"\\*.*";
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"DIR (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {        
        m_pathvar3=fichier.GetPathName();
        CString ch="\\";
        ch+=fichier.GetFileName();        
        int debut=m_pathvar3.Find(ch);
        m_pathvar3.Delete(debut,ch.GetLength());
        maj();
        UpdateData(FALSE);
    }
	
}

void Cproprietes::Onexplorelanguage() 
{
    UpdateData(TRUE);
    CString path;
    reconstitueChemin(m_language,path);
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"XIP (*.xip)|*.xip|");	
    if (fichier.DoModal()==IDOK) {
        m_language=fichier.GetPathName();   
        rechercheChemin(m_language,fichier.GetFileName());
        UpdateData(FALSE);
    }
	
}

int ExtraitClef(char* nomfichier,int& debut,int& fin,VECTA<char*>& CalculClef,char*);
char InsereClef(char* nomfichier, CString clef,int debut,int fin,VECTA<char*>& CalculClef,char*);



void replace(CString& var,char i,char n,char remp) {    
    if (i=='\\') {
        //From windows to unix        
        CString inter=var;
        inter.MakeLower();
        if (inter.Left(9)=="\\\\smbhost") {
            inter=var.Right(var.GetLength()-9);
            var="/home";
            var+=inter;            
        }
    }
    else
        var.Replace("/home","\\\\smbhost");

    if (remp==1)
        var.Replace(i,n);
}


void Cproprietes::Onsavegrmfileas() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CString path;
    reconstitueChemin(m_pathname,path);
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"GRM (*.grm)|*.grm|");	
    if (fichier.DoModal()==IDOK) {
        m_pathname=fichier.GetPathName();   
        rechercheChemin(m_pathname,fichier.GetFileName());
        UpdateData(FALSE);
    }
	
}
