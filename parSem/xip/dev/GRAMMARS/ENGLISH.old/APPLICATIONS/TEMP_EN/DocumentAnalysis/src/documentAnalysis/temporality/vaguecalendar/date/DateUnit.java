/*
 * DateUnit.java
 *
 * Created on January 2, 2007, 11:42 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.date;

import documentAnalysis.temporality.LostGranularityException;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;

/**
 * Abstract class for all Date units (from years to seconds)
 * @author xtannier
 */
public abstract class DateUnit implements TemporalUnit{
    
    private int number;
    private int mod = TemporalConstituent.UNDEFINED;
    
    /**
     * Returns the int value of the DateUnit.
     * @return the int value of the DateUnit.
     */
    public int getIntValue() {
        return this.getNumber();
    }
    
    /**
     * Adds the specified number of units. Returns the number of units to add after the operation.
     * @param i - the number of bigger units to add.
     * @return the number of bigger units to add after the operation.
     */
    public abstract int add(int i) throws LostGranularityException;
    
    /**
     * Returns the date format value with only the date unit defined
     * @return the date format value with only the date unit defined
     */
    public abstract String getDateValue();
    
    
    /**
     * Returns the value of the granularity corresponding to this unit.
     * @return the value of the granularity corresponding to this unit.
     */
    public abstract int getGranularity();
    
    /**
     * Returns the date corresponding to the specified mod and/or String value for this unit
     * @return the corresponding VagueDate.
     */
    public abstract VagueDate getRealDate(boolean complete);
    
    
    /**
     * Gets a clone
     * @return a clone.
     */
    public abstract Object clone() throws CloneNotSupportedException;
    
    
    /**
     * Gets the duration corresponding to the unit
     *
     * @return the SimpleDuration correspond to the unit
     */
    public abstract SimpleDuration getAssociatedDuration();
    
    /**
     * Gets the mod of the unit
     * @return the mod of the unit
     */
    public int getMod() {
        return mod;
    }

    /**
     * Gets the mod of the unit if it corresponds to a moment (beginning, middle, end...).
     * If it corresponds to a vagueness (exact, approx...), return UNDEFINED
     * @return the mod of the unit
     */
    public int getModMoment() {
        if (TemporalConstituent.isModMoment(mod))
            return mod;
        else
            return TemporalConstituent.UNDEFINED;
    }

  
    
    /**
     * Sets the mod of the unit
     * @param mod the mod of the unit
     */
    public void setMod(int mod) {
        this.mod = mod;
    }
    
    public int getNumber() {
        return number;
    }
    
    public void setNumber(int number) throws BadFormatException {
        this.number = number;
    }
    
    
    /**
     * Comparison when at least one month to compare is represented by
     * a String rather than an int.
     */
    int compareSpecial(DateUnit unit){
        VagueDate date1;
        VagueDate date2;
        if (this.getNumber() == -1 || this.getMod() != TemporalConstituent.UNDEFINED)
            date1 = this.getRealDate(true).completeByForce();
        else
            date1 = new VagueSimpleDate(this).completeByForce();
        
        if (unit.getNumber() == -1 || unit.getMod() != TemporalConstituent.UNDEFINED)
            date2 = unit.getRealDate(true).completeByForce();
        else
            date2 = new VagueSimpleDate(unit).completeByForce();
        return date1.compareTo(date2);
    }
    
    
    public abstract boolean equals(Object other);
    
}
