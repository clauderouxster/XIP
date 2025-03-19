/*
 * Element.java
 *
 * Created on June 1, 2007, 3:27 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis;

import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;

/**
 * This class contains the basics for any elements linked to a XipNode (Entities, Temporal expressions...)
 * @author xtannier
 */
public abstract class Element implements XipLink {
    
    private XipNode node;
    private XipUnit unit;
    
    /** Creates a new instance of Element */
    public Element() {
        this.unit = null;
        this.node = null;
    }
    
    /**
     * Builds a element with the specified XipNode.
     * @param unit the XipUnit associated to the element.
     * @param node the XipNode associated to the element.
     */
    public Element(XipUnit unit, XipNode node){
        this.unit = unit;
        this.node = node;
    }
    
    
    /**
     * Returns the XipUnit associated with the current element, null if no XipUnit.
     * @return the XipUnit associated with the current element, null if no XipUnit.
     */
    public XipUnit getXipUnit() {
        return unit;
    }
    
    /**
     * Returns the XipNode associated with the current element, null if no XipNode.
     * @return the XipNode associated with the current element, null if no XipNode.
     */
    public XipNode getXipNode() {
        return node;
    }
    
    /**
     * Associates a XipNode to the current element.
     * @param unit the XipUnit in which the element is found.
     * @param node the XipNode to associate to the current element.
     */
    public void setXipNode(XipUnit unit, XipNode node) {
        this.unit = unit;
        this.node = node;
    }
    
    
   /**
     * Gets the number of the sentence where the element has been found.
     * @return the number of the sentence where the element has been found.
     */
    public long getSentenceNumber() {
        if (this.unit != null)
            return this.unit.getSentenceNb();
        else
            return -1;
    }
    
    
    /**
     * Returns the universal id of the current element (XipNode UID).
     * @return the uid the XipNode associated with the current element, null if no XipNode.
     */
    public String getUid(){
        if (this.getXipNode() != null)
            return this.getXipNode().getUid();
        else
            return null;
    }    
    
    
    /**
     * Gets the category of the associated XipNode.
     * @return the category of the associated XipNode, null if no XipNode associated.
     */
    public String getCategory(){
        if (this.getXipNode() == null)
            return null;
        else
            return this.getXipNode().getCategory();
    }
        
    
    /**
     * Test whether two elements are equal.
     * @param o the object to compare.
     * @return true whether the object is equal to the current element.
     */
    public boolean equals(Object o){
        if (this.getClass().equals(o.getClass())){
            Element e = (Element)o;
            if (this.node != null && e.node != null)
                return this.getSentenceNumber() == e.getSentenceNumber() &&
                        this.node.equals(e.node);
            else if (this.node == null && e.node == null)
                return super.equals(o);
            else
                return false;
        } else
            return false;
    }    
    
    
    /**
     * Returns a String representation of the current element.
     * @return a String representation of the current element.
     */
    public abstract String toString();
        


    
}
