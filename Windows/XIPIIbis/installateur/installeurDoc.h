// installeurDoc.h : interface of the CInstalleurDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTALLEURDOC_H__C3EED0F3_9720_46A9_9319_E9CB20A5CC89__INCLUDED_)
#define AFX_INSTALLEURDOC_H__C3EED0F3_9720_46A9_9319_E9CB20A5CC89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CInstalleurDoc : public CDocument
{
protected: // create from serialization only
	CInstalleurDoc();
	DECLARE_DYNCREATE(CInstalleurDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstalleurDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInstalleurDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CInstalleurDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTALLEURDOC_H__C3EED0F3_9720_46A9_9319_E9CB20A5CC89__INCLUDED_)
