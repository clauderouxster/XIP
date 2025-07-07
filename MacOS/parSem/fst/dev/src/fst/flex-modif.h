/* $Id: flex-modif.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* The output from FLEX needs some modifications before it can
   be used by the parser.
*/

/*   (1)  Add the line
 */
#define YY_USE_PROTOS

/* Re-define YY_READ_BUF_SIZE
 */
#ifdef MAC_OS
#undef YY_READ_BUF_SIZE
#define YY_READ_BUF_SIZE 512
#undef YY_BUF_SIZE
#define YY_BUF_SIZE (YY_READ_BUF_SIZE * 2) /* size of default input buffer */
#endif

/* Re-define YY_INPUT
 */
#ifdef MAC_OS
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
  if (fgets((char *) buf, max_size, yyin))	\
  result = strlen((char *) buf);	\
  else	\
    { \
  result = 0; \
  yy_init = 1; \
    }
#else
/* H.Poirier 7/11/94 : Read line/line instead of big chunk of file. */
#define YY_INPUT(buf,result,max_size) \
    if ( yy_current_buffer->yy_is_interactive ) \
    { \
        int c = getc( yyin ); \
        result = c == EOF ? 0 : 1; \
        buf[0] = (char) c; \
    } \
    else if (fgets( buf, max_size, yyin ) == NULL) \
      { \
        result = 0; \
        if (ferror( yyin )) \
          YY_FATAL_ERROR( "input in flex scanner failed" ); \
      } \
    else \
        result = strlen((char *) buf);
#endif

#if 0
int     fileno(FILE *fd);
#endif

/* Change the original definition of 'yyterminate'
   These are needed to initialize the scanner
   after a file has been completely read or when reading is stopped
   in the middle because of an error.
 */
#undef yyterminate
#define yyterminate() yy_init = 1; return ( YY_NULL )

/* Make the variable 'yy_init' non-static.
 */
/* int yy_init = 1; */		/* whether we need to initialize */
 
/* Required by release 2.4.7 (and 2.4.6 of flex) in MAC_OS. */

#if defined(MAC_OS)
#ifndef YY_MALLOC_DECL
#define YY_MALLOC_DECL void *malloc(size_t size);
#endif
#endif /* MAC_OS */
