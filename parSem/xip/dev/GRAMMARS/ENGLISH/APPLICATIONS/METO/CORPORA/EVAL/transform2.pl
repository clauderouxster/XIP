#!/opt/gnu/bin/perl -w

open(F,$ARGV[0]) ;

$stock=0;
$stocked="" ;
#$toto=0 ;
#$count=0;
while($line=<F>){

  if (($stock==0) & ($line!~"<par>")){
    $line=~s/\<bnc:title\>.*\<\/bnc:title\>//g;
    $line=~s/\<title\>.*\<\/title\>//g;
    print $line;
  }

  if($line=~"<par>"){
        $stock=1 ;
	print $line;
      }

  if(($stock==1) &($line!~"<par>") & ($line!~"</par>")){
    $stocked=$stocked . $line ;
    print $line;
  }

  if($line=~"</par>"){
        $toprint= "<par2>\n" . $stocked ."</par2>\n"  ;
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


