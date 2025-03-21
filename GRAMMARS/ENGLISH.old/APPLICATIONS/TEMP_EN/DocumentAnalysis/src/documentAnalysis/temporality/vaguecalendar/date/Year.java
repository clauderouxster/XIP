/*
 * Year.java
 *
 * Created on November 29, 2006, 10:38 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.date;

import common.NotImplementedException;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;
import documentAnalysis.temporality.vaguecalendar.duration.Centuries;
import documentAnalysis.temporality.vaguecalendar.duration.Decades;
import documentAnalysis.temporality.vaguecalendar.duration.Millenniums;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;
import documentAnalysis.temporality.vaguecalendar.duration.Years;

/**
 *
 * @author xtannier
 */
public class Year extends DateUnit{
    
    private int millennium;
    private int century;
    private int decade;
    private int year;
    private boolean beforeChrist;
    
    
    /** Creates a new instance of Year */
    Year() {
        this.millennium = -1;
        this.century = -1;
        this.decade = -1;
        this.year = -1;
        this.setBeforeChrist(false);
    }
    
    Year(String year) throws BadFormatException {
        if (year.length() != 4) {
            throw new BadFormatException(year);
        } else {
            // millennium char
            if (year.charAt(0) == '-')
                this.millennium = -1;
            else if (!year.substring(0, 1).matches("[0-9]"))
                throw new BadFormatException(year + " (millennium)");
            else
                this.millennium = Integer.parseInt(year.substring(0, 1));
            
            // century char
            if (year.charAt(1) == '-')
                this.century = -1;
            else if (!year.substring(1, 2).matches("[0-9]"))
                throw new BadFormatException(year + " (century)");
            else
                this.century = Integer.parseInt(year.substring(1, 2));
            
            // decade char
            if (year.charAt(2) == '-')
                this.decade = -1;
            else if (!year.substring(2, 3).matches("[0-9]"))
                throw new BadFormatException(year + " (decade)");
            else
                this.decade = Integer.parseInt(year.substring(2, 3));
            
            // year char
            if (year.charAt(3) == '-')
                this.year = -1;
            else if (!year.substring(3, 4).matches("[0-9]"))
                throw new BadFormatException(year + " (year)");
            else
                this.year = Integer.parseInt(year.substring(3, 4));
            
            // default if AD
            this.setBeforeChrist(false);
        }
    }
    
    
    /**
     * Gets a copy.
     * @return a copy.
     */
    public Object clone(){
        Year result = new Year();
        try {
            result.setNumber(this.getNumber());
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
        result.millennium = this.millennium;
        result.century = this.century;
        result.decade = this.decade;
        result.year = this.year;
        result.setMod(this.getMod());
        result.setBeforeChrist(this.isBeforeChrist());
        return result;
    }
    
    
    
    /**
     * Gets the duration corresponding to the unit
     *
     * @return the SimpleDuration correspond to the unit
     */
    public SimpleDuration getAssociatedDuration(){
        if (this.getIntValue() != -1) {
            try {
                SimpleDuration result = (new Years(1)).getDuration();
                result.setMod(TemporalConstituent.EXACT);
                return result;
            } catch (BadFormatException ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            }
        } else {
            if (this.year == -1) {
                try{
                    if (this.getDecade() != -1) {
                        return new Decades(1).getDuration();
                    } else if (this.getCentury() != -1) {
                        return new Centuries(1).getDuration();
                    } else if (this.getMillennium() != -1) {
                        return new Millenniums(1).getDuration();
                    } else
                        throw new NotImplementedException();
                } catch (BadFormatException e){
                    e.printStackTrace();
                    System.exit(-1);
                    return null;
                }
            } else
                throw new NotImplementedException();
        }
        
        
        
    }
    
    /**
     * Tests whether the year if before Christ.
     *
     * @return true if the year if before Christ.
     */
    public boolean isBeforeChrist() {
        return beforeChrist;
    }
    
    /**
     * Sets the year before or after Christ
     *
     * @param beforeChrist must be true if the year if before Christ, false otherwise
     */
    public void setBeforeChrist(boolean beforeChrist) {
        this.beforeChrist = beforeChrist;
    }
    
    
    
    int getMillennium() {
        return this.millennium;
    }
    
    int getCentury() {
        return this.century;
    }
    
    int getDecade() {
        return this.decade;
    }
    
    int getYear() {
        return this.year;
    }
    
    
    public int getIntValue() {
        if (this.isComplete())
            return
                this.millennium * 1000 +
                    this.century * 100 +
                    this.decade * 10 +
                    this.year;
        else
            return -1;
    }
    
    void setMillennium(int i) {
        this.millennium = i;
    }
    
    void setCentury(int i) {
        this.century = i;
    }
    
    void setDecade(int i) {
        this.decade = i;
    }
    
    void setYear(int i) {
        this.year = i;
    }
    
    
    
    
    
    public VagueDate getRealDate(boolean complete){
        try{
            int mod = this.getModMoment();
            if (this.isDefined() &&
                    mod != TemporalConstituent.UNDEFINED){
                VagueSimpleDate date1 = new VagueSimpleDate(this);
                VagueSimpleDate date2 = new VagueSimpleDate(this);
                
                // if the mod goes on the year
                if (this.getYear() != -1) {
                    Month month1;
                    Month month2;
                    if (mod == TemporalConstituent.END){
                        month1 = new Month(9);
                        month2 = new Month(12);
                    } else if (mod == TemporalConstituent.MID){
                        month1 = new Month(5);
                        month2 = new Month(9);
                    } else if (mod == TemporalConstituent.START){
                        month1 = new Month(1);
                        month2 = new Month(4);
                    } else {
                        throw new NotImplementedException();
                    }
                    
                    
                    date1.setMonth(month1);
                    date2.setMonth(month2);
                    date1.setGranularity(TemporalConstituent.MONTH);
                    date2.setGranularity(TemporalConstituent.MONTH);
                }
                // if the mod goes on the decade ("end of this decade")
                else if  (this.getDecade() != -1) {
                    String year1;
                    String year2;
                    if (mod == TemporalConstituent.END){
                        year1 = "7";
                        year2 = "9";
                    } else if (mod == TemporalConstituent.MID){
                        year1 = "3";
                        year2 = "7";
                    } else if (mod == TemporalConstituent.START){
                        year1 = "0";
                        year2 = "3";
                    } else {
                        throw new NotImplementedException();
                    }
                    
                    date1.setYear(new Year(this.getMillennium() + "" + this.getCentury() + "" + this.getDecade() + year1));
                    date2.setYear(new Year(this.getMillennium() + "" + this.getCentury() + "" + this.getDecade() + year2));
                }
                // if the mod goes on the century
                else if  (this.getCentury() != -1) {
                    String year1;
                    String year2;
                    if (mod == TemporalConstituent.END){
                        year1 = "70";
                        year2 = "99";
                    } else if (mod == TemporalConstituent.MID){
                        year1 = "30";
                        year2 = "70";
                    } else if (mod == TemporalConstituent.START){
                        year1 = "00";
                        year2 = "30";
                    } else {
                        throw new NotImplementedException();
                    }
                    
                    date1.setYear(new Year(this.getMillennium() + "" + this.getCentury() + year1));
                    date2.setYear(new Year(this.getMillennium() + "" + this.getCentury() + year2));
                }
                // if the mod goes on the millennium
                else if  (this.getMillennium() != -1) {
                    String year1;
                    String year2;
                    if (mod == TemporalConstituent.END){
                        year1 = "700";
                        year2 = "999";
                    } else if (mod == TemporalConstituent.MID){
                        year1 = "300";
                        year2 = "700";
                    } else if (mod == TemporalConstituent.START){
                        year1 = "000";
                        year2 = "300";
                    } else {
                        throw new NotImplementedException();
                    }
                    
                    date1.setYear(new Year(this.getMillennium() + year1));
                    date2.setYear(new Year(this.getMillennium() + year2));
                }
                
                
                
                if (complete){
                    date1 = date1.completeByForce().getLowerBound();
                    date2 = date2.completeByForce().getUpperBound();
                }
                try {
                    return new VagueInterval(date1, date2);
                } catch (IntervalException ex) {
                    ex.printStackTrace();
                    System.exit(1);
                    return null;
                }
                
            } else{
                VagueSimpleDate result = new VagueSimpleDate(this);
                return result;
            }
        } catch (BadFormatException ex){
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    
    public int add(int i) {
        if (this.year == -1 || i == 0)
            return 0;
        else if (i < 0) {
            if (this.year == 0) {
                if (this.decade != -1) {
                    if (this.decade == 0) {
                        if (this.century != -1) {
                            if (this.century == 0) {
                                if (this.millennium != -1) {
                                    --this.millennium;
                                }
                                this.century = 9;
                            } else {
                                --this.century;
                            }
                        }
                        this.decade = 9;
                    } else {
                        --this.decade;
                    }
                }
                this.year = 9;
            } else {
                --this.year;
            }
            this.add(i + 1);
        }
        // i > 0
        else {
            if (this.year == 9) {
                if (this.decade != -1) {
                    if (this.decade == 9) {
                        if (this.century != -1) {
                            if (this.century == 9) {
                                if (this.millennium != -1) {
                                    ++this.millennium;
                                }
                                this.century = 0;
                            } else {
                                ++this.century;
                            }
                        }
                        this.decade = 0;
                    } else {
                        ++this.decade;
                    }
                }
                this.year = 0;
            } else {
                ++this.year;
            }
            this.add(i - 1);
        }
        return 0;
    }
    
    
    /**
     * Gets the closest year from the current one, corresponding to the defined elements
     * of the specified year.
     * direction = 2 if looking for the closest in the FUTURE and for a date strictly AFTER the current one
     * direction = 1 if looking for the closest in the FUTURE and for a date AFTER, INCLUDED or EQUAL to the current one
     * direction = -2 if looking for the closest in the PAST and for a date strictly AFTER the current one
     * direction = -1 if looking for the closest in the PAST and for a date AFTER, INCLUDED or EQUAL to the current one
     * @param yearToGo the year to reach
     * @param direction the direction
     * @return the new year.
     */
    Year closest(Year yearToGo, int direction) {
        if (!this.isDefined() || yearToGo.isComplete()) {
            return yearToGo;
        } else if (!yearToGo.isDefined()) {
            return this;
        } else {
            Year newYear = (Year)this.clone();
            int addDecade = 0;
            int addCentury = 0;
            int addMillennium = 0;
            // year
            if (this.year == -1 || direction == 0)
                newYear.setYear(yearToGo.getYear());
            else {
                if (direction > 0 && this.year > yearToGo.getYear()) {
                    addDecade = 1;
                }
                if (direction < 0 && this.year < yearToGo.getYear()) {
                    addDecade = -1;
                }
                newYear.setYear(yearToGo.getYear());
            }
            // decade
            if (this.decade == -1 || direction == 0)
                newYear.setDecade(yearToGo.getDecade() + addDecade);
            else {
                if (direction > 0 && this.decade > yearToGo.getDecade()) {
                    addCentury = 1;
                }
                if (direction < 0 && this.decade < yearToGo.getDecade()) {
                    addCentury = -1;
                }
                newYear.setDecade(yearToGo.getDecade() + addDecade);
            }
            // century
            if (this.century == -1 || direction == 0)
                newYear.setCentury(yearToGo.getCentury() + addCentury);
            else {
                if (direction > 0 && this.century > yearToGo.getCentury()) {
                    addMillennium = 1;
                }
                if (direction < 0 -1 && this.century < yearToGo.getCentury()) {
                    addMillennium = -1;
                }
                newYear.setCentury(yearToGo.getCentury() + addCentury);
            }
            // millennium
            if (yearToGo.getMillennium() != -1)
                newYear.setMillennium(yearToGo.getMillennium() + addMillennium);
            
            return newYear;
        }
    }
    
    
    void completeCentury(int century){
        if (this.isCenturyMissing()){
            if (century == 19 && this.decade == 0) {
                this.millennium = 2;
                this.century = 0;
            } else if (century > 99) {
                throw new NotImplementedException();
            } else if (century >= 10) {
                this.millennium = century / 10;
                this.century = century % 10;
            } else {
                this.millennium = 0;
                this.century = century;
            }
        }
    }
    
    boolean isCenturyMissing(){
        return (this.isDefined() && this.millennium == -1 && this.century == -1);
    }
    
    /**
     * Tests if at least the millennium part of the year is defined
     *
     * @returns true whether at least the millennium part of the year is defined, false otherwise
     */
    boolean isAbsolute() {
        return this.millennium != -1;
    }
    
    /**
     * Tests if all parts of the year are defined
     *
     * @returns true whether all parts of the year are defined, false otherwise
     */
    boolean isComplete() {
        return (this.millennium != -1 &&
                this.century != -1 &&
                this.decade != -1 &&
                this.year != -1);
    }
    
    
    public boolean isDefined() {
        return (this.millennium != -1 ||
                this.century != -1 ||
                this.decade != -1 ||
                this.year != -1);  }
    
    
    
    public boolean equals(Object o){
        if (o instanceof Year){
            Year y = (Year)o;
            return this.century == y.century &&
                    this.decade == y.decade &&
                    this.millennium == y.millennium &&
                    this.year == y.year && this.getMod() == y.getMod();
        } else {
            return false;
        }
    }
    
    public String toString() {
        if (this.isDefined())
            return this.getValue();
        else
            return "";
    }
    
    
    public int getGranularity(){
        return TemporalConstituent.YEAR;
    }
    
    public String getValue() {
        String strResult = "";
        if (this.millennium == -1 &&
                this.century == -1 &&
                this.decade == -1 &&
                this.year == -1)
            return "----";
        else {
            if (this.millennium == -1)
                strResult += "-";
            else
                strResult += String.valueOf(this.millennium);
            if (this.century == -1)
                strResult += "-";
            else
                strResult += String.valueOf(this.century);
            if (this.decade == -1)
                strResult += "-";
            else
                strResult += String.valueOf(this.decade);
            if (this.year == -1)
                strResult += "-";
            else
                strResult += String.valueOf(this.year);
            
            return strResult;
        }
    }
    
    /**
     * Compares years without considering AC/AD value
     */
    private int compareYearValue(Year year) {
        if (this.isComplete() && year.isComplete()){
            if (this.getMod() == year.getMod()){
                int i = (new Integer(this.getIntValue())).compareTo(new Integer(year.getIntValue()));
                if (i < 0)
                    return TemporalRelation.BEFORE;
                else if (i > 0)
                    return TemporalRelation.AFTER;
                else
                    return TemporalRelation.EQUALS;
            } else {
                return this.compareSpecial(year);
            }
        } else {
            if (this.millennium < year.millennium)
                return TemporalRelation.BEFORE;
            else if (this.millennium > year.millennium)
                return TemporalRelation.AFTER;
            else if (this.millennium != -1 && year.millennium != -1) {
                if (this.century < year.century)
                    return TemporalRelation.BEFORE;
                else if (this.century > year.century)
                    return TemporalRelation.AFTER;
                else if (this.century != -1 && year.century != -1) {
                    if (this.decade < year.decade)
                        return TemporalRelation.BEFORE;
                    else if (this.decade > year.decade)
                        return TemporalRelation.AFTER;
                    else if (this.decade != -1 && year.decade != -1) {
                        if (this.year < year.year)
                            return TemporalRelation.BEFORE;
                        else if (this.year > year.year)
                            return TemporalRelation.AFTER;
                        else {
                            if (this.getMod() < year.getMod())
                                return TemporalRelation.BEFORE;
                            else if (this.getMod() > year.getMod())
                                return TemporalRelation.AFTER;
                            else
                                return TemporalRelation.EQUALS;
                        }
                    } else
                        return TemporalRelation.UNDEFINED;
                } else
                    return TemporalRelation.UNDEFINED;
            } else
                return TemporalRelation.UNDEFINED;
        }
    }
    
    public int compareTo(TemporalUnit o) {
        if (o instanceof Year){
            Year year = (Year)o;
            // both dates AD
            if (!this.isBeforeChrist() && !year.isBeforeChrist()) {
                return this.compareYearValue(year);
            }
            // both dates BC
            else if (this.isBeforeChrist() && year.isBeforeChrist()) {
                return TemporalRelation.getInverseRelation(this.compareYearValue(year));
            }
            // first BC and second AD
            else if (this.isBeforeChrist() && !year.isBeforeChrist()) {
                return TemporalRelation.BEFORE;
            }
            // first AD and second BC
            else if (!this.isBeforeChrist() && year.isBeforeChrist()) {
                return TemporalRelation.AFTER;
            } else
                throw new NotImplementedException();
        } else {
            throw new RuntimeException("Not comparable!");
        }
    }
    
    
    /**
     * Returns the date format value with only the date unit defined
     * @return the date format value with only the date unit defined
     */
    public String getDateValue(){
        return "-" + this.getValue() + "----T-------O---W---";
    }
    
}
