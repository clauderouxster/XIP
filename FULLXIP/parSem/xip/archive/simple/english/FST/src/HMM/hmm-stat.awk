# =======================================================================
# TYPE:		gawk script
# CONTENT:	compute statistics from marked tagger output
#
# AUTHOR:	Anne Schiller
#
# DATE:		25-Oct-2002
#
# =======================================================================
# USAGE:	gawk -f [ThisFile]
#
# INPUT:	marked reformatted output of ana.sh -col2 -ambig 
#	1 "analysis block" per input token:
#	<CODE> \t <POS> \t <TOK> \t <LEX1> \t <MRK> \t <HMM1> \t <SCORE1>
#	<CODE> \t <POS> \t <TOK> \t <LEX2> \t <MRK> \t <HMM2> \t <SCORE2>
#	...
#	<CODE> \t <POS> \t <TOK> \t <LEXn> \t <MRK> \t <HMMn> \t <SCOREn>
#	\n
# where:
#	POS = manual tag
#	TOK = input token
#	LEX = lexical analysis (usually lemma + morph tags)
#	MRK = lexicon marker ("#lex", "#guess", "#open", ...)
#	HMM = POS tag form lexical FST
#	SCORE = score given by "disamb -a"
#	CODE =
#		"==" : first correct analyis, i.e. HMM1==POS
#		"**" : dismbiguation error, i.e. HMM1!=d POS but some HMMj==POS
#		"##" : lexical error, i.e. HMMi!=POS for all i
#		"=+" : additional correct analyis, i.e. HMMk==POS and HMM1==POS1
#		"=!" : missed correct lines, i.e. HMMk==POS and HMM1!=POS1
# =======================================================================


BEGIN{
  FS = "[	]" ;
}

# ------------------------------------------------------------------------
# MAIN PART
{ 
if (NF>0) 
 {
   CODE=$1; MRK=$NF;
     count_code[CODE]++ ;
     count_mark[MRK]++ ;
     count_code_by_mark[MRK,CODE]++ ;
 }
}

# ------------------------------------------------------------------------
END { 
FORMAT1= "%-22s: %7d (%6.2f%%  of %-7s tokens) \n"
FORMAT0= "%-22s: %7d \n"

# compute and display totals
LexErr = count_code["##"]
TagErr = count_code["**"]+LexErr
TokAll = count_code["=="]+TagErr


printf FORMAT0, "Total tokens", TokAll
printf FORMAT1, "Total tagging errors", TagErr, TagErr*100/TokAll, "total"
printf FORMAT1, "Total lexical errors", LexErr, LexErr*100/TokAll, "total"
printf "\n"

for (mrk in count_mark) {
LexErr = count_code_by_mark[mrk,"##"]
TagErr = count_code_by_mark[mrk,"**"]+LexErr
TokPrt = count_code_by_mark[mrk,"=="]+TagErr

printf FORMAT1, mrk " tokens",         TokPrt, TokPrt*100/TokAll, "total"
printf FORMAT1, mrk " tagging errors", TagErr, TagErr*100/TokPrt, mrk
printf FORMAT1, mrk " lexical errors", LexErr, LexErr*100/TokPrt, mrk
printf "\n"
  
}
}
