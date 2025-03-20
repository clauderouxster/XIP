// xipdll.h : main header file for the XIPDLL DLL
//

#if !defined(AFX_XIPDLL_H__7EB67949_7216_4E79_B731_29B37706804B__INCLUDED_)
#define AFX_XIPDLL_H__7EB67949_7216_4E79_B731_29B37706804B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CXipdllApp
// See xipdll.cpp for the implementation of this class
//

class CXipdllApp : public CWinApp
{
public:
	CXipdllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXipdllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXipdllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XIPDLL_H__7EB67949_7216_4E79_B731_29B37706804B__INCLUDED_)
