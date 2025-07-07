// xipwinView.cpp : implementation of the CSaisieView class
//

#include "stdafx.h"
#include "xipwin.h"

#include "pasofficiel.h"
#include <strstream>

#include "saisiedoc.h"
#include "saisieview.h"
#include "Searchstring.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame* laMain;
extern VECTA<Label*>* labels_index;
extern VECTA<Label*>* labels_numero;
extern VECTA<long> updatedlistlabels;

/////////////////////////////////////////////////////////////////////////////
// CSaisieView

IMPLEMENT_DYNCREATE(CSaisieView, CEditView)

BEGIN_MESSAGE_MAP(CSaisieView, CEditView)
	//{{AFX_MSG_MAP(CSaisieView)
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
    ON_COMMAND(ID_SEARCHINRULE, OnSearchinrule)
    ON_COMMAND(ID_SEARCHAGAIN, OnSearchagain)
    ON_COMMAND(ID_FINDRULENUMBER,OnSearchRulenumber)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaisieView construction/destruction

CSaisieView::CSaisieView()
{
	// TODO: add construction code here  
    buffer=NULL;
    size=0;
    position=0;
    searchstring="";
}

CSaisieView::~CSaisieView()
{
    delete buffer;
}

void FormatRuleText(CString& ruletext,int j,int breakpoint,BOOL);

CString retireespace(CString& rule) {
	char* pt=(char*)LPCTSTR(rule);
	int mx=rule.GetLength()-1;
	for (int i=mx;i>=0;i--) {
		if (pt[i]<=32)
			strcpy(pt+i,pt+i+1);
	}
	return pt;
}

void GetRuleText(int m_selection,CString& ruletext) {
	FormatRuleText(ruletext,m_selection,-1,TRUE);
	int fi=ruletext.Find("\r\n");
	fi=ruletext.Find("\r\n",fi+1)+2;
	ruletext=ruletext.Mid(fi,ruletext.GetLength()-fi);
	ruletext=ruletext.Trim();
	ruletext=retireespace(ruletext);
}

int ReturnRuleNumber(CString& docname,int rawfirst,CString& maligne) {
	int i;
	if (labels_numero->dernier==0)
		return -1;

	for (i=0; i<labels_numero->dernier && (*labels_numero)[i]->dr!=NULL && 
		docname!=(*labels_numero)[i]->dr->nom_fichier;i++);

	maligne=retireespace(maligne);
	for (;i<labels_numero->dernier && (*labels_numero)[i]->dr!=NULL && 
		docname==(*labels_numero)[i]->dr->nom_fichier;i++) {
		if (i<labels_numero->dernier-1 && (*labels_numero)[i+1]->dr!=NULL) {
			CString ruletext;
			GetRuleText((*labels_numero)[i]->index,ruletext);
			if (ruletext.Find(maligne)!=-1)
				return (*labels_numero)[i]->index;
		}
	}
	return -1;
}

BOOL CSaisieView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

    m_dwDefaultStyle = dwStyleDefault;
    m_dwDefaultStyle &= ~ES_AUTOHSCROLL;
    m_dwDefaultStyle &= ~WS_HSCROLL;
    return CCtrlView::PreCreateWindow(cs);

	//return CEditView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSaisieView drawing

void CSaisieView::OnDraw(CDC* pDC)
{
	CSaisieDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CSaisieView printing

BOOL CSaisieView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSaisieView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSaisieView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSaisieView diagnostics

#ifdef _DEBUG
void CSaisieView::AssertValid() const
{
	CEditView::AssertValid();
}

void CSaisieView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CSaisieDoc* CSaisieView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSaisieDoc)));
	return (CSaisieDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSaisieView message handlers


void CSaisieView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
    
    if (buffer!=NULL)
        SetWindowText((char*)buffer);
    CEditView::OnUpdate(pSender,lHint,pHint);		
}

void CSaisieView::OnFileClose() 
{
	// TODO: Add your command handler code here
	
}

void CSaisieView::FindBorne(long first,long last,long& firstsel,long& lastsel,int idrule) {

	if (labels_index==NULL || labels_numero==NULL || (*labels_index)[idrule]==NULL)
		return;

	//First we extract the rule number
	int numero=(*labels_index)[idrule]->numero;
	VECTA<Label*>& labels= *labels_index;


    //If there is a change of rule in the same file
    DecompteRegle* dr=labels[numero]->dr;    
    DecompteRegle* drnext=NULL;
    
    if (labels[numero+1]!=NULL)
        drnext=labels[numero+1]->dr;    


	if (updatedlistlabels[numero]!=0) {
		//If there is a change of rule in the same file
		first+=updatedlistlabels[numero]-1;
		
		dr->position+=updatedlistlabels[numero]-1;
		if (drnext!=NULL) {
			drnext->position+=updatedlistlabels[numero+1]-1;
			last+=updatedlistlabels[numero+1]-1;
		}
		else
			last+=updatedlistlabels[numero]-1;
	}

    if (first>last) {
        firstsel=-1;
        lastsel=-1;
    }
    
    if (last>strlen((char*)buffer))
        last=strlen((char*)buffer);

	char c=buffer[last+1];
	buffer[last+1]=0;

	FindRuleBoundaries((unsigned char*)buffer+first,firstsel,lastsel,dr,drnext);
	if (updatedlistlabels[numero]!=0) {
		//If there is a change of rule in the same file
		dr->position-=updatedlistlabels[numero]-1;
		if (drnext!=NULL)
			drnext->position-=updatedlistlabels[numero+1]-1;
	}

	firstsel+=first;
	lastsel+=first;
	buffer[last+1]=c;

}

void CSaisieView::Selection(long first,long last,int index) {
    long firstsel;
    long lastsel;
    CEdit& edit=GetEditCtrl();
    CString texte;
    edit.GetWindowText(texte);

    if (texte!=buffer)
        AfxMessageBox("Note that the file has been modified. The selection may be inaccurate\r\nPlease Reload the grammar...",MB_OK);

    FindBorne(first,last,firstsel,lastsel,index);    
    edit.SetSel(firstsel,lastsel,FALSE);
}

void CSaisieView::SelectionOnString(CString& ruletext) {
    CEdit& edit=GetEditCtrl();
    CString texte;
    edit.GetWindowText(texte);

    if (texte!=buffer)
        AfxMessageBox("Note that the file has been modified. The selection may be inaccurate\r\nPlease Reload the grammar...",MB_OK);


	int firstsel=texte.Find(ruletext);
	if (firstsel==-1) {
		//Dans ce cas, on ne recherche que jusqu'au premier retour chariot
		int rtx=ruletext.Find("\r");
		if (rtx==-1)
			rtx=ruletext.Find("\n");
		ruletext=ruletext.Mid(0,rtx);
		ruletext=ruletext.Trim();
		firstsel=texte.Find(ruletext);
	}
	int lastsel=firstsel+ruletext.GetLength();
    edit.SetSel(firstsel,lastsel,FALSE);
}

void CSaisieView::IndexSelectionOnString(CString& ruletext,int& firstsel,int& lastsel) {
    CEdit& edit=GetEditCtrl();
    CString texte;
    edit.GetWindowText(texte);

    if (texte!=buffer)
        AfxMessageBox("Note that the file has been modified. The selection may be inaccurate\r\nPlease Reload the grammar...",MB_OK);

	firstsel=texte.Find(ruletext);
	lastsel=firstsel+ruletext.GetLength();
}

void CSaisieView::DeleteRule(long first,long last,int idrule) {
    long firstsel;
    long lastsel;
    CEdit& edit=GetEditCtrl();
    CString texte;
    edit.GetWindowText(texte);

    if (texte!=buffer) {
        AfxMessageBox("The file has been modified. No rule can be deleted\r\nPlease Reload the grammar...",MB_OK);
        return;
    }
	if (labels_index==NULL || labels_numero==NULL || (*labels_index)[idrule]==NULL)
		return;

	//First we extract the rule number
	int numero=(*labels_index)[idrule]->numero;
	VECTA<Label*>& labels= *labels_numero;

    FindBorne(first,last,firstsel,lastsel,idrule);
    int nbcar=lastsel-firstsel+1;
    texte.Delete(firstsel,nbcar);
    edit.SetWindowText(texte);        
    strcpy((char*)buffer,LPCTSTR(texte));
    CString name=labels[numero]->dr->nom_fichier;
    
    //We then remove the element from the list
    for (int i=numero+1;i<labels.dernier;i++) {
        if (labels[i]->dr==NULL)
            break;
        if (name!=labels[i]->dr->nom_fichier)
            break;
        labels[i]->dr->position-=nbcar;
    }
    
    LastAdded* la=new LastAdded(numero,0,nbcar,0,TRUE);
    la->removed=TRUE;
    la->lab_removed=labels.retireElement(numero);
    GetDocument()->lastadded.ajoute(la);
    GetDocument()->SetModifiedFlag(TRUE);
}

void CSaisieView::OnLButtonUp(UINT nFlags,CPoint point) {
    int first;
    int last;
	CEdit& edit=GetEditCtrl();
    edit.GetSel(first,last);
    static char ch[100];
    CString texte;
    edit.GetWindowText(texte);
	int nb=1;
	int i;
	for (i=0;i<first;i++) {
		if (texte[i]==13)
			nb++;
	}

	for (i=first;i>=0;i--) {
		if (texte[i]==13)
			break;
	}
	int ledebut=i+1;
	for (i=first;i<texte.GetLength();i++) {
		if (texte[i]==13)
			break;
	}
	int lafin=i;	
	CString maligne=texte.Mid(ledebut,lafin-ledebut);
	maligne=maligne.Trim();
	CString docname=GetDocument()->docname;
	int index=ReturnRuleNumber(docname,first,maligne);
	sprintf(ch,"Line:%d  Rule:%d",nb,index);
	CString l=ch;
	laMain->AfficheLigne(l);
	CEditView::OnLButtonUp(nFlags,point);
}

void CSaisieView::OnKeyUp(UINT nChar,UINT nRepCnt,UINT nFlags) {

	if (nChar>=33 && nChar<=40) {
		int first;
		int last;
		CEdit& edit=GetEditCtrl();
		edit.GetSel(first,last);
		static char ch[100];
		CString texte;
		edit.GetWindowText(texte);
		int nb=1;
		CString docname=GetDocument()->docname;
		int i;
		for (i=0;i<first;i++) {
			if (texte[i]==13)
				nb++;
		}

		for (i=first;i>=0;i--) {
			if (texte[i]==13)
				break;
		}
		int ledebut=i+1;
		for (i=first;i<texte.GetLength();i++) {
			if (texte[i]==13)
				break;
		}
		int lafin=i;
		CString maligne=texte.Mid(ledebut,lafin-ledebut);
		maligne=maligne.Trim();

		int index=ReturnRuleNumber(docname,first,maligne);
		sprintf(ch,"Line:%d  Rule:%d",nb,index);
		CString l=ch;
		laMain->AfficheLigne(l);
	}
	CEditView::OnKeyUp(nChar,nRepCnt,nFlags);
}

void CSaisieView::FindPreviousField(char** fields,int nbfields,CString& field) {
    if (buffer==NULL) {
        field="";
        return;
    }
    long l=strlen((char*)buffer);
    int i,j;
    while (l>=0) {
        while (l>=0 && buffer[l]!=':') l--;
        if (l>=0) {
            BOOL found=FALSE;
            for (i=0;i<nbfields;i++) {
                found=TRUE;
                int p=l;
                if (fields[i][0]==0)
                    continue;
                for (j=strlen(fields[i])-1;j>=0;j--) {
                    if (fields[i][j]!=tolower(buffer[p--])) {
                        found=FALSE;
                        break;
                    }
                }
                if (found==TRUE)
                    break;
            }
            if (found==TRUE) {
                field=fields[i];
                return;
            }
        }
        l--;
    }
}


void CSaisieView::AddRule(CString& rule) {
    CEdit& edit=GetEditCtrl();
    CString texte;
    edit.GetWindowText(texte);
    texte+=rule;
    edit.SetWindowText(texte);   
    if (texte.GetLength()>size) {
        delete buffer;
        size=texte.GetLength();
        buffer=new unsigned char[size+10];
    }
    
    strcpy((char*)buffer,LPCTSTR(texte));
    GetDocument()->SetModifiedFlag(TRUE);
}

void CSaisieView::InsertText(CString& rule,long first,long last,int idrule,
                             typeRegle typerule,
                             int relativelayer,
                             int absolutelayer) {

    //First we evaluate whether the extern buffer and the inner buffer are the same
    CEdit& edit=GetEditCtrl();
    CString texte;
    edit.GetWindowText(texte);

    if (texte!=buffer) {
        AfxMessageBox("The file has been modified. No new rule can be inserted\r\nPlease Reload the grammar...",MB_OK);
        return;
    }

	if (labels_index==NULL || labels_numero==NULL || (*labels_index)[idrule]==NULL)
		return;

	//First we extract the rule number
	int numero=(*labels_index)[idrule]->numero;
	VECTA<Label*>& labels= *labels_index;

    //First we try to find the first character to match   
    long lastsel=last;
    //FindBorne(first,last,firstsel,lastsel,index);
    //First we check if this rule is of the same sort as the one we want to create
    int add=-1;
    if (buffer[lastsel]=='\n') {
        lastsel++;
        add=0;
    }
    
    if (lastsel>=texte.GetLength())
        texte+=rule;
    else
        texte.Insert(lastsel,rule);
    
    edit.SetWindowText(texte);    
	
    if (texte.GetLength()>size) {
        delete buffer;
        size=texte.GetLength();
        buffer=new unsigned char[size+10];
    }
    
    strcpy((char*)buffer,LPCTSTR(texte));
    long nbcaradded=rule.GetLength()+add;    
    
    Label* lab=new Label;
    lab->etiquette=new char[2];
    lab->etiquette[0]='#';
    lab->etiquette[1]=0;
	lab->numero=-1;
	lab->index=-1;
	lab->space=NULL;
    lab->couche=absolutelayer;    
    lab->type=typerule;
    lab->dr=new DecompteRegle;
    lab->dr->couche=absolutelayer;
    lab->dr->couche_relative=relativelayer;
    lab->dr->user=NULL;
    lab->dr->nom_fichier=strdup(labels[numero]->dr->nom_fichier);
    lab->dr->position=lastsel+first;
    lab->dr->regle=typerule;
    labels.insere(numero+1,lab);
    CString name=lab->dr->nom_fichier;
    //nbcaradded++;
	int i;
    for (i=numero+2;i<labels.dernier;i++) {
        if (labels[i]->dr==NULL)
            break;
        if (name!=labels[i]->dr->nom_fichier)
            break;
        labels[i]->dr->position+=nbcaradded;
    }

    LastAdded* la=new LastAdded(numero+1,i,nbcaradded,0,TRUE);
    GetDocument()->lastadded.ajoute(la);
    GetDocument()->SetModifiedFlag(TRUE);
	edit.SetSel(last,last+rule.GetLength(),FALSE);
}


void CSaisieView::Renumber(int baselayer,int idrule,int sens, int increment,BOOL addflag) {
	if (labels_index==NULL || labels_numero==NULL || (*labels_index)[idrule]==NULL)
		return;

	//First we extract the rule number
	int numero=(*labels_index)[idrule]->numero;
	VECTA<Label*>& labels= *labels_numero;

	CString name=labels[numero]->dr->nom_fichier;
    int i=numero;
    long position;
    char ch[10];    
    int increment_position=0;
    CEdit& edit=GetEditCtrl();
    CString texte;
    edit.GetWindowText(texte);

    if (texte!=buffer) {
        AfxMessageBox("The file has been modified. It cannot be renumbered\r\nPlease Reload the grammar...",MB_OK);
        return;
    }

    //In the case of a decrement, we start on the previous rule.
    if (sens==-1)
        i--;    

    int max=i;
    LastAdded* la;
    while (labels[max]!=NULL &&
           labels[max]->dr!=NULL && 
            name==labels[max]->dr->nom_fichier) 
        max+=sens;

    //It there has been one single loop, we go back one step before.
    if (max!=i)
        max-=sens;

    BOOL loop=TRUE;
    BOOL added=FALSE;
    while (loop==TRUE) {
        if (sens==-1 && i<max)
            break;
        if (sens==1 && i>max)
            break;

        if (labels[i]->dr->couche==-1 || 
            labels[i]->dr->couche_relative<baselayer) {
            i+=sens;
            continue;
        }

        labels[i]->dr->position+=increment_position;
        int layer=labels[i]->dr->couche_relative;
        position=labels[i]->dr->position;
        layer+=increment;
        sprintf(ch,"%d",layer);
        int maxlength=strlen((char*)buffer);

        while (position<maxlength &&
            !isdigit(buffer[position]))
            position++;
        
        int j=position;        
        while (position<maxlength && buffer[position]!='>') {
            while (position<maxlength && !isdigit(buffer[position]))
                position++;

            if (position>=maxlength)
                break;

            j=position;
            while (isdigit(buffer[j])) j++;
            if (buffer[j]=='>')
                break;
            position++;
        }
        
        if (position>=maxlength)
            break;

        added=TRUE;
        for (j=0;j<strlen(ch);j++)
            buffer[position+j]=ch[j];
        if (buffer[position+j]!='>') {
            if (buffer[position+j]==32)            
                buffer[position+j]='>';
            else {                
                CString bb=buffer+position+j;
                buffer[position+j]='>';
                buffer[position+j+1]=' ';
                strcpy((char*)buffer+position+j+2,LPCTSTR(bb));
                increment_position++;                
                la=new LastAdded(i,max,1,0,FALSE);        
                GetDocument()->lastadded.ajoute(la);
                
            }
        }

                        
        if (labels[i]->dr->user==NULL)        
            labels[i]->dr->user=(void*)increment;

        labels[i]->dr->couche_relative+=increment;
        labels[i]->dr->couche+=increment;
        i+=sens;
    }

    if (added==TRUE) {
        la=new LastAdded(numero-1,max,0,increment,FALSE);        
        GetDocument()->lastadded.ajoute(la);
        GetDocument()->SetModifiedFlag(TRUE);
    }
    
    edit.SetWindowText((char*)buffer);
    
}

int CSaisieView::GetLayerNumber(long first,long last,int index) {

    long firstsel=-1,lastsel,j=-1;
    int k;

    FindBorne(first,last,firstsel,lastsel,index);

    j=firstsel;
    if (!isdigit(buffer[j]))
        return -1;
    
    while (isdigit(buffer[j])) j++;
    if (buffer[j]=='>') {
        buffer[j]=0;
        k=atoi((char*)buffer+firstsel);
        buffer[j]='>';
        return k;
    }
    
    return -1;
}

void CSaisieView::OnSearchinrule() 
{
	// TODO: Add your command handler code here
    CSearchstring search;
    CEdit& edit=GetEditCtrl();
    CString buff="";
        
    int first;
    int last;
    edit.GetSel(first,last);
    if (last>first) {
        position=first+1;
        edit.GetWindowText(buff);
        searchstring=buff.Mid(first,last-first);
    }
    search.m_searchstring=searchstring;
    search.DoModal();
    if (search.find==1 || searchstring=="") {
        //we try to find a new string
        searchstring=search.m_searchstring;
        position=0;
    }

    if (searchstring!="") {        
        if (buff=="")
            edit.GetWindowText(buff);
        int val=buff.Find(searchstring,position);
        if (val==-1) {
            AfxMessageBox("String not found",MB_OK);
            searchstring="";
            position=0;
            return;
        }        
        edit.SetSel(val,val+searchstring.GetLength(),FALSE);
        position=val+1;
    }
}


void CSaisieView::FindAndSelectString(CString& s) {
    if (s!="") {
        CEdit& edit=GetEditCtrl();
        CString buff;
        edit.GetWindowText(buff);
        //We look first for the extreme case
        searchstring=s;
        searchstring+=":";
        int val=buff.Find(searchstring,position);
        if (val==-1) {
            searchstring=s;
            searchstring+=" ";
            val=buff.Find(searchstring,position);
            if (val==-1) {
                searchstring=s;searchstring+="\t";
                val=buff.Find(searchstring,position);
                if (val==-1) {
                    searchstring=s;searchstring+="\r";
                    val=buff.Find(searchstring,position);
                    if (val==-1) {
                        searchstring=s;searchstring+="\n";
                        val=buff.Find(searchstring,position);
                        if (val==-1) {
                            searchstring=s;
                            val=buff.Find(searchstring,position);
                            if (val==-1)
                                return;
                        }
                    }
                }
            }
        }
    
        edit.SetSel(val,val+s.GetLength(),FALSE);        
        searchstring=s;
        position=val+1;
    }
}

void CSaisieView::OnSearchagain() 
{
	// TODO: Add your command handler code here
    if (searchstring!="") {
        CEdit& edit=GetEditCtrl();
        CString buff;
        edit.GetWindowText(buff);
        int val=buff.Find(searchstring,position);
        if (val==-1) {
            AfxMessageBox("String not found",MB_OK);
            searchstring="";
            position=0;
            return;
        }        
        edit.SetSel(val,val+searchstring.GetLength(),FALSE);
        position=val+1;
    }
}

void CSaisieView::OnSearchRulenumber() {
	CEdit& edit=GetEditCtrl();

	int first;
	int last;
	edit.GetSel(first,last);
	CString texte;
	edit.GetWindowText(texte);
	CString docname=GetDocument()->docname;
	char ch[100];
	ch[0]=0;
	int i;

	for (i=first;i>=0;i--) {
		if (texte[i]==13)
			break;
	}

	int ledebut=i+1;
	for (i=first;i<texte.GetLength();i++) {
		if (texte[i]==13)
			break;
	}
	int lafin=i;
	CString maligne=texte.Mid(ledebut,lafin-ledebut);
	maligne=maligne.Trim();

	int index=ReturnRuleNumber(docname,first,maligne);

	if (index!=-1) {
		sprintf(ch,"Rule number is:%d",index);            
		AfxMessageBox(ch,MB_OK);
	}
}
