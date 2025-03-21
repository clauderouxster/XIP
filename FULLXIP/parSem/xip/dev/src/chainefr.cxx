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
    
    fprintf(fichier,"s�mantique\n"); 
    fprintf(fichier,"termes\n"); 
    fprintf(fichier,"reconstructeur\n");  
    fprintf(fichier,"Echec sur la remont�e des traits.\n");  
    fprintf(fichier,version); 
    fprintf(fichier,"Changement par r�gle de deduction\n"); 
    fprintf(fichier,"Destruction par r�gle de deduction \n"); 
    fprintf(fichier," donne: \n");  
    fprintf(fichier," avec \n");   
    fprintf(fichier,"Partie droite de la r�gle ne correspond pas\n");  
    fprintf(fichier,"Un test a d�j� �t� introduit par un 'if'\n"); 
    fprintf(fichier,"Attention, il faut une variable pour tester les traits\n");
    fprintf(fichier,"Un point doit �tre pr�sent dans le nom de fichier\n");
    fprintf(fichier,"R�ponse inconnue\n");
    fprintf(fichier,"d�sambigu�sation\n");
    fprintf(fichier,"Champ Nombre manquant ou mal �crit\n");
    fprintf(fichier,"Champ Doublons manquant ou mal �crit\n");
    fprintf(fichier,"n�gation\n");
    fprintf(fichier,"toutmajuscule\n");
    fprintf(fichier,"r�gle\n");
    fprintf(fichier,"r�gles\n");
    fprintf(fichier,"dir�gles\n");
    fprintf(fichier,"t�tes\n");
    fprintf(fichier,"marqueur\n");
    fprintf(fichier,"pr�std\n");
    fprintf(fichier,"pr�rct\n");
    fprintf(fichier,"abr�viations\n");
    fprintf(fichier,"d�duction\n");
    fprintf(fichier,"Erreur, ce champ est inconnu, on attend la valeur: marqueur\n");
    fprintf(fichier,"On attend soit '&' soit ')' soit ';' dans cette r�gle\n");
    fprintf(fichier,"On attend '(' dans cette r�gle\n");
    fprintf(fichier,"abr�viations\n");
    fprintf(fichier,"La variable dans cette fonction ne correspond pas � une variable des tests associ�s\n");
    fprintf(fichier,"Les regles Marqueurs doivent commencer par 'chiffre>' ex: 1> \n");
    fprintf(fichier,"On attend '=' dans cette r�gle\n");
    fprintf(fichier,"On attend '{' dans cette r�gle de d�faut\n");
    fprintf(fichier,"Erreur dans la r�gle, ce caract�re n'est pas valide:\n");
    fprintf(fichier,"On attend '&' dans cette r�gle de d�faut\n");
    fprintf(fichier,"On attend '=' dans cette r�gle\n");
    fprintf(fichier,"On attend ',' ou '}' dans cette r�gle de d�faut\n");
    fprintf(fichier,"d�fauts\n"); 
    fprintf(fichier,"\n");
    fprintf(fichier,"\n");
    fprintf(fichier,"Le symbole d'affectation '=' est attendu dans ce cas\n");
    fprintf(fichier,"d�finitions\n");
    fprintf(fichier,"Pas plus d'une valeur dans la d�finition\n");
    fprintf(fichier,"Il manque une ',' ou un '=' dans cette r�gle\n");
    fprintf(fichier,"zones\n");
    fprintf(fichier,"filtrecouche\n");
    fprintf(fichier,"d�coupes�quence\n");
    fprintf(fichier,"Une seule cat�gorie dans la partie droite de la r�gle de simplification\n"); 
    fprintf(fichier,"cat�gories\n");
    fprintf(fichier," n'a pas de d�finition de traits\n");
    fprintf(fichier,"plr�gles\n");
    fprintf(fichier,"La fl�che est manquante ou mal �crite\n");
    fprintf(fichier,"Il manque une paranth�se\n");
    fprintf(fichier,"Erreur lors de la lecture d'une regle s�mantique\n");
    fprintf(fichier,"La cat�gorie finale ne peut �tre un '?'\n");
    fprintf(fichier,"Cette couche est filtr�e et ne s'applique pas:\n");
    fprintf(fichier,"La partie droite d'une affectation doit se trouver entre {}\n");
    fprintf(fichier,"Il manque une parenth�se fermante\n"); 
    fprintf(fichier,"Il n'y a pas d'abr�viation\n");
    fprintf(fichier,"Pas de parenth�se ici...\n");
    fprintf(fichier,"Seule une valeur de trait peut appara�tre apr�s un '='\n");
    fprintf(fichier,"Attention: Affectation multiple de cat�gories pour une entr�e -> Cat�gorie conserv�: \n");
    fprintf(fichier,"d�faut\n");
    fprintf(fichier,"Il n'existe aucune r�gle par d�faut pour ce trait\n");
    fprintf(fichier,"Erreur, il faut le coupe-choix avec l'analyse longue\n");
    fprintf(fichier,"Impossible, ce trait a d�j� �t� d�clar�\n"); 
    fprintf(fichier,"La partie droite contient des erreurs...\n");
    fprintf(fichier," n'a pas de d�finition de traits\n"); 
    fprintf(fichier,"Cette couche est d�j� associ� avec un filtre.\n");
    fprintf(fichier,"R�gle:\n");
    fprintf(fichier,"Seule une balise d�but peut recevoir ce type de traits\n");
    fprintf(fichier,"Trop de doublons dans cette r�gle,augmenter la valeur de DOUBLONS\n");
    fprintf(fichier,"Pas de r�gle\n");
    fprintf(fichier,"Ce filtre doit �tre associ� � un num�ro de couche.\n");
    fprintf(fichier,"Operateur inconnu: \n");
    fprintf(fichier,"Cette regle commence avec une structure trop complexe...\n");
    fprintf(fichier,"Attention, dans le cas d'un changement, il n'est possible de produire qu'une seule solution.\n");
    fprintf(fichier,":On choisit la r�gle \n"); 
    fprintf(fichier,"On cr�e \n");
    fprintf(fichier,":�chec sur les traits\n");
    fprintf(fichier,":On arr�te les recherches\n");
    fprintf(fichier,"Erreur:Mont�e des traits:\n");
    fprintf(fichier,"Erreur:R�gle de d�faut inexistante. Contexte:\n");
    fprintf(fichier,"Erreur n�gation: \n");
    fprintf(fichier,"Erreur, la d�pendance doit �tre enclose entre '<...>'\n"); 
    fprintf(fichier,"Trop de noms diff�rents (attribut ou valeur) : \n");
    fprintf(fichier,"Erreur de la lecture de la zone\n");
    fprintf(fichier,"affichagefonction\n"); 
    fprintf(fichier,"Attention, une seule fonction peut �tre marqu�e � la fois\n");
    fprintf(fichier,"Seul un ';' est acceptable ici\n");
    fprintf(fichier,"Attention, il manque une '*' ou un '+' apr�s les parenth�ses\n");
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
    fprintf(fichier,"Les variables dans un test doivent �tre associees � des traits\n");
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
    fprintf(fichier,"Une valeur doit �tre sp�cifi� pour chacun des attributs au sein d'une PL-r�gle.\n");
    fprintf(fichier,"Ce mot n'est pas dans la liste de vocabulaire --> \n");
    fprintf(fichier,"Une regle ne peut avoir de partie droite vide.\n");
    fprintf(fichier,"erreurs.err\n");
    fprintf(fichier,"Nombre de Cat�gorie trop grand\n");
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
    fprintf(fichier," � la ligne:\n");
    fprintf(fichier,"Plus de m�moire.\n");
    fprintf(fichier,"Echec d� � la PL-r�gle:\n");
    fprintf(fichier,"Temps: \n");
    fprintf(fichier,"Caract�re de d�but manquant ou erron�\n");
    fprintf(fichier,"Aucune valeur trouv�e pour cet attribut\n");
    fprintf(fichier,"indent�es\n");
    fprintf(fichier,"Le(s) trait(s) suivant(s) doit(vent) �tre pr�sent sur ce noeud: \n"); 
    fprintf(fichier,"On veut associer le noeud: \n");
    fprintf(fichier,"Correspondance refus�e entre ce noeud et cet �l�ment de la r�gle\n");
    fprintf(fichier,"El�ment de la r�gle: \n");
    fprintf(fichier,"Abr�viation d�j� utilis�e : \n");
    fprintf(fichier,"Abr�viation inconnue : \n");
    fprintf(fichier,"Erreur dans la lecture de l'it�ration maximale pour une categorie\n");
    fprintf(fichier,"surface\n");
    fprintf(fichier,"Aucun test sur des valeurs de traits n'est permis ici\n");
    fprintf(fichier,"La r�gle impose que ce noeud ait une valeur pour les traits suivants: \n");
    fprintf(fichier,"Traits non trouv�s dans les abr�viations:\n"); 
    fprintf(fichier,"locale\n");
    fprintf(fichier,"La forme exacte est: ..{ \n"); 
    fprintf(fichier,"Structure de traits non analysable : \n"); 
    fprintf(fichier,"S�quence. Couche: \n"); 
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
    fprintf(fichier,"Champ 'chiffre' absent ou mal �crit\n"); 
    fprintf(fichier,"Apr�s Simplification.\n"); 
    fprintf(fichier,"On �crase l'entr�e: \n");
    fprintf(fichier,"traduction\n");
    fprintf(fichier,"Il manque un '.' ou un '['\n"); 
    fprintf(fichier,"\n"); 
    fprintf(fichier,"\n"); 
    fprintf(fichier,"fonctions\n");
    fprintf(fichier,": cat�gorie inconnue\n");
    fprintf(fichier,"Tous les arguments de cette fonction n'ont pu �tre associ�s avec un element dans la partie droite\n");
    fprintf(fichier,"Extraction r�ussie.\n");
    fprintf(fichier,"cacher\n");
    fprintf(fichier,"Une cat�gorie optionnelle ne peut commencer une r�gle\n");
    fprintf(fichier,"Il manque la taille de la matrice d'affichage (X ou Y)\n");
    fprintf(fichier,"Cette couche doit �tre du type Simplification\n");
    fprintf(fichier,"Fonction inconnue: \n");
    fprintf(fichier,"extraction\n");
    fprintf(fichier,"s�quence\n");
    fprintf(fichier,"Echec extraction.\n");
    fprintf(fichier,"Erreur... le contexte droit doit apparaitre avant le point final \n");
    fprintf(fichier,"Erreur... le contexte gauche ne s'applique pas\n");
    fprintf(fichier,"Erreur... le contexte droit ne s'applique pas\n");
    fprintf(fichier,"Erreur sur la valeur du champ libre :\n");
    fprintf(fichier,"Erreur, Cette couche est d�j� d'un autre type= \n");
    fprintf(fichier,"Marquage: contexte droit insuffisant\n");
    fprintf(fichier,"Erreur, il manque une accolade fermante pour un test \n");
    fprintf(fichier,"Erreur, les r�gles d'affectation ne sont disponibles qu'au sein d'une couche\n");
    fprintf(fichier,"Erreur, Cette couche est d�j� du type: r�gle DI Cat�gorie= \n");
    fprintf(fichier,"Erreur, il manque l'indice de couche pour cette pl-r�gle\n");  
    fprintf(fichier,"Erreur, Aucune DI-r�gle ne peut s'associer avec cette PL-r�gle dans cette couche\n");
    fprintf(fichier,"Erreur, plusieurs cat�gories portent le trait last dans cette r�gle\n");
    fprintf(fichier,"Erreur, plusieurs cat�gories portent le trait first dans cette r�gle\n");
    fprintf(fichier,"Erreur, ce champ est inconnu: \n");
    fprintf(fichier,"codagexml\n");
    fprintf(fichier,"Un seul attribut est permis ici...\n");
    fprintf(fichier,"On ne peut pas g�n�rer plus d'une d�pendance � la fois dans ce contexte.\n");
    fprintf(fichier,"doubler�duction\n");
    fprintf(fichier,"Succ�s lors de l'application de la r�gle.\n");
    fprintf(fichier,"Echec lors de l'application du contexte ou lors d'une comparaison sur les traits.\n");
    fprintf(fichier,"Echec, champ absent..\n");
    fprintf(fichier,"Erreur... Il faut une variable $xxx avec le '^'.\n");
    fprintf(fichier,"Index non valide... Reg�n�rez l'index...\n");
    fprintf(fichier,"Le caract�re '\"' doit toujours �tre pr�c�d� du caract�re d'�chappement: \\\"\n");
    fprintf(fichier,"Cette r�gle d�truit toutes les lectures de: \n");
    fprintf(fichier,". Elle ne peut s'appliquer.\n");
    fprintf(fichier,"> Echec de la r�gle: \n");
    fprintf(fichier," commen�ant au noeud: \n");
    fprintf(fichier,"> R�gle de s�quence: \n");
    fprintf(fichier,"Erreur: boucle infinie...\n");
    fprintf(fichier,"R�gle d'extraction: \n");
    fprintf(fichier,"R�gle de d�pendance: \n");
    fprintf(fichier,"Succ�s: \n");
    fprintf(fichier," sur le noeud: \n");
    fprintf(fichier,"R�gle de marquage: \n");
    fprintf(fichier,"R�gle de reconstruction: \n");
    fprintf(fichier,"Application de la r�gle de reconstruction: \n");
    fprintf(fichier,"garder\n");
    fclose(fichier);
    return 0;
}

const char* CHAINESFR[] = {
        "s�mantique", //0
        "termes", //1
        "reconstructeur",  //2
        "Echec sur la remont�e des traits.",  //3
        VERSION_XIP_BUILD,//4
        "Changement par r�gle de deduction", //5
        "Destruction par r�gle de deduction ", //6
        " donne: ",  //7
        " avec ",   //8
        "Partie droite de la r�gle ne correspond pas",  //9
        "Un test a d�j� �t� introduit par un 'if'", //10
        "Attention, il faut une variable pour tester les traits",//11
        "Un point doit �tre pr�sent dans le nom de fichier",//12
        "R�ponse inconnue",//13
        "d�sambigu�sation",//14
        "Champ Nombre manquant ou mal �crit",//15
        "Champ Doublons manquant ou mal �crit",//16
        "n�gation",//17
        "toutmajuscule",//18
        "r�gle",//19
        "r�gles",//20
        "dir�gles",//21
        "t�tes",//22
        "marqueur",//23
        "pr�std",//24
        "pr�rct",//25
        "abr�viations",//26
        "d�duction",//27
        "Erreur, ce champ est inconnu, on attend la valeur: marqueur",//28
        "On attend soit '&' soit ')' soit ';' dans cette r�gle",//29
        "On attend '(' dans cette r�gle",//30
        "abr�viations",//31
        "La variable dans cette fonction ne correspond pas � une variable des tests associ�s",//32
        "Les regles Marqueurs doivent commencer par 'chiffre>' ex: 1> ",//33
        "On attend '=' dans cette r�gle",//34
        "On attend '{' dans cette r�gle",//35
        "Erreur dans la r�gle, ce caract�re n'est pas valide:",//36
        "On attend '&' dans cette r�gle de d�faut",//37
        "",//38
        "On attend ',' ou '}' dans cette r�gle de d�faut",//39
        "d�fauts", //40
        "taillemot",//41
        "",//42
        "Le symbole d'affectation '=' est attendu dans ce cas",//43
        "d�finitions",//44
        "Pas plus d'une valeur dans la d�finition",//45
        "Il manque une ',' ou un '=' dans cette r�gle",//46
        "zones",//47
        "filtrecouche",//48
        "d�coupes�quence",//49
        "Une seule cat�gorie dans la partie droite de la r�gle de simplification", //50
        "cat�gories",//51
        " n'a pas de d�finition de traits",//52
        "plr�gles",//53
        "La fl�che est manquante ou mal �crite",//54
        "Il manque une paranth�se",//55
        "Erreur lors de la lecture d'une regle s�mantique",//56
        "La cat�gorie finale ne peut �tre un '?'",//57
        "Cette couche est filtr�e et ne s'applique pas:",//58
        "La partie droite d'une affectation doit se trouver entre {}",//59
        "Il manque une parenth�se fermante", //60
        "Il n'y a pas d'abr�viation",//61
        "Pas de parenth�se ici...",//62
        "Seule une valeur de trait peut appara�tre apr�s un '='",//63
        "Attention: Affectation multiple de cat�gories pour une entr�e -> Cat�gorie conserv�: ",//64
        "d�faut",//65
        "Il n'existe aucune r�gle par d�faut pour ce trait",//66
        "Erreur, il faut le coupe-choix avec l'analyse longue",//67
        "Impossible, une valeur a d�j� �t� donn�e pour ce trait : ", //68
        "La partie droite contient des erreurs...",//69
        " n'a pas de d�finition de traits", //70
        "Cette couche est d�j� associ� avec un filtre.",//71
        "R�gle:",//72
        "Seule une balise d�but peut recevoir ce type de traits",//73
        "Trop de doublons dans cette r�gle,augmenter la valeur de DOUBLONS",//74
        "Pas de r�gle",//75
        "Ce filtre doit �tre associ� � un num�ro de couche.",//76
        "Operateur inconnu: ",//77
        "Cette regle commence avec une structure trop complexe...",//78
        "Attention, dans le cas d'un changement, il n'est possible de produire qu'une seule solution.",//79
        ":On choisit la r�gle ", //80
        "On cr�e ",//81
        ":�chec sur les traits",//82
        ":On arr�te les recherches",//83
        "Erreur:Mont�e des traits:",//84
        "Erreur:R�gle de d�faut inexistante. Contexte:",//85
        "Erreur n�gation: ",//86
        "Erreur, la d�pendance doit �tre enclose entre '<...>'", //87
        "Trop de noms diff�rents (attribut ou valeur) : Veuillez augmenter la valeur du champ NOMBRE dans votre fichier langue.xip",//88
        "Erreur de la lecture de la zone",//89
        "affichagefonction", //90
        "Attention, une seule fonction peut �tre marqu�e � la fois",//91
        "Seul un ';' est acceptable ici",//92
        "Attention, il manque une '*' ou un '+' apr�s les parenth�ses",//93
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
        "Les variables dans un test doivent �tre associees � des traits",//126
        "L'une des variables dans la partie droite ne correspond pas a une variable dans la partie gauche...",//127
        "lemme",//128
        "Seules des variables de noeuds sont accept�es ici (#1, #2..)",//129
        "Les definitions locales de traits s'inserent avec '<traits>'",//130
        "Il manque une ',' ou une '}'",//131
        "Il manque une ',' ou un '.' ou un ';'",//132
        "Il manque une ',' ou une ')'",//133
        "Il manque une ',' ou un ']'",//134
        "Il manque un ':'",//135
        "Une seule valeur possible pour le champ libre",//136
        "Une valeur doit �tre sp�cifi� pour chacun des attributs au sein d'une PL-r�gle.",//137
        "Ce mot n'est pas dans la liste de vocabulaire --> ",//138
        "Une regle ne peut avoir de partie droite vide.",//139
        "erreurs.err",//140
        "Nombre de Cat�gorie trop grand",//141
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
        " � la ligne:",//153
        "Plus de m�moire.",//154
        "Echec d� � la PL-r�gle:",//155
        "Temps: ",//156
        "Caract�re de d�but manquant ou erron�",//157
        "Aucune valeur trouv�e pour cet attribut",//158
        "indent�es",//159
        "Le(s) trait(s) suivant(s) doit(vent) �tre pr�sent sur ce noeud: ", //160
        "On veut associer le noeud: ",//161
        "Correspondance refus�e entre ce noeud et cet �l�ment de la r�gle",//162
        "El�ment de la r�gle: ",//163
        "Abr�viation d�j� utilis�e : ",//164
        "Abr�viation inconnue : ",//165
        "Erreur dans la lecture de l'it�ration maximale pour une categorie",//166
        "surface",//167
        "Aucun test sur des valeurs de traits n'est permis ici",//168
        "La r�gle impose que ce noeud ait une valeur pour les traits suivants: ",//169
        "Traits non trouv�s dans les abr�viations:", //170
        "locale",//171
        "La forme exacte est: ..{ ", //172
        "Structure de traits non analysable : ", //173
        "S�quence. Couche: ", //174
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
        "Champ 'chiffre' absent ou mal �crit", //185
        "Apr�s Simplification.", //186
        "On �crase l'entr�e: ",//187
        "traduction",//188
        "Il manque un '.' ou un '['", //189
        "Nombre maximum de r�gles atteint dans un fichier param�tre.", //190
        "Le nom du chemin 'XPath' doit �tre suivi d'un '('", //191
        "fonctions",//192
        ": cat�gorie inconnue",//193
        "Tous les arguments de cette fonction n'ont pu �tre associ�s avec un element dans la partie droite",//194
        "Extraction r�ussie.",//195
        "cacher",//196
        "Une cat�gorie optionnelle ne peut commencer une r�gle",//197
        "Il manque la taille de la matrice d'affichage (X ou Y)",//198
        "Cette couche doit �tre du type Simplification",//199
        "Fonction inconnue: ",//200
        "extraction",//201
        "s�quence",//202
        "Echec extraction.",//203
        "Erreur... le contexte droit doit apparaitre avant le point final ",//204
        "Erreur... le contexte gauche ne s'applique pas",//205
        "Erreur... le contexte droit ne s'applique pas",//206
        "Erreur sur la valeur du champ libre :",//207
        "Erreur, Cette couche est d�j� d'un autre type= ",//208
        "Marquage: contexte droit insuffisant",//209
        "Erreur, il manque une accolade fermante pour un test ",//210
        "Erreur, les r�gles d'affectation ne sont disponibles qu'au sein d'une couche",//211
        "Erreur, Cette couche est d�j� du type: r�gle DI Cat�gorie= ",//212
        "Erreur, il manque l'indice de couche pour cette pl-r�gle",  //213
        "Erreur, Aucune DI-r�gle ne peut s'associer avec cette PL-r�gle dans cette couche",//214
        "Erreur, plusieurs cat�gories portent le trait last dans cette r�gle",//215
        "Erreur, plusieurs cat�gories portent le trait first dans cette r�gle",//216
        "Erreur, ce champ est inconnu: ",//217
        "codagexml",//218
        "Un seul attribut est permis ici...",//219
        "On ne peut pas g�n�rer plus d'une d�pendance � la fois dans ce contexte.",//220
        "doubler�duction",//221
        "Succ�s lors de l'application de la r�gle.",//222
        "Echec lors de l'application du contexte ou lors d'une comparaison sur les traits.",//223
        "Echec, champ absent...",//224
        "Erreur... Il faut une variable $xxx avec le '^'.",//225
        "Index non valide... Reg�n�rez l'index...",//226
        "Le caract�re '\"' doit toujours �tre pr�c�d� du caract�re d'�chappement: \\\"",//227
        "Cette r�gle d�truit toutes les lectures de: ",//228
        ". Elle ne peut s'appliquer.",//229
        "> Echec de la r�gle de s�quence: ",//230
        " commen�ant au noeud: ",//231
        "> R�gle de s�quence: ",//232
        "Erreur: boucle infinie...",//233
        "R�gle d'extraction: ",//234
        "R�gle de d�pendance: ",//235
        "Succ�s: ",//236
        " sur le noeud: ",//237
        "R�gle de marquage: ",//238
        "R�gle de reconstruction: ",//239
        "Application de la r�gle de reconstruction: ",//240
        "garder",//241
        "Ce noeud de r�gle de reconstruction peut occasionner la perte de mots dans l'arbre syntaxique",//242
        "Avertissement, cette r�gle a consomm� trop de temps d'analyse", //243
        "Erreur: cet attribut n'a jamais �t� d�clar�.",//244
        "Erreur: Seule une structure de noeud (#num) peut �tre utilis�e ici.",//245
        "Champ XML manquant.",//246
        "Erreur dans la formule XML.",//247
        "M�thode inconnue.",//248
        "Une variable pour un noeud maison ne peut pas �tre utilis� ailleurs dans la formule",//249
        "La DTD contient des erreurs",//250
        "Ce champ de DTD ne correspond pas � une action pour XIP",//251
        "Cette description de traits devrait comprendre deux �l�ments parmi ses valeurs possibles",//252
        "Cette proc�dure a d�j� �t� d�clar�e",//253
        "Une proc�dure ne peut �tre le r�sultat d'une r�gle de d�duction",//254
        "Cette proc�dure n'a pas de corps d'analyse",//255
        "'_goto' doit �tre unique",//256
        "Ce label a d�j� �t� utilis�",//257
        "Espace d'analyse complet (v�rifiez vos boucles d'analyse)",//258
        "Fichier Param�tre inconnu",//259
        "Trace activ�e",//260
        "Chargement de la grammaire en cours",//261
        "Initialisation du Parseur",//262
        "Les fichiers param�tres ne peuvent �tre charg�s en mode: limit�...",//263
        "Les expressions XIP doivent �tre encloses entre deux '!'",//264
        "Action inconnue: ",//265
        "Action par d�faut inconnue",//266
        "Fonction inconnue (type possible: analyze,merge,feature ou skip)",//267
        "Mauvais nombre d'argument pour une transformation en traits.",//268
        "Param�tre manquant.\nActions possibles:\n\tskip=text|all\n\tanalyze=token|chunk|dependency\n\tfeature=string,string,(token|chunk|dependency).",//269
        "Param�tre inconnu pour _display",//270
        "Le '}' est absent dans une boucle 'WHILE'",//271
        "Il manque un 'WHILE' pour ce '}'",//272
        "Cette �tiquette est inconnue",//273
        "On ne peut affecter une valeur pr�c�d�e d'une n�gation",//274
        "Erreur dans la d�finition de votre lex�me: Parenth�se manquante.",//275
        "Erreur dans la d�finition de votre lex�me: Crochet manquant.",//276
        "Erreur dans la d�finition de votre lex�me: La n�gation ne peut pr�c�der une parenth�se.",//277
        "maxmots",//278
        "Le nombre maximum de mots a �t� atteint",//279
        "Caract�re ',' ou ';' or '}' ou '|' manquant",//280
        "Il faut d�finir une fen�tre de cat�gories pour utiliser des '*' ou des '+': Cat#max",//281
        "La n�gation ne peut �tre appliqu�e qu'� une cat�gorie.",//282
        "L'op�rateur '?-' doit n�cessairement �tre en fin de r�gle.",//283
        "La virgule est mal plac�e dans cette expression r�guli�re",//284
        "On ne peut utiliser une '*' ou un '+' a cet endroit",//285
        "On attend une parenth�se fermante ici: ')'", //286
        "On attend un crochet fermant ici",//287
        "fragmenter",//288
        "Les variables d'expressions r�guli�res doivent appara�tre par ordre croissant 1,2...9",//289
        "On peut seulement avoir un signe d'affectation ('=') devant une variable XIP dans une cr�ation de noeud XML",//290
        "Apr�s un '|' on ne peut avoir que: 'if', 'iftest' ou '{'",//291
        "Erreur dans la lecture de la r�gle de d�duction: ",//292
        "Concept ontologique inconnu",//293
        "Ce chemin ontologique est incoh�rent",//294
        "affichageontologie",//295
        "Erreur: il manque l'alias au sein de l'expression XML",//296
        "Erreur: une string ou un vector doit toujours commencer avec un '_'",//297
        "Erreur: Il faut fournir un indice � cette variable de type vector.",//298
        "Erreur: Ce '}' n'est pas associ� avec un 'for'..",//299
        "Erreur: Il manque une '}'...",//300
        "Erreur: 'iftest' mal �crit..",//301
        "Erreur: Accolade '}' manquante...",//302
        "Erreur: type inconnu...",//303
        "Accolade fermante sans accolade ouvrante...",//304
        "Une expression XMLGuide doit se terminer avec un '.' ou un ';'",//305
        "On doit toujours finir une proc�dure avec un 'return'",//306
        "Proc�dure ou instruction inconnue:",//307
        "Type de param�tre inconnu:",//308
        "Cette variable a d�j� �t� d�clar�e:",//309
        "Les r�gles lexicales doivent �tre pr�c�d�es d'un num�ro de couche ici",//310
        "On ne peut utiliser de variables de noeuds entre crochets de structure de traits [..]",//311
        "Type de vecteur impossible",//312
        "Il manque un '.' dans cette variable struct",//313
        "script",//314
        "Le 'if' ne peut �tre utilis� au sein des r�gles de script",//315
        "Le 'iftest' ne peut �tre utilis� au sein des r�gles de d�duction",//316
        "Les instructions de boucle sont interdites au sein des r�gles de d�duction",//317
        "Une d�pendance peut seulement �tre cr�ee au sein des r�gles de d�duction",//318
        "Une suite d'instruction peut seulement �tre utilis�e au sein d'un script",//319
        "final",//320
        "On attend une ',' ou un ';' ici",//321
        "Attention. Cette d�pendance est d�j� d�clar�e:",//322
        "Champ inconnu dans un fichier grammaire:",//323
        "Erreur... Cette variable est d�j� soit une cat�gorie soit une d�pendance soit une instruction: ",//324
        "Erreur... Partie manquante dans la formule",//325
        "Erreur... Cette version de XIP ne peut lire des expressions XML",//326
        "Erreur... Op�rateur manquant",//327
        "enchargeant",//328
        "initial",//329
        "Parenth�se manquante dans un #ifdef",//330
        "#endif ou #else manquant",//331
        "#endif manquant pour ce #else",//332
        "#else sans #ifdef",//333
        "multimots",//334
        "Cette expression a d�j� �t� utilis�e",//335
        "Erreur: Cette cha�ne est trop longue",//336
        "Erreur: Cette r�gle est mal �crite et inapplicable",//337
        "janvier",//338
        "f�vrier",//339
        "mars",//340
        "avril",//341
        "mai",//342
        "juin",//343
        "juillet",//344
        "ao�t",//345
        "septembre",//346
        "octobre",//347
        "novembre",//348
        "d�cembre",//349
        "lundi",//350
        "mardi",//351
        "mercredi",//352
        "jeudi",//353
        "vendredi",//354
        "samedi",//355
        "dimanche",//356
        "encodageutf8lexique",//357
        "Interface manquante pour un appel Python",//358
        "Il n'y a pas de nom de fonction Python correspondant � l'interface",//359
        "Fonction Python inconnue",//360
        "Erreur, on attend un noeud fictif ici: ##CAT[traits]",//361
        "Erreur, cette variable ne peut �tre une racine. Elle ne pointe pas sur un seul noeud.",//362
        "ATTENTION. Ce module Python a d�j� �t� charg�",//363
		"Erreur: Le code est sup�rieur � 255",//364
		"d�finitioncat�gorie",//365
		"d�coupecat�gorie",//366
		"Cette valeur a d�j� �t� d�clar�e pour cet attribut",//367
		"Encodage inconnu (valeurs accept�es: 'utf8' ou 'latin')",//368
		"Num�ro de couche manquant pour ce type de r�gle.",//369
		"Partie droite de cette r�gle de reconstruction manquante ou erron�e",//370
		"Cat�gorie manquante devant une '{'",//371
		"La partie droite d'une r�gle de s�quence ne peut contenir de sous-noeuds",//372
		"Expression ontologique inconnue",//373
		"Erreur... L'op�rateur '-' ne peut �tre utilis� qu'en fin de s�quence",//374
		"multilemmes",//375
		"Racine de l'espace de nom inconnu. V�rifiez que la premi�re partie de votre nom d'espace soit la m�me que toutes vos autres d�finitions",//376
		"Instruction de format inconnu",//377
		"Fermeture de commentaire: '\\' manquante",//378
		"Le champ guesserfeature n'a pas �t� d�fini dans votre fichier de langue. Le test sur langue est impossible",//379
		"d�finitiontraits",//380
		"expressionfinphrase",//381
		"\" de fermeture manquant",//382
		"Il manque un '.' � la fin de cette r�gle multimots",//383
		"g�n�ration",//384
		"g�n�retraits",//385
		"R�gle g�n�retraits incorrecte",//386
		"Flux manquant",//387
		"Lemme manquant",//388
		"Traits manquants",//389
		"'.' manquant � la fin de cette d�pendance",//390
		"noeuds",//391
		"Seules des fonctions d�clar�es comme NOEUDS sont accept�es comme r�sultat ici",//392
		"Pas de formule autoris�e dans ce contexte",//393
		"D�claration de d�pendances erron�e",//394
		"ordre",//395
		"Seuls des NOEUDS sont valides dans une telle expression",//396
		"Seuls des NOEUDS peuvent �tre associ�s � des variables $#",//397
		"affichageg�n�ration",//398
		"Fichier de probabilit�s inconnu",//399
		"Fichier de probabilit�s corrompu",//400
		"'.' manquant � la fin de cette r�gle d'ordre",//401
		"s�quencesurcontexte",//402
        "@"};
        


