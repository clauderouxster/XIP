/*
 * OrgProperties.java
 *
 * Created on June 18, 2007, 9:38 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import com.xerox.jxip.XipNode;


/**
 *
 * @author ait
 */
public class OrgProperties {
    
    String form;
    String abbreviation;
    
    /** Creates a new instance of OrgProperties */
    public OrgProperties(XipNode node) {
        form = node.getSurface();
        updateWith(node);
    }
    
    
    String getAbbr() {
        return this.abbreviation;
    }
    
    private void updateWith(XipNode node) {
        if (abbreviation == null) {
            abbreviation = this.getAbbr(node);
        }
    }
    
    private String getAbbr(XipNode n) {
        StringBuffer s = new StringBuffer();
        if (n.isLeaf() || n.isFicticious())
            return "";
        else if (n.isPreTerminalNode()) {
            if (n.getCategory().contentEquals("DET") ||
                    n.getCategory().contentEquals("PREP") ) {
                return "";
            } else if (n.fMatches("ACRON") || n.fMatches("ABR") ) {
                return n.getSurface();
            } else {
                String form = n.getSurface();
                boolean firstchar = true;
                for(int i=0; i<form.length(); i++) {
                    if (firstchar && Character.isUpperCase(form.charAt(i))) {
                        s.append(form.charAt(i));
                        firstchar = false;
                    } else if (form.charAt(i)==' ' || form.charAt(i)=='-') {
                        firstchar = true;
                    }
                }
            }
        } else {
            for(int i=0; i < n.getDaughters().size(); i++) {
                String abbr = this.getAbbr( n.getDaughters().elementAt(i) );
                s.append(abbr);
            }
        }
        return s.toString();
    }
    
}
