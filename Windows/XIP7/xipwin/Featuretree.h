#if !defined(AFX_FEATURETREE_H__94D72C67_EA6D_4496_A54E_9A2DD61CF607__INCLUDED_)
#define AFX_FEATURETREE_H__94D72C67_EA6D_4496_A54E_9A2DD61CF607__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Featuretree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFeaturetree dialog


class CFeaturetree : public CDialog
{
// Construction
public:
	CFeaturetree(CWnd* pParent = NULL);   // standard constructor
    void Construction(arbretrait* ar,HTREEITEM root);

// Dialog Data
	//{{AFX_DATA(CFeaturetree)
	enum { IDD = IDD_featuretree };
	CTreeCtrl	m_tree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFeaturetree)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFeaturetree)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkTREEfeatures(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FEATURETREE_H__94D72C67_EA6D_4496_A54E_9A2DD61CF607__INCLUDED_)
