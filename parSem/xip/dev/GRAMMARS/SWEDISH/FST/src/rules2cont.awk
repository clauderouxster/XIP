# conversion of inflection rules into lexc continuation classes

BEGIN{
    lexnum=0;
    classnum=0;
}

{
    # comment
    if ($0~/^#/) 
    {
	# do nothing
    }
    # inflectional features
    else if ($0~/^\$/)
    {
	# remove initial $ sign
	sub(/\$/,"") ;
	c1 = lexnum;
	# register continuation classes
	for (i=1;i<=NF;i++) {
	    feat=$i ;

	    lexnum++ ;
	    if (feat=="UNDEF") 
	    {
		cont[lexnum]=feat ;
	    }
	    else
	    {
		lexname = sprintf("lex%03d",lexnum) ;
		cont[lexnum]=lexname ;
		printf "LEXICON %s\t< \"[%s]\":0 >\t# ;\n", lexname, feat
	    }
	} 
    }

    # inflection classes
    else if (NF>0)
    {
	class= $1 ;
	affix= $2 ;
	remove= $3 ;
	
	classnum++;
	classname = sprintf("class%03d",classnum) ;
	printf "LEXICON %s\t\t!# %s -%s\n", classname, class, affix  ;
	for (i=3;i<=NF;i++) 
	{
	    k=c1+i-2;
	    if (cont[k]!="UNDEF") {
		if ($i=="=") { srf=""} else {srf=( "0:" $i)} ;
		if ($i!="-") {
		    printf "%s\t%s ;\n", srf, cont[k] ;
		}
	    }
	}
    }
}
