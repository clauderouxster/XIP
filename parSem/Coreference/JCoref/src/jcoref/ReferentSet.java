/*
 * ReferentSet.java
 *
 * Created on August 26, 2006, 7:48 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import com.xerox.jxip.XipNode;
import java.util.Collections;
import java.util.Vector;

/**
 *
 * @author ait
 */
public class ReferentSet extends Vector<Referent> {
    
    
    private static final float W_DISTANCE=1;
    private static final float W_REFERENT_SALIENCE=5;
    
    private float totalSalienceScore = 0;
    
    /**
     * Creates a new instance of ReferentSet
     */
    public ReferentSet() {
        super();
    }
    
    /** Adds a referent to this referent set.
     * @param node the node from which to create the new referent to be added
     */
    void addNewReferent(final Referent ref) {
        Tracer.traceAddNewReferent(ref);
        this.totalSalienceScore = this.totalSalienceScore + ref.getSalience();
        // insert the new referent in the referents list at the right position
        int i = this.size()-1;
        while (i>=0 && ref.getSalience() < this.elementAt(i).getSalience()) {
            i--;
        }
        this.insertElementAt(ref,i+1);
        ref.setId(this.size());
    }
    
    
    float getRelativeSalienceOf(Referent r, Referent r2) {
        
        if (this.W_DISTANCE==0)
            return getTextSalienceOf(r);
        else
            return (this.W_REFERENT_SALIENCE*getTextSalienceOf(r) ) /
                    (this.W_DISTANCE*r.getDistanceOf(r2) ) ;
    }
    
    float getTextSalienceOf(Referent r) {
        return r.getSalience()/this.getTotalSalience() ;
    }
    
    
    /** Accessor: gets the total salience score of this set of referents */
    private float getTotalSalience() {
        return this.totalSalienceScore;
    }
    
    
    
    void print() {
        Referent r=null;
        Collections.sort(this);
        System.out.println("Referents and Coreference links:\n-------------------------------");
        for(int i=0; i<this.size(); i++) {
            r = this.elementAt(i);
            if (!r.hasAntecedents())
                System.out.println(r.toString());
        }
    }
    
    /** Returns the score of the semantic matching between referent r and node n */
    float getSemanticMatchingScore(Referent r, Referent r2) {
        float score = 0;
        if ( r.hasProperty( r2.getChunkHead().getLemma() ) ) {
            score = 1;
        }
        return score;
    }
    
    /** Sets a unique and final coreference link
     */
    void setCoreferenceLinkForAntecedent(CoreferenceLink cl) {
        float old_salience = cl.antecedent.getSalience();
        cl.antecedent.addCoreferenceLink(cl);
        this.totalSalienceScore = this.totalSalienceScore - old_salience + cl.antecedent.getSalience() ;
    }
    
    
    void setCoreferenceLink(CoreferenceLink cl) {
        this.setCoreferenceLinkForAntecedent(cl);
        cl.anaphor.setAntecedentLink(cl);
    }
    
    
    void setCoreferenceLinkCandidates(Vector<CoreferenceLink> clcs) {
        this.setCoreferenceLink(clcs.elementAt(0));
        clcs.elementAt(0).anaphor.setAntecedentLinkCandidates(clcs);
    }
    
    
    void init() {
        this.clear();
        this.totalSalienceScore=0;
    }
    
}


