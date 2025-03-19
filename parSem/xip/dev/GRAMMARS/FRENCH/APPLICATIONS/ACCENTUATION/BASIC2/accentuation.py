"""
python:

affichefinal(#1,string s).
"""

import toklook
import accentue
hd=accentue.hd

premier=True

def affichefinal(i,ch):
    global premier
    [s,l,f]=ch.split("\t")
    rh=l+f
    rs=toklook.lookdown(hd,rh)
    mot=rs[0][1]
    if mot=="?":
        if premier:
            s=s[0].upper()+s[1:]
        print s,
    else:
        if premier:
            mot=mot[0].upper()+mot[1:]
        if len(mot)<=2:
            if premier:
                if len(mot)==1:
                    if mot[0] in ["C","N"]:
                        print mot+"'",
                    else:
                        print mot,
                else:
                    mot=mot[0]+mot[1:].lower()
                    print mot,
            else:
                if len(mot)==1 and mot[0] in ["C","N"]:
                    print mot.lower()+"'",
                else:
                    print mot.lower(),
        else:
            print mot,
    premier=False
    if s==".":
        premier=True
        print


