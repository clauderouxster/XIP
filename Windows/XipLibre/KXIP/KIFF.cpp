 
// KIFF.cpp : D�finit les comportements de classe pour l'application.
//

#include "stdafx.h"

#include "afxwinappex.h"
#include "afxdialogex.h"
#include "KIFF.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "KIFFDoc.h"
#include "KIFFView.h"
#include "kifversion.h"
#include "xipstlres.h"
#include "wkiflibs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CKIFFApp

BEGIN_MESSAGE_MAP(CKIFFApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CKIFFApp::OnAppAbout)
	// Commandes de fichier standard
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Commande standard de configuration de l'impression
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_Menu32865, &CKIFFApp::OnMenu32865)
END_MESSAGE_MAP()


// construction CKIFFApp

CKIFFApp::CKIFFApp()
{
	m_bHiColorIcons = TRUE;

	// prend en charge le Gestionnaire de red�marrage
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// Si l'application est cr��e � l'aide de la prise en charge Common Language Runtime (/clr):
	//     1) Ce param�tre suppl�mentaire est n�cessaire � la prise en charge du Gestionnaire de red�marrage.
	//     2) Dans votre projet, vous devez ajouter une r�f�rence � System.Windows.Forms pour la g�n�ration.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: remplacer la cha�ne d'ID de l'application ci-dessous par une cha�ne ID unique�; le format recommand�
	// pour la cha�ne est CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("KXIP.AppID.NoVersion"));

	// TODO: ajoutez ici du code de construction,
	// Placez toutes les initialisations significatives dans InitInstance
}

// Seul et unique objet CKIFFApp

CKIFFApp theApp;

static HKEY OpenKey(HKEY hRootKey, wchar_t* strKey) {
	HKEY hKey;
	LONG nError = RegOpenKeyEx(hRootKey, strKey, NULL, KEY_ALL_ACCESS, &hKey);

	if (nError == ERROR_FILE_NOT_FOUND)
	{
		nError = RegCreateKeyEx(hRootKey, strKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	}

	return hKey;
}

static inline void SetValKey(HKEY hKey, LPCTSTR lpValue, LPCTSTR data,int sz)
{
	DWORD pathLenInBytes = 4 * sz;
	RegSetValueEx(hKey, lpValue, NULL, REG_SZ, (LPBYTE)data, pathLenInBytes);
}


wstring GetKeyVal(wstring key)
{
	HKEY happkey = theApp.GetAppRegistryKey();
	HKEY hKey = OpenKey(happkey, (wchar_t*)key.c_str());
	TCHAR value[2048];
	value[0] = 0;
	DWORD value_length = 2048;
	DWORD res = 1;
	DWORD ret = RegQueryValueEx(hKey, key.c_str(), NULL, &res, (LPBYTE)&value, &value_length);
	RegCloseKey(hKey);
	return value;
}

void SetKeyVal(wstring key, CString value)
{
	HKEY happkey = theApp.GetAppRegistryKey();
	HKEY hKey = OpenKey(happkey, (wchar_t*)key.c_str());
	SetValKey(hKey, key.c_str(), value, value.GetLength());
	RegCloseKey(hKey);
}


static wstring GetValKey(HKEY hKey)
{
	WCHAR value[2048];
	value[0] = 0;
	DWORD value_length = 2048;
	DWORD res = 1;
	RegQueryValueEx(hKey, L"KIFLIBS", NULL, &res, (LPBYTE)&value, &value_length);
	return value;
}

static void SetKIFLIBS(CString& chemin) {
	HKEY happkey = theApp.GetAppRegistryKey();
	HKEY hKey = OpenKey(happkey, L"KIFLIBS");
	SetValKey(hKey, L"KIFLIBS", chemin, chemin.GetLength());
	RegCloseKey(hKey);
	string command("KIFLIBS=");
	string chm;
	s_unicode_to_utf8(chm, (wchar_t*)LPCWSTR(chemin));
	command += chm;
	_putenv(STR(command));
}

static void InitKIFLIBS() {
	HKEY happkey = theApp.GetAppRegistryKey();
	HKEY hKey = OpenKey(happkey, L"KIFLIBS");
	wstring val = GetValKey(hKey);

	if (val.size() == 0) {
		//If KIFLIBS was not set originaly
		char localpath[4096];
		_fullpath(localpath, ".", 4096);
		if (localpath[strlen(localpath) - 1] != '\\')
			strcat(localpath, "\\");
		sc_utf8_to_unicode(val, localpath);
		CString chemin = val.c_str();
		SetKIFLIBS(chemin);
	}
	else
		RegCloseKey(hKey);

	string chm;
	if (val.size()) {
		string command("KIFLIBS=");
		s_unicode_to_utf8(chm, val);
		command += chm;
		_putenv(STR(command));
	}

	string pth = getenv("Path");
	if (pth.find(chm) == string::npos) {
		pth = "Path=" + chm + ";" + pth;
		_putenv(STR(pth));
	}	

	val = GetKeyVal(L"SZBLANKS");
	if (val == L"") {
		CString sz = L"5";
		SetKeyVal(L"SZBLANKS", sz);
	}
}


// initialisation de CKIFFApp

BOOL CKIFFApp::InitInstance()
{
	// InitCommonControlsEx() est requis sur Windows�XP si le manifeste de l'application
	// sp�cifie l'utilisation de ComCtl32.dll version�6 ou ult�rieure pour activer les
	// styles visuels.  Dans le cas contraire, la cr�ation de fen�tres �chouera.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// � d�finir pour inclure toutes les classes de contr�les communs � utiliser
	// dans votre application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialiser les biblioth�ques OLE
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	
	//A UTILISER ABSOLUMENT SINON PLANTAGE ALEATOIRE. Gere la concurence entre THREADS
	//CoInitializeEx(NULL,COINIT_MULTITHREADED);

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// AfxInitRichEdit2() est requis pour utiliser un contr�le RichEdit	
	// AfxInitRichEdit2();

	// Initialisation standard
	// Si vous n'utilisez pas ces fonctionnalit�s et que vous souhaitez r�duire la taille
	// de votre ex�cutable final, vous devez supprimer ci-dessous
	// les routines d'initialisation sp�cifiques dont vous n'avez pas besoin.
	// Changez la cl� de Registre sous laquelle nos param�tres sont enregistr�s
	// TODO: modifiez cette cha�ne avec des informations appropri�es,
	// telles que le nom de votre soci�t� ou organisation
	SetRegistryKey(_T("KXIP"));
	
	LoadStdProfileSettings(10);  // Charge les options de fichier INI standard (y compris les derniers fichiers utilis�s)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Inscrire les mod�les de document de l'application. Ces mod�les
	//  lient les documents, fen�tres frame et vues entre eux
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_KIFFTYPE,
		RUNTIME_CLASS(CKIFFDoc),
		RUNTIME_CLASS(CChildFrame), // frame enfant MDI personnalis�
		RUNTIME_CLASS(CKIFFView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// cr�e la fen�tre frame MDI principale
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// appelle DragAcceptFiles uniquement s'il y a un suffixe
	//  Dans une application MDI, cet appel doit avoir lieu juste apr�s la d�finition de m_pMainWnd
	// Activer les ouvertures via glisser-d�placer
	m_pMainWnd->DragAcceptFiles();

	// Analyser la ligne de commande pour les commandes shell standard, DDE, ouverture de fichiers
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Activer les ouvertures d'ex�cution DDE
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	if (cmdInfo.m_nShellCommand==CCommandLineInfo::FileOpen)
		OnFileNew();

	// Commandes de dispatch sp�cifi�es sur la ligne de commande. Retournent FALSE si
	// l'application a �t� lanc�e avec /RegServer, /Register, /Unregserver ou /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// La fen�tre principale a �t� initialis�e et peut donc �tre affich�e et mise � jour
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	InitKIFLIBS();
	//On tente alors de lire KIFLIBS, dont on se servira pour indiquer le r�pertoire des libs KIF.
	return TRUE;
}


int CKIFFApp::ExitInstance()
{
	//TODO: g�rez les ressources suppl�mentaires que vous avez ajout�es
	AfxOleTerm(FALSE);
	//CoUninitialize();
	return CWinAppEx::ExitInstance();
}

// gestionnaires de messages pour CKIFFApp


// bo�te de dialogue CAboutDlg utilis�e pour la bo�te de dialogue '� propos de' pour votre application

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Donn�es de bo�te de dialogue
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

// Impl�mentation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	CString verssion;
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
	, verssion(_T(""))
{
	string s;
	Whoami(s);
	verssion+=(char*)s.c_str();
#ifdef ARCH64
	verssion+="     KiF(64): ";
#else
	verssion+="     KiF(32): ";
#endif
	verssion+=KIFVERSION;

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, verssion);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &CAboutDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()

// Commande App pour ex�cuter la bo�te de dialogue
void CKIFFApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CKIFFApp, m�thodes de chargement/d'enregistrement de la personnalisation

void CKIFFApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CKIFFApp::LoadCustomState()
{
}

void CKIFFApp::SaveCustomState()
{
}

// gestionnaires de messages pour CKIFFApp



//int main (int argc, char *argv[]) {}




void CAboutDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CKIFFApp::OnMenu32865()
{
	// TODO: Add your command handler code here

	wkiflibs getpath;
	HKEY happkey = theApp.GetAppRegistryKey();
	HKEY hKey = OpenKey(happkey, L"KIFLIBS");
	wstring val = GetValKey(hKey);
	RegCloseKey(hKey);
	getpath.kiflibspath = val.c_str();
	if (getpath.DoModal() == IDOK)
		SetKIFLIBS(getpath.kiflibspath);
}
