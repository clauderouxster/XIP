/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.io.*;
import java.text.DateFormat;
import java.util.*;

/**
 * The purpose of this class is to build a Xip rule
 *
 * @author grondeau @created May 19, 2005
 */
public class RuleManager {

    private Hashtable<Long, Rule> ruleBuffer;
    private int gramHandler;
    private String inputStr;
    private XipResult xipResult;
    private XipResultManager xipResultManager;
    private Rule rule;
    private RuleBuilder ruleBuilder;

    /**
     * Constructor
     */
    public RuleManager() {
        ruleBuffer = new Hashtable<Long, Rule>();
        gramHandler = -1;
        inputStr = "";
        xipResult = null;
        xipResultManager = null;
        rule = null;
        ruleBuilder = null;
    }

    /**
     * Sets the grammarHandler
     *
     * @param handler The new grammarHandler value
     */
    public synchronized void setGrammarHandler(int handler) {
        gramHandler = handler;
    }

    /**
     * Gets the grammarHandler
     *
     * @return The grammarHandler value
     */
    public synchronized int getGrammarHandler() {
        return gramHandler;
    }

    /**
     * Sets the inputStr
     *
     * @param inStr The new inputStr value
     */
    public synchronized void setInputStr(String inStr) {
        inputStr = inStr;
    }

    /**
     * Gets the inputStr
     *
     * @return The inputStr value
     */
    public synchronized String getInputStr() {
        return inputStr;
    }

    /**
     * build a XipResult
     *
     * @return build status
     */
    public synchronized boolean buildXipResult() {
        xipResultManager = new XipResultManager();
        if (gramHandler == -1) {
            System.out.println("RuleManager::buildXipResult() gramHandler not initialized");
            return false;
        }
        int status = -1;
        try {
            status = xipResultManager.buildXipResult(gramHandler, inputStr);
        } catch (Exception ex) {
            System.out.println("RuleManager::buildXipResult() error : " + ex.toString());
            return false;
        }
        if (status == 0) {
            xipResult = xipResultManager.getXipResult();
            return true;
        } else {
            return false;
        }
    }

    /**
     * Gets the xipResult of the previous buildXipResult() call
     *
     * @return The xipResult
     */
    public synchronized XipResult getXipResult() {
        return xipResult;
    }

    /**
     * Generate a xip rule
     *
     * @param selectedObjects Vector of SelectedObject
     * @param focus can be null, or Vector of Long (index)
     * @param ruleType the type of rule
     * @param boundaryIndex
     * @param category
     * @param dependencyArity
     * @return the rule
     */
    public synchronized boolean buildRule(Vector<SelectedObject> selectedObjects,
            Vector<Long> focus, long boundaryIndex, String category, int ruleType, int dependencyArity) {
        try {
            ruleBuilder = new RuleBuilder();
            String ruleString = ruleBuilder.buildRule(selectedObjects, focus,
                    boundaryIndex, category, ruleType, dependencyArity);
            rule = new Rule(ruleString, inputStr, ruleBuilder.getCategoryDependencies());
            return true;
        } catch (Exception ex) {
            System.out.println("RuleManager::buildRule() error : " + ex.toString());
            return false;
        }
    }

    /**
     * Gets the Rule object
     *
     * @return The Rule Object
     */
    public synchronized Rule getRule() {
        return rule;
    }

    /**
     * Adds the Rule (just build) in the ruleBuffer
     */
    public synchronized void addRuleInBuffer() {
        //System.out.println("RuleManager::addRuleInBuffer() rule ID = "
        // + rule.getIdentifier() + "\n" + rule.getRule() + "\n\n");
        ruleBuffer.put(rule.getIdentifier(), rule);
    }

    /**
     * Adds a Rule in the ruleBuffer
     *
     * @param rul The Rule
     */
    public synchronized void addRuleInBuffer(Rule rul) {
        ruleBuffer.put(rul.getIdentifier(), rul);
    }

    /**
     * Removes a rule from the ruleBuffer
     *
     * @param identifier
     */
    public synchronized void removeRuleFromBuffer(Long identifier) {
        ruleBuffer.remove(identifier);
    }

    /**
     * Gets the rule buffer (Hashtable key=rule.identifier, value= Rule object)
     *
     * @return The rule buffer
     */
    public synchronized Hashtable<Long, Rule> getRuleBuffer() {
        return ruleBuffer;
    }

    /**
     *
     *
     * @param out
     * @exception Exception
     */
    private void writeHeader(BufferedWriter out) throws Exception {
        String str = "// Xerox Research Centre Europe - Grenoble Laboratory\n//\n";
        out.write(str, 0, str.length());
        Calendar calendar = Calendar.getInstance();
        String year = String.valueOf(calendar.get(Calendar.YEAR));
        str = "// Copyright (c) " + year + " Xerox Corporation.  All Rights Reserved\n//\n";
        out.write(str, 0, str.length());
        String date = DateFormat.getDateTimeInstance().format(new Date());
        str = "// Date\t\t: " + date + " \n";
        out.write(str, 0, str.length());
        String author = System.getProperty("user.name");
        str = "// Author\t: " + author + "\n\n";
        out.write(str, 0, str.length());
    }

    /**
     * Saves the ruleBuffer in a file
     *
     * @param ruleFileName
     * @param categoryDepFileName
     * @exception Exception
     */
    public synchronized void saveRuleBufferInFile(String ruleFileName,
            String categoryDepFileName) throws Exception {
        //System.out.println("RuleManager::saveRuleBufferInFile() ruleFileName = " + ruleFileName);
        BufferedWriter outRule = new BufferedWriter(new FileWriter(ruleFileName));
        writeHeader(outRule);
        Vector<String> categoryDependencies = new Vector<String>();
        for (Enumeration enumeration1 = ruleBuffer.elements(); enumeration1.hasMoreElements();) {
            Rule rule1 = (Rule) enumeration1.nextElement();
            String source = "\n//" + rule1.getSource() + "\n";
            outRule.write(source, 0, source.length());
            //System.out.println("RuleManager::saveRuleBufferInFile() rule = \n" + rule1.getRule() + "\n\n");
            String ruleStr = rule1.getRule() + "\n";
            outRule.write(ruleStr, 0, ruleStr.length());
            for (Enumeration enumeration2 = (rule1.getCategoryDependencies()).elements();
                    enumeration2.hasMoreElements();) {
                String categoryDependency = (String) enumeration2.nextElement();
                // each categoryDependency must be inserted once
                if (!categoryDependencies.contains(categoryDependency)) {
                    categoryDependencies.add(categoryDependency);
                }
            }
        }
        outRule.close();
        // generate categoryDependency
        File fileTmp = File.createTempFile("xip", "tmp");
        String tmpFile = fileTmp.getPath();
        //System.out.println("RuleManager::saveRuleBufferInFile() tmpFile = " + tmpFile);
        ruleBuilder.generateRuleCategory(gramHandler, tmpFile, categoryDependencies);
        // add header
        BufferedReader in = new BufferedReader(new FileReader(tmpFile));
        BufferedWriter out = new BufferedWriter(new FileWriter(categoryDepFileName));
        writeHeader(out);
        //now copy tmpFile in categoryDepFileName
        String line;
        while ((line = in.readLine()) != null) {
            line += "\n";
            out.write(line);
        }
        //delete tmpFile
        fileTmp.delete();
        out.close();
        in.close();
    }

    /**
     * Reloads the ruleBufferFrom afile
     *
     * @param fileName
     */
    public synchronized void reloadRuleBufferFromFile(String fileName) {
    }
    //______________________________ test -----------------------------

    /**
     * Select a set of xipNode
     *
     * @param node
     * @param selectedObjects
     */
    private void selectXipNodes(XipNode node, Vector<SelectedObject> selectedObjects) {
        SelectedObject selectedObject = new SelectedObject(node);
        selectedObject.setAny(false);
        // select all the features of this node
        for (Enumeration enumeration = (node.getFeatures()).elements(); enumeration.hasMoreElements();) {
            XipFeature xipFeature = (XipFeature) enumeration.nextElement();
            selectedObject.addSelectedFeature(xipFeature);
        }
        selectedObjects.add(selectedObject);
        // process recursively the daughters
        for (Enumeration enumeration = (node.getDaughters()).elements(); enumeration.hasMoreElements();) {
            XipNode daughter = (XipNode) enumeration.nextElement();
            selectXipNodes(daughter, selectedObjects);
        }
    }

    /**
     * Test
     *
     * @param args
     */
    public static void main(String[] args) {
        if (args.length != 2) {
            System.out.println("Two parameters are required: the fullpath "
                    + "of the grm file, and the string to parse");
            System.exit(-1);
        }
        System.out.println("RuleBuilder::main() Loading grammar file: " + args[0]);
        System.out.println("RuleBuilder::main() parsing the string = " + args[1]);
        JXip jxip;
        int gHandler;
        try {
            // create a JXIP object to load the dynamic library in the VM
            jxip = new JXip();
            gHandler = jxip.XipGrmFile(args[0], 0, true);

            RuleManager ruleManager = new RuleManager();
            ruleManager.setGrammarHandler(gHandler);
            ruleManager.setInputStr(args[1]);
            if (ruleManager.buildXipResult()) {
                XipResult xipRes = ruleManager.getXipResult();
                //----------------------------- build the selectedObjects -----------------------------------
                Vector<SelectedObject> selectedObjects = new Vector<SelectedObject>();
                // build a vector of selected object
                for (Enumeration enumeration = (xipRes.getXipUnits()).elements();
                        enumeration.hasMoreElements();) {
                    XipUnit xipUnit = (XipUnit) enumeration.nextElement();
                    XipNode rootNode = xipUnit.getRoot();
                    // select the xipNodes and xipLeaves with all their features
                    ruleManager.selectXipNodes(rootNode, selectedObjects);
                    // select the dependencies with all their features
                    for (Enumeration enumeration1 = (xipUnit.getDependencies()).elements();
                            enumeration1.hasMoreElements();) {
                        XipDependency dependency = (XipDependency) enumeration1.nextElement();
                        SelectedObject selectedObject = new SelectedObject(dependency);
                        // select all the features of this dependency
                        for (Enumeration enumeration2 = (dependency.getFeatures()).elements();
                                enumeration2.hasMoreElements();) {
                            XipFeature xipFeature = (XipFeature) enumeration2.nextElement();
                            selectedObject.addSelectedFeature(xipFeature);
                        }
                        selectedObjects.add(selectedObject);
                    }
                }

                Vector<Long> focus = new Vector<Long>();
                focus.add(new Long(2));
                focus.add(new Long(5));

                int ruleType;
                ruleType = Const.DEPENDENCYRULE;
                //ruleType = Const.SEQUENCERULE;
                //ruleType = Const.LEFTCONTEXT;
                //ruleType = Const.RIGHTCONTEXT;
                //ruleType = Const.IDRULE;
                //ruleType = Const.TAGGINGRULE;
                long boundaryIndex = 3;
                int dependencyArity = 2;
                String category = "focus1";

                //-------------------------------Now the selectedObject is build ----------------------------
                if (ruleManager.buildRule(selectedObjects, focus, boundaryIndex, category,
                        ruleType, dependencyArity)) {
                    Rule rule = ruleManager.getRule();
                    //System.out.println("RuleManager::main() rule = \n" + rule.toString());
                } else {
                    System.out.println("RuleManager::main() buildRule error");
                    System.exit(-1);
                }
                ruleManager.addRuleInBuffer();

                //generates an other rule
                category = "focus2";
                if (ruleManager.buildRule(selectedObjects, focus, boundaryIndex, category,
                        ruleType, dependencyArity)) {
                    Rule rule = ruleManager.getRule();
                    //System.out.println("RuleManager::main() rule = \n" + rule.toString());
                } else {
                    System.out.println("RuleManager::main() buildRule error");
                    System.exit(-1);
                }
                ruleManager.addRuleInBuffer();

                //generates an other rule
                category = "focus3";
                if (ruleManager.buildRule(selectedObjects, focus, boundaryIndex, category,
                        ruleType, dependencyArity)) {
                    Rule rule = ruleManager.getRule();
                    //System.out.println("RuleManager::main() rule = \n" + rule.toString());
                } else {
                    System.out.println("RuleManager::main() buildRule error");
                    System.exit(-1);
                }
                ruleManager.addRuleInBuffer();

                //lists rule in buffer
                Hashtable<Long, Rule> rulBuf = ruleManager.getRuleBuffer();
                Rule rule1 = null;
                for (Enumeration enumeration4 = rulBuf.elements(); enumeration4.hasMoreElements();) {
                    rule1 = (Rule) enumeration4.nextElement();
                    System.out.println("RuleManager::main() rule ID = " + rule1.getIdentifier());
                }

                //removes the last rule
                ruleManager.removeRuleFromBuffer(rule1.getIdentifier());

                //generates rule file
                ruleManager.saveRuleBufferInFile("titi1", "titi2");
            } else {
                System.out.println("RuleManager::main() buildXipResult error");
            }
        } catch (Exception exception) {
            System.out.println("RuleManager::main() " + exception.toString());
            System.exit(-1);
        }
    }
}
