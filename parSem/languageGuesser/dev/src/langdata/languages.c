/*******************************************************************
|***
|***         File: languages.c
|***       Author: Bruno Maximilian Schulze
|***      Purpose: language definitions
|***      Created:
|***     Modified:
|***    Copyright: Xerox Corporation
|***
\*******************************************************************/
/* Copyright (c) 1996-97 by the Xerox Corporation.  All rights reserved.
   Copyright protection claimed includes all forms and matters of
   copyrightable material and information now allowed by statutory or
   judicial law or hereinafter granted, including without limitation,
   material generated from the software programs which are displayed 
   on the screen such as icons, screen display looks, etc.
   */
#include <stdio.h>

#include "languages.h"
#include "langs.h"


lang_tbl languages[] = {
    { "cs", "cze", "Czech",          1, NULL, NULL, NULL, NULL, init_cze  },
    { "da", "dan", "Danish_iso1",    1, NULL, NULL, NULL, NULL, init_dan  },
    { "da", "dau", "Danish_utf8",    1, NULL, NULL, NULL, NULL, init_dau  },
    { "nl", "dut", "Dutch",          1, NULL, NULL, NULL, NULL, init_dut  },
    { "en", "eng", "English",        1, NULL, NULL, NULL, NULL, init_eng  },
    { "fi", "fin", "Finnish_iso1",   1, NULL, NULL, NULL, NULL, init_fin  },
    { "fi", "fiu", "Finnish_utf8",   1, NULL, NULL, NULL, NULL, init_fiu  },
    { "fr", "fre", "French_iso1",    1, NULL, NULL, NULL, NULL, init_fre  },
    { "frU", "freU", "French",       1, NULL, NULL, NULL, NULL, init_freU },
    { "fr", "fru", "French_utf8",    1, NULL, NULL, NULL, NULL, init_fru  },
    { "de", "ger", "German_iso1",    1, NULL, NULL, NULL, NULL, init_ger  },
    { "de", "gmu", "German_utf8",    1, NULL, NULL, NULL, NULL, init_gmu  },
    { "el", "gre", "Greek_iso7",     1, NULL, NULL, NULL, NULL, init_gre  },
    { "el", "gru", "Greek_UTF8",     1, NULL, NULL, NULL, NULL, init_gru  },
    { "hu", "hun", "Hungarian_iso2", 1, NULL, NULL, NULL, NULL, init_hun  },
    { "hu", "huu", "Hungarian_UTF8", 1, NULL, NULL, NULL, NULL, init_huu  },
    { "it", "ita", "Italian_iso1",   1, NULL, NULL, NULL, NULL, init_ita  },
    { "itU", "itaU", "Italian",      1, NULL, NULL, NULL, NULL, init_itaU },
    { "it", "itu", "Italian_utf8",   1, NULL, NULL, NULL, NULL, init_itu  },
    { "no", "nor", "Norwegian_iso1", 1, NULL, NULL, NULL, NULL, init_nor  },
    { "no", "nou", "Norwegian_utf8", 1, NULL, NULL, NULL, NULL, init_nou  },
    { "pl", "plu", "Polish_utf8",    1, NULL, NULL, NULL, NULL, init_plu  },
    { "pl", "pol", "Polish_ascii",   1, NULL, NULL, NULL, NULL, init_pol  },
    { "pl", "pow", "Polish_cp1250",   1, NULL, NULL, NULL, NULL, init_pow  },
    { "pt", "por", "Portuguese_iso1",1, NULL, NULL, NULL, NULL, init_por  },
    { "ptU", "porU", "Portuguese",   1, NULL, NULL, NULL, NULL, init_porU },
    { "pt", "pou", "Portuguese_utf8", 1, NULL, NULL, NULL, NULL, init_pou  },
    { "ru", "rui", "Russian_iso5",   1, NULL, NULL, NULL, NULL, init_rui },
    { "ru", "ruk", "Russian_koi8r",  1, NULL, NULL, NULL, NULL, init_ruk },
    { "ru", "ruu", "Russian_utf8",   1, NULL, NULL, NULL, NULL, init_ruu },
    { "ru", "ruw", "Russian_cp1251", 1, NULL, NULL, NULL, NULL, init_ruw },   
    { "es", "spa", "Spanish_iso1",   1, NULL, NULL, NULL, NULL, init_spa  },
    { "esU", "spaU", "Spanish",      1, NULL, NULL, NULL, NULL, init_spaU },
    { "es", "spu", "Spanish_utf8",   1, NULL, NULL, NULL, NULL, init_spu  },   
    { "sv", "swe", "Swedish_iso1",   1, NULL, NULL, NULL, NULL, init_swe  },
    { "sv", "swu", "Swedish_utf8",   1, NULL, NULL, NULL, NULL, init_swu  },
   
/* must always be the last */
    { "??", "???", "Unknown",        1, NULL, NULL, NULL, NULL, NULL }
};

const int _num_langs = sizeof(languages) / sizeof (languages[0]) - 1;

