/*
 * Tracer.java
 *
 * Created on May 29, 2007, 3:51 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;

import java.io.FileNotFoundException;
import java.io.PrintStream;
import java.text.NumberFormat;


/**
 *
 * @author ait
 */
public class Tracer {
    
    static private NumberFormat numberFormat = NumberFormat.getNumberInstance();
    
    private static boolean traceOn=true;
    private static PrintStream logFile = System.out; 
    
    /** Creates a new instance of Tracer */
    public Tracer(boolean traceflag) {
        traceOn = traceflag;
    }
    
    static public void setTrace(boolean b) {
        traceOn = b;
    }
    
    static public void setTraceFile(String filename) {
        try {
            logFile = new PrintStream(filename);
        } catch (FileNotFoundException ex) {
            logFile = System.out;
            System.err.println("Unable to open log file: "+filename);
        }
    }
    
    static void trace(String info) {
        if (traceOn==false)
            return;
        logFile.print(info );
    }
    static void traceln(String info) {
        if (traceOn==false)
            return;
        logFile.println(info );
    }
    
    static void traceAddNewReferent(Referent r) {
        traceln("Adding a new referent: " +
                r.getSurface() + "  (" + r.getTokId() + ")" );
    }
    
    static void traceTryingFindAntecedents(Referent r) {
        traceln("Trying to find antecedents for: " +
                r.getSurface() + "   (" + r.getTokId() + ")" +
                "  (" + r.getRefType().toString() + ") ... " );
    }
    
    static void traceTryingWithReferent(Referent r) {
        trace("     Trying with: " +
                r.getSurface() +
                "  (" + r.getTokId() + ")" );
    }
    
    static void traceScore(float score) {
        traceln(" --> score=" + numberFormat.format(score));
    }
    
    static void traceSuccess(Referent r) {
        traceln("    SUCCESS!! linked with: " + r.getId() + " " +
                r.getSurface() + "   (" +
                r.getTokId() + ")" );
    }
    
    static void traceFail() {
        traceln("");
        return;
    }
    
    
    
}
