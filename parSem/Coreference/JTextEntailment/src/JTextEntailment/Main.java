/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package JTextEntailment;

import java.io.*;
import java.util.Vector;
//import JTextAnnotVisualization.TextAnnotVisualizationGUI;


/**
 *
 * @authors Guillaume Bouchard & Guillaume Jacquet
 */
public class Main{
    private static String textFile = "/home/gjacquet/tempo/list_test.txt";
    //private static String textFile = "/home/ecabrio/parSem/Coreference/list_test.txt";
    private static String method = "xmltexts_output";
//    private static String grmFile = "/opt/ParSem/users/gjacquet/parSem/Coreference/JCoref/grm/coref.grm";
    private static String grmFile = "/opt/ParSem/users/gjacquet/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEXTENTAILMENT/GRM/text_entailment_for_coref.grm";
//    private static String grmFile = "/opt/ParSem/users/gjacquet/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/TEXTENTAILMENT/GRM/text_entailment.grm";
//    private static String grmFile = "/opt/ParSem/users/gjacquet/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/JCOREF/GRM/coref.grm";

    private static AddaptedOutput corefResolver;
    private static boolean withCoref = true;
    
    public static void main(String[] args){
    	if (method.equals("test_coref")){
            textFile = "/opt/ParSem/Corpora/ENGLISH/Iran_Normalise/petit_extrait_Iran.txt";
    	}
        if (args.length >= 1){
            textFile = args[0];
        }
        if (args.length >= 2){            
        	method = args[1];  
        }
        if (args.length >= 3){            
        	grmFile = args[2];  
        }
        
        if (method.equalsIgnoreCase("xmltexts_output")){
        	xmlTexts_output();//add .rel at the end of the file
        } else if (method.equalsIgnoreCase("xmltext_output")){
        	xmlText_output(textFile);//add .rel at the end of the file
        } else if (method.equalsIgnoreCase("test_coref")){
        	text_output(textFile);//add .rel at the end of the file
        }else{
            System.out.println("invalid method type '" + method + "'");
            return;
        }
    }


    public static void xmlTexts_output(){
        //corefResolver = new AddaptedOutput(grmFile);
        try{
           //Input
        	InputStream ips = new FileInputStream(textFile);
        	InputStreamReader ipsr = new InputStreamReader(ips);
        	BufferedReader br = new BufferedReader(ipsr);

        	String lineStr;
           	while ((lineStr = br.readLine())!=null){
           		System.out.println(lineStr);
           		xmlText_output(lineStr);
           	}
           	br.close();
        }
        catch (Exception e){
           	System.out.println("error in file" + textFile +"\n" + e.toString());
       	}
    }
    
    public static void text_output(String pathname){
        corefResolver = new AddaptedOutput(grmFile);
        try{
           //Input
        	corefResolver.simpleCorefForTest(pathname);
        }
        catch (Exception e){
           	System.out.println("error in file" + textFile +"\n" + e.toString());
       	}
    }
    
    public static void xmlText_output(String pathname){
        corefResolver = new AddaptedOutput(grmFile);
        try{
           //Input
        	Pair pair;
        	File xmlfile = new File(pathname);
        	XmlReader xr = new XmlReader(xmlfile);

            String outfile = pathname+".xml";
            OutputStream ops = new FileOutputStream(outfile);
            OutputStreamWriter opsr = new OutputStreamWriter(ops);
            BufferedWriter bw = new BufferedWriter(opsr);

            System.out.println("starting " + pathname + " treatment");

            bw.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
            bw.write("<entailment-corpus>\n");
        	        	
        	while (xr.hasNextPair()){
        		pair = xr.readNextPair();
                bw.write(corefResolver.getDependenciesAsXmlOutput(pair, withCoref));
        	}
        	bw.write("  </pair>\n");
        	bw.write("</entailment-corpus>\n");

            bw.close();
            System.out.println(outfile + " written");
        }
        catch (Exception e){
        	System.out.println("error in file" + pathname +"\n" + e.toString());
        }
    }
}



