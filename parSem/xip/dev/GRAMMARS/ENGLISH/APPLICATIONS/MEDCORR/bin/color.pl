#!/usr/bin/perl -w

#open (IN, "all-hypotheses.parse") || die ;
open (IN, "$ARGV[0].parse") || die ;
open (OUT, ">temp") || die ;

while ($line = <IN>) {
    $line =~ s/\n/\£/ ;
    print OUT $line ;
}


open (IN, "temp") || die ;
open (OUT, ">temp2") || die ;

while ($line = <IN>) {
    $line =~ s/££SWAN/\n\nSWAN/g ;
    $line =~s/\£([0-9]+> )/\n$1/g ;
    $line =~s/\£\n/\n\n/g ;
    print OUT $line ;
}


open (IN, "temp2") || die ;
#open (OUT, ">all.html") || die ;
open (OUT, ">$ARGV[0].html") || die ;

while ($line = <IN>) {
    $line =~s/^.+\£// ;
    $line =~s/^[0-9]+> // ;
    $line =~ s/\n/\n<br>/ ;
$line =~s/^(.+_VIS.+)/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/ ;
    $line =~ s/^(IMPSUMMARY.+)/\<span style\=\'background\:#BEF574;\;mso\-highlight\:#BEF574;\'\>$1\<\/span\>/ ;
$line =~s/^(SUMMARY.+)/\<span style\=\'background\:yellow;\;mso\-highlight\:yellow;\'\>$1\<\/span\>/ ;
$line =~s/^(IMPSENT.+)/\<span style\=\'background\:aqua;\;mso\-highlight\:aqua;\'\>$1\<\/span\>/ ;
    $line =~ s/IMPSENT_// ;
    $line =~ s/IMPSUMMARY_// ;
    $line =~ s/SUMMARY_// ;
    $line =~ s/LASTSENT_// ;
    $line =~ s/WEAKCONTR/CONTR/g ;
    $line =~ s/STATKNOW//g ;
#    $line =~ s/^SWAN/<br><br>SWAN/g ;
    $line =~ s/(SWAN.+No\.[0-9]+\.)(.+)/$1 \<b\>$2\<\/b\>/g ;
    print OUT $line ;
}
