/*
 * TemporalOptions.java
 *
 * Created on February 5, 2007, 1:31 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality;

import common.Common;

/**
 * Class for temporal options
 * @author xtannier
 */
public class TemporalOptions {
    

    /*
     * Values for configuration file name
     */
    /** url of the temporal dictionary */
    public static final String TEMPORAL_DICT = "TEMPORAL_DICT";
    /** level of temporal inference expected */
    public static final String TEMPORAL_INFERENCE_LEVEL = "TEMPORAL_INFERENCE_LEVEL";
    /** maximum number of sentences for which the
     *   composition of temporal relations is calculated
     *   (0 = infinite)
     */
    public static final String TEMPORAL_COMPOSITION_LEVEL = "TEMPORAL_COMPOSITION_LEVEL";
    /** TRUE or 1 if temporally robust     
     * means that it won't crash because of a Temporal Inconsistency
     */
    public static final String TEMPORAL_ROBUST = "TEMPORAL_ROBUST";
    /** true if the temporal relations should be
     *  infered/kept in an economy mode
     */
    public static final String TEMPORAL_ECONOMY_MODE = "TEMPORAL_ECONOMY_MODE";
    
    /** true if print temporal dictionary for each sentence */
    public static final String TEMPORAL_PRINT_DICT = "TEMPORAL_PRINT_DICT";

    
    /**
     * Treats temporality-specific properties.
     *
     * @param name the name of the property
     * @param value the value of the property
     * @return -1 if the treatment failed (basically if the property is unknown), 0 otherwise.
     */
    public static int setProperty(String name, String value){
        name = name.toUpperCase();
        if (name.equals(TEMPORAL_INFERENCE_LEVEL)){
            return 0;
        } else if (name.equals(TEMPORAL_COMPOSITION_LEVEL)) {
            return 0;
        } else if (name.equals(TEMPORAL_DICT)) {
            return 0;            
        } else if (name.equals(TEMPORAL_ROBUST)) {
            return 0;            
        } else if (name.equals(TEMPORAL_ECONOMY_MODE)) {
            return 0;           
        } else if (name.equals(TEMPORAL_PRINT_DICT)) {
            if (Common.isBoolean(value))
                return 0;
            else
                return -1;
        } else
            return -1;
    }        
}
