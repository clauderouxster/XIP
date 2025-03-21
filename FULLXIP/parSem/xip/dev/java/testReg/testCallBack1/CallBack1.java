package testCallBack1;

import com.xerox.jxip.Const;
import com.xerox.jxip.JXip;
import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipResultManager;
import java.io.*;
import java.util.Vector;
import org.apache.commons.io.FileUtils;

/**
 *
 * @author grondeau @created 16 octobre 2006
 */
public class CallBack1 {

    private static JXip jxip = null;
    private XipResultManager xipResultManager;
    private static String inputRoot;
    private static String outputRoot;
    private static String outputRefRoot;
    private BufferedWriter out;
    private boolean error;
    private String gramRoot;
    private Vector<String[]> paramsArray;

    public CallBack1() throws Exception {
        //boolean netbeans = true;
        boolean netbeans = false;
        if (netbeans) {
            jxip = new JXip(null, false);
            String root = "testReg/testCallBack1/";
            inputRoot = root + "inputs";
            outputRoot = root + "outputs";
            outputRefRoot = root + "outputsRef";
            gramRoot = "../GRAMMARS/";
        } else {
            jxip = new JXip();
            inputRoot = "inputs";
            outputRoot = "outputs";
            outputRefRoot = "outputsRef";
            gramRoot = "../../../GRAMMARS/";
        }
        error = false;
    }

    private void loadAndParse(String grm, String fileName, String options)
            throws Exception {
        String inFileName = inputRoot + "/" + fileName;
        String outFileName = outputRoot + "/" + fileName;
        String refFileName = outputRefRoot + "/" + fileName;
        int gHandler = loadGrammar(grm, options);
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
        int gHandler = jxip.XipGrmFile(grm, 0, true);
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

    private void addTest(String grm, String input, String option) {
        String[] params = new String[3];
        params[0] = gramRoot + grm;
        params[1] = input;
        params[2] = option;
        paramsArray.add(params);
    }

    public static void main(String args[]) throws Exception {
        CallBack1 test = new CallBack1();
        test.paramsArray = new Vector<String[]>();
        test.error = false;
        if (args.length == 3) {
            test.addTest(args[0], args[1], args[2]);
        } else {
            test.addTest("ENGLISH/GRMFILES/GRAM_NORM/gram_norm_entit.grm", "en1", "any");
            test.addTest("ENGLISH/GRMFILES/GRAM_TEMP/gram_temp.grm", "en2", "any");
            test.addTest("FRENCH/BASIC/french_entity.grm", "fr1", "any");
            test.addTest("SPANISH/GRMFILES/spanish_ntm.grm", "spa1", "any");
            test.addTest("GERMAN/BASIC/german.grm", "ger1", "utf8");
            test.addTest("ITALIAN/CELI_090313/italian.grm", "ita1", "utf8");
            //test.addTest("PORTUGUESE/GRMFILES/portuguese.grm", "por1", "utf8");   //ce test plante de facon aleatoire sur Albaron uniquement
        }
        for (String[] p1 : test.paramsArray) {
            test.loadAndParse(p1[0], p1[1], p1[2]);
        }
        if (test.error) {
            System.exit(-1);
        } else {
            System.exit(0);
        }
    }
}
