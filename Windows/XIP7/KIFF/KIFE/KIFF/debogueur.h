#pragma once
#include "afxwin.h"
#include "afxdialogex.h"
#include "KIFFDoc.h"
#include "KIFFView.h"

#define MET_A_JOUR WM_APP+0x10
#define MET_A_JOUR_COULEUR MET_A_JOUR+1

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
	long lignecourante;
	CString voutput;
	string localvariables;
	void Locals();
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
	CEdit lecode;
	CString vlecode;
	afx_msg void OnBnClickedPointarret();
	CButton bsuivant;
	CButton bsaute;
	CButton bsortir;
	CButton bbreak;
	CButton barret;
	CButton bfin;
	CString vsortie;
	CString choixvariables;
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeEdit5();
	LRESULT MAJ(WPARAM wParam, LPARAM lParam);
	CString locals;
	CEdit localvar;
	CButton bentre;
	afx_msg void OnBnClickedEntre();
	int threadId;
	BOOL sticktothread;
	afx_msg void OnBnClickedSticktoThread();
	afx_msg void OnEnChangeEdit6();
};
