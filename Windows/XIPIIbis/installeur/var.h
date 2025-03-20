#if !defined(AFX_VAR_H__BB52F282_765E_4148_9532_C6CD8969CE51__INCLUDED_)
#define AFX_VAR_H__BB52F282_765E_4148_9532_C6CD8969CE51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// var.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Cvar dialog

class Cvar : public CDialog
{
// Construction
public:
	Cvar(CWnd* pParent = NULL);   // standard constructor
    
    
// Dialog Data
	//{{AFX_DATA(Cvar)
	enum { IDD = IDD_DIALOG1 };
	CString	m_var;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cvar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Cvar)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VAR_H__BB52F282_765E_4148_9532_C6CD8969CE51__INCLUDED_)
