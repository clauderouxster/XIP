/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.io.File;

/**
 * The purpose of this class is represent the rule information loaded from a
 * grammar file
 *
 * @author grondeau @created April 15, 2005
 */
public class RuleLoaded {

    //public static enum RuleType {REGLES_SEQUENCE, REGLES_DI, REGLES_SIMPLIFICATION,
    //REGLES_EXTRACTION, REGLES_DEDUCTION, REGLES_SEMANTIQUE,
    //REGLES_MARQUAGE, REGLES_TERME, REGLES_RECONSTRUCTION,REGLES_RECONSTRUCTION_MODELE,
    //REGLES_DECOUPESEQUENCE, REGLES_CARTESIEN,REGLES_BOUCLE, REGLES_FRAGMENTE,
    //REGLES_VOCABULAIRE, REGLES_PL, REGLES_MULTIMOTS };
    // the type of the rule
    //private RuleType ruleType;
    private int ruleType;
    // the file name where this rule is stored
    private File file;
    // the offset in the file of the source code of this rule
    private long offset;
    // the line number in the file of the source code of this rule
    private long lineNb;
    // the absolute layer
    private int absLayer;
    // the relative layer
    private int relLayer;
    // rule number
    private int ruleNb;
    // label name of this rule
    private String label;
    // grammar handler
    private int gHandler;
    // firstRule = 1 it's the first rule of the file, else = 0
    private int firstRule;

    /**
     * Constructor for the RuleLoaded object
     *
     * @param ruleT
     * @param fileN
     * @param off
     * @param lineN
     * @param absL
     * @param relL
     * @param ruleN
     * @param lab
     * @param gHand
     * @param firstRul
     */
    //public RuleLoaded(RuleType ruleT, String fileN, long off, long lineN, int absL, int relL);
    public RuleLoaded(int ruleT, String fileN, long off, long lineN,
            int absL, int relL, int ruleN, String lab, int gHand, int firstRul) {
        //System.out.println("RuleLoaded::RuleLoaded() " + fileN + " " + lineN + " " + ruleN);
        ruleType = ruleT;
        file = new File(fileN);
        offset = off;
        lineNb = lineN;
        absLayer = absL;
        relLayer = relL;
        ruleNb = ruleN;
        gHandler = gHand;
        firstRule = firstRul;
        if (lab.compareTo("#") == 0) {
            label = "";
        } else {
            label = lab;
        }
    }

    /**
     * Gets the file name where the rule is stored
     *
     * @return file name
     */
    public synchronized String getFileName() {
        return file.getName();
    }

    /**
     * Gets the ruleType attribute of the RuleLoaded object
     *
     * @return The ruleType value
     */
    public synchronized String getStringRuleType() {
        //return ruleType.toString();
        String ruleTypeStr;
        switch (ruleType) {
            case 1:
                //ruleTypeStr = "REGLES_SEQUENCE";
                ruleTypeStr = "SEQUENCE RULE";
                break;
            case 2:
                //ruleTypeStr = "REGLES_DI";
                ruleTypeStr = "ID RULE";
                break;
            case 3:
                //ruleTypeStr = "REGLES_SIMPLIFICATION";
                ruleTypeStr = "DISAMBIGUATION RULE";
                break;
            case 4:
                //ruleTypeStr = "REGLES_EXTRACTION";
                ruleTypeStr = "EXTRACTION RULE";
                break;
            case 5:
                //ruleTypeStr = "REGLES_DEDUCTION";
                ruleTypeStr = "DEDUCTION RULE";
                break;
            case 6:
                //ruleTypeStr = "REGLES_SEMANTIQUE";
                ruleTypeStr = "SEMANTIC RULE";
                break;
            case 7:
                //ruleTypeStr = "REGLES_MARQUAGE";
                ruleTypeStr = "MARKING RULE";
                break;
            case 8:
                //ruleTypeStr = "REGLES_TERME";
                ruleTypeStr = "TERM RULE";
                break;
            case 9:
                //ruleTypeStr = "REGLES_RECONSTRUCTION";
                ruleTypeStr = "RESHUFFLING RULE";
                break;
            case 10:
                //ruleTypeStr = "REGLES_RECONSTRUCTION_MODELE";
                ruleTypeStr = "RESHUFFLING MODEL RULE";
                break;
            case 11:
                //ruleTypeStr = "REGLES_DECOUPESEQUENCE";
                ruleTypeStr = "CUT RULE";
                break;
            case 12:
                //ruleTypeStr = "REGLES_CARTESIEN";
                ruleTypeStr = "DOUBLE-REDUCTION RULE";
                break;
            case 13:
                //ruleTypeStr = "REGLES_BOUCLE";
                ruleTypeStr = "LOOP RULE";
                break;
            case 14:
                //ruleTypeStr = "REGLES_FRAGMENTE";
                ruleTypeStr = "SPLITING RULE";
                break;
            case 15:
                //ruleTypeStr = "REGLES_VOCABULAIRE";
                ruleTypeStr = "LEXICON RULE";
                break;
            case 16:
                //ruleTypeStr = "REGLES_PL";
                ruleTypeStr = "LP RULE";
                break;
            case 17:
                //ruleTypeStr = "REGLES_MULTIMOTS";
                ruleTypeStr = "MULTI_WORDS";
                break;
            default:
                ruleTypeStr = "unknow rule";
                break;
        }
        return ruleTypeStr;
    }

    /**
     * Gets the ruleType attribute of the RuleLoaded object
     *
     * @return The ruleType value
     */
    //public synchronized RuleType getRuleType() {
    //	return ruleType;
    //}
    public synchronized int getRuleType() {
        return ruleType;
    }

    /**
     * Gets the file attribute of the RuleLoaded object
     *
     * @return The file value
     */
    public synchronized File getFile() {
        return file;
    }

    /**
     * Gets the rule1Src attribute of the RuleLoaded object
     *
     * @param withComment
     * @return The ruleSrc value
     */
    public synchronized String getRuleSrc(boolean withComment) {
        /*
         * String comment = ""; int indexCom = 0; if (withComment) { if
         * (firstRule == 0) { / ce n'est pas la premiere rule du fichier String
         * str = XipDisplayActivatedRulesImplementation(gHandler, ruleNb - 1);
         * indexCom = str.indexOf("/");
         * /System.out.println("RuleLoaded::getRuleSrc() " + str + " " +
         * indexCom); if (indexCom != -1) { if (indexCom == 0) { comment =
         * str.substring(indexCom); } else { if (str.charAt(indexCom - 1) !=
         * '"') { if (str.charAt(indexCom - 1) != '\'') { comment =
         * str.substring(indexCom); } } } } } else { /c'est la premiere rule du
         * fichier => on ne peut pas recuperer le commentaire de /cette regle en
         * utilisant la regle precedante qui est dans un fichier different } }
         */
        String src = XipDisplayActivatedRulesImplementation(gHandler, ruleNb);
        /*
         *  / find the comment for the next rule and remove it int indexCom =
         * src.indexOf("//"); if (indexCom != -1) { src = src.substring(0,
         * indexCom); } / remove other comment indexCom = src.indexOf("/"); if
         * (indexCom != -1) { if (src.charAt(indexCom - 1) != '"') { if
         * (src.charAt(indexCom - 1) != '\'') { src = src.substring(0,
         * indexCom); } } } return comment + src;
         */
        return src;
    }

    /**
     * Gets the offset attribute of the RuleLoaded object
     *
     * @return The offset value
     */
    public synchronized long getOffset() {
        return offset;
    }

    /**
     * Gets the lineNb attribute of the RuleLoaded object
     *
     * @return The lineNb value
     */
    public synchronized long getLineNb() {
        return lineNb;
    }

    /**
     * Gets the absLayer attribute of the RuleLoaded object
     *
     * @return The absLayer value
     */
    public synchronized int getAbsLayer() {
        return absLayer;
    }

    /**
     * Gets the relLayer attribute of the RuleLoaded object
     *
     * @return The relLayer value
     */
    public synchronized int getRelLayer() {
        return relLayer;
    }

    /**
     * Gets the ruleNb attribute of the RuleLoaded object
     *
     * @return The ruleNb value
     */
    public synchronized int getRuleNb() {
        return ruleNb;
    }

    /**
     * Gets the label attribute of the RuleLoaded object
     *
     * @return The label value
     */
    public synchronized String getLabel() {
        return label;
    }

    /**
     *
     *
     * @return
     */
    @Override
    public synchronized String toString() {
        String str = "";
        str += " " + ruleNb;
        //str += " " + ruleType.toString();
        str += " " + getStringRuleType();
        str += " " + file.getName();
        //str += " " + offset;
        //str += " " + lineNb;
        str += " " + absLayer;
        str += " " + relLayer;
        str += " " + label;
        return str;
    }
    //-------------------------- native method ----------------------

    private native String XipDisplayActivatedRulesImplementation(int gHandler, int ruleNb);
}
