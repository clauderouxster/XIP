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
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.PanelModule;
import java.util.*;
import java.awt.*;
import javax.swing.*;

/**
 * Tnis class is abstract, but it implements the interface Result. 
 * and extends PanelModule in order to implement also IModeul inherited with Result.
 * But also, this class is also  a graphical container because it extends PanelModule.
 * 
 *There are only two abstract methods : closeModule() and setResultName()
 * @author  vgrassau
 */
public abstract class AbstractResult extends PanelModule implements Result {

    //the list of corpusElement 
    protected Vector listCorpusElement;
    protected Icon icon;
    // the id of the result
    private int id = 0;
    // the name of the result, more readable than an id  
    protected String resultName;

    /**
     *Instanciate the result 
     *@param context the context who is coming from a module
     *@param id the id for the result 
     */
    public AbstractResult(ModuleContext context, int id) {
        super(context);
        this.id = id;
        initComponents();
        save.setAction(new AbstractAction("save", XIPUIUtilities.getIcon(UIConstants.SAVE)) {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                //saveActionPerformed(evt);
            }
        });
        //TODO implement the method to save a result
        save.setEnabled(false);
        save.setText("");
        save.setToolTipText("Save");
        //the close method is in property of a module context
        close.setAction((Action) getProperty("CLOSE_RESULT"));
        close.setText("");
        close.setToolTipText("Close this result");
        toolBar.add(close);
        toolBar.add(save);
        this.add(toolBar, BorderLayout.PAGE_START);
        //no border
        this.setBorder(null);
        setResultName();
    }

    /** 
     *Return the id of this result
     *@return the id 
     */
    public int getId() {
        return this.id;
    }

    /**
     *Must be implemented
     *@see Result#setResultName()
     */
    public abstract void setResultName();

    /**
     *Result the name of the result
     */
    public String getResultName() {
        return this.resultName;
    }

    /**
     *Set the list of CorpusElement 
     *@param corpus Vector of CorpusElement
     */
    public void setListCorpusElement(Collection corpus) {
        Vector v = new Vector(corpus);
        this.listCorpusElement = (Vector) v.clone();
    }

    /**
     *get the list of CorpusElement
     *@return Vector of corpus Element
     */
    public Vector getListCorpusElement() {
        return this.listCorpusElement;
    }

    public void setIcon(Icon icon) {
        this.icon = icon;
    }

    /**
     *Call the method getResultName()
     *@return the resultName
     */
    public String getName() {
        return getResultName();
    }

    /**
     *Get the icon
     *@return icon describing this result
     */
    public Icon getIcon() {
        return this.icon;
    }

    /**
     *@TODO Should be implemented
     *@see Result#closeModule()
     */
    public abstract void closeModule();

    /** Close the result  
    this method call the closeModule method and remove all components on this container
     */
    public void close() {
        closeModule();
        this.removeNotify();
        this.listCorpusElement.clear();
        this.listCorpusElement = null;
        this.resultName = null;
        this.isClosed = true;
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        toolBar = new javax.swing.JToolBar();
        close = new javax.swing.JButton();
        save = new javax.swing.JButton();

        toolBar.setFloatable(false);
        toolBar.setRollover(true);
        close.setText("jButton1");
        save.setText("save");
        save.setBorderPainted(false);
        save.setEnabled(false);

        setLayout(new java.awt.BorderLayout());

    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    protected javax.swing.JButton close;
    protected javax.swing.JButton save;
    protected javax.swing.JToolBar toolBar;
    // End of variables declaration//GEN-END:variables
}
