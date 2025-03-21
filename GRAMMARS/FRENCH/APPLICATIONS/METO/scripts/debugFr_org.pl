#!/usr/bin/perl
# SemEvalFr pour les org
# OBJ   : cibler erreurs 
# input : les articles "alignes" (de reference et de xip) obtenus en decommantant la ligne 208 (env) dans eval.pl
# usage : debugFr_org.pl <articles "alignes"> puis rediriger sur <detail_erreurs>

$fich1=$ARGV[0];
open (FICH, "<$fich1") ||die "erreur $!\n"; #en lecture.

$nbLit=0;
$nbMemb=0;
$nbFac=0;
$nbName=0;

while ($line =<FICH>){
    if ($line=~/(article\d+)\|([^\|]+)\|(.+)/){
        $numarticle=$1;
	$a_reading=$2;
	$r_reading=$3;
	
	if($a_reading ne $r_reading){
	   $new_a_reading="$numarticle $a_reading";
	
	   if($r_reading eq "literal"){
	     $lit_nonreconnus{$new_a_reading}=$r_reading;
	     $nbLit++;
	 }

	   if($r_reading eq "organisation-for-members"){
	    $memb_nonreconnus{$new_a_reading}=$r_reading;
	    $nbMemb++;
         }

 	   if($r_reading eq "organisation-for-facility"){
	    $fac_nonreconnus{$new_a_reading}=$r_reading;
	    $nbFac++;
         }

           if($r_reading eq "object-for-name"){
	    $name_nonreconnus{$new_a_reading}=$r_reading;
	    $nbName++;
	 }

       }
  }
  }
print "**********************************************\n";
print "****** Literal non reconnus ($nblit)**********\n";
print "**********************************************\n";

while (($xip,$semeval) = each (%lit_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }
print "**********************************************\n";
print "****** Members non reconnus ($nbmemb)**********\n";
print "**********************************************\n";

while (($xip,$semeval) = each (%memb_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }


print "**********************************************\n";
print "****** Facility non reconnus ($nbfac)**********\n";
print "**********************************************\n";
while (($xip,$semeval) = each (%fac_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }

  }

print "**********************************************\n";
print "****** Name non reconnus ($nbname)**********\n";
print "**********************************************\n";
while (($xip,$semeval) = each (%name_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }


