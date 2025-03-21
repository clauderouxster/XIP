// saisieview.h : interface of the CSaisieView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAISIEWINVIEW_H__CFE1525B_3D3D_4E0F_9535_A12705CDB3AC__INCLUDED_)
#define AFX_SAISIEWINVIEW_H__CFE1525B_3D3D_4E0F_9535_A12705CDB3AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
using namespace std;
#include <string>
#include "xipstlres.h"
#include "xipvecta.h"
#include "generate.h"


class LastAdded {
public:

    int index;
    int max;
    int increment;
    int layer;
    BOOL new_rule;
    BOOL removed;
    Label* lab_removed;

    LastAdded(int ind,int m, int inc,int l,BOOL nr) {
        index=ind;
        max=m;
        increment=inc;
        new_rule=nr;
        layer=l;
        removed=FALSE;
        lab_removed=NULL;
    }
};

class CSaisieView : public CEditView
{
protected: // create from serialization only
	CSaisieView();
	DECLARE_DYNCREATE(CSaisieView)

// Attributes
public:
	CSaisieDoc* GetDocument();    
    unsigned char* buffer;
    long size;
    CString searchstring;
    long position;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaisieView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	void OnLButtonUp(UINT nFlags,CPoint point);
	void OnKeyUp(UINT nChar,UINT nRepCnt,UINT nFlags);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSaisieView();
    void Selection(long first,long last,int index);
	void SelectionOnString(CString& ruletext);
    void InsertText(CString& rule,long first,long last,int index,typeRegle,int rlayer,int alayer);
    void FindBorne(long first,long last,long& firstsel,long& lastsel,int index);
    void Renumber(int baselayer,int index,int sens,int increment,BOOL);
    int GetLayerNumber(long first,long last,int index);
    void AddRule(CString&);
    void FindPreviousField(char** fields,int nbfields,CString& field);
    void FindAndSelectString(CString& s);
    void DeleteRule(long first,long last,int index);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSaisieView)
	afx_msg void OnFileClose();			
	afx_msg void OnSearchinrule();
    afx_msg void OnSearchagain();
    afx_msg void OnSearchRulenumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in xipwinView.cpp
inline CSaisieDoc* CSaisieView::GetDocument()
   { return (CSaisieDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XIPWINVIEW_H__CFE1525B_3D3D_4E0F_9535_A12705CDB3AC__INCLUDED_)
