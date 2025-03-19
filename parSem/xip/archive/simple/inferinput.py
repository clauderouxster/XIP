   def inferinput(self,gentext):
      nbmodif=0 
      ambiguous=[]
      [s,stgr,sgentext]=self.extractsentencefromgeneration(ambiguous,gentext)
      self.texttrans.delete(1.0,END)
      gr={}
      sentence=[]
      lexicals=[]
      #We parse our current sentence.
      try:
          sgr=grammar.parse(stgr,sentence,lexicals,self.dicoparta,self.roots,gr,False,None,grammarcodes.currentlanguage,None)
      except grammar.Eraiag,e:
         return [-1,None]
            
      if self.globalsentence==[]:
         self.globalsentence=lexicals
      ndx={}
      ix=0
      for l in lexicals:
            ndx[l["idx"]]=[ix,l]
            ix+=1
      #It is important to understand, that the graph is based on a RAW analysis of our model sentence
      #while the __selection__ corresponds to the ACTUAL sentence modified, if necessary by the linguist
      genres=None
      for g in self.generationgraphes:
        if g["__language__"]!=None and g["__language__"]!=grammarcodes.currentlanguage and g["__language__"]!="ALL":
            continue
        nbprecmodif=nbmodif
        if semantics.multiplematch(gr,g,True):
            #For each match, we increment one
            nbmodif+=1
            variablelist={}
            for i in g['VARIABLES']:
                for k in g['VARIABLES'][i]:
                    try:
                        variablelist[k][i]=g['VARIABLES'][i][k]
                    except:
                        variablelist[k]={i:g['VARIABLES'][i][k]}
            
            #In __selection__ we get the sentence which we keep as a model
            for lstvarias in variablelist.values(): 
                lstequ=g['ALIGN']
    
                lws=[]
                error=False
                #VARIABLES contains our variables associated to each element of the projecting graph
                removephrases={}
                listtopreplace={}
                listtoremove={}
                addtodelete=len(lstequ)
                #vk is a variable index
                for vk in lstvarias:
                    #This variable is either -1 (not instanciated) or the index of our node
                    matchingnode=int(lstvarias[vk])
                    #if kw==-1 it is an ontological element
                    if matchingnode==-1:
                      continue
                    #We have our VARINDEX      
                    indexvar=int(vk[1:])
                                        
                    #we test, if it has any equivalence?    
                    if indexvar in lstequ:
                        toreplace=lstequ[indexvar]
                        #We keep a list of the element to replace, in that order
                        #We remove the phrases, not just the current element
                        element=ndx[matchingnode]
                        positionelement=element[1]["p"]
                        if "up" in element[1]:
                            parent=element[1]["up"]                            
                            if parent["pos"] in ["NP","VP","AP"]:
                                positionelement=parent["node"][0]["p"]
                                for e in parent["node"]:
                                    removephrases[e["p"]]=True
                                    if toreplace==None:
                                        listtoremove[e["p"]]=True
                            else:
                                removephrases[positionelement]=True
                                if toreplace==None:
                                    listtoremove[positionelement]=True
                        else:
                            removephrases[positionelement]=True
                            if toreplace==None:
                                listtoremove[positionelement]=True
                                
                        if toreplace!=None:
                            listtopreplace[toreplace["i"]]=[positionelement,toreplace,element[1],False]

                    
                 
                #If we have negative value, we insert them into our structure
                #The negative values correspond to unmatched nodes in the right hand of the equivalence
                
                for l in lstequ:
                    if l<0:
                        #The position, will be given by the next word inserted
                        listtoreplace[l*-1]=[-1,lstequ[l],None,False]
                
                #We also check whether certain elements should be inverted
                l=0
                listwithnoindex=[]
                firstnozerovalue=None
                while l<len(listtopreplace)-1:
                    if l not in listtopreplace:
                        l+=1
                        continue
                    ec=listtopreplace[l][0]
                    ecnext=listtopreplace[l+1][0]
                    if firstnozerovalue==-1 and ec!=-1:
                         firstnozerovalue=l
                    
                    if ec!=-1 and ecnext!=-1:
                        #If the elements are inverted, then we invert their positions 
                        if ec>ecnext:
                            listtopreplace[l][0]=ecnext
                            listtopreplace[l+1][0]=ec
                        elif ec==ecnext:
                            listtopreplace[l+1][0]=ec+1
                            listtopreplace[l+1][3]=True
                    elif ec==-1:
                        #then its insertion is just before the next element
                        if ecnext!=-1:
                            listtopreplace[l][0]=ecnext-1
                        else:
                            if l>0:
                                if listtopreplace[l-1][0]!=-1:
                                    listtopreplace[l][0]=listtopreplace[l-1][0]+1
                                else:
                                    listwithnoindex.append(l)
                            else:
                                listwithnoindex.append(l)
                    l+=1
                #List not empty                    
                for l in listwithnoindex:
                    #It could have been set
                    if lstequ[l][0]!=-1:
                        continue
                    if l>firstnonzerovalue:
                        ec=listtopreplace[firstnonzerovalue]+1
                        #It is after                            
                        for i in range(firstnonzerovalue,l):
                            if listtopreplace[i][0]==-1:
                                listtopreplace[i][0]=ec
                                ec+=1
                            else:
                                ec=listtopreplace[i]+1
                    else:
                        #It is before
                        ec=listtopreplace[firstnonzerovalue]-1
                        for i in range(l,firstnonzerovalue):
                            if listtopreplace[i][0]==-1:
                                listtopreplace[i][0]=ec
                                ec-=1
                            else:
                                ec=listtopreplace[i]-1

                #Each element is a word selection
                genres=[]
                i=0
                for e in gentext:
                    #We remove the phrases
                    if i in removephrases:
                        if i in listtoremove:
                            genres.append([e[0],None])
                        i+=1
                        continue
                    genres.append(e[:])
                    i+=1
                
                    
                modif=False
                #lws contains the 
                casereplace=False
                nextinsertion=0
                i=0
                lastcase=None
                [ilx,toreplace,currentword,moved]=listtopreplace[0]
                #We store the case of the first element
                #Which has been removed. This element is the only one for which
                #we need to keep the case right. The other words will take their
                #case marking from the new word list in lstequ
                firstcase=None
                if currentword!=None:
                    if "case" in currentword:
                        firstcase=currentword["case"]
                        lastcase=firstcase
                thekeys=listtopreplace.keys()
                thekeys.sort()        
                for replacement in thekeys:
                    #The size of the matching is also an import criteria
                    #when selecting relative pronoun is at stake
                    nbmodif+=1
                    [ilx,toreplace,l,moved]=listtopreplace[replacement]
                    
                    if toreplace==None:
                        #This is a case when a word has been removed from the initial list
                        #we do not do anything, except modifying the cases
                        modif=True                
                        casereplace=False    
                        firstcase=None
                        continue
    
                    #If we add a preposition then the other element will be
                    #dative
                    if toreplace["pos"]=="Prep": #we must add it
                        modif=True                
                        casereplace=True    
                        firstcase=None
                        gfind=self.addprepcase(toreplace,lastcase)
                        genres.insert(ilx,[gfind,gfind])
                        continue
    
                    #We rebuild our word together with the ending from the target sentence
                    root=toreplace["root"]
                    gfind=toreplace['surf']
                    newcase=None
                    if casereplace:
                       #We have added a prep, then we must change the current case to dative
                       newcase="Dat"
                    else:                                    
                       if firstcase!=None:
                           newcase=firstcase
                       else:
                           if "case" in toreplace:
                               newcase=toreplace["case"] 
              
                    if "case" in toreplace and newcase!=toreplace["case"]:
                       oldcase=toreplace["case"]
                       toreplace["case"]=newcase
                       suff=self.getending(toreplace)
                       gfind=root+suff                           
                       toreplace["case"]=oldcase
    
                    skipdeterminer=False   
                    #If it is one of these elements, the firstcase must not be consummed
                    if toreplace["pos"] not in ["Adv","Det","Adj"]: 
                        firstcase=None
                    else:
                        #Otherwise, we do not want to keep any determiners around
                        skipdeterminer=True
                    
                    #For the other cases
                    #We take as new position the position of the leftest node
                    if l==None:
                        genres.insert(ilx,[gfind,gfind])
                    else:
                        parent=l["up"]
                        if root in self.ontologyheads:
                            #If we are dealing with an ontology head, we simply
                            #add the current word without any modification                
                            genres.insert(ilx,gentext[l["p"]])
                        else:
                            if toreplace["pos"]=="Verb":
                                newcase=None
                                surf=toreplace["surf"]
                                par=parent
                                while par!=None and par["pos"]!="S":
                                    par=par["up"]
                                if par!=None and par["rel"]!=toreplace["rel"]:
                                    rel=toreplace["rel"]
                                    toreplace["rel"]=par["rel"]
                                    suff=self.getending(toreplace)
                                    toreplace["rel"]=rel
                                    surf=root+suff
                                foreign=[gentext[l["p"]][0],surf]
                                genres.insert(ilx,foreign)
                            else:
                                foreign=gentext[l["p"]][0]
                                genres.insert(ilx,[foreign,gfind])
    
                        #We deactivate the casereplace...
                        casereplace=False
                        lastcase=newcase
                        insertx=ilx
                        #If the two matched elements belonged to the same node, then we insert at the beginning.
                        if moved:
                            insertx-=1
                        for e in parent["node"]:
                           if str(e["idx"]) in lstvarias.values():
                               continue
                           if skipdeterminer==True and e["pos"]=="Det":
                               continue
                           translations=gentext[e["p"]]
                           newtrans=[translations[0]]
                           for tw in translations[1:]:
                               if newcase!=None and "case" in e and e["case"]!=newcase:
                                   e["case"]=newcase
                                   e["suffix"]=self.getending(e)
                                   toadd=e["rootbasis"]+e["suffix"]
                               else:
                                    toadd=tw
                               newtrans.append(toadd)
                           genres.insert(insertx,newtrans)   
                           insertx+=1  
                    modif=True
                          
                if modif:              
                    saveambiguous=ambiguous
                    savegr=gr
                    savesentence=[]
                    savelexicals=[]
                    save_s=s
                    savestgr=stgr
                    savesgentext=sgentext
                    
                    ambiguous=[]            
                    [s,stgr,sgentext]=self.extractsentencefromgeneration(ambiguous,genres)  
                    gr={}
                    sentence=[]
                    lexicals=[]
                    #We reparse our current sentence.
                    try:
                        sgr=grammar.parse(stgr,sentence,lexicals,self.dicoparta,self.roots,gr,False,None,grammarcodes.currentlanguage,None)
                        gentext=genres
                    except grammar.Eraiag,e:
                        nbmodif=nbprecmodif
                        gr=savegr
                        sentence=savesentence
                        lexicals=savelexicals
                        s=save_s
                        stgr=savestgr
                        sgentext=savesgentext
                        ambiguous=saveambiguous
                            
      return [nbmodif,genres]
