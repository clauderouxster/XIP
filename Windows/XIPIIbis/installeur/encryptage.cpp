// encryptage.cpp : implementation file
//

#include "stdafx.h"
#include "installateur.h"
#include "encryptage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Cencryptage dialog


Cencryptage::Cencryptage(CWnd* pParent /*=NULL*/)
	: CDialog(Cencryptage::IDD, pParent)
{
	//{{AFX_DATA_INIT(Cencryptage)
	m_date = _T("");
	m_license = _T("");
	m_clef = _T("");
	m_value = _T("");
	m_chunker = FALSE;
	m_extraction = FALSE;
	m_inference = FALSE;
	m_limited = FALSE;
	m_server = FALSE;
	m_semantic = FALSE;
	m_noparameter = FALSE;
	//}}AFX_DATA_INIT
    
    ajoute_fichier=_T("");
    affiche=1;
}

void Cencryptage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cencryptage)
	DDX_Text(pDX, IDC_date, m_date);
	DDX_Text(pDX, IDC_license, m_license);
	DDX_Text(pDX, IDC_clef, m_clef);
	DDX_Text(pDX, IDC_value, m_value);
	DDX_Check(pDX, IDC_chunker, m_chunker);
	DDX_Check(pDX, IDC_extraction, m_extraction);
	DDX_Check(pDX, IDC_inference, m_inference);
	DDX_Check(pDX, IDC_limited, m_limited);
	DDX_Check(pDX, IDC_server, m_server);
	DDX_Check(pDX, IDC_semantic, m_semantic);
	DDX_Check(pDX, IDC_noparameter, m_noparameter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Cencryptage, CDialog)
	//{{AFX_MSG_MAP(Cencryptage)
	ON_BN_CLICKED(IDC_calcul, Oncalcul)
	ON_BN_CLICKED(IDC_limited, Onlimited)
	ON_EN_UPDATE(IDC_value, OnUpdatevalue)
	ON_BN_CLICKED(IDC_noparameter, Onnoparameter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cencryptage message handlers



char Test(CString mod) {
    if (mod=="chunker")
        return 0;   
    if (mod=="inference")
        return 1;
    if (mod=="extraction")
        return 2;
    if (mod=="semantic")
        return 3;
    if (mod=="server")
        return 4;
    if (!memcmp((char*)LPCTSTR(mod),(char*)"limited",7))
        return 5;
    if (mod=="noparameter")
        return 6;
    return -1;
}

void Cencryptage::initialise(VECTA<char*>& CalculClef,char* utilise) {
    calculclef=&CalculClef;
    m_license=CalculClef[0];
    m_date=CalculClef.fin();
    ajoute_fichier=utilise;
    
	m_chunker = FALSE;
	m_extraction = FALSE;
	m_inference = FALSE;
	m_limited = FALSE;
	m_server = FALSE;
	m_semantic = FALSE;
	m_noparameter = FALSE;

    for (int i=1;i<CalculClef.dernier-1;i++) {
        CString val=CalculClef[i];
        if (val!=ajoute_fichier) {
            int test=Test(val);
            switch(test) {
            case 0:
                m_chunker=TRUE;
                break;
            case 1:
                m_inference=TRUE;
                break;
            case 2:
                m_extraction=TRUE;
                break;
            case 3:
                m_semantic=TRUE;
                break;
            case 4:
                m_server=TRUE;
                break;
            case 5:
                m_limited=TRUE;
                m_value=CalculClef[i]+8;
                break;
            case 6:
                m_noparameter=TRUE;
                break;
            }            
        }
    }            
}

char* DupliqueChaine(char* m);
#define duplique(x) DupliqueChaine((char*)LPCTSTR(x))

void ajout(VECTA<char*>* clef,CString fichier,CString val) {
    if (val==fichier)
        return;
    clef->ajoute(duplique(val));
}


char Cencryptage::recharge() {
    UpdateData(TRUE);

    int i;
    for (i=0;i<calculclef->dernier;i++)
        delete[]  (*calculclef)[i];
    calculclef->raz();    

    calculclef->ajoute(duplique(m_license));    

    
    char ajoute=0;

    if (m_chunker==TRUE)
        calculclef->ajoute(DupliqueChaine((char*)"chunker"));
	if (m_extraction == TRUE) 
        calculclef->ajoute(DupliqueChaine((char*)"extraction"));
	if (m_inference == TRUE) 
        calculclef->ajoute(DupliqueChaine((char*)"inference"));
    
    if (m_limited == TRUE) {
        if (m_value=="") {
            message="There is no limit value for the limited field";
            if (affiche==1)
                AfxMessageBox(message);
            return 0;
        }
        CString lim="limited:"+m_value;
        calculclef->ajoute(duplique(lim));
        ajoute=1;
    }
	
    if (m_server == TRUE) 
        calculclef->ajoute(DupliqueChaine((char*)"server"));
	
    if (m_semantic == TRUE) 
        calculclef->ajoute(DupliqueChaine((char*)"semantic"));
    
    if (m_noparameter == TRUE) {
        calculclef->ajoute(DupliqueChaine((char*)"noparameter"));
        ajoute=1;
    }

    if (ajoute==1)
        calculclef->ajoute(duplique(ajoute_fichier));

    calculclef->ajoute(duplique(m_date));
    return 1;
}

char VerifieEtCompare(char* tampon,CString& message);
void CheckCRC(VECTA<char*>& contents,CString& clefresultat);

void Cencryptage::Oncalcul() 
{
	// TODO: Add your control notification handler code here    
    message="";    
    if (recharge()==0)
        return;
    m_clef="";
    char da[100];    
    strcpy(da,(char*)LPCTSTR(m_date));
    if (VerifieEtCompare(da,message)==0) {
        m_clef=message;
        UpdateData(FALSE);
        m_clef="";
        return;
    }

    CheckCRC(*calculclef,m_clef);
    UpdateData(FALSE);
}

void Cencryptage::OnOK() 
{
	// TODO: Add extra validation here
    affiche=0;
    Oncalcul();
    UpdateData(FALSE);
    if (message!="") {
        AfxMessageBox(message);
        affiche=1;
    }
    else
        CDialog::OnOK();
}

void Cencryptage::Onlimited() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);    
    if (m_limited==FALSE)
        m_value="";
    CButton* b= (CButton*)GetDlgItem(IDC_noparameter);
    b->SetCheck(FALSE);
    m_noparameter=FALSE;
    UpdateData(FALSE);
}


void Cencryptage::OnUpdatevalue() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
    if (m_limited==FALSE){
        m_limited=TRUE;        
        CButton* b= (CButton*)GetDlgItem(IDC_limited);
        b->SetCheck(TRUE);     
        b= (CButton*)GetDlgItem(IDC_noparameter);
        b->SetCheck(FALSE);
        m_noparameter=FALSE;        
    }
}

void Cencryptage::Onnoparameter() 
{
    UpdateData(TRUE);    
    if (m_noparameter==TRUE) {
        CButton* b= (CButton*)GetDlgItem(IDC_limited);
        b->SetCheck(FALSE);
        m_limited=FALSE;
        m_value="";
        UpdateData(FALSE);
    }	
}
