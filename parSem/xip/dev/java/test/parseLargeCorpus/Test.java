import com.xerox.jxip.*;
import java.io.*;
import java.util.*;
import java.text.*;

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
	long count;

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
			System.out.println(count++);
			XipResult xipRes = xipResultManager.getXipResult();
			boolean withFeature = true;
			String str = xipRes.toString(withFeature);
			//System.out.println("\n ---- the xipResult ----\n" + str);
			String str1 = xipRes.getSentenceString();
			//System.out.println("\n ---- the sentence ----\n" + str1);
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
	public final static void main(String args[]) throws Exception {
		if (args.length != 2) {
			System.out.println("Two parameters are required: the path of the grm file, and the directory of the files to parse");
			System.exit(-1);
		}
		try {
			final String suffix = ".txt";
			Test test = new Test(args[0]);
			String dirName = args[1];
			File dir = new File(dirName);
			FilenameFilter filter = new FilenameFilter() {
        			public boolean accept(File dir, String name) {
            				return name.endsWith(suffix);
        			}
			};
			String[] children = dir.list(filter);
			if (children == null) {
				System.out.println("diretory doesn't exist!:" + dirName);
				System.exit(-1);
			}
			if (children.length == 0) {
				System.out.println("diretory empty: " + dirName  + " or no file with the suffix: " + suffix);
				System.exit(-1);
			}
			long startTime = System.currentTimeMillis();
			for (int i=0; i<children.length; i++) {
				// Get filename of file or directory
				String fileName = args[1] + "/" + children[i];
				System.out.println("------ processing file: " +i +"   " + fileName);
				test.parse(fileName);
				long endTime = System.currentTimeMillis();
				long elapsed = endTime - startTime;
				SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm:ss");
				dateFormat.setTimeZone(TimeZone.getTimeZone("GMT"));
				System.out.println("elapsed time:" + dateFormat.format(new Date(elapsed)));
			}
		} catch (Exception ex) {
			System.out.println(ex);
			ex.printStackTrace();
			System.exit(-1);
		}
	}
}
 
