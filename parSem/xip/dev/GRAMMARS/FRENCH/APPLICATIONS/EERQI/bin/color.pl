#!/usr/bin/perl -w

open (IN, "$ARGV[0].parse") || die ;
open (OUT, ">$ARGV[0].html") || die ;

while ($line = <IN>) {
    if ( $line =~ /^SENT/ || $line =~ /_OK/ || $line =~ /_NOISE/ || $line =~ /_SILENCE/ || $line =~ /POTKSDEP/ || $line =~ /^Reported/ || $line =~ /^Not reported/ ) {
	$line =~ s/\n/<br>/ ;
	$line =~ s/^(SENT.+)/$1<br>/ ;
	$line =~ s/_PINKW:\+/-PINKW/g ;
	$line =~ s/_BLUEW:\+/-BLUEW/g ;
        $line =~ s/_ORANGEW:\+/-ORANGEW/g ;
        $line =~ s/_VIOLETW:\+/-VIOLETW/g ;
	$line =~ s/_BOLDW:\+/-BOLDW/g ;
	$line =~ s/_YELLOWW:\+/-YELLOWW/g ;
	$line =~ s/_GREENW:\+/-GREENW/g ;

	if ( $line =~ /KSDEP_/ ) {
	    $line =~ s/-BLUEW//g ;
	}
	$line =~ s/_[A-Z]+:\+//g ;
#PINKW-BOLDW
	$line =~ s/([^\( ,]+)-PINKW-BOLDW/\<b\>\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\><\/b\>/g ;
#BLUEW-BOLDW
$line =~ s/([^\( ,]+)-BLUEW-BOLDW/\<b\>\<span style\=\'background\:aqua;\;mso\-highlight\:aqua;\'\>$1\<\/span\><\/b\>/g ;
#ORANGEW-BOLDW
$line =~ s/([^\( ,]+)-BLUEW-BOLDW/\<b\>\<span style\=\'background\:aqua;\;mso\-highlight\:aqua;\'\>$1\<\/span\><\/b\>/g ;
#VIOLETW-BOLDW
$line =~ s/([^\( ,]+)-BLUEW-BOLDW/\<b\>\<span style\=\'background\:aqua;\;mso\-highlight\:aquaw;\'\>$1\<\/span\><\/b\>/g ;
#GREENW-BOLDW
$line =~ s/([^\( ,]+)-BLUEW-BOLDW/\<b\>\<span style\=\'background\:#BEF574;\;mso\-highlight\:#BEF574;\'\>$1\<\/span\><\/b\>/g ;
#YELLOWW-BOLDW
$line =~ s/([^\( ,]+)-BLUEW-BOLDW/\<b\>\<span style\=\'background\:yellow;\;mso\-highlight\:yellow;\'\>$1\<\/span\><\/b\>/g ;

#BLUEW-PINKW
	$line =~ s/([^\( ,]+)-BLUEW-PINKW/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
#ORANGEW-PINKW
	$line =~ s/([^\( ,]+)-ORANGEW-PINKW/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
#VIOLETW-PINKW
	$line =~ s/([^\( ,]+)-VIOLETW-PINKW/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
#PINKW
$line =~ s/([^\( ,]+)-PINKW/\<span style\=\'background\:pink;\;mso\-highlight\:pink;\'\>$1\<\/span\>/g ;
#BOLDW
	$line =~ s/([^\( ,]+)-BOLDW/\<b\>$1<\/b\>/g ;
#BLUEW
	$line =~ s/([^\( ,]+)-BLUEW/\<span style\=\'background\:aqua;\;mso\-highlight\:aqua;\'\>$1\<\/span\>/g ;
	$line =~ s/(.)-BOLDW/\<b\>$1\<\/b\>/g ;
	$line =~ s/(.)-BLUEW/\<span style\=\'background\:aqua;\;mso\-highlight\:aqua;\'\>$1\<\/span\>/g ;
#ORANGEW
	$line =~ s/([^\( ,]+)-ORANGEW/\<span style\=\'background\:orange;\;mso\-highlight\:orange;\'\>$1\<\/span\>/g ;
	$line =~ s/(.)-BOLDW/\<b\>$1\<\/b\>/g ;
	$line =~ s/(.)-ORANGEW/\<span style\=\'background\:orange;\;mso\-highlight\:orange;\'\>$1\<\/span\>/g ;
#VIOLETW
	$line =~ s/([^\( ,]+)-VIOLETW/\<span style\=\'background\:violet;\;mso\-highlight\:violet;\'\>$1\<\/span\>/g ;
	$line =~ s/(.)-BOLDW/\<b\>$1\<\/b\>/g ;
	$line =~ s/(.)-VIOLETW/\<span style\=\'background\:violet;\;mso\-highlight\:violet;\'\>$1\<\/span\>/g ;
#GREENW
	$line =~ s/([^\( ,]+)-GREENW/\<span style\=\'background\:#BEF574;\;mso\-highlight\:#BEF574;\'\>$1\<\/span\>/g ;
	$line =~ s/(.)-BOLDW/\<b\>$1\<\/b\>/g ;
	$line =~ s/(.)-GREENW/\<span style\=\'background\:#BEF574;\;mso\-highlight\:#BEF574;\'\>$1\<\/span\>/g ;
#YELLOWW
	$line =~ s/([^\( ,]+)-YELLOWW/\<span style\=\'background\:yellow;\;mso\-highlight\:yellow;\'\>$1\<\/span\>/g ;
	$line =~ s/(.)-BOLDW/\<b\>$1\<\/b\>/g ;
	$line =~ s/(.)-YELLOWW/\<span style\=\'background\:yellow;\;mso\-highlight\:yellow;\'\>$1\<\/span\>/g ;

	print OUT $line ; }
}
