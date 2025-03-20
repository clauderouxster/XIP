// Arguments.cpp : implementation file
//

#include "stdafx.h"
#include "KIFN.h"
#include "Arguments.h"
#include "afxdialogex.h"


// Arguments dialog

IMPLEMENT_DYNAMIC(Arguments, CDialogEx)

Arguments::Arguments(CWnd* pParent /*=NULL*/)
	: CDialogEx(Arguments::IDD, pParent)
	, arguments(_T(""))
{

}

Arguments::~Arguments()
{
}

void Arguments::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, arguments);
}


BEGIN_MESSAGE_MAP(Arguments, CDialogEx)
END_MESSAGE_MAP()


// Arguments message handlers
