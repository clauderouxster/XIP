#if !defined(AFX_PARAMETREFILE_H__9807FAFD_CB25_43F3_8705_EB6F879F21D2__INCLUDED_)
#define AFX_PARAMETREFILE_H__9807FAFD_CB25_43F3_8705_EB6F879F21D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParametreFile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParametreFile dialog
#include <ostream>
using namespace std;
#include <vector>

class CParametreFile : public CDialog
{
// Construction
public:
	CParametreFile(CWnd* pParent = NULL);   // standard constructor

    BOOL create;
    CString filename;
    vector<CString> filenames;

// Dialog Data
	//{{AFX_DATA(CParametreFile)
	enum { IDD = IDD_parameterfile };
	CComboBox	m_listfilewindow;
	CString	m_rule;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParametreFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParametreFile)
	afx_msg void Onload();
	virtual BOOL OnInitDialog();	
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAMETREFILE_H__9807FAFD_CB25_43F3_8705_EB6F879F21D2__INCLUDED_)
