#ifdef NTMCONNECTION

extern "C" {
#include "fst/context.h"
#include "fst/cfsm.h"
#include "applyloc.h"
}


#include "fst/modify.h"
#include "fst/bin-in.h"
#include "fst/pars-tbl.h"
#include "lktypes.h"
#include "ems.h"
#include "lookup.h"
    
#include "networks.h"

#include "tok-aux.h"
#include "text-io.h"
#include "bin-out.h"
#endif

#include "parsbin.h"
#include "osdir.h"

#ifdef NTMCONNECTION
#include "ntmhmm.h"
#include "maintok.h"
#endif 

#include "varlibxml.h"
#include "kifn.h"
#include "kif.h"
#include "xipkif.h"
#include "conversion.h"
#include "graphe.h"

extern VECTA<GlobalParseur*> gVecteurParseurs;
Exported int XipParseStringAndOS(string texte,int ipar, XipResult* xip,ostringstream* os,char xml,char applique_grammaire);

char parentde(ResultatFonction* fille,ResultatFonction* parent);
ResultatFonction* suivantde(ResultatFonction* courant,ResultatFonction* suivant,int increment);
ResultatFonction* perede(ResultatFonction* courant,ResultatFonction* suivant);
ResultatFonction* filsde(ResultatFonction* courant,ResultatFonction* suivant,char dernier);
ResultatFonction* tokensuivant(GlobalParseur*,ResultatFonction* courant,ResultatFonction* suivant,int pos);
