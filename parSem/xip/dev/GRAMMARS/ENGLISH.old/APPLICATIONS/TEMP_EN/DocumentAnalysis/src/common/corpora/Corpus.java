/*
 * Corpus.java
 *
 * Created on December 8, 2006, 2:38 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.corpora;

import com.xerox.jxip.XipUnit;
import common.*;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.TemporalGraph;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.Hashtable;

/**
 * Class for corpus-specific static methods.
 * @author xtannier
 */
public class Corpus {
    
    
    protected String name;
    
    
    public static String CORPUS_DOCUMENT_ID = "CORPUS_DOCUMENT_ID";    
    
    
    /**
     * Builds a new instance of a Corpus
     */
    public Corpus(){
        
    }
    
    /**
     * Builds a new instance of a Corpus
     */
    public Corpus(Options options){
        this();
    }
    
    /**
     * Method called at the end of each file
     */
    public void clear(){
    }
    
    /**
     * Returns the name of the Corpus.
     * @return the name of the Corpus.
     */
    public String getName() {
        return name;
    }
    
    /**
     * Returns a new instance of Corpus according to the specified name.
     *
     * @param options the parser options
     * @throws common.UnknownCorpusException when the name of the corpus is unknown.
     * @return a new Corpus.
     */
    public static Corpus getNewCorpus(Options options) throws UnknownCorpusException{
        String name = options.getProperty(Options.CORPUS);
        
        if (name != null) {
            String packagePath = "common.corpora";
            Class<?> subClass;
            try {
                subClass = Common.getClass(packagePath, name, true);
            } catch (ClassNotFoundException ex) {
                throw new UnknownCorpusException(name);
            }                            
            
            if (subClass == null) {
                throw new UnknownCorpusException(name);
            } else {
                try {
                    Constructor constructor = subClass.getConstructor(options.getClass());
                    return (Corpus)constructor.newInstance(options);
                } catch (Exception ex) {
                    throw new UnknownCorpusException(name);
                }
            }
        } else {
            return new Corpus(options);
        }
    }
    
    
    
    /**
     * Treats corpus-specific properties.
     * Used in:
     * <ul>
     *  <li>TempEval
     *  <li>WSJ
     * </ul>
     *
     * @param name the name of the property
     * @param value the value of the property
     * @return -1 if the treatment failed (basically if the property is unknown), 0 otherwise.
     */
    public int setProperty(String name, String value){
        return 0;
    }
    
    
    
    
    
    /**
     * Add new corpus-specific temporal relations in the temporal graph after its transitive
     * closure.
     * Used in:
     * <ul>
     *  <li>TempEval
     * </ul>
     *
     * @param temporalGraph the graph to refine
     * @param options the parser options
     */
    public void refineTemporalGraphAfterClosure(TemporalGraph temporalGraph,
            Options options) throws TemporalConsistencyException{
        
    }
    
    
    /**
     * Gets different information from the input file (DCT for example).
     * Used in:
     * <ul>
     *  <li>TempEval
     *  <li>BioVikef
     *  <li>LeMondeDiploEN
     *  <li>AutoCalendar
     *  <li>Test
     * </ul>
     *
     * @param options the parser options.
     */
    public void getInputInfo(Options options){
        
    }
    
    
    /**
     * Gets different information from the current sentence (DCT for example).
     *
     * Used in:
     * <ul>
     *  <li>Reuters
     *  <li>AutoCalendar
     *  <li>Test
     * </ul>
     *
     * @param unit the current parsed sentence
     * @param options the parser options
     */
    public void getSentenceInfo(XipUnit unit, Options options) {
        
    }
    
    /**
     * Gets different information from the XML tag currently parsed.
     * Called in the XML callback.
     * Used in:
     * <ul>
     *   <li>WSJ
     *   <li>TempEval
     *   <li>Polrisk
     * </ul>
     *
     * @param xmlTagInfos the info about XML tag name and attributes.
     * @param unit the last XipUnit parsed.
     * @param options the parser options.
     */
    public void getXMLTagInfo(Hashtable<String,String> xmlTagInfos,
            XipUnit unit,
            Options options){
        
    }
    
    
    /**
     * Apply any appropriate corpus-specific inference with a XipUnit.
     * Used in:
     * <ul>
     *  <li>TempEval
     * </ul>
     *
     * @param unit the current XipUnit treated
     * @param options the parser options.
     */
    public void corpusSpecificInference(XipUnit unit, Options options) throws TemporalConsistencyException{
        
    }
    

    /** 
     * Prints a summary of elements that take memory
     */            
    public void showElements() {
        System.out.println("none");
    }
}
