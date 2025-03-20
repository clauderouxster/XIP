// ruledoc.h : interface of the CRuleDoc class
//
/////////////////////////////////////////////////////////////////////////////
class XipResult;

class CRuleDoc : public CDocument
{
protected: // create from serialization only
	CRuleDoc();
	DECLARE_DYNCREATE(CRuleDoc)

// Attributes
public:
    XipResult* xipsent;
    CString sentence;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuleDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRuleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRuleDoc)
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
