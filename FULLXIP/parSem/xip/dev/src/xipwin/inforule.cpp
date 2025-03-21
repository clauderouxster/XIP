// inforule.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#include "inforule.h"

#include "pasofficiel.h"
#include <afxtempl.h>
#include ".\inforule.h"

#include "saisiedoc.h"
#include "saisieview.h"

void FormatRuleText(CString& ruletext,int j,int breakpoint,BOOL);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char* field_name[];
extern char* noms_champ[];
extern char* rule_name[];

extern VECTA<Label*>* labels_numero;
extern VECTA<Label*>* labels_index;
extern CMultiDocTemplate* pEditTemplate;

/////////////////////////////////////////////////////////////////////////////
// Cinforule dialog


Cinforule::Cinforule(CWnd* pParent /*=NULL*/)
	: CDialog(Cinforule::IDD, pParent)
{
	//{{AFX_DATA_INIT(Cinforule)
	m_ruletext = _T("");
	m_selection = -1;
	//}}AFX_DATA_INIT
    start_rule=0;
    ascending=TRUE;
}


void Cinforule::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cinforule)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_ruletext, m_ruletext);
	DDX_Text(pDX, IDC_select, m_selection);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(Cinforule, CDialog)
	//{{AFX_MSG_MAP(Cinforule)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST, OnColumnclickList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDEdit, OnBnClickedEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cinforule message handlers

static int CALLBACK Comparaison(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {

   // lParamSort contains a pointer to the list view control.
   // The lParam of an item is just its index.
   int col=(int)lParamSort;
  
   datatosort* dr1=(datatosort*)lParam1;
   datatosort* dr2=(datatosort*)lParam2;
   UINT i1,i2;

   switch(col) {
   case 0: //on the index
       if (dr1->index<dr2->index)
           return -1;
       if (dr1->index==dr2->index)
           return 0;
       return 1;
   case 1://on the layer       
       i1=(UINT)dr1->dr->couche;
       i2=(UINT)dr2->dr->couche;
       if (i1<i2)
           return -1;
       if (i1==i2)
           return 0;
       return 1;
   case 2://On the type
       if (dr1->dr->regle<dr2->dr->regle)
           return -1;
       if (dr1->dr->regle==dr2->dr->regle)
           return 0;
       return 1;
   case 3://On the filename
       return strcmp(dr1->dr->nom_fichier,dr2->dr->nom_fichier);
   }
   return 0;
}

    

BOOL Cinforule::OnInitDialog() 
{
    CDialog::OnInitDialog();
    char ch[1000];
    
    CSize lchaine=GetDC()->GetTextExtent("Number ");        
    m_list.InsertColumn(0,"Number",LVCFMT_LEFT, lchaine.cx, 0);
    lchaine=GetDC()->GetTextExtent("Layer ");        
    m_list.InsertColumn(1,"Layer",LVCFMT_LEFT, lchaine.cx, 1);
    lchaine=GetDC()->GetTextExtent("DISAMBIGUATION RULE IS ");    
    m_list.InsertColumn(2,"Type",LVCFMT_LEFT, lchaine.cx, 2);    
    m_list.InsertColumn(3,"File",LVCFMT_LEFT, lchaine.cx, 3);

    

    int item=0;
    for (int i=0;i<labels_numero->dernier-3;i++) {
        
        DecompteRegle* dr=(*labels_numero)[i]->dr;
        if (dr==NULL)
            break;

        char* pt=strrchr((*labels_numero)[i]->dr->nom_fichier,'\\');

        sprintf(ch,"%d",(*labels_numero)[i]->index);        
        CString lay=ch;
                
        int newitem=m_list.InsertItem(item,lay);        
        
        sprintf(ch,"%d",dr->couche);
        CString num=ch;
        m_list.SetItemText(newitem, 1, num);
        strcpy(ch,rule_name[dr->regle]);
        for (int k=strlen(ch)-1;k>=0;k--) {
            if (ch[k]>32)
                break;
            ch[k]=0;
        }
        m_list.SetItemText(newitem, 2, ch);
        m_list.SetItemText(newitem, 3,pt+1);
        datatosort* dt=new datatosort(dr,(*labels_numero)[i]->index);
        datas.ajoute(dt);
        m_list.SetItemData(newitem, (LPARAM)dt);


        item++;

    }       
    
        
    // TODO: Add extra initialization here
    
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}



void Cinforule::OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here    
	m_list.SortItems(Comparaison, (LPARAM)pNMListView->iSubItem);    
    m_list.UpdateData(FALSE);
	*pResult = 0;
}

void Cinforule::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // TODO: Add your control notification handler code here
    
    POSITION pos = m_list.GetFirstSelectedItemPosition();
    if (pos != NULL) {
        int nItem = m_list.GetNextSelectedItem(pos);
        CString text=m_list.GetItemText(nItem,0);
        nItem=atoi(LPCTSTR(text));
        m_ruletext="";
        FormatRuleText(m_ruletext,nItem,-1,TRUE);
        m_selection=nItem;
        UpdateData(FALSE);        
    }
	*pResult = 0;
}


void Cinforule::OnBnClickedEdit()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);    
    if (labels_index->dernier==0 || (*labels_index)[m_selection]==NULL || (*labels_index)[m_selection]->dr==NULL)
        return;

	CString ruletext="";
	FormatRuleText(ruletext,m_selection,-1,TRUE);
	int fi=ruletext.Find("\r\n");
	fi=ruletext.Find("\r\n",fi+1)+2;
	ruletext=ruletext.Mid(fi,ruletext.GetLength()-fi);
	ruletext=ruletext.Trim();
	
	if (labels_index==NULL)
		return;
	VECTA<Label*>& labels= *labels_index;


    CSaisieDoc* doc=NULL;
    DecompteRegle* dr=(*labels_index)[m_selection]->dr;
    
    CString name=dr->nom_fichier;        
	fi=name.ReverseFind('\\');
	if (fi!=-1)
		name=name.Mid(fi+1,name.GetLength()-fi);

    POSITION firstdoc=pEditTemplate->GetFirstDocPosition();
    BOOL found=FALSE;
    while (firstdoc!=NULL) {
        doc=(CSaisieDoc*)pEditTemplate->GetNextDoc(firstdoc);
		CString pat=doc->GetPathName();
		fi=pat.ReverseFind('\\');
		if (fi!=-1)
			pat=pat.Mid(fi+1,pat.GetLength()-fi);
        if (pat==name) {
            found=TRUE;
            break;
        }
    }
    
    if (found==FALSE) {            
        doc= (CSaisieDoc*)pEditTemplate->OpenDocumentFile(name);        
        if (doc==NULL)
            return;        
    }
    
    //We look for the last view that was added
    POSITION pos = doc->GetFirstViewPosition();    
    CSaisieView* pView=(CSaisieView*)doc->GetNextView(pos);                

	pView->SelectionOnString(ruletext);
    pView->GetParentFrame()->BringWindowToTop();
	OnOK();
}
