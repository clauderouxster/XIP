# =======================================================================
# TYPE:		gawk script
# CONTENT:	markup output of test disambiguation
#
# AUTHOR:	Anne Schiller
#
# DATE:		25-Oct-2002
#
# =======================================================================
# USAGE:	gawk -f [ThisFile]
#
# INPUT:	reformatted output of ana.sh -col2 -ambig 
#		1 "analysis block" per input token:
#		<POS> \b <TOK> \t <LEX1> \t <MOR1><MRK><HMM1> \t <SCORE1>
#		<POS> \b <TOK> \t <LEX2> \t <MOR2><MRK><HMM2> \t <SCORE2>
#		...
#		<POS> \b <TOK> \t <LEXn> \t <MORn><MRK><HMMn> \t <SCOREn>
#		\n
#	where:
#		\b  = separator (2 blanks)
#		POS = manual tag
#		sep = "   "
#		TOK = input token
#		LEX = lexical analysis part 1 (usually lemma)
#		MOR = morphological tags
#		MRK = lexicon marker ("+lexicon", "+guesser", "+open", ...)
#		HMM = POS tag form lexical FST
#		SCORE = score given by "disamb -a"
#
# OUTPUT:	same as input with additional fisrt column markup:
#	(a) on first line of analysis "block"
#	"=="	for correct analyis, i.e.
#		HMM1 == POS
#
#	"**"	for dismbiguation error, i.e.
#		(HMM1 != POS) and exists j with (HMMj==POS)
#
#	"##"	for lexical error (correct POS not in lexical FST), i.e.
#		(HMM1 != POS) and for all j (HMMj!=POS)
#
#	(b) on subsequent lines
#	"=+"	for more correct lines, i.e.
#		(HMMk == POS) and (HMM1 == POS1)
#	"=!"	for missed correct lines, i.e.
#		(HMMk == POS) and (HMM1 != POS1)
# =======================================================================




BEGIN{
  FS = "[	]" ;
}

{ #MAIN
  if (NF==0) 	# print out previous block
    { for (i=1; i<=cnt; i++) {print mrk[i] "\t" ana[i]} 
    printf "\n" ;
    cnt=0 ;
    }
  else 
    {
      cnt++; 
      
      POS=$1 ; sub(/  .*/, "", POS ) ;
      k=split($3, x, "+") ;
      HMM = ( "+" x[k] ) ;
      MRK0 = x[k-1] ;
      if (MRK0~/lex/ || MRK0~/guess/ || MRK0~/open/) {
            MRK = ( MRK0 ) 
	      }
      else { MRK = "lexicon" ; } ;
      ana[cnt]=( $0 "	" MRK );

      if (cnt==1) {
	if (POS==HMM) 	{ mrk[1]= "==" }
	else 		{ mrk[1]= "##" }
	}
 if (cnt>1) {
	if (POS==HMM) {
		if (mrk[1]=="==")  { mrk[cnt]= "=+" }
		else {mrk[1]="**" ; mrk[cnt]= "=!" }
		}
	 else { mrk[cnt]="  " }
	}
}
}
