
// KIFNDoc.h : interface de la classe CKIFNDoc
//

#include <afxrich.h>
#pragma once


class CKIFNDoc : public CRichEditDoc
{
protected: // création à partir de la sérialisation uniquement
	CKIFNDoc();
	DECLARE_DYNCREATE(CKIFNDoc)

public:
		CString contenu;
		CString chemin;

// Opérations
public:

// Substitutions
public:
	BOOL OnNewDocument();
	BOOL OnSaveDocument(LPCTSTR lpszPathName);
	void OnCloseDocument( );
	void Serialize(CArchive& ar);
	BOOL OnOpenDocument(LPCTSTR lpszPathName);
	CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const {
		return new CRichEditCntrItem(preo,(CRichEditDoc*)this);
	}	

	CString Chemin() {
		return chemin;
	}

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implémentation
public:
	virtual ~CKIFNDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions générées de la table des messages
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Fonction d'assistance qui définit le contenu de recherche pour un gestionnaire de recherche
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
