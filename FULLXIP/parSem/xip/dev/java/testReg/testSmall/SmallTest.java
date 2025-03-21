package testSmall;

import com.xerox.jxip.*;
import java.io.*;
import org.apache.commons.io.FileUtils;

/**
 *
 * @author grondeau @created 16 October 2006
 */
public class SmallTest {

    private static JXip jxip = null;
    private static JXip2 jxip2 = null;
    private XipResultManager xipResultManager;
    private static String inputRoot;
    private static String outputRoot;
    private static String outputRefRoot;
    private BufferedWriter out;
    private boolean error;
    private String gramRoot;
    private String root;
    private int gHandler;

    public SmallTest() throws Exception {
        boolean netbeans = false;
        //netbeans = true;
        if (netbeans) {
            jxip = new JXip(null, false);
            root = "testReg/testSmall/";
            inputRoot = root + "inputs";
            outputRoot = root + "outputs";
            outputRefRoot = root + "outputsRef";
            gramRoot = "../GRAMMARS/";
        } else {
            jxip = new JXip();
            jxip2 = new JXip2();
            root = "";
            inputRoot = root + "inputs";
            outputRoot = root + "outputs";
            outputRefRoot = root + "outputsRef";
            gramRoot = "../../../GRAMMARS/";
        }
        error = false;
    }

    private void loadAndParse(String grm, String fileName, String options)
            throws Exception {
        String inFileName = inputRoot + "/" + fileName;
        String outFileName = outputRoot + "/" + fileName;
        String refFileName = outputRefRoot + "/" + fileName;
        gHandler = loadGrammar(grm, options);
        System.out.println("loading: " + grm);
        System.out.println("parsing: " + inFileName);
        System.out.println("options: " + options);
        File outFile = new File(outFileName);
        File refFile = new File(refFileName);
        parse(new File(inFileName), gHandler, outFile);
        if (FileUtils.contentEquals(outFile, refFile)) {
            System.out.println("Test OK");
        } else {
            if (System.getProperty("os.name").equals("Darwin")
                    && (outFile.toString().equals("outputs/ita1"))) {
                // error in xip on mac only with italian
            } else {
                error = true;
                System.out.println("Test ERROR!!! these files are not equals: "
                        + outFile + " " + refFile);
            }
        }
    }

    private int loadGrammar(String grm, String options) throws Exception {
        //System.out.println("grm: " + grm);
        gHandler = jxip.XipGrmFile(grm, 0, true);
        jxip.XipSetDisplayMode(gHandler, 0, Const.COL);
        if (options.equals("utf8")) {
            jxip.XipSetDisplayMode(gHandler, Const.UTF8_INPUT, 80);
        } else {
            // detect automatically if UTF8
            jxip.XipAddFlagDisplay(gHandler, Const.CHECK_INPUT_UTF8, Const.COL);
            jxip.XipAddFlagDisplay(gHandler, Const.DISPLAY_CONVERSION_UTF8, Const.COL);
        }
        return gHandler;
    }

    public void parse(File inputFile, int gHandler, File outFile) throws Exception {
        if (!inputFile.exists()) {
            System.out.println("inputFile does not exist: " + inputFile.getAbsolutePath());
            System.exit(-1);
        }
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

    public void processSentence() {
        try {
            XipResult xipRes = xipResultManager.getXipResult();
            if (xipRes != null) {
                out.write("\n ---- the xipResult ----\n" + xipRes.toString(true));
                out.write("\n ---- the sentence ----\n" + xipRes.getSentenceString());
            }
        } catch (Exception ex) {
            System.out.println("processSentence error: " + ex);
            PrintWriter stringWriter = new PrintWriter(new StringWriter());
            ex.printStackTrace(stringWriter);
            System.out.println(stringWriter);
        }
    }

    private void ExceptionNoGrm() {
        try {
            gHandler = jxip.XipGrmFile(root + "grm/grm0.grm", 0, true);
        } catch (Exception exp) {
            String ex = exp.getMessage();
            String mesRef = "Cannot open:";
            if (ex.regionMatches(0, mesRef, 0, mesRef.length())) {
                System.out.println("ExceptionNoGrm ok");
            } else {
                error = true;
                System.out.println("ExceptionNoGrm error: " + ex);
            }
        }
    }

    private void ExceptionGramError1() {
        try {
            gHandler = jxip.XipGrmFile(root + "grm/grm1.grm", 0, true);
        } catch (Exception exp) {
            String ex = exp.getMessage();
            String mesRef = "Unknown NTM script file:";
            if (ex.regionMatches(0, mesRef, 0, mesRef.length())) {
                System.out.println("ExceptionGramError1 ok");
            } else {
                error = true;
                System.out.println("ExceptionGramError1 error: " + ex);
            }
        }
    }

    private void ExceptionGramError2() {
        try {
            gHandler = jxip.XipGrmFile(root + "grm/grm2.grm", 0, true);
        } catch (Exception exp) {
            String ex = exp.getMessage();
            String mesRef = "Unknown File:";
            if (ex.regionMatches(0, mesRef, 0, mesRef.length())) {
                System.out.println("ExceptionGramError2 ok");
            } else {
                error = true;
                System.out.println("ExceptionGramError2 error: " + ex);
            }
        }
    }

    private void ExceptionGramError3() {
        try {
            gHandler = jxip.XipGrmFile(root + "grm/grm3.grm", 0, true);
        } catch (Exception exp) {
            String ex = exp.getMessage();
            String mesRef = "Error, Unknown field:";
            if (ex.regionMatches(0, mesRef, 0, mesRef.length())) {
                System.out.println("ExceptionGramError3 ok");
            } else {
                error = true;
                System.out.println("ExceptionGramError3 error: " + ex);
            }
        }
    }

    private void ExceptionGramError4() {
        try {
            gHandler = jxip.XipGrmFile(root + "grm/grm4.grm", 0, true);
        } catch (Exception exp) {
            String ex = exp.getMessage();
            String mesRef = "Invalid License Number... Please contact Xerox to update your license...";
            if (ex.regionMatches(0, mesRef, 0, mesRef.length())) {
                System.out.println("ExceptionGramError4 ok");
            } else {
                error = true;
                System.out.println("ExceptionGramError4 error: " + ex);
            }
        }
    }

    private void ExceptionGramError5() {
        try {
            gHandler = jxip.XipGrmFile(root + "grm/grm5.grm", 0, true);
        } catch (Exception exp) {
            String ex = exp.getMessage();
            String mesRef = "License has Expired...";
            if (ex.regionMatches(0, mesRef, 0, mesRef.length())) {
                System.out.println("ExceptionGramError5 ok");
            } else {
                error = true;
                System.out.println("ExceptionGramError5 error: " + ex);
            }
        }
    }

    private void ExceptionGramError6() {
        try {
            gHandler = jxip.XipGrmFile(root + "grm/grm6.grm", 0, true);
        } catch (Exception exp) {
            String ex = exp.getMessage();
            String mesRef = "Error: Incorrect Code Name :rules";
            if (ex.regionMatches(0, mesRef, 0, mesRef.length())) {
                System.out.println("ExceptionGramError6 ok");
            } else {
                error = true;
                System.out.println("ExceptionGramError6 error: " + ex);
            }
        }
    }

    private void ExceptionGramError7() {
        try {
            gHandler = jxip.XipGrmFile(root + "grm/grm7.grm", 0, true);
        } catch (Exception exp) {
            String ex = exp.getMessage();
            if (exp.getClass().getName().equals("com.xerox.jxip.XipException")) {
                XipException ex1 = (XipException) exp;
                //System.out.println(ex1.getFileName().substring(ex1.getFileName().length()- 7));
                if (!ex1.getFileName().substring(ex1.getFileName().length() - 7).equals("en7.xip")) {
                    error = true;
                    System.out.println("fileName = " + ex1.getFileName());
                    System.out.println("ExceptionGramError7 error: bad file name");
                }
                if (ex1.getLine() != 33) {
                    error = true;
                    System.out.println("line = " + ex1.getLine());
                    System.out.println("ExceptionGramError7 error: bad line number");
                }
            } else {
                error = true;
                System.out.println("ExceptionGramError7 error: bad Exception");
            }
            String mesRef = "Error: Incorrect Code Name :";
            if (ex.regionMatches(0, mesRef, 0, mesRef.length())) {
                System.out.println("ExceptionGramError7 ok");
            } else {
                error = true;
                System.out.println("ExceptionGramError7 error: " + ex);
            }
        }
    }

    private void checkLoadGrammar() {
        ExceptionNoGrm();
        ExceptionGramError1();
        ExceptionGramError2();
        ExceptionGramError3();
        ExceptionGramError4();
        ExceptionGramError5();
        ExceptionGramError6();
        ExceptionGramError7();
    }

    //------------------ test rule space setting
    //check unvalid rule space
    private void ruleSpace1() {
        System.out.println("\nruleSpace1:");
        String grm = root + "grm/french/BASIC/french.grm";
        try {
            gHandler = jxip.XipGrmFile(grm, 0, true);
            String label = "ThisNotAValidDomain";
            System.out.println("\tdeactivateRuleSpace: check unvalid rule space  " + label);
            System.out.println("\texpected: error code");
            if (!jxip2.deactivateRuleSpace(gHandler, label)) {
                System.out.println("\tok");
            } else {
                System.out.println("\terror");
                error = true;
            }
        } catch (Exception exp) {
            error = true;
            System.out.println("exp = " + exp);
        }
    }

    //check no rule space set => by default all is valid
    private void ruleSpace2() {
        System.out.println("\nruleSpace2:");
        String grm = root + "grm/french/BASIC/french.grm";
        try {
            gHandler = jxip.XipGrmFile(grm, 0, true);
            String res = jxip.parseString(gHandler, ".", (char) 0, (char) 1);
            String expected = "root.general.domain1.domain2.space1.space2.final";
            System.out.println("\tdeactivateRuleSpace: check no rule space set => by default all is valid");
            System.out.println("\texpected: " + expected);
            if (res.equals(expected)) {
                System.out.println("\tok");
            } else {
                System.out.println("\tresult  :" + res);
                System.out.println("\terror");
                error = true;
            }
        } catch (Exception exp) {
            error = true;
            System.out.println("exp = " + exp);
        }
    }

    private void ruleSpace3() {
        System.out.println("\nruleSpace3:");
        String grm = root + "grm/french/BASIC/french.grm";
        try {
            gHandler = jxip.XipGrmFile(grm, 0, true);
            String label = "general.domain1";
            if (!jxip2.deactivateRuleSpace(gHandler, label)) {
                System.out.println("\terror during deactivateRuleSpace()");
            }
            String res = jxip.parseString(gHandler, ".", (char) 0, (char) 1);
            String expected = "root.general.domain2.final";
            System.out.println("\tdeactivateRuleSpace: " + label);
            System.out.println("\texpected: " + expected);
            if (res.equals(expected)) {
                System.out.println("\tok");
            } else {
                System.out.println("\tresult:" + res);
                System.out.println("\terror");
                error = true;
            }
        } catch (Exception exp) {
            error = true;
            System.out.println("exp = " + exp);
        }
    }

    private void ruleSpace4() {
        System.out.println("\nruleSpace4:");
        String grm = root + "grm/french/BASIC/french.grm";
        try {
            gHandler = jxip.XipGrmFile(grm, 0, true);
            String label = "general";
            if (!jxip2.deactivateRuleSpace(gHandler, label)) {
                System.out.println("\terror during deactivateRuleSpace()");
            }
            String res = jxip.parseString(gHandler, ".", (char) 0, (char) 1);
            String expected = "root.general.final";
            System.out.println("\tdeactivateRuleSpace: " + label);
            System.out.println("\texpected: " + expected);
            if (res.equals(expected)) {
                System.out.println("\tok");
            } else {
                System.out.println("\terror");
                System.out.println("    result:" + res);
                System.out.println("    expected:" + expected);
                error = true;
            }
        } catch (Exception exp) {
            error = true;
            System.out.println("exp = " + exp);
        }
    }

    //before activating a rule space it's better to deactivate all
    private void ruleSpace5() {
        System.out.println("\nruleSpace5:");
        String grm = root + "grm/french/BASIC/french.grm";
        try {
            gHandler = jxip.XipGrmFile(grm, 0, true);
            String label = "general";
            if (!jxip2.deactivateRuleSpace(gHandler, label)) {
                System.out.println("\terror during deactivateRuleSpace()");
            }
            System.out.println("\tdeactivateRuleSpace: " + label);
            label = "general.domain1.space1";
            if (!jxip2.activateRuleSpace(gHandler, label, false)) {
                System.out.println("\terror during activateRuleSpace()");
            }
            String res = jxip.parseString(gHandler, ".", (char) 0, (char) 1);
            String expected = "root.general.domain1.space1.final";
            System.out.println("\tactivateRuleSpace: " + label + "    with false");
            System.out.println("\texpected: " + expected);
            if (res.equals(expected)) {
                System.out.println("\tok");
            } else {
                System.out.println("\tresult:" + res);
                System.out.println("\terror");
                error = true;
            }
        } catch (Exception exp) {
            error = true;
            System.out.println("exp = " + exp);
        }
    }

    //before activating a rule space it's better to deactivate all
    private void ruleSpace6() {
        System.out.println("\nruleSpace6:");
        String grm = root + "grm/french/BASIC/french.grm";
        try {
            gHandler = jxip.XipGrmFile(grm, 0, true);
            String label = "general";
            if (!jxip2.deactivateRuleSpace(gHandler, label)) {
                System.out.println("\terror during deactivateRuleSpace()");
            }
            System.out.println("\tdeactivateRuleSpace: " + label);
            label = "general.domain1.space1";
            if (!jxip2.activateRuleSpace(gHandler, label, true)) {
                System.out.println("\terror during activateRuleSpace()");
            }
            String res = jxip.parseString(gHandler, ".", (char) 0, (char) 1);
            String expected = "root.general.space1.final";
            System.out.println("\tactivateRuleSpace: " + label + "    with true");
            System.out.println("\texpected: " + expected);
            if (res.equals(expected)) {
                System.out.println("\tok");
            } else {
                System.out.println("\tresult:" + res);
                System.out.println("\terror");
                error = true;
            }
        } catch (Exception exp) {
            error = true;
            System.out.println("exp = " + exp);
        }
    }

    private void checkRuleSpaceSetting() {
        ruleSpace1();
        ruleSpace2();
        ruleSpace3();
        ruleSpace4();
        ruleSpace5();
        ruleSpace6();
    }

    public static void main(String args[]) throws Exception {
        SmallTest test = new SmallTest();
        test.error = false;
        test.checkLoadGrammar();
        test.checkRuleSpaceSetting();
        //test.loadAndParse(test.gramRoot + "ENGLISH/GRMFILES/GRAM_NORM/gram_norm_entit.grm", "en1", "any");
        if (test.error) {
            System.exit(-1);
        } else {
            System.exit(0);
        }
    }
}
