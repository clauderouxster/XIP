#!/usr/bin/perl -w

open (IN, "temp") || die ;
open (OUT, ">temp.txt") || die ;

while ($line = <IN>) {

    $line =~ s/^[^\(]+\(// ;
    $line =~ s/\_RM\:\+//g ;
    $line =~ s/\)$// ;
    $line =~ s/ \././g ;
    $line =~ s/ \,/,/g ;
    $line =~ s/ \;/;/g ;
    $line =~ s/ \:/:/g ;
    $line =~ s/ \!/!/g ;
    $line =~ s/ \?/?/g ;
    $line =~ s/ \)/)/g ;
    $line =~ s/\( /(/g ;
    print OUT $line ;
}


