
// KIFNDoc.h : interface de la classe CKIFNDoc
//

#include <afxrich.h>
#pragma once


class CKIFNDoc : public CRichEditDoc
{
protected: // cr�ation � partir de la s�rialisation uniquement
	CKIFNDoc();
	DECLARE_DYNCREATE(CKIFNDoc)

public:
		CString contenu;
		CString chemin;

// Op�rations
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

// Impl�mentation
public:
	virtual ~CKIFNDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions g�n�r�es de la table des messages
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Fonction d'assistance qui d�finit le contenu de recherche pour un gestionnaire de recherche
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
