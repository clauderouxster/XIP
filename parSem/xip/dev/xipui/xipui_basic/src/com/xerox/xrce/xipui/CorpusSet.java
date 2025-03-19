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
import org.apache.log4j.*;

/**
 *
 * @author Vianney Grassaud
 *
 *
 */
public class CorpusSet implements Comparable {

    private Logger log;
    private Vector<CorpusElement> liste;
    private boolean toParse = false;
    private File corpus;
    private boolean isDefault = false;

    /**
     * Creates a new corpus set.
     *
     *
     * @param path 
     * @param name
     */
    public CorpusSet(String path, String name) {
        log = Logger.getLogger("project");
        log.info("Creates CorpusSet " + name);
        liste = new Vector();

        String fullPath = path + "/" + name;
        corpus = new File(fullPath.replace('\\', '/'));
        corpus.mkdirs();


    }

    public void setDefault(boolean b) {
        this.isDefault = b;
    }

    public int compareTo(Object o) {
        return toString().compareTo(o.toString());
    }

    /**
     * Creates a new corpus set.
     *
     *
     * @param path 
     * @param name
     */
    public CorpusSet(File path, String name) {
        this(path.getAbsolutePath(), name);
    }

    public Vector<CorpusElement> getCorpusElementToParse() {
        Vector res = new Vector();
        for (int i = 0; i < liste.size(); i++) {
            CorpusElement ce = liste.get(i);
            if (ce.isToParse()) {
                res.add(ce);
            }
        }
        return res;
    }

    public int countCorpusElementToParse() {
        return getCorpusElementToParse().size();
    }

    public boolean isDefault() {
        return this.isDefault;
    }

    public void setToParse(boolean b) {

        Iterator<CorpusElement> ite = getElements();
        while (ite.hasNext()) {
            CorpusElement ce = ite.next();
            ce.setToParse(b);
        }
        this.toParse = b;
    }

    public File getDirectory() {
        File canonicFile = corpus;
        try {
            canonicFile = corpus.getCanonicalFile();
        } catch (Exception e) {
            log.error(e);

        }
        return canonicFile;
    }

    public boolean isToParse() {
        if (this.size() != 0) {
            boolean stop = false;

            Iterator<CorpusElement> ite = getElements();
            while (ite.hasNext() && !stop) {
                CorpusElement ce = ite.next();
                if (!ce.isToParse()) {
                    stop = true;
                }
            }
            toParse = !stop;
            return this.toParse;
        } else {
            return false;
        }
    }

    /**
     * ajoute un coprus a la liste, on alimente a la volee le fichier project-config.xml
     * @param c corpusElement a rajouter au corpus.
     * si le corpus contient deja cet element alors il n'est pas rajoute.
     */
    public void addCorpusElement(CorpusElement c) {
        if (!liste.contains(c)) {
            c.setParent(getName());
            log.trace("Add a CorpusElement for " + getName());
            liste.add(c);
        }
    }

    /**
     *@return renvoie la liste de tous les corpusElement sous forme d'un Vector.
     */
    public Vector<CorpusElement> getAllCorpusElement() {
        return this.liste;
    }

    /**
     *@return renvoie le path absolu du corpus
     */
    public String getPath() {
        return this.corpus.getAbsolutePath();
    }

    /**
     *Verifie si ce corpusSet contient deja le corpusElement passe en parametre
     *@param c corpusElement a verifier
     * @return 
     */
    public boolean contains(CorpusElement c) {
        return liste.contains(c);
    }

    /**
     *@param fileName 
     * @return renvoie le corpusElement correspondant au nom de fichier passe en parametre
     *renvoie null si le corpusElement n'est pas trouve.
     */
    public CorpusElement getCorpusElement(String fileName) {
        File f = new File(fileName);
        CorpusElement result = null;
        for (Iterator<CorpusElement> e = getElements(); e.hasNext() && result == null;) {
            CorpusElement ce = e.next();
            if (ce.getFile().equals(f)) {
                result = ce;
            }
        }
        return result;
    }

    /**
     *Supprime le corpusElement passe en parametre
     * @param ce 
     */
    public void removeCorpusElement(CorpusElement ce) {
        liste.removeElement(ce);
    }

    /**
     *@return retourn la representation d'un corpuSet, c'est a dire son nom
     */
    public String toString() {
        return getName();
    }

    /**
     *@return renvoie le nom de ce CorpusSet
     */
    public String getName() {
        return corpus.getName();
    }

    /**
     *@return renvoie une enumeration des CorpusElement de ce CorpusSet
     */
    public Iterator<CorpusElement> getElements() {
        return liste.iterator();
    }

    /**
     *@return renvoie le dernier CorpusElement ajouter a ce CorpusSet
     *si une exception est capturee alors, on renvoie null
     */
    public CorpusElement getLastAdded() {
        CorpusElement last = null;
        try {
            last = liste.lastElement();
        } catch (Exception e) {
            last = null;
        }
        return last;
    }

    /**
     *@return renvoie la taille de ce corpusSet
     */
    public int size() {
        return liste.size();
    }
}// fin de la classe

