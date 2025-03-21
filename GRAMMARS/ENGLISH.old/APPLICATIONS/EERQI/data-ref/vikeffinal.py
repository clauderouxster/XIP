Python:

vikef_final_processing(int fake_arg, Python Result).

import sys
import os

def vikef_final_processing(fake_arg, Result):
    global TextParseDB

    TextParseDB.endOfParsing = True
    # process post-NLP relations
    TextParseDB.processPostNlp()
    
    # Printing the annotations
    Result.parsedb = TextParseDB
    #TextParseDB.printAnnotations()





