/*
 * VagueSimpleDate.java
 *
 * Created on November 29, 2006, 10:39 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.date;

import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import common.NotImplementedException;
import documentAnalysis.temporality.LostGranularityException;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.CommonFunctions;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;
import documentAnalysis.temporality.vaguecalendar.duration.Centuries;
import documentAnalysis.temporality.vaguecalendar.duration.Days;
import documentAnalysis.temporality.vaguecalendar.duration.Decades;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import documentAnalysis.temporality.vaguecalendar.duration.DurationInterval;
import documentAnalysis.temporality.vaguecalendar.duration.DurationUnit;
import documentAnalysis.temporality.vaguecalendar.duration.Hours;
import documentAnalysis.temporality.vaguecalendar.duration.Millenniums;
import documentAnalysis.temporality.vaguecalendar.duration.Minutes;
import documentAnalysis.temporality.vaguecalendar.duration.Months;
import documentAnalysis.temporality.vaguecalendar.duration.MultipleDuration;
import documentAnalysis.temporality.vaguecalendar.duration.Seconds;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;
import documentAnalysis.temporality.vaguecalendar.duration.Years;
import java.util.Calendar;
import java.util.GregorianCalendar;


/**
 * This class implements a simple date.
 * @author xtannier
 */
public class VagueSimpleDate extends VagueDate{
    
    private static int UNDEFINED = 0;
    private int granularity;
    private Year year;
    private Month month;
    private DayOfMonth dayOfMonth;
    private Hour hour;
    private Minute minute;
    private Second second;
    private DayOfWeek dayOfWeek;
    private GregorianCalendar fullDate;
    
    
//    private static int test = 0;
    
    /**
     * Creates a new instance of VagueSimpleDate
     */
    public VagueSimpleDate() {
        this.year = new Year();
        this.month = new Month();
        this.dayOfMonth = new DayOfMonth();
        this.hour = new Hour();
        this.minute = new Minute();
        this.second = new Second();
        this.dayOfWeek = new DayOfWeek();
        this.granularity = UNDEFINED;
    }
    
    
    /**
     * Creates a new instance of VagueSimpleDate
     * @param unit the XipUnit in which the date is found
     * @param node the XipNode representing the date.
     */
    public VagueSimpleDate(XipUnit unit, XipNode node) {
        this();
        this.setXipNode(unit, node);
    }
    
    
    /**
     * Creates a new instance of a full VagueSimpleDate with a year, month and day
     * @param year the year
     * @param month the month
     * @param dayOfMonth the day
     */
    public VagueSimpleDate(int year, int month, int dayOfMonth){
        this();
        String strYear = String.valueOf(year);
        String strMonth = String.valueOf(month);
        String strDay = String.valueOf(dayOfMonth);
        for (int i = strYear.length() ; i < 4 ; ++i)
            strYear = "0" + strYear;
        for (int i = strMonth.length() ; i < 2 ; ++i)
            strMonth = "0" + strMonth;
        for (int i = strDay.length() ; i < 2 ; ++i)
            strDay = "0" + strDay;
        try {
            this.copy(new VagueSimpleDate("+" + strYear +
                    strMonth +
                    strDay +
                    "T-------O---W---"));
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
    }
    
    
    /**
     * Creates a new instance of a full VagueSimpleDate with a year, month and day
     * @param year the year
     * @param month the month
     * @param dayOfMonth the day
     * @param unit the XipUnit in which the date is found
     * @param node the XipNode representing the date.
     */
    public VagueSimpleDate(int year, int month, int dayOfMonth, XipUnit unit, XipNode node){
        this(year, month, dayOfMonth);
        this.setXipNode(unit, node);
    }
    
    
    /**
     * Creates a new instance of a full VagueSimpleDate from a GregorianCalendar date
     *
     * @param fullDate the GregorianCalendar date
     */
    public VagueSimpleDate(GregorianCalendar fullDate){
        this(fullDate.get(Calendar.YEAR), fullDate.get(Calendar.MONTH) + 1, fullDate.get(Calendar.DAY_OF_MONTH));
        try {
            this.hour = new Hour(fullDate.get(Calendar.HOUR));
            this.minute = new Minute(fullDate.get(Calendar.MINUTE));
            this.second = new Second(fullDate.get(Calendar.SECOND));
            this.setGranularity(TemporalConstituent.SECOND);
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
        
        this.getFullDate();
    }
    
    /**
     * Creates a new instance of a VagueSimpleDate from a formated String
     * @param dateFormat the String representation of the date
     * @throws documentanalysis.temporality.vaguecalendar.BadFormatException if the specified
     * format is not well formed.
     */
    public VagueSimpleDate(String dateFormat) throws BadFormatException {
        //+yyyymmddThhmmss-OdddWwwd
//        ++test;
//        System.err.println("test = " + test );
        
        if (!isDateFormat(dateFormat))
            throw new BadFormatException(dateFormat);
        
        // year
        this.year = new Year(dateFormat.substring(1, 5));
        
        // AC, BC
        if (dateFormat.charAt(0) == '=')
            this.year.setBeforeChrist(true);
        
        // month
        this.month = new Month(dateFormat.substring(5, 7));
        // day of month
        this.dayOfMonth = new DayOfMonth(dateFormat.substring(7, 9));
        
        // hours
        this.hour = new Hour(dateFormat.substring(10, 12));
        // minutes
        this.minute = new Minute(dateFormat.substring(12, 14));
        // seconds
        this.second = new Second(dateFormat.substring(14, 16));
        
        // day of week
        this.dayOfWeek = new DayOfWeek(dateFormat.substring(24, 25));
        
        // full date
        this.getFullDate();
        
        // if the hour is defined and not the minutes
        // minutes are 00
        // 18 => 18:00
        // if we don't precise this, 18:30 will be included into 18, instead of BEFORE
        if (this.hour.isDefined() && this.hour.getIntValue() != -1 && !this.minute.isDefined())
            this.minute = new Minute(0);
        
        if (this.second.isDefined())
            this.granularity = SECOND;
        else if (this.minute.isDefined())
            this.granularity = MINUTE;
        else if (this.hour.isDefined())
            this.granularity = HOUR;
        else if (this.dayOfMonth.isDefined())
            this.granularity = DAY;
        else if (this.dayOfWeek.isDefined())
            this.granularity = DAY;
        else if (this.month.isDefined())
            this.granularity = MONTH;
        else if (this.year.isDefined())
            this.granularity = YEAR;
    }
    
    
    /**
     * Creates a new instance of a VagueSimpleDate from a formated String
     * @param dateFormat the String representation of the date
     * @param unit the XipUnit in which the date is found
     * @param node the XipNode representing the date.
     * @throws documentanalysis.temporality.vaguecalendar.BadFormatException if the specified
     * format is not well formed.
     */
    public VagueSimpleDate(String dateFormat, XipUnit unit, XipNode node) throws BadFormatException {
        this(dateFormat);
        this.setXipNode(unit, node);
    }
    
    
    /**
     * Creates a new instance of a VagueSimpleDate from a copy a a specified VagueSimpleDate
     * Remember that a copy does not keep the XipNode information
     *
     * @param date the VagueSimpleDate to copy
     */
    public VagueSimpleDate(VagueSimpleDate date) throws BadFormatException {
        this.copy(date);
    }
    
    
    /**
     * Creates a new instance of VagueSimpleDate with only the specified date unit
     * defined.
     * @param unit the only defined unit
     */
    public VagueSimpleDate(DateUnit unit){
        this();
        try {
            this.copy(new VagueSimpleDate(unit.getDateValue()));
            if (unit.isDefined())
                this.granularity = unit.getGranularity();
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
    }
    
    
    /**
     * Creates a new instance of VagueSimpleDate with only the specified date unit
     * defined.
     * @param dateUnit the only defined unit
     * @param unit the XipUnit in which the date is found
     * @param node the XipNode representing the date.
     */
    public VagueSimpleDate(DateUnit dateUnit, XipUnit unit, XipNode node){
        this(dateUnit);
        this.setXipNode(unit, node);
    }
    
    
    /**
     * Gets a clone.
     * @return a clone.
     */
    public Object clone() throws CloneNotSupportedException {
        VagueSimpleDate result = new VagueSimpleDate();
        result.year = (Year)this.year.clone();
        result.month = (Month)this.month.clone();
        result.dayOfMonth = (DayOfMonth)this.dayOfMonth.clone();
        result.hour = (Hour)this.hour.clone();
        result.minute = (Minute)this.minute.clone();
        result.second = (Second)this.second.clone();
        result.dayOfWeek = (DayOfWeek)this.dayOfWeek.clone();
        result.granularity = this.granularity;
        result.setMod(this.getMod());
        if (this.fullDate != null)
            result.fullDate = (GregorianCalendar)this.fullDate.clone();
        result.setXipNode(this.getXipUnit(), this.getXipNode());
        return result;
    }
    
    
    /**
     * Copy a specified date
     * @param date the VagueSimpleDate to copy
     */
    public void copy(VagueSimpleDate date) {
        try{
            this.year = (Year)date.year.clone();
            this.month = (Month)date.month.clone();
            this.dayOfMonth = (DayOfMonth)date.dayOfMonth.clone();
            this.hour = (Hour)date.hour.clone();
            this.minute = (Minute)date.minute.clone();
            this.second = (Second)date.second.clone();
            this.dayOfWeek = (DayOfWeek)date.dayOfWeek.clone();
            this.granularity = date.granularity;
            if (date.fullDate != null)
                this.fullDate = (GregorianCalendar)date.fullDate.clone();
        } catch (CloneNotSupportedException ex){
            ex.printStackTrace();
            System.exit(-1);
        }
    }
    
    public void checkFullDate() {
        this.getFullDate();
    }
    
    
    GregorianCalendar getFullDate() {
        if (this.year.isComplete() &&
                this.month.isComplete() &&
                this.dayOfMonth.isComplete()) {
            this.fullDate = new GregorianCalendar();
            
            this.fullDate.set(this.year.getIntValue(),
                    this.month.getIntValue() - 1,
                    this.dayOfMonth.getIntValue());
            this.dayOfMonth.setLimit(this.fullDate.getActualMaximum(Calendar.DAY_OF_MONTH));
            try {
                this.dayOfWeek = new DayOfWeek(this.fullDate.get(Calendar.DAY_OF_WEEK) - 1);
            } catch(BadFormatException e) {
                e.printStackTrace();
            }
            if (this.hour.isDefined() && this.hour.getIntValue() >= 0)
                this.fullDate.set(Calendar.HOUR_OF_DAY, this.hour.getIntValue());
            if (this.minute.isDefined())
                this.fullDate.set(Calendar.MINUTE, this.minute.getIntValue());
            if (this.second.isDefined())
                this.fullDate.set(Calendar.SECOND, this.second.getIntValue());
        } else
            this.fullDate = null;
        return this.fullDate;
    }
    
    /**
     * Returns the date year.
     * @return the date year.
     */
    public Year getYear() {
        if (this.hasGranularity(YEAR)){
            Year result = this.year;
            if (this.granularity == TemporalConstituent.YEAR)
                result.setMod(this.getMod());
            return result;
        } else
            return new Year();
    }
    
    /**
     * Returns the date month.
     * @return the date month.
     */
    public Month getMonth() {
        if (this.hasGranularity(MONTH)){
            Month result = this.month;
            if (this.granularity == TemporalConstituent.MONTH)
                result.setMod(this.getMod());
            return result;
        } else
            return new Month();
    }
    
    /**
     * Returns the date day of month.
     * @return the date day of month.
     */
    public DayOfMonth getDayOfMonth() {
        if (this.hasGranularity(DAY)){
            DayOfMonth result = this.dayOfMonth;
            if (this.granularity == TemporalConstituent.DAY)
                result.setMod(this.getMod());
            return result;
        } else
            return new DayOfMonth();
    }
    
    
    /**
     * Returns the date day of week.
     * @return the date day of week.
     */
    public DayOfWeek getDayOfWeek() {
        if (this.hasGranularity(DAY)){
            DayOfWeek result = this.dayOfWeek;
            if (this.granularity == TemporalConstituent.DAY)
                result.setMod(this.getMod());
            return result;
        } else
            return new DayOfWeek();
    }
    
    /**
     * Returns the date hours.
     * @return the date hours.
     */
    public Hour getHour() {
        if (this.hasGranularity(HOUR)){
            Hour result = this.hour;
            
            if (this.granularity == TemporalConstituent.HOUR)
                result.setMod(this.getMod());
            return result;
        } else
            return new Hour();
    }
    
    /**
     * Returns the date minutes.
     * @return the date minutes.
     */
    public Minute getMinute() {
        if (this.hasGranularity(MINUTE)){
            Minute result = this.minute;
            if (this.granularity == TemporalConstituent.MINUTE)
                result.setMod(this.getMod());
            return result;
        } else
            return new Minute();
    }
    
    
    /**
     * Returns the date seconds.
     * @return the date seconds.
     */
    public Second getSecond() {
        if (this.hasGranularity(SECOND)){
            Second result = this.second;
            if (this.granularity == TemporalConstituent.SECOND)
                result.setMod(this.getMod());
            return result;
        } else {
            return new Second();
        }
    }
    
    
    
    /**
     * Returns the date granularity.
     * @return the date granularity.
     */
    public int getGranularity() {
        return this.granularity;
    }
    
    
    /**
     * Gets the Duration corresponding to the granularity of the date.
     * eg:
     * <ol><li>2006 => 1 year
     *     <li>September => 1 month
     *     <li>Winter    => X months
     *     <li>morning   => X hours
     * </ol>
     * @return the Duration corresponding to the granularity of the date.
     */
    public SimpleDuration getGranularityDuration(){
        try {
            int granularity = this.granularity;
            if (this.getDateUnit(granularity).getIntValue() == -1)
                return new SimpleDuration(DurationUnit.getDurationUnit(granularity, "X"));
            else
                return new SimpleDuration(DurationUnit.getDurationUnit(granularity, "1"));
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    /**
     * Returns the DateUnit corresponding to the specified
     * granularity in the date.
     * @param granularity the expected granularity
     * @return the DateUnit corresponding to the specified
     * granularity in the date.
     */
    public DateUnit getDateUnit(int granularity){
        DateUnit result;
        if (granularity == YEAR)
            result = this.getYear();
        else if (granularity == MONTH)
            result = this.getMonth();
        else if (granularity == DAY || granularity == Calendar.DAY_OF_MONTH)
            result = this.getDayOfMonth();
        else if (granularity == Calendar.DAY_OF_WEEK)
            result = this.getDayOfWeek();
        else if (granularity == HOUR)
            result = this.getHour();
        else if (granularity == MINUTE)
            result = this.getMinute();
        else if (granularity == SECOND)
            result = this.getSecond();
        else
            throw new NotImplementedException("Unknown granularity " + granularity);
        if (this.granularity == granularity)
            result.setMod(this.getMod());
        return result;
    }
    
    
    /**
     * Sets the year
     * @param year the year
     */
    public void setYear(Year year) {
        this.year = year;
        // full date
        this.getFullDate();
        if (!this.hasGranularity(YEAR))
            this.setGranularity(YEAR);
    }
    
    /**
     * Sets the month
     * @param month the month
     */
    public void setMonth(Month month) {
        this.month = month;
        // full date
        this.getFullDate();
        if (!this.hasGranularity(MONTH))
            this.setGranularity(MONTH);
    }
    
    /**
     * Sets the day of the month
     * @param dayOfMonth the day of the month
     */
    public void setDayOfMonth(DayOfMonth dayOfMonth) {
        this.dayOfMonth = dayOfMonth;
        // full date
        this.getFullDate();
        if (!this.hasGranularity(DAY))
            this.setGranularity(DAY);
    }
    
    /**
     * Sets the hour
     * @param hour the hour
     */
    public void setHour(Hour hour) {
        this.hour = hour;
        if (!this.hasGranularity(HOUR))
            this.setGranularity(HOUR);
    }
    
    /**
     * Sets the minute
     * @param minute the minute
     */
    public void setMinute(Minute minute) {
        this.minute = minute;
        if (!this.hasGranularity(MINUTE))
            this.setGranularity(MINUTE);
    }
    
    /**
     * Sets the second
     * @param second the second
     */
    public void setSecond(Second second) {
        this.second = second;
        if (!this.hasGranularity(SECOND))
            this.setGranularity(SECOND);
    }
    
    /**
     * Sets the day of the week
     * @param dayOfWeek the day of the week
     */
    public void setDayOfWeek(DayOfWeek dayOfWeek) {
        this.dayOfWeek = dayOfWeek;
        if (!this.hasGranularity(DAY))
            this.setGranularity(DAY);
    }
    
    /**
     * Sets a new granularity
     * @param i the new granularity value
     */
    public void setGranularity(int i) {
        this.granularity = i;
    }
    
    
    /**
     * Adds a specified number of years to the date.
     * @param i the number of years to add
     * @return the new VagueDate.
     */
    public VagueSimpleDate addYear(int i) {
        this.year.add(i);
        this.setYear(this.year);
        return this;
    }
    
    /**
     * Adds a specified number of months to the date.
     * @param i the number of months to add
     * @return the new VagueDate.
     */
    public VagueSimpleDate addMonth(int i) {
        if (this.getMonth().isDefined()) {
            this.addYear(this.month.add(i));
        }
        
        return this;
    }
    
    
    /**
     *
     * @param field
     * @param amount
     *
     * @return the resulting date
     */
    public VagueDate add(int field, int amount) {
        VagueSimpleDate result = null;
        try {
            result = (VagueSimpleDate) this.clone();
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
        }
        if (amount != 0){
            if (result.granularity >= field){
                if (result.fullDate == null) {
                    int deduct = 0;
                    // seconds
                    if (field == Calendar.SECOND){
                        deduct = result.second.add(amount);
                        amount = 0;
                    }
                    // minutes
                    if (field == Calendar.MINUTE || deduct != 0){
                        deduct = result.minute.add(amount + deduct);
                        amount = 0;
                    }
                    // hours
                    if (field == Calendar.HOUR || deduct != 0){
                        deduct = result.hour.add(amount + deduct);
                        amount = 0;
                    }
                    // days
                    if (field == Calendar.DAY_OF_MONTH || field == Calendar.DAY_OF_WEEK || deduct != 0){
                        try{
                            deduct = result.dayOfMonth.add(amount) + deduct;
                            amount = 0;
                        } catch (LostGranularityException ex){
                            field = Calendar.MONTH;
                            result.granularity = MONTH;
                            amount = ex.getRemaining() / TemporalConstituent.getApproximateMaxNumber(DAY);
                        }
                    }
                    // months
                    if (field == Calendar.MONTH || deduct != 0){
                        if (!result.getMonth().isDefined()) {
                            VagueDate result2 = VagueDate.getNewDate(result.getXipUnit(),
                                    result.getXipNode());
                            return result2;
                        } else if (result.getMonth().getNumber() != -1) {
                            deduct = result.month.add(amount + deduct);
                        } else {
                            VagueDate result2 = result.getRealDate();
                            result2 = result2.add(field, amount);
                            return result2;
                        }
                        amount = 0;
                    }
                    // years
                    if (field == Calendar.YEAR || deduct != 0)
                        result.year.add(amount + deduct);
                    this.getFullDate();
                } else {
                    int granularity = result.granularity;
                    result.fullDate.add(field, amount);
                    result.copy(fullDateToVagueDate(result.fullDate));
                    result.setGranularity(granularity);
                }
            } else {
                amount = amount / TemporalConstituent.getApproximateMaxNumber(field);
                field = TemporalConstituent.getBiggerGranularity(field);
                return result.add(field, amount);
            }
        }
        return result;
    }
    
    
    
    /**
     * Merges two dates and keep the only date information compatible with both.
     *
     * @return a new date
     * @param date the date to merge with the current one
     * @throws documentanalysis.temporality.TemporalConsistencyException if a mergind is impossible
     */
    public VagueDate merge(VagueDate date) throws TemporalConsistencyException {
        VagueDate thisCopy = this;
        try{
            if (date instanceof VagueSimpleDate || date instanceof VagueInterval){
                
                int relation = thisCopy.compareTo(date);
                if (relation == TemporalRelation.INCLUDES)
                    return date;
                else if (relation == TemporalRelation.EQUALS ||
                        relation == TemporalRelation.DURING)
                    return thisCopy;
                else if (relation == TemporalRelation.OVERLAPS)
                    // if dates are not related, compose a multiple date with "OR"
                    // should be refined if necessary
                    //    should try to find the intersecting interval for example.
                    return VagueDate.getNewDate(thisCopy.getValue() + "|" + date.getValue());
//                throw new NotImplementedException("Overlaping between two \"during\" dates," +
//                        "should try to find the intersecting interval");
                else if (relation == TemporalRelation.UNDEFINED)
                    // UNDEFINED can happen if one date is not fully specified (not absolute)
                    return null;
                else {
                    // if dates are not related, compose a multiple date with "OR"
//                return VagueDate.getNewDate(thisCopy.getValue() + "|" + date.getValue());
                    throw new TemporalConsistencyException(thisCopy, date);
                }
            } else if (date instanceof VagueMultipleDate){
                return date.merge(thisCopy);
            } else
                throw new NotImplementedException();
        } catch (BadFormatException e) {
            e.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    /**
     * Returns a full VagueSimpleDate from a Calendar date
     * @param date the Calendar date
     * @return a full VagueSimpleDate from a Calendar date
     */
    public static VagueSimpleDate fullDateToVagueDate(Calendar date) {
        VagueSimpleDate newDate = null;
        try {
            String format = "";
            // BC or AD
            if (date.get(GregorianCalendar.ERA) == GregorianCalendar.BC)
                format = "=";
            else
                format = "+";
            format +=
                    CommonFunctions.formatInt(date.get(Calendar.YEAR), 4) +
                    CommonFunctions.formatInt(date.get(Calendar.MONTH) + 1, 2) +
                    CommonFunctions.formatInt(date.get(Calendar.DAY_OF_MONTH), 2) +
                    "T" +
                    CommonFunctions.formatInt(date.get(Calendar.HOUR_OF_DAY), 2) +
                    CommonFunctions.formatInt(date.get(Calendar.MINUTE), 2) +
                    CommonFunctions.formatInt(date.get(Calendar.SECOND), 2) +
                    "-O" +
                    CommonFunctions.formatInt(date.get(Calendar.DAY_OF_YEAR), 3) +
                    "W" +
                    CommonFunctions.formatInt(date.get(Calendar.WEEK_OF_YEAR), 2) +
                    CommonFunctions.formatInt(date.get(Calendar.DAY_OF_WEEK) - 1, 1);
            newDate = new VagueSimpleDate(format);
        } catch(BadFormatException e) {
            e.printStackTrace();
        }
        return newDate;
    }
    
    
    public VagueDate closest(VagueDate vagueDateToGo, int direction) {
        if (vagueDateToGo instanceof VagueSimpleDate){
            try{
                VagueSimpleDate dateToGo = (VagueSimpleDate)vagueDateToGo;
                VagueDate vagueDateToGo2 = dateToGo.getRealDate();
                if (!(vagueDateToGo2 instanceof VagueSimpleDate))
                    return this.closest(vagueDateToGo2, direction);
                
                VagueSimpleDate newDate = new VagueSimpleDate();
                newDate.copy(this);
                if (dateToGo.isAbsolute())
                    return dateToGo;
                if (this.getMonth().isDefined() && dateToGo.getMonth().equals(this.getMonth()) &&
                        this.getDayOfMonth().isDefined() &&
                        dateToGo.getDayOfMonth().equals(this.getDayOfMonth()) &&
                        dateToGo.getGranularity() == DAY){
                    newDate.setGranularity(DAY);
                    return newDate;
                }
                
                int newDirection = direction;
                
                // year
                if (dateToGo.getYear().isDefined()) {
                    if (newDate.year.isDefined())
                        newDate.setYear(newDate.year.closest(dateToGo.getYear(), newDirection));
                    else
                        newDate.setYear((Year)dateToGo.getYear().clone());
                    newDate.setGranularity(YEAR);
                    newDirection = 0;
                }
                
                // month
                if (dateToGo.getMonth().isDefined()) {
                    if (this.month.isDefined()){
                        if (newDirection == -2 && newDate.month.compareTo(dateToGo.getMonth()) != TemporalRelation.AFTER){
                            newDate.addYear(-1);
                            newDirection = 0;
                        } else if (newDirection == -1 && newDate.month.compareTo(dateToGo.getMonth()) == TemporalRelation.BEFORE){
                            newDate.addYear(-1);
                            newDirection = 0;
                        } else if (newDirection == 2 && newDate.month.compareTo(dateToGo.getMonth()) != TemporalRelation.BEFORE){
                            newDate.addYear(1);
                            newDirection = 0;
                        } else if (newDirection == 1 && newDate.month.compareTo(dateToGo.getMonth()) == TemporalRelation.AFTER){
                            newDate.addYear(1);
                            newDirection = 0;
                        }
                    }
                    
                    if (!newDate.getMonth().equals(dateToGo.getMonth())) {
                        newDate.setMonth((Month)dateToGo.getMonth().clone());
                        newDirection = 0;
                    }
                    newDate.setGranularity(MONTH);
//                    newDirection = 0;
                    
                    // the following is useful for the following example :
                    // direction == -2
                    // date = December 29, 1987
                    // dateToGo = January 29
                    // once we set the month, we have January 29, 1987 (good one)
                    // and we must not go before again
                    // -2 ==> -1
                    if (java.lang.Math.abs(newDirection) == 2)
                        newDirection = newDirection / 2;
                    
                }
                
                
                // day of month
                if (dateToGo.getDayOfMonth().isDefined()) {
                    if (newDate.dayOfMonth.isDefined()){
                        if (newDirection == -1 && newDate.dayOfMonth.compareTo(dateToGo.getDayOfMonth()) == TemporalRelation.BEFORE) {
                            newDate.addMonth(-1);
                            newDirection = 0;
                        } else if (newDirection == 1 && newDate.dayOfMonth.compareTo(dateToGo.getDayOfMonth()) == TemporalRelation.AFTER) {
                            newDate.addMonth(1);
                            newDirection = 0;
                        } else if (newDirection == -2 && newDate.dayOfMonth.compareTo(dateToGo.getDayOfMonth()) != TemporalRelation.AFTER){
                            newDate.addMonth(-1);
                            newDirection = 0;
                        } else if (newDirection == 2 && newDate.dayOfMonth.compareTo(dateToGo.getDayOfMonth()) != TemporalRelation.BEFORE){
                            newDate.addMonth(1);
                            newDirection = 0;
                        }
                    }
                    
                    if (!newDate.getDayOfMonth().equals(dateToGo.getDayOfMonth())) {
                        newDate.setDayOfMonth((DayOfMonth)dateToGo.getDayOfMonth().clone());
                        newDirection = 0;
                    }
                    newDate.setGranularity(DAY);
//                    newDirection = 0;
                    
                    // the following is useful for the following example :
                    // direction == -2
                    // date = December 29, 1987
                    // dateToGo = January 29
                    // once we set the month, we have January 29, 1987 (good one)
                    // and we must not go before again
                    // -2 ==> -1
                    if (java.lang.Math.abs(newDirection) == 2)
                        newDirection = newDirection / 2;
                    
                    
                    
                    // check new date
                    //  if the month does not correspond to the datetogo month
                    //  re-run
                    if (dateToGo.getMonth().isDefined() && !dateToGo.getMonth().equals(newDate.getMonth())) {
                        newDate = newDate.closest(dateToGo, direction).getLowerBound();
                    }
                    //  if the new date has a full date
                    //   and if this full date is not compatible with the day of the week to go
                    //   re-run
                    else if (dateToGo.getDayOfWeek().isDefined() &&
                            dateToGo.getDayOfWeek().getIntValue() != -1){
                        GregorianCalendar fullDate = newDate.getFullDate();
                        if (fullDate != null && !(fullDate.get(Calendar.DAY_OF_WEEK) - 1 == dateToGo.getDayOfWeek().getIntValue() ||
                                (fullDate.get(Calendar.DAY_OF_WEEK) == 1 && dateToGo.getDayOfWeek().getIntValue() == 7))) {
                            if (direction > 0) {
                                newDate = newDate.add(DAY, 1).getUpperBound();
                                newDate = newDate.closest(dateToGo, direction).getLowerBound();
                            } else if (direction < 0) {
                                SimpleDuration duration = new SimpleDuration(new Days(1));
                                duration.setMod(EXACT);
                                newDate = newDate.substractSimpleDuration(duration).getLowerBound();
                                newDate = newDate.closest(dateToGo, direction).getLowerBound();
                            }
                            
                        }
                    }
                }
                
                
                // day of the week
                if (dateToGo.getDayOfWeek().isDefined()) {
                    
                    if (newDate.getFullDate() != null && !dateToGo.getDayOfWeek().isWeekEnd()) {
                        int delta;
                        int fullDateDayOfWeek = newDate.getFullDate().get(Calendar.DAY_OF_WEEK) - 1;
                        if (fullDateDayOfWeek == 0)
                            fullDateDayOfWeek = 7;
                        if (newDirection > 0)
                            delta = (7 + (dateToGo.getDayOfWeek().getIntValue() - fullDateDayOfWeek)) % 7;
                        else if (newDirection < 0)
                            delta = (7 - (dateToGo.getDayOfWeek().getIntValue() - fullDateDayOfWeek)) % 7;
                        else
                            delta = 0;
                        
                        if (delta == 0 && java.lang.Math.abs(newDirection) > 1)
                            delta = 7;
                        
                        if (newDirection != 0)
                            newDate = newDate.add(Calendar.DAY_OF_MONTH, (newDirection / java.lang.Math.abs(newDirection)) * delta).getUpperBound();
                    } else {
                        if (newDate.dayOfWeek.isDefined() && newDate.month.isComplete() && newDate.dayOfMonth.isComplete()) {
                            // potential errors with February 29, but do
                            //   what we can
                            GregorianCalendar auxFalseDate = new GregorianCalendar(1999,
                                    newDate.month.getIntValue() - 1,
                                    newDate.dayOfMonth.getIntValue());
                            int delta;
                            if (newDirection > 0)
                                delta = (7 + (dateToGo.getDayOfWeek().getIntValue() - newDate.dayOfWeek.getIntValue())) % 7;
                            else if (newDirection < 0)
                                delta = (dateToGo.getDayOfWeek().getIntValue() - newDate.dayOfWeek.getIntValue()) % 7;
                            else
                                delta = 0;
                            if (delta == 0 && java.lang.Math.abs(newDirection) > 1)
                                delta = 7;
                            
                            if (newDirection != 0){
                                delta = (newDirection / java.lang.Math.abs(newDirection)) * delta;
                                auxFalseDate.add(Calendar.DAY_OF_MONTH, delta);
                            }
                            newDate.copy(fullDateToVagueDate(auxFalseDate));
                            newDate.setYear(new Year());
                        }
                    }
                    
                    if (!newDate.getDayOfWeek().equals(dateToGo.getDayOfWeek())){
                        newDate.setDayOfWeek((DayOfWeek)dateToGo.getDayOfWeek().clone());
                        newDirection = 0;
                    }
                    newDate.setGranularity(DAY);
                    
                }
                
                
                // hour
                if (dateToGo.getHour().isDefined()) {
                    if (newDate.hour.isDefined()){
                        if (newDirection == -1 && newDate.hour.compareTo(dateToGo.getHour()) == TemporalRelation.BEFORE)
                            newDate = newDate.add(DAY, -1).getLowerBound();
                        else if (newDirection == 1 && newDate.hour.compareTo(dateToGo.getHour()) == TemporalRelation.AFTER)
                            newDate = newDate.add(DAY, 1).getUpperBound();
                        if (newDirection == -2 && newDate.hour.compareTo(dateToGo.getHour()) != TemporalRelation.AFTER)
                            newDate = newDate.add(DAY, -1).getLowerBound();
                        else if (newDirection == 2 && newDate.hour.compareTo(dateToGo.getHour()) != TemporalRelation.BEFORE)
                            newDate = newDate.add(DAY, 1).getUpperBound();
                    }
                    
                    if (!newDate.getHour().equals(dateToGo.getHour())) {
                        newDate.setHour((Hour)dateToGo.getHour().clone());
                        newDirection = 0;
                    }
                    newDate.setGranularity(HOUR);
                    
                }
                
                // minute
                if (dateToGo.getMinute().isDefined()) {
                    if (newDate.minute.isDefined()){
                        if (newDirection == -1 && newDate.minute.compareTo(dateToGo.getMinute()) == TemporalRelation.BEFORE)
                            newDate = newDate.add(HOUR, -1).getLowerBound();
                        else if (newDirection == 1 && newDate.minute.compareTo(dateToGo.getMinute()) == TemporalRelation.AFTER)
                            newDate = newDate.add(HOUR, 1).getUpperBound();
                        if (newDirection == -2 && newDate.minute.compareTo(dateToGo.getMinute()) != TemporalRelation.AFTER)
                            newDate = newDate.add(HOUR, -1).getLowerBound();
                        else if (newDirection == 2 && newDate.minute.compareTo(dateToGo.getMinute()) != TemporalRelation.BEFORE)
                            newDate = newDate.add(HOUR, 1).getUpperBound();
                    }
                    
                    if (!newDate.getMinute().equals(dateToGo.getMinute())) {
                        newDate.setMinute((Minute)dateToGo.getMinute().clone());
                        newDirection = 0;
                    }
                    newDate.setGranularity(MINUTE);
                    
                }
                
                return newDate;
            } catch(CloneNotSupportedException e) {
                e.printStackTrace();
                System.exit(-1);
                return null;
            }
        } else if (vagueDateToGo instanceof VagueInterval){
            return ((VagueInterval)vagueDateToGo).getClosestFrom(this, direction);
        } else if (vagueDateToGo instanceof VagueMultipleDate){
            return ((VagueMultipleDate)vagueDateToGo).getClosestFrom(this, direction);
        } else {
            throw new NotImplementedException();
        }
    }
    
    
    
    /**
     * Gets the closest date from the specified one, corresponding to the defined elements
     * of the current one.
     * direction = 2 if looking for the closest in the FUTURE and for a date strictly AFTER the current one
     * direction = 1 if looking for the closest in the FUTURE and for a date AFTER, INCLUDED or EQUAL to the current one
     * direction = -2 if looking for the closest in the PAST and for a date strictly AFTER the current one
     * direction = -1 if looking for the closest in the PAST and for a date AFTER, INCLUDED or EQUAL to the current one
     * @param dateFrom the date from which starting
     * @param direction the direction
     * @return the new date.
     * @see #closest(VagueDate dateToGo, int direction)
     */
    public VagueDate getClosestFrom(VagueDate dateFrom, int direction){
        return dateFrom.closest(this, direction);
    }
    
    
    /**
     * Returns an integer value corresponding to the hundreds of the date's year
     * eg : if the date is 2007, returns 20 (not the century 21 !!!)
     *
     * @return an integer value corresponding to the hundreds of the date's year,
     *  -1 if the century is not defined
     */
    public int getCentury(){
        if (this.isCenturyMissing())
            return -1;
        else {
            int result = this.year.getMillennium() * 10 + this.year.getCentury();
            if (result < 0)
                return -1;
            else
                return result;
        }
    }
    
    /**
     * Completes the date with the century if it is missing. For example,
     * when only the decade is given (60s...). DO NOT update the current date but returns
     * the result.
     * @param century the century to complete with
     * @return the completed date.
     */
    public VagueDate completeCentury(int century){
        try {
            VagueSimpleDate result = (VagueSimpleDate) this.clone();
            result.year.completeCentury(century);
            result.getFullDate();
            return result;
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    /**
     * Replaces elements of the current date that are lower than current granularity
     * by some fake elements
     * DO NOT update the current date but returns the result.
     * @return the completed date. The completed date is absolute.
     */
    public VagueDate completeByForce(){
        try {
            VagueSimpleDate result = (VagueSimpleDate)this.clone();
            
            if (!this.second.isDefined() && this.hasGranularity(SECOND))
                result.second = new Second(0);
            if (!this.minute.isDefined() && this.hasGranularity(MINUTE))
                result.minute = new Minute(0);
            if (!this.hour.isDefined() && this.hasGranularity(HOUR))
                result.hour = new Hour(0);
            if (!this.dayOfMonth.isDefined() && this.hasGranularity(DAY))
                result.dayOfMonth = new DayOfMonth(1);
            if (!this.month.isDefined() && this.hasGranularity(MONTH))
                result.month = new Month(1);
            if (!this.year.isDefined()){
                try{
                    result.year = new Year("2000");
                } catch (BadFormatException ex){
                    ex.printStackTrace();
                    System.exit(-1);
                    return null;
                }
            }
            return result;
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    /**
     * Completes the missing elements in the date, using the elements of
     * the specified date. DO NOT update the current date but returns
     * the result.
     * @param date the date to use in order to complete the current one.
     * @return the completed date.
     */
    public VagueDate completeDate(VagueDate date){
        try {
            if (!this.isDefined())
                return date;
            else if (date instanceof VagueSimpleDate){
                VagueSimpleDate result = (VagueSimpleDate)this.clone();
                VagueSimpleDate simpleDate = (VagueSimpleDate)date;
                if (granularity >= SECOND && !result.second.isDefined())
                    result.second = (Second)simpleDate.second.clone();
                if (granularity >= MINUTE && !result.minute.isDefined())
                    result.minute = (Minute)simpleDate.minute.clone();
                if (granularity >= HOUR && !result.hour.isDefined())
                    result.hour = (Hour)simpleDate.hour.clone();
                if (granularity >= DAY && !result.dayOfMonth.isDefined())
                    result.dayOfMonth = (DayOfMonth)simpleDate.dayOfMonth.clone();
                if (granularity >= DAY &&
                        !(result.dayOfMonth.isDefined() || result.dayOfWeek.isDefined())){
                    result.dayOfMonth = (DayOfMonth)simpleDate.dayOfMonth.clone();
                    result.dayOfWeek = (DayOfWeek)simpleDate.dayOfWeek.clone();
                }
                if (granularity >= MONTH && !result.month.isDefined() && result.dayOfMonth.isDefined())
                    result.month = (Month)simpleDate.month.clone();
                if (granularity >= YEAR && !result.year.isDefined() && result.month.isDefined())
                    result.year = (Year)simpleDate.year.clone();
                // change --89 into 1989 if possible
                else if (granularity >= YEAR && result.year.isDefined() && !result.year.isComplete()) {
                    if (result.year.getMillennium() == -1 && simpleDate.year.getMillennium() != -1){
                        result.year.setMillennium(simpleDate.year.getMillennium());
                        if (result.year.getCentury() == -1 && simpleDate.year.getCentury() != -1){
                            result.year.setCentury(simpleDate.year.getCentury());
                            if (result.year.getDecade() == -1 && simpleDate.year.getDecade() != -1){
                                result.year.setDecade(simpleDate.year.getDecade());
                            }
                            
                        }
                    }
                }
                result.checkFullDate();
                return result;
            } else if (date instanceof VagueInterval){
                return VagueInterval.completeSimpleDateFromInterval(this, (VagueInterval)date);
            } else if (date instanceof VagueMultipleDate){
                return VagueMultipleDate.completeSimpleDateFromMultipleDate(this, (VagueMultipleDate)date);
            } else {
                throw new NotImplementedException();
            }
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    /**
     * Tests if the century is missing in the date.
     * @return true whether the century is missing in the date, false otherwise
     */
    public boolean isCenturyMissing(){
        return this.year.isCenturyMissing();
    }
    
    
    /**
     * Returns an English description of the current Date.
     * @return an English description of the current Date.
     */
    public String toString() {
        if (this.isDefined()){
            String result = "";
            if (this.granularity >= YEAR)
                result += this.year.toString() + " ";
            if (this.granularity >= MONTH)
                result += this.month.toString() + " ";
            if (this.granularity >= DAY) {
                result += this.dayOfMonth.toString() + " ";
                if (this.dayOfWeek.isDefined())
                    result += "(" + this.dayOfWeek.toString() + ") ";
            }
            if (this.granularity >= HOUR)
                result += this.hour.toString() + " ";
            if (this.granularity >= MINUTE)
                result += this.minute.toString() + " ";
            if (this.granularity >= SECOND)
                result += this.second.toString() + " ";
            return result.trim();
        } else {
            return "SOME_DATE";
        }
    }
    
    /**
     * Gets the String formated representation of the current date.
     * @return the String formated representation of the current date.
     */
    public String getValue(){
        String strResult = "";
        if (this.granularity >= YEAR) {
            if (this.year.isBeforeChrist())
                strResult += "=";
            else
                strResult += "+";
            strResult += this.year.getValue();
            
        } else
            strResult += "-----";
        if (this.granularity >= MONTH)
            strResult += this.month.getValue();
        else
            strResult += "--";
        if (this.granularity >= DAY)
            strResult += this.dayOfMonth.getValue();
        else
            strResult += "--";
        strResult += "T";
        if (this.granularity >= HOUR)
            strResult += this.hour.getValue();
        else
            strResult += "--";
        if (this.granularity >= MINUTE)
            strResult += this.minute.getValue();
        else
            strResult += "--";
        if (this.granularity >= SECOND)
            strResult += this.second.getValue();
        else
            strResult += "--";
        strResult += "-O---W--";
        if (this.granularity >= DAY)
            strResult += this.dayOfWeek.getValue();
        else
            strResult += "-";
        return strResult;
    }
    
    
    /**
     * Tests if the Date is defined.
     * @return true whether the Date is defined, false otherwise.
     */
    public boolean isDefined() {
        return this.year.isDefined() || this.month.isDefined() || this.dayOfMonth.isDefined() ||
                this.hour.isDefined() || this.minute.isDefined() || this.second.isDefined() || this.dayOfWeek.isDefined();
    }
    
    /**
     * Tests if the current Date is absolute. A date is absolute if the year is fully specified.
     * @return true whether the date is absolute, false otherwise
     */
    public boolean isAbsolute() {
        return this.year.isAbsolute();
    }
    
    
    /**
     * Compare dates, starting at a certain granularity. Consider that all lower granularities
     * in both dates are equal.
     * Recursively called by compareTo().
     * @param date the date to compare
     * @param granularity the granularity where to begin the comparison
     * @return the int value of the comparison.
     */
    public int compareLevels(VagueDate date, int granularity){
        if (date instanceof VagueSimpleDate){
            VagueSimpleDate simpleDate = (VagueSimpleDate)date;
            // Different granularities
            if (this.hasGranularity(granularity) && !simpleDate.hasGranularity(granularity))
                return TemporalRelation.DURING;
            else if (simpleDate.hasGranularity(granularity) && !this.hasGranularity(granularity))
                return TemporalRelation.INCLUDES;
            else{
                DateUnit dateUnit1 = this.getDateUnit(granularity);
                DateUnit dateUnit2 = simpleDate.getDateUnit(granularity);
                
                // Same granularities, one defined and the other not
                if (dateUnit1.isDefined() && !dateUnit2.isDefined())
                    return TemporalRelation.DURING;
                else if (dateUnit2.isDefined() && !dateUnit1.isDefined())
                    return TemporalRelation.INCLUDES;
                // Equal, compare higher granularity (SECOND excepted => EQUALS)
                else if (dateUnit1.equals(dateUnit2)){
                    if (granularity != SECOND)
                        return this.compareLevels(simpleDate, TemporalConstituent.getSmallerGranularity(granularity));
                    else
                        return TemporalRelation.EQUALS;
                }
                // Not equals, run unit.compareTo
                else
                    return dateUnit1.compareTo(dateUnit2);
            }
        } else {
            VagueDate cloneDateThis = this.completeByForce();
            VagueDate cloneDateOther = date.completeByForce();
            return cloneDateThis.compareTo(cloneDateOther);
        }
    }
    
    
    /**
     * Get a "real" date from the current Date. This means that underspecified elements
     * like afternoon, Winter, etc. will be changed into intervals of time
     *
     * @return a "real" VagueDate.
     */
    public VagueDate getRealDate(){
        try {
            VagueDate result = (VagueSimpleDate) this.clone();
            result = this.getYear().getRealDate(false).completeDate(result);
            result = this.getMonth().getRealDate(false).completeDate(result);
            result = this.getDayOfMonth().getRealDate(false).completeDate(result);
            result = this.getDayOfWeek().getRealDate(false).completeDate(result);
            result = this.getHour().getRealDate(false).completeDate(result);
            result = this.getMinute().getRealDate(false).completeDate(result);
            result = this.getSecond().getRealDate(false).completeDate(result);
            result.checkFullDate();
            return result;
        } catch (Exception ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    /**
     * Returns the interval (Duration) between two absolute dates.
     * The current date must be BEFORE the specified date.
     *
     * @param date the date to compare to the current one
     * @return the Duration holding between the two dates.
     */
    public Duration getIntervalTo(VagueDate date) {
        int compare = this.compareTo(date);
        if (!this.isAbsolute() || !date.isAbsolute())
            throw new NotImplementedException("This operation can only be performed with absolute dates!");
        else if (compare != TemporalRelation.BEFORE) {
            if (compare == TemporalRelation.AFTER)
                throw new NotImplementedException("This operation cannot be performed if the first date is AFTER the second one!");
            else
                return new SimpleDuration(new Seconds(0));
        } else {
            try {
                VagueDate vagueCurrentDate = ((VagueSimpleDate)this.clone()).getRealDate();
                VagueSimpleDate currentDate;
                if (vagueCurrentDate instanceof VagueSimpleDate)
                    currentDate = (VagueSimpleDate)vagueCurrentDate;
                else
                    return vagueCurrentDate.getIntervalTo(date);
                
                if (date instanceof VagueSimpleDate) {
                    VagueSimpleDate sdate = (VagueSimpleDate) date.clone();
                    SimpleDuration result = new SimpleDuration();
                    int c;
                    int t = 0;
                    // seconds
                    if (currentDate.hasGranularity(SECOND) && sdate.hasGranularity(SECOND)) {
                        c = currentDate.getSecond().compareTo(sdate.getSecond());
                        while (c == TemporalRelation.BEFORE || c == TemporalRelation.AFTER) {
                            currentDate = currentDate.add(SECOND, 1).getUpperBound();
                            ++t;
                            c = currentDate.getSecond().compareTo(sdate.getSecond());
                        }
                        result.setSeconds(new Seconds(t));
                    }
                    t = 0;
                    // minutes
                    if (currentDate.hasGranularity(MINUTE) && sdate.hasGranularity(MINUTE)) {
                        c = currentDate.getMinute().compareTo(sdate.getMinute());
                        while (c == TemporalRelation.BEFORE || c == TemporalRelation.AFTER) {
                            currentDate = currentDate.add(MINUTE, 1).getUpperBound();
                            ++t;
                            c = currentDate.getMinute().compareTo(sdate.getMinute());
                        }
                        result.setMinutes(new Minutes(t));
                    }
                    t = 0;
                    // hours
                    if (currentDate.hasGranularity(HOUR) && sdate.hasGranularity(HOUR)) {
                        c = currentDate.getHour().compareTo(sdate.getHour());
                        while (c == TemporalRelation.BEFORE || c == TemporalRelation.AFTER) {
                            currentDate = currentDate.add(HOUR, 1).getUpperBound();
                            ++t;
                            c = currentDate.getHour().compareTo(sdate.getHour());
                        }
                        result.setHours(new Hours(t));
                    }
                    t = 0;
                    // day of month
                    if (currentDate.hasGranularity(DAY) && sdate.hasGranularity(DAY)) {
                        c = currentDate.getDayOfMonth().compareTo(sdate.getDayOfMonth());
                        while (c == TemporalRelation.BEFORE || c == TemporalRelation.AFTER) {
                            currentDate = currentDate.add(DAY, 1).getUpperBound();
                            ++t;
                            c = currentDate.getDayOfMonth().compareTo(sdate.getDayOfMonth());
                        }
                        result.setDays(new Days(t));
                    }
                    t = 0;
                    // months
                    if (currentDate.hasGranularity(MONTH) && sdate.hasGranularity(MONTH)) {
                        c = currentDate.getMonth().compareTo(sdate.getMonth());
                        while (c == TemporalRelation.BEFORE || c == TemporalRelation.AFTER) {
                            currentDate = currentDate.add(MONTH, 1).getUpperBound();
                            ++t;
                            c = currentDate.getMonth().compareTo(sdate.getMonth());
                        }
                        result.setMonths(new Months(t));
                    }
                    t = 0;
                    // years
                    if (currentDate.hasGranularity(YEAR) && sdate.hasGranularity(YEAR)) {
                        c = currentDate.getYear().compareTo(sdate.getYear());
                        while (c == TemporalRelation.BEFORE || c == TemporalRelation.AFTER) {
                            currentDate = currentDate.add(YEAR, 1).getUpperBound();
                            ++t;
                            c = currentDate.getYear().compareTo(sdate.getYear());
                        }
                        result.setYears(t);
                    }
                    return result;
                } else if (date instanceof VagueInterval) {
                    return date.getIntervalFrom(this);
                } else if (date instanceof VagueMultipleDate) {
                    return date.getIntervalFrom(this);
                } else {
                    throw new NotImplementedException();
                }
            } catch (CloneNotSupportedException ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            }
            
        }
    }
    
    
    /**
     * Returns the interval (Duration) between two absolute dates.
     * The current date must be AFTER the specified date.
     *
     * @param date the date to compare to the current one
     * @return the Duration holding between the two dates.
     */
    public Duration getIntervalFrom(VagueDate date) {
        int compare = this.compareTo(date);
        if (!this.isAbsolute() || !date.isAbsolute())
            throw new NotImplementedException("This operation can only be performed with absolute dates!");
        else if (compare != TemporalRelation.AFTER) {
            if (compare == TemporalRelation.BEFORE)
                throw new NotImplementedException("This operation cannot be performed if the first date is BEFORE the second one!");
            else
                return new SimpleDuration(new Seconds(0));
        } else {
            try {
                VagueDate vagueCurrentDate = ((VagueSimpleDate)this.clone()).getRealDate();
                VagueSimpleDate currentDate;
                if (vagueCurrentDate instanceof VagueSimpleDate)
                    currentDate = (VagueSimpleDate)vagueCurrentDate;
                else
                    return vagueCurrentDate.getIntervalFrom(date);
                
                if (date instanceof VagueSimpleDate) {
                    VagueSimpleDate sdate = null;
                    try {
                        sdate = (VagueSimpleDate) date.clone();
                    } catch (CloneNotSupportedException ex) {
                        ex.printStackTrace();
                    }
                    SimpleDuration result = new SimpleDuration();
                    int c;
                    int t = 0;
                    // seconds
                    if (currentDate.hasGranularity(SECOND) && sdate.hasGranularity(SECOND)) {
                        c = currentDate.getSecond().compareTo(sdate.getSecond());
                        while (c == TemporalRelation.BEFORE || c == TemporalRelation.AFTER) {
                            SimpleDuration dur = new SimpleDuration(new Seconds(1));
                            dur.setMod(EXACT);
                            currentDate = currentDate.substract(dur).getLowerBound();
                            ++t;
                            c = currentDate.getSecond().compareTo(sdate.getSecond());
                        }
                        result.setSeconds(new Seconds(t));
                    }
                    t = 0;
                    // minutes
                    if (currentDate.hasGranularity(MINUTE) && sdate.hasGranularity(MINUTE)) {
                        c = currentDate.getMinute().compareTo(sdate.getMinute());
                        while (c == TemporalRelation.BEFORE || c == TemporalRelation.AFTER) {
                            SimpleDuration dur = new SimpleDuration(new Minutes(1));
                            dur.setMod(EXACT);
                            currentDate = currentDate.substract(dur).getLowerBound();
                            ++t;
                            c = currentDate.getMinute().compareTo(sdate.getMinute());
                        }
                        result.setMinutes(new Minutes(t));
                    }
                    t = 0;
                    // hours
                    if (currentDate.hasGranularity(HOUR) && sdate.hasGranularity(HOUR)) {
                        c = currentDate.getHour().compareTo(sdate.getHour());
                        while (c == TemporalRelation.BEFORE || c == TemporalRelation.AFTER) {
                            SimpleDuration dur = new SimpleDuration(new Hours(1));
                            dur.setMod(EXACT);
                            currentDate = currentDate.substract(dur).getLowerBound();
                            ++t;
                            c = currentDate.getHour().compareTo(sdate.getHour());
                        }
                        result.setHours(new Hours(t));
                    }
                    t = 0;
                    // day of month
                    if (currentDate.hasGranularity(DAY) && sdate.hasGranularity(DAY)) {
                        c = currentDate.getDayOfMonth().compareTo(sdate.getDayOfMonth());
                        while (c == TemporalRelation.BEFORE || c == TemporalRelation.AFTER) {
                            SimpleDuration dur = new SimpleDuration(new Days(1));
                            dur.setMod(EXACT);
                            currentDate = currentDate.substract(dur).getLowerBound();
                            ++t;
                            c = currentDate.getDayOfMonth().compareTo(sdate.getDayOfMonth());
                        }
                        result.setDays(new Days(t));
                    }
                    t = 0;
                    // months
                    if (currentDate.hasGranularity(MONTH) && sdate.hasGranularity(MONTH)) {
                        c = currentDate.getMonth().compareTo(sdate.getMonth());
                        while (c == TemporalRelation.BEFORE || c == TemporalRelation.AFTER) {
                            SimpleDuration dur = new SimpleDuration(new Months(1));
                            dur.setMod(EXACT);
                            currentDate = currentDate.substract(dur).getLowerBound();
                            ++t;
                            c = currentDate.getMonth().compareTo(sdate.getMonth());
                        }
                        result.setMonths(new Months(t));
                    }
                    t = 0;
                    // years
                    try{
                        if (currentDate.hasGranularity(YEAR) && sdate.hasGranularity(YEAR)) {
                            c = currentDate.getYear().compareTo(sdate.getYear());
                            while (c == TemporalRelation.BEFORE || c == TemporalRelation.AFTER) {
                                SimpleDuration dur = new SimpleDuration(new Years(1));
                                dur.setMod(EXACT);
                                currentDate = currentDate.substract(dur).getLowerBound();
                                ++t;
                                c = currentDate.getYear().compareTo(sdate.getYear());
                            }
                            result.setYears(t);
                        }
                    } catch (BadFormatException e) {
                        e.printStackTrace();
                        System.exit(-1);
                        return null;
                    }
                    return result;
                } else if (date instanceof VagueInterval) {
                    return date.getIntervalTo(this);
                } else if (date instanceof VagueMultipleDate) {
                    return date.getIntervalTo(this);
                } else
                    throw new NotImplementedException();
            } catch (CloneNotSupportedException ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            }
        }
    }
    
    
    /**
     * Test whether two dates have exactly the same date components.
     * We don't care about XipNodes or unspecified elements.
     * @param date the date to compare.
     * @return true whether the object is equal to the current element.
     */
    public boolean softEquals(VagueDate date) {
        if (date.getClass().equals(this.getClass())) {
            VagueSimpleDate sdate = (VagueSimpleDate)date;
            boolean equal = true;
            if (!sdate.isDefined() || !this.isDefined())
                equal = false;
            if (sdate.year.isDefined() && this.year.isDefined())
                equal = sdate.year.equals(this.year);
            if (equal && sdate.month.isDefined() && this.month.isDefined())
                equal = sdate.month.equals(this.month);
            if (equal && sdate.dayOfMonth.isDefined() && this.dayOfMonth.isDefined())
                equal = sdate.dayOfMonth.equals(this.dayOfMonth);
            if (equal && sdate.dayOfWeek.isDefined() && this.dayOfWeek.isDefined())
                equal = sdate.dayOfWeek.equals(this.dayOfWeek);
            if (equal && sdate.hour.isDefined() && this.hour.isDefined())
                equal = sdate.hour.equals(this.hour);
            if (equal && sdate.minute.isDefined() && this.minute.isDefined())
                equal = sdate.minute.equals(this.minute);
            if (equal && sdate.second.isDefined() && this.second.isDefined())
                equal = sdate.second.equals(this.second);
            return equal;
        } else
            return false;
    }
    
    
    /**
     * Compares this object with the specified object for order.
     * Use {@link TemporalRelation} temporal constants instead of negative/positive values.
     */
    public int compareTo(TemporalUnit o) {
        if (o instanceof VagueSimpleDate) {
            VagueSimpleDate date = (VagueSimpleDate)o;
            // UNDEFINED
            if (!date.isDefined() || !this.isDefined())
                return TemporalRelation.UNDEFINED;
            // NOT ABSOLUTE => UNDEFINED
            else if (!date.isAbsolute() || !this.isAbsolute()){
                // can be 197- and 196-
                if (date.getYear().isDefined() && this.getYear().isDefined())
                    return this.getYear().compareTo(date.getYear());
                else
                    return TemporalRelation.UNDEFINED;
            } else if (date.getMod() != this.getMod()){
                return this.getRealDate().compareTo(date.getRealDate());
            } else
                return this.compareLevels(date, YEAR);
        } else if (o instanceof VagueInterval) {
            VagueInterval date = (VagueInterval)o;
            return TemporalRelation.getInverseRelation(date.compareTo(this));
        } else if (o instanceof VagueMultipleDate) {
            VagueMultipleDate date = (VagueMultipleDate)o;
            return TemporalRelation.getInverseRelation(date.compareTo(this));
        } else
            throw new NotImplementedException();
    }
    
    
    /**
     * Changes the date granularity. It's only possible to attribute a lower granularity.
     * Special behaviour for granularity WEEK. DOES NOT modify the original date, only returns
     * a new one.
     * @param granularity the new granularity
     * @return the new date
     * @throws LostGranularityException if trying to assign a higher granularity than the current one.
     */
    public VagueDate changeGranularity(int granularity) throws LostGranularityException{
        // lower than YEAR => YEAR
        // MILLENIUM
        if (granularity == MILLENNIUM){
            this.year.setCentury(-1);
            this.year.setDecade(-1);
            this.year.setYear(-1);
        }
        // CENTURY
        if (granularity == CENTURY){
            this.year.setDecade(-1);
            this.year.setYear(-1);
        }
        // DECADE
        if (granularity == DECADE){
            this.year.setYear(-1);
        }
        
        if (granularity == MILLENNIUM || granularity == CENTURY || granularity == DECADE)
            granularity = YEAR;
        
        // WEEK has no meaning if the full date is unknown => DAY
        if (granularity == WEEK && this.fullDate == null)
            granularity = DAY;
        if (this.granularity < granularity)
            throw new LostGranularityException("Cannot assign granularity " + granularity + " to this date.");
        else{
            try {
                if (granularity == WEEK && this.fullDate != null){
                    // WEEK
                    // interval between Monday to Sunday
                    DayOfWeek unitMonday = new DayOfWeek(1);
                    DayOfWeek unitSunday = new DayOfWeek(7);
                    VagueSimpleDate dateMonday = new VagueSimpleDate(unitMonday);
                    VagueSimpleDate dateSunday = new VagueSimpleDate(unitSunday);
                    dateMonday = this.closest(dateMonday, -1).getLowerBound();
                    dateSunday = this.closest(dateSunday, 1).getUpperBound();
                    VagueInterval result = new VagueInterval(dateMonday, dateSunday);
                    return result;
                } else {
                    // else just set new granularity
                    VagueSimpleDate result = (VagueSimpleDate) this.clone();
                    result.setGranularity(granularity);
                    return result;
                }
            } catch (CloneNotSupportedException ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            } catch (BadFormatException ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            } catch (IntervalException ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            }
        }
    }
    
    
    /**
     * Tests if the date has the specified granularity. This is true
     * whether the granularity is equal or higher than the argument.
     * @param granularity the test granularity
     * @return true whether the date has the specified granularity (or higher), false otherwise.
     */
    public boolean hasGranularity(int granularity){
        return this.granularity >= granularity;
    }
    
    
    
    /**
     * Adds a SimpleDuration to the current Date.
     * @param duration the SimpleDuration to add to the current Date
     * @return the resulting VagueDate
     */
    public VagueDate addSimpleDuration(SimpleDuration duration) {
        try{
            VagueDate result = VagueDate.getNewDate();
            result.copy(this);
            int n;
            // seconds
            n = duration.getSeconds().getIntValue();
            if (n == -1) {   // several seconds
                VagueSimpleDate date1 = this.addSimpleDuration((new SimpleDuration(new Seconds(2)))).getLowerBound();
                VagueSimpleDate date2 = this.addSimpleDuration((new SimpleDuration(new Seconds(2 * TemporalConstituent.getApproximateMaxNumber(TemporalConstituent.SECOND))))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date1, date2);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                result = result.add(Calendar.SECOND, n);
            }
            // minutes
            n = duration.getMinutes().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.addSimpleDuration((new SimpleDuration(new Minutes(2)))).getLowerBound();
                VagueSimpleDate date2 = this.addSimpleDuration((new SimpleDuration(new Minutes(2 * TemporalConstituent.getApproximateMaxNumber(TemporalConstituent.MINUTE))))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date1, date2);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.MINUTE, n);
                else
                    result.add(Duration.getNewDuration(new Minutes(n)));
            }
            // hours
            n = duration.getHours().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.addSimpleDuration((new SimpleDuration(new Hours(2)))).getLowerBound();
                VagueSimpleDate date2 = this.addSimpleDuration((new SimpleDuration(new Hours(2 * TemporalConstituent.getApproximateMaxNumber(TemporalConstituent.HOUR))))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date1, date2);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.HOUR, n);
                else
                    result.add(Duration.getNewDuration(new Hours(n)));
            }
            // days
            n = duration.getDays().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.addSimpleDuration((new SimpleDuration(new Days(2)))).getLowerBound();
                VagueSimpleDate date2 = this.addSimpleDuration((new SimpleDuration(new Days(2 * TemporalConstituent.getApproximateMaxNumber(TemporalConstituent.DAY))))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date1, date2);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.DAY_OF_MONTH, n);
                else
                    result.add(Duration.getNewDuration(new Days(n)));
            }
            // weeks
            n = duration.getWeeks().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.addSimpleDuration((new SimpleDuration(new Days(14)))).getLowerBound();
                VagueSimpleDate date2 = this.addSimpleDuration((new SimpleDuration(new Days(20 * TemporalConstituent.getApproximateMaxNumber(TemporalConstituent.MONTH))))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date1, date2);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.DAY_OF_MONTH, n * 7);
                else
                    result.add(Duration.getNewDuration(new Days(n * 7)));
                
            }
            // months
            n = duration.getMonths().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.addSimpleDuration((new SimpleDuration(new Months(2)))).getLowerBound();
                VagueSimpleDate date2 = this.addSimpleDuration((new SimpleDuration(new Months(2 * TemporalConstituent.getApproximateMaxNumber(TemporalConstituent.MONTH))))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date1, date2);
                 else 
                    result = VagueDate.getNewDate();
           } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.MONTH, n);
                else
                    result.add(Duration.getNewDuration(new Months(n)));
            }
            // years
            n = duration.getYears().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.addSimpleDuration((new SimpleDuration(new Years(2)))).getLowerBound();
                VagueSimpleDate date2 = this.addSimpleDuration((new SimpleDuration(new Decades(2)))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date1, date2);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.YEAR, n);
                else
                    result.add(Duration.getNewDuration(new Years(n)));
            }
            // decades
            n = duration.getDecades().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.addSimpleDuration((new SimpleDuration(new Decades(2)))).getLowerBound();
                VagueSimpleDate date2 = this.addSimpleDuration((new SimpleDuration(new Centuries(2)))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date1, date2);
                 else 
                    result = VagueDate.getNewDate();
           } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.YEAR, n * 10);
                else
                    result.add(Duration.getNewDuration(new Years(n * 10)));
            }
            // centuries
            n = duration.getCenturies().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.addSimpleDuration((new SimpleDuration(new Centuries(2)))).getLowerBound();
                VagueSimpleDate date2 = this.addSimpleDuration((new SimpleDuration(new Millenniums(2)))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date1, date2);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.YEAR, n * 100);
                else
                    result.add(Duration.getNewDuration(new Years(n * 100)));
            }
            // millenniums
            n = duration.getMillenniums().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.addSimpleDuration((new SimpleDuration(new Millenniums(2)))).getLowerBound();
                VagueSimpleDate date2 = this.addSimpleDuration((new SimpleDuration(new Millenniums(10)))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date1, date2);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.YEAR, n * 1000);
                else
                    result.add(Duration.getNewDuration(new Years(n * 1000)));
            }
            
            if (result instanceof VagueSimpleDate)
                ((VagueSimpleDate)result).getFullDate();
            return result;
        } catch (BadFormatException ex){
            ex.printStackTrace();
            System.exit(-1);
            return null;
        } catch (IntervalException ex){
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
        
    }
    
    
    /**
     * Substracts a SimpleDuration to the current Date.
     * @param duration the SimpleDuration to substract to the current Date
     * @return the resulting VagueDate
     */
    private VagueDate substractSimpleDuration(SimpleDuration duration) {
        try{
            VagueDate result = VagueDate.getNewDate();
            result.copy(this);
            int n;
            // seconds
            n = duration.getSeconds().getIntValue();
            if (n == -1) {   // several seconds
//                VagueSimpleDate date1 = (VagueSimpleDate)this.clone();
//                VagueSimpleDate date2 = (VagueSimpleDate)this.clone();
                VagueSimpleDate date1 = this.substractSimpleDuration((new SimpleDuration(new Seconds(2)))).getLowerBound();
                VagueSimpleDate date2 = this.substractSimpleDuration((new SimpleDuration(new Seconds(2 * TemporalConstituent.getApproximateMaxNumber(TemporalConstituent.SECOND))))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date2, date1);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                result = result.add(Calendar.SECOND, -n);
            }
            // minutes
            n = duration.getMinutes().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.substractSimpleDuration((new SimpleDuration(new Minutes(2)))).getLowerBound();
                VagueSimpleDate date2 = this.substractSimpleDuration((new SimpleDuration(new Minutes(2 * TemporalConstituent.getApproximateMaxNumber(TemporalConstituent.MINUTE))))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date2, date1);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.MINUTE, -n);
                else
                    result.substract(Duration.getNewDuration(new Minutes(n)));
            }
            // hours
            n = duration.getHours().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.substractSimpleDuration((new SimpleDuration(new Hours(2)))).getLowerBound();
                VagueSimpleDate date2 = this.substractSimpleDuration((new SimpleDuration(new Hours(2 * TemporalConstituent.getApproximateMaxNumber(TemporalConstituent.HOUR))))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date2, date1);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.HOUR, -n);
                else
                    result.substract(Duration.getNewDuration(new Hours(n)));
            }
            // days
            n = duration.getDays().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.substractSimpleDuration((new SimpleDuration(new Days(2)))).getLowerBound();
                VagueSimpleDate date2 = this.substractSimpleDuration((new SimpleDuration(new Days(2 * TemporalConstituent.getApproximateMaxNumber(TemporalConstituent.DAY))))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date2, date1);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.DAY_OF_MONTH, -n);
                else
                    result.substract(Duration.getNewDuration(new Days(n)));
            }
            // weeks
            n = duration.getWeeks().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.substractSimpleDuration((new SimpleDuration(new Days(14)))).getLowerBound();
                VagueSimpleDate date2 = this.substractSimpleDuration((new SimpleDuration(new Days(14 * TemporalConstituent.getApproximateMaxNumber(TemporalConstituent.WEEK))))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date2, date1);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.DAY_OF_MONTH, -n * 7);
                else
                    result.substract(Duration.getNewDuration(new Days(n * 7)));
                
            }
            // months
            n = duration.getMonths().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.substractSimpleDuration((new SimpleDuration(new Months(2)))).getLowerBound();
                VagueSimpleDate date2 = this.substractSimpleDuration((new SimpleDuration(new Months(2 * TemporalConstituent.getApproximateMaxNumber(TemporalConstituent.MONTH))))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date2, date1);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.MONTH, -n);
                else
                    result.substract(Duration.getNewDuration(new Months(n)));
            }
            // years
            n = duration.getYears().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.substractSimpleDuration((new SimpleDuration(new Years(2)))).getLowerBound();
                VagueSimpleDate date2 = this.substractSimpleDuration((new SimpleDuration(new Decades(2)))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date2, date1);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.YEAR, -n);
                else
                    result.substract(Duration.getNewDuration(new Years(n)));
            }
            // decades
            n = duration.getDecades().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.substractSimpleDuration((new SimpleDuration(new Decades(2)))).getLowerBound();
                VagueSimpleDate date2 = this.substractSimpleDuration((new SimpleDuration(new Centuries(2)))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date2, date1);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.YEAR, -n * 10);
                else
                    result.substract(Duration.getNewDuration(new Years(n * 10)));
            }
            // centuries
            n = duration.getCenturies().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.substractSimpleDuration((new SimpleDuration(new Centuries(2)))).getLowerBound();
                VagueSimpleDate date2 = this.substractSimpleDuration((new SimpleDuration(new Millenniums(2)))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date2, date1);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.YEAR, -n * 100);
                else
                    result.substract(Duration.getNewDuration(new Years(n * 100)));
            }
            // millenniums
            n = duration.getMillenniums().getIntValue();
            if (n == -1) {   // several
                VagueSimpleDate date1 = this.substractSimpleDuration((new SimpleDuration(new Millenniums(2)))).getLowerBound();
                VagueSimpleDate date2 = this.substractSimpleDuration((new SimpleDuration(new Millenniums(10)))).getUpperBound();
                if (date1.isDefined() && date2.isDefined())
                    result = new VagueInterval(date2, date1);
                else 
                    result = VagueDate.getNewDate();
            } else if (n != 0){
                if (result instanceof VagueSimpleDate)
                    result = result.add(Calendar.YEAR, -n * 1000);
                else
                    result.substract(Duration.getNewDuration(new Years(n * 1000)));
            }
            
            if (result instanceof VagueSimpleDate)
                ((VagueSimpleDate)result).getFullDate();
            return result;
        } catch (BadFormatException ex){
            ex.printStackTrace();
            System.exit(-1);
            return null;
        } catch (IntervalException ex){
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    
    
    /**
     * Gets the date after fuzzyfication, according to the specified margin duration
     * @param margin the margin duration (before and after the "precise" date)
     * @return an interval or a set of intervals corresponding to the fuzzy Date.
     */
    public VagueDate getFuzzyDate(SimpleDuration margin){
        if (!this.isDefined()) {
            return this;
        }
        try {
            VagueSimpleDate date1 = new VagueSimpleDate();
            VagueSimpleDate date2 = new VagueSimpleDate();
            date1.copy(this);
            date2.copy(this);
            date1 = date1.substract(margin).getLowerBound();
            // to be corrected !
            date2 = date2.add(margin).getUpperBound();
            VagueDate result;
            if (!date1.isDefined() || !date2.isDefined())
                result = date1;
            else if (!date1.equals(date2))
                result = new VagueInterval(date1, date2);
            else
                result = date1;
            return result;
            
        } catch (IntervalException ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
        return null;
    }
    
    
    /**
     * Gets the VagueSimpleDate representing the lower bound of current date.
     * In the case of a VagueSimpleDate, just returns the current date
     * @return the current date
     */
    public VagueSimpleDate getLowerBound(){
        return this;
    }
    
    
    /**
     * Gets the VagueSimpleDate representing the upper bound of current date.
     * In the case of a VagueSimpleDate, just returns the current date
     * @return the current date.
     */
    public VagueSimpleDate getUpperBound(){
        return this;
    }
    
    
    /**
     * Gets the Duration holding between the lower bound and the upper bound of the date.
     * In the case of a VagueSimpleDate, return the Duration corresponding to the granularity.
     * @return the Duration holding between the lower bound and the upper bound of the date
     */
    public SimpleDuration getAssociatedDuration(){
//        try{
        int dateGranularity = this.getGranularity();
        SimpleDuration durationGranularity =
                this.getDateUnit(dateGranularity).getAssociatedDuration();
        return durationGranularity;
//        } catch (BadFormatException e){
//            e.printStackTrace();
//            System.exit(-1);
//            return null;
//        }
    }
    
    
    /**
     * Selects in the current VagueDate the dates corresponding to daytime.
     * Daytime is from 7 AM to 6 PM.
     *
     * @return the dates corresponding to daytime.
     */
    public VagueDate getOnlyDayTime(){
        if (this.hasGranularity(HOUR)) {
            try {
                VagueDate reference = new VagueInterval(new VagueSimpleDate(new Hour(7)), new VagueSimpleDate(new Hour(18)));
                VagueDate completeDate = this.completeByForce();
                reference = reference.completeDate(this).completeByForce();
                int c = completeDate.compareTo(reference);
                if (c != TemporalRelation.AFTER && c != TemporalRelation.BEFORE)
                    return this;
                else
                    return null;
            } catch (BadFormatException ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            } catch (IntervalException ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            }
            
        } else {
            return this;
        }
    }
    
    
    /**
     * Gets the date immediately after the current one. The semantics of "just after"
     * depends on the granularity. eg: July 14 is "just after" July 13 (one day after)
     *
     * @return the date immediately after the current one
     */
    public VagueSimpleDate getJustAfter(){
        SimpleDuration durationGranularity = this.getAssociatedDuration();
        durationGranularity.setMod(TemporalConstituent.EXACT);
        return this.add(durationGranularity).getLowerBound();
    }
    
    
    /**
     * Gets the date immediately before the current one. The semantics of "just before"
     * depends on the granularity. eg: July 12 is "just before" July 13 (one day before)
     *
     * @return the date immediately before the current one
     */
    public VagueSimpleDate getJustBefore(){
        SimpleDuration durationGranularity = this.getAssociatedDuration();
        durationGranularity.setMod(TemporalConstituent.EXACT);
        return this.substract(durationGranularity).getUpperBound();
    }
    
    /**
     * Adds a Duration to the current Date.
     * @param duration the Duration to add to the current Date
     * @return the resulting VagueDate
     */
    public VagueDate add(Duration duration) {
        if (!this.isDefined())
            return this;
        else {
            VagueDate result = null;
            boolean approx = duration.getMod() == APPROX || duration.getMod() == UNDEFINED;
            
            try{
                if (duration instanceof SimpleDuration){
                    result = this.addSimpleDuration((SimpleDuration) duration);
                    if (approx)
                        result = result.getFuzzyDate(((SimpleDuration) duration).getFuzzyMargin());
                } else if (duration instanceof DurationInterval){
                    DurationInterval interval = (DurationInterval) duration;
                    VagueSimpleDate date1 = (VagueSimpleDate)this.clone();
                    VagueSimpleDate date2 = (VagueSimpleDate)this.clone();
                    result = new VagueInterval(
                            date1.addSimpleDuration(interval.getLowerBound()).getLowerBound(),
                            date2.addSimpleDuration(interval.getUpperBound()).getUpperBound());
                } else if (duration instanceof MultipleDuration){
                    String newDateFormat = "";
                    MultipleDuration multipleDuration = (MultipleDuration)duration;
                    String operator = multipleDuration.getOperator();
                    Duration[] durations = multipleDuration.getAllDurations();
                    for (int i = 0 ; i < durations.length ; ++i){
                        VagueDate date = this.add(durations[i]);
                        newDateFormat += date.getValue();
                        if (i < durations.length - 1)
                            newDateFormat += operator;
                    }
                    try {
                        result = getNewDate(newDateFormat);
                    } catch (BadFormatException ex) {
                        ex.printStackTrace();
                        System.exit(-1);
                    }
                } else
                    throw new NotImplementedException("Duration type unknown...");
            } catch (CloneNotSupportedException ex){
                ex.printStackTrace();
                System.exit(-1);
            } catch (IntervalException ex){
                ex.printStackTrace();
                System.exit(-1);
            }
            return result;
        }
    }
    
    
    
    /**
     * Substracts a Duration to the current Date.
     * @param duration the Duration to subtract to the current Date
     * @return the resulting VagueDate
     */
    public VagueDate substract(Duration duration) {
        if (!this.isDefined())
            return this;
        else{
            VagueDate result = null;
            boolean approx = duration.getMod() == APPROX || duration.getMod() == UNDEFINED;
            
            try{
                if (duration instanceof SimpleDuration){
                    result = this.substractSimpleDuration((SimpleDuration) duration);
                    if (approx && !((SimpleDuration) duration).isNaturallyFuzzy())
                        result = result.getFuzzyDate(((SimpleDuration) duration).getFuzzyMargin());
                } else if (duration instanceof DurationInterval){
                    DurationInterval interval = (DurationInterval) duration;
                    VagueSimpleDate date1 = (VagueSimpleDate)this.clone();
                    VagueSimpleDate date2 = (VagueSimpleDate)this.clone();
                    date1 = date1.substractSimpleDuration(interval.getLowerBound()).getLowerBound();
                    date2 = date2.substractSimpleDuration(interval.getUpperBound()).getUpperBound();
                    result = new VagueInterval(date2, date1);
                } else if (duration instanceof MultipleDuration){
                    String newDateFormat = "";
                    MultipleDuration multipleDuration = (MultipleDuration)duration;
                    String operator = multipleDuration.getOperator();
                    Duration[] durations = multipleDuration.getAllDurations();
                    for (int i = 0 ; i < durations.length ; ++i){
                        VagueDate date = (VagueDate)this.clone();
                        date = date.substract(durations[i]);
                        newDateFormat += date.getValue();
                        if (i < durations.length - 1)
                            newDateFormat += operator;
                    }
                    try {
                        result = getNewDate(newDateFormat);
                    } catch (BadFormatException ex) {
                        ex.printStackTrace();
                        System.exit(-1);
                    }
                } else
                    throw new NotImplementedException("Duration type unknown...");
            } catch (CloneNotSupportedException ex){
                ex.printStackTrace();
                System.exit(-1);
            } catch (IntervalException ex){
                ex.printStackTrace();
                System.exit(-1);
            }
            
            return result;
        }
    }
    
    
    
    
    
    
    
    
    
    
    
    /**
     *
     * @param a
     */
    public static void main(String[] a) {
        try {
            GregorianCalendar fullDate = new GregorianCalendar(); //2006, 11, 27);
            
            System.err.println(fullDate.get(Calendar.DAY_OF_WEEK));
            System.err.println(fullDate);
            
            System.err.println("---------------");
            
            String strDate1 = "-20061128T-------O---W---";
            String strDate2 = "---------T-------O---W---";
            VagueSimpleDate vg1 = new VagueSimpleDate(strDate1);
            VagueSimpleDate vg2 = new VagueSimpleDate(strDate2);
            System.err.println(vg1.toString());
            
            System.err.println("---------------");
            
            VagueDate newVg = vg1.closest(vg2, -2);
            
            System.err.println(newVg.toString());
        } catch(Exception e) {
            e.printStackTrace();
        }
    }
    
    
}




