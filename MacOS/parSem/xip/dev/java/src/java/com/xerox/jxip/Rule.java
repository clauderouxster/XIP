/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.util.Date;
import java.util.Vector;

/**
 * The purpose of this class is to build a Xip rule
 *
 * @author grondeau @created April 15, 2005
 */
public class Rule {

    private Long identifier;
    private String rule;
    private String sourceLineUsedToGenerateRule;
    private Vector<String> categoryDependencies;

    /**
     * Constructor
     *
     * @param rul
     * @param source
     * @param categoryDeps
     * @throws java.lang.Exception
     */
    public Rule(String rul, String source, Vector<String> categoryDeps)
            throws Exception {
        rule = JXip.duplicateString(rul);
        sourceLineUsedToGenerateRule = JXip.duplicateString(source);
        categoryDependencies = categoryDeps;
        //sleep 10ms to be sure the rule.identifier of two new Rule() are different
        Thread.sleep(5);
        Date date = new Date();
        identifier = new Long(date.getTime());
        //System.out.println("Rule::Rule() rule = " + rule + " identifier = " + identifier);
    }

    /**
     * Gets the rule
     *
     * @return The rule string
     */
    public synchronized String getRule() {
        return rule;
    }

    /**
     * Gets the source line which has been parsed and used to generate the rule
     *
     * @return The source string
     */
    public synchronized String getSource() {
        return sourceLineUsedToGenerateRule;
    }

    /**
     * Gets the identifier attribute of the Rule object
     *
     * @return The identifier value
     */
    public synchronized Long getIdentifier() {
        return identifier;
    }

    /**
     *
     *
     * @return
     */
    public synchronized Vector<String> getCategoryDependencies() {
        return categoryDependencies;
    }

    /**
     * Gets the rule as a String
     *
     * @return Returns the rule as a String
     */
    @Override
    public synchronized String toString() {
        return rule;
    }
}
