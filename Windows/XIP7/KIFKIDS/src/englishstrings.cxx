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
" (line:", //0 kif.cxx 988
"this", //1 kif.cxx 1087
"null", //2 kif.cxx 1087
"Instruction", //3 kif.cxx 1239
"line %d in %s", //4 kif.cxx 1245
"KIF(413): Incompatible value", //5 kif.cxx 1595
"function", //6 kif.cxx 1694
"frame", //7 kif.cxx 1703
"r", //8 kif.cxx 1812
"w", //9 kif.cxx 1828
"w+", //10 kif.cxx 1836
"true", //11 kif.cxx 3854
"false", //12 kif.cxx 3855
"KIF(421): We only accept one level in a container for a stream", //13 kif.cxx 4142
"KIF(689): Circular structure", //14 kif.cxx 5218
"KIF(777): Too many joined threads", //15 kif.cxx 5633
"STK(999): No more threads", //16 kif.cxx 5802
"KIF(281): String overflow", //17 kif.cxx 6004
"' in object of type '", //18 kif.cxx 8175
"KIF(821): Stream variable not accessible anymore", //19 kif.cxx 8334
"KIF(331): Wrong manipulation of this variable: ", //20 kif.cxx 11791
"KIF(192): Unknown external function: ", //21 kif.cxx 11815
"KIF(301): Unknown function: ", //22 kif.cxx 12269
"KIF(345): Unknown frame function: ", //23 kif.cxx 12297
"self", //24 kif.cxx 12390
"KIF(650): Expecting a function as parameter", //25 kif.cxx 12753
"KIF(431): Wrong usage of operator '::'. Two arguments at most", //26 kif.cxx 13244
"KIF(432): Wrong usage of operator '::', one element should be a vector container", //27 kif.cxx 13270
"runadthread", //28 kif.cxx 14361
"KIF(415): You cannot modify a tuple element", //29 kif.cxx 14671
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
"w", //0 kifglobalthread.cxx 201
"_current", //1 kifglobalthread.cxx 272
"SYS(010): Stream has already been redirected", //2 kifglobalthread.cxx 349
"SYS(001): Cannot open output file", //3 kifglobalthread.cxx 351
"SYS(011): Stream not opened", //4 kifglobalthread.cxx 363
"KIF(629): Unknwown type: ", //5 kifglobalthread.cxx 400
"STK(888): Stack overflow", //6 kifglobalthread.cxx 677
"(Thread:", //7 kifglobalthread.cxx 738
"at line:", //8 kifglobalthread.cxx 826
"in", //9 kifglobalthread.cxx 831
"Instruction", //10 kifglobalthread.cxx 839
"line %ld in %s", //11 kifglobalthread.cxx 846
"line %ld", //12 kifglobalthread.cxx 848
"line %d in %s", //13 kifglobalthread.cxx 964
"null", //14 kifglobalthread.cxx 1151
"this", //15 kifglobalthread.cxx 1152
"sequence", //16 kifglobalthread.cxx 1153
"catch", //17 kifglobalthread.cxx 1154
"self", //18 kifglobalthread.cxx 1155
"_initial", //19 kifglobalthread.cxx 1158
"size", //20 kifglobalthread.cxx 1159
"methods", //21 kifglobalthread.cxx 1166
"vector", //22 kifglobalthread.cxx 1167
"map", //23 kifglobalthread.cxx 1168
"apply", //24 kifglobalthread.cxx 1169
"synchronous", //25 kifglobalthread.cxx 1170
"get", //26 kifglobalthread.cxx 1171
"string", //27 kifglobalthread.cxx 1172
"int", //28 kifglobalthread.cxx 1173
"long", //29 kifglobalthread.cxx 1174
"float", //30 kifglobalthread.cxx 1175
"boolean", //31 kifglobalthread.cxx 1176
"fraction", //32 kifglobalthread.cxx 1177
"true", //33 kifglobalthread.cxx 1193
"false", //34 kifglobalthread.cxx 1194
"_paths", //35 kifglobalthread.cxx 1196
"_KIFMAIN", //36 kifglobalthread.cxx 1199
"_loader", //37 kifglobalthread.cxx 1201
"pop", //38 kifglobalthread.cxx 1202
"stdin", //39 kifglobalthread.cxx 1207
"for", //40 kifglobalthread.cxx 1209
"while", //41 kifglobalthread.cxx 1210
"try", //42 kifglobalthread.cxx 1211
"switch", //43 kifglobalthread.cxx 1212
"if", //44 kifglobalthread.cxx 1213
"_final", //45 kifglobalthread.cxx 1215
"_run_as_thread", //46 kifglobalthread.cxx 1216
"loadin", //47 kifglobalthread.cxx 1217
"_endl", //48 kifglobalthread.cxx 1218
"empty", //49 kifglobalthread.cxx 1219
"smap", //50 kifglobalthread.cxx 1221
"imap", //51 kifglobalthread.cxx 1222
"fmap", //52 kifglobalthread.cxx 1223
"svector", //53 kifglobalthread.cxx 1224
"ivector", //54 kifglobalthread.cxx 1225
"fvector", //55 kifglobalthread.cxx 1226
"fail", //56 kifglobalthread.cxx 1229
"!assertdb", //57 kifglobalthread.cxx 1233
"!retractdb", //58 kifglobalthread.cxx 1235
"!ponder", //59 kifglobalthread.cxx 1236
"_predicatemode", //60 kifglobalthread.cxx 1238
"short", //61 kifglobalthread.cxx 1239
"waiton", //62 kifglobalthread.cxx 1240
"trigger", //63 kifglobalthread.cxx 1241
"sthrough", //64 kifglobalthread.cxx 1243
"ithrough", //65 kifglobalthread.cxx 1244
"fthrough", //66 kifglobalthread.cxx 1245
"vthrough", //67 kifglobalthread.cxx 1246
"return", //68 kifglobalthread.cxx 1316
"predicate", //69 kifglobalthread.cxx 1339
"predicateinstance", //70 kifglobalthread.cxx 1340
"variable", //71 kifglobalthread.cxx 1342
"domain", //72 kifglobalthread.cxx 1343
"ustring", //73 kifglobalthread.cxx 1345
"tuple", //74 kifglobalthread.cxx 1346
"rawstring", //75 kifglobalthread.cxx 1347
"time", //76 kifglobalthread.cxx 1348
"automaton", //77 kifglobalthread.cxx 1349
"transducer", //78 kifglobalthread.cxx 1350
"grammar", //79 kifglobalthread.cxx 1351
"available", //80 kifglobalthread.cxx 1352
"byte", //81 kifglobalthread.cxx 1354
"bit", //82 kifglobalthread.cxx 1358
"bits", //83 kifglobalthread.cxx 1359
"date", //84 kifglobalthread.cxx 1360
"call", //85 kifglobalthread.cxx 1361
"sloop", //86 kifglobalthread.cxx 1364
"uloop", //87 kifglobalthread.cxx 1365
"bloop", //88 kifglobalthread.cxx 1366
"iloop", //89 kifglobalthread.cxx 1367
"floop", //90 kifglobalthread.cxx 1368
"mapu", //91 kifglobalthread.cxx 1376
"mapi", //92 kifglobalthread.cxx 1377
"mapf", //93 kifglobalthread.cxx 1378
"mapis", //94 kifglobalthread.cxx 1379
"mapif", //95 kifglobalthread.cxx 1380
"mapii", //96 kifglobalthread.cxx 1381
"mapfs", //97 kifglobalthread.cxx 1382
"mapff", //98 kifglobalthread.cxx 1383
"mapfi", //99 kifglobalthread.cxx 1384
"mapuu", //100 kifglobalthread.cxx 1385
"mapui", //101 kifglobalthread.cxx 1386
"mapuf", //102 kifglobalthread.cxx 1387
"mapss", //103 kifglobalthread.cxx 1388
"mapsf", //104 kifglobalthread.cxx 1389
"mapsi", //105 kifglobalthread.cxx 1390
"mapiu", //106 kifglobalthread.cxx 1392
"mapfu", //107 kifglobalthread.cxx 1393
"treemap", //108 kifglobalthread.cxx 1395
"treemapu", //109 kifglobalthread.cxx 1396
"treemapi", //110 kifglobalthread.cxx 1397
"treemapf", //111 kifglobalthread.cxx 1398
"treemapis", //112 kifglobalthread.cxx 1399
"treemapif", //113 kifglobalthread.cxx 1400
"treemapii", //114 kifglobalthread.cxx 1401
"treemapfs", //115 kifglobalthread.cxx 1402
"treemapff", //116 kifglobalthread.cxx 1403
"treemapfi", //117 kifglobalthread.cxx 1404
"treemapss", //118 kifglobalthread.cxx 1405
"treemapuu", //119 kifglobalthread.cxx 1406
"treemapui", //120 kifglobalthread.cxx 1407
"treemapuf", //121 kifglobalthread.cxx 1408
"treemapiu", //122 kifglobalthread.cxx 1410
"treemapfu", //123 kifglobalthread.cxx 1411
"treemapsf", //124 kifglobalthread.cxx 1413
"treemapsi", //125 kifglobalthread.cxx 1414
"primemap", //126 kifglobalthread.cxx 1416
"primemapu", //127 kifglobalthread.cxx 1417
"primemapi", //128 kifglobalthread.cxx 1418
"binmapi", //129 kifglobalthread.cxx 1420
"binmapii", //130 kifglobalthread.cxx 1421
"binmapif", //131 kifglobalthread.cxx 1422
"binmapis", //132 kifglobalthread.cxx 1423
"binmapiu", //133 kifglobalthread.cxx 1424
"primemapf", //134 kifglobalthread.cxx 1426
"primemapis", //135 kifglobalthread.cxx 1427
"primemapif", //136 kifglobalthread.cxx 1428
"primemapii", //137 kifglobalthread.cxx 1429
"primemapfs", //138 kifglobalthread.cxx 1430
"primemapff", //139 kifglobalthread.cxx 1431
"primemapfi", //140 kifglobalthread.cxx 1432
"primemapss", //141 kifglobalthread.cxx 1433
"primemapsf", //142 kifglobalthread.cxx 1434
"primemapsi", //143 kifglobalthread.cxx 1435
"primemapuu", //144 kifglobalthread.cxx 1437
"primemapui", //145 kifglobalthread.cxx 1438
"primemapuf", //146 kifglobalthread.cxx 1439
"primemapiu", //147 kifglobalthread.cxx 1440
"primemapfu", //148 kifglobalthread.cxx 1441
"set", //149 kifglobalthread.cxx 1444
"sset", //150 kifglobalthread.cxx 1445
"uset", //151 kifglobalthread.cxx 1446
"fset", //152 kifglobalthread.cxx 1447
"iset", //153 kifglobalthread.cxx 1448
"uvector", //154 kifglobalthread.cxx 1451
"bvector", //155 kifglobalthread.cxx 1454
"table", //156 kifglobalthread.cxx 1457
"term", //157 kifglobalthread.cxx 1459
"list", //158 kifglobalthread.cxx 1460
"iterator", //159 kifglobalthread.cxx 1461
"file", //160 kifglobalthread.cxx 1462
"wfile", //161 kifglobalthread.cxx 1463
"bool", //162 kifglobalthread.cxx 1509
"auto", //163 kifglobalthread.cxx 1511
"let", //164 kifglobalthread.cxx 1512
"maps", //165 kifglobalthread.cxx 1516
"ismap", //166 kifglobalthread.cxx 1520
"iimap", //167 kifglobalthread.cxx 1521
"ifmap", //168 kifglobalthread.cxx 1522
"fsmap", //169 kifglobalthread.cxx 1523
"fimap", //170 kifglobalthread.cxx 1524
"ffmap", //171 kifglobalthread.cxx 1525
"ssmap", //172 kifglobalthread.cxx 1530
"simap", //173 kifglobalthread.cxx 1531
"sfmap", //174 kifglobalthread.cxx 1532
"uumap", //175 kifglobalthread.cxx 1535
"uimap", //176 kifglobalthread.cxx 1536
"ufmap", //177 kifglobalthread.cxx 1537
"iumap", //178 kifglobalthread.cxx 1539
"treemaps", //179 kifglobalthread.cxx 1557
"istreemap", //180 kifglobalthread.cxx 1562
"iitreemap", //181 kifglobalthread.cxx 1563
"iftreemap", //182 kifglobalthread.cxx 1564
"fstreemap", //183 kifglobalthread.cxx 1565
"fitreemap", //184 kifglobalthread.cxx 1566
"fftreemap", //185 kifglobalthread.cxx 1567
"streemap", //186 kifglobalthread.cxx 1569
"itreemap", //187 kifglobalthread.cxx 1570
"ftreemap", //188 kifglobalthread.cxx 1571
"sstreemap", //189 kifglobalthread.cxx 1572
"sitreemap", //190 kifglobalthread.cxx 1573
"sftreemap", //191 kifglobalthread.cxx 1574
"uutreemap", //192 kifglobalthread.cxx 1575
"uitreemap", //193 kifglobalthread.cxx 1576
"uftreemap", //194 kifglobalthread.cxx 1577
"iutreemap", //195 kifglobalthread.cxx 1579
"primemaps", //196 kifglobalthread.cxx 1598
"iibinmap", //197 kifglobalthread.cxx 1608
"ifbinmap", //198 kifglobalthread.cxx 1609
"isbinmap", //199 kifglobalthread.cxx 1610
"iubinmap", //200 kifglobalthread.cxx 1611
"uuprimemap", //201 kifglobalthread.cxx 1614
"uiprimemap", //202 kifglobalthread.cxx 1616
"ufprimemap", //203 kifglobalthread.cxx 1618
"iuprimemap", //204 kifglobalthread.cxx 1620
"fuprimemap", //205 kifglobalthread.cxx 1622
"isprimemap", //206 kifglobalthread.cxx 1624
"iiprimemap", //207 kifglobalthread.cxx 1625
"ifprimemap", //208 kifglobalthread.cxx 1626
"fsprimemap", //209 kifglobalthread.cxx 1627
"fiprimemap", //210 kifglobalthread.cxx 1628
"ffprimemap", //211 kifglobalthread.cxx 1629
"sprimemap", //212 kifglobalthread.cxx 1631
"iprimemap", //213 kifglobalthread.cxx 1632
"fprimemap", //214 kifglobalthread.cxx 1633
"ssprimemap", //215 kifglobalthread.cxx 1634
"siprimemap", //216 kifglobalthread.cxx 1635
"sfprimemap", //217 kifglobalthread.cxx 1636
"kif", //218 kifglobalthread.cxx 1681
"predicatevar", //219 kifglobalthread.cxx 1702
"fumap", //220 kifglobalthread.cxx 1757
"futreemap", //221 kifglobalthread.cxx 1799
"riterator", //222 kifglobalthread.cxx 1873
"tree", //223 kifglobalthread.cxx 1874
"is", //224 kifglobalthread.cxx 2023
"isnot", //225 kifglobalthread.cxx 2024
"or", //226 kifglobalthread.cxx 2059
"and", //227 kifglobalthread.cxx 2060
"on", //228 kifglobalthread.cxx 2062
"notin", //229 kifglobalthread.cxx 2063
"_compatibilities", //230 kifglobalthread.cxx 2268
"infos", //231 kifglobalthread.cxx 2277
"isa", //232 kifglobalthread.cxx 2278
"type", //233 kifglobalthread.cxx 2279
"iscontainer", //234 kifglobalthread.cxx 2280
"isvector", //235 kifglobalthread.cxx 2282
"name", //236 kifglobalthread.cxx 2283
"min", //237 kifglobalthread.cxx 2287
"max", //238 kifglobalthread.cxx 2288
"clear", //239 kifglobalthread.cxx 2289
"addtowaiton", //240 kifglobalthread.cxx 2292
"load", //241 kifglobalthread.cxx 2297
"loadlemma", //242 kifglobalthread.cxx 2304
"model", //243 kifglobalthread.cxx 2305
"look", //244 kifglobalthread.cxx 2306
"spotfinalstate", //245 kifglobalthread.cxx 2307
"loadcompact", //246 kifglobalthread.cxx 2308
"store", //247 kifglobalthread.cxx 2309
"setcodefeature", //248 kifglobalthread.cxx 2311
"editdistance", //249 kifglobalthread.cxx 2312
"add", //250 kifglobalthread.cxx 2314
"settransformationmap", //251 kifglobalthread.cxx 2317
"build", //252 kifglobalthread.cxx 2321
"compilergx", //253 kifglobalthread.cxx 2323
"lookup", //254 kifglobalthread.cxx 2324
"lookdown", //255 kifglobalthread.cxx 2325
"process", //256 kifglobalthread.cxx 2326
"sister", //257 kifglobalthread.cxx 2340
"previous", //258 kifglobalthread.cxx 2342
"mother", //259 kifglobalthread.cxx 2343
"daughter", //260 kifglobalthread.cxx 2344
"lastnode", //261 kifglobalthread.cxx 2345
"depth", //262 kifglobalthread.cxx 2346
"prune", //263 kifglobalthread.cxx 2347
"isolate", //264 kifglobalthread.cxx 2348
"ioredirect", //265 kifglobalthread.cxx 2360
"hash", //266 kifglobalthread.cxx 2362
"reverse", //267 kifglobalthread.cxx 2365
"ord", //268 kifglobalthread.cxx 2366
"bytes", //269 kifglobalthread.cxx 2367
"format", //270 kifglobalthread.cxx 2368
"base", //271 kifglobalthread.cxx 2369
"fill", //272 kifglobalthread.cxx 2370
"padding", //273 kifglobalthread.cxx 2371
"parse", //274 kifglobalthread.cxx 2374
"push", //275 kifglobalthread.cxx 2375
"sizeb", //276 kifglobalthread.cxx 2377
"lisp", //277 kifglobalthread.cxx 2378
"tags", //278 kifglobalthread.cxx 2379
"resize", //279 kifglobalthread.cxx 2380
"evaluate", //280 kifglobalthread.cxx 2382
"tohtml", //281 kifglobalthread.cxx 2384
"toxml", //282 kifglobalthread.cxx 2385
"replaceregexip", //283 kifglobalthread.cxx 2386
"replace", //284 kifglobalthread.cxx 2389
"getstd", //285 kifglobalthread.cxx 2390
"geterr", //286 kifglobalthread.cxx 2391
"split", //287 kifglobalthread.cxx 2392
"splite", //288 kifglobalthread.cxx 2393
"tokenize", //289 kifglobalthread.cxx 2394
"stokenize", //290 kifglobalthread.cxx 2395
"count", //291 kifglobalthread.cxx 2396
"find", //292 kifglobalthread.cxx 2397
"rfind", //293 kifglobalthread.cxx 2398
"removefirst", //294 kifglobalthread.cxx 2399
"removelast", //295 kifglobalthread.cxx 2400
"isutf8", //296 kifglobalthread.cxx 2401
"ngrams", //297 kifglobalthread.cxx 2402
"latin", //298 kifglobalthread.cxx 2404
"dos", //299 kifglobalthread.cxx 2405
"dostoutf8", //300 kifglobalthread.cxx 2406
"left", //301 kifglobalthread.cxx 2407
"byteposition", //302 kifglobalthread.cxx 2408
"charposition", //303 kifglobalthread.cxx 2409
"right", //304 kifglobalthread.cxx 2410
"mid", //305 kifglobalthread.cxx 2411
"isalpha", //306 kifglobalthread.cxx 2412
"isconsonant", //307 kifglobalthread.cxx 2413
"isvowel", //308 kifglobalthread.cxx 2414
"ispunctuation", //309 kifglobalthread.cxx 2415
"isdigit", //310 kifglobalthread.cxx 2416
"extract", //311 kifglobalthread.cxx 2417
"isupper", //312 kifglobalthread.cxx 2418
"islower", //313 kifglobalthread.cxx 2419
"upper", //314 kifglobalthread.cxx 2420
"deaccentuate", //315 kifglobalthread.cxx 2421
"lower", //316 kifglobalthread.cxx 2422
"trim", //317 kifglobalthread.cxx 2423
"trimleft", //318 kifglobalthread.cxx 2424
"trimright", //319 kifglobalthread.cxx 2425
"last", //320 kifglobalthread.cxx 2426
"insert", //321 kifglobalthread.cxx 2427
"query", //322 kifglobalthread.cxx 2435
"remove", //323 kifglobalthread.cxx 2437
"weight", //324 kifglobalthread.cxx 2438
"duplicate", //325 kifglobalthread.cxx 2445
"move", //326 kifglobalthread.cxx 2447
"merge", //327 kifglobalthread.cxx 2451
"totext", //328 kifglobalthread.cxx 2454
"flatten", //329 kifglobalthread.cxx 2455
"test", //330 kifglobalthread.cxx 2456
"join", //331 kifglobalthread.cxx 2457
"reserve", //332 kifglobalthread.cxx 2458
"sort", //333 kifglobalthread.cxx 2463
"permute", //334 kifglobalthread.cxx 2464
"shuffle", //335 kifglobalthread.cxx 2465
"unique", //336 kifglobalthread.cxx 2466
"sortint", //337 kifglobalthread.cxx 2467
"sortfloat", //338 kifglobalthread.cxx 2468
"sortstring", //339 kifglobalthread.cxx 2469
"sortlong", //340 kifglobalthread.cxx 2470
"sum", //341 kifglobalthread.cxx 2471
"product", //342 kifglobalthread.cxx 2472
"pushfirst", //343 kifglobalthread.cxx 2497
"poplast", //344 kifglobalthread.cxx 2499
"pushlast", //345 kifglobalthread.cxx 2500
"popfirst", //346 kifglobalthread.cxx 2501
"first", //347 kifglobalthread.cxx 2512
"run", //348 kifglobalthread.cxx 2530
"runasthread", //349 kifglobalthread.cxx 2533
"kstart", //350 kifglobalthread.cxx 2534
"runend", //351 kifglobalthread.cxx 2535
"debugfunction", //352 kifglobalthread.cxx 2536
"debugclose", //353 kifglobalthread.cxx 2537
"compile", //354 kifglobalthread.cxx 2538
"open", //355 kifglobalthread.cxx 2539
"clean", //356 kifglobalthread.cxx 2540
"exposed", //357 kifglobalthread.cxx 2541
"invert", //358 kifglobalthread.cxx 2546
"keys", //359 kifglobalthread.cxx 2560
"values", //360 kifglobalthread.cxx 2561
"items", //361 kifglobalthread.cxx 2562
"evaluation", //362 kifglobalthread.cxx 2576
"seeds", //363 kifglobalthread.cxx 2577
"openwrite", //364 kifglobalthread.cxx 2609
"openappend", //365 kifglobalthread.cxx 2610
"openread", //366 kifglobalthread.cxx 2611
"setsignature", //367 kifglobalthread.cxx 2612
"getsignature", //368 kifglobalthread.cxx 2613
"flush", //369 kifglobalthread.cxx 2614
"unget", //370 kifglobalthread.cxx 2616
"close", //371 kifglobalthread.cxx 2617
"seek", //372 kifglobalthread.cxx 2618
"tell", //373 kifglobalthread.cxx 2619
"write", //374 kifglobalthread.cxx 2620
"writeln", //375 kifglobalthread.cxx 2621
"writebin", //376 kifglobalthread.cxx 2622
"read", //377 kifglobalthread.cxx 2623
"readln", //378 kifglobalthread.cxx 2624
"eof", //379 kifglobalthread.cxx 2630
"setdate", //380 kifglobalthread.cxx 2639
"year", //381 kifglobalthread.cxx 2641
"month", //382 kifglobalthread.cxx 2642
"day", //383 kifglobalthread.cxx 2643
"yearday", //384 kifglobalthread.cxx 2644
"weekday", //385 kifglobalthread.cxx 2645
"hour", //386 kifglobalthread.cxx 2646
"minute", //387 kifglobalthread.cxx 2647
"second", //388 kifglobalthread.cxx 2649
"reset", //389 kifglobalthread.cxx 2667
"abs", //390 kifglobalthread.cxx 2680
"acos", //391 kifglobalthread.cxx 2681
"acosh", //392 kifglobalthread.cxx 2682
"asin", //393 kifglobalthread.cxx 2683
"asinh", //394 kifglobalthread.cxx 2684
"atan", //395 kifglobalthread.cxx 2685
"atanh", //396 kifglobalthread.cxx 2686
"cbrt", //397 kifglobalthread.cxx 2687
"cos", //398 kifglobalthread.cxx 2688
"cosh", //399 kifglobalthread.cxx 2689
"erf", //400 kifglobalthread.cxx 2690
"erfc", //401 kifglobalthread.cxx 2691
"exp", //402 kifglobalthread.cxx 2692
"exp2", //403 kifglobalthread.cxx 2693
"expm1", //404 kifglobalthread.cxx 2694
"floor", //405 kifglobalthread.cxx 2695
"lgamma", //406 kifglobalthread.cxx 2696
"ln", //407 kifglobalthread.cxx 2697
"log", //408 kifglobalthread.cxx 2698
"log1p", //409 kifglobalthread.cxx 2699
"log2", //410 kifglobalthread.cxx 2700
"logb", //411 kifglobalthread.cxx 2701
"nearbyint", //412 kifglobalthread.cxx 2702
"rint", //413 kifglobalthread.cxx 2703
"round", //414 kifglobalthread.cxx 2704
"sin", //415 kifglobalthread.cxx 2705
"sinh", //416 kifglobalthread.cxx 2706
"sqrt", //417 kifglobalthread.cxx 2707
"tan", //418 kifglobalthread.cxx 2708
"tanh", //419 kifglobalthread.cxx 2709
"tgamma", //420 kifglobalthread.cxx 2710
"trunc", //421 kifglobalthread.cxx 2711
"chr", //422 kifglobalthread.cxx 2717
"iorestate", //423 kifglobalthread.cxx 2719
"partof", //424 kifglobalthread.cxx 2725
"factors", //425 kifglobalthread.cxx 2727
"prime", //426 kifglobalthread.cxx 2728
"radian", //427 kifglobalthread.cxx 2731
"even", //428 kifglobalthread.cxx 2732
"odd", //429 kifglobalthread.cxx 2733
"degree", //430 kifglobalthread.cxx 2734
"simplify", //431 kifglobalthread.cxx 2742
"nd", //432 kifglobalthread.cxx 2744
"euclidian", //433 kifglobalthread.cxx 2747
"next", //434 kifglobalthread.cxx 2750
"end", //435 kifglobalthread.cxx 2751
"setvalue", //436 kifglobalthread.cxx 2752
"nend", //437 kifglobalthread.cxx 2753
"begin", //438 kifglobalthread.cxx 2754
"key", //439 kifglobalthread.cxx 2755
"value", //440 kifglobalthread.cxx 2756
"valuetype", //441 kifglobalthread.cxx 2758
"_stack", //442 kifglobalthread.cxx 3040
"_filename", //443 kifglobalthread.cxx 3041
"pause", //444 kifglobalthread.cxx 3106
"sleep", //445 kifglobalthread.cxx 3107
"eval", //446 kifglobalthread.cxx 3108
"fromjson", //447 kifglobalthread.cxx 3109
"run", //448 kifglobalthread.cxx 3110
"range", //449 kifglobalthread.cxx 3117
"raise", //450 kifglobalthread.cxx 3135
"block", //451 kifglobalthread.cxx 3152
"unlock", //452 kifglobalthread.cxx 3153
"lock", //453 kifglobalthread.cxx 3154
"wait", //454 kifglobalthread.cxx 3155
"waitonfalse", //455 kifglobalthread.cxx 3156
"waitonjoined", //456 kifglobalthread.cxx 3157
"waiting", //457 kifglobalthread.cxx 3158
"cast", //458 kifglobalthread.cxx 3159
"kill", //459 kifglobalthread.cxx 3160
"_erroronkey", //460 kifglobalthread.cxx 3162
"_garbagesurvey", //461 kifglobalthread.cxx 3163
"_garbageframe", //462 kifglobalthread.cxx 3164
"_gcsize", //463 kifglobalthread.cxx 3165
"_version", //464 kifglobalthread.cxx 3166
"_exit", //465 kifglobalthread.cxx 3167
"print", //466 kifglobalthread.cxx 3169
"pflush", //467 kifglobalthread.cxx 3170
"println", //468 kifglobalthread.cxx 3171
"printj", //469 kifglobalthread.cxx 3172
"printjerr", //470 kifglobalthread.cxx 3173
"printjln", //471 kifglobalthread.cxx 3174
"printjlnerr", //472 kifglobalthread.cxx 3175
"predicatedump", //473 kifglobalthread.cxx 3176
"findall", //474 kifglobalthread.cxx 3177
"retractall", //475 kifglobalthread.cxx 3178
"printerr", //476 kifglobalthread.cxx 3179
"printlnerr", //477 kifglobalthread.cxx 3180
"threadid", //478 kifglobalthread.cxx 3186
"parentthreadid", //479 kifglobalthread.cxx 3187
"setdebugfunction", //480 kifglobalthread.cxx 3189
"debugclear", //481 kifglobalthread.cxx 3191
"setdebugdata", //482 kifglobalthread.cxx 3192
"getstackinfo", //483 kifglobalthread.cxx 3194
"inkey", //484 kifglobalthread.cxx 3242
"getc", //485 kifglobalthread.cxx 3271
"random", //486 kifglobalthread.cxx 3274
"valueisa", //487 kifglobalthread.cxx 3352
"build(string input,string output,bool norm,int latin): Build a transducer file out of a text file containing on the first line surface form, then on next line lemma+features. norm is used to make the transducer case incensitive. latin is the latin table encoding number.", //488 kifglobalthread.cxx 3384
"store(string output,bool norm,int encoding): Store a transducer into a file. The last two parameters are optional.", //489 kifglobalthread.cxx 3385
"compilergx(string rgx,svector features,string filename): Build a transducer file out of regular expressions. filename is optional, the resulting automaton is stored in a file.", //490 kifglobalthread.cxx 3386
"load(string file): load a transducer file.", //491 kifglobalthread.cxx 3387
"add(container,bool norm,int encoding): transform a container (vector or map) into a transducer lexion. If the container is a vector, then it should have an even number of values.", //492 kifglobalthread.cxx 3388
"lookup(string wrd,int threshold, int flags): Lookup of a word using a transducer. The lookup can be constrained with a threshold and flags (a_insert, a_first, a_delete, a_switch, a_change).", //493 kifglobalthread.cxx 3389
"lookdown(string wrd): Lookdown of a word using a transducer.", //494 kifglobalthread.cxx 3390
"process(string sentence): Analyse a sequence of words using a transducer.", //495 kifglobalthread.cxx 3391
"weight(): Returns the weight of this predicate.", //496 kifglobalthread.cxx 3394
"query(predicate,var1,var2...): This query is analyzed as a predicate.", //497 kifglobalthread.cxx 3395
"store(sqlite db): Store a predicate in the database. If db is omitted then store it in the knowledge base", //498 kifglobalthread.cxx 3396
"remove(sqlite db): Remove a predicate from the database. If db is omitted then remove it from the knowledge base", //499 kifglobalthread.cxx 3397
"hash(): Return the hash value of a string.", //500 kifglobalthread.cxx 3399
"resize(nb): Resize a rawstring.", //501 kifglobalthread.cxx 3400
"json(): Return the json encoded version of a string.", //502 kifglobalthread.cxx 3401
"byteposition(int pos): convert a character position into a byte position (especially useful in UTF8 strings)", //503 kifglobalthread.cxx 3402
"charposition(int pos): convert a byte position into a character position.", //504 kifglobalthread.cxx 3403
"size(): Return the size of the string.", //505 kifglobalthread.cxx 3404
"evaluate(): evaluate the meta-characters within a string and return the evaluated string.", //506 kifglobalthread.cxx 3405
"clear(): Clean the content of a string.", //507 kifglobalthread.cxx 3406
"tokenize(bool comma,bool separator,bool concatenate): Segment a string into words and punctuations. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token.", //508 kifglobalthread.cxx 3407
"stokenize(map keeps): Segment a string into words and punctuations, with a keep.", //509 kifglobalthread.cxx 3408
"split(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters", //510 kifglobalthread.cxx 3409
"splite(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters. Empty strings are kept in the result.", //511 kifglobalthread.cxx 3410
"removefirst(int nb): remove the first nb characters of a string", //512 kifglobalthread.cxx 3411
"removelast(int nb): remove the last nb characters of a string", //513 kifglobalthread.cxx 3412
"fill(int nb,string c): create a string of nb characters c", //514 kifglobalthread.cxx 3413
"padding(int nb,string c,bool paddattheend): add nb characters c to the current string. Last parameter is optional", //515 kifglobalthread.cxx 3414
"format(p1,p2,p3): Create a new string from the current string in which each '%x' is associated to one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.", //516 kifglobalthread.cxx 3415
"ord(): return the ASCII code of the first character, or a list of all ASCII code if the recipient is a vector", //517 kifglobalthread.cxx 3416
"base(int b): Return the value corresponding to the string in base b", //518 kifglobalthread.cxx 3417
"left(int nb): return the first nb characters of a string", //519 kifglobalthread.cxx 3418
"byteposition(int pos): Convert a character position into a byte position", //520 kifglobalthread.cxx 3419
"charposition(int pos): Convert a byte position into a character position", //521 kifglobalthread.cxx 3420
"right(int nb): return the last nb characters of a string", //522 kifglobalthread.cxx 3421
"mid(int pos,int nb): return the nb characters starting at position pos of a string", //523 kifglobalthread.cxx 3422
"reverse(): reverse the string", //524 kifglobalthread.cxx 3423
"pop(i): remove character at position i", //525 kifglobalthread.cxx 3424
"pop(): remove last character", //526 kifglobalthread.cxx 3425
"last(): return last character", //527 kifglobalthread.cxx 3426
"insert(i,s): insert the string s at i. If i is -1, then insert s between each character in the input string", //528 kifglobalthread.cxx 3427
"trim(): remove the trailing characters", //529 kifglobalthread.cxx 3428
"trimright(): remove the trailing characters on the right", //530 kifglobalthread.cxx 3429
"trimleft(): remove the trailing characters on the left", //531 kifglobalthread.cxx 3430
"deaccentuate(): Remove the accents from accented characters", //532 kifglobalthread.cxx 3431
"upper(): Return the string in upper characters", //533 kifglobalthread.cxx 3432
"lower(): Return the string in lower characters", //534 kifglobalthread.cxx 3433
"islower(): Test if a string only contains lowercase characters", //535 kifglobalthread.cxx 3434
"isupper(): Test if a string only contains uppercase characters", //536 kifglobalthread.cxx 3435
"isalpha(): Test if a string only contains only alphabetical characters", //537 kifglobalthread.cxx 3436
"isconsonant(): Test if a string only contains consonants", //538 kifglobalthread.cxx 3437
"isvowel(): Test if a string only contains only vowels", //539 kifglobalthread.cxx 3438
"ispunctuation(): Test if the character is a punctuation", //540 kifglobalthread.cxx 3439
"extract(int pos,string from,string up1,string up2...): extract substrings between 'from' and 'up1'...'upn' (the shortest string is used). Return a vector of strings", //541 kifglobalthread.cxx 3440
"isdigit(): Test if a string only contains digits", //542 kifglobalthread.cxx 3441
"latin(): convert an UTF8 string in LATIN", //543 kifglobalthread.cxx 3442
"utf8(int table): convert a LATIN string into UTF8. table is optional, by default it is ISO/IEC 8859 part 1.", //544 kifglobalthread.cxx 3443
"dos(): convert a string into DOS encoding", //545 kifglobalthread.cxx 3444
"dostoutf8(): convert a DOS string into UTF8", //546 kifglobalthread.cxx 3445
"isutf8(): Return true is the string is encoded in UTF8", //547 kifglobalthread.cxx 3446
"ngrams(int r): Return a vector of all ngrams of rank r", //548 kifglobalthread.cxx 3447
"count(string sub,int pos,int mx): Count the number of substrings starting at position pos, ending at mx", //549 kifglobalthread.cxx 3448
"find(string sub,int pos): Return the position of substring sub starting at position pos", //550 kifglobalthread.cxx 3449
"sizeb(): Return the size in bytes of the string", //551 kifglobalthread.cxx 3450
"rfind(string sub,int pos): Return the position of substring sub backward starting at position pos", //552 kifglobalthread.cxx 3451
"replaceregexip(sub,str): Replace the substrings matching sub with str", //553 kifglobalthread.cxx 3452
"levenshtein(string s,bool byte): Return the edit distance with 's' according to Levenshtein algorithm. If byte is true, force a byte level comparison. byte is optionnal.", //554 kifglobalthread.cxx 3453
"editdistance(string s,bool byte): Return the edit distance with 's'. If byte is true, force a byte level comparison. byte is optionnal.", //555 kifglobalthread.cxx 3454
"html(): Return the string into an HTML compatible string or as a vector of strings", //556 kifglobalthread.cxx 3455
"tohtml(): Return the string into an HTML compatible string or as a vector of strings", //557 kifglobalthread.cxx 3456
"toxml(): Return the string into an XML compatible string or as a vector of strings", //558 kifglobalthread.cxx 3457
"regexip(sub): Find the substrings matching sub", //559 kifglobalthread.cxx 3458
"replace(sub,str): Replace the substrings matching sub with str", //560 kifglobalthread.cxx 3459
"getstd(bool): catch or release the standard output", //561 kifglobalthread.cxx 3460
"geterr(bool): catch or release the error output", //562 kifglobalthread.cxx 3461
"replacergx", //563 kifglobalthread.cxx 3462
"replacergx(rgx,str): Replace the substrings matching rgx with str", //564 kifglobalthread.cxx 3462
"splitrgx", //565 kifglobalthread.cxx 3463
"splitrgx(rgx): Split string with regular expression rgx. Return a vector of substrings.", //566 kifglobalthread.cxx 3463
"regex(rgx): Test if the regular expression rgx applies to string|vector=regex(rgx): Return all substrings matching rgx|string=regex(rgx): Return the substring matching rgx in the string|int=regex(rgx): Return the position of the substring matching rgx in the string", //567 kifglobalthread.cxx 3464
"parse(): Parse a string as a piece of code and returns the evaluation as a vector.", //568 kifglobalthread.cxx 3465
"lisp(): Parse a string as a parenthetic expressions, which is returned as a vector.|lisp(string o,string c): Parse a string as a parenthetic expressions, where the opening and closing characters are provided.", //569 kifglobalthread.cxx 3466
"tags(string o,string c): Parse a string as a parenthetic expression, where the opening and closing strings are provided.", //570 kifglobalthread.cxx 3467
"factors(): return the decomposition in prime factors", //571 kifglobalthread.cxx 3468
"prime(): return true is the number is a prime", //572 kifglobalthread.cxx 3469
"base(int b): return a string representing a number in base b", //573 kifglobalthread.cxx 3470
"bit(int i): return true if the ith bit is 1.", //574 kifglobalthread.cxx 3471
"invert(): Invert the denominator and the numerator of a fraction", //575 kifglobalthread.cxx 3476
"partof(bits x): Return true if the bits of x are all in the current bits", //576 kifglobalthread.cxx 3478
"d(): Denominator of a fraction (with a parameter: set the value, without parameter return the value)", //577 kifglobalthread.cxx 3480
"infos(string): infos about a method", //578 kifglobalthread.cxx 3481
"methods(): Return a vector of all available methods", //579 kifglobalthread.cxx 3482
"simplify(): Simplification", //580 kifglobalthread.cxx 3483
"n(): Numerator of a fraction (with a parameter: set the value, without parameter return the value)", //581 kifglobalthread.cxx 3485
"nd(int n,int d): Set the Numerator and the Denominator of a fraction", //582 kifglobalthread.cxx 3486
"chr(): return the character whose code is the value of the variable", //583 kifglobalthread.cxx 3487
"#(): return the bit complement", //584 kifglobalthread.cxx 3489
"format(string form): return a string formatted according to the pattern in form. (this format is the same as the sprintf format in C++)", //585 kifglobalthread.cxx 3491
"radian(): Convert a value in degree into a value in radian", //586 kifglobalthread.cxx 3493
"even(): Return true if the value is even.", //587 kifglobalthread.cxx 3494
"odd(): Return true if the value is odd.", //588 kifglobalthread.cxx 3495
"degree(): Convert a value in radian into a value in degree", //589 kifglobalthread.cxx 3496
"get(): Read a string from keyboard", //590 kifglobalthread.cxx 3501
"bytes(): Return the string as a vector of bytes", //591 kifglobalthread.cxx 3502
"get(): Read a number from keyboard", //592 kifglobalthread.cxx 3503
"abs(): absolute value", //593 kifglobalthread.cxx 3506
"acos(): arc cosine", //594 kifglobalthread.cxx 3508
"acosh(): area hyperbolic cosine", //595 kifglobalthread.cxx 3510
"asin(): arc sine", //596 kifglobalthread.cxx 3512
"asinh(): area hyperbolic sine", //597 kifglobalthread.cxx 3514
"atan(): arc tangent", //598 kifglobalthread.cxx 3516
"atanh(): area hyperbolic tangent", //599 kifglobalthread.cxx 3518
"cbrt(): cubic root", //600 kifglobalthread.cxx 3520
"cos(): cosine", //601 kifglobalthread.cxx 3522
"cosh(): hyperbolic cosine", //602 kifglobalthread.cxx 3524
"erf(): error function", //603 kifglobalthread.cxx 3526
"erfc(): complementary error function", //604 kifglobalthread.cxx 3528
"exp(): exponential function", //605 kifglobalthread.cxx 3530
"exp2(): binary exponential function", //606 kifglobalthread.cxx 3532
"expm1(): exponential minus one", //607 kifglobalthread.cxx 3534
"floor(): down value", //608 kifglobalthread.cxx 3536
"lgamma(): log-gamma function", //609 kifglobalthread.cxx 3538
"log(): logarithm base 10", //610 kifglobalthread.cxx 3540
"ln(): natural logarithm", //611 kifglobalthread.cxx 3542
"log1p(): logarithm plus one", //612 kifglobalthread.cxx 3544
"log2(): binary logarithm", //613 kifglobalthread.cxx 3546
"logb(): floating-point base logarithm", //614 kifglobalthread.cxx 3548
"nearbyint(): to nearby integral value", //615 kifglobalthread.cxx 3550
"rint(): to integral value", //616 kifglobalthread.cxx 3552
"round(): to nearest", //617 kifglobalthread.cxx 3554
"sin(): sine", //618 kifglobalthread.cxx 3556
"sinh(): hyperbolic sine", //619 kifglobalthread.cxx 3558
"sqrt(): square root", //620 kifglobalthread.cxx 3560
"tan(): tangent", //621 kifglobalthread.cxx 3562
"tanh(): hyperbolic tangent", //622 kifglobalthread.cxx 3564
"tgamma(): gamma function", //623 kifglobalthread.cxx 3566
"trunc(): value", //624 kifglobalthread.cxx 3568
"resize(nb): Resize a table", //625 kifglobalthread.cxx 3571
"ngrams(int r,string sep): Return a vector of all ngrams of rank r, strings are concatenated with 'sep'", //626 kifglobalthread.cxx 3572
"move(int pfrom,int pto): move the element in pfrom to pto.", //627 kifglobalthread.cxx 3573
"push(a): add a to the vector", //628 kifglobalthread.cxx 3574
"editdistance(s): Return the edit distance with 's'", //629 kifglobalthread.cxx 3575
"flatten(): merge all the sub-vectors elements into the current vector", //630 kifglobalthread.cxx 3576
"merge(v): merge the elements from v into the container", //631 kifglobalthread.cxx 3577
"bytes(): Return the string matching a vector of bytes. Each value should be between 0..255", //632 kifglobalthread.cxx 3578
"totext(): Return the string matching a vector of bytes. Each value should be between 0..255", //633 kifglobalthread.cxx 3579
"test(i): test if i is a valid slot in the vector", //634 kifglobalthread.cxx 3580
"pop(int i|string s): remove the ith element or the value s.", //635 kifglobalthread.cxx 3581
"pop(): remove the last element from the vector and return it.", //636 kifglobalthread.cxx 3582
"last(): return the last element of the vector", //637 kifglobalthread.cxx 3583
"reverse(): reverse the order of the elements in the vector", //638 kifglobalthread.cxx 3584
"join(string sep): concatenate each element in the vector in a string where each element is separated from the others with sep", //639 kifglobalthread.cxx 3585
"reserve(int sz): Reserve sz elements in a vector", //640 kifglobalthread.cxx 3586
"insert(i,x): insert the element x at position i", //641 kifglobalthread.cxx 3587
"sum(): Sum each element with the others", //642 kifglobalthread.cxx 3588
"product(): Multiply each element with the others", //643 kifglobalthread.cxx 3589
"predicate(): Create a new predicate of the form [head,param1,param2..]", //644 kifglobalthread.cxx 3590
"clear(): clean the vector", //645 kifglobalthread.cxx 3591
"min(): return the minimum value in a container", //646 kifglobalthread.cxx 3592
"max(): return the maximum value in a container", //647 kifglobalthread.cxx 3593
"sort(compare): sort the content of the vector according to compare function. compare is optional", //648 kifglobalthread.cxx 3594
"shuffle(): reshuffle the values", //649 kifglobalthread.cxx 3595
"permute(): permute the values in a vector. Return false, when all permutations have been explored.", //650 kifglobalthread.cxx 3596
"unique(): remove duplicates", //651 kifglobalthread.cxx 3597
"sortint(bool order): sort the content of the vector forcing elements to be ints.", //652 kifglobalthread.cxx 3598
"sortfloat(bool order): sort the content of the vector forcing elements to be floats.", //653 kifglobalthread.cxx 3599
"sortlong(bool order): sort the content of the vector forcing elements to be longs.", //654 kifglobalthread.cxx 3600
"sortstring(bool order): sort the content of the vector forcing elements to be strings.", //655 kifglobalthread.cxx 3601
"apply(a,b,c): apply all functions stored in the vector, passing a,b,c etc. as parameters.", //656 kifglobalthread.cxx 3602
"waiton(t1,t2,...,p1,p2..,pn): Waiting for threads, with table as the first argument for of each thread. p1,p2..pn are optional variables that will be passed as next parameters to each thread.", //657 kifglobalthread.cxx 3603
"addtowaiton(t1,t2,...,tn): Addind new threads to the thread waiting list.", //658 kifglobalthread.cxx 3604
"trigger(bool loop): Trigger the threads, if loop is true the function is put in waiting mode back.", //659 kifglobalthread.cxx 3605
"pushfirst(a): add a to the beginning of the list", //660 kifglobalthread.cxx 3607
"test(i): test if i is a valid slot in the list", //661 kifglobalthread.cxx 3610
"pushlast(a): add a at the end of the list", //662 kifglobalthread.cxx 3615
"popfirst(): remove the first element from the list and return it.", //663 kifglobalthread.cxx 3616
"poplast(): remove the last element from the list and return it.", //664 kifglobalthread.cxx 3621
"last(): return the last element of the list", //665 kifglobalthread.cxx 3622
"first(): return the first element of the list", //666 kifglobalthread.cxx 3623
"reverse(): reverse the order of the elements in the list", //667 kifglobalthread.cxx 3624
"join(string sep): concatenate each element in the list in a string where each element is separated from the others with sep", //668 kifglobalthread.cxx 3625
"apply(a,b,c): apply all functions stored in the list, passing a,b,c etc. as parameters.", //669 kifglobalthread.cxx 3629
"clear(): clean the list", //670 kifglobalthread.cxx 3630
"invert(): Invert keys and values in a map", //671 kifglobalthread.cxx 3632
"join(string keysep,string sep): concatenate each element in the map in a string where each key is appended with a keysep and each element is separated from the others with sep", //672 kifglobalthread.cxx 3634
"predicate(): Create a new predicate of the form {'name':name,0:param1,1:param2..]", //673 kifglobalthread.cxx 3638
"test(i): test if i is a valid key in the map", //674 kifglobalthread.cxx 3639
"pop(string key): remove the elements matching key", //675 kifglobalthread.cxx 3641
"keys(): returns the map keys as a vector", //676 kifglobalthread.cxx 3642
"values(): return the values as a vector", //677 kifglobalthread.cxx 3643
"items(): return a vector of key:value", //678 kifglobalthread.cxx 3644
"clear(): clean the map", //679 kifglobalthread.cxx 3647
"evaluation(): evaluate the memory footprint of a primemap", //680 kifglobalthread.cxx 3648
"seeds(ivector iv): set the primemap seed keys", //681 kifglobalthread.cxx 3649
"tree(value): create a tree node, with value as a value. Value can have any types", //682 kifglobalthread.cxx 3651
"sister()|sister(tree n): return the sister node, or add a sister node", //683 kifglobalthread.cxx 3653
"previous|previous(tree n): return the previous node or add n as the previous node", //684 kifglobalthread.cxx 3654
"daughter()|daughter(tree n): return the first child node or add n as the first child node", //685 kifglobalthread.cxx 3655
"mother()|mother(tree n): return the mother node or test if the current node is a parent of n", //686 kifglobalthread.cxx 3656
"lastnode()|lastnode(tree n): return the last node or test if the current node is the last child of n", //687 kifglobalthread.cxx 3657
"isolate(): Extract the current node from its tree", //688 kifglobalthread.cxx 3658
"prune(): Delete the current sub-tree from the global tree", //689 kifglobalthread.cxx 3659
"depth(): Return the depth of the node in the tree", //690 kifglobalthread.cxx 3660
"value(): return the value of the current element", //691 kifglobalthread.cxx 3661
"key(): return the key of the current element", //692 kifglobalthread.cxx 3662
"valuetype(): return the value type of the current element", //693 kifglobalthread.cxx 3663
"valueisa(string type): test the type of the current element", //694 kifglobalthread.cxx 3664
"begin(): initialiaze the iterator with the beginning of the collection", //695 kifglobalthread.cxx 3665
"next(): next element in the collection", //696 kifglobalthread.cxx 3666
"setvalue(value): Set a new value to the current iterator placeholder.", //697 kifglobalthread.cxx 3667
"end(): return true when the end of the collection is reached", //698 kifglobalthread.cxx 3668
"nend(): return true while the end of the collection has not been reached (~end())", //699 kifglobalthread.cxx 3669
"apply(a,b,c): apply a function", //700 kifglobalthread.cxx 3670
"setdate(year,month,day,hour,min,sec): set a time variable", //701 kifglobalthread.cxx 3671
"date(): return the date as a string", //702 kifglobalthread.cxx 3672
"year(): return the year as an integer", //703 kifglobalthread.cxx 3673
"month(): return the month as an integer", //704 kifglobalthread.cxx 3674
"day(): return the day as an integer", //705 kifglobalthread.cxx 3675
"yearday(): return the year day as an integer between 0-365", //706 kifglobalthread.cxx 3676
"weekday(): return the week day as an integer between 0-6, 0 is Sunday.", //707 kifglobalthread.cxx 3677
"hour(): return the hour as an integer", //708 kifglobalthread.cxx 3678
"minute(): return the minutes as an integer", //709 kifglobalthread.cxx 3679
"second(): return the seconds as an integer", //710 kifglobalthread.cxx 3680
"reset (): reinitialize a time variable", //711 kifglobalthread.cxx 3681
"openappend(string filename): open a file in append writing mode", //712 kifglobalthread.cxx 3682
"openwrite(string filename): open a file in write mode", //713 kifglobalthread.cxx 3683
"close(): close a file", //714 kifglobalthread.cxx 3684
"find(string str,bool uppercase): Find a string in a file", //715 kifglobalthread.cxx 3685
"openread(string filename): open a file in read mode", //716 kifglobalthread.cxx 3686
"setsignature(bool s): Set the signature mode", //717 kifglobalthread.cxx 3687
"getsignature(): Return the signature mode", //718 kifglobalthread.cxx 3688
"write(string s1,string s2,): write strings in the file", //719 kifglobalthread.cxx 3689
"writeln(string s1,string s2,): write strings in the file, terminated with a carriage return", //720 kifglobalthread.cxx 3690
"writebin(int s1,int s2,): write character code in a file", //721 kifglobalthread.cxx 3691
"get(): read one character from the file", //722 kifglobalthread.cxx 3692
"unget(): return one character to the stream", //723 kifglobalthread.cxx 3693
"unget(nb): return nb character to the stream", //724 kifglobalthread.cxx 3694
"read(): read a line from a file", //725 kifglobalthread.cxx 3695
"readln(): read a line from a file", //726 kifglobalthread.cxx 3696
"seek(int p): position the file cursor at p", //727 kifglobalthread.cxx 3697
"tell(): return the position of the file cursor", //728 kifglobalthread.cxx 3698
"eof(): return true when the end of file is reached", //729 kifglobalthread.cxx 3699
"load(svector rules): Load a grammar out of a svector. Each line must be a rule.", //730 kifglobalthread.cxx 3701
"apply(string x): Apply the grammar to a string.", //731 kifglobalthread.cxx 3702
"load(string kifpathname,string featchar): Load a word file. Each line should contain one single word. featchar defines the character which starts features, it is optional.", //732 kifglobalthread.cxx 3704
"loadcompact(string kifpathname): Load a file stored in a compact format.", //733 kifglobalthread.cxx 3705
"loadlemma(string kifpathname): Load a word and lemma file. The even line contains a surface form and the odd lines the lemma and features.", //734 kifglobalthread.cxx 3706
"model(function): Set the model function which is used to traverse the automaton", //735 kifglobalthread.cxx 3707
"look(word): Traverses the automaton and returns the most probable readings accoding to the model function", //736 kifglobalthread.cxx 3708
"spotfinalstate(bool): Detect final states in automaton while in 'look'", //737 kifglobalthread.cxx 3709
"settransformationmap(map m): Set the transformation map to set the weights when traversing the automaton with editdistance", //738 kifglobalthread.cxx 3710
"store(string pathname): Store the file into a compact format", //739 kifglobalthread.cxx 3711
"size(): Returns the number of words stored in the automaton", //740 kifglobalthread.cxx 3712
"dump(string path): Dump the automaton content into a file", //741 kifglobalthread.cxx 3713
"setcodefeature(string character): Use this character to detect the features while adding words", //742 kifglobalthread.cxx 3714
"editdistance(string word,int threshold,int flags): Finds words whose edit distance to 'word' is inferior to the threshold. flags values are: 1: action on first character. 2: change character. 4: delete character. 8: insert character.", //743 kifglobalthread.cxx 3715
"clear(): Clears the automaton", //744 kifglobalthread.cxx 3716
"add(string word): add a word to the automaton. add(string word,string lemma): add a word and its lemma to the automaton.", //745 kifglobalthread.cxx 3717
"load(string kifpathname): Load a KiF program", //746 kifglobalthread.cxx 3719
"compile(string kifpathname): Compile a KiF program as a string. Return an handle on the first instruction to execute ", //747 kifglobalthread.cxx 3720
"run(int i): run a program from instruction i (returned by compile)", //748 kifglobalthread.cxx 3721
"kstart(): Return the declared variables as a map", //749 kifglobalthread.cxx 3722
"runasthread(int i): run a program from instruction i (returned by compile) in a thread", //750 kifglobalthread.cxx 3723
"runend(): Return true if the programme execution has been completed", //751 kifglobalthread.cxx 3724
"debugfunction(function,object): set the debug function that will be called during the execution", //752 kifglobalthread.cxx 3725
"debugclose(): Remove the debug mode initiated with debugfunction", //753 kifglobalthread.cxx 3726
"open(): open a KiF session", //754 kifglobalthread.cxx 3727
"clean(): close a KiF session", //755 kifglobalthread.cxx 3728
"name(): Return the pathname of the KiF file", //756 kifglobalthread.cxx 3729
"exposed(): Return the list of methods exposed by KiF", //757 kifglobalthread.cxx 3730
"_loader: A kif variable (of the type described here), which stores a pointer to the loading program.", //758 kifglobalthread.cxx 3731
""};


Exported const char* KIFMAPS[]={
"KIF(881): Error, wrong function arguments", //0 kifmaps.cxx 100
"KIF(689): Circular structure", //1 kifmaps.cxx 1164
""};


Exported const char* KIFPMAPS[]={
"KIF(689): Circular structure", //0 kifpmaps.cxx 762
"KIF(881): Error, wrong function arguments", //1 kifpmaps.cxx 10099
""};


Exported const char* KIFTREEMAPS[]={
"KIF(689): Circular structure", //0 treemaps.cxx 760
"KIF(881): Error, wrong function arguments", //1 treemaps.cxx 10035
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
"KIF(621): expected value should be a 'fmap' or a 'vector of vectors'", //11 kifautomaton.cxx 979
"KIF(635): Missing file name", //12 kifautomaton.cxx 1377
"KIF(636): Cannot store in compact format, number of symbols too large", //13 kifautomaton.cxx 1387
"alphabet", //14 kifautomaton.cxx 1390
"arcs", //15 kifautomaton.cxx 1398
"KIF(637): Cannot load compact format", //16 kifautomaton.cxx 1553
"KIF(411): Error after:<", //17 kifautomaton.cxx 1776
"GRM(002): Wrong negation or empty string.", //18 kifautomaton.cxx 1816
"GRM(001): Unknown rule label:", //19 kifautomaton.cxx 1851
"XIP(223): Cannot load the file", //20 kifautomaton.cxx 3320
""};


Exported const char* KIFCONTAINER[]={
"KIF(689): Circular structure", //0 kifcontainer.cxx 4089
"KIF(877): Missing root in the container", //1 kifcontainer.cxx 7288
"KIF(415): You cannot modify a tuple element", //2 kifcontainer.cxx 8128
"KIF(416): You cannot create an empty tuple", //3 kifcontainer.cxx 8135
"KIF(417): Expecting a container", //4 kifcontainer.cxx 8142
"addtowaiton", //5 kifcontainer.cxx 8787
"resize", //6 kifcontainer.cxx 8787
"count", //7 kifcontainer.cxx 8787
""};


Exported const char* KIFPREDICATE[]={
"PRE(010): Expecting a function", //0 kifpredicate.cxx 112
"PRE(009): You cannot instanciate a predicate this way", //1 kifpredicate.cxx 765
"PRE(001): Cannot instantiate a predicate with this value", //2 kifpredicate.cxx 771
"ponder", //3 kifpredicate.cxx 861
"asserta", //4 kifpredicate.cxx 864
"assertz", //5 kifpredicate.cxx 867
"assertdb", //6 kifpredicate.cxx 870
"retract", //7 kifpredicate.cxx 873
"retractdb", //8 kifpredicate.cxx 876
"name", //9 kifpredicate.cxx 1086
"PRE(003): Unknown predicate name", //10 kifpredicate.cxx 1241
"PRE(004): Missing index parameter in the map", //11 kifpredicate.cxx 1266
"PRE(005): Cannot build a predicate out of that object", //12 kifpredicate.cxx 1275
"unifiable", //13 kifpredicate.cxx 2123
"success:", //14 kifpredicate.cxx 2339
" --> Fail", //15 kifpredicate.cxx 2559
"PRE(006): Expecting a database object", //16 kifpredicate.cxx 3446
""};


Exported const char* KIFRAWSTRINGS[]={
"This wait state does not exist: '", //0 kifraw.cxx 279
"cursor", //1 kifraw.cxx 569
"cursormax", //2 kifraw.cxx 570
"garbage", //3 kifraw.cxx 571
"empty", //4 kifraw.cxx 572
"to_be_cleaned", //5 kifraw.cxx 573
"popping", //6 kifraw.cxx 574
"cursorstring", //7 kifraw.cxx 575
"cursorinteger", //8 kifraw.cxx 576
"cursorfloat", //9 kifraw.cxx 577
"garbagestring", //10 kifraw.cxx 578
"garbageinteger", //11 kifraw.cxx 579
"garbagefloat", //12 kifraw.cxx 580
"XTR(011): Missing parameters", //13 kifraw.cxx 956
" (line:%d)", //14 kifraw.cxx 1152
"KIF(831): Wrong opening or closing character in LISP", //15 kifraw.cxx 1177
"KIF(665): Expecting a container in STOKENIZE", //16 kifraw.cxx 1709
"KIF(550): The thread list has already been implemented.", //17 kifraw.cxx 2234
"KIF(551): Expecting threads as arguments.", //18 kifraw.cxx 2250
"KIF(552): This thread has already been inserted into the thread list", //19 kifraw.cxx 2263
"KIF(553): The thread list should be implemented with 'waiton' first.", //20 kifraw.cxx 2292
"KIF(654): Second parameter should be a vector in 'seeds'", //21 kifraw.cxx 2341
"KIF(510): base too large, max is: %d", //22 kifraw.cxx 3190
"KIF(511): Cannot analyze this string in this base.", //23 kifraw.cxx 3201
"KIF(801): Infinite value", //24 kifraw.cxx 3239
"KIF(802): Not a number", //25 kifraw.cxx 3242
"KIF(877): Expecting a variable", //26 kifraw.cxx 3813
"MATH(100): Error wrong fraction: denominator is 0", //27 kifraw.cxx 3978
"BITS(105): Wrong type of parameter in PartOf", //28 kifraw.cxx 4059
"KIF(543): Cannot intercept the current standard output", //29 kifraw.cxx 4096
"KIF(544): Cannot intercept the current error output", //30 kifraw.cxx 4113
"KIF(124):Incorrect format specifier or size too long", //31 kifraw.cxx 5794
"KIF(635): This function must be called with an automaton variable", //32 kifraw.cxx 6174
"XTR(010): Expecting a vector with an even number of values.", //33 kifraw.cxx 6400
"KIF(832): Argument cannot be processed in 'add' for 'automaton'.", //34 kifraw.cxx 6457
"KIF(644): The structure should be a map of maps", //35 kifraw.cxx 6491
"XTR(001): We expect the second argument to be a vector.", //36 kifraw.cxx 6738
"XTR(002): Wrong regular expression", //37 kifraw.cxx 6754
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
"break", //287 kiflibs.cxx 1491
"continue", //288 kiflibs.cxx 1494
"return", //289 kiflibs.cxx 1497
"_return", //290 kiflibs.cxx 1497
"_stacksize", //291 kiflibs.cxx 1580
"Missing parameter in '_stacksize'", //292 kiflibs.cxx 1592
"_joinedlimit", //293 kiflibs.cxx 1598
"Missing parameter in '_joinedlimit'", //294 kiflibs.cxx 1611
"_threadlimit", //295 kiflibs.cxx 1617
"Missing parameter in '_threadlimit'", //296 kiflibs.cxx 1628
"_mirrordisplay", //297 kiflibs.cxx 1634
"Missing parameter in '_mirrordisplay'", //298 kiflibs.cxx 1645
"_garbagesize", //299 kiflibs.cxx 1651
"Missing parameter in '_garbagesize'", //300 kiflibs.cxx 1666
"_garbagefunction", //301 kiflibs.cxx 1672
"Missing parameter in '_garbagefunction'", //302 kiflibs.cxx 1687
"_setenv", //303 kiflibs.cxx 1693
"Missing parameter in '_setenv'", //304 kiflibs.cxx 1695
"use", //305 kiflibs.cxx 1715
"Missing parameter in 'use'", //306 kiflibs.cxx 1765
"loadin", //307 kiflibs.cxx 1771
"Missing parameter in 'loadin'", //308 kiflibs.cxx 1798
"Unknown function call: ", //309 kiflibs.cxx 1885
"No compatible function found for:", //310 kiflibs.cxx 1959
"vector", //311 kiflibs.cxx 2017
"string", //312 kiflibs.cxx 2017
"Wrong interval for this variable:", //313 kiflibs.cxx 2018
" type is=", //314 kiflibs.cxx 2018
"this", //315 kiflibs.cxx 2413
"Wrong call with 'this'", //316 kiflibs.cxx 2415
"Invalid variable: '", //317 kiflibs.cxx 2505
"true", //318 kiflibs.cxx 2587
"Variable: '", //319 kiflibs.cxx 2670
"' has already been declared", //320 kiflibs.cxx 2672
"Common variable can only be declared in a frame: ", //321 kiflibs.cxx 2699
"Unknown type: '", //322 kiflibs.cxx 2808
"predicatevar", //323 kiflibs.cxx 3000
"private", //324 kiflibs.cxx 3307
"common", //325 kiflibs.cxx 3313
"const", //326 kiflibs.cxx 3316
"range", //327 kiflibs.cxx 3447
"ord", //328 kiflibs.cxx 3470
"Error: Already defined label: '", //329 kiflibs.cxx 3718
"store", //330 kiflibs.cxx 4039
"remove", //331 kiflibs.cxx 4042
"assertdb", //332 kiflibs.cxx 4156
"retractdb", //333 kiflibs.cxx 4164
"Error: Wrong assertdb or retractdb definition", //334 kiflibs.cxx 4176
"Error: Wrong assert or retract definition", //335 kiflibs.cxx 4184
"let", //336 kiflibs.cxx 4312
"' already declared", //337 kiflibs.cxx 4459
"and", //338 kiflibs.cxx 4714
"list", //339 kiflibs.cxx 4911
"dropWhile", //340 kiflibs.cxx 5200
"take", //341 kiflibs.cxx 5222
"drop", //342 kiflibs.cxx 5222
"filter", //343 kiflibs.cxx 5329
"repeat", //344 kiflibs.cxx 5674
"cycle", //345 kiflibs.cxx 5677
"Error: Predefined function, consider choosing another name: '", //346 kiflibs.cxx 5850
"Error: A function with this name already exists: '", //347 kiflibs.cxx 5858
"iterator", //348 kiflibs.cxx 6013
"Error: You cannot declare a function within a function: '", //349 kiflibs.cxx 6238
"joined", //350 kiflibs.cxx 6306
"protected", //351 kiflibs.cxx 6311
"exclusive", //352 kiflibs.cxx 6316
"strict", //353 kiflibs.cxx 6325
"polynomial", //354 kiflibs.cxx 6332
"thread", //355 kiflibs.cxx 6335
"autorun", //356 kiflibs.cxx 6342
"Error: This function has already been used in a call: '", //357 kiflibs.cxx 6368
"Error: Cannot find a matching function to a pre-declared function (check the declaration order): '", //358 kiflibs.cxx 6422
"Error: An AUTORUN cannot have parameters: '", //359 kiflibs.cxx 6440
"KIF(642): Error unknown extension", //360 kiflibs.cxx 6460
"KIF(643): Error unknown function extension", //361 kiflibs.cxx 6477
"Error: Unknown type: '", //362 kiflibs.cxx 6513
"self", //363 kiflibs.cxx 6531
"Error: Cannot use this name as a function extension: '", //364 kiflibs.cxx 6550
"Error: Cannot extend this type: '", //365 kiflibs.cxx 6591
"Error: attempt to use private frame:", //366 kiflibs.cxx 6626
"Error: This frame has already been declared:", //367 kiflibs.cxx 6651
"Cannot open kif file: ", //368 kiflibs.cxx 6788
"float f,g,h; int i,j,k; string s,t,u; vector v; map m;\n", //369 kiflibs.cxx 7069
"hence", //370 kiflibs.cxx 7091
" (line:%d)", //371 kiflibs.cxx 7100
"Error while reading KIF file: ", //372 kiflibs.cxx 7154
"Unknown file: ", //373 kiflibs.cxx 7156
"Unknown file:", //374 kiflibs.cxx 7710
"a 'self' object can only call functions: ", //375 kiflibs.cxx 8101
"partof", //376 kiflibs.cxx 8108
"Only functions can be called from an index: ", //377 kiflibs.cxx 8130
"methods", //378 kiflibs.cxx 8532
"map", //379 kiflibs.cxx 8534
"_current", //380 kiflibs.cxx 8598
"a_first", //381 kiflibs.cxx 8643
"a_change", //382 kiflibs.cxx 8648
"a_delete", //383 kiflibs.cxx 8653
"a_insert", //384 kiflibs.cxx 8658
"a_last", //385 kiflibs.cxx 8663
"a_switch", //386 kiflibs.cxx 8668
"a_prefix", //387 kiflibs.cxx 8673
"a_nocase", //388 kiflibs.cxx 8678
"a_surface", //389 kiflibs.cxx 8683
"a_full", //390 kiflibs.cxx 8688
"a_split", //391 kiflibs.cxx 8693
"a_skip", //392 kiflibs.cxx 8698
"a_track", //393 kiflibs.cxx 8703
"pred_none", //394 kiflibs.cxx 8708
"pred_weight", //395 kiflibs.cxx 8713
"pred_weightstack", //396 kiflibs.cxx 8718
"pred_normalize", //397 kiflibs.cxx 8723
"pred_randomize", //398 kiflibs.cxx 8728
"_KIFMAIN", //399 kiflibs.cxx 8885
"FRAME::", //400 kiflibs.cxx 8908
"FUNCTION::", //401 kiflibs.cxx 8930
"h for help", //402 kiflibs.cxx 9043
"End", //403 kiflibs.cxx 9175
"$: stop debugging", //404 kiflibs.cxx 9213
"$$: termination", //405 kiflibs.cxx 9214
"o: leave current function", //406 kiflibs.cxx 9215
"i: inside function", //407 kiflibs.cxx 9216
"f: File indexes", //408 kiflibs.cxx 9217
"g line (file)|label: goto line", //409 kiflibs.cxx 9218
"b line (file)|label: breakpoints (if line==-1: deletes all breakpoints. Without parameter: display breakpoints)", //410 kiflibs.cxx 9219
"s: stack", //411 kiflibs.cxx 9220
"n (nb lines): list following instructions", //412 kiflibs.cxx 9221
"l f t: list line from f to t", //413 kiflibs.cxx 9222
"w variable: add a watch on a variable", //414 kiflibs.cxx 9223
"r variable: remove a watch on a variable", //415 kiflibs.cxx 9224
"H: command history", //416 kiflibs.cxx 9225
"!nb: command renacted", //417 kiflibs.cxx 9226
"<: down in history", //418 kiflibs.cxx 9227
">: up in history", //419 kiflibs.cxx 9228
"w", //420 kiflibs.cxx 9252
"r", //421 kiflibs.cxx 9273
"Unknown label: ", //422 kiflibs.cxx 9302
"Line:", //423 kiflibs.cxx 9321
" in ", //424 kiflibs.cxx 9321
"All breakpoints deleted", //425 kiflibs.cxx 9330
"Breakpoint cleaned", //426 kiflibs.cxx 9349
"Instruction", //427 kiflibs.cxx 9377
"line %d in %s", //428 kiflibs.cxx 9383
"Unknown variable", //429 kiflibs.cxx 9447
"null", //430 kiflibs.cxx 9462
""};


Exported const char* KIFSPARSESTRINGS[]={
"SPM(100): Wrong index", //0 kifsparse.cxx 771
"MAT(131): Wrong index or Wrong value (recipient should be a map or a mvector)", //1 kifsparse.cxx 851
"name", //2 kifsparse.cxx 968
"size", //3 kifsparse.cxx 982
"MAT(209): Matrix size mismatch", //4 kifsparse.cxx 1201
"MAT(203): Cannot divide by 0", //5 kifsparse.cxx 1238
"MAT(202): Cannot divide with this matrix", //6 kifsparse.cxx 1258
"Incompatible version of KiF", //7 kifsparse.cxx 2545
"matrix", //8 kifsparse.cxx 2554
"imatrix", //9 kifsparse.cxx 2558
"transpose", //10 kifsparse.cxx 2576
"transpose(): return the transposed matrix", //11 kifsparse.cxx 2576
"dimension", //12 kifsparse.cxx 2577
"dimension(): return the matrix size.\ndimension(int rowsize,int columnsize): set the matrix size.", //13 kifsparse.cxx 2577
"determinant", //14 kifsparse.cxx 2578
"determinant(): return the matrix determinant", //15 kifsparse.cxx 2578
"sum", //16 kifsparse.cxx 2579
"sum(): return the sum of all elements", //17 kifsparse.cxx 2579
"product", //18 kifsparse.cxx 2580
"product(): return the product of all elements", //19 kifsparse.cxx 2580
"invert", //20 kifsparse.cxx 2581
"invert(): Return the inverted matrix.", //21 kifsparse.cxx 2581
""};


Exported const char* KIFSOCKETSTRINGS[]={
"No error", //0 kifsocket.cxx 98
"size", //1 kifsocket.cxx 249
"SOCKET(856): Socket error", //2 kifsocket.cxx 417
"SOCKET(851): Cannot find host by name on this machine", //3 kifsocket.cxx 427
"SOCKET(831): Server already launched on this socket", //4 kifsocket.cxx 453
"SOCKET(850): Wrong server name", //5 kifsocket.cxx 466
"SOCKET(872): Error on reuse addess", //6 kifsocket.cxx 482
"SOCKET(853): Error on bind ", //7 kifsocket.cxx 490
"SOCKET(854): Error on listen ", //8 kifsocket.cxx 496
"SOCKET(857): Error on connection ", //9 kifsocket.cxx 542
"SOCKET(855): Error on read ", //10 kifsocket.cxx 556
"SOCKET(824): Non connected socket", //11 kifsocket.cxx 610
"SOCKET(888): Reading connection refused ", //12 kifsocket.cxx 636
"SOCKET(830): Time out ", //13 kifsocket.cxx 643
"SOCKET(889): Writing connection refused ", //14 kifsocket.cxx 663
"SOCKET(820): Missing parameter in read", //15 kifsocket.cxx 688
"SOCKET(858): No client connected", //16 kifsocket.cxx 700
"SOCKET(861): Error on read", //17 kifsocket.cxx 711
"Reading:", //18 kifsocket.cxx 721
"SOCKET(825): RUN can only be launched from a server", //19 kifsocket.cxx 750
"SOCKET(870): Object '", //20 kifsocket.cxx 816
"' unknown ", //21 kifsocket.cxx 818
"SOCKET(871): Function '", //22 kifsocket.cxx 874
"SOCKET(822): Missing parameter in write", //23 kifsocket.cxx 1011
"Writing:", //24 kifsocket.cxx 1034
"SOCKET(859): Error sending", //25 kifsocket.cxx 1040
"SOCKET(821): Missing parameter in receive", //26 kifsocket.cxx 1089
"SOCKET(860): Error on RECEIVE ", //27 kifsocket.cxx 1108
"SOCKET(823): Too many parameter in receive", //28 kifsocket.cxx 1145
"SOCKET(860): Error on RECEIVE", //29 kifsocket.cxx 1167
"SOCKET(827): Missing parameter in send", //30 kifsocket.cxx 1208
"port", //31 kifsocket.cxx 1265
"address", //32 kifsocket.cxx 1267
"SOCKET(852): You cannot use GETPEERNAME on the client side", //33 kifsocket.cxx 1272
"name", //34 kifsocket.cxx 1332
"SOCKET(812): Unknown method", //35 kifsocket.cxx 1418
"Incompatible version of KiF", //36 kifsocket.cxx 1514
"socket", //37 kifsocket.cxx 1524
"remote", //38 kifsocket.cxx 1529
"createserver", //39 kifsocket.cxx 1548
"createserver(string hostname,int port,int nblients: create a server, if hostname is omitted, then the local hostname is used", //40 kifsocket.cxx 1548
"connect", //41 kifsocket.cxx 1549
"connect(string hostname,int port): connect to the server", //42 kifsocket.cxx 1549
"wait", //43 kifsocket.cxx 1550
"wait(): wait for a client to connect and returns its socket id", //44 kifsocket.cxx 1550
"read", //45 kifsocket.cxx 1551
"read(int num): read a string on a socket. On the server side, 'num' is the value returned by 'wait()'. Use 'read()' on the client side", //46 kifsocket.cxx 1551
"run", //47 kifsocket.cxx 1552
"run(int num,string stop): Only on the server side: execute remote method invocations for the client 'num'. 'stop' is the string that can be used to stop the server by the client.", //48 kifsocket.cxx 1552
"write", //49 kifsocket.cxx 1553
"write(int num,string s): write the string s on the socket. On the server side, num is the value returned by wait()'. Use 'write(string s)' on the client side", //50 kifsocket.cxx 1553
"receive", //51 kifsocket.cxx 1554
"receive(int num,int nb): read a string on a socket in a non KiF environment. On the server side, 'num' is the value returned by 'wait()'. Use 'receive()' on the client side", //52 kifsocket.cxx 1554
"get", //53 kifsocket.cxx 1555
"get(int num): get one character from a socket in a non KiF environment. On the server side, 'num' is the value returned by 'wait()'. Use 'get()' on the client side", //54 kifsocket.cxx 1555
"send", //55 kifsocket.cxx 1556
"send(int num,string s): write the string s on the socket in a non KiF environment. On the server side, num is the value returned by wait()'. Use 'send(string s)' on the client side", //56 kifsocket.cxx 1556
"close", //57 kifsocket.cxx 1557
"close(int num): Close a socket. On the server side, if 'num' is provided (it is the value returned by wait()) it closes the client socket, otherwise it closes the current socket.", //58 kifsocket.cxx 1557
"blocking", //59 kifsocket.cxx 1558
"blocking(bool flag): if 'flag' is true, the socket works in 'blocking' mode, otherwise in 'non blocking' mode", //60 kifsocket.cxx 1558
"settimeout", //61 kifsocket.cxx 1559
"settimeout(int t): Set a time out of 't' seconds on the socket", //62 kifsocket.cxx 1559
"gethostname", //63 kifsocket.cxx 1560
"gethostname(): return the current host name", //64 kifsocket.cxx 1560
"port(): return the current port number", //65 kifsocket.cxx 1561
"getpeername", //66 kifsocket.cxx 1562
"getpeername(int num): return the current peer name on the socket 'num'", //67 kifsocket.cxx 1562
"getframe", //68 kifsocket.cxx 1563
"getframe(string frame): return a frame object handle declared on the server side", //69 kifsocket.cxx 1563
"getfunction", //70 kifsocket.cxx 1564
"getfunction(string frame): return a function handle declared on the server side", //71 kifsocket.cxx 1564
"myvariable", //72 kifsocket.cxx 1570
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
"W", //274 kifltk.cxx 2291
"WND(657): Unknown font code", //275 kifltk.cxx 2330
"WND(608): Wrong number of parameters=expected 5 in a complex shape construction", //276 kifltk.cxx 2358
"WND(609): Wrong number of parameters=expected 6 in a regular construction", //277 kifltk.cxx 2363
"WND(612): Cannot call this method here. Have you called beginpoints before?", //278 kifltk.cxx 2522
"WND(612): Cannot call this method here. Have you called beginloop before?", //279 kifltk.cxx 2568
"WND(612): Cannot call this method here. Have you called beginpolygon before?", //280 kifltk.cxx 2593
"WND(612): Cannot call this method here. Have you called begincomplexpolygon before?", //281 kifltk.cxx 2618
"WND(612): Cannot call this method here. Have you called beginline before?", //282 kifltk.cxx 2656
"WND(610): You cannot use this function outside a complex shape construction", //283 kifltk.cxx 2670
"WND(100): Cannot recreate an existing window", //284 kifltk.cxx 2894
"WND(431): Unavailable font id", //285 kifltk.cxx 2923
"WND(109): Wrong element, the first parameter should be a function", //286 kifltk.cxx 2966
"WND(102): Window already instantiated", //287 kifltk.cxx 3174
"WND(341): Unknown mouse action", //288 kifltk.cxx 3190
"WND(342): Wrong parameter in your mouse callback", //289 kifltk.cxx 3196
"WND(344): Unknown keyboard action", //290 kifltk.cxx 3223
"WND(343): Wrong parameter in your keyboard callback", //291 kifltk.cxx 3230
"WND(872): We expect a vector as first parameter.", //292 kifltk.cxx 3286
"WND(871): The vector should contain an even number of elements.", //293 kifltk.cxx 3289
"WND(873): We expect a vector as third parameter", //294 kifltk.cxx 3310
"WND(862): Wrong values to compute coordinates", //295 kifltk.cxx 3421
"WND(167): Unknown image", //296 kifltk.cxx 3499
"WND(168): Unloaded image", //297 kifltk.cxx 3507
"WND(169): Unknown bitmap", //298 kifltk.cxx 3527
"WND(109): Wrong parameter, vector expected", //299 kifltk.cxx 3596
"WND(109): Wrong element, vector expected", //300 kifltk.cxx 3605
"WND(109): Wrong element, missing field in a sub-menu", //301 kifltk.cxx 3632
"WND(109): Wrong element, second field in a sub-menu should be a vector", //302 kifltk.cxx 3637
"WND(109): Wrong element, third field in a sub-menu should be a function", //303 kifltk.cxx 3654
"WND(123): Operation not available", //304 kifltk.cxx 3820
"WND(134): Widget has not been created yet", //305 kifltk.cxx 3832
"WND(133): Widget expected as parameter", //306 kifltk.cxx 3837
"button", //307 kifltk.cxx 4105
"wheelx", //308 kifltk.cxx 4106
"wheely", //309 kifltk.cxx 4107
"xroot", //310 kifltk.cxx 4110
"yroot", //311 kifltk.cxx 4111
"WND(101): input already created", //312 kifltk.cxx 4365
"WND(677): Input not initialized", //313 kifltk.cxx 4391
"WND(101): editor already created", //314 kifltk.cxx 4987
"KIF(881): Error, wrong function arguments", //315 kifltk.cxx 5001
"WND(101): No editor available", //316 kifltk.cxx 5014
"WND(342): Wrong parameter in your key callback", //317 kifltk.cxx 5143
"EDT(768): Wrong number of elements in the vector... 3 or 4 expected", //318 kifltk.cxx 5152
"WND(627): Maximum number of styles reached", //319 kifltk.cxx 5221
"WND(676): Editor not initialized", //320 kifltk.cxx 5266
"EDT(766): Wrong input: map expected", //321 kifltk.cxx 5273
"EDT(770): Wrong input: empty map", //322 kifltk.cxx 5282
"EDT(767): Missing '#' key in the map. '#' is the initial key and is mandatory", //323 kifltk.cxx 5301
"EDT(767): Wrong input: vector expected", //324 kifltk.cxx 5304
"WND(689): Out of range", //325 kifltk.cxx 5531
"WND(679): Style table not initialized", //326 kifltk.cxx 5564
"WND(679): Unknown style key", //327 kifltk.cxx 5575
"start", //328 kifltk.cxx 5844
"end", //329 kifltk.cxx 5845
"cursor", //330 kifltk.cxx 6407
"WND(101): output already created", //331 kifltk.cxx 6609
"WND(679): Output not initialized", //332 kifltk.cxx 6631
"WND(101): button already created", //333 kifltk.cxx 6946
"WND(675): Unknwown button shape: ", //334 kifltk.cxx 6954
"WND(675): Unknwown button type: ", //335 kifltk.cxx 6974
"WND(805): Button not initialized", //336 kifltk.cxx 7023
"WND(809): Image button required", //337 kifltk.cxx 7025
"WND(813): image object required", //338 kifltk.cxx 7029
"WND(654): Unknown action", //339 kifltk.cxx 7104
"WND(656): Unknown key code", //340 kifltk.cxx 7130
"WND(101): box already created", //341 kifltk.cxx 7319
"WND(102): box not created", //342 kifltk.cxx 7336
"WND(767): Unknown type:", //343 kifltk.cxx 7341
"WND(101): group already created", //344 kifltk.cxx 7525
"WND(101): group not created", //345 kifltk.cxx 7547
"WND(507): This group does not belong to a tab structure", //346 kifltk.cxx 7551
"WND(101): tabs already created", //347 kifltk.cxx 7750
"WND(101): tabs not created", //348 kifltk.cxx 7768
"WND(800): No active tab", //349 kifltk.cxx 7782
"WND(506): Empty group", //350 kifltk.cxx 7792
"WND(507): This group does not belong to that tab", //351 kifltk.cxx 7794
"WND(105): You can only add wgroup object", //352 kifltk.cxx 7817
"WND(800): Wrong number of parameters", //353 kifltk.cxx 7823
"WND(505): You can only remove wgroup object", //354 kifltk.cxx 7841
"WND(101): table already created", //355 kifltk.cxx 8157
"WND(102): table has not been created yet", //356 kifltk.cxx 8190
"WND(678): table not initialized", //357 kifltk.cxx 8400
"WND(102): table not created", //358 kifltk.cxx 8477
"WND(805): table not created", //359 kifltk.cxx 8500
"top", //360 kifltk.cxx 8631
"left", //361 kifltk.cxx 8632
"bottom", //362 kifltk.cxx 8633
"right", //363 kifltk.cxx 8634
"values", //364 kifltk.cxx 8636
"WND(100): Cannot recreate an existing scroll", //365 kifltk.cxx 8753
"WND(101): No scroll available", //366 kifltk.cxx 8777
"WND(100): Cannot recreate an existing slider", //367 kifltk.cxx 8941
"WND(101): No slider available", //368 kifltk.cxx 8975
"WND(206): Wrong slider type", //369 kifltk.cxx 9029
"WND(100): Cannot recreate an existing choice", //370 kifltk.cxx 9261
"WND(678): choice not initialized", //371 kifltk.cxx 9283
"WND(100): Cannot recreate an existing browser", //372 kifltk.cxx 9586
"WND(121): Browser widget has not been created yet", //373 kifltk.cxx 9609
"WND(094): You can only instanciate a wtreeitem with another wtreeitem", //374 kifltk.cxx 9827
"WND(117): Unknown tree item", //375 kifltk.cxx 9869
"WND(118): Wrong child index", //376 kifltk.cxx 10085
"WND(210): Cannot recreate an existing wcounter", //377 kifltk.cxx 10268
"WND(211): No wcounter available", //378 kifltk.cxx 10291
"WND(220): Cannot recreate an existing wprogress bar", //379 kifltk.cxx 10603
"WND(221): No wprogress bar available", //380 kifltk.cxx 10625
"WND(100): Cannot recreate an existing wtree", //381 kifltk.cxx 10853
"WND(121): wtree widget has not been created yet", //382 kifltk.cxx 10876
"WND(125): Empty label, cannot create wtree widget", //383 kifltk.cxx 10899
"WND(341): wrong type for the first element", //384 kifltk.cxx 10962
"WND(100): Cannot recreate an existing filebrowser", //385 kifltk.cxx 11518
"WND(101): File browser not initialized", //386 kifltk.cxx 11557
"Incompatible version of KiF", //387 kifltk.cxx 12087
"window", //388 kifltk.cxx 12096
"box", //389 kifltk.cxx 12101
"wtabs", //390 kifltk.cxx 12106
"wgroup", //391 kifltk.cxx 12111
"wchoice", //392 kifltk.cxx 12116
"editor", //393 kifltk.cxx 12126
"winput", //394 kifltk.cxx 12131
"wtable", //395 kifltk.cxx 12136
"woutput", //396 kifltk.cxx 12141
"scroll", //397 kifltk.cxx 12146
"slider", //398 kifltk.cxx 12151
"wcounter", //399 kifltk.cxx 12156
"wprogress", //400 kifltk.cxx 12161
"image", //401 kifltk.cxx 12166
"bitmap", //402 kifltk.cxx 12171
"filebrowser", //403 kifltk.cxx 12176
"browser", //404 kifltk.cxx 12181
"wtreeitem", //405 kifltk.cxx 12186
"wtree", //406 kifltk.cxx 12191
"create", //407 kifltk.cxx 12209
"create(int x,int y,int w, int h,string title): Create a window without widgets, w and h are optional", //408 kifltk.cxx 12209
"begin", //409 kifltk.cxx 12210
"begin(int x,int y,int w, int h,string title): Create a window and begin initialisation, w and h are optional", //410 kifltk.cxx 12210
"end(): end creation", //411 kifltk.cxx 12211
"run", //412 kifltk.cxx 12212
"run(): Launch the GUI", //413 kifltk.cxx 12212
"close", //414 kifltk.cxx 12213
"close(): close the window", //415 kifltk.cxx 12213
"onclose", //416 kifltk.cxx 12214
"onclose(function,object): Callback to call on close", //417 kifltk.cxx 12214
"ontime", //418 kifltk.cxx 12215
"ontime(function,t,object): Set a time out function", //419 kifltk.cxx 12215
"initializefonts", //420 kifltk.cxx 12217
"initializefonts(): load fonts from system. Return the number of available fonts", //421 kifltk.cxx 12217
"getfont", //422 kifltk.cxx 12218
"getfont(int num): get font name.", //423 kifltk.cxx 12218
"getfontsizes", //424 kifltk.cxx 12219
"getfontsizes(int num): return a vector of available font sizes.", //425 kifltk.cxx 12219
"fontnumber", //426 kifltk.cxx 12220
"fontnumber(): Return the number of available fonts.", //427 kifltk.cxx 12220
"plot", //428 kifltk.cxx 12221
"plot(fvector xy,int thickness,fvector landmarks): Plot a graph from a table of successive x,y points according to window size. If thickness===0 then points are continuously plotted, else defines the diameter of the point. Return a float vector which is used with plotcoords. The landmark vector is optional, it is has the following structure: [XmaxWindow,YmaxWindow,XminValue,YminValue,XmaxValue,YmaxValue,incX,incY]. incX,incY are also optional.", //429 kifltk.cxx 12221
"plotcoords", //430 kifltk.cxx 12222
"plotcoords(fvector inits,float x,float y): Compute the coordinates of a point(x,y) according to the previous scale computed with plot. Returns a vector of two elements [xs,ys] corresponding to the screen coordinates in the current window.", //431 kifltk.cxx 12222
"ask", //432 kifltk.cxx 12224
"ask(string msg,string buttonmsg1,string buttonmsg2,string buttonmsg3): Pop up window to pose a question", //433 kifltk.cxx 12224
"alert", //434 kifltk.cxx 12225
"alert(string msg): Pop up window to display an alert", //435 kifltk.cxx 12225
"password", //436 kifltk.cxx 12226
"password(string msg): Pop up window to input a password", //437 kifltk.cxx 12226
"get(string msg): Pop up window to get a value from the user", //438 kifltk.cxx 12227
"menu", //439 kifltk.cxx 12228
"menu(vector,int x,int y,int w, int h): initialize a menu with its callback functions", //440 kifltk.cxx 12228
"resizable", //441 kifltk.cxx 12229
"resizable(object): make the object resizable", //442 kifltk.cxx 12229
"sizerange", //443 kifltk.cxx 12230
"sizerange(int minw,int minh, int maxw,int maxh): define range in which the size of the window can evolve", //444 kifltk.cxx 12230
"modal", //445 kifltk.cxx 12231
"modal(bool b): If true make the window modal. If no parameter return if the window is modal", //446 kifltk.cxx 12231
"border", //447 kifltk.cxx 12232
"border(bool b): If true add or remove borders. If no parameter return if the window has borders", //448 kifltk.cxx 12232
"backgroundcolor", //449 kifltk.cxx 12233
"backgroundcolor(int c|string c): set the background color", //450 kifltk.cxx 12233
"drawcolor", //451 kifltk.cxx 12235
"drawcolor(int c|string c): set the color for the next drawings", //452 kifltk.cxx 12235
"rectangle", //453 kifltk.cxx 12236
"rectangle(int x,int y,int w, int h, string c|int c): Draw a rectangle with optional color c", //454 kifltk.cxx 12236
"vertex", //455 kifltk.cxx 12237
"vertex(float x,float y): Add a vertex to a complex structure", //456 kifltk.cxx 12237
"rectanglefill", //457 kifltk.cxx 12238
"rectanglefill(int x,int y,int w, int h, string c|int c): Fill a rectangle with optional color c", //458 kifltk.cxx 12238
"lineshape", //459 kifltk.cxx 12239
"lineshape(string type,int width): Select the line shape and its thikness", //460 kifltk.cxx 12239
"polygon", //461 kifltk.cxx 12240
"polygon(int x,int y,int x1, int y1,int x2, int y2, int x3, int y3): Draw a polygon, x3 and y3 are optional", //462 kifltk.cxx 12240
"loop", //463 kifltk.cxx 12241
"loop(int x,int y,int x1, int y1,int x2, int y2, int x3, int y3): Draw a series of lines, x3 and y3 are optional", //464 kifltk.cxx 12241
"line", //465 kifltk.cxx 12242
"line(int x,int y,int x1, int y1,int x2, int y2): Draw a line between points, x2 and y2 are optional", //466 kifltk.cxx 12242
"rotation", //467 kifltk.cxx 12243
"rotation(float x,float y,float distance, float angle, bool draw): Compute the coordinate of a rotated point from point x,y, using a distance and an angle. Return a vector of floats with the new coordinates.", //468 kifltk.cxx 12243
"pushmatrix", //469 kifltk.cxx 12245
"pushmatrix(): Save the current transformation", //470 kifltk.cxx 12245
"popmatrix", //471 kifltk.cxx 12246
"popmatrix(): Restore the current transformation", //472 kifltk.cxx 12246
"scale", //473 kifltk.cxx 12247
"scale(float x,float y)|scale(float x): Scale the current transformation", //474 kifltk.cxx 12247
"translate", //475 kifltk.cxx 12248
"translate(float x,float y): translate the current transformation", //476 kifltk.cxx 12248
"rotate", //477 kifltk.cxx 12249
"rotate(float d): rotate of degree d the current transformation", //478 kifltk.cxx 12249
"multmatrix", //479 kifltk.cxx 12250
"multmatrix(float a, float b, float c, float d, float x, float y): combine transformations", //480 kifltk.cxx 12250
"transformx", //481 kifltk.cxx 12252
"transformx(float x, float y): Transform a coordinate X using the current transformation matrix.", //482 kifltk.cxx 12252
"transformy", //483 kifltk.cxx 12253
"transformy(float x, float y): Transform a coordinate Y using the current transformation matrix.", //484 kifltk.cxx 12253
"transformdx", //485 kifltk.cxx 12254
"transformdx(float x, float y): Transform a distance DX using the current transformation matrix.", //486 kifltk.cxx 12254
"transformdy", //487 kifltk.cxx 12255
"transformdy(float x, float y): Transform a distance DY using the current transformation matrix.", //488 kifltk.cxx 12255
"transformedvertex", //489 kifltk.cxx 12256
"transformedvertex(float x, float y): add transformations to vertices list.", //490 kifltk.cxx 12256
"beginpoints", //491 kifltk.cxx 12258
"beginpoints(): Start accumulating vertices", //492 kifltk.cxx 12258
"endpoints", //493 kifltk.cxx 12259
"endpoints(): Stop accumulating vertices", //494 kifltk.cxx 12259
"beginline", //495 kifltk.cxx 12261
"beginline(): Start drawing lines.", //496 kifltk.cxx 12261
"endline", //497 kifltk.cxx 12262
"endline(): End drawing lines.", //498 kifltk.cxx 12262
"beginloop", //499 kifltk.cxx 12264
"beginloop(): Start drawing a closed sequence of lines.", //500 kifltk.cxx 12264
"endloop", //501 kifltk.cxx 12265
"endloop(): End drawing a closed sequence of lines.", //502 kifltk.cxx 12265
"beginpolygon", //503 kifltk.cxx 12267
"beginpolygon(): Start drawing a convex filled polygon.", //504 kifltk.cxx 12267
"endpolygon", //505 kifltk.cxx 12268
"endpolygon(): End drawing a convex filled polygon.", //506 kifltk.cxx 12268
"begincomplexpolygon", //507 kifltk.cxx 12270
"begincomplexpolygon(): Start drawing a complex filled polygon.", //508 kifltk.cxx 12270
"gap", //509 kifltk.cxx 12271
"gap(): Fill the gap while drawing a complex polygon.", //510 kifltk.cxx 12271
"endcomplexpolygon", //511 kifltk.cxx 12272
"endcomplexpolygon(): End drawing a complex filled polygon.", //512 kifltk.cxx 12272
"arc", //513 kifltk.cxx 12275
"arc(int x,int y,int x1, int y1, float a1, float a2): Draw an arc.\rarc(float x,float y,float rad,float a1,float a2): Add a series of points to the current path on the arc of a circle;", //514 kifltk.cxx 12275
"curve", //515 kifltk.cxx 12276
"curve(float x,float y,float x1,float y1,float x1,float y2,float x3,float y3): Add a series of points on a Bezier curve to the path. The curve ends (and two of the points) are at x,y and x3,y3.", //516 kifltk.cxx 12276
"pushclip", //517 kifltk.cxx 12277
"pushclip(int x,int y,int w, int h): Insert a clip region, with the following coordinates", //518 kifltk.cxx 12277
"popclip", //519 kifltk.cxx 12278
"popclip(): Release a clip region", //520 kifltk.cxx 12278
"pie", //521 kifltk.cxx 12280
"pie(int x,int y,int x1, int y1, float a1, float a2): Draw a pie", //522 kifltk.cxx 12280
"point", //523 kifltk.cxx 12281
"point(int x,int y): Draw a pixel", //524 kifltk.cxx 12281
"circle", //525 kifltk.cxx 12282
"circle(int x,int y,int r,int color): Draw a circle. 'color' is optional.", //526 kifltk.cxx 12282
"textsize", //527 kifltk.cxx 12284
"textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels", //528 kifltk.cxx 12284
"drawtext", //529 kifltk.cxx 12285
"drawtext(string l,int x,int y): Put a text at position x,y", //530 kifltk.cxx 12285
"redraw", //531 kifltk.cxx 12286
"redraw(): Redraw the window", //532 kifltk.cxx 12286
"rgbcolor", //533 kifltk.cxx 12287
"rgbcolor(string|int color)|(int r,int g,int b)|(vector color): either return a vector of the color decomposition or return the value of rgb values combined", //534 kifltk.cxx 12287
"font", //535 kifltk.cxx 12289
"font(string f,int sz): Set the font name and its size", //536 kifltk.cxx 12289
"ondragdrop", //537 kifltk.cxx 12290
"ondragdrop(function,object): Set the call back function for drag and drop action.", //538 kifltk.cxx 12290
"onmouse", //539 kifltk.cxx 12291
"onmouse(int action, function,object): Set the call back function on a mouse action with a given object as parameter", //540 kifltk.cxx 12291
"onkey", //541 kifltk.cxx 12292
"onkey(int action, function,object): Set the call back function on a keyboard action with a given object as parameter", //542 kifltk.cxx 12292
"cursorstyle", //543 kifltk.cxx 12293
"cursorstyle(int cursortype, int color,int color): Set the cursor shape", //544 kifltk.cxx 12293
"hide", //545 kifltk.cxx 12294
"hide(bool h): Hide the window if h is true", //546 kifltk.cxx 12294
"show", //547 kifltk.cxx 12295
"show(): Show the window", //548 kifltk.cxx 12295
"focus", //549 kifltk.cxx 12296
"focus(): Get the focus", //550 kifltk.cxx 12296
"flush", //551 kifltk.cxx 12297
"flush(): force redrawing of window", //552 kifltk.cxx 12297
"position", //553 kifltk.cxx 12298
"position()|(int x,int y): Return the window position or set the window position", //554 kifltk.cxx 12298
"size()|(int x,int y,int w, int h): Return the window size or set the window size", //555 kifltk.cxx 12299
"awake", //556 kifltk.cxx 12300
"awake(): Awake a threaded window", //557 kifltk.cxx 12300
"lock", //558 kifltk.cxx 12301
"lock(): Thread lock", //559 kifltk.cxx 12301
"unlock", //560 kifltk.cxx 12302
"unlock(): Thread unlock", //561 kifltk.cxx 12302
"image(image image,int x, int y, int w, int h): Display an image", //562 kifltk.cxx 12303
"bitmap(bitmap image,int color,int x, int y, int w, int h): Display a bitmap", //563 kifltk.cxx 12304
"labelsize", //564 kifltk.cxx 12305
"labelsize(int c): set or return the label font size", //565 kifltk.cxx 12305
"labelfont", //566 kifltk.cxx 12306
"labelfont(int c): set or return the label font", //567 kifltk.cxx 12306
"labelcolor", //568 kifltk.cxx 12307
"labelcolor(int c): set or return the label color", //569 kifltk.cxx 12307
"labeltype", //570 kifltk.cxx 12308
"labeltype(int c): set or return the label type", //571 kifltk.cxx 12308
"label", //572 kifltk.cxx 12309
"label(string s): set or return the label text", //573 kifltk.cxx 12309
"selectioncolor", //574 kifltk.cxx 12310
"selectioncolor(int color): Color for the selected elements", //575 kifltk.cxx 12310
"coords", //576 kifltk.cxx 12311
"coords(int c): return a vector of the widget coordinates", //577 kifltk.cxx 12311
"created", //578 kifltk.cxx 12312
"created(): Return true if the object has been created", //579 kifltk.cxx 12312
"copy", //580 kifltk.cxx 12313
"copy(string): Copy string to clipboard", //581 kifltk.cxx 12313
"paste", //582 kifltk.cxx 12314
"paste(): paste string", //583 kifltk.cxx 12314
"align", //584 kifltk.cxx 12315
"align(int): define the label alignment", //585 kifltk.cxx 12315
"create(int x,int y,int w,int h,string label): Create a scroll", //586 kifltk.cxx 12317
"resize", //587 kifltk.cxx 12318
"resize(object): make the object resizable", //588 kifltk.cxx 12318
"show(): Show the widget", //589 kifltk.cxx 12319
"hide(): Hide the widget", //590 kifltk.cxx 12321
"tooltip", //591 kifltk.cxx 12327
"tooltip(string msg): Add a tooltip message to the widget", //592 kifltk.cxx 12327
"create(int x,int y,int w,int h,string label): Create a wcounter", //593 kifltk.cxx 12335
"bounds", //594 kifltk.cxx 12336
"bounds(float x,float y): defines the wcounter boundary", //595 kifltk.cxx 12336
"type", //596 kifltk.cxx 12337
"type(bool normal): if 'true' then normal wcounter or simple wcounter", //597 kifltk.cxx 12337
"step", //598 kifltk.cxx 12338
"step(double): define the wcounter step", //599 kifltk.cxx 12338
"lstep", //600 kifltk.cxx 12339
"lstep(double): define the large wcounter step", //601 kifltk.cxx 12339
"steps", //602 kifltk.cxx 12340
"steps(double): define the wcounter steps, normal and large.", //603 kifltk.cxx 12340
"value", //604 kifltk.cxx 12341
"value(float): define the value for the wcounter or return its value", //605 kifltk.cxx 12341
"font(int s): set or return the text font", //606 kifltk.cxx 12357
"textcolor", //607 kifltk.cxx 12358
"textcolor(string code|int code): Set the color of the text", //608 kifltk.cxx 12358
"create(int x,int y,int w,int h,string label): Create a progress bar", //609 kifltk.cxx 12360
"minimum", //610 kifltk.cxx 12361
"minimum(float x): defines the minimum or return the minimum", //611 kifltk.cxx 12361
"maximum", //612 kifltk.cxx 12362
"maximum(float x): defines the maximum or return the maximum", //613 kifltk.cxx 12362
"value(float): define the value for the progress bar or return its value", //614 kifltk.cxx 12363
"barcolor", //615 kifltk.cxx 12368
"barcolor(int color): Color for the progression bar", //616 kifltk.cxx 12368
"create(int x,int y,int w,int h,int alignment,bool valueslider,string label): Create a slider or a valueslider", //617 kifltk.cxx 12374
"bounds(int x,int y): defines the slider boundary", //618 kifltk.cxx 12376
"type(int x): Value slider type", //619 kifltk.cxx 12377
"boxtype", //620 kifltk.cxx 12378
"boxtype(int x): Define the slider box type", //621 kifltk.cxx 12378
"align(int): define the slider alignement", //622 kifltk.cxx 12379
"step(int): define the slider step", //623 kifltk.cxx 12380
"value(int): define the value for the slider or return its value", //624 kifltk.cxx 12381
"create(int x,int y,int w,int h,string label): Create a box", //625 kifltk.cxx 12397
"type(string|int boxtype): Define the box type", //626 kifltk.cxx 12398
"create(int x,int y,int w,int h,string type,string shape,string label): Create a button. If type is not provided then it is: FL_Regular.", //627 kifltk.cxx 12414
"when", //628 kifltk.cxx 12415
"when(string when1, string when2,...): Type of event for a button which triggers the callback", //629 kifltk.cxx 12415
"shortcut", //630 kifltk.cxx 12416
"shortcut(string keycode): Set a shortcut to activate the button from the keyboard", //631 kifltk.cxx 12416
"color", //632 kifltk.cxx 12417
"color(string code|int code): Set the color of the button", //633 kifltk.cxx 12417
"value(): return the value of the current button", //634 kifltk.cxx 12418
"align(int): define the button label alignment", //635 kifltk.cxx 12419
"image(image im,string label,int labelalign): Use the image as a button image", //636 kifltk.cxx 12420
"bitmap(bitmap im,int color,string label,int labelalign): Use the bitmap as a button image", //637 kifltk.cxx 12421
"begin(int x,int y,int w, int h,string title): Create a tab window and begin initialisation", //638 kifltk.cxx 12437
"end(): end tab creation", //639 kifltk.cxx 12438
"current", //640 kifltk.cxx 12439
"current(): Return the current active tab", //641 kifltk.cxx 12439
"add", //642 kifltk.cxx 12440
"add(wgroup g): Add dynamically a new tab", //643 kifltk.cxx 12440
"remove", //644 kifltk.cxx 12441
"remove(wgroup g): Remove a tab", //645 kifltk.cxx 12441
"begin(int x,int y,int w, int h,string title): Create a group within a tab and begin initialisation", //646 kifltk.cxx 12458
"end(): end group creation", //647 kifltk.cxx 12459
"activate", //648 kifltk.cxx 12462
"activate(): Active this tab", //649 kifltk.cxx 12462
"loadjpeg", //650 kifltk.cxx 12529
"loadjpeg(string filename): Load a JPEG image", //651 kifltk.cxx 12529
"loadgif", //652 kifltk.cxx 12531
"loadgif(string filename): Load a GIF image", //653 kifltk.cxx 12531
"load", //654 kifltk.cxx 12533
"load(vector bitmaps,int w,int h): Load a bitmap", //655 kifltk.cxx 12533
"create(int x,int y,int w,int h,string label): Create a table of objects, and starts adding", //656 kifltk.cxx 12535
"clear", //657 kifltk.cxx 12536
"clear(): Clear the table", //658 kifltk.cxx 12536
"add(int R,int C,string v): Add a value on row R and column C", //659 kifltk.cxx 12537
"cell", //660 kifltk.cxx 12538
"cell(int R,int C): Return the value of the cell on row R and column C", //661 kifltk.cxx 12538
"cellalign", //662 kifltk.cxx 12539
"cellalign(align): Set the inner cell alignement", //663 kifltk.cxx 12539
"cellalignheadercol", //664 kifltk.cxx 12540
"cellalignheadercol(align): Set the header column alignement", //665 kifltk.cxx 12540
"cellalignheaderrow", //666 kifltk.cxx 12541
"cellalignheaderrow(align): Set the header row alignement", //667 kifltk.cxx 12541
"row", //668 kifltk.cxx 12542
"row()|(int nb): Define the number of rows", //669 kifltk.cxx 12542
"column", //670 kifltk.cxx 12543
"column()|(int nb): Define the number of columns", //671 kifltk.cxx 12543
"rowheight", //672 kifltk.cxx 12544
"rowheight(int height): Define the row height in pixel", //673 kifltk.cxx 12544
"columnwidth", //674 kifltk.cxx 12545
"columnwidth(int width): Define the column width in pixel", //675 kifltk.cxx 12545
"fontsize", //676 kifltk.cxx 12547
"fontsize(int c): set or return the text font size", //677 kifltk.cxx 12547
"colorbg", //678 kifltk.cxx 12548
"colorbg(int c): set or return the cell color background", //679 kifltk.cxx 12548
"colorfg", //680 kifltk.cxx 12549
"colorfg(int c): set or return the cell color foreground", //681 kifltk.cxx 12549
"when(string when1, string when2,...): Type of event to trigger the callback", //682 kifltk.cxx 12550
"colorselection(int color): Color for the selected elements", //683 kifltk.cxx 12551
"boxtype(int boxtype): box type", //684 kifltk.cxx 12552
"rowheader", //685 kifltk.cxx 12553
"rowheader(int pos,string label): Set the row header label at row pos", //686 kifltk.cxx 12553
"columnheader", //687 kifltk.cxx 12554
"columnheader(int pos,string label): Set the column header label at column pos", //688 kifltk.cxx 12554
"rowheaderheight", //689 kifltk.cxx 12555
"rowheaderheight(int height): the size in pixel of the row header", //690 kifltk.cxx 12555
"columnheaderwidth", //691 kifltk.cxx 12556
"columnheaderwidth(int width): the size in pixel of the column header", //692 kifltk.cxx 12556
"create(int x,int y,int w,int h,boolean multiline,string label): Create an input", //693 kifltk.cxx 12574
"value()|(string v): return the input buffer or set the initial buffer", //694 kifltk.cxx 12575
"insert", //695 kifltk.cxx 12576
"insert(string s,int p): insert s at position p in the input", //696 kifltk.cxx 12576
"selection", //697 kifltk.cxx 12577
"selection(): return the selected text in the input", //698 kifltk.cxx 12577
"color(string c|int c): set or return the text color", //699 kifltk.cxx 12578
"word", //700 kifltk.cxx 12579
"word(int pos): Return the word at position pos", //701 kifltk.cxx 12579
"font(string s): set or return the text font", //702 kifltk.cxx 12580
"create(int x,int y,int w,int h,string label): Create an editor", //703 kifltk.cxx 12600
"selection(): return the selected text in the editor or the coordinates of the selection according to the recipient variable", //704 kifltk.cxx 12601
"value()|(string v): return the text in the editor or initialize the editor", //705 kifltk.cxx 12602
"font(string s)|(): set or return the text font", //706 kifltk.cxx 12604
"fontsize(int c)|(): set or return the text font size", //707 kifltk.cxx 12605
"setstyle", //708 kifltk.cxx 12606
"setstyle(int start,int end,string style): set the style of a portion of text", //709 kifltk.cxx 12606
"load(string f): load file into editor", //710 kifltk.cxx 12607
"save", //711 kifltk.cxx 12608
"save(string f): save buffer to file f", //712 kifltk.cxx 12608
"replace", //713 kifltk.cxx 12609
"replace(string s,string sub,int i,bool matchcase): replace s with sub according to matchcase starting at i", //714 kifltk.cxx 12609
"replaceall", //715 kifltk.cxx 12610
"replaceall(string s,string sub,bool matchcase): replace all occurrences of s with sub according to matchcase", //716 kifltk.cxx 12610
"find", //717 kifltk.cxx 12611
"find(string s,int i,bool matchcase): find the position of a substring from i", //718 kifltk.cxx 12611
"rfind", //719 kifltk.cxx 12612
"rfind(string s,int i,bool matchcase): find the position of a substring from i backward", //720 kifltk.cxx 12612
"highlight", //721 kifltk.cxx 12613
"highlight(int start,int end)|(): highlight a portion of text. Return true if the text is highlighted or the highlighted text", //722 kifltk.cxx 12613
"unhighlight", //723 kifltk.cxx 12614
"unhighlight(): unhighlight a portion of text", //724 kifltk.cxx 12614
"cursorstyle(int cursorshape): Set the cursor shape", //725 kifltk.cxx 12615
"onmouse(int action,function f,object o): callback when clicking", //726 kifltk.cxx 12616
"onkey(int action,function f,object o): callback when a key is pressed", //727 kifltk.cxx 12617
"onvscroll", //728 kifltk.cxx 12618
"onvscroll(function f,object o): callback when scrolling vertically", //729 kifltk.cxx 12618
"onhscroll", //730 kifltk.cxx 12619
"onhscroll(function f,object o): callback when scrolling horizontally", //731 kifltk.cxx 12619
"getstyle", //732 kifltk.cxx 12620
"getstyle(int start,int end): return a vector of style for each character of the text section", //733 kifltk.cxx 12620
"addstyle", //734 kifltk.cxx 12621
"addstyle(map style): Initialize styles", //735 kifltk.cxx 12621
"delete", //736 kifltk.cxx 12623
"delete(): delete selected text from editor", //737 kifltk.cxx 12623
"cut", //738 kifltk.cxx 12624
"cut(): cut selected text from editor into clipboard", //739 kifltk.cxx 12624
"copy(string s): copy selected text from editor into clipboard (string s is optional)", //740 kifltk.cxx 12625
"paste(): paste selected text from clipboard", //741 kifltk.cxx 12626
"select", //742 kifltk.cxx 12629
"select(): Return selected string.\rselect(int posfirst,int poslast): Select the string between posfirst and posend", //743 kifltk.cxx 12629
"line(int l): return the current line position of the caret or return the line corresponding to a position", //744 kifltk.cxx 12630
"linebounds", //745 kifltk.cxx 12631
"linebounds(int l): return the beginning and the end at the current line position of the caret or return the line boundaries corresponding to a position", //746 kifltk.cxx 12631
"linecharbounds", //747 kifltk.cxx 12632
"linecharbounds(): return the beginning and the end at the current line position of the caret or return the line boundaries corresponding to a position as characters", //748 kifltk.cxx 12632
"cursor(int l): return the current cursor position of the caret or set the cursor position", //749 kifltk.cxx 12633
"cursorchar", //750 kifltk.cxx 12634
"cursorchar(): return the current cursor position of the caret as characters", //751 kifltk.cxx 12634
"gotoline", //752 kifltk.cxx 12635
"gotoline(int l,bool highlight): Goto line l. If true, highlight the line", //753 kifltk.cxx 12635
"annotate", //754 kifltk.cxx 12636
"annotate(string s|vector v,string style,bool matchcase): Annotate each occurence of s with style (matchcase is a default argument)", //755 kifltk.cxx 12636
"wrap", //756 kifltk.cxx 12638
"wrap(bool): Set the wrapping mode", //757 kifltk.cxx 12638
"annotateregexip", //758 kifltk.cxx 12639
"annotateregexip(string reg,string style): Annotate each string matching reg with style", //759 kifltk.cxx 12639
"append", //760 kifltk.cxx 12655
"append(string s): append s at the end of the input", //761 kifltk.cxx 12655
"byteposition", //762 kifltk.cxx 12656
"byteposition(int i): return the position in byte from a character position", //763 kifltk.cxx 12656
"charposition", //764 kifltk.cxx 12657
"charposition(int i): return the position in character from a byte position", //765 kifltk.cxx 12657
"create(int x,int y,int w,int h,boolean multiline,string label): Create an output", //766 kifltk.cxx 12660
"value(string s): set the output buffer", //767 kifltk.cxx 12661
"create(int x,int y,int w,int h,string label): Create an choice", //768 kifltk.cxx 12683
"value(int s): set the choice initialization value", //769 kifltk.cxx 12684
"menu(vector s): Initialize the menu", //770 kifltk.cxx 12686
"create(string intialdirectory,string filter,bool directory,string label): Open a file browser, select directories if directory is true", //771 kifltk.cxx 12704
"value(): Return the selected file", //772 kifltk.cxx 12705
"close(): Close the file browser", //773 kifltk.cxx 12706
"ok", //774 kifltk.cxx 12707
"ok(): return true if ok was pressed", //775 kifltk.cxx 12707
"open", //776 kifltk.cxx 12710
"open(): Choose a file via a mac window", //777 kifltk.cxx 12710
"load(string): Load a file into a mac window", //778 kifltk.cxx 12711
"create(x,y,w,h,label): Create a browser", //779 kifltk.cxx 12714
"add(label): Add a string to the browser", //780 kifltk.cxx 12715
"load(filename): Load a file into the browser", //781 kifltk.cxx 12716
"insert(l,label): Insert a label before line l", //782 kifltk.cxx 12717
"clear(): Clear the browser from all values", //783 kifltk.cxx 12718
"value(): return the current selected value", //784 kifltk.cxx 12719
"select(): Return selected string.\rselect(int i): Return string in position i", //785 kifltk.cxx 12720
"deselect", //786 kifltk.cxx 12721
"deselect(): Deselect all items\rdeselect(int i): Deselect item i", //787 kifltk.cxx 12721
"columnchar", //788 kifltk.cxx 12722
"columnchar(): Return the column char separator.\rcolumnchar(string): Set the column char separator", //789 kifltk.cxx 12722
"formatchar", //790 kifltk.cxx 12723
"formatchar(): Return the format char.\rformatchar(string): Set the format char", //791 kifltk.cxx 12723
"label(): Return the item label.", //792 kifltk.cxx 12742
"depth", //793 kifltk.cxx 12743
"depth(): Return the depth of the item.", //794 kifltk.cxx 12743
"value(object): Associate the item with a value or return that value.", //795 kifltk.cxx 12744
"clean", //796 kifltk.cxx 12745
"clean(): Remove the object associated through value.", //797 kifltk.cxx 12745
"bgcolor", //798 kifltk.cxx 12746
"bgcolor(int c): Set or return the item background color.", //799 kifltk.cxx 12746
"fgcolor", //800 kifltk.cxx 12747
"fgcolor(int c): Set or return the foreground color.", //801 kifltk.cxx 12747
"font(int c): Set or return the item font.", //802 kifltk.cxx 12748
"fontsize(int c): Set or return the item font size.", //803 kifltk.cxx 12749
"isclosed", //804 kifltk.cxx 12750
"isclosed(): Return true if element is closed.", //805 kifltk.cxx 12750
"isopen", //806 kifltk.cxx 12751
"isopen(): Return true if element is open.", //807 kifltk.cxx 12751
"isroot", //808 kifltk.cxx 12752
"isroot(): Return true if element is root.", //809 kifltk.cxx 12752
"isselected", //810 kifltk.cxx 12753
"isselected(): Return true if element is selected.", //811 kifltk.cxx 12753
"isactive", //812 kifltk.cxx 12754
"isactive(): Return true if element is active.", //813 kifltk.cxx 12754
"parent", //814 kifltk.cxx 12755
"parent(): Return the last element.", //815 kifltk.cxx 12755
"next", //816 kifltk.cxx 12756
"next(): Return the next element.", //817 kifltk.cxx 12756
"previous", //818 kifltk.cxx 12757
"previous(): Return the previous element.", //819 kifltk.cxx 12757
"child", //820 kifltk.cxx 12758
"child(int i): Return the child element at position i.", //821 kifltk.cxx 12758
"children", //822 kifltk.cxx 12759
"children(): Return number of children.", //823 kifltk.cxx 12759
"activate(): Activate the current element.", //824 kifltk.cxx 12760
"deactivate", //825 kifltk.cxx 12761
"deactivate(): Deactivate the current element.", //826 kifltk.cxx 12761
"create(int x,int y,int h,int w,string label): Create a tree", //827 kifltk.cxx 12763
"clear(): Delete the tree items", //828 kifltk.cxx 12764
"add(string path): Add a tree item.\radd(wtreeitem e,string n): Add a tree item after e.", //829 kifltk.cxx 12765
"isclosed(string path): Return true if element is closed.\risclosed(wtreeitem e): Return true if element is closed.", //830 kifltk.cxx 12766
"close(string path): Close the element.\rclose(wtreeitem e): Close the element.", //831 kifltk.cxx 12767
"open(string path): Open the element.\ropen(wtreeitem e): Open the element.", //832 kifltk.cxx 12768
"insert(wtreeitem e,string label,int pos): Insert an element after e with label at position pos in the children list.", //833 kifltk.cxx 12769
"remove(wtreeitem e): Remove the element e from the tree.", //834 kifltk.cxx 12770
"insertabove", //835 kifltk.cxx 12771
"insertabove(wtreeitem e,string label): Insert an element above e with label.", //836 kifltk.cxx 12771
"connectorcolor", //837 kifltk.cxx 12772
"connectorcolor(int c): Set or return the connector color.", //838 kifltk.cxx 12772
"connectorstyle", //839 kifltk.cxx 12773
"connectorstyle(int s): Set or return the connector style.", //840 kifltk.cxx 12773
"connectorwidth", //841 kifltk.cxx 12774
"connectorwidth(int s): Set or return the connector width.", //842 kifltk.cxx 12774
"sortorder", //843 kifltk.cxx 12775
"sortorder(int s): Set or return the sort order.", //844 kifltk.cxx 12775
"selectmode", //845 kifltk.cxx 12776
"selectmode(int s): Set or return the selection mode.", //846 kifltk.cxx 12776
"itembgcolor", //847 kifltk.cxx 12778
"itembgcolor(int c): Set or return the item background color.", //848 kifltk.cxx 12778
"itemfgcolor", //849 kifltk.cxx 12779
"itemfgcolor(int c): Set or return the foreground color.", //850 kifltk.cxx 12779
"itemfont", //851 kifltk.cxx 12780
"itemfont(int c): Set or return the item font.", //852 kifltk.cxx 12780
"itemsize", //853 kifltk.cxx 12781
"itemsize(int c): Set or return the item font size.", //854 kifltk.cxx 12781
"marginleft", //855 kifltk.cxx 12783
"Set or Get the amount of white space (in pixels) that should appear between the widget's left border and the tree's contents.", //856 kifltk.cxx 12783
"margintop", //857 kifltk.cxx 12784
"margintop(int s): Set or Get the amount of white space (in pixels) that should appear between the widget's top border and the top of the tree's contents.", //858 kifltk.cxx 12784
"root", //859 kifltk.cxx 12786
"root(): Return the root element.", //860 kifltk.cxx 12786
"rootlabel", //861 kifltk.cxx 12787
"rootlabel(string r): Set the root label.", //862 kifltk.cxx 12787
"clicked", //863 kifltk.cxx 12788
"clicked(): Return the element that was clicked.", //864 kifltk.cxx 12788
"first", //865 kifltk.cxx 12789
"first(): Return the first element.", //866 kifltk.cxx 12789
"find(string path): Return the element matching the path.", //867 kifltk.cxx 12790
"last", //868 kifltk.cxx 12791
"last(): Return the last element.", //869 kifltk.cxx 12791
"next(wtreeitem e): Return the next element after e.", //870 kifltk.cxx 12792
"previous(wtreeitem e): Return the previous element before e.", //871 kifltk.cxx 12793
""};


Exported const char* KIFLTKSDEBUGGERTRINGS[]={
"println", //0 kifeditor.cxx 44
"while", //1 kifeditor.cxx 45
"for", //2 kifeditor.cxx 46
"switch", //3 kifeditor.cxx 47
"if", //4 kifeditor.cxx 48
"else", //5 kifeditor.cxx 49
"elif", //6 kifeditor.cxx 50
"//Local variables:\n", //7 kifeditor.cxx 1019
"//Main variables:\n", //8 kifeditor.cxx 1031
"//Frame variables:", //9 kifeditor.cxx 1044
"//Function variables:", //10 kifeditor.cxx 1056
"Terminated", //11 kifeditor.cxx 1074
"Find", //12 kifeditor.cxx 1368
"String:", //13 kifeditor.cxx 1369
"Next", //14 kifeditor.cxx 1370
"Cancel", //15 kifeditor.cxx 1371
"Case", //16 kifeditor.cxx 1372
"Replace", //17 kifeditor.cxx 1411
"Find:", //18 kifeditor.cxx 1412
"Replace:", //19 kifeditor.cxx 1413
"Replace All", //20 kifeditor.cxx 1415
"Replace Next", //21 kifeditor.cxx 1416
"Suggest", //22 kifeditor.cxx 1525
"Noname", //23 kifeditor.cxx 1553
"Creating a new file:", //24 kifeditor.cxx 1559
"Line", //25 kifeditor.cxx 1659
"Blanks Size", //26 kifeditor.cxx 1670
"Two 'runs'.\n\tThe first runs in the main frame and CANNOT BE stopped.\n\tThe second runs in a thread and CAN BE stopped.", //27 kifeditor.cxx 2289
"File/Save", //28 kifeditor.cxx 2292
"File/Save as", //29 kifeditor.cxx 2293
"Edit/Undo", //30 kifeditor.cxx 2295
"Edit/Redo", //31 kifeditor.cxx 2296
"Edit/List", //32 kifeditor.cxx 2301
"Edit/Matching", //33 kifeditor.cxx 2302
"Edit/Indent", //34 kifeditor.cxx 2303
"Edit/(Un)comment", //35 kifeditor.cxx 2304
"Edit/Goto line", //36 kifeditor.cxx 2305
"Edit/Set blanks", //37 kifeditor.cxx 2306
"Edit/Find", //38 kifeditor.cxx 2307
"Edit/Replace", //39 kifeditor.cxx 2308
"Edit/Next", //40 kifeditor.cxx 2309
"Edit/Font larger", //41 kifeditor.cxx 2310
"Edit/Font smaller", //42 kifeditor.cxx 2311
"Edit/Cut", //43 kifeditor.cxx 2312
"Edit/Copy", //44 kifeditor.cxx 2313
"Edit/Paste", //45 kifeditor.cxx 2314
"Command/Run (main thread)", //46 kifeditor.cxx 2315
"Command/Run (threaded)", //47 kifeditor.cxx 2316
"Command/Stop", //48 kifeditor.cxx 2317
"Command/Debug", //49 kifeditor.cxx 2318
"Command/Toggle Breakpoint", //50 kifeditor.cxx 2319
"Command/Remove All Breakpoints", //51 kifeditor.cxx 2320
"Choose", //52 kifeditor.cxx 2382
"No description available", //53 kifeditor.cxx 2403
"File/New", //54 kifeditor.cxx 2622
"File/Open", //55 kifeditor.cxx 2623
"File/Quit", //56 kifeditor.cxx 2625
"Edit/Mirror", //57 kifeditor.cxx 2633
"Edit/Editor", //58 kifeditor.cxx 2635
"Edit/Clean", //59 kifeditor.cxx 2636
"*.kif|*.xip", //60 kifeditor.cxx 2653
"Load your file", //61 kifeditor.cxx 2653
"Save your file", //62 kifeditor.cxx 2671
"cleaned\n", //63 kifeditor.cxx 2711
"none", //64 kifeditor.cxx 2887
"Wrong instruction", //65 kifeditor.cxx 3050
"\nrecorded\n", //66 kifeditor.cxx 3055
"File:'", //67 kifeditor.cxx 3155
"' not saved... Do you want to quit? ", //68 kifeditor.cxx 3155
"Do you want to quit?", //69 kifeditor.cxx 3164
"Debugger", //70 kifeditor.cxx 3364
"Short value", //71 kifeditor.cxx 3366
"Locals", //72 kifeditor.cxx 3367
"Alls", //73 kifeditor.cxx 3368
"Stack", //74 kifeditor.cxx 3369
"Filename", //75 kifeditor.cxx 3370
"Goto", //76 kifeditor.cxx 3372
"In", //77 kifeditor.cxx 3373
"Out", //78 kifeditor.cxx 3374
"Stop", //79 kifeditor.cxx 3375
"To End", //80 kifeditor.cxx 3376
"Save your file as", //81 kifeditor.cxx 3541
"Save this window first", //82 kifeditor.cxx 3767
"is", //83 kifeditor.cxx 4062
"do", //84 kifeditor.cxx 4064
"predicatevar", //85 kifeditor.cxx 4065
"predicate", //86 kifeditor.cxx 4066
"term", //87 kifeditor.cxx 4068
"transducer", //88 kifeditor.cxx 4070
"dependency", //89 kifeditor.cxx 4072
"node", //90 kifeditor.cxx 4073
"_garbagefunction", //91 kifeditor.cxx 4074
"grammar", //92 kifeditor.cxx 4075
"otherwise", //93 kifeditor.cxx 4076
"_garbagesize", //94 kifeditor.cxx 4077
"setdebugfunction", //95 kifeditor.cxx 4078
"parentthreadid", //96 kifeditor.cxx 4079
"imatrix", //97 kifeditor.cxx 4080
"setdebugdata", //98 kifeditor.cxx 4081
"sthrough", //99 kifeditor.cxx 4082
"ithrough", //100 kifeditor.cxx 4083
"fthrough", //101 kifeditor.cxx 4084
"createserver", //102 kifeditor.cxx 4085
"getstackinfo", //103 kifeditor.cxx 4086
"waitonfalse", //104 kifeditor.cxx 4087
"env", //105 kifeditor.cxx 4088
"runadthread", //106 kifeditor.cxx 4089
"synchronous", //107 kifeditor.cxx 4090
"filebrowser", //108 kifeditor.cxx 4091
"polynomial", //109 kifeditor.cxx 4092
"printlnerr", //110 kifeditor.cxx 4093
"printjlnerr", //111 kifeditor.cxx 4094
"waitonjoined", //112 kifeditor.cxx 4095
"myvariable", //113 kifeditor.cxx 4096
"marginleft", //114 kifeditor.cxx 4097
"exclusive", //115 kifeditor.cxx 4098
"joined", //116 kifeditor.cxx 4099
"wtreeitem", //117 kifeditor.cxx 4100
"protected", //118 kifeditor.cxx 4101
"riterator", //119 kifeditor.cxx 4103
"fraction", //120 kifeditor.cxx 4104
"printerr", //121 kifeditor.cxx 4105
"printjerr", //122 kifeditor.cxx 4106
"function", //123 kifeditor.cxx 4107
"sequence", //124 kifeditor.cxx 4109
"threadid", //125 kifeditor.cxx 4110
"pathname", //126 kifeditor.cxx 4111
"continue", //127 kifeditor.cxx 4112
"iterator", //128 kifeditor.cxx 4113
"instance", //129 kifeditor.cxx 4114
"printjln", //130 kifeditor.cxx 4116
"address", //131 kifeditor.cxx 4117
"boolean", //132 kifeditor.cxx 4118
"autorun", //133 kifeditor.cxx 4119
"private", //134 kifeditor.cxx 4120
"browser", //135 kifeditor.cxx 4121
"wchoice", //136 kifeditor.cxx 4122
"woutput", //137 kifeditor.cxx 4123
"waiting", //138 kifeditor.cxx 4124
"_version", //139 kifeditor.cxx 4125
"matrix", //140 kifeditor.cxx 4126
"wheely", //141 kifeditor.cxx 4127
"wheelx", //142 kifeditor.cxx 4128
"remote", //143 kifeditor.cxx 4129
"minute", //144 kifeditor.cxx 4130
"_gcsize", //145 kifeditor.cxx 4131
"socket", //146 kifeditor.cxx 4132
"sound", //147 kifeditor.cxx 4133
"mp3", //148 kifeditor.cxx 4134
"button", //149 kifeditor.cxx 4135
"scroll", //150 kifeditor.cxx 4136
"xmldoc", //151 kifeditor.cxx 4138
"editor", //152 kifeditor.cxx 4139
"loadin", //153 kifeditor.cxx 4140
"winput", //154 kifeditor.cxx 4141
"slider", //155 kifeditor.cxx 4142
"wgroup", //156 kifeditor.cxx 4143
"image", //157 kifeditor.cxx 4144
"pflush", //158 kifeditor.cxx 4146
"rawstring", //159 kifeditor.cxx 4147
"string", //160 kifeditor.cxx 4148
"ustring", //161 kifeditor.cxx 4149
"tuple", //162 kifeditor.cxx 4150
"common", //163 kifeditor.cxx 4151
"random", //164 kifeditor.cxx 4152
"bottom", //165 kifeditor.cxx 4153
"vector", //166 kifeditor.cxx 4154
"return", //167 kifeditor.cxx 4155
"_return", //168 kifeditor.cxx 4156
"thread", //169 kifeditor.cxx 4157
"window", //170 kifeditor.cxx 4158
"wcounter", //171 kifeditor.cxx 4159
"wprogress", //172 kifeditor.cxx 4160
"bitmap", //173 kifeditor.cxx 4161
"second", //174 kifeditor.cxx 4162
"strict", //175 kifeditor.cxx 4163
"print", //176 kifeditor.cxx 4164
"printj", //177 kifeditor.cxx 4165
"catch", //178 kifeditor.cxx 4166
"table", //179 kifeditor.cxx 4167
"false", //180 kifeditor.cxx 4168
"eigen", //181 kifeditor.cxx 4169
"wtabs", //182 kifeditor.cxx 4170
"frame", //183 kifeditor.cxx 4171
"extension", //184 kifeditor.cxx 4172
"start", //185 kifeditor.cxx 4174
"stdin", //186 kifeditor.cxx 4175
"not", //187 kifeditor.cxx 4176
"absent", //188 kifeditor.cxx 4177
"from", //189 kifeditor.cxx 4178
"notin", //190 kifeditor.cxx 4179
"pause", //191 kifeditor.cxx 4180
"const", //192 kifeditor.cxx 4181
"sleep", //193 kifeditor.cxx 4182
"reset", //194 kifeditor.cxx 4183
"range", //195 kifeditor.cxx 4184
"break", //196 kifeditor.cxx 4185
"raise", //197 kifeditor.cxx 4186
"float", //198 kifeditor.cxx 4187
"wtree", //199 kifeditor.cxx 4188
"bool", //200 kifeditor.cxx 4190
"self", //201 kifeditor.cxx 4191
"auto", //202 kifeditor.cxx 4192
"eval", //203 kifeditor.cxx 4193
"true", //204 kifeditor.cxx 4194
"fail", //205 kifeditor.cxx 4195
"cast", //206 kifeditor.cxx 4196
"this", //207 kifeditor.cxx 4197
"getc", //208 kifeditor.cxx 4198
"long", //209 kifeditor.cxx 4199
"time", //210 kifeditor.cxx 4200
"call", //211 kifeditor.cxx 4201
"list", //212 kifeditor.cxx 4202
"port", //213 kifeditor.cxx 4203
"_exit", //214 kifeditor.cxx 4204
"wfile", //215 kifeditor.cxx 4205
"file", //216 kifeditor.cxx 4206
"hash", //217 kifeditor.cxx 4208
"bits", //218 kifeditor.cxx 4209
"empty", //219 kifeditor.cxx 4210
"null", //220 kifeditor.cxx 4211
"kill", //221 kifeditor.cxx 4212
"byte", //222 kifeditor.cxx 4213
"sloop", //223 kifeditor.cxx 4214
"uloop", //224 kifeditor.cxx 4215
"bloop", //225 kifeditor.cxx 4216
"iloop", //226 kifeditor.cxx 4217
"floop", //227 kifeditor.cxx 4218
"int", //228 kifeditor.cxx 4219
"isa", //229 kifeditor.cxx 4220
"use", //230 kifeditor.cxx 4221
"top", //231 kifeditor.cxx 4223
"box", //232 kifeditor.cxx 4225
"try", //233 kifeditor.cxx 4226
"bit", //234 kifeditor.cxx 4227
"tree", //235 kifeditor.cxx 4228
"where", //236 kifeditor.cxx 4231
"repeat", //237 kifeditor.cxx 4232
"cycle", //238 kifeditor.cxx 4233
"replicate", //239 kifeditor.cxx 4234
"filter", //240 kifeditor.cxx 4235
"take", //241 kifeditor.cxx 4236
"takeWhile", //242 kifeditor.cxx 4237
"zipWith", //243 kifeditor.cxx 4238
"zip", //244 kifeditor.cxx 4239
"flip", //245 kifeditor.cxx 4240
"drop", //246 kifeditor.cxx 4241
"dropWhile", //247 kifeditor.cxx 4242
"foldr", //248 kifeditor.cxx 4243
"foldl", //249 kifeditor.cxx 4244
"foldr1", //250 kifeditor.cxx 4245
"foldl1", //251 kifeditor.cxx 4246
"scanr", //252 kifeditor.cxx 4247
"scanl", //253 kifeditor.cxx 4248
"scanr1", //254 kifeditor.cxx 4249
"scanl1", //255 kifeditor.cxx 4250
"set", //256 kifeditor.cxx 4252
"sset", //257 kifeditor.cxx 4253
"uset", //258 kifeditor.cxx 4254
"iset", //259 kifeditor.cxx 4255
"fset", //260 kifeditor.cxx 4256
"fvector", //261 kifeditor.cxx 4258
"svector", //262 kifeditor.cxx 4259
"uvector", //263 kifeditor.cxx 4260
"ivector", //264 kifeditor.cxx 4261
"bvector", //265 kifeditor.cxx 4262
"map", //266 kifeditor.cxx 4264
"maps", //267 kifeditor.cxx 4266
"mapf", //268 kifeditor.cxx 4267
"mapi", //269 kifeditor.cxx 4268
"fmap", //270 kifeditor.cxx 4270
"imap", //271 kifeditor.cxx 4271
"smap", //272 kifeditor.cxx 4272
"sfmap", //273 kifeditor.cxx 4274
"simap", //274 kifeditor.cxx 4275
"ssmap", //275 kifeditor.cxx 4276
"ismap", //276 kifeditor.cxx 4278
"ifmap", //277 kifeditor.cxx 4279
"iimap", //278 kifeditor.cxx 4280
"fsmap", //279 kifeditor.cxx 4282
"ffmap", //280 kifeditor.cxx 4283
"fimap", //281 kifeditor.cxx 4284
"treemap", //282 kifeditor.cxx 4286
"treemaps", //283 kifeditor.cxx 4288
"treemapf", //284 kifeditor.cxx 4289
"treemapi", //285 kifeditor.cxx 4290
"ftreemap", //286 kifeditor.cxx 4292
"itreemap", //287 kifeditor.cxx 4293
"streemap", //288 kifeditor.cxx 4294
"sftreemap", //289 kifeditor.cxx 4296
"sitreemap", //290 kifeditor.cxx 4297
"sstreemap", //291 kifeditor.cxx 4298
"istreemap", //292 kifeditor.cxx 4300
"iftreemap", //293 kifeditor.cxx 4301
"iitreemap", //294 kifeditor.cxx 4302
"fstreemap", //295 kifeditor.cxx 4304
"fftreemap", //296 kifeditor.cxx 4305
"fitreemap", //297 kifeditor.cxx 4306
"primemap", //298 kifeditor.cxx 4308
"primemaps", //299 kifeditor.cxx 4310
"primemapf", //300 kifeditor.cxx 4311
"primemapi", //301 kifeditor.cxx 4312
"fprimemap", //302 kifeditor.cxx 4314
"iprimemap", //303 kifeditor.cxx 4315
"sprimemap", //304 kifeditor.cxx 4316
"sfprimemap", //305 kifeditor.cxx 4318
"siprimemap", //306 kifeditor.cxx 4319
"ssprimemap", //307 kifeditor.cxx 4320
"isprimemap", //308 kifeditor.cxx 4322
"ifprimemap", //309 kifeditor.cxx 4323
"iiprimemap", //310 kifeditor.cxx 4324
"fsprimemap", //311 kifeditor.cxx 4326
"ffprimemap", //312 kifeditor.cxx 4327
"fiprimemap", //313 kifeditor.cxx 4328
"xml", //314 kifeditor.cxx 4330
"case", //315 kifeditor.cxx 4332
"let", //316 kifeditor.cxx 4333
"in", //317 kifeditor.cxx 4334
"into", //318 kifeditor.cxx 4335
"on", //319 kifeditor.cxx 4336
"default", //320 kifeditor.cxx 4337
"curl", //321 kifeditor.cxx 4338
"and", //322 kifeditor.cxx 4339
"or", //323 kifeditor.cxx 4340
"with", //324 kifeditor.cxx 4343
"_endl", //325 kifeditor.cxx 4344
"a_first", //326 kifeditor.cxx 4345
"a_last", //327 kifeditor.cxx 4346
"a_prefix", //328 kifeditor.cxx 4347
"a_change", //329 kifeditor.cxx 4348
"a_insert", //330 kifeditor.cxx 4349
"a_delete", //331 kifeditor.cxx 4350
"a_switch", //332 kifeditor.cxx 4351
"a_nocase", //333 kifeditor.cxx 4352
"a_surface", //334 kifeditor.cxx 4353
"a_full", //335 kifeditor.cxx 4354
"a_split", //336 kifeditor.cxx 4355
"a_skip", //337 kifeditor.cxx 4356
"a_track", //338 kifeditor.cxx 4357
"pred_none", //339 kifeditor.cxx 4358
"pred_weight", //340 kifeditor.cxx 4359
"pred_weightstack", //341 kifeditor.cxx 4360
"automaton", //342 kifeditor.cxx 4361
"mapff", //343 kifeditor.cxx 4363
"primemapff", //344 kifeditor.cxx 4364
"treemapff", //345 kifeditor.cxx 4365
"mapfi", //346 kifeditor.cxx 4366
"primemapfi", //347 kifeditor.cxx 4367
"treemapfi", //348 kifeditor.cxx 4368
"mapfs", //349 kifeditor.cxx 4369
"primemapfs", //350 kifeditor.cxx 4370
"treemapfs", //351 kifeditor.cxx 4371
"mapif", //352 kifeditor.cxx 4372
"primemapif", //353 kifeditor.cxx 4373
"treemapif", //354 kifeditor.cxx 4374
"mapii", //355 kifeditor.cxx 4375
"primemapii", //356 kifeditor.cxx 4376
"treemapii", //357 kifeditor.cxx 4377
"mapis", //358 kifeditor.cxx 4378
"primemapis", //359 kifeditor.cxx 4379
"treemapis", //360 kifeditor.cxx 4380
"mapsf", //361 kifeditor.cxx 4381
"primemapsf", //362 kifeditor.cxx 4382
"treemapsf", //363 kifeditor.cxx 4383
"mapsi", //364 kifeditor.cxx 4384
"primemapsi", //365 kifeditor.cxx 4385
"treemapsi", //366 kifeditor.cxx 4386
"mapss", //367 kifeditor.cxx 4387
"primemapss", //368 kifeditor.cxx 4388
"treemapss", //369 kifeditor.cxx 4389
"mapuu", //370 kifeditor.cxx 4390
"uumap", //371 kifeditor.cxx 4391
"mapui", //372 kifeditor.cxx 4392
"uimap", //373 kifeditor.cxx 4393
"mapuf", //374 kifeditor.cxx 4394
"ufmap", //375 kifeditor.cxx 4395
"mapiu", //376 kifeditor.cxx 4396
"iumap", //377 kifeditor.cxx 4397
"mapfu", //378 kifeditor.cxx 4398
"fumap", //379 kifeditor.cxx 4399
"treemapuu", //380 kifeditor.cxx 4400
"uutreemap", //381 kifeditor.cxx 4401
"treemapui", //382 kifeditor.cxx 4402
"uitreemap", //383 kifeditor.cxx 4403
"treemapuf", //384 kifeditor.cxx 4404
"uftreemap", //385 kifeditor.cxx 4405
"treemapiu", //386 kifeditor.cxx 4406
"iutreemap", //387 kifeditor.cxx 4407
"treemapfu", //388 kifeditor.cxx 4408
"futreemap", //389 kifeditor.cxx 4409
"primemapuu", //390 kifeditor.cxx 4410
"uuprimemap", //391 kifeditor.cxx 4411
"primemapui", //392 kifeditor.cxx 4412
"uiprimemap", //393 kifeditor.cxx 4413
"primemapuf", //394 kifeditor.cxx 4414
"ufprimemap", //395 kifeditor.cxx 4415
"primemapiu", //396 kifeditor.cxx 4416
"iuprimemap", //397 kifeditor.cxx 4417
"primemapfu", //398 kifeditor.cxx 4418
"fuprimemap", //399 kifeditor.cxx 4419
"binmapi", //400 kifeditor.cxx 4420
"binmapii", //401 kifeditor.cxx 4421
"binmapif", //402 kifeditor.cxx 4422
"binmapis", //403 kifeditor.cxx 4423
"binmapiu", //404 kifeditor.cxx 4424
"iibinmap", //405 kifeditor.cxx 4425
"ifbinmap", //406 kifeditor.cxx 4426
"isbinmap", //407 kifeditor.cxx 4427
"iubinmap", //408 kifeditor.cxx 4428
"KIF", //409 kifeditor.cxx 4433
""};


Exported const char* KIFSYSSTRINGS[]={
"w", //0 kifsys.cxx 167
"SYS(001): Cannot open output file", //1 kifsys.cxx 169
"r", //2 kifsys.cxx 176
"size", //3 kifsys.cxx 275
"date", //4 kifsys.cxx 276
"access", //5 kifsys.cxx 277
"change", //6 kifsys.cxx 278
"pathname", //7 kifsys.cxx 279
"directory", //8 kifsys.cxx 281
"name", //9 kifsys.cxx 331
"_sys", //10 kifsys.cxx 398
"Incompatible version of KiF", //11 kifsys.cxx 413
"command", //12 kifsys.cxx 440
"command(string com,string outputfile): execute the command 'com'. outputfile is optional and can be used to redirect output to a file.", //13 kifsys.cxx 440
"isdirectory", //14 kifsys.cxx 441
"isdirectory(string path): return 'true' if path is a directory", //15 kifsys.cxx 441
"createdirectory", //16 kifsys.cxx 442
"createdirectory(string path): create the directory of path 'path'", //17 kifsys.cxx 442
"mkdir", //18 kifsys.cxx 443
"mkdir(string path): create the directory of path 'path'", //19 kifsys.cxx 443
"listdirectory", //20 kifsys.cxx 444
"listdirectory(string path): return the content of the directory 'path'", //21 kifsys.cxx 444
"ls", //22 kifsys.cxx 445
"ls(string path): return the content of the directory 'path'", //23 kifsys.cxx 445
"realpath", //24 kifsys.cxx 446
"realpath(path relpath): return the actual path matching 'realpath'", //25 kifsys.cxx 446
"fileinfo", //26 kifsys.cxx 447
"fileinfo(string pathname): returns the info of the file 'pathname'", //27 kifsys.cxx 447
"env", //28 kifsys.cxx 448
"env(string var)|env(string var,string val): return or set the content of the environment variable 'var'", //29 kifsys.cxx 448
""};


Exported const char* KIFCURLSTRINGS[]={
"with", //0 kifcurl.cxx 321
"URL(031): Unknown option", //1 kifcurl.cxx 442
"w", //2 kifcurl.cxx 493
"URL(009): Wrong filename", //3 kifcurl.cxx 495
"name", //4 kifcurl.cxx 540
"size", //5 kifcurl.cxx 561
"Incompatible version of KiF", //6 kifcurl.cxx 626
"curl", //7 kifcurl.cxx 632
"proxy", //8 kifcurl.cxx 658
"proxy(string prox): Proxy connection", //9 kifcurl.cxx 658
"password", //10 kifcurl.cxx 659
"password(string user,string pwd): Provide the user password", //11 kifcurl.cxx 659
"url", //12 kifcurl.cxx 660
"url(string path): Load a URL\rurl(string path,string filename): Load a url into a filename", //13 kifcurl.cxx 660
"execute", //14 kifcurl.cxx 661
"execute(string filename): Execute a curl action, filename is optional.", //15 kifcurl.cxx 661
"options", //16 kifcurl.cxx 662
"options(string option,string parameter): Set the options of the curl object", //17 kifcurl.cxx 662
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
"with", //0 kifsound.cxx 146
"SND(001): Unknown file", //1 kifsound.cxx 179
"SND(005): Error during loading", //2 kifsound.cxx 247
"SND(003): Please load a file first", //3 kifsound.cxx 256
"Incompatible version of KiF", //4 kifsound.cxx 372
"mp3", //5 kifsound.cxx 378
"load", //6 kifsound.cxx 401
"load(string pathname): Load the sound pathname.", //7 kifsound.cxx 401
"play", //8 kifsound.cxx 402
"play(): play the sound", //9 kifsound.cxx 402
"stop", //10 kifsound.cxx 403
"stop(): stop the sound", //11 kifsound.cxx 403
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
"XML(234): Already created", //5 kifxml.h 578
"XML(235): Empty doc", //6 kifxml.h 599
""};


Exported const char* KIFTHEINFOSTRINGS[]={
"transpose(","- transpose(): return the transposed matrix",
"circle(","- circle(int x,int y,int r,int color): Draw a circle. 'color' is optional.",
"create(","- create(int x,int y,int w, int h,string title): Create a window without widgets, w and h are optional\n- create(int x,int y,int w,int h,string label): Create a scroll\n- create(int x,int y,int w,int h,string label): Create a wcounter\n- create(int x,int y,int w,int h,string label): Create a progress bar\n- create(int x,int y,int w,int h,int alignment,bool valueslider,string label): Create a slider or a valueslider\n- create(int x,int y,int w,int h,string label): Create a box\n- create(int x,int y,int w,int h,string type,string shape,string label): Create a button. If type is not provided then it is: FL_Regular.\n- create(int x,int y,int w,int h,string label): Create a table of objects, and starts adding\n- create(int x,int y,int w,int h,boolean multiline,string label): Create an input\n- create(int x,int y,int w,int h,string label): Create an editor\n- create(int x,int y,int w,int h,boolean multiline,string label): Create an output\n- create(int x,int y,int w,int h,string label): Create an choice\n- create(string intialdirectory,string filter,bool directory,string label): Open a file browser, select directories if directory is true\n- create(x,y,w,h,label): Create a browser\n- create(int x,int y,int h,int w,string label): Create a tree\n- create(x1,x2,x3): create a table in a database, with the arguments x1,x2,x3.\rEx. mydb.create('table1','nom TEXT PRIMARY KEY','age INTEGER');\n- create(string topnode): create an XML file with topnode as the top node or with topnode, if topdenode is a full XML structure",
"createserver(","- createserver(string hostname,int port,int nblients: create a server, if hostname is omitted, then the local hostname is used",
"byteposition(","- byteposition(int i): return the position in byte from a character position",
"dimension(","- dimension(): return the matrix size.\ndimension(int rowsize,int columnsize): set the matrix size.",
"determinant(","- determinant(): return the matrix determinant",
"cell(","- cell(int R,int C): Return the value of the cell on row R and column C",
"transformedvertex(","- transformedvertex(float x, float y): add transformations to vertices list.",
"drawtext(","- drawtext(string l,int x,int y): Put a text at position x,y",
"run(","- run(int num,string stop): Only on the server side: execute remote method invocations for the client 'num'. 'stop' is the string that can be used to stop the server by the client.\n- run(): Launch the GUI\n- run(string sqlcommand): execute a sql command. If the input variable is a vector, then all possible values will be stored in it. It the input variable is an iterator, then it is possible to iterate on the results of the sql command. Each result is a map, where each key is a column name.",
"sum(","- sum(): return the sum of all elements",
"vertex(","- vertex(float x,float y): Add a vertex to a complex structure",
"connectorstyle(","- connectorstyle(int s): Set or return the connector style.",
"product(","- product(): return the product of all elements",
"getpeername(","- getpeername(int num): return the current peer name on the socket 'num'",
"read(","- read(int num): read a string on a socket. On the server side, 'num' is the value returned by 'wait()'. Use 'read()' on the client side",
"connect(","- connect(string hostname,int port): connect to the server",
"parse(","- parse(string buffer): parse an xml buffer",
"line(","- line(int x,int y,int x1, int y1,int x2, int y2): Draw a line between points, x2 and y2 are optional\n- line(int l): return the current line position of the caret or return the line corresponding to a position\n- line(): return the line position of the node",
"cursorchar(","- cursorchar(): return the current cursor position of the caret as characters",
"blocking(","- blocking(bool flag): if 'flag' is true, the socket works in 'blocking' mode, otherwise in 'non blocking' mode",
"textsize(","- textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels",
"invert(","- invert(): Return the inverted matrix.",
"lineshape(","- lineshape(string type,int width): Select the line shape and its thikness",
"send(","- send(int num,string s): write the string s on the socket in a non KiF environment. On the server side, num is the value returned by wait()'. Use 'send(string s)' on the client side",
"name(","- name(): name node",
"wait(","- wait(): wait for a client to connect and returns its socket id",
"children(","- children(): Return number of children.",
"onclose(","- onclose(function,object): Callback to call on close",
"write(","- write(int num,string s): write the string s on the socket. On the server side, num is the value returned by wait()'. Use 'write(string s)' on the client side",
"receive(","- receive(int num,int nb): read a string on a socket in a non KiF environment. On the server side, 'num' is the value returned by 'wait()'. Use 'receive()' on the client side",
"get(","- get(int num): get one character from a socket in a non KiF environment. On the server side, 'num' is the value returned by 'wait()'. Use 'get()' on the client side\n- get(string msg): Pop up window to get a value from the user",
"close(","- close(int num): Close a socket. On the server side, if 'num' is provided (it is the value returned by wait()) it closes the client socket, otherwise it closes the current socket.\n- close(): close the window\n- close(): Close the file browser\n- close(string path): Close the element.\rclose(wtreeitem e): Close the element.\n- close(): close a database\n- close(): close an xml file\n- close(): close the sound channel (server side)",
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
"ontime(","- ontime(function,t,object): Set a time out function",
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
"transformx(","- transformx(float x, float y): Transform a coordinate X using the current transformation matrix.",
"plotcoords(","- plotcoords(fvector inits,float x,float y): Compute the coordinates of a point(x,y) according to the previous scale computed with plot. Returns a vector of two elements [xs,ys] corresponding to the screen coordinates in the current window.",
"loadgif(","- loadgif(string filename): Load a GIF image",
"transformy(","- transformy(float x, float y): Transform a coordinate Y using the current transformation matrix.",
"ask(","- ask(string msg,string buttonmsg1,string buttonmsg2,string buttonmsg3): Pop up window to pose a question",
"transformdx(","- transformdx(float x, float y): Transform a distance DX using the current transformation matrix.",
"replaceall(","- replaceall(string s,string sub,bool matchcase): replace all occurrences of s with sub according to matchcase",
"alert(","- alert(string msg): Pop up window to display an alert",
"image(","- image(image image,int x, int y, int w, int h): Display an image\n- image(image im,string label,int labelalign): Use the image as a button image",
"password(","- password(string msg): Pop up window to input a password\n- password(string user,string pwd): Provide the user password",
"find(","- find(string s,int i,bool matchcase): find the position of a substring from i\n- find(string path): Return the element matching the path.",
"sizerange(","- sizerange(int minw,int minh, int maxw,int maxh): define range in which the size of the window can evolve",
"modal(","- modal(bool b): If true make the window modal. If no parameter return if the window is modal",
"drawcolor(","- drawcolor(int c|string c): set the color for the next drawings",
"rectangle(","- rectangle(int x,int y,int w, int h, string c|int c): Draw a rectangle with optional color c",
"rectanglefill(","- rectanglefill(int x,int y,int w, int h, string c|int c): Fill a rectangle with optional color c",
"loop(","- loop(int x,int y,int x1, int y1,int x2, int y2, int x3, int y3): Draw a series of lines, x3 and y3 are optional",
"annotate(","- annotate(string s|vector v,string style,bool matchcase): Annotate each occurence of s with style (matchcase is a default argument)",
"onmouse(","- onmouse(int action, function,object): Set the call back function on a mouse action with a given object as parameter\n- onmouse(int action,function f,object o): callback when clicking",
"rotation(","- rotation(float x,float y,float distance, float angle, bool draw): Compute the coordinate of a rotated point from point x,y, using a distance and an angle. Return a vector of floats with the new coordinates.",
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
"transformdy(","- transformdy(float x, float y): Transform a distance DY using the current transformation matrix.",
"beginpoints(","- beginpoints(): Start accumulating vertices",
"endpoints(","- endpoints(): Stop accumulating vertices",
"linecharbounds(","- linecharbounds(): return the beginning and the end at the current line position of the caret or return the line boundaries corresponding to a position as characters",
"beginline(","- beginline(): Start drawing lines.",
"endline(","- endline(): End drawing lines.",
"xpath(","- xpath(string myxpath): Return a vector of xml nodes matching myxpath",
"beginloop(","- beginloop(): Start drawing a closed sequence of lines.",
"endloop(","- endloop(): End drawing a closed sequence of lines.",
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
"value(","- value(float): define the value for the wcounter or return its value\n- value(float): define the value for the progress bar or return its value\n- value(int): define the value for the slider or return its value\n- value(): return the value of the current button\n- value()|(string v): return the input buffer or set the initial buffer\n- value()|(string v): return the text in the editor or initialize the editor\n- value(string s): set the output buffer\n- value(int s): set the choice initialization value\n- value(): Return the selected file\n- value(): return the current selected value\n- value(object): Associate the item with a value or return that value.",
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
"row(","- row()|(int nb): Define the number of rows",
"column(","- column()|(int nb): Define the number of columns",
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
"itemsize(","- itemsize(int c): Set or return the item font size.",
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
