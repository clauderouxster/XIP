IMPORTANT: this is an *incomplete* and *non stable* demo prototype of the coreference system for English, based on the XIP English parser. It contains many known (and probably yet unknown) bugs. In particular, the treatment of pronoun "it" is in a very premature state.

Usage:

- First, edit the command script corefparse to set the appropriate values for ROOT_XIP, XIPBINDIR and XIPCOREFDIR

- Then do:
          corefparse -text <textfile>

where <textfile> is the filename of the text file to parse for coreference. 

The prototype has been developed and tested using short input texts (newswires or newspaper articles). Larger texts may drastically slow down the program.

The coreference output is stored in a file called res.html in the directory where the corefparse command has been run. It is highly recommended to use Firefox to brows the result file, as the highlighting CSS definitions for coreference links were developed for and tested with Firefox.


