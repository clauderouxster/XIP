# -*- coding: utf8 -*-
######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: comparta.py
#Comment: This file implements the generation of the ending.py file
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

#################################################################
#Some important structures
terminations={"t":["Neutral","Noun"],"m":["Masc","Noun"],"f":["Fem","Noun"],
              "c":["Adj"], "l":["Det"],"ia":["Verb"],"ie":["Rel","Verb"],
              "no":["Adv"],"ar":["Prep"],"er":["Pron"],"nu":["Conjunction"]}

equivalences={"c":"ca","ie":"iag" ,"ar":"ar" ,"er":"er" ,"l":"la","ia":"iag","t":"ta" ,"f":"fa","m":"ma","no":"no","nu":"nu"}

verbending={"g":"Pres","p":"Past","f":"Fut","d":"Cond","gas":"Inf","gad":"InfPrep"}
invertverbs={"Pres":"g","Past":"p","Fut":"f","Cond":"d","Inf":"gas","InfPrep":"gad"}

cases={"#":"Nom","n":"Acc","s":"Gen","d":"Dat"}
invertcases = {"Nom":"","Acc":"n","Gen":"s","Dat":"d"}

conjunctions={"que":"And","quo":"Or"}

m_cases={"#":"M_Nom","n":"M_Acc","s":"M_Gen","d":"M_Dat"}
m_genders=["M_Masc","M_Fem","M_Neutral"]

neginter={"ne":"Neg","qua":"Inter","quane":"Inter+Neg"}

#on genere les noms, les adjectifs, les determinants et les pronoms
nominalcategories=["Noun","Adj","Det"]
semantics={"a":"+Base","e":"+Loc","o":"+Proper","i":"+Temp","u":"+Num"}
basepr={"k":"","f":"+M_Fem","m":"+M_Masc"}
subject_pronoun={"me":"+Sg+Pers1","yu":"+Sg+Pers2","e":"+Sg+Pers3",
               "no":"+Pl+Pers1","vo":"+Pl+Pers2","ze":"+Pl+Pers3"}

possessives= {"t":"","f":"+M_Fem","m":"+M_Masc"}

#################################################################
#Generating all possible endings

def addrevert(i,x,generes):
    if i==x:
        return
    if i in generes:
        if x not in generes[i]:
            generes[i].append(x)
    else:
        generes[i]=[x]

def fillup(endingrules,pronending):
    global m_genders
    global cases
    global m_cases
    global conjunctions
    global neginter
    
    generes={}

    for i in terminations.keys():
        ending=terminations[i]
        basicfeature="+!"+equivalences[i.lower()]+"+"+"+".join(ending)
        if ending[-1]=="Pron":
            for mc in m_cases.keys():
                enrichbasicfeature="+"+m_cases[mc]+basicfeature
                if mc=="#":
                    formebase=i
                else:
                    formebase=mc+i
                for j in cases.keys():
                    #we then build the case value with our features
                    #the ending stores our basic features
                    #we add the case feature
                    cs="+"+cases[j]+enrichbasicfeature
                    csbase="+"+cases[j]+basicfeature            
                    #If it is the "#", then we consider it as the Null value
                    if j=='#':
                        bs=formebase
                    else:
                        #else we add the case string
                        bs=formebase+j
                    #our first rule, is the basic feature with the case feature
                    if mc=="#":
                        endingrules[bs]=csbase
                    pronending[bs]=cs
                    #we build the revert index
                    addrevert(formebase,bs,generes)
        elif ending[-1]=="Prep":
            for mc in m_cases.keys():
                enrichbasicfeature="+"+m_cases[mc]+basicfeature
                if mc=="#":
                    formebase=i
                else:
                    formebase="i"+mc+i                    
                for j in ["#","s"]:
                    #we then build the case value with our features
                    #the ending stores our basic features
                    #we add the case feature
                    cs="+"+cases[j]+enrichbasicfeature
                    csbase="+"+cases[j]+basicfeature            
                    #If it is the "#", then we consider it as the Null value
                    if j=='#':
                        if mc=="#":   
                            bs=formebase
                            endingrules[bs]=csbase
                            addrevert(formebase,bs,generes)
                        else:
                            continue
                    else:
                        #else we add the case string
                        bs=formebase+j
                        endingrules[bs]=cs
                        #we build the revert index
                        addrevert(formebase,bs,generes)
                    #we then add the potential conjunction
                    for s in conjunctions.keys():
                        csb="+"+conjunctions[s]+cs
                        bsb=bs+s
                        endingrules[bsb]=csb
                        addrevert(formebase,bsb,generes)                                                    
        if ending[-1] in nominalcategories:
            for se in semantics:
                enrichbasicfeature=semantics[se]+basicfeature
                formebase=i+se
                for j in cases.keys():
                    #we then build the case value with our features
                    #the ending stores our basic features
                    #we add the case feature
                    cs="+"+cases[j]+enrichbasicfeature
                    #If it is the "#", then we consider it as the Null value
                    if j=='#':
                        bs=formebase
                    else:
                        #else we add the case string
                        bs=formebase+j
                    #our first rule, is the basic feature with the case feature
                    endingrules[bs]=cs
                    #we build the revert index
                    addrevert(formebase,bs,generes)
                    #the det and the pron do not have any possible conjunction value
                    if ending[-1] in ["Det","Adv","Conjunction"]:
                        continue
                    #we then add the potential conjunction
                    for s in conjunctions.keys():
                        csb="+"+conjunctions[s]+cs
                        bsb=bs+s
                        endingrules[bsb]=csb
                        addrevert(formebase,bsb,generes)
        elif ending[-1]=="Adv":
                #We keep the basic values, no case marking in this case
                endingrules[i]=basicfeature
                #we build the revert index
                addrevert(i,i,generes)
        elif ending[-1]=="Conjunction":
                #We keep the basic values, no case marking in this case
                endingrules[i]=basicfeature
                #we build the revert index
                addrevert(i,i,generes)     
                endingrules[i+"s"]="+Gen"+basicfeature
                addrevert(i,i+"s",generes)             
        elif ending[-1]=="Verb":
            #We then add the endings
            for envb in verbending.keys():
                vbform=i+envb
                enrichbasicfeature="+"+verbending[envb]+basicfeature                
                #We keep the basic values
                endingrules[vbform]=enrichbasicfeature
                #we build the revert index
                addrevert(vbform,vbform,generes)            
                #we add the conjunction
                for s in conjunctions.keys():
                    #we add the conjunction to the string
                    strconj=vbform+s
                    #we add the features
                    featconj="+"+conjunctions[s]+enrichbasicfeature
                    #we then create our rule
                    endingrules[strconj]=featconj
                    #and our revert index
                    addrevert(vbform,strconj,generes)
                    #we add the negation
                    for n in neginter.keys():
                        strconjneg=strconj+n
                        featconjneg="+"+neginter[n]+featconj
                        endingrules[strconjneg]=featconjneg
                        addrevert(vbform,strconjneg,generes)
                #we add the negation on the raw form
                for n in neginter.keys():
                    strneg=vbform+n
                    featneg="+"+neginter[n]+enrichbasicfeature
                    endingrules[strneg]=featneg
                    addrevert(vbform,strneg,generes)
        

def complen(x,y):
    if len(x)>len(y):
        return -1
    if len(x)==len(y):
        if x>y:
            return -1
        elif x<y:
            return 1
        return 0
    return 1



#################################################################
#Generating the file corresponding to all possible endings
def generateending():
    endingrules={}
    pronending={}

    fillup(endingrules,pronending)

    codepy=open("ending.py","w")
    
    pos={}
    for t in terminations.keys():
        pos[t]=terminations[t][-1]

    
        
    ajoutforme=[]
    s=endingrules.keys()
    s.sort(complen)
    #We first add the Subject pronoun
    for sp in subject_pronoun.keys():
        racsp=sp
        for e in s:
            rs=endingrules[e]
            if "Base" in rs:
                if rs[-5:]=="+Noun":
                    prn="^"+str(len(e))+sp+e
                    feat="+SubjPron"+subject_pronoun[sp]+rs
                    feat+="+?"+racsp
                    endingrules[prn]=feat
                    ajoutforme.append(prn)
                if (rs[-4:]=="+Adj" and "!ca" in rs) or (rs[-4:]=="+Det" and "!la" in rs):
                    for gn in ["","f","m"]:
                        prn="^"+str(len(e))+sp+gn+e
                        feat=subject_pronoun[sp]+rs
                        feat+="+?"+racsp
                        if gn!="":
                            feat=feat.replace("!","!"+gn)
                            feat+=possessives[gn]
                        feat=feat.replace("+Pl","")
                        endingrules[prn]=feat
                        ajoutforme.append(prn)
                        if e[0]=="l": #a determiner
                            #we need to add the plural form
                            prn="^"+str(len(e))+sp+gn+"mu"+e
                            feat=feat.replace("+Sg","")
                            endingrules[prn]="+Pl"+feat
                            ajoutforme.append(prn)                            
                            

    #We add the basic clause pronouns
    for bs in basepr:
        for se in semantics.keys():
            b=bs+se
            ft=semantics[se]+basepr[bs]
            for e in pronending.keys():
                m="^"+str(len(e))+b+e                
                mf=ft+pronending[e]+"+?"+"k"+se
                mf=mf.replace("!er","!"+e)
                endingrules[m]=mf                
                ajoutforme.append(m)
                #We also add the regular forms of a clause pronoun
                if e not in endingrules:
                    ed="i"+e
                    if ed not in endingrules:                    
                        mf=ft+pronending[e]
                        endingrules[ed]=mf
                        ajoutforme.append(ed)

    #We add the possessive determiners
    for bs in possessives:
        base=bs+"a"
        fbase="+Dem"+possessives[bs]+"+!la+Det"
        for mc in m_cases:
            if mc!='#':
                b=base+mc
            else:
                b=base
            b+="la"
            ft="+"+m_cases[mc]+fbase                                
            for e in cases:
                if e!='#':
                    m="^"+str(len(e))+b+e
                else:
                    m="^0"+b
                mf="+"+cases[e]+ft+"+?"+"ta"
                endingrules[m]=mf
                ajoutforme.append(m)

    #determiners
    for bs in ["al","mul","mual"]:
        if "mu" in bs:
            fbase="+Pl+Det"
        else:
            fbse="+Det"
        myroot=bs[:-1]
        if bs!="mul":
            fbase+="+Def"
        for sem in semantics.keys():
            b=bs+sem
            feat=fbase+semantics[sem]
            for e in cases.keys():
                if e!='#':
                    m="^"+str(len(e))+b+e
                else:
                    m="^0"+b
                mf="+"+cases[e]+feat+"+!la+?"+myroot
                endingrules[m]=mf
                ajoutforme.append(m)
                if e!="#":
                    m=b+e
                else:
                    m=b
                endingrules[m]=mf
                ajoutforme.append(m)
    s=ajoutforme+s
            
    pronouns=[]

    firstending=True
    codepy.write("# -*- coding: utf8 -*-\n\n")
    codepy.write("cases={")
    first=True
    for cx in cases.keys():
        if cx=="#":
            c=""
        else:
            c=cx
        if not first:
            codepy.write(",")
        first=False
        codepy.write('"'+cases[cx]+'":"'+c+'"')
    codepy.write("}\n\n")
    codepy.write("basicwords={}\n")
    maxlen=0
    for r in s:
        ln="-"+str(len(r))
        x=endingrules[r]
        x=x.split("+")
        x=x[1:]
        rs={}
        for i in x:
            if i[0]=="!":
                rs["basis"]=i[1:]
            if i[0]=="?":
                rs["root"]=i[1:]
            elif i in cases.values():
                rs["case"]=i
            elif i in m_cases.values():
                rs["antecedentcase"]=i[2:]
            elif i in m_genders:
                rs["antecedentgender"]=i[2:].lower()
            elif i in pos.values():
                rs["pos"]=i
            elif i in ["Masc","Fem","Neutral"]:
                rs["gender"]=i.lower()
            elif i == "Neg":
                rs["neg"]="+"
            elif i == "Dem":
                rs["possessive"]="+"
            elif i == "Sg":
                rs["sg"]="+"
            elif i == "Pl":
                rs["pl"]="+"
            elif i == "Def":
                rs["def"]="+"
            elif i in ["Pers1","Pers2","Pers3"]:
                rs["pers"]=i[4]
            elif i == "Proper":
                rs["proper"]="+"
            elif i == "SubjPron":
                rs["pronsubj"]="+"                
            elif i == "Num":
                rs["num"]="+"
            elif i == "Loc":
                rs["loc"]="+"
            elif i == "Temp":
                rs["temp"]="+"
            elif i == "Inter":
                rs["inter"]="+"
            elif i == "InfPrep":
                rs["tense"]="Inf"
                rs["case"]="Dat"
            elif i in verbending.values():
                rs["tense"]=i
            elif i == "Rel":
                rs["rel"]="+"
            elif i in conjunctions.values():
                rs["conj"]=i
        if "tense" in rs and "rel" not in rs:
            rs["rel"]="-"
        
                
        
        if r[0]=="^":
            lgend=int(r[1])
            rs['suffix']=r[-lgend:]
            kys="mot"
            lg1=str(rs)
            tst=r[2:]
            codepy.write('basicwords["'+tst+'"]='+lg1+"\n")
        else:
            if firstending:
                codepy.write("\nonending={}\n")
            firstending=False
            kys='mot['+ln+':]'
            tst=r
            rs['suffix']=tst
            lg1=str(rs)
            codepy.write('onending["'+tst+'"]='+lg1+"\n")
        ts=int(ln)*-1
        if maxlen<ts:
            maxlen=ts

    ecr="""
      
def ending(mot):
  motmin=mot.lower()
  if motmin in basicwords:
     res=basicwords[motmin].copy()
     return res
  lg=len(mot)-1
  for i in range(lg,1,-1):
    base=mot[-1*i:]
    if  base in onending:
       res=onending[base].copy()
       res["root"]=mot[:-1*i]
       return res
  return None
"""
    codepy.write(ecr)
    codepy.close()
    
generateending()


