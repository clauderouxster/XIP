/*
 * DayOfMonth.java
 *
 * Created on November 29, 2006, 10:46 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.date;

import common.NotImplementedException;
import documentAnalysis.temporality.LostGranularityException;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.CommonFunctions;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;
import documentAnalysis.temporality.vaguecalendar.duration.Days;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;

/**
 *
 * @author xtannier
 */
public class DayOfMonth extends DateUnit {
    
    /** maximum number of days (depending of the full date, if known) */
    private int limit;
    
    /** Creates a new instance of DayOfMonth */
    DayOfMonth() {
        try {
            this.setNumber(-1);
        } catch (BadFormatException ex) {
            ex.printStackTrace();
        }
        this.limit = 28;
    }
    
    
    /**
     * Creates a new instance of DayOfMonth, according to the specified two-char String representation.
     * @param day the two-char String representation of the day.
     * @throw BadFormatException if the format is not correct.
     */
    DayOfMonth(String day) throws BadFormatException {
        if (day.length() != 2) {
            throw new BadFormatException(day);
        } else {
            if (day.equals("--")) {
                this.setNumber(-1);
            } else if (day.matches("[0-9]+")) {
                this.setNumber(Integer.parseInt(day));
            } else {
                throw new BadFormatException(day);
            }
        }
        this.limit = 28;
    }
    
    
    /**
     * Creates a new instance of Minute, according to the specified number
     * @param day the number of minutes
     */
    DayOfMonth(int day) throws BadFormatException {
        this.setNumber(day);
        this.limit = 28;
    }
    
    /**
     * Gets a copy.
     * @return a copy.
     */
    public Object clone() throws CloneNotSupportedException{
        DayOfMonth result = new DayOfMonth();
        try {
            result.setNumber(this.getNumber());
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
        result.limit = this.limit;
        result.setMod(this.getMod());
        return result;
    }
    
    
    /**
     * Gets the duration corresponding to the unit
     *
     * @return the SimpleDuration correspond to the unit
     */
    public SimpleDuration getAssociatedDuration(){
        SimpleDuration result;
        if (this.getNumber() != -1){
            result = (new Days(1)).getDuration();
            result.setMod(TemporalConstituent.EXACT);
            return result;
        } else
            throw new NotImplementedException();
    }
    
    
    public int getGranularity(){
        return TemporalConstituent.DAY;
    }
    
    
    /**
     * Compares this object with the specified object for order.
     * Use {@link TemporalRelation} temporal constants instead of negative/positive values.
     *
     * @param other the temporal unit to compare with the current one
     * @return an int value representing the temporal relation.
     */
    public int compareTo(TemporalUnit other) {
        if (other instanceof DayOfMonth){
            if (this.equals((DayOfMonth)other))
                return TemporalRelation.EQUALS;
            else if (this.getNumber() != -1 && ((DayOfMonth)other).getNumber() != -1){
                int i = (new Integer(this.getNumber())).compareTo(new Integer(((DayOfMonth)other).getNumber()));
                if (i < 0)
                    return TemporalRelation.BEFORE;
                else if (i > 0)
                    return TemporalRelation.AFTER;
                else
                    return TemporalRelation.EQUALS;
            }
            // one of them not defined
            else
                return TemporalRelation.UNDEFINED;
        } else {
            throw new RuntimeException("Not comparable!");
        }
    }
    
    

    public boolean equals(Object other) {
        if (other instanceof DayOfMonth)
            return this.getNumber() == ((DayOfMonth)other).getNumber() &&
                    this.getMod() == ((DayOfMonth)other).getMod();
        else
            return false;
    }
    
    /**
     * Tests if the Unit is defined.
     * @return true whether the Unit is defined, false otherwise.
     */
    public boolean isDefined() {
        return (this.getNumber() != -1);
    }
    
    
    boolean isComplete() {
        return (this.getNumber() > 0);
    }
    

    public String toString() {
        if (this.getNumber() == -1) {
            return "";
        } else {
            return String.valueOf(this.getNumber());
        }
    }
    
    /**
     * Returns a String formated representation of the Unit.
     * @return a String formated representation of the Unit.
     */
    public String getValue(){
        if (this.getNumber() == -1){
            return "--";
        } else {
            return CommonFunctions.formatInt(this.getNumber(), 2);
        }
    }
    
    /**
     * Gets the maximum number of days in the month (depending of the full date, if known).
     * Returns 28, the lowest maximum, if the full date is unknown.
     * @return the maximum number of days of the month if the full date is known, 28 otherwise
     */
    public int getLimit() {
        return limit;
    }
    
    /**
     * Sets the maximum number of days in the month.
     */
    void setLimit(int limit) {
        this.limit = limit;
    }
    
    
    /**
     * Returns the date corresponding to the specified mod and/or String value for this unit
     * @return the corresponding VagueDate.
     */    
    public VagueDate getRealDate(boolean complete){
        try{
            int mod = this.getModMoment();
            if (this.isDefined() && mod != TemporalConstituent.UNDEFINED){
                Hour hour1;
                Hour hour2;
                if (mod == TemporalConstituent.END){
                    hour1 = new Hour(18);
                    hour2 = new Hour(24);
                } else if (mod == TemporalConstituent.MID){
                    hour1 = new Hour(11);
                    hour2 = new Hour(16);
                } else if (mod == TemporalConstituent.START){
                    hour1 = new Hour(0);
                    hour2 = new Hour(11);
                } else {
                    throw new NotImplementedException();
                }
                
                VagueSimpleDate date1 = new VagueSimpleDate(hour1);
                VagueSimpleDate date2 = new VagueSimpleDate(hour2);
                
                if (complete){
                    date1 = date1.completeByForce().getLowerBound();
                    date2 = date2.completeByForce().getUpperBound();
                }
                try {
                    return new VagueInterval(date1, date2);
                } catch (IntervalException ex) {
                    ex.printStackTrace();
                    System.exit(-1);
                    return null;
                }
            } else{
                VagueSimpleDate result = new VagueSimpleDate(this);
                return result;
            }
        } catch(BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    
    
    
    /**
     * Adds the specified number of days. Returns the number of months to add after the operation.
     * This operation depends on the year and month of the date. This is provided by the <code>limit</code>
     * attributed to each DayOfMonth.
     * @param i the number of days to add.
     * @return the number of months to add after the operation.
     */
    public int add(int i) throws LostGranularityException {
        try{
            if (this.isDefined()){
                if (i > 0){
                    if (i >= this.limit - this.getNumber())
                        throw new LostGranularityException(i);
                    else if (this.getNumber() == this.limit)
                        throw new LostGranularityException(i);
                    else {
                        this.setNumber(this.getNumber() + 1);
                        return this.add(i-1);
                    }
                } else if (i < 0){
                    if (this.getNumber() == 0){
                        throw new LostGranularityException(i);
                    } else {
                        this.setNumber(this.getNumber() - 1);
                        return this.add(i+1);
                    }
                } else
                    // i == 0
                    return 0;
            } else
                return 0;
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return 0;
        }
    }
    
    /**
     * Returns the date format value with only the date unit defined
     * @return the date format value with only the date unit defined
     */
    public String getDateValue(){
        return "-------" + this.getValue() + "T-------O---W---";
    }
    
}
