# ----------------------------------------------------------------
# INPUT: xip lexicon
#
# e.g. 
# Emiratos Árabes Unidos  :       noun  += [ country:+] .
# República del África Central :  noun  += [ country:+] .
# Bahréin  :      noun  += [ country:+] .
# Bután  :        noun  += [ country:+] .
#
# ----------------------------------------------------------------
# OUTPUT : lexc entries
#
# e.g.
# Emiratos% Árabes% Unidos+Prop+Coubntry	MSG; 
# República% del% África% Central+Prop+Country	XSG;
# Bahréin+Prop+Coubntry		XSG; 
# Bután+Prop+Coubntry		XSG; 
#
# ----------------------------------------------------------------
# NOTE: continuation classe must be corrected manually !

# USAGE: gawk -v class="Cont" -v add="+Feat" -f [ThisFile]
BEGIN{
    
    if (class=="") {class = "XSG"; }	# default continuation class

}

function tocap(astr)
{
    tmp = (toupper(substr(astr,1,1)) substr(astr,2) ) ;
    return tmp;
}

function map(afeat)
{
    tmp = ("+" tocap(afeat) );
    return tmp;
}



# ===============================================================
# MAIN PART
{
    if ($0~/^\/\/[ =-]/ ) # comment
    {
	printf "!%s\n", $0
    }
    else if (NF<2) 
    {
	print
    }
    else 
    {
# lemma
	comm="";
	k=index($0,":") ;
	lemma = substr($0,1,k-1);
	if (lemma~/^\/\//) { comm="!" ;}
	sub(/^[	 "/]*/, "", lemma);	# strip off leading spaces, slashes and quotes
	sub(/[ 	"]*$/, "", lemma);	# strip off trailing spaces and quotes
	gsub(/ /,"% ", lemma);		# replace word internal spaces
	lemma = (comm lemma) ;

# features
	rest =  substr($0,k+1);
	sub(/^[	 ]*/, "", rest);	# strip off leading spaces
	k2=index(rest," ") ;
	main=substr(rest,1,k2-1);

# (a) main tag
	feat=map(main);

# (b) other tags
	k=index($0,"[") ;
	tags = substr($0, k+1 );
	sub(/\].*/, "", tags);
	gsub(/ /,"", tags);
	gsub(/[:=]\+/,"", tags);

	n=split(tags,x,",");
	for (i=1;i<=n;i++) 
	{
	feat = (feat map(x[i]) ) ;
	}
    

# print
	printf "%s%s\t\t%s;\n", lemma, feat add, class ;
    }
}



