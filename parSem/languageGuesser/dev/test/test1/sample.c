/*
 Copyright (c) 1996 by the Xerox Corporation.  All rights reserved.
   Copyright protection claimed includes all forms and matters of
   copyrightable material and information now allowed by statutory or
   judicial law or hereinafter granted, including without limitation,
   material generated from the software programs which are displayed 
   on the screen such as icons, screen display looks, etc.
*/


#include <stdio.h>       
#include "guess_lib.h"


static const char *sentences[ ] = {
	 "He went on to the barn", "en",
	 "Ma guardiamo le liste che sono state presentate a Roma", "it",
	 "Major propondrá que los estados bálticos ingresen en la CE", "es",
	 "Förutsättningar för skolans arbete har förändrats kraftig", "sv",
	 "Damit ist der Rahmen äußerst eng gesteckt", "de",
	 "Dikwijls heeft dit praktische redenen", "nl",
	 "Maamme Oi maamme Suomi synnyinmaa soi sana kultainen", "fi",
	 "Ingemann Fjorten Eventyr og FortællingerB", "da",
	 "Ou alors il ne mérite pas d'exister au seul détriment du consommateur", "fr",
	 "Jens og Mathilda står i pøsregnet og hesjar då han kjem der", "no",
	 "Nõs. Felesége pedagógus. Két lánygyermeke van", "hu",
	 "Vitame te z Ameriky a cekame te v Jevanech, objev se co nejdriv", "cs",
	 "São Bartolomeu abriu seu saco de ventos em cima dos ermos", "pt",
	 0 };

int main () {
    char res[20];
    int max_lang, ind1, ind2;
    char *lang;
    guesser_id_type    GuessType = trig_shortword;
    guesser_lang_type  LangType  = guesser_iso639;

    max_lang = init_guesser();

    for(ind1=0; sentences[ind1] != 0; ind1++) {
        char *str = sentences[ind1];
	ind1++;
        lang = sentences[ind1];
        //puts(lang);                     
        guess_language(str, res,  LangType, GuessType);
        //puts(res);                   
    	for(ind2=0;  res[ind2] != 0; ind2++) {
		if (res[ind2] != lang[ind2]) { 
			printf("\n\n----------\nError: for the sentence \"%s\" the answer is \"%s\" rather \"%s\"\n----------\n",str,res,lang);
			exit(-1);
		}
	}
	if (lang[ind2] != 0) {
		/* lang is longuer than res */
			printf("\n\n----------\nError: for the sentence \"%s\" the answer is \"%s\" rather \"%s\"\n----------\n",str,res,lang);
		exit(-1);
	} 
	printf("test OK: for the sentence \"%s\" the answer is \"%s\"\n",str,res);
    }
    exit(0);
}

