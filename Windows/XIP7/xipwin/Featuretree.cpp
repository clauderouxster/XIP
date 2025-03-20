// Featuretree.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#include "pasofficiel.h"
#include "Featuretree.h"
#include "saisiedoc.h"
#include "saisieview.h"

extern int parseurCourant;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFeaturetree dialog

extern CMultiDocTemplate* pEditTemplate;

CFeaturetree::CFeaturetree(CWnd* pParent /*=NULL*/)
	: CDialog(CFeaturetree::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFeaturetree)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFeaturetree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFeaturetree)
	DDX_Control(pDX, IDC_TREEfeatures, m_tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFeaturetree, CDialog)
	//{{AFX_MSG_MAP(CFeaturetree)
	ON_NOTIFY(NM_DBLCLK, IDC_TREEfeatures, OnDblclkTREEfeatures)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFeaturetree message handlers

void CFeaturetree::Construction(arbretrait* ar,HTREEITEM root) {
    HTREEITEM newroot;
    CString label;
    for (int i=0;i<ar->fils.dernier;i++) {        
        if (ar->fils[i]->fils.dernier==1 && ar->fils[i]->fils[0]->fils.dernier==0) {
            label=ar->fils[i]->nom.c_str();
            label+=":";
            label+=ar->fils[i]->fils[0]->nom.c_str();
            if (ar->fils[i]->type()==0) {
                label+=" file:";
                label+=((featureroot*)ar->fils[i])->filename.c_str();                
            }

            newroot=m_tree.InsertItem(label,root);
        }
        else {            
            label=ar->fils[i]->nom.c_str();
			arbretrait* abr=ar->fils[i];
            if (abr->type()==0) {
                label+=" file:";                
                label+=((featureroot*)ar->fils[i])->filename.c_str();                
            }

            newroot=m_tree.InsertItem(label,root);
            m_tree.Expand(newroot,TVE_EXPAND);
            Construction(ar->fils[i],newroot);
        }
    }
}

BOOL CFeaturetree::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    arbretrait* ar=XipGetFeatureTree(parseurCourant);
    HTREEITEM root;
    root=m_tree.InsertItem(ar->nom.c_str());    
    Construction(ar,root);
	m_tree.Expand(root,TVE_EXPAND);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CFeaturetree::OnOK() 
{
	// TODO: Add extra validation here
    HTREEITEM hItem	= m_tree.GetSelectedItem();
    CString featurename=m_tree.GetItemText(hItem);
    int i=featurename.Find(":",0);
    if (i!=-1)
        featurename=featurename.Left(i);

    CString pathname="";
    CSaisieDoc* doc=NULL;
    while (hItem!=NULL) {
        CString ch=m_tree.GetItemText(hItem);
        i=ch.Find(" file:",0);
        if (i!=-1) {
            i+=6;
            pathname=ch.Right(ch.GetLength()-i);
            break;
        }
        hItem=m_tree.GetParentItem(hItem);
    }	
    if (pathname!="") {
        POSITION firstdoc=pEditTemplate->GetFirstDocPosition();
        BOOL found=FALSE;
        while (firstdoc!=NULL) {
            doc=(CSaisieDoc*)pEditTemplate->GetNextDoc(firstdoc);
            if (doc->GetPathName()==pathname) {
                found=TRUE;
                break;
            }
        }
                
        if (found==FALSE) {            
            doc= (CSaisieDoc*)pEditTemplate->OpenDocumentFile(pathname);        
            if (doc==NULL)
                return;        
        }
        
        //We look for the last view that was added
        POSITION pos = doc->GetFirstViewPosition();    
        CSaisieView* pView=(CSaisieView*)doc->GetNextView(pos); 
        pView->FindAndSelectString(featurename);
        pView->GetParentFrame()->BringWindowToTop();
        CDialog::OnOK();
    }		
}

void CFeaturetree::OnDblclkTREEfeatures(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnOK();
	*pResult = 0;
}
