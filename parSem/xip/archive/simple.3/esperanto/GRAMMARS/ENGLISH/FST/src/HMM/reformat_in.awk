# reformat manually tagged input for 2-column lookup
# (1) take only 1st and 2nd column of original input
# (2) replace series of tabs by double space 
#     (otherwise lookup won't work properly!)

BEGIN{ FS="[	]+"}
{
  printf "%s  %s\n", $1, $2 
}

