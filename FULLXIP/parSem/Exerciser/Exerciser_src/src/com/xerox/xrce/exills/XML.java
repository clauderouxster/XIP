/*
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
import java.net.URL;
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

   private Document xmlDoc ; // le document xml
   private Document newDoc ; // le document xml resultant de la transformation xsl
   
   // Pour ecrire un document xml
   private  DocumentFactory xml; // constructeur du document xml
   private  Element root; // element root du document
   private  Element ref; //dernier element reference
    
    
    // pour XPATH
    private Node node ; //noeud d'un document 
    private List nodeList ; // litse de noeud


    /** Constructeur par defaut */
    public XML() throws Exception {
         try {
            //  DocumentFactory xml = new DocumentFactory();
         } catch (Exception e){throw new Exception(e);}
         
    }

    
    
    /**
     * cree le Document correspondant au document XML contenu a l'URL
     * passee en parametre
     * @param xmlFileURL l'URL du doument XML a traiter
     * @exception Exception
     */
    public  XML(String xmlFileURL) throws Exception {
         try {
             File file = new File(xmlFileURL);
             SAXReader xmlReader = new SAXReader();
             System.out.println(xmlFileURL);
             xmlDoc = xmlReader.read(file.toURI().toString());
             root = xmlDoc.getRootElement();
             ref = root;
         } catch (Exception e)  { 
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
        } catch (Exception e)  {
             throw new Exception(e);
         }
    }

    public XML(StringReader xmlStream) throws Exception  {
           try {
              SAXReader xmlReader = new SAXReader();
              xmlDoc = xmlReader.read(xmlStream);
           } catch (Exception e)  {
             throw new Exception(e);
           }
    }

    
  public void setDOCTYPE(String name,String publicId, String SystemId) throws Exception {
      try {  
      DefaultDocumentType docType = new DefaultDocumentType(name,publicId,SystemId );
      xmlDoc.setDocType(docType);
      }
      catch (Exception e ) {
          throw new Exception(e);
      }
  }
    /**
     * Cree la racine du document 
     * @param racine racine du document XML que l'on veut ecrire
     * @exception Exception
     */
    public void setRoot(String racine) throws Exception  {
         try { DocumentFactory xml = new DocumentFactory();
          this.root = xml.createElement(racine);
          xmlDoc = xml.createDocument();
          xmlDoc.setRootElement(this.root);
       } catch (Exception e)  {
             throw new Exception(e);
         }
         }
    
    
    /**
     * tansforme le Document XML en lui appliquant la feuille de style XSLT
     * contenue a l'URL
     * @param xsltFileURL l'URL de la feuille de style XSLT
     */
    public void transform(String xsltFileURL) throws Exception  {
        try {
            TransformerFactory factory = TransformerFactory.newInstance();
            Transformer transformer = factory.newTransformer(new StreamSource(xsltFileURL));

            DocumentSource source = new DocumentSource(xmlDoc);
            DocumentResult result = new DocumentResult();
            transformer.transform(source, result);
            newDoc = result.getDocument();
        } catch (Exception e) {
            System.err.println("Erreur dans transform(String xsltFileURL)" + e.toString());
            newDoc = null;
            throw new Exception(e);
        }
    }

    /**
     * retourne le fichier XML transforme par la feuille de style XSLT
     * @return String
     */
    public String getTransformed() throws Exception  {
      try {
          
       if (newDoc != null)
            return newDoc.asXML();
        return null;
    } catch (Exception e)  {
             throw new Exception(e);
         }
      }

    
    /**
     * selectionne un seul noeud dans le document XML
     * @param xpathExp l'expression XPATH a appliquer
     * @exception Exception
     */
    public void xpath(String xpathExp) throws Exception {
      try {
            /* on cree le chemin XPATH permettant de selectionner le noeud voulu */
            XPath xpathSelector = xmlDoc.createXPath(xpathExp);
            /* on recupere le noeud selectionne */
            node = xpathSelector.selectSingleNode(xmlDoc);
          } catch (Exception e)  {
             throw new Exception(e);
         }
    }

    /**
     * retourne la valeur du noeud selectionne avec le methode xpath
     * @return Sting
     */
    public String getNode() throws Exception {
        try {
        if (node != null)
            return node.getStringValue();

        return null;
    } catch (Exception e)  {
             throw new Exception(e);
         }
        }

    /**
     *Renvoie le texte d'un noeud situe par le xpath. 
     *@param path path du noeud a recuperer 
     */
    public String getNode(String path) throws Exception {
       try {
        xpath(path);
       return getNode();
   } catch (Exception e)  {
             throw new Exception(e);
         }
       }

    
    public String getNodeAsXML() throws Exception  {
        try {   
        if (node != null)
            return node.asXML();
        return null;
    } catch (Exception e)  {
             throw new Exception(e);
         }
    }

    
    
    public static String getValueAttribute(Node n, String name) {
       if (n.getNodeTypeName().equalsIgnoreCase("Element")) {
           Element el = (Element)n;
           return el.attributeValue(name);
       }
       else return new String();
    }
    
    /**
     * selectionne une liste de noeuds dans le document XML
     * @param xpathExp l'expression XPATH a appliquer
     * @throws Exception
     */
    public void xpathNodes(String xpathExp) throws Exception {
         try {
        /* on cree le chemin XPATH permettant de selectionner le noeud voulu */
           XPath xpathSelector = xmlDoc.createXPath(xpathExp);
           /* on recupere le noeud selectionne */
           nodeList = xpathSelector.selectNodes( xmlDoc );
   } catch (Exception e)  {
             throw new Exception(e);
         }
    }

    /**
     * retourne une liste de Noeuds, 
     * selectionne apres avoir appele la methode xpathNodes
     * @return Vector de String
     */
    public Vector getNodeList() throws Exception {
        try {
            Vector liste = null;
        if (nodeList != null ) {
           liste = new Vector(nodeList.size())    ;
           Iterator i = nodeList.iterator();
            while(i.hasNext()) {
              liste.add(((Node)i.next()));
          }
       } 
       return liste;
    } catch (Exception e)  {
             throw new Exception(e);
         } 
    }

    /**
     * retourne un String, correspondant a un sous-arbre
     * selectionne apres avoir appele la methode xpathNodes
     * @return String xml
     */
    public String getNodeListAsXML() throws Exception {
       try {
           String XML = "";
        if (nodeList != null ) {
             Iterator i = nodeList.iterator();
              while(i.hasNext()) {
              XML = XML + ((Node)i.next()).asXML();
              }
        }
        return XML;
    } catch (Exception e)  {
             throw new Exception(e);
         }
    }


   
   /**
     * Ajoute un noeud a la balise root
     * @param Name nom du noeud a ajouter
     * @param Value valeur du noeud a ajouter
     */
public  void addNoeud(String Name,String Value) throws Exception {
         try {
              ref = root.addElement(Name);
              ref.setText(Value);
         } catch (Exception e)  {throw new Exception(e); }     
}

     /**
      * Ajoute un noeud a la balise root
      * @param Name nom du noeud a ajouter
      */
public void addNoeud(String Name) throws Exception {
        try {
             ref = root.addElement(Name);
         } catch (Exception e)  { throw new Exception(e); }     
}

     /**
      * Ajoute un noeud sur le dernier element reference
      * @param Name nom du noeud a ajouter
      * @param value valeur du noeud
      */
public void add(String Name,String value)throws Exception  {
        try {
             ref = ref.addElement(Name);
             ref.setText(value);
        } catch (Exception e)  {throw new Exception(e);}     
}


     /**
      * Ajoute un noeud sur le dernier element reference
      * @param Name nom du noeud a ajouter
      */
  public void add(String Name) throws Exception {
            try{
                ref = ref.addElement(Name);
            } catch (Exception e)  {throw new Exception(e); }        
  }


          /**
           * Donne une valeur au noeud (le dernier noeud reference)
           * @param txt texte du noeud
           */
public void setText(String txt) throws Exception {
            try {
                if (txt != null) 
                  ref.setText(txt);
            } catch (Exception e)  {throw new Exception(e); } 
}

     /**
      * Ajoute un fils au dernier noeud reference
      * @param Name nom du noeud fils
      */
public void addChild(String Name)throws Exception  {
       try {
            ref = ref.addElement(Name);
           } catch (Exception e)  {
             throw new Exception(e);
           }     
}

     /**
      * Remonte au parent du dernier noeud reference, il devient alors le noeud de reference
      */
public void parent() throws Exception {
         ref = ref.getParent();
     }

     /**
      * Remonte i fois dans l'arbre
      * @param i nombre de fois a remonter
      */
public void parent(int i) throws Exception {
        try {
             for (int j=0; j <i;j++)
             ref = ref.getParent();
        } catch (Exception e)  {throw new Exception(e);}
}

     /**
      * Ajoute un fils au dernier noeud reference
      * @param Name nom du fils
      * @param value valeur du fils
      */
     public void addChild(String Name,String value) throws Exception {
         try {
              ref = ref.addElement(Name);
              ref.setText(value);
         } catch (Exception e)  {
             throw new Exception(e);
         }
         }


          /**
           * Ajoute un attribut au dernier noeud reference
           * @param name nom de l'attribut
           * @param value valeur de l'attribut
           */
public void addAttribute(String name,String value) throws Exception {
          try {     
               ref = ref.addAttribute(name,value);
            } catch (Exception e) { throw new Exception(e);}
          }

  

     /**
      * Retourne le document XML
      * @return document xml de tytpe Document
      */
     public Document getDocument() throws Exception {  return xmlDoc;  }


  /**
     * Selection l'element selon un chemin et met celui-ci comme dernier element reference
     * @param path chemin pour atteindre l'element dans dans le document xml
     */

public void selectElement(String path) throws Exception {
     try {
          /* on cree le chemin XPATH permettant de selectionner le noeud voulu */
          XPath xpathSelector = xmlDoc.createXPath(path);
          /* on recupere le noeud selectionne */
          ref = (Element)xpathSelector.selectSingleNode(xmlDoc);
       } catch (Exception e) {
             throw new Exception(e);
       }
 }

 /**
  * Retourne le context en cours
  * @return le dernier element reference
  */
public Object enCours() throws Exception { return ref;}


 /**
  * Selectionne l'element selon le chemin path dans le context passe en argument
  * @param path chemin pour atteindre l'element voulu
  * @param context contexte du chemin
  * @throws Exception 
  */
public void selectElement(String path,Object context) throws Exception  {
     try {
         /* on cree le chemin XPATH permettant de selectionner le noeud voulu */
         XPath xpathSelector = xmlDoc.createXPath(path);
         /* on recupere le noeud selectionne */
         ref = (Element)xpathSelector.selectSingleNode(context);
     
     } catch (Exception e) {
         throw new Exception(e);
     }
}

    /**
     * Retourne le document xml sous forme de String
     * @return le document xml complet
     */
     public String getXML() throws Exception {
         try {
              return this.xmlDoc.asXML();
         } catch (Exception e) {
             throw new Exception(e);
         }
    }
     

     public void removeElement(String xpath) throws Exception{
         try {
              selectElement(xpath);
             if (ref != null) {
               Element element = ref;
               if (!ref.isRootElement())
                  ref = ref.getParent();
               ref.remove(element); 
             }
         }catch (Exception e) {
             throw new Exception(e);
         }
         
     }
     
     
     
      /**
    *Suppression de tous les path du document XML
    */
    public  void removeElements(String xpath) throws Exception {
         try {
          xpathNodes(xpath);
          Vector v = getNodeList();
          Enumeration e = v.elements();
           while (e.hasMoreElements()) {
            Node n = (Node)e.nextElement();
            removeElement((Element)n);
           }
        
       } catch (Exception e ) {
           throw new Exception(e);
        
       }
    }
     
     
     public void removeElement(Element node) throws Exception{
         try {
             if (node != null) {
               ref = node;
               Element element = node;
               if (!ref.isRootElement())
                  ref = ref.getParent();
               ref.remove(element); 
             }
         }catch (Exception e) {
             throw new Exception(e);
         }
         
     }
     
  
     public void save(String path, String nameFile) throws Exception{
        try {
          String fullName = path + "/" + nameFile;
          save(fullName);
         
        } catch (Exception e) {
            throw new Exception(e);
        }
        }
     
     
       public void save(String fullPath) throws Exception{
        try {
           
           
            OutputFormat format = new OutputFormat();
            format.setIndent(true);
            File f = new File(fullPath);
            f.createNewFile();
            FileOutputStream fileOut = new FileOutputStream(f.getAbsolutePath(),false);
            OutputStreamWriter outFile  = new OutputStreamWriter(fileOut,"UTF-8");
            
            
            XMLWriter writer = new XMLWriter( outFile, format );
        
            Document doc = getDocument();
            if ( doc != null) { 
               writer.write( doc );
            }
            outFile.flush();
            outFile.close();
            writer.close();
            fileOut.close();
        
        } catch (Exception e) {
            throw new Exception(e);
        }
        }
     
     /**
      *Methode main servant pour les tests
      */
     public static void main(String args[]) {
         try {
         
         File file = new File("c:/temp/test2.xml");
         
         XML doc2 = new XML(file.getAbsolutePath());
          
         //doc2.xpathNodes("//@file[. = 'input.txt']");
         // System.out.println(doc2.getNodeListAsXML());
          //System.out.println(doc2.getXML()) ;
          doc2.save(file.getAbsolutePath());
         }
         catch (Exception e) {
             System.err.println(e.toString());
             e.printStackTrace();
         }
     }
  
}// fin de la classe
    
    
