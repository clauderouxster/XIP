/*
 * EventCore.java
 *
 * Created on July 26, 2007, 5:11 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.events;

import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import documentAnalysis.Element;

/**
 * This class represent the core of an event. This core is linked to a XipNode, not the event
 * @author xtannier
 */
public class EventCore extends Element{
    
    /**
     * Creates a new instance of EventCore 
     */
    public EventCore() {
        super();
    }
    
    /** 
     * Creates a new instance of EventCore from a XipUnit and a XipNode
     *
     * @param unit the XipUnit
     * @param node the XipNode
     */
    public EventCore(XipUnit unit, XipNode node) {
        super(unit, node);
    }
    
    
    public String toString() {
        if (this.getXipNode() != null)
            return this.getXipNode().getSurface();
        else
            return "SOME_CORE";
    }
}
