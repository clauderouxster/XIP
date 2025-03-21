/*
 * TemporalUnit.java
 *
 * Created on December 11, 2006, 2:04 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar;

/**
 * Interface for any temporal unit (Date or Duration).
 * @author xtannier
 */
public interface TemporalUnit extends Comparable<TemporalUnit>, java.lang.Cloneable{
    
    /**
     * Returns a String formated representation of the Unit.
     * @return a String formated representation of the Unit.
     */
    public String getValue();
    
    /**
     * Tests if the Unit is defined.
     * @return true whether the Unit is defined, false otherwise.
     */
    public boolean isDefined();
    
    /**
     * Gets a clone
     * @return a clone.
     */
    public abstract Object clone() throws CloneNotSupportedException;
    
    /**
     * Compares this object with the specified object for order.
     * Use {@link documentAnalysis.temporality.TemporalRelation} temporal constants 
     * instead of negative/positive values.
     *
     * 
     */
    public abstract int compareTo(TemporalUnit o);        
}
