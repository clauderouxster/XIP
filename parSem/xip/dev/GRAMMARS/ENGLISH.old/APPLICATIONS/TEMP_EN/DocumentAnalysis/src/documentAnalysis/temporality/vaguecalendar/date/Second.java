/*
 * Second.java
 *
 * Created on November 29, 2006, 10:40 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.date;

import common.NotImplementedException;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.CommonFunctions;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;
import documentAnalysis.temporality.vaguecalendar.duration.Seconds;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;

/**
 *
 * @author xtannier
 */
public class Second extends DateUnit{
    
    /** Creates a new instance of Second */
    Second() {
        try {
            this.setNumber(-1);
        } catch (BadFormatException ex) {
            ex.printStackTrace();
        }
    }
    
    /**
     * Creates a new instance of Second, according to the specified two-char String representation.
     * @param second the two-char String representation of the second.
     * @throw BadFormatException if the format is not correct.
     */
    Second(String second) throws BadFormatException {
        if (second.length() != 2) {
            throw new BadFormatException(second);
        } else {
            if (second.equals("--")) {
                this.setNumber(-1);
            } else if (second.matches("[0-9]+")) {
                this.setNumber(Integer.parseInt(second));
            } else {
                throw new BadFormatException(second);
            }
        }
    }
    
    
    /**
     * Creates a new instance of Second, according to the specified number
     * @param second the number of seconds
     */
    Second(int second) throws BadFormatException {
        this.setNumber(second);
    }
    
    /**
     * Gets a copy.
     * @return a copy.
     */
    public Object clone() throws CloneNotSupportedException{
        Second result = new Second();
        try {
            result.setNumber(this.getNumber());
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
        result.setMod(this.getMod());
        return result;
    }
    
    /**
     * Gets the duration corresponding to the unit
     *
     * @return the SimpleDuration correspond to the unit
     */
    public SimpleDuration getAssociatedDuration(){
        if (this.getNumber() != -1) {
            SimpleDuration result = (new Seconds(1)).getDuration();
            result.setMod(TemporalConstituent.EXACT);
            return result;
        } else
            throw new NotImplementedException();
        
    }
    
    
    public boolean equals(Object o){
        if (o instanceof Second) {
            return this.getNumber() == ((Second)o).getNumber() &&
                    this.getMod() == ((Second)o).getMod();
        } else {
            return false;
        }
    }
    
    public boolean isDefined(){
        return this.getNumber() >= 0;
    }
    
    public String toString() {
        if (this.getNumber() == -1) {
            return "";
        } else {
            return String.valueOf(this.getNumber()) + "s";
        }
    }
    
    public int compareTo(TemporalUnit other) {
        if (other instanceof Second){
            if (this.equals(((Second)other)))
                return TemporalRelation.EQUALS;
            else if (this.getNumber() != -1 && ((Second)other).getNumber() != -1){
                int i = (new Integer(this.getNumber())).compareTo(new Integer(((Second)other).getNumber()));
                if (i < 0)
                    return TemporalRelation.BEFORE;
                else if (i > 0)
                    return TemporalRelation.AFTER;
                else
                    return TemporalRelation.EQUALS;
            } else {
                return TemporalRelation.UNDEFINED;
            }
        } else{
            throw new RuntimeException("Not comparable!");
        }
    }
    
    
    public int getGranularity(){
        return TemporalConstituent.SECOND;
    }
    
    public String getValue(){
        if (this.getNumber() == -1){
            return "--";
        } else {
            return CommonFunctions.formatInt(this.getNumber(), 2);
        }
    }
    
    
    public VagueDate getRealDate(boolean complete){
        int mod = this.getModMoment();
        if (this.isDefined() &&
                (this.getNumber() == -1 || mod != TemporalConstituent.UNDEFINED)){
            throw new NotImplementedException();
        } else{
            VagueSimpleDate result = new VagueSimpleDate(this);
            return result;
        }
    }
    
    
    /**
     * Adds the specified number of seconds. Returns the number of minutes to add after the operation.
     * @param i the number of seconds to add.
     * @return the number of minutes to add after the operation.
     */
    public int add(int i) {
        try{
            if (this.isDefined()){
                if (i > 0){
                    if (this.getNumber() == 59){
                        this.setNumber(0);
                        return 1 + this.add(i-1);
                    } else {
                        this.setNumber(this.getNumber() + 1);
                        return this.add(i-1);
                    }
                } else if (i < 0){
                    if (this.getNumber() == 0){
                        this.setNumber(59);
                        return -1 + this.add(i+1);
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
        return "---------T----" + this.getValue() + "-O---W---";
    }
}
