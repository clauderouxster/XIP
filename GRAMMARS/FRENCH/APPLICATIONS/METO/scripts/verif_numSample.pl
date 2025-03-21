#!/usr/bin/perl
# SemEval
#printer les lectures attendues



open(F,$ARGV[0]) ||die "erreur $!\n"; #en lecture.


#*** remplissage donnees de ref
while ($line =<F>){
    if ($line=~/id="document_(\d+)">/){
	push(@numSampGold,$1);
	}
  }



# *** verif
print "\n\n\n***Les num des Samples Gold:  \n";
    foreach $elem(@numSampGold){
      print "$elem\n";
    }


exit 0;



