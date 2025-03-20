/* $Id: ntmload.c,v 1.9 2004/03/25 13:36:54 ait Exp $ */ 


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#include "types.h"
#include "struct.h"
#include "vectorize.h"
#include "pars-tbl.h"
#include "bin-in.h"

#include "ntm.h"
#include "ntmextern.h"

#include <string.h>

#ifdef _WINDOWS
#  define snprintf _snprintf
#  include <ctype.h>
#endif





/* Vectorizes dense states of the n first nets in 'nets'
******************************************************/

void vectorizeNets(NtmContext *ct, NtmNetStruct *nets, int n) {
    int i=0;
    for(i=0; i < n; i++) {
        vectorize_dense_states_of_net(nets[i].net, ct->vectorizeNumber);
    }
}

/* Makes range_maps for nets
*****************************/
void makeRangeMaps(NtmNetStruct *nets, int n) {
    int i=0;
    update_match_tables();
    for(i=0; i < n; i++) {
        make_range_map(nets[i].net, LOWER);
    }
}

/* Load FSTs from a file and put them in 'nets'. Returns the
   number of loaded nets, or exits if any problem with the file
   or if the file contains more than 'max' nets
**************************************************************/
NETptr loadNet(NtmContext *ct, char *filename) {

    NVptr nv;
    FILE *filestream = fopen(filename, "rb");

    if (filestream != NULL) {
        /* file exists and is readable */
        if (std_file_header_p(filestream)) {
            nv = (NVptr) networks_from_file(filestream);
        } else {
			fclose(filestream);
            errorHandler(ct, "'%s' is not a well-formed network", filename);
            return NULL;
        }
    } else {
        errorHandler(ct, "Unable to read '%s'", filename);
        return NULL;
    }

	fclose(filestream);

    if (NV_len(nv) == 0) {
        errorHandler(ct, "No net in '%s'", filename);
        return NULL;
    } else {
		NETptr temp = NV_net(nv, 0);
		free_nv_only(nv);
        return temp;
    }
}




/* Load the lex nets and vectorize them
****************************************/
void ntmLoadAndPrepareNets(NtmContext *ct, NtmNetStruct *nets, int netsNb) {
    int i=0;


    /* load all the nets */
    for(i=0; i < netsNb; i++) {
        fprintf(stderr, "Loading %s=%s\n",
                nets[i].alias,
                nets[i].filename);
        nets[i].net = loadNet(ct, nets[i].filename);
        if (nets[i].net == NULL) return;
    }
    fprintf(stderr, "\n");

    /* prepare range_maps */
    makeRangeMaps(nets, netsNb);
    /* vectorize the nets */
    vectorizeNets(ct, nets, netsNb);

    update_match_tables();

}


/* Replace an existing, loaded network, by a new one specified by
   the given file name. The existing network is removed, even though
   this operation does not free any memory from the current process.
   (the corresponding memory is made available on the heap)
****************************************************************/
void ntmReplaceNet(NtmContext* ct, NtmNetStruct *nets, int netnum, char *filename) {
		
	/* free alocated memory */
	/* WARNING: the c-fsm library allows unloading of networks, but 
	   no memory is actually freed */
	free_network(nets[netnum].net);

	/* Rename network */
	if (strlen(filename)<NTM_FILENAME_SIZE) {
		strcpy(nets[netnum].filename,filename);
	} else nets[netnum].filename[0]=0;
    /* The network alias is not updated since we have no information about it /

	/* Reload network */
	fprintf(stderr, "Loading %s=%s\n",
			nets[netnum].alias,
			nets[netnum].filename);
	nets[netnum].net = (NETptr) loadNet(ct, filename);

	/* c-fsm updates */
	make_range_map(nets[netnum].net, LOWER);
	vectorize_dense_states_of_net(nets[netnum].net, ct->vectorizeNumber);
    update_match_tables();
}



/* Update a finite state network with a newer version from disk 
   'netnum' is the actual position of the network to update in the NtmNetStruct[] table. 
   This function simply calls ntmReplaceNet() with the current
   filename of the network to reload.
****************************************************************/
void ntmReloadNet(NtmContext* ct, NtmNetStruct *nets, int netnum) {
	ntmReplaceNet(ct, nets, netnum, nets[netnum].filename);
}



/* Load a new network from disk after a prior initialization of other networks.
   The description (i.e. filename, alias) of the new net must be 
   defined in the last position of the 'nets' array
   'netsNb' should be the total number of networks (including the new one)
   In order to load a network somewhere else in the array, use ntmReplaceNet() instead.
****************************************************************/
void ntmLoadNewNetLast(NtmContext* ct, NtmNetStruct *nets, int netsNb) {
	
	/* Load network */
	fprintf(stderr, "Loading %s=%s\n",
			nets[netsNb-1].alias,
			nets[netsNb-1].filename);
	nets[netsNb-1].net = (NETptr) loadNet(ct, nets[netsNb-1].filename);

	/* c-fsm updates */
	make_range_map(nets[netsNb-1].net, LOWER);
	vectorize_dense_states_of_net(nets[netsNb-1].net, ct->vectorizeNumber);
    update_match_tables();
}


/* Load a new network from disk after a prior initialization of other networks.
   'nets' is the current NtmNetStruct array
   'currentSize' is the number of networks loaded in 'nets'
   'filename' is the name of the new network to load.
   The function returns the new NtmNetStruct array containing all networks.
   This function actually calls the simpler ntmLoadNewNet() function above. 
   It is simply a convenience to avoid reallocation of the new NtmNetStruct array.
****************************************************************/
NtmNetStruct *ntmLoadNewNet(NtmContext* ct, NtmNetStruct *nets, int currentSize, char *filename) {
	int i;
	NtmNetStruct *nets2;

	/* Create new NtmNetStruct array */
	nets2 = (NtmNetStruct *)malloc((currentSize+1)*sizeof(NtmNetStruct));
	for (i=0; i< currentSize; i++) { 
		nets2[i] = nets[i];
	}
	free(nets);

	if (strlen(filename)<NTM_FILENAME_SIZE) {
		strcpy(nets2[currentSize].filename,filename);
		nets2[currentSize].alias[0]=0;
	} else {
        warningHandler(ct, "ntmLoadNewNet: Filename too long");
		return nets;
	}

	/* Actually load the net */
	ntmLoadNewNetLast(ct, nets2,currentSize+1);

	return nets2;
}



/* read the next non-space string or the next newline string
   returns s (the string) if ok, or NULL if EOF
*************************************************************/
#define METACHAR '%'
#define COMMENTCHAR '#'

inline unsigned char *fgetString(unsigned char *s, int n, FILE *in) {
    int c=0, i=0;

    if ( n <= 1) {
        return NULL;
    }

    /* skip spaces and/or comments*/
    for (c=fgetc(in); (c!=EOF) && (isspace(c)); c=fgetc(in)) {
        if (c == '\n') {
            s[0] = '\n';
            s[1] = 0;
            return s;
        }
    }
   
    if ( c==EOF ) {
        return NULL;
    }
        
    s[i++]= (unsigned char) c;
    c=fgetc(in);
    while ( (i < n) && (c!=EOF) && !isspace(c) ) {
        s[i++]= (unsigned char) c;   
        c=fgetc(in);
    }
    s[i] = 0;
    ungetc(c, in);
    return s;
}

/* small utilities
*******************/

inline int isnewline(unsigned char *s) {
    return ( (s[0] == '\n') && (s[1]==0) );
}

inline int isnumeric(unsigned char c) {
    return ( (c >= '0') || (c < '9') );
}

inline int getValueFromChar(unsigned char c) {
    if ( isnumeric(c) ) {
        return (c - '0');
    } else return -1;
}

inline int getValueFromString(unsigned char *s) {
    int i=0, v=0;
    for(i=0; s[i]; i++) {
        if ( isnumeric(s[i]) ) {
                v = v*10 + getValueFromChar(s[i]);
        } else {
            return -1;
        }
    }
    return v;
}

inline int getCharDecimalCode(unsigned char *s) {
    if (s[0] == '\\') {
        if (s[2] == 0) {
            return (int) s[1];
        } else {
            return getValueFromString(s+1);
        }
    } else {
        if (s[1] != 0) {
            return -1;
        } else {
            return (int) s[0];
        }
    }
}

void getAbsoluteFilename(NtmContext* ct, unsigned char *s,
                         unsigned char *abs) {

    char var[NTM_FILENAME_SIZE], *value;
    int i=0;

    if (s == NULL) return;

    while (*s) {
        for(; (*s != 0) && (*s != '$'); *abs++=*s++);
        if (*s == '$') {
            for(i=0, s++; *s && (*s != '/'); var[i++] = *s++);
            var[i] = '\0';
            if (value = getenv(var)) {
                for(; *value; *abs++ = *value++, abs);
            } else {
                errorHandler(ct, "Environment variable not defined: %s", var);
                return;
            }
        }
    }
    *abs = '\0';
}

/* read a lookup strategy from a script file
*********************************************/


NETptr getNetPtr(NtmNetStruct *nets, int netsNb, unsigned char *alias) {
    int i=0;
    for(i=0; i < netsNb; i++) {
        if (!strcmp(nets[i].alias, (char *) alias))
            return nets[i].net;
    }
    return NULL;
}


/* READS PARAMETERS FROM A SCRIPTFILE
**************************************/

void loadStrategyScript(NtmContext* ct, char *filename) {

    unsigned char s[NTM_FILENAME_SIZE], absfilename[NTM_FILENAME_SIZE];
    int ind=0, code=0, classes_reset=0;
    FILE *inscript= fopen(filename, "r");
    NtmNetStruct nets[NTM_NETS_MAXNB];
    int netsNb=0;
    NETptr normnet=NULL, lexnet=NULL;
    enum {SPACES, END_SPACES, SEPARATORS, END_SEPARATORS, ALIAS,
          FILENAME, NEWLINE, NORMNET, LEXINET} expected;

    if (inscript == NULL) {
        errorHandler(ct, "Unable to read '%s'", filename);
        return;
    } else {
        /* first part : SPACES & SEPARATORS */
        expected = ALIAS;
        while ( fgetString(s, NTM_FILENAME_SIZE, inscript ) ) {
            if (isnewline(s)) {
                if (expected == SPACES) {
                    expected = END_SPACES;
                } else if (expected == SEPARATORS) {
                    expected = END_SEPARATORS;
                } else if (expected == END_SPACES) {
                    expected = SEPARATORS;
                } else if (expected == END_SEPARATORS) {
                    expected = ALIAS;
                    break;
                }
            } else if (!strcmp( (char *) s, "SPACES:")) {
                if (!classes_reset) { /* no spaces/sep were specified yet*/
                    ntmUndefineSpaces(ct);
                    ntmUndefineSeparators(ct);
                    classes_reset = 1;
                }
                expected = SPACES;
            } else if (!strcmp( (char *) s, "SEPARATORS:")) {
                if (!classes_reset) { /* no spaces/sep were specified yet*/
                    ntmUndefineSpaces(ct);
                    ntmUndefineSeparators(ct);
                    classes_reset = 1;
                }
                expected = SEPARATORS;
            } else if (expected == ALIAS) {
                strcpy(nets[netsNb].alias, (char *) s);
                expected = FILENAME;
                break;
            } else {
                code = getCharDecimalCode(s);
                if ( (code < 0) || (code >= NTM_ID_SIZE) ) {
                    errorHandler(ct,"Wrong character code '%s'", s);
                    return;
                } else {
                    ntmSetSeparator(ct, code);
                    if ( (expected == SPACES) ||
                         (expected == END_SPACES) ) {
                        expected = SPACES;
                        ntmSetSpace(ct, code);
                    } else {
                        expected = SEPARATORS;
                    }
                }
            }
        }

        /* read second part: alias and file definitions */
        while ( fgetString(s, NTM_FILENAME_SIZE, inscript ) ) {

            if (isnewline(s)) {
                if (expected == NEWLINE) {
                    /* one newline ! next should be an alias or an NL */
                    expected = ALIAS;

                } else if (expected == FILENAME) {
                    /* first newline */
                     errorHandler(ct, "script: filename expected!");
                     return;
                } else if (expected == ALIAS) {
                    /* second newline !! go to next part of the script */
                    break;
                }
            } else {

                if (expected == ALIAS) {
                    /* an alias to be defined */
                    strcpy(nets[netsNb].alias, (char *) s);
                    expected = FILENAME;

                } else if (expected == FILENAME) {
                    /* a filename for defining an alias */
                    getAbsoluteFilename(ct, s, absfilename);
                    if (ntmError(ct)) return;
                    strcpy(nets[netsNb].filename, (char *) absfilename);
                    netsNb++;
                    expected = NEWLINE;

                } else if (expected == NEWLINE) {
                    errorHandler(ct, "script: expected a newline instead of\
 '%s'", s);
                    return;
                }
            }
        }

        
        ntmLoadAndPrepareNets(ct, nets, netsNb);
        if (ntmError(ct)) return;
        /* read 2nd part of the script (strategy) */
        expected = NORMNET;
        while ( fgetString(s, NTM_FILENAME_SIZE, inscript ) ) {
            if (isnewline(s)) { /* newline */
                if (expected == NEWLINE) {
                    expected = NORMNET;
                } else if (expected == NORMNET) {
                    /* second newline : assume end of the script... */
                    break;
                } else if (expected == LEXINET) {
                    /* this strategy line has only ONE net alias
                     rather, consider this unique net as a lexicon net */
                    ntmAddStrategy(ct, NULL, normnet);
                    expected = NORMNET;
                }
            } else { /* a string */
                if (expected == NORMNET) {
                    /* a typo net for a strategy */
                    if ( (normnet = getNetPtr(nets, netsNb, s)) == NULL) {
                        errorHandler(ct, "script: undefined alias '%s'", s);
                        return;
                    }
                    expected = LEXINET;

                } else if (expected == LEXINET) {
                    /* a lex for a strategy */
                    if ((lexnet = getNetPtr(nets, netsNb, s)) == NULL ) {
                        errorHandler(ct, "script: undefined alias '%s'", s);
                        return;
                    }
                    ntmAddStrategy(ct, normnet, lexnet);
                    expected = NEWLINE;
                } else if (expected == NEWLINE) {
                    errorHandler(ct, "script: expected a newline instead of\
 '%s'", s);
                    return;
                }
            }
        }
    }
    fclose(inscript);
    if ( ct->strategyNb == 0 ) {
        errorHandler(ct, "empty strategy!");
    }

}

/* LOADS strategy nets <norm lex> FROM DISK and adds
   it to the NTM context
************************************************/

void ntmLoadStrategy(NtmContext* ct, char* normfile, char*lexfile) {
    NtmNetStruct nets[NTM_NETS_MAXNB];
    strcpy(nets[0].filename, lexfile);
    nets[0].alias[0]=0;
    if (normfile[0]) { /* normfile is optional */
        strcpy(nets[1].filename, normfile);
        nets[1].alias[0]=0;
        ntmLoadAndPrepareNets(ct, nets, 2);
        ntmAddStrategy(ct, nets[1].net, nets[0].net);
    } else {
        ntmLoadAndPrepareNets(ct, nets, 1);
        ntmAddStrategy(ct, NULL, nets[0].net);
    }
}
