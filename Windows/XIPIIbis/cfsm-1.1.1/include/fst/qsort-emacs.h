/* $Id: qsort-emacs.h,v 1.1 2002/07/03 00:32:05 lk Exp $ */
/* Used to be called qsort-emacs-19.31.h. Name shortened to stay within
   the 15 character limit on filenames that affect the 'ar' command
   in some systems such as Solaris where 'ar -xv libfsmbase.a' doesn't
   work on longer filenames. */

/* Plug-compatible replacement for UNIX qsort.
   Copyright (C) 1989 Free Software Foundation, Inc.
   Written by Douglas C. Schmidt (schmidt@ics.uci.edu)

This file is part of GNU CC.

GNU QSORT is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU QSORT is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU QSORT; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA. */


#ifdef sparc
#include <alloca.h>
#endif

/* int qsort_emacs_19_31(char *base_ptr, int total_elems, int size, int (*cmp)()); */
void qsort_emacs_19_31(void *base_ptr, size_t total_elems, size_t size, 
		       int (*cmp)(const void *, const void *));
