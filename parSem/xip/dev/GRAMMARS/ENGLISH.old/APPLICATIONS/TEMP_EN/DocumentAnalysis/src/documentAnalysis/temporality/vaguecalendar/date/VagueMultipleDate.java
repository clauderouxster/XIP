/*
 * VagueMultipleDate.java
 *
 * Created on December 8, 2006, 9:15 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.date;

import common.Common;
import common.NotImplementedException;
import documentAnalysis.temporality.LostGranularityException;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import documentAnalysis.temporality.vaguecalendar.duration.DurationUnit;
import documentAnalysis.temporality.vaguecalendar.duration.Seconds;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;
import java.util.Vector;

/**
 *
 * @author xtannier
 */
public class VagueMultipleDate extends VagueDate{
    
    private String operator;
    private VagueDate[] dates;
    
    /** Creates a new instance of VagueMultipleDate */
    public VagueMultipleDate() {
        this.operator = "";
        dates = new VagueSimpleDate[1];
        dates[0] = new VagueSimpleDate();
    }
    
    /**
     *
     * @param dateFormat
     * @throws documentanalysis.temporality.vaguecalendar.BadFormatException
     */
    public VagueMultipleDate(String dateFormat) throws BadFormatException {
        String[] strDates;
        if (dateFormat.contains("|")){
            strDates = Common.getNoDuplicateArray(dateFormat.split("\\|"));
            this.dates = new VagueDate[strDates.length];
            for (int i = 0 ; i < strDates.length ; ++i){
                this.dates[i] = VagueDate.getNewDate(strDates[i]);
            }
            this.operator = "|";
        } else if (dateFormat.contains("#")){
            strDates = Common.getNoDuplicateArray(dateFormat.split("#"));
            this.dates = new VagueDate[strDates.length];
            for (int i = 0 ; i < strDates.length ; ++i){
                this.dates[i] = VagueDate.getNewDate(strDates[i]);
            }
            this.operator = "#";
        }
    }
    
    public Object clone() throws CloneNotSupportedException {
        VagueMultipleDate result = new VagueMultipleDate();
        result.operator = this.operator;
        result.dates = new VagueDate[this.dates.length];
        for (int i = 0 ; i < this.dates.length; ++i){
            result.dates[i] = (VagueDate)this.dates[i].clone();
        }
        return result;
    }
    
    
    /**
     * Copy a specified simple date
     * @param date the VagueSimpleDate to copy
     */
    public void copy(VagueSimpleDate date){
        this.operator = "#";
        this.dates = new VagueDate[1];
        this.dates[0].copy(date);
//        this.dates[1].copy(date);
    }
    
    
    /**
     * Gets the granularity of the date.
     * @return the int value of the granularity
     */
    public int getGranularity(){
        int result = this.dates[0].getGranularity();
        for (int i = 1 ; i < this.dates.length ; ++i)
            result = java.lang.Math.min(result, this.dates[i].getGranularity());
        return result;
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
            String format = "";
            for (int i = 1 ; i < this.dates.length ; ++i)
                format += this.dates[i].getIntervalTo(date).getValue() + this.operator;
            format = format.substring(0, format.length() - 1);
            try {
                return Duration.getNewDuration(format);
            } catch (BadFormatException ex) {
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
        else if (this.compareTo(date) != TemporalRelation.AFTER) {
            if (compare == TemporalRelation.BEFORE)
                throw new NotImplementedException("This operation cannot be performed if the first date is BEFORE the second one!");
            else
                return new SimpleDuration(new Seconds(0));
        } else {
            String format = "";
            for (int i = 1 ; i < this.dates.length ; ++i)
                format += this.dates[i].getIntervalFrom(date).getValue() + this.operator;
            format = format.substring(0, format.length() - 1);
            try{
                return Duration.getNewDuration(format);
            } catch (BadFormatException ex) {
                ex.printStackTrace();
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
     * @param dateToGo the date to reach
     * @param direction the direction
     * @return the new date.
     * @see #getClosestFrom(VagueDate dateFrom, int direction)
     */
    public VagueDate closest(VagueDate dateToGo, int direction) {
        try{
            VagueMultipleDate result = (VagueMultipleDate) this.clone();
            for (int i = 0 ; i < result.dates.length ; ++i){
                result.dates[i] = result.dates[i].closest(dateToGo, direction);
            }
            return result;
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
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
        try {
            VagueMultipleDate result = (VagueMultipleDate) this.clone();
            
            for (int i = 0 ; i < result.dates.length ; ++i){
                result.dates[i] = dateFrom.closest(result.dates[i], direction);
            }
            return result;
        } catch (CloneNotSupportedException ex) {
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
            VagueMultipleDate result = (VagueMultipleDate) this.clone();
            for (int i = 0 ; i < this.dates.length ; ++i){
                result.dates[i] = result.dates[i].completeCentury(century);
            }
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
            VagueMultipleDate result = (VagueMultipleDate)this.clone();
            for (int i = 0 ; i < result.dates.length ; ++i)
                result.dates[i] = result.dates[i].completeByForce();
            result.cleanDuplicates();
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
     * @param date the date to use in order to complete the current one.
     * @return the completed date.
     */
    public VagueDate completeDate(VagueDate date){
        try {
            VagueMultipleDate result = (VagueMultipleDate)this.clone();
            
            for (int i = 0 ; i < this.dates.length ; ++i){
                result.dates[i] = result.dates[i].completeDate(date);
            }
            result.cleanDuplicates();
            return result;
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    /**
     * Completes the missing elements of the specified simple <code>date</code>,
     * using the elements of the specified <code>MultipleDate</code>. Returns the result.
     *
     * @param date the SimpleDate to complete
     * @param multipleDate the date to use for completion
     * @return the completed multiple date.
     */
    static VagueMultipleDate completeSimpleDateFromMultipleDate(VagueSimpleDate date, VagueMultipleDate multipleDate){
        try {
            VagueMultipleDate result = new VagueMultipleDate();
            result.operator = multipleDate.operator;
            result.dates = new VagueDate[multipleDate.dates.length];
            for (int i = 0 ; i < multipleDate.dates.length ; ++i){
                result.dates[i] = ((VagueSimpleDate)date.clone()).completeDate(multipleDate.dates[i]);
            }
            result.cleanDuplicates();
            return result;
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    /**
     * Completes the missing elements of the specified simple <code>date</code>,
     * using the elements of the specified <code>VagueInterval</code>. Returns the result.
     *
     * @param date the VagueInterval to complete
     * @param multipleDate the date to use for completion
     * @return the completed multiple date.
     */
    static VagueMultipleDate completeSimpleDateFromMultipleDate(VagueInterval date, VagueMultipleDate multipleDate){
        try {
            VagueMultipleDate result = new VagueMultipleDate();
            result.operator = multipleDate.operator;
            result.dates = new VagueDate[multipleDate.dates.length];
            for (int i = 0 ; i < multipleDate.dates.length ; ++i){
                result.dates[i] = ((VagueInterval)date.clone()).completeDate(multipleDate.dates[i]);
            }
            result.cleanDuplicates();            
            return result;
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
        for (int i = 0 ; i < this.dates.length ; ++i){
            if (this.dates[i].isCenturyMissing())
                return true;
        }
        return false;
    }
    
    /**
     * Adds a specified number of years to the date.
     * @param i the number of years to add
     * @return the new VagueDate.
     */
    public VagueDate addYear(int i) {
        throw new NotImplementedException();
    }
    
    /**
     * Adds a specified number of months to the date.
     * @param i the number of months to add
     * @return the new VagueDate.
     */
    public VagueDate addMonth(int i) {
        throw new NotImplementedException();
    }
    
    
    

    public String toString() {
        String result = "";
        String operatorValue = "";
        if (this.operator.equals("|"))
            operatorValue = " or ";
        else if (this.operator.equals("#"))
            operatorValue = " and ";
        
        for (int i = 0 ; i < this.dates.length ; ++i){
            result += this.dates[i].toString();
            if (i < this.dates.length - 1)
                result += operatorValue;
        }
        return result;
    }
    
    
    
    /**
     * Tests if the Date is defined.
     * @return true whether the Date is defined, false otherwise.
     */
    public boolean isDefined(){
        for (VagueDate date : this.dates){
            if (date.isDefined())
                return true;
        }
        return false;
    }
    
    /**
     * Tests if the current Date is absolute. A MultipleDate is absolute if at least one year
     * is fully specified.
     * @return true whether the date is absolute, false otherwise
     */
    public boolean isAbsolute() {
        for (VagueDate date : this.dates){
            if (date.isAbsolute())
                return true;
        }
        return false;
    }
    
    
    /**
     * Returns a VagueDate array containing all dates present in the VagueMultipleDate
     * @return a VagueDate array containing all dates present in the VagueMultipleDate
     */
    public VagueDate[] getDates(){
        return this.dates;
    }
    
    /**
     * Gets the String formated representation of the current date.
     * @return the String formated representation of the current date.
     */
    public String getValue() {
        String result = "";
        for (int i = 0 ; i < this.dates.length ; ++i){
            result += this.dates[i].getValue();
            if (i < this.dates.length - 1)
                result += this.operator;
        }
        return result;
    }
    
    /**
     *
     * @param field
     * @param amount
     */
    public VagueDate add(int field, int amount) {
        throw new NotImplementedException();
    }
    
    
    
    /**
     * Merges two dates and keep the only date information compatible with both.
     *
     * @return a new date
     * @param date the date to merge with the current one
     * @throws documentanalysis.temporality.TemporalConsistencyException if a mergind is impossible
     */
    public VagueDate merge(VagueDate date) throws TemporalConsistencyException {
        try{
            int i;
            // check if the date is not already in the list of dates
            for (i = 0 ; i < this.dates.length ; ++i){
                if (dates[i].compareTo(date) == TemporalRelation.EQUALS)
                    return this;
            }
            
            // AND
            if (this.operator.equals("#")){
                boolean notNull = false;
                VagueMultipleDate result = (VagueMultipleDate)this.clone();
                for (i = 0 ; i < result.dates.length ; ++i){
                    result.dates[i] = result.dates[i].merge(date);
                    if (result.dates[i] == null)
                        result.dates[i] = VagueDate.getNewDate();
                    else
                        notNull = true;
                }
                if (notNull){
                    result.cleanDuplicates();
                    return result;
                } else
                    return null;
            } // OR
            else if (this.operator.equals("|")){
                String format = "";
                boolean notNull = false;
                VagueDate auxResult;
                for (i = 0 ; i < this.dates.length ; ++i){
                    // if a date generates a temporal inconsistency, forget it
                    try{
                        auxResult = this.dates[i].merge(date);
                        if (auxResult == null)
                            auxResult = VagueDate.getNewDate();
                        else notNull = true;
                        format += "|" + auxResult.getValue();
                    } catch (TemporalConsistencyException e) {
                        continue;
                    }
                }
                if (format.equals(""))
                    throw new TemporalConsistencyException();
                else
                    format = format.substring(1);
                if (notNull)
                    return VagueDate.getNewDate(format);
                else
                    return null;
            } else
                throw new NotImplementedException();
        } catch(CloneNotSupportedException e) {
            e.printStackTrace();
            System.exit(-1);
            return null;
        } catch(BadFormatException e) {
            e.printStackTrace();
            System.exit(-1);
            return null;
        }
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
            VagueMultipleDate mdate = (VagueMultipleDate)date;
            if (mdate.dates.length != this.dates.length || !mdate.operator.equals(this.operator))
                return false;
            else {
                for (int i = 0 ; i < this.dates.length ; ++i) {
                    if (!this.dates[i].softEquals(mdate.dates[i]))
                        return false;
                }
                return true;
            }
        } else
            return false;
    }        
    
    /**
     * Compares this object with the specified object for order.
     * Use {@link TemporalRelation} temporal constants instead of negative/positive values.
     */
    public int compareTo(TemporalUnit o) {
        
        // equality ?
        if (o instanceof VagueMultipleDate){
            VagueMultipleDate date = (VagueMultipleDate)o;
            if (this.dates.length == date.dates.length){
                boolean equality=true;
                for (int i = 0 ; i < this.dates.length && equality ; ++i){
                    if (this.dates[i].compareTo(date.dates[i]) != TemporalRelation.EQUALS)
                        equality=false;
                }
                if (equality)
                    return TemporalRelation.EQUALS;
            }
        }
        // other relations...
        int currentRelation = TemporalRelation.getAllRelations();
        if (o instanceof VagueDate){
            VagueDate date = (VagueDate)o;
            for (int i = 0 ; i < this.dates.length && currentRelation != 0 ; ++i){
                int relation = this.dates[i].compareTo(date);
                if (relation == TemporalRelation.UNDEFINED)
                    currentRelation = 0;
                else
                    currentRelation = relation & currentRelation;
//                if (relation != TemporalRelation.AFTER)
//                    allAfter = false;
//                if (relation != TemporalRelation.BEFORE)
//                    allBefore = false;
//                if (relation != TemporalRelation.OVERLAPS)
//                    allOverlaps = false;
            }
            if (currentRelation == 0)
                return TemporalRelation.UNDEFINED;
            else
                return currentRelation;
//            if (allAfter)
//                return TemporalRelation.AFTER;
//            else if (allBefore)
//                return TemporalRelation.BEFORE;
//            else if (allOverlaps)
//                return TemporalRelation.OVERLAPS;
//            else
//                return TemporalRelation.UNDEFINED;
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
        try {
            VagueMultipleDate result = (VagueMultipleDate)this.clone();
            for (int i = 0 ; i < result.dates.length ; ++i)
                result.dates[i] = result.dates[i].changeGranularity(granularity);
            return result;
        } catch (CloneNotSupportedException ex) {
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
        for (int i = 0 ; i < this.dates.length ; ++i){
            if (!this.dates[i].hasGranularity(granularity))
                return false;
        }
        return true;
    }
    
    
    
    /**
     * Adds a Duration to the current Date.
     * @param duration - the Duration to add to the current Date
     * @return the resulting VagueDate
     */
    public VagueDate add(Duration duration) {
        try {
            VagueMultipleDate result = (VagueMultipleDate)this.clone();
            for (int i = 0 ; i < result.dates.length ; ++i){
                result.dates[i] = result.dates[i].add(duration);
            }
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
            VagueMultipleDate result = (VagueMultipleDate)this.clone();
            for (int i = 0 ; i < result.dates.length ; ++i){
                result.dates[i] = result.dates[i].substract(duration);
            }
            return result;
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    public VagueDate getFuzzyDate(SimpleDuration margin) {
        throw new NotImplementedException();
    }
    
    
    
    /**
     * Gets the VagueSimpleDate representing the lower bound of current date.
     * In the case of a MultipleDate, returns the earlier date of all components.
     * @return the earlier date of all components.
     */
    public VagueSimpleDate getLowerBound(){
        VagueSimpleDate currentLowerBound = this.dates[0].getLowerBound();
        for (int i = 1 ; i < this.dates.length ; ++i){
            VagueSimpleDate lowerBound = this.dates[i].getLowerBound();
            int c = lowerBound.compareTo(currentLowerBound);
            if (c == TemporalRelation.BEFORE || c == TemporalRelation.INCLUDES)
                currentLowerBound = lowerBound;
        }
        return currentLowerBound;
    }
    
    
    /**
     * Gets the VagueSimpleDate representing the lower bound of current date.
     * In the case of a MultipleDate, returns the later date of all components.
     * @return the later date of all components.
     */
    public VagueSimpleDate getUpperBound(){
        VagueSimpleDate currentUpperBound = this.dates[0].getUpperBound();
        for (int i = 1 ; i < this.dates.length ; ++i){
            VagueSimpleDate upperBound = this.dates[i].getUpperBound();
            int c = upperBound.compareTo(currentUpperBound);
            if (c == TemporalRelation.AFTER || c == TemporalRelation.INCLUDES)
                currentUpperBound = upperBound;
        }
        return currentUpperBound;
    }
    
    
    
    /**
     * Gets the Duration holding between the lower bound and the upper bound of the date.
     * @return the Duration holding between the lower bound and the upper bound of the date
     */
    public SimpleDuration getAssociatedDuration(){
        VagueSimpleDate lowerBound = this.getLowerBound();
        VagueSimpleDate upperBound = this.getUpperBound();
        VagueInterval interval;
        try {
            interval = new VagueInterval(lowerBound, upperBound);
        } catch (IntervalException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
        return interval.getAssociatedDuration();
    }
    
    
    /**
     * Get a "real" date from the current Date. This means that underspecified elements
     * like afternoon, Winter, etc. will be changed into intervals of time
     *
     * @return a "real" VagueDate.
     */
    public VagueDate getRealDate(){
        String format = "";
        for (int i = 0 ; i < this.dates.length ; ++i){
            VagueDate newDate = this.dates[i].getRealDate();
            if (newDate != null)
                format += newDate.getValue() + this.operator;
        }
        if (format.equals(""))
            return null;
        else {
            format = format.substring(0, format.length() - 1);
            try {
                return VagueDate.getNewDate(format);
            } catch (BadFormatException ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            }
        }
    }
    
    
    /**
     * Selects in the current VagueDate the dates corresponding to daytime.
     * Daytime is from 7 AM to 6 PM.
     *
     * @return the dates corresponding to daytime.
     */
    public VagueDate getOnlyDayTime(){
        String format = "";
        for (int i = 0 ; i < this.dates.length ; ++i){
            VagueDate newDate = this.dates[i].getOnlyDayTime();
            if (newDate != null)
                format += newDate.getValue() + this.operator;
        }
        if (format.equals(""))
            return null;
        else {
            format = format.substring(0, format.length() - 1);
            try {
                return VagueDate.getNewDate(format);
            } catch (BadFormatException ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            }
        }
    }
    

    /**
     * Removes duplicates from the current multiple date
     */
    public void cleanDuplicates(){
        Vector<VagueDate> newDates = new Vector<VagueDate>();
        
        for (int i = 0 ; i < this.dates.length ; ++i) {
            boolean addIt = true;
            for (VagueDate date : newDates) {
                if (date.softEquals(this.dates[i])) {
                    addIt = false;
                    break;
                }
            }
            if (addIt) {
                if (this.dates[i] instanceof VagueMultipleDate && 
                        ((VagueMultipleDate)this.dates[i]).dates.length == 1)
                    newDates.add(((VagueMultipleDate)this.dates[i]).dates[0]);
                else
                    newDates.add(this.dates[i]);
            }
        }
        
        this.dates = newDates.toArray(new VagueDate[0]);
    }
    
    public void checkFullDate() {
        for (int i = 0 ; i < this.dates.length ; ++i)
            this.dates[i].checkFullDate();
    }
    
    public static void main(String[] a){
        String format = "---------T02-----O---W---|---------T14-----O---W---";
        try {
            VagueDate date = VagueDate.getNewDate(format);
            System.err.println(date);
            VagueDate newDate = date.getOnlyDayTime();
            System.err.println(newDate);
        } catch (BadFormatException ex) {
            ex.printStackTrace();
        }
        
    }
}
