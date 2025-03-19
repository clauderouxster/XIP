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
    
    static public enum RefType { NONE, NEW_REFERENT, ANAPHORIC_PRONOMINAL,
    ANAPHORIC_NOMINAL, PROPER};
    
    private String failRule;
    private SentenceProcessor sentenceProcessor;
    /** Current list of discourse referents */
    private ReferentSet referents = new ReferentSet();
    /** List of anaphors found */
    private Vector<Referent> anaphors = new Vector<Referent>(40);
    /** the Rule that is used to resolve current coreference */
    String rule = "";
    private CorefAlgo corefAlgo = new CorefAlgo();
    
    
    
    /** Creates a new instance of CorefResolver */
    public CorefResolver(String xip_grammar_file) {
        this.sentenceProcessor = new SentenceProcessor(xip_grammar_file);
        this.init();
    }
    
    /** Creates a new instance of CorefResolver
     * @param xipParser an instance of JXip (the parser)
     * @param gHandler a handle (an int) of a grammar loaded by xipParser.
     */
    public CorefResolver(JXip xip_parser, int g_handler) {
        this.sentenceProcessor = new SentenceProcessor(xip_parser, g_handler);
        this.init();
    }
    
    
    public ReferentList getReferentListFromFile(String textfile, boolean utf8, boolean xmlFlag) {
        this.resolveCoreferenceInFile(textfile, utf8, xmlFlag);
        ReferentList all_referents =
                new ReferentList(this.referents.size()+this.anaphors.size());
        all_referents.addAll(this.referents);
        all_referents.addAll(this.anaphors);
        //Collections.sort(all_referents, ReferentNode.comparator1);
        return all_referents;
    }
    
    public ReferentList getReferentListFromString(String text, boolean utf8) {
        this.resolveCoreferenceInString(text, utf8);
        //this.referents.print();
        ReferentList all_referents =
                new ReferentList(this.referents.size()+this.anaphors.size());
        all_referents.addAll(this.referents);
        all_referents.addAll(this.anaphors);
        //Collections.sort(all_referents, ReferentNode.comparator1);
        return all_referents;
        
    }
    
    
    /** Opens docfilename and parses it invoking XIP with current xipgrammar.
     * Warning: After parsing each sentence, XIP callback mecanisms calls
     * processSentence() (method of this current object)
     * @param docfilename the filename of the document on which coreference
     * resolution is performed
     ************************************************************************/
    public ReferentSet resolveCoreferenceInFile(String docfilename, boolean utf8flag, boolean xmlFlag) {
        // Cleaning
        this.init();
        this.sentenceProcessor.processDocument(docfilename, utf8flag, xmlFlag);
        //Tracer.setTraceFile("trace.log");
        // resolve coreference
        Referent ref=null;
        ReferentList relevant_referents = new ReferentList(5);
        SentenceParse sentparse = null;
        Iterator<SentenceParse> iter = this.sentenceProcessor.sentParses.iterator();
        while (iter.hasNext()) {
            sentparse = iter.next();
            //System.out.println(sentparse.toHtmlString());
            
            this.getCorefRelevantReferents(sentparse, relevant_referents);
            Tracer.traceln(this.corefRelevantReferentsToString(relevant_referents));
            
            for(int i=0; i < relevant_referents.size();i++) {
                ref = relevant_referents.elementAt(i);
                if (corefAlgo.resolveCoreferenceFor(ref, this.referents)!=null)
                    this.anaphors.add(ref);
                else
                    this.referents.addNewReferent(ref);
            }
            
        }
        // DONE
        return this.referents;
    }
    
    
    public ReferentSet resolveCoreferenceInString(String text, boolean utf8flag) {
        this.init();
        this.sentenceProcessor.processString(text, utf8flag);
        // resolve coreference
        Referent ref=null;
        ReferentList relevant_referents = new ReferentList(5);
        SentenceParse sentparse = null;
        Iterator<SentenceParse> iter = this.sentenceProcessor.sentParses.iterator();
        while (iter.hasNext()) {
            sentparse = iter.next();
            //traceln(sentparse.toString());
            
            this.getCorefRelevantReferents(sentparse, relevant_referents);
            Tracer.traceln(this.corefRelevantReferentsToString(relevant_referents));
            
            for(int i=0; i < relevant_referents.size();i++) {
                ref = relevant_referents.elementAt(i);
                if (corefAlgo.resolveCoreferenceFor(ref, this.referents)!=null)
                    this.anaphors.add(ref);
                else
                    this.referents.addNewReferent(ref);
            }
        }
        // DONE
        return this.referents;
    }
    
    
    
    
    private void getCorefRelevantReferents(SentenceParse sentparse, ReferentList relevant_referents) {
        Vector<XipDependency> deps = sentparse.getDependencies();
        //  Sort the deps in the good order: the deps with only
        // one parameter ("COREF_") should come first, and among them
        // a dep preceeds another dep if its param node preceeds the
        // param node of the other dep
        Collections.sort(deps,this.depComparator);
        RefType reftype=RefType.NONE;
        relevant_referents.clear();
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
                    // a refnode has already been created for this xipnode
                    continue;
                }
                // check if it's a potential cataphor
                XipNode catanode = n.getHeadIn("CATAPHOR");
                if (catanode!=null && !relevant_referents.containsXipNode(catanode) ) {
                    RefType reftype2 = getRefType(catanode);
                    Referent cataref =
                            new Referent(catanode,sentparse.sentenceId, reftype2);
                    catanode.setMiscInfo(cataref);
                    relevant_referents.add(cataref);
                }
                // Check if it is part of a quotation
                Referent sayer = sentparse.getSayerOf(n);
                if (sayer!=null && !sayer.isAlreadyHandled()) {
                    RefType reftype2 = getRefType(sayer.getNode());
                    sayer.setRefType(reftype2);
                    sayer.setAlreadyHandled(true);
                    relevant_referents.add(sayer);
                }
                Referent ref = new Referent(n,sentparse.sentenceId,reftype);
                ref.setSaidBy(sayer);
                n.setMiscInfo(ref);
                relevant_referents.add(ref);
            }
        }
        
    }
    
    private void init() {
        this.referents.init();
        this.anaphors.clear();
        this.sentenceProcessor.init();
        this.failRule = "";
        this.rule = "";
        
    }
    
    public void reloadGrammar() {
        this.sentenceProcessor.reloadGrammar();
    }
    
    public Vector<SentenceParse> getSentenceParses() {
        return this.sentenceProcessor.getSentenceParses();
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
    
    /**  for debugging */
    public String corefRelevantReferentsToString(ReferentList relevant_referents) {
        Referent r;
        String s = "-----CorefReleventNodes:\n";
        for(int i=0; i < relevant_referents.size();i++) {
            r = relevant_referents.elementAt(i);
            s = s + r.toLightValueString() + "  (" +
                    r.getRefType().toString() + ")\n";
        }
        return s;
    }
    
    
    private void normalizeScores(Vector<CoreferenceLink> clcandidates, float totalscores) {
        for(int i=0; i<clcandidates.size(); i++) {
            clcandidates.elementAt(i).score = clcandidates.elementAt(i).score/totalscores;
        }
    }
    
    
    void printResults() {
        referents.print();
        //System.out.println("ANAPHORS:\n"+anaphors.toString());
    }
    
    
    public void setTrace(boolean b) {
        Tracer.setTrace(b);
    }
    
    
    
    /**
     * Determines the order in which the Nodes will be first processed for
     * coreference.
     * Note: Later, this order might be changed in resolveCorefFor() : if a node
     * is a cataphor candidate or occurs in a quotation that has a sayer node,
     * then the other node is processed first
     **/
    
    
    private Comparator<XipNode> corefNodeComparator = new Comparator<XipNode>() {
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
    
    
    
}

