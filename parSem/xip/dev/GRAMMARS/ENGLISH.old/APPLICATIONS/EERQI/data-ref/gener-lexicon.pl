#! /opt/gnu/bin/perl

open (IN, "/home/sandor/vikef/XIP/XIP-3/lexicon_ref.xip") || die ;
open (OUT, ">/home/sandor/vikef/XIP/XIP-3/lexicon-rule_ref.xip") || die ;

while ($line = <IN>) {
    $line =~ s/^more.+\n// ;
    $line =~ s/^we.+\n// ;
    $line =~ s/^I.+\n// ;
    $line =~ s/^current.+\n// ;
    $line =~ s/^rather.+\n// ;
    $line =~ s/^contrast.+\n// ;
    $line =~ s/^like.+\n// ;
    $line =~ s/^promise.+\n// ;
    $line =~ s/^similarity.+\n// ;
    $line =~ s/.+good\=\+.+\n// ;
#    $line =~ s/.+nohyph\=\+.+\n// ;
    $line =~ s/.+fchallenge\=\+.+\n// ;
    $line =~ s/.+forneg\=\+.+\n// ;
    $line =~ s/^(.+)/1\> $1/ ;
    $line =~ s/^1\> (Vocabulary)/$1/ ;
    $line =~ s/^1\> (\/\/)/$1/ ;
    print OUT $line ;
}


