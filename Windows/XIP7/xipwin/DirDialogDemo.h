// DirDialogDemo.h : main header file for the DIRDIALOGDEMO application
//

#if !defined(AFX_DIRDIALOGDEMO_H__DA1AAF04_9FD7_11D3_BED6_000000000000__INCLUDED_)
#define AFX_DIRDIALOGDEMO_H__DA1AAF04_9FD7_11D3_BED6_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDirDialogDemoApp:
// See DirDialogDemo.cpp for the implementation of this class
//

class CDirDialogDemoApp : public CWinApp
{
public:
	CDirDialogDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirDialogDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDirDialogDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRDIALOGDEMO_H__DA1AAF04_9FD7_11D3_BED6_000000000000__INCLUDED_)
