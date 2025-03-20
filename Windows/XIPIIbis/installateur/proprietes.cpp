// proprietes.cpp : implementation file
//

#include "stdafx.h"

#include "installateur.h"
#include "proprietes.h"
#include "suite.h"
#include "ntm.h"
#include "xipvecta.h"
#include "encryptage.h"

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
	m_variable4 = _T("");
	m_variable5 = _T("");
	m_variable6 = _T("");
	m_variable7 = _T("");
	m_variable8 = _T("");
	m_pathvar4 = _T("");
	m_pathvar5 = _T("");
	m_pathvar6 = _T("");
	m_pathvar7 = _T("");
	m_pathvar8 = _T("");
	m_variable9 = _T("");
	m_variable10 = _T("");
	m_pathvar9 = _T("");
	m_pathvar10 = _T("");
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
	DDX_Text(pDX, IDC_variable4, m_variable4);
	DDX_Text(pDX, IDC_variable5, m_variable5);
	DDX_Text(pDX, IDC_variable6, m_variable6);
	DDX_Text(pDX, IDC_variable7, m_variable7);
	DDX_Text(pDX, IDC_variable8, m_variable8);
	DDX_Text(pDX, IDC_pathvar4, m_pathvar4);
	DDX_Text(pDX, IDC_pathvar5, m_pathvar5);
	DDX_Text(pDX, IDC_pathvar6, m_pathvar6);
	DDX_Text(pDX, IDC_pathvar7, m_pathvar7);
	DDX_Text(pDX, IDC_pathvar8, m_pathvar8);
	DDX_Text(pDX, IDC_variable9, m_variable9);
	DDX_Text(pDX, IDC_variable10, m_variable10);
	DDX_Text(pDX, IDC_pathvar9, m_pathvar9);
	DDX_Text(pDX, IDC_pathvar10, m_pathvar10);
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
	ON_BN_CLICKED(IDKEY, OnKey)
	ON_BN_CLICKED(IDC_towindows, Ontowindows)
	ON_BN_CLICKED(IDC_UNIX, OnUnix)
	ON_BN_CLICKED(IDC_savegrmfileas, Onsavegrmfileas)
	ON_BN_CLICKED(IDC_explore_variable4, Onexplorevariable4)
	ON_BN_CLICKED(IDC_explore_variable5, Onexplorevariable5)
	ON_BN_CLICKED(IDC_explore_variable6, Onexplorevariable6)
	ON_BN_CLICKED(IDC_explore_variable7, Onexplorevariable7)
	ON_BN_CLICKED(IDC_explore_variable8, Onexplorevariable8)
	ON_BN_CLICKED(IDC_explore_variable9, Onexplorevariable9)
	ON_BN_CLICKED(IDC_explore_variable10, Onexplorevariable10)
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
    for (int j=0;j<MaxVars;j++)
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
    for (int i=0;i<MaxVars;i++) {
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
            case 3:
                m_variable4=variables[3]; 
                m_pathvar4=pt+1;
                if (!fait)
                    SetCurrentDirectory(m_pathvar4);
                fait=1;
                break;
            case 4:
                m_variable5=variables[4]; 
                m_pathvar5=pt+1;
                if (!fait)
                    SetCurrentDirectory(m_pathvar5);
                fait=1;
                break;
            case 5:
                m_variable6=variables[5]; 
                m_pathvar6=pt+1;
                if (!fait)
                    SetCurrentDirectory(m_pathvar6);
                fait=1;
                break;
            case 6:
                m_variable7=variables[6]; 
                m_pathvar7=pt+1;
                if (!fait)
                    SetCurrentDirectory(m_pathvar7);
                fait=1;
                break;
            case 7:
                m_variable8=variables[7]; 
                m_pathvar8=pt+1;
                if (!fait)
                    SetCurrentDirectory(m_pathvar8);
                fait=1;
                break;
            case 8:
                m_variable9=variables[8]; 
                m_pathvar9=pt+1;
                if (!fait)
                    SetCurrentDirectory(m_pathvar9);
                fait=1;
                break;
            case 9:
                m_variable10=variables[9]; 
                m_pathvar10=pt+1;
                if (!fait)
                    SetCurrentDirectory(m_pathvar10);
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
        else
            if (var==m_variable3)
                reponse=m_pathvar3;
            else
                if (var==m_variable4)
                    reponse=m_pathvar4;
                else
                    if (var==m_variable5)
                        reponse=m_pathvar5;
                    else
                        if (var==m_variable6)
                            reponse=m_pathvar6;
                        else
                            if (var==m_variable7)
                                reponse=m_pathvar7;       
                            else
                                if (var==m_variable8)
                                    reponse=m_pathvar8;        
                                else
                                    if (var==m_variable9)
                                        reponse=m_pathvar9;      
                                    else
                                        if (var==m_variable10)
                                            reponse=m_pathvar10;                        
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
            else {
                if (m_pathvar4!="" && chem.Find(m_pathvar4,0)==0) {
                    chemin="$"+m_variable4;
                    chemin+=chem.Right(chem.GetLength()-m_pathvar4.GetLength());
                    chemin+=sep;       
                    chemin+=nom;
                }
                else {
                    if (m_pathvar5!="" && chem.Find(m_pathvar5,0)==0) {
                        chemin="$"+m_variable5;
                        chemin+=chem.Right(chem.GetLength()-m_pathvar5.GetLength());
                        chemin+=sep;       
                        chemin+=nom;
                    }
                    else {
                        if (m_pathvar6!="" && chem.Find(m_pathvar6,0)==0) {
                            chemin="$"+m_variable6;
                            chemin+=chem.Right(chem.GetLength()-m_pathvar6.GetLength());
                            chemin+=sep;       
                            chemin+=nom;
                        }
                        else {
                            if (m_pathvar7!="" && chem.Find(m_pathvar7,0)==0) {
                                chemin="$"+m_variable7;
                                chemin+=chem.Right(chem.GetLength()-m_pathvar7.GetLength());
                                chemin+=sep;       
                                chemin+=nom;
                            }
                            else {
                                if (m_pathvar8!="" && chem.Find(m_pathvar8,0)==0) {
                                    chemin="$"+m_variable8;
                                    chemin+=chem.Right(chem.GetLength()-m_pathvar8.GetLength());
                                    chemin+=sep;       
                                    chemin+=nom;
                                }
                                else {
                                    if (m_pathvar9!="" && chem.Find(m_pathvar9,0)==0) {
                                        chemin="$"+m_variable9;
                                        chemin+=chem.Right(chem.GetLength()-m_pathvar9.GetLength());
                                        chemin+=sep;       
                                        chemin+=nom;
                                    }
                                    else {
                                        if (m_pathvar10!="" && chem.Find(m_pathvar10,0)==0) {
                                            chemin="$"+m_variable10;
                                            chemin+=chem.Right(chem.GetLength()-m_pathvar10.GetLength());
                                            chemin+=sep;       
                                            chemin+=nom;
                                        }
                                    }
                                    
                                }
                                
                            }
                            
                        }
                        
                    }
                    
                }
                
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

void Cproprietes::OnKey() 
{
	// TODO: Add your control notification handler code here
    VECTA<char*> CalculClef;
    char utilise[1000];
    utilise[0]=0;
    int debut,fin;
    CString path;
    reconstitueChemin(m_language,path);

    if (ExtraitClef((char*)LPCTSTR(path),debut,fin,CalculClef,utilise)==1) {
        Cencryptage encrypte;
        encrypte.initialise(CalculClef,utilise);
        if (encrypte.DoModal()==IDOK && encrypte.m_clef!="") {            
            if (InsereClef((char*)LPCTSTR(path),encrypte.m_clef,debut,fin,CalculClef,utilise)==1) {
                CString message="File:"+path+" has been modified successfully";
                AfxMessageBox(message);                
            }
        }
    }

    for (int imo=0;imo<CalculClef.dernier;imo++)
        delete[] CalculClef[imo];
    CalculClef.raz();
	
}


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


void Cproprietes::Ontowindows() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
	replace(m_hmm,'/','\\',1);
	replace(m_language,'/','\\',1);
	replace(m_ntm,'/','\\',1);
	replace(m_variable1,'/','\\',1); 
	replace(m_variable2,'/','\\',1);
	replace(m_variable3,'/','\\',1);
	replace(m_variable4,'/','\\',1);
	replace(m_variable5,'/','\\',1);
	replace(m_variable6,'/','\\',1);
	replace(m_variable7,'/','\\',1);
	replace(m_variable8,'/','\\',1);
	replace(m_variable9,'/','\\',1);
	replace(m_variable10,'/','\\',1);
	replace(m_pathvar1,'/','\\',1);
	replace(m_pathvar2,'/','\\',1);
	replace(m_pathvar3,'/','\\',1);
	replace(m_pathvar4,'/','\\',1);
	replace(m_pathvar5,'/','\\',1);
	replace(m_pathvar6,'/','\\',1);
	replace(m_pathvar7,'/','\\',1);
	replace(m_pathvar8,'/','\\',1);
	replace(m_pathvar9,'/','\\',1);
	replace(m_pathvar10,'/','\\',1);
    replace(ntmpath,'/','\\',1);
    UpdateData(FALSE);
	
}

void Cproprietes::OnUnix() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
	replace(m_hmm,'\\','/',1);
	replace(m_language,'\\','/',1);
	replace(m_ntm,'\\','/',1);
	replace(m_variable1,'\\','/',1); 
	replace(m_variable2,'\\','/',1);
	replace(m_variable3,'\\','/',1);
	replace(m_variable4,'\\','/',1);
	replace(m_variable5,'\\','/',1);
	replace(m_variable6,'\\','/',1);
	replace(m_variable7,'\\','/',1);
	replace(m_variable8,'\\','/',1);
	replace(m_variable9,'\\','/',1);
	replace(m_variable10,'\\','/',1);
	replace(m_pathvar1,'\\','/',1);
	replace(m_pathvar2,'\\','/',1);
	replace(m_pathvar3,'\\','/',1);
	replace(m_pathvar4,'\\','/',1);
	replace(m_pathvar5,'\\','/',1);
	replace(m_pathvar6,'\\','/',1);
	replace(m_pathvar7,'\\','/',1);
	replace(m_pathvar8,'\\','/',1);
	replace(m_pathvar9,'\\','/',1);
	replace(m_pathvar10,'\\','/',1);
    replace(ntmpath,'\\','/',1);
    UpdateData(FALSE);	
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

void Cproprietes::Onexplorevariable4() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    // TODO: Add your control notification handler code here
    CString path=m_pathvar4+"\\*.*";
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"DIR (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {        
        m_pathvar4=fichier.GetPathName();
        CString ch="\\";
        ch+=fichier.GetFileName();        
        int debut=m_pathvar4.Find(ch);
        m_pathvar4.Delete(debut,ch.GetLength());
        maj();
        UpdateData(FALSE);
    }	
}

void Cproprietes::Onexplorevariable5() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    // TODO: Add your control notification handler code here
    CString path=m_pathvar5+"\\*.*";
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"DIR (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {        
        m_pathvar5=fichier.GetPathName();
        CString ch="\\";
        ch+=fichier.GetFileName();        
        int debut=m_pathvar5.Find(ch);
        m_pathvar5.Delete(debut,ch.GetLength());
        maj();
        UpdateData(FALSE);
    }	
}

void Cproprietes::Onexplorevariable6() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    // TODO: Add your control notification handler code here
    CString path=m_pathvar6+"\\*.*";
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"DIR (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {        
        m_pathvar6=fichier.GetPathName();
        CString ch="\\";
        ch+=fichier.GetFileName();        
        int debut=m_pathvar6.Find(ch);
        m_pathvar6.Delete(debut,ch.GetLength());
        maj();
        UpdateData(FALSE);
    }	
}

void Cproprietes::Onexplorevariable7() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    // TODO: Add your control notification handler code here
    CString path=m_pathvar7+"\\*.*";
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"DIR (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {        
        m_pathvar7=fichier.GetPathName();
        CString ch="\\";
        ch+=fichier.GetFileName();        
        int debut=m_pathvar7.Find(ch);
        m_pathvar7.Delete(debut,ch.GetLength());
        maj();
        UpdateData(FALSE);
    }	
}

void Cproprietes::Onexplorevariable8() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    // TODO: Add your control notification handler code here
    CString path=m_pathvar8+"\\*.*";
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"DIR (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {        
        m_pathvar8=fichier.GetPathName();
        CString ch="\\";
        ch+=fichier.GetFileName();        
        int debut=m_pathvar8.Find(ch);
        m_pathvar8.Delete(debut,ch.GetLength());
        maj();
        UpdateData(FALSE);
    }	
}


void Cproprietes::Onexplorevariable9() 
{
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    // TODO: Add your control notification handler code here
    CString path=m_pathvar9+"\\*.*";
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"DIR (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {        
        m_pathvar9=fichier.GetPathName();
        CString ch="\\";
        ch+=fichier.GetFileName();        
        int debut=m_pathvar9.Find(ch);
        m_pathvar9.Delete(debut,ch.GetLength());
        maj();
        UpdateData(FALSE);
    }	

}

void Cproprietes::Onexplorevariable10() 
{
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    // TODO: Add your control notification handler code here
    CString path=m_pathvar10+"\\*.*";
    CFileDialog fichier(TRUE,NULL,path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"DIR (*.*)|*.*|");	
    if (fichier.DoModal()==IDOK) {        
        m_pathvar10=fichier.GetPathName();
        CString ch="\\";
        ch+=fichier.GetFileName();        
        int debut=m_pathvar10.Find(ch);
        m_pathvar10.Delete(debut,ch.GetLength());
        maj();
        UpdateData(FALSE);
    }	

}
