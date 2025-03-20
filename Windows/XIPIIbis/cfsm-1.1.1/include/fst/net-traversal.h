/* $Id: net-traversal.h,v 1.5 2002/07/03 01:08:00 lk Exp $ */
/* Copyright (c) 2000 by the Xerox Corporation.  All rights reserved */

#include "types.h"
#include "virtual-base.h"

/******************************************************
 **
 **  NET-TRAVERSAL.H
 **
 **  Caroline Privault, XRCE, September 2000
 **
 **  Network traversal call-back routines
 **
 ******************************************************/

#ifndef C_FSM_NET_TRAVERSAL
#define C_FSM_NET_TRAVERSAL


#ifdef __cplusplus
extern "C" {
#endif
    
/* void* start_state_function(NETptr net, void ** arc_location, int *final); */
/* - Returns true if a start state has been found;
   - Standard:
               Sets *arc_location to the address of the first ARC.
   - Compacted:
               Sets *arc_location to the location of the first arc
               label to be read.
*/

void FsmFstExport *start_state_standard(NETptr net, void ** arc_location, int *final);


void FsmFstExport *start_state_compacted(NETptr net, void ** arc_location, int *final);

/********************************************************************/
/* id_type label_from_arc_location(NETptr net, void ** arc_location, int *final, int *last_arc);  */
/* - Returns the first arc label found from the current location; returns
     ID_NO_SYMBOL if there is no arc to read.
     
     Indicates if the DESTINATION from the current location is final.

     Indicates if the current location contains one more arc label to be read.
     
     The first parameter "net" is necessary only for the compact version of
     the function but has to stand also in the prototype of the standard
     version in order to fit the prototype shared by the 2 functions.
     
   - Standard:
               Uses "arc_location" as a pointer to an ARCptr
   - Compacted:
               Uses "arc_location" as a pointer to an unsigned char
*/

id_type FsmFstExport label_from_arc_location_stand(NETptr net, void ** arc_location, int *final, int *last_arc);

id_type FsmFstExport label_from_arc_location_comp(NETptr net, void ** arc_location, int *final, int *last_arc);


/********************************************************************/
/* void next_arc_location(NETptr net, void ** arc_location, int last_arc); */
/* - Sets *arc_location to the location of the next arc found from the
     current location.
          
     The first parameter "net" is necessary only for the compact version of
     the function but has to stand also in the prototype of the standard
     version in order to fit the prototype shared by the 2 functions.
     
   - Standard:
               Uses "arc_location" as a pointer to an ARCptr
   - Compacted:
               Uses "arc_location" as a pointer to an unsigned char
*/


void FsmFstExport next_arc_location_stand(NETptr net, void ** arc_location, int last_arc);
        

void FsmFstExport next_arc_location_comp(NETptr net, void ** arc_location, int last_arc);

/********************************************************************/
/* void* destination_from_arc(NETptr net, void **arc_location); */
/* - Returns a boolean indicating if we moved to another state;
     
     Updates arc_location: arc_location must point to the location of
     the first arc of the destination state.

     The first parameter "net" is necessary only for the compact version of
     the function but has to stand also in the prototype of the standard
     version in order to fit the prototype shared by the 2 functions.
     
   - Standard:
               Uses "arc_location" as a pointer to an ARCptr
   - Compacted:
               Uses "arc_location" as a pointer to an unsigned char
*/

void FsmFstExport *destination_from_arc_stand(NETptr net, void **arc_location);
void FsmFstExport *destination_from_arc_comp(NETptr net, void **arc_location);


#ifdef __cplusplus
}
#endif

#endif /* C_FSM_NET_TRAVERSAL */
