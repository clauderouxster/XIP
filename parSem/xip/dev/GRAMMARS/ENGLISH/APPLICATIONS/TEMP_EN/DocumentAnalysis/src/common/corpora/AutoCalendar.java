/*
 * AutoCalendar.java
 *
 * Created on March 13, 2007, 8:48 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.corpora;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import common.Common;
import common.NotImplementedException;
import common.Options;
import documentAnalysis.temporality.TempFunctions;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import documentAnalysis.temporality.vaguecalendar.date.VagueSimpleDate;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import documentAnalysis.temporality.vaguecalendar.duration.DurationInterval;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;
import java.io.IOException;
import java.util.Vector;

//import com.google.gdata.client.calendar.*;
//import com.google.gdata.data.*;
//import com.google.gdata.data.calendar.*;
//import com.google.gdata.data.extensions.*;
//import java.net.URL;
//import java.util.Calendar;


/**
 * Thsi class is used for the automatic calendar generation
 * @author xtannier
 */
public class AutoCalendar extends Corpus {
    
    
    Vector<String> currentWhat;
    Vector<String> currentWith;
    Vector<String> currentWhere;
    Vector<String> currentDescription;
    Vector<String> stopWords;        
    boolean properNounFound;

    
    /** Creates a new instance of AutoCalendar */
    public AutoCalendar(Options options) {
        super();
        this.name = "AUTO_CALENDAR";
        this.currentWhat = new Vector<String>();
        this.currentWith = new Vector<String>();
        this.currentWhere = new Vector<String>();
        this.currentDescription = new Vector<String>();
        this.properNounFound = false;
        this.stopWords = null;        
        options.setProperty(TempFunctions.DEFAULT_TEMPORAL_RELATION, "AFTER");
    }
    
    
    /**
     * Gets different information from the input file (DCT for example).
     *
     * @param options the parser options.
     */
    public void getInputInfo(Options options){
        String inputFileName = options.getInputFileName();
        
        this.currentWhat = new Vector<String>();
        this.currentWith = new Vector<String>();
        this.currentWhere = new Vector<String>();
        this.currentDescription = new Vector<String>();
        this.properNounFound = false;
        
        /* get the Document Creation Time (DCT)
         * = today
         */
        VagueSimpleDate dct = VagueDate.getToday();
        try{
            options.setDct(dct);
        } catch (TemporalConsistencyException ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
    }
    
    
    /**
     * Gets different information from the current sentence (DCT for example).
     * @param unit the current parsed sentence
     * @param options the parser options
     */
    public void getSentenceInfo(XipUnit unit, Options options) {
        // WHAT
        // nouns or verbs in the "calendar_event" list
        Vector<XipNode> events = unit.getNodesByFeature("calendar_event", "+");
        for (XipNode event : events) {
            if (!event.isLeaf()){
                Vector<XipDependency> heads = event.getDependenciesByName("HEAD");
                if (heads.isEmpty()){
                    this.currentWhat.add(event.getSurface());
                } else {
                    XipNode allWhat = heads.get(0).getArg(2);
                    this.currentWhat.add(allWhat.getSurface());
                }
            }
        }
        
        String text;
        // if nothing found : proper nouns
        if (this.currentWhat.isEmpty()){
            Vector<XipNode> nps = unit.getNodesByCategory("NP", "tempexpr:~");
            
            boolean isProperNoun;
            
            for (XipNode np : nps) {
                if (np.getDescendantsByDependency("LOCATION").isEmpty() &&
                        np.getDescendantsByDependency("LOCORG").isEmpty() &&
                        np.getDescendantsByDependency("PERSON").isEmpty()) {
                    text = this.removeStopWords(np.getLemma(), options.getProperty("STOP_WORD_LIST"));
                    isProperNoun = text.matches(".*[A-Z].*");
                    
                    if (!text.equals("") && !(!isProperNoun && this.properNounFound)) {
                        this.currentWhat.add(text);
                    }
                    
                    if (isProperNoun)
                        this.properNounFound = true;
                }
            }
        }
        // if nothing found, verbs
        if (this.currentWhat.isEmpty()){
            Vector<XipNode> verbs = unit.getNodesByCategory("VERB");
            for (XipNode verb : verbs) {
                if (verb.isLeaf() && !verb.hasFeature("fictivenode")){
                    text = this.removeStopWords(verb.getSurface(), options.getProperty("STOP_WORD_LIST"));
                    if (!text.equals(""))
                        this.currentWhat.add(text);
                }
            }
        }
        
        // WHERE
        Vector<XipDependency> locations = unit.getDependenciesByName("LOCATION");
        locations.addAll(unit.getDependenciesByName("LOCORG"));
        for (XipDependency location : locations) {
            this.currentWhere.add(location.getArg(1).getSurface());
        }
        
        // WITH
        Vector<XipDependency> persons = unit.getDependenciesByName("PERSON");
        for (XipDependency person : persons) {
            this.currentWith.add(person.getArg(1).getSurface());
        }
        
        // DESCRIPTION
//        if (this.currentDescription.isEmpty()) {
        this.currentDescription.add(unit.getSentenceString());
//        }
        
        // put the info into the options
        options.setProperty("CORPUS_WHAT", this.currentWhat.toString());
        options.setProperty("CORPUS_WHERE", this.currentWhere.toString());        
        options.setProperty("CORPUS_WITH", this.currentWith.toString());                
        options.setProperty("CORPUS_DESCRIPTION", this.currentDescription.toString());                        
    }    

    
    
    private static Vector<Integer> getAllMinuteDurations(Duration duration){
        Vector<Integer> result = new Vector<Integer>();
        if (duration instanceof SimpleDuration)
            result.add(((SimpleDuration)duration).getValue(TemporalConstituent.MINUTE));
        else if (duration instanceof DurationInterval){
            result.add(((DurationInterval)duration).getLowerBound().getValue(TemporalConstituent.MINUTE));
            result.add(((DurationInterval)duration).getUpperBound().getValue(TemporalConstituent.MINUTE));
        } else
            throw new NotImplementedException();
        return result;
    }
    
    

    private String removeStopWords(String text, String listFileName){
        if (listFileName != null && this.stopWords == null) {
            try {
                this.stopWords = Common.readToVector(listFileName);
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
        if (this.stopWords != null && this.stopWords.contains(text.toLowerCase()))
            return "";
        return text;
    }
        
    
    
}
