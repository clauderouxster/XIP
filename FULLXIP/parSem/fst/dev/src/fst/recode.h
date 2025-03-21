/* $Id: recode.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1993 by the Xerox Corporation.  All rights reserved */


/******************************************************
 **
 **  RECODE.H
 **
 **  Lauri Karttunen, Xerox PARC, July 1993
 **
 **  Utility for converting between Macintosh, ISO 8859,
 **  IBM PC, etc. character encodings.
 **
 **
 ******************************************************/

#ifndef C_FSM_RECODE
#define C_FSM_RECODE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SymbolPair {
  char *old;
  char *new;
} SPtype, *SPptr;


typedef struct RecodeKey {
  int len;
  SPptr map;
} RKtype, *RKptr;

#define RK_len(X)	(X)->len
#define RK_map(X)	(X)->map

int recode_net(NETptr net, RKptr key, int reverse_p, long int *count);

int recode_chars(NETptr net, unsigned char *table, int reverse_p,
	long int *count);
int recode_strings(NETptr net, SPtype *pairs, int pair_count,
	int reverse_p, long int *count);
	
RKptr make_recode_key(FILE *stream);
void free_recode_key(RKptr key);

int mac_to_iso(NETptr net, long int *count);
int iso_to_mac(NETptr net, long int *count);
int unix_to_mac(NETptr net, long int *count);
int mac_to_unix(NETptr net, long int *count);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_RECODE */
