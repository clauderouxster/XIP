// KIFFView.cpp : implémentation de la classe CKIFFView
//

/*
LockWindowUpdate();
e.HideSelection(TRUE, FALSE);
SetRedraw(FALSE);

...

e.HideSelection(FALSE, FALSE);
SetRedraw(TRUE);
UnlockWindowUpdate();
Invalidate();
UpdateWindow();

*/

#include "stdafx.h"
// SHARED_HANDLERS peuvent être définis dans les gestionnaires d'aperçu, de miniature
// et de recherche d'implémentation de projet ATL et permettent la partage de code de document avec ce projet.
#ifndef SHARED_HANDLERS
#include "KIFF.h"
#endif


#include "KIFFDoc.h"
#include "KIFFView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "debogueur.h"
#include "x_node.h"
#include "xipstlres.h"
#include "XIPDEBUG.h"
#include "conversion.h"

#include <regex>
#include "kifcontainer.h"
#include "kifmaps.h"
#include "kifautomaton.h"


#include "kifmethods.h"
const int nbcolorlines = 5000;

Exported KifCode* Newkifcodeparseur(int t);

extern "C" {
#include "kifextern.h"
	bool KifSetDisplayFunction(KifDisplayFunction f, void* o);
}

void LaunchWin32();

#include <signal.h>
#include "kifarguments.h"

#define ENDL "\r\n"
#define WCSTR(x) (wchar_t*)LPCWSTR(x)

extern const char* KIFTHEINFOSTRINGS[];
static map<string, string> theinfos;
static vector<CKIFFView*> thebreaks;

static void MapInitInfos() {
	static bool first = true;

	if (first == false)
		return;
	first = false;
	int i = 0;
	string s;
	while (KIFTHEINFOSTRINGS[i][0] != 0) {
		theinfos[KIFTHEINFOSTRINGS[i]] = KIFTHEINFOSTRINGS[i + 1];
		i += 2;
	}
}

void fromCString(string& f, CString l) {
	s_unicode_to_utf8(f, WCSTR(l));
}

void fromstring(CString& commande, string res) {
	wstring base;
	s_utf8_to_unicode(base, STR(res));
	commande = (WCHAR*)base.c_str();
}

void fromaddstring(CString& commande, string res) {
	wstring base;
	s_utf8_to_unicode(base, STR(res));
	commande += (WCHAR*)base.c_str();
}

extern CMainFrame* leMain;


static echange lexchange;
static char	activecursor = 0;

class KifViewLock {
public:
	KifThreadLock* _locker;

	KifViewLock(KifThreadLock* l) : _locker(l) {
		_locker->Locking();
	}

	~KifViewLock() {
		_locker->Unlocking();
	}
};

hmap<wstring, bool> colorkeywords;
extern const char* KIFLTKSDEBUGGERTRINGS[];

x_colorreading::x_colorreading() {
	tokenstorage = false;
	name = "%%STRING";
	pos = 0;
	length = 0;
	int ic = 0;
	if (colorkeywords.size() == 0) {
		while (KIFLTKSDEBUGGERTRINGS[ic][0] != 0) {
			wstring s;
			s_utf8_to_unicode(s, (char*)KIFLTKSDEBUGGERTRINGS[ic]);
			colorkeywords[s] = true;
			ic++;
		}
	}
}

void echange::initialisation(CKIFFView*v, CString& x) {
	compilation = false;
	encours = true;
	vue = v->GetSafeHwnd();
	wvue = v;
	s = x;
}

//------------------------------------------------------------------------------------
wstring GetKeyVal(wstring key);
void SetKeyVal(wstring key, CString value);
void SetBlankSize(int sz);
int GetBlankSize();
//------------------------------------------------------------------------------------
Exported bool kifexplorexn(x_node* xn, string& var, string& type, int first);

static hmap<short, vector<wstring> > thevectorstrings;
static hmap<short, map<wstring, wstring> > thevectorinfos;
static hmap<wstring, short> kiftypes;
static hmap<short, short> equivalences;

static bool comparestr(wstring& e1, wstring& e2) {
	if (e1 < e2)
		return(true);
	return(false);
}

class uniquekifcode {
public:
	bool close;

	uniquekifcode() {
		close = false;
		if (kifGlobals == NULL) {
			openkifsession();
			close = true;
		}
	}

	~uniquekifcode() {
		if (close)
			closekifsession();
	}
};

void Updatesuggestion() {
	uniquekifcode();

	wstring ws;	
	hmap<short, hmap<short, bool> >::iterator it;
	for (it = kifGlobals->kifAllFunctions.begin(); it != kifGlobals->kifAllFunctions.end(); it++) {
		hmap<short, bool>& val = it->second;
		vector<wstring> methods;
		hmap<short, bool>::iterator itsub;
		for (itsub = val.begin(); itsub != val.end(); itsub++) {
			sc_utf8_to_unicode(ws, STR(kifGlobals->KifIdString(itsub->first)));
			if (ws.find(L"FL_") != wstring::npos)
				continue;

			methods.push_back(ws);
		}

		sort(methods.begin(), methods.end(), comparestr);
		thevectorstrings[it->first] = methods;
		map<string, string>::iterator itm;
		wstring ks;
		for (itm = kifGlobals->kifAllInformation[it->first].begin(); itm != kifGlobals->kifAllInformation[it->first].end(); itm++) {
			sc_utf8_to_unicode(ks, STR(itm->first));
			sc_utf8_to_unicode(ws, STR(itm->second));
			thevectorinfos[it->first][ks] = ws;
		}
	}

	equivalences = kifGlobals->equivalences;
	map<string, KifType>::iterator itk;
	for (itk = kifGlobals->kifTypeString.begin(); itk != kifGlobals->kifTypeString.end(); itk++) {
		if (equivalences.find(itk->second) != equivalences.end()) {
			sc_utf8_to_unicode(ws, STR(itk->first));
			kiftypes[ws] = itk->second;
		}
	}
}

//------------------------------------------------------------------------------------

static bool w_alpha(wchar_t c) {
	if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122) || (c >= 48 && c <= 57) || c == '_' || c == '#' || c > 128)
		return true;
	return false;
}

void x_colorreading::loadtoken(bool keepstacktoken) {
	wchar_t s, c;
	Initable();
	table[190] = 0;
	long line = 0;
	wstring ch;
	wstring token;
	stacksize.clear();
	stackln.clear();
	stacktype.clear();
	wstack.clear();

	long pos, posd = 0;
	wchar_t lastc = 0;
	wstring comments;
	s = wget();
	pos = tell();
	while (!eof()) {
		if (s == '/' && token.size() == 0) {
			s = wget();
			if (s == '/') {
				comments.clear();
				comments += '/';
				comments += '/';
				//this is a comment
				s = wget();
				while (s != 10 && s != 13 && !eof()) {
					comments += s;
					s = wget();
				}
				token.clear();
				if (tokenstorage == false) {
					if (keepstacktoken)
						wstack.push_back(L"");
					stacksize.push_back(comments.size());
					stackln.push_back(pos);
					stacktype.push_back(5);
				}
				line++;
				lastc = s;
				s = wget();
				pos = tell();
				continue;
			}
			if (s == '@') {
				comments.clear();
				comments += '/';
				comments += '@';
				lastc = 0;
				//this is a comment
				while (1) {
					if (eof())
						return;
					s = wget();
					if (s == '@') {
						comments += s;
						s = wget();
						comments += s;
						if (s == '/')
							break;
					}
					else {
						if (s == 10 && lastc == 13) {
							lastc = 0;
							comments += s;
						}
						else {
							if (s == 10 || s == 13) {
								comments += s;
								lastc = s;
								line++;
							}
							else
								comments += s;
						}
					}
				}
				token.clear();
				if (tokenstorage == false) {
					if (keepstacktoken)
						wstack.push_back(L"");
					stacksize.push_back(comments.size());
					stackln.push_back(pos);
					stacktype.push_back(5);
				}
				s = wget();
				pos = tell();
				continue;
			}
			seek(pos);
			s = '/';
		}

		c = s;
		if (token.size() == 0) {
			if (c == '@') {
				s = wget();
				if (s == '"') {
					token = c;
					token += s;
					c = wget();
					lastc = 0;
					while (!eof()) {
						if (c == 10 && lastc == 13)
							lastc = 0;
						else {
							if (c == 10 || c == 13) {
								lastc = c;
								line++;
							}
						}
						token += c;
						bool fd = false;
						if (c == '"')
							fd = true;
						c = wget();
						if (fd && c == '@')
							break;
					}
					token += c;
					if (tokenstorage == false) {
						if (keepstacktoken)
							wstack.push_back(token);
						stacksize.push_back(token.size());
						stackln.push_back(pos);
						stacktype.push_back(10);
					}
					token.clear();
					s = wget();
					pos = tell();
					continue;
				}
				seek(pos);
			}
			else {
				if (c == '"') {
					lastc = 0;
					token = c;
					c = wget();
					while (c != '"' && !eof()) {
						if (c == 10 && lastc == 13)
							lastc = 0;
						else
						if (c == 10 || c == 13) {
							lastc = c;
							line++;
						}
						if (c == '\\') {
							token += c;
							c = wget();
						}
						token += c;
						c = wget();
					}
					token += '"';
					if (tokenstorage == false) {
						if (keepstacktoken)
							wstack.push_back(token);
						stacksize.push_back(token.size());
						stackln.push_back(pos);
						stacktype.push_back(1);
					}
					token.clear();
					s = wget();
					pos = tell();
					continue;
				}
				if (c == '\'') {
					token = c;
					c = wget();
					while (c != '\'' && !eof()) {
						if (c == 10 && lastc == 13)
							lastc = 0;
						else
						if (c == 10 || c == 13) {
							lastc = c;
							line++;
						}
						token += c;
						c = wget();
					}
					token += '\'';
					if (tokenstorage == false) {
						if (keepstacktoken)
							wstack.push_back(token);
						stacksize.push_back(token.size());
						stackln.push_back(pos);
						stacktype.push_back(2);
					}
					token.clear();
					s = wget();
					pos = tell();
					continue;
				}
			}
		}
		if (w_alpha(c)) {
			if (token.size() == 0)
				posd = pos;
			token += c;
		}
		else {
			if (token.size() != 0) {
				if (c == '-')
					token += c;
				else {
					if (token[0] == '?' && (c == '+' || c == '*'))
						token += c;
					else {
						if (!wdigitonly(token)) {
							if (tokenstorage == false) {
								if (keepstacktoken)
									wstack.push_back(token);
								stacksize.push_back(token.size());
								if (token[0] == '?')
									stacktype.push_back(9);
								else
								if (token[0] == '#' || token[0] == '$')
									stacktype.push_back(8);
								else
								if (colorkeywords.find(token) != colorkeywords.end())
									stacktype.push_back(3);
								else
									stacktype.push_back(4);
								stackln.push_back(posd);
							}
							else {
								if (token[0] != '?' && token[0] != '#' && token[0] != '$' && colorkeywords.find(token) == colorkeywords.end()) {
									if (!keepstacktoken)
										wstack.push_back(token);
								}
							}
						}
						token.clear();
					}
				}
			}
			else {
				if (c == '?' || c == '#' || c == '$') {
					posd = pos;
					token = c;
				}
			}

			if (tokenstorage == false) {
				if (c > 32) {
					if (keepstacktoken) {
						ch = c;
						wstack.push_back(ch);
					}

					stacksize.push_back(1);
					stackln.push_back(posd);
					switch (c) {
					case '.':
						stacktype.push_back(6);
						break;
					case '#':
						stacktype.push_back(0);
						posd = pos;
						token = c;
						break;
					case '$':
						stacktype.push_back(7);
						posd = pos;
						token = c;
						break;
					case '[':
					case '(':
						stacktype.push_back(7);
						break;
					default:
						stacktype.push_back(0);
					}
				}
			}
			if (c == 10 && lastc == 13)
				lastc = 0;
			else
			if (c == 10 || c == 13) {
				lastc = c;
				line++;
			}
		}
		s = wget();
		pos = tell();
	}
	if (token.size() != 0) {
		if (!wdigitonly(token)) {
			if (keepstacktoken)
				wstack.push_back(token);
			stacksize.push_back(token.size());
			if (token[0] == '?')
				stacktype.push_back(9);
			else
			if (token[0] == '#' || token[0] == '$')
				stacktype.push_back(8);
			else
			if (colorkeywords.find(token) != colorkeywords.end())
				stacktype.push_back(3);
			else
				stacktype.push_back(4);
			stackln.push_back(posd);
		}
	}
}

int Findcomments(CString& ligne) {
	int i;
	int mx = ligne.GetLength();
	for (i = 0; i < mx; i++) {
		if (ligne[i] == '"') {
			i++;
			while (i < mx && ligne[i] != '"') {
				if (ligne[i] == '\\')
					i++;
				i++;
			}
		}
		else
		if (ligne[i] == '\'') {
			i++;
			while (i < mx && ligne[i] != '\'')
				i++;
		}
		else
		if (i < mx - 1 && ligne[i] == '/' && ligne[i + 1] == '/')
			return i;
	}
	return -1;
}


// CKIFFView
static UINT FindDialogMessage = ::RegisterWindowMessage(FINDMSGSTRING);
#define MET_SUGGERE_METHODE MET_A_JOUR_COULEUR+1

IMPLEMENT_DYNCREATE(CKIFFView, CRichEditView)
//http://www.codeproject.com/Questions/327573/Get-document-path-from-tab

BEGIN_MESSAGE_MAP(CKIFFView, CRichEditView)
	// Commandes d'impression standard
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_FILE_PRINT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CKIFFView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CHAR()
	ON_COMMAND(ID_KIF_EXECUTER, &CKIFFView::OnKifExecuter)
	ON_COMMAND(ID_KIF_REEXECUTER, &CKIFFView::OnKifReexecuter)
	ON_COMMAND(ID_KIF_LISTER, &CKIFFView::OnKifLister)
	ON_COMMAND(ID_KIF_RAZ, &CKIFFView::OnKifRaz)
	ON_COMMAND(ID_KIF_PASAPAS, &CKIFFView::OnKifPasaPas)
	ON_COMMAND(ID_KIF_POINTARRET, &CKIFFView::OnKifPointarret)
	ON_COMMAND(ID_KIF_NETTOYERP, &CKIFFView::OnKifNettoyerp)
	ON_COMMAND(ID_KIF_INDENTER, &CKIFFView::OnKifIndenter)
	ON_REGISTERED_MESSAGE(FindDialogMessage, OnFindDialogMessage)
	ON_COMMAND(ID_EDITION_CHERCHE, &CKIFFView::OnEditionCherche)
	ON_COMMAND(ID_EDITION_REMPLACE, &CKIFFView::OnEditionRemplace)
	ON_COMMAND(ID_KIF_BREAK, &CKIFFView::OnKifBreak)
	ON_COMMAND(ID_KIF_COMMENTER, &CKIFFView::OnCommenter)
	ON_COMMAND(ID_KIF_REEXECUTE, &CKIFFView::OnKifAsThread)
	ON_COMMAND(ID_KIF_CORRESPONDANCE, &CKIFFView::OnKifCorrespondance)
	ON_COMMAND(ID_KIF_COMPILER, &CKIFFView::OnKifCompiler)
	ON_COMMAND(ID_EDITION_SUIVANT, &CKIFFView::OnEditionSuivant)
	ON_COMMAND(ID_KIF_ARGUMENTS, &CKIFFView::OnKifArguments)
	ON_COMMAND(ID_EDITION_ALLERLIGNE, &CKIFFView::OnEditionAllerligne)
	ON_UPDATE_COMMAND_UI(ID_KIF_DEBUGXIP, &CKIFFView::OnUpdateKifDebugxip)
	ON_COMMAND(ID_KIF_DEBUGXIP, &CKIFFView::OnKifDebugxip)
	ON_MESSAGE(MET_A_JOUR, &CKIFFView::MAJ)
	ON_MESSAGE(MET_A_JOUR_COULEUR, &CKIFFView::MAJCOULEUR)
	ON_MESSAGE(MET_SUGGERE_METHODE, &CKIFFView::SUGGERE)
	ON_MESSAGE(ID_OUVRIR_DOCUMENT, &CKIFFView::Ouverture)
	ON_MESSAGE(ID_AFFICHER_ERREUR, &CKIFFView::afficher)
	ON_MESSAGE(IDE_AFFICHAGE_COURANT, &CKIFFView::AfficheCourant)
	ON_UPDATE_COMMAND_UI(ID_KIF_REEXECUTE, &CKIFFView::OnUpdateKifReexecute)
	ON_UPDATE_COMMAND_UI(ID_KIF_PASAPAS, &CKIFFView::OnUpdateKifPasapas)
	ON_UPDATE_COMMAND_UI(ID_KIF_EXECUTER, &CKIFFView::OnUpdateKifExecuter)
	ON_UPDATE_COMMAND_UI(ID_KIF_COMPILER, &CKIFFView::OnUpdateKifCompiler)
	ON_UPDATE_COMMAND_UI(ID_KIF_BREAK, &CKIFFView::OnUpdateKifBreak)
	ON_COMMAND(ID_EDITION_ABANDON, &CKIFFView::OnEditionAbandon)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CKIFFView::OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CKIFFView::OnUpdateEditUndo)
	ON_COMMAND(ID_KXIP_NOBREAK, &CKIFFView::OnWxipNobreak)
	ON_UPDATE_COMMAND_UI(ID_KXIP_NOBREAK, &CKIFFView::OnUpdateWxipNobreak)
	ON_COMMAND(ID_EDIT_CUT, &CKIFFView::OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, &CKIFFView::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CKIFFView::OnUpdateEditCut)
	ON_COMMAND(ID_EDITION_SUGGEST, &CKIFFView::OnEditionSuggest)
	ON_UPDATE_COMMAND_UI(ID_EDITION_SUGGEST, &CKIFFView::OnUpdateEditionSuggest)
	ON_COMMAND(ID_EDIT_SETBLANKS, &CKIFFView::OnEditSetblanks)
END_MESSAGE_MAP()

// construction ou destruction de CKIFFView
CString basearguments;
CKIFFView* firstview = NULL;


//static const char* coloration="\\bgarbagefunction\\b|\\bgarbagesize\\b|\\bremoveerroronkey\\b|\\bsetdebugfunction\\b|\\bparentthreadid\\b|\\bsparsevector\\b|\\bsetdebugdata\\b|\\bcreateserver\\b|\\bgetstackinfo\\b|\\bwaitonfalse\\b|\\benvironment\\b|\\brunadthread\\b|\\bsynchronous\\b|\\bfilebrowser\\b|\\bpolynomial\\b|\\bprintlnerr\\b|\\bwaitonjoin\\b|\\bmyvariable\\b|\\bmarginleft\\b|\\bexclusive\\b|\\bwtreeitem\\b|\\bprotected\\b|\\battribute\\b|\\briterator\\b|\\bfraction\\b|\\bprinterr\\b|\\bfunction\\b|\\bsequence\\b|\\bthreadid\\b|\\bpathname\\b|\\bcontinue\\b|\\biterator\\b|\\binstance\\b|\\bprintln\\b|\\baddress\\b|\\bboolean\\b|\\bautorun\\b|\\bprivate\\b|\\bbrowser\\b|\\bwchoice\\b|\\bwoutput\\b|\\bwaiting\\b|\\bversion\\b|\\bsparse\\b|\\bwheely\\b|\\bwheelx\\b|\\bremote\\b|\\bminute\\b|\\bgcsize\\b|\\bsocket\\b|\\bbutton\\b|\\bscroll\\b|\\bswitch\\b|\\bxmldoc\\b|\\beditor\\b|\\bloadin\\b|\\bwinput\\b|\\bslider\\b|\\bwgroup\\b|\\bwimage\\b|\\bsqlite\\b|\\bpflush\\b|\\bstring\\b|\\bcommon\\b|\\brandom\\b|\\bbottom\\b|\\bvector\\b|\\breturn\\b|\\bthread\\b|\\bwindow\\b|\\bsecond\\b|\\bstrict\\b|\\bprint\\b|\\bcatch\\b|\\btable\\b|\\bfalse\\b|\\beigen\\b|\\bwtabs\\b|\\bframe\\b|\\bwhile\\b|\\bstart\\b|\\bstdin\\b|\\bnot\\b|\\bnotin\\b|\\bpause\\b|\\bconst\\b|\\bsleep\\b|\\breset\\b|\\brange\\b|\\bbreak\\b|\\braise\\b|\\bfloat\\b|\\bwtree\\b|\\bclass\\b|\\bbool\\b|\\bself\\b|\\beval\\b|\\btrue\\b|\\bcast\\b|\\bthis\\b|\\bgetc\\b|\\blong\\b|\\btime\\b|\\bweka\\b|\\bcall\\b|\\blist\\b|\\bport\\b|\\bexit\\b|\\barff\\b|\\bfile\\b|\\benum\\b|\\bhash\\b|\\bbits\\b|\\bnull\\b|\\btoto\\b|\\bkill\\b|\\bint\\b|\\bisa\\b|\\buse\\b|\\blib\\b|\\btop\\b|\\bfor\\b|\\bbox\\b|\\btry\\b|\\bbit\\b|\\bfmap\\b|\\bimap\\b|\\bsmap\\b|\\bmap\\b|\\bsys\\b|\\bage\\b|\\bxml\\b|\\bif\\b|\\bin\\b|\\bpas\\b|\\bdefault\\b|\\bcurl\\b|\\belse\\b|\\belif\\b|\\bwith\\b|\\.[a-zA-Z\\x80-\\xFF][0-9a-zA-Z\\x80-\\xFF]*\\(|\"|/|\\n|'";

debogueur* deb = NULL;

map<string, CKIFFView*> lesvues;

CKIFFView* Lavue(string& titre) {
	if (lesvues.find(titre) == lesvues.end())
		return NULL;
	return lesvues[titre];
}

void OuvrirDocument(string nom, long l, CString currentrule = L"") {
	nom = s_to_lower(nom);
	if (firstview == NULL)
		return;
	if (nom == "")
		return;

	firstview->nametoopen = nom;
	firstview->filetoopen.clear();
	firstview->stringtosearch = currentrule;
	s_utf8_to_unicode(firstview->filetoopen, STR(nom));
	if (l != -1)
		SendMessage(firstview->GetSafeHwnd(), ID_OUVRIR_DOCUMENT, (WPARAM)l, (LPARAM)1);
	else
		SendMessage(firstview->GetSafeHwnd(), ID_OUVRIR_DOCUMENT, (WPARAM)l, NULL);
}


static map<string, bool> coloractives;
static map<string, KifThreadLock*> colorlocks;

bool testactivecolor(string& t) {
	KifViewLock _kvl(colorlocks[t]);
	return coloractives[t];
}

void colorthread(void* arg) {
	CKIFFView* cview = (CKIFFView*)arg;
	string titre = cview->stitre;
	CRichEditCtrl& e = cview->GetRichEditCtrl();
	long db, d, dl, fl;
	long nx = -1;
	bool b = true;
	int i, lng, ps;
	bool test = false;
	bool stopall = true;
	int szc;

	while (1) {

		if (testactivecolor(titre))
			return;
		szc = cview->colorblocks.size();
		db = e.GetFirstVisibleLine();
		if (testactivecolor(titre))
			return;
		dl = e.LineIndex(db + 100);
		if (testactivecolor(titre))
			return;
		db = e.LineIndex(db);
		if (testactivecolor(titre))
			return;
		d = cview->colorblocks[0];
		if (testactivecolor(titre))
			return;
		b = cview->activecolorblocks[d];


		ps = 0;
		test = false;
		stopall = true;
		nx = -1;

		for (i = 0; i < szc; i++) {
			if (testactivecolor(titre))
				return;
			lng = cview->colorblocks[i];
			test = cview->activecolorblocks[lng];
			if (!test)
				stopall = false;

			if (db < lng)
				continue;

			ps = i;
			d = lng;
			b = test;
		}

		if (stopall)
			return;

		if (testactivecolor(titre))
			return;

		ps++;
		if (ps < szc) {
			if (testactivecolor(titre))
				return;

			fl = cview->colorblocks[ps];
			if (fl <= dl) {
				test = cview->activecolorblocks[fl];
				if (testactivecolor(titre))
					return;
				if (!test) {
					ps++;
					if (b) {
						d = fl;
						b = false;
					}
					else {
						if (testactivecolor(titre))
							return;
						cview->activecolorblocks[fl] = true;
					}
				}
			}
		}

		if (b == false) {
			if (testactivecolor(titre))
				return;

			if (ps < szc)
				nx = cview->colorblocks[ps];
			else
				nx = e.GetWindowTextLength();
			if (testactivecolor(titre))
				return;
			cview->activecolorblocks[d] = true;
			if (testactivecolor(titre))
				return;
			HWND hw = cview->GetSafeHwnd();

			::SendMessage(hw, MET_A_JOUR_COULEUR, d, nx);
		}
	}
}


LRESULT CKIFFView::SUGGERE(WPARAM wParam, LPARAM lParam) {
	POINT p;
	int d = lParam;
	int letype = wParam;

	CRichEditCtrl& e = GetRichEditCtrl();
	p = e.PosFromChar(d);
	ClientToScreen(&p);

	kifmethods sug;
	sug.x = p.x + 5;
	sug.y = p.y;
	sug.atype = stype.c_str();
	sug.listes = thevectorstrings[letype];
	sug.infos = thevectorinfos[letype];
	sug.DoModal();
	if (sug.letexte != "" && sug.resultat == L"")
		sug.resultat = LPCWSTR(sug.letexte);

	if (sug.resultat != L"") {
		if (sug.resultat[0] != '.')
			sug.resultat = L"." + sug.resultat;
		Addundos(_T(""), d, d);
		Addundos(sug.resultat.c_str(), d, d + sug.resultat.size());
		e.ReplaceSel(sug.resultat.c_str());
	}
	else {
		sug.resultat = L".";
		Addundos(_T(""), d, d);
		Addundos(sug.resultat.c_str(), d, d + sug.resultat.size());
		e.ReplaceSel(sug.resultat.c_str());

	}

	return 0;
}

//-------------------------------------------------------------------------------
bool CKIFFView::FindDeclarations(wstring& wlocalcode, int& letype) {
	if (wlocalcode == L"")
		return false;

	if (thevectorstrings.size() == 0)
		Updatesuggestion();

	bnfxs.init(wlocalcode);
	bnfxs.loadtoken(true);
	if (bnfxs.wstack.size() == 0 || bnfxs.stacktype.back() != 4)
		return false;

	wstring var = bnfxs.wstack.back();
	bnfxs.wstack.pop_back();
	bnfxs.stackln.pop_back();
	bnfxs.stacktype.pop_back();

	//D'abord on compte les '{}' pour savoir combien il en faut pour avoir une analyse complete...

	wchar_t fnc;
	int j;
	int sz = bnfxs.wstack.size();
	vector<wstring> vars;
	int declaration = 0;
	for (j = 0; j < sz; j++) {
		if (bnfxs.stacktype[j] == 4) {
			if (bnfxs.wstack[j] == var) {
				int i = j - 1;
				if (i >= 0) {
					if (bnfxs.wstack[i][0] == ',') {
						i = j - 2;
						bool stop = false;
						while (i >= 0 && kiftypes.find(bnfxs.wstack[i]) == kiftypes.end()) {
							if (bnfxs.wstack[i][0] == ';') {
								stop = true;
								break;
							}
							i--;
						}
						if (i < 0 || stop)
							continue;
					}

					if (kiftypes.find(bnfxs.wstack[i]) != kiftypes.end()) {
						if (declaration)
							vars.push_back(L"#");
						vars.push_back(bnfxs.wstack[i]);
					}
				}
			}
			continue;
		}

		fnc = bnfxs.wstack[j][0];
		switch (fnc) {
		case '(':
			declaration++;
			break;
		case ')':
			declaration--;
			break;
		case '{':
			if (vars.size() >= 2 && vars[vars.size() - 2] == L"#") {
				vars[vars.size() - 2] = L"{";
				break;
			}
			vars.push_back(L"{");
			break;
		case '}':
			while (vars.back() != L"{")
				vars.pop_back();
			vars.pop_back();
		}
	}

	if (vars.size()) {
		stype = L"";
		while (vars.size() && vars.back() == L"{")
			vars.pop_back();
		if (vars.size()) {
			stype = vars.back();
			letype = kiftypes[stype];
			if (equivalences.find(letype) != equivalences.end())
				letype = equivalences[letype];
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------

LRESULT CKIFFView::MAJCOULEUR(WPARAM wParam, LPARAM lParam) {
	CRichEditCtrl& e = GetRichEditCtrl();
	int longueur = e.GetWindowTextLength();
	long it = (long)wParam;
	long nx = (long)lParam;

	long fl = e.GetFirstVisibleLine();
	long d = e.LineIndex(e.LineFromChar(it));

	CString prev;
	long bloc;
	bloc = e.LineIndex(e.LineFromChar(nx));
	if (bloc > longueur)
		bloc = longueur;
	e.GetTextRange(d, bloc, prev);
	wstring ws = WCSTR(prev);

	e.HideSelection(TRUE, FALSE);
	ApplyColor(ws, d);
	e.HideSelection(FALSE, FALSE);

	e.SetSel(0, 0);
	e.LineScroll(fl);

	return 1;
}

LRESULT CKIFFView::MAJ(WPARAM wParam, LPARAM lParam) {
	SetFocus();
	CFrameWnd* pFrame = GetParentFrame();
	if (pFrame != NULL)
		pFrame->ActivateFrame();
	return 1;
}

extern "C" {
	void arretprg();
}

BOOL WINAPI SurBreak(__in  DWORD dwCtrlType) {
	arretprg();
	return TRUE;
}

void Arret(int sig) {
	arretprg();
}


CKIFFView::CKIFFView()
{
	// TODO: ajoutez ici du code de construction
	derniereposition = 0;
	begincolor = false;
	suggestion = true;
	firstdisplay = true;
	debugxip = 0;
	dernierelongueur = 0;
	currentVisible = -1;
	initcf = false;
	nbcurly = 0;
	nbsquare = 0;
	filetoopen = L"";

	if (firstview == NULL) {
		firstview = this;
		//SetConsoleCtrlHandler(SurBreak,TRUE);
		signal(SIGINT, Arret);
	}
}

CKIFFView::~CKIFFView()
{
	for (int i = 0; i < thebreaks.size(); i++) {
		if (thebreaks[i] == this) {
			thebreaks.erase(thebreaks.begin() + i);
			break;
		}
	}

	lesvues.erase(stitre);
	if (colorlocks.find(stitre) != colorlocks.end()) {
		KifViewLock _kvl(colorlocks[stitre]);
		coloractives[stitre] = true;
	}
}


void RuleDebug(int parseurCourant, XipResult* xr, void* data)  {
	//CKIFFView* view=(CKIFFView*)data;
	if (xr->sentences.size() == 0)
		return;
	XIPDEBUG xp;
	xp.parseurCourant = parseurCourant;
	xp.xipsent = xr;
	xp.DoModal();
	XipCleanCurrentXipResult(parseurCourant);
	XipSetCallBack(parseurCourant, RuleDebug, NULL);
}

CKIFFView* vuedebase = NULL;

void CKIFFView::fermertab() {
	if (vuedebase != this) {
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd(); //Get Handle to MainFrame

		CMDITabInfo mdiTabParams;
		if (firstview == this) {
			mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
			mdiTabParams.m_bActiveTabCloseButton = false;      // set to FALSE to place close button at right of tab area
			mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
			mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
			mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
			mdiTabParams.m_bTabCloseButton = false;
		}
		else {
			mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
			mdiTabParams.m_bActiveTabCloseButton = true;      // set to FALSE to place close button at right of tab area
			mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
			mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
			mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
			mdiTabParams.m_bTabCloseButton = true;
		}
		pFrame->EnableMDITabbedGroups(TRUE, mdiTabParams);
		vuedebase = this;
	}
}

void CKIFFView::Addundos(CString s, long d, long f) {

	ModifItem md(s, d, f);
	listundos.push_back(md);

	if (listundos.size() > 100) {
		for (int i = 0; i < 10; i++)
			listundos.pop_front();
	}
}


void CKIFFView::Keepundos(CString& r) {
	CRichEditCtrl& e = GetRichEditCtrl();
	CString s;

	long d, f;
	e.GetSel(d, f);
	s = e.GetSelText();

	Addundos(s, d, f);
	Addundos(r, d, d + r.GetLength());
}

LRESULT CKIFFView::OnFindDialogMessage(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_pFindDialog != NULL);

	// If the FR_DIALOGTERM flag is set,
	// invalidate the handle identifying the dialog box.
	if (m_pFindDialog->IsTerminating())
	{
		m_pFindDialog = NULL;
		return 0;
	}

	CRichEditCtrl& e = GetRichEditCtrl();
	// If the FR_FINDNEXT flag is set,
	// call the application-defined search routine
	// to search for the requested string.
	if (m_pFindDialog->FindNext())
	{
		//read data from dialog
		findString = m_pFindDialog->GetFindString();
		bMatchCase = m_pFindDialog->MatchCase() == TRUE;
		bMatchWholeWord = m_pFindDialog->MatchWholeWord() == TRUE;
		bSearchDown = m_pFindDialog->SearchDown() == TRUE;
		//with given name do search
		FindText(findString, bMatchCase, bMatchWholeWord, bSearchDown);
	}
	else {
		long l, r;
		BOOL search = TRUE;
		if (m_pFindDialog->ReplaceCurrent()) {
			findString = m_pFindDialog->GetFindString();
			e.GetSel(l, r);
			if (l == r)
				search = FindText(findString, bMatchCase, bMatchWholeWord, bSearchDown);
			if (search) {
				replacementString = m_pFindDialog->GetReplaceString();
				e.HideSelection(TRUE, FALSE);
				Keepundos(replacementString);
				e.ReplaceSel(replacementString, 1);
				e.HideSelection(FALSE, FALSE);
			}
		}
		else
		if (m_pFindDialog->ReplaceAll()) {
			findString = m_pFindDialog->GetFindString();
			bMatchCase = m_pFindDialog->MatchCase() == TRUE;
			bMatchWholeWord = m_pFindDialog->MatchWholeWord() == TRUE;
			bSearchDown = m_pFindDialog->SearchDown() == TRUE;
			replacementString = m_pFindDialog->GetReplaceString();
			e.HideSelection(TRUE, FALSE);
			while (FindText(findString, bMatchCase, bMatchWholeWord, bSearchDown)) {
				Keepundos(replacementString);
				e.ReplaceSel(replacementString, 1);
			}
			e.HideSelection(FALSE, FALSE);
		}
	}
	return 0;
}


BOOL CKIFFView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: changez ici la classe ou les styles Window en modifiant
	//  CREATESTRUCT cs
	//cs.dwExStyle |= ES_AUTOHSCROLL| ES_MULTILINE;
	//cs.style &= ~ES_AUTOHSCROLL;
	return CRichEditView::PreCreateWindow(cs);
}

// dessin de CKIFFView

void CKIFFView::OnDraw(CDC* /*pDC*/)
{

	CKIFFDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: ajoutez ici le code de dessin pour les données natives
}


// impression de CKIFFView


void CKIFFView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CKIFFView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// préparation par défaut
	return DoPreparePrinting(pInfo);
}

void CKIFFView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ajoutez une initialisation supplémentaire avant l'impression
}

void CKIFFView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ajoutez un nettoyage après l'impression
}

void CKIFFView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	MapInitInfos();
	//ClientToScreen(&point);
	CRichEditCtrl& e = GetRichEditCtrl();
	long g = e.CharFromPos(point);
	int l = e.LineFromChar(g);
	TCHAR sx[1024];
	int ln = e.GetLine(l, sx, 512);
	sx[ln] = 0;
	long deb = e.LineIndex(l);
	g = g - deb;
	//We isolate the string...
	//to the left
	inittableutf8();
	l = g;
	while (l > 0 && sx[l] > 32 && !c_is_punctuation(sx[l])) l--;
	deb = g;
	while (sx[deb] > 32 && !c_is_punctuation(sx[deb])) deb++;
	CString s;
	for (g = l + 1; g < deb; g++)
		s += sx[g];
	if (s.GetLength() != 0) {
		string code;
		fromCString(code, s);
		if (theinfos.find(code) != theinfos.end()) {
			fromstring(s, theinfos[code]);
			AfxMessageBox(s);
		}
	}
}

void CKIFFView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

void CleanFromSession(HWND xwnd) {
	CWnd* w32 = CWnd::FromHandle(xwnd);
	if (w32 != NULL)
		w32->DestroyWindow();
}

// diagnostics pour CKIFFView

#ifdef _DEBUG
void CKIFFView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CKIFFView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CKIFFDoc* CKIFFView::GetDocument() const // la version non Debug est inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKIFFDoc)));
	return (CKIFFDoc*)m_pDocument;
}
#endif //_DEBUG


// gestionnaires de messages pour CKIFFView

void CKIFFView::Choixligne(string& fname, long l) {

	if (firstview == this || fname.size() == 0)
		return;

	char localpath[1024];
	memset(localpath, 0, 1024);
	_fullpath(localpath, STR(fname), 1024);
	string errorfilename(localpath);
	CKIFFView* vuecourante = Lavue(errorfilename);
	if (vuecourante == NULL)
		OuvrirDocument(errorfilename, -1);

	if (vuecourante != NULL) {
		vuecourante->SetFocus();
		vuecourante->SelectionneLigne(l);
	}
}


LRESULT CKIFFView::Ouverture(WPARAM wParam, LPARAM lParam) {
	if (filetoopen.size() == 0)
		return 0;

	(CKIFFDoc*)theApp.OpenDocumentFile((WCHAR*)filetoopen.c_str());
	if (lParam == 1) {
		CKIFFView* vue = Lavue(nametoopen);
		if (vue != NULL) {
			vue->SetFocus();
			vue->SelectionneLigne((long)wParam);
			if (stringtosearch != L"") {
				int i = stringtosearch.Find(L"\r\n", 0);
				if (i != -1) {
					i = stringtosearch.Find(L"\r\n", i + 1);
					int ending = stringtosearch.Find(L"\r\n", i + 1);
					stringtosearch = stringtosearch.Mid(i, ending - i);
					stringtosearch.Trim();
				}

				vue->FindText(stringtosearch, true, false, true);
			}
		}
	}
	return 1;
}


void CKIFFView::Afficheerreur(string stitre) {
	string lecode = theerrormsg();
	if (lecode.size() == 0)
		return;
	if (kifGlobals->Currentfilename() != "%%STRING")
		stitre = kifGlobals->Currentfilename();

	wstring base;
	s_utf8_to_unicode(base, STR(lecode));
	AfxMessageBox((WCHAR*)base.c_str());

	if (lecode.find("XIP(333)", 0) == -1)
		Choixligne(stitre, kiflignecourantedebug());
	else {
		//A grammar file...
		//we extract the filename and the line from the string
		string ln;
		int i;
		int pos = lecode.find("line:", 0);
		if (pos != -1) {
			for (i = pos + 6; lecode[i] != ' '; i++)
				ln += lecode[i];
			i += 4;
			pos = lecode.find(" -->", i);
			if (pos != -1) {
				string fname;
				fname = lecode.substr(i, pos - i);
				Choixligne(fname, atol(STR(ln)));
			}
		}
	}
}


void CKIFFView::Affichage(CString& s, bool thread) {
	if (firstview == NULL)
		return;

	CRichEditCtrl& e = firstview->GetRichEditCtrl();
	s += ENDL;
	int nLength = e.GetWindowTextLength();
	// put the selection at the end of text
	e.SetSel(nLength, nLength);
	// replace the selection
	char* res = displaykifsession();
	long f;
	if (res != NULL && res[0] != 0) {
		f = firstview->derniereposition - 1;
		fromstring(commande, res);
		e.ReplaceSel(commande, 1);
		::SendMessage(firstview->GetSafeHwnd(), MET_A_JOUR, 0, 0);
	}
}

LRESULT CKIFFView::AfficheCourant(WPARAM wParam, LPARAM lParam) {

	CString* s = (CString*)wParam;
	CRichEditCtrl& er = GetRichEditCtrl();
	int nLength = er.GetWindowTextLength();
	// put the selection at the end of text
	er.SetSel(nLength, nLength);
	er.ReplaceSel(*s, 1);
	delete s;
	if (firstdisplay) {
		lexchange.encours = true;
		SetFocus();
		CFrameWnd* pFrame = GetParentFrame();
		if (pFrame != NULL)
			pFrame->ActivateFrame();
		firstdisplay = false;
	}
	return 1;
}

void DisplayValue(string s, void* o) {
	if (firstview == NULL || s.size() == 0)
		return;
	CString* cs = new CString;
	fromstring(*cs, s);
	SendMessage(firstview->GetSafeHwnd(), IDE_AFFICHAGE_COURANT, (WPARAM)cs, 0);
}

LRESULT CKIFFView::afficher(WPARAM wParam, LPARAM lParam) {
	if (wParam == 1)
		Afficheerreur("");
	else
		Affichage(lexchange.s, true);
	return 1;
}

void executekif(echange& e) {
	if (!runkifsession())
		SendMessage(e.vue, ID_AFFICHER_ERREUR, (WPARAM)1, 0);
	else
		SendMessage(e.vue, ID_AFFICHER_ERREUR, (WPARAM)0, 0);
	
	Updatesuggestion();
	e.encours = false;
	activecursor = 2;
}

//DWORD WINAPI executekif(LPVOID arg) {		
void executekifinthread(void* arg) {
	echange* e = (echange*)arg;
	if (e->debugging)
		kifGlobals->threadmode = true;
	if (!runkifsession())
		SendMessage(e->vue, ID_AFFICHER_ERREUR, (WPARAM)1, 0);
	else
		SendMessage(e->vue, ID_AFFICHER_ERREUR, (WPARAM)0, 0);
	
	Updatesuggestion();
	e->encours = false;
	activecursor = 2;
	if (e->debugging)
		kifGlobals->threadmode = false;
	_endthread();
}


void CKIFFView::runkif(CString& s) {
	if (lexchange.encours)
		return;
	if (s != "") {
		if (!openkifsession())
			return;
		TCHAR c = 2;
		s.Remove(c);
		KifSetDisplayFunction(DisplayValue, NULL);
		kifGlobals->doubledisplay = false;
		if (firstview != NULL)
			firstview->firstdisplay = true;
		string lecode;
		fromCString(lecode, s);
		kiffilename(STR(stitre));
		string args;
		fromCString(args, basearguments);
		kifsetarguments(args);
		int i = compilekifsession(STR(lecode));
		if (i == -1) {
			Afficheerreur(stitre);
			lexchange.encours = false;
			return;
		}

		if (lexchange.compilation) {
			lexchange.compilation = false;
			lexchange.encours = false;
			return;
		}

		//We initialize the main thread id (to be used in FLTK)
		KifSetMainThread();

		if (debugxip)
			XipBaseParseur(RuleDebug, true, NULL);
		else
			XipBaseParseur(NULL, false, NULL);

		activecursor = 1;
		lexchange.initialisation(this, s);
		//if (windowmode() && lexchange.debugging==false)
		//	executekif(lexchange);
		//else
		_beginthread(executekifinthread, 0, (void*)&lexchange);
	}
}

void CKIFFView::AfficheAnalyse() {
	if (firstview == NULL)
		return;

	char* res = displaykifsession();
	if (res != NULL && res[0] != 0) {
		CRichEditCtrl& e = firstview->GetRichEditCtrl();
		fromstring(commande, res);
		e.ReplaceSel(commande, 1);
	}
}

void CKIFFView::executelecode() {
	if (lexchange.encours)
		return;
	if (accumulecode != "") {
		openkifsession();
		KifSetDisplayFunction(DisplayValue, NULL);
		kifGlobals->doubledisplay = false;
		firstview->firstdisplay = true;
		//char* code=wcstombs( pmbhello, pwchello, MB_CUR_MAX );
		string lecode;
		fromCString(lecode, accumulecode);
		kiffilename(STR(stitre));
		string args;
		fromCString(args, basearguments);
		kifsetarguments(args);
		int i = compilekifsession(STR(lecode));

		if (i != -1) {
			activecursor = 1;
			//DWORD tid;
			lexchange.initialisation(this, accumulecode);
			//CreateThread(NULL,0,&executekif,(LPVOID)&lexchange,0,&tid);			
			_beginthread(executekifinthread, 0, (void*)&lexchange);
		}
		else
			Afficheerreur(stitre);
		accumulecode = "";
		nbcurly = 0;
		nbsquare = 0;
	}
}

bool CKIFFView::Couleur(int r, int g, int b) {
	if (begincolor) {
		CHARFORMAT2 cfbis = GetCharFormatSelection();
		if (cfbis.crTextColor == RGB(r, g, b))
			return false;
	}

	CRichEditCtrl& e = GetRichEditCtrl();
	cf.dwEffects &= ~CFE_ITALIC;
	cf.crTextColor = RGB(r, g, b);
	e.SetSelectionCharFormat(cf);
	return true;
}


void CKIFFView::OnInitialUpdate() {
	CRichEditView::OnInitialUpdate();
	CRichEditCtrl& e = GetRichEditCtrl();

	cf = GetCharFormatSelection();
	initcf = true;
	cf.dwMask |= CFM_COLOR;
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.szFaceName[0] = 'A';
	cf.szFaceName[1] = 'r';
	cf.szFaceName[2] = 'i';
	cf.szFaceName[3] = 'a';
	cf.szFaceName[4] = 'l';
	cf.szFaceName[5] = 0;
	e.SetDefaultCharFormat(cf);
	if (firstview != this)
		e.SetTargetDevice(NULL, 1);
}

void CKIFFView::Italique(bool add) {
	CHARFORMAT2 cfbis = GetCharFormatSelection();
	cfbis.dwMask &= ~CFM_COLOR;
	cfbis.dwEffects |= CFE_AUTOCOLOR;
	if (add)
		cfbis.dwEffects |= CFE_ITALIC;
	else
		cfbis.dwEffects &= ~CFE_ITALIC;

	GetRichEditCtrl().SetSelectionCharFormat(cfbis);
}

void CKIFFView::colore1000() {

	begincolor = true;
	bnfxs.stackln.erase(bnfxs.stackln.begin() + 5000, bnfxs.stackln.end());
	bnfxs.stacksize.erase(bnfxs.stacksize.begin() + 5000, bnfxs.stacksize.end());
	bnfxs.stacktype.erase(bnfxs.stacktype.begin() + 5000, bnfxs.stacktype.end());

	stacktype.clear();
	CRichEditCtrl& e = GetRichEditCtrl();
	long mlx = bnfxs.stackln.back();
	mlx = e.LineFromChar(mlx);
	mlx = e.LineFromChar(e.LineIndex(mlx));
	long fin = e.GetTextLength() - 1;
	fin = e.LineFromChar(fin);
	
	for (int i = 0; i < fin; i++) {
		if (i < mlx)
			stacktype.push_back(0);
		else
			stacktype.push_back(1);
	}

	wstring ws;
	ApplyColor(ws, 0, -1, false);
}

void CKIFFView::colorisation() {
	if (!begincolor)
		return;

		int i = 0;
	CRichEditCtrl& e = GetRichEditCtrl();

	long debut = e.GetFirstVisibleLine();
	if (debut >= stacktype.size())
		return;

	int linenumber = debut;
	long d, f;
	e.GetSel(d, f);
	if (d == f) {
		d = e.LineIndex(linenumber);
		f = d;
	}

	long fin = e.GetTextLength() - 1;
	fin = e.LineFromChar(fin);
	int nb = min(debut + 120, stacktype.size());
	if (fin < stacktype.size()) {
		stacktype.erase(stacktype.begin() + debut, stacktype.end());
		for (i = debut; i < fin; i++)
			stacktype.push_back(1);
	}
	else {
		for (i = debut; i < nb; i++) {
			if (stacktype[i] == 1) {
				debut = i;
				break;
			}
		}
		if (debut != i)
			return;
	}

	fin = nb;
	for (i = debut; i < nb; i++) {
		if (stacktype[i] == 0) {
			fin = i;
			break;
		}
	}

	fin = e.LineIndex(fin);
	debut = e.LineIndex(debut);
	CString txt;

	if (fin == -1 || fin >= e.GetTextLength() - 1)
		fin = e.GetTextLength() - 1;

	e.GetTextRange(debut, fin, txt);
	wstring ws = LPCWSTR(txt);

	SetRedraw(FALSE);
	bool toupdate = ApplyColor(ws, debut, fin);
	SetRedraw(TRUE);

	for (i = linenumber; i < nb; i++)
		stacktype[i] = 0;

	if (toupdate) {
		Invalidate();
		UpdateWindow();
	}
	linenumber = e.LineIndex(linenumber);
	e.SetSel(d, f);
	for (i = 0; i < stacktype.size(); i++) {
		if (stacktype[i] == 1)
			return;
	}
	begincolor = false;
}


bool CKIFFView::ApplyColor(wstring& sw, int offset, int mxpos, bool rebuilt) {
	if (rebuilt) {
		bnfxs.init(sw);
		bnfxs.loadtoken();
	}

	if (bnfxs.stacktype.size() >= 10000) {
		colore1000();
		return false;
	}

	CRichEditCtrl& e = GetRichEditCtrl();
	CHARRANGE positions;
	int nb = bnfxs.stacktype.size();
	size_t istack;
	bool colorized = false;
	for (istack = 0; istack < nb; istack++) {
		switch (bnfxs.stacktype[istack]) {
		case 1:
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax>mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(255, 0, 0))
				colorized = true;
			break;
		case 2:	
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax>mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(150, 150, 250))
				colorized = true;
			break;
		case 10: //string between @"..."@
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax>mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(150, 150, 150))
				colorized = true;
			break;
		case 3: //keywords in colorkeywords
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax>mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(0, 0, 255))
				colorized = true;
			break;
		case 4: //regular keyword in context
			//we skip if the file size is too large...
			if (istack<bnfxs.stacktype.size() - 1) {//regular keyword preceded either by a: '[' or '(' (previous code is 7) or a '.' (previous code is 6)
				if (istack>0 && bnfxs.stacktype[istack - 1] == 6 && bnfxs.stacktype[istack + 1] == 7) {
					positions.cpMin = bnfxs.stackln[istack] + offset - 1;
					positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
					if (mxpos != -1 && positions.cpMax>mxpos)
						break;
					e.SetSel(positions);
					if (Couleur(160, 100, 10))
						colorized = true;
				}
				else
				if (bnfxs.stacktype[istack + 1] == 7) {//regular keyword followed with: '[' or '('
					positions.cpMin = bnfxs.stackln[istack] + offset - 1;
					positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
					if (mxpos != -1 && positions.cpMax>mxpos)
						break;
					e.SetSel(positions);
					if (Couleur(140, 0, 160))
						colorized = true;
				}
			}
			break;
		case 5://comments
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax>mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(85, 180, 85))
				colorized = true;
			break;
		case 8:// '#' or '$'
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax>mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(255, 120, 120))
				colorized = true;
			break;
		case 9:// '?'
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax>mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(160, 100, 10))
				colorized = true;
			break;
		}
	}
	m_bSyncCharFormat = TRUE;
	m_bSyncParaFormat = TRUE;
	return colorized;
}

void CKIFFView::Colorie(long d, long f, int nFirstVisible) {
	if (firstview == this)
		return;

	firstview->SetFocus();
	BOOL modif = GetDocument()->IsModified();
	CRichEditCtrl& e = GetRichEditCtrl();
	e.SetUndoLimit(0);

	if (nFirstVisible == -1) {
		nFirstVisible = e.GetFirstVisibleLine();
		e.GetSel(d, f);
	}
	long longueur = e.GetWindowTextLength();

	SetRedraw(FALSE);

	e.SetSel(0, longueur);
	Couleur(0, 0, 0);
	dernierelongueur = longueur;
	CString s;
	GetWindowText(s);

	wstring ws = WCSTR(s);
	ws = s_replacestring(ws, L"\r\n", L"\r");
	longueur = ws.size();

	CWaitCursor wait;
	ApplyColor(ws, 0);

	for (int i = 0; i < thebreaks.size(); i++) {
		if (thebreaks[i] == this) {
			s.Replace(L"\r\n", L"\n");
			long pos = 0;
			TCHAR c = 2;
			while (pos != -1) {
				pos = s.Find(c, pos);
				if (pos != -1) {
					long d = e.LineIndex(e.LineFromChar(pos));
					long f = e.LineIndex(e.LineFromChar(pos) + 1);
					e.SetSel(d, f);
					Italique(true);
					pos++;
				}
			}
			break;
		}
	}

	GetDocument()->SetModifiedFlag(modif);
	SetFocus();
	
	e.SetSel(0, 0);
	if (nFirstVisible > 0)
		e.LineScroll(nFirstVisible, 0);
	e.SetSel(d, f);


	SetRedraw(TRUE);
	Invalidate();
	UpdateWindow();
}


void CKIFFView::OnKifExecuter()
{
	//// TODO: Add your command handler code here
	long d, f;
	CString s;
	TCHAR sx[1024];

	if (kifencours())
		return;

	if (firstview == this) {
		CRichEditCtrl& e = GetRichEditCtrl();
		s = e.GetSelText();
		s = s.Trim();
		if (s == "") {
			e.GetSel(d, f);
			int l = e.LineFromChar(d) - 1;
			int ln = e.GetLine(l, sx, 512);
			sx[ln] = 0;
			s = sx;
			s = s.Trim();
		}
	}
	else
		GetWindowText(s);

	if (deb != NULL) {
		delete deb;
		deb = NULL;
	}

	runkif(s);
}

void CKIFFView::OnWxipNobreak()
{
	// TODO: Add your command handler code here
	if (!closekifsession())
		return;
	lexchange.debugging = false;
	//We clean the output window
	if (firstview != NULL) {
		CRichEditCtrl& e = firstview->GetRichEditCtrl();
		e.SetSel(0, -1);
		firstview->SetWindowText(_T(""));
	}

	OnKifExecuter();
}

void CKIFFView::OnKifReexecuter()
{
	// TODO: Add your command handler code here
	if (!closekifsession())
		return;
	CString txt;
	TCHAR c = 2;
	lexchange.debugging = false;
	for (int i = 0; i < thebreaks.size(); i++) {
		CRichEditCtrl& e = thebreaks[i]->GetRichEditCtrl();
		string titre = thebreaks[i]->stitre;
		e.GetWindowText(txt);
		txt.Replace(L"\r\n", L"\n");		
		long pos = 0;
		while (pos != -1) {
			pos = txt.Find(c, pos);
			if (pos != -1) {
				ajoutebreak(STR(titre), e.LineFromChar(pos) + 1);
				lexchange.debugging = true;
				pos++;
			}
		}
	}
	OnKifExecuter();
}


void CKIFFView::OnKifPasaPas() {
	if (!closekifsession())
		return;
	CString txt;
	TCHAR c = 2;
	lexchange.debugging = false;
	for (int i = 0; i < thebreaks.size(); i++) {
		CRichEditCtrl& e = thebreaks[i]->GetRichEditCtrl();
		string titre = thebreaks[i]->stitre;
		e.GetWindowText(txt);
		txt.Replace(L"\r\n", L"\n");		
		long pos = 0;
		while (pos != -1) {
			pos = txt.Find(c, pos);
			if (pos != -1) {
				ajoutebreak(STR(titre), e.LineFromChar(pos) + 1);
				pos++;
			}
		}
	}
	lexchange.debugging = true;
	modebug(true);
	OnKifExecuter();
}

void CKIFFView::OnKifAsThread()
{
	// TODO: Add your command handler code here
	if (!closekifsession())
		return;

	CString txt;
	TCHAR c = 2;
	lexchange.debugging = false;
	for (int i = 0; i < thebreaks.size(); i++) {
		CRichEditCtrl& e = thebreaks[i]->GetRichEditCtrl();
		string titre = thebreaks[i]->stitre;
		e.GetWindowText(txt);
		txt.Replace(L"\r\n", L"\n");		
		long pos = 0;
		while (pos != -1) {
			pos = txt.Find(c, pos);
			if (pos != -1) {
				ajoutebreak(STR(titre), e.LineFromChar(pos) + 1);
				lexchange.debugging = true;
				pos++;
			}
		}
	}

	OnKifExecuter();
}

void CKIFFView::OnKifCompiler()
{
	// TODO: Add your command handler code here
	if (!closekifsession())
		return;
	lexchange.compilation = true;
	OnKifExecuter();
}

void CKIFFView::OnKifLister()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();

	char* res = listecodes();
	fromstring(commande, res);
	e.ReplaceSel(commande, 1);
	derniereposition = e.GetWindowTextLength();
	e.SetSel(derniereposition, derniereposition);
	Colorie();
}


void CKIFFView::OnKifRaz()
{
	// TODO: Add your command handler code here
	closekifsession();
	accumulecode = "";
	nbcurly = 0;
	nbsquare = 0;
}

void CKIFFView::SelectionneLigne(long l) {
	CRichEditCtrl& e = GetRichEditCtrl();
	long d = e.LineIndex(l - 1);
	long f = d + e.LineLength(d);
	e.SetSel(d, f);
}

CString CKIFFView::GetLineText(long l) {
	CRichEditCtrl& e = GetRichEditCtrl();
	TCHAR sx[1024];
	int ln = e.GetLine(l, sx, 512);
	sx[ln] = 0;
	return sx;
}



void CKIFFView::Chemin(CString titre) {
	string ti;
	fromCString(ti, titre);
	inittableutf8();
	if (stitre == "") {
		stitre = s_to_lower(ti);
		lesvues[stitre] = this;
	}
	else {
		if (stitre != ti) {
			lesvues.erase(ti);
			stitre = s_to_lower(ti);
			lesvues[stitre] = this;
		}
	}
}

void CKIFFView::comptebrackets(CString& s) {
	char instr = 0;
	for (int i = 0; i < s.GetLength(); i++) {
		if (s[i] == '\'') {
			if (instr == 0)
				instr = 1;
			else
			if (instr == 1)
				instr = 0;
			continue;
		}

		if (s[i] == '"') {
			if (instr == 0)
				instr = 2;
			else
			if (instr == 2) {
				if (s[i - 1] != '\\')
					instr = 0;
			}
			continue;
		}

		if (instr)
			continue;

		switch (s[i]) {
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
extern Exported const wchar_t* wkifelse;
extern Exported const wchar_t* wkifelif;
extern Exported const wchar_t* wkifif;

bool Teststring(CString& s, const wchar_t* ch, wchar_t fin) {
	bool tr = true;
	int i = 0;
	int ln = wcslen(ch);
	for (; i < ln; i++) {
		if (s[i] != ch[i])
			return false;
	}
	if (s[i] <= 32 || s[i] == fin || s[i] == '/')
		return true;
	return false;
}

static bool testKifKeyword(wchar_t* wcom) {
	static std::wregex* pattern = NULL;

	if (pattern == NULL) {
		string keywords("\\b");
		keywords += kiffor;
		keywords += " *\\(|\\b";
		keywords += kifwhile;
		keywords += " *\\(|\\b";
		keywords += kifswitch;
		keywords += " *\\(|\\b";
		keywords += kifif;
		keywords += " *\\(";
		wstring reg;
		s_utf8_to_unicode(reg, STR(keywords));
		pattern = new std::wregex(reg);
	}

	return std::regex_search(wcom, *pattern);
}
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
void CKIFFView::CouleurDepuisDebutLigne(long d, long f) {
	//On recupere la position
	CRichEditCtrl& e = GetRichEditCtrl();
	long l = e.LineFromChar(d);
	//Puis le debut de cette ligne...
	long debut = e.LineIndex(l);
	if (f - debut > 0) {
		CString prev;
		e.GetTextRange(debut, f, prev);

		e.HideSelection(TRUE, FALSE);
		//Puis les caracteres de la ligne que l'on transforme
		wstring xs = LPCWSTR(prev);
		e.SetSel(debut, f);
		Couleur(0, 0, 0);
		ApplyColor(xs, debut,f);
		e.SetSel(f, f);

		e.HideSelection(FALSE, FALSE);
	}
}
//--------------------------------------------------------------------------------------------------------
afx_msg void CKIFFView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	listdos.clear();
	if (nChar < 32 && nChar != 9)
		return CWnd::OnChar(nChar, nRepCnt, nFlags);

	CRichEditCtrl& e = GetRichEditCtrl();
	long D, F;
	e.GetSel(D, F);
	CString prev;

	if (nChar == '.' && suggestion) {
		e.GetTextRange(0, D, prev);
		wstring wtxt = WCSTR(prev);
		int letype;
		if (FindDeclarations(wtxt, letype)) {
			PostMessage(MET_SUGGERE_METHODE, letype, F);
			return;
		}
		prev = "";
	}

	if (nChar == 125) {
		int l = e.LineFromChar(F);
		TCHAR sx[1024];
		e.LineFromChar(D);
		int ln = e.GetLine(l, sx, 512);
		sx[ln] = 0;
		CString sn(sx);
		sx[ln - 1] = 0;
		prev = sx;
		sn.Trim();
		if (!sn.GetLength() && ln > 4) {
			long firstline = FindCorrespondance(D, F, 1);
			if (firstline != -1) {
				int firstlength = e.GetLine(firstline, sx, 512);
				if (firstlength > 0) {
					sx[firstlength] = 0;
					CString sfirst(sx);
					sfirst.TrimLeft();
					ln = firstlength - sfirst.GetLength();
				}
				else
					ln = 0;

				while (ln > 0) {
					sn += " ";
					ln--;
				}

				long d = e.LineIndex(l);
				long f = d + e.LineLength(d);

				Addundos(prev, d, f);

				sn += "}";
				e.SetSel(d, f);
				e.ReplaceSel(sn, 1);

				f = d + sn.GetLength();
				Addundos(sn, d, f);
				return;
			}
		}
	}

	if (D != F)
		e.GetTextRange(D, F, prev);
	else
		CouleurDepuisDebutLigne(D, F);

	CWnd::OnChar(nChar, nRepCnt, nFlags);
	Addundos(prev, D, F);
	char n[] = { nChar, 0 };
	CString s(n);
	Addundos(s, D, D + 1);
}
//--------------------------------------------------------------------------------------------------------
BOOL CKIFFView::PreTranslateMessage(MSG* pMsg) {
	static long lastd;
	
	long d, f;
	CRichEditCtrl& e = GetRichEditCtrl();
	e.GetSel(d, f);
	int l;

	l = e.LineFromChar(f);
	char ch[20];
	sprintf_s(ch, 20, "%d", l + 1);
	commande = ch;
	leMain->AfficheLigne(commande);

	if (pMsg->message != 256 && pMsg->message != 258 && dernierelongueur) {
		if (pMsg->wParam != VK_OEM_PERIOD && pMsg->wParam != VK_BACK && pMsg->wParam != VK_DELETE && pMsg->wParam != VK_RETURN) {
			colorisation();
			return CRichEditView::PreTranslateMessage(pMsg);
		}
	}
	
	int longueur = e.GetWindowTextLength();
	if (!longueur) {
		nbcurly = 0;
		nbsquare = 0;
		accumulecode = "";
	}

	if (pMsg->message == 256) {
		CString prev;
		if (pMsg->wParam == VK_BACK) {
			if (d || d != f) {
				if (d != f)
					e.GetTextRange(d, f, prev);
				else
					e.GetTextRange(--d, f, prev);
				Addundos(prev, d, f);
				Addundos(_T(""), d, d);
			}
		}
		else
		if (pMsg->wParam == VK_DELETE) {
			if (f < longueur || d != f) {
				if (d == f)
					e.GetTextRange(d, ++f, prev);
				else {
					e.GetTextRange(d, f, prev);
					//In the case of a partial chunk removal from the editor, the remaining is replaced by the system with a CR
					//In that case, we must remove it, unless we started our deletion from the beginning of the line...
					if (d && prev.Right(1) == 13) {
						//first we check if it is the beginning of the line...
						CString lcar;
						e.GetTextRange(d - 1, d, lcar);
						if (lcar[0] != 13) //it is not the case, we need to remove the trailing CR from the string
							e.GetTextRange(d, --f, prev);
					}
				}
				Addundos(prev, d, f);
				Addundos(_T(""), d, d);
			}
		}
		else
		if (pMsg->wParam == VK_RETURN) {
			if (firstview != this) {
				//we record this last carriage return for further treatment
				//we are in the pre-handling of the carriage return since pMsg->message==256
				prev = e.GetSelText();
				Addundos(prev, d, f);
				lastd = d;
				if (stacktype.size() != 0) {
					l = e.LineFromChar(d);
					stacktype.insert(stacktype.begin() + l, 0);
				}
			}
		}

	}


	if (longueur != dernierelongueur) {
		if (dernierelongueur == 0 || (pMsg->message == 258 && pMsg->wParam == VK_RETURN)) {
			if (dernierelongueur == 0 && firstview != this)
				Colorie();
			if (pMsg->message == 258 && pMsg->wParam == VK_RETURN) {
				TCHAR sx[1024];
				l = e.LineFromChar(d) - 1;
				int ln = e.GetLine(l, sx, 512);
				sx[ln] = 0;
				int sxlen = ln;
				bool exec = false;
				if (firstview == this) {
					commande = sx;
					commande = commande.TrimRight();
					if (commande != "") {
						if (accumulecode != "")
							accumulecode += "\r\n";
						accumulecode += commande;
						wchar_t lst = commande[commande.GetLength() - 1];
						switch (lst) {
						case ',':
						case ':':
							break;
						case ')':
							if (testKifKeyword(WCSTR(commande)) == true)
								break;
						default:
							comptebrackets(commande);
							if (nbcurly == 0 && nbsquare == 0)
								exec = true;
						}
					}
					else {
						nbcurly = 0;
						nbsquare = 0;
						accumulecode = "";
						dernierelongueur = longueur;
						return CRichEditView::PreTranslateMessage(pMsg);
					}
				}

				//We get the trailing white chars from command
				e.HideSelection(TRUE, FALSE);
				wstring xs(sx);
				xs += '\r';
				CString selse(sx);
				selse.Trim();
				int lnw;
				bool belse = false;
				commande = "";
				if (Teststring(selse, wkifelse, L'{') || Teststring(selse, wkifelif, L'{')) {
					//we push up to previous "if"
					lnw = FirstEnd(d, f);
					belse = true;
					if (lnw != -1) {
						int firstlength = e.GetLine(lnw, sx, 512) - 1;
						if (firstlength > 0) {
							sx[firstlength] = 0;
							CString sfirst(sx);
							sfirst.TrimLeft();
							lnw = firstlength - sfirst.GetLength();
							while (lnw > 0) {
								commande += " ";
								lnw--;
							}
						}
					}
				}
				else {
					lnw = FirstChar(d, f, commande);
					while (lnw > 0) {
						commande += " ";
						lnw--;
					}
				}

				if (belse) {
					d = e.LineIndex(l);
					e.SetSel(d, f);
					selse.Insert(0, commande);
					selse += "\r";
					selse += commande;
					lnw = GetBlankSize();
					while (lnw > 0) {
						selse += " ";
						lnw--;
					}
					commande = selse;
					xs = commande;
				}

				if (commande != "")
					e.ReplaceSel(commande, 1);

				commande = _T("\r") + commande;
				Addundos(commande, lastd, lastd + commande.GetLength());

				e.GetSel(d, f);
				long dx = e.LineIndex(l);
				bool bg = begincolor;
				begincolor = false;
				e.SetSel(dx, d);
				Couleur(0, 0, 0);
				ApplyColor(xs, dx);
				e.SetSel(d, f);
				begincolor = bg;
				
				e.HideSelection(FALSE, FALSE);

				if (exec) {
					if (!lexchange.encours) {
						accumulecode += "\r\n";
						executelecode();
					}
					else {
						if (accumulecode != "") {
							nbcurly = 0;
							nbsquare = 0;
							accumulecode = _T("");
							DisplayValue("Cannot execute instructions\r", NULL);
						}
					}
				}
			}
		}

		dernierelongueur = longueur;
	}
	return CRichEditView::PreTranslateMessage(pMsg);
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

extern CKIFFApp theApp;

LockedThread* loquet = NULL;
//DWORD WINAPI debuggage(LPVOID arg) {
void debuggage(void* arg) {
	if (deb->DoModal() == IDOK) {
		delete deb;
		loquet->Released();
		deb = NULL;
	}
	_endthread();
}

void winaffichage(string vrs, string pile, string fname, long ln) {
	CKIFFView* vuecourante = Lavue(fname);
	if (vuecourante == NULL) {
		OuvrirDocument(fname, -1);
		vuecourante = Lavue(fname);
	}

	SendMessage(vuecourante->GetSafeHwnd(), MET_A_JOUR, 0, 0);
	vuecourante->SelectionneLigne(ln);
	activecursor = 2;

	bool actif = true;
	if (deb == NULL) {
		actif = false;
		deb = new debogueur;
		if (loquet != NULL)
			delete loquet;
		loquet = new LockedThread;
		vuecourante->GetDocument()->SetModifiedFlag(FALSE);
	}

	char* res = displaykifsession();
	if (res != NULL && res[0] != 0) {
		fromaddstring(deb->vsortie, res);
		deb->voutput = deb->vsortie;
		if (firstview != NULL)
			firstview->SetWindowText(deb->vsortie);
		kifcleansession();
	}

	CString currentctext = vuecourante->GetLineText(ln - 1);
	wstring wcurrent = WCSTR(currentctext);
	vuecourante->bnfxs.init(wcurrent, true);
	vuecourante->bnfxs.loadtoken();


	deb->threadid = KifCurrentThreadId();
	deb->currentview = vuecourante;
	deb->lignecourante = ln;
	deb->Lapile(pile);
	deb->Variables(vrs);
	string s;
	GetVariables(s);
	deb->choixvariable = STR(s);

	bool added = false;
	int pos = -1;
	string skeyword;

	int i;
	for (i = 0; i < deb->previousvariables.size(); i++) {
		if (pos == -1)
			pos = kifaddselection(deb->previousvariables[i]);
		else
			kifaddselection(deb->previousvariables[i]);
		added = true;
	}

	deb->previousvariables.clear();
	for (i = 0; i < vuecourante->bnfxs.wstack.size(); i++) {
		skeyword = "";
		s_unicode_to_utf8(skeyword, (wchar_t*)vuecourante->bnfxs.wstack[i].c_str());
		if (pos == -1)
			pos = kifaddselection(skeyword);
		else
			kifaddselection(skeyword);
		deb->previousvariables.push_back(skeyword);
		added = true;
	}

	if (added) {
		deb->localvariables.clear();
		ReinitVariables(deb->localvariables);
		deb->Locals();
		kifremove(pos);
	}

	if (!actif) {
		//DWORD tid;
		//CreateThread(NULL,0,&debuggage,NULL,0,&tid);			
		_beginthread(debuggage, 0, (void*)&lexchange);
	}
	else
		PostMessage(deb->GetSafeHwnd(), MET_A_JOUR, (WPARAM)vuecourante->GetSafeHwnd(), NULL);

	activecursor = 1;
	loquet->Blocked();
}

void CKIFFView::OnKifPointarret()
{
	// TODO: Add your command handler code here

	if (firstview != this) {
		CRichEditCtrl& e = GetRichEditCtrl();
		
		bool fnd = false;
		for (int i = 0; i < thebreaks.size(); i++) {
			if (thebreaks[i] == this) {
				fnd = true;
				break;
			}
		}
		if (!fnd)
			thebreaks.push_back(this);

		long d, f;
		TCHAR c = 2;
		CString sub;
		e.GetSel(d, f);
		long l = e.LineFromChar(d);
		d = e.LineIndex(l);
		f = e.LineIndex(l + 1);
		e.SetSel(d, f);
		sub=e.GetSelText();
		if (sub.Find(c) != -1) {
			sub.Remove(c);
			e.ReplaceSel(sub);
			e.SetSel(d, f - 1);
			Italique(false);
		}
		else {
			e.SetSel(d, d);
			sub = c;
			e.ReplaceSel(sub);
			e.SetSel(d, f - 1);
			Italique(true);
		}
		e.SetSel(d, d);
	}
}

void CKIFFView::Pointarret(long l) {

	bool fnd = false;
	for (int i = 0; i < thebreaks.size(); i++) {
		if (thebreaks[i] == this) {
			fnd = true;
			break;
		}
	}
	if (!fnd)
		thebreaks.push_back(this);

	CRichEditCtrl& e = GetRichEditCtrl();
	TCHAR c = 2;
	long d = e.LineIndex(l);
	long f = e.LineIndex(l + 1);
	e.SetSel(d, f);
	CString sub = e.GetSelText();
	if (sub.Find(c) != -1) {
		sub.Remove(c);
		e.ReplaceSel(sub);
		e.SetSel(d, f - 1);
		Italique(false);
	}
	else {
		e.SetSel(d, d);
		sub = c;
		e.ReplaceSel(sub);
		e.SetSel(d, f - 1);
		Italique(true);
	}
	e.SetSel(d, d);
}

void CKIFFView::OnKifNettoyerp()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long d, f;
	e.GetSel(d, f);
	TCHAR c = 2;
	CString txt;
	e.GetWindowText(txt);
	if (txt.Find(c)) {
		txt.Remove(c);
		e.SetWindowText(txt);
		Colorie();
		nettoyerpointarrets();
	}
	thebreaks.clear();
	e.SetSel(d, f);
}

void CKIFFView::OnKifIndenter()
{
	// TODO: Add your command handler code here
	if (firstview != this) {
		CRichEditCtrl& e = GetRichEditCtrl();
		int nFirstVisible = e.GetFirstVisibleLine();
		long d, f;
		e.GetSel(d, f);
		CString s;
		CString sinit;
		string blanc;
		if (d != f)
			e.SetSel(d, d);

		GetWindowText(sinit);

		vector<long> breaks;
		int i;
		TCHAR c = 2;
		for (i = 0; i < thebreaks.size(); i++) {
			if (thebreaks[i] == this) {
				s = sinit;
				s.Replace(L"\r\n", L"\n");
				long pos = 0;				
				while (pos != -1) {
					pos = s.Find(c, pos);
					if (pos != -1) {
						breaks.push_back(e.LineFromChar(pos));
						pos++;
					}
				}
				sinit.Remove(c);
				break;
			}
		}
		
		string ti;
		fromCString(ti, sinit);
		char* res = (char*)indentation(STR(ti), blanc);
		fromstring(s, res);
		cleancodeindente();

		SetRedraw(FALSE);

		if (s != sinit) {
			Addundos(sinit, 0, sinit.GetLength());
			SetWindowText(s);
			if (breaks.size()) {
				sinit = c;
				for (i = breaks.size() - 1; i >= 0; i--) {
					long d = e.LineIndex(breaks[i]);
					e.SetSel(d, d);
					e.ReplaceSel(sinit);
				}
			}

			Colorie(d, f, nFirstVisible);
			Addundos(s, 0, s.GetLength());
		}

		e.SetSel(f, f);
		SetRedraw(TRUE);
		GetDocument()->SetModifiedFlag(TRUE);
	}
}


void CKIFFView::OnEditionCherche()
{
	// TODO: Add your command handler code here
	m_pFindDialog = new CFindReplaceDialog();
	CString s(findString);
	findMode = true;
	m_pFindDialog->Create(findMode, s, NULL, FR_DOWN, this);
}



void CKIFFView::OnEditionRemplace()
{
	// TODO: Add your command handler code here
	m_pFindDialog = new CFindReplaceDialog();
	CString s(findString);
	CString sr(replacementString);
	findMode = false;
	m_pFindDialog->Create(findMode, s, sr, FR_DOWN, this);
}


void stoppingkif(void* arg) {
	arretprg();
	_endthread();
}

void CKIFFView::OnKifBreak()
{
	// TODO: Add your command handler code here
	_beginthread(stoppingkif, 0, 0);
	CString message("Program killed!!!");
	AfxMessageBox(message);
}


void CKIFFView::OnCommenter()
{
	if (firstview == this)
		return;

	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long d, f;
	e.GetSel(d, f);
	commande = e.GetSelText();
	CString res;
	bool blanc = true;
	char uncommente = 0;
	for (int i = 0; i < commande.GetLength(); i++) {
		if (commande[i] == '\n' || commande[i] == '\r') {
			blanc = true;
			res += commande[i];
		}
		else {
			if (blanc) {
				if (commande[i] <= 32) {
					res += commande[i];
					continue;
				}
				if (commande[i] == '/' && commande[i + 1] == '/') {
					if (uncommente != 1) {
						blanc = false;
						uncommente = 2;
						i++;
						continue;
					}
					else
						res += "//";
				}
				else
				if (uncommente != 2) {
					uncommente = 1;
					res += "//";
				}
			}
			blanc = false;
			res += commande[i];
		}
	}

	string ti;
	fromCString(ti, res);
	char* ires = (char*)indentation(STR(ti), "");
	fromstring(res, ires);
	cleancodeindente();
	e.ReplaceSel(res, 1);
	e.SetSel(f, f);
	Addundos(commande, d, f);
	Addundos(res, d, d + res.GetLength());
	Colorie(-1, -1, -1);
	GetDocument()->SetModifiedFlag(TRUE);
}

wchar_t Accolade(CString& ligne, bool virgpar) {
	int i;
	char trouve = 0;
	int mx = ligne.GetLength();
	wchar_t c;
	bool acc = false;
	for (i = 0; i < mx; i++) {
		c = ligne[i];
		if (c <= 32)
			continue;
		switch (c) {
		case '{':
			acc = true;
			trouve = '{';
			break;
		case '}':
			if (acc)
				return 0;
			return c;
		case ',':
			if (virgpar) {
				if (acc)
					trouve = '(';
				else
					trouve = ',';
			}
			break;
		case ')':
			if (virgpar)
				trouve = ')';
			break;
		case '"':
			if (trouve != '{')
				trouve = 0;
			i++;
			while (i < mx && ligne[i] != '"') {
				if (ligne[i] == '\\')
					i++;
				i++;
			}
			break;
		case '\'':
			if (trouve != '{')
				trouve = 0;
			i++;
			while (i < mx && ligne[i] != '\'')
				i++;
			break;
		case ';':
			if (trouve != '{')
				trouve = 0;
			break;
		case '/':
			if (i < mx - 1 && ligne[i + 1] == '/')
				return trouve;
		default:
			if (trouve != '{')
				trouve = 0;
		}
	}
	if (acc && !trouve)
		return '{';
	return trouve;
}

long CKIFFView::FindCorrespondance(long d, long f, int tofind) {
	CRichEditCtrl& e = GetRichEditCtrl();
	TCHAR sx[1024];
	int l, ln;
	l = e.LineFromChar(f);
	int inc = 0;
	int fin = -1;
	wchar_t finchar;

	CString s;
	int compte = 0;
	if (tofind == 2) {
		compte = 1;
		ln = e.GetLine(l, sx, 512);
		sx[ln] = 0;
		s = sx;
		finchar = Accolade(s, false);
		if (finchar == '{') {
			inc = 1;
			long length = e.GetTextLengthEx(GTL_NUMBYTES, 1200);
			fin = e.LineFromChar(length - 1);
		}
		else
		if (finchar == '}')
			inc = -1;
		else
			return -1;
	}
	else {
		inc = -1;
		finchar = '}';
		compte = tofind;
	}
	l += inc;

	wchar_t c;
	while (l != fin) {
		ln = e.GetLine(l, sx, 512);
		sx[ln] = 0;
		s = sx;
		s = s.Trim();
		if (s.GetLength()) {
			c = Accolade(s, false);
			if (c) {
				if (finchar == c)
					compte++;
				else
					compte--;
			}
		}
		if (!compte)
			break;
		l += inc;
	}

	if (!compte)
		return l;
	return -1;
}


long CKIFFView::FirstChar(long d, long f, CString& commande) {
	CRichEditCtrl& e = GetRichEditCtrl();
	TCHAR sx[1024];
	int l, ln;
	l = e.LineFromChar(f);
	int fin = -1;
	l--;
	vector<string> lines;
	vector<int> blancs;
	CString s;

	while (l != -1) {
		ln = e.GetLine(l, sx, 512);
		sx[ln] = 0;
		s = sx;
		s = s.Trim();
		lines.insert(lines.begin(), "");
		s_unicode_to_utf8(lines.front(), WCSTR(s));
		if (sx[0] > 32)
			break;
		l--;
	}

	if (lines.size() == 0)
		return 0;

	lines.push_back("@;");
	string codeindente;
	IndentationCode(codeindente, lines, blancs, 0, false, "else", "elif", "if");
	return blancs.back();
}

long CKIFFView::FirstEnd(long d, long f) {
	CRichEditCtrl& e = GetRichEditCtrl();
	TCHAR sx[1024];
	int l, ln;
	l = e.LineFromChar(f);
	int fin = -1;
	CString s;

	l -= 2;
	int saute = 0;
	int compte = 0;
	while (l != -1) {
		ln = e.GetLine(l, sx, 512);
		sx[ln] = 0;
		s = sx;
		s = s.Trim();
		if (s.GetLength()) {
			wchar_t c = Accolade(s, false);
			if (c == '}')
				compte++;
			else
			if (c == '{')
				compte--;
			if (!compte) {
				if (Teststring(s, wkifelse, L'{')) {
					l--;
					saute++;
					continue;
				}
				if (Teststring(s, wkifif, L'(') || Teststring(s, wkifelif, L'(')) {
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


void CKIFFView::OnKifCorrespondance()
{
	// TODO: Add your command handler code here
	long d, f;
	//fermertab();
	CRichEditCtrl& e = GetRichEditCtrl();
	e.GetSel(d, f);

	long l = FindCorrespondance(d, f, 2);
	if (l != -1)
		SelectionneLigne(++l);
}


void CKIFFView::OnEditionSuivant()
{
	// TODO: Add your command handler code here
	if (findString == "")
		return;
	if (FindText(findString, bMatchCase, bMatchWholeWord, bSearchDown)) {
		if (findMode == false)
			GetRichEditCtrl().ReplaceSel(replacementString, 1);
	}
}


void CKIFFView::OnKifArguments()
{
	// TODO: Add your command handler code here
	kifarguments argums;
	argums.args = basearguments;
	CString ind("Please separate each argument with a space");
	argums.indication = ind;
	if (argums.DoModal() == IDOK)
		basearguments = argums.args;
}


void CKIFFView::OnEditionAllerligne()
{
	// TODO: Add your command handler code here
	kifarguments argums;
	long l;
	CString ind("Line number");
	argums.indication = ind;
	if (argums.DoModal() == IDOK) {
		l = wcstol(LPCWSTR(argums.args), NULL, 10);
		SelectionneLigne(l);
	}
}

void CKIFFView::OnEditUndo()
{

	if (listundos.size() <= 1)
		return;

	ASSERT_VALID(this);
	CRichEditCtrl& e = GetRichEditCtrl();
	ModifItem md(listundos.back().s, listundos.back().d, listundos.back().f);
	listundos.pop_back();
	ModifItem& mdp = listundos.back();
	e.HideSelection(TRUE, FALSE);
	e.SetSel(md.d, md.f);
	e.ReplaceSel(mdp.s, 0);
	e.SetSel(mdp.f, mdp.f);

	e.HideSelection(FALSE, FALSE);
	listdos.push_back(md);
	listdos.push_back(mdp);
	listundos.pop_back();
	m_bSyncCharFormat = m_bSyncParaFormat = TRUE;
	if ((mdp.f - mdp.d) > 50)
		Colorie();
}

void CKIFFView::OnEditRedo()
{
	if (listdos.size() <= 1)
		return;

	ASSERT_VALID(this);
	CRichEditCtrl& e = GetRichEditCtrl();
	ModifItem md(listdos.back().s, listdos.back().d, listdos.back().f);
	listdos.pop_back();
	ModifItem& mdp = listdos.back();
	e.HideSelection(TRUE, FALSE);
	e.SetSel(md.d, md.f);
	e.ReplaceSel(mdp.s, 0);
	e.SetSel(mdp.f, mdp.f);
	e.HideSelection(FALSE, FALSE);
	listundos.push_back(md);
	listundos.push_back(mdp);
	listdos.pop_back();
	m_bSyncCharFormat = m_bSyncParaFormat = TRUE;
	if ((mdp.f - mdp.d) > 50)
		Colorie();
}




void CKIFFView::OnUpdateKifDebugxip(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(debugxip);
	pCmdUI->Enable(1 - lexchange.encours);
}


void CKIFFView::OnKifDebugxip()
{
	// TODO: Add your command handler code here
	if (!closekifsession())
		return;
	debugxip = 1;

	CString txt;
	TCHAR c = 2;
	lexchange.debugging = false;
	for (int i = 0; i < thebreaks.size(); i++) {
		CRichEditCtrl& e = thebreaks[i]->GetRichEditCtrl();
		string titre = thebreaks[i]->stitre;
		e.GetWindowText(txt);
		txt.Replace(L"\r\n", L"\n");		
		long pos = 0;
		while (pos != -1) {
			pos = txt.Find(c, pos);
			if (pos != -1) {
				ajoutebreak(STR(titre), e.LineFromChar(pos) + 1);
				lexchange.debugging = true;
				pos++;
			}
		}
	}

	OnKifExecuter();
	debugxip = 0;
}


void CKIFFView::OnUpdateKifReexecute(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 - lexchange.encours);
	if (activecursor == 1)
		BeginWaitCursor();
	else
	if (activecursor == 2) {
		activecursor = 0;
		EndWaitCursor();
	}
}


void CKIFFView::OnUpdateKifPasapas(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 - lexchange.encours);
}


void CKIFFView::OnUpdateKifExecuter(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 - lexchange.encours);
}


void CKIFFView::OnUpdateKifCompiler(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 - lexchange.encours);
}


void CKIFFView::OnUpdateKifBreak(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(lexchange.encours);
}


void CKIFFView::OnEditionAbandon()
{
	// TODO: Add your command handler code here
	nbcurly = 0;
	nbsquare = 0;
	accumulecode = _T("");

}


void CKIFFView::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(listdos.size() != 0);
}


void CKIFFView::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(listundos.size() != 0);
}




void CKIFFView::OnUpdateWxipNobreak(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 - lexchange.encours);
}


void CKIFFView::OnEditCut()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	CString s = e.GetSelText();
	long d, f;
	e.GetSel(d, f);
	if (d && s.Right(1) == 13) {
		CString lcar;
		e.GetTextRange(d - 1, d, lcar);
		if (lcar[0] != 13) //it is not the case, we need to remove the trailing CR from the string
			e.GetTextRange(d, --f, s);
	}
	Addundos(s, d, f);
	CRichEditView::OnEditCut();
	Addundos(_T(""), d, d);
}



void CKIFFView::OnEditPaste()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	CString s = e.GetSelText();
	long d, f;
	e.GetSel(d, f);
	if (d && s.Right(1) == 13) {
		CString lcar;
		e.GetTextRange(d - 1, d, lcar);
		if (lcar[0] != 13) //it is not the case, we need to remove the trailing CR from the string
			e.GetTextRange(d, --f, s);
	}
	Addundos(s, d, f);
	CRichEditView::OnEditPaste();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(d, fp);
	s = e.GetSelText();
	Addundos(s, d, fp);
	e.SetSel(dp, fp);
}


void CKIFFView::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1);
}



void CKIFFView::OnEditionSuggest()
{
	// TODO: Add your command handler code here
	suggestion = 1 - suggestion;
}



void CKIFFView::OnUpdateEditionSuggest(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(suggestion);
}

void CKIFFView::OnEditSetblanks()
{
	// TODO: Add your command handler code here
	kifarguments argums;
	int l;
	CString ind("Blank size");
	wstring val = GetKeyVal(L"SZBLANKS");
	argums.args = val.c_str();
	argums.indication = ind;
	if (argums.DoModal() == IDOK) {
		l = wcstol(LPCWSTR(argums.args), NULL, 10);
		SetBlankSize(l);
		SetKeyVal(L"SZBLANKS", argums.args);
	}
}
