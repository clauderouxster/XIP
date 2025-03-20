#pragma once


// Arguments dialog

class Arguments : public CDialogEx
{
	DECLARE_DYNAMIC(Arguments)

public:
	Arguments(CWnd* pParent = NULL);   // standard constructor
	virtual ~Arguments();

// Dialog Data
	enum { IDD = IDD_arguments };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString arguments;
};
