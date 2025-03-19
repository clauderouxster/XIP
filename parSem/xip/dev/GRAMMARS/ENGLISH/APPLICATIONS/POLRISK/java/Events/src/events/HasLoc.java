/*
 * HasLoc.java
 * 
 * Created on Jul 23, 2007, 4:10:55 PM
 * 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package events;

/**
 *
 * @author arebotie
 */
public class HasLoc extends Relation{
    
    public HasLoc(Event event, Entity loc) {
        super(event, loc);
    }
    public Entity getLoc()
    {
   return(Entity) super.getEntity2(); 
    }
    
public String toString() {
return super.toString();
}

    
}
