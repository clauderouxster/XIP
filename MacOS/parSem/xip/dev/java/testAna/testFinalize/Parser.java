package testFinalize;

import com.xerox.jxip.Const;
import com.xerox.jxip.JXip;
import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipResultManager;
import java.io.*;

/**
 *
 * @author grondeau @created 16 octobre 2006
 */
public class Parser {

    private final JXip jxip;
    private XipResultManager xipResultManager;
    private final String grm;

    public Parser() throws Exception {
        //boolean netbeans = true;
        boolean netbeans = false;
        if (netbeans) {
            jxip = new JXip(null, false);
            grm = "../GRAMMARS/ENGLISH/GRMFILES/GRAM_NORM/gram_norm_entit.grm";
        } else {
            jxip = new JXip();
            grm = "../../../GRAMMARS/ENGLISH/GRMFILES/GRAM_NORM/gram_norm_entit.grm";
        }
    }

    synchronized public void loadAndParse() throws Exception {
        int gHandler = loadGrammar(grm);
        parse(gHandler);
    }

    private int loadGrammar(String grm) throws Exception {
        System.out.println("grm: " + grm);
        int gHandler = jxip.XipGrmFile(grm, 0, true);
        jxip.XipSetDisplayMode(gHandler, 0, Const.COL);
        jxip.XipAddFlagDisplay(gHandler, Const.CHECK_INPUT_UTF8, Const.COL);
        jxip.XipAddFlagDisplay(gHandler, Const.DISPLAY_CONVERSION_UTF8, Const.COL);
        return gHandler;
    }

    private void parse(int gHandler) throws Exception {
        xipResultManager = new XipResultManager();
        xipResultManager.xipResultBuilderFromString(gHandler, "Peter is happy. The cat is black.", this, "processSentence");
        xipResultManager.destroyNativeXipResult(gHandler);
    }

    public void processSentence() {
        try {
            XipResult xipRes = xipResultManager.getXipResult();
            if (xipRes != null) {
                System.out.println("\n ---- the xipResult ----\n" + xipRes.toString(true));
                System.out.println("\n ---- the sentence ----\n" + xipRes.getSentenceString());
            }
        } catch (Exception ex) {
            System.out.println("processSentence error: " + ex);
            PrintWriter stringWriter = new PrintWriter(new StringWriter());
            ex.printStackTrace(stringWriter);
            System.out.println(stringWriter);
        }
    }

    public void checkFinalizer() throws InterruptedException {
        Runtime.getRuntime().runFinalization();
        Thread.sleep(1000);
        System.gc();
        Thread.sleep(2000);
    }

    @Override
    public void finalize() throws Throwable {
        try {
            System.out.println("++++++++++++++++++ XipFreeAllGrammars start +++++++++++++++");
            jxip.XipFreeAllGrammars();
            System.out.println("++++++++++++++++++ XipFreeAllGrammars done +++++++++++++++");
        } finally {
            super.finalize();
        }
    }
}
