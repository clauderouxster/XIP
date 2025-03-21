#! /opt/gnu/bin/perl

open (IN, "$ARGV[0]") || die ;
open (OUT, ">temp1") || die ;

while ($line = <IN>) {
    $line =~ s/(\<DateCreated\>)\n/$1/ ;
    $line =~ s/(\<\/Year\>)\n/$1/ ;
    $line =~ s/(\<\/Month\>)\n/$1/ ;
    $line =~ s/(\<\/Day\>)\n/$1/ ;
    print OUT $line ;
}


open (IN, "temp1") || die ;
open (OUT, ">temp2") || die ;

while ($line = <IN>) {
    if ( $line =~ /\<DateCreated\>/ || $line =~ /\<PubmedArticle\>/ || $line =~ /\<PMID\>/ || $line =~ /\<ArticleTitle\>/ || $line =~ /\<AbstractText\>/ || $line =~ /\<\/PubmedArticle\>/ ) {
	print OUT $line ;
    }
}


open (IN, "temp2") || die ;
open (OUT, ">temp3") || die ;

while ($line = <IN>) {
    $line =~ s/\<Year\>(.+)\<\/Year\>            \<Month\>(.+)\<\/Month\>            \<Day\>(.+)\<\/Day\>/\<DATE\>Date of creation - $1\.$2\.$3\<\/DATE\>/ ;
    $line =~ s/\<DateCreated\>            // ;
    $line =~ s/        \<\/DateCreated\>// ;
    print OUT $line ;
}


open (IN, "temp3") || die ;
open (OUT, ">temp4") || die ;

print OUT "\<TOP\>\n" ;
while ($line = <IN>) {
    $line =~ s/\<PubmedArticle\>/£begdoc£end/g ;
    $line =~ s/\<PMID\>/£begPMID£end/g ;
    $line =~ s/\<DATE\>/£begDATE£end/g ;
    $line =~ s/\<ArticleTitle\>/£begtitle£end/g ;
    $line =~ s/\<AbstractText\>/£begabstract£end/g ;
    $line =~ s/\<\/PubmedArticle\>/£beg\/doc£end/g ;
    $line =~ s/\<\/PMID\>/£beg\/PMID£end/g ;
    $line =~ s/\<\/DATE\>/£beg\/DATE£end/g ;
    $line =~ s/\<\/ArticleTitle\>/£beg\/title£end/g ;
    $line =~ s/\<\/AbstractText\>/£beg\/abstract£end/g ;
    if ( $line =~ /£begdoc£end/ || $line =~ /£beg\/doc£end/ || $line =~ /£begPMID£end/ || $line =~ /£begDATE£end/ || $line =~ /£begtitle£end/ || $line =~ /£begabstract£end/ ) {
	$line =~ s/\</\&lt\;/g ;
	$line =~ s/\>/\&gt\;/g ;
	$line =~ s/\&/\&amp\;/g ;
	$line =~ s/£beg/\</g ;
	$line =~ s/£end/\>/g ;
	$line =~ s/\<DATE\>.+\n// ;
	print OUT $line ;
    }
}
print OUT "\n\<\/TOP\>\n" ;

open (IN, "temp4") || die ;
open (OUT, ">$ARGV[1]") || die ;

while ($line = <IN>) {
    $line =~ s/\: ([A-Z][^\.]+\.)(\<\/abstract\>)/\. \<lastsent\>$1\<\/lastsent\>$2/ ;
    $line =~ s/\. ([A-Z][^\.]+\.)(\<\/abstract\>)/\. \<lastsent\>$1\<\/lastsent\>$2/ ;
    print OUT $line ;
}


