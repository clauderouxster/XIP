/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * GoldminerEditor.java
 *
 * Created on October 18, 2006, 10:54 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.ui;

import com.xerox.xrce.xipui.Offset;
import com.xerox.xrce.xipui.goldminer.BasicElement;
import com.xerox.xrce.xipui.goldminer.GoldminerEngine;
import com.xerox.xrce.xipui.goldminer.GoldminerOffset;
import com.xerox.xrce.xipui.goldminer.Sentence;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.editor.DefaultEditor;
import java.awt.*;
import javax.swing.*;
import javax.swing.text.*;
import java.awt.event.*;
import java.util.*;

/**
 *
 * @author vgrassau
 */
public class GoldminerEditor extends DefaultEditor implements MouseListener {

    private Vector<GoldminerOffset> offsets;
    private GoldminerEngine engine;
    public static final Color STRONG = new Color(255, 35, 4, 160);
    public static final Color MEDIUM = new Color(255, 128, 0, 128);
    public static final Color NORMAL = new Color(254, 251, 137, 128);

    /** Creates a new instance of GoldminerEditor
     * @param context
     * @param encoding
     * @param wrapping 
     */
    public GoldminerEditor(ModuleContext context, String encoding, boolean wrapping) {
        super(context, encoding, wrapping, true, true);
        engine = (GoldminerEngine) getProperty("goldminerEngine");
        this.lineColor.setEnabled(false);
        this.getTextComponent().addMouseListener(this);
    }

    public void editFile() {
        super.editFile();
        Collection<GoldminerOffset> c = null;
        String path = getFile().getAbsolutePath();
        try {
            c = engine.getOffset(path);
        } catch (Exception e) {
            warning("ERROR : cannot get offests for : " + path);
            DEBUG_LOG.error("ERROR", e);
        }
        //we set the offsets and highlight
        if (c != null) {
            setOffsets(c);
        } else {
            DEBUG_LOG.error("OFFSETS a NULL");
        }
    }

    private void setOffsets(Collection<GoldminerOffset> c) {
        this.offsets = new Vector<GoldminerOffset>(c);
        highlight(offsets);
    }

    private void highlight(Collection<GoldminerOffset> c) {
        Iterator<GoldminerOffset> ite = c.iterator();
        while (ite.hasNext()) {
            GoldminerOffset gOff = ite.next();
            int start = (int) gOff.getLeft();
            int end = (int) gOff.getRight();
            SimpleAttributeSet attr = new SimpleAttributeSet();
            try {
                if (gOff.isSentenceOffset()) {
                    switch (gOff.getType()) {
                        case Sentence.MEDIUM: {
                            StyleConstants.setBackground(attr, MEDIUM);
                            break;
                        }
                        case Sentence.STRONG: {
                            StyleConstants.setBackground(attr, STRONG);
                            //StyleConstants.setForeground(attr,new Color(254,251,137));
                            break;
                        }
                        case Sentence.NORMAL: {
                            StyleConstants.setBackground(attr, NORMAL);
                            break;
                        }
                    }
                } else {
                    attr.addAttribute(SimpleAttributeSet.NameAttribute, "lemma");
                    StyleConstants.setBold(attr, true);
                    StyleConstants.setUnderline(attr, true);
                    StyleConstants.setForeground(attr, new Color(0, 0, 255, 192));
                    //  StyleConstants.setItalic(attr,gOff.isConstraint());
                    if (gOff.isConstraint()) {
                        StyleConstants.setItalic(attr, true);
                    }
                }
                doc.setCharacterAttributes(start, end - start, attr, false);
            } catch (Exception e) {
                System.out.println(e);
            }
        }
    }//end of highlight

    public void mouseClicked(MouseEvent evt) {
        Point p = evt.getPoint();
        Caret caret = this.getCaret();
        caret.setMagicCaretPosition(p);
        int pos = caret.getDot();
        StyledDocument docu = this.getStyledDocument();
        Element el = docu.getCharacterElement(pos);
        GoldminerOffset off = new GoldminerOffset(new Offset(el.getStartOffset(), el.getEndOffset()));
        int index = offsets.indexOf(off);
        if (index != -1) {
            GoldminerOffset gOffset = offsets.get(index);
            AttributeSet attr = getCharacterAttributes();
            if (attr.containsAttribute(attr.NameAttribute, "lemma")) {
                JPopupMenu menu = new JPopupMenu();
                Collection<BasicElement> c = gOffset.getElements();
                Iterator<BasicElement> ite = c.iterator();
                StringBuilder builder = new StringBuilder("<html><body>");
                Collection<String> concepts = gOffset.getConcepts();
                if (!concepts.isEmpty()) {
                    builder.append("<center><p><i>");
                    for (Iterator it = concepts.iterator(); it.hasNext();) {
                        builder.append("&nbsp;");
                        builder.append(it.next());
                        builder.append("&nbsp;");
                    }
                    builder.append("</i></p></center>");
                }
                builder.append("<ol>");
                while (ite.hasNext()) {
                    BasicElement element = ite.next();
                    builder.append("<li>");
                    builder.append("<b><font color=\"red\">");
                    builder.append(element.getLabel());
                    builder.append("&nbsp;:&nbsp;</font></b>");
                    builder.append("<font color=\"blue\">");
                    String desc = element.getDescription();
                    if (desc == null) {
                        desc = " no description";
                    }
                    builder.append(desc);
                    builder.append("</font>");
                    builder.append("</li>");
                //menu.add(element.getLabel());
                }
                builder.append("</ol></p></body></html>");
                menu.add(new JLabel(builder.toString()));
                menu.show(this.getTextComponent(), (int) p.getX(), (int) p.getY());
            }
        }
    }

    public void mouseEntered(MouseEvent evt) {
    }

    public void mouseExited(MouseEvent evt) {
    }

    public void mousePressed(MouseEvent evt) {
    }

    public void mouseReleased(MouseEvent evt) {
    }

    public void changeSizePolice(int size) {
        super.changeSizePolice(size);
        highlight(offsets);
    }
}
