#if !defined(AFX_FILENAMEINPUT_H__9500F9B0_F698_4C43_B7C7_989E953F2417__INCLUDED_)
#define AFX_FILENAMEINPUT_H__9500F9B0_F698_4C43_B7C7_989E953F2417__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// filenameinput.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// filenameinput dialog

class filenameinput : public CDialog
{
// Construction
public:
	filenameinput(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(filenameinput)
	enum { IDD = IDD_Filenameinput };
	CString	m_filename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(filenameinput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(filenameinput)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILENAMEINPUT_H__9500F9B0_F698_4C43_B7C7_989E953F2417__INCLUDED_)
