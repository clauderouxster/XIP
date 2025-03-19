#! /opt/gnu/bin/perl

open (IN, "$ARGV[0]") || die ;
open (OUT, ">$ARGV[1]") || die ;

while ($line = <IN>) {
    $line =~ s/^(file .+)/\<br\>\<br\>\#\#\# $1 \#\#\#\<br\>\<br\>/ ;
    $line =~ s/(^RELTIME_NOTIFICATION_RENEWAL.+)/\<br\>\<span style\=\'background\:#ddffdd;\;mso\-highlight\:#ddffdd;\'\>$1\<\/span\>/;
    $line =~ s/(^RELTIME_NOTIFICATION.+)/\<br\>\<span style\=\'background\:yellow;\;mso\-highlight\:yellow;\'\>$1\<\/span\>/;
    $line =~ s/(^RELTIME_RENEWAL.+)/\<br\>\<span style\=\'background\:aqua;\;mso\-highlight\:aqua;\'\>$1\<\/span\>/;
    $line =~ s/(^NOTETIME.+)/\<br\>\<span style\=\'background\:yellow;\;mso\-highlight\:yellow;\'\>$1\<\/span\>/ ;
    $line =~ s/(^RENEWTIME.+)/\<br\>\<span style\=\'background\:aqua;\;mso\-highlight\:aqua;\'\>$1\<\/span\>/;
    $line =~ s/(^LOCORG.+)/\<br\>\<span style\=\'background\:magenta;\;mso\-highlight\:magenta;\'\>$1\<\/span\>/ ;
    $line =~ s/(^LOCATION.+)/\<br\>\<span style\=\'background\:magenta;\;mso\-highlight\:magenta;\'\>$1\<\/span\>/ ;
    $line =~ s/(^ORG.+)/\<br\>\<span style\=\'background\:magenta;\;mso\-highlight\:magenta;\'\>$1\<\/span\>/ ;
    $line =~ s/(^DATE.+)/\<br\>\<span style\=\'background\:magenta;\;mso\-highlight\:magenta;\'\>$1\<\/span\>/ ;
    $line =~ s/(^EMAIL.+)/\<br\>\<span style\=\'background\:magenta;\;mso\-highlight\:magenta;\'\>$1\<\/span\>/ ;
    
    if ( $line =~ /RELTIME/ | $line =~ /NOTETIME/ | $line =~ /RENEWTIME/ | $line =~ /^SENT\(/ | $line =~ /\#\#\#/ | $line =~ /\>LOC/ | $line =~ /\>ORG/ | $line =~ /\>DATE/ | $line =~ /\>EMAIL/ ) {
	$line =~ s/^SENT\(.*(file - .+)\.\)/\<br\>\<br\>\#\#\# $1 \#\#\#\<br\>\<br\>/ ;
	$line =~ s/^SENT\((.+)\)/\<br\>$1/ ;
	$line =~ s/RELTIME_// ;
	$line =~ s/\'docid\$\:0\'// ;
	$line =~ s/SOMEONE//g ;
	print OUT $line ;
    }
    
   if ( $line =~ /^Ambiguity degree for/ | $line =~ /^No information on/ ) {
	print OUT "<br>";
	print OUT $line;
	}

}

