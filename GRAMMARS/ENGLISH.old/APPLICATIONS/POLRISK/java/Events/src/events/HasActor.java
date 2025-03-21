/*
 * Has_Actor.java
 * 
 * Created on Jul 20, 2007, 2:59:30 PM
 * 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package events;

/**
 *
 * @author arebotie
 */
public class HasActor extends Relation {

    public HasActor(Event event, Entity actor) {
        super(event, actor);
    }
    public Entity getActor()
    {
   return(Entity) super.getEntity2(); 
    }
    
public String toString() {
return super.toString();
}

}
