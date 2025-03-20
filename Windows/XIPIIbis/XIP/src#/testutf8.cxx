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
filename   : testutf8.cxx
Date       : 12/08/2003
Purpose    : Conversion methods to handle UTF8 strings
Programmer : Claude ROUX
Reviewer   :
*/


#ifdef KIF_RAW
#include "kif.h"
#else
#include "parsbin.h"
#endif

#include "conversion.h"
#include "x_node.h"

#ifdef KIF_HASH_MAP
#if defined(WIN32) || defined(MAVERICK)
#include <unordered_map>
#define hmap unordered_map
using std::unordered_map;
#else
#include <tr1/unordered_map>
#define hmap std::tr1::unordered_map
#endif
#else
#define hmap map
#endif

static hmap<string, int> htmlcodes;
static hmap<int, string> codeshtml;
static hmap<string, string> metacodes;
static hmap<int, string> dostoutf8;
static hmap<string, int> utf8todos;
static hmap<int, int> dos2unicode;
static hmap<int, int> unicode2dos;

extern double kifnan;

BULONG s_XLConvert(char* number) {
	int l = strlen(number);
	if (number[0] != '0' || number[1] != 'x') {
		BLONG v = 0;
		int i = 0;
		int m = 1;
		if (*number == '-') {
			i = 1;
			m = -1;
		}
		for (; i<l; i++) {
			if (number[i]<48 || number[i]>57)
				return (v*m);
			v = v * 10 + (number[i] - 48);
		}
		v *= m;
		return v;
	}

	BULONG v = 0;
	for (int i = 2; i<l; i++) {
		v <<= 4;
		switch (number[i]) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			v |= number[i] - 48;
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			v |= number[i] - 55;
			break;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			v |= number[i] - 87;
			break;
		default:
			return v;
		}
	}
	return v;
}

static void InitDosCodes() {
	dostoutf8[128] = "Ç";
	utf8todos["Ç"] = 128;
	dos2unicode[128] = 199;
	unicode2dos[199] = 128;
	dostoutf8[129] = "ü";
	utf8todos["ü"] = 129;
	dos2unicode[129] = 252;
	unicode2dos[252] = 129;
	dostoutf8[130] = "é";
	utf8todos["é"] = 130;
	dos2unicode[130] = 233;
	unicode2dos[233] = 130;
	dostoutf8[131] = "â";
	utf8todos["â"] = 131;
	dos2unicode[131] = 226;
	unicode2dos[226] = 131;
	dostoutf8[132] = "ä";
	utf8todos["ä"] = 132;
	dos2unicode[132] = 228;
	unicode2dos[228] = 132;
	dostoutf8[133] = "à";
	utf8todos["à"] = 133;
	dos2unicode[133] = 224;
	unicode2dos[224] = 133;
	dostoutf8[134] = "å";
	utf8todos["å"] = 134;
	dos2unicode[134] = 229;
	unicode2dos[229] = 134;
	dostoutf8[135] = "ç";
	utf8todos["ç"] = 135;
	dos2unicode[135] = 231;
	unicode2dos[231] = 135;
	dostoutf8[136] = "ê";
	utf8todos["ê"] = 136;
	dos2unicode[136] = 234;
	unicode2dos[234] = 136;
	dostoutf8[137] = "ë";
	utf8todos["ë"] = 137;
	dos2unicode[137] = 235;
	unicode2dos[235] = 137;
	dostoutf8[138] = "è";
	utf8todos["è"] = 138;
	dos2unicode[138] = 232;
	unicode2dos[232] = 138;
	dostoutf8[139] = "ï";
	utf8todos["ï"] = 139;
	dos2unicode[139] = 239;
	unicode2dos[239] = 139;
	dostoutf8[140] = "î";
	utf8todos["î"] = 140;
	dos2unicode[140] = 238;
	unicode2dos[238] = 140;
	dostoutf8[141] = "ì";
	utf8todos["ì"] = 141;
	dos2unicode[141] = 236;
	unicode2dos[236] = 141;
	dostoutf8[142] = "Ä";
	utf8todos["Ä"] = 142;
	dos2unicode[142] = 196;
	unicode2dos[196] = 142;
	dostoutf8[143] = "Å";
	utf8todos["Å"] = 143;
	dos2unicode[143] = 197;
	unicode2dos[197] = 143;
	dostoutf8[144] = "É";
	utf8todos["É"] = 144;
	dos2unicode[144] = 201;
	unicode2dos[201] = 144;
	dostoutf8[145] = "æ";
	utf8todos["æ"] = 145;
	dos2unicode[145] = 230;
	unicode2dos[230] = 145;
	dostoutf8[146] = "Æ";
	utf8todos["Æ"] = 146;
	dos2unicode[146] = 198;
	unicode2dos[198] = 146;
	dostoutf8[147] = "ô";
	utf8todos["ô"] = 147;
	dos2unicode[147] = 244;
	unicode2dos[244] = 147;
	dostoutf8[148] = "ö";
	utf8todos["ö"] = 148;
	dos2unicode[148] = 246;
	unicode2dos[246] = 148;
	dostoutf8[149] = "ò";
	utf8todos["ò"] = 149;
	dos2unicode[149] = 242;
	unicode2dos[242] = 149;
	dostoutf8[150] = "û";
	utf8todos["û"] = 150;
	dos2unicode[150] = 251;
	unicode2dos[251] = 150;
	dostoutf8[151] = "ù";
	utf8todos["ù"] = 151;
	dos2unicode[151] = 249;
	unicode2dos[249] = 151;
	dostoutf8[152] = "ÿ";
	utf8todos["ÿ"] = 152;
	dos2unicode[152] = 255;
	unicode2dos[255] = 152;
	dostoutf8[153] = "Ö";
	utf8todos["Ö"] = 153;
	dos2unicode[153] = 214;
	unicode2dos[214] = 153;
	dostoutf8[154] = "Ü";
	utf8todos["Ü"] = 154;
	dos2unicode[154] = 220;
	unicode2dos[220] = 154;
	dostoutf8[155] = "¢";
	utf8todos["¢"] = 155;
	dos2unicode[155] = 162;
	unicode2dos[162] = 155;
	dostoutf8[156] = "£";
	utf8todos["£"] = 156;
	dos2unicode[156] = 163;
	unicode2dos[163] = 156;
	dostoutf8[157] = "¥";
	utf8todos["¥"] = 157;
	dos2unicode[157] = 165;
	unicode2dos[165] = 157;
	dostoutf8[158] = "₧";
	utf8todos["₧"] = 158;
	dos2unicode[158] = 8359;
	unicode2dos[8359] = 158;
	dostoutf8[159] = "ƒ";
	utf8todos["ƒ"] = 159;
	dos2unicode[159] = 402;
	unicode2dos[402] = 159;
	dostoutf8[160] = "á";
	utf8todos["á"] = 160;
	dos2unicode[160] = 225;
	unicode2dos[225] = 160;
	dostoutf8[161] = "í";
	utf8todos["í"] = 161;
	dos2unicode[161] = 237;
	unicode2dos[237] = 161;
	dostoutf8[162] = "ó";
	utf8todos["ó"] = 162;
	dos2unicode[162] = 243;
	unicode2dos[243] = 162;
	dostoutf8[163] = "ú";
	utf8todos["ú"] = 163;
	dos2unicode[163] = 250;
	unicode2dos[250] = 163;
	dostoutf8[164] = "ñ";
	utf8todos["ñ"] = 164;
	dos2unicode[164] = 241;
	unicode2dos[241] = 164;
	dostoutf8[165] = "Ñ";
	utf8todos["Ñ"] = 165;
	dos2unicode[165] = 209;
	unicode2dos[209] = 165;
	dostoutf8[166] = "ª";
	utf8todos["ª"] = 166;
	dos2unicode[166] = 170;
	unicode2dos[170] = 166;
	dostoutf8[167] = "º";
	utf8todos["º"] = 167;
	dos2unicode[167] = 186;
	unicode2dos[186] = 167;
	dostoutf8[168] = "¿";
	utf8todos["¿"] = 168;
	dos2unicode[168] = 191;
	unicode2dos[191] = 168;
	dostoutf8[169] = "⌐";
	utf8todos["⌐"] = 169;
	dos2unicode[169] = 8976;
	unicode2dos[8976] = 169;
	dostoutf8[170] = "¬";
	utf8todos["¬"] = 170;
	dos2unicode[170] = 172;
	unicode2dos[172] = 170;
	dostoutf8[171] = "½";
	utf8todos["½"] = 171;
	dos2unicode[171] = 189;
	unicode2dos[189] = 171;
	dostoutf8[172] = "¼";
	utf8todos["¼"] = 172;
	dos2unicode[172] = 188;
	unicode2dos[188] = 172;
	dostoutf8[173] = "¡";
	utf8todos["¡"] = 173;
	dos2unicode[173] = 161;
	unicode2dos[161] = 173;
	dostoutf8[174] = "«";
	utf8todos["«"] = 174;
	dos2unicode[174] = 171;
	unicode2dos[171] = 174;
	dostoutf8[175] = "»";
	utf8todos["»"] = 175;
	dos2unicode[175] = 187;
	unicode2dos[187] = 175;
	dostoutf8[176] = "░";
	utf8todos["░"] = 176;
	dos2unicode[176] = 9617;
	unicode2dos[9617] = 176;
	dostoutf8[177] = "▒";
	utf8todos["▒"] = 177;
	dos2unicode[177] = 9618;
	unicode2dos[9618] = 177;
	dostoutf8[178] = "▓";
	utf8todos["▓"] = 178;
	dos2unicode[178] = 9619;
	unicode2dos[9619] = 178;
	dostoutf8[179] = "│";
	utf8todos["│"] = 179;
	dos2unicode[179] = 9474;
	unicode2dos[9474] = 179;
	dostoutf8[180] = "┤";
	utf8todos["┤"] = 180;
	dos2unicode[180] = 9508;
	unicode2dos[9508] = 180;
	dostoutf8[181] = "╡";
	utf8todos["╡"] = 181;
	dos2unicode[181] = 9569;
	unicode2dos[9569] = 181;
	dostoutf8[182] = "╢";
	utf8todos["╢"] = 182;
	dos2unicode[182] = 9570;
	unicode2dos[9570] = 182;
	dostoutf8[183] = "╖";
	utf8todos["╖"] = 183;
	dos2unicode[183] = 9558;
	unicode2dos[9558] = 183;
	dostoutf8[184] = "╕";
	utf8todos["╕"] = 184;
	dos2unicode[184] = 9557;
	unicode2dos[9557] = 184;
	dostoutf8[185] = "╣";
	utf8todos["╣"] = 185;
	dos2unicode[185] = 9571;
	unicode2dos[9571] = 185;
	dostoutf8[186] = "║";
	utf8todos["║"] = 186;
	dos2unicode[186] = 9553;
	unicode2dos[9553] = 186;
	dostoutf8[187] = "╗";
	utf8todos["╗"] = 187;
	dos2unicode[187] = 9559;
	unicode2dos[9559] = 187;
	dostoutf8[188] = "╝";
	utf8todos["╝"] = 188;
	dos2unicode[188] = 9565;
	unicode2dos[9565] = 188;
	dostoutf8[189] = "╜";
	utf8todos["╜"] = 189;
	dos2unicode[189] = 9564;
	unicode2dos[9564] = 189;
	dostoutf8[190] = "╛";
	utf8todos["╛"] = 190;
	dos2unicode[190] = 9563;
	unicode2dos[9563] = 190;
	dostoutf8[191] = "┐";
	utf8todos["┐"] = 191;
	dos2unicode[191] = 9488;
	unicode2dos[9488] = 191;
	dostoutf8[192] = "└";
	utf8todos["└"] = 192;
	dos2unicode[192] = 9492;
	unicode2dos[9492] = 192;
	dostoutf8[193] = "┴";
	utf8todos["┴"] = 193;
	dos2unicode[193] = 9524;
	unicode2dos[9524] = 193;
	dostoutf8[194] = "┬";
	utf8todos["┬"] = 194;
	dos2unicode[194] = 9516;
	unicode2dos[9516] = 194;
	dostoutf8[195] = "├";
	utf8todos["├"] = 195;
	dos2unicode[195] = 9500;
	unicode2dos[9500] = 195;
	dostoutf8[196] = "─";
	utf8todos["─"] = 196;
	dos2unicode[196] = 9472;
	unicode2dos[9472] = 196;
	dostoutf8[197] = "┼";
	utf8todos["┼"] = 197;
	dos2unicode[197] = 9532;
	unicode2dos[9532] = 197;
	dostoutf8[198] = "╞";
	utf8todos["╞"] = 198;
	dos2unicode[198] = 9566;
	unicode2dos[9566] = 198;
	dostoutf8[199] = "╟";
	utf8todos["╟"] = 199;
	dos2unicode[199] = 9567;
	unicode2dos[9567] = 199;
	dostoutf8[200] = "╚";
	utf8todos["╚"] = 200;
	dos2unicode[200] = 9562;
	unicode2dos[9562] = 200;
	dostoutf8[201] = "╔";
	utf8todos["╔"] = 201;
	dos2unicode[201] = 9556;
	unicode2dos[9556] = 201;
	dostoutf8[202] = "╩";
	utf8todos["╩"] = 202;
	dos2unicode[202] = 9577;
	unicode2dos[9577] = 202;
	dostoutf8[203] = "╦";
	utf8todos["╦"] = 203;
	dos2unicode[203] = 9574;
	unicode2dos[9574] = 203;
	dostoutf8[204] = "╠";
	utf8todos["╠"] = 204;
	dos2unicode[204] = 9568;
	unicode2dos[9568] = 204;
	dostoutf8[205] = "═";
	utf8todos["═"] = 205;
	dos2unicode[205] = 9552;
	unicode2dos[9552] = 205;
	dostoutf8[206] = "╬";
	utf8todos["╬"] = 206;
	dos2unicode[206] = 9580;
	unicode2dos[9580] = 206;
	dostoutf8[207] = "╧";
	utf8todos["╧"] = 207;
	dos2unicode[207] = 9575;
	unicode2dos[9575] = 207;
	dostoutf8[208] = "╨";
	utf8todos["╨"] = 208;
	dos2unicode[208] = 9576;
	unicode2dos[9576] = 208;
	dostoutf8[209] = "╤";
	utf8todos["╤"] = 209;
	dos2unicode[209] = 9572;
	unicode2dos[9572] = 209;
	dostoutf8[210] = "╥";
	utf8todos["╥"] = 210;
	dos2unicode[210] = 9573;
	unicode2dos[9573] = 210;
	dostoutf8[211] = "╙";
	utf8todos["╙"] = 211;
	dos2unicode[211] = 9561;
	unicode2dos[9561] = 211;
	dostoutf8[212] = "╘";
	utf8todos["╘"] = 212;
	dos2unicode[212] = 9560;
	unicode2dos[9560] = 212;
	dostoutf8[213] = "╒";
	utf8todos["╒"] = 213;
	dos2unicode[213] = 9554;
	unicode2dos[9554] = 213;
	dostoutf8[214] = "╓";
	utf8todos["╓"] = 214;
	dos2unicode[214] = 9555;
	unicode2dos[9555] = 214;
	dostoutf8[215] = "╫";
	utf8todos["╫"] = 215;
	dos2unicode[215] = 9579;
	unicode2dos[9579] = 215;
	dostoutf8[216] = "╪";
	utf8todos["╪"] = 216;
	dos2unicode[216] = 9578;
	unicode2dos[9578] = 216;
	dostoutf8[217] = "┘";
	utf8todos["┘"] = 217;
	dos2unicode[217] = 9496;
	unicode2dos[9496] = 217;
	dostoutf8[218] = "┌";
	utf8todos["┌"] = 218;
	dos2unicode[218] = 9484;
	unicode2dos[9484] = 218;
	dostoutf8[219] = "█";
	utf8todos["█"] = 219;
	dos2unicode[219] = 9608;
	unicode2dos[9608] = 219;
	dostoutf8[220] = "▄";
	utf8todos["▄"] = 220;
	dos2unicode[220] = 9604;
	unicode2dos[9604] = 220;
	dostoutf8[221] = "▌";
	utf8todos["▌"] = 221;
	dos2unicode[221] = 9612;
	unicode2dos[9612] = 221;
	dostoutf8[222] = "▐";
	utf8todos["▐"] = 222;
	dos2unicode[222] = 9616;
	unicode2dos[9616] = 222;
	dostoutf8[223] = "▀";
	utf8todos["▀"] = 223;
	dos2unicode[223] = 9600;
	unicode2dos[9600] = 223;
	dostoutf8[224] = "α";
	utf8todos["α"] = 224;
	dos2unicode[224] = 945;
	unicode2dos[945] = 224;
	dostoutf8[225] = "ß";
	utf8todos["ß"] = 225;
	dos2unicode[225] = 223;
	unicode2dos[223] = 225;
	dostoutf8[226] = "Γ";
	utf8todos["Γ"] = 226;
	dos2unicode[226] = 915;
	unicode2dos[915] = 226;
	dostoutf8[227] = "π";
	utf8todos["π"] = 227;
	dos2unicode[227] = 960;
	unicode2dos[960] = 227;
	dostoutf8[228] = "Σ";
	utf8todos["Σ"] = 228;
	dos2unicode[228] = 931;
	unicode2dos[931] = 228;
	dostoutf8[229] = "σ";
	utf8todos["σ"] = 229;
	dos2unicode[229] = 963;
	unicode2dos[963] = 229;
	dostoutf8[230] = "µ";
	utf8todos["µ"] = 230;
	dos2unicode[230] = 181;
	unicode2dos[181] = 230;
	dostoutf8[231] = "τ";
	utf8todos["τ"] = 231;
	dos2unicode[231] = 964;
	unicode2dos[964] = 231;
	dostoutf8[232] = "Φ";
	utf8todos["Φ"] = 232;
	dos2unicode[232] = 934;
	unicode2dos[934] = 232;
	dostoutf8[233] = "Θ";
	utf8todos["Θ"] = 233;
	dos2unicode[233] = 920;
	unicode2dos[920] = 233;
	dostoutf8[234] = "Ω";
	utf8todos["Ω"] = 234;
	dos2unicode[234] = 937;
	unicode2dos[937] = 234;
	dostoutf8[235] = "δ";
	utf8todos["δ"] = 235;
	dos2unicode[235] = 948;
	unicode2dos[948] = 235;
	dostoutf8[236] = "∞";
	utf8todos["∞"] = 236;
	dos2unicode[236] = 8734;
	unicode2dos[8734] = 236;
	dostoutf8[237] = "φ";
	utf8todos["φ"] = 237;
	dos2unicode[237] = 966;
	unicode2dos[966] = 237;
	dostoutf8[238] = "ε";
	utf8todos["ε"] = 238;
	dos2unicode[238] = 949;
	unicode2dos[949] = 238;
	dostoutf8[239] = "∩";
	utf8todos["∩"] = 239;
	dos2unicode[239] = 8745;
	unicode2dos[8745] = 239;
	dostoutf8[240] = "≡";
	utf8todos["≡"] = 240;
	dos2unicode[240] = 8801;
	unicode2dos[8801] = 240;
	dostoutf8[241] = "±";
	utf8todos["±"] = 241;
	dos2unicode[241] = 177;
	unicode2dos[177] = 241;
	dostoutf8[242] = "≥";
	utf8todos["≥"] = 242;
	dos2unicode[242] = 8805;
	unicode2dos[8805] = 242;
	dostoutf8[243] = "≤";
	utf8todos["≤"] = 243;
	dos2unicode[243] = 8804;
	unicode2dos[8804] = 243;
	dostoutf8[244] = "⌠";
	utf8todos["⌠"] = 244;
	dos2unicode[244] = 8992;
	unicode2dos[8992] = 244;
	dostoutf8[245] = "⌡";
	utf8todos["⌡"] = 245;
	dos2unicode[245] = 8993;
	unicode2dos[8993] = 245;
	dostoutf8[246] = "÷";
	utf8todos["÷"] = 246;
	dos2unicode[246] = 247;
	unicode2dos[247] = 246;
	dostoutf8[247] = "≈";
	utf8todos["≈"] = 247;
	dos2unicode[247] = 8776;
	unicode2dos[8776] = 247;
	dostoutf8[248] = "°";
	utf8todos["°"] = 248;
	dos2unicode[248] = 176;
	unicode2dos[176] = 248;
	dostoutf8[249] = "∙";
	utf8todos["∙"] = 249;
	dos2unicode[249] = 8729;
	unicode2dos[8729] = 249;
	dostoutf8[250] = "·";
	utf8todos["·"] = 250;
	dos2unicode[250] = 183;
	unicode2dos[183] = 250;
	dostoutf8[251] = "√";
	utf8todos["√"] = 251;
	dos2unicode[251] = 8730;
	unicode2dos[8730] = 251;
	dostoutf8[252] = "ⁿ";
	utf8todos["ⁿ"] = 252;
	dos2unicode[252] = 8319;
	unicode2dos[8319] = 252;
	dostoutf8[253] = "²";
	utf8todos["²"] = 253;
	dos2unicode[253] = 178;
	unicode2dos[178] = 253;
	dostoutf8[254] = "■";
	utf8todos["■"] = 254;
	dos2unicode[254] = 9632;
	unicode2dos[9632] = 254;
}

static void InitHtmlCodes() {
	metacodes["\\n"] = "\n";
	metacodes["\\r"] = "\r";
	metacodes["\\\""] = "\"";
	metacodes["\\t"] = "\t";

	codeshtml[34] = "&quot;";
	codeshtml[38] = "&amp;";
	codeshtml[60] = "&lsaquo;";
	codeshtml[62] = "&rsaquo;";

	codeshtml[128] = "&euro;";
	codeshtml[60] = "&lt;";
	codeshtml[62] = "&gt;";
	codeshtml[156] = "&oelig;";
	codeshtml[159] = "&Yuml;";
	codeshtml[160] = "&nbsp;";
	codeshtml[161] = "&iexcl;";
	codeshtml[162] = "&cent;";
	codeshtml[163] = "&pound;";
	codeshtml[164] = "&curren;";
	codeshtml[165] = "&yen;";
	codeshtml[166] = "&brvbar;";
	codeshtml[167] = "&sect;";
	codeshtml[168] = "&uml;";
	codeshtml[169] = "&copy;";
	codeshtml[170] = "&ordf;";
	codeshtml[171] = "&laquo;";
	codeshtml[172] = "&not;";
	codeshtml[173] = "&shy;";
	codeshtml[174] = "&reg;";
	codeshtml[175] = "&masr;";
	codeshtml[176] = "&deg;";
	codeshtml[177] = "&plusmn;";
	codeshtml[178] = "&sup2;";
	codeshtml[179] = "&sup3;";
	codeshtml[180] = "&acute;";
	codeshtml[181] = "&micro;";
	codeshtml[182] = "&para;";
	codeshtml[183] = "&middot;";
	codeshtml[184] = "&cedil;";
	codeshtml[185] = "&sup1;";
	codeshtml[186] = "&ordm;";
	codeshtml[187] = "&raquo;";
	codeshtml[188] = "&frac14;";
	codeshtml[189] = "&frac12;";
	codeshtml[190] = "&frac34;";
	codeshtml[191] = "&iquest;";
	codeshtml[192] = "&Agrave;";
	codeshtml[193] = "&Aacute;";
	codeshtml[194] = "&Acirc;";
	codeshtml[195] = "&Atilde;";
	codeshtml[196] = "&Auml;";
	codeshtml[197] = "&Aring;";
	codeshtml[198] = "&Aelig;";
	codeshtml[199] = "&Ccedil;";
	codeshtml[200] = "&Egrave;";
	codeshtml[201] = "&Eacute;";
	codeshtml[202] = "&Ecirc;";
	codeshtml[203] = "&Euml;";
	codeshtml[204] = "&Igrave;";
	codeshtml[205] = "&Iacute;";
	codeshtml[206] = "&Icirc;";
	codeshtml[207] = "&Iuml;";
	codeshtml[208] = "&eth;";
	codeshtml[209] = "&Ntilde;";
	codeshtml[210] = "&Ograve;";
	codeshtml[211] = "&Oacute;";
	codeshtml[212] = "&Ocirc;";
	codeshtml[213] = "&Otilde;";
	codeshtml[214] = "&Ouml;";
	codeshtml[215] = "&times;";
	codeshtml[216] = "&Oslash;";
	codeshtml[217] = "&Ugrave;";
	codeshtml[218] = "&Uacute;";
	codeshtml[219] = "&Ucirc;";
	codeshtml[220] = "&Uuml;";
	codeshtml[221] = "&Yacute;";
	codeshtml[222] = "&thorn;";
	codeshtml[223] = "&szlig;";
	codeshtml[224] = "&agrave;";
	codeshtml[225] = "&aacute;";
	codeshtml[226] = "&acirc;";
	codeshtml[227] = "&atilde;";
	codeshtml[228] = "&auml;";
	codeshtml[229] = "&aring;";
	codeshtml[230] = "&aelig;";
	codeshtml[231] = "&ccedil;";
	codeshtml[232] = "&egrave;";
	codeshtml[233] = "&eacute;";
	codeshtml[234] = "&ecirc;";
	codeshtml[235] = "&euml;";
	codeshtml[236] = "&igrave;";
	codeshtml[237] = "&iacute;";
	codeshtml[238] = "&icirc;";
	codeshtml[239] = "&iuml;";
	codeshtml[240] = "&eth;";
	codeshtml[241] = "&ntilde;";
	codeshtml[242] = "&ograve;";
	codeshtml[243] = "&oacute;";
	codeshtml[244] = "&ocirc;";
	codeshtml[245] = "&otilde;";
	codeshtml[246] = "&ouml;";
	codeshtml[247] = "&divide;";
	codeshtml[248] = "&oslash;";
	codeshtml[249] = "&ugrave;";
	codeshtml[250] = "&uacute;";
	codeshtml[251] = "&ucirc;";
	codeshtml[252] = "&uuml;";
	codeshtml[253] = "&yacute;";
	codeshtml[254] = "&thorn;";
	codeshtml[255] = "&yuml;";
	codeshtml[8249] = "&lsaquo;";
	codeshtml[8250] = "&rsaquo;";

	htmlcodes["&quot;"] = 34;
	htmlcodes["&amp;"] = 38;
	htmlcodes["&euro;"] = 128;
	htmlcodes["&lt;"] = 60;
	htmlcodes["&gt;"] = 62;
	htmlcodes["&oelig;"] = 156;
	htmlcodes["&Yuml;"] = 159;
	htmlcodes["&nbsp;"] = 160;
	htmlcodes["&iexcl;"] = 161;
	htmlcodes["&cent;"] = 162;
	htmlcodes["&pound;"] = 163;
	htmlcodes["&curren;"] = 164;
	htmlcodes["&yen;"] = 165;
	htmlcodes["&brvbar;"] = 166;
	htmlcodes["&sect;"] = 167;
	htmlcodes["&uml;"] = 168;
	htmlcodes["&copy;"] = 169;
	htmlcodes["&ordf;"] = 170;
	htmlcodes["&laquo;"] = 171;
	htmlcodes["&not;"] = 172;
	htmlcodes["&shy;"] = 173;
	htmlcodes["&reg;"] = 174;
	htmlcodes["&masr;"] = 175;
	htmlcodes["&deg;"] = 176;
	htmlcodes["&plusmn;"] = 177;
	htmlcodes["&sup2;"] = 178;
	htmlcodes["&sup3;"] = 179;
	htmlcodes["&acute;"] = 180;
	htmlcodes["&micro;"] = 181;
	htmlcodes["&para;"] = 182;
	htmlcodes["&middot;"] = 183;
	htmlcodes["&cedil;"] = 184;
	htmlcodes["&sup1;"] = 185;
	htmlcodes["&ordm;"] = 186;
	htmlcodes["&raquo;"] = 187;
	htmlcodes["&frac14;"] = 188;
	htmlcodes["&frac12;"] = 189;
	htmlcodes["&frac34;"] = 190;
	htmlcodes["&iquest;"] = 191;
	htmlcodes["&Agrave;"] = 192;
	htmlcodes["&Aacute;"] = 193;
	htmlcodes["&Acirc;"] = 194;
	htmlcodes["&Atilde;"] = 195;
	htmlcodes["&Auml;"] = 196;
	htmlcodes["&Aring;"] = 197;
	htmlcodes["&Aelig;"] = 198;
	htmlcodes["&Ccedil;"] = 199;
	htmlcodes["&Egrave;"] = 200;
	htmlcodes["&Eacute;"] = 201;
	htmlcodes["&Ecirc;"] = 202;
	htmlcodes["&Euml;"] = 203;
	htmlcodes["&Igrave;"] = 204;
	htmlcodes["&Iacute;"] = 205;
	htmlcodes["&Icirc;"] = 206;
	htmlcodes["&Iuml;"] = 207;
	htmlcodes["&eth;"] = 208;
	htmlcodes["&Ntilde;"] = 209;
	htmlcodes["&Ograve;"] = 210;
	htmlcodes["&Oacute;"] = 211;
	htmlcodes["&Ocirc;"] = 212;
	htmlcodes["&Otilde;"] = 213;
	htmlcodes["&Ouml;"] = 214;
	htmlcodes["&times;"] = 215;
	htmlcodes["&Oslash;"] = 216;
	htmlcodes["&Ugrave;"] = 217;
	htmlcodes["&Uacute;"] = 218;
	htmlcodes["&Ucirc;"] = 219;
	htmlcodes["&Uuml;"] = 220;
	htmlcodes["&Yacute;"] = 221;
	htmlcodes["&thorn;"] = 222;
	htmlcodes["&szlig;"] = 223;
	htmlcodes["&agrave;"] = 224;
	htmlcodes["&aacute;"] = 225;
	htmlcodes["&acirc;"] = 226;
	htmlcodes["&atilde;"] = 227;
	htmlcodes["&auml;"] = 228;
	htmlcodes["&aring;"] = 229;
	htmlcodes["&aelig;"] = 230;
	htmlcodes["&ccedil;"] = 231;
	htmlcodes["&egrave;"] = 232;
	htmlcodes["&eacute;"] = 233;
	htmlcodes["&ecirc;"] = 234;
	htmlcodes["&euml;"] = 235;
	htmlcodes["&igrave;"] = 236;
	htmlcodes["&iacute;"] = 237;
	htmlcodes["&icirc;"] = 238;
	htmlcodes["&iuml;"] = 239;
	htmlcodes["&eth;"] = 240;
	htmlcodes["&ntilde;"] = 241;
	htmlcodes["&ograve;"] = 242;
	htmlcodes["&oacute;"] = 243;
	htmlcodes["&ocirc;"] = 244;
	htmlcodes["&otilde;"] = 245;
	htmlcodes["&ouml;"] = 246;
	htmlcodes["&divide;"] = 247;
	htmlcodes["&oslash;"] = 248;
	htmlcodes["&ugrave;"] = 249;
	htmlcodes["&uacute;"] = 250;
	htmlcodes["&ucirc;"] = 251;
	htmlcodes["&uuml;"] = 252;
	htmlcodes["&yacute;"] = 253;
	htmlcodes["&thorn;"] = 254;
	htmlcodes["&yuml;"] = 255;
	htmlcodes["&lsaquo;"] = 8249;
	htmlcodes["&rsaquo;"] = 8250;
}

void c_HTMLcode(string& s, BULONG code) {
	if (codeshtml.find(code) != codeshtml.end())
		s = codeshtml[code];
	else {
		char ch[10];
		sprintf(ch, "&%d;", code);
		s = ch;
	}
}

void s_EvaluateMetaCharacters(string& s) {
	BULONG cd;

	switch (s[0]) {
	case '\\':
		switch (s.size()) {
		case 0:
		case 1:
			return;
		case 2:// \n \r \t \" etc...
			if (metacodes.find(s) != metacodes.end())
				s = metacodes[s];
			else
				s = s[1];
			return;
		case 4://ASCII decimal: \234
			if (s[1] == 'x') {
				s[0] = '0';
				cd = s_XLConvert(STR(s));
				break;
			}
			s[0] = ' ';
			cd = atoi(STR(s));
			break;
		case 6://unicode  \u00FE
			s[0] = '0'; s[1] = 'x';
			cd = s_XLConvert(STR(s));
		}
		break;
	case '&':
		if (s[1] == '#') {
			s = s.substr(2, s.size() - 3);
			cd = atoi(STR(s));
		}
		else
		if (htmlcodes.find(s) != htmlcodes.end())
			cd = htmlcodes[s];
		else
			return;
		break;
	case '%': //%CC
		s[0] = 'x';
		s = '0' + s;
		cd = s_XLConvert(STR(s));
		s = (char)cd;
		return;
	}

	uchar inter[10];
	c_unicodetoutf8(cd, inter);
	s = (char*)inter;
}

static int latincodetable[] = { 133, 0x2026, 160, 0x00A0, 161, 0x00A1, 166, 0x00A6, 170, 0x00AA, 172, 0x00AC, 173, 0x00AD, 175, 0x00AF, 176, 0x00B0, 177, 0x00B1,
181, 0x00B5, 182, 0x00B6, 183, 0x00B7, 184, 0x00B8, 186, 0x00BA, 191, 0x00BF, 215, 0x00D7, 247, 0x00F7, 223, 0x00DF, 150, 0x2013, 151, 0x2014, 171, 0x00AB,
187, 0x00BB, 139, 0x3008, 155, 0x3009, 134, 0x2020, 135, 0x2021, 162, 0x00A2, 163, 0x00A3, 164, 0x00A4, 128, 0x20AC, 165, 0x00A5, 188, 0x00BC, 189, 0x00BD,
190, 0x00BE, 185, 0x00B9, 178, 0x00B2, 179, 0x00B3, 145, 0x2018, 146, 0x2019, 147, 0x201C, 148, 0x201D, 0, 0 };

static int table[] = { 65, 97, 2, 66, 98, 2, 67, 99, 2, 68, 100, 2, 69, 101, 2, 70, 102, 2, 71, 103, 2, 72, 104, 2, 73, 105, 2, 74, 106, 2
, 75, 107, 2, 76, 108, 2, 77, 109, 2, 78, 110, 2, 79, 111, 2, 80, 112, 2, 81, 113, 2, 82, 114, 2, 83, 115, 2, 84, 116, 2
, 85, 117, 2, 86, 118, 2, 87, 119, 2, 88, 120, 2, 89, 121, 2, 90, 122, 2, 97, 65, 1, 98, 66, 1, 99, 67, 1, 100, 68, 1
, 101, 69, 1, 102, 70, 1, 103, 71, 1, 104, 72, 1, 105, 73, 1, 106, 74, 1, 107, 75, 1, 108, 76, 1, 109, 77, 1, 110, 78, 1
, 111, 79, 1, 112, 80, 1, 113, 81, 1, 114, 82, 1, 115, 83, 1, 116, 84, 1, 117, 85, 1, 118, 86, 1, 119, 87, 1, 120, 88, 1
, 121, 89, 1, 122, 90, 1, 170, 170, 1, 181, 924, 1, 186, 186, 1, 192, 224, 2, 193, 225, 2, 194, 226, 2, 195, 227, 2, 196, 228, 2
, 197, 229, 2, 198, 230, 2, 199, 231, 2, 200, 232, 2, 201, 233, 2, 202, 234, 2, 203, 235, 2, 204, 236, 2, 205, 237, 2, 206, 238, 2
, 207, 239, 2, 208, 240, 2, 209, 241, 2, 210, 242, 2, 211, 243, 2, 212, 244, 2, 213, 245, 2, 214, 246, 2, 216, 248, 2, 217, 249, 2
, 218, 250, 2, 219, 251, 2, 220, 252, 2, 221, 253, 2, 222, 254, 2, 223, 223, 1, 224, 192, 1, 225, 193, 1, 226, 194, 1, 227, 195, 1
, 228, 196, 1, 229, 197, 1, 230, 198, 1, 231, 199, 1, 232, 200, 1, 233, 201, 1, 234, 202, 1, 235, 203, 1, 236, 204, 1, 237, 205, 1
, 238, 206, 1, 239, 207, 1, 240, 208, 1, 241, 209, 1, 242, 210, 1, 243, 211, 1, 244, 212, 1, 245, 213, 1, 246, 214, 1, 248, 216, 1
, 249, 217, 1, 250, 218, 1, 251, 219, 1, 252, 220, 1, 253, 221, 1, 254, 222, 1, 255, 376, 1, 256, 257, 2, 257, 256, 1, 258, 259, 2
, 259, 258, 1, 260, 261, 2, 261, 260, 1, 262, 263, 2, 263, 262, 1, 264, 265, 2, 265, 264, 1, 266, 267, 2, 267, 266, 1, 268, 269, 2
, 269, 268, 1, 270, 271, 2, 271, 270, 1, 272, 273, 2, 273, 272, 1, 274, 275, 2, 275, 274, 1, 276, 277, 2, 277, 276, 1, 278, 279, 2
, 279, 278, 1, 280, 281, 2, 281, 280, 1, 282, 283, 2, 283, 282, 1, 284, 285, 2, 285, 284, 1, 286, 287, 2, 287, 286, 1, 288, 289, 2
, 289, 288, 1, 290, 291, 2, 291, 290, 1, 292, 293, 2, 293, 292, 1, 294, 295, 2, 295, 294, 1, 296, 297, 2, 297, 296, 1, 298, 299, 2
, 299, 298, 1, 300, 301, 2, 301, 300, 1, 302, 303, 2, 303, 302, 1, 304, 105, 2, 305, 73, 1, 306, 307, 2, 307, 306, 1, 308, 309, 2
, 309, 308, 1, 310, 311, 2, 311, 310, 1, 312, 312, 1, 313, 314, 2, 314, 313, 1, 315, 316, 2, 316, 315, 1, 317, 318, 2, 318, 317, 1
, 319, 320, 2, 320, 319, 1, 321, 322, 2, 322, 321, 1, 323, 324, 2, 324, 323, 1, 325, 326, 2, 326, 325, 1, 327, 328, 2, 328, 327, 1
, 329, 329, 1, 330, 331, 2, 331, 330, 1, 332, 333, 2, 333, 332, 1, 334, 335, 2, 335, 334, 1, 336, 337, 2, 337, 336, 1, 338, 339, 2
, 339, 338, 1, 340, 341, 2, 341, 340, 1, 342, 343, 2, 343, 342, 1, 344, 345, 2, 345, 344, 1, 346, 347, 2, 347, 346, 1, 348, 349, 2
, 349, 348, 1, 350, 351, 2, 351, 350, 1, 352, 353, 2, 353, 352, 1, 354, 355, 2, 355, 354, 1, 356, 357, 2, 357, 356, 1, 358, 359, 2
, 359, 358, 1, 360, 361, 2, 361, 360, 1, 362, 363, 2, 363, 362, 1, 364, 365, 2, 365, 364, 1, 366, 367, 2, 367, 366, 1, 368, 369, 2
, 369, 368, 1, 370, 371, 2, 371, 370, 1, 372, 373, 2, 373, 372, 1, 374, 375, 2, 375, 374, 1, 376, 255, 2, 377, 378, 2, 378, 377, 1
, 379, 380, 2, 380, 379, 1, 381, 382, 2, 382, 381, 1, 383, 83, 1, 384, 579, 1, 385, 595, 2, 386, 387, 2, 387, 386, 1, 388, 389, 2
, 389, 388, 1, 390, 596, 2, 391, 392, 2, 392, 391, 1, 393, 598, 2, 394, 599, 2, 395, 396, 2, 396, 395, 1, 397, 397, 1, 398, 477, 2
, 399, 601, 2, 400, 603, 2, 401, 402, 2, 402, 401, 1, 403, 608, 2, 404, 611, 2, 405, 502, 1, 406, 617, 2, 407, 616, 2, 408, 409, 2
, 409, 408, 1, 410, 573, 1, 411, 411, 1, 412, 623, 2, 413, 626, 2, 414, 544, 1, 415, 629, 2, 416, 417, 2, 417, 416, 1, 418, 419, 2
, 419, 418, 1, 420, 421, 2, 421, 420, 1, 422, 640, 2, 423, 424, 2, 424, 423, 1, 425, 643, 2, 426, 426, 1, 427, 427, 1, 428, 429, 2
, 429, 428, 1, 430, 648, 2, 431, 432, 2, 432, 431, 1, 433, 650, 2, 434, 651, 2, 435, 436, 2, 436, 435, 1, 437, 438, 2, 438, 437, 1
, 439, 658, 2, 440, 441, 2, 441, 440, 1, 442, 442, 1, 443, 443, 0, 444, 445, 2, 445, 444, 1, 446, 446, 1, 447, 503, 1, 448, 448, 0
, 449, 449, 0, 450, 450, 0, 451, 451, 0, 452, 454, 2, 453, 453, 0, 454, 452, 1, 455, 457, 2, 456, 456, 0, 457, 455, 1, 458, 460, 2
, 459, 459, 0, 460, 458, 1, 461, 462, 2, 462, 461, 1, 463, 464, 2, 464, 463, 1, 465, 466, 2, 466, 465, 1, 467, 468, 2, 468, 467, 1
, 469, 470, 2, 470, 469, 1, 471, 472, 2, 472, 471, 1, 473, 474, 2, 474, 473, 1, 475, 476, 2, 476, 475, 1, 477, 398, 1, 478, 479, 2
, 479, 478, 1, 480, 481, 2, 481, 480, 1, 482, 483, 2, 483, 482, 1, 484, 485, 2, 485, 484, 1, 486, 487, 2, 487, 486, 1, 488, 489, 2
, 489, 488, 1, 490, 491, 2, 491, 490, 1, 492, 493, 2, 493, 492, 1, 494, 495, 2, 495, 494, 1, 496, 496, 1, 497, 499, 2, 498, 498, 0
, 499, 497, 1, 500, 501, 2, 501, 500, 1, 502, 405, 2, 503, 447, 2, 504, 505, 2, 505, 504, 1, 506, 507, 2, 507, 506, 1, 508, 509, 2
, 509, 508, 1, 510, 511, 2, 511, 510, 1, 512, 513, 2, 513, 512, 1, 514, 515, 2, 515, 514, 1, 516, 517, 2, 517, 516, 1, 518, 519, 2
, 519, 518, 1, 520, 521, 2, 521, 520, 1, 522, 523, 2, 523, 522, 1, 524, 525, 2, 525, 524, 1, 526, 527, 2, 527, 526, 1, 528, 529, 2
, 529, 528, 1, 530, 531, 2, 531, 530, 1, 532, 533, 2, 533, 532, 1, 534, 535, 2, 535, 534, 1, 536, 537, 2, 537, 536, 1, 538, 539, 2
, 539, 538, 1, 540, 541, 2, 541, 540, 1, 542, 543, 2, 543, 542, 1, 544, 414, 2, 545, 545, 1, 546, 547, 2, 547, 546, 1, 548, 549, 2
, 549, 548, 1, 550, 551, 2, 551, 550, 1, 552, 553, 2, 553, 552, 1, 554, 555, 2, 555, 554, 1, 556, 557, 2, 557, 556, 1, 558, 559, 2
, 559, 558, 1, 560, 561, 2, 561, 560, 1, 562, 563, 2, 563, 562, 1, 564, 564, 1, 565, 565, 1, 566, 566, 1, 567, 567, 1, 568, 568, 1
, 569, 569, 1, 570, 11365, 2, 571, 572, 2, 572, 571, 1, 573, 410, 2, 574, 11366, 2, 575, 11390, 1, 576, 11391, 1, 577, 578, 2, 578, 577, 1
, 579, 384, 2, 580, 649, 2, 581, 652, 2, 582, 583, 2, 583, 582, 1, 584, 585, 2, 585, 584, 1, 586, 587, 2, 587, 586, 1, 588, 589, 2
, 589, 588, 1, 590, 591, 2, 591, 590, 1, 592, 11375, 1, 593, 11373, 1, 594, 11376, 1, 595, 385, 1, 596, 390, 1, 597, 597, 1, 598, 393, 1
, 599, 394, 1, 600, 600, 1, 601, 399, 1, 602, 602, 1, 603, 400, 1, 604, 604, 1, 605, 605, 1, 606, 606, 1, 607, 607, 1, 608, 403, 1
, 609, 609, 1, 610, 610, 1, 611, 404, 1, 612, 612, 1, 613, 613, 1, 614, 614, 1, 615, 615, 1, 616, 407, 1, 617, 406, 1, 618, 618, 1
, 619, 11362, 1, 620, 620, 1, 621, 621, 1, 622, 622, 1, 623, 412, 1, 624, 624, 1, 625, 11374, 1, 626, 413, 1, 627, 627, 1, 628, 628, 1
, 629, 415, 1, 630, 630, 1, 631, 631, 1, 632, 632, 1, 633, 633, 1, 634, 634, 1, 635, 635, 1, 636, 636, 1, 637, 11364, 1, 638, 638, 1
, 639, 639, 1, 640, 422, 1, 641, 641, 1, 642, 642, 1, 643, 425, 1, 644, 644, 1, 645, 645, 1, 646, 646, 1, 647, 647, 1, 648, 430, 1
, 649, 580, 1, 650, 433, 1, 651, 434, 1, 652, 581, 1, 653, 653, 1, 654, 654, 1, 655, 655, 1, 656, 656, 1, 657, 657, 1, 658, 439, 1
, 659, 659, 1, 660, 660, 0, 661, 661, 1, 662, 662, 1, 663, 663, 1, 664, 664, 1, 665, 665, 1, 666, 666, 1, 667, 667, 1, 668, 668, 1
, 669, 669, 1, 670, 670, 1, 671, 671, 1, 672, 672, 1, 673, 673, 1, 674, 674, 1, 675, 675, 1, 676, 676, 1, 677, 677, 1, 678, 678, 1
, 679, 679, 1, 680, 680, 1, 681, 681, 1, 682, 682, 1, 683, 683, 1, 684, 684, 1, 685, 685, 1, 686, 686, 1, 687, 687, 1, 688, 688, 0
, 689, 689, 0, 690, 690, 0, 691, 691, 0, 692, 692, 0, 693, 693, 0, 694, 694, 0, 695, 695, 0, 696, 696, 0, 697, 697, 0, 698, 698, 0
, 699, 699, 0, 700, 700, 0, 701, 701, 0, 702, 702, 0, 703, 703, 0, 704, 704, 0, 705, 705, 0, 710, 710, 0, 711, 711, 0, 712, 712, 0
, 713, 713, 0, 714, 714, 0, 715, 715, 0, 716, 716, 0, 717, 717, 0, 718, 718, 0, 719, 719, 0, 720, 720, 0, 721, 721, 0, 736, 736, 0
, 737, 737, 0, 738, 738, 0, 739, 739, 0, 740, 740, 0, 748, 748, 0, 750, 750, 0, 880, 881, 2, 881, 880, 1, 882, 883, 2, 883, 882, 1
, 884, 884, 0, 886, 887, 2, 887, 886, 1, 890, 890, 0, 891, 1021, 1, 892, 1022, 1, 893, 1023, 1, 902, 940, 2, 904, 941, 2, 905, 942, 2
, 906, 943, 2, 908, 972, 2, 910, 973, 2, 911, 974, 2, 912, 912, 1, 913, 945, 2, 914, 946, 2, 915, 947, 2, 916, 948, 2, 917, 949, 2
, 918, 950, 2, 919, 951, 2, 920, 952, 2, 921, 953, 2, 922, 954, 2, 923, 955, 2, 924, 956, 2, 925, 957, 2, 926, 958, 2, 927, 959, 2
, 928, 960, 2, 929, 961, 2, 931, 963, 2, 932, 964, 2, 933, 965, 2, 934, 966, 2, 935, 967, 2, 936, 968, 2, 937, 969, 2, 938, 970, 2
, 939, 971, 2, 940, 902, 1, 941, 904, 1, 942, 905, 1, 943, 906, 1, 944, 944, 1, 945, 913, 1, 946, 914, 1, 947, 915, 1, 948, 916, 1
, 949, 917, 1, 950, 918, 1, 951, 919, 1, 952, 920, 1, 953, 921, 1, 954, 922, 1, 955, 923, 1, 956, 924, 1, 957, 925, 1, 958, 926, 1
, 959, 927, 1, 960, 928, 1, 961, 929, 1, 962, 931, 1, 963, 931, 1, 964, 932, 1, 965, 933, 1, 966, 934, 1, 967, 935, 1, 968, 936, 1
, 969, 937, 1, 970, 938, 1, 971, 939, 1, 972, 908, 1, 973, 910, 1, 974, 911, 1, 975, 983, 2, 976, 914, 1, 977, 920, 1, 978, 978, 2
, 979, 979, 2, 980, 980, 2, 981, 934, 1, 982, 928, 1, 983, 975, 1, 984, 985, 2, 985, 984, 1, 986, 987, 2, 987, 986, 1, 988, 989, 2
, 989, 988, 1, 990, 991, 2, 991, 990, 1, 992, 993, 2, 993, 992, 1, 994, 995, 2, 995, 994, 1, 996, 997, 2, 997, 996, 1, 998, 999, 2
, 999, 998, 1, 1000, 1001, 2, 1001, 1000, 1, 1002, 1003, 2, 1003, 1002, 1, 1004, 1005, 2, 1005, 1004, 1, 1006, 1007, 2, 1007, 1006, 1, 1008, 922, 1
, 1009, 929, 1, 1010, 1017, 1, 1011, 1011, 1, 1012, 952, 2, 1013, 917, 1, 1015, 1016, 2, 1016, 1015, 1, 1017, 1010, 2, 1018, 1019, 2, 1019, 1018, 1
, 1020, 1020, 1, 1021, 891, 2, 1022, 892, 2, 1023, 893, 2, 1024, 1104, 2, 1025, 1105, 2, 1026, 1106, 2, 1027, 1107, 2, 1028, 1108, 2, 1029, 1109, 2
, 1030, 1110, 2, 1031, 1111, 2, 1032, 1112, 2, 1033, 1113, 2, 1034, 1114, 2, 1035, 1115, 2, 1036, 1116, 2, 1037, 1117, 2, 1038, 1118, 2, 1039, 1119, 2
, 1040, 1072, 2, 1041, 1073, 2, 1042, 1074, 2, 1043, 1075, 2, 1044, 1076, 2, 1045, 1077, 2, 1046, 1078, 2, 1047, 1079, 2, 1048, 1080, 2, 1049, 1081, 2
, 1050, 1082, 2, 1051, 1083, 2, 1052, 1084, 2, 1053, 1085, 2, 1054, 1086, 2, 1055, 1087, 2, 1056, 1088, 2, 1057, 1089, 2, 1058, 1090, 2, 1059, 1091, 2
, 1060, 1092, 2, 1061, 1093, 2, 1062, 1094, 2, 1063, 1095, 2, 1064, 1096, 2, 1065, 1097, 2, 1066, 1098, 2, 1067, 1099, 2, 1068, 1100, 2, 1069, 1101, 2
, 1070, 1102, 2, 1071, 1103, 2, 1072, 1040, 1, 1073, 1041, 1, 1074, 1042, 1, 1075, 1043, 1, 1076, 1044, 1, 1077, 1045, 1, 1078, 1046, 1, 1079, 1047, 1
, 1080, 1048, 1, 1081, 1049, 1, 1082, 1050, 1, 1083, 1051, 1, 1084, 1052, 1, 1085, 1053, 1, 1086, 1054, 1, 1087, 1055, 1, 1088, 1056, 1, 1089, 1057, 1
, 1090, 1058, 1, 1091, 1059, 1, 1092, 1060, 1, 1093, 1061, 1, 1094, 1062, 1, 1095, 1063, 1, 1096, 1064, 1, 1097, 1065, 1, 1098, 1066, 1, 1099, 1067, 1
, 1100, 1068, 1, 1101, 1069, 1, 1102, 1070, 1, 1103, 1071, 1, 1104, 1024, 1, 1105, 1025, 1, 1106, 1026, 1, 1107, 1027, 1, 1108, 1028, 1, 1109, 1029, 1
, 1110, 1030, 1, 1111, 1031, 1, 1112, 1032, 1, 1113, 1033, 1, 1114, 1034, 1, 1115, 1035, 1, 1116, 1036, 1, 1117, 1037, 1, 1118, 1038, 1, 1119, 1039, 1
, 1120, 1121, 2, 1121, 1120, 1, 1122, 1123, 2, 1123, 1122, 1, 1124, 1125, 2, 1125, 1124, 1, 1126, 1127, 2, 1127, 1126, 1, 1128, 1129, 2, 1129, 1128, 1
, 1130, 1131, 2, 1131, 1130, 1, 1132, 1133, 2, 1133, 1132, 1, 1134, 1135, 2, 1135, 1134, 1, 1136, 1137, 2, 1137, 1136, 1, 1138, 1139, 2, 1139, 1138, 1
, 1140, 1141, 2, 1141, 1140, 1, 1142, 1143, 2, 1143, 1142, 1, 1144, 1145, 2, 1145, 1144, 1, 1146, 1147, 2, 1147, 1146, 1, 1148, 1149, 2, 1149, 1148, 1
, 1150, 1151, 2, 1151, 1150, 1, 1152, 1153, 2, 1153, 1152, 1, 1162, 1163, 2, 1163, 1162, 1, 1164, 1165, 2, 1165, 1164, 1, 1166, 1167, 2, 1167, 1166, 1
, 1168, 1169, 2, 1169, 1168, 1, 1170, 1171, 2, 1171, 1170, 1, 1172, 1173, 2, 1173, 1172, 1, 1174, 1175, 2, 1175, 1174, 1, 1176, 1177, 2, 1177, 1176, 1
, 1178, 1179, 2, 1179, 1178, 1, 1180, 1181, 2, 1181, 1180, 1, 1182, 1183, 2, 1183, 1182, 1, 1184, 1185, 2, 1185, 1184, 1, 1186, 1187, 2, 1187, 1186, 1
, 1188, 1189, 2, 1189, 1188, 1, 1190, 1191, 2, 1191, 1190, 1, 1192, 1193, 2, 1193, 1192, 1, 1194, 1195, 2, 1195, 1194, 1, 1196, 1197, 2, 1197, 1196, 1
, 1198, 1199, 2, 1199, 1198, 1, 1200, 1201, 2, 1201, 1200, 1, 1202, 1203, 2, 1203, 1202, 1, 1204, 1205, 2, 1205, 1204, 1, 1206, 1207, 2, 1207, 1206, 1
, 1208, 1209, 2, 1209, 1208, 1, 1210, 1211, 2, 1211, 1210, 1, 1212, 1213, 2, 1213, 1212, 1, 1214, 1215, 2, 1215, 1214, 1, 1216, 1231, 2, 1217, 1218, 2
, 1218, 1217, 1, 1219, 1220, 2, 1220, 1219, 1, 1221, 1222, 2, 1222, 1221, 1, 1223, 1224, 2, 1224, 1223, 1, 1225, 1226, 2, 1226, 1225, 1, 1227, 1228, 2
, 1228, 1227, 1, 1229, 1230, 2, 1230, 1229, 1, 1231, 1216, 1, 1232, 1233, 2, 1233, 1232, 1, 1234, 1235, 2, 1235, 1234, 1, 1236, 1237, 2, 1237, 1236, 1
, 1238, 1239, 2, 1239, 1238, 1, 1240, 1241, 2, 1241, 1240, 1, 1242, 1243, 2, 1243, 1242, 1, 1244, 1245, 2, 1245, 1244, 1, 1246, 1247, 2, 1247, 1246, 1
, 1248, 1249, 2, 1249, 1248, 1, 1250, 1251, 2, 1251, 1250, 1, 1252, 1253, 2, 1253, 1252, 1, 1254, 1255, 2, 1255, 1254, 1, 1256, 1257, 2, 1257, 1256, 1
, 1258, 1259, 2, 1259, 1258, 1, 1260, 1261, 2, 1261, 1260, 1, 1262, 1263, 2, 1263, 1262, 1, 1264, 1265, 2, 1265, 1264, 1, 1266, 1267, 2, 1267, 1266, 1
, 1268, 1269, 2, 1269, 1268, 1, 1270, 1271, 2, 1271, 1270, 1, 1272, 1273, 2, 1273, 1272, 1, 1274, 1275, 2, 1275, 1274, 1, 1276, 1277, 2, 1277, 1276, 1
, 1278, 1279, 2, 1279, 1278, 1, 1280, 1281, 2, 1281, 1280, 1, 1282, 1283, 2, 1283, 1282, 1, 1284, 1285, 2, 1285, 1284, 1, 1286, 1287, 2, 1287, 1286, 1
, 1288, 1289, 2, 1289, 1288, 1, 1290, 1291, 2, 1291, 1290, 1, 1292, 1293, 2, 1293, 1292, 1, 1294, 1295, 2, 1295, 1294, 1, 1296, 1297, 2, 1297, 1296, 1
, 1298, 1299, 2, 1299, 1298, 1, 1300, 1301, 2, 1301, 1300, 1, 1302, 1303, 2, 1303, 1302, 1, 1304, 1305, 2, 1305, 1304, 1, 1306, 1307, 2, 1307, 1306, 1
, 1308, 1309, 2, 1309, 1308, 1, 1310, 1311, 2, 1311, 1310, 1, 1312, 1313, 2, 1313, 1312, 1, 1314, 1315, 2, 1315, 1314, 1, 1316, 1317, 2, 1317, 1316, 1
, 1329, 1377, 2, 1330, 1378, 2, 1331, 1379, 2, 1332, 1380, 2, 1333, 1381, 2, 1334, 1382, 2, 1335, 1383, 2, 1336, 1384, 2, 1337, 1385, 2, 1338, 1386, 2
, 1339, 1387, 2, 1340, 1388, 2, 1341, 1389, 2, 1342, 1390, 2, 1343, 1391, 2, 1344, 1392, 2, 1345, 1393, 2, 1346, 1394, 2, 1347, 1395, 2, 1348, 1396, 2
, 1349, 1397, 2, 1350, 1398, 2, 1351, 1399, 2, 1352, 1400, 2, 1353, 1401, 2, 1354, 1402, 2, 1355, 1403, 2, 1356, 1404, 2, 1357, 1405, 2, 1358, 1406, 2
, 1359, 1407, 2, 1360, 1408, 2, 1361, 1409, 2, 1362, 1410, 2, 1363, 1411, 2, 1364, 1412, 2, 1365, 1413, 2, 1366, 1414, 2, 1369, 1369, 0, 1377, 1329, 1
, 1378, 1330, 1, 1379, 1331, 1, 1380, 1332, 1, 1381, 1333, 1, 1382, 1334, 1, 1383, 1335, 1, 1384, 1336, 1, 1385, 1337, 1, 1386, 1338, 1, 1387, 1339, 1
, 1388, 1340, 1, 1389, 1341, 1, 1390, 1342, 1, 1391, 1343, 1, 1392, 1344, 1, 1393, 1345, 1, 1394, 1346, 1, 1395, 1347, 1, 1396, 1348, 1, 1397, 1349, 1
, 1398, 1350, 1, 1399, 1351, 1, 1400, 1352, 1, 1401, 1353, 1, 1402, 1354, 1, 1403, 1355, 1, 1404, 1356, 1, 1405, 1357, 1, 1406, 1358, 1, 1407, 1359, 1
, 1408, 1360, 1, 1409, 1361, 1, 1410, 1362, 1, 1411, 1363, 1, 1412, 1364, 1, 1413, 1365, 1, 1414, 1366, 1, 1415, 1415, 1 };

static const int maxtable = 1078;

static int ponctuations[] = { 0x21, 0x22, 0x23, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E,
0x3F, 0x40, 0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x7B, 0x7C, 0x7D, 0x7E, 0x9C, 0xA0, 0xA1, 0xA2, 0xA4, 0xA5, 0xA6, 0xAA, 0xAB, 0xAC, 0xAD,
0xAF, 0xB0, 0xB1, 0xB5, 0xB6, 0xB7, 0xB8, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xD7, 0xF7, 0x2BC, 0x2013, 0x2014, 0x2015, 0x2018,
0x2019, 0x201C, 0x201D, 0x2020, 0x2021, 0x2022, 0x2026, 0x2032, 0x2033, 0x203B, 0x212E, 0x2190, 0x2191, 0x2192, 0x2193, 0x2264,
0x2265, 0x263A, 0x3008, 0x3009, 0 };

static hmap<string, string> utf8min;
static hmap<string, string> utf8maj;
static map<BULONG, BULONG> utf8codemin;
static map<BULONG, BULONG> utf8codemaj;
static hmap<int, string> latinmin;
static hmap<int, string> latinmaj;
static map<BULONG, bool> punctuations;
static hmap<int, int> latincodes;
static hmap<string, int> utf8codes;
static hmap<int, string> vowels;
static hmap<int, string> consonants;

Exported int c_unicodetoutf8(BULONG code, unsigned char* utf) {
	if (code < 0x0080) {
		utf[0] = (unsigned char)code;
		utf[1] = 0;
		return 1;
	}
	if (code < 0x0800) {
		utf[0] = 0xc0 | ((code) >> 6);
		utf[1] = 0x80 | ((code)& 0x3f);
		utf[2] = 0;
		return 2;
	}
	if (code < 0x10000) {
		utf[0] = 0xe0 | ((code) >> 12);
		utf[1] = 0x80 | (((code) >> 6) & 0x3f);
		utf[2] = 0x80 | ((code)& 0x3f);
		utf[3] = 0;
		return 3;
	}
	if (code <0x200000) {
		utf[0] = 0xF0 | ((code) >> 18);
		utf[1] = 0x80 | (((code) >> 12) & 0x3f);
		utf[2] = 0x80 | (((code) >> 6) & 0x3f);
		utf[3] = 0x80 | ((code)& 0x3f);
		utf[4] = 0;
		return 4;
	}
	if (code <0x4000000) {
		utf[0] = 0xF8 | ((code) >> 24);
		utf[1] = 0x80 | (((code) >> 18) & 0x3f);
		utf[2] = 0x80 | (((code) >> 12) & 0x3f);
		utf[3] = 0x80 | (((code) >> 6) & 0x3f);
		utf[4] = 0x80 | ((code)& 0x3f);
		utf[5] = 0;
		return 5;
	}
	utf[0] = 0xFC | ((code) >> 30);
	utf[1] = 0x80 | (((code) >> 24) & 0x3f);
	utf[2] = 0x80 | (((code) >> 18) & 0x3f);
	utf[3] = 0x80 | (((code) >> 12) & 0x3f);
	utf[4] = 0x80 | (((code) >> 6) & 0x3f);
	utf[5] = 0x80 | ((code)& 0x3f);
	utf[6] = 0;
	return 6;
}

Exported string s_unicodetoutf8(BULONG code) {
	char utf[10];
	if (code < 0x0080) {
		utf[0] = (unsigned char)code;
		utf[1] = 0;
		return utf;
	}
	if (code < 0x0800) {
		utf[0] = 0xc0 | ((code) >> 6);
		utf[1] = 0x80 | ((code)& 0x3f);
		utf[2] = 0;
		return utf;
	}
	if (code < 0x10000) {
		utf[0] = 0xe0 | ((code) >> 12);
		utf[1] = 0x80 | (((code) >> 6) & 0x3f);
		utf[2] = 0x80 | ((code)& 0x3f);
		utf[3] = 0;
		return utf;
	}
	if (code <0x200000) {
		utf[0] = 0xF0 | ((code) >> 18);
		utf[1] = 0x80 | (((code) >> 12) & 0x3f);
		utf[2] = 0x80 | (((code) >> 6) & 0x3f);
		utf[3] = 0x80 | ((code)& 0x3f);
		utf[4] = 0;
		return utf;
	}
	if (code <0x4000000) {
		utf[0] = 0xF8 | ((code) >> 24);
		utf[1] = 0x80 | (((code) >> 18) & 0x3f);
		utf[2] = 0x80 | (((code) >> 12) & 0x3f);
		utf[3] = 0x80 | (((code) >> 6) & 0x3f);
		utf[4] = 0x80 | ((code)& 0x3f);
		utf[5] = 0;
		return utf;
	}
	utf[0] = 0xFC | ((code) >> 30);
	utf[1] = 0x80 | (((code) >> 24) & 0x3f);
	utf[2] = 0x80 | (((code) >> 18) & 0x3f);
	utf[4] = 0x80 | (((code) >> 12) & 0x3f);
	utf[5] = 0x80 | (((code) >> 6) & 0x3f);
	utf[6] = 0x80 | ((code)& 0x3f);
	utf[7] = 0;
	return utf;
}

Exported void s_unicodetoutf8(BULONG code, char* utf) {
	if (code < 0x0080) {
		utf[0] = (unsigned char)code;
		utf[1] = 0;
		return;
	}
	if (code < 0x0800) {
		utf[0] = 0xc0 | ((code) >> 6);
		utf[1] = 0x80 | ((code)& 0x3f);
		utf[2] = 0;
		return;
	}
	if (code < 0x10000) {
		utf[0] = 0xe0 | ((code) >> 12);
		utf[1] = 0x80 | (((code) >> 6) & 0x3f);
		utf[2] = 0x80 | ((code)& 0x3f);
		utf[3] = 0;
		return;
	}
	if (code <0x200000) {
		utf[0] = 0xF0 | ((code) >> 18);
		utf[1] = 0x80 | (((code) >> 12) & 0x3f);
		utf[2] = 0x80 | (((code) >> 6) & 0x3f);
		utf[3] = 0x80 | ((code)& 0x3f);
		utf[4] = 0;
		return;
	}
	if (code <0x4000000) {
		utf[0] = 0xF8 | ((code) >> 24);
		utf[1] = 0x80 | (((code) >> 18) & 0x3f);
		utf[2] = 0x80 | (((code) >> 12) & 0x3f);
		utf[3] = 0x80 | (((code) >> 6) & 0x3f);
		utf[4] = 0x80 | ((code)& 0x3f);
		utf[5] = 0;
		return;
	}
	utf[0] = 0xFC | ((code) >> 30);
	utf[1] = 0x80 | (((code) >> 24) & 0x3f);
	utf[2] = 0x80 | (((code) >> 18) & 0x3f);
	utf[4] = 0x80 | (((code) >> 12) & 0x3f);
	utf[5] = 0x80 | (((code) >> 6) & 0x3f);
	utf[6] = 0x80 | ((code)& 0x3f);
	utf[7] = 0;
}

Exported void c_latintoutf8(unsigned char code, unsigned char* utf) {
	if (code < 0x0080) {
		utf[0] = (unsigned char)code;
		utf[1] = 0;
		return;
	}
	if (latincodes.find(code) != latincodes.end())
		c_unicodetoutf8(latincodes[code], utf);
	else {
		utf[0] = 0xc0 | ((code) >> 6);
		utf[1] = 0x80 | ((code)& 0x3f);
		utf[2] = 0;
	}
}

Exported bool c_utf8tolatin(unsigned char* utf, unsigned char& code) {
	if (utf[0] < 0x0080) {
		code = utf[0];
		return true;
	}

	if (utf8codes.find((char*)utf) != utf8codes.end()) {
		int x = utf8codes[(char*)utf];
		code = (unsigned char)x;
		return true;
	}

	unsigned char c = utf[0];
	if ((c & 0xE0) == 0xC0) {
		//We take two bytes
		if ((utf[1] & 0xC0) != 0x80)
			return false;
		code = (c ^ 0xc0) << 6;
		code |= utf[1] ^ 0x80;
		return true;
	}
	//we take three
	if ((c & 0xF0) == 0xE0) {
		if ((utf[1] & 0xC0) != 0x80 || (utf[2] & 0xC0) != 0x80)
			return false;
		code = (c ^ 0xE0) << 12;
		code |= (utf[1] ^ 0x80) << 6;
		code |= (utf[2] ^ 0x80);
		return true;
	}
	return false;
}

Exported int c_utf8tounicode(unsigned char* utf, BULONG& code) {
	code = utf[0];
	if (utf[0] < 0x0080)
		return 0;

	unsigned char c = utf[0];
	if ((c & 0xE0) == 0xC0) {
		//We take two bytes
		if ((utf[1] & 0xC0) != 0x80)
			return 0;
		code = (c ^ 0xc0) << 6;
		code |= utf[1] ^ 0x80;
		return 1;
	}
	//we take three
	if ((c & 0xF0) == 0xE0) {
		if ((utf[1] & 0xC0) != 0x80 || (utf[2] & 0xC0) != 0x80)
			return 0;
		code = (c ^ 0xE0) << 12;
		code |= (utf[1] ^ 0x80) << 6;
		code |= (utf[2] ^ 0x80);
		return 2;
	}

	int i;
	//we take four
	if ((c & 0xF8) == 0xF0) {
		for (i = 1; i <= 3; i++)
		if ((utf[i] & 0xC0) != 0x80)
			return 0;
		code = (c ^ 0xF0) << 18;
		code |= (utf[1] ^ 0x80) << 12;
		code |= (utf[2] ^ 0x80) << 6;
		code |= (utf[3] ^ 0x80);
		return 3;
	}

	//we take five
	if ((c & 0xFC) == 0xF8) {
		for (i = 1; i <= 4; i++)
		if ((utf[i] & 0xC0) != 0x80)
			return 0;
		code = (c ^ 0xF8) << 24;
		code |= (utf[1] ^ 0x80) << 18;
		code |= (utf[2] ^ 0x80) << 12;
		code |= (utf[3] ^ 0x80) << 6;
		code |= (utf[4] ^ 0x80);
		return 4;
	}

	//we take six
	if ((c & 0xFF) == 0xFC) {
		for (i = 1; i <= 5; i++)
		if ((utf[i] & 0xC0) != 0x80)
			return 0;
		code = (c ^ 0xFC) << 30;
		code |= (utf[1] ^ 0x80) << 24;
		code |= (utf[2] ^ 0x80) << 18;
		code |= (utf[3] ^ 0x80) << 12;
		code |= (utf[4] ^ 0x80) << 6;
		code |= (utf[5] ^ 0x80);
		return 5;
	}
	return 0;
}

Exported void s_unicodetoutf8(string& s, wstring& str) {
	int i = 0;
	char inter[10];

	while (i<str.size()) {
		if (str[i] < 0x0080)
			s += (char)str[i];
		else {
			c_unicodetoutf8(str[i], (uchar*)inter);
			s += inter;
		}
		i++;
	}
}


Exported void s_unicodetoutf8(string& s, wchar_t* str) {
	int i = 0;
	char inter[10];

	while (str[i] != 0) {
		if (str[i] < 0x0080)
			s += (char)str[i];
		else {
			c_unicodetoutf8(str[i], (uchar*)inter);
			s += inter;
		}
		i++;
	}
}

Exported void s_utf8tounicode(wstring& s, char* str) {
	int i = 0;
	BULONG c;
	while (str[i] != 0) {
		i += c_utf8tounicode((uchar*)str + i, c) + 1;
		s += (wchar_t)c;
	}
}

Exported void s_doubleutf8tounicode(wstring& s, wchar_t* str, long l) {
	string sutf8;
	for (int i = 0; i < l; i++) {
		char lc = str[i] & 0xFF;
		char lr = (str[i] & 0xFF00) >> 8;
		sutf8 += lc;
		sutf8 += lr;
	}
	s_utf8tounicode(s, STR(sutf8));
}


Exported void s_doubleutf8tounicode(wstring& s, wchar_t* str) {
	int i = 0;
	string sutf8;
	while (str[i] != 0) {
		char lc = str[i] & 0xFF;
		char lr = (str[i] & 0xFF00) >> 8;
		sutf8 += lc;
		sutf8 += lr;
		i++;
	}
	s_utf8tounicode(s, STR(sutf8));
}

Exported char c_testutf8(unsigned char* utf) {
	if (utf == NULL || utf[0] < 0x0080)
		return 0;
	unsigned char c = utf[0];
	if ((c & 0xE0) == 0xC0) {
		//We take two bytes
		if ((utf[1] & 0xC0) != 0x80)
			return 0;
		return 1;
	}
	//we take three
	if ((c & 0xF0) == 0xE0) {
		if ((utf[1] & 0xC0) != 0x80 || (utf[2] & 0xC0) != 0x80)
			return 0;
		return 2;
	}

	int i;
	//we take four
	if ((c & 0xF8) == 0xF0) {
		for (i = 1; i <= 3; i++)
		if ((utf[i] & 0xC0) != 0x80)
			return 0;
		return 3;
	}

	//we take five
	if ((c & 0xFC) == 0xF8) {
		for (i = 1; i <= 4; i++)
		if ((utf[i] & 0xC0) != 0x80)
			return 0;
		return 4;
	}

	//we take six
	if ((c & 0xFF) == 0xFC) {
		for (i = 1; i <= 5; i++)
		if ((utf[i] & 0xC0) != 0x80)
			return 0;
		return 5;
	}
	return 0;
}


Exported  int conversionUTF8VersGras(unsigned char* contenu, vector<string>& l) {
	int firstmul = -1;
	int lg = strlen((char*)contenu);
	l.reserve(lg);
	string res;
	for (int i = 0; i<lg; i++) {
		res = c_char_get(contenu, i);
		if (firstmul == -1 && res.size()>1)
			firstmul = res.size();
		l.push_back(res);
	}
	return firstmul;
}

Exported void c_get_utf8_char(short code, string& v) {
	bulongchar xs;
	c_unicodetoutf8(code, xs.ustr);
	v = xs.str;
}

Exported unsigned char conversionUTF8ToLatinOne(short x) {
	if (x >= 32)
		return x;
	return 32;
}


//convert a character position into a byte position
Exported  int c_chartobyteposition(unsigned char* contenu, int charpos) {
	int i = 0;
	while (charpos>0) {
		i += 1 + c_testutf8(contenu + i);
		charpos--;
	}
	return i;
}

Exported  int c_bytetocharposition(unsigned char* contenu, int charpos) {
	int i = 0;
	int nb = 0;
	while (i<charpos) {
		i += 1 + c_testutf8(contenu + i);
		nb++;
	}
	return nb;
}

Exported int c_bytetocharpositionidx(unsigned char* contenu, int charpos, int& nb, int& i) {
	while (i<charpos) {
		i += 1 + c_testutf8(contenu + i);
		nb++;
	}
	return nb;
}

Exported  int c_chartobytepositionidx(unsigned char* contenu, int charpos, int& nb, int& i) {
	while (charpos>0) {
		i += 1 + c_testutf8(contenu + i);
		nb++;
		charpos--;
	}
	return i;
}

Exported  void conversionUTF8VersGrasOne(unsigned char* contenu, string& s) {
	int i = 0;
	s = c_char_get(contenu, i);
}


Exported  int conversionUTF8VersGras(string contenu, vector<string>& l) {
	return conversionUTF8VersGras((unsigned char*)contenu.c_str(), l);
}

Exported  void conversionUTF8VersGrasOne(string contenu, string& s) {
	conversionUTF8VersGrasOne((unsigned char*)contenu.c_str(), s);
}

Exported  string conversionUTF8VersLatin(unsigned char* contenu) {
	int longueur = strlen((char*)contenu);
	string res;
	char cc = 0;
	uchar codelatin;
	int nb;
	for (int i = 0; i<longueur; i++) {
		nb = c_testutf8(contenu + i);
		if (nb == 0)
			res += contenu[i];
		else {
			cc = contenu[i + nb + 1];
			contenu[i + nb + 1] = 0;
			c_utf8tolatin(contenu + i, codelatin);
			res += (char)codelatin;
			contenu[i + nb + 1] = cc;
			i += nb;
		}
	}
	return res;
}

Exported  string conversionLatinVersUTF8(unsigned char* contenu) {
	int longueur = strlen((char*)contenu);
	string res;
	int nb;
	bulongchar xs;
	for (int i = 0; i<longueur; i++) {
		if (contenu[i]<127)
			res += contenu[i];
		else {
			nb = c_testutf8(contenu + i);
			if (nb != 0) {
				char cc = contenu[i + nb + 1];
				contenu[i + nb + 1] = 0;
				res += (char*)(contenu + i);
				contenu[i + nb + 1] = cc;
				i += nb;
			}
			else {
				xs.clear();
				c_latintoutf8(contenu[i], xs.ustr);
				res += xs.str;
			}
		}
	}
	return res;
}

Exported string s_utf8todos(unsigned char* contenu) {
	int longueur = strlen((char*)contenu);
	string res;
	string utf;
	char cc = 0;
	int nb;
	for (int i = 0; i < longueur; i++) {
		nb = c_testutf8(contenu + i);
		if (nb == 0) {
			nb = contenu[i];
			if (nb <= 127 || unicode2dos.find(nb) == unicode2dos.end())
				res += nb;
			else
				res += (char)unicode2dos[nb];
		}
		else {
			cc = contenu[i + nb + 1];
			contenu[i + nb + 1] = 0;
			utf = (char*)contenu + i;
			if (utf8todos.find(utf) != utf8todos.end())
				res += (unsigned char)utf8todos[utf];
			else
				res += "?";
			contenu[i + nb + 1] = cc;
			i += nb;
		}
	}
	return res;
}

Exported string s_utf8todos(const char* contenu) {
	string c(contenu);
	return s_utf8todos(USTR(c));
}

Exported string s_dostoutf8(unsigned char* contenu) {
	int longueur = strlen((char*)contenu);
	string res;
	int nb;
	bulongchar xs;
	for (int i = 0; i<longueur; i++) {
		if (contenu[i] <= 127)
			res += contenu[i];
		else
			res += dostoutf8[contenu[i]];
	}
	return res;
}

Exported string s_dostoutf8(const char* contenu) {
	string c(contenu);
	return s_dostoutf8(USTR(c));
}

Exported void s_latintoutf8(string& res, unsigned char* contenu) {
	res.clear();
	int longueur = strlen((char*)contenu);
	int nb;
	bulongchar xs;
	for (int i = 0; i<longueur; i++) {
		if (contenu[i]<127)
			res += contenu[i];
		else {
			nb = c_testutf8(contenu + i);
			if (nb != 0) {
				char cc = contenu[i + nb + 1];
				contenu[i + nb + 1] = 0;
				res += (char*)(contenu + i);
				contenu[i + nb + 1] = cc;
				i += nb;
			}
			else {
				xs.clear();
				c_latintoutf8(contenu[i], xs.ustr);
				res += xs.str;
			}
		}
	}
}

Exported  string conversionUTF8VersLatin(string contenu) {
	return conversionUTF8VersLatin((unsigned char*)contenu.c_str());
}

Exported  string conversionLatinVersUTF8(string contenu) {
	return conversionLatinVersUTF8((unsigned char*)contenu.c_str());
}

Exported string c_replacestring(string& s, string& reg, string& rep) {
	int gsz = reg.size();
	if (!gsz)
		return s;

	int rsz = s.size();
	size_t foundHere;
	string neo;
	size_t from = 0;

	while ((foundHere = s.find(reg, from)) != string::npos) {
		if (foundHere != from)
			neo += s.substr(from, foundHere - from);
		neo += rep;
		from = foundHere + gsz;
	}

	if (from<rsz)
		neo += s.substr(from, rsz - from);

	return neo;
}

Exported string c_creplacestring(string& s, string reg, string rep) {
	int gsz = reg.size();
	if (!gsz)
		return s;

	int rsz = s.size();
	size_t foundHere;
	string neo;
	size_t from = 0;

	while ((foundHere = s.find(reg, from)) != string::npos) {
		if (foundHere != from)
			neo += s.substr(from, foundHere - from);
		neo += rep;
		from = foundHere + gsz;
	}

	if (from<rsz)
		neo += s.substr(from, rsz - from);

	return neo;
}

Exported wstring c_replacewstring(wstring& s, wstring reg, wstring rep) {
	int gsz = reg.size();
	if (!gsz)
		return s;

	int rsz = s.size();
	size_t foundHere;
	wstring neo;
	size_t from = 0;

	while ((foundHere = s.find(reg, from)) != string::npos) {
		if (foundHere != from)
			neo += s.substr(from, foundHere - from);
		neo += rep;
		from = foundHere + gsz;
	}

	if (from<rsz)
		neo += s.substr(from, rsz - from);

	return neo;
}

Exported int c_char_next(unsigned char* m, int& i) {

	int nb = c_testutf8(m + i);
	i += nb;
	return (nb + 1);
}

Exported int c_code_get(unsigned char* m, int& i, BULONG& code) {
	int nb = c_utf8tounicode(m + i, code);
	i += nb;
	return nb;
}

Exported string c_char_get_next(unsigned char* m, int& i) {
	int nb = c_testutf8(m + i);
	char str[] = { (char)m[i], 0, 0, 0, 0, 0, 0, 0 };

	if (nb == 0) {
		i++;
		return str;
	}

	char cc = m[i + nb + 1];
	m[i + nb + 1] = 0;
	strcpy(str, (char*)(m + i));
	m[i + nb + 1] = cc;
	i += nb + 1;
	return str;
}

Exported  string c_char_get(unsigned char* m, int& i) {
	int nb = c_testutf8(m + i);
	char str[] = { (char)m[i], 0, 0, 0, 0, 0, 0, 0 };

	if (nb == 0)
		return str;

	char cc = m[i + nb + 1];
	m[i + nb + 1] = 0;
	strcpy(str, (char*)(m + i));
	m[i + nb + 1] = cc;
	i += nb;
	return str;
}

Exported  short c_char_get_wide(unsigned char* m, int& i) {
	BULONG code;
	i += c_utf8tounicode(m + i, code);
	return code;
}

Exported  int c_size(string& s) {
	if (s == "")
		return 0;
	int lg = s.size();
	int sz = 0;
	uchar* contenu = USTR(s);
	for (int i = 0; i<lg; i++) {
		i += c_testutf8(contenu + i);
		sz++;
	}
	return sz;
}

Exported  int c_size(const char* s) {
	if (s == NULL || s[0] == 0)
		return 0;
	int lg = strlen((char*)s);
	int sz = 0;
	for (int i = 0; i<lg; i++) {
		i += c_testutf8((uchar*)s + i);
		sz++;
	}
	return sz;
}

Exported  string c_left(string& s, int nb) {
	if (nb<0)
		return s;
	int lg = s.size();
	string res;
	for (int i = 0; i<lg && nb>0; i++) {
		res += c_char_get(USTR(s), i);
		nb--;
	}
	return res;
}

Exported  string c_right(string& s, int nb) {
	if (nb<0)
		return s;
	int lg = s.size();
	int l = c_size(s);
	if (nb >= l)
		nb = 0;
	else
		l = l - nb;
	string res;
	//we position on the first character
	int i;
	uchar* contenu = USTR(s);
	for (i = 0; i<lg && l>0; i++) {
		i += c_testutf8(contenu + i);
		l--;
	}

	for (; i<lg && nb >= 0; i++) {
		res += c_char_get(contenu, i);
		nb--;
	}
	return res;
}

Exported  string c_middle(string& s, int l, int nb) {
	int lg = s.size();
	string res;
	//we position on the first character
	int i;
	uchar* contenu = USTR(s);
	for (i = 0; i<lg && l>0; i++) {
		i += c_testutf8(contenu + i);
		l--;
	}

	for (; i<lg && nb>0; i++) {
		res += c_char_get(contenu, i);
		nb--;
	}
	return res;
}


Exported  bool c_char_index_assign(string& s, string c, int x) {
	if (x>s.size())
		return false;
	int i;
	uchar* contenu = USTR(s);
	int lg = s.size();
	for (i = 0; i<lg && x>0; i++) {
		i += c_testutf8(contenu + i);
		x--;
	}
	if (i == lg)
		s += c;
	else {
		x = i;
		s.erase(i, c_testutf8(contenu + x) + 1);
		s.insert(i, c);
	}
	return true;
}

Exported  bool c_char_index_insert(string& s, string c, size_t x) {
	if (x>s.size())
		return false;
	int i;
	int lg = s.size();
	uchar* contenu = USTR(s);
	for (i = 0; i<lg && x>0; i++) {
		i += c_testutf8(contenu + i);
		x--;
	}
	if (i == lg)
		s += c;
	else
		s.insert(i, c);
	return true;
}

Exported  string c_char_index_remove(string& s, size_t i) {
	int lg = s.size();
	if (i >= lg)
		return "";
	string res;
	int pos = 0;
	int x;
	uchar* contenu = USTR(s);
	for (x = 0; x<lg && pos != i; x++) {
		x += c_testutf8(contenu + x);
		pos++;
	}
	pos = x;
	res = c_char_get(USTR(s), pos);
	s.erase(x, res.size());
	return res;
}


Exported  string c_char_index(string& s, size_t i) {
	int lg = s.size();
	if (i >= lg)
		return "";
	int x;
	uchar* contenu = USTR(s);
	for (x = 0; x<lg && i>0; x++) {
		x += c_testutf8(contenu + x);
		i--;
	}
	if (x == lg)
		return "";

	return c_char_get(USTR(s), x);
}

Exported  string c_char_reverse(string& s, int& length) {
	int lg = s.size();
	string res = "";
	string x;
	length = 0;
	for (int i = 0; i<lg; i++) {
		x = c_char_get(USTR(s), i);
		res.insert(0, x);
		length++;
	}
	return res;
}

Exported string s_revert(string& s) {
	int lg = s.size();
	string res = "";
	string x;
	for (int i = 0; i<lg; i++) {
		x = c_char_get(USTR(s), i);
		res.insert(0, x);
	}
	return res;
}


//return 0 (not alpha), 1 (lower), 2 (upper)
Exported  char c_is_alpha(unsigned char* m, int& i) {
	int nb = c_testutf8(m + i);
	if (nb == 0) {
		if (latinmaj.find(m[i]) != latinmaj.end())
			return 2;

		if (latinmin.find(m[i]) != latinmin.end())
			return 1;
		return 0;
	}

	char cc = m[i + nb + 1];
	m[i + nb + 1] = 0;
	if (nb != -1) {
		char* s = (char*)(m + i);
		if (utf8maj.find(s) != utf8maj.end()) {
			m[i + nb + 1] = cc;
			i += nb;
			return 2;
		}

		if (utf8min.find(s) != utf8min.end()) {
			m[i + nb + 1] = cc;
			i += nb;
			return 1;
		}
		m[i + nb + 1] = cc;
		return 0;
	}

	m[i + nb + 1] = cc;
	if (latinmaj.find(m[i]) != latinmaj.end())
		return 2;

	if (latinmin.find(m[i]) != latinmin.end())
		return 1;
	return 0;
}

Exported  string c_insert_sep_byte(string& s, string sep) {
	string res;
	int lg = s.size();
	for (int i = 0; i<lg; i++) {
		if (i)
			res += sep;
		res += s[i];
	}
	return res;
}


Exported  string c_insert_sep(string s, string sep) {
	string res;
	int lg = s.size();
	bool beg = true;
	for (int i = 0; i<lg; i++) {
		if (beg == false)
			res += sep;
		beg = false;
		res += c_char_get(USTR(s), i);
	}
	return res;
}

Exported  string c_to_utf8(unsigned char* m) {
	return conversionLatinVersUTF8(m);
}

Exported bool c_is_utf8(unsigned char* contenu, int longueur) {
	for (int i = 0; i<longueur; i++) {
		if (contenu[i]<0xC0)
			continue;
		if (!c_testutf8(contenu + i))
			continue;
		return true;
	}
	return false;
}

//return:
//0: if it is latin
//1: if it is pure ASCII
//2: if it is UTF8

Exported char c_is_pure_utf8(unsigned char* contenu, int longueur) {
	char pure = 1;
	for (int i = 0; i<longueur; i++) {
		if (contenu[i]<0xC0) {
			if (contenu[i] >= 0x80)
				pure = 0;
			continue;
		}
		if (!c_testutf8(contenu + i))
			continue;
		return 2;
	}
	return pure;
}


Exported  BULONG c_char_index_code(string& s, size_t i) {
	BULONG lg = s.size();
	uchar* contenu = USTR(s);
	int x;
	for (x = 0; x<lg && i>0; x++) {
		x += c_testutf8(contenu + x);
		i--;
	}
	c_utf8tounicode(contenu + x, lg);
	return lg;
}

Exported  void c_char_index_code_all(string& s, vector<int>& vect) {
	int lg = s.size();
	uchar* contenu = USTR(s);
	vect.reserve(lg);
	BULONG code;
	for (int i = 0; i<lg; i++) {
		i += c_utf8tounicode(contenu + i, code);
		vect.push_back(code);
	}
}

Exported  void c_char_index_code_all_long(string& s, vector<long>& vect) {
	int lg = s.size();
	uchar* contenu = USTR(s);
	vect.reserve(lg);
	BULONG code;
	for (int i = 0; i<lg; i++) {
		i += c_utf8tounicode(contenu + i, code);
		vect.push_back(code);
	}
}


//On renvoie le code LATIN correspondant, 0 sinon
Exported  unsigned char c_utf8_latin(string s) {
	BULONG v;
	c_utf8tounicode(USTR(s), v);
	if (v>255)
		return 0;
	return (uchar)v;
}

//On renvoie la chaine UTF8 correspondante, 0 sinon
string c_latin_utf8(int c) {
	bulongchar xs;
	c_latintoutf8(c, xs.ustr);
	return xs.str;
}

Exported bool c_is_punctuation(string& str) {
	int lg = str.size();
	uchar* contenu = USTR(str);
	BULONG code;
	for (int i = 0; i<lg; i++) {
		i += c_utf8tounicode(contenu + i, code);
		if (punctuations.find(code) == punctuations.end())
			return false;
	}
	return true;
}

Exported  bool c_Isupper(string s) {
	if (s == "")
		return false;
	int lg = s.size();
	for (int i = 0; i<lg; i++) {
		char ty = c_is_alpha(USTR(s), i);
		if (ty != 2)
			return false;
	}
	return true;
}


Exported bool c_Isconsonants(unsigned char* s) {
	if (s[0] == 0)
		return false;
	int lg = strlen((char*)s);
	BULONG code;

	for (int i = 0; i<lg; i++) {
		c_code_get(s, i, code);
		if (consonants.find(code) == consonants.end())
			return false;
	}

	return true;
}

Exported  bool c_Isvowels(unsigned char* s) {
	if (s[0] == 0)
		return false;
	int lg = strlen((char*)s);
	BULONG code;

	for (int i = 0; i<lg; i++) {
		c_code_get(s, i, code);
		if (vowels.find(code) == vowels.end())
			return false;
	}

	return true;
}

static inline bool addbit(BULONG& code, BULONG& v, int& I, int& nb, int mx, int mxsz, char add) {
	if (add == 1)
		code |= v;
	v <<= 1;
	nb++;
	if (nb >= mxsz) {
		I++;
		nb = 0;
		v = 1;
		if (I >= mx)
			return false;
	}
	return true;
}

Exported void s_binary(unsigned char* s, int mx, int lg, BULONG* codes) {
	static map<char, bool> rares;
	static bool init = false;

	if (!init) {
		rares['k'] = true;
		rares['z'] = true;
		rares['w'] = true;
		rares['x'] = true;
		rares['q'] = true;
		rares['h'] = true;
		rares['j'] = true;
		rares['K'] = true;
		rares['Z'] = true;
		rares['W'] = true;
		rares['X'] = true;
		rares['Q'] = true;
		rares['H'] = true;
		rares['J'] = true;
		init = false;
	}

	BULONG code;
	int I = 0;
	BULONG v = 1;
	int nb;
	int mxsz = sizeof(BULONG);
	//z,w,x,q,h,j,k
	string c;

	for (int i = 0; i<lg; i++) {
		c_code_get(s, i, code);
		if (code <= 32)
			continue;
		if (vowels.find(code) != vowels.end()) { //0001
			if (!addbit(codes[I], v, I, nb, mx, mxsz, 1))
				break;
			if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
				break;
			if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
				break;
			if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
				break;
		}
		else {
			if (consonants.find(code) != consonants.end()) {//0010
				c = consonants[code];

				if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
					break;

				if (!addbit(codes[I], v, I, nb, mx, mxsz, 1))
					break;

				if (rares.find(c[0]) != rares.end()) {//0110
					if (!addbit(codes[I], v, I, nb, mx, mxsz, 1))
						break;
				}
				else
				if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
					break;

				if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
					break;
			}
			else { //1000
				if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
					break;
				if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
					break;
				if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
					break;
				if (!addbit(codes[I], v, I, nb, mx, mxsz, 1))
					break;
			}
		}
	}
}


Exported  string s_deaccentuate(unsigned char* s) {
	if (s[0] == 0)
		return "";

	int lg = strlen((char*)s);
	BULONG code;
	string v;
	int nb, I;
	char m[10];
	unsigned char c;

	for (int i = 0; i<lg; i++) {
		I = i + 1;
		nb = c_code_get(s, i, code);
		if (vowels.find(code) != vowels.end()) {
			v += vowels[code];
			continue;
		}
		if (consonants.find(code) != consonants.end()) {
			v += consonants[code];
			continue;
		}
		if (!nb)
			v += s[i];
		else {
			c = s[I + nb];
			s[I + nb] = 0;
			strcpy(m, (char*)s + I - 1);
			s[I + nb] = c;
			v += m;
		}
	}
	return v;
}

Exported  bool c_Isalpha(string s) {
	if (s == "")
		return false;
	int lg = s.size();
	for (int i = 0; i<lg; i++) {
		char ty = c_is_alpha(USTR(s), i);
		if (ty == 0)
			return false;
	}
	return true;
}

Exported  bool c_Islower(string s) {
	if (s == "")
		return false;

	int lg = s.size();
	for (int i = 0; i<lg; i++) {
		char ty = c_is_alpha(USTR(s), i);
		if (ty != 1)
			return false;
	}
	return true;
}

Exported BULONG c_tolower(BULONG c) {
	if (utf8codemaj.find(c) != utf8codemaj.end())
		return utf8codemaj[c];
	return c;
}

Exported BULONG c_toupper(BULONG c) {
	if (utf8codemin.find(c) != utf8codemin.end())
		return utf8codemin[c];
	return c;
}

Exported  string c_Tolower(string s) {
	string res;
	int lg = s.size();
	hmap<int, string>::iterator itlat;
	hmap<string, string>::iterator itutf8;
	for (int i = 0; i<lg; i++) {
		string c = c_char_get(USTR(s), i);
		if (c.size() == 1) {
			itlat = latinmaj.find(c[0]);
			if (itlat != latinmaj.end())
				res += itlat->second;
			else
				res += c;
		}
		else {
			itutf8 = utf8maj.find(c);
			if (itutf8 != utf8maj.end())
				res += itutf8->second;
			else
				res += c;
		}
	}
	return res;
}

Exported  string c_Toupper(string s) {
	string res;
	int lg = s.size();
	hmap<int, string>::iterator itlat;
	hmap<string, string>::iterator itutf8;
	for (int i = 0; i<lg; i++) {
		string c = c_char_get(USTR(s), i);
		if (c.size() == 1) {
			itlat = latinmin.find(c[0]);
			if (itlat != latinmin.end())
				res += itlat->second;
			else
				res += c;
		}
		else {
			itutf8 = utf8min.find(c);
			if (itutf8 != utf8min.end())
				res += itutf8->second;
			else
				res += c;
		}
	}
	return res;
}

Exported  void c_Tolower(string& res, unsigned char* s, int lg) {
	res = "";
	hmap<int, string>::iterator itlat;
	hmap<string, string>::iterator itutf8;
	for (int i = 0; i<lg; i++) {
		string c = c_char_get(s, i);
		if (c.size() == 1) {
			itlat = latinmaj.find(c[0]);
			if (itlat != latinmaj.end())
				res += itlat->second;
			else
				res += c;
		}
		else {
			itutf8 = utf8maj.find(c);
			if (itutf8 != utf8maj.end())
				res += itutf8->second;
			else
				res += c;
		}
	}
}

Exported  void c_Toupper(string& res, unsigned char* s, int lg) {
	res = "";
	hmap<int, string>::iterator itlat;
	hmap<string, string>::iterator itutf8;
	for (int i = 0; i<lg; i++) {
		string c = c_char_get(s, i);
		if (c.size() == 1) {
			itlat = latinmin.find(c[0]);
			if (itlat != latinmin.end())
				res += itlat->second;
			else
				res += c;
		}
		else {
			itutf8 = utf8min.find(c);
			if (itutf8 != utf8min.end())
				res += itutf8->second;
			else
				res += c;
		}
	}
}

//we are looking for the substring substr in s
Exported int c_find(uchar* s, string substr, int i) {
	int nb = 0, ps = 0;
	i = c_chartobytepositionidx(s, i, nb, ps);

	char* pt = strstr((char*)s + i, STR(substr));
	if (pt == NULL)
		return -1;
	i = pt - (char*)s;
	return c_bytetocharpositionidx(s, i, nb, ps);
}


//we are looking for the substring substr in s
Exported int c_rfind(uchar* sx, string substr, int i) {
	string s = (char*)sx;
	i = c_chartobyteposition(sx, i);
	i = s.rfind(substr, i);
	if (i == string::npos)
		return -1;
	return c_bytetocharposition(sx, i);
}

//we are looking for the substring substr in s
Exported int c_find(string& s, string substr, int i) {
	int nb = 0, ps = 0;
	i = c_chartobytepositionidx(USTR(s), i, nb, ps);
	i = s.find(substr, i);
	if (i == string::npos)
		return -1;
	return c_bytetocharpositionidx(USTR(s), i, nb, ps);
}


//we are looking for the substring substr in s
Exported int c_rfind(string& s, string substr, int i) {
	i = c_chartobyteposition(USTR(s), i);
	i = s.rfind(substr, i);
	if (i == string::npos)
		return -1;
	return c_bytetocharposition(USTR(s), i);
}

Exported bool c_is_punctuation(BULONG code) {
	if (punctuations.find(code) != punctuations.end())
		return true;
	return false;
}

Exported bool c_is_digit(BULONG code) {
	if (code >= '0' && code <= '9')
		return true;
	return false;
}

Exported bool c_threedigits(wstring& s, int i) {
	if (i>s.size() - 3)
		return false;
	if (s[i] >= 48 && s[i] <= 57 && s[i + 1] >= 48 && s[i + 1] <= 57 && s[i + 2] >= 48 && s[i + 2] <= 57) {
		if (i<s.size() - 3)
		if (s[i + 3] >= 48 && s[i + 3] <= 57)
			return false;
		return true;
	}
	return false;
}

string& Trim(string& chaine) {
	int d, f;
	for (d = 0; d<chaine.size(); d++) {
		if ((uchar)chaine[d]>32)
			break;
	}

	for (f = chaine.size() - 1; f >= 0; f--) {
		if ((uchar)chaine[f]>32)
			break;
	}
	int lg = f - d + 1;
	if (lg >= 1)
		chaine = chaine.substr(d, lg);
	else
		chaine = "";
	return chaine;
}

string& Trimleft(string& chaine) {
	int d, f;
	f = chaine.size() - 1;
	for (d = 0; d<chaine.size(); d++) {
		if ((uchar)chaine[d]>32)
			break;
	}

	int lg = f - d + 1;
	if (lg >= 1)
		chaine = chaine.substr(d, lg);
	else
		chaine = "";
	return chaine;
}

string& Trimright(string& chaine) {
	int d = 0, f;

	for (f = chaine.size() - 1; f >= 0; f--) {
		if ((uchar)chaine[f]>32)
			break;
	}

	int lg = f - d + 1;
	if (lg >= 1)
		chaine = chaine.substr(d, lg);
	else
		chaine = "";
	return chaine;
}

void v_split(string& thestr, string thesplitter, vector<string>& v) {
	size_t pos = 0;
	size_t found = 0;
	size_t nxt;
	string value;
	//First we isolate the long comments and the long strings... /@...@/ and @"..."@
	//We will remove the CR from these specific strings...
	//First the comments
	found = thestr.find("/@", pos);
	while (found != string::npos) {
		nxt = thestr.find("@/", found);
		//We have our substring, in which every CR, will be removed
		if (nxt != string::npos) {
			for (size_t i = found; i<nxt; i++) {
				if (thestr[i] == '\n')
					thestr[i] = '\r';
			}
		}
		else
			break;
		pos = nxt + 2;
		found = thestr.find("/@", pos);
	}

	pos = 0;
	found = thestr.find("@\"", pos);
	while (found != string::npos) {
		nxt = thestr.find("\"@", found);
		//We have our substring, in which every CR, will be removed
		if (nxt != string::npos) {
			for (size_t i = found; i<nxt; i++) {
				if (thestr[i] == '\n')
					thestr[i] = '\r';
			}
		}
		else
			break;
		pos = nxt + 2;
		found = thestr.find("@\"", pos);
	}

	pos = 0;
	found = 0;

	while (pos != string::npos) {
		found = thestr.find(thesplitter, pos);
		if (found != string::npos) {
			value = thestr.substr(pos, found - pos);
			Trim(value);
			if (value != "")
				v.push_back(value);
			else
			if (thesplitter == "\n")
				v.push_back(thesplitter);
			pos = found + thesplitter.size();
		}
		else
			break;
	}
	value = thestr.substr(pos, thestr.size() - pos);
	Trim(value);
	if (value != "")
		v.push_back(value);
	else
	if (thesplitter == "\n")
		v.push_back(thesplitter);
}

static bool comparing(wstring& w, wstring s) {
	if (w == s)
		return true;
	return false;
}

void s_tokenize(vector<string>& vect, string& thestr, map<string, bool>& keeps) {

	//First we split along spaces
	int i;
	map<wstring, bool> wkeeps;

	wstring ustr;
	string s;
	map<string, bool>::iterator it;
	for (it = keeps.begin(); it != keeps.end(); it++) {
		s = it->first;
		ustr = L"";
		if (!c_is_pure_utf8(USTR(s), s.size())) {
			string fstr;
			s_latintoutf8(fstr, USTR(s));
			s_utf8tounicode(ustr, STR(fstr));
		}
		else
			s_utf8tounicode(ustr, STR(s));
		wkeeps[ustr] = true;
	}

	ustr = L"";
	if (!c_is_pure_utf8(USTR(thestr), thestr.size())) {
		s = "";
		s_latintoutf8(s, USTR(thestr));
		s_utf8tounicode(ustr, STR(s));
	}
	else
		s_utf8tounicode(ustr, STR(thestr));


	BULONG c;
	wstring thesplitter;
	wstring punct;
	//we split along space characters...
	for (i = 0; i < ustr.size(); i++) {
		c = ustr[i];
		if (c <= 32) {
			if (thesplitter.size()) {
				if (wkeeps.find(thesplitter) == wkeeps.end()) {
					if (punct.size()) {
						thesplitter = thesplitter.substr(0, thesplitter.size() - punct.size());
						s = "";
						s_unicodetoutf8(s, (wchar_t*)thesplitter.c_str());
						vect.push_back(s);
						for (int j = 0; j < punct.size(); j++) {
							s = "";
							thesplitter = punct[j];
							s_unicodetoutf8(s, (wchar_t*)thesplitter.c_str());
							vect.push_back(s);
						}
					}
					else {
						s = "";
						s_unicodetoutf8(s, (wchar_t*)thesplitter.c_str());
						vect.push_back(s);
					}
				}
				else {
					s = "";
					s_unicodetoutf8(s, (wchar_t*)thesplitter.c_str());
					vect.push_back(s);
				}
				thesplitter = L"";
				punct = L"";
			}
		}
		else {
			thesplitter += c;
			if (c_is_punctuation(c))
				punct += c;
			else
				punct = L"";
		}
	}

	if (thesplitter.size()) {
		if (wkeeps.find(thesplitter) == wkeeps.end()) {
			if (punct.size()) {
				thesplitter = thesplitter.substr(0, thesplitter.size() - punct.size());
				s = "";
				s_unicodetoutf8(s, (wchar_t*)thesplitter.c_str());
				vect.push_back(s);
				for (int j = 0; j < punct.size(); j++) {
					s = "";
					thesplitter = punct[j];
					s_unicodetoutf8(s, (wchar_t*)thesplitter.c_str());
					vect.push_back(s);
				}
			}
			else {
				s = "";
				s_unicodetoutf8(s, (wchar_t*)thesplitter.c_str());
				vect.push_back(s);
			}
		}
		else {
			s = "";
			s_unicodetoutf8(s, (wchar_t*)thesplitter.c_str());
			vect.push_back(s);
		}
	}
}

void v_tokenize(vector<string>& vect, string& thestr, short flags) {

	bool comma, separator, periodseparator, keepwithdigit;

	if ((flags&token_comma) == token_comma)
		comma = true;
	else
		comma = false;

	if ((flags&token_separator) == token_separator)
		separator = true;
	else
		separator = false;

	if ((flags&token_period) == token_period)
		periodseparator = true;
	else
		periodseparator = false;

	if ((flags&token_keepwithdigit) == token_keepwithdigit)
		keepwithdigit = true;
	else
		keepwithdigit = false;

	char decimal[] = { '.', ',' };

	if (comma) {
		decimal[0] = ',';
		decimal[1] = '.';
	}

	//first we convert into unicode

	wstring ustr;
	if (!c_is_pure_utf8(USTR(thestr), thestr.size())) {
		string fstr;
		s_latintoutf8(fstr, USTR(thestr));
		s_utf8tounicode(ustr, STR(fstr));
	}
	else
		s_utf8tounicode(ustr, STR(thestr));

	thestr.clear();
	wstring token;
	wstring digits;
	string localstring;
	short fcomma = 0;
	BULONG c, cnext, csupnext, csupsupnext;
	bool store;
	int i;
	int sz = ustr.size();
	int nbdigits = 0;
	bool accumulateuptospace = false;

	for (i = 0; i<sz; i++) {
		c = ustr[i];
		cnext = 0;
		csupnext = 0;
		csupsupnext = 0;
		if (i + 1<sz) {
			cnext = ustr[i + 1];
			if (i + 2<sz) {
				csupnext = ustr[i + 2];
				if (i + 3<sz)
					csupsupnext = ustr[i + 3];
			}
		}

		//digit
		store = true;
		if (accumulateuptospace) {
			if (c>32) {
				if (c>32 && (c>127 || !strchr("[]{}()|,;:'\"", (char)c))) {
					token += c;
					store = false;
				}
			}
			else
				store = false;
		}
		else {
			if (c_is_digit(c)) {
				store = false;
				if (token.size() == 0) {
					digits += c;
					nbdigits++;
				}
				else
					token += c;
			}
			else {
				//punctuation		
				if (c_is_punctuation(c)) {
					switch (c) {
					case '#':
						if (digits.size() != 0) {
							token = digits;
							digits.clear();
						}
						token += c;
						store = false;
						break;
					case '@':
						if (digits.size() != 0) {
							token = digits;
							digits.clear();
						}
						token += c;
						store = false;
						accumulateuptospace = true;
						break;
					case '_':
						if (cnext>32 && !c_is_punctuation(cnext)) {
							token += c;
							store = false;
						}
						break;
					case '+':
						if (digits.size() == 0 && c_is_digit(cnext)) {
							digits = '+';
							store = false;
						}
						break;
					case '-':
						if (digits.size() == 0 && c_is_digit(cnext) && token.size() == 0) {
							digits = '-';
							store = false;
						}
						else
						if (token.size() && cnext>32 && !c_is_punctuation(cnext)) {
							token += c;
							store = false;
						}
						break;
					case '/': //UNIX path
						if (digits.size() == 0 && token.size() == 0 && (cnext == '/' || !c_is_punctuation(cnext))) {
							token = c;
							store = false;
							accumulateuptospace = true;
						}
						break;
					case ':': //Windows path
						if (token.size() == 1 && cnext == '\\' && ((token[0] >= 'A' && token[0] <= 'Z') || (token[0] >= 'a' && token[0] <= 'z'))) {
							token += c;
							store = false;
							accumulateuptospace = true;
						}
						break;
					case ',':
					case '.':
						if (digits.size() != 0 && c_is_digit(cnext)) {
							if (c == decimal[1]) {
								if (!separator || nbdigits != 3)
									break;
								nbdigits = 0;
							}
							else
								fcomma++;
							digits += c;
							store = false;
						}
						else {
							if (periodseparator == true && c == '.' && token.size() && cnext>32 && !c_is_punctuation(cnext)) {
								store = false;
								token += c;
							}
						}
						break;
					}
				}
				else {
					store = false;
					if (c>32) {
						if (keepwithdigit && digits.size())
							digits += c;
						else {
							if ((c == 'e' || c == 'E') && digits.size() && fcomma == 1 && (c_is_digit(cnext) || (cnext == '-' && c_is_digit(csupnext)))) {
								digits += c;
								if (cnext == '-') {
									digits += cnext;
									i++;
								}
							}
							else {
								if (digits.size() != 0) {
									localstring.clear();
									s_unicodetoutf8(localstring, (wchar_t*)digits.c_str());
									vect.push_back(localstring);
									fcomma = 0;
									digits.clear();
									nbdigits = 0;
								}
								token += c;
								if (token == L"http" && cnext == ':' && csupnext == '/' && csupsupnext == '/') {
									token += L"://";
									i += 3;
									accumulateuptospace = true;
								}
							}
						}
					}
				}
			}
		}

		if (store || c <= 32) {
			if (token.size() != 0) {
				localstring.clear();
				s_unicodetoutf8(localstring, (wchar_t*)token.c_str());
				vect.push_back(localstring);
			}
			else {
				if (digits.size() != 0) {
					localstring.clear();
					s_unicodetoutf8(localstring, (wchar_t*)digits.c_str());
					vect.push_back(localstring);
				}
			}
			if (store) {
				localstring.clear();
				token = c;
				s_unicodetoutf8(localstring, (wchar_t*)token.c_str());
				vect.push_back(localstring);
			}
			fcomma = 0;
			digits.clear();
			token.clear();
			nbdigits = 0;
			accumulateuptospace = false;
		}
	}

	localstring.clear();
	if (token.size() != 0) {
		s_unicodetoutf8(localstring, (wchar_t*)token.c_str());
		vect.push_back(localstring);
	}
	else
	if (digits.size() != 0) {
		s_unicodetoutf8(localstring, (wchar_t*)digits.c_str());
		vect.push_back(localstring);
	}
}


Exported  void inittableutf8() {
	static bool init = false;
	if (init)
		return;
	InitHtmlCodes();
	InitDosCodes();
	init = true;
	int unicode;
	bulongchar xs;
	bulongchar xse;
	uchar latin;
	int i;
	for (i = 0; i<maxtable; i += 3) {
		unicode = table[i];
		int equ = table[i + 1];
		char type = table[i + 2];
		xs.clear();
		xse.clear();
		c_unicodetoutf8(unicode, xs.ustr);
		c_unicodetoutf8(equ, xse.ustr);
		if (unicode<256)
			latin = unicode;
		else
			latin = 0;

		if (type == 1) {
			utf8min[xs.str] = xse.str;
			utf8codemin[unicode] = equ;
			if (latin != 0)
				latinmin[latin] = xse.str;
		}
		else {
			if (type == 2) {
				utf8maj[xs.str] = xse.str;
				utf8codemaj[unicode] = equ;
				if (latin != 0)
					latinmaj[latin] = xse.str;
			}
		}
	}
	i = 0;
	while (ponctuations[i] != 0) {
		punctuations[ponctuations[i]] = true;
		i++;
	}
	i = 0;
	char ch[10];
	while (latincodetable[i] != 0) {
		latincodes[latincodetable[i]] = latincodetable[i + 1];
		c_unicodetoutf8(latincodetable[i + 1], (uchar*)ch);
		utf8codes[ch] = latincodetable[i];
		i += 2;
	}

	consonants[349] = "s";
	consonants[231] = "c";
	consonants[199] = "C";
	consonants[208] = "D";
	consonants[331] = "n";
	consonants[122] = "z";
	consonants[292] = "H";
	consonants[241] = "n";
	consonants[209] = "N";
	consonants[88] = "X";
	consonants[262] = "C";
	consonants[263] = "c";
	consonants[86] = "V";
	consonants[264] = "C";
	consonants[87] = "W";
	consonants[265] = "c";
	consonants[84] = "T";
	consonants[266] = "C";
	consonants[267] = "c";
	consonants[82] = "R";
	consonants[268] = "C";
	consonants[83] = "S";
	consonants[269] = "c";
	consonants[353] = "s";
	consonants[80] = "P";
	consonants[270] = "D";
	consonants[352] = "S";
	consonants[81] = "Q";
	consonants[271] = "d";
	consonants[78] = "N";
	consonants[272] = "D";
	consonants[273] = "d";
	consonants[66] = "B";
	consonants[284] = "G";
	consonants[67] = "C";
	consonants[285] = "g";
	consonants[286] = "G";
	consonants[287] = "g";
	consonants[288] = "G";
	consonants[289] = "g";
	consonants[290] = "G";
	consonants[291] = "g";
	consonants[293] = "h";
	consonants[329] = "n";
	consonants[120] = "x";
	consonants[294] = "H";
	consonants[328] = "n";
	consonants[295] = "h";
	consonants[347] = "s";
	consonants[106] = "j";
	consonants[308] = "J";
	consonants[346] = "S";
	consonants[107] = "k";
	consonants[309] = "j";
	consonants[345] = "r";
	consonants[104] = "h";
	consonants[310] = "K";
	consonants[344] = "R";
	consonants[311] = "k";
	consonants[342] = "R";
	consonants[103] = "g";
	consonants[313] = "L";
	consonants[341] = "r";
	consonants[100] = "d";
	consonants[314] = "l";
	consonants[340] = "R";
	consonants[315] = "L";
	consonants[98] = "b";
	consonants[316] = "l";
	consonants[99] = "c";
	consonants[317] = "L";
	consonants[318] = "l";
	consonants[319] = "L";
	consonants[320] = "l";
	consonants[321] = "L";
	consonants[322] = "l";
	consonants[323] = "N";
	consonants[324] = "n";
	consonants[325] = "N";
	consonants[326] = "n";
	consonants[327] = "N";
	consonants[343] = "r";
	consonants[348] = "S";
	consonants[350] = "S";
	consonants[351] = "s";
	consonants[354] = "T";
	consonants[355] = "t";
	consonants[356] = "T";
	consonants[357] = "t";
	consonants[358] = "T";
	consonants[359] = "t";
	consonants[372] = "W";
	consonants[373] = "w";
	consonants[377] = "Z";
	consonants[378] = "z";
	consonants[379] = "Z";
	consonants[380] = "z";
	consonants[381] = "Z";
	consonants[382] = "z";
	consonants[384] = "b";
	consonants[385] = "B";
	consonants[386] = "b";
	consonants[387] = "b";
	consonants[388] = "b";
	consonants[389] = "b";
	consonants[391] = "C";
	consonants[392] = "c";
	consonants[393] = "D";
	consonants[394] = "D";
	consonants[395] = "d";
	consonants[396] = "d";
	consonants[401] = "F";
	consonants[402] = "f";
	consonants[403] = "G";
	consonants[408] = "K";
	consonants[409] = "k";
	consonants[452] = "DZ";
	consonants[454] = "dz";
	consonants[455] = "Ǉ";
	consonants[457] = "lj";
	consonants[458] = "NJ";
	consonants[460] = "nj";
	consonants[473] = "Ǚ";
	consonants[484] = "G";
	consonants[485] = "g";
	consonants[486] = "G";
	consonants[487] = "g";
	consonants[488] = "K";
	consonants[489] = "k";
	consonants[496] = "j";
	consonants[497] = "DZ";
	consonants[68] = "D";
	consonants[70] = "F";
	consonants[71] = "G";
	consonants[72] = "H";
	consonants[74] = "J";
	consonants[75] = "K";
	consonants[76] = "L";
	consonants[77] = "M";
	consonants[90] = "Z";
	consonants[102] = "f";
	consonants[108] = "l";
	consonants[109] = "m";
	consonants[110] = "n";
	consonants[112] = "p";
	consonants[113] = "q";
	consonants[114] = "r";
	consonants[115] = "s";
	consonants[116] = "t";
	consonants[118] = "v";
	consonants[119] = "w";


	vowels[200] = "E";
	vowels[277] = "e";
	vowels[192] = "A";
	vowels[201] = "E";
	vowels[276] = "E";
	vowels[193] = "A";
	vowels[73] = "I";
	vowels[279] = "e";
	vowels[194] = "A";
	vowels[278] = "E";
	vowels[195] = "A";
	vowels[196] = "A";
	vowels[197] = "A";
	vowels[275] = "e";
	vowels[198] = "AE";
	vowels[202] = "E";
	vowels[203] = "E";
	vowels[204] = "i";
	vowels[205] = "i";
	vowels[206] = "i";
	vowels[207] = "i";
	vowels[210] = "O";
	vowels[211] = "O";
	vowels[257] = "a";
	vowels[212] = "O";
	vowels[256] = "A";
	vowels[213] = "O";
	vowels[259] = "a";
	vowels[214] = "O";
	vowels[216] = "O";
	vowels[217] = "U";
	vowels[218] = "U";
	vowels[219] = "U";
	vowels[220] = "U";
	vowels[221] = "Y";
	vowels[224] = "a";
	vowels[225] = "a";
	vowels[226] = "a";
	vowels[227] = "a";
	vowels[228] = "a";
	vowels[229] = "a";
	vowels[230] = "ae";
	vowels[232] = "e";
	vowels[233] = "e";
	vowels[234] = "e";
	vowels[235] = "e";
	vowels[236] = "i";
	vowels[237] = "i";
	vowels[238] = "i";
	vowels[239] = "i";
	vowels[242] = "o";
	vowels[243] = "o";
	vowels[244] = "o";
	vowels[245] = "o";
	vowels[246] = "o";
	vowels[248] = "o";
	vowels[249] = "u";
	vowels[250] = "u";
	vowels[251] = "u";
	vowels[252] = "u";
	vowels[253] = "y";
	vowels[255] = "y";
	vowels[258] = "A";
	vowels[260] = "A";
	vowels[261] = "a";
	vowels[274] = "E";
	vowels[280] = "E";
	vowels[281] = "e";
	vowels[282] = "E";
	vowels[69] = "E";
	vowels[283] = "e";
	vowels[296] = "I";
	vowels[297] = "i";
	vowels[298] = "I";
	vowels[117] = "u";
	vowels[299] = "i";
	vowels[300] = "I";
	vowels[301] = "i";
	vowels[302] = "I";
	vowels[303] = "i";
	vowels[304] = "I";
	vowels[111] = "o";
	vowels[305] = "i";
	vowels[306] = "IJ";
	vowels[307] = "ij";
	vowels[332] = "O";
	vowels[333] = "o";
	vowels[334] = "O";
	vowels[335] = "o";
	vowels[336] = "O";
	vowels[337] = "o";
	vowels[338] = "OE";
	vowels[339] = "oe";
	vowels[360] = "U";
	vowels[361] = "u";
	vowels[362] = "U";
	vowels[363] = "u";
	vowels[364] = "U";
	vowels[365] = "u";
	vowels[366] = "U";
	vowels[367] = "u";
	vowels[368] = "U";
	vowels[369] = "u";
	vowels[370] = "U";
	vowels[371] = "u";
	vowels[374] = "Y";
	vowels[375] = "y";
	vowels[376] = "Y";
	vowels[404] = "Y";
	vowels[461] = "A";
	vowels[462] = "a";
	vowels[463] = "I";
	vowels[464] = "i";
	vowels[465] = "O";
	vowels[466] = "o";
	vowels[467] = "U";
	vowels[468] = "u";
	vowels[469] = "U";
	vowels[470] = "u";
	vowels[471] = "U";
	vowels[472] = "u";
	vowels[474] = "u";
	vowels[475] = "U";
	vowels[476] = "u";
	vowels[478] = "A";
	vowels[479] = "a";
	vowels[480] = "A";
	vowels[481] = "a";
	vowels[482] = "AE";
	vowels[483] = "ae";
	vowels[490] = "O";
	vowels[491] = "o";
	vowels[492] = "O";
	vowels[493] = "o";
	vowels[97] = "a";
	vowels[105] = "i";
	vowels[121] = "y";
	vowels[65] = "A";
	vowels[101] = "e";
	vowels[79] = "O";
	vowels[85] = "U";
	vowels[89] = "Y";
}

Exported bool c_is_digit(string& str) {
	static char tabs[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	int lg = str.size();
	int i = 0;
	if (str[0] == '-' || str[0] == '.')
		i++;
	bool digit = false;
	bool dot = false;
	int countdigits = 0;
	for (; i<lg; i++) {
		if (str[i] == '.' || str[i] == ',') {
			if (!digit)
				return false;

			if (str[i] == '.') {
				if (dot)
					return false;
				dot = true;
			}
			else {
				if (countdigits != 3)
					return false;
			}
			countdigits = 0;
			continue;
		}

		if (strchr(tabs, str[i]) == NULL)
			return false;
		countdigits++;
		digit = true;
	}

	if (!digit)
		return false;
	return true;
}

Exported char c_is_number(uchar* str, char decimal, long& l, double& f) {
	size_t sz = strlen((char*)str);
	double nbdecimal = 1;
	bool founddecimalpoint = false;
	l = 0;
	f = 0;
	for (int i = 0; i<sz; i++) {
		if (str[i] >= '0' && str[i] <= '9') {
			if (!founddecimalpoint)
				l = l * 10 + (str[i] - 48);
			else {
				f = f * 10 + (str[i] - 48);
				nbdecimal *= 10;
			}
			continue;
		}
		if (str[i] == decimal) {
			if (founddecimalpoint)
				return 0;
			founddecimalpoint = true;
			continue;
		}
		return 0;
	}
	if (founddecimalpoint) {
		f = l + (f / nbdecimal);
		return 2;
	}
	return 1;
}

void c_normalise(string& code) {
	code = c_creplacestring(code, "\r\n", "\n");
	code = c_creplacestring(code, "\r", "\n");
}

char laccolade(string& ligne) {
	int i;
	char trouve = 0;
	int mx = ligne.size();
	unsigned char c;
	bool acc = false;
	for (i = 0; i<mx; i++) {
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
			trouve = ',';
			break;
		case ':':
			if (i + 1<mx && ligne[i + 1] == '-') { //inference rules
				i++;
				trouve = '-';
				break;
			}
		case ')':
			trouve = ')';
			break;
		case '"':
			if (trouve != '{')
				trouve = 0;
			i++;
			while (i<mx && ligne[i] != '"') {
				if (ligne[i] == '\\')
					i++;
				i++;
			}
			break;
		case '\'':
			if (trouve != '{')
				trouve = 0;
			i++;
			while (i<mx && ligne[i] != '\'')
				i++;
			break;
		case ';':
			if (trouve != '{')
				trouve = 0;
			break;
		case '/':
			if (i<mx - 1 && ligne[i + 1] == '/')
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

bool TestChaine(string& s, const char* ch, char fin) {
	bool tr = true;
	int i = 0;
	int ln = strlen(ch);
	if (s.size()<ln)
		return false;
	for (; i<ln; i++) {
		if (s[i] != ch[i])
			return false;
	}
	if (i == s.size() || s[i] <= 32 || s[i] == fin || s[i] == '/')
		return true;
	return false;
}


void IndentationCode(string& codeindente, vector<string>& code, vector <int>& blancs, int mxbase, bool construit, const char* kifelse, const char* kifelif, const char* kifif) {
	int x, i, compte = 0;
	int mx = mxbase;
	int sz = code.size();
	bool firstcomma = true;
	blancs.reserve(sz);
	for (i = 0; i<sz; i++)
		blancs.push_back(-1);
	int curly = 0;

	vector<int> decalages;
	vector<char> types;
	vector<int> curlies;

	blancs[0] = 0;
	x_readstring xr("");
	for (i = 0; i<sz; i++) {

		if (code[i] == "") {
			if (blancs[i] == -1)
				blancs[i] = blancs[i - 1];
			continue;
		}

		xr.pos = 0;
		xr.parcours = code[i];
		xr.length = xr.parcours.size();
		xr.loadtoken();

		bool toadd = false;
		char lookforif = 0;
		if (xr.stack.size()) {
			if (xr.stack[0] == kifif || xr.stack[0] == kifelif) {
				types.push_back('i');
				toadd = true;
			}
			else {
				if (xr.stack[0] == kifelse) {
					types.push_back('e');
					lookforif = 1;
					toadd = true;
				}
			}

			if (xr.stack.back() == ")") {
				if (!toadd)
					types.push_back(')');
				lookforif = 2;
				toadd = true;
			}
		}

		//If the next element has been predicted and it is not an if or an else
		if (blancs[i] == -1) {
			x = decalages.size() - 1;
			int cpt = 0;
			int shift = 5;
			while (x >= 0) {
				if (lookforif == 1 && types[x] == 'i') {
					if (!cpt) {
						shift = 0;
						break;
					}
				}
				if (types[x] == '}')
					cpt++;
				else {
					if (types[x] == '{') {
						if (!cpt)
							break;
						cpt--;
					}
				}
				x--;
			}
			if (x >= 0)
				blancs[i] = decalages[x] + shift;
			else
				blancs[i] = 0;
		}

		if (toadd)
			decalages.push_back(blancs[i]);

		if (!xr.stack.size())
			continue;

		curly = 0;
		for (x = 0; x<xr.stack.size(); x++) {
			if (xr.stack[x] == "{")
				curly++;

			if (xr.stack[x] == "}")
				curly--;
		}

		if (curly >= 1) {
			lookforif = 0;
			while (curly>0) {
				decalages.push_back(blancs[i]);
				types.push_back('{');
				if (i + 1<sz) {
					curlies.push_back(blancs[i]);
					blancs[i + 1] = blancs[i] + 5;
					if (blancs[i + 1]>mx)
						mx = blancs[i + 1];
				}
				curly--;
			}
		}

		if (curly <= -1) {
			while (curly<0) {
				decalages.push_back(-blancs[i]);
				types.push_back('}');
				if (curlies.size()>0) {
					blancs[i] = curlies.back();
					curlies.pop_back();
				}
				curly++;
			}

			if (blancs[i] == 0) {
				decalages.clear();
				types.clear();
				curlies.clear();
			}
		}

		if (xr.stack.back() == "," || xr.stack.back() == "|" || xr.stack.back() == "&") {
			if (i + 1<sz) {
				if (firstcomma) {
					blancs[i + 1] = blancs[i] + 5;
					if (blancs[i + 1]>mx)
						mx = blancs[i + 1];
				}
				else
					blancs[i + 1] = blancs[i];
			}
			firstcomma = false;
			continue;
		}

		if (xr.stack.size()>1 && xr.stack.back() == "-" && xr.stack[xr.stack.size() - 2] == ":") {
			if (i + 1<sz) {
				blancs[i + 1] = blancs[i] + 5;
				if (blancs[i + 1]>mx)
					mx = blancs[i + 1];
			}
			firstcomma = false;
			continue;
		}

		firstcomma = true;
		if (i + 1<sz && lookforif) {
			blancs[i + 1] = blancs[i] + 5;
			if (blancs[i + 1]>mx)
				mx = blancs[i + 1];
			continue;
		}
	}

	if (construit) {
		mx++;
		char* blanc = new char[mx + 1];
		memset(blanc, ' ', mx);
		blanc[mx] = 0;
		for (i = 0; i<sz; i++) {
			if (code[i].size() == 1 && code[i][0] == '\n') {
				codeindente += "\n";
				continue;
			}
			x = blancs[i];
			if (x == -1)
				x = 0;
			blanc[x] = 0;
			codeindente += blanc;
			blanc[x] = ' ';
			codeindente += code[i];
			codeindente += "\n";
		}
		delete[] blanc;
	}
}

void IndentCode(string& codestr, string& codeindente, int blancs, const char* kifelse, const char* kifelif, const char* kifif) {
	vector<string> vargs;
	vector <int> iblancs;
	c_normalise(codestr);
	v_split(codestr, "\n", vargs);
	codeindente = "";
	IndentationCode(codeindente, vargs, iblancs, blancs, true, kifelse, kifelif, kifif);
	if (codeindente.find("/@") != string::npos || codeindente.find("@\"") != string::npos)
		c_normalise(codeindente);

	Trimright(codeindente);
	codeindente += "\n";
}


