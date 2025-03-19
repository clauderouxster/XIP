// DirDialogDemoDlg.h : header file
//

#if !defined(AFX_DIRDIALOGDEMODLG_H__DA1AAF06_9FD7_11D3_BED6_000000000000__INCLUDED_)
#define AFX_DIRDIALOGDEMODLG_H__DA1AAF06_9FD7_11D3_BED6_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDirDialogDemoDlg dialog

class CDirDialogDemoDlg : public CDialog
{
// Construction
public:
	CDirDialogDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDirDialogDemoDlg)
	enum { IDD = IDD_DIRDIALOGDEMO_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirDialogDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDirDialogDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRDIALOGDEMODLG_H__DA1AAF06_9FD7_11D3_BED6_000000000000__INCLUDED_)
