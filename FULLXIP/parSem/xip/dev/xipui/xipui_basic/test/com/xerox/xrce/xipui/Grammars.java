/*
 * Grammars.java
 *
 * Created on February 15, 2007, 11:39 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.xipui;

/**
 *
 * @author vgrassau
 */
public class Grammars {
    
    /** Creates a new instance of Grammars */
    public Grammars() {
    }
    
    /*English */
    
    public static final String english           = "ENGLISH/GRMFILES/GRAM_GEN/gram_gen.grm";
    
    public static final String englishEntity     = "ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm";
    
    public static final String englishEntityOnly = "ENGLISH/GRMFILES/ENTITIES_ONLY/entities_only.grm";
    
    public static final String englishNorm       = "ENGLISH/GRMFILES/GRAM_NORM/gram_norm.grm";
    
    public static final String englishNormEntity = "ENGLISH/GRMFILES/GRAM_NORM/gram_norm_entit.grm";
    
    public static final String englishTemp       = "ENGLISH/GRMFILES/GRAM_TEMP/gram_temp.grm";
    
    public static final String englishGM         = "ENGLISH/GRMFILES/GRAM_GM/gram_GM.grm";
    
    public static final String englishToxigen    = "ENGLISH/GRMFILES/TOXIGEN/toxigen.grm";
    
    
    /** English Application Grammar */
    
     public static final String coreference    = "ENGLISH/APPLICATIONS/JCOREF/GRM/coref.grm";
     
    
    /* French grammar */
    public static final String french            = "FRENCH/BASIC/french.grm";
    
    public static final String frenchEntityOnly  = "FRENCH/BASIC/french_entityOnly.grm";
    
    public static final String frenchEntity      = "FRENCH/BASIC/french_entity.grm";
    /**
     *@deprecated
     */
     public static final String frenchNorm        = "FRENCH/BASIC/french_norm.grm";
    
    public static final String frenchPE          = "FRENCH/BASIC/french_PE.grm";
    
    public static final String frenchTrip        = "FRENCH/BASIC/french_trip.grm";
    
    public static final String frenchTripSQL     = "FRENCH/BASIC/french_trip_sql_labels.grm";
    
    /* Italian grammar */
    public static final String italian           = "ITALIAN/GRMFILES/italian.grm";
    
    public static final String italianPE         = "ITALIAN/GRMFILES/italian_PE.grm";
    
    
    /* German Grammar */
    public static final String german            = "GERMAN/GRMFILES/german.grm";
    
    public static final String germanPE         = "GERMAN/GRMFILES/german_PE.grm";
    
    public static final String germanTagger     = "GERMAN/GRMFILES/german_tagger.grm";
    
    /* Portuguese Grammar */
    
    public static final String portuguese     =   "PORTUGUESE/GRMFILES/portuguese.grm";
    
    
    /* Spanish grammar */
    public static final String spanish           = "SPANISH/GRMFILES/spanish.grm";
    
    public static final String spanishPE         = "SPANISH/GRMFILES/spanish_PE.grm";
    
}
