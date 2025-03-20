
// kifkidsDoc.cpp : implémentation de la classe CkifkidsDoc
//

#include "stdafx.h"
// SHARED_HANDLERS peuvent être définis dans les gestionnaires d'aperçu, de miniature
// et de recherche d'implémentation de projet ATL et permettent la partage de code de document avec ce projet.
#ifndef SHARED_HANDLERS
#include "kifkids.h"
#endif

#include "kif.h"

#include "kifkidsDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CkifkidsDoc* mondoc=NULL;
extern CString fichiercontenu;
// CkifkidsDoc

IMPLEMENT_DYNCREATE(CkifkidsDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CkifkidsDoc, CRichEditDoc)
END_MESSAGE_MAP()


// construction ou destruction de CkifkidsDoc

CkifkidsDoc::CkifkidsDoc()
{
	// TODO: ajoutez ici le code d'une construction unique
	mondoc=this;	
	premier=true;
}

CkifkidsDoc::~CkifkidsDoc()
{
}

BOOL CkifkidsDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

	if (fichiercontenu=="")
		return TRUE;

	// TODO: ajoutez ici le code de réinitialisation
	// (les documents SDI réutiliseront ce document)
	CFileException fe;
	CFile* pFile = NULL;
	pFile = GetFile(fichiercontenu, CFile::modeReadWrite, &fe);
	if (pFile!=NULL)
		pFile->Close();
	else {
		pFile = GetFile(fichiercontenu, CFile::modeCreate|CFile::modeReadWrite, &fe);
		if (pFile!=NULL)
			pFile->Close();
	}

	SetTitle(fichiercontenu);
	OnOpenDocument(fichiercontenu);

	return TRUE;
}


BOOL CkifkidsDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CRichEditDoc::OnOpenDocument(lpszPathName))
		return FALSE;	

	POSITION pos = GetFirstViewPosition();
	CView* vue= GetNextView(pos);
	vue->SetWindowText(contenu);    

	m_strPathName=lpszPathName;
	return TRUE;
}



// sérialisation de CkifkidsDoc

void CkifkidsDoc::Serialize(CArchive& ar) {
	if (ar.IsStoring())	{
		// TODO: ajoutez ici le code de stockage
		CFile* fichier=ar.GetFile();
		string s;
		//Nous sauvegardons en utf8 pour etre lisible sur d'autres plate-formes
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
void CkifkidsDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CkifkidsDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Définir le contenu de recherche à partir des données du document. 
	// Les parties du contenu doivent être séparées par ";"

	// Par exemple :  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CkifkidsDoc::SetSearchContent(const CString& value)
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

// diagnostics pour CkifkidsDoc

#ifdef _DEBUG
void CkifkidsDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CkifkidsDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG


// commandes pour CkifkidsDoc
