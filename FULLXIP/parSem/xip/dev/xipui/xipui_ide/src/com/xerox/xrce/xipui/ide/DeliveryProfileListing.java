/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * DeliveryProfileListing.java
 *
 * Created on February 2, 2007, 3:19 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide;

import com.xerox.xrce.xipui.ProfileListing;

import org.apache.log4j.Logger;

/**
 *
 * @author vgrassau
 */
public class DeliveryProfileListing extends ProfileListing {

    public static final String IDE = "IDE";
    /** Creates a new instance of DevProfileListing */
    private Logger log = Logger.getLogger("module");

    public DeliveryProfileListing() {
        try {
            addProfile(IDE, com.xerox.xrce.xipui.ide.ui.IDEProfile.class);
        } catch (Exception e) {
            log.error("ERROR", e);
        }
    }

    public String getDefaultProfileName() {
        return IDE;
    }
}
