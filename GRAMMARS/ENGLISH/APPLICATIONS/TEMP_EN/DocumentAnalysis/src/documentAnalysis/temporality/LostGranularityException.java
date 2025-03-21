/*
 * LostGranularityException.java
 *
 * Created on January 3, 2007, 2:23 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality;

/**
 * Exception thrown when an Vague Calendar operation caused the 
 * loss of an information; Typically, when we try to add some days
 * but we don't know the month (and then we don't know when we change month)
 * @author xtannier
 */
public class LostGranularityException extends java.lang.Exception {
    
    private static final long serialVersionUID = 1L;    
    private int remaining = 0;
    
    /**
     * Creates a new instance of <code>LostGranularityException</code> without detail message.
     */
    public LostGranularityException() {
    }
    
    
    /**
     * Constructs an instance of <code>LostGranularityException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public LostGranularityException(String msg) {
        super(msg);
    }
    
    
    /**
     * Constructs an instance of <code>LostGranularityException</code> with the 
     * specified amount remaining to add when the exception occurred.
     * @param remainingAmount - the amount remaining to add when the exception occurred.
     */
    public LostGranularityException(int remainingAmount) {
        this.remaining = remainingAmount;
    }    

    public int getRemaining() {
        return remaining;
    }
}
