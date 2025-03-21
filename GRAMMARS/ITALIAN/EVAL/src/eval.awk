# INPUT: output of sdiff TEST.manual TEST.xip
#4 :: 0001:dep   DATE(20:10 settembre:11 2001:12)                0001:dep        DATE(20:10 settembre:11 2001:12)
#5 :: 0001:dep   ORG(Parlamento:5 europeo:6)                   | 0002:dep        PERSON_PEOPLE(Presidente:2)
#4 :: 0005:dep   DATE(21:1 settembre:2)                          0005:dep        DATE(21:1 settembre:2)
#4 :: 0005:dep   LOC_CITY(Tolosa:15)                             0005:dep        LOC_CITY(Tolosa:15)
#4 :: 0007:dep   LOC_CITY(Tolosa:21)                             0007:dep        LOC_CITY(Tolosa:21)
#3 :: 0007:dep   ORG(Parlamento:11 europeo:12)                 <
#3 :: 0007:dep   ORG_SOC(AZF:6)                                <
#4 :: 0007:dep   PERSON_LASTNAME(Douste-Blazy:23)                0007:dep        PERSON_LASTNAME(Douste-Blazy:23)
#4 ::                                                          > 0031:dep        PERSON_PEOPLE(Presidente:35)
#4 ::                                                          > 0031:dep        PERSON_PEOPLE(Presidente:44)
#4 ::                                                          > 0031:dep        PERSON_PEOPLE(Presidente:77)
#4
# where TEST files are preprocessed with prep-eval.awk !

BEGIN{
  FS = "[	]+" ;
}

# remove word numbers in dependencies

function format(astr) 
{
    res=astr ;
    gsub(/:[0-9]* /," ", res);
    gsub(/:[0-9]*\)/,")", res);
    return res;
}

# remove extensions _XXX from dependencies
function maincat(astr)
{
    res=astr;
    sub(/_[^(]*/, "", res );
    return res ;
}

# extract category/depedency name
function get_cat(astr)
{
    res=astr ;
    sub(/\(.*/, "", res );
    return res;
}

# =====================================================================
# MAIN

{
    if ($1~/:dep/) {
    # determine categories
    # CASE 1: no manual entry : false positive
    # e.g.	> 0031:dep	PERSON_PEOPLE(Presidente:35)
    if ($1=="") 
    {
	man1=""; 
	man2=""; 
	xip1=format($NF); 
	xip2=maincat(xip1); 
	
    }

    # CASE 2: no XIP reusult : false negative
    # e.g.  0007:dep	ORG(Parlamento:11 europeo:12)	<  
    else if ($4=="")
    {
	man1=format($2);
	man2=maincat(man1);
	xip1=""; 
	xip2="";
    }

    # CASE 3: entries differ ; false positive AND false negative
    # e.g. 0001:dep	ORG(Parlamento:5 europeo:6)	| 0002:dep	PERSON_PEOPLE(Presidente:2)

    # CASE 4: entries correspond ; true positive
    # e.g. 0005:dep   	LOC_CITY(Tolosa:15)	0005:dep	LOC_CITY(Tolosa:15)
    else 
    {
	man1=format($2);
	man2=maincat(man1);
	xip1=format($NF);
	xip2=maincat(xip1);
    };

    # debugging
    ## printf "#:: %s\n", $0 ;
    ## printf "### man1=%s _ man2=%s :: xip1=%s _ xip2=%s\n", man1, man2, xip1, xip2 ;

    if (man1!="") { count1[get_cat(man1)]++; }
    if (man2!="") { count2[get_cat(man2)]++; }
    if (xip1!="") { count1[get_cat(xip1)]++; }
    if (xip2!="") { count2[get_cat(xip2)]++; }
    

    # full categories/dependencies
    # e.g. LOC_CITY, LOC_COUNTRY, ORG_SOC, ORG_SPORT
    if (man1==xip1) 
    { 
	count1_tp[get_cat(man1)]++; 
    }
    else 
    {
	if (man1!="") { count1_fn[get_cat(man1)]++ ; };
	if (xip1!="") { count1_fp[get_cat(xip1)]++ ; };
    }
    

    # main categories/dependencies
    # e.g. LOC, ORG, ...
    if (man2==xip2) 
    { 
	count2_tp[get_cat(man2)]++; 
    }
    else 
    {
	if (man2!="") { count2_fn[get_cat(man2)]++ ; };
	if (xip2!="") { count2_fp[get_cat(xip2)]++ ; };
    }
    }
}

END{

# Compare the XIP result to the manually annotated lists:
# - true positives are the analyses which match the manual choice
# - false positives are all other (XIP) results
# - count a false negative if the manual choice is not among the results.
# 
# 
# The overall precision P is computed as
#    P = #(true positives) / #(true positives + false positives)
# 
# The overall recall R for this experiment is
#    R = #(true positives) / #(true positives + false negatives)
# 
# The F-score combines the two previous measures
#    F = (2*precision*recall)/(precision+recall)


# MAIN DEPENDENCIES
    for ( cat in count2 ) 
    {
	printf "---------------------------------------------------------------------------\n" ;
	printf "### all  %s \n", cat ;
	printf "---------------------------------------------------------------------------\n" ;
	
	printf "true positives:  %4d\n", count2_tp[cat] ;
	printf "false positives: %4d\n", count2_fp[cat] ;
	printf "false negatives: %4d\n", count2_fn[cat] ;
	
	print "" ;

	tp_fp=count2_tp[cat]+count2_fp[cat];
	tp_fn=count2_tp[cat]+count2_fn[cat];
	
	precision = 0 ; 
	recall = 0; 
	fscore = 0 ;
	if (tp_fp>0) { precision = count2_tp[cat]/tp_fp ; } ;
	if (tp_fn>0) { recall = count2_tp[cat]/tp_fn ; } ;
	if (precision+recall>0) { fscore = (2*precision*recall)/(precision+recall) ; }

	printf "Precision: %5.2f%%\n", precision*100 ;
	printf "Recall:    %5.2f%%\n", recall*100 ;
	printf "F-score:   %5.2f%%\n", fscore*100 ;

	print "";
    }

# dependencies with features

    for ( cat in count1 ) 
    {
	printf "---------------------------------------------------------------------------\n" ;
	printf "*** %s \n", cat ;
	printf "---------------------------------------------------------------------------\n" ;
	
	printf "true positives:  %4d\n", count1_tp[cat] ;
	printf "false positives: %4d\n", count1_fp[cat] ;
	printf "false negatives: %4d\n", count1_fn[cat] ;
	
	print "" ;

	tp_fp=count1_tp[cat]+count1_fp[cat];
	tp_fn=count1_tp[cat]+count1_fn[cat];

	precision = 0 ; 
	recall = 0; 
	fscore = 0 ;
	if (tp_fp>0) { precision = count1_tp[cat]/tp_fp ; } ;
	if (tp_fn>0) { recall = count1_tp[cat]/tp_fn ; } ;
	if (precision+recall>0) { fscore = (2*precision*recall)/(precision+recall) ; }


	printf "Precision: %5.2f%%\n", precision*100 ;
	printf "Recall:    %5.2f%%\n", recall*100 ;
	printf "F-score:   %5.2f%%\n", fscore*100 ;

	print "";
    }

}
