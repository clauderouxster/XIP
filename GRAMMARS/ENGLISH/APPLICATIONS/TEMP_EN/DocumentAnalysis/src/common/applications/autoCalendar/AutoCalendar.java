/*
 * AutoCalendar.java
 *
 * Created on July 5, 2007, 3:20 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.applications.autoCalendar;

import com.xerox.jxip.XipNode;
import common.NotImplementedException;
import common.Options;
import common.TypedPropertyException;
import common.applications.*;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.TemporalElement;
import documentAnalysis.temporality.TemporalGraph;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.events.Event;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import documentAnalysis.temporality.vaguecalendar.date.VagueInterval;
import documentAnalysis.temporality.vaguecalendar.date.VagueMultipleDate;
import documentAnalysis.temporality.vaguecalendar.date.VagueSimpleDate;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import documentAnalysis.temporality.vaguecalendar.duration.DurationInterval;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;
import java.util.Vector;

/**
 * Class for methods specific to Automatic calendar events finding
 * @author xtannier
 */
public class AutoCalendar extends Application {
    
    EventFrame eventFrame;
    
    /** Creates a new instance of AutoCalendar */
    public AutoCalendar(){
        super();
        this.name = "AutoCalendar";
    }
    
    /** Creates a new instance of AutoCalendar
     *
     * @param options the parer options
     */
    public AutoCalendar(Options options) {
        this();
        this.eventFrame = new EventFrame(options);
        this.eventFrame.setVisible(true);                
    }
    
    
    /**
     * Perform any appropriate operation and close the output file or connection
     * in the output.
     *
     * @param options the parser options.
     */
    public void closeOutput(Options options){
        TemporalGraph graph = options.getTemporalGraph();
        Vector<Event> events = graph.getEvents();
        Vector<VagueDate> dateCandidates = new Vector<VagueDate>();
        Vector<Duration> durationCandidates = new Vector<Duration>();
        
        for (Event event : events) {
            XipNode node = event.getXipNode();
            if (event.hasDuration() && !event.getDuration().isNull())
                durationCandidates.add(event.getDuration());
            if (node != null && node.hasFeature("fictivenode") && node.getLemma().equalsIgnoreCase("VERB")) {
                Vector<TemporalRelation> relations = event.getTemporalRelations(TemporalRelation.DURING, -1);
                for (TemporalRelation relation : relations) {
                    TemporalElement tempexpr = relation.getOtherElement(event);
                    if (tempexpr instanceof VagueDate && !((VagueDate)tempexpr).isDCT()) {
                        dateCandidates.add((VagueDate)tempexpr);
                    } else if (tempexpr instanceof Duration) {
                        durationCandidates.add((Duration)tempexpr);
                    }
//                        else
                    //                        throw new NotImplementedException();
                }
            }
        }
        dateCandidates = mergeDates(dateCandidates);
        dateCandidates = keepUsefulDates(dateCandidates);
        
        Vector<CalendarEvent> calendarEvents = this.getEventsFromDates(dateCandidates);
        
        for (Duration duration : durationCandidates){
            //            Vector<Integer> minutes = getAllMinuteDurations(duration);
            
            if (calendarEvents.isEmpty()){
                //                throw new NotImplementedException();
                //                for (Integer minute : minutes) {
                //                    CalendarEvent calendarEvent = new CalendarEvent();
                //                    calendarEvent.setDuration(minute.intValue());
                //                    calendarEvents.add(calendarEvent);
                //                }
            } else {
                Vector<CalendarEvent> newCalendarEvents = new Vector<CalendarEvent>();
                for (CalendarEvent calendarEvent : calendarEvents){
                    if (calendarEvent.getEnd() == null &&
                            calendarEvent.getSentenceNumber() == duration.getXipUnit().getSentenceNb()){
                        if (duration instanceof SimpleDuration)
                            calendarEvent.setDuration((SimpleDuration)duration);
                        else if (duration instanceof DurationInterval) {
                            CalendarEvent newCalendarEvent = new CalendarEvent(calendarEvent);
                            calendarEvent.setDuration(((DurationInterval)duration).getLowerBound());
                            newCalendarEvent.setDuration(((DurationInterval)duration).getUpperBound());
                            newCalendarEvents.add(newCalendarEvent);
                        } else {
                            throw new NotImplementedException();
                        }
                    }
                }
                calendarEvents.addAll(newCalendarEvents);
            }
        }
        try {
            if (options.getBooleanProperty("INQUICKADD") && calendarEvents.isEmpty()){
                calendarEvents.add(new CalendarEvent(true, 1));
            }
        } catch (TypedPropertyException ex) {
            ex.printStackTrace();
        }
        
        
        // get the info from the options
        String what = options.getProperty("CORPUS_WHAT");
        String where = options.getProperty("CORPUS_WHERE");
        String with = options.getProperty("CORPUS_WITH");
        String description = options.getProperty("CORPUS_DESCRIPTION");
        
        for (CalendarEvent calendarEvent : calendarEvents) {
            //            System.out.println("Valid this event?   (Y/N)");
            calendarEvent.complete();
            calendarEvent.setWhat(what);
            calendarEvent.setWith(with);
            calendarEvent.setWhere(where);
            calendarEvent.setDescription(description);
            System.out.println(calendarEvent.toString());
        }
        
        eventFrame.showEvents(calendarEvents);
        
        options.getTemporalGraph().clear();
        
        
        
//        } catch (MalformedURLException ex) {
        //            ex.printStackTrace();
        //        } catch (ServiceException ex) {
        //            ex.printStackTrace();
        //        } catch (IOException ex) {
        //            ex.printStackTrace();
        //        }
        
    }
    
    

    
    private Vector<CalendarEvent> getEventsFromDates(final Vector<VagueDate> dateCandidates) throws NotImplementedException {
        
        Vector<CalendarEvent> calendarEvents = new Vector<CalendarEvent>();
        for (VagueDate date : dateCandidates){
            long sentenceNumber;
            if (date.getXipUnit() == null)
                sentenceNumber = 1;
            else
                sentenceNumber = date.getXipUnit().getSentenceNb();
            CalendarEvent calendarEvent = new CalendarEvent(sentenceNumber);
            date = date.getRealDate();
            // DATE
            if (date instanceof VagueSimpleDate) {
                VagueSimpleDate sdate = (VagueSimpleDate)date;
                // BEGIN
                calendarEvent.setStart(sdate);
                calendarEvents.add(calendarEvent);
            } else if (date instanceof VagueInterval){
                VagueInterval idate = (VagueInterval)date;
                calendarEvent.setStart(idate.getLowerBound());
                calendarEvent.setEnd(idate.getUpperBound());
                calendarEvents.add(calendarEvent);
            } else {
                VagueMultipleDate mdate = (VagueMultipleDate)date;
                VagueDate[] dates = mdate.getDates();
                Vector<VagueDate> newCandidates = new Vector<VagueDate>();
                for (int i = 0 ; i < dates.length ; ++i){
                    newCandidates.add(dates[i]);
                }
                calendarEvents.addAll(this.getEventsFromDates(newCandidates));
            }
            
        }
        return calendarEvents;
    }
        
    

    
    /**
     * Merge lonely hours with lonely days
     */
    private static Vector<VagueDate> mergeDates(Vector<VagueDate> dates) {
        Vector<VagueDate> result = new Vector<VagueDate>();
        for (VagueDate date : dates) {
            // if the date is an hour but is not absolute (no day, month, year)
            if (date.hasGranularity(TemporalConstituent.HOUR) &&
                    !date.isAbsolute()) {
                // if the date is not already related to an absolute date
                Vector<TemporalRelation> relations = date.getTemporalRelations(TemporalRelation.EQUALS, -1);
                boolean lonely = true;
                for (TemporalRelation relation : relations){
                    TemporalElement otherElement = relation.getOtherElement(date);
                    if (otherElement instanceof VagueDate && ((VagueDate)otherElement).isAbsolute()) {
                        lonely = false;
                        break;
                    }
                }
                
                // look for a lonely day to merge with
                if (lonely){
                    for (VagueDate otherDate : dates) {
                        if (otherDate.isAbsolute() && otherDate.getGranularity() == TemporalConstituent.DAY) {
                            date = date.completeDate(otherDate);
                            result.add(date);
                        }
                    }
                }
            }
        }
        result.addAll(dates);
        return result;
    }
    
    private static Vector<VagueDate> keepUsefulDates(Vector<VagueDate> dates) {
        Vector<VagueDate> result = new Vector<VagueDate>();
        for (VagueDate date : dates) {
            boolean takeIt = true;
            // keep only daytime
            if (date instanceof VagueMultipleDate){
                VagueDate auxDate = date.getOnlyDayTime();
                if (!(date.isDefined() && (auxDate == null || !auxDate.isDefined())))
                    date = auxDate;
            }
            
            //
            for (VagueDate dateInResult : result) {
                if (dateInResult.compareTo(date) == TemporalRelation.EQUALS) {
                    takeIt = false;
                    continue;
                }
            }
            
            // keep only useful dates
            if (takeIt && isUseful(date, dates)) {
                result.add(date);
            }
        }
        return result;
    }
    
    private static boolean isUseful(VagueDate tryDate, Vector<VagueDate> dates) {
        if (tryDate == null || !tryDate.isDefined() || !tryDate.isAbsolute())
            return false;
        
        for (VagueDate date : dates) {
        // a date is not useful if another date with lower granularity 
        //   is during this one
            if (date.getGranularity() > tryDate.getGranularity() &&
                    date.compareTo(tryDate) == TemporalRelation.DURING)
                return false;
        }
        return true;
    }
    
    

        
    
    
}
