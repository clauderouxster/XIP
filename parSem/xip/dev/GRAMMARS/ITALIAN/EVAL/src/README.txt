--------------------------------------------------------------------------------
XIP dependency evaluation
--------------------------------------------------------------------------------

USAGE :

gmake  (ID=textID) (DEP=depID) {init, diff, stat, save}

textID = identifier for the input text (default=eval0)
       	 It corresponds to the name without extension .txt
         (i.e. the corresponding input file name is then textID.txt)

depID  = identifier for the subset of dependencies to extract (default: NE)
       	 These must be defined in a file "def_eval_depID.awk".

--------------------------------------------------------------------------------
(1) init
    
    OUTPUT FILE:	textID_depID.hand1
    DISPLAY:		--

    Initiates a "truth" file for further manual corrections.
    It runs the current version of the xip parser and
    extracts the relevant dependencies.
    The target file must be renamed into textID_depID.hand 
    for manual annotation and subsequent evaluation.

(2) diff

    OUTPUT FILE:	../res/textID_depID.xip
    DISPLAY: 		jdiff textID_depID.hand ../res/textID_depID.xip
    
    Compares XIP results to manual annotations


(3) stat

    OUTPUT FILE:	../res/textID_depID.stat
    DISPLAY: 		less ../res/textID_depID.stat
    
    Computes statistics for comparaision of XIP results and manual annotations

(4) save

    OUTPUT FILE:	../res/textID_depID.xip.$(DATE)
    	   		../res/testID_depID.stat.$(DATE)
    DISPLAY: 		--
    
    Copies XIP output and statistics file to files with a time stamp




    
    	
    

    


