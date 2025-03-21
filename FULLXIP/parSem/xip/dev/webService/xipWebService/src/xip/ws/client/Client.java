package xip.ws.client;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import xip.ws.stub.XipWebServiceStub;
import xip.ws.stub.XipWebServiceStub.GetSupportedLanguageResponse;
import xip.ws.stub.XipWebServiceStub.ParseString;
import xip.ws.stub.XipWebServiceStub.ParseStringResponse;
import xip.ws.stub.XipWebServiceStub.ClientTotalResponseTime;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStreamReader;
//import org.apache.log4j.Logger;
/**
 *
 * @author grondeau
 */
public class Client {

    private static Client client;
    private String url;
    private long timeoutMs;
    //private static final Logger logger = Logger.getLogger(Client.class.getName());
    /**
     *
     */
    public void Client() {
    }

    void test0() {
        System.out.println("----test0----");
        System.out.println(client.getSupportedLanguage());
    }

    void test1() {
        System.out.println("----test1----");
        System.out.println(client.parseString("<Root><TEXT>This is a test1.</TEXT></Root>", "english"));
        // System.out.println(client.parseString("<Root><TEXT>Ceci est un test.</TEXT></Root>", "french1"));
        // System.out.println(client.parseString("<Root><TEXT>This is a test1.</TEXT></Root>", "english"));
        System.out.println(client.parseString("<Root><TEXT>Ceci est un test.</TEXT></Root>", "french"));
    }

    /**
     *
     * @param args
     */
    public static void main(java.lang.String[] args) {
        client = new Client();
        if (args.length == 1) {
            client.url = args[0];
            client.timeoutMs = 3 * 60 * 60 * 1000;
        } else {
            client.url = "http://choranche:8080/axis2/services/xipWebService";
            client.timeoutMs = 3 * 60 * 60 * 1000;
        }
        System.out.println("timeout in s: " + client.timeoutMs/1000 + " url = " + client.url);
        //client.test0();
        //client.test1();
        //client.parseFile("input-engS.xml", "output-engS.xml", "english");
        //client.parseFile("input-frS.xml", "output-frS.xml", "french");
        client.parseFile("input-engM.xml", "output-engM.xml", "english");
        //client.parseFile("input-engL.xml", "output-engL.xml", "english");
    }

    /**
     * 
     * @param inputFileName the Xml file to parse
     * @param outputFileName the result of the parser (in XML format)
     * @param language
     * @return
     */
    public boolean parseFile(String inputFileName, String outputFileName, String language) {
        System.out.println("\n---------- parseFile()   input: " + inputFileName + "    output: " + outputFileName);
        File inFile = new File(inputFileName);
        File outFile = new File(outputFileName);
        if (!inFile.exists()) {
            System.out.println("input file does not exist: " + inputFileName);
            return false;
        }
        if (outFile.exists()) {
            outFile.delete();
        }
        //read the input from the file and store it in a StringBuffer
        StringBuffer strBuf = new StringBuffer();
        try {
            FileInputStream fstream = new FileInputStream(inputFileName);
            DataInputStream in = new DataInputStream(fstream);
            BufferedReader br = new BufferedReader(new InputStreamReader(in));
            String strLine;
            while ((strLine = br.readLine()) != null) {
                strBuf.append(strLine);
            }
            in.close();
        } catch (Exception ex) {
            System.out.println(ex);
            return false;
        }
        //parse
        String output = parseString(strBuf.toString(), language);
        //store the output in a file
        try {
            FileWriter fstream = new FileWriter(outputFileName);
            BufferedWriter out = new BufferedWriter(fstream);
            out.write(output);
            out.close();
        } catch (Exception ex) {
            System.out.println(ex);
            return false;
        }
        return true;
    }

    /**
     * * call the parseString method provided by the web service XipWebService
     * Parse a string
     * @param input an XML string to parse
     * @param language
     * @return the xipParser result string (in XML format)
     */
    public String parseString(String input, String language) {
        System.out.println("\n---------- parseString()   language: " + language + "    input size: " + input.length());
        System.out.println("input size: " + input.length());
        String result = "";
        try {
            XipWebServiceStub stub = new XipWebServiceStub(url);
            ParseString req = new ParseString();
            req.setParam0(input);
            req.setParam1(language);
            stub._getServiceClient().getOptions().setTimeOutInMilliSeconds(timeoutMs);
            long startTime = System.currentTimeMillis();
            ParseStringResponse res = stub.parseString(req);
            result = res.get_return();
            long endTime = System.currentTimeMillis();
            // send this response time to the server
            ClientTotalResponseTime req1 = new ClientTotalResponseTime();
            req1.setParam0(endTime - startTime);
            stub.clientTotalResponseTime(req1);
            System.out.println("output size: " + result.length());
            System.out.println("response time: " + ((endTime - startTime) / 1000) + "s");
        } catch (Exception ex) {
            System.out.println(ex);
        }
        return result;
    }

    /**
     * call the getSupportedLanguage method provided by the web service XipWebService
     * @return the list of supported language
     */
    public String getSupportedLanguage() {
        String result = "";
        try {
            XipWebServiceStub stub = new XipWebServiceStub(url);
            //GetSupportedLanguageResponse req = new GetSupportedLanguageResponse();
            GetSupportedLanguageResponse res = stub.getSupportedLanguage();
            result = res.get_return();
        } catch (Exception ex) {
            System.out.println(ex);
        }
        return result;
    }
}
