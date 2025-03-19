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
package com.xerox.xrce.xipui.util;

import java.io.*;

import java.util.*;
import javax.xml.transform.*;
import javax.xml.transform.stream.*;
import org.dom4j.*;
import org.dom4j.io.*;
import org.dom4j.tree.*;

/**
 *
 * @author Vianney Grassaud
 */
public class XML {

    private Document xmlDoc; // le document xml

    private Document newDoc; // le document xml resultant de la transformation xsl

    // Pour ecrire un document xml
    private Element root; // element root du document

    private Element ref; //dernier element reference

    // pour XPATH
    private Node node; //noeud d'un document

    private List<Node> nodeList; // litse de noeud


    /** Constructeur par defaut */
    public XML() {
    }

    /**
     * cree le Document correspondant au document XML contenu a l'URL
     * passee en parametre
     * @param xmlFileURL l'URL du doument XML a traiter
     * @exception Exception
     */
    public XML(String xmlFileURL) throws Exception {
        this(new File(xmlFileURL));
    }

    /**
     * cree le Document correspondant au document XML 
     * passee en parametre
     * @param xmlFile the XML document
     * @exception Exception
     */
    public XML(File xmlFile) throws Exception {
        try {
            SAXReader xmlReader = new SAXReader();
            xmlDoc = xmlReader.read(xmlFile.toURI().toString());
            root = xmlDoc.getRootElement();
            ref = root;
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * cree le Document correspondant au document XML contenu dans le fux
     * passe en parametre
     * @param xmlStream le flux ou l'on recoit le doument XML a traiter
     * @exception Exception
     */
    public XML(InputStream xmlStream) throws Exception {
        try {
            SAXReader xmlReader = new SAXReader();
            xmlDoc = xmlReader.read(xmlStream);
        } catch (Exception e) {
            throw e;
        }
    }

    public XML(StringReader xmlStream) throws Exception {
        try {
            SAXReader xmlReader = new SAXReader();
            xmlDoc = xmlReader.read(xmlStream);
        } catch (Exception e) {
            throw e;
        }
    }

    public void setDOCTYPE(String name, String publicId, String SystemId) throws Exception {
        try {
            DefaultDocumentType docType = new DefaultDocumentType(name, publicId, SystemId);
            xmlDoc.setDocType(docType);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * Cree la racine du document
     * @param racine racine du document XML que l'on veut ecrire
     * @exception Exception
     */
    public void setRoot(String racine) throws Exception {
        try {
            DocumentFactory xml = new DocumentFactory();
            root = xml.createElement(racine);
            xmlDoc = xml.createDocument();
            xmlDoc.setRootElement(root);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * Tansforme le Document XML en lui appliquant la feuille de style XSLT
     * contenue a l'URL
     * @param xsltFileURL l'URL de la feuille de style XSLT
     * @throws java.lang.Exception 
     */
    public void transform(String xsltFileURL) throws Exception {
        try {
            TransformerFactory factory = TransformerFactory.newInstance();
            Transformer transformer = factory.newTransformer(new StreamSource(xsltFileURL));

            DocumentSource source = new DocumentSource(xmlDoc);
            DocumentResult result = new DocumentResult();
            transformer.transform(source, result);
            newDoc = result.getDocument();
        } catch (Exception e) {
            System.err.println("Error with XSLT transformation");
            e.printStackTrace();
            //assure that the transformation failed
            newDoc = null;
            throw e;
        }
    }

    /**
     * Return the XML document transformed by the XSLT stylesheet
     * @return a String result of the XSLT procedure or null if no transformation was done
     */
    public String getTransformed() {
        String result = null;
        if (newDoc != null) {
            result = newDoc.asXML();
        }
        return result;
    }

    /**
     * Select only one Node in the XML document
     * @param xpathExp the xpath expression to match
     * @throws java.lang.Exception 
     */
    public void xpath(String xpathExp) throws Exception {
        XPath xpathSelector = null;
        node = null;
        try {
            xpathSelector = xmlDoc.createXPath(xpathExp);
            if (xpathSelector != null) {
                node = xpathSelector.selectSingleNode(xmlDoc);
            }
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * Return the value of the selected Node by the xpath method
     * @return the String value of the Node or null, if no Node selected
     */
    public String getNode() {
        String value = null;
        if (node != null) {
            value = node.getStringValue();
        }
        return value;
    }

    /**
     *Return the String value of the selected Node with a given xpath expression
     *@param path the xpath expression
     *@return the string value of the selected Node or null, if the Node has not found
     *@throws java.lang.Exception 
     * @see getNode()
     *@see xpath(String)
     */
    public String getNode(String path) throws Exception {
        try {
            xpath(path);
        } catch (Exception e) {
            throw e;
        }
        return getNode();
    }

    /**
     *Return the selected node by xpath method as an XML node String representation
     *@return The String representation of the node or null, if no node selected
     */
    public String getNodeAsXML() {
        String value = null;
        if (node != null) {
            value = node.asXML();
        }
        return value;
    }

    /**
     *Return the value of an attribute from the given node
     *@param n the node extract the attribute value;
     *@param name the name of the attribute to find.
     *@return The value of attribute or an empty string if not found
     */
    public static String getValueAttribute(Node n, String name) {
        String value = new String();
        if (n.getNodeTypeName().equalsIgnoreCase("Element")) {
            Element el = (Element) n;
            value = el.attributeValue(name);
        }
        return value;
    }

    /**
     * Select several node from an xpath expression
     * @param xpathExp the xpath expression to find
     * @throws Exception if the xpath expression is invalid or not refer to a Node
     * @see getNodeList()
     */
    public void xpathNodes(String xpathExp) throws Exception {
        nodeList = null;
        try {

            /* on cree le chemin XPATH permettant de selectionner le noeud voulu */
            XPath xpathSelector = xmlDoc.createXPath(xpathExp);
            /* on recupere le noeud selectionne */
            if (xpathSelector != null) {
                nodeList = xpathSelector.selectNodes(xmlDoc);
            }
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * retourne une liste de Noeuds,
     * selectionne apres avoir appele la methode xpathNodes
     * @return Vector de String
     * @throws java.lang.Exception 
     */
    public Vector<Node> getNodeList() throws Exception {
        try {
            Vector<Node> liste = null;
            if (nodeList != null) {
                liste = new Vector(nodeList.size(), 1);
                Iterator<Node> i = nodeList.iterator();
                while (i.hasNext()) {
                    liste.add(i.next());
                }
            }
            return liste;
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * retourne un String, correspondant a un sous-arbre
     * selectionne apres avoir appele la methode xpathNodes
     * @return String xml
     */
    public String getNodeListAsXML() {

        StringBuilder XML = new StringBuilder();
        if (nodeList != null) {
            Iterator<Node> i = nodeList.iterator();
            while (i.hasNext()) {
                XML = XML.append(i.next().asXML());
            }
        }
        return XML.toString();
    }

    /**
     * Add a new Element to the root element
     * Element becomes the current Element
     * @param name 
     * @param Value the text value for the Element
     */
    public void addNode(String name, String Value) {
        ref = root.addElement(name);
        setText(Value);
    }

    /**
     * Add a Element to the root element
     * The Element becomes the current Element
     * @param name 
     */
    public void addNode(String name) {
        //ref = root.addElement(Name);
        addNode(name, null);
    }

    /**
     * Add an  Element on the current Element,
     * the Element becomes the current Element after
     * @param name the name for the tag
     * @param value the text value for this element
     *
     * @see add(String)
     * @see setText(String)
     *
     */
    public void add(String name, String value) {
        ref = ref.addElement(name);
        setText(value);
    }

    /**
     * Add a new Element on the current Element
     * The new Element becomes the current Element
     * @param name the name for the new Element to add
     */
    public void add(String name) {
        add(name, null);
    }

    /**
     * Set a text value to the current Element
     * @param txt the text value to assign, if txt is null do nothing
     */
    public void setText(String txt) {
        if (txt != null && ref != null) {
            ref.setText(txt);
        }
    }

    /**
     * The current Element becomes the parent Element if itself
     * if this node supports the parent relationship
     * or null if it is the root element or does not support the parent relationship.
     */
    public void parent() {
        ref = ref.getParent();
    }

    /**
     * Try to apply i times the method parent() on the current Element
     * @param i the number of occurences to apply parent method
     */
    public void parent(int i) {
        for (int j = 0; j < i; j++) {
            ref = ref.getParent();
        }
    }

    /**
     * Add an attribute to the current Element
     * @param name 
     * @param value 
     */
    public void addAttribute(String name, String value) {
        ref = ref.addAttribute(name, value);
    }

    /**
     * Return the XML document
     * @return an XML document
     */
    public Document getDocument() {
        return xmlDoc;
    }

    /**
     * Move the current Element in the XML document from an xpath expression.
     * If the element refered by the xpath is found, it becomes the current Element
     * @param path the xpath expression to select an element
     *@throws java.lang.Exception 
     * @see moveCurrent(String,Context)
     */
    public void moveCurrent(String path) throws Exception {
        moveCurrent(path, xmlDoc);
    }

    /**
     * Return the current Element
     * @return le dernier element reference
     */
    public Element getCurrent() {
        return ref;
    }

    /**
     * Move the current Element int a given context (an xml Node or a list of nodes) from an xpath expression.
     * If the element refered by the xpath is found, it becomes the current Element
     * @param path the xpath expression to select an element
     *@param context - is either a node or a list of nodes on which to evalute the XPath
     * @throws Exception if the xpath is invalid, the Node found is not an Element, if the xpath don't
     * refer to an xml node.
     *@see moveCurrent(String,Context)
     */
    public void moveCurrent(String path, Object context) throws Exception {
        try {
            /* on cree le chemin XPATH permettant de selectionner le noeud voulu */
            XPath xpathSelector = xmlDoc.createXPath(path);
            /* on recupere le noeud selectionne */
            ref = (Element) xpathSelector.selectSingleNode(context);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * Return the XML document in String representation
     * @return the full XML document in a String
     */
    public String getXML() {
        String value = null;
        if (xmlDoc != null) {
            value = xmlDoc.asXML();
        }
        return value;
    }

    /**
     *Remove an Element from an XPath expression
     *It calls the moveCurrent method
     *@param xpath the xpath expression to found the Element to remove
     *@return true if the operation succeed
     * @throws java.lang.Exception 
     */
    public boolean removeElement(String xpath) throws Exception {
        boolean success = false;
        try {
            moveCurrent(xpath);
            if (ref != null) {
                //keep on eye on the Element found by XPath
                Element element = ref;
                if (!ref.isRootElement()) {
                    ref = ref.getParent(); // go to the parent in order to remove the element, an element can't remove itself

                }
                success = ref.remove(element);
            }
        } catch (Exception e) {
            throw e;
        }
        return success;
    }

    /**
     *Remove several elements from an xpath expression
     *the method calls xpathNodes(String)
     *@param xpath the xpath expression
     * @throws java.lang.Exception 
     */
    public void removeElements(String xpath) throws Exception {
        try {
            xpathNodes(xpath);
            Vector<Node> v = getNodeList();
            Enumeration<Node> e = v.elements();
            while (e.hasMoreElements()) {
                Node n = e.nextElement();
                removeElement((Element) n);
            }
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *Remove the given element
     *@param node the element to remove
     *@return true if succeed
     * @throws java.lang.Exception 
     */
    public boolean removeElement(Element node) throws Exception {
        boolean success = false;
        try {
            if (node != null) {
                ref = node; //the node becomes the current node

                Element element = ref;  //keep an eye on this current

                if (!ref.isRootElement()) {
                    ref = ref.getParent();
                }
                ref.remove(element);
            }
        } catch (Exception e) {
            throw e;
        }
        return success;
    }

    /**
     *Store the xml document to a file. The encoding used is UTF-8
     *@param path parent absolute path of the file
     *@param nameFile 
     *@throws java.lang.Exception 
     * @see save(String)
     */
    public void save(String path, String nameFile) throws Exception {
        try {
            String fullName = path + "/" + nameFile;
            save(fullName);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *Store the xml document in a file. The encoding used is UTF-8
     *@param fullPath 
     *@throws Exception
     */
    public void save(String fullPath) throws Exception {
        try {
            OutputFormat format = new OutputFormat();
            format.setIndent(true);
            File f = new File(fullPath);
            f.createNewFile();
            FileOutputStream fileOut = new FileOutputStream(f.getAbsolutePath(), false);
            OutputStreamWriter outFile = new OutputStreamWriter(fileOut, "UTF-8");
            XMLWriter writer = new XMLWriter(outFile, format);
            Document doc = getDocument();
            if (doc != null) {
                writer.write(doc);
            }
            outFile.flush();
            outFile.close();
            writer.close();
            fileOut.close();
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *Methode main servant pour les tests
     * @param args 
     */
    public static void main(String args[]) {
        File file = new File("c:/users/parsem");
        System.out.println(file.mkdirs());
        File f2 = new File(file, "idiot");
        System.out.println(f2.mkdirs());
    //        try {
//            
//            File file = new File("c:/temp/test2.xml");
//            
//            XML doc2 = new XML(file.getAbsolutePath());
//            doc2.xpath("//tru*-*-0c");
//            System.out.println(doc2.getNode());
//            //doc2.xpathNodes("//@file[. = 'input.txt']");
//            // System.out.println(doc2.getNodeListAsXML());
//            //System.out.println(doc2.getXML()) ;
//            doc2.save(file.getAbsolutePath());
//        } catch (Exception e) {
//            System.err.println(e.toString());
//            e.printStackTrace();
//        }
    }
}


