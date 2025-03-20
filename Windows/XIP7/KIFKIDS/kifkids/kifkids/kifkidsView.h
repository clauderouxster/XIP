
// kifkidsView.h : interface de la classe CkifkidsView
//


#pragma once
#include <afxrich.h>


class CkifkidsView : public CRichEditView 
{
protected: // cr�ation � partir de la s�rialisation uniquement
	CkifkidsView();
	DECLARE_DYNCREATE(CkifkidsView)

// Attributs
public:
	CkifkidsDoc* GetDocument() const;
	CkifkidsDoc* pDoc;
	int derniereposition;
	CString commande;
	int fincommande;
	CHARFORMAT2 cf;
	
// Op�rations
public:

// Substitutions
public:
	virtual void OnDraw(CDC* pDC);  // substitu� pour dessiner cette vue
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void runkif(CString& s);
	void Couleur(int r,int g,int b);
	BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Impl�mentation
public:
	virtual ~CkifkidsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions g�n�r�es de la table des messages
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKifExecuter();
	afx_msg void OnKifReexecuter();
	afx_msg void OnKifLister();
};

#ifndef _DEBUG  // version debug dans kifkidsView.cpp
inline CkifkidsDoc* CkifkidsView::GetDocument() const
   { return reinterpret_cast<CkifkidsDoc*>(m_pDocument); }
#endif

