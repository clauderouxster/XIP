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
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.ui.editor.Styles;

import java.awt.*;
import org.apache.log4j.*;
import com.xerox.jxip.*;
import javax.swing.*;
import javax.swing.text.*;

/**
 * Objet permettant de tracer les evenements ou informations de l'application
 * @author Vianney Grassaud
 */
public class Tracer extends JScrollPane {

    private Logger log = Logger.getLogger("tracer");
    //le text edite
    private JTextPane text;
    // la representation du document
    private DefaultStyledDocument doc;
    public static final String newline = "\n";

    /** Creates a new instance of EditText */
    public Tracer() {
        super();
        try {
            doc = new DefaultStyledDocument();
            text = new JTextPane(doc);
            text.setMargin(new Insets(5, 5, 5, 5));
            text.setEditable(false);
            this.setViewportView(text);
            this.setBorder(null);
        } catch (Exception e) {
            log.fatal("ERROR TRACER", e);
        }
    }

    /** Creates a new instance of EditText
     *@param s the String to write in the tracer
     */
    public Tracer(String s) {
        this();
        writeTraceDocument(s, Styles.STYLE_TRACE);

    }

    /**
     * Renvoie la trace sous forme de texte
     * @return trace de l'application
     */
    public String getTrace() {
        return text.getText();
    }

    /**
     *Ecrit du texte dans le tracer, le style employe est STYLE_TRACE
     *@param s texte a ecrire
     *
     *@see com.xerox.xrce.lingdexip.ui.Tracer#writeTraceDocument(String,int)
     *@see com.xerox.xrce.lingdexip.ui.Tracer#writeln(String)
     *@see com.xerox.xrce.lingdexip.ui.editor.Styles
     */
    public void write(String s) {
        Logger l = Logger.getRootLogger();
        l.info(s);
        writeTraceDocument(s, Styles.STYLE_TRACE);
    }

    /**
     *Ecrit du texte en ajoutant le caractere de fin de ligne
     *@param s texte a ecrire
     *
     *@see com.xerox.xrce.lingdexip.ui.Tracer#write(String)
     */
    public void writeln(String s) {
        write(s + Tracer.newline);
    }

    /**
     *Ecrit du texte dans le tracer, le style employe est Styles.STYLE_ERROR
     *@param s texte a ecrire
     *
     *@see com.xerox.xrce.lingdexip.ui.editor.Styles
     *@see com.xerox.xrce.lingdexip.ui.Tracer#writeTraceDocument(String,int)
     */
    public void error(String s) {
        writeTraceDocument(s + Tracer.newline + Tracer.newline, Styles.STYLE_ERROR);
    }

    /**
     *Ecrit le message ainsi que l'exception. De plus, le message et la trace de l'exception
     *sont ecrits dans le fichier de log. Une notification est presenter a l'utilisateur.
     *@param msg message de l'erreur
     *@param e exception qui a ete declanchee
     *
     *@see com.xerox.xrce.lingdexip.ui.Tracer#error(String)
     *@see com.xerox.xrce.lingdexip.ui.Warning#showError(Component,String, Exception)
     */
    public void severe(String msg, Throwable e) {
        if (e instanceof XipException) {
            error(msg + Tracer.newline + e + Tracer.newline);
        } else {
            error(msg + Tracer.newline);
        }
        Tracer.error_(msg, e);
    }

    /**
     *Ecrit le message sous l'apparence d'un warning.
     *
     *@param msg message a ecrire
     *
     *@see com.xerox.xrce.lingdexip.ui.Tracer#writeTraceDocument(String,int)
     *@see com.xerox.xrce.lingdexip.ui.editor.Styles
     *@see com.xerox.xrce.lingdexip.ui.Warning#showWarning(Component,String)
     */
    public void warning(String msg) {
        writeTraceDocument(msg + Tracer.newline, Styles.STYLE_WARNING);
        Tracer.warning_(msg);
    }

    /**
     * Ecrit un message sous l'apparence d'un message.
     * Le message est ecrit dans le fichier de log.
     *
     *
     * @see com.xerox.xrce.lingdexip.ui.editor.Styles
     * @see com.xerox.xrce.lingdexip.ui.Tracer#writeTraceDocument(String,int)
     * @param msg
     */
    public void message(String msg) {
        writeTraceDocument(msg + Tracer.newline, Styles.STYLE_MESSAGE);
    }

    /**
     *Ecrit du texte dans le tracer selon une certaine apparence
     *@param s texte a ecrire
     *@param style style d'ecriture a employer, couleur, souligne, gras, italic...
     *
     *
     */
    public void writeTraceDocument(String s, int style) {
        try {
            doc.insertString(doc.getLength(), s, Styles.getStyle(style));
            text.setCaretPosition(doc.getLength());
        } catch (Exception e) {
            log.warn("ERROR :", e);
        }
    }

    /**
     *
     * @param message
     */
    static public void warning_(String message) {
        //   Logger l = Logger.getRootLogger();
        //  l.warn(message);
        message = message.replaceAll("\n|\r", "<br>");
        StringBuffer buffer = new StringBuffer("<html><body><p>" + message + "</p>");
        buffer.append("</body></html>");
        JOptionPane.showMessageDialog(null, buffer.toString(), "WARNING", JOptionPane.WARNING_MESSAGE);
    }

    /**
     *
     * @param message
     * @param e
     */
    static public void error_(String message, Throwable e) {
        StringBuffer buffer = new StringBuffer("<html><body><p>" + message);
        buffer.append("</p>");
        if (e instanceof XipException) {
            XipException xipE = (XipException) e;
            buffer.append("<p>message : error in file " + xipE.getFileName() + "<br>");
            buffer.append("at line " + xipE.getLine() + " position : " + xipE.getPosition() + "<br>");
            buffer.append("error : " + xipE.getMessage() + "</p>");
        } else {
            buffer.append("<p>" + e.toString() + "</p>");
        }
        buffer.append("</body></html>");
        JOptionPane.showMessageDialog(null, (buffer.toString()).replaceAll("\n|\r", "<br>"), "ERROR", JOptionPane.ERROR_MESSAGE);
    }
}
