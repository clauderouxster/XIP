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

import java.io.*;
import java.util.*;
/**
 * Representation d'un Bean pour l'application Web
 * un label et une valeur
 * @author vgrassau
 */
public class Bean implements Serializable, Comparable {
    
     private String label ="";
     private String value ="";
        
     
     public Bean() {}
     
     /**
      *Construit un Bean : 
      *@param label label pour le bean
      *@param value valeur du bean
      */
     public Bean(String label, String value) {
         this.setLabel(label);
         this.setValue(value);
     }
     
      
     /**
      *Renvoie le label du Bean
      *@return label
      */
     public String getLabel() {
            return this.label;
       }

     /**
      *Renvoie 
      */
      public String getValue() {
           return this.value;
     }
    
       
       public void setLabel(String label) {
           this.label = label;
       }
    
       
       public void setValue(String value) {
           this.value = value;
       }
    
       public boolean equals(Object o) {
           if ( o instanceof Bean) {
               Bean toCompare = (Bean)o;
               return getLabel().equals(toCompare.getLabel()) && getValue().equals(toCompare.getValue());
           } else {
               return false;
           }
       }
    
       public int compareTo(Object o) {
           if ( o instanceof Bean) {
               Bean bean = (Bean)o;
               return bean.getLabel().compareTo(toString());
                                   
           } else {
               return -1;
           }
                   
       }
     
       public String toString() {
           return getLabel();
       }
}
