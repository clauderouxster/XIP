#!/usr/bin/perl -w

open (IN, "lexicon_kgsom.xip") || die ;
open (OUT, ">add-feature_kgsom.xip") || die ;

while ($line = <IN>) {
    $line =~ s/Vocabulary/Tagging/ ;
