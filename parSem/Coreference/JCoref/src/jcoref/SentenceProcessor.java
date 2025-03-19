/*
 * SentenceProcessor.java
 *
 * Created on May 22, 2007, 4:00 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jcoref;


import com.xerox.jxip.*;
import java.util.Vector;
import java.util.Hashtable;


/**
 *
 * @author ait
 */
public class SentenceProcessor {
    
    private String grmFile;
    private JXip jxip;
    private int gHandler;
    private XipResultManager xipResultManager;
    private int numberOfSentences=0;
    protected Vector<SentenceParse> sentParses = new Vector<SentenceParse>();
    /** current list of sentences which contain quotations but for which
     * no sayer has been found yet. They are stored here in order to be processed
     * later when a sayer is available (in one of the following sentences) */
    private Vector<SentenceParse> delayedForSayerResol = new Vector<SentenceParse>();
    /** The node representing the "sayer" of the last found quotation */
    private Referent lastSayer;
    
    /** Creates a new instance of SentenceProcessor */
    public SentenceProcessor(String xip_grammar_file) {
        this.setGrammar(xip_grammar_file);
        this.numberOfSentences=0;
    }
    
    public SentenceProcessor(JXip xip_parser, int g_handler) {
        xipResultManager = new XipResultManager();
        jxip = xip_parser;
        gHandler = g_handler;
    }
    
    
    public void setGrammar(String xip_grammar_file) {
        xipResultManager = new XipResultManager();
        this.grmFile = xip_grammar_file;
        // loads the xip grammar
        try {
            System.out.println("CorefResolver: Loading grammar file: " + grmFile);
            // create a JXIP object to load the dynamic library in the VM
            jxip = new JXip();
            System.out.println(jxip.GetVersion());
            // load the grammar
            gHandler = jxip.XipGrmFile(grmFile, 0, true);
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }
    
    public void reloadGrammar() {
        try {
            gHandler = jxip.XipGrmFile(grmFile, 0, true);
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }
    
    
    /** This method is invoked by the XIP callback mecanism for each
     * parsed sentence. It stores the sentence parses and resolve
     * quotation authors */
    public void processSentence() {
        SentenceParse sentparse=null;
        XipUnit xipunit=null;
        
        // collect xipresult
        // create equivalent SentenceParse object (nodes + deps)
        XipResult xipres = this.xipResultManager.getXipResult();
        xipunit = (XipUnit) xipres.getXipUnits().elementAt(0);
        
        this.numberOfSentences++;
        sentparse = new SentenceParse(xipunit, numberOfSentences);
        System.err.println("Parsed sentence: " + xipunit.getSentenceString());
        // process quotation authors
        if (sentparse.startsWithQuotes()) {
            if (sentparse.getSayer(0) == null) {
                this.delayedForSayerResol.add(sentparse);
            } else {
                this.lastSayer = sentparse.getSayer(0);
                // if the preceding unresolved sentence citation are contiguous
                // to this, then the sayer of this is the sayer of those
                if (!this.delayedForSayerResol.isEmpty() &&
                        (sentparse.sentenceId -
                        this.delayedForSayerResol.lastElement().sentenceId) == 1) {
                    for(int i=0; i < this.delayedForSayerResol.size(); i++) {
                        this.delayedForSayerResol.elementAt(i).setSayer(0,this.lastSayer);
                    }
                    this.delayedForSayerResol.clear();
                }
            }
        } else {
            processDelayedSentenceCitations(sentparse);
        }
        
        Vector<XipLeaf> leaves = sentparse.getLeaves();
        int i = leaves.size()-1;
        if (leaves.elementAt(i).getSurface().compareTo(":")==0) {
            // sentence ending with a semicolon ":", it could be a "say" sentence
            i--;
            while (i>=0 && leaves.elementAt(i).fMatches("INDSPEECH","~"))
                i--;
            if ( i>=0 && leaves.elementAt(i).flMatches("VERB,INDSPEECH") ) {
                XipNode xipnode = leaves.elementAt(i).getMother().getDependentIn("SUBJ-N");
                Referent ref = new Referent(xipnode,numberOfSentences,CorefResolver.RefType.NONE);
                xipnode.setMiscInfo(ref);
                this.lastSayer = ref;
            }
            
        }
        
        //System.out.println(sentparse.toString() );
        
        // Now store the sentence parse
        this.sentParses.add(sentparse);
        
    }
    
    /**
     * Process the delayed sentence citations with no sayer
     * @param sentparse the current sentence parse (==null if end of text)
     */
    private void processDelayedSentenceCitations(SentenceParse sentparse) {
        // If the preceding unresolved citation sentences are too far
        // from current sentence (more than 2 sentence-distance), then
        // set their sayer to lastSayer or forget them
        if (!this.delayedForSayerResol.isEmpty()) {
            if (this.lastSayer!=null &&
                    ( sentparse==null || ( (sentparse.sentenceId -
                    this.delayedForSayerResol.lastElement().sentenceId) == 1) ) ) {
                for(int i=0; i < this.delayedForSayerResol.size(); i++) {
                    this.delayedForSayerResol.elementAt(i).setSayer(0,this.lastSayer);
                }
            }
            this.delayedForSayerResol.clear();
            this.lastSayer = null;
        }
    }
    
    
    /* Process the document with this sentence processor (parsing)
     **/
    void processDocument(String docfilename, boolean utf8flag, boolean xmlFlag) {
        if (utf8flag) {
            try {
                jxip.XipSetDisplayMode(gHandler, Const.UTF8_INPUT, 50);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        
        if (xmlFlag){ 
	        try {
	            // call XIP with current grammar to parse the doc
	        	xipResultManager.xipResultBuilderFromXmlFile(gHandler, docfilename,
	                    this, "processSentence", "processXmlTag");
	        	
	        } catch  (Exception ex) {
	            ex.printStackTrace();
	        }
        }else{
	        try {
	            // call XIP with current grammar to parse the doc
	        	xipResultManager.xipResultBuilderFromTextFile(gHandler, docfilename,
	                    this, "processSentence");
	        } catch  (Exception ex) {
	            ex.printStackTrace();
	        }
        }
        
        if (utf8flag) {
            try {
                jxip.XipRemoveFlagDisplay(gHandler, Const.UTF8_INPUT);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        
        // process the last delayed citations
        processDelayedSentenceCitations(null);
        
    }
    
    public void processXmlTag() {
        Hashtable<String, String> xmlTag = this.xipResultManager.getXmlTagInfo();
        
        
        XipResult r = xipResultManager.getXipResult();
        Vector<XipUnit> units = r.getXipUnits();
        for (XipUnit unit : units){
            System.out.println("unit :\t" + unit.getSentenceString());
        }
        System.out.println(this.numberOfSentences);
        System.out.println("list_des tags");
        for (String key : xmlTag.keySet()){
        	System.out.println(key + "\t" + xmlTag.get(key));
        }
    }
    
    /* Process the text string with this sentence processor (parsing)
     **/    
    void processString(String text, boolean utf8flag) {
        if (utf8flag) {
            try {
                jxip.XipSetDisplayMode(gHandler, Const.UTF8_INPUT, 50);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        
        try {
            // call XIP with current grammar to parse the text string
            xipResultManager.xipResultBuilderFromString(gHandler, text,
                    this, "processSentence");
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        
        if (utf8flag) {
            try {
                jxip.XipRemoveFlagDisplay(gHandler, Const.UTF8_INPUT);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        
        // process the last delayed citations
        processDelayedSentenceCitations(null);
    }
    
    
    /* Initialize this sentence processor
     **/
    void init() {
        this.sentParses.clear();
        this.numberOfSentences=0;
        this.lastSayer = null;
        this.delayedForSayerResol.clear();
    }
    
    
    Vector<SentenceParse> getSentenceParses() {
        return sentParses;
    }
    
    
}
