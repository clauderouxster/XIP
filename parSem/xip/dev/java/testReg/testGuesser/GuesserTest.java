package testGuesser;

import com.xerox.jxip.*;
import java.io.File;
import java.util.HashMap;

/**
 *
 * @author grondeau @created 16 October 2006
 */
public class GuesserTest {

    private static JXip jxip;
    private XipResultManager xipResultManager;
    private HashMap<String, Integer> langCnt;
    private int gHandler;
    private static String inputRoot;
    private String rootGram = "";
    private boolean error;

    public GuesserTest() throws Exception {
        error = false;
        boolean netbeans = false;
        //netbeans = true;
        if (netbeans) {
            jxip = new JXip(null, false);
            rootGram = "../GRAMMARS";
            inputRoot = "testReg/TestGuesser/inputs";
        } else {
            jxip = new JXip();
            rootGram = "../../../GRAMMARS";
            inputRoot = "inputs";
        }
        loadGrammars();
    }

    public static void main(String args[]) {
        try {
            GuesserTest test = new GuesserTest();
            test.langGuesserEnglish();
            test.langGuesserGerman();
            test.langGuesserFrench();
            test.langGuesserSwedish();
            if (test.error) {
                System.exit(-1);
            } else {
                System.exit(0);
            }
        } catch (Exception ex) {
            System.out.println(ex);
            System.exit(-1);
        }
    }

    private void testGuessLanguage(String fileName, String wLang) throws Exception {
        String guessLang = guessLanguage(new File(fileName));
        System.out.println("\nguessLanguage: " + fileName + " " + wLang);
        if (guessLang.equals(wLang)) {
            System.out.println("\ttest OK");
        } else {
            System.out.println("\ttest error:  wait:" + wLang + " computed:" + guessLang);
            error = true;
        }
    }

    public void langGuesserEnglish() throws Exception {
        testGuessLanguage(inputRoot + "/english.txt", "English");
    }

    public void langGuesserGerman() throws Exception {
        testGuessLanguage(inputRoot + "/german.txt", "German");
    }

    public void langGuesserFrench() throws Exception {
        testGuessLanguage(inputRoot + "/french.txt", "French");
    }

    public void langGuesserSwedish() throws Exception {
        testGuessLanguage(inputRoot + "/swedish.txt", "Swedish");
    }

    private void loadGrammars() {
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
            System.exit(-1);
        }
    }

    private int loadGrammar(String grm) throws Exception {
        int gHand = jxip.XipGrmFile(grm, 0, true);
        jxip.XipSetDisplayMode(gHand, 0, Const.COL);
        // detect automatically if UTF8
        jxip.XipAddFlagDisplay(gHand, Const.CHECK_INPUT_UTF8, Const.COL);
        // detect automatically the right grammar on each sentence
        jxip.XipAddFlagDisplay(gHand, Const.LANGUAGE_GUESSER, Const.COL);
        return gHand;
    }

    public String guessLanguage(File file) throws Exception {
        langCnt = new HashMap<String, Integer>();
        //System.out.println("guessLanguage: " + file);
        if (!file.exists()) {
            System.out.println("the file does not exists:" + file);
            return null;
        } else {
            xipResultManager = new XipResultManager();
            xipResultManager.xipResultBuilderFromTextFile(gHandler, file.getPath(), this, "processSentence");
        }
        int maxCnt = 0;
        String language = null;
        //System.out.println(langCnt);
        for (String key : langCnt.keySet()) {
            int value = langCnt.get(key);
            //System.out.println(key + " " + value);
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
