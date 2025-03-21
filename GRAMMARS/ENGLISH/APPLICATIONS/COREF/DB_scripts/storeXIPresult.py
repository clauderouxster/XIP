Python:

saveinmysql(#1,int idsent,string txt,int source).

global inserttheconcepts
global insertsentences
global insertconcepts
global insertiondep
global insertwordoffset
global idsent
global theconcepts
global sentencesforconcept
global cursor

thesource=0

import MySQLdb

LIT_HOST = "etretat"
LIT_DB = "litigation"
LIT_USER = "root"
LIT_PWD ="root"
LIT_PORT = 3306

stopwords=["the","a","in","of","from","by","to","this","that","those","these"]

#We connect to our database dbsase, on the host h with the username u
def connection(h,p,u,pwd,dbase):
    mydb = MySQLdb.connect(db=dbase,host=h,user=u,port=p,passwd=pwd)
    return mydb

def closedatabase(mydb):
    mydb.close()


mydb=connection(LIT_HOST,LIT_PORT,LIT_USER,LIT_PWD,LIT_DB)
cursor=mydb.cursor()

def POS(pos):
    if (pos=="NAV"):
        return 1    
    if (pos=="NOUN"):
        return 1
    if (pos=="VERB"):
        return 1
    if (pos=="ADJ"):
        return 1
    if (pos=="DEP"):
        return 4
    if (pos=="FEAT"):
        return 5
    if (pos=="MARKUP"):
          return 6
    if (pos=="ATTRIBUTE"):
          return 7
    if (pos=="PEID"):
          return 8
    if (pos=="PENORM"):
          return 9
    return 1

def rank(val):
   return val >> 4

def bitpos(val,pos):
   return 1 << (val - (pos << 4))

def addset(liste,val):
    r=rank(val)
    v=bitpos(val,r)
    try:
        liste[r]|=v
        return 0
    except:
        liste[r]=v
        return 2
    
def addindexedset(index,liste,val):
    r=rank(val)
    v=bitpos(val,r)
    key = (index,r)
    try:
        liste[key]|=v
        return 0
    except:
        liste[key]=v
        return 2

    
def addidconcept(cursor,wordpipe,pos):
    global theconcepts
    #different reading are separated with a '|'
    wl=wordpipe.split('|')
    #we keep the first one
    word=wl[0].lower()
    wkey=(word,pos)
    #print wkey
    if wkey in theconcepts:
        return theconcepts[wkey]

    #the new id is the number of concepts already stored
    id=len(theconcepts)
    #we keep a track of this new concept in a table for a faster access
    theconcepts[wkey]=id
    #print "CONCEPTS=",len(theconcepts)
    return id
     

def addsentenceconcepts(ss):
    global insertsentences
    if insertsentences==[]:
        sc='insert into sentencetoconcepts values'+ss
        insertsentences.append(sc)
    else:
        insertsentences.append(ss)
        
    
def saveSentenceConcepts(cursor,conceptsinsentence,idsent):
    global insertsentences
    
    for r in conceptsinsentence:
        ss='('+str(idsent)+','+str(r)+','+str(conceptsinsentence[r])+')'
        addsentenceconcepts(ss)
        if len(insertsentences)>=5000:
            ss=",".join(insertsentences)
            ss=ss+";"
            cursor.execute(ss)
            del insertsentences[0:len(insertsentences)]


def saveSentence(cursor,idsent,txt):
    global thesource
    ss='insert into sentences values('+str(idsent)+','+str(thesource)+',0,"'+txt+'");'
    cursor.execute(ss)

def addinsertwordoffset(ss):
    global insertwordoffset
    if insertwordoffset==[]:
        sc='insert into wordoffset values'+ss
        insertwordoffset.append(sc)
    else:
        insertwordoffset.append(ss)


def saveWordNodes(cursor,conceptsinsentence,idsent):
    global insertwordoffset
    global sentencesforconcept
    global comptesentencesforconcept
    
    #nodelist=xips.lexicals() modification Vianney Grassaud
    nodelist=lexicals()

    for idn in nodelist:
        n=XipNode(idn)
        if (n.pos!="PUNCT"):
            id=addidconcept(cursor,n.lemma,POS(n.pos))
            addset(conceptsinsentence,id)
            addindexedset(id, sentencesforconcept, idsent)
            ss='('+str(id)+','+str(idsent)+','+str(n.leftoffset)+','+str(n.rightoffset)+')'
            addinsertwordoffset(ss)

            if len(insertwordoffset)>5000:
                ss=",".join(insertwordoffset)
                ss=ss+";"
                cursor.execute(ss)
                del insertwordoffset[0:len(insertwordoffset)]
   # print "TAILLE=",comptesentencesforconcept


def saveDependencyNodes(cursor,idnode,listeval,conceptsinsentence,idsent,first):
    global sentencesforconcept
    n=XipNode(idnode)
    #print n.pos, ": ",n.lemma
    
    if (n.next!=-1 and first==False):
        saveDependencyNodes(cursor,n.next,listeval,conceptsinsentence,idsent,False)
    if (n.daughter!=-1):
        if (n.pos=="PUNCT"):
            return
        saveDependencyNodes(cursor,n.daughter,listeval,conceptsinsentence,idsent,False)
    else:
        if (n.pos=="PUNCT"):
            return        
        w=n.lemma
        id=addidconcept(cursor,w,POS(n.pos))
        addset(listeval,id)
        addset(conceptsinsentence,id)
        addindexedset(id, sentencesforconcept, idsent)
  
def getDependencyNodes(cursor,idnode,listeval,first):
    n=XipNode(idnode)
    
    if (n.next!=-1 and first==False):
        getDependencyNodes(cursor,n.next,listeval,False)
    
    if (n.pos=="PUNCT"):
       return

    if (n.daughter!=-1):
        getDependencyNodes(cursor,n.daughter,listeval,False)
    else:
        w=n.lemma
        id=addidconcept(cursor,w,POS(n.pos))
        addset(listeval,id)

def adddependency(ss):
    global insertiondep
    if insertiondep==[]:
        sc='insert into dependances values'+ss
        insertiondep.append(sc)
    else:
        insertiondep.append(ss)
    
def saveDependencyOffsets(cursor,iddep,idsent,left,right):
    if left <0:
       return
    ss='insert into dependencyoffset values('+str(idsent)+','+str(iddep)+','+str(left)+','+str(right)+');'
    try:
         cursor.execute(ss)
    except MySQLdb.Error,(error,description):
         print "DependencyOffset:",ss
    
def saveDependencies(cursor,conceptsinsentence,idsent):
    global insertiondep
    global sentencesforconcept
    global thesource
    
    nbdep=0
 #   deps=xips.dependencyset("?") modification Vianney Grassaud
    deps=dependencyset("?")

    srce=str(thesource)
    unarydeps=[]    
    for idep in deps:
        dep=XipDependency(idep)
        if len(dep.parameters)==1:
           # on limite aux entite qui nous interesse modif du 25/10/2006 
           #modification supprimee car perte de corefence dans certain cas
           #if dep.name in ["PERSON", "DATE", "ORGANISATION", "LOCATION","TEMP"]:
              iddep=addidconcept(cursor,dep.name,4)
              listeval={}
              getDependencyNodes(cursor,dep.parameters[0],listeval,True)
              unarydeps.append([iddep,listeval])
        
    for idep in deps:
          dep=XipDependency(idep)
        # on limite aux dependances qui nous interesse modif du 25/10/2006
        #modification supprimee car perte de corefence dans certain cas
        #if dep.name in ["SUBJ", "OBJ", "MOD", "SUBJ-N", "OBJ-N"]:
          listeval={}
          origine='*'
          iddep=addidconcept(cursor,dep.name,4)
          addset(conceptsinsentence,iddep)
          addindexedset(iddep, sentencesforconcept, idsent)
          addset(listeval,iddep)
          for p in dep.parameters:
              saveDependencyNodes(cursor,p,listeval,conceptsinsentence,idsent,True)
          if len(dep.parameters)!=1:
             for i in listeval.keys():
                  for unaire in unarydeps:
                    try:
                      if (listeval[i] & unaire[1][i]) != 0:
                         addset(listeval,unaire[0])	
                    except:
                      pass
 
          for i in listeval.keys():
              ss='('+srce+','+str(idsent)+','+str(nbdep)+','+str(i)+','+str(listeval[i])+',"'+origine+'")'
              adddependency(ss)

          for pdepi in dep.parameters:
              nx=XipNode(pdepi)
              saveDependencyOffsets(cursor,nbdep,idsent,nx.leftoffset,nx.rightoffset)                        
          nbdep=nbdep+1
          if len(insertiondep)>=500:
              ss=",".join(insertiondep)
              ss=ss+";"
              try:
                  cursor.execute(ss)
              except MySQLdb.Error,(error,description):
                  print description
                  #print "ERREUR=",ss
              del insertiondep[0:len(insertiondep)]                    


#rajouter par Vianney Grassaud afin de stocker les offsets des phrases dans la table
def saveOffsets(cursor,idsent,left,right):
     global thesource

     ss='insert into offsets values('+str(thesource)+',0,'+str(idsent)+','+str(left)+','+str(right)+');'
     try:
          cursor.execute(ss)
     except MySQLdb.Error,(error,description):
          print description




def saveinmysql(idtop,idsent,txts,source):
    global cursor
    global thesource

    thesource=source

    ntop=XipNode(idtop)
    txt= str(txts)
    txt= txt[ntop.leftoffset:ntop.rightoffset]    
    txt=txt.replace('\\','\\\\')
    txt=txt.replace('"','\\"')
    conceptsinsentence={}

   #rajouter par Vianney Grassaud afin de stocker les offsets des phrases  
    saveOffsets(cursor,idsent,ntop.leftoffset,ntop.rightoffset)    

    saveSentence(cursor,idsent,txt)
    saveWordNodes(cursor,conceptsinsentence,idsent)    
    saveDependencies(cursor,conceptsinsentence,idsent)
    saveSentenceConcepts(cursor,conceptsinsentence,idsent)

######################################################################################################
### Specific to coreference saving...
#idconcept is the antecedent of the pronoun in the sentence idsent, at beginOffset
#idconcept is the antecedent of the pronoun in the sentence idsent, at beginOffset
def addinwordoffset(cursor,idconcept,idsent,beginoffset,endoffset):
    ss="insert into wordoffset values("+str(idconcept)+","+str(idsent)+","+str(beginoffset)+","+str(endoffset)+");"
    cursor.execute(ss)


def addinconcepttosentence(cursor,idconcept,idsent):
    r=rank(idsent)
    val=bitpos(idsent,r)
    ss="select * from concepttosentences where (idconcept="+str(idconcept)+" and rank="+str(r)+");"
    nb=cursor.execute(ss)
    if nb==0:
        ss="insert into concepttosentences values("+str(idconcept)+","+str(r)+","+str(val)+");"
        cursor.execute(ss)
    else:
        dd=cursor.fetchone()
        v=dd[2]
        v=v | val
        ss="update concepttosentences set value="+str(v)+" where idconcept="+str(idconcept)+" and rank="+str(r)+";"
        cursor.execute(ss)
    r=rank(idconcept)
    val=bitpos(idconcept,r)
    ss="select * from sentencetoconcepts where (idsent="+str(idsent)+" and rank="+str(r)+");"
    nb=cursor.execute(ss)
    if nb==0:
        ss="insert into sentencetoconcepts values("+str(idsent)+","+str(r)+","+str(val)+");"
        cursor.execute(ss)
    else:
        dd=cursor.fetchone()
        v=dd[2]
        v=v | val
        ss="update sentencetoconcepts set value="+str(v)+" where idsent="+str(idsent)+" and rank="+str(r)+";"
        cursor.execute(ss)

def addindependency(cursor,deplist,idconcept,idsent,idpronlist):
    global thesource

    r=rank(idconcept)
    val=bitpos(idconcept,r)
    valpron=0
    ss="select iddep,value,rank from dependances where idsource="+str(thesource)+" and idsent="+str(idsent)+" and iddep in ("+",".join(deplist)+");"
    nb=cursor.execute(ss)
    if (nb!=0):
        dd=cursor.fetchall()
        for a in dd:            
            try:
                #first we get the value, a[2] is the rank, if the pronoun rank is absent we go in exception
                valpron=idpronlist[a[2]]
                v=a[1]
                #if the value is already part of our test
                if (v & valpron):
                    try:
                        ss="select value from dependances where idsource="+str(thesource)+" and idsent="+str(idsent)+" and iddep="+str(a[0])+" and rank="+str(r)+";"
                        nb=cursor.execute(ss)
                    except MySQLdb.Error,(error,description):
                        print ss
                        print description
                    if (nb==0):
                        try:
                            ss="insert into dependances values("+str(thesource)+","+str(idsent)+","+str(a[0])+","+str(r)+","+str(val)+",\"+\");"
                            cursor.execute(ss)
                        except MySQLdb.Error,(error,description):
                            print ss
                            print description
                    else:
                        dp=cursor.fetchone()
                        v=dp[0]
                        if (v & val == 0):
                            v=v | val;
                            ss='update dependances set value='+str(v)+',type="-" where idsource='+str(thesource)+' and idsent='+str(idsent)+' and iddep='+str(a[0])+' and rank='+str(r)+';'
                            cursor.execute(ss)
            except:
                pass

#basic function to store our new word in the database
def addinfodb(cursor,words,prons,sent,leftoffpron,rightoffpron,pronpos):
    global thesource
    #print "COREF:",sent," Group:",words," Pron:",prons,"=",pronpos
    wordlist=words.split()
    for word in wordlist:
        idword=addidconcept(cursor,word,1)        
        if (idword!=-1):
            pronlist=prons.split()
            listeval={}
            for pron in pronlist:
                if pron not in stopwords:
                    idpron=addidconcept(cursor,pron,1)
                    addset(listeval,idpron)     
            addinwordoffset(cursor,idword,sent,leftoffpron,rightoffpron)
            addinconcepttosentence(cursor,idword,sent)
            ss='select iddep from dependencyoffset where idsent='+str(sent)+' and beginoffset='+str(leftoffpron)+' and endoffset='+str(rightoffpron)+';'
            cursor.execute(ss)
            deptuple=cursor.fetchall()
            deplist=[]
            for dl in deptuple:
                 deplist.append(str(dl[0]))
            addindependency(cursor,deplist, idword,sent,listeval)            
        
    #In the case that we have a group of words coreferenced, then we generate a new dependency
    #NOUNGROUP
    if len(wordlist)>1:
        groupid=addidconcept(cursor,"NOUNGROUP",4)
        addinconcepttosentence(cursor,groupid,sent)
        #we then create a complete dependency with all these words
        ss="select max(iddep) from dependances where idsource="+str(thesource)+" and idsent="+str(sent)+";"
        cursor.execute(ss)
        dd=cursor.fetchone()
        if dd[0]==None:
            nbdep=0
        else:
            nbdep=dd[0]+1
        listeval={}
        addset(listeval,groupid)
        for word in wordlist:
           idword=addidconcept(cursor,word,1)
           addset(listeval,idword)        
        for i in listeval.keys():
            ss='insert into dependances values('+str(thesource)+','+str(sent)+','+str(nbdep)+','+str(i)+','+str(listeval[i])+',"#");'
            cursor.execute(ss)



