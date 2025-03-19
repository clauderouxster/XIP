/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ParseTask.java
 *
 * Created on November 7, 2006, 4:44 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide.task;

import com.xerox.xrce.xipui.CorpusElement;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.RunContext;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.exception.ParseFileException;
import com.xerox.xrce.xipui.ide.grammar.GrammarEdit;
import com.xerox.xrce.xipui.ui.grammar.IndentFile;
import com.xerox.xrce.xipui.ide.ui.*;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.task.AbstractParsingTask;
import com.xerox.xrce.xipui.ui.Result;
import com.xerox.xrce.xipui.ui.RunManager;
import java.util.*;
import java.io.*;

/**
 *
 * @author vgrassau
 */
public class ParseTask extends AbstractParsingTask<Map<CorpusElement, Object>> {

    private boolean apply_rule = true;
    private char xml_mode;
    private boolean isIndentFile = false;
    private boolean xml_output = false;

    /** Creates a new instance of LoadProjectTask
     * @param context 
     */
    public ParseTask(ModuleContext context) {
        super(context);
        Project project = getProject();
        RunContext ctx = project.getRunContext();
        //NTM ONLY option
        apply_rule = !ctx.isNtmOnly();
        //XML mode
        xml_mode = ctx.getXMLMode();
        //XML OUTPUT :
        xml_output = ctx.isXmlOutput();
        //indent file :
        isIndentFile = ctx.isIndentFile();
    }

    @Override
    protected void parsingDone(Map<CorpusElement, Object> h) {
        RunManager runner = (RunManager) getProperty(module_run);
        runner.setEnabledAction(ParseAction.NAME_, true);
        if (h != null && !h.isEmpty()) {
            Result res = new ResultTree(getModuleContext(), idResult, h, xml_output);
            runner.addResult(res);
        } else {
            warning("no result created");
        }
    }

    /***
     *
     */
    private void parseCorpusElement(CorpusElement ce, Map<CorpusElement, Object> h, IndentFile indent) {
        Project project = getProject();
        Xip xip = project.getXip();
        setMessage(" Parsing...");
        if (ce.exists()) {
            //fichier de sortie
            File output = null;
            // fichier d'entree
            File input = null;
            input = ce.getFile();
            output = new File(project.getCorpusDirectory() + "/" + ce.getParent() + "/" + ce.getFile().getName() + ".result");
            //rajouter afin d'effacer les resultats lors de la fermeture de l'interface
            output.deleteOnExit();
            if (output.exists()) {
                output.delete();
            }
            try {
                boolean err = false;
                //write("Parsing of " + ce.getFile().getAbsolutePath() + "\n result in " + output.getAbsolutePath() + "\n encoding : " + ce.getEncoding());
                // L'API JXIP crash si on envoie un chemin d'un fichier de type //smbhost
                if (!ce.isXML()) {
                    // si le corpus n'est pas un document XML
                    err = !xip.parseFile(project.getGhandler(), input, output, xml_mode, apply_rule, ce.getEncoding());
                } else {
                    //Warning profondeur : 0 ou 2 (1 ne fonctionne pas)
                    err = !xip.parseFileXML(project.getGhandler(), input, output, 0, xml_mode, apply_rule, ce.getEncoding());
                }
                if (err) {
                    //si xip n'a pas parser le fichier correctement
                    // insertion du resultat dans la table pour le CorpusElement
                    h.put(ce, new ParseFileException(ce));
                } else {
                    //si on veut afficher le fichier d'indentation
                    if (isIndentFile) {
                        if (indent != null) {
                            if (indent.isClosedModule()) {
                                indent = new IndentFile(getModuleContext());
                                setProperty("indent_file", indent);
                            }
                            indent.readIndent(ce.getEncoding());
                        }
                    }
                    h.put(ce, output);
                }
            } catch (Exception e) {
                severe("Error with " + ce, e);
                //insertion de l'erreur
                h.put(ce, e);
            }
        } else {
            //le fichier du corpusElement n'existe pas ou a ete deplace.
            //insertion de l'erreur du fichier n'exite plus
            h.put(ce, " The file " + ce.getFile() + " was deleted or moved ");
        }
        publish(ce.getFile());
    }

    @Override
    protected Map<CorpusElement, Object> doParsing() {
        Map<CorpusElement, Object> h = new Hashtable();
        Project project = getProject();
        Xip xip = project.getXip();
        if (this.hadtoBeReloaded()) {
            //the grammar reloaded, so need to refresh the GrammarEdit Object
            publish(true);
        }
        //fix the rule limit
        try {
            project.getRunContext().applyGrammarLimit();
        } catch (Exception e) {
            publish(e);
        }
        setProgress(1);
        IndentFile indent = (IndentFile) getProperty("indent_file");
        Iterator<CorpusElement> iteCorpus = corpus.iterator();
        // parsing des corpusElement
        int numCorpus = 1;
        while (iteCorpus.hasNext() && !isCancelled()) {
            CorpusElement ce = iteCorpus.next();
            ce = ce.createSubCorpusElement();
            String res = new String();
            if (ce.isDirectory()) {
                Collection<File> collection = ce.getFiles();
                for (Iterator<File> it = collection.iterator(); it.hasNext();) {
                    File subFile = it.next();
                    CorpusElement sub = new CorpusElement(subFile);
                    sub.setEncoding(ce.getEncoding());
                    sub.setXML(sub.isXML());
                    sub.setInput(sub.isInput());
                    sub.setParent(ce.getParent());
                    parseCorpusElement(sub, h, indent);
                }
            } else {
                parseCorpusElement(ce, h, indent);
            }
            setProgress((numCorpus * 100) / corpus.size());
            numCorpus += 1;
        }
        // we need to close the stream with indent file
        if (indent != null) {
            indent.closeIndentFileStream();
        }
        return h;
    }

    @Override
    protected void processInterim(java.util.List list) {
        Object o = list.get(0);
        if (o instanceof Boolean) {
            GrammarEdit gEdit = (GrammarEdit) getProperty(module_grammar);
            if (gEdit != null) {
                gEdit.reload();
            }
        }
        if (o instanceof File) {
            File f = (File) o;
            info(f + " was parsed");
        }
        if (o instanceof Exception) {
            warning("Error : " + o);
        }
    }
}
