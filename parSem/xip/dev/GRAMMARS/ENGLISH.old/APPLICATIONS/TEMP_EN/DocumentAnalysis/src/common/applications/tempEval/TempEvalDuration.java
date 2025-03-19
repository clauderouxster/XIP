/*
 * TempEvalDuration.java
 *
 * Created on July 6, 2007, 8:20 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.applications.tempEval;

import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;

/**
 * Private class extending Duration with the specific TempEval id.
 * @author xtannier
 */
public class TempEvalDuration extends SimpleDuration{
    
    private String id;
    
    public TempEvalDuration(XipUnit unit, XipNode node, String id){
        super(unit, node);
        if (id == null)
            throw new NullPointerException();
        this.id = id;
    }
    
    /**
     * Returns the id
     * @return the id
     */
    public String getId(){
        return id;
    }
    
    /**
     * to String
     */
    public String toString(){
        return this.id + ": " + super.toString();
    }
}

