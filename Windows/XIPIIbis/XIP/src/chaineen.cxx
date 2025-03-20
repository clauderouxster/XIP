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
   filename   : chaineen.cxx
   Date       : 10/01/1999
   Purpose    : English message strings
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

char RecreeChainesAnglais(char* nom) {
	FILE* fichier=fopen(nom,"w\n");
        if (fichier==NULL)
            return -1;       

        char version[100];
        sprintf_s(version,100,"%s\n",VERSION_XIP_BUILD);

	fprintf(fichier,"semantic\n");//0
	fprintf(fichier,"terms\n");
        fprintf(fichier,"reshuffling\n");
        fprintf(fichier,"Failure on feature percolation\n");
        fprintf(fichier,version);
        fprintf(fichier,"Change with deduction rule\n");
        fprintf(fichier,"Destruction with deduction rule\n");
        fprintf(fichier," yield: \n");
        fprintf(fichier," with \n");
        fprintf(fichier,"Right-side of the rule does not match\n");
        fprintf(fichier,"A test has already been written with IF\n");
        fprintf(fichier,"A variable is needed to test features\n");
        fprintf(fichier,"A file name should comprise a '.'\n");
        fprintf(fichier,"Unknown Answer: \n");
        fprintf(fichier,"tagging\n");
        fprintf(fichier,"Missing Field: 'NUMBER'\n");
        fprintf(fichier,"Missing Field: 'DOUBLE'\n");
        fprintf(fichier,"negation\n");
        fprintf(fichier,"alluppercase\n");
        fprintf(fichier,"rule\n");
        fprintf(fichier,"rules\n");
        fprintf(fichier,"idrules\n");
        fprintf(fichier,"heads\n");
        fprintf(fichier,"marker\n");
        fprintf(fichier,"predfs\n");
        fprintf(fichier,"prefcr\n");
        fprintf(fichier,"alias\n");
        fprintf(fichier,"dependencyrules\n");
        fprintf(fichier,"Error: field unknown, expected value is: 'marker'\n");//28
        fprintf(fichier,"Error: Rule should contain ';' or '&' or ')'\n");//29
        fprintf(fichier,"Error: Rule should contain '('\n");//30
        fprintf(fichier,"alias\n");//31
        fprintf(fichier,"The variable in this function does not match any variables in the test\n");//32
        fprintf(fichier,"Marking Rules should start with a layer number. ex: 1> \n");//33
        fprintf(fichier,"Error: Rule should contain '='\n");//34
        fprintf(fichier,"Error: Default Rule should contain '{'\n");//35
        fprintf(fichier,"Error: this character is invalid:\n");//36
        fprintf(fichier,"Error: Default Rule should contain '&'\n");
        fprintf(fichier,"Error: Default Rule should contain '='\n");
        fprintf(fichier,"Error: Default Rule should contain ',' or '}'\n");
        fprintf(fichier,"default\n");//40
        fprintf(fichier,"wordsize\n");//41
        fprintf(fichier,"\n");
        fprintf(fichier,"'=' expected\n");
        fprintf(fichier,"definitions\n");
        fprintf(fichier,"One value only for this definition\n");
        fprintf(fichier,"Error: Rule should contain ',' or '='\n");
        fprintf(fichier,"zones\n");
        fprintf(fichier,"layerfilter\n");//48
        fprintf(fichier,"split\n");
        fprintf(fichier,"Only one POS permitted in the right-hand side of this disambiguation rule\n");
        fprintf(fichier,"pos\n");
        fprintf(fichier," Feature Definition Required\n");//52
        fprintf(fichier,"lprules\n");
        fprintf(fichier,"Missing arrow\n");
        fprintf(fichier,"Missing '('\n");
        fprintf(fichier,"Error while reading a semantic rule...\n");
        fprintf(fichier,"The last category cannot be a '?'\n");
        fprintf(fichier,"Layer Filtering discard layer:\n");
        fprintf(fichier,"The right-side of an affectation should be enclosed between {}\n");//59
        fprintf(fichier,"Missing ')'\n");
        fprintf(fichier,"Unknown alias\n");
        fprintf(fichier,"No '(' here\n");
        fprintf(fichier,"Feature Value required after a '='\n");
        fprintf(fichier,"Warning: Multiple POS Affectation for a lexicon entry -> Defined POS is: \n");
        fprintf(fichier,"default\n");
        fprintf(fichier,"There is no default rule for this feature\n");
        fprintf(fichier,"Error, Longuest Match requires a cut ('!')\n");
        fprintf(fichier,"Error: this attribute has been declared already.\n"); //68
        fprintf(fichier,"Errors in the right-side of that rule\n");
        fprintf(fichier," does not have a feature definition\n");
        fprintf(fichier,"This layer has already been associated with a filter.\n");
        fprintf(fichier,"Rule:\n");//74
        fprintf(fichier,"Only a starting tag can be associated with this sort of features\n");//73
        fprintf(fichier,"Too many doubles in this rule, increase the value of DOUBLES\n");
        fprintf(fichier,"No rules\n");//75
        fprintf(fichier,"A layer number must be given to that layer filter.\n");//76
        fprintf(fichier,"Unknown Operator: \n");
        fprintf(fichier,"This extraction rule starts with a too complexe structure...\n");//78
        fprintf(fichier,"Only one solution can be produced at a time, when a dependency is to be modified\n");//79
        fprintf(fichier,":We select the rule \n");
        fprintf(fichier,"We create \n");
        fprintf(fichier,":Failure on feature comparison\n");
        fprintf(fichier,":Stop Searching\n");
        fprintf(fichier,"Error: Feature Percolation\n");
        fprintf(fichier,"Error:Absent Default Rule. Contexte:\n");
        fprintf(fichier,"Error negation: \n");
        fprintf(fichier,"Error, the dependency should be enclosed between '<...>'\n");//97
        fprintf(fichier,"Too many name definitions: Update the NUMBER field in your language.xip file\n");
        fprintf(fichier,"Error reading zone\n");
        fprintf(fichier,"functiondisplay\n");
        fprintf(fichier,"Warning, only one function can be marked at a time\n");
        fprintf(fichier,"Only a ';' can be used in this case\n");//92
        fprintf(fichier,"Error. A '*' or a '+' is missing after the parentheses\n");//93
        fprintf(fichier,"Error: Incorrect Code Name :\n");
        fprintf(fichier,"yes\n");
        fprintf(fichier,"no\n");
        fprintf(fichier,"Unknown File: \n");
        fprintf(fichier,"no 'lexicons' field possible in this file\n");
        fprintf(fichier,"boundaries\n");
        fprintf(fichier,"trace\n");
        fprintf(fichier,"tracebis\n");
        fprintf(fichier,"number\n");
        fprintf(fichier,"doubles\n");
        fprintf(fichier,"Cannot utilize variable #0, affected by default to the word under disambiguation.\n");
        fprintf(fichier,"Keyword 'where' expected\n");
        fprintf(fichier,"indentation\n");
        fprintf(fichier,"Layer number is in [1-300]\n");
        fprintf(fichier,"features\n");
        fprintf(fichier,"lexicons\n");
        fprintf(fichier,"vocabulary\n");
        fprintf(fichier,"Missing '['\n");
        fprintf(fichier,"Missing ',' or ']'\n");
        fprintf(fichier,"Unknown feature ---> \n");
        fprintf(fichier,"display\n");
        fprintf(fichier,"No display filter\n");
        fprintf(fichier,"Missing ']'\n");
        fprintf(fichier,"Missing '<'\n");
        fprintf(fichier," is an unknown value for \n");
        fprintf(fichier," is not a possible value for \n");
        fprintf(fichier,"postdfs\n");
        fprintf(fichier,"postfcr\n");
        fprintf(fichier,"When no POS defined, use operator '+='\n");
        fprintf(fichier,"Missing '='\n");
        fprintf(fichier," is an unknown attribute\n");
        fprintf(fichier,"In an affectation: left variable with features or missing features for right variable\n");
        fprintf(fichier,"variables in a test must be associated with features\n");
        fprintf(fichier,"A variable in the right-side does not belong to the left-side.\n");
        fprintf(fichier,"lemma\n");
        fprintf(fichier,"Variable required here\n");
        fprintf(fichier,"Use <features> to test on local feature definitions for a lexical entry\n");
        fprintf(fichier,"Missing ',' or '}'\n");
        fprintf(fichier,"Missing ',' or '.'\n");
        fprintf(fichier,"Missing ',' or ')'\n");
        fprintf(fichier,"Missing ',' or ']'\n");
        fprintf(fichier,"Missing ':'\n");
        fprintf(fichier,"Only one value is possible in the LEMMA field\n");
        fprintf(fichier,"A value must be specified for each attribute in a LP rule.\n");
        fprintf(fichier,"Unknown word --> \n");
        fprintf(fichier,"A rule cannot have an empty right-hand side.\n");
        fprintf(fichier,"errors.err\n");
        fprintf(fichier,"Too many POS\n");
        fprintf(fichier,"Tree\n");
        fprintf(fichier,"[c]\n");
        fprintf(fichier,"Error when applying FCR:\n");
        fprintf(fichier,"on \n");
        fprintf(fichier,"ID-rule. Layer: \n");
        fprintf(fichier,":Base:\n");
        fprintf(fichier,":isolate:\n");
        fprintf(fichier,":searching again:\n");
        fprintf(fichier,"Waiting \n");
        fprintf(fichier,"Increase the value of DOUBLES\n");
        fprintf(fichier,"Error in file:\n");
        fprintf(fichier," line:\n");
        fprintf(fichier,"No memory.\n");
        fprintf(fichier,"LP-rule failure: \n");
        fprintf(fichier,"Time: \n");
        fprintf(fichier,"Missing starting character\n");
        fprintf(fichier,"No value for this attribute\n");
        fprintf(fichier,"indented\n");
        fprintf(fichier,"The following features are absent on this node: \n");
        fprintf(fichier,"We try to associate this node with: \n");
        fprintf(fichier,"Mismatch between this node and the rule\n");
        fprintf(fichier,"Rule element: \n");
        fprintf(fichier,"Alias already defined : \n");
        fprintf(fichier,"Unknown alias : \n");
        fprintf(fichier,"Error while reading the maximum iteration value for a rule element\n");
        fprintf(fichier,"surface\n");
        fprintf(fichier,"No test on values allowed here\n");
        fprintf(fichier,"The rule imposes that the node has the following feature values: \n");
        fprintf(fichier,"Error while reading alias:\n");
        fprintf(fichier,"locale\n");
        fprintf(fichier,"The exact form is : ..{ \n");
        fprintf(fichier,"Wrong Feature Definition : \n");
        fprintf(fichier,"Sequence Rule. Layer: \n");
        fprintf(fichier,"nodedisplay\n");
        fprintf(fichier,"  Cannot delete the only intepretation: \n");
        fprintf(fichier," we delete: \n");
        fprintf(fichier," Delete in a context. \n");
        fprintf(fichier,"uppercase\n");
        fprintf(fichier," Simplification. Layer: \n");
        fprintf(fichier,"tag\n");
        fprintf(fichier,"Before simplification.\n");
        fprintf(fichier,"Unknown TAG:\n");
        fprintf(fichier,"number\n");
        fprintf(fichier,"Missing number field\n");
        fprintf(fichier,"After simplification.\n");
        fprintf(fichier,"We replace the entry\n");
        fprintf(fichier,"translation\n");
        fprintf(fichier,"Missing '.' or '['\n");
        fprintf(fichier,"\n");
        fprintf(fichier,"\n");
        fprintf(fichier,"functions\n");
        fprintf(fichier,": unknown POS\n");
        fprintf(fichier,"Mismatch between the arguments of the function and its right-hand side\n");
        fprintf(fichier,"Extraction Succeeded\n");
        fprintf(fichier,"hidden\n");//196
        fprintf(fichier,"A rule cannot start with an optional POS\n");
        fprintf(fichier,"Missing size of the display matrix (X or Y)\n");
        fprintf(fichier,"Error, This layer already contains Disambiguation Rules: \n");
        fprintf(fichier,"Unknown Function: \n");
        fprintf(fichier,"extraction\n");
        fprintf(fichier,"sequence\n");
        fprintf(fichier,"Extraction Failed\n");
        fprintf(fichier,"Error... Right context to occur before the final '.' \n");
        fprintf(fichier,"Error... Left context does not apply\n");
        fprintf(fichier,"Error... Right context does not apply\n");
        fprintf(fichier,"Error on lemma field :\n");
        fprintf(fichier,"Error, This layer already contains another type of rule: \n");
        fprintf(fichier,"Marking rule. Failure\n");
        fprintf(fichier,"Error, Missing '}' \n");
        fprintf(fichier,"Error, Affectation rules are only available within a layer\n");
        fprintf(fichier,"Error, This layer already contains ID Rules: \n");
        fprintf(fichier,"Error, Missing layer number\n");
        fprintf(fichier,"Error, Mismatch LP-rules (no ID-rules to link to)\n");
        fprintf(fichier,"Error, Too many POS bear the feature: 'last' in this rule\n");
        fprintf(fichier,"Error, Too many POS bear the feature: 'first' in this rule\n");
        fprintf(fichier,"Error, Unkown field: \n");//217
        fprintf(fichier,"xmlcoding\n");//218
        fprintf(fichier,"Only one attribute at a time in this structure...\n");//219
        fprintf(fichier,"Only one dependency can be generated at a time in this contexte...\n");//220   
        fprintf(fichier,"doublereduction\n");//221
        fprintf(fichier,"Rule has applied\n");//222
        fprintf(fichier,"Failure on context application or on features comparison\n");//223  
        fprintf(fichier,"Error, missing field...\n");//224
        fprintf(fichier,"A variable $xxx is expected with the '^' operator.\n");//225
        fprintf(fichier,"Non valid index... Please rebuild the index.\n");//226
        fprintf(fichier,"The '\"' caracter should always be preceded by the escape caracter: \\\" \n");//227
        fprintf(fichier,"This rule deletes all readings from:\n");//228
        fprintf(fichier,". It cannot apply.\n");//229
        fprintf(fichier,"> Rule Failure:\n");//230
        fprintf(fichier," starting at node: \n");//231
        fprintf(fichier,"> Sequence Rule:\n");//232
        fprintf(fichier,"Error, infinite loop...\n");//233
        fprintf(fichier,"Extraction Rule: \n");//234
        fprintf(fichier,"Dependency Rule: \n");//235
        fprintf(fichier,"Success: \n");//236
        fprintf(fichier," on node:\n");//237
        fprintf(fichier,"Marking Rule: \n");//238
        fprintf(fichier,"Reshuffling Rule: \n");//239
        fprintf(fichier,"Application of Reshuffling rule: \n");//240
        fprintf(fichier,"keep\n");//241
        fclose(fichier);
        return 0;
}

const char* CHAINESEN[] = {
	    "semantic",//0
	    "terms",//1
        "reshuffling",//2
        "Failure on feature percolation",//3
        VERSION_XIP_BUILD,//4
        "Change with deduction rule",//5
        "Destruction with deduction rule",//6
        " yield: ",//7
        " with ",//8
        "Right-side of the rule does not match",//9
        "A test has already been written with IF",//10
        "A variable is needed to test features",//11
        "A file name should comprise a '.'",//12
        "Unknown Answer: ",//13
        "tagging",//14
        "Missing Field: 'NUMBER'",//15
        "Missing Field: 'DOUBLE'",//16
        "negation",//17
        "alluppercase",//18
        "rule",//19
        "rules",//20
        "idrules",//21
        "heads",//22
        "marker",//23
        "predfs",//24
        "prefcr",//25
        "alias",//26
        "dependencyrules",//27
        "Error: field unknown, expected value is: 'marker'",//28
        "Error: Rule should contain ';' or '&' or ')'",//29
        "Error: Rule should contain '('",//30
        "alias",//31
        "The variable in this function does not match any variables in the test",//32
        "Marking Rules should start with a layer number. ex: 1> ",//33
        "Error: Rule should contain '='",//34
        "Error: This rule should contain '{'",//35
        "Error: this character is invalid:",//36
        "Error: Default Rule should contain '&'",//37
        "",//38
        "Error: Default Rule should contain ',' or '}'",//39
        "default",//40
        "wordsize",//41
        "",//42
        "'=' expected",//43
        "definitions",//44
        "One value only for this definition",//45
        "Error: Rule should contain ',' or '='",//46
        "zones",//47
        "layerfilter",//48
        "split",//49
        "Only one POS permitted in the right-hand side of this disambiguation rule",//50
        "pos",//51
        " Feature Definition Required",//52
        "lprules",//53
        "Missing arrow",//54
        "Missing '('",//55
        "Error while reading a semantic rule...",//56
        "The last category cannot be a '?'",//57
        "Layer Filtering discard layer:",//58
        "The right-side of an affectation should be enclosed between {}",//59
        "Missing ')'",//60
        "Unknown alias",//61
        "No '(' here",//62
        "Feature Value required after a '='",//63
        "Warning: Multiple POS Affectation for a lexicon entry -> Defined POS is: ",//64
        "default",//65
        "There is no default rule for this feature",//66
        "Error, Longuest Match requires a cut ('!')",//67
        "Error: this feature has already been instantiated",//68
        "Errors in the right-side of that rule",//69
        " does not have a feature definition",//70
        "This layer has already been associated with a filter.",//71
        "Rule:",//72
        "Only a starting tag can be associated with this sort of features",//73
        "Too many doubles in this rule, increase the value of DOUBLES",//74
        "No rules",//75
        "A layer number must be given to that layer filter.",//76
        "Unknown Operator: ",//77
        "This extraction rule starts with a too complexe structure...",//78
        "Only one solution can be produced at a time, when a dependency is to be modified",//79
        ":We select the rule ",//80
        "We create ",//81
        ":Failure on feature comparison",//82
        ":Stop Searching",//83
        "Error: Feature Percolation",//84
        "Error:Absent Default Rule. Contexte:",//85
        "Error negation: ",//86
        "Error, the dependency should be enclosed between '<...>'",//87
        "Too many name definitions: Update the NUMBER field in your language.xip file",//88
        "Error reading zone",//89
        "functiondisplay",//90
        "Warning, only one function can be marked at a time",//91
        "Only a ';' can be used in this case",//92
        "Error. A '*' or a '+' is missing after the parentheses",//93
        "Error: Incorrect Code Name :",//94
        "yes",//95
        "no",//96
        "Unknown File: ",//97
        "no 'lexicons' field possible in this file",//98
        "boundaries",//99
        "trace",//100
        "tracebis",//101
        "number",//102
        "doubles",//103
        "Cannot utilize variable #0, affected by default to the word under disambiguation.",//104
        "Keyword 'where' expected",//105
        "indentation",//106
        "Layer number is in [1-1000]",//107
        "features",//108
        "lexicons",//109
        "vocabulary",//110
        "Missing '['",//111
        "Missing ',' or ']'",//112
        "Unknown feature ---> ",//113
        "display",//114
        "No display filter",//115
        "Missing ']'",//116
        "Missing '<'",//117
        " is an unknown value for ",//118
        " is not a possible value for ",//119
        "postdfs",//120
        "postfcr",//121
        "When no POS defined, use operator '+='",//122
        "Missing '='",//123
        " is an unknown attribute",//124
        "In an affectation: left variable with features or missing features for right variable",//125
        "variables in a test must be associated with features",//126
        "A variable in the right-side does not belong to the left-side.",//127
        "lemma",//128
        "Only node variables are accepted here (#1,#2 etc.)",//129
        "Use <features> to test on local feature definitions for a lexical entry",//130
        "Missing ',' or '}'",//131
        "Missing ',' or '.'",//132
        "Missing ',' or ')'",//133
        "Missing ',' or ']'",//134
        "Missing ':'",//135
        "Only one value is possible in the LEMMA field",//136
        "A value must be specified for each attribute in a LP rule.",//137
        "Unknown word --> ",//138
        "A rule cannot have an empty right-hand side.",//139
        "errors.err",//140
        "Too many POS",//141
        "Tree",//142
        "[c]",//143
        "Error when applying FCR:",//144
        "on ",//145
        "ID-rule. Layer: ",//146
        ":Base:",//147
        ":isolate:",//148
        ":searching again:",//149
        "Waiting ",//150
        "Increase the value of DOUBLES",//151
        "Error in file:",//152
        " line:",//153
        "No memory.",//154
        "LP-rule failure: ",//155
        "Time: ",//156
        "Missing starting character",//157
        "No value for this attribute",//158
        "indented",//159
        "The following features are absent on this node: ",//160
        "We try to associate this node with: ",//161
        "Mismatch between this node and the rule",//162
        "Rule element: ",//163
        "Alias already defined : ",//164
        "Unknown alias : ",//165
        "Error while reading the maximum iteration value for a rule element",//166
        "surface",//167
        "No test on values allowed here",//168
        "The rule imposes that the node has the following feature values: ",//169
        "Error while reading alias:",//170
        "locale",//171
        "The exact form is : ..{ ",//172
        "Wrong Feature Definition : ",//173
        "Sequence Rule. Layer: ",//174
        "nodedisplay",//175
        "  Cannot delete the only intepretation: ",//176
        " we delete: ",//177
        " Delete in a context. ",//178
        "uppercase",//179
        " Simplification. Layer: ",//180
        "tag",//181
        "Before simplification.",//182
        "Unknown TAG:",//183
        "number",//184
        "Missing number field",//185
        "After simplification.",//186
        "We replace the entry",//187
        "translation",//188
        "Missing '.' or '['",//189
        "Maximum number of rules in parameter file reached (Limited Mode)",//190
        "The XPath index name should be followed with '('",//191
        "functions",//192
        ": unknown POS",//193
        "Mismatch between the arguments of the function and its right-hand side",//194
        "Extraction Succeeded",//195
        "hidden",//196
        "A rule cannot start with an optional POS",//197
        "Missing size of the display matrix (X or Y)",//198
        "Error, This layer already contains Disambiguation Rules: ",//199
        "Unknown Function: ",//200
        "extraction",//201
        "sequence",//202
        "Extraction Failed",//203
        "Error... Right context to occur before the final '.' ",//204
        "Error... Left context does not apply",//205
        "Error... Right context does not apply",//206
        "Error on lemma field :",//207
        "Error, This layer already contains another type of rule: ",//208
        "Marking rule. Failure",//209
        "Error, Missing '}' ",//210
        "Error, Affectation rules are only available within a layer",//211
        "Error, This layer already contains ID Rules: ",//212
        "Error, Missing layer number",//213
        "Error, Mismatch LP-rules (no ID-rules to link to)",//214
        "Error, Too many POS bear the feature: 'last' in this rule",//215
        "Error, Too many POS bear the feature: 'first' in this rule",//216
        "Error, Unknown field: ",//217
        "xmlcoding",//218
        "Only one attribute at a time in this structure...",//219
        "Only one dependency can be generated at a time in this contexte...",//220
        "doublereduction",//221
        "Rule has applied",//222
        "Failure on context application or on features comparison",  //223
        "Error, missing field...",//224
        "A variable $xxx is expected with the '^' operator.",//225
        "Non valid index... Please rebuild the index.",//226
        "The '\"' caracter should always be preceded by the escape caracter: \\\" ",//227
        "This rule deletes all readings from:",//228
        ". It cannot apply.",//229
        "> Sequence Rule Failure:",//230
        " starting at node: ",//231
        "> Sequence Rule:",//232
        "Error, infinite loop...",//233
        "Extraction Rule: ",//234
        "Dependency Rule: ",//235
        "Success: ",//236
        " on node:",//237
        "Marking Rule: ",//238
        "Reshuffling Rule: ",//239
        "Application of Reshuffling rule: ",//240
        "keep",//241
        "This node in the reshuffling rule may cause the lost of words in the chunk tree",//242
        "Warning: Above sentence took too much time...",//243
        "Error: This attribute has never been declared before.",//244
        "Error: Only a node (#num) can be used here.",//245
        "Missing XML field.",//246
        "Error in XML formula.",//247
        "Unknown method.",//248
        "A handmade node cannot be assigned to a variable already used in the formula",//249
        "The DTD is not well-formed",//250
        "This field does not correspond to DTD XIP action",//251
        "This feature item should have at two possible values in its description field",//252
        "This procedure has already been declared",//253
        "A procedure cannot be the result of a deduction rule",//254
        "This procedure does not have any body",//255
        "'_goto' must be unique",//256
        "Label already used",//257
        "Parsing Space exhausted (number of loops to large, check your grammar)",//258
        "Parameter file does not exist",//259
        "Trace activated",//260
        "Grammar Loading",//261
        "PARSER INITIALISATION",//262
        "Cannot read parameter files in mode: limited...",//263
        "XIP expression should be enclosed between '!'",//264
        "Unknown action: ",//265
        "Unknown Default action",//266
        "Unknown function (possible types include: analyze,merge,feature or skip)",//267
        "Wrong number of argument for a feature transformation",//268
        "Missing parameter.\nPossible actions:\n\tskip=text|all\n\tanalyze=token|chunk|dependency\n\tfeature=string,string,(token|chunk|dependency).",//269
        "Unknown parameter for _display",//270
        "Missing '}' in a 'WHILE' loop",//271
        "The 'WHILE' is missing for that '}'",//272
        "This label does not exist",//273
        "You cannot affect a valeur preceded with a negation",//274
        "Error in your vocabulary definition: Missing parenthesis.",//275
        "Error in your vocabulary definition: Missing square bracket.",//276
        "Error in your vocabulary definition: The negation cannot precede a parenthesis).",//277
        "maxword",//278
        "The maximum number of words has been reached",//279
        "Missing ',' or ';' or '}' or '|' in this rule",//280
        "To use the '*' or the '+', you must first define the maximum category window: Cat#max",        //281
        "Negation can only applies to categories.",//282
        "The '?-' operator must be at the end of a rule.",//283
        "The comma is wrongly used in this regular expression",//284
        "A '*' or a '+' cannot apply here",//285
        "Mismatched parenthesis ()",//286
        "Mismatched square bracket []",//287
        "fragment",//288
        "Regular expressions variables should occur in increasing order 1,2,3..",//289
        "Only the instantiation sign ('=') is accepted in front of a XIP variable in a XML node creation...",//290
        "After a '|' we expect: 'if', 'iftest', 'compute' or '{'",//291
        "Error while compiling the dependency rule: ",//292
        "Unknown ontology concept",//293
        "This ontological path is incoherent",//294
        "ontologydisplay",//295
        "Error: The alias name is missing in this XML instruction.",//296
        "Error: A string or a vector should always start with a '_'.",//297
        "Error: An indice is needed for that vector.",//298
        "Error: This '}' does not match any 'for'.",//299
        "Error: A '}' is missing for a 'for' statement.",//300
        "Error: Wrong 'iftest' statement.",//301
        "Error: Mismatched '{...}'.",//302
        "Error: Unknown variable type...",//303
        "Mismatched curly bracket {}",//304
        "An XMLGuide expression should end with a '.' or a ';'",//305
        "A return should always terminate a procedure",//306
        "Unknown procedure or instruction:",//307
        "Unknown parameter type:",//308
        "This variable has already been declared:",//309
        "The lexicon rules should be preceded by a layer number in this file",//310
        "A node variable cannot be used within feature brackets [...]",//311
        "Impossible vector type",//312
        "A '.' is missing in this struct variable",//313
        "script",//314
        "The 'if' cannot be used within the script rules",//315
        "The 'iftest' cannot be used within the dependency rules",//316
        "No 'loop' instruction in a dependency rule section",//317
        "A dependency can only be created within the dependency rule section",//318
        "An instruction set '{...}' can only be used within a script section",//319
        "final",//320
        "Expecting a ',' or a ';'",//321
        "Warning. This dependency has already been declared:",//322
        "Unknown GRAMMAR file field:",//323
        "Error... This variable is already a category or a dependency or an instruction: ",//324
        "Error... Missing part in the formula",//325
        "Error... This version of XIP cannot handle XML expressions",//326
        "Error... Missing operator in the test",//327
        "loading",//328
        "initial",//329
        "Missing parenthesis in a #ifdef statement",//330
        "Missing #endif or #else for a #ifdef statement",//331
        "Missing #endif for a #else statement",//332
        "#else without #ifdef",//333
        "multiwords",//334
        "Error: This expression has already been declared",//335
        "Error: This string is too long",//336
        "Error: This rule is not correct. Check the commas and the parentheses",//337
        "January",//338
        "February",//339
        "March",//340
        "April",//341
        "May",//342
        "June",//343
        "July",//344
        "August",//345
        "September",//346
        "October",//347
        "November",//348
        "December",//349
        "Monday",//350
        "Tuesday",//351
        "Wednesday",//352
        "Thursday",//353
        "Friday",//354
        "Saturday",//355
        "Sunday",//356
        "utf8lexiconencoding",//357
        "Missing interface in a Python call",//358
        "There is no python function corresponding to the interface",//359
        "Unknown Python function",//360
        "Error. A fictive node is expected here:: ##CAT[features]",//361
        "Error. This variable cannot be used as a root. It does not point to a unique node.",//362
        "Warning... This Python module has already been loaded...",//363
		"Error: Code is superior to 255",//364
		"categorydefinition",//365
		"categorysplit",//366
		"Value already declared for that attribute",//367
		"Unknown charset: valid values are 'latin' or 'utf8'",//368
		"Missing layer number for this sort of rule.",//369
		"Missing or wrong right-side of reshuffling rule.",//370
		"Missing category definition in front of a '{'",//371
		"The right-side of a sequence rule cannot contain sub-nodes",//372
		"Unknown ontological expression",//373
		"Error... The '-' operator can only be used at the end of a sequence",//374
		"multilemmas",//375
		"Unknown space name root. Check that the first string of your space name matches the other spacename definitions.",//376
		"Format Instruction Unknown",//377
		"Missing closing comment character: '\\'",//378
		"The field 'guesserfeature' is missing in your language file. Language testing is impossible",//379
		"featuredefinition",//380
		"regularboundaries",//381
		"Missing closing quote: \"",//382
		"A '.' is missing in this multiword expression",//383
		"generation",//384
		"generatefeatures",//385
		"Incorrect GenerateFeatures rule",//386
		"Missing stream",//387
		"Missing Lemma",//388
		"Missing Features",//389
		"Missing '.' at the end of this dependency",//390
		"nodes",//391
		"Only NODES functions are valid as a result here",//392
		"Formulas are invalid in this context",//393
		"Wrong dependency declaration",//394
		"order",//395
		"Only NODES are valid in this section",//396
		"Only NODES can be associated to a $# variable",//397
		"generationdisplay",//398
		"Wrong probability file",//399
		"Probability File is corrupted",//400
		"Missing '.' at the end of this order rule",//401
		"sequenceoncontext",//402
        "@"};


