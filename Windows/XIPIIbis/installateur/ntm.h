#if !defined(AFX_NTM_H__051E7876_B991_4D68_8FA5_1389EE9717AA__INCLUDED_)
#define AFX_NTM_H__051E7876_B991_4D68_8FA5_1389EE9717AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ntm.h : header file
//
#include <ostream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


using namespace std;

class Cproprietes;
/////////////////////////////////////////////////////////////////////////////
// Cntm dialog

class Cntm : public CDialog
{
// Construction
public:
	Cntm(CWnd* pParent = NULL);   // standard constructor
    CString fichierntm;
    CString amorce;
    char chargement();
    vector<string> lexiques;
    vector<string> chemins;
    Cproprietes* prop;
    BOOL initial;
    unsigned char buffer[10000];
    char* debutfinbuffer;
    void maj();
    BOOL lecture;
// Dialog Data
	//{{AFX_DATA(Cntm)
	enum { IDD = IDD_ntmscript };
	CListBox	m_lexique;
	CListBox	m_chemins;
	CString	m_ntmfilename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cntm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
    
protected:

	// Generated message map functions
	//{{AFX_MSG(Cntm)
	afx_msg void Onexplore();
	afx_msg void Ondelete();
	afx_msg void Onadd();
	virtual void OnOK();
	afx_msg void OnSelchangechemins();
	afx_msg void OnSelchangelexique();
	afx_msg void Onntmtowindows();
	afx_msg void Onntmtounix();
	afx_msg void Onadaptpath();
	afx_msg void Onntmsaveas();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NTM_H__051E7876_B991_4D68_8FA5_1389EE9717AA__INCLUDED_)
