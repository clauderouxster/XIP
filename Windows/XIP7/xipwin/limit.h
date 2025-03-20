#if !defined(AFX_LIMIT_H__5E48EE55_4CE8_4667_BB99_F7A6AF01CFB3__INCLUDED_)
#define AFX_LIMIT_H__5E48EE55_4CE8_4667_BB99_F7A6AF01CFB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// limit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Climit dialog

class Climit : public CDialog
{
// Construction
public:
	Climit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Climit)
	enum { IDD = IDD_Limit };
	int		m_limit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Climit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Climit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIMIT_H__5E48EE55_4CE8_4667_BB99_F7A6AF01CFB3__INCLUDED_)
