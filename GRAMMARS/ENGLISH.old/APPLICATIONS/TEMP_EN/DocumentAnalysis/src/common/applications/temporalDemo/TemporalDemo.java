/*
 * TemporalDemo.java
 *
 * Created on May 30, 2007, 3:44 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.applications.temporalDemo;

import com.xerox.jxip.XipNode;
import common.Common;
import common.Offset;
import common.Options;
import common.TypedPropertyException;
import common.applications.*;
import documentAnalysis.namedEntities.Entity;
import documentAnalysis.namedEntities.EntitySet;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.TemporalElement;
import documentAnalysis.temporality.TemporalGraph;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.events.Event;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import java.awt.Color;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.HashMap;
import java.util.Vector;
import javax.swing.JOptionPane;
import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;

/**
 *
 * @author xtannier
 */
public class TemporalDemo extends Application{
    
    private DemoFrame demoFrame;
    private HashMap<Long, Vector<Object>> viewOffsets;
    // in the following one, the objects are XipNodes.
    private HashMap<Long, Vector<Object>> infosOffsets;
    private boolean tempexpr;
    private boolean ne;
    private VagueDate manualDCT;
    
    private static Vector<String> corpusNames;
    public static final Color TEMPEXPR = new Color(255,0,0);
    public static final Color ENTITY = new Color(0,0,255);
    public static final Color RELATION1 = new Color(255,0,0);
    public static final Color RELATION2 = new Color(255,100,0);
    public static final Color RELATION3 = new Color(255,150,50);
    public static final Color RELATION4 = new Color(255,255,0);
    public static final Color DCT_FG = new Color(50,50,50);
    public static final Color DCT_BG = new Color(80,180,230, 160);
    public static final Color TEXT_BG = new Color(80,180,230, 160);
    
    
//    public static final Color EVENT = new Color(255,35,4,160);
    
    static{
        corpusNames = new Vector<String>();
        corpusNames.add("None (text file)");
        corpusNames.add("Test");
        corpusNames.add("Biographies");
//        corpusNames.add("None (XML file)");
        corpusNames.add("WSJ");
        corpusNames.add("Reuters");
    }
    
    /** Creates a new instance of TemporalDemo */
    public TemporalDemo() {
        super();
        this.needsInput = false;
        this.viewOffsets = new HashMap<Long, Vector<Object>>();
        this.infosOffsets = new HashMap<Long, Vector<Object>>();
        this.tempexpr = true;
        this.ne = true;
        
    }
    
    
    /** Creates a new instance of TemporalDemo
     *
     * @param options the parser options
     */
    public TemporalDemo(Options options) {
        this();
        this.demoFrame = new DemoFrame(corpusNames, options, this);
        this.demoFrame.setVisible(true);
        this.tempexpr = this.demoFrame.isTempexprEnabled();
        this.ne = this.demoFrame.isNamedEntitiesEnabled();
        this.manualDCT = null;
    }
    
    
    /**
     * Clear whatever you want
     *
     */
    public void clear(){
        this.viewOffsets.clear();
        this.infosOffsets.clear();
        this.demoFrame.clear();
        this.manualDCT = null;
    }

    
    
    
    /**
     * Method launched just before the main parsing operation.
     *
     * @param options the parser options
     */
    public void preParsing(Options options){
        this.demoFrame.launchParsing();
    }
    
    /**
     * Launches the parsing
     */
    public void launchParsing(Options options) {
        try {
            options.getParser().parse(options);
            
            // DCT
            if (this.demoFrame.isTempexprEnabled()) {
                VagueDate dct = options.getDct();
                if (!dct.isDefined()) {
                    String reponse;
                    String message = "No document creation time has been found. Do you want to specify one?";
                    reponse = JOptionPane.showInputDialog(this.demoFrame, message,
                            VagueDate.getToday().toString());
                    if (reponse != null) {
                        String fileName = options.getInputFileName();
                        this.clear();
                        options.clear();
                        options.setInputString("DCT is " + reponse);
                        options.setIsInputStandard(true);
                        options.getParser().parse(options);
                        TemporalGraph graph = options.getTemporalGraph();
                        Vector<VagueDate> dates = graph.getDates();                        
                        if (!dates.isEmpty()){
                            int i = 0;
                            do {
                                dct = dates.get(i);
                            } while (!dct.isDefined());
                            this.clear();
                            options.clear();
                            this.demoFrame.clear();
                            this.manualDCT = dct;
                            options.setInputFileName(fileName);
                            options.setIsInputStandard(false);
                            options.setInputString("");
                            this.demoFrame.setOptions(options);                            
                            this.demoFrame.launchParsing();                            
                        }
                    }
//                    label.setText(reponse);
                }
            }
        } catch (TypedPropertyException ex) {
            ex.printStackTrace();
        } catch (TemporalConsistencyException ex) {
            ex.printStackTrace();
        }
    }
    
    
    /**
     * Gets different information from the input file (DCT for example).
     *
     * @param options the parser options.
     */
    public void getInputInfo(Options options){
        if (this.manualDCT != null && this.manualDCT.isDefined()) {
            try {
                options.setDct(this.manualDCT);
            } catch (TemporalConsistencyException ex) {
                ex.printStackTrace();
            }
        }
    }        
    
    /**
     *
     */
    public void SetTemporalAnnotation(boolean value) {
        this.tempexpr = value;
    }
    
    
    /**
     *
     */
    public void SetNEAnnotation(boolean value) {
        this.ne = value;
    }
    
    
    /**
     * Adds the object in the list of object corresponding to all position
     * represented by the offset
     */
    private HashMap<Long, Vector<Object>> addOffsetInfo(HashMap<Long, Vector<Object>> map, Offset offset, Object object) {
        Long pos;
        for (long l = offset.getLeft() ; l <= offset.getRight() ; ++l){
            pos = new Long(l);
            Vector<Object> oldObjects = map.get(pos);
            
            if (oldObjects == null){
                oldObjects = new Vector<Object>();
            }
            if (!oldObjects.contains(object))
                oldObjects.add(object);
            map.put(new Long(l), oldObjects);
        }
        return map;
    }
    
    
    
    public StyledDocument highlightStyledDocumentFromViewPosition(long pos,
            StyledDocument document){
        int left;
        int right;
        SimpleAttributeSet attr;
        
        // UNhighlight everything
        attr = new SimpleAttributeSet();
        StyleConstants.setBackground(attr, Color.WHITE);
        document.setCharacterAttributes(0, document.getLength(), attr, false);
        
        Vector<Object> objects = this.infosOffsets.get(new Long(pos));
        
        if (objects != null){
            // these objects should be XipNodes
            for (Object object : objects) {
                XipNode node = (XipNode)object;
                if (node != null){
                    left = (int)node.getLeftChar();
                    right = (int)node.getRightChar();
                    attr = new SimpleAttributeSet();
                    StyleConstants.setBackground(attr,TEXT_BG);
                    document.setCharacterAttributes(left, right - left, attr, false);
                }
            }
        }
        
        return document;
    }
    
    
    public StyledDocument updateStyledDocumentWithNamedEntities(Options options,
            StyledDocument document){
        int left;
        int right;
        SimpleAttributeSet attr;
        XipNode node;
        
        EntitySet entitySet = options.getEntitySet();
        
        for (Entity entity : entitySet) {
            node = entity.getXipNode();
            if (node != null){
                left = (int)node.getLeftChar();
                right = (int)node.getRightChar();
//                Offset offset = new Offset(left, right);
                attr = new SimpleAttributeSet();
                StyleConstants.setForeground(attr,ENTITY);
                StyleConstants.setItalic(attr, true);
                document.setCharacterAttributes(left, right - left, attr, false);
                this.viewOffsets = this.addOffsetInfo(this.viewOffsets, new Offset(left, right), entity);
            }
        }
        
        return document;
    }
    
    
    public StyledDocument updateStyledDocumentWithTemporalExpressions(Options options,
            StyledDocument document){
        TemporalGraph temporalGraph = options.getTemporalGraph();
        int left;
        int right;
        SimpleAttributeSet attr;
        XipNode node;
        
        // all temporal elements
        // Dates
        Vector<TemporalConstituent> elements = new Vector<TemporalConstituent>();
        elements.addAll(temporalGraph.getDates());
        elements.addAll(temporalGraph.getDurations());
        for (TemporalConstituent element : elements){
            node = element.getXipNode();
            if (node != null){
                left = (int)node.getLeftChar();
                right = (int)node.getRightChar();
//                Offset offset = new Offset(left, right);
                attr = new SimpleAttributeSet();
                StyleConstants.setForeground(attr,TEMPEXPR);
                StyleConstants.setItalic(attr, true);
                document.setCharacterAttributes(left, right - left, attr, false);
                this.viewOffsets = this.addOffsetInfo(this.viewOffsets, new Offset(left, right), element);
            }
        }
        
        // all events
        Vector<Event> events = new Vector<Event>();
        events.addAll(temporalGraph.getEvents());
        for (Event event : events) {
            node = event.getXipNode();
            if (node != null){
                left = (int)node.getLeftChar();
                right = (int)node.getRightChar();
//                Offset offset = new Offset(left, right);
                attr = new SimpleAttributeSet();
                StyleConstants.setBold(attr, true);
                document.setCharacterAttributes(left, right - left, attr, false);
                this.viewOffsets = this.addOffsetInfo(this.viewOffsets, new Offset(left, right), event);
            }
        }
        
        return document;
        
    }
    
    
    private void addDCT(DefaultStyledDocument document, Options options) throws BadLocationException {
        VagueDate dct = options.getDct();
        if (dct != null && dct.isDefined()){
            SimpleAttributeSet attr = new SimpleAttributeSet();
            StyleConstants.setBold(attr, true);
            StyleConstants.setItalic(attr, true);
            StyleConstants.setForeground(attr, DCT_FG);
            StyleConstants.setBackground(attr, DCT_BG);
            document.insertString(document.getLength(), "DCT is " + dct.toString() + "\n\n", attr);
        }
    }
    
    
    
    private void addTemporalRelations(TemporalElement element, DefaultStyledDocument document,
            Options options) throws BadLocationException {
        
        Vector<TemporalRelation> temporalRelations;
        if (element != null) {
            temporalRelations = element.getTemporalRelations();
        } else {
            temporalRelations = options.getTemporalGraph().getRelations();
        }
        SimpleAttributeSet attr;
        int currentLength;
        String text;
        
        attr = new SimpleAttributeSet();
        StyleConstants.setItalic(attr, true);
        
        temporalRelations = TemporalRelation.sortRelations(temporalRelations);
        
        for (TemporalRelation temporalRelation : temporalRelations){
            TemporalElement element1 = temporalRelation.getTemporalElement1();
            TemporalElement element2 = temporalRelation.getTemporalElement2();
            if (element == null)
                element = element1;
            text = "";
            if ((element1.isDefined() || !element.isDefined()) &&
                    element2.isDefined()) {
                if (!(element1 instanceof VagueDate && element2 instanceof VagueDate)) {
                    text += temporalRelation.toString() + "\n";
                    currentLength = document.getLength();
                    
                    // relation color
                    // depends on the importance
                    // 3/ disjunctive relation
                    if (temporalRelation.isADisjunction())
                        StyleConstants.setForeground(attr, RELATION3);
                    else {
                        // 1/ DURING or INCLUDES
                        int r = temporalRelation.getRelation();
                        if (r == TemporalRelation.INCLUDES || r == TemporalRelation.DURING)
                            StyleConstants.setForeground(attr, RELATION1);
                        else
                            StyleConstants.setForeground(attr, RELATION2);
                    }
                    // if the temporal relation comes from the GRAMMAR
                    // bold text
                    if (temporalRelation.getPriority() <= TemporalRelation.GRAMMAR2)
                        StyleConstants.setBold(attr, true);
                    else
                        StyleConstants.setBold(attr, false);
                    document.insertString(document.getLength(), text, attr);
                    if (element.getXipNode() != null) {
                        this.addOffsetInfo(this.infosOffsets, new Offset(currentLength, document.getLength()),
                                element.getXipNode());
                    }
                    if (element2.getXipNode() != null) {
                        this.addOffsetInfo(this.infosOffsets, new Offset(currentLength, document.getLength()),
                                element2.getXipNode());
                    }
                }
            }
        }
        document.insertString(document.getLength(), "\n", attr);
    }
    
    
    
    private void addTemporalAnnotations(DefaultStyledDocument document,
            TemporalElement element, Options options) throws BadLocationException {
        if (element == null) {
            addDCT(document, options);
            addTemporalRelations(null, document, options);
        } else {
            int currentLength;
            String text = "";
            XipNode node = element.getXipNode();
            SimpleAttributeSet attr = new SimpleAttributeSet();
            StyleConstants.setBold(attr, true);
            StyleConstants.setItalic(attr, true);
            if (element instanceof VagueDate) {
                if (node != null) {
                    if (node.hasFeature("c_set")) {
                        text = "DATE (SET): ";
                    } else {
                        text = "DATE: ";
                    }
                }
            } else if (element instanceof Duration) {
                if (node != null) {
                    if (node.hasFeature("c_set")) {
                        text = "DURATION (SET): ";
                    } else {
                        text = "DURATION: ";
                    }
                }
            } else if (element instanceof Event) {
                text = "EVENT: ";
            }
            document.insertString(document.getLength(), text, attr);
            currentLength = document.getLength();
            TemporalGraph graph = options.getTemporalGraph();
            VagueDate dateToKeep = null;
            String substituteText = "";
            // gets the element textual representation
            if (element.isDefined()) {
                text = element.toString();
            } else {
                // if not defined, try to get an EQUALS temporal relation
                Vector<TemporalRelation> temporalRelations =
                        element.getTemporalRelations(TemporalRelation.EQUALS, -1);
                for (TemporalRelation temporalRelation : temporalRelations) {
                    TemporalElement elem = temporalRelation.getOtherElement(element);
                    if (elem instanceof VagueDate) {
                        if (dateToKeep == null)
                            dateToKeep = (VagueDate)elem;
                        else {
                            try {
                                dateToKeep = dateToKeep.merge((VagueDate)elem);
                            } catch (TemporalConsistencyException ex) {
                                ex.printStackTrace();
                            }
                        }
                    }
                }
                if (dateToKeep != null)
                    text = dateToKeep.toString();
                else {
                    // if not found, get the DURING temporal relation
                    // (for dates)
                    temporalRelations =
                            element.getTemporalRelations(TemporalRelation.DURING, -1);
                    dateToKeep = null;
                    for (TemporalRelation temporalRelation : temporalRelations) {
                        TemporalElement elem = temporalRelation.getOtherElement(element);
                        if (elem instanceof VagueDate) {
                            if (dateToKeep == null)
                                dateToKeep = (VagueDate)elem;
                            else {
                                try {
                                    dateToKeep = dateToKeep.merge((VagueDate)elem);
                                } catch (TemporalConsistencyException ex) {
                                    ex.printStackTrace();
                                }
                            }
                        }
                    }
                    if (dateToKeep != null && dateToKeep.isDefined())
                        text = dateToKeep.toString();
                    else {
                        if (node == null)
                            text = element.toString();
                        else
                            text = node.getSurface();
                    }
                }
                substituteText = text;
            }
            text += "\n\n";
            document.insertString(document.getLength(), text, attr);
            if (node != null) {
                this.addOffsetInfo(this.infosOffsets, new Offset(currentLength, document.getLength()),
                        node);
            }
            
            
            // Temporal relations
            if (dateToKeep != null && dateToKeep.isDefined())
                addTemporalRelations(dateToKeep, document, options);
            else
                addTemporalRelations(element, document, options);
            addDCT(document, options);
        }
        
        
        
    }
    
    
    /**
     * Returns a DefaultStyledDocument representing the informations
     * on the element corresponding to the specified offset
     *
     * @param options the parser options
     * @return a DefaultStyledDocument representing the parsed document
     */
    public DefaultStyledDocument getStyledInformations(long pos, Options options){
        DefaultStyledDocument document = new DefaultStyledDocument();
        int currentLength = 0;
        
        Vector<Object> objects = this.viewOffsets.get(new Long(pos));
        this.infosOffsets.clear();
        
        try{
            VagueDate dct = options.getDct();
            
            // nothing clicked (lauched just after parsing, all must be shown)
            if (pos < 0) {
                addTemporalAnnotations(document, null, options);
            }
            // something clicked but no information on it
            else if (objects == null && pos > 0) {
                // info about the DCT
                if (this.tempexpr){
                    addDCT(document, options);
                }
//                document.insertString(0, "Pas trouve !", new SimpleAttributeSet());
            } else {
                String text = "";
                for (Object object : objects) {
                    /***************************
                     * Temporal elements
                     * (event, date, duration)
                     ***************************/
                    if (object instanceof TemporalElement && this.tempexpr) {
                        TemporalElement element = (TemporalElement)object;
                        addTemporalAnnotations(document, element, options);
                    }
                    
                    /***************************
                     * Named entities
                     * (org, location, person)
                     ***************************/
                    if (object instanceof Entity && this.ne) {
                        XipNode node = ((Entity)object).getXipNode();
                        SimpleAttributeSet attr = new SimpleAttributeSet();
                        StyleConstants.setBold(attr, true);
                        StyleConstants.setItalic(attr, true);
                        
                        currentLength = document.getLength();
                        text = ((Entity)object).toDetailedString() + "\n\n";
                        document.insertString(document.getLength(), text, attr);
                        if (node != null) {
                            this.addOffsetInfo(this.infosOffsets, new Offset(currentLength, document.getLength()),
                                    node);
                        }
                    }
                }
                
                
            }
        } catch (BadLocationException ex) {
            ex.printStackTrace();
        }
        return document;
    }
    
    
    /**
     * Returns a DefaultStyledDocument representing the parsed document
     * as wanted for the application.
     *
     * @param options the parser options
     * @return a DefaultStyledDocument representing the parsed document
     */
    public StyledDocument getStyledDocument(Options options){
        StyledDocument document = new DefaultStyledDocument();
        try {
            document.insertString(0, Common.read(options.getInputFileName()), new SimpleAttributeSet());
            if (this.tempexpr)
                document = updateStyledDocumentWithTemporalExpressions(options, document);
            if (this.ne)
                document = updateStyledDocumentWithNamedEntities(options, document);
            
        } catch (FileNotFoundException ex) {
            ex.printStackTrace();
        } catch (BadLocationException ex) {
            ex.printStackTrace();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
        return document;
    }
    
}
