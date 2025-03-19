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
public class CoreferenceLink implements Comparable {
    
    public ReferentNode antecedent;
    public ReferentNode anaphor;
    public float score;
    public String rule;
    
    static private NumberFormat numberFormat = NumberFormat.getNumberInstance();

    ReferentNode lastAntecedent = null;
    float referentSalience = 0;
    
    
    /** Creates a new instance of CoreferenceLink */
    public CoreferenceLink(Referent antec, ReferentNode anaphor, float score, String rule) {
        this.antecedent = antec.getFirstNode();
        this.anaphor = anaphor;
        this.score = score;
        this.rule = rule;
        numberFormat.setMaximumFractionDigits(5);
        numberFormat.setMinimumFractionDigits(5);
        this.lastAntecedent = antec.getLastNode();
        this.referentSalience = antec.getSalience();
    }
    
    public int compareTo(Object o) {
        if (this.score > ((CoreferenceLink) o).score)
            return 1;
        else if (this.score == ((CoreferenceLink) o).score )
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
    
    public ReferentNode getAntecedent() {
        return antecedent;
    }
    
    public ReferentNode getAnaphor() {
        return this.anaphor;
    }
    
    String getScoreString() {
        return numberFormat.format(this.score);
    }
    
}
