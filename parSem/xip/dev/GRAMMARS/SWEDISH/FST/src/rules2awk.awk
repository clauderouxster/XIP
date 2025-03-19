# conversion of inflection rules into awk rules

BEGIN{
    printf "BEGIN{\n" ;
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
	# don't care ...
    }

    # inflection classes
    else if (NF>0)
    {
	class= $1 ;
	affix= $2 ;
	remove= $3 ;
	classnum++;

	cnum[class]++ ;
	classname = sprintf("class%03d",classnum) ;

	printf "cname[\"%s\",%d]=\"%s\" ;\n", class, cnum[class], classname
	printf "ctest[\"%s\",%d]=\"%s\" ;\n", class, cnum[class], affix
	printf "cremove[\"%s\",%d]=\"%s\" ;\n", class, cnum[class], remove
    }
}
END{
    for (class in cnum) 
    {
	printf "classnum[\"%s\"] = %d ;\n", class, cnum[class]
    }
    printf "}\n" ;
}
