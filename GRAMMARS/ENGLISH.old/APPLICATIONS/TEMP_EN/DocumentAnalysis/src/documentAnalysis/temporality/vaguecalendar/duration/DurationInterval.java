/*
 * DurationInterval.java
 *
 * Created on December 13, 2006, 12:01 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.duration;

import common.NotImplementedException;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;

/**
 *
 * @author xtannier
 */
public class DurationInterval extends Duration{
    
    private SimpleDuration lowerBound;
    private SimpleDuration upperBound;
    
    /** Creates a new instance of VagueInterval */
    public DurationInterval() {
        this.lowerBound = new SimpleDuration();
        this.upperBound = new SimpleDuration();
    }
    
    /**
     * 
     * 
     * 
     * @param lowerBound
     * @param upperBound
     * @throws documentanalysis.temporality.vaguecalendar.BadFormatException
     */
    public DurationInterval(SimpleDuration lowerBound, SimpleDuration upperBound) throws BadFormatException {
        this.lowerBound = lowerBound;
        this.upperBound = upperBound;
    }
    
    /**
     * 
     * @param dateFormat 
     * @throws documentanalysis.temporality.vaguecalendar.BadFormatException 
     */
    public DurationInterval(String dateFormat) throws BadFormatException {
        String[] durations = dateFormat.split("/");
        if (durations.length != 2){
            throw new BadFormatException(dateFormat);
        } else {
            this.lowerBound = new SimpleDuration(durations[0]);
            this.upperBound = new SimpleDuration(durations[1]);
        }
    }
    
    
    /**
     * Gets a clone
     * @return a clone.
     */
    public Object clone() throws CloneNotSupportedException{
        DurationInterval result = new DurationInterval();
        result.lowerBound = (SimpleDuration)this.lowerBound.clone();
        result.upperBound = (SimpleDuration)this.upperBound.clone();        
        return result;
    }    
    
    /**
     * Tests whether the Duration is defined.
     * @return true whether the Duration is defined, false otherwise.
     */
    public boolean isDefined(){
        return getLowerBound().isDefined() || getUpperBound().isDefined();
    }

    /**
     * Returns a String formated representation of the Duration.
     * @return a String formated representation of the Duration
     */
    public String getValue() {
        return this.getLowerBound().getValue() + "/" + this.getUpperBound().getValue();
    }

    /**
     * Returns an English description of the Duration.
     * @return an English description of the Duration
     */
    public String toString() {
        return "between " + this.getLowerBound().toString() + " and " + this.getUpperBound().toString();
    }
    
    /**
     * Tests if the Duration is null.
     * @return true wheter the Duration is null, false otherwise.
     */
    public boolean isNull(){
        return false;
    }

    /**
     * Compares this object with the specified object for order.
     * Use {@link documentAnalysis.temporality.TemporalRelation} temporal constants 
     * instead of negative/positive values.
     *
     * 
     */
    public int compareTo(TemporalUnit o) {
        if (o instanceof DurationInterval){
            DurationInterval dur = (DurationInterval)o;
            if (this.getLowerBound().compareTo(dur) == this.getUpperBound().compareTo(dur))
                return this.getLowerBound().compareTo(dur);
            else
                return TemporalRelation.UNDEFINED;
        } else if (o instanceof SimpleDuration) {
            SimpleDuration dur = (SimpleDuration)o;
            if (dur.compareTo(this.getLowerBound()) == dur.compareTo(this.getUpperBound()))
                return dur.compareTo(this.getLowerBound());
            else
                return TemporalRelation.UNDEFINED;            
        } else if (o instanceof MultipleDuration) {
            int c = ((MultipleDuration)o).compareTo(this);
            if (c == TemporalRelation.UNDEFINED)
                return c;
            else
                return -1 * c;            
        } else 
            throw new NotImplementedException();
    }

    /**
     * Returns the lower bound of the interval
     * @return the lower bound of the interval
     */
    public SimpleDuration getLowerBound() {
        return lowerBound;
    }

    /**
     * Returns the upper bound of the interval
     * @return the upper bound of the interval
     */
    public SimpleDuration getUpperBound() {
        return upperBound;
    }

    
    /**
     * Gets the granularity of the duration. The granularity is the value of the 
     * lowest non-null unit.
     * @return the int value of the granularity.
     */
    public int getGranularity() {
        return java.lang.Math.max(this.lowerBound.getGranularity(), this.upperBound.getGranularity());        
    }         

}
