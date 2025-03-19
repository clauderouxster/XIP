#!/usr/bin/perl
# SemEvalFr pour les countriesFr
# OBJ   : cibler erreurs 
# input : les articles "alignes" (de reference et de xip) obtenus en decommantant la ligne 208 (env) dans eval.pl
# usage : debugFr_countries.pl <articles "alignes"> puis rediriger sur <detail_erreurs>

$fich1=$ARGV[0];
open (FICH, "<$fich1") ||die "erreur $!\n"; #en lecture.

$nbLit=0;
$nbPfp=0;
$nbPfe=0;


while ($line =<FICH>){
    if ($line=~/(document_\d+)\|([^\|]+)\|(.+)/){
        $numarticle=$1;
	$a_reading=$2;
	$r_reading=$3;
	
	if($a_reading ne $r_reading){
	   $new_a_reading="$numarticle $a_reading";
	
	   if($r_reading eq "literal"){
	     $lit_nonreconnus{$new_a_reading}=$r_reading;
	     $nbLit++;
	 }

	   if($r_reading eq "place-for-people"){
	    $memb_nonreconnus{$new_a_reading}=$r_reading;
	    $nbPfp++;
         }


           if($r_reading eq "place-for-event"){
	    $event_nonreconnus{$new_a_reading}=$r_reading;
	    $nbPfe++;
	 }

       }
  }
  }
print "**********************************************\n";
print "****** Literal non reconnus ($nbLit)**********\n";
print "**********************************************\n";

while (($xip,$semeval) = each (%lit_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }
print "**********************************************\n";
print "****** Place-for-people non reconnus ($nbPfp)**********\n";
print "**********************************************\n";

while (($xip,$semeval) = each (%memb_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }


print "**********************************************\n";
print "****** Place-for-event non reconnus ($nbPfe)**********\n";
print "**********************************************\n";
while (($xip,$semeval) = each (%event_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }



