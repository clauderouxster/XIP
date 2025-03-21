BEGIN{
    printf "define MAP [\n  " ;
    line=0 ;
}

{ 
    if (length($1)>0) {
	if (line>0) {printf ", "} ;
	line++ ;
	if ($1=="%") 
	{
	    printf "\"%s\" -> \"#%s\"\n", $1, $1 
	}
	else
	{
	    printf "%%%s -> %%#%%%s\n", $1, $1 
	}
    }
}

END{
    printf ", EPS -> %%#EPS || _ $[SEG] ];\n"
}
