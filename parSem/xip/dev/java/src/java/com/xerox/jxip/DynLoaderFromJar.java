/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2009 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Date;
import java.util.Enumeration;
import java.util.Properties;
import java.util.jar.Attributes;
import java.util.jar.JarFile;
import java.util.jar.Manifest;

/**
 * The purpose of this class is to load the dynamic libs.
 *
 * @author grondeau @created October 12, 2005
 */
class DynLoaderFromJar {

    private static File tmpDir;
    private static String[] libNamesWindows;
    private static String offsetWindows;
    private static String offsetWindows64;
    private static String[] libNamesLinux;
    private static String offsetLinux;
    private static String offsetLinux64;
    private static String[] libNamesSolaris;
    private static String offsetSolaris;
    private static String[] libNamesMac;
    private static String offsetMac;

    public static void loadLibFromJar() {
        //System.out.println("DynLoaderFromJar()");
        DynLoaderFromJarCommon(null);
    }

    public static void loadLibFromJar(String libs[], boolean loadingLibInJar) {
        for (String libPath : libs) {
            try {
                if (new File(libPath).exists()) {
                    //System.out.println("DynLoaderFromJar  loading: " + libPath);
                    System.load(libPath);
                } else {
                    System.out.println("The lib does not exist: " + libPath);
                }
            } catch (Throwable ex) {
                System.out.println("Exception during loading: " + libPath);
                System.out.println("Exception: " + ex);
                throw new RuntimeException("DynLoaderFromJar:" + ex.toString());
            }
        }
        if (loadingLibInJar) {
            DynLoaderFromJarCommon(null);
        }
    }

    private static void DynLoaderFromJarCommon(String jarFile) {
        //System.out.println("DynLoaderFromJarCommon()");
        createTmpDir();
        init();
        loadNativeLibraries(jarFile);
        //deleteTmpDir();
    }

    private static void init() {
        boolean usePython;
        InputStream inputStream = DynLoaderFromJar.class.getResourceAsStream("/init.properties");
        Properties properties = new Properties();
        try {
            properties.load(inputStream);
            usePython = properties.getProperty("python").equals("true");
        } catch (Exception ex) {
            usePython = false;
        }
        offsetWindows = "windows/sharedLib";
        offsetWindows64 = "windows64/sharedLib";
        offsetLinux = "linux/sharedLib";
        offsetLinux64 = "linux64/sharedLib";
        offsetSolaris = "solaris5.10-x86/sharedLib";
        offsetMac = "mac-i386/sharedLib";
        if (usePython) {
            //with python scripting
            libNamesWindows = new String[2];
            libNamesWindows[0] = "python26.dll";
            libNamesWindows[1] = "jXipPy.dll";
            libNamesLinux = new String[2];
            libNamesLinux[0] = "libpython2.6.so";
            libNamesLinux[1] = "libjXipPy.so";
            libNamesSolaris = new String[1];
            libNamesSolaris[0] = "libjXipPy.so";
            libNamesMac = new String[2];
            libNamesMac[0] = "libpython.so";
            libNamesMac[1] = "libjXipPy.so";
        } else {
            //without python scripting
            libNamesWindows = new String[1];
            libNamesWindows[0] = "jXip.dll";
            libNamesLinux = new String[1];
            libNamesLinux[0] = "libjXip.so";
            libNamesSolaris = new String[1];
            libNamesSolaris[0] = "libjXip.so";
            libNamesMac = new String[1];
            libNamesMac[0] = "libjXip.so";
        }
    }

    public static boolean is64BitVM() {
        String bits = System.getProperty("sun.arch.data.model", "?");
        //System.out.println("++++++++++++++++++" + bits + "  " + System.getProperty("java.vm.name"));
        if (bits.equals("64")) {
            return true;
        }
        if (bits.equals("?")) {
            // probably sun.arch.data.model isn't available
            // maybe not a Sun JVM?
            // try with the vm.name property
            return System.getProperty("java.vm.name").toLowerCase().contains("64");
        }
        // probably 32bit
        return false;
    }

    public static boolean isWindows() {
        String windowsStr = "Windows";
        String osName = System.getProperty("os.name");
        if (osName.length() >= windowsStr.length()) {
            osName = osName.substring(0, windowsStr.length());
        }
        return osName.equals(windowsStr);
    }

    public static boolean isLinux() {
        return System.getProperty("os.name").equals("Linux");
    }

    public static boolean isSolaris() {
        return System.getProperty("os.name").equals("SunOS");
    }

    public static boolean isMac() {
        return (System.getProperty("os.name").equals("Mac OS X"))
                || (System.getProperty("os.name").equals("Darwin"));
    }

    /**
     * call loadDynamicLibFromJar with the right parameters
     *
     * @param jarFile Description of the Parameter
     */
    private static void loadNativeLibraries(String jarFile) {
        //System.out.println("loadNativeLibraries()");
        if (isWindows()) {
            if (is64BitVM()) {
                loadDynamicLibFromJar(offsetWindows64, libNamesWindows, jarFile);
            } else {
                loadDynamicLibFromJar(offsetWindows, libNamesWindows, jarFile);
            }
        } else if (isSolaris()) {
            loadDynamicLibFromJar(offsetSolaris, libNamesSolaris, jarFile);
        } else if (isLinux()) {
            if (is64BitVM()) {
                loadDynamicLibFromJar(offsetLinux64, libNamesLinux, jarFile);
            } else {
                loadDynamicLibFromJar(offsetLinux, libNamesLinux, jarFile);
            }
        } else if (isMac()) {
            loadDynamicLibFromJar(offsetMac, libNamesMac, jarFile);
        } else {
            throw new RuntimeException("unknow os:    os.name = " + System.getProperty("os.name"));
        }
    }

    /**
     * create a temporary dir
     */
    public static void createTmpDir() {
        String tmpDirStr = System.getProperty("java.io.tmpdir");
        if (!tmpDirStr.endsWith(File.separator)) {
            tmpDirStr += File.separator;
        }
        tmpDirStr += System.getProperty("user.name");
        tmpDirStr += File.separator + getBuildDateInManifest();
        tmpDir = new File(tmpDirStr);
//        if (tmpDir.exists()) {
//            deleteTmpDir();
//        }
//        tmpDir.mkdirs();
//        tmpDir.deleteOnExit();
        if (!tmpDir.exists()) {
            tmpDir.mkdirs();
            tmpDir.deleteOnExit();
        }
        //System.out.println("createTmpDir() tmpDir = " + tmpDir.getAbsolutePath());
    }

    /**
     * return a String which is the date of the jar build (or the current date ,
     * if the jar build date can't be found)
     */
    private static String getBuildDateInManifest() {
        Enumeration resEnum;
        String jarBuildDate = Long.toString(new Date().getTime()); //default value
        // the default value must be avoid because on windows this create a lot of temporary directories which are not destroy on exit(bug??)
        // so the solution is to create a temporary dir which depends on the building date of the jar (extracted from the manifest) 
        try {
            resEnum = Thread.currentThread().getContextClassLoader().getResources(JarFile.MANIFEST_NAME);
            // there is certainly more than one Manifest.mf file (the class loader get all the resources from all the jar used in the application)
            while (resEnum.hasMoreElements()) {
                URL url = (URL) resEnum.nextElement();
                //System.out.println("url: " + url.toString());
                //if (url.getPath().contains("jxip.jar!")) {
                InputStream is = url.openStream();
                if (is != null) {
                    Manifest manifest = new Manifest(is);
                    Attributes attributes = manifest.getMainAttributes();
                    if ("com.xerox.jxip.JXip".equals(attributes.getValue("Main-class"))) {
                        //it's the manifest file included in jxip.jar
                        jarBuildDate = attributes.getValue("Built-Date");
                        //System.out.println("jarBuildDate: " + jarBuildDate);
                        break;
                    }
                }
            }
        } catch (Exception ex) {
            System.out.println("ex: " + ex);
        }
        return jarBuildDate;
    }

    /**
     * delete the temporary dir
     */
    public static void deleteTmpDir() {
        //System.out.println("deleteTmpDir() tmpDir = " + tmpDir.getAbsolutePath());
        String[] children = tmpDir.list();
        for (String children1 : children) {
            //delete all the files in the tmpDir
            File file = new File(tmpDir, children1);
            file.deleteOnExit();
            file.delete();
        }
        tmpDir.deleteOnExit();
        tmpDir.delete();
    }

    /**
     * extract a list of native libraries(*.dll or *.so) from the jar file,
     * store each of them in a temporary directory, and call System.load for
     * each temporary file.
     *
     * @param jarOffset offset in the jar to found the dynamic lib
     * @param fileNames array of file names (dynamic library: *.dll or *.so)
     * @param jarFileName if null use the current jar, or the jar file name
     * where the dynamic lib are stored
     */
    private static void loadDynamicLibFromJar(String jarOffset, String[] fileNames, String jarFileName) {
//        System.out.println("loadDynamicLibFromJar() jarOffset = " + jarOffset);
        for (String fileName : fileNames) {
            String libPath = extractFileFromJar(jarOffset, fileName, jarFileName);
            try {
                //load the dymanic library
//                System.out.println("\n+++++++++++++++++loadDynamicLibFromJar System.load(" + libPath + ") \n");
                System.load(libPath);
            } catch (Exception ex) {
                System.out.println("Exception during loading: " + libPath);
                System.out.println("Exception: " + ex);
                throw new RuntimeException("loadDynamicLibFromJar:" + ex.toString());
            }
        }
    }

    /**
     * extract from the jar (jarFileName) the filename at the offset in jar
     * defined by jarOffset
     *
     * @param jarOffset
     * @param fileName
     * @param jarFileName if null, extract from the current jar
     * @return a temporary file (extracted file from the jar)
     */
    public static String extractFileFromJar(String jarOffset, String fileName, String jarFileName) {
        byte[] byteArray = new byte[1000000];
        int byteNb;
        BufferedOutputStream bufferedOutputStream;
        InputStream inputStream;
        String resourceName;
        if (jarOffset.equals("")) {
            resourceName = fileName;
        } else {
            resourceName = jarOffset + "/" + fileName;
        }
        //System.out.println("extractFileFromJar() resourceName = " + resourceName);
        String tempFile = tmpDir + File.separator + fileName;
        if (new File(tempFile).exists()) {
            // si le fichier exist deja ne rien faire
            return tempFile;
        } else {
            try {
                //System.out.println("extractFileFromJar() Extract lib: " + tempFile);
                bufferedOutputStream = new BufferedOutputStream(new FileOutputStream(tempFile));
                if (jarFileName == null) {
                    //System.out.println("extractFileFromJar() loading from jar");
                    //the ressources must be extract from the jar of the application
                    // use the class of the application: so we call this.getGlass() rather than getGlass()
                    inputStream = DynLoaderFromJar.class.getResourceAsStream("/" + resourceName);
                    if (inputStream == null) {
                        String errMes = "extractFileFromJar() \n";
                        errMes += "your classPath = " + System.getProperty("java.class.path") + "\n";
                        errMes += "Could not unpack: " + resourceName;
                        throw new RuntimeException(errMes);
                    }
                    while ((byteNb = inputStream.read(byteArray, 0, byteArray.length)) >= 0) {
                        bufferedOutputStream.write(byteArray, 0, byteNb);
                    }
                } else {
                    //the ressources must be extract from an other jar provided in parameter
                    //System.out.println("extractFileFromJar() loading from external jar: " + jarFileName);
                    JarFile jarFile = new JarFile(jarFileName);
                    inputStream = jarFile.getInputStream(jarFile.getEntry(resourceName));
                    while ((byteNb = inputStream.read(byteArray, 0, byteArray.length)) >= 0) {
                        bufferedOutputStream.write(byteArray, 0, byteNb);
                    }
                    jarFile.close();
                }
                bufferedOutputStream.close();
                inputStream.close();
                return tempFile;
            } catch (RuntimeException | IOException ex) {
                System.out.println("Exception: " + ex);
                throw new RuntimeException("extractFileFromJar:" + ex.toString());
            }
        }
    }

//    @Override
//    public void finalize() throws Throwable {
//        try {
//            //System.out.println("finalize ");
//            deleteTmpDir();
//        } finally {
//            super.finalize();
//        }
//    }
}
