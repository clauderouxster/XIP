#!/usr/bin/perl -w

open (IN, "/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/EERQI/data/lex-rule_eerqi.xip") || die ;
open (OUT, ">/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/EERQI/data/rule-lex_eerqi.xip") || die ;

print OUT "Vocabulary:\n\n" ;
while ($line = <IN>) {
    $line =~ s/Vocabulary:// ;
    $line =~ s/^(.+)/1> $1/ ;
    $line =~ s/1> \//\// ;
    $line =~ s/1> \\/\\/ ;
    print OUT $line ;
}
