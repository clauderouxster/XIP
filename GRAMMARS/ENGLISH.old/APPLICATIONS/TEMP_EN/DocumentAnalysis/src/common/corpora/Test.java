/*
 * Test.java
 *
 * Created on February 7, 2007, 8:28 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.corpora;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import common.CommonLog;
import common.Options;
import documentAnalysis.temporality.TempFunctions;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.inference.LinguisticInference;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import java.util.HashMap;
import java.util.Vector;

/**
 * This class contains methods for test corpora
 * @author xtannier
 */
public class Test extends Corpus{
    
    /** Creates a new instance of Test */
    public Test() {
        super();
        this.name = "TEST";
    }
    
    
    /** Creates a new instance of Test
     * @param options the parser options
     */
    public Test(Options options) {
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
         *  Test
         *  Beginning of the file name is YY_MM_DD
         */
        if (inputFileName.matches(".*/\\d\\d_\\d\\d_\\d\\d[^/]*")){
            VagueDate dct = null;
            String format = null;
            inputFileName = inputFileName.replaceAll(".*/", "");
            String year = inputFileName.substring(0, 2);
            String month = inputFileName.substring(3, 5);
            String day = inputFileName.substring(6, 8);            
            System.err.println("YEAR DCT : " + year);
            format = "+--" + year + month + day + "T-------O---W---";
            try{
                dct = LinguisticInference.getNewDate(format);
                options.setDct(dct);
            } catch (TemporalConsistencyException ex) {
                ex.printStackTrace();
            } catch (BadFormatException e) {
                e.printStackTrace();
            }
        }
    }
    
    
    /**
     * Gets different information from the current sentence (DCT for example).
     *
     * @param unit the current parsed sentence
     * @param options the parser options
     */
    public void getSentenceInfo(XipUnit unit, Options options) {
        String fileName = options.getInputFileName();
        /**********
         *  DCT
         **********/
             /* format is:
              *   DCT is <date>.
              *    e.g. : DCT is September 2006.
              */
        if (!options.getDct().isDefined()){
            VagueDate date = null;
            String value;
            
            Vector<XipDependency> dependencies = unit.getDependenciesByName("ATTRIB");
            XipNode node1;
            XipNode node2;
            for (XipDependency dep : dependencies){
                node1 = dep.getArg(1);
                node2 = dep.getArg(2);
                if (node1.getSurface().toUpperCase().equals("DCT") &&
                        node2.hasFeature("DATE", "*")){
                    
                    HashMap<String, String> tempNodeInfo;
                    tempNodeInfo = TempFunctions.getTempNodeInfo(fileName, unit.getSentenceNb(),
                            "DATE", node2.getGramIdNode(), options);
                    
                    if (tempNodeInfo != null){
                        // DATE
                        value = tempNodeInfo.get("value");
                        try {
                            date = VagueDate.getNewDate(value, unit, node2);
                            // decade only: add century
                            if (date.isCenturyMissing()){
                                date = date.completeCentury(19);
                            }
                            options.setDct(date);
//                            CommonLog.infoMessage("DCT is " + date);
                        } catch (TemporalConsistencyException ex) {
                            CommonLog.warningMessage("Temporal consistency exception when adding DCT");
                            options.getTemporalGraph().clear();
                        } catch (BadFormatException ex) {
                            ex.printStackTrace();
                        }
                    }
                }
            }
        }
    }
    
}
