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
package com.xerox.xrce.xipui.ide.grammar;

import com.xerox.xrce.xipui.GrmFile;
import com.xerox.xrce.xipui.ui.editor.Styles;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.editor.DefaultEditor;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class GrmEditor extends DefaultEditor {

    private JButton source;
    private JButton form;
    private boolean isForm = true;
    private GrmForm grmForm;

    /**
     * Creates a new instance of GrmEditor
     * @param properties 
     * @param encoding 
     */
    public GrmEditor(ModuleContext properties, String encoding) {
        super(properties, encoding, false);

//       Project p = getProject();
//       
//      
//       try {
//          
//           this.setFile(new File(p.getFullpathGrm()));
//           GrmFile grm = p.getGrmFile();
//           GrmFile grmEdited = new GrmFile(p.getXip(),p.getProjectContext());
//           grmEdited.setFile(getFile());
//         //  System.out.println(grm.write());
//           grmEdited.read(grm.write());
//           doc.remove(0,doc.getLength());
//           write(grmEdited.write(),Styles.STYLE_NORMAL);
//           setEditable(true);
//           properties.put("GRM_EDITED",grmEdited);
//       
//       } catch (Exception e) {
//           
//       }
//       grmForm = new GrmForm(getModuleContext());
//       
//       source = new JButton("Source");
//       source.setBorderPainted(false);
//       source.addActionListener(new ActionListener(){
//          public void actionPerformed(ActionEvent e) {
//              viewSource(e);
//          }
//       });
//       
//       form = new JButton("Form");
//       form.setBorderPainted(false);
//       form.addActionListener(new ActionListener(){
//          public void actionPerformed(ActionEvent e) {
//              viewForm(e);
//          }
//       });
//       
//        grmForm.initializeForm();
//        scroller.add(grmForm);
//        scroller.setViewportView(grmForm);
//        isForm = true;
//        initToolBar();

    }

    public boolean isForm() {
        return this.isForm;
    }

    private void initToolBar() {
        this.toolBar.add(form, 1);
        this.toolBar.add(source, 2);
        this.paste.setVisible(!isForm);
        this.copy.setVisible(!isForm);
        this.cut.setVisible(!isForm);
        this.undo.setVisible(!isForm);
        this.redo.setVisible(!isForm);

        if (isForm) {
            form.setBackground(Color.DARK_GRAY);
            form.setForeground(Color.WHITE);
            form.setOpaque(true);
            source.setOpaque(false);
            source.setForeground(Color.BLACK);
        } else {
            source.setBackground(Color.DARK_GRAY);
            source.setForeground(Color.WHITE);
            source.setOpaque(true);
            form.setOpaque(false);
            form.setForeground(Color.BLACK);
        }
    }

    private void viewSource(ActionEvent evt) {
        //todo lire le formulaire et generer le fichier.
        try {
            GrmFile grm = grmForm.generateGrm();
            doc.removeUndoableEditListener(this.undoListener);
            doc.removeDocumentListener(editorListener);
            doc.remove(0, doc.getLength());
            write(grm.write(), Styles.STYLE_NORMAL);
            doc.addUndoableEditListener(this.undoListener);
            doc.addDocumentListener(editorListener);
        } catch (Exception e) {
            severe("ERROR", e);
        }
        //this.editFile(getEncoding());    
        scroller.add(text);
        scroller.setViewportView(text);
        isForm = false;
        initToolBar();
    }

    private void viewForm(ActionEvent evt) {
        //sauver le fichier et relire le formulaire
        try {
            GrmFile grm = (GrmFile) getProperty("GRM_EDITED");
            grm.reset();
            grm.read(getText());
        } catch (Exception e) {
            severe("ERROR", e);
        }
        grmForm.initializeForm();
        scroller.add(grmForm);
        scroller.setViewportView(grmForm);
        isForm = true;
        initToolBar();
    }
}
