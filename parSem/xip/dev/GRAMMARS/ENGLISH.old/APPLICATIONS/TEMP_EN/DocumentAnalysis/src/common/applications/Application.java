/*
 * Application.java
 *
 * Created on February 23, 2007, 11:18 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.applications;

import com.xerox.jxip.XipUnit;
import common.Common;
import common.Options;
import common.applications.UnknownApplicationException;
import java.io.File;
import java.lang.reflect.Constructor;

/**
 * Class for application-specific methods
 * @author xtannier
 */
public class Application {
    
    protected String name;
    protected boolean needsInput = true;
    
    
    /** 
     * Creates a new instance of Application 
     */
    public Application() {
    }
    
    
    /**
     * Treats application-specific properties.
     *
     * @param name the name of the property
     * @param value the value of the property
     * @return -1 if the treatment failed (basically if the property is unknown), 0 otherwise.
     */
    public int setProperty(String name, String value){
        return 0;
    }    
    
    /**
     * Tests whether the application needs an input given by the run command.
     * Generally, applications with GUI do not need this, others do.
     *
     * @return true if the application needs an input given by the run command,
     * false otherwise.
     */
    public boolean needsInput() {
        return this.needsInput;
    }
    
    /**
     * Returns the name of the application.
     * @return the name of the application.
     */
    public String getName() {
        return name;
    }
    
    
    /**
     * Returns a new instance of Application according to the specified name.
     *
     * @param options the parser options
     * @throws common.UnknownApplicationException when the name of the corpus is unknown.
     * @return a new Application.
     */
    public static Application getNewApplication(Options options) throws UnknownApplicationException{
        String name = options.getProperty(Options.APPLICATION);
        
        if (name != null){
            String packagePath = "common.applications";
            Class<?> applicationClass;
            try {
                String applicationPackage = Common.getPackage(packagePath, name, true);
                applicationClass = Common.getClass(applicationPackage, name, true);                
            } catch (ClassNotFoundException ex) {
                throw new UnknownApplicationException(name);
            }
                       
            if (applicationClass == null) {
                throw new UnknownApplicationException(name);
            } else {
                try {
                    Constructor constructor = applicationClass.getConstructor(options.getClass());
                    return (Application)constructor.newInstance(options);
                } catch (Exception ex) {
                    throw new UnknownApplicationException(name);
                }
                
            }
        } else {
            return new Application();
        }
    }
    
    
    /**
     * Method launched just before the main parsing operation.
     * Used in:
     * <ul>
     *  <li>TemporalDemo
     * </ul>
     *
     * @param options the parser options
     */
    public void preParsing(Options options){
        
    }
    
    /**
     * Gets different information from the input file (DCT for example).
     * Used in:
     * <ul>
     *  <li>TemporalDemo
     * </ul>
     *
     * @param options the parser options.
     */
    public void getInputInfo(Options options){
        
    }
    
    /**
     * Returns an output file name from the input file.
     * Used in:
     * <ul>
     *  <li>
     * </ul>
     *
     * @param inputFileName the name of the input file
     * @return the name of the output file
     */
    public String getOutputFileName(String inputFileName){
        return inputFileName + ".out";
    }
    
    /**
     * Open the output file or connection if necessary and perform any appropriate operation
     * in the output.
     * Used in:
     * <ul>
     *  <li>TimeForEntities
     *  <li>TimeForEvents
     * </ul>
     *
     * @param options the parser options.
     */
    public void openOutput(Options options){
        
    }    
    
    
    /**
     * Update the output according to the new information brought by the current sentence
     * Used in:
     * <ul>
     *  <li>TimeForEntities
     *  <li>TimeForEvents
     * </ul>
     *
     * @param unit the currently parsed XipUnit
     * @param options the parser options.
     */
    public void updateOutput(XipUnit unit, Options options){
        
    }
    
    
    /**
     * Update the output according to the new information brought by the current document
     * Used in:
     * <ul>
     *  <li>TimeForEntities
     * </ul>
     *
     * @param options the parser options.
     */
    public void updateOutput(Options options){
        
    }    
    
    
    /**
     * Perform any appropriate operation and close the output file or connection
     * in the output.
     * Used in:
     * <ul>
     *  <li>TempEval
     *  <li>AutoCalendar
     * </ul>
     *
     * @param options the parser options.
     */
    public void closeOutput(Options options){
        
    }    
    
    /**
     * Returns an output file name from the input file. The new file name
     * will be located in the specified directory.
     * Used in:
     *
     *
     * @param inputFileName the name of the input file
     * @return the name of the output file
     */
    public String getOutputFileName(String inputFileName, String dirName){
        String abstractName = new File(inputFileName).getName();
        File dir = new File(dirName);
        if (!dir.exists())
            dir.mkdir();
        return dir.getAbsolutePath() + File.separator + abstractName + ".out";
    }
    
    
    /**
     * Clear whatever you want
     * Used in:
     * <ul>
     *  <li>TemporalDemo
     * </ul>
     *
     */
    public void clear(){
    }

    
    /** 
     * Prints a summary of elements that take memory
     */            
    public void showElements() {
        System.out.println("none");
    }
    
    
}
