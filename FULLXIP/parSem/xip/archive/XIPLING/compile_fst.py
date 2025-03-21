import ending
import lingata

endingpos={}
semantics=["loc","temp","proper","num"]

def testsem(feats):
    r=""
    for s in semantics:
        if s in feats:
            r="_"+s
    if "gender" in feats:
        r+="_"+feats["gender"]
    return r

actions={"case":True,"pos":True,"gender":True,"neg":False,"possessive":False,"sg":False,"pl":False,"def":False,"proper":False,"num":False,"loc":False,"temp":False,"inter":False,"tense":True,"conj":False}


order=["case","tense","gender","antecedentcase","antecedentgender","rel","neg","possessive","sg","pl","pers","def","proper","num","loc","temp","inter","conj","pos"]

def features(feats):
    s=""
    for k in order:
        if k in feats:
            try:
                if actions[k]:
                    s+=" +"+feats[k].lower()
                else:
                    s+=" +"+k
            except:
                if k=="rel":
                    if feats[k]=='+':
                        s+=" +rel"
                if k=="pers":
                    s+=" +p"+str(feats[k])
                if k in ["antecedentcase","antecedentgender"]:
                    if k in feats:
                        s+=" +ante"+feats[k].lower()
            
    return s
    

for e in ending.onending:
    feats=ending.onending[e]
    pos=feats["pos"].lower()
    pos+=testsem(feats)

    try:
        endingpos[pos].append(feats)
    except:
        endingpos[pos]=[feats]

fst=open(r"C:\eclipse\XIPLING\lingata.src","w")

i=0


for b in ending.basicwords:
    w=ending.ending(b)
    l=w["root"]
    xlc=" ".join(list(b))
    xl=" ".join(list(l))
    fts=features(w)
    st=xl+fts
    fst.write(st+"\n")
    fst.write(xlc+"\n")
    i+=1
        
for l in lingata.lexicons:
    w=ending.ending(l)
    pos=w["pos"].lower()+testsem(w)
    if pos in endingpos:
        for e in endingpos[pos]:
            lc=w["root"]+e["suffix"]
            xlc=" ".join(list(lc))
            xl=" ".join(list(w["root"]))
            fts=features(e)
            st=xl+fts
            fst.write(st+"\n")
            fst.write(xlc+"\n")
            i+=1
print "Lignes:",i
fst.close()     


punct='%, %; %. %: %! %? %- %_ %" %\' %` %( %) %[ %] %{ %} %^ %= %/ %\\ %| %* %+ %% %$ %\xa3 %# %< %> %~ %& %\xab %\xbb %\xb0'
punct=punct.split(" ")
exp= "define lettre [a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q | r | s | t | u | v | w | x | y | z | A | B | C | D | E | F | G | H | I | J | K | L | M | N | O | P | Q | R | S | T | U | V | W | X | Y | Z | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9] ;\n"
exp+="read regex ["
first=True
for e in ending.onending:
    ajoute=False
    if e[0] in ['m','f','t','c']:
        ajoute=True
    w=" ".join(list(e))    
    feats="+guess"+features(ending.onending[e])
    feats=feats.replace("+","%+")
    if not first:
        sub=" | "
    else:
        sub=""
    first=False
    sub+="\nlettre+ ["+feats+" .x. "+w+"]"
    exp+=sub
    if ajoute:
        sub=" | "
        feats="%+focus "+feats
        w=w[:2]+w[2].upper()+w[3:]
        sub+="\nlettre+ ["+feats+" .x. "+w+"]"
        exp+=sub
    
for p in punct:
    sub=" | "
    sub+="\n["+p+" %+punct .x. "+p+" 0]"
    exp+=sub
exp+="];"
guess=open(r"C:\eclipse\XIPLING\guess.src","w")
guess.write(exp+"\n");
guess.write("save stack guess.fst\n");
guess.write("read spaced-text lingata.src\n");
guess.write("save stack lingvata.fst\n");
guess.close()


        

    
