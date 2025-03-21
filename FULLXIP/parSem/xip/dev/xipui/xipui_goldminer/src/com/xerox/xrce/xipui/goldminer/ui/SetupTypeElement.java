/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * SetupTypeElement.java
 *
 * Created on October 30, 2006, 2:16 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.ui;

import com.xerox.xrce.xipui.goldminer.BasicElement;
import com.xerox.xrce.xipui.goldminer.GoldminerEngine;
import com.xerox.xrce.xipui.goldminer.TypeElement;
import com.xerox.xrce.xipui.modules.ModuleContext;

import java.util.*;

/**
 *
 * @author vgrassau
 */
public class SetupTypeElement extends SetupBasicElement {

    /** Creates a new instance of SetupTypeElement
     * @param context 
     */
    public SetupTypeElement(ModuleContext context) {
        super(context);
    }

    protected void setElements() {
        GoldminerEngine engine = (GoldminerEngine) getProperty("goldminerEngine");
        elements = new Vector(engine.getTypesList().values());
    }

    /**
     * 
     * @param element
     */
    protected void addNewElement(BasicElement element) {
        TypeElement type = new TypeElement(element.getRealName());
        type.setDescription(element.getDescription());
        type.setLabel(element.getLabel());
        type.setWeight(element.getWeight());
        type.setId(element.getId());
        elements.add(type);
    }

    /**
     * 
     */
    protected void setLabels() {
        super.title.setText("The defined types");
        super.labelList.setText("Types used");
        super.labelDialog.setText("Availables Types");
    }

    /**
     * 
     * @return
     */
    public boolean apply() {
        GoldminerEngine engine = (GoldminerEngine) getProperty("goldminerEngine");
        try {
            Iterator<BasicElement> ite = elements.iterator();
            while (ite.hasNext()) {
                TypeElement type = (TypeElement) ite.next();
                engine.addTypeElement(type);
            }
            Iterator<BasicElement> iteRemove = elementsToRemove.iterator();
            while (iteRemove.hasNext()) {
                TypeElement type = (TypeElement) iteRemove.next();
                engine.removeTypeElement(type.getId());
            }
            return true;
        } catch (Exception e) {
            log.error("ERROR", e);
            return false;
        }
    }
}//end of class
