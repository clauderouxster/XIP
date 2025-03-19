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

# USAGE: gawk -v main="Cat" class="Cont" add="+Feat" -f [ThisFile]
BEGIN{
    
    if (main=="") { main = "Prop"; } 	# main category
    if (class=="") {class = "XSG"; }	# default continuation class

}

function map(afeat)
{
    tmp = ("+" toupper(substr(afeat,1,1)) substr(afeat,2) ) ;
    return tmp;
}


# ===============================================================
# MAIN PART
{
    if ( $0~/\+ *=/ && $1!~/^\/\// ) {

# fall back strategy for missing ":" after lemma
	sub(/[	 ]noun/,": noun");

# lemma
	k=index($0,":") ;
	lemma = substr($0,1,k-1);
	sub(/^[	 ]*/, "", lemma);	# strip off leading spaces
	sub(/[ 	]*$/, "", lemma);	# strip off trailing spaces
	gsub(/ /,"% ", lemma);	# replace word internal spaces

# features
# (a) main tag
	feat=map(main);

# (b) other tags
	k=index($0,"[") ;
	tags = substr($0, k+1 );
	sub(/\].*/, "", tags);
	gsub(/ /,"", tags);
	gsub(/:\+/,"", tags);

	n=split(tags,x,",");
	for (i=1;i<=n;i++) 
	{
	feat = (feat map(x[i]) ) ;
	}
    

# print
	printf "%s%s\t\t%s;\n", lemma, feat add, class ;
    }
}



