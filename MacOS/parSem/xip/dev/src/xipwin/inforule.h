#if !defined(AFX_INFORULE_H__0472DDDB_4EFE_4F6F_A751_F3E3C8503CAB__INCLUDED_)
#define AFX_INFORULE_H__0472DDDB_4EFE_4F6F_A751_F3E3C8503CAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// inforule.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Cinforule dialog
class DecompteRegle;
#include "xipvecta.h"
class datatosort {
public:
    DecompteRegle* dr;
    int index;
    datatosort(DecompteRegle* d,int i) {
        dr=d;
        index=i;
    }
};

class Cinforule : public CDialog
{
// Construction
public:
	Cinforule(CWnd* pParent = NULL);   // standard constructor
    BOOL removecomment;
    long start_rule;
    BOOL ascending;
    VECTA<datatosort*> datas;

    ~Cinforule() {
        datas.nettoie();
    }
// Dialog Data
	//{{AFX_DATA(Cinforule)
	enum { IDD = IDD_inforule };
	CListCtrl	m_list;
	CString	m_ruletext;
	int		m_selection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cinforule)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support    
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Cinforule)
	virtual BOOL OnInitDialog();
	afx_msg void OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedEdit();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFORULE_H__0472DDDB_4EFE_4F6F_A751_F3E3C8503CAB__INCLUDED_)
