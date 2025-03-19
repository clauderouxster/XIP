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

package com.xerox.xrce.exills.exercice;

import java.util.*;
import com.xerox.xrce.exills.*;
/**
 *
 * @author vgrassau
 */
public class JSPExerciceFill extends JSPExerciceTemplate {
    
    /**
     *The exerciceFill objet
     */
    private ExerciceFill exercice;
    
    /** Creates a new instance of JSPExerciceFill
     *  @param exercice
     */
    public JSPExerciceFill(String baseName,ExerciceFill exercice) {
        super(baseName + "/" + exercice.getExerciceFileName());
        this.exercice = exercice;
    }
    
    
    /**
     *Write the form
     */
    protected void initPage() {
        writeHeader();
        writeDescription();
        writeExercice();
        writeFooter();
    }
    
    
    
    /**
     *writes the exercice for level 0
     */
    private void level0() {
        Analysis a = this.exercice.getAnalysis();
        Vector sentences = a.getSentences();
        page.append("<ol>");
        for (int k=0; k < sentences.size(); k++) {
            Integer idSent = (Integer)sentences.get(k);
            page.append("<li><img src=\"images/blue-ball-mid.gif\" border=\"0\" align=\"middle\" />");
            
            Lexeme[] array = a.getLexemeInSentence(idSent.intValue());
            for (int i=0  ; i < array.length ; i++ ) {
                if ( array[i].isChecked()) {
                    page.append(this.writeBlank(array[i]));
                    if ( a.isVerb(array[i]) || a.isAuxiliary(array[i])) {
                        if ( a.getLanguage().equalsIgnoreCase("English")) {
                            page.append("<span class=\"verb\">(to " + array[i].getLemma() + ") </span> ");
                        } else {
                            page.append("<span class=\"verb\">( " + array[i].getLemma() + ") </span> ");
                        }
                    }else {
                        page.append("<span class=\"verb\">(" + array[i].getLemma() + ") </span> ");
                    }
                } else {
                    writeSurface(array[i],a);
                }
                
                
            }//fin de la boucle sur les lexemes
            page.append("<br><br></li>");
        }//fin de la boucle sur les phrases
        
        
        
        
        
    }
    
    
    private void level0_flat() {
        Analysis a = this.exercice.getAnalysis();
        Lexeme[] array  = a.getAllLexemeInArray();
        for (int i=array.length - 1 ; i >= 0; i-- ) {
            if ( array[i].isChecked()) {
                page.append(this.writeBlank(array[i]));
                if ( a.isVerb(array[i]) || a.isAuxiliary(array[i])) {
                    if ( a.getLanguage().equalsIgnoreCase("English")) {
                        page.append("<span class=\"verb\">(to " + array[i].getLemma() + ") </span> ");
                    } else {
                        page.append("<span class=\"verb\">( " + array[i].getLemma() + ") </span> ");
                    }
                }else {
                    page.append("<span class=\"verb\">(" + array[i].getLemma() + ") </span> ");
                }
            } else {
                writeSurface(array[i],a);
            }
            
        }
        
    }
    
    private String writeBlank(Lexeme l) {
        return "<html:text property=\"value(" + l.getId() +  ")\" styleClass=\"blank\" />";
    }
    
    
    
    
    
    
    private void writeSurface(Lexeme l,Analysis a) {
        if (a.isPunct(l)) {
            int index = page.length()-1;
            if(index >= 0 && page.charAt(index)== ' ')
                page.deleteCharAt(index);
            page.append(l.getSurface()+" ");
        } else {
            page.append(l.getSurface()+" ");
        }
    }
    
    
    /**
     *writes the exercice for level 1
     */
    private void level1() {
        StringBuffer lemma = new StringBuffer("<br><br> ");
        Analysis a = this.exercice.getAnalysis();
        Vector sentences = a.getSentences();
        Vector vIdLexeme = new Vector();
        page.append("<ol>");
        for (int k=0; k < sentences.size(); k++) {
            Integer idSent = (Integer)sentences.get(k);
            page.append("<li><img src=\"images/blue-ball-mid.gif\" border=\"0\" align=\"middle\" />");
            
            Lexeme[] array = a.getLexemeInSentence(idSent.intValue());
            for (int i=0  ; i < array.length ; i++ ) {
                if ( array[i].isChecked()) {
                    page.append(this.writeBlank(array[i]));
                    vIdLexeme.add(new Integer(array[i].getId()));
//              if( a.isVerb(array[i]) || a.isAuxiliary(array[i])) {
//                   if (a.getLanguage().equalsIgnoreCase("English")) {
//                   lemma.append("<span class=\"verb\">(to "+ array[i].getLemma()+ ") </span>&nbsp;");
//                   } else {
//                       lemma.append("<span class=\"verb\">( "+ array[i].getLemma()+ ") </span>&nbsp;");
//                   }
//              } else {
//               lemma.append("<span class=\"verb\">("+ array[i].getLemma()+ ") </span>&nbsp;");
//              }
                    
                } else {
                    writeSurface(array[i],a);
                }
                
                
                
            }//fin de la boucle sur les lexemes
            page.append("<br><br></li>");
        }//fin de la boucle sur les phrases
        page.append(this.writeRandomListLemma(vIdLexeme));
        
        
        
    }
    
    
    // private void level1_flat() {
//       StringBuffer lemma = new StringBuffer("<br><br> ");
//       Analysis a = this.exercice.getAnalysis();
//       Lexeme[] array  = a.getAllLexemeInArray();
//       for (int i=array.length - 1 ; i >= 0; i-- ) {
//           if ( array[i].isChecked()) {
//               page.append(this.writeBlank(array[i]));
//              if( a.isVerb(array[i]) || a.isAuxiliary(array[i])) {
//                   if (a.getLanguage().equalsIgnoreCase("English")) {
//                   lemma.append("<span class=\"verb\">(to "+ array[i].getLemma()+ ") </span>&nbsp;");
//                   } else {
//                       lemma.append("<span class=\"verb\">( "+ array[i].getLemma()+ ") </span>&nbsp;");
//                   }
//              } else {
//               lemma.append("<span class=\"verb\">("+ array[i].getLemma()+ ") </span>&nbsp;");
//              }
//
//          } else {
//                writeSurface(array[i],a);
//          }
//
//       }
//
//       page.append(lemma.toString());
    
//}
    
    
//    /**
//     *writes the exercice for level 2
//     */
//    private void level2_flat() {
//         Analysis a = this.exercice.getAnalysis();
//        Lexeme[] array  = a.getAllLexemeInArray();
//       for (int i=array.length - 1 ; i >= 0; i-- ) {
//           if ( array[i].isChecked()) {
//               page.append(this.writeBlank(array[i]));
//
//          } else {
//              writeSurface(array[i],a);
//          }
//
//       }
//    }
    
    
    /**
     *writes the exercice for level 2
     */
    private void level2() {
        Analysis a = this.exercice.getAnalysis();
        Vector sentences = a.getSentences();
        page.append("<ol>");
        for (int k=0; k < sentences.size(); k++) {
            Integer idSent = (Integer)sentences.get(k);
            page.append("<li><img src=\"images/blue-ball-mid.gif\" border=\"0\" align=\"middle\" />");
            
            Lexeme[] array = a.getLexemeInSentence(idSent.intValue());
            for (int i=0  ; i < array.length ; i++ ) {
                if ( array[i].isChecked()) {
                    page.append(this.writeBlank(array[i]));
                    
                } else {
                    writeSurface(array[i],a);
                }
                
                
            }//fin de la boucle sur les lexemes
            page.append("<br><br></li>");
        }//fin de la boucle sur les phrases
        
    }
    
    
    /***
     *
     *Writes the description of the exercice
     */
    
    protected void writeDescription() {
        Hashtable attributes = new Hashtable();
        //second line
        writeHeaderSubtitle("title.instruction","instructionID",this.exercice.getDescription());
        // third line
        attributes = writer.createAttribute(null,"id","instructionID");
        attributes = writer.createAttribute(null,"class","show");
        page.append(writer.writeTagStart("tr",attributes));
        page.append(writer.writeTagStart("td",null));
        attributes = writer.createAttribute(null,"class","panel");
        page.append(writer.writeTagStart("p",attributes));
        //the description
        
        page.append(exercice.getInstruction());
        
        page.append(writer.writeTagEnd("p"));
        page.append(writer.writeTagEnd("td"));
        page.append(writer.writeTagEnd("tr"));
    }
    
    
    
    /***
     *Writes the exercice
     */
    protected void writeExercice() {
        Hashtable attributes = new Hashtable();
        
        writeHeaderSubtitle("title.exercice","textID",null);
        attributes = writer.createAttribute(null,"id","textID");
        attributes = writer.createAttribute(null,"class","show");
        page.append(writer.writeTagStart("tr",attributes));
        page.append(writer.writeTagStart("td",null));
        attributes = writer.createAttribute(null,"class","panel");
        page.append(writer.writeTagStart("div",attributes));
        //the exercice
        
        
        if ( exercice != null ) {
            switch (exercice.getLevel()) {
                default: {
                    level0();
                    break;
                }
                case ExerciceFill.LEVEL0_ : {
                    level0();
                    break;
                }
                case ExerciceFill.LEVEL1_ : {
                    level1();
                    break;
                }
                case ExerciceFill.LEVEL2_: {
                    level2();
                    break;
                }
            }
        }
        
        page.append(writer.writeTagEnd("div"));
        page.append(writer.writeTagEnd("td"));
        page.append(writer.writeTagEnd("tr"));
        
    }
    
    
    private String writeRandomListLemma(Vector v) {
        Analysis a = this.exercice.getAnalysis();
        StringBuffer buffer = new StringBuffer();
        int size = v.size();
        Integer[] unordered = new Integer[size];
        Vector indexes = new Vector();
        Random random = new Random();
        for (int i=0; i < size; i++) {
            int index = randomIndex(size,indexes,random);
            Integer idLexeme = (Integer)v.get(index);
            Lexeme lexeme = a.getLexeme(idLexeme.intValue());
            if( a.isVerb(lexeme) || a.isAuxiliary(lexeme)) {
                if (a.getLanguage().equalsIgnoreCase("English")) {
                    buffer.append("<span class=\"verb\">(to "+ lexeme.getLemma()+ ") </span>&nbsp;");
                } else {
                    buffer.append("<span class=\"verb\">( "+ lexeme.getLemma()+ ") </span>&nbsp;");
                }
            } else {
                buffer.append("<span class=\"verb\">("+ lexeme.getLemma()+ ") </span>&nbsp;");
            }
        }
        
        
        
        return buffer.toString();
    }
    
    private int randomIndex(int size,Vector indexes,Random random) {
        Integer index = new Integer(random.nextInt(size));
        if ( indexes.contains(index) ) {
            return randomIndex(size,indexes,random);
        } else {
            indexes.add(index);
            return index.intValue();
        }
        
    }
    
    /**
     *This method writes the head for the JSP page, so it 's verbose
     */
    private void writeHeader() {
        Hashtable attributes = new Hashtable();
        page.append(writer.writeHTMLComment("****************************************************************************************"  +
                " \n Copyright (c) 2006 Xerox Corporation                                               *"  +
                " \n Xerox Research Centre Europe - Grenoble                                            *"  +
                " \n All Rights Reserved                                                                *"  +
                " \n ************************************************************************************"));
        page.append("\n");
        page.append(writer.taglib(writer.HTML_TAG_LIB));
        page.append(writer.taglib(writer.BEAN_TAG_LIB));
        page.append(writer.taglib(writer.LOGIC_TAG_LIB));
        writer.createAttribute(attributes,"import","java.util.*");
        page.append(writer.writeTag(writer.DECL_START,writer.JSP_END,"page",attributes));
        attributes =  writer.createAttribute(null,"id","submitLabel");
        writer.createAttribute(attributes,"scope","page");
        page.append(writer.writeTagStart("bean:define",attributes));
        attributes = writer.createAttribute(null,"key","label.check");
        page.append(writer.writeTagWithoutBody("bean:message",attributes));
        page.append(writer.writeTagEnd("bean:define"));
        attributes =  writer.createAttribute(null,"id","resetLabel");
        attributes = writer.createAttribute(attributes,"scope","page");
        page.append(writer.writeTagStart("bean:define",attributes));
        attributes =  writer.createAttribute(null,"key","label.reset");
        page.append(writer.writeTagWithoutBody("bean:message",attributes));
        page.append(writer.writeTagEnd("bean:define"));
        //end of the definition of beans
        
        page.append(writer.writeTagStart("html:html",null));
        page.append(writer.writeTagStart("head",null));
        page.append(writer.writeTagStart("title",null));
        page.append("Xerox exercice\n");
        page.append(writer.writeTagEnd("title"));
        attributes = writer.createAttribute(null,"rel","stylesheet");
        attributes = writer.createAttribute(attributes,"style","text/css");
        attributes = writer.createAttribute(attributes,"href","css/exercice.css");
        page.append(writer.writeTagWithoutBody("link",attributes));
        attributes =  writer.createAttribute(null,"language","javascript");
        attributes = writer.createAttribute(attributes,"src","scripts/divers.js");
        page.append(writer.writeTagStart("script",attributes));
        page.append(writer.writeTagEnd("script"));
        page.append(writer.writeTagEnd("head"));
        page.append(writer.writeTagStart("body",null));
        page.append(writer.writeTagStart("center",null));
        attributes = writer.createAttribute(null,"class","upperleft");
        page.append(writer.writeTagStart("div",attributes));
        attributes = writer.createAttribute(attributes,"class","upperright");
        page.append(writer.writeTagStart("div",attributes));
        attributes = writer.createAttribute(attributes,"class","lowerleft");
        page.append(writer.writeTagStart("div",attributes));
        attributes = writer.createAttribute(attributes,"class","lowerright");
        page.append(writer.writeTagStart("div",attributes));
        //the form :
        
        attributes = writer.createAttribute(null,"action","/doCorrect");
        attributes = writer.createAttribute(attributes,"method","get");
        attributes = writer.createAttribute(attributes,"target","center");
        page.append(writer.writeTagStart("html:form",attributes));
        attributes = writer.createAttribute(null,"property","page");
        attributes = writer.createAttribute(attributes,"value",this.getFileJSP().getName());
        page.append(writer.writeTagWithoutBody("html:hidden",attributes));
        
        attributes = writer.createAttribute(null,"property","language");
        attributes = writer.createAttribute(attributes,"value",exercice.getLanguage());
        page.append(writer.writeTagWithoutBody("html:hidden",attributes));
        
        attributes = writer.createAttribute(null,"property","author");
        attributes = writer.createAttribute(attributes,"value",exercice.getAuthor());
        page.append(writer.writeTagWithoutBody("html:hidden",attributes));
        
        attributes = writer.createAttribute(null,"property","title");
        attributes = writer.createAttribute(attributes,"value",exercice.getTitle());
        page.append(writer.writeTagWithoutBody("html:hidden",attributes));
        
        
        // the table
        attributes =  writer.createAttribute(null,"cellspacing","0");
        attributes = writer.createAttribute(attributes,"cellpadding","2");
        page.append(writer.writeTagStart("table",attributes));
        // first line
        page.append(writer.writeTagStart("tr",null));
        attributes = writer.createAttribute(null,"class","title");
        attributes = writer.createAttribute(attributes,"align","center");
        page.append(writer.writeTagStart("td",attributes));
        attributes = writer.createAttribute(null,"src","images/jsp.gif");
        attributes = writer.createAttribute(attributes,"border","0");
        attributes = writer.createAttribute(attributes,"align","middle");
        page.append(writer.writeTagWithoutBody("img",attributes));
        attributes = writer.createAttribute(null,"key","title.exercice");
        page.append(writer.writeTagWithoutBody("bean:message",attributes));
        page.append(" : " + exercice.getTitle() + " (" + exercice.getAuthor() + ") ");
        
        attributes = writer.createAttribute(null,"value","Reset");
        attributes = writer.createAttribute(attributes,"title","Reset");
        page.append(writer.writeTagWithoutBody("html:reset",attributes));
        attributes = writer.createAttribute(null,"src","images/reset.gif");
        attributes = writer.createAttribute(attributes,"border","0");
        page.append(writer.writeTagWithoutBody("html:img",attributes));
            
        
        attributes = writer.createAttribute(null,"value","Correct");
        attributes = writer.createAttribute(attributes,"title","Correct");
        page.append(writer.writeTagWithoutBody("html:submit",attributes));
        
        attributes = writer.createAttribute(null,"src","images/next.png");
        attributes = writer.createAttribute(attributes,"border","0");
        page.append(writer.writeTagWithoutBody("html:img",attributes));
        
        
        
        page.append(writer.writeTagEnd("td"));
        page.append(writer.writeTagEnd("tr"));
        
    }
    
    /**
     *Write the footer of the exercice
     */
    private void writeFooter() {
        Hashtable attributes = new Hashtable();
        page.append(writer.writeTagStart("tr",null));
        attributes = writer.createAttribute(null,"align","center");
        page.append(writer.writeTagStart("td",attributes));
        
        
        page.append(writer.writeTagEnd("td"));
        page.append(writer.writeTagEnd("tr"));
        page.append(writer.writeTagEnd("table"));
        page.append(writer.writeTagEnd("html:form"));
        page.append(writer.writeTagEnd("div"));
        page.append(writer.writeTagEnd("div"));
        page.append(writer.writeTagEnd("div"));
        page.append(writer.writeTagEnd("div"));
        page.append(writer.writeTagEnd("center"));
        page.append(writer.writeTagEnd("body"));
        page.append(writer.writeTagEnd("html:html"));
    }
    
    
    /**
     *writes a ligne of table representing a subtitle.
     *@param subtitle the subtitle
     *@id the id to collapse or expand refer to the balise <a>
     */
    private void writeHeaderSubtitle(String subtitle, String id,String param) {
        Hashtable attributes = new Hashtable();
        page.append(writer.writeTagStart("tr",null));
        attributes = writer.createAttribute(null,"class","subtitle");
        page.append(writer.writeTagStart("td",attributes));
        attributes = writer.createAttribute(null,"class","expand");
        attributes = writer.createAttribute(attributes,"href","#");
        attributes = writer.createAttribute(attributes,"onclick","'expand(\""+id +"\",\"visible\",\"relative\");'");
        attributes = writer.createAttribute(attributes,"title","expand");
        page.append(writer.writeTagStart("A",attributes));
        page.append("&nbsp;&nbsp;&nbsp;");
        page.append(writer.writeTagEnd("A"));
        attributes = writer.createAttribute(null,"class","collapse");
        attributes = writer.createAttribute(attributes,"href","#");
        attributes = writer.createAttribute(attributes,"onclick","'expand(\"" + id + "\",\"hidden\",\"absolute\");'");
        attributes = writer.createAttribute(attributes,"title","collapse");
        page.append(writer.writeTagStart("A",attributes));
        page.append("&nbsp;&nbsp;&nbsp;");
        page.append(writer.writeTagEnd("A"));
        page.append("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
        attributes = writer.createAttribute(null,"key",subtitle);
        page.append(writer.writeTagWithoutBody("bean:message",attributes));
        if ( param !=null)
            page.append(" ( "+ param+") ");
        page.append(writer.writeTagEnd("td"));
        page.append(writer.writeTagEnd("tr"));
    }
    
    
    
    
    
    
}//end of class
