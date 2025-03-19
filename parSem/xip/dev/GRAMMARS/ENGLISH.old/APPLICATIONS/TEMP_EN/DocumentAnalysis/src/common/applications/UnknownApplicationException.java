/*
 * UnknownApplicationException.java
 *
 * Created on February 23, 2007, 11:20 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.applications;

/**
 * Exception raised when the specified application is unknown.
 * @author xtannier
 */
public class UnknownApplicationException extends java.lang.Exception {
    
    private static final long serialVersionUID = 1L;
    
    
    /** Creates a new instance of UnknownApplicationException */
    public UnknownApplicationException() {
    }
    
    
    /**
     * Constructs an instance of <code>UnknownApplicationException</code> 
     * with the specified detail message.
     *
     * @param application the application name.
     */
    public UnknownApplicationException(String application) {
        super("Exception: Application " + application + " is unknown.");
    }    
}
