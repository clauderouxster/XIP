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

import java.util.*;
import java.io.*;
/**
 *
 * @author vgrassau
 */
public class Comparison implements Comparator,Serializable  {
    
    private Object object;
    private Vector errorsComparison;
    private boolean undefined;
   
    /**
     * Creates a new instance of Comparison
     */
    public Comparison(Object object) {
      this.object = object;
      this.undefined = false;
      this.errorsComparison = new Vector();
    }
    
    
    
    public boolean isUndefined() {
        return this.undefined;
    }
    
    /**
     *Calcule les diff�rences entre cet objet et celui en param�tre. S'il existe des 
     *diff�rence, celles-ci sont stocker dans un vector. 
     *@param toCompare objet � comparer
     *@return true si les objets sont diff�rents, false sinon
     */
    public boolean setComparison(Object toCompare) {
        boolean error_ = false;
        if ( toCompare instanceof Vector && object instanceof Vector ) {
           
            Vector vObject = (Vector)object;
            Vector vToCompare = (Vector) toCompare;
            if ( vObject.size() == vToCompare.size()) {
                for (int i=0; i < vObject.size(); i++) {
                     Object o1 = vObject.get(i);
                     Object o2 = vToCompare.get(i);
                     int res = this.compare(o1,o2);
                     if ( res != 1) {
                         error_ = true;
                     } 
                } 
                return error_;
            } else {
                undefined= true;
                return true;
            } 
       } else {
            
               int res = this.compare(object,toCompare);
               if ( res != 1) {
               
                  return true;
               }  else {
                  return false;
               }
       }
    }
    
    
    /*** 
     *Renvoie les erreurs trouv�es entre deux objets.
     *@return Vector
     */
    public Vector getErrors() {
        return this.errorsComparison;
    }
    
    
    /**
     *Fait la comparaison entre deux objets et ins�re dans les erreurs trouv�es dans une liste (Vector)
     *@param o1 objet r�f�rence
     *@param o2 objet � comparer
     *@return 1 si les objets sont �gaux.
     */
    public int compare(Object o1,Object o2) {
       
        if (o1 instanceof Morpho && o2 instanceof Morpho) { //comparaison morpho
           Morpho m1 = (Morpho)o1;
           Morpho m2 = (Morpho)o2;
           if ( m1.getId() != m2.getId()) {
               errorsComparison.add(m2);
               return -1;
           } else {
               if ( m1.equals(m2)) {
                   return 1;
               } else {
                    errorsComparison.add(m2);
                    return 0;
               }
           }
       } else if (o1 instanceof Sense && o2 instanceof Sense ) {  //comparaison sense
                   boolean err = true;
                   Sense s1 = (Sense)o1;
                   Sense s2 = (Sense)o2;
                   if ( s1.getId() != s2.getId()) {
                        errorsComparison.add(new Integer(s2.getId()));
                        err= true;
                   } 
                   if ( !s1.getPos().equals(s2.getPos())) {
                        errorsComparison.add(s2.getPos());
                        err= true;
                  } 
                  if (err)
                      return -1;    
                  else 
                      return 1;
      } else if (o1 instanceof Lexeme && o2 instanceof Lexeme ){ //comparaison lexeme
                 Lexeme l1 = (Lexeme)o1;
                 Lexeme l2 = (Lexeme)o2;
                 boolean err = true;
                 if ( !l1.getLemma().equals(l2.getLemma())) {
                     errorsComparison.add(l2.getLemma());
                     err = true;
                 }
                 if (err)
                    return -1;
                 else 
                    return 1;
      } else {
            return -1;
      }
    }
    
    
    /** compare une comparaison avec une autre (fait appel � la m�thode equals par d�faut)
     *@param o objet � comparer.
     *@return true si les objets sont �gaux false sinon
     */
    public boolean equals(Object o) {
        return super.equals(o);
    }
    
}//end of class
