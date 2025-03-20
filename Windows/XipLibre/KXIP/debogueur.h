#pragma once
#include "afxwin.h"
#include "afxdialogex.h"
#include "KIFFDoc.h"
#include "KIFFView.h"

// debogueur dialog

class debogueur : public CDialogEx
{
	DECLARE_DYNAMIC(debogueur)

public:
	debogueur(CWnd* pParent = NULL);   // standard constructor
	BOOL OnInitDialog( );
	virtual ~debogueur();
	void Lapile(string&);
	void Variables(string&);
	void Locals();
	long lignecourante;
	CString voutput;
	string localvariables;
	CKIFFView* currentview;
	vector<string> previousvariables;
// Dialog Data
	enum { IDD = IDD_DEBOGUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CToolTipCtrl m_ToolTip;
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSaute();
	afx_msg void OnBnClickedSortir();
	afx_msg void OnBnClickedFin();
	afx_msg void OnBnClickedSuivant();
	CString vvariables;
	CString vpile;
	afx_msg void OnBnClickedArretprg();
	afx_msg void OnBnClickedPointarret();
	CButton bsuivant;
	CButton bsaute;
	CButton bsortir;
	CButton bbreak;
	CButton barret;
	CButton bfin;
	CString vsortie;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit4();
	CString choixvariable;	
	afx_msg void OnEnChangeEdit5();
	LRESULT MAJ(WPARAM wParam, LPARAM lParam);
	CString locals;
	CEdit localvar;
	afx_msg void OnBnClickedEntre();
	CButton bentre;
	int threadid;
	BOOL stickthread;
	afx_msg void OnBnClickedStickToThread();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeCurrentVariable();
};
