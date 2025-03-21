/*
 * TemporalConsistencyException.java
 *
 * Created on December 14, 2006, 10:10 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality;

import documentAnalysis.temporality.vaguecalendar.date.VagueDate;

/**
 * Exception raised when two temporal relations are not compatible
 * eg: BEFORE(A, B) and BEFORE(B, A)
 * @author xtannier
 */
public class TemporalConsistencyException extends java.lang.Exception {
    
    private static final long serialVersionUID = 1L;

    /**
     * Creates a new instance of <code>TemporalConsistencyException</code> without detail message.
     */
    public TemporalConsistencyException() {
    }
    
    
    /**
     * Constructs an instance of <code>TemporalConsistencyException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public TemporalConsistencyException(String msg) {
        super(msg);
    }

    
    /**
     * Constructs an instance of <code>TemporalConsistencyException</code> with the specified detail message.
     *
     * @param date1 the first date involved in the exception
     * @param date2 the first date involved in the exception
     */
    public TemporalConsistencyException(VagueDate date1, VagueDate date2) {
        this("Temporal consistency problem between " + date1.toString() + " and " +
                date2.toString());
    }    
}
