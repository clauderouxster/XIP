package src;

import com.xerox.jxip.Const;
import com.xerox.jxip.JXip;
import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipResultManager;
import java.io.*;
import org.apache.commons.io.FileUtils;
import static org.junit.Assert.assertTrue;
import org.junit.*;

/**
 *
 * @author grondeau
 */
public class TestCallback {

    private static JXip jxip = null;
    private XipResultManager xipResultManager;
    private static String inputRoot = "testJUnit/inputs/testCallBack";
    private static String outputRoot = "testJUnit/outputs/testCallBack";
    private static String outputRefRoot = "testJUnit/outputRefs/testCallBack";
    private String rootGram = "../GRAMMARS";
//    private static final Logger logger = Logger.getLogger(TestCallback.class.getName());
    private BufferedWriter out;

    public TestCallback() throws IOException, Exception {
        //System.out.println("+++++++++++++++testCallback");
        if (jxip == null) {
//            String log4jPropFileName = "testJUnit/config/log4j.properties";
//            File log4jPropFile = new File(log4jPropFileName);
//            System.out.println(log4jPropFile.getCanonicalPath());
//            if (log4jPropFile.exists()) {
//                PropertyConfigurator.configure(log4jPropFile.getCanonicalPath());
//            }
            boolean netbeans = true;
            if (netbeans) {
                jxip = new JXip(null, false);
            } else {
                jxip = new JXip();
            }
        }
    }

    @BeforeClass
    public static void setUpClass() throws Exception {
    }

    @AfterClass
    public static void tearDownClass() throws Exception {
    }

    @Before
    public void setUp() {
    }

    @After
    public void tearDown() {
    }

    @Test
    public void testCallbackEnglish() throws Exception {
        String grm = rootGram + "/ENGLISH/APPLICATIONS/EERQI/data/english_eerqi.grm";
        testCallback("english.txt", grm);
    }

    @Test
    public void testCallbackGerman() throws Exception {
        String grm = rootGram + "/GERMAN/APPLICATIONS/EERQI/data/german_eerqi.grm";
        testCallback("german.txt", grm);
    }

    @Test
    public void testCallbackFrench() throws Exception {
        String grm = rootGram + "/FRENCH/APPLICATIONS/EERQI/grmfiles/french_eerqi.grm";
        testCallback("french.txt", grm);
    }

    @Test
    public void testCallbackSwedish() throws Exception {
        String grm = rootGram + "/SWEDISH/APPLICATIONS/EERQI/data/swedish_eerqi.grm";
        testCallback("swedish.txt", grm);
    }

    private void testCallback(String fileName, String grm) throws Exception {
        String inFileName = inputRoot + "/" + fileName;
        String outFileName = outputRoot + "/" + fileName;
        String refFileName = outputRefRoot + "/" + fileName;
        System.out.println("loading: " + grm);
        int gHandler = loadGrammar(grm);
        System.out.println("testing: " + fileName);
        File outFile = new File(outFileName);
        File refFile = new File(refFileName);
        parse(new File(inFileName), gHandler, outFile);
        assertTrue("files are not equals: " + outFile + " " + refFile, FileUtils.contentEquals(outFile, refFile));
    }

    private int loadGrammar(String grm) throws Exception {
        //System.out.println("grm: " + grm);
        int gHandler = jxip.XipGrmFile(grm, 0, true);
        jxip.XipSetDisplayMode(gHandler, 0, Const.COL);
        // detect automatically if UTF8
        jxip.XipAddFlagDisplay(gHandler, Const.CHECK_INPUT_UTF8, Const.COL);
        jxip.XipAddFlagDisplay(gHandler, Const.DISPLAY_CONVERSION_UTF8, Const.COL);
        return gHandler;
    }

    public void parse(File inputFile, int gHandler, File outFile) {
        try {
            //System.out.println("parse: " + file);
            if (!inputFile.exists()) {
                System.out.println("the file does not exists:" + inputFile);
            } else {
                if (outFile.exists()) {
                    outFile.delete();
                }
                OutputStreamWriter osw = new OutputStreamWriter(
                        new FileOutputStream(outFile), "UTF-8");
                out = new BufferedWriter(osw);
                xipResultManager = new XipResultManager();
                xipResultManager.xipResultBuilderFromTextFile(gHandler, inputFile.getPath(),
                        this, "processSentence");
                xipResultManager.destroyNativeXipResult(gHandler);
                out.close();
            }
        } catch (Exception ex) {
            System.out.println("parse error:" + ex);
            System.exit(-1);
        }
    }

    public void processSentence() {
        //System.out.println("---------------------------- processSentence--------------------");
        try {
            XipResult xipRes = xipResultManager.getXipResult();
            if (xipRes != null) {
                out.write(xipRes.getSentenceString());
                out.write(xipRes.toString(true));
                //System.out.println(xipRes.toString(true));
            }
        } catch (Exception ex) {
            System.out.println("processSentence error: " + ex);
            PrintWriter stringWriter = new PrintWriter(new StringWriter());
            ex.printStackTrace(stringWriter);
            System.out.println(stringWriter);
        }
    }
}
