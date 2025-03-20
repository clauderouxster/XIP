#pragma once

#define IDD_arguments                   322
#define IDD_largument                   322
#define IDC_EDIT1                       1002
// Arguments dialog

class Arguments : public CDialogEx
{
	DECLARE_DYNAMIC(Arguments)

public:
	Arguments(CWnd* pParent = NULL);   // standard constructor
	virtual ~Arguments();

// Dialog Data
	enum { IDD = IDD_largument };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString arguments;
};
