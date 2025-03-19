/*
 * Referent.java
 *
 * Created on June 25, 2007, 6:36 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package jcoref;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipFeature;
import com.xerox.jxip.XipNode;
import java.util.Vector;

/**
 *
 * @author ait
 */
public class Referent implements Comparable<Referent> {

    private static final float W_SUBJ = 1;
    private static final float W_SUBJ_N = 0.6f;
    private static final float W_OBJ = 0.3f;
    private static final float W_OBJ_N = 0.2f;
    private static final float W_IOBJ = 0.25f;
    private static final float W_MOD_verb = 0.15f;
    private static final float W_MOD_noun = 0.08f;
    private static final float W_QUANTD_verb = 0.05f;
    private boolean alreadyHandled = false;
    private long id = 0;

    public enum SyntacticType {

        PROPER, PRONOUN, NOUN
    };

    public enum SemanticType {

        PERSON, ORG, LOCATION, ANY
    };

    public enum Properties {

        hasFirstname, hasMiddlename, hasLastname, hasPTitle, hasGender
    }
    /** Syntactic type */
    private CorefResolver.RefType syntacticType;
    /** Local salience (in the sentence where it occurs) of this referent */
    private float localSalience = 0;
    /** Current (global) salience score for this referent */
    private float salience = -1;
    public float resultingRefSalience = 0;
    private XipNode node = null;
    private long sentenceId = 0;
    private Referent saidBy = null;
    /** Cumulative morpho-syntactic and semantic features for this referent */
    /** morpho-syntactic features */
    private Vector<XipFeature> features = new Vector<XipFeature>(4);
    /** Cumulative dependency relations for this referent (relations with nodes) */
    private Vector<String> properties = new Vector<String>(4);
    private Vector<Relation> relations = new Vector<Relation>(4);
    private PersonProperties personProps;
    private OrgProperties orgProps;
    private Vector<CoreferenceLink> corefLinks = null;
    private Vector<CoreferenceLink> antecedentLinks = null;
    public String rule = "";

    /** Creates a new instance of Referent */
    public Referent(XipNode node, long sentence_id, CorefResolver.RefType stype) {
        this.syntacticType = stype;
        if (node.fMatches("PERSON")) {
            personProps = new PersonProperties(node);
        } else if (node.fMatches("ORG")) {
            orgProps = new OrgProperties(node);
        }
        this.node = node;
        this.addFeatures(this.node.getFeatures());
        this.addDependencies(this.node.getDependencies());
        this.sentenceId = sentence_id;
        this.localSalience = salience = this.getLocalSalience();
    }

    /** Get the initial (and local) salience of this referent */
    public float getLocalSalience() {
        if (localSalience > 0) {
            return localSalience;
        }
        float val = 0;
        XipNode n = this.node, head = null;
        Vector<XipNode> heads = new Vector<XipNode>(4);

        head = n.getHeadIn("SUBJ");
        if (head != null && (head.fMatches("VERB") || head.fMatches("AUX"))) {
            val = val + W_SUBJ;
            heads.add(head);
        }
        head = n.getHeadIn("SUBJ-N");
        if (head != null && !heads.contains(head)) { //&& !this.contains(head) ) {
            val = val + W_SUBJ_N;
            heads.add(head);
        }
        head = n.getHeadIn("OBJ");
        if (head != null && head.fMatches("VERB") && !heads.contains(head)) {
            val = val + W_OBJ;
            heads.add(head);
        }
        head = n.getHeadIn("OBJ-N");
        if (head != null && !heads.contains(head)) {
            val = val + W_OBJ_N;
            heads.add(head);
        }
        head = n.getHeadIn("IOBJ");
        if (head != null && head.fMatches("VERB") && !heads.contains(head)) {
            val = val + W_IOBJ;
            heads.add(head);
        }
        head = n.getHeadIn("MOD");
        if (head != null && head.fMatches("VERB") && !heads.contains(head)) {
            val = val + W_MOD_verb;
            heads.add(head);
        }
        head = n.getHeadIn("MOD");
        if (head != null && head.fMatches("NOUN") && !heads.contains(head)) {
            val = val + W_MOD_noun;
            heads.add(head);
        }
        head = n.getHeadIn("QUANTD");
        if (head != null && head.fMatches("VERB") && !heads.contains(head)) {
            val = val + W_QUANTD_verb;
            heads.add(head);
        }

        if (val == 0) {
            val = 0.04f;
        } else if (val > 1) {
            val = 1;
        }
        this.localSalience = val;
        return this.localSalience;
    }

    /**  Compute a coreference score between this referent and referent r
     */
    public float isCoreferentWithProper(Referent r) {

        if (r.getLemma().compareTo(getLemma()) == 0) {
            rule = "N1 and N2 have identical lemma strings";
            return 1;
        } else if (similarProperNameHeads(r)) {
            rule = "N1 and N2 have similar heads, and N2 is included in N1";
            return 1;
        } else if (this.getSemanticType() == SemanticType.PERSON) {
            if (r.getSemanticType() == SemanticType.PERSON &&
                    this.personProps.personIsCompatibleWith(r.getPersonProperties())) {
                return 1;
            } else {
                return 0;
            }
        } else if (this.getSemanticType() == SemanticType.ORG) {
            if (r.isAbbr()) {
                String abbr = getAbbr();
                if (Util.matchesAbbr(abbr, r.getSurface())) {
                    rule = "N2 is an abbreviation of N1";
                    return 1;
                }
            } else if (isAbbr() && !r.isAbbr() && r.fMatches("ORG")) {
                // process ABBREVIATION in the reverse order
                String abbr = r.getAbbr();
                if (Util.matchesAbbr(abbr, this.getAbbr())) {
                    rule = "N1 is an abbreviation of N2";
                    return 1;
                }
            }

        }
        return 0;
    }

    /** Check if this referent has a name head that is similar to the name head
     * of referent r
     */
    private boolean similarProperNameHeads(Referent r) {
        XipNode x1 = null, x2 = null;

        if (!this.getNode().getDaughters().isEmpty()) {
            x1 = (XipNode) this.getNode().getDaughters().lastElement();
        }
        if (!r.getNode().getDaughters().isEmpty()) {
            x2 = (XipNode) r.getNode().getDaughters().lastElement();
        }
        if (x1 == null || x2 == null ||
                x1.getSurface().compareTo(x2.getSurface()) != 0) {
            return false;
        }
        int i = this.getNode().getDaughters().size() - 2;
        int j = r.getNode().getDaughters().size() - 2;
        while (i >= 0 && j >= 0) {
            x1 = (XipNode) this.getNode().getDaughters().elementAt(i);
            x2 = (XipNode) r.getNode().getDaughters().elementAt(j);
            if (x1.getSurface().compareTo(x2.getSurface()) != 0) {
                break;
            }
            i--;
            j--;
        }
        if (j < 0) {
            return true;
        } else if (i < 0 && j == 0 &&
                r.getNode().getDaughters().elementAt(0).getCategory().contentEquals("DET")) {
            // N2 is equal to  a DET + N1 ????
            return true;
        } else {
            return false;
        }
    }

    boolean isAbbr() {
        return this.getNode().fMatches("ACRON");
    }

    /** Utilities
     ***********************************************************/
    private void addDependencies(Vector<XipDependency> deps) {
        for (int i = 0; i < deps.size(); i++) {
            if (deps.elementAt(i).getParameters().size() == 1) {
                if (deps.elementAt(i).getName().contentEquals("PERSON") ||
                        deps.elementAt(i).getName().contentEquals("PERSON")) {
                    this.addProperty(deps.elementAt(i).getName());
                }
            } else if (deps.elementAt(i).getParameters().size() == 2) {
                if (deps.elementAt(i).getName().contentEquals("INTERNALS_TITLE")) {
                    XipNode n = (XipNode) deps.elementAt(i).getParameters().elementAt(1);
                    this.addProperty(n.getLemma());
                }
            }
        }
    }

    private void addProperty(String propstr) {
        if (!this.hasProperty(propstr)) {
            this.properties.add(propstr.toLowerCase());
        }
    }

    public boolean hasProperty(String propstr) {
        for (int i = 0; i < this.properties.size(); i++) {
            if (this.properties.elementAt(i).contentEquals(propstr)) {
                return true;
            }
        }
        return false;
    }

    private void addFeatures(Vector<XipFeature> fs) {
        for (int i = 0; i < fs.size(); i++) {
            /* if (fs.elementAt(i).getName().contentEquals("SG") ||
            fs.elementAt(i).getName().contentEquals("PL") ||
            fs.elementAt(i).getName().contentEquals("MASC") ||
            fs.elementAt(i).getName().contentEquals("FEM") ||
            fs.elementAt(i).getName().contentEquals("PERSON") ||
            fs.elementAt(i).getName().contentEquals("ORG")) */
            this.addFeature(fs.elementAt(i));
        }
    }

    private void addFeature(XipFeature f) {
        if (!this.fMatches(f.getName(), f.getValue())) {
            this.features.add(f);
        }
    }

    public boolean fMatches(String f, String v) {

        for (int i = 0; i < features.size(); i++) {
            if (f.compareToIgnoreCase(features.elementAt(i).getName()) == 0) {
                if (v.compareToIgnoreCase(features.elementAt(i).getValue()) == 0) {
                    return true;
                } else {
                    return false;
                }
            }
        }
        if (v.contentEquals("~")) {
            return true;
        } else {
            return false;
        }
    }

    public boolean fMatches(String f) {
        return this.fMatches(f, "+");
    }

    public boolean flMatches(String featstring) {
        String[] feats = featstring.split(",");
        if (feats.length == 1 && feats[0].compareTo("") == 0) {
            return true;
        } else {
            String fv, f, v;
            String[] temp;
            for (int i = 0; i < feats.length; i++) {
                fv = feats[i];
                fv = fv.replace("!", "");
                temp = fv.split(":");
                f = temp[0];
                if (temp.length == 2) {
                    v = temp[1];
                } else {
                    v = "+";
                }
                if (!this.fMatches(f, v)) {
                    return false;
                }
            }
            return true;
        }
    }

    public String getLemma() {
        return node.getLemma();
    }

    public String getSurface() {
        return node.getSurface();
    }

    public long getSentenceId() {
        return sentenceId;
    }

    public XipNode getNode() {
        return this.node;
    }

    public SemanticType getSemanticType() {
        if (this.personProps != null) {
            return SemanticType.PERSON;
        } else if (this.orgProps != null) {
            return SemanticType.ORG;
        } else {
            return SemanticType.ANY;
        }
    }

    public CorefResolver.RefType getSyntacticType() {
        return this.syntacticType;
    }

    private PersonProperties getPersonProperties() {
        return this.personProps;
    }


    public boolean personIsCompatibleWith(PersonProperties pp) {
        return this.personProps.personIsCompatibleWith(pp);
    }


    public String getAbbr() {
        if (this.getSemanticType() == SemanticType.ORG) {
            return this.orgProps.getAbbr();
        } else {
            return null;
        }
    }

    float getSalience() {
        return this.salience;
    }

    boolean agreesInNumberWith(Referent r) {
        if (this.flMatches("SG,PL:~,ORG:~") &&
                r.fMatches("PL") && !r.fMatches("SG")) {
            return false;
        } else if (this.fMatches("PL") && !this.fMatches("SG") &&
                r.fMatches("SG") && !r.fMatches("PL")) {
            return false;
        } else {
            return true;
        }
    }

    boolean agreesInGenderWith(Referent r) {
        if (this.fMatches("MASC") && !this.fMatches("FEM") &&
                r.fMatches("FEM") && !r.fMatches("MASC")) {
            return false;
        } else if (this.fMatches("FEM") && !this.fMatches("MASC") &&
                r.fMatches("MASC") && !r.fMatches("FEM")) {
            return false;
        } else {
            return true;
        }
    }

    boolean agreesInPersonWith(Referent r) {
        if (this.fMatches("P3") != r.fMatches("P3") &&
                !this.flMatches("P1:~,P2:~,P3:~") && !r.flMatches("P1:~,P2:~,P3:~")) {
            return false;
        } else if (this.fMatches("P1") != r.fMatches("P1")) {
            return false;
        } else if (this.fMatches("P2") != r.fMatches("P2")) {
            return false;
        } else {
            return true;
        }
    }

    public String getTokId() {
        return this.getSentenceId() + ":" + this.getLeftTokOffset() + "-" +
                this.getRightTokOffset() + ":" + this.getNode().getIndex();
    }

    public long getLeftTokOffset() {
        return node.getLeftTokOffset();
    }

    public long getRightTokOffset() {
        return node.getRightTokOffset();
    }

    float getDistanceOf(Referent r) {
        if (this.corefLinks != null && !this.corefLinks.isEmpty()) {
            return this.getLastOccurrence().getDistanceOf(r);
        } else {
            float d = Math.abs(r.getSentenceId() - this.getSentenceId());
            d = d + 1;
            if (d == 1) { // same sentence
                //d += 1 - ( 500 / ( (float) Math.pow(this.getAbsoluteTokenDistanceFrom(r),4) + 500 ) ) ;
                d += 1 - (100 / (float) (Math.pow(this.getAbsoluteTokenDistanceFrom(r), 2) + 100));
            //if (d>=2) d = 1.99999f;
            } else if (d == 2) { // previous sentence
                d += 2 / ((float) this.getLeftTokOffset() + 2);
            } else if (d > 2) {
                d = (float) Math.pow(3, d) + 10;
            }
            return d;
        }
    }

    public Referent getLastOccurrence() {
        if (corefLinks != null && !this.corefLinks.isEmpty()) {
            return this.corefLinks.lastElement().anaphor;
        } else {
            return this;
        }
    }

    private long getAbsoluteTokenDistanceFrom(Referent r2) {
        return Math.abs(r2.getLeftTokOffset() - this.getLeftTokOffset());
    }

    public boolean isAlreadyHandled() {
        return alreadyHandled;
    }

    public void setAlreadyHandled(boolean b) {
        alreadyHandled = b;
    }

    public void setRefType(CorefResolver.RefType t) {
        this.syntacticType = t;
    }

    public CorefResolver.RefType getRefType() {
        return this.syntacticType;
    }

    @Override
    public int compareTo(Referent r) {
        if (this.getSentenceId() < r.getSentenceId()) {
            return -1;
        } else if (this.getSentenceId() == r.getSentenceId()) {
            return this.getNode().compareTo(r.getNode());
        } else {
            return 1;
        }
    }

    public String toLightValueString() {
        return this.getTokId() + ":" + this.getSurface();
    }

    void addCoreferenceLink(CoreferenceLink cl) {

        float d = this.getDistanceOf(cl.anaphor);
        if (d < 2) {
            d = 1; // if same sentence then should not decrease salience of main ref
        }
        this.salience = this.salience / d + cl.anaphor.getLocalSalience();
        cl.anaphor.resultingRefSalience = this.salience;

        if (this.corefLinks == null) {
            this.corefLinks = new Vector<CoreferenceLink>(3);
        }
        this.corefLinks.add(cl);

        this.addFeatures(cl.anaphor.getFeatures());
        this.addDependencies(cl.anaphor.getDependencies());

        if (this.getSemanticType() == SemanticType.PERSON) {
            this.personProps.updateWith(cl.anaphor);
        }
    }

    void setAntecedentLink(CoreferenceLink cl) {
        if (this.antecedentLinks == null) {
            this.antecedentLinks = new Vector<CoreferenceLink>(1);
        } else {
            this.antecedentLinks.clear();
        }
        this.antecedentLinks.add(cl);
    }

    void setAntecedentLinkCandidates(Vector<CoreferenceLink> clcs) {
        this.antecedentLinks = clcs;
    }

    private Vector<XipFeature> getFeatures() {
        return this.features;
    }

    private Vector<XipDependency> getDependencies() {
        return this.getNode().getDependencies();
    }

    String getFirstname() {
        if (this.personProps != null) {
            return this.personProps.getFirstname();
        } else {
            return null;
        }
    }

    String getFirstnameInitial() {
        if (personProps != null) {
            return this.personProps.getFirstnameInitial();
        } else {
            return null;
        }
    }

    String getMiddlename() {
        if (this.personProps != null) {
            return this.personProps.getMiddlename();
        } else {
            return null;
        }
    }

    String getMiddlenameInitial() {
        if (this.personProps != null) {
            return this.personProps.getMiddlenameInitial();
        } else {
            return null;
        }
    }

    String getLastname() {
        if (this.personProps != null) {
            return this.personProps.getLastname();
        } else {
            return null;
        }
    }

    String getLastnameInitial() {
        if (this.personProps != null) {
            return this.personProps.getLastnameInitial();
        } else {
            return null;
        }
    }

    String getPTitle() {
        if (this.personProps != null) {
            return this.personProps.getPTitle();
        } else {
            return null;
        }
    }

    boolean isSayerOf(Referent r) {
        if (r.getSaidBy() != null && (r.getSaidBy() == this || r.getSaidBy().getAntecedent() == this)) {
            return true;
        } else {
            return false;
        }
    }

    boolean isArg1(String relname, Referent r2) {
        return this.getNode().isRelHeadOf(r2.getNode(), relname);
    }

    XipNode getArg1(String relname) {
        return this.getNode().getHeadIn(relname);
    }

    XipNode getArg2(String relname, String featstr) {
        return this.getNode().getDependentIn(relname, featstr);
    }

    boolean isArg2(XipNode n) {
        return this.getNode().isRelDependentOf(n);
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

    @Override
    public String toString() {
        String s = this.getId() + " " + this.getTokId() + ":" + this.getSurface();
        if (this.hasAntecedents()) {
            s = s + "\n   Antecedent candidates are:";
            for (int i = 0; i < this.getAntecedentLinkCandidates().size(); i++) {
                CoreferenceLink cl = this.getAntecedentLinkCandidates().elementAt(i);
                if (cl.score < 0.001f) {
                    break;
                }
                s = s + "\n       (" + cl.getScoreString() + ") " +
                        cl.antecedent.getSurface() + " " + cl.antecedent.getTokId();
                s += " (last:" + cl.previousOccurrence.getTokId() + " DIST:" +
                        cl.previousOccurrence.getDistanceOf(this) + ")";
            //s += " (S=" + cl.antecedent.getSalience() + ",  D=" +
            //       cl.lastAntecedent.getDistanceOf(this) +  ")" ;
            }
            s = s + "\n";
        } else if (this.hasAnaphors()) {
            s = s + "\n   Coreferents are:";
            for (int i = 0; i < this.getCoreferenceLinks().size(); i++) {
                CoreferenceLink cl = this.getCoreferenceLinks().elementAt(i);
                s = s + "\n       (" + cl.getScoreString() + ") " +
                        cl.anaphor.getSurface() + " " + cl.anaphor.getTokId();
            }
        }
        return s;
    }

    public boolean hasAntecedents() {
        return (this.antecedentLinks != null && !this.antecedentLinks.isEmpty());
    }

    public boolean hasAnaphors() {
        return (this.corefLinks != null && !this.corefLinks.isEmpty());
    }

    public Vector<CoreferenceLink> getAntecedentLinkCandidates() {
        return this.antecedentLinks;
    }

    public Referent getAntecedent() {
        if (this.hasAntecedents()) {
            return this.antecedentLinks.elementAt(0).antecedent;
        } else {
            return null;
        }
    }

    public Vector<CoreferenceLink> getCoreferenceLinks() {
        return this.corefLinks;
    }

    public long getStart() {
        return node.getLeft();
    }

    public long getEnd() {
        return node.getRight();
    }

    public String getId() {
        return this.id + "";
    }

    public String getNodeId() {
        return this.getSentenceId() + ":" + this.getNode().getId();
    }

    public String getCategory() {
        return this.getNode().getCategory();
    }

    public String toNodeParseString() {
        String s = this.getTokId() + ":" + this.getCategory() + ": " + this.getSurface();
        s = s + "\n   " + this.getFeatures().toString();
        s = s + "\n   " + this.getDependencies().toString();
        return s;
    }

    void setId(int i) {
        this.id = i;
    }

    void setSaidBy(Referent sayer) {
        this.saidBy = sayer;
    }

    Referent getSaidBy() {
        return this.saidBy;
    }

    /**
     * Gets the string value of the property p of this referent
     * Returns the string value of the property p of this referent if it exists
     * null otherwise
     */
    public String getStringProperty(Referent.Properties p) {
        if (this.getSemanticType() != SemanticType.PERSON) {
            return null;
        }
        switch (p) {
            case hasFirstname:
                return this.personProps.firstname;
            case hasLastname:
                return this.personProps.lastname;
            case hasMiddlename:
                return this.personProps.middlename;
            case hasPTitle:
                return this.personProps.pTitle;
            case hasGender:
                if ( this.fMatches("MASC") && !this.fMatches("FEM") ) {
                    return "M";
                } else if (this.fMatches("FEM") && !this.fMatches("MASC")) {
                    return "F";
                } else {
                    return null;
                }
            default:
                return null;
        }
    }

}
