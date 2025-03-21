/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: fst-types.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

#ifndef FST_TYPES_H
#define FST_TYPES_H

enum file_types { STDIN_IN, FILE_IN, PIPE_IN, STRING_IN };

struct input_file
{
  char *name;
  enum file_types type;
};
typedef struct input_file ifile;

struct file_info
{
  void *buffer; /* It is opaque but we know that it serves for the lexer yy_current_buffer  99-6-16 TG */
  void *ch_buf; /* It is opaque but we know that it serves for the lexer yy_current_buffer->yy_ch_buf 99-6-16 TG */
  int line_number;
  ifile file;
};
typedef struct file_info finfo;

#endif /* FST_TYPES_H */

