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

import java.util.ArrayList;
import java.util.List;
import org.apache.log4j.*;
import java.util.Collection;
import java.util.Iterator;

/**
 *
 * @author Vianney Grassaud
 */
public class FileTools {

    Logger log = Logger.getLogger("tracer");
    // les encodages  possibles
    static public final String ENCODING_UNKNOWN = "unknown";
    static public final String ENCODING_DEFAULT = System.getProperty("file.encoding");
    static public final String UTF_8 = "UTF-8";
    static public final String UTF_16 = "UTF-16";
    static public final String ISO_8859_1 = "ISO-8859-1";
    static public final String CP1252 = "Cp1252";
    static public final String ASCII = "ASCII";

    /**
     * Decoupe le path en simple elements et les ajoutes dans une liste dans l'ordre inverse
     * example : soit le path /a/b/c/d.txt, le resultat sera  [d.txt,c,b,a]
     * @param f  fichier
     * @return un List collection avec elements du path dans l'ordre inverse
     */
    public static List getPathList(File f) {

        List l = new ArrayList();
        File r;
        try {
            r = f.getCanonicalFile();
            while (r != null) {
                l.add(r.getName());
                //logger.info("getName "+ r.getName());
                r = r.getParentFile();
            }
        } catch (IOException e) {
            e.printStackTrace();
            l = null;
        }
        return l;
    }

    /**
     * figure out a string representing the relative path of
     * 'f' with respect to 'r'
     * @param r home path
     * @param f path of file
     */
    private static String matchPathLists(List r, List f) {
        int i;
        int j;
        String s;
        // start at the beginning of the lists
        // iterate while both lists are equal
        s = "";
        i = r.size() - 1;
        j = f.size() - 1;
        // first eliminate common root
        while ((i >= 0) && (j >= 0) && (r.get(i).equals(f.get(j)))) {
            i--;
            j--;
        }
        // for each remaining level in the home path, add a ..
        for (; i >= 0; i--) {
            s += ".." + File.separator;
        }
        // for each level in the file path, add the path
        for (; j >= 1; j--) {
            s += f.get(j) + File.separator;
        }
        // file name
        s += f.get(j);
        return s;
    }

    /**
     * get relative path of File 'f' with respect to 'home' directory
     * example : home = /a/b/c
     *           f    = /a/d/e/x.txt
     *           s = getRelativePath(home,f) = ../../d/e/x.txt
     * @param home base path, should be a directory, not a file, or it doesn't make sense
     * @param f file to generate path for
     * @return path from home to f as a string
     */
    public static String getRelativePath(File home, File f) {
        File r;
        List homelist;
        List filelist;
        String s;
        homelist = getPathList(home);
        filelist = getPathList(f);
        s = matchPathLists(homelist, filelist);
        return s;
    }

    /**
     * Stores the given String into the given file
     * @param toStore_ String to store
     * @param into_ Filename
     * @param encoding
     * @throws java.lang.Exception 
     */
    public static void storeString(String toStore_, String into_, String encoding) throws Exception {
        FileOutputStream fileOut = new FileOutputStream(into_, false);
        OutputStreamWriter out;
        if (isUnknownEncoding(encoding)) {
            out = new OutputStreamWriter(fileOut);
        } else {
            out = new OutputStreamWriter(fileOut, encoding);
        }
        out.write(toStore_);
        out.flush();
        out.close();
        fileOut.close();
    }

    public static void storeSentences(Collection<String> sentences, File file, String encoding) throws Exception {
        //outputstream for the file
        try {
            FileOutputStream fileOut = new FileOutputStream(file, true);
            OutputStreamWriter out;
            if (isUnknownEncoding(encoding)) {
                out = new OutputStreamWriter(fileOut);
            } else {
                out = new OutputStreamWriter(fileOut, encoding);
            }
            Iterator<String> ite = sentences.iterator();
            while (ite.hasNext()) {
                String sentence = ite.next();
                out.write(sentence);
            }
            out.flush();
            out.close();
            fileOut.close();
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *Delete files and directories recursively on the disk
     *@param direcory 
     * @return true it was correctly deleted
     */
    static public boolean deleteFiles(File direcory) {
        if (direcory.isFile() || direcory.list().length == 0) {
            direcory.delete();
            return true;
        } else {
            File[] fs = direcory.listFiles();
            boolean res = false;
            for (int i = 0; i < fs.length; i++) {
                res = deleteFiles(fs[i]);
            }
            direcory.delete();
            return res;
        }
    }

    /**
     *This method check if the given file exists.
     * @param file
     * @throws java.io.FileNotFoundException
     * @throws java.lang.NullPointerException 
     */
    public static void exists(File file) throws FileNotFoundException, NullPointerException {
        try {
            if (!file.exists()) {
                throw new FileNotFoundException("the file : " + file + " doesn't exist");
            }
        } catch (NullPointerException e) {
            throw e;
        }
    }

    /**
     * Copy one file to another one
     * @param src Source file
     * @param dst Dest file
     * @throws IOException
     */
    public static void copy(File src, File dst) throws IOException {
        // Copies src file to dst file.
        // If the dst file does not exist, it is created
        if (!dst.exists()) {
            InputStream in = new FileInputStream(src);
            OutputStream out = new FileOutputStream(dst);
            // Transfer bytes from in to out
            byte[] buf = new byte[1024];
            int len;
            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }
            in.close();
            out.close();
        }
    }

    public static String getFileName(String absolutePath) {
        try {
            File file = new File(absolutePath);
            return file.getName();
        } catch (Exception e) {
            return absolutePath;
        }
    }

    /**
     *Creates directories included in a path of a file
     *@param dst file
     */
    public static void subMkdir(File dst) {
        try {
            //on recupere le path du fichier afin de creer les sous repertoires s'ils existent:
            String absolute = dst.getAbsolutePath();
            absolute = absolute.replace('\\', '/');
            String[] paths = absolute.split("/");
            String pathConstruct = paths[0];
            for (int i = 1; i < paths.length - 1; i++) {
                pathConstruct += "/" + paths[i];
                File d = new File(pathConstruct);
                //System.out.println("creation du repertoire " + d.getAbsolutePath());
                d.mkdir();
            }
        } catch (Exception e) {
            System.err.println(e.toString());
            e.printStackTrace();
        }
    }

    public static String removeRoot(String head, String path) {
        String newPath = path.replace('\\', '/');
        if (head != null && !head.equals("")) {
            String h = head.replace('\\', '/');
            String[] tab = newPath.split(h);
            if (tab.length > 0) {
                newPath = tab[tab.length - 1];
            }
        }
        File f = new File(newPath);
        List l = FileTools.getPathList(f);
        Object[] segments = l.toArray();
        String res = "";
        for (int j = segments.length - 1; j >= 0; j--) {
            res += "/" + (String) segments[j];
        }
        if (res.equals("")) {
            return newPath;
        } else {
            return res.replaceAll("//", "/");
        }
    }

    static public boolean isUnknownEncoding(String encoding) {
        if (encoding == null) {
            return true;
        } else {
            return (!encoding.equalsIgnoreCase(UTF_8) &&
                    !encoding.equalsIgnoreCase(UTF_16) &&
                    !encoding.equalsIgnoreCase(ISO_8859_1) &&
                    !encoding.equalsIgnoreCase(CP1252) &&
                    !encoding.equalsIgnoreCase(ASCII) &&
                    !encoding.equalsIgnoreCase(ENCODING_DEFAULT));
        }
    }

    static public String read(File f, String encoding) throws Exception {

        try {
            FileInputStream in = new FileInputStream(f);
            byte[] cars = new byte[(int) f.length()];
            in.read(cars);
            in.close();
            if (isUnknownEncoding(encoding)) {
                encoding = FileTools.ENCODING_DEFAULT;
            }
            return new String(cars, encoding);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *@param f 
     * @param encoding 
     * @param start 
     * @param end 
     * @return 
     * @throws java.lang.Exception 
     * @deprecated
     */
    static public String readFaster(File f, String encoding, int start, int end) throws Exception {
        try {
            FileInputStream in2 = new FileInputStream(f);

            InputStreamReader in3;
            if (isUnknownEncoding(encoding)) {
                in3 = new InputStreamReader(in2);
            } else {
                in3 = new InputStreamReader(in2, encoding);
            }
            StringBuffer buf = new StringBuffer();
            char[] sub = new char[(end - start) + 1];
            // System.out.println(sub.length);
            if (start > 2) {
                in3.skip(start - 1);
            } else {
                in3.skip(start);
            }
            in3.read(sub, 0, sub.length);
            in2.close();
            in3.close();
            boolean stop = false;
            int empty = sub.length;
            for (int i = 0; i < sub.length && !stop; i++) {
                if ((sub[i] == '\0')) {
                    empty = i;
                    stop = true;
                }
            }
            return new String(sub, 0, empty);
        } catch (Exception e) {
            throw e;
        }
    }

    static public String read(File f, String encoding, int start, int end) throws Exception {
        try {
            FileInputStream in2 = new FileInputStream(f);
            byte[] cars = new byte[(end - start)];
            in2.skip(start);
            in2.read(cars);
            in2.close();
            if (isUnknownEncoding(encoding)) {
                encoding = FileTools.ENCODING_DEFAULT;
            }
            return new String(cars, encoding);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *Read a file and creates a sub file of it. the file is read as a sequence of bytes, so we
     *don't use String interpretation.
     *To read all the file use -1 -1 for start end end
     *@param file the file to read
     *@param start - the start line inclusive.
     *@param end  - the end line , inclusive. use -1 for until at the end of file
     *@return a subfile named : start_end_fileName
     * @throws java.lang.Exception 
     *
     */
    static public File createSubFile(File file, int start, int end) throws Exception {
        FileInputStream in = new FileInputStream(file);
        File subFile = File.createTempFile(start + "_" + end + "_" + file.getName(), "tmp");
        if (subFile.exists()) {
            subFile.delete();
        }
        //subFile.deleteOnExit();
        subFile.createNewFile();
        //write the sub file
        FileOutputStream fileOut = new FileOutputStream(subFile, true);
        //the index of line
        int line = 1;
        int car = in.read();
        boolean stop = false;
        while (car != -1 && !stop) {
            if (isLine(car)) {
                //     System.out.println("ligne : " +line);
                line++;
            }
            //if we need to read fully the file
            if (end <= 0) {
                if (start <= line) {
                    fileOut.write(car);
                }
            } else {
                if (start <= line && line <= end) {
                    fileOut.write(car);
                }
            }
            //break the loop if we want a line before the end of file
            if (end < line && end > 0) {
                stop = true;
            }
            car = in.read();
        }
        in.close();
        fileOut.close();
        //   System.out.println("nombre de ligne : " + (line - 1) );
        return subFile;
    }

    static private boolean isLine(int car) {
        String sepLine = ("\n");
        //String sepLine = System.getProperty("line.separator");
        byte[] bytes = sepLine.getBytes();
        for (int i = 0; i < bytes.length; i++) {
            Byte b = new Byte(bytes[i]);
            if (b.intValue() == car) {
                return true;
            }
        }
        return false;
    }

    /**
     * Main
     * @param args - the command line arguments
     */
    public static void main(String[] args) {
        try {
            File file = new File("c:/AllEntit.txt");
            File output = createSubFile(file, -1, -1);
        } catch (Exception e) {
            System.out.println(e);
            e.printStackTrace();
        }
    }
}
