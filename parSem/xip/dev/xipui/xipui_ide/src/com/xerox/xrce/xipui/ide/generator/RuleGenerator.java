/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2006 Xerox Corporation                                                 *
 * Xerox Research Centre Europe - Grenoble                                              *
 * All Rights Reserved                                                                  *
 * Copyright protection claimed includes all forms and matters of copyrightable         *
 * material and information now allowed by statutory or judicial law or                 *
 * hereinafter granted, including without limitation, material generated from           *
 * the software programs which are displayed on the screen such as icons,               *
 * screen display looks, etc. All rights reserved. No part of this document             *
 * may be reproduced, stored in a retrieval system, or transmitted, in any              *
 * form or by any means, electronic, mechanical, photocopied, recorded, or              *
 * otherwise, without prior written consent from XEROX corporation.                     *
 ****************************************************************************************
 */
package com.xerox.xrce.xipui.ide.generator;

import com.xerox.xrce.xipui.ide.generator.ui.GeneralPanel;
import com.xerox.xrce.xipui.modules.ModuleContext;
import java.util.*;
import com.xerox.jxip.*;

/**
 *
 * @author csaillet
 */
public class RuleGenerator extends GeneralPanel {

    /** Creates a new instance of RuleGenerator
     * @param context
     * @param id 
     */
    public RuleGenerator(ModuleContext context, int id) {
        super(context, id);
    }

    public void initPanel() {
    }

    public void clear() {
    }

    public void back() {
    }

    public void next() {
    }

    public void close() {
    }

    public void generateRule() {
    }

    public void indexWord() {
    }

    public void lonMatch() {
    }

    /**
     *
     * @param vect
     * @return 
     */
    public String constructRules(Vector vect) {
        String ruleResult = new String();
        try {
            CommonMethod cmethod = new CommonMethod();
            RuleBuilder ruleBuilder = new RuleBuilder();
            String ruleString = new String();
            String ruleString1 = new String();
            String ruleString2 = new String();
            String ruleString3 = new String();
            int[] indexb = new int[1];
            String typeRule = (String) this.getProperty("choiceRule");
            String dependency = (String) this.getProperty("dependency");
            String arityStrg = (String) this.getProperty("arity");
            //si aritySelect n est pas un chiffre -> message d'erreur
            int arity = Integer.valueOf(arityStrg).intValue();
            Vector indexdependency = (Vector) this.getProperty("vectDependencySelect");
            Vector focus = (Vector) this.getProperty("vectFocusSelect");
            String category = (String) this.getProperty("category");
            Vector indexbody = (Vector) this.getProperty("vectBodySelect");
            Vector indexheadcat = (Vector) this.getProperty("vectHeadSelect");
            Vector indexkeepcat = (Vector) this.getProperty("vectKeepSelect");
            Vector indexleft = (Vector) this.getProperty("vectLeftSelect");
            Vector indexright = (Vector) this.getProperty("vectRightSelect");
            Vector indexmark = (Vector) this.getProperty("vectMarkSelect");
            String lgestmatch = (String) this.getProperty("longestMatch");
            String indlastwrd = (String) this.getProperty("indexlastword");
            if (indexdependency.size() != 0) { // construction des regles de dependences

                ruleString = ruleBuilder.buildRule(indexdependency, focus, -1, dependency, Const.DEPENDENCYRULE, arity);
            } else if (indexmark.size() != 0) { // construction des regles de marking

                ruleString = ruleBuilder.buildRule(indexmark, null, -1, "", Const.SEQUENCERULE, 1);
                DEBUG_LOG.trace("rule = " + ruleString);
            } else if (indexbody.size() != 0) { // construction des regles de sequences,DI et term

                int minBody = (int) cmethod.findMinSelObject(indexbody);
                long leftBody = (long) minBody - 1;
                long rightBody = (long) minBody + 1;
                if (typeRule.equals("ID Rule")) {
                    ruleString = ruleBuilder.buildRule(indexbody, null, -1, "", Const.IDRULE, 1);
                } else {
                    ruleString = ruleBuilder.buildRule(indexbody, null, -1, "", Const.SEQUENCERULE, 1);
                    DEBUG_LOG.trace("ruleString body = " + ruleString);
                }
                if (indexleft.size() != 0) {
                    ruleString2 = ruleBuilder.buildRule(indexleft, null, leftBody, "", Const.LEFTCONTEXT, 1);
                    DEBUG_LOG.trace("ruleString left = " + ruleString2);
                }
                if (indexright.size() != 0) {
                    ruleString3 = ruleBuilder.buildRule(indexright, null, rightBody, "", Const.RIGHTCONTEXT, 1);
                    DEBUG_LOG.trace("ruleString right = " + ruleString3);
                }
                indexb[0] = minBody;
            } else if (indexheadcat.size() != 0) {  // construction des regles de tagging

                long leftHead = (cmethod.findMinSelObject(indexheadcat)) - 1;
                long rightHead = (cmethod.findMaxSelObject(indexheadcat)) + 1;
                ruleString = ruleBuilder.buildRule(indexheadcat, null, -1, "", Const.TAGGINGRULE, 1);
                if (indexkeepcat.size() != 0) {
                    ruleString1 = ruleBuilder.buildRule(indexkeepcat, null, -1, "", Const.TAGGINGRULE, 1);
                    DEBUG_LOG.trace("ruleString keepCategories = " + ruleString1);
                }
                if (indexleft.size() != 0) {
                    ruleString2 = ruleBuilder.buildRule(indexleft, null, leftHead, "", Const.LEFTCONTEXT, 1);
                    DEBUG_LOG.trace("ruleString left = " + ruleString2);
                }
                if (indexright.size() != 0) {
                    ruleString3 = ruleBuilder.buildRule(indexright, null, rightHead, "", Const.RIGHTCONTEXT, 1);
                    DEBUG_LOG.trace("ruleString right = " + ruleString3);
                }
            } else {
                DEBUG_LOG.trace("error generation rule");
            }
            String lemma = new String();
            if (typeRule.equals("Term")) {
                // recuperer le lemme du premier indice du body
                int minBody = (int) cmethod.findMinSelObject(indexbody);
                XipNode node = CommonMethod.xipNodeFromVector(indexbody, minBody);
                DEBUG_LOG.trace("node " + node.getCategory());
                if (CommonMethod.herDaughterIsLeaf(node)) {
                    Vector leaf = node.getDaughters();
                    lemma = ((XipLeaf) leaf.firstElement()).getLemma();
                } else {
                    DEBUG_LOG.trace("error lemma");
                }
            } else {
                lemma = "";
            }
            //concatenation et affichage de la regle
            String ar = "-> ";
            String dot = ".";
            String equ = "= ";
            String lgmatch = " @";
            String qt = "\"" + lemma + "\"" + " : ";
            String indexWord = "<";
            if (typeRule.equals("ID Rule")) {
                ruleResult = category + ar + ruleString2 + ruleString + ruleString3 + dot;
            } else if (typeRule.equals("Sequence")) {
                if (lgestmatch.equals("true")) {
                    ruleResult = category + lgmatch + equ + ruleString2 + ruleString + ruleString3 + dot;
                } else {
                    ruleResult = category + equ + ruleString2 + ruleString + ruleString3 + dot;
                }
            } else if (typeRule.equals("Term")) {
                if (lgestmatch.equals("true") && indlastwrd.equals("false")) {
                    ruleResult = qt + category + lgmatch + equ + ruleString2 + ruleString + ruleString3 + dot;
                } else if (indlastwrd.equals("true") && lgestmatch.equals("false")) {
                    ruleResult = qt + category + indexWord + equ + ruleString2 + ruleString + ruleString3 + dot;
                } else if (lgestmatch.equals("true") && indlastwrd.equals("true")) {
                    ruleResult = qt + category + lgmatch + indexWord + equ + ruleString2 + ruleString + ruleString3 + dot;
                } else {
                    ruleResult = qt + category + equ + ruleString2 + ruleString + ruleString3 + dot;
                }
            } else if (typeRule.equals("Tagging")) {
                ruleResult = ruleString + equ + ruleString2 + ruleString1 + ruleString3 + dot;
            } else if (typeRule.equals("Dependency")) {
                ruleResult = ruleString;
            } else {
                ruleResult = ruleString + dot;
            }
        } catch (Exception e) {
            DEBUG_LOG.trace(e);
//            e.printStackTrace();
            warning("Wrong selection!");
            ruleResult = "problem";
        }
        this.setProperty("ruleResult", ruleResult);
        return ruleResult;
    }
}
