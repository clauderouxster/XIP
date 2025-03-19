package jcoref;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipFeature;
import com.xerox.jxip.XipNode;
import java.util.Vector;
/*
 * Referent.java
 *
 * Created on September 11, 2006, 8:33 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

/**
 *
 * @author ait
 */
public class Referent implements Comparable {
    
    private static long referentNb=0;
    
    /** The id of this referent; should be unique through the list of current referents */
    private long id = 0;
    /** Current salience score for this referent */
    private float salience = -1;
    
    private ReferentNodeList referentNodes = new ReferentNodeList(5);
    
    /** Cumulative morpho-syntactic and semantic features for this referent */
    private Vector<XipFeature> features = new Vector<XipFeature>(4);
    
    /** Cumulative dependency relations for this referent (relations with nodes) */
    //private Vector<XipDependency> relations = new Vector(4);
    private Vector<String> properties = new Vector<String>(4);
    
    private String abbreviation;
    
    /** Creates a new instance of Referent */
    public Referent(ReferentNode firstnode) {
        this.referentNb++;
        this.id = this.referentNb;
        this.addReferentNode(firstnode);
        salience = firstnode.getSalience();
    }
    
    
    public static void init() {
        referentNb=0;
    }
    
    float getSalience() {
        return salience;
    }
    
    ReferentNode getFirstNode() {
        return referentNodes.firstElement();
    }
    
    String getLemma() {
        return getFirstNode().getLemma();
    }
    
    ReferentNode getMainNode() {
        return getFirstNode();
    }
    
    ReferentNode getLastNode() {
        return referentNodes.lastElement();
    }
    
    private void addFeatures(Vector<XipFeature> fs) {
        for(int i=0; i<fs.size(); i++) {
            if (fs.elementAt(i).getName().contentEquals("SG") ||
                    fs.elementAt(i).getName().contentEquals("PL") ||
                    fs.elementAt(i).getName().contentEquals("MASC") ||
                    fs.elementAt(i).getName().contentEquals("FEM") ||
                    fs.elementAt(i).getName().contentEquals("PERSON") ||
                    fs.elementAt(i).getName().contentEquals("ORG"))
                this.addFeature(fs.elementAt(i));
        }
    }
    
    private void addFeature(XipFeature f) {
        if (!this.fMatches(f.getName(),f.getValue()))
            this.features.add(f);
    }
    
    private void addDependencies(Vector<XipDependency> deps) {
        for(int i=0; i<deps.size(); i++) {
            if (deps.elementAt(i).getParameters().size()==1) {
                if (deps.elementAt(i).getName().contentEquals("PERSON") ||
                        deps.elementAt(i).getName().contentEquals("PERSON") ) {
                    this.addProperty(deps.elementAt(i).getName());
                }
            } else if (deps.elementAt(i).getParameters().size()==2) {
                if (deps.elementAt(i).getName().contentEquals("INTERNALS_TITLE") ) {
                    XipNode n  = (XipNode) deps.elementAt(i).getParameters().elementAt(1);
                    this.addProperty(n.getLemma());
                }
            }
        }
    }
    
    
    private void addProperty(String propstr) {
        if (!this.hasProperty(propstr))
            this.properties.add(propstr.toLowerCase());
    }
    
    
    public boolean hasProperty(String propstr) {
        for(int i=0; i<this.properties.size(); i++) {
            if (this.properties.elementAt(i).contentEquals(propstr))
                return true;
        }
        return false;
    }
    
    boolean isSayerOf(XipNode n, SentenceParse sentparse) {
        ReferentNode sayernode = sentparse.getSayerNodeOf(n);
        if ( sayernode!= null && sayernode.getReferent()==this ) {
            return true;
        } else {
            return false;
        }
    }
    
    
    public boolean fMatches(String f, String v) {
        
        for(int i=0; i<features.size(); i++) {
            if (f.compareToIgnoreCase(features.elementAt(i).getName())==0) {
                if (v.compareToIgnoreCase(features.elementAt(i).getValue())==0)
                    return true;
                else
                    return false;
            }
        }
        if (v.contentEquals("~"))
            return true;
        else
            return false;
    }
    
    public boolean fMatches(String f) {
        return this.fMatches(f,"+");
    }
    
    public boolean flMatches(String featstring) {
        String[] feats = featstring.split(",");
        if (feats.length==1 && feats[0].compareTo("")==0) {
            return true;
        } else {
            String fv, f, v;
            String[] temp;
            for(int i=0; i<feats.length; i++) {
                fv = feats[i];
                fv = fv.replace("!","");
                temp = fv.split(":");
                f = temp[0];
                if (temp.length==2)
                    v = temp[1];
                else
                    v = "+";
                if (!this.fMatches(f,v))
                    return false;
            }
            return true;
        }
    }
    
    float getDistanceOf(ReferentNode n) {
        if (!this.referentNodes.isEmpty())
            return this.getLastNode().getDistanceOf(n);
        else
            return 0;
    }
    
    
    
    /*
     float getDistanceOf(XipNode node, long sentid) {
        float d = Math.abs( sentid - this.getLastNode().getSentenceId() );
        d = d + 1;
        if (d==1) { // same sentence
            d = d + ( 1 - (1 / Util.getAbsoluteTokenDistanceBetween(this.getLastNode().getNode(), node) ) );
        } else if (d>2) {
            d = (float) Math.pow(3,d) + 30;
        }
        return d;
    }
     */
    
    XipNode getDependentIn(String depname) {
        return this.getMainNode().getDependentIn(depname);
    }
    
    String getAbbr() {
        if (this.abbreviation!=null)
            return this.abbreviation;
        else
            return this.getMainNode().getAbbr();
    }
    
    boolean isAbbr() {
        return this.getMainNode().isAbbr();
    }
    
    String getSurface() {
        return this.getMainNode().getSurface();
    }
    
    
    boolean agreesInNumberWith(ReferentNode n) {
        if (this.flMatches("SG,PL:~,ORG:~") &&
                n.fMatches("PL") && !n.fMatches("SG") )
            return false;
        else if (this.fMatches("PL") && !this.fMatches("SG") &&
                n.fMatches("SG") && !n.fMatches("PL"))
            return false;
        else
            return true;
    }
    
    boolean agreesInGenderWith(ReferentNode n) {
        if (this.fMatches("MASC") && !this.fMatches("FEM") &&
                n.fMatches("FEM") && !n.fMatches("MASC") )
            return false;
        else if (this.fMatches("FEM") && !this.fMatches("MASC") &&
                n.fMatches("MASC") && !n.fMatches("FEM"))
            return false;
        else
            return true;
    }
    
    boolean agreesInPersonWith(ReferentNode n) {
        if (this.fMatches("P3") != n.fMatches("P3") &&
                !this.flMatches("P1:~,P2:~,P3:~") && !n.flMatches("P1:~,P2:~,P3:~") )
            return false;
        else if (this.fMatches("P1") != n.fMatches("P1"))
            return false;
        else if (n.fMatches("P2") != n.fMatches("P2"))
            return false;
        else
            return true;
    }
    
    
    /** adds a coreferent node to the list of node occurrences of this referent */
    public void addReferentNode(ReferentNode n) {
        float d = this.getDistanceOf(n);
        if (d<2)
            d=1;
        this.salience = this.salience/d + n.getSalience();
        n.setOccurrenceNr(this.referentNodes.size());
        this.referentNodes.add(n);
        n.setReferent(this);
        this.addFeatures(n.getFeatures());
        this.addDependencies(n.getDependencies());
    }
    
    public int compareTo(Object o) {
        if (this.getSentenceId() < ((Referent) o).getSentenceId())
            return -1;
        else if (this.getSentenceId() == ((Referent) o).getSentenceId())
            return this.getFirstNode().compareTo(((Referent) o).getFirstNode());
        else
            return 1;
    }
    
    private long getSentenceId() {
        return this.getFirstNode().getSentenceId();
    }
    
    public String toString() {
        CoreferenceLink cl =null;
        String s=this.getId() + ": \"" + this.getSurface() +
                "\" (" + this.getFirstNode().getId() + "):\n";
        
        for (int i=1; i<this.referentNodes.size();i++) {
            ReferentNode n = this.referentNodes.elementAt(i);
            s = s + "        " + n.getId() + "/ \"" + n.getSurface() +
                    "\" (" + n.getAntecedentLinkCandidates()[0].getScore() +
                    ")      ---> RULE: " +
                    n.getAntecedentLinkCandidates()[0].getRule() + "\n";
        }
        return s;
    }
    
    public String getId() {
        return id+"";
    }
    
    public ReferentNodeList getReferentNodes() {
        return this.referentNodes;
    }

    ReferentNode getOccurrence(int i) {
        if (i<referentNodes.size())
            return referentNodes.elementAt(i);
        else
            return null;
    }
    
    
    
}
