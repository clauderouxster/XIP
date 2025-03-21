#############################################################################
# transform2Fr.pl
# script pour créer un paragraphe 2 avec les balises <annotorg> et <annotloc>
# appel : perl transform2Fr.pl countriesFr_lit.xml > countriesFr_lit2ble.xml
#############################################################################

#!/opt/gnu/bin/perl -w

open(F,$ARGV[0]) ;

$stock=0;
$stocked="" ;
#$toto=0 ;
#$count=0;
while($line=<F>){

  if (($stock==0) & ($line!~"<Body>")){
    $line=~s/\<bnc:title\>.*\<\/bnc:title\>//g;
    $line=~s/\<title\>.*\<\/title\>//g;
    
    print $line;
  }

  if($line=~"<Body>"){
        $stock=1 ;
	
	print $line;
      }

  if(($stock==1) &($line!~"<Body>") & ($line!~"</Body>")){
  #  $line=~s/([A-Z^\b]+)/$1\./g;
    $stocked=$stocked . $line ;
    print "$line";
  }

  if($line=~"</Body>"){
        $toprint= "<Body2>\n" . $stocked ."</Body2>\n"  ;
        $stocked ="" ;
        $stock=0 ;
        print $line ;
	##LOCATION
        $toprint=~s/<location [^\>]+>//g ;
        $toprint=~s/<\/location>//g;
        $toprint=~s/<annot\>/<annotloc\>/g ;
        $toprint=~s/<\/annot\>/<\/annotloc\>/g ;
	##ORG
        #$toprint=~s/<org [^\>]+>//g ;
        #$toprint=~s/<\/org>//g;
        #$toprint=~s/<annot\>/<annotorg\>/g ;
        #$toprint=~s/<\/annot\>/<\/annotorg\>/g ;

       print $toprint;
     }
}


