// xipwinDoc.h : interface of the CXipwinDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XIPWINDOC_H__26A4271F_24DD_4606_BDCE_B3367F0FB556__INCLUDED_)
#define AFX_XIPWINDOC_H__26A4271F_24DD_4606_BDCE_B3367F0FB556__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class ostrstream;

class CXipwinDoc : public CDocument
{
protected: // create from serialization only
	CXipwinDoc();
	DECLARE_DYNCREATE(CXipwinDoc)

// Attributes
public:
    CString texte;

// Operations
public:
    void ViewIndentoutput();
    CView* GetViewIndent();
    CView* GetViewSaisie();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXipwinDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnCloseDocument();
    virtual BOOL SaveModified();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXipwinDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CXipwinDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XIPWINDOC_H__26A4271F_24DD_4606_BDCE_B3367F0FB556__INCLUDED_)
