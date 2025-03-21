/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * Parameters.java
 *
 * Created on March 9, 2007, 3:17 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import java.util.*;

/**
 *
 * @author vgrassau
 */
public class Parameters extends Vector<Parameter> {

    private int occurency = 1;

    /**
     *Returns the occurency of this collection
     *@return an number
     */
    public int getOccurence() {
        return this.occurency;
    }

    /**
     * Increase the occurency of this collection of parameters
     *
     *
     * @see removeOccurence()
     */
    public void addOccurence() {
        this.occurency += 1;
    }

    /**
     * Decrease the occurency of this collection of parameters
     *
     * @see addOccurence()
     */
    public void removeOccurence() {
        this.occurency = this.occurency - 1;
    }

    /**
     *Sets the contraintOk value of each parameter
     *@param b the value to set
     *
     *@see Parameter#setConstraintOk(boolean)
     */
    public void setConstraint(boolean b) {
        for (Iterator<Parameter> it = iterator(); it.hasNext();) {
            Parameter param = it.next();
            param.setConstraintOk(b);
        }
    }
}
