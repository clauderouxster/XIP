#!/usr/bin/perl

# color.pl inputfile keywordfile outputfile

# inputfile: XIP output for PubMed articles

# keywordfile: a list of keywords, one per line, to be used in
#             calculating article scores.  * can be used as a
#             wildcard.  Matching is case-insensitive if the pattern
#             is in lowercase, case-sensitive if the pattern contains
#             one or more capital letters.
#
#




use strict;

open(IN,  $ARGV[0]) || die;
my $keywords = loadkeywords($ARGV[1]) || die;
open(OUT, "|sort -k 3 -nr >$ARGV[2]") || die;
# Each line contains the info for one document, so sorting the lines
# with unix sort does what we want.  -k 3 means use the third word of
# the line (the score) as the sort key.

my $pmid = "";
my $sentence;
my $sentfeatures;
my $abstfeatures;
my $open;
my $close;
my $output;
my $score;

while (<IN> ) {
  last if ! /^Name=/;
}

while (1) {			# sentence loop
  chomp;
  last if $_ eq "";
  $sentence = $_;
  $_ = <IN>;
  $sentfeatures = "";
  while (/^([A-Z_]+)/) {
    $sentfeatures .= $1;
    $_ = <IN>;
  }
  die if ! /^\d+>/;
  $_ = <IN>;
  $_ = <IN> if /^int/;
  die if !/^$/;
  $_ = <IN>;
  die if !/^$/;
  $_ = <IN>;

  while (/^PMID:(\d+)  \*\*\*Abstract type: (.*)\*\*\*/) {
    die if $1 != $pmid;
    $abstfeatures .= "$2 ";
    $_ = <IN>;
  }

  if ($sentence =~ /^\d{8}$/) {
    if ($pmid) {
      print OUT "$pmid: score $score $abstfeatures<br>$output<br><br>\n";
    }
    $pmid = $sentence;
    $output = "";
    $abstfeatures = "";
    $score = 0;
  } else {
    $score += countkw($sentence, $keywords);
    $open = $close = "";
    if ($sentfeatures =~ /IMPSUMMARY/ & $sentfeatures =~ /_VIS/) {
      $open = "<span style='background:magenta;'>($sentfeatures) ";
      $close = "</span>";
      $abstfeatures = "VIS " . $abstfeatures if $sentfeatures =~ /_VIS/;
    } elsif ($sentfeatures =~ /IMPSUMMARY/ ) {
      $open = "<span style='background:#ddffdd;'>($sentfeatures) ";
      $close = "</span>";
      $abstfeatures = "VIS " . $abstfeatures if $sentfeatures =~ /_VIS/;
        } elsif ($sentfeatures =~ /IMPSENT/ & $sentfeatures =~ /_VIS/) {
      $open = "<span style='background:magenta;'>($sentfeatures) ";
      $close = "</span>";
      $abstfeatures = "VIS " . $abstfeatures if $sentfeatures =~ /_VIS/;
    } elsif ($sentfeatures =~ /IMPSENT/) {
      $open = "<span style='background:#33ccff;'>($sentfeatures) ";
      $close = "</span>";
      $abstfeatures = "VIS " . $abstfeatures if $sentfeatures =~ /_VIS/;
    } elsif ($sentfeatures =~ /SUMMARY|LOGIC/) {
      $open = "<span style='background:yellow;'>($sentfeatures) ";
      $close = "</span>";
    }
    $output .= "$open$sentence$close  ";
  }
}

die if $_ ne "";
$_ = <IN>;
die if !/^Variables:/;


##########################################

# Returns an array of arrays.  Each second-level array contains
# equivalent forms of a single keyword.
sub loadkeywords {
  my $file = shift;
  my @keywords;
  my $line;
  my $alternatives;
  my $i;
  open(F, $file) || die "Can't open $file: $!";
  while ($line = <F>) {
    chomp $line;
    next if $line =~ /^$/;     # skip blank line
    $alternatives = [];
    for (split(/\|/, $line)) {
      if (!/[A-Z]/) {           # case insensitive unless it contains a capital.
	$_ = '(?i)' . $_;
      }
      s/\*/\.\*\?/g;           # change * to .*? (shortest match wildcard)
      s/-/ - /g;               # accomodate XIP's splitting of hyphenated words
      s/ /\\s\+/g;             # change space to \s+
      $_ = '(?:^|[^A-Za-z])' .
	$_ .                   # surrounded by non-word, to avoid matching
	  '(?:$|[^A-Za-z])';  # a word that just contains the pattern

      # Precompile pattern.
      push(@$alternatives, qr/$_/);
    }
    push(@keywords, $alternatives);
  }
  close(F);
  \@keywords;
}

# second argument is of type returned by loadkeywords.
sub countkw {
  my $string = shift;
  my $keywords = shift;
  my $tokencount = 0;
  my $kwnum;
  my $alternative;
  my @matches;
  for $kwnum (0..$#$keywords) {
    for $alternative (@{$keywords->[$kwnum]}) {
      @matches = $string =~ /$alternative/g;
      if (@matches) {
	$tokencount += @matches;
      }
    }
  }
  return $tokencount;
}
