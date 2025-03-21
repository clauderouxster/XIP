# -*- coding: utf8 -*-
######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: principles.py
#Comment: This file implements the grammaticality checking of a sentence
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

#########################################################################
#Checking the grammaticality of the input.
import grammarcodes

def cmpfeat(x,y,fts):
    for s in fts:
        if s in x:
            if not s in y:
                return False
        else:
            if s in y:
                return False
    return True

def featurecomparison(x,y):
    #We will relax the feature principle for the moment
    return True
    if not cmpfeat(x,y,["num","loc","temp"]):
        return False
    return True


def determinerrule(s,sz,wrd,i):
    #First rule: if we have a determiner, then we must have a noun next to it with the same case value
    j=i+1
    while j<sz:
        #We can only have noun and adj in between with the same case value
        wrdnext=s[j]        
        if "case" not in wrdnext:
            if wrdnext["pos"]=="Adv":
                j+=1
                continue
            msg="We expect only a noun or adjectives after the determiner: '"+wrd["surf"]+"': "+wrdnext["pos"]+":'"+wrdnext["surf"]+"'"
            raise grammarcodes.Eraiag("DeterminerError",msg)                
        if wrdnext["case"]!=wrd["case"]:
            msg="Adjectives and nouns should share the same case as the determiner: '"+wrd["surf"]+"'"
            raise grammarcodes.Eraiag("CaseError",msg)
        if not featurecomparison(wrd,wrdnext):
            msg="Adjectives and nouns should share the same features as the determiner: '"+wrd["surf"]+"'"
            raise grammarcodes.Eraiag("CaseError",msg)            
        if wrdnext["pos"] in ["Adj","Noun"]:
            break
        j+=1
    if j==sz:
        msg="We could not find a Noun for this determiner: '"+wrd["surf"]+"'"
        raise grammarcodes.Eraiag("DeterminerError",msg)
    return True


def pronounrule(s,sz,wrd,i):
    #Second rule: if we have a pronoun, then we must have a verb with a relative form
    j=i+1
    while j<sz:
        wrdnext=s[j]
        if wrdnext["pos"]=="Verb":
            if wrdnext["rel"]=="-":
                frm=wrdnext["surf"].rfind("ia")
                srf=wrdnext["surf"][:frm]+"ie"+wrdnext["surf"][frm+2:]
                msg="We expect this verb to have the relative form: '"+srf+ "' and not: '"+wrdnext["surf"]+"' after the pronoun: '"+wrd["surf"]+"'"
                raise grammarcodes.Eraiag("VerbError",msg)
            if wrdnext["tense"]=="Inf":
                wrdnext["pronoun"]="+"
            else:
                break
        j+=1
    if j==sz:
        msg="We could not find a Verb with the relative form for this pronoun: '"+wrd["surf"]+"'"
        raise grammarcodes.Eraiag("PronounError",msg)
    wrdnext["pronoun"]="+"
    return True


def preprule(s,sz,wrd,i):
    #Third rule: if we have a preposition, then the next words should be either dative or genetive
    j=i+1
    while j<sz:
        wrdnext=s[j]
        #only det,noun and adj have a case
        if "case" not in wrdnext:
            #Adverbs are accepted
            if wrdnext["pos"]=="Adv":
                j+=1
                continue
            msg="We expect only nouns, determiners and adjectives after this preposition: '"+wrd["surf"]+"': "+wrdnext["pos"]+":'"+wrdnext["surf"]+"'"
            raise grammarcodes.Eraiag("PrepositionError",msg)
        if wrdnext["case"] !="Dat":
            msg="After the preposition: '"+wrd["surf"]+"', the only possible case is a dative: '"+wrdnext["surf"]+"'"
            raise grammarcodes.Eraiag("CaseError",msg)
        if wrdnext["case"]=="Dat" and wrdnext["pos"] in ["Noun","Adj","Verb"]:
            break
        j+=1
    if j==sz:
        msg="We could not find a Noun or an Adjective with the dative case after this preposition: '"+wrd["surf"]+"'"
        raise grammarcodes.Eraiag("PrepositionError",msg)
    return True

def infinitiverule(s,sz,wrd,i):
    #Fith rule: if we have an infinitive verb, then there should be a non infinitive verb after
    j=i+1
    while j<sz:
        wrdnext=s[j]
        if wrdnext["pos"] not in ["Adv","Verb"]:
            msg="We expect only verbs or adverbs after this verb: '"+wrd["surf"]+"'"
            raise grammarcodes.Eraiag("VerbError",msg)
        if wrdnext["pos"]=="Verb" and wrdnext["rel"]!=wrd["rel"]:
            msg="We expect each verb to comply to the same relative form: '"+wrdnext["surf"]+"', '"+wrd["surf"]+"'"
            raise grammarcodes.Eraiag("VerbError",msg)
        if wrdnext["pos"]=="Verb" and wrdnext["tense"]!="Inf":
            break
        j+=1
    if j==sz:
        msg="We could not find a proper verb to match this infinitive verb: '"+wrd["surf"]+"'"
        raise grammarcodes.Eraiag("VerbError",msg)
    return True

def dativeandprep(s,sz,wrd,i):
    #Sixth rule: if we have a dative then, we need a preposition before, only dative and genetive adjectives can occur before
    j=i-1
    while j>=0:
        wrdnext=s[j]

        if wrdnext["pos"]=="Prep":
            return True           
        if "case" not in wrdnext:
            if wrdnext["pos"]=="Adv":
                j-=1
                continue
            msg="We expect the preposition before this dative: '"+wrd["surf"]+"': "+wrdnext["pos"]+":'"+wrdnext["surf"]+"'"
            raise grammarcodes.Eraiag("PrepositionError",msg)
        if wrdnext["case"] !="Dat":
            msg="Before the dative: '"+wrd["surf"]+"', we expect a preposition: '"+wrdnext["surf"]+"'"
            raise grammarcodes.Eraiag("CaseError",msg)
        j-=1
    if j<=0:
        msg="We could not find a preposition for this dative noun: '"+wrd["surf"]+"'"
        raise grammarcodes.Eraiag("PrepositionError",msg)
    return True

def commonfeatures(s,sz,wrd,i):
    #Seventh rule: All words in a chunk should share the same features
    j=i+1
    while j<sz:
        wrdnext=s[j]
        if wrdnext["pos"]=="Adv":
            j+=1
            continue
        #Are we still in a chunk
        if "case" in wrdnext:
            #Does this chunk comprises elements with the same case?
            if wrdnext["case"]!=wrd["case"]:
                break
            #Then THEY must share the same features
            if not featurecomparison(wrd,wrdnext):
                msg="Adjectives and nouns should share the same features: '"+wrd["surf"]+"'"
                raise grammarcodes.Eraiag("CaseError",msg)            
        j+=1
    return True
   
    
def check(s):
    #the sentence here is a list of token
    sz=len(s)
    for i in range(sz):
        wrd=s[i]
        #First rule: if we have a determiner, then we must have a noun next to it with the same case value
        if wrd["pos"]=="Det":
            determinerrule(s,sz,wrd,i)
            
        #Second rule: if we have a pronoun, then we must have a verb with a relative form
        if wrd["pos"]=="Pron":
            pronounrule(s,sz,wrd,i)

        #Third rule: if we have a preposition, then the next words should be either dative or genetive            
        if wrd["pos"]=="Prep":
            preprule(s,sz,wrd,i)

        if "case" in wrd and wrd["case"]=="Dat":
            dativeandprep(s,sz,wrd,i)

        if wrd["pos"]=="Adj":
            commonfeatures(s,sz,wrd,i)
            
        #Fourth rule: if a verb is a principal verb, then it should have no relative feature:
        #=======================================================================
        # if wrd["pos"]=="Verb":
        #    if "pronoun" not in wrd:
        #        if wrd["rel"]=="+":
        #            frm=wrd["surf"].rfind("ie")
        #            srf=wrd["surf"][:frm]+"ia"+wrd["surf"][frm+2:]
        #            msg="We expect this verb to have the non relative form: '"+srf+ "' and not: '"+wrd["surf"]+"'"
        #            raise grammarcodes.Eraiag("VerbError",msg)
        #=======================================================================
            #if wrd["tense"]=="Inf":
            #    infinitiverule(s,sz,wrd,i)

###########################################################################################################
# ORDER

def testnpnotadj(s,sz,wrd,i):
    #Seventh rule: All words in a chunk should share the same features
    j=i+1
    while j<sz:
        wrdnext=s[j]
        if wrdnext["pos"]!="NP":
            return True
        if "node" in wrdnext and "case" in wrdnext:
            if wrdnext["case"]==wrd["case"] and wrdnext["node"][-1]["pos"]=="Adj":
                msg="Adjectives should alway precede nouns: '"+wrdnext["surf"]+"'"
                raise grammarcodes.Eraiag("CaseError",msg)
        j+=1
    return True
                
def checkorder(s):
    sz=len(s)
    for i in range(sz):
        wrd=s[i]
        if wrd["pos"]=="NP" and wrd["node"][-1]["pos"]=="Noun":
            testnpnotadj(s,sz,wrd,i)
            
