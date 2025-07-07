/* $Id: flags.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1997 Xerox Xorporation. All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "fst/struct.h"
#include "fst/error.h"
#include "fst/label.h"
#include "fst/flags.h"
#include "fst/fsm_io.h"
#include "fst/utf8_fatstr_util.h"

void check_for_flags(NETptr net);

static void check_for_flags_in_virtual_net(NETptr net)
{
  /* Recursively check for the presence of flags in a virtual
     network by descending all the way to the real leaf networks
     and percolating the information up the vinfo tree.
  */

  VirtualInfo *vinfo = NET_virtual_info(net);
  NETptr op1 = vinfo->op1;
  NETptr op2 = vinfo->op2;
  int upper_flags_p = FALSE;
  int lower_flags_p = FALSE;

  if (op1)
    {
      check_for_flags(op1);
      if (NET_u_flag_diacr(op1))
	upper_flags_p = TRUE;
      if (NET_l_flag_diacr(op1))
	lower_flags_p = TRUE;
    }
  
  if (op2 && !(upper_flags_p && lower_flags_p))
    {
      check_for_flags(op2);
      if (NET_u_flag_diacr(op2))
	upper_flags_p = TRUE;
      if (NET_l_flag_diacr(op2))
	lower_flags_p = TRUE;
    }
  
  NET_u_flag_diacr(net) = upper_flags_p;
  NET_l_flag_diacr(net) = lower_flags_p;
}

void check_for_flags(NETptr net)
{
  /* Checks for the presence of flag diacritics and sets the
     NET_u_flag_diacr(net) and NET_l_flag_diacr(net) values
     in the appropriate way. If the network is virtual,
     we do not know whether the realized network will have
     flags on either side because the labels have not been
     computed yet. But because we know the sigma alphabet
     of the virtual net, we can set the flags to TRUE on
     both sides if flags are present there. */

  if (NET_Kaplan_compressed(net))
    {
        /* A Kaplan compressed network has no labels or sigma alphabet
           and there is no way to compute it easily: in this case we choose
           to let NET_u_flag_diacr(net) and NET_l_flag_diacr(net) set to
           FALSE by default. */
      NET_u_flag_diacr(net) = FALSE;
      NET_l_flag_diacr(net) = FALSE;
    }
  else if (NET_is_virtual(net))
    check_for_flags_in_virtual_net(net);
  else
    {
      ALPHABETptr  alph = binary_to_label(NET_labels(net));
      int i, upper_flags_p = FALSE, lower_flags_p = FALSE;
      id_type id;


      for (i = 0; i < ALPH_len(alph); i++)
	{
	  id = ALPH_item(alph, i);

	  if (tuple_id_p(id))
	    {
	      if (flag_diacritic_p(id_to_upper_id(id)))
		upper_flags_p = TRUE;

	      if (flag_diacritic_p(id_to_lower_id(id)))
		lower_flags_p = TRUE;
	    }
	  else if (flag_diacritic_p(id))
	    {
	      upper_flags_p = TRUE;
	      lower_flags_p = TRUE;
	    }
	}
  
      NET_u_flag_diacr(net) = upper_flags_p;
      NET_l_flag_diacr(net) = lower_flags_p;
    }
} /* check_for_flags */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
int has_flag_diacritics_p(NETptr net)
{
  /* Returns TRUE if the net's sigma contains a flag diacritic */

  ALPHABETptr sig = label_to_binary(sigma(net));
  int i;

  for (i = 0; i < ALPH_len(sig); i++)
    if (ALPH_item(sig, i) && flag_diacritic_p(i))
      return(TRUE);

  return(FALSE);
}

int has_flag_diacritics_side_p(NETptr net, int side)
{
  /* Returns TRUE if the net's sigma contains a flag diacritic 
   on the selected side */

  ALPHABETptr labs = binary_to_label(NET_labels(net));
  int i;
  id_type id;

  for (i = 0; i < ALPH_len(labs); i++)
    {
      switch (side)
	{
	case (UPPER):
	  if (flag_diacritic_p(upper_id(ALPH_item(labs, i))))
	    {
	      NET_u_flag_diacr(net) = TRUE;
	      return(TRUE);
	    }
	  break;
	case (LOWER):
	  if (flag_diacritic_p(lower_id(ALPH_item(labs, i))))
	    {
	      NET_l_flag_diacr(net) = TRUE;
	      return(TRUE);
	    }
	  break;
	case(BOTH_SIDES):
	  id = ALPH_item(labs, i);
	  if (flag_diacritic_p(ALPH_item(labs, i)))
	    {
	      NET_u_flag_diacr(net) = TRUE;
	      NET_l_flag_diacr(net) = TRUE;
	      return(TRUE);
	    }
	  break;
	default:
	  fsm_fprintf (stderr,"FST Internal error: Incorrect side value: %d\n",
		       side);
	  exit(1);
	  break;
	}
    }
  
  switch(side)
    {
    case UPPER:
      NET_u_flag_diacr(net) = FALSE;
      break;
    case LOWER:
      NET_l_flag_diacr(net) = FALSE;
      break;
    default:
      break;
    }

  return(FALSE);
}

int print_net_flag_diacritics(NETptr net, FILE *stream)
{
  ALPHABETptr sig = label_to_binary(sigma(net));
  int i;
  int count = 0;
  FLAG_DIACRptr flag_diacr;

  for (i = 0; i < ALPH_len(sig); i++)
    if (ALPH_item(sig, i) && (flag_diacr = LABEL_flag(id_to_label(i))))
      {
	count++;
#ifdef DEBUG
	fsm_fprintf(stream, "%d ", i);
#endif /* DEBUG */
	print_label((id_type) i, stream, DONT_ESCAPE);
	fsm_fprintf(stream, ": ");
	print_flag_diacritic(flag_diacr, stream);
	fsm_fprintf(stream, "\n");
      }
  return(count);
}
#endif /* SLIM_FST */


ALPHABETptr make_flag_register(void)
{
  return(make_alph(flag_attribute_count(), BINARY_VECTOR));
}


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

#if NOT_USED
static void flag_action_trace(ALPHABETptr reg, FLAG_DIACRptr flag,
				int first_p, int success_p)
{
  id_type val = ALPH_item(reg, FLAG_DIACR_attribute(flag));

  if (first_p)
    {
      print_flag_diacritic(flag, stdout);
      fsm_fprintf (stdout, "\tCurrent value: ");
    }
  else if (success_p)
    fsm_fprintf (stdout, "SUCCEEDS. New value: ");
  else
    fsm_fprintf (stdout, "FAILS.  New value: ");

  if (val == UNDEFINED_FLAG_VALUE)
    fsm_fprintf(stdout, "(Neutral value)");
  else if ( !FLAG_DIACR_is_a_complement(val) )
    print_fat_string(id_to_flag_value_name(val));
  else
    {
      /* Complement value */
      printf("complement of ");
      print_fat_string(id_to_flag_value_name(FLAG_DIACR_complement(val)));
    }
  fsm_fprintf (stdout, "\n");
}
#endif /* NOT_USED */
#endif /* SLIM_FST */


int flag_action(ALPHABETptr flag_reg, FLAG_DIACRptr flag)
{
  int result;
  id_type attr = FLAG_DIACR_attribute(flag);
  id_type flag_value = FLAG_DIACR_value(flag);
  id_type reg_value = flag_value(flag_reg, attr);



if (flag_value > 32767)
  handle_error("Id of the flag value greater than 32767", "flag_action", flag_value);


#ifdef DEBUG
  /*
  flag_action_trace(flag_reg, flag, TRUE, 0);
  */
#endif /* DEBUG */

  switch(FLAG_DIACR_action(flag))
    {
    case NO_ACTION:
      /* For future extensions */
      return(TRUE);
      break;
    case CLEAR_SETTING:
    case POSITIVE_SETTING:
      /* Set the register to the given flag value.
	 Ignore the previous register value, if any. */
      flag_value(flag_reg, attr) = flag_value;
      result = TRUE;
      break;
    case NEGATIVE_SETTING:
      /* Set the register to the complement of the given flag value.
	 Ignore the previous register value, if any. */
      flag_value(flag_reg, attr) = FLAG_DIACR_complement(flag_value);/* -(flag_value); */
      result = TRUE;
      break;
    case UNIFY_TEST:
      /* Succeeds iff the register value is compatible with the flag value */
      if (reg_value == UNDEFINED_FLAG_VALUE)
	{
	  /* No previous register value. Set it to the flag value. */
	  flag_value(flag_reg, attr) = flag_value;
	  result = TRUE;
	}
      else if (reg_value == flag_value)
	result = TRUE;   /* Same as previous value */
      else if (FLAG_DIACR_is_a_complement(reg_value) && (reg_value != FLAG_DIACR_complement(flag_value)) )
	{
	  /* The register value is compatible with the flag value because
	     it is the complement of some other value. Set the register
	     to the flag value. */
	  flag_value(flag_reg, attr) = flag_value;
	  result = TRUE;
	}
      else
	result = FALSE;
      break;
    case DISALLOW_TEST:
      /* If the flag value is undefined, succeed iff the register value
	 is defined. If the flag value is specified, fail iff
	 it is compatible with the register value. */
      switch (flag_value)
	{
	case UNDEFINED_FLAG_VALUE:
	  result = (flag_value(flag_reg, attr) == UNDEFINED_FLAG_VALUE);
	  break;
	default:
	  /* if (reg_value >= 0)  */
	  if ( ! FLAG_DIACR_is_a_complement(reg_value) ) 
	    result = (reg_value != flag_value); /* incompatible value */
	  else
	    /* result = (-(reg_value) == flag_value); complement value */
	    result = (reg_value == FLAG_DIACR_complement(flag_value) );
	  break;
	}
      break;
    case REQUIRE_TEST:
      /* If the flag value is undefined, succeed iff the register value 
	 is defined. If the flag value is specified, succeed iff
	 the register value is the same.
	 */
      switch (flag_value)
	{
	case UNDEFINED_FLAG_VALUE: /* undefined */
	  result = (flag_value(flag_reg, attr) != UNDEFINED_FLAG_VALUE);
	  break;
	default:
	  result = (reg_value == flag_value);
	  break;
	}
      break;
    case FAIL_ACTION:
      /* Fail unconditionally. */
      result = FALSE;
      break;
    case SET_TO_ATTR:
      /* Set (in the register) flag1 to the value of flag2. Always TRUE. */
      flag_value(flag_reg, attr) = flag_value(flag_reg, flag_value);
      result = TRUE;
      break;
    case EQUAL_ATTR_TEST:
      /* Succeed if flag1 and flag2 have the same value, or if at least
	 one of them has an undefined value. Values are not modified. */
      result =
	((flag_value(flag_reg, attr) == flag_value(flag_reg, flag_value)) ||
	 (flag_value(flag_reg, attr) == UNDEFINED_FLAG_VALUE) ||
	 (flag_value(flag_reg, flag_value) == UNDEFINED_FLAG_VALUE) );
      break;
    default:
      handle_error("Unknown action or test", "FLAG_ACTION",
		   FLAG_DIACR_action(flag));
      result = FALSE;
      break;
    }

#ifdef DEBUG
/*
  flag_action_trace(flag_reg, flag, FALSE, result);
*/
#endif /* DEBUG */
  return(result);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int flag_attribute_occurs_in_p(NETptr net, id_type attr)
{
  /* Returns TRUE or FALSE depending on whether NET contains a
     flag diacritic arc that with ATTR.
     */

  int i;
  LABELptr lab;

  FLAG_DIACRptr flag;

  binary_to_label(NET_labels(net));

  for (i = 0; i < ALPH_len(NET_labels(net)); i++)
    {
      lab = id_to_label(ALPH_item(NET_labels(net), i));

      switch (LABEL_arity(lab))
	{
	case 2:
	  if (((flag = flag_diacritic(fstpair_upper(lab))) ||
	       (flag = flag_diacritic(fstpair_lower(lab)))) &&
	      FLAG_DIACR_attribute(flag) == attr)
	    return(TRUE);
	  break;
	case 1:
	  if (LABEL_flag_attrib(lab) == attr)
	    return(TRUE);
	  break;
	default:
	  handle_error("Odd label arity", "FLAG_ATTRIBUTE_OCCURS_IN_P",
		       LABEL_arity(lab));
	  break;
	}
    }

  return(FALSE);
}
#endif /* SLIM_FST */

