ncbi-keywords-to-abstracts.pl > pubmed.xml

pubmed2doc.pl pubmed.xml doc.xml

parse doc.xml > doc.parse

color.pl doc.parse $1 $2

rm doc*
rm pubmed.xml
rm temp*




