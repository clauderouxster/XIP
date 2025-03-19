#!/opt/gnu/bin/perl -w

## Auteur Caroline Brun
## ESTER: conversion d'un fichier .stm (input ester)
## en fichier XML, analysable par XIP
## XIP n'analyse que le texte compris dans les balises <text></text>
## et insere les balises d'annotations d'EN au sein def ce texte
## UTILISATION: convert-stm2xml.pl input.stm > output.stm.xml
 

open(F,$ARGV[0]);

print("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n") ;
print ("<stm>\n") ;


while($line=<F>){



$line=~s/<([^>]+male)>/@\1@@<\/nontext>/;
$line=~s/<([^>]+unknown)>/@\1@@<\/nontext>/;
$line=~s/<([^>]+,)>/@\1@@<\/nontext>/;
$line=~s/ignore_time_segment_in_scoring/<nontext>ignore_time_segment_in_scoring<\/nontext>/g;

$line=~s/\[-mic\]/<micf\/>/g;
$line=~s/\[mic-\]/<mico\/>/g;
$line=~s/\[mic\]/<mic\/>/g;
$line=~s/\[i\]/<i\/>/g;
$line=~s/\[n\]/<n\/>/g;
$line=~s/\[b\]/<b\/>/g;
$line=~s/\[bb\]/<bb\/>/g;
$line=~s/\[pap\]/<pap\/>/g;
$line=~s/\[-pap\]/<papf\/>/g;
$line=~s/\[pap-\]/<papd\/>/g;
$line=~s/\[-b\]/<bf\/>/g;
$line=~s/\[b-\]/<bi\/>/g;
$line=~s/\[tx\]/<tx\/>/g;
$line=~s/\[pap\]/<pap\/>/g;
$line=~s/\[top\]/<top\/>/g;
$line=~s/\[-top\]/<topf\/>/g;
$line=~s/\[top-\]/<topo\/>/g;
$line=~s/\[-conv\]/<convf\/>/g;
$line=~s/\[conv-\]/<convo\/>/g;
$line=~s/\[-pap\]/<papf\/>/g;
$line=~s/\[pap-\]/<papo\/>/g;
$line=~s/\[-rire\]/<riref\/>/g;
$line=~s/\[rire-\]/<rireo\/>/g;
$line=~s/\[rire\]/<rire\/>/g;
$line=~s/\[-overlap\]/<overlapf\/>/g;
$line=~s/\[overlap-\]/<overlapo\/>/g;

$line=~s/\[-\"oui\"\] /<ouio\/>/g;
$line=~s/\[\"oui\"-\]/<ouif\/>/g;

$line=~s/\[r\]/<r\/>/g;
$line=~s/ euh | euh$/ <euh\/> /g;
$line=~s/\*/<star\/> /g;



#$line=~s/\[-mic\]/<nontext>\[-mic\]<\/nontext>/g;
#$line=~s/\[mic-\]/<nontext>\[mic-\]<\/nontext>/g;
#$line=~s/\[i\]/<nontext>\[i\]<\/nontext>/g;
#$line=~s/\[n\]/<nontext>\[n\]<\/nontext>/g;
#$line=~s/\[b\]/<nontext>\[b\]<\/nontext>/g;
#$line=~s/\[bb\]/<nontext>\[bb\]<\/nontext>/g;
#$line=~s/\[b-\]/<nontext>\[b-\]<\/nontext>/g;
#$line=~s/\[-b\]/<nontext>\[-b\]<\/nontext>/g; 
#$line=~s/\[tx\]/<nontext>\[tx\]<\/nontext>/g;
#$line=~s/\[pap\]/<nontext>\[pap\]<\/nontext>/g;
#$line=~s/\[-top\]/<nontext>\[-top\]<\/nontext>/g;
#$line=~s/\[top-\]/<nontext>\[top-\]<\/nontext>/g;
#$line=~s/\[-conv\]/<nontext>\[-conv\]<\/nontext>/g;
#$line=~s/\[conv-\]/<nontext>\[conv-\]<\/nontext>/g;
#$line=~s/\[-pap\]/<nontext>\[-pap\]<\/nontext>/g;
#$line=~s/\[pap-\]/<nontext>\[pap-\]<\/nontext>/g;
#$line=~s/\[-rire\]/<nontext>\[-rire\]<\/nontext>/g;
#$line=~s/\[rire-\]/<nontext>\[rire-\]<\/nontext>/g;


#$line=~s/\[n\]/<n\/>/g;
#$line=~s/\[b\]/<b\/>/g;
#$line=~s/\[bb\]/<bb\/>/g;
#$line=~s/\[pap\]/<pap\/>/g;
#$line=~s/\[-pap\]/<papf\/>/g;
#$line=~s/\[pap-\]/<papd\/>/g;
#$line=~s/\[-mic\]/<micf\/>/g;
#$line=~s/\[mic-\]/<micd\/>/g;
#$line=~s/\[-b\]/<bf\/>/g;
#$line=~s/\[b-\]/<bi\/>/g;

if ($line=~/^;;/){
   $line=~s/\n/<\/nontext>\n/;
}
#else {
# $line=~s/\n/<\/text>\n/;
#}
$line=~s/^/<nontext>/;
#$line=~s/<text> <\/text>//;
print $line;

}



###
print("</stm>\n");
 
