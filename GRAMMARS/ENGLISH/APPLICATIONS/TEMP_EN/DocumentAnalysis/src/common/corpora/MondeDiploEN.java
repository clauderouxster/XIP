/*
 * MondeDiploEN.java
 *
 * Created on February 7, 2007, 9:38 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.corpora;

import common.CommonLog;
import common.Options;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import java.io.File;
import java.util.Hashtable;

/**
 * This class contains methods for Monde Diplomatique corpus in English
 * @author xtannier
 */
public class MondeDiploEN extends Corpus{
    
    /** Creates a new instance of MondeDiploEN */
    public MondeDiploEN() {
        super();
        this.name = "MONDEDIPLO_EN";
    }
    
    
    /** Creates a new instance of MondeDiploEN 
     * @param options the parser options
     */
    public MondeDiploEN(Options options) {
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
         * last two directories represent 1/ the year 2/ the month
         *   e.g. : ..../1999/10/file.xml
         */
        VagueDate dct = null;
        String format = null;
        String[] dirs = inputFileName.split(File.separator);
        String month = dirs[dirs.length - 2];
        String year = dirs[dirs.length - 3];
        if (!month.matches("[0-1][0-9]")){
            CommonLog.warningMessage("Incorrect month in DCT finder procedure for Le Monde Diplo (en): " + inputFileName);
        }
        if (!year.matches("[1-2][09][09][0-9]")){
            CommonLog.warningMessage("Incorrect year in DCT finder procedure for Le Monde Diplo (en): " + inputFileName);
        }
        format = "+" + year + month + "--T-------O---W---";
        try{
            dct = VagueDate.getNewDate(format);
                options.setDct(dct);
        } catch (TemporalConsistencyException ex) {
            ex.printStackTrace();
            System.exit(-1);
        } catch (BadFormatException e) {
            e.printStackTrace();
            System.exit(-1);
        }        
    }
    
    

}
