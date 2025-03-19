/*
 * BadFormatException.java
 *
 * Created on November 29, 2006, 10:37 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar;

/**
 * This exception is raised when a String format of date or duration is not correct
 * in the context of the method that throws it.
 * @author xtannier
 */
public class BadFormatException extends java.lang.Exception {
    
    private static final long serialVersionUID = 1L;
    
    
    /**
     * Creates a new instance of <code>BadFormatException</code> without detail message.
     */
    public BadFormatException() {
        super();
    }
    
    
    /**
     * Constructs an instance of <code>BadFormatException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public BadFormatException(String msg) {
        super("Bad Format Exception: \"" + msg + "\"");
    }
    
    
    /**
     * Returns a String representation of the exception 
     * @return a String representation of the exception 
     */
    public String toString() {
	return "Bad Format Exception: \"" + this.getMessage() + "\"";
    }   
}
