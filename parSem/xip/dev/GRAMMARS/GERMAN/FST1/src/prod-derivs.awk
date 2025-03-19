# convert derivational word forms

# }Bar ;
# }Dim ;
# }Dim$ ;
# }Er ;
# }Er$ ;
# }Er2 ;
# }Er2$ ;
# }ErIsch ;
# }Ese ;
# }heit ;
# }igkeit ;
# }in ;
# }in$ ;
# }isch ;
# }isch$ ;
# }Isch ;
# }Isch$ ;
# }keit ;
# }ung ;
# }Vr ;

# ------------------------------------------------------------------------------
function cleanup( astr ) {
    tmp = astr ;
    sub( /\[P\]/, "^|", tmp );
    sub( /\[Z\]/, "", tmp );
    sub( /\[G\]/, "", tmp );

    return tmp ;
}

# ------------------------------------------------------------------------------
function umlaut( astr ) 
{
    tmp = astr;
    sub(/[bcdfghjklmnpqrstvwxyzß]*e?[bcdfghjklmnpqrstvwxyzß]*$/, "#&", tmp);
    
    sub(/Au#/,"Äu", tmp);
    sub(/Eu#/,"eu", tmp);
    sub(/Aa#/,"Ä", tmp);
    sub(/A#/,"Ä", tmp);
    sub(/O#/,"Ö", tmp);
    sub(/U#/,"ü", tmp);

    sub(/au#/,"äu", tmp);
    sub(/eu#/,"eu", tmp);

    sub(/aa#/,"ä", tmp);
    sub(/a#/,"ä", tmp);
    sub(/oo#/,"ö", tmp);
    sub(/o#/,"ö", tmp);
    sub(/uu#/,"ü", tmp);
    sub(/u#/,"ü", tmp);

    sub(/#/, "", tmp);
    
    return tmp;

}
# ------------------------------------------------------------------------------
function first_upp( astr )
{
    k=index(astr,"@C.un@") ;
    if (k>0) { k=6 };
    tmp0 = substr(astr,1,k);
    tmp1 = toupper(substr(astr,k+1,1));
    tmp2 = substr(astr,k+2);
    tmp = (tmp0 tmp1 tmp2 );
    return tmp;
}

function first_low( astr )
{
    tmp1 = tolower(substr(astr,1,1));
    tmp2 = substr(astr,2);
    tmp = (tmp1 tmp2 );
    return tmp;
}

# ------------------------------------------------------------------------------
function adjust( astr )
{
    tmp =  astr ;
    sub(/.\^1/, "", tmp );
    sub(/..\^2/, "", tmp );
    sub(/...\^3/, "", tmp );
    sub(/....\^4/, "", tmp );
    return tmp ;
}

# ------------------------------------------------------------------------------

function vstem_cons( astr ) 
{
    tmp = astr ;
    sub(/tmen\+Verb.*/,"tem",tmp);	# beatembar
    sub(/dmen\+Verb.*/,"dem",tmp);	# widembar ?

    sub(/bnen\+Verb.*/,"ben",tmp);	# ebenbar ?
    sub(/chnen\+Verb.*/,"chen",tmp);	# berechenbar
    sub(/cknen\+Verb.*/,"cken",tmp);	# trockenbar
    sub(/ffnen\+Verb.*/,"ffen",tmp);	# eröffenbar ?
    sub(/gnen\+Verb.*/,"gen",tmp);	# beregenbar ?
    sub(/dnen\+Verb.*/,"den",tmp);      # anordenbar ? 
    sub(/mmnen\+Verb.*/,"mmen",tmp);	# willkommenbar ?
    sub(/pnen\+Verb.*/,"pen",tmp);	# wappenbar ?
    sub(/tnen\+Verb.*/,"ten",tmp);	# ??
   	
    sub(/eln\+Verb.*/,"el",tmp);	# regelbar
    sub(/ern\+Verb.*/,"er",tmp);	# federbar

    sub(/en\+Verb.*/,"",tmp);		# spielbar
    return tmp ;
}
   
function vstem_vowel( astr ) 
{
    tmp=astr;
    sub(/eln\+Verb.*/,"^el",tmp);	# regelung
    sub(/ern\+Verb.*/,"^er",tmp);	# federung

    sub(/en\+Verb.*/,"",tmp);		# wendung
    return tmp ;
}

# ------------------------------------------------------------------------------
function adjstem ( astr )
{
    tmp = astr ;
    sub(/\+Adj/,"",tmp);

    sub(/\^[eE]/, "", tmp);
    sub(/e$/, "", tmp);

    return tmp;
}


# ------------------------------------------------------------------------------
function nounstem ( astr )
{
    tmp = astr ;
    if (astr~/\+Noun/) {
	sub(/\+Noun.*\+Fem/,"",tmp);
	sub(/\+Noun.*\+Masc/,"",tmp);
	sub(/\+Noun.*\+Neut/,"",tmp);
	sub(/\+Noun.*\+MN/,"",tmp);
	sub(/\+Noun.*\+MF/,"",tmp);
	sub(/\+Noun.*\+Undef/,"",tmp);
    }
    else {
	sub(/\+Prop.*\+Fem/,"",tmp);
	sub(/\+Prop.*\+Masc/,"",tmp);
	sub(/\+Prop.*\+Neut/,"",tmp);
	sub(/\+Prop.*\+MN/,"",tmp);
	sub(/\+Prop.*\+MF/,"",tmp);
	sub(/\+Prop.*\+Undef/,"",tmp);
    };

    tmp = adjust(tmp);

    sub(/\^[eE]/, "", tmp);
    sub(/e$/, "", tmp);			# Erbe -> Erbin

    return tmp;
}

function nouncat( astr )
{
    tmp = astr ;
    if (astr~/\+Noun/) {
	sub(/.*\+Noun/,"",tmp);
    }
    else {
	sub(/.*\+Prop/,"",tmp);
    };
    sub(/\+Fem.*/,"",tmp);
    sub(/\+Masc.*/,"",tmp);
    sub(/\+Neut.*/,"",tmp);
    sub(/\+MN.*/,"",tmp);
    sub(/\+MF.*/,"",tmp);
    sub(/\+Undef.*/,"",tmp);

    if (tmp=="") { tmp = "+Deriv"; } ;

    return tmp;
}


# ------------------------------------------------------------------------------
BEGIN{ 
    FS="[	]+";
}

{
    lex = cleanup($1) ;
    cont = $2;
    FORMAT = "%s\t%s ;\n" ;

    
    if (cont ~ /}Bar/)
    {
	stem1 = vstem_cons(lex);
	stem2 = first_upp(stem1);
	pref = "@C.un@" ;
	printf FORMAT, pref stem1 "bar+Adj+Deriv", "A_reg" ;
	printf FORMAT, pref stem2 "barkeit+Noun+Deriv+Fem", "Nf_en" ;
    }

    else if  (cont ~ /}ung/)   
    { 
	stem1 = first_upp(vstem_vowel(lex));
	printf FORMAT, stem1 "ung+Noun+Deriv+Fem", "Nf_en" ;
    }
    
    else if (cont ~ /}Vr/ )
    { 
	stem1 = vstem_vowel(lex);
	stem2 = first_upp(stem1);
	
	printf FORMAT, stem2 "er+Noun+Deriv+Masc", "N_0n" ;
	printf FORMAT, stem2 "erin^~+Noun+Deriv+Fem", "Nf_en" ;
	
	printf FORMAT, stem2 "erei+Noun+Deriv+Fem", "Nf_en" ;
	printf FORMAT, stem1 "erisch+Adj+Deriv", "A_reg" ;
	
	if (stem1~/[^i]e[rl]$/)
	{
	    printf FORMAT, stem2 "in^~+Noun+Deriv+Fem", "Nf_en" ;
	    printf FORMAT, stem2 "ei+Noun+Deriv+Fem", "Nf_en" ;
	    printf FORMAT, stem1 "isch+Adj+Deriv", "A_reg" ;
	}
    }


    else if (cont ~ /}Dim/ )
    {
	stem1 = nounstem(lex);
	sub(/en$/,"",stem1);		# Faden -> Fädchen
	
	if (cont ~/\$/) {stem1 = umlaut(stem1); } ;
	
	stem2 = stem1 ;
	if (stem2 ~ /el$/) { sub( /el$/, "e", stem2); } ;	# Engel -> Engelein
	
	printf FORMAT, stem1 "chen+Noun+Dim+Neut", "N_0" ;
	if (stem2 !~ /l$/) {
	    printf FORMAT, stem2 "lein+Noun+Dim+Neut", "N_0" ;
	} ;

    }

    else if (cont ~ /}[iI]sch/ )
    {
	
	stem = nounstem(lex) ;
	feat = nouncat(lex) ;
	
	if (lex ~ /\+Noun/) { sub(/ik?$/,"",stem) ; } ;

	if (cont ~/\$/) {stem = umlaut(stem); } ;

	printf FORMAT, first_low(stem) "isch+Adj" feat "+Masc", "A_reg" ;

	if (cont ~/}Isch/) {
	    printf FORMAT, stem "isch+Noun+Lang" feat "+Neut", "N_Sgs" ;
	}

    }

    else if (cont ~ /}Er/ )
    {
    
	stem = nounstem(lex) ;
	feat = nouncat(lex) ;
	
	if (cont ~/\$/) {stem = umlaut(stem); } ;
	
	printf FORMAT, stem "er+Noun" feat "+Masc", "N_0n" ;
	printf FORMAT, stem "erin^~+Noun" feat "+Fem", "Nf_en" ;
	
	if (cont ~/}Er2/) {
	    printf FORMAT, stem "er+Adj" feat "+Invar+Attr+Upp", "Upp" ;
	}
	
	if (cont ~/}ErIsch/) {
	    printf FORMAT, stem "er+Adj" feat "+Invar+Attr+Upp", "Upp" ;
	    printf FORMAT, first_low(stem) "erisch+Adj" feat, "A_reg" ;
	    printf FORMAT, stem "erisch+Noun+Lang" feat, "N_Sgs" ;
	}
    }

    else if (cont ~ /}Ese/ )
    {
	stem = nounstem(lex) ;
	feat = nouncat(lex) ;
	
	printf FORMAT, stem "ese+Noun" feat "+Masc", "Nm_Wk" ;
	printf FORMAT, stem "esin^~+Noun" feat "+Fem", "Nf_en" ;
	printf FORMAT, first_low(stem) "esisch+Adj" feat, "A_reg" ;
	printf FORMAT, stem "esisch+Noun+Lang" feat, "N_Sgs" ;
	
    }

    else if (cont ~ /}in/ )
    {
	stem = nounstem(lex) ;
	feat = nouncat(lex); 
	
	if (cont ~/}in\$/) {stem = umlaut(stem); } ;
	
	printf FORMAT, stem "in^~+Noun" feat "+Fem", "Nf_en" ;
	
    }


    else if (cont ~ /}.*[kh]eit/ )
    {
	stem = adjstem(lex);
	affix = cont ;
	sub(/}/, "", affix) ;
	sub(/ *;/, "", affix);
	
	printf FORMAT, first_upp(stem) affix "+Noun+Deriv+Fem", "Nf_en" ;
    }

}