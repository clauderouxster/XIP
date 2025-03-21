# evaluation of XIP chunker

# INPUT: 
# File 1: output of command xip -grm GRAMMAR.grm -tr -nm

# USAGE: gawk -f [this_file] INPUT.txt

BEGIN{
#  FS="[ {}]" ;
}

{
  gsub(/[{}]/, " & ");

  # sentence number
  SNUM = $1;
  sub(/>.*/, "", SNUM);

  for (i=1;i<=NF;i++){
#    if ($i~/.*\([0-9]+:[0-9]+\)/) {

      res = $i ; 
      printf "%s\n", res ;} ;
#  } ;
}

