# ---------------------------------------------------------------------
# TYPE:		gawk script
# CONTENT:	convert german morphology DB in lexc format
# AUTHOR:	Anne Schiller, XRCE
#
# DATE:		28-Nov-2002
# ---------------------------------------------------------------------
# USAGE:	gawk -f [ThisFile] < BD.txt 
# ---------------------------------------------------------------------

# ---------------------------------------------------------------------
# INPUT FORMAT:
# Line 	::= Spell \t Lemma \t Cont (\t Info)
#
# Spell		::= " " | "[new]" | "[old]" | "[err]" 
# Lemma		::= Form
# Cont  	::= Cont1 [ "|" Cont1 ]*	
# Info	 	::= Form [ "|" Form ]*	
#
# Pair	::= Class (":" Surf )
# Form	::= Char+
# ---------------------------------------------------------------------

# ---------------------------------------------------------------------
BEGIN{
	FS="[	]+"

}

# ---------------------------------------------------------------------
{ #MAIN
if ($0!~/^\#/ && NF>2) {

# columns:
	spell0=$1	# spelling variant
	lemma=$2	# base form
	contlist=$3	# continuations 

# convert spell marker into flag diacritic
	spell=spell0; sub( /[0-9]/ , "", spell )
#	sub( /\[/,  "@U.spell.", spell )
#	sub( /\]/,  "@",	 spell )

# convert verb control flags
	sub( /\[PZG\]/ , "[P][Z][G]", lemma )
	sub( /\[PZ\]/  , "[P][Z]", lemma )
	sub( /\[ZG\]/  , "[Z][G]", lemma )
	sub( /\[PG\]/  , "[P][G]", lemma )

# convert some diacritics in base form	
	gsub(/\\/,  "@C.un@", lemma )	# adjectival prefix "un"

# cut base form (for irregular inflections)
	stem = lemma ; 
	sub( /\!.*/, "", stem )		# eg. be!kommen => be

# base form + feature (for regular inflections and derivations)
	full = lemma
	sub( /\!/, "", full )

# create special entry for verbs with separable prefixes
# source:	nach[P][Z][G]ahmen+Verb V_reg ;
# new line:	[S]ahmen+Verb V_reg;
	stem2=""; full2=""
	if (spell0!="[old1]") {
	if (stem~/\[P\]/) {stem2=stem ; gsub(/^.*\]/,"[S]",stem2) }
	if (full~/\[P\]/) {full2=full ; gsub(/^.*\]/,"[S]",full2) }
	}

# all continuations (e.g. "Nf_en|}Dim:^$" )

  num=split( contlist, cont, "|")
  for (i=1; i<=num; i++) {
	spell1=spell
	if (cont[i]~/[{}]/) {
		ctype="deriv" 
		if (spell0="[old1]") {spell1=""} 
		} 
	else {ctype="flex"}

	if (cont[i]~/:/) {	
		split( cont[i], x, ":" )
		printf "%s%s%s\t%s ;\n", spell1, full, x[2], x[1]
		}
	else if (cont[i]~/\./)  {	# irregular inflection
		printf "%s%s\t%s ;\n", spell1, stem, cont[i]
		if (stem2!="") 
		   { printf "%s%s\t%s ;\n", spell1, stem2, cont[i] }
		}
	else 	{ 			# regular inflection, or derivation
		printf "%s%s\t%s ;\n", spell1, full, cont[i]
		if (ctype=="flex" && full2!="") 
		   { printf "%s%s\t%s ;\n", spell1, full2, cont[i] }
		}
		
	}
}
}
