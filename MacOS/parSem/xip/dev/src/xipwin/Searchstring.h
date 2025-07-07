#if !defined(AFX_SEARCHSTRING_H__9E3790E9_613D_4158_97E3_EE5E4F324449__INCLUDED_)
#define AFX_SEARCHSTRING_H__9E3790E9_613D_4158_97E3_EE5E4F324449__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Searchstring.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSearchstring dialog

class CSearchstring : public CDialog
{
// Construction
public:
	CSearchstring(CWnd* pParent = NULL);   // standard constructor
    char find;

// Dialog Data
	//{{AFX_DATA(CSearchstring)
	enum { IDD = IDD_search };
	CString	m_searchstring;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchstring)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSearchstring)
	afx_msg void Onfind();
	afx_msg void Onfindnext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHSTRING_H__9E3790E9_613D_4158_97E3_EE5E4F324449__INCLUDED_)
