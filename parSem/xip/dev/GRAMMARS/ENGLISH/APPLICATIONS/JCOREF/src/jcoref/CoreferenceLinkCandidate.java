/*
 * CoreferenceLinkCandidate.java
 *
 * Created on August 28, 2006, 5:20 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

/**
 *
 * @author ait
 */
public class CoreferenceLinkCandidate  implements Comparable {
    
    Referent referent;
    float score;
    String rule;
    
    /** Creates a new instance of CoreferenceLinkCandidate */
    public CoreferenceLinkCandidate(Referent r, float score, String rule) {
        this.referent = r;
        this.score = score;
        this.rule = rule;
    }

    public int compareTo(Object o) {
        if (this.score > ((CoreferenceLinkCandidate) o).score)
            return -1;
        else if (this.score == ((CoreferenceLinkCandidate) o).score )
            return 0;
        else
            return 1;
    }
    
    public String toString() {
        return "(" + score + ") " + referent.getFirstNode().getSurface();
    }
    
}
