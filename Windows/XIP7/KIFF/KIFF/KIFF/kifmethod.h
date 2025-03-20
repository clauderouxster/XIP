#pragma once
#include "afxwin.h"


// kifmethod dialog

class kifmethod : public CDialogEx
{
	DECLARE_DYNAMIC(kifmethod)

public:
	kifmethod(CWnd* pParent = NULL);   // standard constructor
	virtual ~kifmethod();
	vector<wstring> listes;
	wstring resultat;
	int x, y;
	CString lemessage;
	map<wstring, wstring> infos;

	BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_KIFMETHODES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CToolTipCtrl m_ToolTip;
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	CComboBox laliste;
	CString atype;
	afx_msg void OnCbnSelchangeCombo1();
	CString letexte;
	CString dernierechaine;
};
