#!/bin/sh
#


# diractory of data files
LDATA=$ROOT_XIP/SPANISH/FST/data


case `uname -psr` in
  Linux*) BINDIR=/opt/CA/bin/linux; ECHO="echo -e";;
  SunOS*) BINDIR=/opt/CA/bin; ECHO="echo";;
  *) echo "ERROR: Unsupported platform " `uname -psr` "." ; exit 1;;
esac

# -----------------------------------------------------------------
# default options:
DISAMBOPT=""
UTFOPT=""

while [ $# -gt  0 ]
do 
 case $1 in
  "-s" ) DISAMBOPT="-abi1+" ; 	shift ;;
  "-utf8" ) UTFOPT="-utf8" ; shift ;;
   * )  shift ;;
esac
done


# ----------------------------------------------

$BINDIR/tokenize $UTFOPT $LDATA/tok.fst |\
$BINDIR/lookup $UTFOPT -flags cn -f $LDATA/lookup_abs.txt |\
$BINDIR/disamb $DISAMBOPT $LDATA/hmm4xip.txt
