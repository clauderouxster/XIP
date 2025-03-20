#if !defined(AFX_LISTEVARIABLE_H__A741ABC7_C0F6_46DD_8757_ACFD16B24CCD__INCLUDED_)
#define AFX_LISTEVARIABLE_H__A741ABC7_C0F6_46DD_8757_ACFD16B24CCD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListeVariable.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListeVariable dialog

class CListeVariable : public CDialog
{
// Construction
public:
	CListeVariable(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CListeVariable)
	enum { IDD = IDD_ListeVariables };
	CListBox	m_variables;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListeVariable)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CListeVariable)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	afx_msg void OnDblclklistevar();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTEVARIABLE_H__A741ABC7_C0F6_46DD_8757_ACFD16B24CCD__INCLUDED_)
