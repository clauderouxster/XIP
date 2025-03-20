
// KIFNDoc.cpp : implémentation de la classe CKIFNDoc
//

#include "stdafx.h"
// SHARED_HANDLERS peuvent être définis dans les gestionnaires d'aperçu, de miniature
// et de recherche d'implémentation de projet ATL et permettent la partage de code de document avec ce projet.
#ifndef SHARED_HANDLERS
#include "KIFFN.h"
#endif

#include "afxwinappex.h"

#include "KIFNDoc.h"
#include "MainFrm.h"

#include <propkey.h>

#include "kif.h"
#include "KIFNView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CKIFNDoc

IMPLEMENT_DYNCREATE(CKIFNDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CKIFNDoc, CRichEditDoc)
END_MESSAGE_MAP()


// construction ou destruction de CKIFNDoc

CKIFNDoc::CKIFNDoc()
{
	// TODO: ajoutez ici le code d'une construction unique

}

CKIFNDoc::~CKIFNDoc()
{
}


CKIFNDoc* firstdoc=NULL;

BOOL CKIFNDoc::OnNewDocument()
{

	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

	// TODO: ajoutez ici le code de réinitialisation
	// (les documents SDI réutiliseront ce document)

	if (firstdoc==NULL) {
		firstdoc=this;		
		chemin="Console";
		SetTitle(chemin);
	}
	return TRUE;
}


extern CKIFNView* firstview;
BOOL CKIFNDoc::OnOpenDocument(LPCTSTR lpszPathName) {
	
	BOOL v=CRichEditDoc::OnOpenDocument(lpszPathName);
	if (v) {		
		chemin=lpszPathName;
		POSITION pos = GetFirstViewPosition();
		CKIFNView* vue= (CKIFNView*)GetNextView(pos);
		vue->Chemin(lpszPathName);
		vue->SetWindowText(contenu);    
	}
	if (firstdoc==NULL)
		firstview=NULL;
	return v;
}


extern "C" {
	bool closefinalkif();
	bool kifencours();
}

void ExitMFCApp()
{
	// same as double-clicking on main window close box
	ASSERT(AfxGetMainWnd() != NULL);
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}


void CKIFNDoc::OnCloseDocument()
{
	bool dest=false;
	if (firstdoc==this) {
		dest=closefinalkif();
		SetModifiedFlag(FALSE);
	}

	CRichEditDoc::OnCloseDocument();
	if (firstdoc==this) {
		firstdoc=NULL;
		firstview=NULL;
		if (dest)
			ExitMFCApp();
	}
}

BOOL CKIFNDoc::OnSaveDocument(LPCTSTR lpszPathName) {
	if (CRichEditDoc::OnSaveDocument(lpszPathName)==TRUE) {
		chemin=lpszPathName;
		POSITION pos = GetFirstViewPosition();
		CKIFNView* vue= (CKIFNView*)GetNextView(pos);
		vue->Chemin(lpszPathName);
		vue->Colorie();
		return TRUE;
	}
	return FALSE;
}

// sérialisation de CKIFNDoc

void CKIFNDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())	{
		// TODO: ajoutez ici le code de stockage
		CFile* fichier=ar.GetFile();
		string s;
		//Nous sauvegardons en utf8 pour etre lisible sur d'autres plate-formes
		POSITION pos = GetFirstViewPosition();
		CView* vue= GetNextView(pos);
		vue->GetWindowText(contenu);    
		s_unicodetoutf8(s,(wchar_t*)LPCTSTR(contenu));
		fichier->Write((char*)(s.c_str()),s.size());
	}
	else	{
		// TODO: ajoutez ici le code de chargement
		CString toRead;
		CString s;
		contenu="";
		//Nous sauvegardons en utf8 pour etre lisible sur d'autres plate-formes
		//mais lui lit en UNICODE
		while (ar.ReadString(toRead)==TRUE) {
			if (toRead.GetLength() > 0)
				toRead+="\r\n";
			s += toRead;
		}
		//Donc nous convertissons un unicode qui est en fait de l'utf8 (lu double octet par double octet)
		//short par short...
		wstring cs;
		s_doubleutf8tounicode(cs,(wchar_t*)LPCTSTR(s));
		contenu=cs.c_str();
	}
}

#ifdef SHARED_HANDLERS

// Prise en charge des miniatures
void CKIFNDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modifier ce code pour dessiner les données du document
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support pour les gestionnaires de recherche
void CKIFNDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Définir le contenu de recherche à partir des données du document. 
	// Les parties du contenu doivent être séparées par ";"

	// Par exemple :  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CKIFNDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// diagnostics pour CKIFNDoc

#ifdef _DEBUG
void CKIFNDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CKIFNDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG


// commandes pour CKIFNDoc
