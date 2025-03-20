// wkiflibs.cpp : implementation file
//

#include "stdafx.h"
#include "KIFF.h"
#include "wkiflibs.h"
#include "afxdialogex.h"


// wkiflibs dialog

IMPLEMENT_DYNAMIC(wkiflibs, CDialogEx)

wkiflibs::wkiflibs(CWnd* pParent /*=NULL*/)
	: CDialogEx(wkiflibs::IDD, pParent)
	, kiflibspath(_T(""))
{

}

wkiflibs::~wkiflibs()
{
}

void wkiflibs::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_KIFLIBSPATH, kiflibspath);
}


BEGIN_MESSAGE_MAP(wkiflibs, CDialogEx)
	ON_BN_CLICKED(IDOK, &wkiflibs::OnBnClickedOk)
END_MESSAGE_MAP()


// wkiflibs message handlers


void wkiflibs::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
