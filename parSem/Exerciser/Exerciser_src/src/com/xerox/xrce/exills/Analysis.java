/** 
 ****************************************************************************************
 * Copyright (c) 2007 Xerox Corporation                                                 *
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
 * Permet d'utiliser les resultats obtenus avec une analyse. 
 * @author vgrassau
 * @version 1.5
 */
public interface Analysis extends Serializable {
  
    /**
     *Warning : Pour connaitre un nom propre en français : 
     *Il n'y a pas de categorie nom propre, la categorie est : NOUN_INV avec un trait morphologique
     * a +Proper.
     *Pour l'allemand : 
     *C'est la categorie +NOUN avec le trait +Proper
     */
    
    /**
     *Categorie des verbes conforme aux etiquettes renvoyees par FST
     */
    public final String[] VERB_POS ={"+VERP_IMP","+VERB_INF","+VERB_PAP","+VERB_PRI","+VERB_TRA","+VAFIN","+VAINF","+VAPP","+VVFIN","+VVINF","+VVPP","+PARTPAST",
                         "+VDI","+VDPAP","+VDPAST","+VDPRES","+VDPROG","+VI",
                         "+VPAP","+VPAST","+VPRES","+VPROG","+VS","+PAP_INV","+PAP_PL","+PAP_SG",
                         "+VERB_P1P2","+VERB_P3PL","+VERB_P3SG","+VERB_PRP","+PAPPL","+PAPSG","+VERBFIN","VERBIMP",
                         "+VERBIMPCL","+VERBINF","+VERBINFCL","+VERBPRP","+VERBPRPCL"
                          };
   
    /**
     *Categorie des noms conforme aux etiquettes renvoyees par FST
     */
    public final String[] NOUN_POS = {"+NOUN", "+NOUNING","+NOUN_INV","+NOUN_PL","+NOUN_SG","+NOUNSG","+NOUNPL"};
    /**
     *Categorie des ponctuations conforme aux etiquettes renvoyees par FST
     */
    public final String[] PUNCT_POS = {"+PUNCT", "+CM", "+SENT"};
    /**
     *Categorie des adjectifs conforme aux etiquettes renvoyees par FST
     */
    public final String[] ADJECTIVE_POS = {"+ADJ", "+ADJA","+ADJD","+PADJ","+ADJCMP","+ADJING","+ADJPAP","+ADJSUP","+ADJ2_INV","+ADJ2_PL","+ADJ2_SG","+ADJ_INV","+ADJ_PL","+ADJ_SG"
                          };
    /***
     *Categorie des adverbes conforme aux etiquettes renvoyees par FST
     */
    public final String[] ADVERB_POS = {"+ADV","+WADV","+ADVCMP","+ADVSUP","+COADV","+ADVADJ","+ADVINT","+ADVNEG","+ADVREL","+CONADV"};
    /**
     *Categorie des pronoms conforme aux etiquettes renvoyees par FST
     */
    public final String[] PRONOUN_POS = {"+PRON_ACC", "+PRON_DAT","+PRON_GEN","+PRON_INS","+PRON_LOC","+PRON_NOM","+PRON_VOC","+PRONONE","+PRONREL","+PRONPERS","+PRONREFL",
                          "+WPRON","+PC","+PRON_P1P2","+RELPRO","+PRONCLIT","+PRONDEM","+PRONINT","+PRONPOS","+PRONREL"};
    /**
     *Categorie des articles conforme aux etiquettes renvoyees par FST
     */
    public final String[] ARTICLE_POS = {"+PTCL","+ART","+DET","+DETREL","+WDET","+DET_SG","+DET_PL","+DETPL","+DETQUANT","+DETQUANTPL","+DETQUANTSG","+DETSG"+"+PREPDETPL","+PREPDETSG"};
    /**
     *Categorie des prepositions conforme aux etiquettes renvoyees par FST
     */
    public final String[] PREPOSITION_POS = {"+PREP","+PREPADVAS","+PREP_A","+PREP_DE","PREPDET"};
    /**
     *Categorie des Auxiliaires conforme aux etiquettes renvoyees par FST
     */
    public final String[] AUX_POS = {"+AUX","+AUXINF", "+AUXINFCL","+HAB","+HABINF","+HABINFCL","+VAUX_INF","+VAUX_P1P2","+VAUX_P3PL","+VAUX_P3SG","+VAUX_PAP","+VAUX_PRP","+VAUX","+VBI","+VBPAST","+VBPAP","+VBPRES","+VBPROG"
                                      ,"+VHI","+VHPAP","+VHPAST","+VHPRES","+VHPROG"};
    
    /**
     *Categorie des noms propres conforme aux etiquettes renvoyees par FST
     */
    public final String[] PROPER_NOUN = {"+PROP","+NOUN_INV"}; 
                                         

    /**
     *Categorie des quantifieurs conforme aux etiquettes renvoyees par FST
     */
     public final String[] QUANT_POS = {"+QUANT","+QUANTADV","+QUANTCMP","+QUANTSUP","+DETQUANT","+DETQUANTPL","+DETQUANTSG"};
    
    /** 
     *Renvoie le text qui a permis l'analyse
     *@return le texte analyse
     */
    public String getText();
    
    
    /**
     *Renvoie le numero de la phrase contenant le lexeme passe en parametre.
     *@param idLexeme index du lexeme 
     *@return numero de la phrase, -1 si la phrase est non trouvee.
     */
    public int getIdSentence(int idLexeme);
    
    /**
     *Renvoie le langage du text. 
     *Le format de la chaine de retour est defini par le LanguageGuesser
     *par defaut c'est full, c'est a dire English, French
     *@return langage
     */
    public String getLanguage();
    
    /**
     *renvoie une phrase du texte analyse
     *@param i numero de la phrase
     *@return la phrase, ou null si la phrase n'existe pas
     */
    public String getSentence(int i);
    
   /**
    *Compte le nombre de phrases dans le texte
    *@return le nombre de phrases, renvoie 0 si pas de phrases
    */
    public int countSentences();

    /**
     *Renvoie un tableau de lexemes correspondant au lemme passe en parametre
     *@param lemme a rechercher dans l'analyse
     *@return tableau des Lexemes du lemme ou null ;
     */
    public Lexeme[] getLexeme(String lemma);
   
    
    /**
     *Renvoie les lexemes pour une phrase donnee
     *@param idSentence numero de la phrase a chercher
     *@return tableau des lexemes de la phrase
     */
    public Lexeme[] getLexemeInSentence(int idSentence);
    
    /**
     *Renvoie les lexemmes pour une forme de surface donnee
     *@param surface forme de surface, ou mot.
     *@return tableu de Lexemes ou null
     */
    public Lexeme[] getLexemeFromSurface(String surface);
    
    /**
     *Compte le nombre de lexemes enregistres par l'analyse
     *@return le nombre de lexeme total, 0 si pas de lexemes
     */
    public int countLexeme();
    
    /**
     *Renvoie tous les lexemes de l'analyse dans une collection
     *@return Collection de Lexeme ou null is aucune lexeme
     */
    public Collection getAllLexeme();
    /** 
    *Renvoie le lexeme du lemme passe en parametre
    *@param idLexeme id du lexeme
    *@return Lexeme correspondant a l'id ou null si l'id du lexeme n'est n'existe pas
    */
    public Lexeme getLexeme(int idLexeme);
    
    /**
     *Definit si un Lexeme est un verb ou non
     *@param l Lexeme a verifier
     *@return true si le lexeme l est un verbe false sinon
     *
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String[])
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String)
     *@see com.xerox.xrce.exills.Analysis#containsMorpho(Lexeme, Morpho)
     */
    public boolean isVerb(Lexeme l);
    
    /**
     *Definit si un lexeme est un nom ou non
     *@param l lexeme a verifier
     *@return true si c'est un nom false sinon.
     *
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String[])
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String)
     *@see com.xerox.xrce.exills.Analysis#containsMorpho(Lexeme, Morpho)
     
     */
    public boolean isNoun(Lexeme l);
    
    /**
     *Definit si un lexeme est un adjectif
     *@param l lexeme a verirfier
     *@return true si c'est un adjectif false sinon
     *
     **@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String[])
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String)
     *@see com.xerox.xrce.exills.Analysis#containsMorpho(Lexeme, Morpho)
     
     */
    public boolean isAdj(Lexeme l);
    
    
    /**
     *Verifie si un lexeme est un nom propre
     *@param l Lexeme a verifier
     *@return true si le lexeme est un nom propre false sinon
     *
     *NB : cette methode n'assure pas la precision et l'efficacite des resultats.
     *
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String[])
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String)
     *@see com.xerox.xrce.exills.Analysis#containsMorpho(Lexeme, Morpho)
     
     */
    public boolean isProperName(Lexeme l);
    
    
    /**
     *Verifie si un lexeme est un averbe
     *@param l lexeme a verifier
     *@return true si oui, false sinon
     *
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String[])
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String)
     *@see com.xerox.xrce.exills.Analysis#containsMorpho(Lexeme, Morpho)
     
     */
    public boolean isAdverb(Lexeme l);
    
    /**
     *Verifie si un lexeme appartient a la categorie preposition.
     *@param l lexeme a verifier
     *@return true si oui false sinon
     *
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String[])
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String)
     *@see com.xerox.xrce.exills.Analysis#containsMorpho(Lexeme, Morpho)
     
     */
    public boolean isPreposition(Lexeme l);
    
    /**
     *Verifie si le lexeme est un auxilaire. 
     *@param l lexeme a verifier
     *@return true si auxiliaire, false sinon
     *
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String[])
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String)
     *@see com.xerox.xrce.exills.Analysis#containsMorpho(Lexeme, Morpho)
     
     */
    public boolean isAuxiliary(Lexeme l);
    
    
    /**
     * Verifie si le lexeme est un pronom 
     *@param l lexeme a verifier
     *@return true si pronom false sinon
     *
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String[])
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String)
     *@see com.xerox.xrce.exills.Analysis#containsMorpho(Lexeme, Morpho)
     
     */
    public boolean isPronoun(Lexeme l);
    
    /**
     *Verifie si le lexeme est un article 
     *@param l lexeme a verifier 
     *@return true si article false sinon.
     *
     *NB : les articles regroupent les determinants
     *
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String[])
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String)
     *@see com.xerox.xrce.exills.Analysis#containsMorpho(Lexeme, Morpho)
     
     */
    public boolean isArticle(Lexeme l);
    
    /**
     *Verifie si le lexeme est un quantifieur 
     *@param l lexeme a verifier 
     *@return true si article false sinon.
     *
     *NB : En français, il n'y a pas de quantifieurs. Ils sont considérés comme des avderbes
     *
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String[])
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String)
     *@see com.xerox.xrce.exills.Analysis#containsMorpho(Lexeme, Morpho)
     
     */
    public boolean isQuantifier(Lexeme l);
    
    
    /**
     *Renvoie tous les id des phrases trie par ordre croissant.
     *@return un vector contenant les id des phrases
     */
    public Vector getSentences();
  
    
    /***
     *Renvoie tous les temps des verbes 
     *@return tableau de String contenant tous les temps.
     */
    public String[] getTempsOfVerb();
    
    /**
     *Renvoie tous les modes utilises pour les verbes
     *@return table contenant les different mode utilises dans l'analyse
     */
    public String[] getModesOfVerb();
    
    /**
     *Verifie si un lexeme est une ponctuation
     *@param l lexeme a verifier
     *@return true si c'est une ponctuation false sinon
     *
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String[])
     *@see com.xerox.xrce.exills.Analysis#containsPOS(Lexeme, String)
     *@see com.xerox.xrce.exills.Analysis#containsMorpho(Lexeme, Morpho)
     
     */
    public boolean isPunct(Lexeme l);
    
    /**
     *Renvoie tous les lexemes contenant la categorie passee en parametre
     *@param pos categorie a extraire
     *@return un tableau de lexemes, renvoie null si aucun
     *
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     *
     */
    public Lexeme[] getAllPOS(String pos);
    
    
    /**
     *Renvoie tous les lexemes contenant la categorie ou les categories passees en parametre.
     *@param pos tableau de categories de part of speech
     *@return tableau de lexeme ou null si aucun lexeme n'est trouve
     *
     **@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
     */
  
    public Lexeme[] getAllPOS(String[] pos);
    /**
     *Renvoie tous les verbes de l'analyse.
     *@return tableau de Lexeme ou null si aucun
     *
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllVerb();
    
    /**
     *Renvoie tous les noms de l'analyse.
     *@return tableau de Lexeme ou null si aucun
     *
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllNoun();
    
    /**
     *Renvoie tous les adjectifs de l'analyse.
     *@return tableau de Lexeme ou null si aucun
     *
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllAdj();

     /**
     *Renvoie tous les pronoms de l'analyse.
     *@return tableau de Lexeme ou null si aucun
     *
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllPronouns();
    
     /**
     *Renvoie toutes les prepositions de l'analyse.
     *@return tableau de Lexeme ou null si aucun
     *
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllPreposition();
    
     /**
     *Renvoie tous les articles de l'analyse.
     *@return tableau de Lexeme ou null si aucun
     *
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllArticles();
    
    /**
     *Renvoie toutes les punctuations de l'analyse.
     *@return tableau de Lexeme ou null si aucun
     *
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllPunctuations();
    
     /**
     *Renvoie tous les noms propres de l'analyse.
     *@return tableau de Lexeme ou null si aucun
     *
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllProperName();
    
    /**
     *Renvoie tous les adverbes de l'analyse.
     *@return tableau de Lexeme ou null si aucun
     *
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllAdverbs();
    
    /**
     *Renvoie tous les verbes auxiliaires de l'analyse.
     *@return tableau de Lexeme ou null si aucun
     *
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllAuxiliary();
    
     /**
     *Renvoie tous les quantifieurs de l'analyse.
     *@return tableau de Lexeme ou null si aucun
     *
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllQuantifiers();
    
    
    /**
     *Verifie si un lexeme contient une morphologie.
     *@param l lexeme a verifier 
     *@param m morpho a trouver
     *
     *@return false si le lexeme n'a pas la morpho, true sinon
     */
    public boolean containsMorpho(Lexeme l,Morpho m);
    
    
    /**
     *Verifie si le lexeme contient la categorie syntaxique ou non
     *@param l lexeme a verifier 
     *@param pos categorie syntaxique a touver
     *
     *@return false si non trouvee true si trouvee
     */
    public boolean containsPOS(Lexeme l,String pos);
    
    
    /**
     *Verifie si le lexeme contient un ou plusieurs categorie syntaxiques.
     *@param l lexeme a verifier
     *@param pos tableau contenant les etiquettes des ou de la categorie
     *@return true si touvee(s), false sinon
     */
    public boolean containsPOS(Lexeme l,String[] pos);
    
    
    /**
     *Prend un tableau de Lexeme, et positionne l'attribut hidden des lexemes au booleen passe en argument
     *L'analyse renvoie le tableau modifie.
     *@param array tableau de lexeme a modifier
     *@param b valeur de l'option hidden a mettre true ou false
     *@return le tableau array modifie.
     */
    public Lexeme[] setHiddenLexeme(Lexeme[] array,boolean b);
    
    /***
     *Prend un tableau de lexemes et positionne l'attribut check des lexemes a la valeur du booleen
     *passe en parametre. L'analyse renvoie le tableau modifie.
     *@param array tableau de lexemes
     *@param b valeur a metttre pour l'attribut check des lexemes
     *@return le tableau array modifie.
     *
     */
    public Lexeme[] setCheckLexeme(Lexeme[] array,boolean b);
    
      
    /**
     *Renvoie tous les lexemes de l'analyse dans un tableau 
     *@return tous les lexemes sous forme de tableau
     */
    public Lexeme[] getAllLexemeInArray();
    
    
   
    /***
     *Ecrit un formulaire HTML permmettant de cocher ou non tous les 
     *les lexemes de l'analyse ayant l'attribut hidden a true
     *@return text sous format HTML.
     *
     *Pour des raisons ergonomique, il est preferable d'utiliser la methode writeHTMLExtraction()
     *
     *@see com.xerox.xrce.exills.Analysis#writeHTMLExtraction()
     */
    public String writeFormCheckText() throws Exception ;
    
   
    /**
     *Ecrit un formulaire HTML permettant de selectioner les lexemes 
     *de l'analyse ayant l'attribut hidden a true. 
     *L'utilisateur selectionne en fait la forme de surface afin que la lecture soit simplifiee
     *Le texte est coupe en phrase et les phrases sont affichees sous forme d'une liste numerotee.
     *
     *@return une chaine de caractere contenant du code HTML pour selectionner des mots du texte
     *@throws Exception renvoie une exception si un probleme est rencontre lors de la creation du formulaire HTML
     *
     *@see com.xerox.xrce.exills.Lexeme#isHidden()
     *@see com.xerox.xrce.exills.Lexeme#isChecked()
     *@see com.xerox.xrce.exills.Analysis#setCheckLexeme(Lexeme[],boolean)
     *@see com.xerox.xrce.exills.Analysis#setHiddenLexeme(Lexeme[],boolean)
     *@see com.xerox.xrce.exills.Analysis#getLexemeInSentence(int)
     */
    public String writeHTMLExtraction() throws Exception;
    
    
}//end of class
