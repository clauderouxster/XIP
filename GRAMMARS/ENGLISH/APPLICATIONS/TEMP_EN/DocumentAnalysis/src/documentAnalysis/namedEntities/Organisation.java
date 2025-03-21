/*
 * Organisation.java
 *
 * Created on June 1, 2007, 3:47 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.namedEntities;

import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;

/**
 *
 * @author xtannier
 */
public class Organisation extends Entity {
    
    private String name;
    
    /** Creates a new instance of Organisation */
    public Organisation(String name) {
        super();
        this.name = name;
    }
    
    
    /** Creates a new instance of Organisation
     *
     * @param unit the XipUnit associated to the organisation.
     * @param node the XipNode associated to the organisation.
     */
    public Organisation(XipUnit unit, XipNode node) {
        super(unit, node);
        this.name = node.getSurface();
    }
    
    
    /**
     * Returns a String representation of the current element.
     * @return a String representation of the current element.
     */
    public String toString(){
        return this.name;
    }
    
    /**
     * Get a detailed String representation of the entity
     *
     * @return a detailed String representation of the entity
     */
    public String toDetailedString(){
        return "ORGANISATION: " + this.toString();
    }    
}