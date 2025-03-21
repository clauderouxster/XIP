/*
 * CommonLog.java
 *
 * Created on February 5, 2007, 11:01 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common;

/**
 * Class used to handles logs
 * @author xtannier
 */
public class CommonLog {
    
    /**
     * Writes an error message into log.
     * @param message the message to write.
     */
    public static void errorMessage(String message){
        System.err.println(message);
        System.exit(-1);
    }
    
    
    /**
     * Writes a warning message into log.
     * @param message the message to write.
     */
    public static void warningMessage(String message){
        System.out.println("----------------------------------");
        System.out.println(message);
        System.out.println("----------------------------------");        
    }
    
    
    /**
     * Writes an info message into log.
     * @param message the message to write.
     */
    public static void infoMessage(String message){
        System.out.println(message);
    }
    
}
