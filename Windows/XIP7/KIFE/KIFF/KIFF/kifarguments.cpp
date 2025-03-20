// kifarguments.cpp : implementation file
//

#include "stdafx.h"
#include "KIFF.h"
#include "kifarguments.h"
#include "afxdialogex.h"


// kifarguments dialog

IMPLEMENT_DYNAMIC(kifarguments, CDialogEx)

kifarguments::kifarguments(CWnd* pParent /*=NULL*/)
	: CDialogEx(kifarguments::IDD, pParent)
	, args(_T(""))
	, indication(_T(""))
{

}

kifarguments::~kifarguments()
{
}

void kifarguments::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, args);
	DDX_Text(pDX, IDC_Indication, indication);
}


BEGIN_MESSAGE_MAP(kifarguments, CDialogEx)
END_MESSAGE_MAP()


// kifarguments message handlers
