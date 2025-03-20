
// kifkidsView.cpp : implémentation de la classe CkifkidsView
//

#include "stdafx.h"
// SHARED_HANDLERS peuvent être définis dans les gestionnaires d'aperçu, de miniature
// et de recherche d'implémentation de projet ATL et permettent la partage de code de document avec ce projet.
#ifndef SHARED_HANDLERS
#include "kifkids.h"
#endif

#include "MainFrm.h"
#include "kif.h"
#include "kifinit.h"
#include "kifversion.h"

#include "kifkidsDoc.h"
#include "kifkidsView.h"
extern "C" {
#include "kifextern.h"
}

#define ENDL "\r\n"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CMainFrame* laMain;

// CkifkidsView
	
IMPLEMENT_DYNCREATE(CkifkidsView, CRichEditView)

BEGIN_MESSAGE_MAP(CkifkidsView, CRichEditView )
	// Commandes d'impression standard
	ON_COMMAND(ID_FILE_PRINT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CkifkidsView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_KIF_EXECUTER, &CkifkidsView::OnKifExecuter)
	ON_COMMAND(ID_KIF_REEXECUTER, &CkifkidsView::OnKifReexecuter)
	ON_COMMAND(ID_KIF_LISTER, &CkifkidsView::OnKifLister)
END_MESSAGE_MAP()

// construction ou destruction de CkifkidsView

CkifkidsView::CkifkidsView()
{
	// TODO: ajoutez ici du code de construction
	derniereposition=0;
	pDoc = NULL;
	kifinitialisation();
}

CkifkidsView::~CkifkidsView()
{
	 closefinalkif();
}

BOOL CkifkidsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: changez ici la classe ou les styles Window en modifiant
	//  CREATESTRUCT cs

	return CRichEditView::PreCreateWindow(cs);
}

// dessin de CkifkidsView

void CkifkidsView::OnDraw(CDC* /*pDC*/)
{
	pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ajoutez ici le code de dessin pour les données natives
}


// impression de CkifkidsView


void CkifkidsView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CkifkidsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// préparation par défaut
	return DoPreparePrinting(pInfo);
}

void CkifkidsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ajoutez une initialisation supplémentaire avant l'impression
}

void CkifkidsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ajoutez un nettoyage après l'impression
}

void CkifkidsView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CkifkidsView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// diagnostics pour CkifkidsView

#ifdef _DEBUG
void CkifkidsView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CkifkidsView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CkifkidsDoc* CkifkidsView::GetDocument() const // la version non Debug est inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CkifkidsDoc)));
	return (CkifkidsDoc*)m_pDocument;
}
#endif //_DEBUG


// gestionnaires de messages pour CkifkidsView

static KifCode* kcode=NULL;
extern KifCode* premierkifcode;

void CkifkidsView::runkif(CString& s) {	
	if (s!="") {
		openkifsession();
		//char* code=wcstombs( pmbhello, pwchello, MB_CUR_MAX );
		string lecode;
		wchar_t* base=(wchar_t*)LPCWSTR(s);
		s_unicodetoutf8(lecode,base);
		int i=compilekifsession(STR(lecode));
		if (i!=-1)
			runkifsession();
	}
}

void CkifkidsView::Couleur(int r,int g,int b) {
	cf = GetCharFormatSelection();
	cf.dwMask|=CFM_COLOR;
	cf.dwEffects&=~CFE_AUTOCOLOR;
	cf.crTextColor=RGB(r,g,b);
	SetCharFormat (cf);
}

void CkifkidsView::OnKifReexecuter() {
	closekifsession();
	OnKifExecuter();
}


void CkifkidsView::OnKifLister() {
	int nLength;
	CRichEditCtrl& e=GetRichEditCtrl();
	nLength = e.GetWindowTextLength();
	// put the selection at the end of text
	e.SetSel(nLength, nLength);
	Couleur(0,0,255);
	// replace the selection
	char* res=listecodes();
	wstring base;
	s_utf8tounicode(base,res);
	e.ReplaceSel((WCHAR*)base.c_str());	
	derniereposition=e.GetWindowTextLength();	
	e.SetSel(derniereposition,derniereposition);
	Couleur(0,0,255);
}

void CkifkidsView::OnKifExecuter()
{
	CString txt;
	int nLength;
	CRichEditCtrl& e=GetRichEditCtrl();
	long d,f;
	CString s;

	if (commande=="") {
		e.GetSel(d,f);
		GetWindowText(txt);
		s=txt.Mid(d,f-d);	
		s=s.Trim();
	}
	else {
		s=commande;
		commande="";
	}

	BOOL selection=FALSE;
	if (s=="") {
		f=txt.GetLength();
		d=derniereposition;
		if (d>=f) {
			derniereposition=0;
			d=0;
		}
		s=txt.Mid(d,f-d);		
		selection=TRUE;
		s=s.Trim();		
	}

	if (pDoc==NULL)
		pDoc = GetDocument();

	if (s=="videcran") {
		WCHAR b[]={0,0};
		e.SetWindowText(b);
		return;
	}
	if (s=="vidememoire") {
		pDoc->contenu="";
		pDoc->premier=TRUE;
		closekifsession();
		return;
	}

	runkif(s);

	if (s!="") {		
		s+=ENDL;
		if (pDoc->premier) {
			pDoc->premier=FALSE;
			pDoc->contenu=s;
		}
		else
			pDoc->contenu+=s;
		nLength = e.GetWindowTextLength();
		// put the selection at the end of text
		e.SetSel(nLength, nLength);
		// replace the selection
		char* res=displaykifsession();
		if (res!=NULL && res[0]!=0) {
			wstring base;
			s_utf8tounicode(base,res);
			e.ReplaceSel((WCHAR*)base.c_str());	
			derniereposition=e.GetWindowTextLength();
			e.SetSel(f,derniereposition);
			Couleur(255,0,0);
		}
		if (selection)
			e.SetSel(d,f);
	}
}


BOOL CkifkidsView::PreTranslateMessage(MSG* pMsg) {
	long d,f;
	ostringstream os;
	CRichEditCtrl& e=GetRichEditCtrl();
	e.GetSel(d,f);
	int l=e.LineFromChar(f);
	char ch[20];
	sprintf(ch,"%d",l+1);
	CString c;
	c=ch;
	laMain->AfficheLigne(c);
	return CRichEditView::PreTranslateMessage(pMsg); 
}

