/*
 * Copyright Xerox 2009
 */
package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.ontology.OntDocumentManager;
import com.hp.hpl.jena.util.Locator;
import java.io.File;
import java.io.FileNotFoundException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import org.apache.log4j.Logger;

public abstract class Util {

    private static Logger log = Logger.getLogger(Util.class);

    public static File findGrammarFile(Class clazz, String relResourceName)
            throws FileNotFoundException {
        URL grmUrl = clazz.getResource(relResourceName);
        if (grmUrl == null) {
            throw new FileNotFoundException(relResourceName);
        }
        if (!grmUrl.getProtocol().equalsIgnoreCase("file")) {
            throw new FileNotFoundException("Found grammar resource " + relResourceName + " at non-file URL " + grmUrl);
        }

        return new File(grmUrl.getPath());
    }

    public static void registerOntology(Class<?> schemagenClass) throws OntologyLocationException {
        OntDocumentManager dm = OntDocumentManager.getInstance();

        // Add a LocatorJarURL to the document manager handlers, unless
        // one has already been added.
        Iterator locators = dm.getFileManager().locators();
        boolean found = false;
        while (locators.hasNext()) {
            Locator loc = (Locator) locators.next();
            if (loc.getName().equals(LocatorJarUrl.class.getName())) {
                found = true;
                break;
            }
        }
        if (!found) {
            dm.getFileManager().addLocator(new LocatorJarUrl());
        }

        String ontUri = null;
        String msg = schemagenClass.getName() + " appears not to be a valid schemagen class";
        try {
            ontUri = (String) schemagenClass.getField("NS").get(schemagenClass);
        } catch (NoSuchFieldException e) {
            throw new OntologyLocationException(msg, e);
        } catch (SecurityException e) {
            throw new OntologyLocationException(e);
        } catch (IllegalArgumentException e) {
            throw new OntologyLocationException(msg, e);
        } catch (IllegalAccessException e) {
            throw new OntologyLocationException(e);
       }
        if (ontUri.endsWith("#")) {
            ontUri = ontUri.substring(0, ontUri.length() - 1);
        }
        String filename = schemagenClass.getSimpleName()+".owl";
        URL resourceUrl = schemagenClass.getResource(filename);
        if (resourceUrl == null) {
            throw new OntologyLocationException("Failed to find " + filename);
        }
        
        dm.addAltEntry(ontUri, resourceUrl.toString());
    }

    /**
     * Recursive directory listing
     */
    public static List<File> listFiles(File root) {
        File[] children = root.listFiles();
        ArrayList<File> results = new ArrayList<File>();
        for (File f : children) {
            if (f.isDirectory()) {
                results.addAll(listFiles(f));
            } else {
                results.add(f);
            }
        }
        return results;
    }


}
