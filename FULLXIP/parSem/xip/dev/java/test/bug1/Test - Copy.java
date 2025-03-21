import com.xerox.jxip.*;
import java.io.*;
import java.util.*;

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
			// create a JXIP object to load the dynamic library in the VM
			jxip = new JXip();
			//load the grammar
			gHandler = jxip.XipGrmFile(grmFile, 0, true);
		} catch (Exception ex) {
			System.out.println(ex);
			ex.printStackTrace();
		}
	}


	/**
	 *  Description of the Method
	 */
	public void myCB1() {
		try {
			System.out.println("\n ---------------------------- Test.myCB1 --------------------\n");
/*
			XipResult xipRes = xipResultManager.getXipResult();
			boolean withFeature = true;
			String str = xipRes.toString(withFeature);
			System.out.println("\n ---- the xipResult ----\n" + str);
			String str1 = xipRes.getSentenceString();
			System.out.println("\n ---- the sentence ----\n" + str1);
*/
		} catch (Exception ex) {
			System.out.println(ex);
			ex.printStackTrace();
		}
	}


	/**
	 *  Description of the Method
	 */
	public void myCB2() {
		try {
			System.out.println("\n ---------------------------- Test.myCB2 --------------------\n");
/*
			String str = "";
			XipResult xipRes = xipResultManager.getXipResult();
			boolean withFeature = true;
			str = xipRes.toString(withFeature);
			//System.out.println("\n ---- the xipResult ----\n" + str + "\n\n");
			for (Enumeration enumeration = xipRes.getSentences().elements(); enumeration.hasMoreElements(); ) {
				XipUnit xipUnit = (XipUnit) enumeration.nextElement();
				System.out.println("\nxipUnit.getSentenceString() = " + xipUnit.getSentenceString());
				//str += xipUnit.toString(withFeature);
				XipNode xipNode = xipUnit.getRoot();
				System.out.println("\nxipNode.getCategory() = " + xipNode.getCategory());
				System.out.println("\nxipNode.getSurface() = " + xipNode.getSurface());
			}
*/
		} catch (Exception ex) {
			System.out.println(ex);
			ex.printStackTrace();
		}
	}


	/**
	 *  Description of the Method
	 */
	public void myCBXml() {
		try {
			System.out.println("\n ---------------------------- Test.myCBXml ----------------------\n");
		} catch (Exception ex) {
			System.out.println(ex);
			ex.printStackTrace();
		}
	}


	/**
	 *  Description of the Method
	 *
	 * @param  input  Description of the Parameter
	 */
	public void parse(String input) throws Exception {
		//parse and build xipResult
		xipResultManager = new XipResultManager();
		//input with fictif node
		String inputStr = "Chancellor Gordon Brown has denied that extra money in the Budget for pensioners is a pre-election bribe.";
		//System.out.println("inputStr = " + inputStr);
		//xipResultManager.xipResultBuilderFromString(gHandler, inputStr, this, "myCB2");
		//xipResultManager.xipResultBuilderFromString(gHandler, "Peter is happy. The cat is black.", this, "myCB1");
		xipResultManager.xipResultBuilderFromTextFile(gHandler, input, this, "myCB1");
		//xipResultManager.xipResultBuilderFromXmlFile(gHandler, "input.xml", this, "myCB1", "myCBXml");
	}


	/**
	 *  Description of the Method
	 *
	 * @param  args  Description of the Parameter
	 */
	public final static void main(String args[]) {
		if (args.length != 2) {
			System.out.println("Two parameters are required: the fullpath of the grm file, and the input to parse");
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

