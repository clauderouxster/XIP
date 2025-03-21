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

def compatible(b,p):
   pb=b.rfind("_")
   b=b[:pb] 
   if "_" in b:
      lb=b.split("_")
      lb=lb[1:]
   else:
      lb=[b]

   pp=p.rfind("_")
   p=p[:pp] 
   if "_" in p:
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

   if p[4]!=None and p[4]!=b[4]:
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
            

def cleanconcept(c):
    lc=c.split("_")
    if lc[-1].isdigit():
        return lc[:-1]
    return lc


def buildconceptlist(sentencegr):
   conceptgr={}
   for c in sentencegr:
       lc=cleanconcept(c)
       for l in lc:
           if l=="":
               continue
           conceptgr[l]=True
       for cc in sentencegr[c]:
           for i in sentencegr[c][cc]:
               if islist(i):
                   lc=cleanconcept(i[0])
                   for l in lc:
                       if l=="":
                           continue
                       conceptgr[l]=True
               else:
                   lc=cleanconcept(i)
                   for l in lc:
                       if l=="":
                           continue
                       conceptgr[l]=True
                   break
   return conceptgr
    

def getid(x):
    px=x.rfind("_")
    X=int(x[px+1:])
    return X

def trieclef(x,y):
    X=getid(x)
    Y=getid(y)
    if X<Y:
        return -1
    if X==Y:
        return 0
    return 1

def trimatch(x,y):
    if x[0]<y[0]:
        return -1
    if x[0]==y[0]:
        return 0
    return 1

def testpredicate(lp,lb,varias):
     #if the two keys are compatible
     #Each element is stored at a specific location, which should be shared by the two list
     for ilp in lp:
        if ilp not in lb or not ontologytest(lp[ilp],lb[ilp],varias):
            return False
     return True

def multiplematch(grsentence,gr,storevariable):
   #See dependencies.computekey to see where the keys are computed
   
   try:
       if (gr["KEY"] & grsentence["KEY"])!=gr["KEY"]:
           return False
   except:
       pass

   sentencegr=gr["ARGUMENTS"]   
   sentencebase=grsentence["ARGUMENTS"]
   grkey=sentencegr.keys()
   grkey.sort(trieclef)
   basekey=sentencebase.keys()
   basekey.sort(trieclef)
   
   
   #p stands for projector
   #b stands for basis
   
   try:
      for v in gr["VARIABLES"]:
         gr["VARIABLES"][v]={}
   except:
      pass
      
   varias={}
   sets=[]
   #first we organize our match in one single sequence
   for p in grkey:
      for b in basekey:      
          if compatible(b,p)!=0:
             sets.append([getid(b),p,b,0])
   
   #which we sort out according to the index in the word sentence.  
   sets.sort(trimatch)
   #We extract continuous sections
   
   xs=0
   projection=False
   for s in sets:
       if s[3]!=0:
           xs+=1
           continue
       found=True
       sx=xs
       for p in grkey:
           if p!=sets[sx][1]:
               found=False
               break
           sx+=1
       if found:
           sx-=1
           #The last element has 3 as tag
           val=3
           projection=True
           for p in grkey:               
               sets[sx][3]=val
               #The other elements will have 2
               val=2
               sx-=1
           #the first element has 1 as tag, or if we have an unique
           #concept: 4 (3+1)
           #We re-increment sx to point to the right element (which would the first one)
           sx+=1
           if sets[sx][3]==3:
               sets[sx][3]=4
           else: 
               sets[sx][3]=1                 
       xs+=1    
       
   solution=False 
   if projection:
       skip=False
       varias={}
       nbsolutions=0
       for s in sets:
           #We try to find one element
           if s[3]==0:
               continue
           [i,p,b,tag]=s
           if tag in [1,4]:
               skip=False
               varias={}
           elif skip:
               continue 
           lp=sentencegr[p]
           lb=sentencebase[b]
           #If it does not match, we skip the whole sequence
           if not testpredicate(lp,lb,varias):
               skip=True
               continue
           #Everything is fine
           if tag in [3,4]:
               #we have reached the last element of the sequence
               #we store it
               if storevariable:
                   for v in varias:
                       gr["VARIABLES"][v]={nbsolutions:varias[v]}
                   nbsolutions+=1
                   solution=True                    
               else:
                   for v in varias:
                       gr["VARIABLES"][v]=varias[v]
                   return True
                                   
              
   return solution

def match(grsentence,gr):
    return multiplematch(grsentence,gr,False)

def project(grsentence,gr):
    res=matchmultiple(grsentence,gr,False)
    
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
    
