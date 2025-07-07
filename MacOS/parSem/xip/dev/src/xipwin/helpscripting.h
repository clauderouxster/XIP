#if !defined(AFX_HELPSCRIPTING_H__EDC90255_128E_4730_8AD6_E4DCDA5AFFB4__INCLUDED_)
#define AFX_HELPSCRIPTING_H__EDC90255_128E_4730_8AD6_E4DCDA5AFFB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// helpscripting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// helpscripting dialog

class helpscripting : public CDialog
{
// Construction
public:
	helpscripting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(helpscripting)
	enum { IDD = IDD_help_script };
	CTreeCtrl	m_instructions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(helpscripting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(helpscripting)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HELPSCRIPTING_H__EDC90255_128E_4730_8AD6_E4DCDA5AFFB4__INCLUDED_)
