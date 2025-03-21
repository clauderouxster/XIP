/*
 * Month.java
 *
 * Created on November 29, 2006, 10:41 AM
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
import documentAnalysis.temporality.vaguecalendar.duration.Months;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;

/**
 *
 * @author xtannier
 */
public class Month extends DateUnit{
    
    private String month;
    
    
    /** Creates a new instance of Month */
    Month() {
        try {
            this.setNumber(-1);
        } catch (BadFormatException ex) {
            ex.printStackTrace();
        }
        this.month = "";
    }
    
    /**
     * Creates a new instance of Month with the specified formated two-char representation.
     * @param month the formated two-char representation of the Month
     */
    Month(String month) throws BadFormatException {
        if (month.length() != 2) {
            throw new BadFormatException(month);
        } else {
            if (month.equals("--")) {
                this.setNumber(-1);
                this.month = "";
            } else if (month.toUpperCase().equals("FA") ||
                    month.toUpperCase().equals("H1") ||
                    month.toUpperCase().equals("H2") ||
                    month.toUpperCase().equals("Q1") ||
                    month.toUpperCase().equals("Q2") ||
                    month.toUpperCase().equals("Q3") ||
                    month.toUpperCase().equals("Q4") ||
                    month.toUpperCase().equals("SP") ||
                    month.toUpperCase().equals("SU") ||
                    month.toUpperCase().equals("WI")) {
                this.month = month.toUpperCase();
                this.setNumber(-1);
            } else if (month.matches("[0-9]+")) {
                this.month = "--";
                this.setNumber(Integer.parseInt(month));
            } else {
                throw new BadFormatException(month);
            }
        }
    }
    
    /**
     * Creates a new instance of Month with the specified month number.
     */
    Month(int month) throws BadFormatException {
        this.setNumber(month);
        this.month = "--";
    }
    
    /**
     * Gets a copy.
     * @return a copy.
     */
    public Object clone() throws CloneNotSupportedException{
        Month result = new Month();
        try {
            result.setNumber(this.getNumber());
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
        result.month = this.month;
        result.setMod(this.getMod());
        return result;
    }
    
    
    /**
     * Gets the duration corresponding to the unit
     *
     * @return the SimpleDuration correspond to the unit
     */
    public SimpleDuration getAssociatedDuration(){
        SimpleDuration result = null;
        if (this.getNumber() != -1)
            result = (new Months(1)).getDuration();
        else if (month.toUpperCase().equals("FA") ||
                month.toUpperCase().equals("Q1") ||
                month.toUpperCase().equals("Q2") ||
                month.toUpperCase().equals("Q3") ||
                month.toUpperCase().equals("Q4") ||
                month.toUpperCase().equals("SP") ||
                month.toUpperCase().equals("SU") ||
                month.toUpperCase().equals("WI")) {
            result = (new Months(3)).getDuration();
        } else if (month.toUpperCase().equals("H1") ||
                month.toUpperCase().equals("H2")) {
            result = (new Months(6)).getDuration();
        } else
            throw new NotImplementedException();
        result.setMod(TemporalConstituent.EXACT);
        return result;
    }
    
    public void setNumber(int number) throws BadFormatException {
        super.setNumber(number);
        if (this.getNumber() > 12){
            throw new BadFormatException("The Month part of a date must be less than 13!");
        }
    }
    
    
    /**
     * Returns a real VagueDate from a two-chars representation of the Month field
     * or its mod
     * e.g. : Q1 = from January to March, etc...
     * @param complete true if the returned date must be absolute (in this case a fake year
     * will be added)
     * @return the corresponding VagueDate
     */
    public VagueDate getRealDate(boolean complete){
        try{
            int mod = this.getModMoment();
            if (this.isDefined() && this.getNumber() == -1){
                DateUnit unit1;
                DateUnit unit2;
                
                if (this.getValue().toUpperCase().equals("FA")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Month(9);
                        unit2 = new Month(10);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(10);
                        unit2 = new Month(11);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(11);
                        unit2 = new Month(12);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Month(9);
                        unit2 = new Month(12);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("SU")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Month(6);
                        unit2 = new Month(7);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(7);
                        unit2 = new Month(8);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(8);
                        unit2 = new Month(9);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Month(6);
                        unit2 = new Month(9);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("WI")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Month(1);
                        unit2 = new Month(2);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(1);
                        unit2 = new Month(3);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(2);
                        unit2 = new Month(3);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Month(1);
                        unit2 = new Month(3);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("SP")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Month(3);
                        unit2 = new Month(4);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(4);
                        unit2 = new Month(5);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(5);
                        unit2 = new Month(6);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Month(3);
                        unit2 = new Month(6);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("H1")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Month(1);
                        unit2 = new Month(2);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(3);
                        unit2 = new Month(4);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(5);
                        unit2 = new Month(6);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Month(1);
                        unit2 = new Month(6);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("H2")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Month(7);
                        unit2 = new Month(8);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(9);
                        unit2 = new Month(10);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(11);
                        unit2 = new Month(12);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Month(7);
                        unit2 = new Month(12);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("Q1")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Month(1);
                        unit2 = new Month(1);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(2);
                        unit2 = new Month(2);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(3);
                        unit2 = new Month(3);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Month(1);
                        unit2 = new Month(3);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("Q2")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Month(4);
                        unit2 = new Month(4);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(5);
                        unit2 = new Month(5);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(6);
                        unit2 = new Month(6);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Month(4);
                        unit2 = new Month(6);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("Q3")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Month(7);
                        unit2 = new Month(7);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(8);
                        unit2 = new Month(8);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(9);
                        unit2 = new Month(9);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Month(7);
                        unit2 = new Month(9);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("Q4")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new Month(10);
                        unit2 = new Month(10);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(11);
                        unit2 = new Month(11);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new Month(12);
                        unit2 = new Month(12);
                    } else if (mod == TemporalConstituent.UNDEFINED){
                        unit1 = new Month(10);
                        unit2 = new Month(12);
                    } else
                        throw new NotImplementedException();
                } else if (this.getValue().toUpperCase().equals("--")){
                    if (mod == TemporalConstituent.START){
                        unit1 = new DayOfMonth(1);
                        unit2 = new DayOfMonth(10);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new DayOfMonth(10);
                        unit2 = new DayOfMonth(20);
                    } else if (mod == TemporalConstituent.END){
                        unit1 = new DayOfMonth(20);
                        unit2 = new DayOfMonth(31);
                    } else
                        throw new NotImplementedException();
                } else
                    throw new NotImplementedException("Month " + this.getValue() + " unknown.");
                
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
            } else {
                VagueDate result;
                if (mod != TemporalConstituent.UNDEFINED) {
                    DayOfMonth unit1;
                    DayOfMonth unit2;
                    
                    if (mod == TemporalConstituent.START){
                        unit1 = new DayOfMonth(1);
                        unit2 = new DayOfMonth(10);
                    } else if (mod == TemporalConstituent.MID){
                        unit1 = new DayOfMonth(10);
                        unit2 = new DayOfMonth(20);
                    } else if (mod == TemporalConstituent.END){
                        unit1 = new DayOfMonth(20);
                        unit2 = new DayOfMonth(31);
                    } else {
                        throw new NotImplementedException();
                    }
                    VagueSimpleDate date1 = new VagueSimpleDate(this);
                    VagueSimpleDate date2 = new VagueSimpleDate(this);
                    date1.setDayOfMonth(unit1);
                    date2.setDayOfMonth(unit2);
                    try{
                        result = new VagueInterval(date1, date2);
                    } catch (IntervalException ex) {
                        ex.printStackTrace();
                        System.exit(-1);
                        return null;
                    }
                } else {
                    result = new VagueSimpleDate(this);
                }
                return result;
            }
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    /**
     * Adds the specified number of months. Returns the number of years to add after the operation.
     * @param i the number of months to add.
     * @return the number of years to add after the operation.
     */
    public int add(int i) {
        try{
            if (this.isDefined()) {
                if (i == 0) {
                    return 0;
                } else if (i < 0) {
                    if (this.getNumber() == 1) {
                        this.setNumber(12);
                        return this.add(i + 1) - 1;
                    } else if (this.getNumber() != -1) {
                        this.setNumber(this.getNumber() - 1);
                        return this.add(i + 1);
                    } else {
                        throw new NotImplementedException();
                    }
                }
                // i > 0
                else {
                    if (this.getNumber() == 12) {
                        this.setNumber(1);
                        return this.add(i - 1) + 1;
                    } else if (this.getNumber() != -1) {
                        this.setNumber(this.getNumber() + 1);
                        return this.add(i - 1);
                    } else {
                        throw new NotImplementedException();
                    }
                }
                
            } else {
                return 0;
            }
        } catch (BadFormatException e) {
            e.printStackTrace();
            System.exit(-1);
            return 0;
        }
    }
    
    
    
    
    public int compareTo(TemporalUnit other) {
        if (other instanceof Month){
            Month month = ((Month)other);
            if (this.equals(month))
                return TemporalRelation.EQUALS;
            else if (this.getNumber() != -1 && month.getNumber() != -1 &&
                    this.getMod() == TemporalConstituent.UNDEFINED && month.getMod() == TemporalConstituent.UNDEFINED){
                int i = (new Integer(this.getNumber())).compareTo(new Integer(month.getNumber()));
                if (i < 0)
                    return TemporalRelation.BEFORE;
                else if (i > 0)
                    return TemporalRelation.AFTER;
                else
                    return TemporalRelation.EQUALS;
            } else if (this.isDefined() && month.isDefined())
                return compareSpecial(month);
            else
                return TemporalRelation.UNDEFINED;
        } else{
            throw new RuntimeException("Not comparable!");
        }
    }
    
    
    public boolean isDefined() {
        return (this.getNumber() > 0 || this.month != "");
    }
    
    
    boolean isComplete() {
        return (this.getNumber() > 0);
    }
    
    
    public boolean equals(Object other) {
        if (other instanceof Month) {
            return this.month.equals(((Month)other).month) &&
                    this.getNumber() == ((Month)other).getNumber() &&
                    this.getMod() == ((Month)other).getMod();
        } else {
            return false;
        }
        
    }
    
    public String toString() {
        if (this.getNumber() > 0) {
            return TempConst.mapMonths.get(String.valueOf(this.getNumber()));
        } else if (!this.month.equals("")) {
            return TempConst.mapMonths.get(this.month);
        } else {
            return "";
        }
    }
    
    public int getGranularity(){
        return TemporalConstituent.MONTH;
    }
    
    public String getValue(){
        if (this.getNumber() == -1){
            if (!this.month.equals(""))
                return this.month;
            else
                return "--";
        } else {
            return CommonFunctions.formatInt(this.getNumber(), 2);
        }
    }
    
    /**
     * Returns the date format value with only the date unit defined
     * @return the date format value with only the date unit defined
     */
    public String getDateValue(){
        return "-----" + this.getValue() + "--T-------O---W---";
    }
}
