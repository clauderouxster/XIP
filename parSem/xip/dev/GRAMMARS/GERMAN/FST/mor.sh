#!/bin/sh
#


# directory of data files
LDATA=`pwd`/data


case `uname -psr` in
  Linux*) BINDIR=/opt/CA/bin/linux; ECHO="echo -e";;
  SunOS*) BINDIR=/opt/CA/bin; ECHO="echo";;
  *) echo "ERROR: Unsupported platform " `uname -psr` "." ; exit 1;;
esac


# -----------------------------------------------------------------
# default options:
UTFOPT=""

while [ $# -gt  0 ]
do 
 case $1 in
  "-utf8" ) UTFOPT="-utf8" ; shift ;;
   * )  shift ;;
esac
done

# ----------------------------------------------

$BINDIR/tokenize $UTFOPT $LDATA/tok.fst |\
$BINDIR/lookup $UTFOPT -flags cn -f $LDATA/lookup_abs.txt


