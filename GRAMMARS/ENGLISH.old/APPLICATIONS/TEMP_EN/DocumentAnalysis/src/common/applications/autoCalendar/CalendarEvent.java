/*
 * CalendarEvent.java
 *
 * Created on July 5, 2007, 3:29 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.applications.autoCalendar;

import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.vaguecalendar.date.VagueSimpleDate;
import documentAnalysis.temporality.vaguecalendar.duration.Hours;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;
import java.util.GregorianCalendar;

/**
 * A calendar event
 */
public class CalendarEvent {
    private VagueSimpleDate start;
    private VagueSimpleDate end;
    private String what;
    private String with;
    private String where;
    private String description;
    private long sentenceNumber;
//        private SimpleD duration;  // in minutes
    
    
    public CalendarEvent(long sentenceNumber){
        this(false, sentenceNumber);
    }
    
    
    public CalendarEvent(boolean defaultToday, long sentenceNumber){
        this.setSentenceNumber(sentenceNumber);
        if (defaultToday) {
            GregorianCalendar today = new GregorianCalendar();
            VagueSimpleDate dct = new VagueSimpleDate(today);
            this.setStart(dct);
            this.setEnd(dct);
        }
    }
    
    public CalendarEvent(CalendarEvent calendarEvent) {
        this.setStart(calendarEvent.getStart());
        this.setEnd(calendarEvent.getEnd());
//            this.setDuration(calendarEvent.getDuration());
    }
    
    public VagueSimpleDate getStart() {
        return start;
    }
    
    public void setStart(VagueSimpleDate start) {
        this.start = start;
    }
    
    public VagueSimpleDate getEnd() {
        return end;
    }
    
    public void setEnd(VagueSimpleDate end) {
        this.end = end;
    }
    
//        public SimpleDuration getDuration() {
//            return duration;
//        }
    
    public void setDuration(SimpleDuration duration) {
        duration.setMod(TemporalConstituent.EXACT);
        if (this.start != null){
            this.end = this.start.addSimpleDuration(duration).getUpperBound();
        } else if (this.end != null) {
            this.start = this.end.substract(duration).getLowerBound();
        }
    }
    
//    public String getStartString(){
//        String result;
//        if (start == null)
//            return "SOME_DATE";
//        result = String.valueOf(this.start.get(Calendar.YEAR));
//        result += "-" + this.start.get(Calendar.MONTH);
//        result += "-" + this.start.get(Calendar.DAY_OF_MONTH);
//        result += "T" + this.start.get(Calendar.HOUR_OF_DAY);
//        result += ":" + this.start.get(Calendar.MINUTE);
//        return result;
//    }
//
//    public String getEndString(){
//        String result;
//        if (end == null)
//            return "SOME_DATE";
//        result = String.valueOf(this.end.get(Calendar.YEAR));
//        result += "-" + this.end.get(Calendar.MONTH);
//        result += "-" + this.end.get(Calendar.DAY_OF_MONTH);
//        result += "T" + this.end.get(Calendar.HOUR_OF_DAY);
//        result += ":" + this.end.get(Calendar.MINUTE);
//        return result;
//    }
    
    
    public void complete() {
        // default duration = 1 hour
        if (this.end == null && this.start != null) {
            SimpleDuration duration = new SimpleDuration(new Hours(1));
            duration.setMod(TemporalConstituent.EXACT);
            this.end = this.start.addSimpleDuration(duration).getUpperBound();
        }
    }
    
    public String toString() {
        String result = "-----------------------------------------\n";
        result += "When:  " + "from " + this.getStart() + " to " + this.getEnd() + "\n";
        result += "What:  " + this.getWhat() + "\n";
        result += "Where: " + this.getWhere() + "\n";
        result += "With:  " + this.getWith() + "\n";
        result += "Description: " + this.getDescription() + "\n";
        result += "-----------------------------------------" + "\n";
        return result;
    }
    
    public String getWhat() {
        return what;
    }
    
    public void setWhat(String what) {
        this.what = what;
    }
    
    public String getWith() {
        return with;
    }
    
    public void setWith(String with) {
        this.with = with;
    }
    
    public String getDescription() {
        return description;
    }
    
    public void setDescription(String description) {
        this.description = description;
    }
    
    public String getWhere() {
        return where;
    }
    
    public void setWhere(String where) {
        this.where = where;
    }
    
    public long getSentenceNumber() {
        return sentenceNumber;
    }
    
    public void setSentenceNumber(long sentenceNumber) {
        this.sentenceNumber = sentenceNumber;
    }
}

