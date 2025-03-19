/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ProfileListing.java
 *
 * Created on February 2, 2007, 11:39 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui;

import java.util.*;
import org.apache.log4j.*;

/**
 *
 * @author vgrassau
 */
public abstract class ProfileListing {

    //the map between label for profile and the name of the class representing the profile
    private Map<String, Class> map;
    private Logger log = Logger.getLogger("module");

    /**
     *Create new instance
     */
    public ProfileListing() {
        map = new HashMap();
    }

    /**
     *Return the class of the given profile name
     *@param name the name (label) of the profile
     *@return the class to use, or the default profile if not found, or null if nothing was found
     */
    public Class getProfile(String name) {
        Class c = map.get(name);
        if (c == null) {
            c = map.get(getDefaultProfileName());
        }
        return c;
    }

    /**
     *add a profile to the profile listing
     *@param name the name of the profile
     *@param class_ the class to use for this profile
     *
     */
    protected void addProfile(String name, Class class_) {
        map.put(name, class_);
    }

    /**
     *add a profile to the profile listing
     *@param name the name of the profile
     *@param className the class name (e.g java.lang.String) to use
     * @throws java.lang.Exception 
     *
     */
    protected void addProfile(String name, String className) throws Exception {
        try {
            Class _class = Class.forName(className);
            map.put(name, _class);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *Remove the given profile
     *@param name the profile to remove
     */
    protected void removeProfile(String name) {
        map.remove(name);
    }

    /**
     *Return a collection of all profiles names
     *@return Collection of string
     */
    public Collection<String> getProfileNames() {
        return map.keySet();
    }

    /**
     *Return all the class for the profiles
     *@return Collection
     */
    public Collection<Class> getProfiles() {
        return map.values();
    }

    /**
     *Return the default profile to use
     *@return the default profiel name
     */
    public abstract String getDefaultProfileName();

    /**
     *Return the default profile to use
     *@return the default profile class
     */
    public Class getDefaultProfile() {
        return map.get(getDefaultProfileName());
    }
}
