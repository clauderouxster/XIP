// kifdll.h : main header file for the kifdll DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CkifdllApp
// See kifdll.cpp for the implementation of this class
//

class CkifdllApp : public CWinApp
{
public:
	CkifdllApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
