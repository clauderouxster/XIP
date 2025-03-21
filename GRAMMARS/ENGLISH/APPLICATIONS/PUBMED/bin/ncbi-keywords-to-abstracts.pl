#!/opt/gnu/bin/perl

# have to use the /opt/gnu version because the /usr/bin version
# doesn't have libwww installed.

###############################################################################
#
# Retrieve PubMed abstracts matching a query
# Aaron Kaplan, Xerox Research Center Europe, May 2003
#
#
# Quoted from
# http://www.ncbi.nlm.nih.gov:80/entrez/query/static/eutils_help.html :
#
# Do not overload NCBI's systems. Users intending to send numerous
# queries and/or retrieve large numbers of records from Entrez should
# comply with the following: 
#
#     - Run retrieval scripts on weekends or between 9 PM and 5 AM ET
#       weekdays for any series of more than 100 requests.
#
#     - Make no more than one request every 3 seconds.
#
#     - NCBI's Disclaimer and Copyright notice must be evident to
#       users of your service.  NLM does not hold the copyright on the
#       PubMed abstracts the journal publishers do. NLM provides no
#       legal advice concerning distribution of copyrighted materials,
#       consult your legal counsel.
#
#
# The above-mentioned disclaimer and copyright notice can be found at
# http://www.ncbi.nlm.nih.gov:80/About/disclaimer.html .
#
###############################################################################


use strict;
use LWP::UserAgent;

##########################
#
# Configuration variables
#
##########################


my $query = 'antioxid* [Text Word] OR (protein [Text Word] AND (degrad* [Text Word] OR aggregat* [Text Word]))';


my $daysold = 1*365;
#my $max_hits = 10000;
my $max_hits = 50;


my $outputfile = 'abstracts.xml';

my $proxy = 'http://cornillon.grenoble.xrce.xerox.com:8000/';
my $server_base = 'http://www.ncbi.nlm.nih.gov/entrez/eutils';

# Note: the information in $tool and $email are sent to the PubMed
# server.  This is a courtesy that lets them know who's making these
# automated queries and allows them to contact us if there's a
# problem, e.g. if we are putting an unreasonable load on their
# server.
my $tool = 'XRCE-corpus-builder';
my $email = 'Aaron.Kaplan@xrce.xerox.com';

my $header0 = "<?xml version=\"1.0\"?>\n";
my $header1 = "<!DOCTYPE PubmedArticleSet PUBLIC \"-//NLM//DTD PubMedArticle, 1st November 2003//EN\" \"http://www.ncbi.nlm.nih.gov/entrez/query/DTD/pubmed_031101.dtd\">\n";
my $header2 = "<PubmedArticleSet>\n";
my $footer = "</PubmedArticleSet>\n";
my @filterlines;
$filterlines[0] = $header0;
$filterlines[1] = $header1;
$filterlines[2] = $header2;
$filterlines[3] = $footer;


###########################
#
# End config
#
###########################

#die "$outputfile already exists; rename it and try again." if (-e $outputfile);
#die "Can't create $outputfile because directory is not writeable." if (! -w '.');

my $ua = LWP::UserAgent->new;
$ua->proxy('http' => $proxy) if $proxy;

my $response = $ua->get($server_base . "/esearch.fcgi?" .
			"db=pubmed&" .
			"usehistory=y&" .
			"tool=$tool&" .
			"email=$email&" .
			"term=$query&" .
			"reldate=$daysold&" .
			"retmax=$max_hits");
if (! $response->is_success) {
  die "HTTP request for search failed: ", $response->status_line, "\n";
}

sleep 3;

# This line copied from eutils_example.pl by Oleg Khovayko
$response->content =~
  m|<Count>(\d+)</Count>.*<QueryKey>(\d+)</QueryKey>.*<WebEnv>(\S+)</WebEnv>|s;

my $Count    = $1;
my $QueryKey = $2;
my $WebEnv   = $3;

print STDERR "Got $Count hits.  ";

if ($Count > $max_hits) {
  $Count = $max_hits;
}

print STDERR "Retrieving $Count...\n";

# When I try to retrieve thousands of articles in a single request, it
# gets 3000 or so and then times out.  So break it into chunks.

my $buffer = "";

sub callback {
  my ($data, $response, $protocol) = @_;
  my $line;
  my $filter;
  $buffer .= $data;
 LOOP: while ($buffer =~ /^.*?\n/) {
    $line = $&;
    $buffer = $';
    for $filter (@filterlines) {
      if ($line eq $filter) {
	next LOOP; # drop this line on the floor
      }
    }
    print $line;
  }
}



my $fetched;
my $chunk = 1000;
print $header0;
print $header1;
print $header2;
for ($fetched = 0; $fetched < $Count; $fetched += $chunk) {
  # we have an old version of the LWP package whose get function doesn't
  # allow me to specify a filename for the response content, so I
  # explicitly build a request object and use $ua->request.
  my $req = HTTP::Request->new('GET',$server_base . "/efetch.fcgi?" .
			       "db=pubmed" .
			       "&WebEnv=$WebEnv" .
			       "&query_key=$QueryKey" .
			       "&tool=$tool" .
			       "&email=$email" .
			       "&retmode=xml" .
			       "&rettype=abstract".
			       "&retmax=" . ($chunk > $Count-$fetched ? $Count-$fetched : $chunk).
			       "&retstart=$fetched"
			      );
#  $response = $ua->request($req, "$outputfile.$fetched");
  $response = $ua->request($req, \&callback);
  die "result didn't end with newline" if $buffer;
  sleep 3;
}
print $footer;

if (! $response->is_success) {
  die "HTTP request for abstracts failed: ", $response->status_line, "\n";
}
