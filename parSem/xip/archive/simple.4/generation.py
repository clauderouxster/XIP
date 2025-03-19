# -*- coding: utf8 -*-
######################################################
#Author: Claude Roux
#Date: 2009/12/14
#E-mail: mekalinga@free.fr
#File: generation.py
#Comment: Generation grammar from a natural language into mekalingveta
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

import sys
import xips
import translation
import lingata
import grammarcodes
import ending
import objects

debug=True

baseaccents={'é':"e",'è':"e",'ê':"e",'ë':"e",'à':"a",'â':"a",'ä':"a",'ï':"i",'î':"i",'ô':"o",'ö':"o",'ù':"u",'ü':"u",'û':"u"}

accents={}
ks=baseaccents.keys()
for a in ks:
    ee=unicode(baseaccents[a].upper(),"utf8")
    e=unicode(a.upper(),"utf8")
    bb=unicode(baseaccents[a],"utf8")
    b=unicode(a,"utf8")
    accents[e]=ee
    accents[b]=bb
baseaccents=None



def deaccent(x):
    x=unicode(x,"utf8")
    s=""
    for i in x:
        try:
           e=accents[i]
           s+=e
        except:
            s+=i
    return s

def buildidx(frenchToLingve):
    racloc=None
    for i in lingata.lexicons.keys():
         s=lingata.lexicons[i]
         infoword=ending.ending(i)
         pos=infoword["pos"].lower()
         if pos not in frenchToLingve:
             frenchToLingve[pos]={}
         if "francais" in s:
             lct=s["francais"]
             for l in lct:
                 #If our expression is made of POS and lemmas
                 #Then we use this expression as a key for which we keep both
                 #the translation and the POS of the expression
                 if "(" in l:
                     if l not in frenchToLingve:
                         frenchToLingve[l]=[pos.lower(),i]
                 else:
                     try:
                         if i=="beiag":
                             racloc=l
                         #If we have features attached to that entry, we remove it
                         ps=l.find("@")
                         if ps!=-1:
                            l=l[:ps]
                         frenchToLingve[pos][l].append(i)
                     except:
                         frenchToLingve[pos][l]=[i]
    if racloc!=None:
        #We push beiag ahead
        frenchToLingve["verb"][racloc].sort()
        

def addafeat(res,value):
    try:
        res["att"].append(value)
    except:
        res["att"]=[value]
    

cheminbase=sys.path[0]
seps=grammarcodes.seps
cheminxip=cheminbase+seps+"francais"+seps+"grammaire"+seps+"francais.grm"
   
class generation:

    def __init__(self,chemin):
       self.grm=xips.loadgrm(chemin)
       xips.setdisplay(self.grm,xips.XIP_DISPLAY_NONE)
       self.frenchToLingve={}
       buildidx(self.frenchToLingve)
       self.nodes={}
       self.roots={}
       self.endings={"conj":"nu","noun":"ta","det":"la","adj":"ca","adv":"no","conj":"nu","prep":"ar","pron":"er","verb":"iag","conjque":"er","num":"lu"}
       self.pos={"lu":"det","ti":"noun","ci":"adj","ta":"noun","co":"adj","to":"noun","fa":"noun","ma":"noun","la":"det","ca":"adj","no":"adv","nu":"conj","ar":"prep","er":"pron","ag":"verb"}
       self.depidx={"DETERM":0,"SUJV":1,"CODV":2,"CODVPROPQUE":13,"CODVCONJQUE":14,"PREPOBJ":3,"MODN":4,"MODV":5,"COMP":6,"MODA":6,"MODR":7,"ATBSO":8,"CPLV":13,"AUXV":10,"NOUNPHRASE":11,"COORD":12,"NEGAT":14,"PREPCONJ":14}
       self.featuretransfers={"p":["gv","gn"],"gv":["verb","aux"],"gn":["pron","det","noun","adj","ga"],"det":["num"],"gcoord":True,"adv":["adverb"],"ga":["adj"],"verb":["verb"]}    
    
    
    def child(self,xm,xc,position):
        if xc["parent"]==xm or xm==xc:
            return
        if xc["parent"]!=None:
           xc['parent']['children'].remove(xc)
        xc["parent"]=xm
        if position==None:
            xm["children"].append(xc)
        else:
            xm["children"].insert(position,xc)

    def movechild(self,xm,xc,position):
        if xm==xc:
            return
        if xc["parent"]!=None:
           xc['parent']['children'].remove(xc)
        xc["parent"]=xm
        if position==None:
            xm["children"].append(xc)
        else:
            xm["children"].insert(position,xc)
            
         
    def removenode(self,pos,x):
        try:
           hx=self.getparent([pos,x])
           if hx["parent"]!=None:
              hx['parent']['children'].remove(hx)
           hx['parent']=None
           self.nodes[pos.lower()].pop(x[2])
        except:
           pass
         
    def iscase(self,x):
        if x in ['ACC','GEN','DAT','NOM']:
            return True
        return False


    def findcase(self,h):
        for i in h['features']:
            if self.iscase(i):
                return True
        return False


    def find(self,pos,lst):
        for l in lst:
            if pos==l["pos"]:
                return True
        return False
    
    def getnode(self,i):
        try:
            surf=i[3]
        except:
            surf=""
        pos=i[0].lower()
        if pos not in self.nodes:
            self.nodes[pos]={}
        if i[2] not in self.nodes[pos]:
            idx=float(i[2])
            #we keep a track of the highest id in the sentence to create new nodes with new id
            if idx>self.maxidx:
                self.maxidx=idx+1
            lemma=i[1]
            slemma=lemma.split("_")
            thelemma=unicode(slemma[0],"utf8").encode('latin-1')
            thelemma=slemma[0]
            feats=slemma[1:]
            self.nodes[pos][i[2]]={"surface":surf,"lemma":thelemma,"features":[],"children":[],"parent":None,"pos":pos,'translation':[],"id":i[2]}
            if feats!=[]:
                nd=self.nodes[pos][i[2]]
                for f in feats:
                    nd['features'].append(f.upper())

        self.nodes[pos][i[2]]["features"].append("*"+self.currentdep)
        return self.nodes[pos][i[2]]

    def testnode(self,i):
        pos=i[0].lower()
        if pos not in self.nodes:
            return False
        if i[1] not in self.nodes[pos]:
            return False
        return True
        
    def addfeature(self,feats,fs):
        case=self.findcase(feats)
        for f in fs:
            if f not in feats['features']:
                if f=="DAT" and "NO_DAT" not in feats["features"]:
                  feats['features'].append(f)  
                  continue
                #we avoid mixing incoherent case features                
                if self.iscase(f) and case:
                    continue
                feats['features'].append(f)
    
    def getparent(self,root):
        self.roots[root[0].lower()]=True
        return self.getnode([root[0],root[1][1],root[1][2]])

    def topparent(self,n):
        node=self.getnode(n)
        np=node["parent"]
        while np!=None and np["parent"]!=None:
            np=np["parent"]
        return np
    
    def parentphrase(self,root):
        xm=self.getparent(root)
        if xm['children']==[]:
            xc=self.getnode(root[1])
            self.child(xm,xc,None)
        
        feat=root[2]
        if feat!=None:
            if type(feat)==type([]):
                self.addfeature(xm,feat)
            else:
                self.addfeature(xm,[feat])
        return xm

    
    #each node is a pair: [pos,lemma,idx,surface], the phrs is a list of phrases, each corresponding to an element in nodes
    #The root is: [POS,Headword]
    #Example: buidphrase(["P",[Verb,manger,2]],[gn,[Noun,chien,1]],[gv,[Verb,manger,2]]
    #If position==None elements are added at the end, otherwise on position 0
    def buildnode(self,root,nodes,position):
        #First of all, we get the word phrase:
        head=self.parentphrase(root)
        
        for i in nodes:            
            xm=self.parentphrase(i)
            self.child(head,xm,position)
        return head

    def verbtrans(self,w):
        if "REL" in w['features']:
           lst=w['translation']
           w['translation']=[]
           for i in lst:
               wrd=i[:-2]+"e"+i[-1:]
               w['translation'].append(wrd)
        if "INFINITIVE" in w['features'] or "INFINITIVE_PREP" in w['features']:
           lst=w['translation']
           w['translation']=[]
           if 'INFINITIVE' in w['features']:
              add="as"
           else:
              add="ad"
           for i in lst:
               wrd=i+add
               w['translation'].append(wrd)
        else:
            term=None
            if "PAST" in w["features"]:
               term="p"
            elif "FUT" in w["features"]:
               term="f"
            if term!=None:
               for i in range(0,len(w['translation'])):
                   w['translation'][i]=w['translation'][i][:-1]+term
    
    def addcasetoroot(self,w):
        if w['pos']=="verb":
           self.verbtrans(w)
        else:                 
           cs=""
           ics=""
           #In the case of preposition with a dative feature
           #It only means that there is a PP attachment with a GN
           #We transform the DAT into a GEN, so to obtain: kumars
           if w['pos']=="prep" and "DAT" in w['features']:
                w['features'].remove("DAT")
                if "ARGVERB" not in w['features']:
                    w['features'].append("GEN")
           if "DAT" in w['features']:
                cs='d'
           elif "GEN" in w['features']:
                cs='s'
           elif "ACC" in w['features']:
                cs='n'
           #Antecedent features 
           if "ANT_ACC" in w['features']:
                ics='n'
           elif "ANT_GEN" in w['features']:
                ics='s'
           elif "ANT_DAT" in w['features']:
                ics='d'

           quequo=""
           if w["pos"]!="det": 
               if "AND" in w['features']:
                   quequo="que"
               if "OR" in w['features']:
                   quequo="quo"
               
           if cs!="" or ics!="" or quequo!="": 
               lst=w['translation']
               w['translation']=[] 
               for i in lst:
                   xs=i
                   if ics!="":
                      if w['pos']=="pron":
                        xs=xs[:-2]+"i"+ics+xs[-2:]  
                      else:
                        xs=xs[:-2]+"i"+ics+xs[-2:]+"s"
                   xs+=cs+quequo                   
                   w['translation'].append(xs)
                   
        if "NEG" in w["features"]:
            ix=0
            for i in w["translation"]:
                w["translation"][ix]+="ne"
                ix+=1
            
    #if the pos does not match the translation, we MODIFY the translation accordingly
    def analyze(self,w):
        #t,c,l,a,n
        lst=w['translation']
        w['translation']=[]
        for i in lst:
            end=i[-2:]
            try:
                pos=self.pos[end]
            except:
                end=i[-3:-1]
                pos=self.pos[end]
            try:
                mypos=w['pos']
                if pos==mypos or (mypos=="pron" and pos=="noun"):
                   root=i
                else:
                   if pos=="verb":
                      root=i[:-3]
                   else:
                      root=i[:-2]
                   root+=self.endings[mypos]
                if root not in w['translation']:
                    w['translation'].append(root)
            except:
                pass
               
    def gettranslation(self,w):
       basewrd=w['lemma']
       if w['pos']=="det":
            if basewrd=="un":
                if "PL" in w['features']:
                    w['translation']=['mula']
                else:
                    w['translation']=[]
            elif basewrd=="le":
                if "PL" in w['features']:
                    w['translation']=['muala']
                else:
                    w['translation']=['ala']
            else:
               try:
                  tr=self.frenchToLingve["det"][basewrd]
                  w['translation']=tr
               except:
                   pass                
       elif (w['pos']=="conjque" and basewrd=='que') or basewrd=='qui':
           if "KER" in w["features"]:
               w['translation']=["ker"]
           else:
               w['translation']=["kaer"]
       else: 
           if basewrd=="truc" and w['pos']=="noun":
               w['translation']=["reta"]
               return
           if "PROPER" in w['features'] and w['pos'] in ["noun","adj"]:
                if w["pos"]=="noun":
                    lem=basewrd+"to"
                else:
                    lem=basewrd+"co"
                w["lemma"]=w["surface"]
                if w["surface"][0].isupper():
                    lem=lem.capitalize()
                w['translation']=[lem]
                    
                return               
           try:
              thepos=w["pos"]
              if thepos=="pron":
                  thepos="noun"
                  
              tr=self.frenchToLingve[thepos][basewrd]
              if "TO_CONJ" in w["features"]:
                  w["pos"]="conj"
                  lstr=[]
                  for x in tr:
                      lstr.append(x[:-2]+"nus")
                  tr=lstr
              if "TO_PRON" in w["features"]:
                  w["pos"]="pron"
                  lstr=[]
                  for x in tr:
                      lstr.append(x[:-2]+"er")
                  tr=lstr                  
              w['translation']=tr
           except:
              if basewrd[0]=='_':
                w['translation']=[basewrd[1:]]
              else:
                if w["pos"]in ["adj","noun"]:
                  if w["pos"]=="adj":
                      test="noun"
                      term="ca"
                  else:
                      test="adj"
                      term="ta"
                  if basewrd in self.frenchToLingve[test]:
                      tr=self.frenchToLingve[test][basewrd]
                      for x in tr:
                          x=x[:-2]+term
                          w['translation'].append(x)
                      return
                #If this word does not exist and it is a major category
                if w["pos"] in ["adj","noun","verb","num"]:
                    basewrd=deaccent(basewrd)  
                    lem=basewrd+self.endings[w['pos']]
                    w['translation']=[lem]
    
    def translationsorting(self,x,y):
        xe=ending.ending(x)
        ye=ending.ending(y)
        if xe['root'] in self.semanticsorter:
            return -1
        if ye['root'] in self.semanticsorter:
            return 1
        return 0
        
    
    def translate(self,w,modifs):
        if w['children']==[]:
           if "SKIP" in w["features"]:
               w["translation"]=None
               return
           if w["id"] in modifs:
               w["translation"]=modifs[w["id"]]
           else:
               if "KEEP" not in w["features"]:
                   self.gettranslation(w)
               self.analyze(w)
           if w["translation"]!=None:
               w["translation"].sort(self.translationsorting) 
               self.addcasetoroot(w)
        else:
        #we trickle down features to the leaves
            feats=w['features']
            for h in w['children']:
                try:
                    if self.featuretransfers[w["pos"]]==True or h["pos"] in self.featuretransfers[w["pos"]]:
                        self.addfeature(h,feats)
                except:
                    pass
                if "REL" in feats and "REL" not in h["features"]:
                    h["features"].append("REL")
                self.translate(h,modifs)


    
    def getantcase(self,w):
        if "NOM" in w['features']:
            return "ANT_NOM"
        if "ACC" in w['features']:
            return "ANT_ACC"
        if "DAT" in w['features']:
            return "ANT_DAT"
        if "GEN" in w['features']:
            return "ANT_GEN"
        return None
 
    def getcase(self,w):
        if "NOM" in w['features']:
            return "NOM"
        if "ACC" in w['features']:
            return "ACC"
        if "DAT" in w['features']:
            return "DAT"
        if "GEN" in w['features']:
            return "GEN"
        return None
 
    def cleancase(self,w):
        if "NOM" in w['features']:
            w['features'].remove("NOM")
        if "ACC" in w['features']:
            w['features'].remove("ACC")
        if "DAT" in w['features']:
            w['features'].remove("DAT")
        if "GEN" in w['features']:
            w['features'].remove("GEN")
        
            
            
    #The FUNCTIONS
    #Each function creates a specific node. The call is processed through an eval
    #None means that the nodes should be appended
    #0 means that the nodes should be inserted at the beginning of the structure
    def g_SUJV(self,r,feat):
        [dep,phidx,noun,verb]=r
        self.buildnode(["P",verb,feat],[["GN",noun,"NOM"],["GV",verb,None]],None)
        if noun[0]=="CONJQUE":
            hverb=self.getnode(verb)
            hverb=hverb["parent"]
            hverb["features"].append("REL")


    def g_COMP(self,r,feat):
        [dep,phidx,conj,verb]=r
        self.buildnode(["P",verb,"REL"],[["GV",verb,None]],None)
        self.buildnode(["P",verb,"REL"],[["CONJQUE",conj,["KER","ACC"]]],0)
        

    def g_PREPCONJ(self,r,feat):
        [dep,phidx,prep,conj]=r
        hprep=self.getnode(prep)
        hprep=hprep["parent"]
        self.buildnode(['GN',conj,feat],[['Prep',prep,["TO_CONJ"]]],0)
        if hprep!=None and hprep["parent"]!=None:
            hprep["parent"]["children"].remove(hprep)
        
    def g_PREPOBJ(self,r,feat):
        [dep,phidx,noun,prep]=r
        if feat==None:
            feat=["DAT"]
        else:
            feat.append("DAT")
        feat.append("GP")
        if noun[0] in ["NOUN","PRON"]:
            self.buildnode(['GN',noun,feat],[['Prep',prep,None]],0)
        elif noun[0]=="VERB":
           self.buildnode(['GV',noun,feat],[['Prep',prep,None]],0) 
           hn=self.getnode(noun)
           hn['features'].append('INFINITIVE_PREP')


    def g_CODVPROPQUE(self,r,feat):
        [dep,phidx,verbrel,verb]=r
        hnrel=self.getparent(["P",verbrel])
        hnp=self.getparent(["P",verb])
        self.movechild(hnp,hnrel,-1)

    def g_CODVCONJQUE(self,r,feat):
        [dep,phidx,conjque,verb]=r
        [pos,lemma,ix,surf]=conjque
        self.buildnode(["P",verb,"REL"],[["GV",verb,None]],None)
        #we test, if "CONJQUE" has been used as a subject
        if self.testnode(["gn",conjque[2]]):
            hgn=self.topparent(conjque)
            hp=self.getparent(["p",verb])
            self.movechild(hp,hgn,0)
            cq=self.getnode(conjque)
            self.cleancase(cq)
            cq["features"].extend(["KER"])
            #Then we mode the GN conjque node to the top
            hgn=cq["parent"]
            self.movechild(hp,hgn,0)
        else:                
            self.buildnode(["P",verb,"REL"],[["CONJQUE",conjque,["KER","ACC"]]],0)
        
    def g_CODV(self,r,feat):
        [dep,phidx,noun,verb]=r
        [pos,lemma,ix,surf]=noun
        #Vouloir manger
        if pos=="VERB" and verb[0]=="VERB":
           #We check first, if we have Phrase with the first verb
           hpv=self.getparent(["p",verb])
           #If they share the same subject, then, we move everything under the first verb
           hverb=self.getnode(verb)
           if hpv["children"][0]==hverb["parent"]:
               self.buildnode(["GV",noun,feat],[["VERB",verb,None]],None)
               #and we remove "p"
               self.removenode("P",verb)
           #And of course the first verb is an infinitive
           hn=self.getnode(noun)
           hn['features'].append('INFINITIVE')
           return
        if noun[0]=="COORD":
            self.buildnode(["GV",verb,feat],[["gcoord",noun,"ACC"]],-1)
        else:
            self.buildnode(["GV",verb,feat],[["GN",noun,"ACC"]],0)
        #A new word should start with a _lemma so to escape the translation section
        #self.buildnode(["GN",noun,None],[["Det",['Det','_muala',self.idx],None]],0)
        #self.idx+=1
    
    def g_MODN(self,r,feat):
        if feat!=None:
            if "REL" in feat:
                [dep,phidx,verb,noun]=r
                if noun[0]=="CONJQUE":
                    self.buildnode(["GN",noun,feat],[["P",verb,"REL"]],None)
                    #Then we add the case of the antecedent into the conjunction feature list
                    hp=self.getparent(["p",verb])
                    conj=hp['children'][0]
                    hgn=self.getparent(["GN",noun])
                    case=self.getantcase(hgn)
                    if case!=None:
                        conj['features'].append(case)
                else:
                    self.buildnode(["P",verb,feat],[["GN",noun,None]],-1)
                return
            if "NUM" in feat:
                [dep,phidx,num,noun]=r
                if feat==None:
                    feat=["DETERM"]
                else:
                    feat.append("DETERM")
                hgn=self.getparent(["GN",noun])
                if "GP" in hgn["features"]:
                    pos=-1
                else:
                    pos=0
                self.buildnode(["GN",noun,feat],[["det",num,None]],pos)
                return
            
        [dep,phidx,noun,basenoun]=r
        #A past participle
        if noun[0]=="VERB":
           #We force the ADJECTIVE reading
           hd=self.buildnode(["GN",basenoun,feat],[["GV",noun,None]],None)
           hnoun=self.getnode(noun)
           hnoun=hnoun["parent"]["features"].append("INFINITIVE")
        elif noun[0]=="ADJ":
            hd=self.buildnode(["GN",basenoun,feat],[["GA",noun,None]],-1)
        else:
        #if we are dealing with a GP
            hgn=self.getparent(["GN",noun])
            if 'GP' in hgn['features']:
                #we remove the prep in hgn children
                if hgn["children"][0]["pos"]=="prep":
                   hgn["children"].remove(hgn["children"][0])
                   hgn["features"].remove("DAT")
                self.buildnode(["GN",basenoun,feat],[["GN",noun,"GEN"]],None)
            else:
                ps=None
                try:
                    if hgn["children"][-1]["pos"]=="gv":
                        ps=-1
                except:
                    pass                
                self.buildnode(["GN",basenoun,feat],[["GN",noun,"GEN"]],ps)
           

    def g_NOUNPHRASE(self,r,feat):
        [dep,phidx,nounhead,nounadj]=r
        hadj=self.getparent(["GN",nounadj])
        chld=hadj["children"]
        if chld:
            chld.reverse()
            hgn=self.getparent(["GN",nounhead])
            for c in chld[1:]:
                self.child(hgn,c,0)
        self.buildnode(["GN",nounhead,feat],[["GA",nounadj,None]],-1)
        self.removenode("gn",nounadj)
        hadj=self.getnode(nounadj)
        hadj['pos']="adj"
            
    def g_ATBSO(self,r,feat):
        [dep,phidx,adj,verb]=r
            
        if adj[0]=="ADJ":
            self.buildnode(["GV",verb,feat],[["GA",adj,"ACC"]],0)
        elif adj[0]=="NOUN":
            #First we check, it does not have a parent
            hnoun=self.getnode(adj)
            if hnoun["parent"]!=None:
                hnoun=hnoun["parent"]
            hnoun["features"].append("ACC")
            hverb=self.getparent(["gv",verb])
            self.movechild(hverb,hnoun,-1)           

    def g_MODV(self,r,feat):
        [dep,phidx,adv,verb]=r        
        if adv[0] in ["ADV"]:
            #In the case of a negation, the "pas" should not be taken into account...
            if adv[1]=="pas":
                return
            self.buildnode(["verb",verb,feat],[["ADV",adv,None],["verb",[verb[0],verb[1],verb[2]+".4"],None]],None)
            
        if adv[0] in ["NOUN"]:
            hnoun=self.getnode(adv)
            hnp=hnoun["parent"]
            if hnp!=None:
                self.buildnode(["gv",verb,feat],[[hnp["pos"],adv,None]],0)
            else:
                self.buildnode(["gv",verb,feat],[["gn",adv,None]],0)
            

    def g_MODA(self,r,feat):
        [dep,phidx,adv,adj]=r
        self.buildnode(["GA",adj,feat],[["ADV",adv,None]],0)

    def g_MODR(self,r,feat):
        [dep,phidx,advl,advr]=r
        self.buildnode(["ADV",advr,feat],[["ADVERB",advl,None],["ADVERB",[advr[0],advr[1],advr[2]+".5"],None]],None)

    def g_DETERM(self,r,feat):
        [dep,phidx,noun,det]=r
        hdet=self.getnode(det)
        hgn=self.getparent(["GN",noun])
        if 'GP' in hgn['features']:
            position=-1
        else:
            position=0
        if feat==None:
            feat=["DETERM"]
        else:
            feat.append("DETERM")
        self.buildnode(["GN",noun,feat],[["Det",det,None]],position)
    
    #Verb argument
    def g_CPLV(self,r,feat):
        [dep,phidx,noun,verb]=r
        #Particular case: il y a, which we translate with gebi
        if verb[1]=="avoir" and noun[1]=="y":
            hverb=self.getnode(verb)
            hverb["translation"]=["gebeiag"]
            hverb["features"].append("KEEP")
            return
        if noun[0] in ["NOUN","PRON"]:
           if noun[0]=="PRON" and self.testnode(["GV",verb[2]]):
               htop=self.topparent(noun)
               if htop!=None:
                 hgv=self.getparent(["GV",verb])  
                 self.movechild(hgv, htop, None)
                 return 
           self.buildnode(["GV",verb,feat],[["GN",noun,None]],None)
           hgn=self.getparent(['GN',noun])
           hprep=hgn['children'][0]
           if hprep['pos']=='prep':
               hprep['features'].append("ARGVERB")
        elif noun[0]=="VERB":
           self.buildnode(["GV",verb,feat],[["GV",noun,None]],None) 
           hverb=self.getnode(verb)
           hgv=self.getparent(['GV',verb])
           self.movechild(hgv,hverb,None)
        elif noun[0]=="COORD":
            self.buildnode(["GV",verb,feat],[["gcoord",noun,None]],-1)
            

    def g_COORD(self,r,feat):
        [dep,phidx,coord,words]=r
        if coord[1]=="et":
            afeat="AND"
        elif coord[1]=="ou":
            afeat="OR"
        else:
            return

        lastword=words[-1]

        hgn=self.getnode(lastword)
        hp=hgn["parent"]
        feati=hp["features"]
        #Two cases: GP or else
        if "GP" in hp["features"]:
            #Then the conjunction is borne by the preposition
            hp["children"][0]["features"].append(afeat)
        else:
            #it is borne by the current element
            hgn["features"].append(afeat)

        pos=hp["pos"]        
        try:
            toparent=hp["parent"]
            psinparent=toparent["children"].index(hp)
        except:
            toparent=None
            
        for w in words:
            self.buildnode(["GCOORD",coord,feat],[[pos,w,None]],None)
            
        hcoord=self.getnode(coord)
        hcoord["features"].extend([afeat,"SKIP"])
        #We move hcoord to the previous position of HP
        if toparent!=None:
            hcoord["parent"]["features"].extend(feati)
            self.movechild(toparent,hcoord["parent"],0)
        #We keep only the feature intesection
        hpcoord=hcoord["parent"]
        case=self.getcase(hpcoord)
        ret=[]
        for w in words:
            hn=self.getnode(w)
            for f in hpcoord["features"]:
                if f not in hn["features"]:
                    if f not in ret:
                        ret.append(f)
        for f in ret:
            try:
                hpcoord["features"].remove(f)
            except:
                pass
        hpcoord["features"].append(case)          
            

    def g_NEGAT(self,r,feat):
        [dep,phidx,verb]=r
        hn=self.getnode(verb)
        hn["features"].append("NEG")
        

    def g_AUXV(self,r,feat):
        [dep,phidx,aux,verb]=r
        if feat!=None and "PASSIVE" in feat:
            self.buildnode(["GV",aux,feat],[["GV",verb,None]],-1)
            hgv=self.getparent(['GV',verb])
            hgv['features'].append("INFINITIVE")
            return
        hgv=self.getparent(['GV',verb])
        hgv['features'].append("PAST")
        #The subject is linked to the auxiliary
        #We close up the gap between the auxiliary and the verb
        self.buildnode(["GV",aux,feat],[["GV",verb,None]],-1)
        #However, we do not want this form to be generated
        haux=self.getnode(aux)
        haux['features'].append("SKIP")

    #We test a certain node configuation after the initial building of the tree based on dependencies    
    def gn_prep_p(self,node):
        #This specific function transforms a preposition if it is followed by a clause tree (P)
        if "REL" not in node["features"]:
            return
        chld=node["children"]
        if chld[0]["pos"]!="prep":
            return
        for c in chld[1:]:
            #We are looking for a construction: prep,P
            if c["pos"]=="p" and "REL" in c["features"]:
                chld[0]["features"].append("TO_CONJ")
                chld[0]["features"].append("TO_GEN")
                return

    def triage(self,x,y):
        if "INFINITIVE" in x["features"] or "INFINITIVE_PREP" in x["features"]:
            return -1
        return 1
    
    #If the verb is not at the end, we move it    
    def gn_gv_verb(self,node):
        try:
            chld=node["children"]
            lstverb=[]
            lstinf=[]
            for c in chld:
                if c["pos"]in ["verb","gv"]:
                    if "INFINITIVE" in c["features"] or "INFINITIVE_PREP" in c["features"]:
                        lstinf.append(c)
                    else:
                        lstverb.append(c)
            for l in lstverb:
                chld.remove(l)
            for l in lstinf:
                chld.remove(l)
            for l in lstinf:
                chld.append(l)                
            for l in lstverb:
                chld.append(l)                
        except:
            pass 
    
    #If we have a NP containing an infinitive GV, then we modifify this GV into relative linked to that GP
    def gn_gvinf(self,node):
        #first we extract the cases
        cases=self.getantcase(node)
        chld=node["children"]
        for c in chld:
            if c["pos"]=="gv" and "INFINITIVE" in c["features"]:
                c["features"].remove("INFINITIVE")
                if "REL" not in c["features"]:
                    c["features"].append("REL")
                pron=self.getnode(["PRON","de",self.maxidx,"de"])
                self.maxidx+=1
                pron["features"].append(cases)
                pron["translation"].append("deer")
                self.child(c,pron,0)
    #If we have two PREP embedded, the GV one is now a GV pron   
    def gv_gn_prep(self,node):
        try:
            if "GP" in node["features"]:
                chld=node["children"]
                #If we have a prep followed by a gn with a prep
                if chld[0]["pos"]=="prep":
                    i=1
                    while chld[i]["pos"]!="gn" or "GP" not in chld[i]["features"]:
                        i+=1
                        
                    if chld[i]["pos"]=="gn" and "GP" in chld[i]["features"]:
                        #Then, our main verb is no longer a prep
                        node["features"].remove("GP")
                        #The prep is a pronoun
                        chld[0]["features"].append("TO_PRON")
                        node["features"].append("REL")
                        chld[-1]["features"].remove("INFINITIVE_PREP")
        except:
            pass

    def gn_pl(self,node):
        if "PL" in node["features"] and not "DETERM" in node["features"]:
            w=self.getnode(["det","un",self.maxidx,"un"])
            self.maxidx+=1
            if "GP" in node["features"]:
                self.child(node,w,1)
            else:
                self.child(node,w,0)

    #We move the subject at the very beginning of the sentence
    def p_suj(self,node):
       chld=node["children"]
       subj=[]
       first=None
       for c in chld:
           if c["pos"]=="gn":
               try:
                   if first==None and self.find("conjque",c["children"]):                       
                       first=c
                       continue
               except:
                   pass 
               subj.append(c)
       for s in subj:
           chld.remove(s)
       for s in subj:               
           chld.insert(0,s)
       if first!=None:
           chld.remove(first)
           chld.insert(0,first)
            
    #Case specific to French, in which coordonated elements might share a different structure
    def gcoord_gn_gp(self,node):
        chld=node["children"]
        #chld[0] is the coordination
        #chld[1] is the model
        if "AND" in chld[0]["features"]:
            conj="AND"
        else:
            conj="OR"
        lastelement=None
        firstelement=None
        for c in chld:
            if c["pos"]=="coord":
                continue
            if conj in c["features"]:
                lastelement=c
            if c["pos"]=="gn":
                if "GP" not in c["features"]:
                    firstelement=c
                if conj in c["children"][0]["features"]:
                    lastelement=c
            
        if lastelement!=None:
            chld.remove(lastelement)
            chld.append(lastelement)
        if firstelement!=None:
            chld.remove(firstelement)
            chld.insert(1,firstelement)
        if len(chld)<=1 and chld[1]["pos"]!="gn":
            return
        #We first look for the non GP element
        if firstelement!=None and lastelement!=None:
            if "GP" in node["features"]:
                node["features"].remove("GP")
            if "DAT" in node["features"]:            
                node["features"].remove("DAT")
            case=self.getcase(chld[1])
            for c in chld[2:]:
                if c["pos"]=="gn" and "GP" in c["features"]:
                    c["features"].remove("GP")
                    self.cleancase(c)
                    c["features"].append(case)
                    #We skip the preposition
                    c["children"][0]["features"].append("SKIP")
                    if conj in c["children"][0]["features"]:
                        c["features"].append(conj)
        
    def getdependency(self,dep):
       ps=dep.find("_")
       psb=dep.find("(")
       if ps!=-1:
          if ps<psb:
             return dep[:ps]
       return dep[:psb]


    def buildwords(self,head,p,sentences):
        if head["translation"]!=None:
            ls=[head['lemma']]
            ls.extend(head["translation"])
            if head['translation'] !=[]:
                sentences.append(ls)
        for h in head["children"]:
            self.buildwords(h,p+1,sentences)
            

    def printtree(self,head,p):
        for i in range(0,p):
           print " ",
        print head["pos"],head["lemma"],head["features"],head['translation']
        for h in head["children"]:
            self.printtree(h,p+1)
            
    def spreadout(self,sentences,final,res):
        if sentences==[]:
            final.append(" ".join(res))
            return
        rs=res[:]
        multiple=False
        if len(sentences[0])>2:
            multiple=True
        for s in sentences[0][1:]:
            xs=s
            if multiple:
                xs+="*"
            rs.append(xs)
            self.spreadout(sentences[1:],final,rs)
            rs.pop()

    def compare(self,x,y):
        xd=self.getdependency(x)
        yd=self.getdependency(y)
        xi=20
        yi=20
        try:
            xi=self.depidx[xd]
        except:
            pass
        try:
            yi=self.depidx[yd]
        except:
            pass
        if xi<yi:
            return -1
        if xi==yi:
            return 0
        return 1
    
    def comparison(self,x,y):
        xd=x.name
        yd=y.name
        xi=20
        yi=20
        try:
            xi=self.depidx[xd]
        except:
            pass
        try:
            yi=self.depidx[yd]
        except:
            pass
        if xi<yi:
            return -1
        if xi==yi:
            return 0
        return 1
    
    def xipparsing(self,s):
        xs=objects.dependances()
        xips.parsestring(self.grm,s,xs)
        return xs
    
    def parse(self,sent,semanticwords):
       if debug: 
           print sent
       self.semanticsorter=semanticwords 
       self.roots={}
       xs=objects.dependances()
       self.maxidx=0
       s=unicode(sent,"utf-8").encode("latin-1")
       xips.parsestring(self.grm,s,xs)
       self.nodes={}
       xs.deps.sort(self.comparison)
       modifs={}
       for i in xs.deps:
           if debug:
               print i.name,i.features,i.p1,
               if i.p2!=None:
                   print i.p2,
                   if i.p3!=None:
                       print i.p3
                   else:
                       print 
               else:
                   print
                
           feat=i.features
           if "PROPQUE" in feat and i.name=="CODV":
               nm="CODVPROPQUE"
           elif i.p1[0]=="CONJQUE" and i.name=="CODV":
               nm="CODVCONJQUE"
           else:
               nm=i.name
           if i.p2==None:    
               r=[nm,0,i.p1]
           elif i.p3==None:
               r=[nm,0,i.p1,i.p2]
           else:
               r=[nm,0,i.p1,[i.p2,i.p3]]
        
           #The couples...
           couple=None
           if i.p2!=None:
               if i.p3!=None:
                   couple=[i.p2,i.p3]
               else: 
                   couple=[i.p1,i.p2]
               
           self.currentdep=nm
           #We might have a very weird case, such a pre-known expression
           if couple!=None:
               preknown1=couple[0][0]+"("+couple[0][1]+"),"+couple[1][0]+"("+couple[1][1]+")"
               
               pos=None
               trans=None
               #A preknown expression is a multiword expression which is translated by ONE word in lingveta
               #We store the key of these expression as sequence CAT(lemma),CAT(lemma)
               #When we find one, we keep tracks of its translation and its pos. We then get rid of the extra-word
               #It only works for two words, as this is the only information we have with a dependency
               #The dependency does not give any information about the order in the sentence of these words.           
               if preknown1 in self.frenchToLingve:
                   [pos,trans]=self.frenchToLingve[preknown1]
               else:#One expression at a time
                   preknown2=couple[1][0]+"("+couple[1][1]+"),"+couple[0][0]+"("+couple[0][1]+")" 
                   if preknown2 in self.frenchToLingve:
                       [pos,trans]=self.frenchToLingve[preknown2]                      
               if pos!=None:   
                   #modifs is indexed on our anchor word, which is the one sharing the same POS as in the expression POS
                   #Each expression has its own POS...
                   if pos==couple[0][0].lower():
                       modifs[couple[0][2]]=[trans]
                       modifs[couple[1][2]]=None
                   else:
                       modifs[couple[1][2]]=[trans]
                       modifs[couple[0][2]]=None
                              
           #Building the corresponding structure
           est="self.g_"+r[0]+"(r,feat)"
           try:
               eval(est)
           except:
               pass 
       if "gn" in self.nodes:    
           for j in self.nodes["gn"].values():
               self.gn_prep_p(j)
               self.gn_gv_verb(j)
               self.gn_gvinf(j)

       if debug:
           print "FIN"    
               
       if "gv" in self.nodes:
           for j in self.nodes["gv"].values():
                self.gn_gv_verb(j)
                self.gv_gn_prep(j)
       
       if "gcoord" in self.nodes:
           for j in self.nodes["gcoord"].values():
               self.gcoord_gn_gp(j)

       if "gn" in self.nodes:
           for j in self.nodes["gn"].values():
               self.gn_pl(j)

       if "p" in self.nodes:
           for j in self.nodes["p"].values():
               self.p_suj(j)
                          
       sentences=[]       
       for i in self.nodes:
           for j in self.nodes[i].values():
             if j["parent"]==None:
                 sent=[]
                 self.translate(j,modifs)
                 self.buildwords(j,0,sent)
                 if debug:
                     self.printtree(j,0)
                 sentences.extend(sent)
                 sentences.append([",",","])
       if debug: 
           print sentences
       if sentences==[]:
           return None
       #We remove the final ";"
       sentences[-1]=[".","."]
       return sentences

traduire=generation(cheminxip)
#print traduire.parse("Peut-être en saurons-nous plus lors du CES.",[])
#print traduire.parse("Le Skiffer Electronic Show sera placé sous le signe de la tablette tactile.",[])
#print traduire.parse("La dame mangeait le fruitéque.",[])
#print traduire.parse("Dès lundi, Skiff a donné les premiers détails sur ce qu'il dit être le lecteur de livres électroniques le plus fin au monde, d'une épaisseur de seulement 6 millimètres, pour une diagonale de 30 centimètres, très bien adaptée aux journaux et aux magazines.",[])
#print traduire.parse("La dame que je vois mange une glace.",[])
#print traduire.parse("La dame voit que je mange une glace.",[])
#print traduire.parse("Il sort pour acheter une chemise.",[])
#print traduire.parse("les chiens veulent manger un truc.",[])
#print traduire.parse("La dame qui regarde la télévision",[])
#print traduire.parse("Le monsieur mange une belle escalope de veau",[])
#print traduire.parse("Le chien que le monsieur voit mange un os",[])
#print traduire.parse("La dame avec une perruque est belle.",[])
#print traduire.parse("L'homme qui parle avec le chien regarde la télévision.",[])
#print traduire.parse("Son poids ne dépasse pas 450 grammes.",[])
#print traduire.parse("Pour parvenir à ce résultat, le Skiff Reader embarque une nouvelle génération de papier électronique.",[])
#print traduire.parse("Un lecteur est un ordinateur.",[])
#print traduire.parse("Un poids est lourd ou léger.",[])
#print traduire.parse("Le livre de la maison est rouge.",[])
#print traduire.parse("Pour parvenir à ce résultat",[])
#print traduire.parse("Les hommes mangent de la soupe, de la viande et des fruits.",[])
#print traduire.parse("Les premières traces remontent à la dernière fois.",[])
#print traduire.parse("Il y a eu un accident.",[])
print traduire.parse("Il boit du vin.",[])
#print traduire.parse("Il analyse la sortie informatique.",[])
#print traduire.parse("Il dit que l'analyse est simple.",[])
#print traduire.parse("La peur immortelle de la mort est une angoisse naturelle chez l'homme.",[])
#print traduire.parse("Un accord a aussi été noué avec l'opérateur Sprint pour permettre de télécharger des contenus en 3G directement sur le lecteur aux Etats-Unis.",[])
