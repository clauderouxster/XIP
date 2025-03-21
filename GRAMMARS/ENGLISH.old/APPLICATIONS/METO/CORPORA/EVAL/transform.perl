#!/opt/gnu/bin/perl -w

open(F,$ARGV[0]) ;

$stock=0;
$stocked="" ;
#$toto=0 ;
$count=0;
while($line=<F>){

if($line=~"<sample "){
	$stock=1 ;
	$stocked=$stocked . $line ;
	}


if(($stock==1) &($line!~"<sample") & ($line!~"</sample>")){
   $stocked=$stocked . $line ;
 }



if($line=~"</sample>"){
	$toprint=$stocked . $line  ;
	$stocked ="" ;
	$stock=0 ;
	$tab[$count]=$toprint ;
	$count++ ;
	$toprint="";
}

}

#foreach $elem (@tab) {
#  if ($elem=~/\"organisation\-for\-members\"/){
#  }else{
#  print $elem ;
#}
#}

foreach $elem (@tab) {
  #if ($elem=~/"samp120"|"samp408"/){
  if ($elem=~/"samp120"|"samp408"|"samp489"|"samp506"|"samp602"|"samp630"|"samp650"|"samp743"|"samp1321"|"samp1670"|"samp2258"|"samp2265"|"samp2425"|"samp2508"|"samp2687"|"samp2710"|"samp2742"/){
   print $elem ;
}
}
