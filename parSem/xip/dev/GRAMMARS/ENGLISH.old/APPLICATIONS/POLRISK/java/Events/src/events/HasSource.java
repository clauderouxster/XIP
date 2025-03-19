/*
 * HasSource.java
 * 
 * Created on Jul 23, 2007, 4:36:44 PM
 * 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package events;

/**
 *
 * @author arebotie
 */
public class HasSource extends Relation {
    private String factuality;

    public HasSource(Event event, Entity source, String factuality) {
        super(event, source);
        this.factuality = factuality;
    }
    public Entity getSource()
    {
   return(Entity) super.getEntity2(); 
    }
    
    public String getSourcewithFactuality() {
    return super.getEntity2().toString() + " : " + this.factuality;
     }

    public String getFactuality(){
    return this.factuality;
    }
    
}
