#!/opt/gnu/bin/perl -w

## Auteur : Maud Ehrmann

## ESTER :
## post gestion des dependances pour faire une sortie
## correpondant aux guidelines d'ester.

## Certaines balises contiennent un élément à mettre hors de la balise.
## Ce script intervient avant le script deconvert.pl



open(F,$ARGV[0]);

while($line=<F>){


	$line=~s/<date-relative a_enlever="(.+)">(.+)\1<\/date-relative>/<date-relative>$2<\/date-relative>/g;

	print $line ;
}
