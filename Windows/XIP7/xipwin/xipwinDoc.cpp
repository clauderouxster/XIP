// xipwinDoc.cpp : implementation of the CXipwinDoc class
//

#include "stdafx.h"
#include "xipwin.h"

#include "xipwinDoc.h"
#include <ostream>
using namespace std;
#include <strstream>
#include <fstream>
#include "ruledoc.h"

extern CXipwinApp theAppXIPWIN;
CString pathNameIndent="";
CString texteIndent;
CXipwinDoc* DocumentIndent=NULL;
CXipwinDoc* DocumentSaisie=NULL;
CXipwinDoc* docoutput=NULL;
extern int CompteAnalyse;
CString leTexte;
extern CString grammarPathInput;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CXipwinDoc
//XIP: Xerox Incremental Parser 2.0 (2000-2001)\r\n\r\nConception:\r\n\tSalah Aït-Mokhtar\r\n\tJean-Pierre Chanod\r\n\tClaude Roux\r\n\r\nImplementation:\r\n\tClaude Roux

IMPLEMENT_DYNCREATE(CXipwinDoc, CDocument)

BEGIN_MESSAGE_MAP(CXipwinDoc, CDocument)
	//{{AFX_MSG_MAP(CXipwinDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXipwinDoc construction/destruction

CXipwinDoc::CXipwinDoc()
{
	// TODO: add one-time construction code here
        texte="";        
}

CXipwinDoc::~CXipwinDoc()
{
}

BOOL CXipwinDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;    

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)    
    
    //POSITION pos = GetFirstViewPosition();
    //CView* vue = GetNextView( pos );
    //vue->SetWindowText("Fenêtre de Saisie...");
    if (DocumentSaisie==NULL) {
        SetTitle("Typing Window...");
        DocumentSaisie=this;
		CFileException fe;
		CFile* pFile = NULL;
		pFile = GetFile(grammarPathInput, CFile::modeReadWrite, &fe);
		if (pFile!=NULL)
			pFile->Close();
		else {
			pFile = GetFile(grammarPathInput, CFile::modeCreate|CFile::modeReadWrite, &fe);
			if (pFile!=NULL)
				pFile->Close();
		}

		DocumentSaisie->OnOpenDocument(grammarPathInput);
    }
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CXipwinDoc serialization

void CXipwinDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
        CString toRead;
        leTexte="";
        while (ar.ReadString(toRead)==TRUE) {            
            if (toRead.GetLength() > 0)
                toRead+="\r\n";
            leTexte += toRead;
        }
    }
}    
/////////////////////////////////////////////////////////////////////////////
// CXipwinDoc diagnostics

#ifdef _DEBUG
void CXipwinDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CXipwinDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXipwinDoc commands

void CXipwinDoc::OnCloseDocument() 
{

    if (DocumentSaisie==this)
        return;
    if (DocumentIndent==this)
       DocumentIndent=NULL;
    if (docoutput==this)
        docoutput=NULL;

	CDocument::OnCloseDocument();
}

BOOL CXipwinDoc::SaveModified() {
    if (DocumentSaisie==this) {        
        SetModifiedFlag(FALSE);
        return TRUE;
    }
    
    return CDocument::SaveModified();
}


BOOL CXipwinDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
    POSITION pos = GetFirstViewPosition();
    CView* vue= GetNextView(pos);
    CString texte;
    vue->GetWindowText(texte);
    ofstream file(lpszPathName);
    file<<LPCTSTR(texte)<<endl;
    return TRUE;
	//return CDocument::OnSaveDocument(lpszPathName);
}

BOOL Indentation=FALSE;

BOOL CXipwinDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;	

    if (Indentation==FALSE) {
        texte=leTexte;
        POSITION pos = DocumentSaisie->GetFirstViewPosition();
        CView* vue= DocumentSaisie->GetNextView(pos);
        vue->SetWindowText(texte);    
    }
    else
        texteIndent=leTexte;

	return Indentation;
}


void CXipwinDoc::ViewIndentoutput() 
{
    // TODO: Add your command handler code here
    Indentation=TRUE;
    if (DocumentIndent!=NULL)
        delete DocumentIndent;
    
    DocumentIndent=(CXipwinDoc*)theAppXIPWIN.OpenDocumentFile(pathNameIndent);
    if (DocumentIndent==NULL)
        return;
    CString titre="INDENT:";
    char ch[100];
    sprintf(ch,"%d",CompteAnalyse-1);
    titre+=ch;

    DocumentIndent->SetTitle(titre);
    POSITION pos = DocumentIndent->GetFirstViewPosition();
    CView* vue= DocumentIndent->GetNextView(pos);
    vue->SetWindowText(texteIndent);    
    CFrameWnd* pFrame = vue->GetParentFrame();
    if (pFrame != NULL)
        pFrame->ActivateFrame();

    Indentation=FALSE;
    
}


CView* CXipwinDoc::GetViewIndent() {
    if (DocumentIndent==NULL)
        return NULL;
    POSITION pos = DocumentIndent->GetFirstViewPosition();
    CView* vue= DocumentIndent->GetNextView(pos);
    return vue; 
}

CView* CXipwinDoc::GetViewSaisie() {
    POSITION pos = DocumentSaisie->GetFirstViewPosition();
    CView* vue= DocumentSaisie->GetNextView(pos);
    return vue; 
}


