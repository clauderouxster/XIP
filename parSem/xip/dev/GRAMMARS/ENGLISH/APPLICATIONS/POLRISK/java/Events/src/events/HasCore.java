/*
 * hasCore.java
 * 
 * Created on Jul 20, 2007, 2:34:14 PM
 * 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package events;

/**
 *
 * @author arebotie
 */
public class HasCore extends Relation {

    public HasCore(Event event, Core core) {
        super(event, core);
    }
    
    public Core getCore()
    {
   return(Core) super.getEntity2(); 
    }
    
public String toString() {
return super.toString();
}
}

