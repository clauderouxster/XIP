Python:

vikef_process_sentence(#1, string inputfilename, string entryLevelUrl, string parListIds, int parFontSize, int parTop, python Result).


import sys
import string
import re


###
class MyNode:
    def __init__(self, xipnode, sentnb, par=None):
        self.sentenceNb = sentnb
        self.loffset = xipnode.left
        self.roffset = xipnode.right
        self.charloffset = xipnode.leftoffset
        self.charroffset = xipnode.rightoffset
        self.nodeNumber = xipnode.index
        self.par = par
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
        self.entity = None # for VIKEF
        
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
    def __init__(self, itop, sentnb, par):
        self.sentNb = sentnb
        self.nodes = {}
        self.root = self.addNode(itop, sentnb, None, None,par)
        self.props = []
        self.deps = []
        self.preterminals = []
        self.sentInsideQuotes = True        

        deps = xips.dependencyset("?")
        for d in deps:
            self.addDependency(d,par)
            
        self.props.sort(self.compareProps)
        #print 'HAHA ======== ', len(self.preterminals)
        
        
        
        
    def addNode(self,i,sentnb,parent,previous,par):
        if (i == -1):
            return None
        xipnode = XipNode(i)
        node = MyNode(xipnode,sentnb,par)
        # print "## Store node:",i,node.pos,node.form
        if (xipnode.daughter >= 0):
            self.nodes[i] = node
        node.parent = parent
        node.previous = previous
        
        # print "## xipnode.daughter ",xipnode.daughter
        if (xipnode.daughter >= 0):
            node2 = self.addNode(xipnode.daughter,sentnb,node,None,par)
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
            # print  >>sys.stderr, "  TOKEN!!",node.form,node.charloffset,node.charroffset
            
        if (node.isPreTerminal()):
            self.preterminals.append(node)
            
            if (node.unifyfeats("guille")):
                node.insideQuotes = True
            else:
                self.sentInsideQuotes = False
            
        if (xipnode.next >= 0):
            node2 = self.addNode(xipnode.next,sentnb,parent,node,par)
            node.next = node2
        elif (parent != None):
            node.parent.last = node
            
        return node
        
    
    def addDependency(self,d,par):
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
                node = MyNode(xipnode,self.sentNb,par)
                
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
            
    # A function to compare two nodes (to sort a list of nodes)
    # same as above but takes real node objects as args
    def compareDirectNodes(self,n1,n2):
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
            
    # A function to compare two props (to sort a list of nodes)
    def compareProps(self,p1,p2):
        return self.compareDirectNodes(p1.args[0],p2.args[0])
        
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
    
    if (collection_type == 'generic'):
        return PostNlpGeneric()
    elif (collection_type == 'cpo'):
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
        self.titlePar = None
        self.titleEntity = None
        # AUTHOR info
        self.authorsAlreadySelected = False
        self.lastAuthorInd = 0
        self.authorRelations = []
        self.authorIndices = {}
        
    #
    def processSentence(self, textparse, sentparse, par):
        # process title
        textparse.postNlp.processTitle(textparse, sentparse, par)
        textparse.postNlp.processAuthor(textparse, sentparse, par)
        textparse.postNlp.processOther(textparse,sentparse,par)
        
    # checks if this is the title
    def processTitle(self,textparse, sentparse, par):
        if (textparse.titleFoundBySegmanticType or self.titleAlreadySelected):
            return None
        if (self.titlePar==None):
            self.titlePar = par
        # tests if segmantic_type=='title'
        
        if (par.number < 4):
            # print >>sys.stderr, 'Hey!!'
            if (self.titlePar.number == par.number):
                # another sentence within the title paragraph
                self.titleNodes.append(sentparse.root)
            elif ( (par.fontSize > self.titlePar.fontSize) or
            (par.top <= self.titlePar.top) ):
                self.titlePar = par
                self.titleNodes = [sentparse.root]
            return None
        elif (self.titleNodes != []):
            # reached the 4th paragaraph, so stop looking for a title and
            # output the one already found
            self.titleAlreadySelected = True
            self.titleEntity = Entity('TITLE', self.titleNodes, self.titlePar)
            textparse.addEntity(self.titleEntity)
            rel = Relation('TITLE_OF',self.titleEntity,textparse.docEntity)
            textparse.addRelation(rel)
            

    #
    def processAuthor(self,textparse,sentparse,par):
        """check if a PERSON entity is an AUTHOR of current article"""
        if (not textparse.endOfParsing or self.authorsAlreadySelected):
            # for Generic docs, compute AUTHOR after all the doc has been parsed
            return None

        if (not self.titleAlreadySelected):
            return None
            
        authors = []
        i = 0
        while ( i < len(textparse.entities) ):
            if (textparse.entities[i].type == 'PERSON' ):
                break
            i = i+1
        if ( i < len(textparse.entities) and
        ( textparse.entities[i].gloffset < self.titleEntity.gloffset or textparse.entities[i].gloffset > self.titleEntity.groffset ) ):
            
            authors.append(textparse.entities[i])
            i = i+1
            while ( i < len(textparse.entities) ):
                e = textparse.entities[i]
                #print "HAHA",e.type,e.content
                if (e.type == 'PERSON'):
                    authors.append(textparse.entities[i])
                elif (e.type == 'MARKER_CANDIDATE' and authors[len(authors)-1].marker == None and authors[len(authors)-1].groffset==(e.gloffset-1) ):
                    authors[len(authors)-1].marker=e.content
                    e.marker = 'USED'
                elif (e.gloffset > authors[len(authors)-1].groffset ):
                    self.authorsAlreadySelected = True
                    break
                i = i+1
                
            for a in authors:
                rel = Relation('AUTHOR_OF',a,textparse.docEntity)
                textparse.addRelation(rel)

            # check author affiliations and emails
            emark=None
            lastoffset = 0
            while ( i < len(textparse.entities) ):
                e = textparse.entities[i]
                #print  >>sys.stderr, "HOHO!!",e.type,e.content
                if (e.type == 'MARKER_CANDIDATE'):
                    emark = e.content
                elif (e.type == 'ORGANISATION'):
                    if (emark!=None):
                        e.marker = emark
                    for a in authors:
                        if (a.marker == e.marker):
                            rel = Relation('MEMBER_OF',a,e)
                            textparse.addRelation(rel)
                elif (e.type == 'EMAIL'):
                    if (emark!=None):
                        e.marker = emark
                    for a in authors:
                        if (a.marker == e.marker and not a.isArg2In('EMAIL_OF') ):
                            rel = Relation('EMAIL_OF',e,a)
                            textparse.addRelation(rel)
                            break
                elif (e.groffset>lastoffset):
                    emark=None
                    break
                lastoffset = e.groffset
                i = i+1
        
        
        
    def processOther(self,textparse,sentparse,par):
        #if (not textparse.endOfParsing):
            # for Generic docs, compute after all the doc has been parsed
        el = textparse.getSentEntityMatchList(['ORGANISATION','ORGANISATION'])
        pass
        
        
    # returns True iif bib citation label c refers to document d
    def citationMatches(self,c,d):
        # first check string labels
        edoclabel = d.getArg1InRel('LABEL_OF')
        if (edoclabel != None and edoclabel.content==c.content):
            return True

        return False
        
    # process and solve bib citations relations
    def processBibCitations(self,textparse):
        for c in textparse.citationOccs:
            #print  >>sys.stderr, "CITATION!!",c.content
            for d in textparse.citedDocuments:
                if (self.citationMatches(c,d) ):
                    citetypes = ''
                    for udepname in c.nodes[0].props:
                        # check if it has a citation type: must prefixed with
                        # 'CITATIONLABEL_'. Current defined citation types are:
                        # 'CITATIONLABEL_BASED-ON', 'CITATIONLABEL_COMPAR',
                        # 'CITATIONLABEL_BKG','CITATIONLABEL_ASSESS',
                        # 'CITATIONLABEL_UNKNOWN',
                        if (udepname.startswith('CITATIONLABEL_') ):
                            #print  >>sys.stderr, "   -",udepname
                            citetype = udepname.replace('CITATIONLABEL_','',1)
                            if (citetypes!= ''):
                                if (citetypes.find(citetype)<0):
                                    citetypes = citetypes + '|'
                                    citetypes = citetypes + citetype
                            else:
                                citetypes = citetype

                    rel = Relation('CITES',textparse.docEntity,d,citetypes)
                    textparse.addRelation(rel)
                    break;
        
        
    # post NLP process on whole doc
    def processPostNlp(self,textparse):
        # process AUTHOR rel
        textparse.postNlp.processAuthor(textparse,None,None)
        textparse.postNlp.processBibCitations(textparse)
        
        
    # Printing
    def printAnnotations(self,textparse):
        for e in textparse.entities:
            if (e.type in ['MARKER_CANDIDATE','REFNUMBER','CITY','STATE','COUNTRY', 'POSTALCODE','CONTINENT', 'SESSION']):
                continue
                #pass
            elif (e.type=='STRINGVALUE' and not(e.getArg1RelType() in ['PTITLE_OF','FIRSTNAME_OF','FIRSTNAME2_OF','LASTNAME_OF'])):
                continue
            s = e.getXmlEntityString(textparse.nlpTool,textparse.nlpToolVersion)
            s = s.replace('&','&amp;')
            print s
        for r in textparse.relations:
            if (r.type in ['CITY_OF', 'STATE_OF','COUNTRY_OF', 'POSTALCODE_OF', 'CONTINENT_OF', 'ID_OF', 'TYPE_OF', 'SESSIONTITLE_OF', 'SESSIONLOCATION_OF']):
                continue
            if (r.type=='COREF' and r.e1.type!='PERSON'):
                continue
            s = r.getXmlRelationString(textparse.nlpTool,textparse.nlpToolVersion)
            s = s.replace('&','&amp;')
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
        self.sessionIdPattern = re.compile(r"^([A-Z]+[0-9]*)[ ]*\-[ ]*(?:([0-9]+)[ ]*\:)?")
        self.abstractTypePattern = re.compile(r"\([ ]*([^)]+)\)[ ]*$")
        self.inSection = 'title:'
        self.authors = []
        self.mainAuthor = None
        self.mainAuthorInstitution = None
    
    def processTitle(self,textparse, sentparse, par):
        if (self.inSection != 'title:'):
            return
        m = self.sessionIdPattern.match(sentparse.root.form)
        if ( m!= None and self.sessionId==None ):
            #print 'OK, session ID:',sentparse.root.form
            # session Id found, first parag
            grps = m.groups()
            nodes = [sentparse.root.daughter]
            self.sessionId = Entity('STRINGVALUE',nodes,par,'ID_OF','SESSION')
            textparse.addEntity(self.sessionId)
            self.sessionEntity = Entity('SESSION',nodes,par)
            textparse.addEntity(self.sessionEntity)
            rel = Relation('ID_OF',self.sessionId,self.sessionEntity)
            textparse.addRelation(rel)
            if (len(grps) == 2 and grps[1]!=None):
                nodes = [sentparse.root.daughter.next.next]
                self.abstractId = Entity('STRINGVALUE',nodes,par,'ID_OF','DOCUMENT')
                textparse.addEntity(self.abstractId)
                rel = Relation('ID_OF',self.abstractId,textparse.docEntity)
                textparse.addRelation(rel)
            else:
                n = sentparse.root.daughter.next
                if (n.form == '-'):
                    n = n.next
                    while (n!=None and not (n.next==None and n.pos=='INS') ):
                        self.titleNodes.append(n)
                        n = n.next                    
                    
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
                    e = Entity('STRINGVALUE',self.abstractTypeNodes,par,
                    'TYPE_OF','DOCUMENT')
                    textparse.addEntity(e)
                    rel = Relation('TYPE_OF',e,textparse.docEntity)
                    textparse.addRelation(rel)
                
            #self.titleNodes = [sentparse.root]
            self.titlePar = par
            self.titleAlreadySelected = True
            self.titleEntity = Entity('TITLE', self.titleNodes, self.titlePar, 'TITLE_OF', 'DOCUMENT')
            textparse.addEntity(self.titleEntity)
            rel = Relation('TITLE_OF',self.titleEntity,textparse.docEntity)
            textparse.addRelation(rel)
        elif (sentparse.root.form!='Session:'):
            # we are in the same parag where we found the title
            # check if it's an abstact type between parentheses
            m = self.abstractTypePattern.search(sentparse.root.form)
            if ( m ):
                # first, if the first part of this "sentence" is a part of the title, then collect the nodes and add them to the title entity
                n = sentparse.root.daughter
                while (n!=None and not (n.pos=='INS' and n.next==None)):
                    self.titleEntity.addNode(n)
                    n = n.next
                self.abstractTypeNodes = []
                if (n!=None and n.next==None and n.pos=='INS'):
                    n = n.daughter.next
                while (n!=None and not (n.next==None and n.pos=='PUNCT') ):
                    self.abstractTypeNodes.append(n)
                    n = n.next
                if (self.abstractTypeNodes != []):
                    e = Entity('STRINGVALUE',self.abstractTypeNodes,par,
                        'TYPE_OF','DOCUMENT')
                    textparse.addEntity(e)
                    rel = Relation('TYPE_OF',e,textparse.docEntity)
                    textparse.addRelation(rel)
            # anyways, stop processing the title
            self.titleAlreadySelected = True
            
            
    def processSession(self, textparse, sentparse, par):
        if (not self.titleAlreadySelected):
            return
            
        # just to make sure there exists a SESSION entity
        if (self.sessionEntity == None):
            self.sessionEntity = Entity('SESSION',[],par)
            
        # process
        if (self.inSection == 'session:'):
            if (self.sessionTitle==None):
                self.sessionTitle = Entity('STRINGVALUE', [sentparse.root], par,'TITLE_OF','SESSION')
                textparse.addEntity(self.sessionTitle)
                if (self.sessionEntity != None):
                    rel = Relation('SESSIONTITLE_OF',self.sessionTitle, self.sessionEntity)
                    textparse.addRelation(rel)
                    
                    
            elif (self.sessionDate==None):
                e = textparse.getCurrentSentFirstEntity('DATE')
                if (e != None):
                    self.sessionDate = e
                    rel = Relation('DATE_OF',self.sessionDate, self.sessionEntity)
                    textparse.addRelation(rel)
                else:
                    # no date, then it's certainly a remaining part of the session title
                    self.sessionTitle.addNode(sentparse.root)
                    
                if (self.sessionTime==None):
                    e = textparse.getCurrentSentFirstEntity('TIME')
                    if (e != None):
                        self.sessionTime = e
                        rel = Relation('TIME_OF',self.sessionTime, self.sessionEntity)
                        textparse.addRelation(rel)
                        
                if (self.sessionLocation==None):
                    dep = sentparse.getProp('ROOM')
                    if (dep != None):
                        self.sessionLocation = Entity('LOCATION', [dep.args[0]], par,'LOCATION_OF','SESSION')
                        textparse.addEntity(self.sessionLocation)
                        rel = Relation('SESSIONLOCATION_OF',self.sessionLocation, self.sessionEntity)
                        textparse.addRelation(rel)                    
        else:
            sentform = sentparse.root.form.replace(' ','')
            sentform = sentform.lower()
            if (sentform == 'session:'):
                self.inSection = 'session:'
        
    #
    def processAuthor(self,textparse,sentparse,par):
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
                  #      rel = Relation('MEMBER_OF',e,self.mainAuthorInstitution)
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
                    rel = Relation(pref+'AUTHOR_OF',el[0],textparse.docEntity)
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
                    rel = Relation('MEMBER_OF',self.abstractAuthor,el[0])
                    textparse.addRelation(rel)
                    if (len(el)==2):
                        rel = Relation('PART_OF',el[1],el[0])
                        textparse.addRelation(rel)
                    
                    if (self.inSection == 'author:'):
                        self.mainAuthorInstitution = self.authorInstitution
                elif (self.authorLocation==None):
                    el = textparse.getSentEntityMatchList(['LOCATION'])
                    if (el != None):
                        # no author institution, but there is a location
                        rel = Relation('AUTHOR_LOCATION_OF',el[0],self.abstractAuthor)
                        textparse.addRelation(rel)
                        
            elif (self.authorLocation==None):
                el = textparse.getSentEntityMatchList(['LOCATION'])
                if (el != None):
                    self.authorLocation = el[0]
                    if (self.authorInstitution!=None):
                        rel = Relation('LOCATION_OF',el[0],self.authorInstitution)
                        textparse.addRelation(rel)
                        
                        
            
            
    # post NLP process on whole doc
    def processPostNlp(self,textparse):
        pass


    def processOther(self,textparse,sentparse,par):
        self.processSession(textparse,sentparse,par)
        
        
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
                
                
    def printAnnotationsForEvaluation(self,textparse):
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
                deps = e.getArg2Rels(['TITLE_OF','DATE_OF','TIME_OF','LOCATION_OF'])
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
                deps = e.getArg2Rels(['TITLE_OF','TYPE_OF'])
                for d in deps:
                    cpoid = str(e.id)
                    cpotype = 'abstract_' + d.type.replace('_OF','').lower()
                    cpovalue = d.e2.content
                    print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
            elif (e.type == 'PERSON'):
                if (e.isArg1In('AUTHOR_OF')):
                    pref = ''
                elif (e.isArg1In('CO-AUTHOR_OF')):
                    pref = 'co-'
                else:
                    continue
                author = e
                deps = e.getArg2Rels(['PTITLE_OF','FIRSTNAME_OF','LASTNAME_OF'])
                for d in deps:
                    cpoid = str(e.id)
                    cpotype = pref + 'author_' + self.relTranslation(d.type)
                    cpovalue = d.e2.content
                    print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
                
                e1 = e.getHeadInRel('MEMBER_OF')
                instit = e1
                if (e1 != None):
                    cpotype = pref + 'author_' + 'institution'
                    cpovalue = e1.content
                    print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
                    
                    e1 = e1.getDepInRel('PART_OF')
                    if (e1 != None):
                        cpotype = pref + 'author_' + 'department'
                        cpovalue = e1.content
                        print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
                        e1 = e1.getHeadInRel('LOCATION_OF')
                        
                if (instit != None):
                    e1 = instit.getDepInRel('LOCATION_OF')
                else:
                    e1 = author.getDepInRel('AUTHOR_LOCATION_OF')
                    
                if (e1!=None):
                    deps = e1.getArg2Rels(['POSTALCODE_OF','CITY_OF','COUNTRY_OF'])
                    for d in deps:
                        cpotype = pref + 'author_' + d.type.replace('_OF','').lower()
                        cpovalue = d.e2.content
                        print cpourl + '|' + cpotype + '|' + cpoid +'|' + cpovalue
                


####################################################################


class Entity:
    """VIKEF entity"""
    def __init__(self, etype, nodes, par, arg_of_rel = '', head_type = ''):
        self.id = 0
        self.type = etype
        self.argOfRel = arg_of_rel
        self.headtype = head_type
        self.nodes = nodes
        self.content = ''
        self.sourceUrl = ''
        self.par = par
        self.arg1rels = []
        self.arg2rels = []
        self.marker = None
        
        if (nodes != [] and nodes != str(nodes) ):
            self.charloffset = nodes[0].charloffset
            self.loffset = nodes[0].loffset + par.roffset
            #print  >>sys.stderr, '    !!!YOP:',nodes[0].form,nodes[0].loffset
            self.roffset = nodes[len(nodes)-1].roffset + par.roffset
            self.charroffset = nodes[len(nodes)-1].charroffset
            self.gloffset = self.loffset
            self.groffset = self.roffset
            self.content = nodes[0].form
            nodes[0].entity = self
            i=1
            while (i < len(nodes)):
                if (nodes[i-1].charroffset < nodes[i].charloffset):
                    self.content = self.content + ' ' + nodes[i].form
                else:
                    self.content = self.content + nodes[i].form
                nodes[i].entity = self
                i = i+1
            #print  >>sys.stderr, "!!EntityNode=",self.nodes[0].form, self.nodes[0].entity.id

        else:
            self.loffset = 0
            self.roffset = 0
            self.charloffset = 0
            self.charroffset = 0
            self.gloffset = self.loffset
            self.groffset = self.roffset
            if ( nodes == str(nodes) ):
                # nodes is a string
                self.content = nodes
        self.coref = None
        

    def addNode(self,n):
        #print 'AddNode to',self,self.content,'node:',n.form,n.loffset
        if (self.nodes==[]):
            self.loffset = n.loffset  + self.par.roffset
            self.roffset = n.roffset  + self.par.roffset
            self.charloffset = n.charloffset
            self.charroffset = n.charroffset
            self.gloffset = self.loffset
            self.rloffset = self.roffset
            self.content = n.form
            self.nodes.append(n)
        else:
            if ( (n.loffset + self.par.roffset) < self.loffset):
                self.loffset = n.loffset  + self.par.roffset
                self.charloffset = n.charloffset
                self.gloffset = self.loffset
            if ( (n.roffset + self.par.roffset) > self.roffset):
                self.roffset = n.roffset  + self.par.roffset
                self.charroffset = n.charroffset
                self.groffset = self.roffset
                
            if ( n.par.number>self.par.number or self.nodes[len(self.nodes)-1].charroffset < n.charloffset ):
                self.content = self.content + ' ' + n.form
            else:
                self.content = self.content + n.form
                
            self.nodes.append(n)
            
        n.entity = self
            
        
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
            
    def isArg1In(self,rtype,e2=None):
        for r in self.arg1rels:
            if (r.type==rtype and (e2==None or e2==r.e2)):
                return True
        return False
        
    def isArg2In(self,rtype,e1=None):
        for r in self.arg2rels:
            if (r.type==rtype and (e1==None or e1==r.e1)):
                return True
        return False
                
    def isHeadInRel(self,rel,rtype,e1=None):
        for r in self.arg2rels:
            if (r.type==rtype and (e1==None or e1==r.e1)):
                rel.type = rtype
                rel.e2=self
                rel.e1=r.e1
                return True
        return False

    def isDepInRel(self,rel,rtype,e2=None):
        for r in self.arg1rels:
            if (r.type==rtype and (e2==None or e2==r.e2)):
                rel.type = rtype
                rel.e1=self
                rel.e2=r.e2
                return True
        return False
        
    def getArg1InRel(self,rtype):
        for r in self.arg2rels:
            if (r.type==rtype or rtype=='?'):
                return r.e1
        return None

    def getArg2InRel(self,rtype):
        for r in self.arg1rels:
            if (r.type==rtype or rtype=='?'):
                return r.e2
        return None
        
    def getDepInRel(self,rtype):
        for r in self.arg2rels:
            if (r.type==rtype or rtype=='?'):
                return r.e1
        return None
        
    def getArg1RelType(self):
        if (self.arg1rels != None and self.arg1rels!=[]):
            return self.arg1rels[0].type
        else:
            return None

    def getHeadInRel(self,rtype):
        for r in self.arg1rels:
            if (r.type==rtype or rtype=='?'):
                return r.e2
        return None
        
    def getAllDepsInRel(self,rtype):
        deps = []
        for r in self.arg2rels:
            if (r.type==rtype or rtype=='?'):
                deps.append(r.e1)
        return deps

    def getArg2Rels(self,rtypes=None):
        deps = []
        for r in self.arg2rels:
            if (rtypes==None or r.type in rtypes):
                deps.append(r)
        return deps

        
    # creates an XML entity representation from the entity parameters
    def getXmlEntityString_light(self,nlptool,nlptoolversion):
        return string.joinfields([
        '  <ExtractedData Entry_level_url="',
        '" Entry_level_ids="','" loffset="',str(self.loffset), '" roffset="',str(self.roffset),'" tool="',nlptool,'" version="', nlptoolversion,'">\n',
        '    <Entity data_id="','">\n',
        '      <',self.type,'>',self.content,'</',self.type,'>\n',
        '    </Entity>\n',
        '  </ExtractedData>'], '' )

    def getXmlEntityString(self,nlptool,nlptoolversion):
        if (self.par!=None and str(self.par)!=self.par ):
           # par is not a string, it's a real paragraph object
            listids = self.par.listIds
            loffset = self.loffset
            roffset = self.roffset
        else:
            listids = ''
            loffset = 0
            roffset = 0
        return string.joinfields([
        '  <ExtractedData Entry_level_url="',self.sourceUrl,
        '" Entry_level_ids="',listids,'" loffset="',str(loffset), '" roffset="',str(roffset),' Par=',str(self.par.number),'" tool="',nlptool,'" version="', nlptoolversion,'">\n',
        '    <Entity data_id="',str(self.id),'">\n',
        '      <',self.type,'>',self.content,'</',self.type,'>\n',
        '    </Entity>\n',
        '  </ExtractedData>'], '' )
        
        
######################################################################

class Relation:
    """VIKEF relation"""
    def __init__(self, rtype, e1, e2, rsubtype=''):
        self.id = 0
        self.type = rtype
        self.subtype = rsubtype
        self.e1 = e1
        self.e2 = e2
        self.sourceUrl = e1.sourceUrl
        
        
    def setId(self,rid):
        self.id = rid
        
        
    # creates an XML entity representation from the entity parameters
    def getXmlRelationString_verbose(self,nlptool,nlptoolversion):
        if (self.subtype != ''):
            relsubtype= ' subtype="' + self.subtype + '" '
        else:
            relsubtype=''
        return string.joinfields([
        '  <ExtractedData Entry_level_url="',
        '" Entry_level_ids="','" tool="',nlptool,
        '" version="',nlptoolversion,'">\n',
        '    <Relation data_id="',str(self.id),'">\n',
        '      <reltype',relsubtype,'>',self.type,'</reltype>\n',
        '      <subject>',str(self.e1.id)+': '+self.e1.content,'</subject>\n',
        '      <object>',str(self.e2.id)+': '+self.e2.content,'</object>\n',
        '    </Relation>\n',
        '  </ExtractedData>'], '' )
        
    def getXmlRelationString(self,nlptool,nlptoolversion):
        if (self.subtype != ''):
            relsubtype= ' subtype="' + self.subtype + '" '
        else:
            relsubtype=''
        return string.joinfields([
        '  <ExtractedData tool="',nlptool,
        '" version="',nlptoolversion,'">\n',
        '    <Relation data_id="',str(self.id),'">\n',
        '      <reltype',relsubtype,'>',self.type,'</reltype>\n',
        '      <subject>',str(self.e1.id),'</subject>\n',
        '      <object>',str(self.e2.id),'</object>\n',
        '    </Relation>\n',
        '  </ExtractedData>'], '' )
            

        
####################################################################"

# A class to store information on the document structure
# i.e. information on the PARAGRAPH element of harmonized documents



class Paragraph:
    """VIKEF paragraph in a harmonized document"""
    def __init__(self, number, parListIds,parFontSize,parTop):
        self.number = number
        self.listIds = parListIds
        self.fontSize = parFontSize
        self.top = parTop
        self.loffset = 0
        self.roffset = 0

    
####################################################################

# 

class TextParse:
    def __init__(self, postnlp):
        self.nlpTool = 'XIP'
        self.nlpToolVersion = '9.59'
        self.depEntityTypes = ['PERSON','LOCORG','EMAIL','ORGANISATION', 'URL', 'DATE', 'TIME', 'EVENT', 'LOCATION', 'REFNUMBER', 'MARKER_CANDIDATE', 'AUTHOR','STREET','POSTALCODE','CITY','STATE','COUNTRY','CONTINENT', 'CITATIONLABEL', 'DOCUMENT','BIBREFLABEL','TITLE']
        self.depRelationTypes = [ 'LOCATION_OF', 'PART_OF', 'LABEL_OF', 'AUTHOR_OF', 'EDITOR_OF', 'TITLE_OF', 'URL_OF', 'DATE_OF' ]

        self.postNlp = postnlp
        self.docEntity = None
        self.entities = []
        self.sentEntities = []
        self.currentSentEntityInd = 0
        self.relations = []
        self.entityNb = 0
        self.relationNb = 0
        self.dataNb = 0
        self.sentParses = []
        self.sentNb = 0
        self.sourceUrl = None
        self.outputFilename = ''
        self.out = None
        self.currentPar = None
        self.currentLoffset = 0
        self.endOfParsing = False # set to True when end of text is reached
        self.inLocation = None
        self.citationOccs = []
        self.citedDocuments = []
        self.titleFoundBySegmanticType = False
       
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
        self.sourceUrl = None
        self.outputFilename = ''
        self.out = None
        self.currentPar = None
        self.currentLoffset = 0
        self.endOfParsing = False # set to True when end of text is reached
        self.inLocation = None
        self.citationOccs = []
        self.citedDocuments = []
        self.titleFoundBySegmanticType = False

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
        
    # 
    def isNameInitialOf(self,s1,s2):
        i = 0
        j = 0
        while (i < len(s1) and j < len(s2) ):
            if (not (s1[i].isupper() and s1[i]==s2[j]) ):
                return False
            i = i+1
            j = j+1
            if (i<len(s1)):
                if (s1[i].islower()):
                    if (j==len(s2) or s1[i]!=s2[j]):
                        return False
                    else:
                        i = i+1
                        j = j+1
                if (i<len(s1)):
                    if (s1[i].isupper()):
                        return False
                    elif (s1[i]=='.'):
                        while (j<len(s2) and s2[j]!=' ' and s2[j]!='-'):
                            j = j+1
                        i = i+1
                        if (j<len(s2) and s2[j]=='-'):
                            if (i==len(s1) or s1[i]!='-'):
                                return False
                            else:
                                i = i+1
                        j = j+1
        if (i>=len(s1)):
            # print >>sys.stderr, 'Init:',s1,'->',s2
            return True
        else:
            return False
        
    # 2 entities have same person names
    def samePersonName(self,e1,e2):
        #print >>sys.stderr, 'COREF CAND:',e1.content,'-->',e2.content
        
        if (e1.content==e2.content):
            return True
        
        e1lname = e1.getArg1InRel('LASTNAME_OF')
        e2lname = e2.getArg1InRel('LASTNAME_OF')
        
        if (e1lname!=None and e2lname!=None and e1lname.content!=e2lname.content):
            return False
            
        e1fname = e1.getArg1InRel('FIRSTNAME_OF')
        e2fname = e2.getArg1InRel('FIRSTNAME_OF')

        if (e1fname!=None and e2fname!=None and e1fname.content!=e2fname.content and not self.isNameInitialOf(e2fname.content,e1fname.content) and not self.isNameInitialOf(e1fname.content,e2fname.content)):
            return False
            
        e1fname2 = e1.getArg1InRel('FIRSTNAME2_OF')
        e2fname2 = e2.getArg1InRel('FIRSTNAME2_OF')        
        
        if (e1fname2!=None and e2fname2!=None and e1fname2.content!=e2fname2.content and not self.isNameInitialOf(e2fname2.content,e1fname2.content) and not self.isNameInitialOf(e1fname2.content,e2fname2.content)):
            return False
            
        if ( (e1lname==None and e1fname==None) or (e2lname==None and e2fname==None) ):
            return False
            
        return True
        
        
    # if node is of type 'etype' and it is an occ of an entity already found
    # then return the first occurrence entity, otherwise return None
    def getEntityAntecedent(self,e2):
        for e in self.entities:
            if (e.type == e2.type):
                if (e.content==e2.content):
                    #print >>sys.stderr, '     ',e.content,'<--?',e2.content
                    #print >>sys.stderr,'OK!!'
                    return e
                elif (e.type == 'PERSON'):
                    #print >>sys.stderr, '     ',e.content,'<--?',e2.content
                    if( self.samePersonName(e,e2) ):
                        #print >>sys.stderr,'OK!!'
                        return e
        return None
        
        
    # Add a new Entity object to the list of recognized entities
    def addEntity(self,e):
        e.setId(self.dataNb+1)
        self.dataNb = self.dataNb + 1
        e.setSourceUrl(self.sourceUrl)
        i = len(self.entities) - 1
        while ( (i>=0) and (self.entities[i].par.number>e.par.number or (self.entities[i].par.number==e.par.number and self.entities[i].loffset > e.loffset) ) ):
            i = i-1
        self.entities.insert(i+1,e)
        #if (e.nodes!=[] and e.nodes!=str(e.nodes)):
            #print >>sys.stderr, 'ADDED entity:',e.content,e.nodes[0].id,i+1
        return e
        
    
    def addEntities(self,entities):
        """ Add these entities to the list of this TextParse entities """
        for e in entities:
            self.addEntity(e)
        
    # Add 1 relation to the list of recognized relations
    def addRelation(self,rel):
        rel.setId(self.dataNb+1)
        self.dataNb = self.dataNb + 1
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
            e2 = Entity('STRINGVALUE',[node2],self.currentPar,depname,e.type)
            self.sentEntities.append(e2)
            rel = Relation(depname,e2,e)
            self.addRelation(rel)
            
    def compareEntities(self,e1,e2):
        if (e1.par.number < e2.par.number):
            return -1
        elif (e1.par.number > e2.par.number):
            return 1
        if (e1.charloffset < e2.charloffset):
            return -1
        elif (e1.charloffset == e2.charloffset):
            if (e1.charroffset < e2.charroffset):
                return 1
            elif (e1.charroffset == e2.charroffset):
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
        if (self.inLocation == None or self.inLocation.roffset<(e.loffset-1) ):
            self.inLocation = Entity('LOCATION',[],self.currentPar)
            self.sentEntities.append(self.inLocation)
        #print dep.name,e.content,e.nodes[0].charloffset,e.nodes[len(e.nodes)-1].charroffset
        rel = Relation(dep.name+'_OF', e, self.inLocation)
        self.addRelation(rel)
        for n in e.nodes:
            if (self.inLocation.nodes!=[]):
                loclastnode = self.inLocation.nodes[len(self.inLocation.nodes)-1]
                #print '!!!!!!!!',self.inLocation.content,loclastnode.charloffset, loclastnode.charroffset, '<--',n.form,n.charloffset,n.charroffset
            self.inLocation.addNode(n)
            
    # For each sentence parsed by xip, preprocess it and store it
    def processSentence(self,sentparse,inputfilename,par):
        
        # store the sentence parse
        self.sentParses.append(sentparse)
        self.sentNb = self.sentNb + 1
        if (self.currentPar != par):
            self.currentPar = par
            par.loffset = 0  #self.currentLoffset
            par.roffset = 1
            
        #self.currentLoffset = sentparse.root.roffset
            
        self.sentEntities = []
        #print >>sys.stderr, 'Processing sent:',sentparse.root.form
        
        if (sentparse.props == []):
            self.inLocation = None
        # Single entities
        for d in sentparse.props:
            if (d.name in self.depEntityTypes):
                if (d.name == 'MARKER_CANDIDATE' and len(d.args[0].form)>3 ):
                    continue
                elif (d.name == 'EMAIL'):
                    email = d.args[0].form
                    if (email[0]=='{'):
                        email = email.replace('{','')
                        email = email.replace('}','')
                        email = email.replace(' ','')
                        emailparts = email.split('@')
                        if (len(emailparts)!=0):
                            logins = emailparts[0]
                            logins = logins.split(',')
                            for l in logins:
                                email = l + '@' + emailparts[1]
                                e = Entity(d.name,[d.args[0]],self.currentPar)
                                e.content = email
                                self.sentEntities.append(e)
                        continue
                elif (d.name == 'TITLE'):
                    self.titleFoundBySegmanticType = True
                    
                e = Entity(d.name,[d.args[0]],self.currentPar)
                self.sentEntities.append(e)
                #self.addEntity(e)
                if (d.name == 'PERSON'):
                    self.addRelationsByDeps(e,d.args[0],
                    ['PTITLE_OF','FIRSTNAME_OF','LASTNAME_OF','FIRSTNAME2_OF'])
                if (d.name in ['STREET','POSTALCODE','CITY','COUNTRY','STATE','CONTINENT']):
                    self.processLocation(e,d)
                else:
                    self.inLocation = None
                if (d.name == 'CITATIONLABEL'):
                    # citation occurrence: store it for future resolution
                    self.citationOccs.append(e)
                elif (d.name == 'DOCUMENT'):
                    self.citedDocuments.append(e)
            else:
                self.inLocation = None
                    
        self.sentEntities.sort(self.compareEntities)
        # check for COREF relations for the new entities
        for e in self.sentEntities:
            #print >>sys.stderr, 'COREF CAND FOR',e.content,e.nodes[0].id
            if (not e.type in ['PERSON','ORGANISATION','EVENT']):
                continue
            ecoref = self.getEntityAntecedent(e)
            if (ecoref != None):
                # an antecedent has been found for this entity occurrence
                rel = Relation('COREF',e,ecoref)
                self.addRelation(rel)
                continue
        
        self.addEntities(self.sentEntities)
        
        # Process linguistic relations(deps) that correspond to entity relations
        for d in sentparse.deps:
            if (d.name in self.depRelationTypes):
                # print >>sys.stderr, 'Hey!!',d.name,d.args[0].id,d.args[1].id,d.args[0].entity
                if (d.args[0].entity != None and d.args[1].entity != None):
                    # print >>sys.stderr, '   eid:',str(d.args[0].entity.id)
                    rel = Relation(d.name,d.args[0].entity,d.args[1].entity)
                    self.addRelation(rel)
        
        # postNlp prepare/process of this (one) sentence
        self.postNlp.processSentence(self, sentparse, par)
        
        #self.printSentEntities()
        
        self.currentSentEntityInd = len(self.entities)
        
        self.currentPar.roffset = self.currentPar.roffset + sentparse.root.roffset
        
    # post nlp processing of whole doc
    def processPostNlp(self):
        self.postNlp.processPostNlp(self)

    # print annotations
    def printAnnotations(self):
        self.postNlp.printAnnotations(self)


####################################################

TextParseDB = TextParse(getPostNlpClass('generic'))

####################################################
stdout = sys.stdout


def vikef_process_sentence(i,inputfilename,entryLevelUrl,parListIds,parFontSize,parTop, Result):
    """Adds the sentence parse to the TextParse.
    It recognizes entity/relation occurrences when they need more than NLP
    to be recognized.
    Then it outputs all the recognized entities and relations according to
    annotation schemas."""
    
    global TextParseDB
    global stdout
    
    
    if (TextParseDB.sourceUrl!=entryLevelUrl):
        # first call : this is a new document
        #vikefInitProcessing(inputfilename,entryLevelUrl,Result.collectionType)
        #print '!!!!New DOC, col type is:',Result.collectionType
        vikefInitProcessing(inputfilename,entryLevelUrl,Result.collectionType)
        
    # new paragraph?
    if (TextParseDB.currentPar==None):
        par = Paragraph(1,parListIds,parFontSize,parTop)
    elif (TextParseDB.currentPar.listIds != parListIds):
        par = Paragraph(TextParseDB.currentPar.number+1,parListIds,parFontSize,parTop)
    else:
        par = TextParseDB.currentPar
    
    sentparse = SentenceParse(i,TextParseDB.sentNb,par)
    #print "SENTENCE:"
    #print sentparse.root.form
    
    TextParseDB.processSentence(sentparse,inputfilename,par)
        

######

def vikefInitProcessing(inputfilename,entryLevelUrl,postnlp):
    TextParseDB.reinit(getPostNlpClass(postnlp))
    TextParseDB.sourceUrl = entryLevelUrl
    par = Paragraph(0,'',0,0)  # dummy paragraph for meta-entities
    TextParseDB.docEntity = Entity('DOCUMENT','',par)
    urlent = Entity('URL',entryLevelUrl,par,'URL_OF','DOCUMENT')
    TextParseDB.addEntities([TextParseDB.docEntity,urlent])
    rel = Relation('URL_OF',urlent,TextParseDB.docEntity,'')
    TextParseDB.addRelation(rel)
    


