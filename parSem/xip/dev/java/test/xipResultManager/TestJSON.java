import com.xerox.jxip.*;
import java.io.*;
import java.util.*;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    26 juin 2006
 */
public class TestJSON {
	XipResultManager xipResultManager;
	int gHandler;
	JXip jxip;


	/**
	 *  Description of the Method
	 *
	 * @param  grmFile  Description of the Parameter
	 */
	public TestJSON(String grmFile) {
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
	public void myCB() {
		try {
			System.out.println("\n ---------------------------- Test.myCB --------------------\n");
			String str = xipResultManager.getJSonStringCB();
			System.out.println(str);
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
		String inputStr = "Chancellor Gordon Brown has denied that extra money in the Budget for pensioners is a pre-election bribe.";
		jxip.XipAddFlagDisplay(gHandler,Const.DISPLAY_JSON_API,80);
		xipResultManager.xipResultBuilderFromString(gHandler, inputStr, this, "myCB");

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
			TestJSON test = new TestJSON(args[0]);
			test.parse(args[1]);
		} catch (Exception ex) {
			System.out.println(ex);
			ex.printStackTrace();
		}
	}
}

