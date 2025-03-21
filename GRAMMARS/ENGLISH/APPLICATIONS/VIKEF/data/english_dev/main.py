import sys
import os
import xips




def vikefPrintHeader():
    
    print '<?xml version="1.0" encoding="UTF-8"?>\n<LingAnnotations>'
    
    

class AnnotationResult:
    def __init__(self):
        self.parsedb = None
        self.entities = []
        self.relations = []



def main():
    result = AnnotationResult()
    
    print xips.whoami()
    gram = xips.loadgrm( '/home/ait/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/VIKEF/data/english_dev/english.grm')
    
    xips.setdisplay(gram,XIP_DISPLAY_CHUNK_TREE|XIP_UTF8_INPUT)
    #xips.setdisplay(gram,XIP_DISPLAY_NONE|XIP_UTF8_INPUT)

    stdout = sys.stdout
    
    outputdir = './CPOcorpus/Annotations/'
    inputdir = './CPOcorpus/HarmonizedCollection/'
    fics = os.listdir(inputdir)
    i = 0
    for f in fics:
        if (f != '4123.html.xml'):
            continue
        i = i + 1
        #if (i>6):
        #    break
        stdout.write('!!!! Processing file:'+f+'\n')
        outputFilename = outputdir + f.replace('.xml','') + '.annot'
        sys.stdout = open(outputFilename,'w')
        #vikefPrintHeader()
        xips.parsexmlfile(gram,inputdir+f,0,result)
        result.parsedb.printAnnotations()
        #print '</LingAnnotations>\n'
    









if (__name__=='__main__'):
    main()

