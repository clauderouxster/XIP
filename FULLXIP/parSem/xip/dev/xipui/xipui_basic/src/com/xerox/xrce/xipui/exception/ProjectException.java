/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ProjectException.java
 *
 * Created on February 19, 2007, 11:18 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.exception;

/**
 *
 * @author vgrassau
 */
public class ProjectException extends Exception {

    private Class propertyFailed;

    public ProjectException(String message, Throwable cause) {
        super(message, cause);
    }

    public ProjectException(String message, Throwable cause, Class property) {
        this(message, cause);
        setPropertyFailed(property);
    }

    /** Creates a new instance of ProjectException
     * @param message 
     */
    public ProjectException(String message) {
        super(message);

    }

    /** Creates a new instance of ProjectException
     * @param message
     * @param property 
     */
    public ProjectException(String message, Class property) {
        this(message);
        setPropertyFailed(property);

    }

    public void setPropertyFailed(Class property) {
        this.propertyFailed = property;
    }

    public String toString() {
        StringBuilder builder = new StringBuilder(getClass().getName());
        if (propertyFailed != null) {
            builder.append('\n');
            builder.append(propertyFailed.getName());
            builder.append(" failed");
        }
        builder.append('\n');
        builder.append(getMessage());
        return builder.toString();
    }
}//end of class
