import xips

fichier="Test1.txt"

p=xips.loadgrm("french_desamb.grm")

xips.setdisplay(p,xips.XIP_DISPLAY_FULL|xips.XIP_DISPLAY_CHUNK_TREE|xips.XIP_DISPLAY_LEMMA|xips.XIP_DISPLAY_SURFACE|xips.XIP_DISPLAY_ENTREE|xips.XIP_DISPLAY_CATEGORY|xips.XIP_DISPLAY_NONE)
resultat=[]
xips.parsefile(p,fichier,resultat)
s="".join(resultat)
pfinal=xips.loadgrm("french_entity.grm")
xips.setdisplay(pfinal,xips.XIP_DISPLAY_FULL|xips.XIP_DISPLAY_CHUNK_TREE|xips.XIP_DISPLAY_LEMMA|xips.XIP_DISPLAY_SURFACE|xips.XIP_DISPLAY_ENTREE|xips.XIP_DISPLAY_CATEGORY|xips.XIP_DISPLAY_NONE)
res=[]
xips.parsestring(pfinal,s,res)


