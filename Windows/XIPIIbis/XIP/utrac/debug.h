/***************************************************************************
 *            debug.h
 *
 *  Tue Oct  5 11:27:20 2004
 *  Copyright  2004  Alliance MCA
 *  Written by : Antoine Calando (antoine@alliancemca.net)
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
/*!
 * \file debug.h
 * \author Antoine Calando (antoine@alliancemca.net)
 *
 * \note EC ##args est une directive propre à gcc.
 * \note AC The solution is not to use these macro with only one argument
 *       and to create a new macro (with 1 argument) for these cases.
 */
 
#ifndef _DEBUG_H
#define _DEBUG_H

/*************** types ************************/
#include <sys/types.h>

#ifndef __cplusplus
/*	typedef unsigned short int bool;
	#ifndef true
		#define true 1
	#endif
	#ifndef false
		#define false 0
	#endif*/
#endif //__cplusplus
/*typedef unsigned char uchar;*/
#ifndef NULL
	#define NULL 0
#endif

/******************** Debugging stuff ******************/

#ifndef UT_DEBUG
#define UT_DEBUG 1
#endif

#if UT_DEBUG>=1
#define DBG1(msg, args...) \
	{ fprintf(stderr, "DEBUG %s:%d: "  msg  "\n", __FILE__, __LINE__, ## args); }
#else
#define DBG1(msg, args...)
#endif
	
#if UT_DEBUG>=2
#define DBG2(msg, args...) \
	{ fprintf(stderr, "DEBUG %s:%d: "  msg  "\n", __FILE__, __LINE__, ## args); }
#else
#define DBG2(msg, args...)
#endif

#if UT_DEBUG>=3
#define DBG(msg, args...) DBG3(msg, ## args)
#define DBG3(msg, args...) \
	{ fprintf(stderr, "DEBUG %s:%d:"   msg  "\n", __FILE__, __LINE__, ## args); }
#define DBG3_S(msg, args...) \
	{ fprintf(stderr, msg, ## args); }

#else
#define DBG(msg, args...)
#define DBG3(msg, args...)
#define DBG3_S(msg, args...)
#endif

#if UT_DEBUG>=1
#define ASSERT(expr) \
	if(!(expr)) { \
		fprintf(stderr, "\nASSERT %s:%d: ******* Assertion "  #expr  " failed! ******* \n", __FILE__, __LINE__); \
	}
#define ASSERT_MSG(expr, msg, args...) \
	if(!(expr)) { \
		fprintf(stderr, "\nASSERT %s:%d: ******* Assertion "  #expr  " failed!" msg " ******* \n", __FILE__, __LINE__, ## args); \
	}
#define ERROR(msg, args...) \
	{ fprintf(stderr, "ERROR %s:%d:"   msg  "\n", __FILE__, __LINE__, ## args); \
	exit (-1); }
#else
#define ASSERT(expr)
#define ASSERT_MSG(expr, msg, args...)
#define ERROR(msg, args...)
#endif



#endif /* _DEBUG_H */
