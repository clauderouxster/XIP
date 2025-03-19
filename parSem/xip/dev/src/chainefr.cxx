/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the XIP library, 
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : XIP
Version    : 9.60
filename   : chainefr.cxx
Date       : 10/01/1999
Purpose    : French message strings
Programmer : Claude ROUX
Reviewer   : 
*/

#include <stdio.h>

extern char VERSION_XIP_BUILD[];
#ifdef UNIX
#ifndef sprintf_s
#define sprintf_s snprintf
#endif
#endif


char RecreeChainesFrancais(char* nom) {
    FILE* fichier=fopen(nom,"w\n");
    if (fichier==NULL)
        return -1;
    
    char version[100];
    sprintf_s(version,100,"%s\n",VERSION_XIP_BUILD);
    
    fprintf(fichier,"sémantique\n"); 
    fprintf(fichier,"termes\n"); 
    fprintf(fichier,"reconstructeur\n");  
    fprintf(fichier,"Echec sur la remontée des traits.\n");  
    fprintf(fichier,version); 
    fprintf(fichier,"Changement par règle de deduction\n"); 
    fprintf(fichier,"Destruction par règle de deduction \n"); 
    fprintf(fichier," donne: \n");  
    fprintf(fichier," avec \n");   
    fprintf(fichier,"Partie droite de la règle ne correspond pas\n");  
    fprintf(fichier,"Un test a déjà été introduit par un 'if'\n"); 
    fprintf(fichier,"Attention, il faut une variable pour tester les traits\n");
    fprintf(fichier,"Un point doit être présent dans le nom de fichier\n");
    fprintf(fichier,"Réponse inconnue\n");
    fprintf(fichier,"désambiguïsation\n");
    fprintf(fichier,"Champ Nombre manquant ou mal écrit\n");
    fprintf(fichier,"Champ Doublons manquant ou mal écrit\n");
    fprintf(fichier,"négation\n");
    fprintf(fichier,"toutmajuscule\n");
    fprintf(fichier,"règle\n");
    fprintf(fichier,"règles\n");
    fprintf(fichier,"dirègles\n");
    fprintf(fichier,"têtes\n");
    fprintf(fichier,"marqueur\n");
    fprintf(fichier,"préstd\n");
    fprintf(fichier,"prérct\n");
    fprintf(fichier,"abréviations\n");
    fprintf(fichier,"déduction\n");
    fprintf(fichier,"Erreur, ce champ est inconnu, on attend la valeur: marqueur\n");
    fprintf(fichier,"On attend soit '&' soit ')' soit ';' dans cette règle\n");
    fprintf(fichier,"On attend '(' dans cette règle\n");
    fprintf(fichier,"abréviations\n");
    fprintf(fichier,"La variable dans cette fonction ne correspond pas à une variable des tests associés\n");
    fprintf(fichier,"Les regles Marqueurs doivent commencer par 'chiffre>' ex: 1> \n");
    fprintf(fichier,"On attend '=' dans cette règle\n");
    fprintf(fichier,"On attend '{' dans cette règle de défaut\n");
    fprintf(fichier,"Erreur dans la règle, ce caractère n'est pas valide:\n");
    fprintf(fichier,"On attend '&' dans cette règle de défaut\n");
    fprintf(fichier,"On attend '=' dans cette règle\n");
    fprintf(fichier,"On attend ',' ou '}' dans cette règle de défaut\n");
    fprintf(fichier,"défauts\n"); 
    fprintf(fichier,"\n");
    fprintf(fichier,"\n");
    fprintf(fichier,"Le symbole d'affectation '=' est attendu dans ce cas\n");
    fprintf(fichier,"définitions\n");
    fprintf(fichier,"Pas plus d'une valeur dans la définition\n");
    fprintf(fichier,"Il manque une ',' ou un '=' dans cette règle\n");
    fprintf(fichier,"zones\n");
    fprintf(fichier,"filtrecouche\n");
    fprintf(fichier,"découpeséquence\n");
    fprintf(fichier,"Une seule catégorie dans la partie droite de la règle de simplification\n"); 
    fprintf(fichier,"catégories\n");
    fprintf(fichier," n'a pas de définition de traits\n");
    fprintf(fichier,"plrègles\n");
    fprintf(fichier,"La flèche est manquante ou mal écrite\n");
    fprintf(fichier,"Il manque une paranthèse\n");
    fprintf(fichier,"Erreur lors de la lecture d'une regle sémantique\n");
    fprintf(fichier,"La catégorie finale ne peut être un '?'\n");
    fprintf(fichier,"Cette couche est filtrée et ne s'applique pas:\n");
    fprintf(fichier,"La partie droite d'une affectation doit se trouver entre {}\n");
    fprintf(fichier,"Il manque une parenthèse fermante\n"); 
    fprintf(fichier,"Il n'y a pas d'abréviation\n");
    fprintf(fichier,"Pas de parenthèse ici...\n");
    fprintf(fichier,"Seule une valeur de trait peut apparaître après un '='\n");
    fprintf(fichier,"Attention: Affectation multiple de catégories pour une entrée -> Catégorie conservé: \n");
    fprintf(fichier,"défaut\n");
    fprintf(fichier,"Il n'existe aucune règle par défaut pour ce trait\n");
    fprintf(fichier,"Erreur, il faut le coupe-choix avec l'analyse longue\n");
    fprintf(fichier,"Impossible, ce trait a déjà été déclaré\n"); 
    fprintf(fichier,"La partie droite contient des erreurs...\n");
    fprintf(fichier," n'a pas de définition de traits\n"); 
    fprintf(fichier,"Cette couche est déjà associé avec un filtre.\n");
    fprintf(fichier,"Règle:\n");
    fprintf(fichier,"Seule une balise début peut recevoir ce type de traits\n");
    fprintf(fichier,"Trop de doublons dans cette règle,augmenter la valeur de DOUBLONS\n");
    fprintf(fichier,"Pas de règle\n");
    fprintf(fichier,"Ce filtre doit être associé à un numéro de couche.\n");
    fprintf(fichier,"Operateur inconnu: \n");
    fprintf(fichier,"Cette regle commence avec une structure trop complexe...\n");
    fprintf(fichier,"Attention, dans le cas d'un changement, il n'est possible de produire qu'une seule solution.\n");
    fprintf(fichier,":On choisit la règle \n"); 
    fprintf(fichier,"On crée \n");
    fprintf(fichier,":Échec sur les traits\n");
    fprintf(fichier,":On arrête les recherches\n");
    fprintf(fichier,"Erreur:Montée des traits:\n");
    fprintf(fichier,"Erreur:Règle de défaut inexistante. Contexte:\n");
    fprintf(fichier,"Erreur négation: \n");
    fprintf(fichier,"Erreur, la dépendance doit être enclose entre '<...>'\n"); 
    fprintf(fichier,"Trop de noms différents (attribut ou valeur) : \n");
    fprintf(fichier,"Erreur de la lecture de la zone\n");
    fprintf(fichier,"affichagefonction\n"); 
    fprintf(fichier,"Attention, une seule fonction peut être marquée à la fois\n");
    fprintf(fichier,"Seul un ';' est acceptable ici\n");
    fprintf(fichier,"Attention, il manque une '*' ou un '+' après les parenthèses\n");
    fprintf(fichier,"Nom de code incorrecte ou absent:\n"); 
    fprintf(fichier,"oui\n");
    fprintf(fichier,"non\n");
    fprintf(fichier,"Fichier inconnu: \n");
    fprintf(fichier,"Pas de champ 'vocabulaires' a cet endroit\n");
    fprintf(fichier,"finphrase\n");
    fprintf(fichier,"trace\n"); 
    fprintf(fichier,"tracenoeud\n");
    fprintf(fichier,"nombre\n");
    fprintf(fichier,"doublons\n");
    fprintf(fichier,"La variable #0 correspond au mot que l'on simplifie, on ne peut l'utiliser comme variable dans un contexte.\n");
    fprintf(fichier,"Mot clef 'where' attendu\n");
    fprintf(fichier,"sortie\n");
    fprintf(fichier,"La valeur maximale de couche est comprise entre [1-99]\n");
    fprintf(fichier,"traits\n");
    fprintf(fichier,"lexique\n");
    fprintf(fichier,"vocabulaires\n");
    fprintf(fichier,"Il manque une '['\n");
    fprintf(fichier,"Il manque une ',' ou un ']'\n");
    fprintf(fichier,"Ce trait n'existe pas ---> \n");
    fprintf(fichier,"affichage\n"); 
    fprintf(fichier,"Il manque le filtre d'affichage\n");
    fprintf(fichier,"Il manque un ']'\n");
    fprintf(fichier,"Il manque un '<'\n");
    fprintf(fichier," est une valeur inconnue pour \n");
    fprintf(fichier," n'est pas une valeur possible pour \n");
    fprintf(fichier,"poststd\n");
    fprintf(fichier,"postrct\n");
    fprintf(fichier,"Si une categorie n'est pas precisee, il faut utiliser l'operateur '+='\n");
    fprintf(fichier,"Il manque un '='\n");
    fprintf(fichier," est inconnu comme attribut\n");
    fprintf(fichier,"Erreur dans une affectation: traits sont presents dans la variable gauche ou traits manquant dans la variable droite\n");
    fprintf(fichier,"Les variables dans un test doivent être associees à des traits\n");
    fprintf(fichier,"L'une des variables dans la partie droite ne correspond pas a une variable dans la partie gauche...\n");
    fprintf(fichier,"lemme\n");
    fprintf(fichier,"Il faut une variable ici\n");
    fprintf(fichier,"Les definitions locales de traits s'inserent avec '<traits>'\n");
    fprintf(fichier,"Il manque une ',' ou une '}'\n");
    fprintf(fichier,"Il manque une ',' ou un '.' ou un ';'\n");
    fprintf(fichier,"Il manque une ',' ou une ')'\n");
    fprintf(fichier,"Il manque une ',' ou un ']'\n");
    fprintf(fichier,"Il manque un ':'\n");
    fprintf(fichier,"Une seule valeur possible pour le champ libre\n");
    fprintf(fichier,"Une valeur doit être spécifié pour chacun des attributs au sein d'une PL-règle.\n");
    fprintf(fichier,"Ce mot n'est pas dans la liste de vocabulaire --> \n");
    fprintf(fichier,"Une regle ne peut avoir de partie droite vide.\n");
    fprintf(fichier,"erreurs.err\n");
    fprintf(fichier,"Nombre de Catégorie trop grand\n");
    fprintf(fichier,"Arbre\n");
    fprintf(fichier,"[c]\n");
    fprintf(fichier,"Erreur lors de l'application de la RCT:\n");
    fprintf(fichier,"sur \n");
    fprintf(fichier,"DI-regle, couche: \n");
    fprintf(fichier,":Base:\n");
    fprintf(fichier,":On isole:\n");
    fprintf(fichier,":On recherche encore:\n");
    fprintf(fichier,"On met en attente \n");
    fprintf(fichier,"erreur pendant l'analyse, augmenter la valeur de DOUBLONS\n");
    fprintf(fichier,"Erreur dans le fichier:\n");
    fprintf(fichier," à la ligne:\n");
    fprintf(fichier,"Plus de mémoire.\n");
    fprintf(fichier,"Echec dû à la PL-règle:\n");
    fprintf(fichier,"Temps: \n");
    fprintf(fichier,"Caractère de début manquant ou erroné\n");
    fprintf(fichier,"Aucune valeur trouvée pour cet attribut\n");
    fprintf(fichier,"indentées\n");
    fprintf(fichier,"Le(s) trait(s) suivant(s) doit(vent) être présent sur ce noeud: \n"); 
    fprintf(fichier,"On veut associer le noeud: \n");
    fprintf(fichier,"Correspondance refusée entre ce noeud et cet élément de la règle\n");
    fprintf(fichier,"Elément de la règle: \n");
    fprintf(fichier,"Abréviation déjà utilisée : \n");
    fprintf(fichier,"Abréviation inconnue : \n");
    fprintf(fichier,"Erreur dans la lecture de l'itération maximale pour une categorie\n");
    fprintf(fichier,"surface\n");
    fprintf(fichier,"Aucun test sur des valeurs de traits n'est permis ici\n");
    fprintf(fichier,"La règle impose que ce noeud ait une valeur pour les traits suivants: \n");
    fprintf(fichier,"Traits non trouvés dans les abréviations:\n"); 
    fprintf(fichier,"locale\n");
    fprintf(fichier,"La forme exacte est: ..{ \n"); 
    fprintf(fichier,"Structure de traits non analysable : \n"); 
    fprintf(fichier,"Séquence. Couche: \n"); 
    fprintf(fichier,"affichagenoeud\n"); 
    fprintf(fichier," Impossible de virer la seule interpretation: \n"); 
    fprintf(fichier," on vire: \n"); 
    fprintf(fichier," Virer au sein d'un contexte\n"); 
    fprintf(fichier,"majuscule\n"); 
    fprintf(fichier,"Simplification. Couche: \n"); 
    fprintf(fichier,"tag\n"); 
    fprintf(fichier,"Avant Simplification.\n"); 
    fprintf(fichier,"TAG inconnu:\n");
    fprintf(fichier,"chiffre\n");
    fprintf(fichier,"Champ 'chiffre' absent ou mal écrit\n"); 
    fprintf(fichier,"Après Simplification.\n"); 
    fprintf(fichier,"On écrase l'entrée: \n");
    fprintf(fichier,"traduction\n");
    fprintf(fichier,"Il manque un '.' ou un '['\n"); 
    fprintf(fichier,"\n"); 
    fprintf(fichier,"\n"); 
    fprintf(fichier,"fonctions\n");
    fprintf(fichier,": catégorie inconnue\n");
    fprintf(fichier,"Tous les arguments de cette fonction n'ont pu être associés avec un element dans la partie droite\n");
    fprintf(fichier,"Extraction réussie.\n");
    fprintf(fichier,"cacher\n");
    fprintf(fichier,"Une catégorie optionnelle ne peut commencer une règle\n");
    fprintf(fichier,"Il manque la taille de la matrice d'affichage (X ou Y)\n");
    fprintf(fichier,"Cette couche doit être du type Simplification\n");
    fprintf(fichier,"Fonction inconnue: \n");
    fprintf(fichier,"extraction\n");
    fprintf(fichier,"séquence\n");
    fprintf(fichier,"Echec extraction.\n");
    fprintf(fichier,"Erreur... le contexte droit doit apparaitre avant le point final \n");
    fprintf(fichier,"Erreur... le contexte gauche ne s'applique pas\n");
    fprintf(fichier,"Erreur... le contexte droit ne s'applique pas\n");
    fprintf(fichier,"Erreur sur la valeur du champ libre :\n");
    fprintf(fichier,"Erreur, Cette couche est déjà d'un autre type= \n");
    fprintf(fichier,"Marquage: contexte droit insuffisant\n");
    fprintf(fichier,"Erreur, il manque une accolade fermante pour un test \n");
    fprintf(fichier,"Erreur, les règles d'affectation ne sont disponibles qu'au sein d'une couche\n");
    fprintf(fichier,"Erreur, Cette couche est déjà du type: règle DI Catégorie= \n");
    fprintf(fichier,"Erreur, il manque l'indice de couche pour cette pl-règle\n");  
    fprintf(fichier,"Erreur, Aucune DI-règle ne peut s'associer avec cette PL-règle dans cette couche\n");
    fprintf(fichier,"Erreur, plusieurs catégories portent le trait last dans cette règle\n");
    fprintf(fichier,"Erreur, plusieurs catégories portent le trait first dans cette règle\n");
    fprintf(fichier,"Erreur, ce champ est inconnu: \n");
    fprintf(fichier,"codagexml\n");
    fprintf(fichier,"Un seul attribut est permis ici...\n");
    fprintf(fichier,"On ne peut pas générer plus d'une dépendance à la fois dans ce contexte.\n");
    fprintf(fichier,"doubleréduction\n");
    fprintf(fichier,"Succès lors de l'application de la règle.\n");
    fprintf(fichier,"Echec lors de l'application du contexte ou lors d'une comparaison sur les traits.\n");
    fprintf(fichier,"Echec, champ absent..\n");
    fprintf(fichier,"Erreur... Il faut une variable $xxx avec le '^'.\n");
    fprintf(fichier,"Index non valide... Regénérez l'index...\n");
    fprintf(fichier,"Le caractère '\"' doit toujours être précédé du caractère d'échappement: \\\"\n");
    fprintf(fichier,"Cette règle détruit toutes les lectures de: \n");
    fprintf(fichier,". Elle ne peut s'appliquer.\n");
    fprintf(fichier,"> Echec de la règle: \n");
    fprintf(fichier," commençant au noeud: \n");
    fprintf(fichier,"> Règle de séquence: \n");
    fprintf(fichier,"Erreur: boucle infinie...\n");
    fprintf(fichier,"Règle d'extraction: \n");
    fprintf(fichier,"Règle de dépendance: \n");
    fprintf(fichier,"Succès: \n");
    fprintf(fichier," sur le noeud: \n");
    fprintf(fichier,"Règle de marquage: \n");
    fprintf(fichier,"Règle de reconstruction: \n");
    fprintf(fichier,"Application de la règle de reconstruction: \n");
    fprintf(fichier,"garder\n");
    fclose(fichier);
    return 0;
}

const char* CHAINESFR[] = {
        "sémantique", //0
        "termes", //1
        "reconstructeur",  //2
        "Echec sur la remontée des traits.",  //3
        VERSION_XIP_BUILD,//4
        "Changement par règle de deduction", //5
        "Destruction par règle de deduction ", //6
        " donne: ",  //7
        " avec ",   //8
        "Partie droite de la règle ne correspond pas",  //9
        "Un test a déjà été introduit par un 'if'", //10
        "Attention, il faut une variable pour tester les traits",//11
        "Un point doit être présent dans le nom de fichier",//12
        "Réponse inconnue",//13
        "désambiguïsation",//14
        "Champ Nombre manquant ou mal écrit",//15
        "Champ Doublons manquant ou mal écrit",//16
        "négation",//17
        "toutmajuscule",//18
        "règle",//19
        "règles",//20
        "dirègles",//21
        "têtes",//22
        "marqueur",//23
        "préstd",//24
        "prérct",//25
        "abréviations",//26
        "déduction",//27
        "Erreur, ce champ est inconnu, on attend la valeur: marqueur",//28
        "On attend soit '&' soit ')' soit ';' dans cette règle",//29
        "On attend '(' dans cette règle",//30
        "abréviations",//31
        "La variable dans cette fonction ne correspond pas à une variable des tests associés",//32
        "Les regles Marqueurs doivent commencer par 'chiffre>' ex: 1> ",//33
        "On attend '=' dans cette règle",//34
        "On attend '{' dans cette règle",//35
        "Erreur dans la règle, ce caractère n'est pas valide:",//36
        "On attend '&' dans cette règle de défaut",//37
        "",//38
        "On attend ',' ou '}' dans cette règle de défaut",//39
        "défauts", //40
        "taillemot",//41
        "",//42
        "Le symbole d'affectation '=' est attendu dans ce cas",//43
        "définitions",//44
        "Pas plus d'une valeur dans la définition",//45
        "Il manque une ',' ou un '=' dans cette règle",//46
        "zones",//47
        "filtrecouche",//48
        "découpeséquence",//49
        "Une seule catégorie dans la partie droite de la règle de simplification", //50
        "catégories",//51
        " n'a pas de définition de traits",//52
        "plrègles",//53
        "La flèche est manquante ou mal écrite",//54
        "Il manque une paranthèse",//55
        "Erreur lors de la lecture d'une regle sémantique",//56
        "La catégorie finale ne peut être un '?'",//57
        "Cette couche est filtrée et ne s'applique pas:",//58
        "La partie droite d'une affectation doit se trouver entre {}",//59
        "Il manque une parenthèse fermante", //60
        "Il n'y a pas d'abréviation",//61
        "Pas de parenthèse ici...",//62
        "Seule une valeur de trait peut apparaître après un '='",//63
        "Attention: Affectation multiple de catégories pour une entrée -> Catégorie conservé: ",//64
        "défaut",//65
        "Il n'existe aucune règle par défaut pour ce trait",//66
        "Erreur, il faut le coupe-choix avec l'analyse longue",//67
        "Impossible, une valeur a déjà été donnée pour ce trait : ", //68
        "La partie droite contient des erreurs...",//69
        " n'a pas de définition de traits", //70
        "Cette couche est déjà associé avec un filtre.",//71
        "Règle:",//72
        "Seule une balise début peut recevoir ce type de traits",//73
        "Trop de doublons dans cette règle,augmenter la valeur de DOUBLONS",//74
        "Pas de règle",//75
        "Ce filtre doit être associé à un numéro de couche.",//76
        "Operateur inconnu: ",//77
        "Cette regle commence avec une structure trop complexe...",//78
        "Attention, dans le cas d'un changement, il n'est possible de produire qu'une seule solution.",//79
        ":On choisit la règle ", //80
        "On crée ",//81
        ":Échec sur les traits",//82
        ":On arrête les recherches",//83
        "Erreur:Montée des traits:",//84
        "Erreur:Règle de défaut inexistante. Contexte:",//85
        "Erreur négation: ",//86
        "Erreur, la dépendance doit être enclose entre '<...>'", //87
        "Trop de noms différents (attribut ou valeur) : Veuillez augmenter la valeur du champ NOMBRE dans votre fichier langue.xip",//88
        "Erreur de la lecture de la zone",//89
        "affichagefonction", //90
        "Attention, une seule fonction peut être marquée à la fois",//91
        "Seul un ';' est acceptable ici",//92
        "Attention, il manque une '*' ou un '+' après les parenthèses",//93
        "Nom de code incorrecte ou absent:", //94
        "oui",//95
        "non",//96
        "Fichier inconnu: ",//97
        "Pas de champ 'vocabulaires' a cet endroit",//98
        "finphrase",//99
        "trace", //100
        "tracenoeud",//101
        "nombre",//102
        "doublons",//103
        "La variable #0 correspond au mot que l'on simplifie, on ne peut l'utiliser comme variable dans un contexte.",//104
        "Mot clef 'where' attendu",//105
        "sortie",//106
        "La valeur maximale de couche est comprise entre [1-1000]",//107
        "traits",//108
        "lexique",//109
        "vocabulaires",//110
        "Il manque une '['",//111
        "Il manque une ',' ou un ']'",//112
        "Ce trait n'existe pas ---> ",//113
        "affichage", //114
        "Il manque le filtre d'affichage",//115
        "Il manque un ']'",//116
        "Il manque un '<'",//117
        " est une valeur inconnue pour ",//118
        " n'est pas une valeur possible pour ",//119
        "poststd",//120
        "postrct",//121
        "Si une categorie n'est pas precisee, il faut utiliser l'operateur '+='",//122
        "Il manque un '='",//123
        " est inconnu comme attribut",//124
        "Erreur dans une affectation: traits sont presents dans la variable gauche ou traits manquant dans la variable droite",//125
        "Les variables dans un test doivent être associees à des traits",//126
        "L'une des variables dans la partie droite ne correspond pas a une variable dans la partie gauche...",//127
        "lemme",//128
        "Seules des variables de noeuds sont acceptées ici (#1, #2..)",//129
        "Les definitions locales de traits s'inserent avec '<traits>'",//130
        "Il manque une ',' ou une '}'",//131
        "Il manque une ',' ou un '.' ou un ';'",//132
        "Il manque une ',' ou une ')'",//133
        "Il manque une ',' ou un ']'",//134
        "Il manque un ':'",//135
        "Une seule valeur possible pour le champ libre",//136
        "Une valeur doit être spécifié pour chacun des attributs au sein d'une PL-règle.",//137
        "Ce mot n'est pas dans la liste de vocabulaire --> ",//138
        "Une regle ne peut avoir de partie droite vide.",//139
        "erreurs.err",//140
        "Nombre de Catégorie trop grand",//141
        "Arbre",//142
        "[c]",//143
        "Erreur lors de l'application de la RCT:",//144
        "sur ",//145
        "DI-regle, couche: ",//146
        ":Base:",//147
        ":On isole:",//148
        ":On recherche encore:",//149
        "On met en attente ",//150
        "erreur pendant l'analyse, augmenter la valeur de DOUBLONS",//151
        "Erreur dans le fichier:",//152
        " à la ligne:",//153
        "Plus de mémoire.",//154
        "Echec dû à la PL-règle:",//155
        "Temps: ",//156
        "Caractère de début manquant ou erroné",//157
        "Aucune valeur trouvée pour cet attribut",//158
        "indentées",//159
        "Le(s) trait(s) suivant(s) doit(vent) être présent sur ce noeud: ", //160
        "On veut associer le noeud: ",//161
        "Correspondance refusée entre ce noeud et cet élément de la règle",//162
        "Elément de la règle: ",//163
        "Abréviation déjà utilisée : ",//164
        "Abréviation inconnue : ",//165
        "Erreur dans la lecture de l'itération maximale pour une categorie",//166
        "surface",//167
        "Aucun test sur des valeurs de traits n'est permis ici",//168
        "La règle impose que ce noeud ait une valeur pour les traits suivants: ",//169
        "Traits non trouvés dans les abréviations:", //170
        "locale",//171
        "La forme exacte est: ..{ ", //172
        "Structure de traits non analysable : ", //173
        "Séquence. Couche: ", //174
        "affichagenoeud", //175
        " Impossible de virer la seule interpretation: ", //176
        " on vire: ", //177
        " Virer au sein d'un contexte", //178
        "majuscule", //179
        "Simplification. Couche: ", //180
        "tag", //181
        "Avant Simplification.", //182
        "TAG inconnu:",//183
        "chiffre",//184
        "Champ 'chiffre' absent ou mal écrit", //185
        "Après Simplification.", //186
        "On écrase l'entrée: ",//187
        "traduction",//188
        "Il manque un '.' ou un '['", //189
        "Nombre maximum de règles atteint dans un fichier paramètre.", //190
        "Le nom du chemin 'XPath' doit être suivi d'un '('", //191
        "fonctions",//192
        ": catégorie inconnue",//193
        "Tous les arguments de cette fonction n'ont pu être associés avec un element dans la partie droite",//194
        "Extraction réussie.",//195
        "cacher",//196
        "Une catégorie optionnelle ne peut commencer une règle",//197
        "Il manque la taille de la matrice d'affichage (X ou Y)",//198
        "Cette couche doit être du type Simplification",//199
        "Fonction inconnue: ",//200
        "extraction",//201
        "séquence",//202
        "Echec extraction.",//203
        "Erreur... le contexte droit doit apparaitre avant le point final ",//204
        "Erreur... le contexte gauche ne s'applique pas",//205
        "Erreur... le contexte droit ne s'applique pas",//206
        "Erreur sur la valeur du champ libre :",//207
        "Erreur, Cette couche est déjà d'un autre type= ",//208
        "Marquage: contexte droit insuffisant",//209
        "Erreur, il manque une accolade fermante pour un test ",//210
        "Erreur, les règles d'affectation ne sont disponibles qu'au sein d'une couche",//211
        "Erreur, Cette couche est déjà du type: règle DI Catégorie= ",//212
        "Erreur, il manque l'indice de couche pour cette pl-règle",  //213
        "Erreur, Aucune DI-règle ne peut s'associer avec cette PL-règle dans cette couche",//214
        "Erreur, plusieurs catégories portent le trait last dans cette règle",//215
        "Erreur, plusieurs catégories portent le trait first dans cette règle",//216
        "Erreur, ce champ est inconnu: ",//217
        "codagexml",//218
        "Un seul attribut est permis ici...",//219
        "On ne peut pas générer plus d'une dépendance à la fois dans ce contexte.",//220
        "doubleréduction",//221
        "Succès lors de l'application de la règle.",//222
        "Echec lors de l'application du contexte ou lors d'une comparaison sur les traits.",//223
        "Echec, champ absent...",//224
        "Erreur... Il faut une variable $xxx avec le '^'.",//225
        "Index non valide... Regénérez l'index...",//226
        "Le caractère '\"' doit toujours être précédé du caractère d'échappement: \\\"",//227
        "Cette règle détruit toutes les lectures de: ",//228
        ". Elle ne peut s'appliquer.",//229
        "> Echec de la règle de séquence: ",//230
        " commençant au noeud: ",//231
        "> Règle de séquence: ",//232
        "Erreur: boucle infinie...",//233
        "Règle d'extraction: ",//234
        "Règle de dépendance: ",//235
        "Succès: ",//236
        " sur le noeud: ",//237
        "Règle de marquage: ",//238
        "Règle de reconstruction: ",//239
        "Application de la règle de reconstruction: ",//240
        "garder",//241
        "Ce noeud de règle de reconstruction peut occasionner la perte de mots dans l'arbre syntaxique",//242
        "Avertissement, cette règle a consommé trop de temps d'analyse", //243
        "Erreur: cet attribut n'a jamais été déclaré.",//244
        "Erreur: Seule une structure de noeud (#num) peut être utilisée ici.",//245
        "Champ XML manquant.",//246
        "Erreur dans la formule XML.",//247
        "Méthode inconnue.",//248
        "Une variable pour un noeud maison ne peut pas être utilisé ailleurs dans la formule",//249
        "La DTD contient des erreurs",//250
        "Ce champ de DTD ne correspond pas à une action pour XIP",//251
        "Cette description de traits devrait comprendre deux éléments parmi ses valeurs possibles",//252
        "Cette procédure a déjà été déclarée",//253
        "Une procédure ne peut être le résultat d'une règle de déduction",//254
        "Cette procédure n'a pas de corps d'analyse",//255
        "'_goto' doit être unique",//256
        "Ce label a déjà été utilisé",//257
        "Espace d'analyse complet (vérifiez vos boucles d'analyse)",//258
        "Fichier Paramètre inconnu",//259
        "Trace activée",//260
        "Chargement de la grammaire en cours",//261
        "Initialisation du Parseur",//262
        "Les fichiers paramètres ne peuvent être chargés en mode: limité...",//263
        "Les expressions XIP doivent être encloses entre deux '!'",//264
        "Action inconnue: ",//265
        "Action par défaut inconnue",//266
        "Fonction inconnue (type possible: analyze,merge,feature ou skip)",//267
        "Mauvais nombre d'argument pour une transformation en traits.",//268
        "Paramètre manquant.\nActions possibles:\n\tskip=text|all\n\tanalyze=token|chunk|dependency\n\tfeature=string,string,(token|chunk|dependency).",//269
        "Paramètre inconnu pour _display",//270
        "Le '}' est absent dans une boucle 'WHILE'",//271
        "Il manque un 'WHILE' pour ce '}'",//272
        "Cette étiquette est inconnue",//273
        "On ne peut affecter une valeur précédée d'une négation",//274
        "Erreur dans la définition de votre lexème: Parenthèse manquante.",//275
        "Erreur dans la définition de votre lexème: Crochet manquant.",//276
        "Erreur dans la définition de votre lexème: La négation ne peut précéder une parenthèse.",//277
        "maxmots",//278
        "Le nombre maximum de mots a été atteint",//279
        "Caractère ',' ou ';' or '}' ou '|' manquant",//280
        "Il faut définir une fenêtre de catégories pour utiliser des '*' ou des '+': Cat#max",//281
        "La négation ne peut être appliquée qu'à une catégorie.",//282
        "L'opérateur '?-' doit nécessairement être en fin de règle.",//283
        "La virgule est mal placée dans cette expression régulière",//284
        "On ne peut utiliser une '*' ou un '+' a cet endroit",//285
        "On attend une parenthèse fermante ici: ')'", //286
        "On attend un crochet fermant ici",//287
        "fragmenter",//288
        "Les variables d'expressions régulières doivent apparaître par ordre croissant 1,2...9",//289
        "On peut seulement avoir un signe d'affectation ('=') devant une variable XIP dans une création de noeud XML",//290
        "Après un '|' on ne peut avoir que: 'if', 'iftest' ou '{'",//291
        "Erreur dans la lecture de la règle de déduction: ",//292
        "Concept ontologique inconnu",//293
        "Ce chemin ontologique est incohérent",//294
        "affichageontologie",//295
        "Erreur: il manque l'alias au sein de l'expression XML",//296
        "Erreur: une string ou un vector doit toujours commencer avec un '_'",//297
        "Erreur: Il faut fournir un indice à cette variable de type vector.",//298
        "Erreur: Ce '}' n'est pas associé avec un 'for'..",//299
        "Erreur: Il manque une '}'...",//300
        "Erreur: 'iftest' mal écrit..",//301
        "Erreur: Accolade '}' manquante...",//302
        "Erreur: type inconnu...",//303
        "Accolade fermante sans accolade ouvrante...",//304
        "Une expression XMLGuide doit se terminer avec un '.' ou un ';'",//305
        "On doit toujours finir une procédure avec un 'return'",//306
        "Procédure ou instruction inconnue:",//307
        "Type de paramètre inconnu:",//308
        "Cette variable a déjà été déclarée:",//309
        "Les règles lexicales doivent être précédées d'un numéro de couche ici",//310
        "On ne peut utiliser de variables de noeuds entre crochets de structure de traits [..]",//311
        "Type de vecteur impossible",//312
        "Il manque un '.' dans cette variable struct",//313
        "script",//314
        "Le 'if' ne peut être utilisé au sein des règles de script",//315
        "Le 'iftest' ne peut être utilisé au sein des règles de déduction",//316
        "Les instructions de boucle sont interdites au sein des règles de déduction",//317
        "Une dépendance peut seulement être créee au sein des règles de déduction",//318
        "Une suite d'instruction peut seulement être utilisée au sein d'un script",//319
        "final",//320
        "On attend une ',' ou un ';' ici",//321
        "Attention. Cette dépendance est déjà déclarée:",//322
        "Champ inconnu dans un fichier grammaire:",//323
        "Erreur... Cette variable est déjà soit une catégorie soit une dépendance soit une instruction: ",//324
        "Erreur... Partie manquante dans la formule",//325
        "Erreur... Cette version de XIP ne peut lire des expressions XML",//326
        "Erreur... Opérateur manquant",//327
        "enchargeant",//328
        "initial",//329
        "Parenthèse manquante dans un #ifdef",//330
        "#endif ou #else manquant",//331
        "#endif manquant pour ce #else",//332
        "#else sans #ifdef",//333
        "multimots",//334
        "Cette expression a déjà été utilisée",//335
        "Erreur: Cette chaîne est trop longue",//336
        "Erreur: Cette règle est mal écrite et inapplicable",//337
        "janvier",//338
        "février",//339
        "mars",//340
        "avril",//341
        "mai",//342
        "juin",//343
        "juillet",//344
        "août",//345
        "septembre",//346
        "octobre",//347
        "novembre",//348
        "décembre",//349
        "lundi",//350
        "mardi",//351
        "mercredi",//352
        "jeudi",//353
        "vendredi",//354
        "samedi",//355
        "dimanche",//356
        "encodageutf8lexique",//357
        "Interface manquante pour un appel Python",//358
        "Il n'y a pas de nom de fonction Python correspondant à l'interface",//359
        "Fonction Python inconnue",//360
        "Erreur, on attend un noeud fictif ici: ##CAT[traits]",//361
        "Erreur, cette variable ne peut être une racine. Elle ne pointe pas sur un seul noeud.",//362
        "ATTENTION. Ce module Python a déjà été chargé",//363
		"Erreur: Le code est supérieur à 255",//364
		"définitioncatégorie",//365
		"découpecatégorie",//366
		"Cette valeur a déjà été déclarée pour cet attribut",//367
		"Encodage inconnu (valeurs acceptées: 'utf8' ou 'latin')",//368
		"Numéro de couche manquant pour ce type de règle.",//369
		"Partie droite de cette règle de reconstruction manquante ou erronée",//370
		"Catégorie manquante devant une '{'",//371
		"La partie droite d'une règle de séquence ne peut contenir de sous-noeuds",//372
		"Expression ontologique inconnue",//373
		"Erreur... L'opérateur '-' ne peut être utilisé qu'en fin de séquence",//374
		"multilemmes",//375
		"Racine de l'espace de nom inconnu. Vérifiez que la première partie de votre nom d'espace soit la même que toutes vos autres définitions",//376
		"Instruction de format inconnu",//377
		"Fermeture de commentaire: '\\' manquante",//378
		"Le champ guesserfeature n'a pas été défini dans votre fichier de langue. Le test sur langue est impossible",//379
		"définitiontraits",//380
		"expressionfinphrase",//381
		"\" de fermeture manquant",//382
		"Il manque un '.' à la fin de cette règle multimots",//383
		"génération",//384
		"génèretraits",//385
		"Règle génèretraits incorrecte",//386
		"Flux manquant",//387
		"Lemme manquant",//388
		"Traits manquants",//389
		"'.' manquant à la fin de cette dépendance",//390
		"noeuds",//391
		"Seules des fonctions déclarées comme NOEUDS sont acceptées comme résultat ici",//392
		"Pas de formule autorisée dans ce contexte",//393
		"Déclaration de dépendances erronée",//394
		"ordre",//395
		"Seuls des NOEUDS sont valides dans une telle expression",//396
		"Seuls des NOEUDS peuvent être associés à des variables $#",//397
		"affichagegénération",//398
		"Fichier de probabilités inconnu",//399
		"Fichier de probabilités corrompu",//400
		"'.' manquant à la fin de cette règle d'ordre",//401
		"séquencesurcontexte",//402
        "@"};
        


