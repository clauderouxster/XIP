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

import com.xerox.xrce.xipui.Offset;
import com.xerox.xrce.xipui.modules.IModule;
import java.awt.*;
import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;
import java.io.*;

/**
 *
 * @author vgrassau
 */
public interface Editor extends IModule, CaretListener {

    /** 
     *set the encoding to use for the editor 
     *@param encoding encoding to use 
     */
    public void setEncoding(String encoding);

    /** 
     *Return the encoding used by this editor 
     *@return the encoding used
     */
    public String getEncoding();

    /**
     *Return if the editor was modififed
     *@return true if it was false otherwise
     */
    public boolean wasModified();

    /**
     *Change the size of the police to use
     *@param size the new size for the police
     */
    public void changeSizePolice(int size);

    /**
     *Save this editor
     */
    public void save();

    /**
     *Replace some text in this editor
     *@param start the begining offset of the text to clear
     *@param end 
     * @param s the new text to write
     */
    public void replace(long start, long end, String s);

    /**
     *Return the lenght of the text
     *@return the length of the text
     */
    public int getLenght();

    /***
     *Set the tool bar visible or not
     *@param b true to view the tool bar false otherwise
     */
    public void setToolBarVissible(boolean b);

    /**
     *Rend visible ou non la barre de statut de l'editeur
     *@param b boolean true visible false sinon
     */
    public void setStatusVisible(boolean b);

    /***
     *Indique que cet editeur peut etre fermable ou non. Util pour les zones de textes.
     *@param b true fermable false sinon
     */
    public void setClosable(boolean b);

    /***
     *Indique que cet editeur peut etre fermable ou non. Util pour les zones de textes.
     *@param b true fermable false sinon
     */
    public void setSavable(boolean b);

    /***
     *Fichier a editer.
     *Cette methode ne lit pas le fichier passe en parametre.
     *Elle prepare juste le fichier pour l'edition.
     *c'est util si on veut simplement creer un nouveau fichier vide, ou alors remplacer
     *le contenu d'un fichier existant, sans devoir le lire et effacer son contenu.
     *Si le fichier n'existe pas, alors on l'indique dans la barre de statut.
     *@param file_ fichier a editer
     */
    public void setFile(File file_);

    /**
     *Lance l'ecoute de l'edition. C'est a dire permet a l'editeur de gerer les
     *action de undo et redo ainsi que de paste,  cut et copy.
     */
    public void startEditing();

    /**
     *Lit le fichier et insere le contenu dans le document.
     *Cette methode fait appel a startEditing. Il est important de noter
     *que l'utilisateur ne devrait pas utiliser la fonction startEditing apres editFile.
     *Le curseur se place a la position 0.
     *
     */
    public void editFile();

    public int getCaretPosition();

    public void setIcon(Icon icon);

    public Icon getIcon();

    /**
     *Insere du texte a la fin du document que l'on edite.
     *@param text texte a inserer
     *@param style pour le texte a inserer
     */
    public void write(String text, int style);

    /**
     *Write some text from the given position
     *@param position the position to start writing
     *@param text the text to insert
     */
    public void write(long position, String text);

    /**
     *Change le style de tout le document
     *@param attr SimpleAttributSet style a adopter
     *
     */
    public void setStyle(SimpleAttributeSet attr);

    /**
     *Change le style de tout le document
     *@param font  police du texte
     *@param color couleur du texte
     *@param fontSize taille de la police
     */
    public void setStyle(String font, Color color, int fontSize);

    /**
     *Change le style d'une partie du document :
     *@param style nouveau style
     *@param start debut de la partie a changer.
     *@param end fin de la partie a changer.
     */
    public void changeStyle(int style, int start, int end);

    /**
     *Renvoie le contenu de la derniere ligne de document.
     *@return Renvoie la derniere ligne du document
     * si un probleme est capture on renvoie une chaine vide
     */
    public String getLastRow();

    /**
     *
     * @param b 
     */
    public void setEnabled(boolean b);

    /**
     *
     * @return 
     */
    public boolean isEnabled();

    /**
     *Renvoie l'offset du debut d'une ligne
     *@param line numero de la ligne
     *@return l'offset du debut de ligne
     */
    public int startOfLine(int line);

    /** renvoie l'offset de la fin de ligne
     * @param line Numero de ligne
     * @return l'offset de la fin de la ligne
     */
    public int endOfLine(int line);

    /**
     * Renvoie le numero de la ligne courante du curseur.
     * @return un entier coorespondant au numero de ligne, les numeros sont indexes de 0 a n-1
     *
     */
    public int getCurrentLine();

    /**
     *Change le style d'une partie du document :
     *@param style nouveau style
     *@param start debut de la partie a changer.
     *@param end fin de la partie a changer.
     */
    public void changeStyle(SimpleAttributeSet style, int start, int end);

    /**
     *return the text of this editor
     *@return  the text of this editor
     */
    public String getText();

    /***
     *Selectionne le texte
     *@param begin debut de la selection
     *@param end fin de la selection
     */
    public void setSelectedText(long begin, long end);

    /**
     *Positionne le caret a l'endroit pos
     *@param pos offset du caret
     */
    public void setCaretPosition(long pos);

    /**
     *Positionne le caret a l'endroit pos
     *@param pos offset du caret
     */
    public void moveCaretPosition(long pos);

    /**
     *return the text between the given offsets
     *@param start the start offset
     *@param end the end offset
     *@return  the text between the given offsets
     */
    public String getText(int start, int end);

    /*
     *Return the selected text
     *@return the selected text
     */
    public String getSelected();

    /**
     * @return 
     */
    public int getSelectedEnd();

    /***
     *Debut d'une selection
     *@return le debut de la selection
     */
    public int getSelectedStart();

    /**
     *Renvoie l'offset de selection
     *return un objet Offset indiquant les offsets de selection
     * @return 
     */
    public Offset getSelectionOffset();

    /**
     *@return renvoie le fichier editer
     */
    public File getFile();

    /**
     *@return renvoie true si le document est editable, false sinon
     */
    public boolean isEditable();

    /**
     *Return the row at the caret position
     *@return the text corresponding to row at the caret position
     */
    public String getRowAtCaret();

    /**
     * Determine si le document est editable ou non
     * @param b true implique que le document est editable
     *
     */
    public void setEditable(boolean b);

    /**
     *Save this editor under another name
     */
    public void saveAs();

    /**
     *Request the focus
     *@return true if the focus has been requested correctly
     */
    public boolean requestFocusInWindow();

    /**
     *Repaint this editor
     */
    public void repaint();
}
