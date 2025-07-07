
import com.xerox.jxip.Const;
import com.xerox.jxip.JXip;
import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipFeature;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipResultManager;
import com.xerox.jxip.XipUnit;
import java.io.File;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Vector;
import java.util.concurrent.TimeUnit;

/**
 * Description of the Class
 *
 * @author grondeau @created april 2014
 */
public class Test {

    XipResultManager xipResultManager;
    int gHandler;
    JXip jxip;
    static int cnt;
    String rootTestFile;
    private static final HashSet<String> namesOfNE = new HashSet(Arrays.asList("DATE", "PERSON", "ORGANISATION", "LOCORG", "PRODUCT, LOCATION", "URL", "PHONE", "EMAIL"));
    private static final HashSet<String> namesOfIgnoredCategory = new HashSet(Arrays.asList("PREP", "PUNCT", "DET", "CONJ", "PRON", "AUX", "DIG", "PART"));

    public Test() {
    }

    private void xipInit() {
        try {
            jxip = new JXip();
            String grmFile = "../../../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm";
            System.out.println("grm: " + grmFile);
            gHandler = jxip.XipGrmFile(grmFile, 0, true);
            jxip.XipSetDisplayMode(gHandler, Const.UTF8_INPUT, 80);
            memInfo("after xipInit");
        } catch (Exception ex) {
            System.out.println("xipInit:" + ex);
            System.exit(-1);
        }
    }

    public void callBack1() {
        cnt++;
        XipResult xipRes = xipResultManager.getXipResult();
        boolean withFeature = true;
        String str = xipRes.toString(withFeature);
        //System.out.println("\n ---- the xipResult ----\n" + str);
        String str1 = xipRes.getSentenceString();
        System.out.println("\n ---- the sentence ----\n" + str1);
    }

    public void callBack2() throws Exception {
        cnt++;
        XipResult xipRes = xipResultManager.getXipResult();
        //logger.debug(xipRes.getSentenceString());
        for (XipUnit xipUnit : xipRes.getXipUnits()) {
            XipNode root = xipUnit.getRoot();
            processXipNodes(root, "", root.getCategory() + root.getIndex());
            processDependencies(xipUnit);
        }
    }

    private void processXipNodes(XipNode xipNode, String deep, String previous) {
        for (XipNode xipNodeDaughter : xipNode.getDaughters()) {
            if (xipNodeDaughter.isLeaf()) {
                System.out.println(deep + "LEAF       cat:" + xipNodeDaughter.getCategory() + xipNodeDaughter.getIndex()
                        + " prev:" + previous + " surface:" + xipNodeDaughter.getSurface()
                        + "  lemma:" + xipNodeDaughter.getLemma() + " " + xipNodeDaughter.getFeatures());
                if (!xipNodeDaughter.isFicticious()) {
                    if (!namesOfIgnoredCategory.contains(xipNodeDaughter.getCategory())) {
                        if (xipNodeDaughter.getLemma().length() > 1) { //ignore lemma with only one letter
                            System.out.println(xipNodeDaughter.getLemma().toLowerCase());
                        }
                    }
                }
            } else {
                Vector<XipDependency> dependencies = xipNodeDaughter.getDependencies();
                System.out.println(deep + "NOT LEAF: " + " cat:" + xipNodeDaughter.getCategory() + xipNodeDaughter.getIndex()
                        + " prev:" + previous + " surface:" + xipNodeDaughter.getSurface()
                        + "  lemma:" + xipNodeDaughter.getLemma() + " " + dependencies);
                if (!xipNodeDaughter.isFicticious()) {
                    boolean foundNE = false;
                    for (XipDependency dep : dependencies) {
                        String dependencyName = dep.getName();
                        if (namesOfNE.contains(dependencyName)) {
                            //NE we want to add as feature
                            foundNE = true;
                            StringBuilder stringBuilder = new StringBuilder();
                            stringBuilder.append(dependencyName);
                            for (XipNode xipNodeParam : dep.getParameters()) {
                                stringBuilder.append(xipNodeParam.getLemma().trim());
                            }
                            String feat = (stringBuilder.toString()).replaceAll(" ", "").toLowerCase();
                            System.out.println(feat);
                        }
                        if ("SENTIMENT".equals(dependencyName)) {
                            Vector<XipFeature> features = dep.getFeatures();
                            for (XipFeature feature : features) {
                                if ("POSITIVE".equals(feature.getName())) {
                                    System.out.println("opinion_positive");
                                }
                                if ("NEGATIVE".equals(feature.getName())) {
                                    System.out.println("opinion_negative");
                                }
                            }
                        }
                    }
                    if (!foundNE) {
                        processXipNodes(xipNodeDaughter, deep + "   ", xipNodeDaughter.getCategory() + xipNodeDaughter.getIndex());
                    }
                }
            }
        }
    }

    private void processDependencies(XipUnit xipUnit) {
        for (XipDependency dependency : xipUnit.getDependencies()) {
            String dependencyName = dependency.getName();
            //logger.debug("dependencyName: " + dependencyName);
            if (namesOfNE.contains(dependencyName)) {
                Vector<XipNode> params = dependency.getParameters();
                StringBuilder paramStr = new StringBuilder();
                for (XipNode xipNodeParam : params) {
                    paramStr.append(xipNodeParam.getLemma().trim());
                }
                //logger.debug(dependencyName  + paramStr);
                //System.out.println(dependencyName + (paramStr.toString().toLowerCase()));
            }
            if ("SENTIMENT".equals(dependencyName)) {
                Vector<XipFeature> features = dependency.getFeatures();
                for (XipFeature feature : features) {
                    if ("POSITIVE".equals(feature.getName())) {
                        // System.out.println("opinion_positive");
                    }
                    if ("NEGATIVE".equals(feature.getName())) {
                        // System.out.println("opinion_negative");
                    }
                }
            }
        }
    }

    private void parse(String inputFileName) throws Exception {
        System.out.println("--- parsing the file:" + inputFileName);
        cnt = 0;
        File file = new File(inputFileName);
        if (!(file.exists())) {
            System.out.println("file not found: " + file.getAbsolutePath());
            System.exit(-1);
        }
        xipResultManager = new XipResultManager();
        //xipResultManager.xipResultBuilderFromTextFile(gHandler, file.getAbsolutePath(), this, "callBack1");
        xipResultManager.xipResultBuilderFromTextFile(gHandler, file.getAbsolutePath(), this, "callBack2");
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

    private void test0(String args[]) {
        //System.out.println(args[0]);
        xipInit();
        evalParse(args[0]);
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
        } catch (InterruptedException ex) {
            System.out.println("test1: " + ex);
            System.exit(-1);
        }
    }

    public static void main(String args[]) throws Exception {
        Test test = new Test();
        test.memInfo("begin");
        test.test0(args);
        //test.test1();
        test.memInfo("end");
    }
}
