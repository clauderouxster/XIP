#if !defined(AFX_GRM_H__66920B1D_2311_427B_AC64_EF0B8A05063A__INCLUDED_)
#define AFX_GRM_H__66920B1D_2311_427B_AC64_EF0B8A05063A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// grm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Cgrm dialog

class Cgrm : public CDialog
{
// Construction
public:
	Cgrm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Cgrm)
	enum { IDD = IDD_grm };
	CButton	m_explore_path_name;
	CString	m_pathname;
	CString	m_xipkeyfile;
	//}}AFX_DATA

	void Compute();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cgrm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Cgrm)
	afx_msg void Onexplorepathname();
	afx_msg void Onexplorexipkey();
	afx_msg void OnKey();
	afx_msg void OnKeyCrypted();
	afx_msg void OnModify();
	afx_msg void OnExit();
	afx_msg void Onconversion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRM_H__66920B1D_2311_427B_AC64_EF0B8A05063A__INCLUDED_)
