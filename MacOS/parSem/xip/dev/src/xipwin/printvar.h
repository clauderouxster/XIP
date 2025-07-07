#if !defined(AFX_PRINTVAR_H__C5B4A01D_65E8_4E38_8B56_3F1D125BE591__INCLUDED_)
#define AFX_PRINTVAR_H__C5B4A01D_65E8_4E38_8B56_3F1D125BE591__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// printvar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Cprintvar dialog

class Cprintvar : public CDialog
{
// Construction
public:
	Cprintvar(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Cprintvar)
	enum { IDD = IDD_variableaccess };
	CString	m_name;
	CString	m_value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cprintvar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Cprintvar)
	afx_msg void Onprintvalue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTVAR_H__C5B4A01D_65E8_4E38_8B56_3F1D125BE591__INCLUDED_)
