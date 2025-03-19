import xips

xips.loadgrm('/opt/ParSem_db/users/Roux/xip/parSem/xip/dev/GRAMMARS/FRENCH/french.grm');
xips.adddisplay(xips.XIP_DISPLAY_COMPUTE_DISTANCE);

s=""
v=xips.parsestring("La dame mange une glace.")
print v

