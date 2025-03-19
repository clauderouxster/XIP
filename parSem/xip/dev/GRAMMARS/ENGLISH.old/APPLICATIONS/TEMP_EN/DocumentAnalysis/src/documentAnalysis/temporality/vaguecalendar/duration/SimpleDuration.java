/*
 * SimpleDuration.java
 *
 * Created on December 13, 2006, 11:35 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.vaguecalendar.duration;

import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import common.CommonLog;
import common.NotImplementedException;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.TemporalUnit;

/**
 * This class implements methods for a simple duration.
 * the String representation of a simple Duration is:
 * PyYmMwWdDThHmMsS
 * @author xtannier
 */
public class SimpleDuration extends Duration{
    
    private Millenniums millenniums;
    private Centuries centuries;
    private Decades decades;
    private Years years;
    private Months months;
    private Weeks weeks;
    private Days days;
    private Hours hours;
    private Minutes minutes;
    private Seconds seconds;
    private int granularity;
    private boolean naturallyFuzzy;
    
    /**
     * Creates a new instance of SimpleDuration
     */
    public SimpleDuration() {
        this.setMillenniums(new Millenniums());
        this.setCenturies(new Centuries());
        this.setDecades(new Decades());
        this.setYears(new Years());
        this.setMonths(new Months());
        this.setWeeks(new Weeks());
        this.setDays(new Days());
        this.setHours(new Hours());
        this.setMinutes(new Minutes());
        this.setSeconds(new Seconds());
        this.granularity = UNDEFINED;
        this.naturallyFuzzy = true;
    }
    
    
    /**
     * Creates a new instance of VagueSimpleDate
     */
    public SimpleDuration(XipUnit unit, XipNode node) {
        super(unit, node);
        this.setMillenniums(new Millenniums());
        this.setCenturies(new Centuries());
        this.setDecades(new Decades());
        this.setYears(new Years());
        this.setMonths(new Months());
        this.setWeeks(new Weeks());
        this.setDays(new Days());
        this.setHours(new Hours());
        this.setMinutes(new Minutes());
        this.setSeconds(new Seconds());
        this.granularity = UNDEFINED;
        this.naturallyFuzzy = true;        
    }
    
    /**
     * Creates a SimpleDuration based on a DurationUnit
     * @param unit - the duration unit
     */
    public SimpleDuration(DurationUnit unit){
        this();
        if (unit instanceof Millenniums){
            this.setMillenniums((Millenniums) unit);
            this.granularity = MILLENNIUM;
        } else if (unit instanceof Centuries){
            this.setCenturies((Centuries) unit);
            this.granularity = CENTURY;
        } else if (unit instanceof Decades){
            this.setDecades((Decades) unit);
            this.granularity = DECADE;
        } else if (unit instanceof Years){
            this.setYears((Years) unit);
            this.granularity = YEAR;
        } else if (unit instanceof Months){
            this.setMonths((Months) unit);
            this.granularity = MONTH;
        } else if (unit instanceof Weeks){
            this.setWeeks((Weeks) unit);
            this.granularity = WEEK;
        } else if (unit instanceof Days){
            this.setDays((Days) unit);
            this.granularity = DAY;
        } else if (unit instanceof Hours){
            this.setHours((Hours) unit);
            this.granularity = HOUR;
        } else if (unit instanceof Minutes){
            this.setMinutes((Minutes) unit);
            this.granularity = MINUTE;
        } else if (unit instanceof Seconds){
            this.setSeconds((Seconds) unit);
            this.granularity = SECOND;
        }
        this.naturallyFuzzy = unit.getIntValue() == -1;        
    }
    
    
    /**
     *
     * @param durationFormat
     * @throws documentanalysis.temporality.vaguecalendar.BadFormatException
     */
    public SimpleDuration(String durationFormat) throws BadFormatException {
        int pos;
        String number;
        if (durationFormat.charAt(0) != 'P')
            throw new BadFormatException();
        
        // YEARS
        pos = durationFormat.indexOf('Y');
        if (pos == -1)
            throw new BadFormatException();
        number = durationFormat.substring(1, pos);
        // years
        this.setYears(new Years(number.substring(number.length() - 1)));
        // decades
        if (number.length() > 1)
            this.setDecades(new Decades(String.valueOf(number.charAt(number.length() - 2))));
        else
            this.setDecades(new Decades());
        // centuries
        if (number.length() > 2)
            this.setCenturies(new Centuries(String.valueOf(number.charAt(number.length() - 3))));
        else
            this.setCenturies(new Centuries());
        // millenniums
        if (number.length() > 3)
            this.setMillenniums(new Millenniums(String.valueOf(number.charAt(number.length() - 4))));
        else
            this.setMillenniums(new Millenniums());
        
        durationFormat = durationFormat.substring(pos + 1);
        
        // MONTHS
        pos = durationFormat.indexOf('M');
        if (pos == -1)
            throw new BadFormatException();
        number = durationFormat.substring(0, pos);
        this.setMonths(new Months(number));
        durationFormat = durationFormat.substring(pos + 1);
        
        // WEEKS
        pos = durationFormat.indexOf('W');
        if (pos == -1)
            throw new BadFormatException();
        number = durationFormat.substring(0, pos);
        this.setWeeks(new Weeks(number));
        durationFormat = durationFormat.substring(pos + 1);
        
        // DAYS
        pos = durationFormat.indexOf('D');
        if (pos == -1)
            throw new BadFormatException();
        number = durationFormat.substring(0, pos);
        this.setDays(new Days(number));
        durationFormat = durationFormat.substring(pos + 1);
        
        if (durationFormat.charAt(0) != 'T')
            throw new BadFormatException();
        
        // HOURS
        pos = durationFormat.indexOf('H');
        if (pos == -1)
            throw new BadFormatException();
        number = durationFormat.substring(1, pos);
        this.setHours(new Hours(number));
        durationFormat = durationFormat.substring(pos + 1);
        
        // MINUTES
        pos = durationFormat.indexOf('M');
        if (pos == -1)
            throw new BadFormatException();
        number = durationFormat.substring(0, pos);
        this.setMinutes(new Minutes(number));
        durationFormat = durationFormat.substring(pos + 1);
        
        // SECONDS
        pos = durationFormat.indexOf('S');
        if (pos == -1)
            throw new BadFormatException();
        number = durationFormat.substring(0, pos);
        this.setSeconds(new Seconds(number));
        durationFormat = durationFormat.substring(pos + 1);
        
        if (!durationFormat.equals(""))
            throw new BadFormatException();
        
        // granularity
        if (!this.seconds.isNull())
            this.granularity = SECOND;
        else if (!this.minutes.isNull())
            this.granularity = MINUTE;
        else if (!this.hours.isNull())
            this.granularity = HOUR;
        else if (!this.days.isNull())
            this.granularity = DAY;
        else if (!this.weeks.isNull())
            this.granularity = WEEK;
        else if (!this.months.isNull())
            this.granularity = MONTH;
        else if (!this.years.isNull())
            this.granularity = YEAR;
        else if (!this.decades.isNull())
            this.granularity = DECADE;
        else if (!this.centuries.isNull())
            this.granularity = CENTURY;
        else if (!this.millenniums.isNull())
            this.granularity = MILLENNIUM;
    }
    
    
    /**
     * Gets a clone
     * @return a clone.
     */
    public Object clone() throws CloneNotSupportedException{
        SimpleDuration result = new SimpleDuration();
        result.setMillenniums((Millenniums)this.millenniums.clone());
        result.setCenturies((Centuries)this.centuries.clone());
        result.setDecades((Decades)this.decades.clone());
        result.setYears((Years)this.years.clone());
        result.setMonths((Months)this.months.clone());
        result.setWeeks((Weeks)this.weeks.clone());
        result.setDays((Days)this.days.clone());
        result.setHours((Hours)this.hours.clone());
        result.setMinutes((Minutes)this.minutes.clone());
        result.setSeconds((Seconds)this.seconds.clone());
        result.granularity = this.granularity;
        result.setXipNode(this.getXipUnit(), this.getXipNode());
        return result;
    }
    
    
   
    
    
    /**
     * Returns the Millenniums of the duration.
     * @return the Millenniums of the duration.
     */
    public Millenniums getMillenniums() {
        return millenniums;
    }
    
    
    
    /**
     * Returns the Centuries of the duration.
     * @return the Centuries of the duration.
     */
    public Centuries getCenturies() {
        return centuries;
    }
    
    /**
     * Returns the Decades of the duration.
     * @return the Decades of the duration.
     */
    public Decades getDecades() {
        return decades;
    }
    
    
    
    
    
    /**
     * Returns the Years of the duration.
     * @return the Years of the duration.
     */
    public Years getYears() {
        return years;
    }
    
    /**
     * Returns the Months of the duration.
     * @return the Months of the duration.
     */
    public Months getMonths() {
        return months;
    }
    
    /**
     * Returns the Weeks of the duration.
     * @return the Weeks of the duration.
     */
    public Weeks getWeeks() {
        return weeks;
    }
    
    /**
     * Returns the Days of the duration.
     * @return the Days of the duration.
     */
    public Days getDays() {
        return days;
    }
    
    /**
     * Returns the Hours of the duration.
     * @return the Hours of the duration.
     */
    public Hours getHours() {
        return hours;
    }
    
    /**
     * Returns the Minutes of the duration.
     * @return the Minutes of the duration.
     */
    public Minutes getMinutes() {
        return minutes;
    }
    
    /**
     * Returns the Seconds of the duration.
     * @return the Seconds of the duration.
     */
    public Seconds getSeconds() {
        return seconds;
    }
    
    
    
    /**
     * Sets the Millennium unit duration
     * @param millenniums the millenniums of the duration
     */
    public void setMillenniums(Millenniums millenniums) {
        this.millenniums = millenniums;
        if (this.millenniums.getIntValue() == -1)
            this.naturallyFuzzy = true;
    }
    
    /**
     * Sets the Centuries unit duration
     * @param centuries the centuries of the duration
     */
    public void setCenturies(Centuries centuries) {
        this.centuries = centuries;
        if (this.centuries.getIntValue() == -1)
            this.naturallyFuzzy = true;        
    }
    
    /**
     * Sets the Decades unit duration
     * @param decades the decades of the duration
     */
    public void setDecades(Decades decades) {
        this.decades = decades;
        if (this.decades.getIntValue() == -1)
            this.naturallyFuzzy = true;        
    }
    
    /**
     * Sets the Year unit duration
     * @param years the years of the duration
     */
    public void setYears(Years years) {
        this.years = years;
        if (this.years.getIntValue() == -1)
            this.naturallyFuzzy = true;
        
    }
    
    /**
     * Sets the specified number of years in the duration.
     * This number can be spread from milleniums to years.
     *
     * @param years the number of years to set to the duration
     */
    public void setYears(int years) {
        try{
            if (years < 10)
                this.setYears(new Years(years));
            else if (years < 100){
                this.setYears(new Years(years % 10));
                this.setDecades(new Decades(years / 10));
            } else if (years < 1000){
                this.setYears(new Years(years % 10));
                this.setDecades(new Decades((years % 100) / 10));
                this.setCenturies(new Centuries(years / 100));
            } else {
                this.setYears(new Years(years % 10));
                this.setDecades(new Decades((years % 100) / 10));
                this.setCenturies(new Centuries((years % 1000) / 1000));
                this.setMillenniums(new Millenniums(years / 1000));
            }
            if (this.years.getIntValue() == -1)
                this.naturallyFuzzy = true;
            
        } catch (BadFormatException e) {
            e.printStackTrace();
            System.exit(-1);
        }
    }
    
    /**
     * Sets the Month unit duration
     * @param months the months of the duration
     */
    public void setMonths(Months months) {
        this.months = months;
        if (this.months.getIntValue() == -1)
            this.naturallyFuzzy = true;
        
    }
    
    /**
     * Sets the Week unit duration
     * @param weeks the weeks of the duration
     */
    public void setWeeks(Weeks weeks) {
        this.weeks = weeks;
        if (this.weeks.getIntValue() == -1)
            this.naturallyFuzzy = true;
        
    }
    
    /**
     * Sets the Day unit duration
     * @param days the days of the duration
     */
    public void setDays(Days days) {
        this.days = days;
        if (this.days.getIntValue() == -1)
            this.naturallyFuzzy = true;
        
    }
    
    /**
     * Sets the Hour unit duration
     * @param hours the hours of the duration
     */
    public void setHours(Hours hours) {
        this.hours = hours;
        if (this.hours.getIntValue() == -1)
            this.naturallyFuzzy = true;
        
    }
    
    /**
     * Sets the Minute unit duration
     * @param minutes the minutes of the duration
     */
    public void setMinutes(Minutes minutes) {
        this.minutes = minutes;
        if (this.minutes.getIntValue() == -1)
            this.naturallyFuzzy = true;        
    }
    
    /**
     * Sets the Second unit duration
     * @param seconds the seconds of the duration
     */
    public void setSeconds(Seconds seconds) {
        this.seconds = seconds;
        if (this.seconds.getIntValue() == -1)
            this.naturallyFuzzy = true;        
    }
    
    
    /**
     * Tests whether the Duration is defined.
     * @return true whether the Duration is defined, false otherwise.
     */
    public boolean isDefined() {
        return this.days.isDefined() || this.hours.isDefined() || this.minutes.isDefined() ||
                this.months.isDefined() || this.seconds.isDefined() || this.weeks.isDefined() ||
                this.years.isDefined() || this.decades.isDefined() || this.centuries.isDefined() ||
                this.millenniums.isDefined();
    }
    
    /**
     * Returns a String formated representation of the Duration.
     * @return a String formated representation of the Duration
     */
    public String getValue() {
        // years
        String years = mergeYearValues(this.millenniums.getValue(),
                this.centuries.getValue(), this.decades.getValue(), this.years.getValue());
        return "P" + years + this.months.getValue() + this.weeks.getValue() +
                this.days.getValue() + "T" + this.hours.getValue() + this.minutes.getValue() +
                this.seconds.getValue();
    }
    
    
    /**
     * Merges of values corresponding to years (from years to millunniums) to get a single
     * year value.
     *
     */
    private String mergeYearValues(String millenniums, String centuries, String decades, String years){
        String result = years;
        boolean addZero = false;
        // decades
        if (!decades.equals(""))
            result = decades.charAt(0) + result;
        else
            addZero = true;
        // centuries
        if (!centuries.equals("")){
            if (addZero)
                result = centuries.charAt(0) + "0" + result;
            else
                result = centuries.charAt(0) + result;
            addZero = false;
        } else
            addZero = true;
        // millenniums
        if (!millenniums.equals("")){
            if (addZero)
                result = millenniums.substring(0, millenniums.length() - 4) + "0" + result;
            else
                result = millenniums.charAt(0) + result;
        }
        return result;
    }
    
    /**
     * Returns an English description of the Duration.
     * @return an English description of the Duration
     */
    public String toString() {
        if (this.isDefined()){
            if (this.isNull())
                return "null_duration";
            else {
                String result = "";
                if (this.getMod() != TemporalConstituent.UNDEFINED)
                    result += TemporalConstituent.modTypeToString(this.getMod()) + " ";
                if (this.millenniums.isDefined() && !this.millenniums.isNull())
                    result += this.years.toString() + " ";
                if (this.centuries.isDefined() && !this.centuries.isNull())
                    result += this.centuries.toString() + " ";
                if (this.decades.isDefined() && !this.decades.isNull())
                    result += this.decades.toString() + " ";
                if (this.years.isDefined() && !this.years.isNull())
                    result += this.years.toString() + " ";
                if (this.months.isDefined() && !this.months.isNull())
                    result += this.months.toString() + " ";
                if (this.weeks.isDefined() && !this.weeks.isNull())
                    result += this.weeks.toString() + " ";
                if (this.days.isDefined() && !this.days.isNull())
                    result += this.days.toString() +  " ";
                if (this.hours.isDefined() && !this.hours.isNull())
                    result += this.hours.toString() +  " ";
                if (this.minutes.isDefined() && !this.minutes.isNull())
                    result += this.minutes.toString() +  " ";
                if (this.seconds.isDefined() && !this.seconds.isNull())
                    result += this.seconds.toString();
                return result;
            }
        } else
            return "SOME_DURATION";
    }
    
    
    /**
     * Tests if the Duration is null.
     * @return true wheter the Duration is null, false otherwise.
     */
    public boolean isNull() {
        return this.millenniums.isNull() && this.centuries.isNull() && this.decades.isNull() &&
                this.years.isNull() && this.months.isNull() && this.weeks.isNull() &&
                this.days.isNull() && this.hours.isNull() && this.minutes.isNull() &&
                this.seconds.isNull();
    }
    
    /**
     * Multiplies the SimpleDuration by the specified number. Multiplies actually all
     * units by this value.
     * @return a new SimpleDuration.
     */
    public SimpleDuration multiply(int i){
        SimpleDuration duration = this;
        duration.millenniums.multiply(i);
        duration.centuries.multiply(i);
        duration.decades.multiply(i);
        duration.years.multiply(i);
        duration.months.multiply(i);
        duration.weeks.multiply(i);
        duration.days.multiply(i);
        duration.hours.multiply(i);
        duration.minutes.multiply(i);
        duration.seconds.multiply(i);
        return duration;
    }
    


    /**
     * Compares this object with the specified object for order.
     * Use {@link documentAnalysis.temporality.TemporalRelation} temporal constants 
     * instead of negative/positive values.
     *
     * 
     */    
    public int compareTo(TemporalUnit o) {
        int c;
        if (o instanceof SimpleDuration) {
            SimpleDuration dur = (SimpleDuration)o;
            c = millenniums.compareTo(dur.millenniums);
            if (c != 0)
                return c;
            c = centuries.compareTo(dur.centuries);
            if (c != 0)
                return c;
            c = decades.compareTo(dur.decades);
            if (c != 0)
                return c;
            c = years.compareTo(dur.years);
            if (c != 0)
                return c;
            c = months.compareTo(dur.months);
            if (c != 0)
                return c;
            c = weeks.compareTo(dur.weeks);
            if (c != 0)
                return c;
            c = days.compareTo(dur.days);
            if (c != 0)
                return c;
            c = hours.compareTo(dur.hours);
            if (c != 0)
                return c;
            c = minutes.compareTo(dur.minutes);
            if (c != 0)
                return c;
            c = seconds.compareTo(dur.seconds);
            return c;
        } else if (o instanceof DurationInterval) {
            c = ((DurationInterval)o).compareTo(this);
            if (c == TemporalRelation.UNDEFINED)
                return c;
            else
                return -1 * c;
        } else if (o instanceof MultipleDuration) {
            c = ((MultipleDuration)o).compareTo(this);
            if (c == TemporalRelation.UNDEFINED)
                return c;
            else
                return -1 * c;
        } else
            throw new NotImplementedException();
    }
    
    
    /**
     * Returns an int number corresponding to the number of units
     * specified by the granularity.
     *
     * @param granularity the expected unit
     * @return an int representing the number of unit specified by the granularity paramater.
     */
    public int getValue(int granularity){
        int r = 0;
        if (granularity == MINUTE) {
            int aux = this.getMillenniums().getIntValue();
            if (aux != -1)
                r += aux * 1000 * 365 * 24 * 60;
            aux = this.getCenturies().getIntValue();
            if (aux != -1)
                r += aux * 100 * 365 * 24 * 60;
            aux = this.getDecades().getIntValue();
            if (aux != -1)
                r += aux * 10 * 365 * 24 * 60;
            aux = this.getYears().getIntValue();
            if (aux != -1)
                r += aux * 365 * 24 * 60;
            aux = this.getMonths().getIntValue();
            if (aux != -1)
                r += aux * 30 * 24 * 60;
            aux = this.getWeeks().getIntValue();
            if (aux != -1)
                r += aux * 7 * 24 * 60;
            aux = this.getDays().getIntValue();
            if (aux != -1)
                r += aux * 24 * 60;
            aux = this.getHours().getIntValue();
            if (aux != -1)
                r += aux * 60;
            aux = this.getMinutes().getIntValue();
            if (aux != -1)
                r += aux;
            return r;
        } else
            throw new NotImplementedException();
    }
    
    
    /**
     * Gets the granularity of the duration. The granularity is the value of the
     * lowest non-null unit.
     * @return the int value of the granularity.
     */
    public int getGranularity() {
        DurationUnit unit = this.getUnit(this.granularity);
        int approximateMaxNumber = TemporalConstituent.getApproximateMaxNumber(this.granularity);
        if (unit != null && unit.isDefined() &&
                approximateMaxNumber > 0 &&
                unit.getIntValue() >= approximateMaxNumber)
            return TemporalConstituent.getBiggerGranularity(this.granularity);
        else
            return granularity;
    }
    
    
    /**
     * Gets the DurationUnit of the current duration corresponding to the specified
     * granularity value
     *
     * @param granularity the expected granularity of the unit
     * @return the DurationUnit of the current duration corresponding to the specified
     * granularity value
     */
    public DurationUnit getUnit(int granularity){
        if (granularity == MILLENNIUM)
            return this.getMillenniums();
        else if (granularity == CENTURY)
            return this.getCenturies();
        else if (granularity == DECADE)
            return this.getDecades();
        else if (granularity == YEAR)
            return this.getYears();
        else if (granularity == MONTH)
            return this.getMonths();
        else if (granularity == WEEK)
            return this.getWeeks();
        else if (granularity == DAY)
            return this.getDays();
        else if (granularity == HOUR)
            return this.getHours();
        else if (granularity == MINUTE)
            return this.getMinutes();
        else if (granularity == SECOND)
            return this.getSeconds();
        else {
            throw new NotImplementedException("Granularity " + granularity + "unknown.");
        }
    }
    
    
    private SimpleDuration getFuzzyMarginFromDurationUnit(DurationUnit unit){
        int fg = unit.getFuzzyGranularity();
        int value = unit.getIntValue();
        int times = 1;
        SimpleDuration result;
        
        // "several" (value == -1)
        if (value == -1) {
            result = unit.getDuration();
        }
        // 1 unit
        else if (value == 1){
            result = unit.getQuarter().multiply(times);
        }
        // 2 units
        // or less than 2 * FG
        else if (value == 2 || value < (2 * fg)){
            result = unit.getHalf().multiply(times);
        }
        // not a multiple of FG
        else if (value % fg != 0){
            result = unit.getHalf().multiply(times);
        }
        // multiple of FG
        else {
            try {
                Class[] classes = new Class[] {Class.forName("java.lang.String")};
                DurationUnit newUnit = unit.getClass().getConstructor(classes).newInstance(new Object[] {String.valueOf(fg)});
                result = new SimpleDuration(newUnit.multiply(times));
            } catch (Exception ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            }
        }
        result.setMod(EXACT);
        return result;
    }
    
    
    public SimpleDuration getFuzzyMargin(){
        if (this.granularity == MILLENNIUM)
            return getFuzzyMarginFromDurationUnit(this.millenniums);
        else if (this.granularity == CENTURY)
            return getFuzzyMarginFromDurationUnit(this.centuries);
        else if (this.granularity == DECADE)
            return getFuzzyMarginFromDurationUnit(this.decades);
        else if (this.granularity == YEAR)
            return getFuzzyMarginFromDurationUnit(this.years);
        else if (this.granularity == MONTH)
            return getFuzzyMarginFromDurationUnit(this.months);
        else if (this.granularity == WEEK)
            return getFuzzyMarginFromDurationUnit(this.weeks);
        else if (this.granularity == DAY)
            return getFuzzyMarginFromDurationUnit(this.days);
        else if (this.granularity == HOUR)
            return getFuzzyMarginFromDurationUnit(this.hours);
        else if (this.granularity == MINUTE)
            return getFuzzyMarginFromDurationUnit(this.minutes);
        else if (this.granularity == SECOND)
            return getFuzzyMarginFromDurationUnit(this.seconds);
        else
            throw new NotImplementedException("Unknown granularity");
    }

    /**
     * Tests if the SimpleDuration is naturally fuzzy or not.
     * A naturally fuzzy SimpleDuration is a "several something" duration
     * (eg "several days")
     *
     * @return true if the SimpleDuration is naturally fuzzy, false otherwise.
     */
    public boolean isNaturallyFuzzy() {
        return naturallyFuzzy;
    }
    
    
    
}
