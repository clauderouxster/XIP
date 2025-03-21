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
      instanciate(p,b,varias,1)
      return True

   ret=intersection(b[-2],p[-2])
   if ret:
      instanciate(p,b,varias,1)
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
            
            
def match(grsentence,gr):
   try:
       idx=gr["__idx__"]
   except:
       idx=-1
   sentencegr=gr["ARGUMENTS"]
   sentencebase=grsentence["ARGUMENTS"]

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
         


def project(grsentence,gr):
    res=match(grsentence,gr)
    
    if res==True:
        res=1
        #We test whether we have a NEG in grsentence, that does not exist in gr
        if "NEG" in grsentence:
            #we check if an element from gr matches the content of NEG
            baseneg=grsentence["NEG"]
            grkeys=gr["KEYS"]
            gneg=[]
            #We extract the NEG from gr
            if "NEG" in gr:
                gneg=gr["NEG"]

            #Do we have this element in gr?
            for b in baseneg:
                #is b also an element of gr?
                if b in grkeys:
                   #is b in the NEG list of gr?
                    if b not in gneg:
                        #If not, this is a inverse solution
                        return 2
        return 1
    else:
        return 0
    
