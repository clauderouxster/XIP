/*
 * CoreferenceLink.java
 *
 * Created on August 21, 2006, 4:24 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import com.xerox.jxip.XipNode;
import java.text.NumberFormat;
import java.util.Comparator;

/**
 *
 * @author ait
 */
public class CoreferenceLink implements Comparable<CoreferenceLink> {
    
    public Referent antecedent;
    public Referent anaphor;
    public Referent previousOccurrence;
    public float score;
    public String rule;
    
    static private NumberFormat numberFormat = NumberFormat.getNumberInstance();
    
    
    /** Creates a new instance of CoreferenceLink */
    public CoreferenceLink(Referent antec, Referent anaphor, float score, String rule) {
        this.antecedent = antec;
        this.anaphor = anaphor;
        this.previousOccurrence = antec.getLastOccurrence();
        this.score = score;
        this.rule = rule;
        numberFormat.setMaximumFractionDigits(5);
        numberFormat.setMinimumFractionDigits(5);
    }
    
    public int compareTo(CoreferenceLink other) {
        if (this.score < other.score)
            return 1;
        else if (this.score == other.score )
            return 0;
        else
            return -1;
    }
    
    public float getScore() {
        return this.score;
    }
    
    public String getRule() {
        return this.rule;
    }
    
    public Referent getAntecedent() {
        return antecedent;
    }
    
    public Referent getAnaphor() {
        return this.anaphor;
    }
    
    String getScoreString() {
        return numberFormat.format(this.score);
    }
    
}
