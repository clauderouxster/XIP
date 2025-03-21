Python:

coref_process_sentence(#1),
coref_process_sentence_db(#1,int sentNb).


import sys
import os
import math

global TextParseDB




class MyReferent:
    def __init__(self, firstoccnode):
        self.firstocc = firstoccnode
        self.representative = firstoccnode
        self.lastocc = firstoccnode
        self.form = firstoccnode.form
        self.salience = 0
        self.lastUpdatedSalienceInSent = -1
        self.deps = []
        
    def addInformation(dep):
        self.deps.append(dep)

###
class MyNode:
    def __init__(self, xipnode, sentnb):
        self.sentenceNb = sentnb
        self.loffset = xipnode.left
        self.roffset = xipnode.right
        self.offleft = xipnode.leftoffset
        self.offright = xipnode.rightoffset
        self.nodeNumber = xipnode.index
        self.form = xipnode.surface
        self.lemma = xipnode.lemma
        self.pos = xipnode.pos
        xipnode.features = xipnode.features.strip('[')
        xipnode.features = xipnode.features.strip(']')
        xipnode.features = xipnode.features.replace('!','')
        xipnode.features = xipnode.features.lower()
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
                return 1
        return None
        
    
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
                firstchar = True
                for i in range(0,len(form)-1):
                    if (firstchar and ('A' <= form[i] <= 'Z') ):
                        abbr = abbr + form[i]
                        firstchar = False
                    elif (form[i] in [ ' ', '-']):
                        firstchar = True
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
        #if (self.id == '1:23-24:91' and n2.id=='1:25:50'):
            #print "HAHA: ",self.id,': ',self.antecedentLinks
            #debug = True
            
        # if self has anaphors, then the syntactic agreement with n2 should be
        # tested with the last anaphoric link of self
        if (self.anaphorLinks == []):
            antec = self
        else:
            antec = self.anaphorLinks[len(self.anaphorLinks)-1].anaphor
            
            
        dist = n2.loffset-antec.roffset
            
        if ( antec.sentenceNb == n2.sentenceNb ):
            if ( math.fabs(dist) <= 1 ):
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
               

        
    def setReferentSalience(self):
        basis = 1
        self.referentSalience = 0.0
        fheads = []
        
        fhead = self.getHead("SUBJ","verb")
        if (fhead != None):
            self.referentSalience = self.referentSalience + basis * 1
            fheads.append(fhead)
            
        fhead = self.getHead("OBJ","verb")
        if (fhead != None and  not (fhead in fheads) ):
            self.referentSalience = self.referentSalience + basis * 0.3 
            fheads.append(fhead)
            
        fhead = self.getHead("SUBJ-N")
        if (fhead != None and  not (fhead in fheads) ):
            self.referentSalience = self.referentSalience + basis * 0.6
            fheads.append(fhead)
            
        fhead = self.getHead("OBJ-N")
        if (fhead != None and not (fhead in fheads) ):
            self.referentSalience = self.referentSalience + basis * 0.2
            fheads.append(fhead)
            
        fhead = self.getHead("IOBJ","verb")
        if (fhead != None and  not (fhead in fheads) ):
            self.referentSalience = self.referentSalience + basis * 0.25
            fheads.append(fhead)
            
        fhead = self.getHead("MOD","verb")
        if (fhead != None and  not (fhead in fheads) ):
            self.referentSalience = self.referentSalience + basis * 0.15
            fheads.append(fhead)
            
        fhead = self.getHead("MOD","noun")
        if ( fhead != None and  not (fhead in fheads) ):
            self.referentSalience = self.referentSalience + basis * 0.08
            fheads.append(fhead)
            
        fhead = self.getHead("QUANTD","verb")
        if (fhead != None and  not (fhead in fheads) ):
            self.referentSalience = self.referentSalience + basis * 0.05
            
        if (self.referentSalience == 0):
            self.referentSalience = 0.04
        elif (self.referentSalience > 1):
            self.referentSalience = 1
        
        
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
        return
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
        if (self.antecedentLink != None):
            print " ".ljust(4),"ANTECEDENT =", self.antecedentLink.antec.id,self.antecedentLink.antec.form
        print ""



##############
class MyDep:
    def __init__(self, name, myargs):
        self.name = name
        self.args = myargs

    def myprint():
        return
        print self.name,self.args[0].id,self.args[1].id



##############
class AnaphoricLink:
    def __init__(self,n1,n2,score,anaphor_type):
        self.antec = n1
        self.anaphor = n2
        self.score = score
        self.anaphorType = anaphor_type
        
        
    def compare(self,a1,a2):
        if (a1.score > a2.score):
            return -1
        else:
            return 1
        
        
#########################################################

class SentenceParse:
    def __init__(self, itop, sentnb):
        self.sentNb = sentnb
        self.nodes = {}
        self.preterminals = []
        self.sentInsideQuotes = True
        self.root = self.addNode(itop, sentnb, None, None)

        deps = dependencyset("?")
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
        else:
            node.daughter = None
            
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
        else:
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
            
####################################################################"



class NLPMarkup:
    def __init__(self, markuptype, opos, cpos, sentnb, comment='', annot=''):
        self.markuptype = markuptype 
        self.opos = opos
        self.cpos = cpos
        self.sentenceNb = sentnb
        if (markuptype=='nlpl'):
            self.otag = '<SPAN onClick="AffBulle(\'' + comment + '\',0,event);return true;">'
            self.ctag = '</SPAN>'
        elif (markuptype=='antec'):
            self.otag = '<ANTEC>'
            self.ctag = '</ANTEC>$annot'
        elif (markuptype=='anaphor'):
            self.otag = '<ANAPHOR title="$comment">'
            self.ctag = '</ANAPHOR>$annot'
        elif (markuptype=='anaphoric'):
            self.otag = '<ANAPHORIC>'
            self.ctag = '</ANAPHORIC>'
        elif (markuptype=='quoted'):
            self.otag = '<QUOTED title="Said by: $comment">'
            self.ctag = '</QUOTED>'
        elif (markuptype=='coord'):
            self.otag = '<COORD title="COORD: $comment">'
            self.ctag = '</COORD>'
        else:
            self.otag = ''
            self.ctag = ''
        
        if (comment != ''):
            self.otag = self.otag.replace('$comment',comment)
        else:
            self.otag = self.otag.replace('$comment','')
        if (annot != ''):
            self.ctag = self.ctag.replace('$annot','<INDICE><SUB>'+annot+'</SUB></INDICE>')
        else:
            self.ctag = self.ctag.replace('$annot','')
        
    def setOpenPos(self,pos):
        self.opos = pos
        
    def setClosePos(self,pos):
        self.pos = pos
        
    def myprint(self):
        return
        print self.markuptype,self.opos,':',self.cpos



def addUnique(tags, markup):
    for tag in tags:
        if (markup.markuptype==tag.markuptype and markup.opos==tag.opos and markup.cpos==tag.cpos):
            return
    tags.insert(0,markup)
        
        
        
####################################################################"

class TextParse:
    """ TextParse stores the parses of all of the text sentences
    It also stores the list of referents mentioned in the text.
    It provides methods to add a sentence parse and to process
    the whole text parses to resolve anaphora"""
    def __init__(self):
        self.sentParses = []
        self.preterminals = []
        self.referents = []
        self.sentNb = 0
        self.wAntecSUBJ = 1
        self.wAntecPPofSUBJ = 0.75
        self.wAntecOBJ = 0.5
        self.wDistance = 1
        self.wSimilarFunction = 2
        self.rule=""
        self.lastSayer = None
        self.toBeResolved = []
        self.cataphorCandidates = []
        self.tracePrintOption = True
        
    def reset(self):
        self.__init__()
        
    def tracePrint(self,str):
        return
        if (self.tracePrintOption):
            print str
        
        

    def getRefDist(self,n1,n2):
        distance = float(math.fabs(n2.sentenceNb - n1.sentenceNb) + 1)
        if (distance == 1):
            distance = distance + float(math.fabs(n2.loffset - n1.loffset))/(n2.loffset*3)
        elif (distance > 2):
            distance = 2**distance + 10
        return distance
        
        
    def getRelativeSalience(self,n1,n2):
        distance_w = 1
        refsalience_w = 1
        salience = n1.referentSalience/(self.getRefDist(n1,n2) * distance_w)
        
        for a in n1.anaphorLinks:
            salience = salience + a.anaphor.referentSalience /                  (self.getRefDist(a.anaphor,n2) * distance_w)
            
        
        salience = (salience + salience*1000) / ( len(self.referents) + salience*1000 )
        return salience
        
        
        
    def storeReferent(self,node):
        if (node == None):
            return
        node.referentId = len(self.referents)
        
        node.setReferentSalience()
        i = len(self.referents) - 1
        while ( (i >= 0) ):
            if (self.referents[i].referentSalience <= node.referentSalience):
                break;
            i = i-1
        self.referents.insert(i+1,node)
        #self.tracePrint("ADD Referent: " + node.form + " " + node.id + " W= " + str(node.referentSalience) + "\n")
        
        
        
    def addAnaphoricLink(self,x,y,score, anaphor_type):
        alink = AnaphoricLink(x,y,score,'coref')
        y.setReferentSalience()
        if ( (y.antecedentLink == None) or (y.antecedentLink.score < alink.score) ):
            y.antecedentLink = alink
        y.antecedentLinks.append(alink)
        
        
        
    # CHECK IF 2 NPs (person) DENOTE THE SAME REFERENT
    def denoteSamePerson(self,n1,n2):
        n1.rule = ""
        # IF they have same LASTNAME, and firstnames are not different
        s1 = n1.getDependent("INTERNALS_LASTNAME")
        s2 = n2.getDependent("INTERNALS_LASTNAME")
        # print "######### Same Person ? :",n1.id,"and",n2.id
        # print "######### last1:",s1.form,"last2:",s2.form
        if ( (s1 != None) and (s2 != None) ):
            # Both have a lastname
            if (s1.lemma == s2.lemma):
                # same lastname! check firstnames
                # print "######### last1:",s1.form,"last2:",s2.form
                if (not ( n1.numberAgreement(n2) and n1.genderAgreement(n2) and n1.personAgreement(n2) ) ):
                    self.rule = self.rule + n1.rule
                    return False
                s1 = n1.getDependent("INTERNALS_FIRSTNAME1")
                s2 = n2.getDependent("INTERNALS_FIRSTNAME1")
                if ( (s1==None) or (s2==None) ):
                    return True
                elif ( s1.lemma == s2.lemma ):
                    return True
                else:
                    return False
        
        self.rule = "Must have same LASTNAME."
        
        if (n2.loffset == n2.roffset):
            # One-Token Node (Proper)
            if ( (s1 != None) and (s1.lemma == n2.lemma ) ):
                # ex: n1="George Bush" and n2="Bush"
                return True
        
        
        # IF second NP/NOUN has a Title as head and this title is the same as
	    # the title of the preceding NP
        s1 = n1.getDependent("INTERNALS_TITLE")
        if (n2.loffset == n2.roffset or (n2.daughter!=None and n2.daughter.next==None) ):
            s2 = n2
        else: # NP or NOUN with multiple elements
            s2 = n2.getDependent("HEAD")
        #print 'TITLEof n1=',s1
        if ( (s1 != None) and (s2 != None) ):
            #print '     n2 form=',s2.form
            if (n2.getDependent("DETD","def") or s2.form.istitle() ):
                #print '     Yohoo!!!!'
                # Antecedent has a title and current node has a head
                if ( (s1.form == s2.form) or (s1.lemma == s2.lemma) ):
                    return True
                elif ( s1.form.lower() == s2.lemma ):
                    # ex: Title of n1 is "President", and head of n2 is "president"
                    return True

        self.rule = self.rule + " Or the Head of n2 is the TITLE of n1"
        return False
        
    def sameProperNameHead(self,n1,n2):
        
        self.rule = "SameProperNameHead: n1 and n2 have the same head, and n2 is included in n1"
        
        x1 = n1.last
        x2 = n2.last
        
        if (x1==None or x2==None or x1.form!=x2.form):
            return 0
        x1 = x1.previous
        x2 = x2.previous
        while ( x1 and x2):
            if ( x1.form != x2.form ):
                break
            x1 = x1.previous
            x2 = x2.previous
        if (x2==None):
            return 1
        else:
            return 0
        
    # process ProperNameNP coref
    def properNameCoref(self,n1,n2):
        self.rule = ""
        n1.rule = ""
        
        if (n1.lemma == n2.lemma):
            # Same LEMMA
            self.rule = "n1 and n2 have the same lemmas"
            return 1
        elif (n2.isAbbr() and n1.unifyfeats("org")):
            # process ABBREVIATION
            self.rule = "n2 is an Abbreviation of n1"
            abbr1 = n1.getAbbr()
            #print "     > ABBR:",n1.form,"=",abbr1
            if ( n1.matchesAbbr(n2.form) ):
                return 1
        elif (n1.isAbbr() and not n2.isAbbr() and n2.unifyfeats("org")):
            # process ABBREVIATION in the reverse order
            self.rule = "n1 is an Abbreviation of n2"
            abbr1 = n2.getAbbr()
            #print "      > ABBR:",n2.form,"=",abbr1
            if ( n2.matchesAbbr(n1.form) ):
                return 1
            
        
        elif (self.sameProperNameHead(n1,n2) > 0):
            return 1
        
        elif (n1.unifyfeats("person") and n2.unifyfeats("org:~") and self.denoteSamePerson(n1,n2) ):
            self.rule = "n1 and n2 are NEs denoting the same person"
            return 1
            
        elif (n1.unifyfeats("org,location:~") and  n2.unifyfeats("org,location:~") ):
            #print >>sys.stderr, 'Hey!!' + n1.form + '<--' + n2.form
            #if n1 includes n2
            if (n1.form.startswith(n2.form)):
                bign = n1
                subn = n2
            # or if n2 includes n1
            elif (n2.form.startswith(n1.form)):
                bign = n2
                subn = n1
            else:
                bign = None
            if (bign!=None):
                bign = bign.form.replace(subn.form,'',1)
                #print >>sys.stderr, '   Yes!!bign is:' +bign
                if (bign in [' Corp.', ' Inc.']):
                    return 1
            
        return 0
        
        
    #
    def definiteNPAnaphora(self,n1,n2):
        self.rule = "Definite NP anaphora: n2 is definite and all its forms (except the det) are in n1"
        x1 = n1.last
        x2 = n2.last
        # n2 must be DEFINITE
        if (not n2.getDependent("DETD","def")):
            return 0
        #print "##################",n1.form,n1.id,"<-",n2.form
        while ( x1 and x2):
            # print "##################",x1.form,n1.id," === ",x2.form
            if (x2.pos == "DET"):
                return 1
            elif ( x1.form != x2.form ):
                return 0
            x1 = x1.previous
            x2 = x2.previous
        if (x2 and (x2.pos!="DET") ):
            return 0
        else:
            return 1 
        
        
    # process Pronoun coref
    def pronounCoref(self,n1,n2):
        self.rule = ""
        
        if (not n1.numberAgreement(n2) ):
            return 0
        if (not n1.genderAgreement(n2) ):
            return 0
        if ( not n1.personAgreement(n2) ):
            return 0
        if (n2.saidBy == n1  or (n2.saidBy and n2.saidBy.antecedentLink and n2.saidBy.antecedentLink.antec==n1) ):
            return 0
            
        if ( not n1.lexicalSemanticsAgreement(n2) ):
            return 0
            
        if ( not n1.syntacticAgreement(n2) ):
            return 0
            
        # anaphor link accepted, compute and return a score for it
        return self.getRelativeSalience(n1,n2)
        
        
        
    # FIND ANTECEDENT FOR node
    def findAntecedent(self,node):
        
        if (node.antecedentLink != None):
            # already processed
            return
            
        # Check for anaphoric relations
        # Anaphoric NPs or NOUNs (limited to Proper Nouns for now)
        # print "## FIND ANTEC for:",node.form,node.pos
        if ( ( (node.pos=="NP") or ( (node.pos=="NOUN") and (node.next != None) and (node.next.unifyfeats("proper:~")) ) ) and (node.unifyfeats("proper") or node.unifyfeats("prof") ) ):
            #print "###Looking for antecedents to:",node.form,node.id 
            for x in self.referents:
                if ( not (x.unifyfeats("proper") and x.preceedsInText(node)) ):
                    #print "!!!!IGNORED:",x.form,x.id
                    continue
                #print "     TRY COREF:",x.form,x.id,"<-",node.form
                score = self.properNameCoref(x,node)
                if (score > 0):
                    # COREF !!!!!
                    #print "       Succeeded! Rule:",self.rule,"\n"
                    self.addAnaphoricLink(x,node,score,'ProperNameCoref')
                    node.markPreTerminalsAs("anaphoric",None)
                    break;
                else:
                    self.printFailureReasons(x,node)
                    
                    
        # Pronouns
        elif ( (node.pos !="NP") and node.unifyfeats("anaphoric,terminal") ):
            node.markPreTerminalsAs("anaphoric",None)
            #print "###Looking for antecedents to:",node.form,node.id
            
            # Special case of "I", "We" etc. within quoted expressions
            if ( node.unifyfeats("p1") and node.saidBy):
                if (node.saidBy.antecedentLink):
                    self.addAnaphoricLink(node.saidBy.antecedentLink.antec,node,1,'Pronoun')
                else:
                    self.addAnaphoricLink(node.saidBy,node,1,'Pronoun')
                    
            else:
                # regular pronoun cases
                i = len(self.referents)-1
                while (i >= 0):
                    x = self.referents[i]
                    i = i-1
                    if (not ( (x.pos=="NP") or x.unifyfeats("person") ) ):
                        # x and node are in same NP chunk
                        continue
                    if (x.lastAnaphor):
                        lastref = x.lastAnaphor.id
                    else:
                        lastref = 'NONE'
                    #print "    TRY COREF:",x.form,x.id,'LastRef:',lastref,"<-",node.form
                    score = self.pronounCoref(x,node)
                    if (score > 0 ):
                        #print "       Succeeded! Rule:",self.rule,'=>RefDist:',self.getRefDist(x,node),"\n"
                        self.addAnaphoricLink(x,node,score,'Pronoun')
                        #if (len(node.antecedentLinks)>10):
                         #   break
                    else:
                        self.printFailureReasons(x,node)
                    
        # ALL cases: if anaphor case found, select the most probabale anaphor
        # link, and sort the list of candidates (for debuging purposes)  
        if (node.antecedentLink):
            node.antecedentLink.antec.addAnaphor(node.antecedentLink)
            if (len(node.antecedentLinks) > 1):
                node.antecedentLinks.sort(node.antecedentLink.compare)
            
            if (node.anaphorLinks != []):
                # we found an antecedent for "node" AND "node" is itself an antecedent for other anaphors: e.g this happens in case of pronouns of 1st person in quoted expressions, where the "sayer" is a following node which had not been processed yet for coreference by the time it was assigned as an antecedent to the 1st person pronoun. It can also happen in case of cataphora
                # if we are in such situation, then all the anaphors of "node" must be transfered as anaphors of the antecedent of "node"
                for a in node.anaphorLinks:
                    a.antec = node.antecedentLink.antec # change the antec
                    node.antecedentLink.antec.addAnaphor(a)
                    
                # transfer completed, delete the anaphor list of "node"
                node.anaphorLinks = []
                

                    
    def printFailureReasons(self,n1,n2):
        ifictif = 0
        #print "       Failed. Rule:",self.rule,n1.rule,"\n"
                    
                    
                    
    # For each sentence parsed by xip, preprocess it and store it
    def processSentence(self,sentparse):
        
        # Resolve SaidBy relations
        #print '!!!!!!!!!! HEY HEY'
        
        if ( sentparse.preterminals!=[] and sentparse.preterminals[0].insideQuotes):
            if (sentparse.preterminals[0].saidBy == None):
                if (self.lastSayer != None):
                    for preterm in sentparse.preterminals:
                        if (preterm.insideQuotes):
                            preterm.setSaidByUp(self.lastSayer)
                            #print ''
                        else:
                            break
                else:
                    self.toBeResolved.append(sentparse)
            else:
                self.lastSayer = sentparse.preterminals[0].saidBy
                # IF the preceding unresolved citation sentences are too far
                # from current sentence (more than 2 sentence-distance), the
                # forget them (do not use current SAYER for them)
                if ( (self.toBeResolved!=[]) and ( (self.sentNb - self.toBeResolved[len(self.toBeResolved)-1].preterminals[0].sentenceNb) > 2) ):
                    self.toBeResolved = []
                    
                for s in self.toBeResolved:
                    for preterm in s.preterminals:
                        if (preterm.insideQuotes):
                            preterm.setSaidByUp(self.lastSayer)
                            #print ''
                        else:
                            break
                self.toBeResolved = []

        # Store current sentence 'saidBy' for the following potentially 'quoted' sentences
        if (sentparse.preterminals!=[] and sentparse.preterminals[len(sentparse.preterminals)-1].form == ':'):
            i = len(sentparse.preterminals)-2
            while ( (i>=0) and sentparse.preterminals[i].unifyfeats("indspeech:~")):
                i = i - 1
            if ( (i>=0) and sentparse.preterminals[i].unifyfeats("verb,indspeech") ):
                self.lastSayer = sentparse.preterminals[i].getDependent('SUBJ-N')
                #print '!!!!!!!!', self.lastSayer.form, 'is a Sayer in',sentparse.root.form 
        else:
            i = len(sentparse.preterminals)-1
            while ( (i>=0) and sentparse.preterminals[i].saidBy==None):
                i = i - 1
            if ( i>=0 ):
                self.lastSayer = sentparse.preterminals[i].saidBy
            else:
                self.lastSayer = None
        
        
        # concat preterminals
        self.preterminals[len(self.preterminals):] = sentparse.preterminals
        self.sentParses.append(sentparse)
        self.sentNb = self.sentNb + 1
        
        
        
    def process(self):
        for sentparse in self.sentParses:
            keys = sentparse.nodes.keys()
            keys.sort(sentparse.compareNodes)
            
            for k in keys:
                node = sentparse.nodes[k]
                
                # If possible cataphor
                if (node.getHead("CATAPHOR")):
                    refnode = node.getHead("CATAPHOR")
                    # if node is a potential cataphor, then delay its processing
                    # until the referent of which it is potentially a cataphor
                    # is encountered
                    self.cataphorCandidates.append([refnode.id, node])
                else:
                    if (node.saidBy and node.unifyfeats("pron")):
                        # in case node is a pronoun in a quotation, make sure that the sayer of the quotation is processed for coreference before the quoted nodes !!!
                        self.findAntecedent(node.saidBy)
                    self.findAntecedent(node)
                
                # Store potential antecedents (NPs)
                if ( (node.antecedentLink==None) and ( (node.pos == "NP") or ( (node.pos=="NOUN") and (node.next != None) and (node.unifyfeats("person") or node.unifyfeats("org")) ) ) and node.unifyfeats("pron:~,adj:~") and (node.getHead("APPOS")==None) and (node.getHead("ATTRIB")==None or node.getHead("ATTRIB").lemma=='this') ):
                    TextParseDB.storeReferent(node)
                    
                # Process the delayed cataphor candidates when it's time to
                i = len(self.cataphorCandidates) -1
                while (i >= 0):
                    if (node.id == self.cataphorCandidates[i][0]):
                        #print "<<<<<<CATAPHOR CANDIDATE, at node:",node.form,node.id
                        self.findAntecedent(self.cataphorCandidates[i][1])
                        self.cataphorCandidates.pop(i)
                    i = i-1
                
        return None


    def savedb(self):
        global cursor
        for node in self.referents:
            for a in node.anaphorLinks:
                addinfodb(cursor,node.lemma,a.anaphor.lemma,a.anaphor.sentenceNb,a.anaphor.offleft,a.anaphor.offright,a.anaphor.pos)

        
        
    # Print Parses : Nodes + References
    def myprint(self):
        return
        for sentparse in self.sentParses:
            keys = sentparse.nodes.keys()
            keys.sort(sentparse.compareNodes)
            for k in keys:
                node = sentparse.nodes[k]
                # if (node.isPreTerminal()):
                node.myprint()
                    
            # print antecedents
        print "================= Discourse Referents =============="
        for node in self.referents:
            print node.id,node.pos,node.form,":",node.referentSalience
            for a in node.anaphorLinks:
                print " ".ljust(8),a.anaphor.id,a.anaphor.form,'/ (w=',a.score,')'
                
                
    # Print Annotated text in HTML format
    def printAnnotatedText(self,outf):

        stdout = sys.stdout
        # sys.stdout = outf

        
        outf.write('<SCRIPT language="Javascript" type="text/javascript">\n\n');

        for sentparse in self.sentParses:
            keys = sentparse.nodes.keys()
            for k in keys:
                node = sentparse.nodes[k]
                lemmainfo = ''
                if (node.isPreTerminal()):
                    lemmainfo = '<L> --->Lemma: ' + node.lemma + '</L>'
                    
                if (node.parent):
                    if (node.parent.pos == 'TOP'):
                        parentinfo = '<NLPL id=' + node.parent.id + '> #TOP </NLPL>'
                    else:
                        parentinfo = '<NLPL id=' + node.parent.id + '>' + node.parent.pos + ': ' + node.parent.form +  '</NLPL>'
                propsinfo=''
                for d in node.props:
                    propsinfo = propsinfo + d + ' |'
                headsinfo = ''
                depsinfo = ''
                for d in node.deps:
                    if (d.args[0]==node): # head
                        depsinfo = depsinfo + d.name + ' is: <NLPL id=' + d.args[1].id + '>' + d.args[1].getSafeForm() + '</NLPL> | '
                    else:
                        headsinfo = headsinfo + d.name + ' of: <NLPL id=' + d.args[0].id + '>' +  d.args[0].getSafeForm() + '</NLPL> | '
                        
                antecinfo = ''
                for a in node.antecedentLinks:
                    antecinfo = antecinfo + '<BR> <B>antec (' + str(a.score) + '):</B> <NLPL id=' + a.antec.id + '>' + a.antec.form + '</NLPL> '
                    
                outf.write('nodeInfoTab["' + node.id + '"] = "<NLP> <B> <POS>' + node.pos + ':</B></POS> <F>' + node.getSafeForm() + ' (' + node.id + ')</F>' + lemmainfo + ' <BR> <B>Features:</B>' + node.features.replace(',', ', ') + '<BR> <B>Parent:</B>' + parentinfo + '<BR> <B>Props:</B> ' + propsinfo + '<BR> <B>ARG2:</B> ' + headsinfo + '<BR> <B>ARG1:</B> ' + depsinfo + '<BR> <B>Salience: </B>' + str(node.referentSalience) + '<BR>' + antecinfo + '</NLP>";\n\n')
                
        outf.write('</SCRIPT>\n\n')
        
        
        inside_quotes = False
        tags = []
        i = 0
        while (i < len(self.preterminals)):
            n = self.preterminals[i]
            
            form = n.form
            x = n
            while ( x != None ):
                
                if (x.anaphorLinks != []):
                    if (x.id == '0:0:122'):
                        x.myprint()
                        #print 'ANAPHORS:',x.anaphorLinks
                        
                    markup = NLPMarkup('antec',x.loffset,x.roffset, n.sentenceNb, '', str(x.referentId))
                    
                    addUnique(tags,markup)
                    
                if (x.antecedentLink != None):
                    antec_form = x.antecedentLink.antec.form + ' : ' + str(x.antecedentLink.antec.referentId)
                    
                    markup = NLPMarkup('anaphor',x.loffset,x.roffset, n.sentenceNb, antec_form, str(x.antecedentLink.antec.referentId))
                    
                    addUnique(tags,markup)
                        
                elif (x.isAnaphoric):
                    markup = NLPMarkup('anaphoric',x.loffset,x.roffset,n.sentenceNb, '','')
                    
                    addUnique(tags,markup)
                    
                    
                if (x.pos=='NP'):
                    break
                x = x.parent
                
                
                
            if (n.pos == 'CONJ' and n.unifyfeats('coord')):
                coorditems = n.getDependents("COORD")
                coordforms = ''
                for item in coorditems:
                    coordforms = coordforms + ' *' + item.form
                markup = NLPMarkup('coord',n.loffset,n.roffset,n.sentenceNb, coordforms,'')
                tags.insert(0,markup)
            
            # recovering quotes
            quote2 = ''
            if (n.insideQuotes):
                if (not inside_quotes):
                    outf.write('"')
                    inside_quotes = True
                    
                    if (n.saidBy):
                        comment = n.saidBy.form
                        if (n.saidBy.antecedentLink):
                            comment = comment + ' (' + n.saidBy.antecedentLink.antec.form + ' : ' + str(n.saidBy.antecedentLink.antec.referentId) + ')'
                        markup = NLPMarkup('quoted',n.loffset,-1,n.sentenceNb,comment,'')
                        addUnique(tags,markup)
                            
                if ( (i+1) < len(self.preterminals) and not self.preterminals[i+1].insideQuotes):
                    # close "
                    quote2 = '"'
                    for tag in tags:
                        if (tag.markuptype == 'quoted'):
                            tag.cpos = n.roffset
                            tag.sentenceNb = n.sentenceNb
                    
            elif (inside_quotes):
                inside_quotes = False
            
            markup = NLPMarkup('nlpl',n.loffset,n.roffset,n.sentenceNb,n.id,'')
            tags.append(markup)
            
            # printing
            
            for tag in tags:
                if (tag.opos == n.loffset and tag.sentenceNb==n.sentenceNb):
                    outf.write(tag.otag)
                
            outf.write(form)
            
            j = len(tags) - 1
            while ( j >= 0):
                if ( tags[j].cpos == n.roffset and tags[j].sentenceNb==n.sentenceNb ):
                    outf.write(tags[j].ctag)
                    tags.pop(j)
                j = j - 1
                
            outf.write(quote2 + " ")
            
            i = i + 1

            if (n.lemma=="." and ( n.unifyfeats("guille:~") or ( (i<len(self.preterminals)) and self.preterminals[i].unifyfeats("guille:~") ) ) ):
                outf.write("<BR><BR>\n")
                
        
        
        
    #########################################################################"

    # Print Annotated text in HTML format
    def printAnnotatedTextLight(self,outf):

        
        stdout = sys.stdout
        # sys.stdout = outf

        
        inside_quotes = False
        tags = []
        i = 0
        while (i < len(self.preterminals)):
            n = self.preterminals[i]
            
            form = n.form
            x = n
            while ( x != None ):
                
                if (x.anaphorLinks != []):
                    #if (x.id == '0:0:122'):
                     #   x.myprint()
                      #  print 'ANAPHORS:',x.anaphorLinks
                        
                    markup = NLPMarkup('antec',x.loffset,x.roffset, n.sentenceNb, '', str(x.referentId))
                    
                    addUnique(tags,markup)
                    
                if (x.antecedentLink != None):
                    antec_form = x.antecedentLink.antec.form + ' : ' + str(x.antecedentLink.antec.referentId)
                    
                    markup = NLPMarkup('anaphor',x.loffset,x.roffset, n.sentenceNb, antec_form, str(x.antecedentLink.antec.referentId))
                    
                    addUnique(tags,markup)
                        
                elif (x.isAnaphoric):
                    markup = NLPMarkup('anaphoric',x.loffset,x.roffset,n.sentenceNb, '','')
                    
                    addUnique(tags,markup)
                    
                    
                if (x.pos=='NP'):
                    break
                x = x.parent
                
            
            # recovering quotes
            quote2 = ''
            if (n.insideQuotes):
                if (not inside_quotes):
                    outf.write('"')
                    inside_quotes = True
                    
                    if (n.saidBy):
                        comment = n.saidBy.form
                        if (n.saidBy.antecedentLink):
                            comment = comment + ' (' + n.saidBy.antecedentLink.antec.form + ' : ' + str(n.saidBy.antecedentLink.antec.referentId) + ')'
                        markup = NLPMarkup('quoted',n.loffset,-1,n.sentenceNb,comment,'')
                        addUnique(tags,markup)
                            
                if ( (i+1) < len(self.preterminals) and not self.preterminals[i+1].insideQuotes):
                    # close "
                    quote2 = '"'
                    for tag in tags:
                        if (tag.markuptype == 'quoted'):
                            tag.cpos = n.roffset
                            tag.sentenceNb = n.sentenceNb
                    
            elif (inside_quotes):
                inside_quotes = False
            
            # markup = NLPMarkup('none',n.loffset,n.roffset,n.sentenceNb,n.id,'')
            # tags.append(markup)
            
            # printing
            
            for tag in tags:
                if (tag.opos == n.loffset and tag.sentenceNb==n.sentenceNb):
                    outf.write(tag.otag)
                
            outf.write(form)
            
            j = len(tags) - 1
            while ( j >= 0):
                if ( tags[j].cpos == n.roffset and tags[j].sentenceNb==n.sentenceNb ):
                    outf.write(tags[j].ctag)
                    tags.pop(j)
                j = j - 1
                
            outf.write(quote2 + " ")
            
            i = i + 1

            if (n.lemma=="." and ( n.unifyfeats("guille:~") or ( (i<len(self.preterminals)) and self.preterminals[i].unifyfeats("guille:~") ) ) ):
                outf.write("<BR><BR>\n")
                
        
        
        
    def myPrintHeader(self,outf):
        #header = '<!DOCTYPE HTML>\n<HTML>\n\n <HEAD> \n\n <link rel="stylesheet" type="text/css" href="/home/ait/parSem/xip/dev/GRAMMARS/ENGLISH/GRMFILES/COREF/corefstyle.css" /> \n\n </HEAD> \n\n <BODY>'
        
        xipcorefdir = os.environ.get('XIPCOREFDIR')
        
        header = '<HTML>\n\n<HEAD> \n\n<meta http-equiv="Content-Type" content="text/html;charset=utf-8" /> \n\n<link rel="stylesheet" type="text/css" href="' + xipcorefdir + '/corefstyle.css" /> \n\n<script type="text/javascript" src="' + xipcorefdir + '/nlpinspect.js"><!-- nlpInspect: a javascript utiliy to view and inspect the details of text nlp analysis --></script> \n\n</HEAD> \n\n<BODY> \n\n\n'
        
        outf.write(header)
        
        return True
        
    def myPrintDocEnd(self,outf):
        end_tags = '\n</BODY> \n</HTML>\n'
        outf.write(end_tags)
    


####################################################
TextParseDB = TextParse()

####################################################


def coref_process_sentence_db(i,sentNb):
    global TextParseDB

    TextParseDB.sentNb=sentNb;
    sentparse = SentenceParse(i,TextParseDB.sentNb)
    #if (len(TextParseDB.referents) < 43):
    TextParseDB.processSentence(sentparse)


def coref_process_sentence(i):
    global TextParseDB


    sentparse = SentenceParse(i,TextParseDB.sentNb)
    #if (len(TextParseDB.referents) < 43):
    TextParseDB.processSentence(sentparse)
