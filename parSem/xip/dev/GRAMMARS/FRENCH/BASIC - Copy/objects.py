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

  def frere(self,n):
      if n.next!=-1:
          return self.nodes[n.next]
      return None

  def fils(self,n):
      if n.daughter!=-1:
          return self.nodes[n.daughter]
      return None
          
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
   def __str__(self):
       self.printunit()
       return ""

def xipres(i,xipunit):
   global current_parseur
   if i!=-1:
    n=XipNode(i,current_parseur)
    xipunit.insertion(n)
    xipres(n.daughter,xipunit)
    xipres(n.next,xipunit)


def addfromset(pos,hgrm,xipunit):
    nst=nodeset(hgrm,pos)
    for l in nst:
      n=XipNode(l,hgrm)
      xipunit.insertion(n)
  

def transform(i,xipresult):
    '''s=open("C:\\GestionConnaissanceInventaire\\lexiquePalissy\\repr_sans_doublon_multi_point_lemme.txt","a")'''
    "This is an implementation of the XIP procedure: transform(#1). This function is called from a XIP grammar"
    global current_parseur
    
    xp=XipNode(i)
    xipunit=XipUnit()    
    xipres(i,xipunit)   

    dep=dependencyset(current_parseur,"?")
    for i in dep:
       dp=XipDependency(i,current_parseur)
       xipunit.dependencies.append(dp)
    xipresult.insertion(xipunit)
    lex=lexicals(current_parseur)
    for l in lex:
      n=XipNode(l,current_parseur)
      xipunit.insertion(n)

def parseString(phrase,xipresult):
    "parse a sentence with the grammar 0: parse(sentence)"
    setdisplay(current_parseur,XIP_DISPLAY_LEMMA|XIP_DISPLAY_CHUNK_TREE|XIP_DISPLAY_OFFSET|XIP_DISPLAY_EXECUTION_ERROR|XIP_DISPLAY_DEPENDENCY_BY_NODE)   
    parsestring(current_parseur,phrase,xipresult)

def parseFile(nb,filename,xipresult):
    "parse a file with the grammar 0: parsefile(filename)"
    print "on parse:",filename
    setdisplay(nb,XIP_DISPLAY_NONE|XIP_DISPLAY_EXECUTION_ERROR)
    return parsefile(nb,filename,xipresult)


