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
	if (lex==srf) 
	{
	    printf "%s%s\t%s\n", lex, cat, cont ;
	} 
	else
	{
	    printf "%s%s:%s\t%s\n", lex, cat, srf, cont ;
	};
	line=0;
    }
    else if (line==0)
    {
	lex=""; 
	cat=""; 
	cont=$NF ;
	for (i=1;i<NF;i++) 
	{
	    if ($i~/\+/) {cat= (cat $i); }
	    else {lex= (lex map($i)) ; } ;
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