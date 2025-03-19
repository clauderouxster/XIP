/*
 * Duration.java
 *
 * Created on December 13, 2006, 11:53 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.duration;

import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;

/**
 * This class implements a Duration
 * @author xtannier
 */
public abstract class Duration extends TemporalConstituent implements TemporalUnit{
    
    
    
    /**
     * Builds a new instance of Duration.
     */
    public Duration(){
        super();
    }
    
    /**
     * Builds a new instance of Duration.
     * @param unit the XipUnit in which the duration is found
     * @param node the XipNode corresponding to the duration
     */
    public Duration(XipUnit unit, XipNode node){
        super(unit, node);
    }
    
    
    /**
     * Returns a new instance of Duration
     * @return a new null Duration.
     */
    public static Duration getNewDuration() {
        return new SimpleDuration();
    }
    
    
    /**
     * Returns a new instance of Duration
     * @return a new null Duration.
     */
    public static Duration getNewDuration(XipUnit unit, XipNode node) {
        Duration result = getNewDuration();
        result.setXipNode(unit, node);
        return result;
    }    
    
    /**
     * Returns a Duration based on the specified format
     * @param durationFormat the String representation of the duration
     * @throws documentanalysis.temporality.vaguecalendar.BadFormatException when the format is not correct
     * @return a new Duration based on the specified format
     */
    public static Duration getNewDuration(String durationFormat) throws BadFormatException{
        if (durationFormat.contains("#") || durationFormat.contains("|")){
            return new MultipleDuration(durationFormat);
        } else if (durationFormat.contains("/")){
            return new DurationInterval(durationFormat);
        } else {
            return new SimpleDuration(durationFormat);
        }
    }
    
    /**
     * Returns a Duration based on the specified DurationUnit
     * @param unit the DurationUnit 
     * @return a new Duration based on the specified DurationUnit
     */    
    public static Duration getNewDuration(DurationUnit unit) {
        return unit.getDuration();
    }
    
    
    /**
     * Returns a Duration based on the specified format
     * @param durationFormat the String representation of the duration
     * @throws documentanalysis.temporality.vaguecalendar.BadFormatException when the format is not correct
     * @return a new Duration based on the specified format
     */
    public static Duration getNewDuration(String durationFormat, XipUnit unit, XipNode node) throws BadFormatException{
        Duration result = getNewDuration(durationFormat);
        result.setXipNode(unit, node);
        return result;
    }
    
    
//    /**
//     * Sets the current Duration as a copy of specified Duration
//     *
//     * @param duration the Duration to copy
//     */
//    public abstract void copy(Duration duration);
    
    
    /**
     * Tests if the Duration is null.
     * @return true wheter the Duration is null, false otherwise.
     */
    public abstract boolean isNull();
    
    /**
     * Returns a String formated representation of the Duration.
     * @return a String formated representation of the Duration
     */
    public abstract String getValue();

    
    
    
    /**
     * Tests whether the Duration is defined.
     * @return true whether the Duration is defined, false otherwise.
     */
    public abstract boolean isDefined();
    
    /**
     * Returns an English description of the Duration.
     * @return an English description of the Duration
     */
    public abstract String toString();
    
    /**
     * Gets a clone
     * @return a clone.
     */
    public abstract Object clone() throws CloneNotSupportedException;
    
    
    

    public boolean equals(Object e){
        if (e.getClass().equals(this.getClass())){
            Duration duration = (Duration)e;
            if (this.getXipNode() != null && duration.getXipNode() != null)
                return super.equals(e);
            else if (this.getXipNode() == null && duration.getXipNode() == null)
                return this.compareTo(duration) == TemporalRelation.EQUALS;
            else
                return false;
        } else
            return false;
    }

    
    /**
     * Gets the granularity of the duration. The granularity is the value of the
     * lowest non-null unit.
     * @return the int value of the granularity.
     */
    public abstract int getGranularity();
    
}
