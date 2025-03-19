/*
 * CorefResolver.java
 *
 * Created on June 6, 2006, 4:14 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import java.text.NumberFormat;
import java.util.*;
import com.xerox.jxip.*;

/**
 *
 * @author ait
 */
public class CorefResolver {
    
    private String grmFile;
    private JXip jxip;
    private int gHandler;
    private XipResultManager xipResultManager;
    
    private String failRule;
    /** The node representing the "sayer" of the last found quotation */
    private ReferentNode lastSayerNode = null;
    private int numberOfSentences=0;
    
    private boolean traceOn = true;
    static private NumberFormat numberFormat = NumberFormat.getNumberInstance();
    
    /** Creates a new instance of CorefResolver */
    public CorefResolver(String xip_grammar_file) {
        this.setGrammar(xip_grammar_file);
        this.init();
    }
    
    /** Creates a new instance of CorefResolver
     * @param xipParser an instance of JXip (the parser)
     * @param gHandler a handle (an int) of a grammar loaded by xipParser.
     */
    public CorefResolver(JXip xip_parser, int g_handler) {
        xipResultManager = new XipResultManager();
        jxip = xip_parser;
        gHandler = g_handler;
        this.init();
    }
    
    public void setGrammar(String xip_grammar_file) {
        xipResultManager = new XipResultManager();
        this.grmFile = xip_grammar_file;
        // loads the xip grammar
        try {
            System.out.println("CorefResolver: Loading grammar file: " + grmFile);
            // create a JXIP object to load the dynamic library in the VM
            jxip = new JXip();
            System.out.println(jxip.GetVersion());
            // load the grammar
            gHandler = jxip.XipGrmFile(grmFile, 0, true);
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }
    
    public void reloadGrammar() {
        try {
            gHandler = jxip.XipGrmFile(grmFile, 0, true);
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }
    
    /** This method is invoked by the XIP callback mecanism for each
     * parsed sentence. It stores the sentence parses and resolve
     * quotation authors */
    public void processSentence() {
        SentenceParse sentparse=null;
        XipUnit xipunit=null;
        
        // collect xipresult
        // create equivalent SentenceParse object (nodes + deps)
        XipResult xipres = this.xipResultManager.getXipResult();
        xipunit = (XipUnit) xipres.getXipUnits().elementAt(0);
        
        this.numberOfSentences++;
        sentparse = new SentenceParse(xipunit, numberOfSentences);
        
        // process quotation authors
        if (sentparse.startsWithQuotes()) {
            if (sentparse.getSayerNode(0) == null) {
                this.delayedForSayerResol.add(sentparse);
            } else {
                this.lastSayerNode = sentparse.getSayerNode(0);
                // if the preceding unresolved sentence citation are contiguous
                // to this, then the sayer of this is the sayer of those
                if (!this.delayedForSayerResol.isEmpty() &&
                        (sentparse.sentenceId -
                        this.delayedForSayerResol.lastElement().sentenceId) == 1) {
                    for(int i=0; i < this.delayedForSayerResol.size(); i++) {
                        this.delayedForSayerResol.elementAt(i).setSayerNode(0,this.lastSayerNode);
                    }
                    this.delayedForSayerResol.clear();
                }
            }
        } else {
            processDelayedSentenceCitations(sentparse);
        }
        
        Vector<XipLeaf> leaves = sentparse.getLeaves();
        int i = leaves.size()-1;
        if (leaves.elementAt(i).getSurface().compareTo(":")==0) {
            // sentence ending with a semicolon ":", it could be a "say" sentence
            i--;
            while (i>=0 && leaves.elementAt(i).fMatches("INDSPEECH","~"))
                i--;
            if ( i>=0 && leaves.elementAt(i).flMatches("VERB,INDSPEECH") ) {
                XipNode xipnode = leaves.elementAt(i).getMother().getDependentIn("SUBJ-N");
                ReferentNode refnode = new ReferentNode(xipnode,numberOfSentences);
                xipnode.setMiscInfo(refnode);
                this.lastSayerNode = refnode;
            }
            
        }
        
        //System.out.println(sentparse.toString() );
        
        // Now store the sentence parse
        this.sentParses.add(sentparse);
        
    }
    
    /**
     * Process the delayed sentence citations with no sayer
     * @param sentparse the current sentence parse (==null if end of text)
     */
    private void processDelayedSentenceCitations(SentenceParse sentparse) {
        // If the preceding unresolved citation sentences are too far
        // from current sentence (more than 2 sentence-distance), then
        // set their sayer to lastSayer or forget them
        if (!this.delayedForSayerResol.isEmpty()) {
            if (this.lastSayerNode!=null &&
                    ( sentparse==null || ( (sentparse.sentenceId -
                    this.delayedForSayerResol.lastElement().sentenceId) == 1) ) ) {
                for(int i=0; i < this.delayedForSayerResol.size(); i++) {
                    this.delayedForSayerResol.elementAt(i).setSayerNode(0,this.lastSayerNode);
                }
            }
            this.delayedForSayerResol.clear();
            this.lastSayerNode = null;
        }
    }
    
    private static final float W_RELATIVE_SALIENCE=2;
    private static final float W_SEMANTIC_MATCHING=1;
    
    
    /** Current list of discourse referents */
    private ReferentSet referents = new ReferentSet();
    /** List of anaphors found */
    private Vector<ReferentNode> anaphors = new Vector<ReferentNode>(40);
    
    
    /** current list of sentences which contain quotations but for which
     * no sayer has been found yet. They are stored here in order to be processed
     * later when a sayer is available (in one of the following sentences) */
    private Vector<SentenceParse> delayedForSayerResol = new Vector<SentenceParse>();
    
    private Vector<SentenceParse> sentParses = new Vector<SentenceParse>();
    
    /** the Rule that is used to resolve current coreference */
    private String rule = "";
    
    
    public Vector<SentenceParse> getSentenceParses() {
        return sentParses;
    }
    
    public void setTrace(boolean b) {
        this.traceOn = b;
    }
    
    static public enum RefType { NONE, NEW_REFERENT, ANAPHORIC_PRONOMINAL,
    ANAPHORIC_NOMINAL, PROPER};
    Comparator<XipNode> corefNodeComparator = new Comparator<XipNode>() {
        public int compare(XipNode n1, XipNode n2) {
            if (n1==n2.getHeadIn("CATAPHOR")) {
                return 1;
            } else if (n2==n1.getHeadIn("CATAPHOR")) {
                return -1;
            } else {
                return n1.compareTo(n2);
            }
        }
    };
    
    
    /** Opens docfilename and parses it invoking XIP with current xipgrammar.
     * Warning: After parsing each sentence, XIP callback mecanisms calls
     * processSentence() (method of this current object)
     * @param docfilename the filename of the document on which coreference
     * resolution is performed
     ************************************************************************/
    public ReferentSet resolveCoreferenceInFile(String docfilename, boolean utf8) {
        CorefResolver.RefType nodetype;
        
        // Cleaning
        this.init();
        if (utf8) {
            try {
                jxip.XipSetDisplayMode(gHandler, Const.UTF8_INPUT, 50);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        // call XIP with current grammar to parse the doc
	try {
            xipResultManager.xipResultBuilderFromTextFile(gHandler, docfilename, this, "processSentence");
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        if (utf8) {
            try {
                jxip.XipRemoveFlagDisplay(gHandler, Const.UTF8_INPUT);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        
        // process the last delayed citations
        processDelayedSentenceCitations(null);
        
        // resolve coreference
        ReferentNode node=null;
        ReferentNodeList relevant_nodes = new ReferentNodeList(5);
        SentenceParse sentparse = null;
        Iterator<SentenceParse> iter = this.sentParses.iterator();
        while (iter.hasNext()) {
            sentparse = iter.next();
            // System.out.println(sentparse.toString());
            
            
            this.getCorefRelevantNodes(sentparse, relevant_nodes);
            traceln(this.corefRelevantNodesToString(relevant_nodes));
            
            for(int i=0; i < relevant_nodes.size();i++) {
                node = relevant_nodes.elementAt(i);
                resolveCoreferenceFor(node, sentparse);
            }
            
        }
        // DONE
        return this.referents;
    }
    
    public ReferentSet resolveCoreferenceInString(String text, boolean utf8) {
        CorefResolver.RefType nodetype;
        if (utf8) {
            try {
                jxip.XipSetDisplayMode(gHandler, Const.UTF8_INPUT, 50);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        // Cleaning
        this.init();
        // call XIP with current grammar to parse the text string
        try {
            xipResultManager.xipResultBuilderFromString(gHandler, text, this, "processSentence");
        } catch (Exception ex) {
             ex.printStackTrace();
        }
        if (utf8) {
            try {
                jxip.XipRemoveFlagDisplay(gHandler, Const.UTF8_INPUT);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        // process the last delayed citations
        processDelayedSentenceCitations(null);
        
        // resolve coreference
        ReferentNode node=null;
        ReferentNodeList relevant_nodes = new ReferentNodeList(5);
        SentenceParse sentparse = null;
        Iterator<SentenceParse> iter = this.sentParses.iterator();
        while (iter.hasNext()) {
            sentparse = iter.next();
            //traceln(sentparse.toString());
            
            this.getCorefRelevantNodes(sentparse, relevant_nodes);
            traceln(this.corefRelevantNodesToString(relevant_nodes));
            
            for(int i=0; i < relevant_nodes.size();i++) {
                node = relevant_nodes.elementAt(i);
                resolveCoreferenceFor(node, sentparse);
            }
        }
        // DONE
        return this.referents;
    }
    
    private void resolveCoreferenceFor(ReferentNode node, SentenceParse sentparse) {
        RefType nodetype = node.getRefType();
        if (nodetype==RefType.NEW_REFERENT) {
            traceAddNewReferent(node);
            this.referents.addNewReferent(node);
        } else if (nodetype==RefType.PROPER) {
            // PROPER
            if (!this.resolveProperCoreference(node)) {
                traceAddNewReferent(node);
                this.referents.addNewReferent(node);
            }
        } else if (nodetype==RefType.ANAPHORIC_PRONOMINAL) {
            // ANAPHORIC PRONOUN
            // check if node occurs inside a quotation and
            // is "said" by another node
            this.resolvePronominalCoreference(node, sentparse);
        } else if (nodetype==RefType.ANAPHORIC_NOMINAL) {
            // ANAPHORIC NPs and NOUNs
            if (!this.resolveNominalCoreference(node)) {
                traceAddNewReferent(node);
                this.referents.addNewReferent(node);
            }
        }
    }
    
    private void getCorefRelevantNodes(SentenceParse sentparse, ReferentNodeList relevant_nodes) {
        Vector<XipDependency> deps = sentparse.getDependencies();
        //  Sort the deps in the good order: the deps with only
        // one parameter ("COREF_") should come first, and among them
        // a dep preceeds another dep if its param node preceeds the
        // param node of the other dep
        Collections.sort(deps,this.depComparator);
        RefType reftype=RefType.NONE;
        relevant_nodes.clear();
        for(int i=0; i<deps.size(); i++) {
            XipDependency dep =  deps.elementAt(i) ;
            if (dep.getParameters().size()>1) {
                // finished processing one-parameter deps, so stop
                break;
            }
            reftype =  getRefType( dep );
            if (reftype!=RefType.NONE) {
                XipNode n = dep.getParameters().elementAt(0);
                if (n.getMiscInfo()!=null) {
                    // a refnode have already been created for this xipnode
                    continue;
                }
                // check if it's a potential cataphor
                XipNode catanode = n.getHeadIn("CATAPHOR");
                if (catanode!=null && !relevant_nodes.containsXipNode(catanode) ) {
                    RefType reftype2 = getRefType(catanode);
                    ReferentNode catarefnode =
                            new ReferentNode(catanode,reftype2,sentparse.sentenceId);
                    catanode.setMiscInfo(catarefnode);
                    relevant_nodes.add(catarefnode);
                }
                // Check if it is part of a quotation
                ReferentNode sayernode = sentparse.getSayerNodeOf(n);
                if (sayernode!=null && !sayernode.isAlreadyHandled()) {
                    RefType reftype2 = getRefType(sayernode.getNode());
                    sayernode.setRefType(reftype2);
                    sayernode.setAlreadyHandled(true);
                    relevant_nodes.add(sayernode);
                }
                ReferentNode refnode = new ReferentNode(n,reftype,sentparse.sentenceId);
                n.setMiscInfo(refnode);
                relevant_nodes.add(refnode);
            }
        }
        
    }
    
    /** */
    private RefType getRefType(XipDependency dep) {
        if (dep.getParameters().size()==1 &&
                dep.getName().startsWith("COREF_") ) {
            if (dep.getName().contentEquals("COREF_PROPER"))
                return RefType.PROPER;
            else if (dep.getName().contentEquals("COREF_ANAPHORIC_NOMINAL"))
                return RefType.ANAPHORIC_NOMINAL;
            else if (dep.getName().contentEquals("COREF_ANAPHORIC_PRONOMINAL"))
                return RefType.ANAPHORIC_PRONOMINAL;
            else if (dep.getName().contentEquals("COREF_NEW_REFERENT"))
                return RefType.NEW_REFERENT;
            else
                return RefType.NONE;
        } else {
            return RefType.NONE;
        }
    }
    
    private RefType getRefType(XipNode n) {
        Vector<XipDependency> deps = n.getDependencies();
        RefType reftype = RefType.NONE;
        for(int i=0;i<deps.size();i++) {
            reftype = getRefType( deps.elementAt(i) );
            if (reftype!=RefType.NONE)
                return reftype;
        }
        return RefType.NONE;
    }
    
    /** */
    public String corefRelevantNodesToString(ReferentNodeList relevant_nodes) {
        ReferentNode node;
        String s = "-----CorefReleventNodes:\n";
        for(int i=0; i < relevant_nodes.size();i++) {
            node = relevant_nodes.elementAt(i);
            s = s + node.toLightValueString() + "  (" +
                    node.getRefType().toString() + ")\n";
        }
        return s;
    }
    
    
    /** Tries to resolve coreference in case of ProperNames */
    private boolean resolveProperCoreference(ReferentNode node) {
        traceTryingFindAntecedents(node);
        Referent r=null;
        int i=0;
        for(i=this.referents.size()-1; i>=0; i--) {
            r = this.referents.elementAt(i);
            traceTryingWithReferent(r);
            if (node.getLemma().compareTo(r.getLemma())==0) {
                this.rule = "N1 and N2 have identical lemma strings";
                break;
            } else if (this.similarProperNameHeads(r,node)) {
                this.rule = "N1 and N2 have similar heads, and N2 is included in N1";
                break;
            } else if (r.fMatches("PERSON") && !node.fMatches("ORG")) {
                if (this.haveCompatiblePersonNames(r,node)) {
                    this.rule = "N1 and N2 have compatible person names";
                    break;
                }
            } else if (node.isAbbr() && r.fMatches("ORG")) {
                String abbr = r.getAbbr();
                if (Util.matchesAbbr(abbr,node.getSurface())) {
                    this.rule = "N2 is an abbreviation of N1";
                    break;
                }
            } else if (r.isAbbr() && !node.isAbbr() && node.fMatches("ORG")) {
                // process ABBREVIATION in the reverse order
                String abbr = node.getAbbr();
                if (Util.matchesAbbr(abbr,r.getSurface())) {
                    this.rule = "N1 is an abbreviation of N2";
                    break;
                }
            }
            traceScore(0);
        }
        
        if (i>=0) {
            traceScore(1);
            node.setAntecedentLink(r, 1, this.rule, referents);
            anaphors.add(node);
            traceSuccess(r);
            return true;
        } else {
            return false;
        }
    }
    
    
    private boolean resolvePronominalCoreference(ReferentNode node, SentenceParse sentparse) {
        traceTryingFindAntecedents(node);
        Referent r=null;
        Vector<CoreferenceLinkCandidate> clcandidates = new Vector<CoreferenceLinkCandidate>(3);
        float score=0, totalscores = 0;
        int i=0;
        for(i=this.referents.size()-1; i>=0; i--) {
            r = this.referents.elementAt(i);
            traceTryingWithReferent(r);
            if (!this.corefConstraintsSatisfiedBy(r,node,sentparse)) {
                score=0;
            } else {
                score = this.getPronominalCoreferenceScore(r,node,sentparse);
            }
            traceScore(score);
            if (score>0) {
                this.rule = "N2 is a pronoun that co-refers with N1";
                CoreferenceLinkCandidate cl = new CoreferenceLinkCandidate(r,score,this.rule);
                clcandidates.add(cl);
                totalscores = totalscores + score;
            }
        }
        if (clcandidates.isEmpty()) {
            anaphors.add(node);
            traceFail();
            return false;
        } else {
            // this.normalizeScores(clcandidates, totalscores);
            Collections.sort(clcandidates);
            node.setAntecedentLinkCandidates(clcandidates, referents);
            anaphors.add(node);
            traceSuccess(clcandidates.firstElement().referent);
            return true;
        }
    }
    
    
    
    private boolean resolveNominalCoreference(ReferentNode node) {
        traceTryingFindAntecedents(node);
        Referent r=null;
        XipNode headnode = node.getChunkHead();
        Vector<CoreferenceLinkCandidate> clcandidates = new Vector<CoreferenceLinkCandidate>(3);
        float totalscores = 0;
        int i=0;
        for(i=this.referents.size()-1; i>=0; i--) {
            r = this.referents.elementAt(i);
            traceTryingWithReferent(r);
            float score = this.getNominalCoreferenceScore(r,
                    new ReferentNode(headnode,RefType.NONE,node.getSentenceId()) );
            traceScore(score);
            if (score>0) {
                this.rule = "N2 is a definite NP the head of which is a semantic property of N1";
                CoreferenceLinkCandidate cl = new CoreferenceLinkCandidate(r,score,this.rule);
                clcandidates.add(cl);
                totalscores = totalscores + score;
            }
        }
        if (clcandidates.isEmpty()) {
            traceFail();
            return false;
        } else {
            // this.normalizeScores(clcandidates, totalscores);
            Collections.sort(clcandidates);
            node.setAntecedentLinkCandidates(clcandidates, referents);
            anaphors.add(node);
            traceSuccess(clcandidates.firstElement().referent);
            return true;
        }
    }
    
    
    /*******************************************************************/
    
    private boolean similarProperNameHeads(Referent r, ReferentNode node) {
        XipNode x1=null, x2=null;
        
        if ( !r.getMainNode().getDaughters().isEmpty() )
            x1 = (XipNode) r.getMainNode().getDaughters().lastElement();
        
        if ( !node.getDaughters().isEmpty() )
            x2 = (XipNode) node.getDaughters().lastElement();
        
        if (x1==null || x2==null ||
                x1.getSurface().compareTo(x2.getSurface())!=0 )
            return false;
        
        int i=r.getMainNode().getDaughters().size() - 2;
        int j = node.getDaughters().size() - 2;
        while (i>=0 && j>=0) {
            x1 = (XipNode) r.getMainNode().getDaughters().elementAt(i);
            x2 = (XipNode) node.getDaughters().elementAt(j);
            if (x1.getSurface().compareTo(x2.getSurface())!=0)
                break;
            i--;
            j--;
        }
        if (j<0) {
            return true;
        } else if (i<0 && j==0 &&
                node.getDaughters().elementAt(0).getCategory().contentEquals("DET")) {
            // N2 is equal to  a DET + N1 ????
            return true;
        } else {
            return false;
        }
    }
    
    private boolean haveCompatiblePersonNames(Referent r, ReferentNode node) {
        boolean complname=false, compfname1=false, compfname2=false;
        
        XipNode last1 = r.getDependentIn("INTERNALS_LASTNAME");
        XipNode last2 = node.getDependentIn("INTERNALS_LASTNAME");
        
        if (last1!=null && last2!=null &&
                !last1.getSurface().contentEquals(last2.getSurface())) {
            // Both have a lastname, and the lastnames are different
            return false;
        }
        // check firstnames
        XipNode f1 = r.getDependentIn("INTERNALS_FIRSTNAME1");
        XipNode f2 = node.getDependentIn("INTERNALS_FIRSTNAME1");
        if (f1!=null && f2!=null &&
                !f1.getSurface().contentEquals(f2.getSurface()) &&
                !Util.isNameInitialOf(f2.getSurface(),f1.getSurface()) &&
                !Util.isNameInitialOf(f1.getSurface(),f2.getSurface())   ) {
            return false;
        }
        
        if (last1==null && last2==null && f1==null && f2==null ) {
            return false;
        }
        
        if (f2==null && last2==null &&
                f1!=null && !node.getSurface().contentEquals(f1.getSurface()) )
            // if N2 has no firstname and no lastname, and N1 has a firstname
            // different from the N2 surface form, then FALSE
            return false;
        
        if (f2==null && last2==null &&
                last1!=null && !node.getSurface().contentEquals(last1.getSurface()) )
            // if N2 has no firstname and no lastname, and N1 has a lastname
            // different from the N2 surface form, then FALSE
            return false;
        
        if (f1==null && last1==null &&
                f2!=null && !r.getSurface().contentEquals(f2.getSurface()) )
            // if N1 has no firstname and no lastname, and N2 has a firstname
            // different from the N1 surface form, then FALSE
            return false;
        
        if (f1==null && last1==null &&
                last2!=null && !r.getSurface().contentEquals(last2.getSurface()) )
            // if N1 has no firstname and no lastname, and N2 has a lastname
            // different from the N1 surface form, then FALSE
            return false;
        
        // check middlenames
        f1 = r.getDependentIn("INTERNALS_FIRSTNAME2");
        f2 = node.getDependentIn("INTERNALS_FIRSTNAME2");
        if (f1!=null && f2!=null &&
                !f1.getSurface().contentEquals(f2.getSurface()) &&
                !Util.isNameInitialOf(f2.getSurface(),f1.getSurface()) &&
                !Util.isNameInitialOf(f1.getSurface(),f2.getSurface()) ) {
            return false;
        }
        return true;
    }
    
    private float getNominalCoreferenceScore(Referent r, ReferentNode node) {
        float score = referents.getSemanticMatchingScore(r,node);
        if (score==0)
            return 0;
        score = (this.W_SEMANTIC_MATCHING*score +
                this.W_RELATIVE_SALIENCE*
                referents.getRelativeSalienceOf(r,node) ) /
                (this.W_SEMANTIC_MATCHING + this.W_SEMANTIC_MATCHING ) ;
        return score;
    }
    
    
    private void normalizeScores(Vector<CoreferenceLinkCandidate> clcandidates, float totalscores) {
        for(int i=0; i<clcandidates.size(); i++) {
            clcandidates.elementAt(i).score = clcandidates.elementAt(i).score/totalscores;
        }
    }
    
    
    /** Checks coreference constraints
     */
    private boolean corefConstraintsSatisfiedBy(Referent r, ReferentNode n, SentenceParse sentparse) {
        // Number, gender and person
        if (!r.agreesInNumberWith(n) || !r.agreesInGenderWith(n) )
            return false;
        
        // quotation constraints
        if ( r.isSayerOf( n.getNode(), sentparse ) ) {
            if (!n.fMatches("P1"))
                return false;
            else
                return true;
        } else {
            if (!r.agreesInPersonWith(n)) {
                return false;
            }
        }
        
        // Lexical-Semantic constraints
        if (!this.lexsemConstraintsSatisfiedBy(r,n)) {
            return false;
        }
        
        // Syntactic constaints
        if (!this.syntacticConstraintsSatisfiedBy(r,n)) {
            return false;
        }
        return true;
    }
    
    private float getPronominalCoreferenceScore(Referent r, ReferentNode node, SentenceParse sentparse) {
        return referents.getRelativeSalienceOf(r,node);
    }
    
    private boolean lexsemConstraintsSatisfiedBy(Referent r, ReferentNode n) {
        this.failRule = "Lexical/semantic agreement: ";
        if (r.fMatches("PERSON")) {
            if (n.flMatches("PRON,SG,PERSON:~,C_PERSON:~")) {
                this.failRule = this.failRule + "N1 is a person, N2 is not.";
                return false;
            }
        } else if (n.fMatches("PERSON") || n.fMatches("C_PERSON")) {
            if (!r.fMatches("PERSON")) {
                this.failRule = this.failRule + "N2 is a person, N1 is not.";
                return false;
            }
        }
        return true;
    }
    
    private boolean syntacticConstraintsSatisfiedBy(Referent r, ReferentNode n2) {
        // Syntactic constraints should occur between xip nodes
        ReferentNode n1 = r.getLastNode();
        if (n1.getSentenceId()==n2.getSentenceId()) {
            long dist = n2.getLeftTokOffset() - n1.getRightTokOffset();
            if (Math.abs(dist) <=1) {
                this.failRule = "Distance between N1 and N2 is <= 1";
                return false;
            } else if (dist < 0 && !n1.isRelHeadOf(n2,"CATAPHOR") ) {
                this.failRule = "N2 preceeds N1 but there's no potential cataphora between N1 and N2";
                return false;
            }
        }
        XipNode verb = n2.getHeadIn("OBJ");
        if (!n2.fMatches("REFL")) {
            if ( verb!=null && verb.isRelHeadOf(n1.getNode(),"SUBJ-N") ) {
                this.failRule = "N2 is not REFL:+, and N2 is the OBJ of the verb of which N1 is the SUBJ";
                return false;
            }
        } else {
            if (verb!=null && !verb.isRelHeadOf(n1.getNode(),"SUBJ-N")) {
                this.failRule = "N2 is REFL:+, and N2 is not the OBJ of the verb of which N1 is the SUBJ";
                return false;
            }
        }
        //
        verb = n1.getDependentIn("MOD","verb");
        if (verb!=null) {
            if (verb.getDependentIn("MOD","PRON,REL")!=null &&
                    n2.isRelDependentOf(verb) ) {
                this.failRule = "N2 is dependent of the main verb of a relative clause that modifies N1";
                return false;
            }
        }
        return true;
    }
    
    void printResults() {
        referents.print();
        System.out.println("ANAPHORS:\n"+anaphors.toString());
    }
    
    
    public ReferentNodeList getCoreferenceNodesFromFile(String textfile, boolean utf8) {
        this.resolveCoreferenceInFile(textfile, utf8);
        ReferentNodeList corefnodes =
                new ReferentNodeList(this.referents.size()+this.anaphors.size());
        for(int i=0; i<this.referents.size();i++) {
            corefnodes.add(this.referents.elementAt(i).getFirstNode());
        }
        corefnodes.addAll(this.anaphors);
        //Collections.sort(corefnodes, ReferentNode.comparator1);
        return corefnodes;
    }
    
    public ReferentNodeList getCoreferenceNodesFromString(String text, boolean utf8) {
        this.resolveCoreferenceInString(text, utf8);
        ReferentNodeList corefnodes =
                new ReferentNodeList(this.referents.size()+this.anaphors.size());
        for(int i=0; i<this.referents.size();i++) {
            corefnodes.add(this.referents.elementAt(i).getFirstNode());
        }
        corefnodes.addAll(this.anaphors);
        //Collections.sort(corefnodes, ReferentNode.comparator1);
        return corefnodes;
        
    }
    
    private void init() {
        this.referents.init();
        this.anaphors.clear();
        this.delayedForSayerResol.clear();
        this.failRule = "";
        this.lastSayerNode = null;
        this.rule = "";
        this.sentParses.clear();
        this.numberOfSentences=0;
    }
    
    
    
    /**
     * Determines the order in which the Nodes will be first processed for
     * coreference.
     * Note: Later, this order might be changed in resolveCorefFor() : if a node
     * is a cataphor candidate or occurs in a quotation that has a sayer node,
     * then the other node is processed first
     **/
    public static Comparator<XipDependency> depComparator = new Comparator<XipDependency>() {
        public int compare(XipDependency d1, XipDependency d2) {
            if (d1.getParameters().size() <  d2.getParameters().size() ) {
                return -1;
            } else if (d1.getParameters().size() ==  d2.getParameters().size() ) {
                if (d1.getParameters().size()==1) {
                    return d1.getParameters().elementAt(0).compareTo(
                            d2.getParameters().elementAt(0));
                } else {
                    return 0;
                }
            } else {
                return 1;
            }
        }
    };
    
    
    void trace(String info) {
        if (traceOn==false)
            return;
        System.out.print(info );
    }
    void traceln(String info) {
        if (traceOn==false)
            return;
        System.out.println(info );
    }
    
    private void traceAddNewReferent(ReferentNode node) {
        traceln("Adding a new referent: " +
                node.getSurface() + "  (" + node.getTokId() + ")" );
    }
    
    private void traceTryingFindAntecedents(ReferentNode node) {
        traceln("Trying to find antecedents for: " +
                node.getSurface() + "   (" + node.getTokId() + ")" +
                "  (" + node.getRefType().toString() + ") ... " );
    }
    
    private void traceTryingWithReferent(Referent r) {
        trace("     Trying with: " +
                r.getFirstNode().getSurface() +
                "  (" + r.getFirstNode().getTokId() + ")" );
    }
    
    private void traceScore(float score) {
        traceln(" --> score=" + numberFormat.format(score));
    }
    
    private void traceSuccess(Referent r) {
        traceln("    SUCCESS!! linked with: " +
                r.getFirstNode().getSurface() + "   (" +
                r.getFirstNode().getTokId() + ")" );
    }
    
    private void traceFail() {
        traceln("");
        return;
    }
    
    
    
    
    
    
}

