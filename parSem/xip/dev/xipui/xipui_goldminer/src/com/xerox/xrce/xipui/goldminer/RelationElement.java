/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * RelationElement.java
 *
 * Created on September 6, 2006, 11:03 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

/**
 *
 * @author vgrassau
 */
public class RelationElement extends BasicElement {

    /** Creates a new instance of RelationElement
     * @param realRelation 
     */
    public RelationElement(String realRelation) {
        super(realRelation);

    }

    public int getPos() {
        return 4;
    }
}//end of class
