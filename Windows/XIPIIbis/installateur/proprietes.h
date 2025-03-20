#if !defined(AFX_PROPRIETES_H__9256835A_48F6_4DA2_A83F_9EB281B51669__INCLUDED_)
#define AFX_PROPRIETES_H__9256835A_48F6_4DA2_A83F_9EB281B51669__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// proprietes.h : header file
//

#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define MaxVars 20
/////////////////////////////////////////////////////////////////////////////
// Cproprietes dialog

class Cproprietes : public CDialog
{
// Construction
public:
	Cproprietes(CWnd* pParent = NULL);   // standard constructor
    ~Cproprietes();

// Dialog Data
	//{{AFX_DATA(Cproprietes)
	enum { IDD = IDD_proprietes };
	CString	m_hmm;
	CString	m_language;
	CString	m_ntm;
	CString	m_pathname;
	CString	m_variable1;
	CString	m_variable2;
	CString	m_variable3;
	BOOL	m_conversion;
	BOOL	m_crypte;
	CString	m_pathvar1;
	CString	m_pathvar2;
	CString	m_pathvar3;
	CString	m_variable4;
	CString	m_variable5;
	CString	m_variable6;
	CString	m_variable7;
	CString	m_variable8;
	CString	m_pathvar4;
	CString	m_pathvar5;
	CString	m_pathvar6;
	CString	m_pathvar7;
	CString	m_pathvar8;
	CString	m_variable9;
	CString	m_variable10;
	CString	m_pathvar9;
	CString	m_pathvar10;
	//}}AFX_DATA
    
    ostringstream* ntmfichier;
    CString ntmpath;
    void reconstitueChemin(CString chemin,CString& reponse,char* sep="\\");
    vector<string> parameters;
    char variables[MaxVars][1000];
    void LoadGrmFiles(CString);
    void rechercheChemin(CString& chemin,CString nom,char* sep="\\");
    CString params[MaxVars];
    void maj();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cproprietes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Cproprietes)
	afx_msg void Onexplorehmm();
	afx_msg void Onexplorentm();
	afx_msg void OnParams();
	afx_msg void Onexplorevariable1();
	afx_msg void Onexplorevariable2();
	afx_msg void Onexplorevariable3();
	afx_msg void Onexplorelanguage();
	afx_msg void OnKey();
	afx_msg void Ontowindows();
	afx_msg void OnUnix();
	afx_msg void Onsavegrmfileas();
	afx_msg void Onexplorevariable4();
	afx_msg void Onexplorevariable5();
	afx_msg void Onexplorevariable6();
	afx_msg void Onexplorevariable7();
	afx_msg void Onexplorevariable8();
	afx_msg void Onexplorevariable9();
	afx_msg void Onexplorevariable10();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPRIETES_H__9256835A_48F6_4DA2_A83F_9EB281B51669__INCLUDED_)
