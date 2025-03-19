package src;

import com.xerox.jxip.Const;
import com.xerox.jxip.JXip;
import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipResultManager;
import com.xerox.jxip.XipUnit;
import java.io.*;
import java.io.IOException;
import java.util.HashMap;
import static org.junit.Assert.assertEquals;
import org.junit.*;

/**
 *
 * @author grondeau
 */
public class TestGuesser {

    private static JXip jxip = null;
    private XipResultManager xipResultManager;
    private HashMap<String, Integer> langCnt;
    private static int gHandler = -1;
    private static String inputRoot = "testJUnit/inputs/testGuesser";
//    private static final Logger logger = Logger.getLogger(TestGuesser.class.getName());

    public TestGuesser() throws IOException, Exception {
        //System.out.println("+++++++++++++++TestGuesser");
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
            loadGrammars();
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

    private void testGuessLanguage(String fileName, String wLang) {
        assertEquals("testGuessLanguage error: ", wLang, guessLanguage(new File(fileName)));
    }

    @Test
    public void langGuesserEnglish() {
        testGuessLanguage(inputRoot + "/english.txt", "English");
    }

    @Test
    public void langGuesserGerman() {
        testGuessLanguage(inputRoot + "/german.txt", "German");
    }

    @Test
    public void langGuesserFrench() {
        testGuessLanguage(inputRoot + "/french.txt", "French");
    }

    @Test
    public void langGuesserSwedish() {
        testGuessLanguage(inputRoot + "/swedish.txt", "Swedish");
    }

    private void loadGrammars() {
        String rootGram = "../GRAMMARS";
        String grm1 = rootGram + "/ENGLISH/APPLICATIONS/EERQI/data/english_eerqi.grm";
        String grm2 = rootGram + "/FRENCH/APPLICATIONS/EERQI/grmfiles/french_eerqi.grm";
        String grm3 = rootGram + "/GERMAN/APPLICATIONS/EERQI/data/german_eerqi.grm";
        String grm4 = rootGram + "/SWEDISH/APPLICATIONS/EERQI/data/swedish_eerqi.grm";
        try {
            gHandler = loadGrammar(grm1);
            loadGrammar(grm2);
            loadGrammar(grm3);
            loadGrammar(grm4);
        } catch (Exception ex) {
            System.out.println("grammar loading error: " + ex);
        }
    }

    private int loadGrammar(String grm) throws Exception {
        //System.out.println("grm: " + grm);
        int gHand = jxip.XipGrmFile(grm, 0, true);
        jxip.XipSetDisplayMode(gHand, 0, Const.COL);
        // detect automatically if UTF8
        jxip.XipAddFlagDisplay(gHand, Const.CHECK_INPUT_UTF8, Const.COL);
        // detect automatically the right grammar on each sentence
        jxip.XipAddFlagDisplay(gHand, Const.LANGUAGE_GUESSER, Const.COL);
        //System.out.println("gHandler: " + gHand);
        return gHand;
    }

    public String guessLanguage(File file) {
        langCnt = new HashMap<String, Integer>();
        try {
            System.out.println("guessLanguage: " + file);
            if (!file.exists()) {
                System.out.println("the file does not exists:" + file);
            } else {
                xipResultManager = new XipResultManager();
                xipResultManager.xipResultBuilderFromTextFile(gHandler, file.getPath(), this, "processSentence");
            }
        } catch (Exception ex) {
            System.out.println("guessLanguage error:" + ex);
        }
        int maxCnt = 0;
        String language = null;
        System.out.println(langCnt);
        for (String key : langCnt.keySet()) {
            int value = langCnt.get(key);
            System.out.println(key + " " + value);
            if (value > maxCnt) {
                maxCnt = value;
                language = key;
            }
        }
        if (language == null) {
            System.out.println("language not found [null] ++++++++++++");
            return "English";
        } else {
            if (language.equals("")) {
                System.out.println("language not found [] ++++++++++++");
                return "English";
            } else {
                return language;
            }
        }
    }

    public void processSentence() {
        //System.out.println("---------------------------- processSentence--------------------");
        try {
            XipResult xipRes = xipResultManager.getXipResult();
            //System.out.println(xipRes.getSentenceString());
            if (xipRes != null) {
                for (XipUnit xipUnit : xipRes.getXipUnits()) {
                    if (xipUnit != null) {
                        String language = xipUnit.getLanguage();
                        //System.out.println("language:" + language);
                        if (langCnt.containsKey(language)) {
                            int cnt = langCnt.get(language);
                            langCnt.put(language, ++cnt);
                        } else {
                            langCnt.put(language, 1);
                        }
                        break;
                    }
                }
            }
            //System.out.println(langCnt);
        } catch (Exception ex) {
            System.out.println("processSentence error: " + ex);
            //ex.printStackTrace();
        }
    }
}
