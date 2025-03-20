
// kifkids.h : fichier d'en-tête principal pour l'application kifkids
//
#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"       // symboles principaux


// CkifkidsApp:
// Consultez kifkids.cpp pour l'implémentation de cette classe
//

class CkifkidsApp : public CWinAppEx
{
public:
	CkifkidsApp();


// Substitutions
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void ParseCommandLine(CCommandLineInfo& rCmdInfo);

// Implémentation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CkifkidsApp theApp;
