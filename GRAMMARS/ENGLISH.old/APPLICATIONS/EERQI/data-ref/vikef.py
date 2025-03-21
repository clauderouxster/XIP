Python:

vikef_process_sentence(#1, string inputfilename, string parListIds, int parFontSize, int parTop, python Result).


import sys
import string
import re


###
class MyNode:
    def __init__(self, xipnode, sentnb):
        self.sentenceNb = sentnb
        self.loffset = xipnode.left
        self.roffset = xipnode.right
        self.charloffset = xipnode.leftoffset
        self.charroffset = xipnode.rightoffset
        self.nodeNumber = xipnode.index
        self.form = ''
        self.lemma = xipnode.lemma
        self.pos = xipnode.pos
        xipnode.features = xipnode.features.strip('[')
        xipnode.features = xipnode.features.strip(']')
        xipnode.features = xipnode.features.replace('!','')
        self.features = ',' + xipnode.features
        self.daughter = None
        self.parent = None
        self.next = None
        self.previous = None
        self.props = []  # for unary dependencies
        self.deps = []
        self.insideQuotes = False
        self.saidBy = None
        self.anaphorLinks = []
        self.lastAnaphor = None
        self.antecedentLink = None
        self.antecedentLinks = []
        self.salience = 0
        self.partOfAntecedent = None
        self.partOfAnaphor = None
        self.isAnaphoric = False
        self.referentSalience = 0
        self.referentId = 0
        self.id = self.buildIdFromNode(sentnb)
        self.abbr = None
        self.rule=""
        
    # build a node id, n is a XipNode
    def buildIdFromNode(self,sentnb):    
        nid = str(sentnb) + ":" + str(self.loffset);
        if not ( self.loffset == self.roffset ):
            nid = nid + "-" + str(self.roffset)
        nid = nid + ":" + str(self.nodeNumber)
        return nid
       
    def addProp(self,mydepname):
        self.props.append(mydepname)
        # print "##AJOUT",mydepname
        
    def addDep(self,mydep):
        if (not mydep in self.deps):
            self.deps.append(mydep)
            

    def isTerminal(self):
        return (self.daughter == None)
        
    def isPreTerminal(self):
        return ( self.unifyfeats("terminal") and (self.daughter != None) )
        
    def preceedsInText(self,node):
        if ( (self.sentenceNb < node.sentenceNb) or (self.roffset < node.loffset)):
            return True
        else:
            return False
            
    def getSafeForm(self):
        safeform = self.form.replace('\'','\\\'')
        safeform = safeform.replace('"','\\"')
        return safeform;
        
    # add anaphor
    def addAnaphor(self,alink):
        #if (self.anaphorLinks == []):
            # first anaphor for this node
            #self.markPreTerminalsAs("antec",self)
        # alink.anaphor.markPreTerminalsAs("anaphor",alink.anaphor)
        self.anaphorLinks.append(alink)
        self.lastAnaphor = alink.anaphor
        
    # set "saidBy" attribute for this node and all its subnodes
    def setSaidBy(self,n):
        if (self.insideQuotes):
            self.saidBy = n
        subnode = self.daughter
        while (subnode != None):
            subnode.setSaidBy(n)
            subnode = subnode.next
            
    def setSaidByUp(self,n):
        if (self.insideQuotes):
            self.saidBy = n
        supernode = self.parent
        if (supernode != None and supernode.saidBy==None):
            supernode.setSaidByUp(n)
        
    
    # Feature validation
    def unifyfeats(self,featstring):
        feats = featstring.split(',')
        if ( (len(feats)==1 ) and (feats[0]=='') ):
            return True
        for fv in feats:
            fv = fv.strip('!')
            temp = fv.split(':')
            f = temp[0]
            if (len(temp)==2):
                v = temp[1]
            else:
                v = "+"
            if (v == "~"):
                if ( self.features.find( "," + f + ":" ) != -1 ):
                    return False
            else:
                if ( self.features.find( "," + f + ":" + v) == -1 ):
                    return False
        return True
        
        
    def hasProp(self,name):
        for p in self.props:
            if (p.name==name):
                return True
        return False
        
    
    def isDependentOf(self,n2,depname=""):
        for d in self.deps:
            if ( (d.args[0]==n2) and (len(d.args)==2) and (self==d.args[1])):
                if (depname == ""):
                    return True
                elif ( d.name == depname):
                    return d.args[0]
        return False        
        
        
    def isHeadOf(self,n2,depname=""):
        for d in self.deps:
            if ( (d.args[0]==self) and (len(d.args)==2) and (n2==d.args[1])):
                if (depname == ""):
                    return True
                elif (d.name == depname):
                    return d.args[0]
        return False        
        
        
        
    def getDependent(self,depname,features=""):
        for d in self.deps:
            if ( (d.name==depname) and (self==d.args[0]) and (len(d.args)==2) ):
                if (features == ""):
                    return d.args[1]
                elif ( d.args[1].unifyfeats(features) ):
                    return d.args[1]
        return None
        
    def getDependents(self,depname,features=""):
        dependents = []
        for d in self.deps:
            if ( (d.name==depname) and (self==d.args[0]) and (len(d.args)==2) ):
                if (features == ""):
                     dependents.append(d.args[1])
                elif ( d.args[1].unifyfeats(features) ):
                     dependents.append(d.args[1])
        return dependents
        
        
    def getHead(self,depname,features=""):
        for d in self.deps:
            if ( (d.name==depname) and (len(d.args)==2) and (self==d.args[1])):
                if (features == ""):
                    return d.args[0]
                elif ( d.args[0].unifyfeats(features) ):
                    return d.args[0]
        return None
        
        
    def hasSameFormAs(self,n):
        if (self.form == n.form):
            return 1
        else:
            return None

    def hasSameLemmaAs(self,n):
        if (self.lemma == n.lemma):
            return 1
        else:
            return None
            
    def getAbbr(self):   
        if ( self.abbr != None ):
            return self.abbr
            
        abbr = ""
        if (self.isTerminal()):
            abbr = ""
        elif (self.isPreTerminal()):
            if ( self.pos == "DET" or self.pos == "PREP"):
                abbr = ""
            else:
                form = self.form
                for i in range(0,len(form)-1):
                    if ('A' <= form[i] <= 'Z' ):
                        abbr = abbr + form[i]
        else:
            node = self.daughter
            while (node != None):
                abbr = abbr + node.getAbbr()
                if ( (node.next == None) and node.getAbbr()=='' ):
                    # last element and no Uppercase letter
                    # this node has no valid ABBR
                    abbr = ''
                node = node.next
                
        self.abbr = abbr
        return abbr
        
    def matchesAbbr(self,form):
        self.getAbbr()
        if ( (self.abbr == None) or (len(self.abbr) <= 1) ):
            return False
        elif (self.abbr == form):
            return True
        else:
            i = 0
            j = 0
            while ( (i< len(self.abbr)) and (j<len(form)) ):
                if (self.abbr[i] != form[j]):
                    if ( (i == 0) or (form[j] != '.') ):
                        return False
                    else:
                        j = j + 1
                else:
                    i = i + 1
                    j = j + 1
            
            if ( i==len(self.abbr) and ( j==len(form) or (j==(len(form)-1) and form[j]=='.') ) ):
                return True
            else:
                return False
        
        
    def isAbbr(self):
        # ??? (node.unifyfeats("acron") or node.unifyfeats("abr") )
        if (self.form.isupper() and len(self.form)<=6 ):
            return True
        else:
            return False
            
            
    def numberAgreement(self,n2):
        self.rule = "Agreement in Number"
        if ( self.unifyfeats("sg:+,pl:~") and n2.unifyfeats("pl:+,sg:~") ):
            self.rule = self.rule + ": n1 is sg and n2 is pl"
            return False
        if ( self.unifyfeats("pl:+,sg:~") and n2.unifyfeats("sg:+,pl:~") ):
            self.rule = self.rule + ": n1 is pl and n2 is sg"
            return False
        return True


    def genderAgreement(self,n2):
        self.rule = "Agreement in Gender"
        if ( self.unifyfeats("masc:+,fem:~") and n2.unifyfeats("fem:+,masc:~") ):
            self.rule = self.rule + ": n1 is masc and n2 is fem"
            return False
        elif ( self.unifyfeats("fem:+,masc:~") and n2.unifyfeats("masc:+,fem:~") ):
            self.rule = self.rule + ": n1 is fem and n2 is masc"
            return False
        return True


    def personAgreement(self,n2):
        self.rule = "Agreement in Person"
        if ( self.unifyfeats("p3:+") and ( n2.unifyfeats("p1:~,p2:~,p3:~") or n2.unifyfeats("p3:+") ) ):
            return True
        elif ( n2.unifyfeats("p3:+") and ( self.unifyfeats("p1:~,p2:~,p3:~") or self.unifyfeats("p3:+") ) ):
            return True
        elif (self.unifyfeats("p1:~,p2:~,p3:~") and n2.unifyfeats("p1:~,p2:~,p3:~")):
            return True
        elif ( self.unifyfeats("p1:+") and n2.unifyfeats("p1:~") ):
            return False
        elif ( self.unifyfeats("p2:+") and n2.unifyfeats("p2:~") ):
            return False
        elif ( n2.unifyfeats("p1:+") and self.unifyfeats("p1:~") ):
            return False
        elif ( n2.unifyfeats("p2:+") and self.unifyfeats("p2:~") ):
            return False
        return True
            
    def quotationPersonAgreement(self,n2):
        # if ( n2.unifyFeats("guille:+,p1:+") and self.
        return False
        
        
        
    def lexicalSemanticsAgreement(self,n2):
        self.rule = "Lexical/semantic agreement: "
        if (self.unifyfeats("person") ):
            if (n2.unifyfeats("c_person:~,person:~") and n2.unifyfeats("pron,pl:~") ):
                # n2 not person and n2 is not "they"
                self.rule = self.rule + "n1 is person, n2 is not"
                return False
        elif (n2.unifyfeats("person") or n2.unifyfeats("c_person") ):
            if (self.unifyfeats("person:~")):
                self.rule = self.rule + "n1 is not person, n2 is"
                return False
        return True
        

    def syntacticAgreement(self,n2):
        
        # this is for debugging
        debug = False
        if (self.id == '1:23-24:91' and n2.id=='1:25:50'):
            print "HAHA: ",self.id,': ',self.antecedentLinks
            debug = True
            
        # if self has anaphors, then the syntactic agreement with n2 should be
        # tested with the last anaphoric link of self
        if (self.anaphorLinks == []):
            antec = self
        else:
            antec = self.anaphorLinks[len(self.anaphorLinks)-1].anaphor
            
            
        dist = n2.loffset-antec.roffset
        if (debug):
            print "DIST=",dist
            
        if ( antec.sentenceNb == n2.sentenceNb ):
            if ( fabs(dist) <= 1 ):
                self.rule = "Distance between n1 and n2 must be >= 2"
                return False
            elif ( (dist < 0 ) and not antec.isHeadOf(n2,"CATAPHOR") ):
                self.rule = "No potential cataphora between n1 and n2"
                return False
        
        verb = n2.getHead("OBJ")
        if ( n2.unifyfeats("refl:~") ):
            if ( (verb != None) and verb.isHeadOf(antec,"SUBJ-N") ):
                self.rule = "If n2 is not REFL:+, then n2 cannot be the OBJ of the verb of which n1 is the SUBJ"
                return False
        else:
            if ( (verb == None) or (not verb.isHeadOf(antec,"SUBJ-N")) ):
                self.rule = "If n2 is REFL:+, then n2 must be the OBJ of the verb of which n1 is the SUBJ"
                return False
        
        self.rule = "n2 cannot be a dependent of the main verb of a relative clause that modifies n1"
        #liste = [ ["$MOD",self,"x"], ["$MOD","x","y:pron"], ["$?","x",n2]]
        depverb = antec.getDependent("MOD", "verb")
        if ( depverb != None ): 
            pronrel = depverb.getDependent("MOD", "pron,rel")
            if ( (pronrel != None) and n2.isDependentOf(depverb) ):
                return False
        
        return True
        
        
    def markPreTerminalsAs(self, markstr, node):
        if (self.isPreTerminal()):
            n = self
            if (markstr == "antec"):
                n.partOfAntecedent = node
            elif (markstr == "anaphor"):
                n.partOfAnaphor = node
            elif (markstr == "anaphoric"):
                n.isAnaphoric = True 
            if ( (node != None) and (self != node) ):
                # we are processing the subnodes (marking "anaphoric")
                n = self.next
                while (n != None):
                    if (markstr == "antec"):
                        n.partOfAntecedent = node
                    elif (markstr == "anaphor"):
                        n.partOfAnaphor = node
                    elif (markstr == "anaphoric"):
                        n.isAnaphoric = True
                    n = n.next
        elif (not self.isTerminal()):
            self.daughter.markPreTerminalsAs(markstr, node)
        
        
        
    def myprint(self):
        print (self.id+("/")).ljust(11),self.form
        if (self.saidBy):
            print " ".ljust(4),"SAID-BY:",self.saidBy.form,self.saidBy.id
        if (self.insideQuotes):
            print " ".ljust(4),"Inside QUOTES!"
        print " ".ljust(4),self.pos,self.features
        for d in self.props:
            print " ".ljust(5),d
        for d in self.deps:
            print " ".ljust(5),d.name,"(",d.args[0].id,d.args[0].form,", ",d.args[1].id,d.args[1].form,")"
        #if (self.antecedentLink != None):
        #    print " ".ljust(4),"ANTECEDENT =", #self.antecedentLink.antec.id,self.antecedentLink.antec.form
        print ""



##############
class MyDep:
    def __init__(self, name, myargs):
        self.name = name
        self.args = myargs

    def myprint():
        print self.name,self.args[0].id,self.args[1].id




        
#########################################################

class SentenceParse:
    def __init__(self, itop, sentnb):
        self.sentNb = sentnb
        self.nodes = {}
        self.root = self.addNode(itop, sentnb, None, None)
        self.props = []
        self.deps = []
        self.preterminals = []
        self.sentInsideQuotes = True        

        deps = xips.dependencyset("?")
        for d in deps:
            self.addDependency(d)
            
        
        #print 'HAHA ======== ', len(self.preterminals)
        
        
        
        
    def addNode(self,i,sentnb,parent,previous):
        if (i == -1):
            return None
        xipnode = XipNode(i)
        node = MyNode(xipnode,sentnb)
        # print "## Store node:",i,node.pos,node.form
        if (xipnode.daughter >= 0):
            self.nodes[i] = node
        node.parent = parent
        node.previous = previous
        
        # print "## xipnode.daughter ",xipnode.daughter
        if (xipnode.daughter >= 0):
            node2 = self.addNode(xipnode.daughter,sentnb,node,None)
            node.daughter = node2
            node.form = node2.form
            n = node2.next
            while (n!=None):
                if (node2.charroffset < n.charloffset):
                    node.form = node.form + ' ' + n.form
                else:
                    node.form = node.form + n.form
                node2 = n
                n = n.next
        
        else:
            node.daughter = None
            node.form = xipnode.surface
            
        if (node.isPreTerminal()):
            self.preterminals.append(node)
            if (node.unifyfeats("guille")):
                node.insideQuotes = True
            else:
                self.sentInsideQuotes = False
            
        if (xipnode.next >= 0):
            node2 = self.addNode(xipnode.next,sentnb,parent,node)
            node.next = node2
        elif (parent != None):
            node.parent.last = node
            
        return node
        
    
    def addDependency(self,d):
        xipdep = XipDependency(d)
        myargs  = []
        for i in xipdep.parameters:
            # xn = XipNode(i)
            if (i in self.nodes):
                # regular node (belongs to the parse tree) i,xn.left,xn.pos,xn.surface,xipdep.name,len(xipdep.parameters)
                node = self.nodes[i]
            else:
                # dummy node
                xipnode = XipNode(i)
                node = MyNode(xipnode,self.sentNb)
                
            myargs.append(node)
        if (xipdep.name == "SAY"):
            #if (len(myargs)<=1):
                #print 'HAHA: ',myargs[0].form,myargs[0].id
            myargs[1].setSaidBy(myargs[0])
            # return
        mydep = MyDep(xipdep.name,myargs)
        if (len(myargs)==1):
            # Unary dep ==> prop
            myargs[0].addProp(mydep.name)
            self.props.append(mydep)
        else:
            self.deps.append(mydep)
            for arg in myargs:
                arg.addDep(mydep)

    # A function to compare two nodes (to sort a list of nodes)
    def compareNodes(self,i1,i2):
        n1 = self.nodes[i1]
        n2 = self.nodes[i2]
        # print "COMPARE:",n1.id,n2.id
        if (n1.sentenceNb > n2.sentenceNb):
            return 1
        elif ( n1.sentenceNb == n2.sentenceNb ):
            if (n1.loffset > n2.loffset):
                return 1
            elif (n1.loffset == n2.loffset):
                if (n1.roffset < n2.roffset):
                    return 1
                elif (n1.roffset == n2.roffset):
                    # first the Upper node
                    if (n1.nodeNumber < n2.nodeNumber):
                        return 1
                    else:
                        return -1
                else:
                    return -1
            else:
                return -1
        else:
            return -1
            
            
    # returns the (first) unary dep if it exists, None otherwise
    def getProp(self,depname):
        for p in self.props:
            if (p.name==depname):
                return p
        return None
    
    # print a sentence
    def myprint(self):
        print "SENTENCE:",self.sentNb,"------------------------------------------"
        keys = self.nodes.keys()
        keys.sort(self.compareNodes)
        
        for k in keys:
            node = self.nodes[k]
            #node.myprint()
            #print node.id,"(",node.pos,"): ",node.form
            
####################################################################"

# A class to store information on the document structure
# i.e. information on the PARAGRAPH element of harmonized documents

class StructInfo:
    def __init__(self,parListIds,parFontSize,parTop):
        self.Number = 0
        self.listIds = parListIds
        self.fontSize = parFontSize
        self.top = parTop



####################################################################"

# ATTENTION ! ATTENTION !!
# The following class provides rules (actually methods) to recognize
# some entities and relations, when this recognition needs not only NLP
# information but also information on the document structure
#
# Therefore, this class/methods may need to be changed when the
# source document collection changes
#


# PostNLP customization modules


def getPostNlpClass(collection_type):
    """ Returns a PostNlp module object depending on the type of the document
    collection to process. A PostNlp module provides methods (rules) to
    recognize entities/relations when both NLP and information on the
    document structure is needed. """
    
    if (collection_type == 'GENERIC'):
        return PostNlpGeneric()
    elif (collection_type == 'CPO'):
        return PostNlpCPO()
    else:
        return PostNlpGeneric()
        


# Generic PostNlp rules
class PostNlpGeneric: 
    """ Generic Vikef PostNlp rules """
    def __init__(self):
        # TITLE info
        self.titleAlreadySelected = False
        self.titleNodes = []
        self.titleParNumber = 0
        self.titleFontSize = 0
        self.titleTop = 0
        self.titleListIds = ''
        self.titleEntity = None
        # AUTHOR info
        self.authorsAlreadySelected = False
        self.lastAuthorInd = 0
        self.authorRelations = []
        
    #
    def processSentence(self, textparse, sentparse, parinfo):
        # process title
        self.processTitle(textparse, sentparse, parinfo)
        self.processAuthor(textparse, sentparse, parinfo)
        textparse.postNlp.processOther(textparse,sentparse,parinfo)
        
    # checks if this is the title
    def processTitle(self,textparse, sentparse, parinfo):
        if (self.titleAlreadySelected):
            return None
        if (textparse.parNumber < 4):
            if (self.titleParNumber == textparse.parNumber):
                # another sentence within the title paragraph
                self.titleNodes.append(sentparse.root)
            elif ( (parinfo.fontSize > self.titleFontSize) or
            (parinfo.top <= self.titleTop) ):
                self.titleFontSize = parinfo.fontSize
                self.titleTop = parinfo.top
                self.titleNodes = [sentparse.root]
                self.titleParNumber = textparse.parNumber
                self.titleListIds = parinfo.listIds
            return None
        elif (self.titleNodes != []):
            # reached the 4th paragaraph, so stop looking for a title and
            # output the one already found
            self.titleAlreadySelected = True
            self.titleEntity = Entity('TITLE_OF', self.titleNodes, self.titleListIds)
            return self.titleEntity
            

    #
    def processAuthor(self,textparse,sentparse,parinfo):
        """check if a PERSON entity is an AUTHOR of current article"""
        if (not textparse.endOfParsing):
            # for Generic docs, compute AUTHOR after all the doc has been parsed
            return None

        if (not self.titleAlreadySelected):
            return None
            
        i = 0
        while ( i < len(textparse.entities) ):
            if (textparse.entities[i].type == 'PERSON' ):
                break
            i = i+1
        if ( i < len(textparse.entities) and
        ( textparse.entities[i].loffset < self.titleEntity.loffset or textparse.entities[i].loffset > self.titleEntity.roffset ) ):
            while ( i < len(textparse.entities) ):
                if (textparse.entities[i].type == 'PERSON'):
                    rel = Relation('AUTHOR_OF','THIS_ARTICLE',textparse.entities[i],'')
                    self.authorRelations.append(rel)
                    
                elif (textparse.entities[i].type != 'REFNUMBER'):
                    self.authorsAlreadySelected = True
                    return self.authorRelations
                i = i+1
        else:
            return None
        
        
    def processOther(self,textparse,sentparse,parinfo):
        pass
        
    # post NLP process on whole doc
    def processPostNlp(self,textparse):
        # process AUTHOR rel
        textparse.postNlp.processAuthor(textparse,None,None)
        
        
    # Printing
    def printAnnotations(self,textparse):
        for e in textparse.entities:
            s = e.getXmlEntityString(textparse.nlpTool,textparse.nlpToolVersion)
            print s
        for r in textparse.relations:
            s = r.getXmlRelationString(textparse.nlpTool,textparse.nlpToolVersion)
            print s
        

###
class PostNlpCPO(PostNlpGeneric):
    """ PostNlp rules suitable for the CPO collection"""
    def __init__(self):
        PostNlpGeneric.__init__(self)
        self.abstractId = None
        self.abstractTypeNodes = []
        self.abstractAuthor = None
        self.authorInstitution = None
        self.authorLocation = None
        self.inSessionSection = False
        self.sessionEntity = None
        self.sessionId = None
        self.sessionTitle = None
        self.sessionDate = None
        self.sessionTime = None
        self.sessionLocation = None
        self.sessionIdPattern = re.compile(r"^([A-Z]+[0-9]*)[\ ]*\-[\ ]*(?:([0-9]+)[\ ]*\:)?$")
        self.abstractTypePattern = re.compile(r"\([ ]*([^)]+)\)[ ]*$")
        self.inSection = 'title:'
        self.authors = []
        self.mainAuthor = None
        self.mainAuthorInstitution = None
    
    def processTitle(self,textparse, sentparse, parinfo):
        if (self.inSection != 'title:'):
            return
            
        m = self.sessionIdPattern.match(sentparse.root.form)
        if ( m ):
            # session Id found, first parag
            grps = m.groups()
            nodes = [sentparse.root.daughter]
            self.sessionId = Entity('STRINGVALUE',nodes,parinfo.listIds,'ID_OF','SESSION')
            textparse.addEntity(self.sessionId)
            self.sessionEntity = Entity('SESSION',nodes,parinfo.listIds)
            textparse.addEntity(self.sessionEntity)
            rel = Relation('ID_OF',self.sessionEntity,self.sessionId)
            textparse.addRelation(rel)
            if (len(grps) == 2):
                nodes = [sentparse.root.daughter.next.next]
                self.abstractId = Entity('STRINGVALUE',nodes,parinfo.listIds,'ID_OF','DOCUMENT')
                textparse.addEntity(self.abstractId)
                rel = Relation('ID_OF',textparse.docEntity,self.abstractId,'')
                textparse.addRelation(rel)
        elif (self.titleEntity == None):
            # first parag, not a session ID, so it's the TITLE
            n = sentparse.root.daughter
            while (n!=None and not (n.next==None and n.pos=='INS') ):
                self.titleNodes.append(n)
                n = n.next
            if (n!=None and n.next==None and n.pos=='INS'):
                n = n.daughter.next
                while (n!=None and not (n.next==None and n.pos=='PUNCT') ):
                    self.abstractTypeNodes.append(n)
                    n = n.next
                if (self.abstractTypeNodes != []):
                    e = Entity('STRINGVALUE',self.abstractTypeNodes,parinfo.listIds,
                    'TYPE_OF','DOCUMENT')
                    textparse.addEntity(e)
                    rel = Relation('TYPE_OF',textparse.docEntity,e)
                    textparse.addRelation(rel)
                
            #self.titleNodes = [sentparse.root]
            self.titleListIds = parinfo.listIds
            self.titleAlreadySelected = True
            self.titleEntity = Entity('STRINGVALUE', self.titleNodes, self.titleListIds, 'TITLE_OF', 'DOCUMENT')
            textparse.addEntity(self.titleEntity)
            rel = Relation('TITLE_OF',textparse.docEntity,self.titleEntity)
            textparse.addRelation(rel)
        elif (sentparse.root.form!='Session:'):
            # we are in the same parag where we found the title
            # check if it's an abstact type between parentheses
            m = self.abstractTypePattern.search(sentparse.root.form)
            if ( m ):
                # first, if the first par of this "sentence" is a part of the title, then collect the nodes and add them to the title entity
                n = sentparse.root.daughter
                while (not (n.pos=='INS' and n.next==None)):
                    self.titleEntity.addNode(n)
                    n = n.next
                self.abstractTypeNodes = []
                if (n!=None and n.next==None and n.pos=='INS'):
                    n = n.daughter.next
                while (n!=None and not (n.next==None and n.pos=='PUNCT') ):
                    self.abstractTypeNodes.append(n)
                    n = n.next
                if (self.abstractTypeNodes != []):
                    e = Entity('STRINGVALUE',self.abstractTypeNodes,parinfo.listIds,
                        'TYPE_OF','DOCUMENT')
                    textparse.addEntity(e)
                    rel = Relation('TYPE_OF',textparse.docEntity,e)
                    textparse.addRelation(rel)
            # anyways, stop processing the title
            self.titleAlreadySelected = True
            
            
    def processSession(self, textparse, sentparse, parinfo):
        if (not self.titleAlreadySelected):
            return
            
        # just to make sure there exists a SESSION entity
        if (self.sessionEntity == None):
            self.sessionEntity = Entity('SESSION',[],parinfo.listIds)
            
        # process
        if (self.inSection == 'session:'):
            if (self.sessionTitle==None):
                self.sessionTitle = Entity('STRINGVALUE', [sentparse.root], parinfo.listIds,'TITLE_OF','SESSION')
                textparse.addEntity(self.sessionTitle)
                if (self.sessionEntity != None):
                    rel = Relation('TITLE_OF',self.sessionEntity, self.sessionTitle)
                    textparse.addRelation(rel)
                    
            elif (self.sessionDate==None):
                e = textparse.getCurrentSentFirstEntity('DATE')
                if (e != None):
                    self.sessionDate = e
                    rel = Relation('DATE_OF',self.sessionEntity, self.sessionDate)
                    textparse.addRelation(rel)
                    
                if (self.sessionTime==None):
                    e = textparse.getCurrentSentFirstEntity('TIME')
                    if (e != None):
                        self.sessionTime = e
                        rel = Relation('TIME_OF',self.sessionEntity, self.sessionTime)
                        textparse.addRelation(rel)
                        
                if (self.sessionLocation==None):
                    dep = sentparse.getProp('ROOM')
                    if (dep != None):
                        self.sessionLocation = Entity('LOCATION', [dep.args[0]], parinfo.listIds,'LOCATION_OF','SESSION')
                        textparse.addEntity(self.sessionLocation)
                        rel = Relation('LOCATION_OF',self.sessionEntity, self.sessionLocation)
                        textparse.addRelation(rel)                    
        else:
            sentform = sentparse.root.form.replace(' ','')
            sentform = sentform.lower()
            if (sentform == 'session:'):
                self.inSection = 'session:'
        
    #
    def processAuthor(self,textparse,sentparse,parinfo):
        """check if a PERSON entity is an AUTHOR of current article"""
        if (not self.titleAlreadySelected):
            return
        if (self.inSection=='abstract:'):
            return
            
        # check if "author" or "co-author" sections
        sentform = sentparse.root.form.replace(' ','')
        sentform = sentform.lower()
        if (sentform == 'author:'):
            self.inSection = 'author:'
        elif (sentform == 'co-author(s):'):
            self.inSection = 'co-author(s):'
        elif (sentform == 'abstract:'):
            self.inSection = 'abstract:'
            # set default ORGs for co-authors when unscpecified
            #if (self.mainAuthorInstitution != None):
                #for e in self.authors:
                 #   if (not e.isArg2In('MEMBER_OF')):
                  #      rel = Relation('MEMBER_OF',self.mainAuthorInstitution,e)
                   #     textparse.addRelation(rel)
            
        # if we are in an Author section and there is a PERSON entity, it's the author
        elif (self.inSection == 'author:' or self.inSection == 'co-author(s):'):
            if (self.inSection=='author:'):
                pref=''
            else:
                pref='CO-'
            if (self.abstractAuthor == None or self.inSection == 'co-author(s):'):
                el = textparse.getSentEntityMatchList(['PERSON'])
                if (el != None):
                    self.authorLocation = None
                    self.authorInstitution = None
                    self.abstractAuthor = el[0]
                    rel = Relation(pref+'AUTHOR_OF',textparse.docEntity,el[0])
                    textparse.addRelation(rel)
                    self.authorInstitution = None
                    self.authorLocation = None
                    self.authors.append(self.abstractAuthor)
                    if (self.inSection == 'author:'):
                        self.mainAuthor = self.abstractAuthor
                    
            elif (self.authorInstitution == None):
                el = textparse.getSentEntityMatchList(['ORGANISATION','ORGANISATION'])
                if (el == None):
                    el = textparse.getSentEntityMatchList(['ORGANISATION'])
                if (el != None):
                    self.authorInstitution = el[0]
                    rel = Relation('MEMBER_OF',el[0],self.abstractAuthor)
                    textparse.addRelation(rel)
                    if (len(el)==2):
                        rel = Relation('PART_OF',el[0],el[1])
                        textparse.addRelation(rel)
                    
                    if (self.inSection == 'author:'):
                        self.mainAuthorInstitution = self.authorInstitution
                        
            elif (self.authorLocation==None):
                el = textparse.getSentEntityMatchList(['LOCATION'])
                if (el != None):
                    self.authorLocation = el[0]
                    rel = Relation('LOCATION_OF',self.authorInstitution,el[0])
                    textparse.addRelation(rel)
                        
                        
            
            
    # post NLP process on whole doc
    def processPostNlp(self,textparse):
        pass


    def processOther(self,textparse,sentparse,parinfo):
        self.processSession(textparse,sentparse,parinfo)
        
        
    # Printing
    def relTranslation(self,rtype):
        tr = rtype.replace('_OF','')
        if (tr=='PTITLE'):
            tr = 'title'
        elif (tr == 'FIRSTNAME'):
            tr = 'name'
        elif (tr=='LASTNAME'):
            tr = 'fam-name'
        return tr
                
                
    def printAnnotations(self,textparse):
        cpourl = textparse.sourceUrl
        p = re.compile('\..+\/')
        cpourl = p.sub('',cpourl)
        cpourl = cpourl.replace('cpo','')
        cpourl = cpourl.replace('.xml','')
        author = None
        authorinstitution = None
        for e in textparse.entities:
            cpotype = ''
            cpovalue = e.content
            cpoid = str(e.id)
            if (e.type == 'SESSION'):
                e2 = e.getDepInRel('ID_OF')
                if (e2 != None):
                    cpotype = 'abstract_session'
                    cpovalue = e2.content
                    cpoid = str(textparse.docEntity.id)
                    print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
                deps = e.getArg1Rels(['TITLE_OF','DATE_OF','TIME_OF','LOCATION_OF'])
                for d in deps:
                    cpoid = str(e.id)
                    cpotype = 'session_' + d.type.replace('_OF','').lower()
                    cpovalue = d.e2.content
                    print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
            elif (e.type == 'DOCUMENT'):
                e2 = e.getDepInRel('ID_OF')
                if (e2 != None):
                    cpotype = 'abstract_ordernumber'
                    cpovalue = e2.content
                    cpoid = str(textparse.docEntity.id)
                    print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
                deps = e.getArg1Rels(['TITLE_OF','TYPE_OF'])
                for d in deps:
                    cpoid = str(e.id)
                    cpotype = 'abstract_' + d.type.replace('_OF','').lower()
                    cpovalue = d.e2.content
                    print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
            elif (e.type == 'PERSON'):
                if (e.isArg2In('AUTHOR_OF')):
                    pref = ''
                elif (e.isArg2In('CO-AUTHOR_OF')):
                    pref = 'co-'
                else:
                    continue
                deps = e.getArg1Rels(['PTITLE_OF','FIRSTNAME_OF','LASTNAME_OF'])
                for d in deps:
                    cpoid = str(e.id)
                    cpotype = pref + 'author_' + self.relTranslation(d.type)
                    cpovalue = d.e2.content
                    print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
                
                e1 = e.getHeadInRel('MEMBER_OF')
                if (e1==None):
                    continue
                cpotype = pref + 'author_' + 'institution'
                cpovalue = e1.content
                print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
                instit = e1
                
                e1 = e1.getDepInRel('PART_OF')
                if (e1 != None):
                    cpotype = pref + 'author_' + 'department'
                    cpovalue = e1.content
                    print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
                    e1 = e1.getHeadInRel('LOCATION_OF')
                    
                if (instit != None):
                    e1 = instit.getDepInRel('LOCATION_OF')
                    if (e1!=None):
                        deps = e1.getArg1Rels(['POSTALCODE_OF','CITY_OF','COUNTRY_OF'])
                        for d in deps:
                            cpotype = pref + 'author_' + d.type.replace('_OF','').lower()
                            cpovalue = d.e2.content
                            print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
                


####################################################################


class Entity:
    """VIKEF entity"""
    def __init__(self, etype, nodes, listIds, arg_of_rel = '', head_type = ''):
        self.id = 0
        self.type = etype
        self.argOfRel = arg_of_rel
        self.headtype = head_type
        self.nodes = nodes
        self.content = ''
        self.sourceUrl = ''
        self.listIds = listIds
        self.arg1rels = []
        self.arg2rels = []
        
        if (nodes != [] and nodes != str(nodes) ):
            self.loffset = nodes[0].loffset
            self.roffset = nodes[len(nodes)-1].roffset
            self.content = nodes[0].form
            i=1
            while (i < len(nodes)):
                if (nodes[i-1].charroffset < nodes[i].charloffset):
                    self.content = self.content + ' ' + nodes[i].form
                else:
                    self.content = self.content + nodes[i].form
                i = i+1
        else:
            self.loffset = 0
            self.roffset = 0
            if ( nodes == str(nodes) ):
                # nodes is a string
                self.content = nodes
        self.coref = None

    def addNode(self,n):
        if (self.nodes==[]):
            self.loffset = n.loffset
            self.roffset = n.roffset
            self.content = n.form
        else:
            if (n.loffset < self.loffset):
                self.loffset = n.loffset
            if (n.roffset > self.roffset):
                self.roffset = n.roffset
            if (self.nodes[len(self.nodes)-1].charroffset < n.charloffset):
                self.content = self.content + ' ' + n.form
            else:
                self.content = self.content + n.form
        self.nodes.append(n)
        
        
    def setId(self,eid):
        self.id = eid
        
    def setCoref(self,e):
        self.coref = e
        self.setId(e.id)
        
    def setSourceUrl(self,sourceUrl):
        self.sourceUrl = sourceUrl
        
    def setArgOfRel(self,arg_of_rel):
        self.argOfRel = arg_of_rel
        
    def setHeadType(self,head_type):
        self.headtype = head_type
        
        
    def getHeadRel(self,rtype):
        if (self.arg2rels == [] or (rtype != '' and self.arg2rels[0].type != rtype)):
            return None
        else:
            return self.arg2rels[0]
            
    def isArg2In(self,rtype,e1=None):
        for r in self.arg2rels:
            if (r.type==rtype and (e1==None or e1==r.e1)):
                return True
        return False
        
        
    def isHeadInRel(self,rel,rtype,e2=None):
        for r in self.arg1rels:
            if (r.type==rtype and (e2==None or e2==r.e2)):
                rel.type = rtype
                rel.e1=self
                rel.e2=r.e2
                return True
        return False

    def isDepInRel(self,rel,rtype,e1=None):
        for r in self.arg2rels:
            if (r.type==rtype and (e1==None or e1==r.e1)):
                rel.type = rtype
                rel.e2=self
                rel.e1=r.e1
                return True
        return False
        
    def getDepInRel(self,rtype):
        for r in self.arg1rels:
            if (r.type==rtype or rtype=='?'):
                return r.e2
        return None

    def getHeadInRel(self,rtype):
        for r in self.arg2rels:
            if (r.type==rtype or rtype=='?'):
                return r.e1
        return None
        
    def getAllDepsInRel(self,rtype):
        deps = []
        for r in self.arg1rels:
            if (r.type==rtype or rtype=='?'):
                deps.append(r.e2)
        return deps

    def getArg1Rels(self,rtypes=None):
        deps = []
        for r in self.arg1rels:
            if (rtypes==None or r.type in rtypes):
                deps.append(r)
        return deps

        
    # creates an XML entity representation from the entity parameters
    def getXmlEntityString(self,nlptool,nlptoolversion):
        return string.joinfields([
        '  <ExtractedData Entry_level_url="',self.sourceUrl,
        '" Entry_level_ids="',self.listIds,'" loffset="',str(self.loffset), '" roffset="',str(self.roffset),'" tool="',nlptool,'" version="', nlptoolversion,'">\n',
        '    <Entity data_id="',str(self.id),'">\n',
        '      <',self.type,'>',self.content,'</',self.type,'>\n',
        '    </Entity>\n',
        '  </ExtractedData>'], '' )
        
        
        
########################

class Relation:
    """VIKEF relation"""
    def __init__(self, rtype, e1, e2, listIds=''):
        self.id = 0
        self.type = rtype
        self.e1 = e1
        self.e2 = e2
        self.listIds = listIds
        
        
    def setId(self,rid):
        self.id = rid
        
        
        
    # creates an XML entity representation from the entity parameters
    def getXmlRelationString(self,nlptool,nlptoolversion):
        return string.joinfields([
        '  <ExtractedData Entry_level_url="',self.sourceUrl,
        '" Entry_level_ids="',self.listIds,'" tool="',nlptool,
        '" version="',nlptoolversion,'">\n',
        '    <Relation data_id="',str(self.id),'">\n',
        '      <RelationType>',self.type,'</RelationType>\n',
        '      <SubjectEntity>',str(self.e1.id),'</SubjectEntity>\n',
        '      <ObjectEntity>',str(self.e2.id),'</ObjectEntity>\n',
        '    </Relation>\n',
        '  </ExtractedData>'], '' )
        
    
    
    
####################################################################

# 

class TextParse:
    def __init__(self, postnlp):
        self.nlpTool = 'XIP'
        self.nlpToolVersion = '9.58'
        self.depEntityTypes = ['PERSON','LOCORG','EMAIL','ORGANISATION', 'URL', 'DATE', 'TIME', 'EVENT', 'LOCATION', 'REFNUMBER', 'AUTHOR','STREET','POSTALCODE','CITY','STATE','COUNTRY','CONTINENT']

        self.postNlp = postnlp
        self.docEntity = None
        self.entities = []
        self.sentEntities = []
        self.currentSentEntityInd = 0
        self.relations = []
        self.entityNb = 0
        self.relationNb = 0
        self.sentParses = []
        self.sentNb = 0
        self.sourceUrl = ''
        self.outputFilename = ''
        self.out = None
        self.currentListIds = ''
        self.parNumber = 0
        self.endOfParsing = False # set to True when end of text is reached
        self.inLocation = None
        
    # reinit
    def reinit(self, postnlp):
        self.postNlp = postnlp
        self.docEntity = None
        self.entities = []
        self.sentEntities = []
        self.currentSentEntityInd = 0
        self.relations = []
        self.entityNb = 0
        self.relationNb = 0
        self.sentParses = []
        self.sentNb = 0
        self.sourceUrl = ''
        self.outputFilename = ''
        self.out = None
        self.currentListIds = ''
        self.parNumber = 0
        self.endOfParsing = False # set to True when end of text is reached
        self.inLocation = None

    #
    def getCurrentSentFirstEntity(self,etype=''):
        i = self.currentSentEntityInd
        while (i < self.entityNb):
            if (self.entities[i].type==etype or etype==''):
                return self.entities[i]
            i = i + 1
        return None
        
        
    def getEntityMatchListFrom(self,ind,etypelist):
        i = ind
        while ( i <= (len(self.entities) - len(etypelist)) ):
            j = 0
            ematchlist = []
            while ( j < len(etypelist) ):
                if (self.entities[i+j].type != etypelist[j]):
                    break
                ematchlist.append(self.entities[i+j])
                j = j + 1
            if ( j == len(etypelist) ):
                return ematchlist
            i = i + 1
        return None
        
    def getEntityMatchList(self,etypelist):
        return self.getEntityMatchListFrom(0,etypelist)
        
    def getSentEntityMatchList(self,etypelist):
        return self.getEntityMatchListFrom(self.currentSentEntityInd,etypelist)
    
    # if node is of type 'etype' and it is an occ of an entity already found
    # then return the first occurrence entity, otherwise return None
    def getEntityAntecedent(self,e2):
        if (e2.type in ['TITLE']):
            return None
        for e in self.entities:
            if (e.type == e2.type and e.content==e2.content):
                return e
        return None
        
        
    # Add a new Entity object to the list of recognized entities
    def addEntity(self,e):
        ecoref = self.getEntityAntecedent(e)
        if (ecoref == None):
            # no antecedent for this entity occurrence
            # it's a new entity
            e.setId(self.entityNb+1)
            self.entityNb = self.entityNb + 1
        else:
            e.setCoref(ecoref)
        e.setSourceUrl(self.sourceUrl)
        self.entities.append(e)
        return e
        
    
    def addEntities(self,entities):
        """ Add these entities to the list of this TextParse entities """
        for e in entities:
            self.addEntity(e)
        
    # Add 1 relation to the list of recognized relations
    def addRelation(self,rel):
        rel.setId(self.relationNb)
        self.relationNb = self.relationNb + 1
        self.relations.append(rel)
        rel.e1.arg1rels.append(rel)
        rel.e2.arg2rels.append(rel)
        
    # Add relations to the list of recognized relations
    def addRelations(self,rels):
        for rel in rels:
            self.addRelation(rel)
            
    # returns e2 iff e1 is an arg1 of a relation of type rtype with an argé e2 of type e2type
    def getArg2(self,rtype,e1,e2type):
        for rel in e1.arg1rels:
            if (rel.type == rtype and (e2type=='' or rel.e2.type==e2type) ):
                return e2
        return None
        
    def getArg1(self,rtype,e1type,e2):
        for rel in e2.arg2rels:
            if (rel.type == rtype and (e1type=='' or rel.e1.type==e1type) ):
                return e1
        return None
        
    def addRelationsByDeps(self,e,enode,depnames):
        for depname in depnames:
            node2 = enode.getDependent(depname)
            if (node2 == None):
                continue
            e2 = Entity('STRINGVALUE',[node2],self.currentListIds,depname,e.type)
            self.sentEntities.append(e2)
            rel = Relation(depname,e,e2)
            self.addRelation(rel)
            
    def compareEntities(self,e1,e2):
        if (e1.loffset < e2.loffset):
            return -1
        elif (e1.loffset == e2.loffset):
            if (e1.roffset < e2.roffset):
                return 1
            elif (e1.roffset == e2.roffset):
                return 0
            else:
                return -1
        else:
            return 1
            
    def printSentEntities(self):
        print "-----------------------:"
        i = self.currentSentEntityInd
        while (i < len(self.entities)):
            e = self.entities[i]
            print e.type,e.loffset,e.content
            i = i + 1
            
    def processLocation(self,e,dep):
        if (self.inLocation == None):
            self.inLocation = Entity('LOCATION',[],self.currentListIds)
            self.sentEntities.append(self.inLocation)
        rel = Relation(dep.name+'_OF', self.inLocation, e)
        self.addRelation(rel)
        for n in e.nodes:
            self.inLocation.addNode(n)
            
    # For each sentence parsed by xip, preprocess it and store it
    def processSentence(self,sentparse,inputfilename,parinfo):
        
        # store the sentence parse
        self.sentParses.append(sentparse)
        self.sentNb = self.sentNb + 1
        if (self.currentListIds != parinfo.listIds):
            self.currentListIds = parinfo.listIds
            self.parNumber = self.parNumber + 1
            parinfo.Number = self.parNumber
            
        self.sentEntities = []
        if (sentparse.props == []):
            self.inLocation = None
        # Single entities
        for d in sentparse.props:
            if (d.name in self.depEntityTypes):
                e = Entity(d.name,[d.args[0]],self.currentListIds)
                self.sentEntities.append(e)
                #self.addEntity(e)
                if (d.name == 'PERSON'):
                    self.addRelationsByDeps(e,d.args[0],
                    ['PTITLE_OF','FIRSTNAME_OF','LASTNAME_OF','FIRSTNAME2_OF'])
                if (d.name in ['STREET','POSTALCODE','CITY','COUNTRY','STATE','CONTINENT']):
                    self.processLocation(e,d)
                else:
                    self.inLocation = None
            else:
                self.inLocation = None
                    
        self.sentEntities.sort(self.compareEntities)
        self.addEntities(self.sentEntities)

        
        # postNlp prepare/process of this (one) sentence
        self.postNlp.processSentence(self, sentparse, parinfo)
        
        #self.printSentEntities()
        
        self.currentSentEntityInd = len(self.entities)
        
    # post nlp processing of whole doc
    def processPostNlp(self):
        self.postNlp.processPostNlp(self)

    # print annotations
    def printAnnotations(self):
        self.postNlp.printAnnotations(self)


####################################################

TextParseDB = TextParse(getPostNlpClass('CPO'))

####################################################
stdout = sys.stdout


def vikef_process_sentence(i,inputfilename,parListIds,parFontSize,parTop, Result):
    """Adds the sentence parse to the TextParse.
    It recognizes entity/relation occurrences when they need more than NLP
    to be recognized.
    Then it outputs all the recognized entities and relations according to
    annotation schemas."""
    
    global TextParseDB
    global stdout

    if (TextParseDB.sourceUrl!=inputfilename):
        # first call : this is a new document
        vikefInitProcessing(inputfilename,'CPO')
    
    sentparse = SentenceParse(i,TextParseDB.sentNb)
    parinfo = StructInfo(parListIds,parFontSize,parTop)
    #print "SENTENCE:"
    #print sentparse.root.form
    
    TextParseDB.processSentence(sentparse,inputfilename,parinfo)
        

######

def vikefInitProcessing(inputfilename,postnlp):
    #print "Je traite:",inputfilename
    TextParseDB.reinit(getPostNlpClass(postnlp))
    TextParseDB.sourceUrl = inputfilename 
    TextParseDB.docEntity = Entity('DOCUMENT','PROCESSED_DOCUMENT','')
    urlent = Entity('STRINGVALUE',inputfilename,'','URL_OF','DOCUMENT')
    TextParseDB.addEntities([TextParseDB.docEntity,urlent])
    rel = Relation('URL_OF',TextParseDB.docEntity,urlent,'')
    TextParseDB.addRelation(rel)
    


