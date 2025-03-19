import com.xerox.jxip.JXip;
import com.xerox.jxip.Const;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    March 4, 2004
 */
class Test {

	private static JXip jxip;
	private static int gHandler;
	private static int col;
	private static long displayMode;
	private static char xmlFlag;
	private static char applyGrammarFlag;


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
	 * load extension grammar files
	 *
	 * @param  fileName       the grammar file name
	 * @exception  Exception
	 */
	private static void loadExtGram(String fileName) throws Exception {
		//System.out.println("Loading file: " + fileName);
		int res = jxip.XipParameterFile(gHandler, fileName);
		if (res == -1) {
			System.out.println("XipParameterFile call error");
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
			loadExtGram("../../grammar/sample1/features.xip");
			loadExtGram("../../grammar/sample1/lexicon.xip");
			loadExtGram("../../grammar/sample1/functions.xip");
			loadExtGram("../../grammar/sample1/dependencies.xip");
			loadExtGram("../../grammar/sample1/rules.xip");
			String strToParse = "Laurence drove her car. Adam drove his car.";
			System.out.println("Input: \n" + strToParse);

			//displayMode = 0;
			//displayMode = Const.DISPLAY_LEMMA | Const.DISPLAY_TREE;
			displayMode = Const.DISPLAY_TREE;
			//displayMode = Const.DISPLAY_TREE_PHRASE;
			//displayMode = Const.DISPLAY_TREE_COLUMN;
			col = 80;
			jxip.XipSetDisplayMode(gHandler, displayMode, col);

			xmlFlag = Const.XML_NONE;
			//xmlFlag = Const.XML_OUTPUT;
			//xmlFlag = Const.XML_XML_INSERT;
			applyGrammarFlag = (char) 1;
			String res = jxip.parseString(gHandler, strToParse, xmlFlag, applyGrammarFlag);
			System.out.println("\n\nOutput: \n" + res);
		} catch (Exception ex) {
			ex.printStackTrace();
			System.exit(-1);
		}
	}
}


