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

import com.xerox.xrce.xipui.exception.UnknownVarEnvException;
import com.xerox.xrce.xipui.util.FileTools;
import java.util.*;
import org.apache.log4j.*;
import java.io.*;

/**
 * Gestion du fichier grm
 * @author Vianney Grassaud
 *@deprecated
 */
public class GrmFile implements Cloneable {

    /* liste des instructions pour le fichier grm */
    public static final String INSTR_PATH = "path:";
    public static final String INSTR_GRAMMAR = "grammar:";
    public static final String INSTR_GRAM = "gram:";
    public static final String INSTR_NTM = "ntm:";
    public static final String INSTR_HMM = "hmm:";
    public static final String INSTR_PARAMETER = "parameter:";
    public static final String INSTR_VARIABLE = "variable:";
    public static final String INSTR_LOOKUP = "lookup:";
    public static final String INSTR_TOKENIZER = "tokenizer:";
    public static final String INSTR_STRATEGY = "strategy:"; /* strategy:(BREADTH | DEPTH)  */

    public static final String INSTR_FLAGS = "flags:";
    private long lastModified;
    /* table pour les intructions : une instruction pour une liste de maximum CAPACITY directives */
    private Hashtable grm;
    private File file;
    // liste des variables d'envirronement utilisees dans le fichier grm
    private Vector variables;
    // capacite maximale pour une instruction
    public final int CAPACITY = 100;
    private Xip xip;
    //the project context
    private ProjectContext projectContext;
    public Logger log;

    /** Creates a new instance of GrmFile
     * @param xip
     * @param context 
     */
    public GrmFile(Xip xip, ProjectContext context) {
        log = Logger.getLogger("grammar");
        this.xip = xip;
        this.projectContext = context;
        grm = new Hashtable();
        grm.put(INSTR_GRAMMAR, new Vector(CAPACITY));
        grm.put(INSTR_GRAM, new Vector(CAPACITY));
        grm.put(INSTR_PATH, new Vector(CAPACITY));
        grm.put(INSTR_PARAMETER, new Vector(1000, 1));
        grm.put(INSTR_NTM, new Vector(CAPACITY));
        grm.put(INSTR_HMM, new Vector(CAPACITY));
        grm.put(INSTR_VARIABLE, new Vector(CAPACITY));
        grm.put(INSTR_LOOKUP, new Vector(CAPACITY));
        grm.put(INSTR_TOKENIZER, new Vector(CAPACITY));
        grm.put(INSTR_STRATEGY, new Vector(CAPACITY));
        grm.put(INSTR_FLAGS, new Vector(CAPACITY));
        variables = new Vector();

    }

    /** set the environnement variables for the grmfile
     * @param variables_ a Vector of VarEnv
     * nb: we clone the parameter
     */
    public void setAllVariables(Vector variables_) {
        this.variables = (Vector) variables_.clone();
    }

    /**
     *Set the instructions with a hashtable
     *@param grm_ a specific hashtable
     *NB : we clone the parameter
     */
    public void setAllInstructions(Hashtable grm_) {
        this.grm = (Hashtable) grm_.clone();
    }

    /**
     *Set the grmFile to use
     *@param file absolute path for the grm file
     *NB we call setFile(File) before we replace all \\ by /
     */
    public void setFile(String file) {
        setFile(new File(file.replace('\\', '/')));
    }

    /**
     *Return the name of the file
     *@return file name, if no file set else we
     *example : english.grm
     */
    public String getName() {
        if (file != null) {
            return file.getName();
        }
        return null;
    }

    /**
     *set the grm file
     *@param file the grm file
     */
    public void setFile(File file) {
        this.file = file;
        lastModified = file.lastModified();
    }

    /**
     *Clear all directives for a given instruction.
     * @param instruction instruction to clean
     */
    public void resetDirectives(String instruction) {
        Vector v = getDirectives(instruction);
        v.clear();
    }

    /**
     *Remove a variable given its name
     *@param vName the variable to remove do nothing if the variable doesn't exist
     */
    public void removeVariable(String vName) {
        boolean stop = false;
        for (int i = 0; i < variables.size() && !stop; i++) {
            String name_ = this.getVariableName(i);
            if (name_.equals(vName)) {
                stop = true;
                variables.remove(i);
            }
        }
    }

    /**
     *Save the grm file
     *@return true if the save was correctly done false otherwise
     *NB the encoding use it's the platform encoding
     */
    public boolean save() {
        try {
            FileTools.storeString(this.write(), this.getAbsolutePath(), FileTools.ENCODING_DEFAULT);
            return true;
        } catch (Exception e) {
            log.fatal("GrmFile::ERROR ", e);
            return false;
        }
    }

    /**
     *set a directive for a given instruction
     *@param instruction the instruction to set
     *@param directive the directive to add
     *
     *Nb the instruction and the directive can be null or empty
     *example :
     *         path:
     *         GRMFILE=\\Smbhost\Mltt\ParSemDemos\XIP\grammars\english
     */
    public void setDirective(String instruction, String directive) {
        if (instruction != null && directive != null && !instruction.equals("") && !directive.equals("")) {
            Directive d = new Directive(directive);
            //d.setEnvirronement(variables);
            Vector v = this.getDirectives(instruction);
            if (!v.contains(d)) {
                v.add(d);
                if (d.isAffectation) {
                    variables.add(d.getVarEnv());
                }
            }
        }
    }

    /**
     *Return the name of a variable at the index
     *@param index of the variable in the list
     *@return the name of the variable or null
     */
    public String getVariableName(int index) {
        try {
            VarEnv var = (VarEnv) variables.get(index);
            return var.getName();
        } catch (Exception e) {
            return null;
        }
    }

    /**
     *Returne the value of the variable
     *@param index of the variable in the list
     *@return the value or null
     */
    public String getVariableValue(int index) {
        try {
            VarEnv var = (VarEnv) variables.get(index);
            return var.getValue();
        } catch (Exception e) {
            return null;
        }
    }

    /**
     *Return the variable at the index position
     *@param index of the variable
     *@return the variable or null
     */
    public VarEnv getVariable(int index) {
        try {
            VarEnv var = (VarEnv) variables.get(index);
            return var;
        } catch (Exception e) {
            return null;
        }
    }

    /**
     *Return the time of the last modification of the grm file. I mean the file
     *and not the data
     *@return long
     */
    public long lastModified() {
        return this.lastModified;
    }

    /**
     *read the grm file and setup the table or instruction
     **/
    public void read() {
        try {
            if (file == null || !file.exists()) {
                return;
            }
            RandomAccessFile in = new RandomAccessFile(file, "r");
            String line = in.readLine();
            while (line != null) {
                Enumeration e = grm.keys();
                while (e.hasMoreElements()) {
                    String instruction = (String) e.nextElement();
                    if (line.startsWith(instruction)) {
                        setDirective(instruction, in.readLine());
                    }
                }
                line = in.readLine();
            }
            in.close();
        } catch (Exception e) {
        }
    }//fin de read

    public void read(String s) {
        try {
            File f_;
            if (file != null) {
                System.out.println("i write temp file");
                f_ = File.createTempFile(file.getName(), "tmp");
            } else {
                f_ = File.createTempFile("grmFile", "tmp");
            }
            f_.deleteOnExit();
            FileTools.storeString(s, f_.getAbsolutePath(), FileTools.ENCODING_DEFAULT);
            RandomAccessFile in = new RandomAccessFile(f_, "r");
            String line = in.readLine();
            while (line != null) {
                Enumeration e = grm.keys();
                while (e.hasMoreElements()) {
                    String instruction = (String) e.nextElement();
                    if (line.startsWith(instruction)) {
                        this.setDirective(instruction, in.readLine());
                    }
                }
                line = in.readLine();
            }
            in.close();
        } catch (Exception e) {
        }
    }

    public void reset() {
        this.resetDirectives(INSTR_GRAMMAR);
        this.resetDirectives(INSTR_PATH);
        this.resetDirectives(INSTR_PARAMETER);
        this.resetDirectives(INSTR_NTM);
        this.resetDirectives(INSTR_HMM);
        this.resetDirectives(INSTR_VARIABLE);
        this.resetDirectives(INSTR_FLAGS);
        this.resetDirectives(INSTR_STRATEGY);
        this.resetDirectives(INSTR_TOKENIZER);
        this.resetDirectives(INSTR_LOOKUP);
        this.variables.clear();
    }

    /**
     *Ecrit les instructions path du fichier grm. On evalue toutes les variables utilisees dans
     *les instructions path.
     *@param endLine caractere de fin de ligne
     */
    private String writePath(String endLine) {
        Vector vars = getVariables();
        String contents = "";
        Enumeration enumer = vars.elements();
        while (enumer.hasMoreElements()) {
            VarEnv v = (VarEnv) enumer.nextElement();
            StringBuffer value = null;
            if (System.getProperty("file.separator").equals("\\")) {
                value = new StringBuffer(v.getValue().replace('/', '\\'));
            } else {
                value = new StringBuffer(v.getValue().replace('\\', '/'));
            }
            contents += INSTR_PATH + endLine + v.getName() + "=" + value + endLine;
        }
        return contents;
    }

    /**
     *Ecrit une instruction pour le fichier grm.
     *@param instruction nom de l'instruction a ecrire.
     *@param endLine caractere de fin de ligne.
     */
    private String writeInstruction(String instruction, String endLine) {
        String[] tab = getValues(instruction);
        String contents = "";
        for (int i = 0; i < tab.length; i++) {
            StringBuffer subContents = null;
            if (System.getProperty("file.separator").equals("\\")) {
                subContents = new StringBuffer(tab[i].replace('/', '\\'));
            } else {
                subContents = new StringBuffer(tab[i].replace('\\', '/'));
            }
            contents += instruction + endLine + subContents + endLine;
        }
        return contents;
    }

    /**
     *Evalue une directive, remplace une variable d'envirronement par sa valeur.
     *L'evaluation est recursive
     *@param directive 
     * @return le resultat d'une directive
     *@throws UnknownVarEnvException si une variable n'est pas trouvee.
     */
    public String eval(String directive) throws UnknownVarEnvException {
        if (directive != null) {

            String directive_ = directive;
            //on cherche les variables d'environnement dans la directive
            if (!directive_.matches("..*=..*")) {
                // String r = directive_.replaceAll("(([^\\$\\p{Upper}*/])(([^\\$\\p{Upper}*\\\\])))","");
                String r = directive_.replaceAll("[^\\$\\p{Upper}/\\\\_]", "");
                r = r.replaceAll("/.*[^\\$]?|\\\\.*[^\\$]?", "");
                // System.out.println("je garde la variable " + r );
                String[] rSplit = r.split("\\$");
                for (int i = 1; i < rSplit.length; i++) {
                    // System.out.println("j'evalue " + directive_ +  " en remplacant $" + rSplit[i] + " par " + getVariableValue(rSplit[i]));
                    String value_ = getVariableValue(rSplit[i]);
                    if (value_ != null) {
                        String new_value = eval(value_);
                        try {
                            directive_ = directive_.replaceAll("\\$" + rSplit[i], new_value);
                        } catch (IllegalArgumentException e) {
                        }
                    } else {
                        //  System.out.println(rSplit[i] + "est inconnu recherche dans l'envirronement : " + System.getenv(rSplit[i]));
                        if (projectContext != null && !rSplit[i].equals("GRM_PATHNAME")) {
                            VarEnv var = projectContext.getVariable(rSplit[i]);

                            if (var != null) {
                                //insertVariable(rSplit[i],System.getenv(rSplit[i]).replace('\\','/'),0);
                                try {
                                    System.out.println("varenv : " + var);
                                    File dir = new File(var.getValue());
                                    xip.setEnv(var.getName(), dir.getCanonicalPath());
                                    //directive_ = directive_.replaceAll("\\$" + rSplit[i],getVariableValue(rSplit[i]));
                                    directive_ = directive_.replaceAll("\\$" + rSplit[i], var.getValue());
                                } catch (Exception e) {
                                    log.warn("variable problem", e);
                                    throw new UnknownVarEnvException(rSplit[i]);
                                }
                            /** warning sur le probleme */
                            } else {
                                log.warn("variable is null : " + rSplit[i]);
                                throw new UnknownVarEnvException(rSplit[i]);
                            }
                        }
                    }

                }
            }
            return directive_;

        } else {
            return new String();
        }
    }

    /**
     *Evalue toutes les directives pour une instruction donnee
     *@param instruction 
     * @return tableau des directives evaluees
     * @throws com.xerox.xrce.xipui.exception.UnknownVarEnvException 
     */
    public String[] evalInstruction(String instruction) throws UnknownVarEnvException {
        try {
            String[] ds = getValues(instruction);
            String[] res = new String[ds.length];
            for (int i = 0; i < res.length; i++) {
                res[i] = eval(ds[i]);
            }
            return res;
        } catch (UnknownVarEnvException e) {
            throw e;
        }
    }

    public boolean check() throws UnknownVarEnvException {
        try {
            Enumeration eInstructions = grm.keys();
            while (eInstructions.hasMoreElements()) {
                String instruction = (String) eInstructions.nextElement();
                this.evalInstruction(instruction);
            }
            return true;
        } catch (UnknownVarEnvException e) {
            log.error("variable inconnue", e);
            throw e;
        } catch (Exception e) {
            log.fatal("GrmFile::ERROR", e);
            return false;
        }
    }

    /**
     *Ecrit l'objet grm tel qu'il est represente dans un fichier .grm
     *@return
     */
    public String write() {
        System.out.println("|-- Creation du fichier grm");
        String endLine = "\n";

        StringBuffer contents = new StringBuffer(); // contenu du fichier grm;

        // contents += writeInstruction(INSTR_PATH,endLine);
        contents.append(writeInstruction(INSTR_VARIABLE, endLine));
        contents.append(writePath(endLine));
        contents.append(writeInstruction(INSTR_GRAMMAR, endLine));
        contents.append(writeInstruction(INSTR_GRAM, endLine));
        contents.append(writeInstruction(INSTR_NTM, endLine));
        contents.append(writeInstruction(INSTR_HMM, endLine));
        contents.append(writeInstruction(INSTR_LOOKUP, endLine));
        contents.append(writeInstruction(INSTR_TOKENIZER, endLine));
        contents.append(writeInstruction(INSTR_STRATEGY, endLine));
        contents.append(writeInstruction(INSTR_FLAGS, endLine));
        contents.append(writeInstruction(INSTR_PARAMETER, endLine));

        return contents.toString();
    }

    /**
     *Renvoie la premiere variable utilisee dans une instruction.
     *@param instruction 
     * @return un couple (name,value), null s'il n'y a pas de variable
     */
    public VarEnv getVariableInInstruction(String instruction) {
        String directive_ = getValue(instruction);
        if (directive_ != null) {
            String varName = directive_.replaceAll("[^\\$\\p{Upper}/\\\\]", "");
            varName = varName.replaceAll("/.*[^\\$]?|\\\\.*[^\\$]?", "");
            String[] vSplit = varName.split("\\$");
            if (vSplit.length >= 2) {
                return this.getVariable(vSplit[1]);
            }
        }
        return null;
    }

    public Xip getXip() {
        return this.xip;
    }

    public ProjectContext getProjectContext() {
        return this.projectContext;
    }

    /**
     *Renvoie un clone de l'objet
     *@return copie de l'objet
     */
    public Object clone() {
        GrmFile clone = new GrmFile(getXip(), getProjectContext());
        clone.setFile(file);
        clone.setAllVariables(variables);
        clone.setAllInstructions(grm);
        return clone;
    }

    /**
     *representation de l'objet sous forme d'un String.
     *@return fait appel a la methode write.
     */
    public String toString() {
        return write();
    }

    /**
     *Renvoie la liste des variables d'envirronement
     *@return Vector de VarEnv
     */
    public Vector getVariables() {
        return this.variables;
    }

    /**
     *@return le nom absolu di fichier grm
     */
    public String getAbsolutePath() {
        String result = null;
        try {
            result = getFile().getAbsolutePath();
        } catch (NullPointerException e) {
            result = null;
        }
        return result;
    }

    /**
     *@param variableName 
     * @return la premiere variable d'envirronement qui correspond au nom passe en parametre,
     *null si pas de correspondance
     */
    public VarEnv getVariable(String variableName) {
        Enumeration e = variables.elements();
        while (e.hasMoreElements()) {
            VarEnv var = (VarEnv) e.nextElement();
            if (variableName.equalsIgnoreCase(var.getName())) {
                return var;
            }
        }
        return null;
    }

    /**
     *@param variableName 
     * @return la premiere variable d'envirronement qui correspond au nom passe en parametre,
     *null si pas de correspondance
     */
    public String getVariableValue(String variableName) {
        Enumeration e = variables.elements();
        while (e.hasMoreElements()) {
            VarEnv var = (VarEnv) e.nextElement();
            if (variableName.equalsIgnoreCase(var.getName())) {
                return var.getValue();
            }
        }
        return null;
    }

    /**
     *@param var 
     * @return la variable d'envirronement qui est egale a celle passee en parametre, null si pas de correspondance
     */
    public VarEnv getVariable(VarEnv var) {
        int index = variables.indexOf(var);
        if (index != -1 && index < CAPACITY) {
            return (VarEnv) variables.get(index);
        } else {
            return null;
        }
    }

    /**
     *Ajoute la variable d'envirronment passee en parametre
     *@param variable variable d'envirronement
     */
    public void addVariable(VarEnv variable) {
        variables.add(variable);
    }

    /**
     *Cree la variable d'environement dont le nom et la valeur sont donnes et l'ajoute a la liste
     *@param name nom de la variable
     *@param value valeur de la variable
     */
    public void addVariable(String name, String value) {
        VarEnv var = new VarEnv(name, value);
        addVariable(var);
    }

    /**
     *Insere une nouvelle variable a l'index i de l'espace envirronement des variables
     *@param variable variable a inserer, couple (name,value)
     *@param i index pour l'insertion, si l'exception ArrayIndexOutOfBoundsExcepyion est levee, alors
     * on insere la variable a la position 0.
     */
    public void insertVariable(VarEnv variable, int i) {
        try {
            variables.add(i, variable);
        } catch (ArrayIndexOutOfBoundsException e) {
            variables.add(0, variable);
        }
    }

    /**
     *Insere une nouvelle variable a l'index i de l'espace envirronement des variables
     *@param name 
     * @param value 
     * @param i index pour l'insertion, si l'exception ArrayIndexOutOfBoundsExcepyion est levee, alors
     * on insere la variable a la position 0.
     */
    public void insertVariable(String name, String value, int i) {
        VarEnv variable = new VarEnv(name, value);
        this.insertVariable(variable, i);
    }

    /**
     *Modifie une variable d'environement
     *@param name nom de la variable a modifier
     *@param value nouvelle valeur pour la variable
     */
    public void setVariable(String name, String value) {
        VarEnv v = getVariable(name);
        if (v != null) {
            v.setValue(value);
        }
    }

    public File getFile() {
        return this.file;
    }

    /**
     *@return une enumeration de toutes les instructions, les instructions sont des String
     */
    public Enumeration getInstructions() {
        return grm.keys();
    }

    /**
     *@param instruction 
     * @return Renvoie toutes les directives pour une instruction donnee sous forme de tableau de String
     */
    public String[] getValues(String instruction) {
        Vector v = getDirectives(instruction);
        String[] directives = new String[v.size()];
        for (int i = 0; i < directives.length; i++) {
            Directive d = (Directive) v.get(i);
            directives[i] = d.getValue();
        }
        return directives;
    }

    /**
     *Renvoie la premiere directive pour une instruction donnee
     *@param instruction 
     * @return premiere directive
     */
    public String getValue(String instruction) {
        Vector v = getDirectives(instruction);
        if (!v.isEmpty()) {
            Directive d = (Directive) v.firstElement();
            return d.getValue();
        }
        return null;
    }

    /**
     *@return Renvoie toutes les directives pour une instruction donnee sous forme de liste
     */
    private Vector getDirectives(String instruction) {
        try {
            return (Vector) grm.get(instruction);
        } catch (Exception e) {
            return new Vector(CAPACITY);
        }
    }

    /** Sous classe permettant de gerer une directive d'une instruction */
    private class Directive {

        // valeur de la directive
        private String value;
        //variable d'envirronement associee
        private VarEnv varEnv;
        //directive de type affectaion, creation de variable d'envirronement
        private boolean isAffectation = false;
        //directive contient une variable d'environnement
        private boolean containsVar = false;

        /**
         * construit la directive a partir d'une ligne de chaine de caractere
         */
        public Directive(String line) {
            //   envirronement = new Vector();
            value = line.replace('\\', '/');
            setIsAffectation();
            setContainsVar();
            setVar();
        }

        /**
         *Initialise l'envirronement pour cette directive
         *@param env liste des variables d'envirronement
         */
        /*public void setEnvirronement(Vector env) {
        this.envirronement = env;
        }*/
        /**
         *Initialise la variable associee a la directive
         */
        private void setVar() {
            if (isAffectation) {
                String[] v = value.split("=");
                if (v.length == 2) {
                    varEnv = new VarEnv(v[0], v[1]);
                }
            } else {
                if (containsVar && !isAffectation) {
                    String r = value.replaceAll("[^\\$\\p{Upper}*]", "");
                    varEnv = new VarEnv(r.replaceFirst("\\$", ""), "");
                    //on renseigne la valeur de la variable.
                    Iterator ite = variables.iterator();
                    while (ite.hasNext()) {
                        VarEnv var = (VarEnv) ite.next();
                        if (varEnv.getName().equalsIgnoreCase(var.getName())) {
                            varEnv.setValue(var.getValue());
                            break;
                        }
                    }
                }
            }
        }

        /**
         *Reconnait si la directive est une affectation
         */
        private void setIsAffectation() {
            if (value.matches("..*=..*")) {
                containsVar = true;
                isAffectation = true;
            } else {
                isAffectation = false;
            }
        }

        /**
         *Reconnait si la directive contient une variable d'envirronement
         */
        private void setContainsVar() {
            if (!containsVar) {
                containsVar = value.matches(".*\\$..*");
            }
        }

        /**
         *Verifie si la directive est une affectation
         */
        public boolean isAffectation() {
            return this.isAffectation;
        }

        /**
         *Verifie si la directive contient une variable
         */
        public boolean containsVar() {
            return this.containsVar;
        }

        /**
         *Renvoie la directive
         */
        public String getValue() {
            if (isAffectation) {
                return varEnv.toString();
            }
            return this.value;
        }

        public String toString() {
            return getValue();
        }

        /**
         *Renvoie la variable d'environnement
         *@return VarEnV associee a cette directive
         */
        public VarEnv getVarEnv() {
            return this.varEnv;
        }

        public boolean equals(Object obj) {
            return this.toString().equals(obj.toString());
        }
    }
}// fin de la classe
