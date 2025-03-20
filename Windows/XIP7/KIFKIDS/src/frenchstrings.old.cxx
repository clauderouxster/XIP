#include "kif.h"


Exported const char* kifwhile="tantque";
Exported const char* kiffor="pour";
Exported const char* kifif="si";
Exported const char* kifswitch="parmi";
Exported const char* kifdisplayln="afficheln";
Exported const char* kifdisplay="affiche";
Exported const char* kifelif="sinonsi";
Exported const char* kifelse="sinon";

#ifdef WITHWSTRING
Exported const wchar_t* wkifwhile=L"tantque";
Exported const wchar_t* wkiffor=L"pour";
Exported const wchar_t* wkifif=L"si";
Exported const wchar_t* wkifswitch=L"parmi";
Exported const wchar_t* wkifdisplayln=L"afficheln";
Exported const wchar_t* wkifdisplay=L"affiche";
Exported const wchar_t* wkifelif=L"sinonsi";
Exported const wchar_t* wkifelse=L"sinon";
#endif 


Exported const char* KIFSTRINGS[]={
"KIF(0): Mauvais nombre de paramètres pour ECLATE", //0 kif.cxx 79
"KIF(1): Mauvais troisième paramètre pour ECLATE", //1 kif.cxx 80
"KIF(2): Mauvais nombre de paramètres pour LISTE", //2 kif.cxx 81
"KIF(3): Mauvais nombre de paramètres pour UTF8", //3 kif.cxx 82
"KIF(4): Mauvais nombre de paramètres pour LATIN", //4 kif.cxx 83
"KIF(5): Mauvais nombre de paramètres pour GAUCHE", //5 kif.cxx 84
"KIF(6): Mauvais nombre de paramètres pour DROIT", //6 kif.cxx 85
"KIF(7): Mauvais nombre de paramètres pour MILIEU", //7 kif.cxx 86
"KIF(8): Mauvais nombre de paramètres pour ENMINUSCULE", //8 kif.cxx 87
"KIF(9): Mauvais nombre de paramètres pour ENMAJUSCULE", //9 kif.cxx 88
"KIF(10): Mauvais nombre de paramètres pour MINUSCULE", //10 kif.cxx 89
"KIF(11): Mauvais nombre de paramètres pour MAJUSCULE", //11 kif.cxx 90
"KIF(12): Mauvais nombre de paramètres pour ROGNE", //12 kif.cxx 91
"KIF(13): Mauvais nombre de paramètres pour METDATE", //13 kif.cxx 92
"KIF(14): Mauvais paramètre de temps pour METDATE", //14 kif.cxx 93
"KIF(15): Mauvais nombre de paramètres pour DATE", //15 kif.cxx 94
"KIF(16): Mauvais paramètre de temps pour DATE", //16 kif.cxx 95
"KIF(17): Mauvais nombre de paramètres pour ANNEE", //17 kif.cxx 96
"KIF(18): Mauvais paramètre de temps pour ANNEE", //18 kif.cxx 97
"KIF(19): Mauvais nombre de paramètres pour JOUR", //19 kif.cxx 98
"KIF(20): Mauvais paramètre de temps pour JOUR", //20 kif.cxx 99
"KIF(21): Mauvais nombre de paramètres pour HEURE", //21 kif.cxx 100
"KIF(22): Mauvais paramètre de temps pour HEURE", //22 kif.cxx 101
"KIF(23): Mauvais nombre de paramètres pour MOIS", //23 kif.cxx 102
"KIF(24): Mauvais paramètre de temps pour MOIS", //24 kif.cxx 103
"KIF(25): Paramètre manquant pour LEVE", //25 kif.cxx 104
"KIF(26): Mauvais nombre de paramètres pour ECRIT", //26 kif.cxx 105
"KIF(27): Le premier paramètre dans ECRIT devrait être un fichier", //27 kif.cxx 106
"KIF(28): Le premier paramètre dans LIT devrait être un fichier", //28 kif.cxx 107
"KIF(29): Le premier paramètre dans LITLIGNE devrait être un fichier", //29 kif.cxx 108
"KIF(30): Le premier paramètre dans FERME devrait être un fichier", //30 kif.cxx 109
"KIF(31): Le premier paramètre dans OUVREENLECTURE devrait être un fichier", //31 kif.cxx 110
"KIF(32): Erreur pendant l'ouverture du fichier en lecture:", //32 kif.cxx 111
"KIF(33): Le premier paramètre dans FINFICHIER devrait être un fichier", //33 kif.cxx 112
"KIF(34): Le premier paramètre dans OUVREENECRITURE devrait être un fichier", //34 kif.cxx 113
"KIF(35): Erreur pendant l'ouverture du fichier en écriture:", //35 kif.cxx 114
"KIF(36): Mauvais nombre de paramètres pour INSERE", //36 kif.cxx 115
"KIF(37): Mauvais appel de INSERE", //37 kif.cxx 116
"KIF(38): Fonction inconnue:", //38 kif.cxx 117
"KIF(39): Mauvais appel de EMPILE", //39 kif.cxx 118
"KIF(40): Mauvais nombre de paramètres pour dépile", //40 kif.cxx 119
"KIF(41): Mauvais appel de dépile", //41 kif.cxx 120
"KIF(42): Mauvais nombre de paramètres pour DERNIER", //42 kif.cxx 121
"KIF(43): Noeud inconnu pour DERNIER", //43 kif.cxx 122
"", //44 kif.cxx 123 Leave blank
"KIF(45): Mauvais appel de DERNIER", //45 kif.cxx 124
"KIF(46): Mauvais nombre de paramètres pour TRI", //46 kif.cxx 125
"KIF(47): Mauvais appel de TRI", //47 kif.cxx 126
"KIF(48): Paramètre manquants pour APPLIQUE", //48 kif.cxx 127
"KIF(49): Mauvais appel de APPLIQUE", //49 kif.cxx 128
"", //50 kif.cxx 129 Leave blank
"", //51 kif.cxx 130 Leave blank
"", //52 kif.cxx 131 Leave blank
"", //53 kif.cxx 132 Leave blank
"", //54 kif.cxx 133 Leave blank
"", //55 kif.cxx 134 Leave blank
"", //56 kif.cxx 135 Leave blank
"", //57 kif.cxx 136 Leave blank
"", //58 kif.cxx 137 Leave blank
"", //59 kif.cxx 138 Leave blank
"", //60 kif.cxx 139 Leave blank
"", //61 kif.cxx 140 Leave blank
"", //62 kif.cxx 141 Leave blank
"", //63 kif.cxx 142 Leave blank
"", //64 kif.cxx 143 Leave blank
"", //65 kif.cxx 144 Leave blank
"", //66 kif.cxx 145 Leave blank
"", //67 kif.cxx 146 Leave blank
"", //68 kif.cxx 147 Leave blank
"", //69 kif.cxx 148 Leave blank
"", //70 kif.cxx 149 Leave blank
"", //71 kif.cxx 150 Leave blank
"", //72 kif.cxx 151 Leave blank
"", //73 kif.cxx 152 Leave blank
"", //74 kif.cxx 153 Leave blank
"", //75 kif.cxx 154 Leave blank
"", //76 kif.cxx 155 Leave blank
"", //77 kif.cxx 156 Leave blank
"", //78 kif.cxx 157 Leave blank
"", //79 kif.cxx 158 Leave blank
"KIF(80): Mauvais nombre de paramètres pour paramètres", //80 kif.cxx 159
"KIF(81): Deuxieme paramètre devrait être un TABLEAU", //81 kif.cxx 160
"", //82 kif.cxx 161 Leave blank
"KIF(83): Pas de paramètre pour cette variable", //83 kif.cxx 162
"KIF(84): Mauvais nombre de paramètres pour RETIREPREMIER", //84 kif.cxx 163
"KIF(85): Pas de REMOVEFIRST pour cette variable", //85 kif.cxx 164
"KIF(86): Mauvais nombre de paramètres pour INSTANCES", //86 kif.cxx 165
"KIF(87): Pas de INSTANCES pour cette variable", //87 kif.cxx 166
"KIF(88): Mauvais nombre de paramètres pour MET", //88 kif.cxx 167
"KIF(89): Mauvais nombre de paramètres pour REGEX", //89 kif.cxx 168
"KIF(90): Classe manquante pour le MET de Weka", //90 kif.cxx 169
"KIF(91): Attribut manquant pour MET de Weka", //91 kif.cxx 170
"KIF(92): Pas de MET pour cette variable", //92 kif.cxx 171
"KIF(93): Mauvais nombre de paramètres pour RECUPERE", //93 kif.cxx 172
"KIF(94): Classe manquante pour RECUPERE de Weka", //94 kif.cxx 173
"KIF(95): Attribut manquant pour RECUPERE de Weka", //95 kif.cxx 174
"", //96 kif.cxx 175 Leave blank
"", //97 kif.cxx 176 Leave blank
"", //98 kif.cxx 177 Leave blank
"", //99 kif.cxx 178 Leave blank
"", //100 kif.cxx 179 Leave blank
"", //101 kif.cxx 180 Leave blank
"", //102 kif.cxx 181 Leave blank
"", //103 kif.cxx 182 Leave blank
"", //104 kif.cxx 183 Leave blank
"", //105 kif.cxx 184 Leave blank
"", //106 kif.cxx 185 Leave blank
"", //107 kif.cxx 186 Leave blank
"", //108 kif.cxx 187 Leave blank
"", //109 kif.cxx 188 Leave blank
"", //110 kif.cxx 189 Leave blank
"", //111 kif.cxx 190 Leave blank
"", //112 kif.cxx 191 Leave blank
"", //113 kif.cxx 192 Leave blank
"", //114 kif.cxx 193 Leave blank
"", //115 kif.cxx 194 Leave blank
"", //116 kif.cxx 195 Leave blank
"", //117 kif.cxx 196 Leave blank
"", //118 kif.cxx 197 Leave blank
"", //119 kif.cxx 198 Leave blank
"", //120 kif.cxx 199 Leave blank
"", //121 kif.cxx 200 Leave blank
"", //122 kif.cxx 201 Leave blank
"", //123 kif.cxx 202 Leave blank
"", //124 kif.cxx 203 Leave blank
"", //125 kif.cxx 204 Leave blank
"", //126 kif.cxx 205 Leave blank
"", //127 kif.cxx 206 Leave blank
"", //128 kif.cxx 207 Leave blank
"", //129 kif.cxx 208 Leave blank
"", //130 kif.cxx 209 Leave blank
"KIF(131): Paramètre inconnu pour CHARGE", //131 kif.cxx 210
"KIF(132): COMMENCE a échoué pour ITERATEUR", //132 kif.cxx 211
"KIF(133): FIN a échoué pour ITERATEUR", //133 kif.cxx 212
"KIF(134): NFIN a échoué pour ITERATEUR", //134 kif.cxx 213
"KIF(135): Pas de CLEF pour cet ITERATEUR", //135 kif.cxx 214
"KIF(136): Pas de VALEUR pour cet ITERATEUR", //136 kif.cxx 215
"KIF(137): Pas de TYPEVALEUR pour cet ITERATEUR", //137 kif.cxx 216
"", //138 kif.cxx 217 Leave blank
"", //139 kif.cxx 218 Leave blank
"", //140 kif.cxx 219 Leave blank
"", //141 kif.cxx 220 Leave blank
"", //142 kif.cxx 221 Leave blank
"", //143 kif.cxx 222 Leave blank
"", //144 kif.cxx 223 Leave blank
"", //145 kif.cxx 224 Leave blank
"", //146 kif.cxx 225 Leave blank
"", //147 kif.cxx 226 Leave blank
"", //148 kif.cxx 227 Leave blank
"", //149 kif.cxx 228 Leave blank
"", //150 kif.cxx 229 Leave blank
"", //151 kif.cxx 230 Leave blank
"", //152 kif.cxx 231 Leave blank
"", //153 kif.cxx 232 Leave blank
"KIF(154): mauvais type pour ECRIT", //154 kif.cxx 233
"KIF(155): Mauvaise affectation d'une classe", //155 kif.cxx 234
"KIF(156): Mauvaise affectation d'un APPEL", //156 kif.cxx 235
"", //157 kif.cxx 236 Leave blank
"KIF(158): Veuillez vérifier le nombre de paramètres pour:", //158 kif.cxx 237
"KIF(159): Veuillez vérifier le paramètre pour:", //159 kif.cxx 238
"KIF(160): Mauvaise initialisation d'une classe", //160 kif.cxx 239
"KIF(161): Mauvais nombre de paramètres pour POSITIONNER", //161 kif.cxx 240
"KIF(162): Le premier paramètre devrait être un fichier pour POSITIONNER", //162 kif.cxx 241
"KIF(163): Mauvais nombre de paramètres pour DIT", //163 kif.cxx 242
"KIF(164): Le premier paramètre devrait être un fichier pour DIT", //164 kif.cxx 243
"KIF(165): Mauvais nombre de paramètres pour JOINT", //165 kif.cxx 244
"KIF(166): Mauvais paramètre pour JOINT", //166 kif.cxx 245
"KIF(167): Mauvais nombre de paramètres pour COMPILE", //167 kif.cxx 246
"KIF(168): Mauvais paramètre pour COMPILE", //168 kif.cxx 247
"KIF(169): Mauvais nombre de paramètres pour SOMME", //169 kif.cxx 248
"KIF(170): Mauvais paramètre pour SOMME", //170 kif.cxx 249
"KIF(171): Mauvais nombre de paramètres pour PRODUIT", //171 kif.cxx 250
"KIF(172): Mauvais paramètre pour PRODUIT", //172 kif.cxx 251
"KIF(173): Mauvais nombre de paramètres pour REMET", //173 kif.cxx 252
"KIF(174): Pas de REMET pour cette variable", //174 kif.cxx 253
"KIF(175): Mauvais nombre de paramètres pour INVERSE", //175 kif.cxx 254
"KIF(176): Mauvais nombre de paramètres pour EXTRAIT", //176 kif.cxx 255
"KIF(177): Mauvais paramètre pour EXTRAIT", //177 kif.cxx 256
"KIF(178): Fonction inconnue:", //178 kif.cxx 257
"", //179 kif.cxx 258 Leave blank
"", //180 kif.cxx 259 Leave blank
"", //181 kif.cxx 260 Leave blank
"", //182 kif.cxx 261 Leave blank
"KIF(183): Instance inconnue:", //183 kif.cxx 262
"KIF(184): Erreur de chargement:", //184 kif.cxx 263
"KIF(185): Paramètre manquant pour HACHAGE", //185 kif.cxx 264
"KIF(186): Mauvais nombre de paramètres pour VALEURS", //186 kif.cxx 265
"KIF(187): Mauvais type de paramètre pour VALEURS", //187 kif.cxx 266
"KIF(188): Mauvais nombre de paramètre ou mauvais type pour ECLATERGX", //188 kif.cxx 267
"KIF(189): Le chargement de la bibliotheque a echoué:", //189 kif.cxx 268
"KIF(190): Point d'entrée manquant dans KifInitialisationModule dans:", //190 kif.cxx 269
"KIF(191): Mauvais nombre de paramètres pour INFOSFICHIER", //191 kif.cxx 270
"KIF(192): Mauvais nombre de paramètres pour CLEFS", //192 kif.cxx 271
"KIF(193): Mauvais type de paramètre pour CLEFS", //193 kif.cxx 272
"KIF(194): Mauvais nombre de paramètres pour CHERCHE", //194 kif.cxx 273
"KIF(195): Mauvais nombre de paramètres pour COMP", //195 kif.cxx 274
"KIF(196): Mauvais type de paramètres dans:", //196 kif.cxx 275
"KIF(197): Mauvais nombre de paramètres pour TUE", //197 kif.cxx 276
"KIF(198): Mauvais nombre de paramètres pour ENVOIE", //198 kif.cxx 277
"KIF(199): Mauvais nombre de paramètres pour ATTEND", //199 kif.cxx 278
"KIF(200): Mauvais nombre de paramètres pour ENATTENTE", //200 kif.cxx 279
"KIF(201): Mauvais paramètre: élément arbre attendu", //201 kif.cxx 280
"KIF(202): Paramètre ARBRE manquant", //202 kif.cxx 281
"KIF(203): Mauvais nombre de paramètres", //203 kif.cxx 282
"KIF(204): Erreur: le paramètre ARBRE est un  parent de la cible ARBRE", //204 kif.cxx 283
"KIF(205): Vous ne pouvez pas bouger un noeud vers lui-même", //205 kif.cxx 284
"KIF(206): On ne peut pas initialiser un tableau avec cette valeur", //206 kif.cxx 285
"KIF(207): On ne peut pas initialiser un dictionnaire avec cette valeur", //207 kif.cxx 286
"KIF(208): Mauvais nombre de paramètres pour REMPLIT", //208 kif.cxx 287
"KIF(209): Taille trop longue pour REMPLIT", //209 kif.cxx 288
"KIF(210): Mauvais paramètre initial pour REMPLIT", //210 kif.cxx 289
"KIF(211): On ne peut créer une fonction TÂCHE", //211 kif.cxx 290
"KIF(212): Seule une chaine ATTEND est acceptée dans une TÂCHE", //212 kif.cxx 291
"", //213 kif.cxx 292 Leave blank
"KIF(214): TOUCHE n'accepte aucun paramètre", //214 kif.cxx 293
"", //215 kif.cxx 294 Leave blank
"", //216 kif.cxx 295 Leave blank
"KIF(217): ATTEND est interdit dans une section CAPTE", //217 kif.cxx 296
"KIF(218): Mauvais nombre de paramètres pour RAZ", //218 kif.cxx 297
"KIF(219): Mauvais paramètre pour RAZ", //219 kif.cxx 298
"KIF(220): On ne peut pas initialiser une liste avec cette valeur", //220 kif.cxx 299
"KIF(221): Session déjà ouverte", //221 kif.cxx 300
"KIF(222): Session inconnue", //222 kif.cxx 301
"KIF(223): Clef inconnu lors d'un accès conteneur", //223 kif.cxx 302
"", //224 kif.cxx 303 Leave blank
"KIF(225): Division par 0", //225 kif.cxx 304
"KIF(226): Mauvais nombre de paramètres pour INTERVALLE", //226 kif.cxx 305
"KIF(227): INTERVALLE au-delà de sa capacite", //227 kif.cxx 306
"KIF(228): Mauvais nombre de paramètres ou mauvais type pour FORMAT", //228 kif.cxx 307
"", //229 kif.cxx 308 Leave blank
"KIF(230): La chaine devrait être longue d'au moins un caractère pour INTERVALLE", //230 kif.cxx 309
"KIF(231): Pas de fonction compatible trouvée ou fonction inconnue:", //231 kif.cxx 310
"KIF(232): Variable APPEL vide", //232 kif.cxx 311
"KIF(233): Objet inconnu:", //233 kif.cxx 312
"KIF(234): Objet existe déjà:", //234 kif.cxx 313
"KIF(235): Mauvaise utilisation de 'ici':", //235 kif.cxx 314
"KIF(236): Mauvais appel de fonction pour APPLIQUE", //236 kif.cxx 315
"KIF(237): Paramètre manquant pour SYNCHRONISEE", //237 kif.cxx 316
"KIF(238): Paramètre manquant pour VERROUILLE", //238 kif.cxx 317
"KIF(239): Paramètre manquant pour DEVERROUILLE", //239 kif.cxx 318
"KIF(240): Verrou inconnu:", //240 kif.cxx 319
"KIF(241): Mauvaise utilisation de RENVOIE", //241 kif.cxx 320
"KIF(242): Mauvaise définition de comparaison", //242 kif.cxx 321
"KIF(243): Instruction inconnue pour cet élément", //243 kif.cxx 322
"KIF(244): Expression inconnue", //244 kif.cxx 323
"KIF(245): Paramètre inconnu pour FICHIER: devrait être 'l','e', 'e+' ou 'a'", //245 kif.cxx 324
"KIF(246): Mauvais nombre de paramètre ou mauvais type pour EXPOSEE", //246 kif.cxx 325
"KIF(247): Paramètre inconnu pour EXPOSEE", //247 kif.cxx 326
"KIF(248): Mauvais nombre de paramètre ou mauvais type pour TEST", //248 kif.cxx 327
"KIF(249): Mauvais nombre de paramètre ou mauvais type pour _ID", //249 kif.cxx 328
"KIF(250): Une constante ne peut être modifiée", //250 kif.cxx 329
"KIF(251): Mauvais nombre de paramètres ou mauvais type pour POSITIONOCTET", //251 kif.cxx 330
"KIF(252): Mauvais nombre de paramètres ou mauvais type pour ESTUTF8", //252 kif.cxx 331
"KIF(253): Mauvais nombre de paramètre pour ECRITUREBIN", //253 kif.cxx 332
"KIF(254): Code non ASCII pour ECRITUREBIN", //254 kif.cxx 333
"KIF(255): Mauvais nombre de paramètres pour LIT", //255 kif.cxx 334
"KIF(256): Fonction externe inconnue", //256 kif.cxx 335
"KIF(257): Mauvais nombre de paramètres pour:", //257 kif.cxx 336
"KIF(258): FONCTIONDEBUG exige une fonction comme paramètre:", //258 kif.cxx 337
"KIF(259): Mauvais index de pile:", //259 kif.cxx 338
"KIF(260): Objet KiF attendu", //260 kif.cxx 339
"KIF(261): Objet KiF vide:", //261 kif.cxx 340
"KIF(262): Mauvais nombre de paramètres pour:", //262 kif.cxx 341
"KIF(263): Ne peut sauvegarder cette valeur: référence infinie", //263 kif.cxx 342
"KIF(264): fonction d'appel vide", //264 kif.cxx 343
"KIF(265): Valeur 'vide' ne peut être sauvegardée", //265 kif.cxx 344
"curseur", //266 kif.cxx 504
"curseurmax", //267 kif.cxx 505
"ramassemiette", //268 kif.cxx 506
"curseurchaines", //269 kif.cxx 507
"curseurnombres", //270 kif.cxx 508
"curseurréels", //271 kif.cxx 509
"curseurtableaux", //272 kif.cxx 510
"curseurdictionnaires", //273 kif.cxx 511
"curseurlistes", //274 kif.cxx 512
"ramassechaines", //275 kif.cxx 513
"ramassenombres", //276 kif.cxx 514
"ramasseréels", //277 kif.cxx 515
"ramassetableaux", //278 kif.cxx 516
"ramassedictionnaires", //279 kif.cxx 517
"ramasselistes", //280 kif.cxx 518
"/kif_%d", //281 kif.cxx 793     <------- Provide a translation
"Max:", //282 kif.cxx 1221
"Min:", //283 kif.cxx 1222
"Compte:", //284 kif.cxx 1223
"Moyenne:", //285 kif.cxx 1224
"segmente", //286 kif.cxx 1489
"segmente(bool virgule,bool séparateur): Segmente un texte en mots et ponctuation. Si virgule vaut 'vrai' alors le séparateur décimal est une ',' sinon un '.'. Si 'séparateur' alors '3.000,10' est accepté comme nombre.", //287 kif.cxx 1489
"éclate", //288 kif.cxx 1490
"éclate(chaine exploseur,tableau vect): explose une chaine selon exploseur et enregistre le résultat sous la forme d'un tableau. Si exploseur=='', alors la chaine est explosée en un tableau de caractères", //289 kif.cxx 1490
"retirepremier", //290 kif.cxx 1491
"retirepremier(nombre nb): retire les nb premiers caractères d'une chaine", //291 kif.cxx 1491
"retiredernier", //292 kif.cxx 1492
"retiredernier(nombre nb): retire les nb derniers caractères d'une chaine", //293 kif.cxx 1492
"remplit", //294 kif.cxx 1493
"remplit(nombre nb,chaine c): crée une chaine de nb caractères c", //295 kif.cxx 1493
"ord", //296 kif.cxx 1494
"ord(): renvoie le code ASCII du caractère en tête, or une liste de tous les codes ASCII si le receveur est un tableau", //297 kif.cxx 1494
"gauche", //298 kif.cxx 1495
"gauche(nombre nb): renvoie les nb premiers caractères d'une chaine", //299 kif.cxx 1495
"positionoctet(nombre pos)", //300 kif.cxx 1496
"Convertit une position caractère en une position octet", //301 kif.cxx 1496
"positioncar(nombre pos)", //302 kif.cxx 1497
"Convertit une position octet en une position caractère", //303 kif.cxx 1497
"droit", //304 kif.cxx 1498
"droit(nombre nb): renvoie les nb derniers caractères d'une chaine", //305 kif.cxx 1498
"milieu", //306 kif.cxx 1499
"milieu(nombre pos,nombre nb): renvoie les nb caractères commençant à la position pos d'une chaine", //307 kif.cxx 1499
"renverse", //308 kif.cxx 1500
"renverse(): inverse la chaine", //309 kif.cxx 1500
"dépile", //310 kif.cxx 1501
"dépile(i): retire le caractère à la position i", //311 kif.cxx 1501
"dépile(): retire le dernier caractère", //312 kif.cxx 1502
"dernier", //313 kif.cxx 1503
"dernier(): renvoie le dernier caractère", //314 kif.cxx 1503
"insère", //315 kif.cxx 1504
"insère(i,s): insère la chaine s en position 'i'", //316 kif.cxx 1504
"rogne", //317 kif.cxx 1505
"rogne(): retire les caractères d'espace", //318 kif.cxx 1505
"rognedroit", //319 kif.cxx 1506
"rognedroit(): retire les caractères d'espace à droite", //320 kif.cxx 1506
"rognegauche", //321 kif.cxx 1507
"rognegauche(): retire les caractères d'espace à gauche", //322 kif.cxx 1507
"majuscule", //323 kif.cxx 1508
"majuscule(): Met la chaine en majuscule", //324 kif.cxx 1508
"minuscule", //325 kif.cxx 1509
"minuscule(): Met la chaine en minuscule", //326 kif.cxx 1509
"enminuscule", //327 kif.cxx 1510
"enminuscule(): Teste si la chaine est uniquement en minuscule", //328 kif.cxx 1510
"enmajuscule", //329 kif.cxx 1511
"enmajuscule(): Teste si la chaine est uniquement en minuscule", //330 kif.cxx 1511
"estalpha", //331 kif.cxx 1512
"estalpha(): Teste si une chaine ne contient que des caractères alphabétiques", //332 kif.cxx 1512
"extrait", //333 kif.cxx 1513
"tableau v=extrait(nombre pos,chaine depuis,chaine c1,chaine c2...): extrait les sous-chaines entre 'depuis' et 'c1'...'cn' (la chaine la plus courte est utilisée). Renvoie un tableau de chaines", //334 kif.cxx 1513
"estchiffre", //335 kif.cxx 1514
"estchiffre(): Teste si une chaine ne contient que des chiffres", //336 kif.cxx 1514
"latin", //337 kif.cxx 1515
"latin(): convertit une chaine UTF8 en LATIN", //338 kif.cxx 1515
"utf8", //339 kif.cxx 1516
"utf8(): convertit une chaine en LATIN en UTF8", //340 kif.cxx 1516
"estutf8", //341 kif.cxx 1517
"estutf8(): Renvoie vrai si la chaine est encodée en UTF8", //342 kif.cxx 1517
"cherche", //343 kif.cxx 1518
"cherche(chaine sub,nombre pos): Renvoie la position de la sous-chaine sub commençant à la position pos", //344 kif.cxx 1518
"rcherche", //345 kif.cxx 1519
"rcherche(chaine sub,nombre pos): Renvoie la position de la sous-chaine sub par l'arrière commençant à la position pos", //346 kif.cxx 1519
"remplaceregexip", //347 kif.cxx 1520
"remplaceregexip(sub,str): Remplace les chaines correspondant à sub avec str", //348 kif.cxx 1520
"évalue", //349 kif.cxx 1521
"évalue(): Evalue les métacaractères dans une chaine et retourne la chaine évaluée", //350 kif.cxx 1521
"html", //351 kif.cxx 1522     <------- Provide a translation
"html(): Return the string into an HTML compatible string or as a vector of strings", //352 kif.cxx 1522     <------- Provide a translation
"regexip", //353 kif.cxx 1523
"regexip(sub): Trouve les chaines correspondant à sub", //354 kif.cxx 1523
"remplace", //355 kif.cxx 1524
"remplace(sub,str): Remplace la sous-chaines correspondant à sub avec str", //356 kif.cxx 1524
"captestd", //357 kif.cxx 1525
"captestd(bool): capte ou relâche la sortie standard", //358 kif.cxx 1525
"capteerr", //359 kif.cxx 1526
"capteerr(bool): capte ou relâche la sortie erreur", //360 kif.cxx 1526
"remplacergx", //361 kif.cxx 1527
"remplacergx(rgx,str): Remplace la sous-chaine correspondant à rgx avec str", //362 kif.cxx 1527
"éclatergx", //363 kif.cxx 1528
"éclatergx(rgx): Explose  chaine avec l'expression regulière rgx. Renvoie un tableau de sous-chaines.", //364 kif.cxx 1528
"boolean=regex", //365 kif.cxx 1529
"boolean=regex(rgx): Teste si l'expression regulière rgx s'applique à la chaine", //366 kif.cxx 1529
"tableau=regex", //367 kif.cxx 1530
"tableau=regex(rgx): Renvoie toutes les sous-chaines correspondant à rgx", //368 kif.cxx 1530
"chaine=regex", //369 kif.cxx 1531
"chaine=regex(rgx): Renvoie la sous-chaine correspondant à rgx", //370 kif.cxx 1531
"nombre=regex", //371 kif.cxx 1532
"nombre=regex(rgx): Renvoie la position de la sous-chaine correspondant à rgx", //372 kif.cxx 1532
"log", //373 kif.cxx 1533
"log(): renvoie le log base 10 de la valeur", //374 kif.cxx 1533
"ln", //375 kif.cxx 1535
"ln(): renvoie le log neperien", //376 kif.cxx 1535
"abs", //377 kif.cxx 1536
"abs(): renvoie la valeur absolue", //378 kif.cxx 1536
"exp", //379 kif.cxx 1539
"exp(): renvoie l'exponentiel", //380 kif.cxx 1539
"racar", //381 kif.cxx 1541
"racar(): renvoie la racine carrée", //382 kif.cxx 1541
"commute", //383 kif.cxx 1543
"commute(): Inverse le dénominateur et le numérateur de la fraction", //384 kif.cxx 1543
"partie", //385 kif.cxx 1545
"partie(bits x): Renvoie vrai si le bits de x sont tous dans les bits courants", //386 kif.cxx 1545
"d(): Dénominateur de la fraction (avec un paramètre: met la valeur, sans paramètre renvoie la valeur)", //387 kif.cxx 1547
"infos", //388 kif.cxx 1548
"infos(chaine): infos à propos d'une méthode", //389 kif.cxx 1548
"méthodes", //390 kif.cxx 1549
"méthodes(): Retourne a tableau de toutes les méthodes disponibles", //391 kif.cxx 1549
"simplifie", //392 kif.cxx 1550
"simplifie(): Simplification", //393 kif.cxx 1550
"n(): Numérateur de la fraction (avec un paramètre: met la valeur, sans paramètre renvoie la valeur)", //394 kif.cxx 1552
"nd", //395 kif.cxx 1553
"nd(nombre n,nombre d): Initialise le Numérateur et le Dénominateur de la fraction", //396 kif.cxx 1553
"tan", //397 kif.cxx 1554
"tan(): tangente", //398 kif.cxx 1554
"car", //399 kif.cxx 1556
"car(): Renvoie le caractère dont le code est la valeur de la variable", //400 kif.cxx 1556
"sin", //401 kif.cxx 1558
"sin(): sinus", //402 kif.cxx 1558
"cos", //403 kif.cxx 1560
"cos(): cosinus", //404 kif.cxx 1560
"atan", //405 kif.cxx 1562
"atan(): arc tangent", //406 kif.cxx 1562
"#(): renvoie le complément en bits", //407 kif.cxx 1564
"format", //408 kif.cxx 1566
"format(chaine form): renvoie une chaine formatée selon le patron dans form. (ce format est le même que celui de sprintf en C++)", //409 kif.cxx 1566
"lire", //410 kif.cxx 1568
"lire(): Lit une chaine depuis le clavier", //411 kif.cxx 1568
"octets", //412 kif.cxx 1569
"octets(): Renvoie la chaine comme tableau d'octets", //413 kif.cxx 1569
"lire(): Lit une chaine depuis le clavier", //414 kif.cxx 1570
"empile", //415 kif.cxx 1573
"empile(a): rajoute en queue de tableau", //416 kif.cxx 1573
"fusionne", //417 kif.cxx 1574
"fusionne(v): fusionne les éléments de v dans le conteneur", //418 kif.cxx 1574
"octets(): Renvoie la chaine correspondant à un tableau d'octets. Chaque valeur doit être comprise entre 0..255", //419 kif.cxx 1575
"teste", //420 kif.cxx 1576
"teste(i): teste si i est une position valide dans le tableau", //421 kif.cxx 1576
"dépile(nombre i): retire le ieme élément du tableau.", //422 kif.cxx 1577
"dépile(): retire le dernier élément du tableau.", //423 kif.cxx 1578
"dernier(): renvoie le dernier élément du tableau", //424 kif.cxx 1579
"inverse(): inverse l'ordre des éléments dans le tableau", //425 kif.cxx 1580
"joint", //426 kif.cxx 1581
"joint(chaine sep): concatène chaque élément dans le tableau en une chaine ou chaque élément est séparé des autres par sep", //427 kif.cxx 1581
"insère(i,x): insère l'élément x à la position i", //428 kif.cxx 1582
"somme", //429 kif.cxx 1583
"somme(): Somme chaque élément", //430 kif.cxx 1583
"produit", //431 kif.cxx 1584
"produit(): Multiplie chaque élément avec les autres", //432 kif.cxx 1584
"raz", //433 kif.cxx 1585
"raz(): nettoie le tableau", //434 kif.cxx 1585
"min", //435 kif.cxx 1586
"min(): renvoie la valeur minimale dans un conteneur", //436 kif.cxx 1586
"max", //437 kif.cxx 1587
"max(): renvoie la valeur maximale dans un conteneur", //438 kif.cxx 1587
"trie", //439 kif.cxx 1588
"trie(compare): Trie le contenu du  tableau selon une fonction de comparaison, laquelle est optionnelle", //440 kif.cxx 1588
"trienombre", //441 kif.cxx 1589
"trienombre(): trie le contenu du tableau dont chaque élément est considéré comme un nombre.", //442 kif.cxx 1589
"trieréel", //443 kif.cxx 1590
"trieréel(): trie le contenu du tableau dont chaque élément est considéré comme un réel.", //444 kif.cxx 1590
"trielong", //445 kif.cxx 1591
"trielong(): trie le contenu du tableau dont chaque élément est considéré comme un long.", //446 kif.cxx 1591
"triechaine", //447 kif.cxx 1592
"triechaine(): trie le contenu du tableau dont chaque élément est considéré comme une chaine.", //448 kif.cxx 1592
"applique", //449 kif.cxx 1593
"applique(a,b,c): applique sur chaque élément une fonction, ou applique toutes les fonctions dans le conteneur", //450 kif.cxx 1593
"empileentête", //451 kif.cxx 1594
"empileentête(a): ajoute au début de la liste", //452 kif.cxx 1594
"test(i): teste si i est une position valide dans la liste", //453 kif.cxx 1595
"empileenqueue", //454 kif.cxx 1597
"empileenqueue(a): ajoute en queue de la liste", //455 kif.cxx 1597
"dépileentête", //456 kif.cxx 1598
"dépileentête(): retire l'élément en tête de la liste et le renvoie.", //457 kif.cxx 1598
"dépileenqueue", //458 kif.cxx 1601
"dépileenqueue(): retire le dernier élément de la liste et le renvoie.", //459 kif.cxx 1601
"dernier(): renvoie le dernier élément de la liste", //460 kif.cxx 1602
"premier", //461 kif.cxx 1603
"dernier(): renvoie le dernier élément de la liste", //462 kif.cxx 1603
"inverse(): inverse l'ordre des éléments dans la liste", //463 kif.cxx 1604
"joint(chaine sep): concatène chaque élément dans la liste en une chaine ou chaque élément est séparé des autres par sep", //464 kif.cxx 1605
"applique(a,b,c): applique sur chaque élément une fonction, ou applique toutes les fonctions dans le conteneur.", //465 kif.cxx 1609
"raz(): nettoie la liste", //466 kif.cxx 1610
"joint(chaine clefsep,chaine sep): concatène chaque élément du dictionnaire en une chaine ou chaque clef est ajoutée avec clefsep et chaque élément est séparé des autres par sep", //467 kif.cxx 1613
"teste(i): teste si i est une clef valide dans le dictionnaire", //468 kif.cxx 1617
"dépile(chaine clef): retire l'élément correspondant à clef", //469 kif.cxx 1618
"clefs", //470 kif.cxx 1619
"clefs(): renvoies les clefs du dictionnaire comme tableau", //471 kif.cxx 1619
"valeurs", //472 kif.cxx 1620
"valeurs(): renvoie les valeurs du dictionnaire comme un tableau", //473 kif.cxx 1620
"raz(): nettoie le dictionnaire", //474 kif.cxx 1623
"arbre", //475 kif.cxx 1625
"arbre(valeur): crée un noeud d'arbre, avec valeur comme valeur. Les valeurs peuvent avoir n'importe quel type", //476 kif.cxx 1625
"arbre n=100", //477 kif.cxx 1626
"arbre n=100: modifie la valeur du noeud d'arbre avec n'importe quoi, ici avec un nombre, mais ce pourrait être n'importe quel objet.", //478 kif.cxx 1626
"noeudsuivant", //479 kif.cxx 1627
"noeudsuivant(): renvoie le noeud suivant", //480 kif.cxx 1627
"noeudprécédent", //481 kif.cxx 1628
"noeudprécédent(): renvoie le noeud précédent", //482 kif.cxx 1628
"noeudenfant", //483 kif.cxx 1629
"noeudenfant(): renvoie le premier noeud enfant", //484 kif.cxx 1629
"noeudparent", //485 kif.cxx 1630
"noeudparent(): renvoie le noeud parent", //486 kif.cxx 1630
"derniernoeud", //487 kif.cxx 1631
"derniernoeud(): renvoie le dernier noeud frère", //488 kif.cxx 1631
"noeudsuivant(arbre n): compare le noeud suivant avec n", //489 kif.cxx 1632
"noeudprécédent(arbre n): compare le noeud précédent avec n", //490 kif.cxx 1633
"noeudenfant(arbre n): Teste si le noeud courant est un enfant de n", //491 kif.cxx 1634
"noeudparent(arbre n): Teste si le noeud courant est un parent de n", //492 kif.cxx 1635
"derniernoeud(arbre n): Teste si le noeud courant est le dernier enfant de n", //493 kif.cxx 1636
"ajoutenoeudenfant", //494 kif.cxx 1637
"ajoutenoeudenfant(arbre n): Ajoute n sous le noeud courant", //495 kif.cxx 1637
"ajoutenoeudsuivant", //496 kif.cxx 1638
"ajoutenoeudsuivant(arbre n): Ajoute n après le noeud courant", //497 kif.cxx 1638
"ajoutenoeudprécédent", //498 kif.cxx 1639
"ajoutenoeudprécédent(arbre n): Ajoute n avant le noeud courant", //499 kif.cxx 1639
"isolenoeud", //500 kif.cxx 1640
"isolenoeud(): Extrait le noeud courant de l'arbre", //501 kif.cxx 1640
"retirenoeud", //502 kif.cxx 1641
"retirenoeud(): Détruit le sous-arbre", //503 kif.cxx 1641
"profondeur", //504 kif.cxx 1642
"profondeur(): Renvoie la profondeur du noeud dans le arbre", //505 kif.cxx 1642
"instances", //506 kif.cxx 1643
"instances(): renvoie le nombre d'instances dans le weka", //507 kif.cxx 1643
"place", //508 kif.cxx 1644
"place(nombre instance,chaine class,chaine attribut, valeur): Ajoute un élément avec une valeur d'instance de classe,selon un attribut et une instance Id.", //509 kif.cxx 1644
"récupère(nombre instance,chaine class,chaine attribut): Renvoie l'élément avec une valeur de classe d'instance spécifique selon un attribut et une instance Id.", //510 kif.cxx 1645
"sauvegarde", //511 kif.cxx 1647
"sauve(filename,type): sauve le weka sur disque. type peut prendre deux valeurs: aarf ou xml", //512 kif.cxx 1647
"valeur", //513 kif.cxx 1648
"valeur(): renvoie la valeur de l'élément courant", //514 kif.cxx 1648
"clef", //515 kif.cxx 1649
"clef(): renvoie la clef de l'élément courant", //516 kif.cxx 1649
"valeurtype", //517 kif.cxx 1650
"valeurtype(): renvoie le type de la valeur de l'élément courant", //518 kif.cxx 1650
"estvaleurtype", //519 kif.cxx 1651
"estvaleurtype(chaine type): teste le type de l'élément courant", //520 kif.cxx 1651
"commence", //521 kif.cxx 1652
"commence(): initialise l'itérateur avec le début de la collection", //522 kif.cxx 1652
"suivant", //523 kif.cxx 1653
"suivant(): élément suivant dans la collection", //524 kif.cxx 1653
"fin", //525 kif.cxx 1654
"fin(): renvoie vrai quand la fin de la collection est atteinte", //526 kif.cxx 1654
"nfin", //527 kif.cxx 1655
"nfin(): renvoie vrai tant que la fin de la collection n'a pas été atteinte (~fin())", //528 kif.cxx 1655
"applique(a,b,c): applique une fonction à tous les éléments du conteneur ou toutes les fonctions dans le conteneur", //529 kif.cxx 1656
"metdate", //530 kif.cxx 1657
"metdate(année,mois,jour,heure,min,sec): initialise une variable de temps", //531 kif.cxx 1657
"date", //532 kif.cxx 1658
"date(): renvoie la date comme chaine", //533 kif.cxx 1658
"année", //534 kif.cxx 1659
"année(): renvoie l'année comme un nombre", //535 kif.cxx 1659
"mois", //536 kif.cxx 1660
"mois(): renvoie le mois comme un nombre", //537 kif.cxx 1660
"jour", //538 kif.cxx 1661
"jour(): renvoie le jour comme un nombre", //539 kif.cxx 1661
"heure", //540 kif.cxx 1662
"heure(): renvoie l'heure comme un nombre", //541 kif.cxx 1662
"minute", //542 kif.cxx 1663
"minute(): renvoie les minutes comme un nombre", //543 kif.cxx 1663
"seconde", //544 kif.cxx 1664
"seconde(): renvoie les secondes comme un nombre", //545 kif.cxx 1664
"raz", //546 kif.cxx 1665
"raz(): réinitialise une variable de temps", //547 kif.cxx 1665
"ouvreenajout", //548 kif.cxx 1666
"ouvreenajout(chaine fichiername): ouvre un fichier en ajout", //549 kif.cxx 1666
"ouvreenécriture", //550 kif.cxx 1667
"ouvreenécriture(chaine fichiername): ouvre un fichier en écriture", //551 kif.cxx 1667
"ouvreenlecture", //552 kif.cxx 1668
"ouvreenlecture(chaine fichiername): ouvre un fichier en lecture", //553 kif.cxx 1668
"écrit", //554 kif.cxx 1669
"écrit(chaine s1,chaine s2,): écrit les chaines dans le fichier", //555 kif.cxx 1669
"écritbin", //556 kif.cxx 1670
"écrit(nombre s1,nombre s2,): écrit les codes de caractères dans le fichier", //557 kif.cxx 1670
"litun(): lecture d'un caractère depuis le fichier", //558 kif.cxx 1671
"remet", //559 kif.cxx 1672
"remet(): Retourne le caractère dans le flux de lecture", //560 kif.cxx 1672
"remet(nb): renvoie nb caractères dans le flux", //561 kif.cxx 1673
"lit", //562 kif.cxx 1674
"lit(): lit une ligne depuis un fichier", //563 kif.cxx 1674
"litln", //564 kif.cxx 1675
"litln(): lit une ligne depuis un fichier", //565 kif.cxx 1675
"positionne", //566 kif.cxx 1676
"positionne(nombre p): positionne le curseur du fichier à p", //567 kif.cxx 1676
"dit", //568 kif.cxx 1677
"dit(): renvoie la position du  curseur courant dans le fichier", //569 kif.cxx 1677
"findefichier", //570 kif.cxx 1678
"findefichier(): renvoie vrai quand la fin de fichier est atteinte", //571 kif.cxx 1678
"charge", //572 kif.cxx 1681
"charge(chaine kifpathname): charge un programme KiF", //573 kif.cxx 1681
"compile", //574 kif.cxx 1682
"compile(chaine kifpathname): Compile un programme KiF sous la forme d'une chaine. Renvoie un identifiant sur la première instruction à exécuter", //575 kif.cxx 1682
"exécute", //576 kif.cxx 1683
"exécute(nombre i): Exécute un programme depuis l'instruction i (renvoyé par compile)", //577 kif.cxx 1683
"kvariables", //578 kif.cxx 1684
"kvariables(): Renvoie les variables actives sous la forme d'un dictionnaire", //579 kif.cxx 1684
"exécutetâche", //580 kif.cxx 1685
"exécutetâche(nombre i): Exécute en parallèle un programme compilé depuis l'identifiant renvoyé", //581 kif.cxx 1685
"finexécution", //582 kif.cxx 1686
"finexécution(): Renvoie vrai si le programme s'est entièrement exécuté", //583 kif.cxx 1686
"fonctiondebug", //584 kif.cxx 1687
"fonctiondebug(fonction,objet): Initialise la fonction de debuggage qui sera appelée pendant l'exécution", //585 kif.cxx 1687
"fermedebug", //586 kif.cxx 1688
"fermedebug(): Fin du mode debug commencé avec fonctiondebug", //587 kif.cxx 1688
"ouvre", //588 kif.cxx 1689
"ouvre(): ouvre une session KiF", //589 kif.cxx 1689
"nettoie", //590 kif.cxx 1690
"nettoie(): ferme une session KiF", //591 kif.cxx 1690
"nom", //592 kif.cxx 1691
"nom(): Renvoie le chemin du fichier KiF", //593 kif.cxx 1691
"exportées", //594 kif.cxx 1692
"exportées(): Renvoie la liste des méthodes exportées", //595 kif.cxx 1692
"_chargeur", //596 kif.cxx 1693
"_chargeur: Une variable de type 'kif' qui pointe vers le programme le plus englobant.", //597 kif.cxx 1693
" (ligne:", //598 kif.cxx 1700
"ici", //599 kif.cxx 1792
"nulle", //600 kif.cxx 1792
"Instruction", //601 kif.cxx 1907
"ligne %d dans %s", //602 kif.cxx 1913
" à la ligne:", //603 kif.cxx 1943
" dans ", //604 kif.cxx 1946
"ligne %ld dans %s", //605 kif.cxx 1962
"ligne %ld", //606 kif.cxx 1964
"KIF(413): Valeur incompatible", //607 kif.cxx 2422
"fonction", //608 kif.cxx 2544
"classe", //609 kif.cxx 2553
"l", //610 kif.cxx 2644
"e", //611 kif.cxx 2652
"e+", //612 kif.cxx 2660
"vrai", //613 kif.cxx 5429
"faux", //614 kif.cxx 5430
"un objet omni peut seulement appeler des fonctions:", //615 kif.cxx 6144
"Seules des fonctions peuvent être appelées depuis un conteneur:", //616 kif.cxx 6176
"tableau", //617 kif.cxx 6696
"dictionnaire", //618 kif.cxx 6697
"%lld", //619 kif.cxx 8250     <------- Provide a translation
"%lld/%lld", //620 kif.cxx 8257     <------- Provide a translation
"%Y/%m/%d %H:%M:%S", //621 kif.cxx 8272     <------- Provide a translation
"' dans objet de type '", //622 kif.cxx 10353
"KIF(301): fonction inconnu:", //623 kif.cxx 10928
"KIF(345): fonction classe inconnu:", //624 kif.cxx 10954
"KIF(192): fonction externe inconnu:", //625 kif.cxx 11054
"exécutetâche", //626 kif.cxx 12275
"_KIFPRINCIPALE", //627 kif.cxx 12548
"CLASSE::", //628 kif.cxx 12571
"FONCTION::", //629 kif.cxx 12593
"h pour aide", //630 kif.cxx 12705
"%d> ", //631 kif.cxx 12753     <------- Provide a translation
"Fin", //632 kif.cxx 12838
"H", //633 kif.cxx 12850     <------- Provide a translation
"$: fin mode debug", //634 kif.cxx 12876
"$$: fin programme", //635 kif.cxx 12877
"o: quitter la fonction courante", //636 kif.cxx 12878
"i: dans la fonction", //637 kif.cxx 12879
"f: indexes fichiers", //638 kif.cxx 12880
"g ligne(fichier)|label: aller ligne", //639 kif.cxx 12881
"b ligne(fichier)|label: point d'arrêts (si ligne==-1: détruit tous les points d'arrêt. Sans paramètre: affichage des points d'arrêt)", //640 kif.cxx 12882
"s: pile d'exécution", //641 kif.cxx 12883
"n (nb lignes): liste les instructions suivantes", //642 kif.cxx 12884
"l f t: liste les lignes de f à t", //643 kif.cxx 12885
"w variable: ajoute un observateur sur la variable", //644 kif.cxx 12886
"r variable: retire un observateur sur la variable", //645 kif.cxx 12887
"H: historique des commandes", //646 kif.cxx 12888
"!nb: commande rexécutée", //647 kif.cxx 12889
"<: plus bas dans l'historique", //648 kif.cxx 12890
">: plus haut dans l'historique", //649 kif.cxx 12891
"$variable", //650 kif.cxx 12892
"$expression;", //651 kif.cxx 12893
"label inconnu:", //652 kif.cxx 12965
"Ligne:", //653 kif.cxx 12984
"Points d'arrêt tous détruits", //654 kif.cxx 12993
"Points d'arrêt  nettoyés", //655 kif.cxx 13012
"variable inconnue", //656 kif.cxx 13110
"(Tâche:", //657 kif.cxx 13277
"séquence", //658 kif.cxx 13414
"capte", //659 kif.cxx 13415
"omni", //660 kif.cxx 13416
"dans", //661 kif.cxx 13417
"_appel", //662 kif.cxx 13418
"_initiale", //663 kif.cxx 13419
"taille", //664 kif.cxx 13420
"synchrone", //665 kif.cxx 13431
"chaine", //666 kif.cxx 13433
"nombre", //667 kif.cxx 13434
"long", //668 kif.cxx 13435
"réel", //669 kif.cxx 13436
"booléen", //670 kif.cxx 13437
"fraction", //671 kif.cxx 13438
"_chemins", //672 kif.cxx 13457
"_args", //673 kif.cxx 13458
"_OS", //674 kif.cxx 13459
"_sep", //675 kif.cxx 13461
"_pi", //676 kif.cxx 13465     <------- Provide a translation
"_e", //677 kif.cxx 13467     <------- Provide a translation
"entréestandard", //678 kif.cxx 13468
"pour", //679 kif.cxx 13470
"tantque", //680 kif.cxx 13471
"tente", //681 kif.cxx 13472
"parmi", //682 kif.cxx 13473
"si", //683 kif.cxx 13474
"_finale", //684 kif.cxx 13476
"_exécute_comme_tâche", //685 kif.cxx 13477
"chargelocalement", //686 kif.cxx 13478
"_finl", //687 kif.cxx 13479
"vide", //688 kif.cxx 13480
"dchaines", //689 kif.cxx 13482
"dnombres", //690 kif.cxx 13483
"dréels", //691 kif.cxx 13484
"chaines", //692 kif.cxx 13485
"nombres", //693 kif.cxx 13486
"réels", //694 kif.cxx 13487
"_arrêt", //695 kif.cxx 13493
"_pointarrêt", //696 kif.cxx 13495
"_defaut", //697 kif.cxx 13497
"_continue", //698 kif.cxx 13499
"_negation", //699 kif.cxx 13501
"_RaiseError", //700 kif.cxx 13511     <------- Provide a translation
"octet", //701 kif.cxx 13542
"bit", //702 kif.cxx 13545
"bits", //703 kif.cxx 13546
"appel", //704 kif.cxx 13548
"weka", //705 kif.cxx 13559
"liste", //706 kif.cxx 13561
"itérateur", //707 kif.cxx 13562
"fichier", //708 kif.cxx 13563
"chrono", //709 kif.cxx 13625
"enum", //710 kif.cxx 13630
"bool", //711 kif.cxx 13641
"ritérateur", //712 kif.cxx 13644
"pasdans", //713 kif.cxx 13741
"estun", //714 kif.cxx 13751
"type", //715 kif.cxx 13752
"_id", //716 kif.cxx 13759
"hachage", //717 kif.cxx 13790
"positionoctet", //718 kif.cxx 13815
"positioncarac", //719 kif.cxx 13816
"chasse", //720 kif.cxx 13954
"ferme", //721 kif.cxx 13958
"@pi", //722 kif.cxx 14008
"@e", //723 kif.cxx 14008
"euclidien", //724 kif.cxx 14041
"_variables", //725 kif.cxx 14236
"_pile", //726 kif.cxx 14237
"_nomfichier", //727 kif.cxx 14238
"ramassemiette", //728 kif.cxx 14241
"pause", //729 kif.cxx 14268
"suspend", //730 kif.cxx 14269
"évalue", //731 kif.cxx 14270
"intervalle", //732 kif.cxx 14278
"environnement", //733 kif.cxx 14285
"lève", //734 kif.cxx 14287
"verrouille", //735 kif.cxx 14294
"déverrouille", //736 kif.cxx 14295
"attend", //737 kif.cxx 14296
"attendquefaux", //738 kif.cxx 14297
"attendquejoindre", //739 kif.cxx 14298
"enattente", //740 kif.cxx 14299
"libère", //741 kif.cxx 14300
"tue", //742 kif.cxx 14301
"version", //743 kif.cxx 14303
"affiche", //744 kif.cxx 14304
"sortie", //745 kif.cxx 14305
"chasse", //746 kif.cxx 14306
"afficheln", //747 kif.cxx 14307
"afficheerr", //748 kif.cxx 14308
"affichelnerr", //749 kif.cxx 14309
"idtâche", //750 kif.cxx 14313
"tâcheparent", //751 kif.cxx 14314
"fonctiondebug", //752 kif.cxx 14316
"donnéesdebug", //753 kif.cxx 14319
"infopile", //754 kif.cxx 14321
"touche", //755 kif.cxx 14377
"aléatoire", //756 kif.cxx 14380
""};


Exported const char* KIFRAWSTRINGS[]={
"Cet état d'attente n'existe pas: '", //0 kifraw.cxx 98
"MATH(100): Erreur mauvaise fraction: dénominateur est 0", //1 kifraw.cxx 1756
"BITS(105): Mauvais type de paramètre dans partie", //2 kifraw.cxx 1814
"KIF(543): Sortie standard hors de portée", //3 kifraw.cxx 1851
"KIF(544): Sortie erreur hors de portée", //4 kifraw.cxx 1868
"KIF(124): Format incorrect", //5 kifraw.cxx 3049
"Non implante dans KiF:", //6 kifraw.cxx 3652
""};


Exported const char* KIFLIBSTRINGS[]={
"KiF.", //0 kiflibs.cxx 92
"Copyright (C) Xerox Corporation 2010-2011", //1 kiflibs.cxx 95
"Xerox Research Center Europe", //2 kiflibs.cxx 97
"Implementation: Claude Roux", //3 kiflibs.cxx 99
"Init_", //4 kiflibs.cxx 126
"_Terminate", //5 kiflibs.cxx 129
"_KifInitialisationModule", //6 kiflibs.cxx 130
"KIFLIBS", //7 kiflibs.cxx 142
"KIF(999): Veuillez définir KIFLIBS", //8 kiflibs.cxx 145
".dll", //9 kiflibs.cxx 150
".so", //10 kiflibs.cxx 200
"lib", //11 kiflibs.cxx 203
"lib%s.so", //12 kiflibs.cxx 205     <------- Provide a translation
"lib%s", //13 kiflibs.cxx 207     <------- Provide a translation
"%s.so", //14 kiflibs.cxx 211     <------- Provide a translation
"Fonction inconnue: '", //15 kiflibs.cxx 476
"Variable inconnue: '", //16 kiflibs.cxx 506
"Variable inconnue: '", //17 kiflibs.cxx 516
"arrête", //18 kiflibs.cxx 772
"continue", //19 kiflibs.cxx 775
"renvoie", //20 kiflibs.cxx 778
"retourne", //21 kiflibs.cxx 778
"retireerreursurclef", //22 kiflibs.cxx 822
"erreursurclef", //23 kiflibs.cxx 827
"Paramètre manquant dans 'erreursurclef'", //24 kiflibs.cxx 836
"tailleramassemiette", //25 kiflibs.cxx 842
"Paramètre manquant dans 'tailleramassemiette'", //26 kiflibs.cxx 856
"fonctionramassemiette", //27 kiflibs.cxx 862
"Paramètre manquant dans 'fonctionramassemiette'", //28 kiflibs.cxx 877
"importe", //29 kiflibs.cxx 883
"Paramètre manquant dans 'importe'", //30 kiflibs.cxx 930
"chargelocalement", //31 kiflibs.cxx 936
"Paramètre manquant dans 'chargelocalement'", //32 kiflibs.cxx 946
"Fonction appelée inconnue:", //33 kiflibs.cxx 1028
"Aucune fonction compatible trouvée pour:", //34 kiflibs.cxx 1096
"tableau", //35 kiflibs.cxx 1155
"chaine", //36 kiflibs.cxx 1155
"Mauvais intervalle pour cette variable:", //37 kiflibs.cxx 1156
"type est=", //38 kiflibs.cxx 1156
"ici", //39 kiflibs.cxx 1443
"Mauvais appel avec 'ici'", //40 kiflibs.cxx 1445
"Variable invalide: '", //41 kiflibs.cxx 1483
"vrai", //42 kiflibs.cxx 1559
"Variable: '", //43 kiflibs.cxx 1638
"' a déjà été déclarée", //44 kiflibs.cxx 1640
"Les variables communes ne peuvent être déclarée que dans une classe:", //45 kiflibs.cxx 1670
"Type inconnu: '", //46 kiflibs.cxx 1752
"private", //47 kiflibs.cxx 2108
"common", //48 kiflibs.cxx 2114
"const", //49 kiflibs.cxx 2117
"Erreur: Label déjà défini: '", //50 kiflibs.cxx 2282
"Erreur: Fonction predéfinie, Veuillez donner un autre nom: '", //51 kiflibs.cxx 2300
"joindre", //52 kiflibs.cxx 2325
"protégée", //53 kiflibs.cxx 2330
"exclusive", //54 kiflibs.cxx 2335
"stricte", //55 kiflibs.cxx 2344
"polynomiale", //56 kiflibs.cxx 2351
"tâche", //57 kiflibs.cxx 2354
"autoexécute", //58 kiflibs.cxx 2358
"Erreur: Cette fonction a déjà été utilisée: '", //59 kiflibs.cxx 2386
"Erreur: Un AUTOEXECUTE ne peut avoir de paramètres: '", //60 kiflibs.cxx 2422
"Erreur: Ce classe a déjà été déclaré:", //61 kiflibs.cxx 2456
".kif", //62 kiflibs.cxx 2567
"Ne peut ouvrir ce fichier kif:", //63 kiflibs.cxx 2571
"réel f,g,h; nombre i,j,k; chaine s,t,u; tableau v; dictionnaire d;\n", //64 kiflibs.cxx 2699
"EVL(100):", //65 kiflibs.cxx 2711
" (ligne:%d)", //66 kiflibs.cxx 2715
"Erreur pendant la lecture d'un fichier KiF:", //67 kiflibs.cxx 2789
"Fichier inconnu:", //68 kiflibs.cxx 2791
"' déjà déclaré", //69 kiflibs.cxx 3314
"^i_", //70 kiflibs.cxx 3965
""};


Exported const char* KIFSPARSESTRINGS[]={
"SPM(100): Index inconnu", //0 kifsparse.cxx 784
"nom", //1 kifsparse.cxx 892
"taille", //2 kifsparse.cxx 927
"MAT(131): Mauvais index ou mauvaise valeur (réception devrait être un dictionnaire ou un vecteurmatrice)", //3 kifsparse.cxx 1575
"MAT(123): Mauvais type de variable de réception", //4 kifsparse.cxx 1711
"MAT(105): mauvais paramètre, il manque une matrice creuse comme résultat", //5 kifsparse.cxx 3474
"MAT(106): mauvais paramètre, il manque un tableau pour les valeurs réelles", //6 kifsparse.cxx 3479
"MAT(107): mauvais paramètre, il manque un tableau pour les valeurs imaginaires", //7 kifsparse.cxx 3483
"MAT(204): On attend un tableau comme paramètre", //8 kifsparse.cxx 3523
"MAT(103): Pas de décomposition LU pour cette matrice", //9 kifsparse.cxx 3534
"MAT(205): Vous auriez dû appeler 'décompositionLU' avec 'vrai' comme second paramètre", //10 kifsparse.cxx 3548
"MAT(104): Matrice singulière, pas de solution", //11 kifsparse.cxx 3634
"MAT(102): Pas de déterminant pour cette matrice", //12 kifsparse.cxx 3658
"MAT(203): Division par 0 impossible", //13 kifsparse.cxx 4036
"MAT(202): On ne peut diviser avec cette matrice", //14 kifsparse.cxx 4057
"%d:%d", //15 kifsparse.cxx 4241     <------- Provide a translation
"Version de KiF incompatible", //16 kifsparse.cxx 4942
"matrice", //17 kifsparse.cxx 4958
"vecteurmatrice", //18 kifsparse.cxx 4961
"bornes", //19 kifsparse.cxx 4977
"bornes(x,y): Renvoie les bornes de la matrice", //20 kifsparse.cxx 4977
"ligne", //21 kifsparse.cxx 4978
"ligne(x): renvoie la ligne x comme dictionnaire ou comme vecteurmatrice", //22 kifsparse.cxx 4978
"colonne", //23 kifsparse.cxx 4979
"colonne(x): renvoie la colonne x comme dictionnaire ou comme vecteurmatrice", //24 kifsparse.cxx 4979
"clefligne", //25 kifsparse.cxx 4980
"clefligne(nombre l): Renvoie la clef correspondant à la ligne l ou tout les clefs en ligne de la matrice", //26 kifsparse.cxx 4980
"clefcol", //27 kifsparse.cxx 4981
"clefcol(nombre c): renvoie la clef correspondant à la colonne c ou toutes les clefs en colonne de la matrice", //28 kifsparse.cxx 4981
"transposée", //29 kifsparse.cxx 4982
"transposée(): renvoie la matrice transposée", //30 kifsparse.cxx 4982
"déterminant", //31 kifsparse.cxx 4983
"déterminant(): renvoie le déterminant de la matrice", //32 kifsparse.cxx 4983
"duplique", //33 kifsparse.cxx 4984
"duplique(): renvoie une duplication de la matrice", //34 kifsparse.cxx 4984
"décompositionLU", //35 kifsparse.cxx 4985
"décompositionLU(tableau permutations,bool arésoudre): Remplace la matrice courante avec la décomposition LU selon une permutation le long des lignes d'elle-même. arésoudre vaut 'vrai', si l'instruction suivante est résoudLU.", //36 kifsparse.cxx 4985
"résoudLU", //37 kifsparse.cxx 4986
"résoudLU(tableau permutations,tableau valeurs): résoud les équations linéaires basées sur le contenu de valeurs. Renvoie le résultat dans valeurs.", //38 kifsparse.cxx 4986
"commute", //39 kifsparse.cxx 4987
"inserse(): Renvoie la matrice inversée", //40 kifsparse.cxx 4987
"décompositionbandeLU", //41 kifsparse.cxx 4988
"décompositionbandeLU(bool local,tableau res): renvoie une factorisation LU d'une matrice bande", //42 kifsparse.cxx 4988
"solveurbandeLU", //43 kifsparse.cxx 4989
"solveurbandeLU(bool local,tableau res): renvoie une factorisation LU d'une matrice bande", //44 kifsparse.cxx 4989
"min", //45 kifsparse.cxx 4990
"min(): renvoie la valeur minimale de la matrice", //46 kifsparse.cxx 4990
"max", //47 kifsparse.cxx 4991
"max(): renvoie la valeur maximale de la matrice", //48 kifsparse.cxx 4991
"propres", //49 kifsparse.cxx 4992
"somme", //50 kifsparse.cxx 5003
"somme(): renvoie la somme de tous les éléments", //51 kifsparse.cxx 5003
"produit", //52 kifsparse.cxx 5004
"produit(): renvoie le produit de tous les éléments", //53 kifsparse.cxx 5004
"index", //54 kifsparse.cxx 5005
"index(): renvoie l'index de la ligne ou de la colonne de la matrice correspondante", //55 kifsparse.cxx 5005
"min(): renvoie la valeur minimale du tableau de matrice", //56 kifsparse.cxx 5006
"max(): renvoie la valeur maximale du tableau de matrice", //57 kifsparse.cxx 5007
""};


Exported const char* KIFSOCKETSTRINGS[]={
"Pas d'erreur", //0 kifsocket.cxx 97
"taille", //1 kifsocket.cxx 195
"CONNECTEUR(855): Erreur connecteur", //2 kifsocket.cxx 360
"CONNECTEUR(852): Ne peut pas trouver un hôte de ce nom sur cette machine", //3 kifsocket.cxx 363
"CONNECTEUR(851): Serveur déjà lancé sur ce connecteur", //4 kifsocket.cxx 388
"CONNECTEUR(850): Mauvais nom de serveur", //5 kifsocket.cxx 401
"CONNECTEUR(853): Erreur à la connexion", //6 kifsocket.cxx 416
"CONNECTEUR(854): Erreur sur écoute", //7 kifsocket.cxx 422
"CONNECTEUR(857): Erreur à la connexion", //8 kifsocket.cxx 468
"CONNECTEUR(855): Erreur en lecture", //9 kifsocket.cxx 482
"CONNECTEUR(821): Connecteur non connecté", //10 kifsocket.cxx 536
"CONNECTEUR(888): Connexion en lecture refusée", //11 kifsocket.cxx 562
"CONNECTEUR(830): Temps dépassé", //12 kifsocket.cxx 569
"CONNECTEUR(888): Connexion en écriture refusée", //13 kifsocket.cxx 589
"CONNECTEUR(820): Paramètre manquant en lecture", //14 kifsocket.cxx 614
"CONNECTEUR(858): Aucun client connecté", //15 kifsocket.cxx 626
"CONNECTEUR(860): Erreur en lecture", //16 kifsocket.cxx 635
"Lecture:", //17 kifsocket.cxx 645
"CONNECTEUR(823): EXECUTE peut seulement être lancé depuis un serveur", //18 kifsocket.cxx 674
"CONNECTEUR(870): Objet '", //19 kifsocket.cxx 738
"' inconnu", //20 kifsocket.cxx 740
"CONNECTEUR(871): Fonction '", //21 kifsocket.cxx 796
"CONNECTEUR(820): Paramètre manquant en écriture", //22 kifsocket.cxx 931
"&&ERROR&&:", //23 kifsocket.cxx 943
"Ecriture:", //24 kifsocket.cxx 954
"CONNECTEUR(859): Erreur en écriture", //25 kifsocket.cxx 960
"CONNECTEUR(820): Paramètre manquant en LECTUREBRUTE", //26 kifsocket.cxx 1014
"CONNECTEUR(860): Erreur sur LECTUREBRUTE", //27 kifsocket.cxx 1033
"CONNECTEUR(820): Paramètre manquant dans ECRITUREBRUTE", //28 kifsocket.cxx 1068
"port", //29 kifsocket.cxx 1125
"adresse", //30 kifsocket.cxx 1127
"CONNECTEUR(852): Vous ne pouvez pas utiliser NOMCLIENT du côté client", //31 kifsocket.cxx 1132
"nom", //32 kifsocket.cxx 1192
"CONNECTEUR(812): Méthode inconnue", //33 kifsocket.cxx 1278
"Version de KiF incompatible", //34 kifsocket.cxx 1371
"connecteur", //35 kifsocket.cxx 1381
"distant", //36 kifsocket.cxx 1382
"créeserveur", //37 kifsocket.cxx 1400
"créeserveur(chaine hôte,nombre port,nombre nblients): crée un serveur. Si l'hôte est omis, le nom local est alors utilisé", //38 kifsocket.cxx 1400
"connecte", //39 kifsocket.cxx 1401
"connecte(chaine hôte,nombre port): connexion à un serveur", //40 kifsocket.cxx 1401
"attend", //41 kifsocket.cxx 1402
"nombre attend(): attend qu'un client se connecte et renvoie son identifiant.", //42 kifsocket.cxx 1402
"lit", //43 kifsocket.cxx 1403
"chaine lit(nombre num): lit une chaine sur le connecteur. Du côté serveur, num est l'identifiant du client. Utiliser 'lit()' sans paramètre du côté client", //44 kifsocket.cxx 1403
"lance", //45 kifsocket.cxx 1404
"lance(nombre num,chaine stop): Uniquement du côté serveur. Exécute des invocations de méthodes distantes. 'stop' est une chaine qui permet l'arrêt de ce service par le client.", //46 kifsocket.cxx 1404
"écrit", //47 kifsocket.cxx 1405
"écrit(nombre num,chaine s): Ecrit une chaine s sur le connecteur. Du côté serveur, 'num' est l'identifiant client renvoyé par 'attend'. En revanche du côté client, utiliser 'écrit(chaine s)'", //48 kifsocket.cxx 1405
"reçoit", //49 kifsocket.cxx 1406
"chaine reçoit(nombre num): Reçoit une chaine provenant d'une application non KiF. 'num' doit être omis du côté client.", //50 kifsocket.cxx 1406
"envoie", //51 kifsocket.cxx 1407
"envoie(nombre num,chaine s): Ecrit une chaine à destination d'une application non KiF. 'num' doit être omis du côté client.", //52 kifsocket.cxx 1407
"ferme", //53 kifsocket.cxx 1408
"ferme(nombre num): Ferme un connecteur. Du côté serveur, si 'num' est fourni, il ferme la connexion pour un client donné.", //54 kifsocket.cxx 1408
"bloquant", //55 kifsocket.cxx 1409
"bloquant(bool flag): Si 'flag' est 'vrai', le connecteur fonctionne en mode bloquant.", //56 kifsocket.cxx 1409
"tempsmax", //57 kifsocket.cxx 1410
"tempsmax(nombre t): Définit un temps maximum de 't' secondes pour le connecteur", //58 kifsocket.cxx 1410
"nomhôte", //59 kifsocket.cxx 1411
"nomhôte(): Retourne le nom de l'hôte", //60 kifsocket.cxx 1411
"port(): renvoie le port courant", //61 kifsocket.cxx 1412
"nomclient", //62 kifsocket.cxx 1413
"nomclient(nombre num): Retourne le nom du client", //63 kifsocket.cxx 1413
"laclasse", //64 kifsocket.cxx 1414
"laclasse(chaine frame): Retourne un identifiant objet déclaré du côté serveur", //65 kifsocket.cxx 1414
"lafonction", //66 kifsocket.cxx 1415
"lafonction(chaine frame): Retourne une fonction declarée du côté serveur", //67 kifsocket.cxx 1415
"mavariable", //68 kifsocket.cxx 1418
""};


Exported const char* KIFLTKSTRINGS[]={
"WND(101): Pas de fenêtre disponible", //0 kifltk.cxx 104
"COULEUR_AVANTPLAN", //1 kifltk.cxx 175
"COULEUR_ARRIEREPLAN2", //2 kifltk.cxx 176
"COULEUR_ARRIEREPLAN", //3 kifltk.cxx 177
"COULEUR_INACTIVE", //4 kifltk.cxx 178
"COULEUR_SELECTION", //5 kifltk.cxx 179
"GRI0", //6 kifltk.cxx 180
"FONCE3", //7 kifltk.cxx 181
"FONCE2", //8 kifltk.cxx 182
"FONCE1", //9 kifltk.cxx 183
"LEGER1", //10 kifltk.cxx 184
"LEGER2", //11 kifltk.cxx 185
"LEGER3", //12 kifltk.cxx 186
"NOIR", //13 kifltk.cxx 187
"ROUGE", //14 kifltk.cxx 188
"VERT", //15 kifltk.cxx 189
"JAUNE", //16 kifltk.cxx 190
"BLEU", //17 kifltk.cxx 191
"MAGENTA", //18 kifltk.cxx 192
"CYAN", //19 kifltk.cxx 193
"ROUGE_FONCE", //20 kifltk.cxx 194
"VERT_FONCE", //21 kifltk.cxx 195
"JAUNE_FONCE", //22 kifltk.cxx 196
"BLEU_FONCE", //23 kifltk.cxx 197
"MAGENTA_FONCE", //24 kifltk.cxx 198
"CYAN_FONCE", //25 kifltk.cxx 199
"BLANC", //26 kifltk.cxx 200
"Régulier", //27 kifltk.cxx 202
"Cocher", //28 kifltk.cxx 203
"Léger", //29 kifltk.cxx 204
"Répéter", //30 kifltk.cxx 205
"Renvoie", //31 kifltk.cxx 206
"Rond", //32 kifltk.cxx 207
"Image", //33 kifltk.cxx 208
"POUSSE", //34 kifltk.cxx 211
"RELACHE", //35 kifltk.cxx 212
"DRAGUE", //36 kifltk.cxx 213
"BOUGE", //37 kifltk.cxx 214
"ROUESOURIS", //38 kifltk.cxx 215
"TOUCHEPRESSEE", //39 kifltk.cxx 216
"TOUCHERELACHEE", //40 kifltk.cxx 217
"GLISSIERE_VERTICALE", //41 kifltk.cxx 219
"GLISSIERE_HORIZONTALE", //42 kifltk.cxx 220
"GLISSIERE_VERTICALE_PLEINE", //43 kifltk.cxx 221
"GLISSIERE_HORIZONTALE_PLEINE", //44 kifltk.cxx 222
"GLISSIERE_VERTICALE_JOLIE", //45 kifltk.cxx 223
"GLISSIERE_HORIZONTALE_JOLIE", //46 kifltk.cxx 224
"SOLIDE", //47 kifltk.cxx 226
"TIRET", //48 kifltk.cxx 227
"POINTILLE", //49 kifltk.cxx 228
"POINTTIRET", //50 kifltk.cxx 229
"TIRETPOINTPOINT", //51 kifltk.cxx 230
"CAP_PLAT", //52 kifltk.cxx 231
"CAP_ROND", //53 kifltk.cxx 232
"CAP_CARRE", //54 kifltk.cxx 233
"JOINT_MITER", //55 kifltk.cxx 234
"JOINT_ROND", //56 kifltk.cxx 235
"JOINT_BEVEL", //57 kifltk.cxx 236
"ALIGNE_CENTRE", //58 kifltk.cxx 238
"ALIGNE_HAUT", //59 kifltk.cxx 239
"ALIGNE_BAS", //60 kifltk.cxx 240
"ALIGNE_GAUCHE", //61 kifltk.cxx 241
"ALIGNE_DROIT", //62 kifltk.cxx 242
"ALIGNE_DEDANS", //63 kifltk.cxx 243
"ALIGNE_TEXTE_DESSUS_IMAGE", //64 kifltk.cxx 244
"ALIGNE_IMAGE_DESSUS_TEXTE", //65 kifltk.cxx 245
"ALIGNE_CLIP", //66 kifltk.cxx 246
"ALIGNE_ENROULE", //67 kifltk.cxx 247
"ALIGNE_IMAGE_ACOTE_TEXTE", //68 kifltk.cxx 248
"ALIGNE_TEXTE_ACOTE_IMAGE", //69 kifltk.cxx 249
"ALIGNE_IMAGE_BACKDROP", //70 kifltk.cxx 250
"ALIGNE_HAUT_GAUCHE", //71 kifltk.cxx 251
"ALIGNE_HAUT_DROIT", //72 kifltk.cxx 252
"ALIGNE_BAS_GAUCHE", //73 kifltk.cxx 253
"ALIGNE_BAS_DROIT", //74 kifltk.cxx 254
"ALIGNE_GAUCHE_HAUT", //75 kifltk.cxx 255
"ALIGNE_DROIT_HAUT", //76 kifltk.cxx 256
"ALIGNE_GAUCHE_BAS", //77 kifltk.cxx 257
"ALIGNE_DROIT_BAS", //78 kifltk.cxx 258
"ALIGNE_NOWRAP", //79 kifltk.cxx 259
"ALIGNE_POSITION_MASQUE", //80 kifltk.cxx 260
"ALIGNE_IMAGE_MASQUE", //81 kifltk.cxx 261
"LABEL_NORMAL", //82 kifltk.cxx 263
"PAS_DE_LABEL", //83 kifltk.cxx 264
"LABEL_OMBRE", //84 kifltk.cxx 265
"LABEL_GRAVE", //85 kifltk.cxx 266
"LABEL_BOSSELE", //86 kifltk.cxx 267
"CURSEUR_DEFAUT", //87 kifltk.cxx 269
"CURSEUR_FLECHE", //88 kifltk.cxx 270
"CURSEUR_CROIX", //89 kifltk.cxx 271
"CURSEUR_SABLIER", //90 kifltk.cxx 272
"CURSEUR_INSERE", //91 kifltk.cxx 273
"CURSEUR_MAIN", //92 kifltk.cxx 274
"CURSEUR_AIDE", //93 kifltk.cxx 275
"CURSEUR_BOUGE", //94 kifltk.cxx 276
"CURSEUR_NS", //95 kifltk.cxx 277
"CURSEUR_OE", //96 kifltk.cxx 278
"CURSEUR_NOSE", //97 kifltk.cxx 279
"CURSEUR_NESO", //98 kifltk.cxx 280
"CURSEUR_AUCUN", //99 kifltk.cxx 281
"CURSEUR_N", //100 kifltk.cxx 282
"CURSEUR_NE", //101 kifltk.cxx 283
"CURSEUR_E", //102 kifltk.cxx 284
"CURSEUR_SE", //103 kifltk.cxx 285
"CURSEUR_S", //104 kifltk.cxx 286
"CURSEUR_SO", //105 kifltk.cxx 287
"CURSEUR_O", //106 kifltk.cxx 288
"CURSEUR_NO", //107 kifltk.cxx 289
"CURSEUR_NORMAL", //108 kifltk.cxx 291
"CURSEUR_CHARIOT", //109 kifltk.cxx 292
"CURSEUR_DIM", //110 kifltk.cxx 293
"CURSEUR_BLOC", //111 kifltk.cxx 294
"CURSEUR_LOURD", //112 kifltk.cxx 295
"CURSEUR_LIGNE", //113 kifltk.cxx 296
"MAJ", //114 kifltk.cxx 298
"VERROUILLAGE_MAJ", //115 kifltk.cxx 299
"CTRL", //116 kifltk.cxx 300
"ALT", //117 kifltk.cxx 301
"VERROUILLAGE_NUM", //118 kifltk.cxx 302
"META", //119 kifltk.cxx 303
"VERROUILLANCE_DEFILEMENT", //120 kifltk.cxx 304
"BOUTON1", //121 kifltk.cxx 305
"BOUTON2", //122 kifltk.cxx 306
"BOUTON3", //123 kifltk.cxx 307
"BOUTONS", //124 kifltk.cxx 308
"MASQUE_TOUCHE", //125 kifltk.cxx 309
"COMMANDE", //126 kifltk.cxx 310
"CONTROLE", //127 kifltk.cxx 311
"Bouton", //128 kifltk.cxx 313
"Retour_Arrière", //129 kifltk.cxx 314
"Tab", //130 kifltk.cxx 315
"Entrée", //131 kifltk.cxx 316
"Pause", //132 kifltk.cxx 317
"Verrouillage_Défilement", //133 kifltk.cxx 318
"Echappe", //134 kifltk.cxx 319
"Accueil", //135 kifltk.cxx 320
"Gauche", //136 kifltk.cxx 321
"Haut", //137 kifltk.cxx 322
"Droit", //138 kifltk.cxx 323
"Bas", //139 kifltk.cxx 324
"Page_Haut", //140 kifltk.cxx 325
"Page_Bas", //141 kifltk.cxx 326
"Fin", //142 kifltk.cxx 327
"Imprimer", //143 kifltk.cxx 328
"Insère", //144 kifltk.cxx 329
"Menu", //145 kifltk.cxx 330
"Aide", //146 kifltk.cxx 331
"Verrouillage_Num", //147 kifltk.cxx 332
"KP", //148 kifltk.cxx 333
"KP_Entrée", //149 kifltk.cxx 334
"KP_Dernier", //150 kifltk.cxx 335
"F_Dernier", //151 kifltk.cxx 337
"Maj_G", //152 kifltk.cxx 338
"Maj_D", //153 kifltk.cxx 339
"Contrôle_G", //154 kifltk.cxx 340
"Contrôle_D", //155 kifltk.cxx 341
"Verrouillage_Maj", //156 kifltk.cxx 342
"Méta_G", //157 kifltk.cxx 343
"Méta_D", //158 kifltk.cxx 344
"Alt_G", //159 kifltk.cxx 345
"Alt_D", //160 kifltk.cxx 346
"Détruit", //161 kifltk.cxx 347
"PAS_DE_BOITE", //162 kifltk.cxx 349
"BOITE_PLATE", //163 kifltk.cxx 350
"BOITE_HAUTE", //164 kifltk.cxx 351
"BOITE_BASSE", //165 kifltk.cxx 352
"CADRE_HAUT", //166 kifltk.cxx 353
"CADRE_BAS", //167 kifltk.cxx 354
"BOITE_FINE_HAUTE", //168 kifltk.cxx 355
"BOITE_FINE_BASE", //169 kifltk.cxx 356
"CADRE_FIN_HAUT", //170 kifltk.cxx 357
"CADRE_FIN_BASE", //171 kifltk.cxx 358
"BOITE_GRAVEE", //172 kifltk.cxx 359
"BOITE_BOSSELEE", //173 kifltk.cxx 360
"CADRE_GRAVE", //174 kifltk.cxx 361
"CADRE_BOSSELE", //175 kifltk.cxx 362
"BOITE_BORDEE", //176 kifltk.cxx 363
"BOITE_OMBREE", //177 kifltk.cxx 364
"CADRE_BORDE", //178 kifltk.cxx 365
"CADRE_OMBRE", //179 kifltk.cxx 366
"BOITE_RONDE", //180 kifltk.cxx 367
"BOITE_ROMBREE", //181 kifltk.cxx 368
"CADRE_ROND", //182 kifltk.cxx 369
"BOITE_RPLATE", //183 kifltk.cxx 370
"BOITE_RONDE_HAUTE", //184 kifltk.cxx 371
"BOITE_RONDE_BASSE", //185 kifltk.cxx 372
"BOITE_DIAMANT_HAUTE", //186 kifltk.cxx 373
"BOITE_DIAMANT_BASSE", //187 kifltk.cxx 374
"BOITE_OVALE", //188 kifltk.cxx 375
"BOITE_OMBREEO", //189 kifltk.cxx 376
"CADRE_OVAL", //190 kifltk.cxx 377
"BOITE_PLATEO", //191 kifltk.cxx 378
"BOITE_PLASTIC_HAUTE", //192 kifltk.cxx 379
"BOITE_PLASTIC_BASSE", //193 kifltk.cxx 380
"CADRE_PLASTIC_HAUT", //194 kifltk.cxx 381
"CADRE_PLASTIC_BAS", //195 kifltk.cxx 382
"BOITE_PLASTIC_FINE_HAUTE", //196 kifltk.cxx 383
"BOITE_PLASTIC_FINE_BASSE", //197 kifltk.cxx 384
"BOITE_PLASTIC_RONDE_HAUTE", //198 kifltk.cxx 385
"BOITE_PLASTIC_RONDE_BASE", //199 kifltk.cxx 386
"BOITE_GTK_HAUTE", //200 kifltk.cxx 387
"BOITE_GTK_BASSE", //201 kifltk.cxx 388
"CADRE_GTK_HAUT", //202 kifltk.cxx 389
"CADRE_GTK_BAS", //203 kifltk.cxx 390
"BOITE_GTK_FINE_HAUTE", //204 kifltk.cxx 391
"BOITE_GTK_FINE_BASSE", //205 kifltk.cxx 392
"CADRE_GTK_FINE_HAUT", //206 kifltk.cxx 393
"CADRE_GTK_FINE_BAS", //207 kifltk.cxx 394
"BOITE_GTK_ROND_HAUTE", //208 kifltk.cxx 395
"BOITE_GTK_ROND_BASSE", //209 kifltk.cxx 396
"BOITE_TYPE_LIBRE", //210 kifltk.cxx 397
"HELVETICA", //211 kifltk.cxx 399
"HELVETICA_GRAS", //212 kifltk.cxx 400
"HELVETICA_ITALIQUE", //213 kifltk.cxx 401
"HELVETICA_GRAS_ITALIQUE", //214 kifltk.cxx 402
"COURIER", //215 kifltk.cxx 403
"COURIER_GRAS", //216 kifltk.cxx 404
"COURIER_ITALIQUE", //217 kifltk.cxx 405
"COURIER_GRAS_ITALIQUE", //218 kifltk.cxx 406
"TIMES", //219 kifltk.cxx 407
"TIMES_GRAS", //220 kifltk.cxx 408
"TIMES_ITALIQUE", //221 kifltk.cxx 409
"TIMES_GRAS_ITALIQUE", //222 kifltk.cxx 410
"SYMBOLE", //223 kifltk.cxx 411
"ECRAN", //224 kifltk.cxx 412
"ECRAN_GRAS", //225 kifltk.cxx 413
"ZAPF_DINGBATS", //226 kifltk.cxx 414
"FREE_FONT", //227 kifltk.cxx 415
"GRAS", //228 kifltk.cxx 416
"ITALIQUE", //229 kifltk.cxx 417
"GRAS_ITALIQUE", //230 kifltk.cxx 418
"BOUTON_NORMAL", //231 kifltk.cxx 438
"BOUTON_INVERSEUR", //232 kifltk.cxx 439
"BOUTON_RADIO", //233 kifltk.cxx 440
"BOUTON_CACHE", //234 kifltk.cxx 441
"QUAND_JAMAIS", //235 kifltk.cxx 443
"QUAND_CHANGE", //236 kifltk.cxx 444
"QUAND_RELACHE", //237 kifltk.cxx 445
"QUAND_RELACHE_TOUJOURS", //238 kifltk.cxx 446
"QUAND_TOUCHE_ENTREE", //239 kifltk.cxx 447
"QUAND_TOUCHE_ENTREE_TOUJOURS", //240 kifltk.cxx 448
"FL_TAILLE_NORMALE", //241 kifltk.cxx 463
"FL_", //242 kifltk.cxx 469
"FL_CHOIX_SIMPLE", //243 kifltk.cxx 559
"FL_CHOIX_MULTIPLE", //244 kifltk.cxx 563
"FL_CHOIX_SAUVEGARDE", //245 kifltk.cxx 567
"FL_CHOIX_REPERTOIRE", //246 kifltk.cxx 571
"FL_ARBRE_SELECTION_AUCUNE", //247 kifltk.cxx 576
"FL_ARBRE_SELECTION_SIMPLE", //248 kifltk.cxx 580
"FL_ARBRE_SELECTION_MULTI", //249 kifltk.cxx 584
"FL_ARBRE_TRI_AUCUN", //250 kifltk.cxx 588
"FL_ARBRE_TRI_ASCENDANT", //251 kifltk.cxx 592
"FL_ARBRE_TRI_DESCENDANT", //252 kifltk.cxx 596
"FL_ARBRE_CONNECTEUR_AUCUN", //253 kifltk.cxx 600
"FL_ARBRE_CONNECTEUR_POINTILLE", //254 kifltk.cxx 604
"FL_ARBRE_CONNECTEUR_SOLIDE", //255 kifltk.cxx 608
"FL_ARBRE_RAISON_AUCUNE", //256 kifltk.cxx 612
"FL_ARBRE_RAISON_SELECTIONNE", //257 kifltk.cxx 616
"FL_ARBRE_RAISON_DESELECTIONNE", //258 kifltk.cxx 620
"FL_ARBRE_RAISON_OUVERT", //259 kifltk.cxx 624
"FL_ARBRE_RAISON_FERME", //260 kifltk.cxx 628
"WND(098): Widget ne peut être instancié", //261 kifltk.cxx 721
"WND(678): Widget non initialisé", //262 kifltk.cxx 749
"WND(657): Code couleur inconnu", //263 kifltk.cxx 790
"WND(805): Widget non initialisé", //264 kifltk.cxx 807
"WND(202): alignement inconnu", //265 kifltk.cxx 814
"WND(607): code de forme de ligne inconnu", //266 kifltk.cxx 869
"WND(245): Mauvais nombre d'éléments dans le tableau de couleurs", //267 kifltk.cxx 896
"L", //268 kifltk.cxx 1104
"H", //269 kifltk.cxx 1105     <------- Provide a translation
"WND(657): Code de police inconnu", //270 kifltk.cxx 1143
"WND(658): Police inconnue", //271 kifltk.cxx 1151
"WND(901): Widget non initialisé", //272 kifltk.cxx 1217
"WND(904): Tablean nécessaire", //273 kifltk.cxx 1239
"WND(905): Coordonnées manquantes", //274 kifltk.cxx 1242
"WND(905): Coordonnées incohérentes", //275 kifltk.cxx 1255
"WND(678): Widget non initialisé", //276 kifltk.cxx 1367
"WND(102): Widget non crée", //277 kifltk.cxx 1386
"WND(765): Police inconnu:", //278 kifltk.cxx 1398
"nom", //279 kifltk.cxx 1716
"WND(808): Image déjà charge", //280 kifltk.cxx 1739
"WND(809): Image non charge", //281 kifltk.cxx 1745
"taille", //282 kifltk.cxx 1769
"WND(100): Ne peut recréer une fenêtre existante", //283 kifltk.cxx 2022
"WND(431): Identifiant de police non disponible", //284 kifltk.cxx 2052
"WND(109): Le premier élément doit être une fonction", //285 kifltk.cxx 2105
"WND(608): Nombre de paramètres attendu=5", //286 kifltk.cxx 2162
"WND(608): Nombre de paramètres attendu=6", //287 kifltk.cxx 2167
"WND(612): Ne peut appeler cette méthode ici. débutpoints manquant", //288 kifltk.cxx 2327
"WND(612): Ne peut appeler cette méthode ici. débutboucle manquant", //289 kifltk.cxx 2373
"WND(612): Ne peut appeler cette méthode ici. débutpolygone manquant", //290 kifltk.cxx 2396
"WND(612): Ne peut appeler cette méthode ici. débutpolygonecomplexe manquant", //291 kifltk.cxx 2419
"WND(612): Ne peut appeler cette méthode ici. débutligne manquant", //292 kifltk.cxx 2454
"WND(610): Vous ne pouvez pas utiliser cette méthode dans ce contexte", //293 kifltk.cxx 2467
"WND(102): Fenêtre déjà instanciée", //294 kifltk.cxx 2675
"WND(341): Action souris inconnue", //295 kifltk.cxx 2691
"WND(342): Mauvais paramètre dans votre fonction de rappel souris", //296 kifltk.cxx 2697
"WND(344): Action clavier inconnue", //297 kifltk.cxx 2724
"WND(343): Mauvais paramètre dans votre fonction de rappel clavier", //298 kifltk.cxx 2731
"WND(167): Image inconnue", //299 kifltk.cxx 2831
"WND(168): Image non chargée", //300 kifltk.cxx 2839
"WND(109): tableau nécessaire", //301 kifltk.cxx 2873
"WND(109): tableau nécessaire", //302 kifltk.cxx 2882
"WND(109): Mauvais élément, champ manquant dans un sous-menu", //303 kifltk.cxx 2909
"WND(109): Mauvais élément, deuxieme champ dans un sous-menu doit être un tableau", //304 kifltk.cxx 2914
"WND(109): Mauvais élément, troisième champ dans un sous-menu doit être une fonction", //305 kifltk.cxx 2931
"WND(678): Entrée non initialisée", //306 kifltk.cxx 3037
"WND(123): Operation non disponible", //307 kifltk.cxx 3076
"WND(134): Widget n'a pas encore été crée", //308 kifltk.cxx 3088
"WND(133): Widget attendu comme paramètre", //309 kifltk.cxx 3093
"bouton", //310 kifltk.cxx 3278
"rouex", //311 kifltk.cxx 3279
"rouey", //312 kifltk.cxx 3280
"xracine", //313 kifltk.cxx 3283
"yracine", //314 kifltk.cxx 3284
"WND(101): f_entrée déjà crée", //315 kifltk.cxx 3503
"WND(101): f_éditeur déjà crée", //316 kifltk.cxx 4081
"WND(101): Aucun f_éditeur disponible", //317 kifltk.cxx 4106
"WND(342): Mauvais paramètre dans votre fonction de rappel clavier", //318 kifltk.cxx 4217
"EDT(768): Mauvais nombre d'éléments dans le tableau... 3 ou 4 expected", //319 kifltk.cxx 4226
"WND(627): Nombre maximum de styles atteint", //320 kifltk.cxx 4295
"WND(628): Feuille de style incoherente", //321 kifltk.cxx 4316
"WND(678): Editeur non initialisé", //322 kifltk.cxx 4324
"EDT(766): Mauvaise f_entrée: dictionnaire attendu", //323 kifltk.cxx 4331
"EDT(767): Clef '#' manquante dans le dictionnaire. '#' est le clef de base et est obligatoire", //324 kifltk.cxx 4344
"EDT(767): Mauvaise f_entrée: tableau attendu", //325 kifltk.cxx 4347
"WND(689): Hors de portée", //326 kifltk.cxx 4577
"WND(679): Table des styles non initialisé", //327 kifltk.cxx 4607
"WND(679): Clef de style inconnue", //328 kifltk.cxx 4618
"début", //329 kifltk.cxx 4887
"fin", //330 kifltk.cxx 4888
"\n", //331 kifltk.cxx 5164
"curseur", //332 kifltk.cxx 5437
"WND(101): f_sortie déjà crée", //333 kifltk.cxx 5619
"WND(679): f_sortie non initialisé", //334 kifltk.cxx 5641
"WND(101): bouton déjà crée", //335 kifltk.cxx 5931
"WND(675): forme inconnu de bouton:", //336 kifltk.cxx 5936
"WND(675): type inconnu de bouton:", //337 kifltk.cxx 5955
"WND(805): Bouton non initialisé", //338 kifltk.cxx 6004
"WND(813): objet WIMAGE requis", //339 kifltk.cxx 6007
"WND(654): action inconnue", //340 kifltk.cxx 6045
"WND(656): code touche inconnu", //341 kifltk.cxx 6071
"WND(101): f_boite déjà crée", //342 kifltk.cxx 6254
"WND(102): f_boite non crée", //343 kifltk.cxx 6271
"WND(767): type inconnu:", //344 kifltk.cxx 6276
"WND(101): f_groupe déjà crée", //345 kifltk.cxx 6434
"WND(101): f_groupe non crée", //346 kifltk.cxx 6472
"WND(507): Ce f_groupe n'appartient pas à une structure onglet", //347 kifltk.cxx 6476
"WND(101): f_onglet déjà crée", //348 kifltk.cxx 6641
"WND(101): f_onglet non crée", //349 kifltk.cxx 6658
"WND(800): Pas d'onglet actif", //350 kifltk.cxx 6671
"WND(105): Vous pouvez seulement sélectionner un objet f_groupe", //351 kifltk.cxx 6678
"WND(506): Groupe vide", //352 kifltk.cxx 6681
"WND(507): Ce f_groupe n'appartient pas à cet onglet", //353 kifltk.cxx 6683
"WND(105): Vous pouvez seulement ajouter des objets f_groupe", //354 kifltk.cxx 6706
"WND(800): Mauvais nombre de paramètres", //355 kifltk.cxx 6712
"WND(505): Vous pouvez seulement retirer des objets f_groupe", //356 kifltk.cxx 6730
"WND(101): f_table déjà crée", //357 kifltk.cxx 7027
"WND(102): f_table n'a pas encore été créée", //358 kifltk.cxx 7055
"WND(678): f_table non initialisée", //359 kifltk.cxx 7206
"WND(102): f_table non crée", //360 kifltk.cxx 7283
"WND(805): f_table non crée", //361 kifltk.cxx 7306
"haut", //362 kifltk.cxx 7433
"gauche", //363 kifltk.cxx 7434
"bas", //364 kifltk.cxx 7435
"droit", //365 kifltk.cxx 7436
"valeurs", //366 kifltk.cxx 7438
"%d:%d", //367 kifltk.cxx 7442     <------- Provide a translation
"WND(100): Ne peut recréer un widget f_défilement", //368 kifltk.cxx 7553
"WND(101): Pas de f_défilement disponible", //369 kifltk.cxx 7577
"WND(100): Ne peut recréer un widget f_glissière", //370 kifltk.cxx 7735
"WND(101): Pas de f_glissière disponible", //371 kifltk.cxx 7769
"WND(206): Mauvais type de f_glissière", //372 kifltk.cxx 7823
"WND(100): Ne peut recréer un widget f_choix", //373 kifltk.cxx 8037
"WND(678): f_choix non initialisé", //374 kifltk.cxx 8059
"WND(100): Ne peut recréer un widget f_parcourir", //375 kifltk.cxx 8352
"WND(121): Le widget f_parcourir n'a pas encore été crée", //376 kifltk.cxx 8375
"WND(094): Seul un objet f_noeudarbre peut affecter un autre f_noeudarbre", //377 kifltk.cxx 8588
"WND(117): f_noeudarbre inconnu", //378 kifltk.cxx 8630
"WND(118): Mauvais index enfant", //379 kifltk.cxx 8846
"WND(100): Ne peut recréer un widget f_arbre", //380 kifltk.cxx 9055
"WND(121): widget f_arbre n'a pas encore été crée", //381 kifltk.cxx 9076
"WND(125): Label vide, création du widget impossible", //382 kifltk.cxx 9099
"WND(341): Mauvais type pour le premier élément", //383 kifltk.cxx 9162
"WND(100): Ne peut recréer un widget f_parcourirfichiers", //384 kifltk.cxx 9713
"WND(127): Vous devez fournir une fonction de rappel 'avec'", //385 kifltk.cxx 9716
"WND(101): f_parcourirfichiers non initialisé", //386 kifltk.cxx 9742
"Version de KiF incompatible", //387 kifltk.cxx 10184
"fenêtre", //388 kifltk.cxx 10193
"f_boite", //389 kifltk.cxx 10204
"f_onglet", //390 kifltk.cxx 10208
"f_groupe", //391 kifltk.cxx 10212
"f_choix", //392 kifltk.cxx 10216
"f_éditeur", //393 kifltk.cxx 10224
"f_entrée", //394 kifltk.cxx 10228
"f_table", //395 kifltk.cxx 10232
"f_sortie", //396 kifltk.cxx 10236
"f_défilement", //397 kifltk.cxx 10240
"f_glissière", //398 kifltk.cxx 10244
"f_image", //399 kifltk.cxx 10248
"f_parcourirfichiers", //400 kifltk.cxx 10252
"f_parcourir", //401 kifltk.cxx 10256
"f_noeudarbre", //402 kifltk.cxx 10260
"f_arbre", //403 kifltk.cxx 10264
"crée", //404 kifltk.cxx 10280
"crée(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée une fenêtre sans widgets, largeur et hauteur sont optionnels", //405 kifltk.cxx 10280
"commence", //406 kifltk.cxx 10281
"commence(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée une fenêtre et commence l'initialisation, largeur et hauteur sont optionnels", //407 kifltk.cxx 10281
"fin(): fin création", //408 kifltk.cxx 10282
"lance", //409 kifltk.cxx 10283
"lance(): lance la boucle principale de l'interface graphique", //410 kifltk.cxx 10283
"ferme", //411 kifltk.cxx 10284
"ferme(): ferme la fenêtre", //412 kifltk.cxx 10284
"àlafermeture", //413 kifltk.cxx 10285
"àlafermeture(fonction,objet): Fonction de rappel lors de la fermeture", //414 kifltk.cxx 10285
"autemps", //415 kifltk.cxx 10286
"autemps(fonction,t,objet): Initialise une fonction de rappel pour chaque intervalle de temps t", //416 kifltk.cxx 10286
"initialisepolices", //417 kifltk.cxx 10288
"initialisepolices(): Charge les polices systèmes. Renvoie le nombre de polices disponibles", //418 kifltk.cxx 10288
"nompolice", //419 kifltk.cxx 10289
"nompolice(nombre num): nom de la police.", //420 kifltk.cxx 10289
"taillepolices", //421 kifltk.cxx 10290
"taillepolices(nombre num): renvoie un tableau des tailles disponibles de police.", //422 kifltk.cxx 10290
"numéropolice", //423 kifltk.cxx 10291
"numéropolice(): Renvoie le nombre de polices disponibles.", //424 kifltk.cxx 10291
"demande", //425 kifltk.cxx 10293
"demande(chaine msg,chaine boutonmsg1,chaine boutonmsg2,chaine boutonmsg3): Fenêtre pour poser une question", //426 kifltk.cxx 10293
"alerte", //427 kifltk.cxx 10294
"alerte(chaine msg): fenêtre pour afficher une alerte", //428 kifltk.cxx 10294
"menu", //429 kifltk.cxx 10295
"menu(tableau,nombre x,nombre y,nombre  largeur, nombre h): initialise un menu avec ses fonction de rappels", //430 kifltk.cxx 10295
"redimensionnable", //431 kifltk.cxx 10296
"redimensionnable(objet): objet redimensionnable", //432 kifltk.cxx 10296
"taillemax", //433 kifltk.cxx 10297
"taillemax(nombre minw,nombre minh, nombre maxw,nombre maxh): définit les tailles maximum d'agrandissement de la fenêtre", //434 kifltk.cxx 10297
"modal", //435 kifltk.cxx 10298
"modal(bool b): Si vrai rfin la fenêtre modale. Si pas de paramètres renvoie si la fenêtre est modale", //436 kifltk.cxx 10298
"bords", //437 kifltk.cxx 10299
"bords(bool b): Si vrai ajoute ou retire les bords. Si pas de paramètres renvoie si la fenêtre a des bords", //438 kifltk.cxx 10299
"couleurarrièreplan", //439 kifltk.cxx 10300
"couleurarrièreplan(nombre c|chaine c): initialise la couleur d'arrière-plan", //440 kifltk.cxx 10300
"couleurdessin", //441 kifltk.cxx 10302
"couleurdessin(nombre c|chaine c): Couleur des prochains objets dessinés", //442 kifltk.cxx 10302
"rectangle", //443 kifltk.cxx 10303
"rectangle(nombre x,nombre y,nombre  largeur, nombre  hauteur, chaine c|nombre c): Dessine un rectangle avec la couleur optionnelle c", //444 kifltk.cxx 10303
"sommet", //445 kifltk.cxx 10304
"sommet(réel x,réel y): Ajoute un sommet à une structure complexe", //446 kifltk.cxx 10304
"rectangleplein", //447 kifltk.cxx 10305
"rectangleplein(nombre x,nombre y,nombre  largeur, nombre  hauteur, chaine c|nombre c): Remplit un rectangle avec la couleur optionnelle c", //448 kifltk.cxx 10305
"formeligne", //449 kifltk.cxx 10306
"formeligne(chaine type,nombre largeur): Sélectionne la forme de la ligne et son épaisseur", //450 kifltk.cxx 10306
"polygone", //451 kifltk.cxx 10307
"polygone(nombre x,nombre y,nombre x1, nombre y1,nombre x2, nombre y2, nombre x3, nombre y3): Dessine un polygone, x3 et y3 sont optionnels", //452 kifltk.cxx 10307
"boucle", //453 kifltk.cxx 10308
"boucle(nombre x,nombre y,nombre x1, nombre y1,nombre x2, nombre y2, nombre x3, nombre y3): Dessine une série de lignes, x3 et y3 sont optionnels", //454 kifltk.cxx 10308
"ligne", //455 kifltk.cxx 10309
"ligne(nombre x,nombre y,nombre x1, nombre y1,nombre x2, nombre y2): Dessine une ligne entre deux points, x2 et y2 sont optionnels", //456 kifltk.cxx 10309
"empilematrice", //457 kifltk.cxx 10311
"empilematrice(): Sauvegarde la transformation courante", //458 kifltk.cxx 10311
"dépilematrice", //459 kifltk.cxx 10312
"dépilematrice(): Restaure la transformation courante", //460 kifltk.cxx 10312
"échelle", //461 kifltk.cxx 10313
"échelle(réel x,réel y)|(x): Met à l'échelle la transformation courante", //462 kifltk.cxx 10313
"translate", //463 kifltk.cxx 10314
"translate(réel x,réel y): translate la transformation courante", //464 kifltk.cxx 10314
"pivote", //465 kifltk.cxx 10315
"pivote(réel d): tourne de d degrés la transformation courante", //466 kifltk.cxx 10315
"multmatrice", //467 kifltk.cxx 10316
"multmatrice(réel a,réel b,réel c,réel d, réel x, réel y): combine les transformations", //468 kifltk.cxx 10316
"transformex", //469 kifltk.cxx 10318
"transformex(réel x,réel y): Transforme les coordonnées en X sur la base de la matrice courante de transformation", //470 kifltk.cxx 10318
"transformey", //471 kifltk.cxx 10319
"transformey(réel x,réel y): Transforme les coordonnées en Y sur la base de la matrice courante de transformation", //472 kifltk.cxx 10319
"transformedx", //473 kifltk.cxx 10320
"transformedx(réel x,réel y): Transforme la distance DX sur la base de la matrice courante de transformation", //474 kifltk.cxx 10320
"transformedy", //475 kifltk.cxx 10321
"transformedy(réel x,réel y): Transforme la distance DY sur la base de la matrice courante de transformation", //476 kifltk.cxx 10321
"transformesommets", //477 kifltk.cxx 10322
"transformesommets(réel x,réel y): ajoute les transformations à la liste des sommets", //478 kifltk.cxx 10322
"débutpoints", //479 kifltk.cxx 10324
"débutpoints(): Commence à accumuler des sommets", //480 kifltk.cxx 10324
"finpoints", //481 kifltk.cxx 10325
"finpoints(): Cesse d'accumuler des sommets", //482 kifltk.cxx 10325
"débutligne", //483 kifltk.cxx 10327
"débutligne(): Commence à dessiner des lignes.", //484 kifltk.cxx 10327
"finligne", //485 kifltk.cxx 10328
"finligne(): Cesse d'ajouter des lignes", //486 kifltk.cxx 10328
"débutboucle", //487 kifltk.cxx 10330
"débutboucle(): Commence à dessiner des séquences de lignes qui se referment.", //488 kifltk.cxx 10330
"finboucle", //489 kifltk.cxx 10331
"finboucle():    Cesse d'accumuler des séquences de lignes qui se referment.", //490 kifltk.cxx 10331
"débutpolygone", //491 kifltk.cxx 10333
"débutpolygone(): Commence à dessiner un polygone plein convexe", //492 kifltk.cxx 10333
"finpolygone", //493 kifltk.cxx 10334
"finpolygone(): Cesse de dessiner un polygone plein convexe", //494 kifltk.cxx 10334
"débutpolygonecomplexe", //495 kifltk.cxx 10336
"débutpolygonecomplexe(): Commence à dessiner un polygone complexe", //496 kifltk.cxx 10336
"trou", //497 kifltk.cxx 10337
"trou(): remplit les trous lors du dessin d'un polygone complexe.", //498 kifltk.cxx 10337
"finpolygonecomplexe", //499 kifltk.cxx 10338
"finpolygonecomplexe(): Cesser de dessiner un polygone complexe", //500 kifltk.cxx 10338
"arc", //501 kifltk.cxx 10341
"arc(nombre x,nombre y,nombre x1, nombre y1, réel a1, réel a2): Dessine un arc.\rarc(réel x,réel y,réel rad,réel a1,réel a2): Ajoute une série de points en arc de cercle au chemin courant.", //502 kifltk.cxx 10341
"courbe", //503 kifltk.cxx 10342
"courbe(réel x,réel y,réel x1,réel y1,réel x1,réel y2,réel x3,réel y3): Ajoute une série de points sur une courbe de Bezier au chemin courant. La fin de la courbe (et deux de ses points) est à la position x,y et x3,y3.", //504 kifltk.cxx 10342
"camembert", //505 kifltk.cxx 10343
"camembert(nombre x,nombre y,nombre x1, nombre y1, réel a1, réel a2): Dessine un camembert", //506 kifltk.cxx 10343
"point", //507 kifltk.cxx 10344
"point(nombre x,nombre y): Dessine un pixel", //508 kifltk.cxx 10344
"cercle", //509 kifltk.cxx 10345
"cercle(nombre x,nombre y,nombre r): Dessine un cercle", //510 kifltk.cxx 10345
"tailletexte", //511 kifltk.cxx 10346
"tailletexte(chaine l): Renvoie un dictionnaire avec largeur et hauteur comme clef pour donner les largeurs et hauteurs de la chaine en pixels", //512 kifltk.cxx 10346
"dessinetexte", //513 kifltk.cxx 10347
"dessinetexte(chaine l,nombre x,nombre y): Place un texte à la position x,y", //514 kifltk.cxx 10347
"redessine", //515 kifltk.cxx 10348
"redessine(): Redessine la fenêtre", //516 kifltk.cxx 10348
"couleurrvb", //517 kifltk.cxx 10349
"couleurrvb(chaine|nombre couleur)|(nombre r,nombre g,nombre b)|(tableau couleur): Renvoie soit un tableau de la décomposition en couleur ou renvoie la valeur de la couleur correspondant à la combinaison RVB fournie", //518 kifltk.cxx 10349
"police", //519 kifltk.cxx 10351
"police(chaine f,nombre sz): Définit le nom de la police et sa taille", //520 kifltk.cxx 10351
"sisouris", //521 kifltk.cxx 10352
"sisouris(nombre action, fonction,objet): Définit le nom de la fonction de rappel pour une action souris", //522 kifltk.cxx 10352
"sitouche", //523 kifltk.cxx 10353
"sitouche(nombre action, fonction,objet): Définit le nom de la fonction de rappel pour une action clavier", //524 kifltk.cxx 10353
"stylecurseur", //525 kifltk.cxx 10354
"stylecurseur(nombre curseurtype, nombre couleur,nombre couleur): Définit la forme du curseur", //526 kifltk.cxx 10354
"cache", //527 kifltk.cxx 10355
"cache(bool h): Cache la fenêtre si hauteur est vrai", //528 kifltk.cxx 10355
"montre", //529 kifltk.cxx 10356
"montre(): Montre la fenêtre", //530 kifltk.cxx 10356
"focus", //531 kifltk.cxx 10357
"focus(): Récupère le focus", //532 kifltk.cxx 10357
"chasse", //533 kifltk.cxx 10358
"chasse(): force à redessiner la fenêtre si celle-ci est dans une tâche", //534 kifltk.cxx 10358
"position", //535 kifltk.cxx 10359
"position()|(nombre x,nombre y): Renvoie la position de la fenêtre ou définit la position de la fenêtre", //536 kifltk.cxx 10359
"taille()|(nombre x,nombre y,nombre  largeur, nombre h): Renvoie la taille de la fenêtre ou définit la taille de la fenêtre", //537 kifltk.cxx 10360
"empileclip", //538 kifltk.cxx 10361
"empileclip(nombre x,nombre y,nombre  largeur, nombre h): Définit une région clippée de coordonnées x,y,largeur,h", //539 kifltk.cxx 10361
"dépileclip", //540 kifltk.cxx 10362
"dépileclip(): Relache une région clippée", //541 kifltk.cxx 10362
"réveil", //542 kifltk.cxx 10363
"réveil(): Réveille une fenêtre définie dans une tâche", //543 kifltk.cxx 10363
"verrouille", //544 kifltk.cxx 10364
"verrouille(): Verrou FLTK", //545 kifltk.cxx 10364
"déverrouille", //546 kifltk.cxx 10365
"déverrouille(): Deverouillage FLTK", //547 kifltk.cxx 10365
"image", //548 kifltk.cxx 10366
"image(wimage image,nombre x, nombre y, nombre  largeur, nombre h): Affiche une image", //549 kifltk.cxx 10366
"taillelabel", //550 kifltk.cxx 10367
"taillelabel(nombre c): définit ou renvoie la taille de la police du label", //551 kifltk.cxx 10367
"policelabel", //552 kifltk.cxx 10368
"policelabel(nombre c): définit ou renvoie la police du label", //553 kifltk.cxx 10368
"couleurlabel", //554 kifltk.cxx 10369
"couleurlabel(nombre c): définit ou renvoie la couleur du label", //555 kifltk.cxx 10369
"typelabel", //556 kifltk.cxx 10370
"typelabel(nombre c): définit ou renvoie le type du label", //557 kifltk.cxx 10370
"tempsmax", //558 kifltk.cxx 10371
"tempsmax(réel t): définit le temps maximum pour tous les widgets associes à cette fenêtre", //559 kifltk.cxx 10371
"label", //560 kifltk.cxx 10372
"label(chaine s): définit ou renvoie le texte du label", //561 kifltk.cxx 10372
"couleursélection", //562 kifltk.cxx 10373
"couleursélection(nombre couleur): Couleur de l'élément sélectionné", //563 kifltk.cxx 10373
"coords", //564 kifltk.cxx 10374
"coords(nombre c): renvoie le tableau des coordonnées du widget", //565 kifltk.cxx 10374
"créée", //566 kifltk.cxx 10375
"créée(): Renvoie vrai si l'objet a été crée", //567 kifltk.cxx 10375
"copie", //568 kifltk.cxx 10376
"copie(chaine): Copie la chaine dans le presse-papier", //569 kifltk.cxx 10376
"colle", //570 kifltk.cxx 10377
"colle(): Colle chaine depuis le presse-papier", //571 kifltk.cxx 10377
"aligne", //572 kifltk.cxx 10378
"aligne(nombre): définit l'alignement du label", //573 kifltk.cxx 10378
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée un f_défilement", //574 kifltk.cxx 10381
"redimensionnable", //575 kifltk.cxx 10382
"redimensionnable(objet): objet redimensionnable", //576 kifltk.cxx 10382
"montre(): Montre le widget", //577 kifltk.cxx 10383
"cache(): Cache le widget", //578 kifltk.cxx 10385
"truc", //579 kifltk.cxx 10391
"truc(chaine msg): Ajoute un message de 'truc' au widget", //580 kifltk.cxx 10391
"tempsmax(réel t): Définit le temps maximum pour cet objet", //581 kifltk.cxx 10397
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,nombre alignement,bool valeurf_glissière,chaine label): Crée une f_glissière avec ou sans valeur", //582 kifltk.cxx 10400
"bornes", //583 kifltk.cxx 10402
"bornes(nombre x,nombre y): définit les limites de la f_glissière", //584 kifltk.cxx 10402
"type", //585 kifltk.cxx 10403
"type(nombre x): Type de la f_glissière", //586 kifltk.cxx 10403
"typeboite", //587 kifltk.cxx 10404
"typeboite(nombre x): Définit le type de boite autour de la f_glissière", //588 kifltk.cxx 10404
"aligne(nombre): définit l'alignement de la f_glissière", //589 kifltk.cxx 10405
"pas", //590 kifltk.cxx 10406
"pas(nombre): définit le pas de la f_glissière", //591 kifltk.cxx 10406
"valeur", //592 kifltk.cxx 10407
"valeur(nombre): définit la valeur pour f_glissière ou renvoie sa valeur", //593 kifltk.cxx 10407
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée une f_boite", //594 kifltk.cxx 10424
"type(chaine|nombre boitetype): Définit le type de la f_boite", //595 kifltk.cxx 10425
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine type,chaine forme,chaine label): Crée un bouton. Si le type n'est pas fourni, il prend la valeur FL_Régulier par défaut.", //596 kifltk.cxx 10442
"quand", //597 kifltk.cxx 10443
"quand(chaine when1, chaine when2,...): Type de l'événement pour un bouton qui déclenche la fonction de rappel", //598 kifltk.cxx 10443
"raccourci", //599 kifltk.cxx 10444
"raccourci(chaine clef code): Définit un raccourci-clavier correspondant à l'activation du bouton", //600 kifltk.cxx 10444
"couleur", //601 kifltk.cxx 10445
"couleur(chaine code|nombre code): Définit la couleur du bouton", //602 kifltk.cxx 10445
"valeur(): renvoie la valeur du bouton courant", //603 kifltk.cxx 10446
"aligne(nombre): définit l'alignement du label du bouton", //604 kifltk.cxx 10447
"image(wimage im,chaine label,nombre labelalign): Dessine le bouton avec une image particulière", //605 kifltk.cxx 10448
"commence(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée un onglet fenêtre et commence l'initialisation", //606 kifltk.cxx 10465
"fin(): fin de la création de l'onglet", //607 kifltk.cxx 10466
"courant", //608 kifltk.cxx 10467
"courant(): Renvoie l'onglet courant actif", //609 kifltk.cxx 10467
"ajoute", //610 kifltk.cxx 10468
"ajoute(f_groupe g): Ajoute dynamiquement un nouvel onglet", //611 kifltk.cxx 10468
"retire", //612 kifltk.cxx 10469
"retire(f_groupe g): Retire un onglet", //613 kifltk.cxx 10469
"commence(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée un f_groupe au sein d'un onglet et commence l'initialisation", //614 kifltk.cxx 10487
"fin(): fin création du f_groupe", //615 kifltk.cxx 10488
"active", //616 kifltk.cxx 10491
"active(): Active cet onglet", //617 kifltk.cxx 10491
"arc(nombre x,nombre y,nombre x1, nombre y1, réel a1, réel a2): Dessine un arc de cercle", //618 kifltk.cxx 10507
"chargejpeg", //619 kifltk.cxx 10521
"chargejpeg(chaine filenom): Charge une image JPEG", //620 kifltk.cxx 10521
"chargegif", //621 kifltk.cxx 10522
"chargegif(chaine filenom): Charge une image GIF", //622 kifltk.cxx 10522
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée une f_table d'objets, et commence l'ajout", //623 kifltk.cxx 10524
"raz", //624 kifltk.cxx 10525
"raz(): Nettoie la f_table", //625 kifltk.cxx 10525
"ajoute(nombre R,nombre C,chaine v): Ajoute une valeur ligne R, colonne C", //626 kifltk.cxx 10526
"cellule", //627 kifltk.cxx 10527
"cellule(nombre R,nombre C): Renvoie la valeur de la cellule ligne R,colonne C", //628 kifltk.cxx 10527
"ligne", //629 kifltk.cxx 10528
"ligne()|(nombre nb): Définit le nombre de lignes", //630 kifltk.cxx 10528
"colonne", //631 kifltk.cxx 10529
"colonne()|(nombre nb): Définit le nombre de colonnes", //632 kifltk.cxx 10529
"hauteurligne", //633 kifltk.cxx 10530
"hauteurligne(nombre hauteur): Définit la hauteur de la ligne en pixel", //634 kifltk.cxx 10530
"largeurcolonne", //635 kifltk.cxx 10531
"largeurcolonne(nombre largeur): Définit là largeur de la colonne en pixel", //636 kifltk.cxx 10531
"police(nombre s): définit ou renvoie la police du texte", //637 kifltk.cxx 10532
"taillepolice", //638 kifltk.cxx 10533
"taillepolice(nombre c): définit ou renvoie la taille de la police du texte", //639 kifltk.cxx 10533
"couleurarrièreplan", //640 kifltk.cxx 10534
"couleurarrièreplan(nombre c): définit ou renvoie la couleur de la cellule en arrière-plan", //641 kifltk.cxx 10534
"couleuravantplan", //642 kifltk.cxx 10535
"couleuravantplan(nombre c): définit ou renvoie la couleur de la cellule en avant-plan", //643 kifltk.cxx 10535
"quand(chaine when1, chaine when2,...): Type de l'événement déclenchant la fonction de rappel", //644 kifltk.cxx 10536
"couleursélection(nombre couleur): Couleur pour les éléments sélectionnés", //645 kifltk.cxx 10537
"typeboite(nombre boitetype): type boite", //646 kifltk.cxx 10538
"entêteligne", //647 kifltk.cxx 10539
"entêteligne(nombre pos,chaine label): Définit le label de l'entête de la ligne à la ligne pos", //648 kifltk.cxx 10539
"entêtecolonne", //649 kifltk.cxx 10540
"entêtecolonne(nombre pos,chaine label): Définit le label de l'entête de la colonne à la colonne pos", //650 kifltk.cxx 10540
"hauteurentêteligne", //651 kifltk.cxx 10541
"hauteurentêteligne(nombre hauteur): la taille en pixel de l'entête de la ligne", //652 kifltk.cxx 10541
"largeurentêtecolonne", //653 kifltk.cxx 10542
"largeurentêtecolonne(nombre largeur): la taille en pixel de l'entête de la colonne", //654 kifltk.cxx 10542
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,booléen multiline,chaine label): Crée une f_entrée", //655 kifltk.cxx 10561
"valeur()|(chaine v): renvoie le buffer de f_entrée ou définit le buffer initial", //656 kifltk.cxx 10562
"insère", //657 kifltk.cxx 10563
"insère(chaine s,nombre p): insère s à la position p dans la f_entrée", //658 kifltk.cxx 10563
"sélection", //659 kifltk.cxx 10564
"sélection(): renvoie le texte sélectionné dans la f_entrée", //660 kifltk.cxx 10564
"couleur(chaine c|nombre c): définit ou renvoie la couleur du texte", //661 kifltk.cxx 10565
"mot", //662 kifltk.cxx 10566
"mot(nombre pos): Renvoie le mot à la position pos", //663 kifltk.cxx 10566
"police(chaine s): définit ou renvoie la police du texte", //664 kifltk.cxx 10567
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée un f_éditeur", //665 kifltk.cxx 10588
"sélection(): renvoie le texte sélectionné dans l'éditeur ou les coordonnées de la sélection selon la variable de réception", //666 kifltk.cxx 10589
"valeur()|(chaine v): renvoie le texte dans l'éditeur ou initialise l'éditeur", //667 kifltk.cxx 10590
"police(chaine s)|(): définit ou renvoie la police du texte", //668 kifltk.cxx 10592
"taillepolice(nombre c)|(): définit ou renvoie la taille de la police du texte", //669 kifltk.cxx 10593
"metstyle", //670 kifltk.cxx 10594
"metstyle(nombre start,nombre fin,chaine style): définit le style d'une portion de texte", //671 kifltk.cxx 10594
"charge", //672 kifltk.cxx 10595
"charge(chaine f): Charge le fichier dans l'éditeur", //673 kifltk.cxx 10595
"sauvegarde", //674 kifltk.cxx 10596
"sauvegarde(chaine f): Sauvegarde le contenu de l'éditeur dans le fichier f", //675 kifltk.cxx 10596
"remplace", //676 kifltk.cxx 10597
"remplace(chaine s,chaine sub,nombre i,bool casse): remplace s avec sub selon la casse à partir de i", //677 kifltk.cxx 10597
"remplacetout", //678 kifltk.cxx 10598
"remplacetout(chaine s,chaine sub,bool casse): remplace toutes les occurrences de s avec sub selon la casse", //679 kifltk.cxx 10598
"cherche", //680 kifltk.cxx 10599
"cherche(chaine s,nombre i,bool casse): Cherche la position de la sous-chaine s à partir de i", //681 kifltk.cxx 10599
"rcherche", //682 kifltk.cxx 10600
"rcherche(chaine s,nombre i,bool casse): Cherche la position de la sous-chaine s à partir de i en arrière", //683 kifltk.cxx 10600
"surbrille", //684 kifltk.cxx 10601
"surbrille(nombre start,nombre fin)|(): Met une portion de texte en  sur-brillance. Renvoie vrai si le texte est sur-ligné ou le texte en  sur-brillance", //685 kifltk.cxx 10601
"désurbrille", //686 kifltk.cxx 10602
"désurbrille(): Retire la  sur-brillance d'une portion de texte", //687 kifltk.cxx 10602
"stylecurseur(nombre curseurforme): Définit la forme du curseur", //688 kifltk.cxx 10603
"sisouris(nombre action,fonction f,objet o): fonction de rappel après un clic", //689 kifltk.cxx 10604
"sitouche(nombre action,fonction f,objet o): fonction de rappel quand une touche est pressée", //690 kifltk.cxx 10605
"sidéfilementvertical", //691 kifltk.cxx 10606
"sidéfilementvertical(fonction f,objet o): fonction de rappel pour un défilement vertical", //692 kifltk.cxx 10606
"sidéfilementhorizontal", //693 kifltk.cxx 10607
"sidéfilementhorizontal(fonction f,objet o): fonction de rappel pour un défilement horizontal", //694 kifltk.cxx 10607
"lestyle", //695 kifltk.cxx 10608
"lestyle(nombre start,nombre fin): renvoie un tableau de style pour chaque caractère de la section de texte", //696 kifltk.cxx 10608
"ajoutestyle", //697 kifltk.cxx 10609
"ajoutestyle(dictionaire style): Initialise les styles", //698 kifltk.cxx 10609
"détruit", //699 kifltk.cxx 10611
"détruit(): Détruit le texte sélectionné depuis l'éditeur", //700 kifltk.cxx 10611
"coupe", //701 kifltk.cxx 10612
"coupe(): Coupe le texte sélectionné depuis l'éditeur dans le presse-papier", //702 kifltk.cxx 10612
"copie(chaine s): Copie le texte sélectionné depuis l'éditeur dans le presse-papier (chaine s est optionnel)", //703 kifltk.cxx 10613
"colle(): Colle le texte sélectionné depuis le presse-papier", //704 kifltk.cxx 10614
"ligne(nombre l): renvoie la position de la ligne courante du chariot ou renvoie la ligne correspondant à la position", //705 kifltk.cxx 10617
"bornesligne", //706 kifltk.cxx 10618
"bornesligne(nombre l): renvoie le début et la fin de la position de la ligne courante du chariot ou renvoie les limites de la ligne  correspondant à une position", //707 kifltk.cxx 10618
"borneslignecarac", //708 kifltk.cxx 10619
"borneslignecarac(): renvoie le début et la fin de la position de la ligne courante du chariot ou renvoie les limites de la ligne  correspondant à une position en nombre de caractères", //709 kifltk.cxx 10619
"curseur(nombre l): renvoie la position du curseur courant du chariot ou définit la position du curseur", //710 kifltk.cxx 10620
"curseurcarac", //711 kifltk.cxx 10621
"curseurcarac(): renvoie la position du curseur courant du chariot en nombre de caractères", //712 kifltk.cxx 10621
"valigne", //713 kifltk.cxx 10622
"valigne(nombre l,bool surbrille): Va à la ligne l. Si vrai, met la ligne en  sur-brillance", //714 kifltk.cxx 10622
"annote", //715 kifltk.cxx 10623
"annote(chaine s|tableau v,chaine style,bool casse): Annote chaque occurence de s avec style", //716 kifltk.cxx 10623
"sautautomatique", //717 kifltk.cxx 10625
"sautautomatique(bool): Définit un mode coupure à la ligne automatique", //718 kifltk.cxx 10625
"annoteregexip", //719 kifltk.cxx 10626
"annoteregexip(chaine reg,chaine style): Annote chaque chaine correspondant à reg avec style", //720 kifltk.cxx 10626
"accumule", //721 kifltk.cxx 10642
"accumule(chaine s): Ajoute s à la fin de l'entrée", //722 kifltk.cxx 10642
"positionoctet", //723 kifltk.cxx 10643
"positionoctet(nombre i): Convertit une position en octet en une position en caractère", //724 kifltk.cxx 10643
"positioncarac", //725 kifltk.cxx 10644
"positioncarac(nombre i): Convertit une position en caractère en une position en octet", //726 kifltk.cxx 10644
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,booléen multiline,chaine label): Crée une f_sortie", //727 kifltk.cxx 10648
"valeur(chaine s): définit le buffer de f_sortie", //728 kifltk.cxx 10649
"crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée un f_choix", //729 kifltk.cxx 10671
"valeur(nombre s): définit la valeur d'initialisation du f_choix", //730 kifltk.cxx 10672
"menu(tableau s): Initialise le menu", //731 kifltk.cxx 10674
"crée(chaine intialdirectory,chaine filter,bool directory,chaine label): Ouvre un f_parcourirfichiers, en mode répertoire si directory est vrai", //732 kifltk.cxx 10693
"valeur(): Renvoie le fichier sélectionné", //733 kifltk.cxx 10694
"ferme(): Ferme le widget f_parcourirfichiers", //734 kifltk.cxx 10695
"ok", //735 kifltk.cxx 10696
"ok(): renvoie vrai si ok a été sélectionné", //736 kifltk.cxx 10696
"ouvre", //737 kifltk.cxx 10699
"ouvre(): Ouvre un fichier via une fenêtre mac", //738 kifltk.cxx 10699
"charge(): Charge un fichier via une fenêtre mac", //739 kifltk.cxx 10700
"crée(x,y,largeur,hauteur,label): Crée un widget f_parcourir", //740 kifltk.cxx 10703
"ajoute(label): Ajoute une chaine au widget f_parcourir", //741 kifltk.cxx 10704
"charge(filenom): Charge un fichier dans le widget f_parcourir", //742 kifltk.cxx 10705
"insère(l,label): Insère un label avant la ligne l", //743 kifltk.cxx 10706
"raz(): Vide le widget f_parcourir de toutes ses valeurs", //744 kifltk.cxx 10707
"valeur(): renvoie la valeur courante sélectionnée", //745 kifltk.cxx 10708
"sélectionné", //746 kifltk.cxx 10709
"sélectionné(): Renvoie la chaine sélectionnée.\rsélectionné(nombre i): Renvoie la chaine en position i", //747 kifltk.cxx 10709
"désélectionne", //748 kifltk.cxx 10710
"désélectionne(): Désélectionne tous les noeuds\rdésélectionne(nombre i): Désélectionne le noeud i", //749 kifltk.cxx 10710
"séparateurcolonne", //750 kifltk.cxx 10711
"séparateurcolonne(): Renvoie le caractère de séparation de colonnes.\rséparateurcolonne(chaine): Définit le caractère de séparation de colonnes", //751 kifltk.cxx 10711
"formatcarac", //752 kifltk.cxx 10712
"formatcarac(): Renvoie le caractère de formatage.\rformatcarac(chaine): Définit le caractère de formatage", //753 kifltk.cxx 10712
"label(): Renvoie le label du noeud.", //754 kifltk.cxx 10732
"profondeur", //755 kifltk.cxx 10733
"profondeur(): Renvoie la profondeur du noeud.", //756 kifltk.cxx 10733
"valeur(objet): Associe le noeud avec une valeur ou renvoie cette valeur.", //757 kifltk.cxx 10734
"nettoie", //758 kifltk.cxx 10735
"nettoie(): Retire l'objet associé.", //759 kifltk.cxx 10735
"couleurarrièreplan", //760 kifltk.cxx 10736
"couleurarrièreplan(nombre c): Définit ou renvoie la couleur de l'arrière-plan du noeud.", //761 kifltk.cxx 10736
"couleuravantplan", //762 kifltk.cxx 10737
"couleuravantplan(nombre c): Définit ou renvoie la couleur de l'avant-plan du noeud.", //763 kifltk.cxx 10737
"police(nombre c): Définit ou renvoie la police du noeud.", //764 kifltk.cxx 10738
"taillepolice(nombre c): Définit ou renvoie la taille de la police du noeud.", //765 kifltk.cxx 10739
"estfermé", //766 kifltk.cxx 10740
"estfermé(): Renvoie vrai si l'élément est fermé.", //767 kifltk.cxx 10740
"estouvert", //768 kifltk.cxx 10741
"estouvert(): Renvoie vrai si l'élément est ouvert.", //769 kifltk.cxx 10741
"estracine", //770 kifltk.cxx 10742
"estracine(): Renvoie vrai si l'élément est une racine.", //771 kifltk.cxx 10742
"estsélectionné", //772 kifltk.cxx 10743
"estsélectionné(): Renvoie vrai si l'élément est sélectionné.", //773 kifltk.cxx 10743
"estactif", //774 kifltk.cxx 10744
"estactif(): Renvoie vrai si l'élément est actif.", //775 kifltk.cxx 10744
"parent", //776 kifltk.cxx 10745
"parent(): Renvoie le dernier élément.", //777 kifltk.cxx 10745
"suivant", //778 kifltk.cxx 10746
"suivant(): Renvoie l'élément suivant.", //779 kifltk.cxx 10746
"précédent", //780 kifltk.cxx 10747
"précédent(): Renvoie l'élément précédent.", //781 kifltk.cxx 10747
"enfant", //782 kifltk.cxx 10748
"enfant(nombre i): Renvoie l'élément enfant à la position i.", //783 kifltk.cxx 10748
"enfants", //784 kifltk.cxx 10749
"enfants(): Renvoie le nombre d'enfants.", //785 kifltk.cxx 10749
"active(): Active l'élément courant.", //786 kifltk.cxx 10750
"désactive", //787 kifltk.cxx 10751
"désactive(): Désactive l'élément courant.", //788 kifltk.cxx 10751
"crée(nombre x,nombre y,nombre  hauteur,nombre  largeur,chaine label): Crée un arbre", //789 kifltk.cxx 10753
"raz(): Détruit les noeuds de l'arbre", //790 kifltk.cxx 10754
"ajoute(chaine path): Ajoute un noeud.\rajoute(f_noeudarbre e,chaine n): Ajoute un noeud après e.", //791 kifltk.cxx 10755
"estfermé(chaine path): Renvoie vrai si l'élément est fermé.\restfermé(f_noeudarbre e): Renvoie vrai si l'élément est fermé.", //792 kifltk.cxx 10756
"ferme(chaine path): Ferme l'élément.\rferme(f_noeudarbre e): Ferme l'élément.", //793 kifltk.cxx 10757
"ouvre(chaine path): Ouvre l'élément.\rouvre(f_noeudarbre e): Ouvre l'élément.", //794 kifltk.cxx 10758
"insère(f_noeudarbre e,chaine label,nombre pos): Insère un élément après e avec label à la position pos dans la liste des enfants.", //795 kifltk.cxx 10759
"retire(f_noeudarbre e): Retire l'élément e dans l'arbre.", //796 kifltk.cxx 10760
"insèreavant", //797 kifltk.cxx 10761
"insèreavant(f_noeudarbre e,chaine label): Insère un élément au-dessus de e avec label.", //798 kifltk.cxx 10761
"couleurconnecteur", //799 kifltk.cxx 10762
"couleurconnecteur(nombre c): Définit ou renvoie la couleur du connecteur.", //800 kifltk.cxx 10762
"styleconnecteur", //801 kifltk.cxx 10763
"styleconnecteur(nombre s): Définit ou renvoie le style du connecteur.", //802 kifltk.cxx 10763
"largeurconnecteur", //803 kifltk.cxx 10764
"largeurconnecteur(nombre s): Définit ou renvoie là largeur du connecteur.", //804 kifltk.cxx 10764
"ordretri", //805 kifltk.cxx 10765
"ordretri(nombre s): Définit ou renvoie l'ordre du tri.", //806 kifltk.cxx 10765
"modesélection", //807 kifltk.cxx 10766
"modesélection(nombre s): Définit ou renvoie le mode de sélection.", //808 kifltk.cxx 10766
"couleurnoeudarrièreplan", //809 kifltk.cxx 10768
"couleurnoeudarrièreplan(nombre c): Définit ou renvoie la couleur d'arrière-plan du noeud.", //810 kifltk.cxx 10768
"couleurnoeudavantplan", //811 kifltk.cxx 10769
"couleurnoeudavantplan(nombre c): Définit ou renvoie la couleur d'avant-plan du noeud.", //812 kifltk.cxx 10769
"policenoeud", //813 kifltk.cxx 10770
"policenoeud(nombre c): Définit ou renvoie la police du noeud.", //814 kifltk.cxx 10770
"taillenoeud", //815 kifltk.cxx 10771
"taillenoeud(nombre c): Définit ou renvoie la taille de la police du noeud.", //816 kifltk.cxx 10771
"margegauche", //817 kifltk.cxx 10773
"margegauche(nombre s):Définit ou récupère l'espace (en pixels) qui doit apparaitre entre la bordure gauche du widget et le contenu de l'arbre.", //818 kifltk.cxx 10773
"margehaut", //819 kifltk.cxx 10774
"margehaut(nombre s): Définit ou récupère l'espace (en pixels) qui doit apparaitre entre la bordure du haut du widget et le contenu de l'arbre.", //820 kifltk.cxx 10774
"racine", //821 kifltk.cxx 10776
"racine(): Renvoie la racine.", //822 kifltk.cxx 10776
"labelracine", //823 kifltk.cxx 10777
"labelracine(chaine r): Définit le label de la racine.", //824 kifltk.cxx 10777
"cliqué", //825 kifltk.cxx 10778
"cliqué(): Renvoie l'élément qui a été cliqué.", //826 kifltk.cxx 10778
"premier", //827 kifltk.cxx 10779
"premier(): Renvoie le premier élément.", //828 kifltk.cxx 10779
"cherche(chaine path): Renvoie l'élément correspondant au chemin path.", //829 kifltk.cxx 10780
"dernier", //830 kifltk.cxx 10781
"dernier(): Renvoie le dernier élément.", //831 kifltk.cxx 10781
"suivant(f_noeudarbre e): Renvoie l'élément suivant après e.", //832 kifltk.cxx 10782
"précédent(f_noeudarbre e): Renvoie l'élément précédent avant e.", //833 kifltk.cxx 10783
""};


Exported const char* KIFSYSSTRINGS[]={
"taille", //0 kifsys.cxx 190
"date", //1 kifsys.cxx 191
"pathnom", //2 kifsys.cxx 192
"nom", //3 kifsys.cxx 240
"Version de KiF incompatible", //4 kifsys.cxx 301
"sys", //5 kifsys.cxx 310
"commande", //6 kifsys.cxx 336
"commande(chaine com): exécute la commande 'com'", //7 kifsys.cxx 336
"créerépertoire", //8 kifsys.cxx 337
"créerépertoire(chaine chemin): Crée le répertoire 'chemin'", //9 kifsys.cxx 337
"listerépertoire", //10 kifsys.cxx 338
"tableau v=listerépertoire(chaine chemin): Retourne le contenu du répertoire", //11 kifsys.cxx 338
"cheminabsolu", //12 kifsys.cxx 339
"chaine cheminabsolu(chemin vchemin): Retourne le chemin absolu", //13 kifsys.cxx 339
"infosfichier", //14 kifsys.cxx 340
"dictionaire infosfichier(chaine chemin): Retourne les infos du fichier", //15 kifsys.cxx 340
"env", //16 kifsys.cxx 341
"env(chaine var)|(chaine var,chaine val): Retourne ou initialise le contenu de la variable d'environnement 'var'.", //17 kifsys.cxx 341
""};


Exported const char* KIFCURLSTRINGS[]={
"URL(%d): %s", //0 kifcurl.cxx 154     <------- Provide a translation
"e", //1 kifcurl.cxx 202
"URL(009): Mauvais nom de fichier", //2 kifcurl.cxx 204
"nom", //3 kifcurl.cxx 221
"taille", //4 kifcurl.cxx 243
"Version de KiF incompatible", //5 kifcurl.cxx 306
"curl", //6 kifcurl.cxx 322
"proxy", //7 kifcurl.cxx 338     <------- Provide a translation
"proxy(chaine prox): Connexion via un proxy", //8 kifcurl.cxx 338
"motdepasse", //9 kifcurl.cxx 339
"motdepasse(chaine utilisateur,chaine mdp): Fournir le mot de passe utilisateur", //10 kifcurl.cxx 339
"url", //11 kifcurl.cxx 340
"url(chaine chemin): Charge une URL\rurl(chaine chemin,chaine fichier): Charge le contenu du site pointé par l'URL dans un fichier", //12 kifcurl.cxx 340
"mavariable", //13 kifcurl.cxx 343
""};


Exported const char* KIFSQLITESTRINGS[]={
"SQLite(800): Une base données a déjà été ouverte avec cet objet", //0 kifsqlite.cxx 172
"SQLite(801): Erreur durant l'ouverture de la base de données='", //1 kifsqlite.cxx 179
"SQLite(802): Ne peut fermer cette base de données", //2 kifsqlite.cxx 190
"SQLite(803): Ne peut utiliser cette base de données", //3 kifsqlite.cxx 198
"SQLite(811): Erreur d'exécution=", //4 kifsqlite.cxx 211
"table1", //5 kifsqlite.cxx 238
"nom TEXT", //6 kifsqlite.cxx 238
"age INTEGER", //7 kifsqlite.cxx 238
"create table", //8 kifsqlite.cxx 239
"SQLite(805): Erreur de création=", //9 kifsqlite.cxx 255
"SQLite(808): Paramètre manquant dans insèrevia", //10 kifsqlite.cxx 270
"ok", //11 kifsqlite.cxx 274
"PRAGMA table_info(", //12 kifsqlite.cxx 276     <------- Provide a translation
"SQLite(897): Table inconnue", //13 kifsqlite.cxx 282
"INSERT INTO ", //14 kifsqlite.cxx 285     <------- Provide a translation
" VALUES (", //15 kifsqlite.cxx 287     <------- Provide a translation
"type", //16 kifsqlite.cxx 289
"TEXT", //17 kifsqlite.cxx 290     <------- Provide a translation
"BLOB", //18 kifsqlite.cxx 290     <------- Provide a translation
"VARCHAR", //19 kifsqlite.cxx 290     <------- Provide a translation
"INTEGER", //20 kifsqlite.cxx 293     <------- Provide a translation
"REAL", //21 kifsqlite.cxx 296     <------- Provide a translation
"FLOAT", //22 kifsqlite.cxx 296     <------- Provide a translation
"SQLite(899): Mauvaise liste de valeurs", //23 kifsqlite.cxx 309
"SQLite(893): Vous devez exécuter un COMMENCE d'abord", //24 kifsqlite.cxx 314
"SQLite(895): mauvaise valeur pour:", //25 kifsqlite.cxx 332
"SQLite(812): Transaction failed!", //26 kifsqlite.cxx 344     <------- Provide a translation
"BEGIN", //27 kifsqlite.cxx 359     <------- Provide a translation
" TRANSACTION;", //28 kifsqlite.cxx 364     <------- Provide a translation
"SQLite(805): Erreur de commencement de transaction=", //29 kifsqlite.cxx 371
"ROLLBACK", //30 kifsqlite.cxx 373     <------- Provide a translation
"COMMIT;", //31 kifsqlite.cxx 389
"SQLite(807): Erreur de transmission=", //32 kifsqlite.cxx 393
"SQLite(808): Paramètre manquant dans 'INSERE'", //33 kifsqlite.cxx 412
"insert into ", //34 kifsqlite.cxx 414     <------- Provide a translation
"nom", //35 kifsqlite.cxx 416
"toto", //36 kifsqlite.cxx 416
"age", //37 kifsqlite.cxx 416
") valeurs (", //38 kifsqlite.cxx 428
"SQLite(805): Erreur de création=", //39 kifsqlite.cxx 444
"Mauvaise expression SQL:", //40 kifsqlite.cxx 588
"Version de KiF incompatible", //41 kifsqlite.cxx 629
"SQLite est une marque déposée de Hipp, Wyrick & Company, Inc.", //42 kifsqlite.cxx 633
"sqlite", //43 kifsqlite.cxx 640
"ouvre", //44 kifsqlite.cxx 656
"ouvre(chaine chemin): ouvre un fichier de base de données", //45 kifsqlite.cxx 656
"ferme", //46 kifsqlite.cxx 657
"ferme(): ferme une base de données", //47 kifsqlite.cxx 657
"crée", //48 kifsqlite.cxx 658
"crée(x1,x2,x3): crée une table dans la base de données avec  les arguments x1,x2,x3.\rEx. mydb.crée('table1','nom TEXT PRIMARY KEY','age INTEGER');", //49 kifsqlite.cxx 658
"insère", //50 kifsqlite.cxx 659
"insère(chaine table,chaine colonne,nombre valeur,...): insère une ligne dans une table.\rEx. mydb.insère('table1','nom',nm,'age',i);", //51 kifsqlite.cxx 659
"insèrevia", //52 kifsqlite.cxx 660
"insèrevia(string table,int v1,...): Insère une ligne dans un mode commence/transmet.\rEx. mydb.insèrevia('table1',nm,i);", //53 kifsqlite.cxx 660
"lance", //54 kifsqlite.cxx 661
"lance(chaine sqlcommand): Exécute une commande SQL. Si la variable de réception est un tableau, toutes les valeurs extraites seront placées dedans. Si la variable de réception est un itérateur, alors chaque valeur sera un dictionnaire dont les noms seront les attributs de la table.", //55 kifsqlite.cxx 661
"exécute", //56 kifsqlite.cxx 662
"exécute(chaine sqlcommande): exécute une commande sql brute sans retour.", //57 kifsqlite.cxx 662
"commence", //58 kifsqlite.cxx 663
"commence(chaine mode): Pour entrer en mode transaction: le mode par défaut est DEFERRED: Les autres modes sont: IMMEDIATE et EXCLUSIVE", //59 kifsqlite.cxx 663
"transmet", //60 kifsqlite.cxx 664
"transmet(): Fin du mode différé. L'ensemble des commandes est alors transmis à la base de données pour exécution.", //61 kifsqlite.cxx 664
"mémoire", //62 kifsqlite.cxx 665
"mémoire(): renvoie la consommation mémoire de SQLite.", //63 kifsqlite.cxx 665
""};


Exported const char* KIFXMLSTRINGS[]={
"arff", //0 kifxml.cxx 184
"%KIF GENERATED", //1 kifxml.cxx 186     <------- Provide a translation
"@RELATION", //2 kifxml.cxx 187
"@ATTRIBUTE", //3 kifxml.cxx 189
"STRING\n", //4 kifxml.cxx 192
"NUMERIC\n", //5 kifxml.cxx 194
"@ATTRIBUTE class {", //6 kifxml.cxx 197
"@DATA", //7 kifxml.cxx 206
"xml", //8 kifxml.cxx 234
"<Weka name='%s'/>", //9 kifxml.cxx 238     <------- Provide a translation
"UTF-8", //10 kifxml.cxx 241
"class", //11 kifxml.cxx 245
"nom", //12 kifxml.cxx 247
"attribute", //13 kifxml.cxx 250
"type", //14 kifxml.cxx 254
"chaine", //15 kifxml.cxx 254
"réel", //16 kifxml.cxx 256
"valeurs", //17 kifxml.cxx 259
"instance", //18 kifxml.cxx 291
"id", //19 kifxml.cxx 293
"WKA(091): Ne peut sauvegarder la matrice", //20 kifxml.cxx 301
"XML(234): Déjà créé", //21 kifxml.cxx 312
"XML(102): Fichier inconnu", //22 kifxml.cxx 358
"XML(106): N'est pas un buffer XML", //23 kifxml.cxx 422
"XML(235): doc vide", //24 kifxml.cxx 472
"XML(097): Noeud XML vide", //25 kifxml.cxx 561
"XML(099): No document", //26 kifxml.cxx 564     <------- Provide a translation
"Version de KiF incompatible", //27 kifxml.cxx 582
"xmldoc", //28 kifxml.cxx 598
"charge", //29 kifxml.cxx 616
"charge(chaine nomfichier): charge un fichier xml", //30 kifxml.cxx 616
"àlafermeture", //31 kifxml.cxx 617
"àlafermeture(function,object): Fonction de rappel pour les balises fermantes", //32 kifxml.cxx 617
"analyse", //33 kifxml.cxx 618
"analyse(chaine tampon): analyse un tampon xml", //34 kifxml.cxx 618
"crée", //35 kifxml.cxx 619
"crée(chaine topnoeud): Crée un fichier XML avec topnoeud comme noeud racine", //36 kifxml.cxx 619
"xpath", //37 kifxml.cxx 620
"xpath(chaine myxpath): Renvoie un tableau de noeuds xml correspondant au chemin xpath myxpath", //38 kifxml.cxx 620
"sauvegarde", //39 kifxml.cxx 621
"save(string filename,string encoding): save an XML file. Encoding default is 'utf8'", //40 kifxml.cxx 621     <------- Provide a translation
"ferme", //41 kifxml.cxx 622
"ferme(): Ferme le fichier XML", //42 kifxml.cxx 622
"noeud", //43 kifxml.cxx 623
"noeud(): Retourne le noeud racine", //44 kifxml.cxx 623
"chainexml", //45 kifxml.cxx 624
"chainexml(chaine balise,chaine texte): Crée un fichier XML avec balise et texte", //46 kifxml.cxx 624
"racine", //47 kifxml.cxx 626
"racine(): Retourne le noeud racine", //48 kifxml.cxx 626
"document", //49 kifxml.cxx 627
"document(): renvoie le xmldoc associé à ce noeud", //50 kifxml.cxx 627
"suivant", //51 kifxml.cxx 628
"suivant(): noeud XML suivant", //52 kifxml.cxx 628
"délie", //53 kifxml.cxx 629
"délie(): Retire un noeud de la structure arborée", //54 kifxml.cxx 629
"détruit", //55 kifxml.cxx 630
"détruit(): Détruit la representation interne du noeud XML", //56 kifxml.cxx 630
"précédent", //57 kifxml.cxx 631
"précédent(): noeud xml précédent", //58 kifxml.cxx 631
"parent", //59 kifxml.cxx 632
"parent(): noeud xml parent", //60 kifxml.cxx 632
"enfant", //61 kifxml.cxx 633
"enfant(): noeud xml enfant", //62 kifxml.cxx 633
"nom(): nom du noeud", //63 kifxml.cxx 634
"ligne", //64 kifxml.cxx 635
"ligne(): Renvoie la ligne où se trouve le noeud", //65 kifxml.cxx 635
"id(): Renvoie l'identifieur du noeud (uniquement disponible avec les fonctions de rappel)", //66 kifxml.cxx 636
"xmltype", //67 kifxml.cxx 637
"xmltype(): Renvoie le type du noeud XML", //68 kifxml.cxx 637
"propriétés", //69 kifxml.cxx 638
"propriétés(): Retourne les propriétés du noeud xml", //70 kifxml.cxx 638
"contenu", //71 kifxml.cxx 639
"contenu(): Retourne le contenu textuel du noeud xml", //72 kifxml.cxx 639
"namespace", //73 kifxml.cxx 640
"namespace(): Retourne le tableau de namespace du noeud xml", //74 kifxml.cxx 640
"nouveau", //75 kifxml.cxx 641
"nouveau(chaine name): Crée un nouveau noeud XML", //76 kifxml.cxx 641
"chainexml(): Retourne l'arbre complet depuis le noeud courant sous la forme d'une chaine de caractères.", //77 kifxml.cxx 642
""};


Exported const char* KIFREGEXSTRINGS[]={
"RGX(001): Expression régulière fausse:", //0 kifregex.cxx 44
"RGX(001): Expression régulière fausse:", //1 kifregex.cxx 90
"RGX(001): Expression régulière fausse:", //2 kifregex.cxx 124
"Version de KiF incompatible", //3 kifregex.cxx 137
"regex", //4 kifregex.cxx 149
"éclatergx", //5 kifregex.cxx 150
"remplacergx", //6 kifregex.cxx 151
""};


Exported const char* KIFXMLSTRINGSINCLUDE[]={
"XML(098): On attend un noeud XML", //0 kifxml.h 52
"XML(097): Noeud XML vide", //1 kifxml.h 123
"XML(123): On attend un dictionnaire", //2 kifxml.h 350
"nom", //3 kifxml.h 414
"taille", //4 kifxml.h 436
"XML(234): Déjà créé", //5 kifxml.h 624
"XML(235): doc vide", //6 kifxml.h 639
"UTF-8", //7 kifxml.h 640
""};


Exported const char* KIFTHEINFOSTRINGS[]={
"log(","- log(): renvoie le log base 10 de la valeur",
"rcherche(","- rcherche(chaine sub,nombre pos): Renvoie la position de la sous-chaine sub par l'arrière commençant à la position pos\r- rcherche(chaine s,nombre i,bool casse): Cherche la position de la sous-chaine s à partir de i en arrière",
"droit(","- droit(nombre nb): renvoie les nb derniers caractères d'une chaine",
"regexip(","- regexip(sub): Trouve les chaines correspondant à sub",
"segmente(","- segmente(bool virgule,bool séparateur): Segmente un texte en mots et ponctuation. Si virgule vaut 'vrai' alors le séparateur décimal est une ',' sinon un '.'. Si 'séparateur' alors '3.000,10' est accepté comme nombre.",
"remplit(","- remplit(nombre nb,chaine c): crée une chaine de nb caractères c",
"remplace(","- remplace(sub,str): Remplace la sous-chaines correspondant à sub avec str\r- remplace(chaine s,chaine sub,nombre i,bool casse): remplace s avec sub selon la casse à partir de i",
"enfants(","- enfants(): Renvoie le nombre d'enfants.",
"chaine reçoit(","- chaine reçoit(nombre num): Reçoit une chaine provenant d'une application non KiF. 'num' doit être omis du côté client.",
"utf8(","- utf8(): convertit une chaine en LATIN en UTF8",
"débutpoints(","- débutpoints(): Commence à accumuler des sommets",
"retiredernier(","- retiredernier(nombre nb): retire les nb derniers caractères d'une chaine",
"éclate(","- éclate(chaine exploseur,tableau vect): explose une chaine selon exploseur et enregistre le résultat sous la forme d'un tableau. Si exploseur=='', alors la chaine est explosée en un tableau de caractères",
"captestd(","- captestd(bool): capte ou relâche la sortie standard",
"retirepremier(","- retirepremier(nombre nb): retire les nb premiers caractères d'une chaine",
"rognegauche(","- rognegauche(): retire les caractères d'espace à gauche",
"ord(","- ord(): renvoie le code ASCII du caractère en tête, or une liste de tous les codes ASCII si le receveur est un tableau",
"milieu(","- milieu(nombre pos,nombre nb): renvoie les nb caractères commençant à la position pos d'une chaine",
"enmajuscule(","- enmajuscule(): Teste si la chaine est uniquement en minuscule",
"racar(","- racar(): renvoie la racine carrée",
"instances(","- instances(): renvoie le nombre d'instances dans le weka",
"rogne(","- rogne(): retire les caractères d'espace",
"gauche(","- gauche(nombre nb): renvoie les nb premiers caractères d'une chaine",
"atan(","- atan(): arc tangent",
"renverse(","- renverse(): inverse la chaine",
"capteerr(","- capteerr(bool): capte ou relâche la sortie erreur",
"translate(","- translate(réel x,réel y): translate la transformation courante",
"nd(","- nd(nombre n,nombre d): Initialise le Numérateur et le Dénominateur de la fraction",
"dictionaire infosfichier(","- dictionaire infosfichier(chaine chemin): Retourne les infos du fichier",
"truc(","- truc(chaine msg): Ajoute un message de 'truc' au widget",
"dépile(","- dépile(i): retire le caractère à la position i\r- dépile(): retire le dernier caractère\r- dépile(nombre i): retire le ieme élément du tableau.\r- dépile(): retire le dernier élément du tableau.\r- dépile(chaine clef): retire l'élément correspondant à clef",
"modesélection(","- modesélection(nombre s): Définit ou renvoie le mode de sélection.",
"dernier(","- dernier(): renvoie le dernier caractère\r- dernier(): renvoie le dernier élément du tableau\r- dernier(): renvoie le dernier élément de la liste\r- dernier(): renvoie le dernier élément de la liste\r- dernier(): Renvoie le dernier élément.",
"boolean=regex(","- boolean=regex(rgx): Teste si l'expression regulière rgx s'applique à la chaine",
"hauteurentêteligne(","- hauteurentêteligne(nombre hauteur): la taille en pixel de l'entête de la ligne",
"insère(","- insère(i,s): insère la chaine s en position 'i'\r- insère(i,x): insère l'élément x à la position i\r- insère(chaine s,nombre p): insère s à la position p dans la f_entrée\r- insère(l,label): Insère un label avant la ligne l\r- insère(f_noeudarbre e,chaine label,nombre pos): Insère un élément après e avec label à la position pos dans la liste des enfants.\r- insère(chaine table,chaine colonne,nombre valeur,...): insère une ligne dans une table.\rEx. mydb.insère('table1','nom',nm,'age',i);",
"sommet(","- sommet(réel x,réel y): Ajoute un sommet à une structure complexe",
"taillepolices(","- taillepolices(nombre num): renvoie un tableau des tailles disponibles de police.",
"rognedroit(","- rognedroit(): retire les caractères d'espace à droite",
"majuscule(","- majuscule(): Met la chaine en majuscule",
"policenoeud(","- policenoeud(nombre c): Définit ou renvoie la police du noeud.",
"minuscule(","- minuscule(): Met la chaine en minuscule",
"enminuscule(","- enminuscule(): Teste si la chaine est uniquement en minuscule",
"format(","- format(chaine form): renvoie une chaine formatée selon le patron dans form. (ce format est le même que celui de sprintf en C++)",
"profondeur(","- profondeur(): Renvoie la profondeur du noeud dans le arbre\r- profondeur(): Renvoie la profondeur du noeud.",
"infos(","- infos(chaine): infos à propos d'une méthode",
"mois(","- mois(): renvoie le mois comme un nombre",
"octets(","- octets(): Renvoie la chaine comme tableau d'octets\r- octets(): Renvoie la chaine correspondant à un tableau d'octets. Chaque valeur doit être comprise entre 0..255",
"estalpha(","- estalpha(): Teste si une chaine ne contient que des caractères alphabétiques",
"finpoints(","- finpoints(): Cesse d'accumuler des sommets",
"tableau v=extrait(","- tableau v=extrait(nombre pos,chaine depuis,chaine c1,chaine c2...): extrait les sous-chaines entre 'depuis' et 'c1'...'cn' (la chaine la plus courte est utilisée). Renvoie un tableau de chaines",
"sidéfilementvertical(","- sidéfilementvertical(fonction f,objet o): fonction de rappel pour un défilement vertical",
"teste(","- teste(i): teste si i est une position valide dans le tableau\r- teste(i): teste si i est une clef valide dans le dictionnaire",
"estchiffre(","- estchiffre(): Teste si une chaine ne contient que des chiffres",
"latin(","- latin(): convertit une chaine UTF8 en LATIN",
"sélectionné(","- sélectionné(): Renvoie la chaine sélectionnée.\rsélectionné(nombre i): Renvoie la chaine en position i",
"estutf8(","- estutf8(): Renvoie vrai si la chaine est encodée en UTF8",
"car(","- car(): Renvoie le caractère dont le code est la valeur de la variable",
"sauve(","- sauve(filename,type): sauve le weka sur disque. type peut prendre deux valeurs: aarf ou xml",
"cherche(","- cherche(chaine sub,nombre pos): Renvoie la position de la sous-chaine sub commençant à la position pos\r- cherche(chaine s,nombre i,bool casse): Cherche la position de la sous-chaine s à partir de i\r- cherche(chaine path): Renvoie l'élément correspondant au chemin path.",
"remplaceregexip(","- remplaceregexip(sub,str): Remplace les chaines correspondant à sub avec str",
"tableau=regex(","- tableau=regex(rgx): Renvoie toutes les sous-chaines correspondant à rgx",
"position(","- position()|(nombre x,nombre y): Renvoie la position de la fenêtre ou définit la position de la fenêtre",
"évalue(","- évalue(): Evalue les métacaractères dans une chaine et retourne la chaine évaluée",
"remplacergx(","- remplacergx(rgx,str): Remplace la sous-chaine correspondant à rgx avec str",
"éclatergx(","- éclatergx(rgx): Explose  chaine avec l'expression regulière rgx. Renvoie un tableau de sous-chaines.",
"remet(","- remet(): Retourne le caractère dans le flux de lecture\r- remet(nb): renvoie nb caractères dans le flux",
"chaine=regex(","- chaine=regex(rgx): Renvoie la sous-chaine correspondant à rgx",
"active(","- active(): Active cet onglet\r- active(): Active l'élément courant.",
"empile(","- empile(a): rajoute en queue de tableau",
"stylecurseur(","- stylecurseur(nombre curseurtype, nombre couleur,nombre couleur): Définit la forme du curseur\r- stylecurseur(nombre curseurforme): Définit la forme du curseur",
"d(","- d(): Dénominateur de la fraction (avec un paramètre: met la valeur, sans paramètre renvoie la valeur)",
"nombre=regex(","- nombre=regex(rgx): Renvoie la position de la sous-chaine correspondant à rgx",
"ln(","- ln(): renvoie le log neperien",
"abs(","- abs(): renvoie la valeur absolue",
"cos(","- cos(): cosinus",
"exp(","- exp(): renvoie l'exponentiel",
"commute(","- commute(): Inverse le dénominateur et le numérateur de la fraction",
"partie(","- partie(bits x): Renvoie vrai si le bits de x sont tous dans les bits courants",
"méthodes(","- méthodes(): Retourne a tableau de toutes les méthodes disponibles",
"xmltype(","- xmltype(): Renvoie le type du noeud XML",
"#(","- #(): renvoie le complément en bits",
"duplique(","- duplique(): renvoie une duplication de la matrice",
"simplifie(","- simplifie(): Simplification",
"clefligne(","- clefligne(nombre l): Renvoie la clef correspondant à la ligne l ou tout les clefs en ligne de la matrice",
"clef(","- clef(): renvoie la clef de l'élément courant",
"arbre(","- arbre(valeur): crée un noeud d'arbre, avec valeur comme valeur. Les valeurs peuvent avoir n'importe quel type",
"n(","- n(): Numérateur de la fraction (avec un paramètre: met la valeur, sans paramètre renvoie la valeur)",
"désurbrille(","- désurbrille(): Retire la  sur-brillance d'une portion de texte",
"tan(","- tan(): tangente",
"transformesommets(","- transformesommets(réel x,réel y): ajoute les transformations à la liste des sommets",
"sin(","- sin(): sinus",
"lestyle(","- lestyle(nombre start,nombre fin): renvoie un tableau de style pour chaque caractère de la section de texte",
"lire(","- lire(): Lit une chaine depuis le clavier\r- lire(): Lit une chaine depuis le clavier",
"fusionne(","- fusionne(v): fusionne les éléments de v dans le conteneur",
"inverse(","- inverse(): inverse l'ordre des éléments dans le tableau\r- inverse(): inverse l'ordre des éléments dans la liste",
"finpolygone(","- finpolygone(): Cesse de dessiner un polygone plein convexe",
"joint(","- joint(chaine sep): concatène chaque élément dans le tableau en une chaine ou chaque élément est séparé des autres par sep\r- joint(chaine sep): concatène chaque élément dans la liste en une chaine ou chaque élément est séparé des autres par sep\r- joint(chaine clefsep,chaine sep): concatène chaque élément du dictionnaire en une chaine ou chaque clef est ajoutée avec clefsep et chaque élément est séparé des autres par sep",
"somme(","- somme(): Somme chaque élément\r- somme(): renvoie la somme de tous les éléments",
"produit(","- produit(): Multiplie chaque élément avec les autres\r- produit(): renvoie le produit de tous les éléments",
"fin(","- fin(): renvoie vrai quand la fin de la collection est atteinte\r- fin(): fin création\r- fin(): fin de la création de l'onglet\r- fin(): fin création du f_groupe",
"raz(","- raz(): nettoie le tableau\r- raz(): nettoie la liste\r- raz(): nettoie le dictionnaire\r- raz(): réinitialise une variable de temps\r- raz(): Nettoie la f_table\r- raz(): Vide le widget f_parcourir de toutes ses valeurs\r- raz(): Détruit les noeuds de l'arbre",
"min(","- min(): renvoie la valeur minimale dans un conteneur\r- min(): renvoie la valeur minimale de la matrice\r- min(): renvoie la valeur minimale du tableau de matrice",
"max(","- max(): renvoie la valeur maximale dans un conteneur\r- max(): renvoie la valeur maximale de la matrice\r- max(): renvoie la valeur maximale du tableau de matrice",
"trie(","- trie(compare): Trie le contenu du  tableau selon une fonction de comparaison, laquelle est optionnelle",
"envoie(","- envoie(nombre num,chaine s): Ecrit une chaine à destination d'une application non KiF. 'num' doit être omis du côté client.",
"trienombre(","- trienombre(): trie le contenu du tableau dont chaque élément est considéré comme un nombre.",
"motdepasse(","- motdepasse(chaine utilisateur,chaine mdp): Fournir le mot de passe utilisateur",
"trieréel(","- trieréel(): trie le contenu du tableau dont chaque élément est considéré comme un réel.",
"trielong(","- trielong(): trie le contenu du tableau dont chaque élément est considéré comme un long.",
"litln(","- litln(): lit une ligne depuis un fichier",
"triechaine(","- triechaine(): trie le contenu du tableau dont chaque élément est considéré comme une chaine.",
"applique(","- applique(a,b,c): applique sur chaque élément une fonction, ou applique toutes les fonctions dans le conteneur\r- applique(a,b,c): applique sur chaque élément une fonction, ou applique toutes les fonctions dans le conteneur.\r- applique(a,b,c): applique une fonction à tous les éléments du conteneur ou toutes les fonctions dans le conteneur",
"empileentête(","- empileentête(a): ajoute au début de la liste",
"test(","- test(i): teste si i est une position valide dans la liste",
"empileenqueue(","- empileenqueue(a): ajoute en queue de la liste",
"décompositionLU(","- décompositionLU(tableau permutations,bool arésoudre): Remplace la matrice courante avec la décomposition LU selon une permutation le long des lignes d'elle-même. arésoudre vaut 'vrai', si l'instruction suivante est résoudLU.",
"dépileentête(","- dépileentête(): retire l'élément en tête de la liste et le renvoie.",
"dépileenqueue(","- dépileenqueue(): retire le dernier élément de la liste et le renvoie.",
"clefs(","- clefs(): renvoies les clefs du dictionnaire comme tableau",
"ajoutenoeudsuivant(","- ajoutenoeudsuivant(arbre n): Ajoute n après le noeud courant",
"valeurs(","- valeurs(): renvoie les valeurs du dictionnaire comme un tableau",
"noeudsuivant(","- noeudsuivant(): renvoie le noeud suivant\r- noeudsuivant(arbre n): compare le noeud suivant avec n",
"noeudprécédent(","- noeudprécédent(): renvoie le noeud précédent\r- noeudprécédent(arbre n): compare le noeud précédent avec n",
"ajoute(","- ajoute(f_groupe g): Ajoute dynamiquement un nouvel onglet\r- ajoute(nombre R,nombre C,chaine v): Ajoute une valeur ligne R, colonne C\r- ajoute(label): Ajoute une chaine au widget f_parcourir\r- ajoute(chaine path): Ajoute un noeud.\rajoute(f_noeudarbre e,chaine n): Ajoute un noeud après e.",
"metdate(","- metdate(année,mois,jour,heure,min,sec): initialise une variable de temps",
"noeudenfant(","- noeudenfant(): renvoie le premier noeud enfant\r- noeudenfant(arbre n): Teste si le noeud courant est un enfant de n",
"noeudparent(","- noeudparent(): renvoie le noeud parent\r- noeudparent(arbre n): Teste si le noeud courant est un parent de n",
"couleurarrièreplan(","- couleurarrièreplan(nombre c|chaine c): initialise la couleur d'arrière-plan\r- couleurarrièreplan(nombre c): définit ou renvoie la couleur de la cellule en arrière-plan\r- couleurarrièreplan(nombre c): Définit ou renvoie la couleur de l'arrière-plan du noeud.",
"créeserveur(","- créeserveur(chaine hôte,nombre port,nombre nblients): crée un serveur. Si l'hôte est omis, le nom local est alors utilisé",
"derniernoeud(","- derniernoeud(): renvoie le dernier noeud frère\r- derniernoeud(arbre n): Teste si le noeud courant est le dernier enfant de n",
"ajoutenoeudenfant(","- ajoutenoeudenfant(arbre n): Ajoute n sous le noeud courant",
"ajoutenoeudprécédent(","- ajoutenoeudprécédent(arbre n): Ajoute n avant le noeud courant",
"largeurcolonne(","- largeurcolonne(nombre largeur): Définit là largeur de la colonne en pixel",
"isolenoeud(","- isolenoeud(): Extrait le noeud courant de l'arbre",
"retirenoeud(","- retirenoeud(): Détruit le sous-arbre",
"place(","- place(nombre instance,chaine class,chaine attribut, valeur): Ajoute un élément avec une valeur d'instance de classe,selon un attribut et une instance Id.",
"récupère(","- récupère(nombre instance,chaine class,chaine attribut): Renvoie l'élément avec une valeur de classe d'instance spécifique selon un attribut et une instance Id.",
"valeur(","- valeur(): renvoie la valeur de l'élément courant\r- valeur(nombre): définit la valeur pour f_glissière ou renvoie sa valeur\r- valeur(): renvoie la valeur du bouton courant\r- valeur()|(chaine v): renvoie le buffer de f_entrée ou définit le buffer initial\r- valeur()|(chaine v): renvoie le texte dans l'éditeur ou initialise l'éditeur\r- valeur(chaine s): définit le buffer de f_sortie\r- valeur(nombre s): définit la valeur d'initialisation du f_choix\r- valeur(): Renvoie le fichier sélectionné\r- valeur(): renvoie la valeur courante sélectionnée\r- valeur(objet): Associe le noeud avec une valeur ou renvoie cette valeur.",
"finboucle(","- finboucle():    Cesse d'accumuler des séquences de lignes qui se referment.",
"autemps(","- autemps(fonction,t,objet): Initialise une fonction de rappel pour chaque intervalle de temps t",
"valeurtype(","- valeurtype(): renvoie le type de la valeur de l'élément courant",
"estvaleurtype(","- estvaleurtype(chaine type): teste le type de l'élément courant",
"commence(","- commence(): initialise l'itérateur avec le début de la collection\r- commence(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée une fenêtre et commence l'initialisation, largeur et hauteur sont optionnels\r- commence(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée un onglet fenêtre et commence l'initialisation\r- commence(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée un f_groupe au sein d'un onglet et commence l'initialisation\r- commence(chaine mode): Pour entrer en mode transaction: le mode par défaut est DEFERRED: Les autres modes sont: IMMEDIATE et EXCLUSIVE",
"suivant(","- suivant(): élément suivant dans la collection\r- suivant(): Renvoie l'élément suivant.\r- suivant(f_noeudarbre e): Renvoie l'élément suivant après e.\r- suivant(): noeud XML suivant",
"image(","- image(wimage image,nombre x, nombre y, nombre  largeur, nombre h): Affiche une image\r- image(wimage im,chaine label,nombre labelalign): Dessine le bouton avec une image particulière",
"ouvreenécriture(","- ouvreenécriture(chaine fichiername): ouvre un fichier en écriture",
"nfin(","- nfin(): renvoie vrai tant que la fin de la collection n'a pas été atteinte (~fin())",
"date(","- date(): renvoie la date comme chaine",
"largeurconnecteur(","- largeurconnecteur(nombre s): Définit ou renvoie là largeur du connecteur.",
"année(","- année(): renvoie l'année comme un nombre",
"écrit(","- écrit(chaine s1,chaine s2,): écrit les chaines dans le fichier\r- écrit(nombre s1,nombre s2,): écrit les codes de caractères dans le fichier\r- écrit(nombre num,chaine s): Ecrit une chaine s sur le connecteur. Du côté serveur, 'num' est l'identifiant client renvoyé par 'attend'. En revanche du côté client, utiliser 'écrit(chaine s)'",
"jour(","- jour(): renvoie le jour comme un nombre",
"heure(","- heure(): renvoie l'heure comme un nombre",
"minute(","- minute(): renvoie les minutes comme un nombre",
"seconde(","- seconde(): renvoie les secondes comme un nombre",
"remplacetout(","- remplacetout(chaine s,chaine sub,bool casse): remplace toutes les occurrences de s avec sub selon la casse",
"résoudLU(","- résoudLU(tableau permutations,tableau valeurs): résoud les équations linéaires basées sur le contenu de valeurs. Renvoie le résultat dans valeurs.",
"ouvreenajout(","- ouvreenajout(chaine fichiername): ouvre un fichier en ajout",
"ouvreenlecture(","- ouvreenlecture(chaine fichiername): ouvre un fichier en lecture",
"litun(","- litun(): lecture d'un caractère depuis le fichier",
"ligne(","- ligne(x): renvoie la ligne x comme dictionnaire ou comme vecteurmatrice\r- ligne(nombre x,nombre y,nombre x1, nombre y1,nombre x2, nombre y2): Dessine une ligne entre deux points, x2 et y2 sont optionnels\r- ligne()|(nombre nb): Définit le nombre de lignes\r- ligne(nombre l): renvoie la position de la ligne courante du chariot ou renvoie la ligne correspondant à la position\r- ligne(): Renvoie la ligne où se trouve le noeud",
"lit(","- lit(): lit une ligne depuis un fichier",
"positionne(","- positionne(nombre p): positionne le curseur du fichier à p",
"dit(","- dit(): renvoie la position du  curseur courant dans le fichier",
"débutpolygonecomplexe(","- débutpolygonecomplexe(): Commence à dessiner un polygone complexe",
"findefichier(","- findefichier(): renvoie vrai quand la fin de fichier est atteinte",
"nombre attend(","- nombre attend(): attend qu'un client se connecte et renvoie son identifiant.",
"charge(","- charge(chaine kifpathname): charge un programme KiF\r- charge(chaine f): Charge le fichier dans l'éditeur\r- charge(): Charge un fichier via une fenêtre mac\r- charge(filenom): Charge un fichier dans le widget f_parcourir\r- charge(chaine nomfichier): charge un fichier xml",
"compile(","- compile(chaine kifpathname): Compile un programme KiF sous la forme d'une chaine. Renvoie un identifiant sur la première instruction à exécuter",
"exécute(","- exécute(nombre i): Exécute un programme depuis l'instruction i (renvoyé par compile)\r- exécute(chaine sqlcommande): exécute une commande sql brute sans retour.",
"kvariables(","- kvariables(): Renvoie les variables actives sous la forme d'un dictionnaire",
"exécutetâche(","- exécutetâche(nombre i): Exécute en parallèle un programme compilé depuis l'identifiant renvoyé",
"finexécution(","- finexécution(): Renvoie vrai si le programme s'est entièrement exécuté",
"numéropolice(","- numéropolice(): Renvoie le nombre de polices disponibles.",
"fonctiondebug(","- fonctiondebug(fonction,objet): Initialise la fonction de debuggage qui sera appelée pendant l'exécution",
"lafonction(","- lafonction(chaine frame): Retourne une fonction declarée du côté serveur",
"fermedebug(","- fermedebug(): Fin du mode debug commencé avec fonctiondebug",
"ouvre(","- ouvre(): ouvre une session KiF\r- ouvre(): Ouvre un fichier via une fenêtre mac\r- ouvre(chaine path): Ouvre l'élément.\rouvre(f_noeudarbre e): Ouvre l'élément.\r- ouvre(chaine chemin): ouvre un fichier de base de données",
"nettoie(","- nettoie(): ferme une session KiF\r- nettoie(): Retire l'objet associé.",
"àlafermeture(","- àlafermeture(fonction,objet): Fonction de rappel lors de la fermeture\r- àlafermeture(function,object): Fonction de rappel pour les balises fermantes",
"nom(","- nom(): Renvoie le chemin du fichier KiF\r- nom(): nom du noeud",
"exportées(","- exportées(): Renvoie la liste des méthodes exportées",
"bornes(","- bornes(x,y): Renvoie les bornes de la matrice\r- bornes(nombre x,nombre y): définit les limites de la f_glissière",
"namespace(","- namespace(): Retourne le tableau de namespace du noeud xml",
"enfant(","- enfant(nombre i): Renvoie l'élément enfant à la position i.\r- enfant(): noeud xml enfant",
"colonne(","- colonne(x): renvoie la colonne x comme dictionnaire ou comme vecteurmatrice\r- colonne()|(nombre nb): Définit le nombre de colonnes",
"clefcol(","- clefcol(nombre c): renvoie la clef correspondant à la colonne c ou toutes les clefs en colonne de la matrice",
"transposée(","- transposée(): renvoie la matrice transposée",
"déterminant(","- déterminant(): renvoie le déterminant de la matrice",
"inserse(","- inserse(): Renvoie la matrice inversée",
"colle(","- colle(): Colle chaine depuis le presse-papier\r- colle(): Colle le texte sélectionné depuis le presse-papier",
"décompositionbandeLU(","- décompositionbandeLU(bool local,tableau res): renvoie une factorisation LU d'une matrice bande",
"solveurbandeLU(","- solveurbandeLU(bool local,tableau res): renvoie une factorisation LU d'une matrice bande",
"index(","- index(): renvoie l'index de la ligne ou de la colonne de la matrice correspondante",
"connecte(","- connecte(chaine hôte,nombre port): connexion à un serveur",
"chasse(","- chasse(): force à redessiner la fenêtre si celle-ci est dans une tâche",
"chaine lit(","- chaine lit(nombre num): lit une chaine sur le connecteur. Du côté serveur, num est l'identifiant du client. Utiliser 'lit()' sans paramètre du côté client",
"lance(","- lance(nombre num,chaine stop): Uniquement du côté serveur. Exécute des invocations de méthodes distantes. 'stop' est une chaine qui permet l'arrêt de ce service par le client.\r- lance(): lance la boucle principale de l'interface graphique\r- lance(chaine sqlcommand): Exécute une commande SQL. Si la variable de réception est un tableau, toutes les valeurs extraites seront placées dedans. Si la variable de réception est un itérateur, alors chaque valeur sera un dictionnaire dont les noms seront les attributs de la table.",
"débutpolygone(","- débutpolygone(): Commence à dessiner un polygone plein convexe",
"ferme(","- ferme(nombre num): Ferme un connecteur. Du côté serveur, si 'num' est fourni, il ferme la connexion pour un client donné.\r- ferme(): ferme la fenêtre\r- ferme(): Ferme le widget f_parcourirfichiers\r- ferme(chaine path): Ferme l'élément.\rferme(f_noeudarbre e): Ferme l'élément.\r- ferme(): ferme une base de données\r- ferme(): Ferme le fichier XML",
"bloquant(","- bloquant(bool flag): Si 'flag' est 'vrai', le connecteur fonctionne en mode bloquant.",
"tempsmax(","- tempsmax(nombre t): Définit un temps maximum de 't' secondes pour le connecteur\r- tempsmax(réel t): définit le temps maximum pour tous les widgets associes à cette fenêtre\r- tempsmax(réel t): Définit le temps maximum pour cet objet",
"désactive(","- désactive(): Désactive l'élément courant.",
"nomhôte(","- nomhôte(): Retourne le nom de l'hôte",
"port(","- port(): renvoie le port courant",
"nomclient(","- nomclient(nombre num): Retourne le nom du client",
"parent(","- parent(): Renvoie le dernier élément.\r- parent(): noeud xml parent",
"sidéfilementhorizontal(","- sidéfilementhorizontal(fonction f,objet o): fonction de rappel pour un défilement horizontal",
"laclasse(","- laclasse(chaine frame): Retourne un identifiant objet déclaré du côté serveur",
"crée(","- crée(nombre x,nombre y,nombre  largeur, nombre  hauteur,chaine titre): Crée une fenêtre sans widgets, largeur et hauteur sont optionnels\r- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée un f_défilement\r- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,nombre alignement,bool valeurf_glissière,chaine label): Crée une f_glissière avec ou sans valeur\r- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée une f_boite\r- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine type,chaine forme,chaine label): Crée un bouton. Si le type n'est pas fourni, il prend la valeur FL_Régulier par défaut.\r- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée une f_table d'objets, et commence l'ajout\r- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,booléen multiline,chaine label): Crée une f_entrée\r- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée un f_éditeur\r- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,booléen multiline,chaine label): Crée une f_sortie\r- crée(nombre x,nombre y,nombre  largeur,nombre  hauteur,chaine label): Crée un f_choix\r- crée(chaine intialdirectory,chaine filter,bool directory,chaine label): Ouvre un f_parcourirfichiers, en mode répertoire si directory est vrai\r- crée(x,y,largeur,hauteur,label): Crée un widget f_parcourir\r- crée(nombre x,nombre y,nombre  hauteur,nombre  largeur,chaine label): Crée un arbre\r- crée(x1,x2,x3): crée une table dans la base de données avec  les arguments x1,x2,x3.\rEx. mydb.crée('table1','nom TEXT PRIMARY KEY','age INTEGER');\r- crée(chaine topnoeud): Crée un fichier XML avec topnoeud comme noeud racine",
"initialisepolices(","- initialisepolices(): Charge les polices systèmes. Renvoie le nombre de polices disponibles",
"insèrevia(","- insèrevia(string table,int v1,...): Insère une ligne dans un mode commence/transmet.\rEx. mydb.insèrevia('table1',nm,i);",
"valigne(","- valigne(nombre l,bool surbrille): Va à la ligne l. Si vrai, met la ligne en  sur-brillance",
"nompolice(","- nompolice(nombre num): nom de la police.",
"demande(","- demande(chaine msg,chaine boutonmsg1,chaine boutonmsg2,chaine boutonmsg3): Fenêtre pour poser une question",
"noeud(","- noeud(): Retourne le noeud racine",
"alerte(","- alerte(chaine msg): fenêtre pour afficher une alerte",
"estouvert(","- estouvert(): Renvoie vrai si l'élément est ouvert.",
"pivote(","- pivote(réel d): tourne de d degrés la transformation courante",
"menu(","- menu(tableau,nombre x,nombre y,nombre  largeur, nombre h): initialise un menu avec ses fonction de rappels\r- menu(tableau s): Initialise le menu",
"analyse(","- analyse(chaine tampon): analyse un tampon xml",
"redimensionnable(","- redimensionnable(objet): objet redimensionnable\r- redimensionnable(objet): objet redimensionnable",
"taillemax(","- taillemax(nombre minw,nombre minh, nombre maxw,nombre maxh): définit les tailles maximum d'agrandissement de la fenêtre",
"modal(","- modal(bool b): Si vrai rfin la fenêtre modale. Si pas de paramètres renvoie si la fenêtre est modale",
"bords(","- bords(bool b): Si vrai ajoute ou retire les bords. Si pas de paramètres renvoie si la fenêtre a des bords",
"couleurdessin(","- couleurdessin(nombre c|chaine c): Couleur des prochains objets dessinés",
"rectangle(","- rectangle(nombre x,nombre y,nombre  largeur, nombre  hauteur, chaine c|nombre c): Dessine un rectangle avec la couleur optionnelle c",
"rectangleplein(","- rectangleplein(nombre x,nombre y,nombre  largeur, nombre  hauteur, chaine c|nombre c): Remplit un rectangle avec la couleur optionnelle c",
"entêtecolonne(","- entêtecolonne(nombre pos,chaine label): Définit le label de l'entête de la colonne à la colonne pos",
"formeligne(","- formeligne(chaine type,nombre largeur): Sélectionne la forme de la ligne et son épaisseur",
"polygone(","- polygone(nombre x,nombre y,nombre x1, nombre y1,nombre x2, nombre y2, nombre x3, nombre y3): Dessine un polygone, x3 et y3 sont optionnels",
"boucle(","- boucle(nombre x,nombre y,nombre x1, nombre y1,nombre x2, nombre y2, nombre x3, nombre y3): Dessine une série de lignes, x3 et y3 sont optionnels",
"empilematrice(","- empilematrice(): Sauvegarde la transformation courante",
"dépilematrice(","- dépilematrice(): Restaure la transformation courante",
"margegauche(","- margegauche(nombre s):Définit ou récupère l'espace (en pixels) qui doit apparaitre entre la bordure gauche du widget et le contenu de l'arbre.",
"échelle(","- échelle(réel x,réel y)|(x): Met à l'échelle la transformation courante",
"point(","- point(nombre x,nombre y): Dessine un pixel",
"multmatrice(","- multmatrice(réel a,réel b,réel c,réel d, réel x, réel y): combine les transformations",
"transformex(","- transformex(réel x,réel y): Transforme les coordonnées en X sur la base de la matrice courante de transformation",
"labelracine(","- labelracine(chaine r): Définit le label de la racine.",
"transformey(","- transformey(réel x,réel y): Transforme les coordonnées en Y sur la base de la matrice courante de transformation",
"transformedy(","- transformedy(réel x,réel y): Transforme la distance DY sur la base de la matrice courante de transformation",
"transformedx(","- transformedx(réel x,réel y): Transforme la distance DX sur la base de la matrice courante de transformation",
"racine(","- racine(): Renvoie la racine.\r- racine(): Retourne le noeud racine",
"débutligne(","- débutligne(): Commence à dessiner des lignes.",
"précédent(","- précédent(): Renvoie l'élément précédent.\r- précédent(f_noeudarbre e): Renvoie l'élément précédent avant e.\r- précédent(): noeud xml précédent",
"coords(","- coords(nombre c): renvoie le tableau des coordonnées du widget",
"finligne(","- finligne(): Cesse d'ajouter des lignes",
"débutboucle(","- débutboucle(): Commence à dessiner des séquences de lignes qui se referment.",
"trou(","- trou(): remplit les trous lors du dessin d'un polygone complexe.",
"finpolygonecomplexe(","- finpolygonecomplexe(): Cesser de dessiner un polygone complexe",
"annoteregexip(","- annoteregexip(chaine reg,chaine style): Annote chaque chaine correspondant à reg avec style",
"arc(","- arc(nombre x,nombre y,nombre x1, nombre y1, réel a1, réel a2): Dessine un arc.\rarc(réel x,réel y,réel rad,réel a1,réel a2): Ajoute une série de points en arc de cercle au chemin courant.\r- arc(nombre x,nombre y,nombre x1, nombre y1, réel a1, réel a2): Dessine un arc de cercle",
"courbe(","- courbe(réel x,réel y,réel x1,réel y1,réel x1,réel y2,réel x3,réel y3): Ajoute une série de points sur une courbe de Bezier au chemin courant. La fin de la courbe (et deux de ses points) est à la position x,y et x3,y3.",
"camembert(","- camembert(nombre x,nombre y,nombre x1, nombre y1, réel a1, réel a2): Dessine un camembert",
"cercle(","- cercle(nombre x,nombre y,nombre r): Dessine un cercle",
"tailletexte(","- tailletexte(chaine l): Renvoie un dictionnaire avec largeur et hauteur comme clef pour donner les largeurs et hauteurs de la chaine en pixels",
"chargejpeg(","- chargejpeg(chaine filenom): Charge une image JPEG",
"dessinetexte(","- dessinetexte(chaine l,nombre x,nombre y): Place un texte à la position x,y",
"redessine(","- redessine(): Redessine la fenêtre",
"couleurrvb(","- couleurrvb(chaine|nombre couleur)|(nombre r,nombre g,nombre b)|(tableau couleur): Renvoie soit un tableau de la décomposition en couleur ou renvoie la valeur de la couleur correspondant à la combinaison RVB fournie",
"police(","- police(chaine f,nombre sz): Définit le nom de la police et sa taille\r- police(nombre s): définit ou renvoie la police du texte\r- police(chaine s): définit ou renvoie la police du texte\r- police(chaine s)|(): définit ou renvoie la police du texte\r- police(nombre c): Définit ou renvoie la police du noeud.",
"sisouris(","- sisouris(nombre action, fonction,objet): Définit le nom de la fonction de rappel pour une action souris\r- sisouris(nombre action,fonction f,objet o): fonction de rappel après un clic",
"couleursélection(","- couleursélection(nombre couleur): Couleur de l'élément sélectionné\r- couleursélection(nombre couleur): Couleur pour les éléments sélectionnés",
"sitouche(","- sitouche(nombre action, fonction,objet): Définit le nom de la fonction de rappel pour une action clavier\r- sitouche(nombre action,fonction f,objet o): fonction de rappel quand une touche est pressée",
"cache(","- cache(bool h): Cache la fenêtre si hauteur est vrai\r- cache(): Cache le widget",
"montre(","- montre(): Montre la fenêtre\r- montre(): Montre le widget",
"focus(","- focus(): Récupère le focus",
"créerépertoire(","- créerépertoire(chaine chemin): Crée le répertoire 'chemin'",
"taille(","- taille()|(nombre x,nombre y,nombre  largeur, nombre h): Renvoie la taille de la fenêtre ou définit la taille de la fenêtre",
"empileclip(","- empileclip(nombre x,nombre y,nombre  largeur, nombre h): Définit une région clippée de coordonnées x,y,largeur,h",
"dépileclip(","- dépileclip(): Relache une région clippée",
"réveil(","- réveil(): Réveille une fenêtre définie dans une tâche",
"verrouille(","- verrouille(): Verrou FLTK",
"déverrouille(","- déverrouille(): Deverouillage FLTK",
"taillelabel(","- taillelabel(nombre c): définit ou renvoie la taille de la police du label",
"policelabel(","- policelabel(nombre c): définit ou renvoie la police du label",
"couleurlabel(","- couleurlabel(nombre c): définit ou renvoie la couleur du label",
"typelabel(","- typelabel(nombre c): définit ou renvoie le type du label",
"label(","- label(chaine s): définit ou renvoie le texte du label\r- label(): Renvoie le label du noeud.",
"créée(","- créée(): Renvoie vrai si l'objet a été crée",
"copie(","- copie(chaine): Copie la chaine dans le presse-papier\r- copie(chaine s): Copie le texte sélectionné depuis l'éditeur dans le presse-papier (chaine s est optionnel)",
"aligne(","- aligne(nombre): définit l'alignement du label\r- aligne(nombre): définit l'alignement de la f_glissière\r- aligne(nombre): définit l'alignement du label du bouton",
"type(","- type(nombre x): Type de la f_glissière\r- type(chaine|nombre boitetype): Définit le type de la f_boite",
"typeboite(","- typeboite(nombre x): Définit le type de boite autour de la f_glissière\r- typeboite(nombre boitetype): type boite",
"pas(","- pas(nombre): définit le pas de la f_glissière",
"quand(","- quand(chaine when1, chaine when2,...): Type de l'événement pour un bouton qui déclenche la fonction de rappel\r- quand(chaine when1, chaine when2,...): Type de l'événement déclenchant la fonction de rappel",
"raccourci(","- raccourci(chaine clef code): Définit un raccourci-clavier correspondant à l'activation du bouton",
"couleur(","- couleur(chaine code|nombre code): Définit la couleur du bouton\r- couleur(chaine c|nombre c): définit ou renvoie la couleur du texte",
"contenu(","- contenu(): Retourne le contenu textuel du noeud xml",
"courant(","- courant(): Renvoie l'onglet courant actif",
"retire(","- retire(f_groupe g): Retire un onglet\r- retire(f_noeudarbre e): Retire l'élément e dans l'arbre.",
"chargegif(","- chargegif(chaine filenom): Charge une image GIF",
"cellule(","- cellule(nombre R,nombre C): Renvoie la valeur de la cellule ligne R,colonne C",
"hauteurligne(","- hauteurligne(nombre hauteur): Définit la hauteur de la ligne en pixel",
"taillepolice(","- taillepolice(nombre c): définit ou renvoie la taille de la police du texte\r- taillepolice(nombre c)|(): définit ou renvoie la taille de la police du texte\r- taillepolice(nombre c): Définit ou renvoie la taille de la police du noeud.",
"couleuravantplan(","- couleuravantplan(nombre c): définit ou renvoie la couleur de la cellule en avant-plan\r- couleuravantplan(nombre c): Définit ou renvoie la couleur de l'avant-plan du noeud.",
"entêteligne(","- entêteligne(nombre pos,chaine label): Définit le label de l'entête de la ligne à la ligne pos",
"largeurentêtecolonne(","- largeurentêtecolonne(nombre largeur): la taille en pixel de l'entête de la colonne",
"sélection(","- sélection(): renvoie le texte sélectionné dans la f_entrée\r- sélection(): renvoie le texte sélectionné dans l'éditeur ou les coordonnées de la sélection selon la variable de réception",
"mot(","- mot(nombre pos): Renvoie le mot à la position pos",
"metstyle(","- metstyle(nombre start,nombre fin,chaine style): définit le style d'une portion de texte",
"sauvegarde(","- sauvegarde(chaine f): Sauvegarde le contenu de l'éditeur dans le fichier f",
"surbrille(","- surbrille(nombre start,nombre fin)|(): Met une portion de texte en  sur-brillance. Renvoie vrai si le texte est sur-ligné ou le texte en  sur-brillance",
"ajoutestyle(","- ajoutestyle(dictionaire style): Initialise les styles",
"détruit(","- détruit(): Détruit le texte sélectionné depuis l'éditeur\r- détruit(): Détruit la representation interne du noeud XML",
"coupe(","- coupe(): Coupe le texte sélectionné depuis l'éditeur dans le presse-papier",
"bornesligne(","- bornesligne(nombre l): renvoie le début et la fin de la position de la ligne courante du chariot ou renvoie les limites de la ligne  correspondant à une position",
"borneslignecarac(","- borneslignecarac(): renvoie le début et la fin de la position de la ligne courante du chariot ou renvoie les limites de la ligne  correspondant à une position en nombre de caractères",
"curseur(","- curseur(nombre l): renvoie la position du curseur courant du chariot ou définit la position du curseur",
"curseurcarac(","- curseurcarac(): renvoie la position du curseur courant du chariot en nombre de caractères",
"annote(","- annote(chaine s|tableau v,chaine style,bool casse): Annote chaque occurence de s avec style",
"sautautomatique(","- sautautomatique(bool): Définit un mode coupure à la ligne automatique",
"accumule(","- accumule(chaine s): Ajoute s à la fin de l'entrée",
"positionoctet(","- positionoctet(nombre i): Convertit une position en octet en une position en caractère",
"positioncarac(","- positioncarac(nombre i): Convertit une position en caractère en une position en octet",
"délie(","- délie(): Retire un noeud de la structure arborée",
"ok(","- ok(): renvoie vrai si ok a été sélectionné",
"désélectionne(","- désélectionne(): Désélectionne tous les noeuds\rdésélectionne(nombre i): Désélectionne le noeud i",
"margehaut(","- margehaut(nombre s): Définit ou récupère l'espace (en pixels) qui doit apparaitre entre la bordure du haut du widget et le contenu de l'arbre.",
"séparateurcolonne(","- séparateurcolonne(): Renvoie le caractère de séparation de colonnes.\rséparateurcolonne(chaine): Définit le caractère de séparation de colonnes",
"formatcarac(","- formatcarac(): Renvoie le caractère de formatage.\rformatcarac(chaine): Définit le caractère de formatage",
"estfermé(","- estfermé(): Renvoie vrai si l'élément est fermé.\r- estfermé(chaine path): Renvoie vrai si l'élément est fermé.\restfermé(f_noeudarbre e): Renvoie vrai si l'élément est fermé.",
"estracine(","- estracine(): Renvoie vrai si l'élément est une racine.",
"estsélectionné(","- estsélectionné(): Renvoie vrai si l'élément est sélectionné.",
"estactif(","- estactif(): Renvoie vrai si l'élément est actif.",
"insèreavant(","- insèreavant(f_noeudarbre e,chaine label): Insère un élément au-dessus de e avec label.",
"couleurconnecteur(","- couleurconnecteur(nombre c): Définit ou renvoie la couleur du connecteur.",
"styleconnecteur(","- styleconnecteur(nombre s): Définit ou renvoie le style du connecteur.",
"ordretri(","- ordretri(nombre s): Définit ou renvoie l'ordre du tri.",
"couleurnoeudarrièreplan(","- couleurnoeudarrièreplan(nombre c): Définit ou renvoie la couleur d'arrière-plan du noeud.",
"couleurnoeudavantplan(","- couleurnoeudavantplan(nombre c): Définit ou renvoie la couleur d'avant-plan du noeud.",
"taillenoeud(","- taillenoeud(nombre c): Définit ou renvoie la taille de la police du noeud.",
"cliqué(","- cliqué(): Renvoie l'élément qui a été cliqué.",
"premier(","- premier(): Renvoie le premier élément.",
"commande(","- commande(chaine com): exécute la commande 'com'",
"tableau v=listerépertoire(","- tableau v=listerépertoire(chaine chemin): Retourne le contenu du répertoire",
"chaine cheminabsolu(","- chaine cheminabsolu(chemin vchemin): Retourne le chemin absolu",
"env(","- env(chaine var)|(chaine var,chaine val): Retourne ou initialise le contenu de la variable d'environnement 'var'.",
"proxy(","- proxy(chaine prox): Connexion via un proxy",
"url(","- url(chaine chemin): Charge une URL\rurl(chaine chemin,chaine fichier): Charge le contenu du site pointé par l'URL dans un fichier",
"transmet(","- transmet(): Fin du mode différé. L'ensemble des commandes est alors transmis à la base de données pour exécution.",
"nouveau(","- nouveau(chaine name): Crée un nouveau noeud XML",
"mémoire(","- mémoire(): renvoie la consommation mémoire de SQLite.",
"xpath(","- xpath(chaine myxpath): Renvoie un tableau de noeuds xml correspondant au chemin xpath myxpath",
"chainexml(","- chainexml(chaine balise,chaine texte): Crée un fichier XML avec balise et texte\r- chainexml(): Retourne l'arbre complet depuis le noeud courant sous la forme d'une chaine de caractères.",
"document(","- document(): renvoie le xmldoc associé à ce noeud",
"id(","- id(): Renvoie l'identifieur du noeud (uniquement disponible avec les fonctions de rappel)",
"propriétés(","- propriétés(): Retourne les propriétés du noeud xml","",""};
