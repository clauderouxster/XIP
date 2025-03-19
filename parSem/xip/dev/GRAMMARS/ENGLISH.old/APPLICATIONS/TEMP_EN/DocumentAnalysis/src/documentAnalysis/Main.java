/*
 *
 * test arguments : -text /home/xtannier/Corpus/Test/test.txt -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_temp.cnf -printsn -printst -temp_interp -o /home/xtannier/tmp/test.html -corpus test
 * test rien: -text /home/xtannier/Corpus/Test/test.txt -grm /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SPECTEMP/GRMFILES/gram_temporalExp.grm
 * xml : -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_temp.cnf -xmltext /home/xtannier/tmp/wsj7_001.xml --temp_interp
 * other arguments :
 *    -grm /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SPECTEMP/GRMFILES/gram_temporalExp.grm -text /home/xtannier/Corpus/Biographies_VIKEF/Hoaglin_2002.txt --temp_interp -corpus biovikef
 * Tempeval: -xmltext /home/xtannier/tmp/tempeval_test.tml -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_temp.cnf  -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_tempeval.cnf -o /home/xtannier/tmp/test.xml
 * Tempeval: -xmldir /opt/ParSem/Corpora/ENGLISH/SemEval07/TempEval/trial/data/taskAB/BlindTest -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_temp.cnf  -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_tempeval.cnf -odir /home/xtannier/tmp/TempEval
 * TimeforEntities: 
 *           -xmltext /home/xtannier/tmp/wsj_test.xml -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_temp.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_wsj.cnf
 *           -xmldir /opt/ParSem/Corpora/ENGLISH/WSJ/XML/1987 -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_temp.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_wsj.cnf
 * TimeforEvents: 
 *           -xmltext /home/xtannier/tmp/wsj_test.xml -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_temp.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_collect_events.cnf
 *           -xmldir /opt/ParSem/Corpora/ENGLISH/WSJ/XML/1987 -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_temp.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_collect_events.cnf
 *
 * AutoCalendar -stdinput -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_temp.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_autocal.cnf -corpus AutoCalendar -app AutoCalendar
 * AutoCalendar -stdinput -grm /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SPECTEMP/GRMFILES/gram_email_miner.grm -temporal_inference_level 2 -temporal_composition_level 2 -temporal_robust 1 -stop_word_list /home/xtannier/Travail/stop_words.txt -corpus AutoCalendar -app AutoCalendar
 * TempDemo -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties.cnf -conf /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEMP_EN/properties_temp.cnf -app TemporalDemo
 * TempDemo -grm /home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SPECTEMP/GRMFILES/gram_temporalExp.grm -temporal_inference_level 1 -temporal_composition_level 2 -temporal_robust 1 -app TemporalDemo
 * Main.java
 *
 * Created on November 29, 2006, 9:30 AMoptio
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis;


import common.TypedPropertyException;
import documentAnalysis.namedEntities.EntityFunctions;
import documentAnalysis.temporality.TempFunctions;
import common.Options;
import common.corpora.Corpus;
import com.xerox.jxip.*;
import common.Common;
import common.CommonLog;
import common.applications.Application;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.TemporalOptions;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Properties;
import java.util.Set;
import java.util.TimeZone;
import java.util.Vector;



/**
 * Main class
 * @author xtannier
 */
public class Main {
    
    private Options options;
    
    XipResultManager xipResultManager;
    int gHandler;
    JXip jxip;
    XipUnit currentUnit;
    
    
    /**
     * Creates a new instance of Main, loads the grammar.
     *
     * @param options the set of options
     * @throws java.lang.Exception any exception raised in the called methods
     */
    public Main(Options options) throws Exception {
        this(options, new JXip());
    }
    
    public Main(Options options, JXip jxip) {
        this.options = options;
        this.options.setParser(this);
        this.currentUnit = null;
        this.jxip = jxip;
        
        try {
            System.out.println("Main::main() Loading grammar file: " + options.getGrmFileName());
            //load the grammar
            System.err.println(options.getGrmFileName());
            gHandler = jxip.XipGrmFile(options.getGrmFileName(), 0, true);
            
            options.setGrammarHandler(gHandler);
            
        } catch (Exception ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
    }
    
    /**
     * Adds a grammar file to XIP grammar.
     * (same as -p XIP option)
     *
     * @param fileName the grammar file to add
     * @return -1 if error, 0 otherwise
     */
    public int addParameterFile(String fileName) throws Exception{
        return this.jxip.XipParameterFile(this.gHandler, fileName);
    }
    
    /**
     * Empty call backx
     */
    public void emptyCB() {
    }
    
    
    
    /**
     * Call back method for XML input. This method is called when a tag ends
     */
    public void myCB1XML() {
        Hashtable<String,String> xmlTagI = xipResultManager.getXmlTagInfo();
        this.options.getCorpus().getXMLTagInfo(xmlTagI, this.currentUnit, this.options);
    }
    
    
    /**
     *  Call back method for each sentence.
     */
    public void myCB1() {
        String sentence = "";
        try {
            XipResult xipRes = xipResultManager.getXipResult();
            
            // in the callback the XipResult contains only one XipUnit
            if (!xipRes.getXipUnits().isEmpty())
                this.currentUnit = xipRes.getXipUnits().get(0);
            
            // print sentence information if necessary
            //   (special parameters)
            if (this.options.isPrintSentenceString()){
                sentence = xipRes.getSentenceString();
                System.err.println("Sentence " + this.currentUnit.getSentenceNb() + ": " + sentence );
            } else if (this.options.isPrintSentenceNumber()){
                System.out.println("Sentence " + this.currentUnit.getSentenceNb());
            }
            
            // Corpus-specific sentence information
            //  (eg. : DCT, ...)
            // each Corpus can implement this method
            options.getCorpus().getSentenceInfo(this.currentUnit, this.options);
            
            String outputType = this.options.getOutputType();
            
            // TEMPORALITY
            // inference
            TempFunctions.basicInference(this.currentUnit, this.options);
            
            // NAMED ENTITIES
            EntityFunctions.parseEntities(this.currentUnit, this.options);
            
            // OUTPUT
            // corpus dependent, update output for each sentence
            // each Corpus can implement this method
            options.getApplication().updateOutput(this.currentUnit, this.options);
            
        } catch (Exception ex) {
            ex.printStackTrace();
            System.err.println("Exception occurred for sentence:");
            System.err.println("   " + sentence);
            System.exit(-1);
        }
    }
    
    
    
    /**
     * Launch parsing
     *
     * @param   options the parser Options
     * @throws  TypedPropertyException when a configuration property does not have
     *  the appropriate format
     * @throws  TemporalConsistencyException when occurs an inconsistency in the temporal module
     */
    public void parse(Options options) throws TypedPropertyException, TemporalConsistencyException {
        String input;
        options.clear();
        options.getApplication().getInputInfo(options);
        options.getCorpus().getInputInfo(options);
        xipResultManager = new XipResultManager();
        // the following methods "xipResultBuilderFrom***" launch
        //   the parsing for all sentences.
        //   callback methods are called for each sentence and/or for each tag
        try{
            if (options.getBooleanProperty(Options.INPUT_UTF8)) {
                jxip.XipAddFlagDisplay(gHandler, Const.UTF8_INPUT, 80);
            } else {
                jxip.XipRemoveFlagDisplay(gHandler, Const.UTF8_INPUT);
            }
            // Standard input
            if (this.options.isInputStandard()) {
                input = this.options.getInputString();
                xipResultManager.xipResultBuilderFromString(gHandler, input, this, "myCB1");
            }
            // XML file input
            else if (this.options.isInputInXML()) {
                input = options.getInputFileName();
                xipResultManager.xipResultBuilderFromXmlFile(gHandler, input, this, "myCB1", "myCB1XML", 0);
            }
            // Text file input
            else {
                input = options.getInputFileName();
                xipResultManager.xipResultBuilderFromTextFile(gHandler, input, this, "myCB1");
            }
        } catch (Exception e){
            e.printStackTrace();
//            System.exit(-1);
        }
        
        // at this point all sentences have been parsed
        
        /******************************/
        /* END OF DOCUMENT TREATMENTS */
        /******************************/
        endOfDocument(options);
        
        // corpus/application-specific
        // each Corpus or Application can implement these methods
        // 1. close the output
        options.getApplication().closeOutput(options);
        // 2. clear whatever is needed
        options.getCorpus().clear();
    }
    
    
    /**
     * Launch all treatments concerning the end of a document
     *
     * @param options the parser options
     */
    public static void endOfDocument(Options options) throws TemporalConsistencyException {
        // TEMPORALITY
        // Temporal interpretation
        // Transitive closure of the temporal graph
        try{
            if (options.getBooleanProperty(TemporalOptions.TEMPORAL_ROBUST) == false)
                TempFunctions.getGraphTemporalClosure(options, true);
            else
                TempFunctions.getGraphTemporalClosure(options, false);
        } catch (TypedPropertyException e) {
            TempFunctions.getGraphTemporalClosure(options, true);
        }
        
        // update output if needed
        options.getApplication().updateOutput(options);
    }
    
    
    /**
     * Returns a String representation of the help message
     *
     * @return a String representation of the help message
     */
    private static String helpToString(){
        String help = "";
        help += "--------------------------------------------------------------------\n";
        help += "                             HELP MESSAGE                           \n";
        help += "--------------------------------------------------------------------\n";
        help += "\n";
        help += " Options:\n";
        help += "-h, -help                        Help message.\n";
        help += "-o <file>                        Output file name.\n";
        help += "-odir <dir>                      Output file directory.\n";
        help += "-grm <file.grm>                  XIP GRM file.\n";
        help += "-conf <file.cnf>                 Configuration file.\n";
        help += "-text <file.txt>                 Input file (if text only).\n";
        help += "-utf8                            Input is in UTF-8 (if absent, assume ISO-8859-1).\n";
        help += "-xmltext <file.xml>              Input file (if XML).\n";
        help += "-dir <dir>                       Input directory (if text only inside).\n";
        help += "-xmldir <dir>                    Input directory (if XML inside).\n";
        help += "-stdinput                        Input will be given by standard input online.\n";
        help += "-corpus <corpusname>             Name of the treated corpus.\n";
        help += "-app <appname>                   Name of the application.\n";
        help += "\n";
        help += "-printsn                         The sentence number will be printed.\n";
        help += "-printst                         The sentence text will be printed.\n";
        help += " \n Other options must be followed by a value and must be handled by " +
                "corpus- or application-specific classes.\n";
        help += " Parameters having name beginning by \"CORPUS_\" are sent to the specific Corpus object (if specified). \n";
        help += " Parameters having name beginning by \"APP_\" are sent to the specific Application object (if specified). \n";
        help += " Other parameters (non listed, and not CORPUS or APPLICATION) are not treated at the launch time but can be called at any moment. \n\n";
        help += "\n";
        return help;
    }
    
    
    /**
     * main
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        /**********************/
        /* GETTING PARAMETERS */
        /**********************/
        boolean error2;
        boolean error = false;
        int paramNumber = args.length;
        String param = "";
        Vector<String> confFileNames = new Vector<String>();
        boolean htmlOutput = false;
        Options options = new Options();
        Properties commandLineProperties = new Properties();
        
        try {
            if (paramNumber == 0) {
                System.err.println(helpToString());
                System.exit(-1);
            }
            for (int i = 0; i < paramNumber && !error; ++i) {
                param = args[i];
                // -h, -help : help
                if (param.equals("-h") || param.equalsIgnoreCase("-help")){
                    System.err.println(helpToString());
                    System.exit(-1);
                }
                // -o : output file name
                else if (param.equals("-o")) {
                    if (i != paramNumber - 1) {
                        ++i;
                        commandLineProperties.setProperty(Options.OUTPUT_FILE, args[i]);
                    } else {
                        error = true;
                    }
                }
                // -odir : output dir name
                else if (param.equals("-odir")) {
                    if (i != paramNumber - 1) {
                        ++i;
                        commandLineProperties.setProperty(Options.OUTPUT_DIR, args[i]);
                    } else {
                        error = true;
                    }
                }
                // -grm : grm file
                else if (param.equals("-grm")) {
                    if (i != paramNumber - 1) {
                        ++i;
                        commandLineProperties.setProperty(Options.GRM_FILE, args[i]);
                    } else {
                        error = true;
                    }
                }
                // -conf : configuration file
                else if (param.equals("-conf")){
                    if (i != paramNumber - 1) {
                        ++i;
                        confFileNames.add(args[i]);
                    } else {
                        error = true;
                    }
                }
                // -text : input file
                else if (param.equals("-text")) {
                    if (i != paramNumber - 1) {
                        ++i;
                        commandLineProperties.setProperty(Options.INPUT_FILE, args[i]);
                    } else {
                        error = true;
                    }
                }
                // -dir : input dir with text files
                else if (param.equals("-dir")) {
                    if (i != paramNumber - 1) {
                        ++i;
                        commandLineProperties.setProperty(Options.INPUT_DIR, args[i]);
                    } else {
                        error = true;
                    }
                }
                // -stdinput  standard input
                else if (param.equals("-stdinput")){
                    commandLineProperties.setProperty(Options.STD_INPUT, "TRUE");
                }
                // -xmltext
                else if (param.equals("-xmltext")){
                    if (i != paramNumber - 1) {
                        ++i;
                        commandLineProperties.setProperty(Options.INPUT_FILE, args[i]);
                        options.setIsInputInXML(true);
                    } else {
                        error = true;
                    }
                }
                // -xmldir : input dir with XML files
                else if (param.equals("-xmldir")) {
                    if (i != paramNumber - 1) {
                        ++i;
                        commandLineProperties.setProperty(Options.INPUT_DIR, args[i]);
                        options.setIsInputInXML(true);
                    } else {
                        error = true;
                    }
                }
                // -corpus
                else if (param.equals("-corpus")){
                    if (i != paramNumber - 1) {
                        ++i;
                        commandLineProperties.setProperty(Options.CORPUS, args[i]);
                    } else {
                        error = true;
                    }
                }
                // -app
                else if (param.equals("-app")){
                    if (i != paramNumber - 1) {
                        ++i;
                        commandLineProperties.setProperty(Options.APPLICATION, args[i]);
                    } else {
                        error = true;
                    }
                }
                // -printsn    print sentence number
                else if (param.equals("-printsn")){
                    commandLineProperties.setProperty(Options.PRINT_SN, "TRUE");
                }
                // -printst    print sentence text
                else if (param.equals("-printst")){
                    commandLineProperties.setProperty(Options.PRINT_ST, "TRUE");
                }
                // unknown parameter
                //  store it for later treatment
                else if (param.startsWith("-")){
                    if (i != paramNumber - 1) {
                        ++i;
                        commandLineProperties.setProperty(param.substring(1).toUpperCase(), args[i]);
                    } else {
                        error = true;
                    }
                } else if (param.equals("-utf8")) {
                    commandLineProperties.setProperty(Options.INPUT_UTF8, "TRUE");
                } else {
                    options.setInputFileName(args[i]);
                }
            }
            
            /*****************************
             * Error in argument passing
             *****************************/
            if (error)
                CommonLog.errorMessage("Error with argument " + param + ", must be followed by a value");
            
            /*********************************
             * MERGE COMMAND LINE OPTIONS
             *  WITH CONFIGURATION FILE
             *       OPTIONS
             *********************************/
            // Configuration file name
            for (String confFileName : confFileNames){
                try {
                    if (options.updateConfiguration(confFileName) == -1)
                        System.exit(-1);
                }catch (FileNotFoundException e) {
                    System.err.println("Configuration file " + confFileName + " not found.");
                    System.exit(-1);
                } catch (IOException e) {
                    System.err.println("IOException when setting configuration file.");
                    System.exit(-1);
                }
            }
            
            // priority to command line
            Enumeration e = commandLineProperties.propertyNames();
            String propName;
            String propValue;
            while (e.hasMoreElements()){
                propName = e.nextElement().toString();
                propValue = commandLineProperties.getProperty(propName);
                options.setProperty(propName, propValue);
            }
            
            
            
            /**********************/
            /*     Input type     */
            /**********************/
            // Standard input
            options.setIsInputStandard(options.getBooleanProperty(Options.STD_INPUT));
            if (options.isInputStandard()){
                options.setInputFileName("stdinput");
            }
            // XML
            options.setIsInputInXML(options.isInputInXML() ||
                    options.getBooleanProperty(Options.XML_INPUT));
            if (options.isInputInXML()){
                htmlOutput = false;
                options.setOutputType("");
            }
            
            /**************************/
            /* Corpus and application */
            /**************************/
            options.setCorpus(Corpus.getNewCorpus(options));
            options.setApplication(Application.getNewApplication(options));
            
            
            
            /******************************/
            /*    Arguments analysis      */
            /******************************/
            Set<String> allProperties = options.stringPropertyNames();
            for (String name : allProperties){
                // CORPUS-specific options
                if (name.startsWith("CORPUS_")){
                    if (options.getCorpus().setProperty(name, options.getProperty(name)) == -1){
                        CommonLog.errorMessage("Error with argument " + name);
                    }
                }
                
                // APPLICATION-specific options
                if (name.startsWith("APP_")){
                    if (options.getApplication().setProperty(name, options.getProperty(name)) == -1){
                        CommonLog.errorMessage("Error with argument " + name);
                    }
                }
                
                
                // temporal options
                else if (param.startsWith("TEMPORAL_")){
                    if (TemporalOptions.setProperty(name, options.getProperty(name)) == -1){
                        CommonLog.errorMessage("Error with argument " + name);
                    }
                }
            }
            
            
            
            /**********************/
            /*    Print options   */
            /**********************/
            // sentence number
            if (options.getBooleanProperty(Options.PRINT_SN))
                options.setPrintSentenceNumber(true);
            // sentence text
            if (options.getBooleanProperty(Options.PRINT_ST))
                options.setPrintSentenceString(true);
            
            
            /**********************/
            /*   ERROR MESSAGES   */
            /**********************/
            // input file
            if (!options.isInputStandard() && options.getInputFileName() == null && options.getInputFileDir() == null
                    && (options.getApplication() == null || options.getApplication().needsInput())){
                System.err.println("An input file or dir should be specified (option -text or -dir) !");
                System.exit(-1);
            }
            // GRM file
            if (options.getGrmFileName() == null){
                System.err.println("A GRM file should be specified (option -grm) !");
                System.exit(-1);
            }
            
            /**********************/
            /* LAUNCHING GRAMMAR  */
            /**********************/
            long startTime = System.currentTimeMillis();
            Main test = new Main(options);
            
            /***********************/
            /* Extra grammar files */
            /***********************/
            String extraFileList = options.getProperty(Options.XIP_FILES);
            if (extraFileList != null){
                String[] files = extraFileList.split(",");
                String fileName;
                for (int i = 0 ; i < files.length ; ++i){
                    fileName = files[i].trim();
                    File file = Common.getFile(fileName);
                    if (test.addParameterFile(file.getAbsolutePath()) == -1)
                        CommonLog.errorMessage("Error when adding parameter file " + fileName);
                }
            }
            
            
            /******************
             * Prepare output
             ******************/
            options.getApplication().openOutput(options);
            options.getApplication().preParsing(options);
            
            
            /*******************/
            /* LAUNCH PARSING  */
            /*******************/
            // Standard input
            if (options.isInputStandard()) {
                String line = "=";
                while (!line.trim().equals("")) {
                    System.out.println("Enter your text (type Enter to exit): ");
                    BufferedReader is = new BufferedReader(
                            new InputStreamReader(System.in));
                    line = is.readLine();
                    options.setInputString(line + " .");
                    test.parse(options);
                }
            }
            // file
            else if (options.getInputFileName() != null){
                System.out.println("------ processing file: " +"   " + options.getInputFileName());
                test.parse(options);
            }
            // directory
            else if (options.getInputFileDir() != null){
                String dirName = options.getInputFileDir();
                System.out.println("------ processing dir: " +"   " + dirName);
//                    final String suffix = ".txt";
//                    String dirName = args[1];
                File dir = new File(dirName);
//                    FilenameFilter filter = new FilenameFilter() {
//                        public boolean accept(File dir, String name) {
//                            return name.endsWith(suffix);
//                        }
//                    };
                String[] children = dir.list();
                if (children == null) {
                    CommonLog.errorMessage("Directory " + dirName + " doesn't exist!");
                } else if (children.length == 0) {
                    CommonLog.warningMessage("directory empty: " + dirName);
                    System.exit(-1);
                }
                for (int i=0; i<children.length; i++) {
                    // Get filename of file or directory
                    String fileName = options.getInputFileDir() + File.separator + children[i];
                    
                    if ((new File(fileName)).isFile()){
                        options.setInputFileName(fileName);
                        System.out.println("------     file: " + i +"   " + fileName);
                        test.parse(options);
                    }
                }
            }
            
            
            // elapsed time
            long endTime = System.currentTimeMillis();
            long elapsed = endTime - startTime;
            SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm:ss");
            dateFormat.setTimeZone(TimeZone.getTimeZone("GMT"));
            System.out.println("elapsed time:" + dateFormat.format(new Date(elapsed)));
            
        } catch (TemporalConsistencyException ex) {
            ex.printStackTrace();
//            System.exit(-1);
        } catch (TypedPropertyException ex){
            CommonLog.errorMessage(ex.getMessage());
        } catch (Exception ex) {
            System.err.println(ex.getMessage());
            ex.printStackTrace();
//            System.exit(-1);
        }
    }
    
}
