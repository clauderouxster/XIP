# -*- coding: utf-8 -*-

import sys

#Exploding a structure: POS(_FEAT:X):surface^lemme^feats:nb

sys.setrecursionlimit(100000)
debugmode=False

def m_pos_0_1(fx,lreturn,tree,nb):
 if debugmode:
    print 'pos_0_1',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if x_test_ord(fx,lret,'A','Z') or x_test_ord(fx,lret,'a','z'):
   if x_test_tree(subtree):
     tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   return False
 lreturn.extend(lret)
 return True


def m_pos_0(fx,lreturn,tree,nb):
 if debugmode:
    print 'pos_0',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Loop
 subtree={}
 if m_pos_0_1(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 while not fx.eof():
   subtree={}
   if m_pos_0_1(fx,lret,subtree,nb):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
 lreturn.extend(lret)
 return True


def m_pos(fx,lreturn,tree,nb):
 if debugmode:
    print 'pos',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='pos'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_pos_0(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_att_2_3(fx,lreturn,tree,nb):
 if debugmode:
    print 'att_2_3',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if x_test_ord(fx,lret,'A','Z') or x_test_ord(fx,lret,'a','z') or x_test_ord(fx,lret,'0','9'):
   if x_test_tree(subtree):
     tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   return False
 lreturn.extend(lret)
 return True


def m_att_2(fx,lreturn,tree,nb):
 if debugmode:
    print 'att_2',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Loop
 subtree={}
 if m_att_2_3(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 while not fx.eof():
   subtree={}
   if m_att_2_3(fx,lret,subtree,nb):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
 lreturn.extend(lret)
 return True


def m_att(fx,lreturn,tree,nb):
 if debugmode:
    print 'att',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='att'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_att_2(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_val_4_5(fx,lreturn,tree,nb):
 if debugmode:
    print 'val_4_5',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if x_test_ord(fx,lret,'A','Z') or x_test_ord(fx,lret,'a','z') or x_test_ord(fx,lret,'0','9') or x_test_in(fx,lret,['+','-']):
   if x_test_tree(subtree):
     tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   return False
 lreturn.extend(lret)
 return True


def m_val_4(fx,lreturn,tree,nb):
 if debugmode:
    print 'val_4',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Loop
 subtree={}
 if m_val_4_5(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 while not fx.eof():
   subtree={}
   if m_val_4_5(fx,lret,subtree,nb):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
 lreturn.extend(lret)
 return True


def m_val(fx,lreturn,tree,nb):
 if debugmode:
    print 'val',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='val'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_val_4(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_feat_6(fx,lreturn,tree,nb):
 if debugmode:
    print 'feat_6',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'_'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if m_att(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if x_test_one(fx,lret,':'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if m_val(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 lreturn.extend(lret)
 return True


def m_feat(fx,lreturn,tree,nb):
 if debugmode:
    print 'feat',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='feat'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_feat_6(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_surface_7(fx,lreturn,tree,nb):
 if debugmode:
    print 'surface_7',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # LoopConstraint
 subtree={}
 if x_test_one(fx,lret,'^'):
     tree.pop('%NODES')
     fx.seek(pos)
     return False
 lret=[]
 subtree={}
 if x_test_any(fx,lret):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 foundsolution=False
 while not fx.eof():
   subtree={}
   if x_test_one(fx,lret,'^'):
      foundsolution=True
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
      break
   subtree={}
   if x_test_any(fx,lret):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
   subtree={}
 if not foundsolution:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 lreturn.extend(lret)
 return True


def m_surface(fx,lreturn,tree,nb):
 if debugmode:
    print 'surface',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='surface'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_surface_7(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_lemme(fx,lreturn,tree,nb):
 if debugmode:
    print 'lemme',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='lemme'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_surface_7(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_feats_9(fx,lreturn,tree,nb):
 if debugmode:
    print 'feats_9',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # LoopConstraint
 subtree={}
 if x_test_one(fx,lret,':'):
     tree.pop('%NODES')
     fx.seek(pos)
     return False
 lret=[]
 subtree={}
 if x_test_any(fx,lret):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 foundsolution=False
 while not fx.eof():
   subtree={}
   if x_test_one(fx,lret,':'):
      foundsolution=True
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
      break
   subtree={}
   if x_test_any(fx,lret):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
   subtree={}
 if not foundsolution:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 lreturn.extend(lret)
 return True


def m_feats(fx,lreturn,tree,nb):
 if debugmode:
    print 'feats',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='feats'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_feats_9(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_nb_10(fx,lreturn,tree,nb):
 if debugmode:
    print 'nb_10',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Loop
 subtree={}
 if x_test_ord(fx,lret,'0','9'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 while not fx.eof():
   subtree={}
   if x_test_ord(fx,lret,'0','9'):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
 lreturn.extend(lret)
 return True


def m_nb(fx,lreturn,tree,nb):
 if debugmode:
    print 'nb',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='nb'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_nb_10(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_structure_11_12(fx,lreturn,tree,nb):
 if debugmode:
    print 'structure_11_12',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 foundsolution=False
 while not fx.eof():
   subtree={}
   if x_test_one(fx,lret,':'):
      foundsolution=True
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
      break
   subtree={}
   if m_feat(fx,lret,subtree,nb+1):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
   subtree={}
 if not foundsolution:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 lreturn.extend(lret)
 return True


def m_structure_11(fx,lreturn,tree,nb):
 if debugmode:
    print 'structure_11',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_pos(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if m_structure_11_12(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if m_surface(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if m_lemme(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if m_feats(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if m_nb(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 lreturn.extend(lret)
 return True


def m_structure(fx,lreturn,tree,nb):
 if debugmode:
    print 'structure',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='structure'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_structure_11(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True



def x_comparelen(xl,yl):
    x=xl[0]
    y=yl[0]
    if x>y:
        return -1
    if x==y:
        return 0
    return 1


        
def x_test_string(fx,lret,sz,sub):
  pos=fx.tell()
  [lst,lstu]=fx.read(sz)
  if len(lst)<sz:
      fx.seek(pos)
      return False
  if lst==sub or lstu==sub:
      lret.append(lst)
      return True
  fx.seek(pos)
  return False

def x_test_in(fx,lret,sub):
  pos=fx.tell()
  [lst,lstu]=fx.read(1)
  if lst=="":
      fx.seek(pos)
      return False
  if lst in sub or lstu in sub:
      lret.append(lst)
      return True
  fx.seek(pos)
  return False

def x_test_one(fx,lret,onechar):
  pos=fx.tell()
  [lst,lstu]=fx.read(1)
  if lst=="":
      fx.seek(pos)
      return False
  if lst==onechar or lstu==onechar:
      lret.append(lst)
      return True
  fx.seek(pos)
  return False




def x_test_ord(fx,lret,b,g):
  pos=fx.tell()
  [lst,lstu]=fx.read(1)
  if lst=="":
     fx.seek(pos)
     return False
  #print "x_test_ord",lst[0]
  if ord(lst[0])>=ord(b) and ord(lst[0])<=ord(g):
     lret.append(lst)
     return True
  fx.seek(pos)
  return False



def x_test_any(fx,lret):
  pos=fx.tell()
  [lst,lstu]=fx.read(1)
  if lst=="":
     fx.seek(pos)
     return False
  lret.append(lst)
  return True



def x_test_tree(sub):
    if sub=={}:
       return False
    if "%NODES" in sub and sub["%NODES"]==[]:
        if "%NAME" not in sub:
            return False
        else:
            sub.pop("%NODES")
    return True



def x_displayanalysis(tree,nb):
    space="".zfill(nb).replace("0"," ")
    if "%NAME" in tree:
        name=tree["%NAME"]
        value=""
        off=[]
        if "%VALUE" in tree:
            value=tree["%VALUE"]
            off=tree["%OFFSETS"]
        print space,name,off,'"'+value+'"'
    if "%NODES" in tree:
        lst=tree["%NODES"]
        for l in lst:
            x_displayanalysis(l,nb+1)

def x_maxoffset(tree,nb):
    if "%NAME" in tree:
        if "%OFFSETS" in tree:
            off=tree["%OFFSETS"]
            if off[1]>nb[0]:
                nb[0]=off[1]
    if "%NODES" in tree:
        lst=tree["%NODES"]
        for l in lst:
            x_maxoffset(l,nb)




class readstring:
    def __init__(self,s):
        self.parcours=s
        self.pos=0
        self.length=len(s)
        self.lastchar=""

    def seek(self,p):
        self.pos=p

    def tell(self):
        return self.pos

    def peek(self):
        return s[self.pos]

    def eof(self):
        if self.pos>=self.length:
            return True
        return False
        
    def read(self,sz):
        borne=self.pos+sz
        if borne>=self.length:
            borne=self.length
        if self.eof():
            return ["",""]
        s=self.parcours[self.pos:borne]
        self.lastchar=s
        self.pos=borne
        try:
            sutf8=unicode(s).encode("utf8")
            return [s,sutf8]
        except:
            return [s,s]

class readfile:
    def __init__(self,s):
        self.name=s
        self.fichier=open(s,"rb")
        #Jump to the end of the file
        self.fichier.seek(0,2)
        self.size=self.fichier.tell()
        #Return to the beginning of the file
        self.fichier.seek(0,0)
        self.pos=0
        self.lastchar=""

    def seek(self,p):
        self.pos=p
        self.fichier.seek(p)

    def tell(self):
        return self.pos

    def peek(self):
        p=self.tell()
        c=self.fichier.read(1)
        self.seek(p)
        return c

    def eof(self):
        if self.pos>=self.size:
            return True
        return False
        
    def read(self,sz):
        borne=self.pos+sz
        if self.eof():
            return ["",""]
        s=self.fichier.read(sz)
        self.lastchar=s
        self.pos=borne
        try:
            sutf8=unicode(s).encode("utf8")
            return [s,sutf8]
        except:
            return [s,s]

    def close(self):
        self.fichier.close()

def x_parse_string(x,f,full=True):
    fx=readstring(x)
    lret=[]
    tree={}
    res=f(fx,lret,tree,0)
    if not fx.eof() and full:
        maxoff=[0]
        x_maxoffset(tree,maxoff)
        fx.seek(maxoff[0])
        print "Error:"
        print fx.read(300)[0]
        return False
    if res==True:
       x_displayanalysis(tree,0)
       return True
    return False


def x_parse_file(x,f,full=True):
    fx=readfile(x)
    lret=[]
    tree={}
    res=f(fx,lret,tree,0)
    fx.close()
    if not fx.eof() and full:
        maxoff=[0]
        x_maxoffset(tree,maxoff)
        fx.seek(maxoff[0])
        print "Error:"
        print fx.read(300)[0]
        return False
    if res==True:
       x_displayanalysis(tree,0)
       return True
    return False



def parse(x):
    fx=readstring(x)
    lret=[]
    tree={}
    res=m_structure(fx,lret,tree,0)
    if not fx.eof():
        return None
    if res==True:
        return tree
    return None


