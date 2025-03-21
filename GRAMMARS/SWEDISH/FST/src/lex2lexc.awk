# conversion of inflection lexicon


BEGIN{
    printf "Multichar_Symbols\n" ;
    for (i=1;i<=9;i++) { printf " ^%d", i ; } ;
    printf "\n[1]\n\n\n" ;

    printf "LEXICON ROOT\n" ;
}

function test_affix(str1,aff1) 
{
    k=split(aff1,x,",")+1 ;

    for (j=1;j<k;j++)
    {
	atest=( x[j] "$" );
	gsub("C","[bcdfghjklmnpqrstvgwxz]",atest);
	gsub("V","[aeiouyåöäü]",atest);

	res = (match(str1,atest)) ;

	if (res) {return (res); exit } ;
    }
    return (1==2)
}

function surface(suff1) {
    if (suff1=="=") { return "" ;}
    else {
	len=length(suff1) ;
	res = sprintf("^%d", len) ;
	return res ;
    }
}


# --- MAIN ---------------------------------------------------------------
{
    lex = $1 ;
    class = $2 ;
    cmax =  classnum[class] ;
    ctest1= ctest[class,1] ;

    for (i=1;i<=cmax;i++) 
    {
	test=ctest[class,i] ;

	if (test_affix(lex,test) )
	{
	    srf=surface(cremove[class,i]) ;
	    cont=cname[class,i] ;
	    printf "%s%s\t%s ; ! %s-%s\n", lex, srf, cont, class, test ;
	    next ;
	}
    }
}
