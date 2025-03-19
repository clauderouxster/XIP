# -*- coding: iso-8859-1 -*-
import xips

vxip=xips.whoami()
print vxip
grm=xips.loadgrm("/home/vnikouli/GALATEAS/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/GALATEAS/galateas_xipapi.grm")
xr=objects.XipResult()
objects.parse(grm,"Animals in Therapy",xr)
print xr
