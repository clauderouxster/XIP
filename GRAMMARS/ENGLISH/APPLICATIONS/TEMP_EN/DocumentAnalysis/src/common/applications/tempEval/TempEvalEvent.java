/*
 * TempEvalEvent.java
 *
 * Created on July 6, 2007, 8:19 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.applications.tempEval;

import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import documentAnalysis.events.Event;


/**
 * Class extending Event with the specific TempEval id.
 * @author xtannier
 */
public class TempEvalEvent extends Event{
    
    private String eid;
    
    public TempEvalEvent(XipUnit unit, XipNode node, String eid){
        super(unit, node);
        if (eid == null)
            throw new NullPointerException();
        this.eid = eid;
    }
    
    /**
     * Returns the id
     * @return the id
     */
    public String getId(){
        return eid;
    }
    
    /**
     * to String
     */
    public String toString(){
        return this.eid + ": " + super.toString();
    }
}
