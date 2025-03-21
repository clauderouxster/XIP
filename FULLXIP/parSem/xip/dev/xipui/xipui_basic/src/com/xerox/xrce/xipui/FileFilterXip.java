/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2005 Xerox Corporation                                                 *
 * Xerox Research Centre Europe - Grenoble                                              *
 * All Rights Reserved                                                                  *
 * Copyright protection claimed includes all forms and matters of copyrightable         *
 * material and information now allowed by statutory or judicial law or                 *
 * hereinafter granted, including without limitation, material generated from           *
 * the software programs which are displayed on the screen such as icons,               *
 * screen display looks, etc. All rights reserved. No part of this document             *
 * may be reproduced, stored in a retrieval system, or transmitted, in any              *
 * form or by any means, electronic, mechanical, photocopied, recorded, or              *
 * otherwise, without prior written consent from XEROX corporation.                     *
 ****************************************************************************************
 */
package com.xerox.xrce.xipui;

import java.io.*;
import java.util.*;

/**
 *
 * @author Vianney Grassaud
 */
public class FileFilterXip extends javax.swing.filechooser.FileFilter {

    //liste des extensions du filtre
    private Vector extension;
    //expression reguliere pour reconnaitre certains noms de fichiers
    private String regexp;

    /** Creates a new instance of FileFilter */
    public FileFilterXip() {
        extension = new Vector();
        regexp = ".*";
    }

    /**
     * Ajoute une extension de fichier a reconnaitre au filtre
     *@param ext extension a rajouter au filtre
     **/
    public void addExtension(String ext) {
        extension.add(ext);
    }

    /**
     *Ajoute plusieurs extensions de fichier contenu dans un tableau de String au filtre
     *@param ext tableau contenant des extensions
     */
    public void setExtension(String[] ext) {
        for (int i = 0; i < ext.length; i++) {
            extension.add(ext[i]);
        }
    }

    /**
     *Initialise le filtre pour les noms de fichiers
     * @param regexp 
     */
    public void setFilterName(String regexp) {
        this.regexp = regexp;
    }

    /**
     *Retourne l'extension d'un fichier passe en parametre.
     *@param f 
     * @f fichier dont on doit recuperer l'extension
     *@return extension du fichier passe en parmetre si pas d'extension on renvoie null
     */
    public static String getExtensionOfFile(File f) {
        String ext = null;
        String s = f.getName();
        int i = s.lastIndexOf('.');
        if (i > 0 && i < s.length() - 1) {
            ext = s.substring(i + 1).toLowerCase();
        }
        return ext;
    }

    /**
     *@return Renvoie toutes les extensions du filtre
     */
    public Enumeration getExtension() {
        return extension.elements();
    }

    /**
     *@param f 
     * @return boolean Renvoie true si le fichier est accepte, false sinon.
     * On renvoie true si le fichier est un "dossier"
     */
    public boolean accept(File f) {
        if (f.isDirectory()) {
            // si file est un dossier on accepte
            return true;
        }
        // on recupere l'extension du fichier
        String ext = getExtensionOfFile(f);
        if (ext != null) {
            //si cl'extension est non nulle, on verifie qu'elle appartient au filtre
            Enumeration e = this.getExtension();
            // on recupere les extensions du filtre
            while (e.hasMoreElements()) {
                String tempExt = (String) e.nextElement();
                if (ext.equals(tempExt) && (getFileNameWithoutExt(f)).matches(regexp)) {
                    // si l'extension est differente du filtre on retourne false
                    return true;
                }
            }
            // le fichier ne correspond pas au filtre
            return false;
        } else {
            // le fichier ne correspond pas au filtre
            return false;
        }
    }

    /**
     *Renvoie le nom d'un fichier sans son extension.
     *@param f fichier dont il faut recuperer le nom
     *@return le nom du fichier sans l'extension. 
     */
    private String getFileNameWithoutExt(File f) {
        String name = f.getName();
        String[] splitName = name.split("\\.");
        if (splitName.length == 0) {
            return name;
        }
        if (splitName.length == 2) {
            return splitName[0];
        }
        if (splitName.length > 2) {
            String name_ = "";
            for (int i = 0; i < splitName.length - 1; i++) {
                name_ += splitName[i];
            }
            return name_;
        }
        return name;
    }

    /**
     * @return Renvoie la description du filtre.
     * On retourne une chaine de caracteres ayant comme format : *.extension, *.extension 
     */
    public String getDescription() {
        // description retrounee
        String desc = "";
        // on recupere toutes les extensions
        Enumeration e = this.getExtension();
        // la premiere extension (s'il y en a une) ne commence pas avec une virgule ;-)

        if (e.hasMoreElements()) {
            desc = "*." + (String) e.nextElement();
        }
        // suite des extension reconnues
        while (e.hasMoreElements()) {
            desc += ", *." + (String) e.nextElement();
        }
        return desc;
    }
}
