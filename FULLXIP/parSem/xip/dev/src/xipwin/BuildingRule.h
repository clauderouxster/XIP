#if !defined(AFX_BUILDINGRULE_H__09A162EF_05C8_4F07_96A9_222B0E907C79__INCLUDED_)
#define AFX_BUILDINGRULE_H__09A162EF_05C8_4F07_96A9_222B0E907C79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BuildingRule.h : header file
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
		// NOTE: the ClassWizard will add data members here
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

#endif // !defined(AFX_BUILDINGRULE_H__09A162EF_05C8_4F07_96A9_222B0E907C79__INCLUDED_)
