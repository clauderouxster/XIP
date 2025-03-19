# reformat output of fst command "write spaced-text"

# B a h a m a s +Prop +Country     FPL;
# B a h a m a s   0      0     0  0  
# ==> Bahamas+Prop+Country     FPL;

# B u n d e s r e p u b l i k %  D e u t s c h l a n d +Prop +Country +Acron     FSG;
# B R D 0 0 0 0 0 0 0 0 0 0 0  0   0 0 0 0 0 0 0 0 0 0 0   0      0       0    0  0  
# Bundesrepublik% Deutschland+Prop+Country+Acron:BRD	FSG; 

BEGIN{
    line=0 ;
}

# ----------------------------------------------------------------------
function map(achar) 
{
    res = achar ;
    if (achar=="0") { res= "" ; }
    else if (achar=="%") { res = "% " ; } 

    return res ;
}
# ----------------------------------------------------------------------


{
    if (NF==0)
    {
	if (lemma==srf) 
	{
	    printf "%s\t%s\n", lex, cont ;
	} 
	else
	{
	    printf "%s:%s\t%s\n", lex, srf, cont ;
	};
	line=0;
    }
    else if (line==0)
    {
	lex=""; 
	lemma=""; 
	cont=$NF ;
	for (i=1;i<NF;i++) 
	{
	    lex = (lex map($i) );
	    if ($i!~/\+/) {lemma= (lemma map($i)) } ;
	}
	line++;
    }
    else if (line==1)
    {
	srf="";
	for (i=1;i<NF;i++) {
	   srf = (srf map($i)) ;
	}
	line++;
    }
	
}
