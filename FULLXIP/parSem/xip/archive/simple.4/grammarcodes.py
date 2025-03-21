# -*- coding: utf8 -*-
######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: grammarcodes.py
#Comment: Common variables and error code for the grammar
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

import ending
#The actual directory in which the all stuff is stored...
import sys

seps="\\"
currentlanguage="english"
initialpath=sys.path[0]+seps+"Ressources"+seps
rootfile=initialpath+"ractafrancatas.bse"
lexiconfile=initialpath+"lextaF.bse"

def complen(x,y):
    if len(x)>len(y):
        return -1
    if len(x)==len(y):
        return 0
    return 1


categories=["Noun","Det","Adj","Punct","Adv","Prep","Pron","Verb", "NP", "VP", "PP", "AP","S","Conjunction"]
codes={}
endl="\n"
heads=["Noun","Adj","Verb","Prep","Conjunction"]
endings={"Noun":"ta",
         "Det":"li",
         "Adj":"ca",
         "Adv":"no",
         "Conjunction":"nu",
         "Prep":"ar",
         "Pron":"er",
         "Verb":"iag"}

cd=1
for i in categories:
    codes[i]=cd
    cd<<=1

class Eraiag(Exception):
    def __init__(self,error,msg):
        self.error=error
        self.my_message=msg


def displaynode(s):
    if "surf" in s:
        ch=s["surf"]+"["
    else:
        ch=s["pos"]+"["
    for w in s.keys():
       if w not in ["surf","idx","up","iroot","p","node","u","i"]:
           try:
               wx=s[w].encode("utf-8")
               ch+=w+":'"+wx+"' "
           except:
               pass
           
    ch=ch.rstrip()
    ch+="]"
    return ch
    
def get(x):
    p=-1
    if len(x)==1:
        return [x,"-",p]
    if len(x)>2 and x[-2]=='#':
        p=int(x[-1])        
        x=x[:-2]
    if x[-1] in ["^","$"]:
        return [x[:-1],x[-1],p]
    return [x,"-",p]

################################################################################
#Various displays
def getsemfeat(dp):
    if "loc" in dp:
        return "+loc"
    if "num" in dp:
        return "+num"
    if "temp" in dp:
        return "+temp"
    if "proper" in dp:
        return "+proper"
    return None

def putlower(d,x):
    if "loc" not in d and "proper" not in d:
        return x.lower()
    return x

def putlowermin(d,x):
    if "proper" not in d:
        return x.lower()
    return x

def extractroot(x):
    if "root" in x:
        return x["pos"]+"#"+str(x["idx"])+'[lemma="'+x["root"]+'"]'
    s=""
    for n in x["node"]:
        if s!="":
            s+="_"
        s+=extractroot(n)
    return s

def extractminroot(x):
    if "root" in x:
        s=x["root"]
        if s[0].isdigit():
           pos=s.find("_")
           if pos!=-1 and s[:pos].isdigit():
               s=s[pos+1:]
        return s+"_"+str(x["idx"])
    s=""
    for n in x["node"]:
        if s!="":
            s+="_"
        s+=extractminroot(n)
    return s

def extractjustroot(x):
    if "root" in x:
        s=x["root"]
        if s[0].isdigit():
           pos=s.find("_")
           if pos!=-1 and s[:pos].isdigit():
               s=s[pos+1:]
        return s
    s=""
    for n in x["node"]:
        if s!="":
            s+="_"
        s+=extractjustroot(n)
    return s

def extractrootset(x):
    if "root" in x:
        s=x["root"]
        var=None
        if s[0].isdigit():
           pos=s.find("_")
           if pos!=-1 and s[:pos].isdigit():
               var="#"+s[:pos]
               s=s[pos+1:]
        if "meaning" in x:
            return [s,x["pos"],var,str(x["idx"]),getsemfeat(x),x["meaning"],False]    
        else:
            return [s,x["pos"],var,str(x["idx"]),getsemfeat(x),s,False]
    return None
    
def getending(surf):
    res=ending.ending(surf)
    try:
        rs=res["root"]+res["basis"]
        return [rs,rs.lower()]
    except:
        return "error"

def wordform(wrd,dico,root):
    if "root" in wrd:
        rt=wrd["root"]
        rt=putlowermin(wrd,rt)
        if wrd["pos"] in endings:
            [surfm,surf]=getending(wrd["surf"])
            if surf in dico:
                return rt+"="+dico[surf].split("=")[1]
            elif surfm in dico:
                return rt+"="+dico[surfm].split("=")[1]
        if rt in root:
            return rt+":"+root[rt]
        return rt
    return ""

def translate(wrd,dico,root,language):        
    if "root" in wrd:
        rt=wrd["root"]
        rt=putlowermin(wrd,rt)
        if wrd["pos"] in endings:
            surf=wrd["surf"]
            if surf in dico:
                return dico[surf].split("=")[1]
            [surfm,surf]=getending(surf)
            if surf in dico:
                return dico[surf].split("=")[1]
            elif surfm in dico:
                return dico[surfm].split("=")[1]
        if rt in root:
            #In French, if the adverb is missing then we use the adjective with "ment"
            if language=="francais" and wrd["pos"]=="Adv":
                m=root[rt].split("|")
                m=m[0]
                if m[-1] in ["a","e","i","o","u"]:
                    m=m+"ment"
                else:
                    m=m+"ement"
                return m
            #In this case, we try to figure out if a verb exists that might replace the missing
            #adjective
            if language=="francais" or language=="english":
                if wrd["pos"]=="Adj" or (wrd["pos"]=="Noun" and language=="english"):
                    #If the verb is available then we use it
                    vrb=rt+"iag"
                    if vrb in dico:
                        wrd["pos"]="Verb"
                        if wrd["pos"]=="Noun":
                            wrd["tense"]="prog" #ing English only
                        else:
                            wrd["tense"]="ppe"
                        return dico[vrb].split("=")[1]                
            return root[rt]
        return rt
    return ""
    
def display(x,dico,root):
    theres=wordform(x,dico,root)
    pf=theres.find("|")
    if pf!=-1:
        theres=theres[:pf]+"|"
    if theres=="":
        theres=x["pos"]
        if "rg" in x:
            theres+="_"+str(x["rg"])
    else:
        theres+=":"+x["pos"]
    if "node" in x:
        theres+="{"
        theres+=displaysentence(x["node"],dico,root)
        theres+="} "
    return theres

def displaysentence(s,dico,root):
    theres=""
    nb=len(s)
    for i in s:
        theres+=display(i,dico,root)
        nb-=1
        if nb!=0:
            theres+=","
    return theres

def displayalemmanode(s):
    ch=s["pos"]
    if "surf" in s:
        ch+="_"+s["surf"]
    ch+="["
    add=False
    for w in s.keys():
       if w not in ["surf","idx","up","iroot","p","node","u","i","pos","root","basis"]:
           add=True
           ch+=w+":'"+str(s[w])+"' "
    ch=ch.rstrip()
    ch+="]"
    if not add:
        return ch[:-2]
    return ch

def displayanode(s):
    ch=s["pos"]
    ch+="["
    add=False
    for w in s.keys():
       if w not in ["surf","idx","up","iroot","p","node","u","i","pos","root","basis"]:
           add=True
           ch+=w+":'"+str(s[w])+"' "
    ch=ch.rstrip()
    ch+="]"
    if not add:
        return ch[:-2]
    return ch


def nodestr(x):    
    if "root" in x:
        return displayalemmanode(x)    
    return displayanode(x)


def displayatree(s,tr,depth):
    tabs="".zfill(depth).replace("0","  ")
    tr[0]+=tabs
    tr[0]+=nodestr(s)+"\n"
    if "node" in s:
        for n in s["node"]:
            displayatree(n,tr,depth+1)

def displaytree(s):
    tr=["\n"]
    for i in s:        
        displayatree(i,tr,1)
        tr[0]+="\n"
    return tr[0]
