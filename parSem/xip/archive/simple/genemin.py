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
import objectsmin

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

def addafeat(res,value):
    try:
        res["att"].append(value)
    except:
        res["att"]=[value]
    

cheminbase=sys.path[0]
seps=grammarcodes.seps
cheminxip=cheminbase+seps+"francais"+seps+"minimum"+seps+"francais.grm"
   
class generation:

    def __init__(self,chemin):
       self.grm=xips.loadgrm(chemin)
       xips.setdisplay(self.grm,xips.XIP_DISPLAY_NONE)
       self.frenchToLingve={}
       self.englishToLingve={}
       self.nodes={}
       self.roots={}
       self.endings={"conj":"nu","noun":"ta","det":"la","adj":"ca","adv":"no","conj":"nu","prep":"ar","pron":"er","verb":"iag","conjque":"er","num":"lu"}
       self.pos={"lu":"det","ti":"noun","ci":"adj","ta":"noun","co":"adj","te":"noun","to":"noun","fa":"noun","ma":"noun","la":"det","ca":"adj","no":"adv","nu":"conj","ar":"prep","er":"pron","ag":"verb"}
       self.featuretransfers={"p":["gv","gn"],"gv":["verb","aux"],"gn":["pron","conjque","det","noun","adj","num","ga"],"det":["num"],"gcoord":True,"adv":["adverb"],"ga":["adj"],"verb":["verb"]}    
    
    def buildidx(self,dicoenglish,dicofrench):
        racloc=None
        #i is a word in lingveta
        for i in lingata.lexicons.keys():
             s=lingata.lexicons[i]
             infoword=ending.ending(i)
             pos=infoword["pos"].lower()
             if pos not in self.frenchToLingve:
                 self.frenchToLingve[pos]={}
             if "francais" in s:
                 lct=s["francais"]
                 psl=0
                 for l in lct:
                     #If our expression is made of POS and lemmas
                     #Then we use this expression as a key for which we keep both
                     #the translation and the POS of the expression
                     if "(" in l:
                         if l not in self.frenchToLingve:
                             self.frenchToLingve[l]=[pos.lower(),i]
                     else:
                         if not debug:
                             if i not in dicoenglish:
                                 continue
                         try:
                             if i=="beiag":
                                 racloc=l
                             #If we have features attached to that entry, we remove it
                             ps=l.find("@")
                             # l ---> is a French word
                             # i ---> is the translation in lingvta
                             if ps!=-1:
                                l=l[:ps]
                             self.frenchToLingve[pos][l].append(str(psl)+"_"+i)
                         except:
                             self.frenchToLingve[pos][l]=[str(psl)+"_"+i]
                     psl+=1
             if pos not in self.englishToLingve:
                 self.englishToLingve[pos]={}
             if "english" in s:
                 lct=s["english"]
                 psl=0
                 for l in lct:
                     #If our expression is made of POS and lemmas
                     #Then we use this expression as a key for which we keep both
                     #the translation and the POS of the expression
                     if "(" in l:
                         if l not in self.englishToLingve:
                             self.englishToLingve[l]=[pos.lower(),i]
                     else:
                         if not debug:
                             if i not in dicofrench:
                                 continue
                         try:
                             #If we have features attached to that entry, we remove it
                             ps=l.find("@")
                             # l ---> is a English word
                             # i ---> is the translation in lingvta
                             if ps!=-1:
                                l=l[:ps]
                             self.englishToLingve[pos][l].append(str(psl)+"_"+i)
                         except:
                             self.englishToLingve[pos][l]=[str(psl)+"_"+i]
                     psl+=1
        if racloc!=None:
            #We push beiag ahead
            self.frenchToLingve["verb"][racloc].sort()
    
    def generateOntologicalVariations(self,wrd):
        #w is a lingveta word
        lstlingveta=[]
        if grammarcodes.currentlanguage=="english":
            #we rebuild our word
            word=wrd["rootbasis"]+wrd["basis"]
            lstlingveta=[wrd["rootbasis"]]
            pos=wrd["pos"].lower()
            lst=[]
            try:
                englishwords=lingata.lexicons[word]["english"]
                for e in englishwords:
                    if "(" in e:
                        continue
                    ligv=self.englishToLingve[pos][e]
                    for l in ligv:
                        ps=l.find("_")
                        wn=ending.ending(l[ps+1:])
                        if wn["root"] not in lstlingveta:
                            lstlingveta.append(wn["root"])
            except:
                pass
        return lstlingveta
              
        
        
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
            
    def skip(self,w):
        x=self.getnode(w)
        x["features"].append("SKIP")
        
    def removenode(self,pos,x):
        try:
           hx=self.getparent(pos,x)
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

    def returnnode(self,i):
        pos=i[0].lower()
        if pos not in self.nodes:
            return None
        if i[1][2] not in self.nodes[pos]:
            return None
        return self.nodes[pos][i[1][2]]
        
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
    
    def getparent(self,pos,nidx):
        pos=pos.lower()
        self.roots[pos]=True
        if pos==nidx[0].lower():
            return self.getnode(nidx)
        return self.getnode([pos,nidx[1],nidx[2]])

    def topparent(self,n):
        node=self.getnode(n)
        np=node["parent"]
        if np==None:
            return node
        while np!=None and np["parent"]!=None:
            np=np["parent"]
        return np
    
    def parentphrase(self,root,withchild):
        xm=self.getparent(root[0],root[1])
        if xm['children']==[] and withchild:
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
        head=self.parentphrase(root,False)
        
        for i in nodes:            
            xm=self.parentphrase(i,True)
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
                      if w['pos'] in ["conjque","pron"]:
                        xs=xs[:-2]+"i"+ics+xs[-2:]  
                      else:
                        xs=xs[:-2]+"i"+ics+xs[-2:]+"s"
                        cs=""
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
                  if basewrd=="son" and "PL" in w["features"]:
                      w["translation"]=[]
                      for itr in tr:
                          sitr=itr[:-2]+"mula"
                          w["translation"].append(sitr)
                  else:
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
                try:
                    lem=self.frenchToLingve[w["pos"]][basewrd]
                    w['translation'].extend(lem)
                except:
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
        
    
    def addmuala(self,w,rajouts):
       self.maxid+=1
       wmuala={"surface":"muala","lemma":"muala","features":["KEEP"],"children":[],"parent":w["parent"],"pos":"det",'translation':[],"id":self.maxid}
       x=w["translation"][0]
       if x[-1] in ["n","d","s"]:
           wmuala["translation"]=["muala"+x[-1]]
       else:     
           wmuala["translation"]=["muala"]
       if "GP" in w["parent"]["features"]:
           p=w["parent"]["children"].index(w)
           rajouts.append([w["parent"],wmuala,p])
       else:
           rajouts.append([w["parent"],wmuala,0]) 
        
    def translate(self,w,modifs,rajouts):
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
           #Cas particulier: PL, on rajoute un determinant
           if "PL" in w["features"]:
               if w["pos"]=="noun" and "DET" not in w["features"]:
                   self.addmuala(w,rajouts)
               elif w["pos"]=="det" and w["lemma"] not in ["un","le","son"]:
                    self.addmuala(w,rajouts)
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
                self.translate(h,modifs,rajouts)


    
    def getantcase(self,w):
        if "NOM" in w['features']:
            return "ANT_NOM"
        if "ACC" in w['features']:
            return "ANT_ACC"
        if "DAT" in w['features']:
            return "ANT_DAT"
        if "GEN" in w['features']:
            return "ANT_GEN"
        return "ANT_NOM"
 
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
 
    def testcoord(self,w):
        if "OR" in w["features"]:
            return "OR"
        if "AND" in w["features"]:
            return "AND"
        return None
    
    def clean(self,w,lst):
        for l in lst:
            try:
                w["features"].remove(l)
            except:
                pass
            
    def cleancase(self,w):
        if "NOM" in w['features']:
            w['features'].remove("NOM")
        if "ACC" in w['features']:
            w['features'].remove("ACC")
        if "DAT" in w['features']:
            w['features'].remove("DAT")
        if "GEN" in w['features']:
            w['features'].remove("GEN")
        


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


########################################################################################################
########################################################################################################
########################################################################################################
####   Dependency Analysis
    def g_DET_GN(self,r,feat):
        [dep,phidx,det,noun]=r
        hn=self.getnode(noun)
        if hn["parent"]==None:
            self.buildnode(["GN",noun,feat],[["noun",noun,None]],None)
        self.buildnode(["GN",noun,feat],[["det",det,None]],-1)
        hn["features"].append("DET")
        return False
        
    def g_DET_GA(self,r,feat):
        [dep,phidx,det,adj]=r
        self.buildnode(["GN",adj,feat],[["adj",adj,["DET"]]],None)
        self.buildnode(["GN",adj,feat],[["det",det,None]],-1)
        return False
        
    def g_COORD_GP(self,r,feat):
        [dep,phidx,coord,noun]=r
        hnoun=self.getnode(noun)
        if coord[1]=="et":
            hnoun["features"].append("AND")
        else:
            hnoun["features"].append("OR")
        return True
    
    def g_GN_NV_KER(self,r,feat):
        return True
    
    def g_NUM_GN(self,r,feat):
        [dep,phidx,num,noun]=r
        hnoun=self.getnode(noun)
        hnoun["features"].append("DET")
        if hnoun["parent"]==None:
            self.buildnode(["GN",noun,feat],[["noun",noun,None]],None)
        self.buildnode(["GN",noun,feat],[["num",num,None]],-1)
        return False
        

    def g_GP_GP(self,r,feat):
        [dep,phidx,nleft,nright]=r
        hleft=self.getparent("GN",nleft)
        hright=self.getparent("GN",nright)
        self.child(hleft,hright,None)
        return True

    def g_GN_GP(self,r,feat):
        [dep,phidx,noun,prep]=r
        if self.verbfound==False:
            hleft=self.getparent("GN",noun)
            hright=self.getparent("GN",prep)
            hright["features"].append("NOUNCOMP")
            #Cas particulier: de...
            self.child(hleft,hright,None)        
        return True
    
    def g_PREP_GP(self,r,feat):
        [dep,phidx,prep,noun]=r
        hnoun=self.getnode(noun)
        if hnoun["parent"]==None:
            self.buildnode(["GN",noun,None],[["noun",noun,None]],None)
            nnoun=self.getnode(noun)
            cord=self.testcoord(nnoun)
            if cord!=None:
                self.clean(nnoun,["AND","OR"])
                nprep=self.getnode(prep)
                nprep["features"].append(cord)
        
        nprep=self.getnode(prep)
        if "NOUNCOMP" in hnoun["parent"]["features"] and nprep["lemma"]=="de":
            self.cleancase(hnoun["parent"])
            hnoun["parent"]["features"].append("GEN")            
            nprep["features"].append("SKIP")            
            return False

        fsadd=["GP","DAT"]        
        if feat==None:
            feat=fsadd
        else:
            feat.extend(fsadd)
        self.buildnode(["GN",noun,feat],[["prep",prep,None]],0)
        return False

    def g_PREP_PV(self,r,feat):
        [dep,phidx,prep,verb]=r
        hpar=self.getparent("GV",verb)
        fsadd=["INFINITIVE_PREP"]
        if feat==None:
            feat=fsadd
        else:
            feat.extend(fsadd)
        self.buildnode(["GV",verb,feat],[["prep",prep,None]],0)
        return False
    
    def g_GN_NV(self,r,feat):
        [dep,phidx,noun,verb]=r
        hn=self.getnode(noun)
        hv=self.getnode(verb)
        if hn["lemma"]=="ce" and hv["lemma"]=="être":
            hn["features"].append("SKIP")
            return
        if noun[1] in ["y","en","lui","le"]:
            #Particular case: il y a
            if noun[1]=="y" and verb[1]=="avoir":
                #We need to have "il" on the top node
                hpar=self.topparent(verb)
                if hpar["pos"]=="p" and hpar["lemma"]=="il":
                    hverb=self.getnode(verb)
                    hverb["translation"]=["gebeiag"]
                    hverb["features"].append("KEEP")            
                    for c in hpar["children"]:
                        if c["lemma"]=="il":
                            c["children"][0]["features"]="SKIP"
                            break
                    ny=self.getnode(noun)
                    ny["features"].append("SKIP")    
                return False                
            self.buildnode(["GV",verb,feat],[["pron",noun,None]],0)
        else:
            hgv=self.getparent("GV",verb)
            if hgv["parent"]!=None:
                return False
            hpar=self.getparent("P",noun)
            if hpar["children"]==[]:
                self.buildnode(["P",noun,feat],[["GN",noun,None]],0)
            self.buildnode(["P",noun,feat],[["GV",verb,None]],1)
        return False

    def g_CONJQUE_NV(self,r,feat):
        return True

    def g_GN_CONJQUE(self,r,feat):
        return True
        
    def g_NV_GP(self,r,feat):
        [dep,phidx,verb,noun]=r
        hgv=self.getparent("GV",verb)
        if hgv["children"]==[]:
            self.buildnode(["GV",verb,feat],[["verb",verb,None]],None)
        self.buildnode(["GV",verb,feat],[["GN",noun,None]],-1)
        return False

    def g_NV_GN(self,r,feat):        
        [dep,phidx,verb,noun]=r
        self.buildnode(["GV",verb,feat],[["GN",noun,["ACC"]]],-1)
        self.buildnode(["GV",verb,feat],[["verb",verb,None]],None)
        return False

    def g_NV_GA(self,r,feat):
        [dep,phidx,verb,adj]=r
        self.buildnode(["GV",verb,feat],[["GA",adj,["ACC"]]],-1)
        self.buildnode(["GV",verb,feat],[["verb",verb,None]],None)
        return False
            
    def g_GN_PV(self,r,feat):
        [dep,phidx,noun,verb]=r
        #We only consider the farest ones
        hpar=self.getparent("GV",verb)
        if hpar["parent"]==None:
            self.buildnode(["GN",noun,feat],[["GV",verb,None]],None)
        return False

    def g_GP_GN(self,r,feat):
        [dep,phidx,nleft,nright]=r
        #It is next to it. We merge them
        hright=self.getnode(nright)
        if hright["parent"]==None:
            hleft=self.getnode(nleft)
            if hleft["parent"]!=None:
                hleftpar=hleft["parent"]
                if hleftpar["parent"]!=None:
                    ps=hleftpar["parent"]["children"].index(hleftpar)
                    hnright=self.buildnode(["GN",nright,feat],[["noun",nright,None]],None)
                    self.child(hleftpar["parent"],hnright,ps+1)            
            
        hright["parent"]["features"]=["GEN"]
        return False
                                          
    def g_GR_GP(self,r,feat):
        [dep,phidx,adverb,noun]=r
        self.buildnode(["GN",noun,feat],[["adv",adverb,None]],-1)
        return False

    def g_NEG_NV(self,r,feat):
        [dep,phidx,neg,verb]=r
        nverb=self.getnode(verb)
        nverb["features"].append("NEG")
        self.skip(neg)
        
    def g_PV_GN(self,r,feat):
        [dep,phidx,verb,noun]=r
        hpar=self.getparent("GN",noun)
        if hpar["parent"]==None:
            hpar=self.buildnode(["GV",verb,feat],[["GN",noun,["ACC"]]],-1)
            #We modify GV to make into a clause
            hpar["children"][0]["features"].append("TO_PRON")
            hpar["features"].remove("INFINITIVE_PREP")
            hpar["features"].append("REL")
        return False

    def g_NV_NV(self,r,feat):
        [dep,phidx,aux,verb]=r
        haux=self.getparent("GV",aux)
        if aux[1]=="avoir":
            #Past
            
            naux=self.getnode(aux)
            self.clean(naux,["KEEP"])
            gebei=naux["translation"]   
            haux["features"].append("SKIP")
            advs=[]
            for a in haux["children"]:
                if a["pos"]=="adv":
                    advs.append(a)
            if gebei!=[]:
                nverb=self.getnode(verb)
                nverb["translation"].extend(gebei)
                nverb["features"].append("KEEP")
            
            hverb=self.getparent("GV",verb)
            if hverb["parent"]==None:
                hpar=self.buildnode(["GV",verb,feat],[["verb",verb,None]],None)
            hverb["features"].append("PAST")
            for a in advs:
                haux["children"].remove(a)
                a["parent"]=None
                self.child(hverb,a,-1)
            self.child(haux,hverb,-1)
        else:
            hverb=self.getparent("GV",verb)
            hverb["features"].append("INFINITIVE")
            if hverb["parent"]==None:
                self.child(haux,hverb,-1)
        return False
    
    def g_GA_GN(self,r,feat):
        [dep,phidx,adj,noun]=r
        self.buildnode(["GN",noun,feat],[["adj",adj,None]],-1)
        return False

    def g_GR_GA(self,r,feat):
        [dep,phidx,adv,adj]=r
        hadj=self.getnode(adj)
        hpar=hadj["parent"]["pos"]
        self.buildnode([hpar,adj,feat],[["adv",adv,None]],-1)
        return False

    def g_GR_NV(self,r,feat):
        return True        
        
    def g_GN_GA(self,r,feat):
        [dep,phidx,noun,adj]=r
        self.buildnode(["GN",noun,feat],[["GA",adj,None]],-1)
        hga=self.getparent("GA",adj)
        self.cleancase(hga)
        return False

    def g_GP_GA(self,r,feat):
        [dep,phidx,noun,adj]=r
        self.buildnode(["GN",noun,feat],[["GA",adj,None]],-1)
        hga=self.getparent("GA",adj)
        self.cleancase(hga)
        return False
        
    def g_PV_GP(self,r,feat):
        [dep,phidx,verb,noun]=r
        hpar=self.getparent("GN",noun)
        if hpar["parent"]==None:
            hpar=self.buildnode(["GV",verb,feat],[["GN",noun,None]],-1)
            #We modify GV to make into a clause
            hpar["children"][0]["features"].append("TO_PRON")
            hpar["features"].remove("INFINITIVE_PREP")
            hpar["features"].append("REL")
        return False
    
    def g_NV_GR(self,r,feat):
        [dep,phidx,verb,adverb]=r
        nverb=self.getnode(verb)
        if adverb[1]=="pas" and "NEG" in nverb["features"]:
            self.skip(adverb)
            return False
        
        self.buildnode(["GV",verb,feat],[["adv",adverb,None]],-1)
        return False
        
    def g_PV_PV(self,r,feat):
        return True

    def g_GN_GN(self,r,feat):
        return True
    
    def g_NOUN_GN(self,r,feat):
        [dep,phidx,noun,head]=r
        noun[0]="adj"
        self.buildnode(["GN",head,feat],[["GA",noun,None]],-1)
        return False
########################################################################################################
########################################################################################################
###  Post treatment        

    def post_GR_NV(self,r,feat):        
        [dep,phidx,adv,verb]=r
        if adv[1]=="pas":
            self.skip(adv)
            return False
        hverb=self.getnode(verb)
        hpar=hverb["parent"]["pos"]
        self.buildnode([hpar,verb,feat],[["adv",adv,None]],-1)

    def post_COORD_GP(self,r,feat):
        [dep,phidx,coord,noun]=r
        hnoun=self.getparent("GN",noun)
        if "GP" in hnoun["features"]:
            nnoun=self.getnode(noun)
            cord=self.testcoord(nnoun)
            if cord!=None:
                self.clean(nnoun,[cord])
                hnoun["children"][0]["features"].append(cord)
            c=self.getnode(coord)
            c["features"].append("SKIP")
            
    
    def post_GN_GN(self,r,feat):
        [dep,phidx,noun,conj]=r
        if conj[0]=="PRON":
            htopconj=self.topparent(conj)
            if htopconj!=None:
                hnoun=self.buildnode(["P",noun,feat],[[htopconj["pos"],conj,["REL"]]],-1)
                ant_cs=self.getantcase(hnoun)
                hconj=self.getparent("GN",conj)
                hconj["features"].append(ant_cs)
        
    def post_GN_CONJQUE(self,r,feat):
        [dep,phidx,noun,conj]=r
        hnoun=self.getparent("GN",noun)
        hconj=self.getparent("GN",conj)
        self.child(hnoun,hconj,None)
        ant_cs=self.getantcase(hnoun)
        hconj=self.getparent("GN",conj)
        hconj["features"].append(ant_cs)
        
    def post_CONJQUE_NV(self,r,feat):
        [dep,phidx,conj,verb]=r
        htopverb=self.topparent(verb)
        nconj=self.getnode(conj)
        hconj=self.getparent("GN",conj)
        self.child(hconj,nconj,0)
        self.child(hconj,htopverb,None)
        if conj[1]=="que":
            hconj["features"].append("ACC")
            #We then remove, if there is an NP "ACC", the ACC from it
            hverb=self.getparent("GV",verb)
            vlist=[]
            for c in hverb["children"]:
                if c["pos"]=="verb":
                    vlist.append(c)
                if c["pos"]=="gn" and "ACC" in c["features"]:
                    self.cleancase(c)
                    c["features"].append("NOM")
        elif conj[1]=="dont":
            hconj["features"].append("GEN")
        
    def post_PV_PV(self,r,feat):
        [dep,phidx,vleft,vright]=r
        #We merge the two verbs. The second one become the winner
        hleft=self.getparent("GV",vleft)
        hright=self.getparent("GV",vright)
        #We delete hright
        self.removenode("GV",vright)
        self.buildnode(["GV",vleft,feat],[["verb",vright,["INFINITIVE"]]],-1)

    def post_GN_GP(self,r,feat):
        [dep,phidx,noun,prep]=r
        hprep=self.getparent("GN",prep)
        if "NOUNCOMP" in hprep["features"]:
            return 
        hnoun=self.getparent("GN",noun)
        case=self.getantcase(hnoun)
        hprep["children"][0]["features"].append(case)
        hprep["children"][0]["features"].append("GEN")

    def post_GP_GP(self,r,feat):
        [dep,phidx,noun,prep]=r
        hprep=self.getparent("GN",prep)
        if "NOUNCOMP" in hprep["features"]:
            return 
        hnoun=self.getparent("GN",noun)
        case=self.getantcase(hnoun)
        hprep["children"][0]["features"].append(case)
        hprep["children"][0]["features"].append("GEN")
        

    def post_GN_NV_KER(self,r,feat):
        [dep,phidx,noun,verb]=r        
        #we transform the GV into a P        
        #We create our new element
        hgv=self.getparent("GV",verb)
        if hgv["children"][0]["pos"]=="conjque":
            return 
        self.maxid+=1
        ker=["conjque","ker",self.maxid,"ker"]        
        self.buildnode(["GV",verb,feat],[["conjque",ker,None]],0)
        hgv["features"].append("REL")
        hgn=self.getparent("P",noun)
        self.child(hgn,hgv,1)
        
########################################################################################################
########################################################################################################
        
    def xipparsing(self,s):
        xs=objectsmin.dependances()
        xips.parsestring(self.grm,s,xs)
        return xs
    
    def parse(self,sent,semanticwords):
       if debug: 
           print sent
       self.semanticsorter=semanticwords 
       self.roots={}
       xs=objectsmin.dependances()
       s=unicode(sent,"utf-8").encode("latin-1")
       xips.parsestring(self.grm,s,xs)
       self.nodes={}
       modifs={}
       kxs=xs.deps.keys()
       kxs.sort()
       postpone=[]
       self.maxid=xs.maxid
       if debug:
           print "MAX:",self.maxid
           for xdp in kxs:
               for elx in xs.deps[xdp].keys():
                   i=xs.deps[xdp][elx]
                   print i.name,i.features,i.p1,
                   if i.p2!=None:
                       print i.p2,
                       if i.p3!=None:
                           print i.p3
                       else:
                           print 
                   else:
                       print
           print "-------------------------------------"
           print 
       self.verbfound=False 
       for xdp in kxs:
           kxsdeps=xs.deps[xdp].keys()
           kxsdeps.sort()
           for elx in kxsdeps:
               i=xs.deps[xdp][elx]
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
               if r[0][:3]=="NV_":
                   self.verbfound=True
               try:
                   if eval(est):
                       postpone.append([r,feat,"self.post_"+r[0]+"(r,feat)"])
                   if debug: 
                       [dep,phidx,x1,x2]=r
                       h=self.topparent(x1)
                       self.printtree(h,0)
                       print "====================================="
               except:
                   pass 
                          
       for e in postpone:
           try:
               r=e[0]
               feat=e[1]
               eval(e[2])
               if debug: 
                   [dep,phidx,x1,x2]=r
                   print "POST:",dep,x1,x2
                   h=self.topparent(x1)
                   self.printtree(h,0)
                   print "====================================="
           except:
               pass
            
       sentences=[]
              
       for i in self.nodes:
           for j in self.nodes[i].values():
             if j["parent"]==None:
                 sent=[]
                 rajouts=[]
                 self.translate(j,modifs,rajouts)
                 for r in rajouts:
                     r[0]["children"].insert(r[2],r[1])
                 self.buildwords(j,0,sent)
                 if debug:
                     self.printtree(j,0)
                 if sent!=[]:
                     sentences.extend(sent)
                     sentences.append([",",","])
       nb=1          
       for i in sentences[0][1:]:
            sentences[0][nb]=i[0].upper()+i[1:]
            nb+=1
       sent=[]
       for i in sentences:
           k=[]
           for s in i[1:]:
               ps=s.find("_")
               k.append(s[ps+1:])
           k.sort()
           k.insert(0,i[0])
           sent.append(k)
       sentences=sent 
       if debug: 
           print sentences
           print
           print "---------------------------------------------------------------------------------------------------"
           print
       if sentences==[]:
           return None
       #We remove the final ";"
       sentences[-1]=[".","."]
       return sentences

traduire=generation(cheminxip)
#traduire.buildidx([],[])
#traduire.parse("Peut-être en saurons-nous plus lors du CES.",[])
#traduire.parse("Le Skiffer Electronic Show  sera placé sous le signe de la tablette tactile.",[])
#traduire.parse("Le Show allemand sera placé sous le signe de la tablette tactile.",[])
#traduire.parse("La dame mangeait le fruitéque.",[])
#traduire.parse("Dès lundi, Skiff a donné les premiers détails sur ce qu'il dit être le lecteur de livres électroniques le plus fin au monde, d'une épaisseur de seulement 6 millimètres, pour une diagonale de 30 centimètres, très bien adaptée aux journaux et aux magazines.",[])
#traduire.parse("La dame que je vois mange une glace.",[])
#traduire.parse("La dame voit que je mange une glace.",[])
#traduire.parse("Il sort pour acheter une chemise.",[])
#traduire.parse("les chiens veulent manger un truc.",[])
#traduire.parse("La dame qui regarde la télévision",[])
#traduire.parse("Le monsieur mange une belle escalope de veau",[])
#traduire.parse("Le chien que le monsieur voit mange un os",[])
#traduire.parse("Le chien que voit le monsieur mange un os",[])
#traduire.parse("La dame avec une perruque est belle.",[])
#traduire.parse("La dame est avec une perruque.",[])
#traduire.parse("L'homme qui parle avec le chien regarde la télévision.",[])
#traduire.parse("Son poids ne peut pas dépasser 450 grammes.",[])
#traduire.parse("Pour parvenir à ce résultat, le Skiff Reader embarque une nouvelle génération de papier électronique.",[])
#traduire.parse("Un lecteur est un ordinateur.",[])
#traduire.parse("Un poids est lourd ou léger.",[])
#traduire.parse("Le livre de la maison est rouge.",[])
#traduire.parse("Pour parvenir à ce résultat",[])
#traduire.parse("Les hommes mangent de la soupe de la viande et des fruits.",[])
#traduire.parse("L'homme mangent de la viande et des fruits.",[])
#traduire.parse("Les premières traces remontent à la dernière fois.",[])
#traduire.parse("Il y a été un accident.",[])
#traduire.parse("Il y a eu un accident.",[])
#traduire.parse("Il parle de la dame au chien.",[])
#traduire.parse("Il boit du vin.",[])
#traduire.parse("Il analyse la sortie informatique.",[])
#traduire.parse("Il dit que l'analyse est simple.",[])
#traduire.parse("La peur immortelle de la mort est une angoisse naturelle chez l'homme.",[])
#traduire.parse("Un accord a aussi été noué avec l'opérateur Sprint pour permettre de télécharger des contenus en 3G directement sur le lecteur aux Etats-Unis.",[])
#traduire.parse("Ce sont des promesses de marges juteuses.",[])
#traduire.parse("Le gouvernement indonésien prévoit de durcir ses règles de contrôle des sites Internet.",[])
#traduire.parse("Le gouvernement de l'Indonésie prévoit de durcir ses règles de contrôle des sites d'Internet.",[])
