package com.xerox.xrce.xip2jena.temporal;


import com.hp.hpl.jena.datatypes.xsd.XSDDatatype;
import com.hp.hpl.jena.datatypes.xsd.XSDDateTime;


import com.hp.hpl.jena.ontology.Individual;
import com.hp.hpl.jena.ontology.OntClass;
import com.hp.hpl.jena.rdf.model.Literal;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.Property;

import com.xerox.xrce.xip2jena.Xip2Jena;
import com.xerox.xrce.xip2jena.Xip2JenaException;
import documentAnalysis.events.Event;
import documentAnalysis.temporality.TemporalElement;
import documentAnalysis.temporality.TemporalGraph;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;
import java.io.File;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Calendar;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.TimeZone;
import org.apache.log4j.Logger;

public class ToRDF {

    // todo: use the ontology namespace?
    private static final String grmUri = Xip2Jena.grammarBase + "temporal";
    static Logger log = Logger.getLogger(ToRDF.class);
    private Set<String> ignoredRels;
    private Map <String, Individual> nullNode2Individual  = new HashMap <String, Individual> ();

    private Xip2Jena xip2Jena;

    public void init(Model destModel) throws Exception {

        URL ontUrl = ToRDF.class.getResource("EventTemporal.owl");

        xip2Jena = new Xip2Jena(ontUrl, grmUri, destModel);

        ignoredRels = new HashSet<String>();
        
    }

    private Property temporalRelation2Property(TemporalRelation tr) {
        int r = tr.getRelation();
        if (r == TemporalRelation.BEFORE) return EventTemporal.before;
        if (r == TemporalRelation.AFTER) return EventTemporal.after;
        if (r == TemporalRelation.OVERLAPS) return EventTemporal.overlaps;
        if (r == TemporalRelation.OVERLAPPED_BY) return EventTemporal.overlapped_by;
        if (r == TemporalRelation.INCLUDES) return EventTemporal.includes;
        if (r == TemporalRelation.DURING) return EventTemporal.during;
        if (r == TemporalRelation.EQUALS) return EventTemporal.equals;
        return null;
    }
    
    public void processDocument(String inputFileName,
            TemporalGraph temporalGraph) throws ToRDFException, Xip2JenaException {

        xip2Jena.clear();
        
        URL docUrl;
        try {
            docUrl = new File(inputFileName).toURI().toURL();
        } catch (MalformedURLException ex) {
            throw new ToRDFException(ex);
        }

        Map<Individual, Individual> event2Interval = new HashMap<Individual, Individual>();

        /*
         * Create all individuals according to the elements of the TemporalGraph
         */
//        int counter = 0;
        for (TemporalElement element : temporalGraph.getElements()) {

                OntClass ontClass = null;
                if (element instanceof Event) {
                    ontClass = EventTemporal.TemporalEvent;
                    /*counter++;
                    System.out.println("The events are:"+counter);*/
                } else {
                    ontClass = EventTemporal.Interval;
                }
                addInstantsAndDatatypes(ontClass, docUrl, element, event2Interval);
        }

        //Create all relations between individuals
        for (TemporalElement element : temporalGraph.getElements()) {
                //For each of the elements take the temporal relations associated to it
                for (TemporalRelation temporalRelation : element.getTemporalRelations()) {
                    Property prop = temporalRelation2Property(temporalRelation);

                    if (prop == null) {
                        ignoredRels.add(element.toString() + "(.,.)");
                    } else {
                        Individual indiv1 = getInterval(temporalRelation.getTemporalElement1(),
                                    null, docUrl, event2Interval);

                        Individual indiv2 = getInterval(temporalRelation.getTemporalElement2(),
                                    null, docUrl, event2Interval);

                        indiv1.addProperty(prop, indiv2);
                    }
                }
                xip2Jena.commit();
        }
        nullNode2Individual.clear();

    }

    private Individual getInterval(TemporalElement element, OntClass ontClass, URL docUrl,
            Map<Individual,Individual> event2Interval) throws Xip2JenaException {
        Individual result, correspondingInterval;
        if (element.getXipNode() != null){
            result = xip2Jena.xipNode2Individual(element.getXipNode(), ontClass, docUrl);
            correspondingInterval = event2Interval.get(result);
        } else {
            result = nullNode2Individual(element, ontClass);
            if (element instanceof Event)
                correspondingInterval = event2Interval.get(result);
            else
                correspondingInterval = nullNode2Individual.get(element.toString());
        }
        if (correspondingInterval != null)
            result = correspondingInterval;
        return result;
    }

    private Individual nullNode2Individual (TemporalElement element, OntClass ontClass) throws Xip2JenaException{
        Individual result = null;
        result = nullNode2Individual.get(element.toString());
        if (result == null){
            result = xip2Jena.createIndividual(ontClass);
            nullNode2Individual.put(element.toString(), result);
        }
        if (ontClass != null){
            result.addRDFType(ontClass);
        }
        return result;
    }

    //Create the CalendarDate for the Instants denoting the upper and lower bounds of an interval
    private Calendar createCalendarDate(VagueDate vagueDate, boolean upperBound) {
        Calendar calendar = Calendar.getInstance();

        TimeZone tz = TimeZone.getTimeZone("GMT");
        calendar.setTimeZone(tz);

        if (upperBound) {
            if (vagueDate.getGranularity() > 4) { //Check if date is described with a granularity bigger than day
                calendar.set(Calendar.YEAR, vagueDate.getUpperBound().getYear().getIntValue());
                calendar.set(Calendar.MONTH, vagueDate.getUpperBound().getMonth().getIntValue() - 1);
                calendar.set(Calendar.DAY_OF_MONTH, vagueDate.getUpperBound().getDayOfMonth().getIntValue());
            } //Check if the date is described up to the level of the month
            else if (vagueDate.getGranularity() > 1) {
                calendar.set(Calendar.YEAR, vagueDate.getUpperBound().getYear().getIntValue());
                calendar.set(Calendar.MONTH, vagueDate.getUpperBound().getMonth().getIntValue() - 1);
                calendar.set(Calendar.DAY_OF_MONTH, calendar.getActualMaximum(Calendar.DAY_OF_MONTH));
            } else {
                calendar.set(Calendar.YEAR, vagueDate.getUpperBound().getYear().getIntValue());
                calendar.set(Calendar.MONTH, calendar.getActualMaximum(Calendar.MONTH));
                calendar.set(Calendar.DAY_OF_MONTH, calendar.getActualMaximum(Calendar.DAY_OF_MONTH));
            }

            if (vagueDate.getUpperBound().getHour().isDefined()) {
                calendar.set(Calendar.HOUR_OF_DAY, vagueDate.getUpperBound().getHour().getIntValue());
            } else {
                calendar.set(Calendar.HOUR_OF_DAY, 23);
            }
            if (vagueDate.getUpperBound().getHour().isDefined()) {
                calendar.set(Calendar.MINUTE, vagueDate.getUpperBound().getMinute().getIntValue());
            } else {
                calendar.set(Calendar.MINUTE, 59);
            }
            if (vagueDate.getUpperBound().getSecond().isDefined()) {
                calendar.set(Calendar.SECOND, vagueDate.getUpperBound().getSecond().getIntValue());
            } else {
                calendar.set(Calendar.SECOND, 59);
            }

        } else {

            if (vagueDate.getGranularity() > 4) {
                calendar.set(Calendar.YEAR, vagueDate.getLowerBound().getYear().getIntValue());
                calendar.set(Calendar.MONTH, vagueDate.getLowerBound().getMonth().getIntValue() - 1);
                calendar.set(Calendar.DAY_OF_MONTH, vagueDate.getLowerBound().getDayOfMonth().getIntValue());
            } //Check if the date is described up to the level of the month
            else if (vagueDate.getGranularity() > 1) {
                calendar.set(Calendar.YEAR, vagueDate.getLowerBound().getYear().getIntValue());
                calendar.set(Calendar.MONTH, vagueDate.getLowerBound().getMonth().getIntValue() - 1);
                calendar.set(Calendar.DAY_OF_MONTH, 01);
            } else {
                calendar.set(vagueDate.getLowerBound().getYear().getIntValue(), 0, 01);
            }

            if (vagueDate.getLowerBound().getHour().isDefined()) {
                calendar.set(Calendar.HOUR_OF_DAY, vagueDate.getLowerBound().getHour().getIntValue());
            } else {
                calendar.set(Calendar.HOUR_OF_DAY, 0);
            }
            if (vagueDate.getLowerBound().getHour().isDefined()) {
                calendar.set(Calendar.MINUTE, vagueDate.getLowerBound().getMinute().getIntValue());
            } else {
                calendar.set(Calendar.MINUTE, 0);
            }
            if (vagueDate.getLowerBound().getSecond().isDefined()) {
                calendar.set(Calendar.SECOND, vagueDate.getLowerBound().getSecond().getIntValue());
            } else {
                calendar.set(Calendar.SECOND, 1);
            }
        }
        // Set to an arbitrary fixed value, otherwise it gets an unpredictable value
        // that messes up regression tests.  Don't use 0, because a bug in the
        // hash function for XSDDateTime (Jena 2.6.0) causes serialization and
        // deserialization to result in a model that's not isomorphic to the original;
        // see email to jena-dev September 15 2009. -ANK
        calendar.set(Calendar.MILLISECOND, 1);

        return calendar;

    }

    // Check if the duration taken by the TemporalGraph has a valid format
    // (a lot of times instead of having an integer indicating the number of
    // the corresponding field there is X indicating that we don't know the exact digit)
    private boolean isValid (SimpleDuration xsdDuration) {

        boolean flag = true;

        if (xsdDuration.getValue().contains("X") ||
                xsdDuration.getValue().contains("-")){
            flag = false;
        }

        return flag;
    }

    private boolean isValid (VagueDate vagueDate){
        boolean flag = true;

        if (vagueDate.getGranularity() > 4) {
            if ((!vagueDate.getUpperBound().getYear().isDefined())
                    || (!vagueDate.getUpperBound().getMonth().isDefined())
                    || (!vagueDate.getUpperBound().getDayOfMonth().isDefined())) {
                flag = false;
            }
        } else if (vagueDate.getGranularity() > 1){
            if ((!vagueDate.getUpperBound().getYear().isDefined())
                    || (!vagueDate.getUpperBound().getMonth().isDefined())){
                flag = false;
            }
        } else {
            if (!vagueDate.getUpperBound().getYear().isDefined()){
                flag = false;
            }
        }

        return flag;
    }

    //Transform the format of the durations in the valid xsd:duration format
    private String canonicalXSDDuration(SimpleDuration xsdDuration) throws ToRDFException{
        StringBuffer str = new StringBuffer();
        str.append("P");
        //YEARS
        int pos = xsdDuration.getValue().indexOf('Y');
        String number = xsdDuration.getValue().substring(1, pos);
        str.append(number + "Y");
        //MONTHS
        str.append(xsdDuration.getMonths().getValue());

        //DAYS
        pos = xsdDuration.getWeeks().getValue().indexOf('W');
        number = xsdDuration.getWeeks().getValue().substring(0, pos);
        int weeks = 0;
        try {
            weeks = Integer.parseInt(number);
        } catch (NumberFormatException nfe) {
            nfe.printStackTrace();
        }
        int days = weeks * 7;

        pos = xsdDuration.getDays().getValue().indexOf('D');
        number = xsdDuration.getDays().getValue().substring(0, pos);
        try {
            days += Integer.parseInt(number);
        } catch (NumberFormatException nfe) {
            nfe.printStackTrace();
        }
        String stringDays = Integer.toString(days);
        str.append(stringDays + "D");


        str.append("T");
        //HOURS
        str.append(xsdDuration.getHours().getValue());
        //MINUTES
        str.append(xsdDuration.getMinutes().getValue());
        //SECONDS
        str.append(xsdDuration.getSeconds().getValue());

        return str.toString();
    }

    private void addInstantsAndDatatypes(OntClass ontClass,
            URL docUrl, TemporalElement element, Map<Individual, Individual> event2Interval)
            throws ToRDFException, Xip2JenaException {

        Individual indiv;
        if (element.getXipNode() !=null){
            indiv = xip2Jena.xipNode2Individual(element.getXipNode(), ontClass, docUrl);
        } else {
            indiv = nullNode2Individual (element, ontClass);
        }

        XSDDateTime xsdDateTime;

        //If we have a VagueDate then create two Instants according to the lower and upper bound of the VagueDate
        if (element instanceof VagueDate) {

            VagueDate vagueDate = (VagueDate) element;

            if (isValid(vagueDate)) {
                //Create the two Instants
                Individual upperBoundInstant = xip2Jena.createIndividual(EventTemporal.Instant);
                Individual lowerBoundInstant = xip2Jena.createIndividual(EventTemporal.Instant);

                //Define the XSDDataType according to the upperBound of the VagueDate
                xsdDateTime = new XSDDateTime(createCalendarDate(vagueDate, true));
                Literal xsdlit = xip2Jena.createTypedLiteral(xsdDateTime);
                upperBoundInstant.addProperty(EventTemporal.inXSDDataType, xip2Jena.createTypedLiteral(xsdDateTime));

                //Define the XSDDataType according to the lowerBound of the VagueDate
                xsdDateTime = new XSDDateTime(createCalendarDate(vagueDate, false));
                lowerBoundInstant.addProperty(EventTemporal.inXSDDataType, xip2Jena.createTypedLiteral(xsdDateTime));

                //Add the relations between the VagueDate (which is represented as an interval in the KB) and
                //the corresponding instants
                indiv.addProperty(EventTemporal.hasBeginningTime, lowerBoundInstant);
                indiv.addProperty(EventTemporal.hasEndTime, upperBoundInstant);


                if (isValid((SimpleDuration) vagueDate.getAssociatedDuration())) {
                    indiv.addProperty(EventTemporal.inXSDDuration, xip2Jena.createTypedLiteral(canonicalXSDDuration((SimpleDuration) vagueDate.getAssociatedDuration()), XSDDatatype.XSDduration));
                }
            }
        } //In case we have only the duration keep it
        else if (element instanceof SimpleDuration) {
            if (isValid((SimpleDuration) element)) {
                indiv.addProperty(EventTemporal.inXSDDuration, xip2Jena.createTypedLiteral(canonicalXSDDuration((SimpleDuration) element), XSDDatatype.XSDduration));
            }
        } //If the temporal element is an event create a corresponding interval and link it to other intervals
        else if (element instanceof Event) {

            Individual correspondingInterval = xip2Jena.createIndividual(EventTemporal.Interval);
            event2Interval.put(indiv, correspondingInterval);

            indiv.addProperty(EventTemporal.holds, correspondingInterval);
            if (((Event) element).hasDuration()) {
                if (((Event) element).getDuration() instanceof SimpleDuration) {
                    if (isValid((SimpleDuration) ((Event) element).getDuration())) {
                        correspondingInterval.addProperty(EventTemporal.inXSDDuration, xip2Jena.createTypedLiteral(canonicalXSDDuration((SimpleDuration) ((Event) element).getDuration()), XSDDatatype.XSDduration));
                    }
                }
            }
        } 

    }

    public Set<String> getIgnoredRels() {
        return ignoredRels;
    }
}










