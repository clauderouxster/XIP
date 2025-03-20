
// kifkidsDoc.h : interface de la classe CkifkidsDoc
//

#include <afxrich.h>

#pragma once



class CkifkidsDoc : public CRichEditDoc 
{
protected: // création à partir de la sérialisation uniquement
	CkifkidsDoc();
	DECLARE_DYNCREATE(CkifkidsDoc)
	
// Attributs
public:
	CString contenu;
	BOOL premier;

// Opérations
public:

// Substitutions
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	BOOL OnOpenDocument(LPCTSTR lpszPathName);

	CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const {
		return NULL;
	}

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implémentation
public:
	virtual ~CkifkidsDoc();
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
