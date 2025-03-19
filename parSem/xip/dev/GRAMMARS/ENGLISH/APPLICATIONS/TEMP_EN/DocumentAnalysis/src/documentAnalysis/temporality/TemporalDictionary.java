/*
 * TemporalDictionary.java
 *
 * Created on December 22, 2006, 4:25 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality;

import java.util.HashMap;
import java.util.Map;

/**
 * Class representing a temporal dictionary containing information coming from
 * XIP parsing (Python dictionary written into a database)
 * @author xtannier
 */
public class TemporalDictionary extends HashMap<String, HashMap<String, String>>{
    
    private static final long serialVersionUID = 1L;
    
    /** Creates a new instance of TemporalDictionary */
    public TemporalDictionary() {
        super();
    }
    
    /**
     * Returns a String representation of the TemporalDictionary
     *
     * @return a String representation of the TemporalDictionary
     */
    public String toString() {
        String result = "";
        for (Map.Entry<String, HashMap<String, String>> entry : this.entrySet()) {
            String key = entry.getKey();        
            HashMap<String, String> tempDictEntry = entry.getValue();
            result += "* " + key + ":\n";
            result += "     " + tempDictEntry.toString() + "\n";
        }
        return result;       
    }
}
