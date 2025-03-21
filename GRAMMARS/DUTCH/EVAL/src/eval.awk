# INPUT: output of sdiff TEST.manual TEST.xip
# where TEST files are preprocessed with prep-eval.awk !
BEGIN{
  FS = "[	]+" ;
}

{
  # determine category
  if ($1 ~/\(.*\)/ ) 
    {cat = $1 ; } 
  else 
    {cat = $NF ; }   
  ;
  
  sub(/\(.*\)/, "", cat );
  count[cat]++;


  # false positive
  # e.g. PP(0000, 3:4)     <
  if ($0~/     [>]/)  { count_fp[cat]++; }

  # false neg
  # e.g.        > NP(0004, 5:5)
  else if ($0~/     [<]/) { count_fn[cat]++ ; }
		      
  # different -> false neg AND false pos
  # e.g. NP(0003, 4:5)  | NP(0003, 4:4)

  else if ($0~/     [|]/) { count_fn[cat]++; count_fp[cat]++; }

  # otherwise : correct
  # eg. NP(0000, 1:2)      NP(0000, 1:2)
  else {count_tp[cat]++ ; }
}
  

END{

  for ( cat in count ) {
    printf "---------------------------------------------------------------------------\n" ;
    printf "*** %s \n", cat ;
    printf "---------------------------------------------------------------------------\n" ;

    printf "true positives:  %4d\n", count_tp[cat] ;
    printf "false positives: %4d\n", count_fp[cat] ;
    printf "false negatives: %4d\n", count_fn[cat] ;

    print "" ;

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


    tp_fp=count_tp[cat]+count_fp[cat];
    tp_fn=count_tp[cat]+count_fn[cat];

    precision = 0 ; 
    recall = 0; 
    fscore = 0 ;
    if (tp_fp>0) { precision = count_tp[cat]/tp_fp ; } ;
    if (tp_fn>0) { recall = count_tp[cat]/tp_fn ; } ;
    if (precision+recall>0) { fscore = (2*precision*recall)/(precision+recall) ; }


  printf "Precicion: %5.2f%%\n", precision*100 ;
  printf "Recall:    %5.2f%%\n", recall*100 ;
  printf "F-score:   %5.2f%%\n", fscore*100 ;

  print "";
  }
}
