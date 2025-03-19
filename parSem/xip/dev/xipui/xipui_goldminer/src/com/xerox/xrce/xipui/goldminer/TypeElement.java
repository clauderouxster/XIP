/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * TypeElement.java
 *
 * Created on September 6, 2006, 11:02 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

/**
 *
 * @author vgrassau
 */
public class TypeElement extends BasicElement {

    /** Creates a new instance of TypeElement
     * @param realType 
     */
    public TypeElement(String realType) {
        super(realType);
    }

    public int getPos() {
        return 4;
    }
}
