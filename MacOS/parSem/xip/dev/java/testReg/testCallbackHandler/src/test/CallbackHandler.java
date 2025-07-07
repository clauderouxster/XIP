package testCallbackHandler.src.test;

import com.xerox.jxip.JXip;
import com.xerox.jxip.XipResultManager;

/**
 * Description of the Class
 *
 * @author grondeau @created 26 juin 2006
 */
public class CallbackHandler {

    XipResultManager xipResultManager;
    int gHandler;
    JXip jxip;
    C1 c1;
    C2 c2;

    /**
     * Description of the Method
     *
     * @param grmFile Description of the Parameter
     */
    public CallbackHandler(String grmFile) {
        try {
            System.out.println("CallbackHandler::main() Loading grammar file: " + grmFile);
            jxip = new JXip();
            gHandler = jxip.XipGrmFile(grmFile, 0, true);
        } catch (Exception ex) {
            System.out.println(ex);
            //ex.printStackTrace();
            System.exit(-1);
        }
    }

    void check(String str1, String str2, String testName) {
        if (!str1.equals(str2)) {
            System.out.println("--------" + testName + ": ERROR  --------");
            System.out.println("res: " + str1 + "\n---\nref: " + str2);
            System.exit(-1);
        } else {
            System.out.println("--------" + testName + ": OK  --------");
        }
    }
    static String ref1 = "C1.myCB1\n Peter is happy .\nC1.myCB1\n The cat is black .\n";
    static String ref2 = "C1.myCB1\n This is a test .\nC1.myCB1\n The cat is black and the boat is blue .\n";
    static String ref3 = "C0.myCBXml\nC1.myCB1\n Peter is happy .\nC0.myCBXml\nC0.myCBXml\nC1.myCB1\n"
            + " The boat is blue .\nC1.myCB1\n The cat is white .\nC0.myCBXml\n";
    static String ref4 = "C2.myCB2\n Peter is happy .\nC2.myCB2\n The cat is black .\n";
    static String ref5 = "C2.myCB2\n This is a test .\nC2.myCB2\n The cat is black and the boat is blue .\n";
    static String ref6 = "C2.myCBXml\nC1.myCB1\n Peter is happy .\nC2.myCBXml\nC2.myCBXml\nC1.myCB1\n"
            + " The boat is blue .\nC1.myCB1\n The cat is white .\nC2.myCBXml\n";
    static String ref7 = "C2.myCBXml\nC2.myCB2\n Peter is happy .\nC2.myCBXml\nC2.myCBXml\nC2.myCB2\n"
            + " The boat is blue .\nC2.myCB2\n The cat is white .\nC2.myCBXml\n";
    static String inputStr = "Peter is happy. The cat is black.";

    void test1() throws Exception {
        c1.clrBuf();
        xipResultManager.xipResultBuilderFromString(gHandler, inputStr, c1, "myCB1");
        check(c1.getRes(), ref1, "test1");
    }

    void test2() throws Exception {
        c1.clrBuf();
        xipResultManager.xipResultBuilderFromTextFile(gHandler, "input", c1, "myCB1");
        check(c1.getRes(), ref2, "test2");
    }

    void test3() throws Exception {
        c1.clrBuf();
        xipResultManager.xipResultBuilderFromXmlFile(gHandler, "input.xml", c1, "myCB1", "myCBXml");
        check(c1.getRes(), ref3, "test3");
    }

    void test4() throws Exception {
        c2.clrBuf();
        xipResultManager.xipResultBuilderFromString(gHandler, inputStr, c2, "myCB1");
        check(c2.getRes(), ref1, "test4");
    }

    void test5() throws Exception {
        c2.clrBuf();
        xipResultManager.xipResultBuilderFromString(gHandler, inputStr, c2, "myCB2");
        check(c2.getRes(), ref4, "test5");
    }

    void test6() throws Exception {
        c2.clrBuf();
        xipResultManager.xipResultBuilderFromTextFile(gHandler, "input", c2, "myCB1");
        check(c2.getRes(), ref2, "test6");
    }

    void test7() throws Exception {
        c2.clrBuf();
        xipResultManager.xipResultBuilderFromTextFile(gHandler, "input", c2, "myCB2");
        check(c2.getRes(), ref5, "test7");
    }

    void test8() throws Exception {
        c2.clrBuf();
        xipResultManager.xipResultBuilderFromXmlFile(gHandler, "input.xml", c2, "myCB1", "myCBXml");
        check(c2.getRes(), ref6, "test8");
    }

    void test9() throws Exception {
        c2.clrBuf();
        xipResultManager.xipResultBuilderFromXmlFile(gHandler, "input.xml", c2, "myCB2", "myCBXml");
        check(c2.getRes(), ref7, "test9");
    }

    public void parse() throws Exception {
        xipResultManager = new XipResultManager();
        c1 = new C1(xipResultManager);
        test1();
        test2();
        test3();
        c2 = new C2(xipResultManager);
        test4();
        test5();
        test6();
        test7();
        test8();
        test9();
    }

    /**
     * Description of the Method
     *
     * @param args Description of the Parameter
     */
    public static void main(String args[]) {
        if (args.length != 1) {
            System.out.println("one parameter is required: the fullpath of the grm file");
            System.exit(-1);
        }
        try {
            CallbackHandler test = new CallbackHandler(args[0]);
            test.parse();
        } catch (Exception ex) {
            System.out.println(ex);
            //ex.printStackTrace();
        }
    }
}
