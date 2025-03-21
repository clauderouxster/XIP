
// kifkidsDoc.h : interface de la classe CkifkidsDoc
//

#include <afxrich.h>

#pragma once



class CkifkidsDoc : public CRichEditDoc 
{
protected: // cr�ation � partir de la s�rialisation uniquement
	CkifkidsDoc();
	DECLARE_DYNCREATE(CkifkidsDoc)
	
// Attributs
public:
	CString contenu;
	BOOL premier;

// Op�rations
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

// Impl�mentation
public:
	virtual ~CkifkidsDoc();
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
