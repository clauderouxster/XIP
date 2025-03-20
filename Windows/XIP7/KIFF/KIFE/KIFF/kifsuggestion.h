#pragma once
#include "afxwin.h"


// kifsuggestion dialog

class kifsuggestion : public CDialogEx
{
	DECLARE_DYNAMIC(kifsuggestion)

public:
	kifsuggestion(CWnd* pParent = NULL);   // standard constructor
	virtual ~kifsuggestion();
	vector<wstring> listes;
	wstring resultat;
	CComboBox laliste;

	BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_Suggestion };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangelistesuggestion();
	afx_msg void OnCbnEditchangelistesuggestion();
};
