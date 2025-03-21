import com.xerox.jxip.*;

import java.io.*;
import java.util.*;

/**
 *  Description of the Class
 *
 * @author    grondeau
 */
public class Test {

	/**
	 *  Description of the Method
	 *
	 * @param  jxip               Description of the Parameter
	 * @param  gHandler           Description of the Parameter
	 * @param  parameterFileName  Description of the Parameter
	 */
	static void loadPar(JXip jxip, int gHandler, String parameterFileName) {
		int result = jxip.XipParameterFile(gHandler, parameterFileName);
		if (result != 0) {
			System.out.println("Test::main() XipParameterFile Error ");
			System.exit(-1);
		}
	}


	/**
	 *  Gets the inputFiles attribute of the Test class
	 *
	 * @param  inputPath  Description of the Parameter
	 * @return            The inputFiles value
	 */
	static Vector getInputFiles(String inputPath) {
		Vector files = new Vector();
		String fileName;
		fileName = inputPath + "/jaguar/ch2.rtf.ttx";
		files.add(fileName);
		fileName = inputPath + "/landrover/ch5.rtf.ttx";
		files.add(fileName);
		fileName = inputPath + "/mazda/04.rtf.ttx";
		files.add(fileName);
		fileName = inputPath + "/volvo/data.rtf.ttx";
		files.add(fileName);
		fileName = inputPath + "/volvo/larm.rtf.ttx";
		files.add(fileName);
		return files;
	}


	/**
	 *  Description of the Method
	 *
	 * @param  args  Description of the Parameter
	 */
	public final static void main(String args[]) {
		int gHandler;
		JXip jxip;
		try {
			if (args.length != 5) {
				System.out.println("5 parameters are required: " + args.length);
				System.exit(-1);
			}
			jxip = new JXip();
			String grmFileName = args[0];
			gHandler = jxip.XipGrmFile(grmFileName, 0);
			if (gHandler == -1) {
				System.out.println("Test::main() XipGrmFile call error");
				System.exit(-1);
			}

			String scriptPath = args[1];
			loadPar(jxip, gHandler, scriptPath + "/Variables" + ".xip");
			loadPar(jxip, gHandler, scriptPath + "/xml_description" + ".xip");
			loadPar(jxip, gHandler, scriptPath + "/init" + ".xip");
			loadPar(jxip, gHandler, scriptPath + "/clean_np" + ".xip");
			loadPar(jxip, gHandler, scriptPath + "/encoded" + ".xip");
			loadPar(jxip, gHandler, scriptPath + "/dep_gkls" + ".xip");
			loadPar(jxip, gHandler, scriptPath + "/termes" + ".xip");

			String variableName = "filename";
			String variableValue = "output.xml";
			int result = 0;
			result = jxip.XipSetStringVariable(gHandler, variableName, variableValue);
			if (result != 0) {
				System.out.println("Test::main() XipSetStringVariable Error: " + variableName);
				System.exit(-1);
			}

			String dataBaseFileName = args[2];
			String alias = "reference";
			result = jxip.XipLoadXMLDataBase(gHandler, dataBaseFileName, alias);
			if (result != 0) {
				System.out.println("Test::main() XipLoadXMLDataBase Error");
				System.exit(-1);
			}
			System.out.println("XipLoadXMLDataBase DONE dataBaseFileName = " + dataBaseFileName + " alias = " + alias);

			//String strToParse = args[4];
			//System.out.println("Test::main() parsing the string = " + strToParse);

			//int displayMode = Const.DISPLAY_TREE;
			//int col = 80;
			//jxip.XipSetDisplayMode(gHandler, displayMode, col);

			//String res = "";
			//res = jxip.parseUtf8String(gHandler, strToParse, Const.XML_NONE, (char) 1);
			//System.out.println("\nTest::main() resultat after parsing = " + res);

			jxip.XipSetDependencyExtraction(gHandler, true);

			Vector files = getInputFiles(args[3]);

			InputFile inputFile = new InputFile(jxip, gHandler);
			for (Enumeration enumeration = files.elements(); enumeration.hasMoreElements(); ) {
				String fileName = (String) enumeration.nextElement();
				inputFile.addFile(fileName);
			}
			// pour que l'analyse soit partielle
			variableName = "_initialization";
			result = jxip.XipSetIntVariable(gHandler, variableName, 1);
			if (result != 0) {
				System.out.println("Test::main() XipSetIntVariable Error: " + variableName);
				System.exit(-1);
			}
			String input = "";
			input = inputFile.getAllInput();
			if (input == null) {
				System.out.println("InputFile ERROR");
				System.exit(-1);
			} else {
				System.out.println("=================== input =============================\n");
				System.out.println(input);
				System.out.println("=================== End of input ======================\n");
			}
			// pour que l'analyse soit complete
			variableName = "_initialization";
			result = jxip.XipSetIntVariable(gHandler, variableName, 0);
			if (result != 0) {
				System.out.println("Test::main() XipSetIntVariable Error: " + variableName);
				System.exit(-1);
			}

			// to init. the client tag in the xml output
			variableName = "clientname";
			variableValue = "volvo";
			result = jxip.XipSetStringVariable(gHandler, variableName, variableValue);
			if (result != 0) {
				System.out.println("Test::main() XipSetStringVariable Error: " + variableName);
				System.exit(-1);
			}
			// to reset all variables in the scripts
			variableName = "_loading";
			result = jxip.XipSetIntVariable(gHandler, variableName, 1);
			if (result != 0) {
				System.out.println("Test::main() XipSetIntVariable Error: " + variableName);
				System.exit(-1);
			}
			int depth = 0;
			char xml = Const.XML_NONE;
			char apply = 1;
			for (Enumeration enumeration = files.elements(); enumeration.hasMoreElements(); ) {
				String fileName = (String) enumeration.nextElement();
				//System.out.println("\t fileName = " + fileName);
				int displayMode = Const.DISPLAY_NONE;
				int col = 80;
				jxip.XipSetDisplayMode(gHandler, displayMode, col);
				String xmlOut = jxip.XipParseFileXmlOS(gHandler, fileName, depth, xml, apply);
				if (xmlOut == null) {
					System.out.println("XipParseFileXmlOS ERROR");
					System.exit(-1);
				}
				System.out.println(xmlOut);
			}

			variableName = "filereport";
			variableValue = jxip.XipGetStringVariable(gHandler, variableName);
			System.out.println("\n" + variableName + " = \n" + variableValue);

			variableName = "globalreport";
			variableValue = jxip.XipGetStringVariable(gHandler, variableName);
			System.out.println("\n" + variableName + " = " + variableValue);

			Vector aliases = jxip.XipAliasList(gHandler);
			for (Enumeration enumeration = aliases.elements(); enumeration.hasMoreElements(); ) {
				String alias1 = (String) enumeration.nextElement();
				System.out.println("alias = " + alias1);
			}

			String alias1 = "TERMS";
			result = jxip.XipCleanXMLAlias(gHandler, alias1);
			if (result == 0) {
				System.out.println("Test:main() XipCleanXMLAlias OK");
			} else if (result == 1) {
				System.out.println("Test::main() XipCleanXMLAlias Error alias unknown = " + alias1);
				System.exit(-1);
			} else {
				System.out.println("Test::main() XipCleanXMLAlias Error");
				System.exit(-1);
			}

			result = jxip.XipReloadGrammar(gHandler, true, grmFileName);
			if (result == -1) {
				System.out.println("Test::main() XipReloadGrammar Error");
				System.exit(-1);
			}
		} catch (Exception ex) {
			ex.printStackTrace();
			System.exit(-1);
		}
	}
}


