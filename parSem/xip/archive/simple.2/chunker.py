# -*- coding: utf8 -*-
######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: chunker.py
#Comment: This file implements the chunking of a sentence
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

import grammarcodes
from chunkerrules import *

##############################################################################
#The application of those rules

def replacenodes(sentence,categories,node,rg):
    firstelement=rg[0]
    dernier=None
    for i in rg:        
        dernier=sentence.pop(firstelement)
        node["node"].append(dernier)
        dernier["up"]=node

    sentence.insert(firstelement,node)
    ps=0
    for i in sentence:
        i["idx"]=ps        
        ps+=1
    try:
        categories[node["pos"]].append(node)
    except:
        categories[node["pos"]]=[node]
    
    #we also copy the features from the head
    for c in dernier.keys():
        if c in ["pos","root","idx","up","node","u"]:
            continue
        node[c]=dernier[c]

    
def compiling(r,num_rule):
    global compiled
    if num_rule in compiled:
        return compiled[num_rule]
    
    value=0
    mandatory_category=[-1,-1]
    for i in r[1:-2]:
        if type(i)==type([]):
            continue
        [r,kleene,p]=grammarcodes.get(i)
        value=grammarcodes.codes[r]|value
        if kleene=='$':
            mandatory_category[1]=grammarcodes.codes[r]
        if kleene=='^':
            mandatory_category[0]=grammarcodes.codes[r]
    compiled[num_rule]=[value,mandatory_category]
    return [value,mandatory_category]

def apply_rule(sentence,categories,rule,num_rule):
    #A rule is a set of potential list of categories
    #The first element is the final category
    [coderule,mand_cat]=compiling(rule,num_rule)
    #we then scan the sentence from left to right
    well_formed=False
    if mand_cat[0]==-1:
        well_formed=True
    wm=well_formed
    phrs=[]
    
    for ps in range(0,len(sentence)):
        wrd=sentence[ps]
        if "u" in wrd:
            continue
        cd=grammarcodes.codes[wrd["pos"]]
        mot=wrd["pos"]
        if "root" in wrd:
            mot=wrd["root"]
        #Two cases, a starting category or a finishing category
        if (cd & coderule)!=0:
            if mand_cat[0]!=-1 and mand_cat[0]!=cd:
                continue
            phr=[wrd]
            #We then extract the longest possible sequence of categories compatible
            #with our rule
            for w in sentence[ps+1:]:
                cd=grammarcodes.codes[w["pos"]]
                if (cd & coderule)!=0:
                    phr.append(w)
                else:
                    break
                
            #We then apply the constraint rule to this sequence
            #When it fails, we remove the last element and ww try again
            try:
                while phr!=[] and not rule[-2](phr):
                    phr.pop()
            except:
                phr=[]
            
            #If we have a sequence, it did match...
            if phr!=[]:
                phrs.append(phr)
                for i in phr:
                    i["u"]=True

    reslen=len(phrs)

    phrs.reverse()
    for phr in phrs:                    
        node={"pos":rule[0],"idx":-1,"node":[],"up":None}
        if len(phr)==1:
            rg=[phr[0]["idx"]]
        else:
            rg=range(phr[0]["idx"],phr[-1]["idx"]+1)
        replacenodes(sentence,categories,node,rg)
    return reslen
