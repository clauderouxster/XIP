// installeurView.cpp : implementation of the CInstalleurView class
//

#include "stdafx.h"
#include "installateur.h"

#include "installeurDoc.h"
#include "installeurView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstalleurView

IMPLEMENT_DYNCREATE(CInstalleurView, CView)

BEGIN_MESSAGE_MAP(CInstalleurView, CView)
	//{{AFX_MSG_MAP(CInstalleurView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstalleurView construction/destruction

CInstalleurView::CInstalleurView()
{
	// TODO: add construction code here

}

CInstalleurView::~CInstalleurView()
{
}

BOOL CInstalleurView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CInstalleurView drawing

void CInstalleurView::OnDraw(CDC* pDC)
{
	CInstalleurDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CInstalleurView printing

BOOL CInstalleurView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CInstalleurView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CInstalleurView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CInstalleurView diagnostics

#ifdef _DEBUG
void CInstalleurView::AssertValid() const
{
	CView::AssertValid();
}

void CInstalleurView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CInstalleurDoc* CInstalleurView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CInstalleurDoc)));
	return (CInstalleurDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInstalleurView message handlers
