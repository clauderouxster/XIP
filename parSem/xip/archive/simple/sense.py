######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: sense.py
#Comment: This file implements the equivalence between a graphe and a translation
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

import grammar
import semantics
import sensedata

#These dependencies should not be used in the projection algorithm as they are too linked to
#sepecific syntactic contexts.
Dependencies2Skip=["QUESTION","NEG","MAIN","PRESENT","FUTURE","PAST","CONDITONAL","PRONCOMP",
                   "SUBJNUL","DELETED","VARIABLES"]

###Buidl graphs out of senses###
def buildsenses(dico,roots,language):
    for s in sensedata.senses.keys():
        mytranslations=sensedata.senses[s]
        idx=0
        for mytrans in mytranslations:
            idx+=1
            if "graph" in mytrans:
                continue
            mysent=mytrans["wordata"]
            mysent+="."
            deleted=[]
            variables={}
            if "$" in mysent:
                sp=mysent.split(" ")
                for w in sp:
                    if w[-1]=="$":
                        deleted.append(w[:-1])
                mysent=mysent.replace("$","")

            if "#" in mysent:
                sp=mysent.split(" ")
                mysent=[]
                for w in sp:
                    if w[-2]=="#":
                        variables[w[-2:]]=w
                        mysent.append(w[-1]+"_"+w[:-2])
                    elif w[-3]=="#": #punctuation
                        variables[w[-3:-1]]=w[:-1]
                        mysent.append(w[-2]+"_"+w[:-3])
                    else:
                        mysent.append(w)
                mysent=" ".join(mysent)

            #Then we parse the sentence
            gr={}
            sentence=[]
            lexicals=[]
            #We parse the sentence using our grammar to build a graph out of it
            try:
                s=grammar.parse(mysent,sentence,lexicals,dico,roots,gr,False,None,language,None)
                if variables!={}:
                    gr["VARIABLES"]=variables
                if deleted!=[]:
                    gr["DELETED"]=deleted
                #gr["INITIAL"]=mysent
                #we then store our graph into senses
                mytrans["graph"]=gr
                gr["__idx__"]=str(idx)+"_"+s
            except grammar.Eraiag,e:
                res="Exception: "+e.error+" --> "+e.message
                print res
                print mysent
                print "For the sense:",s,mytrans

#wx is the base word
#node is our current node
#ending the word ending, to recreate it with "re" as root
#grbase is our sentence graph
def gettranslationongraph(wx,node,ending,grbase,language,removed,ontologies):
    if "SENSE" in node:
        return node["SENSE"]
    rt="re"+ending
    lstroots=[wx,rt]
    if ontologies!=None:
        if "meaning" in node and node["meaning"] in ontologies:
            for x in ontologies[node["meaning"]]:
                x+=ending
                lstroots.append(x)
    for wrd in lstroots:
        if wrd in sensedata.senses:
            mytranslations=sensedata.senses[wrd]
            for mytrans in mytranslations:
                
                if language in mytrans:
                    gr=mytrans["graph"]
                    if len(mytrans[language])!=1:
                        continue
                    #If we have already projected with this graphe, we skip to another
                    try:
                        if gr['__idx__'] in grbase['__projection__']:
                            continue
                        grbase["__projection__"].append(gr['__idx__'])
                    except:
                        grbase["__projection__"]=[gr['__idx__']]
                    #we project our semantic graph into it
                    res=semantics.match(grbase,gr)       
                    if res:                        
                        #Then this is the right translation, we send it back (if it exists for that language)
                        node["SENSE"]=[1,mytrans[language]]
                        #If some words are to be deleted, we keep a track of them
                        if "DELETED" in gr:
                            removed.extend(gr["DELETED"])
                        if "VARIABLES" in gr:
                            node["APPEND"]={}
                            for i in gr["VARIABLES"]:
                                node["APPEND"][i]=gr["VARIABLES"][i]
                        return mytrans[language]
    node["SENSE"]=""
    return ""



def appenddependenciesongrah(wx,node,ending,grbase,language):
    if "APPEND" in node:
        return False
    rt="re"+ending
    for wrd in [wx,rt]:
        if wrd in sensedata.senses:
            mytranslations=sensedata.senses[wrd]
            for mytrans in mytranslations:                
                if language in mytrans:
                    gr=mytrans["graph"]
                    if len(mytrans[language])==1:
                        continue      
                    #we project our semantic graph into it
                    res=semantics.match(grbase,gr)       
                    if res:                        
                        dep=mytrans[language]
                        structure=[dep[0]]
                        for i in dep[1:]:
                            structure.append(gr["VARIABLES"][i])
                        node["APPEND"]=structure
                        return True
    node["APPEND"]=None
    return False
