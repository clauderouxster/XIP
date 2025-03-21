/*
 * Minute.java
 *
 * Created on November 29, 2006, 10:44 AM
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
import documentAnalysis.temporality.vaguecalendar.duration.Minutes;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;

/**
 *
 * @author xtannier
 */
public class Minute extends DateUnit{
    
    
    /** Creates a new instance of Minute */
    Minute() {
        try {
            this.setNumber(-1);
        } catch (BadFormatException ex) {
            ex.printStackTrace();
        }
    }
    
    /**
     * Creates a new instance of Minute, according to the specified two-char String representation.
     * @param minute the two-char String representation of the minute.
     * @throw BadFormatException if the format is not correct.
     */
    Minute(String minute) throws BadFormatException {
        if (minute.length() != 2) {
            throw new BadFormatException(minute);
        } else {
            if (minute.equals("--")) {
                this.setNumber(-1);
            } else if (minute.matches("[0-9]+")) {
                this.setNumber(Integer.parseInt(minute));
            } else {
                throw new BadFormatException(minute);
            }
        }
    }
    
    /**
     * Creates a new instance of Minute, according to the specified number
     * @param hour the number of minutes
     */
    Minute(int hour) throws BadFormatException {
        this.setNumber(hour);
    }
    
    /**
     * Gets a copy.
     * @return a copy.
     */
    public Object clone() throws CloneNotSupportedException{
        Minute result = new Minute();
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
        if (this.getNumber() != -1){
            SimpleDuration result = (new Minutes(1)).getDuration();
            result.setMod(TemporalConstituent.EXACT);
            return result;
        } else
            throw new NotImplementedException();
    }
    
    public boolean equals(Object o){
        if (o instanceof Minute){
            return this.getNumber() == ((Minute)o).getNumber() &&
                    this.getMod() == ((Minute)o).getMod();
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
            return String.valueOf(this.getNumber()) + "m";
        }
    }
    
    public int getGranularity(){
        return TemporalConstituent.MINUTE;
    }
    
    
    public String getValue(){
        if (this.getNumber() == -1){
            return "--";
        } else {
            return CommonFunctions.formatInt(this.getNumber(), 2);
        }
    }
    
    
    public VagueDate getRealDate(boolean complete){
        try{
            int mod = this.getModMoment();
            if (this.isDefined() &&
                    (this.getNumber() == -1 || mod != TemporalConstituent.UNDEFINED)){
                Second sec1;
                Second sec2;
                if (mod == TemporalConstituent.END){
                    sec1 = new Second(40);
                    sec2 = new Second(59);
                } else if (mod == TemporalConstituent.MID){
                    sec1 = new Second(20);
                    sec2 = new Second(40);
                } else if (mod == TemporalConstituent.START){
                    sec1 = new Second(1);
                    sec2 = new Second(20);
                } else {
                    throw new NotImplementedException();
                }
                
                VagueSimpleDate date1 = new VagueSimpleDate(sec1);
                VagueSimpleDate date2 = new VagueSimpleDate(sec2);
                
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
            } else {
                VagueSimpleDate result = new VagueSimpleDate(this);
                return result;
            }
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
        
    }
    
    
    
    public int compareTo(TemporalUnit o) {
        if (o instanceof Minute){
            int i = (new Integer(this.getNumber())).compareTo(new Integer(((Minute)o).getNumber()));
            if (i < 0)
                return TemporalRelation.BEFORE;
            else if (i > 0)
                return TemporalRelation.AFTER;
            else
                return TemporalRelation.EQUALS;
        } else {
            throw new RuntimeException("Not comparable!");
        }
    }
    
    /**
     * Adds the specified number of minutes. Returns the number of hours to add after the operation.
     * @param i the number of minutes to add.
     * @return the number of hours to add after the operation.
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
        return "---------T--" + this.getValue() + "---O---W---";
    }
}
