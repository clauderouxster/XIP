// RuleBuilding.cpp : implementation file
//

#include "stdafx.h"
#include "xipwin.h"
#define XIPLIBXML
#include "pasofficiel.h"
#include "xipwinDoc.h"
#include "xipwinView.h"

#include "RuleBuilding.h"
#include "generate.h"

#include "inforule.h"
#include "ruledoc.h"
#include "saisiedoc.h"
#include "saisieview.h"
#include "parametrefile.h"
#include "Featuretree.h"
#include "printvar.h"
#include "Filetree.h"
#include "ListeVariable.h"

extern char* field_name[];
extern char* noms_champ[];
extern char* rule_name[];

extern vector<string> liste_categories;
extern vector<string> liste_dependances;
extern CMultiDocTemplate* pGrTemplate;
extern CMultiDocTemplate* pDocTemplate;
extern CMultiDocTemplate* pRuleTemplate;
extern CMultiDocTemplate* pEditTemplate;
extern CXipwinDoc* docoutput;
extern int parseurCourant;

extern string xip_lemma;
extern string xip_surface;

void readTextFromFile(DecompteRegle* dr,DecompteRegle* drnext,char* buffer_read);
void FormatRuleText(CString& ruletext,int j,int breakpoint,BOOL);
XIPLONG ReturnDisplayMode();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern VECTA<Label*>* labels_numero;
extern VECTA<Label*>* labels_index;
extern CMultiDocTemplate* pEditTemplate;
extern int theRuleLimit;

char* insertwindowsnewline(string& texte);

/////////////////////////////////////////////////////////////////////////////
// RuleBuilding dialog


IMPLEMENT_DYNCREATE(RuleBuilding, CFormView)

RuleBuilding::RuleBuilding() : CFormView(IDD_createrule)
{
	//{{AFX_DATA_INIT(RuleBuilding)
	m_rule = _T("");
	m_focus = _T("");
	m_depsize = _T("1");
	m_catdep = _T("");
	m_breakpoint = -1;
	m_runbar = 100;
	m_fenetre = 1;
	m_ruletype = _T("");
	m_numsentence = 0;
	m_longestmatch = FALSE;
	m_lastwordindex = FALSE;
	m_layernumber = 0;
	m_outof = _T("");
	//}}AFX_DATA_INIT
	numero_ordre=-1;
    root=NULL;          
    index_node=0;    
    updatearbre=FALSE;
    updatedependency=FALSE;    
    m_createdependency=TRUE;
    m_createsequence=FALSE;
    m_createtagging=FALSE;
    m_createid=FALSE;
    m_createterm=FALSE;
    m_createmarkingrule=FALSE;    
	int i;
    for (i=labels_numero->dernier-1;i>=0;i--) {
        if ((*labels_numero)[i]!=NULL && (*labels_numero)[i]->dr!=NULL)            
            break;        
    }
	if (theRuleLimit==0) {
		m_runbar=0;
		m_breakpoint=0;
	}
	else
		m_breakpoint=i+1;
    typecontext=0;
    createrule=FALSE;
    right_boundary=NULL;
    left_boundary=NULL;
    left_rule="";
    right_rule="";
    base_rule="";
    head_rule="";
    lemma="";
    m_generate=FALSE;
    current_fileparameter=NULL;
    nb_features=0;
    nb_nodes=0;
    nb_readings=0;
}

RuleBuilding::~RuleBuilding() {
    list_nodes.nettoie();
    itemparameters.nettoie();
    list_feature_dependency.nettoie();
    delete rg;
}

void NumberIndexNode(XipNode* node,int& val) {    
    node->index=val++;
    for (int i=0;i<node->daughters.size();i++)
        NumberIndexNode(node->daughters[i],val);
    
}

void RuleBuilding::LeafAsFeature(MyTree& tree,XipLeaf* leaf,HTREEITEM racine,htreeitem_node* rootitem) {        
    CString surf=leaf->surface.c_str();
    //First we look for <features> node
    HTREEITEM feat=NULL;
    HTREEITEM hItem;
    HTREEITEM element;
    htreeitem_node* newitemroot=NULL;
    
    
    //We loop to find if there is such a node attached to racine
    if (m_arbre.ItemHasChildren(racine)) {
        hItem = m_arbre.GetChildItem(racine);
        
        while (hItem != NULL) {
            if (m_arbre.GetItemText(hItem)=="<features>") {
                feat=hItem;
                break;
            }
            hItem = m_arbre.GetNextSiblingItem(hItem);
        }
    }
    
    //If there no such node, we create it
    if (feat==NULL)
        feat=tree.InsertItem("<features>",racine);                           
    
    
    CString valsl=xip_surface.c_str();
    valsl+=":\"";
    valsl+=leaf->surface.c_str();
    valsl+="\"";
    BOOL found=FALSE;
    //We loop to find if there is already a surface node attached to features
    if (m_arbre.ItemHasChildren(feat)) {
        hItem = m_arbre.GetChildItem(feat);
        
        while (hItem != NULL) {
            if (m_arbre.GetItemText(hItem)==valsl) {
                found=TRUE;
                break;
            }
            hItem = m_arbre.GetNextSiblingItem(hItem);
        }
    }
    if (found==FALSE) {
        element=tree.InsertItem(valsl,feat);                            
        if (rootitem!=NULL) {
            newitemroot=new htreeitem_node(NULL,valsl,element);
            rootitem->addElement(newitemroot);
            items.ajoute(element);
            list_nodes.ajoute(newitemroot);
        }
    }
    
    valsl=xip_lemma.c_str();
    valsl+=":\"";
    valsl+=leaf->lemma.c_str();
    valsl+="\"";
    found=FALSE;
    //We loop to find if there is already a surface node attached to features
    if (m_arbre.ItemHasChildren(feat)) {
        hItem = m_arbre.GetChildItem(feat);
        
        while (hItem != NULL) {
            if (m_arbre.GetItemText(hItem)==valsl) {
                found=TRUE;
                break;
            }
            hItem = m_arbre.GetNextSiblingItem(hItem);
        }
    }
    if (found==FALSE) {
        element=tree.InsertItem(valsl,feat);            
        if (rootitem!=NULL) {
            newitemroot=new htreeitem_node(NULL,valsl,element);
            rootitem->addElement(newitemroot);
            items.ajoute(element);
            list_nodes.ajoute(newitemroot);
        }
    }
}


void RuleBuilding::LeafAsNode(MyTree& tree,XipLeaf* leaf,HTREEITEM racine,htreeitem_node* rootitem) {        

    LeafAsFeature(tree,leaf,racine,rootitem);

    CString surf=leaf->surface.c_str();
    //First we look for <features> node
    HTREEITEM feat=NULL;
    HTREEITEM catitem=NULL;
    HTREEITEM hItem;
    HTREEITEM element;
    
    htreeitem_node* newitemroot=NULL;
    htreeitem_node* leafitemroot=NULL;
    CString cat=leaf->category.c_str();
    cat+="(reading)";
    nb_readings++;
    int i;

    //First we try to find a node with the same category
    if (m_arbre.ItemHasChildren(racine)) {
        hItem = m_arbre.GetChildItem(racine);
        
        while (hItem != NULL) {
            if (m_arbre.GetItemText(hItem)==cat) {
                catitem=hItem;
                break;
            }
            hItem = m_arbre.GetNextSiblingItem(hItem);
        }
    }

	catitem=NULL;
    //If this category does not exist, we do create it
    if (catitem==NULL) {
        catitem=tree.InsertItem(cat,racine);
        if (rootitem!=NULL) {                    
            leafitemroot=new htreeitem_node(leaf,cat,catitem);                    
            rootitem->addElement(leafitemroot);
            list_nodes.ajoute(leafitemroot);
            items.ajoute(catitem);
        }        
    }
    else {
        if (rootitem!=NULL) {
            for (i=0;i<items.dernier;i++) {
                if (items[i]==catitem) {
                    leafitemroot=list_nodes[i];
                    break;
                }
            }
        }
    }
    
    //Then we add the features    
    if (leaf->features.size()>0 && tree==m_arbre) {
        for (i=0;i<leaf->features.size();i++) {            
            CString att=leaf->features[i]->attribute.c_str();            
            att+=":";
            att+=leaf->features[i]->value.c_str();        
            feat=tree.InsertItem(att,catitem);                 
            if (rootitem!=NULL) {
                newitemroot=new htreeitem_node(NULL,att,feat);
                leafitemroot->addElement(newitemroot);
                items.ajoute(feat);            
                list_nodes.ajoute(newitemroot);
            }
        }
    }
       
    CString valsl=xip_lemma.c_str();
    valsl+=":\"";
    valsl+=leaf->lemma.c_str();
    valsl+="\"";
    BOOL found=FALSE;
    //We loop to find if there is already a surface node attached to features
    if (m_arbre.ItemHasChildren(catitem)) {
        hItem = m_arbre.GetChildItem(catitem);
        
        while (hItem != NULL) {
            if (m_arbre.GetItemText(hItem)==valsl) {
                found=TRUE;
                break;
            }
            hItem = m_arbre.GetNextSiblingItem(hItem);
        }
    }
    if (found==FALSE) {
        element=tree.InsertItem(valsl,catitem);            
        if (rootitem!=NULL) {
            newitemroot=new htreeitem_node(NULL,valsl,element);
            leafitemroot->addElement(newitemroot);
            items.ajoute(element);
            list_nodes.ajoute(newitemroot);
        }
    }  
}



void RuleBuilding::ConstructTree(MyTree& tree,XipNode* node,HTREEITEM racine,htreeitem_node* rootitem) {    
    //We add the features
    int i;
    char ch[10];
    HTREEITEM element;
    HTREEITEM feat;
    
    htreeitem_node* newitemroot=NULL;

    if (node->Type()==XIPNODE) {                

        if (node->features.size()>0) {
            element=tree.InsertItem("<features>",racine);                    
            if (rootitem!=NULL) {
                newitemroot=new htreeitem_node(NULL,"<features>",element);
                rootitem->addElement(newitemroot);
                items.ajoute(element);            
                list_nodes.ajoute(newitemroot);                
            }

            for (i=0;i<node->features.size();i++) {
                CString att=node->features[i]->attribute.c_str();                
                att+=":";
                att+=node->features[i]->value.c_str();        
                feat=tree.InsertItem(att,element);                 
                if (rootitem!=NULL) {
                    newitemroot=new htreeitem_node(NULL,att,feat);
                    rootitem->addElement(newitemroot);
                    items.ajoute(feat);            
                    list_nodes.ajoute(newitemroot);
                    nb_features++;
                }
            }
        }

        if (tree==m_arbre && node->daughters.size()!=0 && node->daughters[0]->Type()==XIPNODE)
            m_arbre.Expand(racine,TVE_EXPAND);

        for (i=0;i<node->daughters.size();i++) {
            if (node->daughters[i]->Type()==XIPNODE) {
                sprintf(ch,"%d:",node->daughters[i]->index);
                CString val=ch;
                val+=node->daughters[i]->category.c_str(); 
                if (node->daughters[i]->daughters.size()>0 &&
                    node->daughters[i]->daughters[0]->Type()==XIPLEAF) {
                    val+="_";
                    val+=((XipLeaf*)node->daughters[i]->daughters[0])->surface.c_str();
                }
                element=tree.InsertItem(val,racine);                
                
                if (rootitem!=NULL) {                    
                    newitemroot=new htreeitem_node(node->daughters[i],val,element);                    
                    rootitem->addElement(newitemroot);
                    list_nodes.ajoute(newitemroot);
                    items.ajoute(element);
                    nb_nodes++;
                }
            }
            else {
                element=racine;
                newitemroot=rootitem;
            }
            ConstructTree(tree,node->daughters[i],element,newitemroot);
        }
    }
    else {
        if ((*labels_numero)[m_breakpoint-1]!=NULL && 
            (
                (*labels_numero)[m_breakpoint-1]->dr==NULL ||
                (*labels_numero)[m_breakpoint-1]->dr->regle==REGLES_DEDUCTION ||
                (*labels_numero)[m_breakpoint-1]->dr->regle==REGLES_EXTRACTION)
           )
            LeafAsFeature(tree,(XipLeaf*)node,racine,rootitem);
        else
            LeafAsNode(tree,(XipLeaf*)node,racine,rootitem);
    }
}

void RuleBuilding::ConstructDependency(XipDependency* dep,HTREEITEM asroot) {
    HTREEITEM racine;    
    HTREEITEM feat;
    HTREEITEM element;
    int i;
	if (asroot==NULL)
		racine=m_dependances.InsertItem(dep->name.c_str());    
	else
		racine=m_dependances.InsertItem(dep->name.c_str(),asroot);    
    
    htreeitem_node* deproot=new htreeitem_node(NULL,"",racine);
    itemsdependencies.ajoute(racine);    
    list_feature_dependency.ajoute(deproot);

    if (dep->features.size()>0) {
        feat=m_dependances.InsertItem("<features>",racine);                
        for (i=0;i<dep->features.size();i++) {
            CString att=dep->features[i]->attribute.c_str();
            att+=":";
            att+=dep->features[i]->value.c_str();
            element=m_dependances.InsertItem(att,feat);                            
            htreeitem_node* depitem=new htreeitem_node(NULL,att,element);
            deproot->addElement(depitem);
        }
    }
    
    for (i=0;i<dep->parameters.size();i++) {
        static char ch[1000];
        XipNode* nodex=dep->parameters[i];
        int valind=nodex->index;
        sprintf(ch,"%d:",nodex->index);
        CString val=ch;
        val+=nodex->category.c_str();        
        if (nodex->daughters[0]->Type()==XIPLEAF) {
            val+="_";
            val+=((XipLeaf*)nodex->daughters[0])->surface.c_str();
        }

        feat=m_dependances.InsertItem(val,racine);        
        htreeitem_node* depparam=new htreeitem_node(nodex,"",feat);
        itemparameters.ajoute(depparam);
        //ConstructTree(m_dependances,dep->parameters[i],feat,NULL);
    }

	for (i=0;i<dep->subdependencies.size();i++)
		ConstructDependency(dep->subdependencies[i],racine);	

	m_dependances.Expand(racine,TVE_COLLAPSE);
}


void RuleBuilding::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) {
    //Construction recursive
    HTREEITEM racine;
    CString val;
    char ch[100];

    if (items.dernier!=0)
        return;

    xipsent= ((CRuleDoc*)GetDocument())->xipsent;
    if (xipsent==NULL)
        return;
    int i=0;
	XIPLONG opt=ReturnDisplayMode();
	if ((opt&DISPLAY_GENERATION)==DISPLAY_GENERATION) {
		m_sentences.clear();
		m_sentences.push_back(((CRuleDoc*)GetDocument())->sentence);
	}
	else {
		for (i=0;i<xipsent->sentences.size();i++) {
			long left=xipsent->sentences[i]->root->left;
			long right=xipsent->sentences[i]->root->right;
			CString sent=((CRuleDoc*)GetDocument())->sentence.Mid(left,right-left);
			m_sentences.push_back(sent);
		}
	}

    sprintf(ch,"Out of %d parsed sentences",xipsent->sentences.size());
    m_outof=ch;
    m_arbre.ptrb=this;
    m_dependances.ptrb=this;
    m_dependances.dependency=TRUE;

    index_node=0;
    NumberIndexNode(xipsent->sentences[0]->root,index_node);    
    val=xipsent->sentences[0]->root->category.c_str();        
    rg=new RuleGeneration(xipsent->sentences[0]);    
    racine=m_arbre.InsertItem(val);    
    m_arbre.Expand(racine,TVE_EXPAND);
    root=new htreeitem_node(xipsent->sentences[0]->root,val,racine);       
    
    vector<int> rulenumbers;
    
    XipReturnRules(parseurCourant,rulenumbers);
    for (int j=rulenumbers.size()-1;j>=0;j--) {
        sprintf(ch,"%d",rulenumbers[j]);
        m_appliedrules.AddString(ch);
    }


    items.ajoute(racine);
    list_nodes.ajoute(root);
    ConstructTree(m_arbre,xipsent->sentences[0]->root,racine,root);   
    //we then add the dependencies
    for (i=0;i<xipsent->sentences[0]->dependencies.size();i++)
        ConstructDependency(xipsent->sentences[0]->dependencies[i],NULL);  
    m_arbre.Expand(racine,TVE_EXPAND);    	        
    
    if (XipMessageLanguage(parseurCourant)==1)
        fieldkeywords=field_name;
    else
        fieldkeywords=noms_champ;

    m_listcatdep.ResetContent();
    for (i=0;i<liste_dependances.size();i+=2)
        m_listcatdep.AddString(liste_dependances[i].c_str());
    m_listcatdep.SetCurSel(1);
    Ondependencyrule();

}

void RuleBuilding::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RuleBuilding)
	DDX_Control(pDX, IDC_appliedrules, m_appliedrules);
	DDX_Control(pDX, IDC_catdeplist, m_listcatdep);
	DDX_Control(pDX, IDC_LISTSUBPART, m_listesubpart);
	DDX_Control(pDX, IDC_dependencies, m_dependances);
	DDX_Control(pDX, IDC_resultat, m_arbre);
	DDX_Text(pDX, IDC_EDIT1, m_rule);
	DDX_Text(pDX, IDC_editonfocus, m_focus);
	DDX_Text(pDX, IDC_depsize, m_depsize);
	DDX_Text(pDX, IDC_catdep, m_catdep);
	DDX_Text(pDX, IDC_breakpoint, m_breakpoint);
	DDX_Slider(pDX, IDC_runbar, m_runbar);
	DDX_Text(pDX, IDC_WINDOWREADING, m_fenetre);
	DDX_Text(pDX, IDC_ruletype, m_ruletype);
	DDX_Text(pDX, IDC_numsentence, m_numsentence);
	DDX_Check(pDX, IDC_longestmatch, m_longestmatch);
	DDX_Check(pDX, IDC_lastwordindex, m_lastwordindex);
	DDX_Text(pDX, IDC_Layernumber, m_layernumber);
	DDX_Text(pDX, IDC_outof, m_outof);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RuleBuilding, CFormView)
	//{{AFX_MSG_MAP(RuleBuilding)
	ON_BN_CLICKED(IDC_valueselection, Onvalueselection)
	ON_BN_CLICKED(IDC_focus, Onfocus)
	ON_BN_CLICKED(IDC_clearfocus, Onclearfocus)	
	ON_BN_CLICKED(IDC_BUTTONRUN, OnButtonrun)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_runbar, OnReleasedcapturerunbar)
	ON_BN_CLICKED(IDC_addone, Onaddone)
	ON_BN_CLICKED(IDC_minusone, Onminusone)
	ON_BN_CLICKED(IDC_info, Oninfo)
	ON_BN_CLICKED(IDC_editrule, Oneditrule)
	ON_BN_CLICKED(IDC_addrule, Onaddrule)
	ON_BN_CLICKED(IDC_dependencyrule, Ondependencyrule)
	ON_BN_CLICKED(IDC_sequencerule, Onsequencerule)
	ON_BN_CLICKED(IDC_taggingrule, Ontaggingrule)
	ON_BN_CLICKED(IDC_leftcontext, Onleftcontext)
	ON_BN_CLICKED(IDC_rightcontext, Onrightcontext)
	ON_BN_CLICKED(IDC_selectrule, Onselectrule)
	ON_BN_CLICKED(IDC_idrule, Onidrule)
	ON_BN_CLICKED(IDC_headtagging, Onheadtagging)
	ON_BN_CLICKED(IDC_nextsentence, Onnextsentence)
	ON_BN_CLICKED(IDC_previoussentence, Onprevioussentence)
	ON_BN_CLICKED(IDC_renumber, Onrenumber)
	ON_BN_CLICKED(IDC_addinfile, Onaddinfile)
	ON_BN_CLICKED(IDC_clearrulegeneration, Onclearrulegeneration)
	ON_BN_CLICKED(IDC_clearselection, Onclearselection)
	ON_LBN_DBLCLK(IDC_LISTSUBPART, OnDblclkListsubpart)
	ON_BN_CLICKED(IDC_featuredisplay, Onfeaturedisplay)
	ON_BN_CLICKED(IDC_termrule, Onterm)
	ON_BN_CLICKED(IDC_markingrule, Onmarkingrule)
	ON_BN_CLICKED(IDC_numvar, Onnumvar)
	ON_BN_CLICKED(IDC_seefiles, Onseefiles)
	ON_BN_CLICKED(IDC_deleterule, Ondeleterule)
	ON_LBN_DBLCLK(IDC_appliedrules, OnDblclkappliedrules)
	ON_BN_CLICKED(IDC_ListeVariables, OnListeVariables)
	ON_BN_CLICKED(IDC_seerules, Onseerules)
	//}}AFX_MSG_MAP
	ON_NOTIFY(TVN_SELCHANGED, IDC_dependencies, &RuleBuilding::OnTvnSelchangeddependencies)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RuleBuilding message handlers





void RuleBuilding::CreateRule()  {

	// TODO: Add your control notification handler code here
    
    UpdateData(TRUE);

    int indlist=m_listcatdep.GetCurSel();
    CString m_dependencyname;

    m_listcatdep.GetLBText(indlist,m_dependencyname);
    
    if (m_dependencyname=="" && m_createdependency==TRUE)
        return;

    VECTA<htreeitem_node*> choices;
    int i;

    htreeitem_node* hn;
    HTREEITEM hItem;
    VECTA<int> focus;
    int premier=-1;

    
    VECTA<SelectedNode*> nodeselection;
    VECTA<SelectedDependency*> dependencyselection;
    rg->numvar.raz();
    rg->num_var=1;
    for (i=0;i<index_node;i++)
        rg->numvar.ajoute(-1);

    int depsize;
    depsize=atoi((LPCTSTR)m_depsize);
    int argcounter=depsize;
    arbremaj();

    for (i=0;i<item_focus.dernier;i++) {
        premier=items.cherche(item_focus[i]);        
        hItem=items[premier];        
        if (hItem!=NULL && m_arbre.GetCheck(hItem)==TRUE) {
            hn=list_nodes[premier];
            if (item_focus.cherche(hItem)!=-1) {
                focus.ajoute(hn->node->index);
                if (rg->numvar[hn->node->index]==-1)
                    rg->numvar.affecte(hn->node->index,rg->num_var++);
            }
        }
    }

    SelectedNode* sn;
    for (i=0;i<items.dernier;i++) {        
        hItem=items[i];
        
        if (m_arbre.GetCheck(hItem)==TRUE) {            
            hn=list_nodes[i];
            XipNode* nd=hn->node;
            
            if (hn->node==NULL)
                nd=hn->mother->node;

            if (nd!=NULL) {
                if (typecontext==LEFTCONTEXT && left_boundary->index<=nd->index)
                    break;
                
                if (typecontext==RIGHTCONTEXT) {
                    if (right_boundary->index>=nd->index ||
                        nd->Type()==XIPLEAF)
                    continue;
                }
            }
            
            //CString am=m_arbre.GetItemText(hn->item);
            
            if (hn->node!=NULL)
                nodeselection.ajoute(new SelectedNode(nd,nd->index));                            
            else {//If it is a selected feature, then we add it to the previous node
                if (nodeselection.dernier!=0 && hn->mother!=NULL &&
                    nodeselection.fin()->node==hn->mother->node) {
                    if (hn->feature!="<features>" && hn->feature.Left(5)!="#rule")
                        nodeselection.fin()->features.push_back(LPCTSTR(hn->feature));            
                }
                else {
                    //If this node is not a possible target for this feature (it is not its mother node)
                    //we then create a ? node, if this node was not created already                    
                    sn=new SelectedNode(nd,nd->index);
                    sn->any=1;
                    nodeselection.ajoute(sn);
                    if (hn->feature!="<features>" && hn->feature.Left(5)!="#rule")
                        sn->features.push_back(LPCTSTR(hn->feature));
                }
            }
        }
    }
    
    for (i=0;i<itemsdependencies.dernier;i++) {
        hItem=itemsdependencies[i];
        if (m_dependances.GetCheck(hItem)==TRUE) {
            SelectedDependency* d=new SelectedDependency(xipsent->sentences[m_numsentence]->dependencies[i],i);
            //We the check to see whether a feature was also selected
            htreeitem_node* htn=list_feature_dependency[i];
            for (int hi=0;hi<htn->daughters.dernier;hi++) {
                if (m_dependances.GetCheck(htn->daughters[hi]->item)==TRUE &&
                    htn->daughters[hi]->feature.Left(5)!="#rule")
                    d->features.push_back(LPCTSTR(htn->daughters[hi]->feature));
            }
            dependencyselection.ajoute(d);         
        }
    }

    if (nodeselection.dernier==0)
        return;

    string rule_text;
    rg->categorydependency.clear();
    
    if (typecontext==SEQUENCERULE || typecontext==IDRULE) {
        left_boundary=nodeselection[0]->node;
        right_boundary=nodeselection.fin()->node;
    }

    if (typecontext==TAGGINGRULE) {
        left_boundary=nodeselection[0]->node->mother;
        right_boundary=left_boundary;
    }
    
    XipNode* boundary=NULL;
    if (typecontext==LEFTCONTEXT) {
        boundary=left_boundary;
        if (boundary==NULL)
            return;
    }

    if (typecontext==RIGHTCONTEXT) {
        boundary=right_boundary;    
        if (boundary==NULL)
            return;
    }
    
    if (typecontext==SEQUENCERULE && m_createterm==TRUE) {
        int lwi=0;
        if (m_lastwordindex==TRUE)
            lwi=nodeselection.dernier-1;

        if (nodeselection[lwi]==NULL || 
            nodeselection[lwi]->node==NULL ||            
            (nodeselection[lwi]->node->Type()!=XIPLEAF &&            
            nodeselection[lwi]->node->daughters[0]->Type()!=XIPLEAF)) {
            if (m_lastwordindex==TRUE)
                AfxMessageBox("The last node must be a lexical node in a term rule",MB_OK);
            else
                AfxMessageBox("The first node must be a lexical node in a term rule",MB_OK);
            nodeselection.nettoie();
            return;
        }
        //first we check if a lemma has been chosen
        XipLeaf* leaf=NULL;
        if (nodeselection[lwi]->node->Type()==XIPLEAF)
            leaf=(XipLeaf*)nodeselection[lwi]->node;
        else
            leaf=(XipLeaf*)nodeselection[lwi]->node->daughters[0];

        lemma=leaf->lemma.c_str();
        addstrings[0]=lemma;
        lemma+="\"";
    }
    
    rg->GenerateRule(focus,
        rule_text,
        nodeselection,        
        dependencyselection,
        boundary,
        LPCTSTR(m_dependencyname),
        depsize,typecontext);    
    
    nodeselection.nettoie();
    m_catdep="";
    for (i=0;i<rg->categorydependency.size();i++) {
        m_catdep+=rg->categorydependency[i].c_str();
        m_catdep+="\r\n";
    }


    m_rule=rule_text.c_str();
    switch(typecontext) {    
    case IDRULE:
    case SEQUENCERULE:
        base_rule=m_rule;
        break;
    case LEFTCONTEXT:
        left_rule=m_rule;
        break;
    case RIGHTCONTEXT:
        right_rule=m_rule;
        break;
    }
    m_rule+="\r\n";
    createrule=TRUE;
    UpdateData(FALSE);

}



void RuleBuilding::Onfocus() 
{
	// TODO: Add your control notification handler code here
    HTREEITEM hItem	= m_arbre.GetSelectedItem();    
    UpdateData(TRUE);

    if (hItem!=NULL) {
        if (m_arbre.GetItemText(hItem)=="<features>")
            return;

        HTREEITEM parent=m_arbre.GetParentItem(hItem);
        if (parent!=NULL && m_arbre.GetItemText(parent)=="<features>")
            return;

        int i=item_focus.cherche(hItem);
        if (i!=-1)
            item_focus.retireElement(i);
        else {
            item_focus.ajoute(hItem);
            m_arbre.SetCheck(hItem,TRUE);
        }
     
        m_focus="";
        for (i=0;i<item_focus.dernier;i++) {
            if (i)
                m_focus+=", ";
            m_focus+=m_arbre.GetItemText(item_focus[i]);
        }
        arbremaj();
        UpdateData(FALSE);
    }
    createrule=FALSE;
}






void RuleBuilding::Onclearfocus() 
{
    UpdateData(TRUE);
    m_focus="";
    item_focus.raz();    
    UpdateData(FALSE);
}




void RuleBuilding::arbremaj() {
    int i,j;
    CString vdep;
    CString varbre;
    
    for (j=0;j<list_nodes.dernier;j++) {
        if (list_nodes[j]->node==NULL)
            continue;                        
        varbre=m_arbre.GetItemText(list_nodes[j]->item);
        for (i=0;i<itemparameters.dernier;i++) {
            vdep=m_dependances.GetItemText(itemparameters[i]->item);                        
            if (vdep==varbre) {                                
                m_dependances.SetCheck(itemparameters[i]->item,m_arbre.GetCheck(list_nodes[j]->item));                
            }
        }        
    }   
}

void RuleBuilding::depmaj() {
    int i,j;
    CString vdep;
    CString varbre;
    //first we check the status of the current item that was selected
    if (m_dependances.current==NULL)
        return;

    BOOL status=m_dependances.GetCheck(m_dependances.current);
    vdep=m_dependances.GetItemText(m_dependances.current);
    for (i=0;i<itemparameters.dernier;i++) {
        if (itemparameters[i]->item==m_dependances.current)
            continue;
        varbre=m_dependances.GetItemText(itemparameters[i]->item);
        if (varbre==vdep)
            //They should all share the same status
            m_dependances.SetCheck(itemparameters[i]->item,status);
    }


    //Then we set the other nodes in the m_arbre
    for (i=0;i<itemparameters.dernier;i++) {
        vdep=m_dependances.GetItemText(itemparameters[i]->item);                        
        
        for (j=0;j<list_nodes.dernier;j++) {
            if (list_nodes[j]->node==NULL)
                continue;                        
            varbre=m_arbre.GetItemText(list_nodes[j]->item);
            if (vdep==varbre) 
                m_arbre.SetCheck(list_nodes[j]->item,m_dependances.GetCheck(itemparameters[i]->item));            
        }        
    }   
}






BOOL MyTree::PreTranslateMessage(MSG* pMsg) {

    UINT pflags;    
    CPoint point;
    HTREEITEM hitem;

    if (dependency==FALSE) {
        if(pMsg->message==WM_LBUTTONDOWN) {
            
            point=pMsg->pt;
            
            ScreenToClient(&point);                
            hitem = HitTest(point , &pflags);
            status=FALSE;
            if (hitem!=NULL) {
                for (int i=0;i<ptrb->list_nodes.dernier;i++) {
                    if (ptrb->list_nodes[i]->node==NULL)
                        continue;
                    if (hitem==ptrb->list_nodes[i]->item) {                    
                        status=TRUE;
                        break;
                    }
                }
            }
        }
        else {
            if (status==TRUE) {
                ptrb->arbremaj();
                status=FALSE;
            }
        }
        return CTreeCtrl::PreTranslateMessage(pMsg);
    }

    if(pMsg->message==WM_LBUTTONDOWN) {
        
        point=pMsg->pt;
        
        ScreenToClient(&point);                
        hitem = HitTest(point , &pflags);
        if (hitem!=NULL) {
            for (int i=0;i<ptrb->itemparameters.dernier;i++) {
                if (hitem==ptrb->itemparameters[i]->item) {                    
                    status=TRUE;
                    current=hitem;
                    break;
                }
            }
        }
    }
    else {
        if (status==TRUE) {
            ptrb->depmaj();
            status=FALSE;
            current=NULL;
        }
    }
    
    return CTreeCtrl::PreTranslateMessage(pMsg);
}


void RuleBuilding::Recreate() {
    int nbfeatures=nb_features;
    int nbnodes=nb_nodes;
    int nbreadings=nb_readings;

    m_generate=FALSE;
    createrule=FALSE;
    list_nodes.nettoie();
    itemparameters.nettoie();
    list_feature_dependency.nettoie();

    nb_features=0;
    nb_nodes=0;
    nb_readings=0;

    items.raz();
    itemsdependencies.raz();
    item_focus.raz();
    xipsent->sentences.clear();
	
	if (numero_ordre!=-1)
		XipSetRuleRankNumberLimit(parseurCourant,numero_ordre);
	else 
		XipSetRuleLimit(parseurCourant,m_breakpoint);	
	
	numero_ordre=-1;
    //XipParseString(LPCTSTR(sentences[m_numsentence]),parseurCourant,xipsent);     
    ostringstream os;
	XIPLONG opt=ReturnDisplayMode();
	string generee;
	if ((opt&DISPLAY_GENERATION)==DISPLAY_GENERATION) {
		XipGenerateFromStringXipResult(LPCTSTR(m_sentences[m_numsentence]),xipsent,parseurCourant,&os);
		generee="Generation:\r\n";
		generee+=XipLastGeneratedString(parseurCourant);
		generee+="\r\n";
		generee+="------------------------------------------------------------------\r\n";
	}
	else
		XipParseStringAndOS(LPCTSTR(m_sentences[m_numsentence]),parseurCourant,xipsent,&os,XML_NONE,1);     

	m_arbre.SetRedraw(FALSE);
	m_dependances.SetRedraw(FALSE);
    m_arbre.DeleteAllItems();
    m_dependances.DeleteAllItems();
    
    int i=0;
    CString val;
    HTREEITEM racine;    

    index_node=0;
	if (xipsent->sentences.size()>0) {
		NumberIndexNode(xipsent->sentences[0]->root,index_node);    
		val=xipsent->sentences[0]->root->category.c_str();        
		rg=new RuleGeneration(xipsent->sentences[0]);    
		racine=m_arbre.InsertItem(val);    
		root=new htreeitem_node(xipsent->sentences[0]->root,val,racine);             

		items.ajoute(racine);
		list_nodes.ajoute(root);
		ConstructTree(m_arbre,xipsent->sentences[0]->root,racine,root);   
		//we then add the dependencies
		for (i=0;i<xipsent->sentences[0]->dependencies.size();i++)
			ConstructDependency(xipsent->sentences[0]->dependencies[i],NULL);    
		m_arbre.Expand(racine,TVE_EXPAND);
	}
	m_arbre.SetRedraw(TRUE);
	m_dependances.SetRedraw(TRUE);
    m_rule=generee.c_str();
    
    int j;
    for (j=m_breakpoint-m_fenetre;j<=m_breakpoint+m_fenetre;j++)
        FormatRuleText(m_rule,j,m_breakpoint,TRUE);

    static vector<int> rulenumbers;
    rulenumbers.clear();
    static char ch[1000];
    XipReturnRules(parseurCourant,rulenumbers);
    m_appliedrules.ResetContent();
    for (j=rulenumbers.size()-1;j>=0;j--) {
        sprintf(ch,"%d",rulenumbers[j]);
        m_appliedrules.AddString(ch);
    }

    int idk=m_breakpoint-1;
    if (idk<0)
        idk=0;
    while (idk>0 && 
        ((*labels_numero)[idk]==NULL || 
        (*labels_numero)[idk]->dr==NULL)) idk--;

    m_ruletype=rule_name[(*labels_index)[idk]->dr->regle];
    m_ruletype.TrimRight();
    m_layernumber=(*labels_index)[idk]->dr->couche;
    switch((*labels_index)[idk]->dr->regle) {
    case REGLES_DEDUCTION:
    case REGLES_EXTRACTION:
        Ondependencyrule();
        break;
    case REGLES_SIMPLIFICATION:
        Ontaggingrule();
        break;
    case REGLES_DI:
        Onidrule();
        break;
    default:
        Onsequencerule();
    }
    
    addstrings[0]="";
    addstrings[1]="";
    addstrings[2]="";
    
    if (nbfeatures!=nb_features) {
        sprintf(ch," from %d to %d",nbfeatures,nb_features);
        if (nbfeatures>nb_features)
            addstrings[0]="Less features";
        if (nbfeatures<nb_features)
            addstrings[0]="More features";
        addstrings[0]+=ch;
    }
    
    if (nbnodes!=nb_nodes) {
        sprintf(ch," from %d to %d",nbnodes,nb_nodes);
        if (nbnodes<nb_nodes) 
            addstrings[1]="Nodes added";    
        if (nbnodes>nb_nodes)
            addstrings[1]="Nodes removed";
        addstrings[1]+=ch;
    }
    
    if (nbreadings && nb_readings && nbreadings!=nb_readings) {
        sprintf(ch," from %d to %d",nbreadings,nb_readings);
        if (nbreadings>nb_readings)
            addstrings[2]="Word Readings removed";
        
        if (nbreadings<nb_readings)
            addstrings[2]="Word Readings added";
        addstrings[2]+=ch;
    }

    if (nbfeatures==nb_features && nbnodes==nb_nodes && nbreadings==nb_readings)
        addstrings[0]="No chunking rule has applied";

	if (os.str().size()>0) {
		if (docoutput==NULL) {
			docoutput= (CXipwinDoc*)pDocTemplate->OpenDocumentFile(NULL);        
			docoutput->SetTitle("OUTPUT");
		}
		docoutput->texte=insertwindowsnewline(os.str());
		docoutput->UpdateAllViews(NULL);    
	}

    updatelist();
    UpdateData(FALSE);
}


void RuleBuilding::OnButtonrun() 
{
    UpdateData(TRUE);    
    Recreate();
    float max=m_breakpoint*100/labels_numero->dernier;
    m_runbar=(int)max;
    UpdateData(FALSE);    
}

void RuleBuilding::OnReleasedcapturerunbar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);    
    float max=labels_numero->dernier*m_runbar/100;    
    m_breakpoint=(int)max;
    
    Recreate();
    UpdateData(FALSE);    
	*pResult = 0;
}

void RuleBuilding::Onaddone() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);    
    m_breakpoint++;
    if (m_breakpoint>=labels_numero->dernier)
        m_breakpoint=labels_numero->dernier;
    Recreate();
    float max=m_breakpoint*100/labels_numero->dernier;
    m_runbar=(int)max;
    UpdateData(FALSE);    
	
}

void RuleBuilding::Onminusone() 
{
	UpdateData(TRUE);    
    m_breakpoint--;
    if (m_breakpoint<0)
        m_breakpoint=0;
    Recreate();
    float max=m_breakpoint*100/labels_numero->dernier;
    m_runbar=(int)max;
    UpdateData(FALSE);    

}

void RuleBuilding::Oninfo() 
{
    UpdateData(TRUE);
    Cinforule info;	
    info.start_rule=m_breakpoint;
    info.m_selection=m_breakpoint;
    info.removecomment=TRUE;
    if (info.DoModal()==IDOK) {
        if (info.m_selection!=-1) {
            m_breakpoint=info.m_selection;
            Recreate();
            float max=m_breakpoint*100/labels_numero->dernier;
            m_runbar=(int)max;
            UpdateData(FALSE);    
        }
    }
}

void RuleBuilding::Oneditrule() 
{
	// TODO: Add your control notification handler code here
	//The m_breakpoint corresponds to the rule we want to edit
    UpdateData(TRUE);    
    if (labels_index->dernier==0 || (*labels_numero)[m_breakpoint]==NULL || (*labels_index)[m_breakpoint]->dr==NULL)
        return;

	VECTA<Label*>& labels= *labels_index;
	if (labels[m_breakpoint]==NULL || labels[m_breakpoint]->dr==NULL)
        return;

	CString ruletext="";
	FormatRuleText(ruletext,m_breakpoint,-1,TRUE);
	int fi=ruletext.Find("\r\n");
	fi=ruletext.Find("\r\n",fi+1)+2;
	ruletext=ruletext.Mid(fi,ruletext.GetLength()-fi);
	ruletext=ruletext.Trim();


    CSaisieDoc* doc=NULL;
    DecompteRegle* dr=(*labels_index)[m_breakpoint]->dr;
    
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
		doc= (CSaisieDoc*)pEditTemplate->OpenDocumentFile(dr->nom_fichier);        
        if (doc==NULL)
            return;        
    }
    
    //We look for the last view that was added
    POSITION pos = doc->GetFirstViewPosition();    
    CSaisieView* pView=(CSaisieView*)doc->GetNextView(pos);                

	pView->SelectionOnString(ruletext);
    pView->GetParentFrame()->BringWindowToTop();

}

CString GetNameFromPath(CString name) {
	int fi=name.ReverseFind('\\');
	if (fi!=-1)
		name=name.Mid(fi+1,name.GetLength()-fi);
	return name;
}

void RuleBuilding::Onaddrule() 
{
    UpdateData(TRUE);    
    int index=m_breakpoint-1;
    if (m_generate==FALSE ||
        labels_numero->dernier==0 || 
        (*labels_numero)[index]==NULL || 
        (*labels_numero)[index]->dr==NULL)
        return;

	CString ruletext="";
	FormatRuleText(ruletext,index,-1,TRUE);
	int fi=ruletext.Find("\r\n");
	fi=ruletext.Find("\r\n",fi+1)+2;
	ruletext=ruletext.Mid(fi,ruletext.GetLength()-fi);
	ruletext=ruletext.Trim();

    CSaisieDoc* doc=NULL;
    DecompteRegle* dr=(*labels_index)[index]->dr;    
    DecompteRegle* drnext=(*labels_index)[index+1]->dr;    
    DecompteRegle* drprevious=(*labels_index)[index-1]->dr;    


    CString name=GetNameFromPath(dr->nom_fichier);           

    POSITION firstdoc=pEditTemplate->GetFirstDocPosition();
    BOOL found=FALSE;
    while (firstdoc!=NULL) {
        doc=(CSaisieDoc*)pEditTemplate->GetNextDoc(firstdoc);
		CString pat=GetNameFromPath(doc->GetPathName());
        if (pat==name) {
            found=TRUE;
            break;
        }
    }
    
    if (found==FALSE) {    
		doc= (CSaisieDoc*)pEditTemplate->OpenDocumentFile(dr->nom_fichier);        
        if (doc==NULL)
            return;        
    }

	int firstcar,lastcar;
    POSITION pos = doc->GetFirstViewPosition();    
    CSaisieView* pView=(CSaisieView*)doc->GetNextView(pos);                
	pView->IndexSelectionOnString(ruletext,firstcar,lastcar);

	if (firstcar==-1)
		return;
    
    //We look for the last view that was added
    typeRegle tr=REGLES_DEDUCTION;
    if (m_createsequence==TRUE)
        tr=REGLES_SEQUENCE;
        else
            if (m_createid==TRUE)
                tr=REGLES_DI;
            else
                if (m_createtagging==TRUE)
                    tr=REGLES_SIMPLIFICATION;
                else
                    if (m_createterm==TRUE)
                        tr=REGLES_TERME;
                    else
                        if (m_createmarkingrule==TRUE)
                            tr=REGLES_MARQUAGE;
    
    
    char ch[100];
    ch[0]=0;
    typeRegle add_keyword=VIDE;

    int relative_layer=-1;
    int absolute_layer=-1;
    int nbch=0;
    if (dr->couche!=-1) {
        relative_layer=dr->couche_relative;
        absolute_layer=dr->couche;
        sprintf(ch,"%d> ",relative_layer);
        nbch=strlen(ch);
    }

   //
    //If the type of that rule is different and it is in the same file,
    //first we need to renumber the next rules
    //We have two different cases:
    //a) Either the previous rule is different
    //b) Either the next is rule is different
    if (dr->regle!=tr) {
        if (drnext!=NULL) {
            if (drnext->regle!=tr) {         
                if (drnext->couche!=-1) {                                   
                    //First we renumber the next lines...                
                    pView->Renumber(relative_layer,index+1,1,2,FALSE);
                    //Then we renumber up to lastcar
                    relative_layer++;
                    absolute_layer++;
                    pView->Renumber(relative_layer,index,-1,2,FALSE); 
                    if (name==GetNameFromPath(drnext->nom_fichier))
                        add_keyword=drnext->regle;
                    
                    sprintf(ch,"%d> ",relative_layer);
                    nbch=strlen(ch);
                    
                    //The new relative_layer is dr->couche+1 (the new rule is inserted after this one)
                    //we also have to add the field keyword
                    sprintf(ch,"%s\r\n\r\n%d> ",fieldkeywords[tr],relative_layer);
                }
                else {                    
                    sprintf(ch,"%s\r\n\r\n1> ",fieldkeywords[tr]);
                    nbch=3;
                    if (name==GetNameFromPath(drnext->nom_fichier))
                        add_keyword=drnext->regle;
                }
            }
            else {
                if (drnext->couche!=-1) {
                    relative_layer=drnext->couche_relative;
                    absolute_layer=drnext->couche;
                    sprintf(ch,"%d> ",relative_layer);
                    nbch=strlen(ch);
                }
            }
        }
        else {
            sprintf(ch,"%s\r\n\r\n1> ",fieldkeywords[tr]);        
            nbch=3;
        }
    }
    else {
        //we have to find out whether the keyword is in the way
        if (drnext!=NULL && dr->regle!=drnext->regle) {
            firstcar=dr->position-1;
            int v=0;
            while (fieldkeywords[drnext->regle][v]!=0) {
                firstcar++;
                while (tolower((char)pView->buffer[firstcar])!=fieldkeywords[drnext->regle][0]) firstcar++;
                v=1;
                int p=firstcar+1;
                while (fieldkeywords[drnext->regle][v]!=0 && tolower((char)pView->buffer[p])==fieldkeywords[drnext->regle][v]) {
                    p++;
                    v++;
                }                
            }
            lastcar=firstcar-2;
        }
    }
    

    CString toAdd="\r\n\r\n//Automatic based on:";    
    toAdd+=m_sentences[m_numsentence];
    toAdd+="\r\n";
    toAdd+=ch;
    m_rule.TrimRight();
    firstcar=toAdd.GetLength()-nbch;    
    toAdd+=m_rule;
    toAdd+="\r\n";

    if (add_keyword!=VIDE) {
        toAdd+="\r\n";
        toAdd+=fieldkeywords[add_keyword];
        toAdd+="\r\n\r\n";
    }

    pView->InsertText(toAdd,firstcar,lastcar,index,tr,relative_layer,absolute_layer);
    //m_breakpoint+=1;
    UpdateData(FALSE);
    
}

void RuleBuilding::Ondependencyrule() 
{
	// TODO: Add your control notification handler code here
    m_listcatdep.ResetContent();
    for (int i=0;i<liste_dependances.size();i+=2)
        m_listcatdep.AddString(liste_dependances[i].c_str());
    m_listcatdep.SetCurSel(1);
    createrule=FALSE;
	m_createdependency=TRUE;
    m_createsequence=FALSE;
    m_createtagging=FALSE;
    m_createid=FALSE;
    m_createterm=FALSE;
    m_createmarkingrule=FALSE;
    resetlist();
    CheckRadioButton(IDC_dependencyrule, IDC_markingrule,IDC_dependencyrule);
    CWnd* message=GetDlgItem(IDC_toggledepcat);
    message->SetWindowText("Dependency:");
    message->ShowWindow(SW_SHOW);
    message=GetDlgItem(IDC_catdeplist);
    message->ShowWindow(SW_SHOW);

    message=GetDlgItem(IDC_focusmessage);    
    message->ShowWindow(SW_SHOW);
    message=GetDlgItem(IDC_editonfocus);
    message->ShowWindow(SW_SHOW);

    message=GetDlgItem(IDC_longestmatch);    
    message->ShowWindow(SW_HIDE);

    message=GetDlgItem(IDC_lastwordindex);    
    message->ShowWindow(SW_HIDE);

    CButton* focus=(CButton*)GetDlgItem(IDC_focus);
    focus->ShowWindow(SW_SHOW);
    focus=(CButton*)GetDlgItem(IDC_clearfocus);
    focus->ShowWindow(SW_SHOW);

    CButton* left=(CButton*)GetDlgItem(IDC_leftcontext);
    CButton* right=(CButton*)GetDlgItem(IDC_rightcontext);
    CButton* select=(CButton*)GetDlgItem(IDC_selectrule);
    CButton* head=(CButton*)GetDlgItem(IDC_headtagging);
    
    left->ShowWindow(SW_HIDE);
    right->ShowWindow(SW_HIDE);
    select->ShowWindow(SW_HIDE);
    head->ShowWindow(SW_HIDE);
    
    CWnd* staticdep=(CWnd*)GetDlgItem(IDC_STATICDEP);
    CWnd* depsize=(CWnd*)GetDlgItem(IDC_depsize);
    staticdep->ShowWindow(SW_SHOW);
    depsize->ShowWindow(SW_SHOW);
    

}

void RuleBuilding::Onsequencerule() 
{
	// TODO: Add your control notification handler code here
    m_listcatdep.ResetContent();
    for (int i=0;i<liste_categories.size();i+=2)
        m_listcatdep.AddString(liste_categories[i].c_str());
    m_listcatdep.SetCurSel(1);
    createrule=FALSE;
	m_createdependency=FALSE;
    m_createsequence=TRUE;
    m_createtagging=FALSE;    
    m_createid=FALSE;
    m_createterm=FALSE;
    m_createmarkingrule=FALSE;
    resetlist();
    CheckRadioButton(IDC_dependencyrule, IDC_markingrule,IDC_sequencerule);    
    CWnd* message=GetDlgItem(IDC_toggledepcat);
    message->SetWindowText("Category:");
    message->ShowWindow(SW_SHOW);
    message=GetDlgItem(IDC_catdeplist);
    message->ShowWindow(SW_SHOW);

    message=GetDlgItem(IDC_focusmessage);    
    message->ShowWindow(SW_HIDE);
    message=GetDlgItem(IDC_editonfocus);
    message->ShowWindow(SW_HIDE);

    message=GetDlgItem(IDC_longestmatch);    
    message->ShowWindow(SW_SHOW);

    message=GetDlgItem(IDC_lastwordindex);    
    message->ShowWindow(SW_HIDE);

    CButton* focus=(CButton*)GetDlgItem(IDC_focus);
    focus->ShowWindow(SW_HIDE);
    focus=(CButton*)GetDlgItem(IDC_clearfocus);
    focus->ShowWindow(SW_HIDE);

    CButton* left=(CButton*)GetDlgItem(IDC_leftcontext);
    CButton* right=(CButton*)GetDlgItem(IDC_rightcontext);
    CButton* select=(CButton*)GetDlgItem(IDC_selectrule);
    CButton* head=(CButton*)GetDlgItem(IDC_headtagging);
    left->ShowWindow(SW_SHOW);
    right->ShowWindow(SW_SHOW);
    select->ShowWindow(SW_SHOW);
    head->ShowWindow(SW_HIDE);
    select->SetWindowText("Select Body");
    
    CWnd* staticdep=(CWnd*)GetDlgItem(IDC_STATICDEP);
    CWnd* depsize=(CWnd*)GetDlgItem(IDC_depsize);
    staticdep->ShowWindow(SW_HIDE);
    depsize->ShowWindow(SW_HIDE);
	
}

void RuleBuilding::Ontaggingrule() 
{
	// TODO: Add your control notification handler code here
    m_listcatdep.ResetContent();
    for (int i=0;i<liste_categories.size();i+=2)
        m_listcatdep.AddString(liste_categories[i].c_str());
    m_listcatdep.SetCurSel(1);
    createrule=FALSE;
	m_createdependency=FALSE;
    m_createsequence=FALSE;
    m_createtagging=TRUE;
    m_createid=FALSE;
    m_createterm=FALSE;
    m_createmarkingrule=FALSE;
    resetlist();
    CheckRadioButton(IDC_dependencyrule, IDC_markingrule, IDC_taggingrule);	
    CWnd* message=GetDlgItem(IDC_toggledepcat);
    message->ShowWindow(SW_HIDE);
    message=GetDlgItem(IDC_catdeplist);
    message->ShowWindow(SW_HIDE);

    message=GetDlgItem(IDC_focusmessage);    
    message->ShowWindow(SW_HIDE);
    message=GetDlgItem(IDC_editonfocus);
    message->ShowWindow(SW_HIDE);

    message=GetDlgItem(IDC_longestmatch);    
    message->ShowWindow(SW_HIDE);

    message=GetDlgItem(IDC_lastwordindex);    
    message->ShowWindow(SW_HIDE);

    CButton* focus=(CButton*)GetDlgItem(IDC_focus);
    focus->ShowWindow(SW_HIDE);
    focus=(CButton*)GetDlgItem(IDC_clearfocus);
    focus->ShowWindow(SW_HIDE);

    CButton* left=(CButton*)GetDlgItem(IDC_leftcontext);
    CButton* right=(CButton*)GetDlgItem(IDC_rightcontext);
    CButton* head=(CButton*)GetDlgItem(IDC_headtagging);
    CButton* select=(CButton*)GetDlgItem(IDC_selectrule);
    left->ShowWindow(SW_SHOW);
    right->ShowWindow(SW_SHOW);
    select->ShowWindow(SW_SHOW);
    head->ShowWindow(SW_SHOW);
    select->SetWindowText("Keep Categories");
    
    CWnd* staticdep=(CWnd*)GetDlgItem(IDC_STATICDEP);
    CWnd* depsize=(CWnd*)GetDlgItem(IDC_depsize);
    staticdep->ShowWindow(SW_HIDE);
    depsize->ShowWindow(SW_HIDE);

}


void RuleBuilding::Onleftcontext() 
{
	
    // TODO: Add your control notification handler code here
    m_generate=FALSE;
    if (createrule==FALSE) {
        AfxMessageBox("Please create a rule first",MB_OK);
        return;
    }


    typecontext=LEFTCONTEXT;
    left_rule="";
	CreateRule();
    addstrings[1]=left_rule;
    updatelist();
    UpdateData(FALSE);
}

void RuleBuilding::Onrightcontext() 
{
	// TODO: Add your control notification handler code here
    m_generate=FALSE;
    if (createrule==FALSE) {
        AfxMessageBox("Please create a rule first",MB_OK);
        return;
    }
    typecontext=RIGHTCONTEXT;
    right_rule="";
	CreateRule();
    addstrings[3]=right_rule;
    updatelist();
    UpdateData(FALSE);
}

void RuleBuilding::Onvalueselection() {
    UpdateData(TRUE);

    if (m_createdependency==TRUE) {
        typecontext=DEPENDENCYRULE;
        CreateRule();
        m_generate=TRUE;
        return;
    }
    
    if (m_createmarkingrule==TRUE) {
        typecontext=SEQUENCERULE;
        CreateRule();
        m_generate=TRUE;
        return;
    }

    if (base_rule=="")
        return;
    m_generate=TRUE;
    int indlist=m_listcatdep.GetCurSel();
    CString m_dependencyname;

    m_listcatdep.GetLBText(indlist,m_dependencyname);
    if (m_dependencyname=="")
        return;

    
    if (m_createtagging==TRUE) {
        m_rule=head_rule;
        m_rule+=" = ";
        m_rule+=left_rule;
        m_rule+=" ";
        m_rule+=base_rule;
        if (right_rule!="") {
            m_rule+=" ";
            m_rule+=right_rule;
        }
        m_rule+=".\r\n";
        UpdateData(FALSE);
        return;
    }

    m_rule="";
    if (m_createterm==TRUE) {
        //We create the term part of the rule
        m_rule+="\"";
        m_rule+=lemma;
        m_rule+=" : ";
    }

    m_rule+=m_dependencyname;
    if (m_createid==TRUE)
        m_rule+=" -> ";
    else {
        m_rule+=" ";
        if ((m_createsequence==TRUE || m_createterm==TRUE) && m_longestmatch==TRUE)
            m_rule+="@";        
        if (m_createterm==TRUE && m_lastwordindex==TRUE)
            m_rule+="<";
        m_rule+="=";
    }
    m_rule+=left_rule;
    m_rule+=" ";
    m_rule+=base_rule;
    if (right_rule!="") {
        m_rule+=" ";
        m_rule+=right_rule;
    }
    m_rule+=".\r\n";
    UpdateData(FALSE);
}

void RuleBuilding::Onselectrule() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CString h=head_rule;
    resetlist();
    m_generate=FALSE;
    if (m_createid==TRUE)
        typecontext=IDRULE;
    else
        if (m_createtagging==TRUE) {
            head_rule=h;
            addstrings[0]=head_rule;
            typecontext=TAGGINGRULE;
        }
        else
            typecontext=SEQUENCERULE;    
    CreateRule();
    if (typecontext==TAGGINGRULE) {
        base_rule=m_rule;
        base_rule.TrimRight();
    }
    addstrings[2]=m_rule;
    updatelist();
    UpdateData(FALSE);
}

void RuleBuilding::Onidrule() 
{
	// TODO: Add your control notification handler code here
    m_listcatdep.ResetContent();
    for (int i=0;i<liste_categories.size();i+=2)
        m_listcatdep.AddString(liste_categories[i].c_str());
    m_listcatdep.SetCurSel(1);
    createrule=FALSE;
	m_createdependency=FALSE;
    m_createsequence=FALSE;
    m_createtagging=FALSE;    
    m_createid=TRUE;
    m_createterm=FALSE;
    m_createmarkingrule=FALSE;
    resetlist();
    CheckRadioButton(IDC_dependencyrule, IDC_markingrule,IDC_idrule);    
    CWnd* message=GetDlgItem(IDC_toggledepcat);
    message->SetWindowText("Category:");
    message->ShowWindow(SW_SHOW);
    message=GetDlgItem(IDC_catdeplist);
    message->ShowWindow(SW_SHOW);

    message=GetDlgItem(IDC_focusmessage);    
    message->ShowWindow(SW_HIDE);
    message=GetDlgItem(IDC_editonfocus);
    message->ShowWindow(SW_HIDE);

    message=GetDlgItem(IDC_longestmatch);    
    message->ShowWindow(SW_HIDE);

    message=GetDlgItem(IDC_lastwordindex);    
    message->ShowWindow(SW_HIDE);

    CButton* focus=(CButton*)GetDlgItem(IDC_focus);
    focus->ShowWindow(SW_HIDE);
    focus=(CButton*)GetDlgItem(IDC_clearfocus);
    focus->ShowWindow(SW_HIDE);

    CButton* left=(CButton*)GetDlgItem(IDC_leftcontext);
    CButton* right=(CButton*)GetDlgItem(IDC_rightcontext);
    CButton* select=(CButton*)GetDlgItem(IDC_selectrule);
    CButton* head=(CButton*)GetDlgItem(IDC_headtagging);
    left->ShowWindow(SW_SHOW);
    right->ShowWindow(SW_SHOW);
    select->ShowWindow(SW_SHOW);
    head->ShowWindow(SW_HIDE);	
    select->SetWindowText("Select Body");

    CWnd* staticdep=(CWnd*)GetDlgItem(IDC_STATICDEP);
    CWnd* depsize=(CWnd*)GetDlgItem(IDC_depsize);
    staticdep->ShowWindow(SW_HIDE);
    depsize->ShowWindow(SW_HIDE);

}

void RuleBuilding::Onheadtagging() 
{
	// TODO: Add your control notification handler code here
	head_rule="";
    lemma="";
    m_generate=FALSE;
    typecontext=TAGGINGRULE ;
    createrule=TRUE;
    CreateRule();    
    head_rule=m_rule;
    head_rule.TrimRight();
    addstrings[0]=head_rule;
    updatelist();
    UpdateData(FALSE);
}


void RuleBuilding::updatelist() {
    m_listesubpart.ResetContent();
    for (int i=0;i<4;i++) {
        addstrings[i].TrimRight();
        m_listesubpart.AddString(addstrings[i]);
    }
}

void RuleBuilding::resetlist() {
    right_boundary=NULL;
    left_boundary=NULL;
    m_generate=FALSE;
    head_rule="";
    lemma="";
    left_rule="";
    right_rule="";
    base_rule="";
    for (int i=0;i<4;i++)
        addstrings[i]="";
    updatelist();
    UpdateData(FALSE);
}

void RuleBuilding::Onnextsentence() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    if (m_numsentence>=m_sentences.size()-1)
        return;
    m_numsentence++;
    Recreate();
    UpdateData(FALSE);    
}


void RuleBuilding::Onprevioussentence() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    if (m_numsentence<=0)
        return;
    m_numsentence--;
    Recreate();
    UpdateData(FALSE);    
	
}

void RuleBuilding::Onrenumber() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);    
    if (labels_numero->dernier==0 || (*labels_numero)[m_breakpoint]==NULL || (*labels_numero)[m_breakpoint]->dr==NULL)
        return;

    CSaisieDoc* doc=NULL;
    
    
    DecompteRegle* dr=(*labels_numero)[m_breakpoint]->dr;       
    CString name=dr->nom_fichier;
    

    POSITION firstdoc=pEditTemplate->GetFirstDocPosition();
    BOOL found=FALSE;
    while (firstdoc!=NULL) {
        doc=(CSaisieDoc*)pEditTemplate->GetNextDoc(firstdoc);
        if (doc->GetPathName()==name) {
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

    pView->Renumber(dr->couche_relative,m_breakpoint,1,1,TRUE);
    Oneditrule();
	
}

void RuleBuilding::Onaddinfile() 
{
    // TODO: Add your control notification handler code here
    
    CParametreFile cparm;
    UpdateData(TRUE);    
    if (m_generate==FALSE)
        return;

    POSITION firstdoc=pEditTemplate->GetFirstDocPosition();
    BOOL found=FALSE;
    CSaisieDoc* doc;
    while (firstdoc!=NULL) {
        doc=(CSaisieDoc*)pEditTemplate->GetNextDoc(firstdoc);
        if (doc->parameter==FALSE)
            continue;
        if (current_fileparameter==doc) {
            found=TRUE;        
            cparm.filenames.push_back(doc->GetTitle());
            break;
        }
    }
    if (found=FALSE)
        current_fileparameter=NULL;
    
    cparm.filenames.push_back("New File");

    firstdoc=pEditTemplate->GetFirstDocPosition();
    while (firstdoc!=NULL) {
        doc=(CSaisieDoc*)pEditTemplate->GetNextDoc(firstdoc);
        if (doc->parameter==FALSE)
            continue;
        if (current_fileparameter!=doc)
            cparm.filenames.push_back(doc->GetTitle());
    }
    

    cparm.m_rule=m_rule;
    if (cparm.DoModal()==IDOK) {
        CString name=cparm.filename;
        
        firstdoc=pEditTemplate->GetFirstDocPosition();
        found=FALSE;        
        while (firstdoc!=NULL) {
            doc=(CSaisieDoc*)pEditTemplate->GetNextDoc(firstdoc);
            if (doc->parameter==FALSE)
                continue;
            if (doc->GetTitle()==name) {
                found=TRUE;
                break;
            }
        }
        
        if (found==FALSE) {    
            if (cparm.create==FALSE)
                doc= (CSaisieDoc*)pEditTemplate->OpenDocumentFile(cparm.filename);        
            else
                doc= (CSaisieDoc*)pEditTemplate->OpenDocumentFile(NULL);        
            if (doc==NULL)
                return;        
            doc->parameter=TRUE;            
        }
        
        //We look for the last view that was added
        POSITION pos = doc->GetFirstViewPosition();    
        CSaisieView* pView=(CSaisieView*)doc->GetNextView(pos);                
        
        typeRegle tr=REGLES_DEDUCTION;
        BOOL create_layer=FALSE;
        if (m_createsequence==TRUE) {
            create_layer=TRUE;
            tr=REGLES_SEQUENCE;
        }
        else
            if (m_createid==TRUE) {
                create_layer=TRUE;
                tr=REGLES_DI;
            }
            else
                if (m_createtagging==TRUE) {
                    create_layer=TRUE;
                    tr=REGLES_SIMPLIFICATION;
                }
                else
                    if (m_createterm==TRUE) {
                        create_layer=TRUE;
                        tr=REGLES_TERME;
                    }
                    else
                        if (m_createmarkingrule==TRUE) {
                            tr=REGLES_MARQUAGE;
                            create_layer=TRUE;
                        }
                        
        
        CString field="";
        pView->FindPreviousField(fieldkeywords,fieldnumber,field);    
        CString toAdd;
        if (field!=fieldkeywords[tr]) {
            toAdd+="\r\n";
            toAdd+=fieldkeywords[tr];
        }
        toAdd+="\r\n\r\n//Automatic based on:";    
        toAdd+=m_sentences[m_numsentence];
        toAdd+="\r\n";
        m_rule.TrimRight();
        if (create_layer==TRUE)
            toAdd+="1> ";
        toAdd+=m_rule;
        toAdd+="\r\n";
        
        pView->AddRule(toAdd);
        pView->GetParentFrame()->BringWindowToTop();  
        current_fileparameter=doc;
        UpdateData(FALSE);    
    }
}

void RuleBuilding::Onclearrulegeneration() 
{
	// TODO: Add your control notification handler code here
    resetlist();
    Onclearselection();
    createrule=FALSE;    
}

void RuleBuilding::Onclearselection() 
{
	// TODO: Add your control notification handler code here
    int j;
	for (j=0;j<list_nodes.dernier;j++)
        m_arbre.SetCheck(list_nodes[j]->item,FALSE);    
    for (j=0;j<itemsdependencies.dernier;j++)
        m_dependances.SetCheck(itemsdependencies[j],FALSE);
    for (j=0;j<list_feature_dependency.dernier;j++)
        m_dependances.SetCheck(list_feature_dependency[j]->item,FALSE);
    arbremaj();	
}

void RuleBuilding::OnDblclkListsubpart() 
{
	// TODO: Add your control notification handler code here
	int i=m_listesubpart.GetCurSel();
    m_listesubpart.GetText(i,m_rule);
    m_generate=FALSE;
    UpdateData(FALSE);
}

void RuleBuilding::Onfeaturedisplay() 
{
	// TODO: Add your control notification handler code here
    arbretrait* arbre=XipGetFeatureTree(parseurCourant);
	CFeaturetree cf;    
    cf.DoModal();
}

void RuleBuilding::Onterm() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
    m_listcatdep.ResetContent();
    for (int i=0;i<liste_categories.size();i+=2)
        m_listcatdep.AddString(liste_categories[i].c_str());
    m_listcatdep.SetCurSel(1);
    createrule=FALSE;
	m_createdependency=FALSE;
    m_createsequence=FALSE;
    m_createtagging=FALSE;    
    m_createid=FALSE;
    m_createterm=TRUE;
    m_createmarkingrule=FALSE;
    resetlist();
    CheckRadioButton(IDC_dependencyrule, IDC_markingrule,IDC_termrule);    
    CWnd* message=GetDlgItem(IDC_toggledepcat);
    message->SetWindowText("Category:");
    message->ShowWindow(SW_SHOW);
    message=GetDlgItem(IDC_catdeplist);
    message->ShowWindow(SW_SHOW);

    message=GetDlgItem(IDC_focusmessage);    
    message->ShowWindow(SW_HIDE);
    message=GetDlgItem(IDC_editonfocus);
    message->ShowWindow(SW_HIDE);

    message=GetDlgItem(IDC_longestmatch);    
    message->ShowWindow(SW_SHOW);

    message=GetDlgItem(IDC_lastwordindex);    
    message->ShowWindow(SW_SHOW);

    CButton* focus=(CButton*)GetDlgItem(IDC_focus);
    focus->ShowWindow(SW_HIDE);
    focus=(CButton*)GetDlgItem(IDC_clearfocus);
    focus->ShowWindow(SW_HIDE);

    CButton* left=(CButton*)GetDlgItem(IDC_leftcontext);
    CButton* right=(CButton*)GetDlgItem(IDC_rightcontext);
    CButton* select=(CButton*)GetDlgItem(IDC_selectrule);
    CButton* head=(CButton*)GetDlgItem(IDC_headtagging);
    left->ShowWindow(SW_SHOW);
    right->ShowWindow(SW_SHOW);
    select->ShowWindow(SW_SHOW);
    head->ShowWindow(SW_HIDE);
    select->SetWindowText("Select Body");
    
    CWnd* staticdep=(CWnd*)GetDlgItem(IDC_STATICDEP);
    CWnd* depsize=(CWnd*)GetDlgItem(IDC_depsize);
    staticdep->ShowWindow(SW_HIDE);
    depsize->ShowWindow(SW_HIDE);	
}

void RuleBuilding::Onmarkingrule() 
{
	// TODO: Add your control notification handler code here
    m_listcatdep.ResetContent();
    for (int i=0;i<liste_dependances.size();i+=2)
        m_listcatdep.AddString(liste_dependances[i].c_str());
    m_listcatdep.SetCurSel(1);
    createrule=FALSE;
	m_createdependency=FALSE;
    m_createsequence=FALSE;
    m_createtagging=FALSE;
    m_createid=FALSE;
    m_createterm=FALSE;
    m_createmarkingrule=TRUE;
    resetlist();
    CheckRadioButton(IDC_dependencyrule, IDC_markingrule,IDC_markingrule);
    CWnd* message=GetDlgItem(IDC_toggledepcat);
    message->SetWindowText("Dependency:");
    message->ShowWindow(SW_HIDE);
    message=GetDlgItem(IDC_catdeplist);
    message->ShowWindow(SW_HIDE);

    message=GetDlgItem(IDC_focusmessage);    
    message->ShowWindow(SW_HIDE);
    message=GetDlgItem(IDC_editonfocus);
    message->ShowWindow(SW_HIDE);

    message=GetDlgItem(IDC_longestmatch);    
    message->ShowWindow(SW_HIDE);

    message=GetDlgItem(IDC_lastwordindex);    
    message->ShowWindow(SW_HIDE);

    CButton* focus=(CButton*)GetDlgItem(IDC_focus);
    focus->ShowWindow(SW_HIDE);
    focus=(CButton*)GetDlgItem(IDC_clearfocus);
    focus->ShowWindow(SW_HIDE);

    CButton* left=(CButton*)GetDlgItem(IDC_leftcontext);
    CButton* right=(CButton*)GetDlgItem(IDC_rightcontext);
    CButton* select=(CButton*)GetDlgItem(IDC_selectrule);
    CButton* head=(CButton*)GetDlgItem(IDC_headtagging);
    
    left->ShowWindow(SW_HIDE);
    right->ShowWindow(SW_HIDE);
    select->ShowWindow(SW_HIDE);
    head->ShowWindow(SW_HIDE);
    
    CWnd* staticdep=(CWnd*)GetDlgItem(IDC_STATICDEP);
    CWnd* depsize=(CWnd*)GetDlgItem(IDC_depsize);
    staticdep->ShowWindow(SW_HIDE);
    depsize->ShowWindow(SW_HIDE);	
}

void RuleBuilding::Onnumvar() 
{
	// TODO: Add your control notification handler code here
    Cprintvar prt;
    prt.DoModal();
}

void RuleBuilding::Onseefiles() 
{
	// TODO: Add your control notification handler code here
    CFiletree file;
    file.DoModal();
}


void RuleBuilding::Ondeleterule() 
{
	// TODO: Add your control notification handler code here
    if (labels_numero->dernier==0 || (*labels_numero)[m_breakpoint]==NULL || (*labels_numero)[m_breakpoint]->dr==NULL)
        return;

    Oneditrule();
    CString message="Are you sure you want to delete the rule:";
    char ch[10];
    sprintf(ch,"%d",m_breakpoint);
    message+=ch;
	if (AfxMessageBox(message,MB_OKCANCEL)==IDCANCEL)
        return;

    UpdateData(TRUE);    

    CSaisieDoc* doc=NULL;
    DecompteRegle* dr=(*labels_numero)[m_breakpoint]->dr;
    DecompteRegle* drnext=NULL;
    if ((*labels_numero)[m_breakpoint+1]!=NULL)
        drnext=(*labels_numero)[m_breakpoint+1]->dr;
    
    CString name=dr->nom_fichier;        

    POSITION firstdoc=pEditTemplate->GetFirstDocPosition();
    BOOL found=FALSE;
    while (firstdoc!=NULL) {
        doc=(CSaisieDoc*)pEditTemplate->GetNextDoc(firstdoc);
        if (doc->GetPathName()==name) {
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
    long firstcar=dr->position;
    long lastcar;
    if (drnext!=NULL && name==drnext->nom_fichier)
        lastcar=drnext->position-1;
    else
        lastcar=strlen((char*)pView->buffer);

    pView->DeleteRule(firstcar,lastcar,m_breakpoint);
    pView->GetParentFrame()->BringWindowToTop();
    
    UpdateData(FALSE);    
    Oneditrule();
}

void RuleBuilding::OnDblclkappliedrules() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);    
    int i=m_appliedrules.GetCurSel();
    CString val;
	numero_ordre=m_appliedrules.GetCount()-i-1;
    m_appliedrules.GetText(i,val);
    m_breakpoint=atoi(LPCTSTR(val));
    Recreate();
    float max=m_breakpoint*100/labels_numero->dernier;
    m_runbar=(int)max;
    UpdateData(FALSE);    
	
}

void RuleBuilding::OnListeVariables() 
{
	// TODO: Add your control notification handler code here

    CListeVariable lv;
    lv.DoModal();
}

void RuleBuilding::Onseerules() 
{
	// TODO: Add your control notification handler code here
    //first we loop on the activated rules
    ostringstream os;
    vector<int> rulenumbers;
    
    XipReturnRules(parseurCourant,rulenumbers);
    int j;
    for (j=0;j<rulenumbers.size();j++)
        XipDisplayActivatedRules(parseurCourant,&os,rulenumbers[j], 5);

    m_rule="";
    int l=os.str().size();

    for (j=0;j<l;j++) {
        if (os.str()[j]=='\n' && os.str()[j-1]!='\r')
            m_rule+="\r";
        m_rule+=os.str()[j];
    }

    UpdateData(FALSE);   
}

void RuleBuilding::OnTvnSelchangeddependencies(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
