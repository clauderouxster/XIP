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

import java.io.*;

/**
 * You need to create a class which extends of this class and implement
 * the initPage method, i.e. the content jsp for the file.
 *
 * 
 * @author vgrassau
 */
public abstract class JSPExerciceTemplate {
    
    private File fileJSP;
    protected StringBuffer page;
    protected JSPWriter writer;
    
    /** Creates a new instance of JSPExerciceTemplate 
     * by default the jsp is empty
     *@param fileName the name for the jsp file
     */
    public JSPExerciceTemplate(String fileName){
        page = new StringBuffer();
        writer = new JSPWriter();
        fileJSP = new File(fileName);
    
    } 
    
    
    /**
     *Absrtact method, you need to implement this method iin order to write the specific content 
     *for the JSP file. For this you should use the writer property of this object.
     */
    abstract protected void initPage();
    
    
    /***
     *Abstract method, an exercice must have a description, this method permit 
     *to write it in a JSP page.
     */
    abstract protected void writeDescription();
    
    /***
     *Abstract method, this method permits to write the exercice in a JSP page.
     */
    abstract protected void writeExercice();
    
    
    
    /***
     *Return the file for the jsp form exercice
     *@return the file
     */
    public File getFileJSP() {
        return this.fileJSP;
    }
       
    
    /**
     *Writes the JSP exercice, calls the method initPage
     *@throw IOException if problem
     */
    public void write() throws IOException {
       try {
           initPage();
           System.out.println("store the jsp file : " +  fileJSP.getAbsolutePath());
           if (!fileJSP.exists()) {
               fileJSP.createNewFile();
           }
       
           writer.writeInFile(fileJSP,page.toString());
           
       } catch (IOException e) {
           throw e;
       }
    }
    
}//end of the class
