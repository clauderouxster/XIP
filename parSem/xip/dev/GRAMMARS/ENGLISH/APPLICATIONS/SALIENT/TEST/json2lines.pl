#!/usr/bin/perl -w

open (IN, "$ARGV[0]") || die ;
open (OUT, ">$ARGV[0]-lines") || die ;

while ($line = <IN>) {
    $line =~ s/,\"/,\n"/g ;
    $line =~ s/,\{/,\n{/g ;
    print OUT $line ;
}
