BEGIN{
}

{ 
    for (i=1;i<=NF;i++)
    {
	if ($i~/:#/) 
	{
	    pair=$i;
	    sub(/^</,"",pair);
	    sub(/>$/,"",pair);
	    
	    
	    sub(/EPS\*/,"EPS%*", pair);
	    sub(/\"0\"/,"0", pair);
	    srf=pair; sub(/.*:#/,"#%",srf);
	    lex=pair; sub(/:#.*/,"",lex);

	    sub(/\^/, "^%", lex);
	    sub(/%$/,"",lex);

	    lex2=srf; sub(/^#/,"",lex2) ;

	    printf "substitute label %s for %%%s:%%%s ; echo ... %s -> %s\n", lex2, lex, srf, pair, lex2
	}
    }
}

END{
}
