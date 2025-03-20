
// KIFF.h : fichier d'en-tête principal pour l'application KIFF
//
#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"       // symboles principaux


// CKIFFApp:
// Consultez KIFF.cpp pour l'implémentation de cette classe
//

class CKIFFApp : public CWinAppEx
{
public:
	CKIFFApp();


// Substitutions
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implémentation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFichierSetkiflibs();
};

extern CKIFFApp theApp;
