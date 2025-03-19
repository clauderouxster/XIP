/*
 * Common.java
 *
 * Created on December 4, 2006, 11:32 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common;

import java.io.*;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Vector;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.Document;
import org.w3c.dom.ls.DOMImplementationLS;
import org.w3c.dom.ls.LSOutput;
import org.w3c.dom.ls.LSSerializer;

/**
 * Class for common static methods.
 * @author xtannier
 */
public class Common {
    
    
    
    /**
     * Returns the content of the text file
     * @param fileName the URI of the text file
     * @return the content of the text file
     */
    public static String read(String fileName) throws FileNotFoundException, IOException {
        String myString = "";
        String line;
        
        FileReader myFile = new FileReader(getFile(fileName));
        BufferedReader myBuffer = new BufferedReader(myFile);
        
        // Reading line after line
        while((line = myBuffer.readLine()) != null) {
            myString = myString + line + "\n";
        }
        myBuffer.close();
        myFile.close();
        return myString;
    }
    
    
    /**
     * Writes a text into a file.
     *
     * @param strText the text to write
     * @param strFileName the URI of the file
     * @throws java.io.IOException when an IO problem occurred.
     * @return 1 whether the writing finished properly, -1 otherwise
     */
    public static int write(String strText, String strFileName) throws IOException {
        
        FileWriter myFile = new FileWriter(strFileName);
        BufferedWriter myBuffer = new BufferedWriter(myFile);
        
        myBuffer.write(strText, 0, strText.length());
        myBuffer.flush();
        myBuffer.close();
        myFile.close();
        return 1;
    }
    
    /**
     * Writes a text into a file.
     *
     * @param strText the text to write
     * @param strFileName the URI of the file
     * @param append if true, then bytes will be written to the end of the file rather than the beginning
     * @throws java.io.IOException when an IO problem occurred.
     * @return 1 whether the writing finished properly, -1 otherwise
     */
    public static int write(String strText, String strFileName, boolean append) throws IOException {
        
        FileWriter myFile = new FileWriter(strFileName, append);
        BufferedWriter myBuffer = new BufferedWriter(myFile);
        
        myBuffer.write(strText, 0, strText.length());
        myBuffer.flush();
        myBuffer.close();
        myFile.close();
        return 1;
    }
    
    
    /**
     * Returns a file from its name. Expands environment variables and
     *  get canonical path.
     *
     * @param fileName the initial file name
     * @return File
     */
    public static File getFile(String fileName) throws IOException {
        Pattern p = Pattern.compile("\\$([a-zA-Z_]+)");
        Matcher m = p.matcher(fileName);
        StringBuffer sb = new StringBuffer();
        while (m.find()) {
            if (System.getenv(m.group(1)) != null)
                m.appendReplacement(sb, System.getenv(m.group(1)));
            else
                m.appendReplacement(sb, "");
        }
        m.appendTail(sb);
        return new File(sb.toString());
    }
    
    
    
    /**
     * Read tag names in the file and put them into a <code>Vector<String></code>
     *
     * @param fileName String
     * @return a <code>Vector<String></code> containing the tag names contained in the file
     *     string separators are space and '\n'.
     */
    public static Vector<String> readToVector(String fileName) throws IOException {
        Vector<String> result = new Vector<String>();
        String line;
        
        FileReader myFile = new FileReader(getFile(fileName));
        BufferedReader myBuffer = new BufferedReader(myFile);
        
        // Reading line after line
        while((line = myBuffer.readLine()) != null) {
            result.add(line);
        }
        myBuffer.close();
        myFile.close();
        return result;
    }
    
    
    /**
     * Prints an XML <code>Document</code> into a file.
     *
     * @param document the <code>Document</code> to print
     * @param fileName the name of the output file
     */
    public static void printXMLDocument(Document document, String fileName) throws ParserConfigurationException, FileNotFoundException, IOException {
        //Create builder DOM
        DocumentBuilder builder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
        
        // Create implementation
        DOMImplementationLS domImplLS = (DOMImplementationLS)builder.getDOMImplementation().getFeature("LS", "3.0");
        
        // output
        LSOutput lsOutput = domImplLS.createLSOutput();
        // serializer and output
        LSSerializer lsSerializer = domImplLS.createLSSerializer();
        
        // if it is a document, need XML declaration with appropriate encoding
        lsOutput.setEncoding(document.getXmlEncoding());
        
        FileOutputStream fos = new FileOutputStream(fileName);
        lsOutput.setByteStream(fos);
        lsSerializer.write(document, lsOutput);
        fos.close();
    }
    
    
    /**
     * Returns a copy of the specified array without duplicate.
     *
     * @param array a String array
     * @return a copy of the array without duplicate.
     */
    public static String[] getNoDuplicateArray(String[] array){
        HashSet<String> set = new HashSet<String>();
        for (int i = 0 ; i < array.length ; ++i){
            set.add(array[i]);
        }
        return set.toArray(new String[0]);
    }
    
    
    /**
     * Tests whether the specified String corresponds to a boolean value
     * @param value the String value
     * @return true if the specified String corresponds to a boolean value
     */
    public static boolean isBoolean(String value) {
        return (value != null && (value.equalsIgnoreCase("TRUE") || value.equals("1") ||
                value.equalsIgnoreCase("FALSE") || value.equals("0")));
    }
    
    
    /**
     * return the Class corresponding to the String parameter.
     * Uses getClass.
     *
     * @param packagePath the path of the package (with the final dot)
     * @param name the name of the class
     */
    public static Class getClassIgnoreCase(String packagePath, String name){
        Class result = null;
        try {
            result = Class.forName(packagePath + name);
            return result;
        } catch (ClassNotFoundException ex) {
            if (!name.equals("")){
                String packagePath1 = packagePath + name.substring(0, 1).toUpperCase();
                String packagePath2 = packagePath + name.substring(0, 1).toLowerCase();
                name = name.substring(1);
                result = getClassIgnoreCase(packagePath1, name);
                if (result == null)
                    result = getClassIgnoreCase(packagePath2, name);
                return result;
            } else
                return null;
        }
    }
    
    
    
    /**
     * Returns the Class present in the specified package with the specified name
     *
     * @param packageName the full name of the package
     * @param className the name of the class
     * @param ignoreCase true if the name of the class must not be case-sensitive, false otherwise
     * @return the Class corresponding to the parameters
     *
     * @throws ClassNotFoundException if no appropriate class is found
     */
    public static Class getClass(String packageName, String className, boolean ignoreCase) throws ClassNotFoundException {
        // Get a File object for the package
        File directory = null;
        URL resource;
        String path;
        try {
            ClassLoader cld = Thread.currentThread().getContextClassLoader();
            if (cld == null) {
                throw new ClassNotFoundException("Can't get class loader.");
            }
            path = packageName.replace('.', '/');
            resource = cld.getResource(path);
            if (resource == null) {
                return null;
            }
            directory = new File(resource.getFile());
        } catch (NullPointerException x) {
            throw new ClassNotFoundException(packageName + " (" + directory + ") does not appear to be a valid package");
        }
        // directory
        if (resource.getProtocol().equals("file") && directory.exists()) {
            // Get the list of the files contained in the package
            String[] files = directory.list();
            String className2;
            // we are only interested in .class files
            for (int i = 0; i < files.length; i++) {
                if (files[i].endsWith(".class")) {
                    // removes the .class extension
                    className2 = files[i].substring(0, files[i].length() - 6);
                    if ((ignoreCase && className.equalsIgnoreCase(className2)) ||
                            (!ignoreCase && className.equals(className2))) {
                        return Class.forName(packageName + '.' + className2);
                    }
                }
            }
        }
               // jar file
        else if (resource.getProtocol().equals("jar")){
            // get directory or jar name (classpath)
            String directoryPath = directory.getPath();
            String classPath = directoryPath.substring(0, directoryPath.length() - path.length() - 2);
            classPath = classPath.replaceFirst("file:", "");
            
            // check jar
            if (classPath.endsWith(".jar")) {
                JarFile jfile;
                try {
                    jfile = new JarFile(classPath);
                } catch (IOException ex) {
                    throw new ClassNotFoundException(packageName + " (" + directory + ") does not appear to be a valid package");
                }
                
                String jarEntry;
                // For each entry of jar
                for (Enumeration<JarEntry> entries = jfile.entries(); entries.hasMoreElements();) {
                    JarEntry element = entries.nextElement();
                    jarEntry = element.getName();
                    // check that it matches the expected path + name
                    if ((ignoreCase &&
                            jarEntry.equalsIgnoreCase(path + File.separator + className + ".class")) ||
                            (!ignoreCase &&
                            jarEntry.equals(jarEntry + File.separator + className + ".class"))) {
                        return Class.forName(jarEntry.replaceAll("/", ".").substring(0, jarEntry.length() - 6));
                        
                    }
                }
            }
        }
        throw new ClassNotFoundException("Class not found.");
    }
    
  
    
    /**
     * Returns the Package name present in the specified package with the specified name
     *
     * @param packageName the full name of the package
     * @param childPackageName the name of the package
     * @param ignoreCase true if the name of the class must not be case-sensitive, false otherwise
     * @return the Class corresponding to the parameters
     *
     * @throws ClassNotFoundException if no appropriate class is found
     */
    public static String getPackage(String packageName, String childPackageName, boolean ignoreCase) throws ClassNotFoundException {
        ArrayList<String> packages = new ArrayList<String>();
        // Get a File object for the package
        File directory = null;
        String path;
        URL resource;
        try {
            ClassLoader cld = Thread.currentThread().getContextClassLoader();
            if (cld == null) {
                throw new ClassNotFoundException("Can't get class loader.");
            }
            path = packageName.replace('.', '/');
            resource = cld.getResource(path);
            if (resource == null) {
                throw new ClassNotFoundException("No resource for " + path);
            }
            directory = new File(resource.getFile());
            
        } catch (NullPointerException x) {
            throw new ClassNotFoundException(packageName + " (" + directory + ") does not appear to be a valid package");
        }
        // directory
        if (resource.getProtocol().equals("file") && directory.exists()) {
            // Get the list of the files contained in the package
            File[] files = directory.listFiles();
            // we are only interested in directories
            for (int i = 0; i < files.length; i++) {
                if (files[i].isDirectory()) {
                    File f = files[i];
                    String newPackageName = f.getName();
                    packages.add(newPackageName);
                    
                    if ((ignoreCase && childPackageName.equalsIgnoreCase(newPackageName)) ||
                            (!ignoreCase && childPackageName.equals(newPackageName))) {
                        return packageName + '.' + newPackageName;
                    }
                }
            }
        } 
        // jar file
        else if (resource.getProtocol().equals("jar")){
            // get directory or jar name (classpath)
            String directoryPath = directory.getPath();
            String classPath = directoryPath.substring(0, directoryPath.length() - path.length() - 2);
            classPath = classPath.replaceFirst("file:", "");
            
            // check jar
            if (classPath.endsWith(".jar")) {
                JarFile jfile;
                try {
                    jfile = new JarFile(classPath);
                } catch (IOException ex) {
                    throw new ClassNotFoundException(packageName + " (" + directory + ") does not appear to be a valid package");
                }
                
                String jarEntry;
                // For each entry of jar
                for (Enumeration<JarEntry> entries = jfile.entries(); entries.hasMoreElements();) {
                    JarEntry element = entries.nextElement();
                    jarEntry = element.getName();
                    // check that it matches the expected path + name
                    if ((ignoreCase &&
                            jarEntry.equalsIgnoreCase(path + File.separator + childPackageName + File.separator)) ||
                            (!ignoreCase &&
                            jarEntry.equals(jarEntry + File.separator + childPackageName + File.separator))) {
                        return jarEntry.replaceAll("/", ".").substring(0, jarEntry.length() - 1);
                        
                    }
                }
            }
        }
        throw new ClassNotFoundException("Package not found");
    }
    
}
