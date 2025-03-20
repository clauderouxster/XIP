#include "kif.h"


Exported const char* kifwhile="tantque";
Exported const char* kiffor="pour";
Exported const char* kifif="si";
Exported const char* kifswitch="parmi";
Exported const char* kifdisplayln="afficheligne";
Exported const char* kifdisplay="affiche";
Exported const char* kifelif="sinonsi";
Exported const char* kifelse="sinon";

#ifdef WITHWSTRING
Exported const wchar_t* wkifwhile=L"tantque";
Exported const wchar_t* wkiffor=L"pour";
Exported const wchar_t* wkifif=L"si";
Exported const wchar_t* wkifswitch=L"parmi";
Exported const wchar_t* wkifdisplayln=L"afficheligne";
Exported const wchar_t* wkifdisplay=L"affiche";
Exported const wchar_t* wkifelif=L"sinonsi";
Exported const wchar_t* wkifelse=L"sinon";
#endif


Exported const char* KIFKIFSTRINGS[]={
"(ligne:", //0 kif.cxx 988
"ça", //1 kif.cxx 1087
"nulle", //2 kif.cxx 1087
"Instruction", //3 kif.cxx 1239
"ligne %d dans %s", //4 kif.cxx 1245
"KIF(413): Valeur incompatible", //5 kif.cxx 1595
"fonction", //6 kif.cxx 1694
"classe", //7 kif.cxx 1703
"l", //8 kif.cxx 1812
"e", //9 kif.cxx 1828
"e+", //10 kif.cxx 1836
"vrai", //11 kif.cxx 3854
"faux", //12 kif.cxx 3855
"KIF(421): Nous n'acceptons qu'un seul niveau d'indirection pour les streams", //13 kif.cxx 4142
"KIF(689): Structure circulaire", //14 kif.cxx 5218
"KIF(777): Trop de tâches jointes", //15 kif.cxx 5633
"STK(999): Plus tâches disponibles", //16 kif.cxx 5802
"KIF(281): Dépassement de taille", //17 kif.cxx 6004
"' dans objet de type '", //18 kif.cxx 8175
"KIF(821): Variable de flux non accessible", //19 kif.cxx 8334
"KIF(331): Mauvaise utilisation de cette variable:", //20 kif.cxx 11791
"KIF(192): fonction externe inconnu:", //21 kif.cxx 11815
"KIF(301): fonction inconnu:", //22 kif.cxx 12269
"KIF(345): fonction classe inconnu:", //23 kif.cxx 12297
"omni", //24 kif.cxx 12390
"KIF(650): Le paramètre doit être une fonction", //25 kif.cxx 12753
"KIF(432): Mauvais usage de l'opérateur '::'. Deux éléments maximum", //26 kif.cxx 13244
"KIF(432): Mauvais usage de l'opérateur '::', un des éléments doit une table", //27 kif.cxx 13270
"exécutetâche", //28 kif.cxx 14361
"KIF(415): Un tuple ne peut pas être modifié", //29 kif.cxx 14671
""};


Exported const char* KIFMAINSTRINGS[]={
"-console: ouvre la console", //0 kifmain.cxx 116
"-a 'code source' avec _args contenant tout le 'pipe'", //1 kifmain.cxx 117
"-p 'code source' avec _args contenant un élément du 'pipe' à la fois", //2 kifmain.cxx 118
"-c 'code source' sans données provenant du 'pipe'", //3 kifmain.cxx 119
"Variables prédéclarées:", //4 kifmain.cxx 120
"_args: table des arguments", //5 kifmain.cxx 122
"_chemins: _chemins[0] est le répertoire courant", //6 kifmain.cxx 124
"i,j,k: nombre", //7 kifmain.cxx 126
"f,g,h: décimal", //8 kifmain.cxx 128
"s,t,u: chaine", //9 kifmain.cxx 130
"dictionnaire d", //10 kifmain.cxx 132
"v: table", //11 kifmain.cxx 134
"REGEX disponible", //12 kifmain.cxx 160
"Version Xcode", //13 kifmain.cxx 163
"Bibliothèque Curl disponible", //14 kifmain.cxx 166
"Fast Light ToolKit (FLTK) disponible", //15 kifmain.cxx 169
"-sansdos", //16 kifmain.cxx 188
"-vérifier", //17 kifmain.cxx 201
"Erreur: code manquant", //18 kifmain.cxx 230
"Fichier manquant", //19 kifmain.cxx 318
"Erreur:", //20 kifmain.cxx 351
""};


Exported const char* KIFGLOBALTHREAD[]={
"e", //0 kifglobalthread.cxx 201
"_courant", //1 kifglobalthread.cxx 272
"SYS(010): Le flux a déjà été redirigé", //2 kifglobalthread.cxx 349
"SYS(001): Erreur à l'ouverture du fichier de sortie", //3 kifglobalthread.cxx 351
"SYS(011): Le flux n'a pas été ouvert encore", //4 kifglobalthread.cxx 363
"KIF(629): Type inconnu:", //5 kifglobalthread.cxx 400
"STK(888): Débordement de pile", //6 kifglobalthread.cxx 677
"(Tâche:", //7 kifglobalthread.cxx 738
"à la ligne:", //8 kifglobalthread.cxx 826
"dans", //9 kifglobalthread.cxx 831
"Instruction", //10 kifglobalthread.cxx 839
"ligne %ld dans %s", //11 kifglobalthread.cxx 846
"ligne %ld", //12 kifglobalthread.cxx 848
"ligne %d dans %s", //13 kifglobalthread.cxx 964
"nulle", //14 kifglobalthread.cxx 1151
"ça", //15 kifglobalthread.cxx 1152
"séquence", //16 kifglobalthread.cxx 1153
"capte", //17 kifglobalthread.cxx 1154
"omni", //18 kifglobalthread.cxx 1155
"_initiale", //19 kifglobalthread.cxx 1158
"taille", //20 kifglobalthread.cxx 1159
"méthodes", //21 kifglobalthread.cxx 1166
"table", //22 kifglobalthread.cxx 1167
"dictionnaire", //23 kifglobalthread.cxx 1168
"applique", //24 kifglobalthread.cxx 1169
"synchrone", //25 kifglobalthread.cxx 1170
"saisie", //26 kifglobalthread.cxx 1171
"chaine", //27 kifglobalthread.cxx 1172
"nombre", //28 kifglobalthread.cxx 1173
"long", //29 kifglobalthread.cxx 1174
"décimal", //30 kifglobalthread.cxx 1175
"booléen", //31 kifglobalthread.cxx 1176
"fraction", //32 kifglobalthread.cxx 1177
"vrai", //33 kifglobalthread.cxx 1193
"faux", //34 kifglobalthread.cxx 1194
"_chemins", //35 kifglobalthread.cxx 1196
"_KIFPRINCIPALE", //36 kifglobalthread.cxx 1199
"_chargeur", //37 kifglobalthread.cxx 1201
"dépile", //38 kifglobalthread.cxx 1202
"entréestandard", //39 kifglobalthread.cxx 1207
"pour", //40 kifglobalthread.cxx 1209
"tantque", //41 kifglobalthread.cxx 1210
"tente", //42 kifglobalthread.cxx 1211
"parmi", //43 kifglobalthread.cxx 1212
"si", //44 kifglobalthread.cxx 1213
"_finale", //45 kifglobalthread.cxx 1215
"_exécute_comme_tâche", //46 kifglobalthread.cxx 1216
"chargelocalement", //47 kifglobalthread.cxx 1217
"_finl", //48 kifglobalthread.cxx 1218
"vide", //49 kifglobalthread.cxx 1219
"dicocc", //50 kifglobalthread.cxx 1221
"dicocn", //51 kifglobalthread.cxx 1222
"dicocd", //52 kifglobalthread.cxx 1223
"tablechaines", //53 kifglobalthread.cxx 1224
"tablenombres", //54 kifglobalthread.cxx 1225
"tabledécimaux", //55 kifglobalthread.cxx 1226
"échec", //56 kifglobalthread.cxx 1229
"!assertebd", //57 kifglobalthread.cxx 1233
"!rétractebd", //58 kifglobalthread.cxx 1235
"!pondère", //59 kifglobalthread.cxx 1236
"_modeprédicat", //60 kifglobalthread.cxx 1238
"court", //61 kifglobalthread.cxx 1239
"attentes", //62 kifglobalthread.cxx 1240
"déclenche", //63 kifglobalthread.cxx 1241
"échaine", //64 kifglobalthread.cxx 1243
"énombre", //65 kifglobalthread.cxx 1244
"édécimal", //66 kifglobalthread.cxx 1245
"étable", //67 kifglobalthread.cxx 1246
"renvoie", //68 kifglobalthread.cxx 1316
"prédicat", //69 kifglobalthread.cxx 1339
"instanceprédicat", //70 kifglobalthread.cxx 1340
"variable", //71 kifglobalthread.cxx 1342
"instanceclasse", //72 kifglobalthread.cxx 1343
"uchaine", //73 kifglobalthread.cxx 1345
"tuple", //74 kifglobalthread.cxx 1346
"chainebrute", //75 kifglobalthread.cxx 1347
"chrono", //76 kifglobalthread.cxx 1348
"automate", //77 kifglobalthread.cxx 1349
"transducteur", //78 kifglobalthread.cxx 1350
"grammaire", //79 kifglobalthread.cxx 1351
"disponible", //80 kifglobalthread.cxx 1352
"octet", //81 kifglobalthread.cxx 1354
"bit", //82 kifglobalthread.cxx 1358
"bits", //83 kifglobalthread.cxx 1359
"date", //84 kifglobalthread.cxx 1360
"appel", //85 kifglobalthread.cxx 1361
"bouclec", //86 kifglobalthread.cxx 1364
"boucleu", //87 kifglobalthread.cxx 1365
"boucleb", //88 kifglobalthread.cxx 1366
"bouclen", //89 kifglobalthread.cxx 1367
"boucled", //90 kifglobalthread.cxx 1368
"dicou", //91 kifglobalthread.cxx 1376
"dicon", //92 kifglobalthread.cxx 1377
"dicod", //93 kifglobalthread.cxx 1378
"diconc", //94 kifglobalthread.cxx 1379
"dicond", //95 kifglobalthread.cxx 1380
"diconn", //96 kifglobalthread.cxx 1381
"dicodc", //97 kifglobalthread.cxx 1382
"dicodd", //98 kifglobalthread.cxx 1383
"dicodn", //99 kifglobalthread.cxx 1384
"dicouu", //100 kifglobalthread.cxx 1385
"dicoun", //101 kifglobalthread.cxx 1386
"dicoud", //102 kifglobalthread.cxx 1387
"dicocc", //103 kifglobalthread.cxx 1388
"dicocd", //104 kifglobalthread.cxx 1389
"dicocn", //105 kifglobalthread.cxx 1390
"diconu", //106 kifglobalthread.cxx 1392
"dicodu", //107 kifglobalthread.cxx 1393
"dicotrié", //108 kifglobalthread.cxx 1395
"dicotriéu", //109 kifglobalthread.cxx 1396
"dicotrién", //110 kifglobalthread.cxx 1397
"dicotriéd", //111 kifglobalthread.cxx 1398
"dicotriénc", //112 kifglobalthread.cxx 1399
"dicotriénd", //113 kifglobalthread.cxx 1400
"dicotriénn", //114 kifglobalthread.cxx 1401
"dicotriédc", //115 kifglobalthread.cxx 1402
"dicotriédd", //116 kifglobalthread.cxx 1403
"dicotriédn", //117 kifglobalthread.cxx 1404
"dicotriécc", //118 kifglobalthread.cxx 1405
"dicotriéuu", //119 kifglobalthread.cxx 1406
"dicotriéun", //120 kifglobalthread.cxx 1407
"dicotriéud", //121 kifglobalthread.cxx 1408
"dicotriénu", //122 kifglobalthread.cxx 1410
"dicotriédu", //123 kifglobalthread.cxx 1411
"dicotriécd", //124 kifglobalthread.cxx 1413
"dicotriécn", //125 kifglobalthread.cxx 1414
"dicopremier", //126 kifglobalthread.cxx 1416
"dicopremieru", //127 kifglobalthread.cxx 1417
"dicopremiern", //128 kifglobalthread.cxx 1418
"dicobn", //129 kifglobalthread.cxx 1420
"dicobnn", //130 kifglobalthread.cxx 1421
"dicobnd", //131 kifglobalthread.cxx 1422
"dicobnc", //132 kifglobalthread.cxx 1423
"dicobnu", //133 kifglobalthread.cxx 1424
"dicopremierd", //134 kifglobalthread.cxx 1426
"dicopremiernc", //135 kifglobalthread.cxx 1427
"dicopremiernd", //136 kifglobalthread.cxx 1428
"dicopremiernn", //137 kifglobalthread.cxx 1429
"dicopremierdc", //138 kifglobalthread.cxx 1430
"dicopremierdd", //139 kifglobalthread.cxx 1431
"dicopremierdn", //140 kifglobalthread.cxx 1432
"dicopremiercc", //141 kifglobalthread.cxx 1433
"dicopremiercd", //142 kifglobalthread.cxx 1434
"dicopremiercn", //143 kifglobalthread.cxx 1435
"dicopremieruu", //144 kifglobalthread.cxx 1437
"dicopremierun", //145 kifglobalthread.cxx 1438
"dicopremierud", //146 kifglobalthread.cxx 1439
"dicopremiernu", //147 kifglobalthread.cxx 1440
"dicopremierdu", //148 kifglobalthread.cxx 1441
"sac", //149 kifglobalthread.cxx 1444
"sacchaines", //150 kifglobalthread.cxx 1445
"sacuchaines", //151 kifglobalthread.cxx 1446
"sacdécimaux", //152 kifglobalthread.cxx 1447
"sacnombres", //153 kifglobalthread.cxx 1448
"tableuchaines", //154 kifglobalthread.cxx 1451
"tableoctets", //155 kifglobalthread.cxx 1454
"tableau", //156 kifglobalthread.cxx 1457
"terme", //157 kifglobalthread.cxx 1459
"liste", //158 kifglobalthread.cxx 1460
"itérateur", //159 kifglobalthread.cxx 1461
"fichier", //160 kifglobalthread.cxx 1462
"ffichier", //161 kifglobalthread.cxx 1463
"bool", //162 kifglobalthread.cxx 1509
"auto", //163 kifglobalthread.cxx 1511
"soit", //164 kifglobalthread.cxx 1512
"dicoc", //165 kifglobalthread.cxx 1516
"diconc", //166 kifglobalthread.cxx 1520
"diconn", //167 kifglobalthread.cxx 1521
"dicond", //168 kifglobalthread.cxx 1522
"dicodc", //169 kifglobalthread.cxx 1523
"dicodn", //170 kifglobalthread.cxx 1524
"dicodd", //171 kifglobalthread.cxx 1525
"dicocc", //172 kifglobalthread.cxx 1530
"dicocn", //173 kifglobalthread.cxx 1531
"dicocd", //174 kifglobalthread.cxx 1532
"dicouu", //175 kifglobalthread.cxx 1535
"dicoun", //176 kifglobalthread.cxx 1536
"dicoud", //177 kifglobalthread.cxx 1537
"diconu", //178 kifglobalthread.cxx 1539
"dicotriéc", //179 kifglobalthread.cxx 1557
"dicotriénc", //180 kifglobalthread.cxx 1562
"dicotriénn", //181 kifglobalthread.cxx 1563
"dicotriénd", //182 kifglobalthread.cxx 1564
"dicotriédc", //183 kifglobalthread.cxx 1565
"dicotriédn", //184 kifglobalthread.cxx 1566
"dicotriédd", //185 kifglobalthread.cxx 1567
"dicotriécc", //186 kifglobalthread.cxx 1569
"dicotriécn", //187 kifglobalthread.cxx 1570
"dicotriécd", //188 kifglobalthread.cxx 1571
"dicotriécc", //189 kifglobalthread.cxx 1572
"dicotriécn", //190 kifglobalthread.cxx 1573
"dicotriécd", //191 kifglobalthread.cxx 1574
"dicotriéuu", //192 kifglobalthread.cxx 1575
"dicotriéun", //193 kifglobalthread.cxx 1576
"dicotriéud", //194 kifglobalthread.cxx 1577
"dicotriénu", //195 kifglobalthread.cxx 1579
"dicopremierc", //196 kifglobalthread.cxx 1598
"dicobnn", //197 kifglobalthread.cxx 1608
"dicobnd", //198 kifglobalthread.cxx 1609
"dicobnc", //199 kifglobalthread.cxx 1610
"dicobnu", //200 kifglobalthread.cxx 1611
"dicopremieruu", //201 kifglobalthread.cxx 1614
"dicopremierun", //202 kifglobalthread.cxx 1616
"dicopremierud", //203 kifglobalthread.cxx 1618
"dicopremiernu", //204 kifglobalthread.cxx 1620
"dicopremierdu", //205 kifglobalthread.cxx 1622
"dicopremiernc", //206 kifglobalthread.cxx 1624
"dicopremiernn", //207 kifglobalthread.cxx 1625
"dicopremiernd", //208 kifglobalthread.cxx 1626
"dicopremierdc", //209 kifglobalthread.cxx 1627
"dicopremierdn", //210 kifglobalthread.cxx 1628
"dicopremierdd", //211 kifglobalthread.cxx 1629
"dicopremiercc", //212 kifglobalthread.cxx 1631
"dicopremiercn", //213 kifglobalthread.cxx 1632
"dicopremiercd", //214 kifglobalthread.cxx 1633
"dicopremiercc", //215 kifglobalthread.cxx 1634
"dicopremiercn", //216 kifglobalthread.cxx 1635
"dicopremiercd", //217 kifglobalthread.cxx 1636
"kiff", //218 kifglobalthread.cxx 1681
"varprédicat", //219 kifglobalthread.cxx 1702
"dicodu", //220 kifglobalthread.cxx 1757
"dicotriédu", //221 kifglobalthread.cxx 1799
"ritérateur", //222 kifglobalthread.cxx 1873
"arbre", //223 kifglobalthread.cxx 1874
"est", //224 kifglobalthread.cxx 2023
"estpas", //225 kifglobalthread.cxx 2024
"ou", //226 kifglobalthread.cxx 2059
"et", //227 kifglobalthread.cxx 2060
"sur", //228 kifglobalthread.cxx 2062
"pasdans", //229 kifglobalthread.cxx 2063
"_compatibilités", //230 kifglobalthread.cxx 2268
"infos", //231 kifglobalthread.cxx 2277
"estun", //232 kifglobalthread.cxx 2278
"type", //233 kifglobalthread.cxx 2279
"estunconteneur", //234 kifglobalthread.cxx 2280
"estuntableau", //235 kifglobalthread.cxx 2282
"nom", //236 kifglobalthread.cxx 2283
"min", //237 kifglobalthread.cxx 2287
"max", //238 kifglobalthread.cxx 2288
"raz", //239 kifglobalthread.cxx 2289
"ajouteattentes", //240 kifglobalthread.cxx 2292
"charge", //241 kifglobalthread.cxx 2297
"chargelemmes", //242 kifglobalthread.cxx 2304
"modèle", //243 kifglobalthread.cxx 2305
"recherche", //244 kifglobalthread.cxx 2306
"détecteétatfinal", //245 kifglobalthread.cxx 2307
"chargecompact", //246 kifglobalthread.cxx 2308
"enregistre", //247 kifglobalthread.cxx 2309
"définiscodetrait", //248 kifglobalthread.cxx 2311
"distanceédition", //249 kifglobalthread.cxx 2312
"ajoute", //250 kifglobalthread.cxx 2314
"initialisetransformations", //251 kifglobalthread.cxx 2317
"génère", //252 kifglobalthread.cxx 2321
"compilergx", //253 kifglobalthread.cxx 2323
"haut", //254 kifglobalthread.cxx 2324
"bas", //255 kifglobalthread.cxx 2325
"analyse", //256 kifglobalthread.cxx 2326
"frère", //257 kifglobalthread.cxx 2340
"précédent", //258 kifglobalthread.cxx 2342
"père", //259 kifglobalthread.cxx 2343
"fils", //260 kifglobalthread.cxx 2344
"derniernoeud", //261 kifglobalthread.cxx 2345
"profondeur", //262 kifglobalthread.cxx 2346
"élague", //263 kifglobalthread.cxx 2347
"isole", //264 kifglobalthread.cxx 2348
"redirigeio", //265 kifglobalthread.cxx 2360
"hachage", //266 kifglobalthread.cxx 2362
"renverse", //267 kifglobalthread.cxx 2365
"ord", //268 kifglobalthread.cxx 2366
"octets", //269 kifglobalthread.cxx 2367
"format", //270 kifglobalthread.cxx 2368
"base", //271 kifglobalthread.cxx 2369
"remplit", //272 kifglobalthread.cxx 2370
"comble", //273 kifglobalthread.cxx 2371
"décompose", //274 kifglobalthread.cxx 2374
"empile", //275 kifglobalthread.cxx 2375
"tailleo", //276 kifglobalthread.cxx 2377
"lisp", //277 kifglobalthread.cxx 2378
"balisage", //278 kifglobalthread.cxx 2379
"retaille", //279 kifglobalthread.cxx 2380
"évalue", //280 kifglobalthread.cxx 2382
"vershtml", //281 kifglobalthread.cxx 2384
"versxml", //282 kifglobalthread.cxx 2385
"remplaceregexip", //283 kifglobalthread.cxx 2386
"remplace", //284 kifglobalthread.cxx 2389
"captestd", //285 kifglobalthread.cxx 2390
"capteerr", //286 kifglobalthread.cxx 2391
"éclate", //287 kifglobalthread.cxx 2392
"éclatev", //288 kifglobalthread.cxx 2393
"segmente", //289 kifglobalthread.cxx 2394
"ssegmente", //290 kifglobalthread.cxx 2395
"compte", //291 kifglobalthread.cxx 2396
"cherche", //292 kifglobalthread.cxx 2397
"rcherche", //293 kifglobalthread.cxx 2398
"retirepremier", //294 kifglobalthread.cxx 2399
"retiredernier", //295 kifglobalthread.cxx 2400
"estutf8", //296 kifglobalthread.cxx 2401
"ngrammes", //297 kifglobalthread.cxx 2402
"latin", //298 kifglobalthread.cxx 2404
"dos", //299 kifglobalthread.cxx 2405
"dosversutf8", //300 kifglobalthread.cxx 2406
"gauche", //301 kifglobalthread.cxx 2407
"positionoctet", //302 kifglobalthread.cxx 2408
"positioncarac", //303 kifglobalthread.cxx 2409
"droit", //304 kifglobalthread.cxx 2410
"milieu", //305 kifglobalthread.cxx 2411
"estalphabétique", //306 kifglobalthread.cxx 2412
"estconsonne", //307 kifglobalthread.cxx 2413
"estvoyelle", //308 kifglobalthread.cxx 2414
"estponctuation", //309 kifglobalthread.cxx 2415
"estchiffre", //310 kifglobalthread.cxx 2416
"extrait", //311 kifglobalthread.cxx 2417
"enmajuscule", //312 kifglobalthread.cxx 2418
"enminuscule", //313 kifglobalthread.cxx 2419
"majuscule", //314 kifglobalthread.cxx 2420
"désaccentue", //315 kifglobalthread.cxx 2421
"minuscule", //316 kifglobalthread.cxx 2422
"rogne", //317 kifglobalthread.cxx 2423
"rognegauche", //318 kifglobalthread.cxx 2424
"rognedroit", //319 kifglobalthread.cxx 2425
"dernier", //320 kifglobalthread.cxx 2426
"insère", //321 kifglobalthread.cxx 2427
"interroge", //322 kifglobalthread.cxx 2435
"retire", //323 kifglobalthread.cxx 2437
"poids", //324 kifglobalthread.cxx 2438
"duplique", //325 kifglobalthread.cxx 2445
"déplace", //326 kifglobalthread.cxx 2447
"fusionne", //327 kifglobalthread.cxx 2451
"texte", //328 kifglobalthread.cxx 2454
"applatis", //329 kifglobalthread.cxx 2455
"teste", //330 kifglobalthread.cxx 2456
"joins", //331 kifglobalthread.cxx 2457
"réserve", //332 kifglobalthread.cxx 2458
"trie", //333 kifglobalthread.cxx 2463
"permute", //334 kifglobalthread.cxx 2464
"mélange", //335 kifglobalthread.cxx 2465
"unique", //336 kifglobalthread.cxx 2466
"trienombre", //337 kifglobalthread.cxx 2467
"triedécimal", //338 kifglobalthread.cxx 2468
"triechaine", //339 kifglobalthread.cxx 2469
"trielong", //340 kifglobalthread.cxx 2470
"somme", //341 kifglobalthread.cxx 2471
"produit", //342 kifglobalthread.cxx 2472
"empileentête", //343 kifglobalthread.cxx 2497
"dépileenqueue", //344 kifglobalthread.cxx 2499
"empileenqueue", //345 kifglobalthread.cxx 2500
"dépileentête", //346 kifglobalthread.cxx 2501
"premier", //347 kifglobalthread.cxx 2512
"exécute", //348 kifglobalthread.cxx 2530
"exécutetâche", //349 kifglobalthread.cxx 2533
"kvariables", //350 kifglobalthread.cxx 2534
"finexécution", //351 kifglobalthread.cxx 2535
"fonctiondebug", //352 kifglobalthread.cxx 2536
"fermedebug", //353 kifglobalthread.cxx 2537
"compile", //354 kifglobalthread.cxx 2538
"ouvre", //355 kifglobalthread.cxx 2539
"nettoie", //356 kifglobalthread.cxx 2540
"exportées", //357 kifglobalthread.cxx 2541
"inverse", //358 kifglobalthread.cxx 2546
"clefs", //359 kifglobalthread.cxx 2560
"valeurs", //360 kifglobalthread.cxx 2561
"items", //361 kifglobalthread.cxx 2562
"évaluation", //362 kifglobalthread.cxx 2576
"amorce", //363 kifglobalthread.cxx 2577
"ouvreenécriture", //364 kifglobalthread.cxx 2609
"ouvreenajout", //365 kifglobalthread.cxx 2610
"ouvreenlecture", //366 kifglobalthread.cxx 2611
"metsignature", //367 kifglobalthread.cxx 2612
"signature", //368 kifglobalthread.cxx 2613
"chasse", //369 kifglobalthread.cxx 2614
"remet", //370 kifglobalthread.cxx 2616
"ferme", //371 kifglobalthread.cxx 2617
"positionne", //372 kifglobalthread.cxx 2618
"dit", //373 kifglobalthread.cxx 2619
"écrit", //374 kifglobalthread.cxx 2620
"écritln", //375 kifglobalthread.cxx 2621
"écritbin", //376 kifglobalthread.cxx 2622
"lit", //377 kifglobalthread.cxx 2623
"litln", //378 kifglobalthread.cxx 2624
"findefichier", //379 kifglobalthread.cxx 2630
"metdate", //380 kifglobalthread.cxx 2639
"année", //381 kifglobalthread.cxx 2641
"mois", //382 kifglobalthread.cxx 2642
"jour", //383 kifglobalthread.cxx 2643
"jourannée", //384 kifglobalthread.cxx 2644
"joursemaine", //385 kifglobalthread.cxx 2645
"heure", //386 kifglobalthread.cxx 2646
"minute", //387 kifglobalthread.cxx 2647
"seconde", //388 kifglobalthread.cxx 2649
"raz", //389 kifglobalthread.cxx 2667
"abs", //390 kifglobalthread.cxx 2680
"acos", //391 kifglobalthread.cxx 2681
"acosh", //392 kifglobalthread.cxx 2682
"asin", //393 kifglobalthread.cxx 2683
"asinh", //394 kifglobalthread.cxx 2684
"atan", //395 kifglobalthread.cxx 2685
"atanh", //396 kifglobalthread.cxx 2686
"raccubique", //397 kifglobalthread.cxx 2687
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
"plusprocheentier", //412 kifglobalthread.cxx 2702
"rint", //413 kifglobalthread.cxx 2703
"arrondi", //414 kifglobalthread.cxx 2704
"sin", //415 kifglobalthread.cxx 2705
"sinh", //416 kifglobalthread.cxx 2706
"racar", //417 kifglobalthread.cxx 2707
"tan", //418 kifglobalthread.cxx 2708
"tanh", //419 kifglobalthread.cxx 2709
"tgamma", //420 kifglobalthread.cxx 2710
"tronque", //421 kifglobalthread.cxx 2711
"car", //422 kifglobalthread.cxx 2717
"réinstaureio", //423 kifglobalthread.cxx 2719
"partie", //424 kifglobalthread.cxx 2725
"facteurs", //425 kifglobalthread.cxx 2727
"premier", //426 kifglobalthread.cxx 2728
"radian", //427 kifglobalthread.cxx 2731
"pair", //428 kifglobalthread.cxx 2732
"impair", //429 kifglobalthread.cxx 2733
"degré", //430 kifglobalthread.cxx 2734
"simplifie", //431 kifglobalthread.cxx 2742
"nd", //432 kifglobalthread.cxx 2744
"euclidien", //433 kifglobalthread.cxx 2747
"suivant", //434 kifglobalthread.cxx 2750
"fin", //435 kifglobalthread.cxx 2751
"metvaleur", //436 kifglobalthread.cxx 2752
"nfin", //437 kifglobalthread.cxx 2753
"commence", //438 kifglobalthread.cxx 2754
"clef", //439 kifglobalthread.cxx 2755
"valeur", //440 kifglobalthread.cxx 2756
"valeurtype", //441 kifglobalthread.cxx 2758
"_pile", //442 kifglobalthread.cxx 3040
"_nomfichier", //443 kifglobalthread.cxx 3041
"pause", //444 kifglobalthread.cxx 3106
"suspend", //445 kifglobalthread.cxx 3107
"éval", //446 kifglobalthread.cxx 3108
"depuisjson", //447 kifglobalthread.cxx 3109
"exécute", //448 kifglobalthread.cxx 3110
"intervalle", //449 kifglobalthread.cxx 3117
"lève", //450 kifglobalthread.cxx 3135
"bloque", //451 kifglobalthread.cxx 3152
"déverrouille", //452 kifglobalthread.cxx 3153
"verrouille", //453 kifglobalthread.cxx 3154
"attends", //454 kifglobalthread.cxx 3155
"attendsquefaux", //455 kifglobalthread.cxx 3156
"attendsfinjoindre", //456 kifglobalthread.cxx 3157
"enattente", //457 kifglobalthread.cxx 3158
"libère", //458 kifglobalthread.cxx 3159
"tue", //459 kifglobalthread.cxx 3160
"_erreursurclef", //460 kifglobalthread.cxx 3162
"_sonderamassemiette", //461 kifglobalthread.cxx 3163
"_ramassemietteclasses", //462 kifglobalthread.cxx 3164
"_ramassemiette", //463 kifglobalthread.cxx 3165
"_version", //464 kifglobalthread.cxx 3166
"_sortie", //465 kifglobalthread.cxx 3167
"affiche", //466 kifglobalthread.cxx 3169
"chasse", //467 kifglobalthread.cxx 3170
"afficheligne", //468 kifglobalthread.cxx 3171
"affichej", //469 kifglobalthread.cxx 3172
"affichejerr", //470 kifglobalthread.cxx 3173
"affichejligne", //471 kifglobalthread.cxx 3174
"affichejligneerr", //472 kifglobalthread.cxx 3175
"affichetouslesprédicats", //473 kifglobalthread.cxx 3176
"trouvetous", //474 kifglobalthread.cxx 3177
"rétractetout", //475 kifglobalthread.cxx 3178
"afficheerr", //476 kifglobalthread.cxx 3179
"afficheligneerr", //477 kifglobalthread.cxx 3180
"idtâche", //478 kifglobalthread.cxx 3186
"tâcheparent", //479 kifglobalthread.cxx 3187
"fonctiondebug", //480 kifglobalthread.cxx 3189
"debugraz", //481 kifglobalthread.cxx 3191
"donnéesdebug", //482 kifglobalthread.cxx 3192
"infopile", //483 kifglobalthread.cxx 3194
"iclef", //484 kifglobalthread.cxx 3242
"touche", //485 kifglobalthread.cxx 3271
"aléatoire", //486 kifglobalthread.cxx 3274
"valeurestun", //487 kifglobalthread.cxx 3352
"génère(chaine entrée,chaine sortie,bool norm,nombre latin): Construit un transducteur à partir d'un fichier texte, dont les lignes paires correspondent aux formes de surface et les lignes impaires à lemme+traits. La normalisation permet d'intégrer la possibilité de reconnaître un mot en dépis de sa casse. Latin correspond au numero d'encodage des tables latin.", //488 kifglobalthread.cxx 3384
"enregistre(chaine sortie,bool norm,nombre encodage): Enregistre un transducteur dans un fichier. Les deux derniers paramètres sont optionnels.", //489 kifglobalthread.cxx 3385
"compilergx(chaine rgx,tablechaines traits,chaine nomfichier): Construit un transducteur à partir d'une expression régulière. Le fichier est optionnel, mais dans ce cas le résultat sera enregistré dans un fichier.", //490 kifglobalthread.cxx 3386
"charge(chaine file): charge un fichier transducteur.", //491 kifglobalthread.cxx 3387
"ajoute(conteneur,bool norm, nombre encodage): transforme un conteneur (table ou dictionnaire) en un lexique transducteur. Si le conteneur est une table, son nombre d'éléments doit être pair.", //492 kifglobalthread.cxx 3388
"haut(chaine mot,nombre seuil, nombre actions): Recherche un mot sur la partie haute du transducteur. Le parcours peut être contraint avec un seuil et des actions (a_insère, a_premier, a_détruit, a_commute, a_change)", //493 kifglobalthread.cxx 3389
"bas(chaine mot): Recherche un mot sur la partie basse du transducteur.", //494 kifglobalthread.cxx 3390
"analyse(chaine phrase): Analyse une séquence de mots avec un transducteur.", //495 kifglobalthread.cxx 3391
"poids(): Renvoie le poids de ce prédicat.", //496 kifglobalthread.cxx 3394
"interroge(predicat,var1,var2...): Construit et évalue un prédicat à la volée.", //497 kifglobalthread.cxx 3395
"enregistre(sqlite bd): Enregistre un prédicat dans la base de données. Si db est omis, alors l'enregistrer dans la base de connaissance", //498 kifglobalthread.cxx 3396
"retire(sqlite bd): Retire un prédicat de la base de données. Si db est omis, alors le retirer de la base de connaissance", //499 kifglobalthread.cxx 3397
"hachage(): Renvoie la clef de hachage d'une chaine", //500 kifglobalthread.cxx 3399
"retaille(nb): Modifie la taille d'une chainebrute", //501 kifglobalthread.cxx 3400
"json(): Renvoie la version encodée en JSON de la chaine", //502 kifglobalthread.cxx 3401
"positionoctet(nombre pos): convertit une position caractère en une position octet (très utile pour les chaines UTF8)", //503 kifglobalthread.cxx 3402
"positioncarac(nombre pos): convertit une position octet en une position caractère.", //504 kifglobalthread.cxx 3403
"taille(): Renvoie la taille de la chaine.", //505 kifglobalthread.cxx 3404
"évalue(): Evalue les métacaractères dans une chaine et retourne la chaine évaluée", //506 kifglobalthread.cxx 3405
"raz(): Nettoie le contenu de la chaine", //507 kifglobalthread.cxx 3406
"segmente(bool virgule,bool séparateur,bool groupe): Segmente un texte en mots et ponctuation. Si virgule vaut 'vrai' alors le séparateur décimal est une ',' sinon un '.'. Si 'séparateur' alors '3.000,10' est accepté comme nombre. Si 'groupe' est vrai alors '3a' est une sous-chaine valide.", //508 kifglobalthread.cxx 3407
"ssegmente(dictionaire garde): Segment une chaine en mots et ponctuations, en fonction d'une garde.", //509 kifglobalthread.cxx 3408
"éclate(chaine exploseur,table vect): explose une chaine selon exploseur et enregistre le résultat sous la forme d'une table. Si exploseur=='', alors la chaine est explosée en une table de caractères", //510 kifglobalthread.cxx 3409
"éclatev(chaine exploseur,table vect): explose une chaine selon exploseur et enregistre le résultat sous la forme d'une table. Si exploseur=='', alors la chaine est explosée en une table de caractères. Les chaines vides sont conservées dans le résultat final.", //511 kifglobalthread.cxx 3410
"retirepremier(nombre nb): retire les nb premiers caractères d'une chaine", //512 kifglobalthread.cxx 3411
"retiredernier(nombre nb): retire les nb derniers caractères d'une chaine", //513 kifglobalthread.cxx 3412
"remplit(nombre nb,chaine c): crée une chaine de nb caractères c", //514 kifglobalthread.cxx 3413
"comble(nombre nb,chaine c,bool combleàlafin): rajoute nb caractères c à la chaine.Le dernier paramètre est optionnel", //515 kifglobalthread.cxx 3414
"format(p1,p2,p3): Crée une chaine à partir du format enregistré dans la chaine courante, où chaque '%x' correspond à un paramètre, 'x' est la position de ce paramètre dans la liste des arguments. 'x' commence à 1.", //516 kifglobalthread.cxx 3415
"ord(): renvoie le code ASCII du caractère en tête, or une liste de tous les codes ASCII si le receveur est une table", //517 kifglobalthread.cxx 3416
"base(nombre b): Renvoie la valeur numérique correspondant à la chaine courrante en base b", //518 kifglobalthread.cxx 3417
"gauche(nombre nb): renvoie les nb premiers caractères d'une chaine", //519 kifglobalthread.cxx 3418
"positionoctet(nombre i): Convertit une position en octet en une position en caractère", //520 kifglobalthread.cxx 3419
"positioncarac(nombre pos): convertit une position octet en une position caractère.", //521 kifglobalthread.cxx 3420
"droit(nombre nb): renvoie les nb derniers caractères d'une chaine", //522 kifglobalthread.cxx 3421
"milieu(nombre pos,nombre nb): renvoie les nb caractères commençant à la position pos d'une chaine", //523 kifglobalthread.cxx 3422
"renverse(): inverse la chaine", //524 kifglobalthread.cxx 3423
"dépile(i): retire le caractère à la position i", //525 kifglobalthread.cxx 3424
"dépile(): retire le dernier caractère", //526 kifglobalthread.cxx 3425
"dernier(): renvoie le dernier caractère", //527 kifglobalthread.cxx 3426
"insère(i,s): insère la chaine s en i. Si i est -1, alors insère s entre chaque caractère dans la chaine d'entrée", //528 kifglobalthread.cxx 3427
"rogne(): retire les caractères d'espace", //529 kifglobalthread.cxx 3428
"rognedroit(): retire les caractères d'espace à droite", //530 kifglobalthread.cxx 3429
"rognegauche(): retire les caractères d'espace à gauche", //531 kifglobalthread.cxx 3430
"désaccentue(): Retire les accents des lettres accentuées", //532 kifglobalthread.cxx 3431
"majuscule(): Met la chaine en majuscule", //533 kifglobalthread.cxx 3432
"minuscule(): Met la chaine en minuscule", //534 kifglobalthread.cxx 3433
"enminuscule(): Teste si la chaine est uniquement en minuscule", //535 kifglobalthread.cxx 3434
"enmajuscule(): Teste si la chaine est uniquement en minuscule", //536 kifglobalthread.cxx 3435
"estalphabétique(): Teste si une chaine ne contient que des caractères alphabétiques", //537 kifglobalthread.cxx 3436
"estconsonne(): Teste si une chaine ne contient que des consonnes", //538 kifglobalthread.cxx 3437
"estvoyelle(): Teste si une chaine ne contient que des voyelles", //539 kifglobalthread.cxx 3438
"estponctuation(): Vérifie si un caractère est une ponctuation", //540 kifglobalthread.cxx 3439
"table v=extrait(nombre pos,chaine depuis,chaine c1,chaine c2...): extrait les sous-chaines entre 'depuis' et 'c1'...'cn' (la chaine la plus courte est utilisée). Renvoie une table de chaines", //541 kifglobalthread.cxx 3440
"estchiffre(): Teste si une chaine ne contient que des chiffres", //542 kifglobalthread.cxx 3441
"latin(): convertit une chaine UTF8 en LATIN", //543 kifglobalthread.cxx 3442
"utf8(nombre table): convertit une chaine en LATIN en UTF8. Par défaut il s'agit by default it is ISO/IEC 8859 part 1.", //544 kifglobalthread.cxx 3443
"dos(): convertit une chaine en encodage DOS", //545 kifglobalthread.cxx 3444
"dosversf8(): convertit une chaine DOS en UTF8", //546 kifglobalthread.cxx 3445
"estutf8(): Renvoie vrai si la chaine est encodée en UTF8", //547 kifglobalthread.cxx 3446
"ngrammes(nombre r): Renvoie un vecteur de tous les n-grammes de rang r", //548 kifglobalthread.cxx 3447
"compte(chaine s,nombre p,nombre fin): Compte le nombre de sous-chaines à partir de p jusqu'à fin.", //549 kifglobalthread.cxx 3448
"cherche(chaine sub,nombre pos): Renvoie la position de la sous-chaine sub commençant à la position pos", //550 kifglobalthread.cxx 3449
"tailleo(): Renvoie la taille en octets de la chaine", //551 kifglobalthread.cxx 3450
"rcherche(chaine sub,nombre pos): Renvoie la position de la sous-chaine sub par l'arrière commençant à la position pos", //552 kifglobalthread.cxx 3451
"remplaceregexip(sub,str): Remplace les chaines correspondant à sub avec str", //553 kifglobalthread.cxx 3452
"levenshtein(chaine s, bool octet): Renvoie la distance d'édition avec 's' selon l'algorithme de Levenshtein. Si octet est 'vrai' alors la comparaison se fait par octet. 'octet' est optionel.", //554 kifglobalthread.cxx 3453
"distanceédition(chaine c,bool octet): Calcule la distance d'édition avec 's'. Si 'octet' vaut 'vrai', la comparaison se fait en octet. 'octet' est optionel.", //555 kifglobalthread.cxx 3454
"html(): Renvoie la chaine sous la forme d'une chaine HTML compatible ou d'une table de chaine", //556 kifglobalthread.cxx 3455
"vershtml(): Renvoie la chaine sous la forme d'une chaine HTML compatible ou d'une table de chaine", //557 kifglobalthread.cxx 3456
"versxml(): Revoie la chaine en une chaine compatible XML", //558 kifglobalthread.cxx 3457
"regexip(sub): Trouve les chaines correspondant à sub", //559 kifglobalthread.cxx 3458
"remplace(sub,str): Remplace la sous-chaines correspondant à sub avec str", //560 kifglobalthread.cxx 3459
"captestd(bool): capte ou relâche la sortie standard", //561 kifglobalthread.cxx 3460
"capteerr(bool): capte ou relâche la sortie erreur", //562 kifglobalthread.cxx 3461
"remplacergx", //563 kifglobalthread.cxx 3462
"remplacergx(rgx,str): Remplace la sous-chaine correspondant à rgx avec str", //564 kifglobalthread.cxx 3462
"éclatergx", //565 kifglobalthread.cxx 3463
"éclatergx(rgx): Explose  chaine avec l'expression regulière rgx. Renvoie une table de sous-chaines.", //566 kifglobalthread.cxx 3463
"boolean=regex(rgx): Teste si l'expression regulière rgx s'applique à la chaine|nombre=regex(rgx): Renvoie la position de la sous-chaine correspondant à rgx|chaine=regex(rgx): Renvoie la sous-chaine correspondant à rgx|table=regex(rgx): Renvoie toutes les sous-chaines correspondant à rgx", //567 kifglobalthread.cxx 3464
"décompose(): Décompose la chaine comme une morceau de code et renvoie l'évaluation sous la forme d'une table.", //568 kifglobalthread.cxx 3465
"lisp(): Décompose une chaine parenthétisée et la renvoie sous la forme d'une table.|lisp(chaine o,chaine f): Décompose une chaine parenthétisée dont les caractères ouvrants et fermants sont fournis et la renvoie sous la forme d'une table", //569 kifglobalthread.cxx 3466
"balisage(chaine o,chaine f): Segmente une chaine à la façon d'une chaine parenthétisée sur la base des chaines o et f.", //570 kifglobalthread.cxx 3467
"facteurs(): renvoie la décomposition en facteurs premiers", //571 kifglobalthread.cxx 3468
"premier(): renvoie vrai si le nombre est premier", //572 kifglobalthread.cxx 3469
"base(nombre b): renvoie une chaine représentant le nombre dans la base b", //573 kifglobalthread.cxx 3470
"bit(nombre i): renvoie vrai si le ième bit est 1.", //574 kifglobalthread.cxx 3471
"inverse(): Inverse le dénominateur et le numérateur de la fraction", //575 kifglobalthread.cxx 3476
"partie(bits x): Renvoie vrai si le bits de x sont tous dans les bits courants", //576 kifglobalthread.cxx 3478
"d(): Dénominateur de la fraction (avec un paramètre: met la valeur, sans paramètre renvoie la valeur)", //577 kifglobalthread.cxx 3480
"infos(chaine): infos à propos d'une méthode", //578 kifglobalthread.cxx 3481
"méthodes(): Retourne a table de toutes les méthodes disponibles", //579 kifglobalthread.cxx 3482
"simplifie(): Simplification", //580 kifglobalthread.cxx 3483
"n(): Numérateur de la fraction (avec un paramètre: met la valeur, sans paramètre renvoie la valeur)", //581 kifglobalthread.cxx 3485
"nd(nombre n,nombre d): Initialise le Numérateur et le Dénominateur de la fraction", //582 kifglobalthread.cxx 3486
"car(): Renvoie le caractère dont le code est la valeur de la variable", //583 kifglobalthread.cxx 3487
"#(): renvoie le complément en bits", //584 kifglobalthread.cxx 3489
"format(chaine form): renvoie une chaine formatée selon le patron dans form. (ce format est le même que celui de sprintf en C++)", //585 kifglobalthread.cxx 3491
"radian(): Convertit une valeur en degré en une valeur en radian", //586 kifglobalthread.cxx 3493
"pair(): Renvoie vrai si la valeur est un nombre pair.", //587 kifglobalthread.cxx 3494
"impair(): Renvoie vrai si la valeur est un nombre impair.", //588 kifglobalthread.cxx 3495
"degré(): Convertit une valeur en radian en une valeur en degré", //589 kifglobalthread.cxx 3496
"saisie(): Lit une chaine depuis le clavier", //590 kifglobalthread.cxx 3501
"octets(): Renvoie la chaine comme table d'octets", //591 kifglobalthread.cxx 3502
"saisie(): Lit une chaine depuis le clavier", //592 kifglobalthread.cxx 3503
"abs(): renvoie la valeur absolue", //593 kifglobalthread.cxx 3506
"acos(): arc cosinus", //594 kifglobalthread.cxx 3508
"acosh(): arc cosinus hyperbolique", //595 kifglobalthread.cxx 3510
"asin(): arc sinus", //596 kifglobalthread.cxx 3512
"asinh(): arc sinus hyperbolique", //597 kifglobalthread.cxx 3514
"atan(): arc tangent", //598 kifglobalthread.cxx 3516
"atanh(): arc tangent hyperbolique", //599 kifglobalthread.cxx 3518
"raccubique(): racine cubique", //600 kifglobalthread.cxx 3520
"cos(): cosinus", //601 kifglobalthread.cxx 3522
"cosh(): cosinus hyperbolique", //602 kifglobalthread.cxx 3524
"erf(): fonction d'erreur", //603 kifglobalthread.cxx 3526
"erfc(): fonction d'erreur complémentaire", //604 kifglobalthread.cxx 3528
"exp(): fonction exponentiel", //605 kifglobalthread.cxx 3530
"exp2(): fonction exponentielle binaire", //606 kifglobalthread.cxx 3532
"expm1(): exponential minus one", //607 kifglobalthread.cxx 3534
"plancher(): valeur plancher", //608 kifglobalthread.cxx 3536
"lgamma(): fonction log-gamma", //609 kifglobalthread.cxx 3538
"log(): logarithme base 10", //610 kifglobalthread.cxx 3540
"ln(): renvoie le log neperien", //611 kifglobalthread.cxx 3542
"log1p(): logarithme plus un", //612 kifglobalthread.cxx 3544
"log2(): logarithme binaire", //613 kifglobalthread.cxx 3546
"logb(): logarithme à base virgule flottante", //614 kifglobalthread.cxx 3548
"plusprocheentier(): valeur entière la plus proche", //615 kifglobalthread.cxx 3550
"rentier(): valeur entière la plus proche", //616 kifglobalthread.cxx 3552
"arrondi(): arrondi au plus proche", //617 kifglobalthread.cxx 3554
"sin(): sinus", //618 kifglobalthread.cxx 3556
"sinh(): sinus hyperbolique", //619 kifglobalthread.cxx 3558
"racar(): racine carrée", //620 kifglobalthread.cxx 3560
"tan(): tangente", //621 kifglobalthread.cxx 3562
"tanh(): tangente hyperbolique", //622 kifglobalthread.cxx 3564
"tgamma(): fonction gamma", //623 kifglobalthread.cxx 3566
"tronque(): valeur tronquée", //624 kifglobalthread.cxx 3568
"retaille(nb): Redimmensionne une table", //625 kifglobalthread.cxx 3571
"ngrammes(nombre r,chaine sep): Renvoie un vecteur de tous les n-grammes de rang r. Les chaines sont concaténées avec 'sep'", //626 kifglobalthread.cxx 3572
"déplace(nombre p_depuis,nombre p_vers): déplace un élément depuis p_depuis vers p_vers.", //627 kifglobalthread.cxx 3573
"empile(a): rajoute en queue de table", //628 kifglobalthread.cxx 3574
"distanceédition(chaine c): Calcule la distance d'édition avec 's'", //629 kifglobalthread.cxx 3575
"applatis(): fusionne toutes les sous-listes dans la liste principale", //630 kifglobalthread.cxx 3576
"fusionne(v): fusionne les éléments de v dans le conteneur", //631 kifglobalthread.cxx 3577
"octets(): Renvoie la chaine correspondant à une table d'octets. Chaque valeur doit être comprise entre 0..255", //632 kifglobalthread.cxx 3578
"texte(): Renvoie la chaine correspondant à une table d'octets. Chaque valeur doit être comprise entre 0..255", //633 kifglobalthread.cxx 3579
"teste(i): teste si i est une position valide dans la table", //634 kifglobalthread.cxx 3580
"dépile(nombre n|chaine c): retire le nième élément ou la chaine c.", //635 kifglobalthread.cxx 3581
"dépile(): retire le dernier élément de la table.", //636 kifglobalthread.cxx 3582
"dernier(): renvoie le dernier élément de la table", //637 kifglobalthread.cxx 3583
"inverse(): inverse l'ordre des éléments dans la table", //638 kifglobalthread.cxx 3584
"joins(chaine sep): concatène chaque élément dans la table en une chaine ou chaque élément est séparé des autres par sep", //639 kifglobalthread.cxx 3585
"réserve(nombre nb): Réserve nb éléments dans une table", //640 kifglobalthread.cxx 3586
"insère(i,x): insère l'élément x à la position i", //641 kifglobalthread.cxx 3587
"somme(): Somme chaque élément", //642 kifglobalthread.cxx 3588
"produit(): Multiplie chaque élément avec les autres", //643 kifglobalthread.cxx 3589
"prédicat(): Crée un nouveau prédicat sous la forme [nom,param1,param2,...]", //644 kifglobalthread.cxx 3590
"raz(): nettoie la table", //645 kifglobalthread.cxx 3591
"min(): renvoie la valeur minimale dans un conteneur", //646 kifglobalthread.cxx 3592
"max(): renvoie la valeur maximale dans un conteneur", //647 kifglobalthread.cxx 3593
"trie(compare): Trie le contenu du  table selon une fonction de comparaison, laquelle est optionnelle", //648 kifglobalthread.cxx 3594
"mélange(): mélange les valeurs", //649 kifglobalthread.cxx 3595
"permute(): permute les valeurs dans un vecteur. Renvoie faux quand toutes les permutations ont été faites.", //650 kifglobalthread.cxx 3596
"unique(): retire les doublons", //651 kifglobalthread.cxx 3597
"trienombre(bool ordre): trie le contenu de la table dont chaque élément est considéré comme un nombre.", //652 kifglobalthread.cxx 3598
"triedécimal(bool ordre): trie le contenu de la table dont chaque élément est considéré comme un décimal.", //653 kifglobalthread.cxx 3599
"trielong(bool ordre): trie le contenu de la table dont chaque élément est considéré comme un long.", //654 kifglobalthread.cxx 3600
"triechaine(bool ordre): trie le contenu de la table dont chaque élément est considéré comme une chaine.", //655 kifglobalthread.cxx 3601
"applique(a,b,c): applique sur chaque élément une fonction, ou applique toutes les fonctions dans le conteneur", //656 kifglobalthread.cxx 3602
"attentes(t1,t2,...,tn,p1,p2,..pn): Lance un ensemble de tâches en attente de déclenchement, avec tableau comme premier argument de chaque appel. p1,p2,...pn sont des paramètres optionels passés en arguments lors de l'appel de chaque tâche.", //657 kifglobalthread.cxx 3603
"ajouteattentes(t1,t2,...,tn): Ajoute de nouvelles tâches à la liste des tâches du tableau.", //658 kifglobalthread.cxx 3604
"déclenche(bool boucle): Déclenche les tâches, si boucle est vraie, alors réactive les tâches.", //659 kifglobalthread.cxx 3605
"empileentête(a): ajoute au début de la liste", //660 kifglobalthread.cxx 3607
"teste(i): teste si i est une position valide dans la liste", //661 kifglobalthread.cxx 3610
"empileenqueue(a): ajoute en queue de la liste", //662 kifglobalthread.cxx 3615
"dépileentête(): retire l'élément en tête de la liste et le renvoie.", //663 kifglobalthread.cxx 3616
"dépileenqueue(): retire le dernier élément de la liste et le renvoie.", //664 kifglobalthread.cxx 3621
"dernier(): renvoie le dernier élément de la liste", //665 kifglobalthread.cxx 3622
"premier(): renvoie le premier élément de la liste.", //666 kifglobalthread.cxx 3623
"inverse(): inverse l'ordre des éléments dans la liste", //667 kifglobalthread.cxx 3624
"joins(chaine sep): concatène chaque élément dans la liste en une chaine ou chaque élément est séparé des autres par sep", //668 kifglobalthread.cxx 3625
"applique(a,b,c): applique sur chaque élément une fonction, ou applique toutes les fonctions dans le conteneur.", //669 kifglobalthread.cxx 3629
"raz(): nettoie la liste", //670 kifglobalthread.cxx 3630
"inverse(): Inverse les clefs et les valeurs dans un dictionnaire", //671 kifglobalthread.cxx 3632
"joins(chaine clefsep,chaine sep): concatène chaque élément du dictionnaire en une chaine ou chaque clef est ajoutée avec clefsep et chaque élément est séparé des autres par sep", //672 kifglobalthread.cxx 3634
"prédicat(): Crée un nouveau prédicat sous la forme: {'nom':nom,0:param1,1:param2,...}", //673 kifglobalthread.cxx 3638
"teste(i): teste si i est une clef valide dans le dictionnaire", //674 kifglobalthread.cxx 3639
"dépile(chaine clef): retire l'élément correspondant à clef", //675 kifglobalthread.cxx 3641
"clefs(): renvoies les clefs du dictionnaire comme table", //676 kifglobalthread.cxx 3642
"valeurs(): renvoie les valeurs du dictionnaire comme une table", //677 kifglobalthread.cxx 3643
"items(): renvoie une table de clef:valeur", //678 kifglobalthread.cxx 3644
"raz(): nettoie le dictionnaire", //679 kifglobalthread.cxx 3647
"évaluation(): évalue l'occupation mémoire d'un dicopremier", //680 kifglobalthread.cxx 3648
"amorce(tablenombres tn): spécifie les valeurs de hachage", //681 kifglobalthread.cxx 3649
"arbre(valeur): crée un noeud d'arbre, avec valeur comme valeur. Les valeurs peuvent avoir n'importe quel type", //682 kifglobalthread.cxx 3651
"frère()|frère(arbre a): renvoie le noeud frère, ou l'ajoute comme noeud frère", //683 kifglobalthread.cxx 3653
"précédent()|précédent(arbre a): renvoie le noeud précédent, ou l'ajoute comme noeud précédent", //684 kifglobalthread.cxx 3654
"fils()|fils(arbre a): renvoie le noeud fils, ou l'ajoute comme noeud fils", //685 kifglobalthread.cxx 3655
"père()|père(arbre a): renvoie le noeud père, ou compare a au noeud père", //686 kifglobalthread.cxx 3656
"derniernoeud()|derniernoeud(arbre a): renvoie le dernier noeud, ou compare a au dernier noeud", //687 kifglobalthread.cxx 3657
"isole(): Extrait le noeud courant de son arbre", //688 kifglobalthread.cxx 3658
"élague(): Détruit le sous-arbre de l'arbre global", //689 kifglobalthread.cxx 3659
"profondeur(): Renvoie la profondeur du noeud dans le arbre", //690 kifglobalthread.cxx 3660
"valeur(): renvoie la valeur de l'élément courant", //691 kifglobalthread.cxx 3661
"clef(): renvoie la clef de l'élément courant", //692 kifglobalthread.cxx 3662
"valeurtype(): renvoie le type de la valeur de l'élément courant", //693 kifglobalthread.cxx 3663
"valeurestun(chaine type): teste le type de l'élément courant", //694 kifglobalthread.cxx 3664
"commence(): initialise l'itérateur avec le début de la collection", //695 kifglobalthread.cxx 3665
"suivant(): élément suivant dans la collection", //696 kifglobalthread.cxx 3666
"metvaleur(value): Change la valeur courante pointée par l'itérateur.", //697 kifglobalthread.cxx 3667
"fin(): renvoie vrai quand la fin de la collection est atteinte", //698 kifglobalthread.cxx 3668
"nfin(): renvoie vrai tant que la fin de la collection n'a pas été atteinte (~fin())", //699 kifglobalthread.cxx 3669
"applique(a,b,c): applique une fonction à tous les éléments du conteneur ou toutes les fonctions dans le conteneur", //700 kifglobalthread.cxx 3670
"metdate(année,mois,jour,heure,min,sec): initialise une variable de temps", //701 kifglobalthread.cxx 3671
"date(): renvoie la date comme chaine", //702 kifglobalthread.cxx 3672
"année(): renvoie l'année comme un nombre", //703 kifglobalthread.cxx 3673
"mois(): renvoie le mois comme un nombre", //704 kifglobalthread.cxx 3674
"jour(): renvoie le jour comme un nombre", //705 kifglobalthread.cxx 3675
"jourannée(): Renvoie le jour de l'année comme nombre entre 0-365", //706 kifglobalthread.cxx 3676
"joursemaine(): Renvoie le jour de la semaine comme nombre entre 0-6. 0 est dimanche.", //707 kifglobalthread.cxx 3677
"heure(): renvoie l'heure comme un nombre", //708 kifglobalthread.cxx 3678
"minute(): renvoie les minutes comme un nombre", //709 kifglobalthread.cxx 3679
"seconde(): renvoie les secondes comme un nombre", //710 kifglobalthread.cxx 3680
"raz(): réinitialise une variable de temps", //711 kifglobalthread.cxx 3681
"ouvreenajout(chaine fichiername): ouvre un fichier en ajout", //712 kifglobalthread.cxx 3682
"ouvreenécriture(chaine fichiername): ouvre un fichier en écriture", //713 kifglobalthread.cxx 3683
"ferme(): ferme un fichier", //714 kifglobalthread.cxx 3684
"cherche(chaine c,bool sanscasse): Cherche une chaine dans un fichier et renvoie toutes ses positions.", //715 kifglobalthread.cxx 3685
"ouvreenlecture(chaine fichiername): ouvre un fichier en lecture", //716 kifglobalthread.cxx 3686
"metsignature(bool s): Initialise le mode signature", //717 kifglobalthread.cxx 3687
"signature(): Renvoie le mode signature", //718 kifglobalthread.cxx 3688
"écrit(chaine s1,chaine s2,): écrit les chaines dans le fichier", //719 kifglobalthread.cxx 3689
"écritln(chaine c1,chaine...): écrit des chaines dans un fichier, terminés par un retour chariot", //720 kifglobalthread.cxx 3690
"écrit(nombre s1,nombre s2,): écrit les codes de caractères dans le fichier", //721 kifglobalthread.cxx 3691
"saisie(): lecture d'un caractère depuis le fichier", //722 kifglobalthread.cxx 3692
"remet(): Retourne le caractère dans le flux de lecture", //723 kifglobalthread.cxx 3693
"remet(nb): renvoie nb caractères dans le flux", //724 kifglobalthread.cxx 3694
"lit(): lit une ligne depuis un fichier", //725 kifglobalthread.cxx 3695
"litln(): lit une ligne depuis un fichier", //726 kifglobalthread.cxx 3696
"positionne(nombre p): positionne le curseur du fichier à p", //727 kifglobalthread.cxx 3697
"dit(): renvoie la position du  curseur courant dans le fichier", //728 kifglobalthread.cxx 3698
"findefichier(): renvoie vrai quand la fin de fichier est atteinte", //729 kifglobalthread.cxx 3699
"charge(tablechaines r): Charge une grammaire à partir d'une table de chaines. Chaque élément doit être une règle.", //730 kifglobalthread.cxx 3701
"applique(chaine x): Applique une grammaire à une chaine.", //731 kifglobalthread.cxx 3702
"charge(chaine chemin,chaine cartraits): Charge un fichier de mots. Chaque ligne doit contenir un seul mot. cartraits définit le caractère qui débute une structure de traits. Il est optionnel.", //732 kifglobalthread.cxx 3704
"chargecompact(chaine chemin): Charge un fichier en format compact", //733 kifglobalthread.cxx 3705
"chargelemmes(chaine chemin): Charge un fichier contenant chacun sur une ligne à la suite la forme de surface et son lemme+traits", //734 kifglobalthread.cxx 3706
"modèle(fonction): Définis la fonction modèle utilisée pendant le traitement", //735 kifglobalthread.cxx 3707
"recherche(mot): parcours l'automate et renvoie les lectures les plus probables selon la fonction modèle", //736 kifglobalthread.cxx 3708
"détecteétatfinal(bool): Détecte les états finaux lors d'un 'recherche'", //737 kifglobalthread.cxx 3709
"initialisetransformations(dictionnaire m): Initialise le dictionnaire des poids à utiliser lors d'un distanceédition dans l'automate", //738 kifglobalthread.cxx 3710
"enregistre(chaine chemin): Enregistre le fichier dans un format compact", //739 kifglobalthread.cxx 3711
"taille(): Renvoie le nombre de mots enregistrés dans l'automate", //740 kifglobalthread.cxx 3712
"dump(chaine chemin): Enregistre le contenu de l'automate dans un fichier", //741 kifglobalthread.cxx 3713
"définiscodetrait(chaine caractère): Utilise ce caractère pour détecter le début des traits", //742 kifglobalthread.cxx 3714
"distanceédition(chaine mot,int seuil,int actions): Trouve les mots dont la distance d'édition est inférieur au seuil. Les actions sont: 1: action sur le premier caractère. 2: changement caractère. 4: destruction caractère. 8: insertion caractère.", //743 kifglobalthread.cxx 3715
"raz(): vide l'automate", //744 kifglobalthread.cxx 3716
"ajoute(chaine mot): ajoute un mot dans l'automate. ajoute(chaine mode,chaine lemme): ajoute un mot et son lemme dans l'automate", //745 kifglobalthread.cxx 3717
"charge(chaine kifpathname): charge un programme KiF", //746 kifglobalthread.cxx 3719
"compile(chaine kifpathname): Compile un programme KiF sous la forme d'une chaine. Renvoie un identifiant sur la première instruction à exécuter", //747 kifglobalthread.cxx 3720
"exécute(nombre i): Exécute un programme depuis l'instruction i (renvoyé par compile)", //748 kifglobalthread.cxx 3721
"kvariables(): Renvoie les variables actives sous la forme d'un dictionnaire", //749 kifglobalthread.cxx 3722
"exécutetâche(nombre i): Exécute en parallèle un programme compilé depuis l'identifiant renvoyé", //750 kifglobalthread.cxx 3723
"finexécution(): Renvoie vrai si le programme s'est entièrement exécuté", //751 kifglobalthread.cxx 3724
"fonctiondebug(fonction,objet): Initialise la fonction de debuggage qui sera appelée pendant l'exécution", //752 kifglobalthread.cxx 3725
"fermedebug(): Fin du mode debug commencé avec fonctiondebug", //753 kifglobalthread.cxx 3726
"ouvre(): ouvre une session KiF", //754 kifglobalthread.cxx 3727
"nettoie(): ferme une session KiF", //755 kifglobalthread.cxx 3728
"nom(): Renvoie le chemin du fichier KiF", //756 kifglobalthread.cxx 3729
"exportées(): Renvoie la liste des méthodes exportées", //757 kifglobalthread.cxx 3730
"_chargeur: Une variable de type 'kif' qui pointe vers le programme le plus englobant.", //758 kifglobalthread.cxx 3731
""};


Exported const char* KIFMAPS[]={
"KIF(881): Erreur, mauvais arguments pour cette fonction", //0 kifmaps.cxx 100
"KIF(689): Structure circulaire", //1 kifmaps.cxx 1164
""};


Exported const char* KIFPMAPS[]={
"KIF(689): Structure circulaire", //0 kifpmaps.cxx 762
"KIF(881): Erreur, mauvais arguments pour cette fonction", //1 kifpmaps.cxx 10099
""};


Exported const char* KIFTREEMAPS[]={
"KIF(689): Structure circulaire", //0 treemaps.cxx 760
"KIF(881): Erreur, mauvais arguments pour cette fonction", //1 treemaps.cxx 10035
""};


Exported const char* KIFAUTOMATON[]={
"KIF(637): ne peut créer le fichier", //0 kifautomaton.cxx 208
"KIF(645): Le dictionnaire ne doit comprendre que des sous-dictionnaires", //1 kifautomaton.cxx 229
"KIF(647): Ne peut pas traiter cette structure", //2 kifautomaton.cxx 256
"l", //3 kifautomaton.cxx 269
"KIF(620): Ne peut ouvrir le fichier", //4 kifautomaton.cxx 271
"KIF(622): Le caractère introduisant les traits doit être un caractère unique ou la chaine vide", //5 kifautomaton.cxx 277
"KIF(625): Surface+lemme doivent par paire, chacun sur une ligne à la suite", //6 kifautomaton.cxx 315
"KIF(626): Valeur manquante dans 'CHARGELEMMES'", //7 kifautomaton.cxx 328
"KIF(623): Le caractère introduisant les traits doit être un caractère unique", //8 kifautomaton.cxx 342
"KIF(624): Videz l'automate d'abord", //9 kifautomaton.cxx 346
"KIF(634): Fonction manquante, ne peux pas rechercher", //10 kifautomaton.cxx 828
"KIF(621): La valeur attendue est soit un dicod soit une table de tableaux", //11 kifautomaton.cxx 979
"KIF(635): Nom de fichier manquant", //12 kifautomaton.cxx 1377
"KIF(636): Ne peut enregistrer en format, nombre de symbole trop grand", //13 kifautomaton.cxx 1387
"alphabet", //14 kifautomaton.cxx 1390
"arcs", //15 kifautomaton.cxx 1398
"KIF(637): Ne peut charger un fichier compact", //16 kifautomaton.cxx 1553
"KIF(411): Erreur après:<", //17 kifautomaton.cxx 1776
"GRM(002): Mauvaise négation ou chaine vide.", //18 kifautomaton.cxx 1816
"GRM(001): Tête de règle inconnue:", //19 kifautomaton.cxx 1851
"", //20 kifautomaton.cxx 3320 Leave blank
""};


Exported const char* KIFCONTAINER[]={
"KIF(689): Structure circulaire", //0 kifcontainer.cxx 4089
"KIF(877): Racine manquante dans le conteneur", //1 kifcontainer.cxx 7288
"KIF(415): Un tuple ne peut pas être modifié", //2 kifcontainer.cxx 8128
"KIF(416): On ne peut pas créer un tuple vide", //3 kifcontainer.cxx 8135
"KIF(417): On attend un conteneur", //4 kifcontainer.cxx 8142
"ajouteattentes", //5 kifcontainer.cxx 8787
"retaille", //6 kifcontainer.cxx 8787
"compte", //7 kifcontainer.cxx 8787
""};


Exported const char* KIFPREDICATE[]={
"PRE(010): Une fonction est attendue", //0 kifpredicate.cxx 112
"PRE(009): Vous ne pouvez instancier un prédicat de cette façon", //1 kifpredicate.cxx 765
"PRE(001): Instanciation de ce prédicat avec cette valeur impossible", //2 kifpredicate.cxx 771
"pondère", //3 kifpredicate.cxx 861
"assertea", //4 kifpredicate.cxx 864
"assertez", //5 kifpredicate.cxx 867
"assertebd", //6 kifpredicate.cxx 870
"rétracte", //7 kifpredicate.cxx 873
"rétractebd", //8 kifpredicate.cxx 876
"nom", //9 kifpredicate.cxx 1086
"PRE(003): Nom de prédicat inconnu", //10 kifpredicate.cxx 1241
"PRE(004): Index de paramètre inconnu dans ce dictionnaire", //11 kifpredicate.cxx 1266
"PRE(005): On ne peut construire un prédicat sur la base de cet objet", //12 kifpredicate.cxx 1275
"unifiable", //13 kifpredicate.cxx 2123
"succès:", //14 kifpredicate.cxx 2339
"--> Échec", //15 kifpredicate.cxx 2559
"PRE(006): On attend un objet de type sqlite", //16 kifpredicate.cxx 3446
""};


Exported const char* KIFRAWSTRINGS[]={
"Cet état d'attente n'existe pas: '", //0 kifraw.cxx 279
"curseur", //1 kifraw.cxx 569
"curseurmax", //2 kifraw.cxx 570
"ramassemiette", //3 kifraw.cxx 571
"vide", //4 kifraw.cxx 572
"vont_être_nettoyées", //5 kifraw.cxx 573
"en_suspend", //6 kifraw.cxx 574
"curseurchaines", //7 kifraw.cxx 575
"curseurnombres", //8 kifraw.cxx 576
"curseurdécimaux", //9 kifraw.cxx 577
"ramassechaines", //10 kifraw.cxx 578
"ramassenombres", //11 kifraw.cxx 579
"ramassedécimaux", //12 kifraw.cxx 580
"XTR(011): paramètres manquants", //13 kifraw.cxx 956
"(ligne:%d)", //14 kifraw.cxx 1152
"KIF(831): Mauvais caractère d'ouverture ou de fermeture d'expression pour LISP", //15 kifraw.cxx 1177
"KIF(665): On attend un conteneur dans SSTOKENIZE", //16 kifraw.cxx 1709
"KIF(550): La liste des tâches a déjà été implantée", //17 kifraw.cxx 2234
"KIF(551): On attend des tâches comme argument.", //18 kifraw.cxx 2250
"KIF(552): Cette tâche a déjà été insérée dans la liste des tâches", //19 kifraw.cxx 2263
"KIF(553): Veuillez utiliser 'attentes' pour implanter une liste des tâches", //20 kifraw.cxx 2292
"KIF(654): Le second parmètre doit être une table dans 'amorce'", //21 kifraw.cxx 2341
"KIF(510): base trop grande, max est: %d", //22 kifraw.cxx 3190
"KIF(511): Impossible d'analyser cette chaine dans cette base.", //23 kifraw.cxx 3201
"KIF(801): valeur infinie", //24 kifraw.cxx 3239
"KIF(802): Ce n'est pas un nombre", //25 kifraw.cxx 3242
"KIF(877): Variable attendue", //26 kifraw.cxx 3813
"MATH(100): Erreur mauvaise fraction: dénominateur est 0", //27 kifraw.cxx 3978
"BITS(105): Mauvais type de paramètre dans partie", //28 kifraw.cxx 4059
"KIF(543): Sortie standard hors de portée", //29 kifraw.cxx 4096
"KIF(544): Sortie erreur hors de portée", //30 kifraw.cxx 4113
"KIF(124): Format incorrect", //31 kifraw.cxx 5794
"KIF(635): Cette fonction doit être appelée depuis un automate", //32 kifraw.cxx 6174
"XTR(010): On attend une table avec un nombre pair d'éléments.", //33 kifraw.cxx 6400
"KIF(832): Type argument inutilisable, fonction 'ajout' pour le type 'automate'.", //34 kifraw.cxx 6457
"KIF(644): La structure doit être composée d'un dictionnaire de dictionnaire", //35 kifraw.cxx 6491
"XTR(001): Nous attendons un vecteur comme deuxième argument.", //36 kifraw.cxx 6738
"XTR(002): Mauvaise expression régulière", //37 kifraw.cxx 6754
""};


Exported const char* KIFLIBSTRINGS[]={
"KIF(0): Mauvais nombre de paramètres pour ECLATE", //0 kiflibs.cxx 112
"KIF(1): Mauvais troisième paramètre pour ECLATE", //1 kiflibs.cxx 113
"KIF(2): Mauvais nombre de paramètres pour LISTE", //2 kiflibs.cxx 114
"KIF(3): Mauvais nombre de paramètres pour UTF8", //3 kiflibs.cxx 115
"KIF(4): Mauvais nombre de paramètres pour LATIN", //4 kiflibs.cxx 116
"KIF(5): Mauvais nombre de paramètres pour GAUCHE", //5 kiflibs.cxx 117
"KIF(6): Mauvais nombre de paramètres pour DROIT", //6 kiflibs.cxx 118
"KIF(7): Mauvais nombre de paramètres pour MILIEU", //7 kiflibs.cxx 119
"KIF(8): Mauvais nombre de paramètres pour ENMINUSCULE", //8 kiflibs.cxx 120
"KIF(9): Mauvais nombre de paramètres pour ENMAJUSCULE", //9 kiflibs.cxx 121
"KIF(10): Mauvais nombre de paramètres pour MINUSCULE", //10 kiflibs.cxx 122
"KIF(11): Mauvais nombre de paramètres pour MAJUSCULE", //11 kiflibs.cxx 123
"KIF(12): Mauvais nombre de paramètres pour ROGNE", //12 kiflibs.cxx 124
"KIF(13): Mauvais nombre de paramètres pour METDATE", //13 kiflibs.cxx 125
"KIF(14): Mauvais paramètre de temps pour METDATE", //14 kiflibs.cxx 126
"KIF(15): Mauvais nombre de paramètres pour DATE", //15 kiflibs.cxx 127
"KIF(16): Mauvais paramètre de temps pour DATE", //16 kiflibs.cxx 128
"KIF(17): Mauvais nombre de paramètres pour ANNEE", //17 kiflibs.cxx 129
"KIF(18): Mauvais paramètre de temps pour ANNEE", //18 kiflibs.cxx 130
"KIF(19): Mauvais nombre de paramètres pour JOUR", //19 kiflibs.cxx 131
"KIF(20): Mauvais paramètre de temps pour JOUR", //20 kiflibs.cxx 132
"KIF(21): Mauvais nombre de paramètres pour HEURE", //21 kiflibs.cxx 133
"KIF(22): Mauvais paramètre de temps pour HEURE", //22 kiflibs.cxx 134
"KIF(23): Mauvais nombre de paramètres pour MOIS", //23 kiflibs.cxx 135
"KIF(24): Mauvais paramètre de temps pour MOIS", //24 kiflibs.cxx 136
"KIF(25): Paramètre manquant pour LEVE", //25 kiflibs.cxx 137
"KIF(26): Mauvais nombre de paramètres pour ECRIT", //26 kiflibs.cxx 138
"KIF(27): Le premier paramètre dans ECRIT devrait être un fichier", //27 kiflibs.cxx 139
"KIF(28): Le premier paramètre dans LIT devrait être un fichier", //28 kiflibs.cxx 140
"KIF(29): Le premier paramètre dans LITLIGNE devrait être un fichier", //29 kiflibs.cxx 141
"KIF(30): Le premier paramètre dans FERME devrait être un fichier", //30 kiflibs.cxx 142
"KIF(31): Le premier paramètre dans OUVREENLECTURE devrait être un fichier", //31 kiflibs.cxx 143
"KIF(32): Erreur pendant l'ouverture du fichier en lecture:", //32 kiflibs.cxx 144
"KIF(33): Le premier paramètre dans FINFICHIER devrait être un fichier", //33 kiflibs.cxx 145
"KIF(34): Le premier paramètre dans OUVREENECRITURE devrait être un fichier", //34 kiflibs.cxx 146
"KIF(35): Erreur pendant l'ouverture du fichier en écriture:", //35 kiflibs.cxx 147
"KIF(36): Mauvais nombre de paramètres pour INSERE", //36 kiflibs.cxx 148
"KIF(37): Mauvais appel de INSERE", //37 kiflibs.cxx 149
"KIF(38): Fonction inconnue:", //38 kiflibs.cxx 150
"KIF(39): Mauvais appel de EMPILE", //39 kiflibs.cxx 151
"KIF(40): Mauvais nombre de paramètres pour dépile", //40 kiflibs.cxx 152
"KIF(41): Mauvais appel de dépile", //41 kiflibs.cxx 153
"KIF(42): Mauvais nombre de paramètres pour DERNIER", //42 kiflibs.cxx 154
"KIF(43): Noeud inconnu pour DERNIER", //43 kiflibs.cxx 155
"", //44 kiflibs.cxx 156 Leave blank
"KIF(45): Mauvais appel de DERNIER", //45 kiflibs.cxx 157
"KIF(46): Mauvais nombre de paramètres pour TRI", //46 kiflibs.cxx 158
"KIF(47): Mauvais appel de TRI", //47 kiflibs.cxx 159
"KIF(48): Paramètre manquants pour APPLIQUE", //48 kiflibs.cxx 160
"KIF(49): Mauvais appel de APPLIQUE", //49 kiflibs.cxx 161
"", //50 kiflibs.cxx 162 Leave blank
"", //51 kiflibs.cxx 163 Leave blank
"", //52 kiflibs.cxx 164 Leave blank
"", //53 kiflibs.cxx 165 Leave blank
"", //54 kiflibs.cxx 166 Leave blank
"", //55 kiflibs.cxx 167 Leave blank
"", //56 kiflibs.cxx 168 Leave blank
"", //57 kiflibs.cxx 169 Leave blank
"", //58 kiflibs.cxx 170 Leave blank
"", //59 kiflibs.cxx 171 Leave blank
"", //60 kiflibs.cxx 172 Leave blank
"", //61 kiflibs.cxx 173 Leave blank
"", //62 kiflibs.cxx 174 Leave blank
"", //63 kiflibs.cxx 175 Leave blank
"", //64 kiflibs.cxx 176 Leave blank
"", //65 kiflibs.cxx 177 Leave blank
"", //66 kiflibs.cxx 178 Leave blank
"", //67 kiflibs.cxx 179 Leave blank
"", //68 kiflibs.cxx 180 Leave blank
"", //69 kiflibs.cxx 181 Leave blank
"", //70 kiflibs.cxx 182 Leave blank
"", //71 kiflibs.cxx 183 Leave blank
"", //72 kiflibs.cxx 184 Leave blank
"", //73 kiflibs.cxx 185 Leave blank
"", //74 kiflibs.cxx 186 Leave blank
"", //75 kiflibs.cxx 187 Leave blank
"", //76 kiflibs.cxx 188 Leave blank
"", //77 kiflibs.cxx 189 Leave blank
"", //78 kiflibs.cxx 190 Leave blank
"", //79 kiflibs.cxx 191 Leave blank
"KIF(80): Mauvais nombre de paramètres pour paramètres", //80 kiflibs.cxx 192
"KIF(81): Deuxieme paramètre devrait être un TABLEAU", //81 kiflibs.cxx 193
"", //82 kiflibs.cxx 194 Leave blank
"KIF(83): Pas de paramètre pour cette variable", //83 kiflibs.cxx 195
"KIF(84): Mauvais nombre de paramètres pour RETIREPREMIER", //84 kiflibs.cxx 196
"KIF(85): Pas de REMOVEFIRST pour cette variable", //85 kiflibs.cxx 197
"KIF(86): Mauvais nombre de paramètres pour INSTANCES", //86 kiflibs.cxx 198
"KIF(87): Pas de INSTANCES pour cette variable", //87 kiflibs.cxx 199
"KIF(88): Mauvais nombre de paramètres pour MET", //88 kiflibs.cxx 200
"KIF(89): Mauvais nombre de paramètres pour REGEX", //89 kiflibs.cxx 201
"KIF(90): Classe manquante pour le MET de Weka", //90 kiflibs.cxx 202
"KIF(91): Attribut manquant pour MET de Weka", //91 kiflibs.cxx 203
"KIF(92): Pas de MET pour cette variable", //92 kiflibs.cxx 204
"KIF(93): Mauvais nombre de paramètres pour RECUPERE", //93 kiflibs.cxx 205
"KIF(94): Classe manquante pour RECUPERE de Weka", //94 kiflibs.cxx 206
"KIF(95): Attribut manquant pour RECUPERE de Weka", //95 kiflibs.cxx 207
"", //96 kiflibs.cxx 208 Leave blank
"", //97 kiflibs.cxx 209 Leave blank
"", //98 kiflibs.cxx 210 Leave blank
"", //99 kiflibs.cxx 211 Leave blank
"", //100 kiflibs.cxx 212 Leave blank
"", //101 kiflibs.cxx 213 Leave blank
"", //102 kiflibs.cxx 214 Leave blank
"", //103 kiflibs.cxx 215 Leave blank
"", //104 kiflibs.cxx 216 Leave blank
"", //105 kiflibs.cxx 217 Leave blank
"", //106 kiflibs.cxx 218 Leave blank
"", //107 kiflibs.cxx 219 Leave blank
"", //108 kiflibs.cxx 220 Leave blank
"", //109 kiflibs.cxx 221 Leave blank
"", //110 kiflibs.cxx 222 Leave blank
"", //111 kiflibs.cxx 223 Leave blank
"", //112 kiflibs.cxx 224 Leave blank
"", //113 kiflibs.cxx 225 Leave blank
"", //114 kiflibs.cxx 226 Leave blank
"", //115 kiflibs.cxx 227 Leave blank
"", //116 kiflibs.cxx 228 Leave blank
"", //117 kiflibs.cxx 229 Leave blank
"", //118 kiflibs.cxx 230 Leave blank
"", //119 kiflibs.cxx 231 Leave blank
"", //120 kiflibs.cxx 232 Leave blank
"", //121 kiflibs.cxx 233 Leave blank
"", //122 kiflibs.cxx 234 Leave blank
"", //123 kiflibs.cxx 235 Leave blank
"", //124 kiflibs.cxx 236 Leave blank
"", //125 kiflibs.cxx 237 Leave blank
"", //126 kiflibs.cxx 238 Leave blank
"", //127 kiflibs.cxx 239 Leave blank
"", //128 kiflibs.cxx 240 Leave blank
"", //129 kiflibs.cxx 241 Leave blank
"", //130 kiflibs.cxx 242 Leave blank
"KIF(131): Paramètre inconnu pour CHARGE", //131 kiflibs.cxx 243
"KIF(132): COMMENCE a échoué pour ITERATEUR", //132 kiflibs.cxx 244
"KIF(133): FIN a échoué pour ITERATEUR", //133 kiflibs.cxx 245
"KIF(134): NFIN a échoué pour ITERATEUR", //134 kiflibs.cxx 246
"KIF(135): Pas de CLEF pour cet ITERATEUR", //135 kiflibs.cxx 247
"KIF(136): Pas de VALEUR pour cet ITERATEUR", //136 kiflibs.cxx 248
"KIF(137): Pas de TYPEVALEUR pour cet ITERATEUR", //137 kiflibs.cxx 249
"", //138 kiflibs.cxx 250 Leave blank
"", //139 kiflibs.cxx 251 Leave blank
"", //140 kiflibs.cxx 252 Leave blank
"", //141 kiflibs.cxx 253 Leave blank
"", //142 kiflibs.cxx 254 Leave blank
"", //143 kiflibs.cxx 255 Leave blank
"", //144 kiflibs.cxx 256 Leave blank
"", //145 kiflibs.cxx 257 Leave blank
"", //146 kiflibs.cxx 258 Leave blank
"", //147 kiflibs.cxx 259 Leave blank
"", //148 kiflibs.cxx 260 Leave blank
"", //149 kiflibs.cxx 261 Leave blank
"", //150 kiflibs.cxx 262 Leave blank
"", //151 kiflibs.cxx 263 Leave blank
"", //152 kiflibs.cxx 264 Leave blank
"", //153 kiflibs.cxx 265 Leave blank
"KIF(154): mauvais type pour ECRIT", //154 kiflibs.cxx 266
"KIF(155): Mauvaise affectation d'une classe", //155 kiflibs.cxx 267
"KIF(156): Mauvaise affectation d'un APPEL", //156 kiflibs.cxx 268
"", //157 kiflibs.cxx 269 Leave blank
"KIF(158): Veuillez vérifier le nombre de paramètres pour:", //158 kiflibs.cxx 270
"KIF(159): Veuillez vérifier le paramètre pour:", //159 kiflibs.cxx 271
"KIF(160): Mauvaise initialisation d'une classe", //160 kiflibs.cxx 272
"KIF(161): Mauvais nombre de paramètres pour POSITIONNER", //161 kiflibs.cxx 273
"KIF(162): Le premier paramètre devrait être un fichier pour POSITIONNER", //162 kiflibs.cxx 274
"KIF(163): Mauvais nombre de paramètres pour DIT", //163 kiflibs.cxx 275
"KIF(164): Le premier paramètre devrait être un fichier pour DIT", //164 kiflibs.cxx 276
"KIF(165): Mauvais nombre de paramètres pour JOINT", //165 kiflibs.cxx 277
"KIF(166): Mauvais paramètre pour JOINT", //166 kiflibs.cxx 278
"KIF(167): Mauvais nombre de paramètres pour COMPILE", //167 kiflibs.cxx 279
"KIF(168): Mauvais paramètre pour COMPILE", //168 kiflibs.cxx 280
"KIF(169): Mauvais nombre de paramètres pour SOMME", //169 kiflibs.cxx 281
"KIF(170): Mauvais paramètre pour SOMME", //170 kiflibs.cxx 282
"KIF(171): Mauvais nombre de paramètres pour PRODUIT", //171 kiflibs.cxx 283
"KIF(172): Mauvais paramètre pour PRODUIT", //172 kiflibs.cxx 284
"KIF(173): Mauvais nombre de paramètres pour REMET", //173 kiflibs.cxx 285
"KIF(174): Pas de REMET pour cette variable", //174 kiflibs.cxx 286
"KIF(175): Mauvais nombre de paramètres pour INVERSE", //175 kiflibs.cxx 287
"KIF(176): Mauvais nombre de paramètres pour EXTRAIT", //176 kiflibs.cxx 288
"KIF(177): Mauvais paramètre pour EXTRAIT", //177 kiflibs.cxx 289
"KIF(178): Fonction inconnue:", //178 kiflibs.cxx 290
"", //179 kiflibs.cxx 291 Leave blank
"", //180 kiflibs.cxx 292 Leave blank
"", //181 kiflibs.cxx 293 Leave blank
"", //182 kiflibs.cxx 294 Leave blank
"KIF(183): Instance inconnue:", //183 kiflibs.cxx 295
"KIF(184): Erreur de chargement:", //184 kiflibs.cxx 296
"KIF(185): Paramètre manquant pour HACHAGE", //185 kiflibs.cxx 297
"KIF(186): Mauvais nombre de paramètres pour VALEURS", //186 kiflibs.cxx 298
"KIF(187): Mauvais type de paramètre pour VALEURS", //187 kiflibs.cxx 299
"KIF(188): Mauvais nombre de paramètre ou mauvais type pour ECLATERGX", //188 kiflibs.cxx 300
"KIF(189): Le chargement de la bibliotheque a echoué:", //189 kiflibs.cxx 301
"KIF(190): Point d'entrée manquant dans KifInitialisationModule dans:", //190 kiflibs.cxx 302
"KIF(191): Mauvais nombre de paramètres pour INFOSFICHIER", //191 kiflibs.cxx 303
"KIF(192): Mauvais nombre de paramètres pour CLEFS", //192 kiflibs.cxx 304
"KIF(193): Mauvais type de paramètre pour CLEFS", //193 kiflibs.cxx 305
"KIF(194): Mauvais nombre de paramètres pour CHERCHE", //194 kiflibs.cxx 306
"KIF(195): Mauvais nombre de paramètres pour COMP", //195 kiflibs.cxx 307
"KIF(196): Mauvais type de paramètres dans:", //196 kiflibs.cxx 308
"KIF(197): Mauvais nombre de paramètres pour TUE", //197 kiflibs.cxx 309
"KIF(198): Mauvais nombre de paramètres pour ENVOIE", //198 kiflibs.cxx 310
"KIF(199): Mauvais nombre de paramètres pour ATTENDS", //199 kiflibs.cxx 311
"KIF(200): Mauvais nombre de paramètres pour ENATTENTE", //200 kiflibs.cxx 312
"KIF(201): Mauvais paramètre: élément arbre attendu", //201 kiflibs.cxx 313
"KIF(202): Paramètre ARBRE manquant", //202 kiflibs.cxx 314
"KIF(203): Mauvais nombre de paramètres", //203 kiflibs.cxx 315
"KIF(204): Erreur: le paramètre ARBRE est un  parent de la cible ARBRE", //204 kiflibs.cxx 316
"KIF(205): Vous ne pouvez pas bouger un noeud vers lui-même", //205 kiflibs.cxx 317
"KIF(206): On ne peut pas initialiser une table avec cette valeur", //206 kiflibs.cxx 318
"KIF(207): On ne peut pas initialiser un dictionnaire avec cette valeur", //207 kiflibs.cxx 319
"KIF(208): Mauvais nombre de paramètres pour REMPLIT", //208 kiflibs.cxx 320
"KIF(209): Taille trop longue pour REMPLIT", //209 kiflibs.cxx 321
"KIF(210): Mauvais paramètre initial pour REMPLIT", //210 kiflibs.cxx 322
"KIF(211): On ne peut créer une fonction TÂCHE", //211 kiflibs.cxx 323
"KIF(212): Seule une chaine ATTENDS est acceptée dans une TÂCHE", //212 kiflibs.cxx 324
"", //213 kiflibs.cxx 325 Leave blank
"KIF(214): TOUCHE n'accepte aucun paramètre", //214 kiflibs.cxx 326
"", //215 kiflibs.cxx 327 Leave blank
"", //216 kiflibs.cxx 328 Leave blank
"KIF(217): ATTENDS est interdit dans une section CAPTE", //217 kiflibs.cxx 329
"KIF(218): Mauvais nombre de paramètres pour RAZ", //218 kiflibs.cxx 330
"KIF(219): Mauvais paramètre pour RAZ", //219 kiflibs.cxx 331
"KIF(220): On ne peut pas initialiser une liste avec cette valeur", //220 kiflibs.cxx 332
"KIF(221): Session déjà ouverte", //221 kiflibs.cxx 333
"KIF(222): Session inconnue", //222 kiflibs.cxx 334
"KIF(223): Clef inconnu lors d'un accès conteneur", //223 kiflibs.cxx 335
"", //224 kiflibs.cxx 336 Leave blank
"KIF(225): Division par 0", //225 kiflibs.cxx 337
"KIF(226): Mauvais nombre de paramètres pour INTERVALLE", //226 kiflibs.cxx 338
"KIF(227): INTERVALLE au-delà de sa capacite", //227 kiflibs.cxx 339
"KIF(228): Mauvais nombre de paramètres ou mauvais type pour FORMAT", //228 kiflibs.cxx 340
"", //229 kiflibs.cxx 341 Leave blank
"KIF(230): La chaine devrait être longue d'au moins un caractère pour INTERVALLE", //230 kiflibs.cxx 342
"KIF(231): Pas de fonction compatible trouvée ou fonction inconnue:", //231 kiflibs.cxx 343
"KIF(232): Variable APPEL vide", //232 kiflibs.cxx 344
"KIF(233): Objet inconnu:", //233 kiflibs.cxx 345
"KIF(234): Objet existe déjà:", //234 kiflibs.cxx 346
"KIF(235): Mauvaise utilisation de 'ça':", //235 kiflibs.cxx 347
"KIF(236): Mauvais appel de fonction pour APPLIQUE", //236 kiflibs.cxx 348
"KIF(237): Paramètre manquant pour SYNCHRONISEE", //237 kiflibs.cxx 349
"KIF(238): Paramètre manquant pour VERROUILLE", //238 kiflibs.cxx 350
"KIF(239): Paramètre manquant pour DEVERROUILLE", //239 kiflibs.cxx 351
"KIF(240): Verrou inconnu:", //240 kiflibs.cxx 352
"KIF(241): Mauvaise utilisation de RENVOIE", //241 kiflibs.cxx 353
"KIF(242): Mauvaise définition de comparaison", //242 kiflibs.cxx 354
"KIF(243): Instruction inconnue pour cet élément", //243 kiflibs.cxx 355
"KIF(244): Expression inconnue", //244 kiflibs.cxx 356
"KIF(245): Paramètre inconnu pour FICHIER: devrait être 'l','e', 'e+' ou 'a'", //245 kiflibs.cxx 357
"KIF(246): Mauvais nombre de paramètre ou mauvais type pour EXPOSEE", //246 kiflibs.cxx 358
"KIF(247): Paramètre inconnu pour EXPOSEE", //247 kiflibs.cxx 359
"KIF(248): Mauvais nombre de paramètre ou mauvais type pour TEST", //248 kiflibs.cxx 360
"KIF(249): Mauvais nombre de paramètre ou mauvais type pour _ID", //249 kiflibs.cxx 361
"KIF(250): Une constante ne peut être modifiée", //250 kiflibs.cxx 362
"KIF(251): Mauvais nombre de paramètres ou mauvais type pour POSITIONOCTET", //251 kiflibs.cxx 363
"KIF(252): Mauvais nombre de paramètres ou mauvais type pour ESTUTF8", //252 kiflibs.cxx 364
"KIF(253): Mauvais nombre de paramètre pour ECRITUREBIN", //253 kiflibs.cxx 365
"KIF(254): Code non ASCII pour ECRITUREBIN", //254 kiflibs.cxx 366
"KIF(255): Mauvais nombre de paramètres pour LIT", //255 kiflibs.cxx 367
"KIF(256): Fonction externe inconnue", //256 kiflibs.cxx 368
"KIF(257): Mauvais nombre de paramètres pour:", //257 kiflibs.cxx 369
"KIF(258): FONCTIONDEBUG exige une fonction comme paramètre:", //258 kiflibs.cxx 370
"KIF(259): Mauvais index de pile:", //259 kiflibs.cxx 371
"KIF(260): Objet KiF attendu", //260 kiflibs.cxx 372
"KIF(261): Objet KiF vide:", //261 kiflibs.cxx 373
"KIF(262): Mauvais nombre de paramètres pour:", //262 kiflibs.cxx 374
"KIF(263): Ne peut sauvegarder cette valeur: référence infinie", //263 kiflibs.cxx 375
"KIF(264): fonction d'appel vide", //264 kiflibs.cxx 376
"KIF(265): Valeur 'vide' ne peut être sauvegardée", //265 kiflibs.cxx 377
"KIF(266): Un seul paramètre attendu pour cet objet", //266 kiflibs.cxx 378
"KIF(267): Objet TABLE plein", //267 kiflibs.cxx 379
"KIF(268): On ne peut itérer que sur des tableaux", //268 kiflibs.cxx 380
"KIF(269): La description ne correspond à un des éléments de la table", //269 kiflibs.cxx 381
"KIF(270): Je ne peux itérer que sur des dictionnaires", //270 kiflibs.cxx 382
"KIF(271): Cette structure de dictionnaire ne correspond pas à l'élément courant", //271 kiflibs.cxx 383
"KIF(272): Mauvaise valeur pour l'année. Elle doit être sur 4 chiffres.", //272 kiflibs.cxx 384
"KIF(273): Mauvaise valeur de mois", //273 kiflibs.cxx 385
"KIF(274): Mauvaise valeur de jour", //274 kiflibs.cxx 386
"KIF(275): Mauvaise valeur d'heure", //275 kiflibs.cxx 387
"KIF(276): Mauvaise valeur de seconde ou de minute", //276 kiflibs.cxx 388
"Copyright (C) LE GRAC", //277 kiflibs.cxx 483
"LE GRAC Développement", //278 kiflibs.cxx 485
"Implementation: LE GRAC", //279 kiflibs.cxx 487
"KIF(999): Veuillez définir KIFLIBS", //280 kiflibs.cxx 538
"KIF(881): Erreur, mauvais arguments pour cette fonction", //281 kiflibs.cxx 1054
"Fonction inconnue: '", //282 kiflibs.cxx 1062
"Variable inconnue: '", //283 kiflibs.cxx 1131
"Variable inconnue: '", //284 kiflibs.cxx 1141
"On attend une variable dans ce FOR", //285 kiflibs.cxx 1378
"Seules des variables numériques peuvent être utilisées ici", //286 kiflibs.cxx 1384
"arrête", //287 kiflibs.cxx 1491
"continue", //288 kiflibs.cxx 1494
"renvoie", //289 kiflibs.cxx 1497
"retourne", //290 kiflibs.cxx 1497
"_taillepile", //291 kiflibs.cxx 1580
"Paramètre manquant dans '_taillepile'", //292 kiflibs.cxx 1592
"_limitejoindre", //293 kiflibs.cxx 1598
"Paramètre manquant dans '_limitejoindre'", //294 kiflibs.cxx 1611
"_limitetâches", //295 kiflibs.cxx 1617
"Paramètre manquant dans '_limitetâches'", //296 kiflibs.cxx 1628
"_affichagemiroir", //297 kiflibs.cxx 1634
"Paramètre manquant dans '_affichagemiroir'", //298 kiflibs.cxx 1645
"_tailleramassemiette", //299 kiflibs.cxx 1651
"Paramètre manquant dans '_tailleramassemiette'", //300 kiflibs.cxx 1666
"_fonctionramassemiette", //301 kiflibs.cxx 1672
"Paramètre manquant dans '_fonctionramassemiette'", //302 kiflibs.cxx 1687
"_metenv", //303 kiflibs.cxx 1693
"Paramètre manquant dans '_metenv'", //304 kiflibs.cxx 1695
"importe", //305 kiflibs.cxx 1715
"Paramètre manquant dans 'importe'", //306 kiflibs.cxx 1765
"chargelocalement", //307 kiflibs.cxx 1771
"Paramètre manquant dans 'chargelocalement'", //308 kiflibs.cxx 1798
"Fonction appelée inconnue:", //309 kiflibs.cxx 1885
"Aucune fonction compatible trouvée pour:", //310 kiflibs.cxx 1959
"table", //311 kiflibs.cxx 2017
"chaine", //312 kiflibs.cxx 2017
"Mauvais interval pour cette variable:", //313 kiflibs.cxx 2018
"type est=", //314 kiflibs.cxx 2018
"ça", //315 kiflibs.cxx 2413
"Mauvais appel avec 'ça'", //316 kiflibs.cxx 2415
"Variable invalide: '", //317 kiflibs.cxx 2505
"vrai", //318 kiflibs.cxx 2587
"Variable: '", //319 kiflibs.cxx 2670
"' a déjà été déclarée", //320 kiflibs.cxx 2672
"Les variables communes ne peuvent être déclarée que dans une classe:", //321 kiflibs.cxx 2699
"Type inconnu: '", //322 kiflibs.cxx 2808
"varprédicat", //323 kiflibs.cxx 3000
"privée", //324 kiflibs.cxx 3307
"commune", //325 kiflibs.cxx 3313
"const", //326 kiflibs.cxx 3316
"intervalle", //327 kiflibs.cxx 3447
"ord", //328 kiflibs.cxx 3470
"Erreur: Label déjà défini: '", //329 kiflibs.cxx 3718
"enregistre", //330 kiflibs.cxx 4039
"retire", //331 kiflibs.cxx 4042
"assertebd", //332 kiflibs.cxx 4156
"rétractebd", //333 kiflibs.cxx 4164
"Erreur: Mauvaise définition de assertebd ou de rétractebd", //334 kiflibs.cxx 4176
"Erreur: Mauvaise définition de asserte ou de rétracte", //335 kiflibs.cxx 4184
"soit", //336 kiflibs.cxx 4312
"' déjà déclaré", //337 kiflibs.cxx 4459
"et", //338 kiflibs.cxx 4714
"liste", //339 kiflibs.cxx 4911
"abandonneJusquà", //340 kiflibs.cxx 5200
"prend", //341 kiflibs.cxx 5222
"abandonne", //342 kiflibs.cxx 5222
"filtre", //343 kiflibs.cxx 5329
"répète", //344 kiflibs.cxx 5674
"cycle", //345 kiflibs.cxx 5677
"Erreur: Fonction predéfinie, Veuillez donner un autre nom: '", //346 kiflibs.cxx 5850
"Erreur: Une fonction avec ce nom existe déjà: '", //347 kiflibs.cxx 5858
"itérateur", //348 kiflibs.cxx 6013
"Erreur: Vous ne pouvez pas déclarer une fonction dans une autre: '", //349 kiflibs.cxx 6238
"joindre", //350 kiflibs.cxx 6306
"protégée", //351 kiflibs.cxx 6311
"exclusive", //352 kiflibs.cxx 6316
"stricte", //353 kiflibs.cxx 6325
"polynomiale", //354 kiflibs.cxx 6332
"tâche", //355 kiflibs.cxx 6335
"autoexécute", //356 kiflibs.cxx 6342
"Erreur: Cette fonction a déjà été utilisée: '", //357 kiflibs.cxx 6368
"Erreur: Fonction correspondante à une fonction prédeclarée non trouvée (vérifiez l'ordre des déclarations): '", //358 kiflibs.cxx 6422
"Erreur: Un AUTOEXECUTE ne peut avoir de paramètres: '", //359 kiflibs.cxx 6440
"KIF(642): Erreur extension inconnue", //360 kiflibs.cxx 6460
"KIF(643): Erreur fonction d'extension inconnue", //361 kiflibs.cxx 6477
"Erreur: type inconnu: '", //362 kiflibs.cxx 6513
"omni", //363 kiflibs.cxx 6531
"Erreur: Ne peut utiliser ce nom comme fonction d'extension: '", //364 kiflibs.cxx 6550
"Erreur: Ce type ne peut être étendu: '", //365 kiflibs.cxx 6591
"Erreur: tentative d'utilisation d'une classe privée", //366 kiflibs.cxx 6626
"Erreur: Cette classe a déjà été déclarée:", //367 kiflibs.cxx 6651
"Ne peut ouvrir ce fichier kif:", //368 kiflibs.cxx 6788
"décimal f,g,h; nombre i,j,k; chaine s,t,u; table v; dictionnaire d;\n", //369 kiflibs.cxx 7069
"d'où", //370 kiflibs.cxx 7091
"(ligne:%d)", //371 kiflibs.cxx 7100
"Erreur pendant la lecture d'un fichier KiF:", //372 kiflibs.cxx 7154
"Fichier inconnu:", //373 kiflibs.cxx 7156
"Fichier inconnu:", //374 kiflibs.cxx 7710
"un objet omni peut seulement appeler des fonctions:", //375 kiflibs.cxx 8101
"partie", //376 kiflibs.cxx 8108
"Seules des fonctions peuvent être appelées depuis un conteneur:", //377 kiflibs.cxx 8130
"méthodes", //378 kiflibs.cxx 8532
"dictionnaire", //379 kiflibs.cxx 8534
"_courant", //380 kiflibs.cxx 8598
"a_premier", //381 kiflibs.cxx 8643
"a_change", //382 kiflibs.cxx 8648
"a_détruit", //383 kiflibs.cxx 8653
"a_insère", //384 kiflibs.cxx 8658
"a_dernier", //385 kiflibs.cxx 8663
"a_commute", //386 kiflibs.cxx 8668
"a_préfixe", //387 kiflibs.cxx 8673
"a_sanscasse", //388 kiflibs.cxx 8678
"a_surface", //389 kiflibs.cxx 8683
"a_tout", //390 kiflibs.cxx 8688
"a_découpe", //391 kiflibs.cxx 8693
"a_saute", //392 kiflibs.cxx 8698
"a_trace", //393 kiflibs.cxx 8703
"préd_aucun", //394 kiflibs.cxx 8708
"préd_poids", //395 kiflibs.cxx 8713
"préd_pilepoids", //396 kiflibs.cxx 8718
"pred_normalise", //397 kiflibs.cxx 8723
"pred_aléatoire", //398 kiflibs.cxx 8728
"_KIFPRINCIPALE", //399 kiflibs.cxx 8885
"CLASSE::", //400 kiflibs.cxx 8908
"FONCTION::", //401 kiflibs.cxx 8930
"h pour aide", //402 kiflibs.cxx 9043
"Fin", //403 kiflibs.cxx 9175
"$: fin mode debug", //404 kiflibs.cxx 9213
"$$: fin programme", //405 kiflibs.cxx 9214
"o: quitter la fonction courante", //406 kiflibs.cxx 9215
"i: dans la fonction", //407 kiflibs.cxx 9216
"f: indexes fichiers", //408 kiflibs.cxx 9217
"g ligne(fichier)|label: aller ligne", //409 kiflibs.cxx 9218
"b ligne(fichier)|label: point d'arrêts (si ligne==-1: détruit tous les points d'arrêt. Sans paramètre: affichage des points d'arrêt)", //410 kiflibs.cxx 9219
"s: pile d'exécution", //411 kiflibs.cxx 9220
"n (nb lignes): liste les instructions suivantes", //412 kiflibs.cxx 9221
"l f t: liste les lignes de f à t", //413 kiflibs.cxx 9222
"w variable: ajoute un observateur sur la variable", //414 kiflibs.cxx 9223
"r variable: retire un observateur sur la variable", //415 kiflibs.cxx 9224
"H: historique des commandes", //416 kiflibs.cxx 9225
"!nb: commande réexécutée", //417 kiflibs.cxx 9226
"<: plus bas dans l'historique", //418 kiflibs.cxx 9227
">: plus haut dans l'historique", //419 kiflibs.cxx 9228
"e", //420 kiflibs.cxx 9252
"l", //421 kiflibs.cxx 9273
"label inconnu:", //422 kiflibs.cxx 9302
"Ligne:", //423 kiflibs.cxx 9321
"dans", //424 kiflibs.cxx 9321
"Points d'arrêt tous détruits", //425 kiflibs.cxx 9330
"Points d'arrêt  nettoyés", //426 kiflibs.cxx 9349
"Instruction", //427 kiflibs.cxx 9377
"ligne %d dans %s", //428 kiflibs.cxx 9383
"variable inconnue", //429 kiflibs.cxx 9447
"nulle", //430 kiflibs.cxx 9462
""};


Exported const char* KIFSPARSESTRINGS[]={
"SPM(100): Index inconnu", //0 kifsparse.cxx 771
"MAT(131): Mauvais index ou mauvaise valeur (réception devrait être un dictionnaire ou un vecteurmatrice)", //1 kifsparse.cxx 851
"nom", //2 kifsparse.cxx 968
"taille", //3 kifsparse.cxx 982
"MAT(209): Les tailles des matrices ne correspondent pas", //4 kifsparse.cxx 1201
"MAT(203): Division par 0 impossible", //5 kifsparse.cxx 1238
"MAT(202): On ne peut diviser avec cette matrice", //6 kifsparse.cxx 1258
"Version de KiF incompatible", //7 kifsparse.cxx 2545
"matrice", //8 kifsparse.cxx 2554
"matricen", //9 kifsparse.cxx 2558
"transposée", //10 kifsparse.cxx 2576
"transposée(): renvoie la matrice transposée", //11 kifsparse.cxx 2576
"dimension", //12 kifsparse.cxx 2577
"dimension(): renvoie la taille de la matrice.\ndimension(nombre tailleligne,nombre taillecolonne): définit la taille de la matrice.", //13 kifsparse.cxx 2577
"déterminant", //14 kifsparse.cxx 2578
"déterminant(): renvoie le déterminant de la matrice", //15 kifsparse.cxx 2578
"somme", //16 kifsparse.cxx 2579
"somme(): renvoie la somme de tous les éléments", //17 kifsparse.cxx 2579
"produit", //18 kifsparse.cxx 2580
"produit(): renvoie le produit de tous les éléments", //19 kifsparse.cxx 2580
"inverse", //20 kifsparse.cxx 2581
"inserse(): Renvoie la matrice inversée", //21 kifsparse.cxx 2581
""};


Exported const char* KIFSOCKETSTRINGS[]={
"Pas d'erreur", //0 kifsocket.cxx 98
"taille", //1 kifsocket.cxx 249
"CONNECTEUR(855): Erreur connecteur", //2 kifsocket.cxx 417
"CONNECTEUR(851): Ne peut pas trouver un hôte de ce nom sur cette machine", //3 kifsocket.cxx 427
"CONNECTEUR(831): Serveur déjà lancé sur ce connecteur", //4 kifsocket.cxx 453
"CONNECTEUR(850): Mauvais nom de serveur", //5 kifsocket.cxx 466
"CONNECTEUR(872): Erreur sur la réutilisation de l'adresse", //6 kifsocket.cxx 482
"CONNECTEUR(853): Erreur à la connexion", //7 kifsocket.cxx 490
"CONNECTEUR(854): Erreur sur écoute", //8 kifsocket.cxx 496
"CONNECTEUR(857): Erreur à la connexion", //9 kifsocket.cxx 542
"CONNECTEUR(855): Erreur en lecture", //10 kifsocket.cxx 556
"CONNECTEUR(824): Connecteur non connecté", //11 kifsocket.cxx 610
"CONNECTEUR(888): Connexion en lecture refusée", //12 kifsocket.cxx 636
"CONNECTEUR(830): Temps dépassé", //13 kifsocket.cxx 643
"CONNECTEUR(888): Connexion en écriture refusée", //14 kifsocket.cxx 663
"CONNECTEUR(820): Paramètre manquant en lecture", //15 kifsocket.cxx 688
"CONNECTEUR(858): Aucun client connecté", //16 kifsocket.cxx 700
"CONNECTEUR(860): Erreur en lecture", //17 kifsocket.cxx 711
"Lecture:", //18 kifsocket.cxx 721
"CONNECTEUR(825): EXECUTE peut seulement être lancé depuis un serveur", //19 kifsocket.cxx 750
"CONNECTEUR(870): Objet '", //20 kifsocket.cxx 816
"' inconnu", //21 kifsocket.cxx 818
"CONNECTEUR(871): Fonction '", //22 kifsocket.cxx 874
"CONNECTEUR(822): Paramètre manquant en écriture", //23 kifsocket.cxx 1011
"Ecriture:", //24 kifsocket.cxx 1034
"CONNECTEUR(859): Erreur d'envoi", //25 kifsocket.cxx 1040
"CONNECTEUR(821): Paramètre manquant pour reçoit", //26 kifsocket.cxx 1089
"CONNECTEUR(860): Erreur sur reçoit", //27 kifsocket.cxx 1108
"CONNECTEUR(823): Trop de paramètres dans reçoit", //28 kifsocket.cxx 1145
"CONNECTEUR(860): Erreur sur reçoit", //29 kifsocket.cxx 1167
"CONNECTEUR(827): Paramètre manquant dans envoi", //30 kifsocket.cxx 1208
"port", //31 kifsocket.cxx 1265
"adresse", //32 kifsocket.cxx 1267
"CONNECTEUR(852): Vous ne pouvez pas utiliser NOMCLIENT du côté client", //33 kifsocket.cxx 1272
"nom", //34 kifsocket.cxx 1332
"CONNECTEUR(812): Méthode inconnue", //35 kifsocket.cxx 1418
"Version de KiF incompatible", //36 kifsocket.cxx 1514
"connecteur", //37 kifsocket.cxx 1524
"distant", //38 kifsocket.cxx 1529
"créeserveur", //39 kifsocket.cxx 1548
"créeserveur(chaine hôte,nombre port,nombre nblients): crée un serveur. Si l'hôte est omis, le nom local est alors utilisé", //40 kifsocket.cxx 1548
"connecte", //41 kifsocket.cxx 1549
"connecte(chaine hôte,nombre port): connexion à un serveur", //42 kifsocket.cxx 1549
"attends", //43 kifsocket.cxx 1550
"nombre attends(): attend qu'un client se connecte et renvoie son identifiant.", //44 kifsocket.cxx 1550
"lit", //45 kifsocket.cxx 1551
"lit(nombre num): lit une chaine sur le connecteur. Du côté serveur, num est l'identifiant du client. Utiliser 'lit()' sans paramètre du côté client", //46 kifsocket.cxx 1551
"lance", //47 kifsocket.cxx 1552
"lance(nombre num,chaine stop): Uniquement du côté serveur. Exécute des invocations de méthodes distantes. 'stop' est une chaine qui permet l'arrêt de ce service par le client.", //48 kifsocket.cxx 1552
"écrit", //49 kifsocket.cxx 1553
"écrit(nombre num,chaine s): Ecrit une chaine s sur le connecteur. Du côté serveur, 'num' est l'identifiant client renvoyé par 'attends'. En revanche du côté client, utiliser 'écrit(chaine s)'", //50 kifsocket.cxx 1553
"reçoit", //51 kifsocket.cxx 1554
"reçoit(nombre num): Reçoit une chaine provenant d'une application non KiF. 'num' doit être omis du côté client.", //52 kifsocket.cxx 1554
"saisie", //53 kifsocket.cxx 1555
"saisie(int num): lit un caractère depuis le connecteur. 'num' doit être omis du côté client.", //54 kifsocket.cxx 1555
"envoie", //55 kifsocket.cxx 1556
"envoie(nombre num,chaine s): Ecrit une chaine à destination d'une application non KiF. 'num' doit être omis du côté client.", //56 kifsocket.cxx 1556
"ferme", //57 kifsocket.cxx 1557
"ferme(nombre num): Ferme un connecteur. Du côté serveur, si 'num' est fourni, il ferme la connexion pour un client donné.", //58 kifsocket.cxx 1557
"bloquant", //59 kifsocket.cxx 1558
"bloquant(bool flag): Si 'flag' est 'vrai', le connecteur fonctionne en mode bloquant.", //60 kifsocket.cxx 1558
"tempsmax", //61 kifsocket.cxx 1559
"tempsmax(nombre t): Définit un temps maximum de 't' secondes pour le connecteur", //62 kifsocket.cxx 1559
"nomhôte", //63 kifsocket.cxx 1560
"nomhôte(): Retourne le nom de l'hôte", //64 kifsocket.cxx 1560
"port(): renvoie le port courant", //65 kifsocket.cxx 1561
"nomclient", //66 kifsocket.cxx 1562
"nomclient(nombre num): Retourne le nom du client", //67 kifsocket.cxx 1562
"laclasse", //68 kifsocket.cxx 1563
"laclasse(chaine frame): Retourne un identifiant objet déclaré du côté serveur", //69 kifsocket.cxx 1563
"lafonction", //70 kifsocket.cxx 1564
"lafonction(chaine frame): Retourne une fonction declarée du côté serveur", //71 kifsocket.cxx 1564
"mavariable", //72 kifsocket.cxx 1570
""};


Exported const char* KIFLTKSTRINGS[]={
"WND(303): Pas de fenêtre disponible", //0 kifltk.cxx 139
"KIF(93): Mauvais nombre de paramètres pour RECUPERE", //1 kifltk.cxx 231
"saisie", //2 kifltk.cxx 248
"COULEUR_AVANTPLAN", //3 kifltk.cxx 283
"COULEUR_ARRIEREPLAN2", //4 kifltk.cxx 284
"COULEUR_ARRIEREPLAN", //5 kifltk.cxx 285
"COULEUR_INACTIVE", //6 kifltk.cxx 286
"COULEUR_SELECTION", //7 kifltk.cxx 287
"GRI0", //8 kifltk.cxx 288
"FONCE3", //9 kifltk.cxx 289
"FONCE2", //10 kifltk.cxx 290
"FONCE1", //11 kifltk.cxx 291
"LEGER1", //12 kifltk.cxx 292
"LEGER2", //13 kifltk.cxx 293
"LEGER3", //14 kifltk.cxx 294
"NOIR", //15 kifltk.cxx 295
"ROUGE", //16 kifltk.cxx 296
"VERT", //17 kifltk.cxx 297
"JAUNE", //18 kifltk.cxx 298
"BLEU", //19 kifltk.cxx 299
"MAGENTA", //20 kifltk.cxx 300
"CYAN", //21 kifltk.cxx 301
"ROUGE_FONCE", //22 kifltk.cxx 302
"VERT_FONCE", //23 kifltk.cxx 303
"JAUNE_FONCE", //24 kifltk.cxx 304
"BLEU_FONCE", //25 kifltk.cxx 305
"MAGENTA_FONCE", //26 kifltk.cxx 306
"CYAN_FONCE", //27 kifltk.cxx 307
"BLANC", //28 kifltk.cxx 308
"Régulier", //29 kifltk.cxx 310
"Cocher", //30 kifltk.cxx 311
"Léger", //31 kifltk.cxx 312
"Répéter", //32 kifltk.cxx 313
"Renvoie", //33 kifltk.cxx 314
"Rond", //34 kifltk.cxx 315
"Image", //35 kifltk.cxx 316
"POUSSE", //36 kifltk.cxx 319
"RELACHE", //37 kifltk.cxx 320
"DRAGUE", //38 kifltk.cxx 321
"BOUGE", //39 kifltk.cxx 322
"ROUESOURIS", //40 kifltk.cxx 323
"TOUCHEPRESSEE", //41 kifltk.cxx 324
"TOUCHERELACHEE", //42 kifltk.cxx 325
"GLISSIERE_VERTICALE", //43 kifltk.cxx 327
"GLISSIERE_HORIZONTALE", //44 kifltk.cxx 328
"GLISSIERE_VERTICALE_PLEINE", //45 kifltk.cxx 329
"GLISSIERE_HORIZONTALE_PLEINE", //46 kifltk.cxx 330
"GLISSIERE_VERTICALE_JOLIE", //47 kifltk.cxx 331
"GLISSIERE_HORIZONTALE_JOLIE", //48 kifltk.cxx 332
"SOLIDE", //49 kifltk.cxx 334
"TIRET", //50 kifltk.cxx 335
"POINTILLE", //51 kifltk.cxx 336
"POINTTIRET", //52 kifltk.cxx 337
"TIRETPOINTPOINT", //53 kifltk.cxx 338
"CAP_PLAT", //54 kifltk.cxx 339
"CAP_ROND", //55 kifltk.cxx 340
"CAP_CARRE", //56 kifltk.cxx 341
"JOINT_MITER", //57 kifltk.cxx 342
"JOINT_ROND", //58 kifltk.cxx 343
"JOINT_BEVEL", //59 kifltk.cxx 344
"ALIGNE_CENTRE", //60 kifltk.cxx 346
"ALIGNE_HAUT", //61 kifltk.cxx 347
"ALIGNE_BAS", //62 kifltk.cxx 348
"ALIGNE_GAUCHE", //63 kifltk.cxx 349
"ALIGNE_DROIT", //64 kifltk.cxx 350
"ALIGNE_DEDANS", //65 kifltk.cxx 351
"ALIGNE_TEXTE_AU_DESSUS_IMAGE", //66 kifltk.cxx 352
"ALIGNE_IMAGE_DESSUS_TEXTE", //67 kifltk.cxx 353
"ALIGNE_CLIP", //68 kifltk.cxx 354
"ALIGNE_ENROULE", //69 kifltk.cxx 355
"ALIGNE_IMAGE_ACOTE_TEXTE", //70 kifltk.cxx 356
"ALIGNE_TEXTE_ACOTE_IMAGE", //71 kifltk.cxx 357
"ALIGNE_IMAGE_BACKDROP", //72 kifltk.cxx 358
"ALIGNE_HAUT_GAUCHE", //73 kifltk.cxx 359
"ALIGNE_HAUT_DROIT", //74 kifltk.cxx 360
"ALIGNE_BAS_GAUCHE", //75 kifltk.cxx 361
"ALIGNE_BAS_DROIT", //76 kifltk.cxx 362
"ALIGNE_GAUCHE_HAUT", //77 kifltk.cxx 363
"ALIGNE_DROIT_HAUT", //78 kifltk.cxx 364
"ALIGNE_GAUCHE_BAS", //79 kifltk.cxx 365
"ALIGNE_DROIT_BAS", //80 kifltk.cxx 366
"ALIGNE_NOWRAP", //81 kifltk.cxx 367
"ALIGNE_POSITION_MASQUE", //82 kifltk.cxx 368
"ALIGNE_IMAGE_MASQUE", //83 kifltk.cxx 369
"LABEL_NORMAL", //84 kifltk.cxx 371
"PAS_DE_LABEL", //85 kifltk.cxx 372
"LABEL_OMBRE", //86 kifltk.cxx 373
"LABEL_GRAVE", //87 kifltk.cxx 374
"LABEL_BOSSELE", //88 kifltk.cxx 375
"CURSEUR_DEFAUT", //89 kifltk.cxx 377
"CURSEUR_FLECHE", //90 kifltk.cxx 378
"CURSEUR_CROIX", //91 kifltk.cxx 379
"CURSEUR_SABLIER", //92 kifltk.cxx 380
"CURSEUR_INSERE", //93 kifltk.cxx 381
"CURSEUR_MAIN", //94 kifltk.cxx 382
"CURSEUR_AIDE", //95 kifltk.cxx 383
"CURSEUR_BOUGE", //96 kifltk.cxx 384
"CURSEUR_NS", //97 kifltk.cxx 385
"CURSEUR_OE", //98 kifltk.cxx 386
"CURSEUR_NOSE", //99 kifltk.cxx 387
"CURSEUR_NESO", //100 kifltk.cxx 388
"CURSEUR_AUCUN", //101 kifltk.cxx 389
"CURSEUR_N", //102 kifltk.cxx 390
"CURSEUR_NE", //103 kifltk.cxx 391
"CURSEUR_E", //104 kifltk.cxx 392
"CURSEUR_SE", //105 kifltk.cxx 393
"CURSEUR_S", //106 kifltk.cxx 394
"CURSEUR_SO", //107 kifltk.cxx 395
"CURSEUR_O", //108 kifltk.cxx 396
"CURSEUR_NO", //109 kifltk.cxx 397
"CURSEUR_NORMAL", //110 kifltk.cxx 399
"CURSEUR_CHARIOT", //111 kifltk.cxx 400
"CURSEUR_DIM", //112 kifltk.cxx 401
"CURSEUR_BLOC", //113 kifltk.cxx 402
"CURSEUR_LOURD", //114 kifltk.cxx 403
"CURSEUR_LIGNE", //115 kifltk.cxx 404
"MAJ", //116 kifltk.cxx 406
"VERROUILLAGE_MAJ", //117 kifltk.cxx 407
"VERROUILLAGE_NUM", //118 kifltk.cxx 410
"VERROUILLANCE_DEFILEMENT", //119 kifltk.cxx 412
"BOUTON1", //120 kifltk.cxx 413
"BOUTON2", //121 kifltk.cxx 414
"BOUTON3", //122 kifltk.cxx 415
"BOUTONS", //123 kifltk.cxx 416
"MASQUE_TOUCHE", //124 kifltk.cxx 417
"COMMANDE", //125 kifltk.cxx 418
"CONTROLE", //126 kifltk.cxx 419
"Bouton", //127 kifltk.cxx 421
"Retour_Arrière", //128 kifltk.cxx 422
"Entrée", //129 kifltk.cxx 424
"Pause", //130 kifltk.cxx 425
"Verrouillage_Défilement", //131 kifltk.cxx 426
"Echappe", //132 kifltk.cxx 427
"Accueil", //133 kifltk.cxx 428
"Gauche", //134 kifltk.cxx 429
"Haut", //135 kifltk.cxx 430
"Droit", //136 kifltk.cxx 431
"Bas", //137 kifltk.cxx 432
"Page_Haut", //138 kifltk.cxx 433
"Page_Bas", //139 kifltk.cxx 434
"Fin", //140 kifltk.cxx 435
"Imprimer", //141 kifltk.cxx 436
"Insère", //142 kifltk.cxx 437
"Menu", //143 kifltk.cxx 438
"Aide", //144 kifltk.cxx 439
"Verrouillage_Num", //145 kifltk.cxx 440
"KP_Entrée", //146 kifltk.cxx 442
"KP_Dernier", //147 kifltk.cxx 443
"F_Dernier", //148 kifltk.cxx 445
"Maj_G", //149 kifltk.cxx 446
"Maj_D", //150 kifltk.cxx 447
"Contrôle_G", //151 kifltk.cxx 448
"Contrôle_D", //152 kifltk.cxx 449
"Verrouillage_Maj", //153 kifltk.cxx 450
"Méta_G", //154 kifltk.cxx 451
"Méta_D", //155 kifltk.cxx 452
"Alt_G", //156 kifltk.cxx 453
"Alt_D", //157 kifltk.cxx 454
"Détruit", //158 kifltk.cxx 455
"PAS_DE_BOITE", //159 kifltk.cxx 457
"BOITE_PLATE", //160 kifltk.cxx 458
"BOITE_HAUTE", //161 kifltk.cxx 459
"BOITE_BASSE", //162 kifltk.cxx 460
"CADRE_HAUT", //163 kifltk.cxx 461
"CADRE_BAS", //164 kifltk.cxx 462
"BOITE_FINE_HAUTE", //165 kifltk.cxx 463
"BOITE_FINE_BASE", //166 kifltk.cxx 464
"CADRE_FIN_HAUT", //167 kifltk.cxx 465
"CADRE_FIN_BASE", //168 kifltk.cxx 466
"BOITE_GRAVEE", //169 kifltk.cxx 467
"BOITE_BOSSELEE", //170 kifltk.cxx 468
"CADRE_GRAVE", //171 kifltk.cxx 469
"CADRE_BOSSELE", //172 kifltk.cxx 470
"BOITE_BORDEE", //173 kifltk.cxx 471
"BOITE_OMBREE", //174 kifltk.cxx 472
"CADRE_BORDE", //175 kifltk.cxx 473
"CADRE_OMBRE", //176 kifltk.cxx 474
"BOITE_RONDE", //177 kifltk.cxx 475
"BOITE_ROMBREE", //178 kifltk.cxx 476
"CADRE_ROND", //179 kifltk.cxx 477
"BOITE_RPLATE", //180 kifltk.cxx 478
"BOITE_RONDE_HAUTE", //181 kifltk.cxx 479
"BOITE_RONDE_BASSE", //182 kifltk.cxx 480
"BOITE_DIAMANT_HAUTE", //183 kifltk.cxx 481
"BOITE_DIAMANT_BASSE", //184 kifltk.cxx 482
"BOITE_OVALE", //185 kifltk.cxx 483
"BOITE_OMBREEO", //186 kifltk.cxx 484
"CADRE_OVAL", //187 kifltk.cxx 485
"BOITE_PLATEO", //188 kifltk.cxx 486
"BOITE_PLASTIC_HAUTE", //189 kifltk.cxx 487
"BOITE_PLASTIC_BASSE", //190 kifltk.cxx 488
"CADRE_PLASTIC_HAUT", //191 kifltk.cxx 489
"CADRE_PLASTIC_BAS", //192 kifltk.cxx 490
"BOITE_PLASTIC_FINE_HAUTE", //193 kifltk.cxx 491
"BOITE_PLASTIC_FINE_BASSE", //194 kifltk.cxx 492
"BOITE_PLASTIC_RONDE_HAUTE", //195 kifltk.cxx 493
"BOITE_PLASTIC_RONDE_BASE", //196 kifltk.cxx 494
"BOITE_GTK_HAUTE", //197 kifltk.cxx 495
"BOITE_GTK_BASSE", //198 kifltk.cxx 496
"CADRE_GTK_HAUT", //199 kifltk.cxx 497
"CADRE_GTK_BAS", //200 kifltk.cxx 498
"BOITE_GTK_FINE_HAUTE", //201 kifltk.cxx 499
"BOITE_GTK_FINE_BASSE", //202 kifltk.cxx 500
"CADRE_GTK_FINE_HAUT", //203 kifltk.cxx 501
"CADRE_GTK_FINE_BAS", //204 kifltk.cxx 502
"BOITE_GTK_ROND_HAUTE", //205 kifltk.cxx 503
"BOITE_GTK_ROND_BASSE", //206 kifltk.cxx 504
"BOITE_TYPE_LIBRE", //207 kifltk.cxx 505
"HELVETICA_GRAS", //208 kifltk.cxx 508
"HELVETICA_ITALIQUE", //209 kifltk.cxx 509
"HELVETICA_GRAS_ITALIQUE", //210 kifltk.cxx 510
"COURIER_GRAS", //211 kifltk.cxx 512
"COURIER_ITALIQUE", //212 kifltk.cxx 513
"COURIER_GRAS_ITALIQUE", //213 kifltk.cxx 514
"TIMES_GRAS", //214 kifltk.cxx 516
"TIMES_ITALIQUE", //215 kifltk.cxx 517
"TIMES_GRAS_ITALIQUE", //216 kifltk.cxx 518
"SYMBOLE", //217 kifltk.cxx 519
"ECRAN", //218 kifltk.cxx 520
"ECRAN_GRAS", //219 kifltk.cxx 521
"GRAS", //220 kifltk.cxx 524
"ITALIQUE", //221 kifltk.cxx 525
"GRAS_ITALIQUE", //222 kifltk.cxx 526
"BOUTON_NORMAL", //223 kifltk.cxx 546
"BOUTON_INVERSEUR", //224 kifltk.cxx 547
"BOUTON_RADIO", //225 kifltk.cxx 548
"BOUTON_CACHE", //226 kifltk.cxx 549
"QUAND_JAMAIS", //227 kifltk.cxx 551
"QUAND_CHANGE", //228 kifltk.cxx 552
"QUAND_RELACHE", //229 kifltk.cxx 553
"QUAND_RELACHE_TOUJOURS", //230 kifltk.cxx 554
"QUAND_TOUCHE_ENTREE", //231 kifltk.cxx 555
"QUAND_TOUCHE_ENTREE_TOUJOURS", //232 kifltk.cxx 556
"FL_TAILLE_NORMALE", //233 kifltk.cxx 573
"FL_CHOIX_SIMPLE", //234 kifltk.cxx 674
"FL_CHOIX_MULTIPLE", //235 kifltk.cxx 678
"FL_CHOIX_SAUVEGARDE", //236 kifltk.cxx 682
"FL_CHOIX_REPERTOIRE", //237 kifltk.cxx 686
"FL_ARBRE_SELECTION_AUCUNE", //238 kifltk.cxx 691
"FL_ARBRE_SELECTION_SIMPLE", //239 kifltk.cxx 695
"FL_ARBRE_SELECTION_MULTI", //240 kifltk.cxx 699
"FL_ARBRE_TRI_AUCUN", //241 kifltk.cxx 703
"FL_ARBRE_TRI_ASCENDANT", //242 kifltk.cxx 707
"FL_ARBRE_TRI_DESCENDANT", //243 kifltk.cxx 711
"FL_ARBRE_CONNECTEUR_AUCUN", //244 kifltk.cxx 715
"FL_ARBRE_CONNECTEUR_POINTILLE", //245 kifltk.cxx 719
"FL_ARBRE_CONNECTEUR_SOLIDE", //246 kifltk.cxx 723
"FL_ARBRE_RAISON_AUCUNE", //247 kifltk.cxx 727
"FL_ARBRE_RAISON_SELECTIONNE", //248 kifltk.cxx 731
"FL_ARBRE_RAISON_DESELECTIONNE", //249 kifltk.cxx 735
"FL_ARBRE_RAISON_OUVERT", //250 kifltk.cxx 739
"FL_ARBRE_RAISON_FERME", //251 kifltk.cxx 743
"WND(098): Widget ne peut être instancié", //252 kifltk.cxx 860
"WND(678): Widget non initialisé", //253 kifltk.cxx 894
"WND(805): Widget non initialisé", //254 kifltk.cxx 917
"WND(202): alignement inconnu", //255 kifltk.cxx 924
"WND(657): Code couleur inconnu", //256 kifltk.cxx 953
"WND(904): Tablean nécessaire", //257 kifltk.cxx 1022
"WND(905): Coordonnées manquantes", //258 kifltk.cxx 1025
"WND(905): Coordonnées incohérentes", //259 kifltk.cxx 1038
"WND(678): Widget non initialisé", //260 kifltk.cxx 1150
"WND(102): Widget non crée", //261 kifltk.cxx 1169
"WND(765): Police inconnu:", //262 kifltk.cxx 1181
"WND(658): Police inconnue", //263 kifltk.cxx 1192
"avec", //264 kifltk.cxx 1364
"nom", //265 kifltk.cxx 1563
"WND(808): Image déjà charge", //266 kifltk.cxx 1587
"WND(809): Image non charge", //267 kifltk.cxx 1593
"taille", //268 kifltk.cxx 1618
"WND(809): Mauvais type pour le premier paramètre", //269 kifltk.cxx 1700
"WND(811): La taille du bitmap est fausse. taille=(largeur*hauteur)/8", //270 kifltk.cxx 1714
"WND(808): Bitmap déja chargé", //271 kifltk.cxx 1783
"WND(607): code de forme de ligne inconnu", //272 kifltk.cxx 2005
"WND(245): Mauvais nombre d'éléments dans la table de couleurs", //273 kifltk.cxx 2031
"L", //274 kifltk.cxx 2291
"WND(657): Code de police inconnu", //275 kifltk.cxx 2330
"WND(608): Nombre de paramètres attendu=5", //276 kifltk.cxx 2358
"WND(608): Nombre de paramètres attendu=6", //277 kifltk.cxx 2363
"WND(612): Ne peut appeler cette méthode ici. débutpoints manquant", //278 kifltk.cxx 2522
"WND(612): Ne peut appeler cette méthode ici. débutboucle manquant", //279 kifltk.cxx 2568
"WND(612): Ne peut appeler cette méthode ici. débutpolygone manquant", //280 kifltk.cxx 2593
"WND(612): Ne peut appeler cette méthode ici. débutpolygonecomplexe manquant", //281 kifltk.cxx 2618
"WND(612): Ne peut appeler cette méthode ici. débutligne manquant", //282 kifltk.cxx 2656
"WND(610): Vous ne pouvez pas utiliser cette méthode dans ce contexte", //283 kifltk.cxx 2670
"WND(100): Ne peut recréer une fenêtre existante", //284 kifltk.cxx 2894
"WND(431): Identifiant de police non disponible", //285 kifltk.cxx 2923
"WND(109): Le premier élément doit être une fonction", //286 kifltk.cxx 2966
"WND(102): Fenêtre déjà instanciée", //287 kifltk.cxx 3174
"WND(341): Action souris inconnue", //288 kifltk.cxx 3190
"WND(342): Mauvais paramètre dans votre fonction de rappel souris", //289 kifltk.cxx 3196
"WND(344): Action clavier inconnue", //290 kifltk.cxx 3223
"WND(343): Mauvais paramètre dans votre fonction de rappel clavier", //291 kifltk.cxx 3230
"WND(872): Le premier paramètre doit être une table", //292 kifltk.cxx 3286
"WND(871): Ce conteneur doit contenir un nombre pair d'éléments.", //293 kifltk.cxx 3289
"WND(873): Le troisième paramètre doit être une table", //294 kifltk.cxx 3310
"WND(862): Ces valeurs ne peuvent permettre de calculer des coordonnées", //295 kifltk.cxx 3421
"WND(167): Image inconnue", //296 kifltk.cxx 3499
"WND(168): Image non chargée", //297 kifltk.cxx 3507
"WND(169): bitmap inconnu", //298 kifltk.cxx 3527
"WND(109): table nécessaire", //299 kifltk.cxx 3596
"WND(109): table nécessaire", //300 kifltk.cxx 3605
"WND(109): Mauvais élément, champ manquant dans un sous-menu", //301 kifltk.cxx 3632
"WND(109): Mauvais élément, deuxieme champ dans un sous-menu doit être une table", //302 kifltk.cxx 3637
"WND(109): Mauvais élément, troisième champ dans un sous-menu doit être une fonction", //303 kifltk.cxx 3654
"WND(123): Operation non disponible", //304 kifltk.cxx 3820
"WND(134): Widget n'a pas encore été crée", //305 kifltk.cxx 3832
"WND(133): Widget attendu comme paramètre", //306 kifltk.cxx 3837
"bouton", //307 kifltk.cxx 4105
"rouex", //308 kifltk.cxx 4106
"rouey", //309 kifltk.cxx 4107
"xracine", //310 kifltk.cxx 4110
"yracine", //311 kifltk.cxx 4111
"WND(101): fentrée déjà crée", //312 kifltk.cxx 4365
"WND(677): Entrée non initialisée", //313 kifltk.cxx 4391
"WND(101): éditeur déjà crée", //314 kifltk.cxx 4987
"KIF(881): Erreur, mauvais arguments pour cette fonction", //315 kifltk.cxx 5001
"WND(101): Aucun éditeur disponible", //316 kifltk.cxx 5014
"WND(342): Mauvais paramètre dans votre fonction de rappel clavier", //317 kifltk.cxx 5143
"EDT(768): Mauvais nombre d'éléments dans la table... 3 ou 4 expected", //318 kifltk.cxx 5152
"WND(627): Nombre maximum de styles atteint", //319 kifltk.cxx 5221
"WND(676): Editeur non initialisé", //320 kifltk.cxx 5266
"EDT(766): Mauvaise fentrée: dictionnaire attendu", //321 kifltk.cxx 5273
"EDT(770): Dictionnaire des styles vide", //322 kifltk.cxx 5282
"EDT(767): Clef '#' manquante dans le dictionnaire. '#' est le clef de base et est obligatoire", //323 kifltk.cxx 5301
"EDT(767): Mauvaise fentrée: table attendu", //324 kifltk.cxx 5304
"WND(689): Hors de portée", //325 kifltk.cxx 5531
"WND(679): Table des styles non initialisée", //326 kifltk.cxx 5564
"WND(679): Clef de style inconnue", //327 kifltk.cxx 5575
"début", //328 kifltk.cxx 5844
"fin", //329 kifltk.cxx 5845
"curseur", //330 kifltk.cxx 6407
"WND(101): fsortie déjà crée", //331 kifltk.cxx 6609
"WND(679): fsortie non initialisé", //332 kifltk.cxx 6631
"WND(101): bouton déjà crée", //333 kifltk.cxx 6946
"WND(675): forme inconnu de bouton:", //334 kifltk.cxx 6954
"WND(675): type inconnu de bouton:", //335 kifltk.cxx 6974
"WND(805): Bouton non initialisé", //336 kifltk.cxx 7023
"WND(809): Bouton de type image requis", //337 kifltk.cxx 7025
"WND(813): objet image requis", //338 kifltk.cxx 7029
"WND(654): action inconnue", //339 kifltk.cxx 7104
"WND(656): code touche inconnu", //340 kifltk.cxx 7130
"WND(101): boite déjà crée", //341 kifltk.cxx 7319
"WND(102): boite non crée", //342 kifltk.cxx 7336
"WND(767): type inconnu:", //343 kifltk.cxx 7341
"WND(101): fgroupe déjà crée", //344 kifltk.cxx 7525
"WND(101): fgroupe non crée", //345 kifltk.cxx 7547
"WND(507): Ce fgroupe n'appartient pas à une structure onglet", //346 kifltk.cxx 7551
"WND(101): onglet déjà crée", //347 kifltk.cxx 7750
"WND(101): onglet non crée", //348 kifltk.cxx 7768
"WND(800): Pas d'onglet actif", //349 kifltk.cxx 7782
"WND(506): Groupe vide", //350 kifltk.cxx 7792
"WND(507): Ce fgroupe n'appartient pas à cet onglet", //351 kifltk.cxx 7794
"WND(105): Vous pouvez seulement ajouter des objets fgroupe", //352 kifltk.cxx 7817
"WND(800): Mauvais nombre de paramètres", //353 kifltk.cxx 7823
"WND(505): Vous pouvez seulement retirer des objets fgroupe", //354 kifltk.cxx 7841
"WND(101): ftable déjà crée", //355 kifltk.cxx 8157
"WND(102): ftable n'a pas encore été créée", //356 kifltk.cxx 8190
"WND(678): ftable non initialisée", //357 kifltk.cxx 8400
"WND(102): ftable non crée", //358 kifltk.cxx 8477
"WND(805): ftable non créee", //359 kifltk.cxx 8500
"haut", //360 kifltk.cxx 8631
"gauche", //361 kifltk.cxx 8632
"bas", //362 kifltk.cxx 8633
"droit", //363 kifltk.cxx 8634
"valeurs", //364 kifltk.cxx 8636
"WND(100): Ne peut recréer un widget défilement", //365 kifltk.cxx 8753
"WND(101): Pas de défilement disponible", //366 kifltk.cxx 8777
"WND(100): Ne peut recréer un widget glissière", //367 kifltk.cxx 8941
"WND(101): Pas de glissière disponible", //368 kifltk.cxx 8975
"WND(206): Mauvais type de glissière", //369 kifltk.cxx 9029
"WND(100): Ne peut recréer un widget choix", //370 kifltk.cxx 9261
"WND(678): choix non initialisé", //371 kifltk.cxx 9283
"WND(100): Ne peut recréer un widget fparcourir", //372 kifltk.cxx 9586
"WND(121): Le widget fparcourir n'a pas encore été crée", //373 kifltk.cxx 9609
"WND(094): Seul un objet fnoeudarbre peut affecter un autre fnoeudarbre", //374 kifltk.cxx 9827
"WND(117): fnoeudarbre inconnu", //375 kifltk.cxx 9869
"WND(118): Mauvais index enfant", //376 kifltk.cxx 10085
"WND(210): Impossible de recréer un fcompteur", //377 kifltk.cxx 10268
"WND(211): Aucun fcompteur disponible", //378 kifltk.cxx 10291
"WND(220): On ne peut pas recréer cette fprogression", //379 kifltk.cxx 10603
"WND(221): Pas de fprogression disponible", //380 kifltk.cxx 10625
"WND(100): Ne peut recréer un widget farbre", //381 kifltk.cxx 10853
"WND(121): widget farbre n'a pas encore été crée", //382 kifltk.cxx 10876
"WND(125): Label vide, création du widget impossible", //383 kifltk.cxx 10899
"WND(341): Mauvais type pour le premier élément", //384 kifltk.cxx 10962
"WND(100): Ne peut recréer un widget fparcourirfichiers", //385 kifltk.cxx 11518
"WND(101): fparcourirfichiers non initialisé", //386 kifltk.cxx 11557
"Version de KiF incompatible", //387 kifltk.cxx 12087
"fenêtre", //388 kifltk.cxx 12096
"boite", //389 kifltk.cxx 12101
"onglet", //390 kifltk.cxx 12106
"fgroupe", //391 kifltk.cxx 12111
"choix", //392 kifltk.cxx 12116
"éditeur", //393 kifltk.cxx 12126
"fentrée", //394 kifltk.cxx 12131
"ftable", //395 kifltk.cxx 12136
"fsortie", //396 kifltk.cxx 12141
"défilement", //397 kifltk.cxx 12146
"glissière", //398 kifltk.cxx 12151
"fcompteur", //399 kifltk.cxx 12156
"fprogression", //400 kifltk.cxx 12161
"image", //401 kifltk.cxx 12166
"bitmap", //402 kifltk.cxx 12171
"fparcourirfichiers", //403 kifltk.cxx 12176
"fparcourir", //404 kifltk.cxx 12181
"fnoeudarbre", //405 kifltk.cxx 12186
"farbre", //406 kifltk.cxx 12191
"crée", //407 kifltk.cxx 12209
"crée(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée une fenêtre sans widgets, largeur et hauteur sont optionnels", //408 kifltk.cxx 12209
"commence", //409 kifltk.cxx 12210
"commence(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée une fenêtre et commence l'initialisation, largeur et hauteur sont optionnels", //410 kifltk.cxx 12210
"fin(): fin création", //411 kifltk.cxx 12211
"lance", //412 kifltk.cxx 12212
"lance(): lance la boucle principale de l'interface graphique", //413 kifltk.cxx 12212
"ferme", //414 kifltk.cxx 12213
"ferme(): ferme la fenêtre", //415 kifltk.cxx 12213
"àlafermeture", //416 kifltk.cxx 12214
"àlafermeture(fonction,objet): Fonction de rappel lors de la fermeture", //417 kifltk.cxx 12214
"autemps", //418 kifltk.cxx 12215
"autemps(fonction,t,objet): Initialise une fonction de rappel pour chaque intervalle de temps t", //419 kifltk.cxx 12215
"initialisepolices", //420 kifltk.cxx 12217
"initialisepolices(): Charge les polices systèmes. Renvoie le nombre de polices disponibles", //421 kifltk.cxx 12217
"nompolice", //422 kifltk.cxx 12218
"nompolice(nombre num): nom de la police.", //423 kifltk.cxx 12218
"taillepolices", //424 kifltk.cxx 12219
"taillepolices(nombre num): renvoie une table des tailles disponibles de police.", //425 kifltk.cxx 12219
"nombrepolice", //426 kifltk.cxx 12220
"nombrepolice(): Renvoie le nombre de polices disponibles.", //427 kifltk.cxx 12220
"dessinepoints", //428 kifltk.cxx 12221
"dessinepoints(tabledécimaux xy,nombre épaisseur,tabledécimaux repère): Dessine un graphe à partir des coordonnées successives x,y dans xy. Renvoie une table de décimaux correspondant aux éléments nécessaires pour calculer une nouvelle position avec coordsdessinepoints. Si épaisseur==0, alors les points sont joints les uns aux autres. Le repère a la forme: [maxXfenêtre,maxYfenêtre,minxXValeur,minYValeur,maxXValeur,maxYValeur,incx,incy]. On peut fournir une table comprendant les 2 premières valeurs, les 6 premières ou toutes.", //429 kifltk.cxx 12221
"coordsdessinepoints", //430 kifltk.cxx 12222
"coordsdessinepoints(tabledécimaux inits,décimal x, décimal y): renvoie les coordonnées fenêtre [xe,ye] d'un point x,y en fonction des paramêtres d'échelle calculés pour dessiner le graphe.", //431 kifltk.cxx 12222
"demande", //432 kifltk.cxx 12224
"demande(chaine msg,chaine boutonmsg1,chaine boutonmsg2,chaine boutonmsg3): Fenêtre pour poser une question", //433 kifltk.cxx 12224
"alerte", //434 kifltk.cxx 12225
"alerte(chaine msg): fenêtre pour afficher une alerte", //435 kifltk.cxx 12225
"motdepasse", //436 kifltk.cxx 12226
"motdepasse(chaine msg): Affiche une fenêtre de saisie de mot de passe", //437 kifltk.cxx 12226
"saisie(chaine msg): Affiche une fenêtre pour lire une valeur", //438 kifltk.cxx 12227
"menu", //439 kifltk.cxx 12228
"menu(table,nombre x,nombre y,nombre  largeur, nombre h): initialise un menu avec ses fonction de rappels", //440 kifltk.cxx 12228
"redimensionnable", //441 kifltk.cxx 12229
"redimensionnable(objet): objet redimensionnable", //442 kifltk.cxx 12229
"taillemax", //443 kifltk.cxx 12230
"taillemax(nombre minw,nombre minh, nombre maxw,nombre maxh): définit les tailles maximum d'agrandissement de la fenêtre", //444 kifltk.cxx 12230
"modale", //445 kifltk.cxx 12231
"modale(bool b): Si vrai  la fenêtre modale. Si pas de paramètres renvoie si la fenêtre est modale", //446 kifltk.cxx 12231
"bords", //447 kifltk.cxx 12232
"bords(bool b): Si vrai ajoute ou retire les bords. Si pas de paramètres renvoie si la fenêtre a des bords", //448 kifltk.cxx 12232
"couleurarrièreplan", //449 kifltk.cxx 12233
"couleurarrièreplan(nombre c|chaine c): initialise la couleur d'arrière-plan", //450 kifltk.cxx 12233
"couleurdessin", //451 kifltk.cxx 12235
"couleurdessin(nombre c|chaine c): Couleur des prochains objets dessinés", //452 kifltk.cxx 12235
"rectangle", //453 kifltk.cxx 12236
"rectangle(nombre x,nombre y,nombre  largeur, nombre  hauteur, chaine c|nombre c): Dessine un rectangle avec la couleur optionnelle c", //454 kifltk.cxx 12236
"sommet", //455 kifltk.cxx 12237
"sommet(décimal x,décimal y): Ajoute un sommet à une structure complexe", //456 kifltk.cxx 12237
"rectangleplein", //457 kifltk.cxx 12238
"rectangleplein(nombre x,nombre y,nombre  largeur, nombre  hauteur, chaine c|nombre c): Remplit un rectangle avec la couleur optionnelle c", //458 kifltk.cxx 12238
"formeligne", //459 kifltk.cxx 12239
"formeligne(chaine type,nombre largeur): Sélectionne la forme de la ligne et son épaisseur", //460 kifltk.cxx 12239
"polygone", //461 kifltk.cxx 12240
"polygone(nombre x,nombre y,nombre x1, nombre y1,nombre x2, nombre y2, nombre x3, nombre y3): Dessine un polygone, x3 et y3 sont optionnels", //462 kifltk.cxx 12240
"boucle", //463 kifltk.cxx 12241
"boucle(nombre x,nombre y,nombre x1, nombre y1,nombre x2, nombre y2, nombre x3, nombre y3): Dessine une série de lignes, x3 et y3 sont optionnels", //464 kifltk.cxx 12241
"ligne", //465 kifltk.cxx 12242
"ligne(nombre x,nombre y,nombre x1, nombre y1,nombre x2, nombre y2): Dessine une ligne entre deux points, x2 et y2 sont optionnels", //466 kifltk.cxx 12242
"rotation", //467 kifltk.cxx 12243
"rotation(décimal x,décimal y, décimal distance, décimal angle, bool dessine): Calcule les coordonnées d'un point après rotation du point de coordonnées x,y, selon une distance et un angle (en radian). Renvoie une table de décimaux contenant ces nouvelles coordonnées.", //468 kifltk.cxx 12243
"empilematrice", //469 kifltk.cxx 12245
"empilematrice(): Sauvegarde la transformation courante", //470 kifltk.cxx 12245
"dépilematrice", //471 kifltk.cxx 12246
"dépilematrice(): Restaure la transformation courante", //472 kifltk.cxx 12246
"échelle", //473 kifltk.cxx 12247
"échelle(décimal x,décimal y)|(x): Met à l'échelle la transformation courante", //474 kifltk.cxx 12247
"translate", //475 kifltk.cxx 12248
"translate(décimal x,décimal y): translate la transformation courante", //476 kifltk.cxx 12248
"pivote", //477 kifltk.cxx 12249
"pivote(décimal d): tourne de d degrés la transformation courante", //478 kifltk.cxx 12249
"multmatrice", //479 kifltk.cxx 12250
"multmatrice(décimal a,décimal b,décimal c,décimal d, décimal x, décimal y): combine les transformations", //480 kifltk.cxx 12250
"transformex", //481 kifltk.cxx 12252
"transformex(décimal x,décimal y): Transforme les coordonnées en X sur la base de la matrice courante de transformation", //482 kifltk.cxx 12252
"transformey", //483 kifltk.cxx 12253
"transformey(décimal x,décimal y): Transforme les coordonnées en Y sur la base de la matrice courante de transformation", //484 kifltk.cxx 12253
"transformedx", //485 kifltk.cxx 12254
"transformedx(décimal x,décimal y): Transforme la distance DX sur la base de la matrice courante de transformation", //486 kifltk.cxx 12254
"transformedy", //487 kifltk.cxx 12255
"transformedy(décimal x,décimal y): Transforme la distance DY sur la base de la matrice courante de transformation", //488 kifltk.cxx 12255
"transformesommets", //489 kifltk.cxx 12256
"transformesommets(décimal x,décimal y): ajoute les transformations à la liste des sommets", //490 kifltk.cxx 12256
"débutpoints", //491 kifltk.cxx 12258
"débutpoints(): Commence à accumuler des sommets", //492 kifltk.cxx 12258
"finpoints", //493 kifltk.cxx 12259
"finpoints(): Cesse d'accumuler des sommets", //494 kifltk.cxx 12259
"débutligne", //495 kifltk.cxx 12261
"débutligne(): Commence à dessiner des lignes.", //496 kifltk.cxx 12261
"finligne", //497 kifltk.cxx 12262
"finligne(): Cesse d'ajouter des lignes", //498 kifltk.cxx 12262
"débutboucle", //499 kifltk.cxx 12264
"débutboucle(): Commence à dessiner des séquences de lignes qui se referment.", //500 kifltk.cxx 12264
"finboucle", //501 kifltk.cxx 12265
"finboucle():    Cesse d'accumuler des séquences de lignes qui se referment.", //502 kifltk.cxx 12265
"débutpolygone", //503 kifltk.cxx 12267
"débutpolygone(): Commence à dessiner un polygone plein convexe", //504 kifltk.cxx 12267
"finpolygone", //505 kifltk.cxx 12268
"finpolygone(): Cesse de dessiner un polygone plein convexe", //506 kifltk.cxx 12268
"débutpolygonecomplexe", //507 kifltk.cxx 12270
"débutpolygonecomplexe(): Commence à dessiner un polygone complexe", //508 kifltk.cxx 12270
"trou", //509 kifltk.cxx 12271
"trou(): remplit les trous lors du dessin d'un polygone complexe.", //510 kifltk.cxx 12271
"finpolygonecomplexe", //511 kifltk.cxx 12272
"finpolygonecomplexe(): Cesser de dessiner un polygone complexe", //512 kifltk.cxx 12272
"arc", //513 kifltk.cxx 12275
"arc(nombre x,nombre y,nombre x1, nombre y1, décimal a1, décimal a2): Dessine un arc.\rarc(décimal x,décimal y,décimal rad,décimal a1,décimal a2): Ajoute une série de points en arc de cercle au chemin courant.", //514 kifltk.cxx 12275
"courbe", //515 kifltk.cxx 12276
"courbe(décimal x,décimal y,décimal x1,décimal y1,décimal x1,décimal y2,décimal x3,décimal y3): Ajoute une série de points sur une courbe de Bezier au chemin courant. La fin de la courbe (et deux de ses points) est à la position x,y et x3,y3.", //516 kifltk.cxx 12276
"empileclip", //517 kifltk.cxx 12277
"empileclip(nombre x,nombre y,nombre  largeur, nombre h): Définit une région clippée de coordonnées x,y,largeur,h", //518 kifltk.cxx 12277
"dépileclip", //519 kifltk.cxx 12278
"dépileclip(): Relache une région clippée", //520 kifltk.cxx 12278
"camembert", //521 kifltk.cxx 12280
"camembert(nombre x,nombre y,nombre x1, nombre y1, décimal a1, décimal a2): Dessine un camembert", //522 kifltk.cxx 12280
"point", //523 kifltk.cxx 12281
"point(nombre x,nombre y): Dessine un pixel", //524 kifltk.cxx 12281
"cercle", //525 kifltk.cxx 12282
"cercle(nombre x,nombre y,nombre r,nombre couleur): Dessine un cercle. 'couleur' est optionnelle.", //526 kifltk.cxx 12282
"tailletexte", //527 kifltk.cxx 12284
"tailletexte(chaine l): Renvoie un dictionnaire avec largeur et hauteur comme clef pour donner les largeurs et hauteurs de la chaine en pixels", //528 kifltk.cxx 12284
"dessinetexte", //529 kifltk.cxx 12285
"dessinetexte(chaine l,nombre x,nombre y): Place un texte à la position x,y", //530 kifltk.cxx 12285
"redessine", //531 kifltk.cxx 12286
"redessine(): Redessine la fenêtre", //532 kifltk.cxx 12286
"couleurrvb", //533 kifltk.cxx 12287
"couleurrvb(chaine|nombre couleur)|(nombre r,nombre g,nombre b)|(table couleur): Renvoie soit une table de la décomposition en couleur ou renvoie la valeur de la couleur correspondant à la combinaison RVB fournie", //534 kifltk.cxx 12287
"police", //535 kifltk.cxx 12289
"police(chaine f,nombre sz): Définit le nom de la police et sa taille", //536 kifltk.cxx 12289
"siglissedépose", //537 kifltk.cxx 12290
"siglissedépose(fonction,objet): Initialise la fonction de rappel lors d'un glisser-déposer", //538 kifltk.cxx 12290
"sisouris", //539 kifltk.cxx 12291
"sisouris(nombre action, fonction,objet): Définit le nom de la fonction de rappel pour une action souris", //540 kifltk.cxx 12291
"sitouche", //541 kifltk.cxx 12292
"sitouche(nombre action, fonction,objet): Définit le nom de la fonction de rappel pour une action clavier", //542 kifltk.cxx 12292
"stylecurseur", //543 kifltk.cxx 12293
"stylecurseur(nombre curseurtype, nombre couleur,nombre couleur): Définit la forme du curseur", //544 kifltk.cxx 12293
"cache", //545 kifltk.cxx 12294
"cache(bool h): Cache la fenêtre si hauteur est vrai", //546 kifltk.cxx 12294
"montre", //547 kifltk.cxx 12295
"montre(): Montre la fenêtre", //548 kifltk.cxx 12295
"focus", //549 kifltk.cxx 12296
"focus(): Récupère le focus", //550 kifltk.cxx 12296
"chasse", //551 kifltk.cxx 12297
"chasse(): force à redessiner la fenêtre si celle-ci est dans une tâche", //552 kifltk.cxx 12297
"position", //553 kifltk.cxx 12298
"position()|(nombre x,nombre y): Renvoie la position de la fenêtre ou définit la position de la fenêtre", //554 kifltk.cxx 12298
"taille()|(nombre x,nombre y,nombre  largeur, nombre h): Renvoie la taille de la fenêtre ou définit la taille de la fenêtre", //555 kifltk.cxx 12299
"réveil", //556 kifltk.cxx 12300
"réveil(): Réveille une fenêtre définie dans une tâche", //557 kifltk.cxx 12300
"verrouille", //558 kifltk.cxx 12301
"verrouille(): Verrouille tâche", //559 kifltk.cxx 12301
"déverrouille", //560 kifltk.cxx 12302
"déverrouille(): Déverrouillage tâche", //561 kifltk.cxx 12302
"image(image image,nombre x, nombre y, nombre  largeur, nombre h): Affiche une image", //562 kifltk.cxx 12303
"bitmap(bitmap image,nombre couleur,nombre x, nombre y, nombre w, nombre h): Affiche un bitmap avec une couleur particulière", //563 kifltk.cxx 12304
"taillelabel", //564 kifltk.cxx 12305
"taillelabel(nombre c): définit ou renvoie la taille de la police du label", //565 kifltk.cxx 12305
"policelabel", //566 kifltk.cxx 12306
"policelabel(nombre c): définit ou renvoie la police du label", //567 kifltk.cxx 12306
"couleurlabel", //568 kifltk.cxx 12307
"couleurlabel(nombre c): définit ou renvoie la couleur du label", //569 kifltk.cxx 12307
"typelabel", //570 kifltk.cxx 12308
"typelabel(nombre c): définit ou renvoie le type du label", //571 kifltk.cxx 12308
"label", //572 kifltk.cxx 12309
"label(chaine s): définit ou renvoie le texte du label", //573 kifltk.cxx 12309
"couleursélection", //574 kifltk.cxx 12310
"couleursélection(nombre couleur): Couleur de l'élément sélectionné", //575 kifltk.cxx 12310
"coords", //576 kifltk.cxx 12311
"coords(nombre c): renvoie la table des coordonnées du widget", //577 kifltk.cxx 12311
"créée", //578 kifltk.cxx 12312
"créée(): Renvoie vrai si l'objet a été crée", //579 kifltk.cxx 12312
"copie", //580 kifltk.cxx 12313
"copie(chaine): Copie la chaine dans le presse-papier", //581 kifltk.cxx 12313
"colle", //582 kifltk.cxx 12314
"colle(): Colle chaine depuis le presse-papier", //583 kifltk.cxx 12314
"aligne", //584 kifltk.cxx 12315
"aligne(nombre): définit l'alignement du label", //585 kifltk.cxx 12315
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée un défilement", //586 kifltk.cxx 12317
"retaille", //587 kifltk.cxx 12318
"retaille(objet): objet redimensionnable", //588 kifltk.cxx 12318
"montre(): Montre le widget", //589 kifltk.cxx 12319
"cache(): Cache le widget", //590 kifltk.cxx 12321
"truc", //591 kifltk.cxx 12327
"truc(chaine msg): Ajoute un message de 'truc' au widget", //592 kifltk.cxx 12327
"crée(nombre x,nombre y,nombre  hauteur,nombre  largeur,chaine label): Crée un fcompteur", //593 kifltk.cxx 12335
"bornes", //594 kifltk.cxx 12336
"bornes(décimal x,décimal y): définit les limites du fcompteur", //595 kifltk.cxx 12336
"type", //596 kifltk.cxx 12337
"type(bool normal): si 'vrai' alors fcompteur double sinon fcompteur simple", //597 kifltk.cxx 12337
"pas", //598 kifltk.cxx 12338
"pas(décimal): definit le pas du fcompteur", //599 kifltk.cxx 12338
"paslong", //600 kifltk.cxx 12339
"paslong(décimal): definit le pas long du fcompteur", //601 kifltk.cxx 12339
"lespas", //602 kifltk.cxx 12340
"lespas(décimal court,décimal long): definit les pas court et long du fcompteur", //603 kifltk.cxx 12340
"valeur", //604 kifltk.cxx 12341
"valeur(décimal): définit la valeur du fcompteur ou renvoie sa valeur", //605 kifltk.cxx 12341
"police(nombre s): définit ou renvoie la police du texte", //606 kifltk.cxx 12357
"couleurtexte", //607 kifltk.cxx 12358
"couleurtexte(chaine code|nombre code): Définit la couleur du texte", //608 kifltk.cxx 12358
"crée(nombre x,nombre y,nombre  hauteur,nombre  largeur,chaine label): Crée un fprogression", //609 kifltk.cxx 12360
"minimum", //610 kifltk.cxx 12361
"minimum(décimal x): définit le minimum ou renvoie le minimum", //611 kifltk.cxx 12361
"maximum", //612 kifltk.cxx 12362
"maximum(décimal x): définit le maximum ou renvoie le maximum", //613 kifltk.cxx 12362
"valeur(décimal): définit la valeur de la progression ou renvoie sa valeur", //614 kifltk.cxx 12363
"couleurbarre", //615 kifltk.cxx 12368
"couleurbarre(nombre couleur): Couleur de la barre de progression", //616 kifltk.cxx 12368
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,nombre alignement,bool valeur,glissière,chaine label): Crée une glissière avec ou sans valeur", //617 kifltk.cxx 12374
"bornes(nombre x,nombre y): définit les limites de la glissière", //618 kifltk.cxx 12376
"type(nombre x): Type de la glissière", //619 kifltk.cxx 12377
"typeboite", //620 kifltk.cxx 12378
"typeboite(nombre x): Définit le type de boite autour de la glissière", //621 kifltk.cxx 12378
"aligne(nombre): définit l'alignement de la glissière", //622 kifltk.cxx 12379
"pas(nombre): définit le pas de la glissière", //623 kifltk.cxx 12380
"valeur(nombre): définit la valeur pour glissière ou renvoie sa valeur", //624 kifltk.cxx 12381
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée une boite", //625 kifltk.cxx 12397
"type(chaine|nombre boitetype): Définit le type de la boite", //626 kifltk.cxx 12398
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine type,chaine forme,chaine label): Crée un bouton. Si le type n'est pas fourni, il prend la valeur FL_Régulier par défaut.", //627 kifltk.cxx 12414
"quand", //628 kifltk.cxx 12415
"quand(chaine when1, chaine when2,...): Type de l'événement pour un bouton qui déclenche la fonction de rappel", //629 kifltk.cxx 12415
"raccourci", //630 kifltk.cxx 12416
"raccourci(chaine clef code): Définit un raccourci-clavier correspondant à l'activation du bouton", //631 kifltk.cxx 12416
"couleur", //632 kifltk.cxx 12417
"couleur(chaine code|nombre code): Définit la couleur du bouton", //633 kifltk.cxx 12417
"valeur(): renvoie la valeur du bouton courant", //634 kifltk.cxx 12418
"aligne(nombre): définit l'alignement du label du bouton", //635 kifltk.cxx 12419
"image(image im,chaine label,nombre labelalign): Dessine le bouton avec une image particulière", //636 kifltk.cxx 12420
"bitmap(bitmap im,nombre couleur,chaine label,nombre labelalign): Utilise le bitmap comme image de bouton", //637 kifltk.cxx 12421
"commence(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée un onglet fenêtre et commence l'initialisation", //638 kifltk.cxx 12437
"fin(): fin de la création de l'onglet", //639 kifltk.cxx 12438
"courant", //640 kifltk.cxx 12439
"courant(): Renvoie l'onglet courant actif", //641 kifltk.cxx 12439
"ajoute", //642 kifltk.cxx 12440
"ajoute(fgroupe g): Ajoute dynamiquement un nouvel onglet", //643 kifltk.cxx 12440
"retire", //644 kifltk.cxx 12441
"retire(fgroupe g): Retire un onglet", //645 kifltk.cxx 12441
"commence(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée un fgroupe au sein d'un onglet et commence l'initialisation", //646 kifltk.cxx 12458
"fin(): fin création du fgroupe", //647 kifltk.cxx 12459
"active", //648 kifltk.cxx 12462
"active(): Active cet onglet", //649 kifltk.cxx 12462
"chargejpeg", //650 kifltk.cxx 12529
"chargejpeg(chaine filenom): Charge une image JPEG", //651 kifltk.cxx 12529
"chargegif", //652 kifltk.cxx 12531
"chargegif(chaine filenom): Charge une image GIF", //653 kifltk.cxx 12531
"charge", //654 kifltk.cxx 12533
"charge(table bitmaps,nombre l,nombre h): Charge un bitmap", //655 kifltk.cxx 12533
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée une ftable d'objets, et commence l'ajout", //656 kifltk.cxx 12535
"raz", //657 kifltk.cxx 12536
"raz(): Nettoie la ftable", //658 kifltk.cxx 12536
"ajoute(nombre R,nombre C,chaine v): Ajoute une valeur ligne R, colonne C", //659 kifltk.cxx 12537
"cellule", //660 kifltk.cxx 12538
"cellule(nombre R,nombre C): Renvoie la valeur de la cellule ligne R,colonne C", //661 kifltk.cxx 12538
"cellulealignement", //662 kifltk.cxx 12539
"cellulealignement(alignement): Définit l'alignement interne des cellules", //663 kifltk.cxx 12539
"cellulealignemententêtecol", //664 kifltk.cxx 12540
"cellulealignemententêtecol(alignement): Définit l'alignement interne des cellules d'entête de colonne.", //665 kifltk.cxx 12540
"cellulealignemententêteligne", //666 kifltk.cxx 12541
"cellulealignemententêtecol(alignement): Définit l'alignement interne des cellules d'entête de ligne.", //667 kifltk.cxx 12541
"ligne", //668 kifltk.cxx 12542
"ligne()|(nombre nb): Définit le nombre de lignes", //669 kifltk.cxx 12542
"colonne", //670 kifltk.cxx 12543
"colonne()|(nombre nb): Définit le nombre de colonnes", //671 kifltk.cxx 12543
"hauteurligne", //672 kifltk.cxx 12544
"hauteurligne(nombre hauteur): Définit la hauteur de la ligne en pixel", //673 kifltk.cxx 12544
"largeurcolonne", //674 kifltk.cxx 12545
"largeurcolonne(nombre largeur): Définit là largeur de la colonne en pixel", //675 kifltk.cxx 12545
"taillepolice", //676 kifltk.cxx 12547
"taillepolice(nombre c): définit ou renvoie la taille de la police du texte", //677 kifltk.cxx 12547
"couleurarrièreplan", //678 kifltk.cxx 12548
"couleurarrièreplan(nombre c): définit ou renvoie la couleur de la cellule en arrière-plan", //679 kifltk.cxx 12548
"couleuravantplan", //680 kifltk.cxx 12549
"couleuravantplan(nombre c): définit ou renvoie la couleur de la cellule en avant-plan", //681 kifltk.cxx 12549
"quand(chaine when1, chaine when2,...): Type de l'événement déclenchant la fonction de rappel", //682 kifltk.cxx 12550
"couleursélection(nombre couleur): Couleur pour les éléments sélectionnés", //683 kifltk.cxx 12551
"typeboite(nombre boitetype): type boite", //684 kifltk.cxx 12552
"entêteligne", //685 kifltk.cxx 12553
"entêteligne(nombre pos,chaine label): Définit le label de l'entête de la ligne à la ligne pos", //686 kifltk.cxx 12553
"entêtecolonne", //687 kifltk.cxx 12554
"entêtecolonne(nombre pos,chaine label): Définit le label de l'entête de la colonne à la colonne pos", //688 kifltk.cxx 12554
"hauteurentêteligne", //689 kifltk.cxx 12555
"hauteurentêteligne(nombre hauteur): la taille en pixel de l'entête de la ligne", //690 kifltk.cxx 12555
"largeurentêtecolonne", //691 kifltk.cxx 12556
"largeurentêtecolonne(nombre largeur): la taille en pixel de l'entête de la colonne", //692 kifltk.cxx 12556
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,booléen multiline,chaine label): Crée une fentrée", //693 kifltk.cxx 12574
"valeur()|(chaine v): renvoie le buffer de fentrée ou définit le buffer initial", //694 kifltk.cxx 12575
"insère", //695 kifltk.cxx 12576
"insère(chaine s,nombre p): insère s à la position p dans la fentrée", //696 kifltk.cxx 12576
"sélection", //697 kifltk.cxx 12577
"sélection(): renvoie le texte sélectionné dans la fentrée", //698 kifltk.cxx 12577
"couleur(chaine c|nombre c): définit ou renvoie la couleur du texte", //699 kifltk.cxx 12578
"mot", //700 kifltk.cxx 12579
"mot(nombre pos): Renvoie le mot à la position pos", //701 kifltk.cxx 12579
"police(chaine s): définit ou renvoie la police du texte", //702 kifltk.cxx 12580
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée un éditeur", //703 kifltk.cxx 12600
"sélection(): renvoie le texte sélectionné dans l'éditeur ou les coordonnées de la sélection selon la variable de réception", //704 kifltk.cxx 12601
"valeur()|(chaine v): renvoie le texte dans l'éditeur ou initialise l'éditeur", //705 kifltk.cxx 12602
"police(chaine s)|(): définit ou renvoie la police du texte", //706 kifltk.cxx 12604
"taillepolice(nombre c)|(): définit ou renvoie la taille de la police du texte", //707 kifltk.cxx 12605
"metstyle", //708 kifltk.cxx 12606
"metstyle(nombre start,nombre fin,chaine style): définit le style d'une portion de texte", //709 kifltk.cxx 12606
"charge(chaine f): Charge le fichier dans l'éditeur", //710 kifltk.cxx 12607
"sauvegarde", //711 kifltk.cxx 12608
"sauvegarde(chaine f): Sauvegarde le contenu de l'éditeur dans le fichier f", //712 kifltk.cxx 12608
"remplace", //713 kifltk.cxx 12609
"remplace(chaine s,chaine sub,nombre i,bool casse): remplace s avec sub selon la casse à partir de i", //714 kifltk.cxx 12609
"remplacetout", //715 kifltk.cxx 12610
"remplacetout(chaine s,chaine sub,bool casse): remplace toutes les occurrences de s avec sub selon la casse", //716 kifltk.cxx 12610
"cherche", //717 kifltk.cxx 12611
"cherche(chaine s,nombre i,bool casse): Cherche la position de la sous-chaine s à partir de i", //718 kifltk.cxx 12611
"rcherche", //719 kifltk.cxx 12612
"rcherche(chaine s,nombre i,bool casse): Cherche la position de la sous-chaine s à partir de i en arrière", //720 kifltk.cxx 12612
"surbrille", //721 kifltk.cxx 12613
"surbrille(nombre start,nombre fin)|(): Met une portion de texte en  sur-brillance. Renvoie vrai si le texte est sur-ligné ou le texte en  sur-brillance", //722 kifltk.cxx 12613
"désurbrille", //723 kifltk.cxx 12614
"désurbrille(): Retire la  sur-brillance d'une portion de texte", //724 kifltk.cxx 12614
"stylecurseur(nombre curseurforme): Définit la forme du curseur", //725 kifltk.cxx 12615
"sisouris(nombre action,fonction f,objet o): fonction de rappel après un clic", //726 kifltk.cxx 12616
"sitouche(nombre action,fonction f,objet o): fonction de rappel quand une touche est pressée", //727 kifltk.cxx 12617
"sidéfilementvertical", //728 kifltk.cxx 12618
"sidéfilementvertical(fonction f,objet o): fonction de rappel pour un défilement vertical", //729 kifltk.cxx 12618
"sidéfilementhorizontal", //730 kifltk.cxx 12619
"sidéfilementhorizontal(fonction f,objet o): fonction de rappel pour un défilement horizontal", //731 kifltk.cxx 12619
"lestyle", //732 kifltk.cxx 12620
"lestyle(nombre start,nombre fin): renvoie une table de style pour chaque caractère de la section de texte", //733 kifltk.cxx 12620
"ajoutestyle", //734 kifltk.cxx 12621
"ajoutestyle(dictionaire style): Initialise les styles", //735 kifltk.cxx 12621
"détruit", //736 kifltk.cxx 12623
"détruit(): Détruit le texte sélectionné depuis l'éditeur", //737 kifltk.cxx 12623
"coupe", //738 kifltk.cxx 12624
"coupe(): Coupe le texte sélectionné depuis l'éditeur dans le presse-papier", //739 kifltk.cxx 12624
"copie(chaine s): Copie le texte sélectionné depuis l'éditeur dans le presse-papier (chaine s est optionnel)", //740 kifltk.cxx 12625
"colle(): Colle le texte sélectionné depuis le presse-papier", //741 kifltk.cxx 12626
"sélectionne", //742 kifltk.cxx 12629
"sélectionne(): Renvoie la chaine sélectionnée.\rsélectionne(int pospremier,int posdernier): Sélectionne la chaine entre pospremier et posdernier", //743 kifltk.cxx 12629
"ligne(nombre l): renvoie la position de la ligne courante du chariot ou renvoie la ligne correspondant à la position", //744 kifltk.cxx 12630
"bornesligne", //745 kifltk.cxx 12631
"bornesligne(nombre l): renvoie le début et la fin de la position de la ligne courante du chariot ou renvoie les limites de la ligne  correspondant à une position", //746 kifltk.cxx 12631
"borneslignecarac", //747 kifltk.cxx 12632
"borneslignecarac(): renvoie le début et la fin de la position de la ligne courante du chariot ou renvoie les limites de la ligne  correspondant à une position en nombre de caractères", //748 kifltk.cxx 12632
"curseur(nombre l): renvoie la position du curseur courant du chariot ou définit la position du curseur", //749 kifltk.cxx 12633
"curseurcarac", //750 kifltk.cxx 12634
"curseurcarac(): renvoie la position du curseur courant du chariot en nombre de caractères", //751 kifltk.cxx 12634
"valigne", //752 kifltk.cxx 12635
"valigne(nombre l,bool surbrille): Va à la ligne l. Si vrai, met la ligne en  sur-brillance", //753 kifltk.cxx 12635
"annote", //754 kifltk.cxx 12636
"annote(chaine s|table v,chaine style,bool casse): Annote chaque occurence de s avec style", //755 kifltk.cxx 12636
"sautautomatique", //756 kifltk.cxx 12638
"sautautomatique(bool): Définit un mode coupure à la ligne automatique", //757 kifltk.cxx 12638
"annoteregexip", //758 kifltk.cxx 12639
"annoteregexip(chaine reg,chaine style): Annote chaque chaine correspondant à reg avec style", //759 kifltk.cxx 12639
"accumule", //760 kifltk.cxx 12655
"accumule(chaine s): Ajoute s à la fin de l'entrée", //761 kifltk.cxx 12655
"positionoctet", //762 kifltk.cxx 12656
"positionoctet(nombre i): Convertit une position en octet en une position en caractère", //763 kifltk.cxx 12656
"positioncarac", //764 kifltk.cxx 12657
"positioncarac(nombre i): Convertit une position en caractère en une position en octet", //765 kifltk.cxx 12657
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,booléen multiline,chaine label): Crée une fsortie", //766 kifltk.cxx 12660
"valeur(chaine s): définit le buffer de fsortie", //767 kifltk.cxx 12661
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée un choix", //768 kifltk.cxx 12683
"valeur(nombre s): définit la valeur d'initialisation du choix", //769 kifltk.cxx 12684
"menu(table s): Initialise le menu", //770 kifltk.cxx 12686
"crée(chaine intialdirectory,chaine filter,bool directory,chaine label): Ouvre un fparcourirfichiers, en mode répertoire si directory est vrai", //771 kifltk.cxx 12704
"valeur(): Renvoie le fichier sélectionné", //772 kifltk.cxx 12705
"ferme(): Ferme le widget fparcourirfichiers", //773 kifltk.cxx 12706
"ok", //774 kifltk.cxx 12707
"ok(): renvoie vrai si ok a été sélectionné", //775 kifltk.cxx 12707
"ouvre", //776 kifltk.cxx 12710
"ouvre(): Ouvre un fichier via une fenêtre mac", //777 kifltk.cxx 12710
"charge(chaine): Charge un fichier via une fenêtre mac", //778 kifltk.cxx 12711
"crée(x,y,largeur,hauteur,label): Crée un widget fparcourir", //779 kifltk.cxx 12714
"ajoute(label): Ajoute une chaine au widget fparcourir", //780 kifltk.cxx 12715
"charge(filenom): Charge un fichier dans le widget fparcourir", //781 kifltk.cxx 12716
"insère(l,label): Insère un label avant la ligne l", //782 kifltk.cxx 12717
"raz(): Vide le widget fparcourir de toutes ses valeurs", //783 kifltk.cxx 12718
"valeur(): renvoie la valeur courante sélectionnée", //784 kifltk.cxx 12719
"sélectionne(): Renvoie la chaine sélectionnée.\rsélectionne(int i): Renvoie la chaine en position i", //785 kifltk.cxx 12720
"désélectionne", //786 kifltk.cxx 12721
"désélectionne(): Désélectionne tous les noeuds\rdésélectionne(nombre i): Désélectionne le noeud i", //787 kifltk.cxx 12721
"séparateurcolonne", //788 kifltk.cxx 12722
"séparateurcolonne(): Renvoie le caractère de séparation de colonnes.\rséparateurcolonne(chaine): Définit le caractère de séparation de colonnes", //789 kifltk.cxx 12722
"formatcarac", //790 kifltk.cxx 12723
"formatcarac(): Renvoie le caractère de formatage.\rformatcarac(chaine): Définit le caractère de formatage", //791 kifltk.cxx 12723
"label(): Renvoie le label du noeud.", //792 kifltk.cxx 12742
"profondeur", //793 kifltk.cxx 12743
"profondeur(): Renvoie la profondeur du noeud.", //794 kifltk.cxx 12743
"valeur(objet): Associe le noeud avec une valeur ou renvoie cette valeur.", //795 kifltk.cxx 12744
"nettoie", //796 kifltk.cxx 12745
"nettoie(): Retire l'objet associé.", //797 kifltk.cxx 12745
"couleurarrièreplan", //798 kifltk.cxx 12746
"couleurarrièreplan(nombre c): Définit ou renvoie la couleur de l'arrière-plan du noeud.", //799 kifltk.cxx 12746
"couleuravantplan", //800 kifltk.cxx 12747
"couleuravantplan(nombre c): Définit ou renvoie la couleur de l'avant-plan du noeud.", //801 kifltk.cxx 12747
"police(nombre c): Définit ou renvoie la police du noeud.", //802 kifltk.cxx 12748
"taillepolice(nombre c): Définit ou renvoie la taille de la police du noeud.", //803 kifltk.cxx 12749
"estfermé", //804 kifltk.cxx 12750
"estfermé(): Renvoie vrai si l'élément est fermé.", //805 kifltk.cxx 12750
"estouvert", //806 kifltk.cxx 12751
"estouvert(): Renvoie vrai si l'élément est ouvert.", //807 kifltk.cxx 12751
"estracine", //808 kifltk.cxx 12752
"estracine(): Renvoie vrai si l'élément est une racine.", //809 kifltk.cxx 12752
"estsélectionné", //810 kifltk.cxx 12753
"estsélectionné(): Renvoie vrai si l'élément est sélectionné.", //811 kifltk.cxx 12753
"estactif", //812 kifltk.cxx 12754
"estactif(): Renvoie vrai si l'élément est actif.", //813 kifltk.cxx 12754
"parent", //814 kifltk.cxx 12755
"parent(): Renvoie le dernier élément.", //815 kifltk.cxx 12755
"suivant", //816 kifltk.cxx 12756
"suivant(): Renvoie l'élément suivant.", //817 kifltk.cxx 12756
"précédent", //818 kifltk.cxx 12757
"précédent(): Renvoie l'élément précédent.", //819 kifltk.cxx 12757
"enfant", //820 kifltk.cxx 12758
"enfant(nombre i): Renvoie l'élément enfant à la position i.", //821 kifltk.cxx 12758
"enfants", //822 kifltk.cxx 12759
"enfants(): Renvoie le nombre d'enfants.", //823 kifltk.cxx 12759
"active(): Active l'élément courant.", //824 kifltk.cxx 12760
"désactive", //825 kifltk.cxx 12761
"désactive(): Désactive l'élément courant.", //826 kifltk.cxx 12761
"crée(nombre x,nombre y,nombre  hauteur,nombre  largeur,chaine label): Crée un arbre", //827 kifltk.cxx 12763
"raz(): Détruit les noeuds de l'arbre", //828 kifltk.cxx 12764
"ajoute(chaine path): Ajoute un noeud.\rajoute(fnoeudarbre e,chaine n): Ajoute un noeud après e.", //829 kifltk.cxx 12765
"estfermé(chaine path): Renvoie vrai si l'élément est fermé.\restfermé(fnoeudarbre e): Renvoie vrai si l'élément est fermé.", //830 kifltk.cxx 12766
"ferme(chaine path): Ferme l'élément.\rferme(fnoeudarbre e): Ferme l'élément.", //831 kifltk.cxx 12767
"ouvre(chaine path): Ouvre l'élément.\rouvre(fnoeudarbre e): Ouvre l'élément.", //832 kifltk.cxx 12768
"insère(fnoeudarbre e,chaine label,nombre pos): Insère un élément après e avec label à la position pos dans la liste des enfants.", //833 kifltk.cxx 12769
"retire(fnoeudarbre e): Retire l'élément e dans l'arbre.", //834 kifltk.cxx 12770
"insèreavant", //835 kifltk.cxx 12771
"insèreavant(fnoeudarbre e,chaine label): Insère un élément au-dessus de e avec label.", //836 kifltk.cxx 12771
"couleurconnecteur", //837 kifltk.cxx 12772
"couleurconnecteur(nombre c): Définit ou renvoie la couleur du connecteur.", //838 kifltk.cxx 12772
"styleconnecteur", //839 kifltk.cxx 12773
"styleconnecteur(nombre s): Définit ou renvoie le style du connecteur.", //840 kifltk.cxx 12773
"largeurconnecteur", //841 kifltk.cxx 12774
"largeurconnecteur(nombre s): Définit ou renvoie là largeur du connecteur.", //842 kifltk.cxx 12774
"ordretri", //843 kifltk.cxx 12775
"ordretri(nombre s): Définit ou renvoie l'ordre du tri.", //844 kifltk.cxx 12775
"modesélection", //845 kifltk.cxx 12776
"modesélection(nombre s): Définit ou renvoie le mode de sélection.", //846 kifltk.cxx 12776
"couleurnoeudarrièreplan", //847 kifltk.cxx 12778
"couleurnoeudarrièreplan(nombre c): Définit ou renvoie la couleur d'arrière-plan du noeud.", //848 kifltk.cxx 12778
"couleurnoeudavantplan", //849 kifltk.cxx 12779
"couleurnoeudavantplan(nombre c): Définit ou renvoie la couleur d'avant-plan du noeud.", //850 kifltk.cxx 12779
"policenoeud", //851 kifltk.cxx 12780
"policenoeud(nombre c): Définit ou renvoie la police du noeud.", //852 kifltk.cxx 12780
"taillenoeud", //853 kifltk.cxx 12781
"taillenoeud(nombre c): Définit ou renvoie la taille de la police du noeud.", //854 kifltk.cxx 12781
"margegauche", //855 kifltk.cxx 12783
"margegauche(nombre s):Définit ou récupère l'espace (en pixels) qui doit apparaitre entre la bordure gauche du widget et le contenu de l'arbre.", //856 kifltk.cxx 12783
"margehaut", //857 kifltk.cxx 12784
"margehaut(nombre s): Définit ou récupère l'espace (en pixels) qui doit apparaitre entre la bordure du haut du widget et le contenu de l'arbre.", //858 kifltk.cxx 12784
"racine", //859 kifltk.cxx 12786
"racine(): Renvoie la racine.", //860 kifltk.cxx 12786
"labelracine", //861 kifltk.cxx 12787
"labelracine(chaine r): Définit le label de la racine.", //862 kifltk.cxx 12787
"cliqué", //863 kifltk.cxx 12788
"cliqué(): Renvoie l'élément qui a été cliqué.", //864 kifltk.cxx 12788
"premier", //865 kifltk.cxx 12789
"premier(): Renvoie le premier élément.", //866 kifltk.cxx 12789
"cherche(chaine path): Renvoie l'élément correspondant au chemin path.", //867 kifltk.cxx 12790
"dernier", //868 kifltk.cxx 12791
"dernier(): Renvoie le dernier élément.", //869 kifltk.cxx 12791
"suivant(fnoeudarbre e): Renvoie l'élément suivant après e.", //870 kifltk.cxx 12792
"précédent(fnoeudarbre e): Renvoie l'élément précédent avant e.", //871 kifltk.cxx 12793
""};


Exported const char* KIFLTKSDEBUGGERTRINGS[]={
"afficheligne", //0 kifeditor.cxx 44
"tantque", //1 kifeditor.cxx 45
"pour", //2 kifeditor.cxx 46
"parmi", //3 kifeditor.cxx 47
"si", //4 kifeditor.cxx 48
"sinon", //5 kifeditor.cxx 49
"sinonsi", //6 kifeditor.cxx 50
"//Variables locales:\n", //7 kifeditor.cxx 1019
"//Variables principales:\n", //8 kifeditor.cxx 1031
"//Variables de classe:", //9 kifeditor.cxx 1044
"//Variables de fonction", //10 kifeditor.cxx 1056
"Terminé", //11 kifeditor.cxx 1074
"Cherche", //12 kifeditor.cxx 1368
"Chaine:", //13 kifeditor.cxx 1369
"Suivant", //14 kifeditor.cxx 1370
"Annule", //15 kifeditor.cxx 1371
"Casse", //16 kifeditor.cxx 1372
"Remplace", //17 kifeditor.cxx 1411
"Cherche:", //18 kifeditor.cxx 1412
"Remplace:", //19 kifeditor.cxx 1413
"Remplace tout", //20 kifeditor.cxx 1415
"Remplace suivant", //21 kifeditor.cxx 1416
"Suggérer", //22 kifeditor.cxx 1525
"Sansnom", //23 kifeditor.cxx 1553
"Création d'un nouveau fichier:", //24 kifeditor.cxx 1559
"Ligne", //25 kifeditor.cxx 1659
"Taille indentation", //26 kifeditor.cxx 1670
"Deux exécutions:\n\tL'un en tant que tâche principale qui NE PEUT PAS être arrêtée.\n\tL'autre en tant que sous-tâche qui PEUT être arrêtée avec STOP.", //27 kifeditor.cxx 2289
"Fichier/Sauvegarder", //28 kifeditor.cxx 2292
"Fichier/Sauvegarder sous", //29 kifeditor.cxx 2293
"Edition/Annuler", //30 kifeditor.cxx 2295
"Edition/Rétablir", //31 kifeditor.cxx 2296
"Edition/Lister", //32 kifeditor.cxx 2301
"Edition/Correspondance", //33 kifeditor.cxx 2302
"Edition/Indenter", //34 kifeditor.cxx 2303
"Edition/(Dé)commente", //35 kifeditor.cxx 2304
"Edition/Aller ligne", //36 kifeditor.cxx 2305
"Edition/Taille indentation", //37 kifeditor.cxx 2306
"Edition/Cherche", //38 kifeditor.cxx 2307
"Edition/Remplace", //39 kifeditor.cxx 2308
"Edition/Suivant", //40 kifeditor.cxx 2309
"Edition/Police++", //41 kifeditor.cxx 2310
"Edition/Police--", //42 kifeditor.cxx 2311
"Edition/Couper", //43 kifeditor.cxx 2312
"Edition/Copier", //44 kifeditor.cxx 2313
"Edition/Coller", //45 kifeditor.cxx 2314
"Commande/Exécute (tâche principale)", //46 kifeditor.cxx 2315
"Commande/Exécute (sous-tâche)", //47 kifeditor.cxx 2316
"Commande/Arrêt", //48 kifeditor.cxx 2317
"Commande/Débogue", //49 kifeditor.cxx 2318
"Commande/Point d'arrêt (PA)", //50 kifeditor.cxx 2319
"Commande/Retire tous les PA", //51 kifeditor.cxx 2320
"Choisir", //52 kifeditor.cxx 2382
"Pas de description", //53 kifeditor.cxx 2403
"Fichier/Nouveau", //54 kifeditor.cxx 2622
"Fichier/Ouvrir", //55 kifeditor.cxx 2623
"Fichier/Quitter", //56 kifeditor.cxx 2625
"Edit/Miroir", //57 kifeditor.cxx 2633
"Edition/Editeur", //58 kifeditor.cxx 2635
"Edition/Nettoyer", //59 kifeditor.cxx 2636
"*.kif", //60 kifeditor.cxx 2653
"Charger votre fichier", //61 kifeditor.cxx 2653
"Sauvegarder votre fichier", //62 kifeditor.cxx 2671
"nettoyé\n", //63 kifeditor.cxx 2711
"aucun", //64 kifeditor.cxx 2887
"Mauvaise instruction", //65 kifeditor.cxx 3050
"\nenregistré\n", //66 kifeditor.cxx 3055
"Fichier:'", //67 kifeditor.cxx 3155
"' Non sauvegardé... Voulez-vous vraiment quitter?", //68 kifeditor.cxx 3155
"Voulez-vous quitter?", //69 kifeditor.cxx 3164
"Débogueur", //70 kifeditor.cxx 3364
"Affichage court", //71 kifeditor.cxx 3366
"Locales", //72 kifeditor.cxx 3367
"Tous", //73 kifeditor.cxx 3368
"Pile", //74 kifeditor.cxx 3369
"Nom fichier", //75 kifeditor.cxx 3370
"Aller", //76 kifeditor.cxx 3372
"Dedans", //77 kifeditor.cxx 3373
"Dehors", //78 kifeditor.cxx 3374
"Arrêt", //79 kifeditor.cxx 3375
"Fin", //80 kifeditor.cxx 3376
"Sauvegarder votre fichier comme", //81 kifeditor.cxx 3541
"Sauvegarder cette fenêtre d'abord", //82 kifeditor.cxx 3767
"est", //83 kifeditor.cxx 4062
"faire", //84 kifeditor.cxx 4064
"varprédicat", //85 kifeditor.cxx 4065
"prédicat", //86 kifeditor.cxx 4066
"terme", //87 kifeditor.cxx 4068
"transducteur", //88 kifeditor.cxx 4070
"dépendance", //89 kifeditor.cxx 4072
"noeud", //90 kifeditor.cxx 4073
"_fonctionramassemiette", //91 kifeditor.cxx 4074
"grammaire", //92 kifeditor.cxx 4075
"autrement", //93 kifeditor.cxx 4076
"_tailleramassemiette", //94 kifeditor.cxx 4077
"fonctiondebug", //95 kifeditor.cxx 4078
"tâcheparent", //96 kifeditor.cxx 4079
"matricen", //97 kifeditor.cxx 4080
"donnéesdebug", //98 kifeditor.cxx 4081
"échaine", //99 kifeditor.cxx 4082
"énombre", //100 kifeditor.cxx 4083
"édécimal", //101 kifeditor.cxx 4084
"créeserveur", //102 kifeditor.cxx 4085
"infopile", //103 kifeditor.cxx 4086
"attendsquefaux", //104 kifeditor.cxx 4087
"env", //105 kifeditor.cxx 4088
"exécutetâche", //106 kifeditor.cxx 4089
"synchrone", //107 kifeditor.cxx 4090
"fparcourirfichiers", //108 kifeditor.cxx 4091
"polynomiale", //109 kifeditor.cxx 4092
"afficheligneerr", //110 kifeditor.cxx 4093
"affichejligneerr", //111 kifeditor.cxx 4094
"attendsfinjoindre", //112 kifeditor.cxx 4095
"mavariable", //113 kifeditor.cxx 4096
"margegauche", //114 kifeditor.cxx 4097
"exclusive", //115 kifeditor.cxx 4098
"joindre", //116 kifeditor.cxx 4099
"fnoeudarbre", //117 kifeditor.cxx 4100
"protégée", //118 kifeditor.cxx 4101
"ritérateur", //119 kifeditor.cxx 4103
"fraction", //120 kifeditor.cxx 4104
"afficheerr", //121 kifeditor.cxx 4105
"affichejerr", //122 kifeditor.cxx 4106
"fonction", //123 kifeditor.cxx 4107
"séquence", //124 kifeditor.cxx 4109
"idtâche", //125 kifeditor.cxx 4110
"cheminfichier", //126 kifeditor.cxx 4111
"continue", //127 kifeditor.cxx 4112
"itérateur", //128 kifeditor.cxx 4113
"instance", //129 kifeditor.cxx 4114
"affichejligne", //130 kifeditor.cxx 4116
"adresse", //131 kifeditor.cxx 4117
"booléen", //132 kifeditor.cxx 4118
"autoexécute", //133 kifeditor.cxx 4119
"privée", //134 kifeditor.cxx 4120
"fparcourir", //135 kifeditor.cxx 4121
"choix", //136 kifeditor.cxx 4122
"fsortie", //137 kifeditor.cxx 4123
"enattente", //138 kifeditor.cxx 4124
"_version", //139 kifeditor.cxx 4125
"matrice", //140 kifeditor.cxx 4126
"rouey", //141 kifeditor.cxx 4127
"rouex", //142 kifeditor.cxx 4128
"distant", //143 kifeditor.cxx 4129
"minute", //144 kifeditor.cxx 4130
"_ramassemiette", //145 kifeditor.cxx 4131
"connecteur", //146 kifeditor.cxx 4132
"son", //147 kifeditor.cxx 4133
"mp3", //148 kifeditor.cxx 4134
"bouton", //149 kifeditor.cxx 4135
"défilement", //150 kifeditor.cxx 4136
"xmldoc", //151 kifeditor.cxx 4138
"éditeur", //152 kifeditor.cxx 4139
"chargelocalement", //153 kifeditor.cxx 4140
"fentrée", //154 kifeditor.cxx 4141
"glissière", //155 kifeditor.cxx 4142
"fgroupe", //156 kifeditor.cxx 4143
"image", //157 kifeditor.cxx 4144
"chasse", //158 kifeditor.cxx 4146
"chainebrute", //159 kifeditor.cxx 4147
"chaine", //160 kifeditor.cxx 4148
"uchaine", //161 kifeditor.cxx 4149
"tuple", //162 kifeditor.cxx 4150
"commune", //163 kifeditor.cxx 4151
"aléatoire", //164 kifeditor.cxx 4152
"bas", //165 kifeditor.cxx 4153
"table", //166 kifeditor.cxx 4154
"renvoie", //167 kifeditor.cxx 4155
"retourne", //168 kifeditor.cxx 4156
"tâche", //169 kifeditor.cxx 4157
"fenêtre", //170 kifeditor.cxx 4158
"fcompteur", //171 kifeditor.cxx 4159
"fprogression", //172 kifeditor.cxx 4160
"bitmap", //173 kifeditor.cxx 4161
"seconde", //174 kifeditor.cxx 4162
"stricte", //175 kifeditor.cxx 4163
"affiche", //176 kifeditor.cxx 4164
"affichej", //177 kifeditor.cxx 4165
"capte", //178 kifeditor.cxx 4166
"tableau", //179 kifeditor.cxx 4167
"faux", //180 kifeditor.cxx 4168
"propres", //181 kifeditor.cxx 4169
"onglet", //182 kifeditor.cxx 4170
"classe", //183 kifeditor.cxx 4171
"extension", //184 kifeditor.cxx 4172
"début", //185 kifeditor.cxx 4174
"entréestandard", //186 kifeditor.cxx 4175
"non", //187 kifeditor.cxx 4176
"absent", //188 kifeditor.cxx 4177
"de", //189 kifeditor.cxx 4178
"pasdans", //190 kifeditor.cxx 4179
"pause", //191 kifeditor.cxx 4180
"const", //192 kifeditor.cxx 4181
"suspend", //193 kifeditor.cxx 4182
"raz", //194 kifeditor.cxx 4183
"intervalle", //195 kifeditor.cxx 4184
"arrête", //196 kifeditor.cxx 4185
"lève", //197 kifeditor.cxx 4186
"décimal", //198 kifeditor.cxx 4187
"farbre", //199 kifeditor.cxx 4188
"bool", //200 kifeditor.cxx 4190
"omni", //201 kifeditor.cxx 4191
"auto", //202 kifeditor.cxx 4192
"éval", //203 kifeditor.cxx 4193
"vrai", //204 kifeditor.cxx 4194
"échec", //205 kifeditor.cxx 4195
"libère", //206 kifeditor.cxx 4196
"ça", //207 kifeditor.cxx 4197
"touche", //208 kifeditor.cxx 4198
"long", //209 kifeditor.cxx 4199
"chrono", //210 kifeditor.cxx 4200
"appel", //211 kifeditor.cxx 4201
"liste", //212 kifeditor.cxx 4202
"port", //213 kifeditor.cxx 4203
"_sortie", //214 kifeditor.cxx 4204
"ffichier", //215 kifeditor.cxx 4205
"fichier", //216 kifeditor.cxx 4206
"hachage", //217 kifeditor.cxx 4208
"bits", //218 kifeditor.cxx 4209
"vide", //219 kifeditor.cxx 4210
"nulle", //220 kifeditor.cxx 4211
"tue", //221 kifeditor.cxx 4212
"octet", //222 kifeditor.cxx 4213
"bouclec", //223 kifeditor.cxx 4214
"boucleu", //224 kifeditor.cxx 4215
"boucleb", //225 kifeditor.cxx 4216
"bouclen", //226 kifeditor.cxx 4217
"boucled", //227 kifeditor.cxx 4218
"nombre", //228 kifeditor.cxx 4219
"estun", //229 kifeditor.cxx 4220
"importe", //230 kifeditor.cxx 4221
"haut", //231 kifeditor.cxx 4223
"boite", //232 kifeditor.cxx 4225
"tente", //233 kifeditor.cxx 4226
"bit", //234 kifeditor.cxx 4227
"arbre", //235 kifeditor.cxx 4228
"où", //236 kifeditor.cxx 4231
"répète", //237 kifeditor.cxx 4232
"cycle", //238 kifeditor.cxx 4233
"réplique", //239 kifeditor.cxx 4234
"filtre", //240 kifeditor.cxx 4235
"prend", //241 kifeditor.cxx 4236
"prendJusquà", //242 kifeditor.cxx 4237
"fusionneAvec", //243 kifeditor.cxx 4238
"fusionne", //244 kifeditor.cxx 4239
"flip", //245 kifeditor.cxx 4240
"abandonne", //246 kifeditor.cxx 4241
"abandonneJusquà", //247 kifeditor.cxx 4242
"plied", //248 kifeditor.cxx 4243
"plieg", //249 kifeditor.cxx 4244
"plied1", //250 kifeditor.cxx 4245
"plieg1", //251 kifeditor.cxx 4246
"scand", //252 kifeditor.cxx 4247
"scang", //253 kifeditor.cxx 4248
"scand1", //254 kifeditor.cxx 4249
"scang1", //255 kifeditor.cxx 4250
"sac", //256 kifeditor.cxx 4252
"sacchaines", //257 kifeditor.cxx 4253
"sacuchaines", //258 kifeditor.cxx 4254
"sacnombres", //259 kifeditor.cxx 4255
"sacdécimaux", //260 kifeditor.cxx 4256
"tabledécimaux", //261 kifeditor.cxx 4258
"tablechaines", //262 kifeditor.cxx 4259
"tableuchaines", //263 kifeditor.cxx 4260
"tablenombres", //264 kifeditor.cxx 4261
"tableoctets", //265 kifeditor.cxx 4262
"dictionnaire", //266 kifeditor.cxx 4264
"dicoc", //267 kifeditor.cxx 4266
"dicod", //268 kifeditor.cxx 4267
"dicon", //269 kifeditor.cxx 4268
"dicocd", //270 kifeditor.cxx 4270
"dicocn", //271 kifeditor.cxx 4271
"dicocc", //272 kifeditor.cxx 4272
"dicocd", //273 kifeditor.cxx 4274
"dicocn", //274 kifeditor.cxx 4275
"dicocc", //275 kifeditor.cxx 4276
"diconc", //276 kifeditor.cxx 4278
"dicond", //277 kifeditor.cxx 4279
"diconn", //278 kifeditor.cxx 4280
"dicodc", //279 kifeditor.cxx 4282
"dicodd", //280 kifeditor.cxx 4283
"dicodn", //281 kifeditor.cxx 4284
"dicotrié", //282 kifeditor.cxx 4286
"dicotriéc", //283 kifeditor.cxx 4288
"dicotriéd", //284 kifeditor.cxx 4289
"dicotrién", //285 kifeditor.cxx 4290
"dicotriécd", //286 kifeditor.cxx 4292
"dicotriécn", //287 kifeditor.cxx 4293
"dicotriécc", //288 kifeditor.cxx 4294
"dicotriécd", //289 kifeditor.cxx 4296
"dicotriécn", //290 kifeditor.cxx 4297
"dicotriécc", //291 kifeditor.cxx 4298
"dicotriénc", //292 kifeditor.cxx 4300
"dicotriénd", //293 kifeditor.cxx 4301
"dicotriénn", //294 kifeditor.cxx 4302
"dicotriédc", //295 kifeditor.cxx 4304
"dicotriédd", //296 kifeditor.cxx 4305
"dicotriédn", //297 kifeditor.cxx 4306
"dicopremier", //298 kifeditor.cxx 4308
"dicopremierc", //299 kifeditor.cxx 4310
"dicopremierd", //300 kifeditor.cxx 4311
"dicopremiern", //301 kifeditor.cxx 4312
"dicopremiercd", //302 kifeditor.cxx 4314
"dicopremiercn", //303 kifeditor.cxx 4315
"dicopremiercc", //304 kifeditor.cxx 4316
"dicopremiercd", //305 kifeditor.cxx 4318
"dicopremiercn", //306 kifeditor.cxx 4319
"dicopremiercc", //307 kifeditor.cxx 4320
"dicopremiernc", //308 kifeditor.cxx 4322
"dicopremiernd", //309 kifeditor.cxx 4323
"dicopremiernn", //310 kifeditor.cxx 4324
"dicopremierdc", //311 kifeditor.cxx 4326
"dicopremierdd", //312 kifeditor.cxx 4327
"dicopremierdn", //313 kifeditor.cxx 4328
"xml", //314 kifeditor.cxx 4330
"cas", //315 kifeditor.cxx 4332
"soit", //316 kifeditor.cxx 4333
"dans", //317 kifeditor.cxx 4334
"dedans", //318 kifeditor.cxx 4335
"sur", //319 kifeditor.cxx 4336
"défaut", //320 kifeditor.cxx 4337
"curl", //321 kifeditor.cxx 4338
"et", //322 kifeditor.cxx 4339
"ou", //323 kifeditor.cxx 4340
"avec", //324 kifeditor.cxx 4343
"_finl", //325 kifeditor.cxx 4344
"a_premier", //326 kifeditor.cxx 4345
"a_dernier", //327 kifeditor.cxx 4346
"a_préfixe", //328 kifeditor.cxx 4347
"a_change", //329 kifeditor.cxx 4348
"a_insère", //330 kifeditor.cxx 4349
"a_détruit", //331 kifeditor.cxx 4350
"a_commute", //332 kifeditor.cxx 4351
"a_sanscasse", //333 kifeditor.cxx 4352
"a_surface", //334 kifeditor.cxx 4353
"a_tout", //335 kifeditor.cxx 4354
"a_découpe", //336 kifeditor.cxx 4355
"a_saute", //337 kifeditor.cxx 4356
"a_trace", //338 kifeditor.cxx 4357
"préd_aucun", //339 kifeditor.cxx 4358
"préd_poids", //340 kifeditor.cxx 4359
"préd_pilepoids", //341 kifeditor.cxx 4360
"automate", //342 kifeditor.cxx 4361
"dicodd", //343 kifeditor.cxx 4363
"dicopremierdd", //344 kifeditor.cxx 4364
"dicotriédd", //345 kifeditor.cxx 4365
"dicodn", //346 kifeditor.cxx 4366
"dicopremierdn", //347 kifeditor.cxx 4367
"dicotriédn", //348 kifeditor.cxx 4368
"dicodc", //349 kifeditor.cxx 4369
"dicopremierdc", //350 kifeditor.cxx 4370
"dicotriédc", //351 kifeditor.cxx 4371
"dicond", //352 kifeditor.cxx 4372
"dicopremiernd", //353 kifeditor.cxx 4373
"dicotriénd", //354 kifeditor.cxx 4374
"diconn", //355 kifeditor.cxx 4375
"dicopremiernn", //356 kifeditor.cxx 4376
"dicotriénn", //357 kifeditor.cxx 4377
"diconc", //358 kifeditor.cxx 4378
"dicopremiernc", //359 kifeditor.cxx 4379
"dicotriénc", //360 kifeditor.cxx 4380
"dicocd", //361 kifeditor.cxx 4381
"dicopremiercd", //362 kifeditor.cxx 4382
"dicotriécd", //363 kifeditor.cxx 4383
"dicocn", //364 kifeditor.cxx 4384
"dicopremiercn", //365 kifeditor.cxx 4385
"dicotriécn", //366 kifeditor.cxx 4386
"dicocc", //367 kifeditor.cxx 4387
"dicopremiercc", //368 kifeditor.cxx 4388
"dicotriécc", //369 kifeditor.cxx 4389
"dicouu", //370 kifeditor.cxx 4390
"dicouu", //371 kifeditor.cxx 4391
"dicoun", //372 kifeditor.cxx 4392
"dicoun", //373 kifeditor.cxx 4393
"dicoud", //374 kifeditor.cxx 4394
"dicoud", //375 kifeditor.cxx 4395
"diconu", //376 kifeditor.cxx 4396
"diconu", //377 kifeditor.cxx 4397
"dicodu", //378 kifeditor.cxx 4398
"dicodu", //379 kifeditor.cxx 4399
"dicotriéuu", //380 kifeditor.cxx 4400
"dicotriéuu", //381 kifeditor.cxx 4401
"dicotriéun", //382 kifeditor.cxx 4402
"dicotriéun", //383 kifeditor.cxx 4403
"dicotriéud", //384 kifeditor.cxx 4404
"dicotriéud", //385 kifeditor.cxx 4405
"dicotriénu", //386 kifeditor.cxx 4406
"dicotriénu", //387 kifeditor.cxx 4407
"dicotriédu", //388 kifeditor.cxx 4408
"dicotriédu", //389 kifeditor.cxx 4409
"dicopremieruu", //390 kifeditor.cxx 4410
"dicopremieruu", //391 kifeditor.cxx 4411
"dicopremierun", //392 kifeditor.cxx 4412
"dicopremierun", //393 kifeditor.cxx 4413
"dicopremierud", //394 kifeditor.cxx 4414
"dicopremierud", //395 kifeditor.cxx 4415
"dicopremiernu", //396 kifeditor.cxx 4416
"dicopremiernu", //397 kifeditor.cxx 4417
"dicopremierdu", //398 kifeditor.cxx 4418
"dicopremierdu", //399 kifeditor.cxx 4419
"dicobn", //400 kifeditor.cxx 4420
"dicobnn", //401 kifeditor.cxx 4421
"dicobnd", //402 kifeditor.cxx 4422
"dicobnc", //403 kifeditor.cxx 4423
"dicobnu", //404 kifeditor.cxx 4424
"dicobnn", //405 kifeditor.cxx 4425
"dicobnd", //406 kifeditor.cxx 4426
"dicobnc", //407 kifeditor.cxx 4427
"dicobnu", //408 kifeditor.cxx 4428
"KIFF", //409 kifeditor.cxx 4433
""};


Exported const char* KIFSYSSTRINGS[]={
"e", //0 kifsys.cxx 167
"SYS(001): Erreur à l'ouverture du fichier de sortie", //1 kifsys.cxx 169
"l", //2 kifsys.cxx 176
"taille", //3 kifsys.cxx 275
"date", //4 kifsys.cxx 276
"accès", //5 kifsys.cxx 277
"changement", //6 kifsys.cxx 278
"cheminfichier", //7 kifsys.cxx 279
"répertoire", //8 kifsys.cxx 281
"nom", //9 kifsys.cxx 331
"_sys", //10 kifsys.cxx 398     <------- Provide a translation
"Version de KiF incompatible", //11 kifsys.cxx 413
"commande", //12 kifsys.cxx 440
"commande(chaine com,chaine sortiefichier): exécute la commande 'com'. sortiefichier est optionel et permet de rediriger la sortie vers un fichier.", //13 kifsys.cxx 440
"estrépertoire", //14 kifsys.cxx 441
"estrépertoire(chaine chemin): renvoie 'vrai' si le chemin est un répertoire", //15 kifsys.cxx 441
"créerépertoire", //16 kifsys.cxx 442
"créerépertoire(chaine chemin): Crée le répertoire 'chemin'", //17 kifsys.cxx 442
"mkdir", //18 kifsys.cxx 443
"mkdir(chaine chemin): Crée le répertoire 'chemin'", //19 kifsys.cxx 443
"listerépertoire", //20 kifsys.cxx 444
"listerépertoire(chaine chemin): Retourne le contenu du répertoire", //21 kifsys.cxx 444
"ls", //22 kifsys.cxx 445
"ls(chaine chemin): Retourne le contenu du répertoire", //23 kifsys.cxx 445
"cheminabsolu", //24 kifsys.cxx 446
"chaine cheminabsolu(chemin vchemin): Retourne le chemin absolu", //25 kifsys.cxx 446
"infosfichier", //26 kifsys.cxx 447
"dictionaire infosfichier(chaine chemin): Retourne les infos du fichier", //27 kifsys.cxx 447
"env", //28 kifsys.cxx 448
"env(chaine var)|(chaine var,chaine val): Retourne ou initialise le contenu de la variable d'environnement 'var'.", //29 kifsys.cxx 448
""};


Exported const char* KIFCURLSTRINGS[]={
"avec", //0 kifcurl.cxx 321
"URL(031): Option inconnue", //1 kifcurl.cxx 442
"e", //2 kifcurl.cxx 493
"URL(009): Mauvais nom de fichier", //3 kifcurl.cxx 495
"nom", //4 kifcurl.cxx 540
"taille", //5 kifcurl.cxx 561
"Version de KiF incompatible", //6 kifcurl.cxx 626
"curl", //7 kifcurl.cxx 632
"proxy", //8 kifcurl.cxx 658
"proxy(chaine prox): Connexion via un proxy", //9 kifcurl.cxx 658
"motdepasse", //10 kifcurl.cxx 659
"motdepasse(chaine utilisateur,chaine mdp): Fournir le mot de passe utilisateur", //11 kifcurl.cxx 659
"url", //12 kifcurl.cxx 660
"url(chaine chemin): Charge une URL\rurl(chaine chemin,chaine fichier): Charge le contenu du site pointé par l'URL dans un fichier", //13 kifcurl.cxx 660
"exécute", //14 kifcurl.cxx 661
"exécute(chaine nomfichier): Exécute une action curl, le fichier est optionnel.", //15 kifcurl.cxx 661
"options", //16 kifcurl.cxx 662
"options(chaine option,chaine parameter): Définit les options de curl", //17 kifcurl.cxx 662
""};


Exported const char* KIFSQLITESTRINGS[]={
"SQLite(800): Une base données a déjà été ouverte avec cet objet", //0 kifsqlite.cxx 188
"SQLite(801): Erreur durant l'ouverture de la base de données='", //1 kifsqlite.cxx 195
"SQLite(802): Ne peut fermer cette base de données", //2 kifsqlite.cxx 206
"SQLite(803): Ne peut utiliser cette base de données", //3 kifsqlite.cxx 214
"SQLite(811): Erreur d'exécution=", //4 kifsqlite.cxx 227
"SQLite(805): Erreur de création=", //5 kifsqlite.cxx 271
"SQLite(810): Cette base de données n'a pas encore été ouverte...", //6 kifsqlite.cxx 283
"SQLite(830): Erreur lors de la création de la table de persistence:", //7 kifsqlite.cxx 312
"SQLite(837): Erreur pendant l'enregistrement d'un prédicat", //8 kifsqlite.cxx 346
"SQLite(839): Erreur pendant la destruction d'un prédicat", //9 kifsqlite.cxx 390
"SQLite(837): Erreur pendant le test d'un prédicat", //10 kifsqlite.cxx 436
"SQLite(838): Erreur pendant la lecture d'un prédicat", //11 kifsqlite.cxx 482
"SQLite(808): Paramètre manquant dans insèrevia", //12 kifsqlite.cxx 493
"SQLite(897): Table inconnue", //13 kifsqlite.cxx 505
"type", //14 kifsqlite.cxx 512
"SQLite(899): Mauvaise liste de valeurs", //15 kifsqlite.cxx 531
"SQLite(893): Vous devez exécuter un COMMENCE d'abord", //16 kifsqlite.cxx 536
"SQLite(895): mauvaise valeur pour:", //17 kifsqlite.cxx 554
"SQLite(812): La transaction a échouée!", //18 kifsqlite.cxx 566
"SQLite(805): Erreur de commencement de transaction=", //19 kifsqlite.cxx 593
"SQLite(807): Erreur de transmission=", //20 kifsqlite.cxx 615
"SQLite(808): Paramètre manquant dans 'INSERE'", //21 kifsqlite.cxx 634
"nom", //22 kifsqlite.cxx 638
"age", //23 kifsqlite.cxx 638
"SQLite(805): Erreur de création=", //24 kifsqlite.cxx 666
"Mauvaise expression SQL:", //25 kifsqlite.cxx 858
"Version de KiF incompatible", //26 kifsqlite.cxx 909
"SQLite est une marque déposée de Hipp, Wyrick & Company, Inc.", //27 kifsqlite.cxx 913
"ouvre", //28 kifsqlite.cxx 938
"ouvre(chaine chemin): ouvre un fichier de base de données", //29 kifsqlite.cxx 938
"prédicat", //30 kifsqlite.cxx 939
"prédicat(chaine nom, nombre arité): déclare un prédicat d'arité 'arité' dans la base de données", //31 kifsqlite.cxx 939
"%litprédicat", //32 kifsqlite.cxx 940
"litprédicat(): lit un prédicat dans la base de données", //33 kifsqlite.cxx 940
"%compteprédicat", //34 kifsqlite.cxx 941
"compteprédicat(): compte les prédicats présents dans la base de données", //35 kifsqlite.cxx 941
"%enregistreprédicat", //36 kifsqlite.cxx 942
"enregistreprédicat: enregistre un prédicat dans la base de données", //37 kifsqlite.cxx 942
"%retireprédicat", //38 kifsqlite.cxx 943
"retireprédicat(): retire les prédicats de la base de données", //39 kifsqlite.cxx 943
"ferme", //40 kifsqlite.cxx 945
"ferme(): ferme une base de données", //41 kifsqlite.cxx 945
"crée", //42 kifsqlite.cxx 946
"crée(x1,x2,x3): crée une table dans la base de données avec  les arguments x1,x2,x3.\rEx. mydb.crée('table1','nom TEXT PRIMARY KEY','age INTEGER');", //43 kifsqlite.cxx 946
"insère", //44 kifsqlite.cxx 947
"insère(chaine table,chaine colonne,nombre valeur,...): insère une ligne dans une table.\rEx. mydb.insère('table1','nom',nm,'age',i);", //45 kifsqlite.cxx 947
"insèrevia", //46 kifsqlite.cxx 948
"insèrevia(string table,int v1,...): Insère une ligne dans un mode commence/transmet.\rEx. mydb.insèrevia('table1',nm,i);", //47 kifsqlite.cxx 948
"lance", //48 kifsqlite.cxx 949
"lance(chaine sqlcommand): Exécute une commande SQL. Si la variable de réception est une table, toutes les valeurs extraites seront placées dedans. Si la variable de réception est un itérateur, alors chaque valeur sera un dictionnaire dont les noms seront les attributs de la table.", //49 kifsqlite.cxx 949
"exécute", //50 kifsqlite.cxx 950
"exécute(chaine sqlcommande): exécute une commande sql brute sans retour.", //51 kifsqlite.cxx 950
"commence", //52 kifsqlite.cxx 951
"commence(chaine mode): Pour entrer en mode transaction: le mode par défaut est DEFERRED: Les autres modes sont: IMMEDIATE et EXCLUSIVE", //53 kifsqlite.cxx 951
"transmet", //54 kifsqlite.cxx 952
"transmet(): Fin du mode différé. L'ensemble des commandes est alors transmis à la base de données pour exécution.", //55 kifsqlite.cxx 952
"mémoire", //56 kifsqlite.cxx 953
"mémoire(): renvoie la consommation mémoire de SQLite.", //57 kifsqlite.cxx 953
""};


Exported const char* KIFXMLSTRINGS[]={
"XML(234): Déjà créé", //0 kifxml.cxx 210
"XML(102): Fichier inconnu", //1 kifxml.cxx 256
"table", //2 kifxml.cxx 265
"dictionnaire", //3 kifxml.cxx 276
"XML(013): Variable exigée", //4 kifxml.cxx 347
"XML(106): N'est pas un buffer XML", //5 kifxml.cxx 447
"XML(097): Noeud XML vide", //6 kifxml.cxx 506
"XML(123): On attend un dictionnaire", //7 kifxml.cxx 524
"XML(235): doc vide", //8 kifxml.cxx 573
"XML(099): Pas de document", //9 kifxml.cxx 679
"Version de KiF incompatible", //10 kifxml.cxx 696
"xml", //11 kifxml.cxx 701
"xmldoc", //12 kifxml.cxx 704
"charge", //13 kifxml.cxx 727
"charge(chaine filename): charge un fichier xml", //14 kifxml.cxx 727
"àlafermeture", //15 kifxml.cxx 728
"àlafermeture(function,object): Fonction de rappel pour les balises fermantes", //16 kifxml.cxx 728
"décompose", //17 kifxml.cxx 729
"décompose(chaine buffer): décompose un buffer xml", //18 kifxml.cxx 729
"crée", //19 kifxml.cxx 730
"crée(chaine topnoeud): Crée un fichier XML avec topnoeud comme noeud racine, ou avec topnode au complet, si topnode est une structure XML.", //20 kifxml.cxx 730
"xpath", //21 kifxml.cxx 731
"xpath(chaine myxpath): Renvoie une table de noeuds xml correspondant au chemin xpath myxpath", //22 kifxml.cxx 731
"sauvegarde", //23 kifxml.cxx 732
"sauvegarde(chaine fichier,chaine encodage): Sauvegarde un fichier XML. L'encodage par défaut est 'utf8'", //24 kifxml.cxx 732
"ferme", //25 kifxml.cxx 733
"ferme(): ferme un fichier xml", //26 kifxml.cxx 733
"noeud", //27 kifxml.cxx 734
"noeud(): Retourne le noeud racine", //28 kifxml.cxx 734
"chainexml", //29 kifxml.cxx 735
"chainexml(): Renvoie un document XML sous la forme d'une chaine", //30 kifxml.cxx 735
"sérialise", //31 kifxml.cxx 736
"sérialise(objet): Sérialise un objet KiF sous la forme d'un document XML", //32 kifxml.cxx 736
"sérialisechaine", //33 kifxml.cxx 737
"sérialisechaine(objet): Sérialise un objet KiF sous la forme d'un document XML et renvoie la chaine correspondante", //34 kifxml.cxx 737
"racine", //35 kifxml.cxx 739
"racine(): Retourne le noeud racine", //36 kifxml.cxx 739
"document", //37 kifxml.cxx 740
"document(): renvoie le xmldoc associé à ce noeud", //38 kifxml.cxx 740
"suivant", //39 kifxml.cxx 741
"suivant(): renvoie le noeud XML suivant", //40 kifxml.cxx 741
"délie", //41 kifxml.cxx 742
"délie(): Retire un noeud de la structure arborée", //42 kifxml.cxx 742
"détruit", //43 kifxml.cxx 743
"détruit(): Détruit la representation interne du noeud XML", //44 kifxml.cxx 743
"précédent", //45 kifxml.cxx 744
"précédent(): Renvoie le noeud XML précédent", //46 kifxml.cxx 744
"parent", //47 kifxml.cxx 745
"parent(): renvoie le noeud XML parent", //48 kifxml.cxx 745
"enfant", //49 kifxml.cxx 746
"enfant(): renvoie le noeud XML enfant", //50 kifxml.cxx 746
"nom", //51 kifxml.cxx 747
"nom(): nom du noeud", //52 kifxml.cxx 747
"ligne", //53 kifxml.cxx 748
"ligne(): Renvoie la ligne où se trouve le noeud", //54 kifxml.cxx 748
"id", //55 kifxml.cxx 749
"id(): Renvoie l'identifieur du noeud (uniquement disponible avec les fonctions de rappel)", //56 kifxml.cxx 749
"typexml", //57 kifxml.cxx 750
"typexml(): renvoie le type du noeud XML", //58 kifxml.cxx 750
"propriétés", //59 kifxml.cxx 751
"propriétés(): renvoie les attributs du noeud XML", //60 kifxml.cxx 751
"contenu", //61 kifxml.cxx 752
"contenu(): renvoie le contenu textuel du noeud XML", //62 kifxml.cxx 752
"namespace", //63 kifxml.cxx 753
"namespace(): Retourne la table de namespace du noeud xml", //64 kifxml.cxx 753
"nouveau", //65 kifxml.cxx 754
"nouveau(chaine name): Crée un nouveau noeud XML", //66 kifxml.cxx 754
"chainexml(): Retourne l'arbre complet depuis le noeud courant sous la forme d'une chaine de caractères.", //67 kifxml.cxx 755
""};


Exported const char* KIFREGEXSTRINGS[]={
"RGX(001): Expression régulière fausse:", //0 kifregex.cxx 68
"RGX(002): Expression régulière fausse:", //1 kifregex.cxx 132
"RGX(003): Expression régulière fausse:", //2 kifregex.cxx 207
"Version de KiF incompatible", //3 kifregex.cxx 249
"éclatergx", //4 kifregex.cxx 262
"remplacergx", //5 kifregex.cxx 263
""};


Exported const char* KIFMPSTRING[]={
"avec", //0 kifsound.cxx 146
"SND(001): Fichier inconnu", //1 kifsound.cxx 179
"SND(005): Erreur durant le chargement", //2 kifsound.cxx 247
"SND(003): Veuillez charger un fichier d'abord", //3 kifsound.cxx 256
"Version de KiF incompatible", //4 kifsound.cxx 372
"mp3", //5 kifsound.cxx 378
"charge", //6 kifsound.cxx 401
"charge(chaine chemin): charge le fichier mp3.", //7 kifsound.cxx 401
"joue", //8 kifsound.cxx 402
"joue(): joue le son", //9 kifsound.cxx 402
"interromps", //10 kifsound.cxx 403
"interromps(): interromps le son", //11 kifsound.cxx 403
""};


Exported const char* KIFSOUNDSTRINGS[]={
"SND(003): Erreur au chargement", //0 kifaosound.cxx 202
"SND(013): Format inconnu", //1 kifaosound.cxx 205
"SND(004): Impossible d'ouvrir le flux audio mp3", //2 kifaosound.cxx 219
"SND(004): Impossible d'ouvrir le flux audio", //3 kifaosound.cxx 262
"SND(001): Déjà chargé", //4 kifaosound.cxx 315
"SND(002): Veuillez charger votre fichier d'abord", //5 kifaosound.cxx 341
"SND(005): Erreur lors de la lecture du flux audio", //6 kifaosound.cxx 377
"SND(011): Veuillez ouvrir un flux d'abord", //7 kifaosound.cxx 387
"SND(010): le paramètre d'entrée doit être une tablenombres", //8 kifaosound.cxx 392
"canaux", //9 kifaosound.cxx 496
"taux", //10 kifaosound.cxx 498
"type_octet", //11 kifaosound.cxx 500
"mp3", //12 kifaosound.cxx 502
"bits", //13 kifaosound.cxx 504
"taille", //14 kifaosound.cxx 506
"SND(008): Le paramètre à fournir doit être un dictionnaire", //15 kifaosound.cxx 520
"Version de KiF incompatible", //16 kifaosound.cxx 690
"son", //17 kifaosound.cxx 696
"charge", //18 kifaosound.cxx 720
"charge(chaine chemin): Charge un fichier son.", //19 kifaosound.cxx 720
"joue", //20 kifaosound.cxx 721
"joue(bool raz): joue le son. si raz est vrai le flux repart à 0", //21 kifaosound.cxx 721
"interromps", //22 kifaosound.cxx 722
"interromps(): interromps le son", //23 kifaosound.cxx 722
"paramètres", //24 kifaosound.cxx 723
"paramètres(): Renvoie les infos sous la forme d'un dicocn ou les modifie (uniquement le taux et le nombre de canaux", //25 kifaosound.cxx 723
"raz", //26 kifaosound.cxx 724
"raz(): Remet à zéro le flux audio", //27 kifaosound.cxx 724
"ouvre", //28 kifaosound.cxx 725
"ouvre(dicocn p): Ouvre un flux audio avec les paramètres p (côté serveur)", //29 kifaosound.cxx 725
"ferme", //30 kifaosound.cxx 726
"ferme(): ferme le flux audio (côté serveur)", //31 kifaosound.cxx 726
"décode", //32 kifaosound.cxx 727
"décode(tablenombres ): décode un flux audio, remplit une tablenombres. Renvoie 'faux' quand le flux est fini (côté client)", //33 kifaosound.cxx 727
"encode", //34 kifaosound.cxx 728
"encode(tablenombres tn): joue un flux audio provenant d'un décode (côté serveur)", //35 kifaosound.cxx 728
""};


Exported const char* KIFXMLSTRINGSINCLUDE[]={
"XML(097): Noeud XML vide", //0 kifxml.h 108
"XML(098): On attend un noeud XML", //1 kifxml.h 131
"nom", //2 kifxml.h 328
"taille", //3 kifxml.h 349
"avec", //4 kifxml.h 446
"XML(234): Déjà créé", //5 kifxml.h 578
"XML(235): doc vide", //6 kifxml.h 599
""};


Exported const char* KIFTHEINFOSTRINGS[]={
"reçoit(","- reçoit(nombre num): Reçoit une chaine provenant d'une application non KiF. 'num' doit être omis du côté client.",
"créeserveur(","- créeserveur(chaine hôte,nombre port,nombre nblients): crée un serveur. Si l'hôte est omis, le nom local est alors utilisé",
"inserse(","- inserse(): Renvoie la matrice inversée",
"policenoeud(","- policenoeud(nombre c): Définit ou renvoie la police du noeud.",
"ok(","- ok(): renvoie vrai si ok a été sélectionné",
"cherche(","- cherche(chaine s,nombre i,bool casse): Cherche la position de la sous-chaine s à partir de i\n- cherche(chaine path): Renvoie l'élément correspondant au chemin path.",
"déterminant(","- déterminant(): renvoie le déterminant de la matrice",
"transposée(","- transposée(): renvoie la matrice transposée",
"remplace(","- remplace(chaine s,chaine sub,nombre i,bool casse): remplace s avec sub selon la casse à partir de i",
"dimension(","- dimension(): renvoie la taille de la matrice.\ndimension(nombre tailleligne,nombre taillecolonne): définit la taille de la matrice.",
"estrépertoire(","- estrépertoire(chaine chemin): renvoie 'vrai' si le chemin est un répertoire",
"saisie(","- saisie(int num): lit un caractère depuis le connecteur. 'num' doit être omis du côté client.\n- saisie(chaine msg): Affiche une fenêtre pour lire une valeur",
"connecte(","- connecte(chaine hôte,nombre port): connexion à un serveur",
"siglissedépose(","- siglissedépose(fonction,objet): Initialise la fonction de rappel lors d'un glisser-déposer",
"translate(","- translate(décimal x,décimal y): translate la transformation courante",
"somme(","- somme(): renvoie la somme de tous les éléments",
"lafonction(","- lafonction(chaine frame): Retourne une fonction declarée du côté serveur",
"laclasse(","- laclasse(chaine frame): Retourne un identifiant objet déclaré du côté serveur",
"entêteligne(","- entêteligne(nombre pos,chaine label): Définit le label de l'entête de la ligne à la ligne pos",
"produit(","- produit(): renvoie le produit de tous les éléments",
"nombre attends(","- nombre attends(): attend qu'un client se connecte et renvoie son identifiant.",
"lit(","- lit(nombre num): lit une chaine sur le connecteur. Du côté serveur, num est l'identifiant du client. Utiliser 'lit()' sans paramètre du côté client",
"débutpoints(","- débutpoints(): Commence à accumuler des sommets",
"lance(","- lance(nombre num,chaine stop): Uniquement du côté serveur. Exécute des invocations de méthodes distantes. 'stop' est une chaine qui permet l'arrêt de ce service par le client.\n- lance(): lance la boucle principale de l'interface graphique\n- lance(chaine sqlcommand): Exécute une commande SQL. Si la variable de réception est une table, toutes les valeurs extraites seront placées dedans. Si la variable de réception est un itérateur, alors chaque valeur sera un dictionnaire dont les noms seront les attributs de la table.",
"désurbrille(","- désurbrille(): Retire la  sur-brillance d'une portion de texte",
"alerte(","- alerte(chaine msg): fenêtre pour afficher une alerte",
"document(","- document(): renvoie le xmldoc associé à ce noeud",
"écrit(","- écrit(nombre num,chaine s): Ecrit une chaine s sur le connecteur. Du côté serveur, 'num' est l'identifiant client renvoyé par 'attends'. En revanche du côté client, utiliser 'écrit(chaine s)'",
"transformey(","- transformey(décimal x,décimal y): Transforme les coordonnées en Y sur la base de la matrice courante de transformation",
"demande(","- demande(chaine msg,chaine boutonmsg1,chaine boutonmsg2,chaine boutonmsg3): Fenêtre pour poser une question",
"chaine cheminabsolu(","- chaine cheminabsolu(chemin vchemin): Retourne le chemin absolu",
"envoie(","- envoie(nombre num,chaine s): Ecrit une chaine à destination d'une application non KiF. 'num' doit être omis du côté client.",
"ferme(","- ferme(nombre num): Ferme un connecteur. Du côté serveur, si 'num' est fourni, il ferme la connexion pour un client donné.\n- ferme(): ferme la fenêtre\n- ferme(): Ferme le widget fparcourirfichiers\n- ferme(chaine path): Ferme l'élément.\rferme(fnoeudarbre e): Ferme l'élément.\n- ferme(): ferme une base de données\n- ferme(): ferme un fichier xml\n- ferme(): ferme le flux audio (côté serveur)",
"multmatrice(","- multmatrice(décimal a,décimal b,décimal c,décimal d, décimal x, décimal y): combine les transformations",
"port(","- port(): renvoie le port courant",
"bloquant(","- bloquant(bool flag): Si 'flag' est 'vrai', le connecteur fonctionne en mode bloquant.",
"transformedy(","- transformedy(décimal x,décimal y): Transforme la distance DY sur la base de la matrice courante de transformation",
"taillemax(","- taillemax(nombre minw,nombre minh, nombre maxw,nombre maxh): définit les tailles maximum d'agrandissement de la fenêtre",
"tempsmax(","- tempsmax(nombre t): Définit un temps maximum de 't' secondes pour le connecteur",
"motdepasse(","- motdepasse(chaine msg): Affiche une fenêtre de saisie de mot de passe\n- motdepasse(chaine utilisateur,chaine mdp): Fournir le mot de passe utilisateur",
"nomhôte(","- nomhôte(): Retourne le nom de l'hôte",
"nomclient(","- nomclient(nombre num): Retourne le nom du client",
"sitouche(","- sitouche(nombre action, fonction,objet): Définit le nom de la fonction de rappel pour une action clavier\n- sitouche(nombre action,fonction f,objet o): fonction de rappel quand une touche est pressée",
"crée(","- crée(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée une fenêtre sans widgets, largeur et hauteur sont optionnels\n- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée un défilement\n- crée(nombre x,nombre y,nombre  hauteur,nombre  largeur,chaine label): Crée un fcompteur\n- crée(nombre x,nombre y,nombre  hauteur,nombre  largeur,chaine label): Crée un fprogression\n- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,nombre alignement,bool valeur,glissière,chaine label): Crée une glissière avec ou sans valeur\n- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée une boite\n- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine type,chaine forme,chaine label): Crée un bouton. Si le type n'est pas fourni, il prend la valeur FL_Régulier par défaut.\n- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée une ftable d'objets, et commence l'ajout\n- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,booléen multiline,chaine label): Crée une fentrée\n- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée un éditeur\n- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,booléen multiline,chaine label): Crée une fsortie\n- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée un choix\n- crée(chaine intialdirectory,chaine filter,bool directory,chaine label): Ouvre un fparcourirfichiers, en mode répertoire si directory est vrai\n- crée(x,y,largeur,hauteur,label): Crée un widget fparcourir\n- crée(nombre x,nombre y,nombre  hauteur,nombre  largeur,chaine label): Crée un arbre\n- crée(x1,x2,x3): crée une table dans la base de données avec  les arguments x1,x2,x3.\rEx. mydb.crée('table1','nom TEXT PRIMARY KEY','age INTEGER');\n- crée(chaine topnoeud): Crée un fichier XML avec topnoeud comme noeud racine, ou avec topnode au complet, si topnode est une structure XML.",
"empilematrice(","- empilematrice(): Sauvegarde la transformation courante",
"commence(","- commence(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée une fenêtre et commence l'initialisation, largeur et hauteur sont optionnels\n- commence(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée un onglet fenêtre et commence l'initialisation\n- commence(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée un fgroupe au sein d'un onglet et commence l'initialisation\n- commence(chaine mode): Pour entrer en mode transaction: le mode par défaut est DEFERRED: Les autres modes sont: IMMEDIATE et EXCLUSIVE",
"rectangleplein(","- rectangleplein(nombre x,nombre y,nombre  largeur, nombre  hauteur, chaine c|nombre c): Remplit un rectangle avec la couleur optionnelle c",
"fin(","- fin(): fin création\n- fin(): fin de la création de l'onglet\n- fin(): fin création du fgroupe",
"surbrille(","- surbrille(nombre start,nombre fin)|(): Met une portion de texte en  sur-brillance. Renvoie vrai si le texte est sur-ligné ou le texte en  sur-brillance",
"àlafermeture(","- àlafermeture(fonction,objet): Fonction de rappel lors de la fermeture\n- àlafermeture(function,object): Fonction de rappel pour les balises fermantes",
"couleurdessin(","- couleurdessin(nombre c|chaine c): Couleur des prochains objets dessinés",
"autemps(","- autemps(fonction,t,objet): Initialise une fonction de rappel pour chaque intervalle de temps t",
"initialisepolices(","- initialisepolices(): Charge les polices systèmes. Renvoie le nombre de polices disponibles",
"nompolice(","- nompolice(nombre num): nom de la police.",
"boucle(","- boucle(nombre x,nombre y,nombre x1, nombre y1,nombre x2, nombre y2, nombre x3, nombre y3): Dessine une série de lignes, x3 et y3 sont optionnels",
"taillepolices(","- taillepolices(nombre num): renvoie une table des tailles disponibles de police.",
"nombrepolice(","- nombrepolice(): Renvoie le nombre de polices disponibles.",
"rotation(","- rotation(décimal x,décimal y, décimal distance, décimal angle, bool dessine): Calcule les coordonnées d'un point après rotation du point de coordonnées x,y, selon une distance et un angle (en radian). Renvoie une table de décimaux contenant ces nouvelles coordonnées.",
"dessinepoints(","- dessinepoints(tabledécimaux xy,nombre épaisseur,tabledécimaux repère): Dessine un graphe à partir des coordonnées successives x,y dans xy. Renvoie une table de décimaux correspondant aux éléments nécessaires pour calculer une nouvelle position avec coordsdessinepoints. Si épaisseur==0, alors les points sont joints les uns aux autres. Le repère a la forme: [maxXfenêtre,maxYfenêtre,minxXValeur,minYValeur,maxXValeur,maxYValeur,incx,incy]. On peut fournir une table comprendant les 2 premières valeurs, les 6 premières ou toutes.",
"accumule(","- accumule(chaine s): Ajoute s à la fin de l'entrée",
"coordsdessinepoints(","- coordsdessinepoints(tabledécimaux inits,décimal x, décimal y): renvoie les coordonnées fenêtre [xe,ye] d'un point x,y en fonction des paramêtres d'échelle calculés pour dessiner le graphe.",
"camembert(","- camembert(nombre x,nombre y,nombre x1, nombre y1, décimal a1, décimal a2): Dessine un camembert",
"menu(","- menu(table,nombre x,nombre y,nombre  largeur, nombre h): initialise un menu avec ses fonction de rappels\n- menu(table s): Initialise le menu",
"ajoutestyle(","- ajoutestyle(dictionaire style): Initialise les styles",
"raz(","- raz(): Nettoie la ftable\n- raz(): Vide le widget fparcourir de toutes ses valeurs\n- raz(): Détruit les noeuds de l'arbre\n- raz(): Remet à zéro le flux audio",
"redimensionnable(","- redimensionnable(objet): objet redimensionnable",
"modale(","- modale(bool b): Si vrai  la fenêtre modale. Si pas de paramètres renvoie si la fenêtre est modale",
"transformesommets(","- transformesommets(décimal x,décimal y): ajoute les transformations à la liste des sommets",
"finpoints(","- finpoints(): Cesse d'accumuler des sommets",
"pas(","- pas(décimal): definit le pas du fcompteur\n- pas(nombre): définit le pas de la glissière",
"formeligne(","- formeligne(chaine type,nombre largeur): Sélectionne la forme de la ligne et son épaisseur",
"bords(","- bords(bool b): Si vrai ajoute ou retire les bords. Si pas de paramètres renvoie si la fenêtre a des bords",
"couleurarrièreplan(","- couleurarrièreplan(nombre c|chaine c): initialise la couleur d'arrière-plan\n- couleurarrièreplan(nombre c): définit ou renvoie la couleur de la cellule en arrière-plan\n- couleurarrièreplan(nombre c): Définit ou renvoie la couleur de l'arrière-plan du noeud.",
"rectangle(","- rectangle(nombre x,nombre y,nombre  largeur, nombre  hauteur, chaine c|nombre c): Dessine un rectangle avec la couleur optionnelle c",
"sommet(","- sommet(décimal x,décimal y): Ajoute un sommet à une structure complexe",
"dépilematrice(","- dépilematrice(): Restaure la transformation courante",
"polygone(","- polygone(nombre x,nombre y,nombre x1, nombre y1,nombre x2, nombre y2, nombre x3, nombre y3): Dessine un polygone, x3 et y3 sont optionnels",
"ligne(","- ligne(nombre x,nombre y,nombre x1, nombre y1,nombre x2, nombre y2): Dessine une ligne entre deux points, x2 et y2 sont optionnels\n- ligne()|(nombre nb): Définit le nombre de lignes\n- ligne(nombre l): renvoie la position de la ligne courante du chariot ou renvoie la ligne correspondant à la position\n- ligne(): Renvoie la ligne où se trouve le noeud",
"échelle(","- échelle(décimal x,décimal y)|(x): Met à l'échelle la transformation courante",
"racine(","- racine(): Renvoie la racine.\n- racine(): Retourne le noeud racine",
"sauvegarde(","- sauvegarde(chaine f): Sauvegarde le contenu de l'éditeur dans le fichier f\n- sauvegarde(chaine fichier,chaine encodage): Sauvegarde un fichier XML. L'encodage par défaut est 'utf8'",
"pivote(","- pivote(décimal d): tourne de d degrés la transformation courante",
"sérialisechaine(","- sérialisechaine(objet): Sérialise un objet KiF sous la forme d'un document XML et renvoie la chaine correspondante",
"labelracine(","- labelracine(chaine r): Définit le label de la racine.",
"estracine(","- estracine(): Renvoie vrai si l'élément est une racine.",
"transformex(","- transformex(décimal x,décimal y): Transforme les coordonnées en X sur la base de la matrice courante de transformation",
"transformedx(","- transformedx(décimal x,décimal y): Transforme la distance DX sur la base de la matrice courante de transformation",
"débutligne(","- débutligne(): Commence à dessiner des lignes.",
"finligne(","- finligne(): Cesse d'ajouter des lignes",
"débutboucle(","- débutboucle(): Commence à dessiner des séquences de lignes qui se referment.",
"finboucle(","- finboucle():    Cesse d'accumuler des séquences de lignes qui se referment.",
"formatcarac(","- formatcarac(): Renvoie le caractère de formatage.\rformatcarac(chaine): Définit le caractère de formatage",
"débutpolygone(","- débutpolygone(): Commence à dessiner un polygone plein convexe",
"finpolygone(","- finpolygone(): Cesse de dessiner un polygone plein convexe",
"modesélection(","- modesélection(nombre s): Définit ou renvoie le mode de sélection.",
"sisouris(","- sisouris(nombre action, fonction,objet): Définit le nom de la fonction de rappel pour une action souris\n- sisouris(nombre action,fonction f,objet o): fonction de rappel après un clic",
"débutpolygonecomplexe(","- débutpolygonecomplexe(): Commence à dessiner un polygone complexe",
"entêtecolonne(","- entêtecolonne(nombre pos,chaine label): Définit le label de l'entête de la colonne à la colonne pos",
"trou(","- trou(): remplit les trous lors du dessin d'un polygone complexe.",
"finpolygonecomplexe(","- finpolygonecomplexe(): Cesser de dessiner un polygone complexe",
"arc(","- arc(nombre x,nombre y,nombre x1, nombre y1, décimal a1, décimal a2): Dessine un arc.\rarc(décimal x,décimal y,décimal rad,décimal a1,décimal a2): Ajoute une série de points en arc de cercle au chemin courant.",
"courbe(","- courbe(décimal x,décimal y,décimal x1,décimal y1,décimal x1,décimal y2,décimal x3,décimal y3): Ajoute une série de points sur une courbe de Bezier au chemin courant. La fin de la courbe (et deux de ses points) est à la position x,y et x3,y3.",
"ls(","- ls(chaine chemin): Retourne le contenu du répertoire",
"coupe(","- coupe(): Coupe le texte sélectionné depuis l'éditeur dans le presse-papier",
"empileclip(","- empileclip(nombre x,nombre y,nombre  largeur, nombre h): Définit une région clippée de coordonnées x,y,largeur,h",
"dépileclip(","- dépileclip(): Relache une région clippée",
"point(","- point(nombre x,nombre y): Dessine un pixel",
"cercle(","- cercle(nombre x,nombre y,nombre r,nombre couleur): Dessine un cercle. 'couleur' est optionnelle.",
"tailletexte(","- tailletexte(chaine l): Renvoie un dictionnaire avec largeur et hauteur comme clef pour donner les largeurs et hauteurs de la chaine en pixels",
"dessinetexte(","- dessinetexte(chaine l,nombre x,nombre y): Place un texte à la position x,y",
"redessine(","- redessine(): Redessine la fenêtre",
"couleurrvb(","- couleurrvb(chaine|nombre couleur)|(nombre r,nombre g,nombre b)|(table couleur): Renvoie soit une table de la décomposition en couleur ou renvoie la valeur de la couleur correspondant à la combinaison RVB fournie",
"couleurlabel(","- couleurlabel(nombre c): définit ou renvoie la couleur du label",
"police(","- police(chaine f,nombre sz): Définit le nom de la police et sa taille\n- police(nombre s): définit ou renvoie la police du texte\n- police(chaine s): définit ou renvoie la police du texte\n- police(chaine s)|(): définit ou renvoie la police du texte\n- police(nombre c): Définit ou renvoie la police du noeud.",
"stylecurseur(","- stylecurseur(nombre curseurtype, nombre couleur,nombre couleur): Définit la forme du curseur\n- stylecurseur(nombre curseurforme): Définit la forme du curseur",
"cache(","- cache(bool h): Cache la fenêtre si hauteur est vrai\n- cache(): Cache le widget",
"montre(","- montre(): Montre la fenêtre\n- montre(): Montre le widget",
"focus(","- focus(): Récupère le focus",
"chasse(","- chasse(): force à redessiner la fenêtre si celle-ci est dans une tâche",
"position(","- position()|(nombre x,nombre y): Renvoie la position de la fenêtre ou définit la position de la fenêtre",
"taille(","- taille()|(nombre x,nombre y,nombre  largeur, nombre h): Renvoie la taille de la fenêtre ou définit la taille de la fenêtre",
"réveil(","- réveil(): Réveille une fenêtre définie dans une tâche",
"verrouille(","- verrouille(): Verrouille tâche",
"déverrouille(","- déverrouille(): Déverrouillage tâche",
"image(","- image(image image,nombre x, nombre y, nombre  largeur, nombre h): Affiche une image\n- image(image im,chaine label,nombre labelalign): Dessine le bouton avec une image particulière",
"bitmap(","- bitmap(bitmap image,nombre couleur,nombre x, nombre y, nombre w, nombre h): Affiche un bitmap avec une couleur particulière\n- bitmap(bitmap im,nombre couleur,chaine label,nombre labelalign): Utilise le bitmap comme image de bouton",
"taillelabel(","- taillelabel(nombre c): définit ou renvoie la taille de la police du label",
"policelabel(","- policelabel(nombre c): définit ou renvoie la police du label",
"typelabel(","- typelabel(nombre c): définit ou renvoie le type du label",
"hauteurligne(","- hauteurligne(nombre hauteur): Définit la hauteur de la ligne en pixel",
"couleursélection(","- couleursélection(nombre couleur): Couleur de l'élément sélectionné\n- couleursélection(nombre couleur): Couleur pour les éléments sélectionnés",
"label(","- label(chaine s): définit ou renvoie le texte du label\n- label(): Renvoie le label du noeud.",
"coords(","- coords(nombre c): renvoie la table des coordonnées du widget",
"créée(","- créée(): Renvoie vrai si l'objet a été crée",
"copie(","- copie(chaine): Copie la chaine dans le presse-papier\n- copie(chaine s): Copie le texte sélectionné depuis l'éditeur dans le presse-papier (chaine s est optionnel)",
"colle(","- colle(): Colle chaine depuis le presse-papier\n- colle(): Colle le texte sélectionné depuis le presse-papier",
"aligne(","- aligne(nombre): définit l'alignement du label\n- aligne(nombre): définit l'alignement de la glissière\n- aligne(nombre): définit l'alignement du label du bouton",
"retaille(","- retaille(objet): objet redimensionnable",
"truc(","- truc(chaine msg): Ajoute un message de 'truc' au widget",
"créerépertoire(","- créerépertoire(chaine chemin): Crée le répertoire 'chemin'",
"margegauche(","- margegauche(nombre s):Définit ou récupère l'espace (en pixels) qui doit apparaitre entre la bordure gauche du widget et le contenu de l'arbre.",
"bornes(","- bornes(décimal x,décimal y): définit les limites du fcompteur\n- bornes(nombre x,nombre y): définit les limites de la glissière",
"type(","- type(bool normal): si 'vrai' alors fcompteur double sinon fcompteur simple\n- type(nombre x): Type de la glissière\n- type(chaine|nombre boitetype): Définit le type de la boite",
"paslong(","- paslong(décimal): definit le pas long du fcompteur",
"lespas(","- lespas(décimal court,décimal long): definit les pas court et long du fcompteur",
"valeur(","- valeur(décimal): définit la valeur du fcompteur ou renvoie sa valeur\n- valeur(décimal): définit la valeur de la progression ou renvoie sa valeur\n- valeur(nombre): définit la valeur pour glissière ou renvoie sa valeur\n- valeur(): renvoie la valeur du bouton courant\n- valeur()|(chaine v): renvoie le buffer de fentrée ou définit le buffer initial\n- valeur()|(chaine v): renvoie le texte dans l'éditeur ou initialise l'éditeur\n- valeur(chaine s): définit le buffer de fsortie\n- valeur(nombre s): définit la valeur d'initialisation du choix\n- valeur(): Renvoie le fichier sélectionné\n- valeur(): renvoie la valeur courante sélectionnée\n- valeur(objet): Associe le noeud avec une valeur ou renvoie cette valeur.",
"ouvre(","- ouvre(): Ouvre un fichier via une fenêtre mac\n- ouvre(chaine path): Ouvre l'élément.\rouvre(fnoeudarbre e): Ouvre l'élément.\n- ouvre(chaine chemin): ouvre un fichier de base de données\n- ouvre(dicocn p): Ouvre un flux audio avec les paramètres p (côté serveur)",
"couleurtexte(","- couleurtexte(chaine code|nombre code): Définit la couleur du texte",
"minimum(","- minimum(décimal x): définit le minimum ou renvoie le minimum",
"sélectionne(","- sélectionne(): Renvoie la chaine sélectionnée.\rsélectionne(int pospremier,int posdernier): Sélectionne la chaine entre pospremier et posdernier\n- sélectionne(): Renvoie la chaine sélectionnée.\rsélectionne(int i): Renvoie la chaine en position i",
"maximum(","- maximum(décimal x): définit le maximum ou renvoie le maximum",
"couleurbarre(","- couleurbarre(nombre couleur): Couleur de la barre de progression",
"largeurcolonne(","- largeurcolonne(nombre largeur): Définit là largeur de la colonne en pixel",
"typeboite(","- typeboite(nombre x): Définit le type de boite autour de la glissière\n- typeboite(nombre boitetype): type boite",
"noeud(","- noeud(): Retourne le noeud racine",
"quand(","- quand(chaine when1, chaine when2,...): Type de l'événement pour un bouton qui déclenche la fonction de rappel\n- quand(chaine when1, chaine when2,...): Type de l'événement déclenchant la fonction de rappel",
"raccourci(","- raccourci(chaine clef code): Définit un raccourci-clavier correspondant à l'activation du bouton",
"couleur(","- couleur(chaine code|nombre code): Définit la couleur du bouton\n- couleur(chaine c|nombre c): définit ou renvoie la couleur du texte",
"litprédicat(","- litprédicat(): lit un prédicat dans la base de données",
"courant(","- courant(): Renvoie l'onglet courant actif",
"ajoute(","- ajoute(fgroupe g): Ajoute dynamiquement un nouvel onglet\n- ajoute(nombre R,nombre C,chaine v): Ajoute une valeur ligne R, colonne C\n- ajoute(label): Ajoute une chaine au widget fparcourir\n- ajoute(chaine path): Ajoute un noeud.\rajoute(fnoeudarbre e,chaine n): Ajoute un noeud après e.",
"retire(","- retire(fgroupe g): Retire un onglet\n- retire(fnoeudarbre e): Retire l'élément e dans l'arbre.",
"active(","- active(): Active cet onglet\n- active(): Active l'élément courant.",
"chargejpeg(","- chargejpeg(chaine filenom): Charge une image JPEG",
"chargegif(","- chargegif(chaine filenom): Charge une image GIF",
"couleurconnecteur(","- couleurconnecteur(nombre c): Définit ou renvoie la couleur du connecteur.",
"charge(","- charge(table bitmaps,nombre l,nombre h): Charge un bitmap\n- charge(chaine f): Charge le fichier dans l'éditeur\n- charge(chaine): Charge un fichier via une fenêtre mac\n- charge(filenom): Charge un fichier dans le widget fparcourir\n- charge(chaine filename): charge un fichier xml\n- charge(chaine chemin): charge le fichier mp3.\n- charge(chaine chemin): Charge un fichier son.",
"colonne(","- colonne()|(nombre nb): Définit le nombre de colonnes",
"cellule(","- cellule(nombre R,nombre C): Renvoie la valeur de la cellule ligne R,colonne C",
"remplacetout(","- remplacetout(chaine s,chaine sub,bool casse): remplace toutes les occurrences de s avec sub selon la casse",
"cellulealignement(","- cellulealignement(alignement): Définit l'alignement interne des cellules",
"transmet(","- transmet(): Fin du mode différé. L'ensemble des commandes est alors transmis à la base de données pour exécution.",
"cellulealignemententêtecol(","- cellulealignemententêtecol(alignement): Définit l'alignement interne des cellules d'entête de colonne.\n- cellulealignemententêtecol(alignement): Définit l'alignement interne des cellules d'entête de ligne.",
"taillepolice(","- taillepolice(nombre c): définit ou renvoie la taille de la police du texte\n- taillepolice(nombre c)|(): définit ou renvoie la taille de la police du texte\n- taillepolice(nombre c): Définit ou renvoie la taille de la police du noeud.",
"typexml(","- typexml(): renvoie le type du noeud XML",
"couleuravantplan(","- couleuravantplan(nombre c): définit ou renvoie la couleur de la cellule en avant-plan\n- couleuravantplan(nombre c): Définit ou renvoie la couleur de l'avant-plan du noeud.",
"hauteurentêteligne(","- hauteurentêteligne(nombre hauteur): la taille en pixel de l'entête de la ligne",
"largeurentêtecolonne(","- largeurentêtecolonne(nombre largeur): la taille en pixel de l'entête de la colonne",
"insère(","- insère(chaine s,nombre p): insère s à la position p dans la fentrée\n- insère(l,label): Insère un label avant la ligne l\n- insère(fnoeudarbre e,chaine label,nombre pos): Insère un élément après e avec label à la position pos dans la liste des enfants.\n- insère(chaine table,chaine colonne,nombre valeur,...): insère une ligne dans une table.\rEx. mydb.insère('table1','nom',nm,'age',i);",
"sélection(","- sélection(): renvoie le texte sélectionné dans la fentrée\n- sélection(): renvoie le texte sélectionné dans l'éditeur ou les coordonnées de la sélection selon la variable de réception",
"mot(","- mot(nombre pos): Renvoie le mot à la position pos",
"largeurconnecteur(","- largeurconnecteur(nombre s): Définit ou renvoie là largeur du connecteur.",
"metstyle(","- metstyle(nombre start,nombre fin,chaine style): définit le style d'une portion de texte",
"rcherche(","- rcherche(chaine s,nombre i,bool casse): Cherche la position de la sous-chaine s à partir de i en arrière",
"sidéfilementvertical(","- sidéfilementvertical(fonction f,objet o): fonction de rappel pour un défilement vertical",
"sidéfilementhorizontal(","- sidéfilementhorizontal(fonction f,objet o): fonction de rappel pour un défilement horizontal",
"lestyle(","- lestyle(nombre start,nombre fin): renvoie une table de style pour chaque caractère de la section de texte",
"détruit(","- détruit(): Détruit le texte sélectionné depuis l'éditeur\n- détruit(): Détruit la representation interne du noeud XML",
"bornesligne(","- bornesligne(nombre l): renvoie le début et la fin de la position de la ligne courante du chariot ou renvoie les limites de la ligne  correspondant à une position",
"borneslignecarac(","- borneslignecarac(): renvoie le début et la fin de la position de la ligne courante du chariot ou renvoie les limites de la ligne  correspondant à une position en nombre de caractères",
"curseur(","- curseur(nombre l): renvoie la position du curseur courant du chariot ou définit la position du curseur",
"curseurcarac(","- curseurcarac(): renvoie la position du curseur courant du chariot en nombre de caractères",
"valigne(","- valigne(nombre l,bool surbrille): Va à la ligne l. Si vrai, met la ligne en  sur-brillance",
"ordretri(","- ordretri(nombre s): Définit ou renvoie l'ordre du tri.",
"annote(","- annote(chaine s|table v,chaine style,bool casse): Annote chaque occurence de s avec style",
"sautautomatique(","- sautautomatique(bool): Définit un mode coupure à la ligne automatique",
"désélectionne(","- désélectionne(): Désélectionne tous les noeuds\rdésélectionne(nombre i): Désélectionne le noeud i",
"annoteregexip(","- annoteregexip(chaine reg,chaine style): Annote chaque chaine correspondant à reg avec style",
"positionoctet(","- positionoctet(nombre i): Convertit une position en octet en une position en caractère",
"positioncarac(","- positioncarac(nombre i): Convertit une position en caractère en une position en octet",
"séparateurcolonne(","- séparateurcolonne(): Renvoie le caractère de séparation de colonnes.\rséparateurcolonne(chaine): Définit le caractère de séparation de colonnes",
"profondeur(","- profondeur(): Renvoie la profondeur du noeud.",
"nettoie(","- nettoie(): Retire l'objet associé.",
"estfermé(","- estfermé(): Renvoie vrai si l'élément est fermé.\n- estfermé(chaine path): Renvoie vrai si l'élément est fermé.\restfermé(fnoeudarbre e): Renvoie vrai si l'élément est fermé.",
"estouvert(","- estouvert(): Renvoie vrai si l'élément est ouvert.",
"estsélectionné(","- estsélectionné(): Renvoie vrai si l'élément est sélectionné.",
"estactif(","- estactif(): Renvoie vrai si l'élément est actif.",
"id(","- id(): Renvoie l'identifieur du noeud (uniquement disponible avec les fonctions de rappel)",
"parent(","- parent(): Renvoie le dernier élément.\n- parent(): renvoie le noeud XML parent",
"suivant(","- suivant(): Renvoie l'élément suivant.\n- suivant(fnoeudarbre e): Renvoie l'élément suivant après e.\n- suivant(): renvoie le noeud XML suivant",
"précédent(","- précédent(): Renvoie l'élément précédent.\n- précédent(fnoeudarbre e): Renvoie l'élément précédent avant e.\n- précédent(): Renvoie le noeud XML précédent",
"enfant(","- enfant(nombre i): Renvoie l'élément enfant à la position i.\n- enfant(): renvoie le noeud XML enfant",
"enfants(","- enfants(): Renvoie le nombre d'enfants.",
"désactive(","- désactive(): Désactive l'élément courant.",
"insèreavant(","- insèreavant(fnoeudarbre e,chaine label): Insère un élément au-dessus de e avec label.",
"styleconnecteur(","- styleconnecteur(nombre s): Définit ou renvoie le style du connecteur.",
"couleurnoeudarrièreplan(","- couleurnoeudarrièreplan(nombre c): Définit ou renvoie la couleur d'arrière-plan du noeud.",
"dernier(","- dernier(): Renvoie le dernier élément.",
"couleurnoeudavantplan(","- couleurnoeudavantplan(nombre c): Définit ou renvoie la couleur d'avant-plan du noeud.",
"taillenoeud(","- taillenoeud(nombre c): Définit ou renvoie la taille de la police du noeud.",
"margehaut(","- margehaut(nombre s): Définit ou récupère l'espace (en pixels) qui doit apparaitre entre la bordure du haut du widget et le contenu de l'arbre.",
"cliqué(","- cliqué(): Renvoie l'élément qui a été cliqué.",
"premier(","- premier(): Renvoie le premier élément.",
"commande(","- commande(chaine com,chaine sortiefichier): exécute la commande 'com'. sortiefichier est optionel et permet de rediriger la sortie vers un fichier.",
"mkdir(","- mkdir(chaine chemin): Crée le répertoire 'chemin'",
"listerépertoire(","- listerépertoire(chaine chemin): Retourne le contenu du répertoire",
"dictionaire infosfichier(","- dictionaire infosfichier(chaine chemin): Retourne les infos du fichier",
"env(","- env(chaine var)|(chaine var,chaine val): Retourne ou initialise le contenu de la variable d'environnement 'var'.",
"proxy(","- proxy(chaine prox): Connexion via un proxy",
"","- enregistreprédicat: enregistre un prédicat dans la base de données",
"url(","- url(chaine chemin): Charge une URL\rurl(chaine chemin,chaine fichier): Charge le contenu du site pointé par l'URL dans un fichier",
"nouveau(","- nouveau(chaine name): Crée un nouveau noeud XML",
"exécute(","- exécute(chaine nomfichier): Exécute une action curl, le fichier est optionnel.\n- exécute(chaine sqlcommande): exécute une commande sql brute sans retour.",
"options(","- options(chaine option,chaine parameter): Définit les options de curl",
"prédicat(","- prédicat(chaine nom, nombre arité): déclare un prédicat d'arité 'arité' dans la base de données",
"compteprédicat(","- compteprédicat(): compte les prédicats présents dans la base de données",
"retireprédicat(","- retireprédicat(): retire les prédicats de la base de données",
"insèrevia(","- insèrevia(string table,int v1,...): Insère une ligne dans un mode commence/transmet.\rEx. mydb.insèrevia('table1',nm,i);",
"mémoire(","- mémoire(): renvoie la consommation mémoire de SQLite.",
"décompose(","- décompose(chaine buffer): décompose un buffer xml",
"xpath(","- xpath(chaine myxpath): Renvoie une table de noeuds xml correspondant au chemin xpath myxpath",
"chainexml(","- chainexml(): Renvoie un document XML sous la forme d'une chaine\n- chainexml(): Retourne l'arbre complet depuis le noeud courant sous la forme d'une chaine de caractères.",
"sérialise(","- sérialise(objet): Sérialise un objet KiF sous la forme d'un document XML",
"délie(","- délie(): Retire un noeud de la structure arborée",
"nom(","- nom(): nom du noeud",
"propriétés(","- propriétés(): renvoie les attributs du noeud XML",
"contenu(","- contenu(): renvoie le contenu textuel du noeud XML",
"namespace(","- namespace(): Retourne la table de namespace du noeud xml",
"joue(","- joue(): joue le son\n- joue(bool raz): joue le son. si raz est vrai le flux repart à 0",
"interromps(","- interromps(): interromps le son\n- interromps(): interromps le son",
"paramètres(","- paramètres(): Renvoie les infos sous la forme d'un dicocn ou les modifie (uniquement le taux et le nombre de canaux",
"décode(","- décode(tablenombres ): décode un flux audio, remplit une tablenombres. Renvoie 'faux' quand le flux est fini (côté client)",
"encode(","- encode(tablenombres tn): joue un flux audio provenant d'un décode (côté serveur)","",""};
