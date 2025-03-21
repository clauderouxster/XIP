/*
 * Main.java
 *
 * Created on June 6, 2006, 3:58 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;


import java.io.IOException;


/**
 *
 * @author ait
 */
public class Main {
    
    /** Creates a new instance of Main */
    public Main() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws IOException {
        // TODO code application logic here
        
        // Runtime.getRuntime().exec("tcsh setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/tmp/ait/linux/sharedLib");
        System.out.println("-------->"+System.getenv("LD_LIBRARY_PATH"));
        
        /* Properties props = System.getProperties();
        Enumeration enu = props.keys();
        while (enu.hasMoreElements()) {
            Object key = enu.nextElement();
            System.out.println(key + ": " + props.get(key));
        }
         
         */
        
        //System.out.println("---------------------"+System.getProperties());
        
        
        CorefResolver coref = new CorefResolver("H:/parSem/Coreference/JCoref/grm/coref.grm");
        coref.setTrace(true); 
        coref.resolveCoreferenceInFile("H:/parSem/Coreference/JCoref/test/textbug2.txt", true, false);
        coref.resolveCoreferenceInFile("/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/Tobacco_clean_strict/0/nkm67e00.txt", true, false);
        coref.printResults(); 
        /* LexicalSemanticsResource lsr = new LexicalSemanticsResource();
        System.out.println(lsr.getSynonymsOf("Bordeaux", POS.NOUN, 3).toString()); */
    }
    
}




