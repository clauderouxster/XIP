#! /opt/gnu/bin/perl

open (IN, "/home/sandor/PS/XIP/XIP-1/grammar/lexicon_bk.xip") || die ;
open (OUT, ">/home/sandor/PS/XIP/XIP-1/grammar/lexicon-rule_bk.xip") || die ;

while ($line = <IN>) {
    $line =~ s/^rather.+\n// ;
    $line =~ s/^contrast.+\n// ;
    $line =~ s/.+nohyph\=\+.+\n// ;
    $line =~ s/.+fchallenge\=\+.+\n// ;
    $line =~ s/.+forneg\=\+.+\n// ;
    $line =~ s/^(.+)/1\> $1/ ;
    $line =~ s/^1\> (Vocabulary)/$1/ ;
    $line =~ s/^1\> (\/\/)/$1/ ;
    print OUT $line ;
}
