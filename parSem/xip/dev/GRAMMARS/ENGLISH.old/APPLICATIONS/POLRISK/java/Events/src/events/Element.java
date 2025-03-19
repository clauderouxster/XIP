/*
 * Element.java
 * 
 * Created on Jul 20, 2007, 1:44:36 PM
 * 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package events;

import com.xerox.jxip.XipNode;

/**
 *
 * @author arebotie
 */
public abstract class Element extends Object {
String surface;
XipNode xipnode;
    
    public Element(XipNode xipnode) {
        this.xipnode=xipnode;
        this.surface=xipnode.getSurface();
    }

     public String toString() {
    return surface;
     }
}
