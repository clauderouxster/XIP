// resultat.h : header file
//
#include "ncoord.h"
/////////////////////////////////////////////////////////////////////////////
// Cresultat view
class Cresultat : public CScrollView
{
	DECLARE_DYNCREATE(Cresultat)
public:
	Cresultat();			// protected constructor used by dynamic creation

// Attributes
public:
   NCoord* Ngraphe;
   long maxX;
   long maxY;
   long debutY;
   long kX;
   long kY;
   float px,py;
   long dx,dy,fx,fy;
   int horizontal,vertical;
   SIZE tailleScroll;
   	
// Operations
public:
   void Dessine(CDC*,NCoord*);
   long KX(long x);
   long KY(long y);

// Implementation
protected:
	virtual ~Cresultat();
	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view
	virtual	void OnInitialUpdate();		// first time after construct
	//virtual void OnUpdate(CView* v, LPARAM p, CObject* o);

	// Generated message map functions
	//{{AFX_MSG(Cresultat)
	afx_msg void OnEditCopy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
