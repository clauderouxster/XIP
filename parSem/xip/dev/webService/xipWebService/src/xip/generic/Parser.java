package xip.generic;

import com.xerox.jxip.*;
import org.apache.log4j.Logger;

public class Parser {

    private String grmfile;
    private JXip jxip;
    private int gHandler;
    private XipResultManager xipResultManager;
    private String outputTextFileName;
    private String inputTextFileName;
    private Object objtoCall;
    private String callBack;
    private static final Logger logger = Logger.getLogger(Parser.class.getName());

    /**
     * Constructor for the Parser object.
     * @throws java.lang.Exception
     */
    public Parser() throws Exception {
        jxip = new JXip();
        logger.info("\n\n------------" + jxip.GetVersion() + "------------\n\n");
    }

    /**
     *
     * @return JXip instance.
     */
    public JXip getjXip() {
        return jxip;
    }

    /**
     * Initialize the parser.
     * @param objtoCall
     * @param callBack
     */
//    public void setCallBack(Object objtoCall, String callBack) {
//        this.objtoCall = objtoCall;
//        this.callBack = callBack;
//    }
    /**
     * Initialize the parser with the grammar.
     * @param grammar
     */
    public void setGrammar(Grammar grammar) {
        logger.info("using: " + grammar.getGrm());
        gHandler = grammar.getGHandler();
    }

    /**
     * Initialize the parser with the XML file to parse.
     * @param inputXmlFileName
     */
    public void setInputXmlFile(String inputXmlFileName) {
        this.inputTextFileName = inputXmlFileName;
    }

    /**
     * Initialize the parser with the XML file output.
     * @param outputXmlFileName 
     */
    public void setOutputXmlFileName(String outputXmlFileName) {
        this.outputTextFileName = outputXmlFileName;
    }

    /**
     * call the Xip parser
     */
    public void parse() {
        try {
            //xipResultManager = new XipResultManager();
            //xipResultManager.xipResultBuilderFromTextFile(gHandler, inputXmlFileName, objtoCall, callBack);
            jxip.XipSetDisplayMode(gHandler, Const.CHECK_INPUT_UTF8, 80);
            int depth = 2;
            char xml = Const.XML_OUTPUT;
            char apply = 1;
            jxip.XipParseFileXMLToFile(gHandler, inputTextFileName, outputTextFileName, depth, xml, apply);
        } catch (Exception ex) {
            logger.error(ex);
        }
    }
    /**
     *
     * @return a xipResultManager.
     */
//    public XipResultManager getXipResultManager() {
//        return xipResultManager;
//    }
}
