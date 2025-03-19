# -*- coding: utf8 -*-
######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: dependencies.py
#Comment: This file implements the dependency extraction from a sentence
#Komentata: Tila ficeta ekstrektan dependetas dedars frasetal defineiag. 
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present
#Droita kopatas: Tila ficeta librecan kopatan beiaga kanaiag usaiagque ifer autarca Claudeco Rouxto prezetan beieg

import grammarcodes
import dependencies

def node(x):
    return grammarcodes.displaynode(x)

def setplural(np):
    for n in np["node"]:
        if n["pos"] in ["Det","Adj","Noun"]:
            n["pl"]="+"

def testsubj(depname,x,dps):
    noun=x[0]
    verb=x[1]

    if verb["tense"]=="Inf":
        return None

    if noun["case"]!="Nom":        
        return None
    
    #looking for a S
    while noun["pos"]!="S":
        noun=noun["up"]
        if noun["pos"]=="TOP":
            return None
    #looking for a S
    while verb["pos"]!="S":
        verb=verb["up"]
        if verb["pos"]=="TOP":
            return None
    if verb==noun:
        return [depname,None,None]
    return None

def testsubjpron(depname,x,dps):
    pron=x[0]
    verb=x[1]

    if verb["tense"]=="Inf" or verb["rel"]=="-":
        return None

    if pron["case"]!="Nom":
        return None
    
    #looking for a S
    spron=pron["up"]
    
    #looking for a S
    while verb["pos"]!="S":
        verb=verb["up"]
        if verb["pos"]=="TOP":
            return None
    if verb==spron:
        #we look for the antecedent
        if "ANTECEDENT" in dps:
            dant=dps["ANTECEDENT"]
            for dnt in dant:
                if dnt[1]==pron:
                   return [depname,dnt[2],None] 
    return None


def testcoordverb(depname,x,dps):
    verb1=x[0]
    verb2=x[1]
    if "conj" not in verb2:
        return None
    #First we only coordinate similar verbs
    if verb1["tense"]==verb2["tense"] and verb1["rel"]==verb1["rel"]:
        #second, they must share the same parent, which is up their VP parent
        parent1=verb1["up"]["up"]
        parent2=verb2["up"]["up"]
        if parent1==parent2:
            return [depname,None,None]
    return None

def testcoordnoun(depname,x,dps):
    noun1=x[0]
    noun2=x[1]
    if "conj" not in noun2:
        return None
    #First we only coordinate similar nouns
    if noun1["case"]==noun2["case"]:
        #second, they must share the same parent, which is up their NP parent
        parent1=noun1["up"]["up"]
        parent2=noun2["up"]["up"]
        if parent1==parent2:
            return [depname,None,None]
    return None

def testcoordprep(depname,x,dps):
    prep1=x[0]
    prep2=x[1]

    if "conj" not in prep2:
        return None
    
    #First we only coordinate similar nouns
    if prep1["case"]==prep2["case"] and prep1["meaning"]==prep2["meaning"]:
        #second, they must share the same parent, which is up their NP parent
        parent1=prep1["up"]["up"]
        parent2=prep2["up"]["up"]
        if parent1==parent2:                
            return [depname,None,None]
    return None


def testprepnounverb(depname,x,dps):
    noun=x[0]
    verb=x[1]
    #Must be a main clause verb
    if verb["rel"]=="+":
        return None
    #Noun must be a dative
    if noun["case"]!="Dat":
        return None

    #We fetch the preposition
    try:
        while noun["pos"]!="PP":
            noun=noun["up"]
        
        while verb["pos"]!="S":
            verb=verb["up"]
    except:
        return None
        
    #They must belong to the same S
    if noun["up"]!=verb:
        return None
    
    prep=noun["node"][0]
    try:
        if prep["case"]=="Nom":
            return [depname,None,None]
    except:
        pass
    return None
    
def testcomppron(depname,x,dps):
    #Deactivated for the moment
    return None
    verbieg=x[0]
    verb=x[1]
    if verb["rel"]=="+":
        return None

    if verbieg["tense"]=="Inf" or verbieg["rel"]=="-":
        return None
    
    #looking for a S
    verbieg=verbieg["up"]["up"]
    verbieg=verbieg["up"]
    while verbieg["pos"]!="S" and verbieg["pos"]!="TOP":
        verbieg=verbieg["up"]
        if verbieg["pos"]=="TOP":
            return None
    
    #looking for a S
    while verb["pos"]!="S":
        verb=verb["up"]
        if verb["pos"]=="TOP":
            return None

    if verb==verbieg:
        return [depname,None,None]

    return None

def testcompwithpron(depname,x,dps):
    verbieg=x[0]
    verb=x[1]
    if verb["rel"]=="+" or verb["tense"]=="Inf":
        return None

    if verbieg["tense"]=="Inf" or verbieg["rel"]=="-":
        return None
    
    #looking for a S
    verbieg=verbieg["up"]["up"]
    verbieg=verbieg["up"]
    while verbieg["pos"]!="S" and verbieg["pos"]!="TOP":
        verbieg=verbieg["up"]
        if verbieg["pos"]=="TOP":
            return None
    
    #looking for a S
    while verb["pos"]!="S":
        verb=verb["up"]
        if verb["pos"]=="TOP":
            return None

    if verb==verbieg:
        #The we look for the pronoun
        try:
            pron=x[0]["up"]["up"]["node"][0]
            if pron["pos"]!="Pron":
                return None
            return [depname,pron,x[1]]
        except:
            return None

    return None


def testobjpron(depname,x,dps):
    pron=x[0]
    verb=x[1]

    if verb["tense"]=="Inf" or verb["rel"]=="-":
        return None

    if pron["case"]!="Acc":
        return None
    
    #looking for a S
    spron=pron["up"]
    
    #looking for a S
    while verb["pos"]!="S":
        verb=verb["up"]
        if verb["pos"]=="TOP":
            return None
    if verb==spron:
        return [depname,None,None] 
    return None

def testgenpron(depname,x,dps):
    pron=x[0]
    verb=x[1]

    if verb["tense"]=="Inf" or verb["rel"]=="-":
        return None

    if pron["case"]!="Gen":
        return None
    
    #looking for a S
    spron=pron["up"]
    
    #looking for a S
    while verb["pos"]!="S":
        verb=verb["up"]
        if verb["pos"]=="TOP":
            return None
    if verb==spron:
        #we look for the antecedent
        if "ANTECEDENT" in dps:
            dant=dps["ANTECEDENT"]
            for dnt in dant:
                if dnt[1]==pron:
                   return [depname,dnt[2],None] 
    return None


def testobjprep(depname,x,dps):
    noun=x[0]
    verb=x[1]

    if noun["case"]!="Dat" or "case" not in verb or verb["case"]!="Dat":
        return None
    #They should share the same PP

    while noun["pos"]!="PP":
        noun=noun["up"]
    while verb["pos"]!="PP":
        verb=verb["up"]
    if verb!=noun:
        return None

    return [depname,None,None]
        
def testobj(depname,x,dps):
    noun=x[0]
    verb=x[1]     

    #An infinitive verb within a VP, with more than one verb
    #cannot have an object
    if verb["tense"]=="Inf":
        vp=verb["up"]
        #If it is within a PP, it cannot have an object
        if vp["up"]["pos"]=="PP":
            return None
        nb=0
        for v in vp["node"]:
            if v["pos"]=="Verb":
                nb=nb+1
        if nb>1:
            return None
    
    if noun["case"]!="Acc":
        return None
    if noun["up"]["node"][-1]!=noun:
        return None

    #looking for a S
    while noun["pos"]!="S":
        #if we are in a PP, then it cannot be an object
        noun=noun["up"]
        if noun["pos"]=="PP" or noun["pos"]=="TOP":
            return None            
        
    #looking for a S
    while verb["pos"]!="S":
        verb=verb["up"]
        if verb["pos"]=="TOP":
            return None

    if verb==noun:
        return [depname,None,None]
    return None

def testcomp(depname,x,dps):
    noun=x[0]
    verb=x[1]

    if noun["case"]!="Dat":
        return None

    np=noun["up"]
    if np["pos"]!="NP":
        return None
    pp=np["up"]
    if pp["pos"]!="PP":
        return None
    prep=pp["node"][0]
    if "case" in prep and prep["case"]=="Gen":
        return None
    #looking for a S
    while noun["pos"]!="S":
        noun=noun["up"]
        if noun["pos"]=="TOP":
            return None
    #looking for a S
    while verb["pos"]!="S":
        verb=verb["up"]
        if verb["pos"]=="TOP":
            return None
    if verb==noun:
        return [depname,None,None]
    return None

def testcompverb(depname,x,dps):
    verbiagad=x[0]
    verb=x[1]

    if "case" not in verbiagad or verbiagad["case"]!="Dat":
        return None

    vp=verbiagad["up"]
    if vp["pos"]!="VP":
        return None
    pp=vp["up"]
    if pp["pos"]!="PP":
        return None
    prep=pp["node"][0]
    if "case" in prep and prep["case"]=="Gen":
        return None
    #looking for a S
    while verbiagad["pos"]!="S":
        verbiagad=verbiagad["up"]
        if verbiagad["pos"]=="TOP":
            return None
    #looking for a S
    while verb["pos"]!="S":
        verb=verb["up"]
        if verb["pos"]=="TOP":
            return None
    if verb==verbiagad:
        return [depname,None,None]
    return None


def testcompnde(depname,x,dps):
    noun=x[0]
    nouncomp=x[1]

    if nouncomp["case"]!="Dat":
        return None

    npcomp=nouncomp["up"]
    if npcomp["up"]["pos"]=="NP":
        npcomp=npcomp["up"]
        
    if npcomp["pos"]!="NP":
        return None

    pp=npcomp["up"]
    if pp["pos"]!="PP":
        return None

    #If the last element of the PP is a verb we do nothing
    if pp["node"][-1]["pos"]=="VP":
        return None

    prep=pp["node"][0]
    if not "case" in prep or prep["case"]!="Gen" or prep["meaning"]!="de":
        return None
    if prep["antecedentcase"]!=noun["case"]:
        return None
    return [depname,None,None]
    
def testcompnmod(depname,x,dps):
    noun=x[0]
    nouncomp=x[1]

    if nouncomp["case"]!="Dat":
        return None

    npcomp=nouncomp["up"]
    if npcomp["up"]["pos"]=="NP":
        npcomp=npcomp["up"]
        
    if npcomp["pos"]!="NP":
        return None

    pp=npcomp["up"]
    if pp["pos"]!="PP":
        return None

    #If the last element of the PP is a verb we do nothing
    if pp["node"][-1]["pos"]=="VP":
        return None

    prep=pp["node"][0]
    if not "case" in prep or prep["case"]!="Gen" or prep["meaning"]=="de":
        return None
    if prep["antecedentcase"]!=noun["case"]:
        return None
    return [depname,None,None]


def testcompamod(depname,x,dps):
    noun=x[0]
    adjcomp=x[1]

    if adjcomp["case"]!="Dat":
        return None

    npcomp=adjcomp["up"]
    if npcomp["pos"]!="NP":
        return None
    pp=npcomp["up"]
    if pp["pos"]!="PP":
        return None

    #If the last element of the PP is a verb we do nothing
    if pp["node"][-1]["pos"]=="VP":
        return None

    prep=pp["node"][0]
    if not "case" in prep or prep["case"]!="Gen" or prep["meaning"]=="de":
        return None
    #We test the antecedent case of the preposition
    #to check which noun it should be connected to
    if prep["antecedentcase"]!=noun["case"]:
        return None
    return [depname,None,None]

def testcompvmod(depname,x,dps):
    noun=x[0]
    verbcomp=x[1]

    if "case" not in verbcomp or verbcomp["case"]!="Dat":
        return None

    vpcomp=verbcomp["up"]
    if vpcomp["pos"]!="VP":
        return None
    pp=vpcomp["up"]
    if pp["pos"]!="PP":
        return None
    prep=pp["node"][0]

    if not "case" in prep or prep["case"]!="Gen" or prep["meaning"]=="de":
        return None

    #We test the antecedent case of the preposition
    #to check which noun it should be connected to
    if prep["antecedentcase"]!=noun["case"]:
        return None

    #They should share the same S
    while pp["pos"]!="S":
        pp=pp["up"]
        if pp["pos"]=="TOP":
            return None

    while noun["pos"]!="S":
        noun=noun["up"]
        if noun["pos"]=="TOP":
            return None

    if noun!=pp:
        return None
    
    return [depname,prep,x[0]]


def antecedentpron(pron,wrd):
    if "antecedentgender" in pron:
        if not "gender" in wrd:
            return False
        if wrd["gender"]!=pron["antecedentgender"]:
            return False

    if "proper" in pron:
        if not "proper" in wrd:
            return False

    if "loc" in pron:
        if not "loc" in wrd:
            return False
        
    if "temp" in pron:
        if not "temp" in wrd:
            return False
        
    return True

def testdetant(depname,x,dps):
    noun=x[0]
    det=x[1]
    if "possessive" not in det:
        return None
    if "antecedentgender" not in det or "antecedentcase" not in det:
        return None
    if noun["gender"]==det["antecedentgender"] and noun["case"]==det["antecedentcase"]:
        return [depname,None,None]
    return None

def testpron(depname,x,dps):
    pron=x[1]
    if "antecedentcase" not in pron:
        return None
    
    noun=x[0]    
    if noun["case"]==pron["antecedentcase"]:        
        if not antecedentpron(pron,noun):
            return None;
        if "ANTECEDENT" in dps:
            ant=dps["ANTECEDENT"]
            ips=0
            rmv=-1
            for u in ant:
                try:
                    if u[1]==pron:
                        #It is a more remote one than the current one
                        #we keep only the closest to the pronoun
                        if u[2]["idx"]<noun["idx"]:
                            #we then remove that dependency from our set
                            rmv=ips
                            break
                except:
                    pass
                ips+=1
            if rmv!=-1:
                ant.pop(ips)
        return [depname,None,None]
    return None

def testpronadj(depname,x,dps):
    pron=x[1]
    if "antecedentcase" not in pron:
        return None

    adj=x[0]
    if adj["up"]["node"][-1]!=adj:
        return None
    if adj["case"]==pron["antecedentcase"]:
        if not antecedentpron(pron,adj):
            return None;
        return [depname,None,None]
    return None


def testnouncomp(depname,x,dps):
    noun=x[0]
    noungen=x[1]

    if noungen["case"]!="Gen":
        return None
    npgen=noungen["up"]
    np=noun["up"]
    if "conj" in np:
        np=np["up"]
        
    if np["up"]["pos"]=="PP":
        np=np["up"]
    if npgen["up"]["pos"]=="NP":
        npgen=npgen["up"]
    if np["i"]==npgen["i"]-1 and np["up"]==npgen["up"]:
        return [depname,None,None]    
    return None


def testnoundat(depname,x,dps):
    noun=x[0]
    noungen=x[1]
    
    if noungen["case"]!="Gen":
        return None
    if noun["case"]!="Dat":
        return None

    npnoun=noun["up"]
    npgen=noungen["up"]
    if npnoun["i"]==npgen["i"]-1 and npnoun["up"]["up"]==npgen["up"]:
        return [depname,None,None]
    return None
    


def testneg(depname,x,dps):
    if "neg" in x[0]:
        return [depname,None,None]
    return None

def testinter(depname,x,dps):
    if "inter" in x[0]:
        return [depname,None,None]
    return None

def testnum(depname,x,dps):
    np=x[0]["up"]
    npnoun=x[1]["up"]
    if np!=npnoun:
        return None
    if "num" in x[0]:
        return [depname,None,None]
    return None

def testadj(depname,x,dps):
    adj=x[0]
    noun=x[1]

    if adj["case"]!=noun["case"]:
        return None
    if adj["up"]==noun["up"]:
        return [depname,None,None]
    return None

def testcomparison(depname,x,dps):
    adj1=x[0]
    adj2=x[1]
    #we compare two adjectives together with mikar..mikar or minar..minar or ekar...ekar
    #we get the mother of each
    pp1=adj1["up"]["up"]
    pp2=adj2["up"]["up"]
    if pp1["pos"]!="PP" or pp2["pos"]!="PP":
        return None
    #If should not be the same PP
    if pp1==pp2:
        return None

    #we check the prepositions
    prep1=pp1["node"][0]
    prep2=pp2["node"][0]
    if prep1["meaning"]!=prep2["meaning"]:
        return None
    if prep1["meaning"] not in ["mik","min","ek"]:
        return None
    
    depn=depname+"_"+prep1["meaning"]
    return [depn,None,None]


def testverbmain(depname,x,dps):
    #only one element
    verb=x[0]
    if verb["rel"]=="-" and verb["tense"]!="Inf":
        return [depname,None,None]
    return None

def testverbmainrel(depname,x,dps):
    #only one element
    verb=x[0]
    if verb["rel"]=="+" and verb["tense"]!="Inf":
        return [depname,None,None]
    return None

def testverbpresent(depname,x,dps):
    #only one element
    verb=x[0]
    if verb["tense"]=="Pres":
        return [depname,None,None]
    return None

def testverbpast(depname,x,dps):
    #only one element
    verb=x[0]
    if verb["tense"]=="Past":
        return [depname,None,None]
    return None

def testpunct(depname,x,dps):
    if x[0]["surf"]==".":
        return None
    return [depname,None,None]

def testverbfuture(depname,x,dps):
    #only one element
    verb=x[0]
    if verb["tense"]=="Fut":
        return [depname,None,None]
    return None

def testverbconditional(depname,x,dps):
    #only one element
    verb=x[0]
    if verb["tense"]=="Cond":
        return [depname,None,None]
    return None

def testnounmain(depname,x,dps):
    #only one element
    noun=x[0]
    if noun["case"]!="Nom":
        return None
    np=noun["up"]
    if np["up"]["pos"]=="S":
        if np["up"]["up"]["pos"]=="TOP":
            return [depname,None,None]
    elif np["up"]["pos"]=="TOP":
        return [depname,None,None]        
    return None

def testsex(depname,x,dps):
    #only one element
    noun=x[0]
    if noun["gender"]=="neutral":
        return None
    if noun["gender"]=="fem":
        return ["FEMALE",None,None]
    return ["MALE",None,None]

def testproper(depname,x,dps):
    if "PROPER" in dps:
        ps=dps["PROPER"]
        for p in ps:
            if p[1]==x[0]:
                return None
    if "proper" in x[0]:
        return [depname,None,None]
    return None

def testproperadj(depname,x,dps):
    adj=x[0]
    noun=x[1]
    if adj["up"]!=noun["up"]:
        return None
    if "proper" in adj and "proper" in noun:
        return [depname,None,None]
    return None

def testloc(depname,x,dps):
    if "loc" in x[0]:
        return [depname,None,None]
    return None

def testtemp(depname,x,dps):
    if "temp" in x[0]:
        return [depname,None,None]
    return None


def testrule(depname,x,dps):
    #ifer...komer...
    #if...then...
    pronif=x[0]
    pronthen=x[1]
    if pronif["meaning"].lower()=="if" and pronthen["meaning"].lower()=="kom":
        return [depname,None,None]
    return None

def testpronverbrel(depname,x,dps):
    pron=x[0]
    verb=x[1]
    if verb["rel"]=="-":
        return None
    if verb["up"]["up"]==pron["up"]:
        return [depname,None,None]
    return None

def testpronverbsansant(depname,x,dps):    
    pron=x[0]
    if "antecedentcase" in pron or pron["meaning"]!="k":
        return None
    
    verb=x[1]
    if verb["rel"]=="-":
        return None
    if verb["up"]["up"]==pron["up"]:
        return [depname,None,None]
    return None

def testrule_verb(depname,x,dps):
    if "RULE" in dps:
        pron=x[0]
        verb=x[1]
        if pron["meaning"]=="if":
            s=verb["up"]["up"]
            if s==pron["up"]:
                return [depname,None,None]
    return None

def testrule_verb2(depname,x,dps):
    if "RULE" in dps:
        pron=x[0]
        verb=x[1]
        if pron["meaning"]=="if":
            s=verb["up"]["up"]
            if s!=pron["up"]:
                return [depname,None,None]
    return None

def testdet(depname,x,dps):
    det=x[0]
    noun=x[1]

    #Particular case: mu --> plural form
    if det["up"]==noun["up"]:
        if "pl" in det:
            setplural(det["up"])
        return [depname,None,None]
    return None

def testdetadj(depname,x,dps):
    det=x[0]
    adj=x[1]
    np=adj["up"]

    if det["up"]==np:
        if np["node"][-1]==adj:
            #Particular case: mu --> plural form
            if det["meaning"].lower()=="mu":
                setplural(det["up"])
            return [depname,None,None]
    
    return None

def testverbinf(depname,x,dps):
    verbinf=x[0]
    verb=x[1]

    if verbinf["tense"]!="Inf":
        return None
    
    if verb["tense"]=="Inf":
        return None

    #they should share the same upper node
    if verb["up"]==verbinf["up"]:
        return [depname,None,None]

    return None

#With beiag, we have a specific case, the previous verb should be infinitive
def testverbinfbeiag(depname,x,dps):
    verbinf=x[0]
    verbbeiag=x[1]

    if verbbeiag["meaning"]!="be":
        return None                 

    if verbinf["tense"]!="Inf":
        return None
    
    #they should share the same upper node
    if verbbeiag["up"]==verbinf["up"]:
        ##they should be next to each other
        if verbinf["i"]==verbbeiag["i"]-1:
            return [depname,None,None]

    return None

def testprepnoun(depname,x,dps):
    prep=x[0]

    #In this case, if a verb is the last element of the group, we do nothing
    if prep["up"]["node"][-1]["pos"]=="VP":
        return None
    
    noun=x[1]
    if noun["case"]!="Dat":
        return None
    noun=noun["up"]
    try:
        if noun["up"]["pos"]=="NP":
            noun=noun["up"]
        if prep["up"]!=noun["up"]:
            return None
        return [depname,None,None]
    except:
        return None

def testverbAdv(depname,x,dps):
    Adv=x[0]
    verb=x[1]
    vp=verb["up"]
    if Adv["up"]!=vp:
        return None
    #The verb should be next to it, there should be no other verb in between, only adverbs
    lsts=vp["node"][Adv["i"]+1:]
    for l in lsts:
        if l==verb:
            return [depname,None,None]
        if l["pos"]=="Verb":
            return None
    return None

def testAdjAdv(depname,x,dps):
    Adv=x[0]
    adj=x[1]
    if Adv["up"]!=adj["up"]:
        return None
    return [depname,None,None]

def testprepadj(depname,x,dps):
    prep=x[0]

    #In this case, if a verb is the last element of the group, we do nothing
    if prep["up"]["node"][-1]["pos"]=="VP":
        return None
    
    adj=x[1]
    if adj["case"]!="Dat":
        return None

    #It shoud be the last one
    if adj["up"]["node"][-1]!=adj:
        return None
    
    if prep["up"]!=adj["up"]["up"]:
        return None
    return [depname,None,None]

def testprepverb(depname,x,dps):
    prep=x[0]
    verb=x[1]
    if verb["tense"]!="Inf":
        return None

    #It shoud be the last one
    if verb["up"]["node"][-1]!=verb:
        return None
    
    if prep["up"]!=verb["up"]["up"]:
        return None
    return [depname,None,None]

def testpronverb(depname,x,dps):
    pron=x[0]["up"]
    verb=x[1]
    if x[0]["case"]=="Gen":
        return None
    
    #It shoud be the last one
    if verb["up"]["node"][-1]!=verb:
        return None
    
    if pron["up"]!=verb["up"]["up"]:
        return None
    return [depname,None,None]

def testtemps(depname,x,dps):
    return [depname,x[0]["tense"],None]

def testsubjnul(depname,x,dps):
    verb=x[0]
    if verb["tense"]=="Inf":
        return None
    
    if "MAIN" in dps:
        lstmain=dps["MAIN"]
        for l in lstmain:
            if verb==l[1]:
                key=dependencies.buildelement(None,verb)
                if "ARG" in dps:
                    for arg in dps["ARG"]:                        
                        if len(arg)>=4 and arg[1]==verb and arg[3]==1:
                            return None
                return [depname,None,None]
    return None

def testconjunction(depname,x,dps):
    Adv=x[0]
    if "case" in Adv and Adv["case"]=="Gen":
        return None
    
    verb=x[1]    
    if "rel" not in verb or verb["rel"]=="+":
        return None
    #We go up to the S
    try:
        while verb["pos"]!="S":
            verb=verb["up"]
        #The conjunction should right before the sentence
        if verb["i"]==Adv["i"]+1:
            return [depname,None,None]
    except:        
        return None
    return None

def TestConjPron(depname,x,dps):
    adv=x[0]
    if "case" not in adv:
        return None
    pron=x[1]
    if adv["up"]==pron["up"]:
        return [depname,None,None]
    return None
#######################################################################
###IT IS VERY IMPORTANT TO DEFINE THE ARITY OF EACH DEPENDENCY, WHEN IT IS
###LARGER THAN 1
arity={}
#MOD=["MOD","nom","acc","gen","dat","inf","Pron","tense"]
arity["ARG"]=[None,None,None,None,None,None,None,None,None]
arity["COMPARE"]=[None,None]
arity["COORDINATION"]=[None,None]
arity["CLAUSE"]=[None,None]
arity["RULE"]=[None,None,None,None]
arity["QUANTITY"]=[None,None]
arity["PROPER"]=[None,None]
arity["PUNCT"]=[None,None]
arity["DETERMINER"]=[None,None]
arity["CLAUSE"]=[None,None]
arity["PPE"]=[None,None]
arity["PRONCOMP"]=[None,None]

#######################################################################
###Dependency rules
# The first argument is the dependency with its slot number, the other
# arguments are the nodes. The last argument is a Boolean function, which
# checks the relations between the nodes. Each node is associated to a variable
# that defines its importance. #1 is the key, while #2 is the second node that
# will stored on the slot number associated to the dependency itself.

subject = ["ARG#1","Noun#2","Verb#1",testsubj]
amod = ["ARG#1", "Adj#2","Noun#1",testadj]
subjectpron = ["ARG#1","Pron#2","Verb#1",testsubjpron]

obj = ["ARG#2","Noun#2","Verb#1",testobj]
objpron = ["ARG#2","Pron#2","Verb#1",testobjpron]
obj_adj = ["ARG#2","Adj#2","Verb#1",testobj]
obj_prep = ["ARG#2","Noun#2","Verb#1",testobjprep]

genpron = ["ARG#3","Pron#2","Verb#1",testgenpron]
nmod = ["ARG#3","Noun#1","Noun#2",testnouncomp]
nmoddative = ["ARG#3","Noun#1","Noun#2",testnoundat]
ncmpprepde = ["ARG#3","Noun#1","Noun#2",testcompnde]

complementwithpron = ["PRONCOMP","Verb#2","Verb#1",testcompwithpron]

complementpron = ["ARG#4","Verb#2","Verb#1",testcomppron]
nounprepverbcomp = ["ARG#4","Noun#2","Verb#1",testprepnounverb]
complement = ["ARG#4", "Noun#2","Verb#1",testcomp]
complement_adj = ["ARG#4", "Adj#2","Verb#1",testcomp]
complement_verb = ["ARG#4", "Verb#2","Verb#1",testcompverb]

adverbVerb = ["ARG#6","Adv#2","Verb#1",testverbAdv]
adverbAdj = ["ARG#6","Adv#2","Adj#1",testAdjAdv]
adverbNoun = ["ARG#6","Adv#2","Noun#1",testAdjAdv]
prepnoun = ["ARG#6","Prep#2","Noun#1",testprepnoun]
prepadj = ["ARG#6","Prep#2","Adj#1",testprepadj]
prepverb = ["ARG#6","Prep#2","Verb#1",testprepverb]
mod_rule_verb = ["ARG#6","Pron#2","Verb#1",testpronverbrel]


vmodprep = ["ARG#7","Noun#1","Verb#2",testcompvmod]
nmodprep = ["ARG#7","Noun#1","Noun#2",testcompnmod]
anmodprep = ["ARG#7","Adj#1","Noun#2",testcompnmod]
amodprep = ["ARG#7","Noun#1","Adj#2",testcompamod]
vmodadjprep = ["ARG#7","Adj#1","Verb#2",testcompvmod]
temps = ["ARG#8","Verb#2",testtemps]

antecedent = ["ANTECEDENT","Noun#2","Pron#1",testpron]
antecedentprep = ["ANTECEDENT","Noun#2","Prep#1",testpron]
antecedentadj = ["ANTECEDENT","Adj#2","Pron#1",testpronadj]
antecedentdet = ["ANTECEDENT","Noun#2","Det#1",testdetant]


negation = ["NEG","Verb#1",testneg]
question = ["QUESTION","Verb#1",testinter]

quantitydet = ["QUANTITY","Det#2","Noun#1",testnum]
quantityadj = ["QUANTITY","Adj#2","Noun#1",testnum]

person = ["PROPER","Adj#2","Noun#1",testproperadj]
personsol = ["PROPER","Noun#1",testproper]
loc = ["LOCATION","Noun#1",testloc]
tempo = ["TEMPORAL","Noun#1",testtemp]
gender =["GENDER","Noun#1",testsex]

comparison = ["COMPARE","Adj#1","Adj#2",testcomparison]
comparison_Adj_Noun = ["COMPARE","Adj#1","Noun#2",testcomparison]
comparison_Verb_Verb = ["COMPARE","Verb#1","Verb#2",testcomparison]

principalverb = ["MAIN","Verb#1",testverbmain]
principalnoun = ["MAIN","Noun#1",testnounmain]
principalrel = ["MAIN","Verb#1",testverbmainrel]
presentverb = ["PRESENT","Verb#1",testverbpresent]
pastverb = ["PAST","Verb#1",testverbpast]
futureverb = ["FUTURE","Verb#1",testverbfuture]
conditionalverb = ["CONDITIONAL","Verb#1",testverbconditional]

rule = ["RULE#1", "Pron#1","Pron#2", testrule]
rule_verb = ["RULE#2","Pron#1","Verb#2",testrule_verb]
rule_verb2 = ["RULE#3","Pron#1","Verb#2",testrule_verb2]
mod_rule_verb_clause = ["CLAUSE","Pron#2","Verb#1",testpronverbsansant]

determiner = ["DETERMINER","Det#2","Noun#1",testdet]
determineradj = ["DETERMINER","Det#2","Adj#1",testdetadj]

infinitiveverb = ["ARG#5","Verb#2","Verb#1", testverbinf]
infinitivebeiag = ["PPE","Verb#2","Verb#1", testverbinfbeiag]

pronverb = ["CLAUSE","Pron#2","Verb#1",testpronverb]

coordverb= ["COORDINATION","Verb#1","Verb#2",testcoordverb]
coordnoun= ["COORDINATION","Noun#1","Noun#2",testcoordnoun]
coordadj= ["COORDINATION","Adj#1","Adj#2",testcoordnoun]
coordprep= ["COORDINATION","Prep#1","Prep#2",testcoordprep]

punctuation= ["PUNCT","Punct#1","-1#2",testpunct]

subjnul = ["SUBJNUL","Verb#1",testsubjnul]
conjunctionverb = ["HEADCONJUNCTION","Conjunction#2","Verb#1",testconjunction]
conjunctionpron = ["HEADCONJUNCTION","Conjunction#2","Pron#1",TestConjPron]

argumentrule = [determiner,determineradj,temps,gender,quantitydet,quantityadj,
                  subject,obj,obj_adj,obj_prep,antecedent,antecedentadj,antecedentdet,antecedentprep,
                subjectpron,objpron,genpron,
                  nmod,nmoddative,complement,complementpron,complementwithpron,nounprepverbcomp,comparison_Adj_Noun,comparison_Verb_Verb,
                  complement_adj,complement_verb,ncmpprepde,nmodprep,amodprep,anmodprep,vmodprep,vmodadjprep,amod,negation,question,comparison,
                  principalverb,principalrel,principalnoun,person,personsol,loc,rule,rule_verb,rule_verb2,
                  mod_rule_verb,mod_rule_verb_clause,adverbVerb,tempo,adverbAdj,adverbNoun,
                  prepnoun,prepadj,coordverb,coordnoun,coordadj,coordprep,prepverb,pronverb,
                  presentverb,pastverb,futureverb,conditionalverb,punctuation,
                  infinitiveverb,infinitivebeiag,subjnul,conjunctionverb,conjunctionpron]
