/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * FileStepManager.java
 *
 * Created on March 8, 2006, 10:08 AM
 *
 * To change this template, choose Tools | Template AbstractStepManager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.CorpusElement;
import com.xerox.xrce.xipui.CorpusSet;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.corpus.CorpusElementProperties;
import com.xerox.xrce.xipui.ui.corpus.CorpusManager;
import java.io.*;

/**
 *
 * @author vgrassau
 */
public class FileStepManager extends AbstractStepManager {

    static public final String type_file = "type_file";
    static public final String category_file = "category_file";
    static public final String file_created = "file_created";
    static public final String option_fileManager = "option_fileManager";
    static public final String corpus_parent = "corpus_parent";
    static public final short ALL = 0;
    static public final short GRAMMAR_ONLY = 1;
    static public final short CORPUS_ONLY = 2;

    /**
     * Creates a new instance of FileStepManager
     * @param properties
     * @param option 
     */
    public FileStepManager(ModuleContext properties, short option) {
        super(properties, 2);
        managerProperties.put(option_fileManager, option);
        steps[0] = new TypeFileStep(properties, managerProperties);
        steps[0].initStepManager();
        steps[1] = new NewFileStep(properties, managerProperties);
        setTitle("Create a New File");
        entryPanel.add(steps[0]);
        initDescription(0, "label.file.choose.type", steps[0].getDescription());
        initDescription(1, "label.file.choose.name", NEXT_STEP);
    }

    public boolean finish() {
        try {
            String fileCreated = (String) this.managerProperties.get(FileStepManager.file_created);
            String category = (String) this.managerProperties.get(FileStepManager.category_file);
            File file = new File(fileCreated);
            if (file.exists()) {
                warning("The file : " + file + " already exist");
                return false;
            } else {
                if (!file.createNewFile()) {
                    warning("File not created !! ");
                    return false;
                }
            }
            if (category.equals(TypeFileStep.corpus_file)) {
                Project p = getProject();
                String parent = (String) this.managerProperties.get(FileStepManager.corpus_parent);
                CorpusElement ce = new CorpusElement(file);
                CorpusSet cs = p.getCorpusSet(parent);
                cs.addCorpusElement(ce);
                CorpusElementProperties ceProp = new CorpusElementProperties(ce, null);
                ceProp.showInDialog(true);
                CorpusManager cm = (CorpusManager) getProperty(module_corpus);
                Viewer v = cm.getViewer();
                if (v != null) {
                    v.refresh();
                }
                p.save();
            }
            return true;
        } catch (Exception e) {
            this.severe("ERROR", e);
            return false;
        }
    }

    /***
     *
     */
    public void loadMode() {
    }
}//end of class
