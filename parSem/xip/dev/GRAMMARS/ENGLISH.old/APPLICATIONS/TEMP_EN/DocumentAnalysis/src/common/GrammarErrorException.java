/*
 * GrammarErrorException.java
 *
 * Created on February 2, 2007, 4:55 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common;

/**
 * Exception raised when an operation fails because the result of XIP parsing
 * is not what was expected.
 * @author xtannier
 */
public class GrammarErrorException extends RuntimeException {
    
    private static final long serialVersionUID = 1L;
    
    private String msg;
    
    /**
     * Creates a new instance of <code>GrammarErrorException</code> without detail message.
     */
    public GrammarErrorException() {
        super();
        this.msg = "Exception: Grammar Error";
    }
    
    
    /**
     * Constructs an instance of <code>GrammarErrorException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public GrammarErrorException(String msg) {
        super(msg);
        this.msg = msg;
    }
    
    /**
     * Returns a String representation.
     * @return a String message.
     */
    public String toString() {
        return this.msg;
    }    
}
