# evaluation of XIP chunker

# INPUT: 
# File 1: output of command xip -grm LANG.grm -tr -nm

# USAGE: gawk -f deflist -f [this_file] INPUT.txt

BEGIN{
  FS="[ {}]" ;
  SNUM=0 ;

  true = (1==1);
  false = (1==2) ;
}

function evaldep(ares) {
    for (adep in dep) 
    {
	if (ares~adep) 
	{ 
	    return true ;
	}
    };
    return false
}


{
  if (NF==0) {} # empty line
  else if ($0~/^>[0-9]*.TOP/) 
  {
  # ignore
  } 
  else if ($0~/^[0-9]*> /) 
  {
      SNUM++ ;
      sent=$0;
      sub( /^[0-9]*> /, "", sent);
      printf "%04d:A##\t%s\n", SNUM, sent ;
  } 
   else {
# dependencies
     res = $0 ;
     if (evaldep(res))
     {
	 printf "%04d:dep\t%s\n", SNUM, res ;
     }
   } 
}

