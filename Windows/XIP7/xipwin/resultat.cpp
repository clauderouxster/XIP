// resultat.cpp : implementation file
//

#include "stdafx.h"

#include "xipstlres.h"

#include "xipwin.h"
#include "ncoord.h"
#include "resultat.h"
#include "resdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#define quinze (float)15
#define dix (float)10

/////////////////////////////////////////////////////////////////////////////
// Cresultat

IMPLEMENT_DYNCREATE(Cresultat, CScrollView)



Cresultat::Cresultat()
{
  Ngraphe=NULL;
  dx=-1;fx=-1;dy=-1;fy=-1;
  horizontal=0;vertical=0;
}

Cresultat::~Cresultat()
{
 if (Ngraphe !=NULL) delete Ngraphe;
}


BEGIN_MESSAGE_MAP(Cresultat, CScrollView)
	//{{AFX_MSG_MAP(Cresultat)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Cresultat drawing
extern XipResult resxip;

void Cresultat::OnInitialUpdate()
{ 
 
    CScrollView::OnInitialUpdate();
    maxX=0;maxY=0;
    XipNode* root= ((CResdoc*)GetDocument())->root;
    VECTA<CString*>& vect=((CResdoc*)GetDocument())->chaines;
    Ngraphe= new NCoord(GetDC(),root,&maxX,&maxY,5,NULL,0);
    tailleScroll.cx = maxX+10; 
    debutY=maxY+30;
    if (vect.dernier>0) {
        CSize lchaine=GetDC()->GetTextExtent(*vect[0]);    
        maxY+= 30+(vect.dernier+3)*(lchaine.cy+6);
    }
    
    tailleScroll.cy = maxY+10;
    SetScrollSizes(MM_TEXT, tailleScroll);
    
}


long Cresultat::KX(long x)
 {
  return (x+(int)px-horizontal);
 }

long Cresultat::KY(long y)
 {
  return (y+(int)py-vertical);
 }

void Cresultat::Dessine(CDC* pDC,NCoord* N) {
    NCoord* n;
    
    pDC->TextOut(KX(N->xNom),KY(N->yNom),N->nom);
    n=N->fils;
    while (n) {
        pDC->MoveTo(KX(N->xCentreNom),KY(N->yBasCentre));
        pDC->LineTo(KX(n->xCentreNom),KY(N->yBasCentre));
        pDC->LineTo(KX(n->xCentreNom),KY(n->yHautCentre));
        n=n->suivant;
    }
    if (N->suivant) 
        Dessine(pDC,N->suivant);
    if (N->fils) 
        Dessine(pDC,N->fils);
}
 
void Cresultat::OnDraw(CDC* pDC) {
    RECT r;
    
    dx=-1;fx=-1;dy=-1;fy=-1;
    vertical=0;horizontal=0;
    GetClientRect(&r);
    
    
    if (r.right < maxX) px=quinze; else px = (r.right-(float)maxX)/2;
    if (r.bottom < maxY) py=dix;else py = (r.bottom-(float)maxY)/2;
    
    if (px < quinze) px=quinze;
    if (py<dix) py=dix;
    
    
    if (Ngraphe!=NULL) 
        Dessine(pDC,Ngraphe);

    pDC->MoveTo(dix,KY(debutY-10));
    pDC->LineTo(KX(maxX),KY(debutY-10));

    long maxPos=debutY;
    /*
    CSize lchaine;    
    lchaine=pDC->GetTextExtent("Dependencies:");

    pDC->TextOut(dix,KY(maxPos),"Dependencies:");
    maxPos+=lchaine.cy+10;
    VECTA<CString*>& vect=((CResdoc*)GetDocument())->chaines;
    for (int i=0;i<vect.dernier;i++) {
        lchaine=pDC->GetTextExtent(*vect[i]);
        pDC->TextOut(dix,KY(maxPos),*vect[i]);
        maxPos+=lchaine.cy+5;
    }
*/
    vertical=GetScrollPos(SB_VERT);
    horizontal=GetScrollPos(SB_HORZ);
    
}


/////////////////////////////////////////////////////////////////////////////
// Cresultat message handlers


															 
void Cresultat::OnEditCopy() 
{	RECT taille;
	taille.left=0;
	taille.right=(maxX+10)*40;
	taille.bottom=debutY*40;
	taille.top=0;
	OpenClipboard();
	EmptyClipboard();
	CMetaFileDC fiche;
	CDC hdc;
	hdc.CreateCompatibleDC(NULL);
	if (!fiche.CreateEnhanced(NULL,NULL,&taille,"ROUX Claude Copyrights 2001. XRCE Grenoble")) 
        return;
	fiche.SetAttribDC(hdc.m_hDC);
	float PX=px;
	float PY=py;
	px=quinze;py=dix;
	Dessine(&fiche,Ngraphe);
	px=PX;py=PY;
	HENHMETAFILE h=fiche.CloseEnhanced();
	SetClipboardData(CF_ENHMETAFILE,h);
	CloseClipboard();	
}





void Cresultat::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)  {
    if (dx!=-1) {
        CDC* pdc= GetDC();
        pdc->SetROP2(R2_NOTXORPEN);
        pdc->MoveTo((int)dx,(int)dy);
        pdc->LineTo((int)dx,(int)fy);
        pdc->LineTo((int)fx,(int)fy);
        pdc->LineTo((int)fx,(int)dy);
        pdc->LineTo((int)dx+1,(int)dy);
        dx=-1;
    }
    
    vertical=GetScrollPos(SB_VERT);
    CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void Cresultat::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)  {
    if (dx!=-1) {
        CDC* pdc= GetDC();
        pdc->SetROP2(R2_NOTXORPEN);
        pdc->MoveTo((int)dx,(int)dy);
        pdc->LineTo((int)dx,(int)fy);
        pdc->LineTo((int)fx,(int)fy);
        pdc->LineTo((int)fx,(int)dy);
        pdc->LineTo((int)dx+1,(int)dy);
        dx=-1;
    }
    
    horizontal=GetScrollPos(SB_HORZ);
    CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}
