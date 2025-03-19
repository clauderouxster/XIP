/*
 * IntervalException.java
 *
 * Created on April 12, 2007, 5:02 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.date;

/**
 * This exception is raised when a interval is created with begin point after end point
 *
 * @author xtannier
 */
public class IntervalException extends java.lang.Exception {
    
    private static final long serialVersionUID = 1L;    
    
    /**
     * Creates a new instance of <code>IntervalException</code> without detail message.
     */
    public IntervalException() {
        super("Interval Exception: Begin point must be after end point!");
    }
    
    
    /**
     * Constructs an instance of <code>IntervalException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public IntervalException(String msg) {
        super("Interval Exception: " + msg);
    }
    
   
    /**
     * Constructs an instance of <code>IntervalException</code> with the specified 
     *  dates.
     * @param date1 the first date in the interval
     * @param date2 the second date in the interval
     */    
    public IntervalException(VagueSimpleDate date1, VagueSimpleDate date2) {
        super("Interval Exception: Begin point must be after end point!\n" +
                "Dates: " + date1.toString() + " and " + date2.toString());
    }
}
