/*
 *  Copyright (c) 1999,2004 Xerox Corporation,
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
 *  Description of the Class
 *
 * @author     grondeau
 * @created    October 12, 2005
 */
public class LanguageGuesser {
	/**
	 *Constructor for the LanguageGuesser object
	 *
	 * @exception  Exception  Description of the Exception
	 */
	public LanguageGuesser() throws Exception {
		new DynLoaderFromJar();
	}


	//-----------------------------------------------------------------------------------------------------
	/**
	 * Get the value of a native environment variable
	 *
	 * @return    The enviromnent variable value
	 */
	private native int initGuesserImplementation();


	/**
	 *  Get the value of a native environment variable
	 *
	 * @return                The enviromnent variable value
	 * @exception  Exception  Description of the Exception
	 */
	public synchronized int initGuesser() throws Exception {
		//System.out.println("LanguageGuesser::initGuesser()");
		return initGuesserImplementation();
	}


//-----------------------------------------------------------------------------------------------------
	/**
	 * Get the value of a native environment variable
	 *
	 * @param  input       Description of the Parameter
	 * @param  langueType  iso639, iso639_2 or full
	 * @param  guessType   trig, shortword or trig_shortword
	 * @return             The enviromnent variable value
	 */
	private native String guessLanguageImplementation(String input, String langueType, String guessType);


	/**
	 *  Determines the language of the given string
	 *
	 * @param  input          the string to guess
	 * @param  langueType     iso639, iso639_2 or full
	 * @param  guessType      trig, shortword or trig_shortword
	 * @return                Depending on langueType this will be a two-letter code (iso639), a three-letter code (iso639-2) or the full name of the language.
	 * @exception  Exception  Description of the Exception
	 */
	public synchronized String guessLanguage(String input, String langueType, String guessType) throws Exception {
		//System.out.println("LanguageGuesser::guessLanguage() input = " + input + " langueType = " + langueType + " guessType = " + guessType);
		String langue = guessLanguageImplementation(input, langueType, guessType);
		return langue;
	}


	//-----------------------------------------------------------------------------------------------------

	/**
	 *  Description of the Method
	 *
	 * @param  args  Description of the Parameter
	 */
	public final static void main(String args[]) {
		try {
			LanguageGuesser lg = new LanguageGuesser();
			int cnt = lg.initGuesser();
			System.out.println("LanguageGuessser::main():  " + cnt);
			String langue;
			langue = lg.guessLanguage("This is a test of english text", "iso639", "trig");
			System.out.println("LanguageGuessser::main():  langue = " + langue + "\n");

			cnt = lg.initGuesser();
			langue = lg.guessLanguage("Ma guardiamo le liste che sono state presentate a Roma", "iso639_2", "shortword");
			System.out.println("LanguageGuessser::main():  langue = " + langue + "\n");

			cnt = lg.initGuesser();
			langue = lg.guessLanguage("Dikwijls heeft dit praktische redenen", "full", "trig_shortword");
			System.out.println("LanguageGuessser::main():  langue = " + langue + "\n");

			cnt = lg.initGuesser();
			langue = lg.guessLanguage("le bateau remonte la riviere", "full", "trig_shortword");
			System.out.println("LanguageGuessser::main():  langue = " + langue + "\n");
		} catch (Exception ex) {
			ex.printStackTrace();
			System.exit(-1);
		}
	}
}

