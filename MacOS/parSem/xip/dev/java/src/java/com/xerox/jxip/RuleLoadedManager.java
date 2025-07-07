/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.util.Hashtable;

/**
 * The purpose of this class is to manage all the RuleLoaded objects
 *
 * @author grondeau @created April 15, 2005
 */
public class RuleLoadedManager {

    private Hashtable<Integer, RuleLoaded> rulesByRuleNb;
    private int gHandler;

    /**
     * Constructor for the RuleLoadedManager object
     */
    public RuleLoadedManager() {
    }

    /**
     *
     *
     * @param gHand
     */
    public synchronized void init(int gHand) {
        gHandler = gHand;
        rulesByRuleNb = new Hashtable<Integer, RuleLoaded>();
        getRulesLoadedImplementation(gHandler);
    }

    /**
     * Adds a Rule
     *
     * @param rule The rule
     */
    private synchronized void addRule(RuleLoaded rule) {
        rulesByRuleNb.put(new Integer(rule.getRuleNb()), rule);
    }

    /**
     * Gets a rule by its ruleNb
     *
     * @param ruleNb
     * @return The rulebyRuleNb value
     */
    public synchronized RuleLoaded getRulebyRuleNb(int ruleNb) {
        return rulesByRuleNb.get(new Integer(ruleNb));
    }

    /**
     * Gets the ruleCounter attribute of the RuleLoadedManager object
     *
     * @return The ruleCounter value
     */
    public synchronized int getRuleCounter() {
        return rulesByRuleNb.size();
    }

    /**
     *
     *
     * @return
     */
    public synchronized Hashtable<Integer, RuleLoaded> getRulesByRuleNb() {
        return rulesByRuleNb;
    }

    /**
     * print all rules
     */
    public synchronized void print() {
        System.out.println("ruleNb, ruleLoadedType, fileName, offset,"
                + " lineNb, absLayer, relLayer, label");
        for (int ruleNb1 = 0; ruleNb1 <= rulesByRuleNb.size() - 1; ruleNb1++) {
            RuleLoaded rule = rulesByRuleNb.get(new Integer(ruleNb1));
            System.out.println("----------------------------------------");
            System.out.println(rule.toString());
            boolean withComment = true;
            System.out.println(rule.getRuleSrc(withComment));
        }
    }

    //-------------------------- native method ----------------------
    private native boolean getRulesLoadedImplementation(int gHandler);

    //-------------------------- callBack method (from native C++ code: JXip.cxx)----------------------
    /**
     * Constructor for the RuleLoaded object
     *
     * This callback method is called by the C++ method
     * JXipImp2::getRulesLoadedImplementation()
     *
     * @param fileN
     * @param off
     * @param lineN
     * @param absL
     * @param relL
     * @param ruleN
     * @param lab
     * @param type
     * @param gHand
     * @param firstRule
     */
    private void RuleLoadedManagerCB(int type, String fileN, long off, int lineN,
            int absL, int relL, int ruleN, String lab, int gHand, int firstRule) {
        RuleLoaded rule = null;
        int ruleLoadedType = type;
        /*
         * pour utiliser xip sur le server tomcat externe il faut compiler avec
         * jdk1.4 => ne pas utilser les enum RuleLoaded.RuleType ruleLoadedType
         * = RuleLoaded.RuleType.REGLES_SEQUENCE; switch (type) { case 1:
         * ruleLoadedType = RuleLoaded.RuleType.REGLES_SEQUENCE; break; case 2:
         * ruleLoadedType = RuleLoaded.RuleType.REGLES_DI; break; case 3:
         * ruleLoadedType = RuleLoaded.RuleType.REGLES_SIMPLIFICATION; break;
         * case 4: ruleLoadedType = RuleLoaded.RuleType.REGLES_EXTRACTION;
         * break; case 5: ruleLoadedType = RuleLoaded.RuleType.REGLES_DEDUCTION;
         * break; case 6: ruleLoadedType =
         * RuleLoaded.RuleType.REGLES_SEMANTIQUE; break; case 7: ruleLoadedType
         * = RuleLoaded.RuleType.REGLES_MARQUAGE; break; case 8: ruleLoadedType
         * = RuleLoaded.RuleType.REGLES_TERME; break; case 9: ruleLoadedType =
         * RuleLoaded.RuleType.REGLES_RECONSTRUCTION; break; case 10:
         * ruleLoadedType = RuleLoaded.RuleType.REGLES_RECONSTRUCTION_MODELE;
         * break; case 11: ruleLoadedType =
         * RuleLoaded.RuleType.REGLES_DECOUPESEQUENCE; break; case 12:
         * ruleLoadedType = RuleLoaded.RuleType.REGLES_CARTESIEN; break; case
         * 13: ruleLoadedType = RuleLoaded.RuleType.REGLES_BOUCLE; break; case
         * 14: ruleLoadedType = RuleLoaded.RuleType.REGLES_FRAGMENTE; break;
         * case 15: ruleLoadedType = RuleLoaded.RuleType.REGLES_VOCABULAIRE;
         * break; case 16: ruleLoadedType = RuleLoaded.RuleType.REGLES_PL;
         * break; case 17: ruleLoadedType =
         * RuleLoaded.RuleType.REGLES_MULTIMOTS; break; default:
         * System.out.println("RuleLoadedManagerCB unknow rule type:" + type);
         * break; }
         */
        rule = new RuleLoaded(ruleLoadedType, fileN, off, lineN, absL, relL, ruleN,
                lab, gHand, firstRule);
        addRule(rule);
    }
    //---------------------------------------------------------------------------

    /**
     *
     *
     * @param args
     */
    public static void main(String[] args) {
        try {
            if (args.length != 2) {
                System.out.println("Two parameters are required: the fullpath of the "
                        + "grm file, and the string to parse");
                System.exit(-1);
            }
            System.out.println("JXip::main() Loading grammar file: " + args[0]);
            System.out.println("Xip::main() parsing the string = " + args[1]);
            // create a JXIP object to load the dynamic library in the VM
            JXip jxip = new JXip();
            int gHandler = jxip.XipGrmFile(args[0], 0, true);
            long displayMode = Const.DISPLAY_TREE;
            int col = 80;
            jxip.XipSetDisplayMode(gHandler, displayMode, col);
            String res0 = jxip.parseString(gHandler, args[1], Const.XML_NONE, (char) 1);
            System.out.println("\nXip::main() resultat after parsing = " + res0);
            RuleLoadedManager ruleLoadedManager = new RuleLoadedManager();
            ruleLoadedManager.init(gHandler);
            System.out.println("Xip::main() rule count = " + ruleLoadedManager.getRuleCounter());
            ruleLoadedManager.print();
        } catch (Exception ex) {
            System.exit(-1);
        }
    }
}
