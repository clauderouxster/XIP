// Filetree.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#define XIPLIBXML
#include "xipstlres.h"
#include "pasofficiel.h"

#include "xipwin.h"

#include "MainFrm.h"

#include "saisiedoc.h"
#include "saisieview.h"

#include "Filetree.h"
#include "filenameinput.h"
#include <vector>

extern long offset_begins[3];
VECTA<rule_file*> new_documents;
extern CString grammarFileName[10];
extern int parseurCourant;

extern CMultiDocTemplate* pEditTemplate;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern vector<string> feature_pathnames;
extern vector<string> lexicon_pathnames;
extern vector<string> rule_pathnames;
extern vector<string> param_pathnames;
extern vector<string> scripts_pathnames;
extern vector<string> divers_pathnames;

extern string languageFilename;

/////////////////////////////////////////////////////////////////////////////
// CFiletree dialog


CFiletree::CFiletree(CWnd* pParent /*=NULL*/)
	: CDialog(CFiletree::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFiletree)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFiletree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFiletree)
	DDX_Control(pDX, IDC_pathfiletree, m_pathtree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFiletree, CDialog)
	//{{AFX_MSG_MAP(CFiletree)
	ON_NOTIFY(NM_DBLCLK, IDC_pathfiletree, OnDblclkpathfiletree)
	ON_BN_CLICKED(IDC_newfile, Onnewfile)
	ON_BN_CLICKED(IDC_deleterulefile, Ondeleterulefile)
	ON_BN_CLICKED(IDC_fileadding, Onfileadding)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFiletree message handlers

BOOL CFiletree::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    baseroot= m_pathtree.InsertItem("BASES");
    featureroot=m_pathtree.InsertItem("FEATURES");    
    lexiconroot=m_pathtree.InsertItem("LEXICONS");    
    ruleroot=m_pathtree.InsertItem("RULES");    
	scriptsroot=m_pathtree.InsertItem("SCRIPTS");  	
	diversroot=m_pathtree.InsertItem("MISCELLANEOUS");  	
    paramroot=m_pathtree.InsertItem("PARAMETERS");  

    int i;

    m_pathtree.InsertItem(grammarFileName[parseurCourant],baseroot);
    m_pathtree.InsertItem((char*)languageFilename.c_str(),baseroot);
    
    for (i=0;i<feature_pathnames.size();i++) {
        m_feature_pathnames.push_back(feature_pathnames[i]);
        m_pathtree.InsertItem((char*)feature_pathnames[i].c_str(),featureroot);
    }

    for (i=0;i<lexicon_pathnames.size();i++) {
        m_lexicon_pathnames.push_back(lexicon_pathnames[i]);
        m_pathtree.InsertItem((char*)lexicon_pathnames[i].c_str(),lexiconroot);
    }

    for (i=0;i<rule_pathnames.size();i++) {
        m_rule_pathnames.push_back(rule_pathnames[i]);
        m_pathtree.InsertItem((char*)rule_pathnames[i].c_str(),ruleroot);
    }

    for (i=0;i<param_pathnames.size();i++) {
        m_param_pathnames.push_back(param_pathnames[i]);
        m_pathtree.InsertItem((char*)param_pathnames[i].c_str(),paramroot);
    }

	for (i=0;i<divers_pathnames.size();i++) {
        m_divers_pathnames.push_back(divers_pathnames[i]);
        m_pathtree.InsertItem((char*)divers_pathnames[i].c_str(),diversroot);
    }

	for (i=0;i<scripts_pathnames.size();i++) {
        m_scripts_pathnames.push_back(scripts_pathnames[i]);
        m_pathtree.InsertItem((char*)scripts_pathnames[i].c_str(),scriptsroot);
    }
	
    m_pathtree.Expand(baseroot,TVE_EXPAND);
    m_pathtree.Expand(featureroot,TVE_EXPAND);
    m_pathtree.Expand(ruleroot,TVE_EXPAND);
    m_pathtree.Expand(lexiconroot,TVE_EXPAND);
    m_pathtree.Expand(paramroot,TVE_EXPAND);
    m_pathtree.Expand(diversroot,TVE_EXPAND);
    m_pathtree.Expand(scriptsroot,TVE_EXPAND);
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void RegenerateGRMFile(rule_file* rf) {

    //We reload the grm file
    CFile fichier(grammarFileName[parseurCourant],CFile::modeRead);
    
    CFileStatus status;
    fichier.GetStatus(status);
    char* buff=new char[status.m_size+1];        
    UINT nb=fichier.Read(buff,status.m_size+1);  
    buff[nb]=0;
    fichier.Close();
    
    CString bufstr=buff;
    delete buff;

    //bufstr comprises the whole set of files
    //We search for the first parameter line that correspond to previous
    CString prev=rf->previous;
    int pos=0;
    int posnext;
    CString val="parameter:\n";
    val+=rf->name;
    val+="\n";
    if (prev=="") {
        //We do not have any previous parameter file...
        //we then either reach the end of the file, or we go to the first parameter occurence
        posnext=bufstr.Find("parameter",pos);
        if (posnext==-1)
            //we simply add the file name at the end of the file
            bufstr+=val;            
    }
    else {
        //we have to find the position
        posnext=bufstr.Find(prev,pos);
        posnext+=prev.GetLength()+1;
    }
    
    if (posnext!=-1) 
        //we add then this new path at the right place
        bufstr.Insert(posnext,val);
    ofstream fichie(grammarFileName[parseurCourant],ios::binary);
    fichie<<LPCTSTR(bufstr);
    
}

void RemoveFileInGrm(CString& path) {

    //We reload the grm file
    CFile fichier(grammarFileName[parseurCourant],CFile::modeRead);
    
    CFileStatus status;
    fichier.GetStatus(status);
    char* buff=new char[status.m_size+1];        
    UINT nb=fichier.Read(buff,status.m_size+1);  
    buff[nb]=0;
    fichier.Close();
    
    CString bufstr=buff;
    delete buff;

    //bufstr comprises the whole set of files
    //We search for the first parameter line that correspond to previous
    //we have to find the position
    int pos=bufstr.Find(path,0);
    int posparam=bufstr.Find("parameter:",pos-14);
    if (posparam!=-1)
        bufstr.Delete(posparam,pos+path.GetLength()-posparam+1);    

    ofstream fichie(grammarFileName[parseurCourant],ios::binary);
    fichie<<LPCTSTR(bufstr);

}


void RegenerateGrammarFile(rule_file* rf) {
    if (rf->type==3)
        return;

    CFile fichier((char*)languageFilename.c_str(),CFile::modeRead);
    
    CFileStatus status;
    fichier.GetStatus(status);
    char* buff=new char[status.m_size+1];        
    UINT nb=fichier.Read(buff,status.m_size+1);  
    buff[nb]=0;
    fichier.Close();

    CString bufstr=buff;
    delete buff;
    CString prev=rf->previous;
    CString pathprevious;
    CString add;


    int pos;
    if (prev!="") {
        pos=prev.ReverseFind('\\');
        if (pos!=-1) {
            pathprevious=prev.Left(pos+1);
            prev=prev.Right(prev.GetLength()-pos-1);
        }
        
        
        //We have the position in the string of each of the fields
        //given by XipGrammarPathnames, we search our file name from that position
        pos=bufstr.Find(prev,offset_begins[rf->type]);
        if (pos==-1)
            return;
        
        //On se positionne apres le nom
        pos+=prev.GetLength();
        add=", ";
        if (rf->type==2)
            add+="+";        
        if (rf->name.Find(pathprevious)!=-1)
            rf->name.Delete(0,pathprevious.GetLength());
        add+=rf->name;
    }
    else {
        add=rf->name;
        pos=add.ReverseFind('\\');
        if (pos!=-1)            
            add=add.Right(add.GetLength()-pos-1);   
        pathprevious=" ";
        pathprevious+=add;
        pathprevious+=", ";
        add=pathprevious;
        pos=offset_begins[rf->type];
    }
    
    
    bufstr.Insert(pos,add);
    CString mm=languageFilename.c_str();    
    ofstream fichie(mm,ios::binary);
    fichie<<LPCTSTR(bufstr);
 
}

void CFiletree::OnOK() 
{
	// TODO: Add extra validation here
    HTREEITEM hItem	= m_pathtree.GetSelectedItem();
    
    if (hItem==featureroot || hItem==lexiconroot || hItem==ruleroot || hItem==paramroot || hItem==diversroot || hItem==scriptsroot)
        return;

    CString pathname=m_pathtree.GetItemText(hItem);
    CSaisieDoc* doc=NULL;
    int i;
    rule_file* rf=NULL;

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
            //if it is a new document
            for (i=0;i<m_new_documents.size();i++) {                
                if (pathname==m_new_documents[i]->name) {
                    found=TRUE;
                    break;
                }
            }

            if (found==TRUE) {
                if (m_new_documents[i]->newdoc==TRUE) {
                    doc= (CSaisieDoc*)pEditTemplate->OpenDocumentFile(NULL);
                    doc->SetPathName(pathname);
                }
                else
                    doc= (CSaisieDoc*)pEditTemplate->OpenDocumentFile(pathname);
                
                m_new_documents[i]->doc=doc;                
                doc->parameter=TRUE;
            }
            else
                doc= (CSaisieDoc*)pEditTemplate->OpenDocumentFile(pathname);
            if (doc==NULL)
                return;        
        }

        feature_pathnames.clear();
        lexicon_pathnames.clear();
        rule_pathnames.clear();
        param_pathnames.clear();
        
        for (i=0;i<m_feature_pathnames.size();i++)
            feature_pathnames.push_back(m_feature_pathnames[i]);
        
        for (i=0;i<m_lexicon_pathnames.size();i++)
            lexicon_pathnames.push_back(m_lexicon_pathnames[i]);
        
        for (i=0;i<m_rule_pathnames.size();i++)
            rule_pathnames.push_back(m_rule_pathnames[i]);

        for (i=0;i<m_param_pathnames.size();i++)
            param_pathnames.push_back(m_param_pathnames[i]);
        
        //We look for the last view that was added
        POSITION pos = doc->GetFirstViewPosition();    
        CSaisieView* pView=(CSaisieView*)doc->GetNextView(pos);                                
        pView->GetParentFrame()->BringWindowToTop();
        BOOL modif=FALSE;
        BOOL modifgrm=FALSE;
        for (i=0;i<m_new_documents.dernier;i++) {
            if (m_new_documents[i]->type==3) {
                RegenerateGRMFile(m_new_documents[i]);
                modifgrm=TRUE;
            }
            else {
                modif=TRUE;
                RegenerateGrammarFile(m_new_documents[i]);
            }
        }

        if (modif==TRUE) {            
            CString bufstr="The file:";
            bufstr+=languageFilename.c_str();
            bufstr+=" has been modified";
            AfxMessageBox(bufstr,MB_OK);
        }

        if (modifgrm==TRUE) {            
            CString bufstr="The file:";
            bufstr+=grammarFileName[parseurCourant];
            bufstr+=" has been modified";
            AfxMessageBox(bufstr,MB_OK);
        }

        CDialog::OnOK();
    }	
}

void CFiletree::OnDblclkpathfiletree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnOK();
	*pResult = 0;
}

void CFiletree::Onnewfile() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hItem	= m_pathtree.GetSelectedItem();
    HTREEITEM current = hItem;
    HTREEITEM root=NULL;
    vector<string>* vect=NULL;
    char type=0;
    while (hItem!=NULL) {
        if (hItem==featureroot) {
            root=featureroot;
            vect=&m_feature_pathnames;
            break;
        }
        if (hItem ==lexiconroot) {
            root=lexiconroot;
            vect=&m_lexicon_pathnames;
            type=1;
            break;
        }
        if (hItem ==ruleroot) {
            root=ruleroot;
            vect=&m_rule_pathnames;
            type=2;
            break;
        }
        if (hItem ==paramroot) {
            root=paramroot;
            vect=&m_param_pathnames;
            type=3;            
            break;
        }
        hItem=m_pathtree.GetParentItem(hItem);
    }
    
    if (current!=NULL) {        
        //We add a new file
        filenameinput fileinput;
        CString pathname;
        if (fileinput.DoModal()==IDOK) {
            pathname=fileinput.m_filename;            
            if (pathname=="" ||
                pathname.Find("\\",0)!=-1) {
                AfxMessageBox("Please give a file name (without any path)",MB_OK);
                return;
            }
            if (pathname.Find(".xip",0)==-1)
                pathname+=".xip";
            //We then get the path from the current element
            CString path=pathname;
            vector<string>::iterator vi;
			BOOL trouve=FALSE;
            string pp="";
            if (current!=root) {
                path=m_pathtree.GetItemText(current);
                pp=LPCTSTR(path);
                //we look for that name in our vect...                
                for (vi=vect->begin();vi!=vect->end();vi++) { 
					if (*vi==pp) {
						trouve=TRUE;
                        break;
					}
                }
            }
            int i=path.ReverseFind('\\');
            if (i!=-1) {
                path=path.Left(i);
                path+="\\";
                path+=pathname;
            }

            HTREEITEM newroot;
            m_new_documents.ajoute(new rule_file(path,pp.c_str(),type,TRUE));
            pp=LPCTSTR(path);
            
            if (trouve==FALSE) {
                vi=vect->begin();
                newroot=m_pathtree.InsertItem(path,root,TVI_FIRST);
            }
            else {
                vi++;
                newroot=m_pathtree.InsertItem(path,root,current);
            }

            vect->insert(vi,pp);            
            m_pathtree.SelectItem(newroot);
        }
    }
}


void RemoveFile(CString& path,char type) {
    CFile fichier((char*)languageFilename.c_str(),CFile::modeRead);
    
    CFileStatus status;
    fichier.GetStatus(status);
    char* buff=new char[status.m_size+1];        
    UINT nb=fichier.Read(buff,status.m_size+1);  
    buff[nb]=0;
    fichier.Close();

    CString bufstr=buff;
    delete buff;
    
    CString name=path;
    //We first get the name
    int pos=name.ReverseFind('\\');
    if (pos!=-1)        
        name=name.Right(name.GetLength()-pos-1);    

    //Then we look for its position within the document
    //starting at offset_begins[type]
    pos=bufstr.Find(name,offset_begins[type]);
    //We have the position in the string of each of the fields
    //given by XipGrammarPathnames, we search our file name from that position    
    if (pos==-1)
        return;

    CString mma;

    int lengthToRemove=name.GetLength();
    mma=bufstr.Mid(pos,lengthToRemove);
    //if there is a plus attached to the name we also remove it
    while (bufstr[pos]!=':' && bufstr[pos]!=',') {
        pos--;
        lengthToRemove++;
    }
    
    if (bufstr[pos]==':') {        
        pos++;        
        lengthToRemove--;
        mma=bufstr.Mid(pos,lengthToRemove);
        //We check whether there is a comma after
        int comma=pos+lengthToRemove-1;        
        mma=bufstr.Mid(comma,lengthToRemove);
        while (comma<nb && bufstr[comma]!=',') {
            lengthToRemove++;
            comma++;
        }
        mma=bufstr.Mid(pos,lengthToRemove);
    }

    bufstr.Delete(pos,lengthToRemove);
    CString mm=languageFilename.c_str();    
    ofstream fichie(mm,ios::binary);
    fichie<<LPCTSTR(bufstr);
    bufstr="The file:";
    bufstr+=mm;
    bufstr+=" has been modified";
    AfxMessageBox(bufstr,MB_OK);
}

void CFiletree::Ondeleterulefile() 
{
    // TODO: Add your control notification handler code here
    if (AfxMessageBox("Are you sure you want to remove this file?",MB_OKCANCEL)==IDOK) {
        HTREEITEM hItem	= m_pathtree.GetSelectedItem();
        HTREEITEM current = hItem;
        HTREEITEM root=NULL;
        vector<string>* vect=NULL;
        char type=0;
        while (hItem!=NULL) {
            if (hItem==featureroot) {
                root=featureroot;
                vect=&feature_pathnames;
                break;
            }
            if (hItem ==lexiconroot) {
                root=lexiconroot;
                vect=&lexicon_pathnames;
                type=1;
                break;
            }
            if (hItem ==ruleroot) {
                root=ruleroot;
                vect=&rule_pathnames;
                type=2;
                break;
            }
            if (hItem ==paramroot) {
                root=paramroot;
                vect=&param_pathnames;
                type=3;
                break;
            }
            hItem=m_pathtree.GetParentItem(hItem);
        }
        vector<string>::iterator vi;    
        CString path=m_pathtree.GetItemText(current);
        string pp=LPCTSTR(path);
        //we look for that name in our vect...                
        for (vi=vect->begin();vi!=vect->end();vi++) { 
            if (*vi==pp) {
                vect->erase(vi);
                if (type!=3)
                    RemoveFile(path,type);
                else
                    RemoveFileInGrm(path);
                CDialog::OnOK();
                break;
            }
        }

    }
}

void CFiletree::Onfileadding() 
{
	// TODO: Add your control notification handler code here
    
	HTREEITEM hItem	= m_pathtree.GetSelectedItem();
    HTREEITEM current = hItem;
    HTREEITEM root=NULL;
    vector<string>* vect=NULL;
    char type=0;
    while (hItem!=NULL) {
        if (hItem==featureroot) {
            root=featureroot;
            vect=&m_feature_pathnames;
            break;
        }
        if (hItem ==lexiconroot) {
            root=lexiconroot;
            vect=&m_lexicon_pathnames;
            type=1;
            break;
        }
        if (hItem ==ruleroot) {
            root=ruleroot;
            vect=&m_rule_pathnames;
            type=2;
            break;
        }
        if (hItem ==paramroot) {
            root=paramroot;
            vect=&m_param_pathnames;
            type=3;
            break;
        }        
        hItem=m_pathtree.GetParentItem(hItem);
    }
    
    if (current!=NULL) {      
        CFileDialog addfile(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"File (*.xip)|*.xip|");
        
        if (addfile.DoModal()==IDCANCEL)
            return;
        
        CString pathname=addfile.GetPathName();
        //We then get the path from the current element
        CString path=pathname;
        vector<string>::iterator vi;
        vector<string>::iterator vin;
        string pp="";
		BOOL trouve=FALSE;
        if (current!=root) {
            path=m_pathtree.GetItemText(current);
            pp=LPCTSTR(path);
            //we look for that name in our vect...                            
            for (vi=vect->begin();vi!=vect->end();vi++) { 
				if (*vi==pp) {
                    trouve=TRUE;
					break;
				}
            }
        }
        
        HTREEITEM newroot;                
        
        if (trouve==FALSE) {
            vi=vect->begin();            
            newroot=m_pathtree.InsertItem(pathname,root,TVI_FIRST);
        }
        else {
            vi++;
            newroot=m_pathtree.InsertItem(pathname,root,current);            
        }

        m_new_documents.ajoute(new rule_file(pathname,pp.c_str(),type,FALSE));
        pp=LPCTSTR(pathname);
        vect->insert(vi,pp);
        m_pathtree.SelectItem(newroot);        
    }
}	
