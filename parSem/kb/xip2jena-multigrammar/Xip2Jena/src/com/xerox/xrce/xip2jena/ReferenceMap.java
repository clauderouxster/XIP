package com.xerox.xrce.xip2jena;

import com.xerox.jxip.XipNode;
import java.util.HashMap;
import java.util.Set;
import jcoref.Referent;
import jcoref.ReferentList;

/**
 *
 * @author akaplan
 */
class ReferenceMap {
    
    private HashMap<XipNode,XipNode> map;
    
    /*
     * makes an empty reference map
     */
    ReferenceMap() {
        map = new HashMap<XipNode,XipNode>();
    }
     
    ReferenceMap(ReferentList referents) {
        map = new HashMap<XipNode,XipNode>(referents.size());
        for (Referent referent : referents) {
            Referent antecedent = referent.getAntecedent();
            if (antecedent == null) {
                antecedent = referent;
            }
            map.put(referent.getNode(), antecedent.getNode());
        }
    }
    
    XipNode get(XipNode n) {
        XipNode result = map.get(n);
        if (result == null) {
            result = n;
        }
        return result;
    }
    
    Set<XipNode> getAll() {
        return map.keySet();
    }
}
