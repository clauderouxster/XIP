// xipwinDoc.cpp : implementation of the CSaisieDoc class
//

#include "stdafx.h"
#include "xipwin.h"

#include <ostream>
using namespace std;
#include <strstream>
#include <fstream>
#include "pasofficiel.h"
#include "saisiedoc.h"
#include "saisieview.h"
#include "Filetree.h"

extern long offset_begins[3];

extern VECTA<rule_file*> new_documents;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern VECTA<long> updatedlistlabels;
extern VECTA<Label*>* labels_numero;
extern string languageFilename;
extern int parseurCourant;
/////////////////////////////////////////////////////////////////////////////
// CSaisieDoc
//XIP: Xerox Incremental Parser 2.0 (2000-2001)\r\n\r\nConception:\r\n\tSalah Aït-Mokhtar\r\n\tJean-Pierre Chanod\r\n\tClaude Roux\r\n\r\nImplementation:\r\n\tClaude Roux

IMPLEMENT_DYNCREATE(CSaisieDoc, CDocument)

BEGIN_MESSAGE_MAP(CSaisieDoc, CDocument)
	//{{AFX_MSG_MAP(CSaisieDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaisieDoc construction/destruction

CSaisieDoc::CSaisieDoc()
{
	// TODO: add one-time construction code here
    parameter=FALSE;
    docname="";
}

CSaisieDoc::~CSaisieDoc()
{
}

BOOL CSaisieDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;    

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)    
    
    //POSITION pos = GetFirstViewPosition();
    //CView* vue = GetNextView( pos );
    //vue->SetWindowText("Fenêtre de Saisie...");
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSaisieDoc serialization

void CSaisieDoc::Serialize(CArchive& ar)
{
    POSITION pos = GetFirstViewPosition();
    CSaisieView* vue=(CSaisieView*)GetNextView(pos);
    CEdit& edit=vue->GetEditCtrl();
    char* buff;

    if (ar.IsStoring())
    {
        // TODO: add storing code here        
    }
    else
    {
        // TODO: add loading code here
        //First we get our view
        
        CFile* fichier=ar.GetFile();
        docname=fichier->GetFilePath();
        int i,indexdebut,indexfin;
        indexdebut=-1;indexfin=-1;
        for (i=0;i<labels_numero->dernier;i++) {
            if ((*labels_numero)[i]->dr==NULL)
                break;
            if (docname==(*labels_numero)[i]->dr->nom_fichier) {
                if (indexdebut==-1)
                    indexdebut=i;                
                indexfin=i+1;
            }
        }

        CFileStatus status;
        fichier->GetStatus(status);
        buff=new char[status.m_size+1];        
        vue->buffer=new unsigned char[status.m_size*2+10];        
        vue->size=status.m_size*2;
        UINT nb=ar.Read(buff,status.m_size+1);        
        int j=0;
        for (i=0;i<nb;i++) {
            if (indexdebut<indexfin &&
                updatedlistlabels[indexdebut]==0 &&
                (*labels_numero)[indexdebut]->dr->position==i) {
                //(*labels_numero)[indexdebut]->dr->position=j;
                updatedlistlabels.affecte(indexdebut,j-i+1);
                indexdebut++;
            }

            if (buff[i]==10 && buff[i-1]!=13) {
                vue->buffer[j++]='\r';
                vue->buffer[j++]='\n';
            }
            else
                vue->buffer[j++]=buff[i];
        }
        vue->buffer[j]=0;
        delete buff;
    }
}    
/////////////////////////////////////////////////////////////////////////////
// CSaisieDoc diagnostics

#ifdef _DEBUG
void CSaisieDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSaisieDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSaisieDoc commands

void CSaisieDoc::OnCloseDocument() 
{
    if (IsModified()==TRUE) {
        int diff;
        CString name;
        for (int i=lastadded.dernier-1;i>=0;i--) {
            if (lastadded[i]->removed==TRUE) {
                labels_numero->insere(lastadded[i]->index,lastadded[i]->lab_removed);
                name=(*labels_numero)[lastadded[i]->index]->dr->nom_fichier;
                for (int k=lastadded[i]->index+1;k<labels_numero->dernier;k++) {
                    if ((*labels_numero)[k]->dr==NULL)
                        break;
                    if (name!=(*labels_numero)[k]->dr->nom_fichier)
                        break;
                    (*labels_numero)[k]->dr->position+=lastadded[i]->increment;
                }
                continue;
            }
            
            for (int j=lastadded[i]->index+1;j<=lastadded[i]->max;j++) {                
                if ((*labels_numero)[j]->dr==NULL)
                    continue;
                (*labels_numero)[j]->dr->position-=lastadded[i]->increment;
                
                diff=lastadded[i]->layer;
                (*labels_numero)[j]->dr->couche-=diff;
                (*labels_numero)[j]->dr->couche_relative-=diff;                
            }

            if (lastadded[i]->increment!=-1) {
                name=(*labels_numero)[lastadded[i]->index]->dr->nom_fichier;
                int k=lastadded[i]->index-1;
                diff=lastadded[i]->layer;
                
                while (k>=0 && 
                    (*labels_numero)[k]->dr!=NULL && 
                    name==(*labels_numero)[k]->dr->nom_fichier) {                    
                    (*labels_numero)[k]->dr->couche-=diff;
                    (*labels_numero)[k]->dr->couche_relative-=diff;                    
                    k--;
                }
            }

            //if (lastadded[i]->new_rule==TRUE)
            //    delete labels_numero->retireElement(lastadded[i]->index);
        }        

    }

    lastadded.nettoie();    
	CDocument::OnCloseDocument();
}


BOOL CSaisieDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class

    ofstream file(lpszPathName);
    if (file.fail() ) {
        AfxMessageBox("Writing error: File not saved (protected)",MB_OK);
        return FALSE;
    }
    POSITION pos = GetFirstViewPosition();
    CSaisieView* vue= (CSaisieView*)GetNextView(pos);
    CEdit& edit=vue->GetEditCtrl();
    CString texte;
    edit.GetWindowText(texte);
    const char* buff=LPCTSTR(texte);
    int lg=texte.GetLength();

    if (vue->buffer==NULL)
        vue->buffer=new unsigned char[lg+1];
    
    vue->buffer[0]=0;
    int j=0;int i;
    for (i=0;i<lg;i++) {
        if (buff[i]=='\r')
            continue;
        vue->buffer[j++]=buff[i];
    }
    vue->buffer[j]=0;

    file<<vue->buffer<<endl;
    lastadded.nettoie();
    SetModifiedFlag(FALSE);
    return TRUE;
	//return CDocument::OnSaveDocument(lpszPathName);
}


BOOL CSaisieDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{    
    if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;	

    
	return TRUE;
}


