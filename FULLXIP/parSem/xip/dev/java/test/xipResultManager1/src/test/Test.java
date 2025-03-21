package test;

import com.xerox.jxip.*;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    26 juin 2006
 */
public class Test {

    XipResultManager xipResultManager;
    int gHandler;
    JXip jxip;

    /**
     *  Description of the Method
     *
     * @param  grmFile  Description of the Parameter
     */
    public Test(String grmFile) {
        try {
            System.out.println("Test::main() Loading grammar file: " + grmFile);
            jxip = new JXip();
            gHandler = jxip.XipGrmFile(grmFile, 0, true);
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
            System.exit(-1);
        }
    }

    /**
     *  Description of the Method
     *
     * @throws Exception 
     */
    public void parse() throws Exception {
        xipResultManager = new XipResultManager();
        C2 c2 = new C2(xipResultManager);
        String inputStr;
        inputStr = "Chancellor Gordon Brown has denied that extra money in the Budget for pensioners is a pre-election bribe.";
        //inputStr = "Peter is happy. The cat is black.";
        System.out.println("inputStr = " + inputStr);
        xipResultManager.xipResultBuilderFromString(gHandler, inputStr, c2, "myCB1");
        System.out.println("=====================================");
        xipResultManager.xipResultBuilderFromString(gHandler, inputStr, c2, "myCB2");
        System.out.println("=====================================");
        xipResultManager.xipResultBuilderFromTextFile(gHandler, "input", c2, "myCB1");
        System.out.println("=====================================");
        xipResultManager.xipResultBuilderFromXmlFile(gHandler, "input.xml", c2, "myCB1", "myCBXml");
    }

    /**
     *  Description of the Method
     *
     * @param  args  Description of the Parameter
     */
    public final static void main(String args[]) {
        if (args.length != 1) {
            System.out.println("one parameter is required: the fullpath of the grm file");
            System.exit(-1);
        }
        try {
            Test test = new Test(args[0]);
            test.parse();
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }
}

