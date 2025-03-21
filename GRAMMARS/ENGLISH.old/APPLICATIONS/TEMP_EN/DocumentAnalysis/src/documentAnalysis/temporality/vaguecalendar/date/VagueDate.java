/*
 * VagueDate.java
 *
 * Created on December 8, 2006, 8:26 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.date;


import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import documentAnalysis.temporality.LostGranularityException;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;
import java.util.Calendar;
import java.util.GregorianCalendar;

/**
 *
 * @author xtannier
 */
public abstract class VagueDate extends TemporalConstituent implements TemporalUnit {
    
    private boolean dct = false;
    
    /**
     * Builds a new instance of VagueDate.
     */
    public VagueDate(){
        super();
    }
    
    /**
     * Builds a new instance of VagueDate.
     * @param unit the XipUnit in which the date is found
     * @param node the XipNode corresponding to the date
     */
    public VagueDate(XipUnit unit, XipNode node){
        super(unit, node);
    }
    
    /**
     * Copy a specified simple date
     * @param date the VagueSimpleDate to copy
     */
    public abstract void copy(VagueSimpleDate date);
    
    
    /**
     * Tests if the date is the Document Creation TIMe
     * @return true if the date is the Document Creation Date, false otherwise
     */
    public boolean isDCT(){
        return this.dct;
    }
    
    /**
     * Converts the date into Document Creation Time
     */
    public void setDCT(){
        this.dct = true;
    }
    
    /**
     * Returns a fully under-specified VagueSimpleDate
     * @return a fully under-specified VagueSimpleDate
     */
    public static VagueDate getNewDate(){
        return new VagueSimpleDate();
    }
    
    /**
     * Returns a VagueDate with specified year, month and day.
     * @param year the year
     * @param month the month
     * @param dayOfMonth the day
     * @throws documentAnalysis.temporality.vaguecalendar.BadFormatException
     * @return a VagueDate with specified year, month and day.
     */
    public static VagueDate getNewDate(int year, int month, int dayOfMonth) throws BadFormatException{
        return new VagueSimpleDate(year, month, dayOfMonth);
    }
    
    /**
     * Returns a new date based on the specified String format
     * @param dateFormat the String formated representation of the date
     * @throws documentAnalysis.temporality.vaguecalendar.BadFormatException if the parameter does
     * not correspond to a well-formed date format
     * @return a new VagueDate
     */
    public static VagueDate getNewDate(String dateFormat) throws BadFormatException{
        if (dateFormat.contains("#") || dateFormat.contains("|")){
            return new VagueMultipleDate(dateFormat);
        } else if (dateFormat.contains("/")){
            return new VagueInterval(dateFormat);
        } else {
            return new VagueSimpleDate(dateFormat);
        }
    }
    
    
    /**
     * Returns a new date associated to the specified XipNode.
     * @param node the XipNode to associate with the date
     * @return a new VagueDate
     */
    public static VagueDate getNewDate(XipUnit unit, XipNode node){
        VagueSimpleDate date = new VagueSimpleDate(unit, node);
        return date;
    }
    
    
    
    /**
     * Returns a new date with specified year, month and day, associated to the specified XipNode.
     * @param year the year
     * @param month the month
     * @param dayOfMonth the day of the month
     * @param node the XipNode to associate with the date
     * @return a new VagueDate
     */
    public static VagueDate getNewDate(int year, int month, int dayOfMonth, XipUnit unit, XipNode node){
        VagueSimpleDate date = new VagueSimpleDate(year, month, dayOfMonth);
        date.setXipNode(unit, node);
        return date;
    }
    
    /**
     * Returns a new date based on the specified String format, associated to the specified XipNode.
     * @param dateFormat the String formated representation of the date
     * @param unit the XipUnit in which the date is found.
     * @param node the XipNode to associate with the date
     * @throws documentanalysis.temporality.vaguecalendar.BadFormatException if the parameter does
     * not correspond to a well-formed date format
     * @return a new VagueDate
     */
    public static VagueDate getNewDate(String dateFormat, XipUnit unit, XipNode node) throws BadFormatException{
        VagueDate date = getNewDate(dateFormat);
        date.setXipNode(unit, node);
        return date;
    }
    
    
    
    
    
    /**
     * Returns a new date based on the specified ISO format (time only)
     * @param isoFormat the ISO format of the date
     * @return a VagueDate corresponding to the specified ISO format (time only)
     */
    private static VagueSimpleDate getTimeFromIso(String isoFormat) throws BadFormatException{
        VagueSimpleDate result = new VagueSimpleDate();
        if (isoFormat.matches("\\d\\d:\\d\\d")){
            // format: 13:37
            // hour
            String hour = isoFormat.substring(0, 2);
            // minute
            String minute = isoFormat.substring(3, 5);
            result.setHour(new Hour(hour));
            result.setMinute(new Minute(minute));
        } else if (isoFormat.matches("\\d\\d:\\d\\d:\\d\\d")){
            // format: 14:26:00
            result = getTimeFromIso(isoFormat.substring(0, 5));
            // second
            String second = isoFormat.substring(6, 8);
            result.setSecond(new Second(second));
        } else if (isoFormat.equals("AF") || isoFormat.equals("EV") || isoFormat.equals("DT") ||
                isoFormat.equals("MI") || isoFormat.equals("MO") || isoFormat.equals("NI")){
            // format: AF
            result = new VagueSimpleDate(new Hour(isoFormat));
        } else {
            throw new BadFormatException(isoFormat);
        }
        return result;
    }
    
    
    /**
     * Returns a new date based on the specified ISO format
     * @param isoFormat the ISO format of the date
     * @return a VagueDate corresponding to the specified ISO format
     */
    public static VagueSimpleDate getDateFromISO(String isoFormat) throws BadFormatException{
        int timeBoundary = isoFormat.indexOf('T');
        VagueSimpleDate time = null;
        VagueSimpleDate result;
        if (timeBoundary != -1){
            time = getTimeFromIso(isoFormat.substring(timeBoundary + 1));
            isoFormat = isoFormat.substring(0, timeBoundary);
        }
        if (isoFormat.matches("\\d\\d\\d\\d")){
            // format: 1998
            // year
            result = new VagueSimpleDate(new Year(isoFormat));
        } else if (isoFormat.matches("\\d\\d\\d")){
            // format: 199  (nineties)
            // year
            result = new VagueSimpleDate(new Year(isoFormat + "-"));
        } else if (isoFormat.matches("\\d\\d\\d\\d-WXX")){
            // format: 1998-WXX
            // year
            result = new VagueSimpleDate(new Year(isoFormat.substring(0, 4)));
        } else if (isoFormat.matches("XXXX-WXX-\\d")){
            // format: XXXX-WXX-1   (week day)
            result = new VagueSimpleDate(new DayOfWeek(isoFormat.substring(9, 10)));
        } else if (isoFormat.matches("\\d\\d\\dX")){
            // format: 199X  (nineties)
            // year
            result = new VagueSimpleDate(new Year(isoFormat.substring(0, 3) + "-"));
        } else if (isoFormat.matches("\\d\\d\\d\\d-\\d\\d-\\d\\d")){
            // format: 1998-01-08
            // year
            String year = isoFormat.substring(0, 4);
            // month
            String month = isoFormat.substring(5, 7);
            // day
            String day = isoFormat.substring(8, 10);
            result = new VagueSimpleDate(Integer.parseInt(year),
                    Integer.parseInt(month),
                    Integer.parseInt(day));
        } else if (isoFormat.matches("\\d\\d\\d\\d-XX-XX")){
            // format: 1998-XX-XX
            // year only
            result = new VagueSimpleDate(new Year(isoFormat.substring(0, 4)));
        } else if (isoFormat.matches("\\d\\d\\d\\d-[A-Z][A-Z1234]")){
            // format: 1998-SU
            result = getDateFromISO(isoFormat.substring(0, 4));
            String month = isoFormat.substring(5, 7);
            result.setMonth(new Month(month));
        } else if (isoFormat.matches("\\d\\d\\d\\d-\\d\\d")){
            // format: 1998-11
            result = getDateFromISO(isoFormat.substring(0, 4));
            String month = isoFormat.substring(5, 7);
            result.setMonth(new Month(month));
        } else if (isoFormat.matches("\\d\\d\\d\\d-W\\d\\d")){
            // format: 1998-W48
            // like 1998
            // year only
            result = getDateFromISO(isoFormat.substring(0, 4));
        } else if (isoFormat.matches("\\d\\d\\d\\d-WXX-3")){
            // format: 1998-WXX-3
            // year
            String year = isoFormat.substring(0, 4);
            // day of the week
            String day = isoFormat.substring(9, 10);
            result = new VagueSimpleDate(new Year(year));
            result.setDayOfWeek(new DayOfWeek(day));
        } else
            throw new BadFormatException(isoFormat);
        
        
        // add time
        if (time != null){
            result.setHour(time.getHour());
            result.setMinute(time.getMinute());
            result.setSecond(time.getSecond());
        }
        
        return result;
    }
    
    
//    /**
//     * Copies all date information from date2 to date1.
//     * The result keeps date1's syntactic information (XipUnit, XipNode...)
//     * date1 is not changed, the result is returned.
//     *
//     * @param date1 the date to copy the information to.
//     * @param date2 the date to copy the information from.
//     */
//    public static VagueDate copyDateInfos(VagueDate date1, VagueDate date2){
//        VagueDate result = date2;
//        result.setXipNode(date1.getXipUnit(), date1.getXipNode());
//        result
//                return result;
//    }
    
    
    /**
     * Adds a specified number of years to the date.
     * @param i the number of years to add
     * @return the new VagueDate.
     */
    public abstract VagueDate addYear(int i);
    
    /**
     * Adds a specified number of months to the date.
     * @param i the number of months to add
     * @return the new VagueDate.
     */
    public abstract VagueDate addMonth(int i);
    
    /**
     * Adds an amount of units given by "field" to the date
     * @param field the field
     * @param amount the amount
     * @return the resulting date
     * @see java.util.GregorianCalendar#add(int field, int amount)
     */
    public abstract VagueDate add(int field, int amount);
    
    
    /**
     * Merges two dates and keep the only date information compatible with both.
     *
     * @return a new date
     * @param date the date to merge with the current one
     * @throws documentanalysis.temporality.TemporalConsistencyException if a mergind is impossible
     */
    public abstract VagueDate merge(VagueDate date) throws TemporalConsistencyException;
    
    /**
     * Returns a VagueDate from a Calendar date
     * @param date the Calendar date
     * @return a VagueDate
     */
    public static VagueDate fullDateToVagueDate(Calendar date){
        return VagueSimpleDate.fullDateToVagueDate(date);
    }
    
    /**
     * Gets the granularity of the date.
     * @return the int value of the granularity
     */
    public abstract int getGranularity();
    
    
    
    
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
    public abstract Duration getGranularityDuration();
    
    
    /**
     * Returns the interval (Duration) between two absolute dates.
     * The current date must be BEFORE the specified date.
     *
     * @param date the date to compare to the current one
     * @return the Duration holding between the two dates.
     */
    public abstract Duration getIntervalTo(VagueDate date);
    
    
    /**
     * Returns the interval (Duration) between two absolute dates.
     * The current date must be AFTER the specified date.
     *
     * @param date the date to compare to the current one
     * @return the Duration holding between the two dates.
     */
    public abstract Duration getIntervalFrom(VagueDate date);
    
    
    
    /**
     * Gets the closest date from the current one, corresponding to the defined elements
     * of the specified date. The direction PAST or FUTURE is chosen depending of the closest date.
     *
     * @param dateToGo the date to reach
     * @return the new date.
     * @see #closest(VagueDate dateToGo, int direction)
     * @see #getClosestFrom(VagueDate dateFrom, int direction)
     */
    public VagueDate closest(VagueDate dateToGo) {
        if (dateToGo.isAbsolute())
            return dateToGo;
        else if (this.isAbsolute()) {
            VagueDate date1 = this.closest(dateToGo, -1);
            VagueDate date2 = this.closest(dateToGo, 1);
            // date1 and date2 can be equal in very few cases
            // eg. this = 195- March
            // and dateToGo = September.
            if (date1.equals(date2))
                return date1;
            else {
                Duration interval1 = date1.getIntervalTo(this);
                Duration interval2 = date2.getIntervalFrom(this);
                int c = interval1.compareTo(interval2);
                if (c == -1)
                    return date1;
                else
                    return date2;
            }
        } else {
            return this.closest(dateToGo, 1);
        }
    }
    
    
    
    /**
     * Gets the closest date from the current one, corresponding to the defined elements
     * of the specified date.
     * direction = 2 if looking for the closest in the FUTURE and for a date strictly AFTER the current one
     * direction = 1 if looking for the closest in the FUTURE and for a date AFTER, INCLUDED or EQUAL to the current one
     * direction = -2 if looking for the closest in the PAST and for a date strictly AFTER the current one
     * direction = -1 if looking for the closest in the PAST and for a date AFTER, INCLUDED or EQUAL to the current one
     * @param dateToGo the date to reach
     * @param direction the direction
     * @return the new date.
     * @see #getClosestFrom(VagueDate dateFrom, int direction)
     */
    public abstract VagueDate closest(VagueDate dateToGo, int direction);
    
    
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
    public abstract VagueDate getClosestFrom(VagueDate dateFrom, int direction);
    
    
    /**
     * Completes the date with the century if it is missing. For example,
     * when only the decade is given (60s...). DOES NOT update the current date but returns
     * the result.
     * @param century the century to complete with
     * @return the completed date.
     */
    public abstract VagueDate completeCentury(int century);
    
    
    /**
     * Replaces elements of the current date that are lower than current granularity
     * by some fake elements
     * DOES NOT update the current date but returns the result.
     * @return the completed date. The completed date is absolute.
     */
    public abstract VagueDate completeByForce();
    
    
    /**
     * Completes the missing elements in the date, using the elements of
     * the specified date. DOES NOT update the current date but returns
     * the result.
     * @param date the date to use in order to complete the current one.
     * @return the completed date.
     */
    public abstract VagueDate completeDate(VagueDate date);
    
    
    /**
     * Tests if the century is missing in the date.
     * @return true whether the century is missing in the date, false otherwise
     */
    public abstract boolean isCenturyMissing();
    
    
    /**
     * Test whether two dates are equal. Caution! This method compares also
     * associated XipNodes. This means that two same dates can not be equal if
     * they correspond to two different nodes. To test only the equality of
     * dates in the calendar, independently from the sentence-level, use
     * {@link #compareTo(TemporalUnit o)}.
     * @param e the object to compare.
     * @return true whether the object is equal to the current element.
     */
    public boolean equals(Object e){
        if (e.getClass().equals(this.getClass())){
            VagueDate date = (VagueDate)e;
            if (this.getXipNode() != null && date.getXipNode() != null)
                return super.equals(e);
            else if (this.getXipNode() == null && date.getXipNode() == null) {
                if (this.softEquals(date))
                    return true;
                else
                    return super.equals(e);
            } else
                return false;
        } else
            return false;
    }
    
    
    /**
     * Test whether two dates have exactly the same date components.
     * We don't care about XipNodes or unspecified elements.
     * @param date the date to compare.
     * @return true whether the object is equal to the current element.
     */
    public abstract boolean softEquals(VagueDate date);
    
    /**
     * Compares this object with the specified object for order.
     * Use {@link TemporalRelation} temporal constants instead of negative/positive values.
     *
     * @param o the temporal unit to compare with the current one
     * @return an int value representing the temporal relation.
     */
    public abstract int compareTo(TemporalUnit o);
    
    
    /**
     * Returns an English description of the Date.
     * @return an English description of the Date.
     */
    public abstract String toString();
    
    /**
     * Returns a String formated representation of the date.
     * @return a String formated representation of the date.
     */
    public abstract String getValue();
    
    /**
     * Tests if the Date is defined.
     * @return true whether the Date is defined, false otherwise.
     */
    public abstract boolean isDefined();
    
    /**
     * Tests if the Date is absolute. A Date is absolute if its year value is fully defined.
     * @return true whether the Date is absolute, false otherwise
     */
    public abstract boolean isAbsolute();
    
    
    
    
    /**
     * Adds a Duration to a Date.
     * @param duration - the Duration to add to the current Date
     * @return the resulting VagueDate
     */
    public abstract VagueDate add(Duration duration);
    
    /**
     * Substracts a Duration to a Date.
     * @param duration - the Duration to substract to the current Date
     * @return the resulting VagueDate
     */
    public abstract VagueDate substract(Duration duration);
    
    /**
     * Gets the date after fuzzyfication, according to the specified margin duration
     * @param margin - the margin duration (before and after the "precise" date)
     * @return an interval or a set of intervals corresponding to the fuzzy Date.
     */
    public abstract VagueDate getFuzzyDate(SimpleDuration margin);
    
    
    /**
     * Gets the VagueSimpleDate representing the lower bound of current date
     * @return the VagueSimpleDate representing the lower bound of current date
     */
    public abstract VagueSimpleDate getLowerBound();
    
    
    /**
     * Gets the VagueSimpleDate representing the upper bound of current date
     * @return the VagueSimpleDate representing the upper bound of current date
     */
    public abstract VagueSimpleDate getUpperBound();
    
    
    /**
     * Gets the Duration holding between the lower bound and the upper bound of the date
     * @return the Duration holding between the lower bound and the upper bound of the date
     */
    public abstract SimpleDuration getAssociatedDuration();
    
    
    /**
     * Get a "real" date from the current Date. This means that underspecified elements
     * like afternoon, Winter, etc. will be changed into intervals of time
     *
     * @return a "real" VagueDate.
     */
    public abstract VagueDate getRealDate();
    
    /**
     * Selects in the current VagueDate the dates corresponding to daytime.
     * Daytime is from 7 AM to 6 PM.
     *
     * @return the dates corresponding to daytime.
     */
    public abstract VagueDate getOnlyDayTime();
    
    /**
     * Gets a clone
     * @return a clone.
     */
    public abstract Object clone() throws CloneNotSupportedException;
    
    /**
     * Tests if a String corresponds to a Date format
     * @param date - the String format to be tested.
     * @return <code>true</code> whether the String parameter is a date format,
     *  <code>false</code> otherwise
     */
    public static boolean isDateFormat(String date){
        return date.matches("[+=-]........T.......O...W...");
    }
    
    /**
     * Returns the String value for an non-defined date.
     * @return the String value for an non-defined date.
     */
    public static String getEmptyValue() {
        return "---------T-------O---W---";
    }
    
    
    /**
     * Compare dates, starting at a certain granularity. Consider that all lower granularities
     * in both dates are equal.
     * Recursively called by compareTo().
     * @param date the date to compare
     * @param granularity the granularity where to begin the comparison
     * @return the int value of the comparison.
     */
    public abstract int compareLevels(VagueDate date, int granularity);
    
    
    /**
     * Tests if the date has the specified granularity. This is true
     * whether the granularity is equal or higher than the argument.
     * @param granularity the test granularity
     * @return true whether the date has the specified granularity (or higher), false otherwise.
     */
    public abstract boolean hasGranularity(int granularity);
    
    
    /**
     * Changes the date granularity. It's only possible to attribute a lower granularity.
     * Special behaviour for granularity WEEK. DOES NOT modify the original date, only returns
     * a new one.
     * @param granularity the new granularity
     * @return the new date
     * @throws LostGranularityException if trying to assign a higher granularity than the current one.
     */
    public abstract VagueDate changeGranularity(int granularity) throws LostGranularityException;
    
    
    /**
     * Returns a VagueDate representing the current day
     */
    public static VagueSimpleDate getToday(){
        GregorianCalendar today = new GregorianCalendar();
        VagueSimpleDate date = new VagueSimpleDate(today);
        date.setGranularity(TemporalConstituent.DAY);
        return date;
    }
    
    public abstract void checkFullDate();
    
    public static void main(String[] a){
        try{
            VagueDate date = getNewDate("+20060711T-------O---W---|+20060715T-------O---W---");
            Duration duration = Duration.getNewDuration("P0Y0M0W10DT0H0M0S");
//            duration.setMod(EXACT);
            System.err.println("Date:     " + date.toString());
            System.err.println("Duration: " + duration.toString());
            VagueDate date2 = date.add(duration);
            VagueDate date3 = date.substract(duration);
            System.err.println("New date (addition):     " + date2.toString());
            System.err.println("New date (substrac):     " + date3.toString());
            System.err.println("Old date:     " + date.toString());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
}
