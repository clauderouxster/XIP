import xips

print xips.whoami()

#load grammar
gHandler = xips.loadgrm('../../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm')
#gHandler = xips.loadgrm('..\..\..\GRAMMARS\ENGLISH\GRMFILES\GRAM_GEN\gram_gen_entit.grm')
xips.setdisplay(gHandler,xips.XIP_DISPLAY_FULL | xips.XIP_DISPLAY_TREE)

# launch parsing    
data = []
res = xips.parsestring(gHandler, "This is a test.", data)
print res
