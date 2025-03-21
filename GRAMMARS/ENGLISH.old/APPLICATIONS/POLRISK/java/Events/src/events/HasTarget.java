/*
 * HasTarget.java
 * 
 * Created on Jul 23, 2007, 4:15:23 PM
 * 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package events;

/**
 *
 * @author arebotie
 */
public class HasTarget extends Relation{
    
    public HasTarget(Event event, Entity target) {
        super(event, target);
    }
    public Entity getTarget()
    {
   return(Entity) super.getEntity2(); 
    }
    
public String toString() {
return super.toString();
}

}
