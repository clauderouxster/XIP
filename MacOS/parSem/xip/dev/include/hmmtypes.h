/* $Id: hmmtypes.h,v 1.6 2010/02/05 12:42:49 roux Exp $ */
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
   filename   : hmmtypes.h
   Date       : 10/01/1999
   Purpose    : HMM encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/

/*************************************************************
 *
 *  FILE  "dstypes.h"
 *
 *      DEFINITION OF TYPES, VARIABLES, STRUCTURES, ETC. FOR
 *      DISAMBIGUATION OF ALTERNATIVE TAGS
 *
 *  Andre Kempe, RANK XEROX, Grenoble, France
 *  30/01/96 - ...
 *
 *************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifdef WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif

/* use the following definition if you compile for SunOS */
/*
#define memmove(A,B,N)  bcopy(B,A,N)
*/

#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif

#ifndef min_of
#define min_of(X,Y)  (X<Y)?X:Y
#define max_of(X,Y)  (X>Y)?X:Y
#endif


#define L_LINE_HMM          5000
#define MAX_TOKENS_HMM       100
#define REALLOC_FACTOR_HMM   2.0

#define PROGRAM_NAME_HMM  "DISAMB"

/*=================== Current state data ====================*/

#define ENV_DISAMB_FLAGS      "DISAMB_FLAGS"
#define DEF_DISAMB_FLAGS      "cbi1+"

#define FLAG_SHOW_COMMENTS      'c'
#define FLAG_DONT_BUFFER        'd'
#define FLAG_PRINT_ALL_TAGS     'a'

#define FLAG_TAGCUT_BACKWARD    'b'
#define FLAG_TAGCUT_FORWARD     'f'
#define FLAG_TAGCUT_INCLUSIVE   'i'

typedef unsigned short bit;

typedef struct {
  struct {
    char *flag_string;

    short show_comments;
    short dont_buffer;       /* every fprintf() followed by fflush() */
    short print_all_tags;

    char tagcut_sym;
    short tagcut_num;
    short tagcut_backward;
    short tagcut_forward;
    short tagcut_inclusive;  /*preserve the cut symbol; cut before or behind*/
  }
  user;
  struct {
    time_t seconds;
    long cur_line_num;
    int reading_hmm;
    int reading_input;
  }
  system;
}
T_DISAMB_FLAGS;

/*======================== HMM file =========================*/

/* Standard header.  Not all fields are used by the HMM file. */

#define MAX_COPYRIGHT_SIZE      512
#define MAX_VERSION_SIZE         32
#define TRAINER_MODEL_FILETYPE   15
#define TAG_SEPARATORS          " \t\n[]"

typedef unsigned char byte;                /* my guess: 1 byte */
typedef unsigned short word;           /* my guess: 2 bytes */
#ifdef WIN32
typedef unsigned __int64 longword; 
#else
#ifdef arch64
typedef unsigned long longword;
#else
typedef unsigned long long int longword;   /* my guess: 8 bytes */
#endif
#endif

typedef struct
{
    word	flu;
    byte	fll;
    byte	header_length;

    byte	file_type;
    byte	file_version;
    byte	copyright_loc;
    byte	index_language;

    longword	i_date;
}
HMM_HEADERtype;    /* header of the HMM file */

/*====================  Basic HMM data  =====================*/

typedef struct {
  double *Pi;          /*  Pi [tag]  */
  double **A;          /*  A [prev] [cur]  */
  double *A_buffer;
  double **B;          /*  B [tag] [class]  */
  double *B_buffer;
}
T_HMM;

/*====================   general purpose heaps   =====================*/

typedef struct EQUAL_ITEM_BLOCK_TYPE *EIBLOCKptr;
typedef struct EQUAL_ITEM_BLOCK_TYPE {
  long used;           /* number of items currently used */
  void *items;         /* vector of items */
  EIBLOCKptr next;     /* next item block */
}
EIBLOCKtype;

typedef struct EQUAL_ITEM_HEAP_TYPE {
  char *name;             /* heap name */
  long bytes_per_item;
  long items_per_block;
  long bytes_per_block;
  long blocks_in_heap;
  long bytes_in_heap;
  EIBLOCKptr blocks;      /* list of item blocks */
  void *free_list;        /* free-list of items */
}
EIHEAPtype, *EIHEAPptr;

/*====================   Sorted tag lists   =====================*/

typedef struct ALLTAG_CELL_TYPE *ATGCELLptr;
typedef struct ALLTAG_CELL_TYPE {
  short tag;              /* tag index */
  float score;            /* tag score */
  ATGCELLptr less;       /* list of less likely tags */
}
ATGCELLtype;

#define ATG_BLOCK_LENGTH    200

/*====================   Viterbi Tree   =====================*/

#define NO_LOG_SCORE       1e20   /* this hight score will penalize pathes */

typedef struct VIT_CELL_TYPE *VCELLptr;
typedef struct VIT_CELL_TYPE {
  VCELLptr prev;          /* previous tag cell */
  ATGCELLptr all_tags;    /* sorted tag list */
  short tag;              /* tag index */
  short in_count;         /* count of incomming pointers */
}
VCELLtype;

#define VIT_BLOCK_LENGTH    200

/*-----------------------------*/

typedef struct VIT_VECTOR_TYPE {
  short tag;              /* current tag index */
  VCELLptr prev;          /* list of previous tag cells */
  double score;           /* tag chain score */
  double evalu;           /* temporary evaluation score */
}
VITVECTORtype, *VITVECTORptr;

typedef struct VIT_POSITION_TYPE {
  int max;
  int len;
  VITVECTORptr alt;       /* alternative tags at current position */
  short *tagseq;
  short classe;            /* current class index */
  struct {
    short sent_begin;
    short sent_end;
    short sent_bound;
  }
  flags;
}
VITPOStype, *VITPOSptr;

typedef struct VIT_TREE_TYPE {
  VITPOSptr cur;
  VITPOSptr prev;
  struct {
    EIHEAPptr vcells;    /* viterbi cell heap */
    EIHEAPptr atgcells;  /* heap for tag list cells */
  }
  heap;
}
VTREEtype, *VTREEptr;                    /* Viterbi tree */

/*===================  Tags and classes  ====================*/

#define HASH_LEN_FACTOR   1.4
#define CLASS_END        -1
#define SENT_BOUND       -2
#define NO_INDEX         -1

#define SENT_END_TAG  "+SENT"


typedef struct HASH_TYPE_HMM *HASH_HMMptr;

typedef struct HASH_TYPE_HMM {
  void *item;            /* item of any type */
  HASH_HMMptr next;          /* next hash cell */
}
HASH_HMMtype;


struct TAG_HASH_TYPE_HMM {
  int num;
  char **tag;
  char *buffer;

  char **out_tag;
  char *out_buffer;

  short sent_end;    /* ID of sentence end tag */
  struct {
    int len;
    HASH_HMMptr *tab;
    HASH_HMMptr list;
  }
  hash;
};


struct CLASS_HASH_TYPE_HMM {
  int num;
  short **classe;
  short *buffer;

  struct {
    int len;
    HASH_HMMptr *tab;
    HASH_HMMptr list;
  }
  hash;
};

/*===================  Input and output  ====================*/

#define MAX_WORDS             20
#define MAX_ALTBF             50
#define MAX_LINEBF          1000

#define TAG_POSITION           2   /* token[2] of input line is a tag */
#define INPLINE_SEPARATORS  "\t"
#define SENT_END_SYMBOLS    ".!?:;"

typedef struct INP_LINE_TYPE {
  char *line;             /* input line */
  short tag;              /* tag of the input line */
}
INPLINEtype, *INPLINEptr;

typedef struct INP_WORD_TYPE {
  int num;                /* number of alternatives for current word */
  INPLINEptr alt;         /* vector of alternatives for current word */
  void* utilisateur;
}
INPWORDtype, *INPWORDptr;

typedef struct WORD_BUFFER_TYPE {
  long max;
  long num;            /* number of currently bufferted words */
  INPWORDptr word;     /* vector of currently bufferted words */
  struct {
    struct {
      long max;
      long len;
      INPLINEptr bf;
    }
    alt;               /* alternatives for bufferted words */
    struct {
      long max;
      long len;
      char *bf;        /* input lines */
    }
    line;
  }
  buffer;
  long words_total;    /* total number of words read */
}
WORDBFtype, *WORDBFptr;

/*===========================================================*/





