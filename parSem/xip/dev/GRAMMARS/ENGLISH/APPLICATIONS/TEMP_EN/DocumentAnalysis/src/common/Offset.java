/*
 * Offset.java
 *
 * Created on February 8, 2007, 1:47 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common;

/**
 * This class represents an offset position of a sub-string in a text.
 * @author xtannier
 */
public class Offset {
    
    private long left;
    private long right;
    
    /** Creates a new instance of Offset */
    public Offset(long left, long right) {
        this.left = left;
        this.right = right;
    }

    /**
     * Returns the left offset
     * @return the left offset
     */
    public long getLeft() {
        return this.left;
    }

    /**
     * Returns the right offset.
     * @return the right offset
     */
    public long getRight() {
        return this.right;
    }
    
    /**
     * Returns a String representation of the offset
     *
     * @return a String representation of the offset
     */
    public String toString() {
        return "[" + this.left + "-" + this.right + "]";
    }
    
}
