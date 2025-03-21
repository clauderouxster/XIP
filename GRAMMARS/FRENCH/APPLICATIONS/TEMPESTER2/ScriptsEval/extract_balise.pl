#!/opt/gnu/bin/perl -w

## Auteur Caroline Brun
## ESTER: extrait les elements d'un type donné
## de la référence

$balise="amount" ;
$toprint=1 ; 
open(F,$ARGV[0]);

while($line=<F>){
	if (($line=~$balise) & ($line=~"begin")){
	$debut=1 ; $toprint=0 ;	
	
	}
	if (($line=~$balise) & ($line=~"end")){
	$debut=0 ; $toprint=0 ; print $line ;
	}
	if (($debut==1)){
	print $line ;
	}


}

