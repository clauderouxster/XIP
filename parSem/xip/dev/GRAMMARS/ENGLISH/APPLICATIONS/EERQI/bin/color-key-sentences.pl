#!/usr/bin/perl -w

open (IN, "$ARGV[0].parse") || die ;
open (OUT, ">$ARGV[0].html") || die ;


while ($line = <IN>) {

#    if ( $line =~ /CONCEPT/ || $line =~ /\>TOP/ || $line =~ /[A-Z]\(http/ || $line =~ /^MOD/ ) {
#	next ;
#    }
    if ( $line =~ /\>TOP/ || $line =~ /[A-Z]\(http/ || $line =~ /^MOD/ ) {
	next ;
    }

    $line =~ s/\n/\n<br><br>/ ;

    $line =~ s/(\-)_MD:\+/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
    $line =~ s/([a-zA-Z€àâáäãåöé\?]+)_MD:\+/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
    $line =~ s/([a-z€àâáäãåöé\?]+)_MD:\+/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
    $line =~ s/([A-Z]+)_MD:\+/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;


    $line =~ s/^(PROBLEM-SUMMARY.+)/\<span style\=\'background\:#BEF574;\;mso\-highlight\:#BEF574;\'\>$1\<\/span\>/ ;
    $line =~ s/^(PROBLEM.+)/\<span style\=\'background\:aqua;\;mso\-highlight\:aqua;\'\>$1\<\/span\>/ ;  
    $line =~ s/^(SUMMARY.+)/\<span style\=\'background\:yellow;\;mso\-highlight\:yellow;\'\>$1\<\/span\>/ ;

    print OUT $line ;
}
