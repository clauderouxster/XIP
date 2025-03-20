#if !defined(AFX_RULEBUILDING_H__D94EA966_FA9E_4B8A_9251_561D93D5ABB3__INCLUDED_)
#define AFX_RULEBUILDING_H__D94EA966_FA9E_4B8A_9251_561D93D5ABB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RuleBuilding.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// RuleBuilding dialog

class XipResult;
class XipNode;
class XipLeaf;
class RuleGeneration;
class CSaisieDoc;
class RuleBuilding;

class htreeitem_node {
public:

    XipNode* node;
    CString feature;
    htreeitem_node* mother;
    VECTA<htreeitem_node*> daughters;
    HTREEITEM item;

    htreeitem_node(XipNode* n,CString f,HTREEITEM i) {
        node=n;
        feature=f;
        item=i;
    }

    void addElement(htreeitem_node* hn) {
        hn->mother=this;
        daughters.ajoute(hn);        
    }
};

    
class MyTree : public CTreeCtrl {
public:
    HTREEITEM current;
    BOOL status;
    RuleBuilding* ptrb;
    BOOL dependency;
    BOOL PreTranslateMessage(MSG* pMsg);
    MyTree() {
        status=FALSE;
        current=NULL;
        dependency=FALSE;
    }
};



class RuleBuilding : public CFormView
{
// Construction
    DECLARE_DYNCREATE(RuleBuilding)

public:
	RuleBuilding();   // standard constructor
    void ConstructTree(MyTree& tree,XipNode*,HTREEITEM racine,htreeitem_node* rootitem);
    void ConstructDependency(XipDependency* dep,HTREEITEM root);
    //A tree like structure to record the linguistic information
    htreeitem_node* root;
    VECTA<HTREEITEM> theitems[2];
    VECTA<HTREEITEM> items;
    VECTA<HTREEITEM> itemsdependencies;  
    
    VECTA<htreeitem_node*> itemparameters;
    VECTA<htreeitem_node*> list_nodes;    
    VECTA<htreeitem_node*> list_feature_dependency;
    VECTA<HTREEITEM> item_focus;
    int index_node;

    BOOL m_createdependency;
    BOOL m_createsequence;
    BOOL m_createtagging;
    BOOL m_createid;
    BOOL m_createterm;
    BOOL m_generate;
    BOOL m_createmarkingrule;
    char** fieldkeywords;

    int nb_features;
    int nb_nodes;
	int numero_ordre;
    int nb_readings;


    XipResult* xipsent;    

    RuleGeneration* rg;
    vector<CString> m_sentences;
    char typecontext;
    BOOL createrule;
    XipNode* left_boundary;
    XipNode* right_boundary;
    CString left_rule;
    CString right_rule;
    CString base_rule;
    CString head_rule;
    CString addstrings[4];
    CSaisieDoc* current_fileparameter;
    CString lemma;

    ~RuleBuilding();        
    BOOL updatearbre;
    BOOL updatedependency;
    void arbremaj();
    void depmaj();
    void Recreate();
    void LeafAsFeature(MyTree& tree,XipLeaf* node,HTREEITEM racine,htreeitem_node* rootitem);
    void LeafAsNode(MyTree& tree,XipLeaf* node,HTREEITEM racine,htreeitem_node* rootitem);
    void CreateRule();
    void updatelist();
    void resetlist();
    

// Dialog Data
	//{{AFX_DATA(RuleBuilding)
	enum { IDD = IDD_createrule };
	CListBox	m_appliedrules;
	CComboBox	m_listcatdep;
	CListBox	m_listesubpart;
	MyTree	m_dependances;
	MyTree	m_arbre;
	CString	m_rule;
	CString	m_focus;
	CString	m_depsize;
	CString	m_catdep;
	int		m_breakpoint;
	int		m_runbar;
	int		m_fenetre;
	CString	m_ruletype;
	int		m_numsentence;
	BOOL	m_longestmatch;
	BOOL	m_lastwordindex;
	int		m_layernumber;
	CString	m_outof;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RuleBuilding)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support    
    virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint );
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(RuleBuilding)
	afx_msg void Onvalueselection();
	afx_msg void Onfocus();
	afx_msg void Onclearfocus();	
	afx_msg void OnButtonrun();
	afx_msg void OnReleasedcapturerunbar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void Onaddone();
	afx_msg void Onminusone();
	afx_msg void Oninfo();
	afx_msg void Oneditrule();
	afx_msg void Onaddrule();
	afx_msg void Ondependencyrule();
	afx_msg void Onsequencerule();
	afx_msg void Ontaggingrule();
	afx_msg void Onleftcontext();
	afx_msg void Onrightcontext();
	afx_msg void Onselectrule();
	afx_msg void Onidrule();
	afx_msg void Onheadtagging();
	afx_msg void Onnextsentence();
	afx_msg void Onprevioussentence();
	afx_msg void Onrenumber();
	afx_msg void Onaddinfile();
	afx_msg void Onclearrulegeneration();
	afx_msg void Onclearselection();
	afx_msg void OnDblclkListsubpart();
	afx_msg void Onfeaturedisplay();
	afx_msg void Onterm();
	afx_msg void Onmarkingrule();
	afx_msg void Onnumvar();
	afx_msg void Onseefiles();
	afx_msg void Ondeleterule();
	afx_msg void OnDblclkappliedrules();
	afx_msg void OnListeVariables();
	afx_msg void Onseerules();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangeddependencies(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RULEBUILDING_H__D94EA966_FA9E_4B8A_9251_561D93D5ABB3__INCLUDED_)
