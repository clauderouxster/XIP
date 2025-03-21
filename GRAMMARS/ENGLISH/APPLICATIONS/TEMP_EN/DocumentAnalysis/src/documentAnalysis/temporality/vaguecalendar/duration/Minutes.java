/*
 * Years.java
 *
 * Created on December 13, 2006, 10:51 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.duration;

import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;

/**
 *
 * @author xtannier
 */
public class Minutes extends DurationUnit {
    
    static int FG = 5; 
    
    Minutes(){
        super();
    }
    
    public Minutes(String number) throws BadFormatException {
        super(number);
    }

    
    public Minutes(int number){
        super(number);
    }    
    
    /**
     * Returns a one-letter value of the unit
     * @return a one-letter value of the unit
     */
    protected String getUnitValue(){
        return "M";
    }
    
    /**
     * Returns the name of the unit
     * @return the name of the unit
     */
    protected String getUnitString(){
        return "minutes";
    }

    
    /**
     * Returns a SimpleDuration cooresponding to current duration unit.
     * @return a SimpleDuration cooresponding to current duration unit
     */    
    public SimpleDuration getDuration(){
        try {
            return new SimpleDuration("P0Y0M0W0DT0H" + this.getValue() + "0S");
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            return null;
        }
    }      
    
    public SimpleDuration getHalf() {
        try {
            return new SimpleDuration("P0Y0M0W0DT0H0M30S");
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            return null;
        }
    }

    protected SimpleDuration getQuarter() {
        try {
            return new SimpleDuration("P0Y0M0W0DT0H0M15S");        
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

}
