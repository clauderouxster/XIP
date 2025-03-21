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

/**
 *
 * @author Vianney Grassaud
 */
public class VarEnv {

    public static String AFFECTATION = "=";
    private String name; //nom de la variable d'environement

    private String value; //valeur de la variable d'environement


    /** Creates a new instance of VarEnv */
    public VarEnv() {
        name = new String();
        value = new String();
    }

    /**
     *Check if the varEnv is defined in xip
     *@param xip the xip parser 
     *@return true if exists false otherwise
     */
    public boolean exists(Xip xip) {
        return (xip.getEnv(getName()) != null);
    }

    /** contructeur d'une variable d'environnement
     * @param name nom de la variable
     * @param value valeur de la variable
     */
    public VarEnv(String name, String value) {
        set(name, value);
    }

    /**
     *Initialise le nom de la variable
     *@param name nom de la variable
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     *Initialise la valeur de la variable
     * @param value 
     */
    public void setValue(String value) {
        this.value = value.replace('\\', '/');

    }

    /**
     *Renvoie la valeur de la variable
     *@return valeur de la variable
     */
    public String getValue() {
        return this.value;
    }

    /**
     *Renvoie le nom de la variable
     *@return nom de la variable
     */
    public String getName() {
        return this.name;
    }

    /**
     *Initialise la variable d'envirronement avec, son nom et sa valeur
     *@param name nom de la variable
     * @param value 
     */
    public void set(String name, String value) {
        setName(name);
        setValue(value);
    }

    /**
     *@return true si la variable est affectee (valeur non vide)
     */
    public boolean isAffected() {
        return this.value.equalsIgnoreCase("");
    }

    /**
     *renvoie la variable d'environnement sous forme d'une chaine de caracteres
     *@return : name=value
     */
    public String toString() {
        return this.name + AFFECTATION + this.value;
    }

    /**
     *Verifie l'egalite de la variable d'environnement avec l'objet passe en argument
     *@return true si egalite false sinon
     */
    public boolean equals(Object obj) {
        if (obj instanceof VarEnv) {
            VarEnv v = (VarEnv) obj;
            return (this.name.equalsIgnoreCase(v.getName()) && this.value.equalsIgnoreCase(v.getValue()));

        } else {
            return false;
        }
    }
}// fin de la classe
