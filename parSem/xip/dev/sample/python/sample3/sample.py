import sys
import xips
import objects

def XIP(input):
	gHandler = xips.loadgrm('gram_gen_entit.grm')
    	xips.setdisplay(gHandler, xips.XIP_UTF8_INPUT| xips.XIP_DISPLAY_CONVERSION_UTF8 )
    
	dataS = objects.XipResult()
	s = objects.parse(gHandler, input, dataS)

	print "WORDS:"
	for i in dataS.getwords():
		print i

	print "DEPNDENCIES:"
	for i in dataS.getdep():
		print i



XIP("This is the sentence to parse")
