import grammar
import lingata
import numbers
import semantics
import sense

currentlanguage="francais"
class vocab:

   def __init__(self):
     self.lesmots=[]
     self.dicoparta={}
     self.lesracs=[]
     self.roots={}
     self.loadvocabulary()
     sense.buildsenses(self.dicoparta,self.roots,currentlanguage)


   def loadvocabulary(self):
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
         if currentlanguage in s:
            mt=s[currentlanguage]
            lems=""
            for ms in mt:
               if lems!="":
                  lems+="|"
               lems+=ms
            self.dicoparta[i]=rac+"="+lems
            self.roots[rac]=lems
            self.lesracs.append(rac+":"+lems)
            wsorted[mt[0]]=rac
            self.lesmots.append(mt[0]+"="+rac+"\t"+i)

   def parse(self,txt,gr,generated,sentence):
      prs=grammar.parse(txt,sentence,self.dicoparta,self.roots,gr,True,generated,currentlanguage)
      return prs

def justeparse(txt):
   a=vocab()
   gr={}
   gen=[]
   sentence=[]
   try:
      print a.parse(txt,gr,gen,sentence)      
      print gen
      print
      for g in gr:
         print g,gr[g]
   except grammar.Eraiag,e:
      res="Exception: "+e.error+" --> "+e.message
      print res
   print


#justeparse("Tala lingata alan hutan kumiar konpurtad telakaiega edaiaga kanaiag.")
#justeparse("Tala lingata mulan alan hutan uder kumiar konputtad telakaieg edaiaga kanaiag.")
#justeparse("Tala lingata uder kumiar hutad lingaieg usaiagas beiagas ovaiagne foer tala lingata durcan udiar lernaiegad beieg.")
f=open("last.txt","r")
for s in f:
   justeparse(s)
f.close()
