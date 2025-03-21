/**
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

package com.xerox.xrce.exills;


import com.xerox.xf.*;
import java.util.*;
import java.io.*;
import com.xerox.lg.*;
import org.dom4j.*;
import java.util.logging.*;

/**
 *
 * @author vgrassau
 */
public class AnalysisFactory {
    
    // les tags du documents XML renvoyes par xf
    public static final String SENTENCE ="sentence";
    public static final String LEXEME_LIST ="lexeme-list";
    public static final String LEXEME ="lexeme";
    public static final String MORPHO_LIST ="morpho-tag-list";
    public static final String MORPHO = "morpho-tag";
    public static final String SENSE = "sense";
    public static final String SENSE_LIST ="sense-list";
    public static final String ID = "id";
    public static final String SURFACE = "surface-form";
    public static final String POS = "part-of-speech";
    public static final String LEMMA = "base-form";
    public static final String ROOT = "root";
    
    public static final Logger log = Logger.getLogger("com.xerox.xrce.exills.AnalysisFactory");
    
    
    /**
     *Execute an XF analysis on the text
     *@param language, the language to use for the analysis
     *@param text, the text to analyse
     *@return a document XML containing the analysis by XF, or null if problem
     *@throws Exception
     */
    private static String analyseXF(String language,String text) throws Exception {
        Vector availableLanguages = new Vector();
        availableLanguages.add("French");
        availableLanguages.add("french");
        availableLanguages.add("Spanish");
        availableLanguages.add("spanish");
        availableLanguages.add("English");
        availableLanguages.add("english");
        //---------------------------------
        availableLanguages.add("Czech");
        availableLanguages.add("czech");
        availableLanguages.add("Dutch");
        availableLanguages.add("dutch");
        availableLanguages.add("Greek");
        availableLanguages.add("greek");
        if (!availableLanguages.contains(language) ) {
            throw new Exception("Bad language : " + language);
        }
        
        try {
            
            String[] params = new String[4];
            
            String configXML = null;
            if ( Constants.EXERCISER_HOME != null) {
                configXML = Constants.EXERCISER_HOME + Constants.CONFIG_XF_XML;
                
                params[0] = "-data-topdir";
                params[1] = Constants.EXERCISER_HOME + Constants.DATA_TOPDIR;
                params[2] ="-lang";
                params[3] = language;
            } else {
                throw new Exception("variable EXERCISER_HOME is undefined");
            }
            
            XFXmlRuntime runtime = new XFXmlRuntime();
            
            runtime.loadConfigFile(configXML,params);
            String resXF = runtime.execute(text);
            
            
            if ( resXF == null) {
                throw new Exception("XF analysis failded");
            }
            
            StringBuffer sb = new StringBuffer(resXF);
            /****
             *WARNING :
             * we need a root tag in the xml document
             *
             */
            
            int offset = sb.indexOf("<sentence");
            sb.insert(offset,"<"+ROOT+">\n");
            sb.append("</" + ROOT + ">");
            
            return sb.toString();
        } catch (Exception e) {
            throw e;
        }
        
    }
    
    
    
    /**
     *Initialise la langue du texte, on utilise le languageGuesser
     *@param lg LanguageGuesser permettant d'identifier la langue du texte
     *@param text le texte a analyser
     *@throws Exception
     */
    private static String initLanguage(LanguageGuesser lg, String text) throws Exception {
        try {
            String language = lg.guessLanguage(text, Constants.LANGUAGE_FORM, Constants.LANGUAGE_METHODE);
            if ( language == null) {
                throw new Exception("Language Guesser couldn't find language");
            } else {
                //bogue rencontre lors de l'utilisation avec l'Exerciser.
                // xf ne veut que des languages de type : French, Engish
                if ( language.contains("_") ) {
                    String[] lSplitted = language.split("_");
                    if ( lSplitted.length > 0 ) {
                        language = lSplitted[0];
                    }
                }
                
                return language;
            }
            
        } catch (Exception e) {
            throw e;
        }
    }
    
    
    /**
     *Cree une instance d'Anlysis a partir d'un texte et d'un tableau d'objet pour des parametres eventuels
     *le premier parametre correspond a la langue du texte. Si c'est un LanguageGuesser alors on utilise ce dernier
     *avec le text afin de determiner la langue, sinon on donne la langue directement sous un format FULL e.g. : "English"
     *sous forme d'un String
     *@param param  parametres pour initialiser les objets permettant l'analyse
     *@param text text a analyser
     *@throws Exception
     *@return une analyse
     */
    
    static public Analysis getInstance(Object[] param,String text) throws Exception {
        try {
            long start,end;
            String language = null;
            String docXml = null;
            Hashtable tLexeme = null;
            LanguageGuesser lg = null;
            if ( param != null && param.length > 0 && param[0] instanceof LanguageGuesser){
                lg = (LanguageGuesser)param[0];
                System.out.println("Finding language...");
                start = System.currentTimeMillis();
                language = initLanguage(lg,text);
                end = System.currentTimeMillis();
                log.info("Text is in : " + language +" found in " + (end-start) + " ms...");
            } else if (param != null && param.length > 0 && param[0] instanceof String)  {
                language = param[0].toString();
            } else {
                throw new Exception("ERROR:AnalysisFactory : Development error in parameter");
            }
            
            start = System.currentTimeMillis();
            docXml = analyseXF(language,text);
            System.out.println(docXml);
            //  System.out.println("---------------------------------------------------------------------");
            end = System.currentTimeMillis();
            log.fine("Anlaysis succeed in " + (end-start) + " ms...");
            start = System.currentTimeMillis();
            tLexeme = readAnalysis(docXml);
            DefaultAnalysis analysis = new DefaultAnalysis(tLexeme,language);
            end = System.currentTimeMillis();
            log.fine("Parsing succeed in " + (end-start) + " ms...");
            return analysis;
        } catch (Exception e)  {
            throw e;
        }
        
    }
    
    
    /**
     *Lit l'analyse renvoyee par xf (document XML) est cree la table de hachage  des lexemes
     *@param docXml analyse de xf
     *@throws Exception
     *@return table des Lexemes
     */
    static private Hashtable readAnalysis(String docXml) throws Exception  {
        Hashtable tableLexeme = new Hashtable();
        try {
            if ( docXml != null) {
                XML xmlAnalyse = new XML(new StringReader(docXml));
                // on prend l'ensemble des noeuds sentence
                Document doc = xmlAnalyse.getDocument();
                Element root = doc.getRootElement();
                Iterator  listSentences = root.elementIterator(SENTENCE);
                
                // si la liste est vide ou null
                if (listSentences == null) {
                    throw new Exception("Could not found sentences in anaylse");
                }
                //parcours des noeuds sentences
                while (listSentences.hasNext()) {
                    Object oNode = listSentences.next();
                    if ( oNode instanceof Element) {
                        parseSentence(tableLexeme,(Element)oNode);
                    }
                }
            } else {
                throw new Exception("Parsing analysis failed");
            }
        } catch (Exception e) {
            throw e;
        }
        
        return tableLexeme;
    }
    
    
    /**
     *Parse un noeud sentence
     *@param table table des lexemes a completer
     *@elSent element XML representant une phrase
     */
    static private void parseSentence(Hashtable table,Element elSent) {
        int idSent = Integer.parseInt(elSent.attributeValue(ID));
        Element lexList = elSent.element(LEXEME_LIST);
        if ( lexList != null ) {
            Iterator ite = lexList.elementIterator(LEXEME);
            while (ite.hasNext()) {
                Element eLex = (Element)ite.next();
                int idLex = Integer.parseInt(eLex.attributeValue(ID));
                Lexeme lex = new Lexeme(idLex,idSent);
                table.put(new Integer(lex.getId()),parseLexeme(lex,eLex));
            }
        }
    }
    
    /**
     *Parse un noeud Lexeme
     *@param Lexeme lexeme a creer
     *@param elLex element XML representant un lexeme
     */
    static private Lexeme parseLexeme(Lexeme lexeme,Element elLex) {
        String surface = elLex.elementText(SURFACE);
        if ( surface !=null ) {
            lexeme.setSurface(surface);
        }
        Element senseList = elLex.element(SENSE_LIST);
        if ( senseList != null ) {
            Iterator ite = senseList.elementIterator(SENSE);
            while (ite.hasNext()) {
                Element eSence = (Element)ite.next();
                lexeme.addSense(parseSense(lexeme,eSence));
            }
        }
        return  lexeme;
    }
    
    
    /**
     *Parse un noeud morpho
     *@param idLexeme id du lexeme que l'on traite
     *@param idSense id du Sense que l'on traite
     *@param elMorpho element XMl representant une Morpho
     *@return Morpho
     */
    static private Morpho parseMorpho(int idLexeme, int idSense ,Element elMorpho) {
        int idMorpho = Integer.parseInt(elMorpho.attributeValue(ID));
        Morpho morpho = new Morpho(idMorpho,idSense,idLexeme,elMorpho.getText());
        return morpho;
        
    }
    
    
    /**
     *parse un noeud sense
     *@param lexeme Lexeme que l'on traite
     *@param elSense element XML representant un Sense
     *@return renvoie l'objet Sense associe
     */
    static private Sense parseSense(Lexeme lexeme,Element elSense) {
        int idSense = Integer.parseInt(elSense.attributeValue(ID));
        Sense sense = new Sense(idSense,lexeme.getId());
        sense.setPos(elSense.elementText(POS));
        lexeme.setLemma(elSense.elementText(LEMMA));
        
        Element morphoList = elSense.element(MORPHO_LIST);
        if ( morphoList != null) {
            Iterator ite = morphoList.elementIterator(MORPHO);
            while (ite.hasNext()) {
                Element eMorpho = (Element)ite.next();
                sense.addMorpho(parseMorpho(lexeme.getId(),idSense,eMorpho));
            }
        }
        return sense;
    }
    
    
    /**
     *Fonction de test
     */
    static public void main(String[] args) {
        try {
            long start, end;
//           File f = new File("c:/litigation/mulcahy.txt");
//           RandomAccessFile in = new RandomAccessFile(f,"r");
//           byte[] buffer = new byte[(int)f.length()];
//           in.readFully(buffer);
//           in.close();
//           String text = new String(buffer);
//           System.out.println("creation de l'analyse");
            start = System.currentTimeMillis();
            
            LanguageGuesser lg = new LanguageGuesser();
            
            lg.initGuesser();
            Object[] param = new Object[] {lg};
            
            //Object[] param = new Object[] {"English"};
            
            Analysis a = AnalysisFactory.getInstance(param,"le chien mange deux os.");
            
            
            end = System.currentTimeMillis();
            System.out.println("\n-----------------------------\n");
            System.out.println("analyse cree en  " + (end -start) + " ms");
            System.out.println(a + "\n-----------------------------\n" + a.getText());
            
        } catch (Exception e) {
            System.err.println(e);
            e.printStackTrace();
        }
    }
}
