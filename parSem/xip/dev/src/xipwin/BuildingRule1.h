#if !defined(AFX_BUILDINGRULE1_H__B077B00F_06EA_41AE_8C30_8E79186D36CD__INCLUDED_)
#define AFX_BUILDINGRULE1_H__B077B00F_06EA_41AE_8C30_8E79186D36CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BuildingRule1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// BuildingRule dialog

class BuildingRule : public CDialog
{
// Construction
public:
	BuildingRule(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(BuildingRule)
	enum { IDD = IDD_resultatanalyse };
	CTreeCtrl	m_arbre;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BuildingRule)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(BuildingRule)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUILDINGRULE1_H__B077B00F_06EA_41AE_8C30_8E79186D36CD__INCLUDED_)
