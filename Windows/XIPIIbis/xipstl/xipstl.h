// xipdllpure.h : main header file for the XIPDLLPURE DLL
//

#if !defined(AFX_XIPDLLPURE_H__3FD64E5D_53D5_4B0D_AA33_E8A35F325220__INCLUDED_)
#define AFX_XIPDLLPURE_H__3FD64E5D_53D5_4B0D_AA33_E8A35F325220__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CXipdllpureApp
// See xipdllpure.cpp for the implementation of this class
//

class CXipdllpureApp : public CWinApp
{
public:
	CXipdllpureApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXipdllpureApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXipdllpureApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XIPDLLPURE_H__3FD64E5D_53D5_4B0D_AA33_E8A35F325220__INCLUDED_)
