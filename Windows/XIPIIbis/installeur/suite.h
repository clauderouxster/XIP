#if !defined(AFX_SUITE_H__022A49B4_6767_4C28_BDE8_3A2DFB977092__INCLUDED_)
#define AFX_SUITE_H__022A49B4_6767_4C28_BDE8_3A2DFB977092__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// suite.h : header file
//
class Cproprietes;
/////////////////////////////////////////////////////////////////////////////
// Csuite dialog

class Csuite : public CDialog
{
// Construction
public:
	Csuite(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Csuite)
	enum { IDD = IDD_suite };
	CString	m_param1;
	CString	m_param2;
	CString	m_param3;
	CString	m_param4;
	CString	m_param5;
	CString	m_param6;
	CString	m_param7;
	CString	m_param8;
	//}}AFX_DATA

    CString params[8];
    void versParam(CString* params);
    void deParam(CString* params,Cproprietes* p);
    Cproprietes* prop;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Csuite)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Csuite)
	afx_msg void Onexploreparameter1();
	afx_msg void Onexploreparameter2();
	afx_msg void Onexploreparameter3();
	afx_msg void Onexploreparameter4();
	afx_msg void Onexploreparameter5();
	afx_msg void Onexploreparameter6();
	afx_msg void Onexploreparameter7();
	afx_msg void Onexploreparameter8();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUITE_H__022A49B4_6767_4C28_BDE8_3A2DFB977092__INCLUDED_)
