# add a prefix "ḧmm_" to hmm tags in order to avoid confusion with morphologoical tags

{
  sub(/^\+/,"+hmm_") ;
  gsub(/\[\+/, "[+hmm_") ;
  gsub(/hmm_SENT/, "SENT" );  # must not change SENT tag !!1
  print ;
}

