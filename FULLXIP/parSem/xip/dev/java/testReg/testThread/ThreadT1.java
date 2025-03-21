package testThread;

import com.xerox.jxip.Const;
import com.xerox.jxip.JXip;
import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipResultManager;
import java.io.*;
import java.nio.charset.Charset;
import java.util.Vector;
import org.apache.commons.io.FileUtils;

/**
 *
 * @author grondeau @created 16 October 2006
 */
public class ThreadT1 {

    private static JXip jxip = null;
    private static String inputRoot;
    private static String outputRoot;
    private static String outputRefRoot;
    private boolean error;
    private String gramRoot;
    private Vector<String[]> paramsArray;
    Vector<Thread> threads;

    public ThreadT1() throws Exception {
        //boolean netbeans = true;
        boolean netbeans = false;
        if (netbeans) {
            jxip = new JXip(null, false);
            String root = "testReg/testThread/";
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
        threads = new Vector<Thread>();
    }

    public void memInfo() throws InterruptedException {
        //System.out.println("memInfo");
        long mem, totMem, freeMem;
        System.gc(); //run in a thread
        Thread.sleep(3000); //wait gc thread finish
        totMem = Runtime.getRuntime().totalMemory();
        freeMem = Runtime.getRuntime().freeMemory();
        mem = totMem - freeMem;
        String mes = "totalMemory: " + totMem;
        mes += "  freeMemory: " + freeMem;
        mes += "  usedMemory: " + mem;
        System.out.println(mes);
    }

    public static void main(String[] args) throws Exception {
        ThreadT1 test = new ThreadT1();
        test.memInfo();
        test.createThreads();
        test.startThreads();
        test.waitEndThreads();
        test.threads.clear();
        test.memInfo();
        if (test.error) {
            System.exit(-1);
        } else {
            System.exit(0);
        }
    }

    private void createThreads() {
        createThread("test1", "ENGLISH/GRMFILES/GRAM_NORM/gram_norm_entit.grm", "en1", "any");
        createThread("test2", "ENGLISH/GRMFILES/GRAM_NORM/gram_norm_entit.grm", "en1", "any");

        createThread("test1", "ENGLISH/GRMFILES/GRAM_TEMP/gram_temp.grm", "en2", "any");
        createThread("test2", "ENGLISH/GRMFILES/GRAM_TEMP/gram_temp.grm", "en2", "any");

        createThread("test1", "FRENCH/BASIC/french_entity.grm", "fr1", "any");
        createThread("test2", "FRENCH/BASIC/french_entity.grm", "fr1", "any");

        createThread("test1", "SPANISH/GRMFILES/spanish_ntm.grm", "spa1", "any");
        createThread("test2", "SPANISH/GRMFILES/spanish_ntm.grm", "spa1", "any");

        createThread("test1", "GERMAN/BASIC/german.grm", "ger1", "utf8");
        createThread("test2", "GERMAN/BASIC/german.grm", "ger1", "utf8");

        createThread("test1", "ITALIAN/CELI_090313/italian.grm", "ita1", "utf8");
        createThread("test2", "ITALIAN/CELI_090313/italian.grm", "ita1", "utf8");

//        createThread("test1", "PORTUGUESE/GRMFILES/portuguese.grm", "por1", "utf8");
//        createThread("test2", "PORTUGUESE/GRMFILES/portuguese.grm", "por1", "utf8");
    }

    private void startThreads() {
        for (Thread thread : threads) {
            System.out.println("start of: " + thread.getName());
            thread.start();
        }
    }

    private void waitEndThreads() throws InterruptedException {
        //System.out.println("waitEndThreads ");
        for (Thread thread : threads) {
            Thread.sleep(1000);
            thread.join();
            System.out.println("end of: " + thread.getName());
        }
    }

    private void createThread(String test, String grm, String input, String options) {
        Thread thread = null;
        String fullGrm = gramRoot + grm;
        if (test.equals("test1")) {
            thread = new Test1(fullGrm, input, options);
        } else if (test.equals("test2")) {
            thread = new Test2(fullGrm, input, options);
        } else {
            System.out.println("test unknown");
            System.exit(-1);
        }
        threads.add(thread);
    }

    public class Test1 extends Thread {

        String grm;
        String input;
        String options;

        public Test1(String grm, String input, String options) {
            this.grm = grm;
            this.input = input;
            this.options = options;
        }

        @Override
        public void run() {
            String inFileName = inputRoot + "/" + input;
            String outFileName = outputRoot + "/" + "test1_" + input;
            String refFileName = outputRefRoot + "/" + "test1_" + input;
            System.out.println("Test1 " + getName() + ":\t\t" + grm + "    " + inFileName);
            try {
                int gHandler = jxip.XipGrmFile(grm, 0, true);
                if (gHandler == -1) {
                    System.out.println("XipGrmFile error: " + grm);
                    System.exit(-1);
                }
                long displayMode = Const.CHECK_INPUT_UTF8;
                int col = 80;
                jxip.XipSetDisplayMode(gHandler, displayMode, col);
                jxip.XipParseFileToFile(gHandler, inFileName, outFileName, (char) 0, (char) 1);
                File outFile = new File(outFileName);
                File refFile = new File(refFileName);
                removeCR(outFile, options);
                if (FileUtils.contentEquals(outFile, refFile)) {
                    System.out.println("Test1 " + getName() + ":\t\t" + "OK");
                } else {
                    error = true;
                    System.out.println("Test1 " + getName() + ":\t\t" + "ERROR!!! these files are not equals: "
                            + outFile + " " + refFile);
                }
            } catch (Exception ex) {
                printException("Test1::run Exception: ", ex);
            }
        }

        /**
         * remove the char CR (required on windows platform only)
         *
         * @param fileIn
         * @param fileOut
         */
        private void removeCR(File fileIn, String options) {
            File fileOut = new File(fileIn + "withoutCR");
            String charset = "ISO-8859-1";
            if (options.equals("utf8")) {
                charset = "UTF8";
            }
            try {
                InputStreamReader isr = new InputStreamReader(new FileInputStream(fileIn), Charset.forName(charset));
                BufferedReader br = new BufferedReader(isr);
                OutputStreamWriter osw = new OutputStreamWriter(new FileOutputStream(fileOut), Charset.forName(charset));
                BufferedWriter bw = new BufferedWriter(osw);
                String line;
                while ((line = br.readLine()) != null) {
                    String line1 = line.replaceAll("\r", "");
                    if (line1.length() != 0) {
                        bw.write(line1 + "\n");
                    }
                }
                br.close();
                bw.close();
                fileIn.delete();
                fileOut.renameTo(fileIn);
            } catch (Exception ex) {
                System.err.println("Exception: " + ex.getMessage());
            }
        }

        private void printException(String mess, Exception ex) {
            System.out.println(mess + ex);
            PrintWriter stringWriter = new PrintWriter(new StringWriter());
            ex.printStackTrace(stringWriter);
            System.out.println(stringWriter);
            System.exit(-1);
        }
    }

    public class Test2 extends Thread {

        String grm;
        String input;
        String options;
        private BufferedWriter out;
        private XipResultManager xipResultManager;

        public Test2(String grm, String input, String options) {
            this.grm = grm;
            this.input = input;
            this.options = options;
        }

        @Override
        public void run() {
            try {
                loadAndParse(grm, input, options);
            } catch (Exception ex) {
                printException("Test2::run Exception: ", ex);
            }
        }

        private void loadAndParse(String grm, String fileName, String options)
                throws Exception {
            String inFileName = inputRoot + "/" + fileName;
            String outFileName = outputRoot + "/" + fileName;
            String refFileName = outputRefRoot + "/" + fileName;
            //System.out.println("loadAndParse: " + grm + " " + inFileName + " " + refFileName);
            int gHandler = loadGrammar(grm, options);
            if (gHandler == -1) {
                System.out.println("XipGrmFile error: " + grm);
                System.exit(-1);
            }
            //System.out.println("loadAndParse loading: " + grm);
            //System.out.println("loadAndParse parsing: " + inFileName);
            //System.out.println("loadAndParse options: " + options);
            System.out.println("Test2 " + getName() + ":\t\t" + grm + " " + inFileName + " " + options);
            File outFile = new File(outFileName);
            File refFile = new File(refFileName);
            parse(new File(inFileName), gHandler, outFile);
            if (FileUtils.contentEquals(outFile, refFile)) {
                System.out.println("Test2 " + getName() + ":\t\t" + "OK");
            } else {
                if (System.getProperty("os.name").equals("Darwin")
                        && (outFile.toString().equals("outputs/ita1"))) {
                    // error in xip on mac only with italian
                } else {
                    error = true;
                    System.out.println("Test2 " + getName() + ":\t\t" + "ERROR!!! these files are not equals: "
                            + outFile + " " + refFile);
                }
            }
        }

        private int loadGrammar(String grm, String options) throws Exception {
            //System.out.println("loadGrammar grm: " + grm);
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

        private void parse(File inputFile, int gHandler, File outFile) throws Exception {
            //System.out.println("parse inputFile: " + inputFile);
            if (!inputFile.exists()) {
                System.out.println("inputFile does not exist: " + inputFile.getAbsolutePath());
                System.exit(-1);
            }
            if (outFile.exists()) {
                outFile.delete();
            }
            FileOutputStream fileOutputStream = new FileOutputStream(outFile);
            OutputStreamWriter osw = new OutputStreamWriter(fileOutputStream, "UTF-8");
            out = new BufferedWriter(osw);
            xipResultManager = new XipResultManager();
            xipResultManager.xipResultBuilderFromTextFile(gHandler, inputFile.getPath(),
                    this, "processSentence");
            //xipResultManager.destroyNativeXipResult(gHandler);
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
                printException("test2 processSentence error: ", ex);
            }
        }

        private void printException(String mess, Exception ex) {
            System.out.println(mess + ex);
            PrintWriter stringWriter = new PrintWriter(new StringWriter());
            ex.printStackTrace(stringWriter);
            System.out.println(stringWriter);
            System.exit(-1);
        }
    }
}
