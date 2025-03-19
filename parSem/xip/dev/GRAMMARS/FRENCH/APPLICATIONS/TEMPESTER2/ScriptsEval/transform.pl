#!/opt/gnu/bin/perl -w

# auteur: caroline Brun
# script passant d'un ctm-ne au mauvais format
# vers un ctm-ne au bon format
# patch problemes esters

$count=0 ;
$balise_courante="" ;
$debut=0 ;
open(F,$ARGV[0]);
while($line=<F>){
  $toprint=1 ;
  if (($line=~/\[/) & ($line!~/\]/)){
	$debut=1 ;
	$count++ ;
	$balise_courante=$line;
	$balise_courante=~s/^.*\[//;
	$balise_courante=~s/\n//;
	$balise_courante="--" . $balise_courante . "--" . $count ;
	$toprint=0 ;
	}
  if ($line=~/\]\n/){
	$debut=0 ;
	$toprint=0 ;
	}
  if ($toprint==1){
	if ($debut==1){
		$coco=$line ;
		$coco=~s/\n//;
		$coco= $coco . $balise_courante . "\n" ;
		print $coco ;
		}else{print $line}
	}
}
