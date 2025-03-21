/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * XipUtilities.java
 *
 * Created on February 26, 2007, 2:08 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui;

import com.xerox.jxip.*;
import com.xerox.xrce.xipui.exception.BadGrammarHandlerException;
import com.xerox.xrce.xipui.util.FileTools;
import org.apache.log4j.Logger;
import java.util.*;

/**
 * Some fonctionalities using a Xip instance. 
 *
 * @author vgrassau
 */
public class XipUtilities {

    static private Logger log = Logger.getLogger("xip");

    /**
     *Sets or unsets the input and output of a Xip instance in UTF8
     *@param xip the xip parser
     *@param gHandler the handler of the grammar for the parser
     *@param on 
     * @paran on true to set false to unset
     *@return true if no error, false otherwise
     */
    static public boolean setUTF8(Xip xip, int gHandler, boolean on) {
        boolean success = true;
        if (on) {
            try {
                xip.addFlag(gHandler, Xip.UTF8_INPUT);
                xip.addFlag(gHandler, Xip.DISPLAY_CONVERSION_UTF8);
            } catch (Exception ex) {
                log.error(ex);
                success = false;
            }
        } else {
            try {
                xip.removeFlag(gHandler, Xip.UTF8_INPUT);
                xip.removeFlag(gHandler, Xip.DISPLAY_CONVERSION_UTF8);
            } catch (Exception e) {
                log.error(e);
                success = false;
            }
        }
        return success;
    }

    /**
     *Splits a text input a lemmas form for each word.
     *NB : be careful with the encoding of the given text. 
     *@param xip the xip parser to use
     *@param handler the grammar handler for the xip parser to use
     *@param text  the input text to split and get the lemmas
     *@return a collection of String, each string corresponding to a lemma with no duplication of them in the collection.
     *        returns a empty collection if problem to split in lemmas.
     *@throws Exception if problem 
     *
     *NOTE : this method use the option NTM_ONLY, it means that we call a parsing on a string but without apply 
     *       the grammar. An other solution is to use the XipResult and get lemmas from the leaves.
     */
    static public Collection<String> toLemma(Xip xip, int handler, String text) throws Exception {
        Collection<String> s = new Vector();
        String flexion = xip.parseString(handler, text, Xip.XML_NONE, false);
        //System.out.println(flexion);
        String[] split = flexion.split("\\n|\\r");
        for (int i = 0; i < split.length; i++) {
            String ligne = split[i];
            ligne = ligne.trim();
            //System.out.println(ligne);
            String[] splitLigne = ligne.split("\\s");
            if (splitLigne.length > 2) {
                if (!s.contains(splitLigne[1])) {
                    s.add(splitLigne[1]);
                }
            }
        }
        return s;
    }

    /**
     *Check if a name encoding is UTF-8 
     *@param encoding 
     * @return true if is well UTF-8
     */
    static public boolean isUTF8(String encoding) {
        return encoding.equalsIgnoreCase(FileTools.UTF_8);
    }

    /**
     *Checks the ghanlder, if gHandler == -1 an exception is thrown
     *
     * @param handler
     * @throws com.xerox.xrce.xipui.exception.BadGrammarHandlerException 
     */
    static public void checkHandler(int handler) throws BadGrammarHandlerException {
        if (handler == -1) {
            throw new BadGrammarHandlerException(handler);
        }
    }

    /**
     *This method checks if a collection of features contains a given feature
     *@param features the collection of feature
     *@param feat the feature to found
     *@return true if the given feature is found, false otherwise.
     */
    static public boolean containsFeature(Collection<XipFeature> features, XipFeature feat) {
        Iterator<XipFeature> ite = features.iterator();
        boolean stop = false;
        String toCompare = feat.getName() + ":" + feat.getValue();
        while (ite.hasNext() && !stop) {
            XipFeature curFeat = ite.next();
            String value = curFeat.getName() + ":" + curFeat.getValue();
            if (value.equals(toCompare)) {
                stop = true;
            }
        }
        return stop;
    }
}
