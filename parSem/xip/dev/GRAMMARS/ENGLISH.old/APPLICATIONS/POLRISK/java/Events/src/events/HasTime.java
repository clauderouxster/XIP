/*
 * HasTime.java
 * 
 * Created on Jul 23, 2007, 4:08:37 PM
 * 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package events;

/**
 *
 * @author arebotie
 */
public class HasTime extends Relation{
    
    public HasTime(Event event, Entity time) {
        super(event, time);
    }
    public Entity getTime()
    {
   return(Entity) super.getEntity2(); 
    }
    
public String toString() {
return super.toString();
}

    

}
