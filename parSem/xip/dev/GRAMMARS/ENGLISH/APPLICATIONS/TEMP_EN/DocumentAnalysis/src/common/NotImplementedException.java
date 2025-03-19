/*
 * NotImplementedException.java
 *
 * Created on November 29, 2006, 10:41 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common;

/**
 * Exception raised when a feature is required but not implemented.
 * @author xtannier
 */
public class NotImplementedException extends RuntimeException {
    
    private static final long serialVersionUID = 1L;
    
    private String msg;
    
    
    //UnsupportedOperationException
    /**
     * Creates a new instance of <code>NotImplementedException</code> without detail message.
     */
    public NotImplementedException() {
        super();
        this.msg = "Exception: Not implemented!";
    }
    
    
    /**
     * Constructs an instance of <code>NotImplementedException</code> with the specified detail message.
     *
     * @param msg the detail message.
     */
    public NotImplementedException(String msg) {
        super("Exception: Not implemented! " + msg);
        this.msg = "Exception: Not implemented! " + msg;
    }
    
    
    /**
     * Returns a String representation.
     * @return a String message.
     */
    public String toString() {
        return this.msg;
    }
}
