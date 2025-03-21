/*
 * Biographies.java
 *
 * Created on July 5, 2007, 8:43 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.applications.biographies;

import common.Options;
import common.applications.*;

/**
 * Class for specific methods for biography mining.
 * @author xtannier
 */
public class Biographies extends Application {
    
    /**
     * Creates a new instance of Biographies
     */
    public Biographies() {
        super();
        this.name = "BIOGRAPHY";        
    }
    
    
    /** Creates a new instance of TempEval 
     *
     * @param options the parer options
     */
    public Biographies(Options options) {
        this();
    }    
    
}
