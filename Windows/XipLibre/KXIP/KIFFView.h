
// KIFFView.h : interface de la classe CKIFFView
//
#include "kif.h"
#include "kifinit.h"
#include "kifversion.h"

#pragma once

class x_colorreading : public x_reading {
public:

	vector<int> stacksize;	

	wstring parcours;
	long length;
	bool tokenstorage;
	vector<wstring> wstack;

	x_colorreading();

	void loadtoken(bool keepstacktoken = false);

	void init(wstring& s,bool ts=false) {
		tokenstorage=ts;
		pos=0;
		parcours=s;
		length=s.size();
	}

	char eof() {
		if (pos>length)
			return 1;
		return 0;
	}

	wchar_t wget() {
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

class CKIFFView;

class echange {
public:
	CString s;
	bool debugging;
	bool encours;
	bool compilation;
	HWND vue;
	CKIFFView* wvue;
	//std::thread* lethread;

	echange() {
		///lethread = NULL;
		debugging=false;
		encours=false;
		compilation=false;
		vue=NULL;
	}

	void initialisation(CKIFFView*v,CString& x);
};

class ModifItem {
public:
	CString s;
	long d,f;

	ModifItem(CString x,int D,int F): s(x) {				
		d=D;f=F;
	}
};

class CKIFFView : public CRichEditView
{
protected: // création à partir de la sérialisation uniquement
	CKIFFView();
	DECLARE_DYNCREATE(CKIFFView)

// Attributs
public:

	CKIFFDoc* GetDocument() const;
	int derniereposition;
	CString commande;
	int fincommande;
	int currentVisible;
	long dernierelongueur;
	CHARFORMAT2 cf;
	bool initcf;
	int debugxip;
	string stitre;
	void fermertab();
	CString accumulecode;
	int nbcurly;
	int nbsquare;
	bool firstdisplay;
	bool bMatchCase;
	bool bMatchWholeWord;
	bool bSearchDown;
	x_colorreading bnfxs;
	wstring filetoopen;
	CString stringtosearch;
	string nametoopen;
	list<ModifItem> listundos;
	list<ModifItem> listdos;
	map<long,bool> activecolorblocks;
	vector<long> colorblocks;
	wstring stype;
	bool suggestion;

	bool begincolor;
	vector<char> stacktype;
	void colore1000();
	void colorisation();

// Opérations
public:

// Substitutions
public:
	bool FindDeclarations(wstring& localcode, int& letype);
	CString GetLineText(long l);	
	void CouleurDepuisDebutLigne(long d,long f);
	void Keepundos(CString&);
	void Addundos(CString r,long d,long f);
	virtual void OnDraw(CDC* pDC);  // substitué pour dessiner cette vue
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	BOOL PreTranslateMessage(MSG* pMsg);
	void Colorie(long d=-1,long f=-1,int nFirstLine=-1);
	bool ApplyColor(wstring& ws, int offset, int mxpos = -1, bool rebuilt = true);
	void Chemin(CString s);
	CFindReplaceDialog *m_pFindDialog;
	CString findString;
	CString replacementString;
	bool findMode;
	void comptebrackets(CString& s);
	void Affichage(CString& s,bool thread);
	long FindCorrespondance(long d,long f,int tofind);
	long FirstChar(long d,long f,CString& commande);
	long FirstEnd(long d,long f);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	void OnInitialUpdate();
	//virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

// Implémentation
public:
	virtual ~CKIFFView();
	void runkif(CString& s);
	void executelecode();
	void Afficheerreur(string);
	void AfficheAnalyse();
	void Choixligne(string&,long l);
	bool Couleur(int r,int g,int b);
	void SelectionneLigne(long l);
	void Italique(bool);	
	void Pointarret(long l);
	LRESULT SUGGERE(WPARAM wParam, LPARAM lParam);
	LRESULT MAJ(WPARAM wParam, LPARAM lParam);
	LRESULT MAJCOULEUR(WPARAM wParam, LPARAM lParam);
	LRESULT Ouverture(WPARAM wParam, LPARAM lParam);
	LRESULT AfficheCourant(WPARAM wParam, LPARAM lParam);
	LRESULT afficher(WPARAM wParam, LPARAM lParam);
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
	afx_msg void OnKifBreak();
	afx_msg void OnCommenter();
	afx_msg void OnKifAsThread();
	afx_msg void OnKifCorrespondance();
	afx_msg void OnKifCompiler();
	afx_msg void OnChar(UINT nChar,UINT nRepCnt,UINT nFlags);
	afx_msg void OnEditionSuivant();
	afx_msg void OnKifArguments();
	afx_msg void OnEditionAllerligne();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateKifDebugxip(CCmdUI *pCmdUI);
	afx_msg void OnKifDebugxip();
	afx_msg void OnUpdateKifReexecute(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKifPasapas(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKifExecuter(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKifCompiler(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKifBreak(CCmdUI *pCmdUI);
	afx_msg void OnEditionAbandon();
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnWxipNobreak();
	afx_msg void OnUpdateWxipNobreak(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditionSuggest();
	afx_msg void OnUpdateEditionSuggest(CCmdUI *pCmdUI);
	afx_msg void OnEditSetblanks();
};

#ifndef _DEBUG  // version debug dans KIFFView.cpp
inline CKIFFDoc* CKIFFView::GetDocument() const
   { return reinterpret_cast<CKIFFDoc*>(m_pDocument); }
#endif

