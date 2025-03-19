/*
 * Element.java
 *
 * Created on July 25, 2007, 12:30 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis;

import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;

/**
 * This interface is used to link an element to a XipNode and a XipUnit
 * @author xtannier
 */
public interface XipLink {

    
    /**
     * Returns the XipUnit associated with the current element, null if no XipUnit.
     * @return the XipUnit associated with the current element, null if no XipUnit.
     */
    public XipUnit getXipUnit();
    
    /**
     * Returns the XipNode associated with the current element, null if no XipNode.
     * @return the XipNode associated with the current element, null if no XipNode.
     */
    public XipNode getXipNode();
    
    /**
     * Associates a XipNode to the current element.
     * @param unit the XipUnit in which the element is found.
     * @param node the XipNode to associate to the current element.
     */
    public void setXipNode(XipUnit unit, XipNode node);
    
    
   /**
     * Gets the number of the sentence where the element has been found.
     * @return the number of the sentence where the element has been found.
     */
    public long getSentenceNumber();
    
    
    /**
     * Returns the universal id of the current element (XipNode UID).
     * @return the uid the XipNode associated with the current element, null if no XipNode.
     */
    public String getUid();
    
    
    /**
     * Gets the category of the associated XipNode.
     * @return the category of the associated XipNode, null if no XipNode associated.
     */
    public String getCategory();
        
    
    /**
     * Test whether two elements are equal.
     * @param o the object to compare.
     * @return true whether the object is equal to the current element.
     */
    public boolean equals(Object o);
    
    
    /**
     * Returns a String representation of the current element.
     * @return a String representation of the current element.
     */
    public String toString();
    
    
}
