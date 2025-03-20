// installeurDoc.cpp : implementation of the CInstalleurDoc class
//

#include "stdafx.h"
#include "installateur.h"

#include "installeurDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstalleurDoc

IMPLEMENT_DYNCREATE(CInstalleurDoc, CDocument)

BEGIN_MESSAGE_MAP(CInstalleurDoc, CDocument)
	//{{AFX_MSG_MAP(CInstalleurDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstalleurDoc construction/destruction

CInstalleurDoc::CInstalleurDoc()
{
	// TODO: add one-time construction code here

}

CInstalleurDoc::~CInstalleurDoc()
{
}

BOOL CInstalleurDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CInstalleurDoc serialization

void CInstalleurDoc::Serialize(CArchive& ar)
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
// CInstalleurDoc diagnostics

#ifdef _DEBUG
void CInstalleurDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CInstalleurDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInstalleurDoc commands
