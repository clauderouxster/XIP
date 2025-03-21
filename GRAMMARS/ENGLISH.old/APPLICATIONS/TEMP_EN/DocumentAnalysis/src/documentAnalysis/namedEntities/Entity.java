/*
 * Entity.java
 *
 * Created on June 1, 2007, 3:25 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.namedEntities;

import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import documentAnalysis.Element;

/**
 *
 * @author xtannier
 */
public abstract class Entity extends Element {
    
    
    /**
     * Builds an entity.
     */
    public Entity(){
        super();
    }    
    
    /**
     * Builds an entitywith the specified XipNode.
     * @param unit the XipUnit associated to the entity.
     * @param node the XipNode associated to the entity.
     */
    public Entity(XipUnit unit, XipNode node){
        super(unit, node);
    }
    
    
    /**
     * Get a detailed String representation of the entity
     *
     * @return a detailed String representation of the entity
     */
    public abstract String toDetailedString();
}
