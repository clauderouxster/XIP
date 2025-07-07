/* $Id: net-traversal.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 2000 by the Xerox Corporation.  All rights reserved */


#include "fst/net-traversal.h"
#include "fst/virtual-base.h"


/******************************************************
 **
 **  NET-TRAVERSAL.C
 **
 **  Caroline Privault, XRCE, September 2000
 **
 **  Network traversal call-back routines
 **
 ******************************************************/


/* The following MACROs come from files cmp-net.h or compact.h */
#define first_label_mask 31
#define first_addr_mask 127
#define final_dest_mask 64
#define last_mask 32
#define next_data_mask 127
#define include_next_mask 128
#define more_to_come_p(X)          ((X) & include_next_mask)

/********************************************************************/
/*                     Start state functions                        */
/*                                                                  */
/*  Available versions are:                                         */
/*   - Standard                                                     */
/*   - Compacted                                                    */
/*   -                                                              */
/********************************************************************/

void* start_state_function(NETptr net, void ** arc_location, int *final);
/* - Returns the location of the start state if it has been found;
     NULL otherwise.
   - Standard:
               Sets *arc_location to the address of the first ARC.
   - Compacted:
               Sets *arc_location to the location of the first arc
               label to be read.
*/

void* start_state_standard(NETptr net, void** arc_location, int *final)
{
    STATEptr state;

    state= NET_start_state(net);
    if (state != NULL)
    {
      /* If state is an ordinary state, virtual_arc_set(state)
	 is equivalent to STATE_arc_set(state). If the state
	 is a virtual state virtual_arc_set(state) makes it real.
	 L.K. 2001-02-02.
        *arc_location= (void *) STATE_arc_set(state);
      */
        *arc_location= (void *) virtual_arc_set(state);

        *final= STATE_final(state);
        return ((void*) state);
    }
    else
    {
        *arc_location= NULL;
        *final= FALSE;
        return NULL;
    }
}


void* start_state_compacted(NETptr net, void ** arc_location, int *final)
{
    /* Compacted version:
       Sets *arc_location to the location of the first arc label to be read
       by setting jumping at the next unread byte just after the client
       data encoding. */
                   
    unsigned char * location;

    location=  NET_start_loc(net);

    if (NET_names_matter(net))
    {
        /* Skips over one variable length sequence of bytes. */ 
        while (more_to_come_p(*location++))
          ;
    }
    
    * arc_location= (void *) location;
    
    if (location != NULL)
    {
        *final= NET_start_state_final(net);
        return ((void *) location);
    }
    else
    {
        *final= FALSE;
        return NULL;
    }
}


/********************************************************************/
/*               label_from_arc_location functions                  */
/*                                                                  */
/*  Available versions are:                                         */
/*   - Standard   (can handle virtual too)                          */
/*   - Compacted                                                    */
/*   -                                                              */
/********************************************************************/
id_type label_from_arc_location(NETptr net, void ** arc_location, int *final, int *last_arc); 
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

id_type label_from_arc_location_stand(NETptr net, void ** arc_location, int *final, int *last_arc)
{

    /* At this stage, "*arc_location" cannot have a NULL value; it is tested
       outside just before calling the function. */
    ARCptr arc;

    /* "*arc_location" is supposed to contain a pointer to an ARC object:
       it is not modified by the function; this is the function
       next_arc_location() that moves to the next arc. */
    arc= (ARCptr) *arc_location;
          

    /* If the destination is a real state, virtual_final_p(state)
       is equivalent to STATE_final(state). If it is a virtual
       state, it will be realized to determine its finality.
       2000-02-02 L.K.
    *final= STATE_final(ARC_destination(arc));
    */

    *final = virtual_final_p(ARC_destination(arc));

    /* if (next_arc(arc)) */
/*     { */
/*         *last_arc= FALSE; */
/*     } */
/*     else */
/*     { */
/*         *last_arc= TRUE; */
/*     } */
    return (ARC_label(arc));
}

id_type label_from_arc_location_comp(NETptr net, void ** arc_location, int *final, int *last_arc)
{   /* This fn works only for the "SMALLEST" compact_option. */

    /* At this stage, "*arc_location" cannot have a NULL value; it is tested
       outside just before calling the function.
       Starting from (*arc_location) we are supposed to find a label encoding;
       the function decodes and returns the label id, and it sets also the
       values of FINAL and LAST_ARC.
       It sets (*arc_location) to the next unread byte.  */
    
    id_type LABEL_ID;
    id_type *RECODE_KEY = ALPH_items(NET_recode_key(net));
    unsigned char *loc= (unsigned char*) (*arc_location);
    unsigned char byte;
  
  byte = *loc++;
  
  /* Set label flags. */
  
  *final = byte & final_dest_mask;
  *last_arc = byte & last_mask; 
  
  /* Get label id. */
  
  LABEL_ID = byte & first_label_mask;
  
  while (more_to_come_p(byte))
    {
      byte = *loc++;
      LABEL_ID = (LABEL_ID << 7) | (byte & next_data_mask);
    }

  if (LABEL_ID >= ALPH_len(NET_recode_key(net)))
  {
      /* Empty state. No arcs. */
      LABEL_ID= ID_NO_SYMBOL;		
  }
  else
  {
      LABEL_ID= RECODE_KEY[LABEL_ID];
  }
  
  *arc_location= (void*) loc;
  return(LABEL_ID); 
}



/********************************************************************/
/*                    next_arc_location functions                   */
/*                                                                  */
/*  Available versions are:                                         */
/*   - Standard                                                     */
/*   - Compacted                                                    */
/*                                                                  */
/********************************************************************/
void next_arc_location(NETptr net, void ** arc_location, int last_arc);
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


void next_arc_location_stand(NETptr net, void** arc_location, int last_arc)
{
    ARCptr arc;

    /* At this stage, "*arc_location" is supposed to contain a pointer to
       an ARC object. */
    
    arc= (ARCptr) *arc_location;
        
    arc= next_arc(arc);
        
    *arc_location= (void*) arc;

    
    /* If "*arc_location" is set now to a NULL value then we will backtrack
       in the calling apply function in order to find another location
       to re-start from. */
}
        

void next_arc_location_comp(NETptr net, void** arc_location, int last_arc)
{
    /* This fn works only for the "SMALLEST" compact_option. */

    /* 
     At this stage, an arc label has just been read, and hence
     the location contained in (*arc_location) points to a field that
     encodes the current arc's destination; then two cases can occur:
     - the current arc is the last one to be read from the current
       location; then *arc_location is set to NULL.
     - there is at least one more arc label to be read; then we have first to
       skip over the bytes encoding the address of the destination of the
       current arc.
     NB: the parameter LAST_ARC is used but not updated by this function:
     this is function label_from_arc_location() that sets the value.
  */

    unsigned char *loc= (unsigned char*) (*arc_location);
    

    if (last_arc)
    {
      /* No more arcs to read.
         --------------------------------*/
        *arc_location = NULL;
    }
    else
    {
        /*Move to a new arc label:
           --------------------------------*/

        /* Skip over the previous arc destination:
           (this way of skipping works only for the SMALLEST format).*/
/*         byte = *loc++; */
        while( more_to_come_p(*loc) )
        {
            loc++;
        }
        loc++;
        *arc_location= (void*) loc;
    } /* last_arc == FALSE */
}




/********************************************************************/
/*                  destination_from_arc functions                  */
/*                                                                  */
/*  Available versions are:                                         */
/*   - Standard                                                     */
/*   - Compacted                                                    */
/*                                                                  */
/********************************************************************/
void* destination_from_arc(NETptr net, void **arc_location);
/* - Returns the location of the state we moved to;
     
     Updates arc_location: at the end of the function, arc_location must
     point to the location of the first arc of the destination state.

     The first parameter "net" is necessary only for the compact version of
     the function but has to stand also in the prototype of the standard
     version in order to fit the prototype shared by the 2 functions.
     
   - Standard:
               Uses "arc_location" as a pointer to an ARCptr
   - Compacted:
               Uses "arc_location" as a pointer to an unsigned char
*/

void* destination_from_arc_stand(NETptr net, void** arc_location)
{
    /* At this stage, "*arc_location" is supposed to contain a pointer
       to an ARC object. */
    STATEptr state;
    ARCptr arc= (ARCptr) *arc_location;

    state= ARC_destination(arc);

    /* If state is an ordinary state, virtual_arc_set(state)
       is equivalent to STATE_arc_set(state). If the state
       is an ordinary state, virtual_arc_set(state) computes
       a real arcset for it. L.K. 2001-02-02.
      */

#ifndef WORLD /* Are we linking FSMMORE? */
    *arc_location= (void*) virtual_arc_set(state);
#else
    *arc_location= (void *) STATE_arc_set(state);
#endif /* WORLD */
    return ( (void*) state);
}

void* destination_from_arc_comp(NETptr net, void** arc_location)
{
    /* This fn works only for the "SMALLEST" compact_option. */
    
    /* This function returns the address of the current arc destination.
       It sets (*arc_location) at the next unread byte just after the client
       data encoding.
       
       NB: the function works only for the SMALLEST version of the
       compaction since we suppose here that arc destinations are variable
       length fields. */

    /* At this stage we are supposed to have read an arc label and hence
       *arc_location contains the location from which we can start to read
       the arc destination. */
    unsigned char *loc= (unsigned char*) (*arc_location);
    unsigned char byte = *loc++;
    unsigned long int incr;

    /* Reads the address starting at LOC, and computes the value of the
       destination address. */
    
    incr = (byte & first_addr_mask);
  
    while (more_to_come_p(byte))
    {
        byte = *loc++;
        incr = (incr << 7) | (byte & next_data_mask);
    } 
      
/*     *arc_location= (void*) (NET_arc_block(net) + incr); */

    /* Set (*arc_location) exactly at the place where the label encoding
       starts, by skipping the client data, if any. */
    
    loc= NET_arc_block(net) + incr;
    if (NET_names_matter(net))
    {
        /* Skips over one variable length sequence of bytes. */ 
        while (more_to_come_p(*loc++))
          ;
    }
    *arc_location= (void*) loc;
    
    return ((void*) (NET_arc_block(net) + incr));
}



/* ******* End of traversal sub-routine set ******* */



