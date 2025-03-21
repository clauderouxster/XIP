/*
 * WSJ.java
 *
 * Created on February 7, 2007, 9:55 AM
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
 * This class contains methods for WSL corpus
 * @author xtannier
 */
public class WSJ extends Corpus{
    
    private long docNoSentenceNumber;
    
    private boolean inText;
    
    
    
    
    /** Creates a new instance of WSJ */
    public WSJ() {
        super();
        this.name = "WSJ";
        this.docNoSentenceNumber = 0;
        this.inText = false;
        
    }
    
    /** Creates a new instance of WSJ
     * @param options the parser options
     */
    public WSJ(Options options) {
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
                // run last commands 
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
            // if DOCNO = WSJ870323-0124
            // DCT is 23/03/1987
            
            int year = Integer.parseInt(new String("19" + currentDOCNO.substring(3, 5)));
            int month = Integer.parseInt(currentDOCNO.substring(5, 7));
            int day = Integer.parseInt(currentDOCNO.substring(7, 9));
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
