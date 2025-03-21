Python:

coref_final(int fake),
coref_final_db(int start).

import sys
 

outfhtml = None
outfres = None


def coref_final(fake_arg):
    global TextParseDB
    global outfhtml
    global outfres
    
    #
    TextParseDB.process()
    std = sys.stdout
    
    # Printing results
    if (outfhtml==None):
        outfhtml = open('res.html','w')
        TextParseDB.myPrintHeader(outfhtml)
        
    else:
        outfhtml.write('\n<P><hr><P><br>\n\n')
        
    TextParseDB.printAnnotatedTextLight(outfhtml)
    
    if (fake_arg==1):
        TextParseDB.myPrintDocEnd(outfhtml)
    
    if (outfres==None):
        outfres = open('res','w')
        
    sys.stdout = outfres
        
    TextParseDB.myprint()
    sys.stdout = std
    TextParseDB.reset()

    
#############Partie destinee a la base de donnee
global inserttheconcepts
global insertsentences
global insertconcepts
global insertiondep
global insertwordoffset
global idsent
global theconcepts
global sentencesforconcept

inserttheconcepts=[]
insertsentences=[]
insertconcepts=[]
insertiondep=[]
insertwordoffset=[]
idsent=0
theconcepts={}
sentencesforconcept={}

def addinserttheconcepts(ss):
    global inserttheconcepts
    if inserttheconcepts==[]:
        sc='insert into concepts values'+ss
        inserttheconcepts.append(sc)
    else:
        inserttheconcepts.append(ss)
 

def loadtheconcepts(cursor):
    global theconcepts

    ss='select idconcept,word,POS from concepts;'
    cursor.execute(ss)
    dd=cursor.fetchall()
    for a in dd:
       #print "LECTURE:",a
       wkey=(a[1],int(a[2]))
       theconcepts[wkey]=a[0]
  
def savetheconcepts(cursor):
    global theconcepts
    global inserttheconcepts
    print "Saving concepts"

    inserttheconcepts=[]

    for wkey in theconcepts:
        id=theconcepts[wkey]
        word=wkey[0]
        word=word.replace('\\','\\\\')
        word=word.replace('"','\\"')
        pos=wkey[1]
        ss='('+str(id)+',"'+word+'",1,'+str(pos)+')'
        addinserttheconcepts(ss) 
## nouvelle version
        try:
             ss=",".join(inserttheconcepts)
             ss=ss+";"
             cursor.execute(ss)
        except:
             #print "BAD QUERY ############### ",inserttheconcepts
             pass
        inserttheconcepts=[]


##  ancienne version
#       if len(inserttheconcepts)>5000:
#          try:
#              ss=",".join(inserttheconcepts)
#              ss=ss+";"
#              cursor.execute(ss)
#          except:
#              print "BAD QUERY ############### ",inserttheconcepts
#          inserttheconcepts=[]
#
#  if inserttheconcepts!=[]:
#       ss=",".join(inserttheconcepts)
#       ss=ss+";"
#       try:
#            cursor.execute(ss)
#       except:
#             print "BAD QUERY ############### ",inserttheconcepts
#       inserttheconcepts=[]
    
def addinsertconcepts(ss):
    global insertconcepts
    if insertconcepts==[]:
        sc='insert into concepttosentences values'+ss
        insertconcepts.append(sc)
    else:
        insertconcepts.append(ss)

def saveConceptSentences(cursor):
    global sentencesforconcept
    global insertconcepts


    insertconcepts=[]
    for key in sentencesforconcept:
        idconcept = key[0]
        rank = key[1]
        ss='('+str(idconcept)+','+str(rank)+','+str(sentencesforconcept[key])+')'
        addinsertconcepts(ss)
        if len(insertconcepts)>=10000:
            ss=",".join(insertconcepts)
            ss=ss+";"
            cursor.execute(ss)
            insertconcepts=[]

    if insertconcepts!=[]:
        ss=",".join(insertconcepts)
        ss=ss+";"
        cursor.execute(ss)
        insertconcepts=[]

        
def savefinal():
    global cursor
    global insertsentences
    global insertiondep
    global insertwordoffset

    saveConceptSentences(cursor)
    savetheconcepts(cursor)
    if insertsentences!=[]:
        ss=",".join(insertsentences)
        ss=ss+";"
        cursor.execute(ss)

    if insertwordoffset!=[]:
        ss=",".join(insertwordoffset)
        ss=ss+";"
        cursor.execute(ss)

    if insertiondep!=[]:
        ss=",".join(insertiondep)
        ss=ss+";"
        cursor.execute(ss)
    



global mydb
global cursor

def coref_final_db(start):
    global TextParseDB
    global cursor
    global inserttheconcepts
    global insertsentences
    global insertconcepts
    global insertiondep
    global insertwordoffset
    global idsent
    global theconcepts
    global sentencesforconcept

    if start==1:
        print "PRE-INITIALISATION"
        TextParseDB=TextParse()
        print "INITIALISATION"
        inserttheconcepts=[]
        insertsentences=[]
        insertconcepts=[]
        insertiondep=[]
        insertwordoffset=[]
        idsent=0
        theconcepts={}
        sentencesforconcept={}
        loadtheconcepts(cursor)
        #print "LES CONCEPTS:", len(theconcepts)
        return
    else:
        print "ANALYSE FINALE"
    
    savefinal()
    TextParseDB.process()
    
    TextParseDB.savedb()
    



