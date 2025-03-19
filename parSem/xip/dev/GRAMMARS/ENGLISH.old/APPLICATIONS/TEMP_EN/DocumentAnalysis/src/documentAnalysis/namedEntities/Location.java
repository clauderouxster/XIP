/*
 * Location.java
 *
 * Created on June 1, 2007, 3:49 PM
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
public class Location extends Entity {
    
    private String name;
    
    /** Creates a new instance of Organisation */
    public Location(String name) {
        super();
        this.name = name;
    }
    
    
    /** Creates a new instance of Location
     *
     * @param unit the XipUnit associated to the Location.
     * @param node the XipNode associated to the Location.
     */
    public Location(XipUnit unit, XipNode node) {
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
        return "LOCATION: " + this.toString();
    }
    
    
}