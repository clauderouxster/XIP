#include "kif.h"


Exported const char* kifwhile="while";
Exported const char* kiffor="for";
Exported const char* kifif="if";
Exported const char* kifswitch="switch";
Exported const char* kifdisplayln="println";
Exported const char* kifdisplay="print";
Exported const char* kifelif="elif";
Exported const char* kifelse="else";

#ifdef WITHWSTRING
Exported const wchar_t* wkifwhile=L"while";
Exported const wchar_t* wkiffor=L"for";
Exported const wchar_t* wkifif=L"if";
Exported const wchar_t* wkifswitch=L"switch";
Exported const wchar_t* wkifdisplayln=L"println";
Exported const wchar_t* wkifdisplay=L"print";
Exported const wchar_t* wkifelif=L"elif";
Exported const wchar_t* wkifelse=L"else";
#endif


Exported const char* KIFKIFSTRINGS[]={
" (line:", //0 kif.cxx 895
"this", //1 kif.cxx 954
"null", //2 kif.cxx 954
"Instruction", //3 kif.cxx 1056
"line %d in %s", //4 kif.cxx 1062
"KIF(413): Incompatible value", //5 kif.cxx 1411
"function", //6 kif.cxx 1540
"frame", //7 kif.cxx 1549
"r", //8 kif.cxx 1656
"w", //9 kif.cxx 1672
"w+", //10 kif.cxx 1680
"true", //11 kif.cxx 3446
"false", //12 kif.cxx 3447
"KIF(421): We only accept one level in a container for a stream", //13 kif.cxx 3734
"KIF(689): Circular structure", //14 kif.cxx 4940
"KIF(777): Too many joined threads", //15 kif.cxx 5360
"STK(999): No more threads", //16 kif.cxx 5553
"' in object of type '", //17 kif.cxx 7847
"KIF(821): Stream variable not accessible anymore", //18 kif.cxx 7982
"KIF(331): Wrong manipulation of this variable: ", //19 kif.cxx 11436
"KIF(192): Unknown external function: ", //20 kif.cxx 11458
"KIF(301): Unknown function: ", //21 kif.cxx 11903
"KIF(345): Unknown frame function: ", //22 kif.cxx 11931
"self", //23 kif.cxx 12053
"KIF(650): Expecting a function as parameter", //24 kif.cxx 12416
"KIF(431): Wrong usage of operator '::'. Two arguments at most", //25 kif.cxx 12907
"KIF(432): Wrong usage of operator '::', one element should be a vector container", //26 kif.cxx 12933
"runadthread", //27 kif.cxx 14034
"KIF(415): You cannot modify a tuple element", //28 kif.cxx 14344
""};


Exported const char* KIFMAINSTRINGS[]={
"-console: open the console editor", //0 kifmain.cxx 116
"-a 'source code' with _args fully piped", //1 kifmain.cxx 117
"-p 'source code' with _args piped one string at a time", //2 kifmain.cxx 118
"-c 'source code' without piped data", //3 kifmain.cxx 119
"Predeclared variables:", //4 kifmain.cxx 120
"_args: argument vector", //5 kifmain.cxx 122
"_paths: _paths[0] is the current directory", //6 kifmain.cxx 124
"i,j,k: int", //7 kifmain.cxx 126
"f,g,h: float", //8 kifmain.cxx 128
"s,t,u: string", //9 kifmain.cxx 130
"m: map", //10 kifmain.cxx 132
"v: vector", //11 kifmain.cxx 134
"REGEX available", //12 kifmain.cxx 160
"Xcode version", //13 kifmain.cxx 163
"Curl library Embedded", //14 kifmain.cxx 166
"Fast Light ToolKit (FLTK) available", //15 kifmain.cxx 169
"-nodos", //16 kifmain.cxx 188
"-check", //17 kifmain.cxx 201
"Error: missing code", //18 kifmain.cxx 230
"Missing filename...", //19 kifmain.cxx 318
"Error: ", //20 kifmain.cxx 351
""};


Exported const char* KIFGLOBALTHREAD[]={
"w", //0 kifglobalthread.cxx 200
"_current", //1 kifglobalthread.cxx 271
"SYS(010): Stream has already been redirected", //2 kifglobalthread.cxx 348
"SYS(001): Cannot open output file", //3 kifglobalthread.cxx 350
"SYS(011): Stream not opened", //4 kifglobalthread.cxx 362
"KIF(629): Unknwown type: ", //5 kifglobalthread.cxx 399
"STK(888): Stack overflow", //6 kifglobalthread.cxx 676
"(Thread:", //7 kifglobalthread.cxx 737
"at line:", //8 kifglobalthread.cxx 825
"in", //9 kifglobalthread.cxx 830
"Instruction", //10 kifglobalthread.cxx 838
"line %ld in %s", //11 kifglobalthread.cxx 845
"line %ld", //12 kifglobalthread.cxx 847
"line %d in %s", //13 kifglobalthread.cxx 963
"null", //14 kifglobalthread.cxx 1139
"this", //15 kifglobalthread.cxx 1140
"sequence", //16 kifglobalthread.cxx 1141
"catch", //17 kifglobalthread.cxx 1142
"self", //18 kifglobalthread.cxx 1143
"_initial", //19 kifglobalthread.cxx 1146
"size", //20 kifglobalthread.cxx 1147
"methods", //21 kifglobalthread.cxx 1154
"vector", //22 kifglobalthread.cxx 1155
"map", //23 kifglobalthread.cxx 1156
"apply", //24 kifglobalthread.cxx 1157
"synchronous", //25 kifglobalthread.cxx 1158
"get", //26 kifglobalthread.cxx 1159
"string", //27 kifglobalthread.cxx 1160
"int", //28 kifglobalthread.cxx 1161
"long", //29 kifglobalthread.cxx 1162
"float", //30 kifglobalthread.cxx 1163
"boolean", //31 kifglobalthread.cxx 1164
"fraction", //32 kifglobalthread.cxx 1165
"true", //33 kifglobalthread.cxx 1181
"false", //34 kifglobalthread.cxx 1182
"_paths", //35 kifglobalthread.cxx 1184
"_KIFMAIN", //36 kifglobalthread.cxx 1187
"_loader", //37 kifglobalthread.cxx 1189
"pop", //38 kifglobalthread.cxx 1190
"stdin", //39 kifglobalthread.cxx 1195
"for", //40 kifglobalthread.cxx 1197
"while", //41 kifglobalthread.cxx 1198
"try", //42 kifglobalthread.cxx 1199
"switch", //43 kifglobalthread.cxx 1200
"if", //44 kifglobalthread.cxx 1201
"_final", //45 kifglobalthread.cxx 1203
"_run_as_thread", //46 kifglobalthread.cxx 1204
"loadin", //47 kifglobalthread.cxx 1205
"_endl", //48 kifglobalthread.cxx 1206
"empty", //49 kifglobalthread.cxx 1207
"smap", //50 kifglobalthread.cxx 1209
"imap", //51 kifglobalthread.cxx 1210
"fmap", //52 kifglobalthread.cxx 1211
"svector", //53 kifglobalthread.cxx 1212
"ivector", //54 kifglobalthread.cxx 1213
"fvector", //55 kifglobalthread.cxx 1214
"fail", //56 kifglobalthread.cxx 1217
"!assertdb", //57 kifglobalthread.cxx 1221
"!retractdb", //58 kifglobalthread.cxx 1223
"!ponder", //59 kifglobalthread.cxx 1224
"_predicatemode", //60 kifglobalthread.cxx 1226
"short", //61 kifglobalthread.cxx 1227
"waiton", //62 kifglobalthread.cxx 1228
"trigger", //63 kifglobalthread.cxx 1229
"sthrough", //64 kifglobalthread.cxx 1231
"ithrough", //65 kifglobalthread.cxx 1232
"fthrough", //66 kifglobalthread.cxx 1233
"vthrough", //67 kifglobalthread.cxx 1234
"return", //68 kifglobalthread.cxx 1304
"predicate", //69 kifglobalthread.cxx 1327
"predicateinstance", //70 kifglobalthread.cxx 1328
"variable", //71 kifglobalthread.cxx 1330
"domain", //72 kifglobalthread.cxx 1331
"ustring", //73 kifglobalthread.cxx 1333
"tuple", //74 kifglobalthread.cxx 1334
"rawstring", //75 kifglobalthread.cxx 1335
"time", //76 kifglobalthread.cxx 1336
"automaton", //77 kifglobalthread.cxx 1337
"grammar", //78 kifglobalthread.cxx 1338
"available", //79 kifglobalthread.cxx 1339
"byte", //80 kifglobalthread.cxx 1341
"bit", //81 kifglobalthread.cxx 1345
"bits", //82 kifglobalthread.cxx 1346
"date", //83 kifglobalthread.cxx 1347
"call", //84 kifglobalthread.cxx 1348
"mapu", //85 kifglobalthread.cxx 1357
"mapi", //86 kifglobalthread.cxx 1358
"mapf", //87 kifglobalthread.cxx 1359
"mapis", //88 kifglobalthread.cxx 1360
"mapif", //89 kifglobalthread.cxx 1361
"mapii", //90 kifglobalthread.cxx 1362
"mapfs", //91 kifglobalthread.cxx 1363
"mapff", //92 kifglobalthread.cxx 1364
"mapfi", //93 kifglobalthread.cxx 1365
"mapuu", //94 kifglobalthread.cxx 1366
"mapui", //95 kifglobalthread.cxx 1367
"mapuf", //96 kifglobalthread.cxx 1368
"mapss", //97 kifglobalthread.cxx 1369
"mapsf", //98 kifglobalthread.cxx 1370
"mapsi", //99 kifglobalthread.cxx 1371
"mapiu", //100 kifglobalthread.cxx 1373
"mapfu", //101 kifglobalthread.cxx 1374
"treemap", //102 kifglobalthread.cxx 1376
"treemapu", //103 kifglobalthread.cxx 1377
"treemapi", //104 kifglobalthread.cxx 1378
"treemapf", //105 kifglobalthread.cxx 1379
"treemapis", //106 kifglobalthread.cxx 1380
"treemapif", //107 kifglobalthread.cxx 1381
"treemapii", //108 kifglobalthread.cxx 1382
"treemapfs", //109 kifglobalthread.cxx 1383
"treemapff", //110 kifglobalthread.cxx 1384
"treemapfi", //111 kifglobalthread.cxx 1385
"treemapss", //112 kifglobalthread.cxx 1386
"treemapuu", //113 kifglobalthread.cxx 1387
"treemapui", //114 kifglobalthread.cxx 1388
"treemapuf", //115 kifglobalthread.cxx 1389
"treemapiu", //116 kifglobalthread.cxx 1391
"treemapfu", //117 kifglobalthread.cxx 1392
"treemapsf", //118 kifglobalthread.cxx 1394
"treemapsi", //119 kifglobalthread.cxx 1395
"primemap", //120 kifglobalthread.cxx 1397
"primemapu", //121 kifglobalthread.cxx 1398
"primemapi", //122 kifglobalthread.cxx 1399
"binmapi", //123 kifglobalthread.cxx 1401
"binmapii", //124 kifglobalthread.cxx 1402
"binmapif", //125 kifglobalthread.cxx 1403
"binmapis", //126 kifglobalthread.cxx 1404
"binmapiu", //127 kifglobalthread.cxx 1405
"primemapf", //128 kifglobalthread.cxx 1407
"primemapis", //129 kifglobalthread.cxx 1408
"primemapif", //130 kifglobalthread.cxx 1409
"primemapii", //131 kifglobalthread.cxx 1410
"primemapfs", //132 kifglobalthread.cxx 1411
"primemapff", //133 kifglobalthread.cxx 1412
"primemapfi", //134 kifglobalthread.cxx 1413
"primemapss", //135 kifglobalthread.cxx 1414
"primemapsf", //136 kifglobalthread.cxx 1415
"primemapsi", //137 kifglobalthread.cxx 1416
"primemapuu", //138 kifglobalthread.cxx 1418
"primemapui", //139 kifglobalthread.cxx 1419
"primemapuf", //140 kifglobalthread.cxx 1420
"primemapiu", //141 kifglobalthread.cxx 1421
"primemapfu", //142 kifglobalthread.cxx 1422
"set", //143 kifglobalthread.cxx 1425
"sset", //144 kifglobalthread.cxx 1426
"uset", //145 kifglobalthread.cxx 1427
"fset", //146 kifglobalthread.cxx 1428
"iset", //147 kifglobalthread.cxx 1429
"uvector", //148 kifglobalthread.cxx 1432
"bvector", //149 kifglobalthread.cxx 1435
"table", //150 kifglobalthread.cxx 1438
"term", //151 kifglobalthread.cxx 1440
"list", //152 kifglobalthread.cxx 1441
"iterator", //153 kifglobalthread.cxx 1442
"file", //154 kifglobalthread.cxx 1443
"wfile", //155 kifglobalthread.cxx 1444
"bool", //156 kifglobalthread.cxx 1484
"auto", //157 kifglobalthread.cxx 1486
"let", //158 kifglobalthread.cxx 1487
"maps", //159 kifglobalthread.cxx 1491
"ismap", //160 kifglobalthread.cxx 1495
"iimap", //161 kifglobalthread.cxx 1496
"ifmap", //162 kifglobalthread.cxx 1497
"fsmap", //163 kifglobalthread.cxx 1498
"fimap", //164 kifglobalthread.cxx 1499
"ffmap", //165 kifglobalthread.cxx 1500
"ssmap", //166 kifglobalthread.cxx 1505
"simap", //167 kifglobalthread.cxx 1506
"sfmap", //168 kifglobalthread.cxx 1507
"uumap", //169 kifglobalthread.cxx 1510
"uimap", //170 kifglobalthread.cxx 1511
"ufmap", //171 kifglobalthread.cxx 1512
"iumap", //172 kifglobalthread.cxx 1514
"treemaps", //173 kifglobalthread.cxx 1532
"istreemap", //174 kifglobalthread.cxx 1537
"iitreemap", //175 kifglobalthread.cxx 1538
"iftreemap", //176 kifglobalthread.cxx 1539
"fstreemap", //177 kifglobalthread.cxx 1540
"fitreemap", //178 kifglobalthread.cxx 1541
"fftreemap", //179 kifglobalthread.cxx 1542
"streemap", //180 kifglobalthread.cxx 1544
"itreemap", //181 kifglobalthread.cxx 1545
"ftreemap", //182 kifglobalthread.cxx 1546
"sstreemap", //183 kifglobalthread.cxx 1547
"sitreemap", //184 kifglobalthread.cxx 1548
"sftreemap", //185 kifglobalthread.cxx 1549
"uutreemap", //186 kifglobalthread.cxx 1550
"uitreemap", //187 kifglobalthread.cxx 1551
"uftreemap", //188 kifglobalthread.cxx 1552
"iutreemap", //189 kifglobalthread.cxx 1554
"primemaps", //190 kifglobalthread.cxx 1573
"iibinmap", //191 kifglobalthread.cxx 1583
"ifbinmap", //192 kifglobalthread.cxx 1584
"isbinmap", //193 kifglobalthread.cxx 1585
"iubinmap", //194 kifglobalthread.cxx 1586
"uuprimemap", //195 kifglobalthread.cxx 1589
"uiprimemap", //196 kifglobalthread.cxx 1591
"ufprimemap", //197 kifglobalthread.cxx 1593
"iuprimemap", //198 kifglobalthread.cxx 1595
"fuprimemap", //199 kifglobalthread.cxx 1597
"isprimemap", //200 kifglobalthread.cxx 1599
"iiprimemap", //201 kifglobalthread.cxx 1600
"ifprimemap", //202 kifglobalthread.cxx 1601
"fsprimemap", //203 kifglobalthread.cxx 1602
"fiprimemap", //204 kifglobalthread.cxx 1603
"ffprimemap", //205 kifglobalthread.cxx 1604
"sprimemap", //206 kifglobalthread.cxx 1606
"iprimemap", //207 kifglobalthread.cxx 1607
"fprimemap", //208 kifglobalthread.cxx 1608
"ssprimemap", //209 kifglobalthread.cxx 1609
"siprimemap", //210 kifglobalthread.cxx 1610
"sfprimemap", //211 kifglobalthread.cxx 1611
"kif", //212 kifglobalthread.cxx 1656
"predicatevar", //213 kifglobalthread.cxx 1676
"fumap", //214 kifglobalthread.cxx 1726
"futreemap", //215 kifglobalthread.cxx 1768
"riterator", //216 kifglobalthread.cxx 1842
"tree", //217 kifglobalthread.cxx 1843
"is", //218 kifglobalthread.cxx 1988
"isnot", //219 kifglobalthread.cxx 1989
"or", //220 kifglobalthread.cxx 2024
"and", //221 kifglobalthread.cxx 2025
"on", //222 kifglobalthread.cxx 2027
"notin", //223 kifglobalthread.cxx 2028
"_compatibilities", //224 kifglobalthread.cxx 2201
"infos", //225 kifglobalthread.cxx 2210
"isa", //226 kifglobalthread.cxx 2211
"type", //227 kifglobalthread.cxx 2212
"iscontainer", //228 kifglobalthread.cxx 2213
"isvector", //229 kifglobalthread.cxx 2215
"name", //230 kifglobalthread.cxx 2216
"min", //231 kifglobalthread.cxx 2220
"max", //232 kifglobalthread.cxx 2221
"clear", //233 kifglobalthread.cxx 2222
"addtowaiton", //234 kifglobalthread.cxx 2225
"load", //235 kifglobalthread.cxx 2230
"loadlemma", //236 kifglobalthread.cxx 2235
"model", //237 kifglobalthread.cxx 2236
"look", //238 kifglobalthread.cxx 2237
"spotfinalstate", //239 kifglobalthread.cxx 2238
"loadcompact", //240 kifglobalthread.cxx 2239
"store", //241 kifglobalthread.cxx 2240
"setcodefeature", //242 kifglobalthread.cxx 2242
"editdistance", //243 kifglobalthread.cxx 2243
"add", //244 kifglobalthread.cxx 2245
"settransformationmap", //245 kifglobalthread.cxx 2248
"sister", //246 kifglobalthread.cxx 2262
"previous", //247 kifglobalthread.cxx 2264
"mother", //248 kifglobalthread.cxx 2265
"daughter", //249 kifglobalthread.cxx 2266
"lastnode", //250 kifglobalthread.cxx 2267
"depth", //251 kifglobalthread.cxx 2268
"prune", //252 kifglobalthread.cxx 2269
"isolate", //253 kifglobalthread.cxx 2270
"ioredirect", //254 kifglobalthread.cxx 2282
"hash", //255 kifglobalthread.cxx 2284
"reverse", //256 kifglobalthread.cxx 2287
"ord", //257 kifglobalthread.cxx 2288
"bytes", //258 kifglobalthread.cxx 2289
"format", //259 kifglobalthread.cxx 2290
"fill", //260 kifglobalthread.cxx 2291
"padding", //261 kifglobalthread.cxx 2292
"parse", //262 kifglobalthread.cxx 2294
"sizeb", //263 kifglobalthread.cxx 2295
"lisp", //264 kifglobalthread.cxx 2296
"tags", //265 kifglobalthread.cxx 2297
"resize", //266 kifglobalthread.cxx 2298
"evaluate", //267 kifglobalthread.cxx 2300
"replaceregexip", //268 kifglobalthread.cxx 2302
"replace", //269 kifglobalthread.cxx 2305
"getstd", //270 kifglobalthread.cxx 2306
"geterr", //271 kifglobalthread.cxx 2307
"split", //272 kifglobalthread.cxx 2308
"splite", //273 kifglobalthread.cxx 2309
"tokenize", //274 kifglobalthread.cxx 2310
"stokenize", //275 kifglobalthread.cxx 2311
"count", //276 kifglobalthread.cxx 2312
"find", //277 kifglobalthread.cxx 2313
"rfind", //278 kifglobalthread.cxx 2314
"removefirst", //279 kifglobalthread.cxx 2315
"removelast", //280 kifglobalthread.cxx 2316
"isutf8", //281 kifglobalthread.cxx 2317
"ngrams", //282 kifglobalthread.cxx 2318
"latin", //283 kifglobalthread.cxx 2320
"dos", //284 kifglobalthread.cxx 2321
"dostoutf8", //285 kifglobalthread.cxx 2322
"left", //286 kifglobalthread.cxx 2323
"byteposition", //287 kifglobalthread.cxx 2324
"charposition", //288 kifglobalthread.cxx 2325
"right", //289 kifglobalthread.cxx 2326
"mid", //290 kifglobalthread.cxx 2327
"isalpha", //291 kifglobalthread.cxx 2328
"isconsonant", //292 kifglobalthread.cxx 2329
"isvowel", //293 kifglobalthread.cxx 2330
"ispunctuation", //294 kifglobalthread.cxx 2331
"isdigit", //295 kifglobalthread.cxx 2332
"extract", //296 kifglobalthread.cxx 2333
"isupper", //297 kifglobalthread.cxx 2334
"islower", //298 kifglobalthread.cxx 2335
"upper", //299 kifglobalthread.cxx 2336
"deaccentuate", //300 kifglobalthread.cxx 2337
"lower", //301 kifglobalthread.cxx 2338
"trim", //302 kifglobalthread.cxx 2339
"trimleft", //303 kifglobalthread.cxx 2340
"trimright", //304 kifglobalthread.cxx 2341
"last", //305 kifglobalthread.cxx 2343
"insert", //306 kifglobalthread.cxx 2344
"query", //307 kifglobalthread.cxx 2352
"remove", //308 kifglobalthread.cxx 2354
"weight", //309 kifglobalthread.cxx 2355
"duplicate", //310 kifglobalthread.cxx 2362
"move", //311 kifglobalthread.cxx 2364
"push", //312 kifglobalthread.cxx 2366
"merge", //313 kifglobalthread.cxx 2368
"totext", //314 kifglobalthread.cxx 2371
"flatten", //315 kifglobalthread.cxx 2372
"test", //316 kifglobalthread.cxx 2373
"join", //317 kifglobalthread.cxx 2374
"reserve", //318 kifglobalthread.cxx 2375
"sort", //319 kifglobalthread.cxx 2380
"permute", //320 kifglobalthread.cxx 2381
"shuffle", //321 kifglobalthread.cxx 2382
"unique", //322 kifglobalthread.cxx 2383
"sortint", //323 kifglobalthread.cxx 2384
"sortfloat", //324 kifglobalthread.cxx 2385
"sortstring", //325 kifglobalthread.cxx 2386
"sortlong", //326 kifglobalthread.cxx 2387
"sum", //327 kifglobalthread.cxx 2388
"product", //328 kifglobalthread.cxx 2389
"pushfirst", //329 kifglobalthread.cxx 2414
"poplast", //330 kifglobalthread.cxx 2415
"pushlast", //331 kifglobalthread.cxx 2416
"popfirst", //332 kifglobalthread.cxx 2417
"first", //333 kifglobalthread.cxx 2428
"run", //334 kifglobalthread.cxx 2446
"runasthread", //335 kifglobalthread.cxx 2449
"kstart", //336 kifglobalthread.cxx 2450
"runend", //337 kifglobalthread.cxx 2451
"debugfunction", //338 kifglobalthread.cxx 2452
"debugclose", //339 kifglobalthread.cxx 2453
"compile", //340 kifglobalthread.cxx 2454
"open", //341 kifglobalthread.cxx 2455
"clean", //342 kifglobalthread.cxx 2456
"exposed", //343 kifglobalthread.cxx 2457
"invert", //344 kifglobalthread.cxx 2462
"keys", //345 kifglobalthread.cxx 2476
"values", //346 kifglobalthread.cxx 2477
"items", //347 kifglobalthread.cxx 2478
"evaluation", //348 kifglobalthread.cxx 2492
"seeds", //349 kifglobalthread.cxx 2493
"openwrite", //350 kifglobalthread.cxx 2525
"openappend", //351 kifglobalthread.cxx 2526
"openread", //352 kifglobalthread.cxx 2527
"setsignature", //353 kifglobalthread.cxx 2528
"getsignature", //354 kifglobalthread.cxx 2529
"flush", //355 kifglobalthread.cxx 2530
"unget", //356 kifglobalthread.cxx 2532
"close", //357 kifglobalthread.cxx 2533
"seek", //358 kifglobalthread.cxx 2534
"tell", //359 kifglobalthread.cxx 2535
"write", //360 kifglobalthread.cxx 2536
"writeln", //361 kifglobalthread.cxx 2537
"writebin", //362 kifglobalthread.cxx 2538
"read", //363 kifglobalthread.cxx 2539
"readln", //364 kifglobalthread.cxx 2540
"eof", //365 kifglobalthread.cxx 2546
"setdate", //366 kifglobalthread.cxx 2555
"year", //367 kifglobalthread.cxx 2557
"month", //368 kifglobalthread.cxx 2558
"day", //369 kifglobalthread.cxx 2559
"yearday", //370 kifglobalthread.cxx 2560
"weekday", //371 kifglobalthread.cxx 2561
"hour", //372 kifglobalthread.cxx 2562
"minute", //373 kifglobalthread.cxx 2563
"second", //374 kifglobalthread.cxx 2565
"reset", //375 kifglobalthread.cxx 2583
"log", //376 kifglobalthread.cxx 2592
"ln", //377 kifglobalthread.cxx 2592
"exp", //378 kifglobalthread.cxx 2592
"sqrt", //379 kifglobalthread.cxx 2592
"tan", //380 kifglobalthread.cxx 2592
"sin", //381 kifglobalthread.cxx 2592
"cos", //382 kifglobalthread.cxx 2592
"atan", //383 kifglobalthread.cxx 2592
"abs", //384 kifglobalthread.cxx 2595
"factors", //385 kifglobalthread.cxx 2603
"chr", //386 kifglobalthread.cxx 2610
"iorestate", //387 kifglobalthread.cxx 2612
"partof", //388 kifglobalthread.cxx 2616
"simplify", //389 kifglobalthread.cxx 2626
"nd", //390 kifglobalthread.cxx 2628
"euclidian", //391 kifglobalthread.cxx 2631
"next", //392 kifglobalthread.cxx 2634
"end", //393 kifglobalthread.cxx 2635
"setvalue", //394 kifglobalthread.cxx 2636
"nend", //395 kifglobalthread.cxx 2637
"begin", //396 kifglobalthread.cxx 2638
"key", //397 kifglobalthread.cxx 2639
"value", //398 kifglobalthread.cxx 2640
"valuetype", //399 kifglobalthread.cxx 2642
"_stack", //400 kifglobalthread.cxx 2908
"_filename", //401 kifglobalthread.cxx 2909
"gcsize", //402 kifglobalthread.cxx 2912
"pause", //403 kifglobalthread.cxx 2950
"sleep", //404 kifglobalthread.cxx 2951
"eval", //405 kifglobalthread.cxx 2952
"fromjson", //406 kifglobalthread.cxx 2953
"run", //407 kifglobalthread.cxx 2954
"range", //408 kifglobalthread.cxx 2961
"raise", //409 kifglobalthread.cxx 2973
"block", //410 kifglobalthread.cxx 2990
"unlock", //411 kifglobalthread.cxx 2991
"lock", //412 kifglobalthread.cxx 2992
"wait", //413 kifglobalthread.cxx 2993
"waitonfalse", //414 kifglobalthread.cxx 2994
"waitonjoined", //415 kifglobalthread.cxx 2995
"waiting", //416 kifglobalthread.cxx 2996
"cast", //417 kifglobalthread.cxx 2997
"kill", //418 kifglobalthread.cxx 2998
"version", //419 kifglobalthread.cxx 3000
"print", //420 kifglobalthread.cxx 3001
"exit", //421 kifglobalthread.cxx 3002
"pflush", //422 kifglobalthread.cxx 3003
"println", //423 kifglobalthread.cxx 3004
"printj", //424 kifglobalthread.cxx 3005
"printjerr", //425 kifglobalthread.cxx 3006
"printjln", //426 kifglobalthread.cxx 3007
"printjlnerr", //427 kifglobalthread.cxx 3008
"erroronkey", //428 kifglobalthread.cxx 3009
"predicatedump", //429 kifglobalthread.cxx 3010
"findall", //430 kifglobalthread.cxx 3011
"retractall", //431 kifglobalthread.cxx 3012
"garbagesurvey", //432 kifglobalthread.cxx 3013
"gcsurvey", //433 kifglobalthread.cxx 3014
"garbageframe", //434 kifglobalthread.cxx 3015
"printerr", //435 kifglobalthread.cxx 3016
"printlnerr", //436 kifglobalthread.cxx 3017
"threadid", //437 kifglobalthread.cxx 3023
"parentthreadid", //438 kifglobalthread.cxx 3024
"setdebugfunction", //439 kifglobalthread.cxx 3026
"debugclear", //440 kifglobalthread.cxx 3028
"setdebugdata", //441 kifglobalthread.cxx 3029
"getstackinfo", //442 kifglobalthread.cxx 3031
"inkey", //443 kifglobalthread.cxx 3079
"getc", //444 kifglobalthread.cxx 3104
"random", //445 kifglobalthread.cxx 3107
"valueisa", //446 kifglobalthread.cxx 3183
"weight(): Returns the weight of this predicate.", //447 kifglobalthread.cxx 3215
"query(predicate,var1,var2...): This query is analyzed as a predicate.", //448 kifglobalthread.cxx 3216
"store(sqlite db): Store a predicate in the database. If db is omitted then store it in the knowledge base", //449 kifglobalthread.cxx 3217
"remove(sqlite db): Remove a predicate from the database. If db is omitted then remove it from the knowledge base", //450 kifglobalthread.cxx 3218
"hash(): Return the hash value of a string.", //451 kifglobalthread.cxx 3220
"resize(nb): Resize a rawstring.", //452 kifglobalthread.cxx 3221
"json(): Return the json encoded version of a string.", //453 kifglobalthread.cxx 3222
"byteposition(int pos): convert a character position into a byte position (especially useful in UTF8 strings)", //454 kifglobalthread.cxx 3223
"charposition(int pos): convert a byte position into a character position.", //455 kifglobalthread.cxx 3224
"size(): Return the size of the string.", //456 kifglobalthread.cxx 3225
"evaluate(): evaluate the meta-characters within a string and return the evaluated string.", //457 kifglobalthread.cxx 3226
"clear(): Clean the content of a string.", //458 kifglobalthread.cxx 3227
"tokenize(bool comma,bool separator,bool concatenate): Segment a string into words and punctuations. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token.", //459 kifglobalthread.cxx 3228
"stokenize(map keeps): Segment a string into words and punctuations, with a keep.", //460 kifglobalthread.cxx 3229
"split(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters", //461 kifglobalthread.cxx 3230
"splite(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters. Empty strings are kept in the result.", //462 kifglobalthread.cxx 3231
"removefirst(int nb): remove the first nb characters of a string", //463 kifglobalthread.cxx 3232
"removelast(int nb): remove the last nb characters of a string", //464 kifglobalthread.cxx 3233
"fill(int nb,string c): create a string of nb characters c", //465 kifglobalthread.cxx 3234
"padding(int nb,string c,bool paddattheend): add nb characters c to the current string. Last parameter is optional", //466 kifglobalthread.cxx 3235
"format(p1,p2,p3): Create a new string from the current string in which each '%x' is associated to one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.", //467 kifglobalthread.cxx 3236
"ord(): return the ASCII code of the first character, or a list of all ASCII code if the recipient is a vector", //468 kifglobalthread.cxx 3237
"left(int nb): return the first nb characters of a string", //469 kifglobalthread.cxx 3238
"byteposition(int pos)", //470 kifglobalthread.cxx 3239
"Convert a character position into a byte position", //471 kifglobalthread.cxx 3239
"charposition(int pos)", //472 kifglobalthread.cxx 3240
"Convert a byte position into a character position", //473 kifglobalthread.cxx 3240
"right(int nb): return the last nb characters of a string", //474 kifglobalthread.cxx 3241
"mid(int pos,int nb): return the nb characters starting at position pos of a string", //475 kifglobalthread.cxx 3242
"reverse(): reverse the string", //476 kifglobalthread.cxx 3243
"pop(i): remove character at position i", //477 kifglobalthread.cxx 3244
"pop(): remove last character", //478 kifglobalthread.cxx 3245
"last(): return last character", //479 kifglobalthread.cxx 3246
"insert(i,s): insert the string s at i. If i is -1, then insert s between each character in the input string", //480 kifglobalthread.cxx 3247
"trim(): remove the trailing characters", //481 kifglobalthread.cxx 3248
"trimright(): remove the trailing characters on the right", //482 kifglobalthread.cxx 3249
"trimleft(): remove the trailing characters on the left", //483 kifglobalthread.cxx 3250
"deaccentuate(): Remove the accents from accented characters", //484 kifglobalthread.cxx 3251
"upper(): Return the string in upper characters", //485 kifglobalthread.cxx 3252
"lower(): Return the string in lower characters", //486 kifglobalthread.cxx 3253
"islower(): Test if a string only contains lowercase characters", //487 kifglobalthread.cxx 3254
"isupper(): Test if a string only contains uppercase characters", //488 kifglobalthread.cxx 3255
"isalpha(): Test if a string only contains only alphabetical characters", //489 kifglobalthread.cxx 3256
"isconsonant(): Test if a string only contains consonants", //490 kifglobalthread.cxx 3257
"isvowel(): Test if a string only contains only vowels", //491 kifglobalthread.cxx 3258
"ispunctuation(): Test if the character is a punctuation", //492 kifglobalthread.cxx 3259
"extract(int pos,string from,string up1,string up2...): extract substrings between 'from' and 'up1'...'upn' (the shortest string is used). Return a vector of strings", //493 kifglobalthread.cxx 3260
"isdigit(): Test if a string only contains digits", //494 kifglobalthread.cxx 3261
"latin(): convert an UTF8 string in LATIN", //495 kifglobalthread.cxx 3262
"utf8(int table): convert a LATIN string into UTF8. table is optional, by default it is ISO/IEC 8859 part 1.", //496 kifglobalthread.cxx 3263
"dos(): convert a string into DOS encoding", //497 kifglobalthread.cxx 3264
"dostoutf8(): convert a DOS string into UTF8", //498 kifglobalthread.cxx 3265
"isutf8(): Return true is the string is encoded in UTF8", //499 kifglobalthread.cxx 3266
"ngrams(int r): Return a vector of all ngrams of rank r", //500 kifglobalthread.cxx 3267
"count(string sub,int pos,int mx): Count the number of substrings starting at position pos, ending at mx", //501 kifglobalthread.cxx 3268
"find(string sub,int pos): Return the position of substring sub starting at position pos", //502 kifglobalthread.cxx 3269
"sizeb(): Return the size in bytes of the string", //503 kifglobalthread.cxx 3270
"rfind(string sub,int pos): Return the position of substring sub backward starting at position pos", //504 kifglobalthread.cxx 3271
"replaceregexip(sub,str): Replace the substrings matching sub with str", //505 kifglobalthread.cxx 3272
"levenshtein(string s,bool byte): Return the edit distance with 's' according to Levenshtein algorithm. If byte is true, force a byte level comparison. byte is optionnal.", //506 kifglobalthread.cxx 3273
"editdistance(string s,bool byte): Return the edit distance with 's'. If byte is true, force a byte level comparison. byte is optionnal.", //507 kifglobalthread.cxx 3274
"html(): Return the string into an HTML compatible string or as a vector of strings", //508 kifglobalthread.cxx 3275
"regexip(sub): Find the substrings matching sub", //509 kifglobalthread.cxx 3276
"replace(sub,str): Replace the substrings matching sub with str", //510 kifglobalthread.cxx 3277
"getstd(bool): catch or release the standard output", //511 kifglobalthread.cxx 3278
"geterr(bool): catch or release the error output", //512 kifglobalthread.cxx 3279
"replacergx", //513 kifglobalthread.cxx 3280
"replacergx(rgx,str): Replace the substrings matching rgx with str", //514 kifglobalthread.cxx 3280
"splitrgx", //515 kifglobalthread.cxx 3281
"splitrgx(rgx): Split string with regular expression rgx. Return a vector of substrings.", //516 kifglobalthread.cxx 3281
"regex(rgx): Test if the regular expression rgx applies to string|vector=regex(rgx): Return all substrings matching rgx|string=regex(rgx): Return the substring matching rgx in the string|int=regex(rgx): Return the position of the substring matching rgx in the string", //517 kifglobalthread.cxx 3282
"parse(): Parse a string as a piece of code and returns the evaluation as a vector.", //518 kifglobalthread.cxx 3283
"lisp(): Parse a string as a parenthetic expressions, which is returned as a vector.|lisp(string o,string c): Parse a string as a parenthetic expressions, where the opening and closing characters are provided.", //519 kifglobalthread.cxx 3284
"tags(string o,string c): Parse a string as a parenthetic expression, where the opening and closing strings are provided.", //520 kifglobalthread.cxx 3285
"factors(): return the decomposition in prime factors", //521 kifglobalthread.cxx 3286
"bit(int i): return true if the ith bit is 1.", //522 kifglobalthread.cxx 3287
"log(): return the log base 10 of the value", //523 kifglobalthread.cxx 3290
"ln(): return the neperian log", //524 kifglobalthread.cxx 3292
"abs(): return the absolute value of a number", //525 kifglobalthread.cxx 3293
"exp(): return the exponential", //526 kifglobalthread.cxx 3296
"sqrt(): return the square root", //527 kifglobalthread.cxx 3298
"invert(): Invert the denominator and the numerator of a fraction", //528 kifglobalthread.cxx 3300
"partof(bits x): Return true if the bits of x are all in the current bits", //529 kifglobalthread.cxx 3302
"d(): Denominator of a fraction (with a parameter: set the value, without parameter return the value)", //530 kifglobalthread.cxx 3304
"infos(string): infos about a method", //531 kifglobalthread.cxx 3305
"methods(): Return a vector of all available methods", //532 kifglobalthread.cxx 3306
"simplify(): Simplification", //533 kifglobalthread.cxx 3307
"n(): Numerator of a fraction (with a parameter: set the value, without parameter return the value)", //534 kifglobalthread.cxx 3309
"nd(int n,int d): Set the Numerator and the Denominator of a fraction", //535 kifglobalthread.cxx 3310
"tan(): tangent", //536 kifglobalthread.cxx 3311
"chr(): return the character whose code is the value of the variable", //537 kifglobalthread.cxx 3313
"sin(): sinus", //538 kifglobalthread.cxx 3315
"cos(): cosinus", //539 kifglobalthread.cxx 3317
"atan(): arc tangent", //540 kifglobalthread.cxx 3319
"#(): return the bit complement", //541 kifglobalthread.cxx 3321
"format(string form): return a string formatted according to the pattern in form. (this format is the same as the sprintf format in C++)", //542 kifglobalthread.cxx 3323
"get(): Read a string from keyboard", //543 kifglobalthread.cxx 3325
"bytes(): Return the string as a vector of bytes", //544 kifglobalthread.cxx 3326
"get(): Read a number from keyboard", //545 kifglobalthread.cxx 3327
"resize(nb): Resize a table", //546 kifglobalthread.cxx 3330
"ngrams(int r,string sep): Return a vector of all ngrams of rank r, strings are concatenated with 'sep'", //547 kifglobalthread.cxx 3331
"move(int pfrom,int pto): move the element in pfrom to pto.", //548 kifglobalthread.cxx 3332
"push(a): add a to the vector", //549 kifglobalthread.cxx 3333
"editdistance(s): Return the edit distance with 's'", //550 kifglobalthread.cxx 3334
"flatten(): merge all the sub-vectors elements into the current vector", //551 kifglobalthread.cxx 3335
"merge(v): merge the elements from v into the container", //552 kifglobalthread.cxx 3336
"bytes(): Return the string matching a vector of bytes. Each value should be between 0..255", //553 kifglobalthread.cxx 3337
"totext(): Return the string matching a vector of bytes. Each value should be between 0..255", //554 kifglobalthread.cxx 3338
"test(i): test if i is a valid slot in the vector", //555 kifglobalthread.cxx 3339
"pop(int i): remove the ith element from the vector and return it.", //556 kifglobalthread.cxx 3340
"pop(): remove the last element from the vector and return it.", //557 kifglobalthread.cxx 3341
"last(): return the last element of the vector", //558 kifglobalthread.cxx 3342
"reverse(): reverse the order of the elements in the vector", //559 kifglobalthread.cxx 3343
"join(string sep): concatenate each element in the vector in a string where each element is separated from the others with sep", //560 kifglobalthread.cxx 3344
"reserve(int sz): Reserve sz elements in a vector", //561 kifglobalthread.cxx 3345
"insert(i,x): insert the element x at position i", //562 kifglobalthread.cxx 3346
"sum(): Sum each element with the others", //563 kifglobalthread.cxx 3347
"product(): Multiply each element with the others", //564 kifglobalthread.cxx 3348
"predicate(): Create a new predicate of the form [head,param1,param2..]", //565 kifglobalthread.cxx 3349
"clear(): clean the vector", //566 kifglobalthread.cxx 3350
"min(): return the minimum value in a container", //567 kifglobalthread.cxx 3351
"max(): return the maximum value in a container", //568 kifglobalthread.cxx 3352
"sort(compare): sort the content of the vector according to compare function. compare is optional", //569 kifglobalthread.cxx 3353
"shuffle(): reshuffle the values", //570 kifglobalthread.cxx 3354
"permute(): permute the values in a vector. Return false, when all permutations have been explored.", //571 kifglobalthread.cxx 3355
"unique(): remove duplicates", //572 kifglobalthread.cxx 3356
"sortint(bool order): sort the content of the vector forcing elements to be ints.", //573 kifglobalthread.cxx 3357
"sortfloat(bool order): sort the content of the vector forcing elements to be floats.", //574 kifglobalthread.cxx 3358
"sortlong(bool order): sort the content of the vector forcing elements to be longs.", //575 kifglobalthread.cxx 3359
"sortstring(bool order): sort the content of the vector forcing elements to be strings.", //576 kifglobalthread.cxx 3360
"apply(a,b,c): apply all functions stored in the vector, passing a,b,c etc. as parameters.", //577 kifglobalthread.cxx 3361
"waiton(t1,t2,...,p1,p2..,pn): Waiting for threads, with table as the first argument for of each thread. p1,p2..pn are optional variables that will be passed as next parameters to each thread.", //578 kifglobalthread.cxx 3362
"addtowaiton(t1,t2,...,tn): Addind new threads to the thread waiting list.", //579 kifglobalthread.cxx 3363
"trigger(bool loop): Trigger the threads, if loop is true the function is put in waiting mode back.", //580 kifglobalthread.cxx 3364
"pushfirst(a): add a to the beginning of the list", //581 kifglobalthread.cxx 3366
"test(i): test if i is a valid slot in the list", //582 kifglobalthread.cxx 3369
"pushlast(a): add a at the end of the list", //583 kifglobalthread.cxx 3374
"popfirst(): remove the first element from the list and return it.", //584 kifglobalthread.cxx 3375
"poplast(): remove the last element from the list and return it.", //585 kifglobalthread.cxx 3378
"last(): return the last element of the list", //586 kifglobalthread.cxx 3379
"first(): return the first element of the list", //587 kifglobalthread.cxx 3380
"reverse(): reverse the order of the elements in the list", //588 kifglobalthread.cxx 3381
"join(string sep): concatenate each element in the list in a string where each element is separated from the others with sep", //589 kifglobalthread.cxx 3382
"apply(a,b,c): apply all functions stored in the list, passing a,b,c etc. as parameters.", //590 kifglobalthread.cxx 3386
"clear(): clean the list", //591 kifglobalthread.cxx 3387
"invert(): Invert keys and values in a map", //592 kifglobalthread.cxx 3389
"join(string keysep,string sep): concatenate each element in the map in a string where each key is appended with a keysep and each element is separated from the others with sep", //593 kifglobalthread.cxx 3391
"predicate(): Create a new predicate of the form {'name':name,0:param1,1:param2..]", //594 kifglobalthread.cxx 3395
"test(i): test if i is a valid key in the map", //595 kifglobalthread.cxx 3396
"pop(string key): remove the elements matching key", //596 kifglobalthread.cxx 3398
"keys(): returns the map keys as a vector", //597 kifglobalthread.cxx 3399
"values(): return the values as a vector", //598 kifglobalthread.cxx 3400
"items(): return a vector of key:value", //599 kifglobalthread.cxx 3401
"clear(): clean the map", //600 kifglobalthread.cxx 3404
"evaluation(): evaluate the memory footprint of a primemap", //601 kifglobalthread.cxx 3405
"seeds(ivector iv): set the primemap seed keys", //602 kifglobalthread.cxx 3406
"tree(value): create a tree node, with value as a value. Value can have any types", //603 kifglobalthread.cxx 3408
"sister()|sister(tree n): return the sister node, or add a sister node", //604 kifglobalthread.cxx 3410
"previous|previous(tree n): return the previous node or add n as the previous node", //605 kifglobalthread.cxx 3411
"daughter()|daughter(tree n): return the first child node or add n as the first child node", //606 kifglobalthread.cxx 3412
"mother()|mother(tree n): return the mother node or test if the current node is a parent of n", //607 kifglobalthread.cxx 3413
"lastnode()|lastnode(tree n): return the last node or test if the current node is the last child of n", //608 kifglobalthread.cxx 3414
"isolate(): Extract the current node from its tree", //609 kifglobalthread.cxx 3415
"prune(): Delete the current sub-tree from the global tree", //610 kifglobalthread.cxx 3416
"depth(): Return the depth of the node in the tree", //611 kifglobalthread.cxx 3417
"value(): return the value of the current element", //612 kifglobalthread.cxx 3418
"key(): return the key of the current element", //613 kifglobalthread.cxx 3419
"valuetype(): return the value type of the current element", //614 kifglobalthread.cxx 3420
"valueisa(string type): test the type of the current element", //615 kifglobalthread.cxx 3421
"begin(): initialiaze the iterator with the beginning of the collection", //616 kifglobalthread.cxx 3422
"next(): next element in the collection", //617 kifglobalthread.cxx 3423
"setvalue(value): Set a new value to the current iterator placeholder.", //618 kifglobalthread.cxx 3424
"end(): return true when the end of the collection is reached", //619 kifglobalthread.cxx 3425
"nend(): return true while the end of the collection has not been reached (~end())", //620 kifglobalthread.cxx 3426
"apply(a,b,c): apply a function", //621 kifglobalthread.cxx 3427
"setdate(year,month,day,hour,min,sec): set a time variable", //622 kifglobalthread.cxx 3428
"date(): return the date as a string", //623 kifglobalthread.cxx 3429
"year(): return the year as an integer", //624 kifglobalthread.cxx 3430
"month(): return the month as an integer", //625 kifglobalthread.cxx 3431
"day(): return the day as an integer", //626 kifglobalthread.cxx 3432
"yearday(): return the year day as an integer between 0-365", //627 kifglobalthread.cxx 3433
"weekday(): return the week day as an integer between 0-6, 0 is Sunday.", //628 kifglobalthread.cxx 3434
"hour(): return the hour as an integer", //629 kifglobalthread.cxx 3435
"minute(): return the minutes as an integer", //630 kifglobalthread.cxx 3436
"second(): return the seconds as an integer", //631 kifglobalthread.cxx 3437
"reset (): reinitialize a time variable", //632 kifglobalthread.cxx 3438
"openappend(string filename): open a file in append writing mode", //633 kifglobalthread.cxx 3439
"openwrite(string filename): open a file in write mode", //634 kifglobalthread.cxx 3440
"close(): close a file", //635 kifglobalthread.cxx 3441
"find(string str,bool uppercase): Find a string in a file", //636 kifglobalthread.cxx 3442
"openread(string filename): open a file in read mode", //637 kifglobalthread.cxx 3443
"setsignature(bool s): Set the signature mode", //638 kifglobalthread.cxx 3444
"getsignature(): Return the signature mode", //639 kifglobalthread.cxx 3445
"write(string s1,string s2,): write strings in the file", //640 kifglobalthread.cxx 3446
"writeln(string s1,string s2,): write strings in the file, terminated with a carriage return", //641 kifglobalthread.cxx 3447
"writebin(int s1,int s2,): write character code in a file", //642 kifglobalthread.cxx 3448
"get(): read one character from the file", //643 kifglobalthread.cxx 3449
"unget(): return one character to the stream", //644 kifglobalthread.cxx 3450
"unget(nb): return nb character to the stream", //645 kifglobalthread.cxx 3451
"read(): read a line from a file", //646 kifglobalthread.cxx 3452
"readln(): read a line from a file", //647 kifglobalthread.cxx 3453
"seek(int p): position the file cursor at p", //648 kifglobalthread.cxx 3454
"tell(): return the position of the file cursor", //649 kifglobalthread.cxx 3455
"eof(): return true when the end of file is reached", //650 kifglobalthread.cxx 3456
"load(svector rules): Load a grammar out of a svector. Each line must be a rule.", //651 kifglobalthread.cxx 3458
"apply(string x): Apply the grammar to a string.", //652 kifglobalthread.cxx 3459
"load(string kifpathname,string featchar): Load a word file. Each line should contain one single word. featchar defines the character which starts features, it is optional.", //653 kifglobalthread.cxx 3461
"loadcompact(string kifpathname): Load a file stored in a compact format.", //654 kifglobalthread.cxx 3462
"loadlemma(string kifpathname): Load a word and lemma file. The even line contains a surface form and the odd lines the lemma and features.", //655 kifglobalthread.cxx 3463
"model(function): Set the model function which is used to traverse the automaton", //656 kifglobalthread.cxx 3464
"look(word): Traverses the automaton and returns the most probable readings accoding to the model function", //657 kifglobalthread.cxx 3465
"spotfinalstate(bool): Detect final states in automaton while in 'look'", //658 kifglobalthread.cxx 3466
"settransformationmap(map m): Set the transformation map to set the weights when traversing the automaton with editdistance", //659 kifglobalthread.cxx 3467
"store(string pathname): Store the file into a compact format", //660 kifglobalthread.cxx 3468
"size(): Returns the number of words stored in the automaton", //661 kifglobalthread.cxx 3469
"dump(string path): Dump the automaton content into a file", //662 kifglobalthread.cxx 3470
"setcodefeature(string character): Use this character to detect the features while adding words", //663 kifglobalthread.cxx 3471
"editdistance(string word,int threshold,int flags): Finds words whose edit distance to 'word' is inferior to the threshold. flags values are: 1: action on first character. 2: change character. 4: delete character. 8: insert character.", //664 kifglobalthread.cxx 3472
"clear(): Clears the automaton", //665 kifglobalthread.cxx 3473
"add(string word): add a word to the automaton. add(string word,string lemma): add a word and its lemma to the automaton.", //666 kifglobalthread.cxx 3474
"load(string kifpathname): Load a KiF program", //667 kifglobalthread.cxx 3476
"compile(string kifpathname): Compile a KiF program as a string. Return an handle on the first instruction to execute ", //668 kifglobalthread.cxx 3477
"run(int i): run a program from instruction i (returned by compile)", //669 kifglobalthread.cxx 3478
"kstart(): Return the declared variables as a map", //670 kifglobalthread.cxx 3479
"runasthread(int i): run a program from instruction i (returned by compile) in a thread", //671 kifglobalthread.cxx 3480
"runend(): Return true if the programme execution has been completed", //672 kifglobalthread.cxx 3481
"debugfunction(function,object): set the debug function that will be called during the execution", //673 kifglobalthread.cxx 3482
"debugclose(): Remove the debug mode initiated with debugfunction", //674 kifglobalthread.cxx 3483
"open(): open a KiF session", //675 kifglobalthread.cxx 3484
"clean(): close a KiF session", //676 kifglobalthread.cxx 3485
"name(): Return the pathname of the KiF file", //677 kifglobalthread.cxx 3486
"exposed(): Return the list of methods exposed by KiF", //678 kifglobalthread.cxx 3487
"_loader: A kif variable (of the type described here), which stores a pointer to the loading program.", //679 kifglobalthread.cxx 3488
""};


Exported const char* KIFMAPS[]={
"KIF(881): Error, wrong function arguments", //0 kifmaps.cxx 170
"KIF(689): Circular structure", //1 kifmaps.cxx 1235
""};


Exported const char* KIFPMAPS[]={
"KIF(689): Circular structure", //0 kifpmaps.cxx 762
"KIF(881): Error, wrong function arguments", //1 kifpmaps.cxx 9757
""};


Exported const char* KIFTREEMAPS[]={
"KIF(689): Circular structure", //0 treemaps.cxx 760
"KIF(881): Error, wrong function arguments", //1 treemaps.cxx 9777
""};


Exported const char* KIFAUTOMATON[]={
"KIF(637): Cannot create file", //0 kifautomaton.cxx 208
"KIF(645): The map structure should only contain maps", //1 kifautomaton.cxx 229
"KIF(647): Cannot process this structure", //2 kifautomaton.cxx 256
"r", //3 kifautomaton.cxx 269
"KIF(620): Cannot open file", //4 kifautomaton.cxx 271
"KIF(622): Feature character should be one single character or the empty string", //5 kifautomaton.cxx 277
"KIF(625): Surface+lemma should be in pairs, one on each line", //6 kifautomaton.cxx 315
"KIF(626): Missing value in 'LOADLEMMA'", //7 kifautomaton.cxx 328
"KIF(623): Feature character should be one single character", //8 kifautomaton.cxx 342
"KIF(624): Clear the automaton first.", //9 kifautomaton.cxx 346
"KIF(634): Missing function, cannot look up", //10 kifautomaton.cxx 828
"KIF(621): expected value should be a 'fmap' or a 'vector of vectors'", //11 kifautomaton.cxx 976
"KIF(635): Missing file name", //12 kifautomaton.cxx 1374
"KIF(636): Cannot store in compact format, number of symbols too large", //13 kifautomaton.cxx 1384
"alphabet", //14 kifautomaton.cxx 1387
"arcs", //15 kifautomaton.cxx 1395
"KIF(637): Cannot load compact format", //16 kifautomaton.cxx 1550
"KIF(411): Error after:<", //17 kifautomaton.cxx 1773
"GRM(002): Wrong negation or empty string.", //18 kifautomaton.cxx 1813
"GRM(001): Unknown rule label:", //19 kifautomaton.cxx 1848
""};


Exported const char* KIFCONTAINER[]={
"KIF(689): Circular structure", //0 kifcontainer.cxx 4071
"KIF(877): Missing root in the container", //1 kifcontainer.cxx 7249
"KIF(415): You cannot modify a tuple element", //2 kifcontainer.cxx 8052
"KIF(416): You cannot create an empty tuple", //3 kifcontainer.cxx 8059
"KIF(417): Expecting a container", //4 kifcontainer.cxx 8066
"addtowaiton", //5 kifcontainer.cxx 8711
"resize", //6 kifcontainer.cxx 8711
"count", //7 kifcontainer.cxx 8711
""};


Exported const char* KIFPREDICATE[]={
"PRE(010): Expecting a function", //0 kifpredicate.cxx 112
"PRE(009): You cannot instanciate a predicate this way", //1 kifpredicate.cxx 769
"PRE(001): Cannot instantiate a predicate with this value", //2 kifpredicate.cxx 775
"ponder", //3 kifpredicate.cxx 867
"asserta", //4 kifpredicate.cxx 870
"assertz", //5 kifpredicate.cxx 873
"assertdb", //6 kifpredicate.cxx 876
"retract", //7 kifpredicate.cxx 879
"retractdb", //8 kifpredicate.cxx 882
"name", //9 kifpredicate.cxx 1092
"PRE(003): Unknown predicate name", //10 kifpredicate.cxx 1247
"PRE(004): Missing index parameter in the map", //11 kifpredicate.cxx 1272
"PRE(005): Cannot build a predicate out of that object", //12 kifpredicate.cxx 1281
"unifiable", //13 kifpredicate.cxx 2140
"success:", //14 kifpredicate.cxx 2356
" --> Fail", //15 kifpredicate.cxx 2572
"PRE(006): Expecting a database object", //16 kifpredicate.cxx 3449
""};


Exported const char* KIFRAWSTRINGS[]={
"This wait state does not exist: '", //0 kifraw.cxx 205
"cursor", //1 kifraw.cxx 495
"cursormax", //2 kifraw.cxx 496
"garbage", //3 kifraw.cxx 497
"empty", //4 kifraw.cxx 498
"to_be_cleaned", //5 kifraw.cxx 499
"popping", //6 kifraw.cxx 500
"cursorstring", //7 kifraw.cxx 501
"cursorinteger", //8 kifraw.cxx 502
"cursorfloat", //9 kifraw.cxx 503
"garbagestring", //10 kifraw.cxx 504
"garbageinteger", //11 kifraw.cxx 505
"garbagefloat", //12 kifraw.cxx 506
" (line:%d)", //13 kifraw.cxx 1051
"KIF(831): Wrong opening or closing character in LISP", //14 kifraw.cxx 1076
"KIF(665): Expecting a container in STOKENIZE", //15 kifraw.cxx 1615
"KIF(550): The thread list has already been implemented.", //16 kifraw.cxx 2140
"KIF(551): Expecting threads as arguments.", //17 kifraw.cxx 2156
"KIF(552): This thread has already been inserted into the thread list", //18 kifraw.cxx 2169
"KIF(553): The thread list should be implemented with 'waiton' first.", //19 kifraw.cxx 2198
"KIF(654): Second parameter should be a vector in 'seeds'", //20 kifraw.cxx 2247
"KIF(801): Infinite value", //21 kifraw.cxx 3022
"KIF(802): Not a number", //22 kifraw.cxx 3025
"KIF(877): Expecting a variable", //23 kifraw.cxx 3537
"MATH(100): Error wrong fraction: denominator is 0", //24 kifraw.cxx 3702
"BITS(105): Wrong type of parameter in PartOf", //25 kifraw.cxx 3783
"KIF(543): Cannot intercept the current standard output", //26 kifraw.cxx 3820
"KIF(544): Cannot intercept the current error output", //27 kifraw.cxx 3837
"KIF(124):Incorrect format specifier or size too long", //28 kifraw.cxx 5492
"KIF(635): This function must be called with an automaton variable", //29 kifraw.cxx 5872
"KIF(832): Argument cannot be processed in 'add' for 'automaton'.", //30 kifraw.cxx 6096
"KIF(644): The structure should be a map of maps", //31 kifraw.cxx 6130
""};


Exported const char* KIFLIBSTRINGS[]={
"KIF(0): Wrong number of parameters in SPLIT", //0 kiflibs.cxx 112
"KIF(1): Wrong 3rd parameter in SPLIT", //1 kiflibs.cxx 113
"KIF(2): Wrong number of parameters in LIST", //2 kiflibs.cxx 114
"KIF(3): Wrong number of parameters in UTF8", //3 kiflibs.cxx 115
"KIF(4): Wrong number of parameters in LATIN", //4 kiflibs.cxx 116
"KIF(5): Wrong number of parameters in LEFT", //5 kiflibs.cxx 117
"KIF(6): Wrong number of parameters in RIGHT", //6 kiflibs.cxx 118
"KIF(7): Wrong number of parameters in MID", //7 kiflibs.cxx 119
"KIF(8): Wrong number of parameters in ISLOWER", //8 kiflibs.cxx 120
"KIF(9): Wrong number of parameters in ISUPPER", //9 kiflibs.cxx 121
"KIF(10): Wrong number of parameters in LOWER", //10 kiflibs.cxx 122
"KIF(11): Wrong number of parameters in UPPER", //11 kiflibs.cxx 123
"KIF(12): Wrong number of parameters in TRIM", //12 kiflibs.cxx 124
"KIF(13): Wrong number of parameters in SETDATE", //13 kiflibs.cxx 125
"KIF(14): Wrong time parameter in SETDATE", //14 kiflibs.cxx 126
"KIF(15): Wrong number of parameters in DATE", //15 kiflibs.cxx 127
"KIF(16): Wrong time parameter in DATE", //16 kiflibs.cxx 128
"KIF(17): Wrong number of parameters in YEAR", //17 kiflibs.cxx 129
"KIF(18): Wrong time parameter in YEAR", //18 kiflibs.cxx 130
"KIF(19): Wrong number of parameters in DAY", //19 kiflibs.cxx 131
"KIF(20): Wrong time parameter in DAY", //20 kiflibs.cxx 132
"KIF(21): Wrong number of parameters in HOUR", //21 kiflibs.cxx 133
"KIF(22): Wrong time parameter in HOUR", //22 kiflibs.cxx 134
"KIF(23): Wrong number of parameters in MONTH", //23 kiflibs.cxx 135
"KIF(24): Wrong time parameter in MONTH", //24 kiflibs.cxx 136
"KIF(25): Missing parameter in RAISE", //25 kiflibs.cxx 137
"KIF(26): Wrong number of parameters in WRITE", //26 kiflibs.cxx 138
"KIF(27): First parameter in WRITE should be a file", //27 kiflibs.cxx 139
"KIF(28): First parameter in READ should be a file", //28 kiflibs.cxx 140
"KIF(29): First parameter in READLN should be a file", //29 kiflibs.cxx 141
"KIF(30): First parameter in CLOSE should be a file", //30 kiflibs.cxx 142
"KIF(31): First parameter in OPENREAD should be a file", //31 kiflibs.cxx 143
"KIF(32): Error in OPENREAD while opening the file:", //32 kiflibs.cxx 144
"KIF(33): First parameter in EOF should be a file", //33 kiflibs.cxx 145
"KIF(34): First parameter in OPENWRITE should be a file", //34 kiflibs.cxx 146
"KIF(35): Error in OPENWRITE while opening the file:", //35 kiflibs.cxx 147
"KIF(36): Wrong number of parameters in INSERT", //36 kiflibs.cxx 148
"KIF(37): Wrong call to INSERT", //37 kiflibs.cxx 149
"KIF(38): Unknown function: ", //38 kiflibs.cxx 150
"KIF(39): Wrong call to a PUSH", //39 kiflibs.cxx 151
"KIF(40): Wrong number of parameters in POP", //40 kiflibs.cxx 152
"KIF(41): Wrong call to a POP", //41 kiflibs.cxx 153
"KIF(42): Wrong number of parameters in LAST", //42 kiflibs.cxx 154
"KIF(43): Unknown NODE in LAST", //43 kiflibs.cxx 155
"XIP(44): Unknown GENERATION NODE in LAST", //44 kiflibs.cxx 156
"KIF(45): Wrong call to LAST", //45 kiflibs.cxx 157
"KIF(46): Wrong number of parameters in SORT", //46 kiflibs.cxx 158
"KIF(47): Wrong call to a SORT", //47 kiflibs.cxx 159
"KIF(48): Missing parameters in APPLY", //48 kiflibs.cxx 160
"KIF(49): Wrong call to APPLY", //49 kiflibs.cxx 161
"XIP(50): Wrong number of parameters in CREATEDEPENDENCY", //50 kiflibs.cxx 162
"XIP(51): Unknown dependency:", //51 kiflibs.cxx 163
"XIP(52): 2nd parameter should be a MAP in CREATEDEPENDENCY", //52 kiflibs.cxx 164
"XIP(53): 3rd parameter should be a VECTOR in CREATEDEPENDENCY", //53 kiflibs.cxx 165
"XIP(54): 3rd parameter should only contain NODE in CREATEDEPENDENCY", //54 kiflibs.cxx 166
"XIP(55): 3rd parameter should only contain NODE in CREATEDEPENDENCY", //55 kiflibs.cxx 167
"XIP(56): Unknown feature: ", //56 kiflibs.cxx 168
"XIP(57): Dependency not created", //57 kiflibs.cxx 169
"XIP(58): Wrong number of parameters in SETFEATURE", //58 kiflibs.cxx 170
"XIP(59): Wrong type of variable in SETFEATURE", //59 kiflibs.cxx 171
"XIP(60): Unknown feature: ", //60 kiflibs.cxx 172
"XIP(61): Wrong number of parameters in REMOVEFEATURE", //61 kiflibs.cxx 173
"XIP(62): Wrong type of variable in REMOVEFEATURE", //62 kiflibs.cxx 174
"XIP(63): Unknown attribute: ", //63 kiflibs.cxx 175
"XIP(64): Wrong number of parameters in FEATURE", //64 kiflibs.cxx 176
"XIP(65): Wrong type of variable in FEATURE", //65 kiflibs.cxx 177
"XIP(66): Unknown attribute: ", //66 kiflibs.cxx 178
"XIP(67): Unknown feature value: ", //67 kiflibs.cxx 179
"XIP(68): Parameter should be either a VECTOR or a MAP in CLEAR", //68 kiflibs.cxx 180
"XIP(69): Second parameter should be a MAP in DATA", //69 kiflibs.cxx 181
"XIP(70): Unknown NODE in DATA", //70 kiflibs.cxx 182
"XIP(71): Unknown DEPENDENCY in DATA", //71 kiflibs.cxx 183
"XIP(72): No DATA for this variable", //72 kiflibs.cxx 184
"XIP(73): Unknown NODE in NAME", //73 kiflibs.cxx 185
"XIP(74): Unknown DEPENDENCY in NAME", //74 kiflibs.cxx 186
"XIP(75): No NAME for this variable", //75 kiflibs.cxx 187
"XIP(76): Wrong number of parameters in STACK", //76 kiflibs.cxx 188
"XIP(77): 2nd parameter should be a VECTOR in STACK", //77 kiflibs.cxx 189
"XIP(78): Unknown dependency in STACK", //78 kiflibs.cxx 190
"XIP(79): No STACK for this variable", //79 kiflibs.cxx 191
"KIF(80): Wrong number of parameters in PARAMETERS", //80 kiflibs.cxx 192
"KIF(81): 2nd parameter should be a VECTOR in PARAMETERS", //81 kiflibs.cxx 193
"XIP(82): Unknown dependency in PARAMETERS", //82 kiflibs.cxx 194
"KIF(83): No PARAMETERS for this variable", //83 kiflibs.cxx 195
"KIF(84): Wrong number of parameters in REMOVEFIRST", //84 kiflibs.cxx 196
"KIF(85): No REMOVEFIRST for this variable", //85 kiflibs.cxx 197
"KIF(86): Wrong number of parameters in INSTANCES", //86 kiflibs.cxx 198
"KIF(87): No INSTANCES for this variable", //87 kiflibs.cxx 199
"KIF(88): Wrong number of parameters in SET", //88 kiflibs.cxx 200
"KIF(89): Wrong number of parameters in REGEX", //89 kiflibs.cxx 201
"KIF(90): Missing class in MATRIX SET", //90 kiflibs.cxx 202
"KIF(91): Missing attribute in MATRIX SET", //91 kiflibs.cxx 203
"KIF(92): No SET for this variable", //92 kiflibs.cxx 204
"KIF(93): Wrong number of parameters in GET", //93 kiflibs.cxx 205
"KIF(94): Missing class in MATRIX GET", //94 kiflibs.cxx 206
"KIF(95): Missing attribute in MATRIX GET", //95 kiflibs.cxx 207
"XIP(96): No GET for this variable", //96 kiflibs.cxx 208
"XIP(97): Wrong number of parameters in OFFSET", //97 kiflibs.cxx 209
"XIP(98): Unknown node in OFFSET", //98 kiflibs.cxx 210
"XIP(99): No OFFSET for this variable", //99 kiflibs.cxx 211
"XIP(100): Wrong number of parameters in OFFSETCHAR", //100 kiflibs.cxx 212
"XIP(101): Unknown node in OFFSETCHAR", //101 kiflibs.cxx 213
"XIP(102): No OFFSETCHAR for this variable", //102 kiflibs.cxx 214
"XIP(103): Wrong number of parameters in TOKENOFFSET", //103 kiflibs.cxx 215
"XIP(104): Unknown node in TOKENOFFSET", //104 kiflibs.cxx 216
"XIP(105): No TOKENOFFSET for this variable", //105 kiflibs.cxx 217
"XIP(106): Wrong number of parameters in LEFTOFFSET", //106 kiflibs.cxx 218
"XIP(107): Unknown node in LEFTOFFSET", //107 kiflibs.cxx 219
"XIP(108): No LEFTOFFSET for this variable", //108 kiflibs.cxx 220
"XIP(109): Wrong number of parameters in RIGHTOFFSET", //109 kiflibs.cxx 221
"XIP(110): Unknown node in RIGHTOFFSET", //110 kiflibs.cxx 222
"XIP(111): No RIGHTOFFSET for this variable", //111 kiflibs.cxx 223
"XIP(112): Wrong number of parameters in LEFTOFFSETCHAR", //112 kiflibs.cxx 224
"XIP(113): Unknown node in LEFTOFFSETCHAR", //113 kiflibs.cxx 225
"XIP(114): No LEFTOFFSETCHAR for this variable", //114 kiflibs.cxx 226
"XIP(115): Wrong number of parameters in RIGHTOFFSETCHAR", //115 kiflibs.cxx 227
"XIP(116): Unknown node in RIGHTOFFSETCHAR", //116 kiflibs.cxx 228
"XIP(117): No RIGHTOFFSETCHAR for this variable", //117 kiflibs.cxx 229
"XIP(118): Wrong number of parameters in LEFTTOKENOFFSET", //118 kiflibs.cxx 230
"XIP(119): Unknown node in LEFTTOKENOFFSET", //119 kiflibs.cxx 231
"XIP(120): No LEFTTOKENOFFSET for this variable", //120 kiflibs.cxx 232
"XIP(121): Wrong number of parameters in RIGHTTOKENOFFSET", //121 kiflibs.cxx 233
"XIP(122): Unknown node in RIGHTTOKENOFFSET", //122 kiflibs.cxx 234
"XIP(123): No RIGHTTOKENOFFSET for this variable", //123 kiflibs.cxx 235
"XIP(124): No LEMMA for this variable", //124 kiflibs.cxx 236
"XIP(125): Wrong number of parameters in REMOVELAST", //125 kiflibs.cxx 237
"XIP(126): No REMOVELAST for this variable", //126 kiflibs.cxx 238
"XIP(127): No SURFACE for this variable", //127 kiflibs.cxx 239
"XIP(128): Wrong number of parameters in REPLACE", //128 kiflibs.cxx 240
"XIP(129): Wrong number of parameters in LOAD", //129 kiflibs.cxx 241
"XIP(130): No XMLNODE for this NODE", //130 kiflibs.cxx 242
"KIF(131): Unknown parameter in LOAD", //131 kiflibs.cxx 243
"KIF(132): BEGIN failed for this ITERATOR", //132 kiflibs.cxx 244
"KIF(133): END failed for this ITERATOR", //133 kiflibs.cxx 245
"KIF(134): NEND failed for this ITERATOR", //134 kiflibs.cxx 246
"KIF(135): No KEY for this ITERATOR", //135 kiflibs.cxx 247
"KIF(136): No VALUE for this ITERATOR", //136 kiflibs.cxx 248
"KIF(137): No VALUETYPE for this ITERATOR", //137 kiflibs.cxx 249
"XIP(138): Unknown NODE in NEXT", //138 kiflibs.cxx 250
"XIP(139): Unknown GENERATION NODE in NEXT", //139 kiflibs.cxx 251
"XIP(140): Unknown XML in NEXT", //140 kiflibs.cxx 252
"XIP(141): No NEXT for this variable", //141 kiflibs.cxx 253
"XIP(142): Unknown NODE in PREVIOUS", //142 kiflibs.cxx 254
"XIP(143): Unknown GENERATION NODE in PREVIOUS", //143 kiflibs.cxx 255
"XIP(144): Unknown XML in PREVIOUS", //144 kiflibs.cxx 256
"XIP(145): No PREVIOUS for this variable", //145 kiflibs.cxx 257
"XIP(146): Unknown NODE in PARENT", //146 kiflibs.cxx 258
"XIP(147): Unknown GENERATION NODE in PARENT", //147 kiflibs.cxx 259
"XIP(148): Unknown XML in PARENT", //148 kiflibs.cxx 260
"XIP(149): No PARENT for this variable", //149 kiflibs.cxx 261
"XIP(150): Unknown NODE in CHILD", //150 kiflibs.cxx 262
"XIP(151): Unknown GENERATION NODE in CHILD", //151 kiflibs.cxx 263
"XIP(152): Unknown XML in CHILD", //152 kiflibs.cxx 264
"XIP(153): No CHILD for this variable", //153 kiflibs.cxx 265
"KIF(154): Wrong TYPE for a WRITE", //154 kiflibs.cxx 266
"KIF(155): Wrong frame affectation", //155 kiflibs.cxx 267
"KIF(156): Wrong CALL affectation", //156 kiflibs.cxx 268
"XIP(157): ", //157 kiflibs.cxx 269
"KIF(158): Please check the number of parameters in: ", //158 kiflibs.cxx 270
"KIF(159): Please check the parameters in: ", //159 kiflibs.cxx 271
"KIF(160): Wrong frame initialisation", //160 kiflibs.cxx 272
"KIF(161): Wrong number of parameters in SEEK", //161 kiflibs.cxx 273
"KIF(162): First parameter shoud be a file in SEEK", //162 kiflibs.cxx 274
"KIF(163): Wrong number of parameters in TELL", //163 kiflibs.cxx 275
"KIF(164): First parameter shoud be a file in TELL", //164 kiflibs.cxx 276
"KIF(165): Wrong number of parameters in JOIN", //165 kiflibs.cxx 277
"KIF(166): Wrong parameter in JOIN", //166 kiflibs.cxx 278
"KIF(167): Wrong number of parameters in COMPILE", //167 kiflibs.cxx 279
"KIF(168): Wrong parameter in COMPILE", //168 kiflibs.cxx 280
"KIF(169): Wrong number of parameters in SUM", //169 kiflibs.cxx 281
"KIF(170): Wrong parameter in SUM", //170 kiflibs.cxx 282
"KIF(171): Wrong number of parameters in PRODUCT", //171 kiflibs.cxx 283
"KIF(172): Wrong parameter in PRODUCT", //172 kiflibs.cxx 284
"KIF(173): Wrong number of parameters in UNGET", //173 kiflibs.cxx 285
"KIF(174): No UNGET for this variable", //174 kiflibs.cxx 286
"KIF(175): Wrong number of parameters in REVERSE", //175 kiflibs.cxx 287
"KIF(176): Wrong number of parameters in EXTRACT", //176 kiflibs.cxx 288
"KIF(177): Wrong parameter in EXTRACT", //177 kiflibs.cxx 289
"KIF(178): Unknown function: ", //178 kiflibs.cxx 290
"XIP(179): This parser has already been loaded", //179 kiflibs.cxx 291
"XIP(180): Wrong number of parameters in PARSE", //180 kiflibs.cxx 292
"XIP(181): Empty grammar in PARSE", //181 kiflibs.cxx 293
"XIP(182): Wrong parameters in PARSE", //182 kiflibs.cxx 294
"KIF(183): Unknown instance: ", //183 kiflibs.cxx 295
"KIF(184): Loading error: ", //184 kiflibs.cxx 296
"KIF(185): Missing parameter in HASH", //185 kiflibs.cxx 297
"KIF(186): Wrong number of parameters in VALUES", //186 kiflibs.cxx 298
"KIF(187): Wrong type of parameter in VALUES", //187 kiflibs.cxx 299
"KIF(188): Wrong number of parameter or wrong type in SPLITRGX", //188 kiflibs.cxx 300
"KIF(189): library failed to load: ", //189 kiflibs.cxx 301
"KIF(190): Missing Entry Point KifInitialisationModule in: ", //190 kiflibs.cxx 302
"KIF(191): Wrong number of parameters in FILEINFO", //191 kiflibs.cxx 303
"KIF(192): Wrong number of parameters in KEYS", //192 kiflibs.cxx 304
"KIF(193): Wrong type of parameter in KEYS", //193 kiflibs.cxx 305
"KIF(194): Wrong number of parameters in FIND", //194 kiflibs.cxx 306
"KIF(195): Wrong number of parameters in COMP", //195 kiflibs.cxx 307
"KIF(196): Wrong type of parameters in: ", //196 kiflibs.cxx 308
"KIF(197): Wrong number of parameters in KILL", //197 kiflibs.cxx 309
"KIF(198): Wrong number of parameters in CAST", //198 kiflibs.cxx 310
"KIF(199): Wrong number of parameters in WAIT", //199 kiflibs.cxx 311
"KIF(200): Wrong number of parameters in WAITING", //200 kiflibs.cxx 312
"KIF(201): Wrong parameter: expecting a TREE element", //201 kiflibs.cxx 313
"KIF(202): Missing TREE parameter", //202 kiflibs.cxx 314
"KIF(203): Wrong number of parameters", //203 kiflibs.cxx 315
"KIF(204): Error: The parameter TREE is a parent of the target TREE", //204 kiflibs.cxx 316
"KIF(205): You cannot move a node to itself", //205 kiflibs.cxx 317
"KIF(206): Cannot initialize a vector with this value", //206 kiflibs.cxx 318
"KIF(207): Cannot initialize a map with this value", //207 kiflibs.cxx 319
"KIF(208): Wrong number of parameters in FILL", //208 kiflibs.cxx 320
"KIF(209): Too long a size in FILL", //209 kiflibs.cxx 321
"KIF(210): Wrong initial parameter in FILL", //210 kiflibs.cxx 322
"KIF(211): Cannot create THREAD", //211 kiflibs.cxx 323
"KIF(212): Only one WAIT string is accepted in a thread", //212 kiflibs.cxx 324
"XIP(213): WAITALL is not accepted in a thread", //213 kiflibs.cxx 325
"KIF(214): No parameters expected in GETC", //214 kiflibs.cxx 326
"XIP(215): Wrong parameters in ADDOPTION", //215 kiflibs.cxx 327
"XIP(216): Wrong parameters in REMOVEOPTION", //216 kiflibs.cxx 328
"KIF(217): WAIT are forbiden in a CATCH section", //217 kiflibs.cxx 329
"KIF(218): Wrong number of parameters in RESET", //218 kiflibs.cxx 330
"KIF(219): Wrong parameter in RESET", //219 kiflibs.cxx 331
"KIF(220): Cannot initialize a list with this value", //220 kiflibs.cxx 332
"KIF(221): Session already opened", //221 kiflibs.cxx 333
"KIF(222): Unknown session", //222 kiflibs.cxx 334
"KIF(223): Wrong key in a container access", //223 kiflibs.cxx 335
"XIP(224): ", //224 kiflibs.cxx 336
"KIF(225): Divided by zero", //225 kiflibs.cxx 337
"KIF(226): Wrong number of parameters in RANGE", //226 kiflibs.cxx 338
"KIF(227): RANGE over actual capacity", //227 kiflibs.cxx 339
"KIF(228): Wrong number of parameter or wrong type in FORMAT", //228 kiflibs.cxx 340
"XIP(229): Unknown function: ", //229 kiflibs.cxx 341
"KIF(230): String should be one character long in RANGE", //230 kiflibs.cxx 342
"KIF(231): No compatible function found or unknown function: ", //231 kiflibs.cxx 343
"KIF(232): Empty CALL variable", //232 kiflibs.cxx 344
"KIF(233): Unknown object: ", //233 kiflibs.cxx 345
"KIF(234): Object already exists: ", //234 kiflibs.cxx 346
"KIF(235): Wrong use of 'this': ", //235 kiflibs.cxx 347
"KIF(236): Wrong call to a function in a APPLY", //236 kiflibs.cxx 348
"KIF(237): Missing parameter in SYNCHRONIZED", //237 kiflibs.cxx 349
"KIF(238): Missing parameter in LOCK", //238 kiflibs.cxx 350
"KIF(239): Missing parameter in UNLOCK", //239 kiflibs.cxx 351
"KIF(240): Unknown lock: ", //240 kiflibs.cxx 352
"KIF(241): Bad use of RETURN", //241 kiflibs.cxx 353
"KIF(242): Wrong comparison definition", //242 kiflibs.cxx 354
"KIF(243): Instruction unavailable for this element", //243 kiflibs.cxx 355
"KIF(244): Unknown expression", //244 kiflibs.cxx 356
"KIF(245): Unknown parameter in FILE: should be 'r','w', 'w+' or 'a'", //245 kiflibs.cxx 357
"KIF(246): Wrong number of parameter or wrong type in EXPOSED", //246 kiflibs.cxx 358
"KIF(247): Unknown parameter in EXPOSED", //247 kiflibs.cxx 359
"KIF(248): Wrong number of parameter or wrong type in TEST", //248 kiflibs.cxx 360
"KIF(249): Wrong number of parameter or wrong type in _ID", //249 kiflibs.cxx 361
"KIF(250): Cannot modify constant value", //250 kiflibs.cxx 362
"KIF(251): Wrong number of parameter or wrong type in BYTEPOSITION", //251 kiflibs.cxx 363
"KIF(252): Wrong number of parameter or wrong type in ISUTF8", //252 kiflibs.cxx 364
"KIF(253): Wrong number of parameter in WRITEBIN", //253 kiflibs.cxx 365
"KIF(254): Character code is not ASCII in WRITEBIN", //254 kiflibs.cxx 366
"KIF(255): Wrong number of parameter in READ", //255 kiflibs.cxx 367
"KIF(256): Unknown external library function", //256 kiflibs.cxx 368
"KIF(257): Wrong number of parameters in: ", //257 kiflibs.cxx 369
"KIF(258): A function is requested as parameter in SETDEBUGFUNCTION: ", //258 kiflibs.cxx 370
"KIF(259): Wrong stack index: ", //259 kiflibs.cxx 371
"KIF(260): Expecting a kif object: ", //260 kiflibs.cxx 372
"KIF(261): Empty kif object: ", //261 kiflibs.cxx 373
"KIF(262): Wrong number of parameters in: ", //262 kiflibs.cxx 374
"KIF(263): Cannot store this value: infinite reference ", //263 kiflibs.cxx 375
"KIF(264): Empty call function", //264 kiflibs.cxx 376
"KIF(265): Cannot store an 'empty' value", //265 kiflibs.cxx 377
"KIF(266): Expecting one parameter for this object", //266 kiflibs.cxx 378
"KIF(267): TABLE is full", //267 kiflibs.cxx 379
"KIF(268): Can only iterate on a vector", //268 kiflibs.cxx 380
"KIF(269): Vector structure does not match element", //269 kiflibs.cxx 381
"KIF(270): Can only iterate on a map", //270 kiflibs.cxx 382
"KIF(271): Map structure does not match element", //271 kiflibs.cxx 383
"KIF(272): Wrong year value", //272 kiflibs.cxx 384
"KIF(273): Wrong month value", //273 kiflibs.cxx 385
"KIF(274): Wrong day value", //274 kiflibs.cxx 386
"KIF(275): Wrong hour value", //275 kiflibs.cxx 387
"KIF(276): Wrong minute or second value", //276 kiflibs.cxx 388
"Copyright (C) Xerox Corporation 2010-2014", //277 kiflibs.cxx 483
"Xerox Research Center Europe", //278 kiflibs.cxx 485
"Implementation: Claude Roux", //279 kiflibs.cxx 487
"KIF(999): Please set KIFLIBS", //280 kiflibs.cxx 538
"KIF(881): Error, wrong function arguments", //281 kiflibs.cxx 1054
"Unknown function: '", //282 kiflibs.cxx 1062
"Unknown variable or XIP declared variable: '", //283 kiflibs.cxx 1131
"Unknown variable: '", //284 kiflibs.cxx 1141
"Expecting a variable in FOR", //285 kiflibs.cxx 1378
"Only numerical variable can be used here", //286 kiflibs.cxx 1384
"break", //287 kiflibs.cxx 1489
"continue", //288 kiflibs.cxx 1492
"return", //289 kiflibs.cxx 1495
"_return", //290 kiflibs.cxx 1495
"setstacklimit", //291 kiflibs.cxx 1576
"Missing parameter in 'setstacklimit'", //292 kiflibs.cxx 1587
"setjoinedlimit", //293 kiflibs.cxx 1593
"Missing parameter in 'setjoinedlimit'", //294 kiflibs.cxx 1605
"setthreadlimit", //295 kiflibs.cxx 1611
"Missing parameter in 'setthreadlimit'", //296 kiflibs.cxx 1621
"garbagesize", //297 kiflibs.cxx 1627
"Missing parameter in 'garbagesize'", //298 kiflibs.cxx 1641
"garbagefunction", //299 kiflibs.cxx 1647
"Missing parameter in 'garbagefunction'", //300 kiflibs.cxx 1662
"setenv", //301 kiflibs.cxx 1668
"Missing parameter in 'setenv'", //302 kiflibs.cxx 1670
"use", //303 kiflibs.cxx 1689
"Missing parameter in 'use'", //304 kiflibs.cxx 1738
"loadin", //305 kiflibs.cxx 1744
"Missing parameter in 'loadin'", //306 kiflibs.cxx 1770
"Unknown function call: ", //307 kiflibs.cxx 1857
"No compatible function found for:", //308 kiflibs.cxx 1931
"vector", //309 kiflibs.cxx 1989
"string", //310 kiflibs.cxx 1989
"Wrong interval for this variable:", //311 kiflibs.cxx 1990
" type is=", //312 kiflibs.cxx 1990
"this", //313 kiflibs.cxx 2385
"Wrong call with 'this'", //314 kiflibs.cxx 2387
"Invalid variable: '", //315 kiflibs.cxx 2477
"true", //316 kiflibs.cxx 2559
"Variable: '", //317 kiflibs.cxx 2642
"' has already been declared", //318 kiflibs.cxx 2644
"Common variable can only be declared in a frame: ", //319 kiflibs.cxx 2671
"Unknown type: '", //320 kiflibs.cxx 2780
"predicatevar", //321 kiflibs.cxx 2972
"private", //322 kiflibs.cxx 3213
"common", //323 kiflibs.cxx 3219
"const", //324 kiflibs.cxx 3222
"range", //325 kiflibs.cxx 3353
"ord", //326 kiflibs.cxx 3376
"Error: Already defined label: '", //327 kiflibs.cxx 3624
"store", //328 kiflibs.cxx 3941
"remove", //329 kiflibs.cxx 3944
"assertdb", //330 kiflibs.cxx 4058
"retractdb", //331 kiflibs.cxx 4066
"Error: Wrong assertdb or retractdb definition", //332 kiflibs.cxx 4078
"Error: Wrong assert or retract definition", //333 kiflibs.cxx 4086
"let", //334 kiflibs.cxx 4214
"' already declared", //335 kiflibs.cxx 4361
"and", //336 kiflibs.cxx 4616
"list", //337 kiflibs.cxx 4813
"dropWhile", //338 kiflibs.cxx 5102
"take", //339 kiflibs.cxx 5124
"drop", //340 kiflibs.cxx 5124
"filter", //341 kiflibs.cxx 5231
"repeat", //342 kiflibs.cxx 5576
"cycle", //343 kiflibs.cxx 5579
"Error: Predefined function, consider choosing another name: '", //344 kiflibs.cxx 5752
"Error: A function with this name already exists: '", //345 kiflibs.cxx 5760
"iterator", //346 kiflibs.cxx 5915
"Error: You cannot declare a function within a function: '", //347 kiflibs.cxx 6052
"joined", //348 kiflibs.cxx 6097
"protected", //349 kiflibs.cxx 6102
"exclusive", //350 kiflibs.cxx 6107
"strict", //351 kiflibs.cxx 6116
"polynomial", //352 kiflibs.cxx 6123
"thread", //353 kiflibs.cxx 6126
"autorun", //354 kiflibs.cxx 6133
"Error: This function has already been used in a call: '", //355 kiflibs.cxx 6162
"Error: An AUTORUN cannot have parameters: '", //356 kiflibs.cxx 6199
"KIF(642): Error unknown extension", //357 kiflibs.cxx 6219
"KIF(643): Error unknown function extension", //358 kiflibs.cxx 6236
"Error: Unknown type: '", //359 kiflibs.cxx 6272
"self", //360 kiflibs.cxx 6290
"Error: Cannot use this name as a function extension: '", //361 kiflibs.cxx 6309
"Error: Cannot extend this type: '", //362 kiflibs.cxx 6350
"Error: attempt to use private frame:", //363 kiflibs.cxx 6385
"Error: This frame has already been declared:", //364 kiflibs.cxx 6410
"Cannot open kif file: ", //365 kiflibs.cxx 6547
"float f,g,h; int i,j,k; string s,t,u; vector v; map m;\n", //366 kiflibs.cxx 6828
"hence", //367 kiflibs.cxx 6850
" (line:%d)", //368 kiflibs.cxx 6859
"Error while reading KIF file: ", //369 kiflibs.cxx 6913
"Unknown file: ", //370 kiflibs.cxx 6915
"Unknown file:", //371 kiflibs.cxx 7469
"a 'self' object can only call functions: ", //372 kiflibs.cxx 7859
"partof", //373 kiflibs.cxx 7866
"Only functions can be called from an index: ", //374 kiflibs.cxx 7888
"methods", //375 kiflibs.cxx 8290
"map", //376 kiflibs.cxx 8292
"_current", //377 kiflibs.cxx 8356
"a_first", //378 kiflibs.cxx 8401
"a_change", //379 kiflibs.cxx 8406
"a_delete", //380 kiflibs.cxx 8411
"a_insert", //381 kiflibs.cxx 8416
"a_last", //382 kiflibs.cxx 8421
"a_switch", //383 kiflibs.cxx 8426
"a_prefix", //384 kiflibs.cxx 8431
"a_nocase", //385 kiflibs.cxx 8436
"a_surface", //386 kiflibs.cxx 8441
"a_full", //387 kiflibs.cxx 8446
"a_split", //388 kiflibs.cxx 8451
"a_skip", //389 kiflibs.cxx 8456
"a_track", //390 kiflibs.cxx 8461
"pred_none", //391 kiflibs.cxx 8466
"pred_weight", //392 kiflibs.cxx 8471
"pred_weightstack", //393 kiflibs.cxx 8476
"pred_normalize", //394 kiflibs.cxx 8481
"pred_randomize", //395 kiflibs.cxx 8486
"_KIFMAIN", //396 kiflibs.cxx 8643
"FRAME::", //397 kiflibs.cxx 8666
"FUNCTION::", //398 kiflibs.cxx 8688
"h for help", //399 kiflibs.cxx 8800
"End", //400 kiflibs.cxx 8932
"$: stop debugging", //401 kiflibs.cxx 8970
"$$: termination", //402 kiflibs.cxx 8971
"o: leave current function", //403 kiflibs.cxx 8972
"i: inside function", //404 kiflibs.cxx 8973
"f: File indexes", //405 kiflibs.cxx 8974
"g line (file)|label: goto line", //406 kiflibs.cxx 8975
"b line (file)|label: breakpoints (if line==-1: deletes all breakpoints. Without parameter: display breakpoints)", //407 kiflibs.cxx 8976
"s: stack", //408 kiflibs.cxx 8977
"n (nb lines): list following instructions", //409 kiflibs.cxx 8978
"l f t: list line from f to t", //410 kiflibs.cxx 8979
"w variable: add a watch on a variable", //411 kiflibs.cxx 8980
"r variable: remove a watch on a variable", //412 kiflibs.cxx 8981
"H: command history", //413 kiflibs.cxx 8982
"!nb: command renacted", //414 kiflibs.cxx 8983
"<: down in history", //415 kiflibs.cxx 8984
">: up in history", //416 kiflibs.cxx 8985
"w", //417 kiflibs.cxx 9009
"r", //418 kiflibs.cxx 9030
"Unknown label: ", //419 kiflibs.cxx 9059
"Line:", //420 kiflibs.cxx 9078
" in ", //421 kiflibs.cxx 9078
"All breakpoints deleted", //422 kiflibs.cxx 9087
"Breakpoint cleaned", //423 kiflibs.cxx 9106
"Instruction", //424 kiflibs.cxx 9134
"line %d in %s", //425 kiflibs.cxx 9140
"Unknown variable", //426 kiflibs.cxx 9204
"null", //427 kiflibs.cxx 9219
""};


Exported const char* KIFSPARSESTRINGS[]={
"SPM(100): Wrong index", //0 kifsparse.cxx 433
"name", //1 kifsparse.cxx 532
"size", //2 kifsparse.cxx 563
"MAT(131): Wrong index or Wrong value (recipient should be a map or a mvector)", //3 kifsparse.cxx 1173
"MAT(123): Wrong recipient variable type", //4 kifsparse.cxx 1356
"MAT(105): wrong parameter, missing a sparse matrix as result", //5 kifsparse.cxx 3096
"MAT(106): wrong parameter, missing a vector for real values", //6 kifsparse.cxx 3101
"MAT(107): wrong parameter, missing a vector for imaginary values", //7 kifsparse.cxx 3105
"MAT(204): Expecting a vector as parameter", //8 kifsparse.cxx 3138
"MAT(103): No LU decomposition for this matrix", //9 kifsparse.cxx 3149
"MAT(205): You should have called 'LUdecomp' with second parameter as 'true'", //10 kifsparse.cxx 3163
"MAT(104): Singular matrix, no solution", //11 kifsparse.cxx 3249
"MAT(102): No determinant for this matrix", //12 kifsparse.cxx 3273
"MAT(209): Matrix size mismatch", //13 kifsparse.cxx 3484
"MAT(203): Cannot divide by 0", //14 kifsparse.cxx 3601
"MAT(202): Cannot divide with this matrix", //15 kifsparse.cxx 3618
"Incompatible version of KiF", //16 kifsparse.cxx 4463
"matrix", //17 kifsparse.cxx 4472
"mvector", //18 kifsparse.cxx 4478
"boundaries", //19 kifsparse.cxx 4495
"boundaries(): return the matrix boundaries", //20 kifsparse.cxx 4495
"dimension", //21 kifsparse.cxx 4496
"dimension(): return the matrix size.\ndimension(int rowsize,int columnsize): set the matrix size.", //22 kifsparse.cxx 4496
"occupation", //23 kifsparse.cxx 4497
"occupation(bool row): return the matrix memory occupation as a vector", //24 kifsparse.cxx 4497
"row", //25 kifsparse.cxx 4498
"row(x): return the value on row x as a map or as a mvector", //26 kifsparse.cxx 4498
"column", //27 kifsparse.cxx 4499
"column(x): return the value on column x as a map or as a mvector", //28 kifsparse.cxx 4499
"rowkey", //29 kifsparse.cxx 4500
"rowkey(int r): return the key matching r or the vector of all row keys", //30 kifsparse.cxx 4500
"colkey", //31 kifsparse.cxx 4501
"colkey(int c): return the key matching c or the vector of all column keys", //32 kifsparse.cxx 4501
"transposed", //33 kifsparse.cxx 4502
"transposed(): return the transposed matrix", //34 kifsparse.cxx 4502
"determinant", //35 kifsparse.cxx 4503
"determinant(): return the matrix determinant", //36 kifsparse.cxx 4503
"duplicate", //37 kifsparse.cxx 4504
"duplicate(): return a duplication of the matrix", //38 kifsparse.cxx 4504
"LUdecomp", //39 kifsparse.cxx 4505
"LUdecomp(vector permutations,bool tosolve): Replaces the current matrix with the LU decomposition of a rowwise permutation of itself. tosolve should be used if the next instruction is LUsolve", //40 kifsparse.cxx 4505
"LUsolve", //41 kifsparse.cxx 4506
"LUsolve(vector permutations,vector values): solve linear equations based on values content. Return the result in values...", //42 kifsparse.cxx 4506
"invert", //43 kifsparse.cxx 4507
"invert(): Return the inverted matrix.", //44 kifsparse.cxx 4507
"LUbanddecomp", //45 kifsparse.cxx 4508
"LUbanddecomp(bool local,vector res): return a LU decomposition of a band matrix", //46 kifsparse.cxx 4508
"LUbandsolver", //47 kifsparse.cxx 4509
"LUbandsolver(bool local,vector res): return a LU factorization of a band matrix", //48 kifsparse.cxx 4509
"min", //49 kifsparse.cxx 4510
"min(): return the min value of the matrix", //50 kifsparse.cxx 4510
"max", //51 kifsparse.cxx 4511
"max(): return the max value of the matrix", //52 kifsparse.cxx 4511
"eigen", //53 kifsparse.cxx 4512
"sum", //54 kifsparse.cxx 4520
"sum(): return the sum of all elements", //55 kifsparse.cxx 4520
"product", //56 kifsparse.cxx 4521
"product(): return the product of all elements", //57 kifsparse.cxx 4521
"index", //58 kifsparse.cxx 4522
"index(): return the column or the row current index", //59 kifsparse.cxx 4522
"min(): return the min value of the sparse vector", //60 kifsparse.cxx 4523
"max(): return the max value of the sparse vector", //61 kifsparse.cxx 4524
""};


Exported const char* KIFSOCKETSTRINGS[]={
"No error", //0 kifsocket.cxx 98
"size", //1 kifsocket.cxx 249
"SOCKET(856): Socket error", //2 kifsocket.cxx 416
"SOCKET(851): Cannot find host by name on this machine", //3 kifsocket.cxx 426
"SOCKET(831): Server already launched on this socket", //4 kifsocket.cxx 452
"SOCKET(850): Wrong server name", //5 kifsocket.cxx 465
"SOCKET(872): Error on reuse addess", //6 kifsocket.cxx 481
"SOCKET(853): Error on bind ", //7 kifsocket.cxx 489
"SOCKET(854): Error on listen ", //8 kifsocket.cxx 495
"SOCKET(857): Error on connection ", //9 kifsocket.cxx 541
"SOCKET(855): Error on read ", //10 kifsocket.cxx 555
"SOCKET(824): Non connected socket", //11 kifsocket.cxx 609
"SOCKET(888): Reading connection refused ", //12 kifsocket.cxx 635
"SOCKET(830): Time out ", //13 kifsocket.cxx 642
"SOCKET(889): Writing connection refused ", //14 kifsocket.cxx 662
"SOCKET(820): Missing parameter in read", //15 kifsocket.cxx 687
"SOCKET(858): No client connected", //16 kifsocket.cxx 699
"SOCKET(861): Error on read", //17 kifsocket.cxx 710
"Reading:", //18 kifsocket.cxx 720
"SOCKET(825): RUN can only be launched from a server", //19 kifsocket.cxx 749
"SOCKET(870): Object '", //20 kifsocket.cxx 815
"' unknown ", //21 kifsocket.cxx 817
"SOCKET(871): Function '", //22 kifsocket.cxx 873
"SOCKET(822): Missing parameter in write", //23 kifsocket.cxx 1010
"Writing:", //24 kifsocket.cxx 1033
"SOCKET(859): Error sending", //25 kifsocket.cxx 1039
"SOCKET(821): Missing parameter in receive", //26 kifsocket.cxx 1088
"SOCKET(860): Error on RECEIVE ", //27 kifsocket.cxx 1107
"SOCKET(823): Too many parameter in receive", //28 kifsocket.cxx 1144
"SOCKET(860): Error on RECEIVE", //29 kifsocket.cxx 1166
"SOCKET(827): Missing parameter in send", //30 kifsocket.cxx 1207
"port", //31 kifsocket.cxx 1264
"address", //32 kifsocket.cxx 1266
"SOCKET(852): You cannot use GETPEERNAME on the client side", //33 kifsocket.cxx 1271
"name", //34 kifsocket.cxx 1331
"SOCKET(812): Unknown method", //35 kifsocket.cxx 1417
"Incompatible version of KiF", //36 kifsocket.cxx 1513
"socket", //37 kifsocket.cxx 1523
"remote", //38 kifsocket.cxx 1528
"createserver", //39 kifsocket.cxx 1547
"createserver(string hostname,int port,int nblients: create a server, if hostname is omitted, then the local hostname is used", //40 kifsocket.cxx 1547
"connect", //41 kifsocket.cxx 1548
"connect(string hostname,int port): connect to the server", //42 kifsocket.cxx 1548
"wait", //43 kifsocket.cxx 1549
"wait(): wait for a client to connect and returns its socket id", //44 kifsocket.cxx 1549
"read", //45 kifsocket.cxx 1550
"read(int num): read a string on a socket. On the server side, 'num' is the value returned by 'wait()'. Use 'read()' on the client side", //46 kifsocket.cxx 1550
"run", //47 kifsocket.cxx 1551
"run(int num,string stop): Only on the server side: execute remote method invocations for the client 'num'. 'stop' is the string that can be used to stop the server by the client.", //48 kifsocket.cxx 1551
"write", //49 kifsocket.cxx 1552
"write(int num,string s): write the string s on the socket. On the server side, num is the value returned by wait()'. Use 'write(string s)' on the client side", //50 kifsocket.cxx 1552
"receive", //51 kifsocket.cxx 1553
"receive(int num,int nb): read a string on a socket in a non KiF environment. On the server side, 'num' is the value returned by 'wait()'. Use 'receive()' on the client side", //52 kifsocket.cxx 1553
"get", //53 kifsocket.cxx 1554
"get(int num): get one character from a socket in a non KiF environment. On the server side, 'num' is the value returned by 'wait()'. Use 'get()' on the client side", //54 kifsocket.cxx 1554
"send", //55 kifsocket.cxx 1555
"send(int num,string s): write the string s on the socket in a non KiF environment. On the server side, num is the value returned by wait()'. Use 'send(string s)' on the client side", //56 kifsocket.cxx 1555
"close", //57 kifsocket.cxx 1556
"close(int num): Close a socket. On the server side, if 'num' is provided (it is the value returned by wait()) it closes the client socket, otherwise it closes the current socket.", //58 kifsocket.cxx 1556
"blocking", //59 kifsocket.cxx 1557
"blocking(bool flag): if 'flag' is true, the socket works in 'blocking' mode, otherwise in 'non blocking' mode", //60 kifsocket.cxx 1557
"settimeout", //61 kifsocket.cxx 1558
"settimeout(int t): Set a time out of 't' seconds on the socket", //62 kifsocket.cxx 1558
"gethostname", //63 kifsocket.cxx 1559
"gethostname(): return the current host name", //64 kifsocket.cxx 1559
"port(): return the current port number", //65 kifsocket.cxx 1560
"getpeername", //66 kifsocket.cxx 1561
"getpeername(int num): return the current peer name on the socket 'num'", //67 kifsocket.cxx 1561
"getframe", //68 kifsocket.cxx 1562
"getframe(string frame): return a frame object handle declared on the server side", //69 kifsocket.cxx 1562
"getfunction", //70 kifsocket.cxx 1563
"getfunction(string frame): return a function handle declared on the server side", //71 kifsocket.cxx 1563
"myvariable", //72 kifsocket.cxx 1569
""};


Exported const char* KIFLTKSTRINGS[]={
"WND(303): No window available", //0 kifltk.cxx 139
"KIF(93): Wrong number of parameters in GET", //1 kifltk.cxx 231
"get", //2 kifltk.cxx 248
"FOREGROUND_COLOR", //3 kifltk.cxx 283
"BACKGROUND2_COLOR", //4 kifltk.cxx 284
"BACKGROUND_COLOR", //5 kifltk.cxx 285
"INACTIVE_COLOR", //6 kifltk.cxx 286
"SELECTION_COLOR", //7 kifltk.cxx 287
"GRAY0", //8 kifltk.cxx 288
"DARK3", //9 kifltk.cxx 289
"DARK2", //10 kifltk.cxx 290
"DARK1", //11 kifltk.cxx 291
"LIGHT1", //12 kifltk.cxx 292
"LIGHT2", //13 kifltk.cxx 293
"LIGHT3", //14 kifltk.cxx 294
"BLACK", //15 kifltk.cxx 295
"RED", //16 kifltk.cxx 296
"GREEN", //17 kifltk.cxx 297
"YELLOW", //18 kifltk.cxx 298
"BLUE", //19 kifltk.cxx 299
"MAGENTA", //20 kifltk.cxx 300
"CYAN", //21 kifltk.cxx 301
"DARK_RED", //22 kifltk.cxx 302
"DARK_GREEN", //23 kifltk.cxx 303
"DARK_YELLOW", //24 kifltk.cxx 304
"DARK_BLUE", //25 kifltk.cxx 305
"DARK_MAGENTA", //26 kifltk.cxx 306
"DARK_CYAN", //27 kifltk.cxx 307
"WHITE", //28 kifltk.cxx 308
"Regular", //29 kifltk.cxx 310
"Check", //30 kifltk.cxx 311
"Light", //31 kifltk.cxx 312
"Repeat", //32 kifltk.cxx 313
"Return", //33 kifltk.cxx 314
"Round", //34 kifltk.cxx 315
"Image", //35 kifltk.cxx 316
"PUSH", //36 kifltk.cxx 319
"RELEASE", //37 kifltk.cxx 320
"DRAG", //38 kifltk.cxx 321
"MOVE", //39 kifltk.cxx 322
"MOUSEWHEEL", //40 kifltk.cxx 323
"KEYDOWN", //41 kifltk.cxx 324
"KEYUP", //42 kifltk.cxx 325
"VERT_SLIDER", //43 kifltk.cxx 327
"HOR_SLIDER", //44 kifltk.cxx 328
"VERT_FILL_SLIDER", //45 kifltk.cxx 329
"HOR_FILL_SLIDER", //46 kifltk.cxx 330
"VERT_NICE_SLIDER", //47 kifltk.cxx 331
"HOR_NICE_SLIDER", //48 kifltk.cxx 332
"SOLID", //49 kifltk.cxx 334
"DASH", //50 kifltk.cxx 335
"DOT", //51 kifltk.cxx 336
"DASHDOT", //52 kifltk.cxx 337
"DASHDOTDOT", //53 kifltk.cxx 338
"CAP_FLAT", //54 kifltk.cxx 339
"CAP_ROUND", //55 kifltk.cxx 340
"CAP_SQUARE", //56 kifltk.cxx 341
"JOIN_MITER", //57 kifltk.cxx 342
"JOIN_ROUND", //58 kifltk.cxx 343
"JOIN_BEVEL", //59 kifltk.cxx 344
"ALIGN_CENTER", //60 kifltk.cxx 346
"ALIGN_TOP", //61 kifltk.cxx 347
"ALIGN_BOTTOM", //62 kifltk.cxx 348
"ALIGN_LEFT", //63 kifltk.cxx 349
"ALIGN_RIGHT", //64 kifltk.cxx 350
"ALIGN_INSIDE", //65 kifltk.cxx 351
"ALIGN_TEXT_OVER_IMAGE", //66 kifltk.cxx 352
"ALIGN_IMAGE_OVER_TEXT", //67 kifltk.cxx 353
"ALIGN_CLIP", //68 kifltk.cxx 354
"ALIGN_WRAP", //69 kifltk.cxx 355
"ALIGN_IMAGE_NEXT_TO_TEXT", //70 kifltk.cxx 356
"ALIGN_TEXT_NEXT_TO_IMAGE", //71 kifltk.cxx 357
"ALIGN_IMAGE_BACKDROP", //72 kifltk.cxx 358
"ALIGN_TOP_LEFT", //73 kifltk.cxx 359
"ALIGN_TOP_RIGHT", //74 kifltk.cxx 360
"ALIGN_BOTTOM_LEFT", //75 kifltk.cxx 361
"ALIGN_BOTTOM_RIGHT", //76 kifltk.cxx 362
"ALIGN_LEFT_TOP", //77 kifltk.cxx 363
"ALIGN_RIGHT_TOP", //78 kifltk.cxx 364
"ALIGN_LEFT_BOTTOM", //79 kifltk.cxx 365
"ALIGN_RIGHT_BOTTOM", //80 kifltk.cxx 366
"ALIGN_NOWRAP", //81 kifltk.cxx 367
"ALIGN_POSITION_MASK", //82 kifltk.cxx 368
"ALIGN_IMAGE_MASK", //83 kifltk.cxx 369
"NORMAL_LABEL", //84 kifltk.cxx 371
"NO_LABEL", //85 kifltk.cxx 372
"SHADOW_LABEL", //86 kifltk.cxx 373
"ENGRAVED_LABEL", //87 kifltk.cxx 374
"EMBOSSED_LABEL", //88 kifltk.cxx 375
"CURSOR_DEFAULT", //89 kifltk.cxx 377
"CURSOR_ARROW", //90 kifltk.cxx 378
"CURSOR_CROSS", //91 kifltk.cxx 379
"CURSOR_WAIT", //92 kifltk.cxx 380
"CURSOR_INSERT", //93 kifltk.cxx 381
"CURSOR_HAND", //94 kifltk.cxx 382
"CURSOR_HELP", //95 kifltk.cxx 383
"CURSOR_MOVE", //96 kifltk.cxx 384
"CURSOR_NS", //97 kifltk.cxx 385
"CURSOR_WE", //98 kifltk.cxx 386
"CURSOR_NWSE", //99 kifltk.cxx 387
"CURSOR_NESW", //100 kifltk.cxx 388
"CURSOR_NONE", //101 kifltk.cxx 389
"CURSOR_N", //102 kifltk.cxx 390
"CURSOR_NE", //103 kifltk.cxx 391
"CURSOR_E", //104 kifltk.cxx 392
"CURSOR_SE", //105 kifltk.cxx 393
"CURSOR_S", //106 kifltk.cxx 394
"CURSOR_SW", //107 kifltk.cxx 395
"CURSOR_W", //108 kifltk.cxx 396
"CURSOR_NW", //109 kifltk.cxx 397
"NORMAL_CURSOR", //110 kifltk.cxx 399
"CARET_CURSOR", //111 kifltk.cxx 400
"DIM_CURSOR", //112 kifltk.cxx 401
"BLOCK_CURSOR", //113 kifltk.cxx 402
"HEAVY_CURSOR", //114 kifltk.cxx 403
"LINE_CURSOR", //115 kifltk.cxx 404
"SHIFT", //116 kifltk.cxx 406
"CAPS_LOCK", //117 kifltk.cxx 407
"NUM_LOCK", //118 kifltk.cxx 410
"SCROLL_LOCK", //119 kifltk.cxx 412
"BUTTON1", //120 kifltk.cxx 413
"BUTTON2", //121 kifltk.cxx 414
"BUTTON3", //122 kifltk.cxx 415
"BUTTONS", //123 kifltk.cxx 416
"KEY_MASK", //124 kifltk.cxx 417
"COMMAND", //125 kifltk.cxx 418
"CONTROL", //126 kifltk.cxx 419
"Button", //127 kifltk.cxx 421
"BackSpace", //128 kifltk.cxx 422
"Enter", //129 kifltk.cxx 424
"Pause", //130 kifltk.cxx 425
"Scroll_Lock", //131 kifltk.cxx 426
"Escape", //132 kifltk.cxx 427
"Home", //133 kifltk.cxx 428
"Left", //134 kifltk.cxx 429
"Up", //135 kifltk.cxx 430
"Right", //136 kifltk.cxx 431
"Down", //137 kifltk.cxx 432
"Page_Up", //138 kifltk.cxx 433
"Page_Down", //139 kifltk.cxx 434
"End", //140 kifltk.cxx 435
"Print", //141 kifltk.cxx 436
"Insert", //142 kifltk.cxx 437
"Menu", //143 kifltk.cxx 438
"Help", //144 kifltk.cxx 439
"Num_Lock", //145 kifltk.cxx 440
"KP_Enter", //146 kifltk.cxx 442
"KP_Last", //147 kifltk.cxx 443
"F_Last", //148 kifltk.cxx 445
"Shift_L", //149 kifltk.cxx 446
"Shift_R", //150 kifltk.cxx 447
"Control_L", //151 kifltk.cxx 448
"Control_R", //152 kifltk.cxx 449
"Caps_Lock", //153 kifltk.cxx 450
"Meta_L", //154 kifltk.cxx 451
"Meta_R", //155 kifltk.cxx 452
"Alt_L", //156 kifltk.cxx 453
"Alt_R", //157 kifltk.cxx 454
"Delete", //158 kifltk.cxx 455
"NO_BOX", //159 kifltk.cxx 457
"FLAT_BOX", //160 kifltk.cxx 458
"UP_BOX", //161 kifltk.cxx 459
"DOWN_BOX", //162 kifltk.cxx 460
"UP_FRAME", //163 kifltk.cxx 461
"DOWN_FRAME", //164 kifltk.cxx 462
"THIN_UP_BOX", //165 kifltk.cxx 463
"THIN_DOWN_BOX", //166 kifltk.cxx 464
"THIN_UP_FRAME", //167 kifltk.cxx 465
"THIN_DOWN_FRAME", //168 kifltk.cxx 466
"ENGRAVED_BOX", //169 kifltk.cxx 467
"EMBOSSED_BOX", //170 kifltk.cxx 468
"ENGRAVED_FRAME", //171 kifltk.cxx 469
"EMBOSSED_FRAME", //172 kifltk.cxx 470
"BORDER_BOX", //173 kifltk.cxx 471
"SHADOW_BOX", //174 kifltk.cxx 472
"BORDER_FRAME", //175 kifltk.cxx 473
"SHADOW_FRAME", //176 kifltk.cxx 474
"ROUNDED_BOX", //177 kifltk.cxx 475
"RSHADOW_BOX", //178 kifltk.cxx 476
"ROUNDED_FRAME", //179 kifltk.cxx 477
"RFLAT_BOX", //180 kifltk.cxx 478
"ROUND_UP_BOX", //181 kifltk.cxx 479
"ROUND_DOWN_BOX", //182 kifltk.cxx 480
"DIAMOND_UP_BOX", //183 kifltk.cxx 481
"DIAMOND_DOWN_BOX", //184 kifltk.cxx 482
"OVAL_BOX", //185 kifltk.cxx 483
"OSHADOW_BOX", //186 kifltk.cxx 484
"OVAL_FRAME", //187 kifltk.cxx 485
"OFLAT_BOX", //188 kifltk.cxx 486
"PLASTIC_UP_BOX", //189 kifltk.cxx 487
"PLASTIC_DOWN_BOX", //190 kifltk.cxx 488
"PLASTIC_UP_FRAME", //191 kifltk.cxx 489
"PLASTIC_DOWN_FRAME", //192 kifltk.cxx 490
"PLASTIC_THIN_UP_BOX", //193 kifltk.cxx 491
"PLASTIC_THIN_DOWN_BOX", //194 kifltk.cxx 492
"PLASTIC_ROUND_UP_BOX", //195 kifltk.cxx 493
"PLASTIC_ROUND_DOWN_BOX", //196 kifltk.cxx 494
"GTK_UP_BOX", //197 kifltk.cxx 495
"GTK_DOWN_BOX", //198 kifltk.cxx 496
"GTK_UP_FRAME", //199 kifltk.cxx 497
"GTK_DOWN_FRAME", //200 kifltk.cxx 498
"GTK_THIN_UP_BOX", //201 kifltk.cxx 499
"GTK_THIN_DOWN_BOX", //202 kifltk.cxx 500
"GTK_THIN_UP_FRAME", //203 kifltk.cxx 501
"GTK_THIN_DOWN_FRAME", //204 kifltk.cxx 502
"GTK_ROUND_UP_BOX", //205 kifltk.cxx 503
"GTK_ROUND_DOWN_BOX", //206 kifltk.cxx 504
"FREE_BOXTYPE", //207 kifltk.cxx 505
"HELVETICA_BOLD", //208 kifltk.cxx 508
"HELVETICA_ITALIC", //209 kifltk.cxx 509
"HELVETICA_BOLD_ITALIC", //210 kifltk.cxx 510
"COURIER_BOLD", //211 kifltk.cxx 512
"COURIER_ITALIC", //212 kifltk.cxx 513
"COURIER_BOLD_ITALIC", //213 kifltk.cxx 514
"TIMES_BOLD", //214 kifltk.cxx 516
"TIMES_ITALIC", //215 kifltk.cxx 517
"TIMES_BOLD_ITALIC", //216 kifltk.cxx 518
"SYMBOL", //217 kifltk.cxx 519
"SCREEN", //218 kifltk.cxx 520
"SCREEN_BOLD", //219 kifltk.cxx 521
"BOLD", //220 kifltk.cxx 524
"ITALIC", //221 kifltk.cxx 525
"BOLD_ITALIC", //222 kifltk.cxx 526
"NORMAL_BUTTON", //223 kifltk.cxx 546
"TOGGLE_BUTTON", //224 kifltk.cxx 547
"RADIO_BUTTON", //225 kifltk.cxx 548
"HIDDEN_BUTTON", //226 kifltk.cxx 549
"WHEN_NEVER", //227 kifltk.cxx 551
"WHEN_CHANGED", //228 kifltk.cxx 552
"WHEN_RELEASE", //229 kifltk.cxx 553
"WHEN_RELEASE_ALWAYS", //230 kifltk.cxx 554
"WHEN_ENTER_KEY", //231 kifltk.cxx 555
"WHEN_ENTER_KEY_ALWAYS", //232 kifltk.cxx 556
"FL_NORMAL_SIZE", //233 kifltk.cxx 573
"FL_DIR_SINGLE", //234 kifltk.cxx 674
"FL_DIR_MULTI", //235 kifltk.cxx 678
"FL_DIR_CREATE", //236 kifltk.cxx 682
"FL_DIR_DIRECTORY", //237 kifltk.cxx 686
"FL_TREE_SELECT_NONE", //238 kifltk.cxx 691
"FL_TREE_SELECT_SINGLE", //239 kifltk.cxx 695
"FL_TREE_SELECT_MULTI", //240 kifltk.cxx 699
"FL_TREE_SORT_NONE", //241 kifltk.cxx 703
"FL_TREE_SORT_ASCENDING", //242 kifltk.cxx 707
"FL_TREE_SORT_DESCENDING", //243 kifltk.cxx 711
"FL_TREE_CONNECTOR_NONE", //244 kifltk.cxx 715
"FL_TREE_CONNECTOR_DOTTED", //245 kifltk.cxx 719
"FL_TREE_CONNECTOR_SOLID", //246 kifltk.cxx 723
"FL_TREE_REASON_NONE", //247 kifltk.cxx 727
"FL_TREE_REASON_SELECTED", //248 kifltk.cxx 731
"FL_TREE_REASON_DESELECTED", //249 kifltk.cxx 735
"FL_TREE_REASON_OPENED", //250 kifltk.cxx 739
"FL_TREE_REASON_CLOSED", //251 kifltk.cxx 743
"WND(098): Widget cannot be instantiated", //252 kifltk.cxx 860
"WND(678): Widget not initialized", //253 kifltk.cxx 894
"WND(805): Widget not initialized", //254 kifltk.cxx 917
"WND(202): Unknown alignement", //255 kifltk.cxx 924
"WND(657): Unknown color code", //256 kifltk.cxx 953
"WND(904): Expecting a vector", //257 kifltk.cxx 1022
"WND(905): Missing coordinates", //258 kifltk.cxx 1025
"WND(905): Incoherent coordinates", //259 kifltk.cxx 1038
"WND(678): wdg not initialized", //260 kifltk.cxx 1150
"WND(102): wdg not created", //261 kifltk.cxx 1169
"WND(765): Unknown font:", //262 kifltk.cxx 1181
"WND(658): unknown font", //263 kifltk.cxx 1192
"with", //264 kifltk.cxx 1364
"name", //265 kifltk.cxx 1563
"WND(805): Image already loaded", //266 kifltk.cxx 1587
"WND(805): Image not loaded", //267 kifltk.cxx 1593
"size", //268 kifltk.cxx 1618
"WND(809): Wrong type for the first parameter", //269 kifltk.cxx 1700
"WND(811): Wrong size for the bitmap. sz=(width*height)/8", //270 kifltk.cxx 1714
"WND(808): Bitmap already loaded", //271 kifltk.cxx 1783
"WND(607): Unknown line shape code", //272 kifltk.cxx 2005
"WND(245): Wrong number of elements in the color vector ", //273 kifltk.cxx 2031
"W", //274 kifltk.cxx 2260
"WND(657): Unknown font code", //275 kifltk.cxx 2299
"WND(608): Wrong number of parameters=expected 5 in a complex shape construction", //276 kifltk.cxx 2327
"WND(609): Wrong number of parameters=expected 6 in a regular construction", //277 kifltk.cxx 2332
"WND(612): Cannot call this method here. Have you called beginpoints before?", //278 kifltk.cxx 2491
"WND(612): Cannot call this method here. Have you called beginloop before?", //279 kifltk.cxx 2537
"WND(612): Cannot call this method here. Have you called beginpolygon before?", //280 kifltk.cxx 2562
"WND(612): Cannot call this method here. Have you called begincomplexpolygon before?", //281 kifltk.cxx 2587
"WND(612): Cannot call this method here. Have you called beginline before?", //282 kifltk.cxx 2625
"WND(610): You cannot use this function outside a complex shape construction", //283 kifltk.cxx 2639
"WND(100): Cannot recreate an existing window", //284 kifltk.cxx 2863
"WND(431): Unavailable font id", //285 kifltk.cxx 2892
"WND(109): Wrong element, the first parameter should be a function", //286 kifltk.cxx 2935
"WND(102): Window already instantiated", //287 kifltk.cxx 3143
"WND(341): Unknown mouse action", //288 kifltk.cxx 3159
"WND(342): Wrong parameter in your mouse callback", //289 kifltk.cxx 3165
"WND(344): Unknown keyboard action", //290 kifltk.cxx 3192
"WND(343): Wrong parameter in your keyboard callback", //291 kifltk.cxx 3199
"WND(872): We expect a vector as first parameter.", //292 kifltk.cxx 3255
"WND(871): The vector should contain an even number of elements.", //293 kifltk.cxx 3258
"WND(873): We expect a vector as third parameter", //294 kifltk.cxx 3279
"WND(862): Wrong values to compute coordinates", //295 kifltk.cxx 3389
"WND(167): Unknown image", //296 kifltk.cxx 3467
"WND(168): Unloaded image", //297 kifltk.cxx 3475
"WND(169): Unknown bitmap", //298 kifltk.cxx 3495
"WND(109): Wrong parameter, vector expected", //299 kifltk.cxx 3564
"WND(109): Wrong element, vector expected", //300 kifltk.cxx 3573
"WND(109): Wrong element, missing field in a sub-menu", //301 kifltk.cxx 3600
"WND(109): Wrong element, second field in a sub-menu should be a vector", //302 kifltk.cxx 3605
"WND(109): Wrong element, third field in a sub-menu should be a function", //303 kifltk.cxx 3622
"WND(123): Operation not available", //304 kifltk.cxx 3788
"WND(134): Widget has not been created yet", //305 kifltk.cxx 3800
"WND(133): Widget expected as parameter", //306 kifltk.cxx 3805
"button", //307 kifltk.cxx 4068
"wheelx", //308 kifltk.cxx 4069
"wheely", //309 kifltk.cxx 4070
"xroot", //310 kifltk.cxx 4073
"yroot", //311 kifltk.cxx 4074
"WND(101): input already created", //312 kifltk.cxx 4313
"WND(677): Input not initialized", //313 kifltk.cxx 4339
"WND(101): editor already created", //314 kifltk.cxx 4932
"KIF(881): Error, wrong function arguments", //315 kifltk.cxx 4946
"WND(101): No editor available", //316 kifltk.cxx 4959
"WND(342): Wrong parameter in your key callback", //317 kifltk.cxx 5088
"EDT(768): Wrong number of elements in the vector... 3 or 4 expected", //318 kifltk.cxx 5097
"WND(627): Maximum number of styles reached", //319 kifltk.cxx 5166
"WND(676): Editor not initialized", //320 kifltk.cxx 5211
"EDT(766): Wrong input: map expected", //321 kifltk.cxx 5218
"EDT(770): Wrong input: empty map", //322 kifltk.cxx 5227
"EDT(767): Missing '#' key in the map. '#' is the initial key and is mandatory", //323 kifltk.cxx 5246
"EDT(767): Wrong input: vector expected", //324 kifltk.cxx 5249
"WND(689): Out of range", //325 kifltk.cxx 5476
"WND(679): Style table not initialized", //326 kifltk.cxx 5509
"WND(679): Unknown style key", //327 kifltk.cxx 5520
"start", //328 kifltk.cxx 5789
"end", //329 kifltk.cxx 5790
"cursor", //330 kifltk.cxx 6343
"WND(101): output already created", //331 kifltk.cxx 6539
"WND(679): Output not initialized", //332 kifltk.cxx 6561
"WND(101): button already created", //333 kifltk.cxx 6876
"WND(675): Unknwown button shape: ", //334 kifltk.cxx 6884
"WND(675): Unknwown button type: ", //335 kifltk.cxx 6904
"WND(805): Button not initialized", //336 kifltk.cxx 6953
"WND(809): Image button required", //337 kifltk.cxx 6955
"WND(813): image object required", //338 kifltk.cxx 6959
"WND(654): Unknown action", //339 kifltk.cxx 7034
"WND(656): Unknown key code", //340 kifltk.cxx 7060
"WND(101): box already created", //341 kifltk.cxx 7246
"WND(102): box not created", //342 kifltk.cxx 7263
"WND(767): Unknown type:", //343 kifltk.cxx 7268
"WND(101): group already created", //344 kifltk.cxx 7452
"WND(101): group not created", //345 kifltk.cxx 7474
"WND(507): This group does not belong to a tab structure", //346 kifltk.cxx 7478
"WND(101): tabs already created", //347 kifltk.cxx 7674
"WND(101): tabs not created", //348 kifltk.cxx 7692
"WND(800): No active tab", //349 kifltk.cxx 7706
"WND(506): Empty group", //350 kifltk.cxx 7716
"WND(507): This group does not belong to that tab", //351 kifltk.cxx 7718
"WND(105): You can only add wgroup object", //352 kifltk.cxx 7741
"WND(800): Wrong number of parameters", //353 kifltk.cxx 7747
"WND(505): You can only remove wgroup object", //354 kifltk.cxx 7765
"WND(101): table already created", //355 kifltk.cxx 8081
"WND(102): table has not been created yet", //356 kifltk.cxx 8114
"WND(678): table not initialized", //357 kifltk.cxx 8324
"WND(102): table not created", //358 kifltk.cxx 8401
"WND(805): table not created", //359 kifltk.cxx 8424
"top", //360 kifltk.cxx 8552
"left", //361 kifltk.cxx 8553
"bottom", //362 kifltk.cxx 8554
"right", //363 kifltk.cxx 8555
"values", //364 kifltk.cxx 8557
"WND(100): Cannot recreate an existing scroll", //365 kifltk.cxx 8674
"WND(101): No scroll available", //366 kifltk.cxx 8698
"WND(100): Cannot recreate an existing slider", //367 kifltk.cxx 8859
"WND(101): No slider available", //368 kifltk.cxx 8893
"WND(206): Wrong slider type", //369 kifltk.cxx 8947
"WND(100): Cannot recreate an existing choice", //370 kifltk.cxx 9176
"WND(678): choice not initialized", //371 kifltk.cxx 9198
"WND(100): Cannot recreate an existing browser", //372 kifltk.cxx 9498
"WND(121): Browser widget has not been created yet", //373 kifltk.cxx 9521
"WND(094): You can only instanciate a wtreeitem with another wtreeitem", //374 kifltk.cxx 9736
"WND(117): Unknown tree item", //375 kifltk.cxx 9778
"WND(118): Wrong child index", //376 kifltk.cxx 9994
"WND(210): Cannot recreate an existing wcounter", //377 kifltk.cxx 10177
"WND(211): No wcounter available", //378 kifltk.cxx 10200
"WND(220): Cannot recreate an existing wprogress bar", //379 kifltk.cxx 10509
"WND(221): No wprogress bar available", //380 kifltk.cxx 10531
"WND(100): Cannot recreate an existing wtree", //381 kifltk.cxx 10756
"WND(121): wtree widget has not been created yet", //382 kifltk.cxx 10779
"WND(125): Empty label, cannot create wtree widget", //383 kifltk.cxx 10802
"WND(341): wrong type for the first element", //384 kifltk.cxx 10865
"WND(100): Cannot recreate an existing filebrowser", //385 kifltk.cxx 11418
"WND(101): File browser not initialized", //386 kifltk.cxx 11457
"Incompatible version of KiF", //387 kifltk.cxx 11984
"window", //388 kifltk.cxx 11993
"box", //389 kifltk.cxx 11998
"wtabs", //390 kifltk.cxx 12003
"wgroup", //391 kifltk.cxx 12008
"wchoice", //392 kifltk.cxx 12013
"editor", //393 kifltk.cxx 12023
"winput", //394 kifltk.cxx 12028
"wtable", //395 kifltk.cxx 12033
"woutput", //396 kifltk.cxx 12038
"scroll", //397 kifltk.cxx 12043
"slider", //398 kifltk.cxx 12048
"wcounter", //399 kifltk.cxx 12053
"wprogress", //400 kifltk.cxx 12058
"image", //401 kifltk.cxx 12063
"bitmap", //402 kifltk.cxx 12068
"filebrowser", //403 kifltk.cxx 12073
"browser", //404 kifltk.cxx 12078
"wtreeitem", //405 kifltk.cxx 12083
"wtree", //406 kifltk.cxx 12088
"create", //407 kifltk.cxx 12106
"create(int x,int y,int w, int h,string title): Create a window without widgets, w and h are optional", //408 kifltk.cxx 12106
"begin", //409 kifltk.cxx 12107
"begin(int x,int y,int w, int h,string title): Create a window and begin initialisation, w and h are optional", //410 kifltk.cxx 12107
"end(): end creation", //411 kifltk.cxx 12108
"run", //412 kifltk.cxx 12109
"run(): Launch the GUI", //413 kifltk.cxx 12109
"close", //414 kifltk.cxx 12110
"close(): close the window", //415 kifltk.cxx 12110
"onclose", //416 kifltk.cxx 12111
"onclose(function,object): Callback to call on close", //417 kifltk.cxx 12111
"ontime", //418 kifltk.cxx 12112
"ontime(function,t,object): Set a time out function", //419 kifltk.cxx 12112
"initializefonts", //420 kifltk.cxx 12114
"initializefonts(): load fonts from system. Return the number of available fonts", //421 kifltk.cxx 12114
"getfont", //422 kifltk.cxx 12115
"getfont(int num): get font name.", //423 kifltk.cxx 12115
"getfontsizes", //424 kifltk.cxx 12116
"getfontsizes(int num): return a vector of available font sizes.", //425 kifltk.cxx 12116
"fontnumber", //426 kifltk.cxx 12117
"fontnumber(): Return the number of available fonts.", //427 kifltk.cxx 12117
"plot", //428 kifltk.cxx 12118
"plot(fvector xy,int thickness,fvector landmarks): Plot a graph from a table of successive x,y points according to window size. If thickness===0 then points are continuously plotted, else defines the diameter of the point. Return a float vector which is used with plotcoords. The landmark vector is optional, it is has the following structure: [XmaxWindow,YmaxWindow,XminValue,YminValue,XmaxValue,YmaxValue,incX,incY]. incX,incY are also optional.", //429 kifltk.cxx 12118
"plotcoords", //430 kifltk.cxx 12119
"plotcoords(fvector inits,float x,float y): Compute the coordinates of a point(x,y) according to the previous scale computed with plot. Returns a vector of two elements [xs,ys] corresponding to the screen coordinates in the current window.", //431 kifltk.cxx 12119
"ask", //432 kifltk.cxx 12121
"ask(string msg,string buttonmsg1,string buttonmsg2,string buttonmsg3): Pop up window to pose a question", //433 kifltk.cxx 12121
"alert", //434 kifltk.cxx 12122
"alert(string msg): Pop up window to display an alert", //435 kifltk.cxx 12122
"password", //436 kifltk.cxx 12123
"password(string msg): Pop up window to input a password", //437 kifltk.cxx 12123
"get(string msg): Pop up window to get a value from the user", //438 kifltk.cxx 12124
"menu", //439 kifltk.cxx 12125
"menu(vector,int x,int y,int w, int h): initialize a menu with its callback functions", //440 kifltk.cxx 12125
"resizable", //441 kifltk.cxx 12126
"resizable(object): make the object resizable", //442 kifltk.cxx 12126
"sizerange", //443 kifltk.cxx 12127
"sizerange(int minw,int minh, int maxw,int maxh): define range in which the size of the window can evolve", //444 kifltk.cxx 12127
"modal", //445 kifltk.cxx 12128
"modal(bool b): If true make the window modal. If no parameter return if the window is modal", //446 kifltk.cxx 12128
"border", //447 kifltk.cxx 12129
"border(bool b): If true add or remove borders. If no parameter return if the window has borders", //448 kifltk.cxx 12129
"backgroundcolor", //449 kifltk.cxx 12130
"backgroundcolor(int c|string c): set the background color", //450 kifltk.cxx 12130
"drawcolor", //451 kifltk.cxx 12132
"drawcolor(int c|string c): set the color for the next drawings", //452 kifltk.cxx 12132
"rectangle", //453 kifltk.cxx 12133
"rectangle(int x,int y,int w, int h, string c|int c): Draw a rectangle with optional color c", //454 kifltk.cxx 12133
"vertex", //455 kifltk.cxx 12134
"vertex(float x,float y): Add a vertex to a complex structure", //456 kifltk.cxx 12134
"rectanglefill", //457 kifltk.cxx 12135
"rectanglefill(int x,int y,int w, int h, string c|int c): Fill a rectangle with optional color c", //458 kifltk.cxx 12135
"lineshape", //459 kifltk.cxx 12136
"lineshape(string type,int width): Select the line shape and its thikness", //460 kifltk.cxx 12136
"polygon", //461 kifltk.cxx 12137
"polygon(int x,int y,int x1, int y1,int x2, int y2, int x3, int y3): Draw a polygon, x3 and y3 are optional", //462 kifltk.cxx 12137
"loop", //463 kifltk.cxx 12138
"loop(int x,int y,int x1, int y1,int x2, int y2, int x3, int y3): Draw a series of lines, x3 and y3 are optional", //464 kifltk.cxx 12138
"line", //465 kifltk.cxx 12139
"line(int x,int y,int x1, int y1,int x2, int y2): Draw a line between points, x2 and y2 are optional", //466 kifltk.cxx 12139
"pushmatrix", //467 kifltk.cxx 12141
"pushmatrix(): Save the current transformation", //468 kifltk.cxx 12141
"popmatrix", //469 kifltk.cxx 12142
"popmatrix(): Restore the current transformation", //470 kifltk.cxx 12142
"scale", //471 kifltk.cxx 12143
"scale(float x,float y)|scale(float x): Scale the current transformation", //472 kifltk.cxx 12143
"translate", //473 kifltk.cxx 12144
"translate(float x,float y): translate the current transformation", //474 kifltk.cxx 12144
"rotate", //475 kifltk.cxx 12145
"rotate(float d): rotate of degree d the current transformation", //476 kifltk.cxx 12145
"multmatrix", //477 kifltk.cxx 12146
"multmatrix(float a, float b, float c, float d, float x, float y): combine transformations", //478 kifltk.cxx 12146
"transformx", //479 kifltk.cxx 12148
"transformx(float x, float y): Transform a coordinate X using the current transformation matrix.", //480 kifltk.cxx 12148
"transformy", //481 kifltk.cxx 12149
"transformy(float x, float y): Transform a coordinate Y using the current transformation matrix.", //482 kifltk.cxx 12149
"transformdx", //483 kifltk.cxx 12150
"transformdx(float x, float y): Transform a distance DX using the current transformation matrix.", //484 kifltk.cxx 12150
"transformdy", //485 kifltk.cxx 12151
"transformdy(float x, float y): Transform a distance DY using the current transformation matrix.", //486 kifltk.cxx 12151
"transformedvertex", //487 kifltk.cxx 12152
"transformedvertex(float x, float y): add transformations to vertices list.", //488 kifltk.cxx 12152
"beginpoints", //489 kifltk.cxx 12154
"beginpoints(): Start accumulating vertices", //490 kifltk.cxx 12154
"endpoints", //491 kifltk.cxx 12155
"endpoints(): Stop accumulating vertices", //492 kifltk.cxx 12155
"beginline", //493 kifltk.cxx 12157
"beginline(): Start drawing lines.", //494 kifltk.cxx 12157
"endline", //495 kifltk.cxx 12158
"endline(): End drawing lines.", //496 kifltk.cxx 12158
"beginloop", //497 kifltk.cxx 12160
"beginloop(): Start drawing a closed sequence of lines.", //498 kifltk.cxx 12160
"endloop", //499 kifltk.cxx 12161
"endloop(): End drawing a closed sequence of lines.", //500 kifltk.cxx 12161
"beginpolygon", //501 kifltk.cxx 12163
"beginpolygon(): Start drawing a convex filled polygon.", //502 kifltk.cxx 12163
"endpolygon", //503 kifltk.cxx 12164
"endpolygon(): End drawing a convex filled polygon.", //504 kifltk.cxx 12164
"begincomplexpolygon", //505 kifltk.cxx 12166
"begincomplexpolygon(): Start drawing a complex filled polygon.", //506 kifltk.cxx 12166
"gap", //507 kifltk.cxx 12167
"gap(): Fill the gap while drawing a complex polygon.", //508 kifltk.cxx 12167
"endcomplexpolygon", //509 kifltk.cxx 12168
"endcomplexpolygon(): End drawing a complex filled polygon.", //510 kifltk.cxx 12168
"arc", //511 kifltk.cxx 12171
"arc(int x,int y,int x1, int y1, float a1, float a2): Draw an arc.\rarc(float x,float y,float rad,float a1,float a2): Add a series of points to the current path on the arc of a circle;", //512 kifltk.cxx 12171
"curve", //513 kifltk.cxx 12172
"curve(float x,float y,float x1,float y1,float x1,float y2,float x3,float y3): Add a series of points on a Bezier curve to the path. The curve ends (and two of the points) are at x,y and x3,y3.", //514 kifltk.cxx 12172
"pushclip", //515 kifltk.cxx 12173
"pushclip(int x,int y,int w, int h): Insert a clip region, with the following coordinates", //516 kifltk.cxx 12173
"popclip", //517 kifltk.cxx 12174
"popclip(): Release a clip region", //518 kifltk.cxx 12174
"pie", //519 kifltk.cxx 12176
"pie(int x,int y,int x1, int y1, float a1, float a2): Draw a pie", //520 kifltk.cxx 12176
"point", //521 kifltk.cxx 12177
"point(int x,int y): Draw a pixel", //522 kifltk.cxx 12177
"circle", //523 kifltk.cxx 12178
"circle(int x,int y,int r,int color): Draw a circle. 'color' is optional.", //524 kifltk.cxx 12178
"textsize", //525 kifltk.cxx 12180
"textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels", //526 kifltk.cxx 12180
"drawtext", //527 kifltk.cxx 12181
"drawtext(string l,int x,int y): Put a text at position x,y", //528 kifltk.cxx 12181
"redraw", //529 kifltk.cxx 12182
"redraw(): Redraw the window", //530 kifltk.cxx 12182
"rgbcolor", //531 kifltk.cxx 12183
"rgbcolor(string|int color)|(int r,int g,int b)|(vector color): either return a vector of the color decomposition or return the value of rgb values combined", //532 kifltk.cxx 12183
"font", //533 kifltk.cxx 12185
"font(string f,int sz): Set the font name and its size", //534 kifltk.cxx 12185
"ondragdrop", //535 kifltk.cxx 12186
"ondragdrop(function,object): Set the call back function for drag and drop action.", //536 kifltk.cxx 12186
"onmouse", //537 kifltk.cxx 12187
"onmouse(int action, function,object): Set the call back function on a mouse action with a given object as parameter", //538 kifltk.cxx 12187
"onkey", //539 kifltk.cxx 12188
"onkey(int action, function,object): Set the call back function on a keyboard action with a given object as parameter", //540 kifltk.cxx 12188
"cursorstyle", //541 kifltk.cxx 12189
"cursorstyle(int cursortype, int color,int color): Set the cursor shape", //542 kifltk.cxx 12189
"hide", //543 kifltk.cxx 12190
"hide(bool h): Hide the window if h is true", //544 kifltk.cxx 12190
"show", //545 kifltk.cxx 12191
"show(): Show the window", //546 kifltk.cxx 12191
"focus", //547 kifltk.cxx 12192
"focus(): Get the focus", //548 kifltk.cxx 12192
"flush", //549 kifltk.cxx 12193
"flush(): force redrawing of window", //550 kifltk.cxx 12193
"position", //551 kifltk.cxx 12194
"position()|(int x,int y): Return the window position or set the window position", //552 kifltk.cxx 12194
"size()|(int x,int y,int w, int h): Return the window size or set the window size", //553 kifltk.cxx 12195
"awake", //554 kifltk.cxx 12196
"awake(): Awake a threaded window", //555 kifltk.cxx 12196
"lock", //556 kifltk.cxx 12197
"lock(): Thread lock", //557 kifltk.cxx 12197
"unlock", //558 kifltk.cxx 12198
"unlock(): Thread unlock", //559 kifltk.cxx 12198
"image(image image,int x, int y, int w, int h): Display an image", //560 kifltk.cxx 12199
"bitmap(bitmap image,int color,int x, int y, int w, int h): Display a bitmap", //561 kifltk.cxx 12200
"labelsize", //562 kifltk.cxx 12201
"labelsize(int c): set or return the label font size", //563 kifltk.cxx 12201
"labelfont", //564 kifltk.cxx 12202
"labelfont(int c): set or return the label font", //565 kifltk.cxx 12202
"labelcolor", //566 kifltk.cxx 12203
"labelcolor(int c): set or return the label color", //567 kifltk.cxx 12203
"labeltype", //568 kifltk.cxx 12204
"labeltype(int c): set or return the label type", //569 kifltk.cxx 12204
"label", //570 kifltk.cxx 12205
"label(string s): set or return the label text", //571 kifltk.cxx 12205
"selectioncolor", //572 kifltk.cxx 12206
"selectioncolor(int color): Color for the selected elements", //573 kifltk.cxx 12206
"coords", //574 kifltk.cxx 12207
"coords(int c): return a vector of the widget coordinates", //575 kifltk.cxx 12207
"created", //576 kifltk.cxx 12208
"created(): Return true if the object has been created", //577 kifltk.cxx 12208
"copy", //578 kifltk.cxx 12209
"copy(string): Copy string to clipboard", //579 kifltk.cxx 12209
"paste", //580 kifltk.cxx 12210
"paste(): paste string", //581 kifltk.cxx 12210
"align", //582 kifltk.cxx 12211
"align(int): define the label alignment", //583 kifltk.cxx 12211
"create(int x,int y,int w,int h,string label): Create a scroll", //584 kifltk.cxx 12213
"resize", //585 kifltk.cxx 12214
"resize(object): make the object resizable", //586 kifltk.cxx 12214
"show(): Show the widget", //587 kifltk.cxx 12215
"hide(): Hide the widget", //588 kifltk.cxx 12217
"tooltip", //589 kifltk.cxx 12223
"tooltip(string msg): Add a tooltip message to the widget", //590 kifltk.cxx 12223
"create(int x,int y,int w,int h,string label): Create a wcounter", //591 kifltk.cxx 12231
"bounds", //592 kifltk.cxx 12232
"bounds(float x,float y): defines the wcounter boundary", //593 kifltk.cxx 12232
"type", //594 kifltk.cxx 12233
"type(bool normal): if 'true' then normal wcounter or simple wcounter", //595 kifltk.cxx 12233
"step", //596 kifltk.cxx 12234
"step(double): define the wcounter step", //597 kifltk.cxx 12234
"lstep", //598 kifltk.cxx 12235
"lstep(double): define the large wcounter step", //599 kifltk.cxx 12235
"steps", //600 kifltk.cxx 12236
"steps(double): define the wcounter steps, normal and large.", //601 kifltk.cxx 12236
"value", //602 kifltk.cxx 12237
"value(float): define the value for the wcounter or return its value", //603 kifltk.cxx 12237
"font(int s): set or return the text font", //604 kifltk.cxx 12253
"textcolor", //605 kifltk.cxx 12254
"textcolor(string code|int code): Set the color of the text", //606 kifltk.cxx 12254
"create(int x,int y,int w,int h,string label): Create a progress bar", //607 kifltk.cxx 12256
"minimum", //608 kifltk.cxx 12257
"minimum(float x): defines the minimum or return the minimum", //609 kifltk.cxx 12257
"maximum", //610 kifltk.cxx 12258
"maximum(float x): defines the maximum or return the maximum", //611 kifltk.cxx 12258
"value(float): define the value for the progress bar or return its value", //612 kifltk.cxx 12259
"barcolor", //613 kifltk.cxx 12264
"barcolor(int color): Color for the progression bar", //614 kifltk.cxx 12264
"create(int x,int y,int w,int h,int alignment,bool valueslider,string label): Create a slider or a valueslider", //615 kifltk.cxx 12270
"bounds(int x,int y): defines the slider boundary", //616 kifltk.cxx 12272
"type(int x): Value slider type", //617 kifltk.cxx 12273
"boxtype", //618 kifltk.cxx 12274
"boxtype(int x): Define the slider box type", //619 kifltk.cxx 12274
"align(int): define the slider alignement", //620 kifltk.cxx 12275
"step(int): define the slider step", //621 kifltk.cxx 12276
"value(int): define the value for the slider or return its value", //622 kifltk.cxx 12277
"create(int x,int y,int w,int h,string label): Create a box", //623 kifltk.cxx 12293
"type(string|int boxtype): Define the box type", //624 kifltk.cxx 12294
"create(int x,int y,int w,int h,string type,string shape,string label): Create a button. If type is not provided then it is: FL_Regular.", //625 kifltk.cxx 12310
"when", //626 kifltk.cxx 12311
"when(string when1, string when2,...): Type of event for a button which triggers the callback", //627 kifltk.cxx 12311
"shortcut", //628 kifltk.cxx 12312
"shortcut(string keycode): Set a shortcut to activate the button from the keyboard", //629 kifltk.cxx 12312
"color", //630 kifltk.cxx 12313
"color(string code|int code): Set the color of the button", //631 kifltk.cxx 12313
"value(): return the value of the current button", //632 kifltk.cxx 12314
"align(int): define the button label alignment", //633 kifltk.cxx 12315
"image(image im,string label,int labelalign): Use the image as a button image", //634 kifltk.cxx 12316
"bitmap(bitmap im,int color,string label,int labelalign): Use the bitmap as a button image", //635 kifltk.cxx 12317
"begin(int x,int y,int w, int h,string title): Create a tab window and begin initialisation", //636 kifltk.cxx 12333
"end(): end tab creation", //637 kifltk.cxx 12334
"current", //638 kifltk.cxx 12335
"current(): Return the current active tab", //639 kifltk.cxx 12335
"add", //640 kifltk.cxx 12336
"add(wgroup g): Add dynamically a new tab", //641 kifltk.cxx 12336
"remove", //642 kifltk.cxx 12337
"remove(wgroup g): Remove a tab", //643 kifltk.cxx 12337
"begin(int x,int y,int w, int h,string title): Create a group within a tab and begin initialisation", //644 kifltk.cxx 12354
"end(): end group creation", //645 kifltk.cxx 12355
"activate", //646 kifltk.cxx 12358
"activate(): Active this tab", //647 kifltk.cxx 12358
"loadjpeg", //648 kifltk.cxx 12424
"loadjpeg(string filename): Load a JPEG image", //649 kifltk.cxx 12424
"loadgif", //650 kifltk.cxx 12426
"loadgif(string filename): Load a GIF image", //651 kifltk.cxx 12426
"load", //652 kifltk.cxx 12428
"load(vector bitmaps,int w,int h): Load a bitmap", //653 kifltk.cxx 12428
"create(int x,int y,int w,int h,string label): Create a table of objects, and starts adding", //654 kifltk.cxx 12430
"clear", //655 kifltk.cxx 12431
"clear(): Clear the table", //656 kifltk.cxx 12431
"add(int R,int C,string v): Add a value on row R and column C", //657 kifltk.cxx 12432
"cell", //658 kifltk.cxx 12433
"cell(int R,int C): Return the value of the cell on row R and column C", //659 kifltk.cxx 12433
"cellalign", //660 kifltk.cxx 12434
"cellalign(align): Set the inner cell alignement", //661 kifltk.cxx 12434
"cellalignheadercol", //662 kifltk.cxx 12435
"cellalignheadercol(align): Set the header column alignement", //663 kifltk.cxx 12435
"cellalignheaderrow", //664 kifltk.cxx 12436
"cellalignheaderrow(align): Set the header row alignement", //665 kifltk.cxx 12436
"row", //666 kifltk.cxx 12437
"row()|(int nb): Define the number of rows", //667 kifltk.cxx 12437
"column", //668 kifltk.cxx 12438
"column()|(int nb): Define the number of columns", //669 kifltk.cxx 12438
"rowheight", //670 kifltk.cxx 12439
"rowheight(int height): Define the row height in pixel", //671 kifltk.cxx 12439
"columnwidth", //672 kifltk.cxx 12440
"columnwidth(int width): Define the column width in pixel", //673 kifltk.cxx 12440
"fontsize", //674 kifltk.cxx 12442
"fontsize(int c): set or return the text font size", //675 kifltk.cxx 12442
"colorbg", //676 kifltk.cxx 12443
"colorbg(int c): set or return the cell color background", //677 kifltk.cxx 12443
"colorfg", //678 kifltk.cxx 12444
"colorfg(int c): set or return the cell color foreground", //679 kifltk.cxx 12444
"when(string when1, string when2,...): Type of event to trigger the callback", //680 kifltk.cxx 12445
"colorselection(int color): Color for the selected elements", //681 kifltk.cxx 12446
"boxtype(int boxtype): box type", //682 kifltk.cxx 12447
"rowheader", //683 kifltk.cxx 12448
"rowheader(int pos,string label): Set the row header label at row pos", //684 kifltk.cxx 12448
"columnheader", //685 kifltk.cxx 12449
"columnheader(int pos,string label): Set the column header label at column pos", //686 kifltk.cxx 12449
"rowheaderheight", //687 kifltk.cxx 12450
"rowheaderheight(int height): the size in pixel of the row header", //688 kifltk.cxx 12450
"columnheaderwidth", //689 kifltk.cxx 12451
"columnheaderwidth(int width): the size in pixel of the column header", //690 kifltk.cxx 12451
"create(int x,int y,int w,int h,boolean multiline,string label): Create an input", //691 kifltk.cxx 12469
"value()|(string v): return the input buffer or set the initial buffer", //692 kifltk.cxx 12470
"insert", //693 kifltk.cxx 12471
"insert(string s,int p): insert s at position p in the input", //694 kifltk.cxx 12471
"selection", //695 kifltk.cxx 12472
"selection(): return the selected text in the input", //696 kifltk.cxx 12472
"color(string c|int c): set or return the text color", //697 kifltk.cxx 12473
"word", //698 kifltk.cxx 12474
"word(int pos): Return the word at position pos", //699 kifltk.cxx 12474
"font(string s): set or return the text font", //700 kifltk.cxx 12475
"create(int x,int y,int w,int h,string label): Create an editor", //701 kifltk.cxx 12495
"selection(): return the selected text in the editor or the coordinates of the selection according to the recipient variable", //702 kifltk.cxx 12496
"value()|(string v): return the text in the editor or initialize the editor", //703 kifltk.cxx 12497
"font(string s)|(): set or return the text font", //704 kifltk.cxx 12499
"fontsize(int c)|(): set or return the text font size", //705 kifltk.cxx 12500
"setstyle", //706 kifltk.cxx 12501
"setstyle(int start,int end,string style): set the style of a portion of text", //707 kifltk.cxx 12501
"load(string f): load file into editor", //708 kifltk.cxx 12502
"save", //709 kifltk.cxx 12503
"save(string f): save buffer to file f", //710 kifltk.cxx 12503
"replace", //711 kifltk.cxx 12504
"replace(string s,string sub,int i,bool matchcase): replace s with sub according to matchcase starting at i", //712 kifltk.cxx 12504
"replaceall", //713 kifltk.cxx 12505
"replaceall(string s,string sub,bool matchcase): replace all occurrences of s with sub according to matchcase", //714 kifltk.cxx 12505
"find", //715 kifltk.cxx 12506
"find(string s,int i,bool matchcase): find the position of a substring from i", //716 kifltk.cxx 12506
"rfind", //717 kifltk.cxx 12507
"rfind(string s,int i,bool matchcase): find the position of a substring from i backward", //718 kifltk.cxx 12507
"highlight", //719 kifltk.cxx 12508
"highlight(int start,int end)|(): highlight a portion of text. Return true if the text is highlighted or the highlighted text", //720 kifltk.cxx 12508
"unhighlight", //721 kifltk.cxx 12509
"unhighlight(): unhighlight a portion of text", //722 kifltk.cxx 12509
"cursorstyle(int cursorshape): Set the cursor shape", //723 kifltk.cxx 12510
"onmouse(int action,function f,object o): callback when clicking", //724 kifltk.cxx 12511
"onkey(int action,function f,object o): callback when a key is pressed", //725 kifltk.cxx 12512
"onvscroll", //726 kifltk.cxx 12513
"onvscroll(function f,object o): callback when scrolling vertically", //727 kifltk.cxx 12513
"onhscroll", //728 kifltk.cxx 12514
"onhscroll(function f,object o): callback when scrolling horizontally", //729 kifltk.cxx 12514
"getstyle", //730 kifltk.cxx 12515
"getstyle(int start,int end): return a vector of style for each character of the text section", //731 kifltk.cxx 12515
"addstyle", //732 kifltk.cxx 12516
"addstyle(map style): Initialize styles", //733 kifltk.cxx 12516
"delete", //734 kifltk.cxx 12518
"delete(): delete selected text from editor", //735 kifltk.cxx 12518
"cut", //736 kifltk.cxx 12519
"cut(): cut selected text from editor into clipboard", //737 kifltk.cxx 12519
"copy(string s): copy selected text from editor into clipboard (string s is optional)", //738 kifltk.cxx 12520
"paste(): paste selected text from clipboard", //739 kifltk.cxx 12521
"select", //740 kifltk.cxx 12524
"select(): Return selected string.\rselect(int posfirst,int poslast): Select the string between posfirst and posend", //741 kifltk.cxx 12524
"line(int l): return the current line position of the caret or return the line corresponding to a position", //742 kifltk.cxx 12525
"linebounds", //743 kifltk.cxx 12526
"linebounds(int l): return the beginning and the end at the current line position of the caret or return the line boundaries corresponding to a position", //744 kifltk.cxx 12526
"linecharbounds", //745 kifltk.cxx 12527
"linecharbounds(): return the beginning and the end at the current line position of the caret or return the line boundaries corresponding to a position as characters", //746 kifltk.cxx 12527
"cursor(int l): return the current cursor position of the caret or set the cursor position", //747 kifltk.cxx 12528
"cursorchar", //748 kifltk.cxx 12529
"cursorchar(): return the current cursor position of the caret as characters", //749 kifltk.cxx 12529
"gotoline", //750 kifltk.cxx 12530
"gotoline(int l,bool highlight): Goto line l. If true, highlight the line", //751 kifltk.cxx 12530
"annotate", //752 kifltk.cxx 12531
"annotate(string s|vector v,string style,bool matchcase): Annotate each occurence of s with style (matchcase is a default argument)", //753 kifltk.cxx 12531
"wrap", //754 kifltk.cxx 12533
"wrap(bool): Set the wrapping mode", //755 kifltk.cxx 12533
"annotateregexip", //756 kifltk.cxx 12534
"annotateregexip(string reg,string style): Annotate each string matching reg with style", //757 kifltk.cxx 12534
"append", //758 kifltk.cxx 12550
"append(string s): append s at the end of the input", //759 kifltk.cxx 12550
"byteposition", //760 kifltk.cxx 12551
"byteposition(int i): return the position in byte from a character position", //761 kifltk.cxx 12551
"charposition", //762 kifltk.cxx 12552
"charposition(int i): return the position in character from a byte position", //763 kifltk.cxx 12552
"create(int x,int y,int w,int h,boolean multiline,string label): Create an output", //764 kifltk.cxx 12555
"value(string s): set the output buffer", //765 kifltk.cxx 12556
"create(int x,int y,int w,int h,string label): Create an choice", //766 kifltk.cxx 12578
"value(int s): set the choice initialization value", //767 kifltk.cxx 12579
"menu(vector s): Initialize the menu", //768 kifltk.cxx 12581
"create(string intialdirectory,string filter,bool directory,string label): Open a file browser, select directories if directory is true", //769 kifltk.cxx 12599
"value(): Return the selected file", //770 kifltk.cxx 12600
"close(): Close the file browser", //771 kifltk.cxx 12601
"ok", //772 kifltk.cxx 12602
"ok(): return true if ok was pressed", //773 kifltk.cxx 12602
"open", //774 kifltk.cxx 12605
"open(): Choose a file via a mac window", //775 kifltk.cxx 12605
"load(string): Load a file into a mac window", //776 kifltk.cxx 12606
"create(x,y,w,h,label): Create a browser", //777 kifltk.cxx 12609
"add(label): Add a string to the browser", //778 kifltk.cxx 12610
"load(filename): Load a file into the browser", //779 kifltk.cxx 12611
"insert(l,label): Insert a label before line l", //780 kifltk.cxx 12612
"clear(): Clear the browser from all values", //781 kifltk.cxx 12613
"value(): return the current selected value", //782 kifltk.cxx 12614
"select(): Return selected string.\rselect(int i): Return string in position i", //783 kifltk.cxx 12615
"deselect", //784 kifltk.cxx 12616
"deselect(): Deselect all items\rdeselect(int i): Deselect item i", //785 kifltk.cxx 12616
"columnchar", //786 kifltk.cxx 12617
"columnchar(): Return the column char separator.\rcolumnchar(string): Set the column char separator", //787 kifltk.cxx 12617
"formatchar", //788 kifltk.cxx 12618
"formatchar(): Return the format char.\rformatchar(string): Set the format char", //789 kifltk.cxx 12618
"label(): Return the item label.", //790 kifltk.cxx 12637
"depth", //791 kifltk.cxx 12638
"depth(): Return the depth of the item.", //792 kifltk.cxx 12638
"value(object): Associate the item with a value or return that value.", //793 kifltk.cxx 12639
"clean", //794 kifltk.cxx 12640
"clean(): Remove the object associated through value.", //795 kifltk.cxx 12640
"bgcolor", //796 kifltk.cxx 12641
"bgcolor(int c): Set or return the item background color.", //797 kifltk.cxx 12641
"fgcolor", //798 kifltk.cxx 12642
"fgcolor(int c): Set or return the foreground color.", //799 kifltk.cxx 12642
"font(int c): Set or return the item font.", //800 kifltk.cxx 12643
"fontsize(int c): Set or return the item font size.", //801 kifltk.cxx 12644
"isclosed", //802 kifltk.cxx 12645
"isclosed(): Return true if element is closed.", //803 kifltk.cxx 12645
"isopen", //804 kifltk.cxx 12646
"isopen(): Return true if element is open.", //805 kifltk.cxx 12646
"isroot", //806 kifltk.cxx 12647
"isroot(): Return true if element is root.", //807 kifltk.cxx 12647
"isselected", //808 kifltk.cxx 12648
"isselected(): Return true if element is selected.", //809 kifltk.cxx 12648
"isactive", //810 kifltk.cxx 12649
"isactive(): Return true if element is active.", //811 kifltk.cxx 12649
"parent", //812 kifltk.cxx 12650
"parent(): Return the last element.", //813 kifltk.cxx 12650
"next", //814 kifltk.cxx 12651
"next(): Return the next element.", //815 kifltk.cxx 12651
"previous", //816 kifltk.cxx 12652
"previous(): Return the previous element.", //817 kifltk.cxx 12652
"child", //818 kifltk.cxx 12653
"child(int i): Return the child element at position i.", //819 kifltk.cxx 12653
"children", //820 kifltk.cxx 12654
"children(): Return number of children.", //821 kifltk.cxx 12654
"activate(): Activate the current element.", //822 kifltk.cxx 12655
"deactivate", //823 kifltk.cxx 12656
"deactivate(): Deactivate the current element.", //824 kifltk.cxx 12656
"create(int x,int y,int h,int w,string label): Create a tree", //825 kifltk.cxx 12658
"clear(): Delete the tree items", //826 kifltk.cxx 12659
"add(string path): Add a tree item.\radd(wtreeitem e,string n): Add a tree item after e.", //827 kifltk.cxx 12660
"isclosed(string path): Return true if element is closed.\risclosed(wtreeitem e): Return true if element is closed.", //828 kifltk.cxx 12661
"close(string path): Close the element.\rclose(wtreeitem e): Close the element.", //829 kifltk.cxx 12662
"open(string path): Open the element.\ropen(wtreeitem e): Open the element.", //830 kifltk.cxx 12663
"insert(wtreeitem e,string label,int pos): Insert an element after e with label at position pos in the children list.", //831 kifltk.cxx 12664
"remove(wtreeitem e): Remove the element e from the tree.", //832 kifltk.cxx 12665
"insertabove", //833 kifltk.cxx 12666
"insertabove(wtreeitem e,string label): Insert an element above e with label.", //834 kifltk.cxx 12666
"connectorcolor", //835 kifltk.cxx 12667
"connectorcolor(int c): Set or return the connector color.", //836 kifltk.cxx 12667
"connectorstyle", //837 kifltk.cxx 12668
"connectorstyle(int s): Set or return the connector style.", //838 kifltk.cxx 12668
"connectorwidth", //839 kifltk.cxx 12669
"connectorwidth(int s): Set or return the connector width.", //840 kifltk.cxx 12669
"sortorder", //841 kifltk.cxx 12670
"sortorder(int s): Set or return the sort order.", //842 kifltk.cxx 12670
"selectmode", //843 kifltk.cxx 12671
"selectmode(int s): Set or return the selection mode.", //844 kifltk.cxx 12671
"itembgcolor", //845 kifltk.cxx 12673
"itembgcolor(int c): Set or return the item background color.", //846 kifltk.cxx 12673
"itemfgcolor", //847 kifltk.cxx 12674
"itemfgcolor(int c): Set or return the foreground color.", //848 kifltk.cxx 12674
"itemfont", //849 kifltk.cxx 12675
"itemfont(int c): Set or return the item font.", //850 kifltk.cxx 12675
"itemsize", //851 kifltk.cxx 12676
"itemsize(int c): Set or return the item font size.", //852 kifltk.cxx 12676
"marginleft", //853 kifltk.cxx 12678
"Set or Get the amount of white space (in pixels) that should appear between the widget's left border and the tree's contents.", //854 kifltk.cxx 12678
"margintop", //855 kifltk.cxx 12679
"margintop(int s): Set or Get the amount of white space (in pixels) that should appear between the widget's top border and the top of the tree's contents.", //856 kifltk.cxx 12679
"root", //857 kifltk.cxx 12681
"root(): Return the root element.", //858 kifltk.cxx 12681
"rootlabel", //859 kifltk.cxx 12682
"rootlabel(string r): Set the root label.", //860 kifltk.cxx 12682
"clicked", //861 kifltk.cxx 12683
"clicked(): Return the element that was clicked.", //862 kifltk.cxx 12683
"first", //863 kifltk.cxx 12684
"first(): Return the first element.", //864 kifltk.cxx 12684
"find(string path): Return the element matching the path.", //865 kifltk.cxx 12685
"last", //866 kifltk.cxx 12686
"last(): Return the last element.", //867 kifltk.cxx 12686
"next(wtreeitem e): Return the next element after e.", //868 kifltk.cxx 12687
"previous(wtreeitem e): Return the previous element before e.", //869 kifltk.cxx 12688
""};


Exported const char* KIFLTKSDEBUGGERTRINGS[]={
"println", //0 kifeditor.cxx 44
"while", //1 kifeditor.cxx 45
"for", //2 kifeditor.cxx 46
"switch", //3 kifeditor.cxx 47
"if", //4 kifeditor.cxx 48
"else", //5 kifeditor.cxx 49
"elif", //6 kifeditor.cxx 50
"//Local variables:\n", //7 kifeditor.cxx 1017
"//Main variables:\n", //8 kifeditor.cxx 1029
"//Frame variables:", //9 kifeditor.cxx 1042
"//Function variables:", //10 kifeditor.cxx 1054
"Terminated", //11 kifeditor.cxx 1072
"Find", //12 kifeditor.cxx 1366
"String:", //13 kifeditor.cxx 1367
"Next", //14 kifeditor.cxx 1368
"Cancel", //15 kifeditor.cxx 1369
"Case", //16 kifeditor.cxx 1370
"Replace", //17 kifeditor.cxx 1409
"Find:", //18 kifeditor.cxx 1410
"Replace:", //19 kifeditor.cxx 1411
"Replace All", //20 kifeditor.cxx 1413
"Replace Next", //21 kifeditor.cxx 1414
"Suggest", //22 kifeditor.cxx 1523
"Noname", //23 kifeditor.cxx 1551
"Creating a new file:", //24 kifeditor.cxx 1557
"Line", //25 kifeditor.cxx 1657
"Blanks Size", //26 kifeditor.cxx 1668
"Two 'runs'.\n\tThe first runs in the main frame and CANNOT BE stopped.\n\tThe second runs in a thread and CAN BE stopped.", //27 kifeditor.cxx 2287
"File/Save", //28 kifeditor.cxx 2290
"File/Save as", //29 kifeditor.cxx 2291
"Edit/Undo", //30 kifeditor.cxx 2293
"Edit/Redo", //31 kifeditor.cxx 2294
"Edit/List", //32 kifeditor.cxx 2299
"Edit/Matching", //33 kifeditor.cxx 2300
"Edit/Indent", //34 kifeditor.cxx 2301
"Edit/(Un)comment", //35 kifeditor.cxx 2302
"Edit/Goto line", //36 kifeditor.cxx 2303
"Edit/Set blanks", //37 kifeditor.cxx 2304
"Edit/Find", //38 kifeditor.cxx 2305
"Edit/Replace", //39 kifeditor.cxx 2306
"Edit/Next", //40 kifeditor.cxx 2307
"Edit/Font larger", //41 kifeditor.cxx 2308
"Edit/Font smaller", //42 kifeditor.cxx 2309
"Edit/Cut", //43 kifeditor.cxx 2310
"Edit/Copy", //44 kifeditor.cxx 2311
"Edit/Paste", //45 kifeditor.cxx 2312
"Command/Run (main thread)", //46 kifeditor.cxx 2313
"Command/Run (threaded)", //47 kifeditor.cxx 2314
"Command/Stop", //48 kifeditor.cxx 2315
"Command/Debug", //49 kifeditor.cxx 2316
"Command/Toggle Breakpoint", //50 kifeditor.cxx 2317
"Command/Remove All Breakpoints", //51 kifeditor.cxx 2318
"Choose", //52 kifeditor.cxx 2380
"No description available", //53 kifeditor.cxx 2401
"File/New", //54 kifeditor.cxx 2620
"File/Open", //55 kifeditor.cxx 2621
"File/Quit", //56 kifeditor.cxx 2623
"Edit/Editor", //57 kifeditor.cxx 2632
"Edit/Clean", //58 kifeditor.cxx 2633
"*.kif|*.xip", //59 kifeditor.cxx 2650
"Load your file", //60 kifeditor.cxx 2650
"Save your file", //61 kifeditor.cxx 2668
"cleaned\n", //62 kifeditor.cxx 2704
"none", //63 kifeditor.cxx 2880
"Wrong instruction", //64 kifeditor.cxx 3043
"\nrecorded\n", //65 kifeditor.cxx 3048
"File:'", //66 kifeditor.cxx 3148
"' not saved... Do you want to quit? ", //67 kifeditor.cxx 3148
"Do you want to quit?", //68 kifeditor.cxx 3157
"Debugger", //69 kifeditor.cxx 3357
"Short value", //70 kifeditor.cxx 3359
"Locals", //71 kifeditor.cxx 3360
"Alls", //72 kifeditor.cxx 3361
"Stack", //73 kifeditor.cxx 3362
"Filename", //74 kifeditor.cxx 3363
"Goto", //75 kifeditor.cxx 3365
"In", //76 kifeditor.cxx 3366
"Out", //77 kifeditor.cxx 3367
"Stop", //78 kifeditor.cxx 3368
"To End", //79 kifeditor.cxx 3369
"Save your file as", //80 kifeditor.cxx 3534
"Save this window first", //81 kifeditor.cxx 3760
"is", //82 kifeditor.cxx 4055
"do", //83 kifeditor.cxx 4057
"predicatevar", //84 kifeditor.cxx 4058
"predicate", //85 kifeditor.cxx 4059
"term", //86 kifeditor.cxx 4061
"xiptrans", //87 kifeditor.cxx 4063
"node", //88 kifeditor.cxx 4066
"garbagefunction", //89 kifeditor.cxx 4067
"grammar", //90 kifeditor.cxx 4068
"otherwise", //91 kifeditor.cxx 4069
"garbagesize", //92 kifeditor.cxx 4070
"removeerroronkey", //93 kifeditor.cxx 4071
"setdebugfunction", //94 kifeditor.cxx 4072
"parentthreadid", //95 kifeditor.cxx 4073
"mvector", //96 kifeditor.cxx 4074
"setdebugdata", //97 kifeditor.cxx 4075
"sthrough", //98 kifeditor.cxx 4076
"ithrough", //99 kifeditor.cxx 4077
"fthrough", //100 kifeditor.cxx 4078
"createserver", //101 kifeditor.cxx 4079
"getstackinfo", //102 kifeditor.cxx 4080
"waitonfalse", //103 kifeditor.cxx 4081
"env", //104 kifeditor.cxx 4082
"runadthread", //105 kifeditor.cxx 4083
"synchronous", //106 kifeditor.cxx 4084
"filebrowser", //107 kifeditor.cxx 4085
"polynomial", //108 kifeditor.cxx 4086
"printlnerr", //109 kifeditor.cxx 4087
"printjlnerr", //110 kifeditor.cxx 4088
"waitonjoined", //111 kifeditor.cxx 4089
"myvariable", //112 kifeditor.cxx 4090
"marginleft", //113 kifeditor.cxx 4091
"exclusive", //114 kifeditor.cxx 4092
"joined", //115 kifeditor.cxx 4093
"wtreeitem", //116 kifeditor.cxx 4094
"protected", //117 kifeditor.cxx 4095
"riterator", //118 kifeditor.cxx 4097
"fraction", //119 kifeditor.cxx 4098
"printerr", //120 kifeditor.cxx 4099
"printjerr", //121 kifeditor.cxx 4100
"function", //122 kifeditor.cxx 4101
"sequence", //123 kifeditor.cxx 4103
"threadid", //124 kifeditor.cxx 4104
"pathname", //125 kifeditor.cxx 4105
"continue", //126 kifeditor.cxx 4106
"iterator", //127 kifeditor.cxx 4107
"instance", //128 kifeditor.cxx 4108
"printjln", //129 kifeditor.cxx 4110
"address", //130 kifeditor.cxx 4111
"boolean", //131 kifeditor.cxx 4112
"autorun", //132 kifeditor.cxx 4113
"private", //133 kifeditor.cxx 4114
"browser", //134 kifeditor.cxx 4115
"wchoice", //135 kifeditor.cxx 4116
"woutput", //136 kifeditor.cxx 4117
"waiting", //137 kifeditor.cxx 4118
"version", //138 kifeditor.cxx 4119
"matrix", //139 kifeditor.cxx 4120
"wheely", //140 kifeditor.cxx 4121
"wheelx", //141 kifeditor.cxx 4122
"remote", //142 kifeditor.cxx 4123
"minute", //143 kifeditor.cxx 4124
"gcsize", //144 kifeditor.cxx 4125
"socket", //145 kifeditor.cxx 4126
"sound", //146 kifeditor.cxx 4127
"mp3", //147 kifeditor.cxx 4128
"button", //148 kifeditor.cxx 4129
"scroll", //149 kifeditor.cxx 4130
"xmldoc", //150 kifeditor.cxx 4132
"editor", //151 kifeditor.cxx 4133
"loadin", //152 kifeditor.cxx 4134
"winput", //153 kifeditor.cxx 4135
"slider", //154 kifeditor.cxx 4136
"wgroup", //155 kifeditor.cxx 4137
"image", //156 kifeditor.cxx 4138
"pflush", //157 kifeditor.cxx 4140
"rawstring", //158 kifeditor.cxx 4141
"string", //159 kifeditor.cxx 4142
"ustring", //160 kifeditor.cxx 4143
"tuple", //161 kifeditor.cxx 4144
"common", //162 kifeditor.cxx 4145
"random", //163 kifeditor.cxx 4146
"bottom", //164 kifeditor.cxx 4147
"vector", //165 kifeditor.cxx 4148
"return", //166 kifeditor.cxx 4149
"_return", //167 kifeditor.cxx 4150
"thread", //168 kifeditor.cxx 4151
"window", //169 kifeditor.cxx 4152
"wcounter", //170 kifeditor.cxx 4153
"wprogress", //171 kifeditor.cxx 4154
"bitmap", //172 kifeditor.cxx 4155
"second", //173 kifeditor.cxx 4156
"strict", //174 kifeditor.cxx 4157
"print", //175 kifeditor.cxx 4158
"printj", //176 kifeditor.cxx 4159
"catch", //177 kifeditor.cxx 4160
"table", //178 kifeditor.cxx 4161
"false", //179 kifeditor.cxx 4162
"eigen", //180 kifeditor.cxx 4163
"wtabs", //181 kifeditor.cxx 4164
"frame", //182 kifeditor.cxx 4165
"extension", //183 kifeditor.cxx 4166
"start", //184 kifeditor.cxx 4168
"stdin", //185 kifeditor.cxx 4169
"not", //186 kifeditor.cxx 4170
"absent", //187 kifeditor.cxx 4171
"from", //188 kifeditor.cxx 4172
"notin", //189 kifeditor.cxx 4173
"pause", //190 kifeditor.cxx 4174
"const", //191 kifeditor.cxx 4175
"sleep", //192 kifeditor.cxx 4176
"reset", //193 kifeditor.cxx 4177
"range", //194 kifeditor.cxx 4178
"break", //195 kifeditor.cxx 4179
"raise", //196 kifeditor.cxx 4180
"float", //197 kifeditor.cxx 4181
"wtree", //198 kifeditor.cxx 4182
"bool", //199 kifeditor.cxx 4184
"self", //200 kifeditor.cxx 4185
"auto", //201 kifeditor.cxx 4186
"eval", //202 kifeditor.cxx 4187
"true", //203 kifeditor.cxx 4188
"fail", //204 kifeditor.cxx 4189
"cast", //205 kifeditor.cxx 4190
"this", //206 kifeditor.cxx 4191
"getc", //207 kifeditor.cxx 4192
"long", //208 kifeditor.cxx 4193
"time", //209 kifeditor.cxx 4194
"call", //210 kifeditor.cxx 4195
"list", //211 kifeditor.cxx 4196
"port", //212 kifeditor.cxx 4197
"exit", //213 kifeditor.cxx 4198
"wfile", //214 kifeditor.cxx 4199
"file", //215 kifeditor.cxx 4200
"hash", //216 kifeditor.cxx 4202
"bits", //217 kifeditor.cxx 4203
"empty", //218 kifeditor.cxx 4204
"null", //219 kifeditor.cxx 4205
"kill", //220 kifeditor.cxx 4206
"byte", //221 kifeditor.cxx 4207
"int", //222 kifeditor.cxx 4208
"isa", //223 kifeditor.cxx 4209
"use", //224 kifeditor.cxx 4210
"top", //225 kifeditor.cxx 4212
"box", //226 kifeditor.cxx 4214
"try", //227 kifeditor.cxx 4215
"bit", //228 kifeditor.cxx 4216
"tree", //229 kifeditor.cxx 4217
"where", //230 kifeditor.cxx 4220
"repeat", //231 kifeditor.cxx 4221
"cycle", //232 kifeditor.cxx 4222
"replicate", //233 kifeditor.cxx 4223
"filter", //234 kifeditor.cxx 4224
"take", //235 kifeditor.cxx 4225
"takeWhile", //236 kifeditor.cxx 4226
"zipWith", //237 kifeditor.cxx 4227
"zip", //238 kifeditor.cxx 4228
"flip", //239 kifeditor.cxx 4229
"drop", //240 kifeditor.cxx 4230
"dropWhile", //241 kifeditor.cxx 4231
"foldr", //242 kifeditor.cxx 4232
"foldl", //243 kifeditor.cxx 4233
"foldr1", //244 kifeditor.cxx 4234
"foldl1", //245 kifeditor.cxx 4235
"scanr", //246 kifeditor.cxx 4236
"scanl", //247 kifeditor.cxx 4237
"scanr1", //248 kifeditor.cxx 4238
"scanl1", //249 kifeditor.cxx 4239
"set", //250 kifeditor.cxx 4241
"sset", //251 kifeditor.cxx 4242
"uset", //252 kifeditor.cxx 4243
"iset", //253 kifeditor.cxx 4244
"fset", //254 kifeditor.cxx 4245
"fvector", //255 kifeditor.cxx 4247
"svector", //256 kifeditor.cxx 4248
"uvector", //257 kifeditor.cxx 4249
"ivector", //258 kifeditor.cxx 4250
"bvector", //259 kifeditor.cxx 4251
"map", //260 kifeditor.cxx 4253
"maps", //261 kifeditor.cxx 4255
"mapf", //262 kifeditor.cxx 4256
"mapi", //263 kifeditor.cxx 4257
"fmap", //264 kifeditor.cxx 4259
"imap", //265 kifeditor.cxx 4260
"smap", //266 kifeditor.cxx 4261
"sfmap", //267 kifeditor.cxx 4263
"simap", //268 kifeditor.cxx 4264
"ssmap", //269 kifeditor.cxx 4265
"ismap", //270 kifeditor.cxx 4267
"ifmap", //271 kifeditor.cxx 4268
"iimap", //272 kifeditor.cxx 4269
"fsmap", //273 kifeditor.cxx 4271
"ffmap", //274 kifeditor.cxx 4272
"fimap", //275 kifeditor.cxx 4273
"treemap", //276 kifeditor.cxx 4275
"treemaps", //277 kifeditor.cxx 4277
"treemapf", //278 kifeditor.cxx 4278
"treemapi", //279 kifeditor.cxx 4279
"ftreemap", //280 kifeditor.cxx 4281
"itreemap", //281 kifeditor.cxx 4282
"streemap", //282 kifeditor.cxx 4283
"sftreemap", //283 kifeditor.cxx 4285
"sitreemap", //284 kifeditor.cxx 4286
"sstreemap", //285 kifeditor.cxx 4287
"istreemap", //286 kifeditor.cxx 4289
"iftreemap", //287 kifeditor.cxx 4290
"iitreemap", //288 kifeditor.cxx 4291
"fstreemap", //289 kifeditor.cxx 4293
"fftreemap", //290 kifeditor.cxx 4294
"fitreemap", //291 kifeditor.cxx 4295
"primemap", //292 kifeditor.cxx 4297
"primemaps", //293 kifeditor.cxx 4299
"primemapf", //294 kifeditor.cxx 4300
"primemapi", //295 kifeditor.cxx 4301
"fprimemap", //296 kifeditor.cxx 4303
"iprimemap", //297 kifeditor.cxx 4304
"sprimemap", //298 kifeditor.cxx 4305
"sfprimemap", //299 kifeditor.cxx 4307
"siprimemap", //300 kifeditor.cxx 4308
"ssprimemap", //301 kifeditor.cxx 4309
"isprimemap", //302 kifeditor.cxx 4311
"ifprimemap", //303 kifeditor.cxx 4312
"iiprimemap", //304 kifeditor.cxx 4313
"fsprimemap", //305 kifeditor.cxx 4315
"ffprimemap", //306 kifeditor.cxx 4316
"fiprimemap", //307 kifeditor.cxx 4317
"xml", //308 kifeditor.cxx 4319
"case", //309 kifeditor.cxx 4321
"let", //310 kifeditor.cxx 4322
"in", //311 kifeditor.cxx 4323
"into", //312 kifeditor.cxx 4324
"on", //313 kifeditor.cxx 4325
"default", //314 kifeditor.cxx 4326
"curl", //315 kifeditor.cxx 4327
"and", //316 kifeditor.cxx 4328
"or", //317 kifeditor.cxx 4329
"with", //318 kifeditor.cxx 4332
"_endl", //319 kifeditor.cxx 4333
"a_first", //320 kifeditor.cxx 4334
"a_last", //321 kifeditor.cxx 4335
"a_prefix", //322 kifeditor.cxx 4336
"a_change", //323 kifeditor.cxx 4337
"a_insert", //324 kifeditor.cxx 4338
"a_delete", //325 kifeditor.cxx 4339
"a_switch", //326 kifeditor.cxx 4340
"a_nocase", //327 kifeditor.cxx 4341
"a_surface", //328 kifeditor.cxx 4342
"a_full", //329 kifeditor.cxx 4343
"a_split", //330 kifeditor.cxx 4344
"a_skip", //331 kifeditor.cxx 4345
"a_track", //332 kifeditor.cxx 4346
"pred_none", //333 kifeditor.cxx 4347
"pred_weight", //334 kifeditor.cxx 4348
"pred_weightstack", //335 kifeditor.cxx 4349
"automaton", //336 kifeditor.cxx 4350
"mapff", //337 kifeditor.cxx 4352
"primemapff", //338 kifeditor.cxx 4353
"treemapff", //339 kifeditor.cxx 4354
"mapfi", //340 kifeditor.cxx 4355
"primemapfi", //341 kifeditor.cxx 4356
"treemapfi", //342 kifeditor.cxx 4357
"mapfs", //343 kifeditor.cxx 4358
"primemapfs", //344 kifeditor.cxx 4359
"treemapfs", //345 kifeditor.cxx 4360
"mapif", //346 kifeditor.cxx 4361
"primemapif", //347 kifeditor.cxx 4362
"treemapif", //348 kifeditor.cxx 4363
"mapii", //349 kifeditor.cxx 4364
"primemapii", //350 kifeditor.cxx 4365
"treemapii", //351 kifeditor.cxx 4366
"mapis", //352 kifeditor.cxx 4367
"primemapis", //353 kifeditor.cxx 4368
"treemapis", //354 kifeditor.cxx 4369
"mapsf", //355 kifeditor.cxx 4370
"primemapsf", //356 kifeditor.cxx 4371
"treemapsf", //357 kifeditor.cxx 4372
"mapsi", //358 kifeditor.cxx 4373
"primemapsi", //359 kifeditor.cxx 4374
"treemapsi", //360 kifeditor.cxx 4375
"mapss", //361 kifeditor.cxx 4376
"primemapss", //362 kifeditor.cxx 4377
"treemapss", //363 kifeditor.cxx 4378
"mapuu", //364 kifeditor.cxx 4379
"uumap", //365 kifeditor.cxx 4380
"mapui", //366 kifeditor.cxx 4381
"uimap", //367 kifeditor.cxx 4382
"mapuf", //368 kifeditor.cxx 4383
"ufmap", //369 kifeditor.cxx 4384
"mapiu", //370 kifeditor.cxx 4385
"iumap", //371 kifeditor.cxx 4386
"mapfu", //372 kifeditor.cxx 4387
"fumap", //373 kifeditor.cxx 4388
"treemapuu", //374 kifeditor.cxx 4389
"uutreemap", //375 kifeditor.cxx 4390
"treemapui", //376 kifeditor.cxx 4391
"uitreemap", //377 kifeditor.cxx 4392
"treemapuf", //378 kifeditor.cxx 4393
"uftreemap", //379 kifeditor.cxx 4394
"treemapiu", //380 kifeditor.cxx 4395
"iutreemap", //381 kifeditor.cxx 4396
"treemapfu", //382 kifeditor.cxx 4397
"futreemap", //383 kifeditor.cxx 4398
"primemapuu", //384 kifeditor.cxx 4399
"uuprimemap", //385 kifeditor.cxx 4400
"primemapui", //386 kifeditor.cxx 4401
"uiprimemap", //387 kifeditor.cxx 4402
"primemapuf", //388 kifeditor.cxx 4403
"ufprimemap", //389 kifeditor.cxx 4404
"primemapiu", //390 kifeditor.cxx 4405
"iuprimemap", //391 kifeditor.cxx 4406
"primemapfu", //392 kifeditor.cxx 4407
"fuprimemap", //393 kifeditor.cxx 4408
"binmapi", //394 kifeditor.cxx 4409
"binmapii", //395 kifeditor.cxx 4410
"binmapif", //396 kifeditor.cxx 4411
"binmapis", //397 kifeditor.cxx 4412
"binmapiu", //398 kifeditor.cxx 4413
"iibinmap", //399 kifeditor.cxx 4414
"ifbinmap", //400 kifeditor.cxx 4415
"isbinmap", //401 kifeditor.cxx 4416
"iubinmap", //402 kifeditor.cxx 4417
"KIF", //403 kifeditor.cxx 4422
""};


Exported const char* KIFSYSSTRINGS[]={
"w", //0 kifsys.cxx 166
"SYS(001): Cannot open output file", //1 kifsys.cxx 168
"r", //2 kifsys.cxx 175
"size", //3 kifsys.cxx 274
"date", //4 kifsys.cxx 275
"access", //5 kifsys.cxx 276
"change", //6 kifsys.cxx 277
"pathname", //7 kifsys.cxx 278
"directory", //8 kifsys.cxx 280
"name", //9 kifsys.cxx 330
"Incompatible version of KiF", //10 kifsys.cxx 406
"command", //11 kifsys.cxx 433
"command(string com,string outputfile): execute the command 'com'. outputfile is optional and can be used to redirect output to a file.", //12 kifsys.cxx 433
"isdirectory", //13 kifsys.cxx 434
"isdirectory(string path): return 'true' if path is a directory", //14 kifsys.cxx 434
"createdirectory", //15 kifsys.cxx 435
"createdirectory(string path): create the directory of path 'path'", //16 kifsys.cxx 435
"mkdir", //17 kifsys.cxx 436
"mkdir(string path): create the directory of path 'path'", //18 kifsys.cxx 436
"listdirectory", //19 kifsys.cxx 437
"listdirectory(string path): return the content of the directory 'path'", //20 kifsys.cxx 437
"ls", //21 kifsys.cxx 438
"ls(string path): return the content of the directory 'path'", //22 kifsys.cxx 438
"realpath", //23 kifsys.cxx 439
"realpath(path relpath): return the actual path matching 'realpath'", //24 kifsys.cxx 439
"fileinfo", //25 kifsys.cxx 440
"fileinfo(string pathname): returns the info of the file 'pathname'", //26 kifsys.cxx 440
"env", //27 kifsys.cxx 441
"env(string var)|env(string var,string val): return or set the content of the environment variable 'var'", //28 kifsys.cxx 441
""};


Exported const char* KIFCURLSTRINGS[]={
"with", //0 kifcurl.cxx 320
"URL(031): Unknown option", //1 kifcurl.cxx 441
"w", //2 kifcurl.cxx 492
"URL(009): Wrong filename", //3 kifcurl.cxx 494
"name", //4 kifcurl.cxx 539
"size", //5 kifcurl.cxx 560
"Incompatible version of KiF", //6 kifcurl.cxx 623
"curl", //7 kifcurl.cxx 629
"proxy", //8 kifcurl.cxx 655
"proxy(string prox): Proxy connection", //9 kifcurl.cxx 655
"password", //10 kifcurl.cxx 656
"password(string user,string pwd): Provide the user password", //11 kifcurl.cxx 656
"url", //12 kifcurl.cxx 657
"url(string path): Load a URL\rurl(string path,string filename): Load a url into a filename", //13 kifcurl.cxx 657
"execute", //14 kifcurl.cxx 658
"execute(string filename): Execute a curl action, filename is optional.", //15 kifcurl.cxx 658
"options", //16 kifcurl.cxx 659
"options(string option,string parameter): Set the options of the curl object", //17 kifcurl.cxx 659
""};


Exported const char* KIFSQLITESTRINGS[]={
"SQLite(800): A database has already been opened with this object", //0 kifsqlite.cxx 188
"SQLite(801): Error opening database='", //1 kifsqlite.cxx 195
"SQLite(802): Cannot close this database", //2 kifsqlite.cxx 206
"SQLite(803): Cannot use this database", //3 kifsqlite.cxx 214
"SQLite(811): Execute error=", //4 kifsqlite.cxx 227
"SQLite(805): Create error=", //5 kifsqlite.cxx 271
"SQLite(810): This database has not been opened yet...", //6 kifsqlite.cxx 283
"SQLite(830): Error while creating the persistence table: ", //7 kifsqlite.cxx 312
"SQLite(837): Error while storing a predicate: ", //8 kifsqlite.cxx 346
"SQLite(839): Error while deleting a predicate: ", //9 kifsqlite.cxx 390
"SQLite(837): Error while testing a predicate: ", //10 kifsqlite.cxx 436
"SQLite(838): Error while reading a predicate: ", //11 kifsqlite.cxx 482
"SQLite(808): Missing parameters in 'INSERTBIND'", //12 kifsqlite.cxx 493
"SQLite(897): Unknown table", //13 kifsqlite.cxx 505
"type", //14 kifsqlite.cxx 512
"SQLite(899): Wrong value list", //15 kifsqlite.cxx 531
"SQLite(893): You need to do a BEGIN first", //16 kifsqlite.cxx 536
"SQLite(895): Wrong value for:", //17 kifsqlite.cxx 554
"SQLite(812): Transaction failed!", //18 kifsqlite.cxx 566
"SQLite(805): 'BEGIN' error=", //19 kifsqlite.cxx 593
"SQLite(807): 'COMMIT' error=", //20 kifsqlite.cxx 615
"SQLite(808): Missing parameters in 'INSERT'", //21 kifsqlite.cxx 634
"name", //22 kifsqlite.cxx 638
"age", //23 kifsqlite.cxx 638
"SQLite(809): Create error=", //24 kifsqlite.cxx 666
"Wrong statement: ", //25 kifsqlite.cxx 858
"Incompatible version of KiF", //26 kifsqlite.cxx 909
"SQLite is a trademark of Hipp, Wyrick & Company, Inc.", //27 kifsqlite.cxx 913
"open", //28 kifsqlite.cxx 938
"open(string pathname): open a database", //29 kifsqlite.cxx 938
"predicate", //30 kifsqlite.cxx 939
"predicate(string name,int arity): declare a predicate of arity 'arity' in the database", //31 kifsqlite.cxx 939
"%readpredicate", //32 kifsqlite.cxx 940
"readpredicate(): read a predicate from the database", //33 kifsqlite.cxx 940
"%countpredicate", //34 kifsqlite.cxx 941
"countpredicate(): test if a predicate is in the database", //35 kifsqlite.cxx 941
"%storepredicate", //36 kifsqlite.cxx 942
"storepredicate(): store a predicate in the database", //37 kifsqlite.cxx 942
"%removepredicate", //38 kifsqlite.cxx 943
"removepredicate(): remove a predicate from the database", //39 kifsqlite.cxx 943
"close", //40 kifsqlite.cxx 945
"close(): close a database", //41 kifsqlite.cxx 945
"create", //42 kifsqlite.cxx 946
"create(x1,x2,x3): create a table in a database, with the arguments x1,x2,x3.\rEx. mydb.create('table1','nom TEXT PRIMARY KEY','age INTEGER');", //43 kifsqlite.cxx 946
"insert", //44 kifsqlite.cxx 947
"insert(string table,string column,int value,...): insert a line in a table.\rEx. mydb.insert('table1','nom',nm,'age',i);", //45 kifsqlite.cxx 947
"insertbind", //46 kifsqlite.cxx 948
"insertbind(string table,int v1,...): insert a line in a table through a begin/commit mode.\rEx. mydb.insert('table1',nm,i);", //47 kifsqlite.cxx 948
"run", //48 kifsqlite.cxx 949
"run(string sqlcommand): execute a sql command. If the input variable is a vector, then all possible values will be stored in it. It the input variable is an iterator, then it is possible to iterate on the results of the sql command. Each result is a map, where each key is a column name.", //49 kifsqlite.cxx 949
"execute", //50 kifsqlite.cxx 950
"execute(string sqlcommand): execute a raw sql command.", //51 kifsqlite.cxx 950
"begin", //52 kifsqlite.cxx 951
"begin(string mode): to enter the commit mode: default mode is DEFERRED, other modes are: IMMEDIATE and EXCLUSIVE", //53 kifsqlite.cxx 951
"commit", //54 kifsqlite.cxx 952
"commit(): the SQL command are then processed. It should finish a series of commands initiated with a begin", //55 kifsqlite.cxx 952
"memory", //56 kifsqlite.cxx 953
"memory(): return how much memory SQLite uses", //57 kifsqlite.cxx 953
""};


Exported const char* KIFXMLSTRINGS[]={
"XML(234): Already created", //0 kifxml.cxx 210
"XML(102): Unknown file", //1 kifxml.cxx 256
"vector", //2 kifxml.cxx 265
"map", //3 kifxml.cxx 276
"XML(013): Expecting a variable", //4 kifxml.cxx 347
"XML(106): Not an XML buffer", //5 kifxml.cxx 447
"XML(097): Empty node", //6 kifxml.cxx 506
"XML(123): Expecting a map", //7 kifxml.cxx 524
"XML(235): Empty doc", //8 kifxml.cxx 573
"XML(099): No document", //9 kifxml.cxx 679
"Incompatible version of KiF", //10 kifxml.cxx 696
"xml", //11 kifxml.cxx 701
"xmldoc", //12 kifxml.cxx 704
"load", //13 kifxml.cxx 727
"load(string filename): load an xml file", //14 kifxml.cxx 727
"onclosing", //15 kifxml.cxx 728
"onclosing(function,object): function to call on closing markup tag", //16 kifxml.cxx 728
"parse", //17 kifxml.cxx 729
"parse(string buffer): parse an xml buffer", //18 kifxml.cxx 729
"create", //19 kifxml.cxx 730
"create(string topnode): create an XML file with topnode as the top node or with topnode, if topdenode is a full XML structure", //20 kifxml.cxx 730
"xpath", //21 kifxml.cxx 731
"xpath(string myxpath): Return a vector of xml nodes matching myxpath", //22 kifxml.cxx 731
"save", //23 kifxml.cxx 732
"save(string filename,string encoding): save an XML file. Encoding default is 'utf8'", //24 kifxml.cxx 732
"close", //25 kifxml.cxx 733
"close(): close an xml file", //26 kifxml.cxx 733
"node", //27 kifxml.cxx 734
"node(): return the top node of the file", //28 kifxml.cxx 734
"xmlstring", //29 kifxml.cxx 735
"xmlstring(): Return an XML document as a string", //30 kifxml.cxx 735
"serialize", //31 kifxml.cxx 736
"serialize(object): Serialize as an XML document any KiF object", //32 kifxml.cxx 736
"serializestring", //33 kifxml.cxx 737
"serializestring(object): Serialize as an XML document any KiF object and return the corresponding string", //34 kifxml.cxx 737
"root", //35 kifxml.cxx 739
"root(): return the root node", //36 kifxml.cxx 739
"document", //37 kifxml.cxx 740
"document(): return the xmldoc associated with this node", //38 kifxml.cxx 740
"next", //39 kifxml.cxx 741
"next(): next xml node", //40 kifxml.cxx 741
"unlink", //41 kifxml.cxx 742
"unlink(): remove a node from a tree structure", //42 kifxml.cxx 742
"delete", //43 kifxml.cxx 743
"delete(): delete the internal representation of the XML node", //44 kifxml.cxx 743
"previous", //45 kifxml.cxx 744
"previous(): previous xml node", //46 kifxml.cxx 744
"parent", //47 kifxml.cxx 745
"parent(): parent xml node", //48 kifxml.cxx 745
"child", //49 kifxml.cxx 746
"child(): child xml node", //50 kifxml.cxx 746
"name", //51 kifxml.cxx 747
"name(): name node", //52 kifxml.cxx 747
"line", //53 kifxml.cxx 748
"line(): return the line position of the node", //54 kifxml.cxx 748
"id", //55 kifxml.cxx 749
"id(): return the node id (only available with callback functions)", //56 kifxml.cxx 749
"xmltype", //57 kifxml.cxx 750
"xmltype(): return the XML node type", //58 kifxml.cxx 750
"properties", //59 kifxml.cxx 751
"properties(): return the xml node properties", //60 kifxml.cxx 751
"content", //61 kifxml.cxx 752
"content(): return the text content of an xml node", //62 kifxml.cxx 752
"namespace", //63 kifxml.cxx 753
"namespace(): return the namespace vector of an xml node", //64 kifxml.cxx 753
"new", //65 kifxml.cxx 754
"new(string name): Create a new XML node", //66 kifxml.cxx 754
"xmlstring(): Return the whole tree from the current XML node as a string", //67 kifxml.cxx 755
""};


Exported const char* KIFREGEXSTRINGS[]={
"RGX(001):Wrong regular expression: ", //0 kifregex.cxx 68
"RGX(002):Wrong regular expression: ", //1 kifregex.cxx 132
"RGX(003):Wrong regular expression: ", //2 kifregex.cxx 207
"Incompatible version of KiF", //3 kifregex.cxx 249
"splitrgx", //4 kifregex.cxx 262
"replacergx", //5 kifregex.cxx 263
""};


Exported const char* KIFMPSTRING[]={
"with", //0 kifsound.cxx 145
"SND(001): Unknown file", //1 kifsound.cxx 178
"SND(005): Error during loading", //2 kifsound.cxx 246
"SND(003): Please load a file first", //3 kifsound.cxx 255
"Incompatible version of KiF", //4 kifsound.cxx 369
"mp3", //5 kifsound.cxx 375
"load", //6 kifsound.cxx 398
"load(string pathname): Load the sound pathname.", //7 kifsound.cxx 398
"play", //8 kifsound.cxx 399
"play(): play the sound", //9 kifsound.cxx 399
"stop", //10 kifsound.cxx 400
"stop(): stop the sound", //11 kifsound.cxx 400
""};


Exported const char* KIFSOUNDSTRINGS[]={
"SND(003): Loading error", //0 kifaosound.cxx 202
"SND(013): Unknown format", //1 kifaosound.cxx 205
"SND(004): Cannot open mp3 audio", //2 kifaosound.cxx 219
"SND(004): Cannot open audio", //3 kifaosound.cxx 262
"SND(001): Already loaded", //4 kifaosound.cxx 315
"SND(002): Load your file first", //5 kifaosound.cxx 341
"SND(005): Erreur while playing sound", //6 kifaosound.cxx 377
"SND(011): Please open the audio first", //7 kifaosound.cxx 387
"SND(010): The argument should be an ivector", //8 kifaosound.cxx 392
"channels", //9 kifaosound.cxx 496
"rate", //10 kifaosound.cxx 498
"byte_format", //11 kifaosound.cxx 500
"mp3", //12 kifaosound.cxx 502
"bits", //13 kifaosound.cxx 504
"size", //14 kifaosound.cxx 506
"SND(008): The argument shoud be a map", //15 kifaosound.cxx 520
"Incompatible version of KiF", //16 kifaosound.cxx 690
"sound", //17 kifaosound.cxx 696
"load", //18 kifaosound.cxx 720
"load(string pathname): Load a sound file", //19 kifaosound.cxx 720
"play", //20 kifaosound.cxx 721
"play(bool rst): play the sound. If rst is true, start back to beginning", //21 kifaosound.cxx 721
"stop", //22 kifaosound.cxx 722
"stop(): stop the sound", //23 kifaosound.cxx 722
"parameters", //24 kifaosound.cxx 723
"parameters(): Return the sound configuration values as a simap. Parameters rate and channels can be modified", //25 kifaosound.cxx 723
"clear", //26 kifaosound.cxx 724
"clear(): Reset the audio channel", //27 kifaosound.cxx 724
"open", //28 kifaosound.cxx 725
"open(simap p): Open a sound channel with parameters p (server side)", //29 kifaosound.cxx 725
"close", //30 kifaosound.cxx 726
"close(): close the sound channel (server side)", //31 kifaosound.cxx 726
"decode", //32 kifaosound.cxx 727
"decode(ivector): decode a sound channel, fill in an ivector (client side)", //33 kifaosound.cxx 727
"encode", //34 kifaosound.cxx 728
"encode(ivector): play a sound channel from a decoding sessions (server side)", //35 kifaosound.cxx 728
""};


Exported const char* KIFXMLSTRINGSINCLUDE[]={
"XML(097): Empty node", //0 kifxml.h 108
"XML(098): Expecting an XML node", //1 kifxml.h 131
"name", //2 kifxml.h 328
"size", //3 kifxml.h 349
"with", //4 kifxml.h 446
"XML(234): Already created", //5 kifxml.h 576
"XML(235): Empty doc", //6 kifxml.h 597
""};


Exported const char* KIFTHEINFOSTRINGS[]={
"byteposition(","- byteposition(int i): return the position in byte from a character position",
"dimension(","- dimension(): return the matrix size.\ndimension(int rowsize,int columnsize): set the matrix size.",
"determinant(","- determinant(): return the matrix determinant",
"boundaries(","- boundaries(): return the matrix boundaries",
"ontime(","- ontime(function,t,object): Set a time out function",
"get(","- get(int num): get one character from a socket in a non KiF environment. On the server side, 'num' is the value returned by 'wait()'. Use 'get()' on the client side\n- get(string msg): Pop up window to get a value from the user",
"endloop(","- endloop(): End drawing a closed sequence of lines.",
"occupation(","- occupation(bool row): return the matrix memory occupation as a vector",
"rowkey(","- rowkey(int r): return the key matching r or the vector of all row keys",
"find(","- find(string s,int i,bool matchcase): find the position of a substring from i\n- find(string path): Return the element matching the path.",
"sizerange(","- sizerange(int minw,int minh, int maxw,int maxh): define range in which the size of the window can evolve",
"row(","- row(x): return the value on row x as a map or as a mvector\n- row()|(int nb): Define the number of rows",
"replaceall(","- replaceall(string s,string sub,bool matchcase): replace all occurrences of s with sub according to matchcase",
"alert(","- alert(string msg): Pop up window to display an alert",
"value(","- value(float): define the value for the wcounter or return its value\n- value(float): define the value for the progress bar or return its value\n- value(int): define the value for the slider or return its value\n- value(): return the value of the current button\n- value()|(string v): return the input buffer or set the initial buffer\n- value()|(string v): return the text in the editor or initialize the editor\n- value(string s): set the output buffer\n- value(int s): set the choice initialization value\n- value(): Return the selected file\n- value(): return the current selected value\n- value(object): Associate the item with a value or return that value.",
"transposed(","- transposed(): return the transposed matrix",
"modal(","- modal(bool b): If true make the window modal. If no parameter return if the window is modal",
"column(","- column(x): return the value on column x as a map or as a mvector\n- column()|(int nb): Define the number of columns",
"itemsize(","- itemsize(int c): Set or return the item font size.",
"colkey(","- colkey(int c): return the key matching c or the vector of all column keys",
"duplicate(","- duplicate(): return a duplication of the matrix",
"rectanglefill(","- rectanglefill(int x,int y,int w, int h, string c|int c): Fill a rectangle with optional color c",
"LUdecomp(","- LUdecomp(vector permutations,bool tosolve): Replaces the current matrix with the LU decomposition of a rowwise permutation of itself. tosolve should be used if the next instruction is LUsolve",
"LUsolve(","- LUsolve(vector permutations,vector values): solve linear equations based on values content. Return the result in values...",
"parse(","- parse(string buffer): parse an xml buffer",
"line(","- line(int x,int y,int x1, int y1,int x2, int y2): Draw a line between points, x2 and y2 are optional\n- line(int l): return the current line position of the caret or return the line corresponding to a position\n- line(): return the line position of the node",
"cursorchar(","- cursorchar(): return the current cursor position of the caret as characters",
"blocking(","- blocking(bool flag): if 'flag' is true, the socket works in 'blocking' mode, otherwise in 'non blocking' mode",
"textsize(","- textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels",
"invert(","- invert(): Return the inverted matrix.",
"LUbanddecomp(","- LUbanddecomp(bool local,vector res): return a LU decomposition of a band matrix",
"LUbandsolver(","- LUbandsolver(bool local,vector res): return a LU factorization of a band matrix",
"drawcolor(","- drawcolor(int c|string c): set the color for the next drawings",
"min(","- min(): return the min value of the matrix\n- min(): return the min value of the sparse vector",
"close(","- close(int num): Close a socket. On the server side, if 'num' is provided (it is the value returned by wait()) it closes the client socket, otherwise it closes the current socket.\n- close(): close the window\n- close(): Close the file browser\n- close(string path): Close the element.\rclose(wtreeitem e): Close the element.\n- close(): close a database\n- close(): close an xml file\n- close(): close the sound channel (server side)",
"max(","- max(): return the max value of the matrix\n- max(): return the max value of the sparse vector",
"drawtext(","- drawtext(string l,int x,int y): Put a text at position x,y",
"run(","- run(int num,string stop): Only on the server side: execute remote method invocations for the client 'num'. 'stop' is the string that can be used to stop the server by the client.\n- run(): Launch the GUI\n- run(string sqlcommand): execute a sql command. If the input variable is a vector, then all possible values will be stored in it. It the input variable is an iterator, then it is possible to iterate on the results of the sql command. Each result is a map, where each key is a column name.",
"sum(","- sum(): return the sum of all elements",
"vertex(","- vertex(float x,float y): Add a vertex to a complex structure",
"connectorstyle(","- connectorstyle(int s): Set or return the connector style.",
"product(","- product(): return the product of all elements",
"getpeername(","- getpeername(int num): return the current peer name on the socket 'num'",
"index(","- index(): return the column or the row current index",
"circle(","- circle(int x,int y,int r,int color): Draw a circle. 'color' is optional.",
"create(","- create(int x,int y,int w, int h,string title): Create a window without widgets, w and h are optional\n- create(int x,int y,int w,int h,string label): Create a scroll\n- create(int x,int y,int w,int h,string label): Create a wcounter\n- create(int x,int y,int w,int h,string label): Create a progress bar\n- create(int x,int y,int w,int h,int alignment,bool valueslider,string label): Create a slider or a valueslider\n- create(int x,int y,int w,int h,string label): Create a box\n- create(int x,int y,int w,int h,string type,string shape,string label): Create a button. If type is not provided then it is: FL_Regular.\n- create(int x,int y,int w,int h,string label): Create a table of objects, and starts adding\n- create(int x,int y,int w,int h,boolean multiline,string label): Create an input\n- create(int x,int y,int w,int h,string label): Create an editor\n- create(int x,int y,int w,int h,boolean multiline,string label): Create an output\n- create(int x,int y,int w,int h,string label): Create an choice\n- create(string intialdirectory,string filter,bool directory,string label): Open a file browser, select directories if directory is true\n- create(x,y,w,h,label): Create a browser\n- create(int x,int y,int h,int w,string label): Create a tree\n- create(x1,x2,x3): create a table in a database, with the arguments x1,x2,x3.\rEx. mydb.create('table1','nom TEXT PRIMARY KEY','age INTEGER');\n- create(string topnode): create an XML file with topnode as the top node or with topnode, if topdenode is a full XML structure",
"createserver(","- createserver(string hostname,int port,int nblients: create a server, if hostname is omitted, then the local hostname is used",
"read(","- read(int num): read a string on a socket. On the server side, 'num' is the value returned by 'wait()'. Use 'read()' on the client side",
"connect(","- connect(string hostname,int port): connect to the server",
"lineshape(","- lineshape(string type,int width): Select the line shape and its thikness",
"send(","- send(int num,string s): write the string s on the socket in a non KiF environment. On the server side, num is the value returned by wait()'. Use 'send(string s)' on the client side",
"name(","- name(): name node",
"wait(","- wait(): wait for a client to connect and returns its socket id",
"children(","- children(): Return number of children.",
"onclose(","- onclose(function,object): Callback to call on close",
"write(","- write(int num,string s): write the string s on the socket. On the server side, num is the value returned by wait()'. Use 'write(string s)' on the client side",
"receive(","- receive(int num,int nb): read a string on a socket in a non KiF environment. On the server side, 'num' is the value returned by 'wait()'. Use 'receive()' on the client side",
"settimeout(","- settimeout(int t): Set a time out of 't' seconds on the socket",
"gethostname(","- gethostname(): return the current host name",
"memory(","- memory(): return how much memory SQLite uses",
"getfont(","- getfont(int num): get font name.",
"getfunction(","- getfunction(string frame): return a function handle declared on the server side",
"port(","- port(): return the current port number",
"initializefonts(","- initializefonts(): load fonts from system. Return the number of available fonts",
"getframe(","- getframe(string frame): return a frame object handle declared on the server side",
"begin(","- begin(int x,int y,int w, int h,string title): Create a window and begin initialisation, w and h are optional\n- begin(int x,int y,int w, int h,string title): Create a tab window and begin initialisation\n- begin(int x,int y,int w, int h,string title): Create a group within a tab and begin initialisation\n- begin(string mode): to enter the commit mode: default mode is DEFERRED, other modes are: IMMEDIATE and EXCLUSIVE",
"border(","- border(bool b): If true add or remove borders. If no parameter return if the window has borders",
"end(","- end(): end creation\n- end(): end tab creation\n- end(): end group creation",
"isselected(","- isselected(): Return true if element is selected.",
"menu(","- menu(vector,int x,int y,int w, int h): initialize a menu with its callback functions\n- menu(vector s): Initialize the menu",
"getfontsizes(","- getfontsizes(int num): return a vector of available font sizes.",
"backgroundcolor(","- backgroundcolor(int c|string c): set the background color",
"resizable(","- resizable(object): make the object resizable",
"fontnumber(","- fontnumber(): Return the number of available fonts.",
"clicked(","- clicked(): Return the element that was clicked.",
"focus(","- focus(): Get the focus",
"polygon(","- polygon(int x,int y,int x1, int y1,int x2, int y2, int x3, int y3): Draw a polygon, x3 and y3 are optional",
"plot(","- plot(fvector xy,int thickness,fvector landmarks): Plot a graph from a table of successive x,y points according to window size. If thickness===0 then points are continuously plotted, else defines the diameter of the point. Return a float vector which is used with plotcoords. The landmark vector is optional, it is has the following structure: [XmaxWindow,YmaxWindow,XminValue,YminValue,XmaxValue,YmaxValue,incX,incY]. incX,incY are also optional.",
"plotcoords(","- plotcoords(fvector inits,float x,float y): Compute the coordinates of a point(x,y) according to the previous scale computed with plot. Returns a vector of two elements [xs,ys] corresponding to the screen coordinates in the current window.",
"ask(","- ask(string msg,string buttonmsg1,string buttonmsg2,string buttonmsg3): Pop up window to pose a question",
"image(","- image(image image,int x, int y, int w, int h): Display an image\n- image(image im,string label,int labelalign): Use the image as a button image",
"password(","- password(string msg): Pop up window to input a password\n- password(string user,string pwd): Provide the user password",
"rectangle(","- rectangle(int x,int y,int w, int h, string c|int c): Draw a rectangle with optional color c",
"loop(","- loop(int x,int y,int x1, int y1,int x2, int y2, int x3, int y3): Draw a series of lines, x3 and y3 are optional",
"formatchar(","- formatchar(): Return the format char.\rformatchar(string): Set the format char",
"deselect(","- deselect(): Deselect all items\rdeselect(int i): Deselect item i",
"pushmatrix(","- pushmatrix(): Save the current transformation",
"label(","- label(string s): set or return the label text\n- label(): Return the item label.",
"popmatrix(","- popmatrix(): Restore the current transformation",
"unlock(","- unlock(): Thread unlock",
"scale(","- scale(float x,float y)|scale(float x): Scale the current transformation",
"getstyle(","- getstyle(int start,int end): return a vector of style for each character of the text section",
"translate(","- translate(float x,float y): translate the current transformation",
"rotate(","- rotate(float d): rotate of degree d the current transformation",
"multmatrix(","- multmatrix(float a, float b, float c, float d, float x, float y): combine transformations",
"transformx(","- transformx(float x, float y): Transform a coordinate X using the current transformation matrix.",
"loadgif(","- loadgif(string filename): Load a GIF image",
"transformy(","- transformy(float x, float y): Transform a coordinate Y using the current transformation matrix.",
"transformdx(","- transformdx(float x, float y): Transform a distance DX using the current transformation matrix.",
"transformdy(","- transformdy(float x, float y): Transform a distance DY using the current transformation matrix.",
"cell(","- cell(int R,int C): Return the value of the cell on row R and column C",
"transformedvertex(","- transformedvertex(float x, float y): add transformations to vertices list.",
"beginpoints(","- beginpoints(): Start accumulating vertices",
"endpoints(","- endpoints(): Stop accumulating vertices",
"linecharbounds(","- linecharbounds(): return the beginning and the end at the current line position of the caret or return the line boundaries corresponding to a position as characters",
"beginline(","- beginline(): Start drawing lines.",
"endline(","- endline(): End drawing lines.",
"xpath(","- xpath(string myxpath): Return a vector of xml nodes matching myxpath",
"beginloop(","- beginloop(): Start drawing a closed sequence of lines.",
"beginpolygon(","- beginpolygon(): Start drawing a convex filled polygon.",
"hide(","- hide(bool h): Hide the window if h is true\n- hide(): Hide the widget",
"endpolygon(","- endpolygon(): End drawing a convex filled polygon.",
"begincomplexpolygon(","- begincomplexpolygon(): Start drawing a complex filled polygon.",
"gap(","- gap(): Fill the gap while drawing a complex polygon.",
"gotoline(","- gotoline(int l,bool highlight): Goto line l. If true, highlight the line",
"labelsize(","- labelsize(int c): set or return the label font size",
"endcomplexpolygon(","- endcomplexpolygon(): End drawing a complex filled polygon.",
"arc(","- arc(int x,int y,int x1, int y1, float a1, float a2): Draw an arc.\rarc(float x,float y,float rad,float a1,float a2): Add a series of points to the current path on the arc of a circle;",
"curve(","- curve(float x,float y,float x1,float y1,float x1,float y2,float x3,float y3): Add a series of points on a Bezier curve to the path. The curve ends (and two of the points) are at x,y and x3,y3.",
"pushclip(","- pushclip(int x,int y,int w, int h): Insert a clip region, with the following coordinates",
"popclip(","- popclip(): Release a clip region",
"pie(","- pie(int x,int y,int x1, int y1, float a1, float a2): Draw a pie",
"point(","- point(int x,int y): Draw a pixel",
"redraw(","- redraw(): Redraw the window",
"textcolor(","- textcolor(string code|int code): Set the color of the text",
"rgbcolor(","- rgbcolor(string|int color)|(int r,int g,int b)|(vector color): either return a vector of the color decomposition or return the value of rgb values combined",
"font(","- font(string f,int sz): Set the font name and its size\n- font(int s): set or return the text font\n- font(string s): set or return the text font\n- font(string s)|(): set or return the text font\n- font(int c): Set or return the item font.",
"child(","- child(int i): Return the child element at position i.\n- child(): child xml node",
"ondragdrop(","- ondragdrop(function,object): Set the call back function for drag and drop action.",
"annotate(","- annotate(string s|vector v,string style,bool matchcase): Annotate each occurence of s with style (matchcase is a default argument)",
"onmouse(","- onmouse(int action, function,object): Set the call back function on a mouse action with a given object as parameter\n- onmouse(int action,function f,object o): callback when clicking",
"onkey(","- onkey(int action, function,object): Set the call back function on a keyboard action with a given object as parameter\n- onkey(int action,function f,object o): callback when a key is pressed",
"cursorstyle(","- cursorstyle(int cursortype, int color,int color): Set the cursor shape\n- cursorstyle(int cursorshape): Set the cursor shape",
"show(","- show(): Show the window\n- show(): Show the widget",
"flush(","- flush(): force redrawing of window",
"unhighlight(","- unhighlight(): unhighlight a portion of text",
"position(","- position()|(int x,int y): Return the window position or set the window position",
"createdirectory(","- createdirectory(string path): create the directory of path 'path'",
"size(","- size()|(int x,int y,int w, int h): Return the window size or set the window size",
"labelfont(","- labelfont(int c): set or return the label font",
"awake(","- awake(): Awake a threaded window",
"insertbind(","- insertbind(string table,int v1,...): insert a line in a table through a begin/commit mode.\rEx. mydb.insert('table1',nm,i);",
"mkdir(","- mkdir(string path): create the directory of path 'path'",
"lock(","- lock(): Thread lock",
"bitmap(","- bitmap(bitmap image,int color,int x, int y, int w, int h): Display a bitmap\n- bitmap(bitmap im,int color,string label,int labelalign): Use the bitmap as a button image",
"execute(","- execute(string filename): Execute a curl action, filename is optional.\n- execute(string sqlcommand): execute a raw sql command.",
"labelcolor(","- labelcolor(int c): set or return the label color",
"labeltype(","- labeltype(int c): set or return the label type",
"selectioncolor(","- selectioncolor(int color): Color for the selected elements",
"coords(","- coords(int c): return a vector of the widget coordinates",
"created(","- created(): Return true if the object has been created",
"copy(","- copy(string): Copy string to clipboard\n- copy(string s): copy selected text from editor into clipboard (string s is optional)",
"paste(","- paste(): paste string\n- paste(): paste selected text from clipboard",
"align(","- align(int): define the label alignment\n- align(int): define the slider alignement\n- align(int): define the button label alignment",
"resize(","- resize(object): make the object resizable",
"tooltip(","- tooltip(string msg): Add a tooltip message to the widget",
"bounds(","- bounds(float x,float y): defines the wcounter boundary\n- bounds(int x,int y): defines the slider boundary",
"parameters(","- parameters(): Return the sound configuration values as a simap. Parameters rate and channels can be modified",
"type(","- type(bool normal): if 'true' then normal wcounter or simple wcounter\n- type(int x): Value slider type\n- type(string|int boxtype): Define the box type",
"step(","- step(double): define the wcounter step\n- step(int): define the slider step",
"lstep(","- lstep(double): define the large wcounter step",
"steps(","- steps(double): define the wcounter steps, normal and large.",
"minimum(","- minimum(float x): defines the minimum or return the minimum",
"maximum(","- maximum(float x): defines the maximum or return the maximum",
"storepredicate(","- storepredicate(): store a predicate in the database",
"barcolor(","- barcolor(int color): Color for the progression bar",
"colorfg(","- colorfg(int c): set or return the cell color foreground",
"boxtype(","- boxtype(int x): Define the slider box type\n- boxtype(int boxtype): box type",
"when(","- when(string when1, string when2,...): Type of event for a button which triggers the callback\n- when(string when1, string when2,...): Type of event to trigger the callback",
"open(","- open(): Choose a file via a mac window\n- open(string path): Open the element.\ropen(wtreeitem e): Open the element.\n- open(string pathname): open a database\n- open(simap p): Open a sound channel with parameters p (server side)",
"shortcut(","- shortcut(string keycode): Set a shortcut to activate the button from the keyboard",
"color(","- color(string code|int code): Set the color of the button\n- color(string c|int c): set or return the text color",
"current(","- current(): Return the current active tab",
"add(","- add(wgroup g): Add dynamically a new tab\n- add(int R,int C,string v): Add a value on row R and column C\n- add(label): Add a string to the browser\n- add(string path): Add a tree item.\radd(wtreeitem e,string n): Add a tree item after e.",
"remove(","- remove(wgroup g): Remove a tab\n- remove(wtreeitem e): Remove the element e from the tree.",
"activate(","- activate(): Active this tab\n- activate(): Activate the current element.",
"loadjpeg(","- loadjpeg(string filename): Load a JPEG image",
"load(","- load(vector bitmaps,int w,int h): Load a bitmap\n- load(string f): load file into editor\n- load(string): Load a file into a mac window\n- load(filename): Load a file into the browser\n- load(string filename): load an xml file\n- load(string pathname): Load the sound pathname.\n- load(string pathname): Load a sound file",
"xmltype(","- xmltype(): return the XML node type",
"clear(","- clear(): Clear the table\n- clear(): Clear the browser from all values\n- clear(): Delete the tree items\n- clear(): Reset the audio channel",
"cellalign(","- cellalign(align): Set the inner cell alignement",
"cellalignheadercol(","- cellalignheadercol(align): Set the header column alignement",
"cellalignheaderrow(","- cellalignheaderrow(align): Set the header row alignement",
"isclosed(","- isclosed(): Return true if element is closed.\n- isclosed(string path): Return true if element is closed.\risclosed(wtreeitem e): Return true if element is closed.",
"rowheight(","- rowheight(int height): Define the row height in pixel",
"columnwidth(","- columnwidth(int width): Define the column width in pixel",
"fontsize(","- fontsize(int c): set or return the text font size\n- fontsize(int c)|(): set or return the text font size\n- fontsize(int c): Set or return the item font size.",
"colorbg(","- colorbg(int c): set or return the cell color background",
"colorselection(","- colorselection(int color): Color for the selected elements",
"rowheader(","- rowheader(int pos,string label): Set the row header label at row pos",
"columnheader(","- columnheader(int pos,string label): Set the column header label at column pos",
"rowheaderheight(","- rowheaderheight(int height): the size in pixel of the row header",
"columnheaderwidth(","- columnheaderwidth(int width): the size in pixel of the column header",
"insert(","- insert(string s,int p): insert s at position p in the input\n- insert(l,label): Insert a label before line l\n- insert(wtreeitem e,string label,int pos): Insert an element after e with label at position pos in the children list.\n- insert(string table,string column,int value,...): insert a line in a table.\rEx. mydb.insert('table1','nom',nm,'age',i);",
"selection(","- selection(): return the selected text in the input\n- selection(): return the selected text in the editor or the coordinates of the selection according to the recipient variable",
"root(","- root(): Return the root element.\n- root(): return the root node",
"word(","- word(int pos): Return the word at position pos",
"setstyle(","- setstyle(int start,int end,string style): set the style of a portion of text",
"save(","- save(string f): save buffer to file f\n- save(string filename,string encoding): save an XML file. Encoding default is 'utf8'",
"replace(","- replace(string s,string sub,int i,bool matchcase): replace s with sub according to matchcase starting at i",
"clean(","- clean(): Remove the object associated through value.",
"rfind(","- rfind(string s,int i,bool matchcase): find the position of a substring from i backward",
"highlight(","- highlight(int start,int end)|(): highlight a portion of text. Return true if the text is highlighted or the highlighted text",
"onvscroll(","- onvscroll(function f,object o): callback when scrolling vertically",
"onhscroll(","- onhscroll(function f,object o): callback when scrolling horizontally",
"addstyle(","- addstyle(map style): Initialize styles",
"delete(","- delete(): delete selected text from editor\n- delete(): delete the internal representation of the XML node",
"cut(","- cut(): cut selected text from editor into clipboard",
"isactive(","- isactive(): Return true if element is active.",
"select(","- select(): Return selected string.\rselect(int posfirst,int poslast): Select the string between posfirst and posend\n- select(): Return selected string.\rselect(int i): Return string in position i",
"linebounds(","- linebounds(int l): return the beginning and the end at the current line position of the caret or return the line boundaries corresponding to a position",
"listdirectory(","- listdirectory(string path): return the content of the directory 'path'",
"cursor(","- cursor(int l): return the current cursor position of the caret or set the cursor position",
"wrap(","- wrap(bool): Set the wrapping mode",
"annotateregexip(","- annotateregexip(string reg,string style): Annotate each string matching reg with style",
"append(","- append(string s): append s at the end of the input",
"charposition(","- charposition(int i): return the position in character from a byte position",
"ok(","- ok(): return true if ok was pressed",
"columnchar(","- columnchar(): Return the column char separator.\rcolumnchar(string): Set the column char separator",
"depth(","- depth(): Return the depth of the item.",
"bgcolor(","- bgcolor(int c): Set or return the item background color.",
"fgcolor(","- fgcolor(int c): Set or return the foreground color.",
"isopen(","- isopen(): Return true if element is open.",
"isroot(","- isroot(): Return true if element is root.",
"id(","- id(): return the node id (only available with callback functions)",
"parent(","- parent(): Return the last element.\n- parent(): parent xml node",
"next(","- next(): Return the next element.\n- next(wtreeitem e): Return the next element after e.\n- next(): next xml node",
"previous(","- previous(): Return the previous element.\n- previous(wtreeitem e): Return the previous element before e.\n- previous(): previous xml node",
"deactivate(","- deactivate(): Deactivate the current element.",
"insertabove(","- insertabove(wtreeitem e,string label): Insert an element above e with label.",
"connectorcolor(","- connectorcolor(int c): Set or return the connector color.",
"connectorwidth(","- connectorwidth(int s): Set or return the connector width.",
"sortorder(","- sortorder(int s): Set or return the sort order.",
"selectmode(","- selectmode(int s): Set or return the selection mode.",
"itembgcolor(","- itembgcolor(int c): Set or return the item background color.",
"itemfgcolor(","- itemfgcolor(int c): Set or return the foreground color.",
"itemfont(","- itemfont(int c): Set or return the item font.",
"Set or Get the amount of white space (","- Set or Get the amount of white space (in pixels) that should appear between the widget's left border and the tree's contents.",
"margintop(","- margintop(int s): Set or Get the amount of white space (in pixels) that should appear between the widget's top border and the top of the tree's contents.",
"rootlabel(","- rootlabel(string r): Set the root label.",
"first(","- first(): Return the first element.",
"last(","- last(): Return the last element.",
"command(","- command(string com,string outputfile): execute the command 'com'. outputfile is optional and can be used to redirect output to a file.",
"serializestring(","- serializestring(object): Serialize as an XML document any KiF object and return the corresponding string",
"isdirectory(","- isdirectory(string path): return 'true' if path is a directory",
"ls(","- ls(string path): return the content of the directory 'path'",
"realpath(","- realpath(path relpath): return the actual path matching 'realpath'",
"fileinfo(","- fileinfo(string pathname): returns the info of the file 'pathname'",
"env(","- env(string var)|env(string var,string val): return or set the content of the environment variable 'var'",
"proxy(","- proxy(string prox): Proxy connection",
"new(","- new(string name): Create a new XML node",
"url(","- url(string path): Load a URL\rurl(string path,string filename): Load a url into a filename",
"options(","- options(string option,string parameter): Set the options of the curl object",
"predicate(","- predicate(string name,int arity): declare a predicate of arity 'arity' in the database",
"readpredicate(","- readpredicate(): read a predicate from the database",
"countpredicate(","- countpredicate(): test if a predicate is in the database",
"removepredicate(","- removepredicate(): remove a predicate from the database",
"commit(","- commit(): the SQL command are then processed. It should finish a series of commands initiated with a begin",
"onclosing(","- onclosing(function,object): function to call on closing markup tag",
"node(","- node(): return the top node of the file",
"xmlstring(","- xmlstring(): Return an XML document as a string\n- xmlstring(): Return the whole tree from the current XML node as a string",
"serialize(","- serialize(object): Serialize as an XML document any KiF object",
"document(","- document(): return the xmldoc associated with this node",
"unlink(","- unlink(): remove a node from a tree structure",
"properties(","- properties(): return the xml node properties",
"content(","- content(): return the text content of an xml node",
"namespace(","- namespace(): return the namespace vector of an xml node",
"play(","- play(): play the sound\n- play(bool rst): play the sound. If rst is true, start back to beginning",
"stop(","- stop(): stop the sound\n- stop(): stop the sound",
"decode(","- decode(ivector): decode a sound channel, fill in an ivector (client side)",
"encode(","- encode(ivector): play a sound channel from a decoding sessions (server side)","",""};
