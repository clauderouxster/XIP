/*
 * Hour.java
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
import documentAnalysis.temporality.vaguecalendar.TempConst;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;
import documentAnalysis.temporality.vaguecalendar.duration.Hours;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;

/**
 *
 * @author xtannier
 */
public class Hour extends DateUnit {
    
    private String hour;
    
    /** Creates a new instance of Hour */
    Hour() {
        try {
            this.setNumber(-1);
        } catch (BadFormatException ex) {
            ex.printStackTrace();
        }
        this.hour = "";
    }
    
    /**
     * Creates a new instance of Hour, according to the specified two-char String representation.
     * @param hour the two-char String representation of the hour.
     * @throw BadFormatException if the format is not correct.
     */
    Hour(String hour) throws BadFormatException {
        if (hour.length() != 2) {
            throw new BadFormatException(hour);
        } else {
            if (hour.toUpperCase().equals("DT") ||
                    hour.toUpperCase().equals("EV") ||
                    hour.toUpperCase().equals("AF") ||
                    hour.toUpperCase().equals("MI") ||
                    hour.toUpperCase().equals("MO") ||
                    hour.toUpperCase().equals("NI")) {
                this.hour = hour.toUpperCase();
                this.setNumber(-1);
            } else if (hour.equals("--")) {
                this.hour = "";
                this.setNumber(-1);
            } else if (hour.matches("[0-9]+")) {
                this.hour = "--";
                this.setNumber(Integer.parseInt(hour));
            } else {
                throw new BadFormatException(hour);
            }
        }
    }
    
    /**
     * Creates a new instance of Hour, according to the specified number
     * @param hour the number of hours
     */
    Hour(int hour) throws BadFormatException {
        this.hour = "--";
        this.setNumber(hour);
    }
    
    /**
     * Gets a copy.
     * @return a copy.
     */
    public Object clone() throws CloneNotSupportedException{
        Hour result = new Hour();
        try {
            result.setNumber(this.getNumber());
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
        result.hour = this.hour;
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
            
            SimpleDuration result = (new Hours(1)).getDuration();
            result.setMod(TemporalConstituent.EXACT);
            return result;
        }else
            
            try {
                return (new Hours("X")).getDuration();
            } catch (BadFormatException ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            }
    }
    
    public void setNumber(int number) throws BadFormatException {
        super.setNumber(number);
        if (this.getNumber() > 24)
            throw new BadFormatException("The Hour part of a date must be less than 24");
    }
    
    public boolean isDefined(){
        return this.getNumber() >= 0 || !this.hour.equals("");
    }
    
    public String toString() {
        if (this.getNumber() >= 0) {
            return String.valueOf(this.getNumber()) + "h";
        } else if (!this.hour.equals("")) {
            return TempConst.mapHours.get(this.hour);
        } else {
            return "";
        }
    }
    
    public boolean equals(Object o){
        if (o instanceof Hour){
            return (this.getNumber() == ((Hour)o).getNumber()) && this.hour.equals(((Hour)o).hour) &&
                    this.getMod() == ((Hour)o).getMod();
        } else{
            return false;
        }
        
    }
    
    public int getGranularity(){
        return TemporalConstituent.HOUR;
    }
    
    
    public String getValue(){
        if (this.getNumber() == -1){
            if (!this.hour.equals(""))
                return this.hour;
            else
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
                DateUnit unit1;
                DateUnit unit2;
                
                if (this.getValue().toUpperCase().equals("DT")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Hour(7);
                        unit2 = new Hour(12);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(11);
                        unit2 = new Hour(16);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(15);
                        unit2 = new Hour(18);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Hour(7);
                        unit2 = new Hour(18);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("EV")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Hour(17);
                        unit2 = new Hour(20);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(19);
                        unit2 = new Hour(22);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(20);
                        unit2 = new Hour(23);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Hour(17);
                        unit2 = new Hour(23);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("AF")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Hour(12);
                        unit2 = new Hour(16);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(15);
                        unit2 = new Hour(18);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(17);
                        unit2 = new Hour(19);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Hour(12);
                        unit2 = new Hour(19);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("MI")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Hour(11);
                        unit2 = new Hour(13);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(13);
                        unit2 = new Hour(14);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(13);
                        unit2 = new Hour(15);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Hour(11);
                        unit2 = new Hour(15);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("MO")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Hour(1);
                        unit2 = new Hour(8);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(8);
                        unit2 = new Hour(11);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(10);
                        unit2 = new Hour(12);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Hour(1);
                        unit2 = new Hour(12);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("NI")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Hour(18);
                        unit2 = new Hour(21);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(21);
                        unit2 = new Hour(24);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Hour(22);
                        unit2 = new Hour(24);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Hour(18);
                        unit2 = new Hour(24);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("--")){
                    if (mod == TemporalConstituent.END){
                        unit1 = new Minute(40);
                        unit2 = new Minute(59);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Minute(20);
                        unit2 = new Minute(40);
                    } else if (mod == TemporalConstituent.START){
                        unit1 = new Minute(1);
                        unit2 = new Minute(20);
                    } else {
                        throw new NotImplementedException();
                    }
                } else
                    throw new NotImplementedException("Hour " + this.getValue() + " unknown.");
                
                VagueSimpleDate date1 = new VagueSimpleDate(unit1);
                VagueSimpleDate date2 = new VagueSimpleDate(unit2);
                
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
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    
    /**
     * Returns a real VagueDate from a two-chars representation of the Hour field
     * e.g. : AF = from 12:00 until 18:00
     * @param hour the two-chars representation of the Hour field
     * @param complete true if the returned date must be absolute (in this case a fake year
     * will be added)
     * @return the corresponding VagueDate
     */
    public static VagueDate getRealDateFromStringHour(String hour, boolean complete){
        Hour hour1;
        Hour hour2;
        try{
            if (hour.toUpperCase().equals("DT")){
                hour1 = new Hour(6);
                hour2 = new Hour(19);
            } else if (hour.toUpperCase().equals("EV")){
                hour1 = new Hour(17);
                hour2 = new Hour(23);
            } else if (hour.toUpperCase().equals("AF")){
                hour1 = new Hour(12);
                hour2 = new Hour(19);
            } else if (hour.toUpperCase().equals("MI")){
                hour1 = new Hour(11);
                hour2 = new Hour(15);
            } else if (hour.toUpperCase().equals("MO")){
                hour1 = new Hour(6);
                hour2 = new Hour(12);
            } else if (hour.toUpperCase().equals("NI")){
                hour1 = new Hour(18);
                hour2 = new Hour(24);
            } else
                throw new NotImplementedException("Hour " + hour + " unknown.");
            
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
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
        
    }
    
    
    /**
     * Comparison when at least one hour to compare is represented by
     * a String rather than an int.
     */
    private int compareSpecialHours(Hour hour1, Hour hour2){
        VagueDate date1;
        VagueDate date2;
        if (hour1.getNumber() == -1)
            date1 = getRealDateFromStringHour(hour1.hour, true);
        else
            date1 = new VagueSimpleDate(hour1).completeByForce();
        
        if (hour2.getNumber() == -1)
            date2 = getRealDateFromStringHour(hour2.hour, true);
        else
            date2 = new VagueSimpleDate(hour2).completeByForce();
        return date1.compareTo(date2);
    }
    
    
    public int compareTo(TemporalUnit other) {
        if (other instanceof Hour){
            if (this.equals(((Hour)other)))
                return TemporalRelation.EQUALS;
            else if (this.getNumber() != -1 && ((Hour)other).getNumber() != -1){
                int i = (new Integer(this.getNumber())).compareTo(new Integer(((Hour)other).getNumber()));
                if (i < 0)
                    return TemporalRelation.BEFORE;
                else if (i > 0)
                    return TemporalRelation.AFTER;
                else
                    return TemporalRelation.EQUALS;
            } else if (this.isDefined() && other.isDefined()) {
                // compare str value
                return compareSpecialHours(this, ((Hour)other));
//                String str1 = this.hour;
//                String str2 = ((Hour)other).hour;
//                if (str1.toUpperCase().equals("DT")){
//                    if (str2.toUpperCase().equals("EV"))
//                        return TemporalRelation.BEFORE;
//                    else if (str2.toUpperCase().equals("AF"))
//                        return TemporalRelation.UNDEFINED;
//                    else if (str2.toUpperCase().equals("MI"))
//                        return TemporalRelation.UNDEFINED;
//                    else if (str2.toUpperCase().equals("MO"))
//                        return TemporalRelation.UNDEFINED;
//                    else if (str2.toUpperCase().equals("NI"))
//                        return TemporalRelation.BEFORE;
//                } else if (str1.toUpperCase().equals("EV")){
//                    if (str2.toUpperCase().equals("DT"))
//                        return TemporalRelation.AFTER;
//                    else if (str2.toUpperCase().equals("AF"))
//                        return TemporalRelation.AFTER;
//                    else if (str2.toUpperCase().equals("MI"))
//                        return TemporalRelation.AFTER;
//                    else if (str2.toUpperCase().equals("MO"))
//                        return TemporalRelation.AFTER;
//                    else if (str2.toUpperCase().equals("NI"))
//                        return TemporalRelation.BEFORE;
//                } else if (str1.toUpperCase().equals("AF")){
//                    if (str2.toUpperCase().equals("DT"))
//                        return TemporalRelation.UNDEFINED;
//                    else if (str2.toUpperCase().equals("EV"))
//                        return TemporalRelation.BEFORE;
//                    else if (str2.toUpperCase().equals("MI"))
//                        return TemporalRelation.AFTER;
//                    else if (str2.toUpperCase().equals("MO"))
//                        return TemporalRelation.AFTER;
//                    else if (str2.toUpperCase().equals("NI"))
//                        return TemporalRelation.BEFORE;
//                } else if (str1.toUpperCase().equals("MI")){
//                    if (str2.toUpperCase().equals("DT"))
//                        return TemporalRelation.UNDEFINED;
//                    else if (str2.toUpperCase().equals("EV"))
//                        return TemporalRelation.BEFORE;
//                    else if (str2.toUpperCase().equals("AF"))
//                        return TemporalRelation.BEFORE;
//                    else if (str2.toUpperCase().equals("MO"))
//                        return TemporalRelation.AFTER;
//                    else if (str2.toUpperCase().equals("NI"))
//                        return TemporalRelation.BEFORE;
//                } else if (str1.toUpperCase().equals("MO")){
//                    if (str2.toUpperCase().equals("DT"))
//                        return TemporalRelation.BEFORE;
//                    else if (str2.toUpperCase().equals("EV"))
//                        return TemporalRelation.BEFORE;
//                    else if (str2.toUpperCase().equals("AF"))
//                        return TemporalRelation.BEFORE;
//                    else if (str2.toUpperCase().equals("MI"))
//                        return TemporalRelation.BEFORE;
//                    else if (str2.toUpperCase().equals("NI"))
//                        return TemporalRelation.BEFORE;
//                } else if (str1.toUpperCase().equals("NI")){
//                    if (str2.toUpperCase().equals("DT"))
//                        return TemporalRelation.AFTER;
//                    else if (str2.toUpperCase().equals("EV"))
//                        return TemporalRelation.AFTER;
//                    else if (str2.toUpperCase().equals("AF"))
//                        return TemporalRelation.AFTER;
//                    else if (str2.toUpperCase().equals("MI"))
//                        return TemporalRelation.AFTER;
//                    else if (str2.toUpperCase().equals("MO"))
//                        return TemporalRelation.AFTER;
//                }
            }
            // one of them not defined
            else
                return TemporalRelation.UNDEFINED;
        } else {
            throw new RuntimeException("Not comparable!");
        }
    }
    
    
    /**
     * Adds the specified number of hours. Returns the number of days to add after the operation.
     * @param i - the number of hours to add.
     * @return the number of days to add after the operation.
     */
    public int add(int i) {
        try{
            if (this.isDefined()){
                if (i > 0){
                    if (this.getNumber() == 24){
                        this.setNumber(1);
                        return 1 + this.add(i-1);
                    } else if (this.getNumber() == 23){
                        this.setNumber(0);
                        return 1 + this.add(i-1);
                    } else {
                        this.setNumber(this.getNumber() + 1);
                        return this.add(i-1);
                    }
                } else if (i < 0){
                    if (this.getNumber() == 0){
                        this.setNumber(23);
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
        } catch (BadFormatException e){
            e.printStackTrace();
            System.exit(-1);
            return 0;
            
        }
    }
    
    /**
     * Returns the date format value with only the date unit defined
     * @return the date format value with only the date unit defined
     */
    public String getDateValue(){
        return "---------T" + this.getValue() + "-----O---W---";
    }
}
