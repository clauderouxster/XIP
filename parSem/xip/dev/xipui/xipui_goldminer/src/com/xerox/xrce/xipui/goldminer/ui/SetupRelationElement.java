/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * SetupRelationElement.java
 *
 * Created on October 30, 2006, 2:19 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.ui;

import com.xerox.xrce.xipui.goldminer.BasicElement;
import com.xerox.xrce.xipui.goldminer.GoldminerEngine;
import com.xerox.xrce.xipui.goldminer.RelationElement;
import com.xerox.xrce.xipui.modules.ModuleContext;
import java.util.*;

/**
 *
 * @author vgrassau
 */
public class SetupRelationElement extends SetupBasicElement {

    /** Creates a new instance of SetupTypeElement
     * @param context 
     */
    public SetupRelationElement(ModuleContext context) {
        super(context);
    }

    protected void setElements() {
        GoldminerEngine engine = (GoldminerEngine) getProperty("goldminerEngine");
        elements = new Vector(engine.getRelationsList().values());
    }

    protected void addNewElement(BasicElement element) {
        RelationElement relation = new RelationElement(element.getRealName());
        relation.setDescription(element.getDescription());
        relation.setLabel(element.getLabel());
        relation.setWeight(element.getWeight());
        relation.setId(element.getId());
        elements.add(relation);
    }

    protected void setLabels() {
        super.title.setText("The defined relations");
        super.labelList.setText("Relations used");
        super.labelDialog.setText("Availables Relations");

    }

    public boolean apply() {
        GoldminerEngine engine = (GoldminerEngine) getProperty("goldminerEngine");
        try {
            Iterator<BasicElement> ite = elements.iterator();
            while (ite.hasNext()) {
                RelationElement element = (RelationElement) ite.next();
                engine.addRelationElement(element.getId(), element);
            }
            Iterator<BasicElement> iteRemove = elementsToRemove.iterator();
            while (iteRemove.hasNext()) {
                RelationElement type = (RelationElement) iteRemove.next();
                engine.removeRelationElement(type.getId());
            }
            return true;
        } catch (Exception e) {
            log.error("ERROR", e);
            return false;
        }
    }
}
