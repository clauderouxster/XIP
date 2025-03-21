# -*- coding: utf-8 -*-
######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: translation.py
#Comment: This file implements the generation of a sentence in French out of a mekalinga sentence
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

import sys
import grammarcodes
import sense
import catexpressn
import xips

print xips.whoami()

#########################################################################
#Generation
punct=[",",";","[","(",")","]","{","}"]
seps=grammarcodes.seps
cheminbase=sys.path[0]
if grammarcodes.currentlanguage=="english":
    cheminxip=cheminbase+seps+"english"+seps+"GRMFILES"+seps+"GRAM_GEN"+seps
else:
    cheminxip=cheminbase+seps+"francais"+seps

print cheminxip
    
echange={}
echange["ARG1"]="SUBJ"
echange["ARG1Noun"]="AMOD"
echange["ARG1Adj"]="AMOD"
echange["ARG2"]="OBJ"
echange["ARG3"]="NMOD"
echange["ARG4"]="VMOD"
echange["ARG5"]="VINF"
echange["ARG6"]="PMOD"
echange["ARG6Verb"]="PVMOD"
echange["ARG7"]="PREPMOD"
echange["ARG6Pron"]="RMOD"
echange["ARG6Adv"]="ADVMOD"
echange["ARG6AdvVerb"]="ADVMOD"
echange["MAIN1"]="MAIN"
echange["COORDINATION1"]="COORD"
echange["NEG1"]="NEG"
echange["PRESENT1"]="PRESENT"
echange["PAST1"]="PAST"
echange["CONDITIONAL1"]="CONDITIONNEL"
echange["ANTECEDENT1"]="ANTECEDENT"
echange["PUNCT1"]="PUNCTUATION"
echange["DETERMINER1"]="DETERM"
echange["CLAUSE1"]="CONJMOD"
echange["PPE1"]="PPE"
echange["PRONCOMP1"] = "COMPMOD"
echange["COMPARE1"] = "COMPARE"
echange["SUBJNUL1"] = "SUBJ"
echange["HEADCONJUNCTION1"] = "HEAD"
echange["RULE1"] = "IFTHEN"
echange["CLAUSE1"] = "CLAUSE"

traitsexchange={'tense':'tense',
                'gender':'gender',
                'rel':'rel',
                'pronsubj':'pronsubj',
                'pers':'pers',
                'sg':'sg',
                'pl':'pl',
                'case':'case',
                'def':'def',
                'proper':'proper',
                'loc':'loc',
                'temp':'temp',
                'num':'num',
                'conj':'conjcoord'}


#These rules are used to create dependencies out of node sequences
generationrules={"francais":{},"english":{}}

#prep,noun --> pmod
generationrules["francais"]["prep"]=[{"rule":[["prep",None],["prep","de"],["noun",None]],
                                      "dependency":[["PMOD",2,0],["COMBINE",0,1],["~PMOD",2,1]]},
                                     {"rule":[["prep",None],["noun",None]],"dependency":[["PMOD",1,0]]}]

generationrules["francais"]["det"]=[{"rule":[["det",None],["noun",None]],"dependency":[["DETERM",1,0]]}]
generationrules["francais"]["verb"]=[{"rule":[["verb",None],["prep$",None]],"dependency":[["POBJMOD",0,1]]}]


generationrules["francais"]["pron"]=[ {"rule":[["pron","il",["case:nom"]],["pron","y"],["verb","avoir"]],
                                       "dependency":[["VMOD",2,1],["SUBJ",2,0]]},
                                      {"rule":[["pron","se"],["verb",None]],"dependency":[["VMOD",1,0]]}]

generationrules["francais"]["noun"]=[ {"rule":[["noun",None],["adj",None]],"dependency":[["AMOD",0,1]]}]
#We keep the sequence adj--noun
generationrules["francais"]["adj"]=[ {"rule":[["adj",["preN:+"]],["noun",None]],"dependency":[["AMOD",0,1]]}]
#---------------------------------English----------------------
#prep,noun --> pmod
generationrules["english"]["prep"]=[{"rule":[["prep",None],["noun",None]],"dependency":[["PMOD",1,0]]}]

generationrules["english"]["det"]=[{"rule":[["det",None],["noun",None]],"dependency":[["DETERM",1,0]]}]
generationrules["english"]["verb"]=[{"rule":[["verb",None],["prep$",None]],"dependency":[["POBJMOD",0,1]]}]
generationrules["english"]["noun"]=[ {"rule":[["noun",None],["adj",None]],"dependency":[["AMOD",0,1]]}]


def createnodeargument(node,lembase):
    lem=""
    for i in lembase:
        if i in punct:
            lem+="%"+i
        else:
            lem+=i
            
    w=node["pos"]+"#"+str(node["idx"])+'[lemme="'+lem+'"'
    for i in node.keys():
        try:
            w+=","+traitsexchange[i]+":"+node[i].lower()
        except:
            pass
    w+="]"
    return w
    
#We use addnewdependency to add new dependencies (XIP way) into the common pool of dependencies
#that will be sent out to the generator
def addnewdependency(grbase,sentence,node,dico,root,language,newdeps,nbwrds,newwrds):
    wrdbase=grammarcodes.getending(node["surf"])
    ending=""
    if "basis" in node:
        ending=node["basis"]                        
    if sense.appenddependenciesongrah(wrdbase,node,ending,grbase,language):
        dep=node["APPEND"][0]+"(0,"
        first=True
        for idp in node["APPEND"][1:]:
            if not first:
                dep+=","
            first=False
            dep+=sentence[int(idp)]["wbase"]
        dep+=")."
        newdeps.append(dep)

#grbase is the graph of our sentence
#node is our current node
#dico is our translation dico
#root is our root based dictionary
#newdeps is our enrichments of the local dependencies
def parsexpression(x):
    fx=catexpressn.readstring(x)
    lret=[]
    tree={}
    res=catexpressn.m_parsecat(fx,lret,tree,0)
    if res==True:
       return tree
    return None
    

def getfromtree(tree,values):
    try:        
        name=tree["%NAME"]
        value=tree["%VALUE"]
        if name=="cat":
            values.append([value])
        elif name=="var":
            values[-1].append(value)
        elif name=="word":
            values[-1].append(value[:-1])
    except:
        pass

    try:
        for l in tree["%NODES"]:
            getfromtree(l,values)
    except:
        pass

    


def replacewitharg(sentence,translated,node,dico,root,language):
    rxf=parsexpression(translated)
    lst=[]
    getfromtree(rxf,lst)
    lstwrd=[]
    recreate=False
    ps=-1
    ref=""
    for w in lst:
        [cat,lem]=w
        lem=lem.replace(","," ")
        key=-1
        argn=lem
        theref=False
        suite=[]
        if lem[0]=="#":
            try:
                key=int(node["APPEND"][lem])
                n=sentence[key]
                argn=grammarcodes.translate(n,dico,root,language)
                argn=argn.split("|")[0]
                if key==node["idx"]:
                    theref=True
                    #If argn is an expression from within the lexicon
                    if "(" in argn:
                        #we parse it
                        treeargn=parsexpression(argn)
                        lstargn=[]
                        getfromtree(treeargn,lstargn)
                        findocc=False
                        for la in lstargn:
                            #Up to the category we are looking for
                            #we add new elements
                            #When we find our element (the same category as the current one)
                            #We have our lemma. Then we add for latter use in suite
                            #the remaining
                            if findocc==True:
                                suite.append(la)
                            elif la[0].lower()!=cat.lower():
                                lstwrd.append([la[0],la[1],-1,False])
                            else:
                                #This will be our current lemma
                                argn=la[1]
                                findocc=True
            except:
                argn="???"
                print "Unbounded variable:",lem

        try:
            lem=unicode(argn,"utf-8")
        except:
            lem=argn                        

        if cat[0]=="^":
            cat=cat[1:]
            key=node["idx"]
            recreate=True
            ps=len(lstwrd)

        if theref:
            ref=lem

        lstwrd.append([cat,lem,key,theref])
        for ss in suite:
            lstwrd.append([ss[0],ss[1],-1,False])
        
    if recreate:
        nb=0
        for l in lstwrd:
            if nb==ps:
                ref=l[1]
                l[3]=True
                nb+=1
                continue
            if l[3]:
                l[2]=-1
                l[3]=False
                break
            nb+=1
    return [ref,lstwrd]


def computearg(skey,key,cat,lem,newwrds,nbwrds):
    if key==-1:            
        w=cat+"#"+skey+'[lemme="'+lem+'"'
        if w not in newwrds:
            idx=len(newwrds)+nbwrds+1    
            newwrds[w]=idx
        else:
            idx=newwrds[w]
        w=cat+"#"+str(idx)+'[lemme="'+lem+'"]'
    else:
        w=cat+"#"+str(key)+'[lemme="'+lem+'"]'
    return w

def createdependencies(lstwrd,language,node,newdeps,newwrds,nbwrds):
    if lstwrd!=None:
        skipdeps=[]
        for ix in range(0,len(lstwrd)-1):
            [cat,lem,key,theref]=lstwrd[ix]
            cat=cat.lower()
            try:
                rls=generationrules[language][cat]
                for r in rls:
                    ilst=iter(lstwrd[ix:])
                    error=False
                    try:
                        for ele in r["rule"]:
                            elst=ilst.next()
                            catrule=ele[0].lower()
                            Shouldbein=None
                            if catrule[-1] in ["^","$"]:
                                Shouldbein=catrule[-1]
                                catrule=catrule[:-1]                                
                                
                            if catrule!=elst[0].lower():
                                error=True
                                break
                            if ele[1]!=None and ele[1].lower()!=elst[1].lower():
                                error=True
                                break
                            if Shouldbein=="$" and elst!=lstwrd[-1]:
                                error=True
                                break
                            if Shouldbein=="^" and elst!=lstwrd[0]:
                                error=True
                                break
                    except:
                        error=True
                    if not error:
                        skey=str(node["idx"])
                        #We create our dependency
                        for dr in r["dependency"]:                            
                            dep=dr[0]+"(0,"
                            skip=False
                            if dep[0]=="~":
                                skip=True
                                dep=dep[1:]
                            p1=dr[1]+ix
                            p2=dr[2]+ix
                            [cat,lem,key,theref]=lstwrd[p1]
                            w1=computearg(skey,key,cat,lem,newwrds,nbwrds)
                            if len(r["rule"][p1-ix])>2:
                                f=",".join(r["rule"][p1-ix][2])
                                w1=w1[:-1]+","+f+"]"
                            [cat,lem,key,theref]=lstwrd[p2]
                            w2=computearg(skey,key,cat,lem,newwrds,nbwrds)
                            if len(r["rule"][p2-ix])>2:
                                f=",".join(r["rule"][p2-ix][2])
                                w2=w2[:-1]+","+f+"]"
                            dep+=w1
                            dep+=","+w2+")."
                            if skip:
                                skipdeps.append(dep)
                            else:
                                if dep not in skipdeps:
                                    #We add the new dependency                    
                                    if dep not in newdeps:
                                        newdeps.append(dep)
            except:
                pass
    
def argument(sentence,grbase,node,dico,root,language,newdeps,nbwrds,newwrds,removed,ontologies):
    lstwrd=None

    #wbase is used to keep a track of a node that has already been created
    if "wbase" not in node:
        #Either we have a specific translation associated to a graph
        wrdbase=grammarcodes.getending(node["surf"])
        ending=""
        if "basis" in node:
            ending=node["basis"]                        

        resgr=sense.gettranslationongraph(wrdbase,node,ending,grbase,language,removed,ontologies)
        #either we use the current dictionary
        currentpos=node["pos"]
        arg=grammarcodes.translate(node,dico,root,language)

        #We get the translations from this dictionary for this node
        #We keep only the first one
        a=arg.split("|")
        argreplaces=None
        if resgr=="":
            translated=a[0]
        else:
            translated=resgr[0]

        #In case of a traduction into a sequence of words, we expect
        # the sequence to be: Cat(w1) Cat'(w2) etc...
        lstwrd=None
        if "(" in translated:
            [translated,lstwrd]=replacewitharg(sentence,translated,node,dico,root,language)
            if translated=="":
                #We use the first element sharing the right category
                for l in lstwrd:
                    if l[0].lower()==currentpos.lower():
                        translated=l[1]
                        l[3]=True
                        if l[2]==-1:
                            l[2]=node["idx"]
                        break
                if translated=="":
                    l=lstwrd[0]
                    translated=l[1]
                    l[2]=node["idx"]
                    l[3]=True                
        try:
            lem=unicode(translated,"utf-8")
        except:
            lem=translated
        
        if lem[-1]=="~":
            lem=lem[:-1]
        #####################################################################
        #WE CAN HAVE features in the translation, they occur after a "@"
        ps=lem.find("@")
        featlem=None        
        if ps!=-1:
            inv={}
            inv["sg"]="pl"
            inv["pl"]="sg"
            featlem=lem[ps+1:].split("@")
            lem=lem[:ps]
            for fx in featlem:
                fx=fx.lower()
                if fx in ["masc","fem"]:
                    node["gender"]=fx
                if fx in ["sg","pl"]:
                    if fx not in node:
                        if inv[fx] in node:
                            node.pop(inv[fx])
                        node[fx]="+"
        #########################################################
        w=createnodeargument(node,lem)
        wbase=w
        node["wbase"]=wbase
    else:
        return node["wbase"]
        
    #we then build the dependencies, with the help of our rules
    createdependencies(lstwrd,language,node,newdeps,newwrds,nbwrds)
        
    return wbase

def loadgenere(grm):
    i=xips.loadgrm(grm)
    xips.setdisplay(i,xips.XIP_DISPLAY_LEMMA|xips.XIP_DISPLAY_CHUNK_TREE|xips.XIP_DISPLAY_EXECUTION_ERROR|xips.XIP_DISPLAY_GENERATION|xips.XIP_DISPLAY_TREE)
    return i

localgrm=-1
def load(currentlanguage):
    global localgrm    
    if currentlanguage=="english":
        localgrm=loadgenere(cheminxip+"english.grm")
    else:
        localgrm=loadgenere(cheminxip+"french.grm")

#For each dependency, we extract the correct argument
def generestring(sentence,deps,dico,root,graphesentence,nbwrds,dependencystring,language,ontologies):
    global localgrm  
    resdep=""
    newwrds={}
    newdeps=[]
    #We apply a first loop, where elements are computed (and their result stored in wbase)
    #Which enables us to detect which element to remove
    depsToModify=[]
    for dep in deps.values():
        for d in dep:
            #Each argument might be modified by a semantic graph
            removed=[]
            w1=argument(sentence,graphesentence,d[1],dico,root,language,newdeps,nbwrds,newwrds,removed,ontologies)
            #we remove any dependency that mixes the two roots
            if removed!=[]:
                try:
                    d[1]["removed"].extend(removed)
                except:
                    d[1]["removed"]=removed
            if len(d)==4:
                removed=[]
                w2=argument(sentence,graphesentence,d[2],dico,root,language,newdeps,nbwrds,newwrds,removed,ontologies)
                if removed!=[]:
                    try:
                        d[2]["removed"].extend(removed)
                    except:
                        d[2]["removed"]=removed
            depsToModify.append(d)

    for d in depsToModify:
        #The elements to be removed or skipped.
        #An element is removed only if the key element (the argument of the dependency) AND the element to remove
        #are both in a same dependency.
        if len(d)==4:
            if "removed" in d[1]:
                if d[2]["surf"] in d[1]["removed"]:
                    continue

            if "removed" in d[2]:
                if d[1]["surf"] in d[2]["removed"]:
                    continue
        
        #Each argument might be modified by a semantic graph
        addnewdependency(graphesentence,sentence,d[1],dico,root,language,newdeps,nbwrds,newwrds)
        w1=d[1]["wbase"]
        #we remove any dependency that mixes the two roots
        if len(d)==4:
            addnewdependency(graphesentence,sentence,d[2],dico,root,language,newdeps,nbwrds,newwrds)
            dp=d[0]+str(d[3])
            w2=d[2]["wbase"]
            args="(0,"+w1+","+w2+")"
        else:
            dp=d[0]+str(d[2])
            args="(0,"+w1+")"
        featdep=""
        if "COMPARE" in dp:
            featdep="["+dp[8:-1]+"=+]"
            dp="COMPARE1"
        if dp=="ARG1":                
            #We check the arguments
            if d[1]["pos"]!="Verb":
                dp+=d[1]["pos"]
        if dp=="ARG6":
            if d[2]["pos"]=="Pron" or d[2]["pos"]=="Adv":
                dp+=d[2]["pos"]
            if d[1]["pos"]=="Verb" and d[1]["tense"]=="Inf":
                dp+=d[1]["pos"]
        if dp in echange:
            resdep+=echange[dp]+featdep+args+".\n"
    for dp in newdeps:        
        resdep+=dp+"\n"

    resdep=resdep.encode("iso-8859-1")
    dependencystring.append(resdep)
    sauve=open(cheminxip+"input.txt","w")
    sauve.write(resdep+"\n");
    sauve.close()
    o=[]
    s=""
    try:
        s=xips.generatefromstring(localgrm,resdep,o)
        return o[0]
    except:
        return s

