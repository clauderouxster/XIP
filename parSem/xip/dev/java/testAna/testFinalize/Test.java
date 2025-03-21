package testFinalize;

import java.io.*;

/**
 *
 * @author grondeau @created 16 octobre 2006
 * 
 * Permet de verifier qu'il n'y a pas pas de memory leaks
 * 
 */
public class Test {

    private void run() throws Exception {
        for (int i = 0; i < 50; i++) {
            Parser parser = new Parser();
            System.out.println("--------------- S loop: " + i);
            parser.loadAndParse();
            parser.checkFinalizer();       //absolument nécessaire sinon memory leaks
            System.out.println("--------------- E loop:" + i);
        }
    }

    public static void main(String args[]) {
        try {
            Test test = new Test();
            test.run();
        } catch (Exception ex) {
            System.out.println("error: " + ex);
            PrintWriter stringWriter = new PrintWriter(new StringWriter());
            ex.printStackTrace(stringWriter);
            System.out.println(stringWriter);
            System.exit(-1);
        }
    }
}
