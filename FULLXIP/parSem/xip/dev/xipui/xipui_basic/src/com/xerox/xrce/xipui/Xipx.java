/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * Xipx.java
 *
 * Created on February 26, 2007, 10:55 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui;

import com.xerox.jxip.*;
import com.xerox.xrce.xipui.util.FileTools;
import java.util.*;
import java.io.*;
import java.lang.reflect.*;
import org.apache.log4j.Logger;

/**
 *
 * @author vgrassau
 */
public class Xipx {

    private Xip xip;
    private XipResultManager manager;
    private Logger log = Logger.getLogger("xip");

    /** Creates a new instance of Xipx
     * @param xip 
     */
    public Xipx(Xip xip) {
        this.xip = xip;
        manager = new XipResultManager();
    }

    public Hashtable<String, String> getXmlTagInfo() {
        return manager.getXmlTagInfo();
    }

    public XipResult getXipResult() {
        return manager.getXipResult();
    }

    public synchronized boolean buildXipResult(int gHandler, String text) throws Exception {
        boolean success = false;
        log.info("BuildXipResult with gHandler " + gHandler);
        XipUtilities.checkHandler(gHandler);
        if (text == null) {
            throw new NullPointerException();
        }
        XipUtilities.setUTF8(xip, gHandler, true);
        success = (manager.buildXipResult(gHandler, text) == 0);
        XipUtilities.setUTF8(xip, gHandler, false);
        return success;
    }

    /**
     *Keeps the native XipResult in memory.
     *This method can be used when callback system is used also. Normally after each callback is called, the native xip result is destroyed.
     *If it's necessary to keep it for some reason (see the generator), calls this method in the callback.
     * @throws java.lang.Exception 
     */
    public synchronized void fireKeepNativeXipResult() throws Exception {
        manager.keepNativeXipResult();
    }

    /**
     *Destoys the current native XipResult. This method sould be call <b>before</b> each new native XipResult 
     *will be constructed. When callback are used, a user can specify that he wants to keep the native XipResult, e.g. 
     *the Generator needs to have a XipReuslt reference. So, in order to not grow the memory with the XipReuslt, we need to call
     *this method.
     *@param gHandler the grammar handler
     *@throws Exception if error
     */
    protected synchronized void destroyNativeXipResult(int gHandler) throws Exception {
        manager.destroyNativeXipResult(gHandler);
    }

    /**
     *  build the java XipResult from the c++ data
     *
     * @param  gHandler  a grammar handler
     * @param file 
     * @param encoding 
     * @return   true if no error
     * @throws java.lang.Exception 
     */
    public synchronized boolean buildXipResultFromFile(int gHandler, File file, String encoding) throws Exception {
        log.info("BuildXipResultFromFile with gHandler " + gHandler + " from " + file + " with encoding " + encoding);
        XipUtilities.checkHandler(gHandler);
        FileTools.exists(file);
        if (XipUtilities.isUTF8(encoding)) {
            XipUtilities.setUTF8(xip, gHandler, true);
        }
        boolean res = (manager.buildXipResultFromFile(gHandler, file.getAbsolutePath()) == 0);
        XipUtilities.setUTF8(xip, gHandler, false);
        return res;
    }

    /**
     *  build the java XipResult from the c++ data
     *
     * @param  gHandler  a grammar handler
     * @param file 
     * @param encoding 
     * @return  true if no error
     * @throws java.lang.Exception 
     */
    public synchronized boolean buildXipResultFromXmlFile(int gHandler, File file, String encoding) throws Exception {
        log.info("BuildXipResultFromXmlFile with gHandler " + gHandler + " from " + file + " with encoding " + encoding);
        XipUtilities.checkHandler(gHandler);
        FileTools.exists(file);
        if (XipUtilities.isUTF8(encoding)) {
            XipUtilities.setUTF8(xip, gHandler, true);
        }
        boolean res = (manager.buildXipResultFromXmlFile(gHandler, file.getAbsolutePath()) == 0);
        XipUtilities.setUTF8(xip, gHandler, false);
        return res;
    }

    public void callMethodOnXipUnitFromFile(int gHandler, File file, Object caller, String methodName, String encoding) throws Exception {
        /** check parameters */
        XipUtilities.checkHandler(gHandler);
        // if the fireKeepNativeXipResult was called before 
        destroyNativeXipResult(gHandler);
        FileTools.exists(file);
        checkParamFromCallMethod(caller, methodName);
        //xip.setDisplayMode(gHandler,0,80);
        if (XipUtilities.isUTF8(encoding)) {
            XipUtilities.setUTF8(xip, gHandler, true);
        }
        manager.xipResultBuilderFromTextFile(gHandler, file.getAbsolutePath(), caller, methodName);
        XipUtilities.setUTF8(xip, gHandler, false);
    }

    public void callMethodOnXipUnitFromXMLFile(int gHandler, File file, Object caller, String methodName, String xmlMethod, String encoding) throws Exception {
        XipUtilities.checkHandler(gHandler);
        // if the fireKeepNativeXipResult was called before 
        destroyNativeXipResult(gHandler);
        FileTools.exists(file);
        checkParamFromCallMethod(caller, methodName);
        checkParamFromCallMethod(caller, xmlMethod);
        if (XipUtilities.isUTF8(encoding)) {
            XipUtilities.setUTF8(xip, gHandler, true);
        }
        if (caller == null) {
            throw new NullPointerException();
        }
        manager.xipResultBuilderFromXmlFile(gHandler, file.getAbsolutePath(), caller, methodName, xmlMethod);
        XipUtilities.setUTF8(xip, gHandler, false);
    }

    public void callMethodOnXipUnitFromText(int gHandler, String text, Object caller, String methodName, String encoding) throws Exception {
        XipUtilities.checkHandler(gHandler);
        // if the fireKeepNativeXipResult was called before 
        destroyNativeXipResult(gHandler);
        if (text == null) {
            throw new NullPointerException();
        }
        checkParamFromCallMethod(caller, methodName);
        if (XipUtilities.isUTF8(encoding)) {
            XipUtilities.setUTF8(xip, gHandler, true);
        }
        manager.xipResultBuilderFromString(gHandler, text, caller, methodName);
        XipUtilities.setUTF8(xip, gHandler, false);
    }

    private void checkParamFromCallMethod(Object caller, String methodName) throws Exception {
        if (caller == null) {
            throw new NullPointerException();
        } else {
            Method[] m = caller.getClass().getMethods();
            boolean stop = false;
            for (int i = 0; i < m.length && !stop; i++) {
                if (m[i].getName().equals(methodName)) {
                    stop = true;
                }
            }
            if (!stop) {
                throw new Exception("no method named : " + methodName);
            }
        }
    }
}
