#!/bin/sh
#usage:
#	echo "this is a test" | ./xipparse
#	./xipparse -text fileNameToParse


# set XIP_HOME to the installation directory of XIP
# set XIP_HOME to an absolute path if you want to launch xipparse from anywhere
XIP_HOME=`pwd`/../..

GRMFILE=$XIP_HOME/grammar/english/entity/gram_gen_entit.grm

$XIP_HOME/bin/solaris/xip -grm $GRMFILE -tr -f -t $1 $2 $3 $4 $5
