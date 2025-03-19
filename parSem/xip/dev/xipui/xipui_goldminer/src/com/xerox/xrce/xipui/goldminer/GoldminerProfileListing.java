/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * GoldminerProfileListing.java
 *
 * Created on February 2, 2007, 3:29 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import com.xerox.xrce.xipui.ProfileListing;
import org.apache.log4j.Logger;

/**
 *
 * @author vgrassau
 */
public class GoldminerProfileListing extends ProfileListing {

    /** Creates a new instance of DevProfileListing */
    public static final String FACTSPOTTER = "FACTSPOTTER";
    private Logger log = Logger.getLogger("module");

    public GoldminerProfileListing() {
        try {
            addProfile(FACTSPOTTER, com.xerox.xrce.xipui.goldminer.ui.GoldminerProfile.class);
        } catch (Exception e) {
            log.error(e);
        }
    }

    public String getDefaultProfileName() {
        return FACTSPOTTER;
    }
}
