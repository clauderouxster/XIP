// installeurView.h : interface of the CInstalleurView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTALLEURVIEW_H__1A7DF2C8_43C7_4317_ACCA_0BA14B819297__INCLUDED_)
#define AFX_INSTALLEURVIEW_H__1A7DF2C8_43C7_4317_ACCA_0BA14B819297__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CInstalleurView : public CView
{
protected: // create from serialization only
	CInstalleurView();
	DECLARE_DYNCREATE(CInstalleurView)

// Attributes
public:
	CInstalleurDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstalleurView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInstalleurView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CInstalleurView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in installeurView.cpp
inline CInstalleurDoc* CInstalleurView::GetDocument()
   { return (CInstalleurDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTALLEURVIEW_H__1A7DF2C8_43C7_4317_ACCA_0BA14B819297__INCLUDED_)
