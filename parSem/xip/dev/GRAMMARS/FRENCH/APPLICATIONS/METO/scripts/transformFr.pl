#####################################################################
# transformFr.pl
# script pour trier les articles selon leur lecture
# appel : perl transformFr.pl countriesFr.xml > countriesFr_lit.xml
#####################################################################



#!/opt/gnu/bin/perl -w

open(F,$ARGV[0]) ;
print "<\?xml version=\"1.0\" encoding=\"UTF-8\"\?> \n";
print "<\!DOCTYPE articles SYSTEM \"metoFr.dtd\"> \n";

print "<articles> \n";

$stock=0;
$stocked="" ;
$count=0;
while($line=<F>){

if($line=~"<Article "){
	$stock=1 ;
	$stocked=$stocked . $line ;
	}

if(($stock==1) &($line!~"<Article") & ($line!~"</Article>")){
   $stocked=$stocked . $line ;
 }



if($line=~"</Article>"){
	$toprint=$stocked . $line  ;
	$stocked ="" ;
	$stock=0 ;
	$tab[$count]=$toprint ;
	$count++ ;
	$toprint="";
}

}

foreach $elem (@tab) {
 #if ($elem=~"<location reading=\"literal\">"){
 #if ($elem=~"<location reading=\"metonymic\" metotype=\"place-for-people\">"){
 #if ($elem=~"<location reading=\"metonymic\" metotype=\"place-for-event\">"){
 if ($elem=~"<org reading=\"literal\">"){
 #if ($elem=~"<org reading=\"metonymic\" metotype=\"organisation-for-members\">"){
 #if ($elem=~"<org reading=\"metonymic\" metotype=\"organisation-for-facility\">"){
 #if ($elem=~"<org reading=\"metonymic\" metotype=\"object-for-name\">"){
print "$elem \n";
  }
else{
}
}
print "<\/articles>"; 


