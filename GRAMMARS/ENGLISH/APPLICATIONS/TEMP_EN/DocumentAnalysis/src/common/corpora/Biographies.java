/*
 * Biographies.java
 *
 * Created on February 7, 2007, 9:40 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.corpora;

import common.Options;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;

/**
 * This class contains methods for Biographies for Vikef
 * @author xtannier
 */
public class Biographies extends Corpus{
    
    /**
     * Creates a new instance of Biographies
     */
    public Biographies() {
        super();
        this.name = "Biographies";
    }
    
    /**
     * Creates a new instance of Biographies
     * 
     * @param options the parser options
     */
    public Biographies(Options options) {
        this();
    }    
    
    /**
     * Gets different information from the input file (DCT for example).
     *
     * @param options the parser options.
     */
    public void getInputInfo(Options options){
        String inputFileName = options.getInputFileName();
        /* get the Document Creation Time (DCT)
         *   from the file name
         *  IEEE Biographies for VIKEF
         *  last 4 characters of the file base name
         */
        VagueDate dct = null;
        String format = null;
        String year = inputFileName.substring(inputFileName.length() - 8, inputFileName.length() - 4);
        System.err.println("YEAR DCT : " + year);
        format = "+" + year + "----T-------O---W---";
        try{
            dct = VagueDate.getNewDate(format);
            options.setDct(dct);
        } catch (TemporalConsistencyException ex) {
            ex.printStackTrace();
        } catch (BadFormatException e) {
            e.printStackTrace();
        }
    }
    
}
