#!/usr/bin/perl -w

open (IN, "$ARGV[0]-irms.parse") || die ;
open (OUT, ">$ARGV[0]-eval.html") || die ;


while ($line = <IN>) {
    $line =~ s/\n/\n<br>/ ;
$line =~s/([a-zA-Z]+)_TOPICWORD:\+/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
$line =~s/([a-z]+)_TOPICWORD:\+/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
$line =~s/([A-Z]+)_TOPICWORD:\+/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
#$line =~s/([A-Z]+[a-z]+)_TOPICWORD:\+/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
	$line =~s/^(IMPSENT.+)/\<span style\=\'background\:aqua;\;mso\-highlight\:aqua;\'\>$1\<\/span\>/ ;    
	$line =~s/^(IMPSUMMARY.+)/\<span style\=\'background\:#CFFCCA;\;mso\-highlight\:#CFFCCA;\'\>$1\<\/span\>/ ;
	$line =~s/^(SUMMARY.+)/\<span style\=\'background\:yellow;\;mso\-highlight\:yellow;\'\>$1\<\/span\>/ ;
    if ( $line=~/^SENT/ &  $line=~/\_TOPIC/ & $line =~/\_CONTRAST/ ) {
    $line =~ s/^(SENT.+)/\<span style\=\'background\:#9BC5E5;\;mso\-highlight\:#9BC5E5;\'\>$1\<\/span\>/ ;
    }
if ( $line =~ /^SENT/ & $line=~/\_TOPIC/ & $line=~/\_VIS/ ) {
    $line =~ s/^(SENT.+)/\<span style\=\'background\:#9BC5E5;\;mso\-highlight\:#9BC5E5;\'\>$1\<\/span\>/ ;
}
if ( $line =~ /^SENT/ & $line=~/\_TOPIC/ & $line=~/\_LOGIC/ ) {
    $line =~ s/^(SENT.+)/\<span style\=\'background\:#9BC5E5;\;mso\-highlight\:#9BC5E5;\'\>$1\<\/span\>/ ;
}
if ( $line =~ /^SENT/ & $line=~/\_TOPIC/ & $line=~/\_QUESTON/ ) {
    $line =~ s/^(SENT.+)/\<span style\=\'background\:#9BC5E5;\;mso\-highlight\:#9BC5E5;\'\>$1\<\/span\>/ ;
}
if ( $line =~ /^SENT/ & $line=~/\_TOPIC/ & $line=~/\_NEGATIVE/ ) {
    $line =~ s/^(SENT.+)/\<span style\=\'background\:#9BC5E5;\;mso\-highlight\:#9BC5E5;\'\>$1\<\/span\>/ ;
}
if ( $line =~ /^SENT/ & $line=~/\_TOPIC/ & $line=~/\_WEAKCONTR/ ) {
    $line =~ s/^(SENT.+)/\<span style\=\'background\:#9BC5E5;\;mso\-highlight\:#9BC5E5;\'\>$1\<\/span\>/ ;
}
if ( $line =~ /^SENT/ & $line=~/\_TOPIC/ & $line=~/\_NOSTAT/ ) {
    $line =~ s/^(SENT.+)/\<span style\=\'background\:#9BC5E5;\;mso\-highlight\:#9BC5E5;\'\>$1\<\/span\>/ ;
}
if ( $line =~ /^SENT/ & $line=~/\_TOPIC/ & $line=~/\_TEMPSTAT/ ) {
    $line =~ s/^(SENT.+)/\<span style\=\'background\:#9BC5E5;\;mso\-highlight\:#9BC5E5;\'\>$1\<\/span\>/ ;
}
    $line =~ s/IMPSENT[A-Z_]*//g ;
    $line =~ s/IMPSUMMARY[A-Z_]*//g ;
    $line =~ s/SUMMARY[A-Z_]*//g ;
    $line =~s/SENT_[A-Z_]*\(//g ;
    $line =~s/^SENT[A-Z_]*\(//g ;

    print OUT $line ;
}
