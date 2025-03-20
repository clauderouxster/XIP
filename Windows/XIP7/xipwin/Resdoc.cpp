// outilsDoc.cpp : implementation of the CResdoc class
//

#include "stdafx.h"


#include "xipstlres.h"

#include "xipwin.h"
#include "Resdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern XipResult* resxip;
extern int iAnalysePhrase;
/////////////////////////////////////////////////////////////////////////////
// CResdoc

IMPLEMENT_DYNCREATE(CResdoc, CDocument)

BEGIN_MESSAGE_MAP(CResdoc, CDocument)
	//{{AFX_MSG_MAP(CResdoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResdoc construction/destruction

CResdoc::CResdoc()
{
	// TODO: add one-time construction code here
    root=resxip->sentences[iAnalysePhrase]->root;
    char buffer[10000];
    for (int j=0;j<resxip->sentences[iAnalysePhrase]->dependencies.size();j++) {
        buffer[0]=0;
        ostringstream os;
        //resxip->sentences[iAnalysePhrase]->dependencies[j]->print(os);        
        //buffer[os.pcount()-2]=0;
        chaines.ajoute(new CString(os.str().c_str()));
    }
}

CResdoc::~CResdoc()
{
    chaines.nettoie();
}

BOOL CResdoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

return TRUE;
}

				   
/////////////////////////////////////////////////////////////////////////////
// CResdoc serialization

void CResdoc::Serialize(CArchive& ar)
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
// CResdoc diagnostics

#ifdef _DEBUG
void CResdoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CResdoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CResdoc commands

void CResdoc::OnFileSave() {
    return;
}


void CResdoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(FALSE);
}
