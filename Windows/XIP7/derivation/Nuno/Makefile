CXXFLAGS = -O3 -m32
CXX = c++
XIPFLAGS = -DXIPSTLAPI -DXIPSTLUSE -DXIPPYTHON
SNTFLAGS = -Wall -ansi -pedantic

XIPPATH =           /afs/l2f/projects/xip/xip-9.63-50
XIPLIBPATH =        $(XIPPATH)/lib/linux/sharedLib
XIPLIBPYPATH =      $(XIPPATH)/lib/linux/sharedLibPy
XIPINCLUDESPATH =   $(XIPPATH)/include
XIPINCLUDESPYTHON = /usr/include/python

XIPLIBFLAGS = -L$(XIPLIBPATH) -L$(XIPLIBPYPATH) -Wl -Bdynamic
XIPLIBS = -lxips -lm -lpthread -lz -lpython2.5

all: l2f_xip 

l2f_xip: l2f_xip.o
	$(CXX) -o $@ $^ $(XIPLIBFLAGS) $(XIPLIBS) -m32

l2f_xip.o: l2f_xip.cpp
	$(CXX) -c $(CXXFLAGS) $(XIPFLAGS) -I$(XIPINCLUDESPATH) -I$(XIPINCLUDESPYTHON) l2f_xip.cpp


clean:
	rm -f *~ *.o l2f_xip
