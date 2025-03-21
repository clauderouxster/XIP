import xips

#grmFile="../../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm"
grmFile="../../../grammar/english/entity/gram_gen_entit.grm"
#grmFile="..\..\..\grammar\english\entity\gram_gen_entit.grm"

# display xip info
print xips.whoami()

# load grammar
gHandler = xips.loadgrm(grmFile)

# set parsring option
xips.setdisplay(gHandler,xips.XIP_DISPLAY_FULL | xips.XIP_DISPLAY_TREE)

# launch parsing    
data = []
res = xips.parsestring(gHandler, "This is a test.", data)
print res
