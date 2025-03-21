import xips

grm=xips.loadgrm('/opt/ParSem_db/users/Roux/xip/parSem/xip/dev/GRAMMARS/ENGLISH/GRMFILES/ENTITIES_ONLY/entities_only.grm')

s=""

v=xips.parsestring(grm,"Peter Jones went to Paris last month. John Lemmon went to London last year",s)
print v


