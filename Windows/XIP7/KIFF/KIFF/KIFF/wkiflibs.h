#pragma once


// wkiflibs dialog

class wkiflibs : public CDialogEx
{
	DECLARE_DYNAMIC(wkiflibs)

public:
	wkiflibs(CWnd* pParent = NULL);   // standard constructor
	virtual ~wkiflibs();

// Dialog Data
	enum { IDD = IDD_DIALOGKIFLIBS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString kiflibspath;
};
