# Makefile.in generated by automake 1.6.3 from Makefile.am.
# lib/Makefile.  Generated from Makefile.in by configure.

# Copyright 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.



# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation; either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with MySQL++; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
# USA
SHELL = /bin/sh

srcdir = .
top_srcdir = ..

prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include
pkgdatadir = $(datadir)/mysql++
pkglibdir = $(libdir)/mysql++
pkgincludedir = $(includedir)/mysql++
top_builddir = ..

ACLOCAL = ${SHELL} /home/grondeau/projets/parSem/xip/dev/libStorage/mySQL++/mysql++-2.0.4/missing --run aclocal-1.6
AUTOCONF = ${SHELL} /home/grondeau/projets/parSem/xip/dev/libStorage/mySQL++/mysql++-2.0.4/missing --run autoconf
AUTOMAKE = ${SHELL} /home/grondeau/projets/parSem/xip/dev/libStorage/mySQL++/mysql++-2.0.4/missing --run automake-1.6
AUTOHEADER = ${SHELL} /home/grondeau/projets/parSem/xip/dev/libStorage/mySQL++/mysql++-2.0.4/missing --run autoheader

am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_SCRIPT = ${INSTALL}
INSTALL_HEADER = $(INSTALL_DATA)
transform = s,x,x,
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = 
host_triplet = i686-pc-linux-gnu

EXEEXT = 
OBJEXT = o
PATH_SEPARATOR = :
AMTAR = ${SHELL} /home/grondeau/projets/parSem/xip/dev/libStorage/mySQL++/mysql++-2.0.4/missing --run tar
AS = @AS@
AWK = gawk
CC = gcc
CXX = g++
DEPDIR = .deps
DLLTOOL = @DLLTOOL@
ECHO = echo
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
MAINT = #
MYSQLPP_LIBTOOL_VERSION = 2:4:0
MYSQLPP_VERSION_BUGFIX = 4
MYSQLPP_VERSION_MAJOR = 2
MYSQLPP_VERSION_MINOR = 0
MYSQL_C_LIB = mysqlclient
OBJDUMP = @OBJDUMP@
OPTIONAL_EXAMPLES = examples
PACKAGE = mysql++
PTHREAD_CC = 
PTHREAD_CFLAGS = 
PTHREAD_LIBS = 
RANLIB = ranlib
STRIP = strip
VERSION = 2.0.4
acx_pthread_config = 
am__include = include
am__quote = 
install_sh = /home/grondeau/projets/parSem/xip/dev/libStorage/mySQL++/mysql++-2.0.4/install-sh
INCLUDES = -I$(top_srcdir)/lib
LIBS = -lz -lmysqlclient

MYSQLPP_LIB = $(top_builddir)/lib/libmysqlpp.la

AM_CXXFLAGS = 
LDADD = $(MYSQLPP_LIB) 

fooincludedir = $(pkgincludedir)/foo

EXTRA_DIST = \
		custom.pl \
		Doxyfile \
		Makefile.base \
		Makefile.bc \
		Makefile.gcc \
		Makefile.mingw \
		Makefile.rel \
		Makefile.simple \
		Makefile.vc


lib_LTLIBRARIES = libmysqlpp.la

libmysqlpp_la_LDFLAGS = -version-info 2:4:0

libmysqlpp_la_SOURCES = \
	coldata.cpp connection.cpp datetime.cpp fields.cpp field_names.cpp \
	field_types.cpp manip.cpp myset.cpp qparms.cpp query.cpp \
	result.cpp row.cpp sql_string.cpp string_util.cpp type_info.cpp \
	vallist.cpp


pkginclude_HEADERS = \
	coldata.h compare.h connection.h const_string.h convert.h custom.h \
	custom-macros.h defs.h datetime.h exceptions.h field_names.h \
	field_types.h fields.h lockable.h manip.h myset.h mysql++.h \
	noexceptions.h null.h platform.h qparms.h query.h resiter.h \
	result.h row.h sql_string.h stream2string.h string_util.h \
	tiny_int.h type_info.h vallist.h 

subdir = lib
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = $(top_builddir)/config.h
CONFIG_CLEAN_FILES = mysql++.h Doxyfile
LTLIBRARIES = $(lib_LTLIBRARIES)

libmysqlpp_la_LIBADD =
am_libmysqlpp_la_OBJECTS = coldata.lo connection.lo datetime.lo \
	fields.lo field_names.lo field_types.lo manip.lo myset.lo \
	qparms.lo query.lo result.lo row.lo sql_string.lo \
	string_util.lo type_info.lo vallist.lo
libmysqlpp_la_OBJECTS = $(am_libmysqlpp_la_OBJECTS)

DEFS = -DHAVE_CONFIG_H
DEFAULT_INCLUDES =  -I. -I$(srcdir) -I$(top_builddir)
CPPFLAGS =  -I/usr/include/mysql
LDFLAGS =  -L/usr/lib/mysql
depcomp = $(SHELL) $(top_srcdir)/depcomp
am__depfiles_maybe = depfiles
DEP_FILES = ./$(DEPDIR)/coldata.Plo \
	./$(DEPDIR)/connection.Plo ./$(DEPDIR)/datetime.Plo \
	./$(DEPDIR)/field_names.Plo \
	./$(DEPDIR)/field_types.Plo ./$(DEPDIR)/fields.Plo \
	./$(DEPDIR)/manip.Plo ./$(DEPDIR)/myset.Plo \
	./$(DEPDIR)/qparms.Plo ./$(DEPDIR)/query.Plo \
	./$(DEPDIR)/result.Plo ./$(DEPDIR)/row.Plo \
	./$(DEPDIR)/sql_string.Plo \
	./$(DEPDIR)/string_util.Plo \
	./$(DEPDIR)/type_info.Plo ./$(DEPDIR)/vallist.Plo
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
LTCXXCOMPILE = $(LIBTOOL) --mode=compile $(CXX) $(DEFS) \
	$(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
	$(AM_CXXFLAGS) $(CXXFLAGS)
CXXLD = $(CXX)
CXXLINK = $(LIBTOOL) --mode=link $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) \
	$(AM_LDFLAGS) $(LDFLAGS) -o $@
CXXFLAGS = -g -O2
DIST_SOURCES = $(libmysqlpp_la_SOURCES)
HEADERS = $(pkginclude_HEADERS)

DIST_COMMON = $(pkginclude_HEADERS) Doxyfile.in Makefile.am Makefile.in \
	mysql++.h.in
SOURCES = $(libmysqlpp_la_SOURCES)

all: all-am

.SUFFIXES:
.SUFFIXES: .cpp .lo .o .obj
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/common.am $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --foreign  lib/Makefile
Makefile: # $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe)
mysql++.h: $(top_builddir)/config.status mysql++.h.in
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@
Doxyfile: $(top_builddir)/config.status Doxyfile.in
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@
libLTLIBRARIES_INSTALL = $(INSTALL)
install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    f="`echo $$p | sed -e 's|^.*/||'`"; \
	    echo " $(LIBTOOL) --mode=install $(libLTLIBRARIES_INSTALL) $(INSTALL_STRIP_FLAG) $$p $(DESTDIR)$(libdir)/$$f"; \
	    $(LIBTOOL) --mode=install $(libLTLIBRARIES_INSTALL) $(INSTALL_STRIP_FLAG) $$p $(DESTDIR)$(libdir)/$$f; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	    p="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p"; \
	  $(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p; \
	done

clean-libLTLIBRARIES:
	-test -z "$(lib_LTLIBRARIES)" || rm -f $(lib_LTLIBRARIES)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  dir="`echo $$p | sed -e 's|/[^/]*$$||'`"; \
	  test -z "$dir" && dir=.; \
	  echo "rm -f \"$${dir}/so_locations\""; \
	  rm -f "$${dir}/so_locations"; \
	done
libmysqlpp.la: $(libmysqlpp_la_OBJECTS) $(libmysqlpp_la_DEPENDENCIES) 
	$(CXXLINK) -rpath $(libdir) $(libmysqlpp_la_LDFLAGS) $(libmysqlpp_la_OBJECTS) $(libmysqlpp_la_LIBADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT) core *.core

distclean-compile:
	-rm -f *.tab.c

include ./$(DEPDIR)/coldata.Plo
include ./$(DEPDIR)/connection.Plo
include ./$(DEPDIR)/datetime.Plo
include ./$(DEPDIR)/field_names.Plo
include ./$(DEPDIR)/field_types.Plo
include ./$(DEPDIR)/fields.Plo
include ./$(DEPDIR)/manip.Plo
include ./$(DEPDIR)/myset.Plo
include ./$(DEPDIR)/qparms.Plo
include ./$(DEPDIR)/query.Plo
include ./$(DEPDIR)/result.Plo
include ./$(DEPDIR)/row.Plo
include ./$(DEPDIR)/sql_string.Plo
include ./$(DEPDIR)/string_util.Plo
include ./$(DEPDIR)/type_info.Plo
include ./$(DEPDIR)/vallist.Plo

distclean-depend:
	-rm -rf ./$(DEPDIR)

.cpp.o:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<

.cpp.obj:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) -c -o $@ `cygpath -w $<`

.cpp.lo:
	source='$<' object='$@' libtool=yes \
	depfile='$(DEPDIR)/$*.Plo' tmpdepfile='$(DEPDIR)/$*.TPlo' \
	$(CXXDEPMODE) $(depcomp) \
	$(LTCXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<
CXXDEPMODE = depmode=gcc3

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:
	-rm -f libtool
uninstall-info-am:
pkgincludeHEADERS_INSTALL = $(INSTALL_HEADER)
install-pkgincludeHEADERS: $(pkginclude_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkgincludedir)
	@list='$(pkginclude_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(pkgincludeHEADERS_INSTALL) $$d$$p $(DESTDIR)$(pkgincludedir)/$$f"; \
	  $(pkgincludeHEADERS_INSTALL) $$d$$p $(DESTDIR)$(pkgincludedir)/$$f; \
	done

uninstall-pkgincludeHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(pkginclude_HEADERS)'; for p in $$list; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " rm -f $(DESTDIR)$(pkgincludedir)/$$f"; \
	  rm -f $(DESTDIR)$(pkgincludedir)/$$f; \
	done

ETAGS = etags
ETAGSFLAGS =

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)  $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$tags$$unique" \
	  || $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	     $$tags $$unique

GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

top_distdir = ..
distdir = $(top_distdir)/$(PACKAGE)-$(VERSION)

distdir: $(DISTFILES)
	@list='$(DISTFILES)'; for file in $$list; do \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    dir="/$$dir"; \
	    $(mkinstalldirs) "$(distdir)$$dir"; \
	  else \
	    dir=''; \
	  fi; \
	  if test -d $$d/$$file; then \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -pR $(srcdir)/$$file $(distdir)$$dir || exit 1; \
	    fi; \
	    cp -pR $$d/$$file $(distdir)$$dir || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
check: check-am
all-am: Makefile $(LTLIBRARIES) $(HEADERS) all-local

installdirs:
	$(mkinstalldirs) $(DESTDIR)$(libdir) $(DESTDIR)$(pkgincludedir)

install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  INSTALL_STRIP_FLAG=-s \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-am

clean-am: clean-generic clean-libLTLIBRARIES clean-libtool \
	mostlyclean-am

distclean: distclean-am

distclean-am: clean-am distclean-compile distclean-depend \
	distclean-generic distclean-libtool distclean-local \
	distclean-tags

dvi: dvi-am

dvi-am:

info: info-am

info-am:

install-data-am: install-pkgincludeHEADERS

install-exec-am: install-libLTLIBRARIES

install-info: install-info-am

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-am

maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic \
	mostlyclean-libtool

uninstall-am: uninstall-info-am uninstall-libLTLIBRARIES \
	uninstall-pkgincludeHEADERS

.PHONY: GTAGS all all-am all-local check check-am clean clean-generic \
	clean-libLTLIBRARIES clean-libtool distclean distclean-compile \
	distclean-depend distclean-generic distclean-libtool \
	distclean-local distclean-tags distdir dvi dvi-am info info-am \
	install install-am install-data install-data-am install-exec \
	install-exec-am install-info install-info-am \
	install-libLTLIBRARIES install-man install-pkgincludeHEADERS \
	install-strip installcheck installcheck-am installdirs \
	maintainer-clean maintainer-clean-generic mostlyclean \
	mostlyclean-compile mostlyclean-generic mostlyclean-libtool \
	tags uninstall uninstall-am uninstall-info-am \
	uninstall-libLTLIBRARIES uninstall-pkgincludeHEADERS


all-local: custom.h

custom.h custom-macros.h: custom.pl
	./custom.pl

distclean-local:
	rm -f custom.h custom-macros.h

tags: ctags

ctags:
	( cd .. && make ctags )

clean-docs:
	( cd ../doc/refman/html ; rm -f *.dot [a-z]*.html *.png doxygen.css )
	rm -rf ../doc/refman/latex

refman:
	doxygen
	( cd ../doc/refman/latex && make pdf && mv refman.pdf ../../mysqlpp-refman.pdf ) 
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
