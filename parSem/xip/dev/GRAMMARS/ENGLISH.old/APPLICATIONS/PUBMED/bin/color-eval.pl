#! /opt/gnu/bin/perl

# input: egrep "^PSDEP|\>TOP|^REL|LOGIC|Abstract type" 
# input: egrep "^IMP|>TOP|^SUMMARY|LOGIC|Abstract type|^EVAL"


open (IN, "$ARGV[0]") || die ;
open (OUT, ">temp1") || die ;

while ($line = <IN>) {
    $line =~ s/^(SUMMARY.+)\n/$1\#\#\#/ ;
    $line =~ s/^(IMP.+)\n/$1\#\#\#/ ;
#    $line =~ s/^(PSRELSENT.+)\n/$1\#\#\#/ ;
    $line =~ s/^(LOGIC.+)\n/$1\#\#\#/ ;
    $line =~ s/^(EVAL.+)\n/$1\#\#\#/ ;
    $line =~ s/^(EVAL)[\_A-Z]+\'/$1\'/ ;
    $line =~ s/^[0-9]+\>// ;
    print OUT $line ;
}

open (IN, "temp1") || die ;
open (OUT, ">temp2") || die ;

while ($line = <IN>) {
    $line =~ s/(.+\'pmid\$\:[0-9]+\')(.+\#\#\#)(.+)/$1\($3\)/ ;
    print OUT $line ;
}

open (IN, "temp2") || die ;
open (OUT, ">$ARGV[1]") || die ;

while ($line = <IN>) {
    $line =~ s/(TOP\{NP\{[0-9]+\}\})/\<br\>\<br\>$1/ ;
#    $line =~ s/^[0-9]+\>(TOP\{NP\{[0-9]+\}\})/\<br\>\<br\>$1/ ;
#    $line =~ s/^[0-9]+\>// ;
#    $line =~ s/[A-Z]+\_1\:\+\_SUBJMAT\:\+\{([^\}]+)\}+/\<span style\=\'background\:fuchsia;\;mso\-highlight\:fuchsia;\'\>$1\<\/span\>/g ;
#    $line =~ s/[A-Z]+\_2\:\+\_SUBJMAT\:\+\{([^\}]+)\}+/\<span style\=\'background\:aqua;\;mso\-highlight\:aqua;\'\>$1\<\/span\>/g ;
    $line =~ s/[A-Z]+\_LEVEL\:\+\{([^\}]+)\}+/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
#$line =~ s/[A-Z]+\_NEGWORD\:\+\{([^\}]+)\}+/\<span style\=\'background\:#CCCCCC;\;mso\-highlight\:#CCCCCC;\'\>$1\<\/span\>/g ;
#    $line =~ s/[A-Z]+\{//g ;
#    $line =~ s/\{//g ;
#    $line =~ s/\}//g ;
    if ( $line !~ /EVAL/ ) {
	$line =~ s/\'pmid\$\:[0-9]+\'//g ;
    }
#    $line =~ s/([a-zA-Z]+)\_1\:\+\_SUBJMAT\:\+/\<span style\=\'background\:fuchsia;\;mso\-highlight\:fuchsia;\'\>$1\<\/span\>/g ;
#    $line =~ s/([a-zA-Z]+)\_2\:\+\_SUBJMAT\:\+/\<span style\=\'background\:aqua;\;mso\-highlight\:aqua;\'\>$1\<\/span\>/g ;
    $line =~ s/([a-zA-Z]+)\_LEVEL\:\+/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
#$line =~ s/([a-zA-Z]+)\_NEGWORD\:\+/\<span style\=\'background\:#CCCCCC;\;mso\-highlight\:#CCCCCC;\'\>$1\<\/span\>/g ;
    $line =~ s/\_[A-Z]+\:\+//g ;
    $line =~ s/[A-Z]+\{//g ;
    $line =~ s/\{//g ;
    $line =~ s/\}//g ;
$line =~ s/(^IMPSENT.+)/\<br\>\<span style\=\'background\:#33CCFF;\;mso\-highlight\:#33CCFF;\'\>$1\<\/span\>/ ;
    $line =~ s/^(SUMMARY.+)/\<br\>\<span style\=\'background\:yellow;\;mso\-highlight\:yellow;\'\>$1\<\/span\>/ ;
    $line =~ s/^(LOGIC.+)/\<br\>\<span style\=\'background\:yellow;\;mso\-highlight\:yellow;\'\>$1\<\/span\>/ ;
    $line =~ s/^(EVAL.+)/\<br\>\<span style\=\'background\:fuchsia;\;mso\-highlight\:fuchsia;\'\>$1\<\/span\>/ ;
$line =~ s/(IMPSUMMARY.+)/\<span style\=\'background\:#ddffdd;\;mso\-highlight\:#ddffdd;\'\>$1\<\/span\>/ ;
    $line =~ s/\_VIS//g ;
    $line =~ s/\_SUBJMAT//g ;
    $line =~ s/\_[0-9]:\+//g ;
    $line =~ s/\_LEVEL//g ;
    $line =~ s/(INTRODUCTION \.)/\<br\>$1\<br\>/ ;
    $line =~ s/(DISCUSSION \.)/\<br\>$1\<br\>/ ;
    $line =~ s/\#\#\#.+/\<\/span\>/ ;
    $line =~ s/(PMID:[0-9]+ +\*\*\*Abstract type)/\<br\>\<br\>$1/g ;
    $line =~ s/(EVAL.+)/\<br\>\<br\>$1/g ;
    $line =~ s/NOUN//g ;
    print OUT $line ;
}


