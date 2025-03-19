#!/usr/bin/perl
# SemEval
# OBJ   : cibler erreurs 
# input : les samps "alignes" (de reference et de xip) obtenus en decommantant la ligne 208 (env) dans eval.pl
# usage : debug.pl <samp "alignes"> puis rediriger sur <detail_erreurs>

$fich1=$ARGV[0];
open (FICH, "<$fich1") ||die "erreur $!\n";#en lecture.

$nblit=0;
$nbmemb=0;
$nbprod=0;
$nbfac=0;
$nbmixed=0;
$nbindex=0;
$nbname=0;

while ($line =<FICH>){
    if ($line=~/(samp\d+)\|([^\|]+)\|(.+)/){
        $numsamp=$1;
	$a_reading=$2;
	$r_reading=$3;
	
	if($a_reading ne $r_reading){
	   $new_a_reading="$numsamp $a_reading";
	
	   if($r_reading eq "literal"){
	     $lit_nonreconnus{$new_a_reading}=$r_reading;
	     $nblit++;
	 }

	   if($r_reading eq "organisation-for-members"){
	    $memb_nonreconnus{$new_a_reading}=$r_reading;
	    $nbmemb++;
         }

           if($r_reading eq "organisation-for-product"){
	    $prod_nonreconnus{$new_a_reading}=$r_reading;
	    $nbprod++;
	 }

           if($r_reading eq "organisation-for-facility"){
	    $fac_nonreconnus{$new_a_reading}=$r_reading;
	    $nbfac++;
	 }

          if($r_reading eq "mixed"){
	    $mixed_nonreconnus{$new_a_reading}=$r_reading;
	    $nbmixed++;
	 }

          if($r_reading eq "organisation-for-index"){
	    $index_nonreconnus{$new_a_reading}=$r_reading;
	    $nbindex++;
	 }

           if($r_reading eq "object-for-name"){
	    $name_nonreconnus{$new_a_reading}=$r_reading;
	    $nbname++;
	 }

       }
  }
  }

print "****** Literal non reconnus ($nblit)**********\n";
while (($xip,$semeval) = each (%lit_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }
print "****** Members non reconnus ($nbmemb)**********\n";
while (($xip,$semeval) = each (%memb_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }
print "****** Product non reconnus ($nbprod)**********\n";
while (($xip,$semeval) = each (%prod_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }
print "****** Facility non reconnus ($nbfac)**********\n";
while (($xip,$semeval) = each (%fac_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }
print "****** Mixed non reconnus ($nbmixed)**********\n";
while (($xip,$semeval) = each (%mixed_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }
print "****** Index non reconnus ($nbindex)**********\n";
while (($xip,$semeval) = each (%index_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }
print "****** Name non reconnus ($nbname)**********\n";
while (($xip,$semeval) = each (%name_nonreconnus)){
    print "$xip au lieu de:\t $semeval.\n\n\n";
  }


