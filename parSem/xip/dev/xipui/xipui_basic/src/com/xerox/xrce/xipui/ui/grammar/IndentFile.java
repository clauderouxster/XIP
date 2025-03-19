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

import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import com.xerox.xrce.xipui.Grammar;
import com.xerox.xrce.xipui.ProjectContext;
import com.xerox.xrce.xipui.VarEnv;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.editor.DefaultEditor;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.util.FileTools;
import javax.swing.*;
import java.io.*;

/**
 *
 * @author vgrassau
 */
public class IndentFile extends DefaultEditor {

    /** Creates a new instance of IndentFile
     * @param context 
     */
    public IndentFile(ModuleContext context) {
        super(context, FileTools.ENCODING_DEFAULT, false);
        setClosable(true);
        setSavable(false);
        setEditable(false);
        JTabbedPane outputTabbed = (JTabbedPane) getProperty(OUTPUT_TABBED);
        setRootComponent(outputTabbed);
    //info("Indent file module was correctly loaded");
    }

    public void close() {
        super.close();
        if (getRootComponent() != null) {
            getRootComponent().remove(this);
        }
    }

    public void readIndent(String encoding) throws Exception {
        Grammar grammar = getProject().getGrammar();
        String indentPath = grammar.getIndentPath();
        if (indentPath == null) {
            File indentInSetup = new File(grammar.getGrammarInformation(Grammar.INDENT).toString());
            //we get the working directory
            VarEnv path = getProject().getProjectContext().getVariable(ProjectContext.working_directory);
            indentPath = path.getValue().replace('\\', '/') + "/" + indentInSetup.getName();
        }
        File indent = new File(indentPath);
        if (encoding != null) {
            setEncoding(encoding);
        }
        setFile(indent);
        if (getFile().exists()) {
            editFile();
        }
        JTabbedPane outputTabbed = (JTabbedPane) getProperty(OUTPUT_TABBED);
        XIPUIUtilities.addComponentOnTab(outputTabbed, this, XIPUIUtilities.getIcon(UIConstants.INDENT), "Indent file", getFile().getAbsolutePath());
    }

    /**
     *This method closes the stream with the indent file managed by xip.
     *When the next parsing will be executed, the indent file will be empty.
     *It's usefull otherwise we read all the parsing and increase the memory quickly.
     */
    public void closeIndentFileStream() {
        try {
            Xip xip = getProject().getXip();
            xip.setIndentFile(getProject().getGhandler(), false);
            xip.setIndentFile(getProject().getGhandler(), true);
        } catch (Exception e) {
            write("Can't to close the stream on the indent file");
        }
    }
}
