/*
 * Main.java
 *
 * Created on June 6, 2006, 3:58 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import java.util.Vector;
import net.didion.jwnl.JWNL;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Properties;
import net.didion.jwnl.JWNLException;
import net.didion.jwnl.data.IndexWord;
import net.didion.jwnl.data.POS;
import net.didion.jwnl.data.PointerTarget;
import net.didion.jwnl.data.PointerUtils;
import net.didion.jwnl.data.Synset;
import net.didion.jwnl.data.list.PointerTargetNode;
import net.didion.jwnl.data.list.PointerTargetNodeList;
import net.didion.jwnl.data.list.PointerTargetTreeNodeList;
import net.didion.jwnl.dictionary.Dictionary;
import org.omg.SendingContext.RunTime;

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
        
        
        CorefResolver coref = new CorefResolver("/home/ait/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/JCOREF/GRM/coref.grm");
        coref.resolveCoreferenceInFile("/home/ait/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/JCOREF/test/text-1", true);
        coref.printResults();
        //LexicalSemanticsResource lsr = new LexicalSemanticsResource();
        //System.out.println(lsr.getSynonymsOf("Bordeaux", POS.NOUN, 3).toString());
    }
    
}




