/* (c) 1996-97 Xerox Corp. */

#include <stdlib.h>
#include <stdio.h>

#include "languages.h"
#include "string2id.h"

#define lak_MAX_TRIGs 0

static const langhash_t lak_hash[lak_MAX_TRIGs] = {

};

static const wordhash_t lak_wordhash[] = {
  { "wa", -4.158880 }  , { "to", -4.158880 }  , { "wi", -4.564350 }  , { "tan", -4.564350 }  , { "sa", -4.564350 }  , { "zi", -5.257500 }  , { "zap", -5.257500 }  , { "yetu", -5.257500 }  , { "yan", -5.257500 }  , { "yam", -5.257500 }  , { "win", -5.257500 }  , { "wig", -5.257500 }  , { "wica", -5.257500 }  , { "waste", -5.257500 }  , { "wam", -5.257500 }  , { "te", -5.257500 }  , { "ta", -5.257500 }  , { "suton", -5.257500 }  , { "su", -5.257500 }  , { "sta", -5.257500 }  , { "sna", -5.257500 }  , { "sku", -5.257500 }  , { "si", -5.257500 }  , { "popa", -5.257500 }  , { "pi", -5.257500 }  , { "pe", -5.257500 }  , { "pcun", -5.257500 }  , { "ogna", -5.257500 }  , { "oce", -5.257500 }  , { "niyan", -5.257500 }  , { "mna", -5.257500 }  , { "mi", -5.257500 }  , { "ma", -5.257500 }  , { "la", -5.257500 }  , { "ko", -5.257500 }  , { "ki", -5.257500 }  , { "ke", -5.257500 }  , { "kce", -5.257500 }  , { "kan", -5.257500 }  , { "kaga", -5.257500 }  , { "ka", -5.257500 }  , { "hpe", -5.257500 }  , { "hoksi", -5.257500 }  , { "hin", -5.257500 }  , { "he", -5.257500 }  , { "han", -5.257500 }  , { "guya", -5.257500 }  , { "gluta", -5.257500 }  , { "glo", -5.257500 }  , { "gi", -5.257500 }  , { "ge", -5.257500 }  , { "cokan", -5.257500 }  , { "ci", -5.257500 }  , { "ce", -5.257500 }  , { "blo", -5.257500 }
};

int
init_lak(int lgno) {

    register int  i;
    unsigned long cid;

    if (lak_MAX_TRIGs) {
        languages[lgno].dict = make_dict(2 * lak_MAX_TRIGs, 0);
        languages[lgno].hash = (double *)malloc(sizeof(double) * lak_MAX_TRIGs);
        if (!languages[lgno].hash) return(1);
        for (i = 0; i < lak_MAX_TRIGs; i++) {
           cid = string2id((char *)lak_hash[i].trig, languages[lgno].dict, insert_copy);
           if (cid == ~0) return(1);
           languages[lgno].hash[cid] = lak_hash[i].prob;
        }
        languages[lgno].wdict = make_dict(2 * 55, 0);
        languages[lgno].whash = (double *)malloc(sizeof(double) * 55);
        for ( i = 0; i < 54; i++) {
           cid = string2id((char *)lak_wordhash[i].word, languages[lgno].wdict, insert_copy);
           if (cid == ~0) return(1);
           languages[lgno].whash[cid] = lak_wordhash[i].prob;
        }
    } else languages[lgno].used = 0;

    return(0);
}
