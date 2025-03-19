#if !defined(AFX_FILETREE_H__F936B201_2E79_4326_8179_156B624A2B90__INCLUDED_)
#define AFX_FILETREE_H__F936B201_2E79_4326_8179_156B624A2B90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Filetree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFiletree dialog

class CSaisieDoc;
class rule_file {
public:

    CString name;
    char type; //0 is feature, 1 is lexicon, 2 is rule
    CString previous;
    CSaisieDoc* doc;
    BOOL newdoc;

    rule_file(CString n,CString pr,char t,BOOL nd) {
        previous=pr;
        name=n;
        type=t;
        doc=NULL;
        newdoc=nd;
    }
};

class CFiletree : public CDialog
{
// Construction
public:
	CFiletree(CWnd* pParent = NULL);   // standard constructor
    HTREEITEM featureroot;
    HTREEITEM lexiconroot;
    HTREEITEM ruleroot;
    HTREEITEM paramroot;
	HTREEITEM diversroot;
	HTREEITEM scriptsroot;
    HTREEITEM baseroot;

    vector<string> m_feature_pathnames;
    vector<string> m_lexicon_pathnames;
    vector<string> m_rule_pathnames;
    vector<string> m_param_pathnames;
	vector<string> m_scripts_pathnames;
	vector<string> m_divers_pathnames;

    VECTA<rule_file*> m_new_documents;

    ~CFiletree() {
        m_new_documents.nettoie();
    }

// Dialog Data
	//{{AFX_DATA(CFiletree)
	enum { IDD = IDD_grammarfiletree };
	CTreeCtrl	m_pathtree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFiletree)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFiletree)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkpathfiletree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void Onnewfile();
	afx_msg void Ondeleterulefile();
	afx_msg void Onfileadding();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILETREE_H__F936B201_2E79_4326_8179_156B624A2B90__INCLUDED_)
