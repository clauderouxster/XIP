/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the XIP library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : XIP
Version    : 10.00
filename   : kifltk.cxx
Date       : 09/09/2010
Purpose    : FLTK KiF implementation
Programmer : Claude ROUX
Reviewer   :
*/

//Use KIF_DLL only in Windows, if you link to the KiF DLL
//#define KIF_DLL
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Menu.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_GIF_Image.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Progress.H>

#ifndef FLTKNOJPEG
#include <FL/Fl_JPEG_Image.H>
#endif

#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"
#include "kifmaps.h"

#ifdef WIN32
#define Exporting __declspec(dllexport)

//The two following functions were added to the FLTK library to solve two issues
//First issue: the timer window in WINDOWS was not reset after a relaunch (see Fl_win32.cxx)

//Code added to Fl_win32.cxx
//void FlResetTimer() {
//	if (s_TimerWnd)
//		s_TimerWnd = 0;
//}

void FlResetTimer();

//-------------------------------------------------------------------------

//We launch FLTK in a thread, but in that case the mainthread variable (see Fl_lock.cxx) is no longer set to the proper value
//This function is used to do that...

//Code added to Fl_lock.cxx
//void Flresetmainthread() {
//	main_thread = GetCurrentThreadId();
//}


void Flresetmainthread();
#endif


#ifndef WIN32
#define strcpy_s(a,b,c) strncpy(a,c,b)
#endif

#ifdef MACUI
extern "C" {
	const char* lirefichiermac();
	bool chargefichiermac(const char*);
}
#endif 

//Replace window by your own definition
short KifNewId(string n);
string KifIdString(short n);
extern KifGlobalThread* kifGlobals;

typedef bool(*PauseBack)(void*);
bool KifWaitingFor(double res, PauseBack pb, void* data);
static bool stopall = false;

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType window_type;
static KifType box_type;
static KifType button_type;
static KifType counter_type;
static KifType input_type;
static KifType progress_type;
static KifType table_type;
static KifType output_type;
static KifType editor_type;
static KifType scroll_type;
static KifType slider_type;
static KifType choice_type;
static KifType filebrowser_type;
static KifType image_type;
static KifType bitmap_type;
static KifType tabs_type;
static KifType group_type;
static KifType browser_type;
static KifType wtree_type;
static KifType etree_type;

static int fontmax = 16;
class KifWidget;

void PostWindowCall(int threadid, KifWidget* w, KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc);

#define inthreadcreate(c,d,cl) int idcallthread=(calledthread==0)?_thread:calledthread;\
if (mainwindow == NULL)stopall = false; \
else if (stopall) return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]); \
if (_thread) { PostWindowCall(_thread, this, c, d, cl); return kifNULL; }

#define inthread(c,d,cl) int idcallthread=(calledthread==0)?_thread:calledthread;\
if (stopall) return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]); \
if (_thread) { PostWindowCall(_thread, this, c, d, cl); return kifNULL; }

#define minthread int idcallthread=(calledthread==0)?_thread:calledthread;\
if (stopall) return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);
#define threadtosetvalue(c,d,cl) if (_thread) {PostWindowCall(_thread,this,c,d,cl); return kifNULL;}


KifElement* Returningerror(KifCode* kifcode, int id, int idcall, string mess) {
	if (id != idcall)
		return kifNULL;
	return kifcode->Returnerror(id, mess);
}


//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifwindow;
class Kifbox;
class Kiftabs;
class Kifgroup;
class Kifbutton;
class Kifinput;
class Kiftable;
class Kifoutput;
class Kifeditor;
class Kifscroll;
class Kifslider;
class Kifchoice;
class Kifimage;
class Kifbitmap;
class Kiffilebrowser;
class Kifbrowser;
class Kifwtree;
class Kifetree;
class Kifcounter;
class Kifprogress;

//This typedef defines a type "windowMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifwindow::*windowMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifbox::*boxMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifbutton::*buttonMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifinput::*inputMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kiftable::*tableMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifoutput::*outputMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifeditor::*editorMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifscroll::*scrollMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifslider::*sliderMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifchoice::*choiceMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifimage::*imageMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifbitmap::*bitmapMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kiffilebrowser::*filebrowserMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kiftabs::*tabsMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifgroup::*groupMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifbrowser::*browserMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifetree::*etreeMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifwtree::*wtreeMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifcounter::*counterMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);
typedef KifElement* (Kifprogress::*progressMethod)(KifElement*, KifDomain*, int _thread, KifCallFunction*);


static vector<Kifwindow*> current_window;
static Kifwindow* mainwindow;

static map<int, string> ifonts;

static map<string, unsigned int> boxTypes;
static map<string, unsigned int> fonts;
static map<string, unsigned int> buttonShapes;
static map<string, unsigned int> buttonAction;
static map<string, unsigned int> keyCodes;
static map<string, Fl_Color> colors;
static map<string, unsigned int> aligns;
static map<string, unsigned int> lineshape;
static map<string, unsigned int> slidertype;
static map<string, unsigned int> actions;
static map<string, unsigned int> labeltypes;
static map<string, unsigned int> buttontypes;
static map<string, unsigned int> cursors;
static map<string, unsigned long> commands;


//----------------------------------------------------------------------------------------------------------
//We implement this function to provide a simple input Window to replace the "get" in a window environment
KifElement* PreGetEditor(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->isfile()) {
		if (callfunc->Size() != 1)
			return callfunc->kifcode->Returnerror(KIFLTKSTRINGS[1]);

		KifBaseFile* kfile = (KifBaseFile*)kfirst;
		if (kfile->EndOfFile() == kifTRUE)
			return kifMINUSONE;
		long c = kfile->Get();
		if (pattern->type == kifInteger || pattern->type == kifFloat)
			return callfunc->kifcode->Provideinteger(c);

		return new KifByte(callfunc->kifcode, NULL, (unsigned char)c);
	}

	//In this case, we create a window on the fly to read the value
	string txt;
	if (callfunc->Size() == 2)
		txt = callfunc->Evaluatethread(1, dom, idthread)->String();
	else {
		txt = KIFLTKSTRINGS[2];
		txt += " (";
		txt += kfirst->Type();
		txt += "):";
	}

	const char* buff = fl_input(txt.c_str());

	if (buff == NULL) {
		kfirst->Setvalue(kifNULL, NULL, dom);
		return kifFALSE;
	}

	KifElement* ke = callfunc->kifcode->Providestringraw(buff);
	kfirst->Setvalue(ke, NULL, dom);
	ke->Release();

	return kifTRUE;
}

void ResetGetEditor(KifCode* local) {
	//We replace the current function with a new one, to handle get through a window...
	int idname = KifNewId(KIFLTKSTRINGS[2]);
	if (kifGlobals->kifBaseFunctions.find(idname) != kifGlobals->kifBaseFunctions.end())
		delete kifGlobals->kifBaseFunctions[idname];

	kifGlobals->kifBaseFunctions[idname] = new KifPredefined(NULL, P_ONE | P_TWO, &PreGetEditor, idname);
	kifGlobals->kifBaseFunctions[idname]->kifcode = local;
}
//----------------------------------------------------------------------------------------------------------


static void initialisationtype() {
	static bool init = false;
	if (init == false) {
		colors[KIFLTKSTRINGS[3]] = FL_FOREGROUND_COLOR;
		colors[KIFLTKSTRINGS[4]] = FL_BACKGROUND2_COLOR;
		colors[KIFLTKSTRINGS[5]] = FL_BACKGROUND_COLOR;
		colors[KIFLTKSTRINGS[6]] = FL_INACTIVE_COLOR;
		colors[KIFLTKSTRINGS[7]] = FL_SELECTION_COLOR;
		colors[KIFLTKSTRINGS[8]] = FL_GRAY0;
		colors[KIFLTKSTRINGS[9]] = FL_DARK3;
		colors[KIFLTKSTRINGS[10]] = FL_DARK2;
		colors[KIFLTKSTRINGS[11]] = FL_DARK1;
		colors[KIFLTKSTRINGS[12]] = FL_LIGHT1;
		colors[KIFLTKSTRINGS[13]] = FL_LIGHT2;
		colors[KIFLTKSTRINGS[14]] = FL_LIGHT3;
		colors[KIFLTKSTRINGS[15]] = FL_BLACK;
		colors[KIFLTKSTRINGS[16]] = FL_RED;
		colors[KIFLTKSTRINGS[17]] = FL_GREEN;
		colors[KIFLTKSTRINGS[18]] = FL_YELLOW;
		colors[KIFLTKSTRINGS[19]] = FL_BLUE;
		colors[KIFLTKSTRINGS[20]] = FL_MAGENTA;
		colors[KIFLTKSTRINGS[21]] = FL_CYAN;
		colors[KIFLTKSTRINGS[22]] = FL_DARK_RED;
		colors[KIFLTKSTRINGS[23]] = FL_DARK_GREEN;
		colors[KIFLTKSTRINGS[24]] = FL_DARK_YELLOW;
		colors[KIFLTKSTRINGS[25]] = FL_DARK_BLUE;
		colors[KIFLTKSTRINGS[26]] = FL_DARK_MAGENTA;
		colors[KIFLTKSTRINGS[27]] = FL_DARK_CYAN;
		colors[KIFLTKSTRINGS[28]] = FL_WHITE;

		buttontypes[KIFLTKSTRINGS[29]] = 0;
		buttontypes[KIFLTKSTRINGS[30]] = 1;
		buttontypes[KIFLTKSTRINGS[31]] = 2;
		buttontypes[KIFLTKSTRINGS[32]] = 3;
		buttontypes[KIFLTKSTRINGS[33]] = 4;
		buttontypes[KIFLTKSTRINGS[34]] = 5;
		buttontypes[KIFLTKSTRINGS[35]] = 6;


		actions[KIFLTKSTRINGS[36]] = FL_PUSH;
		actions[KIFLTKSTRINGS[37]] = FL_RELEASE;
		actions[KIFLTKSTRINGS[38]] = FL_DRAG;
		actions[KIFLTKSTRINGS[39]] = FL_MOVE;
		actions[KIFLTKSTRINGS[40]] = FL_MOUSEWHEEL;
		actions[KIFLTKSTRINGS[41]] = FL_KEYDOWN;
		actions[KIFLTKSTRINGS[42]] = FL_KEYUP;

		slidertype[KIFLTKSTRINGS[43]] = FL_VERT_SLIDER;
		slidertype[KIFLTKSTRINGS[44]] = FL_HOR_SLIDER;
		slidertype[KIFLTKSTRINGS[45]] = FL_VERT_FILL_SLIDER;
		slidertype[KIFLTKSTRINGS[46]] = FL_HOR_FILL_SLIDER;
		slidertype[KIFLTKSTRINGS[47]] = FL_VERT_NICE_SLIDER;
		slidertype[KIFLTKSTRINGS[48]] = FL_HOR_NICE_SLIDER;

		lineshape[KIFLTKSTRINGS[49]] = FL_SOLID;
		lineshape[KIFLTKSTRINGS[50]] = FL_DASH;
		lineshape[KIFLTKSTRINGS[51]] = FL_DOT;
		lineshape[KIFLTKSTRINGS[52]] = FL_DASHDOT;
		lineshape[KIFLTKSTRINGS[53]] = FL_DASHDOTDOT;
		lineshape[KIFLTKSTRINGS[54]] = FL_CAP_FLAT;
		lineshape[KIFLTKSTRINGS[55]] = FL_CAP_ROUND;
		lineshape[KIFLTKSTRINGS[56]] = FL_CAP_SQUARE;
		lineshape[KIFLTKSTRINGS[57]] = FL_JOIN_MITER;
		lineshape[KIFLTKSTRINGS[58]] = FL_JOIN_ROUND;
		lineshape[KIFLTKSTRINGS[59]] = FL_JOIN_BEVEL;

		aligns[KIFLTKSTRINGS[60]] = FL_ALIGN_CENTER;
		aligns[KIFLTKSTRINGS[61]] = FL_ALIGN_TOP;
		aligns[KIFLTKSTRINGS[62]] = FL_ALIGN_BOTTOM;
		aligns[KIFLTKSTRINGS[63]] = FL_ALIGN_LEFT;
		aligns[KIFLTKSTRINGS[64]] = FL_ALIGN_RIGHT;
		aligns[KIFLTKSTRINGS[65]] = FL_ALIGN_INSIDE;
		aligns[KIFLTKSTRINGS[66]] = FL_ALIGN_TEXT_OVER_IMAGE;
		aligns[KIFLTKSTRINGS[67]] = FL_ALIGN_IMAGE_OVER_TEXT;
		aligns[KIFLTKSTRINGS[68]] = FL_ALIGN_CLIP;
		aligns[KIFLTKSTRINGS[69]] = FL_ALIGN_WRAP;
		aligns[KIFLTKSTRINGS[70]] = FL_ALIGN_IMAGE_NEXT_TO_TEXT;
		aligns[KIFLTKSTRINGS[71]] = FL_ALIGN_TEXT_NEXT_TO_IMAGE;
		aligns[KIFLTKSTRINGS[72]] = FL_ALIGN_IMAGE_BACKDROP;
		aligns[KIFLTKSTRINGS[73]] = FL_ALIGN_TOP_LEFT;
		aligns[KIFLTKSTRINGS[74]] = FL_ALIGN_TOP_RIGHT;
		aligns[KIFLTKSTRINGS[75]] = FL_ALIGN_BOTTOM_LEFT;
		aligns[KIFLTKSTRINGS[76]] = FL_ALIGN_BOTTOM_RIGHT;
		aligns[KIFLTKSTRINGS[77]] = FL_ALIGN_LEFT_TOP;
		aligns[KIFLTKSTRINGS[78]] = FL_ALIGN_RIGHT_TOP;
		aligns[KIFLTKSTRINGS[79]] = FL_ALIGN_LEFT_BOTTOM;
		aligns[KIFLTKSTRINGS[80]] = FL_ALIGN_RIGHT_BOTTOM;
		aligns[KIFLTKSTRINGS[81]] = FL_ALIGN_NOWRAP;
		aligns[KIFLTKSTRINGS[82]] = FL_ALIGN_POSITION_MASK;
		aligns[KIFLTKSTRINGS[83]] = FL_ALIGN_IMAGE_MASK;

		labeltypes[KIFLTKSTRINGS[84]] = FL_NORMAL_LABEL;
		labeltypes[KIFLTKSTRINGS[85]] = FL_NO_LABEL;
		labeltypes[KIFLTKSTRINGS[86]] = _FL_SHADOW_LABEL;
		labeltypes[KIFLTKSTRINGS[87]] = _FL_ENGRAVED_LABEL;
		labeltypes[KIFLTKSTRINGS[88]] = _FL_EMBOSSED_LABEL;

		cursors[KIFLTKSTRINGS[89]] = FL_CURSOR_DEFAULT;
		cursors[KIFLTKSTRINGS[90]] = FL_CURSOR_ARROW;
		cursors[KIFLTKSTRINGS[91]] = FL_CURSOR_CROSS;
		cursors[KIFLTKSTRINGS[92]] = FL_CURSOR_WAIT;
		cursors[KIFLTKSTRINGS[93]] = FL_CURSOR_INSERT;
		cursors[KIFLTKSTRINGS[94]] = FL_CURSOR_HAND;
		cursors[KIFLTKSTRINGS[95]] = FL_CURSOR_HELP;
		cursors[KIFLTKSTRINGS[96]] = FL_CURSOR_MOVE;
		cursors[KIFLTKSTRINGS[97]] = FL_CURSOR_NS;
		cursors[KIFLTKSTRINGS[98]] = FL_CURSOR_WE;
		cursors[KIFLTKSTRINGS[99]] = FL_CURSOR_NWSE;
		cursors[KIFLTKSTRINGS[100]] = FL_CURSOR_NESW;
		cursors[KIFLTKSTRINGS[101]] = FL_CURSOR_NONE;
		cursors[KIFLTKSTRINGS[102]] = FL_CURSOR_N;
		cursors[KIFLTKSTRINGS[103]] = FL_CURSOR_NE;
		cursors[KIFLTKSTRINGS[104]] = FL_CURSOR_E;
		cursors[KIFLTKSTRINGS[105]] = FL_CURSOR_SE;
		cursors[KIFLTKSTRINGS[106]] = FL_CURSOR_S;
		cursors[KIFLTKSTRINGS[107]] = FL_CURSOR_SW;
		cursors[KIFLTKSTRINGS[108]] = FL_CURSOR_W;
		cursors[KIFLTKSTRINGS[109]] = FL_CURSOR_NW;

		cursors[KIFLTKSTRINGS[110]] = Fl_Text_Display::NORMAL_CURSOR;
		cursors[KIFLTKSTRINGS[111]] = Fl_Text_Display::CARET_CURSOR;
		cursors[KIFLTKSTRINGS[112]] = Fl_Text_Display::DIM_CURSOR;
		cursors[KIFLTKSTRINGS[113]] = Fl_Text_Display::BLOCK_CURSOR;
		cursors[KIFLTKSTRINGS[114]] = Fl_Text_Display::HEAVY_CURSOR;
		cursors[KIFLTKSTRINGS[115]] = 5;

		commands[KIFLTKSTRINGS[116]] = FL_SHIFT;
		commands[KIFLTKSTRINGS[117]] = FL_CAPS_LOCK;
		commands["CTRL"] = FL_CTRL;
		commands["ALT"] = FL_ALT;
		commands[KIFLTKSTRINGS[118]] = FL_NUM_LOCK;
		commands["META"] = FL_META;
		commands[KIFLTKSTRINGS[119]] = FL_SCROLL_LOCK;
		commands[KIFLTKSTRINGS[120]] = FL_BUTTON1;
		commands[KIFLTKSTRINGS[121]] = FL_BUTTON2;
		commands[KIFLTKSTRINGS[122]] = FL_BUTTON3;
		commands[KIFLTKSTRINGS[123]] = FL_BUTTONS;
		commands[KIFLTKSTRINGS[124]] = FL_KEY_MASK;
		commands[KIFLTKSTRINGS[125]] = FL_COMMAND;
		commands[KIFLTKSTRINGS[126]] = FL_CONTROL;

		keyCodes[KIFLTKSTRINGS[127]] = 0xfee8;
		keyCodes[KIFLTKSTRINGS[128]] = 0xff08;
		keyCodes["Tab"] = 0xff09;
		keyCodes[KIFLTKSTRINGS[129]] = 0xff0d;
		keyCodes[KIFLTKSTRINGS[130]] = 0xff13;
		keyCodes[KIFLTKSTRINGS[131]] = 0xff14;
		keyCodes[KIFLTKSTRINGS[132]] = 0xff1b;
		keyCodes[KIFLTKSTRINGS[133]] = 0xff50;
		keyCodes[KIFLTKSTRINGS[134]] = 0xff51;
		keyCodes[KIFLTKSTRINGS[135]] = 0xff52;
		keyCodes[KIFLTKSTRINGS[136]] = 0xff53;
		keyCodes[KIFLTKSTRINGS[137]] = 0xff54;
		keyCodes[KIFLTKSTRINGS[138]] = 0xff55;
		keyCodes[KIFLTKSTRINGS[139]] = 0xff56;
		keyCodes[KIFLTKSTRINGS[140]] = 0xff57;
		keyCodes[KIFLTKSTRINGS[141]] = 0xff61;
		keyCodes[KIFLTKSTRINGS[142]] = 0xff63;
		keyCodes[KIFLTKSTRINGS[143]] = 0xff67;
		keyCodes[KIFLTKSTRINGS[144]] = 0xff68;
		keyCodes[KIFLTKSTRINGS[145]] = 0xff7f;
		keyCodes["KP"] = 0xff80;
		keyCodes[KIFLTKSTRINGS[146]] = 0xff8d;
		keyCodes[KIFLTKSTRINGS[147]] = 0xffbd;
		keyCodes["F"] = 0xffbd;
		keyCodes[KIFLTKSTRINGS[148]] = 0xffe0;
		keyCodes[KIFLTKSTRINGS[149]] = 0xffe1;
		keyCodes[KIFLTKSTRINGS[150]] = 0xffe2;
		keyCodes[KIFLTKSTRINGS[151]] = 0xffe3;
		keyCodes[KIFLTKSTRINGS[152]] = 0xffe4;
		keyCodes[KIFLTKSTRINGS[153]] = 0xffe5;
		keyCodes[KIFLTKSTRINGS[154]] = 0xffe7;
		keyCodes[KIFLTKSTRINGS[155]] = 0xffe8;
		keyCodes[KIFLTKSTRINGS[156]] = 0xffe9;
		keyCodes[KIFLTKSTRINGS[157]] = 0xffea;
		keyCodes[KIFLTKSTRINGS[158]] = 0xffff;

		boxTypes[KIFLTKSTRINGS[159]] = 0;
		boxTypes[KIFLTKSTRINGS[160]] = 1;
		boxTypes[KIFLTKSTRINGS[161]] = 2;
		boxTypes[KIFLTKSTRINGS[162]] = 3;
		boxTypes[KIFLTKSTRINGS[163]] = 4;
		boxTypes[KIFLTKSTRINGS[164]] = 5;
		boxTypes[KIFLTKSTRINGS[165]] = 6;
		boxTypes[KIFLTKSTRINGS[166]] = 7;
		boxTypes[KIFLTKSTRINGS[167]] = 8;
		boxTypes[KIFLTKSTRINGS[168]] = 9;
		boxTypes[KIFLTKSTRINGS[169]] = 10;
		boxTypes[KIFLTKSTRINGS[170]] = 11;
		boxTypes[KIFLTKSTRINGS[171]] = 12;
		boxTypes[KIFLTKSTRINGS[172]] = 13;
		boxTypes[KIFLTKSTRINGS[173]] = 14;
		boxTypes[KIFLTKSTRINGS[174]] = 15;
		boxTypes[KIFLTKSTRINGS[175]] = 16;
		boxTypes[KIFLTKSTRINGS[176]] = 17;
		boxTypes[KIFLTKSTRINGS[177]] = 18;
		boxTypes[KIFLTKSTRINGS[178]] = 19;
		boxTypes[KIFLTKSTRINGS[179]] = 20;
		boxTypes[KIFLTKSTRINGS[180]] = 21;
		boxTypes[KIFLTKSTRINGS[181]] = 22;
		boxTypes[KIFLTKSTRINGS[182]] = 23;
		boxTypes[KIFLTKSTRINGS[183]] = 24;
		boxTypes[KIFLTKSTRINGS[184]] = 25;
		boxTypes[KIFLTKSTRINGS[185]] = 26;
		boxTypes[KIFLTKSTRINGS[186]] = 27;
		boxTypes[KIFLTKSTRINGS[187]] = 28;
		boxTypes[KIFLTKSTRINGS[188]] = 29;
		boxTypes[KIFLTKSTRINGS[189]] = 30;
		boxTypes[KIFLTKSTRINGS[190]] = 31;
		boxTypes[KIFLTKSTRINGS[191]] = 32;
		boxTypes[KIFLTKSTRINGS[192]] = 33;
		boxTypes[KIFLTKSTRINGS[193]] = 34;
		boxTypes[KIFLTKSTRINGS[194]] = 35;
		boxTypes[KIFLTKSTRINGS[195]] = 36;
		boxTypes[KIFLTKSTRINGS[196]] = 37;
		boxTypes[KIFLTKSTRINGS[197]] = 38;
		boxTypes[KIFLTKSTRINGS[198]] = 39;
		boxTypes[KIFLTKSTRINGS[199]] = 40;
		boxTypes[KIFLTKSTRINGS[200]] = 41;
		boxTypes[KIFLTKSTRINGS[201]] = 42;
		boxTypes[KIFLTKSTRINGS[202]] = 43;
		boxTypes[KIFLTKSTRINGS[203]] = 44;
		boxTypes[KIFLTKSTRINGS[204]] = 45;
		boxTypes[KIFLTKSTRINGS[205]] = 46;
		boxTypes[KIFLTKSTRINGS[206]] = 47;
		boxTypes[KIFLTKSTRINGS[207]] = 48;

		fonts["HELVETICA"] = 0;
		fonts[KIFLTKSTRINGS[208]] = 1;
		fonts[KIFLTKSTRINGS[209]] = 2;
		fonts[KIFLTKSTRINGS[210]] = 3;
		fonts["COURIER"] = 4;
		fonts[KIFLTKSTRINGS[211]] = 5;
		fonts[KIFLTKSTRINGS[212]] = 6;
		fonts[KIFLTKSTRINGS[213]] = 7;
		fonts["TIMES"] = 8;
		fonts[KIFLTKSTRINGS[214]] = 9;
		fonts[KIFLTKSTRINGS[215]] = 10;
		fonts[KIFLTKSTRINGS[216]] = 11;
		fonts[KIFLTKSTRINGS[217]] = 12;
		fonts[KIFLTKSTRINGS[218]] = 13;
		fonts[KIFLTKSTRINGS[219]] = 14;
		fonts["ZAPF_DINGBATS"] = 15;
		fonts["FREE_FONT"] = 16;
		fonts[KIFLTKSTRINGS[220]] = 1;
		fonts[KIFLTKSTRINGS[221]] = 2;
		fonts[KIFLTKSTRINGS[222]] = 3;

		ifonts[0] = "HELVETICA";
		ifonts[1] = KIFLTKSTRINGS[208];
		ifonts[2] = KIFLTKSTRINGS[209];
		ifonts[3] = KIFLTKSTRINGS[210];
		ifonts[4] = "COURIER";
		ifonts[5] = KIFLTKSTRINGS[211];
		ifonts[6] = KIFLTKSTRINGS[212];
		ifonts[7] = KIFLTKSTRINGS[213];
		ifonts[8] = "TIMES";
		ifonts[9] = KIFLTKSTRINGS[214];
		ifonts[10] = KIFLTKSTRINGS[215];
		ifonts[11] = KIFLTKSTRINGS[216];
		ifonts[12] = KIFLTKSTRINGS[217];
		ifonts[13] = KIFLTKSTRINGS[218];
		ifonts[14] = KIFLTKSTRINGS[219];
		ifonts[15] = "ZAPF_DINGBATS";
		ifonts[16] = "FREE_FONT";

		buttonShapes[KIFLTKSTRINGS[223]] = FL_NORMAL_BUTTON;
		buttonShapes[KIFLTKSTRINGS[224]] = FL_TOGGLE_BUTTON;
		buttonShapes[KIFLTKSTRINGS[225]] = FL_RADIO_BUTTON;
		buttonShapes[KIFLTKSTRINGS[226]] = FL_HIDDEN_BUTTON;

		buttonAction[KIFLTKSTRINGS[227]] = FL_WHEN_NEVER;
		buttonAction[KIFLTKSTRINGS[228]] = FL_WHEN_CHANGED;
		buttonAction[KIFLTKSTRINGS[229]] = FL_WHEN_RELEASE;
		buttonAction[KIFLTKSTRINGS[230]] = FL_WHEN_RELEASE_ALWAYS;
		buttonAction[KIFLTKSTRINGS[231]] = FL_WHEN_ENTER_KEY;
		buttonAction[KIFLTKSTRINGS[232]] = FL_WHEN_ENTER_KEY_ALWAYS;
		init = true;

	}
}

static bool create_variables(KifCode* kifcode) {
	initialisationtype();

	map<string, unsigned int>::iterator it;
	string name;
	KifVariableDeclaration* var;
	//then we create its value
	KifBaseInteger* local;
	string firstname;
	string lastname;

	name = KIFLTKSTRINGS[233];
	firstname = name;
	short idname = KifNewId(name);

	if (kifcode->gGlobals->kconstants->declarations.check(idname))
		return false;

	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_NORMAL_SIZE);

	for (it = boxTypes.begin(); it != boxTypes.end(); it++) {
		name = "FL_";
		name += it->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, it->second);
	}
	for (it = fonts.begin(); it != fonts.end(); it++) {
		name = "FL_";
		name += it->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, it->second);
	}

	for (it = labeltypes.begin(); it != labeltypes.end(); it++) {
		name = "FL_";
		name += it->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, it->second);
	}

	for (it = buttontypes.begin(); it != buttontypes.end(); it++) {
		name = "FL_";
		name += it->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, it->second);
	}

	for (it = buttonShapes.begin(); it != buttonShapes.end(); it++) {
		name = "FL_";
		name += it->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, it->second);
	}
	for (it = buttonAction.begin(); it != buttonAction.end(); it++) {
		name = "FL_";
		name += it->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, it->second);
	}
	for (it = keyCodes.begin(); it != keyCodes.end(); it++) {
		name = "FL_";
		name += it->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, it->second);
	}
	for (it = cursors.begin(); it != cursors.end(); it++) {
		name = "FL_";
		name += it->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, it->second);
	}
	for (it = aligns.begin(); it != aligns.end(); it++) {
		name = "FL_";
		name += it->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, it->second);
	}
	for (it = lineshape.begin(); it != lineshape.end(); it++) {
		name = "FL_";
		name += it->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, it->second);
	}
	for (it = slidertype.begin(); it != slidertype.end(); it++) {
		name = "FL_";
		name += it->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, it->second);
	}
	for (it = actions.begin(); it != actions.end(); it++) {
		name = "FL_";
		name += it->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, it->second);
	}

	map<string, unsigned long>::iterator its;
	for (its = commands.begin(); its != commands.end(); its++) {
		name = "FL_";
		name += its->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, its->second);
	}
	map<string, Fl_Color>::iterator itx;
	for (itx = colors.begin(); itx != colors.end(); itx++) {
		name = "FL_";
		name += itx->first;
		var = kifcode->Createstaticvariable(name);
		local = new KifBaseInteger(kifcode, var, itx->second);
	}

	name = KIFLTKSTRINGS[234];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, 0);

	name = KIFLTKSTRINGS[235];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, 1);

	name = KIFLTKSTRINGS[236];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, 2);

	name = KIFLTKSTRINGS[237];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, 4);

	//Tree flags
	name = KIFLTKSTRINGS[238];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_SELECT_NONE);

	name = KIFLTKSTRINGS[239];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_SELECT_SINGLE);

	name = KIFLTKSTRINGS[240];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_SELECT_MULTI);

	name = KIFLTKSTRINGS[241];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_SORT_NONE);

	name = KIFLTKSTRINGS[242];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_SORT_ASCENDING);

	name = KIFLTKSTRINGS[243];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_SORT_DESCENDING);

	name = KIFLTKSTRINGS[244];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_CONNECTOR_NONE);

	name = KIFLTKSTRINGS[245];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_CONNECTOR_DOTTED);

	name = KIFLTKSTRINGS[246];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_CONNECTOR_SOLID);

	name = KIFLTKSTRINGS[247];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_REASON_NONE);

	name = KIFLTKSTRINGS[248];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_REASON_SELECTED);

	name = KIFLTKSTRINGS[249];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_REASON_DESELECTED);

	name = KIFLTKSTRINGS[250];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_REASON_OPENED);

	name = KIFLTKSTRINGS[251];
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_REASON_CLOSED);

	lastname = name;
	if (kifGlobals->flfrom == -1) {
		kifGlobals->flfrom = kifGlobals->KifStringId(firstname);
		kifGlobals->flto = kifGlobals->KifStringId(lastname);
	}
	return true;
}

#define KIFWIDGET 1001
class KifWidget;

class Idcouple {
public:

	KifElement* o;
	int id;
	int count;

	Idcouple(KifElement* e = NULL, bool set = false) {
		count = 0;
		o = e;
		if (e != NULL) {
			id = o->Ingarbage();
			if (set) {
				count++;
				o->Setreference();
			}
		}
		else
			id = -1;
	}

	void Use(KifElement* e) {
		if (e != NULL) {
			o = e;
			id = o->Ingarbage();
			if (e->type != window_type) {
				count++;
				e->Setreference();
			}
		}
	}

	void Set() {
		if (o != NULL) {
			count++;
			o->Setreference();
		}
	}

	void Reset() {
		if (o != NULL) {
			while (count > 0) {
				kifGlobals->ResetGarbageElement(o, id);
				count--;
			}
		}
	}

	KifFunction* Function() {
		return (KifFunction*)o;
	}

	KifElement* Object() {
		return o;
	}
};


class widge {
public:
	Fl_Widget* wdg;
	int idthread;
	int id;
	KifWidget* kw;

	widge(Fl_Widget* w, KifWidget* k, int idg, int i) {
		wdg = w;
		kw = k;
		id = idg;
		idthread = i;
	}

	void redraw();
};

void flredraw(void* data) {
	widge* w = (widge*)data;
	if (stopall || Errorid(w->idthread)) {
		delete w;
		return;
	}
	w->redraw();
	delete w;
}


static bool windowmode = false;

class KifWidget : public KifBasic {
public:
	int localreference;
	int calledthread;
	bool update;

	KifWidget(KifCode* kcode, KifElement* base, KifType t) : KifBasic(kcode, base, t) {
		calledthread = 0;
		windowmode = true;
		localreference = 0;
		update = true;
	}

	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		kifcode->Returnerror(KIFLTKSTRINGS[252]);
		return true;
	}

	virtual bool isindraw() {
		return false;
	}

	virtual Fl_Widget* Widget() {
		return NULL;
	}

	virtual void ResetWidget() {}

	int Yourowntype() {
		return KIFWIDGET;
	}

	virtual KifElement* CallbackObject() {
		return kifNULL;
	}

	virtual void setindraw(bool b) {}

	KifElement* MethodCreated(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (Widget() != NULL)
			return kifTRUE;
		return kifFALSE;
	}

	virtual KifElement* MethodCopy(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		string tocopy = callfunc->Evaluatethread(0, domain, _thread)->String();

		Fl::copy(STR(tocopy), tocopy.length(), 1);
		return kifTRUE;
	}

	virtual KifElement* MethodPaste(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		Fl::paste(*Widget(), 1);
		return kifTRUE;
	}

	KifElement* MethodAlign(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[254]);

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == kifString) {
			string align = ke->String();

			if (aligns.find(align) == aligns.end()) {
				align = KIFLTKSTRINGS[255];
				return Returningerror(kifcode, _thread, idcallthread, align);
			}

			wdg->align(aligns[align]);
		}
		else
			wdg->align(ke->Integer());

		return kifTRUE;
	}

	KifElement* MethodBackgroundColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		Fl_Widget* window = Widget();
		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[254]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(window->color());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		window->color(color);
		return kifTRUE;
	}


	KifElement* MethodRedraw(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		int idcallthread = (calledthread == 0) ? _thread : calledthread;

		if (stopall)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		Fl_Widget* wdg;
		//If we are in the main thread, well business as usual		
		wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		if (callfunc->Size() == 1)
			update = callfunc->Evaluatethread(0, domain, _thread)->Boolean();
		else
			update = true;
		if (_thread)
			Fl::awake(flredraw, (void*)new widge(wdg, this, idgarbage, _thread));
		else
			wdg->redraw();
		return kifTRUE;
	}

	KifElement* MethodFocus(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		Fl_Widget* wdg;
		//If we are in the main thread, well business as usual
		wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);
		wdg->take_focus();
		return kifTRUE;
	}

	KifElement* MethodCoordinates(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);
		KifElement* kvect;
		if (callfunc->Size() == 0) {
			kvect = Selectvectorinteger(callfunc, contextualpattern);
			kvect->storevalue((long)wdg->x());
			kvect->storevalue((long)wdg->y());
			kvect->storevalue((long)wdg->w());
			kvect->storevalue((long)wdg->h());
			return kvect;
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		int x, y, w, h;
		if (callfunc->Size() == 1) {
			KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
			if (ke->aVectorContainer() == false)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[257]);
			kvect = ke;
			if (kvect->Size() != 4)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[258]);
			x = kvect->ValueOnIndex(0)->Integer();
			y = kvect->ValueOnIndex(1)->Integer();
			w = kvect->ValueOnIndex(2)->Integer();
			h = kvect->ValueOnIndex(3)->Integer();
		}
		else {
			x = callfunc->Evaluatethread(0, domain, _thread)->Integer();
			y = callfunc->Evaluatethread(1, domain, _thread)->Integer();
			w = callfunc->Evaluatethread(2, domain, _thread)->Integer();
			h = callfunc->Evaluatethread(3, domain, _thread)->Integer();
		}
		if (x >= w || y >= h)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[259]);
		wdg->resize(x, y, w, h);
		return kifTRUE;
	}

	KifElement* MethodTooltip(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		string buf = callfunc->Evaluatethread(0, domain, _thread)->String();
		char* msg = (char*)malloc(buf.size() + 1);
		memcpy(msg, STR(buf), buf.size());
		msg[buf.size()] = 0;
		wdg->tooltip(msg);
		return kifTRUE;
	}

	KifElement* MethodSelectionColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wdg->selection_color());

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		wdg->selection_color(color);
		return kifTRUE;
	}

	KifElement* MethodHide(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		wdg->hide();
		return kifTRUE;
	}

	KifElement* MethodShow(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);
		wdg->show();
		return kifTRUE;
	}


	KifElement* MethodLabelColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		//In our example, we have only two parameters


		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wdg->labelcolor());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string shortcut = ke->String();
			if (colors.find(shortcut) == colors.end()) {
				shortcut = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, shortcut);
			}
			wdg->labelcolor(colors[shortcut]);
		}
		else {
			unsigned int c = ke->Integer();
			wdg->labelcolor(c);
		}

		return kifTRUE;
	}

	KifElement* MethodLabelSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;


		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[260]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wdg->labelsize());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		wdg->labelsize(ke->Integer());
		return kifTRUE;
	}


	KifElement* MethodLabelFont(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[261]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wdg->labelfont());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* labelfont;
		int font = 0;
		for (int i = 0; i < callfunc->Size(); i++) {
			labelfont = callfunc->Evaluatethread(i, domain, _thread);
			if (labelfont->type == kifString) {
				if (fonts.find(labelfont->String()) == fonts.end()) {
					string mes = KIFLTKSTRINGS[262];
					mes += labelfont->String();
					return Returningerror(kifcode, _thread, idcallthread, mes);
				}
				font = fonts[labelfont->String()];
			}
			else
				font = labelfont->Integer();
		}

		if (font >= fontmax)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[263]);
		wdg->labelfont(font);
		return kifTRUE;
	}

	KifElement* MethodLabelType(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;


		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[261]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wdg->labeltype());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* labeltype = callfunc->Evaluatethread(0, domain, _thread);
		wdg->labeltype((Fl_Labeltype)labeltype->Integer());
		return kifTRUE;
	}

	virtual KifElement* MethodLabel(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;


		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[261]);

		string label;
		if (callfunc->Size() == 0) {
			label = wdg->label();
			return kifcode->Providestring(label);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		label = callfunc->Evaluatethread(0, domain, _thread)->String();
		char* ltxt = (char*)malloc(label.size() + 1);
		memcpy(ltxt, STR(label), label.size());
		ltxt[label.size()] = 0;
		strcpy_s(ltxt, label.size() + 1, label.c_str());
		wdg->label(ltxt);
		return kifTRUE;
	}

	//we deactivate the garbage collector for these elements, which exist until destroyed from within
	virtual void Resetreference(int inc = 1) {
		localreference += inc;
		if (Widget() == NULL) {
			popped = true;
			KifElement::Resetreference(localreference);
			popped = false;
			localreference = 0;
		}
	}

	//Then we use the localreference counter to clean these objects up
	void clean() {
		popped = true;
		KifElement::Resetreference(localreference);
		popped = false;
	};

	virtual void itemclose() {}

	virtual KifElement* same(KifElement* a) {
		if (a == kifNULL) {
			if (Widget() == NULL)
				return kifTRUE;
			return kifFALSE;
		}
		if (a->type != type)
			return kifFALSE;
		KifWidget* w = (KifWidget*)a;
		if (Widget() == w->Widget())
			return kifTRUE;
		return kifFALSE;
	}

};


//-------------------------------------------------------------------------------------------------------
class Kifmessage {
public:
	KifWidget* widget;
	int iwidget;
	int idthread;
	KifElement* context;
	KifDomain* domain;
	KifCallFunction* callfunc;
	KifDomain* stackfunc;
	KifDomain* stackdom;
	vector<int> params;

	Kifmessage(int t, KifWidget* w, KifElement* c, KifDomain* d, KifCallFunctionExternal* cl)  {
		stackfunc = kifGlobals->Topthread(t);
		stackdom = kifGlobals->Beforetopthread(t);

		widget = w;
		iwidget = w->Ingarbage();
		idthread = t;
		context = c;
		domain = d;

		callfunc = new KifCallFunctionExternal(NULL, NULL, cl->name, cl->body);
		callfunc->Initialise(cl);
		callfunc->kifcode = cl->kifcode;

		for (int i = 0; i < callfunc->parameters.size(); i++) {
			callfunc->parameters[i] = callfunc->Evaluatethread(i, domain, idthread);
			callfunc->parameters[i]->Setreference();
			params.push_back(callfunc->parameters[i]->Ingarbage());
		}
	}

	bool Valid() {
		if (kifGlobals->Garbageexist(iwidget, widget) == false)
			return false;

		for (int i = 0; i < callfunc->Size(); i++) {
			if (params[i]>0 && kifGlobals->Garbageexist(params[i], callfunc->parameters[i]) == false)
				return false;
		}
		return true;
	}

	~Kifmessage() {
		for (int i = 0; i < callfunc->Size(); i++) {
			if (params[i] <= 0 || kifGlobals->Garbageexist(params[i], callfunc->parameters[i]) == true)
				callfunc->parameters[i]->Resetreference();
		}
		callfunc->parameters.clear();
		delete callfunc;
	}
};

//-------------------------------------------------------------------------------------------------------
void widge::redraw() {
	if (kifGlobals->garbage[id] == kw) {
		if (kw->Widget() == wdg)
			wdg->redraw();
	}
}
//-------------------------------------------------------------------------------------------------------

static vector<Kifmessage*> kiffltkmessages;

//-------------------------------------------------------------------------------------------------------
static int Fltkrun();
//-------------------------------------------------------------------------------------------------------

class KifWidgetCallback : public KifWidget {
public:

	KifElement* function;
	KifElement* object;
	int idom;
	int icontext;
	KifDomain* dom;
	KifElement* context;

	KifWidgetCallback(KifCode* kcode, KifElement* base, KifType t) : KifWidget(kcode, base, t) {
		function = base->Robfunction(this);
		object = kifNULL;
		dom = NULL;
		context = NULL;
		idom = 0;
		icontext = 0;
	}

	//this function is called to test the kind of function we expect from the library with the KIFLTKSTRINGS[264] operator...
	//It works for most callback functions...
	bool testfunction(KifElement* func) {
		if (func->type == kifFunction) {
			KifFunction* kfunction = (KifFunction*)func;
			if (type == editor_type) {
				if (kfunction->arguments.size() != 7)
					return false;
			}
			else {
				if (type == table_type) {
					if (kfunction->arguments.size() != 3)
						return false;
				}
				else {
					if (kfunction->arguments.size() != 2)
						return false;
				}
			}

			if (kfunction->arguments[0]->VariableValue()->inType() != type)
				return false;
			return true;
		}
		return false;
	}


	void Keeptrack(KifDomain* d, KifElement* c) {
		d->Setreference();
		c->Setreference();
		dom = d;
		context = c;
		idom = dom->Ingarbage();
		icontext = context->Ingarbage();
	}

	~KifWidgetCallback() {
		if (dom != NULL) {
			kifGlobals->ResetGarbageElement(dom, idom);
			kifGlobals->ResetGarbageElement(context, icontext);
		}
	}

	bool initialconstructor() {
		return true;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	KifElement* constructor(KifDomain* domloc, KifElement* params, int idthread, KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize() == 0)
			return this;

		//Your first parameter is at position 0 etc...		
		object = params->Instruction(0); //would match i as in the example above: mynewtype toto(i,j);
		if (object->type == kifInstance)
			object = object->Thevariable(domloc, idthread, callfunc);
		else
			object = object->Exec(kifNULL, domloc, kifNULL, idthread, callfunc);
		if (object->type != window_type)
			object->Setreference();
		return this;
	}



};


//This section handles multi-threading. A request to create or modify a widget cannot be handled at the thread level
//it must be executed through the main thread, through a awake

void CleanMessages() {
	for (int i = 0; i < kiffltkmessages.size(); i++)
		delete kiffltkmessages[i];
	kiffltkmessages.clear();
}

void LaunchWindowCall() {
	Kifmessage* cmsg = NULL;
	KifWidget* wdg;
	vector<Kifmessage*> msgs;
	int sz;
	{
		ThreadLock _lock(kifCallExecuteThreadExternal);
		sz = kiffltkmessages.size();
		msgs = kiffltkmessages;
		kiffltkmessages.clear();
	}

	bool err = false;
	int i;
	for (i = 0; i < sz; i++) {
		cmsg = msgs[i];
		wdg = cmsg->widget;
		if (!err && wdg != NULL && cmsg->Valid()) {
			kifGlobals->PushFunctionStackid(0, cmsg->stackfunc, cmsg->stackdom);
			wdg->calledthread = cmsg->idthread;
			if (kifGlobals->THR[cmsg->idthread]->used == true)
				wdg->ExecMethod(cmsg->callfunc->Name(), cmsg->context, cmsg->domain, 0, cmsg->callfunc);
			err = Errorid(0);
			wdg->calledthread = 0;
			kifGlobals->PopFunctionStack(0);
		}
		delete cmsg;
	}
}

void PostWindowCall(int threadid, KifWidget* w, KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
	{
		ThreadLock _lock(kifCallExecuteThreadExternal);
		kiffltkmessages.push_back(new Kifmessage(threadid, w, contextualpattern, domain, (KifCallFunctionExternal*)callfunc));
	}
	Fl::awake();
}

static void SaveItem(KifWidget*);
//---------------------------------------------------------------------------------------------------------------------

class Kifimage : public KifWidget {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, imageMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Image* image;
	string filename;
	//---------------------------------------------------------------------------------------------------------------------
	Kifimage(KifCode* kifcode, KifElement* base) : KifWidget(kifcode, base, image_type) {
		//Do not forget your variable initialisation
		image = NULL;

	}

	void itemclose() {
		image = NULL;
		clean();
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifimage* kf = new Kifimage(kifcode, kifNULL);
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods
#ifndef FLTKNOJPEG
	KifElement* MethodLoadImageJPEG(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (image != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[266]);
		KifElement* kfilename = callfunc->Evaluatethread(0, domain, _thread);
		filename = kfilename->String();
		if (filename != "") {
			image = new Fl_JPEG_Image(filename.c_str());
			if (image == NULL)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[267]);
			SaveItem(this);
			return kifTRUE;
		}
		return kifFALSE;
	}
#endif

	KifElement* MethodLoadImageGIF(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (image != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[266]);
		KifElement* kfilename = callfunc->Evaluatethread(0, domain, _thread);
		filename = kfilename->String();
		if (filename != "") {
			image = new Fl_GIF_Image(filename.c_str());
			if (image == NULL)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[267]);
			SaveItem(this);
			return kifTRUE;
		}
		return kifFALSE;
	}

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return filename; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifimage::kifexportedmethods;
map<short, imageMethod>  Kifimage::linkedmethods;
map<string, string> Kifimage::infomethods;
hmap<short, bool> Kifimage::methodassigned;

//---------------------------------------------------------------------------------------------------------------------
class Kifbitmap : public KifWidget {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, bitmapMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Bitmap* bitmap;
	uchar* bm;
	int szw, szh;
	//---------------------------------------------------------------------------------------------------------------------
	Kifbitmap(KifCode* kifcode, KifElement* base) : KifWidget(kifcode, base, bitmap_type) {
		//Do not forget your variable initialisation
		bitmap = NULL;
		bm = NULL;
		szw = 0;
		szh = 0;
	}

	~Kifbitmap() {
		if (bitmap != NULL)
			delete bitmap;
		if (bm != NULL)
			delete[] bm;
	}

	void itemclose() {
		bitmap = NULL;
		clean();
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifbitmap* kf = new Kifbitmap(kifcode, kifNULL);
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	bool initialconstructor() {
		return true;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	KifElement* constructor(KifDomain* domloc, KifElement* params, int idthread, KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize() != 3)
			return this;

		KifElement* kbitmaps = params->Instruction(0)->Exec(kifNULL, domloc, kifNULL, idthread, callfunc);
		if (!kbitmaps->aVectorContainer())
			return Returningerror(kifcode, idthread, idthread, KIFLTKSTRINGS[269]);

		KifElement* ke = params->Instruction(1)->Exec(kifNULL, domloc, kifNULL, idthread, callfunc);

		szw = ke->Integer();
		ke->Release();
		ke = params->Instruction(2)->Exec(kifNULL, domloc, kifNULL, idthread, callfunc);
		szh = ke->Integer();
		ke->Release();

		int sz = kbitmaps->Size();
		
		int tst = (szw*szh) >> 3;
		if (tst!=sz)
			return Returningerror(kifcode, idthread, idthread, KIFLTKSTRINGS[270]);
		
		int rowBytes = (szw + 7) >> 3;
		bm = new uchar[rowBytes*szh];
		for (int i = 0; i < sz; i++)
			bm[i] = (uchar)kbitmaps->getinteger(i);
		bitmap = new Fl_Bitmap(bm, szw, szh);
		kbitmaps->Release();
		return this;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods
	KifElement* MethodLoadbitmap(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (bitmap != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[271]);
		KifElement* kbitmaps = callfunc->Evaluatethread(0, domain, _thread);
		szw = callfunc->Evaluatethread(1, domain, _thread)->Integer();
		szh = callfunc->Evaluatethread(2, domain, _thread)->Integer();
		if (!kbitmaps->aVectorContainer())
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[269]);

		int sz = kbitmaps->Size();
		int tst = (szw*szh) >> 3;
		if (tst != sz)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[270]);

		int rowBytes = (szw + 7) >> 3;
		bm = new uchar[rowBytes*szh];

		for (int i = 0; i < sz; i++)
			bm[i] = (uchar)kbitmaps->getinteger(i);
		bitmap = new Fl_Bitmap(bm, szw, szh);
		//delete[] bm;
		return kifTRUE;
	}

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return ""; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};

//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifbitmap::kifexportedmethods;
map<short, bitmapMethod>  Kifbitmap::linkedmethods;
map<string, string> Kifbitmap::infomethods;
hmap<short, bool> Kifbitmap::methodassigned;

//---------------------------------------------------------------------------------------------------------------------
static void button_callback(Fl_Widget *w, void *data);
static void window_callback(Fl_Widget *w, void *data);
static void timeout_callback(void *data);
static void menu_callback(Fl_Widget *w, void *data);
static void close_callback(Fl_Widget *w, void *data);

class KifFLCallback {
public:

	KifElement* function;
	Kifwindow* window;
	KifElement* context;
	KifDomain* dom;
	KifElement* object;
	int iobject;
	int idom;
	int icontext;

	KifFLCallback(KifElement* func, KifElement* o, Kifwindow* win, KifElement* cont, KifDomain* domain) {
		function = func;
		window = win;
		context = cont;
		dom = domain;
		object = o;
		dom->Setreference();
		context->Setreference();
		object->Setreference();
		idom = dom->Ingarbage();
		icontext = context->Ingarbage();
		iobject = object->Ingarbage();
	}

	~KifFLCallback() {
		kifGlobals->ResetGarbageElement(dom, idom);
		kifGlobals->ResetGarbageElement(context, icontext);
		kifGlobals->ResetGarbageElement(object, iobject);
	}
};

static void window_termination(Kifwindow* kwnd);

class KifDoubleWindow : public Fl_Double_Window {
public:
	KifElement* function;
	KifElement* object;
	KifElement* context;
	KifDomain* dom;
	int icontext;
	int idom;
	bool ready;
	Kifwindow* window;
	int iwindow;
	KifCode* kifcode;
	map<int, Idcouple> functionmouse;
	map<int, Idcouple> functionkey;
	map<int, Idcouple> objectmouse;
	map<int, Idcouple> objectkey;
	bool dragdrop;
	Idcouple functiondragdrop;
	Idcouple objectdragdrop;
	string eventtext;


	KifDoubleWindow(int x, int y, const char* l, KifElement* f, KifElement* o, KifElement* c, KifDomain* d, Kifwindow* wn);
	KifDoubleWindow(int x, int y, int w, int h, const char* l, KifElement* f, KifElement* o, KifElement* c, KifDomain* d, Kifwindow* wn);

	~KifDoubleWindow();

	int mouse_callback(int e);
	int key_callback(int e);
	int dragdropcall_back();

	bool Setdragdrop(KifElement* f, KifElement* o) {
		functiondragdrop.Use(f);
		objectdragdrop.Use(o);
		return true;
	}

	bool Setmouseaction(int e, KifElement* f, KifElement* object) {
		if (!f->isFunction() || f->Size() != 3 || f->Argument(0)->VariableValue()->inType() != window_type)
			return false;
		switch (e) {
		case FL_PUSH:
		case FL_RELEASE:
		case FL_MOVE:
		case FL_DRAG:
		case FL_MOUSEWHEEL:
			functionmouse[e].Use(f);
			objectmouse[e].Use(object);
			return true;
		}
		return false;
	}

	bool Setkeyaction(int e, KifElement* f, KifElement* object) {
		if (!f->isFunction() || f->Size() != 5 || f->Argument(0)->VariableValue()->inType() != window_type)
			return false;
		if (e != FL_KEYDOWN && e != FL_KEYUP)
			return false;
		functionkey[e].Use(f);
		objectkey[e].Use(object);
		return true;
	}

	void draw();
	int handle(int e);

};

class Kifcomplexwidget : public KifWidgetCallback {
public:
	vector<char> begins;
	bool indraw;

	Kifcomplexwidget(KifCode* kifcode, KifElement* base, KifType t) : KifWidgetCallback(kifcode, base, t) {
		indraw = false;
	}

	void setindraw(bool b) {
		indraw = b;
	}

	bool isindraw() {
		if (indraw)
			ThreadLock _lock(kifDISPLAY);
		return indraw;
	}

	KifElement* MethodPoint(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);

		fl_point(x->Integer(), y->Integer());
		return kifTRUE;
	}

	KifElement* MethodDrawColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		fl_color(color);
		return kifTRUE;
	}

	KifElement* MethodLineShape(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);


		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		int w = callfunc->Evaluatethread(1, domain, _thread)->Integer();

		//0 is the first parameter and so on...
		if (ke->type == kifString) {
			string buf = ke->String();
			if (lineshape.find(buf) == lineshape.end()) {
				buf = KIFLTKSTRINGS[272];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}

			fl_line_style(lineshape[buf], w);
		}
		else
			fl_line_style(ke->Integer(), w);
		return kifTRUE;
	}

	KifElement* MethodRGBColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		if (callfunc->Size() == 3) {
			KifElement* r = callfunc->Evaluatethread(0, domain, _thread);
			KifElement* g = callfunc->Evaluatethread(1, domain, _thread);
			KifElement* b = callfunc->Evaluatethread(2, domain, _thread);
			return kifcode->Provideinteger(fl_rgb_color(r->Integer(), g->Integer(), b->Integer()));
		}

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->aVectorContainer()) {
			if (ke->Size() != 3)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[273]);
			return kifcode->Provideinteger(fl_rgb_color(ke->ValueOnIndex(0)->Integer(), ke->ValueOnIndex(1)->Integer(), ke->ValueOnIndex(2)->Integer()));
		}

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		uchar r, g, b;
		Fl::get_color(color, r, g, b); // Fl_Color to RGB
		KifVectorInteger* kvect = new KifVectorInteger(kifcode, NULL);
		kvect->values.push_back(r);
		kvect->values.push_back(g);
		kvect->values.push_back(b);
		return kvect;
	}

	KifElement* MethodRectangle(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);

		if (callfunc->Size() == 5) {
			Fl_Color color;
			KifElement* ke = callfunc->Evaluatethread(4, domain, _thread);
			if (ke->type == kifString) {
				//0 is the first parameter and so on...
				string buf = ke->String();
				if (colors.find(buf) == colors.end()) {
					buf = KIFLTKSTRINGS[256];
					return Returningerror(kifcode, _thread, idcallthread, buf);
				}
				color = colors[buf];
			}
			else
				color = ke->Integer();
			fl_rect(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), color);
		}
		else
			fl_rect(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
		return kifTRUE;
	}

	KifElement* MethodRectangleFill(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);

		if (callfunc->Size() == 5) {
			Fl_Color color;
			KifElement* ke = callfunc->Evaluatethread(4, domain, _thread);
			if (ke->type == kifString) {
				//0 is the first parameter and so on...
				string buf = ke->String();
				if (colors.find(buf) == colors.end()) {
					buf = KIFLTKSTRINGS[256];
					return Returningerror(kifcode, _thread, idcallthread, buf);
				}
				color = colors[buf];
			}
			else
				color = ke->Integer();
			fl_rectf(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), color);
		}
		else
			fl_rectf(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
		return kifTRUE;
	}

	KifElement* MethodLoop(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* x1 = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* y1 = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* x2 = callfunc->Evaluatethread(4, domain, _thread);
		KifElement* y2 = callfunc->Evaluatethread(5, domain, _thread);

		if (callfunc->Size() == 6)
			fl_loop(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer());
		else {
			KifElement* x3 = callfunc->Evaluatethread(6, domain, _thread);
			KifElement* y3 = callfunc->Evaluatethread(7, domain, _thread);
			fl_loop(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), x3->Integer(), y3->Integer());
		}
		return kifTRUE;
	}

	KifElement* MethodPolygon(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* x1 = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* y1 = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* x2 = callfunc->Evaluatethread(4, domain, _thread);
		KifElement* y2 = callfunc->Evaluatethread(5, domain, _thread);

		if (callfunc->Size() == 6)
			fl_polygon(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer());
		else {
			KifElement* x3 = callfunc->Evaluatethread(6, domain, _thread);
			KifElement* y3 = callfunc->Evaluatethread(7, domain, _thread);
			fl_polygon(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), x3->Integer(), y3->Integer());
		}
		return kifTRUE;
	}

	KifElement* MethodLine(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* x1 = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* y1 = callfunc->Evaluatethread(3, domain, _thread);

		if (callfunc->Size() == 4)
			fl_line(x->Integer(), y->Integer(), x1->Integer(), y1->Integer());
		else {
			KifElement* x2 = callfunc->Evaluatethread(4, domain, _thread);
			KifElement* y2 = callfunc->Evaluatethread(5, domain, _thread);
			fl_line(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer());
		}
		return kifTRUE;
	}

	//we draw a line between from a given point with a distance and an angle... The angle should be given in radian
	KifElement* MethodLineRotation(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		double x = callfunc->Evaluatethread(0, domain, _thread)->Float();
		double y = callfunc->Evaluatethread(1, domain, _thread)->Float();
		double distance = callfunc->Evaluatethread(2, domain, _thread)->Float();
		double angle = callfunc->Evaluatethread(3, domain, _thread)->Float();

		bool draw = false;
		if (callfunc->Size()==5)
			draw = callfunc->Evaluatethread(4, domain, _thread)->Boolean();

		double x1, y1;

		x1 = x + cos(angle)*distance;
		y1 = y - sin(angle)*distance;

		if (draw)
			fl_line((long)x, (long)y, (long)x1, (long)y1);

		KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
		kvect->storevalue(x1);
		kvect->storevalue(y1);
		return kvect;
	}


	KifElement* MethodPie(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* w = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* h = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* a1 = callfunc->Evaluatethread(4, domain, _thread);
		KifElement* a2 = callfunc->Evaluatethread(5, domain, _thread);

		fl_pie(x->Integer(), y->Integer(), w->Integer(), h->Integer(), a1->Float(), a2->Float());
		return kifTRUE;
	}

	KifElement* MethodCircle(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		int x = callfunc->Evaluatethread(0, domain, _thread)->Integer();
		int y = callfunc->Evaluatethread(1, domain, _thread)->Integer();
		int r = callfunc->Evaluatethread(2, domain, _thread)->Integer();

		if (callfunc->Size() == 3) {
			fl_circle(x, y, r);
			return kifTRUE;
		}

		KifElement* kcolor = callfunc->Evaluatethread(3, domain, _thread);
		Fl_Color color;
		if (kcolor->type == kifString) {
			//0 is the first parameter and so on...
			string buf = kcolor->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			color = colors[buf];
		}
		else
			color = kcolor->Integer();

		//we set the color
		fl_color(color);
		fl_circle(x, y, r);
		return kifTRUE;
	}

	KifElement* MethodTextMeasure(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* t = callfunc->Evaluatethread(0, domain, _thread);

		string buf = t->String();
		int w;
		int h;
		fl_measure(buf.c_str(), w, h, 1);
		KifMapStringInteger* map = new KifMapStringInteger(kifcode, NULL);
		map->values[KIFLTKSTRINGS[274]] = w;
		map->values["H"] = h;
		return map;
	}

	KifElement* MethodDrawText(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);
#ifdef WIN32
		//On WIN32, if no font is given beforehand, the whole stuff crashes...
		if (fl_graphics_driver->font_descriptor() == NULL)
			fl_font(FL_HELVETICA, 12);
#endif
		KifElement* t = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* x = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(2, domain, _thread);

		string buf = t->String();
		char* label = new char[buf.size() + 1];
		strcpy_s(label, buf.size() + 1, buf.c_str());
		fl_draw(label, x->Integer(), y->Integer());
		delete[] label;
		return kifTRUE;
	}

	KifElement* MethodTextFont(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* f = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* sz = callfunc->Evaluatethread(1, domain, _thread);
		if (f->type == kifString) {
			string buf = f->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = KIFLTKSTRINGS[275];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			fl_font(fonts[buf], sz->Integer());
		}
		else {
			int i = f->Integer();
			if (i >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[263]);
			fl_font(i, sz->Integer());
		}
		return kifTRUE;
	}

	KifElement* MethodArc(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* w = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* h = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* a1 = callfunc->Evaluatethread(4, domain, _thread);
		if (begins.size() != 0) {
			if (callfunc->Size() == 6)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[276]);
			fl_arc(x->Float(), y->Float(), w->Float(), h->Float(), a1->Float());
		}
		else {
			if (callfunc->Size() != 6)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[277]);
			KifElement* a2 = callfunc->Evaluatethread(5, domain, _thread);
			fl_arc(x->Integer(), y->Integer(), w->Integer(), h->Integer(), a1->Float(), a2->Float());
		}
		return kifTRUE;
	}


	KifElement* MethodMultmatrix(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* a = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* b = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* c = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* d = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* x = callfunc->Evaluatethread(4, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(5, domain, _thread);
		fl_mult_matrix(a->Float(), b->Float(), c->Float(), d->Float(), x->Float(), y->Float());
		return kifTRUE;
	}

	KifElement* MethodRotate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* d = callfunc->Evaluatethread(0, domain, _thread);
		fl_rotate(d->Float());

		return kifTRUE;
	}


	KifElement* MethodTranslate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		fl_translate(x->Float(), y->Float());

		return kifTRUE;
	}


	KifElement* MethodTransform_vertex(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		fl_transformed_vertex(x->Float(), y->Float());

		return kifTRUE;
	}


	KifElement* MethodTransform_dy(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		double v = fl_transform_dy(x->Float(), y->Float());

		return kifcode->Providefloat(v);
	}


	KifElement* MethodTransform_dx(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		double v = fl_transform_dx(x->Float(), y->Float());

		return kifcode->Providefloat(v);
	}


	KifElement* MethodTransform_x(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		double v = fl_transform_x(x->Float(), y->Float());

		return kifcode->Providefloat(v);
	}

	KifElement* MethodTransform_y(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		double v = fl_transform_y(x->Float(), y->Float());

		return kifcode->Providefloat(v);
	}


	KifElement* MethodScale(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		if (callfunc->Size() == 2) {
			KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
			fl_scale(x->Float(), y->Float());
		}
		else
			fl_scale(x->Float());

		return kifTRUE;
	}

	KifElement* MethodPushMatrix(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);
		begins.push_back(6);
		fl_push_matrix();
		return kifTRUE;
	}

	KifElement* MethodPopMatrix(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (!begins.size() || begins.back() != 6)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[278]);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);
		begins.pop_back();
		fl_pop_matrix();
		return kifTRUE;
	}

	KifElement* MethodBeginPoints(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);
		begins.push_back(1);
		fl_begin_points();
		return kifTRUE;
	}

	KifElement* MethodEndPoints(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size() || begins.back() != 1)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[278]);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);
		begins.pop_back();
		fl_end_points();
		return kifTRUE;
	}

	KifElement* MethodBeginLoop(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);
		begins.push_back(2);
		fl_begin_loop();
		return kifTRUE;
	}

	KifElement* MethodEndLoop(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size() || begins.back() != 2)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[279]);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		begins.pop_back();
		fl_end_loop();
		return kifTRUE;
	}

	KifElement* MethodBeginPolygon(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		begins.push_back(3);
		fl_begin_polygon();
		return kifTRUE;
	}

	KifElement* MethodEndPolygon(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size() || begins.back() != 3)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[280]);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		begins.pop_back();
		fl_end_polygon();
		return kifTRUE;
	}

	KifElement* MethodBeginComplexPolygon(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		begins.push_back(4);
		fl_begin_complex_polygon();
		return kifTRUE;
	}

	KifElement* MethodGap(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size() || begins.back() != 4)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[281]);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		fl_gap();
		return kifTRUE;
	}

	KifElement* MethodEndComplexPolygon(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size() || begins.back() != 4)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[281]);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		begins.pop_back();
		fl_end_complex_polygon();
		return kifTRUE;
	}

	KifElement* MethodBeginLine(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		begins.push_back(5);
		fl_begin_line();
		return kifTRUE;
	}

	KifElement* MethodEndLine(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size() || begins.back() != 5)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[282]);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		begins.pop_back();
		fl_end_line();
		return kifTRUE;
	}

	KifElement* MethodCurve(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size())
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[283]);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* x1 = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* y1 = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* x2 = callfunc->Evaluatethread(4, domain, _thread);
		KifElement* y2 = callfunc->Evaluatethread(5, domain, _thread);
		KifElement* x3 = callfunc->Evaluatethread(6, domain, _thread);
		KifElement* y3 = callfunc->Evaluatethread(7, domain, _thread);
		fl_curve(x->Float(), y->Float(), x1->Float(), y1->Float(), x2->Float(), y2->Float(), x3->Float(), y3->Float());
		return kifTRUE;
	}

	KifElement* MethodVertex(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size())
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[283]);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);


		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		fl_vertex(x->Float(), y->Float());
		return kifTRUE;
	}

	KifElement* MethodPopClip(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		fl_pop_clip();
		return kifTRUE;
	}

	KifElement* MethodPushClip(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);


		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);
		fl_push_clip(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
		return kifTRUE;
	}

};

class Kifwindow : public Kifcomplexwidget {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, windowMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;
	string buf;
	KifFLCallback* onclose;
	KifFLCallback* timeout;
	int ionclose;
	int itimeout;
	vector<int> items;
	bool closed;
	char txt[1024];
	Fl_Menu_Item* flmenu;
	int flmenunbelements;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	KifDoubleWindow* window;
	bool end;

	//---------------------------------------------------------------------------------------------------------------------
	Kifwindow(KifCode* kifcode, KifElement* base) : Kifcomplexwidget(kifcode, base, window_type) {
		//Do not forget your variable initialisation
		window = NULL;
		end = false;
		onclose = NULL;
		timeout = NULL;
		closed = false;
		flmenu = NULL;
		flmenunbelements = 0;
	}

	void Resetreference(int inc = 1) {
		if (window != NULL)
			Setpopped();
		KifElement::Resetreference(inc);
	}

	void itemclose() {
		if (flmenunbelements) {
			for (int i = 0; i < flmenunbelements; i++) {
				if (flmenu[i].text != NULL) {
					free((char*)flmenu[i].text);
					delete (KifFLCallback*)flmenu[i].user_data();
				}
			}
			delete[] flmenu;
		}

		KifElement* ke;
		for (int i = 0; i < items.size(); i++) {
			ke = kifGlobals->Garbagefind(items[i]);
			if (ke != NULL)
				((KifWidget*)ke)->itemclose();
		}
		items.clear();
		if (mainwindow == NULL || mainwindow == this)
			stopall = true;
	}

	~Kifwindow() {
		if (window != NULL) {
			if (!end)
				window->end();
			window_termination(this);
		}

		if (onclose != NULL)
			delete onclose;
		if (timeout != NULL)
			delete timeout;
		if (mainwindow == this)
			mainwindow = NULL;
		onclose = NULL;
		timeout = NULL;
	}

	void ResetWidget() {
		dom = NULL;
		if (window != NULL) {
			KifDoubleWindow* wnd = window;
			wnd->window = NULL;
			window = NULL;
			end = false;
			delete wnd;
		}
		if (onclose != NULL)
			delete onclose;
		if (timeout != NULL)
			delete timeout;
		onclose = NULL;
		timeout = NULL;
	}

	Fl_Widget* Widget() {
		return window;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifwindow* kf = new Kifwindow(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* CallbackObject() {
		return object;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodBegin(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthreadcreate(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		if (window != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[284]);
		indraw = true;
		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		string label;
		if (callfunc->Size() == 3) {
			label = callfunc->Evaluatethread(2, domain, _thread)->String();
			strcpy_s(txt, 1024, label.c_str());
			window = new KifDoubleWindow(x->Integer(), y->Integer(), txt, function, object, contextualpattern, domain, this);
			window->size_range(x->Integer(), y->Integer());
		}
		else {
			KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
			KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);
			label = callfunc->Evaluatethread(4, domain, _thread)->String();
			strcpy_s(txt, 1024, label.c_str());
			window = new KifDoubleWindow(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt, function, object, contextualpattern, domain, this);
		}
		current_window.push_back(this);
		window->callback(close_callback, this);
		return kifTRUE;
	}

	KifElement* MethodGetFontSizes(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		int i = callfunc->Evaluatethread(0, domain, _thread)->Integer();
		if (i >= fontmax)
			return kifcode->Returnerror(KIFLTKSTRINGS[285]);

		int* sizes;
		int nb = Fl::get_font_sizes(i, sizes);
		KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
		kvect->Clear();
		if (nb == 0)
			return kvect;
		for (int i = 0; i < nb; i++)
			kvect->storevalue((long)sizes[i]);
		return kvect;
	}

	KifElement* MethodGetFont(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		int i = callfunc->Evaluatethread(0, domain, _thread)->Integer();
		if (i >= fontmax)
			return kifcode->Returnerror(KIFLTKSTRINGS[285]);
		return kifcode->Providestringraw(Fl::get_font_name(i));
	}

	KifElement* MethodSetFonts(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		fontmax = Fl::set_fonts("*");
		return kifcode->Provideinteger(fontmax);
	}

	KifElement* MethodNbFonts(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		return kifcode->Provideinteger(fontmax);
	}

	KifElement* MethodOnClose(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);


		KifElement* function = callfunc->Evaluatethread(0, domain, _thread);

		if (function->type != kifCall && !function->isFunction() && !testfunction(function))
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[286]);

		KifElement* object = callfunc->parameters[1];

		if (object->type == kifInstance)
			object = object->Thevariable(domain, _thread, NULL);
		else
			object = object->Exec(kifNULL, domain,kifNULL,_thread,callfunc);

		onclose = new KifFLCallback(function, object, this, contextualpattern, domain);
		window->callback(window_callback, onclose);
		return kifTRUE;
	}

	KifElement* MethodOntime(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);


		KifElement* function = callfunc->Evaluatethread(0, domain, _thread);

		if (function->type != kifCall && !function->isFunction() && !testfunction(function))
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[286]);

		double timevalue = callfunc->Evaluatethread(1, domain, _thread)->Float();
		KifElement* object = callfunc->parameters[2];

		if (object->type == kifInstance)
			object = object->Thevariable(domain, _thread, NULL);
		else
			object = object->Exec(kifNULL, domain,kifNULL,_thread,callfunc);

		timeout = new KifFLCallback(function, object, this, contextualpattern, domain);

#ifdef WIN32
		FlResetTimer();
#endif
		Fl::add_timeout(timevalue, timeout_callback, (void*)timeout);
		return kifTRUE;
	}

	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthreadcreate(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters


		if (window != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[284]);

		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		string label;
		if (callfunc->Size() == 3) {
			label = callfunc->Evaluatethread(2, domain, _thread)->String();
			strcpy_s(txt, 1024, label.c_str());
			window = new KifDoubleWindow(x->Integer(), y->Integer(), txt, function, object, contextualpattern, domain, this);
			window->size_range(x->Integer(), y->Integer());
		}
		else {
			KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
			KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);
			label = callfunc->Evaluatethread(4, domain, _thread)->String();
			strcpy_s(txt, 1024, label.c_str());
			window = new KifDoubleWindow(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt, function, object, contextualpattern, domain, this);
		}
		end = true;
		window->callback(close_callback, this);
		window->end();
		window->show();
		return kifTRUE;
	}

	KifElement* MethodFlush(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//If we are in the main thread, well business as usual
		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);
		window->flush();
		return kifTRUE;
	}

	KifElement* MethodLabel(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		if (callfunc->Size() == 0) {
			string l = window->label();
			return kifcode->Providestring(l);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		string label = callfunc->Evaluatethread(0, domain, _thread)->String();
		strcpy_s(txt, 1024, label.c_str());
		window->label(txt);
		return kifTRUE;
	}

	KifElement* MethodPosition(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		//In our example, we have only two parameters


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		if (callfunc->Size() == 0) {
			KifVectorInteger* kvect = new KifVectorInteger(kifcode, NULL);
			kvect->values.push_back(window->x_root());
			kvect->values.push_back(window->y_root());
			return kvect;
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		window->position(x->Integer(), y->Integer());
		return kifTRUE;
	}

	KifElement* MethodSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		//In our example, we have only two parameters

		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		if (callfunc->Size() == 0) {
			KifVectorInteger* kvect = new KifVectorInteger(kifcode, NULL);
			kvect->values.push_back(window->x());
			kvect->values.push_back(window->y());
			kvect->values.push_back(window->w());
			kvect->values.push_back(window->h());
			return kvect;
		}


		threadtosetvalue(contextualpattern, domain, callfunc);
		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);
		window->resize(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
		return kifTRUE;
	}

	KifElement* MethodAwake(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);
		Fl::awake();
		return kifTRUE;
	}


	KifElement* MethodLock(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		Fl::lock();
		return kifTRUE;
	}


	KifElement* MethodUnlock(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		Fl::unlock();
		return kifTRUE;
	}


	KifElement* MethodOnDragDrop(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		//0 is the first parameter and so on...
		KifElement* function = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* object = callfunc->parameters[1];

		if (object->type == kifInstance)
			object = object->Thevariable(domain, _thread, NULL);
		else
			object = object->Exec(kifNULL, domain,kifNULL,_thread,callfunc);

		window->Setdragdrop(function, object);
		return kifTRUE;
	}


	KifElement* MethodOnMouse(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);
		if (end)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[287]);

		//0 is the first parameter and so on...
		KifElement* kaction = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* function = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* object = callfunc->parameters[2];

		if (object->type == kifInstance)
			object = object->Thevariable(domain, _thread, NULL);
		else
			object = object->Exec(kifNULL, domain,kifNULL,_thread,callfunc);

		int act;
		if (kaction->type == kifString) {
			string action = kaction->String();
			if (actions.find(action) == actions.end())
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[288]);
			act = actions[action];
		}
		else
			act = kaction->Integer();
		if (window->Setmouseaction(act, function, object) == false)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[289]);
		return kifTRUE;
	}

	KifElement* MethodOnKey(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);
		if (end)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[287]);

		//0 is the first parameter and so on...
		KifElement* kaction = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* function = callfunc->Evaluatethread(1, domain, _thread);

		KifElement* object = callfunc->parameters[2];
		if (object->type == kifInstance)
			object = object->Thevariable(domain, _thread, NULL);
		else
			object = object->Exec(kifNULL, domain,kifNULL,_thread,callfunc);

		int act;
		if (kaction->type == kifString) {
			string action = kaction->String();
			if (actions.find(action) == actions.end())
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[290]);
			act = actions[action];
		}
		else
			act = kaction->Integer();

		if (window->Setkeyaction(act, function, object) == false)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[291]);
		return kifTRUE;
	}

	KifElement* MethodWindowHide(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		//0 is the first parameter and so on...
		bool kaction = callfunc->Evaluatethread(0, domain, _thread)->Boolean();
		if (kaction == true) {
			if (window->shown())
				window->hide();
		}
		else
		if (!window->shown())
			window->show();
		return kifTRUE;
	}

	KifElement* MethodCursorStyle(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		//0 is the first parameter and so on...
		KifElement* kaction = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kcolor1 = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* kcolor2 = callfunc->Evaluatethread(2, domain, _thread);
		Fl_Cursor act;
		if (kaction->type == kifString) {
			string action = kaction->String();
			if (cursors.find(action) == cursors.end())
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[290]);
			act = (Fl_Cursor)actions[action];
		}
		else
			act = (Fl_Cursor)kaction->Integer();

		window->cursor(act, kcolor1->Integer(), kcolor2->Integer());
		return kifTRUE;
	}

	KifElement* MethodPlot(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		KifElement* table = callfunc->Evaluatethread(0, domain, _thread);
		if (!table->aVectorContainer())
			return kifcode->Returnerror(KIFLTKSTRINGS[292]);

		if (table->Size() % 2 != 0)
			return kifcode->Returnerror(KIFLTKSTRINGS[293]);


		int thickness = callfunc->Evaluatethread(1, domain, _thread)->Integer();
		double x, y, a, b;
		double maxX, maxY, minX, minY;
		bool first = true;
		char xe = 0;
		double minx = 0;
		double miny = 0;
		double maxx = window->w();
		double maxy = window->h();
		double incx;
		double incy;

		char action = 0;

		KifElement* klandmark = NULL;
		if (callfunc->Size() == 3) {
			klandmark = callfunc->Evaluatethread(2, domain, _thread);
			if (!klandmark->aVectorContainer())
				return kifcode->Returnerror(KIFLTKSTRINGS[294]);
			if (klandmark->Size() >= 4) {
				minx = klandmark->getfloat(0);
				miny = klandmark->getfloat(1);
				maxx = klandmark->getfloat(2);
				maxy = klandmark->getfloat(3);
				if (klandmark->Size() >= 8) {
					action = 1;
					minX = klandmark->getfloat(4);
					minY = klandmark->getfloat(5);
					maxX = klandmark->getfloat(6);
					maxY = klandmark->getfloat(7);
					if (klandmark->Size() >= 10) {
						action = 2;
						incx = klandmark->getfloat(8);
						incy = klandmark->getfloat(9);
					}
				}
			}
		}

		KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
		int sz = table->Size();
		int i;
		if (!action) {
			maxX = 0; minX = 0; maxY = 0; minY = 0;
			for (i = 0; i < sz; i++) {
				if (first) {
					if (!xe) {
						maxX = table->getfloat(i);
						minX = table->getfloat(i);
					}
					else {
						minY = table->getfloat(i);
						maxY = table->getfloat(i);
						first = false;
					}
				}
				else {
					if (!xe) {
						minX = min(minX, table->getfloat(i));
						maxX = max(maxX, table->getfloat(i));
					}
					else {
						minY = min(minY, table->getfloat(i));
						maxY = max(maxY, table->getfloat(i));
					}
				}
				xe = 1 - xe;
			}
			if (maxX == minX || maxY == minY)
				return kvect;
		}

		first = true;
		xe = 0;

		kvect->storevalue(minx); //0
		kvect->storevalue(miny); //1
		kvect->storevalue(maxx); //2
		kvect->storevalue(maxy); //3

		if (thickness > 1) {
			minx += thickness;
			miny += thickness >> 1;
			maxx -= thickness;
			maxy -= (thickness << 1) - (thickness >> 1);
		}

		if (action != 2) {
			incx = (maxx - minx) / (maxX - minX);
			incy = (maxy - miny) / (maxY - minY);
		}
		for (i = 0; i < sz; i++) {
			if (!xe) {
				x = table->getfloat(i);;
				x = minx + incx*x - incx*minX;
			}
			else {
				y = table->getfloat(i);;
				y = miny + maxy - incy*y + incy*minY;
				if (!thickness) {
					if (!first)
						fl_line((int)a, (int)b, (int)x, (int)y);
					a = x;
					b = y;
					first = false;
				}
				else {
					if (thickness == 1)
						fl_point((int)x, (int)y);
					else
						fl_circle((int)x, (int)y, thickness);
				}
			}
			xe = 1 - xe;
		}

		kvect->storevalue(minX); //4
		kvect->storevalue(minY); //5
		kvect->storevalue(maxX); //6
		kvect->storevalue(maxY); //7
		kvect->storevalue(incx); //8
		kvect->storevalue(incy); //9
		kvect->storevalue((double)thickness); //10
		return kvect;
	}

	KifElement* PlotCoords(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		KifElement* kvect = callfunc->Evaluatethread(0, domain, _thread);
		if (kvect->Size() != 11)
			return kifcode->Returnerror(KIFLTKSTRINGS[295]);

		double x = callfunc->Evaluatethread(1, domain, _thread)->Float();
		double y = callfunc->Evaluatethread(2, domain, _thread)->Float();

		double minx = kvect->getfloat(0);
		double miny = kvect->getfloat(1);
		double maxy = kvect->getfloat(3);
		double minX = kvect->getfloat(4);
		double minY = kvect->getfloat(5);
		double incx = kvect->getfloat(8);
		double incy = kvect->getfloat(9);
		int thickness = kvect->getfloat(10);
		if (thickness > 1) {
			minx += thickness;
			miny += thickness >> 1;
			maxy -= (thickness << 1) - (thickness >> 1);
		}

		x = minx + incx*x - incx*minX;
		y = miny + maxy - incy*y + incy*minY;

		KifElement* kres = Selectvectorfloat(callfunc, contextualpattern);
		kres->storevalue(x);
		kres->storevalue(y);
		return kres;
	}

	KifElement* MethodModal(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		if (callfunc->Size() == 0) {
			if (window->modal() == 0)
				return kifFALSE;
			return kifTRUE;
		}

		bool m = callfunc->Evaluatethread(0, domain, _thread)->Boolean();
		if (m)
			window->set_modal();
		else
			window->set_non_modal();
		return kifTRUE;
	}

	KifElement* MethodBorder(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);
		if (end)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[287]);

		if (callfunc->Size() == 0) {
			if (window->border() == 0)
				return kifFALSE;
			return kifTRUE;
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		bool m = callfunc->Evaluatethread(0, domain, _thread)->Boolean();
		window->border(m);
		return kifTRUE;
	}

	KifElement* MethodImage(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		KifElement* kimage = callfunc->Evaluatethread(0, domain, _thread);
		if (kimage->type != image_type)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[296]);

		KifElement* kx = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* ky = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* kw = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* kh = callfunc->Evaluatethread(4, domain, _thread);
		Fl_Image* fl = ((Kifimage*)kimage)->image;
		if (fl == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[297]);
		if (!end) {
			Fl_Box* box = new Fl_Box(kx->Integer(), ky->Integer(), kw->Integer(), kh->Integer());
			box->image(fl);
			return kifTRUE;
		}

		fl->draw(kx->Integer(), ky->Integer(), kw->Integer(), kh->Integer());
		return kifTRUE;
	}

	KifElement* MethodBitmap(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		KifElement* kbitmap = callfunc->Evaluatethread(0, domain, _thread);
		if (kbitmap->type != bitmap_type)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[298]);

		KifElement* ke = callfunc->Evaluatethread(1, domain, _thread);
		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		int x = callfunc->Evaluatethread(2, domain, _thread)->Integer();
		int y = callfunc->Evaluatethread(3, domain, _thread)->Integer();
		int w = ((Kifbitmap*)kbitmap)->szw;
		int h = ((Kifbitmap*)kbitmap)->szh;

		if (callfunc->Size() == 6) {
			w = callfunc->Evaluatethread(4, domain, _thread)->Integer();
			h = callfunc->Evaluatethread(5, domain, _thread)->Integer();
		}

		Fl_Bitmap* fl = ((Kifbitmap*)kbitmap)->bitmap;
		if (fl == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[298]);
		if (!end) {
			Fl_Box* box = new Fl_Box(x, y, w, h);
			box->labelcolor(color);
			box->image(fl);
			return kifTRUE;
		}

		fl_color(color);
		fl->draw(x, y, w, h);
		return kifTRUE;
	}

	KifElement* MethodSizerange(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);
		if (end)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[287]);

		KifElement* minw = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* minh = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* maxw = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* maxh = callfunc->Evaluatethread(3, domain, _thread);
		window->size_range(minw->Integer(), minh->Integer(), maxw->Integer(), maxh->Integer());
		return kifTRUE;
	}

	KifElement* MethodMenu(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);
		if (end)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[287]);

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		//we expect a vector
		if (ke->aVectorContainer() == false)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[299]);
		KifElement* menu = ke;
		vector<Fl_Menu_Item*> elements;
		Fl_Menu_Item* fl;
		for (int i = 0; i < menu->Size(); i++) {
			//First level
			//each element is a vector
			ke = menu->ValueOnIndex(i);
			if (ke->aVectorContainer() == false)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[300]);
			KifElement* element = ke;
			//the first element should be the name of our element
			if (element->Size() != 0) {
				ke = element->ValueOnIndex(0);
				string nn = ke->String();
				char* n = strdup(STR(nn));

				fl = new Fl_Menu_Item;
				fl->text = n;
				fl->shortcut_ = 0;
				fl->callback_ = NULL;
				fl->user_data_ = 0;
				fl->flags = FL_SUBMENU;
				fl->labeltype_ = 0;
				fl->labelfont_ = 0;
				fl->labelsize_ = 12;
				fl->labelcolor_ = FL_BLACK;

				elements.push_back(fl);
				//then we analyse the sub-elements, the next element should be a vector
				for (int i = 1; i < element->Size(); i++) {
					if (element->ValueOnIndex(i)->aVectorContainer() == false)
						return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[300]);
					KifElement* v = element->ValueOnIndex(i);
					//First element should be the name
					if (v->Size() != 5)
						return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[301]);
					nn = v->ValueOnIndex(0)->String();
					n = strdup(STR(nn));
					//second element shoudl be a vector
					if (v->ValueOnIndex(1)->aVectorContainer() == false)
						return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[302]);
					long code = 0;
					KifElement* vv = v->ValueOnIndex(1);
					for (int j = 0; j < vv->Size(); j++) {
						KifElement* kcom = vv->ValueOnIndex(j);
						if (kcom->type == kifString) {
							string scom = kcom->String();
							if (commands.find(scom) != commands.end())
								code += commands[scom];
							else
								code += scom[0];
						}
						else
							code += kcom->Integer();
					}
					KifElement* function = v->ValueOnIndex(2);
					if (function->type != kifCall && !function->isFunction() && !testfunction(function))
						return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[303]);
					KifElement* object = v->ValueOnIndex(3);

					if (object->type == kifInstance)
						object = object->Thevariable(domain, _thread, NULL);
					else
						object = object->Exec(kifNULL, domain,kifNULL,_thread,callfunc);

					bool sep = v->ValueOnIndex(4)->Boolean();
					fl = new Fl_Menu_Item;
					fl->labeltype_ = 0;
					fl->labelfont_ = 0;
					fl->labelsize_ = 12;
					fl->labelcolor_ = FL_BLACK;
					fl->text = n;
					fl->shortcut_ = code;
					fl->callback_ = menu_callback;
					fl->user_data_ = new KifFLCallback(function, object, this, contextualpattern, domain);
					fl->flags = 0;
					if (sep == true)
						fl->flags = FL_MENU_DIVIDER;
					elements.push_back(fl);
				}
			}
			elements.push_back(NULL);
		}
		elements.push_back(NULL);
		flmenunbelements = elements.size();
		flmenu = new Fl_Menu_Item[flmenunbelements];
		int i;
		for (i = 0; i < flmenunbelements; i++) {
			if (elements[i] != NULL)
				flmenu[i] = *elements[i];
			else {
				flmenu[i].text = 0;
				flmenu[i].shortcut_ = 0;
				flmenu[i].callback_ = NULL;
				flmenu[i].user_data_ = 0;
				flmenu[i].flags = 0;
				flmenu[i].labeltype_ = 0;
				flmenu[i].labelfont_ = 0;
				flmenu[i].labelsize_ = 12;
				flmenu[i].labelcolor_ = FL_BLACK;
			}
			delete elements[i];
		}

		KifElement* x = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(4, domain, _thread);

		Fl_Menu_Bar *m = new Fl_Menu_Bar(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
		m->copy(flmenu);
		return kifTRUE;
	}

	KifElement* MethodEnd(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);
		if (end)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[287]);

		end = true;
		//Fl::unlock();
		window->end();
		window->show();
		current_window.pop_back();
		indraw = false;
		return kifTRUE;
	}

	KifElement* MethodClose(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		if (end == false)
			window->end();

		itemclose();
		KifDoubleWindow* wnd = window;
		wnd->window = NULL;
		window = NULL;
		end = false;
		delete wnd;
		return kifTRUE;
	}

	void Terminate() {

		if (window == NULL)
			return;

		if (end == false)
			window->end();
		end = false;

		if (window != NULL)
			delete window;
	}

	KifElement* MethodAsk(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		int a;
		string msg = callfunc->Evaluatethread(0, domain, _thread)->String();
		string msg1 = callfunc->Evaluatethread(1, domain, _thread)->String();
		string msg2 = callfunc->Evaluatethread(2, domain, _thread)->String();
		if (callfunc->Size() == 3)
			a = fl_choice(msg.c_str(), msg1.c_str(), msg2.c_str(), NULL);
		else {
			string msg3 = callfunc->Evaluatethread(3, domain, _thread)->String();
			if (callfunc->Size() == 4)
				a = fl_choice(msg.c_str(), msg1.c_str(), msg2.c_str(), msg3.c_str());
			else {
				string msg4 = callfunc->Evaluatethread(4, domain, _thread)->String();
				a = fl_choice(msg.c_str(), msg1.c_str(), msg2.c_str(), msg3.c_str(), msg4.c_str());
			}
		}
		return new KifInteger(kifcode, NULL, a);
	}

	KifElement* MethodAlert(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		string msg = callfunc->Evaluatethread(0, domain, _thread)->String();
		fl_alert(msg.c_str());
		return kifTRUE;
	}

	KifElement* MethodPassword(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		string msg = callfunc->Evaluatethread(0, domain, _thread)->String();
		const char* pass = fl_password(msg.c_str());
		if (pass == NULL)
			return kifNULL;
		return kifcode->Providestringraw(pass);
	}

	KifElement* MethodInput(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		string msg;
		if (callfunc->Size() == 1)
			msg = callfunc->Evaluatethread(0, domain, _thread)->String();
		else
			msg = KIFLTKSTRINGS[2];

		const char* ret = fl_input(msg.c_str());
		if (ret == NULL)
			return kifNULL;
		return kifcode->Providestringraw(ret);
	}

	KifElement* MethodResizable(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);
		if (end == false)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[304]);

		if (callfunc->Size() == 0) {
			window->resizable(NULL);
			return kifTRUE;
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->Yourowntype() == KIFWIDGET) {
			Fl_Widget* fl = ((KifWidget*)ke)->Widget();
			if (fl == NULL)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[305]);
			window->resizable(fl);
			return kifTRUE;
		}

		return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[306]);
	}


	KifElement* MethodRun(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (window != NULL) {
			if (end == false) {
				end = true;
				window->end();
				window->show();
			}
			mainwindow = this;
			Fltkrun();
			mainwindow = NULL;
			return kifTRUE;
		}
		return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);
	}


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		KiFArityError(name)
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};

//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifwindow::kifexportedmethods;
map<short, windowMethod>  Kifwindow::linkedmethods;
map<string, string> Kifwindow::infomethods;
hmap<short, bool> Kifwindow::methodassigned;

//-----------------------------------------------------------------------------------------------------
#define FOREVER 1e05
#ifdef MACUI
void fltk_reinit() {
	Fl::lock();
	Fl::awake_ring_head_ = 0;
	Fl::awake_ring_tail_ = 0;
	Fl::unlock();
}
#endif


static int Fltkrun() {
#ifdef MACUI
	fltk_reinit();
#endif

	Fl::lock();

#ifdef WIN32
	Fl::unlock();
	Flresetmainthread();
	Fl::lock();
#endif

	int tt = 1;
	stopall = false;

#ifdef UNIX	
	while (!stopall) {
#else
	while (!stopall && tt != 0) {
#endif
		if (Errorid(0))
			stopall = true;
		else {
			LaunchWindowCall();
			tt = Fl::wait(FOREVER);
		}
	}

	Fl::unlock();

	stopall = true;
	if (mainwindow != NULL)
		mainwindow->Terminate();
	windowmode = false;
	for (int i = 0; i < kifGlobals->garbage.size(); i++) {
		if (kifGlobals->garbage[i] != NULL && kifGlobals->garbage[i]->Yourowntype() == KIFWIDGET)
			((KifWidget*)kifGlobals->garbage[i])->ResetWidget();
	}

	CleanMessages();
	return 0;
}

//-----------------------------------------------------------------------------------------------------

KifDoubleWindow::KifDoubleWindow(int x, int y, const char* l, KifElement* f, KifElement* o, KifElement* c, KifDomain* d, Kifwindow* wn) : ready(false), iwindow(-1), Fl_Double_Window(x, y, l) {
	function = f;
	dragdrop = false;
	object = o;
	context = c;
	dom = d;
	dom->Setreference();
	context->Setreference();
	idom = dom->Ingarbage();
	icontext = context->Ingarbage();
	window = wn;
	iwindow = window->Ingarbage();
	kifcode = dom->kifcode;
}

KifDoubleWindow::KifDoubleWindow(int x, int y, int w, int h, const char* l, KifElement* f, KifElement* o, KifElement* c, KifDomain* d, Kifwindow* wn) : ready(false), iwindow(-1), Fl_Double_Window(x, y, w, h, l) {
	dragdrop = false;
	function = f;
	object = o;
	context = c;
	dom = d;
	idom = dom->Ingarbage();
	icontext = context->Ingarbage();
	window = wn;
	iwindow = window->Ingarbage();
	kifcode = dom->kifcode;
	dom->Setreference();
	context->Setreference();
}

KifDoubleWindow::~KifDoubleWindow() {
	ThreadLock _lock(window_type);
	kifGlobals->ResetGarbageElement(dom, idom);
	kifGlobals->ResetGarbageElement(context, icontext);
	if (window != NULL) {
		//already in deletion
		window->itemclose();
		window->window = NULL;
	}
}

void KifDoubleWindow::draw() {
	if (kifGlobals->garbage[iwindow] == window) {
		if (window->update)
			Fl_Double_Window::draw();
	}

	if (stopall)
		return;

	if (function != NULL) {
#ifndef WIN32
		ThreadLock _lock(kifDISPLAY);
#endif
		window->setindraw(true);
		KifFunction* func = function->Functioncall(dom);
		KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
		kfunc.kifcode = func->kifcode;
		threadblocinit;
		Idcouple kwnd(window, true);
		Idcouple kobj(object, true);
		kfunc.parameters.push_back(window);
		kfunc.parameters.push_back(object);
		kfunc.Exec(context, dom, kifNULL, idthread, callfunc);
		kwnd.Reset();
		kobj.Reset();
		window->setindraw(false);
	}
	ready = true;
}

int KifDoubleWindow::handle(int e) {
	if (stopall || !ready || kifGlobals->Garbagefind(iwindow) == NULL || kifGlobals->Garbageexist(iwindow, window) == false)
		return 1;
	switch (e) {
	case FL_DND_ENTER:
	case FL_DND_RELEASE:
	case FL_DND_LEAVE:
	case FL_DND_DRAG:
		dragdrop = true;
		return 1;
	case FL_PASTE:
		if (dragdrop) {
			eventtext = Fl::event_text();
			dragdropcall_back();
		}
		dragdrop = false;
		return 1;
	case FL_PUSH:
	case FL_RELEASE:
	case FL_DRAG:
	case FL_MOVE:
	case FL_MOUSEWHEEL:
		if (mouse_callback(e) == 1)
			return 1;
		break;
	case FL_KEYDOWN:
	case FL_KEYUP:
		if (key_callback(e) == 1)
			return 1;
		break;
	}
	if (stopall)
		return 1;
	return Fl_Double_Window::handle(e);
}

int KifDoubleWindow::dragdropcall_back() {
	if (functiondragdrop.Function() == NULL)
		return 0;

	static bool encours = false;
	if (encours == true)
		return false;
	encours = true;

	KifFunction* func = functiondragdrop.Function()->Functioncall(dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	KifString* kstr = func->kifcode->Providestring(eventtext);
	kstr->Setreference();
	Idcouple thewnd(window, true);
	kfunc.parameters.push_back(window);
	kfunc.parameters.push_back(kstr);
	kfunc.parameters.push_back(objectdragdrop.Object());
	KifElement* ret = kfunc.Exec(context, dom, kifNULL, idthread, callfunc);
	kstr->Resetreference();
	thewnd.Reset();
	encours = false;

	return ret->Boolean();
}

int KifDoubleWindow::mouse_callback(int e) {
	if (functionmouse.find(e) == functionmouse.end())
		return 0;

	static bool encours = false;
	if (encours == true)
		return false;
	encours = true;

	KifMapStringInteger* kmouse = new KifMapStringInteger(kifcode, NULL);
	kmouse->values[KIFLTKSTRINGS[307]] = Fl::event_button();
	kmouse->values[KIFLTKSTRINGS[308]] = Fl::event_dx();
	kmouse->values[KIFLTKSTRINGS[309]] = Fl::event_dy();
	kmouse->values["x"] = Fl::event_x();
	kmouse->values["y"] = Fl::event_y();
	kmouse->values[KIFLTKSTRINGS[310]] = Fl::event_x_root();
	kmouse->values[KIFLTKSTRINGS[311]] = Fl::event_y_root();

	KifFunction* func = functionmouse[e].Function()->Functioncall(dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	Idcouple thewnd(window, true);
	kmouse->Setreference();
	Idcouple themouse(objectmouse[e].o, true);
	kfunc.parameters.push_back(window);
	kfunc.parameters.push_back(kmouse);
	kfunc.parameters.push_back(objectmouse[e].Object());

	KifElement* ret = kfunc.Exec(context, dom, kifNULL, idthread, callfunc);
	thewnd.Reset();
	kmouse->Resetreference();
	themouse.Reset();

	encours = false;
	return ret->Boolean();
}

int KifDoubleWindow::key_callback(int e) {
	if (functionkey.find(e) == functionkey.end())
		return 0;

	KifFunction* func = functionkey[e].Function()->Functioncall(dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;


	Idcouple thewnd(window, true);
	Idcouple thekey(objectkey[e].o, true);

	KifElement* ks = kifcode->Providestringraw(Fl::event_text());
	KifElement* ki = kifcode->Provideinteger(Fl::event_key());
	KifInteger* kcombine = kifcode->Provideinteger(0);
	if (Fl::event_ctrl())
		kcombine->value |= 1;
	if (Fl::event_alt())
		kcombine->value |= 2;
	if (Fl::event_command())
		kcombine->value |= 4;
	if (Fl::event_shift())
		kcombine->value |= 8;
	kcombine->Setreference();
	ks->Setreference();
	ki->Setreference();
	kfunc.parameters.push_back(window);
	kfunc.parameters.push_back(ks);
	kfunc.parameters.push_back(ki);
	kfunc.parameters.push_back(kcombine);
	kfunc.parameters.push_back(objectkey[e].Object());
	KifElement* ret = kfunc.Exec(context, dom, kifNULL, idthread, callfunc);

	thewnd.Reset();
	thekey.Reset();
	ki->Resetreference();
	ks->Resetreference();
	kcombine->Resetreference();
	return ret->Boolean();
}

void window_termination(Kifwindow* kwnd) {
	ThreadLock _lock(window_type);
	kwnd->closed = true;
	delete kwnd->window;
}


void close_callback(Fl_Widget *w, void *data) {
	ThreadLock _lock(window_type);
	Kifwindow* kwnd = (Kifwindow*)data;
	kwnd->closed = true;
	delete kwnd->window;
}

void window_callback(Fl_Widget *w, void *data) {
	ThreadLock _lock(window_type);
	KifFLCallback* wn = (KifFLCallback*)data;
	Kifwindow* kwnd = wn->window;
	KifFunction* func = wn->function->Functioncall(wn->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	kwnd->Setreference();
	wn->object->Setreference();
	kfunc.parameters.push_back(kwnd);
	kfunc.parameters.push_back(wn->object);
	KifElement* ret = kfunc.Exec(wn->context, wn->dom, kifNULL, idthread, callfunc);
	kwnd->Resetreference();
	wn->object->Resetreference();

	if (ret->Boolean() == false)
		return;

	kwnd->closed = true;
	delete kwnd->window;
}


void timeout_callback(void *data) {
	if (stopall)
		return;

	KifFLCallback* wn = (KifFLCallback*)data;
	Kifwindow* kwnd = wn->window;
	KifFunction* func = wn->function->Functioncall(wn->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	kwnd->Setreference();
	wn->object->Setreference();
	kfunc.parameters.push_back(kwnd);
	kfunc.parameters.push_back(wn->object);
	KifElement* ret = kfunc.Exec(wn->context, wn->dom, kifNULL, idthread, callfunc);
	kwnd->Resetreference();
	wn->object->Resetreference();

	double i = ret->Float();
	ret->Release();
	if (i <= 0 || stopall)
		return;
	Fl::repeat_timeout(i, timeout_callback, data);
}

void menu_callback(Fl_Widget *w, void *data) {
	KifFLCallback* wn = (KifFLCallback*)data;
	Kifwindow* kwnd = wn->window;
	KifFunction* func = wn->function->Functioncall(wn->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	kwnd->Setreference();
	wn->object->Setreference();
	kfunc.parameters.push_back(kwnd);
	kfunc.parameters.push_back(wn->object);
	kfunc.Exec(wn->context, wn->dom, kifNULL, idthread, callfunc);
	kwnd->Resetreference();
	wn->object->Resetreference();
}


static void input_callback(Fl_Widget *w, void *data);

class Kifinput : public KifWidgetCallback {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, inputMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Input* input;
	char txt[1024];


	bool multiline;
	string buf;

	Fl_Widget* Widget() {
		return input;
	}

	void ResetWidget() {
		dom = NULL;
		input = NULL;
	}
	//---------------------------------------------------------------------------------------------------------------------
	Kifinput(KifCode* kifcode, KifElement* base) : KifWidgetCallback(kifcode, base, input_type) {
		//Do not forget your variable initialisation
		input = NULL;
		multiline = false;
	}

	void itemclose() {
		if (function != NULL)
			return;
		input = NULL;
		multiline = false;
		clean();
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifinput* kf = new Kifinput(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	KifElement* CallbackObject() {
		return object;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call

	//-----------------------------------------------------------------------------------------------------------------------


	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* w = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* h = callfunc->Evaluatethread(3, domain, _thread);
		multiline = callfunc->Evaluatethread(4, domain, _thread)->Boolean();
		KifElement* label = callfunc->Evaluatethread(5, domain, _thread);

		if (input != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[312]);

		string thetxt = label->String();
		if (thetxt.size() < 1024)
			strcpy_s(txt, 1024, thetxt.c_str());
		{
			ThreadLock _lock(type);
			if (multiline)
				input = new Fl_Multiline_Input(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
			else
				input = new Fl_Input(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
		}

		if (function != NULL) {
			Keeptrack(domain, contextualpattern);
			input->callback(input_callback, this);
		}

		SaveItem(this);
		return kifTRUE;
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (input == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[313]);

		if (callfunc->Size() == 0) {
			buf = input->value();
			return kifcode->Providestring(buf);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		buf = callfunc->Evaluatethread(0, domain, _thread)->String();
		input->value(buf.c_str());
		return kifTRUE;
	}

	KifElement* MethodSelection(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (input == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[313]);

		int pos = input->position();
		int nb = input->mark();
		buf = input->value();
		buf = buf.substr(pos, nb);
		return kifcode->Providestring(buf);
	}

	KifElement* MethodColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (input == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[313]);

		if (callfunc->Size() == 0) {
			Fl_Color color = input->textcolor();
			return kifcode->Provideinteger(color);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			input->textcolor(colors[buf]);
		}
		else {
			unsigned int c = ke->Integer();
			input->textcolor(c);
		}

		return kifTRUE;
	}

	KifElement* MethodFont(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (input == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[313]);

		if (callfunc->Size() == 0) {
			int i = input->textfont();
			if (ifonts.find(i) != ifonts.end())
				return kifcode->Providestring(ifonts[i]);
			return kifcode->Provideinteger(i);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		if (ke->type == kifString) {
			buf = ke->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = KIFLTKSTRINGS[275];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}

			input->textfont(fonts[buf]);
		}
		else {
			int font = ke->Integer();
			if (font >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[263]);
			input->textfont(font);
		}
		return kifTRUE;
	}

	KifElement* MethodFontSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (input == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[313]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(input->textsize());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		input->textsize(ke->Integer());
		return kifTRUE;
	}

	KifElement* MethodWord(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (input == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[313]);

		//first we get our position
		int pos = callfunc->Evaluatethread(0, domain, _thread)->Integer();
		//then we try to extract the word string
		int start = pos;
		const char* ibuf = input->value();
		string res;
		while (start > 0 && c_is_alpha((uchar*)ibuf, pos)) {
			start--;
			pos = start;
		}
		pos = ++start;
		while (pos <= input->size() && c_is_alpha((uchar*)ibuf, start)) {
			res += c_char_get((uchar*)ibuf, pos);
			pos++;
			start = pos;
		}
		return kifcode->Providestring(res);
	}

	KifElement* MethodInsert(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (input == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[313]);

		buf = callfunc->Evaluatethread(0, domain, _thread)->String();
		KifElement* pos = callfunc->Evaluatethread(1, domain, _thread);
		input->insert(buf.c_str(), pos->Integer());
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
		if (input == NULL)
			return this;

		if (idx == NULL || idx == kifNULL || contextualpattern == idx)
			return this;

		KifElement* ke;
		KifElement* left = idx;
		KifElement* right = NULL;
		if (idx->type == kifIndex) {
			KifIndex* kind = (KifIndex*)idx;
			left = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			if (kind->interval == true)
				right = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		}

		ThreadLock _lock(type);
		string value = input->value();

		int sz = value.size();

		register int ikey;
		if (left->type == kifString) {
			//then we are looking for a substring
			ikey = value.find(left->String(), 0);
			if (ikey == -1)
				return kifNULL;
		}
		else {
			ikey = left->Integer();
			if (ikey < 0)
				ikey += size_c(value);
			ikey = c_chartobyteposition(USTR(value), ikey);
			if (ikey < 0 || ikey >= sz)
				return kifNULL;
		}

		if (right == NULL) {
			ke = kifcode->Providestringraw("");
			((KifString*)ke)->value = c_char_get(USTR(value), ikey);
			return ke;
		}
		int iright;
		if (right->type == kifString) {
			iright = value.find(right->String(), ikey + 1);
			if (iright == -1)
				return kifNULL;
		}
		else {
			iright = right->Integer();

			if (iright <= 0 || right == kifNULL)
				iright += size_c(value);
			iright = c_chartobyteposition(USTR(value), iright);
			if (iright > sz)
				iright = sz;
		}
		if ((iright - ikey) <= 0)
			return kifNULL;

		return kifcode->Providestringraw(value.substr(ikey, iright - ikey));
	}

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }
};
map<short, KifCallMethod> Kifinput::kifexportedmethods;
map<short, inputMethod>  Kifinput::linkedmethods;
map<string, string> Kifinput::infomethods;
hmap<short, bool> Kifinput::methodassigned;

void input_callback(Fl_Widget *w, void *data) {
	Kifinput* input = (Kifinput*)data;
	KifFunction* func = input->function->Functioncall(input->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	input->Setreference();
	input->object->Setreference();

	kfunc.parameters.push_back(input);
	kfunc.parameters.push_back(input->object);
	kfunc.Exec(input->context, input->dom, kifNULL, idthread, callfunc);

	input->Resetreference();
	input->object->Resetreference();

	//input->input = NULL;
	//input->multiline = false;
	//input->clean();
}

class KifTextBuf : public Fl_Text_Buffer {
public:
	char* wbuffer;
	int internalsize;

	KifTextBuf() : Fl_Text_Buffer() {
		internalsize = 1024;
		wbuffer = new char[1024];
		canUndo(1);
	}

	~KifTextBuf() {
		delete[] wbuffer;
	}

	void setstyle(int start, int end, uchar style) {
		int l = end - start;
		if (l >= internalsize) {
			delete[] wbuffer;
			internalsize = l + 100;
			wbuffer = new char[internalsize];
		}
		memset(wbuffer, style, l);
		wbuffer[l] = 0;
		replace(start, end, wbuffer);
	}

	char getstyle(int pos) {
		if (pos < 0 || pos >= length())
			return 'A';
		return byte_at(pos);
	}

	unsigned char* internalbuffer() {
		if (internalsize < length()) {
			delete[] wbuffer;
			internalsize = length() + 100;
			wbuffer = new char[internalsize];
		}
		char* t = text();
		strcpy(wbuffer, t);
		free(t);
		return (uchar*)wbuffer;
	}

	string subrange(int s, int e) {
		if (s > length())
			return "";
		string code;
		char* str = text_range(s, e);
		code = str;
		free(str);
		return code;
	}
};

//
// 'style_update()' - Update the style buffer...
//


class KifTextEditor : public Fl_Text_Editor {
public:
	bool modified;
	KifElement* functionvscroll;
	KifElement* objectvscroll;
	KifElement* functionhscroll;
	KifElement* objecthscroll;
	KifElement* hcontext;
	KifDomain* hdom;
	KifElement* vcontext;
	KifDomain* vdom;
	Kifeditor* editor;
	map<int, Idcouple> functionmouse;
	map<int, Idcouple> functionkey;
	map<int, Idcouple> objectmouse;
	map<int, Idcouple> objectkey;
	KifElement* mousecontext;
	KifDomain* mousedom;

	static void loc_v_scrollbar_cb(Fl_Scrollbar* b, KifTextEditor* textD);
	static void loc_h_scrollbar_cb(Fl_Scrollbar* b, KifTextEditor* textD);

	KifTextEditor(Kifeditor* e, int x, int y, int w, int h, const char* txt) : Fl_Text_Editor(x, y, w, h, txt) {
		modified = true;
		editor = e;
		mVScrollBar->callback((Fl_Callback*)loc_v_scrollbar_cb, this);
		mHScrollBar->callback((Fl_Callback*)loc_h_scrollbar_cb, this);
		functionvscroll = NULL;
		objectvscroll = kifNULL;
		functionhscroll = NULL;
		objecthscroll = kifNULL;
	}


	int Firstchar() {
		return mFirstChar;
	}

	int Line(int pos) {
		int linenum = -1;
		position_to_line(pos, &linenum);
		linenum += get_absolute_top_line_number();
		return linenum;
	}

	int Topline() {
		return mTopLineNum;
	}

	int PositionFromLine(Fl_Text_Buffer* buf, int maxLines) {
		int retPos;
		int retLines;
		int retLineStart;
		int retLineEnd;
		wrapped_line_counter(buf, 0, buf->length(), maxLines, true, 0, &retPos, &retLines, &retLineStart, &retLineEnd, true);
		return retPos;
	}

	int mouse_callback(int e);
	int key_callback(int e);

	bool Setmouseaction(int e, KifElement* f, KifElement* object) {
		if (!f->isFunction() || f->Size() != 3 || f->Argument(0)->VariableValue()->inType() != editor_type)
			return false;
		switch (e) {
		case FL_PUSH:
		case FL_RELEASE:
		case FL_MOVE:
		case FL_DRAG:
		case FL_MOUSEWHEEL:
			functionmouse[e].Use(f);
			objectmouse[e].Use(object);
			return true;
		}
		return false;
	}

	bool Setkeyaction(int e, KifElement* f, KifElement* object) {
		if (!f->isFunction() || f->Size() != 5 || f->Argument(0)->VariableValue()->inType() != editor_type)
			return false;
		if (e != FL_KEYDOWN && e != FL_KEYUP)
			return false;
		functionkey[e].Use(f);
		objectkey[e].Use(object);
		return true;
	}

	int handle(int e);
	void draw_cursor(int X, int Y);
};

struct Stylearg {
	KifTextEditor* editor;
	Fl_Text_Buffer *stylebuf;
	char currentstyle;
};

void style_update(int pos,          // I - Position of update
	int        nInserted,    // I - Number of inserted chars
	int        nDeleted,     // I - Number of deleted chars
	int        nRestyled,    // I - Number of restyled chars
	const char *deletedText, // I - Text that was deleted
	void       *stylarg) {     // I - Callback data



	Fl_Text_Buffer *stylebuf = ((struct Stylearg*)stylarg)->stylebuf;
	KifTextEditor* editor = ((struct Stylearg*)stylarg)->editor;
	unsigned char currentstyle = 'A';

	// If this is just a selection change, just unselect the style buffer...
	if (nInserted == 0 && nDeleted == 0) {
		stylebuf->unselect();
		editor->modified = false;
		return;
	}

	editor->modified = true;
	// Just delete characters in the style buffer...
	if (nDeleted > 0)
		stylebuf->remove(pos, pos + nDeleted);

	// Track changes in the text buffer...
	if (nInserted > 0) {
		// Insert characters into the style buffer...
		if (pos > 0) {
			char* cstyle = stylebuf->text_range(pos - 1, pos);
			currentstyle = cstyle[0];
			free(cstyle);
		}
		char* style = new char[nInserted + 1];
		memset(style, currentstyle, nInserted);
		style[nInserted] = '\0';
		stylebuf->replace(pos, pos + nDeleted, style);
		delete[] style;
	}
}

static void editor_callback(int pos, int nInserted, int nDeleted, int nRestyled, const char* deletedText, void* cbArg);




class Kifeditor : public KifWidgetCallback {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, editorMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	KifTextEditor* editor;

	string buf;
	int nbstyle;
	KifTextBuf* textbuf;
	KifTextBuf* stylebuf;
	string lowercase;
	struct Stylearg stylearg;
	map<string, string> stylekeys;
	map<string, string> keystyles;
	map<string, string> messagestyle;
	map<string, int> messagecolor;
	Fl_Text_Display::Style_Table_Entry* styletable;
	Fl_Double_Window* sticky;
	char txt[1024];

	Fl_Widget* Widget() {
		return editor;
	}

	void ResetWidget() {
		dom = NULL;
		editor = NULL;
	}
	//---------------------------------------------------------------------------------------------------------------------
	Kifeditor(KifCode* kifcode, KifElement* base) : KifWidgetCallback(kifcode, base, editor_type) {
		//Do not forget your variable initialisation
		editor = NULL;
		stylebuf = NULL;
		nbstyle = 0;
		styletable = NULL;
		sticky = NULL;
	}

	void itemclose() {
		editor = NULL;
		clean();
	}


	~Kifeditor() {
		if (styletable != NULL)
			delete[] styletable;
		if (sticky != NULL) {
			Fl_Double_Window* wnd = sticky;
			sticky = NULL;
			delete wnd;
		}
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifeditor* kf = new Kifeditor(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* w = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* h = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* label = callfunc->Evaluatethread(4, domain, _thread);

		if (editor != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[314]);

		string thetxt = label->String();
		if (thetxt.size() < 1024)
			strcpy_s(txt, 1024, thetxt.c_str());
		{
			ThreadLock _lock(type);
			editor = new KifTextEditor(this, x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
			textbuf = new KifTextBuf();
			editor->buffer(textbuf);
		}

		if (function != NULL) {
			if (function->Size() != 7)
				return kifcode->Returnerror(KIFLTKSTRINGS[315]);
			Keeptrack(domain, contextualpattern);
			textbuf->add_modify_callback(editor_callback, this);
		}

		SaveItem(this);
		return kifTRUE;
	}

	KifElement* MethodOnVScroll(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[316]);


		KifElement* function = callfunc->Evaluatethread(0, domain, _thread);

		if (function->type != kifCall && !function->isFunction())
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[286]);

		KifElement* object = callfunc->parameters[1];

		if (object->type == kifInstance)
			object = object->Thevariable(domain, _thread, NULL);
		else
			object = object->Exec(kifNULL, domain,kifNULL,_thread,callfunc);
		editor->functionvscroll = function;
		editor->objectvscroll = object;
		editor->vdom = domain;
		editor->vcontext = contextualpattern;
		return kifTRUE;
	}

	KifElement* MethodOnHScroll(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[316]);


		KifElement* function = callfunc->Evaluatethread(0, domain, _thread);

		if (function->type != kifCall && !function->isFunction())
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[286]);

		KifElement* object = callfunc->parameters[1];

		if (object->type == kifInstance)
			object = object->Thevariable(domain, _thread, NULL);
		else
			object = object->Exec(kifNULL, domain,kifNULL,_thread,callfunc);

		editor->functionhscroll = function;
		editor->objecthscroll = object;
		editor->hdom = domain;
		editor->hcontext = contextualpattern;

		return kifTRUE;
	}

	KifElement* MethodTextMeasure(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[253]);

		KifElement* t = callfunc->Evaluatethread(0, domain, _thread);

		string buf = t->String();
		int w;
		int h;
		fl_measure(buf.c_str(), w, h, 1);
		KifMapStringInteger* map = new KifMapStringInteger(kifcode, NULL);
		map->values[KIFLTKSTRINGS[274]] = w;
		map->values["H"] = h;
		return map;
	}

	KifElement* MethodOnMouseClick(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[316]);

		KifElement* kaction = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* function = callfunc->Evaluatethread(1, domain, _thread);

		KifElement* object = callfunc->parameters[2];

		if (object->type == kifInstance)
			object = object->Thevariable(domain, _thread, NULL);
		else
			object = object->Exec(kifNULL, domain,kifNULL,_thread,callfunc);

		int act;
		if (kaction->type == kifString) {
			string action = kaction->String();
			if (actions.find(action) == actions.end())
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[288]);
			act = actions[action];
		}
		else
			act = kaction->Integer();

		if (editor->Setmouseaction(act, function, object) == false)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[289]);

		editor->mousedom = domain;
		editor->mousecontext = contextualpattern;

		return kifTRUE;
	}

	KifElement* MethodOnKey(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[316]);


		KifElement* kaction = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* function = callfunc->Evaluatethread(1, domain, _thread);

		KifElement* object = callfunc->parameters[2];

		if (object->type == kifInstance)
			object = object->Thevariable(domain, _thread, NULL);
		else
			object = object->Exec(kifNULL, domain,kifNULL,_thread,callfunc);

		int act;
		if (kaction->type == kifString) {
			string action = kaction->String();
			if (actions.find(action) == actions.end())
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[288]);
			act = actions[action];
		}
		else
			act = kaction->Integer();

		if (editor->Setkeyaction(act, function, object) == false)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[317]);

		editor->mousedom = domain;
		editor->mousecontext = contextualpattern;
		return kifTRUE;
	}

	bool InitializeItemMenu(string ckey, KifElement* kvect, unsigned char* key, int& ikey) {
		if (kvect->Size()<3 || kvect->Size()>5) {
			buf = KIFLTKSTRINGS[318];
			return false;
		}

		while (key[ikey] == '0') ikey++;

		string skey;
		skey = key[ikey];
		KifElement* kcolor = kvect->ValueOnIndex(0);
		KifElement* kfont = kvect->ValueOnIndex(1);
		KifElement* ksize = kvect->ValueOnIndex(2);
		string message = "";

		if (kvect->Size() >= 4)
			message = kvect->ValueOnIndex(3)->String();

		long font;
		if (kfont->type == kifString) {
			buf = kfont->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = KIFLTKSTRINGS[275];
				return(false);
			}
			font = fonts[buf];
		}
		else
			font = kfont->Integer();

		if (font >= fontmax) {
			buf = KIFLTKSTRINGS[263];
			return false;
		}

		Fl_Color color;
		if (kcolor->type == kifString) {
			//0 is the first parameter and so on...
			buf = kcolor->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return false;
			}
			color = colors[buf];
		}
		else
			color = kcolor->Integer();

		Fl_Color localcolor = color;
		if (kvect->Size() == 5) {
			kcolor = kvect->ValueOnIndex(4);
			if (kcolor->type == kifString) {
				//0 is the first parameter and so on...
				buf = kcolor->String();
				if (colors.find(buf) == colors.end()) {
					buf = KIFLTKSTRINGS[256];
					return false;
				}
				localcolor = colors[buf];
			}
			else
				localcolor = kcolor->Integer();
		}

		int i;
		if (stylekeys.find(ckey) != stylekeys.end()) {
			skey = stylekeys[ckey];
			i = skey[0] - 'A';
		}
		else {
			if (key[ikey] == 0) {
				buf = KIFLTKSTRINGS[319];
				return false;
			}

			stylekeys[ckey] = skey;
			keystyles[skey] = ckey;
			i = ikey;
			key[ikey++] = '0';
		}

		if (i >= nbstyle) {
			Fl_Text_Display::Style_Table_Entry* prev = styletable;
			styletable = new Fl_Text_Display::Style_Table_Entry[i + 1];
			//first we copy the previous style...
			int p;
			for (p = 0; p < nbstyle; p++)
				styletable[p] = prev[p];

			delete[] prev;
			//second we initialize the intermediary style...
			for (p = nbstyle; p < i; p++) {
				styletable[p].color = 56;
				styletable[p].font = 0;
				styletable[p].size = 12;
				styletable[p].attr = 0;
			}
			nbstyle = i + 1;
		}

		styletable[i].color = color;
		styletable[i].font = font;
		styletable[i].size = ksize->Integer();
		styletable[i].attr = 0;
		//a message can be associated with the style
		if (message != "") {
			messagestyle[skey] = message;
			messagecolor[skey] = localcolor;
		}
		return true;
	}

	KifElement* MethodAddStyles(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		bool first = true;
		if (stylebuf != NULL)
			first = false;

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->aMapContainer() == false)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[321]);

		int key;
		KifElement* kmap = ke;
		nbstyle = kmap->Size();
		if (nbstyle < stylekeys.size())
			nbstyle = stylekeys.size();

		if (nbstyle == 0)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[322]);

		if (first)
			stylebuf = new KifTextBuf();
		else
			delete[] styletable;

		styletable = new Fl_Text_Display::Style_Table_Entry[nbstyle];
		for (key = 0; key < nbstyle; key++) {
			styletable[key].color = 56;
			styletable[key].font = 0;
			styletable[key].size = 12;
			styletable[key].attr = 0;
		}


		string skey = "#";
		ke = kmap->ValueOnKey(skey);
		if (ke == kifNULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[323]);

		if (ke->aVectorContainer() == false)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[324]);

		unsigned char allowedkeys[60];
		unsigned char c;
		for (c = 'A'; c <= 'z'; c++)
			allowedkeys[c - 'A'] = c;
		allowedkeys[c - 'A'] = 0;
		key = 0;

		if (!first) {
			//We remove the non active keys
			map<string, string>::iterator ikey;
			map<string, string> removes;
			for (ikey = stylekeys.begin(); ikey != stylekeys.end(); ikey++) {
				if (kmap->ValueOnKey(ikey->first) == kifNULL)
					removes[ikey->first] = ikey->second;
			}

			for (ikey = removes.begin(); ikey != removes.end(); ikey++) {
				stylekeys.erase(ikey->first);
				keystyles.erase(ikey->second);
				messagestyle.erase(ikey->second);
				messagecolor.erase(ikey->second);
			}

			unsigned char base = 'B';

			for (ikey = stylekeys.begin(); ikey != stylekeys.end(); ikey++) {
				if (ikey->first == "#")
					ikey->second[0] = 'A';
				else {
					ikey->second[0] = base;
					c = base - 'A';
					allowedkeys[c] = '0';
					base++;
				}
			}
		}

		if (!InitializeItemMenu(skey, ke, allowedkeys, key))
			return Returningerror(kifcode, _thread, idcallthread, buf);
		allowedkeys[0] = '0';
		key = 1;
		KifIteration* itr = (KifIteration*)kmap->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			if (itr->IteratorKeyString() == "#")
				continue;
			ke = itr->IteratorValue();
			if (ke->aVectorContainer() == false) {
				ke->Release();
				itr->Release();
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[324]);
			}

			if (!InitializeItemMenu(itr->IteratorKeyString(), ke, allowedkeys, key)) {
				ke->Release();
				itr->Release();
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			ke->Release();
		}
		itr->Release();


		editor->highlight_data(stylebuf, styletable, nbstyle, 'A', NULL, 0);
		stylearg.editor = editor;
		stylearg.stylebuf = stylebuf;
		stylearg.currentstyle = 'A';
		if (first)
			textbuf->add_modify_callback(style_update, &stylearg);
		else
			editor->redraw();
		return kifTRUE;
	}

	KifElement* MethodLoad(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		//we need:
		string f = callfunc->Evaluatethread(0, domain, _thread)->String();
		textbuf->loadfile(STR(f));
		return kifTRUE;
	}

	KifElement* MethodSave(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		//we need:
		string f = callfunc->Evaluatethread(0, domain, _thread)->String();
		textbuf->savefile(STR(f));
		return kifTRUE;
	}

	KifElement* MethodReplace(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		int i = 0;
		//we need:
		string sfind = callfunc->Evaluatethread(0, domain, _thread)->String();
		string rep = callfunc->Evaluatethread(1, domain, _thread)->String();
		int matchcase = 0;
		if (callfunc->Size() >= 3)
			i = callfunc->Evaluatethread(2, domain, _thread)->Integer();
		if (callfunc->Size() == 4)
			matchcase = callfunc->Evaluatethread(3, domain, _thread)->Integer();
		char ret = textbuf->search_forward(i, STR(sfind), &i, matchcase);
		if (ret) {
			textbuf->replace(i, i + sfind.size(), STR(rep));
			return kifcode->Provideinteger(i);
		}
		return kifMINUSONE;
	}

	KifElement* MethodReplaceAll(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		int i = 0;
		//we need:
		string sfind = callfunc->Evaluatethread(0, domain, _thread)->String();
		string rep = callfunc->Evaluatethread(1, domain, _thread)->String();
		int matchcase = 0;
		if (callfunc->Size() == 3)
			matchcase = callfunc->Evaluatethread(2, domain, _thread)->Integer();
		char ret = textbuf->search_forward(i, STR(sfind), &i, matchcase);
		bool made = ret;
		while (ret == 1) {
			textbuf->replace(i, i + sfind.size(), STR(rep));
			i += rep.size();
			ret = textbuf->search_forward(i, STR(sfind), &i, matchcase);
		}
		if (made)
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* MethodFind(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		int i = 0;
		//we need:
		string sfind = callfunc->Evaluatethread(0, domain, _thread)->String();
		if (callfunc->Size() == 2)
			i = callfunc->Evaluatethread(1, domain, _thread)->Integer();
		int matchcase = 0;
		if (callfunc->Size() == 3)
			matchcase = callfunc->Evaluatethread(2, domain, _thread)->Integer();
		if (contextualpattern->aVectorContainer()) {
			KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
			char ret = textbuf->search_forward(i, STR(sfind), &i, matchcase);
			while (ret == 1) {
				kvect->storevalue((long)i);
				i++;
				ret = textbuf->search_forward(i, STR(sfind), &i, matchcase);
			}
			return kvect;
		}

		if (textbuf->search_forward(i, STR(sfind), &i, matchcase) == 1)
			return kifcode->Provideinteger(i);
		return kifMINUSONE;
	}

	KifElement* MethodRFind(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		int i = 0;
		//we need:
		string sfind = callfunc->Evaluatethread(0, domain, _thread)->String();
		if (callfunc->Size() == 2)
			i = callfunc->Evaluatethread(1, domain, _thread)->Integer();
		int matchcase = 0;
		if (callfunc->Size() == 3)
			matchcase = callfunc->Evaluatethread(2, domain, _thread)->Integer();

		if (contextualpattern->aVectorContainer()) {
			KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
			char ret = textbuf->search_backward(i, STR(sfind), &i, matchcase);
			while (ret == 1) {
				kvect->storevalue((long)i);
				i++;
				ret = textbuf->search_backward(i, STR(sfind), &i, matchcase);
			}
			return kvect;
		}

		if (textbuf->search_backward(i, STR(sfind), &i, matchcase) == 1)
			return kifcode->Provideinteger(i);
		return kifcode->Provideinteger(-1);
	}

	KifElement* MethodAppend(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		int start = editor->insert_position();
		string str = callfunc->Evaluatethread(0, domain, _thread)->String();
		textbuf->append(STR(str));
		editor->redisplay_range(start, start + str.size());
		return kifTRUE;
	}


	KifElement* MethodInsert(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		//we need:
		string str = callfunc->Evaluatethread(0, domain, _thread)->String();
		long pos = callfunc->Evaluatethread(1, domain, _thread)->Integer();
		if (pos >= 0 && pos < textbuf->length()) {
			textbuf->insert(pos, STR(str));
			return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* AddStyle(int idcallthread, long start, long end, uchar stylecode, int _thread) {

		if (start<0 || end>stylebuf->length() || start >= end)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[325]);

		//char* style = new char[end-start+1];
		//memset(style,stylecode,end-start);
		//style[end-start]=0;
		//stylebuf->replace(start,end,style);
		//editor->redisplay_range(start, end);
		//delete[] style;

		stylebuf->setstyle(start, end, stylecode);
		editor->redisplay_range(start, end);
		return kifTRUE;
	}

	void Annotate(int idcallthread, char* buf, char* sfind, int sz, unsigned char stylecode, int matchcase) {
		if (sz == 0)
			return;

		int i = 0;
		char* ret = strstr(buf, sfind);
		while (ret != NULL) {
			i = ret - buf;
			AddStyle(idcallthread, i, i + sz, stylecode, idcallthread);
			ret = strstr(++ret, sfind);
		}
	}


	KifElement* MethodAnnotate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		if (styletable == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[326]);

		//we need:
		KifElement* kfind = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kstyle = callfunc->Evaluatethread(1, domain, _thread);
		int matchcase = 0;
		if (callfunc->Size() == 3)
			matchcase = callfunc->Evaluatethread(2, domain, _thread)->Integer();

		string stylekey = kstyle->String();
		if (stylekeys.find(stylekey) == stylekeys.end())
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[327]);
		unsigned char stylecode = stylekeys[stylekey][0];
		string sfind;
		string s;
		char* buf;
		char* fbuf;
		bool tofree = false;
		if (matchcase == 1) {
			fbuf = textbuf->text();
			buf = fbuf;
		}
		else {
			if (editor->modified || lowercase == "") {
				fbuf = textbuf->text();
				c_to_lower(lowercase, (uchar*)fbuf, textbuf->length());
				editor->modified = false;
			}
			buf = STR(lowercase);
		}
		if (kfind->aVectorContainer()) {
			KifElement* kvect = kfind;
			for (int i = 0; i < kvect->Size(); i++) {
				sfind = kvect->ValueOnIndex(i)->String();
				if (!matchcase) {
					s = sfind;
					c_to_lower(sfind, USTR(s), s.size());
				}
				Annotate(_thread, buf, STR(sfind), sfind.size(), stylecode, matchcase);
			}
		}
		else {
			sfind = kfind->String();
			if (!matchcase) {
				s = sfind;
				c_to_lower(sfind, USTR(s), s.size());
			}
			Annotate(_thread, buf, STR(sfind), sfind.size(), stylecode, matchcase);
		}
		if (tofree)
			free(fbuf);
		return kifTRUE;
	}

	KifElement* MethodAnnotateXIPRgx(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		if (styletable == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[326]);

		//we need:
		KifElement* krgx = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kstyle = callfunc->Evaluatethread(1, domain, _thread);
		string stylekey = kstyle->String();
		if (stylekeys.find(stylekey) == stylekeys.end())
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[327]);
		unsigned char stylecode = stylekeys[stylekey][0];
		string reg = krgx->String();
		inittableutf8();
		automate a(USTR(reg), (void*)1);
		VECTA<string*> vectr;
		char* ltxt = textbuf->text();
		a.rechercheavecextractiondansvecteur((uchar*)ltxt, vectr);
		char* ret = ltxt;
		int i;
		for (int iv = 0; iv < vectr.dernier; iv++) {
			ret = strstr(ret, (char*)vectr[iv]->c_str());
			i = ret - ltxt;
			AddStyle(idcallthread, i, i + vectr[iv]->size(), stylecode, _thread);
			ret++;
		}
		vectr.nettoie();
		free(ltxt);
		return kifTRUE;
	}

	KifElement* MethodSetStyle(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		if (styletable == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[326]);

		//we need:
		KifElement* kstart = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kend = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* kstyle = callfunc->Evaluatethread(2, domain, _thread);
		string stylekey = kstyle->String();
		if (stylekeys.find(stylekey) == stylekeys.end())
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[327]);
		unsigned char stylecode = stylekeys[stylekey][0];
		return AddStyle(idcallthread, kstart->Integer(), kend->Integer(), stylecode, _thread);
	}

	KifElement* MethodHighlight(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		if (callfunc->Size() == 0) {
			if (contextualpattern->type == kifString) {
				string s = textbuf->highlight_text();
				return kifcode->Providestring(s);
			}
			return kifcode->Provideinteger(textbuf->highlight());
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		//we need:
		int start = callfunc->Evaluatethread(0, domain, _thread)->Integer();
		int end = callfunc->Evaluatethread(1, domain, _thread)->Integer();
		if (start < 0 || end >= textbuf->length() || start >= end)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[325]);
		textbuf->highlight(start, end);
		editor->insert_position(end);
		editor->show_insert_position();
		return kifNULL;
	}

	KifElement* MethodCursor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		//we need:
		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(editor->insert_position());

		threadtosetvalue(contextualpattern, domain, callfunc);
		int end = callfunc->Evaluatethread(0, domain, _thread)->Integer();
		if (end<0)
			end = 1;
		else
		if (end>textbuf->length())
			end = textbuf->length();
		editor->insert_position(end);
		editor->show_insert_position();
		return kifNULL;
	}

	KifElement* MethodCursorChar(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		uchar* value = textbuf->internalbuffer();
		//the position is in bytes
		int pos = editor->insert_position();
		//we convert it into characters
		pos = c_bytetocharposition(value, pos);
		return callfunc->kifcode->Provideinteger(pos);
	}

	KifElement* MethodUnHighlight(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		textbuf->unhighlight();
		return kifTRUE;
	}

	KifElement* MethodCursorStyle(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		//0 is the first parameter and so on...
		KifElement* kaction = callfunc->Evaluatethread(0, domain, _thread);
		Fl_Cursor act;
		if (kaction->type == kifString) {
			string action = kaction->String();
			if (cursors.find(action) == cursors.end())
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[290]);
			act = (Fl_Cursor)actions[action];
		}
		else
			act = (Fl_Cursor)kaction->Integer();

		editor->cursor_style(act);
		return kifTRUE;
	}

	KifElement* MethodGetStyle(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		if (styletable == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[326]);

		//we need:
		KifElement* kstart = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kend = callfunc->Evaluatethread(1, domain, _thread);
		int start = kstart->Integer();
		int end = kend->Integer();
		if (contextualpattern->aVectorContainer()) {
			KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
			if (start < 0 || end >= stylebuf->length() || start >= end)
				return kvect;

			char* style = stylebuf->text_range(start, end);
			char c[] = { 0, 0 };
			for (int i = 0; i < end - start; i++) {
				c[0] = style[i];
				kvect->storevalue(keystyles[c]);
			}
			free(style);
			return kvect;
		}

		if (start < 0 || end >= stylebuf->length() || start >= end)
			return kifNULL;
		string styles;
		char* style = stylebuf->text_range(start, end);
		char c[] = { 0, 0 };
		for (int i = 0; i < end - start; i++) {
			c[0] = style[i];
			styles += keystyles[c];
		}
		free(style);
		return callfunc->kifcode->Providestring(styles);
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		if (callfunc->Size() == 0) {
			char* ltxt = textbuf->text();
			KifElement* kval = kifcode->Providestringraw(ltxt);
			free(ltxt);
			return kval;
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		buf = callfunc->Evaluatethread(0, domain, _thread)->String();
		textbuf->text(buf.c_str());
		return kifTRUE;
	}

	KifElement* MethodSelection(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		int start = 0, end = 0;

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat) {
			textbuf->selection_position(&start, &end);
			return kifcode->Provideinteger(start);
		}

		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			textbuf->selection_position(&start, &end);
			contextualpattern->Clear();
			contextualpattern->Push(kifcode->Provideinteger(start));
			contextualpattern->Push(kifcode->Provideinteger(end));
			return contextualpattern;
		}

		if (contextualpattern->aMapContainer()) {
			contextualpattern->Clear();
			textbuf->selection_position(&start, &end);
			contextualpattern->Push((char*)KIFLTKSTRINGS[328], kifcode->Provideinteger(start));
			contextualpattern->Push((char*)KIFLTKSTRINGS[329], kifcode->Provideinteger(end));
			return contextualpattern;
		}

		char* str = textbuf->selection_text();
		buf = str;
		free(str);
		return kifcode->Providestring(buf);
	}

	KifElement* MethodSelect(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		if (callfunc->Size() == 0) {
			char* str = textbuf->selection_text();
			KifElement* ke = callfunc->kifcode->Providestringraw(str);
			free(str);
			return ke;
		}

		int posfirst = callfunc->Evaluatethread(0, domain, _thread)->Integer();
		int posend = callfunc->Evaluatethread(1, domain, _thread)->Integer();
		textbuf->select(posfirst, posend);
		return kifTRUE;
	}



	KifElement* MethodColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		if (callfunc->Size() == 0) {
			Fl_Color color = editor->textcolor();
			return kifcode->Provideinteger(color);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			editor->textcolor(colors[buf]);
		}
		else {
			unsigned int c = ke->Integer();
			editor->textcolor(c);
		}

		return kifTRUE;
	}

	KifElement* MethodFont(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		if (callfunc->Size() == 0) {
			int i = editor->textfont();
			if (ifonts.find(i) != ifonts.end())
				return kifcode->Providestring(ifonts[i]);
			return kifcode->Provideinteger(i);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		if (ke->type == kifString) {
			string buf = ke->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = KIFLTKSTRINGS[275];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}

			editor->textfont(fonts[buf]);
		}
		else {
			int font = ke->Integer();
			if (font >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[263]);
			editor->textfont(font);
		}
		return kifTRUE;
	}


	KifElement* MethodFontSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(editor->textsize());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		editor->textsize(ke->Integer());
		return kifTRUE;
	}

	KifElement* MethodDelete(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		Fl_Text_Editor::kf_delete(0, editor);
		return kifTRUE;
	}

	KifElement* MethodCopy(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		if (callfunc->Size() == 0)
			Fl_Text_Editor::kf_copy(0, editor);
		else {
			string tocopy = callfunc->Evaluatethread(0, domain, _thread)->String();
			Fl::copy(STR(tocopy), tocopy.length(), 1);
		}

		return kifTRUE;
	}

	KifElement* MethodCut(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		Fl_Text_Editor::kf_cut(0, editor);
		return kifTRUE;
	}

	KifElement* MethodPaste(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);
		Fl_Text_Editor::kf_paste(0, editor);
		return kifTRUE;
	}

	KifElement* MethodLine(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		int pos;
		KifElement* ke;
		if (callfunc->Size() == 0)
			pos = editor->insert_position();
		else
			pos = callfunc->Evaluatethread(0, domain, _thread)->Integer();

		if (contextualpattern->type == kifInteger) {
			pos = editor->Line(pos);
			return kifcode->Provideinteger(pos);
		}
		else
		if (contextualpattern->type == kifFloat) {
			pos = editor->Line(pos);
			return kifcode->Providefloat(pos);
		}

		//Else we extract the string
		int start = editor->line_start(pos);
		int end = editor->line_end(start, 1);
		ke = kifcode->Providestringraw(textbuf->subrange(start, end));
		return ke;
	}

	KifElement* MethodLineBoundaries(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		int pos;
		if (callfunc->Size() == 0)
			pos = editor->insert_position();
		else
			pos = callfunc->Evaluatethread(0, domain, _thread)->Integer();

		KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
		//Else we extract the string
		int start = editor->line_start(pos);
		int end = editor->line_end(start, 1);
		kvect->storevalue((long)start);
		kvect->storevalue((long)end);
		return kvect;
	}

	KifElement* MethodLineBoundariesChar(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		int pos;
		if (callfunc->Size() == 0)
			pos = editor->insert_position();
		else
			pos = callfunc->Evaluatethread(0, domain, _thread)->Integer();

		KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
		//Else we extract the string
		int start = editor->line_start(pos);
		int end = editor->line_end(start, 1);

		uchar* value = textbuf->internalbuffer();
		//the position is in bytes
		//we convert it into characters
		int nb = 0, i = 0;
		start = c_bytetocharpositionidx(value, start, nb, i);
		end = c_bytetocharpositionidx(value, end, nb, i);
		kvect->storevalue((long)start);
		kvect->storevalue((long)end);
		return kvect;
	}

	KifElement* MethodWrap(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		bool mode = callfunc->Evaluatethread(0, domain, _thread)->Boolean();
		if (mode)
			editor->wrap_mode(3, -1);
		else
			editor->wrap_mode(0, -1);
		return kifTRUE;
	}

	KifElement* MethodWord(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		//first we get our position
		long pos = callfunc->Evaluatethread(0, domain, _thread)->Integer();
		//then we try to extract the word string
		int start = editor->word_start(pos);
		int end = editor->word_end(pos);
		KifString* ke = kifcode->Providestringraw(textbuf->subrange(start, end));
		return ke;
	}

	KifElement* MethodGotoLine(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[320]);

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		bool hg = callfunc->Evaluatethread(1, domain, _thread)->Boolean();
		//0 is the first parameter and so on...
		long line = ke->Integer();
		int pos = 0;
		int end;
		if (line > 1) {
			int ret = 1;
			int nb = 1;
			while (ret != 0 && nb < line) {
				ret = textbuf->search_forward(pos, "\n", &pos, 0);
				pos++;
				nb++;
			}
			editor->scroll(line, 0);
		}

		if (hg) {
			end = pos;
			if (textbuf->search_forward(end, "\n", &end, 0) == 1)
				textbuf->highlight(pos, end);
		}
		end = editor->Line(end);
		editor->insert_position(pos);
		editor->show_insert_position();
		return kifcode->Provideinteger(pos);
	}

	KifElement* MethodBytePosition(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		uchar* value = textbuf->internalbuffer();
		long i = callfunc->Evaluatethread(0, dom, _thread)->Integer();
		i = c_chartobyteposition(value, i);
		return callfunc->kifcode->Provideinteger(i);
	}

	//From a byte position to a character
	KifElement* MethodCharacterPosition(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		uchar* value = textbuf->internalbuffer();
		long i = callfunc->Evaluatethread(0, dom, _thread)->Integer();
		i = c_bytetocharposition(value, i);
		return callfunc->kifcode->Provideinteger(i);
	}

	//---------------------------------------------------------------------------------------------------------------------
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
		if (editor == NULL)
			return this;

		if (idx == NULL || idx == kifNULL || contextualpattern == idx)
			return this;

		KifElement* ke;
		KifElement* left = idx;
		KifElement* right = NULL;
		if (idx->type == kifIndex) {
			KifIndex* kind = (KifIndex*)idx;
			left = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			if (kind->interval == true)
				right = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		}

		int sz = textbuf->length();
		uchar* value = textbuf->internalbuffer();

		register int ikey;
		if (left->type == kifString) {
			//then we are looking for a substring
			ikey = s_find(value, left->String(), 0);
			if (ikey == -1)
				return kifNULL;
		}
		else {
			ikey = left->Integer();
			if (ikey < 0)
				ikey += size_c((const char*)value);
			ikey = c_chartobyteposition(value, ikey);
			if (ikey < 0 || ikey >= sz)
				return kifNULL;
		}

		if (right == NULL) {
			ke = kifcode->Providestringraw("");
			((KifString*)ke)->value = c_char_get(value, ikey);
			return ke;
		}
		int iright;
		if (right->type == kifString) {
			iright = s_find(value, right->String(), ikey + 1);
			if (iright == -1)
				return kifNULL;
		}
		else {
			iright = right->Integer();

			if (iright <= 0 || right == kifNULL)
				iright += size_c((const char*)value);
			iright = c_chartobyteposition(value, iright);
			if (iright > sz)
				iright = sz;
		}
		if ((iright - ikey) <= 0)
			return kifNULL;

		ke = kifcode->Providestringraw(textbuf->subrange(ikey, iright));
		return ke;
	}

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() {
		return textbuf->length();
	}

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }
};
map<short, KifCallMethod> Kifeditor::kifexportedmethods;
map<short, editorMethod>  Kifeditor::linkedmethods;
map<string, string> Kifeditor::infomethods;
hmap<short, bool> Kifeditor::methodassigned;


void KifTextEditor::draw_cursor(int X, int Y) {

	/* Create segments and draw cursor */
	if (mCursorStyle == 5) {
		if (X < text_area.x - 1 || X > text_area.x + text_area.w)
			return;

		fl_color(mCursor_color);
		fl_line(X, Y, X, Y + mMaxsize - 1);
	}
	else
		Fl_Text_Display::draw_cursor(X, Y);
}

void KifTextEditor::loc_v_scrollbar_cb(Fl_Scrollbar* b, KifTextEditor* textD) {
	if (textD->functionvscroll != NULL) {
		Kifeditor* input = (Kifeditor*)textD->editor;
		KifFunction* func = textD->functionvscroll->Functioncall(textD->vdom);
		KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
		kfunc.kifcode = func->kifcode;
		threadblocinit;


		input->Setreference();
		textD->objectvscroll->Setreference();
		kfunc.parameters.push_back(input);
		kfunc.parameters.push_back(textD->objectvscroll);

		kfunc.Exec(textD->vcontext, textD->vdom, kifNULL, idthread, callfunc);

		input->Resetreference();
		textD->objectvscroll->Resetreference();
	}
	v_scrollbar_cb(b, textD);
}

void KifTextEditor::loc_h_scrollbar_cb(Fl_Scrollbar* b, KifTextEditor* textD) {
	if (textD->functionhscroll != NULL) {
		Kifeditor* input = (Kifeditor*)textD->editor;
		KifFunction* func = textD->functionhscroll->Functioncall(textD->hdom);
		KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
		kfunc.kifcode = func->kifcode;
		threadblocinit;


		input->Setreference();
		textD->objecthscroll->Setreference();
		kfunc.parameters.push_back(input);
		kfunc.parameters.push_back(textD->objecthscroll);

		kfunc.Exec(textD->vcontext, textD->hdom, kifNULL, idthread, callfunc);

		input->Resetreference();
		textD->objecthscroll->Resetreference();
	}
	h_scrollbar_cb(b, textD);
}

int KifTextEditor::handle(int e) {
	switch (e) {
	case FL_PUSH:
	case FL_RELEASE:
	case FL_DRAG:
	case FL_MOVE:
	case FL_MOUSEWHEEL:
		if (mouse_callback(e) == 1)
			return 1;
		break;
	case FL_KEYDOWN:
	case FL_KEYUP:
		if (key_callback(e) == 1)
			return 1;
		break;
	}
	return Fl_Text_Editor::handle(e);
}

int KifTextEditor::mouse_callback(int e) {
	static bool encours = false;

	if (encours == true)
		return false;

	encours = true;
	int x, y, xroot, yroot, pos = -1;
	if (editor->messagestyle.size() != 0) {
		x = Fl::event_x();
		y = Fl::event_y();
		xroot = Fl::event_x_root();
		yroot = Fl::event_y_root();
		pos = editor->editor->xy_to_position(x, y);
		//in this case, we might display a little sticky note
		char c[] = { 0, 0 };
		c[0] = editor->stylebuf->byte_at(pos);
		if (editor->messagestyle.find(c) != editor->messagestyle.end()) {
			if (editor->sticky == NULL) {
				int w, h;
				fl_measure(STR(editor->messagestyle[c]), w, h, 1);
				editor->sticky = new Fl_Double_Window(xroot, yroot, w + 30, h + 12);
				editor->sticky->border(0);
				editor->sticky->color(editor->messagecolor[c]);
				Fl_Box* bbox = new Fl_Box(10, 5, w + 5, h + 7, STR(editor->messagestyle[c]));
				editor->sticky->end();
				editor->sticky->show();
			}
		}
		else {
			if (editor->sticky != NULL) {
				Fl_Double_Window* wnd = editor->sticky;
				editor->sticky = NULL;
				delete wnd;
			}
		}
	}

	if (functionmouse.find(e) == functionmouse.end()) {
		encours = false;
		return 0;
	}

	if (pos == -1) {
		x = Fl::event_x();
		y = Fl::event_y();
		xroot = Fl::event_x_root();
		yroot = Fl::event_y_root();
		pos = editor->editor->xy_to_position(x, y);
	}
	KifFunction* func = functionmouse[e].Function()->Functioncall(mousedom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;


	KifMapStringInteger* kmouse = new KifMapStringInteger(func->kifcode, NULL);
	kmouse->values[KIFLTKSTRINGS[307]] = Fl::event_button();
	kmouse->values[KIFLTKSTRINGS[308]] = Fl::event_dx();
	kmouse->values[KIFLTKSTRINGS[309]] = Fl::event_dy();
	kmouse->values["x"] = x;
	kmouse->values["y"] = y;
	kmouse->values[KIFLTKSTRINGS[310]] = xroot;
	kmouse->values[KIFLTKSTRINGS[311]] = yroot;
	kmouse->values[KIFLTKSTRINGS[330]] = pos;

	editor->Setreference();
	objectmouse[e].Set();
	kmouse->Setreference();
	kfunc.parameters.push_back(editor);
	kfunc.parameters.push_back(kmouse);
	kfunc.parameters.push_back(objectmouse[e].Object());
	KifElement* ret = kfunc.Exec(mousecontext, mousedom, kifNULL, idthread, callfunc);
	editor->Resetreference();
	objectmouse[e].Reset();
	kmouse->Resetreference();

	encours = false;
	return ret->Boolean();
}

int KifTextEditor::key_callback(int e) {
	if (functionkey.find(e) == functionkey.end())
		return 0;

	KifFunction* func = functionkey[e].Function()->Functioncall(mousedom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;


	editor->Setreference();
	objectkey[e].Set();
	KifElement* ks = editor->kifcode->Providestringraw(Fl::event_text());
	KifElement* ki = editor->kifcode->Provideinteger(Fl::event_key());
	KifInteger* kcombine = editor->kifcode->Provideinteger(0);
	if (Fl::event_ctrl())
		kcombine->value |= 1;
	if (Fl::event_alt())
		kcombine->value |= 2;
	if (Fl::event_command())
		kcombine->value |= 4;
	if (Fl::event_shift())
		kcombine->value |= 8;

	kcombine->Setreference();
	ks->Setreference();
	ki->Setreference();
	kfunc.parameters.push_back(editor);
	kfunc.parameters.push_back(ks);
	kfunc.parameters.push_back(ki);
	kfunc.parameters.push_back(kcombine);
	kfunc.parameters.push_back(objectkey[e].Object());
	KifElement* ret = kfunc.Exec(mousecontext, mousedom, kifNULL, idthread, callfunc);

	editor->Resetreference();
	objectkey[e].Reset();
	ki->Resetreference();
	ks->Resetreference();
	kcombine->Resetreference();
	return ret->Boolean();
}


void editor_callback(int pos, int nInserted, int nDeleted, int nRestyled, const char* deletedText, void* data) {

	Kifeditor* input = (Kifeditor*)data;
	KifFunction* func = input->function->Functioncall(input->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;



	KifInteger* k1 = input->kifcode->Provideinteger(pos);
	KifInteger* k2 = input->kifcode->Provideinteger(nInserted);
	KifInteger* k3 = input->kifcode->Provideinteger(nDeleted);
	KifInteger* k4 = input->kifcode->Provideinteger(nRestyled);
	KifString* ks = input->kifcode->Providestringraw("");
	if (deletedText != NULL)
		ks->value = deletedText;

	input->Setreference();
	input->object->Setreference();
	k1->Setreference();
	k2->Setreference();
	k3->Setreference();
	k4->Setreference();
	ks->Setreference();

	kfunc.parameters.push_back(input);
	kfunc.parameters.push_back(k1);
	kfunc.parameters.push_back(k2);
	kfunc.parameters.push_back(k3);
	kfunc.parameters.push_back(k4);
	kfunc.parameters.push_back(ks);
	kfunc.parameters.push_back(input->object);

	kfunc.Exec(input->context, input->dom, kifNULL, idthread, callfunc);

	input->Resetreference();
	input->object->Resetreference();
	k1->Resetreference();
	k2->Resetreference();
	k3->Resetreference();
	k4->Resetreference();
	ks->Resetreference();

}


class Kifoutput : public KifWidget {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, outputMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Output* output;
	char txt[1024];

	string buf;
	bool multiline;
	//---------------------------------------------------------------------------------------------------------------------
	Kifoutput(KifCode* kifcode, KifElement* base) : KifWidget(kifcode, base, output_type) {
		//Do not forget your variable initialisation
		output = NULL;
		multiline = false;
	}

	void itemclose() {
		output = NULL;
		multiline = false;
		clean();
	}

	Fl_Widget* Widget() {
		return output;
	}

	void ResetWidget() {
		output = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifoutput* kf = new Kifoutput(kifcode, kifNULL);
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* w = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* h = callfunc->Evaluatethread(3, domain, _thread);
		multiline = callfunc->Evaluatethread(4, domain, _thread)->Boolean();
		KifElement* label = callfunc->Evaluatethread(5, domain, _thread);

		if (output != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[331]);

		string thetxt = label->String();
		if (thetxt.size() < 1024)
			strcpy_s(txt, 1024, thetxt.c_str());

		{
			ThreadLock _lock(type);
			if (multiline)
				output = new Fl_Multiline_Output(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
			else
				output = new Fl_Output(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
		}
		output->cursor_color(FL_WHITE);
		SaveItem(this);
		return kifTRUE;
	}

	KifElement* MethodWrap(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (output == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[332]);

		bool mode = callfunc->Evaluatethread(0, domain, _thread)->Boolean();
		if (mode)
			output->align(FL_ALIGN_WRAP);
		else
			output->align(FL_ALIGN_NOWRAP);
		return kifTRUE;
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (output == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[332]);

		if (callfunc->Size() == 0) {
			buf = output->value();
			return kifcode->Providestring(buf);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* label = callfunc->Evaluatethread(0, domain, _thread);
		buf = label->String();
		output->value((char*)buf.c_str());
		return kifTRUE;
	}

	KifElement* MethodColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (output == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[332]);

		if (callfunc->Size() == 0) {
			Fl_Color color = output->textcolor();
			return kifcode->Provideinteger(color);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			output->textcolor(colors[buf]);
		}
		else {
			unsigned int c = ke->Integer();
			output->textcolor(c);
		}

		return kifTRUE;
	}

	KifElement* MethodFont(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (output == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[332]);

		if (callfunc->Size() == 0) {
			int i = output->textfont();
			if (ifonts.find(i) != ifonts.end())
				return kifcode->Providestring(ifonts[i]);
			return kifcode->Provideinteger(i);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		if (ke->type == kifString) {
			buf = ke->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = KIFLTKSTRINGS[275];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}

			output->textfont(fonts[buf]);
		}
		else {
			int font = ke->Integer();
			if (font >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[263]);
			output->textfont(font);
		}
		return kifTRUE;
	}

	KifElement* MethodFontSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (output == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[332]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(output->textsize());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		output->textsize(ke->Integer());
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }
};
map<short, KifCallMethod> Kifoutput::kifexportedmethods;
map<short, outputMethod>  Kifoutput::linkedmethods;
map<string, string> Kifoutput::infomethods;
hmap<short, bool> Kifoutput::methodassigned;


class KiFImageButton : public Fl_Button {
public:
	// CONSTRUCTOR
	Fl_Image* myimage;
	bool isbitmap;
	unsigned int thecolor;
	string mytext;
	int myalign;

	KiFImageButton(int X, int Y, int W, int H, const char*L = 0) : Fl_Button(X, Y, W, H, L) {
		myimage = NULL;
		myalign = FL_ALIGN_CENTER;
		isbitmap = false;
		thecolor = FL_BLACK;
	}

	void draw() {
		int X = x() + Fl::box_dx(box());  // area inside the button's frame
		int Y = y() + Fl::box_dy(box());
		int W = w() - Fl::box_dw(box());
		int H = h() - Fl::box_dh(box());
		draw_box(value() ? (down_box() ? down_box() : fl_down(box())) : box(), color());  // button up/dn
		fl_push_clip(X, Y, W, H);                  // clip inside button's frame
		{
			if (myimage != NULL) {
				int imgx = X + ((myimage->w() < W) ? (W - myimage->w()) / 2 : 0);
				// horiz center
				int imgy = Y + ((myimage->h() < H) ? (H - myimage->h()) / 2 : 0);
				// vert center
				if (isbitmap)
					fl_color(thecolor);
				myimage->draw(imgx, imgy);
			}
			if (mytext != "") {
				fl_color(labelcolor());
				fl_font(labelfont(), labelsize());
				fl_draw(mytext.c_str(), x(), y(), w(), h(), myalign);
			}
		}
		fl_pop_clip();
	}
};

class Kifbutton : public KifWidgetCallback {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, buttonMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Button* button;
	int thetype;
	char txt[1024];


	bool image;
	//---------------------------------------------------------------------------------------------------------------------
	Kifbutton(KifCode* kifcode, KifElement* base) : KifWidgetCallback(kifcode, base, button_type) {
		//Do not forget your variable initialisation
		button = NULL;
		image = false;
	}

	void itemclose() {
		button = NULL;
		clean();
	}

	Fl_Widget* Widget() {
		return button;
	}

	void ResetWidget() {
		dom = NULL;
		button = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifbutton* kf = new Kifbutton(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* CallbackObject() {
		return object;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true


	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters


		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* w = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* h = callfunc->Evaluatethread(3, domain, _thread);

		KifElement* ktype = NULL;
		KifElement* shape = NULL;
		KifElement* label;

		if (callfunc->Size() == 7) {
			ktype = callfunc->Evaluatethread(4, domain, _thread);
			shape = callfunc->Evaluatethread(5, domain, _thread);
			label = callfunc->Evaluatethread(6, domain, _thread);
		}
		else {
			if (callfunc->Size() == 6) {
				shape = callfunc->Evaluatethread(4, domain, _thread);
				label = callfunc->Evaluatethread(5, domain, _thread);
			}
			else
				label = callfunc->Evaluatethread(4, domain, _thread);
		}

		if (button != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[333]);

		int shap;
		if (shape == NULL)
			shap = 0;
		else {
			if (shape->type == kifString) {
				if (buttonShapes.find(shape->String()) == buttonShapes.end()) {
					string mes = KIFLTKSTRINGS[334];
					mes += shape->String();
					return Returningerror(kifcode, _thread, idcallthread, mes);
				}
				shap = buttonShapes[shape->String()];
			}
			else
				shap = shape->Integer();
		}

		string thetxt = label->String();
		if (thetxt.size() < 1024)
			strcpy_s(txt, 1024, thetxt.c_str());

		if (ktype == NULL)
			thetype = 0;
		else
		if (ktype->type == kifString) {
			string btype = ktype->String();
			if (buttontypes.find(btype) == buttontypes.end()) {
				string mes = KIFLTKSTRINGS[335];
				mes += btype;
				return Returningerror(kifcode, _thread, idcallthread, mes);
			}
			thetype = buttontypes[btype];
		}
		else
			thetype = ktype->Integer();
		{
			ThreadLock _lock(type);
			switch (thetype) {
			case 1: //check
				button = new Fl_Check_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
				break;
			case 2://light
				button = new Fl_Light_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer());
				break;
			case 3://Repeat
				button = new Fl_Repeat_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
				break;
			case 4://Return
				button = new Fl_Return_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
				break;
			case 5://Round
				button = new Fl_Round_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
				break;
			case 6://Image
				button = new KiFImageButton(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
				image = true;
				break;
			default://regular
				button = new Fl_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
			}
			button->type(shap);
		}

		if (function != NULL) {
			Keeptrack(domain, contextualpattern);
			button->callback(button_callback, this);
		}

		SaveItem(this);
		return kifTRUE;
	}

	KifElement* MethodImage(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (button == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[336]);
		if (!image)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[337]);

		KifElement* kimage = callfunc->Evaluatethread(0, domain, _thread);
		if (kimage->type != image_type)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[338]);
		Fl_Image* fl = ((Kifimage*)kimage)->image;
		if (callfunc->Size() >= 2) {
			((KiFImageButton*)button)->mytext = callfunc->Evaluatethread(1, domain, _thread)->String();
			if (callfunc->Size() == 3)
				((KiFImageButton*)button)->myalign = callfunc->Evaluatethread(2, domain, _thread)->Integer();
		}
		((KiFImageButton*)button)->myimage = fl;
		return kifTRUE;
	}

	KifElement* MethodBitmap(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (button == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[336]);
		if (!image)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[337]);
		KifElement* kbitmap = callfunc->Evaluatethread(0, domain, _thread);
		if (kbitmap->type != bitmap_type)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[338]);
		Fl_Bitmap* fl = ((Kifbitmap*)kbitmap)->bitmap;
		KifElement* kcolor = callfunc->Evaluatethread(1, domain, _thread);
		unsigned int c = FL_BLACK;
		if (kcolor->type == kifString) {
			//0 is the first parameter and so on...
			string shortcut = kcolor->String();
			if (colors.find(shortcut) == colors.end()) {
				shortcut = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, shortcut);
			}
			c = colors[shortcut];
		}
		else
			c = kcolor->Integer();

		if (callfunc->Size() >= 3) {
			((KiFImageButton*)button)->mytext = callfunc->Evaluatethread(2, domain, _thread)->String();
			if (callfunc->Size() == 4)
				((KiFImageButton*)button)->myalign = callfunc->Evaluatethread(3, domain, _thread)->Integer();
		}

		((KiFImageButton*)button)->thecolor = c;
		((KiFImageButton*)button)->isbitmap = true;
		((KiFImageButton*)button)->myimage = fl;
		return kifTRUE;
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		if (button == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[336]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(button->value());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* v = callfunc->Evaluatethread(0, domain, _thread);
		button->value(v->Integer());
		return kifTRUE;
	}

	KifElement* MethodWhen(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		if (button == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[336]);

		KifElement* ke;
		int action = 0;
		for (int i = 0; i < callfunc->Size(); i++) {
			ke = callfunc->Evaluatethread(i, domain, _thread);
			if (ke->type == kifString) {
				if (buttonAction.find(ke->String()) == buttonAction.end()) {
					string mess = KIFLTKSTRINGS[339];
					return Returningerror(kifcode, _thread, idcallthread, mess);
				}
				action |= buttonAction[ke->String()];
			}
			else
				action |= ke->Integer();
		}

		button->when(action);
		return kifTRUE;
	}


	KifElement* MethodShortcut(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		if (button == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[336]);

		//0 is the first parameter and so on...
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == kifString) {
			string shortcut = ke->String();
			if (keyCodes.find(shortcut) == keyCodes.end()) {
				shortcut = KIFLTKSTRINGS[340];
				return Returningerror(kifcode, _thread, idcallthread, shortcut);
			}
			button->shortcut(keyCodes[shortcut]);
		}
		else
			button->shortcut(ke->Integer());
		return kifTRUE;
	}

	KifElement* MethodColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		if (button == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[336]);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string shortcut = ke->String();
			if (colors.find(shortcut) == colors.end()) {
				shortcut = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, shortcut);
			}
			button->color(colors[shortcut]);
		}
		else {
			unsigned int c = ke->Integer();
			button->color(c);
		}

		return kifTRUE;
	}


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifbutton::kifexportedmethods;
map<short, buttonMethod>  Kifbutton::linkedmethods;
map<string, string> Kifbutton::infomethods;
hmap<short, bool> Kifbutton::methodassigned;

void button_callback(Fl_Widget *w, void *data) {
	Kifbutton* button = (Kifbutton*)data;
	KifFunction* func = button->function->Functioncall(button->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	button->Setreference();
	button->object->Setreference();
	kfunc.parameters.push_back(button);
	kfunc.parameters.push_back(button->object);
	kfunc.Exec(button->context, button->dom, kifNULL, idthread, callfunc);
	button->Resetreference();
	button->object->Resetreference();

}


class Kifbox : public KifWidget {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, boxMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Box* box;
	char txt[1024];


	//---------------------------------------------------------------------------------------------------------------------
	Kifbox(KifCode* kifcode, KifElement* base) : KifWidget(kifcode, base, box_type) {
		//Do not forget your variable initialisation
		box = NULL;

	}

	void itemclose() {
		box = NULL;
		clean();
	}

	Fl_Widget* Widget() {
		return box;
	}

	void ResetWidget() {
		box = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifbox* kf = new Kifbox(kifcode, kifNULL);
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		//0 is the first parameter and so on...
		KifElement* x1 = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y1 = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* x2 = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* y2 = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* text = callfunc->Evaluatethread(4, domain, _thread);

		if (box != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[341]);

		string thetxt = text->String();
		if (thetxt.size() < 1024)
			strcpy_s(txt, 1024, thetxt.c_str());
		{
			ThreadLock _lock(type);
			box = new Fl_Box(x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), txt);
		}
		SaveItem(this);
		return kifTRUE;
	}

	KifElement* MethodType(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (box == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[342]);

		KifElement* pos = callfunc->Evaluatethread(0, domain, _thread);
		if (pos->type == kifString) {
			if (boxTypes.find(pos->String()) == boxTypes.end()) {
				string mes = KIFLTKSTRINGS[343];
				mes += pos->String();
				return Returningerror(kifcode, _thread, idcallthread, mes);
			}
			else
				box->box((Fl_Boxtype)boxTypes[pos->String()]);
		}
		else
			box->box((Fl_Boxtype)pos->Integer());

		return kifTRUE;
	}



	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifbox::kifexportedmethods;
map<short, boxMethod>  Kifbox::linkedmethods;
map<string, string> Kifbox::infomethods;
hmap<short, bool> Kifbox::methodassigned;

class KiF_Group : public Fl_Group {
public:
	Kifgroup* associate;
	int iassociate;

	KiF_Group(Kifgroup* kg, int x, int y, int w, int h, const char* txt);
	~KiF_Group();

	void draw();
};

class Kifgroup : public Kifcomplexwidget {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, groupMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	KiF_Group* group;
	char txt[1024];
	bool end;


	//---------------------------------------------------------------------------------------------------------------------
	Kifgroup(KifCode* kifcode, KifElement* base) : Kifcomplexwidget(kifcode, base, group_type) {
		//Do not forget your variable initialisation
		group = NULL;
		end = false;

	}

	void itemclose() {
		group = NULL;
		clean();
	}

	~Kifgroup() {
		if (group != NULL) {
			if (!end)
				group->end();
			KiF_Group* g = group;
			group = NULL;
			delete g;
		}
	}

	Fl_Widget* Widget() {
		return group;
	}

	void ResetWidget() {
		dom = NULL;
		if (group != NULL) {
			if (!end)
				group->end();
			clean();
			KiF_Group* g = group;
			g->associate = NULL;
			group = NULL;
			end = false;
			delete g;
		}
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifgroup* kf = new Kifgroup(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodBegin(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthreadcreate(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		//0 is the first parameter and so on...
		KifElement* x1 = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y1 = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* x2 = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* y2 = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* text = callfunc->Evaluatethread(4, domain, _thread);

		if (group != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[344]);

		string thetxt = text->String();
		if (thetxt.size() < 1024)
			strcpy_s(txt, 1024, thetxt.c_str());
		{
			ThreadLock _lock(type);
			group = new KiF_Group(this, x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), txt);
		}

		if (function != NULL)
			Keeptrack(domain, contextualpattern);

		SaveItem(this);
		indraw = true;
		return kifTRUE;
	}

	KifElement* MethodActivate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters
		if (group == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[345]);

		Fl_Widget* tabs = group->parent();
		if (tabs == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[346]);
		((Fl_Tabs*)tabs)->value(group);
		return kifTRUE;
	}

	KifElement* MethodEnd(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters
		if (group == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[345]);
		group->end();
		end = true;
		indraw = false;
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifgroup::kifexportedmethods;
map<short, groupMethod>  Kifgroup::linkedmethods;
map<string, string> Kifgroup::infomethods;
hmap<short, bool> Kifgroup::methodassigned;


KiF_Group::KiF_Group(Kifgroup* kg, int x, int y, int w, int h, const char* txt) : associate(kg), Fl_Group(x, y, w, h, txt) { iassociate = kg->Ingarbage(); }

KiF_Group::~KiF_Group() {
	if (associate != NULL) {
		if (associate->end == false)
			end();
		associate->itemclose();
		associate->group = NULL;
	}
}

void KiF_Group::draw() {
	if (kifGlobals->garbage[iassociate] == associate) {
		if (associate->update)
			Fl_Group::draw();
	}

	if (stopall)
		return;


	if (associate->function != NULL) {
#ifndef WIN32
		ThreadLock _lock(kifDISPLAY);
#endif
		associate->setindraw(true);
		KifFunction* func = associate->function->Functioncall(associate->dom);
		KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
		kfunc.kifcode = func->kifcode;
		threadblocinit;

		associate->Setreference();
		associate->object->Setreference();
		kfunc.parameters.push_back(associate);
		kfunc.parameters.push_back(associate->object);
		kfunc.Exec(associate->context, associate->dom, kifNULL, idthread, callfunc);
		associate->Resetreference();
		associate->object->Resetreference();
		associate->setindraw(false);
	}
}


class Kiftabs : public KifWidget {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, tabsMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Tabs* tabs;
	char txt[1024];
	bool end;


	//---------------------------------------------------------------------------------------------------------------------
	Kiftabs(KifCode* kifcode, KifElement* base) : KifWidget(kifcode, base, tabs_type) {
		//Do not forget your variable initialisation
		tabs = NULL;
		end = false;

	}

	void itemclose() {
		tabs = NULL;
		clean();
	}

	Fl_Widget* Widget() {
		return tabs;
	}

	void ResetWidget() {
		if (end == false && tabs != NULL)
			tabs->end();
		tabs = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kiftabs* kf = new Kiftabs(kifcode, kifNULL);
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodBegin(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthreadcreate(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		//0 is the first parameter and so on...
		KifElement* x1 = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y1 = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* x2 = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* y2 = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* text = callfunc->Evaluatethread(4, domain, _thread);

		if (tabs != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[347]);

		string thetxt = text->String();
		if (thetxt.size() < 1024)
			strcpy_s(txt, 1024, thetxt.c_str());
		{
			ThreadLock _lock(type);
			tabs = new Fl_Tabs(x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), txt);
		}
		SaveItem(this);
		end = false;
		return kifTRUE;
	}

	KifElement* MethodEnd(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters
		if (tabs == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[348]);
		end = true;
		tabs->end();
		return kifTRUE;
	}

	KifElement* MethodCurrent(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (tabs == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[348]);
		if (callfunc->Size() == 0) {
			KiF_Group* flgroup = (KiF_Group*)tabs->value();
			if (flgroup == NULL)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[349]);
			return flgroup->associate;
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type != group_type)
			return Returningerror(kifcode, _thread, idcallthread, "WND(105): You can only select wgroup object");
		Kifgroup* kgroup = (Kifgroup*)ke;
		if (kgroup->group == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[350]);
		if (kgroup->group->parent() != tabs)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[351]);
		tabs->value(kgroup->group);
		return kifTRUE;
	}

	KifElement* MethodAdd(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters
		if (tabs == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[348]);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		if (ke->type == group_type) {
			Kifgroup* kgroup = (Kifgroup*)ke;
			if (kgroup->group == NULL)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[350]);
			tabs->add(kgroup->group);
			tabs->value(kgroup->group);
			tabs->hide();
			tabs->show();
			return kifTRUE;
		}

		return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[352]);
	}

	KifElement* MethodRemove(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters
		return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[353]);
		if (tabs == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[348]);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == group_type) {
			Kifgroup* kgroup = (Kifgroup*)ke;
			if (kgroup->group == NULL)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[350]);
			if (kgroup->group->parent() != tabs)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[351]);
			kgroup->group->hide();
			tabs->remove(kgroup->group);
			delete kgroup->group;
			tabs->hide();
			tabs->show();
			return kifTRUE;
		}

		return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[354]);
	}
	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> Kiftabs::kifexportedmethods;
map<short, tabsMethod>  Kiftabs::linkedmethods;
map<string, string> Kiftabs::infomethods;
hmap<short, bool> Kiftabs::methodassigned;

//--------------------------------------------------------------------------------------------------------------

class TableRow : public Fl_Table {
private:
	Fl_Color cell_bgcolor;				// color of cell's bg color
	Fl_Color cell_fgcolor;				// color of cell's fg color
	Fl_Font font;
	int fontsize;
	hmap<int, hmap<int, string> > rowcolarray;
	hmap<int, string> rowheader;
	hmap<int, string> colheader;
	int maxrow;
	int maxcol;
	int cell_alignement;
	int cell_alignement_header_col;
	int cell_alignement_header_row;

protected:
	void draw_cell(TableContext context, int R = 0, int C = 0, int X = 0, int Y = 0, int W = 0, int H = 0);
	static void table_callback(Fl_Widget*, void*);

public:
	string cell(int R, int C) {
		if (rowcolarray.find(R) != rowcolarray.end())
		if (rowcolarray[R].find(C) != rowcolarray[R].end())
			return rowcolarray[R][C];
		return "";
	}


	void setalignmentheader(int c, bool header) {
		if (header)
			cell_alignement_header_col = c;
		else
			cell_alignement_header_row = c;
	}

	void setalignment(int c) {
		cell_alignement = c;
	}

	void clear() {
		rowheader.clear();
		colheader.clear();
		rowcolarray.clear();
		maxrow = 0;
		maxcol = 0;
		rows(0);		// implies clearing selection
		cols(0);
		Fl_Table::clear();	// clear the table
	}

	void addrowheader(int i, string value) {
		rowheader[i] = value;
	}

	void addcolumnheader(int i, string value) {
		colheader[i] = value;
	}

	void cell(int R, int C, string value) {
		if (rowcolarray.find(R) == rowcolarray.end()) {
			hmap<int, string> col;
			rowcolarray[R] = col;
		}
		if (R > maxrow) {
			maxrow = R;
			rows(R + 1);
		}
		if (C > maxcol) {
			maxcol = C;
			cols(C + 1);
		}
		rowcolarray[R][C] = value;
	}


	TableRow(Kiftable* table, int x, int y, int w, int h, const char *l = 0) : Fl_Table(x, y, w, h, l) {

		cell_alignement_header_col = FL_ALIGN_LEFT;
		cell_alignement_header_row = FL_ALIGN_LEFT;
		cell_alignement = FL_ALIGN_LEFT;
		cell_bgcolor = FL_WHITE;
		cell_fgcolor = FL_BLACK;
		font = FL_HELVETICA;
		fontsize = 16;
		maxrow = 0;
		maxcol = 0;
		callback(&table_callback, (void*)table);
	}

	Fl_Color GetCellFGColor() const { return(cell_fgcolor); }
	Fl_Color GetCellBGColor() const { return(cell_bgcolor); }
	void SetCellFGColor(Fl_Color val) { cell_fgcolor = val; }
	void SetCellBGColor(Fl_Color val) { cell_bgcolor = val; }
	void Font(Fl_Font val) {
		font = val;
	}
	void Fontsize(int sz) {
		fontsize = sz;
	}
	Fl_Font Font() {
		return font;
	}
	int Fontsize() {
		return fontsize;
	}
};

// Handle drawing all cells in table
void TableRow::draw_cell(TableContext context,
	int R, int C, int X, int Y, int W, int H)
{
	string buffer;

	switch (context)
	{
	case CONTEXT_STARTPAGE:
		fl_font(font, fontsize);
		return;

	case CONTEXT_COL_HEADER:
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, col_header_color());
			fl_color(FL_BLACK);
			if (colheader.find(R) != colheader.end())
				buffer = colheader[C];
			fl_draw(buffer.c_str(), X, Y, W, H, cell_alignement_header_col);
		}
		fl_pop_clip();
		return;

	case CONTEXT_ROW_HEADER:
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
			fl_color(FL_BLACK);
			if (rowheader.find(R) != rowheader.end())
				buffer = rowheader[R];
			fl_draw(buffer.c_str(), X, Y, W, H, cell_alignement_header_row);
		}
		fl_pop_clip();
		return;

	case CONTEXT_CELL:
	{
						 fl_push_clip(X, Y, W, H);
						 {
							 // BG COLOR
							 fl_color(is_selected(R, C) ? selection_color() : cell_bgcolor);
							 fl_rectf(X, Y, W, H);

							 // TEXT
							 fl_color(cell_fgcolor);
							 buffer = cell(R, C);
							 fl_draw(buffer.c_str(), X, Y, W, H, cell_alignement);
							 // BORDER
							 fl_color(color());
							 fl_rect(X, Y, W, H);
						 }
						 fl_pop_clip();
						 return;
	}

	case CONTEXT_TABLE:
	case CONTEXT_ENDPAGE:
	case CONTEXT_RC_RESIZE:
	case CONTEXT_NONE:
		return;
	}
}


class Kiftable : public KifWidgetCallback {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, tableMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	TableRow* table;
	char txt[1024];


	//---------------------------------------------------------------------------------------------------------------------
	Kiftable(KifCode* kifcode, KifElement* base) : KifWidgetCallback(kifcode, base, table_type) {
		//Do not forget your variable initialisation
		table = NULL;
	}

	void itemclose() {
		table = NULL;
		clean();
	}

	Fl_Widget* Widget() {
		return table;
	}

	void ResetWidget() {
		dom = NULL;
		table = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kiftable* kf = new Kiftable(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* CallbackObject() {
		return object;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true


	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		//0 is the first parameter and so on...
		KifElement* x1 = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y1 = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* x2 = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* y2 = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* text = callfunc->Evaluatethread(4, domain, _thread);

		if (table != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[355]);

		if (function != NULL && function->Size() != 3)
			return kifcode->Returnerror(KIFLTKSTRINGS[315]);

		string thetxt = text->String();
		if (thetxt.size() < 1024)
			strcpy_s(txt, 1024, thetxt.c_str());
		{
			ThreadLock _lock(type);
			table = new TableRow(this, x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), txt);
			if (function != NULL)
				Keeptrack(domain, contextualpattern);
		}
		SaveItem(this);
		table->selection_color(FL_YELLOW);
		table->when(FL_WHEN_RELEASE);
		table->table_box(FL_NO_BOX);
		table->col_resize_min(4);
		table->row_resize_min(4);

		table->row_header(0);
		table->col_header(0);
		table->row_resize(1);
		table->col_resize(1);
		table->end();
		return kifTRUE;
	}

	KifElement* MethodClear(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		table->clear();
		return kifTRUE;
	}

	KifElement* MethodRowHeader(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		KifElement* position = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* label = callfunc->Evaluatethread(1, domain, _thread);
		//we then activate the row header, since we have something in it to show
		table->row_header(1);
		table->addrowheader(position->Integer(), label->String());
		return kifTRUE;
	}

	KifElement* MethodRowHeaderHeight(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		KifElement* kwidth = callfunc->Evaluatethread(0, domain, _thread);
		table->row_header_width(kwidth->Integer());
		return kifTRUE;
	}


	KifElement* MethodColumnHeader(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		KifElement* position = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* label = callfunc->Evaluatethread(1, domain, _thread);
		table->addcolumnheader(position->Integer(), label->String());
		//we activate then column header
		table->col_header(1);
		return kifTRUE;
	}

	KifElement* MethodColumnHeaderWidth(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		KifElement* kheight = callfunc->Evaluatethread(0, domain, _thread);
		table->col_header_height(kheight->Integer());
		return kifTRUE;
	}

	KifElement* MethodRow(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(table->rows());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* kR = callfunc->Evaluatethread(0, domain, _thread);
		table->rows(kR->Integer());
		return kifTRUE;
	}

	KifElement* MethodColumn(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(table->cols());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* kC = callfunc->Evaluatethread(0, domain, _thread);
		table->cols(kC->Integer());
		return kifTRUE;
	}

	KifElement* MethodRowHeight(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		KifElement* kHeight = callfunc->Evaluatethread(0, domain, _thread);
		table->row_height_all(kHeight->Integer());
		return kifTRUE;
	}

	KifElement* MethodColumnWidth(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		KifElement* kWidth = callfunc->Evaluatethread(0, domain, _thread);
		table->col_width_all(kWidth->Integer());
		return kifTRUE;
	}

	KifElement* MethodAdd(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		KifElement* kR = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kC = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* kvalue = callfunc->Evaluatethread(2, domain, _thread);

		table->cell(kR->Integer(), kC->Integer(), kvalue->String());
		return kifTRUE;
	}

	KifElement* MethodCell(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		KifElement* kR = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kC = callfunc->Evaluatethread(1, domain, _thread);

		string buff = table->cell(kR->Integer(), kC->Integer());

		return kifcode->Providestring(buff);
	}

	KifElement* MethodCellAlignment(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == kifString) {
			string align = ke->String();

			if (aligns.find(align) == aligns.end()) {
				align = KIFLTKSTRINGS[255];
				return Returningerror(kifcode, _thread, idcallthread, align);
			}

			table->setalignment(aligns[align]);
		}
		else
			table->setalignment(ke->Integer());

		return kifTRUE;
	}

	KifElement* MethodCellAlignmentHeaderCol(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == kifString) {
			string align = ke->String();

			if (aligns.find(align) == aligns.end()) {
				align = KIFLTKSTRINGS[255];
				return Returningerror(kifcode, _thread, idcallthread, align);
			}

			table->setalignmentheader(aligns[align], true);
		}
		else
			table->setalignmentheader(ke->Integer(), true);

		return kifTRUE;
	}

	KifElement* MethodCellAlignmentHeaderRow(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[356]);

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == kifString) {
			string align = ke->String();

			if (aligns.find(align) == aligns.end()) {
				align = KIFLTKSTRINGS[255];
				return Returningerror(kifcode, _thread, idcallthread, align);
			}

			table->setalignmentheader(aligns[align], false);
		}
		else
			table->setalignmentheader(ke->Integer(), false);

		return kifTRUE;
	}

	KifElement* MethodFont(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[357]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(table->Font());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		if (ke->type == kifString) {
			string buf = ke->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = KIFLTKSTRINGS[275];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			table->Font(fonts[buf]);
		}
		else {
			int font = ke->Integer();
			if (font >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[263]);
			table->Font(font);
		}

		return kifTRUE;
	}

	KifElement* MethodFontSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[357]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(table->Fontsize());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		table->Fontsize(ke->Integer());
		return kifTRUE;
	}

	KifElement* MethodTableSelectionColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[357]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(table->GetCellBGColor());


		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		table->selection_color(color);
		return kifTRUE;
	}

	KifElement* MethodBoxType(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[358]);

		KifElement* pos = callfunc->Evaluatethread(0, domain, _thread);
		if (pos->type == kifString) {
			if (boxTypes.find(pos->String()) == boxTypes.end()) {
				string mes = KIFLTKSTRINGS[343];
				mes += pos->String();
				return Returningerror(kifcode, _thread, idcallthread, mes);
			}
			else
				table->table_box((Fl_Boxtype)boxTypes[pos->String()]);
		}
		else
			table->table_box((Fl_Boxtype)pos->Integer());

		return kifTRUE;
	}


	KifElement* MethodWhen(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[359]);

		KifElement* ke;
		int action = 0;
		for (int i = 0; i < callfunc->Size(); i++) {
			ke = callfunc->Evaluatethread(i, domain, _thread);
			if (ke->type == kifString) {
				if (buttonAction.find(ke->String()) == buttonAction.end()) {
					string mess = KIFLTKSTRINGS[339];
					return Returningerror(kifcode, _thread, idcallthread, mess);
				}
				action |= buttonAction[ke->String()];
			}
			else
				action |= ke->Integer();
		}

		table->when(action);
		return kifTRUE;
	}


	KifElement* MethodColorbg(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[357]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(table->GetCellBGColor());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		table->SetCellBGColor(color);
		return kifTRUE;
	}

	KifElement* MethodColorfg(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[357]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(table->GetCellFGColor());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		table->SetCellFGColor(color);
		return kifTRUE;
	}//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> Kiftable::kifexportedmethods;
map<short, tableMethod>  Kiftable::linkedmethods;
map<string, string> Kiftable::infomethods;
hmap<short, bool> Kiftable::methodassigned;


// Callback whenever someone clicks on different parts of the table
void TableRow::table_callback(Fl_Widget*, void *data) {
	Kiftable*o = (Kiftable*)data;
	if (o->function != NULL && o->table != NULL) {
		KifFunction* func = o->function->Functioncall(o->dom);
		KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
		kfunc.kifcode = func->kifcode;
		threadblocinit;

		KifMap* kmap = new KifMap(o->kifcode, NULL);
		int rtop, cleft, rbot, cright;
		o->table->get_selection(rtop, cleft, rbot, cright);
		kmap->Push((char*)KIFLTKSTRINGS[360], o->kifcode->Provideinteger(rtop));
		kmap->Push((char*)KIFLTKSTRINGS[361], o->kifcode->Provideinteger(cleft));
		kmap->Push((char*)KIFLTKSTRINGS[362], o->kifcode->Provideinteger(rbot));
		kmap->Push((char*)KIFLTKSTRINGS[363], o->kifcode->Provideinteger(cright));
		KifMapStringString* valmap = new KifMapStringString(o->kifcode, NULL);
		kmap->Push((char*)KIFLTKSTRINGS[364], valmap);
		char buff[20];
		for (int i = rtop; i <= rbot; i++) {
			for (int j = cleft; j <= cright; j++) {
				sprintf_s(buff, 20, "%d:%d", i, j);
				valmap->values[buff] = o->table->cell(i, j);
			}
		}

		o->Setreference();
		o->object->Setreference();
		kmap->Setreference();

		kfunc.parameters.push_back(o);
		kfunc.parameters.push_back(kmap);
		kfunc.parameters.push_back(o->object);
		kfunc.Exec(o->context, o->dom, kifNULL, idthread, callfunc);

		o->Resetreference();
		o->object->Resetreference();
		kmap->Resetreference();

	}
}

//-----------------------------------------------------------------------------------------------
void scroll_callback(Fl_Widget*, void*);

class Kifscroll : public KifWidgetCallback {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, scrollMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Scroll* scroll;
	char txt[1024];


	//---------------------------------------------------------------------------------------------------------------------
	Kifscroll(KifCode* kifcode, KifElement* base) : KifWidgetCallback(kifcode, base, scroll_type) {
		//Do not forget your variable initialisation
		scroll = NULL;
	}

	void itemclose() {
		scroll = NULL;
		clean();
	}

	Fl_Widget* Widget() {
		return scroll;
	}

	void ResetWidget() {
		dom = NULL;
		scroll = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifscroll* kf = new Kifscroll(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* CallbackObject() {
		return object;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		if (scroll != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[365]);

		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);
		string label = callfunc->Evaluatethread(4, domain, _thread)->String();
		strcpy_s(txt, 1024, label.c_str());
		{
			ThreadLock _lock(type);
			scroll = new Fl_Scroll(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
			if (function != NULL) {
				Keeptrack(domain, contextualpattern);
				scroll->callback(scroll_callback, this);
			}
		}
		SaveItem(this);
		return kifTRUE;
	}

	KifElement* MethodResize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (scroll == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[366]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);

		scroll->resize(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
		return kifTRUE;
	}


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};

//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifscroll::kifexportedmethods;
map<short, scrollMethod>  Kifscroll::linkedmethods;
map<string, string> Kifscroll::infomethods;
hmap<short, bool> Kifscroll::methodassigned;

void scroll_callback(Fl_Widget*, void* data) {
	Kifscroll* slid = (Kifscroll*)data;
	KifFunction* func = slid->function->Functioncall(slid->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	slid->Setreference();
	slid->object->Setreference();
	kfunc.parameters.push_back(slid);
	kfunc.parameters.push_back(slid->object);
	kfunc.Exec(slid->context, slid->dom, kifNULL, idthread, callfunc);
	slid->Resetreference();
	slid->object->Resetreference();

}


void slider_callback(Fl_Widget*, void*);

class Kifslider : public KifWidgetCallback {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, sliderMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Slider* slider;
	char txt[1024];


	//---------------------------------------------------------------------------------------------------------------------
	Kifslider(KifCode* kifcode, KifElement* base) : KifWidgetCallback(kifcode, base, slider_type) {
		//Do not forget your variable initialisation
		slider = NULL;
	}

	void itemclose() {
		slider = NULL;
		clean();
	}

	Fl_Widget* Widget() {
		return slider;
	}

	void ResetWidget() {
		dom = NULL;
		slider = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifslider* kf = new Kifslider(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* CallbackObject() {
		return object;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		if (slider != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[367]);

		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* align = callfunc->Evaluatethread(4, domain, _thread);
		bool valueslider = callfunc->Evaluatethread(5, domain, _thread)->Boolean();
		string label = callfunc->Evaluatethread(6, domain, _thread)->String();

		strcpy_s(txt, 1024, label.c_str());
		{
			ThreadLock _lock(type);
			if (valueslider)
				slider = new Fl_Value_Slider(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
			else
				slider = new Fl_Slider(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);

			slider->align(align->Integer());
			slider->type(1);
			slider->step(1);
			if (function != NULL) {
				Keeptrack(domain, contextualpattern);
				slider->callback(slider_callback, this);
			}
		}
		SaveItem(this);
		return kifTRUE;
	}

	KifElement* MethodResize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (slider == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[368]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);

		slider->resize(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
		return kifTRUE;
	}

	KifElement* MethodBoxType(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (slider == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[368]);

		KifElement* pos = callfunc->Evaluatethread(0, domain, _thread);
		if (pos->type == kifString) {
			if (boxTypes.find(pos->String()) == boxTypes.end()) {
				string mes = KIFLTKSTRINGS[343];
				mes += pos->String();
				return Returningerror(kifcode, _thread, idcallthread, mes);
			}
			else
				slider->box((Fl_Boxtype)boxTypes[pos->String()]);
		}
		else
			slider->box((Fl_Boxtype)pos->Integer());

		return kifTRUE;
	}

	KifElement* MethodBoundary(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (slider == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[368]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);

		slider->bounds(x->Integer(), y->Integer());
		return kifTRUE;
	}

	KifElement* MethodType(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (slider == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[368]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		if (x->type == kifString) {
			string buf = x->String();
			if (slidertype.find(buf) == slidertype.end())
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[369]);
			slider->type(slidertype[buf]);
		}
		else
			slider->type(x->Integer());
		return kifTRUE;
	}


	KifElement* MethodStep(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (slider == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[368]);

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		int i = x->Integer();
		slider->step(i);
		return kifTRUE;
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		if (slider == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[368]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(slider->value());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		slider->value(x->Integer());
		return kifTRUE;
	}



	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};

void slider_callback(Fl_Widget*, void* data) {
	Kifslider* slid = (Kifslider*)data;
	KifFunction* func = slid->function->Functioncall(slid->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	slid->Setreference();
	slid->object->Setreference();
	kfunc.parameters.push_back(slid);
	kfunc.parameters.push_back(slid->object);
	kfunc.Exec(slid->context, slid->dom, kifNULL, idthread, callfunc);
	slid->Resetreference();
	slid->object->Resetreference();

}

//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifslider::kifexportedmethods;
map<short, sliderMethod>  Kifslider::linkedmethods;
map<string, string> Kifslider::infomethods;
hmap<short, bool> Kifslider::methodassigned;


void choice_callback(Fl_Widget*, void*);

class KifChoiceCallback {
public:

	KifElement* function;
	Kifchoice* choice;
	KifElement* context;
	KifDomain* dom;
	int icontext;
	int idom;
	KifElement* object;

	KifChoiceCallback(KifElement* func, KifElement* o, Kifchoice* ch, KifElement* cont, KifDomain* domain) {
		function = func;
		choice = ch;
		context = cont;
		dom = domain;
		object = o;
		dom->Setreference();
		context->Setreference();
		idom = dom->Ingarbage();
		icontext = context->Ingarbage();
	}

	~KifChoiceCallback() {
		kifGlobals->ResetGarbageElement(dom, idom);
		kifGlobals->ResetGarbageElement(context, icontext);
	}
};

class Kifchoice : public KifWidget {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, choiceMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Choice* choice;
	Fl_Menu_Item* flmenu;
	int flnbelements;

	int fontsize;
	int font;
	int fontcolor;
	char txt[1024];


	//---------------------------------------------------------------------------------------------------------------------
	Kifchoice(KifCode* kifcode, KifElement* base) : KifWidget(kifcode, base, choice_type) {
		//Do not forget your variable initialisation
		choice = NULL;
		flnbelements = 0;
		font = FL_HELVETICA;
		fontsize = 12;
		fontcolor = FL_BLACK;
		flmenu = NULL;
	}

	void itemclose() {
		if (flmenu != NULL) {
			for (int i = 0; i < flnbelements; i++) {
				free((char*)flmenu[i].text);
				delete (KifChoiceCallback*)flmenu[i].user_data();
			}
			delete[] flmenu;
		}
		choice = NULL;
		clean();
	}

	Fl_Widget* Widget() {
		return choice;
	}

	void ResetWidget() {
		choice = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifchoice* kf = new Kifchoice(kifcode, kifNULL);
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true


	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		if (choice != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[370]);

		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);
		string label = callfunc->Evaluatethread(4, domain, _thread)->String();

		strcpy_s(txt, 1024, label.c_str());
		{
			ThreadLock _lock(type);
			choice = new Fl_Choice(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
		}
		SaveItem(this);
		return kifTRUE;
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (choice == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[371]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(choice->value());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		choice->value(ke->Integer());
		return kifTRUE;
	}

	KifElement* MethodFont(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (choice == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[371]);

		if (callfunc->Size() == 0) {
			int i = choice->textfont();
			if (ifonts.find(i) != ifonts.end())
				return kifcode->Providestring(ifonts[i]);
			return kifcode->Provideinteger(i);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		if (ke->type == kifString) {
			string buf = ke->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = KIFLTKSTRINGS[275];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}

			font = fonts[buf];
		}
		else
			font = ke->Integer();

		if (font >= fontmax)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[263]);
		choice->textfont(font);
		return kifTRUE;
	}

	KifElement* MethodFontColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (choice == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[371]);

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			fontcolor = colors[buf];
		}
		else
			fontcolor = ke->Integer();

		return kifTRUE;
	}


	KifElement* MethodFontSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (choice == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[371]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(choice->textsize());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		fontsize = ke->Integer();
		return kifTRUE;
	}

	KifElement* MethodMenu(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (choice == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[0]);

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		//we expect a vector
		if (ke->aVectorContainer() == false)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[299]);
		KifElement* menu = (KifElement*)ke;
		vector<Fl_Menu_Item*> elements;
		Fl_Menu_Item* fl;
		for (int i = 0; i < menu->Size(); i++) {
			//First level
			//each element is a vector
			ke = menu->ValueOnIndex(i);
			if (ke->aVectorContainer() == false)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[300]);
			KifElement* v = ke;
			//then we analyse the sub-elements, the next element should be a vector
			if (v->Size() != 3)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[301]);

			string nn = v->ValueOnIndex(0)->String();
			char* n = strdup(STR(nn));
			//second element shoudl be a Callback
			KifElement* function = v->ValueOnIndex(1);
			if (function->type != kifCall && !function->isFunction())
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[303]);
			KifElement* object = v->ValueOnIndex(2);

			if (object->type == kifInstance)
				object = object->Thevariable(domain, _thread, NULL);
			else
				object = object->Exec(kifNULL, domain,kifNULL,_thread,callfunc);

			fl = new Fl_Menu_Item;
			fl->labeltype_ = 0;
			fl->labelfont_ = font;
			fl->labelsize_ = fontsize;
			fl->labelcolor_ = fontcolor;
			fl->text = n;
			fl->shortcut_ = 0;
			fl->callback_ = choice_callback;
			fl->user_data_ = new KifChoiceCallback(function, object, this, contextualpattern, domain);
			fl->flags = 0;
			elements.push_back(fl);
		}

		flmenu = new Fl_Menu_Item[elements.size() + 1];
		flnbelements = elements.size();

		int i;
		for (i = 0; i < elements.size(); i++) {
			flmenu[i] = *elements[i];
			delete elements[i];
		}
		flmenu[i].text = 0;
		flmenu[i].shortcut_ = 0;
		flmenu[i].callback_ = NULL;
		flmenu[i].user_data_ = 0;
		flmenu[i].flags = 0;
		flmenu[i].labeltype_ = 0;
		flmenu[i].labelfont_ = 0;
		flmenu[i].labelsize_ = 12;
		flmenu[i].labelcolor_ = FL_BLACK;
		choice->menu(flmenu);
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};

void choice_callback(Fl_Widget*, void* data) {
	KifChoiceCallback* wn = (KifChoiceCallback*)data;
	Kifchoice* kwnd = wn->choice;
	KifFunction* func = wn->function->Functioncall(wn->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	kwnd->Setreference();
	wn->object->Setreference();
	kfunc.parameters.push_back(kwnd);
	kfunc.parameters.push_back(wn->object);
	kfunc.Exec(wn->context, wn->dom, kifNULL, idthread, callfunc);
	kwnd->Resetreference();
	wn->object->Resetreference();

}

//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifchoice::kifexportedmethods;
map<short, choiceMethod>  Kifchoice::linkedmethods;
map<string, string> Kifchoice::infomethods;
hmap<short, bool> Kifchoice::methodassigned;

void browser_callback(Fl_Widget*fc, void* data);

class Kifbrowser : public KifWidgetCallback {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, browserMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Browser* browser;
	char txt[1024];

	//---------------------------------------------------------------------------------------------------------------------
	Kifbrowser(KifCode* kifcode, KifElement* base) : KifWidgetCallback(kifcode, base, browser_type) {
		//Do not forget your variable initialisation
		browser = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifbrowser* kf = new Kifbrowser(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	Fl_Widget* Widget() {
		return browser;
	}

	void ResetWidget() {
		dom = NULL;
		browser = NULL;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* CallbackObject() {
		return object;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	//----------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		//In our example, we have only two parameters

		if (browser != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[372]);

		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);
		string label = callfunc->Evaluatethread(4, domain, _thread)->String();
		strcpy_s(txt, 1024, label.c_str());
		if (function != NULL) {
			browser = new Fl_Select_Browser(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
			Keeptrack(domain, contextualpattern);
			browser->callback(browser_callback, this);
		}
		else
			browser = new Fl_Browser(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);

		return kifTRUE;
	}

	KifElement* MethodAdd(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[373]);

		string v = callfunc->Evaluatethread(0, domain, _thread)->String();
		browser->add(STR(v));
		return kifTRUE;
	}

	KifElement* MethodLoad(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[373]);

		string v = callfunc->Evaluatethread(0, domain, _thread)->String();
		browser->load(STR(v));
		return kifTRUE;
	}

	KifElement* MethodInsert(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[373]);

		int i = callfunc->Evaluatethread(0, domain, _thread)->Integer();
		string v = callfunc->Evaluatethread(1, domain, _thread)->String();
		browser->insert(i, STR(v), 0);
		return kifTRUE;
	}

	KifElement* MethodSelect(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[373]);

		string v;
		int select;
		if (callfunc->Size() == 0)
			select = browser->value();
		else
			select = callfunc->Evaluatethread(0, domain, _thread)->Integer();

		if (select >= 1 && select <= browser->size())
			v = browser->text(select);
		return kifcode->Providestring(v);
	}

	KifElement* MethodClear(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[373]);
		browser->clear();
		return kifTRUE;
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[373]);

		string v;
		int i = browser->value();
		return kifcode->Provideinteger(i);
	}

	KifElement* MethodDeselect(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[373]);
		if (callfunc->Size() == 0)
			browser->deselect();
		else {
			int select = callfunc->Evaluatethread(0, domain, _thread)->Integer();
			browser->deselect(select);
		}
		return kifTRUE;
	}

	KifElement* MethodFormatchar(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[373]);
		string select;
		if (callfunc->Size() == 0) {
			select = browser->format_char();
			return kifcode->Providestring(select);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		select = callfunc->Evaluatethread(0, domain, _thread)->String();
		if (select.size() != 0)
			browser->format_char(select[0]);
		return kifTRUE;
	}

	KifElement* MethodColumnchar(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[373]);
		string select;
		if (callfunc->Size() == 0) {
			select = browser->column_char();
			return kifcode->Providestring(select);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		select = callfunc->Evaluatethread(0, domain, _thread)->String();
		if (select.size() != 0)
			browser->column_char(select[0]);
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)

			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() {
		if (browser == NULL)
			return 0;
		return browser->size();
	}

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return Size(); }
	double Float() { return Size(); }
	bool Boolean() {
		if (Size() == 0)
			return false;
		return true;
	}

};

void browser_callback(Fl_Widget* fc, void* data) {
	Kifbrowser* slid = (Kifbrowser*)data;
	KifFunction* func = slid->function->Functioncall(slid->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	slid->Setreference();
	slid->object->Setreference();

	kfunc.parameters.push_back(slid);
	kfunc.parameters.push_back(slid->object);
	kfunc.Exec(slid->context, slid->dom, kifNULL, idthread, callfunc);
	slid->Resetreference();
	slid->object->Resetreference();

}


//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifbrowser::kifexportedmethods;
map<short, browserMethod>  Kifbrowser::linkedmethods;
map<string, string> Kifbrowser::infomethods;
hmap<short, bool> Kifbrowser::methodassigned;


class Kifetree : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, etreeMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Tree_Item* etree;
	//---------------------------------------------------------------------------------------------------------------------
	Kifetree(KifCode* kifcode, KifElement* base, Fl_Tree_Item* e) : KifBasic(kifcode, base, etree_type) {
		//Do not forget your variable initialisation
		etree = e;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		return new Kifetree(kifcode, kifNULL, etree);
	}


	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		if (kval == kifNULL) {
			etree = NULL;
			return true;
		}

		if (kval->type != etree_type)
			kifcode->Returnerror(KIFLTKSTRINGS[374]);

		etree = ((Kifetree*)kval)->etree;
		return true;
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	//----------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.

	KifElement* MethodDepth(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);

		return kifcode->Provideinteger(etree->depth());
	}

	KifElement* MethodLabel(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);

		return kifcode->Providestringraw(etree->label());
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);

		KifElement* e;
		if (callfunc->Size() == 0) {
			e = (KifElement*)etree->user_data();
			if (e == NULL)
				return kifcode->Providestringraw(etree->label());
			return e;
		}

		e = callfunc->Evaluatethread(0, domain, _thread);
		e->Setreference();
		KifElement* prec = (KifElement*)etree->user_data();
		if (prec != NULL)
			prec->Resetreference();

		if (e == kifNULL)
			etree->user_data(NULL);
		else
			etree->user_data(e);
		return kifTRUE;
	}

	KifElement* MethodClean(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);

		void* val = etree->user_data();
		if (val != NULL)
			((KifElement*)val)->Resetreference();
		etree->user_data(NULL);
		return kifTRUE;
	}

	KifElement* MethodIsclose(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);
		int i = etree->is_close();
		return kifcode->Provideinteger(i);
	}

	KifElement* MethodIsopen(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);
		int i = etree->is_open();
		return kifcode->Provideinteger(i);
	}

	KifElement* MethodIsactive(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);
		int i = etree->is_active();
		return kifcode->Provideinteger(i);
	}

	KifElement* MethodIsroot(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);
		int i = etree->is_root();
		return kifcode->Provideinteger(i);
	}

	KifElement* MethodIsselected(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);
		int i = etree->is_selected();
		return kifcode->Provideinteger(i);
	}

	KifElement* MethodItemBGColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {

		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(etree->labelbgcolor());

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return kifcode->Returnerror(buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		etree->labelbgcolor(color);
		return kifTRUE;
	}

	KifElement* MethodItemFGColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {

		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(etree->labelfgcolor());

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return kifcode->Returnerror(buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		etree->labelfgcolor(color);
		return kifTRUE;
	}

	KifElement* MethodItemFont(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {

		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(etree->labelfont());

		int ft;
		KifElement* f = callfunc->Evaluatethread(0, domain, _thread);
		if (f->type == kifString) {
			string buf = f->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = KIFLTKSTRINGS[275];
				return kifcode->Returnerror(buf);
			}
			ft = fonts[buf];
		}
		else {
			ft = f->Integer();
			if (ft >= fontmax)
				return kifcode->Returnerror(KIFLTKSTRINGS[263]);
		}

		etree->labelfont(ft);
		return kifTRUE;
	}

	KifElement* MethodItemSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);

		if (callfunc->Size() == 0) {
			int i = etree->labelsize();
			return kifcode->Provideinteger(i);
		}

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		etree->labelsize(ke->Integer());
		return kifTRUE;
	}

	KifElement* MethodActivate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);
		etree->activate();
		return kifTRUE;
	}

	KifElement* MethodDeactivate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);
		etree->deactivate();
		return kifTRUE;
	}

	KifElement* MethodNext(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);
		return new Kifetree(kifcode, NULL, etree->next());
	}

	KifElement* MethodPrevious(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);
		return new Kifetree(kifcode, NULL, etree->prev());
	}

	KifElement* MethodParent(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);
		return new Kifetree(kifcode, NULL, etree->parent());
	}

	KifElement* MethodChild(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);
		int i = callfunc->Evaluatethread(0, domain, _thread)->Integer();
		if (i > etree->children())
			return kifcode->Returnerror(KIFLTKSTRINGS[376]);
		return new Kifetree(kifcode, NULL, etree->child(i));
	}

	KifElement* MethodChildren(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror(KIFLTKSTRINGS[375]);
		return kifcode->Provideinteger(etree->children());
	}


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)

			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* domain, KifElement* value, int idthread, KifCallFunction* func) {
		if (etree == NULL)
			return kifNULL;
		return this;
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() {
		return 0;
	}

	//How to interprete your class according to the basic following types:
	string String() {
		if (etree == NULL)
			return "";
		return etree->label();
	}

	long Integer() {
		if (etree == NULL)
			return 0;
		KifElement* e = (KifElement*)etree->user_data();
		return e->Integer();
	}

	double Float() {
		if (etree == NULL)
			return 0;
		KifElement* e = (KifElement*)etree->user_data();
		return e->Float();
	}

	bool Boolean() {
		if (etree == NULL)
			return false;
		return true;
	}

	KifElement* same(KifElement* a) {
		if (a == kifNULL) {
			if (etree == NULL)
				return kifTRUE;
			return kifFALSE;
		}
		if (a->type != type)
			return kifFALSE;
		Kifetree* w = (Kifetree*)a;
		if (etree == w->etree)
			return kifTRUE;
		return kifFALSE;
	}

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifetree::kifexportedmethods;
map<short, etreeMethod>  Kifetree::linkedmethods;
map<string, string> Kifetree::infomethods;
hmap<short, bool> Kifetree::methodassigned;

//-----------------------------------------------------------------------------------
void counter_callback(Fl_Widget* fc, void* data);
class Kifcounter : public KifWidgetCallback {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, counterMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Counter* counter;
	char txt[1024];

	//---------------------------------------------------------------------------------------------------------------------
	Kifcounter(KifCode* kifcode, KifElement* base) : KifWidgetCallback(kifcode, base, counter_type) {
		//Do not forget your variable initialisation
		counter = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifcounter* kf = new Kifcounter(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	Fl_Widget* Widget() {
		return counter;
	}

	void ResetWidget() {
		dom = NULL;
		counter = NULL;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* CallbackObject() {
		return object;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	//----------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		//In our example, we have only two parameters

		if (counter != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[377]);

		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);
		string label = callfunc->Evaluatethread(4, domain, _thread)->String();
		strcpy_s(txt, 1024, label.c_str());
		counter = new Fl_Counter(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
		if (function != NULL) {
			if (function->Size() != 2)
				return kifcode->Returnerror(KIFLTKSTRINGS[315]);
			Keeptrack(domain, contextualpattern);
			counter->callback(counter_callback, this);
		}
		return kifTRUE;
	}

	
	KifElement* MethodStep(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[378]);

		double kstep = callfunc->Evaluatethread(0, domain, _thread)->Float();
		counter->step(kstep);
		return kifTRUE;
	}

	KifElement* MethodLargeStep(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[378]);

		double kstep = callfunc->Evaluatethread(0, domain, _thread)->Float();
		counter->lstep(kstep);
		return kifTRUE;
	}

	KifElement* MethodSteps(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[378]);

		double nstep = callfunc->Evaluatethread(0, domain, _thread)->Float();
		double Lstep = callfunc->Evaluatethread(1, domain, _thread)->Float();
		counter->step(nstep, Lstep);
		return kifTRUE;
	}

	KifElement* MethodType(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[378]);

		bool nstep = callfunc->Evaluatethread(0, domain, _thread)->Boolean();
		if (nstep)
			counter->type(FL_NORMAL_COUNTER);
		else
			counter->type(FL_SIMPLE_COUNTER);
		return kifTRUE;
	}

	KifElement* MethodBoundary(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[378]);

		double a, b;
		if (callfunc->Size() == 0) {
			a = counter->minimum();
			b = counter->maximum();
			KifVectorFloat* v = new KifVectorFloat(callfunc->kifcode, NULL);
			v->values.push_back(a);
			v->values.push_back(b);
			return v;
		}

		a = callfunc->Evaluatethread(0, domain, _thread)->Float();
		b = callfunc->Evaluatethread(1, domain, _thread)->Float();
		counter->bounds(a, b);
		return kifTRUE;
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[378]);

		double a;
		if (callfunc->Size() == 0) {
			a = counter->value();
			return callfunc->kifcode->Providefloat(a);
		}

		a = callfunc->Evaluatethread(0, domain, _thread)->Float();
		counter->value(a);
		return kifTRUE;
	}

	KifElement* MethodFont(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[378]);

		if (callfunc->Size() == 0) {
			int i = counter->textfont();
			if (ifonts.find(i) != ifonts.end())
				return kifcode->Providestring(ifonts[i]);
			return kifcode->Provideinteger(i);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		if (ke->type == kifString) {
			string buf = ke->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = KIFLTKSTRINGS[275];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}

			counter->textfont(fonts[buf]);
		}
		else {
			int font = ke->Integer();
			if (font >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[263]);
			counter->textfont(font);
		}
		return kifTRUE;
	}

	KifElement* MethodFontSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[378]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(counter->textsize());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		counter->textsize(ke->Integer());
		return kifTRUE;
	}

	KifElement* MethodTextColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[378]);

		if (callfunc->Size() == 0) {
			Fl_Color color = counter->textcolor();
			return kifcode->Provideinteger(color);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			counter->textcolor(colors[buf]);
		}
		else {
			unsigned int c = ke->Integer();
			counter->textcolor(c);
		}

		return kifTRUE;
	}
	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)

			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() {
		return 0;
	}

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return Size(); }
	double Float() { return Size(); }
	bool Boolean() {
		if (Size() == 0)
			return false;
		return true;
	}

};

//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifcounter::kifexportedmethods;
map<short, counterMethod>  Kifcounter::linkedmethods;
map<string, string> Kifcounter::infomethods;
hmap<short, bool> Kifcounter::methodassigned;

void counter_callback(Fl_Widget*, void* data) {
	Kifcounter* cnt = (Kifcounter*)data;
	KifFunction* func = cnt->function->Functioncall(cnt->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	cnt->Setreference();
	cnt->object->Setreference();
	kfunc.parameters.push_back(cnt);
	kfunc.parameters.push_back(cnt->object);
	kfunc.Exec(cnt->context, cnt->dom, kifNULL, idthread, callfunc);
	cnt->Resetreference();
	cnt->object->Resetreference();
}

//-----------------------------------------------------------------------------------
void progress_callback(Fl_Widget* fc, void* data);
class Kifprogress : public KifWidgetCallback {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, progressMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Progress* progress;
	char txt[1024];

	//---------------------------------------------------------------------------------------------------------------------
	Kifprogress(KifCode* kifcode, KifElement* base) : KifWidgetCallback(kifcode, base, progress_type) {
		//Do not forget your variable initialisation
		progress = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifprogress* kf = new Kifprogress(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	Fl_Widget* Widget() {
		return progress;
	}

	void ResetWidget() {
		dom = NULL;
		progress = NULL;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* CallbackObject() {
		return object;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	//----------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		//In our example, we have only two parameters

		if (progress != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[379]);

		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);
		string label = callfunc->Evaluatethread(4, domain, _thread)->String();
		strcpy_s(txt, 1024, label.c_str());
		progress = new Fl_Progress(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
		if (function != NULL) {
			if (function->Size() != 2)
				return kifcode->Returnerror(KIFLTKSTRINGS[315]);
			Keeptrack(domain, contextualpattern);
			progress->callback(progress_callback, this);
		}
		return kifTRUE;
	}

	KifElement* MethodMinimum(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (progress == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[380]);

		double a;
		if (callfunc->Size() == 0) {
			a = progress->minimum();
			return callfunc->kifcode->Providefloat(a);
		}

		a = callfunc->Evaluatethread(0, domain, _thread)->Float();
		progress->minimum(a);
		return kifTRUE;
	}

	KifElement* MethodMaximum(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (progress == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[380]);

		double a;
		if (callfunc->Size() == 0) {
			a = progress->maximum();
			return callfunc->kifcode->Providefloat(a);
		}

		a = callfunc->Evaluatethread(0, domain, _thread)->Float();
		progress->maximum(a);
		return kifTRUE;
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (progress == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[380]);

		double a;
		if (callfunc->Size() == 0) {
			a = progress->value();
			return callfunc->kifcode->Providefloat(a);
		}

		a = callfunc->Evaluatethread(0, domain, _thread)->Float();
		progress->value(a);
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)

			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() {
		return 0;
	}

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return Size(); }
	double Float() { return Size(); }
	bool Boolean() {
		if (Size() == 0)
			return false;
		return true;
	}

};

//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifprogress::kifexportedmethods;
map<short, progressMethod>  Kifprogress::linkedmethods;
map<string, string> Kifprogress::infomethods;
hmap<short, bool> Kifprogress::methodassigned;

void progress_callback(Fl_Widget*, void* data) {
	Kifprogress* cnt = (Kifprogress*)data;
	KifFunction* func = cnt->function->Functioncall(cnt->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	cnt->Setreference();
	cnt->object->Setreference();
	kfunc.parameters.push_back(cnt);
	kfunc.parameters.push_back(cnt->object);
	kfunc.Exec(cnt->context, cnt->dom, kifNULL, idthread, callfunc);
	cnt->Resetreference();
	cnt->object->Resetreference();
}


//-----------------------------------------------------------------------------------
void wtree_callback(Fl_Widget* fc, void* data);

class Kifteratorwtree : public KifIteration {
public:
	Fl_Tree* value;
	Fl_Tree_Item* e;
	Kifetree res;

	Kifteratorwtree(KifCode* klc, KifElement* kp, Fl_Tree* v) : res(NULL, NULL, NULL), KifIteration(klc, kp) {
		value = v;
		e = NULL;
		res.kifcode = klc;
	}


	//The next methods are the ones that should be derived to add iterator capacities to KifTemplateName
	KifElement* IteratorKey();
	KifElement* IteratorValue();
	void Next();
	KifElement* End();
	KifElement* Begin();
};


class Kifwtree : public KifWidgetCallback {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, wtreeMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_Tree* wtree;
	char txt[1024];

	//---------------------------------------------------------------------------------------------------------------------
	Kifwtree(KifCode* kifcode, KifElement* base) : KifWidgetCallback(kifcode, base, wtree_type) {
		//Do not forget your variable initialisation
		wtree = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kifwtree* kf = new Kifwtree(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	Fl_Widget* Widget() {
		return wtree;
	}

	void ResetWidget() {
		dom = NULL;
		wtree = NULL;
	}

	KifElement* Newiterator(bool rev) {
		Kifteratorwtree* iter = new Kifteratorwtree(kifcode, NULL, wtree);
		iter->reverse = rev;
		//Your initialisation here for your iterator object
		return iter;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* CallbackObject() {
		return object;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	//----------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		//In our example, we have only two parameters

		if (wtree != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[381]);

		//0 is the first parameter and so on...
		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* wx = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* hy = callfunc->Evaluatethread(3, domain, _thread);
		string label = callfunc->Evaluatethread(4, domain, _thread)->String();
		strcpy_s(txt, 1024, label.c_str());
		wtree = new Fl_Tree(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
		wtree->when(FL_WHEN_RELEASE);
		if (function != NULL) {
			if (function->Size() != 4)
				return kifcode->Returnerror(KIFLTKSTRINGS[315]);
			Keeptrack(domain, contextualpattern);
			wtree->callback(wtree_callback, this);
		}
		return kifTRUE;
	}

	KifElement* MethodClear(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);
		wtree->clear();
		return kifTRUE;
	}

	KifElement* MethodAdd(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);
		KifElement* e = callfunc->Evaluatethread(0, domain, _thread);

		string v;
		Fl_Tree_Item* fti = NULL;
		if (e->type == etree_type) {
			v = callfunc->Evaluatethread(1, domain, _thread)->String();
			fti = wtree->add(((Kifetree*)e)->etree, STR(v));
		}
		else {
			v = e->String();
			if (v != "")
				fti = wtree->add(STR(v));
		}
		if (fti == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[383]);
		return new Kifetree(kifcode, NULL, fti);
	}

	KifElement* MethodIsclose(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);
		KifElement* e = callfunc->Evaluatethread(0, domain, _thread);

		int ans;
		if (e->type == etree_type)
			ans = wtree->is_close(((Kifetree*)e)->etree);
		else {
			string v = e->String();
			ans = wtree->is_close(STR(v));
		}
		return kifcode->Provideinteger(ans);
	}

	KifElement* MethodClose(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);
		KifElement* e = callfunc->Evaluatethread(0, domain, _thread);

		int ans;
		if (e->type == etree_type)
			ans = wtree->close(((Kifetree*)e)->etree);
		else {
			string p = e->String();
			ans = wtree->close(STR(p));
		}

		return kifcode->Provideinteger(ans);
	}

	KifElement* MethodOpen(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);
		KifElement* e = callfunc->Evaluatethread(0, domain, _thread);

		int ans;
		if (e->type == etree_type)
			ans = wtree->open(((Kifetree*)e)->etree);
		else {
			string p = e->String();
			ans = wtree->open(STR(p));
		}

		return kifcode->Provideinteger(ans);
	}

	KifElement* MethodInsert(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		KifElement* e = callfunc->Evaluatethread(0, domain, _thread);

		string v;
		if (e->type != etree_type)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[384]);
		string name = callfunc->Evaluatethread(1, domain, _thread)->String();
		int pos = callfunc->Evaluatethread(2, domain, _thread)->Integer();
		wtree->insert(((Kifetree*)e)->etree, STR(name), pos);
		return kifTRUE;
	}

	KifElement* MethodItemBGColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wtree->item_labelbgcolor());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		wtree->item_labelbgcolor(color);
		return kifTRUE;
	}

	KifElement* MethodItemFGColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wtree->item_labelfgcolor());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		wtree->item_labelfgcolor(color);
		return kifTRUE;
	}

	KifElement* MethodItemFont(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wtree->item_labelfont());

		threadtosetvalue(contextualpattern, domain, callfunc);
		int ft;
		KifElement* f = callfunc->Evaluatethread(0, domain, _thread);
		if (f->type == kifString) {
			string buf = f->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = KIFLTKSTRINGS[275];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			ft = fonts[buf];
		}
		else {
			ft = f->Integer();
			if (ft >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[263]);
		}

		wtree->item_labelfont(ft);
		return kifTRUE;
	}

	KifElement* MethodItemSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		if (callfunc->Size() == 0) {
			int i = wtree->item_labelsize();
			return kifcode->Provideinteger(i);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		wtree->item_labelsize(ke->Integer());
		return kifTRUE;
	}

	KifElement* MethodConnectorColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wtree->connectorcolor());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = KIFLTKSTRINGS[256];
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();

		wtree->connectorcolor(color);
		return kifTRUE;
	}

	KifElement* MethodMarginLeft(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		if (callfunc->Size() == 0) {
			int i = wtree->marginleft();
			return kifcode->Provideinteger(i);
		}
		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		wtree->marginleft(ke->Integer());
		return kifTRUE;
	}

	KifElement* MethodMarginTop(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		if (callfunc->Size() == 0) {
			int i = wtree->margintop();
			return kifcode->Provideinteger(i);
		}
		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		wtree->margintop(ke->Integer());
		return kifTRUE;
	}

	KifElement* MethodSelectmode(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wtree->selectmode());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		int style = ke->Integer();
		wtree->selectmode((Fl_Tree_Select)style);
		return kifTRUE;
	}

	KifElement* MethodSortOrder(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		if (callfunc->Size() == 0) {
			int i = wtree->sortorder();
			return kifcode->Provideinteger(i);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		int style = ke->Integer();
		wtree->sortorder((Fl_Tree_Sort)style);
		return kifTRUE;
	}


	KifElement* MethodConnectorStyle(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		if (callfunc->Size() == 0) {
			int i = wtree->connectorstyle();
			return kifcode->Provideinteger(i);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		int style = ke->Integer();
		wtree->connectorstyle((Fl_Tree_Connector)style);
		return kifTRUE;
	}

	KifElement* MethodConnectorWidth(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		if (callfunc->Size() == 0) {
			int i = wtree->connectorwidth();
			return kifcode->Provideinteger(i);
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		wtree->connectorwidth(callfunc->Evaluatethread(0, domain, _thread)->Integer());
		return kifTRUE;
	}

	KifElement* MethodInsertAbove(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		KifElement* e = callfunc->Evaluatethread(0, domain, _thread);

		string v;
		if (e->type != etree_type)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[384]);
		string name = callfunc->Evaluatethread(1, domain, _thread)->String();
		wtree->insert_above(((Kifetree*)e)->etree, STR(name));
		return kifTRUE;
	}

	KifElement* MethodRemove(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		KifElement* e = callfunc->Evaluatethread(0, domain, _thread);

		string v;
		if (e->type != etree_type)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[384]);
		Kifetree* ie = (Kifetree*)e;
		if (ie->etree == NULL)
			return kifFALSE;
		wtree->remove(ie->etree);
		return kifTRUE;
	}


	KifElement* MethodTreeClear(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);
		wtree->clear();
		return kifTRUE;
	}

	KifElement* MethodClicked(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);
		Fl_Tree_Item* e = wtree->item_clicked();
		return new Kifetree(kifcode, NULL, e);
	}

	KifElement* MethodFirst(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);
		Fl_Tree_Item* e = wtree->first();
		return new Kifetree(kifcode, NULL, e);
	}

	KifElement* MethodRoot(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);
		Fl_Tree_Item* e = wtree->root();
		return new Kifetree(kifcode, NULL, e);
	}

	KifElement* MethodRootLabel(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		string name = callfunc->Evaluatethread(0, domain, _thread)->String();
		wtree->root_label(STR(name));
		return kifTRUE;
	}

	KifElement* MethodFind(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);

		string path = callfunc->Evaluatethread(0, domain, _thread)->String();
		Fl_Tree_Item* e = wtree->find_item(STR(path));
		return new Kifetree(kifcode, NULL, e);
	}

	KifElement* MethodLast(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);
		Fl_Tree_Item* e = wtree->last();
		return new Kifetree(kifcode, NULL, e);
	}


	KifElement* MethodNext(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);
		Fl_Tree_Item* e;
		if (callfunc->Size() == 0)
			e = wtree->next();
		else {
			KifElement* etr = callfunc->Evaluatethread(0, domain, _thread);
			if (etr->type != etree_type)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[384]);
			e = wtree->next(((Kifetree*)etr)->etree);
		}
		return new Kifetree(kifcode, NULL, e);
	}

	KifElement* MethodPrevious(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[382]);
		Fl_Tree_Item* e;
		if (callfunc->Size() == 0)
			e = wtree->prev();
		else {
			KifElement* etr = callfunc->Evaluatethread(0, domain, _thread);
			if (etr->type != etree_type)
				return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[384]);
			e = wtree->prev(((Kifetree*)etr)->etree);
		}
		return new Kifetree(kifcode, NULL, e);
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)

			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() {
		return 0;
	}

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return Size(); }
	double Float() { return Size(); }
	bool Boolean() {
		if (Size() == 0)
			return false;
		return true;
	}

};

void wtree_callback(Fl_Widget* fc, void* data) {
	Kifwtree* tree = (Kifwtree*)data;
	KifFunction* func = tree->function->Functioncall(tree->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	tree->Setreference();
	tree->object->Setreference();
	int i = tree->wtree->callback_reason();

	Fl_Tree_Item* e = tree->wtree->item_clicked();
	Kifetree* etr = new Kifetree(tree->kifcode, NULL, e);
	etr->Setreference();
	KifInteger* reason = tree->kifcode->Provideinteger(i);
	reason->Setreference();
	kfunc.parameters.push_back(tree);
	kfunc.parameters.push_back(etr);
	kfunc.parameters.push_back(reason);
	kfunc.parameters.push_back(tree->object);
	kfunc.Exec(tree->context, tree->dom, kifNULL, idthread, callfunc);
	tree->Resetreference();
	tree->object->Resetreference();
	etr->Resetreference();
	reason->Resetreference();

}


KifElement* Kifteratorwtree::IteratorKey() {
	return &res;
}

KifElement* Kifteratorwtree::IteratorValue() {
	return &res;
}

void Kifteratorwtree::Next() {
	//reverse denotes the direction of iteration (from beginning to end or reverse)
	value->next(e);
}

KifElement* Kifteratorwtree::End() {
	if (e == NULL)
		return kifTRUE;
	return kifFALSE;
}
KifElement* Kifteratorwtree::Begin() {
	//Your initialisation
	//reverse denotes the direction of iteration (from beginning to end or reverse)
	if (value == NULL)
		return kifFALSE;
	e = value->first();
	res.etree = e;
	return kifTRUE;
}

//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifwtree::kifexportedmethods;
map<short, wtreeMethod>  Kifwtree::linkedmethods;
map<string, string> Kifwtree::infomethods;
hmap<short, bool> Kifwtree::methodassigned;

void filebrowser_callback(Fl_File_Chooser *fc, void* data);

class Kiffilebrowser : public KifWidgetCallback {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, filebrowserMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Fl_File_Chooser* filebrowser;

	char directory[1024];
	char filter[1024];
	char txt[1024];

	//---------------------------------------------------------------------------------------------------------------------
	Kiffilebrowser(KifCode* kifcode, KifElement* base) : KifWidgetCallback(kifcode, base, filebrowser_type) {
		//Do not forget your variable initialisation
		filebrowser = NULL;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL

	KifElement* Copy(KifDomain* base, KifElement* dom = kifNULL) {
		Kiffilebrowser* kf = new Kiffilebrowser(kifcode, kifNULL);
		kf->function = function;
		return kf;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* CallbackObject() {
		return object;
	}


	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	//----------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		//In our example, we have only two parameters

		if (filebrowser != NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[385]);


		//0 is the first parameter and so on...
		string d = callfunc->Evaluatethread(0, domain, _thread)->String();
		string f = callfunc->Evaluatethread(1, domain, _thread)->String();
		int t = callfunc->Evaluatethread(2, domain, _thread)->Integer();
		string label = callfunc->Evaluatethread(3, domain, _thread)->String();

		strcpy_s(txt, 1024, label.c_str());
		strcpy_s(directory, 1024, d.c_str());
		strcpy_s(filter, 1024, f.c_str());
		{
			ThreadLock _lock(type);
			filebrowser = new Fl_File_Chooser(directory, filter, t, txt);
			Keeptrack(domain, contextualpattern);
			//filebrowser->callback(filebrowser_callback,this);
			filebrowser->show();
			while (filebrowser->shown())
				Fl::wait();

			if (function != NULL) {
				filebrowser_callback(filebrowser, this);
				return kifTRUE;
			}

			string buf;
			if (filebrowser->value() != NULL)
				buf = filebrowser->value();
			delete filebrowser;
			return kifcode->Providestring(buf);
		}
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		//In our example, we have only two parameters

		if (filebrowser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[386]);

		if (filebrowser->value() == NULL)
			return kifcode->Providestringraw("");

		string buf = filebrowser->value();
		return kifcode->Providestring(buf);
	}

	KifElement* MethodClose(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		if (filebrowser == NULL)
			return kifFALSE;

		delete filebrowser;
		filebrowser = NULL;
		return kifTRUE;
	}

#ifdef MACUI
	KifElement* MethodCharge(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		string d = callfunc->Evaluatethread(0, domain, _thread)->String();
		bool r = chargefichiermac(d.c_str());
		if (r)
			return kifTRUE;
		return kifFALSE;
	}


	KifElement* MethodOuvre(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		const char* m = lirefichiermac();
		if (m == NULL)
			return kifNULL;
		return kifcode->Providestringraw(m);
	}
#endif

	KifElement* MethodOk(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters

		if (filebrowser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, KIFLTKSTRINGS[386]);

		if (filebrowser->shown())
			return kifFALSE;
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)

			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFLTKSTRINGS[265]
			//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFLTKSTRINGS[268] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return txt; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};

void filebrowser_callback(Fl_File_Chooser *fc, void* data) {
	Kiffilebrowser* slid = (Kiffilebrowser*)data;
	KifFunction* func = slid->function->Functioncall(slid->dom);
	KifCallFunctionGeneral kfunc(NULL,NULL,func->Name(), func);
	kfunc.kifcode = func->kifcode;
	threadblocinit;

	slid->Setreference();
	slid->object->Setreference();

	kfunc.parameters.push_back(slid);
	kfunc.parameters.push_back(slid->object);
	kfunc.Exec(slid->context, slid->dom, kifNULL, idthread, callfunc);
	slid->Resetreference();
	slid->object->Resetreference();

}

//We need to declare once again our local definitions.
map<short, KifCallMethod> Kiffilebrowser::kifexportedmethods;
map<short, filebrowserMethod>  Kiffilebrowser::linkedmethods;
map<string, string> Kiffilebrowser::infomethods;
hmap<short, bool> Kiffilebrowser::methodassigned;

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific iterator implementation...
//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifwindowElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifwindow* local = new Kifwindow(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifboxElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifbox* local = new Kifbox(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifchoiceElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifchoice* local = new Kifchoice(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifbrowserElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifbrowser* local = new Kifbrowser(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifetreeElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifetree* local = new Kifetree(kifcode, variable, NULL);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifwtreeElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifwtree* local = new Kifwtree(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekiffilebrowserElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kiffilebrowser* local = new Kiffilebrowser(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifbuttonElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifbutton* local = new Kifbutton(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekiftabsElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kiftabs* local = new Kiftabs(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifgroupElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifgroup* local = new Kifgroup(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifinputElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifinput* local = new Kifinput(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekiftableElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kiftable* local = new Kiftable(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifimageElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifimage* local = new Kifimage(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifbitmapElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifbitmap* local = new Kifbitmap(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifoutputElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifoutput* local = new Kifoutput(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifeditorElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifeditor* local = new Kifeditor(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifscrollElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifscroll* local = new Kifscroll(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifsliderElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifslider* local = new Kifslider(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifcounterElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifcounter* local = new Kifcounter(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

static KifElement* CreatekifprogressElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifprogress* local = new Kifprogress(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitializationWindow(string name, windowMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifwindow::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifwindow::linkedmethods[idname] = func;
	Kifwindow::methodassigned[idname] = true;
	Kifwindow::infomethods[name] = infos;
}

static void MethodInitializationBox(string name, boxMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifbox::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifbox::linkedmethods[idname] = func;
	Kifbox::methodassigned[idname] = true;
	Kifbox::infomethods[name] = infos;
}

static void MethodInitializationtabs(string name, tabsMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kiftabs::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kiftabs::linkedmethods[idname] = func;
	Kiftabs::methodassigned[idname] = true;
	Kiftabs::infomethods[name] = infos;
}

static void MethodInitializationgroup(string name, groupMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifgroup::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifgroup::linkedmethods[idname] = func;
	Kifgroup::methodassigned[idname] = true;
	Kifgroup::infomethods[name] = infos;
}

static void MethodInitializationImage(string name, imageMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifimage::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifimage::linkedmethods[idname] = func;
	Kifimage::methodassigned[idname] = true;
	Kifimage::infomethods[name] = infos;
}

static void MethodInitializationBitmap(string name, bitmapMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifbitmap::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifbitmap::linkedmethods[idname] = func;
	Kifbitmap::methodassigned[idname] = true;
	Kifbitmap::infomethods[name] = infos;
}

static void MethodInitializationChoice(string name, choiceMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifchoice::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifchoice::linkedmethods[idname] = func;
	Kifchoice::methodassigned[idname] = true;
	Kifchoice::infomethods[name] = infos;
}

static void MethodInitializationETree(string name, etreeMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifetree::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifetree::linkedmethods[idname] = func;
	Kifetree::methodassigned[idname] = true;
	Kifetree::infomethods[name] = infos;
}

static void MethodInitializationWTree(string name, wtreeMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifwtree::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifwtree::linkedmethods[idname] = func;
	Kifwtree::methodassigned[idname] = true;
	Kifwtree::infomethods[name] = infos;
}

static void MethodInitializationFilebrowser(string name, filebrowserMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kiffilebrowser::kifexportedmethods[idname] = kcm;
	Kiffilebrowser::linkedmethods[idname] = func;
	Kiffilebrowser::methodassigned[idname] = true;
	Kiffilebrowser::infomethods[name] = infos;
}

static void MethodInitializationBrowser(string name, browserMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifbrowser::kifexportedmethods[idname] = kcm;
	Kifbrowser::linkedmethods[idname] = func;
	Kifbrowser::methodassigned[idname] = true;
	Kifbrowser::infomethods[name] = infos;
}

static void MethodInitializationInput(string name, inputMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifinput::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifinput::linkedmethods[idname] = func;
	Kifinput::methodassigned[idname] = true;
	Kifinput::infomethods[name] = infos;
}

static void MethodInitializationTable(string name, tableMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kiftable::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kiftable::linkedmethods[idname] = func;
	Kiftable::methodassigned[idname] = true;
	Kiftable::infomethods[name] = infos;
}

static void MethodInitializationEditor(string name, editorMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifeditor::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifeditor::linkedmethods[idname] = func;
	Kifeditor::methodassigned[idname] = true;
	Kifeditor::infomethods[name] = infos;
}

static void MethodInitializationScroll(string name, scrollMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifscroll::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifscroll::linkedmethods[idname] = func;
	Kifscroll::methodassigned[idname] = true;
	Kifscroll::infomethods[name] = infos;
}

static void MethodInitializationSlider(string name, sliderMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifslider::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifslider::linkedmethods[idname] = func;
	Kifslider::methodassigned[idname] = true;
	Kifslider::infomethods[name] = infos;
}

static void MethodInitializationCounter(string name, counterMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifcounter::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifcounter::linkedmethods[idname] = func;
	Kifcounter::methodassigned[idname] = true;
	Kifcounter::infomethods[name] = infos;
}

static void MethodInitializationProgress(string name, progressMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifprogress::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifprogress::linkedmethods[idname] = func;
	Kifprogress::methodassigned[idname] = true;
	Kifprogress::infomethods[name] = infos;
}

static void MethodInitializationOutput(string name, outputMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifoutput::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifoutput::linkedmethods[idname] = func;
	Kifoutput::methodassigned[idname] = true;
	Kifoutput::infomethods[name] = infos;
}

static void MethodInitializationButton(string name, buttonMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifbutton::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifbutton::linkedmethods[idname] = func;
	Kifbutton::methodassigned[idname] = true;
	Kifbutton::infomethods[name] = infos;
}

static void SaveItem(KifWidget* e) {
	if (current_window.size() != 0) {
		Kifwindow* w = current_window.back();
		w->items.push_back(e->idgarbage);
	}
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifltk(KifCode* kifcode, string version) {
		stopall = false;
		if (version != KIFVERSION) {
			cerr << KIFLTKSTRINGS[387] << endl;
			return false;
		}
		kifcode->KifInitLib();

		///The new type will be: window. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifwindowElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifwindow)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifwindowElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType(KIFLTKSTRINGS[388], CreatekifwindowElement);
		if (!create_variables(kifcode))
			return false;
		window_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[389], CreatekifboxElement);
		if (res == kifVOID)
			return false;
		box_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[390], CreatekiftabsElement);
		if (res == kifVOID)
			return false;
		tabs_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[391], CreatekifgroupElement);
		if (res == kifVOID)
			return false;
		group_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[392], CreatekifchoiceElement);
		if (res == kifVOID)
			return false;
		choice_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[307], CreatekifbuttonElement);
		if (res == kifVOID)
			return false;
		button_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[393], CreatekifeditorElement);
		if (res == kifVOID)
			return false;
		editor_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[394], CreatekifinputElement);
		if (res == kifVOID)
			return false;
		input_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[395], CreatekiftableElement);
		if (res == kifVOID)
			return false;
		table_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[396], CreatekifoutputElement);
		if (res == kifVOID)
			return false;
		output_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[397], CreatekifscrollElement);
		if (res == kifVOID)
			return false;
		scroll_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[398], CreatekifsliderElement);
		if (res == kifVOID)
			return false;
		slider_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[399], CreatekifcounterElement);
		if (res == kifVOID)
			return false;
		counter_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[400], CreatekifprogressElement);
		if (res == kifVOID)
			return false;
		progress_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[401], CreatekifimageElement);
		if (res == kifVOID)
			return false;
		image_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[402], CreatekifbitmapElement);
		if (res == kifVOID)
			return false;
		bitmap_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[403], CreatekiffilebrowserElement);
		if (res == kifVOID)
			return false;
		filebrowser_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[404], CreatekifbrowserElement);
		if (res == kifVOID)
			return false;
		browser_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[405], CreatekifetreeElement);
		if (res == kifVOID)
			return false;
		etree_type = res;

		res = KifAddNewType(KIFLTKSTRINGS[406], CreatekifwtreeElement);
		if (res == kifVOID)
			return false;
		wtree_type = res;

		return true;
	}


	Exporting bool kifltk_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifltk(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitializationWindow(KIFLTKSTRINGS[407], &Kifwindow::MethodCreate, P_THREE | P_FIVE, KIFLTKSTRINGS[408]);
		MethodInitializationWindow(KIFLTKSTRINGS[409], &Kifwindow::MethodBegin, P_THREE | P_FIVE, KIFLTKSTRINGS[410]);
		MethodInitializationWindow(KIFLTKSTRINGS[329], &Kifwindow::MethodEnd, P_NONE, KIFLTKSTRINGS[411]);
		MethodInitializationWindow(KIFLTKSTRINGS[412], &Kifwindow::MethodRun, P_NONE, KIFLTKSTRINGS[413]);
		MethodInitializationWindow(KIFLTKSTRINGS[414], &Kifwindow::MethodClose, P_NONE, KIFLTKSTRINGS[415]);
		MethodInitializationWindow(KIFLTKSTRINGS[416], &Kifwindow::MethodOnClose, P_TWO, KIFLTKSTRINGS[417]);
		MethodInitializationWindow(KIFLTKSTRINGS[418], &Kifwindow::MethodOntime, P_THREE, KIFLTKSTRINGS[419]);

		MethodInitializationWindow(KIFLTKSTRINGS[420], &Kifwindow::MethodSetFonts, P_NONE, KIFLTKSTRINGS[421]);
		MethodInitializationWindow(KIFLTKSTRINGS[422], &Kifwindow::MethodGetFont, P_ONE, KIFLTKSTRINGS[423]);
		MethodInitializationWindow(KIFLTKSTRINGS[424], &Kifwindow::MethodGetFontSizes, P_ONE, KIFLTKSTRINGS[425]);
		MethodInitializationWindow(KIFLTKSTRINGS[426], &Kifwindow::MethodNbFonts, P_NONE, KIFLTKSTRINGS[427]);
		MethodInitializationWindow(KIFLTKSTRINGS[428], &Kifwindow::MethodPlot, P_TWO | P_THREE, KIFLTKSTRINGS[429]);
		MethodInitializationWindow(KIFLTKSTRINGS[430], &Kifwindow::PlotCoords, P_THREE, KIFLTKSTRINGS[431]);

		MethodInitializationWindow(KIFLTKSTRINGS[432], &Kifwindow::MethodAsk, P_THREE | P_FOUR | P_FIVE, KIFLTKSTRINGS[433]);
		MethodInitializationWindow(KIFLTKSTRINGS[434], &Kifwindow::MethodAlert, P_ONE, KIFLTKSTRINGS[435]);
		MethodInitializationWindow(KIFLTKSTRINGS[436], &Kifwindow::MethodPassword, P_ONE, KIFLTKSTRINGS[437]);
		MethodInitializationWindow(KIFLTKSTRINGS[2], &Kifwindow::MethodInput, P_NONE | P_ONE, KIFLTKSTRINGS[438]);
		MethodInitializationWindow(KIFLTKSTRINGS[439], &Kifwindow::MethodMenu, P_FIVE, KIFLTKSTRINGS[440]);
		MethodInitializationWindow(KIFLTKSTRINGS[441], &Kifwindow::MethodResizable, P_ONE | P_NONE, KIFLTKSTRINGS[442]);
		MethodInitializationWindow(KIFLTKSTRINGS[443], &Kifwindow::MethodSizerange, P_FOUR, KIFLTKSTRINGS[444]);
		MethodInitializationWindow(KIFLTKSTRINGS[445], &Kifwindow::MethodModal, P_ONE | P_NONE, KIFLTKSTRINGS[446]);
		MethodInitializationWindow(KIFLTKSTRINGS[447], &Kifwindow::MethodBorder, P_ONE | P_NONE, KIFLTKSTRINGS[448]);
		MethodInitializationWindow(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);

		MethodInitializationWindow(KIFLTKSTRINGS[451], &Kifcomplexwidget::MethodDrawColor, P_ONE, KIFLTKSTRINGS[452]);
		MethodInitializationWindow(KIFLTKSTRINGS[453], &Kifcomplexwidget::MethodRectangle, P_FOUR | P_FIVE, KIFLTKSTRINGS[454]);
		MethodInitializationWindow(KIFLTKSTRINGS[455], &Kifcomplexwidget::MethodVertex, P_TWO, KIFLTKSTRINGS[456]);
		MethodInitializationWindow(KIFLTKSTRINGS[457], &Kifcomplexwidget::MethodRectangleFill, P_FOUR | P_FIVE, KIFLTKSTRINGS[458]);
		MethodInitializationWindow(KIFLTKSTRINGS[459], &Kifcomplexwidget::MethodLineShape, P_TWO, KIFLTKSTRINGS[460]);
		MethodInitializationWindow(KIFLTKSTRINGS[461], &Kifcomplexwidget::MethodPolygon, P_SIX | P_EIGHT, KIFLTKSTRINGS[462]);
		MethodInitializationWindow(KIFLTKSTRINGS[463], &Kifcomplexwidget::MethodLoop, P_SIX | P_EIGHT, KIFLTKSTRINGS[464]);
		MethodInitializationWindow(KIFLTKSTRINGS[465], &Kifcomplexwidget::MethodLine, P_FOUR | P_SIX, KIFLTKSTRINGS[466]);
		MethodInitializationWindow(KIFLTKSTRINGS[467], &Kifcomplexwidget::MethodLineRotation, P_FOUR | P_FIVE, KIFLTKSTRINGS[468]);

		MethodInitializationWindow(KIFLTKSTRINGS[469], &Kifcomplexwidget::MethodPushMatrix, P_NONE, KIFLTKSTRINGS[470]);
		MethodInitializationWindow(KIFLTKSTRINGS[471], &Kifcomplexwidget::MethodPopMatrix, P_NONE, KIFLTKSTRINGS[472]);
		MethodInitializationWindow(KIFLTKSTRINGS[473], &Kifcomplexwidget::MethodScale, P_TWO | P_ONE, KIFLTKSTRINGS[474]);
		MethodInitializationWindow(KIFLTKSTRINGS[475], &Kifcomplexwidget::MethodTranslate, P_TWO, KIFLTKSTRINGS[476]);
		MethodInitializationWindow(KIFLTKSTRINGS[477], &Kifcomplexwidget::MethodRotate, P_ONE, KIFLTKSTRINGS[478]);
		MethodInitializationWindow(KIFLTKSTRINGS[479], &Kifcomplexwidget::MethodMultmatrix, P_SIX, KIFLTKSTRINGS[480]);

		MethodInitializationWindow(KIFLTKSTRINGS[481], &Kifcomplexwidget::MethodTransform_x, P_TWO, KIFLTKSTRINGS[482]);
		MethodInitializationWindow(KIFLTKSTRINGS[483], &Kifcomplexwidget::MethodTransform_y, P_TWO, KIFLTKSTRINGS[484]);
		MethodInitializationWindow(KIFLTKSTRINGS[485], &Kifcomplexwidget::MethodTransform_dx, P_TWO, KIFLTKSTRINGS[486]);
		MethodInitializationWindow(KIFLTKSTRINGS[487], &Kifcomplexwidget::MethodTransform_dy, P_TWO, KIFLTKSTRINGS[488]);
		MethodInitializationWindow(KIFLTKSTRINGS[489], &Kifcomplexwidget::MethodTransform_x, P_TWO, KIFLTKSTRINGS[490]);

		MethodInitializationWindow(KIFLTKSTRINGS[491], &Kifcomplexwidget::MethodBeginPoints, P_NONE, KIFLTKSTRINGS[492]);
		MethodInitializationWindow(KIFLTKSTRINGS[493], &Kifcomplexwidget::MethodEndPoints, P_NONE, KIFLTKSTRINGS[494]);

		MethodInitializationWindow(KIFLTKSTRINGS[495], &Kifcomplexwidget::MethodBeginLine, P_NONE, KIFLTKSTRINGS[496]);
		MethodInitializationWindow(KIFLTKSTRINGS[497], &Kifcomplexwidget::MethodEndLine, P_NONE, KIFLTKSTRINGS[498]);

		MethodInitializationWindow(KIFLTKSTRINGS[499], &Kifcomplexwidget::MethodBeginLoop, P_NONE, KIFLTKSTRINGS[500]);
		MethodInitializationWindow(KIFLTKSTRINGS[501], &Kifcomplexwidget::MethodEndLoop, P_NONE, KIFLTKSTRINGS[502]);

		MethodInitializationWindow(KIFLTKSTRINGS[503], &Kifcomplexwidget::MethodBeginPolygon, P_NONE, KIFLTKSTRINGS[504]);
		MethodInitializationWindow(KIFLTKSTRINGS[505], &Kifcomplexwidget::MethodEndPolygon, P_NONE, KIFLTKSTRINGS[506]);

		MethodInitializationWindow(KIFLTKSTRINGS[507], &Kifcomplexwidget::MethodBeginComplexPolygon, P_NONE, KIFLTKSTRINGS[508]);
		MethodInitializationWindow(KIFLTKSTRINGS[509], &Kifcomplexwidget::MethodGap, P_NONE, KIFLTKSTRINGS[510]);
		MethodInitializationWindow(KIFLTKSTRINGS[511], &Kifcomplexwidget::MethodEndComplexPolygon, P_NONE, KIFLTKSTRINGS[512]);


		MethodInitializationWindow(KIFLTKSTRINGS[513], &Kifcomplexwidget::MethodArc, P_FIVE | P_SIX, KIFLTKSTRINGS[514]);
		MethodInitializationWindow(KIFLTKSTRINGS[515], &Kifcomplexwidget::MethodCurve, P_EIGHT, KIFLTKSTRINGS[516]);
		MethodInitializationWindow(KIFLTKSTRINGS[517], &Kifcomplexwidget::MethodPushClip, P_FOUR, KIFLTKSTRINGS[518]);
		MethodInitializationWindow(KIFLTKSTRINGS[519], &Kifcomplexwidget::MethodPopClip, P_NONE, KIFLTKSTRINGS[520]);

		MethodInitializationWindow(KIFLTKSTRINGS[521], &Kifcomplexwidget::MethodPie, P_SIX, KIFLTKSTRINGS[522]);
		MethodInitializationWindow(KIFLTKSTRINGS[523], &Kifcomplexwidget::MethodPoint, P_TWO, KIFLTKSTRINGS[524]);
		MethodInitializationWindow(KIFLTKSTRINGS[525], &Kifcomplexwidget::MethodCircle, P_THREE | P_FOUR, KIFLTKSTRINGS[526]);

		MethodInitializationWindow(KIFLTKSTRINGS[527], &Kifcomplexwidget::MethodTextMeasure, P_ONE, KIFLTKSTRINGS[528]);
		MethodInitializationWindow(KIFLTKSTRINGS[529], &Kifcomplexwidget::MethodDrawText, P_THREE, KIFLTKSTRINGS[530]);
		MethodInitializationWindow(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE | P_ONE, KIFLTKSTRINGS[532]);
		MethodInitializationWindow(KIFLTKSTRINGS[533], &Kifcomplexwidget::MethodRGBColor, P_ONE | P_THREE, KIFLTKSTRINGS[534]);

		MethodInitializationWindow(KIFLTKSTRINGS[535], &Kifcomplexwidget::MethodTextFont, P_TWO, KIFLTKSTRINGS[536]);
		MethodInitializationWindow(KIFLTKSTRINGS[537], &Kifwindow::MethodOnDragDrop, P_TWO, KIFLTKSTRINGS[538]);
		MethodInitializationWindow(KIFLTKSTRINGS[539], &Kifwindow::MethodOnMouse, P_THREE, KIFLTKSTRINGS[540]);
		MethodInitializationWindow(KIFLTKSTRINGS[541], &Kifwindow::MethodOnKey, P_THREE, KIFLTKSTRINGS[542]);
		MethodInitializationWindow(KIFLTKSTRINGS[543], &Kifwindow::MethodCursorStyle, P_THREE, KIFLTKSTRINGS[544]);
		MethodInitializationWindow(KIFLTKSTRINGS[545], &Kifwindow::MethodWindowHide, P_ONE, KIFLTKSTRINGS[546]);
		MethodInitializationWindow(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[548]);
		MethodInitializationWindow(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationWindow(KIFLTKSTRINGS[551], &Kifwindow::MethodFlush, P_NONE, KIFLTKSTRINGS[552]);
		MethodInitializationWindow(KIFLTKSTRINGS[553], &Kifwindow::MethodPosition, P_NONE | P_TWO, KIFLTKSTRINGS[554]);
		MethodInitializationWindow(KIFLTKSTRINGS[268], &Kifwindow::MethodSize, P_NONE | P_FOUR, KIFLTKSTRINGS[555]);
		MethodInitializationWindow(KIFLTKSTRINGS[556], &Kifwindow::MethodAwake, P_NONE, KIFLTKSTRINGS[557]);
		MethodInitializationWindow(KIFLTKSTRINGS[558], &Kifwindow::MethodLock, P_NONE, KIFLTKSTRINGS[559]);
		MethodInitializationWindow(KIFLTKSTRINGS[560], &Kifwindow::MethodUnlock, P_NONE, KIFLTKSTRINGS[561]);
		MethodInitializationWindow(KIFLTKSTRINGS[401], &Kifwindow::MethodImage, P_FIVE, KIFLTKSTRINGS[562]);
		MethodInitializationWindow(KIFLTKSTRINGS[402], &Kifwindow::MethodBitmap, P_FOUR | P_SIX, KIFLTKSTRINGS[563]);
		MethodInitializationWindow(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationWindow(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationWindow(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationWindow(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationWindow(KIFLTKSTRINGS[572], &Kifwindow::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationWindow(KIFLTKSTRINGS[574], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[575]);
		MethodInitializationWindow(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationWindow(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);
		MethodInitializationWindow(KIFLTKSTRINGS[580], &KifWidget::MethodCopy, P_ONE, KIFLTKSTRINGS[581]);
		MethodInitializationWindow(KIFLTKSTRINGS[582], &KifWidget::MethodPaste, P_NONE, KIFLTKSTRINGS[583]);
		MethodInitializationWindow(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[585]);

		MethodInitializationScroll(KIFLTKSTRINGS[407], &Kifscroll::MethodCreate, P_FIVE, KIFLTKSTRINGS[586]);
		MethodInitializationScroll(KIFLTKSTRINGS[587], &Kifscroll::MethodResize, P_FOUR, KIFLTKSTRINGS[588]);
		MethodInitializationScroll(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationScroll(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationScroll(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationScroll(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationScroll(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationScroll(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationScroll(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationScroll(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationScroll(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationScroll(KIFLTKSTRINGS[574], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[575]);
		MethodInitializationScroll(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationScroll(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationScroll(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationScroll(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);
		MethodInitializationScroll(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[585]);

		MethodInitializationCounter(KIFLTKSTRINGS[407], &Kifcounter::MethodCreate, P_FIVE, KIFLTKSTRINGS[593]);
		MethodInitializationCounter(KIFLTKSTRINGS[594], &Kifcounter::MethodBoundary, P_TWO, KIFLTKSTRINGS[595]);
		MethodInitializationCounter(KIFLTKSTRINGS[596], &Kifcounter::MethodType, P_ONE, KIFLTKSTRINGS[597]);
		MethodInitializationCounter(KIFLTKSTRINGS[598], &Kifcounter::MethodStep, P_ONE, KIFLTKSTRINGS[599]);
		MethodInitializationCounter(KIFLTKSTRINGS[600], &Kifcounter::MethodLargeStep, P_ONE, KIFLTKSTRINGS[601]);
		MethodInitializationCounter(KIFLTKSTRINGS[602], &Kifcounter::MethodSteps, P_TWO, KIFLTKSTRINGS[603]);
		MethodInitializationCounter(KIFLTKSTRINGS[604], &Kifcounter::MethodValue, P_ONE | P_NONE, KIFLTKSTRINGS[605]);
		MethodInitializationCounter(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationCounter(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationCounter(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationCounter(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationCounter(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationCounter(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationCounter(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationCounter(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationCounter(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationCounter(KIFLTKSTRINGS[574], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[575]);
		MethodInitializationCounter(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationCounter(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationCounter(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationCounter(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);
		MethodInitializationCounter(KIFLTKSTRINGS[527], &Kifcounter::MethodFontSize, P_ONE | P_NONE, KIFLTKSTRINGS[528]);
		MethodInitializationCounter(KIFLTKSTRINGS[535], &Kifcounter::MethodFont, P_ONE | P_NONE, KIFLTKSTRINGS[606]);
		MethodInitializationCounter(KIFLTKSTRINGS[607], &Kifcounter::MethodTextColor, P_ONE | P_NONE, KIFLTKSTRINGS[608]);

		MethodInitializationProgress(KIFLTKSTRINGS[407], &Kifprogress::MethodCreate, P_FIVE, KIFLTKSTRINGS[609]);
		MethodInitializationProgress(KIFLTKSTRINGS[610], &Kifprogress::MethodMinimum, P_ONE | P_NONE, KIFLTKSTRINGS[611]);
		MethodInitializationProgress(KIFLTKSTRINGS[612], &Kifprogress::MethodMaximum, P_ONE | P_NONE, KIFLTKSTRINGS[613]);
		MethodInitializationProgress(KIFLTKSTRINGS[604], &Kifprogress::MethodValue, P_ONE | P_NONE, KIFLTKSTRINGS[614]);
		MethodInitializationProgress(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationProgress(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationProgress(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationProgress(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationProgress(KIFLTKSTRINGS[615], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[616]);
		MethodInitializationProgress(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationProgress(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationProgress(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationProgress(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);

		MethodInitializationSlider(KIFLTKSTRINGS[407], &Kifslider::MethodCreate, P_SEVEN, KIFLTKSTRINGS[617]);
		MethodInitializationSlider(KIFLTKSTRINGS[587], &Kifslider::MethodResize, P_FOUR, KIFLTKSTRINGS[588]);
		MethodInitializationSlider(KIFLTKSTRINGS[594], &Kifslider::MethodBoundary, P_TWO, KIFLTKSTRINGS[618]);
		MethodInitializationSlider(KIFLTKSTRINGS[596], &Kifslider::MethodType, P_ONE, KIFLTKSTRINGS[619]);
		MethodInitializationSlider(KIFLTKSTRINGS[620], &Kifslider::MethodBoxType, P_ONE, KIFLTKSTRINGS[621]);
		MethodInitializationSlider(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[622]);
		MethodInitializationSlider(KIFLTKSTRINGS[598], &Kifslider::MethodStep, P_ONE, KIFLTKSTRINGS[623]);
		MethodInitializationSlider(KIFLTKSTRINGS[604], &Kifslider::MethodValue, P_ONE | P_NONE, KIFLTKSTRINGS[624]);
		MethodInitializationSlider(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationSlider(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationSlider(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationSlider(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationSlider(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationSlider(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationSlider(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationSlider(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationSlider(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationSlider(KIFLTKSTRINGS[574], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[575]);
		MethodInitializationSlider(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationSlider(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationSlider(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationSlider(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);

		MethodInitializationBox(KIFLTKSTRINGS[407], &Kifbox::MethodCreate, P_FIVE, KIFLTKSTRINGS[625]);
		MethodInitializationBox(KIFLTKSTRINGS[596], &Kifbox::MethodType, P_ONE, KIFLTKSTRINGS[626]);
		MethodInitializationBox(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationBox(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationBox(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationBox(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationBox(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationBox(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationBox(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationBox(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationBox(KIFLTKSTRINGS[574], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[575]);
		MethodInitializationBox(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationBox(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationBox(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationBox(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);
		MethodInitializationBox(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[585]);

		MethodInitializationButton(KIFLTKSTRINGS[407], &Kifbutton::MethodCreate, P_SEVEN | P_SIX | P_FIVE, KIFLTKSTRINGS[627]);
		MethodInitializationButton(KIFLTKSTRINGS[628], &Kifbutton::MethodWhen, 65534, KIFLTKSTRINGS[629]);
		MethodInitializationButton(KIFLTKSTRINGS[630], &Kifbutton::MethodShortcut, P_ONE, KIFLTKSTRINGS[631]);
		MethodInitializationButton(KIFLTKSTRINGS[632], &Kifbutton::MethodColor, P_ONE, KIFLTKSTRINGS[633]);
		MethodInitializationButton(KIFLTKSTRINGS[604], &Kifbutton::MethodValue, P_ONE | P_NONE, KIFLTKSTRINGS[634]);
		MethodInitializationButton(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[635]);
		MethodInitializationButton(KIFLTKSTRINGS[401], &Kifbutton::MethodImage, P_ONE | P_TWO | P_THREE, KIFLTKSTRINGS[636]);
		MethodInitializationButton(KIFLTKSTRINGS[402], &Kifbutton::MethodBitmap, P_TWO | P_THREE | P_FOUR, KIFLTKSTRINGS[637]);
		MethodInitializationButton(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationButton(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationButton(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationButton(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationButton(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationButton(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationButton(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationButton(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationButton(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationButton(KIFLTKSTRINGS[574], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[575]);
		MethodInitializationButton(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationButton(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationButton(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationButton(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);

		MethodInitializationtabs(KIFLTKSTRINGS[409], &Kiftabs::MethodBegin, P_FIVE, KIFLTKSTRINGS[638]);
		MethodInitializationtabs(KIFLTKSTRINGS[329], &Kiftabs::MethodEnd, P_NONE, KIFLTKSTRINGS[639]);
		MethodInitializationtabs(KIFLTKSTRINGS[640], &Kiftabs::MethodCurrent, P_NONE | P_ONE, KIFLTKSTRINGS[641]);
		MethodInitializationtabs(KIFLTKSTRINGS[642], &Kiftabs::MethodAdd, P_ONE, KIFLTKSTRINGS[643]);
		MethodInitializationtabs(KIFLTKSTRINGS[644], &Kiftabs::MethodRemove, P_ONE, KIFLTKSTRINGS[645]);
		MethodInitializationtabs(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationtabs(KIFLTKSTRINGS[574], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[575]);
		MethodInitializationtabs(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationtabs(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationtabs(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationtabs(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationtabs(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationtabs(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationtabs(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationtabs(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationtabs(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationtabs(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationtabs(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationtabs(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);
		MethodInitializationtabs(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[585]);

		MethodInitializationgroup(KIFLTKSTRINGS[409], &Kifgroup::MethodBegin, P_FIVE, KIFLTKSTRINGS[646]);
		MethodInitializationgroup(KIFLTKSTRINGS[329], &Kifgroup::MethodEnd, P_NONE, KIFLTKSTRINGS[647]);
		MethodInitializationgroup(KIFLTKSTRINGS[574], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[575]);
		MethodInitializationgroup(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationgroup(KIFLTKSTRINGS[648], &Kifgroup::MethodActivate, P_NONE, KIFLTKSTRINGS[649]);
		MethodInitializationgroup(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationgroup(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationgroup(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationgroup(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationgroup(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationgroup(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationgroup(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationgroup(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationgroup(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationgroup(KIFLTKSTRINGS[451], &Kifcomplexwidget::MethodDrawColor, P_ONE, KIFLTKSTRINGS[452]);
		MethodInitializationgroup(KIFLTKSTRINGS[453], &Kifcomplexwidget::MethodRectangle, P_FOUR | P_FIVE, KIFLTKSTRINGS[454]);
		MethodInitializationgroup(KIFLTKSTRINGS[457], &Kifcomplexwidget::MethodRectangleFill, P_FOUR | P_FIVE, KIFLTKSTRINGS[458]);
		MethodInitializationgroup(KIFLTKSTRINGS[459], &Kifcomplexwidget::MethodLineShape, P_TWO, KIFLTKSTRINGS[460]);
		MethodInitializationgroup(KIFLTKSTRINGS[461], &Kifcomplexwidget::MethodPolygon, P_SIX | P_EIGHT, KIFLTKSTRINGS[462]);
		MethodInitializationgroup(KIFLTKSTRINGS[465], &Kifcomplexwidget::MethodLine, P_FOUR | P_SIX, KIFLTKSTRINGS[466]);
		MethodInitializationgroup(KIFLTKSTRINGS[467], &Kifcomplexwidget::MethodLineRotation, P_FOUR | P_FIVE, KIFLTKSTRINGS[468]);

		MethodInitializationgroup(KIFLTKSTRINGS[469], &Kifcomplexwidget::MethodPushMatrix, P_NONE, KIFLTKSTRINGS[470]);
		MethodInitializationgroup(KIFLTKSTRINGS[471], &Kifcomplexwidget::MethodPopMatrix, P_NONE, KIFLTKSTRINGS[472]);
		MethodInitializationgroup(KIFLTKSTRINGS[473], &Kifcomplexwidget::MethodScale, P_TWO | P_ONE, KIFLTKSTRINGS[474]);
		MethodInitializationgroup(KIFLTKSTRINGS[475], &Kifcomplexwidget::MethodTranslate, P_TWO, KIFLTKSTRINGS[476]);
		MethodInitializationgroup(KIFLTKSTRINGS[477], &Kifcomplexwidget::MethodRotate, P_ONE, KIFLTKSTRINGS[478]);
		MethodInitializationgroup(KIFLTKSTRINGS[479], &Kifcomplexwidget::MethodMultmatrix, P_SIX, KIFLTKSTRINGS[480]);

		MethodInitializationgroup(KIFLTKSTRINGS[481], &Kifcomplexwidget::MethodTransform_x, P_TWO, KIFLTKSTRINGS[482]);
		MethodInitializationgroup(KIFLTKSTRINGS[483], &Kifcomplexwidget::MethodTransform_y, P_TWO, KIFLTKSTRINGS[484]);
		MethodInitializationgroup(KIFLTKSTRINGS[485], &Kifcomplexwidget::MethodTransform_dx, P_TWO, KIFLTKSTRINGS[486]);
		MethodInitializationgroup(KIFLTKSTRINGS[487], &Kifcomplexwidget::MethodTransform_dy, P_TWO, KIFLTKSTRINGS[488]);
		MethodInitializationgroup(KIFLTKSTRINGS[489], &Kifcomplexwidget::MethodTransform_x, P_TWO, KIFLTKSTRINGS[490]);

		MethodInitializationgroup(KIFLTKSTRINGS[491], &Kifcomplexwidget::MethodBeginPoints, P_NONE, KIFLTKSTRINGS[492]);
		MethodInitializationgroup(KIFLTKSTRINGS[493], &Kifcomplexwidget::MethodEndPoints, P_NONE, KIFLTKSTRINGS[494]);

		MethodInitializationgroup(KIFLTKSTRINGS[495], &Kifcomplexwidget::MethodBeginLine, P_NONE, KIFLTKSTRINGS[496]);
		MethodInitializationgroup(KIFLTKSTRINGS[497], &Kifcomplexwidget::MethodEndLine, P_NONE, KIFLTKSTRINGS[498]);

		MethodInitializationgroup(KIFLTKSTRINGS[499], &Kifcomplexwidget::MethodBeginLoop, P_NONE, KIFLTKSTRINGS[500]);
		MethodInitializationgroup(KIFLTKSTRINGS[501], &Kifcomplexwidget::MethodEndLoop, P_NONE, KIFLTKSTRINGS[502]);

		MethodInitializationgroup(KIFLTKSTRINGS[503], &Kifcomplexwidget::MethodBeginPolygon, P_NONE, KIFLTKSTRINGS[504]);
		MethodInitializationgroup(KIFLTKSTRINGS[505], &Kifcomplexwidget::MethodEndPolygon, P_NONE, KIFLTKSTRINGS[506]);

		MethodInitializationgroup(KIFLTKSTRINGS[507], &Kifcomplexwidget::MethodBeginComplexPolygon, P_NONE, KIFLTKSTRINGS[508]);
		MethodInitializationgroup(KIFLTKSTRINGS[509], &Kifcomplexwidget::MethodGap, P_NONE, KIFLTKSTRINGS[510]);
		MethodInitializationgroup(KIFLTKSTRINGS[511], &Kifcomplexwidget::MethodEndComplexPolygon, P_NONE, KIFLTKSTRINGS[512]);


		MethodInitializationgroup(KIFLTKSTRINGS[513], &Kifcomplexwidget::MethodArc, P_FIVE | P_SIX, KIFLTKSTRINGS[514]);
		MethodInitializationgroup(KIFLTKSTRINGS[515], &Kifcomplexwidget::MethodCurve, P_EIGHT, KIFLTKSTRINGS[516]);
		MethodInitializationgroup(KIFLTKSTRINGS[517], &Kifcomplexwidget::MethodPushClip, P_FOUR, KIFLTKSTRINGS[518]);
		MethodInitializationgroup(KIFLTKSTRINGS[519], &Kifcomplexwidget::MethodPopClip, P_NONE, KIFLTKSTRINGS[520]);


		MethodInitializationgroup(KIFLTKSTRINGS[521], &Kifcomplexwidget::MethodPie, P_SIX, KIFLTKSTRINGS[522]);
		MethodInitializationgroup(KIFLTKSTRINGS[523], &Kifcomplexwidget::MethodPoint, P_FOUR, KIFLTKSTRINGS[524]);
		MethodInitializationgroup(KIFLTKSTRINGS[525], &Kifcomplexwidget::MethodCircle, P_THREE | P_FOUR, KIFLTKSTRINGS[526]);
		MethodInitializationgroup(KIFLTKSTRINGS[527], &Kifcomplexwidget::MethodTextMeasure, P_ONE, KIFLTKSTRINGS[528]);
		MethodInitializationgroup(KIFLTKSTRINGS[529], &Kifcomplexwidget::MethodDrawText, P_THREE, KIFLTKSTRINGS[530]);
		MethodInitializationgroup(KIFLTKSTRINGS[531], &Kifcomplexwidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationgroup(KIFLTKSTRINGS[533], &Kifcomplexwidget::MethodRGBColor, P_ONE | P_THREE, KIFLTKSTRINGS[534]);
		MethodInitializationgroup(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationgroup(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationgroup(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);
		MethodInitializationgroup(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[585]);

#ifndef FLTKNOJPEG
		MethodInitializationImage(KIFLTKSTRINGS[650], &Kifimage::MethodLoadImageJPEG, P_ONE, KIFLTKSTRINGS[651]);
#endif
		MethodInitializationImage(KIFLTKSTRINGS[652], &Kifimage::MethodLoadImageGIF, P_ONE, KIFLTKSTRINGS[653]);

		MethodInitializationBitmap(KIFLTKSTRINGS[654], &Kifbitmap::MethodLoadbitmap, P_THREE, KIFLTKSTRINGS[655]);

		MethodInitializationTable(KIFLTKSTRINGS[407], &Kiftable::MethodCreate, P_FIVE, KIFLTKSTRINGS[656]);
		MethodInitializationTable(KIFLTKSTRINGS[657], &Kiftable::MethodClear, P_NONE, KIFLTKSTRINGS[658]);
		MethodInitializationTable(KIFLTKSTRINGS[642], &Kiftable::MethodAdd, P_THREE, KIFLTKSTRINGS[659]);
		MethodInitializationTable(KIFLTKSTRINGS[660], &Kiftable::MethodCell, P_TWO, KIFLTKSTRINGS[661]);
		MethodInitializationTable(KIFLTKSTRINGS[662], &Kiftable::MethodCellAlignment, P_ONE, KIFLTKSTRINGS[663]);
		MethodInitializationTable(KIFLTKSTRINGS[664], &Kiftable::MethodCellAlignmentHeaderCol, P_ONE, KIFLTKSTRINGS[665]);
		MethodInitializationTable(KIFLTKSTRINGS[666], &Kiftable::MethodCellAlignmentHeaderRow, P_ONE, KIFLTKSTRINGS[667]);
		MethodInitializationTable(KIFLTKSTRINGS[668], &Kiftable::MethodRow, P_ONE | P_NONE, KIFLTKSTRINGS[669]);
		MethodInitializationTable(KIFLTKSTRINGS[670], &Kiftable::MethodColumn, P_ONE | P_NONE, KIFLTKSTRINGS[671]);
		MethodInitializationTable(KIFLTKSTRINGS[672], &Kiftable::MethodRowHeight, P_ONE, KIFLTKSTRINGS[673]);
		MethodInitializationTable(KIFLTKSTRINGS[674], &Kiftable::MethodColumnWidth, P_ONE, KIFLTKSTRINGS[675]);
		MethodInitializationTable(KIFLTKSTRINGS[535], &Kiftable::MethodFont, P_ONE | P_NONE, KIFLTKSTRINGS[606]);
		MethodInitializationTable(KIFLTKSTRINGS[676], &Kiftable::MethodFontSize, P_ONE | P_NONE, KIFLTKSTRINGS[677]);
		MethodInitializationTable(KIFLTKSTRINGS[678], &Kiftable::MethodColorbg, P_ONE | P_NONE, KIFLTKSTRINGS[679]);
		MethodInitializationTable(KIFLTKSTRINGS[680], &Kiftable::MethodColorfg, P_ONE | P_NONE, KIFLTKSTRINGS[681]);
		MethodInitializationTable(KIFLTKSTRINGS[628], &Kiftable::MethodWhen, 65534, KIFLTKSTRINGS[682]);
		MethodInitializationTable(KIFLTKSTRINGS[574], &Kiftable::MethodTableSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[683]);
		MethodInitializationTable(KIFLTKSTRINGS[620], &Kiftable::MethodBoxType, P_ONE, KIFLTKSTRINGS[684]);
		MethodInitializationTable(KIFLTKSTRINGS[685], &Kiftable::MethodRowHeader, P_TWO, KIFLTKSTRINGS[686]);
		MethodInitializationTable(KIFLTKSTRINGS[687], &Kiftable::MethodColumnHeader, P_TWO, KIFLTKSTRINGS[688]);
		MethodInitializationTable(KIFLTKSTRINGS[689], &Kiftable::MethodRowHeaderHeight, P_ONE, KIFLTKSTRINGS[690]);
		MethodInitializationTable(KIFLTKSTRINGS[691], &Kiftable::MethodColumnHeaderWidth, P_ONE, KIFLTKSTRINGS[692]);
		MethodInitializationTable(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationTable(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationTable(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationTable(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationTable(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationTable(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationTable(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationTable(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationTable(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationTable(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationTable(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationTable(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationTable(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);
		MethodInitializationTable(KIFLTKSTRINGS[580], &KifWidget::MethodCopy, P_ONE, KIFLTKSTRINGS[581]);
		MethodInitializationTable(KIFLTKSTRINGS[582], &KifWidget::MethodPaste, P_NONE, KIFLTKSTRINGS[583]);
		MethodInitializationTable(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[585]);

		MethodInitializationInput(KIFLTKSTRINGS[407], &Kifinput::MethodCreate, P_SIX, KIFLTKSTRINGS[693]);
		MethodInitializationInput(KIFLTKSTRINGS[604], &Kifinput::MethodValue, P_ONE | P_NONE, KIFLTKSTRINGS[694]);
		MethodInitializationInput(KIFLTKSTRINGS[695], &Kifinput::MethodInsert, P_TWO, KIFLTKSTRINGS[696]);
		MethodInitializationInput(KIFLTKSTRINGS[697], &Kifinput::MethodSelection, P_NONE, KIFLTKSTRINGS[698]);
		MethodInitializationInput(KIFLTKSTRINGS[632], &Kifinput::MethodColor, P_ONE | P_NONE, KIFLTKSTRINGS[699]);
		MethodInitializationInput(KIFLTKSTRINGS[700], &Kifinput::MethodWord, P_ONE, KIFLTKSTRINGS[701]);
		MethodInitializationInput(KIFLTKSTRINGS[535], &Kifinput::MethodFont, P_ONE | P_NONE, KIFLTKSTRINGS[702]);
		MethodInitializationInput(KIFLTKSTRINGS[676], &Kifinput::MethodFontSize, P_ONE | P_NONE, KIFLTKSTRINGS[677]);
		MethodInitializationInput(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationInput(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationInput(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationInput(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationInput(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationInput(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationInput(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationInput(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationInput(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationInput(KIFLTKSTRINGS[574], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[575]);
		MethodInitializationInput(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationInput(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationInput(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationInput(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);
		MethodInitializationInput(KIFLTKSTRINGS[580], &KifWidget::MethodCopy, P_ONE, KIFLTKSTRINGS[581]);
		MethodInitializationInput(KIFLTKSTRINGS[582], &KifWidget::MethodPaste, P_NONE, KIFLTKSTRINGS[583]);
		MethodInitializationInput(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[585]);

		MethodInitializationEditor(KIFLTKSTRINGS[407], &Kifeditor::MethodCreate, P_FIVE, KIFLTKSTRINGS[703]);
		MethodInitializationEditor(KIFLTKSTRINGS[697], &Kifeditor::MethodSelection, P_NONE, KIFLTKSTRINGS[704]);
		MethodInitializationEditor(KIFLTKSTRINGS[604], &Kifeditor::MethodValue, P_ONE | P_NONE, KIFLTKSTRINGS[705]);
		MethodInitializationEditor(KIFLTKSTRINGS[632], &Kifeditor::MethodColor, P_ONE | P_NONE, KIFLTKSTRINGS[699]);
		MethodInitializationEditor(KIFLTKSTRINGS[535], &Kifeditor::MethodFont, P_ONE | P_NONE, KIFLTKSTRINGS[706]);
		MethodInitializationEditor(KIFLTKSTRINGS[676], &Kifeditor::MethodFontSize, P_ONE | P_NONE, KIFLTKSTRINGS[707]);
		MethodInitializationEditor(KIFLTKSTRINGS[708], &Kifeditor::MethodSetStyle, P_THREE, KIFLTKSTRINGS[709]);
		MethodInitializationEditor(KIFLTKSTRINGS[654], &Kifeditor::MethodLoad, P_ONE, KIFLTKSTRINGS[710]);
		MethodInitializationEditor(KIFLTKSTRINGS[711], &Kifeditor::MethodSave, P_ONE, KIFLTKSTRINGS[712]);
		MethodInitializationEditor(KIFLTKSTRINGS[713], &Kifeditor::MethodReplace, P_FOUR | P_THREE | P_TWO, KIFLTKSTRINGS[714]);
		MethodInitializationEditor(KIFLTKSTRINGS[715], &Kifeditor::MethodReplaceAll, P_TWO | P_THREE, KIFLTKSTRINGS[716]);
		MethodInitializationEditor(KIFLTKSTRINGS[717], &Kifeditor::MethodFind, P_ONE | P_TWO | P_THREE, KIFLTKSTRINGS[718]);
		MethodInitializationEditor(KIFLTKSTRINGS[719], &Kifeditor::MethodRFind, P_ONE | P_TWO | P_THREE, KIFLTKSTRINGS[720]);
		MethodInitializationEditor(KIFLTKSTRINGS[721], &Kifeditor::MethodHighlight, P_NONE | P_TWO, KIFLTKSTRINGS[722]);
		MethodInitializationEditor(KIFLTKSTRINGS[723], &Kifeditor::MethodUnHighlight, P_NONE, KIFLTKSTRINGS[724]);
		MethodInitializationEditor(KIFLTKSTRINGS[543], &Kifeditor::MethodCursorStyle, P_ONE, KIFLTKSTRINGS[725]);
		MethodInitializationEditor(KIFLTKSTRINGS[539], &Kifeditor::MethodOnMouseClick, P_THREE, KIFLTKSTRINGS[726]);
		MethodInitializationEditor(KIFLTKSTRINGS[541], &Kifeditor::MethodOnKey, P_THREE, KIFLTKSTRINGS[727]);
		MethodInitializationEditor(KIFLTKSTRINGS[728], &Kifeditor::MethodOnVScroll, P_TWO, KIFLTKSTRINGS[729]);
		MethodInitializationEditor(KIFLTKSTRINGS[730], &Kifeditor::MethodOnHScroll, P_TWO, KIFLTKSTRINGS[731]);
		MethodInitializationEditor(KIFLTKSTRINGS[732], &Kifeditor::MethodGetStyle, P_TWO, KIFLTKSTRINGS[733]);
		MethodInitializationEditor(KIFLTKSTRINGS[734], &Kifeditor::MethodAddStyles, P_ONE, KIFLTKSTRINGS[735]);

		MethodInitializationEditor(KIFLTKSTRINGS[736], &Kifeditor::MethodDelete, P_NONE, KIFLTKSTRINGS[737]);
		MethodInitializationEditor(KIFLTKSTRINGS[738], &Kifeditor::MethodCut, P_NONE, KIFLTKSTRINGS[739]);
		MethodInitializationEditor(KIFLTKSTRINGS[580], &Kifeditor::MethodCopy, P_NONE | P_ONE, KIFLTKSTRINGS[740]);
		MethodInitializationEditor(KIFLTKSTRINGS[582], &Kifeditor::MethodPaste, P_NONE, KIFLTKSTRINGS[741]);
		MethodInitializationEditor(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[585]);

		MethodInitializationEditor(KIFLTKSTRINGS[742], &Kifeditor::MethodSelect, P_TWO | P_NONE, KIFLTKSTRINGS[743]);
		MethodInitializationEditor(KIFLTKSTRINGS[465], &Kifeditor::MethodLine, P_ONE | P_NONE, KIFLTKSTRINGS[744]);
		MethodInitializationEditor(KIFLTKSTRINGS[745], &Kifeditor::MethodLineBoundaries, P_ONE | P_NONE, KIFLTKSTRINGS[746]);
		MethodInitializationEditor(KIFLTKSTRINGS[747], &Kifeditor::MethodLineBoundariesChar, P_ONE | P_NONE, KIFLTKSTRINGS[748]);
		MethodInitializationEditor(KIFLTKSTRINGS[330], &Kifeditor::MethodCursor, P_ONE | P_NONE, KIFLTKSTRINGS[749]);
		MethodInitializationEditor(KIFLTKSTRINGS[750], &Kifeditor::MethodCursorChar, P_NONE, KIFLTKSTRINGS[751]);
		MethodInitializationEditor(KIFLTKSTRINGS[752], &Kifeditor::MethodGotoLine, P_TWO, KIFLTKSTRINGS[753]);
		MethodInitializationEditor(KIFLTKSTRINGS[754], &Kifeditor::MethodAnnotate, P_TWO | P_THREE, KIFLTKSTRINGS[755]);
		MethodInitializationEditor(KIFLTKSTRINGS[700], &Kifeditor::MethodWord, P_ONE, KIFLTKSTRINGS[701]);
		MethodInitializationEditor(KIFLTKSTRINGS[756], &Kifeditor::MethodWrap, P_ONE, KIFLTKSTRINGS[757]);
		MethodInitializationEditor(KIFLTKSTRINGS[758], &Kifeditor::MethodAnnotateXIPRgx, P_TWO, KIFLTKSTRINGS[759]);
		MethodInitializationEditor(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationEditor(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationEditor(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationEditor(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationEditor(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationEditor(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationEditor(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationEditor(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationEditor(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationEditor(KIFLTKSTRINGS[574], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[575]);
		MethodInitializationEditor(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationEditor(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationEditor(KIFLTKSTRINGS[527], &Kifeditor::MethodTextMeasure, P_ONE, KIFLTKSTRINGS[528]);
		MethodInitializationEditor(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationEditor(KIFLTKSTRINGS[695], &Kifeditor::MethodInsert, P_TWO, KIFLTKSTRINGS[696]);
		MethodInitializationEditor(KIFLTKSTRINGS[760], &Kifeditor::MethodAppend, P_ONE, KIFLTKSTRINGS[761]);
		MethodInitializationEditor(KIFLTKSTRINGS[762], &Kifeditor::MethodBytePosition, P_ONE, KIFLTKSTRINGS[763]);
		MethodInitializationEditor(KIFLTKSTRINGS[764], &Kifeditor::MethodCharacterPosition, P_ONE, KIFLTKSTRINGS[765]);
		MethodInitializationEditor(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);

		MethodInitializationOutput(KIFLTKSTRINGS[407], &Kifoutput::MethodCreate, P_SIX, KIFLTKSTRINGS[766]);
		MethodInitializationOutput(KIFLTKSTRINGS[604], &Kifoutput::MethodValue, P_NONE | P_ONE, KIFLTKSTRINGS[767]);
		MethodInitializationOutput(KIFLTKSTRINGS[756], &Kifoutput::MethodWrap, P_ONE, KIFLTKSTRINGS[757]);
		MethodInitializationOutput(KIFLTKSTRINGS[632], &Kifoutput::MethodColor, P_ONE | P_NONE, KIFLTKSTRINGS[699]);
		MethodInitializationOutput(KIFLTKSTRINGS[535], &Kifoutput::MethodFont, P_ONE | P_NONE, KIFLTKSTRINGS[702]);
		MethodInitializationOutput(KIFLTKSTRINGS[676], &Kifoutput::MethodFontSize, P_ONE | P_NONE, KIFLTKSTRINGS[677]);
		MethodInitializationOutput(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationOutput(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationOutput(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationOutput(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationOutput(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationOutput(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationOutput(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationOutput(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationOutput(KIFLTKSTRINGS[574], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[575]);
		MethodInitializationOutput(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationOutput(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationOutput(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationOutput(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);
		MethodInitializationOutput(KIFLTKSTRINGS[580], &KifWidget::MethodCopy, P_ONE, KIFLTKSTRINGS[581]);
		MethodInitializationOutput(KIFLTKSTRINGS[582], &KifWidget::MethodPaste, P_NONE, KIFLTKSTRINGS[583]);
		MethodInitializationOutput(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[585]);

		MethodInitializationChoice(KIFLTKSTRINGS[407], &Kifchoice::MethodCreate, P_FIVE, KIFLTKSTRINGS[768]);
		MethodInitializationChoice(KIFLTKSTRINGS[604], &Kifchoice::MethodValue, P_ONE | P_NONE, KIFLTKSTRINGS[769]);
		MethodInitializationChoice(KIFLTKSTRINGS[535], &Kifchoice::MethodFont, P_ONE | P_NONE, KIFLTKSTRINGS[702]);
		MethodInitializationChoice(KIFLTKSTRINGS[439], &Kifchoice::MethodMenu, P_ONE, KIFLTKSTRINGS[770]);
		MethodInitializationChoice(KIFLTKSTRINGS[676], &Kifchoice::MethodFontSize, P_ONE | P_NONE, KIFLTKSTRINGS[677]);
		MethodInitializationChoice(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationChoice(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationChoice(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationChoice(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationChoice(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationChoice(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationChoice(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationChoice(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationChoice(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationChoice(KIFLTKSTRINGS[574], &KifWidget::MethodSelectionColor, P_ONE | P_NONE, KIFLTKSTRINGS[575]);
		MethodInitializationChoice(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationChoice(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationChoice(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationChoice(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);
		MethodInitializationChoice(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[585]);

		MethodInitializationFilebrowser(KIFLTKSTRINGS[407], &Kiffilebrowser::MethodCreate, P_FOUR, KIFLTKSTRINGS[771]);
		MethodInitializationFilebrowser(KIFLTKSTRINGS[604], &Kiffilebrowser::MethodValue, P_NONE, KIFLTKSTRINGS[772]);
		MethodInitializationFilebrowser(KIFLTKSTRINGS[414], &Kiffilebrowser::MethodClose, P_NONE, KIFLTKSTRINGS[773]);
		MethodInitializationFilebrowser(KIFLTKSTRINGS[774], &Kiffilebrowser::MethodOk, P_NONE, KIFLTKSTRINGS[775]);

#ifdef MACUI
		MethodInitializationFilebrowser(KIFLTKSTRINGS[776], &Kiffilebrowser::MethodOuvre, P_NONE, KIFLTKSTRINGS[777]);
		MethodInitializationFilebrowser(KIFLTKSTRINGS[654], &Kiffilebrowser::MethodCharge, P_ONE, KIFLTKSTRINGS[778]);
#endif

		MethodInitializationBrowser(KIFLTKSTRINGS[407], &Kifbrowser::MethodCreate, P_FIVE, KIFLTKSTRINGS[779]);
		MethodInitializationBrowser(KIFLTKSTRINGS[642], &Kifbrowser::MethodAdd, P_ONE, KIFLTKSTRINGS[780]);
		MethodInitializationBrowser(KIFLTKSTRINGS[654], &Kifbrowser::MethodLoad, P_ONE, KIFLTKSTRINGS[781]);
		MethodInitializationBrowser(KIFLTKSTRINGS[695], &Kifbrowser::MethodInsert, P_TWO, KIFLTKSTRINGS[782]);
		MethodInitializationBrowser(KIFLTKSTRINGS[657], &Kifbrowser::MethodClear, P_NONE, KIFLTKSTRINGS[783]);
		MethodInitializationBrowser(KIFLTKSTRINGS[604], &Kifbrowser::MethodValue, P_NONE, KIFLTKSTRINGS[784]);
		MethodInitializationBrowser(KIFLTKSTRINGS[742], &Kifbrowser::MethodSelect, P_ONE | P_NONE, KIFLTKSTRINGS[785]);
		MethodInitializationBrowser(KIFLTKSTRINGS[786], &Kifbrowser::MethodDeselect, P_NONE | P_ONE, KIFLTKSTRINGS[787]);
		MethodInitializationBrowser(KIFLTKSTRINGS[788], &Kifbrowser::MethodColumnchar, P_ONE | P_NONE, KIFLTKSTRINGS[789]);
		MethodInitializationBrowser(KIFLTKSTRINGS[790], &Kifbrowser::MethodFormatchar, P_ONE | P_NONE, KIFLTKSTRINGS[791]);
		MethodInitializationBrowser(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationBrowser(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationBrowser(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationBrowser(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationBrowser(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationBrowser(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationBrowser(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationBrowser(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationBrowser(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationBrowser(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationBrowser(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationBrowser(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationBrowser(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationBrowser(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);
		MethodInitializationBrowser(KIFLTKSTRINGS[580], &KifWidget::MethodCopy, P_ONE, KIFLTKSTRINGS[581]);
		MethodInitializationBrowser(KIFLTKSTRINGS[582], &KifWidget::MethodPaste, P_NONE, KIFLTKSTRINGS[583]);
		MethodInitializationBrowser(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[585]);

		MethodInitializationETree(KIFLTKSTRINGS[572], &Kifetree::MethodLabel, P_NONE, KIFLTKSTRINGS[792]);
		MethodInitializationETree(KIFLTKSTRINGS[793], &Kifetree::MethodLabel, P_NONE, KIFLTKSTRINGS[794]);
		MethodInitializationETree(KIFLTKSTRINGS[604], &Kifetree::MethodValue, P_ONE | P_NONE, KIFLTKSTRINGS[795]);
		MethodInitializationETree(KIFLTKSTRINGS[796], &Kifetree::MethodClean, P_NONE, KIFLTKSTRINGS[797]);
		MethodInitializationETree(KIFLTKSTRINGS[798], &Kifetree::MethodItemBGColor, P_NONE | P_ONE, KIFLTKSTRINGS[799]);
		MethodInitializationETree(KIFLTKSTRINGS[800], &Kifetree::MethodItemFGColor, P_NONE | P_ONE, KIFLTKSTRINGS[801]);
		MethodInitializationETree(KIFLTKSTRINGS[535], &Kifetree::MethodItemFont, P_NONE | P_ONE, KIFLTKSTRINGS[802]);
		MethodInitializationETree(KIFLTKSTRINGS[676], &Kifetree::MethodItemSize, P_NONE | P_ONE, KIFLTKSTRINGS[803]);
		MethodInitializationETree(KIFLTKSTRINGS[804], &Kifetree::MethodIsclose, P_NONE, KIFLTKSTRINGS[805]);
		MethodInitializationETree(KIFLTKSTRINGS[806], &Kifetree::MethodIsopen, P_NONE, KIFLTKSTRINGS[807]);
		MethodInitializationETree(KIFLTKSTRINGS[808], &Kifetree::MethodIsroot, P_NONE, KIFLTKSTRINGS[809]);
		MethodInitializationETree(KIFLTKSTRINGS[810], &Kifetree::MethodIsselected, P_NONE, KIFLTKSTRINGS[811]);
		MethodInitializationETree(KIFLTKSTRINGS[812], &Kifetree::MethodIsactive, P_NONE, KIFLTKSTRINGS[813]);
		MethodInitializationETree(KIFLTKSTRINGS[814], &Kifetree::MethodParent, P_NONE, KIFLTKSTRINGS[815]);
		MethodInitializationETree(KIFLTKSTRINGS[816], &Kifetree::MethodNext, P_NONE, KIFLTKSTRINGS[817]);
		MethodInitializationETree(KIFLTKSTRINGS[818], &Kifetree::MethodPrevious, P_NONE, KIFLTKSTRINGS[819]);
		MethodInitializationETree(KIFLTKSTRINGS[820], &Kifetree::MethodChild, P_ONE, KIFLTKSTRINGS[821]);
		MethodInitializationETree(KIFLTKSTRINGS[822], &Kifetree::MethodChildren, P_NONE, KIFLTKSTRINGS[823]);
		MethodInitializationETree(KIFLTKSTRINGS[648], &Kifetree::MethodActivate, P_NONE, KIFLTKSTRINGS[824]);
		MethodInitializationETree(KIFLTKSTRINGS[825], &Kifetree::MethodDeactivate, P_NONE, KIFLTKSTRINGS[826]);

		MethodInitializationWTree(KIFLTKSTRINGS[407], &Kifwtree::MethodCreate, P_FIVE, KIFLTKSTRINGS[827]);
		MethodInitializationWTree(KIFLTKSTRINGS[657], &Kifwtree::MethodClear, P_NONE, KIFLTKSTRINGS[828]);
		MethodInitializationWTree(KIFLTKSTRINGS[642], &Kifwtree::MethodAdd, P_ONE | P_TWO, KIFLTKSTRINGS[829]);
		MethodInitializationWTree(KIFLTKSTRINGS[804], &Kifwtree::MethodIsclose, P_ONE, KIFLTKSTRINGS[830]);
		MethodInitializationWTree(KIFLTKSTRINGS[414], &Kifwtree::MethodClose, P_ONE, KIFLTKSTRINGS[831]);
		MethodInitializationWTree(KIFLTKSTRINGS[776], &Kifwtree::MethodOpen, P_ONE, KIFLTKSTRINGS[832]);
		MethodInitializationWTree(KIFLTKSTRINGS[695], &Kifwtree::MethodInsert, P_ONE, KIFLTKSTRINGS[833]);
		MethodInitializationWTree(KIFLTKSTRINGS[644], &Kifwtree::MethodRemove, P_ONE, KIFLTKSTRINGS[834]);
		MethodInitializationWTree(KIFLTKSTRINGS[835], &Kifwtree::MethodInsertAbove, P_TWO, KIFLTKSTRINGS[836]);
		MethodInitializationWTree(KIFLTKSTRINGS[837], &Kifwtree::MethodConnectorColor, P_NONE | P_ONE, KIFLTKSTRINGS[838]);
		MethodInitializationWTree(KIFLTKSTRINGS[839], &Kifwtree::MethodConnectorStyle, P_NONE | P_ONE, KIFLTKSTRINGS[840]);
		MethodInitializationWTree(KIFLTKSTRINGS[841], &Kifwtree::MethodConnectorWidth, P_NONE | P_ONE, KIFLTKSTRINGS[842]);
		MethodInitializationWTree(KIFLTKSTRINGS[843], &Kifwtree::MethodSortOrder, P_NONE | P_ONE, KIFLTKSTRINGS[844]);
		MethodInitializationWTree(KIFLTKSTRINGS[845], &Kifwtree::MethodSelectmode, P_NONE | P_ONE, KIFLTKSTRINGS[846]);

		MethodInitializationWTree(KIFLTKSTRINGS[847], &Kifwtree::MethodItemBGColor, P_NONE | P_ONE, KIFLTKSTRINGS[848]);
		MethodInitializationWTree(KIFLTKSTRINGS[849], &Kifwtree::MethodItemFGColor, P_NONE | P_ONE, KIFLTKSTRINGS[850]);
		MethodInitializationWTree(KIFLTKSTRINGS[851], &Kifwtree::MethodItemFont, P_NONE | P_ONE, KIFLTKSTRINGS[852]);
		MethodInitializationWTree(KIFLTKSTRINGS[853], &Kifwtree::MethodItemSize, P_NONE | P_ONE, KIFLTKSTRINGS[854]);

		MethodInitializationWTree(KIFLTKSTRINGS[855], &Kifwtree::MethodMarginLeft, P_NONE | P_ONE, KIFLTKSTRINGS[856]);
		MethodInitializationWTree(KIFLTKSTRINGS[857], &Kifwtree::MethodMarginTop, P_NONE | P_ONE, KIFLTKSTRINGS[858]);

		MethodInitializationWTree(KIFLTKSTRINGS[859], &Kifwtree::MethodRoot, P_NONE, KIFLTKSTRINGS[860]);
		MethodInitializationWTree(KIFLTKSTRINGS[861], &Kifwtree::MethodRootLabel, P_ONE, KIFLTKSTRINGS[862]);
		MethodInitializationWTree(KIFLTKSTRINGS[863], &Kifwtree::MethodClicked, P_NONE, KIFLTKSTRINGS[864]);
		MethodInitializationWTree(KIFLTKSTRINGS[865], &Kifwtree::MethodFirst, P_NONE, KIFLTKSTRINGS[866]);
		MethodInitializationWTree(KIFLTKSTRINGS[717], &Kifwtree::MethodFind, P_ONE, KIFLTKSTRINGS[867]);
		MethodInitializationWTree(KIFLTKSTRINGS[868], &Kifwtree::MethodLast, P_NONE, KIFLTKSTRINGS[869]);
		MethodInitializationWTree(KIFLTKSTRINGS[816], &Kifwtree::MethodNext, P_NONE | P_ONE, KIFLTKSTRINGS[870]);
		MethodInitializationWTree(KIFLTKSTRINGS[818], &Kifwtree::MethodPrevious, P_NONE | P_ONE, KIFLTKSTRINGS[871]);

		MethodInitializationWTree(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationWTree(KIFLTKSTRINGS[591], &KifWidget::MethodTooltip, P_ONE, KIFLTKSTRINGS[592]);
		MethodInitializationWTree(KIFLTKSTRINGS[547], &KifWidget::MethodShow, P_NONE, KIFLTKSTRINGS[589]);
		MethodInitializationWTree(KIFLTKSTRINGS[549], &KifWidget::MethodFocus, P_NONE, KIFLTKSTRINGS[550]);
		MethodInitializationWTree(KIFLTKSTRINGS[545], &KifWidget::MethodHide, P_NONE, KIFLTKSTRINGS[590]);
		MethodInitializationWTree(KIFLTKSTRINGS[564], &KifWidget::MethodLabelSize, P_ONE | P_NONE, KIFLTKSTRINGS[565]);
		MethodInitializationWTree(KIFLTKSTRINGS[566], &KifWidget::MethodLabelFont, P_ONE | P_NONE, KIFLTKSTRINGS[567]);
		MethodInitializationWTree(KIFLTKSTRINGS[568], &KifWidget::MethodLabelColor, P_ONE | P_NONE, KIFLTKSTRINGS[569]);
		MethodInitializationWTree(KIFLTKSTRINGS[570], &KifWidget::MethodLabelType, P_ONE | P_NONE, KIFLTKSTRINGS[571]);
		MethodInitializationWTree(KIFLTKSTRINGS[572], &KifWidget::MethodLabel, P_ONE | P_NONE, KIFLTKSTRINGS[573]);
		MethodInitializationWTree(KIFLTKSTRINGS[576], &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, KIFLTKSTRINGS[577]);
		MethodInitializationWTree(KIFLTKSTRINGS[531], &KifWidget::MethodRedraw, P_NONE, KIFLTKSTRINGS[532]);
		MethodInitializationWTree(KIFLTKSTRINGS[449], &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, KIFLTKSTRINGS[450]);
		MethodInitializationWTree(KIFLTKSTRINGS[578], &KifWidget::MethodCreated, P_NONE, KIFLTKSTRINGS[579]);
		MethodInitializationWTree(KIFLTKSTRINGS[580], &KifWidget::MethodCopy, P_ONE, KIFLTKSTRINGS[581]);
		MethodInitializationWTree(KIFLTKSTRINGS[582], &KifWidget::MethodPaste, P_NONE, KIFLTKSTRINGS[583]);
		MethodInitializationWTree(KIFLTKSTRINGS[584], &KifWidget::MethodAlign, P_ONE, KIFLTKSTRINGS[585]);

		kifGlobals->kifAllFunctions[filebrowser_type] = Kiffilebrowser::methodassigned;
		kifGlobals->kifAllInformation[filebrowser_type] = Kiffilebrowser::infomethods;
		kifGlobals->kifAllFunctions[browser_type] = Kifbrowser::methodassigned;
		kifGlobals->kifAllInformation[browser_type] = Kifbrowser::infomethods;
		kifGlobals->kifAllFunctions[bitmap_type] = Kifbitmap::methodassigned;
		kifGlobals->kifAllInformation[bitmap_type] = Kifbitmap::infomethods;
		kifGlobals->kifAllFunctions[slider_type] = Kifslider::methodassigned;
		kifGlobals->kifAllInformation[slider_type] = Kifslider::infomethods;
		kifGlobals->kifAllFunctions[scroll_type] = Kifscroll::methodassigned;
		kifGlobals->kifAllInformation[scroll_type] = Kifscroll::infomethods;
		kifGlobals->kifAllFunctions[output_type] = Kifoutput::methodassigned;
		kifGlobals->kifAllInformation[output_type] = Kifoutput::infomethods;
		kifGlobals->kifAllFunctions[editor_type] = Kifeditor::methodassigned;
		kifGlobals->kifAllInformation[editor_type] = Kifeditor::infomethods;
		kifGlobals->kifAllFunctions[button_type] = Kifbutton::methodassigned;
		kifGlobals->kifAllInformation[button_type] = Kifbutton::infomethods;
		kifGlobals->kifAllFunctions[choice_type] = Kifchoice::methodassigned;
		kifGlobals->kifAllInformation[choice_type] = Kifchoice::infomethods;
		kifGlobals->kifAllFunctions[window_type] = Kifwindow::methodassigned;
		kifGlobals->kifAllInformation[window_type] = Kifwindow::infomethods;
		kifGlobals->kifAllFunctions[wtree_type] = Kifwtree::methodassigned;
		kifGlobals->kifAllInformation[wtree_type] = Kifwtree::infomethods;
		kifGlobals->kifAllFunctions[etree_type] = Kifetree::methodassigned;
		kifGlobals->kifAllInformation[etree_type] = Kifetree::infomethods;
		kifGlobals->kifAllFunctions[image_type] = Kifimage::methodassigned;
		kifGlobals->kifAllInformation[image_type] = Kifimage::infomethods;
		kifGlobals->kifAllFunctions[table_type] = Kiftable::methodassigned;
		kifGlobals->kifAllInformation[table_type] = Kiftable::infomethods;
		kifGlobals->kifAllFunctions[input_type] = Kifinput::methodassigned;
		kifGlobals->kifAllInformation[input_type] = Kifinput::infomethods;
		kifGlobals->kifAllFunctions[group_type] = Kifgroup::methodassigned;
		kifGlobals->kifAllInformation[group_type] = Kifgroup::infomethods;
		kifGlobals->kifAllFunctions[tabs_type] = Kiftabs::methodassigned;
		kifGlobals->kifAllInformation[tabs_type] = Kiftabs::infomethods;
		kifGlobals->kifAllFunctions[box_type] = Kifbox::methodassigned;
		kifGlobals->kifAllInformation[box_type] = Kifbox::infomethods;
		kifGlobals->kifAllFunctions[counter_type] = Kifcounter::methodassigned;
		kifGlobals->kifAllInformation[counter_type] = Kifcounter::infomethods;
		kifGlobals->kifAllFunctions[progress_type] = Kifprogress::methodassigned;
		kifGlobals->kifAllInformation[progress_type] = Kifprogress::infomethods;

		create_variables(kifcode);
		return true;
	}

	Exporting bool kifltk_Terminate(KifCode* kifcode) {
		windowmode = false;
		stopall = true;
		Kifimage::kifexportedmethods.clear();
		Kifimage::linkedmethods.clear();
		Kifimage::methodassigned.clear();
		Kifimage::infomethods.clear();
		Kifwindow::kifexportedmethods.clear();
		Kifwindow::linkedmethods.clear();
		Kifwindow::methodassigned.clear();
		Kifwindow::infomethods.clear();
		Kifinput::kifexportedmethods.clear();
		Kifinput::linkedmethods.clear();
		Kifinput::methodassigned.clear();
		Kifinput::infomethods.clear();
		Kifeditor::kifexportedmethods.clear();
		Kifeditor::linkedmethods.clear();
		Kifeditor::methodassigned.clear();
		Kifeditor::infomethods.clear();
		Kifoutput::kifexportedmethods.clear();
		Kifoutput::linkedmethods.clear();
		Kifoutput::methodassigned.clear();
		Kifoutput::infomethods.clear();
		Kifbutton::kifexportedmethods.clear();
		Kifbutton::linkedmethods.clear();
		Kifbutton::methodassigned.clear();
		Kifbutton::infomethods.clear();
		Kifbox::kifexportedmethods.clear();
		Kifbox::linkedmethods.clear();
		Kifbox::methodassigned.clear();
		Kifbox::infomethods.clear();
		Kifgroup::kifexportedmethods.clear();
		Kifgroup::linkedmethods.clear();
		Kifgroup::methodassigned.clear();
		Kifgroup::infomethods.clear();
		Kiftabs::kifexportedmethods.clear();
		Kiftabs::linkedmethods.clear();
		Kiftabs::methodassigned.clear();
		Kiftabs::infomethods.clear();
		Kiftable::kifexportedmethods.clear();
		Kiftable::linkedmethods.clear();
		Kiftable::methodassigned.clear();
		Kiftable::infomethods.clear();
		Kifscroll::kifexportedmethods.clear();
		Kifscroll::linkedmethods.clear();
		Kifscroll::methodassigned.clear();
		Kifscroll::infomethods.clear();
		Kifslider::kifexportedmethods.clear();
		Kifslider::linkedmethods.clear();
		Kifslider::methodassigned.clear();
		Kifslider::infomethods.clear();
		Kifchoice::kifexportedmethods.clear();
		Kifchoice::linkedmethods.clear();
		Kifchoice::methodassigned.clear();
		Kifchoice::infomethods.clear();
		Kifbrowser::kifexportedmethods.clear();
		Kifbrowser::linkedmethods.clear();
		Kifbrowser::methodassigned.clear();
		Kifbrowser::infomethods.clear();
		Kifetree::kifexportedmethods.clear();
		Kifetree::linkedmethods.clear();
		Kifetree::methodassigned.clear();
		Kifetree::infomethods.clear();
		Kifwtree::kifexportedmethods.clear();
		Kifwtree::linkedmethods.clear();
		Kifwtree::methodassigned.clear();
		Kifwtree::infomethods.clear();
		Kiffilebrowser::kifexportedmethods.clear();
		Kiffilebrowser::linkedmethods.clear();
		Kiffilebrowser::methodassigned.clear();
		Kiffilebrowser::infomethods.clear();
		Kifcounter::kifexportedmethods.clear();
		Kifcounter::linkedmethods.clear();
		Kifcounter::methodassigned.clear();
		Kifcounter::infomethods.clear();
		Kifprogress::methodassigned.clear();
		Kifprogress::infomethods.clear();
		return true;
	}

	Exporting bool KifWindowMode() {
		return windowmode;
	}

	Exporting void kiffltk_sessionInit() {
		stopall = false;
		windowmode = false;
		mainwindow = NULL;
	}

	void FLTKRedraw() {
		if (mainwindow != NULL)
			mainwindow->Terminate();
	}
}




