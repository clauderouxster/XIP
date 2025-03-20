#if !defined(AFX_ENCRYPTAGE_H__38C2EFAB_E9E2_47E4_ACAF_CBD47699DB20__INCLUDED_)
#define AFX_ENCRYPTAGE_H__38C2EFAB_E9E2_47E4_ACAF_CBD47699DB20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// encryptage.h : header file
//
#include "xipvecta.h"
/////////////////////////////////////////////////////////////////////////////
// Cencryptage dialog

class Cencryptage : public CDialog
{
// Construction
public:
	Cencryptage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Cencryptage)
	enum { IDD = IDD_cryptage };
	CString	m_date;
	CString	m_license;
	CString	m_clef;
	CString	m_value;
	BOOL	m_chunker;
	BOOL	m_extraction;
	BOOL	m_inference;
	BOOL	m_limited;
	BOOL	m_server;
	BOOL	m_semantic;
	BOOL	m_noparameter;	
	//}}AFX_DATA

    
    void initialise(VECTA<char*>& CalculClef,char*);
    char recharge();
    VECTA<char*>* calculclef;
    CString ajoute_fichier;
    CString message;
    char affiche;
	bool sauve;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cencryptage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Cencryptage)
	afx_msg void Oncalcul();
	virtual void OnOK();
	afx_msg void Onlimited();
	afx_msg void OnUpdatevalue();
	afx_msg void Onnoparameter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENCRYPTAGE_H__38C2EFAB_E9E2_47E4_ACAF_CBD47699DB20__INCLUDED_)
