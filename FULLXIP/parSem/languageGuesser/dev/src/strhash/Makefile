#########################################################################
#
# file name:     Makefile
# purpose:       makefile for string hash library
# author:        Bruno Maximilian Schulze
# created:       24/09/96
# last modified: 25/09/96 (BMS)
#
#########################################################################
#
# (c) 1996 RXRC, Grenoble
#
#########################################################################

INC = ../include
LIB = ../lib

HASHLIBFILES = string2id.o

override CFLAGS += -I$(INC)

default: $(LIB)/libstrhash.a($(HASHLIBFILES))

(%.o): %.c
	$(QUANTIFY) \
	$(PURIFY) \
	$(CC) -c $(CFLAGS) $<
	$(AR) $(ARFLAGS) $@ $%
	rm $%

clean:
	rm -f *.o *.a

depend: 
	makedepend -I $(INC) *.c


$(LIB)/libstrhash.a:
	$(RANLIB) $(LIB)/libstrhash.a


# DO NOT DELETE THIS LINE -- make depend depends on it.

string2id.o: /usr/include/math.h /usr/include/stdio.h
string2id.o: /usr/include/sys/feature_tests.h /usr/include/string.h
string2id.o: ../include/string2id.h
