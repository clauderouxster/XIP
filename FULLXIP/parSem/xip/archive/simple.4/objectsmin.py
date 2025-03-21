"""
python:

three($1,#1,#2,#3,python echange),
two($1,#1,#2,python echange),
single($1,#1,python echange).

"""


def conc(n):
    feat=n.features[1:-1].split(",")
    s=""
    for f in feat:
        ps=f.find(":")
        nm=f[:ps]
        if nm=="":
            continue
        if nm=="STARTBIS":
            nm="START"
        if nm in ["START","PROPER","PL","FUT","PAST","REL","PARTPAS"]:
            if "_"+nm not in s:
                s+="_"+nm
    return n.lemma+s
    
def extract(features):
    feat=features[1:-1].split(",")
    s=[]
    for f in feat:
        ps=f.find(":")
        s.append(f[:ps])
    return s

class adependency:
    def __init__(self,dep,n1,n2,n3):
        self.name=dep.name
        self.features=extract(dep.features)
        self.p1=[n1.pos,conc(n1),str(n1.index),n1.surface]
        if n2==None:
            self.p2=None
        else:
            self.p2=[n2.pos,conc(n2),str(n2.index),n2.surface]
            if n3==None:
                self.p3=None
            else:
                self.p3=[n3.pos,conc(n3),str(n3.index),n3.surface]

        
class dependances:
    def __init__(self):
        self.deps={}
        self.maxid=-1

    def add(self,dep,n1,n2,n3):
        if self.maxid<n1.index:
            self.maxid=n1.index
        if n2!=None and self.maxid<n2.index:
            self.maxid=n2.index
        if n3!=None and self.maxid<n3.index:
            self.maxid=n3.index
            
        d=adependency(dep,n1,n2,n3)
        try:
            self.deps[n1.left][n2.left]=d
        except:
            self.deps[n1.left]={n2.left:d}

def three(d,par1,par2,par3,echange):
    dep=XipDependency(d)
    n1=XipNode(par1)
    n2=XipNode(par2)
    n3=XipNode(par3)
    echange.add(dep,n1,n2,n3)    


def two(d,par1,par2,echange):
    dep=XipDependency(d)
    n1=XipNode(par1)
    n2=XipNode(par2)
    echange.add(dep,n1,n2,None)    

def single(d,par1,echange):
    dep=XipDependency(d)
    if dep.name=="CLOSEDNP":
        return
    n1=XipNode(par1)
    echange.add(dep,n1,None,None)    

    
