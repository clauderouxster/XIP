# reformat lookup output for HmmTrain
# (1) remove empty lines
# (2) add spaces between tags of ambiguitye classes

BEGIN{ FS="[	]+"}
{
  if (NF>0) {
    morph=$1 ;
    gsub(/  /,"	", morph);
    tags=$NF ;
    gsub(/\+/, " +", tags);
    sub(/^ /, "", tags);
    printf "%s\t%s \n", morph, tags
    }
}
