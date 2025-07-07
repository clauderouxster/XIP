// outilsDoc.h : interface of the CResdoc class
//
/////////////////////////////////////////////////////////////////////////////
#include "xipvecta.h"
//#include "xipresult.h"
#include "xipstlres.h"

class CResdoc : public CDocument
{
protected: // create from serialization only
	CResdoc();
	DECLARE_DYNCREATE(CResdoc)

// Attributes
public:
    XipNode* root;
    VECTA<CString*> chaines;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResdoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResdoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CResdoc)
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

