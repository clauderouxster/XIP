Summary: C++ wrapper for the MySQL C API
Name: mysql++
Version: 2.0.4
Release: 1
Copyright: LGPL
Group: Development/Databases
Requires: MySQL-shared
Source: http://tangentsoft.net/mysql++/releases/mysql++-%{version}.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot
%description
MySQL++ makes working with MySQL server queries as easy as working
with STL containers.  This package contains only the libraries needed to
run MySQL++-based programs.  If you are building your own MySQL++-based
programs, you also need to install the -devel package.

%package devel
Summary: MySQL++ developer files (headers, examples, etc.)
Group: Development/Databases
Requires: mysql++
%description devel
These are the files needed to compile MySQL++ based programs, plus
some sample code to get you started.  If you aren't building your own
programs, you probably don't need to install this package.

%package manuals
Summary: MySQL++ user and reference manuals
Group: Development/Databases
%description manuals
This is the MySQL++ documentation.  It's a separate RPM just because
it's so large, and it doesn't change with every release.

%prep
%setup -q
./configure --prefix=/usr --disable-examples --disable-dependency-tracking

%build
make RPM_OPT_FLAGS="$RPM_OPT_FLAGS"

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/lib
mkdir -p $RPM_BUILD_ROOT/usr/include
mkdir -p $RPM_BUILD_ROOT/usr/src/mysql++/examples
%{__make} install DESTDIR=$RPM_BUILD_ROOT
install -m644 examples/*.cpp examples/*.h $RPM_BUILD_ROOT/usr/src/mysql++/examples
install -m644 examples/Makefile.simple $RPM_BUILD_ROOT/usr/src/mysql++/examples/Makefile
cp examples/README doc/README.examples
cp LICENSE doc
rm $RPM_BUILD_ROOT/usr/lib/*.la
rm $RPM_BUILD_ROOT/usr/lib/*.so
rm $RPM_BUILD_ROOT/usr/lib/*.so.[0-9]

%clean
rm -rf $RPM_BUILD_ROOT
rm -f doc/README.examples doc/LICENSE

%post
	ln -sf /usr/lib/libmysqlpp.so.%{version} /usr/lib/libmysqlpp.so
	/sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%doc doc/LICENSE doc/README.mysql++

/usr/lib/libmysqlpp.*

%files devel
%defattr(-,root,root)
%doc doc/README.devel doc/README.examples

/usr/include/mysql++
/usr/src/mysql++/examples

%files manuals
%defattr(-,root,root)
%doc doc/*.pdf doc/refman doc/userman

%changelog
* Sat Apr 30 2005 Warren Young <mysqlpp@etr-usa.com> 1.7.34-1
- Split manuals out into their own sub-package.

* Thu Mar 10 2005 Warren Young <mysqlpp@etr-usa.com> 1.7.32-1
- Disabled building of examples, to speed RPM build.

* Fri Nov 05 2004 Warren Young <mysqlpp@etr-usa.com> 1.7.21-1
- Split out -devel subpackage, which now includes the examples

* Wed Aug 18 2004 Warren Young <mysqlpp@etr-usa.com> 1.7.11-1
- Removed examples from documentation.
- Limited documentation to just the generated files, not the sources.

* Wed Apr 16 2003 Tuan Hoang <tqhoang@bigfoot.com> 1.7.9-4
- Added gcc 3.2.2 patch.
- Packaged using Red Hat Linux 8.0 and 9.

* Thu Nov 14 2002 Tuan Hoang <tqhoang@bigfoot.com> 1.7.9-3
- Changed the version passed to libtool.

* Mon Oct 28 2002 Tuan Hoang <tqhoang@bigfoot.com> 1.7.9-2
- Updated the version numbering of the library to be 1.7.9.
- Packaged using Red Hat Linux 8.0.

* Thu Oct 17 2002 Philipp Berndt <philipp.berndt@gmx.net>
- packaged
