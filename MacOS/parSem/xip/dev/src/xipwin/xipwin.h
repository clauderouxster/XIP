// xipwin.h : main header file for the XIPWIN application
//

#if !defined(AFX_XIPWIN_H__39AC8157_924C_4BD8_A9AC_F57579A911C3__INCLUDED_)
#define AFX_XIPWIN_H__39AC8157_924C_4BD8_A9AC_F57579A911C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CXipwinApp:
// See xipwin.cpp for the implementation of this class
//

class CXipwinApp : public CWinApp
{
public:
	CXipwinApp(LPCTSTR lpszAppName = NULL);
    void Creation();
    void ParseCommandLine(CCommandLineInfo& rCmdInfo);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXipwinApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CXipwinApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XIPWIN_H__39AC8157_924C_4BD8_A9AC_F57579A911C3__INCLUDED_)
