/*
 * ReferentNode.java
 *
 * Created on September 11, 2006, 7:13 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipFeature;
import com.xerox.jxip.XipNode;
import java.util.Comparator;
import java.util.Vector;

/**
 *
 * @author ait
 */
public class ReferentNode implements Comparable {
    
    private static final float W_SUBJ=1;
    private static final float W_SUBJ_N=0.6f;
    private static final float W_OBJ=0.3f;
    private static final float W_OBJ_N=0.2f;
    private static final float W_IOBJ=0.25f;
    private static final float W_MOD_verb=0.15f;
    private static final float W_MOD_noun=0.08f;
    private static final float W_QUANTD_verb=0.05f;
    
    static public enum Type { ANTECEDENT, COREFERENT};
    private Type type;
    private CorefResolver.RefType refType;
    private XipNode node;
    private long sentenceId=0;
    private Referent referent=null;
    private int occurrenceNr=-1;
    private float salience=-1;
    // for anaphor/cataphor nodes only
    private CoreferenceLink[] antecedentLinkCandidates=null;
    //
    private boolean alreadyHandled=false;
    
    
    /**
     * Creates a new instance of ReferentNode
     */
    public ReferentNode(XipNode n, CorefResolver.RefType t, long sentid) {
        refType = t;
        node = n;
        sentenceId = sentid;
        if (refType==CorefResolver.RefType.ANAPHORIC_PRONOMINAL) {
            type = Type.COREFERENT;
        }
    }
    
    public ReferentNode(XipNode n, long sentid) {
        refType = CorefResolver.RefType.NONE;
        node = n;
        sentenceId = sentid;
    }
    
    public boolean equals(ReferentNode n2) {
        return (this.getLeft()==n2.getLeft() && this.getRight()==n2.getRight());
    }
    
    public boolean isAntecedent() {
        return !isCoreferent();
    }
    
    public boolean isCoreferent() {
        if (type==Type.COREFERENT)
            return true;
        else
            return false;
    }
    
    public boolean isAlreadyHandled() {
        return alreadyHandled;
    }
    public void setAlreadyHandled(boolean b) {
        alreadyHandled = b;
    }
    
    public void setRefType(CorefResolver.RefType t) {
        this.refType = t;
    }
    
    public long getSentenceId() {
        return sentenceId;
    }
    
    public long getLeft() {
        return node.getLeft();
    }
    
    public long getRight() {
        return node.getRight();
    }
    
    public long getLeftTokOffset() {
        return node.getLeftTokOffset();
    }
    
    public long getRightTokOffset() {
        return node.getRightTokOffset();
    }
    
    public String getSurface() {
        if (node==null)
            return "";
        else
            return node.getSurface();
    }
    
    public String getLemma() {
        return node.getLemma();
    }
    
    public ReferentNode getAntecedent() {
        if (hasAntecedents())
            return antecedentLinkCandidates[0].antecedent;
        else
            return null;
    }
    
    public CoreferenceLink[] getAntecedentLinkCandidates() {
        if ( hasAntecedents() ) {
            return antecedentLinkCandidates;
        } else {
            return null;
        }
    }
    
    public CorefResolver.RefType getRefType() {
        return refType;
    }
    
    public ReferentNode.Type getType() {
        return type;
    }
    
    
    public void setAntecedentLink(CoreferenceLink cl) {
        this.antecedentLinkCandidates = new CoreferenceLink[1];
        this.antecedentLinkCandidates[0] = cl;
        this.type = Type.COREFERENT;
    }
    
    float getSalience() {
        /*if (salience>=0) {
            return salience;
        } */
        float val = 0;
        XipNode n=this.node, head=null;
        Vector<XipNode> heads = new Vector<XipNode>(4);
        
        head = n.getHeadIn("SUBJ");
        if (head!=null && ( head.fMatches("VERB") || head.fMatches("AUX") ) )  {
            val = val + W_SUBJ;
            heads.add(head);
        }
        head = n.getHeadIn("SUBJ-N");
        if (head!=null && !heads.contains(head) ) { //&& !this.contains(head) ) {
            val = val + W_SUBJ_N;
            heads.add(head);
        }
        head = n.getHeadIn("OBJ");
        if (head!=null && head.fMatches("VERB") && !heads.contains(head) ) {
            val = val + W_OBJ;
            heads.add(head);
        }
        head = n.getHeadIn("OBJ-N");
        if (head!=null && !heads.contains(head) ) {
            val = val + W_OBJ_N;
            heads.add(head);
        }
        head = n.getHeadIn("IOBJ");
        if (head!=null && head.fMatches("VERB") && !heads.contains(head) ) {
            val = val + W_IOBJ;
            heads.add(head);
        }
        head = n.getHeadIn("MOD");
        if (head!=null && head.fMatches("VERB") && !heads.contains(head) ) {
            val = val + W_MOD_verb;
            heads.add(head);
        }
        head = n.getHeadIn("MOD");
        if (head!=null && head.fMatches("NOUN") && !heads.contains(head) ) {
            val = val + W_MOD_noun;
            heads.add(head);
        }
        head = n.getHeadIn("QUANTD");
        if (head!=null && head.fMatches("VERB") && !heads.contains(head) ) {
            val = val + W_QUANTD_verb;
            heads.add(head);
        }
        
        if (val==0) {
            val=0.04f;
        } else if (val>1) {
            val = 1;
        }
        salience = val;
        return salience;
    }
    
    void setReferent(Referent ref) {
        referent = ref;
    }
    
    public XipNode getNode() {
        return node;
    }
    
    /** A compareto method for Collections.sort() to work properly
     */
    public int compareTo(Object o) {
        ReferentNode n2 = (ReferentNode) o;
        if (this.getLeft() < n2.getLeft() )
            return -1;
        else if (this.getLeft() == n2.getLeft() )
            if (this.getRight() < n2.getRight() )
                return -1;
            else if (this.getRight() == n2.getRight() )
                return 0;
            else
                return 1;
        else
            return 1;
    }
    
    Vector<XipNode> getDaughters() {
        return node.getDaughters();
    }
    
    public String getCategory() {
        return this.getNode().getCategory();
    }
    
    
    XipNode getHeadIn(String depname) {
        return node.getHeadIn(depname);
    }
    
    boolean fMatches(String f) {
        return node.fMatches(f);
    }
    
    XipNode getDependentIn(String depname) {
        return node.getDependentIn(depname);
    }
    
    boolean isAbbr() {
        return ( Util.isUpper(this.getSurface()) && this.getSurface().length()<=6 );
    }
    
    String getAbbr() {
        return this.getAbbr(this.getNode());
    }
    
    String getAbbr(XipNode n) {
        StringBuffer s = new StringBuffer();
        if (n.isLeaf() || n.isFicticious())
            return "";
        else if (n.isPreTerminalNode()) {
            if (n.getCategory().contentEquals("DET") ||
                    n.getCategory().contentEquals("PREP") )
                return "";
            else {
                String form = n.getSurface();
                boolean firstchar = true;
                for(int i=0; i<form.length(); i++) {
                    if (firstchar && Character.isUpperCase(form.charAt(i))) {
                        s.append(form.charAt(i));
                        firstchar = false;
                    } else if (form.charAt(i)==' ' || form.charAt(i)=='-') {
                        firstchar = true;
                    }
                }
            }
        } else {
            for(int i=0; i < n.getDaughters().size(); i++) {
                String abbr = this.getAbbr( n.getDaughters().elementAt(i) );
                s.append(abbr);
            }
        }
        return s.toString();
    }
    
    boolean flMatches(String featstr) {
        return this.getNode().flMatches(featstr);
    }
    
    Vector<XipFeature> getFeatures() {
        return this.getNode().getFeatures();
    }
    
    Vector<XipDependency> getDependencies() {
        return this.getNode().getDependencies();
    }
    
    void setAntecedentLink(Referent r, float s, String rule, ReferentSet referents) {
        CoreferenceLink cl = new CoreferenceLink(r,this,s,rule);
        this.antecedentLinkCandidates = new CoreferenceLink[1];
        this.antecedentLinkCandidates[0] = cl;
        this.type = Type.COREFERENT;
        referents.addNewAnaphor(this, r, s, rule);
    }
    
    
    void setAntecedentLinkCandidates(Vector<CoreferenceLinkCandidate> clcs, ReferentSet referents) {
        CoreferenceLink[] coref_links = new CoreferenceLink[clcs.size()];
        for(int i=0; i<clcs.size(); i++) {
            CoreferenceLink cl =
                    new CoreferenceLink(clcs.elementAt(i).referent, this,
                    clcs.elementAt(i).score, clcs.elementAt(i).rule);
            coref_links[i] = cl;
        }
        this.antecedentLinkCandidates = coref_links;
        this.type = Type.COREFERENT;
        referents.addNewAnaphor(this, clcs.firstElement().referent,
                clcs.firstElement().score, clcs.firstElement().rule);
    }
    
    public Referent getReferent() {
        return referent;
    }
    
    public String getReferentId() {
        return this.getReferent().getId();
    }
    
    public String getId() {
        return this.getSentenceId() + ":" + this.getNode().getId();
    }
    
    public String getTokId() {
        return this.getSentenceId() + ":" + this.getLeftTokOffset() + "-" +
                this.getRightTokOffset() + ":" + this.getNode().getIndex();
    }
    
    public boolean hasCoreferents() {
        return (this.isAntecedent() && this.getReferent().getReferentNodes().size()>1);
    }
    
    public boolean hasAntecedents() {
        return this.antecedentLinkCandidates!=null;
    }
    
    /**
     *  Gets the chunk head of this node
     *
     * @return    The chunk head node of this node if this node is a chunk;
     *            this node if this node is not a chunk (i.e. if it is
     * preterminal or terminal node)
     */
    public synchronized XipNode getChunkHead() {
        return this.getNode().getChunkHead();
    }
    
    boolean isRelHeadOf(ReferentNode n2, String depname) {
        return this.getNode().isRelHeadOf(n2.getNode(), depname);
    }
    
    XipNode getDependentIn(String depname, String featstr) {
        return this.getNode().getDependentIn(depname, featstr);
    }
    
    boolean isRelDependentOf(XipNode n) {
        return this.getNode().isRelDependentOf(n);
    }
    
    public static Comparator<ReferentNode> comparator1 = new Comparator<ReferentNode>() {
        public int compare(ReferentNode n1, ReferentNode n2) {
            if (n1.getSentenceId() < n2.getSentenceId())
                return -1;
            else if (n1.getSentenceId() < n2.getSentenceId())
                if (n1.getLeft() < n2.getLeft())
                    return -1;
                else if (n1.getLeft() == n2.getLeft())
                    if (n1.getRight()>n2.getRight())
                        return -1;
                    else
                        return 1;
                else
                    return 1;
            else
                return 1;
        }
    };
    
    public String toString() {
        String s = this.getTokId() + ":" + this.getSurface();
        if (this.hasAntecedents()) {
            for(int i=0; i < this.getAntecedentLinkCandidates().length; i++) {
                CoreferenceLink cl = this.getAntecedentLinkCandidates()[i];
                s = s + "\n       (" + cl.getScoreString() + ") " + cl.antecedent.getSurface() ;
                s += " (last:" + cl.lastAntecedent.getTokId() + ")";
                s += " (S=" + cl.referentSalience + ",  D=" +
                        cl.lastAntecedent.getDistanceOf(this) +  ")" ;
            }
            s = s + "\n";
        }
        return s;
    }
    
    public String toLightValueString() {
        return this.getTokId() + ":" + node.getSurface() ;
    }
    
    public String toNodeParseString() {
        String s = this.getTokId() + ":" + this.getCategory() + ": " + this.getSurface();
        s = s + "\n   " + this.getFeatures().toString();
        s = s + "\n   " + this.getDependencies().toString();
        return s;
    }
    
    /** For GUI annotations
     */
    public int getStart() {
        return (int) getLeft();
    }
    public int getEnd() {
        return (int) getRight();
    }
    
    /**
     * for GUI annotations
     */
    
    public String getToolTipText() {
        return toString();
    }
    
    public String getClickText() {
        return toString(); //toNodeParseString() + "\nSalience=" + getSalience();
    }
    
    public String getVisibleText() {
        if (this.hasCoreferents() || this.hasAntecedents() ) {
            return getReferentId();
        } else {
            return null;
        }
    }
    
    private boolean contains(XipNode node) {
        if (node.getLeft() >= this.getLeft() && node.getRight()<=this.getRight())
            return true;
        else
            return false;
    }
    
    
    float getDistanceOf(ReferentNode n) {
        float d = Math.abs( n.getSentenceId() - this.getSentenceId() );
        d = d + 1;
        if (d==1) { // same sentence
            d += 1 - ( 10 / ( (float) 2*Util.getAbsoluteTokenDistanceBetween(this, n) + 10 ) ) ;
        } else if (d==2) { // previous sentence
            d +=  10 / ( (float) this.getLeftTokOffset() + 10 );
        } else if (d>2) {
            d = (float) Math.pow(3,d) + 10;
        }
        return d;
    }
    
    void setOccurrenceNr(int i) {
        this.occurrenceNr = i;
    }
    
    int getOccurrenceNr() {
        return this.occurrenceNr;
    }
    
    
    
}
