import com.xerox.jxip.JXip;
import java.lang.Thread;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    March 4, 2004
 */
class Test {

	static void memDisplay(String mes) throws Exception {
		Runtime runtime = Runtime.getRuntime();
		runtime.gc();
		Thread.sleep(1000);
		String str = "";
		//str += "totalMemory=" + runtime.totalMemory();
		//str += "    maxMemory=" + runtime.maxMemory();
		str += "    freeMemory=" + runtime.freeMemory();
		System.out.println(mes + ": " + str);
	}

	/**
	 *  Description of the Method
	 *
	 * @param  args  Description of the Parameter
	 */
	public final static void main(String args[]) {
	   try {
		JXip jxip = new JXip();
		memDisplay("before loading grammar");
		int gHandler = jxip.XipGrmFile(args[0], 0);
		if (gHandler == -1) {
			System.out.println("XipGrmFile call error");
			System.exit(-1);
		}
		String strToParse = " The Java Virtual Machine allows an application to have multiple threads of execution running concurrently.";
		String strXmlToParse = "<body>" + strToParse + "</body>";
		int col = 80;
		int displayMode = 0;

		memDisplay("before parsing");
		String str = "";
		String out = "";
		for (int i=0; i <1000; i++) {
			jxip.XipSetDisplayMode(gHandler, displayMode, col);
			jxip.parseString(gHandler, strToParse, (char) 0, (char) 1);
			out = jxip.parseUtf8String(gHandler, strToParse, (char) 0, (char) 1);
			//System.out.println("--------------------------------------------------------\n" + out);

			displayMode = 262144; //tree
			jxip.XipSetDisplayMode(gHandler, displayMode, col);
			out=jxip.parseUtf8String(gHandler, strToParse, (char) 0, (char) 1);
			//System.out.println("--------------------------------------------------------\n" + out);

			displayMode = 64; //full output
			jxip.XipSetDisplayMode(gHandler, displayMode, col);
			out=jxip.parseUtf8String(gHandler, strToParse, (char) 1, (char) 1); //XML output
			//System.out.println("--------------------------------------------------------\n" + out);

			out=jxip.parseStringXmlOS(gHandler, strXmlToParse);
			out=jxip.parseStringXmlOS(gHandler, strXmlToParse, 0, (char)0, (char)1);
			out=jxip.parseStringXml(gHandler, strXmlToParse);
		}
		memDisplay("after  parsing");
		jxip.XipFreeGrammar(gHandler);
		memDisplay("after freeing grammar");
	     } catch (Exception ex) {
			ex.printStackTrace();
			System.exit(-1);
             }
	}
}


