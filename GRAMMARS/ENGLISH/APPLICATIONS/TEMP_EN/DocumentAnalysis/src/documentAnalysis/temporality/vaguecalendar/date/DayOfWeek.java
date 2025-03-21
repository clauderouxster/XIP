/*
 * DayOfWeek.java
 *
 * Created on November 29, 2006, 10:45 AM
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
import documentAnalysis.temporality.vaguecalendar.TempConst;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;
import documentAnalysis.temporality.vaguecalendar.duration.Days;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;

/**
 * This class represents a day of the week (from Monday to Sunday)
 * @author xtannier
 */
public class DayOfWeek extends DateUnit{
    
    private String dayOfWeek;
    
    /** Creates a new instance of DayOfWeek */
    DayOfWeek() {
        try {
            this.setNumber(-1);
        } catch (BadFormatException ex) {
            ex.printStackTrace();
        }
        this.dayOfWeek = "";
    }
    
    
    DayOfWeek(int dayOfWeek) throws BadFormatException {
        this();
        if (dayOfWeek < 0 || dayOfWeek > 7) {
            throw new BadFormatException(String.valueOf(dayOfWeek));
        } else {
            if (dayOfWeek == 0)
                this.setNumber(7);
            else
                this.setNumber(dayOfWeek);
        }
    }
    
    
    DayOfWeek(String dayOfWeek) throws BadFormatException {
        if (dayOfWeek.length() != 1) {
            throw new BadFormatException(dayOfWeek);
        } else {
            if (dayOfWeek.equals("-")) {
                this.dayOfWeek = "";
                this.setNumber(-1);
            } else if (dayOfWeek.toUpperCase().equals("W")) {
                this.dayOfWeek = dayOfWeek.toUpperCase();
                this.setNumber(-1);
            } else if (dayOfWeek.matches("[0-9]+")) {
                this.dayOfWeek = "-";
                this.setNumber(Integer.parseInt(dayOfWeek));
            } else {
                throw new BadFormatException(dayOfWeek);
            }
        }
    }
    
    /**
     * Gets a copy.
     * @return a copy.
     */
    public Object clone() throws CloneNotSupportedException{
        DayOfWeek result = new DayOfWeek();
        try {
            result.setNumber(this.getNumber());
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
        result.dayOfWeek = this.dayOfWeek;
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
        } else if (this.isWeekEnd()) {
            result = (new Days(2)).getDuration();
            result.setMod(TemporalConstituent.EXACT);
            return result;
        } else
            throw new NotImplementedException();
    }
    
    
    public boolean isWeekEnd() {
        return this.dayOfWeek.equals("W");
    }
    
    public int compareTo(TemporalUnit other) {
        if (other instanceof DayOfWeek){
            if (this.equals(((DayOfWeek)other)))
                return TemporalRelation.EQUALS;
            else if (this.getNumber() != -1 && ((DayOfWeek)other).getNumber() != -1){
                int i = (new Integer(this.getNumber())).compareTo(new Integer(((DayOfWeek)other).getNumber()));
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
        if (other instanceof DayOfWeek)
            return (this.getNumber() == ((DayOfWeek)other).getNumber() &&
                    this.dayOfWeek.equals(((DayOfWeek)other).dayOfWeek)) &&
                    this.getMod() == ((DayOfWeek)other).getMod();
        else
            return false;
    }
    
    
    public boolean isDefined() {
        return (this.getNumber() != -1 || !this.dayOfWeek.equals(""));
    }
    
    
    public String toString() {
        if (this.getNumber() > 0) {
            return TempConst.mapDaysOfWeek.get(String.valueOf(this.getNumber()));
        } else if (!this.dayOfWeek.equals("")) {
            return TempConst.mapDaysOfWeek.get(this.dayOfWeek);
        } else {
            return "";
        }
    }
    
    
    public int getGranularity(){
        return TemporalConstituent.DAY;
    }
    
    
    public String getValue(){
        if (this.getNumber() == -1){
            if (!this.dayOfWeek.equals(""))
                return this.dayOfWeek;
            else
                return "-";
        } else {
            return CommonFunctions.formatInt(this.getNumber(), 1);
        }
    }
    
    
    
    
    public VagueDate getRealDate(boolean complete){
        int mod = this.getModMoment();
        if (this.isDefined() &&
                (this.getNumber() == -1 || mod != TemporalConstituent.UNDEFINED)){
            try{
                DateUnit unit1;
                DateUnit unit2;
                
                if (this.getValue().toUpperCase().equals("W")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new DayOfWeek(5);
                        unit2 = new DayOfWeek(6);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new DayOfWeek(6);
                        unit2 = new DayOfWeek(7);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new DayOfWeek(7);
                        unit2 = new DayOfWeek(7);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new DayOfWeek(5);
                        unit2 = new DayOfWeek(7);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("-") ||
                        mod != TemporalConstituent.UNDEFINED){
                    if (mod == TemporalConstituent.END){
                        unit1 = new Hour(18);
                        unit2 = new Hour(24);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(11);
                        unit2 = new Hour(16);
                    } else if (mod == TemporalConstituent.START){
                        unit1 = new Hour(1);
                        unit2 = new Hour(11);
                    } else {
                        throw new NotImplementedException();
                    }
                } else
                    throw new NotImplementedException("Month " + this.getValue() + " unknown.");
                
                VagueSimpleDate date1;
                VagueSimpleDate date2;
                if (this.getNumber() == -1){
                    date1 = new VagueSimpleDate(unit1);
                    date2 = new VagueSimpleDate(unit2);
                } else {
                    date1 = new VagueSimpleDate(this);
                    date2 = new VagueSimpleDate(this);
                    if (unit1 instanceof Hour && unit2 instanceof Hour){
                        date1.setHour((Hour)unit1);
                        date2.setHour((Hour)unit2);
                    }
                }
                
                if (complete){
                    date1 = date1.completeByForce().getLowerBound();
                    date2 = date2.completeByForce().getUpperBound();
                }
                return new VagueInterval(date1, date2);
            } catch (BadFormatException e){
                e.printStackTrace();
                System.exit(-1);
                return null;
            } catch (IntervalException e){
                e.printStackTrace();
                System.exit(-1);
                return null;
            }
        } else{
            VagueSimpleDate result = new VagueSimpleDate(this);
            return result;
        }
    }
    
    
    
    
    
    /**
     * Throws a NotImplementedException. Adding days through this method is not
     * possible.
     * @return nothing
     */
    public int add(int i) {
        throw new NotImplementedException("Do not use this method to add days, use DayOfMonth.add() instead.");
    }
    
    
    /**
     * Returns the date format value with only the date unit defined
     * @return the date format value with only the date unit defined
     */
    public String getDateValue(){
        return "---------T-------O---W--" + this.getValue();
    }
    
    
    
    
}
