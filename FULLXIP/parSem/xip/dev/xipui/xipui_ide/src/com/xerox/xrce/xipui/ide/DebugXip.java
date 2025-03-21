/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * DebugXip.java
 *
 * Created on February 22, 2007, 12:52 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide;

import com.xerox.jxip.XipLeaf;
import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipUnit;
import com.xerox.xrce.xipui.CorpusElement;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.Xipx;
import com.xerox.xrce.xipui.ide.ui.DebugElement;
import java.io.File;
import java.util.*;

/**
 *
 * @author vgrassau
 */
public class DebugXip {

    /** the DebugElement created */
    private Collection<DebugElement> elements;
    /** the Vector of the applied rules */
    private Vector<Integer> appliedRules;
    /** the Xip parser */
    private Xip xip;
    /**
     *The index unit of the DebugElement to break
     */
    private int indexUnit = 0;
    /**
     *The size of XipUnit for a ONE CorpusElement
     */
    private int sizeOfUnit = 0;
    /**
     *The CorpusElement to prepare
     */
    private CorpusElement current;
    /**
     *the grammar handler to use
     */
    private int currentGHandler = -1;
    /**
     *The xip result for the DebugElement to break
     */
    private XipResult xipResultBreaked;
    private Xipx xipx;
    private boolean isIndexLimit = false;
    private int ruleNumber = -1;

    /** Creates a new instance of DebugXip
     * @param xip 
     */
    public DebugXip(Xip xip) {
        this.xip = xip;
        xipx = new Xipx(xip);
        xipResultBreaked = null;
        elements = new Vector();
        appliedRules = new Vector();
        appliedRules.add(-1);
    }

    /**
     *Return the XipResult of a specific DebugElement given with the debug method
     *@return a XipResult or null, if the debugElement was not found.
     */
    public XipResult getResultBreaked() {
        return this.xipResultBreaked;
    }

    /**
     *Return the DebugElement indexed by the preProcess method.
     *@return a set of DebugElement or an empty Collection if the method prepareDebug was not called,
     *        or no XipResult was created by the parser.
     *@see prepareDebug(int,CorpusElement)
     */
    public Collection<DebugElement> getElements() {
        return this.elements;
    }

    /**
     *Return the vector correspoding to the applied rules for a DebugElement given by the debug method
     *@return a set of integer, the first element is -1.
     */
    public Vector<Integer> getAppliedRules() {
        return this.appliedRules;
    }

    /**
     *This method is a callBack, don't use directly,
     *This method is invoked when the prepareDebug(int,CorpusElement) method is invoked too by Xip.
     *For each XipResult, we create a indexed DebugElement representing the XipUnit and we add it in a Collection.
     *After you can use the debug method in order to load xip analysis step by step on a DebugElement
     *@throws java.lang.Exception 
     * @see getElements()
     */
    public void preProcess() throws Exception {
        XipResult xipResult = xipx.getXipResult();
        Vector<XipUnit> units = xipResult.getXipUnits();
        //using method Xip#callMethodOnXipUnitxxx() we should have only one XipUnit in the XipResult
        XipUnit unit = units.firstElement();
        //  DebugElement newSentence = new DebugElement(current,buildSentence(unit),elements.size());
        DebugElement newSentence = new DebugElement(current, unit.getSentenceString(), elements.size());
        newSentence.setUnitIndex(sizeOfUnit);
        elements.add(newSentence);
        sizeOfUnit += 1;
    }

    /**
     *This method is a call back, it's used internally.
     *Don't use directly
     *try to keep, the XipResult and the Vector of the applied rules for the DebugElement given with
     *the debug(int,DebugElement) method
     *
     *@see getAppliedRules()
     *@see getResultBreaked()
     *@throws java.lang.Exception 
     * @see debug(int,DebugElement,boolean,int)
     */
    public final void debugUnit() throws Exception {
        if (sizeOfUnit == indexUnit) {
            // we have the wanted XipUnit. Xip stop the parsing at the end of the method
            this.xipResultBreaked = xipx.getXipResult();
            xip.setXipResult(xipResultBreaked);
            this.appliedRules.addAll(xip.getAppliedRules(currentGHandler));
            this.appliedRules.add(0);
            //notify to xipx that the native XipResult has to be kepts for other methods as the generator
            xipx.fireKeepNativeXipResult();
        } else if (sizeOfUnit == (indexUnit - 1)) {
            //the next XipUnit will be the wanted XipUnit,
            // put the rule limit or the rank limit.
            if (this.isIndexLimit && ruleNumber != 0) {
                xip.setRuleRankNumberLimit(currentGHandler, this.ruleNumber - 1);
            } else {
                xip.setRuleLimit(this.currentGHandler, ruleNumber);
            }
        } else {
            // XipUnit that we don't want to debug need to be parsed with the full grammar.
            xip.setRuleLimit(this.currentGHandler, -1);
        }
        sizeOfUnit += 1; //increases the size of the XipUnit parsed
    }

    /**
     *This method build the sentence from a XipUnit
     *@param unit the Syntactic analysis of a sentence
     *@return the sentence producing the given XipUnit.
     *        NB : this method doesn't return the real source of the sentence but only a concatenation of
     *             leaves from the XipUnit.
     */
    private String buildSentence(XipUnit unit) {
        Collection<XipLeaf> leaves = unit.getLeaves();
        StringBuilder sentence = new StringBuilder();
        long currentLeft = -1;
        long currentRight = -1;
        for (Iterator<XipLeaf> it = leaves.iterator(); it.hasNext();) {
            XipLeaf leaf = it.next();
            if (!leaf.isFicticious()) {
                long left = leaf.getLeft();
                long right = leaf.getRight();
                if ((left != currentLeft) && (right != currentRight)) {
                    sentence.append(leaf.getSurface());
                    sentence.append(" ");
                }
                currentLeft = left;
                currentRight = right;
            }
        }
        return sentence.toString();
    }

    /**
     *This method is a callback, it's used internally by the Xip Object.
     *This method is called when a XML document is parsed, and forearch XML node according to the guideline,
     *this methode will be invoked.
     *Don't use directly
     *
     *@see Xip#callMethodOnXipUnitFromXMLFile(int ,file,Object,String,String,String)
     *
     */
    public void doOnXmlTag() {
    }

    /**
     *Set the current CorpusElement to parse
     *@param el the CorpusElement to parse
     */
    private void setCurrentCorpusElemnt(CorpusElement el) {
        this.current = el;
    }

    /**
     *Prepare this object to debug a CorpusElement. Foreach XipUnit created by the xip parser, this method call
     *the preProccess() method and the doOnXmlTag() if element is an XML file.
     *
     *@param gHandler 
     * @param element the CorpusElement to parse
     *
     *@throws java.lang.Exception 
     * @see preProcess()
     *@see doOnXmlTag()
     */
    public void prepareDebug(int gHandler, CorpusElement element) throws Exception {
        this.currentGHandler = gHandler;
        this.sizeOfUnit = 0;
        setCurrentCorpusElemnt(element);
        File file = element.getFile();
        if (element.isXML()) {
            xipx.callMethodOnXipUnitFromXMLFile(gHandler, file, this, "preProcess", "doOnXmlTag", element.getEncoding());
        } else {
            xipx.callMethodOnXipUnitFromFile(gHandler, file, this, "preProcess", element.getEncoding());
        }
        setCurrentCorpusElemnt(null);
    }

    /**
     *Debug the given DebugElement, get the Result with the getResultBreaked() method.
     *No check is done for the ruleNumber parameter. This method parses each XipUnit until the indexUnit of the given DebugElement.
     *For the wanted index, the method puts the rule  or index rule limit, it depends of the cases.
     *
     *@param gHandler the grammar handler
     *@param d the DebugElement to get the result
     *@param isIndexLimit determinates if the given limit is a rule number or a rule index in an applyed rules list.
     *@param ruleNumber 
     * @throws java.lang.Exception 
     * @see getResultBreaked()
     *@see getAppliedRules()
     *@see debugUnit()
     */
    public void debug(int gHandler, DebugElement d, boolean isIndexLimit, int ruleNumber) throws Exception {
        this.currentGHandler = gHandler;
        this.indexUnit = d.getUnitIndex();
        this.isIndexLimit = isIndexLimit;
        this.ruleNumber = ruleNumber;
        xip.setMaxXipUnit(gHandler, indexUnit);
        CorpusElement ce = d.getCorpusElement();
        File file = ce.getFile();
        /*
        To debug the first XipUnit of a document.  The first XipUnit has the index equal to 0.
        We need to put the rule limit or the index rule limit before the parsing of this XipUnit.
        In a callback Xip has already parsed the XipUnit, so it's too late to put a limit for the first XipUnit in the
        callBack itself. For the other XipUnit, the limits are set in the callBack
         */
        if (indexUnit == 0) {
            if (isIndexLimit && ruleNumber != 0) {
                xip.setRuleRankNumberLimit(currentGHandler, this.ruleNumber - 1);
            } else {
                xip.setRuleLimit(this.currentGHandler, ruleNumber);
            }
        }
        if (ce.isXML()) {
            xipx.callMethodOnXipUnitFromXMLFile(gHandler, file, this, "debugUnit", "doOnXmlTag", d.getEncoding());
        } else {
            xipx.callMethodOnXipUnitFromFile(gHandler, file, this, "debugUnit", d.getEncoding());
        }
        xip.setMaxXipUnit(gHandler, -1);
    }
}
