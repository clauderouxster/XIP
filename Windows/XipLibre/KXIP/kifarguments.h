#pragma once


// kifarguments dialog

class kifarguments : public CDialogEx
{
	DECLARE_DYNAMIC(kifarguments)

public:
	kifarguments(CWnd* pParent = NULL);   // standard constructor
	virtual ~kifarguments();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString args;
	CString indication;
};
