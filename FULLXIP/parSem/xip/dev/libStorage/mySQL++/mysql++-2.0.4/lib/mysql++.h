/// \file mysql++.h
/// \brief The main MySQL++ header file.
///
/// This file brings in all MySQL++ headers except for custom*.h,
/// which is a strictly optional feature of MySQL++.
///
/// There is no point in trying to optimize which headers you include,
/// because every MySQL++ program needs query.h, and that #includes
/// all the other headers indirectly, except for custom*.h and
/// compare.h.  (query.h doesn't bring in compare.h because it's not
/// used within the library anywhere; its facilities are only for
/// end-user programs.)  The only possible optimization is to include
/// query.h instead of mysql++.h, and this results only in trivial
/// compile time reductions at the expense of code clarity.

/***********************************************************************
 Copyright (c) 1998 by Kevin Atkinson, (c) 1999, 2000 and 2001 by
 MySQL AB, and (c) 2004, 2005 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the CREDITS
 file in the top directory of the distribution for details.

 This file is part of MySQL++.

 MySQL++ is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.

 MySQL++ is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with MySQL++; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
 USA
***********************************************************************/

#if !defined(MYSQLPP_MYSQLPP_H)
#define MYSQLPP_MYSQLPP_H

// Encode MySQL++ library version number.  MYSQLPP_VERSION macro takes
// major, minor and bugfix numbers (e.g. 1, 2, and 3) and encodes them
// like 0x010203.  MYSQLPP_LIB_VERSION is the current library version
// in the same scheme, so between these two macros you can create
// conditional code based on the MySQL++ library version.
#define MYSQLPP_VERSION(major, minor, bugfix) \
		(((major) << 16) | ((minor) << 8) | (bugfix))
#define MYSQLPP_LIB_VERSION MYSQLPP_VERSION(2, 0, 4)

// This #include order gives the fewest redundancies in the #include
// dependency chain.
#include "connection.h"
#include "query.h"
#include "compare.h"

#endif // !defined(MYSQLPP_MYSQLPP_H)


/**
	\mainpage MySQL++ Reference Manual

	\section getting_started Getting Started

	The best place to get started is the
	<a href="../../userman/html/index.html">user manual</a>. It
	provides a guide to the example programs and more.


	\section classes Major Classes

	In MySQL++, the main user-facing classes are mysqlpp::Connection,
	mysqlpp::Query, mysqlpp::Result, and mysqlpp::Row.

	In addition, MySQL++ has a mechanism called Specialized SQL
	Structures (SSQLS), which allow you to create C++ structures
	that parallel the definition of the tables in your database
	schema. These let you manipulate the data in your database using
	native C++ data structures. Programs using this feature often
	include very little SQL code, because MySQL++ can generate most
	of what you need automatically when using SSQLSes. There is a
	whole chapter in the user manual on how to use this feature of
	the library, plus a section in the user manual's tutorial chapter
	to introduce it. It's possible to use MySQL++ effectively without
	using SSQLS, but it sure makes some things a lot easier.


	\section files Major Files

	The only two header files your program ever needs to include
	are mysql++.h, and optionally custom.h. (The latter implements
	the SSQLS mechanism.) All of the other files are used within
	the library only.


	\section user_questions If You Have Questions...

	If you want to email someone to ask questions about this library,
	we greatly prefer that you send mail to the MySQL++ mailing list,
	which you can subscribe to here: http://lists.mysql.com/plusplus

	That mailing list is archived, so if you have questions, do a
	search to see if the question has been asked before.

	You may find people's individual email addresses in various
	files within the MySQL++ distribution. Please do not send mail
	to them unless you are sending something that is inherently
	personal. Questions that are about MySQL++ usage may well be
	ignored if you send them to our personal email accounts. Those of
	us still active in MySQL++ development monitor the mailing list,
	so you aren't getting any extra "coverage" by sending messages
	to those addresses in addition to the mailing list.


	\section licensing Licensing

	MySQL++ is licensed under the GNU Lesser General Public License,
	which you should have received with the distribution package in
	a file called "LGPL" or "LICENSE". You can also view it here:
	http://www.gnu.org/licenses/lgpl.html or receive a copy by
	writing to Free Software Foundation, Inc., 51 Franklin Street,
	Fifth Floor, Boston, MA 02110-1301, USA.
*/

