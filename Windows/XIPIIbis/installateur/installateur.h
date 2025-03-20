// installeur.h : main header file for the INSTALLEUR application
//

#if !defined(AFX_INSTALLEUR_H__A616A1C7_A6C4_44C7_BBF9_06AE42FDD22F__INCLUDED_)
#define AFX_INSTALLEUR_H__A616A1C7_A6C4_44C7_BBF9_06AE42FDD22F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CInstalleurApp:
// See installeur.cpp for the implementation of this class
//

class CInstalleurApp : public CWinApp
{
public:
	CInstalleurApp();
    void ParseCommandLine(CCommandLineInfo& rCmdInfo);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstalleurApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CInstalleurApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTALLEUR_H__A616A1C7_A6C4_44C7_BBF9_06AE42FDD22F__INCLUDED_)
