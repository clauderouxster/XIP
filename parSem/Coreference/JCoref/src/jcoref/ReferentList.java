/*
 * ReferentNodeList.java
 *
 * Created on September 12, 2006, 10:45 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import com.xerox.jxip.XipNode;
import java.util.Vector;

/**
 *
 * @author ait
 */
public class ReferentList extends Vector<Referent> {
    
    /** Creates a new instance of ReferentNodeList */
    public ReferentList() {
        super();
    }
    
    /** Creates a new instance of ReferentNodeList */
    public ReferentList(int n) {
        super(n);
    }
    
    public boolean containsXipNode(XipNode n) {
        for(int i=0; i < this.size(); i++) {
            if (this.elementAt(i).getNode()==n)
                return true;
        }
        return false;
    }

    
}
