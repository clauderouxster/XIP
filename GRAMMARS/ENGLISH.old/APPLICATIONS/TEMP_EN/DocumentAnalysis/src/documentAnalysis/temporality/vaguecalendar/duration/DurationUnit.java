/**
 * Years.java
 *
 * Created on December 13, 2006, 10:51 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.duration;

import common.NotImplementedException;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;

/**
 * This abstract class allows the definition of any duration unit.
 * @author xtannier
 */
public abstract class DurationUnit implements TemporalUnit{
    
    int number;
    boolean isDefined;
    
    
    /**
     * Creates a new instance of DurationUnit
     */
    DurationUnit(){
        this.number = 0;
        this.isDefined = false;
    }
    
    /**
     * Creates a new instance of DurationUnit
     */
    public DurationUnit(String number) throws BadFormatException {
        if (number.matches("[0-9]+")){
            this.number = Integer.parseInt(number);
            this.isDefined = true;
        } else if (number.equals("X")) {
            this.number = -1;
            this.isDefined = true;
        } else {
            throw new BadFormatException(number);
        }
    }
    
    
    /**
     * Gets a clone
     * @return a clone.
     */
    public Object clone() throws CloneNotSupportedException{
        return super.clone();
    }
    
    
    /**
     * Creates a new instance of DurationUnit
     */
    DurationUnit(int number){
        this.number = number;
        this.isDefined = true;
    }
    
    
    /**
     * Tests if the unit has been defined.
     * @return true whether the unit is defined, false othersiwe.
     */
    public boolean isDefined(){
        return this.isDefined;
    }
    
    /**
     * Returns a String formated representation of the value
     * @return a String formated representation of the value
     */
    public String getValue(){
        if (this.isDefined){
            if (this.number == -1){
                return "X" + this.getUnitValue();
            } else {
                return String.valueOf(this.number) + this.getUnitValue();
            }
        } else {
            return "0" + this.getUnitValue();
        }
    }
    
    
    /**
     * Returns the number corresponding to the Unit.
     * @return the int number corresponding to the Unit, and -1 if the DurationUnit is
     *  not defined.
     */
    public int getIntValue(){
        return this.number;
    }
    
    
    /**
     * Returns an English description of the value.
     * @return an English description of the value.
     */
    public String toString(){
        if (this.isDefined){
            if (this.number == -1){
                return "several " + this.getUnitString();
            } else {
                return String.valueOf(this.number) + " " + this.getUnitString();
            }
        } else {
            return "";
        }
    }
    
    /**
     *
     * @param o the TemporalUnit to compare to the current unit.
     * @return the same as in usual compareTo.
     */
    public int compareTo(TemporalUnit o){
        if (o instanceof DurationUnit){
            DurationUnit unit = (DurationUnit)o;
            if (unit.getClass().equals(this.getClass())){
                if (!unit.isDefined() && !this.isDefined())
                    return 0;
                else if (!unit.isDefined() && this.isNull())   
                    return 0;
                else if (unit.isNull() && !this.isDefined())   
                    return 0;                
                else if (unit.isDefined() && this.isDefined()){
                    // number = -1 means "several""
                    if (this.number == -1){
                        if (unit.number == 1 || unit.number == 2)
                            return 1;
                        else 
                            return TemporalRelation.UNDEFINED;
                    } else if (unit.number == -1){
                        if (this.number == 1 || this.number == 2)
                            return -1;
                        else 
                            return TemporalRelation.UNDEFINED;
                    } else
                        return (new Integer(this.number)).compareTo(unit.number);
                }
                else
                    return -1;
            } else {
                throw new RuntimeException("These elements cannot be compared !");
            }
        } else
            throw new RuntimeException("Not comparable!");
    }
    
    /**
     * Tests if the duration unit has a null value.
     * @return true whether the duration unit has a null value, false otherwise.
     */
    boolean isNull(){
        return (this.number == 0);
    }
    
    
    /**
     * Returns a DurationUnit corresponding to the specified granularity and number
     * @param granularity the granularity of the expected DurationUnit
     * @param number the number of units
     * @return the expected DurationUnit
     */
    public static DurationUnit getDurationUnit(int granularity, String number) throws BadFormatException{
        if (granularity == TemporalConstituent.MILLENNIUM)
            return new Millenniums(number);
        else if (granularity == TemporalConstituent.CENTURY)
            return new Centuries(number);
        else if (granularity == TemporalConstituent.DECADE)
            return new Decades(number);
        else if (granularity == TemporalConstituent.YEAR)
            return new Years(number);
        else if (granularity == TemporalConstituent.MONTH)
            return new Months(number);
        else if (granularity == TemporalConstituent.WEEK)
            return new Weeks(number);
        else if (granularity == TemporalConstituent.DAY)
            return new Days(number);
        else if (granularity == TemporalConstituent.HOUR)
            return new Hours(number);
        else if (granularity == TemporalConstituent.MINUTE)
            return new Minutes(number);
        else if (granularity == TemporalConstituent.SECOND)
            return new Seconds(number);
        else
            throw new NotImplementedException("Unknown granularity");
    }
    
    
    /**
     * Gets the fuzzy granularity for the duration unit.
     * The fuzzy granularity is the number of units used for expressing fuzzyness
     * e.g. : 17 minutes is not fuzzy, while 15 minutes can be, because the granularity of minutes is 5.
     * @return the fuzzy granularity for the current duration unit.
     */
    protected abstract int getFuzzyGranularity();
            
    /**
     * Multiplies the value of the unit by the specified number.
     * @return a new DurationUnit
     */
    protected DurationUnit multiply(int i){
        DurationUnit result = this;
        result.number *= i;
        return result;
    }
    
    /**
     * Returns a one-letter value of the unit
     * @return a one-letter value of the unit
     */
    protected abstract String getUnitValue();
    
    /**
     * Returns the name of the unit
     * @return the name of the unit
     */
    protected abstract String getUnitString();

    
    /**
     * Returns a SimpleDuration cooresponding to current duration unit.
     * @return a SimpleDuration cooresponding to current duration unit
     */
    public abstract SimpleDuration getDuration();
    
    
    /**
     * Returns a SimpleDuration cooresponding to a half of the current duration unit.
     * @return a SimpleDuration cooresponding to a half of the current duration unit
     */
    protected abstract SimpleDuration getHalf();
    
    /**
     * Returns a SimpleDuration cooresponding to a quarter of the current duration unit.
     * @return a SimpleDuration cooresponding to a quarter of the current duration unit
     */
    protected abstract SimpleDuration getQuarter();
    
    
}
