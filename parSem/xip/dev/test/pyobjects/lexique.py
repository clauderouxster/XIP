import toklook

def testlist(i):
        if i.__class__.__name__=='list':
                return True
        return False

def testint(i):
        if i.__class__.__name__=='int':
                return True
        return False

def teststr(i):
        if i.__class__.__name__=='str':
                return True
        return False

def display(ref,wrd,p,pasglob):
        for i in wrd:
                if i=="n":
                        continue
                if wrd[i]["n"] in pasglob:
                        continue
                pasglob[wrd[i]["n"]]=True
                sp="".ljust(p," ")
                try:
                        key=i.split("_")
                        print sp+chr(int(key[0]))+"/"+chr(int(key[1])),wrd["n"],
                except:
                        print sp,i,
                try:
                        print wrd[i]["n"]
                        display(ref,wrd[i],p+1,pasglob)
                except:
                        pass                  

def lastarc(wrd,m,r,p):
        if (p==len(m)):
                return wrd

        key=m[p]+"_"+r[p]
        try:
                return lastarc(wrd[key],m,r,p+1)
        except:
                pass
                

def printinf(a):
        e=a.split("|")
        sb=""
        sh=""
        for i in e:
                k=i.split("_")
                sb+=chr(int(k[1]))
                sh+=chr(int(k[0]))
                
        print sb,"/",sh


def keydisp(a):
        s=""
        try:
                b=a.split("_")
                s=chr(int(b[0]))+"_"+chr(int(b[1]))
        except:
                pass
        return s


finalstate=chr(1)+chr(5)

def perfecthash(wrds,base):
        global finalstr
        kys=wrds.keys()
        print wrds[kys[0]].keys(),kys[0]
        print kys
        if len(kys)==0:
                return
        for k in kys:
                print k
                if k=='n':
                        continue
                perfecthash(wrds[k],base)
                if k[0:2]==finalstate:
                        #Final state
                        base[0]+=1
                
                base[0]+=1
                
                
                     

thearcs={}
def arcandstate():
        global thearcs
        toklook.loadscript("C:\\XIP\\Test\\toksrc\\French\\min.scr","","raw","raw")
        f=open("C:\\XIP\\Test\\toksrc\\French\\french.un","r")
        wrdstates={}
        lbsatates={}
        thelb={}
        ilb=0
        arcs=[]
        arcslb=[]
        endings={}
        iendings=0
        theendings=[]
        finals={}
        thefinals={}
        iarc=0
        cc=0
        ifinal=0
        isave=0
        isavearc=0
        idxwrd=0
        maxend=0
        #f={"abcefgh12345":[["abcde","abcd","+12+34"]],"ijklmn12345":[["jklde","kkjji","+12+34"],["jklde","kkjj","+34"]]}
        for mt in f:
                cc+=1
                if cc%500==0:
                        print ".",
                        if cc%10000==0:
                                print cc,iarc,ilb,len(thearcs)
                                break
                        
                mt=mt[0:-1]
                #mtencode=mt.decode("latin").encode("utf8")
                lstmt=[mt]
                
                #if mtencode!=mt:
                #        lstmt.append(mtencode)
                s=toklook.lookup(0,mt)
                #print mt
                #s=f[mt]
                for e in s:
                        #state is a dictionary of arcs
                        res=e[1]
                        ends=e[2]
##                        try:
##                                trm=endings[ends]
##                        except:
##                                endings[ends]=iendings
##                                theendings.append(ends)
##                                trm=iendings
##                                iendings+=1
                        for m in lstmt:
                                m+=chr(1)
                                if len(m)!=len(res):
                                        if len(m)>len(res):
                                                res=res.ljust(len(m),chr(3))
                                        else:
                                                m=m.ljust(len(res),chr(1))
                                pos=0
                                ckeys=[]
                                ckeystr=[]
                                for i in m:
                                        #key=str(ord(i))+"_"+str(ord(res[pos]))
                                        key=i+chr(5)+res[pos]
                                        keystr=i+res[pos]
                                        ckeys.append(key)
                                        ckeystr.append(keystr)
                                        pos+=1
                                pos=0
                                state=wrdstates
##                                for key in ckeys:
##                                        #We build a internal index, to share the endings                                        
##                                        #new arc
##                                        try:
##                                                arc=state[key]
##                                        except:
##                                                arc={"n":iarc}
##                                                state[key]=arc
##                                                arcs.append({"k":key,"a":arc})
##                                                iarc+=1
##                                        state=arc
##                                        pos+=1
                                
                                #we share some structure                                
                                for key in ckeys:
                                        #We build a internal index, to share the endings                                        
                                        #new arc
                                        try:
                                                arc=state[key]
                                        except:
                                                reverseindex="".join(ckeystr[pos:])                                                
                                                try:
                                                        arc=thearcs[reverseindex]
                                                        state[key]=arc
                                                        isave+=1
                                                        #We have what we want, we stop
                                                        break
                                                except:
                                                        arc={"n":iarc}
                                                        state[key]=arc
                                                        arcs.append({"k":key,"a":arc})
                                                        iarc+=1
                                                        if len(reverseindex)!=0:
                                                                thearcs[reverseindex]=arc
                                        state=arc
                                        pos+=1

                                state=lbsatates
                                pos=0

                                for k in ends:
                                        key=ord(k)
                                        try:
                                                arc=state[key]
                                        except:
                                                rv="".join(ends[pos:])
                                                try:
                                                        arc=thelb[rv]
                                                        state[key]=arc
                                                        break
                                                except:
                                                        arc={"n":ilb}
                                                        state[key]=arc
                                                        arcslb.append({"k":key,"a":arc})
                                                        ilb+=1
                                                        if len(rv)!=0:
                                                                thelb[rv]=arc
                                        state=arc
                                        pos+=1


        #Now we compute some hash values
        base=[0]
        perfecthash(wrdstates,base)
        print
        print "VALUE"
                        

##        for i in wrdstates:
##                print "Arbre"
##                print i
##                display(wrdstates,wrdstates[i],1,{})
##                print
##        print "Total"
##        display(wrdstates,1,{})
##        print "Clef:"
##        for i in thearcs:
##                print i,":",thearcs[i]["n"]
        #we look for a word
        arret=True
        print
        while not arret:
                m=raw_input("Chaine:")
                if m[0]=='#':
                        try:
                                exec(m[1:])
                        except:
                                print "Non"
                        continue
                rparse=toklook.lookup(0,m)
                print m,rparse
                for rp in rparse:
                        res=rp[1]
                        m+=chr(1)
                        if len(m)!=len(res):
                                if len(m)>len(res):
                                        res=res.ljust(len(m),chr(3))
                                else:
                                        m=m.ljust(len(res),chr(1))
                        print m,res
                        pos=0
                        st=wrdstates
                        for u in m:
                                key=u+chr(5)+res[pos]
                                try:
                                        st=st[key]
                                        print u,res[pos],st["n"],
                                except:
                                        pass
                                pos+=1
                        print
                        

        ia=0
        #We store the endings
        print "SAVING"
        saving=open("C:\\XIP\\Test\\toksrc\\French\\french.arc","w")        
        saving.write(str(len(endings))+"\n")
        #lst=theendings.keys()
        #lst.sort()
        for i in theendings:
                saving.write(i+"\n")
        saving.write("\n")
        ia=0
        lst={}
        #we store the initial character
        for i in wrdstates:
                k=i.split(chr(5))
                k1,k2=k[0],k[1]
                try:
                        lst[k1].append(wrdstates[i]["n"])
                except:
                        lst[k1]=[wrdstates[i]["n"]]
        klst=lst.keys()
        for i in klst:
                for j in lst[i]:
                        st="i"+str(ord(i))+"a"+str(j)
                        saving.write(st)
        saving.write("e")
        saving.write(str(len(arcs)))
        ia=0
        for a in arcs:
                # a["k"] is the key
                # a["a"] is the next arcs
                # a["a"]["n"] is the id
                if ia%10000==0:
                        print ".",
                        if ia%100000==0:
                                print ia
                ky=a["k"]
                k=ky.split(chr(5))
                debut=True
                if k[0]==chr(1):
                        s="z",k[1]
                        if len(k[1])==0:
                                print s,ky
                        debut=False
                elif k[1]==chr(2):
                        s="y",k[0]
                        debut=False                        
                else:
                        s="u",k[0],k[1]
                        debut=False

                lst=a["a"].keys()
                sortinglst={}
                cpt=0
                for i in lst:
                        if i=='n':
                                continue
                        if i=='e':
                                continue
                        if testint(i):
                                k=-1
                        else:
                                k=i.split("_")
                                k=k[0]
                        
                        try:
                                sortinglst[k].append(i)
                        except:
                                sortinglst[k]=[i]
                        
                lst=sortinglst.keys()
                lst.sort()
                for i in lst:
                        ky=sortinglst[i]
                        for k in ky:
                                try:
                                        diff=a["a"][k]["n"]-ia
                                        s+="n",diff
                                except:
                                        pass
                st=""
                for i in s:
                        st+=str(i)
                saving.write(st)
                ia+=1
        print
        saving.close()
arcandstate()

#toklook.loadscript("/home/roux/cvsxip/python/Python-2.4.1/toksrc/French/min.scr","","raw","raw")
#rrs=toklook.lookup(0,"texte")
#print rrs

                        
bodies={}


def onlyoneform(res):
        s=res[0][0]
        for i in res:
                if s!=i[0]:
                        return False
        return True

def getforms(res,mots):
        l=[]
        s={}
        for r in res:
                lst=mots[r]
                for i in lst:
                        if i[0] not in l:
                                l.append(i[0])
                                try:
                                        s[i[0]].append(r)
                                except:
                                        s[i[0]]=[r]
        return [l,s]


def initfile():
        saving=open("/home/roux/cvsxip/python/Python-2.4.1/toksrc/english.py","w")
        toklook.loadscript("/home/roux/cvsxip/python/Python-2.4.1/toksrc/min.scr","","raw","raw")
        f=open("/home/roux/cvsxip/python/Python-2.4.1/toksrc/words.un")
        bodies={}
        compteur=0
        cc=0
        mm=0
        mots={}
        indexmots={}
        lecompteur=0
        lindex=0
        saving.write("# coding: iso-8859-1\n")
        saving.write("# -*- coding: iso-8859-1 -*-\n")
        saving.write("ew={}\n")
        for m in f:
                m=m[0:-1]
                s=toklook.lookup(0,m)
                saving.write('ew["'+m+'"]='+str(s)+"\n")
                
#initfile()
                
def checkwrd(wrd,m,r,p):
        if len(m)==p:
                return True
        try:
                key=str(ord(m[p]))+"_"+str(ord(res[p]))
        except:
                key=str(ord(m[p]))+"_0"
        if key in wrd:
                return check(wrd[key],m,r,p+1)
        return False
                

def parsereg(s,rule):
    #code: 1 --> %? (any)
    #code: 2 --> %C (uppercase)
    #code: 3 --> %c (lowercase)
    #code: 4 --> %d (digit)    
    #code: 5 --> %p (punctuation)
    #code: 6 --> +
    #code: 7 --> *

    
    sourcecode="?Ccdp"
    pos=0
    while pos<len(s):
            i=s[pos]
            if i=='%':
                    code=sourcecode.find(s[pos+1])
                    if code!=-1:
                            code+=1
                            rule.append(code)
                            pos+=2
                            continue
                    else:
                        rule.append(s[pos+1])
                        pos+=2
                        continue
            if i==',':
                pos+=1
                continue
            if i=='[' or i=='(':
                    rl=[]
                    pos+=parsereg(s[pos+1:],rl)
                    rule.append(rl)
                    continue
            if i==']' or i==')':
                    if i==')':
                        rule.append(0)
                    return pos+2
            if i=='+' or i=='*':
                if i=='+':
                    code=6
                else:
                    code=7
                rule[-1]=[code,rule[-1]]
                pos+=1
                continue
            rule.append(s[pos])
            pos+=1


def addarcs(rl,wrds,arcs,liarc,disjunct):
        state=wrds
        for i in rl:
                print "Inter",i
                if testlist(i):
                        print "Liste"
                        #The first element is an indication of the type of element (either +, or *)
                        loop=0
                        suite=i
                        if testint(i[0]) and (i[0]==6 or i[0]==7):
                                loop=i[0]
                                suite=i[1:]
                        wd={}
                        addarcs(suite,wd,arcs,liarc,True)
                        #we create an arc epsilon
                        if loop==0:
                                key="0_0"
                                arc={"n":liarc[0]}
                                state[key]=arc
                                arcs.append({"k":key,"a":arc})
                                liarc[0]+=1
                                for l in wd:
                                        arc[l]=wd[l]
                                state=arc
                                print "WD",wrds
                        continue
                key=""
                if testint(i):
                        print "INT"
                        key=str(i)+"_"+str(i)
                if teststr(i):
                        print "Str"
                        key=key=str(ord(i))+"_"+str(ord(i))
                print key
                try:
                        arc=state[key]
                except:
                        #new arc
                        arc={"n":liarc[0]}
                        state[key]=arc
                        arcs.append({"k":key,"a":arc})
                        liarc[0]+=1
                if disjunct==False:
                        state=arc

