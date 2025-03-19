/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * CryptedFileException.java
 *
 * Created on May 2, 2006, 10:55 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.exception;

import java.io.*;

/**
 *
 * @author vgrassau
 */
public class CryptedFileException extends java.lang.Exception {

    /** Creates a new instance of CryptedFileException */
    public CryptedFileException() {
        super("Crypted file exception ");
    }

    public CryptedFileException(File f) {
        super("The file : " + f + " is crypted");
    }

    public String toString() {
        return getMessage();
    }
}
