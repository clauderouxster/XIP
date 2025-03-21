/*
 * XipUITestRunListener.java
 *
 * Created on February 26, 2007, 5:18 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.xipui;
/*
 * XipRunListener.java
 *
 * Created on February 15, 2007, 1:59 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */



import java.io.PrintStream;
import java.text.NumberFormat;
import org.junit.runner.*;
import org.junit.runner.notification.*;


/**
 *
 * @author vgrassau
 */
public class XipUITestRunListener extends RunListener {
    
    private final PrintStream fWriter;
    /** Default write on the System.out */
    public XipUITestRunListener() {
        this(System.out);
    }
    
    /**
     *Write on the prinstrem given
     *@param writer the Stream
     */
    public XipUITestRunListener(PrintStream writer) {
        this.fWriter= writer;
    }
    
    
    @Override
    /**
     *Call When all test have finished
     *@param result The result of the suite
     */
    public void testRunFinished(Result result) {
        printHeader(result.getRunTime());
        printFailures(result);
        printFooter(result);
    }
    
    
    /**
     *Call when a atomci test is starting
     *@param description the Description of the atomic test
     */
    @Override
    public void testStarted(Description description) {
        fWriter.append('.');
    }
    
    /**
     *Call when an atomic test failed
     *@param Failure the Failure for explain the causes
     */
    @Override
    public void testFailure(Failure failure) {
        fWriter.append("ERROR:" );
    }
    
    /**
     *Call when a test is ignored (with an @ignored annotation )
     *@param description a description of the atomic test
     */
    @Override
    public void testIgnored(Description description) {
        fWriter.append(description.getDisplayName() + " Ignored\n");
    }
    
        /*
         * Internal methods for the listener
         */
    /**
     *Return the writer
     *@return an PrintStream, default it's System.out
     *
     */
    private PrintStream getWriter() {
        return fWriter;
    }
    
    /**
     *Print an Header with the elapse time for the test
     *@param runTime the time elapsed
     */
    protected void printHeader(long runTime) {
        getWriter().println();
        getWriter().println("Time: " + elapsedTimeAsString(runTime));
    }
    
    /**
     *Print all failures found
     *@param result, the result of the suite
     */
    protected void printFailures(Result result) {
        if (result.getFailureCount() == 0) {
            return;
        } if (result.getFailureCount() == 1) {
            getWriter().println("There was " + result.getFailureCount() + " failure:");
        } else {
            getWriter().println("There were " + result.getFailureCount() + " failures:");
        }
        int i= 1;
        for (Failure each : result.getFailures()) {
            printFailure(each, i++);
        }
    }
    /**
     *Print a failure
     *@param failure the failure
     *@param count the number of the failure
     */
    protected void printFailure(Failure failure, int count) {
        printFailureHeader(failure, count);
        printFailureTrace(failure);
    }
    
    /** Print the header of a Failure and indicate its number 
     *@param failure the Failure
     *@param count the number of the failure
     */
    protected void printFailureHeader(Failure failure, int count) {
        getWriter().println(count + ") " + failure.getTestHeader());
    }
    
    /**
     *Print the trace of the failure
     *@param failure
     */
    protected void printFailureTrace(Failure failure) {
        getWriter().print(failure.getTrace());
    }
    
    /**
     *Print the footer of the result
     *@param result the result of the suite
     */
    protected void printFooter(Result result) {
        if (result.wasSuccessful()) {
            getWriter().println();
            getWriter().print("OK");
            getWriter().println(" (" + result.getRunCount() + " test" + (result.getRunCount() == 1 ? "" : "s") + ")");
            
        } else {
            getWriter().println();
            getWriter().println("FAILURES!!!");
            getWriter().println("Tests run: " + result.getRunCount() + ",  Failures: " + result.getFailureCount());
        }
        getWriter().println();
    }
    
    /**
     * Returns the formatted string of the elapsed time. Duplicated from
     * BaseTestRunner. Fix it.
     */
    protected String elapsedTimeAsString(long runTime) {
        return NumberFormat.getInstance().format((double) runTime / 1000);
    }
    
    /**
     *When an atomic test is finished succeed or not 
     */
    @Override
    public void testFinished(Description desc) throws Exception {
        super.testFinished(desc);
        System.out.println(desc.getDisplayName() + " finished");
    }
}
