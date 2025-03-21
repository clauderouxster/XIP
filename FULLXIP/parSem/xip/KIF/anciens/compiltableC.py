import rulen
import sys

debugmode=False
classname=""

def tl(x):
    return (type(x)==type([]))
def td(x):
    return (type(x)==type({}))
        

def applyrule(r,tree):
    lreturn=[]
    s=rulen.readstring(r)
    return rulen.m_regle(s,lreturn,tree,0)


def explore(tree,nb,sub,symbols):
    if "%NODES" in tree:
        lst=tree["%NODES"]
        for l in lst:
            treeanalysis(l,nb,sub,symbols)

def insertion(struct,sub,operator):
    struct.insert(0,operator)
    if sub[0][0] in ["&","|","()","{}","^",";"]:
        if sub[0][0]==operator:
            struct.extend(sub[0][1:])
        else:
            struct.extend(sub)
    else:
        struct.extend(sub[0])
    


def treeanalysis(tree,nb,struct,symbols):
    #space="".zfill(nb).replace("0"," ")
    #Nous nous occupons que des categories suivantes: CROCHET, UNION, STRING, CHIFFRE, MOT
    if "%NAME" in tree:        
        name=tree["%NAME"]
        value=tree["%VALUE"]
        #print space,name,value
        if name=="regle":
            rulename=tree["%NODES"][0]["%NODES"][0]["%VALUE"]
            symbols.append(rulename)
        if name in ["string","point","interval","mot","digits","symbole"]:
            element={name:value}
            struct.append(element)
        elif name=="negation":
            #Constraint on applying the ANY operator on the next element
            sub=[]
            explore(tree,nb+1,sub,symbols)
            struct.append(["~",sub])
        elif name=="kleene":
            sub=[]
            explore(tree,nb+1,sub,symbols)
            if sub==[]:
                struct[-1]=[value,struct[-1]]
            else:
                struct[-1]=[value[0],struct[-1],sub]
        elif name=="liste":
            sub=[]
            explore(tree,nb+1,sub,symbols)
            struct.append(sub)
        elif name=="contexte":
            sub=[]
            explore(tree,nb+1,sub,symbols)
            sub.insert(0,"{}")
            struct.append(sub)
        elif name=="optionel":
            sub=[]
            explore(tree,nb+1,sub,symbols)
            sub.insert(0,"()")
            struct.append(sub)
        elif name=="union":
            sub=[]
            explore(tree,nb+1,sub,symbols)
            insertion(struct,sub,"|")
        elif name=="exclusif":
            sub=[]
            explore(tree,nb+1,sub,symbols)
            insertion(struct,sub,"^")
        elif name=="sequence":
            sub=[]
            explore(tree,nb+1,sub,symbols)
            insertion(struct,sub,"&")
        elif name=="coupechoix":
            sub=[]
            explore(tree,nb+1,sub,symbols)
            adigit=sub[0]["digits"]
            sub=sub[1:]
            insertion(struct,sub,";")
            struct[1]["$"]=adigit
        else:
            explore(tree,nb+1,struct,symbols)
    else:
        explore(tree,nb+1,struct,symbols)
    

def displayanalysis(tree,nb):
    space="".zfill(nb).replace("0"," ")
    if "%NAME" in tree:
        name=tree["%NAME"]
        value=tree["%VALUE"]
        print space,name,value
    if "%NODES" in tree:
        lst=tree["%NODES"]
        for l in lst:
            displayanalysis(l,nb+1)
           
        
def compiling(filename):
    f=open(filename,"r")
    erreur=[]
    rules={}
    comments=[]
    kept=[]
    removed=[]
    errormessages=[]
    callpersonal=[]
    symbols={}
    for e in f:
        l=e.strip()
        if l=="":
            continue
        if l[0]=='#':
            comments.append(l)
        elif l[0]=="+":
            kpt=l[1:].strip().split(",")
            for k in kpt:
                kept.append(k.strip())
        elif l[0]=="%":
            kpt=l[1:].strip().split(",")
            for k in kpt:
                callpersonal.append(k.strip())
        elif l[0]=="!":
            kpt=l[1:].strip().split(",")
            for k in kpt:
                removed.append(k.strip())
        elif l[0]=="*":
            kpt=l[1:].strip().split("=")
            errormessages.append([kpt[0],kpt[1]])
        else:
            tree={}
            symb=[]
            if not applyrule(l,tree):
                erreur.append(l)
            else:
                struct=[{"%":l}]
                treeanalysis(tree,0,struct,symb)
                #displayanalysis(tree,0)
                rules[symb[0]]=struct
    for r in rules:
        rules[r]=rules[r][2:][0]
        print r,rules[r]
        print
    f.close()

try:
    chemin=sys.argv[1]
except:
    chemin=r"W:\users\Roux\xip\parSem\xip\KIF\kif"
compiling(chemin)
