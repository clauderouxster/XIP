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

package com.xerox.xrce.xipui.ui.editor;


import javax.swing.text.*;
import javax.swing.*;
import java.awt.*;


/**
 *
 * @author vgrassau
 */
public class Styles {

     /** liste des styles */
    static public final int  STYLE_ERROR  = 1;
    static public final int  STYLE_NORMAL = 0;
    static public final int  STYLE_NON_EDITABLE  = 2;
    static public final int  STYLE_RULE  = 3;
    static public final int  STYLE_MESSAGE  = 4;
    static public final int  STYLE_WARNING  = 5;
    static public final int  STYLE_TRACE  = 6;
   
    
    /** Creates a new instance of Styles */
    public Styles() {
    }
    
    
    static public SimpleAttributeSet createStyle(String font,Color color, int fontSize, boolean bold, boolean u,boolean i) {
       
         SimpleAttributeSet attr = new SimpleAttributeSet();
         StyleConstants.setFontFamily(attr, font);
         StyleConstants.setFontSize(attr, fontSize);
         StyleConstants.setForeground(attr,  color); 
         StyleConstants.setBold(attr, bold) ;
         StyleConstants.setUnderline(attr, u) ;
         StyleConstants.setItalic(attr, i) ;
         return attr;
   }
    

      static public SimpleAttributeSet createStyle(Color color, boolean bold, boolean u,boolean i) {
       
         SimpleAttributeSet attr = new SimpleAttributeSet();
        
         StyleConstants.setForeground(attr,  color); 
         StyleConstants.setBold(attr, bold) ;
         StyleConstants.setUnderline(attr, u) ;
         StyleConstants.setItalic(attr, i) ;
         return attr;
   }
    
    
    
        static public SimpleAttributeSet createStyle(Color bgColor,Color color,int size, boolean bold, boolean u,boolean i) {
       
         SimpleAttributeSet attr = new SimpleAttributeSet();
              
//    StyleConstants.setFontFamily(attr, "Arial");
         StyleConstants.setFontSize(attr, size);
         StyleConstants.setBackground(attr,  bgColor); 
         StyleConstants.setForeground(attr,  color); 
         StyleConstants.setBold(attr, bold) ;
         StyleConstants.setUnderline(attr, u) ;
         StyleConstants.setItalic(attr, i) ;
         return attr;
   }


    
    
    static public SimpleAttributeSet createStyle(String font,Color color, int fontSize) {
       
         SimpleAttributeSet attr = new SimpleAttributeSet();
         StyleConstants.setFontFamily(attr, font);
         StyleConstants.setFontSize(attr, fontSize);
         StyleConstants.setForeground(attr,  color); 
         return attr;
   }
    
    static public SimpleAttributeSet createStyle(String font,int fontSize) {
       
         SimpleAttributeSet attr = new SimpleAttributeSet();
         StyleConstants.setFontFamily(attr, font);
         StyleConstants.setFontSize(attr, fontSize);
         
         return attr;
   }
    
    static public SimpleAttributeSet getStyle(int style) {
        
        SimpleAttributeSet attr = new SimpleAttributeSet();
        //StyleConstants.setFontFamily(attr, "Arial");
        StyleConstants.setFontFamily(attr, "Monospaced");
        StyleConstants.setFontSize(attr, 12);
        StyleConstants.setForeground(attr, Color.black); 
        StyleConstants.setItalic(attr, false);
        StyleConstants.setUnderline(attr,false);
        switch(style) {
            case STYLE_ERROR:{
                 StyleConstants.setForeground(attr, Color.red); 
                 break;
            }
            case STYLE_NORMAL:{
                break;
            }
            case STYLE_NON_EDITABLE:{
               StyleConstants.setForeground(attr, new Color(125,125,125)); 
               break;
            }
            case STYLE_RULE:{
                  StyleConstants.setFontSize(attr, 14);
                  StyleConstants.setForeground(attr, Color.BLUE); 
                  break;
            }
            case STYLE_TRACE: {
                  break;
            }
            case STYLE_MESSAGE : {
                StyleConstants.setForeground(attr, Color.blue); 
               break;
            }
            case STYLE_WARNING : {
              StyleConstants.setForeground(attr, new Color(37,150,51)); 
              break;
            }
                     
        }
    
    
    return attr;
    }

    
   static public SimpleAttributeSet createHighlight(String txt,String entity) {
        SimpleAttributeSet attr = new SimpleAttributeSet();
        //StyleConstants.setBackground(attr,bgColor);
        //StyleConstants.setForeground(attr,bgColor);// bidouille
        JButton l= new JButton(txt);
        //JLabel l = new JLabel(txt);
        //l.setOpaque(true);
        //l.setBackground(bgColor);
        l.setForeground(Color.BLUE);
        l.setVerticalAlignment(SwingConstants.TOP);
        l.setToolTipText(entity);
        l.setBorder(BorderFactory.createRaisedBevelBorder());
        StyleConstants.setComponent(attr,l);
        
        
        return attr;
       
}
   
    static public SimpleAttributeSet createSimpleHighlight(Color bgColor) {
        SimpleAttributeSet attr = new SimpleAttributeSet();
        //StyleConstants.setBackground(attr,bgColor);
        StyleConstants.setForeground(attr,bgColor);
        return attr;
       
}
   
   
   
   
}
