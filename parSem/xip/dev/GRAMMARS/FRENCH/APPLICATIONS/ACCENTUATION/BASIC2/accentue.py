"""
python:

affiche(#1,string s,python reponse).
"""

import toklook

hd=toklook.loadscript("base.scr","","raw","raw")
phono=toklook.loadscript("phono.scr","","raw","raw")
retour=toklook.loadscript("lex.scr","","raw","raw")

def affiche(i,ch,reponse):
    [s,l,f]=ch.split("\t")
    rh=l+f
    rs=toklook.lookdown(hd,rh)
    mot=rs[0][1]
    if mot=="?":
        if len(s)<=2:
            reponse.append(s.lower()+"\t"+l+"\t"+f+"\n\n")
        else:
            phs=toklook.lookup(phono,s.lower())
            smp=phs[0][1]
            rsmp=toklook.lookup(retour,smp)
            if rsmp[0][1]!="?":
                for r in rsmp:
                    surface=toklook.lookdown(hd,r[1]+r[2])
                    if surface[0][1]!="?":
                        reponse.append(surface[0][1]+"\t"+r[1]+"\t"+r[2]+"\n")
                    else:
                        reponse.append(s.lower()+"\t"+l+"\t+Guessed+Noun"+"\n")
                reponse.append("\n")
            else:
                reponse.append(s.lower()+"\t"+l+"\t+Guessed+Noun"+"\n\n")
    else:        
        reponse.append(mot+"\t"+l+"\t"+f+"\n\n")

