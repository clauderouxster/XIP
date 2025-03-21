/*
 * MultipleDuration.java
 *
 * Created on December 13, 2006, 11:57 AM
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
public class MultipleDuration extends Duration{
    
    private String operator;
    private Duration[] durations;
    
    /** Creates a new instance of VagueMultipleDate */
    public MultipleDuration() {
        this.operator = "";
        this.durations = new SimpleDuration[1];
        this.durations[0] = new SimpleDuration();
    }
    
    /**
     *
     * @param durationFormat
     * @throws documentanalysis.temporality.vaguecalendar.BadFormatException
     */
    public MultipleDuration(String durationFormat) throws BadFormatException {
        String[] durations;
        if (durationFormat.contains("|")){
            durations = durationFormat.split("\\|");
            this.durations = new Duration[durations.length];
            for (int i = 0 ; i < durations.length ; ++i){
                this.durations[i] = Duration.getNewDuration(durations[i]);
            }
            this.operator = "|";
        } else if (durationFormat.contains("#")){
            durations = durationFormat.split("#");
            this.durations = new Duration[durations.length];
            for (int i = 0 ; i < durations.length ; ++i){
                this.durations[i] = Duration.getNewDuration(durations[i]);
            }
            this.operator = "#";
        }
    }
    
    
    /**
     * Gets a clone
     * @return a clone.
     */
    public Object clone() throws CloneNotSupportedException{
        MultipleDuration result = new MultipleDuration();
        result.operator = this.operator;
        result.durations = new Duration[this.durations.length];
        for (int i = 0 ; i < this.durations.length ; ++i){
            result.durations[i] = (Duration)this.durations[i].clone();
        }
        return result;
    }
    
    
    /**
     * Tests whether the Duration is defined.
     * @return true whether the Duration is defined, false otherwise.
     */
    public boolean isDefined() {
        for (Duration duration : durations){
            if (duration.isDefined())
                return true;
        }
        return false;
    }
    
    /**
     * Returns a String formated representation of the Duration.
     * @return a String formated representation of the Duration
     */
    public String getValue() {
        String result = "";
        for (int i = 0 ; i < this.durations.length ; ++i){
            result += this.durations[i].getValue();
            if (i < this.durations.length - 1)
                result += this.getOperator();
        }
        return result;
    }
    
    /**
     * Returns an English description of the Duration.
     * @return an English description of the Duration
     */
    public String toString() {
        String result = "";
        String operatorValue = "";
        if (this.getOperator().equals("|"))
            operatorValue = " or ";
        else if (this.getOperator().equals("#"))
            operatorValue = " and ";
        
        for (int i = 0 ; i < this.durations.length ; ++i){
            result += this.durations[i].toString();
            if (i < this.durations.length - 1)
                result += operatorValue;
        }
        return result;
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
        boolean allSmaller = true;
        boolean allBigger  = true;
        if (o instanceof Duration) {
            Duration dur = (Duration)o;
            for (int i = 0 ; i < this.durations.length && (allSmaller || allBigger) ; ++i){
                if (this.durations[i].compareTo(dur) >= 0)
                    allSmaller = false;
                if (this.durations[i].compareTo(dur) <= 0)
                    allBigger = false;
            }
            if (allSmaller)
                return -1;
            else if (allBigger)
                return 1;
            else
                return TemporalRelation.UNDEFINED;
        } else
            throw new NotImplementedException();
    }
    
    /**
     * Returns the operator of this multiple duration.
     * @return the operator of this multiple duration.
     */
    public String getOperator() {
        return operator;
    }
    
    /**
     * Returns an array containing all durations of this multiple duration.
     * @return an array containing all durations of this multiple duration.
     */
    public Duration[] getAllDurations() {
        return durations;
    }
    
    
    /**
     * Gets the granularity of the duration. The granularity is the value of the
     * lowest non-null unit.
     * @return the int value of the granularity.
     */
    public int getGranularity() {
        int max = this.durations[0].getGranularity();
        for (int i = 1 ; i < this.durations.length ; ++i)
            max = java.lang.Math.max(max, this.durations[i].getGranularity());
        return max;
    }
}
