/*
 *  Copyright (c) 1999,2005 Xerox Corporation,
 *  project XIP (Xerox Incremental Parser)
 *  Xerox Research Centre Europe
 *  All Rights Reserved
 *  Copyright protection claimed includes all forms and matters of copyrightable
 *  material and information now allowed by statutory or judicial law or
 *  hereinafter granted, including without limitation, material generated from
 *  the software programs which are displayed on the screen such as icons,
 *  screen display looks, etc. All rights reserved. No part of this document
 *  may be reproduced, stored in a retrieval system, or transmitted, in any
 *  form or by any means, electronic, mechanical, photocopied, recorded, or
 *  otherwise, without prior written consent from XEROX corporation.
 *
 */
package com.xerox.lg;

import java.io.*;
import java.util.*;

/**
 * The purpose of this class is to provide a Java implementation of the XIP library.
 *
 * @author     grondeau
 * @created    October 12, 2005
 */
public class DynLoaderFromJar {

	/**
	 *   extract from the jar file and load the native libary
	 *
	 * @exception  Exception  Description of the Exception
	 */
	public DynLoaderFromJar() throws Exception {
		unzipNativeLibraries();
		loadNativeLibraries();
	}


	/**
	 *  extract the files from the jar file
	 *
	 * @exception  Exception
	 */
	private void unzipNativeLibraries() throws Exception {
		//System.out.println("JXip::unzipNativeLibraries()");
		if ((System.getProperty("os.name").equals("Windows 2000")) ||
				(System.getProperty("os.name").equals("Windows 2003")) ||
				(System.getProperty("os.name").equals("Windows XP"))) {
			spoolFile(ConstLibDyn.offsetWindows.replace('\\', '/'), ConstLibDyn.offsetWindows, ConstLibDyn.libNamesWindows);
			return;
		}
		if (System.getProperty("os.name").equals("SunOS")) {
			spoolFile(ConstLibDyn.offsetSolaris, ConstLibDyn.offsetSolaris, ConstLibDyn.libNamesSolaris);
			return;
		}
		if (System.getProperty("os.name").equals("Linux")) {
			String libPath = getDirOffset(ConstLibDyn.offsetLinux) + ConstLibDyn.offsetLinux;
			System.out.println("*********************************************************************");
			System.out.println("Warning : You must add \"" + libPath + "\" to your LD_LIBRARY_PATH");
			System.out.println("*********************************************************************");
			spoolFile(ConstLibDyn.offsetLinux, ConstLibDyn.offsetLinux, ConstLibDyn.libNamesLinux);
			return;
		}
	}


	/**
	 *  create a temporary dir and return the path of this temporary dir
	 *
	 * @param  offSet
	 * @return                  path of a temporary dir
	 * @exception  IOException
	 */
	private String getDirOffset(String offSet) throws IOException {
		//System.out.println("JXip::getDirOffset() offSet = " + offSet);
		String tempDir = System.getProperty("java.io.tmpdir");
		if (!tempDir.endsWith(File.separator)) {
			tempDir += File.separator;
		}
		tempDir += System.getProperty("user.name") + File.separator;
		new File(tempDir + File.separator + offSet).mkdirs();
		//System.out.println("JXip::getDirOffset() tempDir = " + tempDir);
		return tempDir;
	}


	/**
	 *  extract a list of resources(native libraries) from the system ClassLoader and store each
	 *  native library in a temporary file. These resources are loaded from the jxip.jar using the classPath
	 *
	 * @param  jarOffset
	 * @param  offSet
	 * @param  fileNames      array of file names
	 * @exception  Exception  Description of the Exception
	 */
	private void spoolFile(String jarOffset, String offSet, String[] fileNames) throws Exception {
		String tempFile = null;
		InputStream inputStream = null;
		//System.out.println("JXip::spoolFile() jarOffset = " + jarOffset + " offSet = " + offSet + " fileNames = " + fileNames[0]);
		for (int i = 0; i < fileNames.length; i++) {
			//NOTE: do NOT replace "/" by File.separator: it does not work on windows only
			String resourceName = "/" + jarOffset + "/" + fileNames[i];
			//System.out.println("JXip::spoolFile() resourceName = " + resourceName);
			//Enumeration en = ClassLoader.getSystemResources(resourceName);
			//while (en.hasMoreElements()) {
			//System.out.println("JXip::spoolFile() resources: " + (en.nextElement()).toString());
			//}
			// use the class of the application: so we call this.getGlass() rather than getGlass()
			inputStream = this.getClass().getResourceAsStream(resourceName);
			if (inputStream == null) {
				String errMes = "JXip::spoolFile() update your classPath to access jxip.jar\n";
				errMes += "your classPath = " + System.getProperty("java.class.path") + "\n";
				errMes += "Could not unpack: " + resourceName;
				throw new Exception(errMes);
			}
			tempFile = getDirOffset(offSet) + offSet + File.separator + fileNames[i];
			//System.out.println("JXip::spoolFile() Spooling lib: " + tempFile);
			BufferedOutputStream os = new BufferedOutputStream(new FileOutputStream(tempFile));
			int bytesRead;
			byte[] byteArray = new byte[100000];
			while ((bytesRead = inputStream.read(byteArray, 0, byteArray.length)) >= 0) {
				os.write(byteArray, 0, bytesRead);
			}
			os.close();
			inputStream.close();
		}
	}


	/**
	 * load the native libraries for XIP based on the current operating system
	 * and hardware being used.
	 *
	 * @exception  Exception  Description of the Exception
	 */
	private void loadNativeLibraries() throws Exception {
		//System.out.println("JXip::loadNativeLibraries()");
		if ((System.getProperty("os.name").equals("Windows 2000")) ||
				(System.getProperty("os.name").equals("Windows 2003")) ||
				(System.getProperty("os.name").equals("Windows XP"))) {
			loadLibraries(ConstLibDyn.libNamesWindows, ConstLibDyn.offsetWindows);
			return;
		}
		if (System.getProperty("os.name").equals("SunOS")) {
			loadLibraries(ConstLibDyn.libNamesSolaris, ConstLibDyn.offsetSolaris);
			return;
		}
		if (System.getProperty("os.name").equals("Linux")) {
			loadLibraries(ConstLibDyn.libNamesLinux, ConstLibDyn.offsetLinux);
			return;
		}
	}


	/**
	 * Loads files as dynamic libraries
	 *
	 * @param  libNames       array of library name
	 * @param  offSet         platform name
	 * @exception  Exception  Description of the Exception
	 */
	private synchronized void loadLibraries(String[] libNames, String offSet) throws Exception {
		//System.out.println("JXip::loadLibraries() libNames = " + libNames[0] + " offSet = " + offSet);
		String tempLibFile = null;
		String libPath = getDirOffset(offSet) + offSet;
		//System.out.println("JXip::loadLibraries() libPath = " + libPath);
		System.setProperty("java.library.path", System.getProperty("java.library.path") + File.pathSeparator + libPath);
		//System.out.println("JXip::loadLibraries()  java.library.path = " + System.getProperty("java.library.path"));
		for (int i = 0; i < libNames.length; i++) {
			// a full library path name is required
			tempLibFile = convertToCanonicalFilename(libPath + File.separator + libNames[i]);
			//System.out.println("JXip::loadLibraries() Loading native lib " + tempLibFile);
			//Loads a code file with the specified filename from the local file system as a dynamic library.
			try {
				System.load(tempLibFile);
			} catch (java.lang.UnsatisfiedLinkError ex) {
				throw ex;
			}
		}
	}


	/**
	 * @param  fileName       Description of the Parameter
	 * @return                Description of the Return Value
	 * @exception  Exception  Description of the Exception
	 */
	private synchronized String convertToCanonicalFilename(String fileName) throws Exception {
		return new File(fileName).getCanonicalPath();
	}
}

