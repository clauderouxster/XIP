import com.meterware.httpunit.*;
import org.w3c.dom.*;
import com.xerox.jxip.*;
import java.text.*;
import java.io.*;
import org.apache.tools.ant.*;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    11 mai 2006
 */
public class Test {
	//attention doit etre identique que dans ../web/server/web/page1.jsp
	private static String startParsingButtonStr = "Start parsing";
	private static String clearTextButtonStr = "Clear text";
	private static String defaultExampleButtonStr = "Predefined example";
	//
	private static String urlRoot;
	private static String url;
	private static String server;
	private static WebConversation conversation;
	private static WebRequest request;
	private static String webOutput;
	private static String inputUsed;
	private static JXip jxip;
	private static String rootXip;
	private static boolean error;
	private static int gHandlerFrench, gHandlerEnglish, gHandlerSpanish, gHandlerPortuguese, gHandlerItalian, gHandlerGerman;


	/**
	 *  Description of the Method
	 *
	 * @exception  Exception  Description of the Exception
	 */
	public static void init() throws Exception {
		//System.out.println("init()");
		conversation = new WebConversation();
		//to avoid javascript error
		HttpUnitOptions.setExceptionsThrownOnScriptError(false);
		jxip = new JXip();
		loadGram();
	}


	/**
	 *  Description of the Method
	 *
	 * @exception  Exception  Description of the Exception
	 */
	public static void init_dev() throws Exception {
		//System.out.println("test of dev web server");
		urlRoot = "http://dima.grenoble.xrce.xerox.com:8082";
		url = urlRoot + "/xip/page1.jsp";
		init();
	}


	/**
	 *  Description of the Method
	 *
	 * @exception  Exception  Description of the Exception
	 */
	public static void init_intern() throws Exception {
		//System.out.println("test of intern web server");
		urlRoot = "http://dima.grenoble.xrce.xerox.com:8086";
		url = urlRoot + "/xip/page1.jsp";
		init();
	}


	/**
	 *  Description of the Method
	 *
	 * @exception  Exception  Description of the Exception
	 */
	public static void init_public() throws Exception {
		//System.out.println("test of public web server");
		urlRoot = "http://www2.grenoble.xrce.xerox.com:8003";
		url = urlRoot + "/xip/page1.jsp";
		init();
	}


	/**
	 *  Description of the Method
	 *
	 * @exception  Exception  Description of the Exception
	 */
	public static void init_extern() throws Exception {
		//System.out.println("test of extern web server");
		urlRoot = "http://www.xrce.xerox.com";
		url = urlRoot + "/xip/page1.jsp";
		init();
	}


	/**
	 *  Sets the param attribute of the Test object
	 *
	 * @param  input     The new requestParam value
	 * @param  language  The new requestParam value
	 * @param  option    The new requestParam value
	 */
	private static void setRequestParam(String input, String language, String option) {
		//System.out.println("setRequestParam language = " + language + " option = " + option);
		//System.out.println("setRequestParam input = " + input);
		if (input == null) {
			//predefined example
			request.setParameter("defaultExample", defaultExampleButtonStr);
		} else {
			request.setParameter("input", input);
		}
		//language value= english, french, spanish, portuguese, italian, german
		request.setParameter("language", language);
		// option values= option2, option3, option4, option5, option6, option7
		request.setParameter("option", option);
		request.setParameter("submitParam", startParsingButtonStr);
	}


	/**
	 *  Gets the running attribute of the Test class
	 *
	 * @return                The running value
	 * @exception  Exception  Description of the Exception
	 */
	public static boolean isRunning() throws Exception {
		System.out.println("isRunning() url = " + url);
		WebConversation conversation = new WebConversation();
		//set proxy to allow extern web access
		conversation.setProxyServer("cornillon.grenoble.xrce.xerox.com", 8000);
		//to avoid javascript error
		HttpUnitOptions.setExceptionsThrownOnScriptError(false);
		request = new GetMethodWebRequest(url);
		try {
			WebResponse response = conversation.getResponse(request);
		} catch (com.meterware.httpunit.HttpException ex1) {
			System.out.println("com.meterware.httpunit.HttpException: " + ex1);
			return false;
		}
		catch (Exception ex) {
			System.out.println("Exception: " + ex);
			return false;
		}
		return true;
	}


	/**
	 * @param  input          Description of the Parameter
	 * @param  language       Description of the Parameter
	 * @param  option         Description of the Parameter
	 * @return                Description of the Return Value
	 * @exception  Exception  Description of the Exception
	 */
	public static String sendWebRequest(String input, String language, String option) throws Exception {
		//System.out.println("sendWebRequest() url = " + url);
		try {
			WebConversation conversation = new WebConversation();
			//set proxy to allow extern web access
			conversation.setProxyServer("cornillon.grenoble.xrce.xerox.com", 8000);
			//to avoid javascript error
			HttpUnitOptions.setExceptionsThrownOnScriptError(false);
			request = new GetMethodWebRequest(url);
			setRequestParam(input, language, option);
			//System.out.println("request " + request);
			WebResponse response = conversation.getResponse(request);
			//System.out.println(response.getText());
			HTMLElement htmlElement;
			htmlElement = response.getElementWithID("xipInput");
			if (htmlElement != null) {
				inputUsed = htmlElement.getText();
			} else {
				inputUsed = "";
			}
			htmlElement = response.getElementWithID("xipOutput");
			if (htmlElement != null) {
				//System.out.println(htmlElement.getText());
				return htmlElement.getText();
			} else {
				System.out.println("htmlElement.getText(): null");
				return null;
			}
		} catch (com.meterware.httpunit.HttpException ex1) {
			System.out.println("com.meterware.httpunit.HttpException: " + ex1);
			return null;
		}
		catch (Exception ex) {
			System.out.println("Exception: " + ex);
			return null;
		}
	}


	/**
	 *  Description of the Method
	 */
	private static void loadGram() {
		//System.out.println("loadGram()");
		String grmFile = "";
		try {
			//----
			grmFile = rootXip + "/french/entity/french_entity.grm";
			System.out.println("Loading grm file: " + grmFile);
			gHandlerFrench = jxip.XipGrmFile(grmFile, 0, true);
			//gHandlerFrench = 32;
			if (gHandlerFrench == -1) {
				System.out.println("loading grammar error: " + grmFile);
				System.exit(-1);
			}
			//----
			grmFile = rootXip + "/english/entity/gram_gen_entit.grm";
			System.out.println("Loading grm file: " + grmFile);
			gHandlerEnglish = jxip.XipGrmFile(grmFile, 0, true);
			//gHandlerEnglish = 32;
			if (gHandlerEnglish == -1) {
				System.out.println("loading grammar error: " + grmFile);
				System.exit(-1);
			}
			//----
			grmFile = rootXip + "/spanish/basic/spanish.grm";
			System.out.println("Loading grm file: " + grmFile);
			gHandlerSpanish = jxip.XipGrmFile(grmFile, 0, true);
			//gHandlerSpanish = 32;
			if (gHandlerSpanish == -1) {
				System.out.println("loading grammar error: " + grmFile);
				System.exit(-1);
			}
			//----
			grmFile = rootXip + "/italian/basic/italian.grm";
			System.out.println("Loading grm file: " + grmFile);
			gHandlerItalian = jxip.XipGrmFile(grmFile, 0, true);
			//gHandlerItalian = 32;
			if (gHandlerItalian == -1) {
				System.out.println("loading grammar error: " + grmFile);
				System.exit(-1);
			}
			//----
			grmFile = rootXip + "/portuguese/basic/portuguese.grm";
			System.out.println("Loading grm file: " + grmFile);
			gHandlerPortuguese = jxip.XipGrmFile(grmFile, 0, true);
			//gHandlerPortuguese = 32;
			if (gHandlerPortuguese == -1) {
				System.out.println("loading grammar error: " + grmFile);
				System.exit(-1);
			}
			//----
			grmFile = rootXip + "/german/basic/german.grm";
			//System.out.println("Loading grm file: " + grmFile);
			//gHandlerGerman = jxip.XipGrmFile(grmFile, 0, true);
			gHandlerGerman = 32;
			if (gHandlerGerman == -1) {
				System.out.println("loading grammar error: " + grmFile);
				System.exit(-1);
			}
		} catch (Exception ex) {
			System.out.println("loadGram ERROR: " + ex.toString());
			ex.printStackTrace();
			System.exit(-1);
		}
	}


	/**
	 *  A unit test for JUnit
	 *
	 * @param  input          Description of the Parameter
	 * @param  language       Description of the Parameter
	 * @param  option         Description of the Parameter
	 * @return                Description of the Return Value
	 * @exception  Exception  Description of the Exception
	 */
	public static boolean test(String input, String language, String option) throws Exception {
		System.out.println("=============================  test: " + language + " " + option + "==============");
		int retry = 1;
		while ((webOutput = sendWebRequest(input, language, option)) == null) {
			retry++;
			if (retry == 5) {
				error = true;
				return false;
			}
			Thread.sleep(2);
		}
		//System.out.println("-----------  input ------------------");
		//System.out.println(inputUsed);
		//System.out.println("-----------  webOutput ------------------");
		//System.out.println(webOutput);
		// now parse directly with xip (without the webUI, with the same jar and grammar)
		int gHandler = -1;
		if (language.compareTo("french") == 0) {
			gHandler = gHandlerFrench;
		} else if (language.compareTo("english") == 0) {
			gHandler = gHandlerEnglish;
		} else if (language.compareTo("spanish") == 0) {
			gHandler = gHandlerSpanish;
		} else if (language.compareTo("portuguese") == 0) {
			gHandler = gHandlerPortuguese;
		} else if (language.compareTo("italian") == 0) {
			gHandler = gHandlerItalian;
		} else if (language.compareTo("german") == 0) {
			gHandler = gHandlerGerman;
		} else {
			System.out.println("language unknow: " + language);
			return false;
		}
		//System.out.println("gHandler = " + gHandler);
		int displayMode = 0;
		char xmlFlag = 0;
		if (option.compareTo("option2") == 0) {
			displayMode = 32;
			//DISPLAY_reduce
		} else if (option.compareTo("option3") == 0) {
			displayMode = 64;
			//DISPLAY_FULL
		} else if (option.compareTo("option4") == 0) {
			//tree
			displayMode = 262144;
		} else if (option.compareTo("option5") == 0) {
			//large tree
			displayMode = 1048576;
		} else if (option.compareTo("option6") == 0) {
			//entity extraction
			displayMode = 0;
		} else if (option.compareTo("option7") == 0) {
			displayMode = 64;
			//DISPLAY_FULL 64
			xmlFlag = 1;
		} else {
			System.out.println("option unknow");
			return false;
		}
		int colSize = 70;
		jxip.XipSetDisplayMode(gHandler, displayMode, colSize);
		// set Const.DISPLAY_CHUNK_TREE
		jxip.XipAddFlagDisplay(gHandler, 2147483648L, colSize);
		String xipOutput = jxip.parseStringCharSet(gHandler, inputUsed, "iso-8859-1", Const.XML_NONE, (char) 1);
		//System.out.println("-----------  xipOutput ------------------\n" + xipOutput);
		// now suppress in xipOutput and webOutput strings, the following pattern
		String pattern = "[0-9]+>";
		String webOutputBis = webOutput.replaceAll(pattern, "").trim();
		String xipOutputBis = xipOutput.replaceAll(pattern, "").trim();
		//compare xipOutput and webOutput
		if (xipOutputBis.compareTo(webOutputBis) != 0) {
			//System.out.println("--------------- webOutputBis --------\n" + webOutputBis);
			//System.out.println("--------------- xipOutputBis --------\n" + xipOutputBis);
			StringCharacterIterator iterWeb = new StringCharacterIterator(webOutputBis);
			StringCharacterIterator iterXip = new StringCharacterIterator(xipOutputBis);
			char cWeb = iterWeb.first();
			char cXip = iterXip.first();
			int count = 0;
			StringBuffer strWeb = new StringBuffer("");
			StringBuffer strXip = new StringBuffer("");
			while (cWeb != CharacterIterator.DONE && cXip != CharacterIterator.DONE) {
				cWeb = iterWeb.next();
				cXip = iterXip.next();
				if (cWeb != cXip) {
					strWeb.append(cWeb);
					strXip.append(cXip);
					count++;
				}
				if (count == 300) {
					break;
				}
			}
			System.out.println("------web-----\n" + strWeb);
			System.out.println("------xip-----\n" + strXip);
			FileWriter fxip = new FileWriter("xipOutput");
			fxip.write(xipOutputBis);
			fxip.close();
			FileWriter fweb = new FileWriter("webOutput");
			fweb.write(webOutputBis);
			fweb.close();
			System.out.println("ERROR !!!!!!!!!!!!!!!!!!!!");
			error = true;
			return false;
		}
		return true;
	}


	/**
	 *  Description of the Method
	 *
	 * @exception  Exception  Description of the Exception
	 */
	private static void stopStartServerWeb() throws Exception {
		System.out.println("stop and start web server");
		Runtime runtime = Runtime.getRuntime();
		runtime.exec("ant -f ../../build.xml stop" + server);
		Thread.sleep(6000);
		runtime.exec("ant -f ../../build.xml start" + server);
		/*
		 *  Project ant = new Project();
		 *  ant.init();
		 *  /ProjectHelper helper = new ProjectHelperImpl();
		 *  /helper.parse(ant, new File("../../build.xml"));
		 *  ProjectHelper.configureProject(ant, new File("../../build.xml"));
		 *  System.out.println("stop begin");
		 *  try {
		 *  ant.executeTarget("stop" + server);
		 *  } catch (com.meterware.httpunit.HttpException ex) {
		 *  System.out.println("Exception");
		 *  }
		 *  /Thread.sleep(6000);
		 *  System.out.println("stop end");
		 *  System.out.println("start begin");
		 *  ant.executeTarget("start" + server);
		 *  System.out.println("start end");
		 */
		Thread.sleep(5000);
		while (!isRunning()) {
			System.out.println("waiting");
			//wait 2s
			Thread.sleep(3000);
		}
	}


	/**
	 *  The main program for the Test class
	 *
	 * @param  args           The command line arguments
	 * @exception  Exception  Description of the Exception
	 */
	public static void main(String[] args) throws Exception {
		server = args[0];
		rootXip = args[1];
		int loopCnt1 = 1;
		int loopCnt2 = 1;
		boolean startStop = false;
		//System.out.println(args.length);
		if (args.length == 3) {
			startStop = false;
			loopCnt2 = 1;
			loopCnt2 = new Integer(args[2]).intValue();
		} else if (args.length == 4) {
			startStop = true;
			loopCnt1 = new Integer(args[2]).intValue();
			loopCnt2 = 3;
		} else {
			System.out.println("java -cp ... Test serverName rootXip loopCnt [any]");
			System.exit(-1);
		}
		if (server.compareTo("ext") == 0) {
			init_extern();
		} else if (server.compareTo("int") == 0) {
			init_intern();
		} else if (server.compareTo("pub") == 0) {
			init_public();
		} else if (server.compareTo("dev") == 0) {
			init_dev();
		} else {
			System.out.println("server name unknow");
			System.exit(-1);
		}
		error = false;
		System.out.println("loopCnt1 = " + loopCnt1);
		System.out.println("loopCnt2 = " + loopCnt2);
		System.out.println("server = " + server);
		System.out.println("rootXip = " + rootXip);
		System.out.println("url = " + url);
		long startTime = System.currentTimeMillis();
		for (int ind1 = 1; ind1 <= loopCnt1; ind1++) {
			System.out.println("\n*********************** " + ind1 + " **************************");
			if (startStop) {
				stopStartServerWeb();
			}
			for (int ind2 = 1; ind2 <= loopCnt2; ind2++) {
				System.out.println("\n------ " + ind2 + " --------");
				test(null, "english", "option2");
				test(null, "french", "option2");
				test(null, "spanish", "option2");
				test(null, "italian", "option2");
				test(null, "portuguese", "option2");
				//test(null, "german", "option2");
			}
		}
		long endTime = System.currentTimeMillis();
		System.out.println("\n\nExecution time : " + (endTime - startTime) / 1000 + " seconds");
		if (error) {
			System.out.println("\nERROR in some test\n");
			System.exit(-1);
		} else {
			System.out.println("\n\nTEST OK\n");
		}
	}
}

