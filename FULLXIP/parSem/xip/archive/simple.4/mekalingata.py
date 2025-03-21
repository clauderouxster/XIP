# -*- coding: utf-8 -*-
######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: mekalingata.py
#Comment: This file implements a user graphical interface
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

import time
import datetime
import threading
import sys
import grammarcodes
import xips
import selecttrans

try:
    language=sys.argv[1]
except:
    language="english"

grammarcodes.currentlanguage=language

import Dialog
import lingata

import grammar
import numbers
import semantics
import sense
import sensedata
import genemin
from Tkinter import *
from Tkconstants import *
import random
import ending
import translation
import chunker

#We do not want to display any information during generation
genemin.debug=False


sys.setcheckinterval(1)

grammarrules="""

•    The adv ending is: no

•    The conjunction ending is: nu
         A conjunction is an element which always independent from the other
         words such as: yesterday=hieranu
         
•    Termination Semantics: a,u,e,o,i
o   –a is the regular ending.py    (pronounciation: "a" as in "par")
o   –u is the numeral ending       (pronounciation: "u" as in "cool")
o   -e is the location ending      (pronounciation: "é" as in "step")
o   -o is the proper name ending   (pronounciation: "o" as in "no")
o   -i is the temporal ending      (pronounciation: "ee" as in "steep")

•    Case Marker Semantics: n,s,l
o   raw form is the nominative
o   -n is the accusative
o   -s is the genetive
o   -d is the dative

•    The determiner ending are: la,lu,le,lo,li
o   –la(n,s,d) is the regular ending
o   –lu(n,s,d) is the ending for numeral determiners
o   -le(n,s,d) is the ending for location.
o   -lo(n,s,d) is the ending for proper names.
o   -li(n,s,d) is the ending for temporal expressions

•    The noun endings are ta,tu,te,to,ti.
o   -ta(n,s,d) is the regular ending of common words. 
o   -tu(n,s,d) is the ending for numbers.
o   -te(n,s,d) is the ending for location.
o   -to(n,s,d) is the ending for proper names.
o   -ti(n,s,d) is the ending for temporal expressions

•    The adjective endings are: ca,cu,ce,co,ci (pronounciation: "ch" as in "shy").
o   –ca(n,s,d) is the regular ending for adjectives
o   –cu(n,s,d) is the ending for number adjectives.
o   -ce(n,s,d) is the ending for location.
o   -co(n,s,d) is the ending for proper names.
o   -ci(n,s,d) is the ending for temporal expressions

•    The tense markers are: g,p,f,d,ga
o   –g is the present tense.
o   –p is the past tense.
o   –f is the future tense.
o   –d is the conditional tense.
o   –gas is the infinitive form.
o   –gad is the prepositional form.

•    The verb ending are: ia,ie
o   –ia(g,p,f,d,gas,gad) is the regular verb ending.
o   –ie(g,p,f,d,gas,gad) is the ending for verbs in a clause.

•    The preposition endings are: ar,ars, i(n,s,d)ars
o   –ar is for verbal complement attachment.
o   –ars is for nominal phrase attachment with a nominative (genetive ending).
o   –inars is for nominal phrase attachment with an accusative(genetive ending).
o   –idars is for nominal phrase attachment with a dative noun(genetive ending).
o   –isars is for nominal phrase attachment with a genetive noun(genetive ending).

•    The pronoun endings are: er,ern,ers,erd
o   –er  is a nominative in the clause
o   –ern is an accusative in the clause
o   –ers is a genetive in the clause
o   –erd is a dative in the clause

•    The pronoun antecedent case infixes are: er,iner,iser,ider
        -  These infixes help disambiguate which phrase is the antecedent of that pronoun
o   –er  the antecedent is the nominative phrase in the sentence
o   –iner the antecedent is the accusative phrase in the sentence
o   –iser the antecedent is the previous genetive phrase in the sentence
o   –ider the antecedent is the previous dative phrase in the sentence
•    Example:
         uder: for
         udiner: for, but with an accusative antecedent
         
•    Some clause pronouns.
        For this pronoun, the "i" is not inserted as for other pronouns, as the radical is limited to one consonant
        The basic pronoun is "K"
        - with two variants: "F" and "M", to add a gender connotation for the antecedent
        - followed by one of the semantic markers (a,o,e,i,u)
        - followed by one of the case markers (n,s) used to detect the antecedent
o   ker:   that_subject      (k+er)      introduce a clause which already has a subject
o   kaer:  that_subject      (k+a+er)    also  faer,maer
o   kaern: that_object       (k+a+er+n)  also  faern,maern
o   kaers: whose             (k+a+er+s)  also  faers,maers
o   koer:  that_subject      (k+o+er)    also  foer,moer
o   koern: that_object       (k+o+er+n)  also  foern,moern
o   koers: whom              (k+o+er+s)  also  foers,moers
o   keer:  where             (k+e+er)
o   kier:  when              (k+i+er)
o   kuer:  how_many,how_much (k+u+er)

•    Examples:
         fonern:  The antecedent should have the following property: proper name, female, genetive
                  The pronoun itself will be a direct object of the verb in the clause
         Huma Lisaton foners huta lingaieg vidaiag.
         The woman watches Lisa whom the man talks.

•    Conjunction Suffixes: que,quo,qua,ne
o   -que indicates a conjunction (AND)
o   -quo indicates a disjunction (OR)
o   -qua introduces a question on that word
o   –ne  introduces a negation

•    Particle Order: que|quo,qua,ne
o   Hufa itaiagne               The woman does not eat
o   Hufa itaiagqua               Does the woman eat?
o   Hufa drinkiag itaiagque     The woman drinks and eats
o   Hufa drinkiag itaiagquene   The woman drinks and does not eat
o   Rete Reti retaque fliiag    Place Time and things fly
"""+str(numbers.numbers)

def islist(s):
   if type(s)==type([]):
      return True
   return False

trucmot=""
   
class App:

   def __init__(self, master):

#     self.colors=["black","gray75","gray50","white","darkred","red","DarkMagenta","magenta",
#                 "green4","green","Gold4","yellow","navy","blue","turquoise4","cyan"]

     self.colors=[]
##     red=["00","10","20","30","40","50","60","70","80","90","A0","B0","C0","D0","E0","F0"]
##     green=["00","10","20","30","40","50","60","70","80","90","A0","B0","C0","D0","E0","F0"]
##     blue=["00","10","20","30","40","50","60","70","80","90","A0","B0","C0","D0","E0","F0"]

     red=["00","FF","A0"]
     green=["00","A0","FF"]
     blue=["00","A0","FF"]
     for r in red:
        for g in green:
           for b in blue:
              s="#"+r+g+b
              self.colors.append(s)

     self.colors.sort()
     self.previouslinks={} 
     self.links={}
     self.debut=True
     self.handle=-1
     self.maitre=master
     self.associatedlanguage=None
     self.frame = Frame(master)
     self.frame.pack()
     self.all=False
     self.lastsentence=""
     self.lastgraphe=None
     self.lastlexicals=None
     self.generationgraphes=[]
     self.pronouns={}
     self.semanticwords={}
     self.parsed=False
     try:
         self.generationgrapheslist=selecttrans.translationgraphes
         self.ontologies=selecttrans.ontologies
         self.ontologyheads=selecttrans.ontologyheads
     except:
         self.generationgrapheslist=[]
         self.ontologies={}
         self.ontologyheads={}
     
     self.ontologyheads['re']=True
     self.globalsentence=[]
     
     #We load our grammars
     self.traduire=genemin.generation(genemin.cheminxip)
     translation.load(language)
     #self.frameboitebis = Frame(master)
     #self.frameboitebis.pack()

     #self.frameboite = Frame(master)
     #self.frameboite.pack()
     
     self.savetheinput=True
     self.frameinput = Frame(master)
     self.frameinput.pack()

     self.frametext = Frame(master)
     self.frametext.pack()

     self.lesracs=[]
     self.keepinterpretation=[]

     self.gen_text=""
     self.generationraw = Button(self.frameinput, text="Raw", fg="red", command=self.generation)
     self.generationraw.pack(side=LEFT)
     self.generation = Button(self.frameinput, text="Straight into", fg="red", command=self.straight)
     self.generation.pack(side=LEFT)
     self.generation = Button(self.frameinput, text="French into Lingveta", fg="red", command=self.generationfull)
     self.generation.pack(side=LEFT)

     self.trysentence = Button(self.frameinput, text="Analyze Output", fg="red", command=self.trythesentence)
     self.trysentence.pack(side=LEFT)
     self.getlegraph = Button(self.frameinput, text="Equivalence", fg="red", command=self.getlocalgraph)
     self.getlegraph.pack(side=LEFT)
     self.keepsentence = Button(self.frameinput, text="Keep", fg="red", command=self.keepselection)
     self.keepsentence.pack(side=LEFT)
     self.lstant=["ANT_NOM","ANT_ACC","ANT_GEN","ANT_DAT"]
  
     self.questreset = Button(self.frameinput, text="Reset", fg="red", command=self.reset)
     self.questreset.pack(side=LEFT)

     self.questresetsem = Button(self.frameinput, text="Semantics", fg="red", command=self.semanticfilter)
     self.questresetsem.pack(side=LEFT)
     self.questresetsemsave = Button(self.frameinput, text="Store Semantics", fg="red", command=self.storesemantics)
     self.questresetsemsave.pack(side=LEFT)

     self.entreequ=Entry(self.frameinput)
     self.entreequ.pack(side=LEFT)

     self.entreefr=Entry(self.frameinput)
     self.entreepar=Entry(self.frameinput)
     self.entreefr.pack(side=LEFT)
     self.entreepar.pack(side=LEFT)

     self.dicoparta={} 
     police = ('Courier', 10, 'normal')
     self.textinput = Text(self.frametext, width=200,height=10, font=police,background='white',spacing1=1, spacing2=1, tabs='24',highlightbackground="black")
     self.textinput.bind("<KeyRelease-space>", self.interception)
     self.textinput.bind("<KeyRelease-.>", self.interception)
     self.textinput.bind("<KeyRelease-#>", self.translate)
     self.frametranslation = Frame(master)
     self.frametranslation.pack()

     self.texttrans = Text(self.frametranslation, width=200,height=10, font=police,foreground='black',background='white',spacing1=1, spacing2=1, tabs='24',highlightbackground="black")

     self.quest3 = Button(self.frame, text="Search", fg="blue", command=self.search)
     self.quest3.pack(side=LEFT)

     police = ('Courier', 10, 'normal')
     self.text = Text(master, height=13, font=police,background='white',spacing1=1, spacing2=1, tabs='24',highlightbackground="black")

     self.textinput.pack(side=TOP, expand=YES, fill=BOTH)
     self.texttrans.pack(side=BOTTOM, expand=YES, fill=BOTH)
     self.text.pack(side=LEFT, expand=YES, fill=BOTH)

     self.insertgraph()

     self.lesmots=[]
     self.roots={}
     
     self.loading=False
     self.lafin=False

     self.quest6 = Button(self.frame, text="Analyze", fg="black", command=self.partiag)
     self.quest6.pack(side=LEFT)

     self.quest7 = Button(self.frame, text="Infer", fg="black", command=self.infer)
     self.quest7.pack(side=LEFT)

     self.tokenize = Button(self.frame, text="Tokenize", fg="black", command=self.tokenize)
     self.tokenize.pack(side=LEFT)

     self.quest4 = Button(self.frame, text="Rules", fg="blue", command=self.displayrules)
     self.quest4.pack(side=LEFT)

     self.reference = Button(self.frame, text="Store reference", fg="blue", command=self.storereference)
     self.reference.pack(side=LEFT)

     self.comparereference = Button(self.frame, text="Compare with reference", fg="blue", command=self.comparereference)
     self.comparereference.pack(side=LEFT)

     self.reloadgrammar = Button(self.frame, text="Reload", fg="blue", command=self.reloadgrammar)
     self.reloadgrammar.pack(side=LEFT)

     self.button = Button(self.frame, text="Quit", fg="red", command=self.quitting)
     self.button.pack(side=LEFT)


##     scrollbarrac = Scrollbar(self.frameboitebis, orient=VERTICAL)
##     self.rootMekaBox=Listbox(self.frameboitebis, width=50, selectmode=SINGLE,yscrollcommand=scrollbarrac.set)
##     self.rootFrenchBox=Listbox(self.frameboitebis, width=50, selectmode=SINGLE,yscrollcommand=scrollbarrac.set)
##     scrollbarrac.config(command=self.yviewb)
##     scrollbarrac.pack(side=RIGHT, fill=Y)        
##     self.rootMekaBox.pack(side=LEFT)
##     self.rootFrenchBox.pack(side=LEFT)


##     scrollbar = Scrollbar(self.frameboite, orient=VERTICAL)
##     self.wordBox=Listbox(self.frameboite, width=50, selectmode=SINGLE,yscrollcommand=scrollbar.set)
##     self.mekaBox=Listbox(self.frameboite, width=50, selectmode=SINGLE,yscrollcommand=scrollbar.set)
##     scrollbar.config(command=self.yview)
##     scrollbar.pack(side=RIGHT, fill=Y)        
##     self.wordBox.pack(side=LEFT, fill=BOTH, expand=1)
##     self.mekaBox.pack(side=LEFT, fill=BOTH, expand=1)

     self.loadvocabulary()
     self.lastanalysis=None
     self.lastgraphes=None
     self.loadinput()
     sense.buildsenses(self.dicoparta,self.roots,grammarcodes.currentlanguage)
     self.spaces=[]
     self.lstcoord=[]

     self.gretan={}
     generated=[]
     sentence=[]
     lexicals=[]
     variables={"#1":"reta","#2":"retan"}
     grammar.parse("1_reta 2_retan beiag",sentence,lexicals,self.dicoparta,self.roots,self.gretan,True,generated,grammarcodes.currentlanguage,None)
        
     self.gretan['VARIABLES']=variables
     self.gretan['__idx__']=0

     self.grecan={}
     generated=[]
     sentence=[]
     lexicals=[]
     variables={"#1":"reca","#2":"recan"}
     grammar.parse("1_reca 2_recan beiag",sentence,lexicals,self.dicoparta,self.roots,self.grecan,True,generated,grammarcodes.currentlanguage,None)
     self.grecan['VARIABLES']=variables
     self.grecan['__idx__']=1
     #We initialize the internal translation system
     self.traduire.buildidx(self.dicoparta)
     self.initgenerationgraphes()
     self.buildfromending()

########################################################################################
###A space or a "." automatically triggers an analysis of the sentence
   
   def buildfromending(self):
       self.featurestoending={}
       self.featureslst={}
       for i in ending.onending:
           lstfeat=ending.onending[i].keys()
           lstfeat.sort()
           s=""
           for l in lstfeat:
               if l in ["basis","root","suffix"]:
                   continue
               self.featureslst[l]=True
               s+="_"+l+"@"+ending.onending[i][l]
           self.featurestoending[s]=i
            
   def getending(self,w):
       lstfeat=w.keys()
       lstfeat.sort()
       s=""
       for l in lstfeat:
           if l in self.featureslst:
               s+="_"+l+"@"+w[l]
       try:
           return self.featurestoending[s]
       except:
           return ""
               
   def interception(self,*args):
       self.partiag()

   def translate(self,*args):
       #we must first get the word with the "#"
      bg=str(self.textinput.index(INSERT))+"linestart"
      ed=str(self.textinput.index(INSERT))+"lineend"
      letext=self.textinput.get(bg,ed)
      #The end of the string should contain our word, we split along the " "
      wrds=letext.split()
      for w in wrds:
         if w[-1]=="#":
            #we look for this word in our
            lem=w[:-1]            
            self.lookfor(lem,"")


   def initfourtimes(self,ny,nx,vx,vy):
      #We prepare n*4 elements, which should circle in a square
      #n elements down
      for i in range(0,ny):
         self.spaces.append([vx,vy])
         vy+=50
         vx+=2
         
      #n elements right
      for i in range(0,nx):
         self.spaces.append([vx,vy])
         vx+=80
         vy+=10

      #n elements up
      for i in range(0,ny):
         self.spaces.append([vx,vy])
         vy-=50
         vx-=2

      #n elements left
      for i in range(0,nx):
         self.spaces.append([vx,vy])
         vx-=80
         vy-=10
         
      return [vx,vy]
   
   def initspaces(self,n,vx,vy):
      self.spaces=[]
      self.initfourtimes(n+2,n,vx,vy)

   def increasespaces(self):
      l=len(self.spaces)
      self.spaces.append([l*100+30,100-l*20])
      
########################################################################################
###GRAPH PLOTTING
   def insertgraph(self):
      self.maxgx=600
      self.maxgy=1600
      master=self.maitre
      self.toavoid=[]
      self.incx=0
      self.incy=0      
      framegraphic = Frame(master, bd=3, relief=SUNKEN)

      framegraphic.grid_rowconfigure(0, weight=1)
      framegraphic.grid_columnconfigure(0, weight=1)

      xscrollbar = Scrollbar(framegraphic, orient=HORIZONTAL)
      xscrollbar.grid(row=1, column=0, sticky="ew")

      yscrollbar = Scrollbar(framegraphic,orient=VERTICAL)
      yscrollbar.grid(row=0, column=1, sticky="ns")

      self.graphiczone=Canvas(framegraphic,bd=0,width=self.maxgx, height=self.maxgy)

      xscrollbar.config(command=self.xviewg)
      yscrollbar.config(command=self.yviewg)

      self.graphiczone.config(width=self.maxgx, height=self.maxgy,
                             scrollregion=(0,0,self.maxgx*2,self.maxgy),
                             xscrollcommand=xscrollbar.set,
                             yscrollcommand=yscrollbar.set)

      self.selectedelement=None
      self.graphiczone.bind("<B1-Motion>",self.movingbutton)
      self.graphiczone.bind("<Button-1>",self.buttonpressed)
      self.graphiczone.bind("<Double-Button-1>",self.buttonremoved)
      self.graphiczone.bind("<ButtonRelease>",self.redrawbutton)
      
##      xscrollbar.pack(side=BOTTOM,fill=X,ipadx=400)
##      yscrollbar.pack(side=RIGHT,fill=Y,ipady=400)
      xscrollbar.pack(side=BOTTOM,fill=X,expand=True)
      yscrollbar.pack(side=RIGHT,fill=Y,expand=True)
      self.graphiczone.pack()
      framegraphic.pack(side=RIGHT)      

########################################################################################
###We keep a track of each new node. If a node has already been created, then we return it
   def findelement(self,args):
      px=args.x+self.incx
      py=args.y+self.incy
      for li in self.links:
         if li in self.toavoid:
            continue
         l=self.links[li]
         if l[0]<px and px<l[2] and l[1]<py and py<l[3]:
            return li,[l[0],l[1]]
      return None

########################################################################################
###When the left mouse button is pressed, we detect which concept was selected.      
   def buttonpressed(self,args):
      #we search for our selected element
      self.selectedelement=None
      res=self.findelement(args)
      if res!=None:
         self.selectedelement,self.currentposition=res

########################################################################################
###When the left button is pressed twice, we remove it from drawn graph nodes
   def buttonremoved(self,args):
      if self.selectedelement==None:
         return
      self.toavoid.append(self.selectedelement)
      self.drawing()
      self.selectedelement=None
########################################################################################
###When moving around, we draw it again at the new position 
   def movingbutton(self,args):
      if self.selectedelement==None:
         return
      self.drawelement(self.selectedelement,args.x+self.incx,args.y+self.incy)

########################################################################################
###The button is released. We redraw the whole graph      
   def redrawbutton(self,args):      
      try:
         px=args.x+self.incx
         py=args.y+self.incy
         l=self.links[self.selectedelement]
         dx=l[2]-l[0]
         dy=l[3]-l[1]
         l=(px,py,px+dx,py+dy)
         self.links[self.selectedelement]=l
         self.drawing()
      except:
         pass
      
########################################################################################
###We compute the position of each element
   def computeelement(self,element,coord):
      xleft,yleft=coord
      k=self.graphiczone.create_text(xleft,yleft,text=element,anchor="nw")
      co=self.graphiczone.bbox(k)
      circle=self.graphiczone.create_oval(xleft-5,yleft-5,co[2]+5,co[3]+5)
      co=self.graphiczone.bbox(circle)
      self.links[element]=co
      return co
      
   def clearelement(self,element,xleft,yleft):
      k=self.graphiczone.create_text(xleft,yleft,text=element,anchor="nw",fill=self.graphiczone["background"])
      co=self.graphiczone.bbox(k)
      circle=self.graphiczone.create_oval(xleft-5,yleft-5,co[2]+5,co[3]+5,fill=self.graphiczone["background"])

   def drawelement(self,element,xleft,yleft):
      k=self.graphiczone.create_text(xleft,yleft,text=element,anchor="nw")
      co=self.graphiczone.bbox(k)
      circle=self.graphiczone.create_oval(xleft-5,yleft-5,co[2]+5,co[3]+5)


########################################################################################
### Recursively drawing each element
   def recursivedraw(self,lst,coordxy,hub,consumed):
      if len(lst)==0:
         return
      if lst[0]=="ARG":
         lst=lst[1:-1]
      #First we want to draw our element on the right
      crecxy=[coordxy[0]+100,coordxy[1]]
      for element in lst:
         if element==None:
            continue
         if islist(element):
            for e in element:
               if e in consumed:
                  continue
               consumed.append(e)
               ps=crecxy[1]
               if e in self.lastanalysis.keys():
                  self.recursivedraw(self.lastanalysis[e],crecxy,hub,consumed)
               if e not in hub:
                  if ps==crecxy[1]:
                     crecxy[1]+=50
                     cd=crecxy
                  else:
                     cd=[crecxy[0],ps+(crecxy[1]-ps)/2]
                  hub.append(e)                  
                  self.computeelement(e,crecxy)
         else:
            ps=crecxy[1]
            ls=[]
            if element not in consumed:
               consumed.append(element)
               if element in self.lastanalysis.keys():
                  ls=self.lastanalysis[element]
                  self.recursivedraw(ls,crecxy,hub,consumed)
               if element not in hub:
                  hub.append(element)
                  if len(ls)==1:
                     cd=crecxy
                  else:
                     cd=[crecxy[0],ps+(crecxy[1]-ps)/2]
                  self.computeelement(element,cd)
                  crecxy[1]+=50
      coordxy[1]=crecxy[1]
########################################################################################
### Recursively getting each element
   def addtograph(self,lst,roots,consumed):
      if len(lst)==0:
         return
      if lst[0]=="ARG":
         lst=lst[1:-1]
      for element in lst:
         if element==None:
            continue
         if islist(element):
            for e in element:
               if e in self.lastanalysis.keys():
                  if e in consumed:
                     continue
                  consumed.append(e)
                  self.addtograph(self.lastanalysis[e],roots,consumed)
                  if e in roots:
                     roots.remove(e)
         elif element in self.lastanalysis.keys():
               if element not in consumed:
                  consumed.append(element)
                  self.addtograph(self.lastanalysis[element],roots,consumed)
                  if element in roots:
                     roots.remove(element)

########################################################################################
###We compute the position of each element from the graph      
      
   def computexy(self):
      if self.lastanalysis==None:
         return None

      self.toavoid=["ARG"]
      self.selectedelement=None
      self.previouslinks=self.links
      self.links={}
      self.coords={}
      #First we detect the hubs
      #A hub is an element, which is not a key
      hub=[]

      posinspace=[0]
      coord=[100,100]
      roots=self.lastanalysis.keys()[:]
      consumed=[]
      for i in self.lastanalysis.keys():
         lst=self.lastanalysis[i]
         self.addtograph(lst,roots,consumed)
      for i in roots:
         lst=self.lastanalysis[i]
         add=True
         if i not in hub:
            ps=coord[1]
            consumed=[]
            self.recursivedraw(lst,coord,hub,consumed)
            #we then position our structure in the middle
            cd=[coord[0],ps+(coord[1]-ps)/2]
            self.computeelement(i,cd)


      #We then check, whether the new links are equivalent to the previous ones
      #If a link has been moved previously by hand, then we can reuse its position
      if self.links.keys()==self.previouslinks.keys():
         self.links=self.previouslinks
      

   #1 is middle
   #2 is left
   #3 is right
   def middle(self,xdl,xdr):
      xl=float(xdl)
      xr=float(xdr)
      X=xl+(xr-xl)/2
      return int(X)-4
   
   def valuexin(self,xl,xr, Xl,Xr):
      return [self.middle(xl,xr),self.middle(Xl,Xr)]

   def valueXin(self,xl,xr, Xl,Xr):
      #base on left
      vx=6
      vX=6
      if Xr<xl:
         return [xl-vx,Xr-vX]
      if xr<Xl:
         return [xr-vx,Xl-vX]
      if xr>Xl and xr<Xr:
         return [xl-vx,Xl-vX]
      return [xr-vx,Xr-vX]

   def valueyin(self,yl,yr, Yl,Yr):
      vh=-3
      vb=-9
      if yr==Yr and yl==Yl:
         return [True,self.middle(yl,yr),self.middle(Yl,Yr)]
      if yr>Yl and yr<Yr:
         #return [True,yr+vb,Yl+vh]
         return [True,self.middle(yl,yr)+vb,self.middle(Yl,Yr)+vh]
      if yl>Yl and yl<Yr:
         #return [True,yl+vh,Yr+vb]
         return [True,self.middle(yl,yr)+vb,self.middle(Yl,Yr)+vh]
      if yr<Yl:
         return [False,yr+vb,Yl+vh]
      if Yr<yl:
         return [False,yl+vh,Yr+vb]
      return [False,yl+vh,Yl+vh]
      
########################################################################################
###We draw the graph eventually 
   def drawing(self):
      lst=self.graphiczone.find_all()
      for i in lst:
         self.graphiczone.itemconfig(i,fill="white")
         self.graphiczone.delete(i)
      ps=0
      for i in self.lastanalysis.keys():
         if i in self.toavoid:
            continue
         try:  
            self.drawelement(i,self.links[i][0],self.links[i][1])
            #Base element
            bxleft=self.links[i][0]
            byleft=self.links[i][1]
            bxright=self.links[i][2]
            byright=self.links[i][3]
         
            lst=self.lastanalysis[i]
            if lst[0]=="ARG":
               lst=lst[1:-1]
            for ele in lst:            
               if ele==None:
                  continue
               if not islist(ele):
                  elementset=[ele]
               else:
                  elementset=ele
               for element in elementset:
                  if element in self.toavoid:
                     continue
                  self.drawelement(element,self.links[element][0],self.links[element][1])
                  exleft=self.links[element][0]
                  eyleft=self.links[element][1]
                  exright=self.links[element][2]
                  eyright=self.links[element][3]
                  xright,xleft=self.valuexin(exleft,exright,bxleft,bxright)
                  cmpx,yright,yleft=self.valueyin(eyleft,eyright,byleft,byright)
                  if cmpx:
                     xright,xleft=self.valueXin(exleft,exright,bxleft,bxright)
                  line=self.graphiczone.create_line(xleft,yleft,xright,yright,arrow="last",
                                                    fill=self.colors[ps],width=2.0)
            ps+=1
            if ps>=len(self.colors):
               ps=0
         except:
            pass
      
########################################################################################
###Some scrollbar handling
##   def yview(self, *args):
##      apply(self.wordBox.yview, args)
##      apply(self.mekaBox.yview, args)

   def xviewg(self, *args):
      self.incx=float(args[1])
      self.incx*=self.maxgx*2
      self.incx=int(self.incx)
      apply(self.graphiczone.xview, args)

   def yviewg(self, *args):
      self.incy=float(args[1])
      self.incy*=self.maxgy
      self.incy=int(self.incy)
      apply(self.graphiczone.yview, args)

   def yviewb(self, *args):
      apply(self.rootFrenchBox.yview, args)
      apply(self.rootMekaBox.yview, args)            
########################################################################################
   def getlemma(self,i):
      if i==None:
         return None
      pos=i.find("lemma")
      if pos!=-1:
         posf=i.find('"',pos+7)
         lem=i[pos+7:posf]
         pos=i.find("#")
         posf=i.find('[',pos)
         num=i[pos:posf]
         return lem+num
      return None

   def reloadgrammar(self):
       xips.reloadgrm(0) 

   def createlastanalysis(self):
      nb=0
      self.lastanalysis={}
      for i in self.lastgraphes.keys():
         if not islist(self.lastgraphes[i]):
             continue 
         lemi=self.getlemma(i)
         nb+=1
         if lemi==None:
            #Specific case, we bypass the dependency i, we are only interested in pairs
            for g in self.lastgraphes[i]:
               if islist(g):
                  try:
                     [w1,w2]=g
                     l1=self.getlemma(w1)
                     l2=self.getlemma(w2)
                     nb+=1
                     if l1 not in self.lastanalysis:
                        self.lastanalysis[l1]=[]
                     try:
                        self.lastanalysis[l1][-1].append(l2)
                     except:
                        self.lastanalysis[l1].append([l2])
                  except:
                     pass
            continue
         
         self.lastanalysis[lemi]=[]
         for g in self.lastgraphes[i]:
            if islist(g):
               for e in g:
                  leme=self.getlemma(e)
                  if leme==None:
                     continue
                  nb+=1
                  try:
                     self.lastanalysis[lemi][-1].append(leme)
                  except:
                     self.lastanalysis[lemi].append([leme])
            else:
               lemg=self.getlemma(g)
               if lemg==None:
                  continue
               self.lastanalysis[lemi].append(lemg)
               nb+=1
      return nb
         
########################################################################################
###Parsing the sentence

   #self.gen_text contains a list of [french, meka1,meka2..] elements
   #From this list, we extract:
   #s --> a first sentence based on the first interpretation of each element
   #stgr --> another sentence, where each word is a combination of all readings, joined with "_". When an element has been added through 
   #the ontology, then a $ is appended to the string.
   #sgentext --> a redisplay of self.gen_text, with french=[t1,t2] as elements

   def extractsentencefromgeneration(self,ambiguous,gentext):
      sgentext=""
      removes=[]
      i=0
      for si in gentext:
         if si[1]==None:
             removes.insert(0,i)
             i+=1
             continue
         try:
             sgentext+=unicode(si[0],"utf8")+"=[ "      
         except:
             sgentext+=si[0]+"=[ "   
         for e in si[1:]:
            sgentext+=e+" "
         sgentext=sgentext.strip()
         sgentext+=" ] "
         i+=1

      for si in removes:
          gentext.pop(si)
      s=""
      stgr=""
      ix=0
      for w in gentext:
          if s!="":
            s+=" "
            stgr+=" "          
          s+=w[1]
          #If the word is ambiguous (different translations)
          if len(w)>2:
            for ww in w[1:]:
                wx=ending.ending(ww)
                try:
                  #when we have an ontological element, then we add $
                  stgr+="_"+"_".join(self.ontologies[str(wx['root'])])+"$"
                except:
                  pass                   
                lm=wx['root']+wx['basis']
                wx['surf']=lm
                ambiguous.append(wx)
                stgr+="_"+wx["root"]
            stgr+="_"+w[1]
            ambiguous.append(-1)
          else:
             try:
               #when we have an ontological element, then we add $
               wx=ending.ending(w[1])
               stgr+="_"+"_".join(self.ontologies[wx['root']])+"$_"
             except:
               pass           
             stgr+=w[1]
          ix+=1

      return [s,stgr,sgentext]

   def getletext(self,mytext):
      letext=mytext.get(1.0,END)
      letext=letext.encode("utf8")
      if not self.all:
         try:
            letext=mytext.get(SEL_FIRST,SEL_LAST)
         except:
          try:
            #In this case, we try to figure out which sentence to analyse
            bg=str(mytext.index(INSERT))+"linestart"
            ed=str(mytext.index(INSERT))+"lineend"
            letext=mytext.get(bg,ed)
          except:
            pass
      return letext
   
   #We extract a sentence in French (or any other natural language), we apply a translation on it, which
   #returns a list [[fw1,t1.1,t2.2..],[fw2,t2.1,t2.2]...  fw is a French word, tx.y a translation
   def generationbase(self):
      self.parsed=False
      letext=self.textinput.get(1.0,END)
      f=open("input.txt","wb")
      letext=letext.encode("utf8")
      f.write(letext.strip()+"\n")
      f.close()
      letext=self.getletext(self.textinput)

      f=open("last.txt","wb")
      letext=letext.encode("utf8")
      f.write(letext)
      f.close()
      self.texttrans.delete(1.0,END)
      self.gen_text=self.traduire.parse(letext,self.semanticwords)
      if self.gen_text==None:
         self.texttrans.insert(INSERT,"No analysis for:"+letext+"\n","a")
         return

      self.globalsentence=[]

   #Generation minimal. RAW. The parsing of the current sentence is displayed without any specific 
   #semantic treatment
   def generation(self):
      self.generationbase()
      self.modifiedsentence(self.texttrans)
      self.texttrans.delete(1.0,END)
      ambiguous=[]
      [s,stgr,sgentext]=self.extractsentencefromgeneration(ambiguous,self.gen_text)
      gr={}
      sentence=[]
      lexicals=[]
      try:
          sgr=grammar.parse(stgr,sentence,lexicals,self.dicoparta,self.roots,gr,False,None,grammarcodes.currentlanguage,None)
      except grammar.Eraiag,e:
         res="Exception: "+e.error+" --> "+e.my_message
         self.text.delete(1.0,END)
         self.text.insert(INSERT,res,"a")

      self.globalsentence=lexicals
      self.texttrans.insert(INSERT,sgentext+"\n","a")

      self.texttrans.insert(INSERT,s+"\n","a")
      self.text.delete(1.0,END)
      if ambiguous!=[]:
          tr="Ambiguous Translations\n\n"
          for w in ambiguous:
                if w==-1:
                    tr+="---\n"
                    continue
                sx=grammarcodes.translate(w,self.dicoparta,self.roots,grammarcodes.currentlanguage)
                tr+=w['surf']+"="+sx+"\n"
      else:
          tr="No Ambiguous Translation\n"
      self.text.insert(INSERT,tr,"a")

   def dupliquegentext(self,g):
       dup=[]
       for i in g:
           dup.append(i[:])
       return dup
   
   def aff(self,g): 
        for i in g:
            print i[1],
        print
        
   def recursivecasegeneration(self,pronouns,i):        
        if i>=len(pronouns):
             nbmodif=0             
             try:
                [nbmodifbis,genres]=self.cleanInput(self.gen_text)
                nbmodif+=nbmodifbis
                while nbmodifbis>0:
                    [nbmodifbis,genresfinal]=self.cleanInput(genres)                    
                    if nbmodifbis>0:
                        nbmodif+=nbmodifbis
                        genres=genresfinal
             except:
                nbmodif=-100
             #we keep the most promising antecedent, which is the one that maximize
             #the number of projection.
             if nbmodif and nbmodif>self.maxprojections:
                 self.solutions=genres
                 self.maxprojections=nbmodif
                 return nbmodif
             return 0
        else:
             [p,tradw]=pronouns[i]
             initialelement=self.gen_text[p]
             bs=self.gen_text[p][0]
             for lant in self.lstant:
                 tradw['features'].append(lant)
                 tradw['translation']=[tradw['base']]
                 self.traduire.addcasetoroot(tradw)
                 trad=tradw['translation'][0]
                 self.gen_text[p]=[bs,trad]
                 kx=lant+str(i)
                 if kx in self.trackcases:
                     if self.trackcases[kx][0]==2:
                         return
                 nbmodif=self.recursivecasegeneration(pronouns,i+1)
                 try:
                     if self.trackcases[kx][1]==nbmodif:
                         self.trackcases[kx][0]=2
                 except:
                     self.trackcases[kx]=[1,nbmodif]
                                                                             
                 tradw['features'].pop()
             self.gen_text[p]=initialelement
        
   #We add a first layer of imediate semantic treatment
   def generationfull(self):
      self.generationbase()
      #we apply it twice in order to have all the graphs applying with a full context
      #First we detect the pronouns...
      self.pronouns=[]
      ix=0
      for w in self.gen_text:
          try:
              wx=ending.ending(w[1])
              if wx['pos']=='Pron':
                basestr=w[0]+"er" 
                tradw={'translation': [basestr], 'features': [], 'pos': 'pron','base':basestr}
                tradw['features'].append(wx['case'].upper())                
                self.pronouns.append([ix,tradw])
              if wx["pos"]=="Prep":
                 basestr=w[0]+"ar" 
                 tradw={'translation': [basestr], 'features': [], 'pos': 'prep',"base":basestr}
                 tradw['features'].append(wx['case'].upper())                
                 self.pronouns.append([ix,tradw])                 
          except:
              pass
          ix+=1
      
      initialgentext=self.dupliquegentext(self.gen_text)
      if self.pronouns!=[]:
         self.solutions=None
         self.maxprojections=-1
         self.trackcases={}
         self.decomptage=0
         self.recursivecasegeneration(self.pronouns,0)
         print self.decomptage
         if self.solutions!=None:
             self.gen_text=self.solutions
         else:
             self.gen_text=initialgentext
             
      #we apply or reapply our graphs. This solution is sub efficient.
      self.displaygraph(self.gen_text)
      
   def extractvariables(self,lexicals,g,ontos,heads):
      ndx={}
      ix=0
      for l in lexicals:
            ndx[l["idx"]]=l["root"]
            ix+=1
      lst=[]
      for i in g['VARIABLES']:
         kw=g['VARIABLES'][i]
         #if it is an ontological element, we skip it
         if kw==-1:
            continue
         if kw.isdigit():
            k=int(kw)
            lst.append(ndx[k])

      if len(lst)!=2:
          return False
        
      ky=str(lst[1])
      vl=str(lst[0])
      try:
         if vl not in ontos[ky]:
            ontos[ky].append(vl)
            heads[vl]=True
      except:
         ontos[ky]=[vl]
         heads[vl]=True
      return True


   def infer(self):
      self.saveinput() 
      letext=self.getletext(self.textinput)
      #We split the text (from texttrans) into a list of words
      lstwords=letext.split()
      self.gen_text=[]
      for w in lstwords:
          wx=w.encode("utf8")
          self.gen_text.append([wx,wx])
      self.thecurrentsentence=""
      self.inferinput(self.gen_text)
      self.parsesentence(self.thecurrentsentence) 
      self.texttrans.insert(INSERT,self.thecurrentsentence,"a")
   
   def straight(self):
      self.generationfull()
      self.parsesentence(self.thecurrentsentence) 
      self.texttrans.insert(INSERT,self.thecurrentsentence,"a")
           
   #this method has the privilege to apply graphs to the sentence (from generationbase)
   def cleanInput(self,gentext):      
      self.modifiedsentence(self.texttrans)
      return self.inferinput(gentext)
   
   def regularsort(self,x,y):
       if x[0]<y[0]:
           return -1
       if x[0]==y[0]:
           return 0
       return 1

   def invertsort(self,x,y):
       if x[0]<y[0]:
           return 1
       if x[0]==y[0]:
           return 0
       return -1
   
   def addprepcase(self,w,lastcase):
       try:
           if w["case"]=="Gen":
               if lastcase!="Nom":
                   suffix="i"+ending.cases[lastcase]+"ars"
                   return w["root"]+suffix
       except:
            pass
       return w["surf"]
               
   def displaygraph(self,gentext):
      ambiguous=[]
      [s,stgr,sgentext]=self.extractsentencefromgeneration(ambiguous,gentext)
      self.texttrans.delete(1.0,END)
      self.texttrans.insert(INSERT,sgentext+"\n","a")
      self.thecurrentsentence=s
      self.texttrans.insert(INSERT,s+"\n","a")
      self.text.delete(1.0,END)
      if ambiguous!=[]:
          tr="Ambiguous Translations\n\n"
          for w in ambiguous:
                if w==-1:
                    tr+="---\n"
                    continue              
                sx=grammarcodes.translate(w,self.dicoparta,self.roots,grammarcodes.currentlanguage)
                tr+=w['surf']+"="+sx+"\n"
      else:
          tr="No Ambiguous Translation\n"
      self.text.insert(INSERT,tr,"a")
        
   def inferinput(self,gentext):
      nbmodif=0 
      ambiguous=[]
      [s,stgr,sgentext]=self.extractsentencefromgeneration(ambiguous,gentext)
      self.texttrans.delete(1.0,END)
      gr={}
      sentence=[]
      lexicals=[]
      #We parse our current sentence.
      try:
          sgr=grammar.parse(stgr,sentence,lexicals,self.dicoparta,self.roots,gr,False,None,grammarcodes.currentlanguage,None)
      except grammar.Eraiag,e:
         return [-100,None]
      
      if self.globalsentence==[]:
         self.globalsentence=lexicals
      ndx={}
      ix=0
      for l in lexicals:
            ndx[l["idx"]]=[ix,l]
            ix+=1
      #It is important to understand, that the graph is based on a RAW analysis of our model sentence
      #while the __selection__ corresponds to the ACTUAL sentence modified, if necessary by the linguist
      genres=None
      for g in self.generationgraphes:
        if g["__language__"]!=None and g["__language__"]!=grammarcodes.currentlanguage and g["__language__"]!="ALL":
            continue
        nbprecmodif=nbmodif
        if semantics.match(gr,g):
            #For each match, we increment one
            nbmodif+=1
            lstvarias={}
            for i in g['VARIABLES']:
                lstvarias[i]=g['VARIABLES'][i]
            
            #In __selection__ we get the sentence which we keep as a model

            if '__equivalence__' not in g:
                wordlist=g['__selection__'].split()
                listofnewwords=[]
                for w in wordlist:
                    we=ending.ending(w)
                    we["surf"]=w
                    listofnewwords.append(we)
                g['__equivalence__']=listofnewwords
            lstequ=g['__equivalence__']

            lws=[]
            error=False
            #VARIABLES contains our variables associated to each element of the projecting graph
            removephrases={}
            for i in lstvarias:
                #This variable is either -1 (not instanciated) or the index of our node
                kw=lstvarias[i]
                #if kw==-1 it is an ontological element
                if kw==-1:
                  continue      
                try:          
                    if kw.isdigit():
                        k=int(i[1:])
                        #k is the int position in the base sentence, which we have matched with
                        #we need to find the correct word
                        element=ndx[int(kw)]
                        lws.append(element)
                        if "up" in element[1]:
                            parent=element[1]["up"]
                            for e in parent["node"]:
                                removephrases[e["p"]]=True
                        else:
                            removephrases[element[1]["p"]]=True
                except:
                    error=True
                    break
            
            if error:
                continue
            lws.sort(self.regularsort)
            
            #Each element is a word selection
            gens=[]
            genres=[]
            i=0
            for e in gentext:
                if i in removephrases:
                    i+=1
                    continue
                genres.append(e[:])
                i+=1
                
            modif=False
            selectnewword=0
            #lws contains the 
            offset=0
            lstpreps=[]
            casereplace=False
            nextinsertion=0
            i=0
            modifcase=False
            lastcase=None
            while i<len(lws):
                #The size of the matching is also an import criteria
                #when selecting relative pronoun is at stake
                nbmodif+=1
                [ilx,l]=lws[i]
                i+=1
                gwrd=lstequ[selectnewword]
                if l["pos"]==gwrd["pos"]:
                    #Same POS, we do nothing specific
                       root=gwrd['root'].lower()
                       #In this case, we add without any modification
                       if root in self.ontologyheads:
                           parent=l["up"]
                           for e in parent["node"]:
                               genres.insert(e["p"],gentext[e["p"]])
                               nextinsertion=e["p"]
                           continue
                       #We rebuild our word together with the ending from the target sentence
                       newcase=None
                       if modifcase:
                           #If we have removed a PREP, then we must take our case from the example
                           gfind=gwrd['surf']
                           newcase=gwrd["case"]
                       else: 
                           if casereplace:
                               #We have added a prep, then we must change the current case to dative
                               gfind=root+"d"
                               newcase="Dat"
                           else: 
                               gfind=root+l['suffix']
                               if "case" in l:
                                   newcase=l["case"]
                                              
                       parent=l["up"]
                       #We take as new position the position of the leftest node
                       insertx=parent["node"][0]["p"]
                       foreign=gentext[l["p"]][0]
                       genres.insert(insertx,[foreign,gfind])
                       #We deactivate the casereplace...
                       casereplace=False
                       lastcase=newcase
                       for e in parent["node"]:
                           if e["p"]==ilx:
                               continue
                           translations=gentext[e["p"]]
                           newtrans=[translations[0]]
                           for tw in translations[1:]:
                               if "case" in e and e["case"]!=newcase:
                                   e["case"]=newcase
                                   e["suffix"]=ending.cases[newcase]
                                   toadd=tw[:-1]+e["suffix"]
                               else:
                                    toadd=tw
                               newtrans.append(toadd)
                           genres.insert(insertx,newtrans)   
                           insertx+=1  
                       nextinsertion=ilx 
                       selectnewword+=1
                       modif=True
                       modifcase=False
                       continue
                else:
                    #We have two cases, either it was present in the sentence, or it is to be added
                    if l["pos"]=="Prep": #we must remove it
                        genres.insert(ilx,[gentext[ilx][0],None])
                        modif=True
                        modifcase=True
                        continue
                    if gwrd["pos"]=="Prep": #we must add it
                        selectnewword+=1
                        modif=True                
                        nextinsertion+=1        
                        lstpreps.append([nextinsertion,self.addprepcase(gwrd,lastcase)])
                        casereplace=True    
                        modifcase=True
                        i-=1
                        continue
                    #If it is a verb, we need to know if it is in sub
                    surf=gwrd["surf"]          
                    if gwrd["pos"]=="Verb":
                        par=l["up"]
                        while par!=None and par["pos"]!="S":
                            par=par["up"]
                        if par!=None and par["rel"]!=gwrd["rel"]:
                            rel=gwrd["rel"]
                            gwrd["rel"]=par["rel"]
                            suff=self.getending(gwrd)
                            gwrd["rel"]=rel
                            surf=gwrd["root"]+suff
                    foreign=[gentext[l["p"]][0],surf]
                    genres.insert(ilx,foreign)

            lstpreps.sort(self.invertsort)
            for e in lstpreps:
                [ilx,surf]=e
                genres.insert(ilx,[surf,surf])
                  
            if modif:              
                saveambiguous=ambiguous
                savegr=gr
                savesentence=[]
                savelexicals=[]
                save_s=s
                savestgr=stgr
                savesgentext=sgentext
                
                ambiguous=[]            
                [s,stgr,sgentext]=self.extractsentencefromgeneration(ambiguous,genres)  
                gr={}
                sentence=[]
                lexicals=[]
                #We reparse our current sentence.
                try:
                    sgr=grammar.parse(stgr,sentence,lexicals,self.dicoparta,self.roots,gr,False,None,grammarcodes.currentlanguage,None)
                    gentext=genres
                except grammar.Eraiag,e:
                    nbmodif=nbprecmodif
                    gr=savegr
                    sentence=savesentence
                    lexicals=savelexicals
                    s=save_s
                    stgr=savestgr
                    sgentext=savesgentext
                    ambiguous=saveambiguous
                            
      return [nbmodif,genres]
    
   def comparelen(self,x,y):
        xl=len(x['__selection__'])
        yl=len(y['__selection__'])
        if xl>yl:
            return -1
        if xl==yl:
            return 0
        return 1

   def comparelenstr(self,x,y):
        xl=len(x[1].split())
        yl=len(y[1].split())
        if xl>yl:
            return -1
        if xl==yl:
            return 0
        return 1

   #The goal of this method is to remove from the gen_text variables
   #the elements that the user has not used.
   #Then we split it into a list of words
   def modifiedsentence(self,mytext):
      letext=self.getletext(mytext)
      #We split the text (from texttrans) into a list of words
      lstwords=letext.split()
      i=0
      for w in lstwords:
        #We compare it to our gen_text
        #If the word does not appear in gen_text, we replace the current element with it
        if w not in self.gen_text[i][1:]:
            self.gen_text[i]=[self.gen_text[i][0],w]
        i+=1 
      thesent=" ".join(lstwords)
      return thesent
   
   #First we get our updated sentence
   #Then we parse it with the regular mekalingvata grammar     
   def trythesentence(self):
      thesent=self.modifiedsentence(self.texttrans)
      self.text.delete(1.0,END)
      self.parsesentence(thesent)  
      self.lastsentence=thesent            
      self.parsed=True

   #We transform a piece of mekalingveta into a graph 
   def getlocalgraph(self):
      letext=self.getletext(self.textinput)
      lstwords=letext.split()
      self.gen_text=[]
      eknu=False
      sentleft=""
      sentright=""
      language=None
      addlanguage=False
      for l in lstwords:
         if addlanguage:
              language=l
              break
         if l=="|":
             addlanguage=True
             continue
         if l=="eknu":
             eknu=True
         else:
             if eknu:
                 sentright+=" "+l
             else:
                 sentleft+=" "+l
                 self.gen_text.append([l,l])

      sentleft=sentleft.strip()
      sentright=sentright.strip()      
      self.texttrans.delete(1.0,END)
      if sentright=="":
          self.texttrans.insert(INSERT,sentleft+"\n","a")
      else:
          self.texttrans.insert(INSERT,sentleft+" --> "+sentright+"\n","a")
      if language!=None:
            self.texttrans.insert(INSERT,"Language:"+language+"\n","a")
      gr={}
      sentence=[]
      lexicals=[]
      try:
          grammar.parse(sentleft,sentence,lexicals,self.dicoparta,self.roots,gr,False,None,grammarcodes.currentlanguage,None)
      except grammar.Eraiag,e:
         res="Exception: "+e.error+" --> "+e.my_message
         self.text.delete(1.0,END)
         self.text.insert(INSERT,res,"a")
          
      self.globalsentence=lexicals
      self.lastgraphe=gr
      if sentright=="":
          self.lastsentence=sentleft
      else:
          self.lastsentence=sentright
      self.associatedlanguage=language
      self.parsed=True

   def updatesenses(self,sent,res,language):
    if language!=None and language!=grammarcodes.currentlanguage:
       return
   
    if "~" in res:
          #By default we use our current language
          if language==None:
              language=grammarcodes.currentlanguage
          #The last word of our expression is the key
          tokensres=res.split()
          listofwords=[]
          error=False
          for w in tokensres:
              wtr=None
              if "/" in w:
                  wr=w.split("/")
                  w=wr[1]
                  wtr=wr[0]
              we=ending.ending(w)              
              ps=we["root"].find("~")
              if ps!=-1:
                  we["wrd"]=we["root"][:ps]
              else:
                  if wtr==None:
                      we["wrd"]=we["root"]
                  else:
                      we["wrd"]=wtr
                      we["trans"]=wtr
                        
              listofwords.append(we)
          listofbasicwords=[]
          tokens=sent.split()  
          for w in tokens:
              we=ending.ending(w)
              ps=we['root'].find("_")
              if ps!=-1:
                  we["diese"]="#"+str(int(we["root"][:ps])+1)
                  we["root"]=we["root"][ps+1:]
              else:
                  error=True
                  break  
              listofbasicwords.append(we)
          #we then try to parallel the two list
          clef=None
          clefpos=None
          if not error:
              newsent=[]
              for lw in listofwords:
                  trouve=False
                  trouvepos=False
                  for lbw in listofbasicwords:
                    if "CS" in lbw:
                        continue  
                    if lbw["root"]==lw['root'] and "trans" not in lw:
                       if "CPOS" in lw:
                           lw.pop("CPOS")
                           newsent.pop()                         
                       lbw["CS"]=True 
                       newsent.append(lw["pos"]+"("+lbw["diese"]+")")
                       clef=lbw["root"]+lbw["basis"]
                       trouve=True
                       break                   
                    elif lbw["pos"]==lw['pos']:
                       if "CPOS" in lw:
                           continue 
                       lw["CPOS"]=lbw
                       newsent.append(lw["pos"]+"("+lw["wrd"]+")")
                       clefpos=lbw["root"]+lbw["basis"]
                       trouve=True
                       
                  if "CPOS" in lw:
                      lw["CPOS"]["CPOS"]=True
                  if not trouve:  
                    newsent.append(lw["pos"]+"("+lw["wrd"]+")")
                    
          if clef==None:
              clef=clefpos
              keytofind="CPOS"
          else:
              keytofind="CS"
                
          if clef!=None:                        
              newsent=",".join(newsent)
              wordata=[]
              for s in listofbasicwords:
                  if "CPOS" in s:
                     wordata.append(s["root"]+s["suffix"]+"$")             
                  elif keytofind in s:
                     wordata.append(s["root"]+s["suffix"]+s["diese"])
                  else:  
                      wordata.append(s["root"]+s["suffix"])
              wordata=" ".join(wordata)
              ele={"wordata":wordata,language:[newsent]}
              if clef in sensedata.senses:
                  sensedata.senses[clef].append(ele)
              else:
                  sensedata.senses[clef]=[ele]
              sense.buildsenses(self.dicoparta,self.roots,grammarcodes.currentlanguage)
              return True
    return False
       
   def initgenerationgraphes(self):
       idx=0
       
       for spair in self.generationgrapheslist:
          [sent,res,language]=spair
          #If it is a French or English expression, we store it in sensedata
          #if self.updatesenses(sent,res,language):
          #    continue

          newgr={}
          sentence=[]
          variables={}
          lexicals=[]

          tokens=sent.split()                                    
          for w in tokens:
              #The first characters are integers
              sub=w.find("_")
              ix=w[:sub]
              variables["#"+str(ix)]=w
            
          try:    
              sgr=grammar.parse(sent,sentence,lexicals,self.dicoparta,self.roots,newgr,False,None,grammarcodes.currentlanguage,None)
          except grammar.Eraiag,e:
              res="Exception: "+e.error+" --> "+e.my_message
              self.text.delete(1.0,END)
              self.text.insert(INSERT,res,"a")
              return                 
        
          newgr["VARIABLES"]=variables
          newgr["__idx__"]=idx
          idx+=1
          newgr['__selection__']=res
          newgr["__language__"]=language
          #We keep our word solutions
          lexicals=res.split()
          for l in lexicals:
              if not l[-1].isalpha():
                  l=l[:-1]
              w=ending.ending(l)  
              self.semanticwords[w["root"]]=True
          newgr['__basesentence__']=sent            
          self.generationgraphes.append(newgr)
       #self.generationgraphes.sort(self.comparelen)

   def semanticfilter(self):
       #First we save the input
       if not self.savetheinput:
           self.savetheinput=True
           self.textinput.delete(1.0,END)
           self.loadinput()
           return 
           
       self.saveinput()
       self.savetheinput=False
       self.textinput.delete(1.0,END)
       for i in self.generationgrapheslist:
           [sent,res,language]=i
           if language!=None:
               self.textinput.insert(INSERT,language+"\n","a")
           else: 
               self.textinput.insert(INSERT,"ALL\n","a")
           self.textinput.insert(INSERT,sent+"\n","a")
           self.textinput.insert(INSERT,res+"\n===\n","a")
   
   def storesemantics(self):
       if self.savetheinput:
           return
       letext=self.textinput.get(1.0,END)
       listoftext=letext.split("===")
       self.generationgrapheslist=[]
       for l in listoftext:
           entre=l.strip()
           if entre=="":
               continue
           entre=entre.split("\n")
           self.generationgrapheslist.append([entre[1],entre[2],entre[0]])
       self.savegraphes()
       self.savetheinput=True
       self.textinput.delete(1.0,END)
       self.loadinput() 
                   
       
   def keepselection(self):
          if not self.parsed:
            self.trythesentence()
          self.parsed=False
          self.text.delete(1.0,END)
          newgr={}
          sentence=[]
          nvgs=[]
          variables={}
          ix=0
          for w in self.globalsentence:
            if w['pos'] in ['Prep','Pron','Noun','Adj','Verb','Adv']:
               v=w['surf'].lower()
               variables["#"+str(ix)]=v
               nvgs.append(str(ix)+"_"+v)
               ix+=1

          sent=" ".join(nvgs)
          basicsent=sent
          lexicals=[]
          try:
              sgr=grammar.parse(sent,sentence,lexicals,self.dicoparta,self.roots,newgr,False,None,grammarcodes.currentlanguage,None)
          except grammar.Eraiag,e:
              res="Exception: "+e.error+" --> "+e.my_message
              self.text.delete(1.0,END)
              self.text.insert(INSERT,res,"a")
              return                 
              
          newgr["VARIABLES"]=variables
          newgr["__idx__"]=len(self.generationgraphes)

          #We remove from the last sentence, the determiners
          sent=[]
          for w in self.lastsentence.split():
              if not w[-1].isalpha():
                  w=w[:-1]
              x=ending.ending(w)
              if x!=None and x['pos'] in ['Prep','Pron','Noun','Adj','Verb',"Adv"]:
                 sent.append(w)

          self.lastsentence=" ".join(sent)          
          if self.lastsentence=="":
              self.text.insert(INSERT,"Error...","a")
              return

          newgr['__selection__']=self.lastsentence
          newgr['__language__']=self.associatedlanguage
          newgr['__basesentence__']=basicsent
          
          sentencebase=str(newgr)
          for g in self.generationgraphes:
            if str(g['ARGUMENTS'])==sentencebase:
                self.text.insert(INSERT,"Already known: "+self.lastsentence+"\n","a")   
                return


          #Is it an ontology element (reta|reca retan|recan beiag) ?
          if semantics.match(self.lastgraphe,self.grecan):
            if self.extractvariables(self.lastlexicals,self.grecan,self.ontologies,self.ontologyheads):
                self.text.insert(INSERT,"Ontological input: "+self.lastsentence+"\n","a")
          elif semantics.match(self.lastgraphe,self.gretan):
            if self.extractvariables(self.lastlexicals,self.gretan,self.ontologies,self.ontologyheads):        
                self.text.insert(INSERT,"Ontological input: "+self.lastsentence+"\n","a")

          self.saveinput()  
          #we test the nature of our new graph
          addingraph=True
          #if "~" in self.lastsentence:
              #We have added this description in the senses 
          #    if self.updatesenses(basicsent,self.lastsentence,self.associatedlanguage):
          #        addingraph=False
          
          if addingraph:  
              self.generationgraphes.append(newgr)
              self.generationgraphes.sort(self.comparelen)
              
          self.generationgrapheslist.append([basicsent,self.lastsentence,self.associatedlanguage])
          try:
              self.generationgrapheslist.sort(self.comparelenstr)
              self.associatedlanguage=None
              self.text.insert(INSERT,"We keep: "+self.lastsentence+"\n","a")
              self.savegraphes()
          except:
              print "Erreur:",self.generationgrapheslist
   
   def savegraphes(self):          
          f=open("selecttrans.py","w")
          f.write("# -*- coding: utf-8 -*-\n\n")
          f.write("translationgraphes=")
          f.write(str(self.generationgrapheslist)+"\n")
          f.write("ontologies=")
          f.write(str(self.ontologies)+"\n")
          f.write("ontologyheads=")
          f.write(str(self.ontologyheads)+"\n")
          f.close()
        
   #This method takes as input a sentence from the text zone (see partiag) or the texttrans zone (extracted with trythesentence)
   #It parses it with the regular grammar
   def parsesentence(self,letext):
      self.lastgraphe={}
      generated=[]
      sentence=[]
      self.lastlexicals=[]
      try:
         prs=grammar.parse(letext,sentence,self.lastlexicals,self.dicoparta,self.roots,self.lastgraphe,True,generated,grammarcodes.currentlanguage,self.ontologies)
         prs+="\n"
         resg=""
         res=""
         for g in generated:
            resg+=g
            resg+="\n"
         #If a question is posed then we project the question against the previous
         #sentence or set of sentenced that have been analyzed
         if self.lastgraphes!=None and "QUESTION" in self.lastgraphe:
            qst=semantics.project(self.lastgraphes,self.lastgraphe)
            resg="Initieca deskriptita\n"
            res=str(self.lastgraphes)+"\n\n"
            res+="Quata:\n"
            res+=str(self.lastgraphe)+"\n\n"
            if qst==0:
               resg+="---> Respondeta malverecan beiag\n"
            elif qst==1:
               resg+="---> Respondeta verecan beiag\n"
            else:
               resg+="---> Respondeta nekonatecan beiag\n"
         else:
            res=prs
            #We redraw the graph
            self.lastgraphes=self.lastgraphe
            maxnodes=self.createlastanalysis()
            nbmaxnode=maxnodes/4
            self.initspaces(nbmaxnode,30,30)
            self.lstcoord=[]
            self.computexy()
            self.drawing()
            for g in self.lastgraphe:
                if g in ["KEY","ARGUMENTS"]:
                    continue
                res+=g+":"+str(self.lastgraphe[g])+"\n"
         res+="\n\n"
         self.text.delete(1.0,END)
         self.text.insert(INSERT,res,"a")
         self.texttrans.delete(1.0,END)
         xresg=unicode(resg,"latin-1")
         self.texttrans.insert(INSERT,xresg,"a")
      except grammar.Eraiag,e:
         res="Exception: "+e.error+" --> "+e.my_message
         self.text.delete(1.0,END)
         self.text.insert(INSERT,res,"a")                 
   
   
   def saveinput(self):
      if not self.savetheinput:
          return 
      letext=self.textinput.get(1.0,END)
      letext=letext.encode("utf8")
      f=open("input.txt","wb")
      f.write(letext.strip()+"\n")
      f.close()
      letext=self.getletext(self.textinput)

      f=open("last.txt","wb")
      letext=letext.encode("utf8")
      f.write(letext)
      f.close()
      return letext
       
   def partiag(self):
      letext=self.saveinput() 
      self.parsesentence(letext)
      


   def buildsentencenode(self,sentence,nodes):
      for s in sentence:
         if "node" in s:
            self.buildsentencenode(s["node"],nodes)
         else:
            nodes.append(s)
            
   def tokenize(self):
      letext=self.getletext(self.textinput)
      try:
         gr={}
         generated=[]         
         sent=[]
         lexicals=[]
         grammar.parse(letext,sent,lexicals,self.dicoparta,self.roots,gr,True,generated,grammarcodes.currentlanguage,None)
         sentence=[]
         self.buildsentencenode(sent,sentence)
         self.text.delete(1.0,END)
         for s in sentence:
            ch=grammarcodes.displaynode(s)+"\n\n"
            self.text.insert(INSERT,ch,"a")                 
      except grammar.Eraiag,e:
         res="Exception: "+e.error+" --> "+e.my_message
         self.text.delete(1.0,END)
         self.text.insert(INSERT,res,"a")                 


########################################################################################
###Online help
   def displayrules(self):
      self.text.delete(1.0,END)
      letext=self.textinput.get(1.0,END)
      self.text.insert(INSERT,grammarrules,"a")


   def storereference(self):
      self.all=True
      self.partiag()
      self.all=False
      letext=self.texttrans.get(1.0,END)
      f=open("translation_"+grammarcodes.currentlanguage+".txt","w")
      f.write(letext.strip()+"\n")
      f.close()

   def comparereference(self):
      self.all=True
      self.partiag()
      self.all=False
      letext=self.texttrans.get(1.0,END)
      letext=letext.strip()+"\n"
      f=open("translation_"+grammarcodes.currentlanguage+".txt","r")
      base=f.read()
      f.close()
      letext=letext.split("\n")
      base=base.split("\n")
      self.texttrans.delete(1.0,END)
      letext.sort()
      base.sort()
      
      b=iter(base)
      error=False
      
      
      for l in letext:
         a=b.next()
         if l.strip()!=a.strip():
            if not error:
               self.texttrans.insert(INSERT,"Error:\n","a")
            error=True
            self.texttrans.insert(INSERT,"Basis:   "+a.strip()+"\n","a")
            self.texttrans.insert(INSERT,"Current: "+l.strip()+"\n\n","a")
      if not error:
         self.texttrans.insert(INSERT,"Ok\n","a")
            
      
   def message(self,msg):
      d=Dialog.Dialog(self.maitre,title="Message",text=msg,strings=["  Ok  "],bitmap="",default=0)
      

   def quitting(self):
     self.lafin=True
     self.frame.quit()
     #self.frameboite.quit()
     #self.frameinput.quit()

   def reset(self):
     self.lastanalysis=None
     self.lastgraphes=None
     self.previouslinks=None
     self.entreefr.delete(0,END)
     self.entreepar.delete(0,END)
     self.entreequ.delete(0,END)
     self.text.delete(1.0,END)
    
########################################################################################
###Looking for a word in the lexicon
   def lookfor(self,motlatin,trad):
      global trucmot
      trucmot=motlatin
      aword=motlatin.encode("utf-8")
      trouve=[-1,-1]
      i=0
      self.text.delete(1.0,END)
      self.text.insert(INSERT,"ROOTS\n","a")
      for lrc in self.lesracs:
         l="^"+lrc+"$"
         try:
            l=l.encode("utf-8")
         except:
            pass
         if l.find(aword)!=-1:
            if trouve[0]==-1:
               trouve[0]=i
            self.text.insert(INSERT,lrc+"\n","a")
         if trad!="" and l.find(trad)!=-1:
            if trouve[0]==-1:
               trouve[0]=i
            self.text.insert(INSERT,lrc+"\n","a")
         i+=1
      self.text.insert(INSERT,"\nMOTS\n","a")
      i=0
      for l in self.lesmots:
         if l.find(aword)!=-1:
            if trouve[1]==-1:
               trouve[1]=i
            self.text.insert(INSERT,l+"\n","a")
         if trad!="" and l.find(trad)!=-1:
            if trouve[1]==-1:
               trouve[1]=i
            self.text.insert(INSERT,l+"\n","a")
         i+=1
      return trouve

      
   def search(self):
     aword=self.entreequ.get()
     self.text.delete(1.0,END)
     res=self.lookfor(aword,"")
     idx=res[0]
##     if idx!=-1:
##        self.rootMekaBox.see(idx)
##        self.rootFrenchBox.see(idx)
##        self.rootMekaBox.select_set(idx)
##        self.rootFrenchBox.select_set(idx)
##     idx=res[1]
##     if idx!=-1:
##        self.mekaBox.see(idx)
##        self.wordBox.see(idx)
##        self.mekaBox.select_set(idx)
##        self.wordBox.select_set(idx)
# -*- coding: utf8 -*-
########################################################################################
###We store each new sentence into an input.txt file
   def loadinput(self):
      try:
         f=open("input.txt","rb+")
         for e in f:
            try:
                e=e.strip()
                e=unicode(e,"utf8")
                e+="\n"
                self.textinput.insert(INSERT,e,"a")
            except:
                print "Erreur: ",e
         f.close()
      except:         
         pass
########################################################################################
###Loading the listbox with words
### We handle two sorts of lexicon: for French to mekalinga and back
   def loadvocabulary(self):
##     self.mekaBox.delete(0,END)
##     self.wordBox.delete(0,END)
##     self.rootMekaBox.delete(0,END)
##     self.rootFrenchBox.delete(0,END)

     self.lesmots=[]
     self.dicoparta={}
     self.lesracs=[]
     self.roots={}

     msorted=lingata.lexicons.keys()
     msorted.sort()
     wsorted={}
     for i in msorted:
         s=lingata.lexicons[i]
         rac=s["rakta"]
         lemf=""
         leme=""
         lems=""
         #We fetch the possible translation in French or in English
         if "francais" in s:
            mtf=s["francais"]
            for ms in mtf:
               if lemf!="":
                  lemf+="|"
               lemf+=ms

         if "english" in s:
            mte=s["english"]
            for ms in mte:
               if leme!="":
                  leme+="|"
               leme+=ms
         #We then compute our final translation
         if grammarcodes.currentlanguage=="francais":
            if lemf!="":
               lems=lemf+"||"+leme
               mt=mtf
         else:
            if leme!="":
               lems=leme+"||"+lemf
               mt=mte

         if lems!="":
            self.dicoparta[i]=unicode(rac+"="+lems,"utf8")
            self.roots[rac]=unicode(lems,"utf8")
            self.lesracs.append(unicode(rac+":"+lems,"utf8"))
            wsorted[mt[0]]=unicode(rac,"utf8")
            lg=mt[0]+"="+rac+"\t"+i
            self.lesmots.append(unicode(lg,"utf8"))
            
     msorted=wsorted.keys()
     msorted.sort()     

root=Tk()
nm="MEKALINGATA"
root.title(nm)


apparta=App(root)


root.mainloop()
print "The End"

