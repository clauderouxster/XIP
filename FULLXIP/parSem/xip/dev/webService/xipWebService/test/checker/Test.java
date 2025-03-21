import com.meterware.httpunit.*;
import org.w3c.dom.*;
import com.xerox.jxip.*;
import java.text.*;
import java.io.*;
import org.apache.tools.ant.*;

import java.util.Timer;
import java.util.TimerTask;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.Date;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    11 mai 2006
 */
public class Test extends TimerTask {
	//attention doit etre identique que dans ../web/server/web/page1.jsp
	private static String startParsingButtonStr = "Start parsing";
	private static String clearTextButtonStr = "Clear text";
	private static String defaultExampleButtonStr = "Predefined example";
	//
	private String urlRoot;
	private String url;
	private String server;
	private WebConversation conversation;
	private WebRequest request;
	private String webOutput;
	private String inputUsed;
	private JXip jxip;
	private String rootXip;
	private int gHandlerFrench, gHandlerEnglish, gHandlerSpanish, gHandlerPortuguese, gHandlerItalian, gHandlerGerman;
	private int hStop, mStop;


	/**
	 *  Description of the Method
	 *
	 * @exception  Exception  Description of the Exception
	 */
	public void init() throws Exception {
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
	public void init_dev() throws Exception {
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
	public void init_intern() throws Exception {
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
	public void init_public() throws Exception {
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
	public void init_extern() throws Exception {
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
	private void setRequestParam(String input, String language, String option) {
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
		
	public void log(String mes) {
		try {
			FileWriter fstream = new FileWriter("log", true);
			BufferedWriter out = new BufferedWriter(fstream);
			out.write( new Date() + " :  " + mes + "\n");
			out.close();
		} catch (Exception e){
			System.err.println("log(): " + e.getMessage());
		}
	}
	
	 /**
         *  
         *
         * @return                The running value
         * @exception  Exception  Description of the Exception
         */
        public boolean isRunning()  {
                //System.out.println("isRunning() url = " + url);
                WebConversation conversation = new WebConversation();
                //set proxy to allow extern web access
                conversation.setProxyServer("cornillon.grenoble.xrce.xerox.com", 8000);
                //to avoid javascript error
                HttpUnitOptions.setExceptionsThrownOnScriptError(false);
                request = new GetMethodWebRequest(url);
                try {
                        WebResponse response = conversation.getResponse(request);
                } catch (com.meterware.httpunit.HttpException ex1) {
                        //System.out.println("com.meterware.httpunit.HttpException: " + ex1);
                        return false;
                }
                catch (Exception ex) {
                        log("isRunning(): " + ex);
                        return false;
                }
                return true;
        }

	/**
	 *  Description of the Method
	 *
	 * @exception  Exception  Description of the Exception
	 */
	private void stopStartServerWeb() throws Exception {
		//System.out.println(" ++++++++++++++  stopStartServerWeb()      " + url);
		log(" ++++++++++++++  stopStartServerWeb()        " + url);
		Runtime runtime = Runtime.getRuntime();
		runtime.exec("ant -f ../../build.xml stop" + server);
		Thread.sleep(6000);
		runtime.exec("ant -f ../../build.xml start" + server);
		Thread.sleep(3000);
		while (!isRunning()) {
			//System.out.println("waiting");
			Thread.sleep(2000);
		}
	}

	/**
	 *  Description of the Method
	 */
	private void loadGram() {
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
			log("loadGram(): loadGram ERROR: " + ex.toString());
			ex.printStackTrace();
			System.exit(-1);
		}
	}

	/**
	 * @param  input          Description of the Parameter
	 * @param  language       Description of the Parameter
	 * @param  option         Description of the Parameter
	 * @return                Description of the Return Value
	 * @exception  Exception  Description of the Exception
	 */
	public String sendWebRequest(String input, String language, String option) throws Exception {
		//System.out.println("sendWebRequest() url = " + url);
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
			log("sendWebRequest(): htmlElement = null");
			return null;
		}
	}

	/**
	 *  
	 *
	 * @param  input          Description of the Parameter
	 * @param  language       Description of the Parameter
	 * @param  option         Description of the Parameter
	 * @return                Description of the Return Value
	 * @exception  Exception  Description of the Exception
	 */
	public boolean check(String input, String language, String option) throws Exception {
		//System.out.println("=============================  test: " + language + " " + option + "==============");
		int retry = 0;
		int maxRetry = 3;
		while (true) {
			retry++;
			try {
				if ((webOutput = sendWebRequest(input, language, option)) != null) {
					break;
				} else {
					log("check(): retry = " + retry);
					if (retry == maxRetry) {
						retry = 0;
						stopStartServerWeb();
					}
				}
			} catch (com.meterware.httpunit.HttpException ex1) {
				log("sendWebRequest(): HttpException: " + ex1);
				if (retry == maxRetry) {
					retry = 0;
					stopStartServerWeb();
				}
			}
			catch (Exception ex) {
				log("sendWebRequest(): Exception: " + ex);
				if (retry == maxRetry) {
					retry = 0;
					stopStartServerWeb();
				}
			}
			Thread.sleep(1000 * retry); 
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
			//System.out.println("ERROR !!!!!!!!!!!!!!!!!!!!");
			log("check(): ERROR the web output is different !!!!!!!!!!!!!!!!!!!!");
			return false;
		}
		return true;
	}

	void oneTest() {
		//log("test");
		try {
			if (!check(null, "english", "option2")) {
				log("run(): check error english");
			}
			if (!check(null, "french", "option2")) {
				log("run(): check error english");
			}
			if (!check(null, "spanish", "option2")) {
				log("run(): check error english");
			}
			if (!check(null, "italian", "option2")) {
				log("run(): check error english");
			}
			if (!check(null, "portuguese", "option2")) {
				log("run(): check error english");
			}
			//check(null, "german", "option2")) {log("run(): check error english");}
		} catch (com.meterware.httpunit.HttpException ex) {
			log("run(): HttpException " + ex.toString());
		} catch (Exception ex) {
			log("run(): " + ex.toString());
		}
	}
	
	void periodicTest() {
		//expressed in milliseconds
		long periode = 1000*60*3; //every 3 minutes
		//long periode = 1000*5*1; //every 5s
		hStop = 4;
		mStop = 30;
		String mes = "-------------" + new Date().toString() + "---------------\n";
		mes += "server = " + server + "\n";
		mes += "rootXip = " + rootXip + "\n";
		mes += "url = " + url + "\n";
		mes += "test every = " + periode/1000 + " seconds" + "\n";
		mes += "no check from " + hStop + ":00 to " + hStop + ":" + mStop  + "\n";
		log(mes);
		System.out.println(mes);
		System.out.println("\nlog = " + new File("log").getAbsolutePath() + "\n");
		Timer timer = new Timer();
		timer.scheduleAtFixedRate(this, 0, periode);
	}
	
	public void run() {
		//String mes = "Checking at " + new Date() + "  " + url;
		//System.out.println(mes);
		//log(mes);
		Calendar cal = Calendar.getInstance();
		int hour = cal.get(Calendar.HOUR_OF_DAY);
		int minute = cal.get(Calendar.MINUTE);
		// do nothing from 04:00 to 04:30   (during cns server restart)
		boolean doIt = true;
		if (hour == hStop) {
			if (minute < mStop) {
				doIt = false;
			}
		}
		if (doIt) {
			oneTest();
		} else {
			//log("out of time");
		}
		System.out.flush();
	}
	
	/**
	 *  
	 *
	 * @param  args           The command line arguments
	 * @exception  Exception  Description of the Exception
	 */
	public static void main(String[] args) throws Exception {
		//redirect out to a file
		//System.setOut(new PrintStream(new BufferedOutputStream(new FileOutputStream("log"))));
		Test test = new Test();
		test.server = args[0];
                test.rootXip = args[1];
		if (test.server.compareTo("ext") == 0) {
			test.init_extern();
		} else if (test.server.compareTo("int") == 0) {
			test.init_intern();
		} else if (test.server.compareTo("pub") == 0) {
			test.init_public();
		} else if (test.server.compareTo("dev") == 0) {
			test.init_dev();
		} else {
			System.out.println("server name unknow");
			System.exit(-1);
		}
		//test.oneTest();
		test.periodicTest();
	}
}

