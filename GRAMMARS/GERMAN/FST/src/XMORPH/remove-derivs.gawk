# ---------------------------------------------------------------------
# TYPE:		gawk script
# CONTENT:	remove 9some) derivational classes from lexc file
# AUTHOR:	Anne Schiller, XRCE
#
# DATE:		10-Apr-2003
# ---------------------------------------------------------------------
# USAGE:	gawk -f [ThisFile] < LEX.lexc
# ---------------------------------------------------------------------


# ---------------------------------------------------------------------
BEGIN{
	FS="[	]+"

deriv["}Bar ;"]++;

deriv["}ung ;"]++;

deriv["}Vr ;"]++;

deriv["}heit ;"]++;
deriv["}keit ;"]++;
deriv["}igkeit ;"]++;

deriv["}isch ;"]++;
deriv["}isch$ ;"]++;
deriv["}Isch ;"]++;
deriv["}Isch$ ;"]++;

deriv["}Er ;"]++;
deriv["}Er$ ;"]++;
deriv["}Er2 ;"]++;
deriv["}Er2$ ;"]++;
deriv["}ErIsch ;"]++;
deriv["}ErIsch$ ;"]++;

deriv["}Ese ;"]++;
}

# ---------------------------------------------------------------------
{ #MAIN
if (deriv[$2]==0) {print}

if ($0=="LEXICON VerbStems") {printf "\t	VerbStemsD ;\n"}
if ($0=="LEXICON OtherStems") {printf "\t	OtherStemsD ;\n"}

}
