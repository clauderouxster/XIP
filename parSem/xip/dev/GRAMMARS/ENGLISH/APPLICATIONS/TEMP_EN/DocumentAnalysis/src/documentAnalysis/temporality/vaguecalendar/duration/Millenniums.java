/*
 * Millenniums.java
 *
 * Created on February 26, 2007, 8:17 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.duration;

import documentAnalysis.temporality.vaguecalendar.BadFormatException;

/**
 * this class represents millenniums
 * @author xtannier
 */
public class Millenniums extends DurationUnit  {
    
    static int FG = 10;

    
    /**
     * Creates a new instance of Millenniums
     */
    public Millenniums() {
        super();
    }
    
    public Millenniums(String number) throws BadFormatException {
        super(number);
    }
    
    
    public Millenniums(int number) throws BadFormatException {
        super(number);
    }
    
    /**
     * Returns a one-letter value of the unit
     * @return a one-letter value of the unit
     */
    protected String getUnitValue(){
        return "Y";
    }
    
    /**
     * Returns the name of the unit
     * @return the name of the unit
     */
    protected String getUnitString(){
        return "millenniums";
    }

    
    /**
     * Returns a SimpleDuration cooresponding to current duration unit.
     * @return a SimpleDuration cooresponding to current duration unit
     */    
    public SimpleDuration getDuration(){
        try {
            return new SimpleDuration("P" + this.getValue() + "0M0W0DT0H0M0S");
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            return null;
        }
    }      
    
    protected SimpleDuration getHalf() {
        try {
            return new SimpleDuration("P500Y0M0W0DT0H0M0S");
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            return null;
        }
    }
    
    protected SimpleDuration getQuarter() {
        try {
            return new SimpleDuration("P250Y0M0W0DT0H0M0S");
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            return null;
        }
    }
    
    /**
     * Gets the fuzzy granularity for the duration unit.
     * The fuzzy granularity is the number of units used for expressing fuzzyness
     * e.g. : 17 minutes is not fuzzy, while 15 minutes can be, because the granularity of minutes is 5.
     * @return the fuzzy granularity for the current duration unit.
     */
    protected int getFuzzyGranularity() {
        return FG;
    }
    
    
    /**
     * Returns a String formated representation of the value
     * @return a String formated representation of the value
     */    
    public String getValue(){
        if (this.isDefined){
            if (this.number == -1){
                return "X000" + this.getUnitValue();
            } else {
                return String.valueOf(this.number) + "000" + this.getUnitValue();
            }
        } else {
            return "";
        }
    }
    
}
