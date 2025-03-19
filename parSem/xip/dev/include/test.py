# -*- coding: iso-8859-1 -*-

a="autre sujet à l'ordre du jour : la coopération bilatérale"
s=unicode(a,"Latin-1").encode("UTF-8")
f=open("C:\\XIP\\Test\\French\\BASIC\\input.xml","w")
f.write('<?xml version="1.0" encoding="utf-8"?>\n')
f.write('<root><t>\n')
f.write(s)
f.write('\n</t>\n</root>\n')
f.close()



