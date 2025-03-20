// KIFFView.cpp : implémentation de la classe CKIFNView
//

#include "stdafx.h"
// SHARED_HANDLERS peuvent être définis dans les gestionnaires d'aperçu, de miniature
// et de recherche d'implémentation de projet ATL et permettent la partage de code de document avec ce projet.
#ifndef SHARED_HANDLERS
#include "KIFFN.h"
#endif

#include "KIFNDoc.h"
#include "KIFNView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "debogueur.h"

extern "C" {
#include "kifextern.h"
}

#include "Arguments.h"
#define ENDL "\r\n"
CString basearguments;

extern CMainFrame* leMain;
#include "x_node.h"

CString fromstring(string res) {
	wstring base;
	s_utf8tounicode(base,STR(res));
	CString commande;
	commande=(WCHAR*)base.c_str();
	return commande;
}


// CKIFNView
static UINT FindDialogMessage = ::RegisterWindowMessage(FINDMSGSTRING);

IMPLEMENT_DYNCREATE(CKIFNView, CRichEditView)

BEGIN_MESSAGE_MAP(CKIFNView, CRichEditView)
	// Commandes d'impression standard
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_FILE_PRINT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CKIFNView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CHAR()
	ON_COMMAND(ID_KIF_EXECUTER, &CKIFNView::OnKifExecuter)
	ON_COMMAND(ID_KIF_REEXECUTER, &CKIFNView::OnKifReexecuter)
	ON_COMMAND(ID_KIF_LISTER, &CKIFNView::OnKifLister)
	ON_COMMAND(ID_KIF_RAZ, &CKIFNView::OnKifRaz)
	ON_COMMAND(ID_KIF_PASAPAS, &CKIFNView::OnKifPasaPas)
	ON_COMMAND(ID_KIF_POINTARRET, &CKIFNView::OnKifPointarret)
	ON_COMMAND(ID_KIF_NETTOYERP, &CKIFNView::OnKifNettoyerp)
	ON_COMMAND(ID_KIF_INDENTER, &CKIFNView::OnKifIndenter)
	ON_REGISTERED_MESSAGE(FindDialogMessage, OnFindDialogMessage)
	ON_COMMAND(ID_EDITION_CHERCHE, &CKIFNView::OnEditionCherche)
	ON_COMMAND(ID_EDITION_REMPLACE, &CKIFNView::OnEditionRemplace)
	ON_COMMAND(ID_KIF_COMMENTER, &CKIFNView::OnKifCommenter)
	ON_COMMAND(ID_KIF_BREAK, &CKIFNView::OnKifBreak)
	ON_COMMAND(ID_KIF_COMMET32812, &CKIFNView::OnKifAsThread)
	ON_COMMAND(ID_KIF_CORRESPONDANCE, &CKIFNView::OnKifCorrespondance)
	ON_COMMAND(ID_KIF_COMPILER, &CKIFNView::OnKifCompiler)
	ON_COMMAND(ID_EDITION_SUIVANT, &CKIFNView::OnEditionSuivant)
	ON_COMMAND(ID_EDITION_ARGUMENTS, &CKIFNView::OnEditionArguments)
	ON_COMMAND(ID_EDITION_ALLERLIGNE, &CKIFNView::OnEditionAllerligne)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------------------------------
//-------------------------------COMMUNES-----------------------------------------------------------
// construction ou destruction de CKIFNView
static const char* coloration[]={"fonctionramassemiette","ramassedictionnaires","f_parcourirfichiers","tailleramassemiette","retireerreursurclef","attendquejoindre","ramassevecteurs","ramassechaines","vecteurmatrice","entréestandard","ramassenombres","ramassemiette","environnement","attendquefaux","ramasselistes","ramasseréels","affichelnerr","donnéesdebug","f_défilement","dictionnaire","f_noeudarbre","chargelocal","f_glissière","tâcheparent","polynomiale","f_parcourir","autoexécute","mavariable","afficheerr","ritérateur","intervalle","connecteur","attribute","itérateur","synchrone","exclusive","enattente","f_éditeur","afficheln","aléatoire","fonction","séquence","protégée","fraction","f_onglet","infopile","continue","instance","f_groupe","f_entrée","f_sortie","écritbin","sinonsi","tantque","f_image","affiche","importe","booléen","propres","idtâche","seconde","suspend","matrice","vecteur","version","f_table","pathnom","f_arbre","fichier","f_choix","f_boite","pasdans","fenêtre","renvoie","private","distant","nombres","adresse","stricte","chaines","hachage","touche","arrête","évalue","bouton","xmldoc","classe","sqlite","chrono","minute","chaine","nombre","libère","sortie","common","liste","rouey","parmi","tâche","sinon","appel","estun","capte","class","pause","début","rouex","const","tente","réels","pour","vrai","lève","faux","long","réel","port","enum","bool","dans","haut","toto","omni","weka","bits","arff","vide","tue","age","bit","lib","ici","bas","sys","xml","si","pas","défaut","curl","sinon","sinonsi","avec","0"};

void echange::initialisation(CKIFNView*v,CString& x) {
	compilation=false;
	encours=true;		
	vue=v->GetSafeHwnd();
	wvue=v;
	s=x;
}


hmap<wstring,bool> colorkeywords;

x_colorreading::x_colorreading() {
	name="%%STRING"; 
	pos=0;
	length=0;
	int ic=0;
	if (colorkeywords.size()==0) {
		while (coloration[ic][0]!='0') {
			wstring s;
			s_utf8tounicode(s,(char*)coloration[ic]);
			colorkeywords[s]=true;
			ic++;
		}
	}
}

void x_colorreading::loadtoken(bool keepspace) {
	unsigned char s,c;
	Initable();
	long line=0;
	wstring ch;
	wstring token;
	stacksize.clear();
	stackln.clear();	
	stacktype.clear();

	long pos,posd=0;		
	char lastc=0;
	wstring comments;
	s=get();
	pos=tell();
	while (!eof()) {
		if (s=='/' && token.size()==0) {
			s=get();
			if (s=='/') {
				comments.clear();
				comments+='/';
				comments+='/';
				//this is a comment
				s=get();
				comments+=s;
				while (s!=10 && s!=13 && !eof()) {
					s=get();
					comments+=s;
				}
				token.clear();
				//stack.push_back("");
				stacksize.push_back(comments.size());
				stackln.push_back(pos);
				stacktype.push_back(5);
				line++;
				lastc=s;
				s=get();
				pos=tell();	
				continue;
			}
			seek(pos);
			s='/';	
		}

		c=s;
		if (token.size()==0) {
			if (c=='"') {
				lastc=0;
				token=c;
				c=get();				
				while (c!='"' && !eof()) {
					if (c==10 && lastc==13)
						lastc=0;
					else
						if (c==10 || c==13) {
							lastc=c;
							line++;
						}						
						if (c=='\\') {
							token+=c;
							c=get();
						}						
						token+=c;
						c=get();
				}
				token+='"';
				//stack.push_back("");
				stacksize.push_back(token.size());
				stackln.push_back(pos);
				stacktype.push_back(1);
				token.clear();
				s=get();
				pos=tell();				
				continue;
			}
			if (c=='\'') {
				token=c;
				c=get();
				while (c!='\'' && !eof()) {
					if (c==10 && lastc==13)
						lastc=0;
					else
						if (c==10 || c==13) {
							lastc=c;
							line++;
						}						
						token+=c;
						c=get();
				}
				token+='\'';
				//stack.push_back("");				
				stacksize.push_back(token.size());
				stackln.push_back(pos);
				stacktype.push_back(2);
				token.clear();
				s=get();
				pos=tell();				
				continue;
			}
		}
		if (strchr((char*)table,c)) {
			if (token.size()==0)
				posd=pos;
			token+=c;
		}
		else {
			if (token.size()!=0) {
				if (!wdigitonly(token)) {
					//stack.push_back(token);						
					stacksize.push_back(token.size());
					if (colorkeywords.find(token)!=colorkeywords.end())
						stacktype.push_back(3);
					else
						stacktype.push_back(4);
					stackln.push_back(posd);
				}
				token.clear();
			}
			if (c>32) {
				//stack.push_back(ch);
				stacksize.push_back(1);
				stackln.push_back(posd);
				if (c=='.')
					stacktype.push_back(6);
				else
					if (c=='(')
						stacktype.push_back(7);
					else
						stacktype.push_back(0);
			}
			if (c==10 && lastc==13)
				lastc=0;
			else
				if (c==10 || c==13) {
					lastc=c;
					line++;
				}
		}
		s=get();
		pos=tell();
	}
	if (token.size()!=0) {
		if (!wdigitonly(token)) {
			//stack.push_back(token);
			stacksize.push_back(token.size());
			if (colorkeywords.find(token)!=colorkeywords.end())
				stacktype.push_back(3);
			else
				stacktype.push_back(4);
			stackln.push_back(posd);
		}
	}
}

int Findcomments(CString& ligne) {
	int i;
	int mx=ligne.GetLength();
	for (i=0;i<mx;i++) {
		if (ligne[i]=='"') {
			i++;
			while (i<mx && ligne[i]!='"') {
				if (ligne[i]=='\\')
					i++;
				i++;
			}
		}
		else
			if (ligne[i]=='\'') {
				i++;
				while (i<mx && ligne[i]!='\'') 
					i++;
			}
			else
				if (i<mx-1 && ligne[i]=='/' && ligne[i+1]=='/')
					return i;
	}
	return -1;
}

CKIFNView* firstview=NULL;

string courantnomfichier;
debogueur* deb=NULL;
bool runningasthread=false;

map<string,CKIFNView*> lesvues;

CKIFNView* Lavue(string& titre) {
	if (lesvues.find(titre)==lesvues.end())
		return NULL;
	return lesvues[titre];
}

CKIFNView::CKIFNView()
{
	// TODO: ajoutez ici du code de construction
	derniereposition=0;
	dernierelongueur=0;
	initcf=false;
	nbcurly=0;
	nbsquare=0;
	if (firstview==NULL)
		firstview=this;
}

CKIFNView::~CKIFNView()
{
	lesvues.erase(stitre);

}


LRESULT CKIFNView::OnFindDialogMessage(WPARAM wParam, LPARAM lParam)
{
    ASSERT(m_pFindDialog != NULL);

    // If the FR_DIALOGTERM flag is set,
    // invalidate the handle identifying the dialog box.
    if (m_pFindDialog->IsTerminating())
    {
        m_pFindDialog = NULL;
        return 0;
    }

    // If the FR_FINDNEXT flag is set,
    // call the application-defined search routine
    // to search for the requested string.
	if(m_pFindDialog->FindNext())
	{
		//read data from dialog
		FindName = m_pFindDialog->GetFindString();
		bMatchCase = m_pFindDialog->MatchCase() == TRUE;
		bMatchWholeWord = m_pFindDialog->MatchWholeWord() == TRUE;
		bSearchDown = m_pFindDialog->SearchDown() == TRUE;
		//with given name do search
		FindText(FindName, bMatchCase, bMatchWholeWord, bSearchDown);
	}
	else
		if(m_pFindDialog->ReplaceCurrent()) {
			if (FindName!="") {
				CString avec = m_pFindDialog->GetReplaceString();
				GetRichEditCtrl().HideSelection(TRUE, FALSE);
				GetRichEditCtrl().ReplaceSel(avec);
				GetRichEditCtrl().HideSelection(FALSE, FALSE);
			}
		}
		else
			if(m_pFindDialog->ReplaceAll()) {
				CString FindName = m_pFindDialog->GetFindString();
				bMatchCase = m_pFindDialog->MatchCase() == TRUE;
				bMatchWholeWord = m_pFindDialog->MatchWholeWord() == TRUE;
				bSearchDown = m_pFindDialog->SearchDown() == TRUE;
				CString avec = m_pFindDialog->GetReplaceString();
				GetRichEditCtrl().HideSelection(TRUE, FALSE);
				while (FindText(FindName, bMatchCase, bMatchWholeWord, bSearchDown))
					GetRichEditCtrl().ReplaceSel(avec);				
				GetRichEditCtrl().HideSelection(FALSE, FALSE);
			}
    return 0;
}


BOOL CKIFNView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: changez ici la classe ou les styles Window en modifiant
	//  CREATESTRUCT cs

	return CRichEditView::PreCreateWindow(cs);
}

// dessin de CKIFNView

void CKIFNView::OnDraw(CDC* /*pDC*/)
{

	CKIFNDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: ajoutez ici le code de dessin pour les données natives
}


// impression de CKIFNView


void CKIFNView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CKIFNView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// préparation par défaut
	return DoPreparePrinting(pInfo);
}

void CKIFNView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ajoutez une initialisation supplémentaire avant l'impression
}

void CKIFNView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ajoutez un nettoyage après l'impression
}

void CKIFNView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CKIFNView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

void CleanFromSession(HWND xwnd) {
	CWnd* w32=CWnd::FromHandle(xwnd);
	if (w32!=NULL)
		w32->DestroyWindow();
}

// diagnostics pour CKIFNView

#ifdef _DEBUG
void CKIFNView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CKIFNView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CKIFNDoc* CKIFNView::GetDocument() const // la version non Debug est inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKIFNDoc)));
	return (CKIFNDoc*)m_pDocument;
}
#endif //_DEBUG


// gestionnaires de messages pour CKIFNView
CKIFNView* vuecourante;

void CKIFNView::Choixligne() {
	if (firstview==this)
		return;
	vuecourante=Lavue(kifGlobals->currentfile);
	if (runningasthread) {
		if (vuecourante!=NULL)
			vuecourante->SelectionneLigne(kiflignecourantedebug());
		return;
	}
	if (vuecourante!=NULL) {
		vuecourante->SetFocus();
		CFrameWnd* pFrame = vuecourante->GetParentFrame();
		if (pFrame != NULL)
			pFrame->ActivateFrame();
	}
	else {

		CString n;
		wstring code;
		s_utf8tounicode(code,STR(kifGlobals->currentfile));
		CKIFNDoc* doc=(CKIFNDoc*)theApp.OpenDocumentFile((WCHAR*)code.c_str());
		vuecourante=Lavue(kifGlobals->currentfile);
	}
	if (vuecourante!=NULL)
		vuecourante->SelectionneLigne(kiflignecourantedebug());
}

void CKIFNView::Afficheerreur() {
	Choixligne();
	string lecode=theerrormsg();
	wstring base;
	s_utf8tounicode(base,STR(lecode));
	AfxMessageBox((WCHAR*)base.c_str());
}

void CKIFNView::Affichage(CString& s,bool thread) {
	if (firstview==NULL)
		return;
	CRichEditCtrl& e=firstview->GetRichEditCtrl();
	s+=ENDL;
	int nLength = e.GetWindowTextLength();
	// put the selection at the end of text
	e.SetSel(nLength, nLength);
	// replace the selection
	char* res=displaykifsession();
	long f;
	if (res!=NULL && res[0]!=0) {
		f=firstview->derniereposition-1;
		wstring base;
		s_utf8tounicode(base,res);
		commande=(WCHAR*)base.c_str();
		e.ReplaceSel(commande,1);	
		if (!thread) {
			firstview->SetFocus();
			CFrameWnd* pFrame = firstview->GetParentFrame();
			if (pFrame != NULL)
				pFrame->ActivateFrame();
		}
	}		
}

DWORD WINAPI executekif(LPVOID arg) {
	kifGlobals->Increment();
	echange* e=(echange*)arg;	
	CKIFNView* vue=(CKIFNView*)CWnd::FromHandle(e->vue);
	if (!runkifsession())
		vue->Afficheerreur();		
	else
		e->wvue->Affichage(e->s,true);
	kifGlobals->Decrement();
	e->encours=false;
	runningasthread=false;
	vue->EndWaitCursor();
	return 0;
}



void CKIFNView::runkif(CString& s) {	
	if (e.encours)
		return;
	if (s!="") {
		if (!openkifsession())
			return;
		string lecode;
		wchar_t* base=(wchar_t*)LPCWSTR(s);
		s_unicodetoutf8(lecode,base);
		kiffilename(STR(stitre));
		string args;
		wchar_t* baseargs=(wchar_t*)LPCWSTR(basearguments);
		s_unicodetoutf8(args,baseargs);
		kifsetarguments(args);

		int i=compilekifsession(STR(lecode));
		if (i==-1) {
			if (kifGlobals->currentfile=="%%STRING")
				kifGlobals->currentfile=stitre;
			Afficheerreur();
			e.encours=false;
			return;
		}

		if (e.compilation) {
			e.compilation=false;
			e.encours=false;
			return;
		}

		if (windowmode()) {
			if (e.pasapas==false) {
				map<long,BOOL>::iterator itb;
				for (itb=vbreaks.begin();itb!=vbreaks.end();itb++)
					ajoutebreak(STR(stitre),itb->first);
				e.pasapas=true;
				runningasthread=false;
			}
		}

		if (e.pasapas) {
			e.pasapas=false;
			CWaitCursor wait;
			if (!runkifsession())
				Afficheerreur();
			else
				Affichage(s,false);				
			e.encours=false;
		}
		else {
			BeginWaitCursor();
			DWORD tid;
			e.initialisation(this,s);
			BeginWaitCursor();
			CreateThread(NULL,0,&executekif,(LPVOID)&e,0,&tid);			
		}		
	}
}

void CKIFNView::AfficheAnalyse() {
	if (firstview==NULL)
		return;
	char* res=displaykifsession();
	if (res!=NULL && res[0]!=0) {
		CRichEditCtrl& e=firstview->GetRichEditCtrl();
		wstring base;
		s_utf8tounicode(base,res);
		commande=(WCHAR*)base.c_str();
		e.ReplaceSel(commande,1);	
	}		
}

void CKIFNView::executelecode() {	
	if (accumulecode!="") {
		openkifsession();
		//char* code=wcstombs( pmbhello, pwchello, MB_CUR_MAX );
		string lecode;
		wchar_t* base=(wchar_t*)LPCWSTR(accumulecode);
		s_unicodetoutf8(lecode,base);
		kiffilename(STR(stitre));
		string args;
		wchar_t* baseargs=(wchar_t*)LPCWSTR(basearguments);
		s_unicodetoutf8(args,baseargs);
		kifsetarguments(args);
		int i=compilekifsession(STR(lecode));

		if (i!=-1) {
			if (!runkifsession())
				Afficheerreur();
			else
				AfficheAnalyse();
		}
		else {
			if (kifGlobals->currentfile=="%%STRING")
				kifGlobals->currentfile=stitre;
			Afficheerreur();
		}
		accumulecode="";
		nbcurly=0;
		nbsquare=0;
	}
}
void CKIFNView::Couleur(int r,int g,int b) {
	if (!initcf) {
		cf = GetCharFormatSelection();
		cf.dwMask|=CFM_COLOR;
		cf.dwEffects&=~CFE_AUTOCOLOR;
		initcf=true;
		cf.cbSize = sizeof(CHARFORMAT2);
	}
	cf.dwEffects&=~CFE_ITALIC;
	cf.crTextColor=RGB(r,g,b);
	GetRichEditCtrl().SetSelectionCharFormat(cf);		
}

void CKIFNView::Italique(bool add) {
	CHARFORMAT2 cfbis= GetCharFormatSelection();
	cfbis.dwMask&=~CFM_COLOR;
	cfbis.dwEffects|=CFE_AUTOCOLOR;
	if (add)
		cfbis.dwEffects |= CFE_ITALIC; 
	else
		cfbis.dwEffects &= ~CFE_ITALIC; 
	SetCharFormat(cfbis);
}

void CKIFNView::ApplyColor(wstring& sw,int offset) {
	bnfxs.init(sw);
	bnfxs.loadtoken();
	CWaitCursor wait;
	CRichEditCtrl& e=GetRichEditCtrl();
	CHARRANGE positions;
	for(size_t istack=0;istack<bnfxs.stacktype.size();istack++) {	
		switch (bnfxs.stacktype[istack]) {
		case 1:
			positions.cpMin=bnfxs.stackln[istack]+offset-1;
			positions.cpMax=positions.cpMin+bnfxs.stacksize[istack];
			e.SetSel(positions);
			Couleur(255,0,0);
			break;
		case 2:			
			positions.cpMin=bnfxs.stackln[istack]+offset-1;
			positions.cpMax=positions.cpMin+bnfxs.stacksize[istack];
			e.SetSel(positions);
			Couleur(150,150,250);
			break;
		case 3:
			positions.cpMin=bnfxs.stackln[istack]+offset-1;
			positions.cpMax=positions.cpMin+bnfxs.stacksize[istack];
			e.SetSel(positions);
			Couleur(0,0,255);
			break;
		case 4:			
			if (istack<bnfxs.stacktype.size()-1) {
				if (istack>0 && bnfxs.stacktype[istack-1]==6 && bnfxs.stacktype[istack+1]==7) {
					positions.cpMin=bnfxs.stackln[istack]+offset-1;
					positions.cpMax=positions.cpMin+bnfxs.stacksize[istack];
					e.SetSel(positions);
					Couleur(160,100,10);
				}
				else
					if (bnfxs.stacktype[istack+1]==7) {
						positions.cpMin=bnfxs.stackln[istack]+offset-1;
						positions.cpMax=positions.cpMin+bnfxs.stacksize[istack];
						e.SetSel(positions);
						Couleur(140,0,160);
					}
			}
			break;
		case 5:
			positions.cpMin=bnfxs.stackln[istack]+offset-1;
			positions.cpMax=positions.cpMin+bnfxs.stacksize[istack];
			e.SetSel(positions);
			Couleur(85,180,85);
			break;
		}
	}
}

void CKIFNView::Colorie(long d,long f,int nFirstVisible) {
	if (firstview==this)
		return;
	BOOL modif=GetDocument()->IsModified();
	CRichEditCtrl& e=GetRichEditCtrl();
	bool stackundo=false;
	if (initcf==false)  {
		e.SetUndoLimit(0);
		stackundo=true;
	}
	if (nFirstVisible==-1) {
		nFirstVisible = e.GetFirstVisibleLine();
		e.GetSel(d,f);
	}
	long longueur=e.GetWindowTextLength();	
	LockWindowUpdate();
	e.HideSelection(TRUE, FALSE);
	SetRedraw(FALSE);
	e.SetSel(0,longueur);
	Couleur(0,0,0);
	dernierelongueur=longueur;
	CString s;
	GetWindowText(s);
	size_t i;

	wstring ws=(wchar_t*)LPCWSTR(s);
	wstring sw;
	for (i=0;i<ws.size();i++) {
		if (ws[i]!='\r')
			sw+=ws[i];
	}
	longueur=i;


	ApplyColor(sw,0);

	if (stackundo)
		e.SetUndoLimit(100);

	long xd,xf;
	map<long,BOOL>::iterator itb;
	for (itb=vbreaks.begin();itb!=vbreaks.end();itb++) {
		xd=e.LineIndex(itb->first-1);
		xf=xd+e.LineLength(xd);
		e.SetSel(xd,xf);
		Italique(true);
	}

	e.SetSel(0,0);
	if (nFirstVisible > 0)
		e.LineScroll(nFirstVisible, 0);
	e.SetSel(d,f);
	e.HideSelection(FALSE, FALSE);
	SetRedraw(TRUE);
	UnlockWindowUpdate();
	Invalidate();
	UpdateWindow();
	GetDocument()->SetModifiedFlag(modif);
}

void CKIFNView::OnKifExecuter()
{
	//// TODO: Add your command handler code here
	long d,f;
	CString s;	
	TCHAR sx[512]; 

	if (kifencours())
		return;

	if (firstview==this) {
		CRichEditCtrl& e=GetRichEditCtrl();
		s=e.GetSelText();
		s=s.Trim();
		if (s=="") {
			e.GetSel(d,f);
			int l=e.LineFromChar(d)-1;
			int ln=e.GetLine(l,sx,512);
			sx[ln]=0;
			s=sx;
			s=s.Trim();
		}
	}
	else
		GetWindowText(s);

	if (deb!=NULL) {
		delete deb;
		deb=NULL;
		courantnomfichier="";
	}

	runkif(s);
}



void CKIFNView::OnKifReexecuter()
{
	// TODO: Add your command handler code here
	if (!closekifsession())
		return;
	map<long,BOOL>::iterator itb;
	for (itb=vbreaks.begin();itb!=vbreaks.end();itb++)
		ajoutebreak(STR(stitre),itb->first);
	e.pasapas=true;
	runningasthread=false;
	OnKifExecuter();
}


void CKIFNView::OnKifPasaPas() {
	if (!closekifsession())
		return;
	map<long,BOOL>::iterator itb;
	for (itb=vbreaks.begin();itb!=vbreaks.end();itb++)
		ajoutebreak(STR(stitre),itb->first);
	modebug(true);
	e.pasapas=true;
	runningasthread=false;
	OnKifExecuter();
}

void CKIFNView::OnKifLister()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e=GetRichEditCtrl();

	char* res=listecodes();
	wstring base;
	s_utf8tounicode(base,res);
	commande=(WCHAR*)base.c_str();
	e.ReplaceSel(commande,1);	
	derniereposition=e.GetWindowTextLength();	
	e.SetSel(derniereposition,derniereposition);
	Colorie();
}


void CKIFNView::OnKifRaz()
{
	// TODO: Add your command handler code here
	closekifsession();
	accumulecode="";
	nbcurly=0;
	nbsquare=0;
}

void CKIFNView::SelectionneLigne(long l) {
	CRichEditCtrl& e=GetRichEditCtrl();
	long d=e.LineIndex(l-1);
	long f=d+e.LineLength(d);
	e.SetSel(d,f);
}

void CKIFNView::Chemin(CString titre) {
	wchar_t* base=(wchar_t*)LPCWSTR(titre);
	string ti;
	s_unicodetoutf8(ti,base);
	if (stitre=="") {
		stitre=ti;
		lesvues[stitre]=this;
	}
	else {
		if (stitre!=ti) {
			lesvues.erase(ti);
			stitre=ti;
			lesvues[stitre]=this;
		}
	}
}

void CKIFNView::comptebrackets(CString& s) {
	char instr=0;
	for (int i=0;i<s.GetLength();i++) {
		if (s[i]=='\'') {
			if (instr==0)
				instr=1;
			else
				if (instr==1)
					instr=0;
			continue;
		}

		if (s[i]=='"') {
			if (instr==0)
				instr=2;
			else
				if (instr==2) {
					if (s[i-1]!='\\')
						instr=0;
				}
			continue;
		}

		if (instr)
			continue;

		switch(s[i]) {
		case '{':
			nbcurly++;
			break;
		case '}':
			nbcurly--;
			break;
		case '[':
			nbsquare++;
			break;
		case ']':
			nbsquare--;
			break;
		}
	}
}

extern Exported const char* kifwhile;
extern Exported const char* kiffor;
extern Exported const char* kifswitch;
extern Exported const char* kifif;
extern Exported const char* kifelse;
extern Exported const char* kifelif;

bool Teststring(CString& s,const char* ch,char fin) {
	bool tr=true;
	int i=0;
	for (;i<strlen(ch);i++) {
		if (s[i]!=ch[i])
			return false;
	}
	if (s[i]<=32 || s[i]==fin || s[i]=='/')
		return true;
	return false;
}

BOOL CKIFNView::PreTranslateMessage(MSG* pMsg) {
	static CString _pour(kiffor);
	static CString _tantque(kifwhile);
	static CString _parmi(kifswitch);
	static CString _si(kifif);

	long d,f;
	//fermertab();
	CRichEditCtrl& e=GetRichEditCtrl();
	e.GetSel(d,f);
	int l=e.LineFromChar(f);
	char ch[20];
	sprintf(ch,"%d",l+1);
	commande=ch;
	leMain->AfficheLigne(commande);
	int longueur=e.GetWindowTextLength();	
	if (longueur!=dernierelongueur) {		
		if(dernierelongueur==0 || (pMsg->message==258 && pMsg->wParam == VK_RETURN)) {
			if (dernierelongueur==0)
				Colorie();
			if (pMsg->message==258 && pMsg->wParam == VK_RETURN) {
				TCHAR sx[512]; 
				int l=e.LineFromChar(d)-1;
				int ln=e.GetLine(l,sx,512);
				sx[ln]=0;			
				if (firstview==this) {
					commande=sx;
					commande=commande.TrimRight();
					bool exec=false;
					if (commande!="") {
						if (accumulecode!="")
							accumulecode+="\n";
						accumulecode+=commande;					
						wchar_t lst=commande[commande.GetLength()-1];
						switch(lst) {
						case ',':
						case ':':
							break;
						case ')':
							if (commande.Find(_pour,0)!=-1 || commande.Find(_tantque,0)!=-1 || commande.Find(_parmi,0)!=-1 || commande.Find(_si,0)!=-1)
								break;
						default:
							comptebrackets(commande);
							if (nbcurly==0 && nbsquare==0)
								exec=true;
						}
						if (exec)
							executelecode();
					}
				}
				else {
					//We get the trailing white chars from command
					e.HideSelection(TRUE, FALSE);
					SetRedraw(FALSE);
					wstring xs(sx);
					xs+='\n';
					CString selse(sx);
					selse.Trim();
					int lnw;
					bool belse=false;
					commande="";
					if (Teststring(selse,kifelse,'{')) {
						//we push up to previous "if"
						lnw=FirstEnd(d,f);
						belse=true;
					}
					else
						lnw=FirstChar(d,f,commande);
					if (lnw==-1)
						//We look for the first correspondance
						lnw=FindCorrespondance(d,f,0);

					if (lnw!=-1) {
						int firstlength=e.GetLine(lnw,sx,512)-1;
						if (firstlength>0) {
							sx[firstlength]=0;
							CString sfirst(sx);
							sfirst.TrimLeft();
							lnw=firstlength-sfirst.GetLength();
							while (lnw>0) {
								commande+=" ";
								lnw--;
							}
						}
					}
					if (belse) {
						d=e.LineIndex(l);
						e.SetSel(d,f);
						selse.Insert(0,commande);
						selse+="\n";
						selse+=commande;
						selse+="     ";
						commande=selse;
						xs=commande;
					}

					if (commande!="")
						e.ReplaceSel(commande,1);									
					e.GetSel(d,f);
					long dx=e.LineIndex(l);
					e.SetSel(dx,d);
					Couleur(0,0,0);
					ApplyColor(xs,dx);
					e.SetSel(d,f);
					e.HideSelection(FALSE, FALSE);
					SetRedraw(TRUE);
					Invalidate();
					UpdateWindow();
				}
			}
		}
	}
	return CRichEditView::PreTranslateMessage(pMsg); 
}

extern CKIFNApp theApp;

bool endofprocess;
DWORD WINAPI debuggage(LPVOID arg) {
	if(deb->DoModal()==IDOK) {
		ThreadLock _lock(kifTemporary);
		endofprocess=true;
		delete deb;
		deb=NULL;
		courantnomfichier="";
	}
	return 0;
}

void winaffichage(string vrs,string pile,string fname,long ln) {
	vuecourante=Lavue(fname);
	if (vuecourante!=NULL) {
		vuecourante->SetFocus();
		CFrameWnd* pFrame = vuecourante->GetParentFrame();
		if (pFrame != NULL)
			pFrame->ActivateFrame();
		vuecourante->SelectionneLigne(ln);
	}
	else {
		CString n;
		wstring code;
		s_utf8tounicode(code,STR(fname));
		CKIFNDoc* doc=(CKIFNDoc*)theApp.OpenDocumentFile((WCHAR*)code.c_str());		
		vuecourante=Lavue(fname);
		vuecourante->SelectionneLigne(ln);
	}
	
	bool actif=true;
	endofprocess=false;
	if (deb==NULL) {
		actif=false;
		deb=new debogueur;
	}

	if (courantnomfichier!=fname)
		vuecourante->GetWindowText(deb->vlecode);
	
	char* res=displaykifsession();
	if (res!=NULL && res[0]!=0) {
		deb->vsortie+=fromstring(res);
		deb->voutput=deb->vsortie;
		if (firstview!=NULL)
			firstview->SetWindowText(deb->vsortie);
		kifcleansession();
	}

	courantnomfichier=fname;
	deb->lignecourante=ln;
	deb->Lapile(pile);
	deb->Variables(vrs);
	string s;
	GetVariables(s);
	deb->choixvariables=STR(s);

	if (!actif) {
		DWORD tid;
		CreateThread(NULL,0,&debuggage,NULL,0,&tid);			
	}
	else
		::PostMessage(deb->GetSafeHwnd(),MET_A_JOUR,NULL,NULL);
	while (!endofprocess) {
		ThreadLock _lock(kifTemporary);
	}
}

void CKIFNView::OnKifPointarret()
{
	// TODO: Add your command handler code here

	if (firstview!=this) {
		long xd,xf;
		CRichEditCtrl& e=GetRichEditCtrl();
		long d,f;
		e.GetSel(d,f);
		long l=e.LineFromChar(f)+1;
		if (ajoutebreak(STR(stitre),l)==-2) {
			vbreaks.erase(l);			
			xd=e.LineIndex(l-1);
			xf=xd+e.LineLength(xd);
			e.SetSel(xd,xf);
			Italique(false);
		}
		else {
			vbreaks[l]=true;			
			xd=e.LineIndex(l-1);
			xf=xd+e.LineLength(xd);
			e.SetSel(xd,xf);
			Italique(true);
		}

	}
}

void CKIFNView::Pointarret(long l)
{
	long xd,xf;
	CRichEditCtrl& e=GetRichEditCtrl();
	if (ajoutebreak(STR(stitre),l)==-2) {
		vbreaks.erase(l);
		xd=e.LineIndex(l-1);
		xf=xd+e.LineLength(xd);
		e.SetSel(xd,xf);
		Italique(false);
	}
	else {
		vbreaks[l]=true;
		xd=e.LineIndex(l-1);
		xf=xd+e.LineLength(xd);
		e.SetSel(xd,xf);
		Italique(true);
	}
}

void CKIFNView::OnKifNettoyerp()
{
	// TODO: Add your command handler code here
	long xd,xf;
	CRichEditCtrl& e=GetRichEditCtrl();
	map<long,BOOL>::iterator itb;
	for (itb=vbreaks.begin();itb!=vbreaks.end();itb++) {
		xd=e.LineIndex(itb->first-1);
		xf=xd+e.LineLength(xd);
		e.SetSel(xd,xf);
		Italique(false);
	}
	vbreaks.clear();
	nettoyerpointarrets();	
}


void CKIFNView::OnKifIndenter()
{
	// TODO: Add your command handler code here
	if (firstview!=this) {
		CRichEditCtrl& e=GetRichEditCtrl();
		int nFirstVisible = e.GetFirstVisibleLine();
		long d,f;
		e.GetSel(d,f);
		CString s;
		string blanc;
		if (d==f)
			GetWindowText(s);
		else {
			s=e.GetSelText();
			for (int i=0;i<s.GetLength();i++) {
				if (s[i]==32 || s[i]==9)
					blanc+=s[i];
				else
					break;
			}
		}
		wchar_t* base=(wchar_t*)LPCWSTR(s);
		string ti;
		s_unicodetoutf8(ti,base);
		char* res=(char*)indentation(STR(ti),blanc);
		wstring idt;
		s_utf8tounicode(idt,res);
		s=(WCHAR*)idt.c_str();
		e.HideSelection(TRUE, FALSE);
		SetRedraw(FALSE);
		if (d==f) {
			SetWindowText(s);
			Colorie(d,f,nFirstVisible);
		}
		else {
			e.ReplaceSel(s,1);
			e.SetSel(d,d+s.GetLength());
			Couleur(0,0,0);
			ApplyColor(idt,d);
			e.SetSel(f,f);
			e.HideSelection(FALSE, FALSE);
			SetRedraw(TRUE);
			Invalidate();
			UpdateWindow();
		}
		GetDocument()->SetModifiedFlag(TRUE);
	}
}


void CKIFNView::OnEditionCherche()
{
	// TODO: Add your command handler code here
	m_pFindDialog = new CFindReplaceDialog();
	CString s(FindName);
	m_pFindDialog->Create(TRUE, s, NULL, FR_DOWN, this);
}



void CKIFNView::OnEditionRemplace()
{
	// TODO: Add your command handler code here
	m_pFindDialog = new CFindReplaceDialog();
	CString s(FindName);
	m_pFindDialog->Create(FALSE, s, NULL, FR_DOWN, this);
}



void CKIFNView::OnKifCommenter()
{
	if (firstview==this)
		return;

	// TODO: Add your command handler code here
	CRichEditCtrl& e=GetRichEditCtrl();
	long d,f;
	e.GetSel(d,f);	
	commande=e.GetSelText();
	CString res;
	bool blanc=true;
	char uncommente=0;
	for (int i=0;i<commande.GetLength();i++) {
		if (commande[i]=='\n' || commande[i]=='\r') {
			blanc=true;
			res+=commande[i];
		}		
		else {
			if (blanc) {
				if (commande[i]<=32) {
					res+=commande[i];
					continue;
				}
				if (commande[i]=='/' && commande[i+1]=='/') {
					if (uncommente!=1) {
						blanc=false;
						uncommente=2;
						i++;
						continue;
					}
					else
						res+="//";
				}
				else 
					if (uncommente!=2) {
						uncommente=1;
						res+="//";
					}
			}
			blanc=false;			
			res+=commande[i];
		}
	}
	wchar_t* base=(wchar_t*)LPCWSTR(res);
	string ti;
	s_unicodetoutf8(ti,base);
	char* ires=(char*)indentation(STR(ti),"");
	wstring idt;
	s_utf8tounicode(idt,ires);
	res=(WCHAR*)idt.c_str();
	e.ReplaceSel(res,1);
	e.SetSel(f,f);
	Colorie(-1,-1,-1);
}


void CKIFNView::OnKifBreak()
{
	// TODO: Add your command handler code here
	arretprg();
	CString message("Fin du programme");
	AfxMessageBox(message);
}


void CKIFNView::OnKifAsThread()
{
	// TODO: Add your command handler code here
	if (!closekifsession())
		return;
	if (vbreaks.size()!=0) {
		map<long,BOOL>::iterator itb;
		for (itb=vbreaks.begin();itb!=vbreaks.end();itb++)
			ajoutebreak(STR(stitre),itb->first);
		e.pasapas=true;
		runningasthread=false;
	}
	else {
		e.pasapas=false;
		runningasthread=true;
	}
	OnKifExecuter();
}

char Accolade(CString& ligne,bool virgpar) {
	int i;
	char trouve=0;
	int mx=ligne.GetLength();
	unsigned char c;
	bool acc=false;
	for (i=0;i<mx;i++) {
		c=ligne[i];
		if (c<=32)
			continue;
		switch(c) {
		case '{':
			acc=true;
			trouve='{';
			break;
		case '}':		
			if (acc)
				return 0;
			return c;
		case ',':
			if (virgpar) {
				if (acc)
					trouve='(';
				else
					trouve=',';
			}
			break;
		case ')':
			if (virgpar)
				trouve=')';
			break;
		case '"':
			if (trouve!='{')
				trouve=0;
			i++;
			while (i<mx && ligne[i]!='"') {
				if (ligne[i]=='\\')
					i++;
				i++;
			}
			break;
		case '\'':
			if (trouve!='{')
				trouve=0;
			i++;
			while (i<mx && ligne[i]!='\'') 
				i++;
			break;
		case ';':			
			if (trouve!='{')
				trouve=0;
			break;
		case '/':
			if (i<mx-1 && ligne[i+1]=='/')
				return trouve;
		default:
			if (trouve!='{')
				trouve=0;
		}
	}
	if (acc && !trouve)
		return '{';
	return trouve;
}

long CKIFNView::FindCorrespondance(long d,long f,int tofind) {
	CRichEditCtrl& e=GetRichEditCtrl();
	TCHAR sx[512]; 
	int l,ln;
	l=e.LineFromChar(f);
	int inc=0;
	int fin=-1;
	char finchar;
	
	CString s;
	int compte=0;
	if (tofind==2) {		
		compte=1;
		ln=e.GetLine(l,sx,512);
		sx[ln]=0;
		s=sx;
		finchar=Accolade(s,false);
		if (finchar=='{') {
			inc=1;
			long length=e.GetTextLengthEx(GTL_NUMBYTES,1200);
			fin=e.LineFromChar(length-1);		
		}
		else
			if (finchar=='}')
				inc=-1;			
			else
				return -1;			
	}
	else {
		inc=-1;
		finchar='}';
		compte=tofind;
	}
	l+=inc;
	
	char c;
	while (l!=fin) {
		ln=e.GetLine(l,sx,512);
		sx[ln]=0;
		s=sx;		
		s=s.Trim();		
		if (s.GetLength()) {
			c=Accolade(s,false);
			if (c) {
				if (finchar==c) 
					compte++;
				else
					compte--;
			}
		}
		if (!compte)
			break;
		l+=inc;
	}
	
	if (!compte)
		return l;
	return -1;
}


long CKIFNView::FirstChar(long d,long f,CString& commande) {
	CRichEditCtrl& e=GetRichEditCtrl();
	TCHAR sx[512]; 
	int l,ln;
	l=e.LineFromChar(f);
	int fin=-1;
	CString s;
	
	l--;
	char c=0;
	bool first=true;
	int found=0;
	while (l!=-1) {
		ln=e.GetLine(l,sx,512);
		sx[ln]=0;
		s=sx;
		s=s.Trim();		
		if (s.GetLength()) {
			c=Accolade(s,true);
			switch (c) {
			case '(':
				if (first) {
					commande="      ";
					return l;
				}
				break;
			case ',':
				if (first)
					return l;
				break;
			case ')':
				if (first) {
					commande="     ";
					return l;
				}
				break;
			case '{':
				if (!found) {
					commande="     ";
					return l;
				}
				found--;
				if (!found)
					first=true;
				break;
			case '}':						
				if (!first && !found)
					return -1;
				found++;
			}
			first=false;			
			if (sx[0]>32)
				return l;
		}
		l--;
	}
	if (c=='(' || c==')' || c==',')
		return 0;
	return -1;
}

long CKIFNView::FirstEnd(long d,long f) {
	CRichEditCtrl& e=GetRichEditCtrl();
	TCHAR sx[512]; 
	int l,ln;
	l=e.LineFromChar(f);
	int fin=-1;
	CString s;
	
	l-=2;
	int saute=0;
	int compte=0;
	while (l!=-1) {
		ln=e.GetLine(l,sx,512);
		sx[ln]=0;
		s=sx;
		s=s.Trim();		
		if (s.GetLength()) {
			char c=Accolade(s,false);
			if (c=='}')
				compte++;
			else
				if (c=='{')
					compte--;
			if (!compte) {
				if (Teststring(s,kifelse,'{')) {
					l--;
					saute++;
					continue;
				}
				if (Teststring(s,kifif,'(') || Teststring(s,kifelif,'(')) {
					if (!saute)
						return l;
					saute--;
				}
			}
		}
		l--;
	}
	return -1;
}

afx_msg void CKIFNView::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags) {
	if (nChar==125 && firstview!=this) {
		CRichEditCtrl& e=GetRichEditCtrl();
		long d,f;
		e.GetSel(d,f);
		int l=e.LineFromChar(f);
		TCHAR sx[512]; 
		e.LineFromChar(d);
		int ln=e.GetLine(l,sx,512);
		sx[ln]=0;
		CString sn(sx);
		sn.Trim();
		if (!sn.GetLength() && ln>4) {
			long firstline=FindCorrespondance(d,f,1);
			if (firstline!=-1) {
				int firstlength=e.GetLine(firstline,sx,512);
				if (firstlength>0) {
					sx[firstlength]=0;
					CString sfirst(sx);
					sfirst.TrimLeft();
					ln=firstlength-sfirst.GetLength();
				}
				else
					ln=0;
				while (ln>0) {
					sn+=" ";
					ln--;
				}
				sn+="}";
				d=e.LineIndex(l);
				f=d+e.LineLength(d);
				e.SetSel(d,f);
				e.ReplaceSel(sn,1);
				return;
			}
		}
	}
	CWnd::OnChar(nChar,nRepCnt,nFlags);
}



void CKIFNView::OnKifCorrespondance()
{
	// TODO: Add your command handler code here
	long d,f;
	//fermertab();
	CRichEditCtrl& e=GetRichEditCtrl();
	e.GetSel(d,f);

	long l=FindCorrespondance(d,f,2);
	if (l!=-1)
		SelectionneLigne(++l);
}


void CKIFNView::OnKifCompiler()
{
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
	if (!closekifsession())
		return;
	e.pasapas=false;
	runningasthread=false;
	e.compilation=true;
	OnKifExecuter();
	e.compilation=false;

}



void CKIFNView::OnEditionSuivant()
{
	// TODO: Add your command handler code here
	if (FindName=="")
		return;
	FindText(FindName, bMatchCase, bMatchWholeWord, bSearchDown);
}


void CKIFNView::OnEditionArguments()
{
	// TODO: Add your command handler code here
	Arguments argums;
	argums.arguments=basearguments;
	if (argums.DoModal()==IDOK) 
		basearguments=argums.arguments;
}


void CKIFNView::OnEditionAllerligne()
{
	// TODO: Add your command handler code here
	Arguments argums;
	long l;
	if (argums.DoModal()==IDOK) {
		l=wcstol(LPCWSTR(argums.arguments),NULL,10);
		SelectionneLigne(l);
	}
}

void CKIFNView::OnEditUndo()
{
	ASSERT_VALID(this);
	CRichEditCtrl& e=GetRichEditCtrl();
	int longueur=e.GetWindowTextLength();
	while (e.CanUndo() && e.GetUndoName()==UID_UNKNOWN) {
		if (e.Undo()==FALSE)
			break;
		if (longueur!=e.GetWindowTextLength()) {
			long d,f;
			e.GetSel(d,f);
			e.SetSel(f,f);
			return;
		}
	}
	if (e.CanUndo())
		e.Undo();
	m_bSyncCharFormat = m_bSyncParaFormat = TRUE;
}

void CKIFNView::OnEditRedo()
{
	ASSERT_VALID(this);
	CRichEditCtrl& e=GetRichEditCtrl();
	int longueur=e.GetWindowTextLength();
	long d,f;
	while (e.CanUndo() && e.GetRedoName()==UID_UNKNOWN) {
		if (e.Redo()==FALSE)
			break;
		if (longueur!=e.GetWindowTextLength()) {
			e.GetSel(d,f);
			e.SetSel(f,f);
			return;
		}
	}
	if (e.CanUndo())
		e.Redo();
	m_bSyncCharFormat = m_bSyncParaFormat = TRUE;
}
