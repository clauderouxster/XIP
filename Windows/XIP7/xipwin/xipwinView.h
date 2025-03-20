// xipwinView.h : interface of the CXipwinView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XIPWINVIEW_H__CFE1525B_3D3D_4E0F_9535_A12705CDB3AC__INCLUDED_)
#define AFX_XIPWINVIEW_H__CFE1525B_3D3D_4E0F_9535_A12705CDB3AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CXipwinView : public CEditView
{
protected: // create from serialization only
	CXipwinView();
	DECLARE_DYNCREATE(CXipwinView)

// Attributes
public:
	CXipwinDoc* GetDocument();    

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXipwinView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXipwinView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CXipwinView)
	afx_msg void OnFileClose();
	afx_msg void OnEditOptions();
	afx_msg void OnUpdateEditOptions(CCmdUI* pCmdUI);
	afx_msg void Onsetlimit();
	afx_msg void Onseefiles();
	afx_msg void OnUpdateseefiles(CCmdUI* pCmdUI);
	afx_msg void Oninfo();
	afx_msg void OnUpdateinfo(CCmdUI* pCmdUI);
	afx_msg void Onfeaturedisplay();
	afx_msg void OnUpdatefeaturedisplay(CCmdUI* pCmdUI);
	afx_msg void Onloadanothergrammar();
	afx_msg void OnUpdateloadanothergrammar(CCmdUI* pCmdUI);
	afx_msg void OnViewGrammar1();
	afx_msg void OnUpdateViewGrammar1(CCmdUI* pCmdUI);
	afx_msg void OnViewGrammar2();
	afx_msg void OnUpdateViewGrammar2(CCmdUI* pCmdUI);
	afx_msg void OnViewGrammar3();
	afx_msg void OnUpdateViewGrammar3(CCmdUI* pCmdUI);
	afx_msg void OnViewGrammar4();
	afx_msg void OnUpdateViewGrammar4(CCmdUI* pCmdUI);
	afx_msg void Onhelpscripting();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in xipwinView.cpp
inline CXipwinDoc* CXipwinView::GetDocument()
   { return (CXipwinDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XIPWINVIEW_H__CFE1525B_3D3D_4E0F_9535_A12705CDB3AC__INCLUDED_)
