
// KIFNView.h : interface de la classe CKIFNView
//
#include "kif.h"
#include "kifinit.h"
#include "kifversion.h"

#pragma once

class CKIFNView;

class x_colorreading : public x_reading {
public:

	vector<int> stacksize;	

	wstring parcours;
	long length;


	x_colorreading();

	void loadtoken(bool keepspace=false);

	void init(wstring& s) {
		pos=0;
		parcours=s;
		length=s.size();
	}

	char eof() {
		if (pos>length)
			return 1;
		return 0;
	}

	char get() {
		if (pos>length)
			return -1;
		if (pos==length) {
			pos++;
			return -1;
		}
		return parcours[pos++];
	}

	bool wdigitonly(wstring& thestr) {
		int i=0;
		static char tabs[]={'0','1','2','3','4','5','6','7','8','9','.',0};
		if (thestr[i]=='-')
			i++;
		for (;i<thestr.size();i++) 
			if (strchr(tabs,thestr[i])==NULL)
				return false;		
		return true;
	}

};

class echange {
public:
	CString s;
	bool encours;
	bool pasapas;
	bool compilation;
	HWND vue;
	CKIFNView* wvue;

	echange() {
		encours=false;
		pasapas=false;
		compilation=false;
		vue=NULL;
	}

	void initialisation(CKIFNView*v,CString& x);
};

class CKIFNView : public CRichEditView
{
protected: // création à partir de la sérialisation uniquement
	CKIFNView();
	DECLARE_DYNCREATE(CKIFNView)

// Attributs
public:
	CKIFNDoc* GetDocument() const;
	int derniereposition;
	CString commande;
	int fincommande;
	long dernierelongueur;
	CHARFORMAT2 cf;
	bool initcf;
	string stitre;
	map<long,BOOL> vbreaks;
	echange e;
	bool bMatchCase;
	bool bMatchWholeWord;
	bool bSearchDown;
	x_colorreading bnfxs;

// Opérations
public:

// Substitutions
public:
	virtual void OnDraw(CDC* pDC);  // substitué pour dessiner cette vue
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	BOOL PreTranslateMessage(MSG* pMsg);
	void Colorie(long d=-1,long f=-1,int nFirstVisible=-1);
	void Chemin(CString s);
	CFindReplaceDialog *m_pFindDialog;
	CString FindName;
	CString accumulecode;
	int nbcurly;
	int nbsquare;
	void comptebrackets(CString& s);
	void Affichage(CString& s,bool);
	void Executekif(CString& s);
	void ApplyColor(wstring& sw,int offset);
	long FindCorrespondance(long d,long f,int tofind);
	long FirstChar(long d,long f,CString&);
	long FirstEnd(long d,long f);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

// Implémentation
public:
	virtual ~CKIFNView();
	void runkif(CString& s);
	void executelecode();
	void Afficheerreur();
	void AfficheAnalyse();
	void Choixligne();
	void Couleur(int r,int g,int b);
	void SelectionneLigne(long l);
	void Italique(bool);	
	void Pointarret(long l);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	
// Fonctions générées de la table des messages
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKifExecuter();
	afx_msg void OnKifReexecuter();
	afx_msg void OnKifLister();
	afx_msg void OnKifRaz();
	afx_msg void OnKifPasaPas();
	afx_msg void OnKifPointarret();
	afx_msg void OnKifNettoyerp();
	afx_msg void OnKifIndenter();
	afx_msg LRESULT OnFindDialogMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEditionCherche();
	afx_msg void OnEditionRemplace();
	afx_msg void OnKifCommenter();
	afx_msg void OnKifBreak();
	afx_msg void OnKifAsThread();
	afx_msg void OnKifCorrespondance();
	afx_msg void OnKifCompiler();
	afx_msg void OnChar(UINT nChar,UINT nRepCnt,UINT nFlags);
	afx_msg void OnEditionSuivant();
	afx_msg void OnEditionArguments();
	afx_msg void OnEditionAllerligne();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
};
#ifndef _DEBUG  // version debug dans KIFNView.cpp
inline CKIFNDoc* CKIFNView::GetDocument() const
   { return reinterpret_cast<CKIFNDoc*>(m_pDocument); }
#endif

