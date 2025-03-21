/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ProfileManager.java
 *
 * Created on November 10, 2006, 1:59 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui;

import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.AbstractProfile;
import java.lang.reflect.Constructor;

import java.util.*;
import org.apache.log4j.*;

/**
 * Cette classe permet de decrire les profiles possible pour un projet.
 * @author vgrassau
 */
public class ProfileManager {

    private ProfileListing listing = null;
    private Logger log;
    /** cache des profiles en cours d'utilisation */
    private Map<String, Collection<AbstractProfile>> cache;

    /** Creates a new instance of ProfileManager */
    private ProfileManager() {
        log = Logger.getLogger("module");
    }

    private ProfileManager(ProfileListing listing) {
        this();
        this.listing = listing;
    }

    /**
     *Renvoie le profile par defaut
     *@return le profile par default a utiliser
     */
    public String getDefaultProfile() {
        return this.listing.getDefaultProfileName();
    }

    /**
     * Renvoie un profile graphique selon un profile donne ainsi que le nom du projet.
     *
     * @param profileName 
     * @param projectPath chemin absolu du projet (pas celui du fichier de configuration)
     * @return un AbstractProfile si trouve sinon null
     */
    public AbstractProfile getProfile(String profileName, String projectPath) {
        Collection<AbstractProfile> guisCached = getGUIcached(profileName);
        // on recherche si la liste contient l'un des projets :
        Iterator<AbstractProfile> ite = guisCached.iterator();
        AbstractProfile newProfile = null;
        boolean found = false;
        while (ite.hasNext() && !found) {
            AbstractProfile gui = ite.next();
            Project project = gui.getProject();
            if (project != null) {
                String projectPathGUI = gui.getProject().getFullName();
                if (projectPathGUI.equals(projectPath)) {
                    found = true;
                    newProfile = gui;
                }
            } else {
                //le projet est null, le profile a ete ferme
                ite.remove();
            }
        }
        return newProfile;
    }

    private Collection<AbstractProfile> getGUIcached(String profileName) {
        if (cache == null) {
            cache = new Hashtable();
        }
        Collection<AbstractProfile> guisCached = cache.get(profileName);
        if (guisCached == null) {

            guisCached = new Vector();
            cache.put(profileName, guisCached);
        }
        return guisCached;
    }

    /**
     *Creer un profile graphique a partir d'un profile donne, d'un projet et d'un modulecontext
     *@param profileName 
     * @param project le projet a utiliser
     *@param initialContext le context initial
     * @return 
     *
     */
    public AbstractProfile createProfile(String profileName, Project project, ModuleContext initialContext) {
        Collection<AbstractProfile> guisCached = getGUIcached(profileName);
        AbstractProfile newProfile = getProfile(profileName, project.getFullName());
        if (newProfile == null) {
            try {
                Class c = listing.getProfile(profileName);
                Constructor cons = c.getConstructor(com.xerox.xrce.xipui.modules.ModuleContext.class,
                        com.xerox.xrce.xipui.Project.class);
                newProfile = (AbstractProfile) cons.newInstance(initialContext, project);
            } catch (Exception e) {
                log.error("ERROR", e);
            }
            guisCached.add(newProfile);
        }
        return newProfile;
    }

    /**
     *Renvoie la liste de tous les profiles disponibles
     *@return tableau de profile
     */
    public Collection<String> getProfiles() {
        return this.listing.getProfileNames();
    }

    private static class SingletonHolder {

        //private static ProfileManager instance = new ProfileManager();
        private static ProfileManager instance2 = null;
    }

    public static ProfileManager getExistedInstance() {
        return SingletonHolder.instance2;
    }

    public static ProfileManager newInstance(ProfileListing listing) {
        return SingletonHolder.instance2 = new ProfileManager(listing);
    }
}
