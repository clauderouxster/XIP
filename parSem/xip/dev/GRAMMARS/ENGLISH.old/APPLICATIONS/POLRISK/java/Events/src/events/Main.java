/*
 * Main.java
 * 
 * Created on Jul 20, 2007, 12:16:18 PM
 * 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package events;

import com.xerox.jxip.*;
import java.util.*;

/**
 *
 * @author arebotie
 */
public class Main {

    /** Creates a new instance of Main */
    public Main() {
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        String grmFile = "";
        String input = "";
        if (args.length == 2) {
            grmFile = args[0];
            input = args[1];
            //System.out.println("Two parameters are required: the path of the grm file, and the file to parse");
            //System.exit(-1);
        } else if (args.length == 1) {
            grmFile = args[0];
            input = "/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/POLRISK/corpus/demo3juillet.txt";
        } else {
            grmFile = "/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/POLRISK/data/english_pr.grm";
            input = "/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/POLRISK/corpus/demo3juillet.txt";
        }
       try {
            Parser parser = new Parser();
            parser.loadGrammar(grmFile);
            parser.parseTextFile(input);
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }

}
