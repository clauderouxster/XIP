/*
 * GlasgowHerald.java
 *
 * Created on July 5, 2007, 3:02 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.corpora;

import com.xerox.jxip.XipUnit;
import common.CommonLog;
import common.Options;
import documentAnalysis.Main;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import java.util.Hashtable;

/**
 * This class contains methods for Glasgow Herald Tribune corpus
 * @author xtannier
 */
public class GlasgowHerald extends Corpus{
    
    
    private String currentDOCNO;
    private long docNoSentenceNumber;
    
    private boolean inText;
    
    /** Creates a new instance of GlasgowHerald */
    public GlasgowHerald() {
        super();
        this.name = "GlasgowHerald";
    }
    
    /** Creates a new instance of Reuters
     * @param options the parser options
     */
    public GlasgowHerald(Options options) {
        this();
    }
    
    /**
     * Gets different information from the XML tag currently parsed (e.g. DCT)
     * Called by the XML callback.
     *
     * @param xmlTagInfos the info about XML tag name and attributes.
     * @param unit the last XipUnit parsed.
     * @param options the parser options.
     */
    public void getXMLTagInfo(Hashtable<String,String> xmlTagInfos,
            XipUnit unit,
            Options options){
        // get tag name
        String tagName = xmlTagInfos.get("tagName");
        
//        System.err.println("tagName = " + tagName);
        
        if (tagName == null)
            CommonLog.errorMessage("Tag name can't be found in xmlTagInfos !");
        else if (tagName.equalsIgnoreCase("TEXT")){
            inText = true;
        } else if (tagName.equalsIgnoreCase("DOC") && inText){
            // end of document
            if (unit != null && unit.getSentenceNb() - this.docNoSentenceNumber > 1) {
                // run last SQL commands concerning the DCT
                if (options.getDct() != null && options.getDct().isAbsolute()) {
                    
                    System.err.println("---------------");
                    System.err.println("END OF DOCUMENT");
                    System.err.println("---------------");
                    try {                        
                        Main.endOfDocument(options);
                    } catch (TemporalConsistencyException ex) {
                        ex.printStackTrace();
                        System.exit(-1);
                    }
                    
//                    for (String command : this.commands){
//                        command = command.replaceAll("DCT--VALUE", options.getDct().getValue());
//                        this.connection.executeUpdate(command);
//                    }
//                    this.commands.clear();
                }
                
                options.clear();
//                options.showElements();
                inText= false;
            }
            
            /*************************************
             *  Document Number and new document
             *************************************/
            // Document Number (and first tag of a new document DOC)
        } else if (tagName.equalsIgnoreCase("DOCNO")){
            String currentDOCNO = unit.getSentenceString().replaceAll(" ", "");
            options.setProperty(CORPUS_DOCUMENT_ID, currentDOCNO);
            this.docNoSentenceNumber = unit.getSentenceNb();
            
            /**********
             *  DCT
             **********/
            // eg:
            // if DOCNO = GH870323-0124
            // DCT is 23/03/1987
            
            int year = Integer.parseInt(new String("19" + currentDOCNO.substring(2, 4)));
            int month = Integer.parseInt(currentDOCNO.substring(4, 6));
            int day = Integer.parseInt(currentDOCNO.substring(6, 8));
            try {
                VagueDate date = VagueDate.getNewDate(year, month, day);
                options.setDct(date);
            } catch (TemporalConsistencyException ex) {
                CommonLog.warningMessage("Temporal consistency exception when adding DCT");
                options.getTemporalGraph().clear();
            } catch (BadFormatException ex) {
                ex.printStackTrace();
                System.exit(-1);
            }
        }
    }
    
}
