import sys
import xips
import translation


import lingata

frenchToLingve={}
def buildidx(frenchToLingve):
    for i in lingata.lexicons.keys():
         s=lingata.lexicons[i]
         if "francais" in s:
             lct=s["francais"]
             for l in lct:
                 try:
                     frenchToLingve[l].append(i)
                 except:
                     frenchToLingve[l]=[i]
                     
                
def splitword(w):
    [pos,m]=w.split(":")
    return [pos,m]

def COMP(n,w1,w2):
    return ["COMP",n,splitword(w1),splitword(w2)]
def COORD(n,w1,w2):
    return ["COORD",n,splitword(w1),splitword(w2)]
def JUXT(n,w1,w2):
    return ["JUXT",n,splitword(w1),splitword(w2)]
def APPOS(n,w1,w2):
    return ["APPOS",n,splitword(w1),splitword(w2)]
def SUJV(n,w1,w2):
    return ["SUJ",n,splitword(w1),splitword(w2)]
def AUXV(n,w1,w2):
    return ["AUX",n,splitword(w1),splitword(w2)]
def CODV(n,w1,w2):
    return ["COD",n,splitword(w1),splitword(w2)]
def MODV(n,w1,w2):
    return ["MODV",n,splitword(w1),splitword(w2)]
def MODN(n,w1,w2):
    return ["MODN",n,splitword(w1),splitword(w2)]
def MODA(n,w1,w2):
    return ["MODA",n,splitword(w1),splitword(w2)]
def MODR(n,w1,w2):
    return ["MODR",n,splitword(w1),splitword(w2)]
def MODP(n,w1,w2):
    return ["MODP",n,splitword(w1),splitword(w2)]
def CPLV(n,w1,w2):
    return ["CPLV",n,splitword(w1),splitword(w2)]
def ATBSO(n,w1,w2):
    return ["ATBSO",n,splitword(w1),splitword(w2)]


cheminbase=sys.path[0]
cheminxip=cheminbase+"\\francais\\grammaire\\francais.grm"
buildidx(frenchToLingve)

def loadgrm(grm):
    i=xips.loadgrm(grm)
    xips.setdisplay(i,xips.XIP_DISPLAY_LEMMA|xips.XIP_DISPLAY_REDUCED|xips.XIP_DISPLAY_CATEGORY)
    return i


grm=0
grm=loadgrm(cheminxip)

s=""
x=xips.parsestring(grm,"Le chien mange une glace.",s)
ls=x.split()
res=[]
for i in ls:
    res.append(eval(i[:-1]))


