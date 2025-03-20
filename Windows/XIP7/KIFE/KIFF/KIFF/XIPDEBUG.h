#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "xipstlres.h"
#include "xipvecta.h"
#include "pasofficiel.h"
// XIPDEBUG dialog

class XIPDEBUG : public CDialogEx
{
	DECLARE_DYNAMIC(XIPDEBUG)

public:
	XIPDEBUG(CWnd* pParent = NULL);   // standard constructor
	virtual ~XIPDEBUG();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	VECTA<Label*>* labels;
	int currentindex;
	CString currentrule;
	CString rulenumber;
	CTreeCtrl treenode;
	vector<int> rulenumbers;
	int position;
	XipResult* xipsent;
	string currentstring;
	int ruleslider;
	int parseurCourant;
    void ConstructTree(CTreeCtrl& tree,XipNode*,HTREEITEM racine);
    void LeafAsFeature(CTreeCtrl& tree,XipLeaf* node,HTREEITEM racine);
    void LeafAsNode(CTreeCtrl& tree,XipLeaf* node,HTREEITEM racine);
	void ConstructDependency(XipDependency* dep,HTREEITEM racine);
	void Onseerules();
	void Recreate();

	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CTreeCtrl thedependencies;
	CString laphrase;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
};
