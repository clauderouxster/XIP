######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: semantics.py
#Comment: This file implements the graph projection
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

#########################################################################
#Semantic representation
#GRAPHES and their utilization
#We project gr onto grsentence

debugmode=False

def printd(*arg):
   if debugmode:
      for a in arg:
         print a,
      print

def islist(s):
   if type(s)==type([]):
      return True
   return False

def myconcept(es):
   if es[0].isdigit() and es[1]=="_":
      return es[2:]
   lastvariable=""
   return es

def testvar(es):
   if es[0].isdigit() and es[1]=="_":
      return True
   return False
   
def getlemma(a):
   p=a.split('"')
   if len(p)==1:
      return a

   return p[1]
   

#We need to remove the indexes
def remove(a):
    if a==None:
        return a

    if not islist(a):
       return getlemma(a)
    
    res=[]
    for i in a:
        if i==None:
            continue
        if islist(i):
           rs=remove(i)
           res.extend(rs)
        else:
           p=getlemma(i)
           res.append(p)
    return res
        

def thekeys(g):
    res={}
    for i in g.keys():
        s=remove(i)
        try:
            res[s].append(i)
        except:
            res[s]=[i]
    return res

def thepos(g):
    res={}
    for i in g.keys():
        s=remove(i)
        if s!=i:
           pos=i.split("#")[0]
           try:
               res[pos].append(i)
           except:
               res[pos]=[i]
    return res


#For the moment only one general match is accepted: re
def TestConcept(e,ebase):
   if e=="re" or e in ebase:
      return True
   return False

def ExtractConcepts(conc,pos,basepos,baseconc):
   #If we use "re" which is the empty word
   lookin=[]
   if conc=="re":
      #Then we extract all potential keys: they should have a common POS                   
      if pos in basepos:
         lookin=basepos[pos]
   else:
      lookin=baseconc[conc]
   return lookin

#The root: "re" has a specific meaning, which is almost empty sauf their semantics interpretation:
#rete --> place
#reta --> thing
#reti --> time
#reto --> person
#For this specific root, we only test the features, and not the root. This root can be seen as
# a simplified ontological element.

def projection(grsentence,gr,skip):
    #baseconc is a list where elements from grsentence are stripped from their Id and their root used as key.
    #We also removed any dependencies from within and keep only the words these keys are connected to    
    baseconc=thekeys(grsentence)
    #Words that share a common POS
    basepos=thepos(grsentence)
    for conc in gr.keys():
        #Certain elements of GR might be skipped
        if conc in skip:
            continue
        #We strip conc from its internal index.
        rconc=remove(conc)
        #The word "re" is used as a semantic variable
        if TestConcept(rconc,baseconc):
            i=-1
            pos=conc.split("#")[0]                               
            #conc is a word, which has been associated with other elements through a specific relation also
            #referred to as a dependency.
            #gr[conc] contains a list of words with their id number, the first element is the DEPENDENCY name
            for E in gr[conc]:
                i+=1
                if E==None:
                    continue
                
                #E is a word
                #We do not use the tenses in the projection
                if E in ["Pres","Fut","Past","Cond","Inf"]:
                   continue
                findsolution=False
                printd("E=",E,rconc,conc) 
                #We might have more than one keys matching rconc (the root index)
                lookin=ExtractConcepts(rconc,pos,basepos,baseconc)
                printd("Lookin=",lookin)
                
                #lookin is a list of keys
                for c in lookin:
                    #if E is an ARG, which is the most complex dependency around, we then browse the 
                    if E=="ARG":
                        try:
                            #c is associated to a list of element
                            Ebase=grsentence[c][i]
                        except:
                            continue
                    else:
                        #It is not a list of element, but a simple string
                        Ebase=grsentence[c]
                    #we strip E from its indexes
                    e=remove(E)
                    printd("e=",e,E)
                    #we also string Ebase from its indexes
                    ebase=remove(Ebase)
                    printd(ebase,e)
                    #Then e and ebase are both reduced to their root form
                    #four cases
                    #1 e is a list
                    if islist(e):
                        #if ebase is also a list, the two lists should be equal
                        if islist(ebase):
                           Findsolution=True
                           for elem in e:
                              if not TestConcept(elem,ebase):                                 
                                Findsolution=False
                                break
                        #if ebase is not a list, it is an error
                    #if e is not a list, then if ebase is a list
                    elif islist(ebase):
                        #then e must be part of that list
                        #"RE" is an empty word, which is used to match everytime
                        if TestConcept(e,ebase):
                           findsolution=True
                           break
                    #else if e and ebase are singletons then they should be equal
                    elif TestConcept(e,[ebase]):
                        findsolution=True
                        break
                if not findsolution:
                    printd("ECHEC 1",ebase,e)
                    return 0
        else:
           printd("ECHEC 2",rconc,conc)
           return 0
    printd("REUSSITE")
    return 1


def project(grsentence,gr,skip):
    res=projection(grsentence,gr,skip)
    
    if res==1:
        #We test whether we have a NEG in grsentence, that does not exist in gr
        if "NEG" in grsentence:
            #we check if an element from gr matches the content of NEG
            baseneg=remove(grsentence["NEG"])
            grkeys=thekeys(gr)
            gneg=[]
            #We extract the NEG from gr
            if "NEG" in gr:
                gneg=remove(gr["NEG"])

            #Do we have this element in gr?
            for b in baseneg:
                #is b also an element of gr?
                if b in grkeys:
                   #is b in the NEG list of gr?
                    if b not in gneg:
                        #If not, this is a inverse solution
                        return 2
    return res
    



def getlemmapos(a):
   p=a.split('"')
   if len(p)==1:
      return [a,"d",None,None]
   s=p[1]
   x=None
   sense=p[-1].split("/")
   sense=sense[-1]

   if s[0].isdigit():
      pos=s.find("_")
      if pos!=-1 and s[:pos].isdigit():
          x="#"+s[:pos]
          s=s[pos+1:]
   num=p[0][p[0].find("#")+1:p[0].find("[")]
   return [s,p[0][0:3].lower(),x,num,sense,False]
   

def displayph(ph):
   i=0
   for p in ph:
      if p[0]!=None:
         try:
            print p[0]+"_"+p[1],
         except:
            print p,
      else:
         print i,
      i+=1
   print

def affiche(ph):
    for i in ph:
        if type(i)==type([]):
            for p in ph:
                affiche(ph)
        else:
            if type(ph)==type({}):
                print i,":",ph[i]
            else:
                print i
            

def computekey(ph):
   s="IDX"
   for i in ph:
      if i==None:
         continue
      if islist(i):
         s+=computekey(i)
      else:
         s+="_"+i
   return s


def instanciate(p,b,varias,ty):
    if p[2]!=None:
       if ty==1:
         varias[p[2]]=b[3]
       else:
         varias[p[2]]=-1


def intersection(b,p):
   if b[0]=="_":
      lb=b.split("_")
      lb=lb[1:]
   else:
      lb=[b]
   
   if p[0]=="_":
      lp=p.split("_")
      lp=lp[1:]
   else:
      lp=[p]

   testontological=False
   for l in lb:
      if l[-1]=="$":   
         testontological=True
         continue
      if l in lp:
         return 1

   if testontological:
      for l in lb:
         if l[-1]!="$":   
            continue
         l=l[:-1]
         if l in lp:
            return 2


   return 0
   
#p[1] --> pos
#p[0] --> lem
#b is the sentence
#p[-1] --> root
def ontologytest(p,b,varias):
   if p==None:
      return True
   if b==None:
      return False

   #If p is a list, then each element of p should be in b
   if islist(p[0]):
       #We the prepare B for the match sequence
       if islist(b[0]):
           for i in b:
               i[-1]=False
       for i in p:
            if not ontologytest(i,b,varias):
                return False
       return True
   
   #If b is a list, then at least P should be in b 
   #Here P cannot be a list, as this case has been treated above
   #One match is enough
   if islist(b[0]):
       for i in b:
            if i[-1]:
                continue
            if ontologytest(p,i,varias):
                i[-1]=True
                return True
       return False
        
   if p[1]!=b[1]:
      return False
   if p[0]=="re":
      instanciate(p,b,varias,1)
      return True

   ret=intersection(b[0],p[0])
   if ret:
      instanciate(p,b,varias,ret)
      return True

   ret=intersection(b[-2],p[-2])
   if ret:
      instanciate(p,b,varias,ret)
      return True
          
   return False


def nodecomparison(p,b):
   if islist(p[0]):
      #Then each element of p should have a match in b
      if islist(b[0]):         
         for i in p:
            found=False
            for j in b:
               if ontologytest(i,j):
                  found=True
                  break
            if not found:
               return [False,None]
         return [True,None]
      else:
         #Else in this case, this is an error
         return [False,None]
   else:
      xb=None
      found=False
      if islist(b[0]):
         found=False
         #At least one
         for ib in b:               
            if ontologytest(p,ib):
               xb=ib
               found=True
               break
      else:
         if ontologytest(p,b):
            xb=b
            found=True
            
      return [found,xb]


#We build a structure out of our graph for each ARG
#Each element is stored at the index position corresponding to the ARGUMENT position
#Each element of sentence is the key (with some modification) of the ARG structure.
def extractpos(gr,sentence):
    if type(gr)==type({}):
        for i in gr:
            try:
                if gr[i][0]=="ARG":
                    si=getlemmapos(i)
                    #Our new key in the sentence is our graph key
                    ky=si[0]+"_"+str(si[3])
                    #We insert the element in our sentence with this new key 
                    sentence[ky]={}
                    #The first element of that new dictionary is the complete interpretation of that element
                    sentence[ky][0]=si
                    ipos=1
                    for kg in gr[i][1:-1]:
                        #we skip the NULL element
                        if kg==None:
                            ipos+=1
                            continue
                        #If it is a set, it means that the element had the same argument many times
                        #Each element stored in this structure is a list: [lemme,pron,variable,index,meaning,WorkField]
                        if type(kg)==type([]):
                            sol=[]
                            for kx in kg:
                                res=getlemmapos(kx)
                                sol.append(res)
                            if sol!=[]:
                                #we store it as a list of complex element
                                sentence[ky][ipos]=sol
                        else:
                            #Only one element, which also computable
                            res=getlemmapos(kg)
                            #We store our complex element in it...
                            sentence[ky][ipos]=res
                        ipos+=1                                                
            except:
                pass
            
            
def match(wrd,grsentence,gr):
   idx=gr["__idx__"]
   if "__LINEAR__" in gr:
      sentencegr=gr["__LINEAR__"]
   else:
      sentencegr={}
      extractpos(gr,sentencegr)
      gr["__LINEAR__"]=sentencegr

   if "__LINEAR__" in grsentence:
      sentencebase=grsentence["__LINEAR__"]
   else:
      sentencebase={}
      extractpos(grsentence,sentencebase)
      grsentence["__LINEAR__"]=sentencebase

   try:
      for v in gr["VARIABLES"]:
         gr["VARIABLES"][v]=-1
   except:
      pass
      
   lstgr=[]
   thekeys=[]
   varias={}

   for p in sentencegr:
      found=False
      lp=sentencegr[p]
      for b in sentencebase:
         lb=sentencebase[b]
         #already consummed
         if b in thekeys:
            continue
         if ontologytest(lp[0],lb[0],varias):
             #if the two keys are compatible
             test=True
             #Each element is stored at a specific location, which should be shared by the two list
             for ilp in lp:
                if ilp not in lb or not ontologytest(lp[ilp],lb[ilp],varias):
                   test=False
                   break
             if not test:
               continue
             found=True
         if found:
            thekeys.append(b)
            lstgr.append([lb[0][3],idx])
            break
      if not found:
         #print "Erreur:",displayph(pgr)
         return False
   for v in varias:
     gr["VARIABLES"][v]=varias[v]
       
   for [i,g] in lstgr:
      try:
         grsentence[i].append(g)
      except:
         grsentence[i]=[g]
   return True
         
