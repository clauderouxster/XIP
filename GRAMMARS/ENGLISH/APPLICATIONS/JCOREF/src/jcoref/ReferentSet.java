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
    private static final float W_REFERENT_SALIENCE=1;
    
    private float totalSalienceScore = 0;
    
    /**
     * Creates a new instance of ReferentSet
     */
    public ReferentSet() {
        super();
    }
    
    /** Adds a referent to this referent set.
     * @param node the node from which to create the new referent to be added*/
    void addNewReferent(final ReferentNode refnode) {
        Referent ref = new Referent(refnode);
        this.totalSalienceScore = this.totalSalienceScore + ref.getSalience();
        // insert the new referent in the referents list at the right position
        int i = this.size()-1;
        while (i>=0 && ref.getSalience() < this.elementAt(i).getSalience()) {
            i--;
        }
        this.insertElementAt(ref,i+1);
    }
    
    
    float getRelativeSalienceOf(Referent r, ReferentNode node) {
        
        if (this.W_DISTANCE==0)
            return getTextSalienceOf(r);
        else
            return (this.W_REFERENT_SALIENCE*getTextSalienceOf(r) ) /
                    (this.W_DISTANCE*r.getDistanceOf(node) ) ;
    }
    
    float getTextSalienceOf(Referent r) {
        return r.getSalience()/this.getTotalSalience() ;
    }
    
    /** Returns the relative salience of referent with index i */
    float getTextSalienceOf(int i) {
        return this.getTextSalienceOf(this.elementAt(i));
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
            System.out.println(r.toString());
        }
    }
    
    /** Returns the score of the semantic matching between referent r and node n */
    float getSemanticMatchingScore(Referent r, ReferentNode n) {
        float score = 0;
        if ( r.hasProperty( n.getLemma() ) ) {
            score = 1;
        }
        return score;
    }
    

    
    void addNewAnaphor(ReferentNode refnode, Referent r, float score, String rule) {
        float old_salience = r.getSalience();
        r.addReferentNode(refnode);
        this.totalSalienceScore = this.totalSalienceScore - old_salience + r.getSalience() ;
    }
    
    void init() {
        this.clear();
        this.totalSalienceScore=0;
        Referent.init();
    }
    
}


