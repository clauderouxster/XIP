# -*- coding: utf8 -*-
######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: chunker.py
#Comment: This file implements the chunking of a sentence
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

import grammarcodes
#########################################################################
#The ID rules
def always(x):
    return True


def comparefeature(X):
    for p in range(0,len(X)):
        if X[p]["pos"]!="Adv":
            break
    x=X[p]
    for i in X[p+1:]:
        if i["pos"]=="Adv":
            continue
        if x["case"]!=i["case"]:
            return False
    return True

def comparen(x):
    if x[-1]["pos"]!="Noun":
        return False
    cpt=0
    for i in x:
        if i["pos"]=="Noun":
            cpt+=1
    if cpt>1:
        return False
    return comparefeature(x)

def comparea(x):
    if x[-1]["pos"]!="Adj":
        return False
    return comparefeature(x)


def comparecoord(x):
    if not "conj" in x[-1]:
        return False
    for i in x:
        if i["pos"]=="NP":
            continue
    return comparefeature(x)

def comparecoordverb(x):
    if not "conj" in x[-1]:
        return False
    for i in x[:-1]:
        if i["pos"]!="VP" or i["tense"]!=x[-1]["tense"]:
            return False
    return True
        
def testverb(x):
    cpt=0
    for i in x:
        if i["pos"]=="Verb":
            cpt+=1
    if cpt>1:
        return False
    return True


def testnom(x):
    #we look for the first noun
    verb=False
    for i in x:
        if i["pos"]=="VP":
            verb=True
            if "rel" not in i or i["rel"]=="+":
                return False
    return verb


def testrelative(x):
    if x[-1]["pos"]!="VP" or x[-1]["rel"]=="-":
        return False
    
    #We also check, if the last element has the infinitive
    
    verb=False
    for i in x:
        if i["pos"]=="Pron":
            return False
        elif i["pos"]=="VP":
            #If there is another verb in between then it is an error
            if verb:
                return False
            verb=True
            if i["rel"]=="-":
                return False
        #There should nothing but verbs
        elif verb:
            return False
    return verb
    
def testnomrel(x):
    #we look for the first noun
    #the first element should be a pronoun
    return testrelative(x[1:])

def testnomconjrel(x):
    #we look for the first noun
    #the first element should be a pronoun
    if len(x)<=1:
        return False
    if "case" not in x[0]:
        return False
    if x[1]["pos"]!="Pron":
        return False        
    return testrelative(x[2:])

def testinf(x):
    if len(x)==1:
        return False
    #the last element should be a verb
    if x[-1]["pos"]!="Verb":
        return False
    if x[-1]["tense"]=="Inf":
        return False
    #Then all elements should be infinitive except the last element
    for i in x[:-1]:
        if i["pos"]=="Verb":
            if i["tense"]!="Inf":
                return False
            else:
                if "case" in i and i["case"]=="Dat":
                    return False
    return True

def testdative(x):

    if "case" not in x[-1]:
        return False
    
    if x[-1]["case"]!="Dat":
        return False
    for i in x[:-1]:
        if i["pos"]=="NP":
            return False
    return True

def testinfprep(x):

    if "tense" not in x[-1]:
        return False
    

    #A verb in a preposition must be an infinitive and a dative: -iagad
    if x[-1]["tense"]!="Inf" or "case" not in x[-1]:
        return False
    if x[-1]["case"]!="Dat":
        return False

    for i in x[1:-1]:
        #Only one preposition can occur
        if i["pos"]=="Prep":
            return False
    return True

#The ^ imposes the presence of that category as the first one
np_rule = ["NP","Det","Adv","Adj","Noun$",comparen,always]
np_rule_adj = ["NP","Det","Adv","Adj",comparea,always]
np_rule_coord=["NP","NP^","NP",comparecoord,always]
pp_rule_dat = ["PP","Prep^","NP",testdative,always]
pp_rule_inf = ["PP","Prep^","NP","VP",testinfprep,always]
vp_rule = ["VP","Adv","Verb$",testverb,always]
vp_rule_inf = ["VP","Adv","Verb$",testinf,always]
vp_rule_bis = ["VP","Adv","Verb$",always,always]
vp_rule_coord=["VP","VP^","VP",comparecoordverb,always]
s_rule_conj = ["S","Conjunction^","Pron","Punct","S","NP","PP","VP$",testnomconjrel,always]
s_rule_pron = ["S","Pron^","NP","S","PP","Punct","VP$",testnomrel,always]
s_rule = ["S","NP^","PP","S","VP$",testnom,always]
s_rule_bis = ["S","PP","S","VP$",testnom,always]

compiled={}


grammarrules=[np_rule,
              np_rule_adj,
              np_rule_coord,

              vp_rule_inf,
              vp_rule,
              vp_rule_coord,
              pp_rule_inf,
              pp_rule_dat,
#We allow up to two embedding levels
              s_rule_conj,
              s_rule_pron,
              s_rule_conj,
              s_rule_pron,
              s_rule,
              s_rule_bis]
