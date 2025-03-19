package com.xerox.lg;

/**
 *  Description of the Class
 *
 * @author     grondeau
 * @created    October 12, 2005
 */
public class ConstLibDyn {
	// the libs to load for each supported platform
	// TAKE CARE ABOUT THE LIBRARY ORDER
	/**
	 *  Description of the Field
	 */
	public final static String[] libNamesWindows = {"languageGuesser.dll", "lg.dll"};
	/**
	 *  Description of the Field
	 */
	public final static String offsetWindows = "windows\\sharedLib";

	/**
	 *  Description of the Field
	 */
	public final static String[] libNamesLinux = {"liblanguageGuesser.so", "liblg.so"};
	/**
	 *  Description of the Field
	 */
	public final static String offsetLinux = "linux/sharedLib";

	/**
	 *  Description of the Field
	 */
	public final static String[] libNamesSolaris = {"liblanguageGuesser.so", "liblg.so"};
	/**
	 *  Description of the Field
	 */
	public final static String offsetSolaris = "solaris/sharedLib";
}

