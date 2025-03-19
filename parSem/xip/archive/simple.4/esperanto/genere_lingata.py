# -*- coding: utf-8 -*-
#francais=open(r"C:\ANL\Mekalinga\ANL\Mekalinga\simple\esperanto\francais.txt","r")
#anglais=open(r"C:\ANL\Mekalinga\ANL\Mekalinga\simple\esperanto\anglais.txt","r")
#lettres=['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z','a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z']
#position=25

##############################################
# Important
# Words in lexicon can be enriched with features in the following way:
#  esperanto        esperanto@MASC
# For the moment only four values are accepted:
#                  FEM and MASC for gender and PL and SG for number
##############################################


import sys
import xips
import ending

seps="\\"
localpath=sys.path[0]+seps

grmfr=0
grmen=1

temporels = ["lundo","mardo","merkredo","ĵaŭdo","vendredo","sabato","dimanĉo","januaro","februaro",
             "marto","aprilo","majo","junio","aŭgusto","septembro","oktobro","novembro","decembro","semajnfino",
             "semajno","semajnofino","tago","monato","jaro","jarcento","jarmilo","tempo","horo","minuto","sekundo",
             "duonhoro","mateno","posttagmezo","tagmezo","nokto"]

adjtemp=[]
for t in temporels:
    tadj=t[:-1]+"a"
    adjtemp.append(tadj)
    tadj=t[:-1]+"e"
    adjtemp.append(tadj)
temporels.extend(adjtemp)
adjtemp=[]
for t in temporels:
    tw=unicode(t,"utf8")
    adjtemp.append(tw)
temporels=adjtemp


frenchpath=localpath+"GRAMMARS"+seps+"FRENCH"+seps+"BASIC"+seps+"french.grm"
englishpath=localpath+"GRAMMARS"+seps+"ENGLISH"+seps+"GRMFILES"+seps+"GRAM_GEN"+seps+"gram_gen.grm"
try:
    grmfr=xips.loadgrm(frenchpath)
except:
    print "Chargement erreur du francais"
try:
    grmen=xips.loadgrm(englishpath)
except:
    print "Chargement erreur de l'anglais"

xips.setdisplay(grmfr,xips.XIP_DISPLAY_NONE)
xips.setdisplay(grmen,xips.XIP_DISPLAY_NONE)

def decoupe(j):
    apos=False
    ls=j.replace("(","(_")

    lstmot=ls.split(",")
    reslist=[]
    for l in lstmot:    
        k=l.split("(")
        if (len(k)!=1):
            liste=[]
            for i in k:
                p=i.split(")")
                if len(p)==1:
                    liste.append(i)
                else:
                    liste.extend(p)
            lst=[]
            for l in liste:
                if len(l)>0:
                    if l[0]!="_":
                        lst.append(l)
                    elif l=="_s'~" or l=="_se ~":
                        st=lst.pop()
                        st="PRON(se);VERB("+st.strip()+")"
                        lst.append(st)
            reslist.extend(lst)
        elif l[0:2]=="s'":
            st="PRON(se);VERB("+l[2:]+")"
            reslist.append(st)
        elif l[0:3]=="se ":
            st="PRON(se);VERB("+l[3:]+")"
            reslist.append(st)            
        else:
            reslist.append(l)
    res=[]
    for r in reslist:
        pos=r.find("[")
        if pos!=-1:
            r=r[:pos]
        res.append(r)
    return res


def xiperie(s,langue):
    xr=""
    res=xips.parsestring(langue,s,xr)
    res=res.strip()
    if res[-1]==";":
        res=res[:-1]
    pos=res.find("(")
    cats=res[:pos].upper()
    return [cats,res]
    

def traiteexpressions(lfr,cat,lg):
    reslst=[]

    for l in lfr:
        if " " in l.strip() and ";" not in l:
            try:
                s=unicode(l,"utf-8").encode("latin-1")
            except:
                sl=l.split()
                s=" ".join(sl[:-1])
                s=unicode(s,"utf-8").encode("latin-1")
                s+=" "+unicode("à","utf8").encode("latin-1")
                
            [cats,res]=xiperie(s,lg)
            if cats!=cat:
                if cat=="NOUN" or cat=="ADVERB":
                    prescats=cats
                    if cats=="VERB" or cats=="ADJ":
                        if lg==grmfr:
                            s="Deux "+s
                            [cats,res]=xiperie(s,lg)
                            res=res[10:]
                        else:
                            s="Two "+s
                            [cats,res]=xiperie(s,lg)
                            res=res[9:]
            try:
                res=unicode(res,"latin-1").encode("utf-8")
            except:
                print "Autre erreur:",res
            reslst.append(res)
        else:
            reslst.append(l)
    return reslst


def getpos(i):
    lst=i.split(";")
    reps=[]
    for l in lst:
        ps=l.find("(")
        if ps==-1:
            return None
        reps.append(l[:ps])
    if len(reps)==1:
        return None
    return ",".join(reps)


lstpos={"francais":[],"english":[]}

def composemot(mot,langue,cat):
    if langue=="francais":
        lg=grmfr
    else:
        lg=grmen

    prt=mot.find("(")
    if mot[:prt].lower() in ["prep","noun","verb","adj","adv","pron","det","num","conj","prefix","coord","neg"]:
        return mot
    if "crème glacée" in mot:
        return "NOUN(crème),ADJ(glacé)"

    lfr=decoupe(mot)
    tr=traiteexpressions(lfr,cat,lg)
    for i in tr:
        reps=getpos(i)
        if reps!=None:
            if reps not in lstpos[langue]:
                lstpos[langue].append(reps)
            
    return tr[0].replace(";",",")
    
lexicons={}
from baselexicon import *

etre=unicode("être","utf-8")
auterminaison=unicode("aw","utf-8")
terminaisons={"a":"ca","i":"iag","o":"ta","e":"no","n":"no"}
lvoyelles=["a","e","i","o","u","ŭ"]
consonnes=["b","c","d","f","g","h","j","k","l","m","n","p","q","r","s","t","v","w","x","z"]
lstsuffixes={'aĉ':'pejorative', 'ad':'duration', 'aĵ':'concretisation', 'an':'member', 'ant':'patient',
          'ar':'group', 'ebl':'possibility', 'ec':'abstract',
          'eg':'augmentative', 'ej':'location', 'end':'obligation', 'er':'unit',
          'estr':'leader', 'et':'small', 'id':'child', 'ig':'make', 'iĝ':'become',
          'il':'tool', 'ind':'merit', 'ing':'partial_container',
          'ism':'doctrin', 'ist':'profession', 
          'obl':'multiplicative', 'on':'fraction', 'op':'collective',
          'uj':'full_container', 'ul':'specificity', 'um':'none'}
voyelles=[]
for i in lvoyelles:
    voyelles.append(unicode(i,"utf-8"))

suffixes={}
for si in lstsuffixes:
    k=unicode(si,"utf-8")
    suffixes[k]=lstsuffixes[si]

suffixeskeys=[]
for si in suffixes:
    if len(si) not in suffixeskeys:
        suffixeskeys.append(len(si))        
suffixeskeys.sort(reverse=True)

remplace={"j":"y", "ĵ":"j", "c":"ts", "ĉ":"tc", "ĝ":"dj", "ĥ":"r", "ŝ":"c", "ŭ":"w"}


for i in lexicons:
    tr=[]
    if "francais" in lexicons[i]:
        for f in lexicons[i]["francais"]:
            tr.append(unicode(f,"cp1252").encode("utf-8"))
        lexicons[i]["francais"]=tr


remplacements={}

for r in remplace:
    remplacements[unicode(r,"utf-8")]=remplace[r]
def traitelarge(chemin,langue,position):
    print chemin
    fichier=open(chemin,"r")
    for i in fichier:
        mot=i
        if len(mot)<=position+1:
            continue
        if mot[0]=="-":
            continue
        trad=mot[position:]
        esperanto=mot[:position]
        trads=trad.split(",")
        trados=[]
        for t in trads:            
            tr=unicode(t,"utf-8")
            tr=tr.replace(";",",")
            tr=tr.strip()
            if tr[:5]=="avoir" or tr[:4]==etre:
                trsplit=tr.split()
                ltr=[]
                letre=False
                if len(trsplit)>1:
                    dc=0
                    for ttr in trsplit:
                        if "'" in ttr:
                            x=ttr.split("'")
                            if x[0]=="l":
                                ltr.append("det(le)")
                            else:
                                ltr.append("prep(de)")
                            ltr.append("noun("+x[1]+")")
                        elif ttr=="avoir" or ttr==etre:
                            letre=True
                            ltr.append("verb("+ttr+")")
                        elif ttr=="mauvaise":
                            ltr.append("adj(mauvaise)")
                        elif ttr=="bien":
                            ltr.append("adverb("+ttr+")")
                        elif ttr in ["en","de","pour","dans","sur"]:
                            ltr.append("prep("+ttr+")")
                        elif ttr in ["l'","le","la","les"]:
                            ltr.append("det("+ttr+")")
                        elif dc==1 and letre:
                            ltr.append("adj("+ttr+")")
                        else:
                            ltr.append("noun("+ttr+")")
                        dc+=1
                    tr=",".join(ltr)
            trados.append(tr)
            
        trads=trados
        esperanto=unicode(esperanto,"utf-8")
        esperanto=esperanto.strip()
        temporal=False
        if esperanto in temporels:
            temporal=True
            
            
        #If we are dealing with an adjective or a noun, we try to analyze the
        #suffix, which might be of a certain semantic value
        suffixroot=""
        features=""
        noending=False
        if esperanto[-1]=="_":
            noending=True
            esperanto=esperanto[:-1]
        baseword=esperanto
        if esperanto=="pro":
            suffixroot=""
            esperanto+="e"
        elif esperanto[-1] in ["o","a","i"] and " " not in esperanto and not esperanto[0].isupper():
            pos=esperanto[-1]
            ourroot=esperanto[:-1]
            for lk in suffixeskeys:
                #We suppose a minimum root of 3 characters
                baseroot=ourroot[:-lk]
                if len(baseroot)<3:
                    continue
                extract=ourroot[-lk:]
                try:
                    anchor=ourroot[-lk-1]
                    if anchor in voyelles:
                        continue
                    features=suffixes[extract]
                    esperanto=baseroot+pos
                    suffixroot=extract
                    break
                except:
                    continue
                                
        mot=""    
        for i in esperanto:
            if i in remplacements:
                mot+=remplacements[i]
            else:
                mot+=i

        suff=""
        for i in suffixroot:
            if i in remplacements:
                suff+=remplacements[i]
            else:
                suff+=i

        suffixroot=suff        
        mot=mot.replace("ks","x")
        if mot[0:2]=="ss":
            mot=mot[1:]
        termine=""
        if mot=='bei':
            mot='beai'

        if noending==False:
            if len(mot)>6 and mot[-3:]=="ino":
                termine="fa"
                mot=mot[:-3]
            elif len(mot)>2 and mot[-2:]==auterminaison:
                termine="no"
                mot=mot[:-1]
            elif mot[-1] in terminaisons:
                termine=terminaisons[mot[-1]]
                if len(mot)>2:
                    mot=mot[:-1]        
        
        senseroot=mot
        if termine!="":            
            if mot[-1] in consonnes:
                mot+="e"
            elif mot[-1]=="i":
                mot+="o"
            elif mot[-1]=="y":
                mot+="a"
            senseroot=mot
            #we remove the first letter of the suffix root, which is always a vowel
            mot+=suffixroot[1:]
            if mot[-1] in consonnes:
                mot+="e"
            if mot[0].isupper():
                clef=mot+"te"
            else:
                clef=mot+termine

            trados=[]
            for t in trads:
                tr=t.replace("'","\'")
                tr=tr.encode("utf-8")
                tr=tr.strip()
                tr=composemot(tr,langue,ending.ending(clef)["pos"].upper())
                trados.append(tr)
                
                
            trads=trados
            baseword=baseword.encode("utf-8")
            mot=mot.encode("utf-8")
            if temporal:
                #In the case of an adverb, we do not modify it
                if clef[-2]!="n":                    
                    clef=clef[:-1]+"i"
                    if clef[-2]=="f":
                        clef=clef[:-2]+"ti"
                    
            if " " not in clef:                    
                if clef in lexicons:
                    if langue in lexicons[clef]:
                        for tr in trads:
                            if tr not in lexicons[clef][langue]:
                                lexicons[clef][langue].append(tr)
                    else:                
                        lexicons[clef][langue]=trads
                else:
                    lexicons[clef]={"rakta":mot,langue:trads,"esperanto":[baseword],"sense":senseroot}
                if features!="":
                    try:
                        lexicons[clef]["features"][features]="+"
                    except:
                        lexicons[clef]["features"]={features:"+"}
                    
    fichier.close()
            
titre="""# -*- coding: utf-8 -*- 
######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: lingata.py
#Comment: This file implements the lexicon file on the basis of the esperanto lexicon
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

lexicons={}
"""
traitelarge(localpath+"francais_grand.txt","francais",25)    
traitelarge(localpath+"francais_tres_large.txt","francais",25)    
traitelarge(localpath+"anglais_grand.txt","english",31)    
#traitelarge(r"C:\ANL\Mekalinga\ANL\Mekalinga\simple\esperanto\allemand.txt","deutsch",50)

tri=lexicons.keys()
tri.sort()
sauve=open(localpath+".."+seps+"lingata.py","w")
sauve.write(titre)
sauve.write("\n")
for i in tri:
    ch='lexicons["'+i+'"]='+str(lexicons[i])+"\n"
    cvh=ch.encode("utf-8")
    sauve.write(cvh)
sauve.write("\n")
sauve.close()
sauve=open(localpath+"\\expressions.txt","w")
for l in lstpos["francais"]:
    sauve.write(l+"\n")
sauve.close()


