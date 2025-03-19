/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.util.Enumeration;
import java.util.Vector;

/**
 * Description of the Class
 *
 * @author grondeau @created January 18, 2005
 */
public class InputFile {

    Vector<String> files;
    int gHandler;
    JXip jxip;
    Vector<String> filesInError;

    /**
     * Constructor for the InputFile object
     *
     * @param gHand
     * @param jx
     */
    public InputFile(JXip jx, int gHand) {
        jxip = jx;
        files = new Vector<String>();
        filesInError = new Vector<String>();
        gHandler = gHand;
    }

    /**
     * Adds a feature to the File attribute of the InputFile object
     *
     * @param file The feature to be added to the File attribute
     */
    public synchronized void addFile(String file) {
        files.add(file);
    }

    /**
     * Gets the filesInError attribute of the InputFile object
     *
     * @return The filesInError value
     */
    public synchronized Vector<String> getFilesInError() {
        return filesInError;
    }

    /**
     * Gets the allInput attribute of the InputFile object
     *
     * @return The allInput value
     * @exception Exception Description of the Exception
     */
    public synchronized String getAllInput() throws Exception {
        filesInError = new Vector<String>();
        String inputBuffer = "";
        long displayMode = Const.DISPLAY_NONE | Const.DISPLAY_SENTENCE;
        int col = 100;
        jxip.XipSetDisplayMode(gHandler, displayMode, col);
        jxip.XipSetDependencyExtraction(gHandler, false);
        for (Enumeration enumeration = files.elements(); enumeration.hasMoreElements();) {
            String fileName = (String) enumeration.nextElement();
            //System.out.println("fileName = " + fileName);
            int depth = 0;
            char xml = Const.XML_NONE;
            char apply = 1;
            inputBuffer += "\n\n---------------------------------------- "
                    + fileName + " ------------------\n\n";
            String temp = jxip.XipParseFileXmlOS(gHandler, fileName, depth, xml, apply);
            if (temp.compareTo("") == 0) {
                jxip.XipSetDependencyExtraction(gHandler, true);
                filesInError.add(fileName);
            } else {
                inputBuffer += temp;
            }
        }
        jxip.XipSetDependencyExtraction(gHandler, true);
        return inputBuffer;
    }

    /**
     *
     *
     * @param args
     */
    public static void main(String[] args) {
        JXip jxip;
        int gHand;
        try {
            jxip = new JXip();
            gHand = jxip.XipGrmFile(args[0], 0, true);

            InputFile inputFile = new InputFile(jxip, gHand);
            inputFile.addFile(args[1] + "/volvo/SkerhetTOC.rtf.ttx");
            inputFile.addFile(args[1] + "/volvo/larmTOC.rtf.ttx");
            inputFile.addFile(args[1] + "/volvo/KlimatTOC.rtf.ttx");
            String input = inputFile.getAllInput();
            Vector<String> errorFiles = inputFile.getFilesInError();
            if (errorFiles.isEmpty()) {
                if (input == null) {
                    System.out.println("ERROR");
                } else {
                    System.out.println("inputs = \n" + input);
                }
            } else {
                System.out.println("\n\nFiles in error: " + errorFiles.toString());
            }
        } catch (Exception exception) {
            String errMes = "RuleManager::main(): " + exception.toString();
            errMes += "RuleManager::main(): message = " + exception.getMessage();
            System.out.println(errMes);
        }
    }
}