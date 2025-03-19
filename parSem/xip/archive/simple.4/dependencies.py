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

#########################################################################
#Dependencies
import grammarcodes
#print grammar.test("Huma Lisafon fonern hufa husetas lingaieg vidaiag.",gr)
from dependencyrules import *


####################################################################
##Key computing: bitsize is the maximum number of bits that will be kept
def getsem(dp,lst):
    if "loc" in dp:
        lst.append("+loc")
        return
    if "num" in dp:
        lst.append("+num")
        return
    if "temp" in dp:
        lst.append("+temp")
        return
    if "proper" in dp:
        lst.append("+proper")
    
bitsize=256
def computekey(dp,gr):
    if gr==None:
        return
    if "KEY" not in gr:
        gr["KEY"]=0
    x=dp["rootbasis"]
    #X is a string
    #current an integer
    #we keep 2048 classes
    lst=x.split("_")
    getsem(dp,lst)
    for l in lst:
        if l=="":
            continue
        if l[0]!="+":
            if not l.isalpha() or l=="re":
                continue
        key=1<<(hash(l.lower())%bitsize)
        gr["KEY"]|=key
    

####################################################################
#Application of argument rules
def buildelement(graphe,dpe):        
    root=grammarcodes.extractroot(dpe)
    root=grammarcodes.putlower(dpe,root)
    try:
        root+="_"+str(dpe["iroot"])
        root+=str("/"+dpe["meaning"])
        computekey(dpe,graphe)
    except:
        pass
    return root

#We have two cases. Either the position of the second argument into the graph is already defined with #x,
#either the dependency arity is restricted to 2 elements at most
#posarg is the index that has been provided together with the dependency name in the rule. It is used to position
#the second argument in the graph structure. The first element is the key to that structure
def addelements(graphes,dp,posarg):
    #first we use the first element as our main key...!
    #The structure of dp is the following:
    # dp[1] is the first argument of the dependency
    # dp[2] is the second argument. When only one argument is available, we use addelement.
    root=buildelement(graphes,dp[1])
    arg=buildelement(graphes,dp[2])

    depname=dp[0]
    ar=None
    try:
        xroot=graphes[root]
    except:
        try:
            ar=arity[depname][:]
        except:
            ar=[None,None]
        xroot=ar
        graphes[root]=xroot
        xroot[0]=depname
    if xroot[posarg]==None:
        xroot[posarg]=arg
    else:#There is a value for it already, we replace it with a set
        try:
            if arg not in xroot[posarg]:
                xroot[posarg].append(arg)
        except:
            xroot[posarg]=[xroot[posarg],arg]
    
#In this case, the rule was not given with a specific position value 
def addarguments(graphes,dp):
    #first we use the first element as our main key...!
    arg=[buildelement(graphes,dp[1]),buildelement(graphes,dp[2])]
    depname=dp[0]

    try:
        #we extract the set associated to that specific dependency
        xroot=graphes[depname]
        xroot.append(arg)
    except:
        #It is a new dependency of that sort
        ar=[None,None]
        xroot=[arg]
        graphes[depname]=xroot


#Only one argument is added to the graph. In this case, we use the dependency itself as the key: PRESENT
def addargument(graphes,dp):
    #the root is the dependency itself
    root=dp[0]
    arg=grammarcodes.extractjustroot(dp[1])

    try:
        if arg not in graphes[root]:
            graphes[root].append(arg)
    except:
        graphes[root]=[arg]

#In this case, the position of the only argument is given with its position within the structure.
#It is again a dependency with a position: ARG#7
def addelement(graphes,dp,val,posarg):
    depname=dp[0]
    root=buildelement(graphes,dp[1])

    try:
        xroot=graphes[root]
    except:
        try:
            ar=arity[depname][:]
        except:
            ar=[None,None]
        xroot=ar
        graphes[root]=xroot
        xroot[0]=depname
    if xroot[posarg]==None:
        xroot[posarg]=val
    else:#There is a value for it already, we replace it with a set
        try:
            if val not in xroot[posarg]:
                xroot[posarg].append(val)
        except:
            xroot[posarg]=[xroot[posarg],val]
    
def lastsibling(s):
    if "node" in s:    
        return lastsibling(s["node"][-1])
    return s

def firstsibling(s):
    if "node" in s:    
        return firstsibling(s["node"][0])
    return s
    
def apply_rule_arg(sentence,categories,rule,graphes,localgraphe,dps):
    #A rule is a set of potential list of categories
    #The first element is the final category
    graphestoappend=[]
    [r,kleene,p]=grammarcodes.get(rule[1])
    dname=rule[0]
    posarg=1
    keyonword=False
    if len(dname)>3 and dname[-2]=="#":
        posarg=int(dname[-1])
        dname=dname[:-2]
        keyonword=True
    if r in categories:
        lst=categories[r]
        for nodeleft in lst:
            if len(rule)==4:
                [r2,kleene2,p2]=grammarcodes.get(rule[2])
                nblast=None
                #If the category is given (the second argument only) as a digit, it is used as a way
                #to takle the element placed at this valeur from the current element 
                try:
                    off=int(r2)
                    if off>0:
                        nblast=firstsibling(sentence[nodeleft["i"]+off])
                    else:
                        nblast=lastsibling(sentence[nodeleft["i"]+off])
                    r2=nblast["pos"]
                except:
                    pass
                if r2 in categories:
                    if nblast==None:
                        lstbis=categories[r2]                        
                    else:
                        lstbis=[nblast]
                    idx=nodeleft["idx"]
                    #We check then the position of the nodes, to see if they are in the right order (given by the rule)
                    for noderight in lstbis:
                        #The order is the reverse, we continue
                        if noderight["idx"]<=idx:
                            #Except if we are dealing with a specific offset, in this case, it was
                            #imposed by the rule itself.
                            if nblast==None:
                                continue
                        phr=[nodeleft,noderight]
                        #we apply the rule, associated with our rule itself (last element) 
                        try:
                            depname=rule[-1](dname,phr,dps)
                        except:
                            depname=None
                        if depname!=None:
                            #The rule returs the name of the dependency and the two arguments
                            [depname,arg1,arg2]=depname
                            dp=[depname,None,None,posarg]
                            #By default, each of the argument is None, but the rule
                            #might impose its own value, otherwise we take the two default nodes l and lb
                            if arg1==None:
                                arg1=nodeleft
                            if arg2==None:
                                arg2=noderight

                            dp[p]=arg1
                            dp[p2]=arg2
                            try:
                                dps[depname].append(dp)
                            except:
                                dps[depname]=[dp]
                            graphestoappend.append([2,depname,keyonword,dp,posarg])
            else: #one element at a time
                resdep=rule[-1](dname,[nodeleft],dps)
                if resdep!=None:
                    depname=resdep[0]
                    dp=[depname,nodeleft,posarg]
                    try:
                        dps[depname].append(dp)
                    except:
                        dps[depname]=[dp]
                        
                    graphestoappend.append([1,depname,resdep[1],dp,posarg])
    #creategraphes(graphestoappend,graphes,localgraphe,dps)
    createprojector(graphestoappend,graphes,localgraphe,dps)
                

    
def createprojector(graphestoappend,graphes,localgraphe,dps):
    if "ARGUMENTS" not in graphes:
        graphes["ARGUMENTS"]={}
    if "ARGUMENTS" not in localgraphe:
        localgraphe["ARGUMENTS"]={}
    for g in graphestoappend:
        depname=g[1]
        test=g[2]
        dp=g[3]
        posarg=g[4]
        #If it has been destroyed, we do not create its graphs.
        if dp not in dps[depname]:
            continue
        if g[0]==2:
            #ROOT= ROOT_ID
            root=grammarcodes.extractminroot(dp[1])
            root=grammarcodes.putlower(dp[1],root)
            if root not in graphes["ARGUMENTS"]:
                graphes["ARGUMENTS"][root]={}
            if root not in localgraphe["ARGUMENTS"]:
                localgraphe["ARGUMENTS"][root]={}
            #ARG=[ROOT,POS,VARIABLE,ID,MEANING,WORK]    
            arg0=grammarcodes.extractrootset(dp[1])
            graphes["ARGUMENTS"][root][0]=arg0   
            localgraphe["ARGUMENTS"][root][0]=arg0                     
            arg=grammarcodes.extractrootset(dp[2])
            try:                
                graphes["ARGUMENTS"][root][posarg].append(arg)
                localgraphe["ARGUMENTS"][root][posarg].append(arg)
            except:
                graphes["ARGUMENTS"][root][posarg]=[arg]
                localgraphe["ARGUMENTS"][root][posarg]=[arg]
            if test:
                addelements(graphes,dp,posarg)
        else:
            if test!=None:
                addelement(graphes,dp,test,posarg)
            else:
                #In the case of mono-dependencies, we keep a track of them
                #It is especially useful for NEG and QUESTION
                addargument(graphes,dp)
            


#def creategraphes(graphestoappend,graphes,localgraphe,dps):
#    for g in graphestoappend:
#        depname=g[1]
#        test=g[2]
#        dp=g[3]
#        posarg=g[4]
#        #If it has been destroyed, we do not create its graphs.
#        if dp not in dps[depname]:
#            continue
#        if g[0]==1:
#            if test!=None:
#                addelement(graphes,dp,test,posarg)
#                addelement(localgraphe,dp,test,posarg)
#            else:
#                addargument(graphes,dp)
#                addargument(localgraphe,dp)
#        else:
#            if test:
#                addelements(graphes,dp,posarg)
#                addelements(localgraphe,dp,posarg)
#            else:
#                addarguments(graphes,dp)
#                addarguments(localgraphe,dp)
            