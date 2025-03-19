/*
 * Reuters.java
 *
 * Created on February 7, 2007, 9:36 AM
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
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import java.util.HashMap;
import java.util.Vector;

/**
 * This class contains methods for Reuters corpus
 * @author xtannier
 */
public class Reuters extends Corpus{
    
    
    
    
    /** Creates a new instance of Reuters */
    public Reuters() {
        super();
        this.name = "REUTERS";
    }
    
    /** Creates a new instance of Reuters
     * @param options the parser options
     */
    public Reuters(Options options) {
        this();
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
        if (!options.getDct().isDefined()){
            VagueDate date = VagueDate.getNewDate();
            String value;
            Vector<XipDependency> dependencies = unit.getDependenciesByName("DCT");
            for (XipDependency dep : dependencies){
                XipNode node = dep.getArg(1);
                if (node.hasFeature("tempexpr", "*")){
                    HashMap<String, String> tempNodeInfo;
                    tempNodeInfo = TempFunctions.getTempNodeInfo(fileName, unit.getSentenceNb(),
                            "DATE", node.getGramIdNode(), options);
                    
                    if (tempNodeInfo != null){
                        // DATE
                        value = tempNodeInfo.get("value");
                        try {
                            date = VagueDate.getNewDate(value, unit, node);
                            options.setDct(date);
                        } catch (BadFormatException ex) {
                            ex.printStackTrace();
                        } catch (TemporalConsistencyException ex) {
                            CommonLog.warningMessage("Temporal consistency exception when adding DCT");
                            options.getTemporalGraph().clear();
                        }
                    }
                }
            }
        }
    }
    
}
