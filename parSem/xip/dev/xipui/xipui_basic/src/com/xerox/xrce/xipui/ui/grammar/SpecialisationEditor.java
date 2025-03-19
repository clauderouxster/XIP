/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2005 Xerox Corporation                                                 *
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
package com.xerox.xrce.xipui.ui.grammar;

import com.xerox.jxip.*;

import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.*;

import com.xerox.xrce.xipui.ui.editor.*;
import java.util.*;
import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.*;
import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class SpecialisationEditor extends DefaultEditor {

    // regles de la grammaire
    private Vector rules;
    private AbstractAction comment;
    private AbstractAction unComment;
    private RenumberingAction renumber;

    /** Creates a new instance of SpecialisationEditor
     * @param properties
     * @param encoding_ 
     */
    public SpecialisationEditor(ModuleContext properties, String encoding_) {
        super(properties, encoding_, false);
        this.rules = new Vector();
        comment = new AbstractAction("comment", XIPUIUtilities.getIcon(UIConstants.COMMENT)) {

            public void actionPerformed(ActionEvent evt) {
                commentActionPerformed(evt);
            }
        };
        unComment = new AbstractAction("uncomment", XIPUIUtilities.getIcon(UIConstants.UNCOMMENT)) {

            public void actionPerformed(ActionEvent evt) {
                unCommentActionPerformed(evt);
            }
        };
        this.toolBar.addSeparator();
        this.toolBar.add(XIPUIUtilities.createJButton("", comment, false));
        this.toolBar.add(XIPUIUtilities.createJButton("", unComment, false));
        renumber = new RenumberingAction(properties);
        this.toolBar.add(XIPUIUtilities.createJButton("", renumber, false));
    }

    /** Creates a new instance of SpecialisationEditor
     * @param properties
     * @param encoding_
     * @param rules 
     */
    public SpecialisationEditor(ModuleContext properties, String encoding_, Vector rules) {
        this(properties, encoding_);
        setRules(rules);
    }

    /**
     *Return the number of the rule at the given offset
     *If the rule was not found we return -1 
     *@param offset offset to found
     *@return the rule number or -1 if the rule was not found 
     */
    private int getRuleNumber(int offset) {
        boolean stop = false;
        int number_ = -1;
        for (int i = rules.size() - 1; i > 0 && !stop; i--) {
            RuleLoaded rule = (RuleLoaded) rules.get(i);
            RuleLoaded rulePrec = (RuleLoaded) rules.get(i - 1);
            int start = (int) rule.getOffset();
            int end = (int) rulePrec.getOffset();
            if (offset >= start && offset <= end) {
                stop = true;
                number_ = rule.getRuleNb();
            }
        }
        if (rules.size() > 0) {
            RuleLoaded lastRule = (RuleLoaded) rules.get(0);
            int startOfLastRule = (int) lastRule.getOffset();
            if (offset >= startOfLastRule) {
                number_ = lastRule.getRuleNb();
            }
        }
        return number_;
    }

    // evenment sur le caret Listener
    public void caretUpdate(CaretEvent e) {
        super.caretUpdate(e);
        if (this.rules != null) {
            int nRule = getRuleNumber(e.getDot()); ////////////// attttttttennnntiiiionnn

            if (nRule <= -1) {
                divers.setText("");
                renumber.setEnabled(false);
            } else {
                divers.setText("Rule number : " + (nRule));
                renumber.setToolTipText(nRule);
                renumber.putValue(RenumberingAction.EDITOR, this);
                renumber.putValue(RenumberingAction.RULE_NUMBER, nRule);
                renumber.setEnabled(true);
            }
        }
        repaint();
    }

    /** 
     * set the vector of RuleLoaded element for this xip file
     * the vector will be sorted by the offset, the first rule is the last rule written in the file.
     *@param rules_ the vector of RuleLoaded for the xip file
     **/
    public void setRules(Vector rules_) {
        rules = new Vector();
        Iterator ite = rules_.iterator();
        while (ite.hasNext()) {
            RuleLoaded rule = (RuleLoaded) ite.next();
            Long offset = new Long(rule.getOffset());
            int indexToInsert = 0;
            boolean stop = false;
            for (int i = 0; i < rules.size() && !stop; i++) {
                RuleLoaded rule2 = (RuleLoaded) rules.get(i);
                if (offset.compareTo(rule2.getOffset()) > 0) {
                    indexToInsert = i;
                    stop = true;
                } else {
                    indexToInsert = i + 1;
                }
            }
            rules.add(indexToInsert, rule);
        }//end of eLex enum



    }

    private void unCommentActionPerformed(java.awt.event.ActionEvent evt) {
        try {
            if (text.getSelectedText() == null) {
                int off = text.getCaretPosition();
                int sd = Utilities.getRowStart(text, off);
                int sf = Utilities.getRowEnd(text, off);
                String line = doc.getText(sd, sf - sd);
                if (line.startsWith("//")) {
                    doc.replace(sd, sf - sd, line.replaceFirst("//", ""), currentAttr);
                }
            } else {
                int begin = text.getSelectionStart();
                int end = text.getSelectionEnd();
                String subText = getText(begin, end);
                subText = subText.replaceAll("\n//", "\n");
                subText = subText.replaceFirst("^//", "");
                // DEBUG_LOG.trace(subText);
                doc.replace(begin, (end - begin), subText, currentAttr);
            }
        } catch (Exception e) {
            DEBUG_LOG.trace("Error", e);
        }
    }

    private void commentActionPerformed(java.awt.event.ActionEvent evt) {
        try {
            if (text.getSelectedText() == null) {
                int off = text.getCaretPosition();
                int s = Utilities.getRowStart(text, off);
                doc.insertString(s, "//", currentAttr);
            } else {
                int begin = text.getSelectionStart();
                int end = text.getSelectionEnd();

                String subText = getText(begin, end);
                subText = subText.replaceAll("\n", "\n//");
                //  DEBUG_LOG.trace(subText);
                doc.replace(begin, (end - begin), "//" + subText, currentAttr);
            }
        } catch (Exception e) {
            DEBUG_LOG.trace("Error", e);
        }

    }
}
