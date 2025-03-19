/*
 * Options.java
 *
 * Created on December 8, 2006, 3:04 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common;

import common.applications.Application;
import common.corpora.Corpus;
import documentAnalysis.Main;
import documentAnalysis.namedEntities.EntitySet;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.TemporalGraph;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.events.Event;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.Set;


/**
 * Class for handling main options.
 * @author xtannier
 */
public class Options {
    
    // grammar handler
    private int gHandler;
    private String outputType;
    private boolean isInputInXML;
    private boolean isInputStandard;
    private TypedProperties properties;
    private Corpus corpus;
    private Application application;
    private String inputString;
    private Main parser;
    
    // Output specs
    private boolean printSentenceNumber;
    private boolean printSentenceString;
    
    // Temporality
    private VagueDate dct;
    private TemporalGraph temporalGraph;
    
    // Named entities
    private EntitySet entitySet;

    /** Input file name */
    public static final String INPUT_FILE = "INPUT_FILE";
    /** Input directory name */
    public static final String INPUT_DIR = "INPUT_DIR";
    /** Boolean: TRUE if the input comes from standard input */
    public static final String STD_INPUT = "STR_INPUT";
    /** Input in XML.
     * If the input is in XML, this property must be set to "True" (case insensitive)
     * or 1.
     */
    public static final String XML_INPUT = "XML_INPUT";
    /** output file name */
    public static final String OUTPUT_FILE = "OUTPUT_FILE";
    /** output dir name */
    public static final String OUTPUT_DIR = "OUTPUT_DIR";
    /** GRM file location */
    public static final String GRM_FILE = "GRM_FILE";
    /** corpus name or type */
    public static final String CORPUS = "CORPUS";
    /** application name or type */
    public static final String APPLICATION = "APPLICATION";
    /** MySQL DB host name */
    public static final String MYSQL_HOSTNAME = "MYSQL_HOSTNAME";
    /** MySQL DB pilote name */
    public static final String MYSQL_PILOTE = "MYSQL_PILOTE";
    /** Extra XIP grammar files */
    public static final String XIP_FILES = "XIP_FILES";
    /** Special task for a given corpus */
    public static final String CORPUS_TASK = "CORPUS_TASK";
    /** Print sentence number */
    public static final String PRINT_SN = "PRINT_SENTENCE_NUMBER";
    /** Print sentence text */
    public static final String PRINT_ST = "PRINT_SENTENCE_TEXT";
    /** Input is in UTF-8 (TRUE = UTF-8, FALSE = ISO-8859-1) */
    public static final String INPUT_UTF8 = "INPUT_UTF8";
    
    
    
    /** Creates a new instance of Options */
    public Options() {
        this.outputType = "";
        this.gHandler = -1;
        this.setIsInputInXML(false);
        this.setTemporalGraph(new TemporalGraph());        
        this.setEntitySet(new EntitySet());
        this.setCorpus(new Corpus());
        this.properties = new TypedProperties();
        this.setPrintSentenceNumber(false);
        this.setPrintSentenceString(false);
        try {
            this.setDct(VagueDate.getNewDate());
        } catch (TemporalConsistencyException ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
    }
    
    /**
     * Clears whatever is needed
     */
    public void clear(){
        try {
            this.setDct(VagueDate.getNewDate());
        } catch (TemporalConsistencyException ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
        this.setTemporalGraph(new TemporalGraph());
        this.setEntitySet(new EntitySet());        
    }
    
    
    /**
     * Prints a summary of the options that take memory
     *
     */
    public void showElements() {
        System.out.println("properties     = " + this.properties.size());
        System.out.println("corpus         = "); this.corpus.showElements();
        System.out.println("application    = "); this.application.showElements();
        System.out.println("temporal graph = " + this.temporalGraph.getNodeNumber() + " / " + 
                    this.temporalGraph.getRelationNumber());
        System.out.println("entity set     = " + this.entitySet.size());
    }
    
    /**
     * Returns the grammar handler
     * @return the grammar handler
     */
    public int getGrammarHandler() {
        return gHandler;
    }
    
    
    /**
     * Sets the grammar handler
     *
     * @param gHandler the grammar handler
     */
    public void setGrammarHandler(int gHandler) {
        this.gHandler = gHandler;
    }
    
    
    /**
     * Returns the input file name.
     * @return the input file name.
     */
    public String getInputFileName() {
        return this.properties.getProperty(INPUT_FILE);
    }
    
    /**
     * Sets the input file name.
     * @param inputFileName the input file name.
     */
    public void setInputFileName(String inputFileName) {
        this.properties.setProperty(INPUT_FILE, inputFileName);
    }
    
    
    /**
     * Returns the input directory.
     * @return the input directory
     */
    public String getInputFileDir() {
        return this.properties.getProperty(INPUT_DIR);
    }
    
    /**
     * Sets the input directory.
     * @param inputFileDir the input directory
     */
    public void setInputFileDir(String inputFileDir) {
        this.properties.setProperty(INPUT_DIR, inputFileDir);
    }
    
    
    /**
     * Sets the input String.
     * @param input the input String.
     */
    public void setInputString(String input){
        this.inputString = input;
    }
    
    /**
     * Returns the input string.
     * @return the input string.
     */
    public String getInputString() {
        return this.inputString;
    }
    
    
    /**
     * Returns the output file name.
     * @return the output file name.
     */
    public String getOutputFileName() {
        return this.properties.getProperty(OUTPUT_FILE);
    }
    
    /**
     * Sets the output file name.
     * @param outputFileName the output file name
     */
    public void setOutputFileName(String outputFileName) {
        this.properties.setProperty(OUTPUT_FILE, outputFileName);
    }
    
    /**
     * Returns the output dir name.
     * @return the output dir name.
     */
    public String getOutputDirName() {
        return this.properties.getProperty(OUTPUT_DIR);
    }
    
    /**
     * Sets the output dir name.
     * @param outputDirName the input file name.
     */
    public void setOutputDirName(String outputDirName) {
        this.properties.setProperty(OUTPUT_DIR, outputDirName);
    }
    
    
    /**
     * Returns the name of the corpus.
     * @return the name of the corpus.
     */
    public String getCorpusName() {
        return this.corpus.getName();
    }
    
    /**
     * Returns the name of the GRM file.
     * @return the name of the GRM file.
     */
    public String getGrmFileName() {
        return this.properties.getProperty(GRM_FILE);
    }
    
    /**
     * Sets the name of the GRM file.
     * @param grmFile the name of the GRM file
     */
    public void setGrmFileName(String grmFile) {
        this.properties.setProperty(GRM_FILE, grmFile);
    }
    
    
    /**
     * Returns the type of output.
     * @return the type of output, if specified (null otherwise)
     */
    public String getOutputType() {
        return outputType;
    }
    
    /**
     * Sets the type of output.
     * @param outputType - the type of output
     */
    public void setOutputType(String outputType) {
        this.outputType = outputType;
    }
    
    /**
     * Tests whether the option for an XML input is specified.
     * @return true if the option for an XML input is specified
     */
    public boolean isInputInXML() {
        return isInputInXML;
    }
    
    /**
     * Sets whether the option for an XML input is specified.
     * @param isInputInXML whether the option for an XML input is specified
     */
    public void setIsInputInXML(boolean isInputInXML) {
        this.isInputInXML = isInputInXML;
    }
    
    
    /**
     * Tests whether the option for an input for standard input is specified.
     * @return true if the option for an input for standard input is specified.
     */
    public boolean isInputStandard() {
        return isInputStandard;
    }
    
    /**
     * Sets whether the option for an input for standard input is specified.
     * @param isInputStandard whether the option for an input for standard input is specified.
     */
    public void setIsInputStandard(boolean isInputStandard) {
        this.isInputStandard = isInputStandard;
    }
    
    
    /**
     * Tests whether the sentence number should be printed.
     * @return true if the sentence number should be printed, false otherwise.
     */
    public boolean isPrintSentenceNumber() {
        return printSentenceNumber;
    }
    
    /**
     * Sets whether the sentence number should be printed.
     * @param printSentenceNumber true if the sentence number should be printed. false otherwise.
     */
    public void setPrintSentenceNumber(boolean printSentenceNumber) {
        this.printSentenceNumber = printSentenceNumber;
    }
    
    /**
     * Tests whether the current sentence should be printed.
     * @return true if the current sentence should be printed, false otherwise.
     */
    public boolean isPrintSentenceString() {
        return printSentenceString;
    }
    
    /**
     * Sets whether the current sentence should be printed.
     * @param printSentenceString true if the current sentence should be printed. false otherwise.
     */
    public void setPrintSentenceString(boolean printSentenceString) {
        this.printSentenceString = printSentenceString;
    }
    
    
    
    /**
     * Returns the Document Creation Time.
     * @return the DCT
     */
    public VagueDate getDct() {
        return dct;
    }
    
    /**
     * Sets the Document Creation Time.
     * @param dct the DCT
     * @throws documentanalysis.temporality.TemporalConsistencyException raised
     *    when two relations are not compatible (eg: before(A, B) and after(A, B))
     */
    public void setDct(VagueDate dct) throws TemporalConsistencyException {
        this.dct = dct;
        if (dct != null && dct.isDefined()){
            dct.setDCT();
            this.temporalGraph.addDate(dct);
            Event speechTime = new Event(true);
            this.temporalGraph.setSpeechTime(speechTime);
            this.temporalGraph.setDCT(dct);
            this.temporalGraph.addRelation(new TemporalRelation(speechTime, dct,
                    TemporalRelation.DURING, TemporalRelation.TEMPORAL_EXPRESSION, "DCT / ST"));
        }
    }
    
    /**
     * Returns the current temporal graph.
     * @return the current temporal graph
     */
    public TemporalGraph getTemporalGraph() {
        return temporalGraph;
    }
    
    /**
     * Sets the current temporal graph.
     * @param temporalGraph the temporal graph
     */
    public void setTemporalGraph(TemporalGraph temporalGraph) {
        this.temporalGraph = temporalGraph;
    }

    /** 
     * Returns the current entity set.
     * @return the current entity set.
     */
    public EntitySet getEntitySet() {
        return entitySet;
    }

    /** 
     * Sets the current entity set.
     * @param entitySet the current entity set.
     */    
    public void setEntitySet(EntitySet entitySet) {
        this.entitySet = entitySet;
    }
    
    
    
    /**
     * Reads and analyses the configuration file.
     *
     * @param fileName String
     * @return <code>0</code> if the file exists, <code>-1</code> otherwise.
     */
    public int updateConfiguration(String fileName) throws IOException {
        if (fileName != null) {
            this.properties.load((InputStream)(new FileInputStream(Common.getFile(fileName))));
        }
        
        Enumeration e = this.properties.propertyNames();
        String propValue;
        String propName;
        
        // Removes eventual quotation mark around the property.
        while (e.hasMoreElements()){
            propName = e.nextElement().toString();
            propValue = this.properties.getProperty(propName);
            
            if (propValue != null && propValue.charAt(0) == '"' && propValue.charAt(propValue.length() - 1) == '"')
                propValue = propValue.substring(1, propValue.length() - 1);
            this.properties.setProperty(propName, propValue);
        }
        
        return 0;
        
    }
    
    /**
     * Returns a property specified in the configuration file
     *
     * @param propertyName the name of the property
     * @return the value of the property
     * @see common.TypedProperties#getProperty(String propertyName)
     */
    public String getProperty(String propertyName) {
        return this.properties.getProperty(propertyName);
    }
    
    
    /**
     * Returns a boolean value corresponding to the key Property.
     * Value <code>true</code> is returned if the value of the property is "1" or "true"
     * <code>false</code> if the value is "0" or "false" (case-insensitive).
     *
     * @param propertyName the key of the property
     * @throws TypedPropertyException if the property has no boolean value representation
     * @return a boolean value corresponding to the key Property, <code>false</code> if the
     * property is not found.
     * @see common.TypedProperties#getBooleanProperty(String propertyName)
     */
    public boolean getBooleanProperty(String propertyName) throws TypedPropertyException {
        return this.properties.getBooleanProperty(propertyName);
    }
    
    /**
     * Returns a boolean value corresponding to the key Property.
     * Value <code>true</code> is returned if the value of the property is "1" or "true"
     * <code>false</code> if the value is "0" or "false" (case-insensitive).
     *
     * @param propertyName the key of the property
     * @param defaultValue the default value if the property does not exist
     * @throws TypedPropertyException if the property has no boolean value representation
     * @return a boolean value corresponding to the key Property, <code>false</code> if the
     * property is not found.
     * @see common.TypedProperties#getBooleanProperty(String propertyName, boolean defaultValue)
     */
    public boolean getBooleanProperty(String propertyName, boolean defaultValue) throws TypedPropertyException {
        return this.properties.getBooleanProperty(propertyName, defaultValue);
    }
    
    
    /**
     * Returns an integer value corresponding to the key Property.
     *
     * @param propertyName the key of the property
     * @throws TypedPropertyException if the property has no integer value representation
     * @return an integer value corresponding to the key Property, 0 if the
     * property is not found.
     * @see common.TypedProperties#getIntProperty(String propertyName)
     */
    public int getIntProperty(String propertyName) throws TypedPropertyException {
        return this.properties.getIntProperty(propertyName);
    }
    
    /**
     * Sets a property
     *
     * @param name the name of the property
     * @param value the value of the property
     * @return the old property value, null if this property did not exist.
     * @see java.util.Properties#setProperty(String key, String value)
     */
    public Object setProperty(String name, String value) {
        return this.properties.setProperty(name, value);
    }
    
    
    /**
     * Returns a set of keys in this property list where the key and its
     * corresponding value are strings, including distinct keys in the default
     * property list if a key of the same name has not already been found
     * from the main properties list.
     * @return a set of keys in this property list where the key and its
     *   corresponding value are strings, including the keys in the default property list.
     * @see java.util.Properties#stringPropertyNames()
     */
    public Set<String> stringPropertyNames(){
        return this.properties.stringPropertyNames();
    }
    
    /**
     * Returns the current Corpus.
     * @return the current Corpus.
     */
    public Corpus getCorpus() {
        return corpus;
    }
    
    /**
     * Sets the current Corpus.
     * @param corpus the current corpus.
     */
    public void setCorpus(Corpus corpus) {
        this.corpus = corpus;
    }
    
    /**
     * Returns the current Application.
     * @return the current Application.
     */
    public Application getApplication() {
        return application;
    }
    
    /**
     * Sets the current Application.
     * @param application the current Application.
     */
    public void setApplication(Application application) {
        this.application = application;
    }
    
    /**
     * Gets the parser corresponding to the current set of Options
     *
     * @return the parser corresponding to the current set of Options
     */
    public Main getParser() {
        return parser;
    }
    
    /**
     * Sets the parser corresponding to the current set of Options
     *
     * @param parser the parser corresponding to the current set of Options
     */
    public void setParser(Main parser) {
        this.parser = parser;
    }

}
