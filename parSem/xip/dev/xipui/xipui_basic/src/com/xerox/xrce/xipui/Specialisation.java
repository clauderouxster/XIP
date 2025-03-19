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
import org.apache.log4j.*;

/**
 * @author Vianney Grassaud
 */
public class Specialisation implements Comparable {

    private Logger log;
    // flag optionel indiquant si la specialisation est utilisee comme parametre  a XIP 
    private boolean parameter = false;
    //indique si la specialisation est cryptee ou non
    private boolean isCrypted = false;
    private File file;

    /**
     *Constructeur prenant en entree le nom de la specialisation :
     * @param path 
     */
    public Specialisation(String path) {
        log = Logger.getLogger("grammar");
        file = new File(path);
    }

    /**
     *Positionne le flag parameter a true ou false. Si true alors la specialisation doit etre
     *chargee comme parametre.
     *@param b true ou false
     */
    public void setParameter(boolean b) {
        this.parameter = b;
    }

    /**
     *Check if the specialisation is a parameter or not
     *@return true if the specialisation is a parameter, false otherwise
     */
    public boolean isParameter() {
        return this.parameter;
    }

    public File getFile() {
        return this.file;
    }

    /**
     *Indique que cette specialisation est cryptee ou non
     *@param b true specialisation crytpee, false sinon
     */
    public void setCrypted(boolean b) {
        this.isCrypted = b;
    }

    public int compareTo(Object o) {
        return toString().compareTo(o.toString());
    }

    /**
     *Renvoie si la specialistion est cryptee.
     *@return true la specialisation est cryptee, false sinon
     *
     */
    public boolean isCrypted() {
        return this.isCrypted;
    }

    /**
     *Verifie l'egalite de cette specialisation avec un autre objet
     *@param o objet a comparer
     *@return true si les objets sont egaux, false sinon
     */
    public boolean equals(Object o) {
        if (o instanceof Specialisation) {
            Specialisation os = (Specialisation) o;
            return this.file.equals(os.getFile());
        }
        return false;
    }

    void setParameter(String bool) {
        if (bool.equalsIgnoreCase("true") || bool.equalsIgnoreCase("yes") || bool.equalsIgnoreCase("oui")) {
            setParameter(true);
        } else {
            setParameter(false);
        }
    }

    /**
     *@return envoie le nom de la specialisation
     *
     */
    public String getName() {
        if (this.file != null) {
            return this.file.getName();
        } else {
            return null;
        }
    }

    /**
     *@return renvoie le nom absolu du fichier
     */
    public String getAbsolutePath() {
        if (this.file != null) {
            return this.file.getAbsolutePath();
        } else {
            return null;
        }

    }

    public String toString() {
        if (file != null) {
            return getFile().getName();
        } else {
            return null;
        }
    }
}//end of the class
