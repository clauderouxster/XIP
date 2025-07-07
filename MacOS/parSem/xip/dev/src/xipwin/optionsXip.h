#include "afxwin.h"
#if !defined(AFX_OPTIONSXIP_H__4E552FC6_3364_449A_9DEC_0CF1613D67BD__INCLUDED_)
#define AFX_OPTIONSXIP_H__4E552FC6_3364_449A_9DEC_0CF1613D67BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// optionsXip.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CoptionsXip dialog

class CoptionsXip : public CDialog
{
// Construction
public:
	CoptionsXip(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CoptionsXip)
	enum { IDD = IDD_OPTIONSXIP };
	BOOL	m_categories;
	BOOL	m_full;
	BOOL	m_lemme;
	BOOL	m_offset;
	BOOL	m_reduit;
	BOOL	m_sentence;
	BOOL	m_surface;
	BOOL	m_wordnumb;
	BOOL	m_utf8;
	BOOL	m_dependency;
	BOOL	m_maths;
	BOOL	m_rulenumber;
	BOOL	m_inpututf8;
	BOOL	m_detectlanguage;
	BOOL	m_debugmode;
	//}}AFX_DATA

    BOOL m_obname;
    BOOL m_obnode;
    BOOL m_obcreation;    

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CoptionsXip)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CoptionsXip)
	afx_msg void Onbycreation();
	afx_msg void Onbyname();
	afx_msg void Onbynode();
	afx_msg void Onlemme();
	afx_msg void Onsurface();
	afx_msg void Onoffset();
	afx_msg void Onwordnumb();
	afx_msg void Onfull();
	afx_msg void Onreduit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int upto;
	afx_msg void OnBnClickedOk();
	CEdit pythonmodule;
	BOOL pythondebugmode;
	int modulepython;
	int lignepython;
	BOOL tagger_mode;
	int edittest;
	int edittestvaleur;
	BOOL output_enable;
	afx_msg void OnBnClickedCheck2();
	BOOL select_algo;
	afx_msg void OnBnClickeddisplayfeaturevaluedep();
	BOOL display_feature_value;
	afx_msg void OnBnClickedMultwrddisplay();
	BOOL MULTIWRDDISPLAY;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSXIP_H__4E552FC6_3364_449A_9DEC_0CF1613D67BD__INCLUDED_)
