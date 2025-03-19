/*
 * VagueInterval.java
 *
 * Created on December 8, 2006, 9:01 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.date;

import common.CommonLog;
import common.NotImplementedException;
import documentAnalysis.temporality.LostGranularityException;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;
import documentAnalysis.temporality.vaguecalendar.duration.Days;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import documentAnalysis.temporality.vaguecalendar.duration.DurationInterval;
import documentAnalysis.temporality.vaguecalendar.duration.DurationUnit;
import documentAnalysis.temporality.vaguecalendar.duration.Hours;
import documentAnalysis.temporality.vaguecalendar.duration.Minutes;
import documentAnalysis.temporality.vaguecalendar.duration.Months;
import documentAnalysis.temporality.vaguecalendar.duration.Seconds;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;
import java.util.GregorianCalendar;

/**
 *
 * @author xtannier
 */
public class VagueInterval extends VagueDate {
    
    private VagueSimpleDate dateBegin;
    private VagueSimpleDate dateEnd;
    
    /**
     * Creates a new instance of VagueInterval
     */
    public VagueInterval() {
        this.dateBegin = new VagueSimpleDate();
        this.dateEnd = new VagueSimpleDate();
    }
    
    /**
     * Creates a new instance of VagueInterval with begin and end dates.
     *
     * @param dateBegin the lower bound date
     * @param dateEnd the upper bound date
     */
    public VagueInterval(VagueSimpleDate dateBegin, VagueSimpleDate dateEnd) throws IntervalException {
        if (dateBegin == null || dateEnd == null)
            throw new NullPointerException();
        if (!dateBegin.isDefined() || !dateEnd.isDefined())
            throw new NullPointerException();
        int c = dateBegin.compareTo(dateEnd);
        if (c != TemporalRelation.BEFORE && c != TemporalRelation.EQUALS && c != TemporalRelation.UNDEFINED)
            throw new IntervalException(dateBegin, dateEnd);
        this.dateBegin = dateBegin;
        this.dateEnd = dateEnd;
    }
    
    /**
     *
     * @param dateFormat
     * @throws documentanalysis.temporality.vaguecalendar.BadFormatException
     */
    public VagueInterval(String dateFormat) throws BadFormatException {
        String[] dates = dateFormat.split("/");
        // a length greater than 2 is possible:
        //  [...] pursued in 1982-1983 to 1984-1985 .
        this.dateBegin = new VagueSimpleDate(dates[0]);
        this.dateEnd = new VagueSimpleDate(dates[dates.length-1]);
    }
    
    
    public Object clone() throws CloneNotSupportedException {
        VagueInterval result = new VagueInterval();
        try {
            result.dateBegin = (VagueSimpleDate)this.dateBegin.clone();
            result.dateEnd = (VagueSimpleDate)this.dateEnd.clone();
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
        }
        return result;
    }
    
    
    /**
     * Copy a specified simple date
     * @param date the VagueSimpleDate to copy
     */
    public void copy(VagueSimpleDate date){
        this.dateBegin.copy(date);
        this.dateEnd.copy(date);
    }
    
    /**
     * Gets the granularity of the date.
     * @return the int value of the granularity
     */
    public int getGranularity(){
        return java.lang.Math.min(this.dateBegin.getGranularity(), this.dateEnd.getGranularity());
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
    public Duration getGranularityDuration(){
        try {
            int granularity = this.getGranularity();
            return new SimpleDuration(DurationUnit.getDurationUnit(granularity, "X"));
        } catch (BadFormatException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    /**
     * Tests if the Date is defined.
     * @return true whether the Date is defined, false otherwise.
     */
    public boolean isDefined(){
        return dateBegin.isDefined() || dateEnd.isDefined();
    }
    

    /**
     * Tests if the Date is absolute. A Date is absolute if its year value is fully defined.
     * @return true whether the Date is absolute, false otherwise
     */
    public boolean isAbsolute(){
        return dateBegin.isAbsolute() || dateEnd.isAbsolute();
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
        else if (this.compareTo(date) != TemporalRelation.BEFORE) {
            if (compare == TemporalRelation.AFTER)
                throw new NotImplementedException("This operation cannot be performed if the first date is AFTER the second one!");
            else
                return new SimpleDuration(new Seconds(0));
        } else {
            try{
                if (date instanceof VagueSimpleDate) {
                    VagueSimpleDate sdate = (VagueSimpleDate)date;
                    SimpleDuration interval1 = (SimpleDuration)this.getDateBegin().getIntervalTo(sdate);
                    SimpleDuration interval2 = (SimpleDuration)this.getDateEnd().getIntervalTo(sdate);
                    return new DurationInterval(interval1, interval2);
                } else if (date instanceof VagueInterval) {
                    VagueInterval idate = (VagueInterval)date;
                    SimpleDuration interval1 = (SimpleDuration)this.getDateBegin().getIntervalTo(idate.getDateBegin());
                    SimpleDuration interval2 = (SimpleDuration)this.getDateEnd().getIntervalTo(idate.getDateEnd());
                    return new DurationInterval(interval1, interval2);
                } else if (date instanceof VagueMultipleDate) {
                    return date.getIntervalFrom(this);
                } else {
                    throw new NotImplementedException("Unknown type of date");
                }
            } catch (BadFormatException e) {
                e.printStackTrace();
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
        else if (this.compareTo(date) != TemporalRelation.AFTER) {
            if (compare == TemporalRelation.BEFORE)
                throw new NotImplementedException("This operation cannot be performed if the first date is BEFORE the second one!");
            else
                return new SimpleDuration(new Seconds(0));
        } else {
            try{
                if (date instanceof VagueSimpleDate) {
                    VagueSimpleDate sdate = (VagueSimpleDate)date;
                    SimpleDuration interval1 = (SimpleDuration)this.getDateBegin().getIntervalFrom(sdate);
                    SimpleDuration interval2 = (SimpleDuration)this.getDateEnd().getIntervalFrom(sdate);
                    return new DurationInterval(interval1, interval2);
                } else if (date instanceof VagueInterval) {
                    VagueInterval idate = (VagueInterval)date;
                    SimpleDuration interval1 = (SimpleDuration)this.getDateBegin().getIntervalFrom(idate.getDateBegin());
                    SimpleDuration interval2 = (SimpleDuration)this.getDateEnd().getIntervalFrom(idate.getDateEnd());
                    return new DurationInterval(interval1, interval2);
                } else if (date instanceof VagueMultipleDate) {
                    return date.getIntervalTo(this);
                } else {
                    throw new NotImplementedException("Unknown type of date");
                }
            } catch (BadFormatException e) {
                e.printStackTrace();
                System.exit(-1);
                return null;
            }
        }
    }
    
    
    
    /**
     * Gets the closest date from the current one, corresponding to the defined elements
     * of the specified date.
     * direction = 2 if looking for the closest in the FUTURE and for a date strictly AFTER the current one
     * direction = 1 if looking for the closest in the FUTURE and for a date AFTER, INCLUDED or EQUAL to the current one
     * direction = -2 if looking for the closest in the PAST and for a date strictly AFTER the current one
     * direction = -1 if looking for the closest in the PAST and for a date AFTER, INCLUDED or EQUAL to the current one
     *
     * @param dateToGo the date to reach
     * @param direction the direction
     * @return the new date.
     * @see #getClosestFrom(VagueDate dateFrom, int direction)
     */
    public VagueDate closest(VagueDate dateToGo, int direction) {
        try{
            if (dateToGo.isAbsolute())
                return (VagueDate)dateToGo.clone();
            else if (direction == 2)
                return this.dateEnd.closest(dateToGo, direction);
            else if (direction == 1)
                return this.dateBegin.closest(dateToGo, direction);
            else if (direction == -2)
                return this.dateBegin.closest(dateToGo, direction);
            else if (direction == -1)
                return this.dateEnd.closest(dateToGo, direction);
            else
                throw new NotImplementedException();
        } catch(CloneNotSupportedException e){
            e.printStackTrace();
            System.exit(-1);
            return null;
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
        try{
            if (dateFrom instanceof VagueSimpleDate){
                // AFTER: gets the closest dateBegin and find the dateEnd afterwards
                if (direction > 0){
                    VagueSimpleDate resultDateBegin = dateFrom.closest(this.dateBegin, direction).getLowerBound();
                    VagueSimpleDate resultDateEnd = resultDateBegin.closest(this.dateEnd, direction).getUpperBound();
                    return new VagueInterval(resultDateBegin, resultDateEnd);
                } // BEFORE: gets the closest dateEnd and find the dateBegin afterwards
                else if (direction < 0){
                    VagueSimpleDate resultDateEnd = dateFrom.closest(this.dateEnd, direction).getLowerBound();
                    VagueSimpleDate resultDateBegin = resultDateEnd.closest(this.dateBegin, direction).getUpperBound();
                    return new VagueInterval(resultDateBegin, resultDateEnd);
                } else {
                    VagueSimpleDate resultDateBegin = dateFrom.closest(this.dateBegin, direction).getLowerBound();
                    VagueSimpleDate resultDateEnd = dateFrom.closest(this.dateEnd, direction).getUpperBound();
                    return new VagueInterval(resultDateBegin, resultDateEnd);
                }
            } else {
                throw new NotImplementedException();
            }
        } catch (IntervalException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
            
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
            VagueInterval result = (VagueInterval) this.clone();
            // if only the second date misses the century
            // complete with first date's century
            // eg 2007-12
            if (!result.dateBegin.isCenturyMissing() && result.dateEnd.isCenturyMissing()){
                result.dateEnd = result.dateEnd.completeCentury(result.dateBegin.getCentury()).getUpperBound();
                return result;
            } 
            // else complete both dates
            else {
                result.dateBegin = result.dateBegin.completeCentury(century).getLowerBound();
                result.dateEnd = result.dateEnd.completeCentury(century).getUpperBound();
                return result;
            }
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
            VagueInterval result = (VagueInterval) this.clone();
            result.dateBegin = result.dateBegin.completeByForce().getLowerBound();
            result.dateEnd = result.dateEnd.completeByForce().getUpperBound();
            return result;
            
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    /**
     * Completes the missing elements in the date, using the elements of
     * the specified date. DO NOT update the current date but returns
     * the result.
     *
     * @param date the date to use in order to complete the current one.
     * @return the completed date.
     */
    public VagueDate completeDate(VagueDate date){
        if (!this.isDefined())
            return date;
        else if (date instanceof VagueSimpleDate){
            try {
                return new VagueInterval(this.dateBegin.completeDate(date).getLowerBound(),
                        this.dateEnd.completeDate(date).getUpperBound());
            } catch (IntervalException ex) {
                // From 11 PM to 1 AM
                // this exception is raised but 1 AM must actually be the next day
                try{
                    if (this.hasGranularity(HOUR) && date.getGranularity() == DAY && date.isAbsolute()) {
                        VagueDate secondDate = (VagueDate)date.clone();
                        secondDate = secondDate.add(DAY, 1);
                        return new VagueInterval(this.dateBegin.completeDate(date).getLowerBound(),
                                this.dateEnd.completeDate(secondDate).getUpperBound());
                    } else {
                        CommonLog.warningMessage(ex.getMessage());
                        return VagueDate.getNewDate();
                    }
                } catch (IntervalException ex2) {
                    ex2.printStackTrace();
                    System.exit(-1);
                    return null;
                } catch (CloneNotSupportedException ex2) {
                    ex2.printStackTrace();
                    System.exit(-1);
                    return null;
                }
            }
        } else if (date instanceof VagueInterval){
            VagueInterval result = new VagueInterval();
            result.dateBegin = this.dateBegin.completeDate(((VagueInterval)date).dateBegin).getLowerBound();
            result.dateEnd   = this.dateEnd.completeDate(((VagueInterval)date).dateEnd).getUpperBound();
            return result;
        } else if (date instanceof VagueMultipleDate){
            return VagueMultipleDate.completeSimpleDateFromMultipleDate(this, (VagueMultipleDate)date);
        } else {
            throw new NotImplementedException();
        }
    }
    
    /**
     * Completes the missing elements of the specified simple <code>date</code>,
     * using the elements of the specified <code>interval</code>. Returns the result.
     * @param date the SimpleDate to complete
     * @param interval the interval to use for completion
     * @return the completed interval date.
     */
    static VagueInterval completeSimpleDateFromInterval(VagueSimpleDate date, VagueInterval interval){
        try {
            VagueSimpleDate resultBegin = (VagueSimpleDate)date.clone();
            VagueSimpleDate resultEnd = (VagueSimpleDate)date.clone();
            resultBegin = resultBegin.completeDate(interval.getDateBegin()).getLowerBound();
            resultEnd = resultEnd.completeDate(interval.getDateEnd()).getUpperBound();
            return new VagueInterval(resultBegin, resultEnd);
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        } catch (IntervalException ex) {
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
        return this.dateBegin.isCenturyMissing() || this.dateEnd.isCenturyMissing();
    }
    
    public VagueDate addYear(int i) {
        throw new NotImplementedException();
    }
    
    public VagueDate addMonth(int i) {
        throw new NotImplementedException();
    }
    
    public String getValue(){
        return this.dateBegin.getValue() + "/" + this.dateEnd.getValue();
    }
    
    
    public String toString() {
        return "From " + this.dateBegin + " to " + this.dateEnd;
    }
    
    public VagueDate add(int field, int amount) {
        try {
            VagueInterval result = (VagueInterval) this.clone();
            result.dateBegin = result.dateBegin.add(field, amount).getLowerBound();
            result.dateEnd = result.getUpperBound().add(field, amount).getUpperBound();
            return result;
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    /**
     * Merges two dates and keep the only date information compatible with both.
     *
     * @return a new date
     * @param date the date to merge with the current one
     * @throws documentanalysis.temporality.TemporalConsistencyException if a mergind is impossible
     */
    public VagueDate merge(VagueDate date) throws TemporalConsistencyException {
        if (date instanceof VagueSimpleDate)
            return date.merge(this);
        else if (date instanceof VagueInterval){
            VagueDate begin = this.dateBegin.merge(((VagueInterval)date).dateBegin);
            VagueDate end = this.dateEnd.merge(((VagueInterval)date).dateEnd);
            if (begin == null || end == null)
                return null;
            else {
                try{
                    VagueSimpleDate sbegin = begin.getLowerBound();
                    VagueSimpleDate send = end.getUpperBound();
                    
                    return new VagueInterval(sbegin, send);
                } catch (IntervalException ex) {
                    ex.printStackTrace();
                    System.exit(-1);
                    return null;
                }
            }
        } else if (date instanceof VagueMultipleDate){
            return date.merge(this);
        } else
            throw new NotImplementedException();
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
        return TemporalRelation.getInverseRelation(date.compareLevels(this, granularity));
    }
    
    
    /**
     * Test whether two dates have exactly the same date components.
     * We don't care about XipNodes or unspecified elements.
     * @param date the date to compare.
     * @return true whether the object is equal to the current element.
     */
    public boolean softEquals(VagueDate date) {
        if (date.getClass().equals(this.getClass())) {
            VagueInterval idate = (VagueInterval)date;
            return idate.dateBegin.softEquals(this.dateBegin) &&
                    idate.dateEnd.softEquals(this.dateEnd);
        } else
            return false;
    }    
    
    /**
     * Compares this object with the specified object for order.
     * Use {@link TemporalRelation} temporal constants instead of negative/positive values.
     */
    public int compareTo(TemporalUnit o) {
        if (o instanceof VagueInterval){
            VagueInterval interval = (VagueInterval)o;
            // BEFORE
            if (this.dateEnd.compareTo(interval.dateBegin) == TemporalRelation.BEFORE)
                return TemporalRelation.BEFORE;
            // MEETS (=> BEFORE)
            else if (this.dateEnd.compareTo(interval.dateBegin) == TemporalRelation.EQUALS)
                return TemporalRelation.BEFORE;
            // OVERLAPS
            else if (this.dateBegin.compareTo(interval.dateBegin) == TemporalRelation.BEFORE &&
                    this.dateEnd.compareTo(interval.dateEnd) == TemporalRelation.BEFORE &&
                    this.dateEnd.compareTo(interval.dateBegin) == TemporalRelation.AFTER)
                return TemporalRelation.OVERLAPS;
            // STARTS (=> DURING)
            else if (this.dateBegin.compareTo(interval.dateBegin) == TemporalRelation.EQUALS &&
                    this.dateEnd.compareTo(interval.dateEnd) == TemporalRelation.BEFORE)
                return TemporalRelation.DURING;
            // FINISHES (=> DURING)
            else if (this.dateEnd.compareTo(interval.dateEnd) == TemporalRelation.EQUALS &&
                    this.dateBegin.compareTo(interval.dateBegin) == TemporalRelation.AFTER)
                return TemporalRelation.DURING;
            // DURING
            else if (this.dateBegin.compareTo(interval.dateBegin) == TemporalRelation.AFTER &&
                    this.dateEnd.compareTo(interval.dateEnd) == TemporalRelation.BEFORE)
                return TemporalRelation.DURING;
            // EQUALS
            else if (this.dateBegin.compareTo(interval.dateBegin) == TemporalRelation.EQUALS &&                
                    this.dateEnd.compareTo(interval.dateEnd) == TemporalRelation.EQUALS)
                return TemporalRelation.EQUALS;            
            // AFTER
            else if (this.dateBegin.compareTo(interval.dateEnd) == TemporalRelation.AFTER)
                return TemporalRelation.AFTER;            
            // IS_MET (=> AFTER)
            else if (this.dateBegin.compareTo(interval.dateEnd) == TemporalRelation.EQUALS)
                return TemporalRelation.AFTER;
            // OVERLAPPED_BY
            else if (this.dateBegin.compareTo(interval.dateBegin) == TemporalRelation.AFTER &&
                    this.dateEnd.compareTo(interval.dateEnd) == TemporalRelation.AFTER &&
                    this.dateBegin.compareTo(interval.dateEnd) == TemporalRelation.BEFORE)
                return TemporalRelation.OVERLAPPED_BY;
            // STARTED_BY (=> INCLUDES)
            else if (this.dateBegin.compareTo(interval.dateBegin) == TemporalRelation.EQUALS &&
                    this.dateEnd.compareTo(interval.dateEnd) == TemporalRelation.AFTER)
                return TemporalRelation.INCLUDES;            
            // FINISHED_BY (=> INCLUDES)
            else if (this.dateEnd.compareTo(interval.dateEnd) == TemporalRelation.EQUALS &&
                    this.dateBegin.compareTo(interval.dateBegin) == TemporalRelation.BEFORE)
                return TemporalRelation.INCLUDES;
            // INCLUDES
            else if (this.dateBegin.compareTo(interval.dateBegin) == TemporalRelation.BEFORE &&
                    this.dateEnd.compareTo(interval.dateEnd) == TemporalRelation.AFTER)
                return TemporalRelation.INCLUDES;            
            // UNDEFINED
            else
                return TemporalRelation.UNDEFINED;            
        } else if (o instanceof VagueSimpleDate){
            VagueSimpleDate date = (VagueSimpleDate)o;
            if (this.dateBegin.compareTo(date) == TemporalRelation.AFTER)
                return TemporalRelation.AFTER;
            else if (this.dateEnd.compareTo(date) == TemporalRelation.BEFORE)
                return TemporalRelation.BEFORE;
            else if (this.dateBegin.compareTo(date) == TemporalRelation.BEFORE &&
                    this.dateEnd.compareTo(date) == TemporalRelation.AFTER)
                return TemporalRelation.INCLUDES;
            else if (this.dateBegin.compareTo(date) == TemporalRelation.EQUALS)
                return TemporalRelation.INCLUDES;
            else if (this.dateEnd.compareTo(date) == TemporalRelation.EQUALS)
                return TemporalRelation.INCLUDES;
            else if (this.dateBegin.compareTo(date) == TemporalRelation.DURING &&
                    this.dateEnd.compareTo(date) == TemporalRelation.DURING)
                return TemporalRelation.DURING;
            else
                return TemporalRelation.UNDEFINED;
        } else if (o instanceof VagueMultipleDate){
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
        try{
            return new VagueInterval(this.dateBegin.changeGranularity(granularity).getLowerBound(),
                    this.dateEnd.changeGranularity(granularity).getUpperBound());
        } catch (IntervalException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    /**
     * Tests if the date has the specified granularity. This is true
     * whether the granularity is equal or higher than the argument.
     * @param granularity the test granularity
     * @return true whether the date has the specified granularity (or higher), false otherwise.
     */
    public boolean hasGranularity(int granularity){
        return this.dateBegin.hasGranularity(granularity) && this.dateEnd.hasGranularity(granularity);
    }
    
    
    
    /**
     * Adds a Duration to the current Date.
     * @param duration - the Duration to add to the current Date
     * @return the resulting VagueDate
     */
    public VagueDate add(Duration duration) {
        try {
            VagueInterval result = (VagueInterval) this.clone();
            VagueDate date1 = result.dateBegin.add(duration);
            if (date1 instanceof VagueSimpleDate)
                result.dateBegin = (VagueSimpleDate)date1;
            else if (date1 instanceof VagueInterval)
                result.dateBegin = ((VagueInterval)date1).dateBegin;
            else
                throw new NotImplementedException();
            
            VagueDate date2 = result.dateEnd.add(duration);
            if (date2 instanceof VagueSimpleDate)
                result.dateEnd = (VagueSimpleDate)date2;
            else if (date2 instanceof VagueInterval)
                result.dateEnd = ((VagueInterval)date2).dateEnd;
            else
                throw new NotImplementedException();
            
            if (result.dateBegin.equals(result.dateEnd))
                return dateBegin;
            else
                return result;
            
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    /**
     * Substracts a Duration to the current Date.
     * @param duration - the Duration to substract to the current Date
     * @return the resulting VagueDate
     */
    public VagueDate substract(Duration duration) {
        try {
            VagueInterval result = (VagueInterval) this.clone();
            VagueDate date1 = result.dateBegin.substract(duration);
            if (date1 instanceof VagueSimpleDate)
                result.dateBegin = (VagueSimpleDate)date1;
            else if (date1 instanceof VagueInterval)
                result.dateBegin = ((VagueInterval)date1).dateBegin;
            else
                throw new NotImplementedException();
            
            VagueDate date2 = result.dateEnd.substract(duration);
            if (date2 instanceof VagueSimpleDate)
                result.dateEnd = (VagueSimpleDate)date2;
            else if (date2 instanceof VagueInterval)
                result.dateEnd = ((VagueInterval)date2).dateEnd;
            else
                throw new NotImplementedException();
            
            if (result.dateBegin.equals(result.dateEnd))
                return result.dateBegin;
            else
                return result;
            
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    /**
     * Gets the beginning of the interval
     * @return the beginning of the interval
     */
    public VagueSimpleDate getDateBegin() {
        return dateBegin;
    }
    
    /**
     * Returns the end of the interval
     * @return the end of the interval
     */
    public VagueSimpleDate getDateEnd() {
        return dateEnd;
    }
    
    
    public VagueDate getFuzzyDate(SimpleDuration margin) {
        try {
            VagueSimpleDate date1 = (VagueSimpleDate) this.dateBegin.clone();
            VagueSimpleDate date2 = (VagueSimpleDate)this.dateEnd.clone();
            date1 = date1.substract(margin).getLowerBound();
            date2 = date2.add(margin).getUpperBound();
            VagueInterval result = new VagueInterval(date1, date2);
            return result;
            
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        } catch (IntervalException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    /**
     * Gets the VagueSimpleDate representing the lower bound of current date.
     * In the case of a VagueInterval, returns the lower bound of the interval
     * @return the lower bound of the interval
     */
    public VagueSimpleDate getLowerBound(){
        return this.dateBegin;
    }
    
    
    /**
     * Gets the VagueSimpleDate representing the upper bound of current date.
     * In the case of a VagueInterval, returns the upper bound of the interval
     * @return the upper bound of the interval
     */
    public VagueSimpleDate getUpperBound(){
        return this.dateEnd;
    }
    
    
    /**
     * Gets the Duration holding between the lower bound and the upper bound of the date.
     * @return the Duration holding between the lower bound and the upper bound of the date
     */
    public SimpleDuration getAssociatedDuration(){
        SimpleDuration result = new SimpleDuration();
        int millenniums = 0;
        int centuries = 0;
        int decades = 0;
        int years = 0;
        int months = 0;
        int days = 0;
        int hours = 0;
        int minutes = 0;
        int seconds = 0;
        // if "false" interval, where begin and end date are the same
        // return the associated duration of one of these dates
        if (this.dateBegin.compareTo(this.dateEnd) == TemporalRelation.EQUALS)
            return this.dateBegin.getAssociatedDuration();
        // year
        if (this.dateBegin.getYear().isDefined() && this.dateBegin.getYear().getIntValue() >= 0 &&
                this.dateEnd.getYear().isDefined() && this.dateEnd.getYear().getIntValue() >= 0)
            years = this.dateEnd.getYear().getIntValue() - this.dateBegin.getYear().getIntValue();
        // months
        if (this.dateBegin.getMonth().isDefined() && this.dateBegin.getMonth().getIntValue() >= 0 &&
                this.dateEnd.getMonth().isDefined() && this.dateEnd.getMonth().getIntValue() >= 0)
            months = this.dateEnd.getMonth().getIntValue() - this.dateBegin.getMonth().getIntValue();
        // days
        if (this.dateBegin.getDayOfMonth().isDefined() && this.dateBegin.getDayOfMonth().getIntValue() >= 0 &&
                this.dateEnd.getDayOfMonth().isDefined() && this.dateEnd.getDayOfMonth().getIntValue() >= 0)
            days = this.dateEnd.getDayOfMonth().getIntValue() - this.dateBegin.getDayOfMonth().getIntValue();
        // hours
        if (this.dateBegin.getHour().isDefined() && this.dateBegin.getHour().getIntValue() >= 0 &&
                this.dateEnd.getHour().isDefined() && this.dateEnd.getHour().getIntValue() >= 0)
            hours = this.dateEnd.getHour().getIntValue() - this.dateBegin.getHour().getIntValue();
        // minutes
        if (this.dateBegin.getMinute().isDefined() && this.dateBegin.getMinute().getIntValue() >= 0 &&
                this.dateEnd.getMinute().isDefined() && this.dateEnd.getMinute().getIntValue() >= 0)
            minutes = this.dateEnd.getMinute().getIntValue() - this.dateBegin.getMinute().getIntValue();
        // seconds
        if (this.dateBegin.getSecond().isDefined() && this.dateBegin.getSecond().getIntValue() >= 0 &&
                this.dateEnd.getSecond().isDefined() && this.dateEnd.getSecond().getIntValue() >= 0)
            seconds = this.dateEnd.getSecond().getIntValue() - this.dateBegin.getSecond().getIntValue();
        result.setYears(years);
        result.setMonths(new Months(months));
        result.setDays(new Days(days));
        result.setHours(new Hours(hours));
        result.setMinutes(new Minutes(minutes));
        result.setSeconds(new Seconds(seconds));
        return result;
    }
    
    
    /**
     * Get a "real" date from the current Date. This means that underspecified elements
     * like afternoon, Winter, etc. will be changed into intervals of time
     *
     * @return a "real" VagueDate.
     */
    public VagueDate getRealDate(){
        VagueDate date1 = this.dateBegin.getRealDate();
        VagueDate date2 = this.dateEnd.getRealDate();
        VagueSimpleDate result1;
        VagueSimpleDate result2;
        if (date1 instanceof VagueSimpleDate)
            result1 = (VagueSimpleDate)date1;
        else if (date1 instanceof VagueInterval)
            result1 = ((VagueInterval)date1).getLowerBound();
        else
            throw new NotImplementedException();
        
        if (date2 instanceof VagueSimpleDate)
            result2 = (VagueSimpleDate)date2;
        else if (date2 instanceof VagueInterval)
            result2 = ((VagueInterval)date2).getLowerBound();
        else
            throw new NotImplementedException();
        try {
            
            return new VagueInterval(result1, result2);
        } catch (IntervalException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    /**
     * Selects in the current VagueDate the dates corresponding to daytime.
     * Daytime is from 7 AM to 6 PM.
     *
     * @return the dates corresponding to daytime.
     */
    public VagueDate getOnlyDayTime(){
        if (this.getDateBegin().getOnlyDayTime() != null || this.getDateEnd().getOnlyDayTime() != null)
            return this;
        else
            return null;
    }
    
    /**
     * Gets an interval around a specified date, with an space of the specified duration
     *
     * @param pivot the pivot date
     * @param duration the semi-interval
     * @return the new date
     */
    public static VagueInterval getAroundDate(VagueDate pivot, Duration duration) {
        duration.setMod(EXACT);
        if (pivot instanceof VagueSimpleDate){
            VagueSimpleDate date1 = pivot.substract(duration).getLowerBound();
            VagueSimpleDate date2 = pivot.add(duration).getUpperBound();
            try{
                VagueInterval result = new VagueInterval(date1, date2);
                return result;
            } catch (IntervalException ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            }
        } else {
            throw new NotImplementedException();
        }
    }
    
    
    public void checkFullDate() {
        this.dateBegin.getFullDate();
        this.dateEnd.getFullDate();
    }
}
