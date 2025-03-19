# -*- coding: utf8 -*-
######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: grammar.py
#Comment: This file is the grammar entry point to parse and analyze a sentence
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

testing=True

import ending

from grammarcodes import *

import numbers
import principles
import chunker
import dependencies
import lingata
import translation


#########################################################################
#Parsing

def renum(s,nb,idxs):
    s["idx"]=nb[0]
    idxs[nb[0]]=s
    nb[0]+=1
    if "node" in s:
        renumber(s["node"],nb,idxs)

def renumber(sent,nb,idxs):
    brth=0
    for s in sent:
        renum(s,nb,idxs)
        s["i"]=brth
        brth+=1


def idxroot(commonroots,x):
    rt=x["root"].lower()
    try:
        i=commonroots[rt]
        commonroots[rt]=i+1
    except:
        commonroots[rt]=1
        i=0
    x["iroot"]=i        

def parse(ph,sentence,lexicals,dico,root,graphes,debug,generated,language):
    #deux cas, avec un 
   phrs=ph.split(".")
   commonroots={}
   res=""
   for i in phrs:
       s=i.strip()
       if s!="":
           if s[-1]!=".":
               s+="."
           sent=[]
           res+=parsing(s,sent,lexicals,dico,root,graphes,commonroots,debug,generated,language)
           sentence.extend(sent)
           res+=endl
   return res

def parsing(ph,sentence,lexicals,dico,root,graphes,commonroots,debug,generated,language):
    if debug:
        print ph
    lph=ph.split()
    categories={}
    sentcat=[]
    cpt=0
    for m in lph:
        punct=""
        incrementpunct=True
        if not m[-1].isalpha():
            rs=m[:-1]
            punct=m[-1]
            if rs=="":
                #In this case, it is a single punctuation, which has its own count
                #In the other case, it is concatenated to a string, without any index
                incrementpunct=False
        else:
            rs=m
        
        if rs!="":
            x=None
            #In the case, the string starts with an index as in a graph
            #we remove it
            if rs[0].isdigit():
                pos=rs.find('_')
                if pos!=-1 and rs[:pos].isdigit():
                    rssharp=rs[pos+1:]    
                    x=ending.ending(rssharp)
                    myrootis=x['root']
                    x['root']=rs[:pos+1]+x['root']
            
            if x==None:
                x=ending.ending(rs)
                if x!="" and x!=None:
                    myrootis=x['root']
                
            if x=="" or x==None:
                raise Eraiag("WordError", "Unknown word form: '"+rs+"'")
    
            if "num" in x:
                if not x["root"].isdigit():
                    vl=[0]
                    if numbers.number(x["root"],[],True,vl):
                        x["root"]=str(vl[0])
                    else:
                        raise Eraiag("numberError", "Unknown number form: '"+rs+"'")
    
            if "temp" in x:
                if not x["root"].isdigit():
                    vl=[0]
                    if numbers.number(x["root"],[],True,vl):
                        x["root"]=str(vl[0])
            x["surf"]=rs
            sentence.append(x)
            sentcat.append(x["pos"])
            x["idx"]=cpt
            x["p"]=cpt
            x["up"]=None
            idxroot(commonroots,x)
            lexicals.append(x)
    
            basicform=myrootis+x["basis"]
            lfeat=None
            if basicform in lingata.lexicons:
                lfeat=lingata.lexicons[basicform]
                if "features" in lfeat:
                    for f in lfeat["features"]:
                        x[f]=lfeat["features"][f]
            
            x["meaning"]=myrootis.lower()
            if lfeat!=None:
                if "sense" in lfeat:
                    x["meaning"]=lfeat["sense"]
    
            try:
                categories[x["pos"]].append(x)
            except:
                categories[x["pos"]]=[x]
                    
        if punct!="":
            if incrementpunct:
                cpt+=1
            x={"pos":"Punct","root":punct,"up":None,"surf":punct,"sense":punct}
            x["idx"]=cpt
            x["p"]=cpt
            idxroot(commonroots,x)
            sentence.append(x)
            sentcat.append(x["pos"])
            try:
                categories["Punct"].append(x)
            except:
                categories["Punct"]=[x]
        cpt+=1
    principles.check(sentence)
    numrule=0
    nbwrds=len(sentence)
    wrdlist=sentence[:]
            
    for g in chunker.grammarrules:
        if chunker.apply_rule(sentence,categories,g,numrule)!=0:
            principles.checkorder(sentence)
        numrule+=1

    sent={"pos":"TOP", "node":[],"idx":0}
    for wrd in sentence:
        sent["node"].append(wrd)
        wrd["up"]=sent
                
    sent=[sent]

    mxnb=[0]
    #idxsentence is a dictionary of nodes indexed on idx
    idxsentence={}
    renumber(sent,mxnb,idxsentence)

    numrule=0
    dps={}
    theres=""
    localgraphe={}
    for dp in dependencies.argumentrule:
          dependencies.apply_rule_arg(sentence,categories,dp,graphes,localgraphe,dps)

    if debug:
        for dep in dps.values():
            for d in dep:
                wf=wordform(d[1],dico,root)
                pf=wf.find("|")
                if pf!=-1:
                    wf=wf[:pf]+"-"
                theres+=str(len(d))+":"
                theres+=d[0]
                if len(d)==4:
                    theres+="_"+str(d[3])+"("
                    theres+=wf
                    wf2=wordform(d[2],dico,root)
                    pf=wf2.find("|")
                    if pf!=-1:
                        wf2=wf2[:pf]+"-"
                    theres+=","+wf2
                else:
                    theres+="_"+str(d[2])+"("
                    theres+=wf
                theres+=")\n"
        theres+=displaysentence(sent,dico,root)
        theres+="\n"
    theres+=displaytree(sent)
        
    theres+="\n"
    if generated!=None:
        dependencystring=[]
        stgen=translation.generestring(idxsentence,dps,dico,root,localgraphe,mxnb[0],dependencystring,language)
        stgen=stgen.rstrip()
        if stgen!="" and stgen[-1]!=".":
            stgen+="."
        if stgen!="":
            generated.append(stgen)
        theres+="\n\n"
        depstr=unicode(dependencystring[0],"latin-1")
        try:
            theres+=depstr
        except:
            pass
    return theres

def test(sentence,gr):
    dico={}
    root={}
    msorted=lingata.lexicons.keys()
    msorted.sort()
    for i in msorted:
     s=lingata.lexicons[i]
     rac=s["rakta"]
     if "francais" in s:
        mt=s["francais"]
        lems=""
        for ms in mt:
           if lems!="":
              lems+=","
           lems+=ms
        dico[i]=rac+"="+lems
        root[rac]=lems
    lexicals=[]
    return parse(sentence,lexicals,dico,root,gr,True)


