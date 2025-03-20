/* $Id: header.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1993 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  HEADER.H
 **
 **  Lauri Karttunen, RXRC December 1993
 **
 ******************************************************/

#ifndef C_FSM_HEADER

#define C_FSM_HEADER

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#include <time.h>

#include "fst/syst-dep.h"
#include "fst/struct.h"
#include "fst/sig-prop.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	unsigned int final : 1;
	unsigned int last_arc : 1;
	long int code;
} label;

#define getbit(X,Y)         ((X) >> (Y) & 1)    /* Returns the Y'th bit in X */

/* FST file type */
#define KAPLAN_COMPACTED_FILE_TYPE 1
#define OLD_STANDARD_FILE_TYPE 8
#define STANDARD_FILE_TYPE 19
#define OLD_STANDARD_COMPACTED_FILE_TYPE 101
#define STANDARD_COMPACTED_FILE_TYPE 20
#define STATUS_BAR_WIDTH 33
#define NEW_STANDARD_FILE_TYPE 21
#define NEW_COMPACTED_FILE_TYPE 22

/* In transducers, single OTHER is written to a file as the pair <1,1>
   whereas OTHER:OTHER is represented as a negative pair <-1,-1>.  */
   
#define ONE_SIDED_OTHER ((int) -1)

#define LABEL_final(X) (X).final
#define LABEL_final_p(X) (X).final
#define LABEL_last_arc(X) (X).last_arc
#define LABEL_last_p(X) (X).last_arc
#define LABEL_code(X) (X).code

enum {end_of_data_bit = 7, type_bit = 6, last_arc_bit = 5,
      final_state_bit = 4};
     
#define include_next            128             /* Bit 7 */
#define type_mask                        64             /* Bit 6 */
#define more_data_mask          127             /* Binary 1111111 */
#define first_number_mask        63             /* Binary  111111 */
     
#define type_is_label                    64             /* set bit 7 */
#define last_arc_in_set          32             /* set bit 6 */
#define dest_is_final            16             /* set bit 5 */
     
#define number_p(X)        !((X) & (unsigned char) type_mask)
#define label_p(X)          ((X) & (unsigned char) type_mask)
#define continue_p(X)   ((X) & (unsigned char) include_next)
#define last_arc_p(X)   getbit((X), last_arc_bit)
#define final_p(X)              getbit((X), final_state_bit)

STANDARD_HEADERptr FstBaseExport make_header(void);
STANDARD_HEADERptr FstBaseExport copy_header(STANDARD_HEADERptr dest, STANDARD_HEADERptr src);
void FstBaseExport free_header(STANDARD_HEADERptr header);
void FstBaseExport strip_global_headers();

void report_dates (time_t other_date, time_t file_date, int order);
    
/* Retrieves the file type code when reading a net from a file. */
int FstBaseExport get_file_type (char* file_name, int* file_type);
int FstBaseExport get_user_license_type();
    void FstBaseExport set_user_license_type(int level);
STANDARD_HEADERptr FstBaseExport get_header(FILE *stream, unsigned long *byte_count);
STANDARD_HEADERptr FstBaseExport read_header (FILE *stream, unsigned long *byte_count,
	STANDARD_HEADERptr header, int check_only_p);
int FstBaseExport std_file_header_p (FILE *stream);
 
STANDARD_HEADERptr FstBaseExport write_header(FILE *stream, int type, int version,
				char *copyright_notice, int net_count,
				NETptr net, unsigned long *byte_count,
				int old_format_p);
                                 
STANDARD_HEADERptr FstBaseExport get_last_header(void);
void FstBaseExport set_header_file_name(STANDARD_HEADERptr header, char *name);
void FstBaseExport print_std_file_header_s(STANDARD_HEADERptr header, int rm,
                             int indent, FILE *stream);
void FstBaseExport print_file_info (FILE *stream);
char FstBaseExport *decode_license_type(int code);
int FstBaseExport encode_license_type(char *str);
#define print_std_file_header(X, Y, Z) \
	print_std_file_header_s((X), (Y), (Z), stdout)

int FstBaseExport valid_license_type(int code);

/* Type of license for the FST application writing or reading a FST */
#define FILE_TYPE_LICENSE_PRIVATE  127
  /* Can read/write all types of FST */
#define FILE_TYPE_LICENSE_RESERVED 63
  /* Reserved for future use */
#define FILE_TYPE_LICENSE_LICENSED 31 
  /* Can read/write LICENSED and PUBLIC types of FST */
#define FILE_TYPE_LICENSE_PUBLIC   15
  /* Can read/write only PUBLIC type of FST */
#define FILE_TYPE_LICENSE_NONE      0
  /* Can write STANDARD_FILE_TYPE and STANDARD_COMPACTED_TYPE
     legacy files */

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_HEADER */

