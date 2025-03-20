#pragma once
#include "afxwin.h"


// kifmethods dialog

class kifmethods : public CDialogEx
{
	DECLARE_DYNAMIC(kifmethods)

public:
	kifmethods(CWnd* pParent = NULL);   // standard constructor
	virtual ~kifmethods();
	vector<wstring> listes;
	wstring resultat;
	CString lemessage;
	int x, y;
	map<wstring, wstring> infos;

	BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_KIFMETHODS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CToolTipCtrl m_ToolTip;
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	CString atype;
	CComboBox laliste;
	afx_msg void OnCbnSelchangeCombo1();
	CString letexte;
	CString dernierechaine;
};
