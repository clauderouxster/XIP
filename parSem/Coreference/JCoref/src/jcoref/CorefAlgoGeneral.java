/*
 * GeneralResolver.java
 *
 * Created on May 30, 2007, 3:57 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

/**
 *
 * @author ait
 */
public abstract class CorefAlgoGeneral {
    static protected final float W_RELATIVE_SALIENCE=2;
    static protected final float W_SEMANTIC_MATCHING=1;
    
    public String rule;
    public String failRule;
    
    /** Creates a new instance of GeneralResolver */
    public CorefAlgoGeneral() {
    }
    
    
}

