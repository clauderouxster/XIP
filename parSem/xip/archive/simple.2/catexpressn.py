# -*- coding: utf-8 -*-

import sys

#Exploding an expression: cat(nnn),cat(nnn)
#x_parse_string("cattt(dsadkl)",m_element)
#x_parse_string("cattt(#1)",m_element)

sys.setrecursionlimit(100000)
debugmode=False

def m_var_0_1(fx,lreturn,tree,nb):
 if debugmode:
    print 'var_0_1',fx.lastchar
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


def m_var_0(fx,lreturn,tree,nb):
 if debugmode:
    print 'var_0',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'#'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if m_var_0_1(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 lreturn.extend(lret)
 return True


def m_var(fx,lreturn,tree,nb):
 if debugmode:
    print 'var',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='var'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_var_0(fx,lret,subtree,nb):
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


def m_cat_2_3(fx,lreturn,tree,nb):
 if debugmode:
    print 'cat_2_3',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Optional
 subtree={}
 if x_test_one(fx,lret,'^'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
   lret=[]
 lreturn.extend(lret)
 return True


def m_cat_2_4_5(fx,lreturn,tree,nb):
 if debugmode:
    print 'cat_2_4_5',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if x_test_ord(fx,lret,'a','z') or x_test_ord(fx,lret,'A','Z'):
   if x_test_tree(subtree):
     tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   return False
 lreturn.extend(lret)
 return True


def m_cat_2_4(fx,lreturn,tree,nb):
 if debugmode:
    print 'cat_2_4',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Loop
 subtree={}
 if m_cat_2_4_5(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 while not fx.eof():
   subtree={}
   if m_cat_2_4_5(fx,lret,subtree,nb):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
 lreturn.extend(lret)
 return True


def m_cat_2(fx,lreturn,tree,nb):
 if debugmode:
    print 'cat_2',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_cat_2_3(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if m_cat_2_4(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 lreturn.extend(lret)
 return True


def m_cat(fx,lreturn,tree,nb):
 if debugmode:
    print 'cat',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='cat'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_cat_2(fx,lret,subtree,nb):
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


def m_word_6(fx,lreturn,tree,nb):
 if debugmode:
    print 'word_6',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # LoopConstraint
 subtree={}
 if x_test_one(fx,lret,')'):
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
   if x_test_one(fx,lret,')'):
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


def m_word(fx,lreturn,tree,nb):
 if debugmode:
    print 'word',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='word'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_word_6(fx,lret,subtree,nb):
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


def m_element_7_8_9(fx,lreturn,tree,nb):
 if debugmode:
    print 'element_7_8_9',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_var(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if x_test_one(fx,lret,')'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 lreturn.extend(lret)
 return True


def m_element_7_8(fx,lreturn,tree,nb):
 if debugmode:
    print 'element_7_8',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if m_element_7_8_9(fx,lret,subtree,nb) or m_word(fx,lret,subtree,nb+1):
   if x_test_tree(subtree):
     tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   return False
 lreturn.extend(lret)
 return True


def m_element_7(fx,lreturn,tree,nb):
 if debugmode:
    print 'element_7',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_cat(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'('):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if m_element_7_8(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 lreturn.extend(lret)
 return True


def m_element(fx,lreturn,tree,nb):
 if debugmode:
    print 'element',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='element'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_element_7(fx,lret,subtree,nb):
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


def m_list_10(fx,lreturn,tree,nb):
 if debugmode:
    print 'list_10',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,','):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if m_element(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 lreturn.extend(lret)
 return True


def m_list(fx,lreturn,tree,nb):
 if debugmode:
    print 'list',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='list'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_list_10(fx,lret,subtree,nb):
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


def m_parsecat_11_12(fx,lreturn,tree,nb):
 if debugmode:
    print 'parsecat_11_12',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 while not fx.eof():
   subtree={}
   if m_list(fx,lret,subtree,nb+1):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
 lreturn.extend(lret)
 return True


def m_parsecat_11(fx,lreturn,tree,nb):
 if debugmode:
    print 'parsecat_11',fx.lastchar
 lret=[]
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_element(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 # Sequence
 subtree={}
 if m_parsecat_11_12(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    return False
 lreturn.extend(lret)
 return True


def m_parsecat(fx,lreturn,tree,nb):
 if debugmode:
    print 'parsecat',fx.lastchar
 lret=[]
 pos=fx.tell()
 tree['%NAME']='parsecat'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_parsecat_11(fx,lret,subtree,nb):
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


