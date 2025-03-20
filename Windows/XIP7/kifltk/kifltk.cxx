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
else if (stopall) return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available"); \
if (_thread) { PostWindowCall(_thread, this, c, d, cl); return kifNULL; }

#define inthread(c,d,cl) int idcallthread=(calledthread==0)?_thread:calledthread;\
if (stopall) return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available"); \
if (_thread) { PostWindowCall(_thread, this, c, d, cl); return kifNULL; }

#define minthread int idcallthread=(calledthread==0)?_thread:calledthread;\
if (stopall) return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");
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
			return callfunc->kifcode->Returnerror("KIF(93): Wrong number of parameters in GET");

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
		txt = "get";
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
	int idname = KifNewId("get");
	if (kifGlobals->kifBaseFunctions.find(idname) != kifGlobals->kifBaseFunctions.end())
		delete kifGlobals->kifBaseFunctions[idname];

	kifGlobals->kifBaseFunctions[idname] = new KifPredefined(NULL, P_ONE | P_TWO, &PreGetEditor, idname);
	kifGlobals->kifBaseFunctions[idname]->kifcode = local;
}
//----------------------------------------------------------------------------------------------------------


static void initialisationtype() {
	static bool init = false;
	if (init == false) {
		colors["FOREGROUND_COLOR"] = FL_FOREGROUND_COLOR;
		colors["BACKGROUND2_COLOR"] = FL_BACKGROUND2_COLOR;
		colors["BACKGROUND_COLOR"] = FL_BACKGROUND_COLOR;
		colors["INACTIVE_COLOR"] = FL_INACTIVE_COLOR;
		colors["SELECTION_COLOR"] = FL_SELECTION_COLOR;
		colors["GRAY0"] = FL_GRAY0;
		colors["DARK3"] = FL_DARK3;
		colors["DARK2"] = FL_DARK2;
		colors["DARK1"] = FL_DARK1;
		colors["LIGHT1"] = FL_LIGHT1;
		colors["LIGHT2"] = FL_LIGHT2;
		colors["LIGHT3"] = FL_LIGHT3;
		colors["BLACK"] = FL_BLACK;
		colors["RED"] = FL_RED;
		colors["GREEN"] = FL_GREEN;
		colors["YELLOW"] = FL_YELLOW;
		colors["BLUE"] = FL_BLUE;
		colors["MAGENTA"] = FL_MAGENTA;
		colors["CYAN"] = FL_CYAN;
		colors["DARK_RED"] = FL_DARK_RED;
		colors["DARK_GREEN"] = FL_DARK_GREEN;
		colors["DARK_YELLOW"] = FL_DARK_YELLOW;
		colors["DARK_BLUE"] = FL_DARK_BLUE;
		colors["DARK_MAGENTA"] = FL_DARK_MAGENTA;
		colors["DARK_CYAN"] = FL_DARK_CYAN;
		colors["WHITE"] = FL_WHITE;

		buttontypes["Regular"] = 0;
		buttontypes["Check"] = 1;
		buttontypes["Light"] = 2;
		buttontypes["Repeat"] = 3;
		buttontypes["Return"] = 4;
		buttontypes["Round"] = 5;
		buttontypes["Image"] = 6;


		actions["PUSH"] = FL_PUSH;
		actions["RELEASE"] = FL_RELEASE;
		actions["DRAG"] = FL_DRAG;
		actions["MOVE"] = FL_MOVE;
		actions["MOUSEWHEEL"] = FL_MOUSEWHEEL;
		actions["KEYDOWN"] = FL_KEYDOWN;
		actions["KEYUP"] = FL_KEYUP;

		slidertype["VERT_SLIDER"] = FL_VERT_SLIDER;
		slidertype["HOR_SLIDER"] = FL_HOR_SLIDER;
		slidertype["VERT_FILL_SLIDER"] = FL_VERT_FILL_SLIDER;
		slidertype["HOR_FILL_SLIDER"] = FL_HOR_FILL_SLIDER;
		slidertype["VERT_NICE_SLIDER"] = FL_VERT_NICE_SLIDER;
		slidertype["HOR_NICE_SLIDER"] = FL_HOR_NICE_SLIDER;

		lineshape["SOLID"] = FL_SOLID;
		lineshape["DASH"] = FL_DASH;
		lineshape["DOT"] = FL_DOT;
		lineshape["DASHDOT"] = FL_DASHDOT;
		lineshape["DASHDOTDOT"] = FL_DASHDOTDOT;
		lineshape["CAP_FLAT"] = FL_CAP_FLAT;
		lineshape["CAP_ROUND"] = FL_CAP_ROUND;
		lineshape["CAP_SQUARE"] = FL_CAP_SQUARE;
		lineshape["JOIN_MITER"] = FL_JOIN_MITER;
		lineshape["JOIN_ROUND"] = FL_JOIN_ROUND;
		lineshape["JOIN_BEVEL"] = FL_JOIN_BEVEL;

		aligns["ALIGN_CENTER"] = FL_ALIGN_CENTER;
		aligns["ALIGN_TOP"] = FL_ALIGN_TOP;
		aligns["ALIGN_BOTTOM"] = FL_ALIGN_BOTTOM;
		aligns["ALIGN_LEFT"] = FL_ALIGN_LEFT;
		aligns["ALIGN_RIGHT"] = FL_ALIGN_RIGHT;
		aligns["ALIGN_INSIDE"] = FL_ALIGN_INSIDE;
		aligns["ALIGN_TEXT_OVER_IMAGE"] = FL_ALIGN_TEXT_OVER_IMAGE;
		aligns["ALIGN_IMAGE_OVER_TEXT"] = FL_ALIGN_IMAGE_OVER_TEXT;
		aligns["ALIGN_CLIP"] = FL_ALIGN_CLIP;
		aligns["ALIGN_WRAP"] = FL_ALIGN_WRAP;
		aligns["ALIGN_IMAGE_NEXT_TO_TEXT"] = FL_ALIGN_IMAGE_NEXT_TO_TEXT;
		aligns["ALIGN_TEXT_NEXT_TO_IMAGE"] = FL_ALIGN_TEXT_NEXT_TO_IMAGE;
		aligns["ALIGN_IMAGE_BACKDROP"] = FL_ALIGN_IMAGE_BACKDROP;
		aligns["ALIGN_TOP_LEFT"] = FL_ALIGN_TOP_LEFT;
		aligns["ALIGN_TOP_RIGHT"] = FL_ALIGN_TOP_RIGHT;
		aligns["ALIGN_BOTTOM_LEFT"] = FL_ALIGN_BOTTOM_LEFT;
		aligns["ALIGN_BOTTOM_RIGHT"] = FL_ALIGN_BOTTOM_RIGHT;
		aligns["ALIGN_LEFT_TOP"] = FL_ALIGN_LEFT_TOP;
		aligns["ALIGN_RIGHT_TOP"] = FL_ALIGN_RIGHT_TOP;
		aligns["ALIGN_LEFT_BOTTOM"] = FL_ALIGN_LEFT_BOTTOM;
		aligns["ALIGN_RIGHT_BOTTOM"] = FL_ALIGN_RIGHT_BOTTOM;
		aligns["ALIGN_NOWRAP"] = FL_ALIGN_NOWRAP;
		aligns["ALIGN_POSITION_MASK"] = FL_ALIGN_POSITION_MASK;
		aligns["ALIGN_IMAGE_MASK"] = FL_ALIGN_IMAGE_MASK;

		labeltypes["NORMAL_LABEL"] = FL_NORMAL_LABEL;
		labeltypes["NO_LABEL"] = FL_NO_LABEL;
		labeltypes["SHADOW_LABEL"] = _FL_SHADOW_LABEL;
		labeltypes["ENGRAVED_LABEL"] = _FL_ENGRAVED_LABEL;
		labeltypes["EMBOSSED_LABEL"] = _FL_EMBOSSED_LABEL;

		cursors["CURSOR_DEFAULT"] = FL_CURSOR_DEFAULT;
		cursors["CURSOR_ARROW"] = FL_CURSOR_ARROW;
		cursors["CURSOR_CROSS"] = FL_CURSOR_CROSS;
		cursors["CURSOR_WAIT"] = FL_CURSOR_WAIT;
		cursors["CURSOR_INSERT"] = FL_CURSOR_INSERT;
		cursors["CURSOR_HAND"] = FL_CURSOR_HAND;
		cursors["CURSOR_HELP"] = FL_CURSOR_HELP;
		cursors["CURSOR_MOVE"] = FL_CURSOR_MOVE;
		cursors["CURSOR_NS"] = FL_CURSOR_NS;
		cursors["CURSOR_WE"] = FL_CURSOR_WE;
		cursors["CURSOR_NWSE"] = FL_CURSOR_NWSE;
		cursors["CURSOR_NESW"] = FL_CURSOR_NESW;
		cursors["CURSOR_NONE"] = FL_CURSOR_NONE;
		cursors["CURSOR_N"] = FL_CURSOR_N;
		cursors["CURSOR_NE"] = FL_CURSOR_NE;
		cursors["CURSOR_E"] = FL_CURSOR_E;
		cursors["CURSOR_SE"] = FL_CURSOR_SE;
		cursors["CURSOR_S"] = FL_CURSOR_S;
		cursors["CURSOR_SW"] = FL_CURSOR_SW;
		cursors["CURSOR_W"] = FL_CURSOR_W;
		cursors["CURSOR_NW"] = FL_CURSOR_NW;

		cursors["NORMAL_CURSOR"] = Fl_Text_Display::NORMAL_CURSOR;
		cursors["CARET_CURSOR"] = Fl_Text_Display::CARET_CURSOR;
		cursors["DIM_CURSOR"] = Fl_Text_Display::DIM_CURSOR;
		cursors["BLOCK_CURSOR"] = Fl_Text_Display::BLOCK_CURSOR;
		cursors["HEAVY_CURSOR"] = Fl_Text_Display::HEAVY_CURSOR;
		cursors["LINE_CURSOR"] = 5;

		commands["SHIFT"] = FL_SHIFT;
		commands["CAPS_LOCK"] = FL_CAPS_LOCK;
		commands["CTRL"] = FL_CTRL;
		commands["ALT"] = FL_ALT;
		commands["NUM_LOCK"] = FL_NUM_LOCK;
		commands["META"] = FL_META;
		commands["SCROLL_LOCK"] = FL_SCROLL_LOCK;
		commands["BUTTON1"] = FL_BUTTON1;
		commands["BUTTON2"] = FL_BUTTON2;
		commands["BUTTON3"] = FL_BUTTON3;
		commands["BUTTONS"] = FL_BUTTONS;
		commands["KEY_MASK"] = FL_KEY_MASK;
		commands["COMMAND"] = FL_COMMAND;
		commands["CONTROL"] = FL_CONTROL;

		keyCodes["Button"] = 0xfee8;
		keyCodes["BackSpace"] = 0xff08;
		keyCodes["Tab"] = 0xff09;
		keyCodes["Enter"] = 0xff0d;
		keyCodes["Pause"] = 0xff13;
		keyCodes["Scroll_Lock"] = 0xff14;
		keyCodes["Escape"] = 0xff1b;
		keyCodes["Home"] = 0xff50;
		keyCodes["Left"] = 0xff51;
		keyCodes["Up"] = 0xff52;
		keyCodes["Right"] = 0xff53;
		keyCodes["Down"] = 0xff54;
		keyCodes["Page_Up"] = 0xff55;
		keyCodes["Page_Down"] = 0xff56;
		keyCodes["End"] = 0xff57;
		keyCodes["Print"] = 0xff61;
		keyCodes["Insert"] = 0xff63;
		keyCodes["Menu"] = 0xff67;
		keyCodes["Help"] = 0xff68;
		keyCodes["Num_Lock"] = 0xff7f;
		keyCodes["KP"] = 0xff80;
		keyCodes["KP_Enter"] = 0xff8d;
		keyCodes["KP_Last"] = 0xffbd;
		keyCodes["F"] = 0xffbd;
		keyCodes["F_Last"] = 0xffe0;
		keyCodes["Shift_L"] = 0xffe1;
		keyCodes["Shift_R"] = 0xffe2;
		keyCodes["Control_L"] = 0xffe3;
		keyCodes["Control_R"] = 0xffe4;
		keyCodes["Caps_Lock"] = 0xffe5;
		keyCodes["Meta_L"] = 0xffe7;
		keyCodes["Meta_R"] = 0xffe8;
		keyCodes["Alt_L"] = 0xffe9;
		keyCodes["Alt_R"] = 0xffea;
		keyCodes["Delete"] = 0xffff;

		boxTypes["NO_BOX"] = 0;
		boxTypes["FLAT_BOX"] = 1;
		boxTypes["UP_BOX"] = 2;
		boxTypes["DOWN_BOX"] = 3;
		boxTypes["UP_FRAME"] = 4;
		boxTypes["DOWN_FRAME"] = 5;
		boxTypes["THIN_UP_BOX"] = 6;
		boxTypes["THIN_DOWN_BOX"] = 7;
		boxTypes["THIN_UP_FRAME"] = 8;
		boxTypes["THIN_DOWN_FRAME"] = 9;
		boxTypes["ENGRAVED_BOX"] = 10;
		boxTypes["EMBOSSED_BOX"] = 11;
		boxTypes["ENGRAVED_FRAME"] = 12;
		boxTypes["EMBOSSED_FRAME"] = 13;
		boxTypes["BORDER_BOX"] = 14;
		boxTypes["SHADOW_BOX"] = 15;
		boxTypes["BORDER_FRAME"] = 16;
		boxTypes["SHADOW_FRAME"] = 17;
		boxTypes["ROUNDED_BOX"] = 18;
		boxTypes["RSHADOW_BOX"] = 19;
		boxTypes["ROUNDED_FRAME"] = 20;
		boxTypes["RFLAT_BOX"] = 21;
		boxTypes["ROUND_UP_BOX"] = 22;
		boxTypes["ROUND_DOWN_BOX"] = 23;
		boxTypes["DIAMOND_UP_BOX"] = 24;
		boxTypes["DIAMOND_DOWN_BOX"] = 25;
		boxTypes["OVAL_BOX"] = 26;
		boxTypes["OSHADOW_BOX"] = 27;
		boxTypes["OVAL_FRAME"] = 28;
		boxTypes["OFLAT_BOX"] = 29;
		boxTypes["PLASTIC_UP_BOX"] = 30;
		boxTypes["PLASTIC_DOWN_BOX"] = 31;
		boxTypes["PLASTIC_UP_FRAME"] = 32;
		boxTypes["PLASTIC_DOWN_FRAME"] = 33;
		boxTypes["PLASTIC_THIN_UP_BOX"] = 34;
		boxTypes["PLASTIC_THIN_DOWN_BOX"] = 35;
		boxTypes["PLASTIC_ROUND_UP_BOX"] = 36;
		boxTypes["PLASTIC_ROUND_DOWN_BOX"] = 37;
		boxTypes["GTK_UP_BOX"] = 38;
		boxTypes["GTK_DOWN_BOX"] = 39;
		boxTypes["GTK_UP_FRAME"] = 40;
		boxTypes["GTK_DOWN_FRAME"] = 41;
		boxTypes["GTK_THIN_UP_BOX"] = 42;
		boxTypes["GTK_THIN_DOWN_BOX"] = 43;
		boxTypes["GTK_THIN_UP_FRAME"] = 44;
		boxTypes["GTK_THIN_DOWN_FRAME"] = 45;
		boxTypes["GTK_ROUND_UP_BOX"] = 46;
		boxTypes["GTK_ROUND_DOWN_BOX"] = 47;
		boxTypes["FREE_BOXTYPE"] = 48;

		fonts["HELVETICA"] = 0;
		fonts["HELVETICA_BOLD"] = 1;
		fonts["HELVETICA_ITALIC"] = 2;
		fonts["HELVETICA_BOLD_ITALIC"] = 3;
		fonts["COURIER"] = 4;
		fonts["COURIER_BOLD"] = 5;
		fonts["COURIER_ITALIC"] = 6;
		fonts["COURIER_BOLD_ITALIC"] = 7;
		fonts["TIMES"] = 8;
		fonts["TIMES_BOLD"] = 9;
		fonts["TIMES_ITALIC"] = 10;
		fonts["TIMES_BOLD_ITALIC"] = 11;
		fonts["SYMBOL"] = 12;
		fonts["SCREEN"] = 13;
		fonts["SCREEN_BOLD"] = 14;
		fonts["ZAPF_DINGBATS"] = 15;
		fonts["FREE_FONT"] = 16;
		fonts["BOLD"] = 1;
		fonts["ITALIC"] = 2;
		fonts["BOLD_ITALIC"] = 3;

		ifonts[0] = "HELVETICA";
		ifonts[1] = "HELVETICA_BOLD";
		ifonts[2] = "HELVETICA_ITALIC";
		ifonts[3] = "HELVETICA_BOLD_ITALIC";
		ifonts[4] = "COURIER";
		ifonts[5] = "COURIER_BOLD";
		ifonts[6] = "COURIER_ITALIC";
		ifonts[7] = "COURIER_BOLD_ITALIC";
		ifonts[8] = "TIMES";
		ifonts[9] = "TIMES_BOLD";
		ifonts[10] = "TIMES_ITALIC";
		ifonts[11] = "TIMES_BOLD_ITALIC";
		ifonts[12] = "SYMBOL";
		ifonts[13] = "SCREEN";
		ifonts[14] = "SCREEN_BOLD";
		ifonts[15] = "ZAPF_DINGBATS";
		ifonts[16] = "FREE_FONT";

		buttonShapes["NORMAL_BUTTON"] = FL_NORMAL_BUTTON;
		buttonShapes["TOGGLE_BUTTON"] = FL_TOGGLE_BUTTON;
		buttonShapes["RADIO_BUTTON"] = FL_RADIO_BUTTON;
		buttonShapes["HIDDEN_BUTTON"] = FL_HIDDEN_BUTTON;

		buttonAction["WHEN_NEVER"] = FL_WHEN_NEVER;
		buttonAction["WHEN_CHANGED"] = FL_WHEN_CHANGED;
		buttonAction["WHEN_RELEASE"] = FL_WHEN_RELEASE;
		buttonAction["WHEN_RELEASE_ALWAYS"] = FL_WHEN_RELEASE_ALWAYS;
		buttonAction["WHEN_ENTER_KEY"] = FL_WHEN_ENTER_KEY;
		buttonAction["WHEN_ENTER_KEY_ALWAYS"] = FL_WHEN_ENTER_KEY_ALWAYS;
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

	name = "FL_NORMAL_SIZE";
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

	name = "FL_DIR_SINGLE";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, 0);

	name = "FL_DIR_MULTI";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, 1);

	name = "FL_DIR_CREATE";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, 2);

	name = "FL_DIR_DIRECTORY";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, 4);

	//Tree flags
	name = "FL_TREE_SELECT_NONE";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_SELECT_NONE);

	name = "FL_TREE_SELECT_SINGLE";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_SELECT_SINGLE);

	name = "FL_TREE_SELECT_MULTI";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_SELECT_MULTI);

	name = "FL_TREE_SORT_NONE";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_SORT_NONE);

	name = "FL_TREE_SORT_ASCENDING";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_SORT_ASCENDING);

	name = "FL_TREE_SORT_DESCENDING";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_SORT_DESCENDING);

	name = "FL_TREE_CONNECTOR_NONE";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_CONNECTOR_NONE);

	name = "FL_TREE_CONNECTOR_DOTTED";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_CONNECTOR_DOTTED);

	name = "FL_TREE_CONNECTOR_SOLID";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_CONNECTOR_SOLID);

	name = "FL_TREE_REASON_NONE";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_REASON_NONE);

	name = "FL_TREE_REASON_SELECTED";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_REASON_SELECTED);

	name = "FL_TREE_REASON_DESELECTED";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_REASON_DESELECTED);

	name = "FL_TREE_REASON_OPENED";
	var = kifcode->Createstaticvariable(name);
	local = new KifBaseInteger(kifcode, var, FL_TREE_REASON_OPENED);

	name = "FL_TREE_REASON_CLOSED";
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
		kifcode->Returnerror("WND(098): Widget cannot be instantiated");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		string tocopy = callfunc->Evaluatethread(0, domain, _thread)->String();

		Fl::copy(STR(tocopy), tocopy.length(), 1);
		return kifTRUE;
	}

	virtual KifElement* MethodPaste(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		Fl::paste(*Widget(), 1);
		return kifTRUE;
	}

	KifElement* MethodAlign(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(805): Widget not initialized");

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == kifString) {
			string align = ke->String();

			if (aligns.find(align) == aligns.end()) {
				align = "WND(202): Unknown alignement";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(805): Widget not initialized");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(window->color());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

		Fl_Widget* wdg;
		//If we are in the main thread, well business as usual		
		wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");
		wdg->take_focus();
		return kifTRUE;
	}

	KifElement* MethodCoordinates(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");
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
				return Returningerror(kifcode, _thread, idcallthread, "WND(904): Expecting a vector");
			kvect = ke;
			if (kvect->Size() != 4)
				return Returningerror(kifcode, _thread, idcallthread, "WND(905): Missing coordinates");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(905): Incoherent coordinates");
		wdg->resize(x, y, w, h);
		return kifTRUE;
	}

	KifElement* MethodTooltip(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wdg->selection_color());

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		wdg->hide();
		return kifTRUE;
	}

	KifElement* MethodShow(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");
		wdg->show();
		return kifTRUE;
	}


	KifElement* MethodLabelColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		//In our example, we have only two parameters


		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wdg->labelcolor());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string shortcut = ke->String();
			if (colors.find(shortcut) == colors.end()) {
				shortcut = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): wdg not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): wdg not created");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wdg->labelfont());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* labelfont;
		int font = 0;
		for (int i = 0; i < callfunc->Size(); i++) {
			labelfont = callfunc->Evaluatethread(i, domain, _thread);
			if (labelfont->type == kifString) {
				if (fonts.find(labelfont->String()) == fonts.end()) {
					string mes = "WND(765): Unknown font:";
					mes += labelfont->String();
					return Returningerror(kifcode, _thread, idcallthread, mes);
				}
				font = fonts[labelfont->String()];
			}
			else
				font = labelfont->Integer();
		}

		if (font >= fontmax)
			return Returningerror(kifcode, _thread, idcallthread, "WND(658): unknown font");
		wdg->labelfont(font);
		return kifTRUE;
	}

	KifElement* MethodLabelType(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;


		Fl_Widget* wdg = Widget();
		if (wdg == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): wdg not created");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): wdg not created");

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

	//this function is called to test the kind of function we expect from the library with the "with" operator...
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
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(805): Image already loaded");
		KifElement* kfilename = callfunc->Evaluatethread(0, domain, _thread);
		filename = kfilename->String();
		if (filename != "") {
			image = new Fl_JPEG_Image(filename.c_str());
			if (image == NULL)
				return Returningerror(kifcode, _thread, idcallthread, "WND(805): Image not loaded");
			SaveItem(this);
			return kifTRUE;
		}
		return kifFALSE;
	}
#endif

	KifElement* MethodLoadImageGIF(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (image != NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(805): Image already loaded");
		KifElement* kfilename = callfunc->Evaluatethread(0, domain, _thread);
		filename = kfilename->String();
		if (filename != "") {
			image = new Fl_GIF_Image(filename.c_str());
			if (image == NULL)
				return Returningerror(kifcode, _thread, idcallthread, "WND(805): Image not loaded");
			SaveItem(this);
			return kifTRUE;
		}
		return kifFALSE;
	}

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, idthread, idthread, "WND(809): Wrong type for the first parameter");

		KifElement* ke = params->Instruction(1)->Exec(kifNULL, domloc, kifNULL, idthread, callfunc);

		szw = ke->Integer();
		ke->Release();
		ke = params->Instruction(2)->Exec(kifNULL, domloc, kifNULL, idthread, callfunc);
		szh = ke->Integer();
		ke->Release();

		int sz = kbitmaps->Size();
		
		int tst = (szw*szh) >> 3;
		if (tst!=sz)
			return Returningerror(kifcode, idthread, idthread, "WND(811): Wrong size for the bitmap. sz=(width*height)/8");
		
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
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(808): Bitmap already loaded");
		KifElement* kbitmaps = callfunc->Evaluatethread(0, domain, _thread);
		szw = callfunc->Evaluatethread(1, domain, _thread)->Integer();
		szh = callfunc->Evaluatethread(2, domain, _thread)->Integer();
		if (!kbitmaps->aVectorContainer())
			return Returningerror(kifcode, _thread, idcallthread, "WND(809): Wrong type for the first parameter");

		int sz = kbitmaps->Size();
		int tst = (szw*szh) >> 3;
		if (tst != sz)
			return Returningerror(kifcode, _thread, idcallthread, "WND(811): Wrong size for the bitmap. sz=(width*height)/8");

		int rowBytes = (szw + 7) >> 3;
		bm = new uchar[rowBytes*szh];

		for (int i = 0; i < sz; i++)
			bm[i] = (uchar)kbitmaps->getinteger(i);
		bitmap = new Fl_Bitmap(bm, szw, szh);
		//delete[] bm;
		return kifTRUE;
	}

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);

		fl_point(x->Integer(), y->Integer());
		return kifTRUE;
	}

	KifElement* MethodDrawColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");


		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		int w = callfunc->Evaluatethread(1, domain, _thread)->Integer();

		//0 is the first parameter and so on...
		if (ke->type == kifString) {
			string buf = ke->String();
			if (lineshape.find(buf) == lineshape.end()) {
				buf = "WND(607): Unknown line shape code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		if (callfunc->Size() == 3) {
			KifElement* r = callfunc->Evaluatethread(0, domain, _thread);
			KifElement* g = callfunc->Evaluatethread(1, domain, _thread);
			KifElement* b = callfunc->Evaluatethread(2, domain, _thread);
			return kifcode->Provideinteger(fl_rgb_color(r->Integer(), g->Integer(), b->Integer()));
		}

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->aVectorContainer()) {
			if (ke->Size() != 3)
				return Returningerror(kifcode, _thread, idcallthread, "WND(245): Wrong number of elements in the color vector ");
			return kifcode->Provideinteger(fl_rgb_color(ke->ValueOnIndex(0)->Integer(), ke->ValueOnIndex(1)->Integer(), ke->ValueOnIndex(2)->Integer()));
		}

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
					buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
					buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* t = callfunc->Evaluatethread(0, domain, _thread);

		string buf = t->String();
		int w;
		int h;
		fl_measure(buf.c_str(), w, h, 1);
		KifMapStringInteger* map = new KifMapStringInteger(kifcode, NULL);
		map->values["W"] = w;
		map->values["H"] = h;
		return map;
	}

	KifElement* MethodDrawText(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* f = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* sz = callfunc->Evaluatethread(1, domain, _thread);
		if (f->type == kifString) {
			string buf = f->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = "WND(657): Unknown font code";
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			fl_font(fonts[buf], sz->Integer());
		}
		else {
			int i = f->Integer();
			if (i >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, "WND(658): unknown font");
			fl_font(i, sz->Integer());
		}
		return kifTRUE;
	}

	KifElement* MethodArc(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* w = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* h = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* a1 = callfunc->Evaluatethread(4, domain, _thread);
		if (begins.size() != 0) {
			if (callfunc->Size() == 6)
				return Returningerror(kifcode, _thread, idcallthread, "WND(608): Wrong number of parameters=expected 5 in a complex shape construction");
			fl_arc(x->Float(), y->Float(), w->Float(), h->Float(), a1->Float());
		}
		else {
			if (callfunc->Size() != 6)
				return Returningerror(kifcode, _thread, idcallthread, "WND(609): Wrong number of parameters=expected 6 in a regular construction");
			KifElement* a2 = callfunc->Evaluatethread(5, domain, _thread);
			fl_arc(x->Integer(), y->Integer(), w->Integer(), h->Integer(), a1->Float(), a2->Float());
		}
		return kifTRUE;
	}


	KifElement* MethodMultmatrix(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* d = callfunc->Evaluatethread(0, domain, _thread);
		fl_rotate(d->Float());

		return kifTRUE;
	}


	KifElement* MethodTranslate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		fl_translate(x->Float(), y->Float());

		return kifTRUE;
	}


	KifElement* MethodTransform_vertex(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		fl_transformed_vertex(x->Float(), y->Float());

		return kifTRUE;
	}


	KifElement* MethodTransform_dy(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		double v = fl_transform_dy(x->Float(), y->Float());

		return kifcode->Providefloat(v);
	}


	KifElement* MethodTransform_dx(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		double v = fl_transform_dx(x->Float(), y->Float());

		return kifcode->Providefloat(v);
	}


	KifElement* MethodTransform_x(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		double v = fl_transform_x(x->Float(), y->Float());

		return kifcode->Providefloat(v);
	}

	KifElement* MethodTransform_y(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		double v = fl_transform_y(x->Float(), y->Float());

		return kifcode->Providefloat(v);
	}


	KifElement* MethodScale(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");
		begins.push_back(6);
		fl_push_matrix();
		return kifTRUE;
	}

	KifElement* MethodPopMatrix(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (!begins.size() || begins.back() != 6)
			return Returningerror(kifcode, _thread, idcallthread, "WND(612): Cannot call this method here. Have you called beginpoints before?");

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");
		begins.pop_back();
		fl_pop_matrix();
		return kifTRUE;
	}

	KifElement* MethodBeginPoints(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");
		begins.push_back(1);
		fl_begin_points();
		return kifTRUE;
	}

	KifElement* MethodEndPoints(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size() || begins.back() != 1)
			return Returningerror(kifcode, _thread, idcallthread, "WND(612): Cannot call this method here. Have you called beginpoints before?");

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");
		begins.pop_back();
		fl_end_points();
		return kifTRUE;
	}

	KifElement* MethodBeginLoop(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");
		begins.push_back(2);
		fl_begin_loop();
		return kifTRUE;
	}

	KifElement* MethodEndLoop(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size() || begins.back() != 2)
			return Returningerror(kifcode, _thread, idcallthread, "WND(612): Cannot call this method here. Have you called beginloop before?");

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		begins.pop_back();
		fl_end_loop();
		return kifTRUE;
	}

	KifElement* MethodBeginPolygon(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		begins.push_back(3);
		fl_begin_polygon();
		return kifTRUE;
	}

	KifElement* MethodEndPolygon(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size() || begins.back() != 3)
			return Returningerror(kifcode, _thread, idcallthread, "WND(612): Cannot call this method here. Have you called beginpolygon before?");

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		begins.pop_back();
		fl_end_polygon();
		return kifTRUE;
	}

	KifElement* MethodBeginComplexPolygon(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		begins.push_back(4);
		fl_begin_complex_polygon();
		return kifTRUE;
	}

	KifElement* MethodGap(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size() || begins.back() != 4)
			return Returningerror(kifcode, _thread, idcallthread, "WND(612): Cannot call this method here. Have you called begincomplexpolygon before?");

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		fl_gap();
		return kifTRUE;
	}

	KifElement* MethodEndComplexPolygon(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size() || begins.back() != 4)
			return Returningerror(kifcode, _thread, idcallthread, "WND(612): Cannot call this method here. Have you called begincomplexpolygon before?");

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		begins.pop_back();
		fl_end_complex_polygon();
		return kifTRUE;
	}

	KifElement* MethodBeginLine(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		begins.push_back(5);
		fl_begin_line();
		return kifTRUE;
	}

	KifElement* MethodEndLine(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size() || begins.back() != 5)
			return Returningerror(kifcode, _thread, idcallthread, "WND(612): Cannot call this method here. Have you called beginline before?");

		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		begins.pop_back();
		fl_end_line();
		return kifTRUE;
	}

	KifElement* MethodCurve(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (!begins.size())
			return Returningerror(kifcode, _thread, idcallthread, "WND(610): You cannot use this function outside a complex shape construction");


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(610): You cannot use this function outside a complex shape construction");


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");


		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);
		fl_vertex(x->Float(), y->Float());
		return kifTRUE;
	}

	KifElement* MethodPopClip(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		fl_pop_clip();
		return kifTRUE;
	}

	KifElement* MethodPushClip(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters


		if (Widget() == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");


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
			return Returningerror(kifcode, _thread, idcallthread, "WND(100): Cannot recreate an existing window");
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
			return kifcode->Returnerror("WND(431): Unavailable font id");

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
			return kifcode->Returnerror("WND(431): Unavailable font id");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");


		KifElement* function = callfunc->Evaluatethread(0, domain, _thread);

		if (function->type != kifCall && !function->isFunction() && !testfunction(function))
			return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong element, the first parameter should be a function");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");


		KifElement* function = callfunc->Evaluatethread(0, domain, _thread);

		if (function->type != kifCall && !function->isFunction() && !testfunction(function))
			return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong element, the first parameter should be a function");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(100): Cannot recreate an existing window");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");
		window->flush();
		return kifTRUE;
	}

	KifElement* MethodLabel(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");
		if (end)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): Window already instantiated");

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
				return Returningerror(kifcode, _thread, idcallthread, "WND(341): Unknown mouse action");
			act = actions[action];
		}
		else
			act = kaction->Integer();
		if (window->Setmouseaction(act, function, object) == false)
			return Returningerror(kifcode, _thread, idcallthread, "WND(342): Wrong parameter in your mouse callback");
		return kifTRUE;
	}

	KifElement* MethodOnKey(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");
		if (end)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): Window already instantiated");

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
				return Returningerror(kifcode, _thread, idcallthread, "WND(344): Unknown keyboard action");
			act = actions[action];
		}
		else
			act = kaction->Integer();

		if (window->Setkeyaction(act, function, object) == false)
			return Returningerror(kifcode, _thread, idcallthread, "WND(343): Wrong parameter in your keyboard callback");
		return kifTRUE;
	}

	KifElement* MethodWindowHide(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);


		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

		//0 is the first parameter and so on...
		KifElement* kaction = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kcolor1 = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* kcolor2 = callfunc->Evaluatethread(2, domain, _thread);
		Fl_Cursor act;
		if (kaction->type == kifString) {
			string action = kaction->String();
			if (cursors.find(action) == cursors.end())
				return Returningerror(kifcode, _thread, idcallthread, "WND(344): Unknown keyboard action");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

		KifElement* table = callfunc->Evaluatethread(0, domain, _thread);
		if (!table->aVectorContainer())
			return kifcode->Returnerror("WND(872): We expect a vector as first parameter.");

		if (table->Size() % 2 != 0)
			return kifcode->Returnerror("WND(871): The vector should contain an even number of elements.");


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
				return kifcode->Returnerror("WND(873): We expect a vector as third parameter");
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
			return kifcode->Returnerror("WND(862): Wrong values to compute coordinates");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");
		if (end)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): Window already instantiated");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

		KifElement* kimage = callfunc->Evaluatethread(0, domain, _thread);
		if (kimage->type != image_type)
			return Returningerror(kifcode, _thread, idcallthread, "WND(167): Unknown image");

		KifElement* kx = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* ky = callfunc->Evaluatethread(2, domain, _thread);
		KifElement* kw = callfunc->Evaluatethread(3, domain, _thread);
		KifElement* kh = callfunc->Evaluatethread(4, domain, _thread);
		Fl_Image* fl = ((Kifimage*)kimage)->image;
		if (fl == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(168): Unloaded image");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

		KifElement* kbitmap = callfunc->Evaluatethread(0, domain, _thread);
		if (kbitmap->type != bitmap_type)
			return Returningerror(kifcode, _thread, idcallthread, "WND(169): Unknown bitmap");

		KifElement* ke = callfunc->Evaluatethread(1, domain, _thread);
		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(169): Unknown bitmap");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");
		if (end)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): Window already instantiated");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");
		if (end)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): Window already instantiated");

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		//we expect a vector
		if (ke->aVectorContainer() == false)
			return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong parameter, vector expected");
		KifElement* menu = ke;
		vector<Fl_Menu_Item*> elements;
		Fl_Menu_Item* fl;
		for (int i = 0; i < menu->Size(); i++) {
			//First level
			//each element is a vector
			ke = menu->ValueOnIndex(i);
			if (ke->aVectorContainer() == false)
				return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong element, vector expected");
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
						return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong element, vector expected");
					KifElement* v = element->ValueOnIndex(i);
					//First element should be the name
					if (v->Size() != 5)
						return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong element, missing field in a sub-menu");
					nn = v->ValueOnIndex(0)->String();
					n = strdup(STR(nn));
					//second element shoudl be a vector
					if (v->ValueOnIndex(1)->aVectorContainer() == false)
						return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong element, second field in a sub-menu should be a vector");
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
						return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong element, third field in a sub-menu should be a function");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");
		if (end)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): Window already instantiated");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

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
			msg = "get";

		const char* ret = fl_input(msg.c_str());
		if (ret == NULL)
			return kifNULL;
		return kifcode->Providestringraw(ret);
	}

	KifElement* MethodResizable(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (window == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");
		if (end == false)
			return Returningerror(kifcode, _thread, idcallthread, "WND(123): Operation not available");

		if (callfunc->Size() == 0) {
			window->resizable(NULL);
			return kifTRUE;
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->Yourowntype() == KIFWIDGET) {
			Fl_Widget* fl = ((KifWidget*)ke)->Widget();
			if (fl == NULL)
				return Returningerror(kifcode, _thread, idcallthread, "WND(134): Widget has not been created yet");
			window->resizable(fl);
			return kifTRUE;
		}

		return Returningerror(kifcode, _thread, idcallthread, "WND(133): Widget expected as parameter");
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
		return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");
	}


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
	kmouse->values["button"] = Fl::event_button();
	kmouse->values["wheelx"] = Fl::event_dx();
	kmouse->values["wheely"] = Fl::event_dy();
	kmouse->values["x"] = Fl::event_x();
	kmouse->values["y"] = Fl::event_y();
	kmouse->values["xroot"] = Fl::event_x_root();
	kmouse->values["yroot"] = Fl::event_y_root();

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): input already created");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(677): Input not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(677): Input not initialized");

		int pos = input->position();
		int nb = input->mark();
		buf = input->value();
		buf = buf.substr(pos, nb);
		return kifcode->Providestring(buf);
	}

	KifElement* MethodColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (input == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(677): Input not initialized");

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
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(677): Input not initialized");

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
				buf = "WND(657): Unknown font code";
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}

			input->textfont(fonts[buf]);
		}
		else {
			int font = ke->Integer();
			if (font >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, "WND(658): unknown font");
			input->textfont(font);
		}
		return kifTRUE;
	}

	KifElement* MethodFontSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (input == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(677): Input not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(677): Input not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(677): Input not initialized");

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
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): editor already created");

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
				return kifcode->Returnerror("KIF(881): Error, wrong function arguments");
			Keeptrack(domain, contextualpattern);
			textbuf->add_modify_callback(editor_callback, this);
		}

		SaveItem(this);
		return kifTRUE;
	}

	KifElement* MethodOnVScroll(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): No editor available");


		KifElement* function = callfunc->Evaluatethread(0, domain, _thread);

		if (function->type != kifCall && !function->isFunction())
			return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong element, the first parameter should be a function");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): No editor available");


		KifElement* function = callfunc->Evaluatethread(0, domain, _thread);

		if (function->type != kifCall && !function->isFunction())
			return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong element, the first parameter should be a function");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): Widget not initialized");

		KifElement* t = callfunc->Evaluatethread(0, domain, _thread);

		string buf = t->String();
		int w;
		int h;
		fl_measure(buf.c_str(), w, h, 1);
		KifMapStringInteger* map = new KifMapStringInteger(kifcode, NULL);
		map->values["W"] = w;
		map->values["H"] = h;
		return map;
	}

	KifElement* MethodOnMouseClick(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): No editor available");

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
				return Returningerror(kifcode, _thread, idcallthread, "WND(341): Unknown mouse action");
			act = actions[action];
		}
		else
			act = kaction->Integer();

		if (editor->Setmouseaction(act, function, object) == false)
			return Returningerror(kifcode, _thread, idcallthread, "WND(342): Wrong parameter in your mouse callback");

		editor->mousedom = domain;
		editor->mousecontext = contextualpattern;

		return kifTRUE;
	}

	KifElement* MethodOnKey(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): No editor available");


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
				return Returningerror(kifcode, _thread, idcallthread, "WND(341): Unknown mouse action");
			act = actions[action];
		}
		else
			act = kaction->Integer();

		if (editor->Setkeyaction(act, function, object) == false)
			return Returningerror(kifcode, _thread, idcallthread, "WND(342): Wrong parameter in your key callback");

		editor->mousedom = domain;
		editor->mousecontext = contextualpattern;
		return kifTRUE;
	}

	bool InitializeItemMenu(string ckey, KifElement* kvect, unsigned char* key, int& ikey) {
		if (kvect->Size()<3 || kvect->Size()>5) {
			buf = "EDT(768): Wrong number of elements in the vector... 3 or 4 expected";
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
				buf = "WND(657): Unknown font code";
				return(false);
			}
			font = fonts[buf];
		}
		else
			font = kfont->Integer();

		if (font >= fontmax) {
			buf = "WND(658): unknown font";
			return false;
		}

		Fl_Color color;
		if (kcolor->type == kifString) {
			//0 is the first parameter and so on...
			buf = kcolor->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
					buf = "WND(657): Unknown color code";
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
				buf = "WND(627): Maximum number of styles reached";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
		bool first = true;
		if (stylebuf != NULL)
			first = false;

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->aMapContainer() == false)
			return Returningerror(kifcode, _thread, idcallthread, "EDT(766): Wrong input: map expected");

		int key;
		KifElement* kmap = ke;
		nbstyle = kmap->Size();
		if (nbstyle < stylekeys.size())
			nbstyle = stylekeys.size();

		if (nbstyle == 0)
			return Returningerror(kifcode, _thread, idcallthread, "EDT(770): Wrong input: empty map");

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
			return Returningerror(kifcode, _thread, idcallthread, "EDT(767): Missing '#' key in the map. '#' is the initial key and is mandatory");

		if (ke->aVectorContainer() == false)
			return Returningerror(kifcode, _thread, idcallthread, "EDT(767): Wrong input: vector expected");

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
				return Returningerror(kifcode, _thread, idcallthread, "EDT(767): Wrong input: vector expected");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
		//we need:
		string f = callfunc->Evaluatethread(0, domain, _thread)->String();
		textbuf->loadfile(STR(f));
		return kifTRUE;
	}

	KifElement* MethodSave(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
		//we need:
		string f = callfunc->Evaluatethread(0, domain, _thread)->String();
		textbuf->savefile(STR(f));
		return kifTRUE;
	}

	KifElement* MethodReplace(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

		int start = editor->insert_position();
		string str = callfunc->Evaluatethread(0, domain, _thread)->String();
		textbuf->append(STR(str));
		editor->redisplay_range(start, start + str.size());
		return kifTRUE;
	}


	KifElement* MethodInsert(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(689): Out of range");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
		if (styletable == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(679): Style table not initialized");

		//we need:
		KifElement* kfind = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kstyle = callfunc->Evaluatethread(1, domain, _thread);
		int matchcase = 0;
		if (callfunc->Size() == 3)
			matchcase = callfunc->Evaluatethread(2, domain, _thread)->Integer();

		string stylekey = kstyle->String();
		if (stylekeys.find(stylekey) == stylekeys.end())
			return Returningerror(kifcode, _thread, idcallthread, "WND(679): Unknown style key");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
		if (styletable == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(679): Style table not initialized");

		//we need:
		KifElement* krgx = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kstyle = callfunc->Evaluatethread(1, domain, _thread);
		string stylekey = kstyle->String();
		if (stylekeys.find(stylekey) == stylekeys.end())
			return Returningerror(kifcode, _thread, idcallthread, "WND(679): Unknown style key");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
		if (styletable == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(679): Style table not initialized");

		//we need:
		KifElement* kstart = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kend = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* kstyle = callfunc->Evaluatethread(2, domain, _thread);
		string stylekey = kstyle->String();
		if (stylekeys.find(stylekey) == stylekeys.end())
			return Returningerror(kifcode, _thread, idcallthread, "WND(679): Unknown style key");
		unsigned char stylecode = stylekeys[stylekey][0];
		return AddStyle(idcallthread, kstart->Integer(), kend->Integer(), stylecode, _thread);
	}

	KifElement* MethodHighlight(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(689): Out of range");
		textbuf->highlight(start, end);
		editor->insert_position(end);
		editor->show_insert_position();
		return kifNULL;
	}

	KifElement* MethodCursor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
		textbuf->unhighlight();
		return kifTRUE;
	}

	KifElement* MethodCursorStyle(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

		//0 is the first parameter and so on...
		KifElement* kaction = callfunc->Evaluatethread(0, domain, _thread);
		Fl_Cursor act;
		if (kaction->type == kifString) {
			string action = kaction->String();
			if (cursors.find(action) == cursors.end())
				return Returningerror(kifcode, _thread, idcallthread, "WND(344): Unknown keyboard action");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
		if (styletable == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(679): Style table not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
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
			contextualpattern->Push("start", kifcode->Provideinteger(start));
			contextualpattern->Push("end", kifcode->Provideinteger(end));
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
				buf = "WND(657): Unknown font code";
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}

			editor->textfont(fonts[buf]);
		}
		else {
			int font = ke->Integer();
			if (font >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, "WND(658): unknown font");
			editor->textfont(font);
		}
		return kifTRUE;
	}


	KifElement* MethodFontSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
		Fl_Text_Editor::kf_delete(0, editor);
		return kifTRUE;
	}

	KifElement* MethodCopy(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
		Fl_Text_Editor::kf_cut(0, editor);
		return kifTRUE;
	}

	KifElement* MethodPaste(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");
		Fl_Text_Editor::kf_paste(0, editor);
		return kifTRUE;
	}

	KifElement* MethodLine(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (editor == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(676): Editor not initialized");

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
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
	kmouse->values["button"] = Fl::event_button();
	kmouse->values["wheelx"] = Fl::event_dx();
	kmouse->values["wheely"] = Fl::event_dy();
	kmouse->values["x"] = x;
	kmouse->values["y"] = y;
	kmouse->values["xroot"] = xroot;
	kmouse->values["yroot"] = yroot;
	kmouse->values["cursor"] = pos;

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): output already created");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(679): Output not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(679): Output not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(679): Output not initialized");

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
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(679): Output not initialized");

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
				buf = "WND(657): Unknown font code";
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}

			output->textfont(fonts[buf]);
		}
		else {
			int font = ke->Integer();
			if (font >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, "WND(658): unknown font");
			output->textfont(font);
		}
		return kifTRUE;
	}

	KifElement* MethodFontSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (output == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(679): Output not initialized");

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
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): button already created");

		int shap;
		if (shape == NULL)
			shap = 0;
		else {
			if (shape->type == kifString) {
				if (buttonShapes.find(shape->String()) == buttonShapes.end()) {
					string mes = "WND(675): Unknwown button shape: ";
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
				string mes = "WND(675): Unknwown button type: ";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(805): Button not initialized");
		if (!image)
			return Returningerror(kifcode, _thread, idcallthread, "WND(809): Image button required");

		KifElement* kimage = callfunc->Evaluatethread(0, domain, _thread);
		if (kimage->type != image_type)
			return Returningerror(kifcode, _thread, idcallthread, "WND(813): image object required");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(805): Button not initialized");
		if (!image)
			return Returningerror(kifcode, _thread, idcallthread, "WND(809): Image button required");
		KifElement* kbitmap = callfunc->Evaluatethread(0, domain, _thread);
		if (kbitmap->type != bitmap_type)
			return Returningerror(kifcode, _thread, idcallthread, "WND(813): image object required");
		Fl_Bitmap* fl = ((Kifbitmap*)kbitmap)->bitmap;
		KifElement* kcolor = callfunc->Evaluatethread(1, domain, _thread);
		unsigned int c = FL_BLACK;
		if (kcolor->type == kifString) {
			//0 is the first parameter and so on...
			string shortcut = kcolor->String();
			if (colors.find(shortcut) == colors.end()) {
				shortcut = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(805): Button not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(805): Button not initialized");

		KifElement* ke;
		int action = 0;
		for (int i = 0; i < callfunc->Size(); i++) {
			ke = callfunc->Evaluatethread(i, domain, _thread);
			if (ke->type == kifString) {
				if (buttonAction.find(ke->String()) == buttonAction.end()) {
					string mess = "WND(654): Unknown action";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(805): Button not initialized");

		//0 is the first parameter and so on...
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == kifString) {
			string shortcut = ke->String();
			if (keyCodes.find(shortcut) == keyCodes.end()) {
				shortcut = "WND(656): Unknown key code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(805): Button not initialized");
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string shortcut = ke->String();
			if (colors.find(shortcut) == colors.end()) {
				shortcut = "WND(657): Unknown color code";
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
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): box already created");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): box not created");

		KifElement* pos = callfunc->Evaluatethread(0, domain, _thread);
		if (pos->type == kifString) {
			if (boxTypes.find(pos->String()) == boxTypes.end()) {
				string mes = "WND(767): Unknown type:";
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
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): group already created");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): group not created");

		Fl_Widget* tabs = group->parent();
		if (tabs == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(507): This group does not belong to a tab structure");
		((Fl_Tabs*)tabs)->value(group);
		return kifTRUE;
	}

	KifElement* MethodEnd(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters
		if (group == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): group not created");
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
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): tabs already created");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): tabs not created");
		end = true;
		tabs->end();
		return kifTRUE;
	}

	KifElement* MethodCurrent(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (tabs == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): tabs not created");
		if (callfunc->Size() == 0) {
			KiF_Group* flgroup = (KiF_Group*)tabs->value();
			if (flgroup == NULL)
				return Returningerror(kifcode, _thread, idcallthread, "WND(800): No active tab");
			return flgroup->associate;
		}

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type != group_type)
			return Returningerror(kifcode, _thread, idcallthread, "WND(105): You can only select wgroup object");
		Kifgroup* kgroup = (Kifgroup*)ke;
		if (kgroup->group == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(506): Empty group");
		if (kgroup->group->parent() != tabs)
			return Returningerror(kifcode, _thread, idcallthread, "WND(507): This group does not belong to that tab");
		tabs->value(kgroup->group);
		return kifTRUE;
	}

	KifElement* MethodAdd(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters
		if (tabs == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): tabs not created");
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		if (ke->type == group_type) {
			Kifgroup* kgroup = (Kifgroup*)ke;
			if (kgroup->group == NULL)
				return Returningerror(kifcode, _thread, idcallthread, "WND(506): Empty group");
			tabs->add(kgroup->group);
			tabs->value(kgroup->group);
			tabs->hide();
			tabs->show();
			return kifTRUE;
		}

		return Returningerror(kifcode, _thread, idcallthread, "WND(105): You can only add wgroup object");
	}

	KifElement* MethodRemove(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		//In our example, we have only two parameters
		return Returningerror(kifcode, _thread, idcallthread, "WND(800): Wrong number of parameters");
		if (tabs == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): tabs not created");
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == group_type) {
			Kifgroup* kgroup = (Kifgroup*)ke;
			if (kgroup->group == NULL)
				return Returningerror(kifcode, _thread, idcallthread, "WND(506): Empty group");
			if (kgroup->group->parent() != tabs)
				return Returningerror(kifcode, _thread, idcallthread, "WND(507): This group does not belong to that tab");
			kgroup->group->hide();
			tabs->remove(kgroup->group);
			delete kgroup->group;
			tabs->hide();
			tabs->show();
			return kifTRUE;
		}

		return Returningerror(kifcode, _thread, idcallthread, "WND(505): You can only remove wgroup object");
	}
	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): table already created");

		if (function != NULL && function->Size() != 3)
			return kifcode->Returnerror("KIF(881): Error, wrong function arguments");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

		table->clear();
		return kifTRUE;
	}

	KifElement* MethodRowHeader(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

		KifElement* kwidth = callfunc->Evaluatethread(0, domain, _thread);
		table->row_header_width(kwidth->Integer());
		return kifTRUE;
	}


	KifElement* MethodColumnHeader(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

		KifElement* kheight = callfunc->Evaluatethread(0, domain, _thread);
		table->col_header_height(kheight->Integer());
		return kifTRUE;
	}

	KifElement* MethodRow(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

		KifElement* kHeight = callfunc->Evaluatethread(0, domain, _thread);
		table->row_height_all(kHeight->Integer());
		return kifTRUE;
	}

	KifElement* MethodColumnWidth(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

		KifElement* kWidth = callfunc->Evaluatethread(0, domain, _thread);
		table->col_width_all(kWidth->Integer());
		return kifTRUE;
	}

	KifElement* MethodAdd(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

		KifElement* kR = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kC = callfunc->Evaluatethread(1, domain, _thread);
		KifElement* kvalue = callfunc->Evaluatethread(2, domain, _thread);

		table->cell(kR->Integer(), kC->Integer(), kvalue->String());
		return kifTRUE;
	}

	KifElement* MethodCell(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

		KifElement* kR = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* kC = callfunc->Evaluatethread(1, domain, _thread);

		string buff = table->cell(kR->Integer(), kC->Integer());

		return kifcode->Providestring(buff);
	}

	KifElement* MethodCellAlignment(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == kifString) {
			string align = ke->String();

			if (aligns.find(align) == aligns.end()) {
				align = "WND(202): Unknown alignement";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == kifString) {
			string align = ke->String();

			if (aligns.find(align) == aligns.end()) {
				align = "WND(202): Unknown alignement";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table has not been created yet");

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == kifString) {
			string align = ke->String();

			if (aligns.find(align) == aligns.end()) {
				align = "WND(202): Unknown alignement";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): table not initialized");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(table->Font());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		//0 is the first parameter and so on...
		if (ke->type == kifString) {
			string buf = ke->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = "WND(657): Unknown font code";
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			table->Font(fonts[buf]);
		}
		else {
			int font = ke->Integer();
			if (font >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, "WND(658): unknown font");
			table->Font(font);
		}

		return kifTRUE;
	}

	KifElement* MethodFontSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (table == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): table not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): table not initialized");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(table->GetCellBGColor());


		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(102): table not created");

		KifElement* pos = callfunc->Evaluatethread(0, domain, _thread);
		if (pos->type == kifString) {
			if (boxTypes.find(pos->String()) == boxTypes.end()) {
				string mes = "WND(767): Unknown type:";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(805): table not created");

		KifElement* ke;
		int action = 0;
		for (int i = 0; i < callfunc->Size(); i++) {
			ke = callfunc->Evaluatethread(i, domain, _thread);
			if (ke->type == kifString) {
				if (buttonAction.find(ke->String()) == buttonAction.end()) {
					string mess = "WND(654): Unknown action";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): table not initialized");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(table->GetCellBGColor());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): table not initialized");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(table->GetCellFGColor());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
		kmap->Push("top", o->kifcode->Provideinteger(rtop));
		kmap->Push("left", o->kifcode->Provideinteger(cleft));
		kmap->Push("bottom", o->kifcode->Provideinteger(rbot));
		kmap->Push("right", o->kifcode->Provideinteger(cright));
		KifMapStringString* valmap = new KifMapStringString(o->kifcode, NULL);
		kmap->Push("values", valmap);
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(100): Cannot recreate an existing scroll");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): No scroll available");

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
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(100): Cannot recreate an existing slider");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): No slider available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): No slider available");

		KifElement* pos = callfunc->Evaluatethread(0, domain, _thread);
		if (pos->type == kifString) {
			if (boxTypes.find(pos->String()) == boxTypes.end()) {
				string mes = "WND(767): Unknown type:";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): No slider available");

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		KifElement* y = callfunc->Evaluatethread(1, domain, _thread);

		slider->bounds(x->Integer(), y->Integer());
		return kifTRUE;
	}

	KifElement* MethodType(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (slider == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): No slider available");

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		if (x->type == kifString) {
			string buf = x->String();
			if (slidertype.find(buf) == slidertype.end())
				return Returningerror(kifcode, _thread, idcallthread, "WND(206): Wrong slider type");
			slider->type(slidertype[buf]);
		}
		else
			slider->type(x->Integer());
		return kifTRUE;
	}


	KifElement* MethodStep(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (slider == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): No slider available");

		KifElement* x = callfunc->Evaluatethread(0, domain, _thread);
		int i = x->Integer();
		slider->step(i);
		return kifTRUE;
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		if (slider == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): No slider available");

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
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(100): Cannot recreate an existing choice");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): choice not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): choice not initialized");

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
				buf = "WND(657): Unknown font code";
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}

			font = fonts[buf];
		}
		else
			font = ke->Integer();

		if (font >= fontmax)
			return Returningerror(kifcode, _thread, idcallthread, "WND(658): unknown font");
		choice->textfont(font);
		return kifTRUE;
	}

	KifElement* MethodFontColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);

		if (choice == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): choice not initialized");

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(678): choice not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(303): No window available");

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);
		//we expect a vector
		if (ke->aVectorContainer() == false)
			return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong parameter, vector expected");
		KifElement* menu = (KifElement*)ke;
		vector<Fl_Menu_Item*> elements;
		Fl_Menu_Item* fl;
		for (int i = 0; i < menu->Size(); i++) {
			//First level
			//each element is a vector
			ke = menu->ValueOnIndex(i);
			if (ke->aVectorContainer() == false)
				return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong element, vector expected");
			KifElement* v = ke;
			//then we analyse the sub-elements, the next element should be a vector
			if (v->Size() != 3)
				return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong element, missing field in a sub-menu");

			string nn = v->ValueOnIndex(0)->String();
			char* n = strdup(STR(nn));
			//second element shoudl be a Callback
			KifElement* function = v->ValueOnIndex(1);
			if (function->type != kifCall && !function->isFunction())
				return Returningerror(kifcode, _thread, idcallthread, "WND(109): Wrong element, third field in a sub-menu should be a function");
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
			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(100): Cannot recreate an existing browser");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): Browser widget has not been created yet");

		string v = callfunc->Evaluatethread(0, domain, _thread)->String();
		browser->add(STR(v));
		return kifTRUE;
	}

	KifElement* MethodLoad(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): Browser widget has not been created yet");

		string v = callfunc->Evaluatethread(0, domain, _thread)->String();
		browser->load(STR(v));
		return kifTRUE;
	}

	KifElement* MethodInsert(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): Browser widget has not been created yet");

		int i = callfunc->Evaluatethread(0, domain, _thread)->Integer();
		string v = callfunc->Evaluatethread(1, domain, _thread)->String();
		browser->insert(i, STR(v), 0);
		return kifTRUE;
	}

	KifElement* MethodSelect(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): Browser widget has not been created yet");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): Browser widget has not been created yet");
		browser->clear();
		return kifTRUE;
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): Browser widget has not been created yet");

		string v;
		int i = browser->value();
		return kifcode->Provideinteger(i);
	}

	KifElement* MethodDeselect(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (browser == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): Browser widget has not been created yet");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): Browser widget has not been created yet");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): Browser widget has not been created yet");
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

			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			kifcode->Returnerror("WND(094): You can only instanciate a wtreeitem with another wtreeitem");

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
			return kifcode->Returnerror("WND(117): Unknown tree item");

		return kifcode->Provideinteger(etree->depth());
	}

	KifElement* MethodLabel(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");

		return kifcode->Providestringraw(etree->label());
	}

	KifElement* MethodValue(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");

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
			return kifcode->Returnerror("WND(117): Unknown tree item");

		void* val = etree->user_data();
		if (val != NULL)
			((KifElement*)val)->Resetreference();
		etree->user_data(NULL);
		return kifTRUE;
	}

	KifElement* MethodIsclose(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");
		int i = etree->is_close();
		return kifcode->Provideinteger(i);
	}

	KifElement* MethodIsopen(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");
		int i = etree->is_open();
		return kifcode->Provideinteger(i);
	}

	KifElement* MethodIsactive(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");
		int i = etree->is_active();
		return kifcode->Provideinteger(i);
	}

	KifElement* MethodIsroot(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");
		int i = etree->is_root();
		return kifcode->Provideinteger(i);
	}

	KifElement* MethodIsselected(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");
		int i = etree->is_selected();
		return kifcode->Provideinteger(i);
	}

	KifElement* MethodItemBGColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {

		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(etree->labelbgcolor());

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return kifcode->Returnerror("WND(117): Unknown tree item");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(etree->labelfgcolor());

		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return kifcode->Returnerror("WND(117): Unknown tree item");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(etree->labelfont());

		int ft;
		KifElement* f = callfunc->Evaluatethread(0, domain, _thread);
		if (f->type == kifString) {
			string buf = f->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = "WND(657): Unknown font code";
				return kifcode->Returnerror(buf);
			}
			ft = fonts[buf];
		}
		else {
			ft = f->Integer();
			if (ft >= fontmax)
				return kifcode->Returnerror("WND(658): unknown font");
		}

		etree->labelfont(ft);
		return kifTRUE;
	}

	KifElement* MethodItemSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");

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
			return kifcode->Returnerror("WND(117): Unknown tree item");
		etree->activate();
		return kifTRUE;
	}

	KifElement* MethodDeactivate(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");
		etree->deactivate();
		return kifTRUE;
	}

	KifElement* MethodNext(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");
		return new Kifetree(kifcode, NULL, etree->next());
	}

	KifElement* MethodPrevious(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");
		return new Kifetree(kifcode, NULL, etree->prev());
	}

	KifElement* MethodParent(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");
		return new Kifetree(kifcode, NULL, etree->parent());
	}

	KifElement* MethodChild(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");
		int i = callfunc->Evaluatethread(0, domain, _thread)->Integer();
		if (i > etree->children())
			return kifcode->Returnerror("WND(118): Wrong child index");
		return new Kifetree(kifcode, NULL, etree->child(i));
	}

	KifElement* MethodChildren(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (etree == NULL)
			return kifcode->Returnerror("WND(117): Unknown tree item");
		return kifcode->Provideinteger(etree->children());
	}


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)

			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(210): Cannot recreate an existing wcounter");

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
				return kifcode->Returnerror("KIF(881): Error, wrong function arguments");
			Keeptrack(domain, contextualpattern);
			counter->callback(counter_callback, this);
		}
		return kifTRUE;
	}

	
	KifElement* MethodStep(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(211): No wcounter available");

		double kstep = callfunc->Evaluatethread(0, domain, _thread)->Float();
		counter->step(kstep);
		return kifTRUE;
	}

	KifElement* MethodLargeStep(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(211): No wcounter available");

		double kstep = callfunc->Evaluatethread(0, domain, _thread)->Float();
		counter->lstep(kstep);
		return kifTRUE;
	}

	KifElement* MethodSteps(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(211): No wcounter available");

		double nstep = callfunc->Evaluatethread(0, domain, _thread)->Float();
		double Lstep = callfunc->Evaluatethread(1, domain, _thread)->Float();
		counter->step(nstep, Lstep);
		return kifTRUE;
	}

	KifElement* MethodType(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(211): No wcounter available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(211): No wcounter available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(211): No wcounter available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(211): No wcounter available");

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
				buf = "WND(657): Unknown font code";
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}

			counter->textfont(fonts[buf]);
		}
		else {
			int font = ke->Integer();
			if (font >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, "WND(658): unknown font");
			counter->textfont(font);
		}
		return kifTRUE;
	}

	KifElement* MethodFontSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (counter == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(211): No wcounter available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(211): No wcounter available");

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
				buf = "WND(657): Unknown color code";
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

			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(220): Cannot recreate an existing wprogress bar");

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
				return kifcode->Returnerror("KIF(881): Error, wrong function arguments");
			Keeptrack(domain, contextualpattern);
			progress->callback(progress_callback, this);
		}
		return kifTRUE;
	}

	KifElement* MethodMinimum(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (progress == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(221): No wprogress bar available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(221): No wprogress bar available");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(221): No wprogress bar available");

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

			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(100): Cannot recreate an existing wtree");

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
				return kifcode->Returnerror("KIF(881): Error, wrong function arguments");
			Keeptrack(domain, contextualpattern);
			wtree->callback(wtree_callback, this);
		}
		return kifTRUE;
	}

	KifElement* MethodClear(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");
		wtree->clear();
		return kifTRUE;
	}

	KifElement* MethodAdd(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(125): Empty label, cannot create wtree widget");
		return new Kifetree(kifcode, NULL, fti);
	}

	KifElement* MethodIsclose(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

		KifElement* e = callfunc->Evaluatethread(0, domain, _thread);

		string v;
		if (e->type != etree_type)
			return Returningerror(kifcode, _thread, idcallthread, "WND(341): wrong type for the first element");
		string name = callfunc->Evaluatethread(1, domain, _thread)->String();
		int pos = callfunc->Evaluatethread(2, domain, _thread)->Integer();
		wtree->insert(((Kifetree*)e)->etree, STR(name), pos);
		return kifTRUE;
	}

	KifElement* MethodItemBGColor(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wtree->item_labelbgcolor());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wtree->item_labelfgcolor());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wtree->item_labelfont());

		threadtosetvalue(contextualpattern, domain, callfunc);
		int ft;
		KifElement* f = callfunc->Evaluatethread(0, domain, _thread);
		if (f->type == kifString) {
			string buf = f->String();
			if (fonts.find(buf) == fonts.end()) {
				buf = "WND(657): Unknown font code";
				return Returningerror(kifcode, _thread, idcallthread, buf);
			}
			ft = fonts[buf];
		}
		else {
			ft = f->Integer();
			if (ft >= fontmax)
				return Returningerror(kifcode, _thread, idcallthread, "WND(658): unknown font");
		}

		wtree->item_labelfont(ft);
		return kifTRUE;
	}

	KifElement* MethodItemSize(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		minthread;

		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

		if (callfunc->Size() == 0)
			return kifcode->Provideinteger(wtree->connectorcolor());

		threadtosetvalue(contextualpattern, domain, callfunc);
		KifElement* ke = callfunc->Evaluatethread(0, domain, _thread);

		Fl_Color color;
		if (ke->type == kifString) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

		KifElement* e = callfunc->Evaluatethread(0, domain, _thread);

		string v;
		if (e->type != etree_type)
			return Returningerror(kifcode, _thread, idcallthread, "WND(341): wrong type for the first element");
		string name = callfunc->Evaluatethread(1, domain, _thread)->String();
		wtree->insert_above(((Kifetree*)e)->etree, STR(name));
		return kifTRUE;
	}

	KifElement* MethodRemove(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

		KifElement* e = callfunc->Evaluatethread(0, domain, _thread);

		string v;
		if (e->type != etree_type)
			return Returningerror(kifcode, _thread, idcallthread, "WND(341): wrong type for the first element");
		Kifetree* ie = (Kifetree*)e;
		if (ie->etree == NULL)
			return kifFALSE;
		wtree->remove(ie->etree);
		return kifTRUE;
	}


	KifElement* MethodTreeClear(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");
		wtree->clear();
		return kifTRUE;
	}

	KifElement* MethodClicked(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");
		Fl_Tree_Item* e = wtree->item_clicked();
		return new Kifetree(kifcode, NULL, e);
	}

	KifElement* MethodFirst(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");
		Fl_Tree_Item* e = wtree->first();
		return new Kifetree(kifcode, NULL, e);
	}

	KifElement* MethodRoot(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");
		Fl_Tree_Item* e = wtree->root();
		return new Kifetree(kifcode, NULL, e);
	}

	KifElement* MethodRootLabel(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

		string name = callfunc->Evaluatethread(0, domain, _thread)->String();
		wtree->root_label(STR(name));
		return kifTRUE;
	}

	KifElement* MethodFind(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");

		string path = callfunc->Evaluatethread(0, domain, _thread)->String();
		Fl_Tree_Item* e = wtree->find_item(STR(path));
		return new Kifetree(kifcode, NULL, e);
	}

	KifElement* MethodLast(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");
		Fl_Tree_Item* e = wtree->last();
		return new Kifetree(kifcode, NULL, e);
	}


	KifElement* MethodNext(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");
		Fl_Tree_Item* e;
		if (callfunc->Size() == 0)
			e = wtree->next();
		else {
			KifElement* etr = callfunc->Evaluatethread(0, domain, _thread);
			if (etr->type != etree_type)
				return Returningerror(kifcode, _thread, idcallthread, "WND(341): wrong type for the first element");
			e = wtree->next(((Kifetree*)etr)->etree);
		}
		return new Kifetree(kifcode, NULL, e);
	}

	KifElement* MethodPrevious(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		inthread(contextualpattern, domain, callfunc);
		if (wtree == NULL)
			return Returningerror(kifcode, _thread, idcallthread, "WND(121): wtree widget has not been created yet");
		Fl_Tree_Item* e;
		if (callfunc->Size() == 0)
			e = wtree->prev();
		else {
			KifElement* etr = callfunc->Evaluatethread(0, domain, _thread);
			if (etr->type != etree_type)
				return Returningerror(kifcode, _thread, idcallthread, "WND(341): wrong type for the first element");
			e = wtree->prev(((Kifetree*)etr)->etree);
		}
		return new Kifetree(kifcode, NULL, e);
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)

			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			return Returningerror(kifcode, _thread, idcallthread, "WND(100): Cannot recreate an existing filebrowser");


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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): File browser not initialized");

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
			return Returningerror(kifcode, _thread, idcallthread, "WND(101): File browser not initialized");

		if (filebrowser->shown())
			return kifFALSE;
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		KiFArityError(name)

			//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
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
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}
		kifcode->KifInitLib();

		///The new type will be: window. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifwindowElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifwindow)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifwindowElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("window", CreatekifwindowElement);
		if (!create_variables(kifcode))
			return false;
		window_type = res;

		res = KifAddNewType("box", CreatekifboxElement);
		if (res == kifVOID)
			return false;
		box_type = res;

		res = KifAddNewType("wtabs", CreatekiftabsElement);
		if (res == kifVOID)
			return false;
		tabs_type = res;

		res = KifAddNewType("wgroup", CreatekifgroupElement);
		if (res == kifVOID)
			return false;
		group_type = res;

		res = KifAddNewType("wchoice", CreatekifchoiceElement);
		if (res == kifVOID)
			return false;
		choice_type = res;

		res = KifAddNewType("button", CreatekifbuttonElement);
		if (res == kifVOID)
			return false;
		button_type = res;

		res = KifAddNewType("editor", CreatekifeditorElement);
		if (res == kifVOID)
			return false;
		editor_type = res;

		res = KifAddNewType("winput", CreatekifinputElement);
		if (res == kifVOID)
			return false;
		input_type = res;

		res = KifAddNewType("wtable", CreatekiftableElement);
		if (res == kifVOID)
			return false;
		table_type = res;

		res = KifAddNewType("woutput", CreatekifoutputElement);
		if (res == kifVOID)
			return false;
		output_type = res;

		res = KifAddNewType("scroll", CreatekifscrollElement);
		if (res == kifVOID)
			return false;
		scroll_type = res;

		res = KifAddNewType("slider", CreatekifsliderElement);
		if (res == kifVOID)
			return false;
		slider_type = res;

		res = KifAddNewType("wcounter", CreatekifcounterElement);
		if (res == kifVOID)
			return false;
		counter_type = res;

		res = KifAddNewType("wprogress", CreatekifprogressElement);
		if (res == kifVOID)
			return false;
		progress_type = res;

		res = KifAddNewType("image", CreatekifimageElement);
		if (res == kifVOID)
			return false;
		image_type = res;

		res = KifAddNewType("bitmap", CreatekifbitmapElement);
		if (res == kifVOID)
			return false;
		bitmap_type = res;

		res = KifAddNewType("filebrowser", CreatekiffilebrowserElement);
		if (res == kifVOID)
			return false;
		filebrowser_type = res;

		res = KifAddNewType("browser", CreatekifbrowserElement);
		if (res == kifVOID)
			return false;
		browser_type = res;

		res = KifAddNewType("wtreeitem", CreatekifetreeElement);
		if (res == kifVOID)
			return false;
		etree_type = res;

		res = KifAddNewType("wtree", CreatekifwtreeElement);
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
		MethodInitializationWindow("create", &Kifwindow::MethodCreate, P_THREE | P_FIVE, "create(int x,int y,int w, int h,string title): Create a window without widgets, w and h are optional");
		MethodInitializationWindow("begin", &Kifwindow::MethodBegin, P_THREE | P_FIVE, "begin(int x,int y,int w, int h,string title): Create a window and begin initialisation, w and h are optional");
		MethodInitializationWindow("end", &Kifwindow::MethodEnd, P_NONE, "end(): end creation");
		MethodInitializationWindow("run", &Kifwindow::MethodRun, P_NONE, "run(): Launch the GUI");
		MethodInitializationWindow("close", &Kifwindow::MethodClose, P_NONE, "close(): close the window");
		MethodInitializationWindow("onclose", &Kifwindow::MethodOnClose, P_TWO, "onclose(function,object): Callback to call on close");
		MethodInitializationWindow("ontime", &Kifwindow::MethodOntime, P_THREE, "ontime(function,t,object): Set a time out function");

		MethodInitializationWindow("initializefonts", &Kifwindow::MethodSetFonts, P_NONE, "initializefonts(): load fonts from system. Return the number of available fonts");
		MethodInitializationWindow("getfont", &Kifwindow::MethodGetFont, P_ONE, "getfont(int num): get font name.");
		MethodInitializationWindow("getfontsizes", &Kifwindow::MethodGetFontSizes, P_ONE, "getfontsizes(int num): return a vector of available font sizes.");
		MethodInitializationWindow("fontnumber", &Kifwindow::MethodNbFonts, P_NONE, "fontnumber(): Return the number of available fonts.");
		MethodInitializationWindow("plot", &Kifwindow::MethodPlot, P_TWO | P_THREE, "plot(fvector xy,int thickness,fvector landmarks): Plot a graph from a table of successive x,y points according to window size. If thickness===0 then points are continuously plotted, else defines the diameter of the point. Return a float vector which is used with plotcoords. The landmark vector is optional, it is has the following structure: [XmaxWindow,YmaxWindow,XminValue,YminValue,XmaxValue,YmaxValue,incX,incY]. incX,incY are also optional.");
		MethodInitializationWindow("plotcoords", &Kifwindow::PlotCoords, P_THREE, "plotcoords(fvector inits,float x,float y): Compute the coordinates of a point(x,y) according to the previous scale computed with plot. Returns a vector of two elements [xs,ys] corresponding to the screen coordinates in the current window.");

		MethodInitializationWindow("ask", &Kifwindow::MethodAsk, P_THREE | P_FOUR | P_FIVE, "ask(string msg,string buttonmsg1,string buttonmsg2,string buttonmsg3): Pop up window to pose a question");
		MethodInitializationWindow("alert", &Kifwindow::MethodAlert, P_ONE, "alert(string msg): Pop up window to display an alert");
		MethodInitializationWindow("password", &Kifwindow::MethodPassword, P_ONE, "password(string msg): Pop up window to input a password");
		MethodInitializationWindow("get", &Kifwindow::MethodInput, P_NONE | P_ONE, "get(string msg): Pop up window to get a value from the user");
		MethodInitializationWindow("menu", &Kifwindow::MethodMenu, P_FIVE, "menu(vector,int x,int y,int w, int h): initialize a menu with its callback functions");
		MethodInitializationWindow("resizable", &Kifwindow::MethodResizable, P_ONE | P_NONE, "resizable(object): make the object resizable");
		MethodInitializationWindow("sizerange", &Kifwindow::MethodSizerange, P_FOUR, "sizerange(int minw,int minh, int maxw,int maxh): define range in which the size of the window can evolve");
		MethodInitializationWindow("modal", &Kifwindow::MethodModal, P_ONE | P_NONE, "modal(bool b): If true make the window modal. If no parameter return if the window is modal");
		MethodInitializationWindow("border", &Kifwindow::MethodBorder, P_ONE | P_NONE, "border(bool b): If true add or remove borders. If no parameter return if the window has borders");
		MethodInitializationWindow("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");

		MethodInitializationWindow("drawcolor", &Kifcomplexwidget::MethodDrawColor, P_ONE, "drawcolor(int c|string c): set the color for the next drawings");
		MethodInitializationWindow("rectangle", &Kifcomplexwidget::MethodRectangle, P_FOUR | P_FIVE, "rectangle(int x,int y,int w, int h, string c|int c): Draw a rectangle with optional color c");
		MethodInitializationWindow("vertex", &Kifcomplexwidget::MethodVertex, P_TWO, "vertex(float x,float y): Add a vertex to a complex structure");
		MethodInitializationWindow("rectanglefill", &Kifcomplexwidget::MethodRectangleFill, P_FOUR | P_FIVE, "rectanglefill(int x,int y,int w, int h, string c|int c): Fill a rectangle with optional color c");
		MethodInitializationWindow("lineshape", &Kifcomplexwidget::MethodLineShape, P_TWO, "lineshape(string type,int width): Select the line shape and its thikness");
		MethodInitializationWindow("polygon", &Kifcomplexwidget::MethodPolygon, P_SIX | P_EIGHT, "polygon(int x,int y,int x1, int y1,int x2, int y2, int x3, int y3): Draw a polygon, x3 and y3 are optional");
		MethodInitializationWindow("loop", &Kifcomplexwidget::MethodLoop, P_SIX | P_EIGHT, "loop(int x,int y,int x1, int y1,int x2, int y2, int x3, int y3): Draw a series of lines, x3 and y3 are optional");
		MethodInitializationWindow("line", &Kifcomplexwidget::MethodLine, P_FOUR | P_SIX, "line(int x,int y,int x1, int y1,int x2, int y2): Draw a line between points, x2 and y2 are optional");
		MethodInitializationWindow("rotation", &Kifcomplexwidget::MethodLineRotation, P_FOUR | P_FIVE, "rotation(float x,float y,float distance, float angle, bool draw): Compute the coordinate of a rotated point from point x,y, using a distance and an angle. Return a vector of floats with the new coordinates.");

		MethodInitializationWindow("pushmatrix", &Kifcomplexwidget::MethodPushMatrix, P_NONE, "pushmatrix(): Save the current transformation");
		MethodInitializationWindow("popmatrix", &Kifcomplexwidget::MethodPopMatrix, P_NONE, "popmatrix(): Restore the current transformation");
		MethodInitializationWindow("scale", &Kifcomplexwidget::MethodScale, P_TWO | P_ONE, "scale(float x,float y)|scale(float x): Scale the current transformation");
		MethodInitializationWindow("translate", &Kifcomplexwidget::MethodTranslate, P_TWO, "translate(float x,float y): translate the current transformation");
		MethodInitializationWindow("rotate", &Kifcomplexwidget::MethodRotate, P_ONE, "rotate(float d): rotate of degree d the current transformation");
		MethodInitializationWindow("multmatrix", &Kifcomplexwidget::MethodMultmatrix, P_SIX, "multmatrix(float a, float b, float c, float d, float x, float y): combine transformations");

		MethodInitializationWindow("transformx", &Kifcomplexwidget::MethodTransform_x, P_TWO, "transformx(float x, float y): Transform a coordinate X using the current transformation matrix.");
		MethodInitializationWindow("transformy", &Kifcomplexwidget::MethodTransform_y, P_TWO, "transformy(float x, float y): Transform a coordinate Y using the current transformation matrix.");
		MethodInitializationWindow("transformdx", &Kifcomplexwidget::MethodTransform_dx, P_TWO, "transformdx(float x, float y): Transform a distance DX using the current transformation matrix.");
		MethodInitializationWindow("transformdy", &Kifcomplexwidget::MethodTransform_dy, P_TWO, "transformdy(float x, float y): Transform a distance DY using the current transformation matrix.");
		MethodInitializationWindow("transformedvertex", &Kifcomplexwidget::MethodTransform_vertex, P_TWO, "transformedvertex(float x, float y): add transformations to vertices list.");

		MethodInitializationWindow("beginpoints", &Kifcomplexwidget::MethodBeginPoints, P_NONE, "beginpoints(): Start accumulating vertices");
		MethodInitializationWindow("endpoints", &Kifcomplexwidget::MethodEndPoints, P_NONE, "endpoints(): Stop accumulating vertices");

		MethodInitializationWindow("beginline", &Kifcomplexwidget::MethodBeginLine, P_NONE, "beginline(): Start drawing lines.");
		MethodInitializationWindow("endline", &Kifcomplexwidget::MethodEndLine, P_NONE, "endline(): End drawing lines.");

		MethodInitializationWindow("beginloop", &Kifcomplexwidget::MethodBeginLoop, P_NONE, "beginloop(): Start drawing a closed sequence of lines.");
		MethodInitializationWindow("endloop", &Kifcomplexwidget::MethodEndLoop, P_NONE, "endloop(): End drawing a closed sequence of lines.");

		MethodInitializationWindow("beginpolygon", &Kifcomplexwidget::MethodBeginPolygon, P_NONE, "beginpolygon(): Start drawing a convex filled polygon.");
		MethodInitializationWindow("endpolygon", &Kifcomplexwidget::MethodEndPolygon, P_NONE, "endpolygon(): End drawing a convex filled polygon.");

		MethodInitializationWindow("begincomplexpolygon", &Kifcomplexwidget::MethodBeginComplexPolygon, P_NONE, "begincomplexpolygon(): Start drawing a complex filled polygon.");
		MethodInitializationWindow("gap", &Kifcomplexwidget::MethodGap, P_NONE, "gap(): Fill the gap while drawing a complex polygon.");
		MethodInitializationWindow("endcomplexpolygon", &Kifcomplexwidget::MethodEndComplexPolygon, P_NONE, "endcomplexpolygon(): End drawing a complex filled polygon.");


		MethodInitializationWindow("arc", &Kifcomplexwidget::MethodArc, P_FIVE | P_SIX, "arc(int x,int y,int x1, int y1, float a1, float a2): Draw an arc.\rarc(float x,float y,float rad,float a1,float a2): Add a series of points to the current path on the arc of a circle;");
		MethodInitializationWindow("curve", &Kifcomplexwidget::MethodCurve, P_EIGHT, "curve(float x,float y,float x1,float y1,float x1,float y2,float x3,float y3): Add a series of points on a Bezier curve to the path. The curve ends (and two of the points) are at x,y and x3,y3.");
		MethodInitializationWindow("pushclip", &Kifcomplexwidget::MethodPushClip, P_FOUR, "pushclip(int x,int y,int w, int h): Insert a clip region, with the following coordinates");
		MethodInitializationWindow("popclip", &Kifcomplexwidget::MethodPopClip, P_NONE, "popclip(): Release a clip region");

		MethodInitializationWindow("pie", &Kifcomplexwidget::MethodPie, P_SIX, "pie(int x,int y,int x1, int y1, float a1, float a2): Draw a pie");
		MethodInitializationWindow("point", &Kifcomplexwidget::MethodPoint, P_TWO, "point(int x,int y): Draw a pixel");
		MethodInitializationWindow("circle", &Kifcomplexwidget::MethodCircle, P_THREE | P_FOUR, "circle(int x,int y,int r,int color): Draw a circle. 'color' is optional.");

		MethodInitializationWindow("textsize", &Kifcomplexwidget::MethodTextMeasure, P_ONE, "textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels");
		MethodInitializationWindow("drawtext", &Kifcomplexwidget::MethodDrawText, P_THREE, "drawtext(string l,int x,int y): Put a text at position x,y");
		MethodInitializationWindow("redraw", &KifWidget::MethodRedraw, P_NONE | P_ONE, "redraw(): Redraw the window");
		MethodInitializationWindow("rgbcolor", &Kifcomplexwidget::MethodRGBColor, P_ONE | P_THREE, "rgbcolor(string|int color)|(int r,int g,int b)|(vector color): either return a vector of the color decomposition or return the value of rgb values combined");

		MethodInitializationWindow("font", &Kifcomplexwidget::MethodTextFont, P_TWO, "font(string f,int sz): Set the font name and its size");
		MethodInitializationWindow("ondragdrop", &Kifwindow::MethodOnDragDrop, P_TWO, "ondragdrop(function,object): Set the call back function for drag and drop action.");
		MethodInitializationWindow("onmouse", &Kifwindow::MethodOnMouse, P_THREE, "onmouse(int action, function,object): Set the call back function on a mouse action with a given object as parameter");
		MethodInitializationWindow("onkey", &Kifwindow::MethodOnKey, P_THREE, "onkey(int action, function,object): Set the call back function on a keyboard action with a given object as parameter");
		MethodInitializationWindow("cursorstyle", &Kifwindow::MethodCursorStyle, P_THREE, "cursorstyle(int cursortype, int color,int color): Set the cursor shape");
		MethodInitializationWindow("hide", &Kifwindow::MethodWindowHide, P_ONE, "hide(bool h): Hide the window if h is true");
		MethodInitializationWindow("show", &KifWidget::MethodShow, P_NONE, "show(): Show the window");
		MethodInitializationWindow("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationWindow("flush", &Kifwindow::MethodFlush, P_NONE, "flush(): force redrawing of window");
		MethodInitializationWindow("position", &Kifwindow::MethodPosition, P_NONE | P_TWO, "position()|(int x,int y): Return the window position or set the window position");
		MethodInitializationWindow("size", &Kifwindow::MethodSize, P_NONE | P_FOUR, "size()|(int x,int y,int w, int h): Return the window size or set the window size");
		MethodInitializationWindow("awake", &Kifwindow::MethodAwake, P_NONE, "awake(): Awake a threaded window");
		MethodInitializationWindow("lock", &Kifwindow::MethodLock, P_NONE, "lock(): Thread lock");
		MethodInitializationWindow("unlock", &Kifwindow::MethodUnlock, P_NONE, "unlock(): Thread unlock");
		MethodInitializationWindow("image", &Kifwindow::MethodImage, P_FIVE, "image(image image,int x, int y, int w, int h): Display an image");
		MethodInitializationWindow("bitmap", &Kifwindow::MethodBitmap, P_FOUR | P_SIX, "bitmap(bitmap image,int color,int x, int y, int w, int h): Display a bitmap");
		MethodInitializationWindow("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationWindow("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationWindow("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationWindow("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationWindow("label", &Kifwindow::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationWindow("selectioncolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
		MethodInitializationWindow("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationWindow("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
		MethodInitializationWindow("copy", &KifWidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
		MethodInitializationWindow("paste", &KifWidget::MethodPaste, P_NONE, "paste(): paste string");
		MethodInitializationWindow("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the label alignment");

		MethodInitializationScroll("create", &Kifscroll::MethodCreate, P_FIVE, "create(int x,int y,int w,int h,string label): Create a scroll");
		MethodInitializationScroll("resize", &Kifscroll::MethodResize, P_FOUR, "resize(object): make the object resizable");
		MethodInitializationScroll("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationScroll("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationScroll("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationScroll("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationScroll("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationScroll("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationScroll("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationScroll("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationScroll("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationScroll("selectioncolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
		MethodInitializationScroll("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationScroll("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationScroll("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationScroll("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
		MethodInitializationScroll("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the label alignment");

		MethodInitializationCounter("create", &Kifcounter::MethodCreate, P_FIVE, "create(int x,int y,int w,int h,string label): Create a wcounter");
		MethodInitializationCounter("bounds", &Kifcounter::MethodBoundary, P_TWO, "bounds(float x,float y): defines the wcounter boundary");
		MethodInitializationCounter("type", &Kifcounter::MethodType, P_ONE, "type(bool normal): if 'true' then normal wcounter or simple wcounter");
		MethodInitializationCounter("step", &Kifcounter::MethodStep, P_ONE, "step(double): define the wcounter step");
		MethodInitializationCounter("lstep", &Kifcounter::MethodLargeStep, P_ONE, "lstep(double): define the large wcounter step");
		MethodInitializationCounter("steps", &Kifcounter::MethodSteps, P_TWO, "steps(double): define the wcounter steps, normal and large.");
		MethodInitializationCounter("value", &Kifcounter::MethodValue, P_ONE | P_NONE, "value(float): define the value for the wcounter or return its value");
		MethodInitializationCounter("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationCounter("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationCounter("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationCounter("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationCounter("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationCounter("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationCounter("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationCounter("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationCounter("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationCounter("selectioncolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
		MethodInitializationCounter("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationCounter("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationCounter("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationCounter("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
		MethodInitializationCounter("textsize", &Kifcounter::MethodFontSize, P_ONE | P_NONE, "textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels");
		MethodInitializationCounter("font", &Kifcounter::MethodFont, P_ONE | P_NONE, "font(int s): set or return the text font");
		MethodInitializationCounter("textcolor", &Kifcounter::MethodTextColor, P_ONE | P_NONE, "textcolor(string code|int code): Set the color of the text");

		MethodInitializationProgress("create", &Kifprogress::MethodCreate, P_FIVE, "create(int x,int y,int w,int h,string label): Create a progress bar");
		MethodInitializationProgress("minimum", &Kifprogress::MethodMinimum, P_ONE | P_NONE, "minimum(float x): defines the minimum or return the minimum");
		MethodInitializationProgress("maximum", &Kifprogress::MethodMaximum, P_ONE | P_NONE, "maximum(float x): defines the maximum or return the maximum");
		MethodInitializationProgress("value", &Kifprogress::MethodValue, P_ONE | P_NONE, "value(float): define the value for the progress bar or return its value");
		MethodInitializationProgress("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationProgress("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationProgress("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationProgress("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationProgress("barcolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "barcolor(int color): Color for the progression bar");
		MethodInitializationProgress("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationProgress("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationProgress("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationProgress("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");

		MethodInitializationSlider("create", &Kifslider::MethodCreate, P_SEVEN, "create(int x,int y,int w,int h,int alignment,bool valueslider,string label): Create a slider or a valueslider");
		MethodInitializationSlider("resize", &Kifslider::MethodResize, P_FOUR, "resize(object): make the object resizable");
		MethodInitializationSlider("bounds", &Kifslider::MethodBoundary, P_TWO, "bounds(int x,int y): defines the slider boundary");
		MethodInitializationSlider("type", &Kifslider::MethodType, P_ONE, "type(int x): Value slider type");
		MethodInitializationSlider("boxtype", &Kifslider::MethodBoxType, P_ONE, "boxtype(int x): Define the slider box type");
		MethodInitializationSlider("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the slider alignement");
		MethodInitializationSlider("step", &Kifslider::MethodStep, P_ONE, "step(int): define the slider step");
		MethodInitializationSlider("value", &Kifslider::MethodValue, P_ONE | P_NONE, "value(int): define the value for the slider or return its value");
		MethodInitializationSlider("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationSlider("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationSlider("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationSlider("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationSlider("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationSlider("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationSlider("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationSlider("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationSlider("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationSlider("selectioncolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
		MethodInitializationSlider("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationSlider("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationSlider("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationSlider("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");

		MethodInitializationBox("create", &Kifbox::MethodCreate, P_FIVE, "create(int x,int y,int w,int h,string label): Create a box");
		MethodInitializationBox("type", &Kifbox::MethodType, P_ONE, "type(string|int boxtype): Define the box type");
		MethodInitializationBox("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationBox("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationBox("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationBox("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationBox("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationBox("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationBox("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationBox("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationBox("selectioncolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
		MethodInitializationBox("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationBox("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationBox("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationBox("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
		MethodInitializationBox("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the label alignment");

		MethodInitializationButton("create", &Kifbutton::MethodCreate, P_SEVEN | P_SIX | P_FIVE, "create(int x,int y,int w,int h,string type,string shape,string label): Create a button. If type is not provided then it is: FL_Regular.");
		MethodInitializationButton("when", &Kifbutton::MethodWhen, 65534, "when(string when1, string when2,...): Type of event for a button which triggers the callback");
		MethodInitializationButton("shortcut", &Kifbutton::MethodShortcut, P_ONE, "shortcut(string keycode): Set a shortcut to activate the button from the keyboard");
		MethodInitializationButton("color", &Kifbutton::MethodColor, P_ONE, "color(string code|int code): Set the color of the button");
		MethodInitializationButton("value", &Kifbutton::MethodValue, P_ONE | P_NONE, "value(): return the value of the current button");
		MethodInitializationButton("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the button label alignment");
		MethodInitializationButton("image", &Kifbutton::MethodImage, P_ONE | P_TWO | P_THREE, "image(image im,string label,int labelalign): Use the image as a button image");
		MethodInitializationButton("bitmap", &Kifbutton::MethodBitmap, P_TWO | P_THREE | P_FOUR, "bitmap(bitmap im,int color,string label,int labelalign): Use the bitmap as a button image");
		MethodInitializationButton("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationButton("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationButton("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationButton("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationButton("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationButton("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationButton("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationButton("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationButton("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationButton("selectioncolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
		MethodInitializationButton("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationButton("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationButton("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationButton("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");

		MethodInitializationtabs("begin", &Kiftabs::MethodBegin, P_FIVE, "begin(int x,int y,int w, int h,string title): Create a tab window and begin initialisation");
		MethodInitializationtabs("end", &Kiftabs::MethodEnd, P_NONE, "end(): end tab creation");
		MethodInitializationtabs("current", &Kiftabs::MethodCurrent, P_NONE | P_ONE, "current(): Return the current active tab");
		MethodInitializationtabs("add", &Kiftabs::MethodAdd, P_ONE, "add(wgroup g): Add dynamically a new tab");
		MethodInitializationtabs("remove", &Kiftabs::MethodRemove, P_ONE, "remove(wgroup g): Remove a tab");
		MethodInitializationtabs("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationtabs("selectioncolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
		MethodInitializationtabs("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationtabs("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationtabs("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationtabs("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationtabs("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationtabs("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationtabs("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationtabs("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationtabs("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationtabs("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationtabs("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationtabs("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
		MethodInitializationtabs("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the label alignment");

		MethodInitializationgroup("begin", &Kifgroup::MethodBegin, P_FIVE, "begin(int x,int y,int w, int h,string title): Create a group within a tab and begin initialisation");
		MethodInitializationgroup("end", &Kifgroup::MethodEnd, P_NONE, "end(): end group creation");
		MethodInitializationgroup("selectioncolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
		MethodInitializationgroup("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationgroup("activate", &Kifgroup::MethodActivate, P_NONE, "activate(): Active this tab");
		MethodInitializationgroup("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationgroup("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationgroup("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationgroup("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationgroup("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationgroup("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationgroup("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationgroup("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationgroup("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationgroup("drawcolor", &Kifcomplexwidget::MethodDrawColor, P_ONE, "drawcolor(int c|string c): set the color for the next drawings");
		MethodInitializationgroup("rectangle", &Kifcomplexwidget::MethodRectangle, P_FOUR | P_FIVE, "rectangle(int x,int y,int w, int h, string c|int c): Draw a rectangle with optional color c");
		MethodInitializationgroup("rectanglefill", &Kifcomplexwidget::MethodRectangleFill, P_FOUR | P_FIVE, "rectanglefill(int x,int y,int w, int h, string c|int c): Fill a rectangle with optional color c");
		MethodInitializationgroup("lineshape", &Kifcomplexwidget::MethodLineShape, P_TWO, "lineshape(string type,int width): Select the line shape and its thikness");
		MethodInitializationgroup("polygon", &Kifcomplexwidget::MethodPolygon, P_SIX | P_EIGHT, "polygon(int x,int y,int x1, int y1,int x2, int y2, int x3, int y3): Draw a polygon, x3 and y3 are optional");
		MethodInitializationgroup("line", &Kifcomplexwidget::MethodLine, P_FOUR | P_SIX, "line(int x,int y,int x1, int y1,int x2, int y2): Draw a line between points, x2 and y2 are optional");
		MethodInitializationgroup("rotation", &Kifcomplexwidget::MethodLineRotation, P_FOUR | P_FIVE, "rotation(float x,float y,float distance, float angle, bool draw): Compute the coordinate of a rotated point from point x,y, using a distance and an angle. Return a vector of floats with the new coordinates.");

		MethodInitializationgroup("pushmatrix", &Kifcomplexwidget::MethodPushMatrix, P_NONE, "pushmatrix(): Save the current transformation");
		MethodInitializationgroup("popmatrix", &Kifcomplexwidget::MethodPopMatrix, P_NONE, "popmatrix(): Restore the current transformation");
		MethodInitializationgroup("scale", &Kifcomplexwidget::MethodScale, P_TWO | P_ONE, "scale(float x,float y)|scale(float x): Scale the current transformation");
		MethodInitializationgroup("translate", &Kifcomplexwidget::MethodTranslate, P_TWO, "translate(float x,float y): translate the current transformation");
		MethodInitializationgroup("rotate", &Kifcomplexwidget::MethodRotate, P_ONE, "rotate(float d): rotate of degree d the current transformation");
		MethodInitializationgroup("multmatrix", &Kifcomplexwidget::MethodMultmatrix, P_SIX, "multmatrix(float a, float b, float c, float d, float x, float y): combine transformations");

		MethodInitializationgroup("transformx", &Kifcomplexwidget::MethodTransform_x, P_TWO, "transformx(float x, float y): Transform a coordinate X using the current transformation matrix.");
		MethodInitializationgroup("transformy", &Kifcomplexwidget::MethodTransform_y, P_TWO, "transformy(float x, float y): Transform a coordinate Y using the current transformation matrix.");
		MethodInitializationgroup("transformdx", &Kifcomplexwidget::MethodTransform_dx, P_TWO, "transformdx(float x, float y): Transform a distance DX using the current transformation matrix.");
		MethodInitializationgroup("transformdy", &Kifcomplexwidget::MethodTransform_dy, P_TWO, "transformdy(float x, float y): Transform a distance DY using the current transformation matrix.");
		MethodInitializationgroup("transformedvertex", &Kifcomplexwidget::MethodTransform_x, P_TWO, "transformedvertex(float x, float y): add transformations to vertices list.");

		MethodInitializationgroup("beginpoints", &Kifcomplexwidget::MethodBeginPoints, P_NONE, "beginpoints(): Start accumulating vertices");
		MethodInitializationgroup("endpoints", &Kifcomplexwidget::MethodEndPoints, P_NONE, "endpoints(): Stop accumulating vertices");

		MethodInitializationgroup("beginline", &Kifcomplexwidget::MethodBeginLine, P_NONE, "beginline(): Start drawing lines.");
		MethodInitializationgroup("endline", &Kifcomplexwidget::MethodEndLine, P_NONE, "endline(): End drawing lines.");

		MethodInitializationgroup("beginloop", &Kifcomplexwidget::MethodBeginLoop, P_NONE, "beginloop(): Start drawing a closed sequence of lines.");
		MethodInitializationgroup("endloop", &Kifcomplexwidget::MethodEndLoop, P_NONE, "endloop(): End drawing a closed sequence of lines.");

		MethodInitializationgroup("beginpolygon", &Kifcomplexwidget::MethodBeginPolygon, P_NONE, "beginpolygon(): Start drawing a convex filled polygon.");
		MethodInitializationgroup("endpolygon", &Kifcomplexwidget::MethodEndPolygon, P_NONE, "endpolygon(): End drawing a convex filled polygon.");

		MethodInitializationgroup("begincomplexpolygon", &Kifcomplexwidget::MethodBeginComplexPolygon, P_NONE, "begincomplexpolygon(): Start drawing a complex filled polygon.");
		MethodInitializationgroup("gap", &Kifcomplexwidget::MethodGap, P_NONE, "gap(): Fill the gap while drawing a complex polygon.");
		MethodInitializationgroup("endcomplexpolygon", &Kifcomplexwidget::MethodEndComplexPolygon, P_NONE, "endcomplexpolygon(): End drawing a complex filled polygon.");


		MethodInitializationgroup("arc", &Kifcomplexwidget::MethodArc, P_FIVE | P_SIX, "arc(int x,int y,int x1, int y1, float a1, float a2): Draw an arc.\rarc(float x,float y,float rad,float a1,float a2): Add a series of points to the current path on the arc of a circle;");
		MethodInitializationgroup("curve", &Kifcomplexwidget::MethodCurve, P_EIGHT, "curve(float x,float y,float x1,float y1,float x1,float y2,float x3,float y3): Add a series of points on a Bezier curve to the path. The curve ends (and two of the points) are at x,y and x3,y3.");
		MethodInitializationgroup("pushclip", &Kifcomplexwidget::MethodPushClip, P_FOUR, "pushclip(int x,int y,int w, int h): Insert a clip region, with the following coordinates");
		MethodInitializationgroup("popclip", &Kifcomplexwidget::MethodPopClip, P_NONE, "popclip(): Release a clip region");


		MethodInitializationgroup("pie", &Kifcomplexwidget::MethodPie, P_SIX, "pie(int x,int y,int x1, int y1, float a1, float a2): Draw a pie");
		MethodInitializationgroup("point", &Kifcomplexwidget::MethodPoint, P_FOUR, "point(int x,int y): Draw a pixel");
		MethodInitializationgroup("circle", &Kifcomplexwidget::MethodCircle, P_THREE | P_FOUR, "circle(int x,int y,int r,int color): Draw a circle. 'color' is optional.");
		MethodInitializationgroup("textsize", &Kifcomplexwidget::MethodTextMeasure, P_ONE, "textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels");
		MethodInitializationgroup("drawtext", &Kifcomplexwidget::MethodDrawText, P_THREE, "drawtext(string l,int x,int y): Put a text at position x,y");
		MethodInitializationgroup("redraw", &Kifcomplexwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationgroup("rgbcolor", &Kifcomplexwidget::MethodRGBColor, P_ONE | P_THREE, "rgbcolor(string|int color)|(int r,int g,int b)|(vector color): either return a vector of the color decomposition or return the value of rgb values combined");
		MethodInitializationgroup("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationgroup("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationgroup("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
		MethodInitializationgroup("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the label alignment");

#ifndef FLTKNOJPEG
		MethodInitializationImage("loadjpeg", &Kifimage::MethodLoadImageJPEG, P_ONE, "loadjpeg(string filename): Load a JPEG image");
#endif
		MethodInitializationImage("loadgif", &Kifimage::MethodLoadImageGIF, P_ONE, "loadgif(string filename): Load a GIF image");

		MethodInitializationBitmap("load", &Kifbitmap::MethodLoadbitmap, P_THREE, "load(vector bitmaps,int w,int h): Load a bitmap");

		MethodInitializationTable("create", &Kiftable::MethodCreate, P_FIVE, "create(int x,int y,int w,int h,string label): Create a table of objects, and starts adding");
		MethodInitializationTable("clear", &Kiftable::MethodClear, P_NONE, "clear(): Clear the table");
		MethodInitializationTable("add", &Kiftable::MethodAdd, P_THREE, "add(int R,int C,string v): Add a value on row R and column C");
		MethodInitializationTable("cell", &Kiftable::MethodCell, P_TWO, "cell(int R,int C): Return the value of the cell on row R and column C");
		MethodInitializationTable("cellalign", &Kiftable::MethodCellAlignment, P_ONE, "cellalign(align): Set the inner cell alignement");
		MethodInitializationTable("cellalignheadercol", &Kiftable::MethodCellAlignmentHeaderCol, P_ONE, "cellalignheadercol(align): Set the header column alignement");
		MethodInitializationTable("cellalignheaderrow", &Kiftable::MethodCellAlignmentHeaderRow, P_ONE, "cellalignheaderrow(align): Set the header row alignement");
		MethodInitializationTable("row", &Kiftable::MethodRow, P_ONE | P_NONE, "row()|(int nb): Define the number of rows");
		MethodInitializationTable("column", &Kiftable::MethodColumn, P_ONE | P_NONE, "column()|(int nb): Define the number of columns");
		MethodInitializationTable("rowheight", &Kiftable::MethodRowHeight, P_ONE, "rowheight(int height): Define the row height in pixel");
		MethodInitializationTable("columnwidth", &Kiftable::MethodColumnWidth, P_ONE, "columnwidth(int width): Define the column width in pixel");
		MethodInitializationTable("font", &Kiftable::MethodFont, P_ONE | P_NONE, "font(int s): set or return the text font");
		MethodInitializationTable("fontsize", &Kiftable::MethodFontSize, P_ONE | P_NONE, "fontsize(int c): set or return the text font size");
		MethodInitializationTable("colorbg", &Kiftable::MethodColorbg, P_ONE | P_NONE, "colorbg(int c): set or return the cell color background");
		MethodInitializationTable("colorfg", &Kiftable::MethodColorfg, P_ONE | P_NONE, "colorfg(int c): set or return the cell color foreground");
		MethodInitializationTable("when", &Kiftable::MethodWhen, 65534, "when(string when1, string when2,...): Type of event to trigger the callback");
		MethodInitializationTable("selectioncolor", &Kiftable::MethodTableSelectionColor, P_ONE | P_NONE, "colorselection(int color): Color for the selected elements");
		MethodInitializationTable("boxtype", &Kiftable::MethodBoxType, P_ONE, "boxtype(int boxtype): box type");
		MethodInitializationTable("rowheader", &Kiftable::MethodRowHeader, P_TWO, "rowheader(int pos,string label): Set the row header label at row pos");
		MethodInitializationTable("columnheader", &Kiftable::MethodColumnHeader, P_TWO, "columnheader(int pos,string label): Set the column header label at column pos");
		MethodInitializationTable("rowheaderheight", &Kiftable::MethodRowHeaderHeight, P_ONE, "rowheaderheight(int height): the size in pixel of the row header");
		MethodInitializationTable("columnheaderwidth", &Kiftable::MethodColumnHeaderWidth, P_ONE, "columnheaderwidth(int width): the size in pixel of the column header");
		MethodInitializationTable("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationTable("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationTable("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationTable("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationTable("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationTable("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationTable("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationTable("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationTable("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationTable("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationTable("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationTable("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationTable("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
		MethodInitializationTable("copy", &KifWidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
		MethodInitializationTable("paste", &KifWidget::MethodPaste, P_NONE, "paste(): paste string");
		MethodInitializationTable("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the label alignment");

		MethodInitializationInput("create", &Kifinput::MethodCreate, P_SIX, "create(int x,int y,int w,int h,boolean multiline,string label): Create an input");
		MethodInitializationInput("value", &Kifinput::MethodValue, P_ONE | P_NONE, "value()|(string v): return the input buffer or set the initial buffer");
		MethodInitializationInput("insert", &Kifinput::MethodInsert, P_TWO, "insert(string s,int p): insert s at position p in the input");
		MethodInitializationInput("selection", &Kifinput::MethodSelection, P_NONE, "selection(): return the selected text in the input");
		MethodInitializationInput("color", &Kifinput::MethodColor, P_ONE | P_NONE, "color(string c|int c): set or return the text color");
		MethodInitializationInput("word", &Kifinput::MethodWord, P_ONE, "word(int pos): Return the word at position pos");
		MethodInitializationInput("font", &Kifinput::MethodFont, P_ONE | P_NONE, "font(string s): set or return the text font");
		MethodInitializationInput("fontsize", &Kifinput::MethodFontSize, P_ONE | P_NONE, "fontsize(int c): set or return the text font size");
		MethodInitializationInput("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationInput("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationInput("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationInput("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationInput("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationInput("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationInput("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationInput("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationInput("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationInput("selectioncolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
		MethodInitializationInput("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationInput("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationInput("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationInput("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
		MethodInitializationInput("copy", &KifWidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
		MethodInitializationInput("paste", &KifWidget::MethodPaste, P_NONE, "paste(): paste string");
		MethodInitializationInput("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the label alignment");

		MethodInitializationEditor("create", &Kifeditor::MethodCreate, P_FIVE, "create(int x,int y,int w,int h,string label): Create an editor");
		MethodInitializationEditor("selection", &Kifeditor::MethodSelection, P_NONE, "selection(): return the selected text in the editor or the coordinates of the selection according to the recipient variable");
		MethodInitializationEditor("value", &Kifeditor::MethodValue, P_ONE | P_NONE, "value()|(string v): return the text in the editor or initialize the editor");
		MethodInitializationEditor("color", &Kifeditor::MethodColor, P_ONE | P_NONE, "color(string c|int c): set or return the text color");
		MethodInitializationEditor("font", &Kifeditor::MethodFont, P_ONE | P_NONE, "font(string s)|(): set or return the text font");
		MethodInitializationEditor("fontsize", &Kifeditor::MethodFontSize, P_ONE | P_NONE, "fontsize(int c)|(): set or return the text font size");
		MethodInitializationEditor("setstyle", &Kifeditor::MethodSetStyle, P_THREE, "setstyle(int start,int end,string style): set the style of a portion of text");
		MethodInitializationEditor("load", &Kifeditor::MethodLoad, P_ONE, "load(string f): load file into editor");
		MethodInitializationEditor("save", &Kifeditor::MethodSave, P_ONE, "save(string f): save buffer to file f");
		MethodInitializationEditor("replace", &Kifeditor::MethodReplace, P_FOUR | P_THREE | P_TWO, "replace(string s,string sub,int i,bool matchcase): replace s with sub according to matchcase starting at i");
		MethodInitializationEditor("replaceall", &Kifeditor::MethodReplaceAll, P_TWO | P_THREE, "replaceall(string s,string sub,bool matchcase): replace all occurrences of s with sub according to matchcase");
		MethodInitializationEditor("find", &Kifeditor::MethodFind, P_ONE | P_TWO | P_THREE, "find(string s,int i,bool matchcase): find the position of a substring from i");
		MethodInitializationEditor("rfind", &Kifeditor::MethodRFind, P_ONE | P_TWO | P_THREE, "rfind(string s,int i,bool matchcase): find the position of a substring from i backward");
		MethodInitializationEditor("highlight", &Kifeditor::MethodHighlight, P_NONE | P_TWO, "highlight(int start,int end)|(): highlight a portion of text. Return true if the text is highlighted or the highlighted text");
		MethodInitializationEditor("unhighlight", &Kifeditor::MethodUnHighlight, P_NONE, "unhighlight(): unhighlight a portion of text");
		MethodInitializationEditor("cursorstyle", &Kifeditor::MethodCursorStyle, P_ONE, "cursorstyle(int cursorshape): Set the cursor shape");
		MethodInitializationEditor("onmouse", &Kifeditor::MethodOnMouseClick, P_THREE, "onmouse(int action,function f,object o): callback when clicking");
		MethodInitializationEditor("onkey", &Kifeditor::MethodOnKey, P_THREE, "onkey(int action,function f,object o): callback when a key is pressed");
		MethodInitializationEditor("onvscroll", &Kifeditor::MethodOnVScroll, P_TWO, "onvscroll(function f,object o): callback when scrolling vertically");
		MethodInitializationEditor("onhscroll", &Kifeditor::MethodOnHScroll, P_TWO, "onhscroll(function f,object o): callback when scrolling horizontally");
		MethodInitializationEditor("getstyle", &Kifeditor::MethodGetStyle, P_TWO, "getstyle(int start,int end): return a vector of style for each character of the text section");
		MethodInitializationEditor("addstyle", &Kifeditor::MethodAddStyles, P_ONE, "addstyle(map style): Initialize styles");

		MethodInitializationEditor("delete", &Kifeditor::MethodDelete, P_NONE, "delete(): delete selected text from editor");
		MethodInitializationEditor("cut", &Kifeditor::MethodCut, P_NONE, "cut(): cut selected text from editor into clipboard");
		MethodInitializationEditor("copy", &Kifeditor::MethodCopy, P_NONE | P_ONE, "copy(string s): copy selected text from editor into clipboard (string s is optional)");
		MethodInitializationEditor("paste", &Kifeditor::MethodPaste, P_NONE, "paste(): paste selected text from clipboard");
		MethodInitializationEditor("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the label alignment");

		MethodInitializationEditor("select", &Kifeditor::MethodSelect, P_TWO | P_NONE, "select(): Return selected string.\rselect(int posfirst,int poslast): Select the string between posfirst and posend");
		MethodInitializationEditor("line", &Kifeditor::MethodLine, P_ONE | P_NONE, "line(int l): return the current line position of the caret or return the line corresponding to a position");
		MethodInitializationEditor("linebounds", &Kifeditor::MethodLineBoundaries, P_ONE | P_NONE, "linebounds(int l): return the beginning and the end at the current line position of the caret or return the line boundaries corresponding to a position");
		MethodInitializationEditor("linecharbounds", &Kifeditor::MethodLineBoundariesChar, P_ONE | P_NONE, "linecharbounds(): return the beginning and the end at the current line position of the caret or return the line boundaries corresponding to a position as characters");
		MethodInitializationEditor("cursor", &Kifeditor::MethodCursor, P_ONE | P_NONE, "cursor(int l): return the current cursor position of the caret or set the cursor position");
		MethodInitializationEditor("cursorchar", &Kifeditor::MethodCursorChar, P_NONE, "cursorchar(): return the current cursor position of the caret as characters");
		MethodInitializationEditor("gotoline", &Kifeditor::MethodGotoLine, P_TWO, "gotoline(int l,bool highlight): Goto line l. If true, highlight the line");
		MethodInitializationEditor("annotate", &Kifeditor::MethodAnnotate, P_TWO | P_THREE, "annotate(string s|vector v,string style,bool matchcase): Annotate each occurence of s with style (matchcase is a default argument)");
		MethodInitializationEditor("word", &Kifeditor::MethodWord, P_ONE, "word(int pos): Return the word at position pos");
		MethodInitializationEditor("wrap", &Kifeditor::MethodWrap, P_ONE, "wrap(bool): Set the wrapping mode");
		MethodInitializationEditor("annotateregexip", &Kifeditor::MethodAnnotateXIPRgx, P_TWO, "annotateregexip(string reg,string style): Annotate each string matching reg with style");
		MethodInitializationEditor("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationEditor("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationEditor("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationEditor("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationEditor("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationEditor("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationEditor("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationEditor("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationEditor("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationEditor("selectioncolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
		MethodInitializationEditor("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationEditor("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationEditor("textsize", &Kifeditor::MethodTextMeasure, P_ONE, "textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels");
		MethodInitializationEditor("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationEditor("insert", &Kifeditor::MethodInsert, P_TWO, "insert(string s,int p): insert s at position p in the input");
		MethodInitializationEditor("append", &Kifeditor::MethodAppend, P_ONE, "append(string s): append s at the end of the input");
		MethodInitializationEditor("byteposition", &Kifeditor::MethodBytePosition, P_ONE, "byteposition(int i): return the position in byte from a character position");
		MethodInitializationEditor("charposition", &Kifeditor::MethodCharacterPosition, P_ONE, "charposition(int i): return the position in character from a byte position");
		MethodInitializationEditor("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");

		MethodInitializationOutput("create", &Kifoutput::MethodCreate, P_SIX, "create(int x,int y,int w,int h,boolean multiline,string label): Create an output");
		MethodInitializationOutput("value", &Kifoutput::MethodValue, P_NONE | P_ONE, "value(string s): set the output buffer");
		MethodInitializationOutput("wrap", &Kifoutput::MethodWrap, P_ONE, "wrap(bool): Set the wrapping mode");
		MethodInitializationOutput("color", &Kifoutput::MethodColor, P_ONE | P_NONE, "color(string c|int c): set or return the text color");
		MethodInitializationOutput("font", &Kifoutput::MethodFont, P_ONE | P_NONE, "font(string s): set or return the text font");
		MethodInitializationOutput("fontsize", &Kifoutput::MethodFontSize, P_ONE | P_NONE, "fontsize(int c): set or return the text font size");
		MethodInitializationOutput("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationOutput("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationOutput("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationOutput("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationOutput("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationOutput("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationOutput("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationOutput("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationOutput("selectioncolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
		MethodInitializationOutput("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationOutput("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationOutput("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationOutput("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
		MethodInitializationOutput("copy", &KifWidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
		MethodInitializationOutput("paste", &KifWidget::MethodPaste, P_NONE, "paste(): paste string");
		MethodInitializationOutput("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the label alignment");

		MethodInitializationChoice("create", &Kifchoice::MethodCreate, P_FIVE, "create(int x,int y,int w,int h,string label): Create an choice");
		MethodInitializationChoice("value", &Kifchoice::MethodValue, P_ONE | P_NONE, "value(int s): set the choice initialization value");
		MethodInitializationChoice("font", &Kifchoice::MethodFont, P_ONE | P_NONE, "font(string s): set or return the text font");
		MethodInitializationChoice("menu", &Kifchoice::MethodMenu, P_ONE, "menu(vector s): Initialize the menu");
		MethodInitializationChoice("fontsize", &Kifchoice::MethodFontSize, P_ONE | P_NONE, "fontsize(int c): set or return the text font size");
		MethodInitializationChoice("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationChoice("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationChoice("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationChoice("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationChoice("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationChoice("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationChoice("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationChoice("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationChoice("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationChoice("selectioncolor", &KifWidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
		MethodInitializationChoice("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationChoice("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationChoice("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationChoice("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
		MethodInitializationChoice("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the label alignment");

		MethodInitializationFilebrowser("create", &Kiffilebrowser::MethodCreate, P_FOUR, "create(string intialdirectory,string filter,bool directory,string label): Open a file browser, select directories if directory is true");
		MethodInitializationFilebrowser("value", &Kiffilebrowser::MethodValue, P_NONE, "value(): Return the selected file");
		MethodInitializationFilebrowser("close", &Kiffilebrowser::MethodClose, P_NONE, "close(): Close the file browser");
		MethodInitializationFilebrowser("ok", &Kiffilebrowser::MethodOk, P_NONE, "ok(): return true if ok was pressed");

#ifdef MACUI
		MethodInitializationFilebrowser("open", &Kiffilebrowser::MethodOuvre, P_NONE, "open(): Choose a file via a mac window");
		MethodInitializationFilebrowser("load", &Kiffilebrowser::MethodCharge, P_ONE, "load(string): Load a file into a mac window");
#endif

		MethodInitializationBrowser("create", &Kifbrowser::MethodCreate, P_FIVE, "create(x,y,w,h,label): Create a browser");
		MethodInitializationBrowser("add", &Kifbrowser::MethodAdd, P_ONE, "add(label): Add a string to the browser");
		MethodInitializationBrowser("load", &Kifbrowser::MethodLoad, P_ONE, "load(filename): Load a file into the browser");
		MethodInitializationBrowser("insert", &Kifbrowser::MethodInsert, P_TWO, "insert(l,label): Insert a label before line l");
		MethodInitializationBrowser("clear", &Kifbrowser::MethodClear, P_NONE, "clear(): Clear the browser from all values");
		MethodInitializationBrowser("value", &Kifbrowser::MethodValue, P_NONE, "value(): return the current selected value");
		MethodInitializationBrowser("select", &Kifbrowser::MethodSelect, P_ONE | P_NONE, "select(): Return selected string.\rselect(int i): Return string in position i");
		MethodInitializationBrowser("deselect", &Kifbrowser::MethodDeselect, P_NONE | P_ONE, "deselect(): Deselect all items\rdeselect(int i): Deselect item i");
		MethodInitializationBrowser("columnchar", &Kifbrowser::MethodColumnchar, P_ONE | P_NONE, "columnchar(): Return the column char separator.\rcolumnchar(string): Set the column char separator");
		MethodInitializationBrowser("formatchar", &Kifbrowser::MethodFormatchar, P_ONE | P_NONE, "formatchar(): Return the format char.\rformatchar(string): Set the format char");
		MethodInitializationBrowser("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationBrowser("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationBrowser("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationBrowser("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationBrowser("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationBrowser("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationBrowser("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationBrowser("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationBrowser("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationBrowser("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationBrowser("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationBrowser("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationBrowser("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationBrowser("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
		MethodInitializationBrowser("copy", &KifWidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
		MethodInitializationBrowser("paste", &KifWidget::MethodPaste, P_NONE, "paste(): paste string");
		MethodInitializationBrowser("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the label alignment");

		MethodInitializationETree("label", &Kifetree::MethodLabel, P_NONE, "label(): Return the item label.");
		MethodInitializationETree("depth", &Kifetree::MethodLabel, P_NONE, "depth(): Return the depth of the item.");
		MethodInitializationETree("value", &Kifetree::MethodValue, P_ONE | P_NONE, "value(object): Associate the item with a value or return that value.");
		MethodInitializationETree("clean", &Kifetree::MethodClean, P_NONE, "clean(): Remove the object associated through value.");
		MethodInitializationETree("bgcolor", &Kifetree::MethodItemBGColor, P_NONE | P_ONE, "bgcolor(int c): Set or return the item background color.");
		MethodInitializationETree("fgcolor", &Kifetree::MethodItemFGColor, P_NONE | P_ONE, "fgcolor(int c): Set or return the foreground color.");
		MethodInitializationETree("font", &Kifetree::MethodItemFont, P_NONE | P_ONE, "font(int c): Set or return the item font.");
		MethodInitializationETree("fontsize", &Kifetree::MethodItemSize, P_NONE | P_ONE, "fontsize(int c): Set or return the item font size.");
		MethodInitializationETree("isclosed", &Kifetree::MethodIsclose, P_NONE, "isclosed(): Return true if element is closed.");
		MethodInitializationETree("isopen", &Kifetree::MethodIsopen, P_NONE, "isopen(): Return true if element is open.");
		MethodInitializationETree("isroot", &Kifetree::MethodIsroot, P_NONE, "isroot(): Return true if element is root.");
		MethodInitializationETree("isselected", &Kifetree::MethodIsselected, P_NONE, "isselected(): Return true if element is selected.");
		MethodInitializationETree("isactive", &Kifetree::MethodIsactive, P_NONE, "isactive(): Return true if element is active.");
		MethodInitializationETree("parent", &Kifetree::MethodParent, P_NONE, "parent(): Return the last element.");
		MethodInitializationETree("next", &Kifetree::MethodNext, P_NONE, "next(): Return the next element.");
		MethodInitializationETree("previous", &Kifetree::MethodPrevious, P_NONE, "previous(): Return the previous element.");
		MethodInitializationETree("child", &Kifetree::MethodChild, P_ONE, "child(int i): Return the child element at position i.");
		MethodInitializationETree("children", &Kifetree::MethodChildren, P_NONE, "children(): Return number of children.");
		MethodInitializationETree("activate", &Kifetree::MethodActivate, P_NONE, "activate(): Activate the current element.");
		MethodInitializationETree("deactivate", &Kifetree::MethodDeactivate, P_NONE, "deactivate(): Deactivate the current element.");

		MethodInitializationWTree("create", &Kifwtree::MethodCreate, P_FIVE, "create(int x,int y,int h,int w,string label): Create a tree");
		MethodInitializationWTree("clear", &Kifwtree::MethodClear, P_NONE, "clear(): Delete the tree items");
		MethodInitializationWTree("add", &Kifwtree::MethodAdd, P_ONE | P_TWO, "add(string path): Add a tree item.\radd(wtreeitem e,string n): Add a tree item after e.");
		MethodInitializationWTree("isclosed", &Kifwtree::MethodIsclose, P_ONE, "isclosed(string path): Return true if element is closed.\risclosed(wtreeitem e): Return true if element is closed.");
		MethodInitializationWTree("close", &Kifwtree::MethodClose, P_ONE, "close(string path): Close the element.\rclose(wtreeitem e): Close the element.");
		MethodInitializationWTree("open", &Kifwtree::MethodOpen, P_ONE, "open(string path): Open the element.\ropen(wtreeitem e): Open the element.");
		MethodInitializationWTree("insert", &Kifwtree::MethodInsert, P_ONE, "insert(wtreeitem e,string label,int pos): Insert an element after e with label at position pos in the children list.");
		MethodInitializationWTree("remove", &Kifwtree::MethodRemove, P_ONE, "remove(wtreeitem e): Remove the element e from the tree.");
		MethodInitializationWTree("insertabove", &Kifwtree::MethodInsertAbove, P_TWO, "insertabove(wtreeitem e,string label): Insert an element above e with label.");
		MethodInitializationWTree("connectorcolor", &Kifwtree::MethodConnectorColor, P_NONE | P_ONE, "connectorcolor(int c): Set or return the connector color.");
		MethodInitializationWTree("connectorstyle", &Kifwtree::MethodConnectorStyle, P_NONE | P_ONE, "connectorstyle(int s): Set or return the connector style.");
		MethodInitializationWTree("connectorwidth", &Kifwtree::MethodConnectorWidth, P_NONE | P_ONE, "connectorwidth(int s): Set or return the connector width.");
		MethodInitializationWTree("sortorder", &Kifwtree::MethodSortOrder, P_NONE | P_ONE, "sortorder(int s): Set or return the sort order.");
		MethodInitializationWTree("selectmode", &Kifwtree::MethodSelectmode, P_NONE | P_ONE, "selectmode(int s): Set or return the selection mode.");

		MethodInitializationWTree("itembgcolor", &Kifwtree::MethodItemBGColor, P_NONE | P_ONE, "itembgcolor(int c): Set or return the item background color.");
		MethodInitializationWTree("itemfgcolor", &Kifwtree::MethodItemFGColor, P_NONE | P_ONE, "itemfgcolor(int c): Set or return the foreground color.");
		MethodInitializationWTree("itemfont", &Kifwtree::MethodItemFont, P_NONE | P_ONE, "itemfont(int c): Set or return the item font.");
		MethodInitializationWTree("itemsize", &Kifwtree::MethodItemSize, P_NONE | P_ONE, "itemsize(int c): Set or return the item font size.");

		MethodInitializationWTree("marginleft", &Kifwtree::MethodMarginLeft, P_NONE | P_ONE, "Set or Get the amount of white space (in pixels) that should appear between the widget's left border and the tree's contents.");
		MethodInitializationWTree("margintop", &Kifwtree::MethodMarginTop, P_NONE | P_ONE, "margintop(int s): Set or Get the amount of white space (in pixels) that should appear between the widget's top border and the top of the tree's contents.");

		MethodInitializationWTree("root", &Kifwtree::MethodRoot, P_NONE, "root(): Return the root element.");
		MethodInitializationWTree("rootlabel", &Kifwtree::MethodRootLabel, P_ONE, "rootlabel(string r): Set the root label.");
		MethodInitializationWTree("clicked", &Kifwtree::MethodClicked, P_NONE, "clicked(): Return the element that was clicked.");
		MethodInitializationWTree("first", &Kifwtree::MethodFirst, P_NONE, "first(): Return the first element.");
		MethodInitializationWTree("find", &Kifwtree::MethodFind, P_ONE, "find(string path): Return the element matching the path.");
		MethodInitializationWTree("last", &Kifwtree::MethodLast, P_NONE, "last(): Return the last element.");
		MethodInitializationWTree("next", &Kifwtree::MethodNext, P_NONE | P_ONE, "next(wtreeitem e): Return the next element after e.");
		MethodInitializationWTree("previous", &Kifwtree::MethodPrevious, P_NONE | P_ONE, "previous(wtreeitem e): Return the previous element before e.");

		MethodInitializationWTree("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationWTree("tooltip", &KifWidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
		MethodInitializationWTree("show", &KifWidget::MethodShow, P_NONE, "show(): Show the widget");
		MethodInitializationWTree("focus", &KifWidget::MethodFocus, P_NONE, "focus(): Get the focus");
		MethodInitializationWTree("hide", &KifWidget::MethodHide, P_NONE, "hide(): Hide the widget");
		MethodInitializationWTree("labelsize", &KifWidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
		MethodInitializationWTree("labelfont", &KifWidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
		MethodInitializationWTree("labelcolor", &KifWidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
		MethodInitializationWTree("labeltype", &KifWidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
		MethodInitializationWTree("label", &KifWidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
		MethodInitializationWTree("coords", &KifWidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "coords(int c): return a vector of the widget coordinates");
		MethodInitializationWTree("redraw", &KifWidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
		MethodInitializationWTree("backgroundcolor", &KifWidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
		MethodInitializationWTree("created", &KifWidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
		MethodInitializationWTree("copy", &KifWidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
		MethodInitializationWTree("paste", &KifWidget::MethodPaste, P_NONE, "paste(): paste string");
		MethodInitializationWTree("align", &KifWidget::MethodAlign, P_ONE, "align(int): define the label alignment");

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




