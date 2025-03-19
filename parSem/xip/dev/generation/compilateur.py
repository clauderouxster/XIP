"""
Below is the list of all possible attributes for the ROOT tag:
    ITEMS ROOT attributes:
        items="..."    this is the name of ontology variable 
        class="..."    the class which will encapsulate the generator
    
    PLAN ROOT attributes:
        sentence="..."     the sentence variable which will store the generated sentence
        items="..."        the ontology variable
        attribute="..."    the attribute that defines a lexical unit
        value="..."        the value of that lexical unit
        class="..."        the class which will encapsulate the generator
        
    RULE ROOT attributes:
        It is IMPORTANT to understand that the "class" attribute (see above) should
        share the same name as the root tag name... If class="generation" then the name of the
        TAG ROOT should be: <generation...
        sentence="..."     the sentence variable which will store the generated sentence
        items="..."        the ontology variable
        attribute="..."    the attribute that defines a lexical unit
        value="..."        the value of that lexical unit

    ATTRIBUTES for the other tags:
        attribute:    the name of the attribute (could be a list of attributes separated with a space)
        value:        its alphabetical value
        num:          its numerical value (cannot have the two XML attributes at the same time)
        position:     only used in GETNODE, can take first,last or a number as value
        args:         the list of arguments: "arg1 arg2". Each argument is separated with a space.
                      If an argument is written at the end of the list with two: "arg1 arg2 [arg]"
                      then this argument will be used as a table of arguments.
        message:      Used in printout or in comment.
        index:        Defines an index in the ontology
        comment:      The comment will be appended in the generated code as a comment
        

Below is the list of all possible instructions:

       <printout>
           Print out a message on the standard output for a given argument: 
                <printout message="..." args="node"/>
                <printout message="..." args="node" attribute="x"/>
               attribute is optional
       
 
      <end/>
          Stop a generation process
        
     <comments message="..."/> 
         Add a comment message in the generated Python body. This tag can be replaced by a "comment" attribute for each instruction tag

    <feature args="node">...</feature>
        This method is used to define a set of features for a given argument:

    <get args="..." attribute="..."/>
        This method copies the source attribute value from the source argument to the target attribute of the target argument
    <get args="source target" attribute="attSource attTarget"/>
        If only one argument is provided then the system expects two attributes.
        In that case, attSource is set with the value of attTarget
    <get args="source" attribute="attSource attTarget"/>
        if attribute contains only one value, then the exchange will be done for that unique attribute
    
    <set args="node" attribute="a1 a2 a3" value="v1 v2 v3"/>
        set a value to an attribute for a given node
        if the set is in a <feature...>...</feature> section, then the node is optional
        <set args="node" attribute="a1 a2 a3" value="v1 v2 v3"/>

    <duplicate args="target source"/>
        Duplication of an argument:
        <duplicate args="target source"/>
        source is duplicated as target
        All the source features are copied into the target
    
   <getnode args="node" position="first"/>  
   <getnode args="node" position="last"/>
   <getnode args="node" position="nb"/>
          This method is used to get the first or the last word of a sentence.
          It can also be used to get the nbth word from a sentence

        
    <create args="new"/>
        Creates a new node
        attributes and values can be provided, They should be in the same number
        <create args="new" attributes="a1 a2..." values="v1 v2..."/>

    <test ...> or <testnot ...>
        This method is used to test the validity of a node on certain attributes
        By default op='=='
        <test args="node" attribute="a" value="v" op='>'/>
        
        If the comparison is done between two nodes:
        <test args="node1 node2" attribute="a"/>
        
        Of course the attributes could be different as well
        <test args="node1 node2" attribute="a1 a2"/>
        If only one attribute is provided, then the comparison is done on that unique attribute
        
        If the test is on an empty list:
        <test args="liste">...</test>
        Returns True if the list is not empty
    
    <otherwise>
        This method should be used within a test or a testnot:
        <test...>
            ...
            <otherwise>
            ...
            </otherwise>
        </test>
        It is the equivalent of a else


    <generate args="node1 node2 node3..."/>
        Generate a list of nodes at the sentence level

    <sub args="parent child1 child2 child3"/>
        adds a node at the sub level:
        childi is added under parent

    <minor args="parent child1 child2 child3"/>
        Adds a node in the minor section of another node

    <concat args="x" attribute="a">string</concat>
        Concatenation of a string to the value of an attribute

    <inc args="xxx" attribute="a" step=2/>
        Increment a num attribute in a node
        step is optional, its default value is 1

    <dec args="xxx" attribute="a" step=2/>
        Decrement a num attribute in a node
        step is optional, its default value is 1

     <upperfirstchar args="n"/>  
         Put the first character of an argument in upper

    <entree index="...">
        <set attribute="..." value="..">
    </entree>
        This tag is used to define an entree in the ontology

     <use...>
        use implements the inheritance mechanism
        <use args="node" index="...">
        If used in an entree, the args is optional
    
    <variable args="cpt">
        <set attribute="compteur" num="0"/>
    </variable>
        This tag implements a variable declaration. This variable should be used
        with a self in the code.
        Ex:
            <printout args="self.cpt" message="Counter:"/>

"""
from xml.sax import ContentHandler, make_parser, SAXParseException
from string import Template

####################################################################################################################
####################################################################################################################
####################################################################################################################

class handleattribute :
    def __init__(self):
        self.body=""
        self.indentation=[]
        self.listeargument=""
        self.lastposition=0

    def replace(self,i):
       i=i.replace(';','')
       i=i.replace(' ','')
       i=i.replace(',','')
       i=i.replace('&','')
       i=i.replace('"','')
       i=i.replace("'",'')
       i=i.replace("$",'')
       i=i.replace(".",'')
       i=i.replace("-",'')
       if i[0].isdigit():
           i="D_"+i
       return i

    def getargs(self,attrs):
       x=self.replace(attrs["args"])
       return x
        
    def getarg(self,attrs):
       if "args" in attrs:
           x=self.replace(attrs["args"])
           return x
       else:
           return self.argument

    def patron(self,code,**args):
        pad=self.padding()
        s=Template(code)
        s=s.substitute(args,p=pad)
        s=s[1:]+"\n"
        self.body+=s
        
    def splitattribute(self,name,attrs,args,addenda):
        liste=False
        try:
            s=attrs[name]
            args.extend(s.split())
            if args[-1][0]=='[' and args[-1][-1]==']':
                args[-1]='*'+args[-1][1:-1]            
            if args[-1]==self.listeargument:
                liste=True
            if name!="value":
                j=0
                for i in args:
                    i=self.replace(i) 
                    if addenda!="":
                        i=addenda+i
                    args[j]=i
                    j+=1
        except:
            msg="Missing XML attribute: "+name
            raise SAXParseException(msg,ValueError,self._locator) 
        return liste
            
    def padding(self):
        pad=""
        if len(self.indentation)!=0:
            pad="".zfill(self.indentation[-1]*2-1).replace('0',' ')
        return pad
        
    def add(self,st):
        if len(self.indentation)!=0:
            self.body+="".zfill(self.indentation[-1]*2).replace('0',' ')
        self.body+=st+"\n"

    def addb(self,st):
        if len(self.indentation)!=0:
            self.body+="".zfill(self.indentation[-1]*2).replace('0',' ')
        self.body+=st
    
    def addid(self,st):
        self.incremente()
        self.add(st)
        self.decremente()

    def adddi(self,st):
        self.decremente()
        self.add(st)
        self.incremente()

    def addi(self,st):
        self.add(st)
        self.incremente()

    def addd(self,st):
        self.decremente()
        self.add(st)

    def incremente(self):
        self.indentation.append(len(self.indentation)+1)

    def decremente(self):
        self.indentation.pop()

    def getincrement(self):
        return len(self.indentation)
    
    def setincrement(self,s):
        si=len(self.indentation)
        for i in range(s,si,1):
            self.indentation.pop()

    def getmultiplevalues(self,name,ag,attrs):
        attributes=[]
        values=[]
        if "attribute" in attrs:
            self.splitattribute("attribute",attrs,attributes,"")
            qop=''
            qclose=''
            if "value" in attrs:                
                self.splitattribute("value",attrs,values,"")
                qop='"'
                qclose='"'
            elif "num" in attrs:
                self.splitattribute("num",attrs,values,"")
            else:
                raise SAXParseException("Missing values in '"+name+"'",ValueError,self._locator)
            if len(values)!=len(attributes):
                raise SAXParseException("Different number of values and attributes in a'"+name+"'",ValueError,self._locator)
            else:
                i=0                        
                for a in attributes:
                    self.add(ag+'["'+a+'"]='+qop+values[i]+qclose)
                    i+=1
        else:
            raise SAXParseException("Missing values in '"+name+"'",ValueError,self._locator)

####################################################################################################################
####################################################################################################################
####################################################################################################################
####################################################################################################################

class rulegeneration(ContentHandler,handleattribute):
    "Document Handler personnel"
    def __init__(self):
        "initialisation"
        self.xmlvalues=["items","value","attribute","class","sentence"]
        self.xmlattributes=["attribute","value","num","position","args","message","index","comment"]
        handleattribute.__init__(self)
        self.rulebasis="""
        
  def loadonto(self,x,n):
      try:
          keys=self.$ontologie[x]
          for i in keys:
              if keys[i]=='@':
                  self.loadonto(i,n)
              else:
                  n[i]=keys[i]
      except:
          pass

  def initfeatures(self,n):
      try:
          if ":" not in n:
              x=n["index"]
              self.loadonto(x,n)               
              n[':']='+'
              for i in n['@']:
                  self.loadonto(i,n)
      except:
          n[':']='+'
          pass        

  def display(self,phr,*f):
      self.recursivedisplay(phr,1)
      print


  def recursivedisplay(self,phr,indent):
      if "sub" not in phr:
          return
      for i in phr["sub"]:
          try:
            _min=i['minor']
            for j in _min:             
             if "$level" not in j or j["$level"]=="$lex":
               pad="".zfill(indent).replace('0',' ')
               print pad+j["lemma"]
             if "sub" in j:
                self.recursivedisplay(j,indent+1)  
          except:
            pass
          if "$level" not in i or i["$level"]=="$lex":
             pad="".zfill(indent).replace('0',' ')
             print pad+i["lemma"]                         
          self.recursivedisplay(i,indent+1)  

  def createlistofnodes(self,phr):
      self.listofnodes=[]
      self.recursiveenliste(phr)


  def recursiveenliste(self,phr):
      if "sub" not in phr:
          return
      for i in phr["sub"]:
          try:
            _min=i['minor']
            for j in _min:             
             if "$level" not in j or j["$level"]=="$lex" and j not in self.listofnodes:
                 self.listofnodes.append(j)
          except:
            pass
          if "$level" not in i or i["$level"]=="$lex" and i not in self.listofnodes:
              self.listofnodes.append(i)
          self.recursiveenliste(i)  

"""

        #A makup tag under the root might be transformed into a function
        #A function is associated to a return argument
        self.ancestors=[]
        self.ancestorsid=[]
        self.procedures=[]
        self.noms=[]
        self.erreur=[]
        self.phrase=""
        self.argument=""
        self.items=""
        self.phrs=[]
        self.premier=True
        self.ix=0
        self.retour=[]
        self.bar=""
        self.lexical=""
        self.typetest=[]
        self.depth=0

        
    def startDocument(self):
        "fonction appelee lorsque le parser rencontre le premier element"
        pass

    def startElement(self, name, attrs):
        """
        fonction appelee lorsque le parser rencontre une balise ouvrante.
        This is also here that the "comment" attribute is processed
        """
        for s in attrs.keys():
            if s not in self.xmlvalues:
                msg="Unknown XML attribute '"+s+"' in XML markup tag: '"+name+"'"
                raise SAXParseException(msg,ValueError,self._locator)
        
        #If there is a comment attribute in the XML node, we process it and generate a comment
        #into the final source code.
        if "comment" in attrs:
            self.add('#'+attrs["comment"])
            
        if self.depth==0:
            self.xmlvalues=self.xmlattributes
            
        self.depth+=1
        try:
            eval("self."+name+"(attrs)")
        except AttributeError:
            self.cree(name,attrs)
                
        self.ancestors.append(name)


#############  Markup tags implementation
############# Each tag, which corresponds to a specific function, is implemented as a method

    def cree(self,name,attrs):
       if self.premier==True:
           self.premier=False
           try:
               self.phrase=attrs["sentence"]
               self.items=attrs["items"]
               self.bar=attrs["attribute"]
               self.lexical=attrs["value"]
               self.incremente()
               self.incremente()
               self.add('self.lastnode=0')
               self.add('self.listofnodes=[]')
               self.add('self.index=0\n')
               self.decremente()
               self.patron(self.rulebasis,ontologie=self.items, phr=self.phrase, level=self.bar, lex=self.lexical)
           except:
               print "erreur sur generation"
       else:
           if self.depth>2:
               self.addi("try:")
               args=[]
               liste=self.splitattribute("args",attrs,args,"")
               self.add("_rec"+name+"={}")
               if "attribute" in attrs:                   
                   self.getmultiplevalues(name,"_rec"+name,attrs)
               
               self.addb('self.'+name+'('+self.phrs[-1]+",_rec"+name)
               msgerr="_rec"+name
               for i in args:
                   msgerr+=','
                   self.body+=","
                   if i==self.listeargument:
                       self.body+='*'
                   self.body+=i
                   msgerr+=i
               self.body+=")\n"
               self.adddi("except:")
               self.add('print "Error in call to '+name+'('+self.phrs[-1]+','+msgerr+') in '+self.noms[-1]+'",'+msgerr)
               self.decremente()
           else:
               self.listeargument=""
               self.procedures.append(self.getincrement())
               self.noms.append(name)
               self.addb('def '+name+'(self,phr,_'+name)
               args=[]
               self.splitattribute("args",attrs,args,"")
               for i in args:
                   self.body+=","
                   self.body+=i
               self.body+="):\n"
               self.incremente()
               for a in args:
                   if a[0]!='*':
                       self.add("self.initfeatures("+a+")")
                   else:
                       self.listeargument=a[1:]
               self.phrs.append("phr")

    
    def printout(self,attrs):
       '''Print out a message on the standard output for a given argument: 
            <printout message="..." args="node"/>
            <printout message="..." args="node" attribute="x"/>
       attribute is optional'''
       arg=""
       if "args" in attrs:
           args=[]
           self.splitattribute("args", attrs, args,"")
           if len(args)!=1:
               raise SAXParseException("Wrong number of args in PRINTOUT",ValueError,self._locator)           
           arg=args[0]           
       msg=""
       try:
           msg=attrs["message"]
       except:
           pass
       try:
           attr=attrs["attribute"]
           self.addi('try:')
           self.add('print "'+msg+" "+arg+'['+attr+']:",'+arg+'["'+attr+'"]')
           self.adddi('except:')
           self.add('print "no attribute:'+arg+'['+attr+']"')
           self.decremente()
       except:
           self.add('print "'+msg+" "+arg+' ",'+arg)
       
    def use(self,attrs):
       '''
        use implements the inheritance mechanism
        <use args="node" index="...">
       '''
       if "args" in attrs:
           args=[]
           self.splitattribute("args", attrs, args,"")
           if len(args)!=1:
               raise SAXParseException("Wrong number of args in USE",ValueError,self._locator)           
           arg=args[0]           
       else:
           arg=self.argument
       try:           
           l='self.loadonto("'+attrs["index"]+'",'+arg+')'
           self.add(l)
       except:
           msg="Missing index attribute in 'use'"
           raise SAXParseException(msg,ValueError,self._locator) 
    
    def getnode(self,attrs):
        '''
          <getnode args="node" position="first"/>  
          <getnode args="node" position="last"/>
          <getnode args="node" position="nb"/>
          This method is used to get the first or the last word of a sentence.
          It can also be used to get the nbth word from a sentence
        '''
        position=""
        if "args" in attrs:
           args=[]
           self.splitattribute("args", attrs, args,"")
           if len(args)!=1:
              raise SAXParseException("Wrong number of args in GETNODE",ValueError,self._locator)
           self.argument=args[0]                       
        else:
            msg="Missing argument in GETNODE"
            raise SAXParseException(msg,ValueError,self._locator) 
        try:
            position=attrs["position"]            
        except:
            msg="No NODE field in GETNODE"
            raise SAXParseException(msg,ValueError,self._locator) 
        self.add("self.createlistofnodes("+self.phrs[-1]+")")
        position=position.lower()
        if position=="first":
            self.add(self.argument+"=self.listofnodes[0]")
        elif position=="last":
            self.add(self.argument+"=self.listofnodes[-1]")
        elif position.isdigit():
            self.add(self.argument+"=self.listofnodes["+position+"]")
        else:
            msg="Wrong POSITION value in GETNODE (should be 'first','last' or a number)"        
            raise SAXParseException(msg,ValueError,self._locator) 

            
    def end(self,attrs):
        'Exit a method'
        self.add("return")
        
    def upperfirstchar(self,attrs):
        '''Put the first character of an argument in upper
        <upperfirstchar args="n"/>
        '''
        if "args" in attrs:
           args=[]
           self.splitattribute("args", attrs, args,"")
           if len(args)!=1:
               raise SAXParseException("Wrong number of args in UPPERFIRSTCHAR",ValueError,self._locator)           
           self.argument=args[0]                       
        else:
            msg="Missing argument in upperfirstchar"
            raise SAXParseException(msg,ValueError,self._locator) 
        try:
            self.attribute=attrs["attribute"]
            code="""
$p s=$arg["$att"]
$p s=s[0].upper()+s[1:]
$p $arg["$att"]=s"""
            self.patron(code,arg=self.argument,att=self.attribute)
        except:
            msg="Error in an upperfirstchar"
            raise SAXParseException(msg,ValueError,self._locator) 
            
    def feature(self,attrs):
        '''
        This method is used to define a set of features for a given argument:
        <feature args="node">...</feature>
        '''
        if "args" in attrs:
           args=[]
           self.splitattribute("args", attrs, args,"")
           if len(args)!=1:
              raise SAXParseException("Wrong number of args in FEATURE",ValueError,self._locator)

           self.argument=args[0]                       
        else:
            msg="Missing argument in feature"
            raise SAXParseException(msg,ValueError,self._locator) 

    def comments(self,attrs):
        'To add a comment message in the generated Python body: <comments message="..."/>'
        if "message" in attrs:
            self.add('#'+attrs["message"])

    def get(self,attrs):
        '''
        This method copies the source attribute value from the source argument to the target attribute of the target argument
        <get args="source target" attribute="attSource attTarget"/>
        If only one argument is provided then the system expects two attributes.
        In that case, attSource is set with the value of attTarget
        <get args="source" attribute="attSource attTarget"/>
        if attribute contains only one value, then the exchange will be done for that unique attribute
        '''
        args=[]
        attributes=[]
        self.splitattribute("args",attrs,args,"")            
        self.splitattribute("attribute",attrs,attributes,"")
        if len(attributes)>2:
            msg="Wrong number of attributes in a GET:",attrs
            raise SAXParseException(msg,ValueError,self._locator) 
        
        att1='["'+attributes[0]+'"]'
        matt1=attributes[0]
        if len(attributes)==1:
            att2='["'+attributes[0]+'"]'
            matt2=attributes[0]
        else:
            att2='["'+attributes[1]+'"]'
            matt2=attributes[1]
            
        x=args[0]+att1+"="
        err=args[0]
        if len(args)==1:
            x+=args[0]+att2
        else:
            x+=args[1]+att2
            err=args[1]
        self.addi("try:")
        self.add(x)
        self.adddi("except:")
        self.add('print "(GET) unknown attribute: '+matt1+','+matt2+' in '+self.noms[-1]+' for '+err+':",'+err)
        self.decremente()
        
    def duplicate(self,attrs):
        '''Duplication of an argument:
        <duplicate args="target source"/>
        source is duplicated as target
        All the source features are copied into the target
        '''
        args=[]
        if "args" in attrs:
            self.splitattribute("args",attrs,args,"")            
            if len(args)!=2:
                raise SAXParseException("Wrong number of arguments",ValueError,self._locator)  
            self.add(args[0]+'={}')          
            self.addi('for _x in '+args[1]+'.keys():')
            self.addi('if _x not in ["sub","minor","root"]:')
            self.add(args[0]+'[_x]='+args[1]+'[_x]')
            self.decremente()
            self.decremente()
        else:
            raise SAXParseException("Error on a DUPLICATE",ValueError,self._locator)
            
    def create(self,attrs):
        '''
        Creates a new node
        <create args="new"/>
        
        attributes and values can be provided, They should be in the same number
        <create args="new" attributes="a1 a2..." values="v1 v2..."/>
        '''
        args=[]
        self.splitattribute("args", attrs, args,"")
        if len(args)!=1:
            raise SAXParseException("Wrong number of args in CREATE",ValueError,self._locator)
        self.argument=args[0]
        self.add(self.argument+"={}")
        self.getmultiplevalues("CREATE",self.argument,attrs)
            
    def set(self,attrs):
        '''
        set a value to an attribute for a given node
        if the set is in a <feature...>...</feature> section, then the node is optional
        <set args="node" attribute="a1 a2 a3" value="v1 v2 v3"/>
        '''
        if "args" in attrs:
            args=[]
            self.splitattribute("args", attrs, args,"")
            if len(args)!=1:
                raise SAXParseException("Wrong number of args in SET",ValueError,self._locator)
            ag=args[0]            
        else:
            ag=self.argument
        self.getmultiplevalues("SET",ag,attrs)
                                                       
    def test(self,attrs):
        '''
        This method is used to test the validity of a node on certain attributes
        By default op='=='
        <test args="node" attribute="a" value="v" op='>'/>
        
        If the comparison is done between two nodes:
        <test args="node1 node2" attribute="a"/>
        
        Of course the attributes could be different as well
        <test args="node1 node2" attribute="a1 a2"/>
        If only one attribute is provided, then the comparison is done on that unique attribute
        
        If the test is on an empty list:
        <test args="liste">...</test>
        Returns True if the list is not empty
        '''
        try:
            op=attrs["op"]
        except:
            op="=="
            
        args=[]
        attributes=[]
        values=[]
        self.typetest.append(1)
        liste=self.splitattribute("args",attrs,args,"")
        self.retour.append(self.getincrement())
        self.argument=args[0]
        if liste==True:
            self.addi('if '+self.listeargument+'!=():')
            return
        
        self.splitattribute("attribute",attrs,attributes,"")
            
        att1=attributes[0]
        if len(attributes)==1:
            att2=attributes[0]
        else:
            att2=attributes[1]
        
        if len(args)==1:
            self.splitattribute("value",attrs,values,"")
            if len(values)==0:
                self.splitattribute("num",attrs,values,"")
            if len(values)!=len(attributes):
                msg="Mismatch between attribute and value"
                raise SAXParseException(msg,ValueError,self._locator)
            
            if len(attributes)==1:
                code="""
$p if "$att" in $arg and $arg["$att"] $oper "$val":"""
                self.patron(code,att=att1,arg=args[0],val=values[0],oper=op)
                self.incremente()
            else:
                bdy="if "
                v=0
                for a in attributes:
                    if v!=0:
                        bdy+=" and "
                    bdy+='"'+a+'" in '+args[0] +' and '+args[0]+'["'+a+'"]'+op+'"'+values[v]+'"'
                    v+=1
                bdy+=":"
                self.addi(bdy)
        else:
            if len(args)==2:
                code="""
$p if "$at1" in $a0 and $at2 in $a1 and $a0["$at1"] $oper $a1["$at2"]:"""
                self.patron(code,at1=att1,at2=att2,a0=args[0],a1=args[1],oper=op)
                self.incremente()
            else:
                msg="Error in a 'test'. Checks the arguments"
                raise SAXParseException(msg,ValueError,self._locator)             

    def testnot(self,attrs):
        '''The same as test but tests on the negative side of the expression'''
        try:
            op=attrs["op"]
        except:
            op="=="
        
    
        args=[]
        attributes=[]
        values=[]
        liste=self.splitattribute("args",attrs,args,"")
        self.typetest.append(2)
        self.retour.append(self.getincrement())
        self.argument=args[0]
        if liste==True:
            self.addi('if '+self.listeargument+'==():')
            return

        self.splitattribute("attribute",attrs,attributes,"")
        att1=attributes[0]
        if len(attributes)==1:
            att2=attributes[0]
        else:
            att2=attributes[1]

        if len(args)==1:
            self.splitattribute("value",attrs,values,"")
            if len(values)==0:
                self.splitattribute("num",attrs,values,"")
            if len(values)!=len(attributes):
                msg="Mismatch between attribute and value"
                raise SAXParseException(msg,ValueError,self._locator)
            if len(attributes)==1:
                code="""
$p if "$att" not in $arg or not $arg["$att"] $oper "$val":"""
                self.patron(code,att=att1,arg=args[0],val=values[0],oper=op)
                self.incremente()
            else:
                bdy="if "
                v=0
                for a in attributes:
                    if v!=0:
                        bdy+=" or "
                    bdy+='"'+a+'" not in '+args[0] +' or not '+args[0]+'["'+a+'"]'+op+'"'+values[v]+'"'
                    v+=1
                bdy+=":"
                self.addi(bdy)
        else:
            if len(args)==2:
                code="""
$p if "$at1" not in $a0 or "$at2" not in $a1 or not $a0["$at1"] $oper $a1["$at1"]:"""
                self.patron(code,at1=att1,at2=att2,a0=args[0],a1=args[1],oper=op)
                self.incremente()
            else:
                msg="Error in a 'testnot'. Checks the arguments"
                raise SAXParseException(msg,ValueError,self._locator)             


    def generate(self,attrs):
        '''
        Generate a node at the sentence level
        <generate args="node"/>
        '''
        if "args" in attrs:
            code="""
$p try:
$p   _root=$arg["root"]
$p   _root.remove($arg)
$p   $arg.pop("root")
$p except:
$p   pass
$p $arg["root"]=$ph["sub"]
$p $ph["sub"].append($arg)"""
            args=[]
            self.splitattribute("args", attrs, args,"")
            if len(args)==0:
                raise SAXParseException("Wrong number of args in GENERATE",ValueError,self._locator)
            for ag in args:
                self.patron(code,arg=ag,ph=self.phrs[-1])
        else:
            msg="Missing argument node in a 'generate'"
            raise SAXParseException(msg,ValueError,self._locator)             
                 
    def sub(self,attrs):
        '''
        adds a node at the sub level:
        child is added under parent
        <sub args="parent child1 child2 child3"/>
        '''
        self.phrs.append("x"+str(self.ix))
        self.ix+=1
        if "args" in attrs:
            args=[]
            self.splitattribute("args",attrs,args,"")
            if len(args)<2:
                raise SAXParseException("Wrong number of args in SUB",ValueError,self._locator)
            code="""
$p try:
$p   _root=$arg1["root"]
$p   _root.remove($arg1)
$p   $arg1.pop("root")
$p except NameError:
$p   print "Unknown argument in a sub: $arg1"
$p except:
$p   pass
$p try:
$p   _sub=$arg0["sub"]
$p except:
$p   _sub=[]
$p   $arg0["sub"]=_sub
$p $arg1["root"]=_sub
$p _sub.append($arg1)
$p if "root" not in $arg0:
$p    $arg0["root"]=phr["sub"]
$p    phr["sub"].append($arg0)"""
            for i in args[1:]:
                self.patron(code,arg0=args[0],arg1=i,ph1=self.phrs[-1],ph2=self.phrs[-2],ph=self.phrase)
        else:
            msg="Missing argument node in a 'sub'"
            raise SAXParseException(msg,ValueError,self._locator)             

    def minor(self,attrs):
        '''
        Adds a node in the minor section of another node
        <minor args="parent child"/>
        '''
        if "args" in attrs:
            args=[]
            self.splitattribute("args",attrs,args,"")
            if len(args)<2:
                raise SAXParseException("Wrong number of args in MINOR",ValueError,self._locator)
            code="""
$p try:
$p   _root=$arg2["root"]
$p   _root.remove($arg2)
$p   $arg2.pop("root")
$p except NameError:
$p   print "Unknown argument in a minor: $arg2"
$p except:
$p   pass
$p try:
$p   _minor=$arg1["minor"]
$p except:
$p   _minor=[]
$p   $arg1["minor"]=_minor
$p $arg2["root"]=_minor
$p _minor.append($arg2)
$p if "root" not in $arg1:
$p    $arg1["root"]=phr["sub"]
$p    phr["sub"].append($arg1)"""
            for i in args[1:]:
                self.patron(code,arg1=args[0],arg2=i,ph=self.phrase)            
        else:
            msg="Missing argument node in a 'minor'"
            raise SAXParseException(msg,ValueError,self._locator)             

    def major(self,attrs):
        '''
        Adds a node in the minor section of another node
        <minor args="parent child"/>
        '''
        if "args" in attrs:
            args=[]
            self.splitattribute("args",attrs,args,"")
            if len(args)<2:
                raise SAXParseException("Wrong number of args in MAJOR",ValueError,self._locator)
            code="""
$p try:
$p   _root=$arg2["root"]
$p   _root.remove($arg2)
$p   $arg2.pop("root")
$p except NameError:
$p   print "Unknown argument in a major: $arg2"
$p except:
$p   pass
$p try:
$p   _major=$arg1["major"]
$p except:
$p   _major=[]
$p   $arg1["major"]=_major
$p $arg2["root"]=_major
$p _major.append($arg2)
$p if "root" not in $arg1:
$p    $arg1["root"]=phr["sub"]
$p    phr["sub"].append($arg1)"""
            for i in args[1:]:
                self.patron(code,arg1=args[0],arg2=i,ph=self.phrase)            
        else:
            msg="Missing argument node in a 'MAJOR'"
            raise SAXParseException(msg,ValueError,self._locator)             

            
    def concat(self,attrs):        
        '''
        Concatenation of a string to the value of an attribute
        <concat args="x" attribute="a">string</concat>
        '''
        if "args" in attrs:
            args=[]
            self.splitattribute("args", attrs, args,"")
            if len(args)!=1:
                raise SAXParseException("Wrong number of args in CONCAT",ValueError,self._locator)
            self.argument=args[0]            
            self.attribute=attrs["attribute"]
        else:
            msg="Missing attribute in a 'concat'"
            raise SAXParseException(msg,ValueError,self._locator)             
            
    
    def otherwise(self,args):
        '''
        This method should be used within a test or a testnot:
        <test...>
            ...
            <otherwise>
            ...
            </otherwise>
        </test>
        It is the equivalent of a else
        '''
        self.setincrement(self.retour[-1])
        self.addi("else:")

    def inc(self,attrs):
        '''
        Increment a num attribute in a node
        <inc args="xxx" attribute="a" step=2/>
        step is optional
        '''
        try:
            ite=self.getargs(attrs)
        except:
            ite=self.item
        try:
            i=attrs["step"]
        except:
            i=1
        try:
            s=ite+'["'+attrs["attribute"]+'"]+='+i
            self.add(s)
        except:
            print "Missing arguments in inc"

    def dec(self,attrs):
        '''
        Decrement a num attribute in a node
        <dec args="xxx" attribute="a" step=2/>
        step is optional
        '''
        try:
            ite=self.getargs(attrs)
        except:
            ite=self.item
        try:
            i=attrs["step"]
        except:
            i=1
        try:
            s=ite+'["'+attrs["attribute"]+'"]-='+i
            self.add(s)
        except:
            print "Missing arguments in dec"

###########################################################################
       
    def e_test(self):
        self.setincrement(self.retour[-1])
        self.retour.pop()
        self.typetest.pop()

    def e_testnot(self):
        self.setincrement(self.retour[-1])
        self.retour.pop()
        self.typetest.pop()

    def e_create(self):
        self.add('self.initfeatures('+self.argument+')')
    
    def e_sub(self):
        self.phrs.pop()

    def e_otherwise(self):
        self.decremente()

    def endElement(self, name):
        "fonction appelee lorsque le parser rencontre une balise fermante"
        self.depth-=1
        x="self.e_"+name+"()"        
        try:
            eval(x)
        except:
            pass
        if len(self.procedures)!=0 and self.depth==1:
            self.setincrement(self.procedures.pop())
            self.phrs.pop()
            self.noms.pop()
            
        self.ancestors.pop()


    def c_concat(self,chrs):
        self.addi("try:")
        self.add(self.argument+'["'+self.attribute+'"]'+'+="'+chrs+'"')
        self.adddi("except:")
        self.add("pass")
        self.decremente()

    def characters(self, chrs):
        "fonction appelee lorsque le parser rencontre des donnees dans un element"        
        try:
            x="self.c_"+self.ancestors[-1]+"(chrs)"
            eval(x)
        except:
            pass       
               
    def endDocument(self):
        "fonction appelee lorsque le parser rencontre le dernier element"
        pass

####################################################################################################################
####################################################################################################################
####################################################################################################################

class itemgeneration(ContentHandler,handleattribute):
    "Document Handler personnel"
    def __init__(self):
        "initialisation"
        #A makup tag under the root might be transformed into a function
        #A function is associated to a return argument
        self.xmlvalues=["items","value","attribute","class"]
        self.xmlattributes=["attribute","value","num","args","index","comment"]
        handleattribute.__init__(self)
        self.ancestors=[]
        self.attribut=""
        self.item=""
        self.phrase=""
        self.nomitem=""
        self.theclass=""
        self.depth=0
      
    def startDocument(self):
        "fonction appelee lorsque le parser rencontre le premier element"
        pass

    def incremente(self):
        self.indentation.append(len(self.indentation)+1)

    def decremente(self):
        self.indentation.pop()

    def entree(self,attrs):
        self.nomitem=attrs["index"]        
        self.item=self.attribut+'["'+self.nomitem+'"]'
        self.add(self.item+'={}')
        if "attribute" in attrs:
            self.getmultiplevalues("ENTREE",self.item,attrs)
        
    def graph(self,attrs):
        self.theclass=attrs["class"]
        self.add("class "+self.theclass+":")
        self.incremente()
        self.add("def __init__(self):")
        self.incremente()
        self.attribut="self."+attrs["items"]
        self.add(self.attribut+"={}")

    def variable(self,attrs):
        try:
            self.item="self."+self.getargs(attrs)            
            self.add(self.item+'={}')
        except:
            print "Error in a variable definition",attrs
            
    def set(self,attrs):
        try:
            ite=self.getargs(attrs)
        except:
            ite=self.item
        try:
            s=ite+'["'+attrs["attribute"]+'"]="'+attrs["value"]+'"'
        except:
            s=ite+'["'+attrs["attribute"]+'"]='+attrs["num"]
        self.add(s)

            
    def use(self,attrs):
        n=attrs["index"]
        s=self.item+'["'+n+'"]="@"'
        self.add(s)
      
    def startElement(self, name, attrs):
        "fonction appelee lorsque le parser rencontre une balise ouvrante"
        for s in attrs.keys():
            if s not in self.xmlvalues:
                msg="Unknown XML attribute '"+s+"' in XML markup tag: '"+name+"'"
                raise SAXParseException(msg,ValueError,self._locator)
        
        #If there is a comment attribute in the XML node, we process it and generate a comment
        #into the final source code.
        if "comment" in attrs:
            self.add('#'+attrs["comment"])

        if self.depth==0:
            self.xmlvalues=self.xmlattributes
            
        self.depth+=1
        eval("self."+name+"(attrs)")
                
        self.ancestors.append(name)
          

    def endElement(self, name):
        "fonction appelee lorsque le parser rencontre une balise fermante"
        self.ancestors.pop()

    def characters(self, chrs):
        "fonction appelee lorsque le parser rencontre des donnees dans un element"
        pass
              
    def endDocument(self):
        "fonction appelee lorsque le parser rencontre le dernier element"

####################################################################################################################
####################################################################################################################
####################################################################################################################

class plangeneration( rulegeneration ):
   """
permet de generer la classe plan
   """

   def endDocument(self):
       self.body+="\n"
       self.body+=self.initbody
       
  
   def __init__(self):
       rulegeneration.__init__(self)
       self.initbody=""
       self.premiersentence=0
       
   def use(self,attrs):
       arg=self.getarg(attrs)
       try:           
           self.addi('if "@" not in '+arg+':')
           self.add(arg+'["@"]=[]')
           self.decremente()
           l=arg+"['@'].append('"+attrs["attribute"]+"')"
           self.add(l)
       except:
           pass 


   def entree( self, attrs ):
        try:            
            arg=self.getarg(attrs)            
            self.argument=arg
            self.add(arg+'={"index":"'+self.replace(attrs["index"])+'","lemma":"'+self.replace(attrs["index"])+'"}' )            
        except:
            print "Erreur dans entree dans plan"                             
        if "attribute" in attrs:                   
            self.getmultiplevalues("ENTREE",self.argument,attrs)
   
   def sentence(self,attrs):
       self.lastposition=self.getincrement()
       self.add("def sentence"+attrs["num"]+"(self,rules):")
       if self.premiersentence<=1:
           self.initbody+="   self.sentence"+attrs["num"]+"(rules)\n"
           self.initbody+="   self.display()\n"
           self.initbody+="   self.displaysentence()\n"           
       else:
           self.initbody+="   #self.sentence"+attrs["num"]+"(rules)\n"
       self.premiersentence+=1
       self.incremente()
       self.add(self.phrase+"={'sub':[]}")
   
   def e_sentence(self):
       self.setincrement(self.lastposition)

   def cree( self, name, attrs ):
       if self.premier==True:
           self.premier=False
           rules="""
           
  def display(self):
      self.recursivedisplay($phr,1)
      print


  def recursivedisplay(self,phr,indent):
      if "sub" not in phr:
          return
      for i in phr["sub"]:
          try:
            _min=i['minor']
            for j in _min:             
             if "$level" not in j or j["$level"]=="$lex":
               pad="".zfill(indent).replace('0',' ')
               print pad+'_'+j["lemma"]               
             if "sub" in j:
                self.recursivedisplay(j,indent+1)
            _maj=i['major']
            for j in _maj:             
             if "$level" not in j or j["$level"]=="$lex":
               pad="".zfill(indent).replace('0',' ')
               print pad+'-'+j["lemma"]               
             if "sub" in j:
                self.recursivedisplay(j,indent+1)
          except:
            pass
          if "$level" not in i or i["$level"]=="$lex":
             pad="".zfill(indent).replace('0',' ')
             print pad+i["lemma"]                         
          self.recursivedisplay(i,indent+1)  

  def displaysentence(self):
      self.recursivedisplaysentence($phr)
      print


  def recursivedisplaysentence(self,phr):
      if "sub" not in phr:
          return
      for i in phr["sub"]:
          try:
            _min=i['minor']
            for j in _min:             
             if "$level" not in j or j["$level"]=="$lex":
               print j["lemma"],               
             if "sub" in j:
                self.recursivedisplaysentence(j)
            _maj=i['major']
            for j in _maj:             
             if "$level" not in j or j["$level"]=="$lex":
               print j["lemma"],               
             if "sub" in j:
                self.recursivedisplaysentence(j)
          except:
            pass
          if "$level" not in i or i["$level"]=="$lex":
             print i["lemma"],
          self.recursivedisplaysentence(i)  

"""           
           try:
               self.phrase="self."+attrs["sentence"]
               self.items=attrs["items"]
               self.bar=attrs["attribute"]
               self.lexical=attrs["value"]
               self.add("\n\nclass "+name+":")
               self.incremente()
               self.patron(rules,ontologie=self.items, phr=self.phrase, level=self.bar, lex=self.lexical)
               self.initbody="  def __init__(self):\n"
               self.initbody+=('   '+self.phrase+"={'sub':[]}\n")
               self.initbody+=('   '+"rules="+attrs["class"]+"()\n")
           except:
               print "erreur sur plan"
       else:
               rules="""
$p try:
$p    rules.$nom($phr,$feat$bd)
$p except:
$p    print "Error in call to $nom with: ",$argstr
"""
               self.add("_"+name+"={}")
               if "attribute" in attrs:                   
                   self.getmultiplevalues(name,"_"+name,attrs)
               args=[]
               self.splitattribute("args", attrs, args,"")
               bdy=""
               for i in args:
                   bdy+=","
                   bdy+=i
               start=True
               msg=""
               for i in args:
                   if start==False:
                       msg+=","
                   start=False
                   msg+=i
               self.patron(rules,feat="_"+name,nom=name,phr=self.phrase,bd=bdy,argstr=msg) 

def rules(chemin):
    doc=rulegeneration()
    saxparseur=make_parser()
    saxparseur.setContentHandler(doc)

    sour=open(chemin,"r")
    try:
        saxparseur.parse(sour)
    except SAXParseException,x:
        print "Erreur XML:",x        
    return doc.body

def plan(chemin):
    doc=plangeneration()
    saxparseur=make_parser()
    saxparseur.setContentHandler(doc)
    sour=open(chemin,"r")
    try:
        saxparseur.parse(sour)
    except SAXParseException,x:
        print "Erreur XML:",x
    return doc.body

def items(chemin):
    doc=itemgeneration()
    saxparseur=make_parser()
    saxparseur.setContentHandler(doc)

    sour=open(chemin,"r")
    try:
        saxparseur.parse(sour)
    except SAXParseException,x:
        print "Erreur XML:",x
    return doc.body


   
