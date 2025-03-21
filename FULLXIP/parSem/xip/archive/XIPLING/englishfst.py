import ending
import lingata

def evalue(t):
    plus=0
    espace=1
    for i in t:
        if i=="+":
            plus+=1
        elif i==" ":
            espace+=1
    if espace!=plus:
        return False
    return True
        
fst=open(r"C:\eclipse\XIPLING\lingvataenglish.src","w")



i=0
       
for l in lingata.lexicons:
    w=lingata.lexicons[l]
    if "english" in w:
        en=w["english"]
        l=unicode(l,"utf8")
        xl=" ".join(list(l))
        for f in en:
            if "..." in f:
                continue
            f=unicode(f,"utf8")            
            feat=""
            if "(" in f:
                xtrad=""
                testtrad=""
                fpos=f.split(",")
                for xfpos in fpos:
                    if xtrad!="":
                        xtrad+=" %# "
                        testtrad+=" "
                    ps=xfpos.find("(")
                    pos=xfpos[:ps]                    
                    wrd=xfpos[ps+1:-1]
                    testtrad+=wrd+"+"+pos
                    wrd=" ".join(list(wrd))
                    if evalue(testtrad):
                        xtrad+=wrd
                        feat+="%+"+pos+" "
                    else:
                        xtrad=wrd
            else:
                xtrad=" ".join(list(f))
            try:
                feat=feat.encode("utf8")
            except:
                pass
            feat+=" %+En"
            lg=xtrad.encode("utf8")+"\t"+feat
            lg=lg.strip()
            fst.write(lg+"\r\n")
            lg=xl.encode("utf8")
            lg=lg.strip()
            fst.write(lg+"\r\n")

fst.close()     


