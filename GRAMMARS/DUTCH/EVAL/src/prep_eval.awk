# evaluation of XIP chunker

# INPUT: 
# File 1: output of command xip -grm LANG.grm -tr -nm

# USAGE: gawk -f [this_file] INPUT.txt

BEGIN{
  FS="[ {}]" ;
  SNUM=0 ;
  ins = sprintf("(%04d, ", SNUM ) ;
}

{
  if (NF==0) {} # empty line
  else if ($0~/^[0-9]*.TOP/) {
# chunks
    for (i=2;i<=NF;i++){
      if ($i~/.*\([0-9]+:[0-9]+\)/) {
	res = $i ; 
	sub( /[(]/, ins, res );
	printf "chunk:%s\n", res ;
      } ;
    } ;
    SNUM++ ;
    ins = sprintf("(%04d, ", SNUM ) ;
   } 
   else {
# dependencies
     res = $0 ;
     sub( /[(]/, ins, res );
     printf "dep:%s\n", res ;
   } 
}

