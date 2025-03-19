// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__FC677033_C4AE_4256_9B1F_CB9F12E51C48__INCLUDED_)
#define AFX_MAINFRM_H__FC677033_C4AE_4256_9B1F_CB9F12E51C48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();    

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
    BOOL TreeDisplay;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAnalyseParse();
	afx_msg void OnButton32772();
	afx_msg void OnClose();
	afx_msg void OnButton32774();
	afx_msg void OnUpdateButton32774(CCmdUI* pCmdUI);
	afx_msg void OnButton32775();
	afx_msg void OnUpdateButton32775(CCmdUI* pCmdUI);
	afx_msg void OnAnalyzisReloadgrammar();
	afx_msg void OnUpdateAnalyzisReloadgrammar(CCmdUI* pCmdUI);
	afx_msg void OnButton32779();
	afx_msg void OnUpdateButton32772(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButton32779(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAnalyseParse(CCmdUI* pCmdUI);
	afx_msg void OnButtonxml();
	afx_msg void OnUpdateButtonxml(CCmdUI* pCmdUI);
	afx_msg void OnButtontxt();
	afx_msg void OnUpdateButtontxt(CCmdUI* pCmdUI);
	afx_msg void OnParsingParsetexttoxml();
	afx_msg void OnUpdateParsingParsetexttoxml(CCmdUI* pCmdUI);
	afx_msg void Oncreaterule();
	afx_msg void OnUpdatecreaterule(CCmdUI* pCmdUI);
	afx_msg void Onrulenumber();
	afx_msg void OnParsingParsefile();
	afx_msg void Onseefiles();
	afx_msg void OnUpdateseefiles(CCmdUI* pCmdUI);
	afx_msg void Oninfo();
	afx_msg void OnUpdateinfo(CCmdUI* pCmdUI);
	afx_msg void Onfeaturedisplay();
	afx_msg void OnUpdatefeaturedisplay(CCmdUI* pCmdUI);
	afx_msg void Onhelpscripting();
	afx_msg void OnXmlInput();
	afx_msg void OnUpdateXmlInput(CCmdUI* pCmdUI);
	afx_msg void OnNtmOnly();
	afx_msg void OnUpdateNtmOnly(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRuleDebugfrom0();
	afx_msg void OnUpdateRuleDebugfrom0(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonplus(CCmdUI *pCmdUI);
	afx_msg void OnButtonplus();
	afx_msg void OnButtonmoins();
	afx_msg void OnUpdateButtonmoins(CCmdUI *pCmdUI);
	void AfficheLigne(CString& l);
	afx_msg void OnReloadAndParse();
	afx_msg void OnUpdateReloadAndParse(CCmdUI *pCmdUI);
	afx_msg void OnDependanceAnalyse();
	afx_msg void OnUpdateDependanceAnalyse(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDebugdep(CCmdUI *pCmdUI);
	afx_msg void OnDebugdep();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__FC677033_C4AE_4256_9B1F_CB9F12E51C48__INCLUDED_)
