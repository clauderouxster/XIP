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
public class Sense implements Serializable  {
    
    private int id; //id du sense
    private int idLexeme; // id du lexeme auquel appartient ce sense
    private Hashtable tableMorpho; //table des morphos pour ce sense
    private String pos; //part of speech du sense
    
    /**
     * Cr�e une instance d'un objet Sense
     *@param id id du Sense � cr�er
     */
    public Sense(int id) {
       this.id = id;
       this.tableMorpho = new Hashtable();
    }
    
    /**
     *Cr�er une instance de Sense 
     *@param id, id du Sense
     *@param idLex id du Lexeme contenant ce sense
     */
    public Sense(int id,int idLex) {
       this(id);
       this.setIdLexeme(idLex);
    }
    
    
    /**
     *Renvoie l'id de Sense
     *@return entier correspondant � l'id
     */
    public int getId(){
        return this.id;
    }
    
    /**
     *Renvoie l'id du Lexeme
     *@return entier
     */
    public int getIdLexeme() {
        return this.idLexeme;
    }
    
    /**
     *Initialise l'id du lexeme pour ce Sense
     *@param idLexeme, id du Lexeme
     */
    public void setIdLexeme(int idLexeme) {
        this.idLexeme = idLexeme;
    }
    
    /**
     *Ajoute une morpho � la table des morphos
     *@param m Morpho � rajouter
     */
    public void addMorpho(Morpho m) {
        this.tableMorpho.put(new Integer(m.getId()),m);
    }
    
    /**
     *Renvoie la table des Morphos
     *@return table des Morphos cle : id Morpho valeur l'objet Morpho
     */
    public Hashtable getTableMorpho() {
        return this.tableMorpho;
    }
    
    /**
     *Renvoie une �num�ration des id des Morpho, c'est � dire les cl�s de la table 
     *des morphos
     *@return enumeration d'entiers
     */
    public Enumeration getListIdMorpho() {
        return this.tableMorpho.keys();
   }
    
    /**
     *Renvoie une collection des morphos, c'est � dire toutes les valeurs des morphos
     *@return Collection de Morpho
     */
    public Collection getListMorpho() {
        return this.tableMorpho.values();
    }
    
    
    /**
     *Renvoie une Morpho du Sense
     *@param id, id de la Morpho � renvoyer
     *@return Morpho si trouv�, null sinon
     */
    public Morpho getMorpho(int id){
        return (Morpho)this.tableMorpho.get(new Integer(id));
    }
    
    
    /**
     *V�rifie di lengthsense contient une Morpho
     *@param m Morpho � v�rifier
     *@return true si la morpho existe, false sinon
     */
    public boolean contains(Morpho m) {
        return this.tableMorpho.containsValue(m);
    }
    
    /**
     *Initialise le part of speech du Sense
     *@param pos part of speech pour ce Sense
     */
    public void setPos(String pos) {
        this.pos = pos;
    }
    
    
    /**
     *Renvoie le part of speech du Sense
     *@return une cha�ne correspondant au part of Speech.
     */
    public String getPos() {
        return this.pos;
    }
    
    
    /***
     *Renvoie une r�pr�sentation du Sense sous la forme d'un String
     *@return String d�crivant ce Sense
     */
    public String toString() {
        StringBuffer sb = new StringBuffer("sense:"+this.getId());
        sb.append("\n\t\t POS : " + this.getPos());
        sb.append("\n\t\t Morpho ("+ this.tableMorpho.size() + ") :\n");
        Iterator ite = this.getListMorpho().iterator();
        while (ite.hasNext()) {
            Morpho m = (Morpho)ite.next();
            sb.append("\t\t\t morpho:"+m.getId()+ " " + m +  "\n");
        }
        return sb.toString();
    }
    
}//end of class
