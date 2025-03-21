package xip.ws.server;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Properties;
import org.apache.log4j.Logger;
import xip.generic.Grammar;
import xip.generic.Parser;

/**
 *
 * @author grondeau
 */
public class XipServer {

    public static XipServer server = null;
    private static Parser parser;
    private static final Logger logger = Logger.getLogger(XipServer.class.getName());
    private static String supportedLanguage;
    private long totalServerTime;
    private Hashtable<String, Grammar> grammars;

    public XipServer() {
        logger.info("------ parser init. -------");
        supportedLanguage = "";
        grammars = new Hashtable<String, Grammar>();
        try {
            parser = new Parser();
            if (!loadGrammars()) {
                System.exit(-1);
            }
        } catch (Exception ex) {
            logger.fatal("XipServer:" + ex.toString());
            ex.printStackTrace();
            System.exit(-1);
        }
    }

    public static XipServer getInstance() {
        //logger.info("get XipServer instance");
        if (parser == null) {
            server = new XipServer();
        }
        return server;
    }

    private boolean loadGrammars() {
        logger.info("------ loading grammars ------");
        // Read properties file.
        Properties sysProp = System.getProperties();
        String propFileName;
        if (sysProp.get("os.name").equals("Linux")) {
            propFileName = "grammars.properties";
        } else {
            propFileName = "c:\\users\\grondeau\\grammars.properties";
        }
        logger.info("propFileName: " + propFileName);
        Properties properties = new Properties();
        try {
            properties.load(new FileInputStream(propFileName));
        } catch (IOException ex) {
            logger.error("error: " + ex.toString());
            return false;
        }
        //logger.info("properties loaded: \n" + properties.toString());
        for (Enumeration enum1 = properties.propertyNames(); enum1.hasMoreElements();) {
            String gramName = (String) enum1.nextElement();
            logger.info("gramName: " + gramName);
            String grm = (String) properties.get(gramName);
            File file = new File(grm);
            grm = file.toString();
            logger.info("grm: " + grm);
            if (!file.isFile()) {
                logger.info("the grm file does not exist !!!!!    gramName: " + gramName + "      grm: " + grm);
                return false;
            }
            logger.info("gramName: " + gramName + "      grm: " + grm);
            Grammar gram = new Grammar(parser.getjXip());
            gram.setGrm(grm);
            gram.loadGrammar();
            grammars.put(gramName, gram);
            if (supportedLanguage == null) {
                supportedLanguage = gramName;
            } else {
                supportedLanguage += "," + gramName;
            }
        }
        return true;
    }

    /**
     * 
     * @return the supported languages
     */
    synchronized public String getSupportedLanguage() {
        logger.info("------- supported language: " + supportedLanguage + "-----------");
        return supportedLanguage;
    }

    synchronized void clientTotalResponseTime(long time) {
        logger.info("clientTotalResponseTime: " + time + "ms     data transfert time: " + (time - totalServerTime) + " ms    server response time: " + totalServerTime + " ms");
    }

    synchronized String parseString(String input, String language) {
        logger.info("\n------------- parseString   -----------");
        logger.info("language: " + language);
        String res = "";
        if ((input == null) || (input.length() == 0)) {
            logger.error("input is empty: " + input);
            return "ERROR: input is empty";
        }
        if ((language == null) || (language.length() == 0)) {
            logger.error("language is not defined: " + language);
            return "ERROR: language is not defined";
        }
        try {
            //select grammar
            if (grammars.containsKey(language)) {
                Grammar gram = grammars.get(language);
                parser.setGrammar(gram);
            } else {
                logger.error("unknow language: " + language);
                String languages = "";
                for (Enumeration enum1 = (grammars.keys()); enum1.hasMoreElements();) {
                    languages += " " + enum1.nextElement();
                }
                logger.error("languages: " + languages);
                return "ERROR: unknow language: " + language;
            }
            logger.info("input size: " + input.length());
            // save the input string to a temporary file
            long startTime = System.currentTimeMillis();
            totalServerTime = startTime;
            File inputTempFile = null;
            inputTempFile = File.createTempFile("input", ".tmp");
            BufferedWriter out = new BufferedWriter(new FileWriter(inputTempFile));
            out.write(input);
            out.close();
            logger.info("time write input to file: " + ((System.currentTimeMillis() - startTime) / 1000) + "s");
            File outputTempFile = null;
            // parse the temporary input file
            outputTempFile = File.createTempFile("output", ".tmp");
            parser.setOutputXmlFileName(outputTempFile.getCanonicalPath());
            parser.setInputXmlFile(inputTempFile.getCanonicalPath());
            //System.out.println("XipServer::parseString() outputTempFile = " + outputTempFile.getCanonicalPath());
            //System.out.println("XipServer::parseString() inputTempFile = " + inputTempFile.getCanonicalPath());
            startTime = System.currentTimeMillis();
            parser.parse();
            logger.info("time to parse: " + ((System.currentTimeMillis() - startTime) / 1000) + "s");
            //read the output file generated by xip, and store it in a String
            startTime = System.currentTimeMillis();
            StringBuffer strBuf = new StringBuffer();
            FileInputStream fstream = new FileInputStream(outputTempFile);
            DataInputStream in = new DataInputStream(fstream);
            BufferedReader br = new BufferedReader(new InputStreamReader(in));
            String strLine;
            while ((strLine = br.readLine()) != null) {
                strBuf.append(strLine);
            }
            in.close();
            res = strBuf.toString();
            if (inputTempFile.exists()) {
                inputTempFile.delete();
            }
            if (outputTempFile.exists()) {
                outputTempFile.delete();
            }
            logger.info("output size: " + res.length());
            logger.info("time to write the output: " + ((System.currentTimeMillis() - startTime) / 1000) + "s");
            totalServerTime = System.currentTimeMillis() - totalServerTime;
            //System.out.println("XipServer::parseString() res = \n" + res);
            return res;
        } catch (IOException ex) {
            logger.error(ex.toString());
            return "ERROR: " + ex.toString();
        }
    }
}
