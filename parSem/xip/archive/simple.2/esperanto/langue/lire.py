# -*- coding: utf-8 -*-
import xips

#grm=xips.loadgrm(r"W:\users\Roux\xip\parSem\xip\dev\GRAMMARS\FRENCH\BASIC\french.grm")
#grm=xips.loadgrm(r"W:\users\Roux\xip\parSem\xip\dev\GRAMMARS\ENGLISH\GRMFILES\GRAM_GEN\gram_gen.grm")

#xips.setdisplay(grm,xips.XIP_DISPLAY_NONE)

conversions={}
conversions["\xc3\xa6"]="\xc4\x89"  #c
conversions["\xc3\xb8"]="\xc4\x9d"  #g
conversions["\xc2\xb6"]="\xc4\xa5"  #h
conversions["\xc2\xbc"]="\xc4\xb5"  #j
conversions["\xc3\xbe"]="\xc5\x9d"  #s
conversions["\xc3\xbd"]="\xc5\xad"  #u
conversions["\xc3\x86"]="\xc4\x88"  #C
conversions["\xc2\xa6"]="\xc4\xa4"  #H
conversions["\xc3\x98"]="\xc4\x9c"  #G
conversions["\xc3\x9e"]="\xc5\x9c"  #S
conversions["é"]="e"                #é
conversions["è"]="e"                #è
conversions["â"]="a"                #â
gardes={}

def decoupe(j):
    ls=j.replace("(","(_")

    lstmot=ls.split(",")
    reslist=[]
    for l in lstmot:    
        k=l.split("(")

        if (len(k)!=1):
            liste=[]
            for i in k:
                p=i.split(")")
                if len(p)==1:
                    liste.append(i)
                else:
                    liste.extend(p)
            lst=[]
            for l in liste:
                if len(l)>0:                    
                    if l[0]!="_":
                        lst.append(l)
                    elif l=="_s'~" or l=="_se ~":
                        st=lst.pop()
                        st="pron(se);verb("+st.strip()+")"
                        lst.append(st)
            reslist.extend(lst)
        else:
            reslist.append(l)
    res=[]
    for r in reslist:
        pos=r.find("[")
        if pos!=-1:
            r=r[:pos]
        res.append(r)
    return res


def xiperie(s):
    xr=""
    res=xips.parsestring(grm,s,xr)
    res=res.strip()
    pos=res.find("(")
    cats=res[:pos].upper()
    return [cats,res]
    

def traiteexpressions(lfr,cat):
    reslst=[]
    for l in lfr:
        if " " in l.strip():
            s=unicode(l,"utf-8").encode("latin-1")
            [cats,res]=xiperie(s)
            if cats!=cat:
                if cat=="NOUN":
                    prescats=cats
                    if cats=="VERB" or cats=="ADJ":
                        s="Two "+s
                        [cats,res]=xiperie(s)
                        res=res[10:]
            res=unicode(res,"latin-1").encode("utf-8")
            reslst.append(res)
        else:
            reslst.append(l)
    return reslst

f=open(r"C:\ANL\Mekalinga\ANL\Mekalinga\simple\esperento\langue\English_Esperanto.wb","rb")
#f=open(r"C:\ANL\Mekalinga\ANL\Mekalinga\simple\esperento\langue\fresp.wb","rb")
dicos=""
for i in f:
    dicos=i
f.close()



print len(dicos)
chaines=[]
s=""

for d in dicos:
    if ord(d)==0:
        if s!="":
            mot=unicode(s,"latin-1").encode("utf-8")
            mot=mot.replace("[2]","")
            mot=mot.replace("1/4","")
            chaines.append(mot)
            print mot
        s=""
        continue
    s+=d




terminaisons={"a":"ADJ","i":"VERB","o":"NOUN","e":"ADV"}

lg=len(chaines)
f=open(r"C:\ANL\Mekalinga\ANL\Mekalinga\simple\esperento\langue\anglais_tres_large.txt","w")
try:
    for i in range(0,lg,2):
        esper=chaines[i]
        for c in conversions:
            esper=esper.replace(c,conversions[c])

        espere=decoupe(esper)
        englishword=chaines[i+1]
        print espere,englishword
        
        for iesp in espere:
            esp=iesp.strip()        
            if esp[0]=="-":
                continue
            if " " in esp:
                continue

            try:
                cat=terminaisons[esp[-1]]
            except:
                cat="Noun"
            
            try:     
                ch=unicode(esp,"utf-8").encode("utf-8")
            except:
                continue
            leng=len(unicode(ch,"utf-8"))
            if leng<90:
                ch+="".zfill(90-leng).replace("0"," ")
            lfr=decoupe(englishword)
            #lfr=traiteexpressions(lfr,cat)
            sfr=",".join(lfr)
            fr=unicode(sfr,"utf-8").encode("utf-8")
            fr=fr.replace("\xc2\x9c","oe")
            fr=fr.replace("\xc2\x8c","Oe")
            ch+=fr
            ch=unicode(ch,"utf-8").encode("utf-8")
            if esp in gardes:
                gardes[esp]+=", "+fr
            else:
                gardes[esp]=ch
except:
    pass
sgardes=gardes.keys()
sgardes.sort()
for i in sgardes:
    ch=gardes[i]
    f.write(ch+"\n")
f.close()

