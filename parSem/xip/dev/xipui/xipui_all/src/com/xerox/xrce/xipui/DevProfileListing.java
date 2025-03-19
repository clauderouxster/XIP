/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * DevProfileListing.java
 *
 * Created on February 2, 2007, 2:16 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui;

import org.apache.log4j.Logger;

/**
 *
 * @author vgrassau
 */
public class DevProfileListing extends ProfileListing {

    public static final String IDE = "IDE";
    //public static final String GOLDMINER_1 = "GOLDMINER_1";
    public static final String FACTSPOTTER = "FACTSPOTTER";
    /** Creates a new instance of DevProfileListing */
    private Logger log = Logger.getLogger("module");

    /**
     * 
     */
    public DevProfileListing() {
        try {
            addProfile(IDE, com.xerox.xrce.xipui.ide.ui.IDEInternProfile.class);
            //addProfile(GOLDMINER_1,com.xerox.xrce.xipui.litigation.ui.GoldminerProject.class);
            addProfile(FACTSPOTTER, com.xerox.xrce.xipui.goldminer.ui.GoldminerProfile.class);
        } catch (Exception e) {
            log.error("ERROR", e);
        }
    }

    public String getDefaultProfileName() {
        return IDE;
    }
}
