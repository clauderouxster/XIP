import com.xerox.jxip.*;
import java.io.*;
import java.util.*;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    16 octobre 2006
 */
public class Test {
	XipResultManager xipResultManager;
	int gHandler;
	JXip jxip;


	/**
	 *Constructor for the Test object
	 *
	 * @param  grmFile  Description of the Parameter
	 */
	public Test(String grmFile) {
		try {
			System.out.println("Test::main() Loading grammar file: " + grmFile);
			jxip = new JXip();
			//load the grammar
			gHandler = jxip.XipGrmFile(grmFile, 0, true);
		} catch (Exception ex) {
			System.out.println(ex);
			ex.printStackTrace();
			System.exit(-1);
		}
	}


	/**
	 *  Description of the Method
	 */
	public void myCB1() {
		try {
			//System.out.println("\n ---------------------------- Test.myCB1 --------------------\n");
			XipResult xipRes = xipResultManager.getXipResult();
			boolean withFeature = true;
			String str = xipRes.toString(withFeature);
			System.out.println("\n ---- the xipResult ----\n" + str);
			String str1 = xipRes.getSentenceString();
			System.out.println("\n ---- the sentence ----\n" + str1);
		} catch (Exception ex) {
			System.out.println(ex);
			ex.printStackTrace();
			System.exit(-1);
		}
	}


	/**
	 *  Description of the Method
	 *
	 * @param  input  Description of the Parameter
	 */
	public void parse(String input) throws Exception {
		xipResultManager = new XipResultManager();
		xipResultManager.xipResultBuilderFromTextFile(gHandler, input, this, "myCB1");
	}


	/**
	 *  Description of the Method
	 *
	 * @param  args  Description of the Parameter
	 */
	public final static void main(String args[]) {
		if (args.length != 2) {
			System.out.println("Two parameters are required: the path of the grm file, and the text files to parse");
			System.exit(-1);
		}
		try {
			Test test = new Test(args[0]);
			test.parse(args[1]);
		} catch (Exception ex) {
			System.out.println(ex);
			ex.printStackTrace();
		}
	}
}

