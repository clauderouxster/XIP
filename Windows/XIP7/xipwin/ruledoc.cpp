// ruledoc.cpp : implementation of the CRuleDoc class
//
#include "stdafx.h"
#include "ruledoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRuleDoc

IMPLEMENT_DYNCREATE(CRuleDoc, CDocument)

BEGIN_MESSAGE_MAP(CRuleDoc, CDocument)
	//{{AFX_MSG_MAP(CRuleDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRuleDoc construction/destruction

CRuleDoc::CRuleDoc()
{
	// TODO: add one-time construction code here}
    xipsent=NULL;
}

CRuleDoc::~CRuleDoc()
{
}

BOOL CRuleDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

return TRUE;
}

				   
/////////////////////////////////////////////////////////////////////////////
// CRuleDoc serialization

void CRuleDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRuleDoc diagnostics

#ifdef _DEBUG
void CRuleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRuleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRuleDoc commands

void CRuleDoc::OnFileSave() {
    return;
}


void CRuleDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(FALSE);
}
