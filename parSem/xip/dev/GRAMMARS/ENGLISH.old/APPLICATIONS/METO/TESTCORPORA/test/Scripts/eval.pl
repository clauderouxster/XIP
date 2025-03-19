#!usr/bin/perl


##########################################################
# Scriptname: eval.pl
# Creator: Katja Markert
# Questions to : markert@comp.leeds.ac.uk
# Date: 2.1.2007
#######
# Purpose: To evaluate Semeval submissions in accordance with the evaluation measures mentioned in the guidelines
#          and to check that submissions conform to the correct format 
# Input: A submission file in the correct format as specified in the guidelines and a goldstandard file as in ../countries.train
########
# Call: perl eval.pl <submissionfile> <goldstandardfile>
#Example Call: perl eval.pl ../SampleSubmission/NLPuni.location.fine ../countries.train


#####################################################################






&readsubmission;  #reads submissions, performs some well-formedness checks and initialises measures
&readgold; #reads in gold standard file, and sets baseline
&check; #checks that the same samplenumbers are evaluated
&compare; #compares readings
&printme; #prints evaluation measures

#######################################SUBROUTINES######################


sub readsubmission{
    @submissionsamples=();
    open(SUB, "$ARGV[0]") || die "cannot open $ARGV[0]\n";
    $linenumber=0;
    while(<SUB>){
	chomp;
	$linenumber++;
	if($linenumber==1){
	    if($_ =~ /\#\s*(.*)\s*/){
		$systemidentifier=$1; #should not contain any white space
		print STDOUT "systemidentifier=$systemidentifier\n";
	    }
	    else{
		die "Your submission does not contain a systemidentifier on the first line\n";
	    }
	}
	elsif($linenumber==2){
	    if($_ =~ /\#\s*(.*)\s*/){
		$class=$1; #should not contain any white space
		print STDOUT "class=$class\n"
	    }
	    else{
		die "Your submission does not contain a classname on the second  line\n";
	    }
	    if(!($class eq "org" || $class eq "location")){
		die "your classname must be either location or org. Please check your submission\n";
	    }
	}
	elsif($linenumber==3){
	    if($_ =~ /\#\s*(.*)\s*/){
		$granularity=$1;  #should not contain any white space
		print STDOUT "granularity=$granularity\n\n\n";
	    }
	    else{
		die "Your submission does not contain a granularity on the third  line\n";
	    }
	    if(!($granularity eq "coarse" || $granularity eq "medium" || $granularity eq "fine")){
		die "your granularity must be coarse, medium or fine Please check your submission\n";
	    }
	    &getreadings;
	    &initialise;
	}
	else{
	    if($_ =~ /(samp\d+)\|(.*)\s*/){
		$sample=$1;
		$assignedreading{$sample}=$2;
		if(&included($sample,@submissionsamples) ==1){
		    die "the samplenumber $sample is included twice in your submission. Each sample must be included exactly once!\n";
		}

		else{
		    push(@submissionsamples,$sample);

		}

	    }
	    if((&included($assignedreading{$sample},@readings) ==0) && (!($assignedreading{$sample} eq "unknown"))){
		die "the samplenumber $sample was assigned a reading by you which is not in the target categories!\n";
	    }
	}
    }
    close(SUB); 

}


sub getreadings{
   
    if($granularity  eq "coarse"){
	@readings = ("literal","non-literal");

    }
    elsif($granularity eq "medium"){
	@readings = ("literal","metonymic","mixed");


    }
    elsif($granularity eq "fine"){
	if($class eq "location"){
	    @readings = ("literal","place-for-people","place-for-event","place-for-product","object-for-name","object-for-representation","othermet","mixed");

	}
	elsif($class eq "org"){
	    @readings = ("literal","organisation-for-members","organisation-for-event","organisation-for-product","organisation-for-facility","organisation-for-index","object-for-name","object-for-representation","othermet","mixed");
	}

    }
    $b = "literal";  #baselinereading is literal reading (no metonymy recognition)

}

#measures with all|backoff as prefix not used for semeval (not printed at the end)
sub initialise {
    $accuracy = 0;
    $coverage =0;
  
    $allaccuracy = 0;
    $allcoverage =0;
    foreach $r(@readings){
	$precision{$r} ="undef";
	$recall{$r} = "undef";
	$fscore{$r} ="undef";


	$allprecision{$r} ="undef";
	$allrecall{$r} = "undef";
	$allfscore{$r} ="undef";
	$occurring{$r}=0;
	$percentage{$r}=0;
    }
   
   

}


sub readgold {  #
    @goldsamples=();
    open(GOLD, "$ARGV[1]") || die "cannot open $ARGV[1]\n";
    while(<GOLD>){
	chomp;
	if($_=~ /id\=\"(samp\d+)\"/){
	    $sample = $1;
	    push(@goldsamples,$sample);
	    
	}
	elsif($_=~ /reading\=\"([^\"]*)\"/){ #This contains the medium reading
	    $realreading{$sample}=$1;
	
	    if($granularity eq "coarse"){
		if(!($realreading{$sample} eq "literal")){
		    $realreading{$sample} = "non=literal";
		}

	    }
	    elsif($granularity eq "medium"){
		#no changes necessary

	    }
	    elsif($granularity eq "fine"){
		if($_ =~ /metotype\=\"([^\"]*)\"/){ #gets fine-grained reading
		    $realreading{$sample}=$1;
		}
	

	    }
	}
	
    }
    close(GOLD);
    
}


sub check{ #checks whether the same samplenumbers are collected
    @submissionsamples = sort @submissionsamples;
    @goldsamples = sort @goldsamples;
    if(!(@submissionsamples eq @goldsamples)){
	die "The samplenumbers you submitted do not correspond to the samplenumbers in teh gold file\n";
    }
  

}

#Comparison uses some measures not used for semeval, namely all the ones with "all" or "backoff" as prefix
sub compare{

    foreach $sample (@goldsamples){
	$samples++; #counts number of samples
	$rr = $realreading{$sample};
	$ar = $assignedreading{$sample};
	#print STDOUT "$sample\|$ar|$rr\n";
	$occurring{$rr}++; #counts occurrence of a single reading
       
	if($b eq $rr){ #computes baselineperformance
	    $balltrues++;
	}
	if($ar eq $rr){  #correct assignments
	    $trues++; 
	    $covered++; 
	    $alltrues++;
	    $allcovered++;
	    $truepos{$rr}++; 
	    $alltruepos{$rr}++; 
	    $assigned{$ar}++; 
	    $allassigned{$ar}++; 
	    
	}
	elsif($ar eq "unknown"){  #deal with partial subm,issions
	  
	    if($b eq $rr){
		$alltrues++;
		$allcovered++;
		$alltruepos{$rr}++;
		$allassigned{$b}++;
	    }
	    else{
		$allcovered++;
		$allassigned{$b}++;
	
	    }
	    
	}
	
	else{  #wrong assignments
	   
	    $covered++; 
	    $allcovered++;
	    
	    $assigned{$ar}++; 
	    $allassigned{$ar}++; 
	    
	}
    }
}

sub printme{
    
   
    $coverage = $covered/$samples; 
    $allcoverage = $allcovered/$samples;#should always be 1
    $ballacc=$balltrues/$samples; 
    
    unless($covered==0){
	$accuracy = $trues/$covered;
	
    }
    unless($allcovered==0){
	$allaccuracy = $alltrues/$allcovered;

    }
    
    foreach $r (@readings){
	$percentage{$r} = $occurring{$r}/$samples;
	unless($assigned{$r}==0){
	    $precision{$r} = $truepos{$r}/$assigned{$r};
	}
	 unless($allassigned{$r}==0){
	     $allprecision{$r} = $alltruepos{$r}/$allassigned{$r};
	 }
	unless($occurring{$r}==0){
	    $recall{$r} = $truepos{$r}/$occurring{$r};
	    $allrecall{$r} = $alltruepos{$r}/$occurring{$r};
	}	
	unless(($precision{$r} eq "undef") || ($recall{$r} eq "undef") || ($precision{$r}+$recall{$r}==0)) {
	    $fscore{$r}=(2*$precision{$r}*$recall{$r})/($precision{$r}+$recall{$r});
	}
	unless(($allprecision{$r} eq "undef") || ($allrecall{$r} eq "undef") || ($allprecision{$r}+$allrecall{$r}==0)) {
	    $allfscore{$r}=(2*$allprecision{$r}*$allrecall{$r})/($allprecision{$r}+$allrecall{$r});
	}


    }


    
    print "numberofsamples=$samples\n\n";
#    print "covered=$covered\n";
#    print "correct=$trues\n";
    print "accuracy=$accuracy\n";
    print "coverage=$coverage\n";
    print "allaccuracy=$allaccuracy\n";
    print "allcoverage=$allcoverage\n\n\n";
    

#    print "baselinecorrect (of all)=$balltrues\n";
#    print "baselinecovered (of all)=$samples\n";
  #  print "baselineaccuracy (of all)=$ballacc\n";
   print "baselineaccuracy=$ballacc\n";
    print "baselinecoverage=1\n\n\n";

 
    foreach $r (@readings) {
	print "reading: $r; occurring (in gold data) = $occurring{$r}, percentage (in gold data) = $percentage{$r}\n";
	print "-------------------\n";
	print "precision=$precision{$r}\n"; 	
	print "recall=$recall{$r}\n";	
	print "fscore=$fscore{$r}\n\n";

	print "allprecision=$allprecision{$r}\n"; 	
	print "allrecall=$allrecall{$r}\n";	
	print "allfscore=$allfscore{$r}\n\n";
	
	print "\n\n"
	    
	}

   
}


sub included {    #checks whether a new element  is already in a list; necessary to avoid cooccurrence duplication within a sample

    local($element,@list) = @_;
    local($incl) = 0;
    foreach $i (@list){
	if($i eq $element){
	    $incl =1;}
    }
  
    return $incl;

}

