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
import java.io.*;
/**
 *
 * @author vgrassau
 */
public class JSPWriter {
    
    public static final int HTML_TAG_LIB = 0;
    public static final int BEAN_TAG_LIB = 1;
    public static final int LOGIC_TAG_LIB = 2;
        
    
    public static final String DECL_START = "<%@ ";
    public static final String JSP_END = "%>";
    public static final String JSP_START = "<% ";
    public static final String TAG_START = "<";
    public static final String TAG_END = ">";
    public static final String TAG_START_END= "</";
    public static final String HTML_COMMENT_ON = "<!-- ";
    public static final String HTML_COMMENT_OFF = " -->";
    
    public static final String JSP_COMMENT_ON = "<%-- ";
    public static final String JSP_COMMENT_OFF = " --%>";
    
    
    /** Creates a new instance of JSPWriter */
    public JSPWriter() {}
    
    /**
     *Write a declaration of a taglib for a jsp page 
     *example : <%@ taglib uri="http://jakarta.apache.org/struts/tags-html" prefix="html"%>
     *@param lib, indicates the lib to write
     */
    public String taglib(int lib) {
        String res = "";
        Hashtable table = new Hashtable();
        switch (lib) {
            case HTML_TAG_LIB : {
              table.put("uri","http://jakarta.apache.org/struts/tags-html");
              table.put("prefix","html");
              break;
            }
         case BEAN_TAG_LIB : {
             table.put("uri","http://jakarta.apache.org/struts/tags-bean"); 
             table.put("prefix","bean");
             break;
            }
         case LOGIC_TAG_LIB : {
             table.put("uri","http://jakarta.apache.org/struts/tags-logic");
             table.put("prefix","logic");
             break;
         }
      }
      return writeTag(this.DECL_START,this.JSP_END,"taglib",table);
    }
  
    /***
     *Writes an attribute into a tag
     *@param name the name of the attribute
     *@param value the value of the attribute
     */
    public String writeAttribute(String name,String value) {
        if (value.startsWith("'")) {
            return " " + name + "=" + value;
        } else {
          return " " + name + "=\"" + value + "\"";
        }
    }
    
    /**
     *Writes a simple tag opened 
     *example : <body class="myBody">
     *@param name the name for the tag
     *@param value Hashtable of attribute, the keys are the names of the attributes.
     */
    public String writeTagStart(String name,Hashtable values ) {
        return writeTag(this.TAG_START,this.TAG_END,name,values);
    }
    
    /***
     *Writes a simple tag closed
     *example : </body>
     *@param name the name for the tag
     */
    public String writeTagEnd(String name){
        return this.TAG_START_END + name + this.TAG_END + "\n";
    }
    
    /**
     *Writes a tag which doest not contain a body 
     *example : <link rel="stylesheet" type="text/css" href="css/exercice.css" />
     *@param the name for the tag
     *@param values the attibutes for the tag
     */
    public String writeTagWithoutBody(String name,Hashtable values) {
         return writeTag(this.TAG_START,"/" +this.TAG_END ,name,values);
    }
 
    /**
     *Creates an attribute table for a tag or other
     *@param table, the table to insert the attribute, if it's null, a new instance of hashtable is created
     *@param name the name of the attribute
     *@param value the value for the attribute
     *@retrun the hashtable containing attributes
     */
    public Hashtable createAttribute(Hashtable table,String name,String value) {
        if (table == null) {
            table = new Hashtable();
        }
        table.put(name,value);
        return table;
    }
    
 /**
  *Write a tag which the tag_style opened and close are specified by the parameter
  *@param tag_start the style of the opened tag example <% or <
  *@param tag_end the style of closed tag exameple %> or />
  *@param name the name of the tag
  *@param attibutes the attributes table for the tag
  */
    public String writeTag(String tag_start,String tag_end,String name,Hashtable attributes){
        StringBuffer tag = new StringBuffer(tag_start);
        tag.append(name);
        if (attributes != null) {
           Enumeration eAttributes = attributes.keys();
           while (eAttributes.hasMoreElements()) {
              Object attribute = eAttributes.nextElement();
              Object value = attributes.get(attribute);
              tag.append(writeAttribute(attribute.toString(),value.toString()));
           }
        }
        tag.append(tag_end + "\n");
        return tag.toString();
    }
    
    
    /***
     *Writes the JSP content into a file
     *@param file the file to write
     *@param textJSP the jsp content
     *@throw IOException if problems during the writting
     */
    public void writeInFile(File file,String textJSP) throws IOException  {
       try {
        FileWriter fw = new FileWriter(file);
        BufferedWriter bufW = new BufferedWriter(fw);
        bufW.write(textJSP);
        bufW.close();
       } catch (IOException e) {
           throw e;
       }
    }
    
    
    /**
     *Write a comment for a JSP page
     *example : <%-- a comment JSP --%> or \/* a commment JAVA *\/ or <!-- a comment HTML -->...
     *@param commentON the tag on for the comment
     *@param commentOFF the tag off for the comment
     *@param text the text for the comment
     *@return the comment 
     */
    
    public String writeComment(String commentON, String commentOFF, String text) {
        StringBuffer comment = new StringBuffer();
        comment.append(commentON);
        comment.append(text);
        comment.append(commentOFF);
        return comment.toString();
    }
    
    
    
    /**
     *Writes an HTML comment this method uses writeComment method
     *example <!-- Xerox copyright (c) -->
     *@param text the text for the comment
     *@return return the comment
     *
     */
    public String writeHTMLComment(String text) {
        return writeComment(HTML_COMMENT_ON,HTML_COMMENT_OFF,text);
        
    }
    
    
    /**
     *Writes a JSP comment, this method uses the writeComment method
     *example : <%-- iterator variable --%>
     *@param text the comment
     *@return return the comment
     */
    public String writeJSPComment(String text) {
        return writeComment(JSP_COMMENT_ON,JSP_COMMENT_OFF,text);
    }
    
    
    
    /**
     *Main method for my test
     */
    public static void main(String[] args) {
        try {
        File f = File.createTempFile("myTest",".jsp");
        StringBuffer s = new StringBuffer();
        JSPWriter writer = new JSPWriter();
        s.append(writer.taglib(writer.HTML_TAG_LIB));
        s.append(writer.taglib(writer.LOGIC_TAG_LIB));
        s.append(writer.taglib(writer.BEAN_TAG_LIB));
        s.append(writer.writeTagStart("html:html",null));
        s.append(writer.writeTagStart("head",null));
           Hashtable t = new Hashtable();
           writer.createAttribute(t,"rel","stylesheet");
           writer.createAttribute(t,"style","text/css");
           writer.createAttribute(t,"href","css/exercice.css");
           s.append(writer.writeTagWithoutBody("link",t));  
        s.append(writer.writeTagEnd("head"));
        
        s.append(writer.writeTagStart("body",null));
        
        
        
        s.append(writer.writeTagEnd("body"));        
        s.append(writer.writeTagEnd("html:html"));     
        
        writer.writeInFile(f,s.toString());
        
        } catch (Exception e) {
            System.err.println("erreur " + e.toString() );
            e.printStackTrace();
        }
    }
    
    
    
    
}// endof class
