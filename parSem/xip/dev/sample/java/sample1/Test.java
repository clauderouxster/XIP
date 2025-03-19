import com.xerox.jxip.JXip;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    March 4, 2004
 */
class Test {

	private static JXip jxip;
	private static int gHandler;


	/**
	 *  load the grammar files using a grm file
	 *
	 * @param  grmFile        the grm file name
	 * @exception  Exception
	 */
	private static void loadBaseGram(String grmFile) throws Exception {
		//System.out.println("Loading grm file: " + grmFile);
		gHandler = jxip.XipGrmFile(grmFile, 0, true);
		if (gHandler == -1) {
			System.out.println("XipGrmFile call error");
			System.exit(-1);
		}
	}


	/**
	 * @param  args
	 */
	public static void main(String args[]) {
		try {
			jxip = new JXip();
			System.out.println(jxip.GetVersion());
			loadBaseGram(args[0]);
			String strToParse = "Laurence drove her car. Adam drove his car.";
			System.out.println("Input: \n" + strToParse);
			int col = 80;
			int displayMode = 0;
			jxip.XipSetDisplayMode(gHandler, displayMode, col);
			String res = jxip.parseString(gHandler, strToParse, (char) 0, (char) 1);
			System.out.println("\n\nOutput: \n" + res);
		} catch (Exception ex) {
			ex.printStackTrace();
			System.exit(-1);
		}
	}
}

