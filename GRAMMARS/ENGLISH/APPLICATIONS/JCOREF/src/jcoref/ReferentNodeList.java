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
public class ReferentNodeList extends Vector<ReferentNode> {
    
    /** Creates a new instance of ReferentNodeList */
    public ReferentNodeList() {
        super();
    }
    
    /** Creates a new instance of ReferentNodeList */
    public ReferentNodeList(int n) {
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
