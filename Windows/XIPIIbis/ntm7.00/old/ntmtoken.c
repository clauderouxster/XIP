/* $Id: ntmtoken.c,v 1.9 2004/05/14 09:47:00 ait Exp $ */ 


#include "ntmtoken.h"
#include "ntmlevels.h"
#include "ntm.h"
#include "label.h"
#include <string.h>

#ifdef _WINDOWS
#  define snprintf _snprintf
#endif

#define ATOM_NAME_LENGTH 24


/************* 
 Tokens 
*************/

NtmToken lastToken = { 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL};


/* return this dummy token that represents the end of file
 */
inline NtmToken *getLastToken() {
    return &lastToken;
}


/* return 1 iff unknown token
 */
inline int ntmIsUnknownToken(NtmToken *token) {
    return (token->strategy == NTM_LAST_STRATEGY);
}



/* return 1 iff end of file
 */
inline int ntmIsEofToken(NtmToken *token) {
    return (token == getLastToken());
}


inline void tokenSetIgnore(NtmToken *token) {token->ignore = 1;}
inline void tokenSetWeight(NtmToken *token, int w) {token->weight = w;} 
inline void tokenSetTambig(NtmToken *token) {token->tambig = 1;} 
inline void tokenSetStrategy(NtmToken *token, int i) {token->strategy = i;} 

/* adds an entry (form, lemma, features) to token t
   !! assume all added entries are contiguous in the Entries array !!
 */
inline void
addEntryToToken( NtmToken *token,
                 NtmEntry *e
    ) {

    if ( (e == NULL) || (token == NULL) ) return;

    if ( token->entryNb == 0 ) {
        /* first entry for token t */
        token->entrySeq = e;
    }
    token->entryNb++;
}


/* print token into string s
******************************/

/****** utility
//char * idtostr(id_type id) {
//    char idstring[NTM_SYMBOL_MAXSIZE];
//    FST_fat_to_thin( id_to_atom_name(id), idstring);
//    printf("|%s|\n",idstring);
//    return idstring;
//}

*/


/* prints an id seq to char string s, with max length = maxlg
   returns the number of printed chars
**************************************************************/


inline int ntmSnprintIdSeq(char *s, int maxlg, id_type *idseq) {
    char idstring[NTM_SYMBOL_MAXSIZE];
    int ind=0, i=0;
    if (idseq != NULL) {
        for(; *idseq != ID_NO_SYMBOL ; idseq++) {
            FST_fat_to_thin( id_to_atom_name(*idseq), idstring);
            i = 0;
            while ( (idstring[i] != 0) && (ind < maxlg) ) {
                s[ind++] = idstring[i++];
            }
        }
    }
    s[ind] = 0;
    return ind;
}


inline int snprintInputFormUTF8(Levels* lev, char *s, int maxlg, NtmToken* token) {
    int i=0, ind=0; char c=0;
	int id;
    if ( token->inputLength <  (maxlg-5) ) {
        for(i=token->initpos;
            i < (token->inputLength+token->initpos); i++) {
            id = getInputCharId(lev, i);
			ind += FST_fat_to_thin_name(s+ind, LABEL_MAP->map[id]->content.atom, maxlg - ind ,0);
			s[ind]=0;
			}
	}
    return strlen(s);
}


inline int snprintInputForm(Levels* lev, char *s, int maxlg, NtmToken* token) {
    int i=0, ind=0; char c=0;
	int id;
    if ( token->inputLength <  (maxlg-5) ) {
        for(i=token->initpos;
            i < (token->inputLength+token->initpos); i++) {
            id = getInputCharId(lev, i);
			ind += FST_fat_to_thin_name(s+ind, LABEL_MAP->map[id]->content.atom, maxlg - ind ,0);
			s[ind]=0;
			}
	}
    return strlen(s);
}


inline int ntmPrintTokenTxt(NtmContext* ct, NtmToken* token, char *s, int maxlg) {

    int ind=0, i=0;
    NtmEntry *e=NULL;


    if (ct->printIndices || ct->printByteIndices ) {
        if (ct->printIndices && !ct->printByteIndices) {
            ind += snprintf(s, maxlg-ind, "%d-%d\n",
                       token->initpos, token->initpos+token->inputLength);
        } else if (ct->printIndices) {
            ind += snprintf(s, maxlg-ind, "%d-%d",
                       token->initpos, token->initpos+token->inputLength);
        }
        if (ct->printByteIndices) {
            ind += snprintf(s+ind, maxlg-ind, "[%d-%d]\n",
                       token->initBytepos, token->initBytepos+token->inputByteLength);
        }
        
    }

    for(i=0, e = token->entrySeq; i < token->entryNb; i++, e++) {

        ind += snprintInputForm(&ct->levels, s+ind, maxlg-ind-5, token);
        /* ind += ntmSnprintIdSeq(s+ind, maxlg-ind, e->normform); */
        if (ind < maxlg) s[ind++] = '\t';
        ind += ntmSnprintIdSeq(s+ind, maxlg-ind-5, e->lemma);
        if (ind < maxlg) s[ind++] = '\t';
        ind += ntmSnprintIdSeq(s+ind, maxlg-ind-5, e->features);
        if (ind < maxlg) s[ind++] = '\n';
    }
    if (ind < maxlg) s[ind++] = '\n';
    s[ind] = '\0';
    return ind;
}

/* XML XML XML
   print one token in XML format */

inline int ntmSnprintFeaturesXml(char *s, int maxlg, id_type *idseq) {
    char idstring[NTM_SYMBOL_MAXSIZE];
    int ind=0;
    ind += snprintf(s+ind, maxlg-ind, "<FEATURES>\n");
    if (idseq != NULL) {
        for(; *idseq != ID_NO_SYMBOL ; idseq++) {
            FST_fat_to_thin( id_to_atom_name(*idseq), idstring);
            ind += snprintf(s+ind, maxlg-ind,
                            "<FEATURE name=\"%s\"/> ", idstring);
        }
    }
    ind += snprintf(s+ind, maxlg-ind, "\n</FEATURES>\n");
    s[ind] = 0;
    return ind;
}  

inline int ntmPrintTokenXml(NtmContext* ct, NtmToken* token, char *s, int maxlg) {

    int ind=0, i=0;
    NtmEntry *e=NULL;


    ind += snprintf(s, maxlg-ind, "<TOKEN");
    if (ct->printIndices) {
        ind += snprintf(s+ind, maxlg-ind, " start=%d end=%d",
                        token->initpos, token->initpos+token->inputLength);
    }
    if (ct->printByteIndices) {
        ind += snprintf(s+ind, maxlg-ind, " bytestart=%d byteend=%d",
                        token->initBytepos, token->initBytepos+token->inputByteLength);
    }
    ind += snprintf(s+ind, maxlg-ind, ">\n"); 
    ind += snprintInputForm(&ct->levels, s+ind, maxlg-ind-5, token);

    for(i=0, e = token->entrySeq; i < token->entryNb; i++, e++) {
        ind += snprintf(s+ind, maxlg-ind, "\n<READING> <LEMMA>");
        /* ind += ntmSnprintIdSeq(s+ind, maxlg-ind, e->normform); */
        ind += ntmSnprintIdSeq(s+ind, maxlg-ind-5, e->lemma);
        ind += snprintf(s+ind, maxlg-ind, "</LEMMA>\n");
        ind += ntmSnprintFeaturesXml(s+ind, maxlg-ind-5, e->features);
        ind += snprintf(s+ind, maxlg-ind, "</READING>");
    }
    ind += snprintf(s+ind, maxlg-ind, "\n</TOKEN>\n");
    s[ind] = '\0';
    return ind;
}



/* Printing a token : simple or XML format */
inline int ntmPrintToken(NtmContext* ct, NtmToken* token, char *s, int maxlg) {
    if (ct->xmlOutput)
        return ntmPrintTokenXml(ct, token, s, maxlg);
    else
        return ntmPrintTokenTxt(ct, token, s, maxlg);        
}






