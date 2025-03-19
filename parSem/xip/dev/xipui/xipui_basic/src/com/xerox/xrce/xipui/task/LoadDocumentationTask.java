/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * LoadDocumentationTask.java
 *
 * Created on November 13, 2006, 3:36 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.task;

import com.xerox.xrce.xipui.util.Hyperactive;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.TaskModule;
import javax.swing.*;
import javax.swing.text.html.HTMLDocument;
import java.util.*;

/**
 *
 * @author vgrassau
 */
public class LoadDocumentationTask extends TaskModule<Void, Object> {

    private String resource;

    /** Creates a new instance of LoadDocumentationTask
     * @param context 
     */
    public LoadDocumentationTask(ModuleContext context) {
        super(context);
        resource = (String) getProperty("resource");
    }

    @Override
    protected void done() {
        log.info("documentation is loaded");
    }

    protected void process(java.util.List<Object> interims) {
        Iterator ite = interims.iterator();
        while (ite.hasNext()) {
            Object result = ite.next();
            if (result instanceof Exception) {
                severe("ERROR", (Exception) result);
                log.error(result);
            } else if (result instanceof JEditorPane) {
                JEditorPane editor = (JEditorPane) result;
                if (editor != null) {
                    JDialog dialog = new JDialog();
                    JScrollPane scroller = new JScrollPane(editor);
                    dialog.setAlwaysOnTop(false);
                    dialog.getContentPane().add(scroller, java.awt.BorderLayout.CENTER);
                    dialog.setSize(800, 600);
                    dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
                    dialog.setVisible(true);
                }
            }
        }
    }

    protected Void doInBackground() {
        JEditorPane htmlEdit = null;
        try {
            htmlEdit = new JEditorPane();
            htmlEdit.setContentType("text/html");
            htmlEdit.setEditable(false);
            HTMLDocument doc = (HTMLDocument) htmlEdit.getDocument();
            doc.setAsynchronousLoadPriority(-1);
            htmlEdit.addHyperlinkListener(new Hyperactive());
            htmlEdit.setPage(getClass().getResource(resource));
            publish(htmlEdit);
        } catch (Exception e) {
            log.debug("HELP HTML ERROR", e);
            publish(e);
            htmlEdit = null;
        }
        return null;
    }
}
