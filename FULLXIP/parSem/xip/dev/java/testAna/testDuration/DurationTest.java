package testDuration;

import com.xerox.jxip.Const;
import com.xerox.jxip.JXip;
import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipResultManager;
import java.io.File;
import java.util.concurrent.TimeUnit;

/**
 * Description of the Class
 *
 * @author grondeau @created 16 October 2006
 */
public class DurationTest {

    XipResultManager xipResultManager;
    int gHandler;
    JXip jxip;
    static int cnt;
    String rootTestFile;

    public DurationTest() {
    }

    private void xipInit() {
        try {
            String grmFile;
            boolean netbeans = false;
            //netbeans = true;
            if (netbeans) {
                grmFile = "../../../xip/dev/GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm";
                rootTestFile = "testAna/testMem1/";
                jxip = new JXip(null, false);
            } else {
                grmFile = "../../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm";
                rootTestFile = "";
                jxip = new JXip();
            }
            gHandler = jxip.XipGrmFile(grmFile, 0, true);
            jxip.XipSetDisplayMode(gHandler, Const.UTF8_INPUT, 80);
            memInfo("after xipInit");
        } catch (Exception ex) {
            System.out.println("xipInit:" + ex);
            System.exit(-1);
        }
    }

    /**
     * Description of the Method
     */
    public void myCB1() {
        try {
            cnt++;
            //System.out.println("\n ---------------------------- DurationTest.myCB1 --------------------\n");
            XipResult xipRes = xipResultManager.getXipResult();
            boolean withFeature = true;
            String str = xipRes.toString(withFeature);
            //System.out.println("\n ---- the xipResult ----\n" + str);
            String str1 = xipRes.getSentenceString();
            //System.out.println("\n ---- the sentence ----\n" + str1);
            xipResultManager.destroyNativeXipResult(gHandler);
        } catch (Exception ex) {
            System.out.println("myCB1: " + ex);
            System.exit(-1);
        }
    }

    /**
     * Description of the Method
     *
     * @param inputFileName
     * @throws Exception
     */
    private void parse(String inputFileName) throws Exception {
        //System.out.println("--- parsing the file:" + inputFileName);
        cnt = 0;
        File file = new File(inputFileName);
        if (!(file.exists())) {
            System.out.println("file not found: " + file.getAbsolutePath());
            System.exit(-1);
        }
        xipResultManager = new XipResultManager();
        xipResultManager.xipResultBuilderFromTextFile(gHandler, file.getAbsolutePath(), this, "myCB1");
        xipResultManager.destroyNativeXipResult(gHandler);
    }

    private void evalParse(String fileName) {
        try {
            File file = new File(fileName);
            long begin = System.currentTimeMillis();
            parse(fileName);
            long end = System.currentTimeMillis();
            String mes = "";
            mes += "duration: " + formatFromMillis(end - begin);
            mes += "\tsentence: " + cnt;
            mes += "\tfileSize: " + file.length() + " bytes";
            mes += "\tafterParsing: " + file.getName();
            memInfo(mes);
        } catch (Exception ex) {
            System.out.println("evalParse: " + ex);
            System.exit(-1);
        }
    }

    public String formatFromMillis(long millis) {
        long minutes = TimeUnit.MILLISECONDS.toMinutes(millis);
        long seconds = TimeUnit.MILLISECONDS.toSeconds(millis) - TimeUnit.MINUTES.toSeconds(minutes);
        return String.format("%d min %d sec", minutes, seconds);
    }

    private void test0() {
        xipInit();
        evalParse(rootTestFile + "input-1000");
        evalParse(rootTestFile + "input-5000");
        evalParse(rootTestFile + "input-10000");
        evalParse(rootTestFile + "input-20000");
        evalParse(rootTestFile + "input-30000");
        evalParse(rootTestFile + "input-50000");
        evalParse(rootTestFile + "input-100000");
    }

    private long memInfo(String info) {
        long mem = 0, totMem, freeMem;
        try {
            System.gc(); //run in a thread
            Thread.sleep(3000); //wait gc thread finish
            totMem = Runtime.getRuntime().totalMemory();
            freeMem = Runtime.getRuntime().freeMemory();
            mem = totMem - freeMem;
            String mes = "";
            mes += "totalMem: " + totMem;
            mes += "\tfreeMem: " + freeMem;
            mes += "\tusedMem: " + mem;
            mes += "\t" + info;
            System.out.println(mes);
        } catch (Exception ex) {
            System.out.println("memInfo: " + ex);
            System.exit(-1);
        }
        return mem;
    }

    private byte[] allocate100Mb() throws InterruptedException {
        byte[] ar = new byte[100000000];
        memInfo("after allocate: " + ar.length);
        return ar;
    }

    private void test1() {
        try {
            byte[] ar1 = allocate100Mb();
            byte[] ar2 = allocate100Mb();
            byte[] ar3 = allocate100Mb();
            byte[] ar4 = allocate100Mb(); //error with Xmx512m
            byte[] ar5 = allocate100Mb();
            byte[] ar6 = allocate100Mb();
            byte[] ar7 = allocate100Mb();
            //byte[] ar8 = allocate100Mb(); //error with Xmx1024m
        } catch (Exception ex) {
            System.out.println("test1: " + ex);
            System.exit(-1);
        }
    }

    /**
     * Description of the Method
     *
     * @param args Description of the Parameter
     * @throws Exception
     */
    public static void main(String args[]) throws Exception {
        DurationTest test = new DurationTest();
        test.memInfo("begin");
        test.test0();
        //test.test1();
        test.memInfo("end");
    }
}
