"""
Python:

transform(#1,python xipresult).
"""

class XipUnit:
  def insertion(self,x):
    if (x.parent==-1):
       self.root=x
    self.nodes[x.index]=x
    
  def __init__(self):
     self.dependencies=[]
     self.nodes={}
     self.root=XipNode
     self.words = []

  def printtree(self,indent,node):
      st=""
      n=self.nodes[node]
      if n.daughter==-1:
         return
      for i in range(indent):
          st=st+" "
      st+=n.pos[0]+"("+str(n.index)+"):"+n.lemma[0]
      print st
      if (n.daughter!=-1):
         self.printtree(indent+1,n.daughter)
      if (n.next!=-1):
         self.printtree(indent,n.next)

  def printdep(self):
    for i in self.dependencies:
      st=i.name+"("
      premier=0
      for j in i.parameters:
        if premier!=0:
          st=st+","
        premier=1
        n=self.nodes[j]
        st=st+n.lemma[0]+":"+str(n.index)
      st=st+")"
      print st

      
  def getdep(self):
    return self.dependencies
      
  def getwords(self):
    """
    for i in self.words:
      print i[0]
#      print i[0][2].encode("utf-8")  
      
      depi = i[1]
      depi.extend(i[2])
      for k in depi:
        d = self.dependencies[k]
        st=d.name+"("
        premier=0
        for j in d.parameters:
          if premier!=0:
            st=st+","
          premier=1
          n=self.nodes[j]
          st=st+n.lemma[0]+":"+str(n.index)
        st=st+")"
        print st
    """    
    return self.words  
                  
  def __str__(self):
      self.printtree(0,self.root.index)
      self.printdep()
      return ""


class XipResult:
   def insertion(self,u):
      self.units.append(u)

   def __init__(self):
      self.units = []

   def printunit(self):
      for i in self.units:
          print i

   def getwords(self):
      words = []
      for i in self.units:
         words.extend(i.getwords())
      return words

   def __str__(self):
       self.printunit()
       return ""

   def getdep(self):
     dependencies = []
     for i in self.units:
       dependencies.extend(i.getdep())
     return dependencies

   def printdep(self):
     for i in self.units:
       i.printdep()

   def printtree(self):
      for i in self.units:
        i.printtree(0, i.root)

def xipres(i,xipunit):
   if i!=-1:
      n=XipNode(i)
      xipunit.insertion(n)
      xipres(n.daughter,xipunit)
      xipres(n.next,xipunit)

def transform(i,xipresult):
    "This is an implementation of the XIP procedure: transform(#1). This function is called from a XIP grammar"
    global current_parseur

    xipunit=XipUnit()    
    xipres(i,xipunit) 
    fictiveliste=fictivenodes(current_parseur)
  
    for fictif in fictiveliste:
       n=XipNode(fictif)
       xipunit.insertion(n)       
    """
    dep=dependencyset("?")
    
    for i in dep:
      dp=XipDependency(i)
      params = []
      for param in dp.parameters:
        x = XipNode(param)
        params.append((x.index, x.lemma))
      xipunit.dependencies.append((dp.name, params))
    """
    words = lexicals(current_parseur)

    dep1 = {}
    dep2 = {}
    for i in range(len(words)):
      dep1i = dependencyonfirstnode(current_parseur,words[i])
      dep1[i] = dep1i
      dep2i = dependencyonsecondnode(current_parseur,words[i])
      dep2[i] = dep2i
      x = XipNode(words[i])
      word = (x.index, x.lemma, x.surface, x.pos)
      xipunit.words.append(word)
    for i in  range(len(words)):
      for j in range(len(words)):
        for d1 in dep1[i]:
          for d2 in dep2[j]:
            if d1==d2:
              dp=XipDependency(d1)
#              x1 = XipNode(i)
#              word1 = (x1.index, x1.lemma, x1.surface, x1.pos)
#              x1 = XipNode(j)
#              word2 = (x1.index, x1.lemma, x1.surface, x1.pos)
              xipunit.dependencies.append((dp.name + str(dp.features),i, j))
    xipresult.insertion(xipunit)

def parse(grammar,phrase,xipresult):
    "parse a sentence with the grammar 0: parse(sentence)"
    return parsestring(grammar,phrase,xipresult)

def parsefile(filename,xipresult):
    "parse a file with the grammar 0: parsefile(filename)"
    setdisplay(0,XIP_DISPLAY_LEMMA|XIP_DISPLAY_CHUNK_TREE|XIP_DISPLAY_OFFSET|XIP_DISPLAY_EXECUTION_ERROR)
    print parsefile(0,filename,xipresult)

