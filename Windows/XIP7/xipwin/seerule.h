#if !defined(AFX_SEERULE_H__77B5B271_C5B4_470D_B729_CDF29832E738__INCLUDED_)
#define AFX_SEERULE_H__77B5B271_C5B4_470D_B729_CDF29832E738__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// seerule.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Cseerule dialog

class Cseerule : public CDialog
{
// Construction
public:
	Cseerule(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Cseerule)
	enum { IDD = IDD_seerule };
	int		m_rulenumber;
	int		m_rulenumberlast;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cseerule)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Cseerule)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEERULE_H__77B5B271_C5B4_470D_B729_CDF29832E738__INCLUDED_)
