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

import com.xerox.xrce.xipui.util.FileTools;
import java.io.*;
import java.text.*;
import java.util.*;
import org.apache.log4j.*;
import org.dom4j.io.*;

/**
 * CorpusElement :
 * classe permettant la gestion de fichier de corpus
 * TODO : possibilite d'identifier un CorpusElement comme un repertoire contenant des fichiers de meme nature et non un
 *        fichier uniquement. Ceci peut etre utiliser pour des gros parsing par exemple contenant beaucoup de fichier et evite d'avoir
 *        autant de CorpusElement que de fichiers.
 * @author Vianney Grassaud
 */
public class CorpusElement implements Comparable {

    /**
     *Nom du corpus par defaut
     */
    public static final String DEFAULT = "input.txt";
    /**
     *Log
     */
    private Logger log = Logger.getLogger("debug");
    /**
     *Fichier
     */
    private File file;
    /**
     *This attribute
     */
    private int countFiles = 1;
    /**
     *indique si le fichier est un document XML
     */
    private boolean isXML;
    /**
     *encodage du fichier pour lire et ecrire
     */
    private String encoding;
    /**
     *Indique si le fichier est le fichier d'input pour le projet
     */
    private boolean isInput;
    /**
     *Indique si le fichier est une partie de l'input du projet
     */
    private boolean isPartOfInput = false;
    /**
     *Nom du corpusSet auquel appartient ce CorpusElement
     */
    private String parent;
    /**
     *Description de ce CorpusElement
     */
    private String description;
    /**
     *Indique si le fichier doit etre a parser
     */
    private boolean toParse;
    private long lastModified = 0;
    private long size = 0;
    /**
     *this boolean is used to know if the user wants to write on the project xml file
     *the start line and the end line values and reuse them after.
     */
    private boolean keepValueLine = false;
    //this field is optionnal. It permits to indicate the start line when we want to analyse it
    private int startLine = -1;
    //this field is optionnal It permits to indicate the end line when we want to analyse it
    private int endLine = -1;

    /**
     *Constructeur d'un corpusElement
     *@param file fichier texte en general, representant le corpusElement
     *
     *@see com.xerox.xrce.lingdexip.CorpusElement#setFile(File)
     */
    public CorpusElement(File file) {
        setFile(file);
        setDescription("");
    }

    /**
     * Compare this object to an other object
     * We return the result of the string representation comparison.
     * @param o the Object to compare,
     *@return  if String representation of o it's a filename, so
     *          we return the result of the lexcical order between two strings
     */
    public int compareTo(Object o) {
        return toString().compareTo(o.toString());
    }

    /**
     *Set the line to start the parsing.If the isDirectory() method return true, this method does nothing
     *@param start a number -1 or 0 indicates the begining of the corpus element
     */
    public void setStartLine(int start) {
        if (!isDirectory() && (start >= -1) && start != 0) {
            this.startLine = start;
        }
    }

    /**
     *Set the line to finish the parsing. If the isDirectory() method return true, this method does nothing
     *@param end a number -1 or 0 indicates the end of file
     */
    public void setEndLine(int end) {
        if (!isDirectory() && (end >= -1) && end != 0) {
            this.endLine = end;
        }
    }

    /**
     *Return the line to finish  the parsing
     *@return a number -1 indicates the end of file
     */
    public int getEndLine() {
        return this.endLine;
    }

    /***
     *Keep the limits values for parsing for a next restarting
     *@param b true to keep, false otherwise
     */
    public void setKeepValueLine(boolean b) {
        this.keepValueLine = b;
    }

    /***
     *Check if the limits values for parsing  have to be kept for a next restarting
     *@return true to keep false otherwise (by default it's false)
     */
    public boolean isKeepValueLine() {
        return this.keepValueLine;
    }

    /**
     *Return the line to start  the parsing
     *@return a number -1 indicates the begining of the file
     */
    public int getStartLine() {
        return this.startLine;
    }

    /**
     *With the getStartLine() and the getEndLine() we
     *create a subCorpusElement of this corpus element.
     * @return 
     */
    public CorpusElement createSubCorpusElement() {
        CorpusElement sub = this;
        if (getStartLine() != -1 || getEndLine() != -1) {
            File subFile = null;
            try {
                log.info("Create sub corpus element of " + getFile() + " from " + getStartLine() + " to " + getEndLine());
                subFile = FileTools.createSubFile(getFile(), getStartLine(), getEndLine());
            } catch (Exception e) {
                log.fatal("can't create subfile from : " + getStartLine() + " to " + getEndLine(), e);
                sub = this;
            }
            if (subFile == null) {
                return this;
            } else {
                subFile.deleteOnExit();
                sub = new CorpusElement(subFile);
                sub.setXML(isXML());
                sub.setEncoding(getEncoding());
                sub.setParent(getParent());
                return sub;
            }
        }
        return sub;
    }

    /**
     *Permet d'indiquer si le corpusElement provient de l'input du projet
     *@param b true ou false
     */
    public void setIsPartOfInput(boolean b) {
        this.isPartOfInput = b;
    }

    /**
     *Construit un CorpusElement a partir d'un texte,
     *le fichier sera cree temporairement
     *Le CorpusElement sera considere comme input et une partie d'input
     *@param text texte du fichier
     *@return corpusElement temporaire
     * @throws java.lang.Exception 
     *
     *
     */
    static public CorpusElement createTempCorpusElement(String text) throws Exception {
        String prefix = "input_";
        File tmpFile = File.createTempFile(prefix, ".txt");
        FileTools.storeString(text, tmpFile.getAbsolutePath(), FileTools.ENCODING_DEFAULT);
        CorpusElement tempCe = new CorpusElement(tmpFile);
        tempCe.setParent(Project.DEFAULT_CORPUSSET);
        tempCe.setInput(true);
        tempCe.setToParse(true);
        tempCe.setIsPartOfInput(true);
        tempCe.setEncoding(FileTools.ENCODING_DEFAULT);
        return tempCe;
    }

    /**
     *Construit un CorpusElement a partir d'un texte,
     *le fichier sera cree temporairement
     *Le CorpusElement sera considere comme input et une partie d'input
     *@param ceParent 
     * @param text texte du fichier
     *@return corpusElement temporaire
     * @throws java.lang.Exception 
     *
     *
     */
    static public CorpusElement createTempCorpusElement(CorpusElement ceParent, Vector<String> text) throws Exception {
        //System.err.println("Sentences from xmlunitextrcation : \n" + text + " \n");
        File tmpFile = File.createTempFile("xml_extract_", ceParent.getFile().getName());
        FileTools.storeSentences(text, tmpFile, ceParent.getEncoding());

        CorpusElement tempCe = new CorpusElement(tmpFile);
        tempCe.setParent(ceParent.getParent());
        tempCe.setToParse(true);
        tempCe.setEncoding(ceParent.getEncoding());
        tempCe.setXML(false);
        return tempCe;
    }

    /***
     *Permet d'identifier ce CorpusElement comme input d'un projet.
     *@param b true input false sinon
     *NB : si le fichier n'exsiste pas alors on essaie de la creer.
     *
     *@see com.xerox.xrce.lingdexip.CorpusElement#getFile()
     *@see java.io.File#createNewFile()
     */
    public void setInput(boolean b) {
        try {
            File f = getFile();
            if (b && (f == null || !f.exists())) {
                f.createNewFile();
            }
        } catch (Exception e) {
            log.warn("ERROR " + e);
        }

        this.isInput = b;
    }

    /**
     *Permet d'indiquer que ce corpusElement est a parser.
     *@param b true pour parser false sinon
     */
    public void setToParse(boolean b) {
        this.toParse = b;
    }

    /**
     *Verifie l'exsitence du fichier de ce corpusElement
     *@return true si exsite false sinon
     *
     *@see java.io.File#exists()
     */
    public boolean exists() {
        return file.exists();
    }

    /**
     *Verifie si le corpusElement est a parser ou non
     *@return true si oui false sinon
     */
    public boolean isToParse() {
        return this.toParse;
    }

    /**
     *Permet d'indiquer si ce corpusElement est un document XML.
     *ceci est pratique pour changer la facon de parser les textes avec XIP
     *@param b
     *
     */
    public void setXML(boolean b) {
        this.isXML = b;
    }

    /**
     *Initialise le CorpusSet qui contient ce CorpusElement
     *@param parent nom du CorpusSet, NB : le nom d'un CorpusSet est unique.
     */
    public void setParent(String parent) {
        this.parent = parent;
    }

    /***
     *Renvoie le CorpusSet auquel ce CorpusElement appartient.
     *@return nom du CorpusSet.
     */
    public String getParent() {
        return this.parent;
    }

    /**
     *Returns the number of bytes used by this CorpusElement
     *@return taille en bytes de ce CorpusElement
     *
     *@see java.io.File#length()
     */
    public long size() {
        if (size == 0) {
            getFiles();
        } else {
            if (lastModified < file.lastModified()) {
                getFiles();
            }
        }
        return size;

    }

    /*
     *Renvoie le fichier absolue de ce corpusElement
     *@return fichier absolu
     *
     *@see java.io.File#getAbsoluteFile()
     */
    public File getFile() {
        return file.getAbsoluteFile();
    }

    /**
     *Check if the file that refer this CorpusElement is a directory or not.
     *@return true the file is a directory, false otherwise
     */
    public boolean isDirectory() {
        return file.isDirectory();
    }

    /**
     *Returns a collection of File
     *this collection contains only files and not directory found from this CorpusElement
     *if the CorpusElement doesn't refer to a directory, this method return only the file himself
     *@return a collection of files
     */
    @SuppressWarnings("empty-statement")
    public Collection<File> getFiles() {
        this.size = 0;
        Collection<File> collection = new Vector();
        addSubFile(collection, file);
        this.countFiles = collection.size();
        this.lastModified = file.lastModified();
        return collection;
    }

    /**
     *Return the number of files for this CorpusElement.
     *This method call the getFiles() method to determine the number of file
     *@return 1 if the corpusElement is not a directory or the number of files
     *@see getFiles();
     */
    public int getCountFiles() {
        if (countFiles == 1) {
            getFiles();
        } else {
            if (lastModified < file.lastModified()) {
                getFiles();
            }
        }
        return countFiles;
    }

    /**
     *Add file from the listFiles method  in a collection of file.
     *the method is called recursively until no directory or an empty directory is found.
     *@param collection, the collection to fill
     *@param f the file to list
     *
     */
    private void addSubFile(Collection<File> collection, File f) {
        if (f.isDirectory()) {
            File[] list = file.listFiles();
            for (int i = 0; i < list.length; i++) {
                addSubFile(collection, list[i]);
            }
        } else {
            // if ( !collection.contains(f)) {
            collection.add(f);
            size += f.length();
        //}
        }
    }

    /***
     *Initialise la description de ce corpusElement
     *@param description texte decrivant le corpusElement
     *
     *NB: la description est stockee par la suite dans un document XML,
     *il n'y a pas de verification sur les caracteres contenus dans cette description
     *et qui sont interdits dans un document XML, comme '<'
     */
    public void setDescription(String description) {
        this.description = description;
    }

    /**
     *Renvoie la description de ce corpusElement.
     *@return texte decrivant le CorpusElement.
     */
    public String getDescription() {
        return this.description;
    }

    /**
     * Initialise le fichier de ce corpusElement
     * Un corpusElement fait partie d'un corpus. Un corpus est en fait
     * un repertoire.
     * @param file fichier du corpusElement.
     *
     * l'encodage par defaut est celui de la plateforme
     * le fichier n'est pas a parser.
     * On essaye de verifier si le fichier est un document XML,
     * si oui l'option XML est a true.
     *
     */
    public void setFile(File file) {
        this.file = file;
        this.lastModified = file.lastModified();
        this.isXML = checkXML();
        this.encoding = FileTools.ENCODING_DEFAULT;
        this.toParse = false;
    }

    /**
     *Verifie si le CorpusElement est un fichier XML ou non
     *la verifiaction se fait en essayer de lire le document XML
     *avec Saxreader
     *@return true si il correspond a un document XML, false sinon.
     */
    private boolean checkXML() {
        boolean success = false;
        if (this.file != null && this.file.exists()) {
            try {
                SAXReader reader = new SAXReader();
                reader.read(this.file.getAbsolutePath());
                success = true;
            } catch (Exception e) {
                success = false;
            }
        }
        return success;
    }

    /**
     *Renvoie l'encodage du CorpusElement
     *@return String, ne doit pas etre null
     *
     *@see com.xerox.xrce.lingdexip.util.FileTools
     */
    public String getEncoding() {
        return this.encoding;
    }

    /**
     *intialise l'encodage du fichier
     *@param encoding encodage a uiliser.
     *si null est passe en parametre alors on utilise l'encodage de la plateforme.
     */
    public void setEncoding(String encoding) {
        if (encoding != null) {
            this.encoding = encoding;
        } else {
            this.encoding = FileTools.ENCODING_DEFAULT;
        }
    }

    /**
     *Renvoie si le CorpusElement est de type input.
     *@return true si ce corpusElement correspond a la zone de saisie false sinon.
     *
     */
    public boolean isInput() {
        return isInput;
    }

    /**
     *Verifie si le CorpusElement est une partie de l'input.
     *
     * @return 
     */
    public boolean isPartOfInput() {
        return this.isPartOfInput;
    }

    /**
     *Permet de savoir si ce CorpusElement est un document XML.
     *@return true si le CorpusELement est un fichier XML,  false sinon
     */
    public boolean isXML() {
        return this.isXML;
    }

    public String getName() {
        return this.file.getName();
    }

    /**
     * Verifie si deux corpusElements sont egaux.
     * @param o 
     * @return true si vrai, false sinon.
     * NB : deux corpusElement sont egaux si :
     * - ils ont le meme fichier absolu.
     *@see java.io.File.equals(Object)
     *@see com.xerox.xrce.lingdexip.CorpusElement#getFile()
     *
     */
    public boolean equals(Object o) {
        boolean equality = false;
        if (o instanceof CorpusElement) {
            CorpusElement o2c = (CorpusElement) o;
            equality = this.getFile().equals(o2c.getFile());
        }
        return equality;
    }

    /**
     *Representation du corpusElement sous forme d'une chaine de carcacteres
     *@return le nom du fichier.
     *@see java.io.File#getName()
     */
    public String toString() {
        return file.getName();
    }

    /**
     * Renvoie tout le texte d'un corpusElement
     *
     * @return le contenu texte du fichier represente par ce CorpusElement
     * Si une exeception est capturee alors on renvoie une chaine vide.
     * @see com.xerox.xrce.lingdexip.util.FileTools.read(File,String)
     * @see com.xerox.xrce.lingdexip.CorpusElement#getEncoding()
     */
    public String getText() {
        String text = new String();
        try {
            text = FileTools.read(file, getEncoding());
        } catch (Exception e) {
            text = new String();
        }
        return text;
    }

    /**
     *Renvoie la date sous un format de type yyyy/MM/dd -hh:mm:ss du fichier.
     *La date correspond a la creation du fichier.
     *@return date formatee
     */
    public String getDate() {
        Date date = new Date(getFile().lastModified());
        String sDate = new String();
        SimpleDateFormat sd = new SimpleDateFormat("yyyy/MM/dd -hh:mm:ss");
        return sd.format(date);
    }
}

